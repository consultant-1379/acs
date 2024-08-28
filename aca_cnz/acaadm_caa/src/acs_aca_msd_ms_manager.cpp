//****************************************************************
//
// NAME
//   ACAMSD_MS_Manager.cpp

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
//    XLANSRI   Sridhar Lanka

//  DESCRIPTION
//    ....................

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO          DATE          NAME       DESCRIPTION
//    A               2012-10-03    xlansri    product release
//****************************************************************

#include <sys/eventfd.h>
#include <mqueue.h>

#include <new>
#include <stdexcept>

// XCASALE: Uncomment this line only for debugging purpose
//#define ACS_ACA_DEBUGGING_LOG_ENABLED

#include "acs_aca_macros.h"
#include "acs_aca_msd_service.h"
#include "acs_aca_system_exception.h"
#include "acs_aca_message_store_interface.h"
#include "acs_aca_logger.h"
#include "acs_aca_cptable_observer.h"
#include "acs_aca_constants.h"
#include "acs_aca_msd_ms_manager.h"


namespace {
	const char * const g_class_name = "ACAMSD_MS_Manager";

#if defined(ACS_ACA_DEBUGGING_LOG_ENABLED)
	unsigned _cpSystemId = ~0;
#endif
}

//TODO check use
using Ericsson::System::Exception;
using Ericsson::System::SystemException;


//****************************************************************
// Constructors and initializer
//****************************************************************
ACAMSD_MS_Manager::ACAMSD_MS_Manager (const ACAMS_Parameters * msParameters, const char * domainName)
: _tid(0), _state(THREAD_STATE_NOT_STARTED), indextoParam(0),_isMSThreadAlive(false),	_msParameters(msParameters), thedsdServer(0),
  _domainName(domainName), _dsdEvents(), _dsdNumOfEvents(4), _acceptMTAPConnNewRetryNum(0),
  _initDSDSessionAcceptRetryNum(0), _mtapChannelMap(), _msipChannelMap(), _cpIdList(),
  _isMultipleCPSystem(false), _msipPort(0),	_msipListenSocket(INVALID_SOCKET), _msipService(),
  _msipListenEvent(0), _mtapHasToBeRestarted(false), _msipHasToBeRestarted(false), _syncAccess(), _MTAP_syncAccess(), _MSIP_syncAccess(),
  _mtap_msip_shared_sync(), _stop_event_fd() {

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	_msipService.sin_family = AF_INET;
	_msipService.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	_msipService.sin_port = ::htons(6000);

	for (int i = 0; i < 4; ++i) _dsdEvents[i] = ACE_INVALID_HANDLE;

	fxInitialize();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

ACAMSD_MS_Manager::ACAMSD_MS_Manager (const short index, const char * domainName)
: _tid(0), _state(THREAD_STATE_NOT_STARTED), indextoParam(index),_isMSThreadAlive(false),
  _msParameters(0), thedsdServer(0), _domainName(domainName), _dsdEvents(), _dsdNumOfEvents(4),
  _acceptMTAPConnNewRetryNum(0), _initDSDSessionAcceptRetryNum(0), _mtapChannelMap(), _msipChannelMap(),
  _cpIdList(), _isMultipleCPSystem(false), _msipPort(0), _msipListenSocket(INVALID_SOCKET), _msipService(),
  _msipListenEvent(0), _mtapHasToBeRestarted(false), _msipHasToBeRestarted(false), _syncAccess(), _MTAP_syncAccess(), _MSIP_syncAccess(),
  _mtap_msip_shared_sync(), _stop_event_fd() {

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	_msipService.sin_family = AF_INET;
	_msipService.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	_msipService.sin_port = ::htons(6000);

	for (int i = 0; i < 4; ++i) _dsdEvents[i] = ACE_INVALID_HANDLE;

	fxInitialize();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

ACAMSD_MS_Manager::ACAMSD_MS_Manager (const ACAMS_Parameters * msParameters, std::vector<unsigned> cpIdList, const char * domainName)
: _tid(0), _state(THREAD_STATE_NOT_STARTED), indextoParam(0),_isMSThreadAlive(false),
  _msParameters(msParameters), thedsdServer(0), _domainName(domainName), _dsdEvents(), _dsdNumOfEvents(4),
  _acceptMTAPConnNewRetryNum(0), _initDSDSessionAcceptRetryNum(0), _mtapChannelMap(), _msipChannelMap(),
  _cpIdList(cpIdList), _isMultipleCPSystem(true), _msipPort(0), _msipListenSocket(INVALID_SOCKET), _msipService(),
  _msipListenEvent(0), _mtapHasToBeRestarted(false), _msipHasToBeRestarted(false), _syncAccess(), _MTAP_syncAccess(), _MSIP_syncAccess(),
  _mtap_msip_shared_sync(), _stop_event_fd() {

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	_msipService.sin_family = AF_INET;
	_msipService.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	_msipService.sin_port = ::htons(6000);

	for (int i = 0; i < 4; ++i) _dsdEvents[i] = ACE_INVALID_HANDLE;

	fxInitialize();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

ACAMSD_MS_Manager::ACAMSD_MS_Manager (std::vector<unsigned> cpIdList, short index, const char * domainName)
: _tid(0), _state(THREAD_STATE_NOT_STARTED), indextoParam(index),_isMSThreadAlive(false),
  _msParameters(0), thedsdServer(0), _domainName(domainName), _dsdEvents(), _dsdNumOfEvents(4),
  _acceptMTAPConnNewRetryNum(0), _initDSDSessionAcceptRetryNum(0), _mtapChannelMap(), _msipChannelMap(),
  _cpIdList(cpIdList), _isMultipleCPSystem(true), _msipPort(0), _msipListenSocket(INVALID_SOCKET), _msipService(),
  _msipListenEvent(0), _mtapHasToBeRestarted(false), _msipHasToBeRestarted(false), _syncAccess(), _MTAP_syncAccess(), _MSIP_syncAccess(),
  _mtap_msip_shared_sync(), _stop_event_fd() {

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	_msipService.sin_family = AF_INET;
	_msipService.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	_msipService.sin_port = ::htons(6000);

	for (int i = 0; i < 4; ++i) _dsdEvents[i] = ACE_INVALID_HANDLE;

	fxInitialize();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACAMSD_MS_Manager::fxInitialize () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s'",	indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);

	_stop_event_fd = ::eventfd(0, 0);
	if ( _stop_event_fd == ACE_INVALID_HANDLE )
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create 'eventfd' object - errno = %d", errno);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}
		
//****************************************************************
// Destructor and fxFinalilze method
//****************************************************************
ACAMSD_MS_Manager::~ACAMSD_MS_Manager () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', TID = %ld",	indextoParam,
			ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		fxFinalize();
		_cpIdList.clear();
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	::close(_stop_event_fd);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACAMSD_MS_Manager::fxFinalize () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	try {
		fxStopMTAPChannels();
		fxDeleteMTAPChannels();

		if (thedsdServer) {
			if (!thedsdServer->unregister()) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'unregister' failed! Error text is: %s", thedsdServer->last_error_text());
			}
			thedsdServer->close();
			delete thedsdServer; thedsdServer = 0;
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Dsd Server successfully closed!");
		}

		fxStopMSIPChannels();
		fxDeleteMSIPChannels();
		fxStopMSIP();

		if (_isMSThreadAlive) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "MS thread is still alive, index = %d, MS = %s, TID = %ld",
					indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);
		}
	} catch(...) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Exception occurred while stopping/deleting MTAP/MSIP channels, "
				"index = %d, MS = %s, TID = %ld", indextoParam,
				ACS_MSD_Service::getAddress(indextoParam)->name, _tid);
	}
	
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}


//****************************************************************
// Get the CP list associated to message store name
//****************************************************************
std::list<unsigned short> ACAMSD_MS_Manager::getCPList () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	std::list<unsigned short> cplist;
	
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);

		for (mtapChannelMap_t::iterator it = _mtapChannelMap.begin(); it != _mtapChannelMap.end(); cplist.push_back((it++)->first)) ;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return cplist;
}

const ACAMSD_MTAP_ChannelManager * ACAMSD_MS_Manager::getMTAPChannelMgrByCP (unsigned int cpid_) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_MTAP_syncAccess);
	mtapChannelMap_t::iterator it = _mtapChannelMap.find(cpid_);

	return (it == _mtapChannelMap.end() ? 0 : it->second);
}

//****************************************************************
// messageAvailableFromMTAP
//****************************************************************
int ACAMSD_MS_Manager::messageAvailableFromMTAP (unsigned cpSystemId) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACAMSD_MSIP_ChannelManager * msipChannel = 0;

	int errorLevel = fxGetMSIPChannelManager(cpSystemId, msipChannel);
	if (errorLevel) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxGetMSIPChannelManager' failed, return code is %d", errorLevel);
		return errorLevel;
	}

	if (!msipChannel) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The ACAMSD_MSIP_ChannelManager pointer is NULL, returning 4!");
		return 4;
	}

	//Notify my MSIP Channel manager that at least one message is available.
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_mtap_msip_shared_sync);
		errorLevel = msipChannel->notifyMessageAvailableToClients();
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	if (errorLevel) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'notifyMessageAvailableToClients' failed, return code is %d", errorLevel);
		return errorLevel;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

const char * ACAMSD_MS_Manager::msName() {
	ACAMS_Parameters * p = ACS_MSD_Service::getAddress(indextoParam);

	return (p ? p->name : 0);
}

const ACAMS_Parameters * ACAMSD_MS_Manager::messageStoreParameters () const {
	return ACS_MSD_Service::getAddress(indextoParam);
}

//****************************************************************
// deleteDataFile
//****************************************************************
int ACAMSD_MS_Manager::deleteDataFile (unsigned cpSystemId, std::string fileName) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Trying to delete the file '%s'", fileName.c_str());

	ACAMSD_MTAP_ChannelManager * mtapChannel = 0;
	int errorLevel = 0;

	if ((errorLevel = fxGetMTAPChannelManager(cpSystemId, mtapChannel))) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxGetMTAPChannelManager' failed, return code is %d", errorLevel);
		return errorLevel;
	}

	if (!mtapChannel) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The ACAMSD_MTAP_ChannelManager pointer is NULL, returning 4!");
		return 4;
	}

	//Delegate the delete operation to MTAP Channel Manager
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_mtap_msip_shared_sync);
		if ((errorLevel = mtapChannel->deleteDataFile(fileName))) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'deleteDataFile' failed, return code is %d", errorLevel);
			return errorLevel;
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// getNextMsgNumber
//****************************************************************
int ACAMSD_MS_Manager::getNextMsgNumber (unsigned cpSystemId, ULONGLONG & msgNumber) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACAMSD_MTAP_ChannelManager * mtapChannel = 0;
	int errorLevel = 0;

	if ((errorLevel = fxGetMTAPChannelManager(cpSystemId, mtapChannel))) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxGetMTAPChannelManager' failed, return code is %d", errorLevel);
		return errorLevel;
	}

	if (!mtapChannel) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The ACAMSD_MTAP_ChannelManager pointer is NULL, returning 4!");
		return 4;
	}

	//Delegate the get operation to MTAP Channel Manager
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_mtap_msip_shared_sync);
		if ((errorLevel = mtapChannel->getNextMsgNumber(msgNumber))) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getNextMsgNumber' failed, return code is %d", errorLevel);
			return errorLevel;
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::stopMS() {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Stopping this MS Manager - index = %d, MS = %s, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	_state = THREAD_STATE_STOPPED;

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Signaling stop event! - index = %d, MS = %s, TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	eventfd_t stop_event = 1U;
	if (::eventfd_write(_stop_event_fd, stop_event))
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to signal the stop event! - errno = %d", errno);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::workerMain () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "MS Manager Thread, starting to work - index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	int errorLevel = 0;
	_isMSThreadAlive = true;

	//Calculate the MSIP listening port
	_msipPort = _isMultipleCPSystem
		? ACAMS_MSIP_Protocol::getMSIPportNo(ACS_MSD_Service::getAddress(indextoParam)->name) //Multiple CP System
		: ACAMS_MSIP_Protocol::getMSIPportNo(ACS_MSD_Service::getAddress(indextoParam)->site, ACS_MSD_Service::getAddress(indextoParam)->name); //Single Cp System


	for (bool again = true; again; ) {
		if (_state == THREAD_STATE_STOPPED) break;

		if (ACS_MSD_Service::getAddress(indextoParam)->nameChanged == true) {
			const int old_msip_port = _msipPort;
			_msipPort = _isMultipleCPSystem
				? ACAMS_MSIP_Protocol::getMSIPportNo(ACS_MSD_Service::getAddress(indextoParam)->name) //Multiple CP System
				: ACAMS_MSIP_Protocol::getMSIPportNo(ACS_MSD_Service::getAddress(indextoParam)->site, ACS_MSD_Service::getAddress(indextoParam)->name); //Single Cp System

			ACS_ACA_LOG(LOG_LEVEL_INFO, "The MS name was changed: "
					"the value of the MSIP port has been changed from '%d' to '%d'!", old_msip_port, _msipPort);

			ACS_MSD_Service::reset_name_changed_attribute(indextoParam);
		}

		if (_state == THREAD_STATE_STOPPED) break;

		errorLevel = fxStartMSIP();
		if (errorLevel) continue;

		if (_state == THREAD_STATE_STOPPED) break;

		errorLevel = fxStartMTAP();
		if (errorLevel) continue;

		if (_state == THREAD_STATE_RUNNING) {
			errorLevel = fxWorking();

			if (ACS_MSD_Service::getAddress(indextoParam)->nameChanged) {
				ACS_ACA_LOG(LOG_LEVEL_INFO, "Message store name for index %d was changed, restarting MTAP and MSIP!", indextoParam);
				_mtapHasToBeRestarted = _msipHasToBeRestarted = true;
			}
	
			again = (errorLevel >= 0);
		}

		if (_state == THREAD_STATE_STOPPED) break;

		if (_mtapHasToBeRestarted) {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Required to restart MTAP, index = %d, MS = %s",
					indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);

			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				fxStopMTAPChannels();
				fxDeleteMTAPChannels();
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			fxStopMTAP();
		}

		if (_msipHasToBeRestarted) {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Required to restart MSIP, index = %d, MS = %s",
					indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);

			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				fxStopMSIPChannels();
				fxDeleteMSIPChannels();
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			fxStopMSIP();
		}

		_mtapHasToBeRestarted = _msipHasToBeRestarted = false;
	}

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Stopping and deleting MTAP and MSIP channels (_state is %d), index = %d, MS = %s",
			_state, indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
		fxStopMTAPChannels();
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
		fxStopMSIPChannels();
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
		fxDeleteMTAPChannels();
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	fxStopMTAP();

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
		fxDeleteMSIPChannels();
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	fxStopMSIP();

	_isMSThreadAlive = false;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

//******************************************************************************
//                           Private help-functions
//******************************************************************************
int ACAMSD_MS_Manager::fxStartMTAP (unsigned long retryNum, ACE_INT32 /*milliseconds*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Starting MTAP - index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (thedsdServer) return 0;
	
	if (retryNum <= 0) retryNum = 1;

	int errorLevel = 0;

	while (retryNum--) {
		errorLevel = 0;

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		//Create the DSD Server.
		if (!(thedsdServer = new (std::nothrow) ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET))) {
			errorLevel = 3;
			ACE_OS::sleep(1);
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to allocate memory for ACS_DSD_Server object, retrying...");
			continue;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				if (thedsdServer) {
					thedsdServer->close();
					delete thedsdServer; thedsdServer = 0;
				}
			ACS_ACA_CRITICAL_SECTION_GUARD_END(); 

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}
		//Open the DSD Server
		int res = thedsdServer->open();

		if (res < 0) {
			errorLevel = 3;
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				delete thedsdServer; thedsdServer = 0;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();
			ACE_OS::sleep(1);

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'open' failed with error code %d, retrying...", res);
			continue;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				if (thedsdServer) {
					thedsdServer->close();
					delete thedsdServer; thedsdServer = 0;
				}
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		res = thedsdServer->publish(ACS_MSD_Service::getAddress(indextoParam)->name, _domainName.c_str() /*,acs_dsd::SERVICE_VISIBILITY_LOCAL_SCOPE*/ );

		//Publish the DSD Server
		if (res != 0) {
			errorLevel = 3;
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				if (thedsdServer) {
					thedsdServer->close();
					delete thedsdServer; thedsdServer = 0;
				}
			ACS_ACA_CRITICAL_SECTION_GUARD_END();
			ACE_OS::sleep(1);

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'publish' failed with error code %d, retrying...", res);
			continue;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				if (thedsdServer) {
					thedsdServer->unregister();
					thedsdServer->close();
					delete thedsdServer; thedsdServer = 0;
				}
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		//Get DSD Server handles
		int ret = thedsdServer->get_handles(_dsdEvents, _dsdNumOfEvents);
		if (ret < 0) {
			errorLevel = 3;
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				if (thedsdServer) {
					thedsdServer->unregister();
					thedsdServer->close();
					delete thedsdServer; thedsdServer = 0;
				}
			ACS_ACA_CRITICAL_SECTION_GUARD_END();
			ACE_OS::sleep(1);

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'get_handles' failed, retrying...");
			continue;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				if (thedsdServer) {
					thedsdServer->unregister();
					thedsdServer->close();
					delete thedsdServer; thedsdServer = 0;
				}
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		//Check the number of handles got by DSD Server.
		if (_dsdNumOfEvents <= 0) {
			errorLevel = 3;

			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
				if (thedsdServer) {
					thedsdServer->unregister();
					thedsdServer->close();
					delete thedsdServer; thedsdServer = 0;
				}
			ACS_ACA_CRITICAL_SECTION_GUARD_END();
			ACE_OS::sleep(1);

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'get_handles' returned 0 handles, retrying...");
			continue;
		}

		break;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACAMSD_MS_Manager::fxStopMTAP () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Stopping MTAP - index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	int errorLevel = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
		if (!thedsdServer) return 0;

		//Unregister, close and delete the DSD Server.
		int res = 0;
		res = thedsdServer->unregister();
		if (res) {
			errorLevel = 3;
		}

		if (thedsdServer->close() != 0) {
			errorLevel++;
		}

		delete thedsdServer; thedsdServer = 0;
	
		_dsdNumOfEvents = 0;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();
	
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACAMSD_MS_Manager::fxStartMSIP (unsigned long retryNum, ACE_INT32 /*milliseconds*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Starting MSIP - index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (_msipListenSocket != INVALID_SOCKET) return 0;

	if (retryNum <= 0) retryNum = 1;

	int errorLevel = 0;

	while (retryNum--) {
		errorLevel = 0;

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		//Create the listening socket.
		if ((_msipListenSocket = ACE_OS::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
			errorLevel = 3;
			ACE_OS::sleep(1);
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to create the MSIP socket, retrying...");
			continue;
		}
	
		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				if ((_msipListenSocket != INVALID_SOCKET) && ((ACE_OS::closesocket(_msipListenSocket)) == SOCKET_ERROR)) {}
				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		//Build the local socket name
		_msipService.sin_family = AF_INET;
		_msipService.sin_addr.s_addr = ::inet_addr("127.0.0.1");
		_msipService.sin_port = ::htons(_msipPort);

		//Try to bind the MSIP service.
		if (::bind(_msipListenSocket, reinterpret_cast<sockaddr *>(&_msipService), sizeof(_msipService))) { //Error
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'bind' failed for MSIP socket, errno = %d, index = %d, MS = %s",
					errno, indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);

			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				if ((_msipListenSocket != INVALID_SOCKET) && ACE_OS::closesocket(_msipListenSocket)) {}
				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			errorLevel = 3;
			ACE_OS::sleep(1);
			continue;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				if ((_msipListenSocket != INVALID_SOCKET) && ACE_OS::closesocket(_msipListenSocket)) {}
				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		//Try to listen on the socket
		if (::listen(_msipListenSocket, 10)) { // Error
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'listen' failed for MSIP socket, errno = %d, index = %d, MS = %s",
					errno, indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);

			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				if ((_msipListenSocket != INVALID_SOCKET) && ACE_OS::closesocket(_msipListenSocket)) {}
				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			errorLevel = 3;
			ACE_OS::sleep(1);
			continue;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				if ((_msipListenSocket != INVALID_SOCKET) && ACE_OS::closesocket(_msipListenSocket)) {}
				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		if ((_msipListenEvent = new (std::nothrow) ACE_Event()) == 0) { //Manually reset event
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				if ((_msipListenSocket != INVALID_SOCKET) && ACE_OS::closesocket(_msipListenSocket)) {}
				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			errorLevel = 3;
			ACE_OS::sleep(1);
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to allocate memory for ACE_Event object, retrying...");
			continue;
		}

		if (_state == THREAD_STATE_STOPPED) {
			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				delete _msipListenEvent;
				_msipListenEvent = 0;

				if (_msipListenEvent && !_msipListenEvent->remove()) {}

				_msipListenEvent = 0;

				if ((_msipListenSocket != INVALID_SOCKET) && ACE_OS::closesocket(_msipListenSocket)) {}

				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
			return 1;
		}

		//Finally, try to select the ACCEPT event on the event object created.

		//need to change the statement while real implementation as it require some socket analysis

		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		const ACE_Time_Value tv(5,0);
		
		ACE_Handle_Set readHandleSet;
		readHandleSet.set_bit(_msipListenSocket);

		int ret = ACE_OS::select(readHandleSet.max_set() + 1, readHandleSet, 0, 0, tv);
		if (ret < 0) { // ERROR
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'select' failed, errno = %d, index = %d, MS = %s",
					ACE_OS::last_error(), indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);

			ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
				delete _msipListenEvent; _msipListenEvent = 0;

				if ((_msipListenSocket != INVALID_SOCKET) && ACE_OS::closesocket(_msipListenSocket)) {}
				_msipListenSocket = INVALID_SOCKET;
			ACS_ACA_CRITICAL_SECTION_GUARD_END();

			errorLevel = 3;
			ACE_OS::sleep(1);
			continue;
		}

		break;
	}

	if (errorLevel) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Error level = %d", errorLevel);
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACAMSD_MS_Manager::fxStopMSIP () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Stopping MSIP - index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
		if (_msipListenSocket != INVALID_SOCKET) {
			ACE_OS::shutdown(_msipListenSocket, 2); // SD_BOTH
		}

		delete _msipListenEvent; _msipListenEvent = 0;

		if (_msipListenSocket != INVALID_SOCKET) {
			ACE_OS::closesocket(_msipListenSocket);
		}
		_msipListenSocket = INVALID_SOCKET;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::fxWorking () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int handleNum = 1 + _dsdNumOfEvents + 1;	// 1 stop_event + N MTAP + 1 MSIP
	int errno_save = 0;
		
	if (handleNum >= 64) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The handle number is greater than the limit, returning 3");
		return 3;
	}

	int errorLevel = 0;

	//Build the listening event array
	ACE_HANDLE listenEvents[64] = {ACE_INVALID_HANDLE};

	listenEvents[0] = _stop_event_fd;

	int j = 1;
	for (int i = 0; i < _dsdNumOfEvents; listenEvents[j] = _dsdEvents[i++], j++) ;

	listenEvents[j] = _msipListenSocket;

	ACE_INT32 waitResult = 0;

	struct pollfd myPollfds[handleNum];
	for (ACE_INT32 i = 0; i < handleNum; i++) {
		myPollfds[i].fd = listenEvents[i];
		myPollfds[i].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
		myPollfds[i].revents = 0;
	}

	ACE_Time_Value selectTime;
	selectTime.set(60);
	
	for (bool again = true; again; ) {
		errorLevel = 0;

		if ((_state == THREAD_STATE_STOPPED) || ACS_MSD_Service::getAddress(indextoParam)->nameChanged) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested (state is %d) or name changed for index %d, returning 1",	_state, indextoParam);
			return 1;
		}
			
		fxCreateMTAPChannelManagersFromCPList();
		fxCreateMSIPChannelManagersFromCPList();
			
		if ((_state == THREAD_STATE_STOPPED) || ACS_MSD_Service::getAddress(indextoParam)->nameChanged) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested (state is %d) or name changed for index %d, returning 1", _state, indextoParam);
			return 1;
		}

		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Calling poll - index = %d, MS = %s", indextoParam,
				ACS_MSD_Service::getAddress(indextoParam)->name);

		errno = 0;
		waitResult = ACE_OS::poll(myPollfds, handleNum, &selectTime);
		errno_save = errno;

		switch (waitResult) {
		case -1: // Wait failed
			if (errno_save != EINTR) {
				again = false;
				errorLevel = 5;
				_mtapHasToBeRestarted = _msipHasToBeRestarted = true;
			}

			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'poll' failed, errno = %d, returning %d!", errno_save, errorLevel);
			break;

		case 0: // Wait timed out
			ACS_ACA_LOG(LOG_LEVEL_TRACE, "Expired timeout - index = %d, MS = %s",
					indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);
			break;

		default:
			// FIRST: Check if the stop event has been signaled
			if ((myPollfds[0].revents & (POLLIN | POLLPRI)) || (ACS_MSD_Service::getAddress(indextoParam)->nameChanged)) {
				ACS_ACA_LOG(LOG_LEVEL_INFO, "Stop requested or name changed for index %d (revents is %d), returning 1",
						indextoParam, myPollfds[0].revents);
				return 1;
			}

			// SECOND: Check if an connection (MTAP or MSIP) is available
			bool event_found = false;
			for (int i = 1; i < handleNum; i++) {
				if (myPollfds[i].revents & (POLLIN | POLLPRI)) {
					event_found = true;

					if (i < (handleNum - 1)) {
						// An MTAP connection is available, try to accept it
						ACS_ACA_LOG(LOG_LEVEL_DEBUG, "An MTAP connection for the message store %s is available, trying to accept it!",
								ACS_MSD_Service::getAddress(indextoParam)->name);

						_mtapHasToBeRestarted = !(again = ((errorLevel = fxTryAcceptMTAPConnection()) >= 0));
					}
					else {
						// An MSIP connection is available, try to accept it
						ACS_ACA_LOG(LOG_LEVEL_DEBUG, "An MSIP connection for the message store %s is available, trying to accept it!",
								ACS_MSD_Service::getAddress(indextoParam)->name);

						if (!_msipListenEvent->reset()) {}
						_msipHasToBeRestarted = !(again = ((errorLevel = fxTryAcceptMSIPConnection()) >= 0));
					}
				}
				else if (myPollfds[i].revents & (POLLRDHUP | POLLHUP | POLLERR)) {
					// Error on the handle, restart the acceptor
					if (i < (handleNum - 1)) {
						// Error on an MTAP handle, restart the MTAP acceptor
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error on an MTAP handle (index %d) for the message store %s, restarting "
								"MTAP acceptor: revents = %d", i, ACS_MSD_Service::getAddress(indextoParam)->name, myPollfds[i].revents);

						_mtapHasToBeRestarted = !(again = false);
						break;
					}
					else {
						// Error on the MSIP handle, restart the MSIP acceptor
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error on an MSIP handle (index %d) for the message store %s, restarting "
								"MSIP acceptor: revents = %d", i, ACS_MSD_Service::getAddress(indextoParam)->name, myPollfds[i].revents);

						_msipHasToBeRestarted = !(again = false);
						break;
					}
				}
			}

			if (!event_found && again) {
				_mtapHasToBeRestarted = _msipHasToBeRestarted = true;
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "No events found, restarting the channels - index = %d, MS = '%s'",
						indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name);
				return 5;
			}

			break;
		}

		if ((_state == THREAD_STATE_STOPPED) || ACS_MSD_Service::getAddress(indextoParam)->nameChanged) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested (state is %d) or name changed for index %d, returning 1", _state, indextoParam);
			return 1;
		}

		fxCheckMTAPChannels();
		fxCheckMSIPChannels();
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACAMSD_MS_Manager::fxTryAcceptMTAPConnection () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	ACS_DSD_Session * pSession = 0;

	if (!(pSession = new (std::nothrow) ACS_DSD_Session())) {
		if (_acceptMTAPConnNewRetryNum++ >= ACCEPT_MTAP_CONN_MAX_NEW_RETRIES) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERT [_acceptMTAPConnNewRetryNum++ >= ACCEPT_MTAP_CONN_MAX_NEW_RETRIES], returning -5");
			return -5;
		}

		return 5;
	}
	_acceptMTAPConnNewRetryNum = 0;

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		delete pSession;
		return 1;
	}

	int errorLevel = 0;

	if ((errorLevel = fxInitializeDSDSession(pSession))) {
		delete pSession;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxInitializeDSDSession' failed with error code %d", errorLevel);
		return errorLevel;
	}

	if (_state == THREAD_STATE_STOPPED) {
		pSession && pSession->close();
		delete pSession;
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	ACS_DSD_Node remoteNode;

	int ret = pSession->get_remote_node(remoteNode);
	if (ret < 0) {
		pSession && pSession->close();
		delete pSession;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'get_remote_node' failed with error code %d", ret);
		return 5;
	}

	if (_state == THREAD_STATE_STOPPED) {
		pSession && pSession->close();
		delete pSession;
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}
		
	ACAMSD_MTAP_ChannelManager * mtapChannel = 0;
		
	if ((errorLevel = fxGetMTAPChannelManager(remoteNode.system_id, mtapChannel))) {
		pSession && pSession->close();
		delete pSession;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxGetMTAPChannelManager' failed, return code is %d", errorLevel);
		return errorLevel;
	}
		
	if (_state == THREAD_STATE_STOPPED) {
		pSession && pSession->close();
		delete pSession;
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}
		
	//Can add the session to channel manager
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_mtap_msip_shared_sync);
		errorLevel = mtapChannel->addSession(pSession);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	if (errorLevel != ACAMSD_MTAP_ChannelManager::RETURN_OK) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'addSession' failed, return code is %d", errorLevel);
		pSession && pSession->close();
		delete pSession;
		return errorLevel;
	}

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "New MTAP session added for the couple (%s, %d)",
			ACS_MSD_Service::getAddress(indextoParam)->name, remoteNode.system_id);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACAMSD_MS_Manager::fxTryAcceptMSIPConnection () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACE_HANDLE clientSocket;
	int errorLevel = 0;

	struct sockaddr_in clientAddrInfo;
	int addrLen = sizeof(clientAddrInfo);

	clientSocket = ACE_OS::accept(_msipListenSocket, reinterpret_cast<struct sockaddr *>(&clientAddrInfo), &addrLen);
		
	if (clientSocket == INVALID_SOCKET) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'accept' failed, errno = %d", ACE_OS::last_error());
		return 5;
	}

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, sending ConnectionRefused primitive!");
		if (!ACAMS_MSIP_Protocol::sendConnectionRefused(clientSocket, "MS Manager is stopped!")) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send ConnectionRefused primitive!");
		}

		if (ACE_OS::closesocket(clientSocket)) {}

		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	 }

	ACE_Event * clientSockEvent = new (std::nothrow) ACE_Event();

	if (!clientSockEvent) { // ERROR: memory allocation
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for ACE_Event object, sending ConnectionRefused primitive!");
		if (!ACAMS_MSIP_Protocol::sendConnectionRefused(clientSocket, "'WSACreateEvent()' failed at MSIP server side!")) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send ConnectionRefused primitive!");
		}

		if (ACE_OS::closesocket(clientSocket)) {}
		return 5;
	}

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, sending ConnectionRefused primitive!");
		if (!ACAMS_MSIP_Protocol::sendConnectionRefused(clientSocket, "MS Manager is stopped!")) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send ConnectionRefused primitive!");
		}

		delete clientSockEvent; clientSockEvent = 0;

		if (ACE_OS::closesocket(clientSocket)) {}
		return 1;
	}

	ACAMSD_MSIP_ChannelManager * msipChannel = 0;
	unsigned cpSystemId = SINGLE_CP_ID;

	if ((errorLevel = (_isMultipleCPSystem ? fxGetCPSystemIdFromMSIPClient(clientSocket, cpSystemId) : 0)) ||
			(errorLevel = fxGetMSIPChannelManager(cpSystemId, msipChannel))) {

		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Server error: call 'fxGetCPSystemIdFromMSIPClient' or 'fxGetMSIPChannelManager' "
				"failed, sending ConnectionRefused primitive!");

		if (!ACAMS_MSIP_Protocol::sendConnectionRefused(clientSocket, "Server error!")) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to send ConnectionRefused primitive!");
		}

		delete clientSockEvent; clientSockEvent = 0;

		if (ACE_OS::closesocket(clientSocket)) {}
		return errorLevel;
	}

	//Can add the session to channel manager
	//need to check while real implementation
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_mtap_msip_shared_sync);
		errorLevel = msipChannel->addSession(clientSocket, clientSockEvent);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	if (errorLevel) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'addSession' failed, return code is %d", errorLevel);
		delete clientSockEvent; clientSockEvent = 0;

		if (ACE_OS::closesocket(clientSocket)) {}
		return errorLevel;
	}

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "New MSIP session added for the couple (%s, %d)",
			ACS_MSD_Service::getAddress(indextoParam)->name, cpSystemId);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACAMSD_MS_Manager::fxInitializeDSDSession (ACS_DSD_Session * pSession) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int errorLevel = 0;
	
	//Try to accept the MTAP comunication session
	int res = thedsdServer->accept(*pSession);
	if (res < 0) {
		if (_initDSDSessionAcceptRetryNum++ < INIT_DSD_SESSION_MAX_ACCEPT_RETRIES) {
			errorLevel = 5;
		}
		else {
			errorLevel = -5;
		}

		if (_initDSDSessionAcceptRetryNum >= INIT_DSD_SESSION_MAX_ACCEPT_RETRIES) {}

		ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'accept' failed, return code is %d, returning %d", res, errorLevel);
	}
	else {
		_initDSDSessionAcceptRetryNum = 0;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACAMSD_MS_Manager::fxStopMTAPChannels (unsigned long /*retryNum*/, ACE_INT32 /*milliseconds*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (_mtapChannelMap.size() <= 0) return 0;

	ACAMSD_MTAP_ChannelManager * mtapChannel = 0;
	mtapChannelMap_t::iterator it = _mtapChannelMap.begin();
	
	//Propagate the stop signal to all MTAP channel children
	while (it != _mtapChannelMap.end()) {
		if ((mtapChannel = (it++)->second)) mtapChannel->stopMTAP();
	}

	for (it = _mtapChannelMap.begin(); it != _mtapChannelMap.end(); it++) {
		mtapChannel = it->second;
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Joining the MTAP Channel Manager Thread for cp_id %u...", it->first);
		ACE_Thread_Manager::instance()->join(mtapChannel->_tid);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "MTAP Channel Manager Thread for cp_id %u terminated!", it->first);
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::fxDeleteMTAPChannels () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (_mtapChannelMap.size() <= 0)	return 0;
	
	for (mtapChannelMap_t::iterator it = _mtapChannelMap.begin(); it != _mtapChannelMap.end(); (it++)->second = 0)
		delete it->second;

	_mtapChannelMap.clear();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::fxCheckMTAPChannels () {
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
		if (_mtapChannelMap.size() <= 0) return 0;

		/*
		 * New code to solve map structure corruption and also Windows code bug
		 */

		mtapChannelMap_t mtapChannelMapSave(_mtapChannelMap);
		ACAMSD_MTAP_ChannelManager * mtapChannel = 0;

		_mtapChannelMap.clear();

		for (mtapChannelMap_t::iterator it = mtapChannelMapSave.begin(); it != mtapChannelMapSave.end(); ++it) {
			if ((mtapChannel = it->second)) { // MTAP channel object found
				if (mtapChannel->_state != THREAD_STATE_STOPPED)
					_mtapChannelMap[it->first] = mtapChannel; // This MTAP channel objet is good. Keep it!
				else {
					ACS_ACA_LOG(LOG_LEVEL_WARN, "The MTAP channel for cp_id = %d is stopped, joining it!", it->first);
					ACE_Thread_Manager::instance()->join(mtapChannel->_tid);
					delete mtapChannel;
				}
			}
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return 0;
}

int ACAMSD_MS_Manager::fxStopMSIPChannels (unsigned long /*retryNum*/, ACE_INT32 /*milliseconds*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (_msipChannelMap.size() <= 0) return 0;

	ACAMSD_MSIP_ChannelManager * msipChannel = 0;
	msipChannelMap_t::iterator it = _msipChannelMap.begin();
	
	//Propagate the stop signal to all MSIP channel children
	while (it != _msipChannelMap.end()) {
		if ((msipChannel = (it++)->second)) msipChannel->stopMSIP();
	}

	for (it = _msipChannelMap.begin(); it != _msipChannelMap.end(); it++) {
		msipChannel = it->second;
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Joining the MSIP Channel Manager Thread for cp_id %u...", it->first);
		ACE_Thread_Manager::instance()->join(msipChannel->_tid);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "MSIP Channel Manager Thread for cp_id %u terminated!", it->first);
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::fxDeleteMSIPChannels () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "index = %d, MS = '%s', TID = %ld",
			indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (_msipChannelMap.size() <= 0)	return 0;
	
	for (msipChannelMap_t::iterator it = _msipChannelMap.begin(); it != _msipChannelMap.end(); (it++)->second = 0)
		delete (it)->second;

	_msipChannelMap.clear();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::fxCheckMSIPChannels () {
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
		if (_msipChannelMap.size() <= 0) return 0;

		/*
		 * New code to solve map structure corruption and also Windows code bug
		 */

		msipChannelMap_t msipChannelMapSave(_msipChannelMap);
		ACAMSD_MSIP_ChannelManager * msipChannel = 0;

		_msipChannelMap.clear();

		for (msipChannelMap_t::iterator it = msipChannelMapSave.begin(); it != msipChannelMapSave.end(); ++it) {
			if ((msipChannel = it->second)) { // MSIP channel object found
				if (msipChannel->_state != THREAD_STATE_STOPPED)
					_msipChannelMap[it->first] = msipChannel; // This MSIP channel objet is good. Keep it!
				else {
					ACS_ACA_LOG(LOG_LEVEL_WARN, "The MSIP channel for cp_id = %d is stopped, joining it!", it->first);
					ACE_Thread_Manager::instance()->join(msipChannel->_tid);
					delete msipChannel; // This MSIP channel must be deleted
				}
			}
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return 0;
}

int ACAMSD_MS_Manager::fxGetMTAPChannelManager (unsigned cpSystemId, ACAMSD_MTAP_ChannelManager * & mtapCM) {
	ACAMSD_MTAP_ChannelManager * cm = 0;

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	int errorLevel = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
		if (!_isMultipleCPSystem) cpSystemId = SINGLE_CP_ID;

		for (;;) {
			mtapChannelMap_t::iterator it = _mtapChannelMap.find(cpSystemId);
			if (it == _mtapChannelMap.end()) { //Not found
				if (_state == THREAD_STATE_STOPPED) {
					ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
					return 1;
				}

				if ((errorLevel = fxCreateMTAPChannelManager(cpSystemId, cm))) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxCreateMTAPChannelManager' failed!");
					return errorLevel;
				}

				if ((errorLevel = fxStartMTAPChannelManager(cm))) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxStartMTAPChannelManager' failed!");
					delete cm;

					//CHANGE_HR65432: 10th September 2013
					cm = 0;
					
					return errorLevel;
				}

				_mtapChannelMap.insert(mtapChannelMap_t::value_type(cpSystemId, cm));
				break;
			}
			else { // MTAP Channel manager found into the internal map
				if (!(cm = it->second) || (cm->_state == THREAD_STATE_STOPPED)) {
					delete cm;

					//CHANGE_HR65432: 10th September 2013
					it->second = cm = 0;

					ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERT[!(cm = it->second) || !(cm->_state)], removing the channel from the map!");
					_mtapChannelMap.erase(it);
				}
				else break;
			}
		}

		mtapCM = cm;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return 0;
}

int ACAMSD_MS_Manager::fxGetMSIPChannelManager (unsigned cpSystemId, ACAMSD_MSIP_ChannelManager * & msipCM) {
	ACAMSD_MSIP_ChannelManager * cm = 0;

	if (_state == THREAD_STATE_STOPPED) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
		return 1;
	}

	int errorLevel = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
		if (!_isMultipleCPSystem) cpSystemId = SINGLE_CP_ID;

		for (;;) {
			msipChannelMap_t::iterator it = _msipChannelMap.find(cpSystemId);
			
			if (it == _msipChannelMap.end()) { //Not found
				if (_state == THREAD_STATE_STOPPED) {
					ACS_ACA_LOG(LOG_LEVEL_WARN, "Stop requested, returning 1");
					return 1;
				}

				if ((errorLevel = fxCreateMSIPChannelManager(cpSystemId, cm))) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxCreateMSIPChannelManager' failed!");
					return errorLevel;
				}

				if ((errorLevel = fxStartMSIPChannelManager(cm))) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxStartMSIPChannelManager' failed!");
					delete cm; cm = 0;
					return errorLevel;
				}

				_msipChannelMap.insert(msipChannelMap_t::value_type(cpSystemId, cm));
				break;
			}
			else { // An MSIP channel manger found into the internal map
				if (!(cm = it->second) || (cm->_state == THREAD_STATE_STOPPED)) {
					delete cm;

					//CHANGE_HR65432: 10th September 2013
					it->second = cm = 0;

					ACS_ACA_LOG(LOG_LEVEL_ERROR, "ASSERT[!(cm = it->second) || !(cm->_state)], removing the channel from the map!");
					_msipChannelMap.erase(it);
				}
				else break;
			}
		}

		msipCM = cm;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();
	
	return 0;
}

int ACAMSD_MS_Manager::fxCreateMTAPChannelManager (unsigned systemId, ACAMSD_MTAP_ChannelManager * & cm) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Creating an MTAP channel manager for cp_id = %u, index = %d, MS = '%s', TID = %ld",
			systemId, indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (_isMultipleCPSystem)
		cm = new (std::nothrow) ACAMSD_MTAP_ChannelManager(this, systemId, _isMultipleCPSystem, indextoParam);
	else
		cm = new (std::nothrow) ACAMSD_MTAP_ChannelManager(this, indextoParam);

	if (!cm) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for ACAMSD_MTAP_ChannelManager object, returning -5");
		return -5;
	}

	const int call_result = cm->initialize_post_ctor();
	if ( call_result ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to initialize the ACAMSD_MTAP_ChannelManager object,"
				" call_result = %d, returning -5", call_result);
		delete cm; cm = 0;
		return -5;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::fxCreateMSIPChannelManager (unsigned systemId, ACAMSD_MSIP_ChannelManager * & cm) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Creating an MSIP channel manager for cp_id = %u, index = %d, MS = '%s', TID = %ld",
			systemId, indextoParam, ACS_MSD_Service::getAddress(indextoParam)->name, _tid);

	if (_isMultipleCPSystem)
		cm = new (std::nothrow) ACAMSD_MSIP_ChannelManager(this, systemId, _isMultipleCPSystem, indextoParam);
	else
		cm = new (std::nothrow) ACAMSD_MSIP_ChannelManager(this, indextoParam);

	if (!cm) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for ACAMSD_MSIP_ChannelManager object, returning -5");
		return -5;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

ACE_THR_FUNC_RETURN threadStartupMTAP (void * param) {
	ACAMSD_MTAP_ChannelManager * mtapMgr = reinterpret_cast<ACAMSD_MTAP_ChannelManager *>(param);

	mtapMgr->_state = THREAD_STATE_RUNNING;
	mtapMgr->workerMain();
	mtapMgr->_stoppedState = true;
	mtapMgr->_state = THREAD_STATE_STOPPED;

	return 0;
}

int ACAMSD_MS_Manager::fxStartMTAPChannelManager (ACAMSD_MTAP_ChannelManager * cm) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	const int call_result = ACE_Thread_Manager::instance()->spawn(
			threadStartupMTAP,
			cm,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&(cm->_tid),
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if (call_result == -1) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the MTAP Channel Manager Thread, errno = %d", ACE_OS::last_error());
			return -1;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

ACE_THR_FUNC_RETURN threadStartupMSIP (void * param) {
	ACAMSD_MSIP_ChannelManager * msipMgr = reinterpret_cast<ACAMSD_MSIP_ChannelManager *>(param);

	msipMgr->_state = THREAD_STATE_RUNNING;
	msipMgr->workerMain();
	msipMgr->_stoppedState = true;
	msipMgr->_state = THREAD_STATE_STOPPED;

	return 0;
}

int ACAMSD_MS_Manager::fxStartMSIPChannelManager (ACAMSD_MSIP_ChannelManager * cm) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	const int call_result = ACE_Thread_Manager::instance()->spawn(
			threadStartupMSIP,
			cm,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&(cm->_tid),
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if (call_result == -1) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the MSIP Channel Manager Thread, errno = %d", ACE_OS::last_error());
			return -1;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MS_Manager::fxCreateMTAPChannelManagersFromCPList () {
	ACAMSD_MTAP_ChannelManager * mtapChannel = 0;

	if (!_isMultipleCPSystem) return fxGetMTAPChannelManager(SINGLE_CP_ID, mtapChannel);

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		unsigned systemId = 0;
		int errorLevel = 0;

		for (std::vector<unsigned>::size_type i = 0; i < _cpIdList.size(); ++i) {
			systemId = _cpIdList[i];
			if ((errorLevel = fxGetMTAPChannelManager(systemId , mtapChannel)))
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'fxGetMTAPChannelManager' failed, return code is %d", errorLevel);
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return 0;
}

int ACAMSD_MS_Manager::fxCreateMSIPChannelManagersFromCPList () {
	ACAMSD_MSIP_ChannelManager * msipChannel = 0;

	if (!_isMultipleCPSystem) return fxGetMSIPChannelManager(SINGLE_CP_ID, msipChannel);

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
		unsigned systemId = 0;
		int errorLevel = 0;

		for (std::vector<unsigned>::size_type i = 0; i < _cpIdList.size(); ++i) {
			systemId = _cpIdList[i];
			if ((errorLevel = fxGetMSIPChannelManager(systemId , msipChannel)))
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'fxGetMSIPChannelManager' failed, return code is %d", errorLevel);
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	return 0;
}

int ACAMSD_MS_Manager::fxGetCPSystemIdFromMSIPClient (ACE_SOCKET clientSock, unsigned & cpSystemId) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (!ACAMS_MSIP_Protocol::sendCPSystemIdRequest(clientSock)) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendCPSystemIdRequest' failed, returning 4");
		return 4;
	}
	
	ACAMS_MSIP_Protocol::Primitive primitiveId;
	unsigned int size = 0;
	unsigned char buffer[2 * 1024] = {0};
	
	primitiveId = ACAMS_MSIP_Protocol::getPrimitive(clientSock, size, buffer);

	int errorLevel = 0;

	switch (primitiveId) {
	case ACAMS_MSIP_Protocol::CPSystemId:
		if (ACAMS_MSIP_Protocol::unpackCPSystemId(buffer, size, cpSystemId) != ACAMS_MSIP_Protocol::NoError) {
			errorLevel = 5;
		}
		break;
		
	case ACAMS_MSIP_Protocol::CPSystemIdUnavailable:
		errorLevel = 2;
		break;

	default:
		errorLevel = 4;
		break;
	}
	
	if (errorLevel)
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Something failed, errorLevel is %d", errorLevel);
	
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

bool ACAMSD_MS_Manager::setCPConnectionNotificationEvent () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	mqd_t mq_descriptor;
	char mq_name[aca_rtr_communication::MESSAGE_QUEUE_NAME_MAX_SIZE];
	(!indextoParam)
			?	::snprintf(mq_name, sizeof(mq_name), "%s%s", aca_rtr_communication::MESSAGE_QUEUE_PREFIX,	aca_rtr_communication::MS_MESS_QUEUE_NAME_PREFIX)
			: ::snprintf(mq_name, sizeof(mq_name), "%s%s%d", aca_rtr_communication::MESSAGE_QUEUE_PREFIX,	aca_rtr_communication::MS_MESS_QUEUE_NAME_PREFIX, indextoParam);

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Trying to open the message queue %s", mq_name);
	if ((mq_descriptor = ::mq_open(mq_name, O_WRONLY)) == -1) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mq_open' failed for the message queue %s, errno = %d", mq_name, errno);
		return false;
	}

	char buffer[aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE];
	::snprintf(buffer, sizeof(buffer), "%s", aca_rtr_communication::NEW_CP_CONNECTED_MESSAGE);

	if (::mq_send(mq_descriptor, buffer, ::strlen(buffer), aca_rtr_communication::MESSAGE_QUEUE_MSG_DEFAULT_PRIORITY) == -1) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mq_send' failed, errno = %d", errno);
		::mq_close(mq_descriptor);
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Info about the new CP correctly sent on the message queue %s!", mq_name);
	if (::mq_close(mq_descriptor) == -1) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'mq_close' failed, errno = %d", errno);
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

void ACAMSD_MS_Manager::notify_cptable_change(unsigned cp_id, int op_type) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (op_type == ACS_ACA_CPTable_Observer::BLADE_ADD) {
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Adding the CP having system id = %u in the internal collection of IDs.", cp_id);

		ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
			_cpIdList.push_back(cp_id);
		ACS_ACA_CRITICAL_SECTION_GUARD_END();
		return;
	}

	if (op_type == ACS_ACA_CPTable_Observer::BLADE_REMOVE) {
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Stopping and deleting MTAP and MSIP channels for the CP having system id = %u.", cp_id);
		stop_remove_mtap_channel(cp_id);
		stop_remove_msip_channel(cp_id);

		ACS_ACA_LOG(LOG_LEVEL_INFO, "Removing the CP having system id = %u from the internal collection of IDs.", cp_id);

		ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_syncAccess);
			for (std::vector<unsigned>::iterator it = _cpIdList.begin(); it != _cpIdList.end(); it++) {
				if (*it == cp_id) {
					_cpIdList.erase(it);
					break;
				}
			}
		ACS_ACA_CRITICAL_SECTION_GUARD_END();
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}


#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
int ACAMSD_MS_Manager::notify_os_caching_status(bool /*caching_status*/)
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int n_notify_errors = 0;

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);

	for (mtapChannelMap_t::iterator it = _mtapChannelMap.begin(); it != _mtapChannelMap.end(); ++it) {
		ACAMSD_MTAP_ChannelManager * mtap_channel = it->second;
		if (!mtap_channel) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Found a null MTAP channel manager. MAP KEY is %u !", it->first);
			continue;
		}

		if(mtap_channel->notify_event(ACAMSD_MTAP_ChannelManager::EVENT_OS_CACHING_PARAM_CHANGE)) {
			++n_notify_errors;
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Unable to notify OS caching parameter change to the MTAP channel manager having MAP KEY  %u !", it->first);
		}
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return -n_notify_errors;
}
#endif

void ACAMSD_MS_Manager::stop_remove_mtap_channel (unsigned cp_id) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MTAP_syncAccess);
		mtapChannelMap_t::iterator it = _mtapChannelMap.find(cp_id);
		if (it == _mtapChannelMap.end()) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "No MTAP channel manager found for CP having system id = %u!", cp_id);
			return;
		}

		ACAMSD_MTAP_ChannelManager * mtap_channel = it->second;
		if (!mtap_channel) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "The MTAP channel manager for CP having system id = %u is NULL!", cp_id);
			return;
		}

		mtap_channel->stopMTAP();

		ACS_ACA_LOG(LOG_LEVEL_INFO, "Joining the MTAP Channel Manager Thread for cp_id %u...", cp_id);
		ACE_Thread_Manager::instance()->join(mtap_channel->_tid);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "MTAP Channel Manager Thread for cp_id %u terminated!", cp_id);

		delete mtap_channel; mtap_channel = 0;
		_mtapChannelMap.erase(cp_id);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACAMSD_MS_Manager::stop_remove_msip_channel (unsigned cp_id) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_MSIP_syncAccess);
		msipChannelMap_t::iterator it = _msipChannelMap.find(cp_id);
		if (it == _msipChannelMap.end()) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "No MSIP channel manager found for CP having system id = %u!", cp_id);
			return;
		}

		ACAMSD_MSIP_ChannelManager * msip_channel = it->second;
		if (!msip_channel) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "The MSIP channel manager for CP having system id = %u is NULL!", cp_id);
			return;
		}

		msip_channel->stopMSIP();

		ACS_ACA_LOG(LOG_LEVEL_INFO, "Joining the MSIP Channel Manager Thread for cp_id %u...", cp_id);
		ACE_Thread_Manager::instance()->join(msip_channel->_tid);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "MSIP Channel Manager Thread for cp_id %u terminated!", cp_id);

		delete msip_channel; msip_channel = 0;
		_msipChannelMap.erase(cp_id);
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}
