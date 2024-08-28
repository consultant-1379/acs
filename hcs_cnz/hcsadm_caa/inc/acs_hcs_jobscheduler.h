//========================================================================================
// COPYRIGHT (C) Ericsson AB 2011 - All rights reserved
//
// The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written permission from Ericsson
// AB or in accordance with the terms and conditions stipulated in the agreement/contract
// under which the program(s) have been supplied.
//========================================================================================

#ifndef AcsHcs_jobScheduler_h
#define AcsHcs_jobScheduler_h

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <cstdlib>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <ACS_TRA_trace.h>
#include "acs_hcs_tra.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief jobScheduler
 * jobScheduler scheduler class is used to execute the jobs according to the schedule.
 * It is having functions to spawn a jobscheduler thread
 *
 */
class jobScheduler : public ACE_Task_Base
{
	/*=====================================================================
                                       PUBLIC DECLARATION SECTION
        ==================================================================== */
        public:
		/**
         	*  svc method: This method is used to spawn a thread.
         	*  @return int 
         	*/
                int svc();

		/**
         	*  updateRuntime method: This method is called by svc function.
        	*  @return void 
         	*/
		void scheduledExecution();
		
		 /*=====================================================================
					STATIC VARIABLES DECLARATION
	        ==================================================================== */
		/**
        	* @brief   Initially this value is 'false' and it will be 'true' for thread exiting .
        	*/
		static bool stopRequested;

		/**
                * @brief   Initially this value is 'false' and it will be 'true' once the thread get exited.

                */
                static bool threadExited;

		/**
                * @brief   Initially this value is 'true' and it will be 'false' when there is a job 'ONGOING' when a scheduled job is trying to execute.
                */
		static bool jobExecution;

		/**
                * @brief   Initially this value is 'false' and it will be 'true' if any of the events(single,periodic or calendar) is modified/deleted. so that next scheduled execution will change accordingly.
                */
		static bool eventCallbackFlag;

};

#endif

