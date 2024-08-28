// -*- C++ -*-

//=============================================================================
/**
 *  @file    ACS_SSU_FolderQuota.cpp
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *
 *  @author 2010-09-25 by XSIRKUM
 *
 *  @documentno
 *
 * @todo Implement RemoteStop
 */
//=============================================================================

#include <ace/ACE.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/quota.h>
#include <mntent.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <sys/mount.h>

#include "acs_ssu_common.h"
//#include "ACS_SSU_PHA.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_path.h"
#include "acs_ssu_types.h"
#include "acs_ssu_folder_quota.h"

#define ONE_KB                0x400
#define ONE_MB                0x100000
#define ONE_GB                0x40000000

using namespace std;

ACS_SSU_FolderQuota::ACS_SSU_FolderQuota( ACS_SSU_AlarmHandler* pAlarmHandler,
                            LPSSU_FOLDERQUOTA_MONITOR lpFolderQuota,
                            ACE_Recursive_Thread_Mutex & refsrctQuotaCS
                            )
: m_refsrctQuotaCS( refsrctQuotaCS ),
  m_IsThreadActive(false)
{
   m_pAlarmHandler = pAlarmHandler;
   m_listSubDirObjs.clear();
   m_enumNotType = ntQuotaEvent;

   ACE_OS::strncpy(m_szPath, lpFolderQuota->Path, SSU_MAX_PATH-1);
   ACE_OS::strncpy(m_szConfigType, lpFolderQuota->ConfigType, SSU_MAX_CONFIG_LENGTH);
   ACE_OS::strncpy(m_szFolderSymName,lpFolderQuota->symName, SSU_MAX_PATH-1);
   m_u64EventNumber = lpFolderQuota->EventNumber;

   m_u64Limit = lpFolderQuota->QuotaLimit;
   m_u64CurrentConsumption = 0;
   m_nA1AlarmLevel = lpFolderQuota->A1AlarmLevel;
   m_nA1CeaseLevel = lpFolderQuota->A1CeaseLevel;
   m_nA2AlarmLevel = lpFolderQuota->A2AlarmLevel;
   m_nA2CeaseLevel = lpFolderQuota->A2CeaseLevel;

   m_u64LimitQuota = (ACE_UINT64)m_u64Limit * (ACE_UINT64)ONE_MB;

   double nQuota = ((100.0 - (double)m_nA1AlarmLevel)  / 100.0);
   m_u64A1AlarmQuota = (ACE_UINT64)((double)m_u64LimitQuota * nQuota);

   nQuota = ((100.0 - (double)m_nA1CeaseLevel) / 100.0);
   m_u64A1CeaseQuota = (ACE_UINT64)((double)m_u64LimitQuota * nQuota);

   nQuota = ((100.0 - (double)m_nA2AlarmLevel) / 100.0);
   m_u64A2AlarmQuota = (ACE_UINT64)((double)m_u64LimitQuota * nQuota);

   nQuota = ((100.0 - (double)m_nA2CeaseLevel) / 100.0);
   m_u64A2CeaseQuota = (ACE_UINT64)((double)m_u64LimitQuota * nQuota);

   //! Flag to tell if the folder quota has been defined in the RSM
   m_bFolderQuotaCreated = false;

   //! Flag to set when a alarm has been raised
   m_bAlarmA1Raised = m_bAlarmA2Raised = false;

   remoteStop = false;

   m_szBlockDevice.clear();
}

//! Destructor
ACS_SSU_FolderQuota::~ACS_SSU_FolderQuota()
{
}

//! Returns the Block device associated with the current folder
string ACS_SSU_FolderQuota::szGetBlockDevice()
{

	if( m_szBlockDevice.empty() )
	{

		FILE *fp = NULL;
		struct mntent *mnt;

		if( (fp = setmntent(MOUNTED, "r")) != NULL )
		{
			while ((mnt = getmntent(fp)) != NULL)
			{

				if (hasmntopt(mnt,SSU_QUOTASTRING) && ( ACE_OS::strcmp(SSU_ACS_DATAPATH,mnt->mnt_dir) == 0) )
				{
					m_szBlockDevice = mnt->mnt_fsname;
					break;
				}
			}
		}

		if( m_szBlockDevice.empty() )
		{
			ERROR(" Block device for FileSystem:%s not found in /etc/mtab",SSU_ACS_DATAPATH);
			m_szBlockDevice = ACS_SSU_DATA_BLOCK_DEVICE;
		}
	}

	return m_szBlockDevice;
}

BOOL ACS_SSU_FolderQuota::AddFolderQuota( BOOL bResetQuota )
{
   ACE_stat fileStat;
   BOOL bSuccess = true;

   if( ACE_OS::stat(m_szPath,&fileStat) == 0 )
   {
      struct dqblk dq;

      if ( quotactl(QCMD(Q_GETQUOTA, GRPQUOTA), szGetBlockDevice().c_str(),
    		  fileStat.st_gid, (caddr_t)&dq) )
      {
    	 DEBUG(" quotactl Failed %s szBlockDevice : %s", m_szPath,szGetBlockDevice().c_str());
		 bSuccess = false;
      }

	  if( bSuccess == true )
	  {
		 if( bResetQuota == true )
		 {
	        dq.dqb_bsoftlimit = 0;
			dq.dqb_bhardlimit = 0;
		 }
		 else
		 {
            dq.dqb_bsoftlimit = m_u64A2AlarmQuota/BLOCK_SIZE;
			//dq.dqb_bhardlimit = m_u64A1AlarmQuota/BLOCK_SIZE;
			dq.dqb_bhardlimit = m_u64LimitQuota/BLOCK_SIZE;	//TR HU60639
			
		 }

		 INFO(" Set Folder Quotas for %s SoliLimit:%d HardLimit:%d GID:%d ",m_szPath,(ACE_INT32)dq.dqb_bsoftlimit,
				 (ACE_INT32)dq.dqb_bhardlimit,(ACE_INT32)fileStat.st_gid);

		 if( quotactl(QCMD(Q_SETQUOTA, GRPQUOTA), szGetBlockDevice().c_str(),
				 fileStat.st_gid, (caddr_t) &dq))
		 {
			 DEBUG("%s For block device:%s"," AddFolderQuota: SetQuota",ACE_OS::strerror(errno),szGetBlockDevice().c_str());
			 bSuccess = false;
		  }
	   }
   }
   else
   {
	   bSuccess = false;
   }

   return bSuccess;
}

//! Format an 64 bit int value to a string of the format "100MB", "10GB" or "10.23GB"
const ACE_TCHAR* ACS_SSU_FolderQuota::BytesToSizeString(const ACE_UINT64 & nSize,
                                                     const BOOL bUseDelOnGB )
{
   static ACE_TCHAR szSize[16] = ACE_TEXT("");

   DOUBLE dMbytes = ((DOUBLE)nSize / (DOUBLE)ONE_MB);

   if ( dMbytes > 1024.0 )
   {
      if (bUseDelOnGB)
         ACE_OS::sprintf(szSize, ACE_TEXT("%.02fGB"), (dMbytes / 1024.0));
      else
         ACE_OS::sprintf(szSize, ACE_TEXT("%dGB"), (ACE_INT32)(dMbytes / 1024.0));
   }
   else
      ACE_OS::sprintf(szSize, ACE_TEXT("%dMB"), (ACE_INT32)dMbytes);


   return szSize;
}

//! Calculate folder quota as from the the quota limit (expressed in bytes) and
//! the current folder size. Return the result in percentage
DOUBLE ACS_SSU_FolderQuota::CalculateFolderQuota(ACE_UINT64& u64Limit,
		ACE_UINT64& u64CurrentSize)
{
   DEBUG("Current size value: %ld bytes\n",u64CurrentSize);
   DEBUG("Limit value: %ld bytes\n",u64Limit);
   DOUBLE dPart = (DOUBLE)((DOUBLE)u64CurrentSize/ (DOUBLE)u64Limit);
   DEBUG("dPart value: %f bytes\n",dPart);

   DOUBLE dValue = (DOUBLE)100.0 - (dPart * (DOUBLE)100.0);
   DEBUG("dValue value: %f bytes\n",dValue);
   return (dValue < 0.0 ? 0.0 : dValue);
}

bool ACS_SSU_FolderQuota::CalculateFolderSize( ACE_UINT64& u64Size,
												BOOL recursive )
{
	struct dqblk dq;
    ACE_stat fileStat;
	BOOL bSuccess = true;

   if( ACE_OS::stat(m_szPath,&fileStat) == 0 )
   {
     if (quotactl(QCMD(Q_GETQUOTA, GRPQUOTA), szGetBlockDevice().c_str(),
    		  fileStat.st_gid, (caddr_t) & dq))
     {
        ERROR("%p ","quotactl Q_GETQUOTA:");
        return false;
     }
     else
     {
         //INFO("Size of folder %s is %ld",m_szPath,dq.dqb_curspace);
     }

     u64Size = dq.dqb_curspace;

     std::list<ACS_SSU_FolderQuota*>::iterator it = m_listSubDirObjs.begin();

     //! If recursive get current quota consumption of subfolders
     if( recursive == true )
     {
        ACE_UINT64 u64SubFolderSize = 0;
        for(;it!= m_listSubDirObjs.end(); ++it)
        {
    	   u64SubFolderSize = 0;
    	   (*it)->CalculateFolderSize(u64SubFolderSize);
    	   u64Size+=u64SubFolderSize;
        }
     }

  }
  else
  {
	  bSuccess = false;
  }

  return bSuccess;
}

//! Configuration type has changed. Set the new limits, alarm limits, etc.
void ACS_SSU_FolderQuota::ChangeConfig(LPSSU_FOLDERQUOTA_MONITOR lpFolderQuota)
{

   if (m_bAlarmA1Raised)
   {
      m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA1CeaseLevel, 0.0);
      m_bAlarmA1Raised = false;
   }

   if (m_bAlarmA2Raised)
   {
      m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA2CeaseLevel, 0.0);
      m_bAlarmA2Raised = false;
   }

   (void)ACE_OS::strcpy(m_szConfigType, lpFolderQuota->ConfigType);
   m_u64EventNumber = lpFolderQuota->EventNumber;

   m_u64Limit = lpFolderQuota->QuotaLimit;
   m_nA1AlarmLevel = lpFolderQuota->A1AlarmLevel;
   m_nA1CeaseLevel = lpFolderQuota->A1CeaseLevel;
   m_nA2AlarmLevel = lpFolderQuota->A2AlarmLevel;
   m_nA2CeaseLevel = lpFolderQuota->A2CeaseLevel;

   if (m_u64Limit < 1000)
      m_u64LimitQuota = (ACE_UINT64)m_u64Limit * (ACE_UINT64)ONE_MB;
   else
      m_u64LimitQuota = (ACE_UINT64)(((DOUBLE)m_u64Limit / 1000.0) * (DOUBLE)ONE_GB);

   DOUBLE nQuota = ((100.0 - (DOUBLE)m_nA1AlarmLevel)  / 100.0);
   m_u64A1AlarmQuota = (ACE_UINT64)((DOUBLE)m_u64LimitQuota * nQuota);

   nQuota = ((100.0 - (DOUBLE)m_nA1CeaseLevel) / 100.0);
   m_u64A1CeaseQuota = (ACE_UINT64)((DOUBLE)m_u64LimitQuota * nQuota);

   nQuota = ((100.0 - (DOUBLE)m_nA2AlarmLevel) / 100.0);
   m_u64A2AlarmQuota = (ACE_UINT64)((DOUBLE)m_u64LimitQuota * nQuota);

   nQuota = ((100.0 - (DOUBLE)m_nA2CeaseLevel) / 100.0);
   m_u64A2CeaseQuota = (ACE_UINT64)((DOUBLE)m_u64LimitQuota * nQuota);

   /*cout << "Path: " << m_szPath
		   << "\nNew limit: " << m_u64LimitQuota
		   << " bytes\nNew A1 alarm level: " << m_nA1AlarmLevel << "%, " << m_u64A1AlarmQuota
		   << " bytes\nNew A1 cease level: " << m_nA1CeaseLevel << "%, " << m_u64A1CeaseQuota
		   << " bytes\nNew A2 alarm level: " << m_nA2AlarmLevel << "%, " << m_u64A2AlarmQuota
		   << " bytes\nNew A2 cease level: " << m_nA2CeaseLevel << "%, " << m_u64A2CeaseQuota
		   << " bytes\n" << endl; */

   DEBUG("Path: %s \nNew limit: %ld bytes\n",m_szPath,m_u64LimitQuota);
   DEBUG("New A1 alarm level: %d %, %ld  bytes\n",m_nA1AlarmLevel,m_u64A1AlarmQuota);
   DEBUG("New A1 cease level: %d %, %ld  bytes\n",m_nA1CeaseLevel,m_u64A1CeaseQuota);
   DEBUG("New A2 alarm level: %d %, %ld  bytes\n",m_nA2AlarmLevel,m_u64A2AlarmQuota);
   DEBUG("New A2 cease level: %d %, %ld  bytes\n",m_nA2CeaseLevel,m_u64A2CeaseQuota);


}

//! Check if configuration type can changed. Check if the new limits, alarm
//! limits, etc. dont exceed the current values
BOOL ACS_SSU_FolderQuota::CheckConfig(LPSSU_FOLDERQUOTA_MONITOR lpFolderQuota,
                                      ACE_TCHAR* pData )
{
   ACE_UINT64 u64NewQuota;

   u64NewQuota = (ACE_UINT64)lpFolderQuota->QuotaLimit * (ACE_UINT64)ONE_MB;

   ACE_UINT64 u64FolderSize=0;
   CalculateFolderSize(u64FolderSize);

   if ( u64FolderSize > u64NewQuota )
   {
      ACE_OS::snprintf(pData,SSU_CHANGE_CONFIG_STATUS_TEXT_LENGTH-1,
                ACE_TEXT("The space currently allocated by folder %s prevents the\nchange of folder quota configuration.\nCurrent folder size: %s\nNew space allocation limit: %s"),
                m_szPath,
                BytesToSizeString(u64FolderSize, true),
                BytesToSizeString(lpFolderQuota->QuotaLimit));

   }

   return (u64FolderSize < u64NewQuota);
}

//! Check if the folder to supervise is available
BOOL ACS_SSU_FolderQuota::FolderIsAvailable()
{
   return (ACS_SSU_Common::CheckFile(m_szPath) == SSU_DIRECTORY);
}
void ACS_SSU_FolderQuota::print()
{

	INFO("Path : %s ",get_Path());
	INFO("Configuration type : %s",get_ConfigType());
	INFO("Folder Limit %ld: ",getFolderLimit());
	INFO("A1 Alarm : %d ",getA1AlarmLevel());
	INFO("A2 Alarm :%d ",getA2AlarmLevel());
	INFO("A1 Cease :%d ",getA1CeaseLevel());
	INFO("A2 Cease : %d",getA2CeaseLevel());

}

//! Get all values for the folde quota: path, limit, current quota, alarm
//! limits (in %), etc.
BOOL ACS_SSU_FolderQuota::GetQuotaValue(ACE_TCHAR* lpszBuffer)
{
   ACE_UINT64 nFolderSize;
   nFolderSize = 0;

   //! Calculate the size of the sum of all files in the directory tree
   CalculateFolderSize( nFolderSize);

   ACE_INT32 nLen = ACE_OS::sprintf(lpszBuffer, ACE_TEXT("%-30s%s\n"), ACE_TEXT("Folder name:"), m_szPath);

   //EOLOARV, TR HJ77198 and HJ81998, Changed to dividing by 1000.0 instead of 1000
   nLen += ACE_OS::sprintf(lpszBuffer+nLen,
							ACE_TEXT("%-30s%0.2f%s\n"),
							ACE_TEXT("Quota limit:"),
							(m_u64Limit >= 1000 ? (m_u64Limit/1000.0) : (DOUBLE) m_u64Limit),
							(ACE_TCHAR*)(m_u64Limit >= 1000 ? ACE_TEXT("GB") : ACE_TEXT("MB")));

   DOUBLE nSize;
   ACE_INT32 k;
   ACE_UINT64 nPerc = (ACE_UINT64)(((DOUBLE)nFolderSize/(DOUBLE)m_u64LimitQuota)*1000.0) ;
   k= nPerc%10;
   if(k>=5)nPerc=(nPerc/10)+1;
   else nPerc=nPerc/10;

   m_u64CurrentConsumption = nFolderSize;

   if (nFolderSize < (ACE_UINT64)ONE_MB)
   {
	  //EOLOARV, TR HJ77198 showing folder size with 2 decimals
	  nSize = ((DOUBLE)nFolderSize / (DOUBLE)ONE_KB);
      nLen += ACE_OS::sprintf(lpszBuffer+nLen,
                        ACE_TEXT("%-30s%0.2f%s (%lu%% of quota limit)\n"),
                        ACE_TEXT("Current folder size:"),
                        nSize,
                        (ACE_TCHAR*)ACE_TEXT("KB"),
                        nPerc);
   }
   else if (nFolderSize < (ACE_UINT64)ONE_GB)
   {
	  //EOLOARV, TR HJ77198 showing folder size with 2 decimals
     
	  nSize = ((DOUBLE)nFolderSize / (DOUBLE)ONE_MB);
		
	 	    	  
      nLen += ACE_OS::sprintf(lpszBuffer+nLen,
                        ACE_TEXT("%-30s%0.2f%s (%lu%% of quota limit)\n"),
                        ACE_TEXT("Current folder size:"),
                        nSize,
                        (ACE_TCHAR*)ACE_TEXT("MB"),
                        nPerc);
   }
   else
   {
	  //EOLOARV, TR HJ77198 showing folder size with 2 decimals

      nSize = ((DOUBLE)nFolderSize / (DOUBLE)ONE_GB);

    
      nLen += ACE_OS::sprintf(lpszBuffer+nLen,
                        ACE_TEXT("%-30s%0.2f%s (%lu%% of quota limit)\n"),
                        ACE_TEXT("Current folder size:"),
                        nSize,
                        (ACE_TCHAR*)ACE_TEXT("GB"),
                        nPerc);
   }



   nLen += ACE_OS::sprintf(lpszBuffer+nLen,
                     ACE_TEXT("%-30s%d%% free space\n"),
                     ACE_TEXT("A2 alarm level:"),
                     m_nA2AlarmLevel);

   nLen += ACE_OS::sprintf(lpszBuffer+nLen,
                     ACE_TEXT("%-30s%d%% free space\n"),
                     ACE_TEXT("A2 cease level:"),
                     m_nA2CeaseLevel);

   nLen += ACE_OS::sprintf(lpszBuffer+nLen,
                     ACE_TEXT("%-30s%d%% free space\n"),
                     ACE_TEXT("A1 alarm level:"),
                     m_nA1AlarmLevel);

   nLen += ACE_OS::sprintf(lpszBuffer+nLen,
                     ACE_TEXT("%-30s%d%% free space"),
                     ACE_TEXT("A1 cease level:"),
                     m_nA1CeaseLevel);

   return true;
}

//============================================================================
// IsActive
// Check if supervision has started of this folder
//============================================================================
BOOL ACS_SSU_FolderQuota::IsActive()
{
   return m_IsThreadActive;
}

BOOL ACS_SSU_FolderQuota::ModifyFolderQuota()
{
   INFO("Modifying Folder Quota for %s",m_szPath);

   BOOL bSuccess = AddFolderQuota();

   if( bSuccess == false )
   {
      //! Failed to RUN command
      const ACE_TCHAR *szData = "\n Failed to run command Quotactl to modify folder quotas";

      ERROR("%s",szData);

      //! Report error
      (void)ACS_SSU_AEH::ReportEvent(1003,
                         ACE_TEXT("EVENT"),
                         CAUSE_SYSTEM_ANALYSIS,
                         PROCESS_NAME,
                         szData,
                         ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));
   }

   return bSuccess;
}

//============================================================================
// Folder quota Monitor thread
//============================================================================
int ACS_SSU_FolderQuota::svc()
{
   BOOL bContinue = true;
   FolderQuotaCheckStatus enResult = EN_FOLDER_SUCCESS;

   m_IsThreadActive = true;

   {
	   ACE_Guard<ACE_Recursive_Thread_Mutex> guard2(this->m_refsrctQuotaCS);

	   //! Add quota
	   if(!AddFolderQuota())
	   {
		   Stop(false);
		   return -1;
	   }
   }

   while ( bContinue )
   {
	   ACE_Message_Block *mb = 0;

	   if( this->getq(mb) == -1 )
	   {
		   ACE_ERROR_BREAK((LM_ERROR, ACE_TEXT("%p\n"),ACE_TEXT("getq")));
		   DEBUG(" FolderQuota :%s Loop getq(mb)==-1", m_szPath);
	   }

	   if( mb->msg_type() == ACE_Message_Block::MB_HANGUP )
	   {
		   DEBUG(" Shutting Down FolderQuota : %s Thread" , m_szPath );
		   break;
	   }
	   else
	   {
		   ACE_UINT32 u32data = 0;

		   //! Decode the message here
		   ACE_OS::memcpy(&u32data,mb->rd_ptr(),sizeof(u32data));
		   m_enumNotType = ( NotificationType )u32data;

		   mb->release(); //HO80279
		   //! Quota Lock To avoid changes during alarm check
		   {
			   ACE_Guard<ACE_Recursive_Thread_Mutex> guard1(this->m_refsrctQuotaCS);

			   if ( m_enumNotType == ntChangeConfig )
			   {
				   DEBUG("%s"," Received Quota config change Request");
				   //! Configuration type has been changed
				   if (!m_bFolderQuotaCreated)
				   {
					   if( AddFolderQuota() == true )
					   {
						   m_bFolderQuotaCreated = true;
					   }
				   }
				   else
				   {
					   //! Folder quota  config changed
					   (void)ModifyFolderQuota();
					   enResult = enCheckAndRaiseAlarm();
				   }
			   }
			   else if ( m_enumNotType == ntQuotaEvent  )
			   {
				   //DEBUG("%s"," Received ntQuotaEvent - ACS_SSU_FolderQuota::svc()");
				   enResult = enCheckAndRaiseAlarm();
			   }
			   else if( m_enumNotType ==  ntFailOver )
			   {
				   enResult = EN_FOLDER_SUCCESS;
				   Stop(false);
			   }

			   if( enResult == EN_FOLDER_FOLDER_NOT_AVAILABLE )
			   {
				   bContinue = false;
			   }

		   }
	   }
   }

   if( enResult == EN_FOLDER_FOLDER_NOT_AVAILABLE )
   {
	   //! Stop Alarm and remove quota
	   Stop(true);
   }
   else
   {
	  //! Only Stop Alarm. If remotestop alarms are already stopped
	  if( remoteStop == false )
	  {
		 //ACE_Guard<ACE_Recursive_Thread_Mutex> guard1(this->m_refsrctQuotaCS);
         Stop(FALSE);
	  }
	  else
	  {
		  remoteStop = false;
	  }
   }

   m_IsThreadActive = false;

   return 0;
}

void ACS_SSU_FolderQuota::RemoteStop(const BOOL bRemoveRemoveQuota)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard1(this->m_refsrctQuotaCS);
   remoteStop=true;
   Stop(bRemoveRemoveQuota);
}

//============================================================================
// Folder quota thread register function
//============================================================================
ACE_INT32 ACS_SSU_FolderQuota::MonitorThread(void *lpvThis)
{
   ((ACS_SSU_FolderQuota*)lpvThis)->svc();
   return 0;
}

//============================================================================
// Signal the event that a folder quota threshold or limit has been reached
//============================================================================
void ACS_SSU_FolderQuota::Notify(const DOUBLE /*nValue*/)
{
   m_enumNotType = ntQuotaEvent;
//   (void)::SetEvent(m_hEvent);
}

BOOL ACS_SSU_FolderQuota::RemoveFolderQuota()
{
   BOOL bResetQuota = true;

   bResetQuota = AddFolderQuota(bResetQuota);

   return bResetQuota;
}

//! Stop monitoring a folder quota
void ACS_SSU_FolderQuota::Stop(const BOOL bRemoveQuota)
{
   if (m_bAlarmA1Raised || m_bAlarmA2Raised)
   {
	//  ACE_Guard<ACE_Recursive_Thread_Mutex> guard1(this->m_refsrctQuotaCS);

      if (m_bAlarmA1Raised)
      {
         // Send alarm cease
         m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA1CeaseLevel, 0.0);
      }

      if (m_bAlarmA2Raised)
      {
         //! Send alarm cease
         m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA2CeaseLevel, 0.0);
      }

      m_bAlarmA1Raised = false;
      m_bAlarmA1Raised = false;
   }

   if ( bRemoveQuota )
      (void)RemoveFolderQuota();
}

void ACS_SSU_FolderQuota::vAddSubDirObj( ACS_SSU_FolderQuota * ptrFolderQuotaObj)
{
   m_listSubDirObjs.push_back(ptrFolderQuotaObj);
}

void ACS_SSU_FolderQuota::vRemoveSubDirObjs()
{
   m_listSubDirObjs.clear();
}


FolderQuotaCheckStatus ACS_SSU_FolderQuota::enCheckAndRaiseAlarm()
{
   ACE_UINT64 u64FolderSize;
   FolderQuotaCheckStatus enResult = EN_FOLDER_SUCCESS;

   if ( ACS_SSU_Common::CheckFile(m_szPath) == SSU_DIRECTORY )
   {
	   double dCurrentUsageInPer;
	   u64FolderSize = 0;
	   CalculateFolderSize(u64FolderSize);
	   if (m_u64LimitQuota !=0)
	   {	
	   dCurrentUsageInPer = CalculateFolderQuota( m_u64LimitQuota,u64FolderSize );

	   if ( dCurrentUsageInPer <= m_nA1AlarmLevel )
	   {
		DEBUG("%s","Inside dCurrentUsageInPer <=  m_nA1AlarmLevel");
		   if (m_bAlarmA2Raised)
		   {
			   // Send A2 alarm cease
                           DEBUG("Folder %s :  Free:%f A1:%d A2:%d",m_szPath,(float)dCurrentUsageInPer,(ACE_INT32)m_nA1AlarmLevel,(ACE_INT32)m_nA2AlarmLevel);
			   m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA2CeaseLevel, dCurrentUsageInPer);
			   m_bAlarmA2Raised = false;

		   }

		   if (!m_bAlarmA1Raised)
		   {
			   //! Send A1 alarm
                           DEBUG("Folder %s :  Free:%f A1:%d A2:%d",m_szPath,(float)dCurrentUsageInPer,(ACE_INT32)m_nA1AlarmLevel,(ACE_INT32)m_nA2AlarmLevel);
			   m_pAlarmHandler->SendFolderQuotaAlarm(m_u64EventNumber, ACE_TEXT("A1"), m_szPath, m_nA1AlarmLevel, dCurrentUsageInPer);
			   m_bAlarmA1Raised = true;

		   }
	   }
	   else if ( dCurrentUsageInPer <= m_nA2AlarmLevel )
	   {
		   if (m_bAlarmA1Raised)
		   {
			   if ( dCurrentUsageInPer > m_nA1CeaseLevel )
			   {
				   // Send A1 alarm cease
                                   DEBUG("Folder %s :  Free:%f A1:%d A2:%d",m_szPath,(float)dCurrentUsageInPer,(ACE_INT32)m_nA1AlarmLevel,(ACE_INT32)m_nA2AlarmLevel);
				   m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA1CeaseLevel, dCurrentUsageInPer);
				   m_bAlarmA1Raised = false;

				   // Send A2 alarm
				   ACE_OS::sleep(50);
				   m_pAlarmHandler->SendFolderQuotaAlarm(m_u64EventNumber, ACE_TEXT("A2"), m_szPath, m_nA2AlarmLevel, dCurrentUsageInPer);
				   m_bAlarmA2Raised = true;

			   }
		   }
		   else
		   {
			   if (!m_bAlarmA2Raised)
			   {
				   // Send A2 alarm
                                   DEBUG("Folder %s :  Free:%f A1:%d A2:%d",m_szPath,(float)dCurrentUsageInPer,(ACE_INT32)m_nA1AlarmLevel,(ACE_INT32)m_nA2AlarmLevel);
				   m_pAlarmHandler->SendFolderQuotaAlarm(m_u64EventNumber, ACE_TEXT("A2"), m_szPath, m_nA2AlarmLevel, dCurrentUsageInPer);
				   m_bAlarmA2Raised = true;
			   }
		   }
	   }
	   else
	   {
		   if (m_bAlarmA1Raised)
		   {
			   // Send A1 alarm cease
                           DEBUG("Folder %s :  Free:%f A1:%d A2:%d",m_szPath,(float)dCurrentUsageInPer,(ACE_INT32)m_nA1AlarmLevel,(ACE_INT32)m_nA2AlarmLevel);
			   m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA1CeaseLevel, dCurrentUsageInPer);
			   m_bAlarmA1Raised = false;
		   }

		   if (m_bAlarmA2Raised)
		   {
//			   if ( dCurrentUsageInPer < m_nA2CeaseLevel )
//			   {
				   // Send A2 alarm cease
                                   DEBUG("Folder %s :  Free:%f A1:%d A2:%d",m_szPath,(float)dCurrentUsageInPer,(ACE_INT32)m_nA1AlarmLevel,(ACE_INT32)m_nA2AlarmLevel);
				   m_pAlarmHandler->SendFolderQuotaCease(m_u64EventNumber, m_szPath, m_nA2CeaseLevel, dCurrentUsageInPer);
				   m_bAlarmA2Raised = false;
//			   }
		   }
	   }
   
}
else
{
	return enResult;
}

}
   else
   {
	   enResult = EN_FOLDER_FOLDER_NOT_AVAILABLE;
	   ERROR("enCheckAndRaiseAlarm Folder: %s Not available", m_szPath);
   }

   if( m_bAlarmA1Raised || m_bAlarmA2Raised )
   {
       ACE_TCHAR pszBuffer[1024];
       GetQuotaValue(pszBuffer);
   }

   return enResult;
}

ACE_UINT64 ACS_SSU_FolderQuota::getFolderLimit()
{
	return m_u64Limit;
}
ACE_UINT64 ACS_SSU_FolderQuota::getFolderCurrentConsumption()
{
	return m_u64CurrentConsumption;
}
ACE_UINT32 ACS_SSU_FolderQuota::getA1AlarmLevel()
{
	return m_nA1AlarmLevel;
}
ACE_UINT32 ACS_SSU_FolderQuota::getA1CeaseLevel()
{
	return m_nA1CeaseLevel;
}
ACE_UINT32 ACS_SSU_FolderQuota::getA2AlarmLevel()
{
	return m_nA2AlarmLevel;
}
ACE_UINT32 ACS_SSU_FolderQuota::getA2CeaseLevel()
{
	return m_nA2CeaseLevel;
}


