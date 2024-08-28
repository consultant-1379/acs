/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2012
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file mcs_adh_adm.h
 *
 * @brief
 * This is the main class to be run in USA. It is an active object that is
 * started by calling start() and then stopped by calling stop().
 * The thread is run in svc().
 *
 * @author 
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 *
 */

#ifndef ACS_USA_ADM_H
#define ACS_USA_ADM_H

#include <syslog.h>
#include <ace/Task_T.h>
#include <ace/OS.h>
#include <ace/Sig_Handler.h>
#include "acs_usa_types.h"
#include "acs_usa_tratrace.h"

#define TRCE_LOG_DIR "/var/log/acs/tra/logging/"   // hardcode for now. 

// enable trace
#define CATEGORY_ALL    0xffffffff

//Forwards
class usaHAClass;
class ACS_USA_ReactorRunner;
class ACS_USA_ZombieMon;
class ACS_USA_CpuLdAvg;
class ACS_USA_SLA;

//------------------------------------------------------------------------

class ACS_USA_Adm: public ACE_Task<ACE_SYNCH> {

   private:

	usaHAClass* m_haObj;
	ACS_USA_ReactorRunner* m_reactorRunner;
	ACS_USA_ZombieMon* m_zMonObj;
	ACS_USA_CpuLdAvg* m_ldAvgObj;
	ACS_USA_SLA* m_slaObj;
	bool ShutdownDone;
	int m_retryCount;
	int m_timerid;

   public:
	ACS_USA_Adm();
	~ACS_USA_Adm();
	
	ACE_Sig_Handler sig_shutdown_;

	int close(u_long);

	int handle_signal(int signum,siginfo_t *,ucontext_t *);
	
	int handle_close(ACE_HANDLE, ACE_Reactor_Mask mask);
	
	int handle_timeout(const ACE_Time_Value&, const void* );
	
	void finalizeNtf();
	
	void finalizeNtp();
	
	int initializeNtf();

	int initializeNtp();

	int start(usaHAClass*);
	
	int start(int argc, char* argv[]);
	
	void stop();
	
	int svc();

	bool initializeAdhUserObserver();
}; 

#endif /* ACS_USA_ADM_H */

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

