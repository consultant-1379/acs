/*=================================================================== */
/**
  @file   acs_emf_aeh.cpp

  @brief Header file for EMF module.

  @version 1.0.0


  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module


  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       DD/MM/YYYY   XRAMMAT   Initial Release
 */
/*==================================================================== */

/*=====================================================================
  INCLUDE DECLARATION SECTION
  ==================================================================== */
#include <stdio.h>
#include <iostream>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>

#include "acs_emf_aeh.h"
#include <acs_emf_tra.h>
#include <acs_aeh_evreport.h>
using namespace std;

/*=====================================================================
  DEFINE DECLARATION SECTION
  ==================================================================== */
#define EMF_EVENT_FILE     ACE_TEXT(("/tmp/EMF_Event.log"))

/*===================================================================
ROUTINE: ACS_EMF_AEH
=================================================================== */
ACS_EMF_AEH::ACS_EMF_AEH()
{
}//End of constructor

/*===================================================================
ROUTINE: ReportEvent
=================================================================== */
bool ACS_EMF_AEH::ReportEvent(const ACE_UINT32  EventNumber,
		const ACE_TCHAR * Severity,
		const ACE_TCHAR * Cause,
		const ACE_TCHAR * ObjectOfReference,
		const ACE_TCHAR * Data,
		const ACE_TCHAR * Text)
{
	ACE_TCHAR szProcName[64];
	ACE_OS::sprintf(szProcName,ACE_TEXT(("%s:%d")),EMF_EV_PROCNAME,ACE_OS::getpid());

	bool bResult = false;

	INFO(1,"%s"," Send Event to AEH \n");

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

	return bResult;

}//End of ReportEvent
