#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	class FDivideSigOperator : public Metasound::TExecutableOperator<FDivideSigOperator> {
	public:
		FDivideSigOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InDividend, const Metasound::FAudioBufferReadRef& InDivsor);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;

		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();

	private:
		// Inputs
		Metasound::FAudioBufferReadRef Dividend;
		Metasound::FAudioBufferReadRef Divisor;
		
		// Outputs
		Metasound::FAudioBufferWriteRef Output;
		
	};

	using FDivideSigNode = Metasound::TNodeFacade<FDivideSigOperator>;
}
