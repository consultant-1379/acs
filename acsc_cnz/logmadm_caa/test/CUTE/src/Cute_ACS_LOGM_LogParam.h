/**@file Cute_ACS_LOGM_LogParam.h
 * @brief header file for Cute_ACS_LOGM_LogParam.cpp
 * This file consists declaration of cute test class for acs_logm_srvparam_oihandler
 * @version 1.1.1
 */
#ifndef _CUTE_ACS_LOGM_LOG_PARAM_H
#define _CUTE_ACS_LOGM_LOG_PARAM_H
/*Include section*/
#include <ace/ACE.h>
#include <ace/Event_Handler.h>
#include <ace/Synch.h>
#include "cute_suite.h"
#include "acs_logm_logparam_oihandler.h"

/**
 * @brief Cute_ACS_LOGM_LogParam
 * Is a class which consists test functions to test acs_logm_srvparam_oihandler
 */
class Cute_ACS_LOGM_LogParam
{
public:
	static void init();

        static void destroy();

	/**
	 * @brief vTestCreate
	 *
	 * No arguments
	 * @return void
	 *
	 */
	static void vTestCreate();
	/**
	 * @brief vTestModify
	 *
	 * No arguments
	 * @return void
	 *
	 */

	static void vTestModifylogName();
	static void vTestModifylogPath();
	static void vTestModifylogType();
	static void vTestModifylogIOOperation();
	static void vTestModifylogNoOfDays();
	/**
	 * @brief vTestDelete
	 *
	 * No arguments
	 * @return void
	 *
	 */
	static void vTestDelete();

	/**
	 * @brief
	 *static function to make the test suite
	 * No arguments
	 * @return cute::suite
	 * return type is of type cute::suite
	 */
	static cute::suite make_suite_Cute_ACS_LOGM_LogParam();

/* private variable declaration */
private:
	/**
	* @brief
	* static reference of acs_logm_srvparam_oihandler
	*/
	static acs_logm_logparam_oihandler *m_poLogParamOiHandler;

};/* end Cute_ACS_LOGM_LogParam class declaration */


#endif
