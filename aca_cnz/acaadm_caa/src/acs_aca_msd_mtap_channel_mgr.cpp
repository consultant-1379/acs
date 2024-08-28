//****************************************************************
//
// NAME
//   ACAMSD_MTAP_Channelmanager.cpp

// COPYRIGHT
//   COPYRIGHT Ericsson Lab Pagani, Italy 2006.
//   All rights reserved.
//
//   The Cofpyright to the computer program(s) herein
//   is the property of Ericsson Lab Pagani, Italy.
//   The program(s) may be used and/or copied only with 
//   the written permission from Ericsson Lab Pagani or in 
//   accordance with the terms and conditions stipulated in the 
//   agreement/contract under which the program(s) have been 
//   supplied.

//  DOCUMENT NO
//    CAA 137 ----

//  AUTHOR 
//    XLANSRI		Sridhar Lanka 

//  DESCRIPTION
//    ....................

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO          DATE          NAME       DESCRIPTION
//    A               2012-10-03    xlansri    Product Release
//****************************************************************

#include <sys/types.h>
#include <sys/eventfd.h>

#include <string>
#include <sstream>
#include <new>
#include <pthread.h>
#include <stdexcept>

#include "ace/ACE.h"
#include "ace/Event.h"
#include "ace/Event_Handler.h"
#include "ace/Thread_Manager.h"
#include "ace/Handle_Set.h"

// XCASALE: Uncomment this line only for debugging purpose
//#define ACS_ACA_DEBUGGING_LOG_ENABLED

#include "acs_aca_macros.h"
#include "acs_aca_msd_service.h"
#include "acs_aca_msd_ms_manager.h"
#include "acs_aca_common.h"
#include "acs_aca_defs.h"
#include "acs_aca_system_exception.h"
#include "acs_aca_logger.h"
#include "acs_aca_msd_mtap_channel_mgr.h"

using Ericsson::System::Exception;

namespace {
	const char * const g_class_name = "ACAMSD_MTAP_ChannelManager";
}


//****************************************************************
// Constructors and initializer
//****************************************************************
ACAMSD_MTAP_ChannelManager::ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, const short index)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(index), _cpSystemId(SINGLE_CP_ID),
	  _isMultipleCPSystem(false), _msParameters(0), _incomingSessionQueue(), _lastActivityTime(0), _sessions(),
	  _sessionCount(0), _sessionInactivityTimeout(SESSION_INACTIVITY_DEFAULT_TIMEOUT), _tooManyConnectionsCount(0),
	  _echoOn(false), _forceTerminate(false), _currentVersion(3), _myMsgStore(0), _myMsgCollection(0),
	  _myCurrentFileNumber(0), _myMsManager(myMsManager), _tryToStore(false), _firstCPMessage(true),
	  _syncAccess(), _events_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

ACAMSD_MTAP_ChannelManager::ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, const ACAMS_Parameters * msParameters, int sessionInactivityTimeout)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(0), _cpSystemId(SINGLE_CP_ID),
	  _isMultipleCPSystem(false), _msParameters(msParameters), _incomingSessionQueue(),_lastActivityTime(0), _sessions(),
		_sessionCount(0), _sessionInactivityTimeout(sessionInactivityTimeout), _tooManyConnectionsCount(0),
		_echoOn(false), _forceTerminate(false), _currentVersion(3), _myMsgStore(0), _myMsgCollection(0),
		_myCurrentFileNumber(0), _myMsManager(myMsManager), _tryToStore(false), _firstCPMessage(true),
		_syncAccess(), _events_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

ACAMSD_MTAP_ChannelManager::ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId, bool isMultipleCPSystem, short index)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(index), _cpSystemId(cpSystemId),
	  _isMultipleCPSystem(isMultipleCPSystem), _msParameters(0), _incomingSessionQueue(), _lastActivityTime(0), _sessions(),
	  _sessionCount(0), _sessionInactivityTimeout(SESSION_INACTIVITY_DEFAULT_TIMEOUT), _tooManyConnectionsCount(0),
		_echoOn(false), _forceTerminate(false), _currentVersion(3), _myMsgStore(0), _myMsgCollection(0),
		_myCurrentFileNumber(0), _myMsManager(myMsManager), _tryToStore(false), _firstCPMessage(true),
		_syncAccess(0), _events_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

ACAMSD_MTAP_ChannelManager::ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId,	bool isMultipleCPSystem, const ACAMS_Parameters * msParameters,	int sessionInactivityTimeout)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(0), _cpSystemId(cpSystemId),
	  _isMultipleCPSystem(isMultipleCPSystem), _msParameters(msParameters), _incomingSessionQueue(),_lastActivityTime(0),
	  _sessions(), _sessionCount(0), _sessionInactivityTimeout(sessionInactivityTimeout), _tooManyConnectionsCount(0),
		_echoOn(false), _forceTerminate(false), _currentVersion(3), _myMsgStore(0), _myMsgCollection(0),
		_myCurrentFileNumber(0), _myMsManager(myMsManager), _tryToStore(false), _firstCPMessage(true),
		_syncAccess(0), _events_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

int ACAMSD_MTAP_ChannelManager::initialize_post_ctor() {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Index = %d, MS = '%s', CP = %d, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	try {
		(_isMultipleCPSystem)
				? _myMsgStore = new (std::nothrow) ACAMSD_MsgStore(_cpSystemId, indextoParam)
				: _myMsgStore = new (std::nothrow) ACAMSD_MsgStore(indextoParam);
	}
	catch (std::runtime_error & ex) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Exception (runtime_error) caught while creating "
				"ACAMSD_MsgStore object, message: '%s'", ex.what());
		return -1;
	}
	catch (...) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Exception caught while creating ACAMSD_MsgStore object");
		return -2;
	}

	if ( !_myMsgStore ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for ACAMSD_MsgStore object, returning -1");
		return -3;
	}

	const int call_result = _myMsgStore->initialize_post_ctor(_myMsgCollection);
	if ( call_result ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to initialize the ACAMSD_MsgStore object, call_result = %d, returning -2", call_result);
		delete _myMsgStore; _myMsgStore = 0;
		return -4;
	}

	unsigned char mtapVer = _myMsgStore->getVersion();

	if ((0 < mtapVer) && (mtapVer < 4))
		_currentVersion = mtapVer; //YES

	if (ACS_MSD_Service::getAddress(indextoParam))
		_echoOn = ACS_MSD_Service::getAddress(indextoParam)->echoValue;

	//set inactivity timeout
	if (_echoOn)
		_sessionInactivityTimeout = SESSION_INACTIVITY_TIMEOUT_ECHO_ON;
	else
		_sessionInactivityTimeout = SESSION_INACTIVITY_TIMEOUT_ECHO_OFF;

	for (int i = 0; i < 64; ++i) {
		_sessions[i].first = 0; _sessions[i].second = 0;
	}

	_events_fd = ::eventfd(0, 0);
	if ( _events_fd == ACE_INVALID_HANDLE ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create 'eventfd' object - errno = %d, returning -4", errno);
		delete _myMsgStore; _myMsgStore = 0;
		return -5;
	}

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', CP = %d, _currentVersion = %i, _echoOn = %i, _sessionInactivityTimeout = %i",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _currentVersion, _echoOn, _sessionInactivityTimeout);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// Destructor and finalizer
//****************************************************************
ACAMSD_MTAP_ChannelManager::~ACAMSD_MTAP_ChannelManager () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', CP = %d, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);
	
	fxFinalize();
	::close(_events_fd);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACAMSD_MTAP_ChannelManager::fxFinalize () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	fxDeleteActiveSessions();
	fxDeleteIncomingSessions();
	delete _myMsgStore; _myMsgStore = 0;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// addSession
//****************************************************************
int ACAMSD_MTAP_ChannelManager::addSession (ACS_DSD_Session * pSession) {
	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Stop requested by someone: _state == %d, _sessionCount == %i", _state, _sessionCount);

		try {
			bool sendOk = ACAMSD_MTAP_Protocol::sendError(*pSession, ACAMSD_MTAP_Protocol::UnspecifiedError);(void)sendOk;
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Error 'UnspecifiedError' sent!");
		} catch (...) {
		}
		
		if (_tooManyConnectionsCount++ >= (MAX_RETRIES_ON_TOO_MANY_CONNECTION * 2)) {
			_forceTerminate = true;
			::usleep(5 * 1000); //force thread context switch
		}

		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Returning STOP!");
		return RETURN_STOP;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		int totalSessionCount = _sessionCount + static_cast<int>(_incomingSessionQueue.size());
		int param_noOfConn = ACS_MSD_Service::getAddress(indextoParam)->noOfConn;

		if ((totalSessionCount >= param_noOfConn) || (totalSessionCount >= 64)) {
			try {
				bool sendOk = ACAMSD_MTAP_Protocol::sendError(*pSession, ACAMSD_MTAP_Protocol::TooManyConnections);(void)sendOk;//To be removed later (done to avoid warnings)
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Error 'TooManyConnections' sent, param_noOfConn == %d", param_noOfConn);
			} catch (...) {
			}

			if (_tooManyConnectionsCount++ >= MAX_RETRIES_ON_TOO_MANY_CONNECTION) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Too many retries of session adding, stopping MTAP");
				stopMTAP();
			}

			::usleep(5 * 1000); //force thread context switch

			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Returning TOO_MANY_CONNECTIONS!");
			return RETURN_TOO_MANY_CONNECTIONS;
		} else {
			_tooManyConnectionsCount = 0;
		}
		
		//Add the incoming session to the internal queue.
		_incomingSessionQueue.push(sessionInfo_t(pSession, time(0)));
	ACS_ACA_CRITICAL_SECTION_GUARD_END();
             
	return RETURN_OK;
}

//****************************************************************
// ackToCP
//****************************************************************
bool ACAMSD_MTAP_ChannelManager::ackToCP(ACAMSD_MTAP_Message * msgToAck, ACAMSD_MTAP_Protocol::ErrorCode result) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	unsigned char * buffer = 0;
	unsigned sessionIndex = msgToAck->getChannel();
	ACS_DSD_Session * pSession = _sessions[sessionIndex].first;

	if (!pSession && ((sessionIndex = fxSelectLowestPrioritySession()) < MAX_WAIT_OBJECTS)) {
		pSession = _sessions[sessionIndex].first; //New session found
	}
	
	if (!pSession) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "No session found for index == %u", sessionIndex);
		return false;
	}

	unsigned short len = 0;

	try {
		len = msgToAck->getResponse(buffer, result);
	} catch (...) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'msgToAck->getResponse' threw an exception!");
		return false;
	}
	
	if (ACAMSD_MTAP_Protocol::send(*pSession, buffer, len) < 0) { //Error while sending response.
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMSD_MTAP_Protocol::send' failed, error_code = %d - "
				"deleting the session at index %u", pSession->last_error(), sessionIndex);
		fxDeleteSession(sessionIndex);
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

//****************************************************************
// deleteDataFile
//****************************************************************
int ACAMSD_MTAP_ChannelManager::deleteDataFile (std::string fileName) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning...");
		return 1;
	}

	if (!_myMsgStore) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "My MsgStore pointer is NULL, returning");
		return 3;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		_myMsgStore->deleteDataFile(fileName);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "File '%s' successfully deleted!", fileName.c_str());
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// getNextMsgNumber
//****************************************************************
int ACAMSD_MTAP_ChannelManager::getNextMsgNumber (unsigned long long & msgNumber) {
	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning...");
		return 1;
	}

	if (!_myMsgStore) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "My MsgStore pointer is NULL, returning");
		return 3;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		msgNumber = _myMsgStore->getNextMsgNumber();
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return 0;
}

int ACAMSD_MTAP_ChannelManager::stopMTAP () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Stopping this MTAP Channel Manager - index = %d, MS = %s, CP = %u, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	_state = THREAD_STATE_STOPPED;

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Signaling stop event! - index = %d, MS = %s, CP = %u, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	eventfd_t stop_event = 1U;
	if (::eventfd_write(_events_fd, stop_event))
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to signal the stop event! - errno = %d", errno);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}


int ACAMSD_MTAP_ChannelManager::notify_event(ACAMSD_MTAP_ChannelManager::Event_type evt) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int retval = 0;

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Signaling event '%#010x' ! - index = %d, MS = %s, CP = %u, TID = %ld",
			evt, indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	if((retval = ::eventfd_write(_events_fd, evt))) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to signal event '%#010x'. Stopping this MTAP Channel Manager ! - index = %d, MS = %s, CP = %u, TID = %ld - errno = %d",
					evt, indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid, errno);
		_state = THREAD_STATE_STOPPED;
		retval = -1;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retval;
}


//****************************************************************
// threadMain
//****************************************************************
int ACAMSD_MTAP_ChannelManager::workerMain () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "MTAP Channel Manager Thread, starting to work - index = %d, MS = '%s', CP = %d, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	int exitCode = 0;
	_lastActivityTime = time(0);

	for (;;) {
		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, breaking...");
			break;
		}

		exitCode = fxActivateIncomingSessions();
		if (exitCode < 0) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'fxActivateIncomingSessions' returned %d, breaking...", exitCode);
			break;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, breaking...");
			break;
		}
		
		exitCode = fxValidateSessions();
		if (exitCode < 0) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'fxValidateSessions' returned %d, breaking...", exitCode);
			break;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, breaking...");
			break;
		}

		if ((exitCode = fxDoWork()) < 0) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'fxDoWork' returned %d, breaking...", exitCode);
			break;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, breaking...");
			break;
		}
		
		ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
			if (_sessionCount) {
				time_t inactiveTime = static_cast<time_t>((_sessionCount + static_cast<int>(_incomingSessionQueue.size()) + 2)
								* _sessionInactivityTimeout);

				time_t now = ::time(0);
				if (inactiveTime < (now - _lastActivityTime)) {
					ACS_ACA_LOG(LOG_LEVEL_INFO, "This thread has been inactive for too long, exiting");
					break;
				}
			}
		ACS_ACA_CRITICAL_SECTION_GUARD_END();
	}
	
	if (_state == THREAD_STATE_RUNNING) {
		fxDeleteActiveSessions(true);
		fxDeleteIncomingSessions(true);
		stopMTAP(); //stop myself
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return exitCode;
}

//****************************************************************
// fxActivateIncomingSessions
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxActivateIncomingSessions () {
	int freePos = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		while (!_incomingSessionQueue.empty()) {
			//Find the first free position
			while ((freePos < 64) && _sessions[freePos].first) {
				freePos++;
			}

			if (freePos >= 64) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "The internal sessions array is full");
				return -5;
			}

			_sessions[freePos++] = _incomingSessionQueue.front();
			_sessionCount++;
			_incomingSessionQueue.pop();

			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "New session added at index %d, _sessionCount = %d", freePos, _sessionCount);

			if (_state == THREAD_STATE_STOPPED) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning STOP");
				return RETURN_STOP;
			}
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return RETURN_OK;
}

//****************************************************************
// fxValidateSessions
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxValidateSessions () {
	for (unsigned i = 0; i < 64; ++i) {
		time_t now = ::time(0);
		if (_sessions[i].first && (_sessionInactivityTimeout < (now - _sessions[i].second))) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "This session (index %u) has been inactive for more that %d seconds, "
					"deleting it and sending the STOP to the CP!", i, _sessionInactivityTimeout);
			fxDeleteSession(i, true);
		}
	}
	return RETURN_OK;
}

//****************************************************************
// fxDoWork
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxDoWork () {
	if (_sessionCount <= 0) {
		int num_of_events = 1;
		ACE_Time_Value poll_timeout(0, 500000);	// 500 milliseconds
		struct pollfd pollfds[num_of_events];
		pollfds[0].fd = _events_fd;
		pollfds[0].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
		pollfds[0].revents = 0;

		errno = 0;
		const int poll_result = ACE_OS::poll(pollfds, num_of_events, &poll_timeout);
		const int errno_save = errno;

		if ( poll_result < 0 ) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "POLL WAIT FAILED: errno_save = %d", errno_save);
			return (errno_save == EINTR ? 5 : -5);
		}
		else if ( poll_result == 0 ) {
			ACS_ACA_LOG(LOG_LEVEL_TRACE, "No sessions found for the couple (%s,%d), returning",
						ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
		}
		else {
			// event detected on '_event_fd' file descriptor
			eventfd_t event_cnt = 0;
			if(::eventfd_read(_events_fd, &event_cnt)) {
				int errno_save = errno;
				ACS_ACA_LOG(LOG_LEVEL_WARN,
						"An event for the couple (%s,%d) has been signaled, but failed to read from the eventfd object ! - errno = %d ",
									ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, errno_save);
			}

			if ( pollfds[0].revents & (POLLIN | POLLPRI) ) {
				// understand what event has occurred
				if(_state == THREAD_STATE_STOPPED) { // the signaled event is a 'stop request' event
					ACS_ACA_LOG(LOG_LEVEL_WARN, "The stop event for the couple (%s,%d) has been signaled, returning 1",
							ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
					return 1;
				}
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
				// being here the signaled event should be an 'OS caching parameter change' event;
				// set caching strategy for Data files
				if(_myMsgStore->setCachingStrategyForDataFiles(ACS_MSD_Service::isOsCachingEnabled())) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "unable to set data files caching strategy for the couple(%s,%d), returning -5",
										ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
					return -5;
				}
#endif
			}
			else {	// Error on event fd
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error signaled on event fd (revents = %d) for the couple(%s,%d), returning -5",
						pollfds[0].revents, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
				return -5;
			}
		}

		return 0;
	}

	ACE_INT32 eventCount = 0;
	ACE_INT32 hCount = 1;
	ACE_HANDLE events[64];
	unsigned reverseIndex[64];
	ACS_DSD_Session * pSession = 0;

	//Initialize the handle array for the WaitForMultipleObjects call
	for (unsigned i = 0; i < 64; ++i) {
		if ((pSession = _sessions[i].first)) {
			int myStatus = pSession->get_handles(&events[i],hCount);

			if (myStatus < 0) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Bad session found at index i = %u, 'get_handles' returned %d", i, myStatus);
				return -1;
			} else {
				reverseIndex[eventCount++] = i;
			}
		}
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		if (eventCount != _sessionCount) { //Internal fields corrupt
			_sessionCount = eventCount;
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Internal field corruption: the new _sessionCount is %d", _sessionCount);
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	// Increment the value of eventCount in order to include also the stop event
	eventCount++;
	
	struct pollfd myPollfds[eventCount];
	for (ACE_INT32 i = 0; i < (eventCount - 1); i++) {
		myPollfds[i].fd = events[i];
		myPollfds[i].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
		myPollfds[i].revents = 0;
	}

	myPollfds[eventCount - 1].fd = _events_fd;
	myPollfds[eventCount - 1].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
	myPollfds[eventCount - 1].revents = 0;

	int errorLevel = 0;

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Calling poll (MTAP) for (%s,%d) with %d events",
				ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, eventCount);

	ACE_Time_Value selectTime;
	selectTime.set(0, 100000);

	errno = 0;
	ACE_INT32 waitResult = ACE_OS::poll(myPollfds, eventCount, &selectTime);
	int errno_save = errno;

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}
	
	switch (waitResult) {
	case -1: { // WAIT_FAILED:
			ACS_ACA_LOG(LOG_LEVEL_WARN, "POLL WAIT FAILED: errno_save = %d", errno_save);
			errorLevel = (errno_save == EINTR ? 5 : -5);
		}
		break;

	case 0: // WAIT_TIMEOUT:
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Expired timeout - index = %d, MS = %s",
					indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);
		break;

	default: {
			int index = 0;
			bool validEvent = false;

			if ( myPollfds[eventCount -1].revents & (POLLIN | POLLPRI) ) {
				// an event has been signaled. It can be a 'stop request' event, or an 'OS caching parameter change' event.
				// read the eventfd object counter (so resetting it to zero !)
				eventfd_t event_cnt = 0;
				if(::eventfd_read(_events_fd, &event_cnt)) {
					int errno_save = errno;
					ACS_ACA_LOG(LOG_LEVEL_WARN,
							"An event for the couple (%s,%d) has been signaled, but failed to read from the eventfd object ! - errno = %d ",
										ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, errno_save);
				}

				if(_state == THREAD_STATE_STOPPED) { // the signaled event is a 'stop request' event
					ACS_ACA_LOG(LOG_LEVEL_WARN, "The stop event for the couple (%s,%d) has been signaled, returning 1",
							ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
					return 1;
				}

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
				// being here the signaled event should be an 'OS caching parameter change' event;
				// set caching strategy for Data files
				if(_myMsgStore->setCachingStrategyForDataFiles(ACS_MSD_Service::isOsCachingEnabled())) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "unable to set data files caching strategy for the couple(%s,%d), returning -5",
										ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
					return -5;
				}
#endif
			}

			for (index = 0; index < (eventCount - 1); index++) {
				if (myPollfds[index].revents & (POLLIN | POLLPRI)) {
					validEvent = true;
					break;
				}
				else if (myPollfds[index].revents & (POLLRDHUP | POLLHUP | POLLERR | POLLNVAL)) {
						ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Error on the session at index %d: revents is %d, deleting it",
								index, myPollfds[index].revents);
						errorLevel = 5;
						fxDeleteSession(reverseIndex[index]);
				}
			}

			if (validEvent) {
				errorLevel = fxExchange(reverseIndex[index]);
				_lastActivityTime = time(0);
			}
		}
		break;
	}

	return errorLevel;
}

//****************************************************************
// fxDeleteSession
//****************************************************************
void ACAMSD_MTAP_ChannelManager::fxDeleteSession (unsigned index, bool sendStopPrimitive) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_DSD_Session * pSession = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		if ((index >= 64) || !(pSession = _sessions[index].first))	return;

		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Deleting session at index %u", index);

		if (sendStopPrimitive) {
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Sending STOP primitive to the CP on the session %u", index);
			ACAMSD_MTAP_Protocol::sendStop(*pSession);
		}

		pSession->close();
		delete pSession; pSession=0;
		_sessionCount--;
		_sessions[index].first = 0;
		_sessions[index].second = static_cast<time_t>(0);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// fxExchange
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxExchange (unsigned sessionIndex) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Exchanging using session at index %u", sessionIndex);

	static const unsigned BUFFER_SIZE = 64 * 1024 + 8; //64Kb + 8b

	if (sessionIndex >= 64) { // Index out of range
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Session index out of range");
		return 3;
	}

	int result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
	unsigned char buffer[BUFFER_SIZE];
	ACAMSD_MTAP_Protocol::Primitive primitive;
	unsigned long numberOfOctets = 0UL;
	ACAMSD_MTAP_Protocol::ErrorCode error = ACAMSD_MTAP_Protocol::NoError;
	unsigned char fileNumber = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		ACS_DSD_Session * pSession = _sessions[sessionIndex].first;

		if (!pSession) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "The session pointer is NULL");
			return 3;
		}

		// Get the primitive code
		primitive = ACAMSD_MTAP_Protocol::getPrimitive(*pSession, numberOfOctets, buffer);

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

	switch (primitive) {
			case ACAMSD_MTAP_Protocol::Do_Will:
			result = fxHandleOptionNegotiation(pSession, buffer, numberOfOctets);
			break;

		case ACAMSD_MTAP_Protocol::EchoCheck:
			result = fxHandleEchoCheck(pSession, buffer, numberOfOctets);
			break;

		case ACAMSD_MTAP_Protocol::GetParameter:
			result = fxHandleGetParameter(pSession, buffer, numberOfOctets);
			break;

		case ACAMSD_MTAP_Protocol::PutRecord:
		case ACAMSD_MTAP_Protocol::Skip:
			result = fxHandlePutRecord(sessionIndex, pSession, buffer, numberOfOctets, primitive);
			_tryToStore = (result == ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok) || _tryToStore;
			break;

		case ACAMSD_MTAP_Protocol::Synchronize:
			result = fxHandleSynchronize(pSession, buffer, numberOfOctets);

			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Synchronize primitive received, deleting "
					"all the sessions except the current one (index = %u), _sessionCount = %d!",
					sessionIndex, _sessionCount);

			// Close all (possibly) open sessions but "my" own.
			for (unsigned i = 0; i < 64; ++i) {
				if (i ^ sessionIndex) {
					fxDeleteSession(i, true);
				}
			}
			break;

		case ACAMSD_MTAP_Protocol::UnlinkAllFiles:
			if (numberOfOctets != 4) {
				result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
			} else {
				result = fxHandleUnlink(pSession, buffer, error);
			}
			break;

		case ACAMSD_MTAP_Protocol::UnlinkFile:
			error = ACAMSD_MTAP_Protocol::unpackUnlinkFile(buffer, numberOfOctets, fileNumber);
			if (error != ACAMSD_MTAP_Protocol::NoError) {
				result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
			} else { // Let MTAP file numbering override any possible mis-correspondence of file numbering.
				_myCurrentFileNumber = fileNumber;
				result = fxHandleUnlink(pSession, buffer, error);
			}
			break;

		case ACAMSD_MTAP_Protocol::Stop:
			result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
			break;

		case ACAMSD_MTAP_Protocol::Error:
			result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
			break;

		case ACAMSD_MTAP_Protocol::None:
			result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
			break;

		case ACAMSD_MTAP_Protocol::PeerClosedConnection:
			result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_PeerClosedConnection;
			break;

		default:
			result = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
			break;
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	// Act upon the result from the operation
	//***************************************
	switch (result) {
	case ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt:
	case ACAMSD_MTAP_ChannelManager::MTAP_RESULT_PeerClosedConnection:
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "The CP closed the connection or the connection is corrupted, result == %d", result);
		fxDeleteSession(sessionIndex, (result == ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt));
		break;

	case ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok: // Successful
		if (_tryToStore) {
			// Now store any messages collected. Messages that are stored will also
			// be acknowledged to CP by MsgStore calling 'ACAMSD_MS_Deamon::ackToCP'
			//*********************************************************************
			int numberOfMsgStored = 0;  //HC91955, variable was not initialized to 0.

			if (!_myMsgStore->store(this, numberOfMsgStored)) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "The message store is FULL");
				return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_MsgStoreFull;
			}

			if (numberOfMsgStored > 0) {
				if (_myMsManager) {
					if (_firstCPMessage) {
						if (!_myMsManager->setCPConnectionNotificationEvent()) {
						} else _firstCPMessage = false;
					}
					ACS_ACA_LOG(LOG_LEVEL_TRACE, "Notifying the presence of messages to the clients");

					if (_myMsManager->messageAvailableFromMTAP(_cpSystemId)) {
						ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to notify messages availability to clients");
					}
				} else {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "MS manager pointer is NULL, very strange");
				}
			}

			_tryToStore = false;
		}
		break;
	}  // End switch

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		_sessions[sessionIndex].second = time(0);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	return result;
}

//****************************************************************
// fxDeleteActiveSessions
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxDeleteActiveSessions (bool sendStopPrimitive) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_DSD_Session * pSession = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		for (int i = 0; i < 64; ++i) {
			if ((pSession = _sessions[i].first)) {
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Deleting session at index %d, _sessionCount = %d", i, _sessionCount);

				if (sendStopPrimitive) {
					bool sendStopOk = ACAMSD_MTAP_Protocol::sendStop(*pSession);(void) sendStopOk;//To be removed later
					ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Stop primitive sent on on the session %d!", i);
				}

				pSession->close();
				delete pSession; pSession=0;

				_sessions[i].first = 0;
				_sessions[i].second = static_cast<time_t>(0);
			}
		}
		_sessionCount = 0;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// fxDeleteIncomingSessions
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxDeleteIncomingSessions (bool sendStopPrimitive) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_DSD_Session * pSession = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		while (!_incomingSessionQueue.empty()) {
			const sessionInfo_t & sessionInfo = _incomingSessionQueue.front();

			if ((pSession = sessionInfo.first)) {
				if (sendStopPrimitive) {
					bool sendStopOk = ACAMSD_MTAP_Protocol::sendStop(*pSession);(void)sendStopOk;//to remove later
					ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Stop primitive sent!");
				}

				pSession->close();
				delete pSession; pSession=0;
			}

			_incomingSessionQueue.pop();
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// fxHandleOptionNegotiation
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxHandleOptionNegotiation (
		ACS_DSD_Session * pSession,
		const unsigned char * buffer,
		unsigned long numberOfOctets) {
	ACAMSD_MTAP_Protocol::Option option = ACAMSD_MTAP_Protocol::Version;
	unsigned char optionValue = 0;

	ACAMSD_MTAP_Protocol::ErrorCode result = ACAMSD_MTAP_Protocol::unpackOptionNegotiation(buffer, numberOfOctets,
			option, optionValue);
	if (result != ACAMSD_MTAP_Protocol::NoError) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to unpack OptionNegotiation primitive, error = %d", result);
		return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}
	 
	// Here it is decided if the CP version of MTAP corresponds with our supported version
	if (option != ACAMSD_MTAP_Protocol::Version) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The CP MTAP version doesn't correspond with supported version");
		return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}

	if (optionValue < ACAMSD_MTAP_ChannelManager::SUPPORTED_VERSION_1) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Unspecified option value");
		result = ACAMSD_MTAP_Protocol::UnspecifiedError;
	}

	if (optionValue > ACAMSD_MTAP_ChannelManager::SUPPORTED_VERSION_3) {
		optionValue = ACAMSD_MTAP_ChannelManager::SUPPORTED_VERSION_3;
	}

	_myMsgStore->putVersion(_currentVersion = optionValue);

	int resultCode = 0;
	if(ACAMSD_MTAP_Protocol::sendOptionNegotiationResponse(*pSession, buffer, optionValue, result)) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "OptionNegotiationResponse primitive sent!");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send OptionNegotiationResponse primitive");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}

	return resultCode;
}

//****************************************************************
// fxHandleEchoCheck
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxHandleEchoCheck (
		ACS_DSD_Session * pSession,
		const unsigned char * buffer,
		unsigned long numberOfOctets) {
	if (numberOfOctets != 4) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Number of octets is different from the expected one!");
		return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}

	// We don't check anything in the 'Echo-check' primitive, as it contains
	// no data and the response has no 'Error' field. Just send the response
	int resultCode = 0;
	if(ACAMSD_MTAP_Protocol::sendEchoCheckResponse(*pSession, buffer)) {
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "EchoCheckResponse primitive sent!");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send EchoCheckResponse primitive");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}

	return resultCode;
}

//****************************************************************
// fxHandleGetParameter
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxHandleGetParameter (
		ACS_DSD_Session * pSession,
		const unsigned char * buffer,
		unsigned long numberOfOctets) {
	std::list<unsigned char> parameterList;
	std::list<unsigned long> parameterValueList;

	ACAMSD_MTAP_Protocol::ErrorCode result = ACAMSD_MTAP_Protocol::unpackGetParameter(buffer, numberOfOctets, parameterList);
	
	if (result != ACAMSD_MTAP_Protocol::NoError) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to unpack GetParameter primitive, error = %d", result);
		return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}
	
	std::list<unsigned char>::iterator it = parameterList.begin();
	// Get value for all parameters requested. Skip any unknown parameter.
	while (it != parameterList.end()) {
		ACAMSD_MTAP_Protocol::Parameter parameter = static_cast<ACAMSD_MTAP_Protocol::Parameter>(*it++);

		switch (parameter) {
		case ACAMSD_MTAP_Protocol::WinSize: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->winSize); 
			break;
	
		case ACAMSD_MTAP_Protocol::NoOfConn: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->noOfConn); 
			break;

		case ACAMSD_MTAP_Protocol::NoOfRecords: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->noOfRecords); 
			break;

		case ACAMSD_MTAP_Protocol::NoOfResends: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->noOfResends); 
			break;

		case ACAMSD_MTAP_Protocol::ResendTime: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->resendTime); 
			break;

		case ACAMSD_MTAP_Protocol::ReconnTime: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->reconnTime); 
			break;

		case ACAMSD_MTAP_Protocol::FAV: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->FAV); 
			break;
	
		case ACAMSD_MTAP_Protocol::Echo: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->echoValue); 
			break;

		case ACAMSD_MTAP_Protocol::RecordSize: 
			parameterValueList.push_back(ACS_MSD_Service::getAddress(indextoParam)->recordSize); 
			break;

		default:
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Unknown parameter, skipping it");
			break;
		} // End switch
	} // End while

	// If we have found any parameter, the result is always OK.
	if (parameterList.size() > 0) {
		result = ACAMSD_MTAP_Protocol::NoError;
	}

	// Now send the response and return the result.
	int resultCode = 0;
	if(ACAMSD_MTAP_Protocol::sendGetParameterResponse(*pSession, buffer, parameterList, parameterValueList, result)) {
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "GetParameterResponse primitive sent!");
	} else {
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send GetParameterResponse primitive");
	}

	return resultCode;
}

//****************************************************************
// fxHandleUnlink
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxHandleUnlink (
		ACS_DSD_Session * pSession,
		const unsigned char * buffer,
		ACAMSD_MTAP_Protocol::ErrorCode & error) {

	if (error == ACAMSD_MTAP_Protocol::NoError) {
		if (!_myMsgStore->closeDataFile()) {
			int resultCode = 0;
			if(ACAMSD_MTAP_Protocol::sendUnlink_X_Response(*pSession, buffer, ACAMSD_MTAP_Protocol::DiskSpaceExhaust)) {
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Unlink_X_Response primitive sent, returning MsgStoreFull!");
				resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_MsgStoreFull;
			} else {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send Unlink_X_Response primitive");
				resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
			}

			return resultCode;
		}
		_myMsgCollection = 0;
		_myCurrentFileNumber = 0;
	}

	// Now send the response
	int resultCode = 0;
	if(ACAMSD_MTAP_Protocol::sendUnlink_X_Response(*pSession, buffer, error)) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Unlink_X_Response primitive sent!");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send Unlink_X_Response primitive");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}

	return resultCode;
}

//****************************************************************
// fxHandleSynchronize
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxHandleSynchronize (
		ACS_DSD_Session * pSession,
		const unsigned char * buffer,
		unsigned long numberOfOctets) {
	unsigned char fileNumber = 0;
	unsigned long recordNumber = 0;
	std::list<unsigned long> missingList;
	
	ACAMSD_MTAP_Protocol::ErrorCode result = ACAMSD_MTAP_Protocol::unpackSynchronize(buffer, numberOfOctets,
			fileNumber, recordNumber);

	if (result != ACAMSD_MTAP_Protocol::NoError) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to unpack Synchronize primitive, error = %d", result);
		return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}
	
	if (!_myMsgCollection) {
		result = ACAMSD_MTAP_Protocol::BadFileNumber;
		ACS_ACA_LOG(LOG_LEVEL_WARN, "_myMsgCollection is NULL");
	} else {
		result = (_myMsgCollection->getMissingMsg(recordNumber, missingList) == -1)
					 ? ACAMSD_MTAP_Protocol::RecordOutOfRange
					 : ACAMSD_MTAP_Protocol::NoError;

		if (result == ACAMSD_MTAP_Protocol::RecordOutOfRange)
			ACS_ACA_LOG(LOG_LEVEL_WARN, "The record %lu is out of the range", recordNumber);
	}
	
	int resultCode = 0;
	if(ACAMSD_MTAP_Protocol::sendSynchronizeResponse(*pSession, buffer, missingList, result)) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "SynchronizeResponse primitive sent!");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send SynchronizeResponse primitive");
		resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}
	
	return resultCode;
}

//****************************************************************
// fxHandlePutRecord
//****************************************************************
int ACAMSD_MTAP_ChannelManager::fxHandlePutRecord (
		unsigned sessionIndex,
		ACS_DSD_Session * /*pSession*/,
		const unsigned char * buffer,
		unsigned long numberOfOctets,
		ACAMSD_MTAP_Protocol::Primitive primitive) {
	int resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;

	if (numberOfOctets < 8) { // Corrupt message
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Number of octets is different from the expected one!");
		return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}

	ACAMSD_MTAP_Message * msg = new ACAMSD_MTAP_Message(sessionIndex, buffer, numberOfOctets, _currentVersion, primitive);
	if ((_currentVersion > 2)) {
		numberOfOctets -= 2;
	}

	if (numberOfOctets > (static_cast<unsigned long>(ACS_MSD_Service::getAddress(indextoParam)->recordSize)
				+ static_cast<unsigned long>(ACAMS_MTAP_Overhead))) {
		// Message over-sized. Dump it
		 
		if (ackToCP(msg, ACAMSD_MTAP_Protocol::RecordTooLarge)) {
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "RecordTooLarge sent to CP!");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
		} else {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send RecordTooLarge to CP");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
		}

		delete msg; msg = 0;
		return resultCode;
	}

	if (numberOfOctets < 8) { // Corrupt message
		delete msg; msg = 0;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Number of octets is different from the expected one, leaving");
		return ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
	}

	if ((numberOfOctets == 8) && (primitive != ACAMSD_MTAP_Protocol::Skip)) { // Message without body
		if(ackToCP(msg, ACAMSD_MTAP_Protocol::UnspecifiedError)) {
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "UnspecifiedError sent to CP!");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
		} else {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send UnspecifiedError to CP");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
		}

		delete msg; msg = 0;
		return resultCode;
	}

	// If we have no Collection, it is time to create a new data file.
	// ***************************************************************
	ACAMSD_MsgStore::MsgEcode ret = ACAMSD_MsgStore::NoError;

	if (_myMsgCollection == 0) {
		_myCurrentFileNumber = msg->getMtapFileNumber();
		ret = _myMsgStore->createDataFile(_myCurrentFileNumber, _myMsgCollection);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Creating a new data file, file number is '%u'", _myCurrentFileNumber);
	}

	// If not successful operation, from either createDataFile or storeExpRecord
	if (ret != ACAMSD_MsgStore::NoError) {
		if(ackToCP(msg, static_cast<ACAMSD_MTAP_Protocol::ErrorCode>(ret))) {
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "The message store is full, info sent to CP!");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_MsgStoreFull;
		} else {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send message store full info to CP");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
		}

		delete msg; msg = 0;
		return resultCode;
	}

	// Add a normal data record to the collection for storage later
	// ************************************************************
	resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;

	switch (_myMsgCollection->addMsg(msg)) {
	case ACAMSD_MsgCollection::Added: // Msg added to the collection
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Message added to the collection, leaving");
		return resultCode;
		break;

	case ACAMSD_MsgCollection::AlreadyInCollection:
		break;

	case ACAMSD_MsgCollection::AlreadyStored:
		if(ackToCP(msg)) {
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "The record has been already stored, info sent to CP!");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
		} else {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send record already stored info to CP");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
		}
		break;

	case ACAMSD_MsgCollection::OutOfRange:
		if(ackToCP(msg, ACAMSD_MTAP_Protocol::RecordOutOfRange)) {
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "The record is out of the range, info sent to CP!");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_Ok;
		} else {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send record out of range info to CP");
			resultCode = ACAMSD_MTAP_ChannelManager::MTAP_RESULT_ConnectionCorrupt;
		}
		break;
	}

	delete msg; msg = 0;

	return resultCode;
}

//****************************************************************
// fxSelectLowestPrioritySession
//****************************************************************
unsigned ACAMSD_MTAP_ChannelManager::fxSelectLowestPrioritySession () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	time_t lowestTime = 0;
	unsigned lowestIndex = 64;

	for (unsigned i = 0; i < 64; ++i) {
		if (_sessions[i].first && (_sessions[i].second > lowestTime)) {
			lowestTime = _sessions[lowestIndex = i].second;
		}
	}

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "The session with lowest priority has index %u", lowestIndex);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return lowestIndex;
}
