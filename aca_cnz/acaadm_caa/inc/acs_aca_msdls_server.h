/*=================================================================== */
/**
        @file           acs_aca_msdls_server.h

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the MCS_ALD_AldaAdm class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       10/09/2011     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ACA_MSDLS_SERVER_H
#define ACS_ACA_MSDLS_SERVER_H
/*=====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <stdio.h>

#include <list>

#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_DSD_Server.h"

#include "acs_aca_macros.h"
#include "acs_aca_msdls_worker_thread.h"
#include "acs_aca_msd_ms_manager.h"

/*=====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACS_ACA_MSDLS_Server  {

/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                                CONSTANT DECLARATION SECTION
==================================================================== */
	static const string	MSDLS_SRV_DOMAIN;
	static const string	MSDLS_SRV_NAME;
	static const ACE_INT32 MAX_NO_OF_HANDLES;

/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for ACS_ACA_MSDLS_Server class.
                         create a thread in suspended mode

=================================================================== */
	ACS_ACA_MSDLS_Server (unsigned theStackSize = 0U);
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_MSDLS_Server ();
/*===================================================================

        @brief          workerMain

        @return         int
=================================================================== */
	int workerMain ();
/*===================================================================

        @brief          stopMSDLS

        @return         void
=================================================================== */
	void stopMSDLS ();

/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          garbageCollector

        @return         void  
=================================================================== */
	void garbageCollector ();
/*===================================================================

        @brief          startProtocol

        @return         bool
=================================================================== */
	bool startProtocol ();
/*===================================================================

        @brief          findConnection

        @return         bool
=================================================================== */
	bool findConnection ();

public:
/*====================================================================
                               PUBLIC DATA MEMBER DECLARATION SECTION
==================================================================== */
	thread_state_t _state;
	ACE_HANDLE stopMSThreads[2];

private:
/*====================================================================
                               PRIVATE DATA MEMBER DECLARATION SECTION
==================================================================== */
	ACE_Recursive_Thread_Mutex _worker_list_sync;
	std::list<ACS_ACA_MSDLS_WorkerThread *> worker_list;	//Array of worker threads
	ACS_DSD_Server * pmsdlsServer; //ACS:ACA_MSDLS
	ACE_HANDLE listenH[4]; // handles on listen pipes
	ACE_INT32 noOfHandles; // number of handles for listen pipes
};

#endif
