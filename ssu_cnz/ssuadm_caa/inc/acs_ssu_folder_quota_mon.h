/*=================================================================== */
/**
   @file acs_ssu_folder_quota_mon.h

   This module contains the implementation for This Thread reads configuration data from IMM and modifies folder quotas accordingly

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#ifndef _ACS_SSU_FOLDERQUOTAMON_H
#define _ACS_SSU_FOLDERQUOTAMON_H

#include <list>
#include <ace/ACE.h>
#include <ace/Task.h>
#include <ACS_APGCC_Util.H>
#include <acs_apgcc_omhandler.h>
#include <ace/Monotonic_Time_Policy.h>
#include "acs_ssu_types.h"
#include "acs_ssu_alarm_handler.h"
#include "acs_ssu_folder_quota.h"
#include "acs_ssu_common.h"

#define MAX_NUM_OF_PARAMS     64

typedef std::list<ACS_SSU_FolderQuota*>     ACS_SSU_FolderQuotaList;
typedef std::list<SSU_FOLDERQUOTA_MONITOR*> ACS_SSU_QuotasList;
typedef std::list<SSU_FOLDERQUOTA_MONITOR> ACS_SSU_QuotasMasterList;

class ACS_SSU_FolderQuotaMon:public ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>
{
public:
	/** @brief ACS_SSU_FolderQuotaMon constructor
	*
	*	ACS_SSU_FolderQuotaMon Constructor of class
	*   @param poEvent: ACE_Event pointer
	*   @param pAlarmHandler: ACS_SSU_AlarmHandler pointer
	*   @param omHandlerPtr: OmHandler pointer
	*	@remarks Remarks
	*/
	ACS_SSU_FolderQuotaMon(ACE_Event* poEvent, ACS_SSU_AlarmHandler* pAlarmHandler, OmHandler* omHandlerPtr);
	/** @brief ACS_SSU_FolderQuotaMon Destructor
	*
	*	ACS_SSU_FolderQuotaMon Destructor of class
	*
	*	@remarks Remarks
	*/
	~ACS_SSU_FolderQuotaMon();
	void stopFolderQuotaMonThread();
	int svc();
	/*!
	*  Monitor thread content function
	*/
#if 0
	/*!
	* Called by ssuls command to get the current configuration
	* @param bSetAllImmData: if true, it fetches all folder settings and pushes into runtime objects.
	*                        if false, it fetches only current size of the folder and pushes into runtime object.
	* @return Result Code
	*/
	ACE_UINT32 ListFolderQuotaConfig(BOOL bSetAllImmData = false);
#endif
	/*!
	* fetches current settings of given folder
	* @param objName: the distinguished name of the object for which the folder settings requested
	* @param folderObject: Contains the folder settings.
	* @return void
	*/
	void getFolderSettings(const char* objName,SSU_FOLDER_OBJ&  folderObject);
	/*!
	* sets current configuration of folder quota monitor to requested configuration.
	* @param szNewConfigType: the new configuration.
	* @return ACE_INT32
	*/
	ACE_INT32 SetConfigType( const ACE_TCHAR * szNewConfigType );
	/*!
	* Checks the requested configuration is allowed on current system configuration or not.
	* @param szNewConfigType: the new configuration.
	* @return ACE_INT32
	*/
	ACE_INT32 enCheckConfigChange( const ACE_TCHAR * szNewConfigType );
	/*=================================================================== */
	/** @brief   m_FolderLock.
	*/
	/*=================================================================== */
	static ACE_Recursive_Thread_Mutex  m_FolderLock;
	/*=================================================================== */
	/** @brief   m_listFolderQuotas.
	*/
	/*=================================================================== */
	ACS_SSU_FolderQuotaList   m_listFolderQuotas;
	//FileM Folder List
	ACS_SSU_FolderQuotaList   m_listFileMFolderQuotas;

private:
	/*!
	* fetches data disk size.
	* @return int on  Success
	* @return -1 on Failure
	*/
	//int getDataDiskSize(std::string aDiskSize);
        int getDataDiskSize();
	/*!
	* Gives configuration based on aptType, syatemType, apNodeNumber and data disk size
	* @param type: aptType viz MSC,HLR,BSC,...
	* @param system: systemType viz SINGLE CP or MULTI CP
	* @param nodeNum: ap node number viz AP1, AP2, ...
	* @param size: size of the data disk
	* @param boardType : Type of board GEP, GEP2, GEP5.....
	* @return ACE_TCHAR*
	*/
	ACE_TCHAR* getConfigString(ACE_INT32 type, ACE_INT32 system, ACE_INT32 nodeNum,ACE_INT32 size,
				   const ACE_TCHAR* boardType);
	/*!
	* Gives the Board type GEP1, GEP2, GEP5, GEP5-400,...
	* @return ACE_TCHAR* 
	*/
	ACE_TCHAR* getBoardType();
        /*!
        * Reads the content of SSUQuotas file in /opt/AP/ACS/etc , parses the content
        * and creates corresponding folder quota objects
        * @return TRUE Success
        * @return FALSE Failure
        */

	BOOL PrepareFolderQuotas();
	/*!
	*  Prepares and creates necessary quota configuration files
	* @return TRUE Success
	* @return FALSE Failure
	*/
	BOOL SetupFolderQuotaMonitor();
	/*!
	* Delete entries in QuotaList
	* @param QuotaList
	*/
	void EraseQuotaList(ACS_SSU_QuotasList& QuotaList);
	/*!
	* Checks for similar entry(pQuota) entry in QuotaList(QuotaList)
	* @param pQuota   Entry to be checked
	* @param QuotaList Quota List
	* @return TRUE Exists
	* @return FALSE Does not exist
	*/
	BOOL CheckQuotaListItemExist(LPSSU_FOLDERQUOTA_MONITOR pQuota,
			ACS_SSU_QuotasList& QuotaList);
	/*!
	* Reads Current Quota configuration from from IMM database
	* @return Success or Failure
	*/
	BOOL ReadConfigType();
	/*!
	* Parse each entry in SSUQuota file
	* @param hQuotaFile Quotafile
	* @param pQuotaDef
	* @return Parsed content to be stored
	*/
	ACE_INT32 ReadQuota( ifstream * hQuotaFile, SSU_FOLDERQUOTA_MONITOR* pQuotaDef);
	//FileM Change 
	ACE_INT32 ReadFileMQuota( ifstream * hQuotaFile, SSU_FOLDERQUOTA_MONITOR* pQuotaDef);
	/*!
	* Parse contents in quotas file
	* @param QuotaList Quota List object to be filled after parsing
	* @param lpszConfigType Current Configuration
	* @param prepare TRUE Prepares SSU for config even with no quota settings and return TRUE
	* @param prepare FALSE Returns FALSE if quota settings are empty
	* @return
	*/
	BOOL ReadQuotas(ACS_SSU_QuotasList& QuotaList,
			const ACE_TCHAR* lpszConfigType,
			BOOL prepare);
	//FileM Change
	BOOL ReadFileMQuotas(ACS_SSU_QuotasList& QuotaList,
			const ACE_TCHAR* lpszConfigType,
			BOOL prepare);
	/*!
	* Used by Parser to initialize pQuotaDef based on the text parsed
	* @param lpszValue Value of a field in pQuotaDef
	* @param nFieldIdx Filed ID in pQuotaDef
	* @param pQuotaDef Quota Structure whose fields need to be initialized
	* @return
	*/
	BOOL SetQuotaDefValue(const ACE_TCHAR* lpszValue,
			const ACE_INT32 nFieldIdx,
			SSU_FOLDERQUOTA_MONITOR* pQuotaDef);
	//FileM Change
	BOOL SetFileMQuotaDefValue(const ACE_TCHAR* lpszValue,
			const ACE_INT32 nFieldIdx,
			SSU_FOLDERQUOTA_MONITOR* pQuotaDef);
	/*!
	* Checks /etc/fstab directory whether quotas are enabled
	* @return TRUE Enabled
	* @return FALSE Not Enabled
	*/
	BOOL bCheckFolderQuotaEnabled();
	/*!
	* Send the quota message enType to all the Folder Quota threads
	* @param enType Notification to be sent
	*/
	void vSendMessageToQuotaThreads( enum NotificationType enType );
	/*!
	* Build the subdirectory tree for each quota object. Where each subdirectory is a separate quota object
	*/
	void vUpdateSubDirObjs();
	/*=================================================================== */
	/** @brief   m_pAlarmHandler.
	*/
	/*=================================================================== */
	ACS_SSU_AlarmHandler*     m_pAlarmHandler;
	/*=================================================================== */
	/** @brief   m_phEndEvent.
	*/
	/*=================================================================== */
	ACE_Event *m_phEndEvent;
	/*=================================================================== */
	/** @brief   m_srctQuotaCS.
	*/
	/*=================================================================== */
	ACE_Recursive_Thread_Mutex          m_srctQuotaCS;
	/*=================================================================== */
	/** @brief   m_srctThreadCS.
	*/
	/*=================================================================== */
	ACE_Recursive_Thread_Mutex          m_srctThreadCS;
	/*=================================================================== */
	/** @brief   m_szConfigType.
	*/
	/*=================================================================== */
	ACE_TCHAR                 *m_szConfigType;
	/*=================================================================== */
	/** @brief   m_nNumOfParams.
	*/
	/*=================================================================== */
	ACE_UINT16                m_nNumOfParams;
	/*=================================================================== */
	/** @brief   m_szDummyParams.
	*/
	/*=================================================================== */
	ACE_TCHAR                    m_szDummyParams[256];
	/*=================================================================== */
	/** @brief   m_bIsActiveNode.
	*/
	/*=================================================================== */
	BOOL                      m_bIsActiveNode;
	/*=================================================================== */
	/** @brief   m_listAllFolderQuotas.
	*/
	/*=================================================================== */
	ACS_SSU_QuotasMasterList	m_listAllFolderQuotas;
	/*=================================================================== */
	/** @brief   m_bIsPrepared.
	*/
	/*=================================================================== */
	BOOL m_bIsPrepared;
	/*=================================================================== */
	/** @brief   m_bIsThreadPrepared.
	*/
	/*=================================================================== */
	BOOL m_bIsThreadPrepared;
	/*=================================================================== */
	/** @brief   m_omHandler.
	*/
	/*=================================================================== */
	OmHandler*  m_omHandler;
	/*=================================================================== */
	/** @brief   aptType.
	*/
	/*=================================================================== */
	ACE_INT32 aptType;
	/*=================================================================== */
	/** @brief   apNodeNumber.
	*/
	/*=================================================================== */
	ACE_INT32 apNodeNumber;
	/*=================================================================== */
	/** @brief   systemType.
	*/
	/*=================================================================== */
	ACE_INT32 systemType;
	/*=================================================================== */
	/** @brief   dataDiskSize.
	*/
	/*=================================================================== */
	ACE_INT32 dataDiskSize;
	/*=================================================================== */
        /** @brief   m_szBoardType.
        */
        /*=================================================================== */
        ACE_TCHAR                 *m_szBoardType;
	char 		  myGEPVersion[15];


#ifdef ACS_SSU_CUTE_TEST
	friend class Cute_SSU_FolderQuotaMon;
#endif

};

#endif
