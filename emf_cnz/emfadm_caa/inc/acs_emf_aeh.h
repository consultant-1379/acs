/*=================================================================== */
/**
   @file   acs_emf_aeh.h

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
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_EMF_AEH_H
#define ACS_EMF_AEH_H
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
// Event heading
#define EMF_EV_HEADING                 ACE_TEXT("EXTERNAL MEDIA FUNCTION FAULT")

// Process name
#define EMF_EV_PROCNAME                ACE_TEXT("ACS_EMF_Server")

// Event severity's
#define EMF_EV_SEVERITY_EVENT          ACE_TEXT("EVENT")
#define EMF_EV_SEVERITY_A1             ACE_TEXT("A1")
#define EMF_EV_SEVERITY_A2             ACE_TEXT("A2")
#define EMF_EV_SEVERITY_CEASE          ACE_TEXT("CEASE")
#define EMF_EV_OBJ_REFERENCE                 "acs_emfserverd/Alarm"

// Event intervall to be used by EMF is 10400-10499
#define EMF_EV_EVENTNUM_ERROR          10400
#define EMF_EV_EVENTNUM_SOCKSERVERROR  10401
#define EMF_EV_EVENTNUM_INTERNALERROR  10499
#define EMF_EV_FAULTY_MISSING_EVENT_CODE_START_NUM	10402   // Board faulty/missing event code numbers 10720 + slot number

#define EMF_EV_EVENTTEXT_ERROR         ACE_TEXT("EXTERNAL MEDIA FUNCTION ERROR")
#define EMF_EV_EVENTTEXT_FAULT         ACE_TEXT("EXTERNAL MEDIA FUNCTION FAULT")
#define EMF_EV_EVENTTEXT_INTERNALERROR ACE_TEXT("EXTERNAL MEDIA FUNCTION INTERNAL ERROR")
#define EMF_EV_NANOUSB_PROBLEM_DATA    ACE_TEXT("ExternalMedia is Unavailable")
#define EMF_EV_NANOUSB_PROBLEM_DATAFORBOTH ACE_TEXT("Both ExternalMedia are Unavailable")
#define EMF_EV_NANOUSB_PROBLEM_DATAFORPASSIVE    ACE_TEXT("The nanoUSB is missing on passive node")

#define EMF_EV_NANOUSB_PROBLEM_TEXT_HEAD 	ACE_TEXT("PROBLEM                    SLOTNUMBER\n")
#define EMF_EV_NANOUSB_PROBLEM_TEXT_TAIL  	ACE_TEXT("ExternalMedia Unavailable")
#define EMF_EV_NANOUSBFAULTY_PROBLEM_TEXT_TAIL  	ACE_TEXT("NANOUSB FAULTY")
#define EMF_EV_NANOUSBFAULTY_PROBLEM_DATA    "The nanoUSB is faulty"
#define EMF_EV_NANOUSBFAULTY_PROBLEM_DATAFORPASSIVE    "The nanoUSB is faulty on passive node"

#define CAUSE_AP_INTERNAL_FAULT  ACE_TEXT("AP INTERNAL FAULT")
#define CAUSE_AP_FAULT  ACE_TEXT("AP FAULT")

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief    ACS_EMF_AEH
                Implementation class for EMF AEH event handler.This class uses APGCC common classes for event reporting and
				to raise alarms
 */
/*=================================================================== */
class ACS_EMF_AEH
{
	/*=====================================================================
							PROTECTED DECLARATION SECTION
	==================================================================== */
protected:

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**
		  @brief           ACS_EMF_AEH : Not allowed to instanciate an object of this class.

		  @exception       none
	 */
	/*=================================================================== */
	ACS_EMF_AEH();
	/*=====================================================================
							PUBLIC DECLARATION SECTION
	==================================================================== */
public:

	/*===================================================================
                              PUBLIC METHOD
   =================================================================== */
	/*=================================================================== */
	/**
   	  @brief           This function should be used for event reporting. This internally used

   	  @param           EventNumber

   	  @param 		   Severity

      @param 		   Cause

	  @param 		   ObjectOfReference

      @param 		   Data

      @param 		   Text

      @param 		   bReportToAEH

   	  @return          ACE_INT32

   	  @exception       none
	 */
	/*=================================================================== */
	static bool ReportEvent(const ACE_UINT32 EventNumber,
			const ACE_TCHAR* Severity,
			const ACE_TCHAR* Cause,
			const ACE_TCHAR* ObjectOfReference,
			const ACE_TCHAR* Data,
			const ACE_TCHAR* Text);
};

#endif
