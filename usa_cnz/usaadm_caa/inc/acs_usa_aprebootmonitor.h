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
 * @file acs_usa_aprebootmonitor.h
 *
 * @brief
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

#ifndef ACS_USA_APREBOOTMON_H
#define ACS_USA_APREBOOTMON_H

#include <syslog.h>
#include <ace/Task_T.h>
#include <ace/OS.h>

#include "acs_usa_types.h" 
#include "acs_usa_time.h"
#include "acs_usa_tratrace.h"
#include "acs_apgcc_omhandler.h"

class Global;
class ACS_USA_Adm;
//------------------------------------------------------------------------

class ACS_USA_APRebootMon: public ACE_Task<ACE_SYNCH>{

	private:
	     	Global *m_global_instance;	
		long m_timerid;
		unsigned int rebootMonitorTimeInterval;
		ACS_USA_Adm *m_admObj;
		void queryRunLevel();
	public:
		ACS_USA_APRebootMon();
		virtual ~ACS_USA_APRebootMon();
		int svc();
		int close(u_long);
		int handle_timeout(const ACE_Time_Value& tv, const void*);
		int open(ACS_USA_Adm *p_admObj);
		int close();
}; 

#endif /* ACS_USA_APREBOOTMON_H */

