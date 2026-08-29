#include "Nodes/CosNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_CosNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace CosNode {
		// Input params
		METASOUND_PARAM(Input, "In", "The value in [0,1] where to read the cosine.");
		
		// Output params
		METASOUND_PARAM(Output, "Out", "The value of the cosine in In * 2 * PI.");
	}

	// Constructor
	FCosOperator::FCosOperator(const FOperatorSettings& InSettings, const FFloatReadRef& InputValue)
		: Input(InputValue), Output(FFloatWriteRef::CreateNew()) {}

	const FNodeClassMetadata& FCosOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("Cos"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_CosDisplayName", "Cos");
			Info.Description       = LOCTEXT("MetasoundExtras_CosNodeDescription", "A cosine wavetable reader. The input is a linear index from 0 to 1 and you can also consider it a cosine trigonometric function of the input multiplied by 2 * PI. Values outside the 0-1 range get wrapped inside this range.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_CosNodeCategory", "Extras") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FCosOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace CosNode;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Input), Input);
	}

	void FCosOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace CosNode;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Output), Output);
	}

	const FVertexInterface& FCosOperator::GetVertexInterface() {
		using namespace CosNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(Input), 1.0f)
			),

			FOutputVertexInterface(
				TOutputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(Output))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FCosOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace CosNode;
		
		FFloatReadRef InputRef = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(
			METASOUND_GET_PARAM_NAME(Input), InParams.OperatorSettings);

		return MakeUnique<FCosOperator>(InParams.OperatorSettings, InputRef);
	}

	void FCosOperator::Execute() {
		float clampedValue = (*Input < 0.0f)? 0.0f : (*Input > 1.0f)? 1.0f : *Input;
		*Output = cos(clampedValue * 2 * UE_PI);
	}

	METASOUND_REGISTER_NODE(FCosNode)
}

#undef LOCTEXT_NAMESPACE

