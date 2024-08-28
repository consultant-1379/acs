/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

#ifndef _LSERV_H_
#define _LSERV_H_
/*H****************************************************************
* FILENAME    : lserv.h
*
* DESCRIPTION :
*           Contains public function prototypes, macros and defines
*           needed for licensing an app using Sentinel RMS Developer
*           Kit library.
* USAGE       :
*           This file should be included by all users of Sentinel RMS
*           Developer Kit client library.
*
*H*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>     /* For definition of FILE*  */

#ifndef _RMS_TIME_DEFINITION_
#define _RMS_TIME_DEFINITION_
   #if (defined _VMSWIN_) || defined(_WIN64) || defined(_MSC_VER)   /* for windows */ 
      typedef __int64 Time64_T;
      #define TIME64_PRINT_FORMAT   "%I64d"
      #if defined(_WIN64)
         typedef __int64 Time_T;				/* It is not 'long' because on 64 bit windows long is still 32 bit.*/
      #else
         typedef long Time_T;
      #endif
   #else
      typedef long Time_T;						/* Time_T is changed to 32/64 bit correctly for non-windows
                                               because 'long' varies automatically to 32/64 bits */
      #if (defined _TRU64_) || (defined _V_LP64_)	/* 64 bit non-windows system */
         typedef long Time64_T;
         #define TIME64_PRINT_FORMAT   "%ld"
      #else    /* 32 bit non-windows */
         typedef long long Time64_T;
         #define TIME64_PRINT_FORMAT   "%lld"
      #endif
   #endif   /* end for Time64_T definition */
#endif	/* _RMS_TIME_DEFINITION_ */ 

   /*
    * Developer can compile with LSNOPROTO to force no prototyping.
    */

#if defined(_VWIN95_) || defined(_VWINNT_) || defined(_VWINCE_)
   /* 32-bit MS Windows application */
   /* This section is for internal use.  Do not define _VWIN95_ or _VWINNT_ */
# define VMSWINAPI
# define LSFAR
#ifdef _SLMSLIB_
#  define VDLL32
#else
#  define VDLL32    __declspec(dllexport)
#endif /* _SLMSLIB_ */
#else /* (_VWIN95_) || (_VWINNT_) || (_VWINCE_) */
#if defined(_VMSWIN_)
# define VMSWINAPI __cdecl
#else
# define VMSWINAPI
#endif
# define LSFAR
# define VDLL32
#endif /* MS Windows */

#if defined(_VMSWIN_) || defined (WIN32)
#pragma pack(push, 1)
#endif /*_VMSWIN_*/

   /*------------------------------------------------------------------------*/
   /* To inactivate licensing completely, use the following macro which      */
   /* will make all Sentinel RMS Development Kit functions void:             */
   /*------------------------------------------------------------------------*/

   /*
   #define NO_LICENSE
   */

   /*------------------------------------------------------------------------*/
   /* LSAPI constants                                                        */
   /*------------------------------------------------------------------------*/

#define  LS_DEFAULT_UNITS          (unsigned long)0xFFFFFFFF
#define  LS_ANY                    ((unsigned char LSFAR *)NULL)
#define  LS_USE_LAST               (unsigned long)0x0800FFFF

   /*------------------------------------------------------------------------*/
   /* Standalone mode constants                                              */
   /*------------------------------------------------------------------------*/

#define VLS_STANDALONE             "no-net"
#define VLS_NETWORK_MODE              0
#define VLS_STANDALONE_MODE           1
#define VLS_PERPETUAL_MODE            2
#define VLS_REPOSITORY_MODE           VLS_PERPETUAL_MODE
#define VLS_INVALID_STANDALONE_FLAG  -1


   /*------------------------------------------------------------------------*/
   /* Trace level                                                            */
   /*------------------------------------------------------------------------*/

#define VLS_NO_TRACE                0
#define VLS_TRACE_KEYS              1
#define VLS_TRACE_FUNCTIONS         2
#define VLS_TRACE_ERRORS            4
#define VLS_TRACE_ALL               (VLS_TRACE_KEYS | VLS_TRACE_FUNCTIONS | \
                                    VLS_TRACE_ERRORS)   /*i.e. 7*/

#define VLS_TRACE_DEFAULT           VLS_NO_TRACE

   /*------------------------------------------------------------------------*/
   /* Error handling                                                         */
   /*------------------------------------------------------------------------*/

#define VLS_ON                     1  /* This is the default value */
#define VLS_OFF                    0
#define VLS_CONTROL_RDP_OFF        2  /* Default for RDP */
#define VLS_CONTROL_RDP_ON         3


   /*------------------------------------------------------------------------*/
   /* Error handling                                                         */
   /*------------------------------------------------------------------------*/
#define VLS_EH_SET_ALL             0

   /*------------------------------------------------------------------------*/
   /* True/False                                                             */
   /*------------------------------------------------------------------------*/

#define VLS_TRUE                   0
#define VLS_FALSE                  1

   /*------------------------------------------------------------------------*/
   /* Additive property flags                                                */
   /*------------------------------------------------------------------------*/
#define VLS_ADDITIVE                0
#define VLS_EXCLUSIVE               1
#define VLS_AGGREGATE               2

   /*------------------------------------------------------------------------*/
   /*  Licnse Type flags                                                     */
   /*------------------------------------------------------------------------*/
#define VLS_TRIAL_LIC                    1
#define VLS_NORMAL_LIC                   0
   /*------------------------------------------------------------------------*/
   /*  Commuter lic                                                          */
   /*------------------------------------------------------------------------*/
#define VLS_COMMUTED_CODE                 2
#define VLS_ISSUE_COMMUTER_CODES          1
#define VLS_NOT_ISSUE_COMMUTER_CODES      0
#define VLS_COMMUTERDAYS_UNRESTRICTED     0
   /*------------------------------------------------------------------------*/
   /*  Redundant flag                                                        */
   /*------------------------------------------------------------------------*/
#define  VLS_REDUNDANT_CODE               1
#define  VLS_NON_REDUNDANT_CODE           0
   /*------------------------------------------------------------------------*/
   /*  Majority rule flag                                                    */
   /*------------------------------------------------------------------------*/

#define VLS_MAJORITY_RULE_FOLLOWS         1
#define VLS_MAJORITY_RULE_NOT_FOLLOWS     0
   /*------------------------------------------------------------------------*/
   /* Sharing criteria                                                       */
   /*------------------------------------------------------------------------*/

#define VLS_NO_SHARING                    0
#define VLS_USER_NAME_ID                  1
#define VLS_CLIENT_HOST_NAME_ID           2
#define VLS_X_DISPLAY_NAME_ID             3
#define VLS_VENDOR_SHARED_ID              4
#define VLS_NO_SHARING_STRING            "0"
#define VLS_USER_NAME_ID_STRING          "1"
#define VLS_CLIENT_HOST_NAME_ID_STRING   "2"
#define VLS_X_DISPLAY_NAME_ID_STRING     "3"
#define VLS_VENDOR_SHARED_ID_STRING      "4"
   /*------------------------------------------------------------------------*/
   /* Team creation criteria                                                 */
   /*------------------------------------------------------------------------*/

#define VLS_NO_TEAM                       VLS_NO_SHARING
#define VLS_USER_NAME_BASED_TEAM          VLS_USER_NAME_ID
#define VLS_HOST_NAME_BASED_TEAM          VLS_CLIENT_HOST_NAME_ID
#define VLS_X_DISPLAY_BASED_TEAM          VLS_X_DISPLAY_NAME_ID
#define VLS_VENDOR_DEFINED_TEAM           VLS_VENDOR_SHARED_ID

#define VLS_NO_TEAM_STRING                VLS_NO_SHARING_STRING
#define VLS_USER_NAME_BASED_TEAM_STRING   VLS_USER_NAME_ID_STRING
#define VLS_HOST_NAME_BASED_TEAM_STRING   VLS_CLIENT_HOST_NAME_ID_STRING
#define VLS_X_DISPLAY_BASED_TEAM_STRING   VLS_X_DISPLAY_NAME_ID_STRING
#define VLS_VENDOR_DEFINED_TEAM_STRING    VLS_VENDOR_SHARED_ID_STRING

   /*------------------------------------------------------------------------*/



   /*------------------------------------------------------------------------*/
   /* Holding criteria                                                       */
   /*------------------------------------------------------------------------*/

#define VLS_HOLD_NONE              0
#define VLS_HOLD_VENDOR            1
#define VLS_HOLD_CODE              2
#define VLS_HOLD_NONE_STRING      "0"
#define VLS_HOLD_VENDOR_STRING    "1"
#define VLS_HOLD_CODE_STRING      "2"

   /*------------------------------------------------------------------------*/
   /* Client-server lock mode                                                */
   /*------------------------------------------------------------------------*/

#define VLS_NODE_LOCKED            0
#define VLS_FLOATING               1
#define VLS_DEMO_MODE              2
#define VLS_CLIENT_NODE_LOCKED     3

   /*------------------------------------------------------------------------*/
   /* Locking criteria                                                       */
   /*------------------------------------------------------------------------*/

   /* Test whether a particular locking criterion is being used. */
#define VLS_LOCK_TO_ID_PROM(V)        (( (V) >> 0 ) & 0x1)
#define VLS_LOCK_TO_IP_ADDR(V)        (( (V) >> 1 ) & 0x1)
#define VLS_LOCK_TO_DISK_ID(V)        (( (V) >> 2 ) & 0x1)
#define VLS_LOCK_TO_HOSTNAME(V)       (( (V) >> 3 ) & 0x1)
#define VLS_LOCK_TO_ETHERNET(V)       (( (V) >> 4 ) & 0x1)
#define VLS_LOCK_TO_NW_IPX(V)         (( (V) >> 5 ) & 0x1)
#define VLS_LOCK_TO_NW_SERIAL(V)      (( (V) >> 6 ) & 0x1)
#define VLS_LOCK_TO_PORTABLE_SERV(V)  (( (V) >> 7 ) & 0x1)
#define VLS_LOCK_TO_CUSTOM(V)         (( (V) >> 8 ) & 0x1)
#define VLS_LOCK_TO_CPU(V)            (( (V) >> 9 ) & 0x1)
#define VLS_LOCK_TO_CUSTOMEX(V)       (( (V) >> 10) & 0x1)
#define VLS_LOCK_TO_HARD_DISK_SERIAL(V)  (( (V) >> 11 ) & 0x1)
#define VLS_LOCK_TO_CPU_INFO(V)          (( (V) >> 12 ) & 0x1)
#define VLS_LOCK_TO_UUID(V)              (( (V) >> 13 ) & 0x1)

   /* To set a particular locking criterion. */
#define VLS_LOCK_ID_PROM        0x1
#define VLS_LOCK_IP_ADDR        0x2
#define VLS_LOCK_DISK_ID        0x4
#define VLS_LOCK_HOSTNAME       0x8
#define VLS_LOCK_ETHERNET       0x10
#define VLS_LOCK_NW_IPX         0x20
#define VLS_LOCK_NW_SERIAL      0x40
#define VLS_LOCK_PORTABLE_SERV  0x80
#define VLS_LOCK_CUSTOM         0x100
#define VLS_LOCK_CPU            0x200
#define VLS_LOCK_CUSTOMEX       0x400
#define VLS_LOCK_HARD_DISK_SERIAL   0x800
#define VLS_LOCK_CPU_INFO       0x1000
#define VLS_LOCK_UUID           0x2000

	/* To disable Revocation or commuter functionalities. */
#define VLS_DISABLE_OLD_NW              0x1
#define VLS_DISABLE_COMM_MIGRATION      0x2
#define VLS_ENABLE_DIFF_BACKUP_PERSISTENCE_LOCATION 0x4

   /* Highest bit currently in use : */
#define VLS_LOCK_HIGHEST_BIT    14    /* Starting from 1... */
   /* Mask with all locking criteria set. */
#define VLS_LOCK_ALL            0x3FFF
#define VLS_LOCK_NONET          VLS_LOCK_ALL^VLS_LOCK_IP_ADDR^VLS_LOCK_PORTABLE_SERV
   /* The maximum count that customEx lock criteria can be collected */
#define VLS_MAX_CUSTOMEX_COUNT  8

   /* Max size of string which is generated through machineID converting. If the data
    * isn't string, then it should be formated as string against hex format.So for non-string
    * data type, its size may be 2*sizeof(non-string type). say 0xff -> "ff", it's 1 byte to
    * 2 bytes.
    */
#define VLS_MACHINEID_STRING_SIZE   (sizeof(VLSmachineID) + 7*sizeof(unsigned long) + sizeof(VLScustomEx))

#define VLS_HASHED_MACHINEID_STRING_SIZE   (sizeof(VLShashedMachineID))

   /* The maximum size of lock code, include '\0'. (bytes) */
#define VLS_LOCK_CODE_SIZE      17

   /*------------------------------------------------------------------------*/
   /* License does not have an expiration date                               */
   /*------------------------------------------------------------------------*/

#define VLS_NO_EXPIRATION          -1

   /*------------------------------------------------------------------------*/
   /* This number represents infinite keys                                   */
   /*------------------------------------------------------------------------*/

#define VLS_INFINITE_KEYS          0x1FFFFE /*2,097,150 Hard Limit*/
#define VLS_KEY_MAX_LIMIT          0xFFFFFFFE /* 4,294,967,294 Hard Limit 9.5.0 onwards */
#define VLS_INFINITE_KEYS_STRING   ""         /* Same representation is used for all license versions */

   /*------------------------------------------------------------------------*/
   /* Maximum size of machine finger print of remote machine                 */
   /*------------------------------------------------------------------------*/

#define MAX_FINGER_PRINT (VLS_MACHINEID_STRING_SIZE+VLS_LOCK_HIGHEST_BIT)*2+4

   /*------------------------------------------------------------------------*/
   /* Type definitions                                                       */
   /*------------------------------------------------------------------------*/
#if (defined _TRU64_ || defined _V_LP64_)
   /* On 64 bit system, these must be 32 bit int */
   typedef  unsigned int                     LS_STATUS_CODE;
   typedef  unsigned int                     LS_HANDLE;
   typedef  unsigned int                     QUEUE_HANDLE;
#else
   typedef  unsigned long                     LS_STATUS_CODE;
   typedef  unsigned long                     LS_HANDLE;
   typedef  unsigned long                     QUEUE_HANDLE;
#endif

#define  VLS_MAX_NAME_LEN  128
#define  VLS_MAX_BUF_LEN   1024
#define  VLS_MAX_ENCRYPTION_LEVEL     4
#define  VLS_MAX_CPU_ID_LEN 24
#define  VLS_MAX_CPU_INFO_LEN 1024
#define  VLS_MAX_UUID_LEN 36

#if !defined(VLS_NOCOMPAT)
#define MAX_NAME_LEN VLS_MAX_NAME_LEN
#define MAX_BUF_LEN  VLS_MAX_BUF_LEN
#define MAX_ENCRYPTION_LEVEL VLS_MAX_ENCRYPTION_LEVEL
#define MAX_CPU_ID_LEN  VLS_MAX_CPU_ID_LEN
#endif

#define VLS_DISC_NO_OPTIONS       0
#define VLS_DISC_RET_ON_FIRST     1
#define VLS_DISC_PRIORITIZED_LIST 2
#define VLS_DISC_NO_USERLIST      4
#define VLS_DISC_REDUNDANT_ONLY   8
#define VLS_DISC_DEFAULT_OPTIONS  VLS_DISC_NO_OPTIONS

#define NO_RET_ON_FIRST 0
#define RET_ON_FIRST 1

#define VLS_REQ_GET        01
#define VLS_REQ_QUEUE      02
#define VLS_COMMUTER_GET   04
#define VLS_GRACE_REQ      8
#define VLS_REQ_GET_SW     16 /* switched re-request */
   /* Don't use this value in your code*/
#define VLS_CAPACITY_GET   8

#define VLS_SERV_LOCALE_STR_LEN   35
#define VLS_SERV_VNDINFO_STR_LEN  50
#define VLS_SERV_PLATFORM_STR_LEN 20
#define VLS_SERV_UNUSED1_STR_LEN  20


#define VLS_GET_ETHERNET            0x1
#define VLS_GET_CID                 0x2
#define VLS_GET_CUSTOMEX            0x4
#define VLS_GET_HARD_DISK_SERIAL    0x8


   typedef enum {VLS_LOCAL_UPD_ENABLE, VLS_LOCAL_UPD_DISABLE} VLS_LOC_UPD_STAT;

   /* commuter */

#define HOSTID_ARRAY_SIZE           4

#define FAIL                        1

#define VLS_CAPACITY_NONE                 0
#define VLS_CAPACITY_NON_POOLED           1
#define VLS_CAPACITY_POOLED               2

#define VLS_INFINITE_CAPACITY             0xffffffff

   /* Following is being used in lslic.
    * SLM 7.3.0
    */
#define VLS_NOLIMIT_STRING                  "NOLIMIT"
#define VLS_YES_NO_BUFFER_SIZE              2

   /* Following is being used in upgradelockcode
    * SLM 7.3.0
    */


#define ULC_CODE_VERSION_1                        1
#define ULC_CODE_VERSION_2                        2
#define ULC_CODE_VERSION_3                        3
#define ULC_CODE_VERSION                        ULC_CODE_VERSION_3
#define BASE_LOCK_CODE_LENGTH                   16
#define VENDOR_HASH_LENGTH                      7
#define VLScg_MAX_CODE_COMP_LEN                 512

   /*------------------------------------------------------------------------*/
   /* Grace Period                                                           */
   /*------------------------------------------------------------------------*/
#define VLS_NO_GRACE_PERIOD           0
#define VLS_STANDARD_GRACE_PERIOD     1
#define VLS_GRACE_REMAIN_ON_NONET    0
   /*------------------------------------------------------------------------*/
   /*  Grace license installation error handling                             */
   /*------------------------------------------------------------------------*/
#define VLS_IGNORE_GRACE_ERROR        0
#define VLS_NOTIFY_GRACE_ERROR        1
#define VLS_DISABLE_GRACE_BROADCAST   2 /* This value is used to disable broadcast in grace license request */

   /*------------------------------------------------------------------------*/
   /* Overdraft License -                                                    */
   /*------------------------------------------------------------------------*/
#define VLS_NO_OVERDRAFT        0
#define VLS_STANDARD_OVERDRAFT  1
#define VLS_UNLIMITED_OVERDRAFT_HOURS 0

   /*------------------------------------------------------------------------*/
   /* Local request locking criteria flag -                                  */
   /*------------------------------------------------------------------------*/
#define VLS_LOCAL_REQUEST_LOCKCRIT_USEDEFAULT   0
#define VLS_LOCAL_REQUEST_LOCKCRIT_DEFINED      1

   /*------------------------------------------------------------------------*/
   /* License revocation defines -                                           */
   /*------------------------------------------------------------------------*/
   /* This should always be greater than sizeof(VLSrevocationTicketT) */
#define VLS_MAX_LICENSE_REVOCATION_TICKET_SIZE         1024

   /*------------------------------------------------------------------------*/
   /* License Versions                                                       */
   /*------------------------------------------------------------------------*/
#define VLS_LICENSE_VERSION_NOT_DEFINED 0xffffffff
#define VLS_LICENSE_VERSION_TOO_OLD    0x00000000
#define VLS_LICENSE_VERSION_700        0x07000000
#define VLS_LICENSE_VERSION_730        0x07030000
#define VLS_LICENSE_VERSION_7301       0x07030001
#define VLS_LICENSE_VERSION_800        0x08000000
#define VLS_LICENSE_VERSION_810        0x08100000
#define VLS_LICENSE_VERSION_823        0x08230000
#define VLS_LICENSE_VERSION_840        0x08400000
#define VLS_LICENSE_VERSION_850        0x08500000
#define VLS_LICENSE_VERSION_860        0x08600000
#define VLS_LICENSE_VERSION_900        0x09000000
#define VLS_LICENSE_VERSION_910        0x09100000
#define VLS_LICENSE_VERSION_920        0x09200000
#define VLS_LICENSE_VERSION_941        0x09410000
#define VLS_LICENSE_VERSION_950        0x09500000
#define VLS_LICENSE_VERSION_970        0x09700000
#define VLS_LICENSE_VERSION_LATEST     VLS_LICENSE_VERSION_970

   /* This defines the maximum length of a license string */
#define VLS_MAX_LICENSE_SIZE           6400

   /* This defines the maximum length of custom encrypted license string */
#define VLS_MAX_CUSTOM_LICENSE_SIZE    (2 * VLS_MAX_LICENSE_SIZE)

   /*------------------------------------------------------------------------*/
   /* Trial Licensing                                                        */
   /*------------------------------------------------------------------------*/
#define VLS_TRIAL_DAYSCNT_DISABLED      0
#define VLS_TRIAL_ELAPSEDHOURS_DISABLED 0
#define VLS_TRIAL_EXECUTIONCNT_DISABLED 0

   /* Following macros indicate the state of trial license (returned by
      VLSgetFeatureInfo APIs) */
#define VLS_TRIAL_UNUSED     0 /* Initial state - license is available but
                                never used */
#define VLS_TRIAL_ACTIVE     1 /* License is atleast once used and is not
                                expired */
#define VLS_TRIAL_EXHAUSTED  2 /* Trial license is exhausted */

   /* Trial Precedence Values */
#define VLS_PRECEDENCE_TRIAL_OVERRIDE_NORMAL    (-1)
#define VLS_PRECEDENCE_DISABLE                  (0)
#define VLS_PRECEDENCE_DEFAULT                  (1)

   /* License hash length */
#define VLS_MAX_LICENSE_HASH_LEN        17

   /* Following macros define the store types */
#define VLS_LICENSE_STORE               0
#define VLS_TRIAL_STORE                 1
#define VLS_REVOCATION_STORE            2
#define VLS_COMMUTER_CLIENT_STORE       3
#define VLS_COMMUTER_SERVER_STORE       4
#define VLS_GRACE_STORE                 5
#define VLS_CONSUME_STORE               6
#define VLS_NETWORK_REVOCATION_STORE    7
#define VLS_TIME_TAMPER_STORE           8
#define VLS_USAGE_LOG_STORE             9
#define VLS_USAGE_GUID_STORE            10
#define VLS_USAGE_LOG_NONET_STORE       11
#define VLS_CANCEL_LEASE_STORE          12
#define VLS_ROLLOVER_COUNT_STORE        13
#define VLS_USAGE_LOG_COMMON_STORE      14


typedef enum { VLS_LIMIT_TYPE_VOLUME = 1, VLS_LIMIT_TYPE_DURATION } CONSUME_LIMIT_TYPE;
typedef enum { VLS_SET = 1, VLS_RESET } CONSUME_OPERATION_TYPE;

  /* License storage recovery mode */
#define VLS_LICENSE_STORE_NO_RECOVERY               0x00
#define VLS_LICENSE_STORE_CHECK_FILES_ON_OPEN       0x01
#define VLS_LICENSE_STORE_CHECK_RECORDS_ON_OPEN     0x02
#define VLS_LICENSE_STORE_RECOVER_ON_READ           0x04
#define VLS_LICENSE_STORE_CHECK_BACKUP_ON_READ      0x08
#define VLS_LICENSE_STORE_ALL_RECOVERY_MODES        0x0F

   /* License storage capacity macros */
#define VLS_LICENSE_STORE_MIN_CAPACITY              1
#define VLS_LICENSE_STORE_MAX_CAPACITY              0x3FFFFF

   /* Trial storage macros */
#define VLS_PERSISTENCE_STORE_MIN_CAPACITY          1
#define VLS_PERSISTENCE_STORE_MAX_CAPACITY          0x1FFFFF
#define VLS_TRIAL_DATA_MIN_UPDATETIME               1
#define VLS_TRIAL_DATA_MAX_UPDATETIME               1440



   /* License state */
#define VLS_LICENSE_STATE_INACTIVE      0 /* License is not a part of feature
                                             information. */
#define VLS_LICENSE_STATE_ACTIVE        1 /* License is a part of feature
                                             information. */


   /*------------------------------------------------------------------------*/
   /* Custom Data Information                                                */
   /*------------------------------------------------------------------------*/

#define VLS_CUSTOM_DATA_FIELD_SIZE      31 /* 8.5.0 - Max size of custom host
                                              name and custom user name */

/* To be filled by the user before passing it to VLSsetCustomData API
   for setting custom host name and user name */
typedef struct custom_data_struct
{
   unsigned long  structSz;
   char           username[VLS_CUSTOM_DATA_FIELD_SIZE + 1];
   char           hostname[VLS_CUSTOM_DATA_FIELD_SIZE + 1];
}
VLScustomData;

#define VENDOR_IDENTIFIER_LEN         25  /* Max length for vendor specific identifier */

/* Following struct is to be filled by the user in VLSenableVendorIsolation API
   for setting vendor specific identifier used for isolated server */
typedef struct vendor_isolation_struct
{
   unsigned long  structSz; /* Reserved for future use. */
   char           vendor_identifier[VENDOR_IDENTIFIER_LEN+1];
} VLSvendorIsolation;

   /*------------------------------------------------------------------------*/
   /* Identity mask flags for client reset/cleanup API                                   */
   /*------------------------------------------------------------------------*/

#define VLS_CI_HOST_NAME    0x1
#define VLS_CI_USER_NAME    0x2
#define VLS_CI_HOST_ID      0x4
#define VLS_CI_ALL          (VLS_CI_HOST_NAME | VLS_CI_USER_NAME | VLS_CI_HOST_ID)

   /*------------------------------------------------------------------------*/
   /* Challenge, ChallengeResponse structs                                   */
   /*------------------------------------------------------------------------*/

/* Maximum size of challenge data */
#define VLS_MAX_CHAL_DATA 30
/* Maximum size of response data */
#define VLS_MAX_RESP_DATA 16
   typedef struct
   {
      unsigned long   ulReserved;
      unsigned long   ulChallengedSecret;
      unsigned long   ulChallengeSize;
      unsigned char   ChallengeData[VLS_MAX_CHAL_DATA];
   }
   CHALLENGE, LS_CHALLENGE;

   typedef struct
   {
      unsigned long ulResponseSize;
      unsigned char ResponseData[VLS_MAX_RESP_DATA];
   }
   CHALLENGERESPONSE;

   typedef struct
   {
      long     wait_time;   /*max time, the client can be in queue */
      long     hold_time;   /*After allotement, the maximum time interval
                                  for which the server will keep the requested
                                  units reserved for this client */
      int      priority_num;/*Priority vis-a-vis other clients, as decided
                                   by the client application. For use in future.
                                   Not implemented in SLM7.0*/
      long     absPosition; /*The maximum position within the queue,
                                   before which the client can be queued.
                                   if the client doesn't care.*/
      long     grpPosition; /*The maximum position within the queue,
                                  considering only those queued clients that
                                  belong to the same group to which this client
                                  belongs to, before which the client can be
                                  queued -1 if the client doesn't care.*/
   }
   VLSqueuePreference;

   /*------------------------------------------------------------------------*/
   /* Client and feature information structures                              */
   /* To be used in VLSgetClientInfo, VLSgetFeatureInfo and VLShandleInfo    */
   /*------------------------------------------------------------------------*/

#define VLS_MAXFEALEN          64  /* For Rainbows Internal use only. */
#define VLS_MAXFEALEN_API_USER 25  /* For SLM API user to specifying the length of feature name. */
#define VLS_MAXLEN             VLS_MAXFEALEN
#define VLS_MAXVERLEN          12    /*11 chars, as allowed for long-codes, plus 1*/
#define VLS_SITEINFOLEN        150
#define VLS_VENINFOLEN         2000	 /* Private 8.4.0 and onwards*/
#define VLS_VENINFOLEN_810     395
#define VLS_PUBLIC_VENINFOLEN  395
#define VLS_LICENSE_VENINFOLEN  512
#define VLS_VENINFOLEN_OLD     98 /* prior to 7.3.0 version */
#define VLS_MAXCLLOCKLEN       200
#define VLS_LOGCOMMENTLEN      100
#define VLS_MAXPATHLEN_851     256
#define VLS_MAXFILEPATHLEN     256
#define VLS_MAXEIDLEN          46  /* For Internal use only. */
#define VLS_MAXAIDLEN          9  /* For  Internal use only. */
#ifdef _VMSWIN_
#define VLS_MAXPATHLEN         1024
#else
#define VLS_MAXPATHLEN         256 
#endif
#define VLS_LICENSE_STORAGE_MAXPATHLEN_851 VLS_MAXPATHLEN_851
#define VLS_LICENSE_STORAGE_MAXPATHLEN VLS_MAXPATHLEN
#define VLS_LICENSE_STORAGE_MAXPATHLEN_UNICODE 256

#define VLS_MAXSRVLOCKLEN 250 /* To store server lock informations(both primary and secondary)11(servers)x2=22*/

#define VLS_MAX_CONTEXT_LEN    255

#if !defined(VLS_NOCOMPAT)
#define MAXFEALEN    VLS_MAXFEALEN
#define MAXLEN       VLS_MAXFEALEN
#define MAXVERLEN    VLS_MAXVERLEN
#define SITEINFOLEN  VLS_SITEINFOLEN
#define VENINFOLEN   VLS_VENINFOLEN
#define MAXCLLOCKLEN VLS_MAXCLLOCKLEN
#define PUBLIC_VENINFOLEN VLS_PUBLIC_VENINFOLEN
#endif

/*Enumerators for Virtual Machine detection*/
typedef enum {
               VLS_VIRTUAL_MACHINE_ALLOWED,
               VLS_VIRTUAL_MACHINE_DISALLOWED
} VLS_VM_DETECTION;

typedef enum {
               VLS_NO_VIRTUAL_MACHINE_DETECTED,
               VLS_VIRTUAL_MACHINE_DETECTED
} VLS_VM_DETECTION_STATE;

   /*Virtual Machine Info Structure*/
   typedef struct vm_info_struct
   {
      long structSz;
      VLS_VM_DETECTION_STATE isVirtualMachine;

   }VLSVMInfo;

   /* Client Information structure */
   struct client_info_struct
   {
      char          user_name[VLS_MAXLEN];
      unsigned long host_id;
      char          group[VLS_MAXLEN];
      Time_T        start_time;
      Time_T        hold_time;
      Time_T        end_time;
      long          key_id;
      char          host_name[VLS_MAXLEN];
      char          x_display_name[VLS_MAXLEN];
      char          shared_id_name[VLS_MAXLEN];
      int           num_units;
      int           q_wait_time;
      int           is_holding;             /* VLS_TRUE/VLS_FALSE          */
      int           is_sharing;             /* # of clients using this key */
      int           is_commuted;

      /* Following fields are added to support capacity
       * licenses.
       * SLM 7.3.0
       */
      long          structSz;
      unsigned long team_capacity;      /* Total capacity */
      unsigned long total_resv_team_capacity; /* Total reserved capacity. */
      unsigned long reserved_team_capacity_in_use; /* Capacity given to clients from reserved capacity. */
      unsigned long unreserved_team_capacity_in_use; /* Capacity given to clients from unreserved capacity. */


      unsigned long user_capacity_from_reserved;
      unsigned long user_capacity_from_unreserved;

      /*Total units for this license*/
      int    total_team_tokens_resv;         /*Total reserved units*/
      int    reserved_team_tokens_in_use;      /*Units given from reserved pool to active*/
      int    unreserved_team_tokens_in_use;
      Time64_T      start_time64;					/* To support epoc time beyond year 2038 */
      Time64_T      end_time64;						/* See Time64_T definition above */
      char          client_log_comment[VLS_LOGCOMMENTLEN+1]; /* Added 860: to retrieve log comment from LM server */
	  unsigned int  num_units_v2; /*From 950 num_units_v2 can have upto 4 billion Tokens */
   };
   typedef  struct client_info_struct         VLSclientInfo;

   /* Queued Client Information Struct */
   typedef struct queued_client_info_struct
   {
      char           user_name[VLS_MAXLEN];
      char           host_name[VLS_MAXLEN];
      char           x_display_name[VLS_MAXLEN];
      char           shared_id_name[VLS_MAXLEN];
      char           group_name[VLS_MAXLEN];
      unsigned long  host_id;
      long           server_start_time;
      long           server_end_time;
      unsigned long  qkey_id;
      int            num_units;
      int            num_resvd_default;
      int            num_resvd_native;
      long           wait_time;   /*in secs*/
      long           hold_time;   /*in secs*/
      int            priority_num;
      long           absPosition; /*Current abs. position within the queue*/
      long           grpPosition; /*Current position within the queue,
                                      considering only those queued clients
                                      that belong to the same group to which
                                      this client belongs to */
      long           availabilityTime;
      long           structSz;
      Time64_T       server_start_time64;	/* To support epoc time beyond year 2038. See Time64_T definition above */
      Time64_T       server_end_time64;		/* To support epoc time beyond year 2038 */
      Time64_T       availabilityTime64;		/* To support epoc time beyond year 2038 */
   }
   VLSqueuedClientInfo;


   /* Feature Information structure */
   typedef struct  feature_info_struct
   {
      long   structSz;
      char   feature_name[VLS_MAXFEALEN];
      char   version[VLS_MAXFEALEN];
      int    lic_type;
      int    trial_days_count;
      long   birth_day;
      long   death_day;
      int    num_licenses;       /*Total units for this license*/
      int    total_resv;         /*Total reserved units*/
      int    lic_from_resv;      /*Units given from reserved pool to active
                                     clients*/
      int    qlic_from_resv;      /*Units reserved from reserved-pool by
                                     aspirants and engaged clients. */
      int    lic_from_free_pool; /*Units given from free pool to active
                                     clients*/
      int    qlic_from_free_pool;/*Units reserved from free-pool by aspirants
                                     and engaged clients.*/
      int    is_node_locked;     /*VLS_FLOATING/VLS_NODE_LOCKED/... */
      int    concurrency;
      int    sharing_crit;
      int    locking_crit;
      int    holding_crit;
      int    num_subnets;
      char   site_license_info[VLS_SITEINFOLEN];
      long   hold_time;
      int    meter_value;
      char   vendor_info[VLS_VENINFOLEN + 1 ];
      char   cl_lock_info[VLS_MAXCLLOCKLEN];
      long   key_life_time;
      int    sharing_limit;
      int    soft_num_licenses;
      int    is_standalone;      /*VLS_STANDALONE_MODE/VLS_NETWORK_MODE/VLS_PERPETUAL_MODE */
      int    check_time_tamper;
      int    is_additive;        /*VLS_TRUE/VLS_FALSE */
      int    isRedundant;
      int    majority_rule;
      int    num_servers;
      int    isCommuter;
      int    log_encrypt_level;
      int    elan_key_flag;
      long   conversion_time;
      long   avg_queue_time;     /*whether for past clients or present??? */
      long   queue_length;
      int    tot_lic_reqd;       /*By all queued Clients i.e. units required
                                     by bachlores + aspirants -units reserved
                                     by aspirants */
      int    isELMEnabled;
      int    commuted_keys;      /* number of commuted keys that have been
                                          checked out  */
      int    commuter_keys_left; /* number of commuter keys left */
      char   server_locking_info[VLS_MAXSRVLOCKLEN];    /*new field added for storing the server lock info*/
      int           capacity_flag; /* VLS_CAPACITY_NONE or VLS_CAPACITY_NON_POOLED or VLS_CAPACITY_POOLED */
      unsigned long capacity;      /* Total capacity */
      unsigned long total_resv_capacity; /* Total reserved capacity. */
      unsigned long in_use_capacity_from_reserved; /* Capacity given to clients from reserved capacity. */
      unsigned long in_use_capacity_from_unreserved; /* Capacity given to clients from unreserved capacity. */
      long  commuter_max_checkout_days;
      /*Max days license can be checked out. 0=no limit*/
      long  grace_period_flag;  /* Must be VLS_STANDARD_GRACE_PERIOD */
      long  grace_period_calendar_days;
      /* Max days license can be used in grace period  */
      long  grace_period_elapsed_hours;
      /* Max hours license can be used in grace period */
      long overdraft_flag;     /* VLS_NO_OVERDRAFT or VLS_STANDARD_OVERDRAFT */
      long overdraft_hours;    /* Max hours overdraft license can be used.       */
      long overdraft_users;    /* Simultaneous users allowed in overdraft      */
      long overdraft_users_in_use; /* Current number of users in overdraft */

      int  local_request_lockcrit_flag;
      /* VLS_LOCAL_REQUEST_LOCKCRIT_DEFINED = use the specified
         lockcrit fields below. Otherwise use defaults.
         These values are to be used by commuter license, perpetual
         licenses and grace period licenses.              */
      int  local_request_lockcrit_required;
      /* Required items for local request locking. */
      int  local_request_lockcrit_float;
      /* Floating items for local request locking. */
      int  local_request_lockcrit_min_num;
      /* Total number of items must for local request locking. */
      int  isGraceLicense;
      /* VLS_FALSE, when grace license is not in use, otherwise VLS_TRUE. */
      int  license_version;  /* License version(codegenversion) mapped to slm version. */

      /* Public Vendor Info: Appear as plain text in the license string */
      char plain_vendor_info[VLS_PUBLIC_VENINFOLEN  + 1];

      int  trial_elapsed_hours; /* Trial usage hours */
      int  trial_execution_count; /* Trial Execution limit */
      int  trial_calendar_period_left; /* Trial calendar days left */
      int  trial_elapsed_period_left;  /* Trial usage left (in seconds) */
      int  trial_executions_left;      /* Trial execution count left */
      int  trial_current_status; /*VLS_TRIAL_UNUSED - Unused means the license has
                                       never been requested and therefore the trial
                                       period has not yet begun.
                                   VLS_TRIAL_ACTIVE - License is atleast once
                                       requested,but some trial usage is still left
                                   VLS_TRIAL_EXHAUSTED - Usage limit of trial license
                                       is exhausted*/
      VLS_VM_DETECTION vm_detection; /*Request on virtual machine detection allowed/disallowed*/
      Time64_T   birth_day64;		/* To support epoc time beyond year 2038 */
      Time64_T   death_day64;		/* See Time64_T definition above */
      Time64_T   grace_period_remaining; /* Grace period left */
      Time64_T   grace_period_expiry_date;
      char eid[VLS_MAXEIDLEN];        /* Entitlement Id*/
      int pid;        /* Product Id*/
      int fid;        /* Feature Id*/
      char aid[VLS_MAXAIDLEN];        /* Authorization Id*/
      int cloud_usage_flag;        /* cloud usage on/off*/
      int lic_source;        /* license is cloud generated or not */
      Time64_T   activation_birth_time; /* for internal use only */
      Time64_T   activation_expiry_time; /* for internal use only */
      unsigned int num_licenses_v2;        /* From 950 4B Tokens suppored*/
      unsigned int total_resv_v2;             /*Total reserved units From 950 4B Tokens supported */
      unsigned int    lic_from_resv_v2;       /*From 950 4B Tokens supported */
      unsigned int lic_from_free_pool_v2;     /*From 950 4B Tokens supported*/
      unsigned int soft_num_licenses_v2;      /* From 950 4B Tokens supported*/
	  unsigned int commuted_keys_v2;          /*  From 950 4B Tokens supported*/
      unsigned int commuter_keys_left_v2;     /* From 950 4B Tokens supported*/
   }
   VLSfeatureInfo;

   /* Commuter Information Structure */
   typedef struct commuter_info_struct
   {
      int        commuter_code_version;
      int        codegen_version;
      char       feature_name[VLS_MAXFEALEN]; /* Feature name */
      char       feature_version[VLS_MAXVERLEN]; /* Feature version */
      int        birth_day;
      int        birth_month;
      int        birth_year;
      int        death_day; /* 1 - 31 */
      int        death_month; /* 1 - 12 */
      int        death_year; /* 91 - ? -- This is basically year - 1900.*/
      int        num_of_licenses;
      int        locking_crit; /* locking criteria of client */
      char       lock_info[VLS_MAXCLLOCKLEN];/*lock info of client */
      char       vendor_info[VLS_VENINFOLEN + 1];
      char       issuing_server[MAX_NAME_LEN]; /* IP addresses if the protocol_type
                                                                      is UDP. */
      long        key_life_time;
      int         protocol_type;  /* VLScc_TCP(0)  / VLScc_IPX(1) */
      int         status; /* 1 - ACTIVE
                              0 - INACTIVE */
      int         structSz;       /* Included in RMS-9.1.0 to support commuting of license to granularity of hours & minutes */
      int         birth_hours;
      int         birth_minutes;
      int         death_hours;
      int         death_minutes;
      unsigned long   num_of_licenses_v2;
   }
   VLScommuterInfo;


   /* License Information Structure */
   typedef struct  license_info_struct
   {
      long   structSz;
      char   feature_name[VLS_MAXFEALEN + 1];
      char   version[VLS_MAXFEALEN + 1];
      int    lic_type;
      int    trial_days_count;
      Time_T birth_day;
      Time_T death_day;
      int    num_licenses;       /*Total units for this license*/
      int    is_node_locked;     /*VLS_FLOATING/VLS_NODE_LOCKED/... */
      int    concurrency;
      int    sharing_crit;
      int    locking_crit;
      int    holding_crit;
      int    num_subnets;
      char   site_license_info[VLS_SITEINFOLEN + 1];
      long   hold_time;
      int    meter_value;
      char   vendor_info[VLS_VENINFOLEN + 1];
      char   cl_lock_info[VLS_MAXCLLOCKLEN + 1];
      long   key_life_time;
      int    sharing_limit;
      int    soft_num_licenses;
      int    is_standalone;      /* VLS_STANDALONE_MODE/VLS_NETWORK_MODE/
                                    VLS_PERPETUAL_MODE */
      int    check_time_tamper;
      int    is_additive;        /* VLS_TRUE/VLS_FALSE */
      int    num_servers;
      int    isRedundant;
      int    majority_rule;
      int    log_encrypt_level;
      int    elan_key_flag;
      long   conversion_time;
      char   server_locking_info[VLS_MAXSRVLOCKLEN + 1];/*new field added for
                                                          storing the server
                                                          lock info*/
      int    capacity_flag;        /* VLS_CAPACITY_NONE or
                                      VLS_CAPACITY_NON_POOLED or
                                      VLS_CAPACITY_POOLED */
      unsigned long capacity;      /* Total capacity */
      int   isCommuter;
      long  commuter_max_checkout_days;
      /*Max days license can be checked out. 0=no limit*/
      long  grace_period_flag;  /* Must be VLS_STANDARD_GRACE_PERIOD */
      long  grace_period_calendar_days;
      /* Max days license can be used in grace period  */
      long  grace_period_elapsed_hours;
      /* Max hours license can be used in grace period */
      long overdraft_flag;    /* VLS_NO_OVERDRAFT or VLS_STANDARD_OVERDRAFT */
      long overdraft_hours;   /* Max hours overdraft license can be used.  */
      long overdraft_users;   /* Simultaneous users allowed in overdraft   */

      int  local_request_lockcrit_flag;
      /* VLS_LOCAL_REQUEST_LOCKCRIT_DEFINED = use the specified
         lockcrit fields below. Otherwise use defaults.
         These values are to be used by commuter license, perpetual
         licenses and grace period licenses.              */
      int  local_request_lockcrit_required;
      /* Required items for local request locking. */
      int  local_request_lockcrit_float;
      /* Floating items for local request locking. */
      int  local_request_lockcrit_min_num;
      /* Total number of items must for local request locking. */

      int  license_version;
      /* License version(codegenversion) mapped to slm version. */

      /* Public Vendor Info: Appear as plain text in the license string */
      char plain_vendor_info[VLS_PUBLIC_VENINFOLEN + 1];

      int  trial_elapsed_hours; /* Trial usage hours */
      int  trial_execution_count; /* Trial Execution Count */
      int  trial_calendar_period_left; /* Trial days left */
      int  trial_elapsed_period_left;  /* Trial usage left (in seconds) */
      int  trial_executions_left;      /* Trial execution count left */
      int  trial_current_status; /*VLS_TRIAL_UNUSED - Unused means the license has
                                       never been requested and therefore the trial
                                       period has not yet begun.
                                   VLS_TRIAL_ACTIVE - License is atleast once
                                       requested,but some trial usage is still left
                                   VLS_TRIAL_EXHAUSTED - Usage limit of trial license
                                       is exhausted*/

      /* License string hash */
      char license_hash[VLS_MAX_LICENSE_HASH_LEN + 1];
      /* License storage location */
      char license_storage[VLS_LICENSE_STORAGE_MAXPATHLEN + 1];
      int  license_state;   /* license state: VLS_LICENSE_STATE_INACTIVE/
                                              VLS_LICENSE_STATE_ACTIVE */
      int  license_precedence; /* Precedence of license set while activating
                                  the license */
      VLS_VM_DETECTION vm_detection; /*Request on Virtual Machine Allowed or disallowed*/
#ifdef _VMSWIN_
      wchar_t license_storage_unicode[VLS_LICENSE_STORAGE_MAXPATHLEN_UNICODE + 1];
#else 
      char license_storage_unicode[VLS_LICENSE_STORAGE_MAXPATHLEN_UNICODE*2 +1];
#endif       
      Time64_T   deferred_revocation_time; /* 0 means not marked for revocation
                                          Any other positive value = time in seconds since EPOC
                                          after which revocation will happen*/
      Time64_T   birth_day64;		/* To support epoc time beyond year 2038 */
      Time64_T   death_day64;		/* See Time64_T definition above */

      char eid[VLS_MAXEIDLEN];        /* Entitlement Id*/
      int pid;        /* Product Id*/
      int fid;        /* Feature Id*/
      char aid[VLS_MAXAIDLEN];        /* Authorization Id*/
      int cloud_usage_flag;        /* cloud usage on/off*/
      int lic_source;        /* license is cloud generated or not */

      Time64_T   activation_birth_time; /* for internal use only */
      Time64_T   activation_expiry_time; /* for internal use only */
	  
      char license_vendor_info[VLS_LICENSE_VENINFOLEN + 1];  /* License Vendor Info - 
                                                             stores information in ASCII. 
                                                          1 extra byte needed for null termination */
      unsigned int num_licenses_v2;
      unsigned int soft_num_licenses_v2;

   } VLSlicenseInfo;

   /* Trial Usage Information Structure */
   typedef struct  trial_usage_info_struct
   {
      long   structSz;
      char   feature_name[VLS_MAXFEALEN + 1];
      char   version[VLS_MAXFEALEN + 1];
      int    cumulative_trial_days_count;           /* Cumulative Trial days */
      int    cumulative_trial_elapsed_hours;        /* Cumulative Trial usage hours */
      int    cumulative_trial_execution_count;      /* Cumulative Trial Execution Count */
      int    cumulative_trial_calendar_period_left; /* Cumulative Trial days left */
      int    cumulative_trial_elapsed_period_left;  /* Cumulative Trial usage left (in seconds) */
      int    cumulative_trial_executions_left;      /* Cumulative Trial execution count left */

   } VLStrialUsageInfo;


   /*------------------------------------------------------------------------*/
   /* Client version information structure                                   */
   /* To be used in VLSgetLibInfo                                            */
   /*------------------------------------------------------------------------*/

   /* VLSgetLibInfo() should return the same version string in szVersion: */
   /* The LS_VERSION format must be kept as X.YZ as this parameter is used
      in API versioning */
#define LS_VERSION   "9.7.0.0036"
#define LS_MAJOR_VERSION "9.7"
#define LIBRARY_VERSION_BUILD 36

#define PRODUCT_VERSION_MAJOR 9
#define PRODUCT_VERSION_MINOR 7
#define PRODUCT_VERSION_POINT 0
#define PRODUCT_VERSION_BUILD 36
#define PRODUCT_VERSION_STRING "9, 7, 0, 0036\0"

#define FILE_VERSION_STRING   PRODUCT_VERSION_STRING
#define FILE_VERSION_MAJOR    PRODUCT_VERSION_MAJOR
#define FILE_VERSION_MINOR    PRODUCT_VERSION_MINOR
#define FILE_VERSION_POINT    PRODUCT_VERSION_POINT
#define FILE_VERSION_BUILD    PRODUCT_VERSION_BUILD

#define LS_PROD_NAME "Sentinel RMS Development Kit"
#define LS_PROD_NAME_SRV "Sentinel RMS License Manager"

#define LS_COPYRIGHT \
"    Copyright (C) 2020 SafeNet, Inc.\n\n"

#define LIBINFOLEN  32

   typedef struct
   {
      unsigned long ulInfoCode;
      char          szVersion  [LIBINFOLEN];
      char          szProtocol [LIBINFOLEN];
      char          szPlatform [LIBINFOLEN];
      char          szLocale   [LIBINFOLEN];
      char          szUnused2  [LIBINFOLEN];
   }
   LS_LIBVERSION;

#define MAX_LINE_NUM 512

typedef struct error_Msg
{
   char     errorMsg[MAX_LINE_NUM];
   int      lineNumber;
}VLSerrorLine;
   /*------------------------------------------------------------------------*/
   /* VLSDiscover info structure to be passed as array in VLSDiscoverExt     */
   /* to get the server charateristics information                           */
   /*------------------------------------------------------------------------*/

   typedef struct
   {
      short     protocol;      /* bit 1 - IPX, bit 2 - TCP/IP */
      short     isRedundant;   /* server is token sharing type or not */
      int       num_clients;   /* Clients connected to server */
      char      ip_address[VLS_MAXLEN];
      int       num_units_available;
      int       is_served;     /* V_TRUE if already served by this  server,
                                      V_FALSE otherwise */
      char      pool_name[8];
      long      reserved1;
      long      reserved2;
   }
   discover_info_type, VLSdiscoverInfo;

   /* Introduced a new discoverInfo struct
    * in v9.5.0.
    */
   typedef struct
   {
      short     protocol;      /* bit 1 - IPX, bit 2 - TCP/IP */
      short     isRedundant;   /* server is token sharing type or not */
      int       num_clients;   /* Clients connected to server */
      char      ip_address[VLS_MAXLEN];
      int       num_units_available;
      int       is_served;     /* V_TRUE if already served by this  server,
                                      V_FALSE otherwise */
      char      pool_name[8];
      long      reserved1;
      long      reserved2;
      unsigned long units_available;
      unsigned long number_of_clients;
      long      structSz;
   }
   discover_info_type_ext, VLSdiscoverInfoExt;

   /* SLM 7.3.0
    * Prototype Code for upgrade code
    */

   typedef struct
   {
      int version_num;
      char hash_vendor_string[VENDOR_HASH_LENGTH];
      int capacity_flag;
      int standalone_flag;
      unsigned num_keys;
      int birth_day;
      int birth_month;
      int birth_year;
      int death_day;
      int death_month;
      int death_year;
      int client_server_lock_mode;
      unsigned char base_lock_code[BASE_LOCK_CODE_LENGTH + 1];

      /* For internal use */
      char base_feature_name[VLScg_MAX_CODE_COMP_LEN + 1];
      char base_feature_version[VLScg_MAX_CODE_COMP_LEN + 1];
      unsigned long capacity;
      int base_license_version;      /* Introduced in 9.2.0 for version ULC_CODE_VERSION_3 & above */
      int base_license_keyid;        /* Introduced in 9.2.0 for version ULC_CODE_VERSION_3 & above */
   }
   ulcCode;

   /*------------------------------------------------------------------------*/
   /* Macros for status codes                                                */
   /* prefix LS  :  LSAPI status codes                                       */
   /* prefix VLS :  Our own status codes                                     */
   /*------------------------------------------------------------------------*/

   /* The function completed successfully. */
#define LS_SUCCESS                   0x0

   /* Client handle refers to an invalid licensing system context. */
#define LS_BADHANDLE                 (LS_STATUS_CODE)0xC8001001

   /* Licensing system could not locate enough available licensing resources
    * to satisfy the request.
    */
#define LS_INSUFFICIENTUNITS         (LS_STATUS_CODE)0xC8001002

   /* No licensing system could be found with which to perform the function
    * invoked.
    */
#define LS_LICENSESYSNOTAVAILABLE    (LS_STATUS_CODE)0xC8001003

   /* The licensing system has determined that the resources used to satisfy
    * a previous request are no longer granted to the calling application.
    */
#define LS_LICENSETERMINATED         (LS_STATUS_CODE)0xC8001004

   /* The licensing system has no licensing resources that could satisfy the
    * request.
    */
#define LS_NOAUTHORIZATIONAVAILABLE  (LS_STATUS_CODE)0xC8001005

   /* All licensing tokens for the specified feature are already in use. */
#define LS_NOLICENSESAVAILABLE       (LS_STATUS_CODE)0xC8001006

   /* Insufficient resources (such as memory) are available to complete the
    * request.
    */
#define LS_NORESOURCES               (LS_STATUS_CODE)0xC8001007

   /* The network is unavailable. */
#define LS_NO_NETWORK                (LS_STATUS_CODE)0xC8001008

   /* A warning occured while looking up an error messge string for the
    * LSGetMessage() function.
    */
#define LS_NO_MSG_TEXT               (LS_STATUS_CODE)0xC8001009

   /* An unrecognized status code was passed into the LSGetMessage() function. */
#define LS_UNKNOWN_STATUS            (LS_STATUS_CODE)0xC800100A

   /* An invalid index has been specified. */
#define LS_BAD_INDEX                 (LS_STATUS_CODE)0xC800100B

   /* No additional units are available. */
#define LS_NO_MORE_UNITS             (LS_STATUS_CODE)0xC800100C

   /* Feature cannot run anymore because the license expiration date has
    * reached.
    */
#define LS_LICENSE_EXPIRED           (LS_STATUS_CODE)0xC800100D

   /* Input buffer is too small, need a bigger buffer. */
#define LS_BUFFER_TOO_SMALL          (LS_STATUS_CODE)0xC800100E

   /* Failed in performing the requisite operation. */
#define LS_NO_SUCCESS                (LS_STATUS_CODE)0xC800100F

   /* Grace Days have been used up. */
#define LS_GRACE_EXPIRED             (LS_STATUS_CODE)0xC8001010

   /* Unexpected state of Grace License. */
#define LS_GRACE_INVALID_STATE       (LS_STATUS_CODE)0xC8001011

   /* Grace Hours have been used up. */
#define LS_GRACE_HOURS_EXHAUSTED     (LS_STATUS_CODE)0xC8001012

   /* Licensing system could not locate enough available licensing resources
    * on the follower server.
    */
#define LS_INSUFFICIENTUNITS_ON_FOLLOWER  (LS_STATUS_CODE)0xC8001013

   /* Generic error when a license is denied by a server.
    * If reasons are known, more specific errors are given.
    */
#define VLS_NO_LICENSE_GIVEN                              1

   /* Application has not been given a name. */
#define VLS_APP_UNNAMED                                   2

   /* Failed to resolve the server host. */
#define VLS_HOST_UNKNOWN                                  3

   /* Failed to figure out the license server correctly. Set environment
    * variable LSHOST to (tilde-separated) name(s) of server(s).
    */
#define VLS_NO_SERVER_FILE                                4

   /* On the specified machine, license server is not RUNNING. */
#define VLS_NO_SERVER_RUNNING                             5

   /* Feature is not licensed to run on this machine due to server/client
    * lock-code mismatch.
    */
#define VLS_APP_NODE_LOCKED                               6

   /* Attempt to return a non-existent token by the client application. */
#define VLS_NO_KEY_TO_RETURN                              7

   /* Failed to return the token issued to this client application. */
#define VLS_RETURN_FAILED                                 8

   /* No more clients exists for this feature. */
#define VLS_NO_MORE_CLIENTS                               9

   /* No more features available on license server. */
#define VLS_NO_MORE_FEATURES                             10

   /* General error by vendor in calling function etc. */
#define VLS_CALLING_ERROR                                11

   /* Internal error in licensing or accessing feature. */
#define VLS_INTERNAL_ERROR                               12

   /* Irrecoverable Internal error in licensing or accessing feature. */
#define VLS_SEVERE_INTERNAL_ERROR                        13

   /* On the specified machine, license server is not responding.
    * (Probable cause - network down, wrong port number, some other
    * application on that port etc.)
    */
#define VLS_NO_SERVER_RESPONSE                           14

   /* This user/machine has been excluded from accessing the feature. */
#define VLS_USER_EXCLUDED                                15

   /* Unknown shared id. */
#define VLS_UNKNOWN_SHARED_ID                            16

   /* No servers responded to client broadcast. */
#define VLS_NO_RESPONSE_TO_BROADCAST                     17

   /* No such feature is available on the license server. */
#define VLS_NO_SUCH_FEATURE                              18

   /* Failed to add license. */
#define VLS_ADD_LIC_FAILED                               19

   /* Failed to delete license. */
#define VLS_DELETE_LIC_FAILED                            20

   /* Last update was done locally. */
#define VLS_LOCAL_UPDATE                                 21

   /* Last update was done by the license server. */
#define VLS_REMOTE_UPDATE                                22

   /* 1. The vendor identification of requesting application does not match with that of the application licensed by this system.
    * 2. License of different vendor is being added to an isolated server.
    */
#define VLS_VENDORIDMISMATCH                             23

   /* Feature is licensed by multiple vendors other than your vendor. */
#define VLS_MULTIPLE_VENDORID_FOUND                      24

   /* An error has occured in decrypting (or decoding) a network message.
    * Probably an incompatible or unknown server, or a version mismatch.
    */
#define VLS_BAD_SERVER_MESSAGE                           25

   /* Operation has denied due to clock tamper detection. */
#define VLS_CLK_TAMP_FOUND                               26

   /* The specified operation is not permitted - authorization failed. */
#define VLS_NOT_AUTHORIZED                               27

   /* The domain of server is different from that of client. */
#define VLS_INVALID_DOMAIN                               28

   /* The specified log filename not found on License Server. */
#define VLS_LOG_FILE_NAME_NOT_FOUND                      34

   /* Cannot change specified log filename on license server. */
#define VLS_LOG_FILE_NAME_NOT_CHANGED                    35

   /* Machine's fingerprint mismatch for feature. */
#define VLS_FINGERPRINT_MISMATCH                         36

   /* Trial license usage exhausted or trial days expired. */
#define VLS_TRIAL_LIC_EXHAUSTED                          37

   /* No Updates have taken place so far. */
#define VLS_NO_UPDATES_SO_FAR                            38

   /* Eventhough the client asked License release API to return a specific
    * number of units, but it returned all the issued units.
    */
#define VLS_ALL_UNITS_RELEASED                           39

   /* The LS_HANDLE is a queued handle */
#define VLS_QUEUED_HANDLE                                40

   /* The LS_HANDLE is an active handle */
#define VLS_ACTIVE_HANDLE                                41

   /* The status of LS_HANDLE is ambiguous. */
#define VLS_AMBIGUOUS_HANDLE                             42

   /* Could not queue the client because the queue is full. */
#define VLS_NOMORE_QUEUE_RESOURCES                       43

   /* No client as specified, found with the server. */
#define VLS_NO_SUCH_CLIENT                               44

   /* Client not authorized to make the specified request. */
#define VLS_CLIENT_NOT_AUTHORIZED                        45

   /* Processing not done because current leader is not known. */
#define VLS_LEADER_NOT_PRESENT                           47

   /* Tried to add a server to pool which is already there. */
#define VLS_SERVER_ALREADY_PRESENT                       48

   /* Tried to delete a server who is not in pool currently. */
#define VLS_SERVER_NOT_PRESENT                           49

   /* File can not be open. */
#define VLS_FILE_OPEN_ERROR                              50

   /* Host name is either not valid or can not be resolved. */
#define VLS_BAD_HOSTNAME                                 51

   /* Different API version. Client server version mismatch. */
#define VLS_DIFF_LIB_VER                                 52

   /* A non-redundant server contacted for redundant server related information. */
#define VLS_NON_REDUNDANT_SRVR                           53

   /* Message forwarded to leader. It is not an error. */
#define VLS_MSG_TO_LEADER                                54

   /* Update fail. May be contact server died or modified. */
#define VLS_CONTACT_FAILOVER_SERVER                      55

   /* IP address given can not be resolved. */
#define VLS_UNRESOLVED_IP_ADDRESS                        56

   /* Hostname given is unresoled. */
#define VLS_UNRESOLVED_HOSTNAME                          57

   /* Invalid IP address format. */
#define VLS_INVALID_IP_ADDRESS                           58

   /* Server is synchronizing dist table. Not an error. */
#define VLS_SERVER_FILE_SYNC                             59

   /* Pool is already having maximum number of servers it can handle. */
#define VLS_POOL_FULL                                    60

   /* Pool will not exist if this only server is removed. */
#define VLS_ONLY_SERVER                                  61

   /* The feature is inactive on the requested server. */
#define VLS_FEATURE_INACTIVE                             62

   /* The token cannot be issued because of majority rule failure. */
#define VLS_MAJORITY_RULE_FAILURE                        63

   /* Configuration file modifications failed. */
#define VLS_CONF_FILE_ERROR                              64

   /* A non-redundant feature contacted for redundant feature related operation. */
#define VLS_NON_REDUNDANT_FEATURE                        65

   /* Can not find trial usage information for given feature. */
#define VLS_NO_TRIAL_INFO                                66

   /* Failure in retrieving the trial usage information for the given feature. */
#define VLS_TRIAL_INFO_FAILED                            67

   /* commuter related error code */

   /* Application is not linked to integrated library. */
#define VLS_NOT_LINKED_TO_INTEGRATED_LIBRARY             69

   /* Client commuter code does not exist. */
#define VLS_CLIENT_COMMUTER_CODE_DOES_NOT_EXIST          70

   /* No more checked-out commuter code exists on the client. */
#define VLS_NO_MORE_COMMUTER_CODE                        72

   /* Failed to get client commuter information. */
#define VLS_GET_COMMUTER_INFO_FAILED                     73

   /* Unable to uninstall the client commuter license. */
#define VLS_UNABLE_TO_UNINSTALL_CLIENT_COMMUTER_CODE     74

   /* Unable to issue a commuter license to client. */
#define VLS_ISSUE_COMMUTER_CODE_FAILED                   75

   /* DEPRECATED - Please use VLS_NON_COMMUTER_LICENSE */
   /* A non-commuter license is requested for commuter related operation. */
#define VLS_UNABLE_TO_ISSUE_COMMUTER_CODE                76

   /* A non-commuter license is requested for commuter related operation. */
#define VLS_NON_COMMUTER_LICENSE                         76

   /* Not enough key available to check out commuter code */
#define VLS_NOT_ENOUGH_COMMUTER_KEYS_AVAILABLE           77

   /* Invalid commuter information from client. */
#define VLS_INVALID_INFO_FROM_CLIENT                     78

   /* Server has already check out one commuter code for this client */
#define VLS_CLIENT_ALREADY_EXIST                         79

   /* Client has already had commuter code with this feature version. */
#define VLS_COMMUTER_CODE_ALREADY_EXIST                  81

   /* Redundant server synchronization in progress. */
#define VLS_SERVER_SYNC_IN_PROGRESS                      82

   /* This commuter license is checked out remotely, so it cant be checked-in. */
#define VLS_REMOTE_CHECKOUT                              83

   /* Unable to install remote commuter code.  */
#define VLS_UNABLE_TO_INSTALL_COMMUTER_CODE              84

   /* Failed to get remote locking code string. */
#define VLS_UNABLE_TO_GET_MACHINE_ID_STRING              85

   /* Invalid remote locking code string. */
#define VLS_INVALID_MACHINEID_STR                        86

   /* Commuter code expiration is greater than license itself. */
#define  VLS_EXCEEDS_LICENSE_LIFE                        87

   /* Operating in stand-alone mode using terminal client. This is not allowed
    * by the vendor.
    */
#define VLS_TERMINAL_SERVER_FOUND                        88

   /* DEPRECATED - Please use VLS_NOT_SUPPORTED_IN_NET_ONLY_MODE */
   /* The feature is not supported in the Net-Only mode of library. */
#define VLS_NOT_APPROPRIATE_LIBRARY                      89

   /* The feature is not supported in the Net-Only mode of library. */
#define VLS_NOT_SUPPORTED_IN_NET_ONLY_MODE               89

   /* The specified file type is not supported. */
#define VLS_INVALID_FILETYPE                             90


   /* The requested operation is not supported on this license server. */
#define VLS_NOT_SUPPORTED                                91

   /* License string is invalid. */
#define VLS_INVALID_LICENSE                              92

   /* License string is duplicate and already available on the server. */
#define VLS_DUPLICATE_LICENSE                            93

   /* Insufficient user capacity available. */
#define VLS_INSUFFICIENT_USER_CAPACITY                   94

   /* Team limit exhausted. */
#define VLS_TEAM_LIMIT_EXHAUSTED                         95

   /* Insufficient team capacity available. */
#define VLS_INSUFFICIENT_TEAM_CAPACITY                   96

   /* Deletion of upgraded feature/license is not allowed. */
#define VLS_CANNOT_DELETE_UPGRADED_LIC                   97

   /* License upgrade feature is not allowed for redundant licenses, commuter
    * licenses and trial licenses.
    */
#define VLS_UPGRADE_NOT_ALLOWED                          98

   /* This feature is already marked for check-in. */
#define VLS_FEATURE_MARKED_FOR_DELETION                  99

   /* This team has been excluded from accessing the requisite feature. */
#define VLS_TEAM_EXCLUDED                               101

   /* A network server is contacted for standalone related information. */
#define VLS_NETWORK_SRVR                                102

   /* The contacted feature is a Perpetual License. */
#define VLS_PERPETUAL_LICENSE                           103

   /* The contacted feature is a Repository(earlier called Perpetual) License. */
#define VLS_REPOSITORY_LICENSE                          VLS_PERPETUAL_LICENSE

   /* A commuter token has already been checked out for this license. */
#define VLS_COMMUTER_CHECKOUT                           104

   /* Error Codes For Revoke License */

   /* License with given feature/version is either not available on the server
    * or belongs to a different vendor.
    */
#define VLS_REVOKE_ERR_NO_FEATURE                       105

   /* The message received by the server was corrupted. */
#define VLS_REVOKE_ERR_CORRUPT_MESSAGE                  106

   /* The received capacity/number of licenses to revoke out of range. */
#define VLS_REVOKE_ERR_OUT_VALID_RANGE                  107

   /* Error loading the MD5 plugin dll at the server. */
#define VLS_REVOKE_ERR_MD5_PLUGIN_LOAD_FAIL             108

   /* Error in executing the authentication plugin. */
#define VLS_REVOKE_ERR_MD5_PLUGIN_EXEC_FAIL             109

   /* This feature has less number of total licenses. */
#define VLS_REVOKE_ERR_INSUFFICIENT_FEATURE_LICENSES    110

   /* Default group does not has sufficient licenses, reconfigure your user
    * resevation file.
    */
#define VLS_REVOKE_ERR_INSUFFICIENT_DEFAULT_GROUP       111

   /* Currently required number of licenses are not free for revoke in the
    * default group.
    */
#define VLS_REVOKE_ERR_INSUFFICIENT_FREE_IN_DEFAULT     112

   /* Invalid SessionID sent by the client in packet. */
#define VLS_REVOKE_ERR_INVALID_SESSION_ID               113

   /* Invalid password for revocation. */
#define VLS_REVOKE_ERR_INVALID_PASSWORD                 114

   /* Revocation failed due to internal server error. */
#define VLS_REVOKE_ERR_INTERNAL_SERVER                  115

   /* Infinite revoke not possible with enabled group distribution. */
#define VLS_REVOKE_ERR_INFINITE_GRP_DIST                116

   /* All licenses must be free for infinite revocation. */
#define VLS_REVOKE_ERR_INFINITE_LIC_IN_USE              117

   /* License has infinite keys. Only infinite license revocation request is
    * allowed for this license.
    */
#define VLS_REVOKE_ERR_INFINITE_LIC_FINITE_REQ          118

   /* Ticket generation for revoke failed. */
#define VLS_REVOKE_ERR_TICKET_GENERATION                119

   /* Revocation feature is not supported for the specified license version. */
#define VLS_REVOKE_ERR_CODGEN_VERSION_UNSUPPORTED       120

   /* Revocation feature is not supported for redundant licenses. */
#define VLS_REVOKE_ERR_RDNT_LIC_UNSUPPORED              121

   /* Revocation feature is not supported for capacity licenses. */
#define VLS_REVOKE_ERR_CAPACITY_LIC_UNSUPPORED          122

   /* Unexpected challenge packet received from server. */
#define VLS_REVOKE_ERR_UNEXPECTED_AUTH_CHLG_PKT         123

   /* Revocation feature is not supported For trial licenses. */
#define VLS_REVOKE_ERR_TRIAL_LIC_UNSUPPORED             124

   /* Local Request Locking criteria */

   /* Not all required lock selectors are available. */
#define VLS_REQUIRED_LOCK_FIELDS_NOT_FOUND              125

   /* Total lock selectors available is less than minimum number */
#define VLS_NOT_ENOUGH_LOCK_FIELDS                      126

   /* Remote checkout is not allowed for perpetual licenses */
#define VLS_REMOTE_CHECKOUT_NOT_ALLOWED_FOR_PERPETUAL   127

   /* Remote checkout is not allowed for repository(earlier called perpetual) licenses */
#define VLS_REMOTE_CHECKOUT_NOT_ALLOWED_FOR_REPOSITORY  VLS_REMOTE_CHECKOUT_NOT_ALLOWED_FOR_PERPETUAL

   /* Installation of grace license on client machine failed. */
#define VLS_GRACE_LIC_INSTALL_FAIL                      128

   /* DEPRECATED - Please use VLS_NOT_SUPPORTED_IN_NONET_MODE    */
   /* The API is not supported in the No-Net mode of the server. */
#define VLS_NOT_SUPPORTED_IN_NONET_LIBRARY              129

   /* The feature is not supported in the No-Net mode of the server. */
#define VLS_NOT_SUPPORTED_IN_NONET_MODE                 129

   /* No active client handle exists. */
#define VLS_NO_ACTIVE_HANDLE                            130

   /* Library is not in initialized state. */
#define VLS_LIBRARY_NOT_INITIALIZED                     131

   /* Library is already in initialized state. */
#define VLS_LIBRARY_ALREADY_INITIALIZED                 132

   /* Fail to acquire API lock. API call should be re-tried on receiving this
    * error.
    */
#define VLS_RESOURCE_LOCK_FAILURE                       133

   /* No install location is set. */
#define VLS_INSTALL_STORE_NOT_SET                     134

   /* No more license store */
#define VLS_NO_MORE_LICENSE_STORES                    135

   /* No such license store */
#define VLS_NO_SUCH_LICENSE_STORE                     136

   /* License store is full */
#define VLS_LICENSE_STORE_FULL                        137

   /* Specified size of store is too small  */
#define VLS_STORE_SIZE_TOO_SMALL                      138


   /* No more licenses. */
#define VLS_NO_MORE_LICENSES                            139

   /* No license found with the specified feature/version/hash. */
#define VLS_NO_SUCH_LICENSE                             140

   /* License is in use and have active clients. */
#define VLS_LICENSE_IN_USE                              141

   /* Failure in setting the precedence for the specified trial license. */
#define VLS_SET_LICENSE_PRECEDENCE_FAILED               142

   /* Failure in accessing the license or persistence store. */
#define VLS_STORE_ACCESS_ERROR                          143

   /* Corruption in store */
#define VLS_STORE_DATA_INCONSISTENT                   144

   /* Unable to create/open the store */
#define VLS_STORE_OPEN_ERROR                          145

   /* License store query failed */
#define VLS_LICENSE_STORE_QUERY_FAILED                146

   /* Specified lock selector is not valid. */
#define VLS_LOCK_SELECTOR_INVALID                       147

  /* Currently not supported lock code. */
#define VLS_LOCK_CODE_NOT_SUPPORTED                     148

   /* Invalid lock code version. */
#define VLS_LOCK_CODE_VER_INVALID                       149

   /* Invalid lock code. */
#define VLS_LOCK_CODE_INVALID                           150

   /* No available machine id for specified lock selector. */
#define VLS_NO_AVAILABLE_MACHINE_ID                     151

   /* Code generator library initialization failed, for example at the time of
    * decoding a license.
    */
#define VLS_CODE_GENERATOR_LIBRARY_FAILED               152

   /* Some recoverable error occured in trial request/update. */
#define VLS_TRIAL_LIC_DATA_ACCESS_ERROR                 153

   /* Some un-recoverable error occured in trial update/request which required
    * manual intervention , may be persistence is corrupt or not configured.
    */
#define VLS_TRIAL_LIC_DATA_INCONSISTENT                 154

   /* Days-Based trial license is requested before the date of its first use.
    */
#define VLS_TRIAL_LIC_DATE_RESTRICTED                   155

   /* A disabled trial license is requested. */
#define VLS_TRIAL_LIC_NOT_ACTIVATED                     156

/* Failure in calling sequence of the API */
#define VLS_CALL_SEQUENCE_ERROR                       157

   /* Specified record doesn't exits in the store  */
#define VLS_RECORD_NOT_FOUND                          158

   /* No more records available in the store */
#define VLS_NO_MORE_RECORDS                           159

   /* Specify that the license doesn't get processed, in
    * the specified operation */
#define VLS_LICENSE_NOT_PROCESSED                     160

   /* Specify that the given configuartion is not allowed */
#define VLS_CONFIGURATION_NOT_ALLOWED                 161

   /* Value exceeds the maximum size for the field */
#define VLS_EXCEEDS_MAX_SIZE                          162

   /* Value specified is not within the valid range */
#define VLS_VALUE_OUT_OF_RANGE                        163

   /* Error in specifying Persistence configuration data  */
#define  VLS_PERSISTENCE_CONFIGURATION_ERROR          164

   /* When network request is made to a standalone server  */

   /* A network request is made to the standalone library. */
#define  VLS_NONET_LIBRARY                              165

   /* The store with specified name already exists  */
#define  VLS_STORE_ALREADY_EXISTS                     166

   /* Failure in specifying the backup information correctly. */
#define  VLS_BACKUP_CONFIGURATION_ERROR               167

   /*Record in license store/Trial store/Revocation store is corrupt*/
#define  VLS_RECORD_CORRUPT                           168

   /*Specified record in license store is empty*/
#define  VLS_LICENSE_RECORD_EMPTY                     169

   /*Specifies failure in writing to file in save license API*/
#define  VLS_SAVE_LICENSE_FILE_WRITE_ERROR            170


   /*Specifies file already exists in save license API*/
#define  VLS_SAVE_LICENSE_FILE_ALREADY_EXISTS           171

   /*Specifies Persistence store is full*/
#define  VLS_PERSISTENCE_STORE_FULL                     172

   /*Cleaning is not required on the current store
     as it is already in good state */
#define  VLS_CLEAN_REPAIR_NOT_REQUIRED                  173

   /*Cleaning attemped more than twice on the same
     persistence context objext. */
#define VLS_CLEAN_REPAIR_ATTEMPTED                      174

   /*Persistence store is un-recoverable when
     repairing/cleaning is tried. */
#define VLS_CLEAN_NOT_RECOVERABLE                       175

   /*Persistence store is not relevant */
#define VLS_CLEAN_WRONG_FILE                            176

   /*Unable to clean/repair */
#define VLS_CLEAN_REPAIR_FAIL                           177

   /*All data is lost in cleaning, i.e. no recovery */
#define VLS_CLEAN_REPAIR_COMPLETE_LOSS                  178

   /* Partial recovery taken place on cleaning */
#define VLS_CLEAN_REPAIR_WITH_LOSS                      179

   /* License is not client locked */
#define VLS_LICENSE_NOT_LOCKED                          180

   /* License has not expired and has valid lock code
      or is unlocked */
#define VLS_LICENSE_NOT_EXPIRED_AND_HAS_VALID_LOCK_CODE 181

   /* Mismatch between the specified lock code. */
#define VLS_LOCK_CODE_MISMATCH                          182

   /*Handler function is already registered.*/
#define VLS_HANDLER_ALREADY_REGISTERED                  183

   /* Specified license is not a trial one. */
#define VLS_NON_TRIAL_LICENSE                           184

   /*License addition cancelled by callback  */
#define VLS_ADD_LIC_CANCELLED_BY_USER                   185

   /* Neither any active handle nor any pending cache updates exists*/
#define VLS_NO_UPDATE_REQUIRED                          186

   /* This license has already been revoked.*/
#define VLS_LICENSE_ALREADY_REVOKED                     187

   /* License start date not yet reached. */
#define VLS_LICENSE_START_DATE_NOT_REACHED              188

/* Rehost related error code */

/* buffer too small */
#define VLS_REHOST_BUFFER_TOO_SMALL                     193

/* buffer too small, it should not happen */
#define VLS_REHOST_BUFFER_TOO_SMALL_UNEXPECTED          194

/* parameters error */
#define VLS_REHOST_PARAMETERS_ERROR                     195

/* algorithm not supported */
#define VLS_REHOST_UNSUPPORTED_ALGO                     196

/* invalid tlv data format */
#define VLS_REHOST_INVALID_DATA_FORMAT                  197

/* invalid rehost request data */
#define VLS_REHOST_INVALID_REQUEST_DATA                 198

/* operation type not supported */
#define VLS_REHOST_UNSUPPORTED_OPERATION_TYPE           199

/* memory allocation failure */
#define VLS_REHOST_ALLOCATE_MEMORY_FAILURE              200

/* tag can not be found in tlv */
#define VLS_REHOST_TAG_NOT_FOUND                        201

/* lock info is not matching */
#define VLS_REHOST_DIFFERENT_LOCK_INFO                  202

/* the license is used, can not be rovoked */
#define VLS_REHOST_LICENSE_IN_USE                       203

/* it should not happen, something unexpected */
#define VLS_REHOST_UNEXPECTED_ERROR                     204

/* the license is already revoked */
#define VLS_REHOST_HAVE_BEEN_REVOKED_BEFORE             205

/* revocation requested over the capacity of license */
#define VLS_REHOST_REVOKE_OVER_TOTAL                    206

/* license already exist */
#define VLS_REHOST_LICENSE_EXIST                        207

/* was canceled by callback */
#define VLS_REHOST_CANCELED_BY_USER                     208

/* rehost status is not defined */
#define VLS_REHOST_STATUS_NOT_DEFINED                   209

/*  Rehost related error codes end here */


   /* Grace code length exceeds maximum limit. */
#define VLS_GRACE_CODE_LENGTH_OVERFLOW_ERROR            210

   /* DEPRECATED - Please use VLS_ERROR_NO_MORE_FINGERPRINT_VALUE */
   /* No more fingerprint information is available. */
#define VLS_ERROR_NO_MORE_ITEMS                         211

   /* No more fingerprint information is available. */
#define VLS_ERROR_NO_MORE_FINGERPRINT_VALUE             211

   /* DEPRECATED - Please use VLS_ERROR_FINGERPRINT_VALUE_NOT_FOUND   */
   /* No fingerprint information is available on the specified index. */
#define VLS_ERROR_FILE_NOT_FOUND                        212

   /* No fingerprint information is available on the specified index. */
#define VLS_ERROR_FINGERPRINT_VALUE_NOT_FOUND           212

   /* Delete license from file is not supported for grace licenses, redundunt
    * licenses and checkout(commuter/perpetual) licenses.
    */
#define VLS_LICENSE_DELETION_NOT_ALLOWED                213

   /* The capacity value does not match, or an operation related to capacity
    * licensing is requested for a non-capacity license (or vice-versa).
    */
#define VLS_CAPACITY_MISMATCH                           214

   /* Commuter code is expired. */
#define VLS_EXPIRED_COMMUTER_CODE                       215

   /* Commuter code start date is not reached. */
#define VLS_COMMUTER_CODE_DATE_RESTRICTED               216

/* included for VTL persistence */
#define VLS_NEW_RECORD_FOUND                          217
#define VLS_NO_RECORDS_FOUND                          218
#define VLS_OPERATION_NOT_SUCCESSFUL                  219

/* Included for restricting commuter checkout to primary leader server */
#define VLS_ERROR_READING_SERVER_CONFIG_FILE             220
#define VLS_CHECKOUT_NOT_ALLOWED_FROM_NONPRIMARY_LEADER  221

/* Included for VTL and Standalone revocation support to only V11 license */
#define VLS_REHOST_LIC_VERSION_NOT_SUPPORTED          222

#define VLS_USAGE_FILE_TAMPERED                     223

#define VLS_NO_MATCH_FOUND                          224

/* TCPIP protocol version specified for client library is incorrect */
#define VLS_INVALID_TCPIP_VERSION               225

/*Error code if a server is present on a virtual machine*/
#define VLS_VIRTUAL_MACHINE_IS_DETECTED   226

/*Error code if standalone init failed at the time of grace license installation */
#define VLS_GRACE_LIC_TIME_TAMPER_INIT_FAIL 227

/* Either persistence is corrupted or doesnt exist. */
#define VLS_REVOKE_LIC_DATA_INCONSISTENT        228

/* PT size is more than supported length, opertions should be reduced from PT */
#define VLS_TOO_MANY_OPERATIONS_IN_SINGLE_PT 229

/* PT operation already executed on the server */
#define VLS_PT_ALREADY_EXECUTED_FOR_THIS_OPERATION 230

/* Different vendor ID of PT */
#define VLS_PT_VENDOR_ID_MISMATCH 231

/* Expired License can not be added or revoked*/
#define VLS_REVOKE_EXPIRED_LIC_FOUND 232

/* Commuter code checkout not allowed for this feature */
#define VLS_COMMUTER_CHECKOUT_NOT_ALLOWED  233

/* Application can not run on Remote desktop session */
#define VLS_RDP_SESSION_FOUND 234

/* Not enough key available for the duration specified to check out commuter code */
#define VLS_NOT_ENOUGH_COMMUTER_KEYS_FOR_DURATION 235

/* Old PT's (less than 853 ) are not supported in new NR */
#define VLS_REHOST_UNSUPPORTED_PT_VERSION      236

/*A deferred revocation has been performed successfully*/
#define VLS_DEFERRED_REVOCATION_SUCCESS 237

/*A deferred revocation request has already been executed for this license*/
#define VLS_LIC_ALREADY_SCHEDULED_FOR_DEFERRED_REVOKE 238

/*Commuter checkout and repository request not allowed after deferred revocation operation is performed on a feature-version */
#define VLS_OPERATION_NOT_ALLOWED_AFTER_DEFERRED_REVOCATION 239

/*Deferred revocation is not allowed in case commuter keys already issued against the feature-version
 * or repository license is already requested*/
#define VLS_DEFERRED_REVOCATION_NOT_ALLOWED 240

/* Logging backup disabled because '-x' option is specified at LM server startup */
#define VLS_USAGE_LOG_BACKUP_DISABLED       241

/* Either persistence is corrupted or doesnt exist. */
#define VLS_COMMUTER_DATA_INCONSISTENT        242

/* For vendor isolation - client not communicating with intended server */
#define VLS_NON_INTENDED_SERVER_CONTACTED   243

/* Operation is not permitted on the follower server */
#define VLS_OPERATION_ONLY_ALLOWED_ON_LEADER_SERVER   244

/* Insufficient tokens available at server to increase during license update. */
#define VLS_LICENSE_UPGRADE_DENIED   245

/* Client key info cleanup on server failed */
#define VLS_CLEAN_KEYINFO_CLIENT_FAILED               246

/*Client key info cleanup on server failed as no key exists for same*/
#define VLS_CLEAN_KEYINFO_NO_KEYS_EXISTS              247

/* The signing key index of the requested license is lower than the one sent by the client. */
#define VLS_LOWER_SIGNING_KEYINDEX_IN_LIC              248

/*Client key info cleanup on server passed excluding cleanup for commuter keys*/
#define VLS_CLEAN_KEYINFO_CLIENT_SUCC_EXCL_COMM       249

/*The feature being returned was commuted by different mechanism*/
#define VLS_INCOMPATIBLE_COMMUTER_CODE                250
/* The lease license is already cancelled. */
#define VLS_LEASE_LICENSE_CANCELLED                    252
/* Session has been terminated */
#define VLS_USER_SESSION_TERMINATED               257

   /*------------------------------------------------------------------------*/
   /* Type of file read by server at startup                                 */
   /*------------------------------------------------------------------------*/
   typedef enum {VLS_LSERVRC,VLS_LSERVRCCNF,VLS_ULSERVRC,VLS_GENERICCONF}VLS_FILE_TYPE;


   /*------------------------------------------------------------------------*/
   /* Function Prototypes                                                    */
   /*------------------------------------------------------------------------*/

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetFileName (
#ifndef LSNOPROTO
      VLS_FILE_TYPE filetype, /*IN*/
      unsigned char* fileName,/*IN*/
      unsigned char* unused1,
      unsigned long* unused2
#endif /* LSNOPROTO */
   );
   
   typedef struct
   {
      char *fileName;
#ifdef _VMSWIN_
      wchar_t *wFileName;    
#endif	 
   }VLSFileNameStruct;
   
   
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetFileNameExt (
#ifndef LSNOPROTO
      VLS_FILE_TYPE filetype, /*IN*/
	  VLSFileNameStruct *FileConfig
#endif /* LSNOPROTO */
   );



   /*------------------------------------------------------------------------*/
   /* Function Prototypes                                                    */
   /*------------------------------------------------------------------------*/
VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetConsumeLimit (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,        /*IN*/
      CONSUME_LIMIT_TYPE    consume_type,    /*IN*/
      long           LSFAR *consume_value,   /*OUT*/
      LS_CHALLENGE   LSFAR *challenge        /* Unused */
#endif /* LSNOPROTO */
   );

VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetConsumeLimit (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,        /*IN*/
      CONSUME_LIMIT_TYPE    consume_type,    /*IN*/
      CONSUME_OPERATION_TYPE    consume_op_type,  /*IN*/
      long           LSFAR *consume_value,   /*IN/OUT*/
      LS_CHALLENGE   LSFAR *challenge        /* Unused */
#endif /* LSNOPROTO */
   );


VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetContextData (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,      /*IN*/
      unsigned char  LSFAR *context_buff, /*OUT*/
      unsigned long          buff_len,     /*IN*/
      LS_CHALLENGE   LSFAR *challenge      /* Unused */
#endif /* LSNOPROTO */
   );

VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetContextData (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,      /*IN*/
      unsigned char  LSFAR *context_buff, /*IN*/
      unsigned long          buff_len,     /*IN*/
      LS_CHALLENGE   LSFAR *challenge      /* Unused */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI LSRequest (
#ifndef LSNOPROTO
      unsigned char  LSFAR *license_system,
      unsigned char  LSFAR *publisher_name,
      unsigned char  LSFAR *product_name,
      unsigned char  LSFAR *version,
      unsigned long  LSFAR *units_reqd,
      unsigned char  LSFAR *log_comment,
      LS_CHALLENGE   LSFAR *challenge,
      LS_HANDLE      LSFAR *lshandle
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI LSRelease (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,
      unsigned long         units_consumed,
      unsigned char  LSFAR *log_comment
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetAndInstallCommuterCode (
#ifndef LSNOPROTO
      unsigned char * feature_name,      /* IN */
      unsigned char * feature_version,   /* IN */
      long          * units_reqd,        /* IN */
      int           * duration,          /* IN/OUT */ /*No of days */
#if ((defined _HP_UX11_ || defined _AIX_5X_) && defined _V_LP64_)
      unsigned long *lock_mask,
#else
      int           * lock_mask,         /* IN/OUT */
#endif
      unsigned char * log_comment,       /* IN */
      LS_CHALLENGE  * challenge         /* IN/OUT */

#endif /* LSNOPROTO */
   );

   /* Added in 9.5.0 to support higher value for parameter units_reqd to 
      VLS_KEY_MAX_LIMIT. Functionality provided by this api is same as VLSgetAndInstallCommuterCode() */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetAndInstallCommuterCodeExt (
#ifndef LSNOPROTO
      unsigned char * feature_name,      /* IN */
      unsigned char * feature_version,   /* IN */
      unsigned int  * units_reqd,        /* IN */
      unsigned int  * duration,          /* IN/OUT */ /*No of days */
      unsigned int  * lock_mask,         /* IN/OUT */
      unsigned char * log_comment,       /* IN */
      LS_CHALLENGE  * challenge         /* IN/OUT */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSuninstallAndReturnCommuterCode(
#ifndef LSNOPROTO
      unsigned char              *feature_name,   /*  IN  */
      unsigned char              *feature_version,        /*  IN  */
      unsigned char              *log_comment    /*  IN  */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLScleanExpiredCommuterCode(
#ifndef LSNOPROTO
      unsigned char              *feature_name,    /*  IN  */
      unsigned char              *feature_version, /*  IN  */
      unsigned char              *log_comment,     /*  IN  */
      unsigned long              *unused           /*  IN  */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetCommuterInfo(
#ifndef LSNOPROTO
      unsigned char *feature_name,     /* IN */
      unsigned char *version,          /* IN */
      int index,                       /* IN */
      VLScommuterInfo *commuter_info  /* OUT */
#endif /* LSNOPROTO */
   );

/* Below API is extended version of VLSgetCommuterInfo(). Older API VLSgetCommuterInfo() will
   be deprecated in future so avoid using it for RMS 9.1.0 onwards */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetCommuterInfoExt(
#ifndef LSNOPROTO
      unsigned char *feature_name,     /* IN */
      unsigned char *version,          /* IN */
      int index,                       /* IN */
      VLScommuterInfo *commuter_info  /* OUT; preallocated and 'structSz' must be set to sizeof(VLScommuterInfo) */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI LSUpdate (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,                  /* IN */
      unsigned long         ulGraceSwitchToNetworkTm,  /* IN */
      long           LSFAR *new_units_reqd,            /* IN/OUT - new units required */
      unsigned char  LSFAR *log_comment,               /* IN */
      LS_CHALLENGE   LSFAR *challenge                  /* IN/OUT */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSbatchUpdate (
#ifndef LSNOPROTO
      int                    numHandles,     /* IN                          */
      LS_HANDLE      LSFAR * lshandle,       /* INOUT - numHandles elements */
      unsigned long  LSFAR * unused1,        /* IN    - should be NULL      */
      long           LSFAR * unused2,        /* IN    - should be NULL      */
      unsigned char  LSFAR * log_comment,    
      LS_CHALLENGE   LSFAR * unused4,        /* IN    - should be NULL      */
      LS_STATUS_CODE LSFAR * status          /* OUT   - numHandles elements */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSchangeUsageLogFileName (
#ifndef LSNOPROTO
      char  LSFAR * hostName,       /* IN   */
      char  LSFAR * newFileName    /* IN    */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLScreateUsageLogBackupFile (
#ifndef LSNOPROTO
      char  LSFAR * hostName,       /* IN   */
      int   * backUpCreationStatus  /* OUT */
#endif
   );
   
/*Enumerators for VLScreateUsageLogBackupFile
  prefix ULB_ means UsageLogBackup */
   typedef enum {
        ULB_INITIATED = 1,
        ULB_IN_PROGRESS,
        ULB_COMPLETED,
        ULB_RECENTLY_CREATED,
        ULB_ERROR
   }VLSbackupCreationStatus;

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetUsageLogFileName (
#ifndef LSNOPROTO
      char    LSFAR   *hostName,            /* IN   */
      char    LSFAR   *fileName             /* OUT  */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSshutDown (
#ifndef LSNOPROTO
      char    LSFAR   *hostName
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI LSGetMessage (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,
      LS_STATUS_CODE        Value,
      unsigned char  LSFAR *Buffer,
      unsigned long         BufferSize
#endif
   );

   /* Single-call licensing. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSlicense(
#ifndef LSNOPROTO
      unsigned char  LSFAR *feature_name,
      unsigned char  LSFAR *version,
      LS_HANDLE      LSFAR *handle
#endif  /* LSNOPROTO */
   );

   /* Disables single-call licensing; returns license key. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdisableLicense(
#ifndef LSNOPROTO
      LS_HANDLE      LSFAR *handle
#endif  /* LSNOPROTO */
   );

   /*------------------------------------------------------------------------*/
   /* Disables automatic renewal of license                                  */
   /* call with handle to disable automatic renewal of one feature           */
   /* call with (LS_HANDLE) 0 to disable auto renewal of all features        */
   /* on UNIX, call VLSdisableAutoTimer before using sleep                   */
   /* on Win32, call VLSdisableAutoTimer when thread has no message loop     */
   /*------------------------------------------------------------------------*/
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdisableAutoTimer(
#ifndef LSNOPROTO
      LS_HANDLE handle,
      int       state        /* VLS_ON or VLS_OFF */
#endif  /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetTraceLevel(
#ifndef LSNOPROTO
      int trace_level
#endif /* LSNOPROTO*/
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetContactServer(
#ifndef LSNOPROTO
      char LSFAR *server_name
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetServerList (
#ifndef LSNOPROTO
      char LSFAR *outBuf,
      int        outBufSz
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSinitServerList(
#ifndef LSNOPROTO

      char LSFAR *ServerList,
      int        option_flag
#endif /* LSNOPROTO */
   );

   /* Get the name of license server. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetContactServer(
#ifndef LSNOPROTO
      char   LSFAR     *outBuf,
      int    outBufSz
#endif
   );

   /* Get the name of license server from Handle. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetServerNameFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE        handle_id,
      char   LSFAR     *outBuf,
      int    outBufSz
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSerrorHandle (
#ifndef LSNOPROTO
      int errorHandle
#endif
   );

   /*
    * Replaces the default error handler for the specified error.
    * Error Handlers are automatically called on error, unless disabled.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetErrorHandler(
#ifndef LSNOPROTO
      LS_STATUS_CODE (VMSWINAPI * myErrorHandler)(LS_STATUS_CODE, char LSFAR *),
      LS_STATUS_CODE LS_ErrorType
#endif /* LSNOPROTO */
   );

   /*
    * Configures displaying of error msgs to the user through the default
    * error handlers.  If you disable the default error handlers you do not
    * need to use this function.
    * Default behavior:
    *   Windows - Pop up a Message Box.
    *   Unix    - Write to stderr.
    * You can alter this behavior by providing either a FILE* or a file path.
    * The other parameter should be NULL.
    * If you provide both, preference will be given to the FILE*.
    */
   typedef enum {
      VLS_NULL, VLS_STDOUT, VLS_STDERR
   } VLS_ERR_FILE;

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetUserErrorFile(
#ifndef LSNOPROTO
      VLS_ERR_FILE msgFile,  /* IN - Desired error file */
      char LSFAR * filePath  /* IN - Full path of desired error file */
#endif /* LSNOPROTO */
   );

   /* API to register custom handler for trace messages */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetTraceHandler(
#ifndef LSNOPROTO
      LS_STATUS_CODE (*myTraceHandler)(int, char LSFAR *,int)
#endif /* LSNOPROTO */
   );

   /* API to set trace messages log file */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetUserTraceFile(
#ifndef LSNOPROTO
      VLS_ERR_FILE msgFile, /* IN - Desired error file */
      char LSFAR * filePath /* IN - Full path of desired error file */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetRemoteRenewalTime(
#ifndef LSNOPROTO
      unsigned char LSFAR *feature_name,
      unsigned char LSFAR *version,
      int renewal_time     /* renewal time in secs */
#endif /* LSNOPROTO */
   );

   /* Extension to VLSdiscoverExt is added in v9.5.0.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdiscoverExt2 (
#ifndef LSNOPROTO
         unsigned char   LSFAR *feature_name,           /* IN */
         unsigned char   LSFAR *version,                /* IN */
         unsigned char   LSFAR *unused1,                /* IN */
         int                   *num_servers,            /* IN-OUT */
         VLSdiscoverInfoExt    *discoverInfo,          /* IN-OUT*/
         int                   optionFlag,              /* IN */
         int                   sharing_crit,            /* IN */
         char            LSFAR *vendor_list
#endif /* LSNOPROTO */
      );

   /* The following API is an extension of VLSDiscover, It now returns
    * Server Characteristics Information in discoverInfo Array
    *
    * NOTE: num_servers is an IN-OUT parameter.
    * As INPUT it passes the array size of discoverInfo
    * and as OUT it tells how many servers responded
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdiscoverExt (
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,           /* IN */
      unsigned char   LSFAR *version,                /* IN */
      unsigned char   LSFAR *unused1,                /* IN */
      int                   *num_servers,            /* IN-OUT */
      VLSdiscoverInfo       *discoverInfo,          /* IN-OUT*/
      int                   optionFlag,              /* IN */
      int                   sharing_crit,            /* IN */
      char            LSFAR *vendor_list
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdiscover (
#ifndef LSNOPROTO
      unsigned char LSFAR *feature_name,
      unsigned char LSFAR *version,
      unsigned char LSFAR *unused1,
      int                  bufferSize,
      char          LSFAR *server_names,
      int                  broadcastFlag,
      char          LSFAR *vendor_list
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSaddFeature (
#ifndef LSNOPROTO
      unsigned char LSFAR *license_string,
      unsigned char LSFAR *unused1,
      LS_CHALLENGE  LSFAR *unused2
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSaddFeatureToFile (
#ifndef LSNOPROTO
      unsigned char LSFAR *license_string,
      unsigned char LSFAR *unused1,
      unsigned char LSFAR *unused2,
      LS_CHALLENGE  LSFAR *unused3
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdeleteFeature (
#ifndef LSNOPROTO
      unsigned char LSFAR *feature_name,
      unsigned char LSFAR *version,
      unsigned char LSFAR *unused1,
      LS_CHALLENGE  LSFAR *unused2
#endif
   );

   /* Capacity licensing.
    * SLM 7.3.0
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdeleteFeatureExt (
#ifndef LSNOPROTO
      unsigned char LSFAR *feature_name,
      unsigned char LSFAR *version,
      unsigned long LSFAR *capacity,
      unsigned char LSFAR *log_comment,
      LS_CHALLENGE  LSFAR *challenge,
      unsigned char LSFAR *unused1,
      unsigned long LSFAR *unused2
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetVersions (
#ifndef LSNOPROTO
      char          LSFAR *feature_name,
      int                  bufferSize,
      char          LSFAR *versionList,
      char          LSFAR *unused1
#endif
   );

   /* Capacity licensing.
    * SLM 7.3.0
    */
   /**************************************************************************
   * DESCRIPTION:
   *           This function will get capacities of all the licenses having
   *           specified feature & version but different capacity.
   *           It returns list of capacities as one string, each capacity
   *           separated by a space character.
   *           If capacityList is passed as NULL, the API returns the
   *           bufferSize required. The index field returns the index of the
   *           license upto which the capacity has been retrieved based on the
   *           buffersize.
   ************************************************************************/
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetCapacityList (
#ifndef LSNOPROTO
      unsigned char LSFAR *feature_name,  /*IN*/
      unsigned char LSFAR *feature_version, /*IN*/
      int           LSFAR *index,          /*INOUT*/
      unsigned long LSFAR *bufferSize,    /*INOUT*/
      char          LSFAR *capacityList,  /*OUT*/
      char          LSFAR *log_comment, /* currently unused */
      unsigned long LSFAR *unused2      /* currently unused */
#endif
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetHandleInfo (
#ifndef LSNOPROTO
      LS_HANDLE            lshandle,
      VLSclientInfo LSFAR *client_info
#endif
   );

   /* Get information about client */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetClientInfo (
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,
      unsigned char   LSFAR *version,
      int                    index,
      char            LSFAR *unused1,
      VLSclientInfo   LSFAR *client_info
#endif /* LSNOPROTO */
   );

   /* Extended the functionality of VLSgetClientInfo API to support
    * capacity licenses also.
    * Added in SLM 7.3.0
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetClientInfoExt (
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,
      unsigned char   LSFAR *version,
      unsigned long   LSFAR *capacity,
      int                    index,
      char            LSFAR *log_comment,
      unsigned long   LSFAR *unused1,
      VLSclientInfo   LSFAR *client_info
#endif /* LSNOPROTO */
   );

   /* Extended the functionality of VLSgetClientInfoExt API to support
    * 64 bit epoc time data members in VLSclientInfo. Added in SLM 8.5.5 
	* Important: must set the 'structSz' member of struct being passed (i.e. VLSclientInfo)
	* to the 'sizeof (VLSclientInfo)' before calling this API otherwise 
	* an error "VLS_CALLING_ERROR" will be returned */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetClientInfoExt2 (
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,
      unsigned char   LSFAR *version,
      unsigned long   LSFAR *capacity,
      int                    index,
      char            LSFAR *log_comment,
      unsigned long   LSFAR *unused1,
      VLSclientInfo   LSFAR *client_info
#endif /* LSNOPROTO */
   );

   /* Get information about queued client. Imp : This will be obsolete in future better use 'VLSgetQueuedClientInfoExt()' */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetQueuedClientInfo (
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,
      unsigned char   LSFAR *version,
      int                    index,
      char            LSFAR *unused1,
      VLSqueuedClientInfo   LSFAR *client_info
#endif /* LSNOPROTO */
   );

   /* This API is the new version of 'VLSgetQueuedClientInfo()' since 8.5.5
    * Important: must set the 'structSz' member of struct being passed (i.e. VLSqueuedClientInfo)
	* to the 'sizeof (VLSqueuedClientInfo)' before calling this API otherwise 
	* an error "VLS_CALLING_ERROR" will be returned */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetQueuedClientInfoExt(
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,
      unsigned char   LSFAR *version,
      int                    index,
      char            LSFAR *unused1,
      VLSqueuedClientInfo   LSFAR *client_info
#endif /* LSNOPROTO */
   );

   /* Get information about feature */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetFeatureInfo(
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,
      unsigned char   LSFAR *version,
      int                    index,
      char            LSFAR *unused1,
      VLSfeatureInfo  LSFAR *feature_info
#endif /* LSNOPROTO */
   );

   /* Extended the functionality of VLSgetFeatureInfo API to support
    * capacity licenses also.
    * Added in SLM 7.3.0
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetFeatureInfoExt (
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name,
      unsigned char   LSFAR *version,
      unsigned long   LSFAR *capacity,
      int                    index,
      char            LSFAR *unused1,
      unsigned long   LSFAR *unused2,
      VLSfeatureInfo  LSFAR *feature_info
#endif /* LSNOPROTO */
   );

   /* API to calculate License Hash, Added in 8.4.0 */

   VDLL32 LS_STATUS_CODE VMSWINAPI VLScalculateLicenseHash (
#ifndef LSNOPROTO
	  char            LSFAR *pcLicenseString,
      unsigned char   LSFAR *pucLicenseHash,
      int             LSFAR *piLicenseHashLength
#endif /* LSNOPROTO */
	  );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdestroyHandleInClient (
#ifndef LSNOPROTO
   LS_HANDLE lshandle
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetFeatureFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE              lshandle,
      char            LSFAR *Buffer,
      unsigned long          BufferSize
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetVersionFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE              lshandle,
      char            LSFAR *Buffer,
      unsigned long          BufferSize
#endif
   );

   /* Extracts allocated team capacity and user capacity
    * from the handle.
    * SLM 7.3.0
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetCapacityFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE       lshandle,
      unsigned long LSFAR *user_capacity,   /* OUT - user capacity issued by by server */
      unsigned long LSFAR *team_capacity,    /* OUT - team capacity issued by by server */
      unsigned long LSFAR *license_capacity  /* OUT - license capacity */
#endif
   );
   /*
    * Note that the information returned by this function will be correct
    * only immediately after acquiring the handle.  The information in the
    * handle is NOT updated subsequently.
    *
    * The function is used when the clocks may not be in sync. It
    * returns the difference in seconds between the estimated current
    * time on the server and the estimated time on the client.
    * The estimation error is usually the network latency time.
    *
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetTimeDriftFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE            lshandle,                    /* IN  */
      long          LSFAR *secondsServerAheadOfClient   /* OUT */
#endif
   );


   /*
    * Note that the information returned by this function will be correct
    * only immediately after acquiring the handle.  The information in the
    * handle is NOT updated subsequently.
    *
    * The function is used when the clocks may not be in sync. It
    * returns the difference in seconds between the estimated current
    * time on the server and the estimated feature expiration time
    * on the server.
    *
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetFeatureTimeLeftFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE              lshandle,                       /* IN */
      unsigned long   LSFAR *secondsUntilTheFeatureExpires   /* OUT */
#endif
   );

   /*
    * Note that the information returned by this function will be correct
    * only immediately after acquiring the handle.  The information in the
    * handle is NOT updated subsequently.
    *
    * The function is used when the clocks may not be in sync. It
    * returns the difference in seconds between the estimated current
    * time on the server and the estimated key expiration time on
    * on the server.
    *
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetKeyTimeLeftFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE              lshandle,                   /* IN */
      unsigned long   LSFAR *secondsUntilTheKeyExpires   /* OUT */
#endif
   );

   /*
    * Note that the information returned by this function will be correct
    * only immediately after acquiring the handle.  The information in the
    * handle is NOT updated subsequently.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLicInUseFromHandle (
#ifndef LSNOPROTO
      LS_HANDLE              lshandle,
      int             LSFAR *totalKeysIssued   /* OUT - keys issued by server */
#endif
   );

   /*
    * Extended version of VLSgetLicInUseFromHandle() api because from 9.5.0, a high value
    * of tokens is supported () for hard-limit & soft-limit. So this extended api accepts
    * unsigned int to support larger values.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLicInUseFromHandleExt (
#ifndef LSNOPROTO
      LS_HANDLE              lshandle,
      unsigned int    LSFAR *totalKeysIssued   /* OUT - keys issued by server */
#endif
   );

   /*
    * Returns the list of currently active client handles in the
    * outHandleBuf parameter.
    * The caller will: preallocate outHandleBuf memory
    *                  pass the number of handle for which memory
    *                      is pre-allocated as numHandle parameter.
    * In case NULL is passed as outHandleBuf, then number
    * of currently active handles will be returned by the API.
    */

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetActiveHandleList(
      LS_HANDLE       *outHandleBuf, /*OUT*/
      unsigned long   *numHandle     /*IN/OUT*/
   );


   /*
    * Returns the value VLS_LOCAL_UPDATE or VLS_REMOTE_UPDATE
    * depending on whether the last SUCCESSFUL update was locally done or
    * done by the Sentinel RMS Development Kit server.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetRenewalStatus (
#ifndef LSNOPROTO
      void
#endif
   );

   /*
    * Calling this function makes all future update calls
    * go directly to the Sentinel RMS Development Kit server.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdisableLocalRenewal(
#ifndef LSNOPROTO
      void
#endif
   );

   /*
    * Calling this function allows the client libraries to process each
    * future update and send only those updates which are necessary
    * to the server. This is the default behaviour and please read the
    * user manual for further description on the default behaviour.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSenableLocalRenewal(
#ifndef LSNOPROTO
      void
#endif
   );

   /*
    * This function tells us whether local renewal of keys is enabled,
    * or if all LSUpdate calls go straight to the server (disabled).
    */
   VDLL32 VLS_LOC_UPD_STAT VMSWINAPI VLSisLocalRenewalDisabled(
#ifndef LSNOPROTO
      void
#endif
   );

   /* Call this function to get a description of the client library version */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLibInfo(
#ifndef LSNOPROTO
      LS_LIBVERSION LSFAR * pInfo
#endif
   );

   /* This API is used to generate output report file for Output_file_name parameter
   *from input log file file_name. This API can generate out according to option pass by
   *Option_field. If doesn't want to pass option Option_field is passed as NULL or "".
   *API returns error if invalid value is passed to Option_field.
 */

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSusageFileDecrypt(
      unsigned char *file_name/*IN*/,
      unsigned char *Option_field/*IN*/,
      unsigned char *Output_file_name /*IN*/
      );

   /*This API is used to find if usage file is tampered or not. If usage file is not tampered
    *then API shall return LS_SUCCESS and "len" parameter shall be 0. If usage file is tampered
    *then API returns proper error code and fills the "error_line" parameter (memory needs to be 
    *allocated by the caller) with tampered file information.
   */


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSusageAuthenticate (
                                      unsigned char *file_name, /*IN*/
                                      int *len,/*IN/OUT*/
                                      VLSerrorLine *error_line /*OUT*/);

   VDLL32 int VMSWINAPI VLSgetServerPort(
#ifndef LSNOPROTO
      void
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSinitialize(
#ifndef LSNOPROTO
      void
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI  VLScleanup(
#ifndef LSNOPROTO
      void
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetTimeoutInterval (
#ifndef LSNOPROTO
      long interval
#endif
   );

   VDLL32 long VMSWINAPI VLSgetTimeoutInterval(
#ifndef LSNOPROTO
      void
#endif
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetBroadcastInterval (
#ifndef LSNOPROTO
      long interval
#endif
   );

   VDLL32 long VMSWINAPI VLSgetBroadcastInterval(
#ifndef LSNOPROTO
      void
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSqueuedRequest (
#ifndef LSNOPROTO
      unsigned char  LSFAR *license_system,
      unsigned char  LSFAR *publisher_name,
      unsigned char  LSFAR *product_name,
      unsigned char  LSFAR *version,
      unsigned long  LSFAR *units_reqd,
      unsigned char  LSFAR *log_comment,
      LS_CHALLENGE   LSFAR *challenge,
      LS_HANDLE      LSFAR *lshandle,
      VLSqueuePreference LSFAR *qPreference,
      int            LSFAR *requestFlag
#endif /* LSNOPROTO */
   );

   /* Important: Use the new version of this API instead because this old API will be obsolete in future */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSupdateQueuedClient (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,
      Time_T         LSFAR *absExpiryTime,
      unsigned char  LSFAR *unused3,
      LS_CHALLENGE   LSFAR *unused4
#endif /* LSNOPROTO */
   );

   /* New version of API VLSupdateQueuedClient().
    * Important: Use this API instead of VLSupdateQueuedClient() because this will be obsolete in future */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSupdateQueuedClientNew (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,
      Time64_T       LSFAR *absExpiryTime,		/* See Time64_T definition above */
      unsigned char  LSFAR *unused3,
      LS_CHALLENGE   LSFAR *unused4
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetQueuedLicense (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,
      unsigned char  LSFAR *log_comment,
      LS_CHALLENGE   LSFAR *challenge
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetHandleStatus (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSinitQueuePreference (
#ifndef LSNOPROTO
      VLSqueuePreference  *qPreference
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSremoveQueuedClient (
#ifndef LSNOPROTO
      unsigned char         *feature_name,  /* IN */
      unsigned char         *version,       /* IN */
      long                  qkey_id,        /*IN */
      char                  *log_comment    /* IN */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSremoveQueue (
#ifndef LSNOPROTO
      unsigned char         *feature_name,  /* IN */
      unsigned char         *version,       /* IN */
      char                  *log_comment    /* IN */
#endif /* LSNOPROTO */
   );

   /*------------------------------------------------------------------------*/
   /* Fingerprinting-related types and functions:                            */
   /*------------------------------------------------------------------------*/

   #define  VLS_CUSTOMEX_SIZE          64

   /* customEx lock struct */
   typedef struct _vlscustomEx
   {
      unsigned char     customEx[VLS_CUSTOMEX_SIZE];       /* customEx locking criteria */
      int               len;                         /* The size of custom locking criterion */
   }
   VLScustomEx;

   typedef struct _vlsmachineID
   {
      unsigned long id_prom;               /* VLS_LOCK_ID_PROM */
      char          ip_addr[VLS_MAXLEN];       /* VLS_LOCK_IP_ADDR */
      unsigned long disk_id;               /* VLS_LOCK_DISK_ID */
      char          host_name[VLS_MAXLEN];     /* VLS_LOCK_HOSTNAME */
      char          ethernet[VLS_MAXLEN];      /* VLS_LOCK_ETHERNET */
      unsigned long nw_ipx;                        /* VLS_LOCK_NW_IPX *//*unused*/
      unsigned long nw_serial;             /* VLS_LOCK_NW_SERIAL */
      char          portserv_addr[VLS_MAXLEN]; /* VLS_LOCK_PORTABLE_SERV */
      unsigned long custom;                /* VLS_LOCK_CUSTOM */

      unsigned long reserved;              /* For internal use */
      char          cpu_id[VLS_MAX_CPU_ID_LEN + 1];   /* VLS_LOCK_CPU */
      VLScustomEx   customEx;              /* VLS_LOCK_CUSTOMEX */
      char          hard_disk_serial[VLS_MAXLEN];     /*VLS_LOCK_HARD_DISK_SERIAL*/
      char          cpu_info[VLS_MAX_CPU_INFO_LEN + 1];
      char          uuid[VLS_MAX_UUID_LEN + 1];
      unsigned long unused2;               /* Reserved for future use. */
   }
   VLSmachineID;

   typedef struct _vlshashedmachineID
   {
      char            id_prom_hash[VLS_MAX_LICENSE_HASH_LEN];            /* VLS_LOCK_ID_PROM */
      char            ip_addr_hash[VLS_MAX_LICENSE_HASH_LEN];            /* VLS_LOCK_IP_ADDR */
      char            disk_id_hash[VLS_MAX_LICENSE_HASH_LEN];            /* VLS_LOCK_DISK_ID */
      char            host_name_hash[VLS_MAX_LICENSE_HASH_LEN];          /* VLS_LOCK_HOSTNAME */
      char            ethernet_hash[VLS_MAX_LICENSE_HASH_LEN];           /* VLS_LOCK_ETHERNET */
      char            nw_ipx_hash[VLS_MAX_LICENSE_HASH_LEN];             /* VLS_LOCK_NW_IPX *//*unused*/
      char            nw_serial_hash[VLS_MAX_LICENSE_HASH_LEN];          /* VLS_LOCK_NW_SERIAL */
      char            portserv_addr_hash[VLS_MAX_LICENSE_HASH_LEN];      /* VLS_LOCK_PORTABLE_SERV */
      char            custom_hash[VLS_MAX_LICENSE_HASH_LEN];             /* VLS_LOCK_CUSTOM */
      char            cpu_id_hash[VLS_MAX_LICENSE_HASH_LEN];             /* VLS_LOCK_CPU */
      char            customEx_hash[VLS_MAX_LICENSE_HASH_LEN];           /* VLS_LOCK_CUSTOMEX */
      char            hard_disk_serial_hash[VLS_MAX_LICENSE_HASH_LEN];   /* VLS_LOCK_HARD_DISK_SERIAL*/
      char            cpu_info_hash[VLS_MAX_LICENSE_HASH_LEN];           /* VLS_LOCK_CPU_INFO */
      char            uuid_hash[VLS_MAX_LICENSE_HASH_LEN];               /* VLS_LOCK_UUID */
   }
   VLShashedMachineID;

   /* Initializes a machine id struct to blank/default values. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSinitMachineID(
#ifndef LSNOPROTO
      VLSmachineID LSFAR *machineID       /* OUT - should be pre-allocated */
#endif
   );

   /*
    * Sets the values of the machine id struct for the current host.
    * The input machine ID struct is initialized and then only those items
    * indicated by the lock_selector_in will (try to) be obtained and set.
    * If lock_selector_out is not NULL, *lock_selector_out is set to a bitmask
    * specifying which items could actually be obtained.
    * To try to obtain all possible machine id struct items, set
    * lock_selector_in to VLS_LOCK_ALL.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetMachineID(
#ifndef LSNOPROTO
      unsigned long        lock_selector_in, /* IN */
      VLSmachineID  LSFAR *machineID,        /* OUT - should be pre-allocated */
      unsigned long LSFAR *lock_selector_out /* OUT - may be NULL */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetMachineIDOld(
#ifndef LSNOPROTO
      unsigned long        lock_selector_in, /* IN */
      VLSmachineID  LSFAR *machineID,        /* OUT - should be pre-allocated */
      unsigned long LSFAR *lock_selector_out /* OUT - may be NULL */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetNumberedMachineID(
#ifndef LSNOPROTO
      unsigned long        lock_selector_in,  /* IN */
      VLSmachineID  LSFAR *machineID,         /* OUT - should be pre-allocated */
      unsigned long LSFAR *lock_selector_out, /* OUT - may be NULL */
      int                  flag,              /* IN  - VLS_GET_CID/VLS_GET_ETHERNET/
                                          VLS_GET_CUSTOMEX/VLS_GET_HARD_DISK_SERIAL */
      int                  index,             /* IN */
      int                  reserved           /* IN */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetNumberedMachineIDExt(
#ifndef LSNOPROTO
      unsigned char LSFAR *server_name,       /* IN */
      unsigned long        lock_selector_in,  /* IN */
      VLSmachineID  LSFAR *machineID,         /* OUT - should be pre-allocated */
      unsigned long LSFAR *lock_selector_out, /* OUT - may be NULL */
      int                  flag,              /* IN  - VLS_GET_CID/VLS_GET_ETHERNET/
                                          VLS_GET_CUSTOMEX/VLS_GET_HARD_DISK_SERIAL */
      int                  index,             /* IN */
      int                  reserved           /* IN */
#endif
   );

   /* Computes locking code of machineID struct based on lock selector. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSmachineIDtoLockCode(
#ifndef LSNOPROTO
      VLSmachineID  LSFAR *machineID,     /* IN */
      unsigned long        lock_selector, /* IN */
      unsigned long LSFAR *lockCode       /* OUT - effective locking code */
#endif
   );

   /* Computes locking code of hashedMachineID struct based on lock selector. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLShashedMachineIDToLockCode(
#ifndef LSNOPROTO
      VLShashedMachineID  LSFAR *hashedMachineID,     /* IN */
      unsigned long        lock_selector, /* IN */
      char*         lockCode,             /* OUT - new mechanism locking code */
      int           lockCodeLen,          /* IN */
      int           unused
#endif
   );

   /* This funciton is added to support new locking code generated using new mechsanism from SLM8.1*/
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSmachineIDToLockCodeEx(
#ifndef LSNOPROTO
      VLSmachineID  LSFAR *machineID,     /* IN */
      unsigned long lock_selector,        /* IN */
      char*         lockCode,             /* OUT - new mechanism locking code */
      int           lockCodeLen,          /* IN */
      int           unused
#endif
   );

   /*------------------------------------------------------------------------*/
   /* Function Prototypes of General-Purpose Utility Functions:              */
   /*------------------------------------------------------------------------*/

   /*
    * This function is called for scheduling eventhandler to be awakened after
    * so many seconds. It handles only SIGALRM signal. No. of events that can be
    * scheduled is 100. A particular eventhandler can be executed more
    * than once by specifying it in repeat_event argument. This function is
    * available only on UNIX platforms.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSscheduleEvent(
#ifndef LSNOPROTO
      unsigned long   seconds,              /* IN -- Time Interval in seconds */
      void            (*eventHandler) (void),   /* IN -- Signal Handler Fn. */
      long            repeat_event          /* IN -- No of event repetitions :
                                                     -1 for infinite */
#endif
   );

   /*
    * This function is called for disabling the events scheduled using
    * VLSscheduleEvent function. To disable a particular event pass the event
    * handler function name as the argument. To disable all the events pass
    * NULL as argument. Returns LS_SUCCESS on success. This function is available
    * only on UNIX platforms.
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdisableEvents(
#ifndef LSNOPROTO
      void        (*eventHandler) (void)  /* IN -- Signal Handler Fn.: NULL for All */
#endif
   );

   /*------------------------------------------------------------------------*/
   /* Function Prototypes of Redundant server related client APIs            */
   /*------------------------------------------------------------------------*/

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetDistbCrit(
#ifndef LSNOPROTO
      char   * feature_name,   /* IN */
      char   * feature_version,   /* IN */
      char   * dist_crit,      /* OUT - pre-malloced */
      int       distcrit_buflen   /* IN */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetDistbCritToFile(
#ifndef LSNOPROTO
      char   * feature_name,   /* IN */
      char   * feature_version,   /* IN */
      char   *file_name     /* IN */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSchangeDistbCrit(
#ifndef LSNOPROTO
      char   * feature_name,
      char   * version,
      char   * dist_crit
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLeaderServerName(
#ifndef LSNOPROTO
      char   * leader_name, /* OUT */
      int       leader_name_len   /* IN */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSaddServerToPool(
#ifndef LSNOPROTO
      char *server_name,
      char *server_addr
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSdelServerFromPool(
#ifndef LSNOPROTO
      char *server_name,
      char *server_addr
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSaddFeatureExt(
#ifndef LSNOPROTO
      unsigned char *licenseString,    /* IN */
      unsigned char *DistCritString,   /* IN */
      unsigned char *log_comment,   /* IN */
      LS_CHALLENGE  *challenge   /* INOUT */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSaddFeatureToFileExt(
#ifndef LSNOPROTO
      unsigned char *    licenseString,      /* IN */
      unsigned char *    comment_info,      /* IN */
      unsigned char *    DistCritString,  /* IN */
      unsigned char *    log_comment,        /* IN */
      LS_CHALLENGE  *    challenge           /* INOUT */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLicSharingServerList
   (
#ifndef LSNOPROTO
      unsigned char *  feature_name,       /* IN */
      unsigned char *  feature_version,    /* IN */
      int              server_list_len,    /* IN */
      char          *  server_list,        /* OUT */
      int           *  num_servers         /* OUT */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetTrialPeriodLeft (
#ifndef LSNOPROTO
      unsigned char       * feature_name,     /* IN */
      unsigned char       * version ,         /* IN */
      unsigned long       * trialperiod,      /* OUT */
      unsigned char LSFAR * unused1

#endif /* LSNOPROTO */
   );

/* Callback mode for rehost - Pre-Check, or Operate - */
#define VLS_EVENT_CALLBACK_MODE_PRECHECK  'P'
#define VLS_EVENT_CALLBACK_MODE_OPERATE   'O'
#define VLS_TRANSACTION_ID_LENGTH         8

   typedef struct
   {
      unsigned char*   pucData;        /* any data */
      unsigned long    ulDataLength;
      unsigned char    cMode;          /* callback mode, 'P' for pre-check, and 'O' for others (Add, or Revoke) */
   } VLScallbackModeT;

   /* generate revocation ticket by permission ticket */
   VDLL32 LS_STATUS_CODE VMSWINAPI  VLSrevokeByPermissionTicket(
#ifndef LSNOPROTO
      unsigned char        * pucServerName,                /* IN - only for network license */
      unsigned char        * pucPassword,                  /* IN - currently unused, should be passed as NULL  */
      unsigned char        * pucPermissionTicket,          /* IN - the permission ticket (TLV) */
      unsigned int           ui16PermissionTicketLength,   /* IN - size of permission ticket */
      unsigned char        * pucRevocationTicket,          /* OUT - the generated rehost ticket */
      unsigned int         * pui16RevocationTicketLength   /* IN/OUT - IN: buffer size; OUT: ticket size */
#endif /* LSNOPROTO */
      );

   /* This API is introduced for new NR and deferred revoke to generate revocation ticket by permission ticket */
   VDLL32 LS_STATUS_CODE VMSWINAPI  VLSrevokeByPermissionTicketExt(
#ifndef LSNOPROTO
      unsigned char        * pucServerName,                /* IN - only for network license */
      unsigned char        * pucPassword,                  /* IN - currently unused, should be passed as NULL  */
      unsigned char        * pucPermissionTicket,          /* IN - the permission ticket (TLV) */
      unsigned int           ui16PermissionTicketLength,   /* IN - size of permission ticket */
      unsigned char        * pucRevocationTicket,          /* OUT - the generated rehost ticket */
      unsigned int         * pui16RevocationTicketLength   /* IN/OUT - IN: buffer size; OUT: ticket size */
#endif /* LSNOPROTO */
      );


/*This API gets the status,
 * For network model: whether the server is running on virtual machine
 * For stand alone model: whether the application is running on virtual machine*/
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSisVirtualMachine(
#ifndef LSNOPROTO
         VLSVMInfo *vm_info
         //VLS_VM_DETECTION_STATE *isVirtualMachine  /*IN/OUT */
#endif /* LSNOPROTO */
   );

/* 8.5.0 - This API sets the custom data i.e.host name and user name
 * that the user shall pass to the client library and pass on to the server */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetCustomData(
#ifndef LSNOPROTO
      VLScustomData *pCustomData
#endif /* LSNOPROTO */
);

   /*------------------------------------------------------------------------*/
   /* Macros with default licensing values.                                  */
   /* There should be no space(s) between macro name and open parenthesis.   */
   /* Most of these are for backward compatibility.                          */
   /*------------------------------------------------------------------------*/

#define VLS_REQUEST(feature_name, version, handle_addr)  \
        LSRequest(LS_ANY,(unsigned char LSFAR *) "Sentinel RMS Development Kit User", \
                     (unsigned char LSFAR *)feature_name, \
                     (unsigned char LSFAR *)version, \
                     (unsigned long LSFAR *)NULL, (unsigned char LSFAR *)NULL, \
                     (LS_CHALLENGE LSFAR *)NULL, handle_addr)

#define VLS_RELEASE(handle)     \
        LSRelease(handle, LS_DEFAULT_UNITS, (unsigned char LSFAR *)NULL)

#define VLS_UPDATE(handle)                                     \
        LSUpdate     (handle, LS_DEFAULT_UNITS, (long LSFAR *)NULL, \
                     (unsigned char LSFAR *)NULL, (LS_CHALLENGE LSFAR *)NULL)

#define VLS_INITIALIZE() VLSinitialize()

#define VLS_CLEANUP()    VLScleanup()

   /*-----------------------------------------------------------------------*/
   /* Macros over sharing APIs                                              */
   /*-----------------------------------------------------------------------*/
#define VLSsetTeamId(team_id, func_ptr) \
                VLSsetSharedId(team_id,func_ptr)
#define VLSsetTeamIdValue(team_id,team_id_value) \
                VLSsetSharedIdValue(team_id,team_id_value)

#define VLS_UNKNOWN_TEAM_ID VLS_UNKNOWN_SHARED_ID

   /*------------------------------------------------------------------------*/
   /* Macros which will make all Sentinel RMS Development Kit functions void:*/
   /*------------------------------------------------------------------------*/

#ifdef NO_LICENSE
#define LSGetMessage(a1,a2,a3,a4)                 (LS_SUCCESS)
#define LSRelease(a1,a2,a3)                       (LS_SUCCESS)
#define LSRequest(a1,a2,a3,a4,a5,a6,a7,a8)        (LS_SUCCESS)
#define LSUpdate(a1,a2,a3,a4,a5)                  (LS_SUCCESS)
#define VLSaddFeature(a1,a2,a3)                   (LS_SUCCESS)
#define VLSaddFeatureToFile(a1,a2,a3,a4)          (LS_SUCCESS)
#define VLSbatchUpdate(a1,a2,a3,a4,a5,a6,a7)      (LS_SUCCESS)
#define VLScleanup()                              (LS_SUCCESS)
#define VLSdeleteFeature(a1,a2,a3,a4)             (LS_SUCCESS)
#define VLSdisableAutoTimer(a1,a2)                (LS_SUCCESS)
#define VLSdisableLicense(a1)                     (LS_SUCCESS)
#define VLSdisableLocalRenewal()                  (LS_SUCCESS)
#define VLSdiscover(a1,a2,a3,a4,a5,a6,a7)         (LS_SUCCESS)
#define VLSenableLocalRenewal()                   (LS_SUCCESS)
#define VLSerrorHandle(a1)                        (LS_SUCCESS)
#define VLSgetBroadcastInterval()                 (9)
#define VLSgetClientInfo(a1,a2,a3,a4,a5)          (VLS_NO_MORE_CLIENTS)
#define VLSgetQueuedClientInfo(a1,a2,a3,a4,a5)    (VLS_NO_MORE_CLIENTS)
#define VLSgetContactServer(a1,a2)                (LS_SUCCESS)
#define VLSgetFeatureFromHandle(a1,a2,a3)         (LS_BADHANDLE)
#define VLSgetFeatureInfo(a1,a2,a3,a4,a5)         (VLS_NO_MORE_FEATURES)
#define VLSgetFeatureTimeLeftFromHandle(a1,a2)    (LS_BADHANDLE)
#define VLSgetHandleInfo(a1,a2)                   (LS_BADHANDLE)
#define VLSgetKeyTimeLeftFromHandle(a1,a2)        (LS_BADHANDLE)
#define VLSgetLibInfo(a1)                         (LS_SUCCESS)
#define VLSgetLicInUseFromHandle(a1,a2)           (LS_BADHANDLE)
#define VLSgetMachineID(a1,a2,a3)                 (LS_SUCCESS)
#define VLSgetRenewalStatus()                     (VLS_LOCAL_UPDATE)
#define VLSgetServerList(a1,a2)                   (LS_SUCCESS)
#define VLSgetServerNameFromHandle(a1,a2,a3)      (LS_SUCCESS)
#define VLSgetServerPort()                        (5093)
#define VLSgetTimeDriftFromHandle(a1,a2)          (LS_BADHANDLE)
#define VLSgetTimeoutInterval()                   (30)
#define VLSgetVersionFromHandle(a1,a2,a3)         (LS_BADHANDLE)
#define VLSgetVersions(a1,a2,a3,a4)               (VLS_NO_SUCH_FEATURE)
#define VLSinitMachineID(a1)                      (LS_SUCCESS)
#define VLSinitServerList(a1,a2)                  (LS_SUCCESS)
#define VLSinitialize()                           (LS_SUCCESS)
#define VLSisLocalRenewalDisabled()               (VLS_LOCAL_UPD_ENABLE)
#define VLSlicense(a1,a2,a3)                      (LS_SUCCESS)
#define VLSmachineIDtoLockCode(a1,a2,a3)          (LS_SUCCESS)
#define VLSmachineIDToLockCodeEx(a1,a2,a3,a4,a5)  (LS_SUCCESS)
#define VLSsetBroadcastInterval(a1)               (LS_SUCCESS)
#define VLSsetContactServer(a1)                   (LS_SUCCESS)
#define VLSsetErrorHandler(a1,a2)                 (LS_SUCCESS)
#define VLSsetRemoteRenewalTime(a1,a2,a3)         (LS_SUCCESS)
#define VLSsetTimeoutInterval(a1)                 (LS_SUCCESS)
#define VLSsetTraceLevel(a1)                      (LS_SUCCESS)
#define VLSsetTraceHandler(a1)                    (LS_SUCCESS)
#define VLSsetUserErrorFile(a1,a2)                (LS_SUCCESS)
#define VLSsetUserTraceFile(a1,a2)                (LS_SUCCESS)
#define VLSshutDown(a1)                           (LS_SUCCESS)
#define VLSdiscoverExt(a1,a2,a3,a4,a5,a6,a7,a8)   (LS_SUCCESS)
#define VLSgetDistbCrit(a1,a2,a3,a4)        (LS_SUCCESS)
#define VLSgetDistbCritToFile(a1,a2,a3)        (LS_SUCCESS)
#define VLSchangeDistbCrit(a1,a2,a3)              (LS_SUCCESS)
#define VLSgetLeaderServerName(a1,a2)            (LS_SUCCESS)
#define VLSaddServerToPool(a1, a2)           (LS_SUCCESS)
#define VLSdelServerFromPool(a1, a2)                  (LS_SUCCESS)
#define VLSaddFeatureExt(a1,a2,a3,a4)             (LS_SUCCESS)
#define VLSaddFeatureToFileExt(a1,a2,a3,a4,a5)    (LS_SUCCESS)
#define VLSgetActiveHandleList(a1,a2)             (VLS_NO_ACTIVE_HANDLE)
#define VLSisVirtualMachine(a1)                   (LS_SUCCESS)
#define VLSsetCustomData(a1)                      (LS_SUCCESS)
#endif /* NO_LICENSE */

   typedef enum {LOG_SRVR_UP = 1, \
                 LOG_LDR_ELECT, \
                 LOG_HRT_BT, \
                 LOG_BORROW_REQ_RESP, \
                 LOG_USG_NOTIFY, \
                 LOG_CHNG_DIST_CRIT, \
                 LOG_DIST_CRIT_SYNC, \
                 LOG_CFG_FILE, \
                 LOG_SRVR_DOWN, \
                 LOG_MOD_SERVER, \
                 LOG_ADD_DEL_LIC} srvrLogState;

#if defined(_VMSWIN_) || defined (WIN32)
#pragma pack(pop)
#endif /*_VMSWIN_*/

   /*********************************************************************j
   *
   * DESCRIPTION :
   *       This section contains prototypes and header declarations for
   *       customizing the client/server.  There are various aspects of the
   *       client/server that can be customized to suit a vendor's needs.
   *       This file lists all the aspects.
   *
   * USAGE       :
   *       All files related to customization must include this section.
   *       IMPORTANT-  If a vendor customizes his/her server in any way,
   *       he/she must also change the port number of his/her server via
   *       the API call VLSchangePortNumber(), so that the customized
   *       server does not interfere with other vendors' applications that
   *       rely on a default (uncustomized) server.  Of course, the clients
   *       must also be modified to contact the server on the new port
   *       number, using the client API call VLSsetServerPort().
   *
   * NOTES       :
   *       All functions in this section that are marked OVERRIDE, when present
   *       in vendor's object files, will override default function bodies
   *       present in static libraries of Sentinel RMS Development Kit.
   *       For this to work correctly, vendor must specify his/her overriding
   *       object files BEFORE RMS Development Kit libraries in the linker
   *       command.  These functions are called by the client/server as and
   *       when needed.
   *
   *       All functions in this section that are marked BUILT-IN, are
   *       functions that can be called from any vendor functions.  Vendor
   *       should NOT override these functions (i.e., provide his/her own
   *       functions by the same names).
   *
   *H*/


   /*------------------------------------------------------------------------*/
   /* Prototypes for client-side customization:                              */
   /*------------------------------------------------------------------------*/

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetHoldTime (  /* BUILT-IN */
#ifndef LSNOPROTO
      unsigned char   LSFAR *feature_name, /* IN */
      unsigned char   LSFAR *version,      /* IN */
      int                    hold_time     /* IN */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetSharedId (  /* BUILT-IN */
#ifndef LSNOPROTO
      int shared_id,                                              /* IN */
      LS_STATUS_CODE (VMSWINAPI * mySharedIdFunc) (char LSFAR *)  /* IN */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetSharedIdValue(  /* BUILT-IN */
#ifndef LSNOPROTO
      int                  shared_id,     /* IN */
      char          LSFAR *sharedIdValue  /* IN */
#endif
   );

   VDLL32 void VMSWINAPI VLSsetServerPort(  /* BUILT-IN */
#ifndef LSNOPROTO
      int port_number                    /* IN */
#endif
   );

   /*------------------------------------------------------------------------*/
   /* Struct containing time tampering Info.                                 */
   /*------------------------------------------------------------------------*/

   typedef struct  timetampering_info_struct
   {
      long   structSz;
      Time_T lastTime;
      Time_T currTime; /* current time */
      long   grace_period; /* grace-period allowed */
      int    percentViolationAllowed; /* percentage of violations allowed. in case of UNIX*/
      int    numViolationForError; /* number of violations allowed. in case of UNIX */
      int    numViolationFound; /*Actual number of violations found in case of UNIX*/
      int    percentViolationFound; /*Actual  percent violation found in case of UNIX*/
      unsigned long clkSetBackTime; /*Actual period  by which clock id found back*/
   }
   VLStimeTamperInfo;

   typedef struct  timetampering_info_struct64
   {
      long     structSz;
      Time64_T lastTime64;
      Time64_T currTime64; /* current time */
      long     grace_period; /* grace-period allowed */
      int      percentViolationAllowed; /* percentage of violations allowed. in case of UNIX*/
      int      numViolationForError; /* number of violations allowed. in case of UNIX */
      int      numViolationFound; /*Actual number of violations found in case of UNIX*/
      int      percentViolationFound; /*Actual  percent violation found in case of UNIX*/
      Time64_T clkSetBackTime64; /*Actual period  by which clock id found back*/
   }
   VLStimeTamperInfo64;

   /*------------------------------------------------------------------------*/
   /* structure contains server information                                   */
   /*------------------------------------------------------------------------*/

   typedef struct
   {
      long              structSz;
      int               major_no;
      int               minor_no;
      int               revision_no;
      int               build_no;
      unsigned char     locale[VLS_SERV_LOCALE_STR_LEN];
      unsigned char     vendor_info[VLS_SERV_VNDINFO_STR_LEN];
      unsigned char     platform[VLS_SERV_PLATFORM_STR_LEN];
      unsigned long     lock_mask;
      unsigned char     unused1[VLS_SERV_UNUSED1_STR_LEN];
      long              unused2;
      VLStimeTamperInfo tmtmpr_info;
      VLSmachineID      machine_id;
      VLStimeTamperInfo64 tmtmpr_info64;
   }
   VLSservInfo;

/* Values for last argument of VLSgetServInfoExt2 */
/* NOTE: If the user wants to retrieve more than one value, 
   the below values can also be OR'ed and passed to the API */
#define VLS_SI_SERVER_VERSION       0x1/*fetches version related information. This also includes server platform and locale information. 
                                         *Fields filled - major_no, minor_no, revision_no, build_no, locale and platform.*/
#define VLS_SI_VENDOR_IDENTIFIER    0x2/*fetches only vendor defined identifier registered with server. Fields filled - vendor_info.*/
#define VLS_SI_TIME_TAMPER_INFO     0x4/*fetches time-tamper related information. Fields filled - nested structure tmtmpr_info*/
#define VLS_SI_MACHINE_ID           0x8/*fetches server machine's finger print. Fields filled - lock_mask and nested structure machine_id*/
#define VLS_SI_ALL                  0xF/*fetches complete server-info struct parameters*/

   /*------------------------------------------------------------------------*/
   /* Prototypes for API retrieve information about server                   */
   /*------------------------------------------------------------------------*/

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetServInfo(
#ifndef LSNOPROTO
      unsigned char LSFAR *server_name, /*  IN */
      VLSservInfo   LSFAR *srv_info,  /* out */
      unsigned char LSFAR *unused1,   /*reserved*/
      unsigned long LSFAR *unused2    /*reserved*/
#endif
   );


#define VLSgetServInfoExt(server_name, srv_info, unused1, unused2)	VLSgetServInfoExt2(server_name, srv_info, NULL, NULL)


   /* 8.5.4 - Introducing lighter variant of the above API that fetches the information on request */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetServInfoExt2(
#ifndef LSNOPROTO
      unsigned char LSFAR *server_name, /*  IN */
      VLSservInfo   LSFAR *srv_info,  /* out */
      unsigned char LSFAR *unused1,   /*reserved*/
      unsigned long LSFAR *serverInfoReqMask /* IN */
#endif
   );
   /*------------------------------------------------------------------------*/
   /* Prototypes for client-and-server-side customization:                   */
   /*------------------------------------------------------------------------*/

   /* Supply custom hostid function/mechanism. If the function is called, it means that current custom lock criteria will
    * be changed. That is say current locked licenses may become invalid. Hence standalone server has to be cleaned up and
    * then re-initialized automatically. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetHostIdFunc(  /* BUILT-IN */
#ifndef LSNOPROTO
      unsigned long (VMSWINAPI * customGetHostIdFunc)(void)  /* IN */
#endif
   );

   /* Supply customEx lock function/mechanism. If the function is called, it means that current custom extended lock criteria will
    * be changed. That is say current locked licenses may become invalid. Hence standalone server has to be cleaned up and
    * then re-initialized automatically. */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetCustomExFunc(
#ifndef LSNOPROTO
      long (VMSWINAPI * pmyGetCustExTableFunc)(VLScustomEx* pCustomExTable, unsigned long* pulCount)
#endif
   );

   /* Network messages encryption/decryption customization: */
   VDLL32 int VMSWINAPI VLSencryptMsg(                         /* OVERRIDE */
#ifndef LSNOPROTO
      char *decrypted_mesg,          /* IN */
      char *encrypted_mesg,          /* OUT - allocated by caller */
      int   size                     /* IN */
#endif
   );

   VDLL32 int VMSWINAPI VLSdecryptMsg(                         /* OVERRIDE */
#ifndef LSNOPROTO
      char *encrypted_mesg,          /* IN */
      char *decrypted_mesg,          /* OUT - allocated by caller */
      int   size                     /* IN */
#endif
   );


   /*------------------------------------------------------------------------*/
   /* Types and prototypes for server hook functions customization:          */
   /*------------------------------------------------------------------------*/

#define HOOK_LS_MAX_PATHLEN           128   /* Path Length */
#define HOOK_CLIENT_IDENTIFIER_SIZE   100   /* Client identifier size */

   /* ***** Event types ***** */
#define LS_REQ_PRE               0 /* EVENT : Before processing lsreq() */
#define LS_REQ_POST              1 /* EVENT : After  processing lsreq() */
#define LS_REL_PRE               2 /* EVENT : Before processing lsrel() */
#define LS_REL_POST              3 /* EVENT : After  processing lsrel() */
/* Added 8.5.3: event types for update hook events */
#define LS_UPD_PRE               4 /* EVENT : Before processing update */
#define LS_UPD_POST              5 /* EVENT : After  processing update */


   /* ***** Error codes on server side ***** */
#define LSERV_STATUS_SUCCESS        LS_SUCCESS    /* Success status */
#define LSERV_STATUS_DENY           101  /* Denial by vendor event handler */

   typedef int LSERV_STATUS;

   /* Structure for File Location Info passed to vendor event handlers. */
   typedef struct
   {
      char       lservrcFile    [HOOK_LS_MAX_PATHLEN];   /* lservrc file path */
      char       lservrcCnfFile [HOOK_LS_MAX_PATHLEN];   /* lserv cnf file path */
      char       lservStaFile   [HOOK_LS_MAX_PATHLEN];   /* lserv usage file path */
      char       lservLogFile   [HOOK_LS_MAX_PATHLEN];   /* lserv error message file path */
      char       lsGrResvFile   [HOOK_LS_MAX_PATHLEN];   /* lserv group file path */
      char       reserved       [HOOK_LS_MAX_PATHLEN];   /* reserved */
   }
   VLSfileLocInfo;

   /* Structure for Misc. Info passed to vendor event handlers. */
   typedef struct
   {
      char            ipAddress     [VLS_MAX_NAME_LEN];    /* of client */
      /* Flags indicate status of tests for this request: */
      int             nodeLockPass;       /* 1 => Node locking tests pass */
      int             siteLicensePass;    /* 1 => Site licensing tests pass */
      int             licExpirationPass;  /* 1 => License expiration tests pass */
      int             clockTamperPass;    /* 1 => Clock tampering tests pass */
      char            reserved      [VLS_MAX_NAME_LEN];
   }
   VLSmiscInfo;

   /* The complete structure passed to vendor event handlers. */
   typedef struct
   {
      VLSclientInfo   clientInfoStruct;  /* Same as client API struct */
      VLSfeatureInfo  featureInfoStruct; /* Same as client API struct */
      VLSfileLocInfo  fileLocInfoStruct;
      VLSmiscInfo     miscInfoStruct;
   }
   VLShandlerStruct;


   /*
    * Called by server during server initialization.  This is where
    * calls to VLSeventAddHook() should be placed, to configure the server
    * to consult vendor event handler functions.
    */
   VDLL32 LSERV_STATUS VMSWINAPI VLSserverVendorInitialize(    /* OVERRIDE */
#ifndef LSNOPROTO
      void
#endif
   );

   VDLL32 LSERV_STATUS VMSWINAPI VLSserverVendorFinalize(    /* OVERRIDE */
#ifndef LSNOPROTO
      void
#endif
   );
   
   typedef enum {
		VLS_POLICY_LIC_INSTALL_ADD_LICENSE,    //add this license
		VLS_POLICY_LIC_INSTALL_IGNORE_LICENSE, //do not add this license
		VLS_POLICY_LIC_INSTALL_STOP_SERVER     // stop the server
   } VLS_POLICY_LIC_INSTALL_STATUS;
   
   typedef enum {
		VLS_POLICY_LIC_INSTALL_SERVER_STARTUP,       //license install scenario at server startup
		VLS_POLICY_LIC_INSTALL_DYNAMIC_ADDITION,     //license install scenario at the time of dynamic license addition    
   } VLS_POLICY_LIC_INSTALL_SCENARIO;
   
   /* Server customizable API to determine license install policy*/
   VLS_POLICY_LIC_INSTALL_STATUS VLSpolicyLicenseInstall (    /* OVERRIDE */
     unsigned char                   *license_string,        /* IN  */
     VLSlicenseInfo                  license_info,           /* IN */      
     VLS_POLICY_LIC_INSTALL_SCENARIO licInstallScenario      /* IN */
   );
 
   /*API to set vendor specific information */
   VDLL32 LSERV_STATUS VMSWINAPI VLSsetServerInfo(
#ifndef LSNOPROTO
      char  LSFAR **vendorInfo
#endif
   );

   /* API to set vendor specific identifier */
   VDLL32 LSERV_STATUS VMSWINAPI VLSenableVendorIsolation(
#ifndef LSNOPROTO
      VLSvendorIsolation *pVendorIdentifier,  /* OUT */
	  char LSFAR **reservedBuffer,     /* OUT */
      unsigned long *reserved
#endif
   );
   
   /* Call to register an event handler with the server. */
   VDLL32 LSERV_STATUS VMSWINAPI VLSeventAddHook(              /* BUILT-IN */
#ifndef LSNOPROTO
      int       eventName,  /* IN - event type , LS_REQ / LS_REL */
      /* _PRE / _POST */
      int       (*handlerFuncPtr)(VLShandlerStruct *, char *, char *, int),
      /* IN - function pointer */
      char   *  identifier  /* IN - client identifier to match */
#endif
   );


   /*------------------------------------------------------------------------*/
   /* Client-side calls to use Server Hooks:                                 */
   /*------------------------------------------------------------------------*/

   /* Struct passed to server from client while using server hooks: */
   typedef  struct
   {
      char   identifier[VLS_MAX_NAME_LEN];
      char   inBuf[VLS_MAX_BUF_LEN];   /* String passed to the server */
      char   outBuf[VLS_MAX_BUF_LEN];  /* String returned by the server */
   }
   VLSserverInfo;


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSinitServerInfo (  /* BUILT-IN */
#ifndef LSNOPROTO
      VLSserverInfo    LSFAR    *serverInfo  /* OUT - allocated by caller */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrequestExt (  /* BUILT-IN */
#ifndef LSNOPROTO
      unsigned char  LSFAR *license_system,  /* IN */
      unsigned char  LSFAR *publisher_name,  /* IN */
      unsigned char  LSFAR *product_name,    /* IN */
      unsigned char  LSFAR *version,         /* IN */
      unsigned long  LSFAR *units_reqd,      /* IN */
      unsigned char  LSFAR *log_comment,     /* IN */
      LS_CHALLENGE   LSFAR *challenge,       /* INOUT - allocated by caller */
      LS_HANDLE      LSFAR *lshandle,        /* OUT - allocated by caller */
      VLSserverInfo  LSFAR *serverInfo       /* INOUT - allocated by caller */
#endif /* LSNOPROTO */
   );

   /* Added 8.5.3 to support server hook in updates */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSupdateExt (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,                  /* IN */
      unsigned long         ulGraceSwitchToNetworkTm,  /* IN */
      long           LSFAR *new_units_reqd,            /* IN/OUT - new units required */
      unsigned char  LSFAR *log_comment,               /* IN */
      LS_CHALLENGE   LSFAR *challenge,                 /* IN/OUT */
      VLSserverInfo  LSFAR *serverInfo                 /* IN/OUT */
#endif /* LSNOPROTO */
   );

   /* Added 9.5.0 to support higher value for new_units_required to VLS_KEY_MAX_LIMIT */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSupdateExt2 (
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,                  /* IN */
      unsigned long         ulGraceSwitchToNetworkTm,  /* IN */
      unsigned int   LSFAR *new_units_reqd,            /* IN/OUT - new units required */
      unsigned char  LSFAR *log_comment,               /* IN */
      LS_CHALLENGE   LSFAR *challenge,                 /* IN/OUT */
      VLSserverInfo  LSFAR *serverInfo                 /* IN/OUT */
#endif /* LSNOPROTO */
   );


   /* New Client API for requesting Capacity licenses.
    * Added in SLM 7.3.0
    */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrequestExt2 (  /* BUILT-IN */
#ifndef LSNOPROTO
      unsigned char  LSFAR *license_system,  /* IN */
      unsigned char  LSFAR *publisher_name,  /* IN */
      unsigned char  LSFAR *product_name,    /* IN */
      unsigned char  LSFAR *version,         /* IN */
      unsigned long  LSFAR *units_reqd,      /* IN */
      unsigned char  LSFAR *log_comment,     /* IN */
      LS_CHALLENGE   LSFAR *challenge,       /* INOUT - allocated by caller */
      LS_HANDLE      LSFAR *lshandle,        /* OUT - allocated by caller */
      VLSserverInfo  LSFAR *serverInfo,      /* INOUT - allocated by caller */
      unsigned long  LSFAR *team_capacity_reqd,
      unsigned long  LSFAR *user_capacity_reqd,
      unsigned char  LSFAR *vendor_identifier, /* IN - 	value should be same as vendor-identifier used while calling VLSenableVendorIsolation */
      unsigned long  LSFAR *special_flag     /* INOUT - general purpose flag.
                                                  Used for grace installation error
                                                  handling in SLM 800 */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSqueuedRequestExt (  /* BUILT-IN */
#ifndef LSNOPROTO
      unsigned char      LSFAR *license_system,  /* IN */
      unsigned char      LSFAR *publisher_name,  /* IN */
      unsigned char      LSFAR *product_name,    /* IN */
      unsigned char      LSFAR *version,         /* IN */
      unsigned long      LSFAR *units_reqd,      /* IN */
      unsigned char      LSFAR *log_comment,     /* IN */
      LS_CHALLENGE       LSFAR *challenge,       /* INOUT - allocated by caller */
      LS_HANDLE          LSFAR *lshandle,        /* OUT - allocated by caller */
      VLSqueuePreference LSFAR *qPreference,     /* INOUT - allocated by caller*/
      int                LSFAR *requestFlag,     /* INOUT - allocated by caller*/
      VLSserverInfo      LSFAR *serverInfo       /* INOUT - allocated by caller */
#endif /* LSNOPROTO */
   );

   /* Returns LS_SUCCESS if the number of units actually returned is same as the
      value of "units_consumed" paramter passed to this API.
      Returns VLS_ALL_UNITS_RELEASED, if it returnes all the units evenif asked for
      some specific number of units to be returned.
    */

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSreleaseExt (  /* BUILT-IN */
#ifndef LSNOPROTO
      LS_HANDLE             lshandle,        /* IN */
      unsigned long         units_consumed,  /* IN */
      unsigned char  LSFAR *log_comment,     /* IN */
      VLSserverInfo  LSFAR *serverInfo       /* INOUT - allocated by caller */
#endif /* LSNOPROTO */
   );


   /*------------------------------------------------------------------------*/
   /* Time tamper customization (for server and standalone mode):            */
   /*------------------------------------------------------------------------*/

   typedef enum {VLS_CONT_AFTER_TM_TAMPER, VLS_EXIT_AFTER_TM_TAMPER}
   VLSactionOnTmTamper;

   typedef enum {VLS_ENABLE_DEFAULT_TM_TAMPER, VLS_DISABLE_DEFAULT_TM_TAMPER}
   VLStmTamperMethod;


   /*
    * Called by server each time server needs to verify whether the system
    * clock has been set back.  Default behavior of the server can be
    * customized here.  Note this is called BEFORE any checks are performed
    * by the server.
    */
   VDLL32 void VMSWINAPI VLSconfigureTimeTamper (              /* OVERRIDE */
#ifndef LSNOPROTO
      VLSactionOnTmTamper *   actionOnTmTamper,       /* OUT */
      VLStmTamperMethod   *   tmTamperMethod,         /* OUT */
#ifdef _V_LP64_
      int                *   gracePeriod,            /* OUT */
#else
      long                *   gracePeriod,            /* OUT */
#endif
      int                 *   percentViolations,      /* OUT */
      int                 *   numViolationsForError   /* OUT */
#endif
   );

   /*
    * Vendor's function to tell the server if clock has been set back.
    * Called only in case vendor's VLSconfigureTimeTamper() function returns
    * tmTamperMethod to be VLS_DISABLE_DEFAULT_TM_TAMPER, not otherwise.
    * Should return 0 if clock is not set back.
    */
   VDLL32 int VMSWINAPI VLSisClockSetBack(
#ifndef LSNOPROTO
      void
#endif
   );                   /* OVERRIDE */



   /*------------------------------------------------------------------------*/
   /* License encryption/decryption customization (server and standalone)    */
   /*------------------------------------------------------------------------*/

   VDLL32 int VMSWINAPI VLSencryptLicense(                     /* OVERRIDE */
#ifndef LSNOPROTO
      char *decrypted_mesg,          /* IN */
      char *encrypted_mesg,          /* OUT - allocated by caller */
      int   size                     /* IN */
#endif
   );

   VDLL32 int VMSWINAPI VLSdecryptLicense(                     /* OVERRIDE */
#ifndef LSNOPROTO
      char *encrypted_mesg,          /* IN */
      char *decrypted_mesg,          /* OUT - allocated by caller */
      int   size                     /* IN */
#endif
   );


   /*------------------------------------------------------------------------*/
   /* Upgrade License encryption/decryption customization (server and standalone)    */
   /*------------------------------------------------------------------------*/

   VDLL32 int VMSWINAPI VLSencryptUpgradeLicense(                     /* OVERRIDE */
#ifndef LSNOPROTO
      char *decrypted_mesg,          /* IN */
      char *encrypted_mesg,          /* OUT - allocated by caller */
      int   size                     /* IN */
#endif
   );

   VDLL32 int VMSWINAPI VLSdecryptUpgradeLicense(                     /* OVERRIDE */
#ifndef LSNOPROTO
      char *encrypted_mesg,          /* IN */
      char *decrypted_mesg,          /* OUT - allocated by caller */
      int   size                     /* IN */
#endif
   );


   /*------------------------------------------------------------------------*/
   /* Server UDP port number customization:                                  */
   /*------------------------------------------------------------------------*/

   /* Should return the desired UDP port number of server */
   VDLL32 int VMSWINAPI VLSchangePortNumber(                   /* OVERRIDE */
#ifndef LSNOPROTO
      int  currentPort      /* IN  - Currently configured port number */
#endif
   );

   /*------------------------------------------------------------------------*/
   /* Server TFTP port number customization:                                  */
   /*------------------------------------------------------------------------*/

   /* Should return the desired UDP port number of server */
   VDLL32 int VMSWINAPI VLSchangeTFTPPortNumber(                   /* OVERRIDE */
#ifndef LSNOPROTO
      int  current_port      /* IN  - Currently configured TFTP port number */
#endif
   );
   
   /* The following API VLSconfigureServerFunctionality is DEPRECATED from 8.6.0 onwards */
   /* Should return the desired configuration of server */
   VDLL32 unsigned long VMSWINAPI VLSconfigureServerFunctionality(
#ifndef LSNOPROTO
		unsigned long current_config /*IN - Current configuration */
#endif
	);

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetPoolServerList (
#ifndef LSNOPROTO
      char * outBuf,
      int    outBufSz
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetBorrowingStatus (
#ifndef LSNOPROTO
      char   *   feature_name,
      char   *   version,
      int        state
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI  VLSsetServerLogState (
#ifndef LSNOPROTO
      int  event,
      int  state
#endif
   );

#ifdef _VMSWIN_
   VDLL32 LS_STATUS_CODE VMSWINAPI  VLSsetOutputCP(
#ifndef LSNOPROTO
      int  cp /*in*/
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI  VLSgetOutputCP(
#ifndef LSNOPROTO
      int  *cp    /*out*/
#endif
   );
#endif  /*_VMSWIN_*/

   /* Available only on UNIX */
   /* Function to be used instead of sleep */
   void  VLSeventSleep (
#ifndef LSNOPROTO
      unsigned long seconds
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetMachineIDString(
#ifndef LSNOPROTO
      unsigned long *lock_selector, /* INOUT   */
      unsigned char *machineIDString,  /* OUT - preallocated*/
      unsigned long *bufSz /* INOUT - returns buffer size if
                                    machineIDString is NULL */
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetCommuterCode (
#ifndef LSNOPROTO
      unsigned char  *feature_name,    /* IN */
      unsigned char  *feature_version, /* IN */
      unsigned long  *units_reqd,      /* IN */
#if ((defined _HP_UX11_  || defined _AIX_5X_) && defined _V_LP64_)
      int *duration,        /* IN/OUT */
#else
      unsigned long *duration,
#endif
      unsigned long  *lock_mask,       /* IN */
      unsigned char  *log_comment,     /* IN */
      unsigned char  *machineIDString, /* IN */
      unsigned char  *commuter_code,   /* OUT */
      LS_CHALLENGE   *challenge,       /* IN/OUT */
      VLSserverInfo  *requestInfo,     /* IN/OUT */
      VLSserverInfo  *commuterInfo,    /* IN/OUT - to be used in future for hooks */
      unsigned long  *extension_in_duration  /* IN */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSinstallCommuterCode (
#ifndef LSNOPROTO
      unsigned char  *commuter_code, /* IN */
      unsigned char  *reserved1,     /* IN */
      unsigned long   reserved2      /* IN */
#endif /* LSNOPROTO */
   );

   LS_STATUS_CODE VLSgenerateUpgradeLockCode (
#ifndef LSNOPROTO
      unsigned char *lic_string,
      unsigned char *upgrade_lock_code,
      unsigned long *szUpgradecode
#endif
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLScontrolRemoteSession (
#ifndef LSNOPROTO
      int toCheckRemoteSession
#endif
   );

   /**********************************************************************************
    * DESCRIPTION :
    *  It decodes the upgarde lock code  string "UpgradeLockCodeString" and puts the
    *  corresponding ulcCode struct in the last argument.Address of a pointer to
    *  ulcCode struct is to be passed as the last argument.
    *  This pointer will contain the ulcCode of the input upgardelock code string.
    *  This function takes care of all memory allocations it uses.
    *
    * RETURN VALUES:
    *  returns LS_SUCCESS on successful return.
    *  returns LS_NORESOURCES on malloc failure.
    *  else returns VLS_INTERNAL_ERROR
    * NOTE :
    */

   int VLSdecodeUpgradelockCode(
#ifndef LSNOPROTO
      char *           upgrade_lock_code,         /* IN  upgrade lock code string*/
      char *           compacted_upd_lock_code,   /* OUT upgrade lock code string after removing
                                                     *     comment chars and white spaces
                                                     *     NOTE:if this parameter "compacted_upd_lock_code"
                                                     *          is set as NULL then set third parameter "length"
                                                     *          as 0 else set third parameter length  as
                                                     *         " (sizeof(compacted_upd_lock_code)+1)"
                                                     */
      int              length,                    /* IN  length of compacted_upd_lock_code */
      ulcCode **       ulcCodePP                  /* OUT */
#endif /* LSNOPROTO */
   );

   
   /* This API provide an interface for getting information on all the
    * licenses for a specific Feature/Version/Vendor that are loaded
    * to the server in-memory table.
    */

    VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLicenseInfo
    (
#ifndef LSNOPROTO
       unsigned char     *feature_name,           /* IN */
       unsigned char     *version,                /* IN */
       int                feature_index,          /* IN */
       unsigned char     *license_hash,           /* IN */
       int                license_hash_len,       /* IN */
       int                license_index,          /* IN */
       VLSlicenseInfo    *license_info            /*IN/OUT */
#endif
    );

    VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLicenseInfoExt
    (
#ifndef LSNOPROTO
       unsigned char     *feature_name,           /* IN */
       unsigned char     *version,                /* IN */
       unsigned long     *puiCapacity,            /* IN */
       int                feature_index,          /* IN */
       unsigned char     *license_hash,           /* IN */
       int                license_hash_len,       /* IN */
       int                license_index,          /* IN */
       VLSlicenseInfo    *license_info            /*IN/OUT */
#endif
    );
   /* This API provide an interface for getting cumulative Trial usage
    * information of all the licenses for a specific Feature/Version/Vendor
    * that are loaded to the server in-memory table.
    */

    VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetTrialUsageInfo
    (
#ifndef LSNOPROTO
       unsigned char     *feature_name,           /* IN */
       unsigned char     *version,                /* IN */
       int                feature_index,          /* IN */
       VLStrialUsageInfo *trial_usage_info        /*IN/OUT */
#endif
    );


   /* This API provide an interface for setting the precedence level for
    * a trial license which is already added to the server.
    * Here precedence_level can be either:
    *                                     0
    *                                     -1
    *                                     >= 1
    */

    VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetLicensePrecedence
    (
#ifndef LSNOPROTO
       unsigned char     *feature_name,           /* IN */
       unsigned char     *version,                /* IN */
       unsigned char     *license_hash,           /* IN */
       int                license_hash_len,       /* IN */
       int                precedence_level,       /* IN */
       void              *unused1,                /* IN */
       int                unused2                 /* IN */
#endif
    );


   /* This API provides an interface for deleting a license added to
    * the server from both the server in-memory table and the license
    * storage. The function takes as input the license name, version
    * and the license hash to identify a license string.
    */

    VDLL32 LS_STATUS_CODE VMSWINAPI VLSdeleteLicenseFromFile
    (
#ifndef LSNOPROTO
       unsigned char     *feature_name,           /* IN */
       unsigned char     *version,                /* IN */
       unsigned char     *license_hash,           /* IN */
       int                license_hash_len,       /* IN */
       unsigned char     *license_string,         /* OUT*/
       int               *license_string_bufsize, /* IN/OUT */
       void              *unused1,                /* IN */
       int                unused2                 /* IN */
#endif
    );

    VDLL32 LS_STATUS_CODE VMSWINAPI VLSdeleteLicenseFromFileExt
    (
#ifndef LSNOPROTO
       unsigned char     *feature_name,           /* IN */
       unsigned char     *version,                /* IN */
       unsigned long     *puiCapacity,            /* IN */
       unsigned char     *license_hash,           /* IN */
       int                license_hash_len,       /* IN */
       unsigned char     *license_string,         /* OUT*/
       int               *license_string_bufsize, /* IN/OUT */
       void              *unused1,                /* IN */
       int                unused2                 /* IN */
#endif
    );

   /* This API extracts the last error status code for the specified
    * client handle identifier.
    */

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgetLastErrorStatusFromHandle
   (
#ifndef LSNOPROTO
      LS_HANDLE         lshandle,                  /* IN */
      LS_STATUS_CODE   *status_code                /* OUT */
#endif
   );


   /* This API sets the the behaviour if a grace license can be requested
    * or not when the contact server has been set to "no-net"
    */

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetGraceRequestFlag
   (
#ifndef LSNOPROTO
      int    state  /* IN ( VLS_ON(default)/VLS_OFF) */
#endif
   );

   /* This API returns the state if the grace request is enabled
    * or disabled when the contact server is set to no-net.
    */

   VDLL32 int VMSWINAPI VLSgetGraceRequestFlag
   (
#ifndef LSNOPROTO
      void
#endif
   );


    /* This API provides an interface for cleaning up issued keys
     * details based on the mask value provided of the client from 
     * the contact server over the network.
     */

VDLL32 LS_STATUS_CODE VMSWINAPI VLScleanupIssuedKeys(
#ifndef LSNOPROTO
      unsigned char              *feature_name,           /*  IN  */
      unsigned char              *feature_version,        /*  IN  */
      unsigned long              *capacity,               /*  IN  */
      int                        *client_identity_mask,   /*  IN/OUT  */
      unsigned char              *log_comment,            /*  IN  */
      unsigned long              *unused1,                /*  IN  */
      unsigned char              *unused2                 /*  IN  */
#endif /* LSNOPROTO */
);


 /*Signing key index for restricting license consumption to the more secure RSA signed licenses (version 18 and above).*/
#define VLS_RSA_SIGNING_KEY_INDEX                1
/*Signing key index for allowing consumption of licenses of all versions, including the AES encrypted licenses (version 17 and below).*/
#define VLS_AES_SIGNING_KEY_INDEX                0
#define VLS_DEFAULT_SIGNING_KEY_INDEX            VLS_AES_SIGNING_KEY_INDEX

/* This API sets the minimum signing key index.
*  Please go through the 'API Reference Guide' for details.
*/
VDLL32 LS_STATUS_CODE VMSWINAPI VLSsetMinimumSigningKeyIndex
(
#ifndef LSNOPROTO
  unsigned int uiMinimumSigningKeyIndex /*IN - VLS_RSA_SIGNING_KEY_INDEX /VLS_AES_SIGNING_KEY_INDEX */
#endif
);

   /*------------------------------------------------------------------------*/
   /* Macros with default licensing values.                                  */
   /* There should be no space(s) between macro name and open parenthesis.   */
   /*------------------------------------------------------------------------*/

#define VLSgetLicenseInfoByIndex(featureIndex, license_hash, license_hash_len, \
                                                  license_index, license_info) \
        VLSgetLicenseInfo((unsigned char *)NULL,\
                          (unsigned char *)NULL,\
                          featureIndex,\
                          license_hash,\
                          license_hash_len,\
                          license_index,\
                          license_info)

#define VLSgetLicenseInfoByName(feature_name, version, license_hash,\
                                license_hash_len, license_index, license_info)\
        VLSgetLicenseInfo((unsigned char *)feature_name,\
                          (unsigned char *)version,\
                          -1,\
                          license_hash,\
                          license_hash_len,\
                          license_index,\
                          license_info)


   /*------------------------------------------------------------------------*/
   /* Macros which will make all Sentinel RMS Development Kit functions void:*/
   /*------------------------------------------------------------------------*/

#ifdef NO_LICENSE
#define VLSinitServerInfo(a1)                                  (LS_SUCCESS)
#define VLSeventAddHook(a1,a2,a3)                              (LS_SUCCESS)
#define VLSreleaseExt(a1,a2,a3,a4)                             (LS_SUCCESS)
#define VLSrequestExt(a1,a2,a3,a4,a5,a6,a7,a8,a9)              (LS_SUCCESS)
#define VLSsetHoldTime(a1,a2)                                  (LS_SUCCESS)
#define VLSsetHostIdFunc(a1)                                   (LS_SUCCESS)
#define VLSsetCustomExFunc(a1)                                 (LS_SUCCESS)
#define VLSsetServerPort(a1)                                /* void return */
#define VLSsetSharedId(a1,a2)                                  (LS_SUCCESS)
#define VLSsetSharedIdValue(a1,a2)                             (LS_SUCCESS)
#define VLSrequestExt2(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13)  (LS_SUCCESS)
#define VLSgetFeatureInfoExt(a1,a2,a3,a4,a5,a6,a7)             (VLS_NO_MORE_FEATURES)
#define VLSgetClientInfoExt(a1,a2,a3,a4,a5,a6,a7)              (VLS_NO_MORE_CLIENTS)
#define VLSgetCapacityList(a1,a2,a3,a4,a5,a6,a7)               (VLS_NO_MORE_FEATURES)
#define VLSgetCapacityFromHandle(a1,a2,a3,a4)                  (LS_BADHANDLE)
#define VLSdeleteFeatureExt(a1,a2,a3,a4,a5,a6,a7)              (LS_SUCCESS)
#define VLSgetLicenseInfo(a1,a2,a3,a4,a5,a6,a7)                (VLS_NO_MORE_LICENSES)
#define VLSgetTrialUsageInfo(a1,a2,a3,a4)                      (VLS_NO_MORE_FEATURES)
#define VLSsetLicensePrecedence(a1,a2,a3,a4,a5,a6,a7)          (LS_SUCCESS)
#define VLSdeleteLicenseFromFile(a1,a2,a3,a4,a5,a6,a7,a8)      (LS_SUCCESS)
#define VLSgetLastErrorStatusFromHandle(a1,a2)                 (LS_SUCCESS)

#endif  /* end of NO_LICENSE */

#ifdef __cplusplus
}
#endif

#endif /* _LSERV_H_ */




