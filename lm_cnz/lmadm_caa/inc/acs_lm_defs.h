//******************************************************************************
//
//  NAME
//     ACS_LM_Defs.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR 
//     2008-12-2 by XCSVEMU PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#ifndef ACS_LM_DEFS_H
#define ACS_LM_DEFS_H

#define SERVICE_NAME       "ACS_LM_Server"
#define SERVICE_TITLE      "ACS_LM_Server"

#define CP_JTP_SERVICE              "LMCLI"
#define AP_JTP_SERVICE              "APLM"  //TBR

//Max cmd time out incase peers are not responsive for read/write
//licinst will take more time due to sentinel Initialization and clean up
#define ACS_LM_IO_TIMEOUT           30000
#define SERVICE_WAIT_HINT           (DWORD)5000L
#define ACS_LM_IO_MAX_BUFFER_LEN    2048
#define LM_CMD_SERVICE              "acs_lm_cmdservice"
#define JTP_BUFFER_SIZE				32*1024
#define LM_DOMAIN_LENGTH			255

#define LM_PERSISTENT_BACKUP_PATH  "/cluster/etc/ap/acs/lm/lmdata"
#define LM_LKF_BACKUP_PATH  	   "/cluster/etc/ap/acs/lm/lservrc"
#define LM_BACKUP_DIRECTORY_PATH   "/cluster/etc/ap/acs/lm/"
#define LM_CLUSTER_CONF_FILE       "/cluster/etc/cluster.conf"
#define LM_CMD_DSD_SERVER_NAME         "LMCMDSERVER"
#define LM_CMD_DSD_SERVER_DOMAIN         "LM"

//#define LM_PSA_PATH  "/usr/share/pso/storage-paths/config"
#define LM_PSA_PATH "/data/acs/data"
#define LM_PRIMARY_DIRECTORY_PATH "/data/acs/data/ACS-LM/"
#define LM_PERSISTENT_PRIMARY_PATH  "/data/acs/data/ACS-LM/lmdata"
#define LM_LKF_PRIMARY_PATH	"/data/acs/data/ACS-LM/lservrc"
#define LM_OLD_PRIMARY_PATH	"/storage/system/config/ACS-LM"

#define LM_MIDNIGHT_TRIGGERING_TIME	"T00:05:00"	 

#define ACS_LM_ELECTRONICKEYIMPLEMENTER "LMElectronicKeyImplementer"
#define ACS_LM_DATAIMPLEMENTER "LMDATAImplementer"

#define NETWORK_MAIN_OBJ_DN "northBoundId=1,networkConfigurationId=1"
#define NORTHBOUND_NETWORK_MAIN_CLASS_NAME "NorthBound"
#define MANAGEDELEMENT_RDN  "managedElementId=1"
#define NETWORK_CLUSTER_IP_ADDRESS "clusterIpAddress"
#define NETWORK_MANAGEDELEMENT_ATTR "networkManagedElementId"
#define READ_PARAM_ERROR	"Error reading parameter for APZ from Parameter Database"
#define CAUSE_AP_INTERNAL_FAULT  ACE_TEXT("AP INTERNAL FAULT")
#define ACS_APZ_IMM_APZ_CLASS_NAME "ManagedElement"

/* LM Maintenance Mode */
#define MAINTEMODE_LOCKED	0
#define MAINTEMODE_UNLOCKED	1

#define ELECTRONICKEY_CLASSNAME     "AxeLicenseManagementElectronicKey"
#define ELECTRONICKEY_RDN_ID	  	"electronicKeyId"
#define ELECTRONICKEY_LICENSE_ID   	"licenseKeyNumber"
#define ELECTRONICKEY_VALUE	  		"licensedCapacityValue"
#define ELECTRONICKEY_STARTDATE	  	"startDate"
#define ELECTRONICKEY_EXPIRYDATE	"expiryDate"
#define ELECTRONICKEY_DESCRIPTION	"description"
#define ELECTRONICKEY_DAYS_TO_EXPIRE	"daysToExpire"

#define LM_LOCAL_USRG0_NAME         "Administrators"
#define LM_LOCAL_USRG1_NAME         "ACSADMG"
#define LM_LOCAL_USRG2_NAME         "ACSUSRG"


// LM Runtime attribute names
#define ACS_IMM_LM_ROOT_CLASS_NAME      "AxeLicenseManagementLicenseM"
#define ACS_IMM_LM_ROOT_IMPLEMENTER_NAME      "LMRootImplementer"
#define ACS_IMM_LM_MANAGED_IMPLEMENTER_NAME      "LMManagedImplementer"
#define ACS_IMM_LM_NORTHBOUND_IMPLEMENTER_NAME   "LMNorthBoundImplementer"

#define LICENSEMODE_RDNVALUE         "licenseModeId=1"
#define LICENSEMODE_CLASSNAME        "LicenseMode"

#define LICINITRDN_ATTRIBUTE                 "licenseModeId"
#define FINGERPRINT_ATTRIBUTE                "fingerprint"
#define HASHEDFINGERPRINT_ATTRIBUTE          "hashedFingerprint"	/* Display Fignerprint in hashed format */	
#define EMERGENCYENDDATE_ATTRIBUTE           "endDateforEmergencyMode"
#define EMERGENCYACTIVATIONCOUNT_ATTRIBUTE   "emergencyActivationCount"
#define LMMODE_ATTRIBUTE       		     "licenseMode"
/* LM Maintenance Mode */
#define MAINTENANCEENDDATE_ATTRIBUTE	     "endDateforMaintenanceMode" 
#define MAINTENANCE_UNLOCK		     "maintenanceMode"  
#define ACS_IMM_LM_ROOT_OBJ_DN 		     "AxeLicenseManagementlicenseMId=1"

// LM ACTIONS
#define LM_INSTALL_LSK_FILE_ACTION 1
#define LM_EMERGENCY_UNLOCK_ACTION 2
#define LM_EXPORT_LKF_ACTION	   3   //exportLicenseKeyFile() action.
#define LKF_FILEPATH_ATTRIBUTE "licensefile"


#define LM_MANAGEDELEMENT_ATTRIBUTE	"ManagedElement"
#define LM_SYSTEMFUNCTIONS_ATTRIBUTE	"SystemFunctions"
#define LM_SYSTEMFUNCTIONS_ATTRIBUTE	"SystemFunctions"
#define LM_FILEM_ATTRIBUTE		"FileM"
#define LM_LICENSEFILE_ATTRIBUTE	"LicenseFile"
#define LM_FILEMPATH_ATTRIBUTE		"licenseFile"

#define AP1_NET1_NODEA  "169.254.208.1"
#define AP1_NET1_NODEB  "169.254.208.2"
#define AP1_NET2_NODEA  "192.168.170.1"
#define AP1_NET2_NODEB  "192.168.170.2"
#define AP2_NET1_NODEA  "192.168.169.3"  
#define AP2_NET1_NODEB  "192.168.169.4"
#define AP2_NET2_NODEA  "192.168.170.3"
#define AP2_NET2_NODEB  "192.168.170.4"
#define CXCEMERGENCYMODE  "CXC4011121/112"
#define CXCTESTMODE       "CXC4011121/110"
#define CXC_VIRT_MSC_IPSTP_DF   "CXC4011121/0178"     /*XLUIDIR VAPZ LM*/
#define CXC_VIRT_HLR_DF   "CXC4011442/0502"     /*Dummy License for vHLR*/
#define CXC_VIRT_BSC_DF   "FAT1024333"     /*Dummy License for vBSC*/

#define LM_BRA_PSA_PATH = "/usr/share/pso/storage-paths/config";

enum ACS_LM_MODE
{
	VIRGINMODE = 1 ,
	LICENSECONTROLLEDMODE = 2,
	GRACEMODE = 3,
	EMERGENCYMODE = 4,
        MAINTENANCEMODE = 5,  /* LM Maintenance Mode */ 
};

enum ACS_LM_AppExitCode
{
	ACS_LM_RC_OK = 0,
	ACS_LM_RC_NOK = 1,
	ACS_LM_RC_INCUSAGE = 2,
	ACS_LM_RC_UNREAS = 3,	
	ACS_LM_RC_PHYFILEERROR = 8,	
	ACS_LM_RC_INVALIDPATH = 10,
	ACS_LM_RC_CMD_NOTALLOWED = 19,
	ACS_LM_RC_LKEYALREADYEXISTS = 20,
	ACS_LM_RC_LKEYNOTFOUND = 21,
	ACS_LM_RC_MOREDATA = 22,
	ACS_LM_RC_INVALIDLKF = 23,
    ACS_LM_RC_MISSINGLKF = 24,//--> new code
    ACS_LM_RC_TESTMODENOTALLOWED = 26,
	ACS_LM_RC_EMERGENCYMODENOTALLOWED = 27,
	ACS_LM_RC_EMERGENCYMODEALREADYACTIVATED = 28,
	/* LM Maintenance Mode */
	ACS_LM_RC_MAINTENANCEMODENOTALLOWED = 29,
	ACS_LM_RC_MAINTENANCEMODEALREADYACTIVATED = 30,
	ACS_LM_RC_MAINTENANCEMODEALREADYDEACTIVATED = 31,
	/* exportLicenseKeyFile action error codes */
	ACS_LM_RC_VIRGIN_EXPORTLKFNOTALLOWED = 32,
	ACS_LM_RC_GRACE_EXPORTLKFNOTALLOWED = 33,
	ACS_LM_RC_SRC_NOFILE_EXPORTLKFNOTALLOWED = 34,
	ACS_LM_RC_DEST_NOTWRITE_EXPORTLKFNOTALLOWED = 35,
	ACS_LM_RC_SERVICE_LOCKED = 36,	
        ACS_LM_RC_EXPORTNOTOK = 37, 	
	ACS_LM_RC_BRFINPROGRESS = 114,
    	ACS_LM_RC_SERVERNOTRESPONDING = 117,
	ACS_LM_RC_UNKNOWNCMDTYPE = 118,
	ACS_LM_RC_OTHERERROR = 120
};

enum ACS_LM_AppCmdCode
{
    ACS_LM_CMD_UNKNOWN = 0,
	ACS_LM_CMD_TESTLKACT = 1,
	ACS_LM_CMD_TESTLKADD = 2,
	ACS_LM_CMD_TESTLKREM = 3,
	ACS_LM_CMD_TESTLKLIST = 4,	
	ACS_LM_CMD_TESTLKDEACT = 5,

    ACS_LM_CMD_FPPRINT = 6,
    ACS_LM_CMD_LKINST = 7,
	ACS_LM_CMD_LKLIST = 8,
	ACS_LM_CMD_LKMAPLIST = 9,
    ACS_LM_CMD_LKEMSTART = 10,
    ACS_LM_CMD_SHOWLICENSE = 11
};

enum ACS_LM_AppMode
{
	ACS_LM_MODE_GRACE = 0x0001,
	ACS_LM_MODE_EMERGENCY = 0x0002,
	ACS_LM_MODE_TEST = 0x0004,
	ACS_LM_MODE_VIRGIN = 0x0008, //CNI55_4_744
        ACS_LM_MODE_MAINTENANCE = 0x0010  /* LM Maintenance Mode */ 
};

enum ACS_LM_FileType
{
    ACS_LM_FILE_NOT_FOUND = 0xFFFF,
    ACS_LM_FILE_ISFILE = 0x0001,
    ACS_LM_FILE_ISDIR = 0x0002, 
    ACS_LM_FILE_ACCESSDENIED = 0x0003
};

enum LkSenderType
{
	ACS_LM_SENDER_SERVICE_STARTUP,
    ACS_LM_SENDER_SCHEDULER,
    ACS_LM_SENDER_TESTACT,
	ACS_LM_SENDER_TESTDEACT,
    ACS_LM_SENDER_CPRESTART,
	ACS_LM_SENDER_LKINST,
	ACS_LM_SENDER_EMSTART,
	ACS_LM_SENDER_MAINTENANCE  /* LM Maintenance Mode */ 
};

enum JtpNodeType
{
	ACS_LM_JTP_NODE_TYPE_BC,
	ACS_LM_JTP_NODE_TYPE_CP,
	ACS_LM_JTP_NODE_TYPE_NONE
};

struct JtpNode
{
	JtpNodeType type;
    std::string nodeName;
	int id;
};

struct LkData
{
	int status;
	int value;
	std::string setName;
	std::string lkId;
	std::string paramName;	
	std::string vendorInfo;
    std::string startDate;
    std::string endDate;
};

struct BackupFile
{
	std::string srcPath;
	std::string destPath;
};
/*
struct PipeTrustee
{
    char name[64];
    TRUSTEE_TYPE trusteeType;
};
*/

#endif
