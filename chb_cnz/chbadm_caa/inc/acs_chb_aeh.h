
/*=================================================================== */
/**
   @file   acs_chb_aeh.h

   @brief Header file for CHB module.

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
   N/A       11/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_AEH_H
#define ACS_CHB_AEH_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Log_Msg.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Brief Description of CAUSE_SYSTEM_ANALYSIS

 */
/*=================================================================== */
#define CAUSE_SYSTEM_ANALYSIS    ACE_TEXT("AP SYSTEM ANALYSIS")
/*=================================================================== */
/**
   @brief Brief Description of CAUSE_AP_FAULT

 */
/*=================================================================== */
#define CAUSE_AP_INTERNAL_FAULT  ACE_TEXT("AP INTERNAL FAULT")
/*=================================================================== */
/**
   @brief Brief Description of CAUSE_AP_FAULT

 */
/*=================================================================== */
#define CAUSE_AP_FAULT           ACE_TEXT("AP FAULT")
/*=================================================================== */
/**
   @brief Brief Description of PROCESS_NAME

 */
/*=================================================================== */
#define PROCESS_NAME             ACE_TEXT("ACS_CHBBIN")

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_AEH


*/
/*=================================================================== */

class ACS_CHB_AEH
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

	      @brief           Constructor for ACS_CHB_AEH class.

	                       Not allowed to instantiate an object of this class.


	   */
	/*=================================================================== */

   ACS_CHB_AEH();

public:
   /*===================================================================
   					PUBLIC DECLARATION SECTION
   =================================================================== */
   /*===================================================================
                          PUBLIC METHOD
   =================================================================== */
   /*=================================================================== */
      /**
         @brief       This function should be used for event reporting. This internally used

         @param       EventNumber

	 @param       Severity

	 @param       Cause

	 @param       ObjectOfReference

	 @param       Data

         @param       Text

         @param       bReportToAEH

         @return      bool
					  true/false

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
