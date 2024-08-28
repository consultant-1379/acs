#ifndef ACS_SCH_CLUSTERCPOBSERVER_H_
#define ACS_SCH_CLUSTERCPOBSERVER_H_

#include <iostream>
#include <unistd.h>
#include <fstream>

#include "ACS_SCH_ImUtils.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_SCH_IMMCallbackThread.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Trace.h"
#include <ace/Task.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"

#include <acs_apgcc_oihandler_V3.h>

class ACS_SCH_ClusterCPObserver: public acs_apgcc_objectimplementereventhandler_V3, public ACE_Task_Base {
public:

	ACS_SCH_ClusterCPObserver(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope);
	virtual ~ACS_SCH_ClusterCPObserver();
	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
	virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);
	virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);
	ACE_INT32 svc();
	ACE_INT32 shutdown();
	ACE_INT32 removeImplementer();

private:
  ACE_Reactor* theReportReactorPtr;
  ACE_TP_Reactor* theReportReactorImplPtr;
  acs_apgcc_oihandler_V3 *theCPHandler;
	std::string theClassName;
	bool m_isObjImplAdded;
};

#endif /* ACS_SCH_CLUSTERCPOBSERVER_H_ */

