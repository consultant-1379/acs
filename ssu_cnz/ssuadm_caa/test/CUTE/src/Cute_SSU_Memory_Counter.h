/**
 * @file Cute_SSU_Memory_Counter.h
 * @brief Header file for Cute_SSU_Memory_Counter.cpp
 * This header file contains declaration of the cute test class for ACS_SSU_Memory_Counter.
 *@version 1.1.1
 */
#ifndef _CUTE_SSU_MEMORY_COUNTER_H_
#define _CUTE_SSU_MEMORY_COUNTER_H_
#include "cute_suite.h"

class ACS_SSU_Memory_Counter;

/**
 * @brief Cute_SSU_Memory_Counter
 * Cute test class having test functions to test ACS_SSU_Memory_Counter
 */
class Cute_SSU_Memory_Counter{
public:
	/**
	* @brief
	* Default constructor
	* No arguments
	* @return void
	*/
	Cute_SSU_Memory_Counter();

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

	static void bTestUpdatePerformanceCounters();
	static void vTestCheckAndRaiseMemoryAlarm();
	/**
	* @brief
	* It is a static function to test checking the performance data and ceasing alarm
	*  No arguments
	* @return void
	*/
	static void vTestCheckAndCeaseMemoryAlarm();
	/**
	* @brief
	* It is a static function to delete the memory counter object pointer
	*  No arguments
	* @return void
	*/
	static void vMemoryCounterCleanup();
	/**
	* @brief
	* It is a static function to get the Memory counter object pointer
	*  No arguments
	* @return void
	*/
	 static ACS_SSU_Memory_Counter * getPoMemoryCounter();
	 /**
	 * @brief
	 * It is a static function to make test suite for the Cute_SSU_Memory_Counter class
	 * No arguments
	 * @return cute::suite
	 */
	 	static cute::suite make_suite_Cute_SSU_FileHandle_Counter();

     static cute::suite make_suite_Cute_SSU_Memory_Counter();

private:
     static ACS_SSU_Memory_Counter * m_pOMemoryCounter;
};
#endif
