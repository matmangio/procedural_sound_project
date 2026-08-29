#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	// Computes cosine of a value in [0, 1]
	class FCosOperator : public Metasound::TExecutableOperator<FCosOperator> {
	public:
		FCosOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FFloatReadRef& InputValue);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;

		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();

	private:

		// Inputs
		Metasound::FFloatReadRef Input;
		
		// Outputs
		Metasound::FFloatWriteRef Output;
	};

	using FCosNode = Metasound::TNodeFacade<FCosOperator>;
}