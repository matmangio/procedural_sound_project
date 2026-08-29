#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	// Computes cosine of a value in [0, 1]
	class FOneZeroFilterOperator : public Metasound::TExecutableOperator<FOneZeroFilterOperator> {
	public:
		FOneZeroFilterOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudio, const Metasound::FFloatReadRef& InCoefficient);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;

		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();
		
		void Reset(const IOperator::FResetParams& InParams);

	private:

		// Inputs
		Metasound::FAudioBufferReadRef In;
		Metasound::FFloatReadRef Coefficient;
		
		// Outputs
		Metasound::FAudioBufferWriteRef Out;
		
	};

	using FZeroOneFilterNode = Metasound::TNodeFacade<FOneZeroFilterOperator>;
}