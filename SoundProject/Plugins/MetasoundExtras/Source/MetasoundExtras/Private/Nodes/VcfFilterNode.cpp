#include "Nodes/VcfFilterNode.h"

#define LOCTEXT_NAMESPACE "MetasoundExtras_VcfFilterNode"

namespace MetasoundExtras {
	
	using namespace Metasound;

	namespace VcfFilter {
		// Input params
		METASOUND_PARAM(AudioIn, "In", "The audio signal to be filtered.");
		METASOUND_PARAM(Frequency, "Frequency", "The filter's resonant frequency.");
		METASOUND_PARAM(Q, "Q", "The filter's bandwidth.")
		
		// Output params
		METASOUND_PARAM(ReAudioOut, "Re", "Real output (bandpass filtered signal).");
		METASOUND_PARAM(ImAudioOut, "Im", "Imaginary output (lowpass filtered signal).");
	}

	// Constructor
	FVcfFilterOperator::FVcfFilterOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudio, const FAudioBufferReadRef& InFrequency, const FFloatReadRef& InQ)
		: AudioIn(InAudio),
		  Frequency(InFrequency),
	      Q(InQ),
		  ReAudioOut(FAudioBufferWriteRef::CreateNew(InSettings)),
		  ImAudioOut(FAudioBufferWriteRef::CreateNew(InSettings)),
		  SampleRate(InSettings.GetSampleRate()) {}

	const FNodeClassMetadata& FVcfFilterOperator::GetNodeInfo() {
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("UE"), TEXT("VcfFilter"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 0;
			Info.DisplayName       = LOCTEXT("MetasoundExtras_VcfFilterDisplayName", "Vcf Filter");
			Info.Description       = LOCTEXT("MetasoundExtras_VcfFilterNodeDescription", "A resonant bandpass filter that takes an audio signal as center frequency, which may thus change continuously in time as in an analog voltage controlled filter.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("MetasoundExtras_VcfFilterNodeCategory", "Filters") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FVcfFilterOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData) {
		using namespace VcfFilter;
		
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(AudioIn), AudioIn);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Frequency), Frequency);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Q), Q);
	}

	void FVcfFilterOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData) {
		using namespace VcfFilter;

		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(ReAudioOut), ReAudioOut);
		InOutVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(ImAudioOut), ImAudioOut);
	}

	const FVertexInterface& FVcfFilterOperator::GetVertexInterface() {
		using namespace VcfFilter;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(AudioIn)),
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(Frequency)),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(Q), 1.0f)
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(ReAudioOut)),
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(ImAudioOut))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FVcfFilterOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults) {
		using namespace VcfFilter;
		
		FAudioBufferReadRef AudioInRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(AudioIn), InParams.OperatorSettings);
		FAudioBufferReadRef FrequencyRef = InParams.InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(
			METASOUND_GET_PARAM_NAME(Frequency), InParams.OperatorSettings);
		FFloatReadRef QRef = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(
			METASOUND_GET_PARAM_NAME(Q), InParams.OperatorSettings);

		return MakeUnique<FVcfFilterOperator>(InParams.OperatorSettings, AudioInRef, FrequencyRef, QRef);
	}

	void FVcfFilterOperator::Execute() {
		const float* In = AudioIn->GetData();
		const float* Freq = Frequency->GetData();
		float* ReOut = ReAudioOut->GetData();
		float* ImOut = ImAudioOut->GetData();
		
		const int32 NumSamples = AudioIn->Num();
		
		// Preliminary computations
		const float freq_scale = 2 * PI / SampleRate;
		const float q_inv = (*Q > 0.0f)? (1.0f / *Q) : 0.0f;
		const float amp_correct = 2.0f - 2.0f / (*Q + 2.0f);
		
		for (int32 i = 0; i < NumSamples; i++) {
			// Compute frequency in radians/sample
			float cf = Freq[i] * freq_scale;
			if (cf < 0.0f) {
				cf = 0.0f;
			}
			
			// Compute r
			float r = (q_inv > 0.0f)? (1.0f - cf * q_inv) : 0.0f;
			if (r < 0.0f) {
				r = 0.0f;
			}
			
			// Compute cos and sin of cf
			const float cosCF = cos(cf);
			const float sinCF = sin(cf);
			
			// Update state
			const float tempRe = Re;
			
			Re = amp_correct * (1.0f - r) * In[i] + r * cosCF * tempRe - r * sinCF * Im;
			Im = r * sinCF * tempRe + r * cosCF * Im;
			
			// Set output
			ReOut[i] = Re;
			ImOut[i] = Im;
		}
	}

	void FVcfFilterOperator::Reset(const IOperator::FResetParams& InParams) {
		ReAudioOut->Zero();
		ImAudioOut->Zero();
		Re = 0.0f;
		Im = 0.0f;
	}

	METASOUND_REGISTER_NODE(FVcfFilterNode)
}

#undef LOCTEXT_NAMESPACE

