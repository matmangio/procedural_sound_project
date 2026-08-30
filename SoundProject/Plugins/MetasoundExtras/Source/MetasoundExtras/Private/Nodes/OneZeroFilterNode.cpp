#include "Nodes/OneZeroFilterNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_ZeroOneFilterNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace OneZeroFilter {
		// Input params
		METASOUND_PARAM(In, "In", "The audio input buffer.");
		METASOUND_PARAM(Coefficient, "Coefficient", "The filter coefficient.");
		
		// Output params
		METASOUND_PARAM(Out, "Out", "The audio output buffer.");
	}

	// Constructor
	FOneZeroFilterOperator::FOneZeroFilterOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudio, const FFloatReadRef& InCoefficient)
		: In(InAudio), Coefficient(InCoefficient), Out(FAudioBufferWriteRef::CreateNew(InSettings)) {}

	const FNodeClassMetadata& FOneZeroFilterOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("OneZeroFilter"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_OneZeroDisplayName", "OneZeroFilter");
			Info.Description       = LOCTEXT("MetasoundExtras_OneZeroNodeDescription", "Raw one-zero non-recursive real filter.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_OneZeroNodeCategory", "Extras") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FOneZeroFilterOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace OneZeroFilter;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(In), In);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Coefficient), Coefficient);
	}

	void FOneZeroFilterOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace OneZeroFilter;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Out), Out);
	}

	const FVertexInterface& FOneZeroFilterOperator::GetVertexInterface() {
		using namespace OneZeroFilter;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(In)),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(Coefficient), 1.0f)
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Out))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FOneZeroFilterOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace OneZeroFilter;
		
		FAudioBufferReadRef InRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(In), InParams.OperatorSettings);
		FFloatReadRef CoefficientRef = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(
			METASOUND_GET_PARAM_NAME(Coefficient), InParams.OperatorSettings);

		return MakeUnique<FOneZeroFilterOperator>(InParams.OperatorSettings, InRef, CoefficientRef);
	}

	void FOneZeroFilterOperator::Execute() {
		const float* InputAudio = In->GetData();
		float* OutputAudio = Out->GetData();
		
		const int32 NumSamples = In->Num();
		
		for (int32 i = 0; i < NumSamples; i++) {
			OutputAudio[i] = InputAudio[i];
			if (i > 0) {
				OutputAudio[i] -= *Coefficient * InputAudio[i-1];
			}
		}
	}

	void FOneZeroFilterOperator::Reset(const IOperator::FResetParams& InParams) {
		Out->Zero();
	}

	METASOUND_REGISTER_NODE(FZeroOneFilterNode)
}

#undef LOCTEXT_NAMESPACE
