
//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_Main
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************
#ifndef ACS_APSESH_Main_h
#define ACS_APSESH_Main_h 1

#include <string>
#include <iostream>

#include "ACS_APSESH_JtpReturnType.h"
#include "ACS_APSESH_JtpMgr.h"
#include "ACS_APSESH_JtpMgrInterface.h"

#include <ace/Timer_Queue_Adapters.h>
#include <ace/Timer_Heap.h>

static const int MAX_CPS = 64;

//	This class implements the APSESH service.  This service is intended to run
//	as long as the AP is operating normally.  This service receives cluster
//	quorum data from the CP side, and relays it to CS on the AP side.  It
//	performs this function continually as long as the service is running.


typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> ActiveTimer;

class ACS_APSESH_Main : ACE_Event_Handler
{
  public:

    struct CpData 
    {
		// Data Members for Class Attributes
        unsigned char id;
        unsigned char state;
        unsigned char applicationId;
        unsigned char apzSubstate;
        unsigned char stateTransition;
        unsigned char aptSubstate;
        unsigned char blockingInfoLow;
        unsigned char blockingInfoHigh;
        unsigned char cpCapacityB0;
        unsigned char cpCapacityB1;
        unsigned char cpCapacityB2;
        unsigned char cpCapacityB3;
        unsigned char spare[8];
    };
    //	Contains the cluster quorum information for a blade cluster system.  It
    //	provides quorum-wide data, as well as data for each blade in the quorum.
    struct QuorumData
    {
        // Data Members for Class Attributes
        unsigned char trafficIsolatedCpId;
        unsigned char trafficLeaderCpId;
        unsigned char aqr;
        unsigned char profileApzHigh;
        unsigned char profileApzLow;
        unsigned char profileAptHigh;
        unsigned char profileAptLow;
        unsigned char ctCps;
        unsigned char spare[16];
        // Data Members for Associations
        CpData cpData[MAX_CPS];
    };

    ACS_APSESH_Main();

    virtual ~ACS_APSESH_Main();
    //	runApsesh() - Performs some initialization, and then enters a continuous
    //	loop that performs that data relays between the CP side and CA on the AP
    //	side.
    int runApsesh();

    static const char *ACS_APSESH_PROCESS_NAME;
    static const char *ACS_APSESH_DAEMON_NAME;
    static const char *ACS_APSESH_USER_NAME;

    bool keepRunning;

private:

    bool setQuorumDissolveTimer(int timeout);
    bool handleQuorumDissolveTimerExpired();
    bool cancelQuorumDissolveTimer();
    virtual int handle_timeout(const ACE_Time_Value & tv, const void *arg);
    void StartActiveTimer();

      static const short DEF_TRACE_MSG_SIZE;

      bool m_timerActive;

      ACS_APSESH_JtpMgrInterface* m_jtpMgr;

      long m_ActiveTimerId;

      ActiveTimer m_ActiveTimer;

};
#endif
