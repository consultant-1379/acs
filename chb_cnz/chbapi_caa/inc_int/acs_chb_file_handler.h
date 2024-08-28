/*=================================================================== */
/**
	@file		acs_chb_file_handler.h

	@brief		Header file for MTZ  module.

				This module contains all the declarations useful to
				specify the class.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/12/2010     NS       Initial Release
==================================================================== */
//******************************************************************************
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_filehandler_H
#define ACS_CHB_filehandler_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */

#include <acs_chb_mtz_common.h>
#include <ACS_CC_Types.h>
#include <acs_apgcc_omhandler.h>
#include <acs_chb_ntzone.h>
#include <ace/File_Lock.h>
#include <ace/ACE.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Mutex.h>	
#include <ACS_TRA_trace.h>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;


/*=====================================================================
					FORWARD DECLARATION SECTION
==================================================================== */
class ACS_CHB_NtZone ;
/*=====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/**
 *	@brief	cpTime
 *
 */
typedef ACE_INT32 cpTime;
/*=================================================================== */
/**
   @brief		Map of int, ACS_CHB_NtZone

 */
/*=================================================================== */

typedef map<cpTime, ACS_CHB_NtZone> FileMapType;

/*=================================================================== */
/**
   @brief		Pair of int, ACS_CHB_NtZone

 */
/*=================================================================== */

typedef pair<cpTime, ACS_CHB_NtZone> FilePairType;


/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**   
	@brief		ACS_CHB_filehandler

				This class includes functions to get, delete, add timezones into
				IMM and also helps to read/write timezones and CPTime into
				ACS_Tz_links.cfg file on the data disk.
*/
/*=================================================================== */
class ACS_CHB_filehandler
{
/*===================================================================
					PRIVATE DECLARATION SECTION
=================================================================== */
	private:
#if 0
// COMMENTED TO RESOLVE CPPCHECK ERRORS
/*=================================================================== */
/**

	@brief		Removes trailing blanks in the string.

	@param		String
				Input String whose trailing blanks are to be removed.

	@return		bool

*/
/*=================================================================== */
	bool RemoveTrailingBlank(char *String);
#endif
/*=================================================================== */
/**
	@brief		theRecursiveMutex

				This is the guard of the FileMap. When accessing the
				FileMap, all code have to signal the Mutex to avoid
				conflicts with other threads.
*/
/*=================================================================== */
	ACE_Recursive_Thread_Mutex  theRecursiveMutex;
/*=================================================================== */
/**
	@brief		theReadWriteMutex

				This is the guard of the ACE_Heap_COnfiguration when accessing the
				the ACS_TZ_links.cfg file for reading or writing data.
				Avoids conflicts with other threads.
*/
/*=================================================================== */
	static ACE_Mutex  theReadWriteMutex;

/*=================================================================== */
/**
	@brief		FileMap

				The filemap use to handle conversion between cpTime and NT timezone.
				The map use cpTime as key to find the ACS_CHB_NtZone for that key.
				The map assumes that all key are unique, i.e. cpTime can not be duplicate.
				This map is read from disk when the disk changes.
*/
/*=================================================================== */

	FileMapType FileMap;

/*=================================================================== */
/**

	@brief		This function returns the TIME_ZONE_INFORMATION
				structure for a given Time zone name.

	@param		TZString	//IN
				Name of time zone.

	@param		tzi
				TIME_ZONE_INFORMATION structure.

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool getTzi(const ACE_TCHAR* TZString, TIME_ZONE_INFORMATION* tzi);

/*=================================================================== */
/**
	@brief          datadiskPath

*/
/*=================================================================== */
	char datadiskPath[PATHLENGTH];

/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:


/*=====================================================================
						CLASS CONSTRUCTORS
==================================================================== */
/**

	@brief		Constructor for ACS_CHB_filehandler class.


*/
/*=================================================================== */

	ACS_CHB_filehandler();
/*===================================================================
						CLASS DESTRUCTOR
=================================================================== */
/**

	@brief		Destructor for ACS_CHB_filehandler class.


*/
	~ACS_CHB_filehandler();
	/*=================================================================== */
	/**

	        @brief          This function validates whether a given time zone name is
	                                valid or not.

	        @param          TimeZone
	                                Name of time zone.

	        @return         bool
	                                Success/Failure
	*/
	/*=================================================================== */
	        bool validateTimeZone(ACE_TCHAR* TimeZone);

	/*=================================================================== */
	/**

		@brief		This function writes the time zone from the pOneMap to the
					ACS_TZ_links.cfg file in data disk.

		@param		pOneMap
					Map containing time zone information.

		@return		bool
					Success/Failure
	*/
	/*=================================================================== */

	bool WriteZoneFile(FileMapType* pOneMap);

/**

	@brief		This function writes the CP Time and updated CP time
				into ACS_TZ_links.cfg file in the data disk.

	@param		CPtimeSec
				CP Time in time_t format.

	@param		writtenSec
				Updated CP Time in time_t format.

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool WriteZoneFileCPtime(time_t& CPtimeSec, time_t& writtenSec);

/**

	@brief		This function reads the time zones from the ACS_TZ_links.cfg file
				and stores them into the map.

	@param		pOneMap
				FileMap in which time zones are stored.

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool ReadZoneFile(FileMapType* pOneMap = NULL);

/*=================================================================== */
/**

	@brief		This function reads the time zones from IMM and store them into the
				map.

        @param         omHandler

	@param		pOneMap
				FileMap in which time zone are stored.

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool ReadTZsFromIMM(OmHandler &omHandler, FileMapType* pOneMap = NULL);

/**

	@brief		This function reads the CP time from the ACS_TZ_links.cfg file
				from the data disk.

	@param		CPtimeSec
				CP time.

	@param		written
				Updated CP Time

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool ReadZoneFileCPtime(time_t& CPtimeSec, time_t& written);

/**

	@brief		This function gives the TIME_ZONE_INFORMATION for a
				time zone number.

	@param		lookUpTime
				Time zone Number.

	@param		NTTime
				TIME_ZONE_INFORMATION structure.

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool GetNTZone(const cpTime lookUpTime, TIME_ZONE_INFORMATION& NTTime);

/**

	@brief		This function checks whether a particular time zone exists in
				linux database or not. It is present, it returns the
				time zone information in Tzi.

	@param		TimeZone
				Name of time zone.

	@param		Tzi
				TIME_ZONE_INFORMATION

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool CheckTimeZone(const char* TimeZone, TIME_ZONE_INFORMATION* Tzi);

/**

	@brief		This function removes the time zone information corresponding to a
				time zone number from IMM.

        @param         omHandler

	@param		TMZ_value
				Time Zone number.

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool RemoveTZFromIMM(OmHandler& omHandler, ACE_INT32 TMZ_value);

/*=================================================================== */
/*=================================================================== */
/**

	@brief		This function adds the Time zone name corresponding
				to a time zone number in IMM.
	@param         omHandler

	@param		TMZ_value
				Time zone number. It should be >= 0 and < 24.

	@param		TZ_name
				Name of Time zone.

	@return		bool
				Success/Failure
*/
/*=================================================================== */

	bool AddTZToIMM(OmHandler& omHandler, ACE_INT32 TMZ_value, ACE_TCHAR* TZ_name);

/*=================================================================== */
/**

	@brief		This function updates the Time zone name corresponding
				to a time zone number in IMM.

        @param         omHandler

	@param		TMZ_value
				Time zone number. It should be >= 0 and < 24.

	@param		TZ_name
				Name of Time zone.

	@return		bool
				Success/Failure
*/
/*=================================================================== */
	bool UpdateTZIMM(OmHandler& omHandler, ACE_INT32 TMZ_value, ACE_TCHAR* TZ_name);

	/**
	 * @brief	This function will validate a string. it is contains
	 * 			characters other than digits, it return false else
	 * 			it returns true.
	 *
	 * @param	strTMZValue
	 *
	 * @return	bool
	 * 			true/false.
	 */
	bool ValidateTMZNumber( char* strTMZValue);
/*=================================================================== */
	//TR_HT72009 Changes Begin

	int getMonth(string tmp);

	time_t CalculateSec(struct tm st);

	//TR_HT72009 Changes End

	int timeZone_offset(const char* TZOrigString);
/*=================================================================== */

};
#endif

