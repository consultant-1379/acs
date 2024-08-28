/*=================================================================== */
/**
   @file   acs_logm_aeh.h

   @brief Header file for LOGM module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
 */

/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       18/08/2010   XKUSATI   Initial Release
==================================================================== */




/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_LOGM_AEH_H
#define ACS_LOGM_AEH_H


/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief CAUSE_SYSTEM_ANALYSIS
 */
/*=================================================================== */
#ifndef CAUSE_SYSTEM_ANALYSIS
#define CAUSE_SYSTEM_ANALYSIS    ACE_TEXT("AP SYSTEM ANALYSIS")
#endif /* !defined CAUSE_SYSTEM_ANALYSIS */

/*=================================================================== */
/**
   @brief CAUSE_AP_INTERNAL_FAULT
 */
/*=================================================================== */
#ifndef CAUSE_AP_INTERNAL_FAULT
#define CAUSE_AP_INTERNAL_FAULT  ACE_TEXT("AP INTERNAL FAULT")
#endif /* !defined CAUSE_AP_INTERNAL_FAULT */

/*=================================================================== */
/**
   @brief CAUSE_AP_FAULT
 */
/*=================================================================== */
#ifndef CAUSE_AP_FAULT
#define CAUSE_AP_FAULT           ACE_TEXT("AP FAULT")
#endif /* !defined CAUSE_AP_FAULT */

/*=================================================================== */
/**
   @brief PROCESS_NAME
 */
/*=================================================================== */
#ifndef PROCESS_NAME
#define PROCESS_NAME             ACE_TEXT("ACS_ACSC_Logmaint")
#endif /* !defined PROCESS_NAME */





/*=====================================================================
					CLASS DECLARATION SECTION
=======================================================================*/

/*=====================================================================*/
/**
	@brief		ACS_LOGM_AEH

				Implementation class for LOGM AEH event handler
 *
 * 				This class uses APGCC common classes for event reporting and
 * 				to raise alarms
 */
/*======================================================================= */

class ACS_LOGM_AEH
{

	/*====================================================================
				   PROTECTED DECLARATION SECTION
	====================================================================== */
protected:

	/*=====================================================================
				   CLASS CONSTRUCTORS
	======================================================================= */
	/**

		@brief		Constructor for ACS_LOGM_AEH class.


	 */
	/*===================================================================== */
	ACS_LOGM_AEH();


	/*=====================================================================
					CLASS DESTRUCTOR
	=========================================================================== */
	/**

	@brief		Destructor for ACS_LOGM_AEH class.


	 */
	~ACS_LOGM_AEH();

	/*=====================================================================
	                PUBLIC DECLARATION SECTION
	=========================================================================== */

public:

	/*=================================================================== */
	/**
        @brief     	ReportEvent
			This function should be used for event reporting. This internally used

        @pre       	none

        @post     	none

		@param 		EventNumber

		@param 		Severity

		@param 		Cause

        @param 		ObjectOfReference

        @param 		Data

        @param 		Text

        @param 		bReportToAEH

        @return		bool

        @exception 	none
	 */
	/*=================================================================== */
	static bool ReportEvent(const ACE_UINT32 EventNumber,
			const ACE_TCHAR* Severity,
			const ACE_TCHAR* Cause,
			const ACE_TCHAR* ObjectOfReference,
			const ACE_TCHAR* Data,
			const ACE_TCHAR* Text,
			bool bReportToAEH = true);
};

#endif
