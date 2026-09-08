#include "Nodes/WrapNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_WrapNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace WrapNode {
		// Input params
		METASOUND_PARAM(Input, "In", "Input audio.");
		
		// Output params
		METASOUND_PARAM(Output, "Out", "Modulo 1 of the audio.");
	}

	// Constructor
	FWrapOperator::FWrapOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudio)
		: Input(InAudio), Output(FAudioBufferWriteRef::CreateNew(InSettings)) {}

	const FNodeClassMetadata& FWrapOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("Wrap"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_WrapDisplayName", "Wrap");
			Info.Description       = LOCTEXT("MetasoundExtras_WrapNodeDescription", "Gives the difference between the input and the largest integer not exceeding it (for positive numbers this is the fractional part).");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_WrapNodeCategory", "Math") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FWrapOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace WrapNode;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Input), Input);
	}

	void FWrapOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace WrapNode;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Output), Output);
	}

	const FVertexInterface& FWrapOperator::GetVertexInterface() {
		using namespace WrapNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Input))
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Output))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FWrapOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace WrapNode;
		
		FAudioBufferReadRef InputRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(Input), InParams.OperatorSettings);

		return MakeUnique<FWrapOperator>(InParams.OperatorSettings, InputRef);
	}

	void FWrapOperator::Execute() {
		const float* in = Input->GetData();
		float* out = Output->GetData();
		
		int32 NumSamples = Input->Num();
		
		for (int i = 0; i < NumSamples; i++) {
			out[i] = in[i] - FMath::Floor(in[i]);
		}
	}

	METASOUND_REGISTER_NODE(FWrapNode)
}

#undef LOCTEXT_NAMESPACE

