#include "Nodes/DivideSigNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_DivideSigNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace DivideSigNode {
		// Input params
		METASOUND_PARAM(Dividend, "A", "Dividend.");
		METASOUND_PARAM(Divisor, "B", "Divisor.");
		
		// Output params
		METASOUND_PARAM(Output, "Out", "A / B.");
	}

	// Constructor
	FDivideSigOperator::FDivideSigOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InDividend, const FAudioBufferReadRef& InDivisor)
		: Dividend(InDividend), Divisor(InDivisor), Output(FAudioBufferWriteRef::CreateNew(InSettings)) {}

	const FNodeClassMetadata& FDivideSigOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("Divide (Audio)"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_DivideSigDisplayName", "Divide (Audio)");
			Info.Description       = LOCTEXT("MetasoundExtras_DivideSigNodeDescription", "Outputs the division of two audio streams.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_DivideSigNodeCategory", "Math") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FDivideSigOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace DivideSigNode;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Dividend), Dividend);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Divisor), Divisor);
	}

	void FDivideSigOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace DivideSigNode;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Output), Output);
	}

	const FVertexInterface& FDivideSigOperator::GetVertexInterface() {
		using namespace DivideSigNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Dividend)),
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Divisor))
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Output))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FDivideSigOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace DivideSigNode;
		
		FAudioBufferReadRef DividendRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(Dividend), InParams.OperatorSettings);
		FAudioBufferReadRef DivisorRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(Divisor), InParams.OperatorSettings);

		return MakeUnique<FDivideSigOperator>(InParams.OperatorSettings, DividendRef, DivisorRef);
	}

	void FDivideSigOperator::Execute() {
		const float* dividend = Dividend->GetData();
		const float* divisor = Divisor->GetData();
		float* out = Output->GetData();
		
		int32 NumSamples = Dividend->Num();
		
		for (int i = 0; i < NumSamples; i++) {
			out[i] = dividend[i] / divisor[i];
		}
	}

	METASOUND_REGISTER_NODE(FDivideSigNode)
}

#undef LOCTEXT_NAMESPACE

