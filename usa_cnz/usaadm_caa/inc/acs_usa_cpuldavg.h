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
 * @file acs_usa_cpuldavg.h
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

#ifndef ACS_USA_CPULDAVG_H
#define ACS_USA_CPULDAVG_H

#include <syslog.h>
#include <ace/Task_T.h>
#include <ace/OS.h>

#include "acs_usa_types.h"
#include "acs_usa_tratrace.h"
#include "acs_apgcc_omhandler.h"

//Forwards
class Global;

//------------------------------------------------------------------------

class ACS_USA_CpuLdAvg: public ACE_Task<ACE_SYNCH>{

	private:

		enum {
			NO_ALARM = 0,
			MINOR_ALARM = 1,
			MAJOR_ALARM = 2,
			CRITICAL_ALARM = 3
		};
//		typedef std::string String;
                void monitorCpuLdAvg();
		ACS_USA_ReturnType setPerameter(String paramName, int value);
		int initialize();
		void reportProblem() const;
		int getCPUCount();
                Global *m_global_instance;
                long m_timerid;
		int cpuLoadAvgMonitorTimeInterval;
		double minorThreshold;
		double majorThreshold;
		double criticalThreshold;
		bool alarmRaised;
		bool ceaseAlarm;
		int lastAlarmSeverity;
		bool disableAlarmFilter;

        public:
                ACS_USA_CpuLdAvg();
                virtual ~ACS_USA_CpuLdAvg();
                int svc();
                int close(u_long);
                int handle_timeout(const ACE_Time_Value& tv, const void*);
                int open();
                int close();

}; 

#endif /* ACS_USA_ADM_H */

