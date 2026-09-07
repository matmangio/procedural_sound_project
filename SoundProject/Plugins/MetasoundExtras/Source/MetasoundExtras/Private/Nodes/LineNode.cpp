#include "Nodes/LineNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_LineNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace LineNode {
		// Input params
		METASOUND_PARAM(Trigger, "Trigger", "When triggered, generates the line output.");
		METASOUND_PARAM(DecayTime, "Decay Time", "The decay time for the line, in seconds.");
		
		// Output params
		METASOUND_PARAM(Output, "Out", "The value of the line.");
	}

	// Constructor
	FLineOperator::FLineOperator(const FOperatorSettings& InSettings, const FTriggerReadRef& InTrigger, const FFloatReadRef& InDecayTime) 
		: Trigger(InTrigger), DecayTime(InDecayTime), Output(FAudioBufferWriteRef::CreateNew(InSettings)), SampleRate(InSettings.GetSampleRate()) {}

	const FNodeClassMetadata& FLineOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("Line"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_LineDisplayName", "Line");
			Info.Description       = LOCTEXT("MetasoundExtras_LineNodeDescription", "Ramp generator that goes to 1 on trigger and decays after.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_LineNodeCategory", "Generators") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FLineOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace LineNode;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Trigger), Trigger);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(DecayTime), DecayTime);
	}

	void FLineOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace LineNode;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Output), Output);
	}

	const FVertexInterface& FLineOperator::GetVertexInterface() {
		using namespace LineNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(Trigger)),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(DecayTime), 1.0f)
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Output))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FLineOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace LineNode;
		
		FTriggerReadRef TriggerRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FTrigger>(
			METASOUND_GET_PARAM_NAME(Trigger), InParams.OperatorSettings);
		FFloatReadRef DecayTimeRef = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(
			METASOUND_GET_PARAM_NAME(DecayTime), InParams.OperatorSettings);

		return MakeUnique<FLineOperator>(InParams.OperatorSettings, TriggerRef, DecayTimeRef);
	}

	void FLineOperator::Execute() {
		float* Out = Output->GetData();
		float decay = FMath::Min(*DecayTime, 0.0001f);
		
		// Esegue la prima funzione sui sample pre-trigger, la seconda su quelli post-trigger
		Trigger->ExecuteBlock(
			// Pre: continua il decay
			[&](const int32 start_sample, const int32 end_sample) {
				for (int i = start_sample; i < end_sample; i++) {
					Out[i] = CurrentValue;
					if (CurrentValue > 0.0f) {
						CurrentValue = FMath::Max(CurrentValue - 1.0f / (decay * SampleRate), 0.0f);
					}
				}
			},
			// Post: reset a 1
			[&](int32 start_sample, int32 end_sample) {
				CurrentValue = 1.0f;
				
				for (int i = start_sample; i < end_sample; i++) {
					Out[i] = CurrentValue;
					if (CurrentValue > 0.0f) {
						CurrentValue = FMath::Max(CurrentValue - 1.0f / (decay * SampleRate), 0.0f);
					}
				}
			}
		);
	}

	METASOUND_REGISTER_NODE(FLineNode)
}

#undef LOCTEXT_NAMESPACE

