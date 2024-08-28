//****************************************************************
//
// NAME
//   ACAMSD_MSIP_ChannelManager.cpp

// COPYRIGHT
//   COPYRIGHT Ericsson Lab Pagani, Italy 2006.
//   All rights reserved.
//
//   The Copyright to the computer program(s) herein 
//   is the property of Ericsson Lab Pagani, Italy.
//   The program(s) may be used and/or copied only with 
//   the written permission from Ericsson Lab Pagani or in 
//   accordance with the terms and conditions stipulated in the 
//   agreement/contract under which the program(s) have been 
//   supplied.

//  DOCUMENT NO
//    CAA 137 ----

//  AUTHOR 
//    G/P/AP SA/ERI  Nicola Muto

//  DESCRIPTION
//    ....................

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO          DATE          NAME       DESCRIPTION
//    A               YYYY-MM-DD    qnicmut    product release
//****************************************************************

#include <sys/eventfd.h>
#include <time.h>

#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Handle_Set.h>

//TODO: To remove this line
//#define ACS_ACA_DEBUGGING_LOG_ENABLED

#include "acs_aca_ms_msip_protocol.h"
#include "acs_aca_msd_ms_manager.h"
#include "acs_aca_msd_service.h"
#include "acs_aca_defs.h"
#include "acs_aca_macros.h"
#include "acs_aca_logger.h"


namespace {
	const char * const g_class_name = "ACAMSD_MSIP_ChannelManager";
}

//****************************************************************
// Constructors and initializer
//****************************************************************
ACAMSD_MSIP_ChannelManager::ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, const short index)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(index), _cpSystemId(SINGLE_CP_ID),
	  _isMultipleCPSystem(false), _msParameters(0),	_incomingSessionQueue(),_lastActivityTime(0), _sessions(), _sessionCount(0),
		_sessionInactivityTimeout(SESSION_INACTIVITY_DEFAULT_TIMEOUT), _myMsManager(myMsManager), _syncAccess(), _stop_event_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	fxInitialize();
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

ACAMSD_MSIP_ChannelManager::ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, const ACAMS_Parameters * msParameters,	int sessionInactivityTimeout)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(0), _cpSystemId(SINGLE_CP_ID),
	  _isMultipleCPSystem(false), _msParameters(msParameters),_incomingSessionQueue(), _lastActivityTime(0), _sessions(),
	  _sessionCount(0), _sessionInactivityTimeout(sessionInactivityTimeout), _myMsManager(myMsManager), _syncAccess(),
	  _stop_event_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	fxInitialize();
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

ACAMSD_MSIP_ChannelManager::ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId, bool isMultipleCPSystem, short index)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(index), _cpSystemId(cpSystemId),
	  _isMultipleCPSystem(isMultipleCPSystem), _msParameters(0), _incomingSessionQueue(), _lastActivityTime(0), _sessions(),
	  _sessionCount(0),	_sessionInactivityTimeout(SESSION_INACTIVITY_DEFAULT_TIMEOUT), _myMsManager(myMsManager), _syncAccess(),
	  _stop_event_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	fxInitialize();
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

ACAMSD_MSIP_ChannelManager::ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId, bool isMultipleCPSystem, const ACAMS_Parameters * msParameters,int sessionInactivityTimeout)
	: _tid(0), _state(THREAD_STATE_NOT_STARTED), _stoppedState(false), indextoParam(0), _cpSystemId(cpSystemId),
	  _isMultipleCPSystem(isMultipleCPSystem), _msParameters(msParameters), _incomingSessionQueue(), _lastActivityTime(0),
	  _sessions(), _sessionCount(0), _sessionInactivityTimeout(sessionInactivityTimeout), _myMsManager(myMsManager),
	  _syncAccess(), _stop_event_fd()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering, indextoParam = %d", indextoParam);
	fxInitialize();
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving, indextoParam = %d", indextoParam);
}

void ACAMSD_MSIP_ChannelManager::fxInitialize () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Index = %d, MS = '%s', CP = %d",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);

	for (int i = 0; i < MAX_WAIT_OBJECTS; ++i) {
		_sessions[i]._socket = ACE_INVALID_HANDLE;
		_sessions[i]._event = 0;
		_sessions[i]._time = 0; //static_cast<time_t>(0);//__time64_t>(0);
	}

	_stop_event_fd = ::eventfd(0, 0);
	if ( _stop_event_fd == ACE_INVALID_HANDLE ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create 'eventfd' object - errno = %d", errno);
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// Destructor and finalizer
//****************************************************************
ACAMSD_MSIP_ChannelManager::~ACAMSD_MSIP_ChannelManager () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', CP = %d, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	fxFinalize();
	::close(_stop_event_fd);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACAMSD_MSIP_ChannelManager::fxFinalize () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	fxDeleteActiveSessions();
	fxDeleteIncomingSessions();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// addSession
//****************************************************************
int ACAMSD_MSIP_ChannelManager::addSession (ACE_SOCKET clientSocket, ACE_Event * clientSockEvent) {
	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Stop requested by someone: _state == %d, _sessionCount == %i", _state, _sessionCount);

		try {
			bool sendOk = ACAMS_MSIP_Protocol::sendConnectionRefused(clientSocket, "MSIP Channel Manager is stopped!");
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Call 'sendConnectionRefused' returned %s", (sendOk) ? "TRUE" : "FALSE");
		}
		catch (...) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'sendConnectionRefused' threw an exception!");
		}

		ACS_ACA_LOG(LOG_LEVEL_WARN, "Returning STOP!");
		return RETURN_STOP;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		int totalSessionCount = _sessionCount + static_cast<int>(_incomingSessionQueue.size());

		if ((totalSessionCount >= MAX_SESSION_NUMBER) || (totalSessionCount >= MAX_WAIT_OBJECTS)) {
			char primitive_message[512] = {0};
			ACE_OS::snprintf(primitive_message, sizeof(primitive_message) - 1, "Max number of connection reached: %d connections "
					"already activated", totalSessionCount);

			try {
				ACAMS_MSIP_Protocol::sendConnectionRefused(clientSocket, primitive_message);
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Primitive 'ConnectionRefused' sent! - primitive_message: '%s'", primitive_message);
			}
			catch (...) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'sendConnectionRefused' threw an exception!");
			}

			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Returning TOO_MANY_CONNECTIONS!");
			return RETURN_TOO_MANY_CONNECTIONS;
		}

		//Notify the client
		if (!ACAMS_MSIP_Protocol::sendConnectionAccepted(clientSocket)) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to sent 'ConnectionAccepted' primitive, returning 5!");
			return 5;
		}

		//Add the incoming session to the internal queue.
		_incomingSessionQueue.push(MSIPSession(clientSocket, clientSockEvent, time(0)));
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return RETURN_OK;
}

int ACAMSD_MSIP_ChannelManager::notifyMessageAvailableToClients () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning...");
		return 1;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		MSIPSession session = _sessions[0];
		if (session._socket == ACE_INVALID_HANDLE) {
			return 0;
		}

		if (!ACAMS_MSIP_Protocol::sendMsgAvailable(session._socket)) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendMsgAvailable' failed! deleting the session at index 0");
			fxDeleteSession(0);
			return 2;
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MSIP_ChannelManager::threadMain (void * /*parameters*/) {
	return 0;
}

int ACAMSD_MSIP_ChannelManager::stopMSIP () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Stopping this MSIP Channel Manager - index = %d, MS = %s, CP = %u, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	_state = THREAD_STATE_STOPPED;

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Signaling stop event! - index = %d, MS = %s, CP = %u, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, _tid);

	eventfd_t stop_event = 1U;
	if (::eventfd_write(_stop_event_fd, stop_event))
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to signal the stop event! - errno = %d", errno);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MSIP_ChannelManager::workerMain () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "MSIP Channel Manager Thread, starting to work - index = %d, MS = '%s', CP = %d, TID = %ld",
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
			if ( _sessionCount ) {
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
		fxDeleteActiveSessions();
		fxDeleteIncomingSessions();
		stopMSIP(); //stop myself
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return exitCode;
}

//****************************************************************
// fxActivateIncomingSessions
//****************************************************************
int ACAMSD_MSIP_ChannelManager::fxActivateIncomingSessions () {
	int freePos = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		while (!_incomingSessionQueue.empty()) {
			//Find the first free position
			while ((freePos < MAX_WAIT_OBJECTS) && (_sessions[freePos]._socket != ACE_INVALID_HANDLE)) freePos++;

			if (freePos >= MAX_WAIT_OBJECTS) {
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
int ACAMSD_MSIP_ChannelManager::fxValidateSessions () {
		for (unsigned i = 0; i < 64; ++i) {
		time_t now = ::time(0);
		if ((_sessions[i]._socket != ACE_INVALID_HANDLE) && (_sessionInactivityTimeout < (now - _sessions[i]._time))) {
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Deleting session at index %d", i);
			fxDeleteSession(i); //Destroy this session because was inactive for more than _sessionInactivityTimeout seconds.
		}
	}
	return RETURN_OK;
}

//****************************************************************
// fxDoWork
//****************************************************************
int ACAMSD_MSIP_ChannelManager::fxDoWork () {
	if (_sessionCount <= 0) {
		int num_of_events = 1;
		ACE_Time_Value poll_timeout(0, 500000);	//500 milliseconds
		struct pollfd pollfds[num_of_events];
		pollfds[0].fd = _stop_event_fd;
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
			if ( pollfds[0].revents & (POLLIN | POLLPRI) ) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "The stop event for the couple (%s,%d) has been signaled, returning 1",
						ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
				return 1;
			}
			else {	// Error on stop event fd
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error signaled on stop event fd (revents = %d) for the couple(%s,%d), returning -5",
						pollfds[0].revents, ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
				return -5;
			}
		}

		return 0;
	}

	ACE_INT32 eventCount = 0;
	ACE_HANDLE events[64];
	unsigned reverseIndex[64];

	//Initialize the handle array for the WaitForMultipleObjects call
	for (unsigned i = 0; i < 64; ++i)
		if (_sessions[i]._socket != ACE_INVALID_HANDLE) {
			events[eventCount] = _sessions[i]._socket;
			reverseIndex[eventCount++] = i;
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
	for (ACE_INT32 i = 0; i < (eventCount -1); i++) {
		myPollfds[i].fd = events[i];
		myPollfds[i].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
		myPollfds[i].revents = 0;
	}

	myPollfds[eventCount - 1].fd = _stop_event_fd;
	myPollfds[eventCount - 1].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
	myPollfds[eventCount - 1].revents = 0;

	ACE_Time_Value selectTime;
	selectTime.set(0, 100000);
	int errorLevel = 0;

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Calling poll (MSIP) for (%s,%d) with %d events",
			ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId, eventCount);

	errno = 0;
	ACE_INT32 waitResult = ACE_OS::poll(myPollfds, eventCount, &selectTime);
	int errno_save = errno;

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	switch (waitResult) {
	case -1: //WAIT_FAILED:
		ACS_ACA_LOG(LOG_LEVEL_WARN, "POLL WAIT FAILED: errno_save = %d", errno_save);
		errorLevel = (errno_save == EINTR ? 5 : -5);
		break;

	case 0: 
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Expired timeout - index = %d, MS = %s",
				indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);
		break;

	default: {
			int index = 0;

			if (waitResult >= 1) {
				bool validEvent = false;

				if ( myPollfds[eventCount -1].revents & (POLLIN | POLLPRI) ) { // The stop event has been signaled
					ACS_ACA_LOG(LOG_LEVEL_WARN, "The stop event for the couple (%s,%d) has been signaled, returning 1",
							ACS_MSD_Service::getAddress(indextoParam)->name, _cpSystemId);
					return 1;
				}

				for (index = 0; index < (eventCount -1); index++) {
					if (myPollfds[index].revents & (POLLIN | POLLPRI)) {
						validEvent = true;
						break;
					}
					else {
						if (myPollfds[index].revents & (POLLRDHUP | POLLHUP | POLLERR | POLLNVAL)) {
							ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Error on the session at index %d: revents is %d, deleting it",
									index, myPollfds[index].revents);
							errorLevel = 5;
							fxDeleteSession(reverseIndex[index]);
						}
					}
				}

				if (validEvent) {
					errorLevel = fxExchange(reverseIndex[index]);
					_lastActivityTime = time(0);
				}
			} else {
				errorLevel = 5;
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERTION[waitResult < 1]: deleting session at index %d", index);
				fxDeleteSession(reverseIndex[index]);
			}
		}
		break;
	}

	return errorLevel;
}

//****************************************************************
// fxDeleteSession
//****************************************************************
void ACAMSD_MSIP_ChannelManager::fxDeleteSession (unsigned index) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		if ((index >= 64) || (_sessions[index]._socket == ACE_INVALID_HANDLE)) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERTION[(index >= 64) || (_sessions[index]._socket == ACE_INVALID_HANDLE) - "
					"index == %u, _sessionCount == %u",	index, _sessionCount);
			return;
		}

		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Deleting session at index %u", index);
		MSIPSession session = _sessions[index];

		::shutdown(session._socket, 2);
		if ((session._event != (ACE_Event*)NULL)) {
			delete session._event;
			session._event = 0;
		}

		_sessions[index]._event = 0;
		ACE_OS::closesocket(session._socket);
		_sessions[index]._socket = ACE_INVALID_HANDLE;
		_sessions[index]._time = 0;

		_sessionCount--;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// fxExchange
//****************************************************************
int ACAMSD_MSIP_ChannelManager::fxExchange (unsigned sessionIndex) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Exchanging using session at index %u", sessionIndex);

	static const unsigned BUFFER_SIZE = 64 * 1024 + 8; //64Kb + 8b

	if (sessionIndex >= 64) { //Index out of range
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Session index out of range");
		return 3;
	}

	unsigned char buffer[BUFFER_SIZE];
	ACAMS_MSIP_Protocol::Primitive primitive;
	unsigned int primitiveSize = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		ACE_SOCKET socket = _sessions[sessionIndex]._socket;

		if (socket == ACE_INVALID_HANDLE) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "The socket is invalid!");
			return 3;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		primitive = ACAMS_MSIP_Protocol::getPrimitive(socket, primitiveSize, buffer);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	if (primitiveSize <= 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERTION[primitiveSize <= 0]: Deleting session at index %i: "
				"_sessionCount == %i, primitiveSize == %u", sessionIndex, _sessionCount, primitiveSize);
		fxDeleteSession(sessionIndex);
		return 0;
	}

	ACAMS_MSIP_Protocol::ErrorCode errCode = ACAMS_MSIP_Protocol::NoError;

	switch (primitive) {
	case ACAMS_MSIP_Protocol::None: { //ERROR or lost connection
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERTION[primitive == ACAMS_MSIP_Protocol::None]: Deleting session at index %i:"
					" _sessionCount == %i", sessionIndex, _sessionCount);
			fxDeleteSession(sessionIndex);
		}
		break;

	case ACAMS_MSIP_Protocol::DeleteFile: {
			std::string fileName;
			errCode = ACAMS_MSIP_Protocol::unpackDeleteFile(buffer, primitiveSize, fileName);

			if (errCode != ACAMS_MSIP_Protocol::NoError) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERTION[primitive == ACAMS_MSIP_Protocol::DeleteFile && "
						"errCode != ACAMS_MSIP_Protocol::NoError]: Deleting session at index %i: "
						"_sessionCount == %i, errCode == %d", sessionIndex, _sessionCount, errCode);
				fxDeleteSession(sessionIndex);
				return 2;
			}

			if (_myMsManager) {
				try {
					ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Trying to delete the file '%s'", fileName.c_str());
					_myMsManager->deleteDataFile(_cpSystemId, fileName);
				}
				catch (...) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Exception threw during 'deleteDataFile'!");
					return 4;
				}
			}
			else {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "My Ms Manager pointer is NULL!");
			}
		}
		break;

	case ACAMS_MSIP_Protocol::RequestMsg: {
			ULONGLONG msgNumber = 0;
			errCode = ACAMS_MSIP_Protocol::unpackRequestMsg(buffer, primitiveSize, msgNumber);

			if (errCode != ACAMS_MSIP_Protocol::NoError) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'unpackRequestMsg' failed, deleting session at index %u", sessionIndex);
				fxDeleteSession(sessionIndex);
				return 2;
			}

			ULONGLONG nextMsgNumber = 0;

			ACS_ACA_LOG(LOG_LEVEL_TRACE, "Calling 'getNextMsgNumber'...");
			if (_myMsManager->getNextMsgNumber(_cpSystemId, nextMsgNumber)) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getNextMsgNumber' failed!");
			}

			if (msgNumber < nextMsgNumber) { //Message already available
				bool call_result = false;

				ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
					ACE_SOCKET socket = _sessions[sessionIndex]._socket;

					if (socket == ACE_INVALID_HANDLE) {
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "The socket is invalid!");
						return 3;
					}

					ACS_ACA_LOG(LOG_LEVEL_TRACE, "Sending primitive 'MsgAvailable'...");
					call_result = ACAMS_MSIP_Protocol::sendMsgAvailable(socket);
				ACS_ACA_CRITICAL_SECTION_GUARD_END();

				if (!call_result) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to sent 'MsgAvailable' primitive, deleting the session at index %u", sessionIndex);
					fxDeleteSession(sessionIndex);
					return 2;
				}
			}
		}
		break;

	case ACAMS_MSIP_Protocol::ApplDisc:
		ACS_ACA_LOG(LOG_LEVEL_WARN, "'ApplDisc' primitive received, destroing the session at index %u", sessionIndex);
		fxDeleteSession(sessionIndex);
		break;

	default:
		break;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		if (_sessions[sessionIndex]._socket != ACE_INVALID_HANDLE) {
			_sessions[sessionIndex]._time = time(0);
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	return 0;
}

//****************************************************************
// fxDeleteActiveSessions
//****************************************************************
int ACAMSD_MSIP_ChannelManager::fxDeleteActiveSessions () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	MSIPSession session;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		for (int i = 0; i < MAX_WAIT_OBJECTS; ++i) {
			session = _sessions[i];

			if (session._socket != ACE_INVALID_HANDLE) {
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Deleting session at index %d, _sessionCount = %d", i, _sessionCount);
				::shutdown(session._socket, 2);
			}
			
			if (session._event != (ACE_Event*)NULL) {
				//CHANGE: 21st August 2013
				delete session._event;

				session._event=0;
				_sessions[i]._event = 0;
			}

			if (session._socket != ACE_INVALID_HANDLE) {
				ACE_OS::closesocket(session._socket);
			}

			_sessions[i]._socket = ACE_INVALID_HANDLE;
			_sessions[i]._time = 0;
		}

		_sessionCount = 0;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// fxDeleteIncomingSessions
//****************************************************************
int ACAMSD_MSIP_ChannelManager::fxDeleteIncomingSessions () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	MSIPSession session;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		while (!_incomingSessionQueue.empty()) {
			session = _incomingSessionQueue.front();

			if (session._socket != ACE_INVALID_HANDLE) {
				::shutdown(session._socket,2);
			}

			if ((session._event != (ACE_Event*)NULL)) {
				delete session._event;
				session._event = 0;
			}

			if (session._socket != ACE_INVALID_HANDLE) {
				ACE_OS::closesocket(session._socket);
			}

			//CHANGE_HR65432 and HR71119: 10th September 2013
			_incomingSessionQueue.front()._event = 0;

			_incomingSessionQueue.pop();
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}
