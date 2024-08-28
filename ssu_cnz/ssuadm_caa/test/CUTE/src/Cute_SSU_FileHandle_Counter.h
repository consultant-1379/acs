/**
 * @file Cute_SSU_Filehandle_Counter.h
 * @brief Header file for Cute_SSU_FileHandle_Counter.cpp
 * This header file contains declaration of the cute test class for ACS_SSU_FileHandle_Counter.
 *@version 1.1.1
 */


#ifndef _CUTE_SSU_FILEHANDLE_COUNTER_H_
#define _CUTE_SSU_FILEHANDLE_COUNTER_H_
#include "cute_suite.h"

class ACS_SSU_FileHandle_Counter;
/**
 * @brief Cute_SSU_FileHandle_Counter
 * Cute test class having test functions to test ACS_SSU_FileHandle_Counter
 */

class Cute_SSU_FileHandle_Counter{
public:
	/**
	* @brief
	* Default constructor
	* No arguments
	* @return void
	*/
	Cute_SSU_FileHandle_Counter();

	/**
	* @brief
	* It is a static function to test parse command output
	*  No arguments
	* @return void
	*/
	static void vTestParseCommandOutput();

	/**
	* @brief
	* It is a static function to test checking the performance data and raising alarm
	*  No arguments
	* @return void
	*/

	static void vTestbUpdatePerformanceCounters();

	static void vTestCheckAndRaiseAlarm();
	/**
	* @brief
	* It is a static function to test checking the performance data and ceasing alarm
	*  No arguments
	* @return void
	*/

	static void vTestCheckAndCeaseAlarm();

	/**
	* @brief
	* It is a static function to delete the FileHandle object pointer
	*  No arguments
	* @return void
	*/

	static void vFileHandleCleanup();

	/**
	* @brief
	* It is a static function to get the FileHandle object pointer
	*  No arguments
	* @return void
	*/
	static ACS_SSU_FileHandle_Counter * getPoFileHandleCounter();

	/**
	* @brief
	* It is a static function to make test suite for the Cute_SSU_Filehandle_Counter class
	* No arguments
	* @return cute::suite
	*/
	static cute::suite make_suite_Cute_SSU_FileHandle_Counter();
    private:

	static ACS_SSU_FileHandle_Counter * m_pOFileHandleCounter;
};

#endif
