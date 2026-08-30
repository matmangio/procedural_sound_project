#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	// Computes cosine of a value in [0, 1]
	class FVcfFilterOperator : public Metasound::TExecutableOperator<FVcfFilterOperator> {
	public:
		FVcfFilterOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudio, const Metasound::FAudioBufferReadRef& InFrequency, const Metasound::FFloatReadRef& InQ);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;

		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();
		
		void Reset(const IOperator::FResetParams& InParams);

	private:

		// Inputs
		Metasound::FAudioBufferReadRef AudioIn;
		Metasound::FAudioBufferReadRef Frequency;
		Metasound::FFloatReadRef Q;
		
		// Outputs
		Metasound::FAudioBufferWriteRef ReAudioOut;
		Metasound::FAudioBufferWriteRef ImAudioOut;
		
		// Utilities
		float SampleRate = 48000.0f;
		
		// Persistent state
		float Re = 0.0f;
		float Im = 0.0f;
	};

	using FVcfFilterNode = Metasound::TNodeFacade<FVcfFilterOperator>;
}
