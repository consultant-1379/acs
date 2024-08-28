
/*! \addtogroup perfm "Performance Monitoring"
 *
 *  @{
 */

//=============================================================================
/**
 *  @file    ACS_SSU_Memory_Counter.cpp
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

#ifndef _ACS_SSU_MEMORY_COUNTER_H
#define _ACS_SSU_MEMORY_COUNTER_H

//! Standard Includes
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

//! ACE Includes
#include <ace/Thread_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Event.h>
//! Local Includes
#include "acs_ssu_types.h"
#include "acs_ssu_common.h"
#include "acs_ssu_counter.h"

#ifdef ACS_SSU_CUTE_TEST
#include <Cute_SSU_Memory_Counter.h>
#endif

//class ACE_Event;
class ACS_SSU_AlarmHandler;

class ACS_SSU_Memory_Counter:public ACS_SSU_Counter
{
public:
	/*!
	 * @brief Constructor for Memory Counter
	 * @param hEndEvent
	 * @param pAlarmHandler
	 * @return
	 */
	ACS_SSU_Memory_Counter(ACE_Event * hEndEvent,
                     ACS_SSU_AlarmHandler* pAlarmHandler);

   ~ACS_SSU_Memory_Counter();

public:

   /*!
    * @brief Thread execution call back function
    * @param pvThis Pointer to Memory Counter Object
    * @return Thread exit value
    */
   static  ACE_THR_FUNC_RETURN MonitorThread(void* pvThis);

private:
   ACE_UINT64 m_totalMem;
   ACE_UINT64 m_usedMem;

   /*!
    * @brief Fetch the current performance counter values for free and swap memory
    */
   bool bUpdatePerformanceCounters();

   /*!
    * @brief Parse the output from above command
    * @return TRUE Success
    *         False Failure
    */
   bool bParseCommandOutPut();

   /*!
    * @brief Raise the alarm based on above command output
    * @return TRUE If Alarm is raised
    *         FALSE No alarms are raised
    */
   bool bCheckAndRaiseAlarm();
   #ifdef ACS_SSU_CUTE_TEST
   friend class Cute_SSU_Memory_Counter;
   #endif

};

#endif

/*! @} */

