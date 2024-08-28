/*=================================================================== */
/**
	@file		acs_logm_logmaint.h

	@brief		Header file for LOGM  module.

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
N/A       23/08/2010     XKUSATI   Initial Release
N/A       16/01/1013	 XCSRPAD   Periodic deletion of CMX logs 
==================================================================== */

/*=====================================================================
		DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_ACSC_LOGMAINT_H
#define _ACS_ACSC_LOGMAINT_H

/*====================================================================
		INCLUDE DECLARATION SECTION
==================================================================== */

#include <ace/Task.h>
#include <string>
#include <acs_apgcc_omhandler.h>
#include <acs_logm_types.h>
#include <acs_logm_aeh.h>


using namespace std;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

//***************************************************************************
//Service Trigger specific Parameters in IMM Database
//***************************************************************************

/*====================================================================*/
/**
 *	@brief	SERVICE_TRIGGER_HOUR
 */
/*==================================================================== */
#define SERVICE_TRIGGER_HOUR		"initiationHour"

/*====================================================================*/
/**
 *	@brief	SERVICE_TRIGGER_MINUTES
 */
/*==================================================================== */

#define SERVICE_TRIGGER_MINUTES		"initiationMinute"

/*====================================================================*/
/**
 *	@brief	SERVICE_TRIGGER_FREQUENCY
 */
/*==================================================================== */

#define SERVICE_TRIGGER_FREQUENCY	"initiationFrequency"

/*==================================================================== */
/**
 *	@brief	LOG_FILE_COUNT
 */
/*==================================================================== */

#define LOG_FILE_COUNT				"logTypeCnt"

/*==================================================================== */
/**
 *      @brief  FILE_COUNT_FREQUNCY
 */
/*==================================================================== */

#define FILE_COUNT_FREQUNCY                          "scxFileCleanupFrequency"

/*====================================================================*/
/**
 *	@brief	STS_STATUS_LOG_DEL
 */
/*==================================================================== */

#define STS_STATUS_LOG_DEL			"statisticalOutputFilesDeletion"

/*==================================================================== */
/**
 *	@brief	LOG_FILE_DESCRIPTION
 */
/*==================================================================== */

#define LOG_FILE_DESCRIPTION		"logParam"

//***************************************************************************
//Log specific Parameters in IMM Database
//***************************************************************************

/*==================================================================== */
/**
 *	@brief	SERVICE_LOG_NAME
 */
/*==================================================================== */

#define SERVICE_LOG_NAME			"logName"

/*==================================================================== */
/**
 *	@brief	SERVICE_LOG_PATH
 */
/*==================================================================== */

#define SERVICE_LOG_PATH			"logFilePath"

/*==================================================================== */
/**
 *	@brief	SERVICE_LOG_TYPE
 */
/*==================================================================== */

#define SERVICE_LOG_TYPE			"logExtensionType"

/*==================================================================== */
/**
 *	@brief	SERVICE_LOG_OPERATION
 */
/*==================================================================== */

#define SERVICE_LOG_OPERATION			"logNumberOfFile"

/*==================================================================== */
/**
 *	@brief	SERVICE_LOG_OLD_DAYS
 */
/*==================================================================== */

#define SERVICE_LOG_OLD_DAYS			"logDeletionFrequency"




//***************************************************************************
//Values of parameters in parameter file
//***************************************************************************

/*====================================================================*/
/**
 *	@brief	DAILY_FREQUENCY
 */
/*==================================================================== */

//#define DAILY_FREQUENCY				0

/*==================================================================== */
/**
 *	@brief	WEEKLY_FREQUENCY
 */
/*==================================================================== */

//#define WEEKLY_FREQUENCY			1


/*====================================================================*/
/**
 *	@brief	DELETE_FILES
 */
/*==================================================================== */

#define DELETE_FILES				0

/*==================================================================== */
/**
 *	@brief	TRUNCATE_FILES
 */
/*==================================================================== */

#define TRUNCATE_FILES				1

/*==================================================================== */
/**
 *	@brief	WAIT_TIME_FOR_DAILY
 */
/*==================================================================== */

#define WAIT_TIME_FOR_DAILY			(1000 * 24 * 3600)

/*==================================================================== */
/**
 *	@brief	SECONDS_IN_DAY
 */
/*==================================================================== */

#define SECONDS_IN_DAY				(24 * 3600)

/*==================================================================== */
/**
 *	@brief	SECONDS_IN_WEEK
 */
/*==================================================================== */

#define SECONDS_IN_WEEK				(24 * 3600*7)

/*==================================================================== */
/**
 *	@brief	WAIT_TIME_FOR_WEEKLY
 */
/*==================================================================== */

#define WAIT_TIME_FOR_WEEKLY		(1000 * 24 * 3600 * 7)

/*====================================================================*/
/**
 *	@brief	FHANDLE
 */
/*==================================================================== */

#define FHANDLE						unsigned int



//***************************************************************************
//Parameters related to Error handling
//***************************************************************************

/*==================================================================== */
/**
 *	@brief	PAR_ERROR
 */
/*==================================================================== */

#define PAR_ERROR						0

/*==================================================================== */
/**
 *	@brief	SUCCESS
 */
/*==================================================================== */

#define SUCCESS							1


//#define ERROR							0

/*==================================================================== */
/**
 *	@brief	READ_PARAM_ERROR
 */
/*==================================================================== */
#define READ_PARAM_ERROR	"Error reading parameter for LOGM from Parameter Database"

/*==================================================================== */
/**
 *	@brief	LOG_DELETION_ERROR
 */
/*==================================================================== */

#define LOG_DELETION_ERROR	"Error deleting log file after first attempt"

/*==================================================================== */
/**
 *	@brief	FOLDER_DELETION_ERROR
 */
/*==================================================================== */

#define FOLDER_DELETION_ERROR		"Error deleting folder after first attempt"
	
/*==================================================================== */
/**
 *	@brief	INVALID_PARAM_ERROR
 */
/*==================================================================== */

#define INVALID_PARAM_ERROR			"Invalid parameter value - "

/*==================================================================== */
/**
 *	@brief	TimerLowValue
 */
/*==================================================================== */

#define TimerLowValue					0

/*==================================================================== */
/**
 *	@brief	TimerHighValue
 */
/*==================================================================== */

#define TimerHighValue					0

/*==================================================================== */
/**
 *	@brief	SLEEP_IF_FIRST_DELETION_FAILS
 */
/*==================================================================== */

#define SLEEP_IF_FIRST_DELETION_FAILS 5

/*==================================================================== */
/**
 *	@brief	ERROR_FILE_NOT_FOUND
 */

/*==================================================================== */
#define ERROR_FILE_NOT_FOUND	-1


/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */


#ifdef ACS_LOGM_CUTE_TEST
#include <Cute_ACS_ACSC_Logmaint.h>
#endif




/*====================================================================
			STRUCTURE DECLARATION SECTION
==================================================================== */
/**
 *	@struct		LIN_64_FIND_DATA
 *
 *	@brief		This structure stores the file creation time and File name of the file.
 *
 *	@par		dwFileAttributes
 *
 *	@par		ftFileCreationTime
 *
 *	@par		cFileName
 *
 */

struct LIN_64_FIND_DATA
{
/*====================================================================*/
  /**
   * @brief		dwFileAttributes
   */
/*====================================================================*/
	int dwFileAttributes;
	
/*====================================================================*/
	  /**
	   * @brief		ftFileCreationTime
	   */
/*====================================================================*/
	struct tm* ftFileCreationTime;

/*====================================================================*/
	  /**
	   * @brief		cFileName
	   */
/*====================================================================*/
	char cFileName[MAX_FILE_PATH];
};


 struct LIN_64_FIND_DATA_SCX
 {
 /*====================================================================*/
   /**
    * @brief		dwFileAttributes
    */
 /*====================================================================*/
 	int dwFileAttributes;

 /*====================================================================*/
 	  /**
 	   * @brief		ftFileCreationTime
 	   */
 /*====================================================================*/
 	struct tm* ftFileCreationTime;

 /*====================================================================*/
 	  /**
 	   * @brief		cFileName
 	   */
 /*====================================================================*/
 	char cFileName[MAX_FILE_PATH];
 };
 struct LIN_64_FIND_DATA_CMX
 {
 /*====================================================================*/
   /**
    * @brief		dwFileAttributes
    */
 /*====================================================================*/
 	int dwFileAttributes;

 /*====================================================================*/
 	  /**
 	   * @brief		ftFileCreationTime
 	   */
 /*====================================================================*/
 	struct tm* ftFileCreationTime;

 /*====================================================================*/
 	  /**
 	   * @brief		cFileName
 	   */
 /*====================================================================*/
 	char cFileName[MAX_FILE_PATH];
 };

/*=====================================================================
					FORWARD DECLARATION SECTION
==================================================================== */
 /*====================================================================*/
 /**
  *	@brief		ACE_Event
  */
 /*====================================================================*/
class ACE_Event;


/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
	@brief		ACS_ACSC_Logmaint

				This class includes functions to fetch server trigger
				and log specific  parameters from PHA and delete log
				files based on these parameters.
*/
/*=================================================================== */


class ACS_ACSC_Logmaint:public ACE_Task_Base
{

#ifdef ACS_LOGM_CUTE_TEST
	friend class Cute_ACS_ACSC_Logmaint;
#endif
	

	/*===================================================================
				PRIVATE DECLARATION SECTION
	=================================================================== */
	private:
	/*=================================================================== */
	/*====================================================================*/
	/**

		@brief		m_phEndEvent

					Pointer to event.

	*/
	/*==================================================================== */
	ACE_Event *m_phEndEvent;

	/*====================================================================*/
	/**
		@brief		isStatusActive

	 */
	/*==================================================================== */
	static bool isStatusActive;

	std::string theDataSTSPath;
	std::string theOutputSTSPath;

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
	public:
	/*=================================================================== */

	/*=====================================================================
				CLASS CONSTRUCTORS
	==================================================================== */
	/*====================================================================*/
	/**

		@brief		ACS_ACSC_Logmaint
					Constructor for ACS_ACSC_Logmaint class.

		@par 		ACE_Event*
					Event for stopping Logmaint service
	*/
	/*=================================================================== */

	ACS_ACSC_Logmaint(ACE_Event* poEvent);

	/*===================================================================
				CLASS DESTRUCTOR
	=================================================================== */
	/*====================================================================*/
	/**

		@brief		Destructor for ACS_ACSC_Logmaint class.


	*/
	/*=================================================================== */

   ~ACS_ACSC_Logmaint();


	/*=================================================================== */
	/**

		@brief			cleanLogFilesCoreMiddleWare
						This routine waits for service trigger time and triggers
						cleanup activity of Logmaint when invoked by Core Middleware.
	 *
	 *	@return			void
	 */
	/*=================================================================== */

   void cleanLogFilesCoreMiddleWare();

	/*=================================================================== */
        /**

                @brief                  cleanSCXLogFilesCoreMiddleWare
                                                This routine waits for service trigger time and triggers
                                                cleanup activity of Logmaint when invoked by Core Middleware.
         *
         *      @return                 void
         */
        /*=================================================================== */

   void cleanSCXLogFilesCoreMiddleWare();
	/*=================================================================== */
        /**

                @brief                  cleanCMXLogFilesCoreMiddleWare
                                                This routine waits for service trigger time and triggers
                                                cleanup activity of Logmaint when invoked by Core Middleware.
         *
         *      @return                 void
         */
        /*=================================================================== */

   void cleanCMXLogFilesCoreMiddleWare();
	/*=================================================================== */
	/**

		@brief			cleanLogFilesInteractive
						This routine waits for service trigger time and triggers
						cleanup activity of Logmaint in interactive mode.
	 *
	 *	@return			void
	*/
	/*=================================================================== */

   void cleanLogFilesInteractive();

/*=================================================================== */
        /**

                @brief                  cleanSCXLogFilesInteractive
                                                This routine waits for service trigger time and triggers
                                                cleanup activity of Logmaint in interactive mode.
         *
         *      @return                 void
        */
        /*=================================================================== */

   void cleanSCXLogFilesInteractive();
	/*=================================================================== */
	/**

                @brief                  cleanCMXLogFilesInteractive
                                                This routine waits for service trigger time and triggers
                                                cleanup activity of Logmaint in interactive mode.
         *
         *      @return                 void
        */
        /*=================================================================== */

   void cleanCMXLogFilesInteractive();
	/*=================================================================== */
	/**

		@brief		GetPHAParameterInt
					This routine gets the parameters of type Integer from PHA

		@param		aParameterName
					The name of the parameter for which the value in PHA is to
					be fetched.

		@param		aVal
					value of the parameter

		@return		ACE_UINT32
					1 Success
					0 Failure

	*/
	/*=================================================================== */

   ACE_UINT32 GetPHAParameterInt(std::string aParameterName,ACE_UINT32 &aVal);



	/*=================================================================== */
	/**

		@brief		GetPHAParameterString
					This routine gets the parameters in form of string from PHA

		@param		aParameterName
					The name of the parameter for which the value in PHA is to
					be fetched.

		@param		aVal
					value of the parameter

		@return		ACE_UINT32
					1 Success
					0 Failure

	*/
	/*=================================================================== */

   ACE_UINT32 GetPHAParameterString(std::string aParameterName,std::string &aVal);

	/*=================================================================== */
	/**

		@brief		GetLogFileParams
					This routine gets the Log file parameters from PHA

		@param		ptrParams
					Structure in which the PHA parameters will be stored after its
					fetched from PHA.

		@param		iIndex
					The index of the cleanup activity. Eg: FTLOG=1, STSag=2, etc

		@return		ACE_UINT32
					1 Success
					0 Failure

	*/
	/*=================================================================== */
	ACE_INT32  GetLogFileParams(_ACS_LOGM_LogFileSpec *ptrParams,ACE_INT32 iIndex);

	/*=================================================================== */
	/**

		@brief		GetInitialTimeToWait
					This routine calculates the time to wait (in seconds)
					for service trigger to take place

		@param		ptrParams (Input)
					PHA parameters containing data required for service trigger

		@param		dSecondsToWait (Output)
					number of seconds to wait.

		@return		true - if the routine was able to calculate the time required to
					wait for Logmaint service trigger
					false - if the routine was not able to calculate the time required
					to wait for Logmaint service trigger

	*/
	/*=================================================================== */
	ACE_INT32 GetInitialTimeToWait(_ACS_LOGM_ServiceParams *ptrParams,double *dSecondsToWait);


	/*=================================================================== */
	/**

		@brief		GetServiceparams
					This routine gets the Service file parameters from PHA

		@param		ptrParams
					Structure in which the PHA parameters will be stored after its
					fetched from PHA.

		@return		ACE_UINT32
					1 Success
					0 Failure

	*/
	/*=================================================================== */
	ACE_INT32 GetServiceparams(_ACS_LOGM_SERVICEPARAMS *ptrParams);


	//VOID performLogCleanup();

	/*===================================================================
						PRIVATE DECLARATION SECTION
	=================================================================== */
	private:


	/*=================================================================== */
	/**

		@brief		FindFileAndFolders
					This routine deletes the log files for STSag, STSIn, ddiRm,
					TRAlog periodically based on the configuration set by designer
					in IMM Database

		@param		sRelativePath (Input)
					The path in which log file deletion activity will
					take place for a given LOG Name fetched from IMM Database.

		@param		ptrParams (Output)
					Log file specification structure _ACS_LOGM_LOGFILESPEC
					in which the fetched value of log file parameters from IMM
					database will be placed

		@param		iTrgType (Input)
					Log deletion trigger (0 or 1) fetched from IMM Database
		@return		void

	*/
	/*=================================================================== */
	void FindFileAndFolders(string sRelativePath,_ACS_LOGM_LOGFILESPEC *ptrParams , int iTrgType );

	/*=================================================================== */
	/**

		@brief		DeleteLogFiles
					This routine deletes the log files of all the services

		@param		iLogTypCnt (Input)
					Log count value fetched from IMM database. The other parameters dwTimerLowValue and
					dwTimerHighValue are not used.

		@param          dwTimerLowValue 

		@param          dwTimerHighValue 

		@return		void

	*/
	/*=================================================================== */
	void DeleteLogFiles (ACE_UINT32 iLogTypCnt, ACE_UINT32 dwTimerLowValue, ACE_UINT32 dwTimerHighValue);

	/*=================================================================== */
	/**

		@brief		isDirectory
					This routine checks if the given path is a directory or not

		@param		scannedPath (Input)
					The absolute path of the file or directory

		@return		true if the the given path is a directory
					false if the given path is not a directory

	*/
	/*=================================================================== */
	bool isDirectory(string scannedPath);

	/*=================================================================== */
	/**

		@brief		ElapsedTimeCal
					This routine calculate the time difference of file modification time with current time

		@param		stFile (Input)
					Modification time of the file

		@return		The difference of the file creation time with current time in seconds

	*/
	/*=================================================================== */
	double ElapsedTimeCal(struct tm *stFile);

	/*=================================================================== */
	/**

		@brief		convertToString
					This routine converts the character pointer to string

		@param		recString (Input)
					A character pointer string

		@return		String in form of C++ standard library of datatype string

	*/
	/*=================================================================== */
	string convertToString(char* recString);


	/*=================================================================== */
	/**

		@brief		isWildCard
					This routine checks the presence of character '*' in a string and return true in case it finds it.

		@param		wildCardString (Input)
					Input string in which the presence of character '*' is to be checked

		@return		true if the character '*' is present in the input string
					false if the character '*' is not present in the input string

	*/
	/*=================================================================== */
	bool isWildCard(string wildCardString);


	/*=================================================================== */
	/**

		@brief		wildCardMatch
				This routine checks the string patters for paths of files or folders starting with names in format
				 "FOLDER*" and "*.filename" only.
				 Ex: the function checks for "/root/MSTPSVC*" in "/root/MSTPSVC5/abc.log" and returns true
				 Ex: the function checks for "/root/MSTPSVC*" in "/root/DDI/abc.log" and returns false since there
				 is no match for "/root/MSTPSVC*" in "/root/DDI/abc.log"

		@param		wildCardString (Input)
				Input string/sub-string having wildcard that needs to be compared with complete path
				path (Input)
				Complete path of the file/directory for which the wildcard pattern has to be matched.

		@param          path 
		
		@return		true  if the character '*' is present in the input string
				false if the character '*' is not present in the input string

	*/
	/*=================================================================== */
	bool wildCardMatch(string wildCardString, string path);


	/*=================================================================== */
	/**

		@brief		stringReverse
				This routine reverses a given string

		@param		receiveString (Input)
				Input string which has to be reversed

		@return		reversed string

	*/
	/*=================================================================== */
	string stringReverse(string receiveString);

	/*=================================================================== */
	/**

		@brief		FindFirstFile
				This routine find the first file in a given directory and stores its name and its last
				modification date in structure of type  LIN_64_FIND_DATA

		@param		directoryName (Input)
				Directory name in which the search is to be performed
		@param		ldFindFileData (Output)
				LIN_64_FIND_DATA structure for storing the first found file name and its last modification date

		@return		returns 0 if a file in a given directory is found
				returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
				returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid

	*/
	/*=================================================================== */
	FHANDLE FindFirstFile(string directoryName,LIN_64_FIND_DATA *ldFindFileData);


	/*=================================================================== */
	/**

		@brief		FindFirstFileSCX
				This routine find the first file in a given directory and stores its name and its last
				modification date in structure of type  LIN_64_FIND_DATA

		@param		directoryName (Input)
				Directory name in which the search is to be performed
		@param		ldFindFileData (Output)
				LIN_64_FIND_DATA structure for storing the first found file name and its last modification date

		@return		returns 0 if a file in a given directory is found
				returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
				returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid

	*/
	/*=================================================================== */
	FHANDLE FindFirstFileSCX(string directoryName,LIN_64_FIND_DATA_SCX *ldFindFileData);

/*=================================================================== */
	/**

		@brief		FindFirstFileCMX
				This routine find the first file in a given directory and stores its name and its last
				modification date in structure of type  LIN_64_FIND_DATA

		@param		directoryName (Input)
				Directory name in which the search is to be performed
		@param		ldFindFileData (Output)
				LIN_64_FIND_DATA structure for storing the first found file name and its last modification date

		@return		returns 0 if a file in a given directory is found
				returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
				returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid

	*/
	/*=================================================================== */
	FHANDLE FindFirstFileCMX(string directoryName,LIN_64_FIND_DATA_CMX *ldFindFileData);
	/*=================================================================== */
	/**

		@brief		FindFirstFileInQueue
				This routine find the first file name in dqueue logFileContainerWC and stores its name and its last
 				modification date in structure of type  LIN_64_FIND_DATA

		@param		ldFindFileData (Output)
				LIN_64_FIND_DATA structure for storing the first found file name and its last modification date

		@return		returns 0 if a file in a given directory is found
				returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
				returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid

	*/
	/*=================================================================== */
	FHANDLE	 FindFirstFileInQueue(LIN_64_FIND_DATA *ldFindFileData);


	/*=================================================================== */
	/**

		@brief		FindNextFile
				This routine takes in the input file handle returned by FindFirstFile function and finds the next file
				in a given directory path. It also increments the input file handle by 1.

		@param		fileHandle (Input)
				Input file handle

		@param		ldFindNextFileData (Output)
				LIN_64_FIND_DATA structure for storing the first found file name and its last modification date

		@return		returns 1 if the file corresponding to next file handle is found
  				returns 0 if the file corresponding to next file handle is not found

	*/
	/*=================================================================== */
	bool FindNextFile(FHANDLE *fileHandle,LIN_64_FIND_DATA *ldFindNextFileData);

	/*=================================================================== */
	/**

		@brief		FindNextFileSCX
				This routine takes in the input file handle returned by FindFirstFile function and finds the next file
				in a given directory path. It also increments the input file handle by 1.

		@param		fileHandle (Input)
				Input file handle

		@param		ldFindNextFileData (Output)
				LIN_64_FIND_DATA structure for storing the first found file name and its last modification date

		@return		returns 1 if the file corresponding to next file handle is found
  				returns 0 if the file corresponding to next file handle is not found

	*/
	/*=================================================================== */
	bool FindNextFileSCX(FHANDLE *fileHandle,LIN_64_FIND_DATA_SCX *ldFindNextFileData);
/*=================================================================== */
	/**

		@brief		FindNextFileCMX
				This routine takes in the input file handle returned by FindFirstFile function and finds the next file
				in a given directory path. It also increments the input file handle by 1.

		@param		fileHandle (Input)
				Input file handle

		@param		ldFindNextFileData (Output)
				LIN_64_FIND_DATA structure for storing the first found file name and its last modification date

		@return		returns 1 if the file corresponding to next file handle is found
  				returns 0 if the file corresponding to next file handle is not found

	*/
	/*=================================================================== */
	bool FindNextFileCMX(FHANDLE *fileHandle,LIN_64_FIND_DATA_CMX *ldFindNextFileData);
	/*=================================================================== */
	/**

		@brief		removedirectoryrecursively
				This routine recursively deletes all the files and folders in a given directory.

		@param		dirname (Input)
				Absolute Directory path

		@return		returns 1 if all the contents in the given directory was removed
  				returns 0 if all the contents in the given directory was not removed

	*/
	/*=================================================================== */
	int removedirectoryrecursively(const char *dirname,string logPath);


	/*=================================================================== */
	/**

		@brief		extractFileNameFrompath
				This routine returns the file name from a given absolute path of a file.

		@param		path (Input)
				Absolute path of a file

		@return		file name identified from the absolute path
	*/
	/*=================================================================== */
	string extractFileNameFrompath(string path);


	/*=================================================================== */
	/**

		@brief		findPathSearchPatternFromWildCardDirectory
				This routine takes in the absolute path of a file containing wildcard characters (only '*') and
				returns the partial path of that file containing preceding '*'

		@param		wildcard (Input)
				Absolute path of a file with wild card character '*'

		@return		partial path of that file containing preceding '*'
	*/
	/*=================================================================== */
	string findPathSearchPatternFromWildCardDirectory(string wildcard);


	/*=================================================================== */
	/**

		@brief		searchPathFromWildCard
				This routine takes in the absolute path of a file or directory containing wildcard characters (only '*') and
				returns the path of that file/directory containing preceding '*'

		@param		receiveDir (Input)
				Absolute path of a file

		@return		path of that file containing preceding '*'
	*/
	/*=================================================================== */
	string searchPathFromWildCard(string receiveDir);

	/*=================================================================== */
	/**

		@brief		removeFile
				This routine deletes a given file

		@param		fileName (Input)
				Absolute path of a file

		@return		true if the file was deleted successfully
				false if the file was not deleted successfully
	*/
	/*=================================================================== */
	bool removeFile(char* fileName);

	/*=================================================================== */
	/**

		@brief		printDeque
				This routine prints the content of queue logFileContainerWC
	*/
	/*=================================================================== */

	void printDeque();
	/*=================================================================== */
		/**

			@brief		printDequeSCX
					This routine prints the content of queue logFileContainerWC
		*/
		/*=================================================================== */

		void printDequeSCX();
 /*=================================================================== */
                /**

                        @brief          printDequeCMX
                                        This routine prints the content of queue logFileContainerWC
                */
                /*=================================================================== */

        void printDequeCMX();
	/*=================================================================== */
	/**

		@brief		theOmHandler

	*/
	/*=================================================================== */

	OmHandler theOmHandler;
	
	/*=================================================================== */
        /**

                @brief          service initiation freq enumuration

        */
        /*=================================================================== */

	enum
	{
		DAILY,
		WEEKLY
	}initiationFrequencyStatus;

	/*=================================================================== */
	/**

		@brief		getTimeStampOfFile
				This routine get the last modification time of a given file

		@param		fileName (Input)
				Absolute path of a file

		@return		Last modification time of the file in struct tm* format
	*/
	/*=================================================================== */
	struct tm* getTimeStampOfFile(string fileName);

	/*=================================================================== */
	/**

		@brief		showDirectoriesWildCard
				This routine updates the dqueue logFileContainerWC with the path of files or directories that matches
	     `			its path containing wild card

		@param		receiveDirWildCard (Input)
				Absolute path of the directory containing wild card character '*'

		@return		returns 0 if a file in a given directory is found
 				returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
 				returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid
	*/
	/*=================================================================== */
	int showDirectoriesWildCard(string receiveDirWildCard,bool isSCXLog);
       /*=================================================================== */
	/**

		@brief		showCmxDirectoriesWildCard
				This routine updates the dqueue logFileContainerWC with the path of files or directories that matches
	     `			its path containing wild card

		@param		receiveDirWildCard (Input)
				Absolute path of the directory containing wild card character '*'

		@return		returns 0 if a file in a given directory is found
 				returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
 				returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid
	*/
	/*=================================================================== */
	int showCmxDirectoriesWildCard(string receiveDirWildCard);
	/*=================================================================== */
	/**

		@brief		filterByExtension
				This routine updates the dqueue logFileContainerWC with the path of files with the
supplied extention of the file


		@param		extensionString (Input)
				Extention of the file having wildcard character '*'

		@return		none
	*/
	/*=================================================================== */
	void filterByExtension(string extensionString);

	/*=================================================================== */
	/**

		@brief		showFilesandDirectories
				Walks through the files and directories recursively and lists them and stores its absolute path in
				dqueue logFileContainer.


		@param		receiveDir (Input)
				Absolute path of Directory in which the search has to be performed

		@param          isSCXLog
				whether search is for scxlog

		@return		returns 0 if a file in a given directory is found
 				returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
 				returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid
	*/
	/*=================================================================== */
	int showFilesandDirectories(string receiveDir,bool isSCXLog);

	 /*=================================================================== */
        /**

                @brief          showCmxFilesandDirectoies
                                Walks through the files and directories recursively and lists them and stores its absolute path in
                                dqueue logFileContainer.


                @param          receiveDir (Input)
                                Absolute path of Directory in which the search has to be performed

                @return         returns 0 if a file in a given directory is found
                                returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
                                returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid
        */
        /*=================================================================== */
        int showCmxFilesandDirectories(string receiveDir);
	
	/*=================================================================== */
	/**

		@brief		getCurrentTime
				This routine gets the current time in struct tm format

		@return		current time in struct tm format
	*/
	/*=================================================================== */
	struct tm* getCurrentTime();
	
	 /*=================================================================== */
        /**

                @brief          showCmxFilesandDirectoies
                                Walks through the files and directories recursively and lists them and stores its absolute path in
                                dqueue logFileContainer.


                @param          receiveDir (Input)
                                Absolute path of Directory in which the search has to be performed

                @return         returns 0 if a file in a given directory is found
                                returns ERROR_FILE_NOT_FOUND (-1) if file in the directory was not found
                                returns INVALID_HANDLE_VALUE (-2) if the supplied directory path is invalid
        */
        /*=================================================================== */
        int showCmxFilesandDirectories(string receiveDir);

	ACE_INT32 isDirectoryEmpty(string dname);
	/*=================================================================== */
	/**

		@brief		deleteSCXFiles
					This routine deletes the SCX log files

		@param		dirPath 
					SCX log file path
		@param		numOfDays
					No of days a file can't be deleted.
								
		@return		void

	*/
	/*=================================================================== */
	void deleteSCXFiles(string dirPath, int numOfDays);
	/*=================================================================== */
	/**
	@brief		deleteCMXFiles
					This routine deletes the SCX log files

		@param		dirPath 
					CMX log file path
		@param		numOfDays
					No of days a file can't be deleted.
								
		@return		void

	*/
	/*=================================================================== */
	void deleteCMXFiles(string dirPath, int numOfDays);
	 /*=================================================================== */
        /**
                 @brief           DeleteMoreThen
                                        This routine deletes SCX log files
                                        more than the specified number in a subdirectory

                 @param          refcstrRootDirectory (Input)
                                        SCX log file path

                 @param          fileNum (Input)
                                        Number of SCX files to be deleted


                @return         void

        */
        /*=================================================================== */

	   void DeleteMoreThen(const std::string &refcstrRootDirectory, int fileNum);
         /*=================================================================== */
        /**
                 @brief           DeleteCmxMoreThen
                                        This routine deletes CMX log files
                                        more than the specified number in a subdirectory

                 @param          refcstrRootDirectory (Input)
                                        CMX log file path

                 @param          fileNum (Input)
                                        Number of CMX files to be deleted


                @return         void

        */
        /*=================================================================== */
        void DeleteCmxMoreThen(const std::string &refcstrRootDirectory, int fileNum);
        /*=================================================================== */
	/**

		@brief		DeleteExtraFiles
					This routine deletes the SCX log files

	
		 @param          iLogTypCnt (Input)
                                        Log count value fetched from IMM database. 
					The other parameters dwTimerLowValue and
                                        dwTimerHighValue are not used.

                @param          dwTimerLowValue

                @param          dwTimerHighValue

                @return         void
	*/
	/*=================================================================== */
	void DeleteExtraFiles(ACE_UINT32 iLogTypCnt,
          ACE_UINT32 dwTimerLowValue, ACE_UINT32 dwTimerHighValue);
	/*=================================================================== */
	/**

		@brief		DeleteMoreThen
					This routine deletes all SCX log files 
					more than the specified number

		@param		refcstrRootDirectory (Input)
					SCX log file path
		
		@param		fileNum (Input)
					Number of SCX files to be deleted
								
		@return		void

	*/
	/*=================================================================== */
};
#endif
