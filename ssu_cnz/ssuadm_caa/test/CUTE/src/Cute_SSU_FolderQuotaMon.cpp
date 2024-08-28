#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_SSU_FolderQuotaMon.h"

#include "acs_ssu_folder_quota_mon.h"
#include "acs_ssu_common.h"
#include "acs_ssu_path.h"
#include "Cute_SSU_Monitor_Service.h"
#include "acs_ssu_types.h"

#include "cute_suite.h"

#define TEST_QUOTA_FILE "/opt/ap/acs/etc/SSU_Quotas"

//std::string ACS_SSU_Common::dnOfSSURoot = "";

ACS_SSU_FolderQuotaMon * Cute_SSU_FolderQuotaMon::pOFolderQuotaMon=0;

ACE_Event* Cute_SSU_Monitor_Service::m_plhEventHandles[NO_OF_HANDLES]={0};

void Cute_SSU_FolderQuotaMon::vListFolderQuotaConfig()
{
	ACE_UINT32 result;
	result = pOFolderQuotaMon->ListFolderQuotaConfig(true);
	ASSERTM("Failed to list the folder in the current configuration", result == 0);
}


void Cute_SSU_FolderQuotaMon::vCheckIfActiveNode()
{
	ASSERTM("Node is not Active",ACS_SSU_Common::CheckIfActiveNode()==true);
}

void Cute_SSU_FolderQuotaMon::vPrepareFolderQuotas()
{
  ASSERTM("Unable to prepare the folder quotas",pOFolderQuotaMon->PrepareFolderQuotas()==true);
}
void Cute_SSU_FolderQuotaMon::vSetupFolderQuotaMonitor()
{
 ASSERTM("Unable to set folder quota monitor",pOFolderQuotaMon->SetupFolderQuotaMonitor()==true);
}

//void Cute_SSU_FolderQuotaMon::vUpdateConfigType(const ACE_TCHAR* lpszConfigType)
//{
//
//}
void Cute_SSU_FolderQuotaMon::vReadQuotas()
{
	ACS_SSU_QuotasList QuotaList;
    ACE_TCHAR* lpszConfigType=new char[20];
    ACE_OS::strcpy(lpszConfigType,"MSC");
  ASSERTM("Unable to Read Quta file",pOFolderQuotaMon->ReadQuotas(QuotaList,lpszConfigType,1)!=-1);
}

void Cute_SSU_FolderQuotaMon::vTestbCheckFolderQuotaEnabled()
{
	ASSERTM("Folder Quota is not enabled",pOFolderQuotaMon->bCheckFolderQuotaEnabled());
}

void Cute_SSU_FolderQuotaMon::vTestcreateMap()
{

	ACS_SSU_Path objGetFolderPath;
	ASSERTM("Map for folder is not created",objGetFolderPath.createMap()== true);
}
void Cute_SSU_FolderQuotaMon::vTestReadConfigType()
{
	ASSERTM("Failed to read the configuration type",pOFolderQuotaMon->ReadConfigType() == true);
}
/*void Cute_SSU_FolderQuotaMon::vTestSendMessageToQuotaThreads()
{
	bool ntFailOver;
	ASSERTM("failed to send message to quota thread",pOFolderQuotaMon->vSendMessageToQuotaThreads(ntFailOver));
}*/
void Cute_SSU_FolderQuotaMon::vTestSetConfigType()
{
	const ACE_TCHAR * newconfig;
	newconfig ="MSC";
	ASSERTM("failed to set the new configuration type",pOFolderQuotaMon->SetConfigType(newconfig));
}

void Cute_SSU_FolderQuotaMon::vTestenCheckConfigChange()
{
	ACE_UINT32 result;
	const ACE_TCHAR * newconfig;
		newconfig ="MSC";
	result = pOFolderQuotaMon->enCheckConfigChange(newconfig);
	ASSERTM("Failed to change the configuration",result == 0);
}



cute::suite Cute_SSU_FolderQuotaMon::make_suite_Cute_SSU_FolderQuotaMon(){
	OmHandler *om = new OmHandler();
        pOFolderQuotaMon=new ACS_SSU_FolderQuotaMon(Cute_SSU_Monitor_Service::getPlhEventHandles(),Cute_SSU_Monitor_Service::getPAlarmHandler(),om);

	cute::suite s;
	//s.push_back(CUTE(vListFolderQuotaConfig));
	s.push_back(CUTE(vCheckIfActiveNode));
	s.push_back(CUTE(vTestReadConfigType));
	s.push_back(CUTE(vTestcreateMap));
	//s.push_back(CUTE(vTestSetConfigType));
	 s.push_back(CUTE(vReadQuotas));

	s.push_back(CUTE(vPrepareFolderQuotas));
	//s.push_back(CUTE(vTestenCheckConfigChange));
	s.push_back(CUTE(vSetupFolderQuotaMonitor));
	s.push_back(CUTE(vTestbCheckFolderQuotaEnabled));

//	s.push_back(CUTE(vCheckQuotaListItemExist));
//	s.push_back(CUTE(vUpdateConfigType));

//	s.push_back(CUTE(SetQuotaDefValue));

	return s;
}



