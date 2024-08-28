/*
 * ACS_USA_NtpSrvM.cpp
 *
 *  Created on: 
 *      Author: 
 */
#include <string>
#include <vector>
#include <ace/Reactor.h>

#include "acs_usa_global.h"
#include "acs_usa_logtrace.h"
#include "acs_usa_ntpSrvM.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_error.h"

ACS_USA_Trace traNTP ("ACS_USA_NtpSrvM                ");
const unsigned int implementerRetryTimeout = 2;
ACE_Recursive_Thread_Mutex impLock;

/*============================================================================
	ROUTINE: ACS_USA_NtpSrvM
 ============================================================================ */
ACS_USA_NtpSrvM::ACS_USA_NtpSrvM() :  acs_apgcc_objectimplementerinterface_V3(IMPLEMENTER_NTP),
m_ImmClassName(classNtpServer),m_timerid(-1)
{
	USA_TRACE_ENTER2("Constructor");
	
	USA_TRACE_LEAVE2("Constructor");

}

/*============================================================================
	ROUTINE: ~ACS_USA_NtpSrvM
 ============================================================================ */
ACS_USA_NtpSrvM::~ACS_USA_NtpSrvM()
{
	USA_TRACE_ENTER2("Destructor");
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM Destructor called.");	
	
	acs_apgcc_oihandler_V3 oiHandler;
	if(oiHandler.removeClassImpl(this, this->getIMMClassName())== ACS_CC_FAILURE)
	{
		traNTP.traceAndLogFmt(DEBUG, "%s() - Failed to remove the implementer", __func__);
		USA_TRACE_1("Error occured while deleting implementer");
	}
	
	USA_TRACE_LEAVE2("Destructor");
}

int ACS_USA_NtpSrvM::registerNtpObserver()
{
	USA_TRACE_ENTER();
	setScope(ACS_APGCC_ONE);
	acs_apgcc_oihandler_V3 oiHandler;
	//bool implementerSet=true;
	ACE_Guard<ACE_Recursive_Thread_Mutex> lock(impLock);		
	if(!ACS_USA_Global::instance()->implementer_set()) {
		if(oiHandler.addClassImpl(this, this->getIMMClassName())== ACS_CC_FAILURE)
		{
			traNTP.traceAndLogFmt(ERROR, "%s() - Failed to set the implementer", __func__);
			USA_TRACE_1("Error occured while setting implementer");
		}
		else
		{
			ACS_USA_Global::instance()->implementer_set(true);
		}
	}

	if(ACS_USA_Global::instance()->implementer_set())
	{
		traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM - INFO: setImplementer success.");

		if(ACS_USA_Global::instance()->reactor()->register_handler(acs_apgcc_objectimplementerinterface_V3::getSelObj(), this, ACE_Event_Handler::READ_MASK) < 0)
        	{
                	string tempObj(objName);
			traNTP.traceAndLogFmt(ERROR, "ACS_USA_NtpSrvM:%s() - Register Implementer Failed[%s]", __func__, tempObj.c_str());
			return -1; //return error
		}
		else
		{	traNTP.traceAndLogFmt(INFO, "registerImplementer() - INFO: Success.");
			cout << " Register Implementer Success: " << objName << endl;
		}
	}
	else
	{
		traNTP.traceAndLogFmt(ERROR, "ACS_USA_ntpSrvM:%s() - Implementer not set..Going for Retry.", __func__);
		const ACE_Time_Value schedule_time(implementerRetryTimeout);
		m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);
                if (this->m_timerid < 0){
                        traNTP.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
                        return -1;
                }
	}

        return 0;
}

ACS_CC_ReturnType ACS_USA_NtpSrvM::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **/*attr*/)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM:%s() - ObjectCreateCallback invocated.", __func__);
	cout << " oiHandle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << className << endl;
	cout << " parent name: " << parentname << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	m_callback=createT;
	return result;
}

ACS_CC_ReturnType ACS_USA_NtpSrvM::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpServM:%s() - ObjectDeleteCallback invocated.", __func__);

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;


	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	m_callback=deleteT;
	return result;
}

ACS_CC_ReturnType ACS_USA_NtpSrvM::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **/*attrMods*/)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM:%s() - ObjectModifyCallback invocated.", __func__);	
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	m_callback=modifyT;

	return result;
}

ACS_CC_ReturnType ACS_USA_NtpSrvM::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM:%s() - ObjectCompleteCallback invocated.", __func__);

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "COMPLETECallback " << endl;
	cout << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	return result;
}

void ACS_USA_NtpSrvM::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM:%s() - ObjectAbortCallback invocated.", __func__);
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "ABORTCallback " << endl;
	cout << endl;
}

void ACS_USA_NtpSrvM::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM:%s() - ObjectApplyCallback invocated.", __func__);
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "APPLYCallback " << endl;
	cout << endl;
        switch(m_callback)
        {
        case createT:
               	ceaseNtpServerAlarm(); 
                break;
        case deleteT:
                monitorNtpServer();
                break;
        default:
                break;
        }


}

ACS_CC_ReturnType ACS_USA_NtpSrvM::updateRuntime(const char* /*p_objName*/, const char** /*p_attrName*/)
{
	printf("------------------------------------------------------------\n");
	printf("                   updateRuntime called                     \n");
	printf("------------------------------------------------------------\n");

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	return result;
}


void ACS_USA_NtpSrvM::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**/*paramList*/) {

	printf("------------------------------------------------------------\n");
	printf("                   adminOperationCallback called             \n");
	printf("------------------------------------------------------------\n");

	cout<<"oiHandle   : "<< oiHandle <<endl;
	cout<<"invocation : "<< invocation <<endl;
	cout<<"p_objName  : "<< p_objName <<endl;
	cout<<"operationId: "<< operationId <<endl;
	cout << " Implementing Admin Operation with ID = " << operationId << endl;

}

bool ACS_USA_NtpSrvM::removeNtpObserver ()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> lock(impLock);
	if(ACS_USA_Global::instance()->reactor()->remove_handler(acs_apgcc_objectimplementerinterface_V3::getSelObj(), ACE_Event_Handler::READ_MASK) < 0)
	{
		string tempObj(objName);
		traNTP.traceAndLogFmt(ERROR, "ACS_USA_NtpSrvM:%s() - Deregister Implementer Failed[%s]", __func__, tempObj.c_str());
		cout << " Deregister Implementer Failed: " << objName << endl;
		return false;
	}
	else {
		traNTP.traceAndLogFmt(INFO, "deRegisterImplementer() - INFO: Success.");
		cout << " Deregister Implementer Success: " << objName << endl;
	}
	if (m_timerid != -1) {
                ACS_USA_Global::instance()->reactor()->cancel_timer(m_timerid);
        }

	acs_apgcc_oihandler_V3 oiHandler;
	if(oiHandler.removeClassImpl(this, this->getIMMClassName())== ACS_CC_FAILURE)
	{
		traNTP.traceAndLogFmt(DEBUG, "ACS_USA_NtpSrvM::%s() - Failed to delete the implementer", __func__);
		USA_TRACE_1("Error occured while deleting implementer");
	}
	return true;
}

int ACS_USA_NtpSrvM::handle_input(ACE_HANDLE fd){

        ACE_UNUSED_ARG(fd);
        USA_TRACE_ENTER();

        this->dispatch(ACS_APGCC_DISPATCH_ONE);

        USA_TRACE_LEAVE();
        return 0;
}

void ACS_USA_NtpSrvM::reportProblem() const {

        USA_TRACE_ENTER();

        ACS_USA_ErrorType err = error.getError();
        String perceivedSeverity(ACS_USA_PERCEIVED_SEVERITY_EVENT);
        String probableCause(ACS_USA_INTLFAULT);
        if (eventManager.report(err + ACS_USA_FirstErrorCode,
                                perceivedSeverity,
                                probableCause,
                                String(),
                                String(error.getErrorText()),
                                String(error.getProblemText(error.getError())),
                                "",
                                0) == ACS_USA_Error)
        {
        }

        USA_TRACE_LEAVE();

        return;
}

int ACS_USA_NtpSrvM::monitorNtpServer()
{
        char *className = const_cast<char*>(m_ImmClassName.c_str());
        ACS_CC_ReturnType result;
        std::vector<std::string> p_dnList;
        OmHandler immHandle;

        result = immHandle.Init();
        if ( result != ACS_CC_SUCCESS ){
                traNTP.traceAndLogFmt(ERROR,"ACS_USA_NtpSrvM:%s() Initialization FAILURE",__func__);
                USA_TRACE_1("Initialization FAILURE");
                error.setError(ACS_USA_SyntaxError);
                error.setErrorText(ACS_USA_ImmInitFailed);
                reportProblem();
                return ACS_USA_Error;
        }
        result = immHandle.getClassInstances(className, p_dnList);
        size_t instancesNumber  = p_dnList.size();

	traNTP.traceAndLogFmt(INFO,"ACS_USA_NtpSrvM:%s() - %d NtpServer object found",__func__,instancesNumber);
        USA_TRACE_1("Instances Found:[%zu]",instancesNumber);

        if (instancesNumber == 0)
		raiseNtpServerAlarm();
	else
		ceaseNtpServerAlarm();
	result = immHandle.Finalize();
        return result;
}

int ACS_USA_NtpSrvM::raiseNtpServerAlarm()
{
	ACS_CC_ReturnType result=ACS_CC_SUCCESS;
	ACS_USA_SpecificProblem specificProblem= ACS_USA_NTP_SPECIFIC_PROBLEM;
        String perceivedSeverity(ACS_USA_PERCEIVED_SEVERITY_A2);
        String probableCause(ACS_USA_NTPSERVER_FAULT);
        String objectOfReference(ACS_USA_processName);
        String node;
        String problemText(ACS_USA_ntpProblemText);
        String problemData(ACS_USA_ntpProblemData);
        if (eventManager.report(specificProblem, perceivedSeverity, probableCause,objectOfReference, problemData, problemText, node,0, false)== ACS_USA_Error) 
	{
		traNTP.traceAndLogFmt(ERROR,"ACS_USA_NtpSrvM:%s() - Error in raising alarm",__func__);	
                USA_TRACE_1("Alarm Failed to raise");
                result=ACS_CC_FAILURE;
        }
	return result;
}

int ACS_USA_NtpSrvM::ceaseNtpServerAlarm()
{
	ACS_CC_ReturnType result=ACS_CC_SUCCESS;
	ACS_USA_SpecificProblem specificProblem= ACS_USA_NTP_SPECIFIC_PROBLEM;
	String probableCause(ACS_USA_NTPSERVER_FAULT);
	String objectOfReference(ACS_USA_processName);
	String problemData(ACS_USA_ntpProblemData);
	String node;
	String problemText(ACS_USA_ntpProblemText);
	if (eventManager.report(specificProblem, String(ACS_USA_PERCEIVED_SEVERITY_CEASING), probableCause,objectOfReference,
			 problemData,problemText, node,0, false) == ACS_USA_Error) 
	{
		traNTP.traceAndLogFmt(ERROR,"ACS_USA_NtpSrvM:%s() - Error in ceasing alarm",__func__);
		USA_TRACE_1("Alarm Failed to cease");
		result=ACS_CC_FAILURE;
	}
	return result;
}

int ACS_USA_NtpSrvM::handle_close(ACE_HANDLE, ACE_Reactor_Mask /*mask*/) {
	
        USA_TRACE_ENTER();
	int rCode=0;
	traNTP.traceAndLogFmt(INFO,"ACS_USA_NtpSrvM:%s() -called ",__func__);

	acs_apgcc_oihandler_V3 oiHandler;	
	rCode=oiHandler.removeClassImpl(this, this->getIMMClassName());
        if(rCode!=0)
        {
                for (int i=0; i<5; i++){
                        rCode=oiHandler.removeClassImpl(this, this->getIMMClassName());
                        if ( rCode == 0 ) {
                                traNTP.traceAndLogFmt(INFO, "Implementer deletion - INFO: Success.");
				ACS_USA_Global::instance()->implementer_set(false);
                                break;
                        }
                        ::sleep(1);
                        traNTP.traceAndLogFmt(ERROR, "ACS_USA_NtpSrvM::%s() - Failed to delete the implementer..retrying", __func__);
                        USA_TRACE_1("Error occured while deleting implementer");
                }
        }
	else {
		ACS_USA_Global::instance()->implementer_set(false);
	}
	if(rCode!=0)
	{
		traNTP.traceAndLogFmt(ERROR, "ACS_USA_NtpSrvM::%s() - Failed to delete the implementer", __func__);
	}
	if (m_timerid != -1) {
		ACS_USA_Global::instance()->reactor()->cancel_timer(m_timerid);
	}	
        USA_TRACE_LEAVE();
        return 0;
}

int ACS_USA_NtpSrvM::handle_timeout(const ACE_Time_Value&, const void* ) {

        USA_TRACE_ENTER();
	int rCode=0;
	traNTP.traceAndLogFmt(INFO, "ACS_USA_NtpSrvM::%s() - called.", __func__);
	if(!ACS_USA_Global::instance()->implementer_set()){
		rCode=registerNtpObserver();
	}
        USA_TRACE_LEAVE();
        return rCode;
}
