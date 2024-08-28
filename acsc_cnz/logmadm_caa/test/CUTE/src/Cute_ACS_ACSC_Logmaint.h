/**
 * @file Cute_ACS_ACSC_Logmaint.h
 * @brief header file for Cute_ACS_ACSC_Logmaint.cpp
 * It consists declaration of the cute test class for ACS_ACSC_Logmaint
 * @version 1.1.1
 */

#ifndef CUTE_ACS_ACSC_LOGMAINT_H_
#define CUTE_ACS_ACSC_LOGMAINT_H_

#include "acs_logm_types.h"

/*Include Section */
/* Inclusion of cute libraries */
#include "cute_suite.h"

class ACS_ACSC_Logmaint;

/**
 * @brief Cute_ACS_ACSC_Logmaint
 * Cute test class having test functions to test ACS_ACSC_Logmaint
 */
class Cute_ACS_ACSC_Logmaint
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_ACS_ACSC_Logmaint();

	/**
	 * @brief
	 *static test function to test ACS_ACSC_Logmaint::bgetDiskMonitoringSettings()
	 * No arguments
	 * @return void
	 *
	 */
	static void vTestGetLogFileParams();
	/**
	 * @brief
	 * static test function to test ACS_ACSC_Logmaint::bGetDiskData()
	 * No arguments
	 * @return void
	 *
	 */
	static void vTestGetServiceparams();

	/**
	 * @brief
	 * static test function to test ACS_ACSC_Logmaint::bRemoveSmallPartitions()
	 * No arguments
	 * @return void
	 *
	 */
	static void vGetInitialTimeToWait();
	static void vGetPHAParameterInt();
	static void vGetPHAParameterString();
	static void vFindFirstFile();
	static void vFindNextFile();
	static void vpOCleanup();
	static void vTestisDirectory();
	static void vTestgetCurrentTime();
	static void vTestElapsedTimeCal();
	static void vTestisWildCard();
	static void vTestwildCardMatch();
	static void vTestremovedirectoryrecursively();
	static void vTestextractFileNameFrompath();
	static void vTestfindPathSearchPatternFromWildCardDirectory();
	static void vTestsearchPathFromWildCard();
	static void vTeststringReverse();
	static void vTestFindFileAndFolders();
	static void vTestDeleteLogFilesFTLOG();
	static void vTestDeleteLogFilesSTSMP();
	static void vTestDeleteLogFilesSTSMPDummy();
	static void vTestDeleteEmptySTSMPFolder();
	static void  vTestDeleteLogFilesDDI();
	static void vTestDeleteLogFilesTRA();
	static void vremoveFile();
	static void vgetTimeStampOfFile();
	static void vshowDirectoriesWildCard();
	static void vshowFilesandDirectories();


	/**
	 * @brief
	 * static test function to test ACS_ACSC_Logmaint::bGetSpecificPartitionSettings()
	 * No arguments
	 * @return void
	 *
	 */
	static void vtGetCurrentTime();

	/**
	 * @brief
	 * static function returns pointer reference of ACS_ACSC_Logmaint
	 * No arguments
	 * @return ACS_ACSC_Logmaint
	 *
	 */

	static ACS_ACSC_Logmaint * getPoCleanup();



	/**
	 * @brief
	 * static function to make the test suite for  ACS_ACSC_Logmaint
	 * No arguments
	 * @return void
	 *
	 */
	static cute::suite make_suite_Cute_ACS_ACSC_Logmaint();
//private members declaration
private:
	/**
	 * @brief
	 * its a static reference of type ACS_ACSC_Logmaint
	 */
	static ACS_ACSC_Logmaint *m_poCleanup;


};

#endif /* CUTE_ACS_ACSC_LOGMAINT_H_ */
