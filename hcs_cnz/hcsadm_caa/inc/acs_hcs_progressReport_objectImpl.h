//=============================================================================
/**
   @file    acs_hcs_inUse_objectImpl.h

   @brief Header file for HC module.
          It acts as OI for handling IMM Callbacks of HC inUse object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012   XHARBAV   Initial Release
 */
//=============================================================================
#ifndef ACS_HC_PROGRESSREPORT_OBJECTIMPL_H
#define ACS_HC_PROGRESSREPORT_OBJECTIMPL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <cstdlib>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <ACS_TRA_trace.h>
#include "ACS_APGCC_RuntimeOwner_V2.h"
#include "acs_hcs_tra.h"
#include "acs_hcs_progressReportHandler.h"


#define ACS_HC_PROGRESSREPORT_CLASS_NAME "HealthCheckAsyncActionProgress"

class acs_hcs_progressReport_ObjectImpl : public ACS_APGCC_RuntimeOwner_V2
{
public:
	acs_hcs_progressReport_ObjectImpl();
	~acs_hcs_progressReport_ObjectImpl();

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);
	
	ACS_CC_ReturnType updateCallback(const char* p_objName, const char** p_attrNames);	

	enum result
	{
		SUCCESS = 1,
		FAILURE = 2, 
		NOT_AVAILABLE = 3
	};

	enum state
	{
		CANCELLING = 1, 
		RUNNING = 2, 
		FINISHED = 3, 
		CANCELLED = 4
	};

	enum schedulerstate
	{
		LOCKED = 0,
		UNLOCKED = 1,
		SHUTTINGDOWN = 2
	};

	struct progress
	{
		acs_hcs_progressReportHandler* handler;
		acs_hcs_progressReport_ObjectImpl* impl;	
		std::string DN;
		int state;
		int result;
		string resultInfo;
		string timeActionStarted;
		string timeOfLastStatusUpdate;
		string timeActionCompleted;
		string additionalInfo;
	};

	static std::vector<progress> progressReport;
	static state stateEnum;
	static result resultEnum;
	void shutdown();
     
private:

};

#endif
