/**
 * @file Cute_NSF_Biostime_Recovery.h
 * @brief header file for Cute_NSF_Biostime_Recovery.cpp
 * It consists declaration of the cute test class for ACS_NSF_Biostime_Recovery
 * @version 1.1.1
 */
#ifndef _CUTE_NSF_Biostime_Recovery_H_
#define _CUTE_NSF_Biostime_Recovery_H_
/*Include Section */
/* Inclusion of cute libraries */
#include "cute_suite.h"





/* Forward declaration */
class NSF_Biostime_Recovery;

/**
 * @brief Cute_NSF_Biostime_Recovery
 * Cute test class having test functions to test NSF_Biostime_Recovery_
 */

class Cute_NSF_Biostime_Recovery
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_NSF_Biostime_Recovery();
	~Cute_NSF_Biostime_Recovery();
	/**
					 * @brief
					 *static test function to test NSF_Biostime_Recovery::bCheckForTimestampFile()
					 * No arguments
					 * @return bool
					 *
					 */
	static void vCheckForTimestampFile();
	static void vGetFileTime();
	static void vCheckForPrivileges();
	static void vCheckCurrentTimeWithFileTime();

	static void init();

	static NSF_Biostime_Recovery* getPoBiosTime(){
		return m_poBiosTime;
	}

	static void vBiosTimeCleanup();

	/**
											 * @brief
											 * static function to make the test suite for NSF_Biostime_Recovery 
											 * No arguments
											 * @return void
											 *
											 */
	static cute::suite make_suite_Cute_NSF_Biostime_Recovery();
//private members declaration
private:
	/**
	 * @brief
	 * its a static reference of type NSF_Biostime_Recovery
	 */
	static NSF_Biostime_Recovery *m_poBiosTime;
};

#endif
