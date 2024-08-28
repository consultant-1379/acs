//******************************************************************************
//
// NAME
// acs_rtr_filereport.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-17 by XSAMECH
//
// DESCRIPTION
// This class encapsulates the file transfer API.
//
// CHANGES
// RELEASE REVISION HISTORY
// DATE		NAME			DESCRIPTION
//******************************************************************************
#include "acs_rtr_filereport.h"
#include <string> 
#include "acs_rtr_filebuild.h"
#include <iostream>
#include <cstdlib>

#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"
using namespace std;

ACS_RTR_TRACE_DEFINE(ACS_RTR_FILE_REP)

using namespace std;


//*************************************************************************
// Constructor
//*************************************************************************
RTRfileReporter::RTRfileReporter(const ACE_TCHAR* subSys,
                                                                 const ACE_TCHAR* appName,
                                                                 const ACE_TCHAR* filedest,
                                                                 const ACE_TCHAR* evtext,
                                                                 const ACE_TCHAR* path):

connected(false),
fd(filedest),
failedAttemptsCnt(0),
_fileHandler(0)
{
	ACS_RTR_TRACE_FUNCTION;
	_fileHandler = new AES_OHI_FileHandler(subSys, appName, filedest, evtext, path);
}

//*************************************************************************
// Constructor
//*************************************************************************
RTRfileReporter::RTRfileReporter(const ACE_TCHAR* subSys,
                                                                 const ACE_TCHAR* appName,
                                                                 const ACE_TCHAR* filedest,
                                                                 const ACE_TCHAR* evtext):
connected(false),
fd(filedest),
failedAttemptsCnt(0),
_fileHandler(0)
{
	ACS_RTR_TRACE_FUNCTION;
	_fileHandler = new AES_OHI_FileHandler(subSys, appName, filedest, evtext);
}

//*************************************************************************
// Destructor
//*************************************************************************
RTRfileReporter::~RTRfileReporter()
{
	if(_fileHandler)
		delete _fileHandler;
}


//*************************************************************************
// attachGOH

// Attach to the file destination.

//  return value:
//    true : ok.
//    false: something went wrong, err contains a fault value.
//*************************************************************************
bool RTRfileReporter::attachGOH(unsigned int& err)
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_TCHAR path[FILENAME_MAX] = {0};
	connected = false;

	err = _fileHandler->attach();	
	switch( err ) {
		case AES_OHI_NOERRORCODE: //successful operation
			if ( (err = _fileHandler->getDirectoryPath(path)) != AES_OHI_NOERRORCODE ) 
			{
				//getDirectoryPath error case
				connected = false;
				(void) detach(__LINE__);
			} else {
				//getDirectoryPath successfull
				connected = true;
				failedAttemptsCnt = 0; //reset number of failed Attempts
			}
			break;
		case AES_OHI_NOSERVERACCESS: //Indicate that there is no contact with aes_afp_server.
		case AES_OHI_ERRORUNKNOWN: //Indicates that an unknown error was encountered.
		case AES_OHI_FILEISPROTECTED: //Indicates that the transfer queue is already protected by another user.
		case AES_OHI_INCORRECTCOMMAND: //Using file transfer, this error code indicates that the command towards AES_CDH_server was incorrect.
		case AES_OHI_NOCDHSERVER: //Indicates that there is no contact with AES_CDH_server
		case AES_OHI_NODESTINATION: //Indicates that there is no destination for the transfer queue
		case AES_OHI_NOPROCORDER: //Indicates that the transfer queue is not defined
		default:
			{
				(void) detach(__LINE__);
			}
	}
	if (!connected) failedAttemptsCnt++;
	return connected;
}

//*************************************************************************
// sendGOH

// Report a file to the destination.

//  return value:
//    true : ok.
//    false: something went wrong, err contains a fault value.
//*************************************************************************
bool RTRfileReporter::sendGOH(const std::string st, unsigned int& err)
{
	ACS_RTR_TRACE_FUNCTION;
	switch( (err = _fileHandler->send(st.c_str())) ) {
		case AES_OHI_NOERRORCODE:
		case AES_OHI_SENDITEMEXIST: //Indicates that the file is already reported to the transfer queue
		case AES_OHI_FILENOTFOUND: //Indicates that the file was not found in the specified source directory
			connected = true;
			break;

		case AES_OHI_CATASTROPHIC: //Indicates that a server error was encountered and that the file was not reported to AFP
		case AES_OHI_INVALIDTRANSMODE: //Indicates that the selected transfer mode is invalid
		case AES_OHI_NOSERVERACCESS: //Indicate that there is no contact with aes_afp_server
		case AES_OHI_SENDITEMNAMEINVAL: //Indicates that the file name is either to long or has invalid characters
		default:
			{
				ACS_RTR_TRACE_MESSAGE("ERROR: Detaching with err : %d",err);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "Detaching with err : %d",err);
				(void) detach(__LINE__);
			}
			connected = false;
	}
	return connected;
}

//*************************************************************************
// handleEvent

// Callback function for receiving events from GOH.

//*************************************************************************
void RTRfileReporter::HandleGOHevent(void)
{
	ACS_RTR_TRACE_FUNCTION;
	AES_OHI_Eventcodes eventCode;
	if (_fileHandler->getEvent(eventCode)==AES_OHI_NOSERVERACCESS) {
		(void) detach(__LINE__);
		connected = false;
		return;
	}

	{
		//LOG

		ACE_TCHAR tracep[512] = {0};
		switch(eventCode) {
			case AES_OHI_EVEBLOCKTRANSFERFAILED:
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "Block transfer failed");
				break;
			case AES_OHI_EVEFILETRANSFERFAILED:
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "File transfer failed (alarm is raised)");
				break;
			case AES_OHI_EVEFILETRANSFERFAILEDCEASE:
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "File transfer recover (alarm is ceased)");
				break;
			case AES_OHI_EVELINKDOWN:
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "Link down (alarm is raised)");
				break;
			case AES_OHI_EVELINKDOWNCEASE:
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "Link up (alarm is ceased)");
				break;
			case AES_OHI_EVELOSTSERVER:
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "Lost connection to GOH Servers");
				break;
			default:
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "Unknown Error. Event Code: %d", eventCode);
		}
		ACS_RTR_TRACE_MESSAGE("%s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
	}

}

//*************************************************************************
// tooManyFailedAttempts

// Keeps track on the number of failed connection attempts to GOH
// and returns true if the number of attemts exceeds a certain limit.

//  return value:
//    true : too many attempts.
//    false: not too many attempts.
//*************************************************************************
bool RTRfileReporter::tooManyFailedAttempts(void)
{
	ACS_RTR_TRACE_FUNCTION;
	if (failedAttemptsCnt >= 5)
	{
		return true;
	}
	return false;
}
//*************************************************************************
//              GetFailedAttempts
//*************************************************************************
void RTRfileReporter::GetFailedAttempts(ACE_INT32& nFailed)
{
	nFailed = failedAttemptsCnt;
}

//-------
// detach
//-------
unsigned RTRfileReporter::detach(ACE_INT32 srcLine)
{
	ACS_RTR_TRACE_FUNCTION;
	(void)srcLine;
	if (_fileHandler)
	{
		ACE_UINT32 detachError = 0; 
		ACE_Time_Value FILE_DETACH_WAIT_TV(0,FILE_DETACH_WAIT*1000);
		for (ACE_INT32 retry = 0; (retry < FILE_MAX_RETRIES_ON_ERROR) && ((detachError = _fileHandler->detach()) != AES_OHI_NOERRORCODE); retry++)
		{
			ACE_OS::sleep(FILE_DETACH_WAIT_TV);
		}
		connected = false;
		return detachError;
	}
	else if (!connected)
	{
		return AES_OHI_NOERRORCODE;
	}
	else
	{
		return AES_OHI_ERRORUNKNOWN;
	}
}

//---------------
// getEventHandle
//---------------
ACE_Semaphore* RTRfileReporter::getEventHandle()
{
	return _fileHandler->getEventHandle();
}
