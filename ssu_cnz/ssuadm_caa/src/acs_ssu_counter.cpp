
/*! \addtogroup perfm "Performance Monitoring"
 *
 *  @{
 */

//=============================================================================
/**
 *  @file    ACS_SSU_Counter.cpp
 *
 *
 *  @version  1.0.0
 *
 *  @author 2010-08-18 by XSIRKUM
 *
 *  @documentno
 *
 */

/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
==================================================================== */
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Log_Msg.h>
//! Common Class include for Getting Active Node
#include <ACS_APGCC_Util.H>
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_counter.h"
#include "acs_ssu_alarm_handler.h"

//! Not used currently
const ACE_TCHAR* GetPdhError()
{
   static ACE_TCHAR szMsg[256] = ACE_TEXT("");

   return szMsg;
}

ACS_SSU_Counter::ACS_SSU_Counter( ACE_Event * hEndEvent,
                                 ACS_SSU_AlarmHandler* pAlarmHandler)
{
   //! Copy/assign input parameters
   m_hEndEvent = hEndEvent;
   m_pAlarmHandler = pAlarmHandler;
   m_listPerfMonMonitor.clear();

}

ACS_SSU_Counter::~ACS_SSU_Counter()
{
   //! Cease any outstanding alarm before leaving

	for( std::list<SSU_PERF_DATA>::const_iterator it = m_listPerfMonMonitor.begin(); it != m_listPerfMonMonitor.end() ; ++it )
	{
	   m_pAlarmHandler->SendPerfCease( (LPSSU_PERF_DATA)&*it, 0.0);
	}
}

BOOL ACS_SSU_Counter::bAddAlarm( SSU_PERF_DATA & refoPerfData )
{
   //! Lock before modifyig the Alarm List
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_listMutex);

   //! Ensure that the severity class test is in uppercase
   std::string oszSeverity = refoPerfData.Severity;
   ACS_APGCC::toUpper(oszSeverity);
   ACE_OS::strcpy(refoPerfData.Severity, oszSeverity.c_str());

   //!Add Alarm
   m_listPerfMonMonitor.push_back(refoPerfData);

   //! Sort Alarms based on the priority
   m_listPerfMonMonitor.sort(AlarmSortPredicate<SSU_PERF_DATA>());

   return true;
}

//! Returns a reference to the list of SSU_PERF_DATA struct
const std::list<SSU_PERF_DATA>& ACS_SSU_Counter::PerfData() const
{
   return m_listPerfMonMonitor;
}

void ACS_SSU_Counter::Monitor()
{
   INFO("%s","DEBUG Start ACS_SSU_Counter::Monitor Thread");

////! ssu_debug for showoff
//#ifndef SSU_DEBUG
//   ACE_Time_Value oTimeValue(10);
//#else
//   ACE_Time_Value oTimeValue((*m_listPerfMonMonitor.begin()).Interval);
//#endif

   ACE_Time_Value oTimeValue((*m_listPerfMonMonitor.begin()).Interval);

   //! Wait till end event from service loader
   while ( m_hEndEvent->wait(&oTimeValue,0) )
   {
	   //! Lock the Performance Data list to gain exclusive access
	   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_listMutex);
	   int value = oTimeValue.msec()/1000 ;

	   if (value == 120)
	   {
		   INFO("Performance monitor for memory  runs after %d sec", value);
	   }
	   else
	   {
		   INFO("Performance monitor for file handle count runs after %d sec", value);
	   }
	   if( bUpdatePerformanceCounters() )
	   {
		   bCheckAndRaiseAlarm();
	   }
   }

   INFO("%s","ACS_SSU_Counter::Monitor Exiting");

   //! @TODO Check if later needed ACE_OS::sleep(2);
}

ACE_THR_FUNC_RETURN ACS_SSU_Counter::MonitorThread(void * pvThis)
{
   ((ACS_SSU_Counter*)pvThis)->Monitor();
   return 0;
}

/*! @} */
