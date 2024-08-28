/*=================================================================== */
/**
   @file  acs_aca_ms_statistics_file.h 

   @brief Header file for aca module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACAMS_StatisticsFile_H
#define ACAMS_StatisticsFile_H

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_aca_common.h>
#include<time.h>
#include <ace/ACE.h>

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      ACAMS_StatisticsFile
 */
/*=================================================================== */
class  ACAMS_StatisticsFile {

/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Constructor for ACAMS_StatisticsFile class.

   @param      fullPathOfFileName

   @param      writePermission    
 */
/*=================================================================== */
	ACAMS_StatisticsFile(const std::string & fullPathOfFileName, bool writePermission = false);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMS_StatisticsFile();
/*=================================================================== */
/**
   @brief      getCreationTime
                 Return file creation time

   @return     struct tm  
 */
/*=================================================================== */
	struct tm getCreationTime() const;
/*=================================================================== */
/**
   @brief      getNumberOfMsg
                 Return total number of messages stored in the message store since creation time

   @return     ULONGLONG
 */
/*=================================================================== */
	ULONGLONG getNumberOfMsg() const;
/*=================================================================== */
/**
   @brief      getNumberOfBytes
                 Return total number of bytes stored in the message store since creation time

   @return     ULONGLONG 
 */
/*=================================================================== */
	ULONGLONG getNumberOfBytes() const;
/*=================================================================== */
/**
   @brief      getNumberOfLostMsg
                 Return number of messages lost since creation time

   @return     int  
 */
/*=================================================================== */
	int getNumberOfLostMsg() const;
/*=================================================================== */
/**
   @brief       getNumberOfSkippedMsg
                  Return number of messages skipped since creation time

   @return      int   
 */
/*=================================================================== */
	int getNumberOfSkippedMsg() const;
/*=================================================================== */
/**
   @brief      incrementCounters

   @param      numberOfMsg

   @param      numberOfBytes

   @param      numberOfLostMsg

   @param      numberOfSkippedMsg

   @return     void 
 */
/*=================================================================== */
	// Add these values to the counters
	void incrementCounters(int numberOfMsg, int numberOfBytes, int numberOfLostMsg, int numberOfSkippedMsg);
/*=================================================================== */
/**
   @brief      syncToDisk
                Store current counter values to disk file

   @return     bool 
 */
/*=================================================================== */
	bool syncToDisk();
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private: 
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Constructor for ACAMS_StatisticsFile class.
 */
/*=================================================================== */
	ACAMS_StatisticsFile();
/*=================================================================== */
/**
   @brief      Constructor for ACAMS_StatisticsFile class.

   @param      another
 */
/*=================================================================== */
	ACAMS_StatisticsFile(const ACAMS_StatisticsFile & another);
/*=================================================================== */
/**
   @brief      operator=
 */
/*=================================================================== */
	const ACAMS_StatisticsFile & operator=(const ACAMS_StatisticsFile & another);
/*=================================================================== */
/**
   @brief      initMembers

   @param      writingAllowed

   @param      fileCreated

   @return     void
 */
/*=================================================================== */
	// Private help functions

	// Initialise the pointers into the memory mapped region, and set 'myFileMap' to the start of it 
	void initMembers(bool writingAllowed, bool fileCreated = false);
 /*=====================================================================
                          DATA MEMBER DECLARATION SECTION
==================================================================== */
	// Member data 
	ACE_HANDLE stat_hand;
	void *myView;         // View of file
	char dummyBuffer[128]; // used in case no file created.  

	// The counters. Pointers into the memory mapped region
	ULONGLONG * myNumberOfMsg;
	ULONGLONG * myNumberOfBytes;
	int * myNumberOfLostMsg;
	int * myNumberOfSkippedMsg;
	struct tm * fileCreationTime;
};

#endif
