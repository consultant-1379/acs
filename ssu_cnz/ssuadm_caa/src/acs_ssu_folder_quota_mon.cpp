/*=================================================================== */
/**
   @file acs_ssu_folder_quota_mon.cpp

   This module contains the implementation for This Thread reads configuration data from IMM and modifies folder quotas accordingly.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
   N/A       18/10/2022     zpsxsou  Added for 1300gb disk capacity.

                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <mntent.h>
#include <sys/quota.h>
#include <sys/inotify.h>
#include <acs_ssu_execute.h>
#include "acs_ssu_types.h"
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_path.h"
#include "acs_ssu_folder_quota.h"
#include "acs_ssu_folder_quota_mon.h"
#include "acs_ssu_types.h"
//APGCC
#include "acs_apgcc_omhandler.h"
#include <acs_apgcc_paramhandling.h>
#include "ACS_APGCC_CommonLib.h"
#include "ACS_APGCC_CLibTypes.h"

using namespace std;

#define PARAM_LEN                    2048
#define POPEN_READ 0
#define POPEN_WRITE 1

ACE_Recursive_Thread_Mutex  ACS_SSU_FolderQuotaMon::m_FolderLock;
std::map<std::string,std::string> ACS_SSU_Path::m_mapInternalPaths;
//FileM Change
std::map<std::string,std::string> ACS_SSU_Path::m_mapInternalFileMPaths;
int ObjectCnt = 1;
bool ssuconfigChange = false;
bool stopINotifyThread = false;

pid_t pid_in_progress = -1;
pid_t popen_with_pid(std::string command, int *infp, int *outfp);

void iNotifyThread(void *aPtr);

pid_t popen_with_pid(std::string command, int *infp, int *outfp)
{
	int p_stdin[2], p_stdout[2];
	pid_t pid = -1;

	if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
		return -1;

	pid = fork();

	if (pid < 0)
		return pid;
	else if (pid == 0)
	{
		close(p_stdin[POPEN_WRITE]);
		dup2(p_stdin[POPEN_READ], POPEN_READ);
		close(p_stdout[POPEN_READ]);
		dup2(p_stdout[POPEN_WRITE], POPEN_WRITE);
		dup2(p_stdout[POPEN_WRITE],2);

		execl("/bin/sh", "sh", "-c", command.c_str(), NULL);
		perror("execl");
		exit(1);
	}

	if (infp == NULL)
		close(p_stdin[POPEN_WRITE]);
	else
		*infp = p_stdin[POPEN_WRITE];

	if (outfp == NULL)
		close(p_stdout[POPEN_READ]);
	else
		*outfp = p_stdout[POPEN_READ];

	close(p_stdin[POPEN_READ]);		//HX58640
	close(p_stdout[POPEN_WRITE]);	//HX58640

	return pid;
}//End of popen_with_pid

ACE_INT32 killAllOutstandingPIDs()
{
	FILE *fp = NULL;
	ACE_TCHAR cmd[100];

	if(pid_in_progress != -1)
	{
		ACE_INT32 status = -1;
		ACE_INT32 ret = -1;
		sprintf(cmd,"%s %d %s","kill -9 ",pid_in_progress," 1>/dev/null 2>/dev/null");
		DEBUG("%s",cmd);
		fp = popen(cmd, "r");

		if (fp == NULL)
		{
			DEBUG("Error in executing the %s",cmd);
			return -1;
		}

		status = pclose(fp);

		if (WIFEXITED(status) )
		{
			ret=WEXITSTATUS(status);
			DEBUG("%s execution status = %d",cmd,ret);
		}

		if (ret == 0 )
		{
			DEBUG("%s","outstanding PID of quotacheck process is killed !!");
			return ret;
		}
		else
		{
			DEBUG("%s failed",cmd);
			return -1;

		}
	}
	else
	{
		DEBUG("%s","There are no outstanding Jobs(PIDs)");
		return 0;
	}
}//End of killAllOutstandingPIDs

ACS_SSU_FolderQuotaMon::ACS_SSU_FolderQuotaMon( ACE_Event* poEvent, ACS_SSU_AlarmHandler* pAlarmHandler, OmHandler* omHandlerPtr )
:
						m_pAlarmHandler( pAlarmHandler ),
						m_phEndEvent( poEvent ),
						m_bIsPrepared(false),
						m_bIsThreadPrepared( true ),
						m_omHandler(omHandlerPtr)
{
	m_szConfigType = new char[25];
	memset(m_szConfigType,'\0',25);

	//! Set the default config type
	ACE_OS::strcpy(m_szConfigType, SSU_DEFAULT_CONFIG_TYPE);
	m_nNumOfParams=0;
	strcpy(m_szDummyParams,"");
	m_bIsActiveNode = false;
	aptType=0;
	apNodeNumber=0;
	systemType=0;
	dataDiskSize=0;
	m_szBoardType = new char[16];
	memset(m_szBoardType,'\0',sizeof(m_szBoardType));
	memset(myGEPVersion,'\0',sizeof(myGEPVersion));
}

//! Destructor
ACS_SSU_FolderQuotaMon::~ACS_SSU_FolderQuotaMon()
{
	DEBUG("%s","Entering ACS_SSU_FolderQuotaMon::~ACS_SSU_FolderQuotaMon()");
	delete []m_szConfigType;
	delete []m_szBoardType;
	//sleep(3);
	DEBUG("%s","Exiting ACS_SSU_FolderQuotaMon::~ACS_SSU_FolderQuotaMon()");
}

void ACS_SSU_FolderQuotaMon::stopFolderQuotaMonThread()
{
	DEBUG("%s","Entering ACS_SSU_FolderQuotaMon::stopFolderQuotaMonThread()......");
	killAllOutstandingPIDs();
	 // To close thread properly.
	ACE_INT32 Wait = SSU_FQUOTA_MON_LONG_WTIME;
	ACE_Time_Value oTimeValue1(Wait);
	ACS_SSU_FolderQuotaMon::m_FolderLock.acquire();
	DEBUG("%s","Acquired ACS_SSU_FolderQuotaMon thread in stopFolderQuotaMonThread()"); 

	while (!m_listFolderQuotas.empty())
	{
		ACS_SSU_FolderQuota* pFolderQuota = *m_listFolderQuotas.begin();
		ACE_Message_Block * mb;
		ACE_NEW(mb,ACE_Message_Block(0,ACE_Message_Block::MB_HANGUP));
		pFolderQuota->putq(mb);
		pFolderQuota->wait();
		(void)m_listFolderQuotas.erase(m_listFolderQuotas.begin());
		delete pFolderQuota;
		pFolderQuota = NULL;
	}

	while (!m_listFileMFolderQuotas.empty())
	{
		ACS_SSU_FolderQuota* pFolderQuota = *m_listFileMFolderQuotas.begin();
		ACE_Message_Block * mb;
		ACE_NEW(mb,ACE_Message_Block(0,ACE_Message_Block::MB_HANGUP));
		pFolderQuota->putq(mb);
		pFolderQuota->wait();
		(void)m_listFileMFolderQuotas.erase(m_listFileMFolderQuotas.begin());
		delete pFolderQuota;
		pFolderQuota = NULL;
	}
	DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread in stopFolderQuotaMonThread()");
	stopINotifyThread = true;
	ACS_SSU_FolderQuotaMon::m_FolderLock.release();
	DEBUG("%s","Exiting ACS_SSU_FolderQuotaMon::stopFolderQuotaMonThread()");
}
//! Check that extracted folder quota does not already exist in the quota list
BOOL ACS_SSU_FolderQuotaMon::CheckQuotaListItemExist(LPSSU_FOLDERQUOTA_MONITOR pQuota,
		ACS_SSU_QuotasList& QuotaList)
{
	BOOL bExist = FALSE;
//	cout << pQuota->symName << endl;
	// SSU Monitors only FMS Folder
//	if(ACE_OS::strcmp(pQuota->symName,"FMS_DATA_BASE")!= 0)
//	{
//		return true;
//	}
	ACS_SSU_QuotasList::iterator iter = QuotaList.begin();
	
	while (!bExist && iter != QuotaList.end())
	{
		LPSSU_FOLDERQUOTA_MONITOR pListItem = (*iter);
		if (ACE_OS::strcmp(pListItem->Path, pQuota->Path) == 0 &&
				ACE_OS::strcmp(pListItem->ConfigType, pQuota->ConfigType) == 0 )
			bExist = true;
		else
			++iter;
	}

	if ( bExist )
		ERROR("Folder Quota %s already monitored.",pQuota->Path );
	return bExist;
}

//! Erase all SSU_FOLDERQUOTA_MONITOR structures in the STL list object
void ACS_SSU_FolderQuotaMon::EraseQuotaList(ACS_SSU_QuotasList& QuotaList)
{
	//! Remove and deallocate all elements in the folder quota list
	//! Remove and deallocate all elements in the folder quota list
	while (!QuotaList.empty())
	{
		SSU_FOLDERQUOTA_MONITOR* pQuota = *QuotaList.begin();
		(void)QuotaList.erase(QuotaList.begin());
		delete pQuota;
	}
}

//! The threads main function
//!@todo Change s32Wait to 5 Minutes after review
int ACS_SSU_FolderQuotaMon::svc()
{
	DEBUG("%s","Entering ACS_SSU_FolderQuotaMon::svc()");

	ACS_SSU_FolderQuotaList::iterator iter, iterFileM;
	ACS_SSU_FolderQuota* pFolderQuota;
	ACE_INT32 s32Wait = SSU_FQUOTA_MON_SHORT_WTIME;
	ACE_Time_Value oTimeValue(s32Wait);
	BOOL bContinue=true;
	bool isDataDiskPresent = true;
	bool isStopEventSignalled = true;

	isDataDiskPresent = bCheckFolderQuotaEnabled();
	while(isDataDiskPresent == false && (isStopEventSignalled = m_phEndEvent->wait(&oTimeValue,0)))
	{
		WARNING("%s" ,"/data disk is not yet Ready on Active Node");
		WARNING("%s","Monitoring of the folders is not yet initiated !!");
		isDataDiskPresent = bCheckFolderQuotaEnabled();
	}

	if (!isStopEventSignalled)
	{
		// IN CASE OF STOP EVENT
		DEBUG("%s","STOP EVENT is signaled! Hence FolderQuotaMon thread is Exiting.")
		return 0;
	}

	while( bContinue && m_phEndEvent->wait(&oTimeValue,0) )
	{
		s32Wait = SSU_FQUOTA_MON_MEDIUM_WTIME;

		ACE_INT32 Wait = SSU_FQUOTA_MON_LONG_WTIME;
		ACE_Time_Value oTimeValue1(Wait);
		ACS_SSU_FolderQuotaMon::m_FolderLock.acquire(&oTimeValue1);
		DEBUG("%s","Acquired ACS_SSU_FolderQuotaMon thread in svc()");

		if (!m_bIsPrepared)
		{
			DEBUG("Prepare Folder Quota supervision : Polling Interval is %d",s32Wait);
			if ( SetupFolderQuotaMonitor() )
			{
				m_bIsPrepared = true;
				iter = m_listFolderQuotas.begin();
				while (iter != m_listFolderQuotas.end())
				{
					pFolderQuota = (*iter);

					if (!pFolderQuota->FolderIsAvailable())
					{
						pFolderQuota->AddFolderQuota();
						iter = m_listFolderQuotas.erase(iter);
						delete pFolderQuota;
						pFolderQuota = NULL;
					}
					else
					{
					 ++iter;
					}
				}
#if 0
				//FileM Changes
				iterFileM = m_listFileMFolderQuotas.begin();

				while (iterFileM != m_listFileMFolderQuotas.end())
				{
					pFolderQuota = (*iterFileM);
					pFolderQuota->AddFolderQuota();
					++iterFileM;
				}
#endif
				s32Wait = SSU_FQUOTA_MON_MEDIUM_WTIME;

				INFO("Size of object list = %d",m_listFolderQuotas.size());
				INFO("Size of fileM folder object list = %d",m_listFileMFolderQuotas.size());
				//Create inotify thread to watch /data directory
	                        stopINotifyThread = false;
        	                ACE_thread_t iNofityThrId;
                	        int myResult = ACE_OS::thr_create((ACE_THR_FUNC)iNotifyThread, 0, THR_DETACHED,&iNofityThrId);
                        	if(myResult != 0)
                        	{
                                	DEBUG("%s","iNotify thread spawning failed");
                        	}
				else
				{
					DEBUG("%s","iNotify thread spawned successfully");
				}

			}
			else
			{
				bContinue = false;
			}
		}
		else
		{
			s32Wait = SSU_FQUOTA_MON_MEDIUM_WTIME;
			//! Check if any folder not supervised yet is available
			iter = m_listFolderQuotas.begin();
			while (iter != m_listFolderQuotas.end())
			{
				pFolderQuota = (*iter);
				if (pFolderQuota->FolderIsAvailable() && !pFolderQuota->IsActive())
				{
					DEBUG(" Activate Folder Quota Thread : %s ", pFolderQuota->get_Path());
					pFolderQuota->activate(THR_NEW_LWP|THR_JOINABLE|THR_SUSPENDED);
					ACE_Time_Value oTimeSleep(0,400*pow(10,-6));
					ACE_OS::sleep(oTimeSleep);
				}
				++iter;
			}
			//DEBUG("Size of m_listFileMFolderQuotas = %d",m_listFileMFolderQuotas.size());	
			iterFileM = m_listFileMFolderQuotas.begin();	
			while (iterFileM != m_listFileMFolderQuotas.end())
			{
				pFolderQuota = (*iterFileM);
				if (pFolderQuota->FolderIsAvailable() && !pFolderQuota->IsActive())
				{
					DEBUG(" Activate Folder Quota Thread : %s ", pFolderQuota->get_Path());
					pFolderQuota->activate(THR_NEW_LWP|THR_JOINABLE|THR_SUSPENDED);
					ACE_Time_Value oTimeSleep(0,400*pow(10,-6));
					ACE_OS::sleep(oTimeSleep);
				}
				++iterFileM;
			}
		
			DEBUG(" # %s ","FolderQuotaMon Thread is Active and sending QuotaEvent for checking Alarm limits");
			vSendMessageToQuotaThreads(ntQuotaEvent);

		}
		oTimeValue.set(s32Wait,0);
                DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread in svc()");
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		
	}

	DEBUG("%s","Exiting ACS_SSU_FolderQuotaMon::svc()");
	return 0;
}

#if 0
//! Gets the folder quota configurations and send back to the command ssuls
ACE_UINT32 ACS_SSU_FolderQuotaMon::ListFolderQuotaConfig(BOOL bSetAllImmData)
{
	//! To be changes after implemenation of SSULS command
	DEBUG(" Received command:%s", "ListFolderQuotaConfig");
	if(bSetAllImmData) ssuconfigChange = true;

	ACE_TCHAR szBuffer[1024];
	void* new_value[1] = {0};
	char dnName[256];
	int index = 1;
	/// Adding into IMM
	ACS_CC_ImmParameter parToModify;
	ACS_CC_ReturnType enResult;

	ACS_SSU_FolderQuotaList::iterator iter = m_listFolderQuotas.begin();
	while (iter != m_listFolderQuotas.end())
	{
		(*iter)->print();
		if ((*iter)->FolderIsAvailable())
		{
			if ((*iter)->GetQuotaValue(szBuffer))
			{
				sprintf(dnName,"%s=%s,%s",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_RDN_NAME),(*iter)->get_symbolicName(),ACS_SSU_Common::dnOfSSURoot.c_str());
				DEBUG("Runtime Object that is going to Modified is :%s\n", dnName);

		        unsigned long currentSize = (*iter)->getFolderCurrentConsumption();
				parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_CURR_FOLDERSIZE;
				parToModify.attrType = ATTR_UINT64T;
				parToModify.attrValuesNum = 1;
				new_value[0] = reinterpret_cast<void*>(&currentSize);
				parToModify.attrValues = new_value;
				enResult = runtimeOwner.modifyRuntimeObj(dnName, &parToModify);
				DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_CURR_FOLDERSIZE),enResult);

				if (bSetAllImmData == true)
				{
					long maxFolderSize = (*iter)->getFolderLimit();
					parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_UPPER_LIMIT;
					parToModify.attrType = ATTR_INT64T;
					parToModify.attrValuesNum = 1;
					new_value[0] = reinterpret_cast<void*>(&maxFolderSize);
					parToModify.attrValues = new_value;
					enResult = runtimeOwner.modifyRuntimeObj(dnName, &parToModify);
					DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_UPPER_LIMIT),enResult);

					unsigned int value = (*iter)->getA1AlarmLevel();

					parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A1_ALARM_LIMIT;
					parToModify.attrType = ATTR_UINT32T;
					parToModify.attrValuesNum = 1;
					new_value[0] = reinterpret_cast<void*>(&value);
					parToModify.attrValues = new_value;
					enResult = runtimeOwner.modifyRuntimeObj(dnName, &parToModify);
					DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A1_ALARM_LIMIT),enResult);

					value = (*iter)->getA1CeaseLevel();
					parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A1_CEASE_LIMIT;
					parToModify.attrType = ATTR_UINT32T;
					parToModify.attrValuesNum = 1;
					new_value[0] = reinterpret_cast<void*>(&value);
					parToModify.attrValues = new_value;
					enResult = runtimeOwner.modifyRuntimeObj(dnName, &parToModify);
					DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A1_CEASE_LIMIT),enResult);

					value = (*iter)->getA2AlarmLevel();
					parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A2_ALARM_LIMIT;
					parToModify.attrType = ATTR_UINT32T;
					parToModify.attrValuesNum = 1;
					new_value[0] = reinterpret_cast<void*>(&value);
					parToModify.attrValues = new_value;
					enResult = runtimeOwner.modifyRuntimeObj(dnName, &parToModify);
					DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A2_ALARM_LIMIT),enResult);

					value = (*iter)->getA2CeaseLevel();
					parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A2_CEASE_LIMIT;
					parToModify.attrType = ATTR_UINT32T;
					parToModify.attrValuesNum = 1;
					new_value[0] = reinterpret_cast<void*>(&value);
					parToModify.attrValues = new_value;
					enResult = runtimeOwner.modifyRuntimeObj(dnName, &parToModify);
					DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A2_CEASE_LIMIT),enResult);
				}
				index++;
			}
		}
		++iter;
	}

	if (ssuconfigChange)
	{
		ssuconfigChange = false;
	}

	return 0;
}
#endif

//! Prepare and initialize the folder quota supervision
BOOL ACS_SSU_FolderQuotaMon::PrepareFolderQuotas()
{
	DEBUG("%s", "Entering ACS_SSU_FolderQuotaMon::PrepareFolderQuotas()");

	if (!ReadConfigType())
		return false;

	DEBUG(" PrepareFolderQuotas %s", m_szConfigType);

	ACS_SSU_QuotasList listQuotas, listFileMQuotas ;

	ACS_SSU_Path objGetFolderPath;
	BOOL pathStatus = objGetFolderPath.createMap();
	if(!pathStatus)
	{
		ERROR("Status of creating the path map : %d ",pathStatus);
	}

	pathStatus = objGetFolderPath.createFileMMap();
	if(!pathStatus)
	{
		ERROR("Status of creating the fileM path map : %d ",pathStatus);
	}
	if (!ReadQuotas(listQuotas, m_szConfigType, true))
	{
		ERROR("%s","ReadQuotas failed in PrepareFolderQuotas");
		return false;
	}

	DEBUG("%s","Before calling ReadFileMQuotas()");
	if (!ReadFileMQuotas(listFileMQuotas, m_szConfigType, true))
	{
		ERROR("%s","ReadFileMQuotas failed in PrepareFolderQuotas");
		return false;
	}
	DEBUG("%s","After calling ReadFileMQuotas()");

	ACE_INT32 Wait = 10;
        ACE_Time_Value oTimeValue(Wait);
        ACS_SSU_FolderQuotaMon::m_FolderLock.acquire(&oTimeValue);
	DEBUG("%s","Acquired ACS_SSU_FolderQuotaMon thread in PrepareFolderQuotas()");


	ACS_SSU_FolderQuota* pFolderQuota;
	ACS_SSU_QuotasList::iterator iter = listFileMQuotas.begin();
	BOOL bSuccess = true;

	//Filling list of structures which contains filem path
	while (bSuccess && iter != listFileMQuotas.end())
	{
		pFolderQuota = new ACS_SSU_FolderQuota(m_pAlarmHandler,
				*iter,
				m_srctQuotaCS);
		if (!pFolderQuota)
		{
			//! Report error
			(void)ACS_SSU_AEH::ReportEvent(1003,
					ACE_TEXT("EVENT"),
					CAUSE_AP_INTERNAL_FAULT,
					ACE_TEXT(""),
					ACE_TEXT("Failed to create \"ACS_SSU_FolderQuota\" object."),
					ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

			bSuccess = false;
		}
		else
		{
			// Add specific alarm level to list
			m_listFileMFolderQuotas.push_back(pFolderQuota);
			++iter;
		}
	}

	iter = listQuotas.begin();
	while (bSuccess && iter != listQuotas.end())
	{
		pFolderQuota = new ACS_SSU_FolderQuota(m_pAlarmHandler,
				*iter,
				m_srctQuotaCS);
		if (!pFolderQuota)
		{
			//! Report error
			(void)ACS_SSU_AEH::ReportEvent(1003,
					ACE_TEXT("EVENT"),
					CAUSE_AP_INTERNAL_FAULT,
					ACE_TEXT(""),
					ACE_TEXT("Failed to create \"ACS_SSU_FolderQuota\" object."),
					ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

			bSuccess = false;
		}
		else
		{
			// Add specific alarm level to list
			m_listFolderQuotas.push_back(pFolderQuota);
			++iter;
		}
	}
	 DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread in PrepareFolderQuotas()");
	ACS_SSU_FolderQuotaMon::m_FolderLock.release();
	EraseQuotaList(listFileMQuotas);
	EraseQuotaList(listQuotas);

	DEBUG("%s", "Leaving ACS_SSU_FolderQuotaMon::PrepareFolderQuotas()");

	return bSuccess;
}

//! Read the AP configuration type (BSC, MSC or HLR) from the CONFIG file
BOOL ACS_SSU_FolderQuotaMon::ReadConfigType()
{
	ACS_CC_ImmParameter paramToFind;
	paramToFind.attrName = new char [30];
	ACS_CC_ReturnType enResult;
	std::string apzDN(SSU_APZ_DN);
	// Fetch aptType
	ACE_OS::strcpy( paramToFind.attrName , SSU_APZ_APTTYPE);

	if( ( enResult = m_omHandler->getAttribute( apzDN.c_str(), &paramToFind ) ) == ACS_CC_SUCCESS )
	{
		aptType = *(reinterpret_cast<int*>(*(paramToFind.attrValues)));
		WARNING(" aptType recieved is :%d",aptType);
	}

	if( enResult != ACS_CC_SUCCESS )
	{
		aptType = 2;
		WARNING("%s","Fetching aptType is Failed. Initializing to default value i.e 2");

	}

	if ( aptType < 0 || aptType > 5)
	{
		aptType = 2;
		WARNING("%s","aptType validation is Failed. Initializing to default value i.e 2");
	}

	// Fetch systemType
	ACE_OS::strcpy( paramToFind.attrName , SSU_APZ_SYSTEMTYPE);

	if( ( enResult = m_omHandler->getAttribute( apzDN.c_str(), &paramToFind ) ) == ACS_CC_SUCCESS )
	{
		systemType = *(reinterpret_cast<int*>(*(paramToFind.attrValues)));
		WARNING(" systemType recieved is :%d",systemType);
	}

	if( enResult != ACS_CC_SUCCESS )
	{
		systemType = 0;
		WARNING("%s","Fetching systemType is Failed. Initializing to default value i.e 0");

	}

	if ( systemType < 0 || systemType > 1)
	{
		systemType = 0;
		WARNING("%s","systemType validation is Failed. Initializing to default value i.e 0");
	}

	// Fetch apNodeNumber
	ACE_OS::strcpy( paramToFind.attrName , SSU_APZ_APNODENUMBER);

	if( ( enResult = m_omHandler->getAttribute( apzDN.c_str(), &paramToFind ) ) == ACS_CC_SUCCESS )
	{
		apNodeNumber = *(reinterpret_cast<int*>(*(paramToFind.attrValues)));
		WARNING(" apNodeNumber recieved is :%d",apNodeNumber);
	}

	if( enResult != ACS_CC_SUCCESS )
	{
		apNodeNumber = 0;
		WARNING("%s","Fetching apNodeNumber is Failed. Initializing to default value i.e 1");

	}

	if ( apNodeNumber < 1 || apNodeNumber > 28)
	{
		apNodeNumber = 0;
		WARNING("%s","apNodeNumber validation is Failed. Initializing to default value i.e 1");
	}


        dataDiskSize = getDataDiskSize();
				DEBUG("data disk size is %d",dataDiskSize);
	
	ACE_OS::strcpy(m_szBoardType,getBoardType());
	DEBUG("boardType is  %s",m_szBoardType);

        if (dataDiskSize == -1)
         {
                dataDiskSize = 300;
                WARNING("%s","dataDiskSize validation is Failed. Initializing to default value i.e 300");
         }

        WARNING("Data Disk size is  %d",dataDiskSize);
	ACE_OS::strcpy(m_szConfigType,getConfigString(aptType,systemType,apNodeNumber,dataDiskSize,m_szBoardType));
	delete [] paramToFind.attrName;
	return true;
}

//! Read and parse a single line from the folder quota parameter file
//! Example of a line: "\data\acs\data",6106,50,8,10,12,14;
ACE_INT32 ACS_SSU_FolderQuotaMon::ReadFileMQuota( ifstream * hFile,
		SSU_FOLDERQUOTA_MONITOR* pQuotaDef)
{

	DEBUG("%s","Entering ACS_SSU_FolderQuotaMon::ReadFileMQuota()");

	//! Line to be processed is copied in this buffer
	ACE_TCHAR pszCharBuff[200] = {0};
	std::string pszReadLine ;
	ACE_TCHAR* pszToken;

	ACE_OS::memset(pQuotaDef,0,sizeof(SSU_FOLDERQUOTA_MONITOR));

	//! Read Lines from command output and process it accordingly
	//! End-of-file
	if (std::getline( *hFile, pszReadLine) <= 0)
	{
		return 1;
	}
	if(pszReadLine.size() != 0)
	{
		ACE_OS::strcpy( pszCharBuff , pszReadLine.c_str());

		pszToken = ACE_OS::strtok( pszCharBuff, ", ");

		ACE_INT32 s32FieldIdx = 0;

		DEBUG("%s","ACS_SSU_FolderQuotaMon::ReadFileMQuota() - Calling SetFileMQuotaDefValue() 1st time");

		SetFileMQuotaDefValue(pszToken, ++s32FieldIdx, pQuotaDef);

		//pszToken = ACE_OS::strtok( pszCharBuff, ", ");
		//SetFileMQuotaDefValue(pszToken, ++s32FieldIdx, pQuotaDef);

		while ( (( pszToken = ACE_OS::strtok(0,";, ")) != 0 ))
		{
			DEBUG("%s","ACS_SSU_FolderQuotaMon::ReadFileMQuota() - Calling SetFileMQuotaDefValue() multiple times");
			if (!SetFileMQuotaDefValue(pszToken, ++s32FieldIdx, pQuotaDef))
				return 2;
		}

		if (!ACS_SSU_Common::CheckFileExists(pQuotaDef->Path))
		{
			return 3;
		}
	}
	DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::ReadFileMQuota()");
	return 0;
}
//! Read and parse a single line from the folder quota parameter file
//! Example of a line: BSC,"\data\acs\data",6106,50,8,10,12,14;

ACE_INT32 ACS_SSU_FolderQuotaMon::ReadQuota( ifstream * hFile,
		SSU_FOLDERQUOTA_MONITOR* pQuotaDef)
{
	//! Line to be processed is copied in this buffer
	ACE_TCHAR pszCharBuff[200] = {0};
	std::string pszReadLine ;
	ACE_TCHAR* pszToken;

	ACE_OS::memset(pQuotaDef,0,sizeof(SSU_FOLDERQUOTA_MONITOR));

	//! Read Lines from command output and process it accordingly
	//! End-of-file
	if (std::getline( *hFile, pszReadLine) <= 0)
	{
		//cout<<"Reached EOF "<<endl;
		return 1;
	}
	if(pszReadLine.size() != 0)
	{
		ACE_OS::strcpy( pszCharBuff , pszReadLine.c_str());

//		INFO(" pszCharBuff :%s", pszCharBuff );

		//! Header Token ( "total" or "Mem" or "Swap" )
		pszToken = ACE_OS::strtok( pszCharBuff, ", ");

		ACE_INT32 s32FieldIdx = 0;

		SetQuotaDefValue(pszToken, ++s32FieldIdx, pQuotaDef);

		while ( (( pszToken = ACE_OS::strtok(0,";, ")) != 0 ))
		{
//			INFO(" pszToken :%s",pszToken);
			if (!SetQuotaDefValue(pszToken, ++s32FieldIdx, pQuotaDef))
				return 2;
		}

		if (!ACS_SSU_Common::CheckFileExists(pQuotaDef->Path))
		{
			return 3;
		}
	}
	return 0;
}

//! Reading the quotas from the file
BOOL ACS_SSU_FolderQuotaMon::ReadFileMQuotas(ACS_SSU_QuotasList& QuotaList,
		const ACE_TCHAR* lpszConfigType,
		BOOL prepare)
{
	DEBUG("%s","Entering ACS_SSU_FolderQuotaMon::ReadFileMQuotas()");
	ACE_TCHAR szTemp[512];
	ACE_OS::strncpy(szTemp, ACE_TEXT(SSU_FILEM_QUOTAS_FILE), 511);

	ifstream * m_hQuotaFile = new ifstream(szTemp,ios::in);

	if ( m_hQuotaFile == 0 )
	{
		ACE_TCHAR szData[512];
		ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Error1 - Failed to open the FileM Folder Quota file \"%s\". \n%m"),
				szTemp
		);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		ERROR("%s",szData);
		DEBUG("%s","Error - Exiting ACS_SSU_FolderQuotaMon::ReadFileMQuotas()");
		return false;
	}

	SSU_FOLDERQUOTA_MONITOR srctQuota;

	ACE_INT32 nResult=0;
	BOOL bResult = true;
	BOOL bContinue = true;
	while (bContinue)
	{
		SSU_FOLDERQUOTA_MONITOR* pQuota;
		ACE_OS::memset(&srctQuota,0,sizeof(SSU_FOLDERQUOTA_MONITOR));
		DEBUG("%s","Before calling ReadFileMQuota()");
		nResult = ReadFileMQuota(m_hQuotaFile, &srctQuota);
		DEBUG("%s","After calling ReadFileMQuota()");

		DEBUG("nResult = %d",nResult);

		if (nResult != 0)
		{
			if(nResult == 1)
			{
				bContinue = false;
			}
			if (nResult == 2)
			{
				ACE_TCHAR szData[512];
				ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
						ACE_TEXT("Invalid format of the FileM Folder Quota file \"%s\""), szTemp);

				ERROR(" %s",szData);

				//! Report error
				(void)ACS_SSU_AEH::ReportEvent(1003,
						ACE_TEXT("EVENT"),
						CAUSE_AP_INTERNAL_FAULT,
						ACE_TEXT(""),
						szData,
						ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));
				bResult = false;
			}

			if (nResult == 3)
			{
				ACE_TCHAR szData[512];
				ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
						ACE_TEXT("Invalid FileM Folder Quota path \"%s\""), srctQuota.Path);

				ERROR(" %s",szData);

				//! Report error
				(void)ACS_SSU_AEH::ReportEvent(1003,
						ACE_TEXT("EVENT"),
						CAUSE_AP_INTERNAL_FAULT,
						ACE_TEXT(""),
						szData,
						ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));
			}

			if (nResult != 3 )
				bContinue = false;
		}
		else
		{
			if ( ACE_OS::strcmp(srctQuota.ConfigType, lpszConfigType) == 0 &&
					*srctQuota.Path != ACE_TEXT('\0') &&
					!CheckQuotaListItemExist(&srctQuota, QuotaList) )
			{
				pQuota = new SSU_FOLDERQUOTA_MONITOR;

				if (!pQuota)
				{
					// Report error
					(void)ACS_SSU_AEH::ReportEvent(1003,
							ACE_TEXT("EVENT"),
							CAUSE_AP_INTERNAL_FAULT,
							ACE_TEXT(""),
							ACE_TEXT("Failed to allocate \"SSU_FOLDERQUOTA_MONITOR\" structure."),
							ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

					bContinue = bResult = false;
				}
				else
				{
					// Add specific alarm level to list
					ACE_OS::memcpy(pQuota, &srctQuota, sizeof(SSU_FOLDERQUOTA_MONITOR));
					QuotaList.push_back(pQuota);
					//DEBUG(" Adding Quota %s %s Structure to list",pQuota->ConfigType,pQuota->Path);
				}
			}
		}
	}
#if 0
	//INFO("%s","List of FileM folders with given configtype");
	//DEBUG("%s","============================================");

	ACS_SSU_QuotasList::iterator QuotaListIter;
	for(QuotaListIter = QuotaList.begin();QuotaListIter != QuotaList.end();++(QuotaListIter))
	{
		//INFO("path = %s \t config type = %s",(*QuotaListIter)->Path,(*QuotaListIter)->ConfigType);

	}
	//DEBUG("%s","============================================");
#endif
	//! Should never occur
	if (bResult && QuotaList.size() == 0)
	{
		ACE_TCHAR szData[512];
		ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Error2 - Failed to open the FileM Folder Quota file \"%s\".\n%m"),
				szTemp );

		ERROR("\n %s",szData);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				ACE_TEXT("No Folder Quota parameters found"),
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		if (!prepare)
			bResult = false;
	}

	m_hQuotaFile->close();
	delete m_hQuotaFile;
	DEBUG("%s","Exiting ACS_SSU_FolderQuotaMon::ReadFileMQuotas()");
	return bResult;
}

//! Reading the quotas from the file
BOOL ACS_SSU_FolderQuotaMon::ReadQuotas(ACS_SSU_QuotasList& QuotaList,
		const ACE_TCHAR* lpszConfigType,
		BOOL prepare)
{
	DEBUG("%s","Entering ACS_SSU_FolderQuotaMon::ReadQuotas()");
	ACE_TCHAR szTemp[512];
	//	restore this condition
#if 0
	ACE_OS::strncpy(szTemp, ACS_SSU_Path(SSU_QUOTAS_FILE), 511);
	ACE_OS::strncpy(szTemp, ACE_TEXT(SSU_QUOTAS_FILE), 511);
#endif
	ACE_OS::strncpy(szTemp, ACE_TEXT(SSU_QUOTAS_FILE), 511);
	//cout<<"szTemp: SSU_QUOTAS_FILE = "<<szTemp<<endl;

	DEBUG(" ReadQuotas From File %s", szTemp);

	ifstream * m_hQuotaFile = new ifstream(szTemp,ios::in);

	if ( m_hQuotaFile == 0 )
	{
		ACE_TCHAR szData[512];
		ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Failed to open the Folder Quota file \"%s\". \n%m"),
				szTemp
		);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		ERROR("%s",szData);
		DEBUG("%s","Exiting ACS_SSU_FolderQuotaMon::ReadQuotas()");
		return false;
	}

	SSU_FOLDERQUOTA_MONITOR srctQuota;

	ACE_INT32 nResult;
	BOOL bResult = true;
	BOOL bContinue = true;
	while (bContinue)
	{
		SSU_FOLDERQUOTA_MONITOR* pQuota;
		ACE_OS::memset(&srctQuota,0,sizeof(SSU_FOLDERQUOTA_MONITOR));
		nResult = ReadQuota(m_hQuotaFile, &srctQuota);
		if (nResult != 0)
		{
			if(nResult == 1)
			{
				//cout<<"Reached EOF"<<endl;
				bContinue = false;
			}
			if (nResult == 2)
			{
				ACE_TCHAR szData[512];
				ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
						ACE_TEXT("Invalid format of the Folder Quota file \"%s\""), szTemp);

				ERROR(" %s",szData);

				//! Report error
				(void)ACS_SSU_AEH::ReportEvent(1003,
						ACE_TEXT("EVENT"),
						CAUSE_AP_INTERNAL_FAULT,
						ACE_TEXT(""),
						szData,
						ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));
				bResult = false;
			}

			if (nResult == 3)
			{
				WARNING("srctQuota.Path = %s",srctQuota.Path);
				if (ACE_OS::strcmp(srctQuota.Path,"/data/ftpvol") != 0 && ACE_OS::strcmp(srctQuota.Path,"/data/images") != 0) 

				{
					DEBUG("srctQuota.Path inside = %s",srctQuota.Path);
					ACE_TCHAR szData[512];
					ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
							ACE_TEXT("Invalid Folder Quota path \"%s\""), srctQuota.Path);

					ERROR(" %s",szData);

					//! Report error
					(void)ACS_SSU_AEH::ReportEvent(1003,
							ACE_TEXT("EVENT"),
							CAUSE_AP_INTERNAL_FAULT,
							ACE_TEXT(""),
							szData,
							ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));
				}
			}

			if (nResult != 3 )
				bContinue = false;
		}
		else
		{
			//INFO( "Folder Quota Path: %s Config : %s Current:%s", srctQuota.Path,srctQuota.ConfigType,lpszConfigType);

			if ( ACE_OS::strcmp(srctQuota.ConfigType, lpszConfigType) == 0 &&
					*srctQuota.Path != ACE_TEXT('\0') &&
					!CheckQuotaListItemExist(&srctQuota, QuotaList) )
			{
				pQuota = new SSU_FOLDERQUOTA_MONITOR;

				if (!pQuota)
				{
					// Report error
					(void)ACS_SSU_AEH::ReportEvent(1003,
							ACE_TEXT("EVENT"),
							CAUSE_AP_INTERNAL_FAULT,
							ACE_TEXT(""),
							ACE_TEXT("Failed to allocate \"SSU_FOLDERQUOTA_MONITOR\" structure."),
							ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

					bContinue = bResult = false;
				}
				else
				{
					// Add specific alarm level to list
					ACE_OS::memcpy(pQuota, &srctQuota, sizeof(SSU_FOLDERQUOTA_MONITOR));
					QuotaList.push_back(pQuota);
					//DEBUG("Adding Quota %s %s Structure to list",pQuota->ConfigType,pQuota->Path);
				}
			}
		}

		//		delete pQuota;
		//		pQuota=NULL;

	}
	#if 0
	//INFO("%s","List of folders with given configtype");
	//DEBUG("%s","============================================");

	ACS_SSU_QuotasList::iterator QuotaListIter;
	for(QuotaListIter = QuotaList.begin();QuotaListIter != QuotaList.end();++(QuotaListIter))
	{
		//INFO("path = %s \t config type = %s",(*QuotaListIter)->Path,(*QuotaListIter)->ConfigType);

	}
	//DEBUG("%s","============================================");
	#endif

	//! Should never occur
	if (bResult && QuotaList.size() == 0)
	{

		ACE_TCHAR szData[512];
		ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Failed to open the Folder Quota file \"%s\".\n%m"),
				szTemp );

		ERROR("\n %s",szData);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				ACE_TEXT("No Folder Quota parameters found"),
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		if (!prepare)
			bResult = false;
	}

	m_hQuotaFile->close();
	delete m_hQuotaFile;
	DEBUG("%s","Exiting ACS_SSU_FolderQuotaMon::ReadQuotas()");
	return bResult;
}


//! Set the configuration type and send back the result to the command ssucfg
ACE_INT32 ACS_SSU_FolderQuotaMon::SetConfigType( const ACE_TCHAR * szNewConfigType )
{
	ACE_INT32 s32Result = SSU_RC_OK;

	ACE_INT32 Wait = 10;
	ACE_Time_Value oTimeValue(Wait);
	ACS_SSU_FolderQuotaMon::m_FolderLock.acquire(&oTimeValue);

	DEBUG("%s","In ACS_SSU_FolderQuotaMon::SetConfigType Method");
	//! Read the new quotas settings from the quota file
	ACS_SSU_QuotasList listQuotas;
	if ( ReadQuotas(listQuotas, szNewConfigType, false ) == false )
	{
		EraseQuotaList(listQuotas);
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		return SSU_RC_QUOTAFILEERROR;
	}
	//ACE_Guard<ACE_Recursive_Thread_Mutex> guard2(this->m_srctQuotaCS);

	BOOL bFound;//, bSuccess = true;
	ACS_SSU_FolderQuotaList::iterator FolderQuotasIter;

	//! Perform the actual change to all quota objects
	ACS_SSU_FolderQuota* pFolderQuota;
	SSU_FOLDERQUOTA_MONITOR* pQuota;
	ACS_SSU_QuotasList::iterator QuotasIter;
	QuotasIter = listQuotas.begin();
	while( QuotasIter != listQuotas.end() )
	{
		bFound = false;
		pQuota = *QuotasIter;

		FolderQuotasIter = m_listFolderQuotas.begin();
		while (!bFound && FolderQuotasIter != m_listFolderQuotas.end())
		{
			//! Check if this folder is supervised by us

			if (ACE_OS::strcmp((*FolderQuotasIter)->get_Path(),pQuota->Path) == 0/*(*FolderQuotasIter)->IsEqual(pQuota->Path)*/)
			{
				//! Change the folder quota configuration
				(*FolderQuotasIter)->ChangeConfig(pQuota);
				bFound = true;
			}
			else
				++FolderQuotasIter;
		}

		if (!bFound)
		{
			//! Add a new ACS_SSU_FolderQuota object to supervise
			pFolderQuota = new ACS_SSU_FolderQuota(m_pAlarmHandler,
					*QuotasIter,
					m_srctQuotaCS);

			if (!pFolderQuota)
			{
				// Report error
				(void)ACS_SSU_AEH::ReportEvent(1003,
						ACE_TEXT("EVENT"),
						CAUSE_AP_INTERNAL_FAULT,
						ACE_TEXT(""),
						ACE_TEXT("Failed to create \"ACS_SSU_FolderQuota\" object."),
						ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));
			}
			else
			{
				//! Add specific alarm level to list
				m_listFolderQuotas.push_back(pFolderQuota);
			}
		}

		++QuotasIter;
	}
	FolderQuotasIter = m_listFolderQuotas.begin();
	std::string dnName;
	while( FolderQuotasIter != m_listFolderQuotas.end() )
	{
		//! Check if there is an supervised folder with the "old" configuration type
		//! (should never occur).

		if( ACE_OS::strcmp((*FolderQuotasIter)->get_ConfigType(), szNewConfigType) != 0 )
		{
			//! Delete the supervision folder
			ACS_SSU_FolderQuota* pFolderQuota = (*FolderQuotasIter);
			pFolderQuota->RemoteStop(true);

			FolderQuotasIter = m_listFolderQuotas.erase(FolderQuotasIter);
		}
		else
		{
			++FolderQuotasIter;
		}
	}

	EraseQuotaList(listQuotas);

	/*if (!bSuccess)
	{
		s32Result = SSU_RC_NEWQUOTAEXCEEDED;
	}
	else
	{*/
		//! Set new config type
		ACE_OS::strcpy(m_szConfigType, szNewConfigType);
		vUpdateSubDirObjs();
		vSendMessageToQuotaThreads( ntChangeConfig );
	//}
	DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread in SetConfigType()");
	ACS_SSU_FolderQuotaMon::m_FolderLock.release();
	return s32Result;
}

//! Set the value of the parameter
BOOL ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue(const ACE_TCHAR* lpszValue,
		const ACE_INT32 nFieldIdx,
		SSU_FOLDERQUOTA_MONITOR* pQuotaDef)
{
	DEBUG("%s", "Entering ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");

	switch (nFieldIdx)
	{
	case 1:
		DEBUG("%s", "Case1 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		//(void)ACE_OS::strncpy(pQuotaDef->ConfigType, m_szConfigType, 15);
		(void)ACE_OS::strncpy(pQuotaDef->ConfigType, lpszValue, 24);
		break;
	case 2:
	{
		DEBUG("%s", "Case2 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		(void)ACE_OS::strncpy(pQuotaDef->symName,lpszValue, 127);
		ACS_SSU_Path objPath;
		objPath.ExpandFileMPath(lpszValue);
		(void)ACE_OS::strncpy(pQuotaDef->Path,objPath.m_szPath, 127);
		DEBUG( " Path %s -> %s --- %s ", lpszValue,pQuotaDef->Path,objPath.m_szPath);
	}
	break;

	case 3:
		DEBUG("%s", "Case3 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		pQuotaDef->EventNumber = ACE_OS::atoi(lpszValue);
		break;

	case 4:
		DEBUG("%s", "Case4 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		pQuotaDef->QuotaLimit = ACE_OS::atoi(lpszValue);
		break;

	case 5:
		DEBUG("%s", "Case5 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		pQuotaDef->A1AlarmLevel = ACE_OS::atoi(lpszValue);
		break;

	case 6:
		DEBUG("%s", "Case6 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		pQuotaDef->A1CeaseLevel = ACE_OS::atoi(lpszValue);
		break;

	case 7:
		DEBUG("%s", "Case7 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		pQuotaDef->A2AlarmLevel = ACE_OS::atoi(lpszValue);
		break;

	case 8:
		DEBUG("%s", "Case8 :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		pQuotaDef->A2CeaseLevel = ACE_OS::atoi(lpszValue);
		break;

	default:
		DEBUG("%s", "Default :  ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");
		return false;
	}
	DEBUG("%s", "Leaving ACS_SSU_FolderQuotaMon::SetFileMQuotaDefValue()");

	return true;

}
//! Set the value of the parameter
BOOL ACS_SSU_FolderQuotaMon::SetQuotaDefValue(const ACE_TCHAR* lpszValue,
		const ACE_INT32 nFieldIdx,
		SSU_FOLDERQUOTA_MONITOR* pQuotaDef)
{
	switch (nFieldIdx)
	{
	case 1:
		(void)ACE_OS::strncpy(pQuotaDef->ConfigType, lpszValue, 24);
		break;
	case 2:
	{
		(void)ACE_OS::strncpy(pQuotaDef->symName,lpszValue, 127);
		ACS_SSU_Path objPath;
		objPath.Expand(lpszValue);
		(void)ACE_OS::strncpy(pQuotaDef->Path,objPath.m_szPath, 127);
		//DEBUG( " Path %s -> %s", lpszValue,pQuotaDef->Path);
	}
	break;

	case 3:
		pQuotaDef->EventNumber = ACE_OS::atoi(lpszValue);
		break;

	case 4:
		pQuotaDef->QuotaLimit = ACE_OS::atoi(lpszValue);
		break;

	case 5:
		pQuotaDef->A1AlarmLevel = ACE_OS::atoi(lpszValue);
		break;

	case 6:
		pQuotaDef->A1CeaseLevel = ACE_OS::atoi(lpszValue);
		break;

	case 7:
		pQuotaDef->A2AlarmLevel = ACE_OS::atoi(lpszValue);
		break;

	case 8:
		pQuotaDef->A2CeaseLevel = ACE_OS::atoi(lpszValue);
		break;

	default:
		return false;
	}

	return true;

}

//============================================================================
// Prepare and startup the Folder Quota supervision
//============================================================================
BOOL ACS_SSU_FolderQuotaMon::SetupFolderQuotaMonitor()
{
	DEBUG( "%s","Entering ACS_SSU_FolderQuotaMon::SetupFolderQuotaMonitor");
	ACE_INT32 status = -1;
	if(!bCheckFolderQuotaEnabled())
	{
		DEBUG("%s" ,"bCheckFolderQuotaEnabled failed due to missing groupquota flag in /etc/mtab file in active node");
		DEBUG( "%s","Leaving ACS_SSU_FolderQuotaMon::SetupFolderQuotaMonitor");
		return false;
	}

	if (!PrepareFolderQuotas())
	{
		DEBUG("%s" ,"PrepareFolderQuotas failed in active node");
		DEBUG( "%s","Leaving ACS_SSU_FolderQuotaMon::SetupFolderQuotaMonitor");
		return false;
	}

	vUpdateSubDirObjs();

	DEBUG( "%s"," Running QuotaCheck to Update Folder Configuration files");

	string szCommand = ACS_SSU_QUOTA_OFF_CMD;
	DEBUG( "%s"," Running QuotaCheck off command");
	ACE_INT32 s32rc = ACE_OS::system(ACS_SSU_QUOTA_OFF_CMD);


	if( s32rc != -1 )
	{
		s32rc = -1;
		szCommand = ACS_SSU_QUOTA_UPDATE_CMD;
		DEBUG( "%s"," Running QuotaCheck update command");
		//s32rc = ACE_OS::system(ACS_SSU_QUOTA_UPDATE_CMD);

		int infp, outfp;
		std::string cmd(ACS_SSU_QUOTA_UPDATE_CMD);
		if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0)
		{
			DEBUG("Error in executing the %s",cmd.c_str());
			return false;
		}
		DEBUG("PID of Job %d",pid_in_progress);
		waitpid(pid_in_progress,&status,0);

		close(infp);	//HX58640
		close(outfp);	//HX58640

		if (WIFEXITED(status) )
		{
			s32rc=WEXITSTATUS(status);
			pid_in_progress = -1;
			DEBUG("%s execution status = %d",cmd.c_str(),s32rc);
		}

		if( s32rc != -1 )
		{
			szCommand = ACS_SSU_QUOTA_ON_CMD;
			DEBUG( "%s"," Running QuotaCheck on command");
			s32rc = ACE_OS::system(ACS_SSU_QUOTA_ON_CMD);
		}
		else
		{
			DEBUG("%s execution status = %d",cmd.c_str(),s32rc);
			return false;
		}
	}
	std::string myPath("/data/aquota.group.new");
        if(ACS_SSU_Common::CheckFileExists(myPath.c_str()))
        {
                DEBUG("%s","Found  aquota.group.new file during SetupFolderQuotaMonitor")
                ACE_OS::system("rm -rf /data/aquota.group.new");
                DEBUG("%s","Removed aquota.group.new file succesfully present during SetupFolderQuotaMonitor");
        }
	if( s32rc == -1 )
	{
		DEBUG(" Running %s command failed with Error code",szCommand.c_str(),WEXITSTATUS(s32rc));
		return FALSE;
	}



	DEBUG(" SetupFolderQuotaMonitor Complete:%s","True");
	DEBUG( "%s","Leaving ACS_SSU_FolderQuotaMon::SetupFolderQuotaMonitor");
	return true;
}

BOOL ACS_SSU_FolderQuotaMon::bCheckFolderQuotaEnabled()
{
	FILE *fp;
	struct mntent *mnt;
	ACE_TCHAR pszDevice[ACS_SSU_PARITION_NAME_MAX];

	BOOL bResult = false;

	if ((fp = setmntent(MOUNTED, "r")) == NULL) {
		fprintf(stderr, "Error calling setmntent\n");
		bResult = false;
	}

	while((mnt = getmntent(fp)) != NULL)
	{
		if ( hasmntopt(mnt, SSU_QUOTASTRING) )
		{

			ACE_OS::snprintf(pszDevice,ACS_SSU_PARITION_NAME_MAX,"%s",mnt->mnt_fsname);

			//! @todo Remove ACS_SSU_DATA_DEVICE once LCTBIN provides the interface to fetch data device
			if( ACE_OS::strcmp(pszDevice, SSU_ACS_DATAPATH ) )
			{
				//! Quota settings on data disk are enabled
				bResult = true;
				break;
			}
		}
	}

	if( bResult == false )
	{
		INFO("bCheckFolderQuotaEnabled: Quota Not enabled: bResult:%d", bResult);
	}

	return bResult;

}

void ACS_SSU_FolderQuotaMon::vSendMessageToQuotaThreads( enum NotificationType enType )
{
	ACE_INT32 Wait = 10;
        ACE_Time_Value oTimeValue(Wait);
        ACS_SSU_FolderQuotaMon::m_FolderLock.acquire(&oTimeValue);
	DEBUG("%s","Acquired ACS_SSU_FolderQuotaMon thread in vSendMessageToQuotaThreads()");

	ACS_SSU_FolderQuotaList::const_iterator it = m_listFolderQuotas.begin();

	ACE_UINT32 u32Message = (ACE_UINT32)enType;

	for( ;it!=m_listFolderQuotas.end(); ++it)
	{
		ACS_SSU_FolderQuota* pFolderQuota = *it;

		ACE_Message_Block *mb;
		ACE_NEW(mb,ACE_Message_Block(sizeof(ACE_UINT32)));
		mb->copy((ACE_TCHAR*)&u32Message,sizeof(ACE_UINT32));
		pFolderQuota->putq(mb);
	}

	//FileM Change
	it = m_listFileMFolderQuotas.begin();

	u32Message = (ACE_UINT32)enType;

	for( ;it!=m_listFileMFolderQuotas.end(); ++it)
	{
		ACS_SSU_FolderQuota* pFolderQuota = *it;

		ACE_Message_Block *mb;
		ACE_NEW(mb,ACE_Message_Block(sizeof(ACE_UINT32)));
		mb->copy((ACE_TCHAR*)&u32Message,sizeof(ACE_UINT32));
		pFolderQuota->putq(mb);
	}
	DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread in vSendMessageToQuotaThreads()");
	 ACS_SSU_FolderQuotaMon::m_FolderLock.release();

}

//! Add subdirectory objects to the each directory element
void ACS_SSU_FolderQuotaMon::vUpdateSubDirObjs()
{
	ACE_INT32 Wait = 10;
        ACE_Time_Value oTimeValue(Wait);
        ACS_SSU_FolderQuotaMon::m_FolderLock.acquire(&oTimeValue);
	DEBUG("%s","Acquired ACS_SSU_FolderQuotaMon thread in vUpdateSubDirObjs()");
	ACS_SSU_FolderQuotaList::iterator it1 = m_listFolderQuotas.begin();

	//! For each directory element
	for(;it1!=m_listFolderQuotas.end(); ++it1)
	{
		(*it1)->vRemoveSubDirObjs();
		ACS_SSU_FolderQuotaList::iterator it2 = m_listFolderQuotas.begin();

		//! Browse through other directory objects
		for(;it2!=m_listFolderQuotas.end(); ++it2)
		{
			//! If directory objects are not same
			if( ACE_OS::strcmp((*it1)->get_Path(),(*it2)->get_Path()) )
			{
				const ACE_TCHAR *pszDiffer = ACE_OS::strstr((*it2)->get_Path(),(*it1)->get_Path());

				//! And if other directory is subdirectory add object to the list
				if( pszDiffer == (*it2)->get_Path())
				{
					if(ACE_OS::strlen((*it2)->get_Path())>ACE_OS::strlen((*it1)->get_Path()))
						(*it1)->vAddSubDirObj(*it2);
				}
			}
		}

	}

	//FileM Change	
	it1 = m_listFileMFolderQuotas.begin();

	//! For each directory element
	for(;it1!=m_listFileMFolderQuotas.end(); ++it1)
	{
		(*it1)->vRemoveSubDirObjs();
		ACS_SSU_FolderQuotaList::iterator it2 = m_listFileMFolderQuotas.begin();

		//! Browse through other directory objects
		for(;it2!=m_listFileMFolderQuotas.end(); ++it2)
		{
			//! If directory objects are not same
			if( ACE_OS::strcmp((*it1)->get_Path(),(*it2)->get_Path()) )
			{
				const ACE_TCHAR *pszDiffer = ACE_OS::strstr((*it2)->get_Path(),(*it1)->get_Path());

				//! And if other directory is subdirectory add object to the list
				if( pszDiffer == (*it2)->get_Path())
				{
					if(ACE_OS::strlen((*it2)->get_Path())>ACE_OS::strlen((*it1)->get_Path()))
						(*it1)->vAddSubDirObj(*it2);
				}
			}
		}

	}
	DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread in vUpdateSubDirObjs()");
	ACS_SSU_FolderQuotaMon::m_FolderLock.release();
}

void ACS_SSU_FolderQuotaMon::getFolderSettings(const char* objName, SSU_FOLDER_OBJ&  folderObject)
{
	std::string str(objName);
	size_t found;
	found = str.find_first_of("=");
	str = str.substr(found+1).c_str();

	found = str.find_first_of(",");
	str = str.substr(0,found).c_str();

	DEBUG("Folder Symbolic Name %s",str.c_str());

	ACE_TCHAR szBuffer[1024];

	ACE_INT32 Wait = 10;
        ACE_Time_Value oTimeValue(Wait);
        ACS_SSU_FolderQuotaMon::m_FolderLock.acquire(&oTimeValue);
	DEBUG("%s","Acquired ACS_SSU_FolderQuotaMon thread in getFolderSettings()");

	ACS_SSU_FolderQuotaList::iterator iter = m_listFolderQuotas.begin();
	while (iter != m_listFolderQuotas.end())
	{
		(*iter)->print();
		if ((*iter)->FolderIsAvailable())
		{
			if ((*iter)->GetQuotaValue(szBuffer))
			{
				if (ACE_OS::strcmp((*iter)->get_symbolicName(),str.c_str()) == 0)
				{
					ACE_OS::strcpy(folderObject.symName,(*iter)->get_symbolicName());

					folderObject.QuotaLimit = (*iter)->getFolderLimit();
					folderObject.CurrentConsumption = (*iter)->getFolderCurrentConsumption();
					folderObject.A1AlarmLevel = (*iter)->getA1AlarmLevel();
					folderObject.A1CeaseLevel = (*iter)->getA1CeaseLevel();
					folderObject.A2AlarmLevel = (*iter)->getA2AlarmLevel();
					folderObject.A2CeaseLevel = (*iter)->getA2CeaseLevel();
				}
			}
		}
		++iter;
	}
	
	//FileM Change
	iter = m_listFileMFolderQuotas.begin();
    while (iter != m_listFileMFolderQuotas.end())
    {
        (*iter)->print();
        if ((*iter)->FolderIsAvailable())
        {
            if ((*iter)->GetQuotaValue(szBuffer))
            {
                if (ACE_OS::strcmp((*iter)->get_symbolicName(),str.c_str()) == 0)
                {
                    ACE_OS::strcpy(folderObject.symName,(*iter)->get_symbolicName());

                    folderObject.QuotaLimit = (*iter)->getFolderLimit();
                    folderObject.CurrentConsumption = (*iter)->getFolderCurrentConsumption();
                    folderObject.A1AlarmLevel = (*iter)->getA1AlarmLevel();
                    folderObject.A1CeaseLevel = (*iter)->getA1CeaseLevel();
                    folderObject.A2AlarmLevel = (*iter)->getA2AlarmLevel();
                    folderObject.A2CeaseLevel = (*iter)->getA2CeaseLevel();
                }
            }
        }
        ++iter;
    }
	DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread in getFolderSettings()");
        ACS_SSU_FolderQuotaMon::m_FolderLock.release();

}

//! How to send the error code to IMM if config cannot be changed
ACE_INT32 ACS_SSU_FolderQuotaMon::enCheckConfigChange( const ACE_TCHAR * szNewConfigType )
{
	ACE_INT32 Wait = 10;
	ACE_Time_Value oTimeValue(Wait);
	ACS_SSU_FolderQuotaMon::m_FolderLock.acquire(&oTimeValue);
	DEBUG("%s","Acquired ACS_SSU_FolderQuotaMon thread in enCheckConfigChange()");
	ACE_INT32 s32Result = SSU_RC_OK;
	//! This command can only be executed from the active node
	/*! @todo commented for testing, need to un comment */

	if(!m_bIsPrepared)
	{
		DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread m_bIsPrepared");
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		return SSU_RC_SERVICEISBUSY;
	}

	if (!ACS_SSU_Common::CheckIfActiveNode())
	{
		m_bIsActiveNode = false;
		DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread CheckIfActiveNode");
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		return SSU_RC_NOTACTIVENODE;
	}

	if (!szNewConfigType)
	{
		DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread szNewConfigType");
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		return SSU_RC_INVALIDARGUMENT;
	}

	WARNING("Old Configuration = %s New Configuration = %s",m_szConfigType,szNewConfigType);

	if( ACE_OS::strcmp(m_szConfigType, szNewConfigType) == 0 )
	{
		ACE_TCHAR szData[512];
		ACE_OS::strcpy(szData, ACE_TEXT("The current configuration is already "));
		ACE_OS::strcat(szData, m_szConfigType);
		mErrorCurrentConfig = szData;
		WARNING ("%s", "Both Configurations are same. No need to Modify\n");
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		return SSU_RC_CONFIGTYPEALREADYEXIST;
	}

	//! Read the new quotas settings from the quota file
	ACS_SSU_QuotasList listQuotas;
	if ( ReadQuotas(listQuotas, szNewConfigType, false ) == false)
	{
		EraseQuotaList(listQuotas);
		//		cout<<"Releasing lock in enCheckConfigChange()::if( ReadQuotas(listQuotas, szNewConfigType, false ) == false)"<<endl;
		DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread ReadQuotas");
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		return SSU_RC_QUOTAFILEERROR;
	}

	SSU_FOLDERQUOTA_MONITOR* pQuota;
	//! Walk through all quotas object and check if we can change the configuration,
	//! i.e. check if the new limits, alarm limits, etc. dont exceed the current values
	ACS_SSU_QuotasList::iterator QuotasIter = listQuotas.begin();
	BOOL bSuccess = true;
	BOOL bFound = false;
	while (bSuccess && QuotasIter != listQuotas.end())
	{
		bFound = false;
		pQuota = *QuotasIter;
		ACS_SSU_FolderQuotaList::iterator FolderQuotasIter;
		FolderQuotasIter = m_listFolderQuotas.begin();
		while (!bFound && FolderQuotasIter != m_listFolderQuotas.end())
		{
			//! Check if this folder is supervised by us
			if ((*FolderQuotasIter)->IsEqual(pQuota->Path))
			{
				//! Change the folder quota configuration
				DEBUG("Folder Path = %s",pQuota->Path);
				ACE_TCHAR szData[SSU_CHANGE_CONFIG_STATUS_TEXT_LENGTH];
				bSuccess = (*FolderQuotasIter)->CheckConfig(pQuota, szData);
				//                         cout<<"status of config check for "<< pQuota->Path<<" is :"<<bSuccess<<endl;
				INFO("New space allocation limit: %u",(ACE_UINT64)pQuota->QuotaLimit * (ACE_UINT64)ONE_MB);
				INFO("Current folder size: %u",(*FolderQuotasIter)->getFolderCurrentConsumption());
				INFO("Folder Name: %s",(*FolderQuotasIter)->get_Path());
				ACE_TCHAR mnewSpaceLimit[100];
				ACE_TCHAR mcharCurrConsumption[100];
				std::string mErrorText;
				if(!bSuccess)
				{
					ACE_OS::sprintf(mnewSpaceLimit,"%ld",(ACE_UINT64)pQuota->QuotaLimit*(ACE_UINT64)ONE_MB);
					ACE_OS::sprintf(mcharCurrConsumption,"%ld",((*FolderQuotasIter)->getFolderCurrentConsumption()));
					mErrorText.clear();
					mErrorText.append("The space currently allocated by folder ");
					mErrorText.append((*FolderQuotasIter)->get_Path());
					mErrorText.append(" prevents the change of folder quota configuration.\nCurrent folder size: ");
					mErrorText.append(mcharCurrConsumption);
					mErrorText.append("\nNew space allocation limit: ");
					mErrorText.append(mnewSpaceLimit);
					//mErrorChange="The space currently allocated by folder "+((*FolderQuotasIter)->get_Path())+" prevents the change of folder quota configuration.\nCurrent folder size: "+mcharCurrConsumption+"\nNew space allocation limit: "+mnewSpaceLimit+"\n";
					mErrorChange = mErrorText;
				}
				DEBUG("Status of configuration check:%d",bSuccess);
				bFound = true;
			}
			else
				++FolderQuotasIter;
		}
		++QuotasIter;
	}
	//! The current quota will exceed the new limit
	if (!bSuccess)
	{
		EraseQuotaList(listQuotas);
		//		cout<<"Releasing lock at ACS_SSU_FolderQuotaMon::enCheckConfigChange() end "<<endl;
		DEBUG("%s","Release ACS_SSU_FolderQuotaMon thread bSuccess");
		ACS_SSU_FolderQuotaMon::m_FolderLock.release();
		return SSU_RC_NEWQUOTAEXCEEDED;
	}
	return s32Result;
}

int ACS_SSU_FolderQuotaMon::getDataDiskSize()
{
	WARNING("%s","Entering ACS_SSU_FolderQuotaMon::getDataDiskSize");
	int size=0;
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	//myAPGCCCommonLib.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION | ACS_APGCC_GET_DISKCAPACITY );
	myAPGCCCommonLib.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_DISKCAPACITY );
	DEBUG("In getDiskCapacity function disk capacity: %d",hwInfo.disksCapacity);

	if(hwInfoResult.disksCapacityResult == ACS_APGCC_HWINFO_SUCCESS)
	{
		WARNING("%s","GetHWInfo success");
		switch(hwInfo.disksCapacity)
		{
		case ACS_APGCC_DISKSCAPACITY_147GB:
			size = SSU_147_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_300GB:
			size = SSU_300_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_450GB:
			size = SSU_450_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_600GB:
			size = SSU_600_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_400GB:
			size = SSU_GEP5_400_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_1200GB:
			size = SSU_GEP5_1200_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_250GB:
			size = SSU_250_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_700GB:
			size = SSU_700_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_1000GB:
			size = SSU_1000_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_1300GB:
                        size = SSU_1300_DISK_SIZE;
                        break;

		case ACS_APGCC_DISKSCAPACITY_1400GB:
			size = SSU_1400_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_1450GB:
			size = SSU_1450_DISK_SIZE;
			break;

		case ACS_APGCC_DISKSCAPACITY_1600GB:
			size = SSU_GEP7L_1600_DISK_SIZE;
			break;

		default:
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getDataDiskSize");
			return -1;
		}
		WARNING("size is = %d",size);


	}
	else{
		WARNING("%s","gethwinfo failed");
	}
	return size;
}

ACE_TCHAR* ACS_SSU_FolderQuotaMon::getConfigString(ACE_INT32 type, ACE_INT32 system, ACE_INT32 nodeNum, ACE_INT32 size,      							 const ACE_TCHAR* boardType)
{
	WARNING("%s","Entering ACS_SSU_FolderQuotaMon::getConfigString");
	WARNING("Application type is = %d",type); 
switch(type)
	{
	case 0:
		// MSC
		if (system == 0)
		{
			// SINGLE CP
			// Updated for vMSC-300 and vMSC-147
			DEBUG("The boardType identified by SSU is %s",boardType);
			if ((ACE_OS::strcmp(boardType,SSU_HWVER_VM_300STRING) == 0) || (ACE_OS::strcmp(boardType,SSU_HWVER_VM_147STRING) == 0))
			{
				if ( nodeNum == 1 && size >= 300 )
				{
					// if node is AP1 and data disk size is more than equal to 300 GB - vMSC-300
					DEBUG("The Configuration identified by SSU is %s",SSU_vMSC_300_AP1_CONFIG);
					DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_vMSC_300_AP1_CONFIG;
				}
				else if ( nodeNum == 1 && size >= 147 )
				{
					// if node is AP1 and data disk size is more than equal to 147 GB - vMSC-147
					DEBUG("The Configuration identified by SSU is %s",SSU_vMSC_147_AP1_CONFIG);
					DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_vMSC_147_AP1_CONFIG;

				}
			}
			if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_400STRING)== 0)
			{
				if ( nodeNum == 1 )
				{
					// if node is AP1 and baord type is GEP7L-400 - MSC-400-AP1
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP1_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP1_CONFIG;
				}
				else if ( nodeNum == 2)
				{
					// if node is AP2 and data disk size is GEP7L-400 - MSC-400-AP2
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP2_CONFIG;
				}
			}
			else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_1600STRING)== 0)
			{
				if ( nodeNum == 1 )
				{
					// if node is AP1 and baord type is GEP7L-1600 - MSC-400-AP1
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP1_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP1_CONFIG;
				}
				else if ( nodeNum == 2)
				{
					// if node is AP2 and data disk size is GEP7L-1600 - MSC-400-AP2
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP2_CONFIG;
				}
			}
			if ((ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)!=0) 
					&& (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)!= 0))
			{
				if ( nodeNum == 2 && size >= 147)
				{
					// if node is AP2 and data disk size is 147 GB - MSC-147-AP2
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_147_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_147_AP2_CONFIG;
				}
				else
				{
					// configuration is MSC
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_CONFIG;

				}
			}
			else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)== 0)
			{
				if ( nodeNum == 1 )
				{
					// if node is AP1 and baord type is GEP5-400 - MSC-400-AP1
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP1_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP1_CONFIG;
				}

				else if ( nodeNum == 2)
				{
					// if node is AP2 and data disk size is GEP-400 - MSC-400-AP2
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP2_CONFIG;
				}
			}
			else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)== 0)
			{
				if ( nodeNum == 1 )
				{
					// if node is AP1 and baord type is GEP5-1200 - MSC-400-AP1
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP1_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP1_CONFIG;
				}

				else if ( nodeNum == 2)
				{
					// if node is AP2 and data disk size is GEP-1200 - MSC-400-AP2
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_400_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_400_AP2_CONFIG;
				}
			}
		}
		else
		{
			// MULTI CP
			if((nodeNum == 1))
			{
				if ((ACE_OS::strcmp(boardType,SSU_HWVER_VM_700STRING)==0)&& size == 700)
				{
					// if data disk size is equal to 700 GB - vMSCHC_SMALL_700
					DEBUG("The Configuration identified by SSU is %s",SSU_vMSCHC_SMALL_700_CONFIG );
					DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_vMSCHC_SMALL_700_CONFIG ;
				}
				else if((ACE_OS::strcmp(boardType,SSU_HWVER_VM_1000STRING)==0)&& size == 1000)
				{
					// if data disk size is equal to 1000 GB - vMSCHC_MEDIUM_1000
					DEBUG("The Configuration identified by SSU is %s",SSU_vMSCHC_MEDIUM_1000_CONFIG );
					DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_vMSCHC_MEDIUM_1000_CONFIG ;
				}
				else if((ACE_OS::strcmp(boardType,SSU_HWVER_VM_1300STRING)==0)&& size == 1300)
                                {
                                        // if data disk size is equal to 1300 GB - vMSCHC_LARGE_1300
                                        DEBUG("The Configuration identified by SSU is %s",SSU_vMSCHC_LARGE_1300_CONFIG );
                                        DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
                                        return (char *) SSU_vMSCHC_LARGE_1300_CONFIG ;
                                }

				else if((ACE_OS::strcmp(boardType,SSU_HWVER_VM_1400STRING)==0)&& size == 1400)
				{
					// if data disk size is equal to 1400 GB - vMSCHC_LARGE_1400
					DEBUG("The Configuration identified by SSU is %s",SSU_vMSCHC_LARGE_1400_CONFIG );
					DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_vMSCHC_LARGE_1400_CONFIG ;
				}
				else if ((ACE_OS::strcmp(boardType,SSU_HWVER_VM_1450STRING)==0)&& size == 1450)
				{
					// if data disk size is equal to 1450 GB - vMSCHC_LARGE_1450
					DEBUG("The Configuration identified by SSU is %s",SSU_vMSCHC_LARGE_1450_CONFIG );
					DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_vMSCHC_LARGE_1450_CONFIG ;
				}
				else if ((ACE_OS::strcmp(boardType,SSU_HWVER_VM_250STRING)==0)||(ACE_OS::strcmp(boardType,SSU_HWVER_VM_147STRING)==0)||
						(ACE_OS::strcmp(boardType,SSU_HWVER_VM_300STRING)==0)||(ACE_OS::strcmp(boardType,SSU_HWVER_VM_STRING)==0))
				{
					DEBUG("The Configuration identified by SSU is %s",SSU_vMSC_147_AP1_CONFIG);
					DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_vMSC_147_AP1_CONFIG;
				}
			}
			if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_1600STRING)==0)
			{
				if(nodeNum == 1)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_BSP_MSC_BC_AP1_1200_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_BSP_MSC_BC_AP1_1200_CONFIG;
				}
				else if (nodeNum == 2)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_BSP_MSC_BC_AP2_1200_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_BSP_MSC_BC_AP2_1200_CONFIG;
				}
			}
			else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_400STRING)==0)
			{
				if(nodeNum == 1)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_BC_AP1_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_BC_AP1_CONFIG;
				}
				else if (nodeNum == 2)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_BC_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_BC_AP2_CONFIG;
				}
			}

			if ((nodeNum == 1) && ((ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)!=0)
					&& (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)!= 0)))
			{
				// FOR AP1
				if ( size == 600 )  
				{
					// FOR MSCBC-AP1-8M600
					WARNING("The Configuration identified by SSU is %s",SSU_MSCBC_AP1_8M600_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSCBC_AP1_8M600_CONFIG;

				}
				else if ( size == 450)
				{
					// FOR MSCBC-AP1-450
					WARNING("The Configuration identified by SSU is %s",SSU_MSCBC_AP1_8M450_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSCBC_AP1_8M450_CONFIG;
				}
				else if ( size == 300)
				{
					// MSC-BC-AP1
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_BC_AP1_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_BC_AP1_CONFIG;
				}
			}
			else if ((nodeNum == 2) && ((ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)!=0)
					&& (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)!= 0)))

			{
				// FOR AP2
				if (size == 600)
				{
					// FOR MSCBC-AP2-8M600
					WARNING("The Configuration identified by SSU is %s",SSU_MSCBC_AP2_8M600_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSCBC_AP2_8M600_CONFIG;
				}
				else if (size == 450)
				{
					// FOR MSCBC-AP2-8M450
					WARNING("The Configuration identified by SSU is %s",SSU_MSCBC_AP2_8M450_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSCBC_AP2_8M450_CONFIG;
				}
				else if (size == 300)
				{
					// FOR MSC-BC-AP2
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_BC_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_BC_AP2_CONFIG;
				}
			}
			else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)==0)
			{
				if(nodeNum == 1)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_BSP_MSC_BC_AP1_1200_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_BSP_MSC_BC_AP1_1200_CONFIG;
				}
				else if (nodeNum == 2)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_BSP_MSC_BC_AP2_1200_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_BSP_MSC_BC_AP2_1200_CONFIG;
				}
			}
			else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)==0)  //Added for TR:HT15424 
			{
				if(nodeNum == 1)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_BC_AP1_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_BC_AP1_CONFIG;
				}
				else if (nodeNum == 2)
				{
					WARNING("The Configuration identified by SSU is %s",SSU_MSC_BC_AP2_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_MSC_BC_AP2_CONFIG;
				}
			}


		}
		break;
	case 1:
		// HLR
		// Configuration for VM
		if((ACE_OS::strcmp(boardType,SSU_HWVER_VM_250STRING)==0)||(ACE_OS::strcmp(boardType,SSU_HWVER_VM_300STRING)==0)||
        (ACE_OS::strcmp(boardType,SSU_HWVER_VM_STRING)==0))
    {
      //check whether node is MCP and data disk size is greater than 250 GB
      if ((system == 1) && (size>=250) )
      {
        WARNING("The Configuration identified by SSU is %s",SSU_vHLR_BS_250_CONFIG);
        WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
        return (char *) SSU_vHLR_BS_250_CONFIG;
      }
      else
      {
    	  WARNING("The Configuration identified by SSU is %s",SSU_HLR_CONFIG);
    	  WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
    	  return (char *) SSU_HLR_CONFIG;
      }
    }
        else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_1600STRING)==0)
        {
        	if (system == 0)
        	{
        		WARNING("The Configuration identified by SSU is %s",SSU_HLR_SS_1200_CONFIG);
        		WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
        		return (char *) SSU_HLR_SS_1200_CONFIG;
        	}
        	else
        	{
        		WARNING("The Configuration identified by SSU is %s",SSU_BSP_HLR_BS_1200_CONFIG);
        		WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
        		return (char *) SSU_BSP_HLR_BS_1200_CONFIG;
        	}
        }
		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_400STRING)==0)
		{
			if (system == 0)
			{
				WARNING("The Configuration identified by SSU is %s",SSU_HLR_SS_CONFIG);
				WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_HLR_SS_CONFIG;
			}
			else
			{
				WARNING("The Configuration identified by SSU is %s",SSU_HLR_BS_CONFIG);
				WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_HLR_BS_CONFIG;
			}
		}
		else if ( (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)!= 0)
				&& (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)!=0))
		{
			// SINGLE CP
			if (system == 0)
			{	
				if (size == 147)
				{
					// FOR HLR
					WARNING("The Configuration identified by SSU is %s",SSU_HLR_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_HLR_CONFIG;
				}
				else /*if (size == 300)*///commented for HLR DD size issue
				{
					// FOR HLR_SS
					WARNING("The Configuration identified by SSU is %s",SSU_HLR_SS_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_HLR_SS_CONFIG;
				}
			}
			else
			{
				if (size >= 450)
				{

					// MULTI CP - HLR_BS
					WARNING("The Configuration identified by SSU is %s",SSU_HLR_BS_CONFIG);
					WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
					return (char *) SSU_HLR_BS_CONFIG;
				}
			}
		}
		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)==0)
		{
			if (system == 0)
			{
				WARNING("The Configuration identified by SSU is %s",SSU_HLR_SS_1200_CONFIG);
				WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_HLR_SS_1200_CONFIG;
			}
			else
			{
				WARNING("The Configuration identified by SSU is %s",SSU_BSP_HLR_BS_1200_CONFIG);
				WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_BSP_HLR_BS_1200_CONFIG;
			}
		}
		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)==0) //Added for TR:HT15424
		{
			if (system == 0)
			{
				WARNING("The Configuration identified by SSU is %s",SSU_HLR_SS_CONFIG);
				WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_HLR_SS_CONFIG;
			}
			else
			{
				WARNING("The Configuration identified by SSU is %s",SSU_HLR_BS_CONFIG);
				WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_HLR_BS_CONFIG;
			}
		}
		break;
	case 2:
		if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_400STRING)== 0)
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_BSC_400_CONFIG);
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_BSC_400_CONFIG;
		}
		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_1600STRING)== 0)
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_BSC_400_CONFIG);
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_BSC_400_CONFIG;
		}
		else if ((ACE_OS::strcmp(boardType,SSU_HWVER_VM_300STRING)== 0)
				    || (ACE_OS::strcmp(boardType,SSU_HWVER_VM_250STRING)== 0))
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_vBSC_300_CONFIG);
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_vBSC_300_CONFIG;
		}

		if ((ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)!= 0)
				&& (strcmp(boardType,SSU_HWVER_GEP5_1200STRING)!=0))

		{		// BSC
			WARNING("The Configuration identified by SSU is  %s",SSU_BSC_CONFIG);
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_BSC_CONFIG;
		}

		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)== 0)
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_BSC_400_CONFIG);
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_BSC_400_CONFIG;
		}

		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)== 0)
		{

			WARNING("The Configuration identified by SSU is  %s",SSU_BSC_400_CONFIG);
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_BSC_400_CONFIG;
		}
		break;

	case 3:
	        // WIRELINE /*start of WLN GEP5 HW support*/
                if ( (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)!= 0)
                                && (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)!=0)
								&& (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_400STRING)!=0)
								&& (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_1600STRING)!=0))
                {
                   //Restricting WLN-147 Only for GEP1/GEP2 configurations
                   //Added condition for GEP7 as it should return default BSC configuration if it is a GEP7 HW
                   if (system == 0)
                       {
                         // SINGLE CP
                         //INCLUDED FOR WIRELINE
                          if ( nodeNum == 1 && size >= 147 )
                             {
                               // if node is AP1 and data disk size is greater than or equal to 147 GB
                                WARNING("The Configuration identified by SSU is  %s",SSU_WLN_147_AP1_CONFIG);
                                WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
                                return (char *) SSU_WLN_147_AP1_CONFIG;
                             }
                       }
                }
                else if ((ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING) == 0)
                                || (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING) == 0))
                {
                	//Restricting WLN-400-AP1 to GEP5-400 and GEP5-1200 HW only
                	if (system == 0)
                	{
                		// SINGLE CP
                		//same configuration for both GEP5-400 and GEP5-1200 as WLN is a single CP
                		if (nodeNum == 1 && size >= 400 )
                		{
                			// if node is AP1 and data disk size is greater than or equal to 400 GB
                			WARNING("The Configuration identified by SSU is  %s",SSU_WLN_400_AP1_CONFIG);
                			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
                			return (char *) SSU_WLN_400_AP1_CONFIG;
                		}
                	}

                }/*end of WLN GEP5 HW support*/
                WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString WIRELINE");
                break;         
	case 4:
		// TSC
		if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_1600STRING)== 0)
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_HLR_TSC_1200_CONFIG );
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_HLR_TSC_1200_CONFIG;
		}
		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP7L_400STRING)== 0)
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_HLR_TSC_CONFIG );
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_HLR_TSC_CONFIG;
		}
		else if ((ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)!= 0)
				&& (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)!=0) && (size == 300))

		{
			WARNING("The Configuration identified by SSU is  %s",SSU_HLR_TSC_CONFIG);
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_HLR_TSC_CONFIG;
		}

		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_1200STRING)== 0)
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_HLR_TSC_1200_CONFIG );
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_HLR_TSC_1200_CONFIG;
		}

		else if (ACE_OS::strcmp(boardType,SSU_HWVER_GEP5_400STRING)== 0) //Added for TR:HT15424
		{
			WARNING("The Configuration identified by SSU is  %s",SSU_HLR_TSC_CONFIG );
			WARNING("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
			return (char *) SSU_HLR_TSC_CONFIG;
		}


		break;
	case 5:
		//IP-STP
		if (system == 0) //for Single CP
		{
			DEBUG("The boardType identified by SSU is %s",boardType);
			if (ACE_OS::strcmp(boardType,SSU_HWVER_VM_147STRING) == 0)
			{
				DEBUG("The Configuration identified by SSU is %s",SSU_vIPSTP_147_AP1_CONFIG);
				DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_vIPSTP_147_AP1_CONFIG;
			}
			else //Added for IPSTP SCP
			{
				DEBUG("The Configuration identified by SSU is %s",SSU_IPSTP_CONFIG);
				DEBUG("%s","Leaving ACS_SSU_FolderQuotaMon::getConfigString");
				return (char *) SSU_IPSTP_CONFIG;
			}

		}
		break;

	default:
		break;
	}

	WARNING("None of the configurations is read by SSU.SSU is setting the default configuration  %s",SSU_BSC_CONFIG);
	return (char *) SSU_BSC_CONFIG;

}


void iNotifyThread(void *aPtr)
{
	DEBUG("%s", "Inside iNotifyThread");
	int fd,wd,len=0,i=0;
	ACE_INT32 status = -1;
	char buffer[1024];
	fd = inotify_init();
	if(fd == -1)
	{
		DEBUG("%s","Failed to initialize inotify thread");
		return;
	}
	struct inotify_event *event;
	wd = inotify_add_watch(fd,"/data", IN_CREATE | IN_DELETE);
	if(wd == -1)
	{
		ACE_OS::close(fd);
		DEBUG("%s","Failed to add watch inotify thread");
		DEBUG("The error number is %d", errno); 
		return;
	}
	while(stopINotifyThread == false)
	{
		i=0;
		len=read(fd,buffer,1024);
		while(i<len)
		{
			event = (struct inotify_event * ) &buffer[i];
			if (event->mask & IN_DELETE)
			{
				if (event->mask & IN_ISDIR)
				{
					DEBUG("Folder is being deleted %s ", event->name);
				}
				else
				{
					DEBUG("File is being deleted %s ",event->name );
				}
			}
			if(event->mask & IN_CREATE)
			{
				if (event->mask & IN_ISDIR)
				{
					DEBUG("Folder is being created %s ", event->name);
				}
				else
				{
					std::string myStrs= event->name;
					std::string myStrd = "aquota.group.new";
					if(! strcmp(myStrs.c_str(), myStrd.c_str()))
					{
						//Check foraquota.group.new  file existence
						std::string myPath("/data/aquota.group.new");
						if(ACS_SSU_Common::CheckFileExists(myPath.c_str()))
						{
							DEBUG("%s","aquota.group.new file created and exists");
							ACE_OS::system("rm -rf /data/aquota.group.new");
							DEBUG( "%s"," Running QuotaCheck to Update Folder Configuration files");
							string szCommand = ACS_SSU_QUOTA_OFF_CMD;
							DEBUG( "%s"," Running QuotaCheck off command");
							ACE_INT32 s32rc = ACE_OS::system(ACS_SSU_QUOTA_OFF_CMD);
							if( s32rc != -1 )
							{
								s32rc = -1;
								szCommand = ACS_SSU_QUOTA_UPDATE_CMD;
								DEBUG( "%s"," Running QuotaCheck update command");
								int infp, outfp;
								std::string cmd(ACS_SSU_QUOTA_UPDATE_CMD);
								if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0)
								{
									DEBUG("Error in executing the %s",cmd.c_str());
									return;
								}
								close(infp);
                                				close(outfp);

								DEBUG("PID of Job %d",pid_in_progress);
								waitpid(pid_in_progress,&status,0);

								if (WIFEXITED(status) )
								{
									s32rc=WEXITSTATUS(status);
									pid_in_progress = -1;
									DEBUG("%s execution status = %d",cmd.c_str(),s32rc);
								}

								if( s32rc != -1 )
								{
									szCommand = ACS_SSU_QUOTA_ON_CMD;
									DEBUG( "%s"," Running QuotaCheck on command");
									s32rc = ACE_OS::system(ACS_SSU_QUOTA_ON_CMD);
								}
								else
								{
									DEBUG("%s execution status = %d",cmd.c_str(),s32rc);
									return;
								}
							}

							if( s32rc == -1 )
							{
								DEBUG(" Running %s command failed with Error code %d",szCommand.c_str(),WEXITSTATUS(s32rc));
								return;
							}
						}
					}
				}
			}
			i+=sizeof(struct inotify_event)+event->len;
		}
	}

	if(inotify_rm_watch(fd,wd) == -1)
	{
		DEBUG("%s","Failed to remove watch on the inotify directory");
	}
	ACE_OS::close(fd);

}

ACE_TCHAR* ACS_SSU_FolderQuotaMon::getBoardType()
{
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	myAPGCCCommonLib.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION | ACS_APGCC_GET_DISKCAPACITY );

	if(hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
		DEBUG("hwversion is : %d", hwInfo.hwVersion);
		DEBUG("diskCapacity is : %d", hwInfo.disksCapacity);
		switch(hwInfo.hwVersion){
		case ACS_APGCC_HWVER_GEP1:
			ACE_OS::strcpy(myGEPVersion,SSU_HWVER_GEP1_STRING);
			WARNING("Gepversion is : %s", myGEPVersion)
			break;

		case ACS_APGCC_HWVER_GEP2:
			ACE_OS::strcpy(myGEPVersion,SSU_HWVER_GEP2_STRING);
			WARNING("Gepversion is : %s", myGEPVersion);
			break;

		case ACS_APGCC_HWVER_GEP5:
			if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_400GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_GEP5_400STRING);
				WARNING("Gepversion is : %s", myGEPVersion);
			}
			else {
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_GEP5_1200STRING);
				WARNING("Gepversion is : %s", myGEPVersion);
			}
			break;
		case ACS_APGCC_HWVER_GEP7:
			if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_400GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_GEP7L_400STRING);
				WARNING("Gepversion is : %s", myGEPVersion);
			}
			else {
				if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_1600GB) {
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_GEP7L_1600STRING);
				WARNING("Gepversion is : %s", myGEPVersion);
				}
			}
			break;
			//	Updated for vMSC-300 and vMSC-147
		case ACS_APGCC_HWVER_VM:
			if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_300GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_300STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_147GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_147STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_250GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_250STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_700GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_700STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_1000GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_1000STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_1300GB){
                                ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_1300STRING);
                                DEBUG("Gepversion is : %s", myGEPVersion);
                        }
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_1400GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_1400STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_1450GB){
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_1450STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			else{
				ACE_OS::strcpy(myGEPVersion,SSU_HWVER_VM_STRING);
				DEBUG("Gepversion is : %s", myGEPVersion);
			}
			break;
		default:
			ACE_OS::strcpy(myGEPVersion,"");
			WARNING("Gepversion is : %s", myGEPVersion);
		}
	}

	return (char *) myGEPVersion;
}

