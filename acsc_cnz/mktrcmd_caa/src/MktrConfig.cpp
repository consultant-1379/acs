/*
 * MktrConfig.cpp
 *
 *  Created on: Sep 6, 2010
 *      Author: xludesi
 */

#include "MktrConfig.h"
#include "MktrCommonDefs.h"


#define CLISS_CMD_INPUT_FILE_FOR_FULL_DUMP   				"/var/log/acs/mktr/cliss_cmd_show_all.txt"
#define CLISS_CMD_INPUT_FILE_FOR_HARDWARE_INFO          	"/var/log/acs/mktr/cliss_cmd_show_HardwareInfo.txt"
#define CLISS_CMD_INPUT_FILE_FOR_CONFIGURATION_INFO     	"/var/log/acs/mktr/cliss_cmd_show_ConfigurationInfo.txt"
#define CLISS_CMD_INPUT_FILE_FOR_CP_FILESYSTEM_M     		"/var/log/acs/mktr/cliss_cmd_show_CpFileSystemM.txt"
#define CLISS_CMD_INPUT_FILE_FOR_DATA_TRANSFER_M     		"/var/log/acs/mktr/cliss_cmd_show_DataTransferM.txt"
#define CLISS_CMD_INPUT_FILE_FOR_LICENSE_M     				"/var/log/acs/mktr/cliss_cmd_show_LicenseM.txt"
#define CLISS_CMD_INPUT_FILE_FOR_BLOCK_TRANSFER_MANAGER 	"/var/log/acs/mktr/cliss_cmd_show_BlockTransferManager.txt"
#define CLISS_CMD_INPUT_FILE_FOR_FILE_TRANSFER_MANAGER 		"/var/log/acs/mktr/cliss_cmd_show_FileTransferManager.txt"
#define CLISS_CMD_INPUT_FILE_FOR_ALPHANUMERICDEVICE_M     	"/var/log/acs/mktr/cliss_cmd_show_AlphanumericDeviceM.txt"
#define CLISS_CMD_INPUT_FILE_FOR_CPRELOAD_M 				"/var/log/acs/mktr/cliss_cmd_show_CpReloadM.txt"
#define CLISS_CMD_INPUT_FILE_FOR_TIMEREFERENCE_M			"/var/log/acs/mktr/cliss_cmd_show_TimeReferenceM.txt"
#define CLISS_CMD_INPUT_FILE_FOR_DATA_RECORD_M				"/var/log/acs/mktr/cliss_cmd_show_DataRecordM.txt"

#define CLISS_CMD_INPUT_FILE_FOR_HWCLS 						"/var/log/acs/mktr/cliss_cmd_show_hwcls.txt"
#define CLISS_CMD_INPUT_FILE_FOR_SWRPRINT 					"/var/log/acs/mktr/cliss_cmd_show_swrprint.txt"
#define CLISS_CMD_INPUT_FILE_FOR_STMOTLS 					"/var/log/acs/mktr/cliss_cmd_show_stmotls.txt"
#define CLISS_CMD_INPUT_FILE_FOR_STMMP 						"/var/log/acs/mktr/cliss_cmd_show_stmmp.txt"
#define CLISS_CMD_INPUT_FILE_FOR_STMRP 						"/var/log/acs/mktr/cliss_cmd_show_stmrp.txt"
#define CLISS_CMD_INPUT_FILE_FOR_FDLS 						"/var/log/acs/mktr/cliss_cmd_show_fdls.txt"
#define CLISS_CMD_INPUT_FILE_FOR_DBOLS						"/var/log/acs/mktr/cliss_cmd_show_dbols.txt"
#define CLISS_CMD_INPUT_FILE_FOR_AFPLS						"/var/log/acs/mktr/cliss_cmd_show_afpls.txt"

#define CLISS_CMD_OUTPUT_FILE_FOR_FULL_DUMP   				"com_fulldump.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_HARDWARE_INFO          	"com_hardwareInfo.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_CONFIGURATION_INFO     	"com_configurationInfo.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_CP_FILESYSTEM_M     		"com_CpFileSystemM.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_DATA_TRANSFER_M     		"com_DataTransferM.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_LICENSE_M     			"com_LicenseM.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_ALPHANUMERICDEVICE_M     	"com_AlphanumericDeviceM.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_CPRELOAD_M     			"com_CpReloadM.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_TIMEREFERENCE_M     		"com_TimeReferenceM.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_DATA_RECORD_M     		"com_DataRecordM.txt"

#define CLISS_CMD_OUTPUT_FILE_FOR_BLOCK_TRANSFER_MANAGER 	"com_block_transfer_TQ.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_FILE_TRANSFER_MANAGER 	"com_file_transfer_TQ.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_HWCLS 					"com_hwcls.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_SWRPRINT 					"com_swrprint.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_STMOTLS 					"com_stmotls.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_STMMP 					"com_stmmp.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_STMRP 					"com_stmrp.txt"
#define CLISS_CMD_OUTPUT_FILE_FOR_FDLS 						"com_fdls.txt"

#define IMMDUMP_OUTPUT_FILE_FULL_PATH   					"/var/log/acs/mktr/immdump.txt"
#define DMIDECODE_OUTPUT_FILE_FULL_PATH						"/var/log/acs/mktr/dmidecode.txt"
extern const char* mktruserStr;

MktrConfig::MktrConfig()
{
	_commonFileTable=0;
	_commonCmdTable=0;
	_cmdTable_SingleCp=0;
	_cmdTable_MultiCp=0;
	_memFileTable=0;
	_alogFileTable=0;
	_aposFileTable=0;
	_plogFileTable=0;
	_lbbCltTable=0;
	_aposCmdTable=0;
	_phaCmdTable=0;
	_scxFileTable=0;
	_cmxFileTable=0;
	_smxFileTable=0;
	_scxCmdTable=0;
	_smxCmdTable=0;
	_clissCmdAuxInfoTable=0;

}

MktrConfig::~MktrConfig()
{
	// TODO Auto-generated destructor stub
}

int MktrConfig::buildWithDefaults()
{
	this->_commonFileTable = MktrConfig::_default_commonFileTable;
	this->_commonCmdTable =  MktrConfig::_default_commonCmdTable;
	this->_cmdTable_SingleCp = MktrConfig::_default_cmdTable_SingleCp;
	this->_cmdTable_MultiCp = MktrConfig::_default_cmdTable_MultiCp;
	this->_scxCmdTable = MktrConfig::_default_scxCmdTable;
	this->_smxCmdTable = MktrConfig::_default_smxCmdTable;
	//this->_memFileTable = MktrConfig::_default_memFileTable;
	this->_alogFileTable = MktrConfig::_default_alogFileTable;
        this->_plogFileTable = MktrConfig::_default_plogFileTable;//PLOG
	this->_aposFileTable = MktrConfig::_default_aposFileTable;
	//this->_masFileTable = MktrConfig::_default_masFileTable;

	//this->_aposCmdTable = MktrConfig::_default_aposCmdTable;
	//this->_lbbCltTable = MktrConfig::_default_lbbCltTable;
	//this->_phaCmdTable = MktrConfig::_default_phaCmdTable;
	this->_scxFileTable = MktrConfig::_default_scxFileTable;
	this->_cmxFileTable = MktrConfig::_default_cmxFileTable;
	this->_smxFileTable = MktrConfig::_default_smxFileTable;
	this->_clissCmdAuxInfoTable = MktrConfig::_default_clissCmdAuxInfoTable;

	return 0;
}


MktrCommonDefs::File  MktrConfig::_default_commonFileTable[] =
{
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Cluster Logs", "/cluster/home/coremw_appdata/*", "Cluster_logs.tar.gz"},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "MAS logs", "/data/mas/logs/*", "MAS_logs.tar.gz", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "CQRH logs", "/data/cqs/logs/*", "CQRH_logs.tar.gz"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "TRA Logs", "/var/log/acs/tra/*", "TRA_logs.tar.gz", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "SSU Alarm Log", "/tmp/SSU_Event.log", "SSU_Event.log"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "IMM Full Dump", IMMDUMP_OUTPUT_FILE_FULL_PATH, NULL, false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "COM Logs", "/cluster/storage/clear/com-apr9010443/log/<hostname1>/*", "COM_logs_<hostname1>.tar.gz", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "COM Logs", "/cluster/storage/clear/com-apr9010443/log/<hostname2>/*", "COM_logs_Node_B.tar.gz"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "CBA Logs", "/var/log/esi-log/*", "CBA_logs.tar.gz", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "CMW Logs", "/var/log/acs/mktr/CMW_Logs.tar.gz", NULL, false},	
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "LDE Logs", "/var/log/acs/mktr/LDE_Logs.tar.gz", NULL, false},/*HY79380*/	
//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "HA Logs", "/cluster/storage/no-backup/coremw/var/log/saflog/*", "HA_logs.tar.gz"},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "SFC installation", "/var/log/ACS/logs/sfc/acs_sfc_installation.log", "c_acs_sfc_installation.log"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "SEC logs", "/var/log/cba-sec/persistent/*"/*HY13949*/, "SEC_logs.tar.gz", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "SEC CERT logs", "/var/log/sec/*", "SEC_CERT_logs.tar.gz", false},
        {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Security audit Logs", "/var/log/<hostname1>/security_audit*", "Security_Audit_<hostname1>.tar.gz", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "ACS SSU procdump", "/var/log/acs_ssu_procdump*.txt", "ACS_SSU_PRCDUMP.tar.gz", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "AP Integrity Report", "/var/log/acs/integrityapreport.log", NULL, false},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Mini Dumps cabs", "/var/logs/core/cabs/*", "CORE.tar.gz"},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Mini Dumps counts", "/var/logs/core/counts/*", "ACS_CORE_COUNTS.tar.gz"},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "AFP rootfile", "/data/aes/data/afp/rootfile", NULL},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "CDH destfile", "/data/aes/data/cdh/dest/cdhdestfile.txt", NULL},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "CDH destsetfile", "/data/aes/data/cdh/destset/cdhdestsetfile.txt", NULL},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "DBO alarmfile", "/data/aes/data/dbo/dboalarmfile", NULL},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "DBO systemfile", "/data/aes/data/dbo/dbosystemfile", NULL},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "AES Logs", "/data/aes/logs/*", "AES_Logs.tar.gz"},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "DBO filedest", "/data/AES/logs/GCC/FILEDEST.log", NULL},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "cdhdestfileex", "/data/aes/data/cdh/dest/cdhdestfileex.txt", NULL},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Cluster Configuration", "/cluster/etc/cluster.conf", "cluster_config.txt", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Software Configuration Node A", "/cluster/nodes/1/etc/rpm.conf", "software_config_node_A.txt", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Software Configuration Node B", "/cluster/nodes/2/etc/rpm.conf", "software_config_node_B.txt", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Product version", "/etc/cluster/product/version", "product_version.txt", false},
    {MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "SCRinfo data 1", "/cluster/storage/system/config/STS-Stmd/ScrInfo_data1", NULL, false},
    {MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "SCRinfo data 2", "/cluster/storage/system/config/STS-Stmd/ScrInfo_data2", NULL, false},
    {MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "SCRinfo file name", "/cluster/storage/system/config/STS-Stmd/ScrInfo_fileName", NULL, false},

 /*  ... TR  HQ47164. The collection of the following 4 files has been moved to "-l" option
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "DMA log", "/proc/dma", "DMA_log.txt"},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "IRQ log", "/proc/interrupts", "IRQ_log.txt"},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Memory log", "/proc/iomem", "Mem_log.txt"},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Ports log", "/proc/ioports", "Ports_log.txt"},
*/
	//{MktrCommonDefs::AP_1, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "MCS trace", "/data/MCS/logs/trace.log", "mcs_trace.log"},
	//{MktrCommonDefs::AP_1, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "MCS ALIS", "/data/MCS/data/ALIS/*", "MCS_ALIS.tar.gz"},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "MCS config", "/data/mcs/data/adh/config/*", "MCS_ADH_CONF.tar.gz", false},
	//{MktrCommonDefs::AP_1, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "MCS ALDA", "/data/MCS/data/ALDA/*", "MCS_ALDA.tar.gz"},

	//Blade cluster logs
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "sysconf logs", "/data/mcs/data/adh/sysconf/*", "sysconf.tar.gz", false},
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "STS logs", "/data/sts/logs/*", "STS_Logs.tar.gz"},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "CS Tables from var", "/var/log/ACS/data/CS/*", "CS_CTables.tar.gz"},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "CS Tables from data", "/data/ACS/data/CS/*", "CS_KTables.tar.gz"},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "CS logs from var", "/var/log/ACS/logs/CS/*", "CS_Clogs.tar.gz"},

	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL, false}
};

//Common logs which are collected by default (with out any options given).
//LSV14 :Common Naming file conventions
MktrCommonDefs::Command MktrConfig::_default_commonCmdTable[] =
{
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "prcstate -l", "prcstate -l", "acs_prcstate.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "netstat -an", "/bin/netstat -an", "netstat.txt", 15, "", "", false},
	//CNI60_1_45
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "swrprint -l", "swrprint -l", "acs_swrprint.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "tzls -a", "tzls -a", "acs_tzls.txt", 120, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mtzln -p", "mtzln -p", "acs_mtzln.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "WMIC Information", "ps -efLj", "pstat.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "tasklist", "ps aux", "tasklist.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Cluster state", "/usr/bin/amf-state", "cluster_state.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "CMW status node", "/opt/coremw/bin/cmw-status node", "cmw_status_node.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "APG Services status", "/opt/coremw/bin/cmw-status -v su si comp;", "apg_services_status.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "ispprint", "ispprint", "acs_ispprint.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "hwmls -l", "hwmls -l", "acs_hwmls.txt", 15}, "", ""},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "hwcls", "hwcls", "acs_hwcls.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, (MktrCommonDefs::APG_APG40C2|MktrCommonDefs::APG_APG40C4), MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "hwils", "hwils", "acs_hwils.txt", 15, "", ""},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG40C2, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"FCC Getbib", "fcc_getbib", "fcc_getbib.txt" ,15, "", ""},

	/*  begin addition of commands requested in TR  HQ47164 */
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "HW Version", "/opt/ap/apos/conf/apos_hwtype.sh", "acs_hwver.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "APG core dumps bt", "/opt/ap/acs/bin/acs_btscript.sh", "acs_btcore.txt", 120, "", "", false}, /*HZ74989*/
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "vlanls", "vlanls", "acs_vlanls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL,~(MktrCommonDefs::APG_SHELF_ARCHITECTURE_DMX | MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "psls", "psls", "acs_psls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_212_33, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "ipnaadm -show", "ipnaadm -show", "acs_ipnaadm_show.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_212_33, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "ipnaadm -list", "ipnaadm -list", "acs_ipnaadm_list.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1,   MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "lmlkmapls", "lmlkmapls", "acs_lmlkmapls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1,   MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "show_licenses", "show_licenses", "acs_show_licenses.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_SCB, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "hwmscbls", "hwmscbls", "acs_hwmscbls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_SCX, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "hwmxls", "hwmxls", "acs_hwmxls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "hwclock -r", "hwclock -r", "acs_hwclock.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "date", "date", "acs_date.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "CMW repository list", "/opt/coremw/bin/cmw-repository-list", "cmw_repository_list.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "LDE INFO", "/usr/bin/lde-info", "lde_info.txt", 15, "", "", false},/*HY79380*/
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "listtsuser", "listtsuser", "acs_listtsuser.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "free -m", "free -m", "memfree.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "route", "route", "route.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "uptime", "uptime", "uptime.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "mount", "mount", "mount.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "ll -R /data", "find /data  -wholename '/data/opt/ap/nbi_fuse' -prune -o  -type d -print0 | sort -z  | xargs -0 ls -l", "ll_data_fs.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "ps [pcpu]", "ps aux --sort pcpu", "pstat_pcpu.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "ps [rss]", "ps aux --sort rss", "pstat_rss.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "systemd services", "systemctl list-unit-files", "systemd_services.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "DHCPD leases", "cat /var/lib/dhcp/db/dhcpd.leases", "dhcpd_leases.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "TIPC config", "tipc link statistics show", "tipc_config.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "aehevls", "aehevls", "aehevls.txt", 300, "", "", false},
	/*	end of commands requested in TR  HQ47164 */

	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "lsof", "lsof", "lsof.txt", 30, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"dmidecode", "/usr/sbin/dmidecode >& " DMIDECODE_OUTPUT_FILE_FULL_PATH, NULL ,15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"integrityap", "integrityap >& /dev/null ", NULL, 300, "", "", false},

	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"Alarm status", "cluster alarm --status --full --all", "alarm_status.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"Disk usage status", "/bin/df -h", "disk_usage_status.txt" ,15, "", "", false},

	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Virtual memory stats", "/usr/bin/vmstat", "vmstat.txt", 15, "", "", false},
        {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "NFS io stats", "/usr/sbin/nfsiostat", "nfsiostat.txt", 15, "", "", false},


	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_1_2, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"SMART status dev 1", "/usr/sbin/smartctl -a /dev/sdb1 || true", "SMART_status_dev_1.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_1_2, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"SMART status dev 2", "/usr/sbin/smartctl -a /dev/sdb2 || true", "SMART_status_dev_2.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_1_2, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"SMART status sdb", "smartctl -a /dev/sdb || true", "SMART_status_sdb.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_1_2, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"SMART status sdc", "smartctl -a /dev/sdc || true", "SMART_status_sdc.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_1_2, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"SMART status sdd", "smartctl -a /dev/sdd || true", "SMART_status_sdd.txt", 30, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_4_5, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"SMART status disk", "/usr/sbin/smartctl -a  `readlink -f /dev/eri_disk` || true", "SMART_status_disk.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_7_8, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"SMART status disk", "/usr/sbin/smartctl -a  `readlink -f /dev/eri_disk` || true", "SMART_status_disk.txt", 15, "", "", false},	//Fix for TR HY98712
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE,"stmotls -l", "stmotls -l", "sts_stmotls_l.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1,  MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE,"stmotls", "stmotls", "sts_stmotls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1,  MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE,"stmmp", "stmmp -L -l", "sts_stmmp.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE,"stmrp", "stmrp -L -l", "sts_stmrp.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"stmfols", "stmfols", "sts_stmfols.txt", 15, "", ""},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE,"ls sts", "ls -R /data/sts", "sts_ls.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"Network Adapter", "/sbin/ifconfig -a", "network_adapter.txt", 15, "", ""},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"Network Adapter", "ip a", "ip_a.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"Environment Variables", "env", "env__vars_log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"Services", "/opt/ap/apos/bin/servicemgmt/servicemgmt list all", "services_log.txt", 30, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE,"Saf Services", "/opt/coremw/bin/cmw-status -v siass", "saf_services_log.txt", 30, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"CMW Logs", "/opt/coremw/bin/cmw-collect-info /var/log/acs/mktr/CMW_Logs.tar.gz", NULL, 300, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"LDE Logs", "cd /var/log/acs/mktr;/usr/bin/lde-collect-info -f LDE_Logs;cd $OLDPWD;", NULL, 300, "", "", false},/*HY79380*/
	/*HR69489: The following two lines commented, nslookup command no more needed on linux*/
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Lmhosts Node A", "nslookup <hostname1>", "lmhostscache_Node_A.txt", 30, "", "", false},
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Lmhosts Node B", "nslookup <hostname2>", "lmhostscache_Node_B.txt", 30, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"Configure housekeeping for CBA logs", "/usr/bin/esi-collect-info configure housekeeping_control=1", NULL, 30, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE,"CBA Logs", "/usr/bin/esi-collect-info collect", NULL, 600, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "alist", "alist", "acs_alist.txt", 30, "", "", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "ifconfig", "/sbin/ifconfig", "ifconfig.txt", 15, "", "", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "dsdls -a", "dsdls -a", "acs_dsdls_a.txt", 20, "", "", false},
    //{MktrCommonDefs::AP_1, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cpdlist -l", "cpdlist -l", "mcs_cpdlist_l.txt", 15, "", ""},
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "dbols -l", "dbols -l", "aes_dbols.txt", 15, "", ""},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cdhls -l", "/usr/bin/cdhls -l", "aes_cdhls.txt", 15, "", "", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cdhdsls -l", "/usr/bin/cdhdsls -l", "aes_cdhdsls.txt", 15, "", "", false},
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "afpls", "/usr/bin/afpls -ls", "aes_afpls.txt", 15, "", "", false},

    // Blade cluster Logs(LSV14)
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cpdtest -c", "cpdtest -c", "mcs_cpdtest_c.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cpdtest -p", "cpdtest -p", "mcs_cpdtest_p.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cpdlist -a", "cpdlist -a", "mcs_cpdlist_a.txt", 15, "", ""},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "fdls", "fdls", "acs_fdls.txt",15, "", ""},

	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "lmlkls", "lmlkls", "lmlkls.txt", 15, "", ""},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "nels", "nels", "acs_nels.txt", 15, "", ""},

	// Start of definitions for IMM commands
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "IMM - Full Dump", "immdump " IMMDUMP_OUTPUT_FILE_FULL_PATH, NULL, 60, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "IMM - Network Configuration Info", "immlist northBoundId=1,networkConfigurationId=1", "imm_networkConfigurationInfo.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "IMM - Configuration Info", "immlist axeFunctionsId=1", "imm_configurationInfo.txt", 15, "", "", false},
	// ... End of definitions for IMM commands

	// Start of definitions for CLISS commands ...
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - Full Dump", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_FULL_DUMP, CLISS_CMD_OUTPUT_FILE_FOR_FULL_DUMP, 1500, mktruserStr, "com-emergency"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - Hardware Info", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_HARDWARE_INFO, CLISS_CMD_OUTPUT_FILE_FOR_HARDWARE_INFO, 60, mktruserStr, "com-emergency", false},
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - Configuration Info", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_CONFIGURATION_INFO, CLISS_CMD_OUTPUT_FILE_FOR_CONFIGURATION_INFO, 120, mktruserStr, "com-emergency"},
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO CpFileSystemM", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_CP_FILESYSTEM_M, CLISS_CMD_OUTPUT_FILE_FOR_CP_FILESYSTEM_M, 60, mktruserStr, "com-emergency"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO DataTransferM", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_DATA_TRANSFER_M, CLISS_CMD_OUTPUT_FILE_FOR_DATA_TRANSFER_M, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1,   MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO LicenseM", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_LICENSE_M, CLISS_CMD_OUTPUT_FILE_FOR_LICENSE_M, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO BlockTransferManager", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_BLOCK_TRANSFER_MANAGER, CLISS_CMD_OUTPUT_FILE_FOR_BLOCK_TRANSFER_MANAGER, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO FileTransferManager", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_FILE_TRANSFER_MANAGER, CLISS_CMD_OUTPUT_FILE_FOR_FILE_TRANSFER_MANAGER, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO AlphanumericDeviceM", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_ALPHANUMERICDEVICE_M, CLISS_CMD_OUTPUT_FILE_FOR_ALPHANUMERICDEVICE_M, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO CpReloadM", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_CPRELOAD_M, CLISS_CMD_OUTPUT_FILE_FOR_CPRELOAD_M, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO TimeReferenceM", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_TIMEREFERENCE_M, CLISS_CMD_OUTPUT_FILE_FOR_TIMEREFERENCE_M, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - show MO DataRecordM", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_DATA_RECORD_M, CLISS_CMD_OUTPUT_FILE_FOR_DATA_RECORD_M, 60, mktruserStr, "com-emergency", false},
//  {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - hwcls", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_HWCLS, CLISS_CMD_OUTPUT_FILE_FOR_HWCLS, 60, mktruserStr, "com-emergency"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - swrprint", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_SWRPRINT, CLISS_CMD_OUTPUT_FILE_FOR_SWRPRINT, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1,   MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - stmotls", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_STMOTLS, CLISS_CMD_OUTPUT_FILE_FOR_STMOTLS, 600, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1,   MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - stmmp", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_STMMP, CLISS_CMD_OUTPUT_FILE_FOR_STMMP, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1,   MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - stmrp", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_STMRP, CLISS_CMD_OUTPUT_FILE_FOR_STMRP, 60, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_ALL,   MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL &(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "COM - fdls", "/opt/com/bin/cliss < " CLISS_CMD_INPUT_FILE_FOR_FDLS, CLISS_CMD_OUTPUT_FILE_FOR_FDLS, 60, mktruserStr, "com-emergency", false},
	// ... End of definitions for CLISS commands

	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "hwcls -x long", "hwcls -x long", "acs_hwcls_x.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "rtrls", "rtrls", "acs_rtrls.txt", 15, "", ""},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "rtrls -d", "rtrls -d", "acs_rtrls_d.txt", 15, "", ""},

	//Collect some HW Logs
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "FE Log", "eri-ipmitool dfx -n 250", "FE_Log.txt", 30, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "FW Log", "eri-ipmitool gif", "FW_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "HW Log", "eri-ipmitool gih", "HW_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_7_8, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "SEL Log", "/usr/bin/ipmitool sel list -vv", "SEL_Log.txt", 15, "", "", false},	//Fix for TR HY98712
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Board Product Log", "eri-ipmitool gp", "Board_Product_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Time Log", "eri-ipmitool gt", "Time_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Sensors Log", "eri-ipmitool rs", "Sensors_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_4_5, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Sensors Table", "eri-ipmitool rst", "Sensors_table_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Led Status Log", "eri-ipmitool gled", "Led_Status_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR Registers 0 Log", "eri-ipmitool rgpr ram 0", "GPR_Register_0_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR Registers 1 Log", "eri-ipmitool rgpr ram 1", "GPR_Register_1_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Board Temperature Log", "eri-ipmitool gtemp", "Board_Temperature_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_4_5, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Default BCS Register Log", "eri-ipmitool rdbcsgep5", "Default_BCS_Register_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_4_5, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "BCS Register Log", "eri-ipmitool rbcsgep5", "BCS_Register_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_4_5, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "RTFD Flag Log", "eri-ipmitool rrtfd", "RTFD_Flag_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 0 Log", "eri-ipmitool rgpr nvram 0", "GPR_NVRAM_Register_0_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 1 Log", "eri-ipmitool rgpr nvram 1", "GPR_NVRAM_Register_1_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 2 Log", "eri-ipmitool rgpr nvram 2", "GPR_NVRAM_Register_2_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 3 Log", "eri-ipmitool rgpr nvram 3", "GPR_NVRAM_Register_3_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 4 Log", "eri-ipmitool rgpr nvram 4", "GPR_NVRAM_Register_4_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 5 Log", "eri-ipmitool rgpr nvram 5", "GPR_NVRAM_Register_5_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 6 Log", "eri-ipmitool rgpr nvram 6", "GPR_NVRAM_Register_6_Log.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL & (~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "GPR NVRAM Registers 7 Log", "eri-ipmitool rgpr nvram 7", "GPR_NVRAM_Register_7_Log.txt", 15, "", "", false},

	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", "", false}
};

MktrCommonDefs::Command MktrConfig::_default_cmdTable_SingleCp[] =
{
    //{MktrCommonDefs::AP_1, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cuals", "cuals", "mcs_cuals.txt",  15, "", ""},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "dsdls", "dsdls" , "acs_dsdls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cpdtest -s", "cpdtest -s ", "mcs_cpdtest.txt", 30, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cpfls", "cpfls -ls", "fms_cpfls.txt",30, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "msdls", "msdls -m <msgstore>", "acs_msdls.txt", 30, "", "", false},
	//{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cpfdf", "cpfdf", "fms_cpfdf.txt",30, "", ""},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "rtrls -s", "rtrls -s", "acs_rtrls_s.txt", 30, "", ""},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml SAOSP", "mml SAOSP", "mml_saosp.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml IOROP", "mml IOROP", "mml_iorop.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml  IOSYP", "mml IOSYP", "mml_iosyp.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml  IOMSP:AP=1,ALL,QUEUE", "mml IOMSP:AP=1,ALL,QUEUE", "mml_iomsp.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml  PCORP:BLOCK=AD2", "mml  PCORP:BLOCK=AD2", "mml_pcorp_ad2.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml IOSTP", "mml IOSTP", "mml_iostp.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL,  MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml  CACLP:ALL", "mml CACLP:ALL", "mml_caclp_all.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml  IOUAP:USER=ALL", "mml  IOUAP:USER=ALL", "mml_iouap_all.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml  ALLIP:ALCAT=APZ", "mml  ALLIP:ALCAT=APZ", "mml_allip_apz.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml SYRIP:SURVEY", "mml SYRIP:SURVEY", "mml_syrip_survey.txt", 30, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml SAAEP:SAE=302", "mml  SAAEP:SAE=302", "mml_saaep_302.txt", 30, mktruserStr, "com-emergency", false},

	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml FCCMP", "mml FCCMP", "mml_fccmp.txt",30, mktruserStr, "com-emergency", false},

	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", "", false}
};

MktrCommonDefs::Command MktrConfig::_default_cmdTable_MultiCp[] =
{
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "capls", "capls", "capls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL&(~MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED), MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "vlanls", "vlanls", "acs_vlanls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "dsdls -n", "dsdls -n", "acs_dsdls_n.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "cpls -l", "cpls -l","acs_cpls.txt", 15, "", "", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "cpgls", "cpgls", "acs_cpgls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "msdls", "msdls -cp <cpname> -m <msgstore>", "acs_msdls.txt", 30, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "cpdtest", "cpdtest -s -cp <cpname>", "mcs_cpdtest.txt",30, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "cpfls", "cpfls -cp <cpname> -ls", "fms_cpfls.txt",30, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "cpfls cluster", "cpfls -cp CLUSTER -ls", "fms_cluster_cpfls.txt", 30, "", "", false},
	//{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "cpfdf", "cpfdf -cp <cpname>", "fms_cpfdf.txt",30, "", ""},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "rtrls -s", "rtrls -s <msgstore>", "acs_rtrls_s.txt", 30, "", ""},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "cqrhlls -l", "cqrhlls -l", "cqrhlls.txt", 15, "", "", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml SAOSP", "mml SAOSP", "mml_saosp.txt", 15, "", "", false},
    {MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml CQMSP:CP=ALL", "mml CQMSP:CP=ALL", "mml_cqmsp.txt", 15, "", "", false},
	//The following ones should target ALLBC:
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml -m -cp ALLBC IOMSP:AP=1,ALL", "mml -m -cp ALLBC IOMSP:AP=1,ALL", "mml_iomsp.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml -m -cp ALLBC PCORP:BLOCK=AD2", "mml -m -cp ALLBC PCORP:BLOCK=AD2", "mml_pcorp_ad2.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml -m -cp ALLBC IOSTP", "mml -m -cp ALLBC IOSTP", "mml_iostp.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml -cp ALLBC SYRIP:SURVEY", "mml -cp ALLBC SYRIP:SURVEY", "mml_syrip_survey.txt", 15, mktruserStr, "com-emergency", false},
	//The following ones should target Operative Group:
    {MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml IOSYP", "mml IOSYP", "mml_iosyp.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml CACLP:ALL", "mml CACLP:ALL", "mml_caclp_all.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml ALLIP:ALCAT=APZ", "mml ALLIP:ALCAT=APZ", "mml_allip_apz.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "mml SAAEP:SAE=302", "mml SAAEP:SAE=302", "mml_saaep_302.txt", 15, mktruserStr, "com-emergency", false},
	//The following commands should be given for each dual-sided CP present in the NE.

	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "mml -cp dualcp IOROP", "mml -cp <dualcp> IOROP", "mml_iorop_dualcp.txt", 15, mktruserStr, "com-emergency", false},
    {MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "mml -cp dualcp IOSYP", "mml -cp <dualcp> IOSYP", "mml_iosyp_dualcp.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "mml -cp dualcp PCORP:BLOCK=AD2", "mml -cp <dualcp> PCORP:BLOCK=AD2", "mml_pcorp_ad2_dualcp.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "mml -cp dualcp ALLIP:ALCAT=APZ", "mml -cp <dualcp> ALLIP:ALCAT=APZ", "mml_allip_apz_dualcp.txt", 15, mktruserStr, "com-emergency", false},
	{MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "mml -cp dualcp SYRIP:SURVEY", "mml -cp <dualcp> SYRIP:SURVEY", "mml_syrip_survey_dualcp.txt", 15, mktruserStr, "com-emergency", false},
    {MktrCommonDefs::AP_1, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_CS, "mml -cp dualcp SAAEP:SAE=302", "mml -cp <dualcp> SAAEP:SAE=302", "mml_saaep_302_dualcp.txt", 15, mktruserStr, "com-emergency", false},

    {MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", "", false}
};

//Logs collected when -d option is given
//MktrCommonDefs::File MktrConfig::_default_memFileTable[] =
//{
//    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "Memory dump", "/var/log/core/core*", "core_dump.tar.gz"},
//    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "Cluster dump", "/cluster/dumps/*.core", "cluster_dump.tar.gz"},
//    {MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL}
//};

//Logs collected when -a option is given
MktrCommonDefs::Command MktrConfig::_default_alogFileTable[] =
{
    //{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, "Alogfind", "/data/acs/data/alog/log/audit.*", "alogfind.tar.gz"},
	//{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL}
    {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "Alogfind", "alogfind", "alogfind.txt", 600, "", "", false},
    {MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", "", false}
};

//Logs collected when -l option is given
MktrCommonDefs::File MktrConfig::_default_aposFileTable[] =
{
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Kernel log Node A", "/var/log/<hostname1>/kernel", "Kernel_log_Node_A.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Kernel log Node B", "/var/log/<hostname2>/kernel", "Kernel_log_Node_B.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Authentication log Node A", "/var/log/<hostname1>/auth", "Auth_log_Node_A.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Authentication log Node B", "/var/log/<hostname2>/auth", "Auth_log_Node_B.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Disk status", "/proc/drbd", "Disk_status.txt", false},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "OpenSaf logs Node A", "/cluster/storage/no-backup/coremw/var/log/<hostname1>/*", "OpenSaf_logs_Node_A.tar.gz"},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "OpenSaf logs Node B", "/cluster/storage/no-backup/coremw/var/log/<hostname2>/*", "OpenSaf_logs_Node_B.tar.gz"},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "APOS Security logs", "/var/log/audit/*.log", "apos_security.tar.gz", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "DMA log", "/proc/dma", "DMA_log.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "IRQ log", "/proc/interrupts", "IRQ_log.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Memory log", "/proc/iomem", "Mem_log.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Ports log", "/proc/ioports", "Ports_log.txt", false},
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "DMI Data report", DMIDECODE_OUTPUT_FILE_FULL_PATH, NULL, false},

/* NFS,Disk releated logs */
        {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Disk stats", "/proc/diskstats", "Disk_stats.txt", false},
        {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Kernel NFS server stats", "/proc/net/rpc/nfsd", "NFS_server_stats.txt", false},
        {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Kernel NFS client stats", "/proc/net/rpc/nfs", "NFS_client_stats.txt", false},
        {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "Kernel stats", "/proc/stat", "kernel_stats.txt", false},
        {MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, "System load average", "/proc/loadavg", "system_loadavg.txt", false},





	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL, false}
};
//Start -- PLOG
//Protected Audit Logs collected when -a option is used
MktrCommonDefs::File MktrConfig::_default_plogFileTable[] =
{
		{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "Protected Audit Logs", "/data/opt/ap/internal_root/audit_logs/protected_logs/*.zip", "palogfind.tar.gz", false},
		{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL, false}

};
//End -- PLOG



//Blade cluster CP logs when -b option is given
//MktrCommonDefs::File MktrConfig::_default_masFileTable[] =
//{
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "MAS logs", "/data/mas/logs/*", "MAS_logs.tar.gz"},
	//{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_APG43L, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "CQRH logs", "/data/cqs/logs/*", "CQRH_logs.tar.gz"},

//	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL}
//};

//Logs collected when -l option is given
//MktrCommonDefs::Command MktrConfig::_default_aposCmdTable[] =
//	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", ""}
//};

//Logs collected when -l option is given
//MktrCommonDefs::Collector MktrConfig::_default_lbbCltTable[] =
//{
//	{MktrCommonDefs::AP_1, (MktrCommonDefs::APG_APG40C2|MktrCommonDefs::APG_APG40C4), MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "LBB cyclogs","C:\\acs\\data\\ftp\\MktrConfigbuild\\lbbcontlog", "\"C:\\Program Files\\FORCE\\ContLC\\ContLogColl\" -extra","lbbCyclogs.zip",600, "", ""},
//	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, NULL, 0, "", ""}
//};

//Logs collected when -p option is given
//MktrCommonDefs::Command MktrConfig::_default_phaCmdTable[] =
//{
//	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, "Phaprint", "phaprint", "acs_phaprint.txt", 15, "", ""},
//
//	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", ""}
//};

//Logs collected when -x option is given
MktrCommonDefs::Command MktrConfig::_default_scxCmdTable[] =
{
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "xcountls", "xcountls", "xcountls.txt", 30, "", "", false},
	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", "", false},
};


MktrCommonDefs::Command MktrConfig::_default_smxCmdTable[] =
{
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_SMX, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_ACTIVE, MktrCommonDefs::CMD_FILLER_NONE, "dShelfMngrLog", "dShelfMngrLog", "dShelfMngrLog.txt", 300, "", "", false},
	{MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::APG_HW_GEP_ALL, MktrCommonDefs::NODE_BOTH, MktrCommonDefs::CMD_FILLER_NONE, NULL, NULL, NULL, 0, "", "", false},
};

//Logs collected when -x option is given
MktrCommonDefs::File MktrConfig::_default_scxFileTable[] =
{
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "SCX Logs", "/data/apz/data/boot/scx/*", "SCX_Logs.tar.gz", false},
    {MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL, false}
};

MktrCommonDefs::File MktrConfig::_default_cmxFileTable[] =
{
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "CMX Logs", "/data/apz/data/boot/cmx/*", "CMX_Logs.tar.gz", false},
    {MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL, false}
};

MktrCommonDefs::File MktrConfig::_default_smxFileTable[] =
{
	{MktrCommonDefs::AP_ALL, MktrCommonDefs::APG_ALL, MktrCommonDefs::APG_SHELF_ARCHITECTURE_SMX, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_ACTIVE, "SMX Logs", "/data/apz/data/boot/smx/*", "SMX_Logs.tar.gz", false},
    {MktrCommonDefs::AP_NONE, MktrCommonDefs::APG_NONE, MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL, MktrCommonDefs::APZ_ALL, MktrCommonDefs::NODE_BOTH, NULL, NULL, NULL, false}
};

// Auxiliary info associated to 'cliss' commands
MktrCommonDefs::Cliss_command_aux_info MktrConfig::_default_clissCmdAuxInfoTable[] =
{
	// COM command to print out all Managed Objects info
	//{"show all", CLISS_CMD_INPUT_FILE_FOR_FULL_DUMP},

	// COM command to print out hardware info
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemComponentHandling=1,EquipmentM=1,HardwareMgmt=1", CLISS_CMD_INPUT_FILE_FOR_HARDWARE_INFO},

	// COM command to print out configuration info
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemHandling=1", CLISS_CMD_INPUT_FILE_FOR_CONFIGURATION_INFO},

	// COM command to show MO CpFileSystemM
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemHandling=1,CpFileSystemM=1", CLISS_CMD_INPUT_FILE_FOR_CP_FILESYSTEM_M},

	// COM command to show MO DataTransferM
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,DataTransferM=1", CLISS_CMD_INPUT_FILE_FOR_DATA_TRANSFER_M},

	// COM command to show MO LicenceM
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemHandling=1,LicenseM=1", CLISS_CMD_INPUT_FILE_FOR_LICENSE_M},

	// COM command to show MO AlphanumericDeviceM
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemHandling=1,AlphanumericDeviceM=1", CLISS_CMD_INPUT_FILE_FOR_ALPHANUMERICDEVICE_M},

	// COM command to show MO CpReloadM
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemHandling=1,CpReloadM=1", CLISS_CMD_INPUT_FILE_FOR_CPRELOAD_M},

	// COM command to show MO TimeReferenceM
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemHandling=1,TimeReferenceM=1", CLISS_CMD_INPUT_FILE_FOR_TIMEREFERENCE_M},

	// COM command template to print out the complete configuration of TQ/destinationSet/destination for block transfer.
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,DataTransferM=1,BlockTransferManager=1", CLISS_CMD_INPUT_FILE_FOR_BLOCK_TRANSFER_MANAGER},

	// COM command template to print out the complete configuration of TQ/destinationSet/destination for file transfer.
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,DataTransferM=1,FileTransferManager=1", CLISS_CMD_INPUT_FILE_FOR_FILE_TRANSFER_MANAGER},

	// COM command template equivalent to 'hwcls' command
	//{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemComponentHandling=1,EquipmentM=1,HardwareMgmt=1", CLISS_CMD_INPUT_FILE_FOR_HWCLS},

	// COM command template equivalent to 'swrprint' command
	{"show all ManagedElement=%s,SystemFunctions=1,SwInventory=1", CLISS_CMD_INPUT_FILE_FOR_SWRPRINT},

	// COM command template  equivalent to 'stmotls' command
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,StatisticalCounterM=1,ObjectTypes=1", CLISS_CMD_INPUT_FILE_FOR_STMOTLS},

	// COM command template  equivalent to 'stmmp' command
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,StatisticalCounterM=1,MeasurementPrograms=1", CLISS_CMD_INPUT_FILE_FOR_STMMP},

	// COM command template equivalent to 'stmrp' command
	{"show all ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,StatisticalCounterM=1,Reports=1", CLISS_CMD_INPUT_FILE_FOR_STMRP},

	// COM command template equivalent to 'fdls' command
	{"show all verbose ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,SystemHandling=1,FunctionDistributionM=1", CLISS_CMD_INPUT_FILE_FOR_FDLS},

	// COM command template to print out the complete information about charging data configuration
	{"show all verbose ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,DataRecordM=1", CLISS_CMD_INPUT_FILE_FOR_DATA_RECORD_M},

	// End of definitions
	{NULL, NULL}
};
