/**@file Cute_ACS_LOGM__Service.h
 * @brief header file for Cute_ACS_LOGM__Service.cpp
 * This file consists declaration of cute test class for ACS_LOGM_SVC_Loader
 * @version 1.1.1
 */
#ifndef _CUTE_ACS_LOGM_SERVICE_H_
#define _CUTE_ACS_LOGM_SERVICE_H_
/*Include section*/
/*inclusion of ACE headers*/
#include <ace/ACE.h>
#include <ace/Event_Handler.h>
#include <ace/Synch.h>
/*Inclusion of LOGM block and cute headers*/

#include "cute_suite.h"
/*defining macro*/
#define NO_OF_HANDLES      1

/* Forward declaration */
class LOGM_Service_Loader ;

/**
 * @brief Cute_ACS_LOGM__Service
 * Is a class which consists test functions to test ACS_LOGM_SVC_Loader
 */
class Cute_ACS_LOGM_Service
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_ACS_LOGM_Service();

	/**
	 * @brief
	 *static test function to create instances of ACE_Event
	 * No arguments
	 * @return void
	 *
	 */
	static void vCuteLOGMInit();
	/**
	 * @brief
	 *static test function to delete instances of ACE_Event
	 * No arguments
	 * @return void
	 *
	 */
	static void vCuteLOGMCleanUp();

	/**
	 * @brief
	 *static test function to test LOGM_Service_Loader::startThread() function
	 * No arguments
	 * @return void
	 *
	 */
	static void vTestStartThreadFunction();

	/**
	 * @brief
	 *static test function to test LOGM_Service_Loader::parseArgs() function
	 * No arguments
	 * @return void
	 *
	 */
	static void vTestParseArgs();

	/**
	 * @brief
	 *static test function to test LOGM_Service_Loader::getProcessLock() function
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
	 *static function to return the pointer reference of LOGM_Service_Loader class
	 * No arguments
	 * @return LOGM_Service_Loader
	 */
	static LOGM_Service_Loader *getPoServiceLoader()
	{
		return m_poServiceLoader;
	}/* end LOGM_Service_Loader */

	/**
	 * @brief
	 *static function to make the test suite
	 * No arguments
	 * @return cute::suite
	 * return type is of type cute::suite
	 */
	static cute::suite make_suite_Cute_ACS_LOGM_Service();

/* private variable declaration */
private:
	/**
	* @brief
	* static reference of LOGM_Service_Loader
	*/
	static LOGM_Service_Loader *m_poServiceLoader;

	/**
	* @brief
	* static reference of ACE_Event
	*/
	static ACE_Event *m_plhEventHandles[NO_OF_HANDLES];
};/* end Cute_ACS_LOGM_Service class declaration */


#endif
