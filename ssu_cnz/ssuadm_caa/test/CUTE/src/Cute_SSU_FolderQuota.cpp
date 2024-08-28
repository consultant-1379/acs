#include <ace/ACE.h>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "acs_ssu_alarm_handler.h"
#include "Cute_SSU_Monitor_Service.h"
#include "acs_ssu_folder_quota.h"
#include "Cute_SSU_FolderQuota.h"
#include "acs_ssu_common.h"
#include <ace/OS_NS_strings.h>

#define ONE_KB                0x400
#define ONE_MB                0x100000
#define ONE_GB                0x40000000
#define CUTE_ACS_SSU_DATA_PATH "/data/acs/data"

ACS_SSU_FolderQuota *Cute_SSU_FolderQuota::m_poFolderQuota;
ACE_Recursive_Thread_Mutex Cute_SSU_FolderQuota::m_srctQuotaCS;
//ACS_SSU_AlarmHandler* Cute_SSU_Monitor_Service::m_poAlarmHandler;

cute::suite Cute_SSU_FolderQuota::make_suite_Cute_SSU_FolderQuota()
{
	SSU_FOLDERQUOTA_MONITOR *poFolderQuotaData = new SSU_FOLDERQUOTA_MONITOR;

	poFolderQuotaData->A1AlarmLevel = 100;
	poFolderQuotaData->A1CeaseLevel = 95;
	poFolderQuotaData->A2AlarmLevel = 50;
	poFolderQuotaData->A2CeaseLevel = 45;
	ACE_OS::strcpy( poFolderQuotaData->ConfigType,"BSC");
	poFolderQuotaData->EventNumber  = 123456;
	ACE_OS::strcpy( poFolderQuotaData->Path , "/data/acs/data" );

    m_poFolderQuota = new ACS_SSU_FolderQuota(
    		Cute_SSU_Monitor_Service::getPAlarmHandler(),
    		                    poFolderQuotaData,
    		                    m_srctQuotaCS
                             );

    m_poFolderQuota->activate(THR_NEW_LWP|THR_JOINABLE|THR_SUSPENDED);

	cute::suite s;
	//s.push_back(CUTE(vTestIsActive));
	s.push_back(CUTE(vTestIsEqual));
	s.push_back(CUTE(vTestget_Path));
	s.push_back(CUTE(vTestCalculateFolderSize));
	s.push_back(CUTE(vTestCheckConfigSet));
	s.push_back(CUTE(vTestGetQuotaValue));
	s.push_back(CUTE(vTestStop));
	s.push_back(CUTE(vTestRemoteStop));
	s.push_back(CUTE(vTestFolderAvailable));
	//s.push_back(CUTE(vTestCheckConfig));
	s.push_back(CUTE(vTestget_ConfigType));
	s.push_back(CUTE(vTestCalculateFolderSize));
	s.push_back(CUTE(vTestenCheckAndRaiseAlarm));
	s.push_back(CUTE(vTestszGetBlockDevice));
	s.push_back(CUTE(vTestAddFolderQuota));
	//s.push_back(CUTE(vTestModifyFolderQuota));
	//s.push_back(CUTE(VTestRemoveFolderQuota));
	s.push_back(CUTE(vTestBytesToSizeString));
	s.push_back(CUTE(vTestCalculateFolderQuota));

	delete poFolderQuotaData;

	return s;
}


void Cute_SSU_FolderQuota::vFolderQuotaCleanup()
{
   delete m_poFolderQuota;
}

void Cute_SSU_FolderQuota::vTestCheckConfigSet()
{
	SSU_FOLDERQUOTA_MONITOR *poFolderQuotaData = new SSU_FOLDERQUOTA_MONITOR;

	ACE_OS::strcpy(poFolderQuotaData->ConfigType,"MSC");
	poFolderQuotaData->QuotaLimit = 400;
	poFolderQuotaData->A1AlarmLevel = 95;
	poFolderQuotaData->A1CeaseLevel = 90;
	poFolderQuotaData->A2AlarmLevel = 40;

	poFolderQuotaData->A2CeaseLevel = 35;

	m_poFolderQuota->ChangeConfig(poFolderQuotaData);

	ASSERTM(" Config Change Failure ", ACE_OS::strcasecmp( m_poFolderQuota->get_ConfigType(),"MSC")==0);

	delete poFolderQuotaData;
}

void Cute_SSU_FolderQuota::vTestGetQuotaValue()
{
	 ACE_TCHAR pzBuffer[1024];
	ASSERTM("Not able to retrive Quota value",m_poFolderQuota->GetQuotaValue(pzBuffer)== true);
}

void Cute_SSU_FolderQuota::vTestIsActive()
{
	ASSERTM(" Thread is not active ", m_poFolderQuota->IsActive()== true);
}

void Cute_SSU_FolderQuota::vTestIsEqual()
{
	ASSERTM(" Thread is not active ", m_poFolderQuota->IsEqual(CUTE_ACS_SSU_DATA_PATH) == true);
}

void Cute_SSU_FolderQuota::vTestStop()
{
	m_poFolderQuota->Stop(true);


}

void Cute_SSU_FolderQuota::vTestRemoteStop()
{
	m_poFolderQuota->RemoteStop(false);
}
void Cute_SSU_FolderQuota::vTestFolderAvailable()
{
   ASSERTM	("Folder is not available",m_poFolderQuota->FolderIsAvailable()== true);
}
void Cute_SSU_FolderQuota::vTestAddSubDirObj()
{
}

void Cute_SSU_FolderQuota::vTestvRemoveSubDirObjs()
{
}
void Cute_SSU_FolderQuota::vTestCheckConfig()
{
	SSU_FOLDERQUOTA_MONITOR *poFolderQuotaData = new SSU_FOLDERQUOTA_MONITOR;
	ACE_TCHAR szData[512];
	ASSERTM("Checking of Configuration is failed",m_poFolderQuota->CheckConfig(poFolderQuotaData,szData));
	delete poFolderQuotaData;
}

void Cute_SSU_FolderQuota::vTestget_Path()
{
	ASSERTM(" Paths Does not match ", ACE_OS::strcmp(m_poFolderQuota->get_Path(),"/data/acs/data")==0 );
}

void Cute_SSU_FolderQuota::vTestget_ConfigType()
{
	ASSERTM(" Paths Does not match ", ACE_OS::strcmp(m_poFolderQuota->get_ConfigType(),"BSC")==0 );
}

void Cute_SSU_FolderQuota::vTestCalculateFolderSize()
{
	ACE_UINT64 u64Size = 0;
	m_poFolderQuota->CalculateFolderSize(u64Size,true);
}

void Cute_SSU_FolderQuota::vTestenCheckAndRaiseAlarm()
{
	ASSERTM("",m_poFolderQuota->enCheckAndRaiseAlarm());
}
void Cute_SSU_FolderQuota::vTestszGetBlockDevice()
{
	ASSERTM(" szGetBlockDevice returned wrong device string ", ACE_OS::strcmp(m_poFolderQuota->szGetBlockDevice().c_str(),"/dev/md0"));
}

void Cute_SSU_FolderQuota::vTestAddFolderQuota()
{
	ASSERTM(" Set Folder Quota Failed ", m_poFolderQuota->AddFolderQuota()==true);
}

void Cute_SSU_FolderQuota::vTestModifyFolderQuota()
{
	ASSERTM(" Modify Folder Quota Failed ", m_poFolderQuota->ModifyFolderQuota()==true);
}

void Cute_SSU_FolderQuota::VTestRemoveFolderQuota()
{
	ASSERTM(" Reset Folder Quota Failed ", m_poFolderQuota->RemoveFolderQuota()==true);
}

void Cute_SSU_FolderQuota::vTestBytesToSizeString()
{
	ACE_UINT64 u64Size=ONE_MB;

	const ACE_TCHAR *pszData = m_poFolderQuota->BytesToSizeString(u64Size,false);

	ASSERTM(" Bytes to String Comparison Failed ", ACE_OS::strcmp(pszData,"1MB")==0);
}

void Cute_SSU_FolderQuota::vTestCalculateFolderQuota()
{
	ACE_UINT64 u64Limit=100;
	ACE_UINT64 u64CurrentSize=50;

	ASSERTM(" vTestCalculateFolderQuota Failed",m_poFolderQuota->CalculateFolderQuota(u64Limit,u64CurrentSize)== (double)0.5);
}

