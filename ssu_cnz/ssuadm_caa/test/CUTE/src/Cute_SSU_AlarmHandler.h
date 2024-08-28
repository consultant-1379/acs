/**
 * @file Cute_SSU_AlarmHandler.h
 * @brief Header file for Cute_SSU_AlarmHandler.cpp
 * This header file contains declaration of the cute test class for ACS_SSU_AlarmHandler.
 *@version 1.1.1
 */
#ifndef _CUTE_SSU_ALARMHANDLER_H_
#define _CUTE_SSU_ALARMHANDLER_H_

/* include Section */
/* inclusion of Cute libraries */
#include "cute_suite.h"

//forward declaration
class ACS_SSU_AlarmHandler;
/**
 * @brief Cute_SSU_AlarmHandler
 * Cute test class having test functions to test ACS_SSU_AlarmHandler.cpp
 */
class Cute_SSU_AlarmHandler
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_SSU_AlarmHandler();

	/**
			 * @brief
			 * It is a static function to test alarm send function
			 *  No arguments
			 * @return void
			 */
	static void  vTestSendAlarm();
	/**
			 * @brief
			 * It is a static function to test alarm cease cease
			 * No arguments
			 * @return void
			 */
	static void vTestCeaseAlarm();

	static void getPoFileHandleCounter();
	static void vTestSendFolderQuotaAlarm();
	static void  vTestSendFolderQuotaCease();

	/**
		 * @brief
		 * It is a static function to make test suite for the Cute_SSU_AlarmHandler class
		 * No arguments
		 * @return cute::suite
		 */
	static cute::suite make_suite_Cute_SSU_AlarmHandler();

};


#endif
