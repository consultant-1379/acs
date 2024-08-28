/*=================================================================== */
/**
   @file acs_ssu_perf_mon.h

   This module contains the implementation for Memory Counter thread is spawned by
   Performance Monitor Thread and is responsible for monitoring Memory and Swap

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */

#ifndef ACS_SSU_PERFMON_H
#define ACS_SSU_PERFMON_H

#include <list>
#include "acs_ssu_types.h"
#include "acs_ssu_counter.h"
#include "acs_ssu_alarm_handler.h"
#include <ace/Event.h>
//! Forward Declaration
//class ACE_Event;
class ACS_SSU_AlarmHandler;

class ACS_SSU_PerfMon
{
public:
	/*!
	 * @brief Constructor
	 * @param poEvent  Handle to Terminating Event
	 * @param pAlarmHandler Pointer to Alarm Handler
	 * @return
	 */
   ACS_SSU_PerfMon(ACE_Event* poEvent, ACS_SSU_AlarmHandler* pAlarmHandler);

   /*!
    * @brief Performance counter objects are deleted from memory
    */
   ~ACS_SSU_PerfMon();

public:

   /*!
    *  @brief Performance Monitor thread execute function
    */
   void Execute();

private:

   /*!
    * @brief Reads Performance Counter Alarm settings from PHA and spawns counter threads
    * @return TRUE Success
    *         FALSE Failure
    *
    * dwResult = 0 - Success
    * dwResult = 1 - PHA error
    * dwResult = 2 - Memory allocation failure
    * dwResult = 3 - Thread Spawn Failure
    */
   BOOL GetPerfCounterSettings();

   /*!
    * @Returns Performance monitor counter object defined by pszPerfObjectString
    * @param pszPerfObjectString String which defines coutner object to be returned
    * @return Pointer to Counter Object
    *
    */
   ACS_SSU_Counter * poGetPerformanceMonitorObject( const ACE_TCHAR* pszPerfObjectString );

   /*!
    * @brief Reads all PHA params from IMM required for Performance FILE HANDLE Monitor
    * @param None
    * @return true on success
    *         false on failure
    *
    */
   bool readAllPHAValuesForFileHandle();

   /*!
   * @brief Reads all PHA params from IMM required for Performance MEMORY Monitor
   * @param None
   * @return true on success
   *         false on failure
   *
   */
   bool readAllPHAValuesForMemory();

private:

   /*=================================================================== */
   /** @brief   m_mapPerfCounters - Performance Counter Objects are stored here.
   */
   /*=================================================================== */
   std::map<std::string, ACS_SSU_Counter*>  m_mapPerfCounters;
   /*=================================================================== */
   /** @brief   m_pAlarmHandler - Alarm Handler Pointer.
   */
   /*=================================================================== */
   ACS_SSU_AlarmHandler*        m_pAlarmHandler;
   /*=================================================================== */
   /** @brief   m_phEndEvent - Pointer to Termination Event.
   */
   /*=================================================================== */
   ACE_Event *m_phEndEvent;
   /*=================================================================== */
   /** @brief   m_s32GrpId - Group ID of the threads spawned by performance counter.
   */
   /*=================================================================== */
   ACE_INT32 m_s32GrpId;
};

#endif

/*! @} */


