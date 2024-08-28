
#ifndef _ACS_SSU_TYPES_H
#define _ACS_SSU_TYPES_H

#include <ace/ACE.h>

//!@todo To be removed once high level defines for boolean variables are defined for project
typedef bool BOOL;
#if 0
#define TRUE true
#define FALSE false
#endif

#define DOUBLE double
typedef unsigned char UBYTE;
typedef ACE_TCHAR *LPBYTE;

//! Result codes
#define SSU_RC_OK                            0
#define SSU_RC_NOK                           1
#define SSU_RC_UNKNOWNCMDTYPE                2
#define SSU_RC_OPERATIONINPROGRESS           3
#define SSU_RC_SERVERNOTRESPONDING           4
#define SSU_RC_CONFIGTYPEALREADYEXIST        5
#define SSU_RC_NOTACTIVENODE                 6
#define SSU_RC_QUOTAFILEERROR                7
#define SSU_RC_CONFIGFILEERROR               8
#define SSU_RC_NEWQUOTAEXCEEDED              9
#define SSU_RC_ONAPG43ONLY                   10
#define SSU_RC_ISNOTMULTCP                   11
#define SSU_RC_QUOTACHECKERROR               12
#define SSU_RC_CONFIGCHANGEERROR             13
#define SSU_RC_SERVICEISBUSY				 16
#define SSU_RC_INVALIDARGUMENT               31
#define SSU_RC_PERMISSIONSDENIED             32
#define SSU_RC_UNKNOWNNODENAME               33
#define SSU_RC_OVERLAPPED_IO                 34
#define SSU_RC_WAIT_TIMEOUT                  35
#define SSU_RC_ENDEVENTSIGNALED              200
#define SSU_RC_OTHERERROR                    255

/*! macros to use while raising alarm (Run first time or each time) */
#define EXECUTEFIRSTTIMEONLY 1
#define EXECUTEEVERYTIME 0

#define GIGABYTE (1024ULL * 1024 * 1024)
#define MEGABYTE (1024ULL * 1024)
#define KILOBYTE (1024ULL)

#define ONE_KB                0x400
#define ONE_MB                0x100000
#define ONE_GB                0x40000000

/*! macros to use for FOLDER QUOTA MONITOR waiting */
#define SSU_FQUOTA_MON_SHORT_WTIME 10
#define SSU_FQUOTA_MON_MEDIUM_WTIME 15
#define SSU_FQUOTA_MON_LONG_WTIME 30

#define SSU_SERVICE_VER "1.0"
#define SSU_QUOTASTRING "grpjquota"
#define SSU_PID_FILE    "/var/run/ssumonitorservice.pid"
#define SSU_QUOTAS_FILE ACE_TEXT("/opt/ap/acs/etc/SSU_Quotas")
#define SSU_FILEM_QUOTAS_FILE ACE_TEXT("/opt/ap/acs/etc/SSU_FileMQuotas")
#define INFINITE        (((ACE_INT64)0)-1)  // Infinite timeout

//#define SSU_ACS_DATAPATH                      "/data/acs/data"
#define SSU_ACS_DATAPATH                      "/data"
#define SSU_AEH_TEXT_INT_SUPERVISOR_ERROR     ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR")
#define SSU_PERF_MON_OBJ_STRING_MEM ACE_TEXT("MEMORY")
#define SSU_PERF_MON_OBJ_STRING_FILE_HANDLE_COUNT ACE_TEXT("PROCESS")

#define FILE_MAX_PATH                         260
#define ACS_SSU_PARITION_NAME_MAX             255

#define SSU_DEFAULT_CONFIG_TYPE              ACE_TEXT("BSC")
#define SSU_CHANGE_CONFIG_STATUS_TEXT_LENGTH  512

//! IMM Defines
// IMM Object Description for fetching IMM values for Folder Quota Monitor
#define PARTIALDN	"folderQuotaSettingsId"
//#define SSU_CONFIG_OBJ_DNAME "systemSupervisionConfigurationId=1,systemSupervisionMId=1,safApp=safImmService"
#define SSU_CONFIG_OBJ_RDN "systemSupervisionConfigurationId=1"
#define SSU_IMM_CONFIG_ATTR_NAME "systemConfiguration"
//#define SSU_IMM_LS_OPTIONS_ATTR_NAME "saSmfSSULSOptions"

#define ACS_SSU_FOLDER_QUOTA_OBJECT_NAME "folderQuotaInfoId"
//#define ACS_SSU_PARENT_NAME "systemSupervisionMId=1,safApp=safImmService"
#define ACS_SSU_RUNTIMEIMPLEMENTER "SSUFolderQuotaImplementer"

//#define SSU_ERRORHANDLER_PARENT_ERROR_CLASS_NAME "safApp=safImmService"
#define SSU_ERRORHANDLER_ERROR_CLASS_NAME "SSUErrorHandler"
#define SSU_ERRORHANDLER_RDN_ATTRIBUTE_NAME "ssuErrorHandlerId"
#define SSU_ERRORHANDLER_CCBID_ATTRIBUTE_NAME "ccbId"
#define SSU_ERRORHANDLER_ERRORId_ATTRIBUTE_NAME "errorId"
#define SSU_ERRORHANDLER_ERRORTEXT_RDNATTRIBUTE_NAME "errorText"
#define SSU_ERRORHANDLER_RDNOBJECTNAME "ssuErrorHandlerId=1"
#define SSU_ERRORHANDLER_IMPLEMENTERNAME "SSUErrorRuntimeHandler"

#define ACS_IMM_SSU_ROOT_CLASS_NAME  "SystemSupervisionM"
#define ACS_IMM_SSU_ROOT_CLASS_RDN  "SystemSupervisionMId=1"
#define SSU_COFIGURATION_CLASS_NAME "SystemSupervisionConfiguration"
#define ACS_SSU_ROOT_IMPL    "acs_ssu_root_impl"
#define ACS_SSU_PERF_IMPL    "acs_ssu_perf_impl"
#define ACS_SSU_DISK_IMPL    "acs_ssu_disk_impl"
#define ACS_SSU_CONFIG_IMPL  "acs_ssu_cfg_impl"
#define SLEEP_TIME 5

// PHA PARAMETERS
#define SSU_DISK_MON_OBJECT "prvDiskMonObject"
//#define SSU_DISK_MON_COUNTER "prvDiskMonCounter"
#define SSU_DISK_MON_INSTANCE "prvDiskMonInstance"
#define SSU_A1_SPECIFIC_PARTITION_NAME "prvA1SpecificPartitionName"
#define SSU_A2_ALARM_LEVEL_LARGE_PARTITION "prvA2AlarmLevelLargePartition"
#define SSU_A1_CEASE_LEVEL_SMALL_PARTITION "prvA1CeaseLevelSmallPartition"
#define SSU_A1_ALARM_LEVEL_SMALL_PARTITION "prvA1AlarmLevelSmallPartition"
#define SSU_A1_CEASE_LEVEL_LARGE_PARTTION "prvA1CeaseLevelLargePartition"
#define SSU_A1_ALARM_LEVEL_LARGE_PARTITION "prvA1AlarmLevelLargePartition"
//#define SSU_DISK_MON_EVENT_NUM "prvDiskMonEventNumber"
#define SSU_DISK_MON_POLLING_INTERVAL "prvDiskMonPollingInterval"
#define SSU_DISK_MON_ALERT_IF_OVER "prvDiskMonAlertIfOver"
#define SSU_A1_SPECIFIC_ALARM_LEVEL "prvA1SpecificAlarmLevel"
#define SSU_A1_SPECIFIC_CEASE_LEVEL "prvA1SpecificCeaseLevel"
#define SSU_MAX_SIZE_OF_SMALL_PARTITION "prvMaxSizeOfSmallPartition"
#define SSU_A2_CEASE_LEVEL_SMALL_PARTTION "prvA2CeaseLevelSmallPartition"
#define SSU_A2_ALARM_LEVEL_SMALL_PARTTION "prvA2AlarmLevelSmallPartition"
#define SSU_A2_CEASE_LEVEL_LARGE_PARTION "prvA2CeaseLevelLargePartition"
#define SSU_PERF_MON_MEM_A2_ALARM_LIMIT "prvPerfMonMemoryA2AlarmLimit"
#define SSU_PERF_MON_ALERT_IF_OVER "prvPerfMonAlertIfOver"
#define SSU_PERF_MON_MEM_INSTANCE "prvPerfMonMemoryInstance"
//#define SSU_PERF_MON_MEM_COUNTER "prvPerfMonMemoryCounter"
#define SSU_PERF_MON_MEM_OBJECT "prvPerfMonMemoryObject"
#define SSU_A2_SPECIFIC_CEASE_LEVEL "prvA2SpecificCeaseLevel"
#define SSU_A2_SPECIFIC_PARTITION_NAME "prvA2SpecificPartitionName"
#define SSU_A2_SPECIFIC_ALARM_LEVEL "prvA2SpecificAlarmLevel"
#define SSU_PERF_MON_MEM_A2_AEH_EVENT_NUM "prvPerfMonMemA2AehEventNumber"
#define SSU_PERF_MON_MEM_A1_CEASE_LIMIT "prvPerfMonMemoryA1CeaseLimit"
#define SSU_PERF_MON_MEM_A1_ALARM_LIMIT "prvPerfMonMemoryA1AlarmLimit"
#define SSU_PERF_MON_MEM_A1_AEH_EVENT_NUM "prvPerfMonMemA1AehEventNumber"
#define SSU_PERF_MON_MEM_POLLING_INTERVAL "prvPerfMonMemPollingInterval"
#define SSU_PERF_MON_MEMORY_A2_CEASE_LIMIT "prvPerfMonMemoryA2CeaseLimit"
//#define SSU_A1_SEVERITY "prvA1Severity"
#define SSU_PERF_MON_FILE_HANDLE_CEASE_LIMIT "prvPerfMonFileHandleCeaseLimit"
#define SSU_PERF_MON_FILE_HANDLE_ALARM_LIMIT "prvPerfMonFileHandleAlarmLimit"
#define SSU_PERF_MON_FILE_HANDLE_AEH_EVENT_NUM "prvPerfMonFHandleAehEventNum"
#define SSU_PERF_MON_HANDLE_POLL_INTERVAL "prvPerfMonFHandlePollInterval"
#define SSU_PERF_MON_HANDLE_INSTANCE "prvPerfMonFileHandleInstance"
#define SSU_PERF_MON_FILE_HANDLE_COUNTER "prvPerfMonFileHandleCounter"
#define SSU_PERF_MON_FILE_HANDLE_OBJECT "prvPerfMonFileHandleObject"
#define SSU_PERF_MON_APPLICATION_TO_RUN "prvApplicationToRun"
#define SSU_NODE "prvNode"
//#define SSU_A2_SEVERITY "prvA2Severity"
#define SSU_EXECUTE_APP_FIRST_TIME "prvExecuteAppFirstTime"


// Names of classes and thier attribute in new model

#define SSU_ROOT_CLASS_NAME  "SystemSupervision"

#define SSU_SYSTEM_CONFIGURATION  "systemConfiguration"

#define SSU_DISKMONITOR_INFO_CLASS_NAME "DiskMonitor"

#define SSU_PERFMONITOR_INFO_CLASS_NAME "PerformanceMonitor"

#define SSU_FUNCTIONQUOTA_INFO_CLASS_NAME "FolderQuotaInfo"

#define SSU_FUNCTIONQUOTA_INFO_RDN_NAME  "folderQuotaInfoId"
#define SSU_FUNCTIONQUOTA_INFO_UPPER_LIMIT  "upperLimit"
#define SSU_FUNCTIONQUOTA_INFO_CURR_FOLDERSIZE "currentFolderSize"
#define SSU_FUNCTIONQUOTA_INFO_A1_ALARM_LIMIT "folderHighAlarmLevelLimit"
#define SSU_FUNCTIONQUOTA_INFO_A1_CEASE_LIMIT "folderHighCeaseLevelLimit"
#define SSU_FUNCTIONQUOTA_INFO_A2_ALARM_LIMIT "folderLowAlarmLevelLimit"
#define SSU_FUNCTIONQUOTA_INFO_A2_CEASE_LIMIT "folderLowCeaseLevelLimit"


// END

// SSU PHA Params - New Model

// HARD CODED VALUES FOR DISK AND PERF MONITORS.
#define SSU_DISK_MON_OBJECT_NAME "System Disk"
#define SSU_DATA_DISK_MON_OBJECT_NAME "Data Disk"
#define SSU_DATA_DISK_MON_INSTANCE_NAME "max_size"	
#define VAR_LOG_PARTITION "/var/log"
#define CLUSTER_PARTITION "/cluster"
#define DATA_PARTITION "/data"

#define SSU_PERF_MON_MEM_OBJECT_NAME  "Memory"
#define SSU_PERF_MON_FILEHANDLE_OBJECT_NAME  "Process"

#define SSU_DISK_MON_COUNTER "%FreeSpace"
#define SSU_PERF_MON_MEM_COUNTER  "Percentage"
#define SSU_PERF_MON_FILEHANDLE_COUNTER  "HandleCount"

#define SSU_DISK_MON_INSTANCE_VALUE "null"
#define SSU_PERF_MON_MEM_INSTANCE_VALUE  "Mem"
#define SSU_PERF_MON_FILEHANDLE_INSTANCE_VALUE  "Total"

#define SSU_DISK_MON_EVENT_NUM  6104
#define SSU_PERF_MON_MEM_A2_EVENT_NUM  6101
#define SSU_PERF_MON_MEM_A1_EVENT_NUM  6102
#define SSU_PERF_MON_FILEHANDLE_EVENT_NUM  6105

#define SSU_APPLICATION_TO_RUN  "acs_ssu_procls.sh"

#define SSU_A1_SEVERITY "A1"
#define SSU_A2_SEVERITY "A2"

#define SSU_MAX_SIZEOF_SMALL_PARTITION  "4294967296"

#define SSU_ENABLE_ALERT_IF_OVER 1
#define SSU_DISABLE_ALERT_IF_OVER 0

#define SSU_ENABLE_EXECUTE_APPL_FIRST_TIME 1
#define SSU_DISABLE_EXECUTE_APPL_FIRST_TIME 0

// PHA PARAMETER NAMES
#if 0
#define SSU_PERF_MON_CLASS_NAME  "PerformanceMonitor"
#define SSU_PERF_MON_RDN_NAME    "performanceMonitorId"
#define SSU_PERF_MON_ALARM_LIMIT "alarmLevelLimit"
#define SSU_PERF_MON_CEASE_LIMIT "ceaseLevelLimit"
#define SSU_PERF_MON_POLL_INTERVEL "pollingInterval"
#define SSU_PERF_MON_DUMP_FREQ   "processDumpFrequency"
#define SSU_PERF_MON_MONITORING_TYPE  "monitoringOnNode"

#define SSU_SMALL_DISK_A2_RDN    "performanceMonitorId=SMALL_DISK_A2"
#define SSU_SMALL_DISK_A1_RDN    "performanceMonitorId=SMALL_DISK_A1"
#define SSU_LARGE_DISK_A2_RDN    "performanceMonitorId=LARGE_DISK_A2"
#define SSU_LARGE_DISK_A1_RDN    "performanceMonitorId=LARGE_DISK_A1"
#define SSU_MEMORY_A2_RDN        "performanceMonitorId=MEM_A2"
#define SSU_MEMORY_A1_RDN        "performanceMonitorId=MEM_A1"
#define SSU_FILEHANDLE_A2_RDN    "performanceMonitorId=FILE_HANDLE_A2"
#endif

#define SSU_LOGICAL_DISK_RDN     "diskMonitorId=DISK"
#define SSU_LOGICAL_SMALLDISK_A1_ALARM  "smallDiskHighAlarmLevelLimit"
#define SSU_LOGICAL_SMALLDISK_A2_ALARM "smallDiskLowAlarmLevelLimit"
#define SSU_LOGICAL_SMALLDISK_A1_CEASE  "smallDiskHighCeaseLevelLimit"
#define SSU_LOGICAL_SMALLDISK_A2_CEASE  "smallDiskLowCeaseLevelLimit"

#define SSU_LOGICAL_LARGEDISK_A1_ALARM  "largeDiskHighAlarmLevelLimit"
#define SSU_LOGICAL_LARGEDISK_A2_ALARM  "largeDiskLowAlarmLevelLimit"
#define SSU_LOGICAL_LARGEDISK_A1_CEASE  "largeDiskHighCeaseLevelLimit"
#define SSU_LOGICAL_LARGEDISK_A2_CEASE  "largeDiskLowCeaseLevelLimit"

#define SSU_LOGICAL_DISK_POLL_INTERVAL  "pollingInterval"
#define SSU_LOGICAL_DISK_DUMP_FREQ      "processDumpFrequency"
#define SSU_LOGICAL_DISK_MONITORING_TYPE "monitoringOnNode"


#define SSU_PERF_MON_MEM_RDN     "performanceMonitorId=MEM"
#define SSU_PERF_MON_FILEHANDLE_RDN "performanceMonitorId=FILE_HANDLE"

#define SSU_PERF_MON_A1_ALARM  "highAlarmLevelLimit"
#define SSU_PERF_MON_A2_ALARM  "lowAlarmLevelLimit"
#define SSU_PERF_MON_A1_CEASE  "highCeaseLevelLimit"
#define SSU_PERF_MON_A2_CEASE  "lowCeaseLevelLimit"

#define SSU_PERF_MON_POLL_INTERVAL  "pollingInterval"
#define SSU_PERF_MON_A1_DUMP_FREQ      "processDumpFrequencyForHigh"
#define SSU_PERF_MON_A2_DUMP_FREQ      "processDumpFrequencyForLow"
#define SSU_PERF_MON_MONITORING_TYPE "monitoringOnNode"


// END

//! Internal Data structures
#include <ace/ACE.h>

#define SSU_PHA_BASICDISKMONDATA    "ACS_SSUBIN_BasicDiskMonData"
#define SSU_PHA_PRODUCT "ACS/CXC1371274"
#define ACS_CSBIN_isMultipleCPSystem "ACS_CSBIN_isMultipleCPSystem"

// Default disk monitor alarm/cease settings parameter ("low" and "high" alarm)
#define SSU_PHA_DEFDISKMONLOW       "ACS_SSUBIN_DefDiskMonLow"
#define SSU_PHA_DEFDISKMONHIGH      "ACS_SSUBIN_DefDiskMonHigh"

#define ACS_SSU_SHELL_PROG  "/bin/sh"

#define ACS_SSU_PERF_MEM_FILE "/tmp/perfmonfree.txt"
#define ACS_SSU_PERF_MEM_CMD "cat /proc/meminfo > /tmp/perfmonfree.txt"


#define ACS_SSU_QUOTA_CHECK_BIN_PATH "/sbin/quotacheck"
#define ACS_SSU_QUOTA_OFF_CMD "quotaoff -g /data"
#define ACS_SSU_QUOTA_UPDATE_CMD "quotacheck -m -g /data"
#define ACS_SSU_QUOTA_ON_CMD "quotaon -g /data"

#define ACS_SSU_PERF_FILE_NR_FILE  "/tmp/perfmonfilehandlecount.txt"
#define ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD "cat /proc/sys/fs/file-nr > /tmp/perfmonfilehandlecount.txt"
#define ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_ALLOCATED "cat /proc/sys/fs/file-nr | awk '{print $1}'"
#define ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_ALLOCATED_BUT_UNUSED "cat /proc/sys/fs/file-nr | awk '{print $2}'"
#define ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_TOTAL "cat /proc/sys/fs/file-nr | awk '{print $3}'"
#define ACS_SSU_RSYSLOG_MEMORY_CHECK_CMD_SCRIPT "/opt/ap/apos/bin/ssu_mem_recovery.sh"


// SSU CONFIG TYPES
#define SSU_BSC_CONFIG_VALUE   				1
#define SSU_BSC_CONFIG   					ACE_TEXT("BSC")
#define SSU_MSC_CONFIG_VALUE   				2
#define SSU_MSC_CONFIG   					ACE_TEXT("MSC")
#define SSU_HLR_CONFIG_VALUE   				3
#define SSU_HLR_CONFIG   					ACE_TEXT("HLR")
#define SSU_HLR_SS_CONFIG_VALUE				4
#define SSU_HLR_SS_CONFIG					ACE_TEXT("HLR_SS")
#define SSU_MSC_BC_AP1_CONFIG_VALUE   		5
#define SSU_MSC_BC_AP1_CONFIG   			ACE_TEXT("MSC-BC-AP1")
#define SSU_MSC_BC_AP2_CONFIG_VALUE   		6
#define SSU_MSC_BC_AP2_CONFIG   			ACE_TEXT("MSC-BC-AP2")
#define SSU_MSC_400_AP1_CONFIG_VALUE        7
#define SSU_MSC_400_AP1_CONFIG              ACE_TEXT("MSC-400-AP1")
#define SSU_MSC_400_AP2_CONFIG_VALUE        8
#define SSU_MSC_400_AP2_CONFIG              ACE_TEXT("MSC-400-AP2")
#define SSU_MSCBC_AP1_8M450_CONFIG_VALUE 	9
#define SSU_MSCBC_AP1_8M450_CONFIG 			ACE_TEXT("MSCBC-AP1-8M450")
#define SSU_MSCBC_AP2_8M450_CONFIG_VALUE  	10
#define SSU_MSCBC_AP2_8M450_CONFIG  		ACE_TEXT("MSCBC-AP2-8M450")
#define SSU_MSCBC_AP1_8M600_CONFIG_VALUE  	11
#define SSU_MSCBC_AP1_8M600_CONFIG  		ACE_TEXT("MSCBC-AP1-8M600")
#define SSU_MSCBC_AP2_8M600_CONFIG_VALUE  	12
#define SSU_MSCBC_AP2_8M600_CONFIG  		ACE_TEXT("MSCBC-AP2-8M600")
#define SSU_MSC_147_AP2_CONFIG_VALUE   		13
#define SSU_MSC_147_AP2_CONFIG   			ACE_TEXT("MSC-147-AP2")
#define SSU_HLR_BS_CONFIG_VALUE				14
#define SSU_HLR_BS_CONFIG                   ACE_TEXT("HLR_BS")
#define SSU_HLR_TSC_CONFIG_VALUE            15
#define SSU_HLR_TSC_CONFIG                  ACE_TEXT("HLR_TSC")
#define SSU_BSC_400_CONFIG_VALUE            16
#define SSU_BSC_400_CONFIG                  ACE_TEXT("BSC-400")
#define SSU_BSP_MSC_BC_AP1_1200_CONFIG_VALUE		17
#define SSU_BSP_MSC_BC_AP1_1200_CONFIG           	ACE_TEXT("BSP-MSCBC-AP1-1200")
#define SSU_BSP_MSC_BC_AP2_1200_CONFIG_VALUE     	18
#define SSU_BSP_MSC_BC_AP2_1200_CONFIG           	ACE_TEXT("BSP-MSCBC-AP2-1200")
#define SSU_HLR_SS_1200_CONFIG_VALUE            	19
#define SSU_HLR_SS_1200_CONFIG                  	ACE_TEXT("HLR-SS-1200")
#define SSU_BSP_HLR_BS_1200_CONFIG_VALUE        	20
#define SSU_BSP_HLR_BS_1200_CONFIG              	ACE_TEXT("BSP-HLR-BS-1200")
#define SSU_HLR_TSC_1200_CONFIG_VALUE           	21
#define SSU_HLR_TSC_1200_CONFIG                 	ACE_TEXT("HLR_TSC_1200")
//Updated for vMSC-300 and vMSC-147
#define SSU_vMSC_300_AP1_CONFIG_VALUE               22
#define SSU_vMSC_300_AP1_CONFIG                     ACE_TEXT("vMSC_300")
#define SSU_vMSC_147_AP1_CONFIG_VALUE               23
#define SSU_vMSC_147_AP1_CONFIG                     ACE_TEXT("vMSC_147")
//Updated for IPSTP
#define SSU_vIPSTP_147_AP1_CONFIG_VALUE             24
#define SSU_vIPSTP_147_AP1_CONFIG                   ACE_TEXT("vIPSTP_147")
#define SSU_IPSTP_CONFIG_VALUE          			25
#define SSU_IPSTP_CONFIG            				ACE_TEXT("IPSTP")
#define vHLR_BS_250_CONFIG_VALUE          27
#define SSU_vHLR_BS_250_CONFIG            ACE_TEXT("vHLR-BS-250")
//Updated for WIRELINE
#define SSU_WLN_147_AP1_CONFIG_VALUE                   26
#define SSU_WLN_147_AP1_CONFIG                         ACE_TEXT("WLN-147")

//Updated for vMSCHC-SMALL-700
#define SSU_vMSCHC_SMALL_700_CONFIG_VALUE               28
#define SSU_vMSCHC_SMALL_700_CONFIG                     ACE_TEXT("vMSCHC_SMALL_700")

//Updated for vMSCHC-MEDIUM-1000
#define SSU_vMSCHC_MEDIUM_1000_CONFIG_VALUE               29
#define SSU_vMSCHC_MEDIUM_1000_CONFIG                    ACE_TEXT("vMSCHC_MEDIUM_1000")

//Updated for vMSCHC-LARGE-1400
#define SSU_vMSCHC_LARGE_1400_CONFIG_VALUE               30
#define SSU_vMSCHC_LARGE_1400_CONFIG                     ACE_TEXT("vMSCHC_LARGE_1400")

//Updated for vMSCHC-LARGE-1450
#define SSU_vMSCHC_LARGE_1450_CONFIG_VALUE               31
#define SSU_vMSCHC_LARGE_1450_CONFIG                     ACE_TEXT("vMSCHC_LARGE_1450")

//Updated for WIRELINE
#define SSU_WLN_400_AP1_CONFIG_VALUE                   32
#define SSU_WLN_400_AP1_CONFIG                         ACE_TEXT("WLN-400-AP1")

//Updated for vBSC with 300GB data disk
#define SSU_vBSC_300_CONFIG_VALUE                   33
#define SSU_vBSC_300_CONFIG                         ACE_TEXT("vBSC-300")

//Updated for vMSCHC-LARGE-1300
#define SSU_vMSCHC_LARGE_1300_CONFIG_VALUE               34
#define SSU_vMSCHC_LARGE_1300_CONFIG                     ACE_TEXT("vMSCHC_LARGE_1300")

#define SSU_HWVER_APG43_1_STRING "APG43"
#define SSU_HWVER_APG43_2_STRING "APG43/2"
#define SSU_HWVER_APG43_3_400_STRING "APG43/3-400"
#define SSU_HWVER_APG43_3_1200_STRING "APG43/3-1200"

#define SSU_HWVER_GEP1_STRING "GEP1"
#define SSU_HWVER_GEP2_STRING "GEP2"
#define SSU_HWVER_GEP5_STRING "GEP5"
#define SSU_HWVER_GEP5_400STRING "GEP5-400"
#define SSU_HWVER_GEP5_1200STRING "GEP5-1200"
#define SSU_HWVER_GEP7_STRING "GEP7"
#define SSU_HWVER_GEP7L_400STRING "GEP7L-400"
#define SSU_HWVER_GEP7L_1600STRING "GEP7L-1600"



//Updated for vMSC-300 and vMSC-147
#define SSU_HWVER_VM_300STRING "VM-300"
#define SSU_HWVER_VM_147STRING "VM-147"
#define SSU_HWVER_VM_250STRING "VM-250"
#define SSU_UNDE_HWVER_STRING "UNDEFINED HARDWARE VERSION"
#define SSU_HWVER_VM_STRING "VM"

//Updated for vMSCHC_SMALL_700
#define SSU_HWVER_VM_700STRING "VM-700"
//Updated for vMSCHC_MEDIUM_1000
#define SSU_HWVER_VM_1000STRING "VM-1000"
//Updated for vMSCHC_LARGE_1300
#define SSU_HWVER_VM_1300STRING "VM-1300"
//Updated for vMSCHC_LARGE_1400
#define SSU_HWVER_VM_1400STRING "VM-1400"
//Updated for vMSCHC_LARGE_1450
#define SSU_HWVER_VM_1450STRING "VM-1450"

//SSu GEP5 DISK SIZE
#define SSU_GEP5_400_DISK_SIZE 400
#define SSU_GEP5_1200_DISK_SIZE 1200

//SSu GEP7 DISK SIZE
#define SSU_GEP7L_400_DISK_SIZE 400
#define SSU_GEP7L_1600_DISK_SIZE 1600

//SSU DISK SIZE
#define SSU_147_DISK_SIZE 147
#define SSU_250_DISK_SIZE 250
#define SSU_300_DISK_SIZE 300
#define SSU_450_DISK_SIZE 450
#define SSU_600_DISK_SIZE 600
#define SSU_700_DISK_SIZE 700
#define SSU_1000_DISK_SIZE 1000
#define SSU_1300_DISK_SIZE 1300
#define SSU_1400_DISK_SIZE 1400
#define SSU_1450_DISK_SIZE 1450

// SSU FileM constants
#define SSU_FILEM_DN                                   	ACE_TEXT("AxeNbiFoldersnbiFoldersMId=1")
#define NBI_ROOT_PATH		                            ACE_TEXT("/data/opt/ap/internal_root")

// SSU Non FileM constants
#define SSU_APZ_DN                                      ACE_TEXT("axeFunctionsId=1")
#define SSU_APZ_APTTYPE                                 ACE_TEXT("axeApplication")
#define SSU_APZ_SYSTEMTYPE                              ACE_TEXT("systemType")
#define SSU_APZ_APNODENUMBER                            ACE_TEXT("apNodeNumber")

// Data buffer sizes and length
#define ACS_SSU_DATA_LEN                     (0x10000 - sizeof(ACE_UINT32))

// IMM Parameters for Folder Quota Monitor
#define CONFIGTYPE 	"prvConfigType"
#define PATH 	   	"prvPath"
#define EVENTNUMBER	"prvEventNumber"
#define QUOTALIMIT	"prvQuotaLimit"
#define A1ALARMLEVEL	"prvA1AlarmLevel"
#define A1CEASELEVEL	"prvA1CeaseLevel"
#define A2ALARMLEVEL	"prvA2AlarmLevel"
#define A2CEASELEVEL	"prvA2CeaseLevel"

//Path to make alarm string map
#define SSU_FILEM_CPPRINTOUT        ACE_TEXT("/data/opt/ap/internal_root/cp/printouts")
#define SSU_FILEM_CPFILES           ACE_TEXT("/data/opt/ap/internal_root/cp/files")
#define SSU_FILEM_MMLCMDFILES       ACE_TEXT("/data/opt/ap/internal_root/cp/mml")
#define SSU_FILEM_AUDITLOG          ACE_TEXT("/data/opt/ap/internal_root/audit_logs")
#define SSU_FILEM_SCRFILES          ACE_TEXT("/data/opt/ap/internal_root/sts_scr")
#define SSU_FILEM_DATATRANSFER      ACE_TEXT("/data/opt/ap/internal_root/data_transfer")
#define SSU_FILEM_BUR               ACE_TEXT("/data/opt/ap/internal_root/backup_restore")
#define SSU_FILEM_SWPKG             ACE_TEXT("/data/opt/ap/internal_root/sw_package")
#define SSU_FILEM_HEALTHCHK         ACE_TEXT("/data/opt/ap/internal_root/health_check")
#define SSU_FILEM_LICENSEFILE       ACE_TEXT("/data/opt/ap/internal_root/license_file")
#define SSU_FILEM_CERTIFICATES       ACE_TEXT("/data/opt/ap/internal_root/certificates")
#define SSU_FILEM_TOOLS             ACE_TEXT("/data/opt/ap/internal_root/tools")
#define SSU_FILEM_SUPPORTDATA       ACE_TEXT("/data/opt/ap/internal_root/support_data")
#define SSU_NONFILEM_FMS	    ACE_TEXT("/data/fms")
#define SSU_NONFILEM_ACA            ACE_TEXT("/data/acs/data/aca")
#define SSU_NONFILEM_CHB            ACE_TEXT("/data/acs/data/chb")
#define SSU_NONFILEM_MTZ            ACE_TEXT("/data/acs/data/mtz")
#define SSU_NONFILEM_RTR            ACE_TEXT("/data/acs/data/rtr")
#define SSU_NONFILEM_SSU            ACE_TEXT("/data/acs/data/ssu")
#define SSU_NONFILEM_AES            ACE_TEXT("/data/aes")
#define SSU_NONFILEM_APZ            ACE_TEXT("/data/apz")
#define SSU_NONFILEM_CPS            ACE_TEXT("/data/cps")
#define SSU_NONFILEM_MCS            ACE_TEXT("/data/mcs")
#define SSU_NONFILEM_OCS            ACE_TEXT("/data/ocs")
#define SSU_NONFILEM_STS            ACE_TEXT("/data/sts")

typedef struct _ACS_SSU_Data
{
   ACE_UINT32 Code;
   ACE_UINT8  Data[ACS_SSU_DATA_LEN];
} ACS_SSU_Data;

typedef struct _SSU_DISKRAID_DATA
{
   ACE_UINT32  Interval;
   ACE_UINT8 Severity[6];
   ACE_UINT8 Command[260];
} SSU_DISKRAID_DATA, *LPSSU_DISKRAID_DATA;

typedef struct _SSU_PERF_DATA
{
   ACE_TCHAR  Object[32];
   ACE_TCHAR  Counter[32];
   ACE_TCHAR  Instance[32];
   bool    AlertIfOver;
   ACE_UINT32  Value;
   ACE_UINT32   Interval;
   ACE_TCHAR  Node[16];
   ACE_UINT32    EventNumber;
   ACE_TCHAR  Severity[6];
   ACE_UINT32  CeaseValue;
   bool    RunFirstTime;
   ACE_TCHAR  Application[260];

} SSU_PERF_DATA, *LPSSU_PERF_DATA;

typedef struct _SSU_DISK_DATA
{
   ACE_TCHAR  Object[32];
   ACE_TCHAR  Counter[32];
   ACE_TCHAR  Instance[32];
   bool    AlertIfOver;
   ACE_UINT32   Interval;
   long    EventNumber;
   bool    RunFirstTime;
   ACE_TCHAR  Application[260];
} SSU_DISK_DATA, *LPSSU_DISK_DATA;

typedef struct _SSU_DISK_ALARM_LEVEL
{
   ACE_TCHAR  Severity[6];
   ACE_TCHAR  SmallDiskSizeLimit[24];
   ACE_UINT32     AlarmLevelSmallDisk;
   ACE_UINT32     CeaseLevelSmallDisk;
   ACE_UINT32     AlarmLevelLargeDisk;
   ACE_UINT32     CeaseLevelLargeDisk;
} SSU_DISK_ALARM_LEVEL, *LPSSU_DISK_ALARM_LEVEL;

typedef struct _SSU_DISK_MONITOR
{
   ACE_TCHAR  Partition[ACS_SSU_PARITION_NAME_MAX];
   ACE_TCHAR  Severity[6];
   ACE_UINT32     AlarmLevel;
   ACE_UINT32     CeaseLevel;
} SSU_DISK_MONITOR, *LPSSU_DISK_MONITOR;

typedef struct _SSU_FOLDERQUOTA_MONITOR
{
   ACE_TCHAR  ConfigType[25];
   ACE_TCHAR  Path[128];
   ACE_TCHAR  symName[128];
   ACE_UINT32     EventNumber;
   ACE_UINT32     QuotaLimit;
   ACE_UINT32     A1AlarmLevel;
   ACE_UINT32     A1CeaseLevel;
   ACE_UINT32     A2AlarmLevel;
   ACE_UINT32     A2CeaseLevel;
} SSU_FOLDERQUOTA_MONITOR, *LPSSU_FOLDERQUOTA_MONITOR;

typedef struct _SSU_FOLDER_OBJECT
{
	ACE_TCHAR  	   symName[128];
	ACE_UINT32     QuotaLimit;
	ACE_UINT32     CurrentConsumption;
	ACE_UINT32     A1AlarmLevel;
	ACE_UINT32     A1CeaseLevel;
	ACE_UINT32     A2AlarmLevel;
	ACE_UINT32     A2CeaseLevel;
} SSU_FOLDER_OBJ;

typedef enum NotificationType
{
   ntQuotaEvent ,
   ntChangeConfig,
   ntFailOver
} NotificationType;



//! Predicate class for Alarm container sorting
template<class T>
class AlarmSortPredicate{
public:
   BOOL operator()( T& oTPerfData1,T& oTPerfData2)
   {
	   return ( ACE_OS::strcmp(oTPerfData1.Severity, oTPerfData2.Severity) < 0 );
   }
};


#endif  // _ACS_SSU_TYPES_H
