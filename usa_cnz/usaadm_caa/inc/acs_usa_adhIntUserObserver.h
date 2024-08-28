#ifndef ACS_USA_AXEADDITIONALINFO_OBSERVER_H_
#define ACS_USA_AXEADDITIONALINFO_OBSERVER_H_

#include <iostream>
#include <unistd.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include <ace/Task.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "acs_usa_imm.h"

class ACS_USA_AdhIntUserObserver: public acs_apgcc_objectimplementereventhandler_V3, public ACE_Task_Base {

	public:
		enum OperationType
		{
			CREATE_OPERATION = 0,
			MODIFY_OPERATION_USER_SET,
			MODIFY_OPERATION_STATE_SET,
			DELETE_OPERATION,
			NONE
		};
		
		enum StateType
		{
			DISABLE = 0,
			ENABLE,
			NOSTATE
		};
		
		ACS_USA_AdhIntUserObserver( std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope);
		virtual ~ACS_USA_AdhIntUserObserver();
		virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
		virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
		virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
		virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
		virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
		virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
		virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);
		virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,ACS_APGCC_AdminOperationParamType** paramList);
		ACE_INT32 svc();
		ACE_INT32 shutdown();
		ACE_INT32 removeImplementer();
		
	private:
		std::string theClassName;
		acs_apgcc_oihandler_V3 *theObserver;
		ACE_Reactor* theReportReactorPtr;
		ACE_TP_Reactor* theReportReactorImplPtr;
		bool m_isObjImplAdded;
		OperationType typeOfOperation;
		std::string userName;
		StateType state;
};

#endif /* ACS_USA_AXEADDITIONALINFO_OBSERVER_H_ */

