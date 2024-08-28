/*
 * @file ACS_CS_API_SetOmProfileNotification.cpp
 * @author xmikhal
 * @date Jan 4, 2011
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */

#include <sstream>

#include "ACS_CS_Util.h"
#include "ACS_CS_API_Set.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_API_Util_Internal.h"
#include "ACS_CS_API_SetOmProfileNotification.h"

#include "acs_apgcc_adminoperation.h"

//#include "ACS_CS_Trace.h"
//ACS_CS_Trace_TDEF(ACS_CS_API_SetOmProfileNotification_TRACE);
#include "ACS_CS_API_Tracer.h"
ACS_CS_API_TRACER_DEFINE(ACS_CS_API_SetOmProfileNotification_TRACE);

using namespace std;
using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using namespace ACS_CS_Event_NS;
using namespace ACS_CS_INTERNAL_API;

namespace
{
	const ACS_APGCC_AdminOperationIdType OM_PROFILE_NOTIFICATION_SUCCESS = 1;
	const ACS_APGCC_AdminOperationIdType OM_PROFILE_NOTIFICATION_FAILURE = 2;
	const ACS_APGCC_AdminOperationIdType OM_PROFILE_APA_NOTIFICATION	 = 3;

	const int PHASE_IDLE = -1;
	const int PHASE_VALIDATE = 0;
	const int PHASE_AP_NOTIFY = 1;
	const int PHASE_CP_NOTIFY = 2;
	const int PHASE_COMMIT = 3;

	const char * ADVANCED_CONFIGURATION_DN = "advancedConfigurationId=1,AxeEquipmentequipmentMId=1";
	char const * const ACTION_PHASE_PARAMETER = "phase";
	char const * const ACTION_REASON_PARAMETER = "reason";
	char const * const ACTION_APZ_PARAMETER = "apzProfile";
	char const * const ACTION_APT_PARAMETER = "aptProfile";
}


ACS_CS_API_SetOmProfileNotification::ACS_CS_API_SetOmProfileNotification() :
        profileHandling(NULL)
{
    profileHandling = new ACS_CS_API_ProfileHandling();
}


ACS_CS_API_SetOmProfileNotification::~ACS_CS_API_SetOmProfileNotification()
{
    if (profileHandling)
        delete profileHandling;
}


ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_SetOmProfileNotification::setOmProfileNotificationStatus(
        ACS_CS_API_OmProfilePhase::PhaseValue phase, int reasonCode)
{
    const std::string notifyTypeString = (reasonCode ? "setOmProfileNotificationFailure" : "setOmProfileNotificationSuccess");
    ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_Success;
    int profilePhase = static_cast<int>(phase);

    ACS_CS_API_TRACER_MESSAGE("%s phase == %d, reason == %d", notifyTypeString.c_str(), profilePhase, reasonCode);

    //--------------------------
    // Set action parameters
    ACS_APGCC_AdminOperationIdType operationId = OM_PROFILE_NOTIFICATION_SUCCESS;
    vector<ACS_APGCC_AdminOperationParamType> paramVector;

    ACS_APGCC_AdminOperationParamType phaseParameter;
    phaseParameter.attrName = (char *)ACTION_PHASE_PARAMETER;
    phaseParameter.attrType = ATTR_INT32T;
    int phaseValue = static_cast<int>(phase);
    phaseParameter.attrValues = reinterpret_cast<void*>(&phaseValue);

    paramVector.push_back(phaseParameter);

    if(reasonCode) // // Failure notification from subscriber
    {
    	ACS_APGCC_AdminOperationParamType reasonParameter;
    	reasonParameter.attrName = (char *) ACTION_REASON_PARAMETER;
    	reasonParameter.attrType = ATTR_INT32T;
    	reasonParameter.attrValues = reinterpret_cast<void*>(&reasonCode);
    	paramVector.push_back(reasonParameter);
    	operationId = OM_PROFILE_NOTIFICATION_FAILURE;
    }

    //------------------------------------
    // initialize the connection to IMM
    acs_apgcc_adminoperation admin;
    ACS_CC_ReturnType adminResult = admin.init();

    if (ACS_CC_SUCCESS == adminResult)
    {
    	//-------------------------
    	// invoke operation

    	int ret = 0;
    	bool send = false;
    	int retryAction = 0;
//    	ACS_CC_ReturnType operationResult = admin.adminOperationInvoke(ADVANCED_CONFIGURATION_DN, 0, operationId, paramVector, &ret, 0);
//    	int retryAction = 0; ((retryAction < 5) && (ACS_CC_SUCCESS != admin.adminOperationInvoke(ADVANCED_CONFIGURATION_DN, 0, operationId, paramVector, &ret, 0))); ++retryAction)
    	while( retryAction < 3 && !send)
    	{
    		if (ACS_CC_SUCCESS == admin.adminOperationInvoke(ADVANCED_CONFIGURATION_DN, 0, operationId, paramVector, &ret, 0))
    		{
    			send = true;
    			//check answer from Object Implementer (ACS_CS_ServiceHandler)
    			if (SA_AIS_OK != ret)
    			{
    				//-------------------------
    				// log error
    				ostringstream eventData;
    				eventData << notifyTypeString << " cannot be invoked. phase == " << phase << ", reason == " << reasonCode <<
    						". Implementer return value == " << ret << ".";
    				ACS_CS_EVENT(Event_GeneralFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    				ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    				ACS_CS_EventReporter::instance()->resetAllEvents();
    				returnValue = ACS_CS_API_SET_NS::Result_Failure;
    			}
    		}
    		else
    		{
    			ostringstream traceData;
    			traceData << notifyTypeString << " Retry to initialize connection to IMM. phase == " << phase << ", reason == " << reasonCode <<
    			". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    			ACS_CS_API_TRACER_MESSAGE("%s", traceData.str().c_str());
    			++retryAction;
    			sleep(1);
    		}
    	}

    	if (!send)
    	{
    		// log error
    		ostringstream eventData;
    		eventData << notifyTypeString << " cannot be invoked. Cannot initialize connection to IMM. phase == " << phase << ", reason == " << reasonCode <<
    				". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    		ACS_CS_EVENT(Event_IOFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    		ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    		ACS_CS_EventReporter::instance()->resetAllEvents();
    		returnValue = ACS_CS_API_SET_NS::Result_Failure;
    	}

    	for ( int retry = 0; ((retry < 10) && (ACS_CC_SUCCESS != admin.finalize())); ++retry)
    	{
    		//-------------------------
    		// log error
    		ostringstream eventData;
    		eventData << notifyTypeString << " invoked, but IMM connection cannot be finalized. phase == " << phase << ", reason == " << reasonCode <<
    				". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    		ACS_CS_EVENT(Event_IOFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    		ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    		ACS_CS_EventReporter::instance()->resetAllEvents();
    	}
    }
    else
    {
    	//-------------------------
    	// log error
    	ostringstream eventData;
    	eventData << notifyTypeString << " cannot be invoked. Cannot initialize connection to IMM. Return value == "<< adminResult <<". phase == " << phase << ", reason == " << reasonCode <<
    			". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    	ACS_CS_EVENT(Event_IOFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    	ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    	ACS_CS_EventReporter::instance()->resetAllEvents();
    	returnValue = ACS_CS_API_SET_NS::Result_Failure;
    }

    return returnValue;
}



ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_SetOmProfileNotification::setOmProfileApaNotification(ACS_CS_API_OmProfileChange::Profile aptProfile,
		ACS_CS_API_OmProfileChange::Profile apzProfile)
{
    ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_Success;
    int apzValue = static_cast<int>(apzProfile);
    int aptValue = static_cast<int>(aptProfile);

    ACS_CS_API_TRACER_MESSAGE("apzProfile == %d, aptProfile == %d", apzValue, aptValue);

    //--------------------------
    // Set action parameters
    ACS_APGCC_AdminOperationIdType operationId = OM_PROFILE_APA_NOTIFICATION;
    vector<ACS_APGCC_AdminOperationParamType> paramVector;

    ACS_APGCC_AdminOperationParamType apzParameter;
    apzParameter.attrName = (char *)ACTION_APZ_PARAMETER;
    apzParameter.attrType = ATTR_INT32T;
    apzParameter.attrValues = reinterpret_cast<void*>(&apzValue);

    ACS_APGCC_AdminOperationParamType aptParameter;
    aptParameter.attrName = (char *)ACTION_APT_PARAMETER;
    aptParameter.attrType = ATTR_INT32T;
    aptParameter.attrValues = reinterpret_cast<void*>(&aptValue);

    paramVector.push_back(apzParameter);
    paramVector.push_back(aptParameter);


    //------------------------------------
    // initialize the connection to IMM
    acs_apgcc_adminoperation admin;
    ACS_CC_ReturnType adminResult = admin.init();

    if (ACS_CC_SUCCESS == adminResult)
    {
    	//-------------------------
    	// invoke operation

    	int ret = 0;
    	bool send = false;
    	int retryAction = 0;

    	while( retryAction < 5 && !send)
    	{
    		if (ACS_CC_SUCCESS == admin.adminOperationInvoke(ADVANCED_CONFIGURATION_DN, 0, operationId, paramVector, &ret, 0))
    		{
    			send = true;
    			//check answer from Object Implementer (ACS_CS_ServiceHandler)
    			if (SA_AIS_OK != ret)
    			{
    				//-------------------------
    				// log error
    				ostringstream eventData;
    				eventData << " cannot be invoked. apzProfile == " << apzValue << ", aptProfile == " << aptValue <<
    						". Implementer return value == " << ret << ".";
    				ACS_CS_EVENT(Event_GeneralFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    				ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    				ACS_CS_EventReporter::instance()->resetAllEvents();
    				returnValue = ACS_CS_API_SET_NS::Result_Failure;
    			}
    		}
    		else
    		{
    			ostringstream traceData;
    			traceData << " Retry to initialize connection to IMM. apzProfile == " << apzValue << ", aptProfile == " << aptValue <<
    			". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    			ACS_CS_API_TRACER_MESSAGE("%s", traceData.str().c_str());
    			++retryAction;
    			sleep(1);
    		}
    	}

    	if (!send)
    	{
    		// log error
    		ostringstream eventData;
    		eventData << " cannot be invoked. Cannot initialize connection to IMM. apzProfile == " << apzValue << ", aptProfile == " << aptValue <<
    				". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    		ACS_CS_EVENT(Event_IOFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    		ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    		ACS_CS_EventReporter::instance()->resetAllEvents();
    		returnValue = ACS_CS_API_SET_NS::Result_Failure;
    	}

    	for ( int retry = 0; ((retry < 10) && (ACS_CC_SUCCESS != admin.finalize())); ++retry)
    	{
    		//-------------------------
    		// log error
    		ostringstream eventData;
    		eventData << " invoked, but IMM connection cannot be finalized. apzProfile == " << apzValue << ", aptProfile == " << aptValue <<
    				". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    		ACS_CS_EVENT(Event_IOFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    		ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    		ACS_CS_EventReporter::instance()->resetAllEvents();
    	}
    }
    else
    {
    	//-------------------------
    	// log error
    	ostringstream eventData;
    	eventData << " cannot be invoked. Cannot initialize connection to IMM. Return value == "<< adminResult <<". apzProfile == " << apzValue << ", aptProfile == " << aptValue <<
    			". Error code == " << admin.getInternalLastError() << ", Error Message == " << admin.getInternalLastErrorText() << ".";
    	ACS_CS_EVENT(Event_IOFailure, ACS_CS_EventReporter::Severity_Event, "IMM Operation", eventData.str(), "");
    	ACS_CS_API_TRACER_MESSAGE("%s", eventData.str().c_str());
    	ACS_CS_EventReporter::instance()->resetAllEvents();
    	returnValue = ACS_CS_API_SET_NS::Result_Failure;
    }

    return returnValue;
}

