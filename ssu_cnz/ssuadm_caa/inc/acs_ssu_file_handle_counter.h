/*! \addtogroup perfm "Performance Monitoring"
 *
 *  File Handle Counter thread is spawned by Performance Monitor Thread and is responsible
 *  for monitoring File Handle Count
 *
 *  @{
 */

//=============================================================================
/**
 *  @file    ACS_SSU_FileHandle_Counter.h
 *
 *
 *  @version  1.0.0
 *
 *  @author 2010-08-16 by XSIRKUM
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


#ifndef _ACS_SSU_FILEHANDLE_COUNTER_H
#define _ACS_SSU_FILEHANDLE_COUNTER_H

#include <string>

//! ACE Includes
#include <ace/Thread_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Event.h>
#include "acs_ssu_types.h"
#include "acs_ssu_counter.h"

#ifdef ACS_SSU_CUTE_TEST
#include <Cute_SSU_FileHandle_Counter.h>
#endif

//class ACE_Event;
class ACS_SSU_AlarmHandler;

#define ACS_SSU_FILE_NR_MAX_FIELDS 3

//! Fields in /proc/sts/fs/file-nr
#define FILE_NR_ALLOCATED 0
#define FILE_NR_ALLOCATED_BUT_UNUSED 1
#define FILE_NR_TOTAL 2


class ACS_SSU_FileHandle_Counter:public ACS_SSU_Counter
{
public:
	ACS_SSU_FileHandle_Counter(ACE_Event * hEndEvent,
                     ACS_SSU_AlarmHandler* pAlarmHandler);

   ~ACS_SSU_FileHandle_Counter();

public:

   static  ACE_THR_FUNC_RETURN MonitorThread(void* pvThis);

   //! Properties
   const std::list<SSU_PERF_DATA>& PerfData() const;

private:
   /*!
    * @brief bUpdatePerformanceCounters - Update the count of file handlers  (used , unused and total)
    */
   bool bUpdatePerformanceCounters( );
   bool bParseCommandOutPut();
   /*!
    * @brief bCheckAndRaiseAlarm - Checks the file handle count and
    * raise the alarm if not in the predefined range
    */
   bool bCheckAndRaiseAlarm();
   ACE_UINT64 m_lsu64FileHandleCount[ACS_SSU_FILE_NR_MAX_FIELDS];

   #ifdef ACS_SSU_CUTE_TEST
   friend class Cute_SSU_FileHandle_Counter;
   #endif
};

#endif
