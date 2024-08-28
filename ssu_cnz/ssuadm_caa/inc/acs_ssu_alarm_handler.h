
/*! \addtogroup alarm "Alarm Handling"
 *
 *   Raising and Ceasing of Alarms.
 *
 *  @{
 */

//=============================================================================
/**
 *  @file    ACS_SSU_AlarmHandler.h
 *
 *
 *  @version  1.1.0
 *
 *  @author 2010-06-20 by XSIRKUM
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

#ifndef ACS_SSU_ALARMHANDLER_H
#define ACS_SSU_ALARMHANDLER_H

//! ACE Includes
#include <ace/Thread_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <list>

//! Internal Includes
#include "acs_ssu_types.h"

using namespace std;

class ACS_SSU_AlarmHandler
{
public:

   /*!
	* @brief Constructor
	*/
   ACS_SSU_AlarmHandler();

   /*!
    * @brief Destructor
    *
    * Cease all alarms and clear Run only once application list
    */
   ~ACS_SSU_AlarmHandler();

private:
   //! An instance of this class is used to store active alarms in m_listAlarms
   class Alarm
   {
   public:
      Alarm(LPSSU_PERF_DATA pPerfData, double nValue)
      {
         ACE_OS::memcpy(&m_srctPerfData, pPerfData, sizeof(SSU_PERF_DATA));
         m_nValue = nValue;
      };

      LPSSU_PERF_DATA PerfData() const
      {
         return (const LPSSU_PERF_DATA)&m_srctPerfData;
      };

   private:
      SSU_PERF_DATA m_srctPerfData;
      double        m_nValue;
   };

   /*!
    *  Class to hold a performance counter that shall call "Application"only
    *  the first time a alarm is raised
    */

   class RunFirstTime
   {
   public:
      RunFirstTime()
      {
         *Object = ACE_TEXT('\0');
         *Counter = ACE_TEXT('\0');
         *Instance = ACE_TEXT('\0');
         *Severity = ACE_TEXT('\0');
      };

      RunFirstTime(LPSSU_PERF_DATA pPerfData)
      {
         (void)ACE_OS::strncpy(Object, pPerfData->Object, 32);
         (void)ACE_OS::strncpy(Counter, pPerfData->Counter, 32);
         (void)ACE_OS::strncpy(Instance, pPerfData->Instance, 32);
         (void)ACE_OS::strncpy(Severity, pPerfData->Severity, 6);
      };

      BOOL Match(LPSSU_PERF_DATA pPerfData)
      {
         return (ACE_OS::strcmp(pPerfData->Object, Object) == 0 &&
        		 ACE_OS::strcmp(pPerfData->Counter, Counter) == 0 &&
        		 ACE_OS::strcmp(pPerfData->Instance, Instance) == 0 &&
        		 ACE_OS::strcmp(pPerfData->Severity, Severity) == 0);
      };

   private:
      //! Alarm Atrributes
      ACE_TCHAR Object[32];
      ACE_TCHAR Counter[32];
      ACE_TCHAR Instance[32];
      ACE_TCHAR Severity[6];
   };

public:
	pid_t 	outstandingPID;

	//Flag to be set during shutdown
	bool isShutdownTriggered;

	pid_t popen_with_pid(std::string command, int *infp, int *outfp);
   /*!
    * @brief Sends an alarm/event for an performance counter
    * @param pPerfData Performance data
    * @param nValue Current Value
    */
   bool SendPerfAlarm(const LPSSU_PERF_DATA pPerfData, const double nValue);

   /*!
    * @brief Sends an cease for an performance counter
    * @param pPerfData Performance data
    * @param nValue Current value
    */
   bool SendPerfCease(const LPSSU_PERF_DATA pPerfData, const double nValue);


   /*!
    * @brief Sends an alarm for an folder quota
    * @param nEventNumber Event Number
    * @param lpszSeverity Severity of the alaram
    * @param lpszPath Path of the Alarm application
    * @param nLimit Cease Limit Value
    * @param nValue Value
    */
   bool SendFolderQuotaAlarm(const long nEventNumber,
                             const ACE_TCHAR* lpszSeverity,
                             const ACE_TCHAR* lpszPath,
                             const ACE_UINT32 nLimit,
                             const double nValue);
   /*!
    * @brief Ceases an alarm for an folder quota
    * @param nEventNumber Event NUmber
    * @param lpszPath Path of the Alarm application
    * @param nLimit Cease Limit Value
    * @param nValue Value
    */
   bool SendFolderQuotaCease(const ACE_UINT32 nEventNumber,
                             const ACE_TCHAR* lpszPath,
                             const ACE_UINT32 nLimit,
                             const double nValue);

   void ExecuteRsyslogCheck();

private:
   /*!
    * @brief Checks if an event/alarm form an performance counter is already sent
    * @param pPerfData Performance data
    * @return TRUE Alarm already sent
    * @return FALSE Alarm not sent
    */
   BOOL CheckIfSent(LPSSU_PERF_DATA pPerfData);

   /*!
    * @brief Checks if alarm level of pAlarmData >  pPerfData
    * @param pPerfData Performance data 1
    * @param pAlarmData Already existing Alarm data
    * @return
    */
   BOOL CheckIfHigher(LPSSU_PERF_DATA pPerfData, LPSSU_PERF_DATA pAlarmData);

   /*!
    * @brief Check if there exists any alarm for performance counter pPerfData with
    *        higher severity
    * @param pPerfData
    * @return
    */
   BOOL CheckIfHigherSent(LPSSU_PERF_DATA pPerfData);

   /*!
    * @brief Checks if an alarm/event has an application to run when an alarm is raised
    * @param pPerfData Performance data
    * @return TRUE Run application
    * @return FALSE do not run application
    */
   BOOL CheckIfRunApplication(LPSSU_PERF_DATA pPerfData);

   /*!
    * @brief Cease all alarms for a performance counter pPerfData, used for ceasing lower alarm levels
    *
    * @param pPerfData Performance Data
    *
    */
   void CeaseLowerAlarms(LPSSU_PERF_DATA pPerfData);

   void FormatFolderQuotaText(const ACE_TCHAR* lpszPath,
                              const ACE_UINT32 nLimit,
                              double nValue,
                              const ACE_TCHAR* lpszSeverity,
                              ACE_TCHAR* lpszObjectOfReference,
                              ACE_TCHAR* lpszData,
                              ACE_TCHAR* lpszText);

   /*!
    * @brief  Formats ObjectOfReference, ProblemData and ProblemText for a performance
    *         counter alarm/event
    * @param pPerfData Performance data
    * @param nValue Current Value
    * @param lpszSeverity Severity
    * @param lpszObjectOfReference Object Text
    * @param lpszData Free text description to be logged
    * @param lpszText Free text description to be printed
    */
   void FormatPerfText(const LPSSU_PERF_DATA pPerfData,
                       const double nValue,
                       const ACE_TCHAR* lpszSeverity,
                       ACE_TCHAR* lpszObjectOfReference,
                       ACE_TCHAR* lpszData,
                       ACE_TCHAR* lpszText);

private:

   //! Mutex for protecting Alarm list
   ACE_Recursive_Thread_Mutex m_srctMutex;

   std::list<Alarm*>        m_listAlarms;

   std::list<RunFirstTime*> m_listRunFirstTime;

   std::map<std::string,std::string> alarmString;

#ifdef SSU_CUTE_TEST
 friend class Cute_SSU_AlarmHandler;
#endif
};

#endif //ACS_SSU_ALARMHANDLER_H

/*! @} */
