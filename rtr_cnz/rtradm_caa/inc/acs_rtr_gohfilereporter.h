/*=================================================================== */
/**
   @file  acs_rtr_gohfilereporter.h 

   @brief Header file for rtr module.

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
   N/A       23/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_RTR_GOH_FILEREPORTER_H_
#define _ACS_RTR_GOH_FILEREPORTER_H_
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_rtr_gohreporter.h"
#include "acs_rtr_filereport.h"
#include "acs_rtr_filebuild.h"
//#include <process.h>
#include <string>
#include "ace/ACE.h"

using namespace std;
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief       ACS_RTR_GOHFileReporter
                   Inherited publically from ACS_RTR_GOHReporter

 */
/*=================================================================== */
class ACS_RTR_GOHFileReporter : public ACS_RTR_GOHReporter {
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
    enum { // indexes in the HANDLE array
    	stop_pos = 0,	// 0 Service shutdown
    	elem_pos,		// 1 output queue handle
    	rtrrm_pos, 		// 2 rtrrm command issued
	};
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief       FREP_NO_ERROR
 */
/*=================================================================== */
	static const int FREP_NO_ERROR = 0; //NO error

/*=================================================================== */
/**
   @brief       FREP_ERR_CREATE_EVENT
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_CREATE_EVENT = -1;
/*=================================================================== */
/**
   @brief       FREP_ERR_GET_REPDIR
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_GET_REPDIR = -2;
/*=================================================================== */
/**
   @brief       FREP_ERR_CREATE_OHI
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_CREATE_OHI = -3;
/*=================================================================== */
/**
   @brief       FREP_ERR_ATTACH
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_ATTACH = -4;
/*=================================================================== */
/**
   @brief       FREP_ERR_WAIT_FAILED
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_WAIT_FAILED = -5;
/*=================================================================== */
/**
   @brief       FREP_ERR_MOVE_FAILED
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_MOVE_FAILED = -6;
/*=================================================================== */
/**
   @brief       FREP_ERR_SEND_FAILED
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_SEND_FAILED = -7;
/*=================================================================== */
/**
   @brief       FREP_ERR_REFRESH_FAILED
                   Error constant
 */
/*=================================================================== */
	static const int FREP_ERR_REFRESH_FAILED = -8;
/*=================================================================== */
/**
   @brief       FREP_MAX_RETRIES_ON_ERROR   
 */
/*=================================================================== */
	static const int FREP_MAX_RETRIES_ON_ERROR = 30;
/*=================================================================== */
/**
   @brief       FREP_WAIT_TIMEOUT
 */
/*=================================================================== */
	static const int FREP_WAIT_TIMEOUT = 1; //1 sec
/*=================================================================== */
/**
   @brief       FREP_ATTACH_FAILED_TIMEOUT
 */
/*=================================================================== */
	static const int FREP_ATTACH_FAILED_TIMEOUT = 500;
/*=================================================================== */
/**
   @brief       FREP_MOVE_FAILED_TIMEOUT
 */
/*=================================================================== */
	static const int FREP_MOVE_FAILED_TIMEOUT = 1; // 1 sec
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief       Constructor for ACS_RTR_GOHFileReporter class.

   @param       transferQueue

   @param       queue

   @param       eventHandler

   @param       statptr

   @param       msName

   @param       siteName  
 */
/*=================================================================== */
	ACS_RTR_GOHFileReporter(std::string transferQueue,
							ACS_RTR_OutputQueue * queue,
							RTR_Events * eventHandler,
							RTR_statistics * statptr,
							std::string msName = "",
							std::string siteName = "");
/*=====================================================================
                        CLASS DESTRUCTOR
==================================================================== */
public:
/*=================================================================== */
/**
   @brief       Destructor for ACS_RTR_GOHFileReporter class.
 */
/*=================================================================== */
	virtual ~ACS_RTR_GOHFileReporter();
/*=================================================================== */
/**
   @brief       onStop

   @return      void
 */
/*=================================================================== */
	void onStop();

	void fxStop();

/*=================================================================== */
/**
   @brief       setRtrrmEvent

   @return      bool
 */
/*=================================================================== */
	bool setRtrrmEvent();

	virtual int open(ACE_HANDLE& stopEvent, ACE_HANDLE& rmEvent);

private:
/*=================================================================== */
/**
   @brief       fxDoWork

   @return      int
 */
/*=================================================================== */
	int fxDoWork();
/*=================================================================== */
/**
   @brief       fxInitialize

   @return      bool
 */
/*=================================================================== */
	bool fxInitialize();
/*=================================================================== */

/*=================================================================== */
/**
   @brief       fxGetReportedDirectory

   @param       dir

   @param       maxlen

   @param       err

   @return      bool
 */
/*=================================================================== */
	bool fxGetReportedDirectory(char * dir, int maxlen, int& err);
/*=================================================================== */
/**
   @brief       fxCreateFileReporter

   @param       reportedDir

   @param       err

   @return      bool
 */
/*=================================================================== */
	bool fxCreateFileReporter(char * reportedDir, unsigned int& err);
/*=====================================================================
                        PROTECTED DECLARATION SECTION
==================================================================== */	
protected:
/*=================================================================== */
/**
   @brief       operator
 */
/*=================================================================== */
//	virtual unsigned operator()(void * procArgs = 0);



        virtual int svc (void );

/*=====================================================================
                        PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
/**
   @brief       _msName
 */
/*=================================================================== */
	std::string _msName;
/*=================================================================== */
/**
   @brief       _siteName
 */
/*=================================================================== */
	std::string _siteName;
/*=================================================================== */
/**
   @brief       _fRep
                  OHI file handler interface
 */
/*=================================================================== */
	RTRfileReporter* _fRep; 
};
#endif // _ACS_RTR_GOH_FILEREPORTER_H_
