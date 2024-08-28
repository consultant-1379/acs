/**@file Cute_SSU_Monitor_Service.h
 * @brief header file for Cute_SSU_Monitor_Service.cpp
 * This file consists declaration of cute test class for ACS_SSU_SVC_Loader
 * @version 1.1.1
 */
#ifndef _CUTE_SSU_MONITOR_SERVICE_H_
#define _CUTE_SSU_MONITOR_SERVICE_H_
/*Include section*/
/*inclusion of ACE headers*/
#include <ace/ACE.h>
#include <ace/Event_Handler.h>
#include <ace/Synch.h>
#include "cute_suite.h"
/*defining macro*/
#define NO_OF_HANDLES      1

/* Forward declaration */
class SSU_Service_Loader ;
class ACS_SSU_AlarmHandler;
/**
 * @brief Cute_SSU_Monitor_Service
 * Is a class which consists test functions to test ACS_SSU_SVC_Loader
 */
class Cute_SSU_Monitor_Service{
public:
	/**
		 * @brief
		 * Default constructor
		 * No arguments
		 * @return void
		 */
	Cute_SSU_Monitor_Service();

	/**
			 * @brief
			 *static test function to create instances of ACS_SSU_AlarmHandler and ACE_Event
			 * No arguments
			 * @return void
			 *
			 */
	static void vCuteSSUInit();
	/**
				 * @brief
				 *static test function to delete instances of ACS_SSU_AlarmHandler and ACE_Event
				 * No arguments
				 * @return void
				 *
				 */
	static void vCuteSSUCleanUp();

	/**
					 * @brief
					 *static test function to test SSU_Service_Loader::startThread() function
					 * No arguments
					 * @return void
					 *
					 */
	static void vTestStartThreadFunction();

	/**
					 * @brief
					 *static test function to test SSU_Service_Loader::SetupFileAndDirectories() function
					 * No arguments
					 * @return void
					 *
					 */
	static void vSetupFilesAndDirectories();

	/**
						 * @brief
						 *static test function to test SSU_Service_Loader::parseArgs() function
						 * No arguments
						 * @return void
						 *
						 */
	static void vTestParseArgs();

	/**
							 * @brief
							 *static test function to test SSU_Service_Loader::getProcessLock() function
							 * No arguments
							 * @return void
							 *
							 */
	static void vTestGetProcessLock();
	/*static void vTestLockFd();
	static void vTestUnlockFd();
	static void vTestBecomeDaemon();*/
	//static void vSetTimeoutForInteractiveMode();

	/**
								 * @brief
								 *static function to return the pointer reference of ACS_SSU_AlarmHandler class
								 * No arguments
								 * @return ACS_SSU_AlarmHandler
								 */
    static ACS_SSU_AlarmHandler *getPAlarmHandler()
    {
        return m_poAlarmHandler;
    }/* end getPAlarmHandler()*/
/**
    								 * @brief
    								 *static function to return the pointer reference of ACE_Event class
    								 * No arguments
    								 * @return ACE_Event
    								 */
    static ACE_Event *getPlhEventHandles()
    {
        return (ACE_Event*)m_plhEventHandles[0];
    }/*end getPlhEventHandles() */

    /**
        								 * @brief
        								 *static function to return the pointer reference of SSU_Service_Loader class
        								 * No arguments
        								 * @return SSU_Service_Loader
        								 */
    static SSU_Service_Loader *getPoServiceLoader()
    {
        return m_poServiceLoader;
    }/* end SSU_Service_Loader */

    /**
           								 * @brief
           								 *static function to make the test suite
           								 * No arguments
           								 * @return cute::suite
           								 * return type is of type cute::suite
           								 */
    static cute::suite make_suite_Cute_SSU_Monitor_Service();

    /* private variable declaration */
private:
    /**
     * @brief
     * static reference of SSU_Service_Loader
     */
	static SSU_Service_Loader *m_poServiceLoader;
	/**
	     * @brief
	     * static reference of ACS_SSU_AlarmHandler
	     */
	static 	ACS_SSU_AlarmHandler *m_poAlarmHandler;
	/**
		     * @brief
		     * static reference of ACE_Event
		     */
	static ACE_Event *m_plhEventHandles[NO_OF_HANDLES];
};/* end Cute_SSU_Monitor_Service class declaration */


#endif
