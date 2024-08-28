/*=================================================================== */
/**
   @file   acs_emf_defs.h

   @brief Header file for EMF module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A        DD/MM/YYYY   XRAMMAT   Initial Release
 */
/*==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_EMF_DEFS_H
#define ACS_EMF_DEFS_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>

namespace acs_emf_constants {
	enum  emf_exit_codes {
		EMF_EXITCODE_SUCCESSFUL =0,
		EMF_EXITCODE_INVALIDARGUMENT = 2,
		EMF_EXITCODE_FILENOTFOUND = 3,
		EMF_EXITCODE_PATHNOTFOUND = 4,
		EMF_EXITCODE_EMPTYFILE = 8,
		EMF_EXITCODE_INVALIDPATHNAME = 9,
		EMF_EXITCODE_INVALIDLABELNAME = 10,
		EMF_EXITCODE_OPERATIONINPROGRESS = 14,
		EMF_EXITCODE_PERMISSIONDENIED = 15,
		EMF_EXITCODE_FILEALREADYEXIST = 16,
		EMF_EXITCODE_SOURCETOOLONG =17,
		EMF_EXITCODE_DVDDEVICENOTFOUND = 21,
		EMF_EXITCODE_NOMEDIAINDRIVE = 22,
		EMF_EXITCODE_DVDDRIVEBUSY =23,
		EMF_EXITCODE_LABELNAMETOOLONG=26,
		EMF_EXITCODE_OPTIONNOTVALIDONTHISPLATFORM = 31,
		EMF_EXITCODE_MEDIA_NOTOVERWRITABLE = 34,
		EMF_EXITCODE_TOOMANYDESTINATIONS = 35,
		EMF_EXITCODE_MEDIAACCESSNOTENABLED = 36,
		EMF_EXITCODE_INVALIDDESTINATIONNAME = 41,
		EMF_EXITCODE_FORMATOPTIONNOTVALIDONTHISPLATFORM = 42,
		EMF_EXITCODE_MEDIAACCESSALREADYENABLED = 81,
		EMF_EXITCODE_UNABLETOCONNECTSERV = 117,
		EMF_EXITCODE_OTHERERROR = 255,      //TO BE VERIFIED!!!
	};
};

/* --*-- Return Codes Section --*-- */
#define EMF_RC_OK                            	0
#define EMF_RC_NOK                           	1
#define EMF_RC_UNKNOWNCMDTYPE                	2
#define EMF_RC_OPERATIONINPROGRESS           	3
#define EMF_RC_SERVERNOTRESPONDING           	4
#define EMF_RC_FILENOTFOUND                  	5
#define EMF_RC_PATHNOTFOUND                  	6
#define EMF_RC_FILEALREADYEXIST              	7
#define EMF_RC_FILECREATEERROR               	8
#define EMF_RC_FILEREADERROR                 	9
#define EMF_RC_FILEWRITEERROR                	10
#define EMF_RC_CRYPTHASHERROR                	11
#define EMF_RC_HASHCHECKSUMDIFFER            	12
#define EMF_RC_TAPEDRIVENOTFOUND             	13
#define EMF_RC_DVDDRIVEBUSY                  	14
#define EMF_RC_TAPEDRIVEERROR                	15
#define EMF_RC_TAPEMEDIAERROR                	16
#define EMF_RC_TAPEMEDIAWRITEPROTECTED       	17
#define EMF_RC_TAPEMEDIAFORMATTINGERROR      	18
#define EMF_RC_TAPEMEDIAINVALIDFORMAT        	19
#define EMF_RC_TAPEFILEINVALID               	20
#define EMF_RC_NOTAPEINDRIVE                 	21
#define EMF_RC_TAPEMEDIAEOF                  	22
#define EMF_RC_TAPEMEDIAINVALIDBLOCKSIZE     	23
#define EMF_RC_TAPEMEDIAREADERROR            	24
#define EMF_RC_TAPEMEDIAWRITEERROR           	25
#define EMF_RC_INVALIDFILENAME               	26
#define EMF_RC_INVALIDPATHNAME               	27
#define EMF_RC_INVALIDLABELNAME              	28
#define EMF_RC_FILENAMETOOLONG               	29
#define EMF_RC_FILEPATHTOOLONG               	30
#define EMF_RC_SOURCETOOLONG                 	31
#define EMF_RC_LABELNAMETOOLONG              	32
#define EMF_RC_FILEEMPTY                     	33
#define EMF_RC_INVALIDARGUMENT               	34
#define EMF_RC_PERMISSIONSDENIED             	35
#define EMF_RC_UNKNOWNNODENAME               	36
#define EMF_RC_OVERLAPPED_IO                 	37
#define EMF_RC_DATA                          	38
#define EMF_RC_NO_MORE_DATA                  	39
#define EMF_RC_WAIT_TIMEOUT                  	40
#define EMF_RC_SERVERONACTIVENOTRESPONDING   	41
#define EMF_RC_FILETOLARGE                   	42
#define EMF_RC_CHANGEHWCOMPFAILED            	43
#define EMF_RC_HWCOMPNOTSUPPORTED            	44
#define EMF_RC_CHANGEHWCOMPNOTSUPPORTED      	45
#define EMF_RC_CHANGEHWCOMPBOPONLY           	46
#define EMF_RC_LIBRARYNOTFOUND               	47
#define EMF_RC_PROCNOTFOUNDINLIBRARY         	48
#define EMF_RC_DVDDEVICENOTFOUND             	49
#define EMF_RC_DVDDEVICEBUSY                 	50
#define EMF_RC_DVDNOTASSIGNEDONTHISNODE      	51
#define EMF_RC_GEARLICENSEFILENOTFOUND       	52
#define EMF_RC_GEARDEVICEPARAMINITFAILED     	53
#define EMF_EC_GEARMEDIAPARAMINITFAILED      	54
#define EMF_RC_NOMEDIAINDRIVE                	55
#define EMF_RC_DISCMEDIAERROR                	56
#define EMF_RC_CDFORMATNOTSUPPORTED          	57
#define EMF_RC_DVDFORMATNOTSUPPORTED         	58
#define EMF_RC_ERASEDISCFAILED               	59
#define EMF_RC_NEWVOLUMEFAILED               	60
#define EMF_RC_NEWTRACKFAILED                	61
#define EMF_RC_OPENSESSIONFAILED             	62
#define EMF_RC_COPYTOTRACKFAILED             	63
#define EMF_RC_WRITETRACKTOMEDIAFAILED       	64
#define EMF_RC_WRITETRACKTOIMAGEFILEFAILED   	65
#define EMF_RC_WRITETRACKFROMIMAGEFILEFAILED 	66
#define EMF_RC_DISCNOTAPPENDABLE             	67
#define EMF_RC_DISCNOTERASABLE               	68
#define EMF_RC_DISCFIXATIONFAILED            	69
#define EMF_RC_IMAGEFILEALREADYEXIST         	70
#define EMF_RC_IMAGEFILENOTEXIST             	71
#define EMF_RC_IMAGEFILEFILEERROR            	72
#define EMF_RC_GETDVDDEVICEFAILED            	73
#define EMF_RC_MEDIANOTOVERWRITABLE           	74
#define EMF_CMD_TYPE_COPYFROMMEDIA			 	75
#define EMF_RC_COPYFROMDVDFAILED             	76
#define EMF_RC_MOUNTINGFAILED                	77
#define EMF_RC_UNMOUNTINGFAILED              	78
#define EMF_RC_NOTACTIVENODE                 	79
#define EMF_RC_UNMOUNTMEDIAONACTIVENODE      	80
#define EMF_RC_INVALIDDESTINATIONNAME           81
#define EMF_RC_TOOMANYDESTINATIONS              82
#define EMF_RC_COMMANDNOTVALIDONTHISPLATFORM 	197
#define EMF_RC_OPTIONNOTVALIDONTHISPLATFORM  	198
#define EMF_RC_OPERATIONCANCELLEDBYUSER      	199
#define EMF_RC_ENDEVENTSIGNALED              	200
#define EMF_RC_UNABLETOCONNECTSERV           	201
#define EMF_RC_CANNOTCREATEEMFFOLDER         	202
#define EMF_RC_CANNOTRECEIVEROMSERVER        	204
#define EMF_RC_CANNOTSENDCMDTOSERVER         	205
#define EMF_RC_ERRORRECEIVINGFROMSERVER	     	206
#define EMF_RC_MEDIAACCESSNOTENABLED         	207
#define EMF_RC_MEDIAACCESSALREADYENABLED     	208
#define EMF_RC_MEDIAMOUNTEDONOTHERDIR       	209
#define EMF_RC_MEDIANOTMOUNTED                  210
#define EMF_RC_FORMATOPTIONNOTVALIDONTHISPLATFORM  211
#define EMF_RC_CANNOTACCESSTOMEDIA              212
#define EMF_RC_OTHERERROR                    	255
#define EMF_RC_EMPTYLABEL		    			256
#define EMF_RC_ERROR							-1
#define UNKNOWN									-1

/* --*-- General Section --*-- */
#define EMF_DEFAULT_SOCKSERV_PORTNO          	14020

#define EMF_INVALID_FILENAME_CHARS           	ACE_TEXT("<>:\"\\|\?*")
#define EMF_INVALID_PATHNAME_CHARS           	ACE_TEXT("<>:\"|\?*")
#define EMF_VALID_FILENAME_CHARS             	ACE_TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_")
#define EMF_VALID_LABEL_CHARS                	ACE_TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_")

#define EMF_MAX_SOURCE     						256
#define EMF_CMD_MAXLEN    						1024
#define EMF_LCMD_MAXLEN    						10000
#define EMF_CMD_COPY_BUFF_LEN					1024
#define EMF_IPADDRESS_MAXLEN					256
#define EMF_DEVNAME_MAXLEN	    				64
#define ACS_EMF_IMM_DN_PATH_SIZE_MAX 			512
#define ACS_EMF_ARRAYSIZE						512
#define ACS_EMF_IMM_RDN_PATH_SIZE_MAX 			256
#define ACS_EMF_MAX_LENGTH						128
#define EMF_MAX_PATH 							255
#define EMF_COPY_MAX_NUMFILES 					100
#define ACS_EMF_USBSIZE_KBYTES					33554430
#define ACS_EMF_EMPTY_USBSIZE_KBYTES			4096
#define EMF_MEDIATYPE_MAXLEN 					32
#define EMF_SIZE_DATA							20
#define EMF_IMAGENAME_MAXLEN					128

#define ACS_EMF_DATA_LEN                     	(0x10000 - sizeof(DWORD))

#define EMF_LOCAL_USRG1_NAME                 	ACE_TEXT("Administrators")
#define EMF_LOCAL_USRG2_NAME                 	ACE_TEXT("APBACKUPUSRG")
#define EMF_LOCAL_USRG3_NAME                 	ACE_TEXT("SYSTEM")

#define EMF_LOCAL_USRG1_COMMENT              	ACE_TEXT("Administrators have complete and unrestricted access to the computer/domain")
#define EMF_LOCAL_USRG2_COMMENT              	ACE_TEXT("AP Backup User Group, According to 2/10260-ANZ22203")
#define EMF_NODEA                            	ACE_TEXT("SC-2-1")
#define EMF_NODEB                            	ACE_TEXT("SC-2-2")

#define theEMFInfoMediaType    					ACE_TEXT("mediaType")
#define theEMFInfoTotalSize						ACE_TEXT("totalSizeOfMedia")
#define theEMFInfoUsedSpace						ACE_TEXT("usedSpaceOnMedia")
#define theEMFInfoFreeSpace						ACE_TEXT("freeSpaceOnMedia")
#define theEMFInfoMediaOwner					ACE_TEXT("ownerOfMedia")
#define theEMFLastUpdatedTime               	ACE_TEXT("lastUpdatedTime")
#define theEMFStateOfOperation              	ACE_TEXT("stateOfOperation")
#define theEMFResultOfOperation					ACE_TEXT("resultOfOperation")
#define theEMFOperationalState              	ACE_TEXT("operationalState")
#define theEMFMediaState			ACE_TEXT("mediaState")

#define theEMFStruId                        	ACE_TEXT("id")
#define theEMFStruState                     	ACE_TEXT("state")
#define theEMFStruLastUpdatedTime           	ACE_TEXT("lastUpdatedTime")
#define theEMFStruActionEndTime             	ACE_TEXT("actionEndTime")
#define theEMFStruActionStartTime           	ACE_TEXT("actionStartTime")
#define theEMFStruResultInfo                	ACE_TEXT("resultInfo")
#define theEMFStruResult                    	ACE_TEXT("result")
#define theEMFStruActionName                	ACE_TEXT("actionName")
#define theEMFStruRDN                       	ACE_TEXT("id=emfInstance")

#define RUNNING									2
#define FINISHED								3	
#define EXPORT_TO_MEDIA							1
#define FORMAT_AND_EXPORT_TO_MEDIA				2
#define IMPORT_FROM_MEDIA						3
#define LOCK_MEDIA             					4
#define UNLOCK_MEDIA           					5
#define SYNC_MEDIA								6
#define FORMAT_USBINT              				1
#define FORMAT_USBEXT              				2
#define SUCCESS									0
#define FAILURE									1
#define NOT_AVAIALABLE							2
#define EMF_CMD_UNLOCK_MEDIA     				7
#define EMF_CMD_LOCK_MEDIA   					8

#define MEDIA_FEATURE_DISABLE  					0
#define MEDIA_FEATURE_ENABLE   					1

#define MEDIA_ON_CURRENT_NODE  					0
#define MEDIA_ON_PARTNER_NODE  					1
#define NO_MEDIA              					-1

#define theEMFHistoryInfoClassName 			ACE_TEXT("AxeExternalMediaExternalMediaHistory")
#define theEMFHistoryInfoRDN			    	ACE_TEXT("externalMediaHistoryId")

// Media status
#define EMF_MEDIA_NOK						 -1
#define EMF_MEDIA_SUCCESS					  0
#define EMF_MEDIA_MOUNTED					  1
#define EMF_MEDIA_NOTMOUNTED			      2
#define EMF_MEDIA_MOUNTEDBAD			      3
#define EMF_MEDIA_UMOUNTREQ				      4

#define theEMFHistoryInfoNodeName    		    ACE_TEXT("nodeName")
#define theEMFHistoryInfoNodeState				ACE_TEXT("operationState")
#define theEMFHistoryInfoLastOperation			ACE_TEXT("operationType")
#define theEMFHistoryInfoOperResult				ACE_TEXT("operationResult")
#define theEMFHistoryInfoUser					ACE_TEXT("user")
#define theEMFHistoryInfoStartTime				ACE_TEXT("operationStartTime")
#define theEMFHistoryInfoEndTime				ACE_TEXT("operationEndTime")
#define theEMFHistoryInfoFileName				ACE_TEXT("fileNames")
#define theEMFHistoryInfoStatus					ACE_TEXT("status")

#define ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN    ACE_TEXT("externalMediaHistoryId=CURRENT")
#define ACS_EMF_FIRST_HISTORY_INSTANCE_RDN		ACE_TEXT("externalMediaHistoryId=1")
#define SUCCESSFUL								ACE_TEXT("Successful")
#define FAILED									ACE_TEXT("Failed")
#define DONE 									ACE_TEXT("Done")
#define IDLE									ACE_TEXT("IDLE")
#define PROCESSING								ACE_TEXT("PROCESSING")

#define FROMDVD  								0
#define TODVD  									1

#define DVD_IDLE 								0
#define DVD_PROCESSING 							1

#define ACS_EMF_IMM_ROOT_DN 					"id=emfInstance,AxeExternalMediaexternalMediaMId=1"
#define ACS_EMF_ROOT_CLASS_NAME         		"AxeExternalMediaExternalMediaM"
#define ACS_EMF_ROOT_IMPL_NAME          		"acs_emf_root_impl"

#define ACS_EMF_DATA_FOLDER						ACE_TEXT("/emf")
#define ACS_EMF_MNT_DIR							ACE_TEXT("/mnt")

#define POPEN_READ 								0
#define POPEN_WRITE 							1
#define AP1_SYSID       						2001
#define AP2_SYSID								2002

/* --*-- Bool Section --*-- */
#ifndef TRUE
#define TRUE 									1
#endif

#ifndef FALSE
#define FALSE 									0
#endif

/* --*-- NBI PATH Section --*-- */
#define NBI_tools                              	"/tools"
#define NBI_swPackageSCXB                      	"/sw_package/SCXB"
#define NBI_swPackageIPTB                      	"/sw_package/IPTB"
#define NBI_swPackageFW                        	"/sw_package/FW"
#define NBI_swPackageEvoET                     	"/sw_package/EvoET"
#define NBI_swPackageEPB1                      	"/sw_package/EPB1"
#define NBI_swPackageCP                        	"/sw_package/CP"
#define NBI_swPackageCMXB                      	"/sw_package/CMXB"
#define NBI_swPackageAPG                       	"/sw_package/APG"
#define NBI_swPackage                          	"/sw_package"
#define NBI_supportData                        	"/support_data"
#define NBI_sourceDataForStsFile               	"/data_transfer/source/sts"
#define NBI_sourceDataForCpFile                	"/data_transfer/source/cp_file"
#define NBI_scrFiles                           	"/sts_scr"
#define NBI_mmlCommandsFile                    	"/cp/mml"
#define NBI_mirroring                          	"/data_transfer/data_mirrored"
#define NBI_licenseFile                        	"/license_file"
#define NBI_healthCheck                        	"/health_check"
#define NBI_dataTransferForResponding          	"/data_transfer"
#define NBI_cpPrintout                         	"/cp/printouts"
#define NBI_cpFiles                            	"/cp/files"
#define NBI_backupRestore                      	"/backup_restore"
#define NBI_auditLog                           	"/audit_logs"
#define NBI_root 	             				"/data/opt/ap/internal_root"
#define NBI_backupRestoreCompPath              	"/data/opt/ap/internal_root/backup_restore"
	
#define MediaType 								ACE_TEXT("THUMBDRIVE")
#define MasterTD_Pos							0
#define Sync_Pos								2	
#define NO_Sync									2
#define Sync									1
#define formatPosition							4
#define ACS_EMF_CLUSTER     					"/cluster"
//TR HU81933
//#define ACS_EMF_THUMBDRIVE						"/dev/sdm"
#define ACS_EMF_THUMBDRIVE						"/dev/eri_thumbdrive"
#define ACS_EMF_ACTIVE_DATA						"/active_data"
#define ACS_EMF_ACTIVE_MEDIA 					"/data/opt/ap/internal_root/media"
#define ACS_EMF_PASSIVE_MEDIA 					"/media"
#define ACS_EMF_LOST_FOUND      				"lost+found"
#define ACS_EMF_SLASH                   		"/"
#define ACS_EMF_CURRENT         				"."
#define ACS_EMF_ASTERISK        				"*"
#define ACS_EMF_CLUSTER_ACTIVEDATA         		"/cluster/active_data"


/* --*-- File Section --*-- */
#define ACS_EMF_FILE_NODE_ID               		"/etc/cluster/nodes/this/id"
#define ACS_EMF_FILE_PEER_ID                	"/etc/cluster/nodes/peer/id"
#define ACS_EMF_FILE_NODE_NAME              	"/etc/cluster/nodes/this/hostname"
#define ACS_EMF_FILE_PROC_MOUNTS				"/proc/mounts"

/* --*-- Command Section --*-- */
#define ACS_EMF_CMD_GNRL_OPERATIONS         	"/opt/ap/acs/bin/acs_emf_generalOps.sh"
#define ACS_EMF_CMD_NFS_OPERATIONS          	"/opt/ap/acs/bin/acs_emf_nfsOps.sh"
#define ACS_EMF_CMD_NFS_REMOVESHARE          	"/opt/ap/acs/bin/acs_emf_remove_passivedvd.sh"
#define EMF_CMD_TYPE_COPYTOTAPE              	1
#define EMF_CMD_TYPE_COPYFROMTAPE            	2
#define EMF_CMD_TYPE_COPYTOMEDIA               	3
#define EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA       	4
#if 0
#define EMF_CMD_TYPE_COPYIMAGEFILETODVD      	4
#endif
#define EMF_CMD_TYPE_INITCOPYTODVDREMOTE     	5
#define EMF_CMD_TYPE_FINISHEDCOPYTODVDREMOTE 	6
#define EMF_CMD_TYPE_CANCELCOPYTODVDREMOTE   	7
#define EMF_CMD_TYPE_INITCOPYFILEDATA        	8
#define EMF_CMD_TYPE_COPYFILEDATA            	9
#define EMF_CMD_TYPE_ENDCOPYFILEDATA         	10

#define EMF_CMD_TYPE_GETMEDIAINFO            	13
#define EMF_CMD_TYPE_GETDVDOWNER             	14
#define EMF_CMD_TYPE_GETDVDOWNERSHIP         	15
#define EMF_CMD_TYPE_SYNCMEDIA				 	16
#define EMF_CMD_TYPE_CLEANUPANDLOCK          	36

/* --*-- Command Options --*-- */
#define ACS_EMF_COPY_OPTS						" --copy"
#define ACS_EMF_FORMAT_OPTS						" --format &>/dev/null"
#define ACS_EMF_UNMOUNT_OPTS					" --unmount"
#define ACS_EMF_MOUNT_OPTS						" --mount"
#define ACS_EMF_CHCK_USB_OPTS					" --is-usb-available &>/dev/null"
#define ACS_EMF_LABL_APPLY_OPTS					" --apply-label"
#define ACS_EMF_CHCK_FS_OPTS					" --check-fs"
#define ACS_EMF_RMVE_OPTS						" --remove"
#define ACS_EMF_CLEANUP_OPTS					" --clean-up" 
#define ACS_EMF_MOUNT_NFS_OPTS					" --mount-nfs" 
#define ACS_EMF_EXPORT_NBI_OPTS					" --export-nbi &>/dev/null" 
#define ACS_EMF_EXPORT_MEDIA_OPTS				" --export-media &>/dev/null" 
#define ACS_EMF_RMVE_NBI_OPTS					" --remove-nbi &>/dev/null" 
#define ACS_EMF_RMVE_MEDIA_OPTS					" --remove-media" 

/* --*-- typedef Section --*-- */
typedef ACE_UINT32 DWORD;
typedef bool BOOL;
typedef ACE_UINT64 ULONG;
typedef ACE_INT64 LONG;
typedef ACE_TCHAR* LPBYTE;
typedef ACE_TCHAR BYTE;

typedef  enum  {
	EMF_MEDIA_OPERATION_IS_IDLE,
	EMF_TOMEDIA_OPERATION_IS_IN_PROGRESS,
	EMF_FROMMEDIA_OPERATION_IS_IN_PROGRESS,
	EMF_GETMEDIA_INFO_IS_IN_PROGRESS,
	EMF_GETOWNER_INFO_IS_IN_PROGRESS,
	EMF_MOUNT_OPERATION_IS_IN_PROGRESS,
	EMF_UNMOUNT_OPERATION_IS_IN_PROGRESS,
	EMF_GETDRIVE_OWNERSHIP_IS_IN_PROGESS,
	EMF_SYNC_OPERATION_IS_IN_PROGRESS,
	EMF_ERASETOMEDIA_OPERATION_IS_IN_PROGRESS
} acs_emf_operation_status_t;

/* This lines have to be moved to IMM handling Class */
typedef enum imm_result_constants {
	EMF_IMM_RESULT_SUCCESS = 0,
	EMF_IMM_RESULT_FAILURE = 1
} emf_imm_result_t;

/* Structure to send command data between a command and ACS_EMF_Server */
typedef struct _ACS_EMF_Data {
	DWORD Code;
	BYTE  Data[ACS_EMF_DATA_LEN];
} ACS_EMF_DATA;

typedef struct _ACS_EMF_File {
	ACE_TCHAR Filename[EMF_MAX_PATH];
	BOOL   bIsDirectory;
} ACS_EMF_File;

#endif

