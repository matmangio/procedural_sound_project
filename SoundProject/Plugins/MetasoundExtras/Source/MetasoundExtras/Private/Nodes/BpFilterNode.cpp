#include "Nodes/BpFilterNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_BpFilterNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace BpFilter {
		// Input params
		METASOUND_PARAM(AudioIn, "In", "The audio signal to be filtered.");
		METASOUND_PARAM(Frequency, "Frequency", "The filter's center frequency.");
		METASOUND_PARAM(Q, "Q", "The filter's bandwidth.");
		
		// Output params
		METASOUND_PARAM(AudioOut, "Out", "The filtered signal output.");
	}

	// Constructor
	BpFilterOperator::BpFilterOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudio, const FFloatReadRef& InFrequency, const FFloatReadRef& InQ)
		: AudioIn(InAudio),
		  Frequency(InFrequency),
	      Q(InQ),
		  AudioOut(FAudioBufferWriteRef::CreateNew(InSettings)),
		  SampleRate(InSettings.GetSampleRate()) {}

	const FNodeClassMetadata& BpFilterOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("BpFilter"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_BpFilterDisplayName", "BpFilter");
			Info.Description       = LOCTEXT("MetasoundExtras_BpFilterNodeDescription", "A 2-pole bandpass filter.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_BpFilterNodeCategory", "Extras") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void BpFilterOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace BpFilter;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(AudioIn), AudioIn);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Frequency), Frequency);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Q), Q);
	}

	void BpFilterOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace BpFilter;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(AudioOut), AudioOut);
	}

	const FVertexInterface& BpFilterOperator::GetVertexInterface() {
		using namespace BpFilter;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(AudioIn)),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(Frequency), 1.0f),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(Q), 1.0f)
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(AudioOut))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> BpFilterOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace BpFilter;
		
		FAudioBufferReadRef AudioInRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(AudioIn), InParams.OperatorSettings);
		FFloatReadRef FrequencyRef = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(
			METASOUND_GET_PARAM_NAME(Frequency), InParams.OperatorSettings);
		FFloatReadRef QRef = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(
			METASOUND_GET_PARAM_NAME(Q), InParams.OperatorSettings);

		return MakeUnique<BpFilterOperator>(InParams.OperatorSettings, AudioInRef, FrequencyRef, QRef);
	}

	void BpFilterOperator::Execute() {
		const float* In = AudioIn->GetData();
		float* Out = AudioOut->GetData();
		float f = *Frequency;
		float q = *Q;
		
		const int32 NumSamples = AudioIn->Num();
		
		// Clamping of parameters
		if (f < 0.001f) {
			f = 10.0f;
		}
		if (q < 0.0f) {
			q = 0;
		}
		
		// Coefficients computations
		float omega = f * 2 * PI / SampleRate;
		float oneminusr = (q < 0.001f)? 1.0f : omega / q;
		oneminusr = (oneminusr > 1.0f)? 1.0f : oneminusr;
		float r = 1 - oneminusr;
		
		float coeff1 = 2 * cos(omega) * r;
		float coeff2 = -(r * r);
		float gain = 2 * oneminusr * (oneminusr + r * omega);
		
		// Compute output samples
		for (int32 i = 0; i < NumSamples; i++) {
			float tmp = In[i] + coeff1 * S1 + coeff2 * S2;
			Out[i] = gain * tmp;
			
			// Save state
			S2 = S1;
			S1 = tmp;
		}
	}

	void BpFilterOperator::Reset(const IOperator::FResetParams& InParams) {
		AudioOut->Zero();
		S1 = 0.0f;
		S2 = 0.0f;
	}

	METASOUND_REGISTER_NODE(FBpFilterNode)
}

#undef LOCTEXT_NAMESPACE


