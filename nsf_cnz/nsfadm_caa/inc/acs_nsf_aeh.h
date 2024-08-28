/*=================================================================== */
/**
   @file   acs_nsf_aeh.h

   @brief Header file for NSF module.

          This module contains all the declarations useful to
          specify the ACS_NSF_AEH class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.

==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_NSF_AEH_H
#define ACS_NSF_AEH_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_nsf_common.h"
/*=====================================================================
                          DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief System cause analysis definition
 */
/*=================================================================== */
#define CAUSE_SYSTEM_ANALYSIS    ACE_TEXT("AP SYSTEM ANALYSIS")

/*=================================================================== */
/**
   @brief AP Internal fault cause definition
 */
/*=================================================================== */
#define CAUSE_AP_INTERNAL_FAULT  ACE_TEXT("AP INTERNAL FAULT")

/*=================================================================== */
/**
   @brief AP Fault cause definition
 */
/*=================================================================== */
#define CAUSE_AP_FAULT           ACE_TEXT("AP FAULT")

/*=================================================================== */
/**
   @brief Process name definition
 */
/*=================================================================== */
#define PROCESS_NAME             ACE_TEXT("ACS_NSF_Monitor")

/*=================================================================== */
/**
   @brief NSF Event file location
 */
/*=================================================================== */
#define NSF_EVENT_FILE     ACE_TEXT(("/tmp/NSF_Event.log"))

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @class     ACS_NSF_AEH

      @brief     This class implements the NSF AEH event handler. This class
		 uses APGCC common classes for event reporting and to raise
		 alarms.
 */
/*=================================================================== */

class ACS_NSF_AEH
{

	/*=====================================================================
        	                PROTECTED DECLARATION SECTION
	==================================================================== */
protected:

        /*=====================================================================
                                        CLASS CONSTRUCTOR
        ==================================================================== */
        /*=================================================================== */
        /**
              @brief     Constructor for ACS_NSF_AEH class

                             Not allowed to instanciate an object of this class.

        */
        /*=================================================================== */
   	ACS_NSF_AEH();

        /*=====================================================================
                                        PUBLIC DECLARATION SECTION
        ==================================================================== */
public:
        /*===================================================================
                                   PUBLIC METHODS
        =================================================================== */
        /*=================================================================== */
        /**
                @brief     This function should be used for event reporting.

                @par       Deprecated
                           never

                @param     EventNumber
				Event Number.
		
                @param     Severity
				Severity.

                @param     Cause
				Cause.

                @param     ObjectOfReference
				The object of reference.

                @param     Data
				Data

                @param     Text
				Problem text.

                @param     bReportToAEH
				Report to AEH flag.

                @return    Return status
				True	Success
				False	Failure

                @exception none
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
