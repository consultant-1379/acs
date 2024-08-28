/*=================================================================== */
/**
        @file           acs_aca_msd_msip_channel_mgr.h

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACAMSD_MSIP_ChannelManager class.

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
#ifndef __ACAMSD_MSIP_CHANNEL_MANAGER_H__
#define __ACAMSD_MSIP_CHANNEL_MANAGER_H__

/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <utility>
#include <queue>
#include <sys/types.h>
#include <pthread.h>

#include "ace/ACE.h"
#include "ace/Event.h"
#include "ace/Event_Handler.h"
#include "ace/Synch.h"
#include "ace/Thread_Manager.h"

#include "acs_aca_ms_parameters.h"
#include "acs_aca_msd_msg_store.h"
#include "acs_aca_msd_msg_collection.h"
#include "acs_aca_defs.h"
#include "acs_aca_ms_msip_protocol.h"


/*====================================================================
                                FORWARD DECLARATION SECTION
==================================================================== */
class ACAMSD_MS_Manager;

/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACAMSD_MSIP_ChannelManager {

/*====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          MSIPSession
=================================================================== */
	struct MSIPSession {
		ACE_HANDLE _socket;
		ACE_Event *_event;
		time_t _time;
		MSIPSession() : _socket(INVALID_SOCKET), _event(0), _time(0) {}
		MSIPSession(ACE_HANDLE socket, ACE_Event * event, time_t time) : _socket(socket), _event(event), _time(time) {}
	};

/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                CONSTANT DECLARATION SECTION
==================================================================== */
	static const int RETURN_OK                          = 0;
	static const int RETURN_STOP                        = 1;
	static const int RETURN_TOO_MANY_CONNECTIONS        = 2;
	static const int MAX_SESSION_NUMBER                 = 1;
	static const int SESSION_INACTIVITY_DEFAULT_TIMEOUT = 60 * 60 * 12; //12 hours
	static const unsigned SINGLE_CP_ID                  = ~0U;

/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for ACAMSD_MSIP_ChannelManager class.

        @param          myMsManager

        @param          index

=================================================================== */
	ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, short index);
/*===================================================================

        @brief          Constructor for ACAMSD_MSIP_ChannelManager class.

        @param          myMsManager

        @param          msParameters

        @param          sessionInactivityTimeout

=================================================================== */
	ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, const ACAMS_Parameters * msParameters,	int sessionInactivityTimeout);
/*===================================================================

        @brief          Constructor for ACAMSD_MSIP_ChannelManager class.

        @param          myMsManager

        @param          cpSystemId

        @param          isMultipleCPSystem

        @param          index

=================================================================== */
	ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId,	bool isMultipleCPSystem, short index);
/*===================================================================

        @brief          Constructor for ACAMSD_MSIP_ChannelManager class.

        @param          myMsManager

        @param          cpSystemId

        @param          isMultipleCPSystem

        @param          msParameters

        @param          sessionInactivityTimeout

=================================================================== */
	ACAMSD_MSIP_ChannelManager (ACAMSD_MS_Manager * myMsManager, unsigned cpSystemId, bool isMultipleCPSystem, const ACAMS_Parameters * msParameters,	int sessionInactivityTimeout);

/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          fxInitialize

        @return         void
=================================================================== */
	void fxInitialize ();

/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMSD_MSIP_ChannelManager ();

/*=====================================================================
	                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          fxFinalize

        @return         void

=================================================================== */
	void fxFinalize ();

/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================

        @brief          addSession

        @param          clientSocket

        @param          clientSockEvent 

        @return         int

=================================================================== */
	int addSession (ACE_HANDLE clientSocket, ACE_Event * clientSockEvent);
/*===================================================================

        @brief          cpSystemId

        @return         _cpSystemId

=================================================================== */
	inline unsigned cpSystemId () { return _cpSystemId; }
/*===================================================================

        @brief          notifyMessageAvailableToClients

        @return         int

=================================================================== */
	int notifyMessageAvailableToClients ();
/*===================================================================

        @brief          workerMain

        @return         int

=================================================================== */
	int workerMain ();
/*===================================================================

        @brief          stopMSIP

        @return         int

=================================================================== */
	int stopMSIP ();

/*=====================================================================
                                PROTECTED DECLARATION SECTION
==================================================================== */
protected:
/*===================================================================

        @brief          threadMain
                           virtual function.

        @param          parameters

        @return         int

=================================================================== */
	virtual int threadMain (void * parameters = 0);

/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          fxActivateIncomingSessions

        @return         int

=================================================================== */
	int fxActivateIncomingSessions ();
/*===================================================================

        @brief          fxValidateSessions

        @return         int

=================================================================== */
	int fxValidateSessions ();
/*===================================================================

        @brief          fxDoWork

        @return         int

=================================================================== */
	int fxDoWork ();
/*===================================================================

        @brief          fxDeleteSession

        @param          index

        @return         void

=================================================================== */
	void fxDeleteSession (unsigned index);
/*===================================================================

        @brief          fxExchange

        @param          sessionIndex

        @return         int

=================================================================== */
	int fxExchange (unsigned sessionIndex);
/*===================================================================

        @brief          fxDeleteActiveSessions

        @return         int

=================================================================== */
	int fxDeleteActiveSessions ();
/*===================================================================

        @brief          fxDeleteIncomingSessions

        @return         int

=================================================================== */
	int fxDeleteIncomingSessions ();

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
	std::queue<MSIPSession> _incomingSessionQueue;
	time_t _lastActivityTime;
	MSIPSession _sessions[MAX_WAIT_OBJECTS];
	int _sessionCount;
	int _sessionInactivityTimeout;
	ACAMSD_MS_Manager * _myMsManager;
	ACE_Recursive_Thread_Mutex _syncAccess;
	int _stop_event_fd;
};

#endif
