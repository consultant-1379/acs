/**@file Cute_ACS_LOGM_SrvParam.h
 * @brief header file for Cute_ACS_LOGM_SrvParam.cpp
 * This file consists declaration of cute test class for acs_logm_srvparam_oihandler
 * @version 1.1.1
 */
#ifndef _CUTE_ACS_LOGM_SRV_PARAM_H
#define _CUTE_ACS_LOGM_SRV_PARAM_H
/*Include section*/
#include <ace/ACE.h>
#include <ace/Event_Handler.h>
#include <ace/Synch.h>
#include "cute_suite.h"
#include "acs_logm_srvparam_oihandler.h"

/**
 * @brief Cute_ACS_LOGM_SrvParam
 * Is a class which consists test functions to test acs_logm_srvparam_oihandler
 */
class Cute_ACS_LOGM_SrvParam
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
	static void vTestModifysrvTrgHour();
	static void vTestModifysrvTrgMin();
	static void vTestModifysrvTrgFreq();
	static void vTestModifysrvLogFileCount();
	static void vTestModifysrvStsDel();


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
	static cute::suite make_suite_Cute_ACS_LOGM_SrvParam();

/* private variable declaration */
private:
	/**
	* @brief
	* static reference of acs_logm_srvparam_oihandler
	*/
	static acs_logm_srvparam_oihandler *m_poSrvParamOiHandler;

};/* end Cute_ACS_LOGM_SrvParam class declaration */


#endif
