/*=================================================================== */
/**
        @file           acs_aca_msd_mtap_channel_mgr.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACAMSD_MTAP_ChannelManager class. 

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       09/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef __ACAMSD_MTAP_CHANNEL_MANAGER_H__
#define __ACAMSD_MTAP_CHANNEL_MANAGER_H__

/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <utility>
#include <queue>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>

#include "ace/ACE.h"
#include "ace/Event.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Event_Handler.h"
#include "ace/Synch.h"
#include "ace/Thread_Manager.h"

#include "acs_aca_ms_parameters.h"
#include "acs_aca_msd_msg_store.h"
#include "acs_aca_msd_msg_collection.h"
#include "acs_aca_msd_mtap_protocol.h"
#include "acs_aca_msd_mtap_message.h"
#include "acs_aca_ms_const_values.h"
#include "acs_aca_common.h"

/*====================================================================
                                FORWARD DECLARATION SECTION
==================================================================== */
class ACAMSD_MS_Manager;

/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACAMSD_MTAP_ChannelManager {

/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                TYPEDEF DECLARATION SECTION
==================================================================== */
	typedef std::pair<ACS_DSD_Session *, time_t> sessionInfo_t;

public:
/*====================================================================
                                CONSTANT DECLARATION SECTION
==================================================================== */
	static const int RETURN_OK                           = 0;
	static const int RETURN_STOP                         = 1;
	static const int RETURN_TOO_MANY_CONNECTIONS         = 2;
	static const int MAX_RETRIES_ON_TOO_MANY_CONNECTION  = 10;
	static const int SESSION_INACTIVITY_DEFAULT_TIMEOUT  = 60 * 60 * 12; //12 hours
	static const int SESSION_INACTIVITY_TIMEOUT_ECHO_OFF = 60 * 15; //15 minutes
	static const int SESSION_INACTIVITY_TIMEOUT_ECHO_ON  = 10 * 2; // if the channel is inactive, CP sends an echo msg every 10 seconds
	static const int MTAP_RESULT_Ok                      = 0;
	static const int MTAP_RESULT_MsgStoreFull            = 1;
	static const int MTAP_RESULT_ConnectionCorrupt       = 2;
	static const int MTAP_RESULT_PeerClosedConnection    = 3;
	static const unsigned char SUPPORTED_VERSION_1       = 1;
	static const unsigned char SUPPORTED_VERSION_2       = 2;
	static const unsigned char SUPPORTED_VERSION_3       = 3;
	static const unsigned SINGLE_CP_ID                   = ~0U;

/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */

	enum Event_type {
		EVENT_STOP_REQUEST = 0x1,
		EVENT_OS_CACHING_PARAM_CHANGE = 0x1000
	};

public:
/*=====================================================================
                        CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

   @brief  Constructor for ACAMSD_MTAP_ChannelManager class 

   @param  myMsManager

   @param  index

=================================================================== */
	ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, const short index);
/*===================================================================

   @brief  Constructor for ACAMSD_MTAP_ChannelManager class

   @param  myMsManager

   @param  msParameters

   @param  sessionInactivityTimeout

=================================================================== */
	ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, const ACAMS_Parameters * msParameters,	int sessionInactivityTimeout);
/*===================================================================

   @brief  Constructor for ACAMSD_MTAP_ChannelManager class

   @param  myMsManager

   @param  cpSystemId

   @param  isMultipleCPSystem

   @param  index

=================================================================== */
	ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId, bool isMultipleCPSystem, short index);
/*===================================================================

   @brief  Constructor for ACAMSD_MTAP_ChannelManager class

   @param  myMsManager

   @param  cpSystemId

   @param  isMultipleCPSystem

   @param  msParameters

   @param  sessionInactivityTimeout

=================================================================== */
	ACAMSD_MTAP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId, bool isMultipleCPSystem, const ACAMS_Parameters * msParameters, int sessionInactivityTimeout);

/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================

   @brief  initialize_post_ctor

   @return int

=================================================================== */
	int initialize_post_ctor();

/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMSD_MTAP_ChannelManager ();

/*=====================================================================
                        PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

   @brief  fxFinalize

   @return void

=================================================================== */
	void fxFinalize ();

/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================

   @brief  addSession

   @param  pSession

   @return int

=================================================================== */
	int addSession (ACS_DSD_Session * pSession);
/*===================================================================

   @brief  cpSystemId

   @return unsigned

=================================================================== */
	inline unsigned cpSystemId () { return _cpSystemId; }
/*===================================================================

   @brief  ackToCP
 
   @param  msgToAck

   @param  result

   @return bool

=================================================================== */
	bool ackToCP (ACAMSD_MTAP_Message * msgToAck, ACAMSD_MTAP_Protocol::ErrorCode result = ACAMSD_MTAP_Protocol::NoError);
/*===================================================================

   @brief  getMsgStore

   @return ACAMSD_MsgStore

=================================================================== */
	inline ACAMSD_MsgStore * getMsgStore () const { return _myMsgStore; }
/*===================================================================

   @brief  deleteDataFile

   @param  fileName

   @return int

=================================================================== */
	int deleteDataFile (std::string fileName);
/*===================================================================

   @brief  getNextMsgNumber

   @param  msgNumber

   @return int

=================================================================== */
	int getNextMsgNumber (unsigned long long & msgNumber);
/*===================================================================

   @brief  workerMain

   @return int

=================================================================== */
	int workerMain ();
/*===================================================================

   @brief  stopMTAP

   @return int

=================================================================== */
	int stopMTAP ();

/*===================================================================

   @brief  notify_event

   @param  evt

   @return int

=================================================================== */
	int notify_event(Event_type evt);


/*=====================================================================
                        PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

   @brief  fxActivateIncomingSessions

   @return int

=================================================================== */
	int fxActivateIncomingSessions ();
/*===================================================================

   @brief  fxValidateSessions

   @return int

=================================================================== */
	int fxValidateSessions ();
/*===================================================================

   @brief  fxDoWork

   @return int

=================================================================== */
	int fxDoWork ();
/*===================================================================

   @brief  fxDeleteSession

   @param  index

   @param  sendStopPrimitive

   @return void

=================================================================== */
	void fxDeleteSession (unsigned index, bool sendStopPrimitive = false);
/*===================================================================

   @brief  fxExchange

   @param  sessionIndex

   @return int

=================================================================== */
	int fxExchange (unsigned sessionIndex);
/*===================================================================

   @brief  fxDeleteActiveSessions

   @param  sendStopPrimitive

   @return int

=================================================================== */
	int fxDeleteActiveSessions (bool sendStopPrimitive = false);
/*===================================================================

   @brief  fxDeleteIncomingSessions

   @param  sendStopPrimitive

   @return int

=================================================================== */
	int fxDeleteIncomingSessions (bool sendStopPrimitive = false);
/*===================================================================

   @brief  fxHandleOptionNegotiation

   @param  pSession

   @param  buffer

   @param  numberOfOctets

   @return int

=================================================================== */
	int fxHandleOptionNegotiation (ACS_DSD_Session * pSession, const unsigned char * buffer, unsigned long numberOfOctets);
/*===================================================================

   @brief  fxHandleEchoCheck

   @param  pSession

   @param  buffer

   @param  numberOfOctets

   @return int

=================================================================== */
	int fxHandleEchoCheck (ACS_DSD_Session * pSession, const unsigned char * buffer, unsigned long numberOfOctets);
/*===================================================================

   @brief  fxHandleGetParameter

   @param  pSession

   @param  buffer

   @param  numberOfOctets

   @return int

=================================================================== */
	int fxHandleGetParameter (ACS_DSD_Session * pSession, const unsigned char * buffer, unsigned long numberOfOctets);
/*===================================================================

   @brief  fxHandleUnlink

   @param  pSession

   @param  buffer

   @param  error

   @return int

=================================================================== */
	int fxHandleUnlink (ACS_DSD_Session * pSession, const unsigned char * buffer, ACAMSD_MTAP_Protocol::ErrorCode & error);
/*===================================================================

   @brief  fxHandleSynchronize

   @param  pSession

   @param  buffer

   @param  numberOfOctets

   @return int

=================================================================== */
	int fxHandleSynchronize (ACS_DSD_Session * pSession, const unsigned char * buffer, unsigned long numberOfOctets);
/*===================================================================

   @brief  fxHandlePutRecord

   @param  sessionIndex

   @param  pSession

   @param  buffer

   @param  numberOfOctets

   @param  primitive

   @return void

=================================================================== */
	int fxHandlePutRecord (
			unsigned sessionIndex,
			ACS_DSD_Session * pSession,
			const unsigned char * buffer,
			unsigned long numberOfOctets,
			ACAMSD_MTAP_Protocol::Primitive primitive);
/*===================================================================

   @brief  fxSelectLowestPrioritySession

   @return unsigned

=================================================================== */
	unsigned fxSelectLowestPrioritySession ();

public:
/*====================================================================
                                PUBLIC DATA MEMBER DECLARATION SECTION
==================================================================== */
	ACE_thread_t _tid;
	thread_state_t _state;
	bool _stoppedState;
	const short indextoParam;

private:
/*====================================================================
                                PRIVATE DATA MEMBER DECLARATION SECTION
==================================================================== */
	unsigned _cpSystemId;
	bool _isMultipleCPSystem;
	const ACAMS_Parameters * _msParameters;
	std::queue<sessionInfo_t> _incomingSessionQueue; //to delete into finalizer
	time_t _lastActivityTime;
	sessionInfo_t _sessions[64]; //to delete into finalizer
	int _sessionCount;
	int _sessionInactivityTimeout;
	int _tooManyConnectionsCount;
	bool _echoOn;
	bool _forceTerminate;
	unsigned char _currentVersion;
	ACAMSD_MsgStore * _myMsgStore; //to delete into finalizer
	ACAMSD_MsgCollection * _myMsgCollection;
	unsigned char _myCurrentFileNumber; // MTAP file number of current file
	ACAMSD_MS_Manager * _myMsManager;
	bool _tryToStore;
	bool _firstCPMessage;
	ACE_Recursive_Thread_Mutex _syncAccess;
	int _events_fd;	// used to notify events to the channel manager
};

#endif
