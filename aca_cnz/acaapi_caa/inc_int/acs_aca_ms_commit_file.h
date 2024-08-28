/*=================================================================== */
/**
   @file  acs_aca_ms_event_handler.h

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
#ifndef ACAMS_CommitFile_H
#define ACAMS_CommitFile_H

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <list>

#include <ace/ACE.h>
#include <ace/Event.h>

#include "acs_aca_ms_const_values.h"
#include "acs_aca_common.h"
#include "acs_aca_defs.h"

using namespace std;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      ACAMS_CommitFile
 */
/*=================================================================== */
class  ACAMS_CommitFile {

/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Constructor for ACAMS_CommitFile class.

   @param      fullPathOfCommitFileName

   @param      createFile  
 */
/*=================================================================== */
	ACAMS_CommitFile(const std::string & fullPathOfCommitFileName, bool createFile = false);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMS_CommitFile();
/*=================================================================== */
/**
   @brief      commit
                 Write to disk-file

   @param      commitMsgNum

   @param      fileName

   @param      transactionData

   @param      transactionDataLength

   @return     bool 
 */
/*=================================================================== */
	// Write to disk-file
	bool commit(ULONGLONG commitMsgNum, const std::string & fileName, const unsigned char * transactionData, const unsigned int transactionDataLength);
/*=================================================================== */
/**
   @brief      commit

   @param      numOfMessages

   @param      commitMsgNum

   @param      fileName

   @param      transactionData

   @param      transactionDataLength

   @return     bool   
 */
/*=================================================================== */
	bool commit(ULONGLONG numOfMessages, ULONGLONG commitMsgNum, const std::string & fileName, const unsigned char * transactionData, const unsigned int transactionDataLength);
/*=================================================================== */
/**
   @brief      getMsgNumber
                  Get Message Number of last commited message

   @return     ULONGLONG
 */
/*=================================================================== */
	ULONGLONG getMsgNumber() const;
/*=================================================================== */
/**
   @brief      getFileName
                 Get file name of data file where last commited message is stored

   @return     string
 */
/*=================================================================== */
	const std::string getFileName() const;
/*=================================================================== */
/**
   @brief      getTransactionData
                 Get transaction data from last commit

   @return     unsigned char 
 */
/*=================================================================== */
	const unsigned char * getTransactionData() const;
/*=================================================================== */
/**
   @brief      list_all_files

   @param      sStartDir

   @param      lstFound

   @param      bRecursive

   @return     void 
 */
/*=================================================================== */
	void list_all_files(const string& sStartDir,std::list<string>&lstFound, bool bRecursive);
/*=================================================================== */
/**
   @brief      getTransactionDataLength

   @return     int
 */
/*=================================================================== */
	// Get size of transaction data from last commit
	int getTransactionDataLength() const;
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */

private: 
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
	ACAMS_CommitFile();
/*=================================================================== */
/**
   @brief      Constructor for ACAMS_CommitFile class.

   @param      another
 */
/*=================================================================== */
	ACAMS_CommitFile(const ACAMS_CommitFile & another);
/*=================================================================== */
/**
   @brief      operator=
 */
/*=================================================================== */
	const ACAMS_CommitFile & operator=(const ACAMS_CommitFile & another);
/*=================================================================== */
/**
   @brief      repairFile

   @param      string

   @param      string

   @return     bool
 */
/*=================================================================== */
	bool repairFile(std::string, std::string);
/*=================================================================== */
/**
   @brief      validDataFile

   @param      FileName

   @param      msgNumber

   @return     bool
 */
/*=================================================================== */
	bool validDataFile(std::string FileName, ULONGLONG & msgNumber);
/*=================================================================== */
/**
   @brief      findFirstFile

   @param      sStartDir

   @param      fileFound

   @param      compareWith

   @return     bool
 */
/*=================================================================== */
	bool findFirstFile (const string& sStartDir, std::string& fileFound, std::string compareWith);

/*=====================================================================
                          DATA MEMBER DECLARATION SECTION
==================================================================== */
	ACE_HANDLE hand_commitf;
	ULONGLONG myLastMsgNumber; 
	const unsigned char * myTransactionDataPtr;  
	unsigned int myTransactionDataLength; 
	unsigned char myBuffer[ACAMS_MaxSizeOfCommitData]; // I/O-buffer
	std::string myDataFileName;     // Data file name of last committed msg
	std::string myName;             // Full path of CommitFile name
};

#endif //ACAMS_CommitFile_H
