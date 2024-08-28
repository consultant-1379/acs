
// -*- C++ -*-

//=============================================================================
/**
*  @file    ACS_SSU_AEH.cpp
*
*  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
*
*  @author 2010-06-11 by XSIRKUM
*
*  @documentno
*
* Event Report Fields :
*
* Process Name              : Process:ProcessID
* Specific Problem          : Unique Event Number
* Perceived Severity        : Severity
* Probable Cause            : Short Description of the Discovered problem
* Object Class Of Reference : Class of Object (  APZ for ACS )
* Object Of Reference       : In which sub object of class this problem was created ( Mem,Disc)
* Problem Data              : Free text Description
* Problem Text              : Contributes to the POD in the CP. Should be properly formatted
*
*  - A combination of Specific Problem and Object of reference should be unique for a alarm across APG
*  - There can be reports with same Process Name/Specific Problem.
*  - For ceasing Specific Problem and Object of reference should be same as alarm raise.
*
*
*  @todo  After all APGCC interfaces are available recheck Event reporting
*  @todo  Bug in APGCC::formatDateTime only converts 4 chars because of sizeof issue
*/
//=============================================================================

#include <stdio.h>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <acs_aeh_evreport.h>
#include <ACS_APGCC_Util.H>
#include <acs_ssu_aeh.h>
#include <acs_ssu_common.h>
#include <acs_ssu_types.h>

using namespace std;

#define SSU_EVENT_FILE     ACE_TEXT(("/tmp/SSU_Event.log"))

//============================================================================
// Constructor
//============================================================================
ACS_SSU_AEH::ACS_SSU_AEH()
{
}

//! Send a event/alarm to AEH or a logfile
bool ACS_SSU_AEH::ReportEvent(const ACE_UINT32  EventNumber,
		const ACE_TCHAR * Severity,
		const ACE_TCHAR * Cause,
		const ACE_TCHAR * ObjectOfReference,
		const ACE_TCHAR * Data,
		const ACE_TCHAR * Text,
		bool bReportToAEH)
{
	ACE_TCHAR szProcName[64]="";
	if (EventNumber == 6102)
	{
		ACE_OS::sprintf(szProcName,ACE_TEXT(("%s")),ACE_TEXT("acs_ssumonitord"));
	}
	else
	{
		ACE_OS::sprintf(szProcName,ACE_TEXT(("%s:%d")),PROCESS_NAME,ACE_OS::getpid());
	}

	bool bResult = false;

	if (bReportToAEH)
	{
		acs_aeh_evreport * pEvReport = new acs_aeh_evreport();
		ACS_AEH_ReturnType status = pEvReport->sendEventMessage(szProcName,					// process name
				EventNumber,					// specific problem
				Severity,					// perceived severity
				Cause,	    // probably cause
				"APZ",						// object class of reference
				ObjectOfReference,					// object of reference
				Data,	// problem data
				Text);	// problem text
		if (status == ACS_AEH_error)
		{
			ACE_INT32 err_code = pEvReport->getError();
			switch (err_code)
			{
			case ACS_AEH_syntaxError:

				ERROR("Eventhandler:ACS_AEH_syntaxError for processname and its Id: %s",szProcName);
				break;

			case ACS_AEH_eventDeliveryFailure:

				ERROR("ACS_AEH_eventDeliveryFailure for process name and its Id:%s", szProcName);
				break;
			}
		}
		else if(status == ACS_AEH_ok)
		{
			INFO("Event/Alarm sent successfully to AEH for the process name and its Id: %s",szProcName);
		}

		/* COMMENTED TO REDUCE SHUTDOWN TIME OF THE SERVICE */
        /*
		INFO("%s","\n Write event to file");

		const ACE_TCHAR *pchTime  = ACS_APGCC::formatDateTime(" %04Y-%02m-%02d %02H:%02M:%02S \n");


		FILE* eventfile = ACE_OS::fopen(ACE_TEXT(SSU_EVENT_FILE), ACE_TEXT("a"));

		if (!eventfile)
			return false;

		ACE_TCHAR szAlarmData[1024];

		ACE_OS::snprintf(szAlarmData,(sizeof(szAlarmData)/sizeof(*szAlarmData) )- 1,
				"%s\nProcessName: %s\nSpecificProblem: %lu\nSeverity: %s\nProbableCause: %s\nObjectClassOfReference: APZ"\
				"\nObjectOfReference: %s\n"\
				"ProblemData: %s\n"\
				"ProblemText: %s\n\n",
				pchTime,szProcName,EventNumber,Severity,Cause,ObjectOfReference,Data,Text);

		//DEBUG("%s",szAlarmData);

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
        */
		bResult = true;
		if(pEvReport)
		{
			delete pEvReport;
			pEvReport = NULL;
		}
	}
	return bResult;

}
