
/*=================================================================== */
   /**
   @file acs_logm_aeh.cpp

   Class method implementation for CHB module.

   This module contains the implementation of class declared in
   the acs_logm_aeh.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       19/08/2010   XKUSATI   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ace/Log_Msg.h>
#include <ACS_APGCC_Event.H>
#include <ACS_APGCC_Util.H>
#include <acs_logm_aeh.h>
#include <acs_logm_common.h>


using namespace std;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define LOGM_EVENT_FILE     ACE_TEXT(("/tmp/LOGM_Event.log"))

//static bool ReportToAEH = false;


/*=====================================================================
			   CLASS CONSTRUCTORS
======================================================================= */
ACS_LOGM_AEH::ACS_LOGM_AEH()
{
}

/*===================================================================
   ROUTINE: ReportEvent
=================================================================== */
// Send a event/alarm to AEH or a logfile
bool ACS_LOGM_AEH::ReportEvent(const ACE_UINT32  EventNumber,
                              const ACE_TCHAR * Severity,
                              const ACE_TCHAR * Cause,
                              const ACE_TCHAR * ObjectOfReference,
                              const ACE_TCHAR * Data,
                              const ACE_TCHAR * Text,
                              bool bReportToAEH)
{
	ACS_APGCC_Event oGCCEvent;
	ACE_TCHAR szProcName[64];
    ACE_OS::sprintf(szProcName,ACE_TEXT(("%s:%d")),PROCESS_NAME,ACE_OS::getpid());

   bool bResult = false;

   if (bReportToAEH)
   {
	   DEBUG("%s \n"," Sending Event to AEH ... \n");
	   bResult = oGCCEvent.raise(EventNumber,
                                      Severity,
                                      ObjectOfReference,
                                      Text,
                                      Cause,
                                      ACE_TEXT("APZ"),
                                      Data
                                     );

   }
   //! else  
   {
	   DEBUG("%s"," Write event to file");

      const ACE_TCHAR *pchTime  = ACS_APGCC::formatDateTime(" %04Y-%02m-%02d %02H:%02M:%02S \n");


      FILE* eventfile = ACE_OS::fopen(ACE_TEXT(LOGM_EVENT_FILE), ACE_TEXT("a"));

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

      bResult = true;
   }

   return bResult;
}//End of ReportEvent

