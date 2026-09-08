#pragma once

#include "HAL/Platform.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace MetasoundExtras {
	
	class FWrapOperator : public Metasound::TExecutableOperator<FWrapOperator> {
	public:
		FWrapOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudio);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;

		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();

	private:
		// Inputs
		Metasound::FAudioBufferReadRef Input;
		
		// Outputs
		Metasound::FAudioBufferWriteRef Output;
		
	};

	using FWrapNode = Metasound::TNodeFacade<FWrapOperator>;
}
