#ifndef ACS_SCH_CRMOBSERVER_H_
#define ACS_SCH_CRMOBSERVER_H_

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <acs_apgcc_oihandler_V3.h>
#include "ACS_SCH_ImUtils.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_SCH_IMMCallbackThread.h"
#include "ACS_SCH_Server.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Trace.h"
#include <ace/Task.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ACS_CS_API.h"

class ACS_SCH_Server;
class ACS_SCH_CRRHandler;
class ACS_SCH_CrmObserver: public acs_apgcc_objectimplementereventhandler_V3, public ACE_Task_Base {

	public:
		ACS_SCH_CrmObserver( std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope);
		virtual ~ACS_SCH_CrmObserver();
		virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
		virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
		virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
		virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
		virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
		virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
		virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);
		virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);
		static void  createCRRObject(string crname, string vAPZ_name,string roleName,int state);
		static bool getBCsCount(vector<string>& bcRole,int& bladeCount);
		static void getTrafficLeaderBcName (std::string& trafficLeaderBcName);
		ACE_INT32 svc();
		ACE_INT32 shutdown();
		ACE_INT32 removeImplementer();
		static void scaleout(string bcName, string crrDn);
		static bool scaleoutPreconditions();
		static void performScaleout(string bcName,string vAPZ_name,string roleName);
		static void updateCRR(void* param);
                static void createbootOptsFile(string trafficLeaderBcName,string bcName);
		static bool fileExists(const string& fileName);
		static void copyFile(const string& source, const string& dest);

	private:
		std::string theClassName;
		acs_apgcc_oihandler_V3 *theCRMHandler;
		ACE_Reactor* theReportReactorPtr;
		ACE_TP_Reactor* theReportReactorImplPtr;
		static string _threadParam;
		OmHandler immHandle;
		bool m_isObjImplAdded;
};

#endif /* ACS_SCH_CRMOBSERVER_H_ */

