#include "Nodes/CosSigNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_CosSigNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace CosSigNode {
		// Input params
		METASOUND_PARAM(Input, "In", "Input audio, wrapped inside [0, 1].");
		
		// Output params
		METASOUND_PARAM(Output, "Out", "Cosine of 2 * PI * the input audio.");
	}

	// Constructor
	FCosSigOperator::FCosSigOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudio)
		: Input(InAudio), Output(FAudioBufferWriteRef::CreateNew(InSettings)) {}

	const FNodeClassMetadata& FCosSigOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("Cos (Audio)"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_CosSigDisplayName", "Cos (Audio)");
			Info.Description       = LOCTEXT("MetasoundExtras_CosSigNodeDescription", "Outputs the cosine of 2 * PI * its signal input.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_CosSigNodeCategory", "Math") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FCosSigOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace CosSigNode;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Input), Input);
	}

	void FCosSigOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace CosSigNode;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Output), Output);
	}

	const FVertexInterface& FCosSigOperator::GetVertexInterface() {
		using namespace CosSigNode;

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

	TUniquePtr<IOperator> FCosSigOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace CosSigNode;
		
		FAudioBufferReadRef InputRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(Input), InParams.OperatorSettings);

		return MakeUnique<FCosSigOperator>(InParams.OperatorSettings, InputRef);
	}

	void FCosSigOperator::Execute() {
		const float* in = Input->GetData();
		float* out = Output->GetData();
		
		int32 NumSamples = Input->Num();
		
		for (int i = 0; i < NumSamples; i++) {
			float wrappedValue = fmod(1.0f + fmod(in[i], 1.0f), 1.0f);
			out[i] = cos(wrappedValue * 2 * PI);
		}
	}

	METASOUND_REGISTER_NODE(FCosSigNode)
}

#undef LOCTEXT_NAMESPACE

