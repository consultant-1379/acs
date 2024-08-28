//********************************************************************************
//
// NAME
// acs_rtr_gohfilereporter.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2012
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-17 by XSAMECH
//
// DESCRIPTION 
// This class implements a thread for the communication with AFP.
// It retrieves blocks from ACS_RTR_ACAChannel thread using a shared output queue.
//
//********************************************************************************
#include "acs_rtr_gohfilereporter.h"
#include "acs_rtr_systemconfig.h"
#include "acs_rtr_global.h"
#include "acs_rtr_server.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"
#include <iostream>
#include <cstdlib>
using namespace std;


namespace {
//----------
// Constants
//----------
const ACE_TCHAR * const _FREP_DERR_CREATE_EVENT = "Cannot create a windows event.";
const ACE_TCHAR * const _FREP_DERR_GET_REPDIR = "Cannot get the reported directory.";
const ACE_TCHAR * const _FREP_DERR_CREATE_OHI = "Cannot connect to AFP.";
const ACE_TCHAR * const _FREP_DERR_ATTACH = "Attach to GOH failed.";
const ACE_TCHAR * const _FREP_DERR_WAIT_FAILED = "'WaitForMultipleObjects(...' failed!!";
const ACE_TCHAR * const _FREP_DERR_MOVE_FAILED = "'MoveFile(...' failed!!";
const ACE_TCHAR * const _FREP_DERR_SEND_FAILED = "'sendGOH(...' failed!!";
const ACE_TCHAR * const _FREP_DERR_REFRESH_FAILED = "Cannot refresh the REF* file name!!";


}

ACS_RTR_TRACE_DEFINE(ACS_RTR_GOHFileReporter);

//----------------
// Constructor
//----------------
ACS_RTR_GOHFileReporter::ACS_RTR_GOHFileReporter(std::string transferQueue,
		ACS_RTR_OutputQueue* queue,
		RTR_Events * eventHandler,
		RTR_statistics * statptr,
		std::string msName,
		std::string siteName)
: ACS_RTR_GOHReporter(transferQueue, queue, eventHandler, statptr),
  _msName(msName),
  _siteName(siteName),
  _fRep(NULL)
{

}

//---------------
// Destructor
//---------------
ACS_RTR_GOHFileReporter::~ACS_RTR_GOHFileReporter()
{
	ACS_RTR_TRACE_MESSAGE("In");

	if(_fRep)
	{
		if(_fRep->connected)
		{
			ACS_RTR_TRACE_MESSAGE("Calling detach");
			_fRep->detach();
		}

		delete _fRep;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

void ACS_RTR_GOHFileReporter::fxStop()
{
	ACS_RTR_TRACE_MESSAGE("In");
	_gohStopRequested=true;
	ACS_RTR_TRACE_MESSAGE("Out");
}

int ACS_RTR_GOHFileReporter::open(ACE_HANDLE& stopEvent, ACE_HANDLE& rmEvent)
{
	ACS_RTR_TRACE_MESSAGE("In");
	_gohStopRequested = false;

	m_stopEvent = stopEvent;
	m_jobRemoved = rmEvent;

	int result = activate();

	ACS_RTR_TRACE_MESSAGE("Out, result:<%d>", result);
	return result;
}


//--------------
// Operator
//--------------
int ACS_RTR_GOHFileReporter::svc (void )
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACE_INT32 exitCode = 0, numOfFailure = 0;

	while (!fxInitialize() && !_gohStopRequested)
	{
		ACE_TCHAR trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "Initialization failed for TQ %s!! Error Description: %s", _transferQueue.c_str(), lastErrorDescription());
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
	}

	setState(ACTIVE);

	if (_gohStopRequested)
	{
		setState(IDLE);
		return 0;
	}

	do
	{
		exitCode = fxDoWork();

		if (_gohStopRequested) break;

		if (!exitCode) numOfFailure = 0;
		else
		{
			char trace[512] = {0};
			snprintf(trace, sizeof(trace) - 1, "error in doWork for transfer queue %s. Last error Description: %s", _transferQueue.c_str(), lastErrorDescription());
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

			ACE_Time_Value safeSleep(0,200*1000);
			ACE_OS::sleep(safeSleep);
		}
	} while ((exitCode < 0) && (numOfFailure++ < FREP_MAX_RETRIES_ON_ERROR) && !_gohStopRequested);

	setState(IDLE);	

	ACS_RTR_TRACE_MESSAGE("Out");
	return exitCode;
}

//--------------
// fxInitialize
//--------------
bool ACS_RTR_GOHFileReporter::fxInitialize()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool initialized = false;

	int err = 0;
	ACE_TCHAR reportedDir[FILENAME_MAX] = {0};

	for(int retry = 0; !(initialized = fxGetReportedDirectory(reportedDir, FILENAME_MAX, err)) && !_gohStopRequested && (retry < FREP_MAX_RETRIES_ON_ERROR); ++retry)
	{
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s System Error Code:%d", _FREP_DERR_GET_REPDIR, err);
		if (!setLastError(FREP_ERR_GET_REPDIR, trace, true))  setLastError(FREP_ERR_GET_REPDIR, _FREP_DERR_GET_REPDIR);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
		ACE_Time_Value safeSleep(0,100*1000);
		ACE_OS::sleep(safeSleep);
	}

	unsigned int ohiError = 0;

	if (initialized)
		for (int retry = 0; !(initialized = fxCreateFileReporter(reportedDir, ohiError)) && !_gohStopRequested;  ++retry)
		{
			ACS_RTR_TRACE_MESSAGE("Error: Failed in fxCreateFileReporter");
			char trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "[%s@%s] %s OHI Error Code:%u (retry nr %d)", _msName.c_str(), _transferQueue.c_str(), _FREP_DERR_ATTACH, ohiError, retry);
			if (!setLastError(FREP_ERR_CREATE_OHI, trace, true)) setLastError(FREP_ERR_CREATE_OHI, _FREP_DERR_CREATE_OHI);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
			ACE_OS::sleep(1);
		}

	ACS_RTR_TRACE_MESSAGE("After for loop in fxInitialize fxCreateFileReporter");

	ACS_RTR_TRACE_MESSAGE("Out");
	return initialized;
}

//------------------------------
// fxGetReportedDirectory
//------------------------------
bool ACS_RTR_GOHFileReporter::fxGetReportedDirectory(ACE_TCHAR * dir, ACE_INT32 maxlen, int& err) {
	ACS_RTR_TRACE_MESSAGE("In");

	ACE_TCHAR strAPG43[FILENAME_MAX] = {0};
	if (getRTRdir(strAPG43))
	{
		return false;
	}

	ACE_OS::snprintf(dir, maxlen - 1, "%s%s%s/%s/", strAPG43, RTR_NBI::FILETRANSFER_FOLDER, _msName.c_str(), _transferQueue.c_str());


	bool result = createDir(dir, err);

	ACS_RTR_TRACE_MESSAGE("Out (dir == %s)", dir);
	return result;
}

//--------------------------
// fxCreateFileReporter
//--------------------------
bool ACS_RTR_GOHFileReporter::fxCreateFileReporter(ACE_TCHAR * reportedDir, ACE_UINT32& err)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = true;
	if (!_fRep)
	{	//Instantiate a file report object.
		_fRep = new (std::nothrow) RTRfileReporter(OHI_USERSPACE::SUBSYS,
									OHI_USERSPACE::APPNAME,
									_transferQueue.c_str(),
									OHI_USERSPACE::EVENTFILETXT,
									reportedDir);
		if(!_fRep)
		{
			ACS_RTR_TRACE_MESSAGE("ERROR: RTRfileReporter object creation failed for TQ %s", _transferQueue.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "RTRfileReporter object creation failed for TQ %s", _transferQueue.c_str());
			return false;
		}

		if(!_fRep->attachGOH(err))
		{
			ACS_RTR_TRACE_MESSAGE("ERROR: ATTACH FAILED TO TQ %s. Goh Error: %d", _transferQueue.c_str(), err);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ATTACH FAILED TO TQ %s. Goh Error: %d", _transferQueue.c_str(), err);

			delete _fRep;
			_fRep = 0;
			result = false;
		}
		else
		{
			result = true;
		}
	}
	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

//---------------------------
// setRtrrmEvent
// rtrrm command issued
//---------------------------
bool ACS_RTR_GOHFileReporter::setRtrrmEvent()
{
	ACS_RTR_TRACE_MESSAGE("In");

	return true;
}

//---------
// onStop
//---------
void ACS_RTR_GOHFileReporter::onStop()
{
	ACS_RTR_TRACE_MESSAGE("In");
	if (_fRep && (_fRep->connected))
	{
		ACS_RTR_TRACE_MESSAGE("Calling detach");
		_fRep->detach();
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

//-------------
// fxDoWork
//-------------
ACE_INT32 ACS_RTR_GOHFileReporter::fxDoWork()
{
	ACS_RTR_TRACE_FUNCTION;

	ACS_RTR_TRACE_MESSAGE("\n"
			"#####################################################\n"
			"\tStarting GOH thread for FileJob [TQ: %s, CHS: %s]...\n"
			"#####################################################", _transferQueue.c_str(), _msName.c_str());

	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "\n"
			"#####################################################\n"
			"\tStarting GOH thread for FileJob [TQ: %s, CHS: %s]...\n"
			"#####################################################", _transferQueue.c_str(), _msName.c_str());

	bool execute = false;
	bool rtrrmInd = false;
	unsigned int err = 0;
	ACE_INT32 failureCounter = 0;
	RTRfile * pFile = 0;
	RTR_statistics * pStat = 0;

	const nfds_t nfds = (rtrrm_pos + 1U);
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	fds[stop_pos].fd = m_stopEvent;
	fds[stop_pos].events = POLLIN;

	fds[elem_pos].fd = _outputQueue->getHandle();;
	fds[elem_pos].events = POLLIN;

	fds[rtrrm_pos].fd = m_jobRemoved;
	fds[rtrrm_pos].events = POLLIN;

	do
	{
		ACE_Time_Value fileTimeout(FREP_WAIT_TIMEOUT);
		ACE_INT32 pollResult = ACE_OS::poll(fds, nfds, fileTimeout);

		//----------------------
		// Check for poll error
		if(pollResult < 0)
		{
			setLastError(FREP_ERR_WAIT_FAILED, _FREP_DERR_WAIT_FAILED);
			ACS_RTR_TRACE_MESSAGE("ERROR: [%s@%s] %s. Last Error: %d", _msName.c_str(), _transferQueue.c_str(), lastErrorDescription(), ACE_OS::last_error());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s@%s] %s. Last Error: %d", _msName.c_str(), _transferQueue.c_str(), lastErrorDescription(), ACE_OS::last_error());

			if (++failureCounter > FREP_MAX_RETRIES_ON_ERROR)
			{
				return FREP_ERR_WAIT_FAILED;
			}
			else
			{
				ACE_OS::sleep(1);
			}

			execute = !_gohStopRequested;

			continue;
		}
		else
		{
			failureCounter = 0;
		}


		if(fds[stop_pos].revents & POLLIN)								// THREAD STOP REQUEST
		{
			ACS_RTR_TRACE_MESSAGE("[TQ: %s, CHS: %s] Received a stop request!", _transferQueue.c_str(), _msName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_INFO, "[TQ: %s, CHS: %s] Received a stop request!", _transferQueue.c_str(), _msName.c_str());
			break;
		}

		if(fds[rtrrm_pos].revents & POLLIN)						// JOB REMOVE REQUESTED
		{
			ACS_RTR_TRACE_MESSAGE("[TQ: %s, CHS: %s] Received a job remove request!", _transferQueue.c_str(), _msName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_INFO, "[TQ: %s, CHS: %s] Received a job remove request!", _transferQueue.c_str(), _msName.c_str());
			rtrrmInd = true;
			if (_outputQueue->queueEmpty()) break;
		}

		if( (pollResult == 0) || (fds[elem_pos].revents & POLLIN) ) //ELEMENT IN QUEUE
		{
			if( (_fRep->connected) && (!_outputQueue->queueEmpty()) )
			{
				//------------------------------
				//Cease Alarm
				if (m_attachFileTQerror == 1)
				{
					_eventHandler->ceaseAlarm(alarmInfo::specificProblem1, _transferQueue );
					m_attachFileTQerror = 0;
				}

				//------------------------------
				//get file to transmit
				ACS_RTR_GohTransmitInfo* qPacket = _outputQueue->getFirst();

				if(NULL == qPacket)
				{
					_outputQueue->popData();
					continue;
				}

				pFile = qPacket->getRTRFile();
				if(!pFile)
				{
					delete qPacket;
					_outputQueue->popData();
					continue;
				}

				pStat = qPacket->getStatistics();

				//------------------------
				//transmit the file
				if(pFile->MoveRTRfile(toReportedDir, err))
				{
					if(_fRep->sendGOH(pFile->getFileReportedName(), err))
					{
						//--------------------------
						// cease alarm
						if( m_sendFilesError == 1 )
						{
							_eventHandler->ceaseAlarm(alarmInfo::specificProblem2, _transferQueue );
							m_sendFilesError = 0;
						}

						//-------------------------------------------
						// refresh REF file: rename to new value
						for (int i = 0; (pFile->refreshRefFile(err) == false) && (i < 5); ++i)
						{
							char trace[512] = {0};
							char errorText[512] = {0};
							strerror_r(err, errorText, sizeof(errorText) - 1);
							ACE_OS::snprintf( trace, sizeof (trace) - 1, "[TQ: %s, CHS: %s] %s Error code: <%d>. Description: <%s>", _transferQueue.c_str(), _msName.c_str(), _FREP_DERR_REFRESH_FAILED, err, errorText );
							if(!setLastError(FREP_ERR_REFRESH_FAILED, trace, true)) setLastError(FREP_ERR_REFRESH_FAILED, _FREP_DERR_REFRESH_FAILED);
							ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
							ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: %s", trace);

							ACE_OS::sleep(FREP_MOVE_FAILED_TIMEOUT);
						}

						//-----------------------------------------------------
						// Remove transmitted file from internal queue
						_outputQueue->popData();
						if(pStat) pStat->addReportedData(pFile->getFSize());

						delete qPacket;
						pFile = 0;
						pStat = 0;
					}
					else
					{
						//----------------------
						// sendGOH failed

						//----
						//Log
						{
							char trace[512] = {0};
							ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s OHI Error Code: %u for file %s", _FREP_DERR_SEND_FAILED, err, pFile->getFileReportedName().c_str());
							if (!setLastError(FREP_ERR_SEND_FAILED, trace, true)) setLastError(FREP_ERR_SEND_FAILED, _FREP_DERR_SEND_FAILED);
							ACS_RTR_TRACE_MESSAGE("%s", trace);
							ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
						}

						//---------------
						// raise alarm
						if (m_sendFilesError == 0)
						{
							_eventHandler->initAlarm(alarmInfo::specificProblem2, _transferQueue);
							m_sendFilesError = 1;
						}

						//-----------------------------------------------------
						// Move File back in order to re-transmit it
						for ( ; !(pFile->MoveRTRfile(fromReportedDir, err)) && !_gohStopRequested; )
						{
							//---------------------
							// disk problem

							//Log
							{
								char trace[512] = {0};
								if(ENOENT == err)			ACE_OS::snprintf(trace, sizeof(trace) - 1,"%s direction: 'from ReportedDir'. File sent to GOH. File name: %s. TQ: %s. Error Code: %u", _FREP_DERR_MOVE_FAILED, pFile->getFileName().c_str(), _transferQueue.c_str(), err);
								else if (EACCES == err )	ACE_OS::snprintf(trace, sizeof(trace) - 1,"%s direction: 'from ReportedDir'. Access denied to file: %s. TQ: %s Error Code: %u", _FREP_DERR_MOVE_FAILED, pFile->getFileName().c_str(), _transferQueue.c_str(), err);
								else						ACE_OS::snprintf(trace, sizeof(trace) - 1,"%s direction: 'from ReportedDir'. Probable disk full. File name: %s. TQ: %s Error Code: %u", _FREP_DERR_MOVE_FAILED, pFile->getFileName().c_str(), _transferQueue.c_str(), err);

								ACS_RTR_TRACE_MESSAGE("%s", trace);
								ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

								if (!setLastError(FREP_ERR_MOVE_FAILED, trace, true)) setLastError(FREP_ERR_MOVE_FAILED, _FREP_DERR_MOVE_FAILED);
							}

							ACE_OS::sleep(FREP_MOVE_FAILED_TIMEOUT);

							// File does not exist anymore
							if (ENOENT == err || REF_FILE_ERROR == err)
							{
								char trace[512] = {0};
								ACE_OS::snprintf(trace, sizeof(trace) - 1,
										"[TQ: %s, CHS: %s] %s File name: %s. Error Code: %u. errno: %d. Remove file from sending queue. The file does not exist.\n",
										_transferQueue.c_str(), _msName.c_str(), _FREP_DERR_MOVE_FAILED, pFile->getFileName().c_str(), err, errno);

								ACS_RTR_TRACE_MESSAGE("%s", trace);
								ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

								_outputQueue->popData(); //remove from queue
								delete qPacket;
								pFile = 0;
								pStat = 0;
								break;
							}
						}
					}
				}
				else
				{
					// move failed, probable disk full
					char trace[512] = {0};
					if(( EDQUOT == err) || (ENOSPC == err) )	// Quota exceeded OR No space left on device
					{
						ACE_OS::snprintf(trace, sizeof(trace) - 1,"[TQ: %s, CHS: %s] %s disk full. File name: %s. Error Code: %u. errno: %d. %s\n",
								_transferQueue.c_str(), _msName.c_str(), _FREP_DERR_MOVE_FAILED, pFile->getFileName().c_str(), err, errno, (pFile->isRestored() ? "The file was restored." : ""));
						if (!setLastError(FREP_ERR_MOVE_FAILED, trace, true)) setLastError(FREP_ERR_MOVE_FAILED, _FREP_DERR_MOVE_FAILED);
					}
					else if (ENOENT == err || REF_FILE_ERROR == err)		// No such file or directory
					{
						ACE_OS::snprintf(trace, sizeof(trace) - 1,"[TQ: %s, CHS: %s] %s File name: %s. Error Code: %u. errno: %d. Remove file from sending queue.\n",
								_transferQueue.c_str(), _msName.c_str(), _FREP_DERR_MOVE_FAILED, pFile->getFileName().c_str(), err, errno);
						if (!setLastError(FREP_ERR_MOVE_FAILED, trace, true)) setLastError(FREP_ERR_MOVE_FAILED, _FREP_DERR_MOVE_FAILED);

						_outputQueue->popData(); //remove from queue
						delete qPacket;
						pFile = 0;
						pStat = 0;
					}
					else
					{
						ACE_OS::snprintf(trace, sizeof(trace) - 1, "[TQ: %s, CHS: %s] %s disk problem. File name: %s. Error Code: %u. errno: %d. %s \n",
								_transferQueue.c_str(), _msName.c_str(), _FREP_DERR_MOVE_FAILED, pFile->getFileName().c_str(), err, errno, (pFile->isRestored() ? "The file was restored." : " "));

						if (!setLastError(FREP_ERR_MOVE_FAILED, trace, true)) setLastError(FREP_ERR_MOVE_FAILED, _FREP_DERR_MOVE_FAILED);
					}

					ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
					ACE_OS::sleep(FREP_MOVE_FAILED_TIMEOUT);
				}
			}
			else
			{
				if (rtrrmInd && _outputQueue->queueEmpty())
				{
					// queue is empty or ohi is disconnected
					break;
				}
				else if (!_fRep->connected)
				{
					//it's not connected to GOH
					if (_statptr)
					{
						//increment Attempts in statistics
						_statptr->incAttachGOHattempts();
					}

					if(!_fRep->attachGOH(err))
					{
						//try to connect to GOH
						char trace[512] = {0};
						int nAtt = 0;
						_fRep->GetFailedAttempts(nAtt);
						ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s Attempt number: %d Transfer queue:%s OHI Error Code:%u", _FREP_DERR_ATTACH, nAtt, _transferQueue.c_str(), err);

						if (!setLastError(FREP_ERR_ATTACH, trace, true)) setLastError(FREP_ERR_ATTACH, _FREP_DERR_ATTACH);

						ACS_RTR_TRACE_MESSAGE("%s", trace);
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

						if (m_attachFileTQerror == 0)
						{
							_eventHandler->initAlarm(alarmInfo::specificProblem1, _transferQueue);
							m_attachFileTQerror = 1;
						}

						ACE_Time_Value attachFailedTimeout(0, FREP_ATTACH_FAILED_TIMEOUT*1000);
						ACE_OS::sleep(attachFailedTimeout);
					}
				}
			}
		}

		//--------------------------
		// Check thread status
		if(!rtrrmInd)
		{
			execute = !_gohStopRequested;
		}
		else
		{
			execute = !(_outputQueue->queueEmpty()); // rtrrm issued

			if (execute)
			{
				ACS_RTR_TRACE_MESSAGE("WARNING: [TQ: %s] Remove Job requested, but there are %zu not transmitted files. Ignoring request.", _transferQueue.c_str(), _outputQueue->queueSize());
				ACS_RTR_LOG(LOG_LEVEL_WARN, "[TQ: %s] Remove Job requested, but there are %zu not transmitted files. Ignoring request.", _transferQueue.c_str(), _outputQueue->queueSize());
			}
		}

		//-----------------------------
		// Check error of handles
		for(unsigned int idx = 0; idx < nfds; ++idx)
		{
			// Compare with : Error condition or Hung up or Invalid polling request
			if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				ACS_RTR_TRACE_MESSAGE("ERROR: [TQ: %s] poll on handle pos:<%d> failed error:<%d> revents:<%d>", _transferQueue.c_str(), idx, errno, fds[idx].revents);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[TQ: %s] poll on handle pos:<%d> failed error:<%d> revents:<%d>", _transferQueue.c_str(), idx, errno, fds[idx].revents);
				execute = false;
				break;
			}
		}

	} while(execute);

	ACS_RTR_TRACE_MESSAGE("\n"
			"#####################################################\n"
			"\tTerminated GOH thread for FileJob [TQ: %s, CHS: %s]...\n"
			"#####################################################", _transferQueue.c_str(), _msName.c_str());

	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "\n"
			"#####################################################\n"
			"\tTerminated GOH thread for FileJob [TQ: %s, CHS: %s]...\n"
			"#####################################################", _transferQueue.c_str(), _msName.c_str());

	return 0;
}

