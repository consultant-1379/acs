/*=================================================================== */
/**
   @file   acs_chb_clock_def.h

   @brief Header file for CHB module.

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

#ifndef ACS_CHB_CLOCK_DEF_H
#define ACS_CHB_CLOCK_DEF_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_chb_event.h>

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Boolean
 */
/*=================================================================== */
enum Boolean {False, True}; 

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Name of the clock synchronizer process when running as a service
 */
/*=================================================================== */

const char ACS_CHB_ServiceExePath [] =	"ACS_CHB_ClockSyncService.exe";

/*=================================================================== */
/**
   @brief Clock sync service name
 */
/*=================================================================== */
const char ACS_CHB_ServiceName[] = "ACS_CHB_ClockSyncService";

/*=================================================================== */
/**
   @brief Name of the application
 */
/*=================================================================== */
const char ACS_CHB_Clock [] =		"CLOCKSYNC";

/*=================================================================== */
/**
   @brief Event code. System call failed.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_SysCallFailed =         9001;   

/*=================================================================== */
/**
   @brief Event code. AP clock and CP clock are not synchronized, APZ alarm.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ClockNotSync =          9006;   

/*=================================================================== */
/**
   @brief Event code. Just to notify that service has closed down.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ClockSyncStopping =     9007;   

/*=================================================================== */
/**
   @brief Event code. Clock reading from the CP is not reliable.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ClockUnreliable =       9008;   

/*=================================================================== */
/**
   @brief Event code. Unable to start clock sync as service
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_StartSvcFail =          9011;   

/*=================================================================== */
/**
   @brief Parameter name. This parameter states the number of seconds
	  between two requests to synchronize the AP system clock.
 */
/*=================================================================== */
const char
ACS_CHB_cycleTime [] =          "cycleTime";

/*=================================================================== */
/**
   @brief Parameter name. This parameter states the maximum number of 
          microseconds that a request to read the CP clock may take unless
	  the result is discarded.	  
 */
/*=================================================================== */
const char
ACS_CHB_maxCallTime [] =        "maximumCalltime";

/*=================================================================== */
/**
   @brief Parameter name. This parameter states the minimum number of 
	  microseconds that the CP clock and the AP clock may differ in 
	  order to adjust the time. 
 */
/*=================================================================== */
const char
ACS_CHB_minDeviation [] =       "minimumDeviation";

/*=================================================================== */
/**
   @brief Parameter name. This parameter states the maximum number of
	  seconds that the CP clock and the AP clock may differ before
	  an APZ alarm is issued.
 */
/*=================================================================== */
const char
ACS_CHB_maxDeviation [] =       "maximumDeviation";

/*=================================================================== */
/**
   @brief Lengh in bytes of data buffer.
 */
/*=================================================================== */
const short BuffLength =                    7;

/*=================================================================== */
/**
   @brief Maximum time in seconds to wait for a connect response.
 */
/*=================================================================== */
const unsigned short InitTimeLimit =                 10;

/*=================================================================== */
/**
   @brief Maximum time in seconds to wait for a data indication.
 */
/*=================================================================== */
const unsigned short DataTimeLimit =                 30;

/*=================================================================== */
/**
   @brief Event code. Fault detected when connecting to remote application.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem ACS_CHB_ConnectFault =          9003;
	
/*=================================================================== */
/**
   @brief Micro second resolution
 */
/*=================================================================== */
const long int MICROSEC =	1000000;

/*=================================================================== */
/**
   @brief Creating this file on the exe-path of ACS_CHB_Clock.exe will 
	  cause it to die gracefully, deleting the file as it does so.
 */
/*=================================================================== */
const char KillerFile[] = "ClockSync.kill"; 
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  eventName

 */
/*=================================================================== */
#define eventName "daEventWeHaveAllBeenWaitingFor"

/*=================================================================== */
/**
   @brief	CLOCKSYNC_PARAM_IMPL_NAME
*/
/*=================================================================== */
#define CLOCKSYNC_PARAM_IMPL_NAME "ClockSyncIMMParamImpl"
/*=================================================================== */
/**
   @brief	CLOCKSYNC_PARAM_OBJ_DN
*/
/*=================================================================== */
#define CLOCKSYNC_PARAM_OBJ_DN	"clockSyncId=1"
/*=================================================================== */

#endif
