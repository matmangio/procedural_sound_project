#include "Nodes/LineSigNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_LineNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace LineSigNode {
		// Input params
		METASOUND_PARAM(Target, "Target", "The target to reach, either 0 or 1.");
		METASOUND_PARAM(RampTime, "Time", "How much time to reach the target, in seconds.");
		
		// Output params
		METASOUND_PARAM(Output, "Out", "The value of the line.");
	}

	// Constructor
	FLineSigOperator::FLineSigOperator(const FOperatorSettings& InSettings, const FInt32ReadRef& InTarget, const FFloatReadRef& InRampTime) 
		: Target(InTarget), RampTime(InRampTime), Output(FAudioBufferWriteRef::CreateNew(InSettings)), SampleRate(InSettings.GetSampleRate()) {}

	const FNodeClassMetadata& FLineSigOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("Line (with target)"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_LineSigDisplayName", "Line (with target)");
			Info.Description       = LOCTEXT("MetasoundExtras_LineSigNodeDescription", "Ramp generator that goes to 0 or 1 in the specified time and keeps that value.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_LineSigNodeCategory", "Generators") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FLineSigOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace LineSigNode;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Target), Target);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(RampTime), RampTime);
	}

	void FLineSigOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace LineSigNode;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Output), Output);
	}

	const FVertexInterface& FLineSigOperator::GetVertexInterface() {
		using namespace LineSigNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<int32>(METASOUND_GET_PARAM_NAME_AND_METADATA(Target), 1),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(RampTime), 1.0f)
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Output))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FLineSigOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace LineSigNode;
		
		FInt32ReadRef TargetRef = InParams.InputData.GetOrCreateDefaultDataReadReference<int32>(
			METASOUND_GET_PARAM_NAME(Target), InParams.OperatorSettings);
		FFloatReadRef RampTimeRef = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(
			METASOUND_GET_PARAM_NAME(RampTime), InParams.OperatorSettings);

		return MakeUnique<FLineSigOperator>(InParams.OperatorSettings, TargetRef, RampTimeRef);
	}

	void FLineSigOperator::Execute() {
		float* Out = Output->GetData();
		int32 NumSamples = Output->Num();
		
		float increment = ((*Target == 1)? 1.0f : -1.0f)/ (*RampTime * SampleRate);
		
		for (int i = 0; i < NumSamples; i++) {
			Out[i] = CurrentValue;
			CurrentValue = FMath::Clamp(CurrentValue + increment, 0, 1);
		}
	}

	METASOUND_REGISTER_NODE(FLineSigNode)
}

#undef LOCTEXT_NAMESPACE
