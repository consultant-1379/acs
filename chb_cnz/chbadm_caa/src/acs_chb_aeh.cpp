/*=================================================================== */
   /**
   @file acs_chb_aeh.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_aeh.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/01/2011   XNADNAR   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ACS_APGCC_Util.H>
#include <acs_aeh_evreport.h>
#include <acs_chb_aeh.h>
#include <acs_chb_tra.h>

using namespace std;

/*===================================================================
                        DEFINE DECLARATION SECTION
=================================================================== */
/*=================================================================== */
  /**
	        @brief     CHB_EVENT_FILE

   */
/*=================================================================== */
#define CHB_EVENT_FILE     ACE_TEXT(("/tmp/CHB_Event.log"))

/*===================================================================
   ROUTINE: ACS_CHB_AEH
=================================================================== */
ACS_CHB_AEH::ACS_CHB_AEH()
{
}// End of constructor

/*===================================================================
   ROUTINE: ReportEvent
=================================================================== */
// Send a event/alarm to AEH or a logfile
bool ACS_CHB_AEH::ReportEvent(const ACE_UINT32  EventNumber,
                              const ACE_TCHAR * Severity,
                              const ACE_TCHAR * Cause,
                              const ACE_TCHAR * ObjectOfReference,
                              const ACE_TCHAR * Data,
                              const ACE_TCHAR * Text,
                              bool bReportToAEH)
{
	//ACE_TCHAR szProcName[64];
	ACE_TCHAR * szProcName = new ACE_TCHAR[64];
	if (EventNumber == 9030)
	{
		//szProcName = "acs_chbheartbeat";
              ACE_OS::sprintf(szProcName,ACE_TEXT(("%s")),ACE_TEXT("acs_chbheartbeat"));
	}
        else 
        {
	ACE_OS::sprintf(szProcName,ACE_TEXT(("%s:%d")),PROCESS_NAME,ACE_OS::getpid());
        }

	bool bResult = false;

	if (bReportToAEH)
	{

		acs_aeh_evreport * pEvReport = new acs_aeh_evreport();
		DEBUG( 1,"Send Event to AEH with Process Name and its ID: %s ",szProcName);
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

						DEBUG(1,"Eventhandler:ACS_AEH_syntaxError for processname and its Id: %s",szProcName);

						break;

					case ACS_AEH_eventDeliveryFailure:

						DEBUG(1,"ACS_AEH_eventDeliveryFailure for process name and its Id:%s",szProcName);

						break;
					}
				}
				else if(status == ACS_AEH_ok)
				{
					DEBUG(1,"Event/Alarm sent successfully to AEH for the process name and its Id: %s",szProcName);
				}
				delete pEvReport;
				pEvReport = 0;



	}
	//! else  @todo Restore else condition
	{
		//   DEBUG("%s"," Write event to file");
#if 0
		const ACE_TCHAR *pchTime  = ACS_APGCC::formatDateTime(" %04Y-%02m-%02d %02H:%02M:%02S \n");

		FILE* eventfile = ACE_OS::fopen(ACE_TEXT(CHB_EVENT_FILE), ACE_TEXT("a"));

		if (!eventfile)
			return false;


		ACE_TCHAR szAlarmData[1024];

		ACE_OS::snprintf(szAlarmData,(sizeof(szAlarmData)/sizeof(*szAlarmData) )- 1,
			"%s\nProcessName: %s\nSpecificProblem: %lu\nSeverity: %s\nProbableCause: %s\nObjectClassOfReference: APZ"\
						"\nObjectOfReference: %s\n"\
						   "ProblemData: %s\n"\
						   "ProblemText: %s\n\n",
							   pchTime,szProcName,EventNumber,Severity,Cause,ObjectOfReference,Data,Text);


		fwprintf(eventfile, ACS_APGCC::toWide(pchTime));
		fwprintf(eventfile, ACS_APGCC::toWide("\nProcessName: %s\r\n"), szProcName);
		fwprintf(eventfile, ACS_APGCC::toWide("SpecificProblem: %lu\r\n"), EventNumber);
		fwprintf(eventfile, ACS_APGCC::toWide("Severity: %s\r\n"), Severity);
		fwprintf(eventfile, ACS_APGCC::toWide("ProbableCause: %s\r\n"), Cause);
		fwprintf(eventfile, ACS_APGCC::toWide("ObjectClassOfReference: APZ\r\n"));
		fwprintf(eventfile, ACS_APGCC::toWide("ObjectOfReference: %s\r\n"), ObjectOfReference);
		fwprintf(eventfile, ACS_APGCC::toWide("ProblemData: %s\r\n"), Data);
		fwprintf(eventfile, ACS_APGCC::toWide("ProblemText: %s\r\n\r\n"), Text);

		fflush(eventfile);
		fclose(eventfile);
#endif
		bResult = true;
	}
	delete [] szProcName;
	return bResult;
}//End of ReportEvent

