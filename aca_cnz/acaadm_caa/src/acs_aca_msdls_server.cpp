#include "ace/Handle_Set.h"

#include "acs_aca_structured_request.h"
#include "acs_aca_logger.h"
#include "acs_aca_msdls_server.h"

//CONST
const string ACS_ACA_MSDLS_Server::MSDLS_SRV_DOMAIN("ACS");
const string ACS_ACA_MSDLS_Server::MSDLS_SRV_NAME("ACS_MSD_Server");
const ACE_INT32 MAX_NO_OF_HANDLES = 64;


//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
ACS_ACA_MSDLS_Server::ACS_ACA_MSDLS_Server (unsigned /*theStackSize*/)
	: _state(THREAD_STATE_NOT_STARTED), stopMSThreads(), _worker_list_sync(),
	  worker_list(), pmsdlsServer(0), listenH(), noOfHandles(4) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACE_OS::pipe(stopMSThreads);
	for (int i = 0; i < 4; ++i) listenH[i] = ACE_INVALID_HANDLE;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_MSDLS_Server::~ACS_ACA_MSDLS_Server () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_worker_list_sync);
		if (!worker_list.empty()) {
			std::list<ACS_ACA_MSDLS_WorkerThread *>::iterator it;
			for (it = worker_list.begin(); it != worker_list.end(); ++it) {
				delete *it;
			}
			worker_list.clear();
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	delete pmsdlsServer; pmsdlsServer = 0;
	ACE_OS::close(stopMSThreads[0]); ACE_OS::close(stopMSThreads[1]);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACS_ACA_MSDLS_Server::stopMSDLS () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Signaling STOP to MSDLS Server Thread...");
	_state = THREAD_STATE_STOPPED;
	char buf[] = {1 , 1};

	if (ACE_OS::write(stopMSThreads[1], buf, sizeof(buf)) <= 0) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Write on pipe failed, errno = %d", ACE_OS::last_error());
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_worker_list_sync);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Signaling STOP to MSDLS Worker Threads (number_of_threads = %zu)", worker_list.size());
		std::list<ACS_ACA_MSDLS_WorkerThread *>::const_iterator it;

		for (it = worker_list.begin(); it != worker_list.end(); it++) {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Joining an MSDLS Worker Thread...");
			ACE_Thread_Manager::instance()->join((*it)->_tid);
			ACS_ACA_LOG(LOG_LEVEL_INFO, "MSDLS Worker Thread terminated!");
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}


ACE_THR_FUNC_RETURN threadStartupMSDLSworker (void * param) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_MSDLS_WorkerThread * msdlsWorker = reinterpret_cast<ACS_ACA_MSDLS_WorkerThread *>(param);

	msdlsWorker->_state = THREAD_STATE_RUNNING;
	msdlsWorker->workerMain();
	msdlsWorker->_state = THREAD_STATE_STOPPED;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}


//------------------------------------------------------------------------------
//      msdls server work
//------------------------------------------------------------------------------
int ACS_ACA_MSDLS_Server::workerMain () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	// Start MSDLS protocol: DSD open/publish
	while (!startProtocol()) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to start MSDLS protocol");
		
		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 0");
			return 0;
		}

		ACE_OS::sleep(1);

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 0");
			return 0;
		}
	}

	while (_state != THREAD_STATE_STOPPED) {
		if (findConnection()) {	// wait for connections...
			ACS_DSD_Session * session = new (std::nothrow) ACS_DSD_Session();
			if (!session) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate ACS_DSD_Session object!");
				continue;
			}

			int accept_res = 0;
			if ((accept_res = pmsdlsServer->accept(*session)) < 0) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'accept' failed, call_result = %d, error_message: '%s'",
						accept_res, pmsdlsServer->last_error_text());
				delete session; session = 0;
				continue;
			}

			ACS_ACA_MSDLS_WorkerThread * worker = new (std::nothrow) ACS_ACA_MSDLS_WorkerThread(session);
			if (!worker) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate ACS_ACA_MSDLS_WorkerThread object!");
				session->close();
				delete session; session = 0;
				continue;
			}

			const int call_result = ACE_Thread_Manager::instance()->spawn(
					threadStartupMSDLSworker,
					worker,
					THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
					&(worker->_tid),
					0,
					ACE_DEFAULT_THREAD_PRIORITY,
					-1,
					0,
					ACE_DEFAULT_THREAD_STACKSIZE);

			if (call_result == -1) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create a MSDLS Worker Thread, errno = %d", ACE_OS::last_error());
				delete worker; worker = 0;
				continue;
			}

			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_worker_list_sync);
				worker_list.push_back(worker);
			ACS_ACA_CRITICAL_SECTION_GUARD_END();
		}

		//check workers in stopped state and delete them
		garbageCollector();
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

void ACS_ACA_MSDLS_Server::garbageCollector () {
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_worker_list_sync);
		if (worker_list.empty())	return;

		// Save the current list
		std::list<ACS_ACA_MSDLS_WorkerThread *> worker_list_save(worker_list);
		worker_list.clear();

		std::list<ACS_ACA_MSDLS_WorkerThread *>::iterator it;
		ACS_ACA_MSDLS_WorkerThread * worker = 0;

		for (it = worker_list_save.begin(); it != worker_list_save.end(); ++it) {
			if ((worker = (*it))) {	// Worker Thread object found
				if (worker->_state != THREAD_STATE_STOPPED) // Thread active, save the object
					worker_list.push_back(worker);
				else {	//Thread not active, join and delete it
					ACE_Thread_Manager::instance()->join(worker->_tid);
					delete worker;
				}
			}
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();
}

//------------------------------------------------------------------------------
//      Publish the MSDLS Pipe Server
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_Server::startProtocol () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	pmsdlsServer = new (std::nothrow) ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET);
	
	if (!pmsdlsServer) { // ERROR: DSD server object not allocated
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for the pmsdlsServer object, returning false");
		return false;
	}

	int res = pmsdlsServer->open();
	if (res < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'open' failed, call_result = %d, returning false", res);
		delete pmsdlsServer; pmsdlsServer = 0;
		return false;
	}

	int status = pmsdlsServer->publish(MSDLS_SRV_NAME.c_str(), MSDLS_SRV_DOMAIN.c_str());
	if ( status < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'publish' failed, returning false");
		delete pmsdlsServer; pmsdlsServer = 0;
		return false;
	} else {
		//get listen handles
		status = pmsdlsServer->get_handles(listenH, noOfHandles);

		if ((status < 0) || (noOfHandles <= 0)) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'get_handles' failed, returning false");
			pmsdlsServer->unregister();
			delete pmsdlsServer; pmsdlsServer = 0;
			return false;
		} else {
			ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
			return true;
		}
	}
}

//------------------------------------------------------------------------------
//      Wait for a new connection for 500 ms
//		Return true if it founds a new session
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_Server::findConnection () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	bool return_code = false;
	struct pollfd pollfds[noOfHandles + 1];

	for (int i = 0; i < noOfHandles; i++) {
		pollfds[i].fd = listenH[i];
		pollfds[i].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
		pollfds[i].revents = 0;
	}

	pollfds[noOfHandles].fd = stopMSThreads[0];
	pollfds[noOfHandles].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
	pollfds[noOfHandles].revents = 0;

	ACE_Time_Value timeout(10, 0);
	errno = 0;
	const int poll_result = ACE_OS::poll(pollfds, (noOfHandles + 1), &timeout);
	int errno_save = errno;

	if (poll_result < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'poll' failed, errno = %d", errno_save);
	}
	else if (poll_result == 0) {
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Timeout expired, errno = %d", errno_save);
	}
	else {
		if (pollfds[noOfHandles].revents & (POLLIN | POLLPRI)) { // Stop event signaled
			ACS_ACA_LOG(LOG_LEVEL_WARN, "The stop event has been signaled!");
		}

		for (int i = 0; i < noOfHandles; i++) {
			if (pollfds[i].revents & (POLLIN | POLLPRI)) { // Handle signaled
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "The handle having index %d has been signaled, returning true", i);
				return_code = true;
				break;
			}
			else if (pollfds[i].revents & (POLLRDHUP | POLLHUP | POLLERR)) { // Error signaled on the handle
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error on the handle having index %d, revents = %d!", i, pollfds[i].revents);
			}
		}
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return return_code;
}
