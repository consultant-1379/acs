/*=================================================================== */
/**
        @file           acs_aca_msd_ms_manager.h

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACAMSD_MS_Manager class.

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

#ifndef __ACAMSD_MS_MANAGER_H__
#define __ACAMSD_MS_MANAGER_H__

/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>
#include <map>
#include <list>

#include "ACS_DSD_Server.h"

#include "acs_aca_common.h"
#include "acs_aca_msd_mtap_channel_mgr.h"
#include "acs_aca_msd_msip_channel_mgr.h"


/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACAMSD_MS_Manager {
/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                TYPEDEF DECLARATION SECTION
==================================================================== */
	typedef std::map<unsigned int, ACAMSD_MTAP_ChannelManager *> mtapChannelMap_t;
	typedef std::map<unsigned int, ACAMSD_MSIP_ChannelManager *> msipChannelMap_t;

public:
/*====================================================================
                                CONSTANT DECLARATION SECTION
==================================================================== */
	static const unsigned long START_MTAP_RETRY_NUM        = 20;
	static const ACE_INT32 START_MTAP_TIMEOUT              = 15000; //15 seconds
	static const ACE_INT32 LISTENING_WAIT_TIMEOUT          = 60; //1 minutes   //INFINITE;
	static const int ACCEPT_MTAP_CONN_MAX_NEW_RETRIES      = 5;
	static const int INIT_DSD_SESSION_MAX_ACCEPT_RETRIES   = 20;
	static const unsigned long STOP_MTAP_CHANNEL_RETRY_NUM = 20;
	static const ACE_INT32 STOP_MTAP_CHANNEL_TIMEOUT       = 25; //milliseconds
	static const unsigned SINGLE_CP_ID                     = ~0U;
	static const unsigned long START_MSIP_RETRY_NUM        = 20;
	static const ACE_INT32 START_MSIP_TIMEOUT              = 15000; //15 seconds
	static const unsigned long STOP_MSIP_CHANNEL_RETRY_NUM = 20;
	static const ACE_INT32 STOP_MSIP_CHANNEL_TIMEOUT       = 25; //milliseconds
	

public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for ACAMSD_MS_Manager class.

        @param          msParameters

        @param          domainName

=================================================================== */
	ACAMSD_MS_Manager (const ACAMS_Parameters * msParameters,	const char * domainName = "MTAP");
/*===================================================================

        @brief          Constructor for ACAMSD_MS_Manager class.

        @param          msParameters

        @param          domainName

=================================================================== */
	ACAMSD_MS_Manager (short index, const char * domainName = "MTAP");
/*===================================================================

        @brief          Constructor for ACAMSD_MS_Manager class.

        @param          msParameters

        @param          event 

        @param          domainName

=================================================================== */
	ACAMSD_MS_Manager (const ACAMS_Parameters * msParameters,	ACE_Event * event, const char * domainName = "MTAP");
/*===================================================================

        @brief          Constructor for ACAMSD_MS_Manager class.

        @param          msParameters

        @param          cpIdList

        @param          domainName

=================================================================== */
	ACAMSD_MS_Manager (const ACAMS_Parameters * msParameters, std::vector<unsigned> cpIdList, const char * domainName = "MTAP");
/*===================================================================

        @brief          Constructor for ACAMSD_MS_Manager class.

        @param          msParameters

        @param          cpIdList

        @param          domainName

=================================================================== */
	ACAMSD_MS_Manager(std::vector<unsigned> cpIdList, short index, const char * domainName = "MTAP");

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
	virtual ~ACAMSD_MS_Manager ();

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

        @brief          msName

        @return         char 
=================================================================== */
	const char * msName ();
/*===================================================================

        @brief          messageStoreParameters

        @return         ACAMS_Parameters 
=================================================================== */
	const ACAMS_Parameters * messageStoreParameters () const;
/*===================================================================

        @brief          stopMS

        @return         int

=================================================================== */
	int stopMS ();
/*===================================================================

        @brief          getCPList

        @return         unsigned short

=================================================================== */
	std::list<unsigned short> getCPList ();
/*===================================================================

        @brief          getMTAPChannelMgrByCP

        @param          cpid_

=================================================================== */
	const ACAMSD_MTAP_ChannelManager * getMTAPChannelMgrByCP (unsigned int cpid_);
/*===================================================================

        @brief          messageAvailableFromMTAP

        @param          cpSystemId 

        @return         int

=================================================================== */
	int messageAvailableFromMTAP (unsigned cpSystemId);
/*===================================================================

        @brief          deleteDataFile

        @param          cpSystemId

        @param          fileName

        @return         int

=================================================================== */
	int deleteDataFile (unsigned cpSystemId, std::string fileName);
/*===================================================================

        @brief          getNextMsgNumber

        @param          cpSystemId

        @param          msgNumber

        @return         int

=================================================================== */
	int getNextMsgNumber (unsigned cpSystemId, ULONGLONG & msgNumber); // Get next Message Number to use
/*===================================================================

        @brief          setCPConnectionNotificationEvent

        @return         bool

=================================================================== */
	bool setCPConnectionNotificationEvent ();
/*===================================================================

        @brief          workerMain

        @return         int

=================================================================== */
	int workerMain ();
/*===================================================================

	       @brief          notify_cptable_change

	       @param          cp_id

	       @param          op_type

	       @return         void

=================================================================== */
	void notify_cptable_change (unsigned cp_id, int op_type);


#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
/*===================================================================

		   @brief          notify_os_caching_status

		   @param          caching_status (true --> OS caching enabled; false --> OS caching disabled)

		   @return         void

=================================================================== */
	int notify_os_caching_status(bool caching_status);
#endif


/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          fxStartMTAP

        @param          retryNum

        @param          milliseconds   

        @return         int

=================================================================== */
	int fxStartMTAP (unsigned long retryNum = START_MTAP_RETRY_NUM, ACE_INT32 milliseconds = START_MTAP_TIMEOUT);
/*===================================================================

        @brief          fxStopMTAP

        @return         int

=================================================================== */
	int fxStopMTAP ();
/*===================================================================

        @brief          fxStartMSIP

        @param          retryNum

        @param          milliseconds

        @return         int

=================================================================== */
	int fxStartMSIP (unsigned long retryNum = START_MSIP_RETRY_NUM, ACE_INT32 milliseconds = START_MSIP_TIMEOUT);
/*===================================================================

        @brief          fxStopMSIP

        @return         int

=================================================================== */
	int fxStopMSIP ();
/*===================================================================

        @brief          fxWorking

        @return         int

=================================================================== */
	int fxWorking ();
/*===================================================================

        @brief          fxTryAcceptMTAPConnection

        @return         int

=================================================================== */
	int fxTryAcceptMTAPConnection ();
/*===================================================================

        @brief          fxTryAcceptMSIPConnection

        @return         int

=================================================================== */
	int fxTryAcceptMSIPConnection ();
/*===================================================================

        @brief          fxInitializeDSDSession

        @param          pSession

        @return         int

=================================================================== */
	int fxInitializeDSDSession (ACS_DSD_Session * pSession);
/*===================================================================

        @brief          fxStopMTAPChannels

        @param          retryNum

        @param          milliseconds

        @return         int

=================================================================== */
	int fxStopMTAPChannels (
			unsigned long retryNum = STOP_MTAP_CHANNEL_RETRY_NUM,
			ACE_INT32 milliseconds = STOP_MTAP_CHANNEL_TIMEOUT);
/*===================================================================

        @brief          fxDeleteMTAPChannels

        @return         int

=================================================================== */
	int fxDeleteMTAPChannels ();
/*===================================================================

        @brief          fxCheckMTAPChannels

        @return         int

=================================================================== */
	int fxCheckMTAPChannels ();
/*===================================================================

        @brief          fxStopMSIPChannels

        @param          retryNum

        @param          milliseconds

        @return         int

=================================================================== */
	int fxStopMSIPChannels (
			unsigned long retryNum = STOP_MSIP_CHANNEL_RETRY_NUM,
			ACE_INT32 milliseconds = STOP_MSIP_CHANNEL_TIMEOUT);
/*===================================================================

        @brief          fxDeleteMSIPChannels

        @return         int

=================================================================== */
	int fxDeleteMSIPChannels ();
/*===================================================================

        @brief          fxCheckMSIPChannels

        @return         int

=================================================================== */
	int fxCheckMSIPChannels ();
/*===================================================================

        @brief          fxGetMTAPChannelManager

        @param          cpSystemId

        @param          mtapCM

        @return         int

=================================================================== */
	int fxGetMTAPChannelManager (unsigned cpSystemId, ACAMSD_MTAP_ChannelManager * & mtapCM);
/*===================================================================

        @brief          fxGetMSIPChannelManager

        @param          cpSystemId

        @param          msipCM

        @return         int

=================================================================== */
	int fxGetMSIPChannelManager (unsigned cpSystemId, ACAMSD_MSIP_ChannelManager * & msipCM);
/*===================================================================

        @brief          fxCreateMTAPChannelManager

        @param          systemId

        @param          cm

        @return         int

=================================================================== */
	int fxCreateMTAPChannelManager (unsigned systemId, ACAMSD_MTAP_ChannelManager * & cm);
/*===================================================================

        @brief          fxCreateMSIPChannelManager

        @param          systemId

        @param          cm 

        @return         int

=================================================================== */
	int fxCreateMSIPChannelManager (unsigned systemId, ACAMSD_MSIP_ChannelManager * & cm);
/*===================================================================

        @brief          fxStartMTAPChannelManager

        @param          cm

        @return         int

=================================================================== */
	int fxStartMTAPChannelManager (ACAMSD_MTAP_ChannelManager * cm);
/*===================================================================

        @brief          fxStartMSIPChannelManager

        @param          cm

        @return         int

=================================================================== */
	int fxStartMSIPChannelManager (ACAMSD_MSIP_ChannelManager * cm);
/*===================================================================

        @brief          fxCreateMTAPChannelManagersFromCPList

        @return         int

=================================================================== */
	int fxCreateMTAPChannelManagersFromCPList ();
/*===================================================================

        @brief          fxCreateMSIPChannelManagersFromCPList

        @return         int 

=================================================================== */
	int fxCreateMSIPChannelManagersFromCPList ();
/*===================================================================

        @brief          fxGetCPSystemIdFromMSIPClient

        @param          clientSock

        @param          cpSystemId

        @return         int

=================================================================== */
	int fxGetCPSystemIdFromMSIPClient (ACE_SOCKET clientSock, unsigned & cpSystemId);
/*===================================================================

        @brief          stop_remove_mtap_channel

        @param          cp_id

        @return         void

=================================================================== */
	void stop_remove_mtap_channel (unsigned cp_id);
/*===================================================================

        @brief          stop_remove_msip_channel

        @param          cp_id

        @return         void

=================================================================== */
	void stop_remove_msip_channel (unsigned cp_id);

public:
/*====================================================================
                                PUBLIC DATA MEMBER DECLARATION SECTION
==================================================================== */
	ACE_thread_t _tid;
	thread_state_t _state;
	const short indextoParam;
	bool _isMSThreadAlive;

private:
/*====================================================================
                                PRIVATE DATA MEMBER DECLARATION SECTION
==================================================================== */
	const ACAMS_Parameters * _msParameters;
	ACS_DSD_Server * thedsdServer; //to delete into finalizer
	std::string _domainName;
	ACE_HANDLE _dsdEvents[4];
	int _dsdNumOfEvents;
	int _acceptMTAPConnNewRetryNum;
	int _initDSDSessionAcceptRetryNum;
	mtapChannelMap_t _mtapChannelMap;
	msipChannelMap_t _msipChannelMap;
	std::vector<unsigned> _cpIdList;
	bool _isMultipleCPSystem;
	int _msipPort;
	ACE_HANDLE _msipListenSocket; //to close into finalizer
	sockaddr_in _msipService;
	ACE_Event * _msipListenEvent; //to close into finalizer
	bool _mtapHasToBeRestarted;
	bool _msipHasToBeRestarted;
	ACE_Recursive_Thread_Mutex _syncAccess;
	ACE_Recursive_Thread_Mutex _MTAP_syncAccess;
	ACE_Recursive_Thread_Mutex _MSIP_syncAccess;
	ACE_Recursive_Thread_Mutex _mtap_msip_shared_sync;
	int _stop_event_fd;
};

#endif
