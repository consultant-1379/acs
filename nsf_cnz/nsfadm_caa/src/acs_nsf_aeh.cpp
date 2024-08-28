/*=================================================================== */
   /**
   @file acs_nsf_aeh.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the acs_nsf_aeh.h module.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Task.h>
#include "acs_nsf_aeh.h"
#include "acs_nsf_types.h"
#include <ACS_APGCC_Util.H>
#include <acs_aeh_evreport.h>


using namespace std;

/*===================================================================
   ROUTINE: ACS_NSF_AEH
===================================================================*/
ACS_NSF_AEH::ACS_NSF_AEH()
{
}

/*===================================================================
   ROUTINE: ReportEvent
===================================================================*/
bool ACS_NSF_AEH::ReportEvent(const ACE_UINT32  EventNumber,
                              const ACE_TCHAR * Severity,
                              const ACE_TCHAR * Cause,
                              const ACE_TCHAR * ObjectOfReference,
                              const ACE_TCHAR * Data,
                              const ACE_TCHAR * Text,
                              bool bReportToAEH)
{
	//ACS_APGCC_Event oGCCEvent;
	//ACS_APGCC_Alarm oGCCAlarm;
	ACE_TCHAR szProcName[64];
    	ACE_OS::sprintf(szProcName,ACE_TEXT(("%s:%d")),PROCESS_NAME,ACE_OS::getpid());

   	bool bResult = false;
   	if (bReportToAEH)
   	{
			acs_aeh_evreport * nEvReport = new acs_aeh_evreport();
   				DEBUG("Send Event to AEH with Process Name and its ID:%s",szProcName);
   				ACS_AEH_ReturnType status = nEvReport->sendEventMessage(szProcName,					// process name
   						EventNumber,					// specific problem
   						Severity,					// perceived severity
   						Cause,	    // probably cause
   						"APZ",						// object class of reference
   						ObjectOfReference,					// object of reference
   						Data,	// problem data
   						Text);	// problem text
   				if (status == ACS_AEH_error)
   				{
   					ACE_INT32 err_code = nEvReport->getError();
   					switch (err_code)
   					{
   					case ACS_AEH_syntaxError:

   						DEBUG("Eventhandler:ACS_AEH_syntaxError with EventNumber: %d",EventNumber);
   						//std::cout<<"Syntax Error in the event"<<endl;
   						break;

   					case ACS_AEH_eventDeliveryFailure:

   						DEBUG("ACS_AEH_eventDeliveryFailure for process name and its Id:%s",szProcName);
   						//std::cout<<"ACS_AEH_eventDeliveryFailure for Event"<<endl;
   						break;
   					}
   				}
   				else if(status == ACS_AEH_ok)
   				{
   					DEBUG("Event/Alarm sent successfully to AEH for the process name and its Id: %s",szProcName);
   				}

	

	      #if 0
   	      const ACE_TCHAR *pchTime  = ACS_APGCC::formatDateTime(" %04Y-%02m-%02d %02H:%02M:%02S \n");


   	      FILE* eventfile = ACE_OS::fopen(ACE_TEXT(NSF_EVENT_FILE), ACE_TEXT("a"));

   	      if (!eventfile)
   	         return false;

   	      ACE_TCHAR szAlarmData[1024];

   	      ACE_OS::snprintf(szAlarmData,(sizeof(szAlarmData)/sizeof(*szAlarmData) )- 1,
   	                       "\n%s\nProcessName: %s\nSpecificProblem: %lu\nSeverity: %s\nProbableCause: %s\nObjectClassOfReference: APZ"\
   	                       "\nObjectOfReference: %s\n"\
   	    		           "ProblemData: %s\n"\
   	    		           "ProblemText: %s\n\n",
   	    		           pchTime,szProcName,EventNumber,Severity,Cause,ObjectOfReference,Data,Text);

   	      fwprintf(eventfile, ACS_APGCC::toWide(pchTime));
   	      fwprintf(eventfile, ACS_APGCC::toWide("\nProcessName: %s\n"), szProcName);
   	      fwprintf(eventfile, ACS_APGCC::toWide("SpecificProblem: %lu\n"), EventNumber);
   	      fwprintf(eventfile, ACS_APGCC::toWide("Severity: %s\n"), Severity);
   	      fwprintf(eventfile, ACS_APGCC::toWide("ProbableCause: %s\n"), Cause);
   	      fwprintf(eventfile, ACS_APGCC::toWide("ObjectClassOfReference: APZ\n"));
   	      fwprintf(eventfile, ACS_APGCC::toWide("ObjectOfReference: %s\n"), ObjectOfReference);
   	      fwprintf(eventfile, ACS_APGCC::toWide("ProblemData: %s\n"), Data);
   	      fwprintf(eventfile, ACS_APGCC::toWide("ProblemText: %s\n\n"), Text);

   	      fflush(eventfile);
   	      fclose(eventfile);
	      #endif

   	      bResult = true;
   	}
   	return bResult;
}
