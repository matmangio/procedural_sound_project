#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	class FLineSigOperator : public Metasound::TExecutableOperator<FLineSigOperator> {
	public:
		FLineSigOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FInt32ReadRef& InTarget, const Metasound::FFloatReadRef& InRampTime);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;

		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();

	private:

		// Inputs
		Metasound::FInt32ReadRef Target;
		Metasound::FFloatReadRef RampTime;
		
		// Outputs
		Metasound::FAudioBufferWriteRef Output;
		
		// Utilities
		float SampleRate = 48000.0f;
		
		// Persistent state
		float CurrentValue = 0.0f;
	};

	using FLineSigNode = Metasound::TNodeFacade<FLineSigOperator>;
}