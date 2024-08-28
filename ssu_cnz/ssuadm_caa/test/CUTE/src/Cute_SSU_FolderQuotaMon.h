#include "cute_suite.h"

class ACS_SSU_FolderQuotaMon;

class Cute_SSU_FolderQuotaMon
{
public:
//Cute_SSU_FolderQuotaMon();
static cute::suite make_suite_Cute_SSU_FolderQuotaMon();
static void vCheckIfActiveNode();
static void vListFolderQuotaConfig();
static void vTestSetConfigType();
//static void vEnCheckConfigChange( const ACE_TCHAR * szNewConfigType );
static void vTestenCheckConfigChange();
static void vPrepareFolderQuotas();
static void vSetupFolderQuotaMonitor();
static void vTestbCheckFolderQuotaEnabled();
//static void vCheckQuotaListItemExist();

static void vTestcreateMap();
static void vTestReadConfigType();
static void vTestSendMessageToQuotaThreads();
//static void vUpdateConfigType(const ACE_TCHAR* lpszConfigType);
static void vReadQuotas();

/*static void SetQuotaDefValue(const ACE_TCHAR* lpszValue,
                         const ACE_INT32 nFieldIdx,
                         SSU_FOLDERQUOTA_MONITOR* pQuotaDef)*/

public:
static ACS_SSU_FolderQuotaMon * pOFolderQuotaMon;
};
