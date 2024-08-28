/*=================================================================== */
/**
   @file   acs_ssu_service.h

   @brief Header file for SSU HA module.

          This module contains all the declarations useful to
          specify the ACS_SSU_Service class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/03/2012     XRAMMAT        APG43 on Linux.

==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_SSU_Service_H
#define ACS_SSU_Service_H

#include <string>
#include <vector>
#include <map>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/ace_wchar.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ACS_APGCC_ApplicationManager.h>
#include "acs_ssu_svc_loader.h"
#include <unistd.h>


#if 0
/**
 *      @brief          ThrExitHandler
 *                      Class for handling thread exit.
 *
 */

class ThrExitHandler : public ACE_At_Thread_Exit
{
public:
	/*=================================================================== */
	/**
						@brief                  This method initializes thread exit handler.
						@pre					none
						@post					none
						@return                 none
						@exception              none
	 */
	/*=================================================================== */
	static bool init()
	{
		ThrExitHandler *thrExitHandler = new ThrExitHandler();
		if( thrExitHandler == 0 )
		{
			return false;
		}
		thrExitHandler->setThrStatusInMap(ACE_Thread_Manager::instance()->thr_self(), true);

		int result = ACE_Thread_Manager::instance()->at_exit( thrExitHandler );

		if( result == -1 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	/*=================================================================== */
	/**
					@brief                  This method cleanup thread exit handler.
					@pre					none
					@post					none
					@return                 none
					@exception              none
	 */
	/*=================================================================== */
	static void cleanup()
	{
		theThrMutex.remove();
		theThrStatusMap.clear();
	}

	/*=================================================================== */
	/**
				@brief                  This method checks all threads status in map.
				@pre					none
				@post					none
				@return                 bool
				@exception              none
	 */
	/*=================================================================== */
	static bool CheckAllFuncThreads( void )
	{
		//Lock the mutex.
		theThrMutex.acquire();

		if(theThrStatusMap.empty())
		{
			theThrMutex.release();
			return false;
		}

		//Check if any thread has exited or not.
		for( map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.begin();
				itr != theThrStatusMap.end();  ++itr )
		{
			if( itr->second == false )
			{
				theThrMutex.release();
				return false;
			}
		}
		theThrMutex.release();
		return true;
	}


	virtual void apply(void)
	{
		if( !theThrStatusMap.empty())
		{
			setThrStatusInMap( ACE_Thread_Manager::instance()->thr_self(), false);
		}
	}

private:

	/*=================================================================== */
	/**
			@brief                  This method sets thread status in map.
			@pre					none
			@post					none
			@return                 none
			@exception              none
	 */
	/*=================================================================== */
	void setThrStatusInMap( ACE_thread_t thrId, bool isThrAlive)
	{
		bool isThrPresentInMap = false;

		//Acquire the Mutex
		theThrMutex.acquire();

		//Now update the data in the global map.
		map<ACE_thread_t, bool>::iterator itr =  theThrStatusMap.begin();

		while( itr != theThrStatusMap.end() )
		{
			if( itr->first == thrId )
			{
				isThrPresentInMap = true;
				break;
			}
			++itr;
		}
		if( isThrPresentInMap )
		{
			theThrStatusMap.erase(thrId);
		}
		theThrStatusMap.insert(std::pair<ACE_thread_t, bool>(thrId, isThrAlive));
		theThrMutex.release();
	}

	static map<ACE_thread_t, bool> theThrStatusMap;
	static ACE_Recursive_Thread_Mutex  theThrMutex;
};
#endif

class  ACS_SSU_Service : public ACS_APGCC_ApplicationManager
{
private:
	/**
	@brief          m_ssuApplObj
	*/
	SSU_Service_Loader *m_ssuApplObj;
	/**
	@brief          passiveToActive
	*/
	ACE_UINT32 passiveToActive;
	/**
	* @brief	activateApp
	* @return  ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType activateApp();
	/**
	* @brief	passifyApp
	* @return  ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType passifyApp();
	/**
	* @brief	shutdownApp
	* @return  ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType shutdownApp();

public:

	/**
	* @brief	ACS_SSU_Service
	* @param daemon_name
	* @param username
	* @return none
	*/
	ACS_SSU_Service(const char* daemon_name, const char* username);
	/**
	* @brief	~ACS_SSU_Service
	* @return none
	*/
	~ACS_SSU_Service();
	/**
	* @brief	performStateTransitionToActiveJobs
	* @param previousHAState
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/**
	* @brief	performStateTransitionToPassiveJobs
	* @param previousHAState
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/**
	* @brief	performStateTransitionToQueisingJobs
	* @param previousHAState
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/**
	* @brief	performStateTransitionToQuiescedJobs
	* @param previousHAState
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/**
	* @brief	performComponentHealthCheck
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	/**
	* @brief	performComponentTerminateJobs
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	/**
	* @brief	performComponentRemoveJobs
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	/**
	* @brief	performApplicationShutdownJobs
	* @return ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType performApplicationShutdownJobs();
}; 
#endif /* ACS_SSU_Service_H */

