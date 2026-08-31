#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	// Computes cosine of a value in [0, 1]
	class BpFilterOperator : public Metasound::TExecutableOperator<BpFilterOperator> {
	public:
		BpFilterOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudio, const Metasound::FFloatReadRef& InFrequency, const Metasound::FFloatReadRef& InQ);

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
		Metasound::FFloatReadRef Frequency;
		Metasound::FFloatReadRef Q;
		
		// Outputs
		Metasound::FAudioBufferWriteRef AudioOut;
		
		// Utilities
		float SampleRate = 48000.0f;
		
		// Persistent state
		float S1 = 0.0f, S2 = 0.0f;
	};

	using FBpFilterNode = Metasound::TNodeFacade<BpFilterOperator>;
}

