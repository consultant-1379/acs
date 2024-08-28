/*! \addtogroup folderm "Folder Quota Monitoring"
 *
 *  Each instance of Folder quota object is created for each folder to be monitored.
 *  This instance will run in its own thread context.
 *
 *  It will wait for a message from Main folder quota monitoring thread. This message can be
 *  one of the following
 *  -  Quota Message : To check and raise alarms
 *  - Config Message : Change in configuration to changes quotas
 *  - HangUp Message : Shutdown Message
 *
 *
 *
 *  @{
 */


// -*- C++ -*-

//=============================================================================
/**
 *  @file    ACS_SSU_FolderQuota.h
 *
 *  @version  1.1.0
 *
 *  @author 2010-10-11 by XSIRKUM
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

#ifndef ACS_SSU_FOLDERQUOTA_H
#define ACS_SSU_FOLDERQUOTA_H

#include <ace/Task.h>
#include <ace/Monotonic_Time_Policy.h>
#include <iostream>
#include <ace/Event.h>
#include "acs_ssu_types.h"
#include "acs_ssu_alarm_handler.h"

//class ACE_Event;

#define SSU_MAX_PATH 260
#define SSU_MAX_CONFIG_LENGTH 24 
#define BLK_LEN 255

#define ACS_SSU_DATA_BLOCK_DEVICE "/dev/md0"

enum FolderQuotaCheckStatus
{
	EN_FOLDER_SUCCESS=1,
	EN_FOLDER_FOLDER_NOT_AVAILABLE=2,
	EN_FOLDER_CHECK_FAILURE=3
};

class ACS_SSU_FolderQuota:public ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>
{
public:
   ACS_SSU_FolderQuota(
		                ACS_SSU_AlarmHandler* pAlarmHandler,
                        LPSSU_FOLDERQUOTA_MONITOR lpFolderQuota,
                        ACE_Recursive_Thread_Mutex &m_refsrctQuotaCS
                       );

   ~ACS_SSU_FolderQuota();

public:
   void ChangeConfig(LPSSU_FOLDERQUOTA_MONITOR lpFolderQuota);
   BOOL CheckConfig(LPSSU_FOLDERQUOTA_MONITOR lpFolderQuota,
		               ACE_TCHAR* pData );
   BOOL FolderIsAvailable();
   BOOL GetQuotaValue(ACE_TCHAR* lpszBuffer);
	/** @brief IsActive - Checks whether the functionaling thread is active
	 *
	 *	@remarks Remarks
	 */
   BOOL IsActive();
   /*!
    * @brief IsEqual - check whether the folder-path is valid
    * @param lpszPath - the folder path to be checked
    */
   BOOL IsEqual(const ACE_TCHAR* lpszPath) { return (ACE_OS::strcmp(m_szPath, lpszPath) == 0); }
   /*!
    * @brief Notify - raise signal event that folder quota limit is reached
    */
   void Notify(const double nValue = 0.0);
   /*!
    * @brief svc  - service thread waits for the folder-quota events and does processing
    */
   virtual int svc();
   /*!
    * @brief Stop  - This function cease the alarm on a particular folder
    */
   void Stop(const BOOL bRemoveQuota);
   /*!
    * @brief RemoteStop  - This function cease the alarm on a particular folder
    */
   void RemoteStop(const BOOL bRemoveRemoveQuota);
   /*!
    * @brief vAddSubDirObj  - This function is used to add sub directories
    */
   void vAddSubDirObj( ACS_SSU_FolderQuota * ptrFolderQuotaObj);
   /*!
    * @brief vRemoveSubDirObjs  - This function is used to remove sub directories
    */
   void vRemoveSubDirObjs();
   /*!
    * @brief print  - This function prints the attributes
    * folder path, quota limit , config type , a1,a2 alarm and cease.
    */
   void print();

   // For RT
	/** @brief getFolderLimit - the Folder quota limit
	 *
	 *	@remarks Remarks
	 */
   ACE_UINT64 getFolderLimit();
	/** @brief getFolderCurrentConsumption - Get the Folder Current Consumption
	 *
	 *	@remarks Remarks
	 */
   ACE_UINT64 getFolderCurrentConsumption();
	/** @brief getA1AlarmLevel - Get the A1 Alarm Level
	 *
	 *	@remarks Remarks
	 */
   ACE_UINT32 getA1AlarmLevel();
	/** @brief getA1CeaseLevel  - Get the A1 Cease Level
	 *
	 *	@remarks Remarks
	 */
   ACE_UINT32 getA1CeaseLevel();
	/** @brief getA2AlarmLevel - Get A2 Alarm Level
	 *
	 *	@remarks Remarks
	 */
   ACE_UINT32 getA2AlarmLevel();
	/** @brief getA2CeaseLevel - Get A2 the Cease Level
	 *
	 *	@remarks Remarks
	 */
   ACE_UINT32 getA2CeaseLevel();

public:
   // property methods
   const ACE_TCHAR* get_Path() const { return m_szPath; };
   const ACE_TCHAR* get_symbolicName() const { return m_szFolderSymName; };
   const ACE_TCHAR* get_ConfigType() const { return m_szConfigType; };

public:
   ACE_INT32 MonitorThread( void * lpParameter );
   /*!
    * @brief CalculateFolderSize  - Calculates the folder size
    * @param u64Size - folder size out parameter
    * @return TRUE/FALSE
    */
   bool CalculateFolderSize( ACE_UINT64& u64Size,
			                 BOOL recursive=true);
   /*!
    * @brief enCheckAndRaiseAlarm  - Check the folder quota and raise the alarm
    * @return Folder Quota check status
    */
   FolderQuotaCheckStatus enCheckAndRaiseAlarm();

   BOOL AddFolderQuota( BOOL bResetQuota = false );

private:

   BOOL ModifyFolderQuota();
   BOOL RemoveFolderQuota();

private:
   const ACE_TCHAR* BytesToSizeString(const ACE_UINT64& nSize,
                                   const BOOL bUseDelOnGB = false);
   /*!
    * @brief CalculateFolderQuota  - Calculates the folder quota
    * @param nLimit - limit values of folder quota
    * @param  nCurrentSize - current size of the folder
    *
    */
   double CalculateFolderQuota(ACE_UINT64 & nLimit,
		                          ACE_UINT64& nCurrentSize);
   /*!
    * @brief szGetBlockDevice  - returns the Block Device
    * @return Block Device name
    */
  inline string szGetBlockDevice();

private:
   ACS_SSU_AlarmHandler*  m_pAlarmHandler;
   NotificationType       m_enumNotType;
   ACE_TCHAR              m_szPath[SSU_MAX_PATH];
   ACE_TCHAR              m_szFolderSymName[SSU_MAX_PATH];
   ACE_TCHAR              m_szConfigType[25];
   std::list<ACS_SSU_FolderQuota*> m_listSubDirObjs;
   ACE_Recursive_Thread_Mutex & m_refsrctQuotaCS;

   ACE_UINT64          m_u64EventNumber;
   ACE_UINT64          m_u64LimitQuota;
   ACE_UINT64          m_u64A1AlarmQuota;
   ACE_UINT64          m_u64A1CeaseQuota;
   ACE_UINT64          m_u64A2AlarmQuota;
   ACE_UINT64          m_u64A2CeaseQuota;

   ACE_UINT64          m_u64Limit;
   ACE_UINT64          m_u64CurrentConsumption;
   ACE_UINT32          m_nA1AlarmLevel;
   ACE_UINT32          m_nA1CeaseLevel;
   ACE_UINT32          m_nA2AlarmLevel;
   ACE_UINT32          m_nA2CeaseLevel;


   BOOL                m_bFolderQuotaCreated;
   BOOL                m_bAlarmA1Raised;
   BOOL			       remoteStop;
   BOOL                m_bAlarmA2Raised;
   BOOL                m_IsThreadActive;
   std::string         m_szBlockDevice;

   //! Only defined for CUTE
   #ifdef ACS_SSU_CUTE_TEST
       friend class Cute_SSU_FolderQuota;
   #endif

};

#endif
