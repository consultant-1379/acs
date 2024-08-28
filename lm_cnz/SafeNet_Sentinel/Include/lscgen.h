/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

/*H****************************************************************
* FILENAME    : lscgen.h
*
* DESCRIPTION :
*     PUBLIC API.
*     This file contains public types, defines and prototypes to be used by
*     applications using the license code generation library.
*
*     Contains functions to check dependencies between user-provided
*     licensing parameters, and functions to test validity of data
*     entered by the user. Hence, all checking functions accept the
*     data as strings (char*), and convert into integers internally.
*
* USAGE       :
*
*      To license an application the first call should be to
*      VLScgInitialize() API. This API does the following tasks :
*
*      a) It first checks for the maximum limit of handles.
*
*      b) Allocates memory for internal data structures.
*
*      c) Initializes the error list and sets error count to
*         zero and sets the maximum severity of error to the lowest
*         possible value.
*
*  NOTE :
*         The handle is of type int and  provides an index
*         to internal data structures which are further used
*         for error processing, etc.
*
*       The last call in an application should be to VLScgCleanup()
*       API. In fact corresponding to every VLScgInitialize() there
*       should be a VLScgCleanup() call. This API function performs the
*       following tasks :
*
*     a) It frees the memory allocated for internal data structures.
*
*
*     Typical sequence of calls would be:
*     First allocate (memory for) a codeT struct.
*     Call VLScgReset on each new codeT struct, before filling
*     values into it.
*     Call VLScgSetCodegenVersion API to set the version of license
*     codes to generate.
*     Obtain input from the user. Sequence of input is important.
*     Should call the VLScgAllow functions to check feature-dependencies
*     between various codegen capabilities. For instance, vendor info
*     is allowed only in long codes.
*     Call the VLScgSet functions to test-and-set values given by
*     the user. These functions also record errors, which can be
*     printed out using API call VLScgPrintError() after every VLScgSet
*     call.
*     After all input is received, call VLScgGenerateLicense()
*     which generates the license string.
*
*
*     Possible sequence of calling VLScgAllowXXX() and VLScgSetXXX()
*     functions :
*
*
*    1. First get the type of code generated i.e short/long.
*
*    2. VLScgAllowFeatureName() and corresponding set function
*
*    3. VLScgAllowFeatureVersion() and corresponding set function
*
*    4. VLScgAllowCapacityLic() and set function
*
*    5. VLScgAllowStandAloneFlag() and set function
*
*    6. VLScgAllowAdditive() and set function
*
*    7. VLScgAllowVendorInfo() and set function
*
*    8a. VLScgAllowSharedLic()/VLScgAllowTeamCriteria() and set function
*        VLScgSetSharedLicType()/VLScgSetTeamCriteria()
*
*    8b. VLScgAllowShareLimit()/VLScgAllowTeamLimit() and set function
*        VLScgSetShareLimit()/VLScgSetTeamLimit()
*
*    9. VLScgAllowKeyLifeUnits() and set function
*       VLScgAllowKeyLifetime() and set function
*
*   10. VLScgAllowHeldLic() and set function
*       VLScgSetHoldingCrit()
*
*   11. VLScgAllowKeyHoldUnits() and set function
*
*   12. VLScgAllowKeyHoldtime() and set function
*
*   13. VLScgAllowSecrets() and set function
*
*   14. VLScgAllowLicBirth() and corresponding set functions for
*       setting the year, month and day of commencement, in that order.
*
*   15. VLScgAllowLicExpiration() and set functions for setting
*       the year, month and day of expiration, in that order.
*
*   16. VLScgAllowNumKeys() and set function. This is the hard
*        concurrent limit.
*
*   17. VLScgAllowSoftLimit() and set function.
*
*   18. VLScgAllowLockModeQuery() and set function
*       VLScgSetClientServerLockMode()
*
*   19. VLScgAllowServerLockInfo() and set functions
*       VLScgSetServerLockMechanism1()
*       VLScgSetServerLockInfo1()
*       VLScgSetServerLockMechanism2()
*       VLScgSetServerLockInfo2()
*
*   20. VLScgAllowClientLockInfo() and set functions
*       VLScgSetClientLockMechanism()
*       VLScgSetClientLockInfo()
*       VLScgSetKeysPerNode()
*
*   21. VLScgAllowSiteLic() and set function.
*       VLScgSetNumSubnets()
*       VLScgSetSiteLicInfo()
*
*   22. VLScgAllowClockTamperFlag() and set function.
*
*   23. VLScgAllowOutLicType() and set function
*
*   24. VLScgAllowLicenseType().
*
*   25. VLScgAllowCodegenVersion() and set function
*
*   26. VLScgAllowRedundantFlag() and set function
*
*   27. VLScgAllowMajorityRuleFlag() and set function
*
*   28. VLScgAllowCommuterLicense() and set function
*
*   29. VLScgAllowLogEncryptLevel() and set function
*
*   30. VLScgAllowMultiKey() and set function
*
*   31. VLScgAllowMultipleFeature() and set function
*
*   32. VLScgAllowVmDetection() and set function
*
* NOTES       :
*     Hook for vendor encryption is called while generating the license.
*     Never directly modify the codeT struct, always use VLScgSet
*     functions.
*
*
*H*/

#define _LSCGEN_UNIX_

#ifndef _LSCGEN_H_
#define _LSCGEN_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "lserv.h"     /* For some defines */

   typedef int VLScg_HANDLE;


   /* Standard return/error codes: */

#define VLScg_SUCCESS                      0      /* Success */
#define VLScg_NO_FEATURE_NAME              2
#define VLScg_INVALID_INT_TYPE             3
#define VLScg_EXCEEDS_MAX_VALUE            4
#define VLScg_LESS_THAN_MIN_VALUE          5
#define VLScg_EXCEEDS_MAX_STRLEN           6
#define VLScg_NOT_MULTIPLE                 7
#define VLScg_INVALID_DEATH_YEAR           8
#define VLScg_INVALID_BIRTH_YEAR           9
#define VLScg_INVALID_DATE                 10
#define VLScg_INVALID_HEX_TYPE             11
#define VLScg_INVALID_IP_TYPE              12
#define VLScg_INVALID_YEAR                 13
#define VLScg_RESERV_STR_ERR               14
#define VLScg_INVALID_RANGE                15
#define VLScg_INVALID_CHARS                16
#define VLScg_SHORT_STRING                 17
#define VLScg_PREMATURE_TERM               18
#define VLScg_REMAP_DEFAULT                19
#define VLScg_DECRYPT_FAIL                 20
#define VLScg_DYNAMIC_DECRYPT_FAILURE      21
#define VLScg_INVALID_CHKSUM               22
#define VLScg_FIXED_STR_ERROR              23
#define VLScg_SECRET_DECRYPT_FAILURE       24
#define VLScg_SIMPLE_ERROR                 25
#define VLScg_MALLOC_FAILURE               26
#define VLScg_INTERNAL_ERROR               27
#define VLScg_UNKNOWN_LOCK                 28
#define VLScg_VALUE_LARGE                  29
#define VLScg_INVALID_INPUT                30
#define VLScg_MAX_LIMIT_CROSSED            31
#define VLScg_NO_RESOURCES                 32
#define VLScg_BAD_HANDLE                   33
#define VLScg_FAIL                         34
#define VLScg_INVALID_VENDOR_CODE          35
#define VLScg_VENDOR_ENCRYPTION_FAIL       36
#define VLScg_INVALID_EXP_DATE             37
#define VLScg_INVALID_EXP_YEAR             38
#define VLScg_INVALID_EXP_MONTH            39

   /*
    * RMS Development Kit LicenseMeter related error codes.
    */
#define VLScg_LICMETER_EXCEPTION           40
#define VLScg_LICMETER_DECREMENT_OK        41
#define VLScg_LICMETER_ACCESS_ERROR        42
#define VLScg_LICMETER_COUNTER_TOOLOW      43
#define VLScg_LICMETER_CORRUPT             44
#define VLScg_LICMETER_VERSION_MISMATCH    45
#define VLScg_LICMETER_EMPTY               46

   /*
    * RMS Development Kit PortableServer related error codes.
    */
#define VLScg_PORTSERV_EXCEPTION           47
#define VLScg_PORTSERV_ACCESS_ERROR        48
#define VLScg_PORTSERV_VERSION_MISMATCH    49
#define VLScg_PORTSERV_CORRUPT             50

   /*
    * RMS Development Kit LicenseMeter related constants.
    */
#define VLScg_LICMETER_UNITS_INFINITE      (long) (-1)
#define VLScg_LICMETER_UNITS_UNAVAILABLE   (long) (-2)
#define VLScg_TRIALMETER_UNITS_UNAVAILABLE (long) (-3)


   /* New Error codes */
#define VLScg_EXPIRED_LICENSE               51
#define VLScg_INVALID_LICTYPE               52
#define VLScg_INVALID_TRIALDAYS             53
#define VLScg_INVALID_TRIAL_COUNT           54
#define VLScg_TRIALMETER_EMPTY              55
#define VLScg_TRIAL_SUCCESS                 56
#define VLScg_NO_NETWORK_AUTHORIZATION      57
#define VLScg_NO_ENABLE_FEATURE             58
#define VLScg_VI18N_INITIALIZE_FAIL         59
#define VLScg_INVALID_NUM_SERVERS           60
#define VLScg_NO_CAPACITY_AUTHORIZATION     61
#define VLScg_LICMETER_NOT_SUPPORTED        70
#define VLScg_INCORRECT_BASE_FEATURE_VAL    71
#define VLScg_ENCRYPTION_FAIL               72
#define VLScg_INVALID_CAPACITY_SETTINGS     73
#define VLScg_EXP_DATE_BEFORE_START_DATE    74

#define VLScg_INVALID_TRIALHOURS            75
#define VLScg_INVALID_TRIAL_EXECUTIONCOUNT  76
#define VLScg_GETLOCK_FAIL                  77
#define VLScg_GETLOCK_TIMEOUT               78

#define VLScg_VENDOR_DECRYPTION_FAIL        79

/* Rehost related error codes */
/* Version value found in revocation ticket does not
   match with that specified in request structure */
#define VLScg_RT_VERSION_MISMATCH                   80
/* One or more operation specific information in revocation
   ticket is invalid */
#define VLScg_RT_REHOST_LINE_CORRUPT                81
/* Transaction ID value found in revocation ticket does not
   match with that specified in request structure */
#define VLScg_RT_TRANSACTION_ID_MISMATCH            82
/* Lock code value found in revocation ticket does not
   match with that specified in request structure */
#define VLScg_RT_LOCK_CODE_MISMATCH                 83
/* One or more operation(s) specified in request structure
   is/are not performed at client side as corresponding
   information is not found in revocation ticket */
#define VLScg_RT_REQUESTED_ACTION_NOT_PERFORMED     84
/* Request structure does not specify one or more operation(s)
   corresponding to which information is found in revocation ticket */
#define VLScg_RT_NON_REQUESTED_ACTION_PERFORMED     85
/* Requested Operation is not successfully carried out at client
   side */
#define VLScg_RT_ACTION_STATUS_NOT_SUCCESS          86
/* Request struture does not specify any operation */
#define VLScg_RT_REQUEST_EMPTY                      87
/* Request structure specify an invalid operation */
#define VLScg_RT_REQUEST_LINE_INVALID               88
/* Revocation ticket is invalid */
#define VLScg_RT_REHOST_TICKET_INVALID_TLV_STRUCT   89
/* Rehost tag is not found in revocation ticket */
#define VLScg_RT_REHOST_TAG_MISSING                 90
/* version tag is not found in revocation ticket */
#define VLScg_RT_VERSION_TAG_MISSING                91
/* Transaction ID tag is not found in revocation ticket */
#define VLScg_RT_TRANSACTION_ID_TAG_MISSING         92
/* Lock selector tag is not found in revocation ticket */
#define VLScg_RT_LOCK_SELECTOR_TAG_MISSING          93
/* Lock selector value found in revocation ticket does not
   match with that specified in request structure */
#define VLScg_RT_LOCK_SELECTOR_MISMATCH             94
/* Lock Code tag is not found in revocation ticket */
#define VLScg_RT_LOCK_CODE_TAG_MISSING              95
/* Rehost line tag is not found in revocation ticket */
#define VLScg_RT_REHOST_LINE_TAG_MISSING            96
/* Hash tag is not found in revocation ticket */
#define VLScg_RT_HASH_TAG_MISSING                   97
/* Time stamp value found in revocation ticket does not
   match with that specified in request structure */
#define VLScg_RT_TIME_STAMP_MISMATCH                98
/* Time stamp tag is not found in revocation ticket */
#define VLScg_RT_TIME_STAMP_TAG_MISSING             99
/* Revocation ticket reported single error */
#define VLScg_RT_VERIFY_SINGLE_ERROR                100
/* Revocation ticket reported multiple errors */
#define VLScg_RT_VERIFY_MULTIPLE_ERRORS             101
/* Buffer too small */
#define VLScg_RT_BUFFER_TOO_SMALL                   102
/* Parameters error */
#define VLScg_RT_PARAMETERS_ERROR                   103
/* Memory allocation failure */
#define VLScg_RT_ALLOCATE_MEMORY_FAILURE            104
/* Operation type not supported */
#define VLScg_RT_UNSUPPORTED_OPERATION_TYPE         105
/* Invalid rehost request data */
#define VLScg_RT_INVALID_REQUEST_DATA               106
/* Tag can not be found in tlv */
#define VLScg_RT_TAG_NOT_FOUND                      107
/* Buffer too small */
#define VLScg_BUFFER_TOO_SMALL                      108
/* Insufficient actual licenes tokens to revoke */
#define VLScg_RT_INSUFFICIENT_TOKENS_TO_REVOKE      109
/* Mixed Operation (Network + Standalone) Found*/
#define VLScg_RT_MIXED_OPERATION_TYPE_UNSUPPORTED   110
/* License has infinite keys. Only Full revoke is allowed for this license */
#define VLScg_RT_INFINITE_LIC_FINITE_REQ            111
/* PT generation is not supported for redundant licenses */
#define VLScg_RT_RDNT_LIC_UNSUPPORED                112
/* PT string provided for verification is corrupted */
#define VLScg_RT_CORRUPT_ORIG_REQ                   113
/* Custom defined data tag is not found in revocation ticket */
#define VLScg_RT_CUSTOM_DATA_TAG_MISSING            114
/* Custom defined data value found in revocation ticket does not
   match with that specified in request structure / String */
#define VLScg_RT_CUSTOM_DATA_MISMATCH               115
/* Either standalone revoke request is provided to verify with network RT, or network revoke
   request is provided to verify with standalone RT */
#define VLScg_RT_REVOCATION_TYPE_MISMATCH           116
/* Request structure has more operations for single PT */
#define VLScg_TOO_MANY_OPERATIONS_FOR_SINGLE_PT     117
/* Different vendor ID license found in request */
#define VLScg_VENDOR_ID_MISMATCH                    118
/* Attribute not supported for this license version */
#define VLScg_CODGEN_VERSION_UNSUPPORTED            119
/* Deferred revoke days should be between 0 to 30 days. */
#define VLScg_DEFERRED_REVOKE_DAYS_INVALID          120
/* Old PT generation is unsupported in new API, old structs can't be passed to generate PT. */
#define VLScg_OLD_PT_GENERATION_UNSUPPORTED         121
/* The license string is invalid. Hence, it could not be used for PT generation. */
#define VLScg_INVALID_LICENSE                       122
/* The API for verification of redundant license does not support older PTs */
#define VLScg_PT_VERSION_UNSUPPORTED                123
/* The number of (valid/non-duplicate) RTs passed in the API is lesser than the number of lock codes inside the PT */
#define VLScg_MISSING_RTS                           124
/*The vendor info in the license generator binary is not valid*/
#define VLScg_INVALID_VENDOR_INFO                   125

#define VLScg_RESERV_CHAR_ERR               130
#define VLScg_LOWER_THAN_MIN_STRLEN         131

/*
 * RMS Development Kit Ethernet related error codes
 * VLS_ERROR_NO_MORE_ITEMS(211)
 * VLS_ERROR_FILE_NOT_FOUND(212)
 */
#define VLScg_ERROR_NO_MORE_ITEMS                   VLS_ERROR_NO_MORE_ITEMS
#define VLScg_ERROR_FILE_NOT_FOUND                  VLS_ERROR_FILE_NOT_FOUND


   /**********************************************************/
#define  VLScg_INVALID_HANDLE             ((VLScg_HANDLE) (-1))
#define  VLScg_MAX_CODE_COMP_LEN          512

#define  VLScg_MAX_VENDORBLOBLEN                  1024
#define  VLScg_MAX_VENDOR_BLOB_V2_LEN             4096
#define  VLScg_MAX_VENINFOLEN                     511
#define  VLScg_MAX_PRIVATE_VENINFOLEN_OLD         395
#define  VLScg_MAX_PUBLIC_VENINFOLEN              395
#define  VLScg_MAX_REDUNDANT_VENINFOLEN           395

#define  VLScg_MAX_PRIVATE_VENINFO_BUFFER_LEN     2116
#define  VLScg_MAX_PRIVATE_VENINFO_ALLOWED_LEN    2000
#define  VLScg_MAX_LICENSE_VENINFO_BUFFER_LEN     512
#define  VLScg_LICENSE_VENINFO_BUFFER_PAD_LEN     8

/**
  * From license version v18 onwards the current limit of minimum number of redundant servers is changed from 3 to 2.
  * This is applicable to license version v18 onwards only.
  */
#define  VLScg_MIN_NUM_SERVERS            2
#define  VLScg_MAX_NUM_SERVERS            11
#define  VLScg_MAX_SERVER_LOCK_INFO_LEN   16   /* chars */
#define  VLScg_MAX_NUM_NL_CLIENTS         7    /* Node locked clients */
#define  VLScg_MAX_NUM_ADDL_NL_CLIENTS    1000 /* Node locked clients */
#define  VLScg_MAX_NL_CLIENT_INFO_LEN     16   /* chars */
#define  VLScg_MAX_NUM_SUBNETS            7
#define  VLScg_MAX_NUM_ADDL_SUBNETS       1000 /* Subnets in server tables */
#define  VLScg_MAX_SUBNET_INFO_LEN        64   /* for base2 rep, in chars */
#define  VLScg_MAX_NUM_SECRETS            7    /* Challenge response salt */
#define  VLScg_MAX_SECRET_LEN             16   /* Challenge response salt */
#define  MAX_NUM_ADDITIVE_LICENSES        10
#define  VLScg_MAX_NUM_FEATURES           63   /* Maximum no of features allowed in multi key */


#define  VLScg_MAX_EID_LEN           46   /* Max no. of characters allowed in EID is 46 including NULL('\0' char) */
#define  VLScg_MAX_AID_LEN           9    /*  Max no. of characters allowed in AID is 9 including NULL('\0' char) */

#define VLScg_MAX_OLD_NUM_FEATURES        11
#define  VLScg_MIN_NUM_FEATURES           2   /* Minimum no of features allowed in multi key */

#define VLScgAllowRepositoryFlag           VLScgAllowPerpetualFlag /* rebranding of perpetual to repository frm 8.5.0 onwards */ 
   
   /* V13 onwards only */
typedef enum{
	disable_vm_detection,
	enable_vm_detection 
} vmDetectionFlagT;

/* 8.5.3 - Max limit for revoke grace days (deferred revoke) */
#define VLScg_MAX_REVOKE_GRACE_DAYS       30

/* 8.5.5 - To generate PT for redundant licenses */
#define MAX_REDUNDANT_SERVERS_IN_PT 11
   
   /**********************************************************/


   typedef struct
   {
      /* List of flags to be set by external callers: */

      int  code_type      ;  /* VLScg_SHORT_CODE/VLScg_LONG_CODE
                                  /VLScg_SHORT_NUMERIC_CODE   */
      int  additive       ;  /* VLScg_ADDITIVE / VLScg_EXCLUSIVE / VLScg_AGGREGATE_LICENSE */
      int  client_server_lock_mode;
      /* VLScg_BOTH_NODE_LOCKED/ VLScg_FLOATING/          */
      /* VLScg_DEMO_MODE/ VLScg_CLIENT_NODE_LOCKED        */
      int  holding_crit   ;  /* Criterion for held licenses                      */
      /* VLScg_HOLD_NONE/VLScg_HOLD_VENDOR/VLScg_HOLD_CODE*/
      int  sharing_crit   ;  /* Criterion for sharing of licenses                */
      /* VLScg_NO_SHARING/ VLScg_USER_SHARING/            */
      /* VLScg_HOSTNAME_SHARING/ VLScg_XDISPLAY_SHARING   */
      /* VLScg_VENDOR_SHARING                         */
      /* used as a team_criteria in case of capacity license */
      /* VLScg_NO_TEAM/ VLScg_USER_BASED_TEAM/            */
      /* VLScg_HOSTNAME_BASED_TEAM/ VLScg_XDISPLAY_BASED_TEAM   */
      /* VLScg_VENDOR_BASED_TEAM                         */

      int  server_locking_crit1[VLScg_MAX_NUM_SERVERS];
      /* Server lock selector/criterion (group 1)     */
      /* Allow 2 hostid's per server                  */
      int  server_locking_crit2[VLScg_MAX_NUM_SERVERS];
      /* Server lock selector/criterion (group 2)     */
      /* Allow 2 hostid's per server                  */
      int  client_locking_crit[VLScg_MAX_NUM_NL_CLIENTS];
      /* Client lock selector/criterion               */
      /* Allow 1 hostid per client                    */
      int  standalone_flag;  /* VLScg_NETWORK/VLScg_STANDALONE/VLScg_PERPETUAL */
      int  out_lic_type   ;  /* VLScg_ENCRYPTED/ VLScg_EXPANDED_READABLE/    */
      /* VLScg_CONCISE_READABLE                       */
      int  clock_tamper_flag;/* VLScg_NO_CHECK_TAMPER/ VLScg_CHECK_TAMPER    */

      /* List of data fields to be set by external callers: */
      char feature_name      [VLScg_MAX_CODE_COMP_LEN+1];
      char feature_version   [VLScg_MAX_CODE_COMP_LEN+1];
      int  birth_day      ;  /* 1 - max day of the month (28-31)             */
      int  birth_month    ;  /* 0 - 11 or JAN - DEC                          */
      int  birth_year     ;  /* 1998 to ...                                  */
      int  death_day      ;  /* 1 - max day of the month (28-31)             */
      int  death_month    ;  /* 0 - 11 or JAN - DEC                          */
      int  death_year     ;  /* 1998 to ...                                  */
      int  num_servers    ;  /* Always 1 for single server application       */
      char server_lock_info1 [VLScg_MAX_NUM_SERVERS]
      [VLScg_MAX_SERVER_LOCK_INFO_LEN+1];
      /* Stores information in ascii                  */
      /* +1 needed for null termination               */
      char server_lock_info2 [VLScg_MAX_NUM_SERVERS]
      [VLScg_MAX_SERVER_LOCK_INFO_LEN+1];
      /* Stores information in ascii                  */
      /* +1 needed for null termination               */
      int  num_nl_clients ;  /* Number of nodelocked clients                 */
      char nl_client_lock_info[VLScg_MAX_NUM_NL_CLIENTS]
      [VLScg_MAX_NL_CLIENT_INFO_LEN+1];
      /* Stores information in ascii                  */
      /* +1 needed for null termination               */
      unsigned num_keys[VLScg_MAX_NUM_FEATURES];
      /* Number of concurrent keys                    */
      unsigned soft_limit ;  /* 0 to num_keys                                */
      unsigned keys_per_node [VLScg_MAX_NUM_NL_CLIENTS];
      int  num_subnets    ;  /* 0 => site licensing disabled                 */
      char site_lic_info     [VLScg_MAX_NUM_SUBNETS][VLScg_MAX_SUBNET_INFO_LEN+1];
      /* Stores information in binary                 */
      /* +1 needed for null termination               */
      unsigned share_limit;  /*share_limit/team_limit - Number of users/clients */
      /* who can share a single license key */
      /* used as a team_limit in case of capacity license */
      int  key_life_units ;  /* Flag which determines lifetime least count   */
#ifdef _V_LP64_
      unsigned int key_lifetime;
#else
      unsigned long key_lifetime;
#endif
      /* absolute value in minutes                    */
      int  key_hold_units;  /* Flag which determines heldtime least count   */
#ifdef _V_LP64_
      unsigned int key_holdtime;
#else
      unsigned long key_holdtime;
#endif
      /* absolute value in minutes                    */
      int  num_secrets    ;  /* Number of Challenge response secrets         */
      char secrets           [VLScg_MAX_NUM_SECRETS][VLScg_MAX_SECRET_LEN+1];
      /* Stores information in ascii                  */
      /* +1 needed for null termination               */
      char vendor_info       [VLScg_MAX_PRIVATE_VENINFO_BUFFER_LEN+1];

      int   licType ;          /* Trial or Normal license type */
      int   trialDaysCount;    /* Life of trial license. */
      int   use_auth_code;     /* For multi-keys or short numeric codes */
      int   numeric_type;     /* For short numeric codes, 0 - non-numeric */
      /* 1 - general short numeric 2 - general numeric */
      /* 10 and above specific type */
#ifdef _V_LP64_
      int  conversion_time;
#else
      long conversion_time;
#endif
      int isRedundant;
      int majority_rule;

      int  isCommuter;
#ifdef _V_LP64_
      int   commuter_max_checkout_days;
#else
      long  commuter_max_checkout_days;
#endif
      /* Max days license can be checked out. 0=no limit */

      int log_encrypt_level;  /* For encryption level in the license code.*/
      int elan_key_flag;

      /* Fields for internal use, or unused */
      int  vendor_code    ;            /* Internal use                       */
      int  version_num    ;            /* Internal - Version number          */
      int  licensing_crit ;            /* Internal - VLScg_USER_BASED        */
#ifdef _V_LP64_
      unsigned int meter_value;
#else
      unsigned long  meter_value    ;  /* Internal                           */
#endif
      /*Fields for multi_key for short numeric codegen version >=2 */
      int num_features; /* number of features in case of multi key */
      int key_type;     /* Single key/Multi key for short numeric only */

      /* Fields for capacity Licensing */
      int           capacity_flag; /* VLScg_CAPACITY_NONE/VLScg_CAPACITY_NON_POOLED
                                        /VLCScg_CAPACITY_POOLED */
      int           capacity_units;  /* Flag which determines capacity least count   */
#ifdef _V_LP64_
      unsigned int capacity;   
#else
      unsigned long capacity; 
#endif
  /* The capacity of this license. In case of Pooled 
                                     Capacity license, this capacity is for the whole
                                     of "num_keys". In case of Non-Pooled Capacity
                                     license, this capacity is per token of hard
                                     limit. */

      int  grace_period_flag;  /* Must be VLScg_STANDARD_GRACE_PERIOD */
      int  grace_period_calendar_days;
      /* Max days license can be used in grace period  */
      int  grace_period_elapsed_hours;
      /* Max hours license can be used in grace period */

      int  overdraft_flag;   /* VLScg_NO_OVERDRAFT or VLScg_STANDARD_OVERDRAFT   */
      int  overdraft_hours;
      /* Max hours overdraft license can be used.         */
      int  overdraft_users;
      /* Simultaneous users allowed in overdraft          */
      int  overdraft_users_isPercent;
      /* VLS_TRUE = Users field is percent of hard limit  */

      int  local_request_lockcrit_flag;
      /* VLScg_LOCAL_REQUEST_LOCKCRIT_DEFINED = use the specified
         lockcrit fields below. Otherwise use defaults.
         These values are to be used by commuter license, perpetual
         licenses and grace period licenses.              */
      int  local_request_lockcrit_required;
      /* Required items for local request locking. */
      int  local_request_lockcrit_float;
      /* Floating items for local request locking. */
      int  local_request_lockcrit_min_num;
      /* Total number of items must for local request locking. */

      int  trial_elapsed_hours; /* Trial usage hours */
      int  trial_execution_count; /* Trial Execution Count */
      /* Public vendor info - to be included as plain text to license string */
      char plain_vendor_info[VLScg_MAX_PUBLIC_VENINFOLEN+1];
      vmDetectionFlagT vm_detection;	/* V13 onwards only*/
	  
#ifdef _V_LP64_
      int structSz;
#else
      long structSz;
#endif


      /* License Birth and Expiration will now be extended to include hours and minutes - v16 onwards */
      int birth_hours;    /* 0 - 23 */
      int birth_minutes;  /* 0 - 59 */
      int death_hours;    /* 0 - 23 */
      int death_minutes;  /* 0 - 59 */	       
      char eid[VLScg_MAX_EID_LEN];       /* Internal use only */
      int pid;                           /* Internal use only */
      int fid;                           /* Internal use only */
      char aid[VLScg_MAX_AID_LEN];       /* Internal use only */
      int cloud_usage_flag;              /* Internal use only */
      int lic_source;                    /* Internal use only */
      unsigned char vendor_secret_blob[VLScg_MAX_VENDORBLOBLEN];         /* Internal use only */
      Time64_T activation_birth_time;    /* Internal use only */
      Time64_T activation_expiry_time;   /* Internal use only */
      unsigned char vendor_secret_blob2[VLScg_MAX_VENDOR_BLOB_V2_LEN];   /* Internal use only */
      char license_vendor_info[VLScg_MAX_LICENSE_VENINFO_BUFFER_LEN + VLScg_LICENSE_VENINFO_BUFFER_PAD_LEN];/* License vendor info. v19 onwards. Stores information in ascii */      
      Time64_T license_generation_time;  /* Internal use only */
   }
   codeT;

   /* License Revocation Ticket Structure */
   typedef struct
   {
      unsigned long struct_size;
      unsigned long time_stamp;
      unsigned char feature_name[VLS_MAXFEALEN];
      unsigned char feature_version[VLS_MAXFEALEN];
      unsigned long capacity;
      unsigned int  base_license_hard_limit;
      unsigned int  number_licenses_revoked;
      unsigned int  total_number_licenses_revoked;
      unsigned long capacity_revoked;
      unsigned long server_locking_criteria;
      unsigned char server_locking_info[VLS_MAXSRVLOCKLEN];
   }
   VLSrevocationTicketInfoT;

   /* License revocation defines -  */

   /* This should always be greater than sizeof(VLSrevocationTicketInfoT) */
#define VLScg_MAX_LICENSE_REVOCATION_TICKET_SIZE       VLS_MAX_LICENSE_REVOCATION_TICKET_SIZE

   /* Following represents the License Revocation Ticket secret key length */
#define VLScg_LICENSE_REVOCATION_TICKET_SECRET_LENGTH  16


   /************************* codeT member values ********************/

   /* code gen version */
#define VLScg_CODEGEN_VERSION_7           7
#define VLScg_CODEGEN_VERSION_8           8
#define VLScg_CODEGEN_VERSION_9           9
#define VLScg_CODEGEN_VERSION_10          10
#define VLScg_CODEGEN_VERSION_11          11
#define VLScg_CODEGEN_VERSION_12          12
#define VLScg_CODEGEN_VERSION_13          13
#define VLScg_CODEGEN_VERSION_14          14
#define VLScg_CODEGEN_VERSION_15          15
#define VLScg_CODEGEN_VERSION_16          16
#define VLScg_CODEGEN_VERSION_17          17
#define VLScg_CODEGEN_VERSION_18          18
#define VLScg_CODEGEN_VERSION_19          19
#define VLScg_CODEGEN_VERSION_20          20
#define VLScg_CODEGEN_VERSION_21          21

#define VLScg_CODEGEN_VERSION_7_STRING  "7"
#define VLScg_CODEGEN_VERSION_8_STRING  "8"
#define VLScg_CODEGEN_VERSION_9_STRING  "9"
#define VLScg_CODEGEN_VERSION_10_STRING "10"
#define VLScg_CODEGEN_VERSION_11_STRING "11"
#define VLScg_CODEGEN_VERSION_12_STRING "12"
#define VLScg_CODEGEN_VERSION_13_STRING "13"
#define VLScg_CODEGEN_VERSION_14_STRING "14"
#define VLScg_CODEGEN_VERSION_15_STRING "15"
#define VLScg_CODEGEN_VERSION_16_STRING "16"
#define VLScg_CODEGEN_VERSION_17_STRING "17"
#define VLScg_CODEGEN_VERSION_18_STRING "18"
#define VLScg_CODEGEN_VERSION_19_STRING "19"
#define VLScg_CODEGEN_VERSION_20_STRING "20"
#define VLScg_CODEGEN_VERSION_21_STRING "21"

   /* Grace period */
#define VLScg_NO_GRACE_PERIOD               VLS_NO_GRACE_PERIOD
#define VLScg_STANDARD_GRACE_PERIOD         VLS_STANDARD_GRACE_PERIOD
   /* String versions for VLScgSet functions. */
#define VLScg_NO_GRACE_PERIOD_STRING        "0"
#define VLScg_STANDARD_GRACE_PERIOD_STRING  "1"
   /* Maximum values */
#define VLScg_MAX_GRACE_PERIOD_DAYS_LIMIT    180
#define VLScg_MAX_GRACE_PERIOD_HOURS_LIMIT   1440

   /* int  code_capacity : */
#define VLScg_CAPACITY_NONE                 VLS_CAPACITY_NONE
#define VLScg_CAPACITY_NON_POOLED           VLS_CAPACITY_NON_POOLED
#define VLScg_CAPACITY_POOLED               VLS_CAPACITY_POOLED
   /* String versions for VLScgSet functions. */
#define VLScg_CAPACITY_NONE_STRING         "0"
#define VLScg_CAPACITY_NON_POOLED_STRING   "1"
#define VLScg_CAPACITY_POOLED_STRING       "2"

#define VLScg_CAPACITY_UNITS_MIN_VALUE      0
#define VLScg_CAPACITY_UNITS_MAX_VALUE      4

   /* int  code_type : */
#define VLScg_SHORT_CODE           0
#define VLScg_LONG_CODE            1
#define VLScg_SHORT_NUMERIC_CODE   2
   /* String versions for VLScgSet functions. */
#define VLScg_SHORT_CODE_STRING   "0"
#define VLScg_LONG_CODE_STRING    "1"
#define VLScg_SHORT_NUMERIC_CODE_STRING   "2"

   /* int  licensing_crit : */
#define VLScg_USER_BASED           0
#define VLScg_METERED              1
   /* String versions for VLScgSet functions. */
#define VLScg_USER_BASED_STRING   "0"
#define VLScg_METERED_STRING      "1"

   /* int  additive : */
#define VLScg_ADDITIVE             0
#define VLScg_EXCLUSIVE            1
#define VLScg_AGGREGATE_LICENSE    2
   /* String versions for VLScgSet functions. */
#define VLScg_ADDITIVE_STRING            "0"
#define VLScg_EXCLUSIVE_STRING           "1"
#define VLScg_AGGREGATE_LICENSE_STRING   "2"

   /* key_type */
#define VLScg_SINGLE_KEY              0
#define VLScg_MULTI_KEY               1
   /* String versions for VLScgSet functions. */
#define VLScg_SINGLE_KEY_STRING   "0"
#define VLScg_MULTI_KEY_STRING    "1"

   /* int isRedundant : */
#define VLScg_NON_REDUNDANT_CODE       0
#define VLScg_REDUNDANT_CODE           1
   /* String versions for VLScgSet functions. */
#define VLScg_NON_REDUNDANT_CODE_STRING  "0"
#define VLScg_REDUNDANT_CODE_STRING      "1"


   /* int majority_rule : */
#define VLScg_MAJORITY_RULE_NOT_FOLLOWS     0
#define VLScg_MAJORITY_RULE_FOLLOWS         1
   /* String versions for VLScgSet functions. */
#define VLScg_MAJORITY_RULE_NOT_FOLLOWS_STRING  "0"
#define VLScg_MAJORITY_RULE_FOLLOWS_STRING      "1"

   /* int log_encrypt_level  */
#define VLScg_NO_ENCRYPTION             0
#define VLScg_MAX_ENCRYPTION_LEVEL      4

   /* int isCommuter : */
#define VLScg_NOT_ISSUE_COMMUTER_CODES   0
#define VLScg_ISSUE_COMMUTER_CODES       1
   /* String versions for VLScgSet functions. */
#define VLScg_NOT_ISSUE_COMMUTER_CODES_STRING   "0"
#define VLScg_ISSUE_COMMUTER_CODES_STRING       "1"
   /* Local request locking criteria flag */
#define VLScg_LOCAL_REQUEST_LOCKCRIT_USEDEFAULT   VLS_LOCAL_REQUEST_LOCKCRIT_USEDEFAULT
#define VLScg_LOCAL_REQUEST_LOCKCRIT_DEFINED      VLS_LOCAL_REQUEST_LOCKCRIT_DEFINED
   /* String versions for VLScgSet functions */
#define VLScg_LOCAL_REQUEST_LOCKCRIT_USEDEFAULT_STRING  "0"
#define VLScg_LOCAL_REQUEST_LOCKCRIT_DEFINED_STRING     "1"
   /* Maximum values */
#define VLScg_MAX_COMMUTERDAYS_SHORTCODE_LIMIT 60
#define VLScg_MAX_COMMUTERDAYS_LONGCODE_LIMIT  1827 /* 5 years */
#define VLScg_COMMUTERDAYS_UNRESTRICTED        VLS_COMMUTERDAYS_UNRESTRICTED
#define VLScg_COMMUTERDAYS_UNRESTRICTED_STRING "NO_RESTRICT"

   /* int  client_server_lock_mode : */
#define VLScg_FLOATING                    0
#define VLScg_BOTH_NODE_LOCKED            1
#define VLScg_DEMO_MODE                   2
#define VLScg_CLIENT_NODE_LOCKED          3  /* server not locked */
   /* String versions for VLScgSet functions. */
#define VLScg_FLOATING_STRING            "0"
#define VLScg_BOTH_NODE_LOCKED_STRING    "1"
#define VLScg_DEMO_MODE_STRING           "2"
#define VLScg_CLIENT_NODE_LOCKED_STRING  "3"

   /* int  team_crit : */
#define VLScg_NO_TEAM                       VLScg_NO_SHARING
#define VLScg_USER_BASED_TEAM               VLScg_USER_SHARING
#define VLScg_HOSTNAME_BASED_TEAM           VLScg_HOSTNAME_SHARING
#define VLScg_XDISPLAY_BASED_TEAM           VLScg_XDISPLAY_SHARING
#define VLScg_VENDOR_BASED_TEAM             VLScg_VENDOR_SHARING
   /* String versions for VLScgSet functions. */
#define VLScg_NO_TEAM_STRING                VLScg_NO_SHARING_STRING
#define VLScg_USER_BASED_TEAM_STRING        VLScg_USER_SHARING_STRING
#define VLScg_HOSTNAME_BASED_TEAM_STRING    VLScg_HOSTNAME_SHARING_STRING
#define VLScg_XDISPLAY_BASED_TEAM_STRING    VLScg_XDISPLAY_SHARING_STRING
#define VLScg_VENDOR_BASED_TEAM_STRING      VLScg_VENDOR_SHARING_STRING


   /* int  sharing_crit : */
#define VLScg_NO_SHARING                 VLS_NO_SHARING
#define VLScg_USER_SHARING               VLS_USER_NAME_ID
#define VLScg_HOSTNAME_SHARING           VLS_CLIENT_HOST_NAME_ID
#define VLScg_XDISPLAY_SHARING           VLS_X_DISPLAY_NAME_ID
#define VLScg_VENDOR_SHARING             VLS_VENDOR_SHARED_ID
   /* String versions for VLScgSet functions. */
#define VLScg_NO_SHARING_STRING          VLS_NO_SHARING_STRING
#define VLScg_USER_SHARING_STRING        VLS_USER_NAME_ID_STRING
#define VLScg_HOSTNAME_SHARING_STRING    VLS_CLIENT_HOST_NAME_ID_STRING
#define VLScg_XDISPLAY_SHARING_STRING    VLS_X_DISPLAY_NAME_ID_STRING
#define VLScg_VENDOR_SHARING_STRING      VLS_VENDOR_SHARED_ID_STRING

   /* Test whether a particular locking criterion is being used. */
#define VLScg_LOCK_TO_ID_PROM        VLS_LOCK_TO_ID_PROM
#define VLScg_LOCK_TO_IP_ADDR        VLS_LOCK_TO_IP_ADDR
#define VLScg_LOCK_TO_DISK_ID        VLS_LOCK_TO_DISK_ID
#define VLScg_LOCK_TO_HOSTNAME       VLS_LOCK_TO_HOSTNAME
#define VLScg_LOCK_TO_ETHERNET       VLS_LOCK_TO_ETHERNET
#define VLScg_LOCK_TO_NW_IPX         VLS_LOCK_TO_NW_IPX
#define VLScg_LOCK_TO_HARD_DISK_SERIAL    VLS_LOCK_TO_HARD_DISK_SERIAL
#define VLScg_LOCK_TO_NW_SERIAL      VLS_LOCK_TO_NW_SERIAL
#define VLScg_LOCK_TO_PORTABLE_SERV  VLS_LOCK_TO_PORTABLE_SERV
#define VLScg_LOCK_TO_CUSTOM         VLS_LOCK_TO_CUSTOM
#define VLScg_LOCK_TO_CPU            VLS_LOCK_TO_CPU
#define VLScg_LOCK_TO_CUSTOMEX       VLS_LOCK_TO_CUSTOMEX
#define VLScg_LOCK_TO_CPU_INFO       VLS_LOCK_TO_CPU_INFO
#define VLScg_LOCK_TO_UUID           VLS_LOCK_TO_UUID

   /* int  locking_crit (bit flags) : */
#define VLScg_LOCK_ID_PROM        VLS_LOCK_ID_PROM
#define VLScg_LOCK_IP_ADDR        VLS_LOCK_IP_ADDR
#define VLScg_LOCK_DISK_ID        VLS_LOCK_DISK_ID
#define VLScg_LOCK_HOSTNAME       VLS_LOCK_HOSTNAME
#define VLScg_LOCK_ETHERNET       VLS_LOCK_ETHERNET
#define VLScg_LOCK_NW_IPX         VLS_LOCK_NW_IPX
#define VLScg_LOCK_NW_SERIAL      VLS_LOCK_NW_SERIAL
#define VLScg_LOCK_PORTABLE_SERV  VLS_LOCK_PORTABLE_SERV
#define VLScg_LOCK_CUSTOM         VLS_LOCK_CUSTOM
#define VLScg_LOCK_CPU            VLS_LOCK_CPU
#define VLScg_LOCK_CUSTOMEX       VLS_LOCK_CUSTOMEX
#define VLScg_LOCK_HARD_DISK_SERIAL    VLS_LOCK_HARD_DISK_SERIAL
#define VLScg_LOCK_CPU_INFO       VLS_LOCK_CPU_INFO
#define VLScg_LOCK_UUID           VLS_LOCK_UUID

   /* Highest bit currently in use : */
#define VLScg_LOCK_HIGHEST_BIT    VLS_LOCK_HIGHEST_BIT /* Starting from 1... */
   /* Mask with all locking criteria set. */
#define VLScg_LOCK_ALL            VLS_LOCK_ALL
   /* The maximum size of lock code */
#define VLScg_LOCK_CODE_SIZE      VLS_LOCK_CODE_SIZE

   /* Buffer size needed for converting VLSmachineID to string. Note that 1 byte
    * will be reflected as 2 chars. For example: 255(1 byte) --> "FF"(2 bytes)
    *
    * 7*sizeof(unsigned long) - VLSmachineID includes 7 unsigned long type datas
    * sizeof(VLScustomEx) - VLSmachineID includes VLScustomEx type data
    *
    * The size should be enough for every case because the buffer of VLScustomEx.len isn't
    * used at all, thus don't worry about string end flag - NULL.
    *
    */
#define VLScg_MACHINEID_STRING_SIZE    VLS_MACHINEID_STRING_SIZE
#define VLScg_HASHED_MACHINEID_STRING_SIZE    VLS_HASHED_MACHINEID_STRING_SIZE

   /* int standalone_flag : */
#define VLScg_NETWORK              VLS_NETWORK_MODE
#define VLScg_STANDALONE           VLS_STANDALONE_MODE
#define VLScg_PERPETUAL            VLS_PERPETUAL_MODE
#define VLScg_REPOSITORY           VLScg_PERPETUAL
   /* String versions for VLScgSet functions. */
#define VLScg_NETWORK_STRING      "0"
#define VLScg_STANDALONE_STRING   "1"
#define VLScg_PERPETUAL_STRING    "2"
#define VLScg_REPOSITORY_STRING   VLScg_PERPETUAL_STRING

   /* int out_lic_type : */
#define VLScg_ENCRYPTED                   0
#define VLScg_EXPANDED_READABLE           1
#define VLScg_CONCISE_READABLE            2
   /* String versions for VLScgSet functions. */
#define VLScg_ENCRYPTED_STRING           "0"
#define VLScg_EXPANDED_READABLE_STRING   "1"
#define VLScg_CONCISE_READABLE_STRING    "2"

   /* int clock_tamper_flag  */
#define VLScg_NO_CHECK_TAMPER             0
#define VLScg_CHECK_TAMPER                1
   /* String versions for VLScgSet functions. */
#define VLScg_NO_CHECK_TAMPER_STRING     "0"
#define VLScg_CHECK_TAMPER_STRING        "1"

#define  VLScg_LIC_SOURCE_CLOUD      1    /* For internal use ONLY */
#define  VLScg_LIC_SOURCE_CLOUD_STR  "1"  /* For internal use ONLY */

#define  VLScg_CLOUD_USAGE_ON        1    /* For internal use ONLY */
#define  VLScg_CLOUD_USAGE_ON_STR    "1"  /* For internal use ONLY */


   /* int  holding_crit (criterion for a held license): */
#define VLScg_HOLD_NONE     VLS_HOLD_NONE   /* Held licenses not allowed */
#define VLScg_HOLD_VENDOR   VLS_HOLD_VENDOR /* Client specifies hold time */
#define VLScg_HOLD_CODE     VLS_HOLD_CODE   /* Lic code specifies hold time */
   /* String versions for VLScgSet functions. */
#define VLScg_HOLD_NONE_STRING       VLS_HOLD_NONE_STRING
#define VLScg_HOLD_VENDOR_STRING     VLS_HOLD_VENDOR_STRING
#define VLScg_HOLD_CODE_STRING       VLS_HOLD_CODE_STRING

   /* unsigned num_keys, soft_limit */
#define VLScg_INFINITE_KEYS          VLS_INFINITE_KEYS     /* Applicable for license version <= v19 */
#define VLScg_KEY_MAX_LIMIT          VLS_KEY_MAX_LIMIT     /* Applicable for license version >= v20 */
   /* String versions for VLScgSet functions. */
#define VLScg_INFINITE_KEYS_STRING   VLS_INFINITE_KEYS_STRING
#define VLScg_INFINITE_CAPACITY      VLS_INFINITE_CAPACITY

   /*  int licType */
#define VLScg_TRIAL_LIC     VLS_TRIAL_LIC
#define VLScg_NORMAL_LIC    VLS_NORMAL_LIC
   /* String versions for VLScgSet functions. */
#define VLScg_TRIAL_LIC_STRING  "1"
#define VLScg_NORMAL_LIC_STRING "0"

   /*  int numeric_type */
#define VLScg_NUMERIC_UNKNOWN    0
#define VLScg_NOT_NUMERIC        1
#define VLScg_MISC_SHORT_NUMERIC 2
#define VLScg_MISC_NUMERIC       3

#define VLScg_REHOST_OPERATION_ADD              'A'
#define VLScg_REHOST_OPERATION_REVOKE_FULL      'R'
#define VLScg_REHOST_OPERATION_REVOKE_PARTIAL   'P'      /* Applicable for Network Revoke only */

/* Short numeric license type definition used by VLScgSetSNCodeType API */
#define VLScg_30D_STANDALONE_DEMO 1
#define VLScg_30D_NETWORK_DEMO    2
#define VLScg_ABS_STANDALONE_DEMO 3
#define VLScg_ABS_NETWORK_DEMO    4
#define VLScg_LCK_STANDALONE_DEMO 5
#define VLScg_BUY_STANDALONE_PROD 6
#define VLScg_BUY_NETWORK_PROD    7
#define VLScg_LCK_NETWORK_DEMO    8

/* Short numeric license expiration information */
#define VLSCG_EXPIRED_AFTER_FIRST_QUARTER    1
#define VLSCG_EXPIRED_AFTER_SECOND_QUARTER   2
#define VLSCG_EXPIRED_AFTER_THIRD_QUARTER    3
#define VLSCG_EXPIRED_AFTER_FOURTH_QUARTER   4

/* Short numeric license type definition used by short numeric codeT struct */
   typedef enum shortLicenseType {
      sa_eval_lockdisk=11, /* 0,1,2 reserved above for unknown, non-numeric or */
      /* non-specific values */
      sa_abstime_anyhost,
      sa_reltime_anyhost,
      sa_abstime_locked,
      sa_abstime_lockdisk,     /* not used */
      sa_noexp_lockdiskdongle, /* not used */
      sa_noexp_locked,
      net_reltime_anyhost,
      net_abstime_anyhost,
      net_noexp_lockdisk_dongle_prom, /* not used */
      net_noexp_locked,
      net_abstime_locked,
   } shortLicenseType;
   


   /* int trialDaysCount */
#define VLScg_MAX_TRIALDAYS_SHORTCODE_LIMIT   730
#define VLScg_MAX_TRIALDAYS_LONGCODE_LIMIT    1461
#define VLScg_MAX_TRIALDAYS_SHRTNUMCODE_LIMIT 1461

   /* Trial Elapsed Hours range */
#define VLScg_MAX_TRIALHOURS_LONGCODE_LIMIT   35064
#define VLScg_MIN_TRIALHOURS_LONGCODE_LIMIT   1

   /* Trial String Values for VLScgSet functions */
#define VLScg_TRIAL_DAYSCNT_DISABLED_STR        "DISABLED"
#define VLScg_TRIAL_ELAPSEDHOURS_DISABLED_STR   "DISABLED"

#define VLScg_TRIALDAYS_USEDEFAULT_STRING        "30"
#define VLScg_TRIALHOURS_USEDEFAULT_STRING       VLScg_TRIAL_ELAPSEDHOURS_DISABLED_STR

   /******** Special strings accepted by certain VLScgSet functions: ********/
#define VLScg_NOLIMIT_STRING    "NOLIMIT"
#define VLScg_NEVER_STRING      "NEVER"
#define VLScg_NO_STRING      "NO"
   /* int vm detection flag  */
#define VLScg_VM_ALLOWED             0
#define VLScg_VM_DISALLOWED                1
   /* String versions for VLScgSet functions. */
#define VLScg_VM_ALLOWED_STRING     "0"
#define VLScg_VM_DISALLOWED_STRING        "1"

   /******** Special value stored in codeT for infinite year: ********/
   /*
    * Used in members birth_year, death_year.  Never set this value directly
    * into a codeT struct.   You can check against this value while decoding
    * a license string.
    */
#define VLScg_INFINITE_YEARS     2500
#undef _LSCGEN_UNIX_

   /********* Macros for allow and set APIs for Team creation *********/
#define VLScgAllowTeamCriteria(iHandle, codeP)               VLScgAllowSharedLic(iHandle, codeP)
#define VLScgSetTeamCriteria(iHandle, codeP, flag)           VLScgSetSharedLicType(iHandle, codeP, flag)
#define VLScgAllowTeamLimit(iHandle, codeP)                  VLScgAllowShareLimit(ihandle, codeP)
#define VLScgSetTeamLimit(iHandle, codeP, decimalNum)        VLScgSetShareLimit(iHandle, codeP, decimalNum)

#define _LSCGEN_UNIX_
   /******************************************************************
    * DESCRIPTION :
    * Initialize the handle, which may be useful in multithreaded
    * systems. Initialize the decrypt keys also.
    *
    * RETURN VALUES :
    * returns 0 on successful return.
    * returns VLScg_MAX_LIMIT_CROSSED on failure.
    *
    * On failure returns, the handle returned MAY be valid and contain error
    * messages. Caller can check if the handle is (equal to)
    * VLScg_INVALID_HANDLE, and if not, can use the VLScgXXXError() routines
    * to aquire the error messages from the handle. In this case the caller
    * should call VLScgCleanup() to free the resources associated with the handle.
    *
    * NOTES :
    */

   int VLScgInitialize
   (
#ifndef LSNOPROTO
      VLScg_HANDLE * iHandleP   /* Instance handle for this library
                                 provides access to the internal data
                                 structure */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    * Destroys created handle.
    *
    * RETURN VALUES :
    * returns 0 on successful return.
    *
    * NOTES :
    */

   int VLScgCleanup
   (
#ifndef LSNOPROTO
      VLScg_HANDLE *iHandleP
#endif
   );
#undef _LSCGEN_UNIX_

   /******************************************************************
    * DESCRIPTION :
    * Resets the codeP. It must be called before calling VLScgSet
    * functions.
    *
    * RETURN VALUES :
    * returns 0 on successful return.
    *
    * NOTES :
    */

   int VLScgReset
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,
      codeT *codeP
#endif
   );


#define _LSCGEN_UNIX_

   /****** These functions can be used to retrieve or print errors: ******/



   /******************************************************************
    * DESCRIPTION :
    * This function retrieves number of messages recorded in the handle.
    *
    * RETURN VALUES :
    * returns VLScg_NO_RESOURCES if no resources available.
    * returns VLScg_FAIL on failure.
    * returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgGetNumErrors
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,    /* IN  */
      int    *numMsgsP         /* OUT */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    * This function retrieves the length of msg # msgNum recorded in the
    * handle. It includes the space required for NULL termination.
    *
    * RETURN VALUES :
    * returns VLScg_NO_RESOURCES if no resources available.
    * returns VLScg_FAIL on failure.
    * returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgGetErrorLength
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle, /* IN  */
      int     msgNum,       /* IN  - starts from 0 */
      int    *errLenP       /* OUT */
#endif
   );



   /*******************************************************************
    * DESCRIPTION :
    * This function retrieves earliest error from handle, upto bufLen
    *  chars
    * and bufLen must be the length of the pre allocated buffer msgBuf.
    * Msg returned will always be NULL terminated.
    *
    * RETURN VALUES :
    * returns VLScg_NO_RESOURCES if no resources available.
    * returns VLScg_FAIL on failure.
    * returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgGetErrorMessage
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,   /* IN  */
      char   *msgBuf,         /* INOUT */
      int     bufLen          /* IN */
#endif
   );



   /******************************************************************
    * DESCRIPTION :
    * This function spills the error struct to the file given.
    *
    * RETURN VALUES :
    * returns VLScg_NO_RESOURCES if no resources available.
    * returns VLScg_FAIL on failure.
    * returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgPrintError
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,    /* IN  */
      FILE  *file              /* INOUT */
#endif
   );

   int VLScgPrintErrorExt
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,    /* IN  */
      char  *fileName          /* INOUT */
#endif
   );

#undef _LSCGEN_UNIX_


   /*****************************************************************
    * Begin functions that set fields of the code struct:
    * The struct contains info independent of short/long codes.
    * These functions return 0 on success.
    */

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->version_num to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds MAX_CODEGEN_VERSION.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetCodegenVersion
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle, /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag
#endif
   );



   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->additive to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds VLScg_AGGREGATE_LICENSE.   
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *   VLScg_EXCLUSIVE.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetAdditive
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - The value of flag indicates whether the
                         license to be generated is additive/exclusive.The
                         legal values are 0/1 for additive/exclusive
                         respectively     */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->isRedundant to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *                                          VLScg_REDUNDANT_CODE.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *                                           VLScg_NON_REDUNDANT_CODE.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetRedundantFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - Valid values are :
                  VLScg_REDUNDANT_CODE     - redundant license
                  VLScg_NON_REDUNDANT_CODE - non redundant license
                  */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->majority_rule to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *                                          VLScg_MAJORITY_RULE_FOLLOWS.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *                                           VLScg_MAJORITY_RULE_NOT_FOLLOWS.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetMajorityRuleFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - Valid values are :
                  VLScg_MAJORITY_RULE_FOLLOWS     -Set majority_rule_flag
                  VLScg_MAJORITY_RULE_NOT_FOLLOWS -Unset majority_rule_flag
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->key_type to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *                                               VLScg_MULTI_KEY.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *                                               VLScg_SINGLE_KEY.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetKeyType
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - Valid values are :
                             VLScg_SINGLE_KEY
                             VLScg_MULTI_KEY
                           */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->num_features to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *                                          VLScg_MAX_NUM_FEATURES.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *                                          VLScg_MIN_NUM_FEATURES.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetNumFeatures
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN  */

#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->num_servers to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *                                          VLScg_MAX_NUM_SERVERS.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *                                          VLScg_MIN_NUM_SERVERS.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetNumServers
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,
      codeT *codeP,
      char  *str  /* Number of servers: should be from 0 to VLScg_MAX_NUM_SERVERS */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->log_encrypt_level to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *                                           VLScg_MAX_ENCRYPTION_LEVEL.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *                                           VLScg_NO_ENCRYPTION.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLogEncryptLevel
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - Max allowed value is
                             VLScg_MAX_ENCRYPTION_LEVEL
                          */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->isCommuter to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *                                           VLScg_ISSUE_COMMUTER_CODES.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *                                           VLScg_NOT_ISSUE_COMMUTER_CODES.

    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetCommuterLicense
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - Valid values are :
                   VLScg_ISSUE_COMMUTER_CODES_STRING
                   VLScg_NOT_ISSUE_COMMUTER_CODES_STRING
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->commuter_days to the passed value.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds the maximum
    *                                                     allowed value.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum
    *                                                     allowed value.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetCommuterMaxCheckoutDays
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle, /* IN */
      codeT *      codeP,   /* INOUT - the license code structure */
      char  *      daysStr  /* IN */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->local_request_lockcrit_flag to
    *   the passed value.
    *   VLScg_LOCAL_REQUEST_LOCKCRIT_USEDEFAULT = use default locking
    *       criteria.
    *   VLScg_LOCAL_REQUEST_LOCKCRIT_DEFINED = use specified criteria
    *       as set in VLScgSetLocalRequestLockCrit API.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds the maximum
    *                                                     allowed value.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum
    *                                                     allowed value.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLocalRequestLockCritFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle, /* IN */
      codeT *      codeP,   /* INOUT - the license code structure */
      char  *      str      /* IN */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->local_request_lockcrit_required,
    *   local_request_lockcrit_float, and local_request_lockcrit_min_num  to
    *   the passed value.
    *   Three inputs are expected to be seperated by colons.
    *   Example:  0x4:0x3FF:2
    *             0x4 required locking criteria.
    *             0x3FF floating locking criteria.
    *             2 total locking criteria must be found.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds the maximum
    *                                                     allowed value.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum
    *                                                     allowed value.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLocalRequestLockCrit
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle, /* IN */
      codeT *      codeP,   /* INOUT - the license code structure */
      char  *      str      /* IN */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->code_type to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds VLScg_LONG_CODE .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *    VLScg_SHORT_CODE.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetCodeLength
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - The value of flag is used to set the
                  code_type member of codeT struct.  Legal values are :
                  VLScg_SHORT_CODE_STRING
                  VLScg_LONG_CODE_STRING
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->capacity_flag to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds VLScg_CAPACITY_POOLED.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *     VLScg_CAPACITY_NONE.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetCapacityFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - The value of flag is used to set the
                  capacity_flag of codeT struct.  Legal values are :
                  VLScg_CAPACITY_NONE_STRING
                  VLScg_CAPACITY_NON_POOLED_STRING
                  VLScg_CAPACITY_POOLED_STRING
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->capacity to the value of decimalNum.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetCapacity
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *  codeP,     /* INOUT - the license code structure */
      char  *  decimalNum /* IN - Controls the Capacity.
                 Use a numeric decimal value.
                 */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->standalone_flag to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds VLScg_STANDALONE.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *     VLScg_NETWORK.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetStandAloneFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - The value of flag is used to set the
                  standalone_flag of codeT struct.  Legal values are :
                  VLScg_NETWORK_STRING
                  VLScg_STANDALONE_STRING
                  VLScg_PERPETUAL_STRING
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->code_type to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds VLScg_TRIAL_LIC .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *     VLScg_NORMAL_LIC.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetLicenseType
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *      codeP,  /* INOUT - the license code structure */
      char  *      flag    /* IN - The flag is used to set the code_type
                                member of codeT struct and the values it can take
                                are:
                                VLScg_NORMAL_LIC_STRING - non-trial license
                                VLScg_TRIAL_LIC_STRING  - trial license
                              */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->holding_crit to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds VLScg_HOLD_CODE .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *     VLScg_HOLD_NONE.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetHoldingCrit
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - The flag is used to set the holding_crit
                  member of codeT struct and the values it can take are :
                  VLScg_HOLD_NONE_STRING   -  Held licenses not allowed
                  VLScg_HOLD_VENDOR_STRING -  Client API specifies hold time
                  VLScg_HOLD_CODE_STRING   -  License code specifies hold time
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *  Sets the value codeP->client_server_lock_mode to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetClientServerLockMode
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - The value of flag is used to set the
                  client_server_lock_mode member.  The values it may take are:
                  VLScg_FLOATING_STRING           - Server is locked
                  VLScg_BOTH_NODE_LOCKED_STRING   - Clients and server are locked
                  VLScg_DEMO_MODE_STRING          - Demo license (no locking)
                  VLScg_CLIENT_NODE_LOCKED_STRING - Only clients are locked
                  */
#endif
   );


   /******************************************************************
    * FUNCTION NAME: VLScgSetSharedLicType()/VLScgSetTeamCriteria()
    * DESCRIPTION :
    *   Sets the value codeP->sharing_crit to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *     VLScg_VENDOR_SHARING .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than
    *     VLScg_NO_SHARING.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetSharedLicType
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle, /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - This flag enables shared licenses and
                  specifies the sharing criteria.  Legal values are :
                  VLScg_NO_SHARING_STRING
                  VLScg_USER_SHARING_STRING
                  VLScg_HOSTNAME_SHARING_STRING
                  VLScg_XDISPLAY_SHARING_STRING
                  VLScg_VENDOR_SHARING_STRING   - Vendor-defined/customized.
                  Need to customize the client library for this.
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Controls action on detection of clock being set back on the machine.
    *   Sets the value codeP->clock_tamper_flag to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_INVALID_RANGE if value is not in the range allowed.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetClockTamperFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - Valid values are :
                  VLScg_NO_CHECK_TAMPER_STRING - Do not check clock tamper
                  VLScg_CHECK_TAMPER_STRING - Check clock tamper
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Controls the type of license string generated.
    *   Sets the value codeP->out_lic_type to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_INVALID_RANGE if value is not in the range allowed.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetOutLicType
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - Valid values are :
                  VLScg_ENCRYPTED_STRING
                  VLScg_EXPANDED_READABLE_STRING
                  VLScg_CONCISE_READABLE_STRING
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the server fingerprint criterion (group 1).
    *   A server's fingerprint (hostid) could be computed from a number
    *   of fingerprinting elements, such as ID PROM, ethernet address,
    *   etc.  In criterion, there is one bit reserved for each type of
    *   fingerprinting element, as indicated in the VLScg_LOCK_ defines.
    *   criterion can be any bit-OR combination of the types of fingerprinting
    *   elements.
    *   A server can be locked to either of 2 groups of fingerprints.  The 2nd
    *   group will be tried if the first licensed fingerprint group fails to
    *   match the server's fingerprint, at the end-user site.
    *   This function sets the criterion for the 1st group.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if value is too big.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_INVALID_HEX_TYPE if value is not in hexadecimal format.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetServerLockMechanism1
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,     /* INOUT - the license code structure */
      char  *   criterion, /* IN - The value to be set - should be in hex */
      int       server     /* IN - Number of server, should be 0 to VLScg_MAX_NUM_SERVERS-1 */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the server fingerprint criterion (group 2).
    *   A server's fingerprint (hostid) could be computed from a number
    *   of fingerprinting elements, such as ID PROM, ethernet address,
    *   etc.  In criterion, there is one bit reserved for each type of
    *   fingerprinting element, as indicated in the VLScg_LOCK_ defines.
    *   criterion can be any bit-OR combination of the types of fingerprinting
    *   elements.
    *   A server can be locked to either of 2 groups of fingerprints.  The 2nd
    *   group will be tried if the first licensed fingerprint group fails to
    *   match the server's fingerprint, at the end-user site.
    *   This function sets the criterion for the 2nd group.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if value is too big.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_INVALID_HEX_TYPE if value is not in hexadecimal format.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetServerLockMechanism2
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,     /* INOUT - the license code structure */
      char  *   criterion, /* IN - The value to be set - should be in hex */
      int       server     /* IN - Number of server, should be 0 to VLScg_MAX_NUM_SERVERS-1 */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets a client's fingerprint criterion.
    *   A client's fingerprint (hostid) could be computed from a number
    *   of fingerprinting elements, such as ID PROM, ethernet address,
    *   etc.  In criterion, there is one bit reserved for each type of
    *   fingerprinting element, as indicated in the VLScg_LOCK_ defines.
    *   criterion can be any bit-OR combination of the types of fingerprinting
    *   elements.
    *   A license string can support limited clients.  Supply the client
    *   number as well.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if value is too big.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_INVALID_HEX_TYPE if value is not in hexadecimal format.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetClientLockMechanism
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,     /* INOUT - the license code structure */
      char  *   criterion, /* IN - The value to be set - should be in hex */
      int       client_num /* IN - The client number - 0 to ... */
#endif
   );

   /* Functions which set data fields: */

   /******************************************************************
    * FUNCTION NAME: VLScgSetShareLimit()/VLScgSetTeamLimit()
    * DESCRIPTION :
    *   Sets the value codeP->share_limit to the value of decimalNum.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetShareLimit
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *  codeP,     /* INOUT - the license code structure */
      char  *  decimalNum /* IN - Controls the number of
                 users/clients who can share a single license key.
                 Use a numeric decimal value.
                 Use NOLIMITSTR for no limit.
                 */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->num_secrets to the value of valu.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetNumSecrets
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   valu    /* IN - This valu sets the number of secrets.
                                  Should be from 0 to ... . */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->secrets[num] to the value of valu.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_CHARS if string is not valid.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetSecrets
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   valu,   /* IN - Any printable ASCII. */
      int        num    /* IN - Number of secret: should be from 0 to
                                  num_secrets-1 */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->vendor_info (private evndor info)to the
    *   value of pcPrivateVendorInfo. codeP->plain_vendor_info (Public
    *   vendor info) will be set to "".
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_CHARS if string is not valid.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    *
    */

   int VLScgSetVendorInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   pcPrivateVendorInfo /* IN - Any printable ASCII except # */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the values private vendor info (codeP->vendor_info) &
    *   public vendor info (codeP->plain_vendor_info) to the values of
    *   pcPrivateVendorInfo & pcPublicVendorInfo respectively.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_CHARS if string is not valid.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    *
    */

   int VLScgSetVendorInfoExt
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   pcPrivateVendorInfo,/* IN - Any printable ASCII except #.
                                            This value will appear as
                                            encrypted string in the license.*/
      char  *   pcPublicVendorInfo  /* IN - Any printable ASCII except #.
                                            This value will appear as
                                            unencrypted string in the license.*/
#endif
   );


/******************************************************************
 * DESCRIPTION :
 *   Sets the value codeP->license_vendor_info to the
 *   value of pcLicenseVendorInfo.
 *   Checks the user input and saves the value in the code struct.
 *
 * RETURN VALUES :
 *   returns VLScg_SUCCESS on successful return.
 *   returns VLScg_EXCEEDS_MAX_STRLEN if length exceeds maximum length of 512 bytes.
 *   returns VLScg_INVALID_CHARS if string is not valid.
 *   returns VLScg_RESERV_STR_ERR if value entered is a reserved string.
 *
 */

int VLScgSetLicenseVendorInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,        /* INOUT - the license code structure */
      char  *   pcLicenseVendorInfo /* IN - This value will appear as
                                            encrypted string in the license. */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->feature_name to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_NO_FEATURE_NAME if the name is NULL
    *   returns VLScg_RESERV_STR_ERROR if the string is a reserved string
    *   returns VLScg_INVALID_CHARS if the string characters are not printable.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetFeatureName
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Any printable ASCII except # */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->feature_version to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_RESERV_STR_ERROR if the string is a reserved string
    *   returns VLScg_INVALID_CHARS if the string characters are not printable.
    *   returns VLScg_EXCEEDS_MAX_VALUE if string exceeds maximum no. of chars.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetFeatureVersion
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Any printable ASCII except # */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->server_lock_info1[num] to the value of lockCode.
    *   lockCode should be a <= 8-character hexadecimal string (32 bit numeric
    *   value), optionally preceded by "0x".
    *   This function writes the value to lock code (fingerprint) group 1.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if value is too big.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_INVALID_HEX_TYPE if value is not in hexadecimal format.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetServerLockInfo1
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,    /* INOUT - the license code structure */
      char  *   lockCode, /* IN - The lock code to be checked and set */
      int       num       /* IN - Number of server, should be 0 to VLScg_MAX_NUM_SERVERS-1 */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->server_lock_info[num] to the value of lockCode.
    *   lockCode should be a <= 8-character hexadecimal string (32 bit numeric
    *   value), optionally preceded by "0x".
    *   This function writes the value to lock code (fingerprint) group 2.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if value is too big.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLScg_INVALID_HEX_TYPE if value is not in hexadecimal format.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetServerLockInfo2
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,    /* INOUT - the license code structure */
      char  *   lockCode, /* IN - The lock code to be checked and set */
      int       num       /* IN - Number of server, should be 0 to VLScg_MAX_NUM_SERVERS-1 */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the codeP->nl_client_lock_info[num] to the value of lockCode.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if num is greater than
    *     num_nl_clients-1
    *   returns VLScg_LESS_THAN_MIN_VALUE if num is less than 0.
    *   returns VLScg_INVALID_HEX_TYPE if value is not in hexadecimal
    *    format.
    *   returns VLScg_INVALID_IP_TYPE if value is not in dot format.
    *   returns VLScg_UNKNOWN_LOCK if the locking criteria is unknown.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    *
    */

   int VLScgSetClientLockInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,    /* INOUT - the license code structure */
      char  *   lockCode, /* IN - This buffer is used to set the lock
                               information i.e. the lock code for clients. */
      int        num      /* IN - Number of client: should be from 0 to
                               the max number of clients specified - 1 */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->keys_per_node[num] to the value of keys.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_EXCEEDS_MAX_VALUE if num exceeds num_nl_clients-1
    *   returns VLScg_LESS_THAN_MIN_VALUE if num < 0.
    *   returns VLScg_INVALID_INT_TYPE if num is not a non-negative integer.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetKeysPerNode
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   keys,   /* IN - This is used to set the number of keys
                             per node.  Give any decimal value. */
      /* should be from 0 ... */
      /* Give NOLIMITSTR for no limit */
      int        num    /* IN - Number of client: should be from 0 to
                                  the max number of clients specified - 1 */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->num_subnets to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if input is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum no of
    *    subnets.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 0.
    *   returns VLScg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLScgSetNumSubnets
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - This is used to set the  number
                                  of Subnets: should be from 1 to ... */
      /* 0 subnets is a special value which means no site
         licensing */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->site_lic_info[num] to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *   Processes string containing wildcards of the form *.123.*.28
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_RANGE if value is not in the range allowed
    *   returns VLScg_INVALID_RANGE if value is not a valid character
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetSiteLicInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info,   /* IN - This  is used to set the site
                                  licensing info in the codeT struct   */
      int        num    /* IN - Subnet number: should be from 0 to
                                  number of sites-1 */

#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->num_nl_clients to the value of info.
    *   Does checks to validate the user input.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if input is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum no of
   clients.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 1.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetNumClients
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Number of client lock codes to be specified.
                                  1 to ... */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->num_keys[num] to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds
    *    maximum no of keys allowed.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value less than 0.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetNumKeys
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info,    /* IN - This is used to set the  number of
                                  concurrent keys: should be from 0 to ... */
      /*      NOLIMITSTR for no limit */
      int       num     /* should be 0 in case of  single key and from 0 to
                             "no_of_features -1" in case of multi key. */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->soft_limit to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if info is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if info exceeds num_keys
    *   returns VLScg_LESS_THAN_MIN_VALUE if info less than 0.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetSoftLimit
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - This is used to set soft limit
                                should be from 0 to ...
                                NOLIMITSTR for no limit */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->key_life_units to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if info is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds 3
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 0
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetKeyLifetimeUnits
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Lifetime specification units of keys: from
                                  0 to 3.  The semantics are:
                             "0" - Multiple of 1 minute(s), maximum 15 minutes
                             "1" - Multiple of 10 minute(s), maximum 150 minutes
                             "2" - Multiple of 30 minute(s), maximum 450 minutes
                             "3" - Multiple of 60 minute(s), maximum 900 minutes
                           */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->capacity_units to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if info is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds 4
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 0
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetCapacityUnits
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Capacity specification units: from
                                  0 to 4.  The semantics are:
                             "0" - Multiple of 1(s), maximum 1022
                             "1" - Multiple of 10(s), maximum 10220
                             "2" - Multiple of 100(s), maximum 102200
                             "3" - Multiple of 1000(s), maximum 1022000
                             "4" - Multiple of 10000(s), maximum 10220000
                           */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->key_hold_units to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds 3
    *   returns VLScg_LESS_THAN_MIN_VALUE if value less than 0.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetKeyHoldtimeUnits
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Holdtime specification units of keys: from
                                  0 to 3.  The semantics are:
                             "0" - Multiple of 1 minute(s), maximum 15 minutes
                             "1" - Multiple of 10 minute(s), maximum 150 minutes
                             "2" - Multiple of 30 minute(s), maximum 450 minutes
                             "3" - Multiple of 60 minute(s), maximum 900 minutes
                           */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->key_lifetime to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_NOT_MULTIPLE if value is not a correct multiple.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum key lifetime.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 1.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetKeyLifetime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Indicates lifetime of keys: from 1 - ... */
      /* Absolute value in minutes */
      /* Maximum range depends on the codeP->key_life_units */
      /* Use NOLIMITSTR for infinite lifetime */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->key_holdtime to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_NOT_MULTIPLE if value is not a correct multiple.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds maximum allowed hold time.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value less than 0.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetKeyHoldtime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Indicates holdtime of keys: from 0 - ... */
      /* Absolute value in minutes */
      /* Maximum range depends on the codeP->key_hold_units */
      /* Use NOLIMITSTR for infinite holdtime */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets start date (year) of the license.
    *   Sets the value codeP->birth_year to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_YEAR if year is invalid.
    *   returns VLScg_INVALID_BIRTH_YEAR if year is too early.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed year.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLicBirthYear
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Enter year in four digits */
      /* Use  NEVERSTRING for infinite.  No need to specify
         birth month and birth day if year is infinite. */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets start date (month) of the license.
    *   Sets the value codeP->birth_month to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_CHARS if not a valid string.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 1.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds 12.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLicBirthMonth
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Month of year ("1"-"12") or ("jan"-"dec") */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets start date (day of month) of the license.
    *   Sets the value codeP->birth_day to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_DATE if value is not valid for the month.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 1.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed value.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLicBirthDay
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Day of the month ("1"-"31")
                                depending on the particular month. */
#endif
   );

   
   /******************************************************************
    * DESCRIPTION :
    *   Sets start time (hours) for the specified start date of the license.
    *   Sets the value codeP->birth_hours to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed value, i.e., 23.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
	*   Available from v16 onwards.
    */
   int VLScgSetLicBirthHours
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Hours corresponding to the license start time for the specified license start date
                            	  valid values: "0" - "23" */
#endif
   );
   
   
   /******************************************************************
    * DESCRIPTION :
    *   Sets start time (minutes) for the specified start date of the license.
    *   Sets the value codeP->birth_minutes to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed value, i.e., 59.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
	*   Available from v16 onwards.
    */
   int VLScgSetLicBirthMinutes
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Minutes corresponding to the license start time for the specified license start date
	                            valid values: "0" - "59" */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets expiration date (year) of the license.
    *   Sets the value codeP->death_year to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_YEAR if year is invalid.
    *   returns VLScg_INVALID_DEATH_YEAR if year is too early.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed year.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLicExpirationYear
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Enter year in four digits */
      /* Use  NEVERSTRING for infinite. No need to specify
         death month and death day if year is infinite. */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets expiration date (month) of the license.
    *   Sets the value codeP->death_month to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_CHARS if not valid string.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 1.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds 12.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLicExpirationMonth
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Month of year ("1"-"12") or ("jan"-"dec") */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets expiration date (day of month) of the license.
    *   Sets the value codeP->death_day to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_DATE if value is not valid for the month.
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 1.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed value.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetLicExpirationDay
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Day of the month ("1"-"31")
                                  depending on the particular month. */
#endif
   );
   
   /******************************************************************
    * DESCRIPTION :
    *   Sets expiration time (hours) for the specified expiration date of the license.
    *   Sets the value codeP->death_hours to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed value, i.e., 23.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
	*   Available from v16 onwards.
    */
   int VLScgSetLicExpirationHours
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Hours corresponding to the license expiration time for the specified license expiration date
                            	  valid values: "0" - "23" */
#endif
   );
   
   
   /******************************************************************
    * DESCRIPTION :
    *   Sets expiration time (minutes) for the specified expiration date of the license.
    *   Sets the value codeP->death_minutes to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds max allowed value, i.e., 59.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
	*   Available from v16 onwards.
    */
   int VLScgSetLicExpirationMinutes
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Minutes corresponding to the license expiration time for the specified license expiration date
	                            valid values: "0" - "59" */
#endif
   );   



   /******************************************************************
    * DESCRIPTION :
    *   Sets the grace period items to the provided value.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a decimal number.
    *   returns VLScg_EXCEEDS_MAX_VALUE if either value exceeds the
    *     maximum allowed value.
    *   returns VLScg_LESS_THAN_MIN_VALUE if either value is lower
    *     than minimum allowed value.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetGracePeriodDays
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,          /* IN */
      codeT *      codeP,            /* INOUT - the license structure */
      char  *      calendar_daysStr  /* IN */
#endif
   );

   int VLScgSetGracePeriodHours
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,          /* IN */
      codeT *      codeP,            /* INOUT - the license structure */
      char  *      elapsed_hoursStr  /* IN */
#endif
   );

   int VLScgSetGracePeriodFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,          /* IN */
      codeT *      codeP,            /* INOUT - the license structure */
      char  *      flag              /* IN */
#endif
   );

   /* ***************************************************************** */
   /*                                                                   */
   /* VLScgSetSNxxx APIs specified for generating short numeric license */
   /*                                                                   */
   /* ***************************************************************** */
   int VLScgSetSNNumKeys
      (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,   /* INOUT - the license code structure */
      char  *   info ,   /* IN - This is used to set the  number of
                                 concurrent keys: should be from 0 to ... */
                         /*      NOLIMITSTR for no limit */
      int num_features   /* specifies number of features in case of multi-key*/
#endif
      );

   int VLScgSetSNCodeType
      (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *      codeP,  /* INOUT - the license code structure */
      char  *      flag    /* IN - Short numeric license type
                                   VLScg_30D_STANDALONE_DEMO
                                   VLScg_30D_NETWORK_DEMO
                                   ...
                            */
#endif
      );

   int VLScgSetSNQtr_Year
      (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP,        /* INOUT - the license code structure */
      char *in_str         /* IN -    Quarter Year in which to stop licensing
                                      VLSCG_EXPIRED_AFTER_FIRST_QUARTER
                                      VLSCG_EXPIRED_AFTER_SECOND_QUARTER
                                      ...
                           */
#endif
      );

   int VLScgSetSN_Month(
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP,        /* INOUT -  the license code structure */
      char *str            /* IN -     Month of year ("1"-"12") or ("jan"-"dec") */
#endif
      );


   
   /******************************************************************
    * DESCRIPTION :  Internal use only.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_EXCEEDS_MAX_STRLEN if length of info exceeds 45 chars.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
    *   Available from v16 onwards.
    */
   int VLScgSetEntitlementId
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Entitlement ID : maximum of length 45 chars */
#endif
   );
   
   
   
   /******************************************************************
    * DESCRIPTION :  Internal use only.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_EXCEEDS_MAX_STRLEN if length of info exceeds 8 chars.
	*   returns VLScg_SUCCESS on successful return.
    * NOTES : 
	*   Available from v16 onwards.
    */
   int VLScgSetAuthorizationId
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Authorization ID : maximum of length 8 chars */
#endif
   );
   
   
   
   /******************************************************************
    * DESCRIPTION :  Internal use only.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
	*   Available from v16 onwards.
    */
   int VLScgSetProductId
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Product ID */
#endif
   );

   
   
   /******************************************************************
    * DESCRIPTION :  Internal use only.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
    *   Available from v16 onwards.
    */
   int VLScgSetFeatureId
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Feature ID */
#endif
   );

   
   
   /******************************************************************
    * DESCRIPTION :  Internal use only.    
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
    *   Available from v16 onwards.
    */
   int VLScgSetCloudUsageFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN -Cloud Usage flag : possible values 0 or 1*/
#endif
   );

   
   
   /******************************************************************
    * DESCRIPTION :  Internal use only.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if value is not a non-negative integer.
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
	 *   Available from v16 onwards.
    */
   int VLScgSetLicSourceFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN -Lic Source flag : possible values 0 or 1*/
#endif
   );

   /******************************************************************
    * DESCRIPTION : Internal use only.
    * RETURN VALUES :
    *   VLScg_SUCCESS        - All went well.
    *   VLScg_INVALID_INPUT  - Input parameters are not as expected e.g NULL input or input exceeding max size.
    *   VLScg_INTERNAL_ERROR - Setting this value is not allowed based on library and other inputs but user tried setting this.
    * NOTES : Available from v16 onwards.
    */
   int VLScgSetVendorSecretBlob
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,          /* IN */
      codeT *   codeP,               /* INOUT - the license code structure */
      char  *   pcVendorSecretBlob   /* IN */
#endif
   );

   /******************************************************************
    * DESCRIPTION : Internal use only.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
    *   Available from v17 onwards.
    */
   int VLScgSetActivationBirthTime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Product ID */
#endif
   );

   /******************************************************************
    * DESCRIPTION :Internal use only.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
    *   Available from v17 onwards.
    */
    
   int VLScgSetActivationExpiryTime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Product ID */
#endif
   );

  /******************************************************************
    * DESCRIPTION :   Internal Use Only
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INPUT if  One or more input parameters are invalid.
    *   returns VLScg_SUCCESS on successful return.
    * NOTES : 
    *   Available from v19 onwards.
    */
    
   int VLScgSetLicenseGenerationTime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle, /* IN */
      codeT     *codeP,     /* INOUT - the license code structure */
      char      *strLicGenTime       /* IN - time */
#endif
   );

   /******************************************************************
   * DESCRIPTION :  VLScgAllowxx() FUNCTIONS
   *
   *  Boolean functions: Return 1 on TRUE, 0 on FALSE.
   *
   *  These functions test whether the corresponding VLScgSetxx()
   *  should be called or not.  If VLScgAllowxx() returns 1 only then
   *  corresponding VLScgSetxx() function should be called.
   **********************************************************/
   /* Whether there is an option to generate Readable Licenses or not. */
   int VLScgAllowOutLicType
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowVendorInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowVendorInfoExt
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

int VLScgAllowLicenseVendorInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowLicenseType
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowAdditive
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowAggregateLicense
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowCodegenVersion
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   /* checks if the Multi-key functionality is applicable for the passed
      codeP struct. It depends upon the Codegen version as set
      using VLScgSetCodegenVersion API. By default, the codegen version is set
      to the latest version of the library.
    */
   int VLScgAllowMultiKey
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP            /* INOUT - The Pointer to codeT struct   */
#endif
   );

   /* Checks if the Multiple-Features are allowed for the passed codeP struct.
      It depends upon the key_type field of the passed codeP struct. This field
      is set using VLScgSetKeyType API.
    */
   int VLScgAllowMultipleFeature
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP            /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowRedundantFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowMajorityRuleFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowCommuterLicense
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowCommuterMaxCheckoutDays
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowLocalRequestLockCritFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowLocalRequestLockCrit
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowLogEncryptLevel
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowFeatureName
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowFeatureVersion
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowLockMechanism
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowLockModeQuery
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowMultipleServerInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowServerLockInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowClientLockInfo
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowKeysPerNode
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,  /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowSecrets
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowNumKeys
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowSoftLimit
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowSiteLic
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowKeyLifeUnits
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowKeyLifetime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowLicBirth
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   
   int VLScgAllowLicBirthTime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   
   int VLScgAllowLicExpiration
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   
   int VLScgAllowLicExpirationTime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );   


/******* VLScgAllowSharedLic()/VLScgAllowTeamCriteria()*****/

   int VLScgAllowSharedLic
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   /******* VLScgAllowShareLimit()/VLScgAllowTeamLimit()*****/

   int VLScgAllowShareLimit
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowHeldLic
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowKeyHoldtime
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowKeyHoldUnits
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   int VLScgAllowClockTamperFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   /* Whether the option of Capacity Licensing allowed */
   int VLScgAllowCapacityLic
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   /* Whether this a capacity license or not */
   int VLScgAllowCapacity
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowStandAloneFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowNetworkFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowPerpetualFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowGracePeriodFlag
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowGracePeriod
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );


   /* Trial Fields - Usage hours */
   int VLScgAllowTrialHours
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   
      /* VM Detection */
   int VLScgAllowVmDetection 
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   

   
   int VLScgAllowEntitlementId /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   int VLScgAllowProductId /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   int VLScgAllowAuthorizationId /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   int VLScgAllowFeatureId /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   int VLScgAllowCloudUsageFlag /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   int VLScgAllowLicSourceFlag /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   
   int VLScgAllowVendorSecretBlob /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowActivationBirthTime /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );

   int VLScgAllowActivationExpiryTime /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *codeP    /* INOUT - The Pointer to codeT struct   */
#endif
   );
   
   int VLScgAllowLicenseGenerationTime /* Internal Use Only */
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,    /* IN */
      codeT         *codeP      /* INOUT - The Pointer to codeT struct   */
#endif
   );   
   

   /**********************************************************
    * DESCRIPTION :
    *  It generates the license string for the given codeT struct.
    *  It should be called after all the VLScgSet functions are called.
    *  Memory allocation and free for codeT are the responsibilities
    *  of the caller of function.
    *  Memory allocation for the license string is being done by the
    *  function.  Pointer to a char will hold the string and its address
    *  is to be passed by caller of this function in second argument.
    * RETURN VALUES:
    *  returns VLScg_SUCCESS on successful return.
    *  returns VLScg_INVALID_VENDOR_CODE if vendor identification is illegal.
    *  returns VLScg_VENDOR_ENCRYPTION_FAIL if vendor-customized encryption fails.
    * NOTE :
    */

   int VLScgGenerateLicense
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,      /* IN */
      codeT *codeP,              /* IN */
      char ** result             /* OUT */
#endif
   );
#define _LSCGEN_UNIX_

   /**********************************************************
    * DESCRIPTION :
    *  It decodes the license string "AnyLicenseString" and puts the
    *  corresponding CodeT struct in the last argument.Address of a
    *  pointer to codeT struct is to be passed as the last argument.
    *  This pointer will contain the codeT of the input license string.
    *  This function takes care of all memory allocations it uses.
    *  Memory to codePP is given by this function.
    *
    * RETURN VALUES:
    *  returns VLScg_SUCCESS on successful return.
    *  returns error codes for errors.
    * NOTE :
    */

   int VLScgDecodeLicense
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,       /* IN */
      char * AnyLicenseString,    /* IN  license string*/
      char * lic_string,          /* OUT license string after removing comment chars and white spaces
                                     *     NOTE:if this parameter "lic_string"
                                     *           is set as NULL then set third parameter "lic_string_length"
                                     *           as 0 else set third parameter lic_string_length  as
                                     *           " (sizeof(lic_string)+1)"
                                     */
      int  lic_string_length,     /* IN  length of lic_string*/
      codeT ** codePP             /* OUT */
#endif
   );

   /* This API decodes a license entry and fills the codeP structure
    * provided by user. This API is an extension to VLScgDecodeLicense().
    * This API takes as input codeP structure allocated by user. The difference
    * between this API and VLScgDecodeLicense() is that this API expects
    * that all memory is allocated by the caller.
    */

   int VLScgDecodeLicenseExt
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,/*IN*/
      char * any_license_string,/*IN-User provided license string
                                     to be decoded */
      char * license_string,/*OUT-User allocated buffer to receive
                                 decoded license string*/
      int  * license_string_buflen, /*IN/OUT Length of the buffer
                                 allocated for decoded license string.*/
      codeT * codeP/*OUT-Pointer to codeT which will returned the
                        decoded information. (Allocated by the caller) */
#endif
    );


#undef _LSCGEN_UNIX_

   /* Functions for accessing dongle related data. */

   /**********************************************************
    * DESCRIPTION :
    *  Returns the number of license generation units available in the
    *  attached dongles.
    * RETURN VALUES:
    *  returns VLScg_SUCCESS on successful return.
    *    *initialUnitsP has the number of units that were initially available.
    *    This value MAY be VLScg_LICMETER_UNITS_UNAVAILABLE.
    *    *unitsLeftP has the number of units left. If its an unlimited key,
    *    *unitsLeftP is set to VLScg_LICMETER_UNITS_INFINITE.
    *  Error return codes are:
    *    VLScg_LICMETER_EXCEPTION
    *    VLScg_LICMETER_ACCESS_ERROR
    *    VLScg_LICMETER_CORRUPT
    *    VLScg_LICMETER_VERSION_MISMATCH
    *    V_FAIL
    *  On failure, *initalUnitsP and *unitsLeftP are set to
    *  VLScg_LICMETER_UNITS_UNAVAILABLE.
    *  On platforms that don't support dongles this functions returns V_FAIL.
    * NOTE :
    */
   int VLScgGetLicenseMeterUnits(
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,         /* IN */
      long         *initialUnitsP,   /* OUT */
      long         *unitsLeftP,      /* OUT */
      int          codegen_version   /* IN  */
#endif
   );

   /* Machine ID related functions.  Map to the functions in lserv.h: */
#define VLScgInitMachineID         VLSinitMachineID
#define VLScgGetMachineID          VLSgetMachineID
#define VLScgMachineIDtoLockCode   VLSmachineIDtoLockCode
#define VLScgMachineIDToLockCodeEx VLSmachineIDToLockCodeEx
#define VLScgHashedMachineIDtoLockCode VLShashedMachineIDToLockCode                                  




   /********************************************************************
   *
   *
   *  DESCRIPTION :
   *             Returns VLScg_SUCCESS if user is allowed to generate trial
   *             Licenses.
   *
   */

   int VLScgAllowTrialLicFeature(
      VLScg_HANDLE iHandle,     /* IN  */
      codeT * codep           /*  IN */
   );


   /************************************************************************
   *
   *   DESCRIPTION :
   *
   *             Sets the licType member of codeT struct to trial license
   *             or EVAL license or NORMAL license type.
   *
   *   RETURN VALUES :
   *
   *            Returns VLScg_SUCCESS on success otherwise it returns
   *            Error codes.
   *
   */

   int VLScgSetLicType(
      VLScg_HANDLE iHandle,     /* IN  */
      codeT        *codep,     /* IN  */
      char *       lictype     /*  IN */
   );


   /***********************************************************************
   *
   *   DESCRIPTION :
   *            Sets the trialDaysCount member of codeT struct.
   *
   *   RETURN VALUES :
   *            Returns VLScg_SUCCESS  on success otherwise it returns
   *            Error code.
   *
   */

   int VLScgSetTrialDaysCount(
      VLScg_HANDLE iHandle,       /* IN */
      codeT       *codep,        /* IN */
      char        *daysStr       /* IN */
   );

   /***********************************************************************
   *
   *   DESCRIPTION :
   *            Sets the trial_elapsed_hours member of codeT struct.
   *
   *   RETURN VALUES :
   *            Returns VLScg_SUCCESS  on success otherwise it returns
   *            Error code.
   *
   */

   int VLScgSetTrialHours(
      VLScg_HANDLE iHandle,       /* IN */
      codeT       *codep,        /* IN */
      char        *trialHoursStr      /* IN */
   );

   /***********************************************************************
   *
   *   DESCRIPTION :
   *            Sets the 'vm_detection' member of codeT struct.
   *
   *   RETURN VALUES :
   *            Returns VLScg_SUCCESS  on success otherwise it returns
   *            Error code.
   *
   */

   int VLScgSetVmDetection
   (
      VLScg_HANDLE     iHandle,			/* IN */
      codeT            *codeP,  			/* IN */
      char             *vmDetectionStr /* IN */
   );


   
   
   /********************************************************************
   *
   *  DESCRPTION :
   *           Gets the no. of units available to generate trial
   *           licenses from dongle.
   *
   *   RETURN VALUES :
   *             Returns VLScgSUCCESS on success otherwise it
   *             returns Error codes.
   *
   */
   int VLScgGetTrialLicenseMeterUnits(
      VLScg_HANDLE iHandle,            /* IN  */
      int          *units,           /* OUT */
      int          codegen_version   /*IN  */
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value codeP->numeric_type to the value of num.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLScg_INVALID_INT_TYPE if num is not a non-negative integer.
    *   returns VLScg_EXCEEDS_MAX_VALUE if value exceeds 3
    *   returns VLScg_LESS_THAN_MIN_VALUE if value is less than 0
    *   returns VLScg_SUCCESS on successful return.
    * NOTES :
    */
   int VLScgSetNumericType
   (
#ifndef LSNOPROTO
      VLScg_HANDLE iHandle,   /* IN */
      codeT *   codeP,  /* INOUT - the license code structure */
      int  num          /* IN - numeric type 0 to 3.
                              VLScg_NUMERIC_UNKNOWN    0
                              VLScg_NOT_NUMERIC        1
                              VLScg_MISC_SHORT_NUMERIC 2
                              VLScg_MISC_NUMERIC       3
                          */
#endif
   );

   typedef struct _VPT_REQUEST_LINE
   {
      unsigned char          ucOperation;               /* requested operation */
      unsigned char        * pucVendorDefined;        /* vendor defined data */
      unsigned long          ulVendorDefinedLength; /* length of vendor defined data */
      unsigned char        * pucLicenseLine;             /* license to be added or revoked */
      unsigned long          ulLicenseLineLength;      /* license line length */
   } VPT_REQUEST_LINE, *PVPT_REQUEST_LINE;

   typedef struct _VPT_REQUEST
   {
      unsigned char        * pucTransactionId;        /* transaction ID */
      unsigned long          ulLockCodeSelector;       /* lock code selector */
      unsigned char        * pucLockInfo;               /* target UDI */
      unsigned long          ulTimeStamp;             /* time stamp */
      unsigned long          ulRequestArraySize;       /* number of requests */
      PVPT_REQUEST_LINE      pvRequestArray;
   } VPT_REQUEST, *PVPT_REQUEST;


   //Added 8.4.1, 
   typedef struct _VPT_REQUEST_LINE_EXT
   {
      unsigned long struct_size;
      unsigned char ucOperation;
      unsigned int  hard_limit_to_revoke;         /*Network Partial Revoke PT Specific */
      unsigned char *pucVendorDefined;
      unsigned long ulVendorDefinedLength;
      unsigned char *pucLicenseLine;
      unsigned long ulLicenseLineLength;
   } VPT_REQUEST_LINE_EXT, *PVPT_REQUEST_LINE_EXT;

   //Added 8.5.3, 
   typedef struct _VPT_REQUEST_LINE_EXT2
   {
      unsigned long struct_size;
      unsigned char ucOperation;
      unsigned int  hard_limit_to_revoke;         /*Network Partial Revoke PT Specific */
      unsigned char *pucVendorDefined;
      unsigned long ulVendorDefinedLength;
      unsigned char *pucLicenseLine;
      unsigned long ulLicenseLineLength;
#if (defined _WIN64) || (defined _V_LP64_) || (defined _TRU64_) 
      unsigned long long uiRevokeGraceDays; /* Data type modifed for 64-bit ONLY to address compatibility across platforms, max allowed value remains VLScg_MAX_REVOKE_GRACE_DAYS */
#else
      unsigned int  uiRevokeGraceDays;   /* New member added to store deferred revoke days value */
#endif	
   } VPT_REQUEST_LINE_EXT2, *PVPT_REQUEST_LINE_EXT2;

   //Added 8.4.1, 
   typedef struct _VPT_REQUEST_EXT
   {
      unsigned long struct_size;
      unsigned char * pucTransactionId;
      unsigned long ulLockCodeSelector;
      unsigned char * pucLockInfo;
      unsigned long ulTimeStamp;
      unsigned char *pucCustomDefined;
      unsigned long ulCustomDefinedLength;
      unsigned long ulRequestArraySize;
      PVPT_REQUEST_LINE_EXT pvRequestArrayExt;
   } VPT_REQUEST_EXT, *PVPT_REQUEST_EXT;

   //Added 8.5.3 
   typedef struct _VPT_REQUEST_EXT2
   {
      unsigned long struct_size;
      unsigned char * pucTransactionId;
      unsigned long ulLockCodeSelector;
      unsigned char * pucLockInfo;
      unsigned long ulTimeStamp;
      unsigned char *pucCustomDefined;
      unsigned long ulCustomDefinedLength;
      unsigned long ulRequestArraySize;
      PVPT_REQUEST_LINE_EXT2 pvRequestArrayExt;      
   } VPT_REQUEST_EXT2, *PVPT_REQUEST_EXT2;

   //Added 8.5.5 - Generate PT for multiple servers in the redundant pool
   typedef struct _VPT_REQUEST_EXT3
   {
      unsigned long struct_size;
      unsigned char *pucTransactionId;
      unsigned long ulLockCodeSelectorArr[MAX_REDUNDANT_SERVERS_IN_PT];
      unsigned char *pucLockInfoArr[MAX_REDUNDANT_SERVERS_IN_PT];
      unsigned long ulTimeStamp;
      unsigned char *pucCustomDefined;
      unsigned long ulCustomDefinedLength;
      unsigned long ulRequestArraySize;
      PVPT_REQUEST_LINE_EXT2 pvRequestArrayExt;      
   } VPT_REQUEST_EXT3, *PVPT_REQUEST_EXT3;

   typedef struct _VRT_VERIFY_ERROR_LINE
   {
      unsigned long          ulErrorCode;               /* error code */
      unsigned char          ucOperation;               /* operation type */
      unsigned long          ulStatus;                  /* status */
      unsigned char          pucLicenseLine[VLS_MAX_CUSTOM_LICENSE_SIZE]; /* license line */
      unsigned long          ulLicenseLineLength;       /* license line length */
      unsigned long          ulCapacityRevoked;         /* capacity revoked */
      unsigned long          ulNumberOfLicensesRevoked; /* number of licenses revoked */
   } VRT_VERIFY_ERROR_LINE, *PVRT_VERIFY_ERROR_LINE;

   typedef struct _VRT_VERIFY_ERRORS
   {
      unsigned long          ulNumOfErrors;             /* number of error */
      VRT_VERIFY_ERROR_LINE* pvRTErrorArray;            /* error line */
   } VRT_VERIFY_ERRORS, *PVRT_VERIFY_ERRORS;

   typedef struct _VRT_VERIFY_ERRORS_LIST
   {
      struct _VRT_VERIFY_ERRORS_LIST* pvNext;
      PVRT_VERIFY_ERROR_LINE          pvError;
   } VRT_VERIFY_ERRORS_LIST, *PVRT_VERIFY_ERRORS_LIST;



   typedef struct _VRT_REVOKE_TICKET_LINE
   {
	unsigned long             struct_size;
	unsigned char             feature_name[VLS_MAXFEALEN];
	unsigned char             feature_version[VLS_MAXFEALEN];
	unsigned char             *pucLicenseHash;                   // Specific to network revoke
	unsigned char             *pucLicenseLine;                   // Specific to Standalone revoke
	unsigned long             ulLicenseLineLength;               // Specific to standalone revoke
	unsigned char             ucOperation;
	unsigned int              base_license_hard_limit;           // Specific to network revoke
	unsigned int              number_licenses_revoked;           // Specific to network revoke
	unsigned int              status;
	unsigned int              *unused1;
	unsigned int              unused2;
   } VRT_REVOKE_TICKET_LINE,  *PVRT_REVOKE_TICKET_LINE;


   typedef struct _VRT_REVOKE_TICKET_INFO
   {
	unsigned long             struct_size;
	unsigned char             *pucTransactionId;
	unsigned long             locking_criteria;
	unsigned char             *locking_info;
	unsigned long             time_stamp;
	unsigned char             *pucCustomDefined;
	unsigned long             ulCustomDefinedLength;
	PVRT_REVOKE_TICKET_LINE   pvRevokeInfoArray;
	unsigned long             ulRevokeInfoArraySize;
	unsigned int              *unused1;
	unsigned int              unused2;
   } VRT_REVOKE_TICKET_INFO, *PVRT_REVOKE_TICKET_INFO;



   /* generate permission ticket by request */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgeneratePermissionTicket(
#ifndef LSNOPROTO
      PVPT_REQUEST           pvRequest,                     /* IN - request data */
      unsigned char          *pucPermissionTicket,          /* OUT the generated permission ticket*/
      unsigned int           *pui16PermissionTicketLength   /* IN/OUT - permission ticket length*/
#endif /* LSNOPROTO */
      );

   //Added 8.4.1, 
   /* generate permission ticket for network and standalone revoke operations */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgeneratePermissionTicketExt(
#ifndef LSNOPROTO
      PVPT_REQUEST           pvRequest,                    /* IN - request data, For old version PT, Standalone requests only */
      unsigned char          * pucPermissionTicket,        /* OUT the generated permission ticket*/
      unsigned int           * pui16PermissionTicketLength,/* IN/OUT - permission ticket length*/
      PVPT_REQUEST_EXT       pvRequestExt                  /* IN - request data, For New version PT, Standalone/Network requests */
#endif /* LSNOPROTO */
      );

   //Added 8.5.3, 
   /* generate extended permission ticket for network revoke only */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgeneratePermissionTicketExt2(
#ifndef LSNOPROTO
      void                   * vpRequest,   /* IN - request data */
	  unsigned int             uiStructSize,
      unsigned char          * pucPermissionTicket,        /* OUT the generated permission ticket*/
      unsigned int           * pui16PermissionTicketLength  /* IN/OUT - permission ticket length*/
      
#endif /* LSNOPROTO */
      );

      /* verify revocation ticket */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSverifyRevocationTicket(
#ifndef LSNOPROTO
      PVPT_REQUEST           pvOriginalRequest,            /* IN - original request */
      unsigned char         *pucRevocationTicket,          /* IN - revocation ticket */
      unsigned long          ulRevocationTicketLength,     /* IN - revocation ticket length */
      PVRT_VERIFY_ERRORS     pvErrorInfo,                  /* OUT - error info */
      unsigned long         *pulErrorInfoTotalLength       /* IN/OUT - error line total length */
#endif /* LSNOPROTO */
      );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSverifyRevocationTicketExt(
#ifndef LSNOPROTO
	  PVPT_REQUEST_EXT     pvOriginalRequestExt,		/* IN - original request: If not provided pucPermissionTicket */
      unsigned char        *pucPermissionTicket,		/* IN - original request: If not provided pvOriginalRequestExt */
      unsigned long        ulPermissionTicketLength,    /* IN - permission ticket length */
      unsigned char        *pucRevocationTicket,      	/* IN - revocation ticket */
      unsigned long        ulRevocationTicketLength,    /* IN - revocation ticket length */
      PVRT_VERIFY_ERRORS   pvErrorInfo,                 /* OUT - error info */
      unsigned long        *pulErrorInfoTotalLength,    /* IN/OUT - error line total length */
      unsigned long        *unused
#endif /* LSNOPROTO */
      );
      
   typedef struct _VRT_VERIFY_INFO_REDUNDANT_LIC
   {
      unsigned char        *pucRevocationTicket;      	/* IN - revocation ticket */
      unsigned long        ulRevocationTicketLength;    /* IN - revocation ticket length */
      PVRT_VERIFY_ERRORS   pvErrorInfo;                 /* OUT - error info */
      unsigned long        ulErrorInfoTotalLength;    /* IN/OUT - error line total length - would need to be allocated by the user in case of error.*/
   } VRT_VERIFY_INFO_REDUNDANT_LIC, *PVRT_VERIFY_INFO_REDUNDANT_LIC;

   typedef struct _VRT_LOCK_INFO
   {
      unsigned long ulLockCriteria;
      unsigned char pucLockCode[VLS_MAXSRVLOCKLEN];
   } VRT_LOCK_INFO, *PVRT_LOCK_INFO;
   
/*  VLSverifyRevocationTicketExt2  - API for RT verification for a redundant license. Introduced from 8.5.5
                                     Also supports verification for non redundant license (network revocation via license string)*/
VDLL32 LS_STATUS_CODE VMSWINAPI VLSverifyRevocationTicketExt2(
#ifndef LSNOPROTO	  
      void*                              pvOriginalRequest,           /* IN - original request, if PT is not provided.
                                                                           Can be of type PVPT_REQUEST_EXT2 or PVPT_REQUEST_EXT3 */
      unsigned long                      ulStructSize,                /* IN - size of the pvOriginalRequest structure, if being passed. */
      unsigned char                      *pucPermissionTicket,		  /* IN - original request */
      unsigned long                      ulPermissionTicketLength,    /* IN - permission ticket length */
      unsigned long                      ulNumRevocationTickets,        /*IN - Number of revocation tickets that are being verified */
      PVRT_VERIFY_INFO_REDUNDANT_LIC     pVerifyInfo,                 /*IN/OUT – array of  verify Info structs, memory allocation user's responsibility */
      unsigned long                      *pulNumRTsAbsent,            /* IN/OUT - number of RTs that were missing as against the PT passed, also reflects the size of the pLockInfoOfAbsentRTs array*/
      PVRT_LOCK_INFO                     pLockInfoOfAbsentRTs,        /* OUT - Lock Info array, to provide details of missing RTs; memory allocation, user’s responsibility. Max possible array size is MAX_REDUNDANT_SERVERS_IN_PT. The intention of this parameter is to help the ISV know the details of the machine for which RT(s) is/are missing.*/
      void                               *unused
#endif /* LSNOPROTO */
      );      


   VDLL32 LS_STATUS_CODE VMSWINAPI VLScgDecodeLicenseRevocationTicketExt(
#ifndef LSNOPROTO
	  char                       *license_revocation_ticket_buffer_old,        /* IN: to decode LRT */
	  int                        license_revocation_ticket_buffer_old_size,    /* IN: to decode LRT */
	  unsigned char              *revocation_ticket_buffer,                    /* IN: to decode RT */
	  unsigned long              revocation_ticket_buffer_size,                /* IN: to decode RT */
	  char                       *secret_key,                                  /* specific to decode LRT */
	  int                        secret_key_length,
	  VLSrevocationTicketInfoT   *license_revocation_ticket_old,               /* OUT structure for For LRT */
	  PVRT_REVOKE_TICKET_INFO    revocation_ticket,                            /* OUT structure for For RT */
	  unsigned long              *pulRevocation_ticket_size,
	  unsigned long              *unused
#endif /* LSNOPROTO */
 	);


   /**********************************************************************
   *
   *
   *   DESCRIPTION   :
   *             Sets and loads the software license file (lscgen.lic)
   *
   *    RETURN VALUES :
   *             Returns VLScgSUCCESS on success otherwise it returns
   *             corresponding error codes.
   */

   int VLScgSetLoadSWLicFile(
      VLScg_HANDLE  iHandle,        /* IN  */
      char          *filename      /* Complete name and path of sw
                                             license file */
   );


   /***********************************************************************
   *
   *
   *   DESCRIPTION   :
   *   This function calculates the license hash.
   *
   *   pcLicenseString     - Points to the string to be hashed.
   *   pucLicenseHash      - Points to the location to store the hash value.
   *   piLicenseHashLength - Length of the hash buffer.
   *
   *   Status Values Returned:
   *
   *   VLScg_SUCCESS          - All went well.
   *   VLScg_INVALID_INPUT    - One or more input parameters are invalid.
   *   VLScg_BUFFER_TOO_SMALL - If the input size of the buffer specified for
   *                            storing hash is smaller than required.
   */
   int VLScgCalculateLicenseHash(
      char            *pcLicenseString,     /* IN */
      unsigned char   *pucLicenseHash,      /* OUT */
      int             *piLicenseHashLength  /* INOUT */
   );


   /**********************************************************************
   *
   * DESCRIPTION:
   *   This validates the structure prior to generating a license.
   *
   * RETURN VALUES: VLScg_ error codes.
   *
   */
   int VLScgValidateCodeT
   (
#ifndef LSNOPROTO
      VLScg_HANDLE  iHandle,
      codeT*        codeP
#endif
   );
   
   /* Frees the memory allocated for license string by VLScgGenerateLicense API. */
   void VLScgFreeLicenseString
   (
      char* result
   );

#define LIBINFOLEN  32

typedef struct
{
      long           structSz;
      char           szVersion  [LIBINFOLEN];
}
 VLScg_LIBVERSION;

/* Call this function to get a description of the client library version */
   int VLScgGetLibInfo
   (
		VLScg_LIBVERSION * pStruct /* INOUT */
	);

#define _LSCGEN_UNIX_

#ifdef __cplusplus
}
#endif


#endif /* _LSCGEN_H_ */
#undef _LSCGEN_UNIX_

