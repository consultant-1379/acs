/*=================================================================== */
/**
	@file		acs_logm_types.h

	@brief		Header file for logm  module.

			This module contains all the declarations for
			common utilities.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/11/2010     XKUSATI   Initial Release
==================================================================== */
//******************************************************************************
/*=====================================================================
			DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef _ACS_LOGM_TYPES_H
#define _ACS_LOGM_TYPES_H

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
//! Internal Data structures
#include <ace/ACE.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
 *	@brief		LOGM_PID_FILE
 */
/*=================================================================== */
#define LOGM_PID_FILE   	"/var/run/LOGMservice.pid"

#define LOGM_STS_OUTPUT     "/data/sts/output/"
#define LOGM_STS_DATA     "/data/sts/data/"
/*=================================================================== */
/**
 *	@brief		INFINITE
 */
/*=================================================================== */
#define INFINITE        	(((ACE_INT64)0)-1)  // Infinite timeout
/*=================================================================== */
/**
 *	@brief		LOGM_ACS_DATAPATH
 */
/*=================================================================== */
#define LOGM_ACS_DATAPATH                  ACE_TEXT("/tmp")     //ACS_DATA$")
/*=================================================================== */
/**
 *	@brief		LOGM_CONFIG_FILEPATH
 */
/*=================================================================== */
#define LOGM_CONFIG_FILEPATH                ACE_TEXT("/tmp/LOGM")
/*=================================================================== */
/**
 *	@brief		LOGM_CONFIG_FILE
 */
/*=================================================================== */
#define LOGM_CONFIG_FILE                    ACE_TEXT("/tmp/LOGM/CONFIG")
/*=================================================================== */
/**
 *	@brief		LOGM_SERVICE_VER
 */
/*=================================================================== */
#define LOGM_SERVICE_VER					"1.0"
/*=================================================================== */
/**
 *	@brief		MAX_FILE_PATH
 */
/*=================================================================== */
#define MAX_FILE_PATH                       1024
/*=================================================================== */
/**
 *	@brief		INVALID_HANDLE_VALUE
 */
/*=================================================================== */
#define INVALID_HANDLE_VALUE				-2
//extern bool isStatusActive = true;
/*=================================================================== */
/**
 *	@brief		LOGM_AEH_TEXT_INT_SUPERVISOR_ERROR
 */
/*=================================================================== */
#define LOGM_AEH_TEXT_INT_SUPERVISOR_ERROR  ACE_TEXT("INTERNAL LOGM SYSTEM SUPERVISOR ERROR")
/**
 *	@brief		bool
 */
/*=================================================================== */
typedef bool 		BOOL;
/*=================================================================== */
/*=================================================================== */
/**
 *	@brief		VOID
 */
/*=================================================================== */
#define VOID 		void
/*=================================================================== */
/**
 *	@brief		void*
 */
/*=================================================================== */
typedef void*		LPVOID;

// Data buffer sizes and length
/*=================================================================== */
/**
 *	@brief		ACS_LOGM_DATA_LEN
 */
/*=================================================================== */
#define ACS_LOGM_DATA_LEN                     (0x10000 - sizeof(ACE_UINT32))
/*=================================================================== */
/**
 *	@struct		_ACS_LOGM_LogFileSpec
 *	@brief		This structure stores the Log specific parameters.
 *
 *	@par		strPath_LOGNAME
 *
 *	@par		strPath
 *
 *	@par		strType
 *
 *	@par		iOperation
 *
 *	@par		iNoOFDays
 */
/*=================================================================== */
typedef struct _ACS_LOGM_LogFileSpec
{
   std::string strPath_LOGNAME;
   std::string strPath;
   std::string strType;
   ACE_UINT32 iNoOfFiles;
   ACE_UINT32 iNoOFDays;
} _ACS_LOGM_LOGFILESPEC;
/*=================================================================== */
/**
 *	@struct		_ACS_LOGM_ServiceParams
 *	@brief		This structure stores the Service specific parameters.
 *
 *	@par		iTimeHH
 *
 *	@par		iTimeMM
 *
 *	@par		iFreq
 *
 *	@par		iLogTypCnt
 *
 *	@par		iStsTrg
 */
/*=================================================================== */
typedef struct _ACS_LOGM_ServiceParams
{
   ACE_UINT32  iTimeHH;
   ACE_UINT32  iTimeMM;
   ACE_UINT32  iFreq;
   ACE_UINT32  iLogTypCnt;
   ACE_UINT32  iStsTrg;
   ACE_UINT32  iFileCntFreq;
} _ACS_LOGM_SERVICEPARAMS;

/*=====================================================================
		CLASS DECLARATION SECTION
=======================================================================*/

/*=====================================================================*/
/**
	@brief		FileDetails

 * 			This class contains the name of the file and the elapsed time
 * 			in seconds from the time of its last modification
 */
/*======================================================================= */

class FileDetails
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
   public:
	/*=================================================================== */
	/**
	 *	@brief		cFileName
	 *			Name of the file
	 */
	/*=================================================================== */
	ACE_TCHAR cFileName[256];
	/*=================================================================== */
	/**
	 *	@brief		ftElapsedTime
	 *			Elapsed time in seconds from the time of its last modification
	 */
	/*=================================================================== */
	double ftElapsedTime;
	/*=================================================================== */
	/**
	 *	@brief		FileDetails
	 *			function that stores the file name and its elapsed time
	 */
	/*=================================================================== */
	FileDetails(ACE_TCHAR name[256], double ft)
	{
		ACE_OS::strcpy(cFileName,name);
		ftElapsedTime = ft;
	}
};


#endif  // _ACS_LOGM_TYPES_H
