
/*! \addtogroup perfm "Performance Monitoring"
 *
 *  Performance Monitoring involves monitoring following functionality
 *  1. Main Memory consumption
 *  2. Swap Memory Consumption
 *  3. File Descriptor count
 *
 *  This class is base for all counters. Derived class needs to overridethe following
 *  pure virtual functions
 *
 *  1. vUpdatePerformanceCounters() - Fetch the current values of the performance counter
 *  2. bParseCommandOutPut() - Parse the file generated after running the function vUpdatePerformanceCounters
 *  3. virtual void vCheckAndRaiseAlarm() Check and raise alarm based on Parser output
 *
 *  @{
 */


// -*- C++ -*-

//=============================================================================
/**
 *  @file    ACS_SSU_Counter.h
 *
 *  @version  1.1.0
 *
 *  @author 2010-06-11 by XSIRKUM
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


#ifndef _ACS_SSU_COUNTER_H
#define _ACS_SSU_COUNTER_H

#include <list>
#include <string>

//! ACE Includes
#include <ace/Thread_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Event.h>

#include "acs_ssu_types.h"

//class ACE_Event;
class ACS_SSU_AlarmHandler;

/*!
 * @class ACS_SSU_Counter
 *
 * Counter base class which implements basic tasks to be followed to update,parse and raise alarm data.
 * Derived class should override the required functions
 *
 */
class ACS_SSU_Counter
{
public:
   /*!
    * @brief Constructor for Performance Counter
    * @param hEndEvent  Pointer to End Event
    * @param pAlarmHandler Pointer to Alarm Handler
    * @return
    */
   ACS_SSU_Counter(ACE_Event * hEndEvent,
                   ACS_SSU_AlarmHandler* pAlarmHandler);

   virtual ~ACS_SSU_Counter();

public:

   /*!
    * @brief Performance Management Thread Functionality Loop
    * In this loop we update the performance counter values, parse them and raise alarms accordingly
    */
   virtual void Monitor();

   /*!
    * Thread spawn call back function
    * @param pvThis Pointer to Self Counter Object
    * @return Thread Exit status
    */
   static  ACE_THR_FUNC_RETURN MonitorThread(void* pvThis);

   /*!
    * @brief Add new alarm to this counter
    * @param refoPerfData Performance data related to this alarm
    * @return TRUE Alarm Successfully Added
    * @return FALSE Failed to add alarm
    */
   virtual bool bAddAlarm(SSU_PERF_DATA & refoPerfData);

   /*!
    * @brief Return reference to list of Performance data objects
    * @return Reference to Alarm list containing performance data
    */
   virtual const std::list<SSU_PERF_DATA>& PerfData() const;

   /*!
    * Updates the Performance counter value to a file
    */
   virtual bool bUpdatePerformanceCounters()=0;

   /*!
    * Parses File which contains performance data
    * @return TRUE Success
    * @return FALSE Failure
    */
   virtual bool bParseCommandOutPut()=0;

   /*!
    * Raise alarm based on Parsed Performance counter values
    */
   virtual bool bCheckAndRaiseAlarm()=0;

   //! Mutex for protecting Alarm list
   ACE_Recursive_Thread_Mutex m_listMutex;


protected:

   //! Pointer to alarm handler
   ACS_SSU_AlarmHandler*  m_pAlarmHandler;
   //! Pointer to End event , this event is triggered from SVC_Loader
   ACE_Event              *m_hEndEvent;
   //! List of Performance Alarm settings
   std::list<SSU_PERF_DATA>  m_listPerfMonMonitor;

};

#endif

/*! @} */
