#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	// Computes cosine of a value in [0, 1]
	class FLineOperator : public Metasound::TExecutableOperator<FLineOperator> {
	public:
		FLineOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FTriggerReadRef& InTrigger, const Metasound::FFloatReadRef& InDecayTime);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;

		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();

	private:

		// Inputs
		Metasound::FTriggerReadRef Trigger;
		Metasound::FFloatReadRef DecayTime;
		
		// Outputs
		Metasound::FAudioBufferWriteRef Output;
		
		// Utilities
		float SampleRate = 48000.0f;
		
		// Persistent state
		float CurrentValue = 0.0f;
	};

	using FLineNode = Metasound::TNodeFacade<FLineOperator>;
}