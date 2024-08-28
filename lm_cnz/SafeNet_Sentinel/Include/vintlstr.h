#ifndef _VINTLSTR_H_
#define _VINTLSTR_H_
/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

/*H******************************************************************
* FILENAME        : vintlstr.h
*
* DESCRIPTION     :
*           Contains the string repository for RMS Developer Kit
*
* FUNCTIONS       :
*           Do not carry any function headers.
*
* NOTES           :
*   All RMS Developer Kit strings could be positionally divided into
*   four major categories.
*     1) Client Library          [CLL]
*     2) Server                  [SRV]
*     3) Code Generator Library  [CGN]
*     4) Tools                   {specific to the tool }
*           4.1 lsmon                [LSM]
*           4.2 lsrvdown             [LSD]
*           4.3 lslic                [LSL]
*           4.4 enctordc             [ETR]
*           4.5 lswhere              [LSW]
*           4.6 lspool               [LSP]
*           4.7 rlftool              [LCF]
*           4.8 echoid               [ECO]
*           4.9 rdctoenc             [RTE]
*           4.10 lsdecode            [LSD]
*           4.11 lsusage and vusage  [USG]
*           4.12 lcommute            [CMT]
*           4.13 timefix             [TFX]
*           4.14 Shell command-line  [SHL]
*           4.15 time tamper         [TTU]
*           4.15 rcommutr            [RMT]
            4.16 lsinit, lsgeninit   [INIT]
*
*
*
*     In addition, we need create a new COMMON category where we can place
*     strings common to the complete RMS Developer Kit.
*
*     5) Common         [COM]
*     Within, this broad category of positional categorization, strings are
*     further categorized on their purpose. Some of the categories and their
*     corresponding acronyms are :
*
*          5.1) Error            [ERR]
*          5.2) Information      [INF]
*          5.3) Confirmation     [CFM]
*          5.4) Prompt           [PRM]
*          5.5) Debug/Trace      [TRC]
*          5.6) Label            [LBL]
*          5.7) Header           [HDR]
*          5.8) Log File         [LFE]
*
*     6) GUIs
*          6.1) bounce and qbounce  [BNC]
*      6.2) wlsgrmgr            [GMR]
*          6.3) makeserl            [MKS]
*          6.4) loadls (lservnt)    [SNT]
*
*     7) Samples                    [SMP]
*
*H*/

#ifndef NEW_LINE
   #define NEW_LINE "\n"
#endif

/* COMMON STINGS - Acronym [COM] */

#define VL10N_COM_LBL_LS_PROD_NAME_STR    "Sentinel RMS Development Kit"
#define VL10N_COM_LBL_LS_PROD_NAME_SRV_STR    "Sentinel RMS License Manager"
/* Do not translate */
#define VL10N_COM_LBL_LS_VERSION_STR      "9.7.0.0036"
#define VL10N_COM_LBL_LS_COPYRIGHT_STR    "  Copyright (C) 2020 SafeNet, Inc.\n\n"
#define VL10N_COM_RESOURCE_COPYRIGHT_STR  "Copyright (C) 2020 SafeNet, Inc.\0"
#define VL10N_COM_ERR_UNABLE_OPEN_STR       "Unable to open file %s.\n"
/* Example: Unable to open file filename.dat */

#define VL10N_COM_ERR_UNAUTHORIZE_SW_STR    "Unauthorized copy of software.\n"
#define VL10N_COM_ERR_INIT_CGEN_LIB_STR     "Error initializing code generator library.\n"
#define VL10N_COM_ERR_INP_REENTER_STR      "\n Invalid input , please reenter : "
#define VL10N_COM_ERR_INP_INSUFFICIENT_STR      "\n Insufficient input. Quitting...\n"


#define VL10N_COM_PRM_HIT_ENTER_STR        "\n Press Enter to continue . . . \n"
#define VL10N_COM_PRM_HIT_ENTER_01_STR     "Press Enter at the end of each input.\n\n"
#define VL10N_COM_PRM_CHOICE_STR           "Please enter choice: "
#define VL10N_COM_PRM_ENTER_SKIP_STR       "Press <Enter> to skip: "
#define VL10N_COM_ERR_REENTER_STR          "\nPlease reenter:\n "
#define VL10N_COM_ERR_INVALID_INPUT_STR    "\nInvalid Input"
#define VL10N_COM_ERR_INVALID_INPUT_02_STR "\nInvalid Input. "
#define VL10N_COM_INF_RESPONSE_STR         "\nPlease type the desired responses at the prompts below.\n"
#define VL10N_COM_ERR_REENTER_01_STR       "Please reenter: "
#define VL10N_COM_ERR_REENTER_VER_STR      "Please reenter feature version: "
#define VL10N_COM_ERR_REENTER_NM_STR       "Please reenter feature name: "


#define VL10N_COM_INF_INFINITE_STR         "  INFINITE"
#define VL10N_COM_INF_EXIT_STR             "Exiting...\n"
#define VL10N_COM_INF_FILE_DEL_STR         "\nDeleting File : %s ...\n\n\n"
#define VL10N_COM_ERR_Y2KUNSAFE_STR        "System time is Y2K unsafe \n "
#define VL10N_COM_INF_VENCONT_STR          "Contact your Vendor."
#define VL10N_COM_ERR_ID_STR               "Error[%d]: "
#define VL10N_COM_ERR_ID_LONG_STR          "Error[%u]: "
#define VL10N_COM_ERR_LONG_ID_STR          "Error[0x%lx]: "
#define VL10N_COM_ERR_FATAL_STR            "Fatal: internal error"
#define VL10N_COM_ERR_STR                  "Error"

#define VL10N_COM_ERR_CLL_FAILURE_STR      "Library initialization error\n"
#define VL10N_COM_ERR_FILE_OPEN_01_STR     "File open error.\n"
#define VL10N_COM_ERR_FILE_OPEN_02_STR     "Error opening the file \"%s\"\n"
#define VL10N_COM_ERR_OBTAIN_VER_STR       "Unable to obtain %s version\n"
#define VL10N_COM_ERR_INVALID_VALUE_STR    "Invalid value. Please reenter.\n"
#define VL10N_COM_ERR_INVALID_ANS_STR      "Invalid answer - type Y or N : "
#define VL10N_COM_ERR_UNKNOWN_STR          "Unknown error code, cannot print error message.\n"

#define VL10N_COM_LBL_USAGE_STR            " Usage: %s %s\n"
#define VL10N_COM_LBL_ON_01_STR            "ON"
#define VL10N_COM_LBL_ON_02_STR            "On"
#define VL10N_COM_LBL_OFF_01_STR           "OFF"
#define VL10N_COM_LBL_OFF_02_STR           "Off"
#define VL10N_COM_LBL_YES_01_STR           "Y"
#define VL10N_COM_LBL_YES_02_STR           "y"
#define VL10N_COM_LBL_NO_01_STR            "N"
#define VL10N_COM_LBL_NO_02_STR            "n"

#define VL10N_COM_HDR_DIST_CRIT_STR       "\n The Distribution Criteria is:"
#define VL10N_COM_ERR_USAGE_STR           " Use LSHOST environment variable to specify the contact server.\n"
#define VL10N_COM_ERR_USAGE1_STR          " Use LSFORCEHOST environment variable to specify the contact server.\n"
#define VL10N_COM_PRM_LICSTR_STR         " Enter the license code: "
#define VL10N_COM_LBL_LICSTR_STR         "\n License code: %s"
#define VL10N_COM_LBL_DIST_CRIT_STR      "\n Distribution criteria: %s"
/* Do not translate */
#define VL10N_COM_LBL_QUIET_STR          "LS_QUIET"
#define VL10N_COM_ERR_INVALID_STR        "\n Invalid license code: \"%s\"\n"
#define VL10N_COM_ERR_BAD_HANDLE_STR     " Bad handle.\n"
#define VL10N_COM_ERR_ACK_QUIT_STR   "\n Acknowledged. Quitting...\n"
#define VL10N_COM_ERR_SRV_NAME_STR       "\n Server Name cannot be resolved."
#define VL10N_COM_LBL_SRV_ADDED_STR      "\n %s added successfully to the pool\n\n"
#define VL10N_COM_LBL_SRV_DELETED_STR    "\n %s deleted successfully from the pool\n\n"
#define VL10N_COM_ERR_TOOOLD_STR          " Error: Expired license \"%s\" is too old to be converted.\n"
#define VL10N_COM_ERR_NOTAUTHR_STR        " Error: Not authorized to convert: \"%s\"\n"
#define VL10N_COM_ERR_UNABLE_STR          " Error: Unable to convert license code: \"%s\"\n"
#define VL10N_COM_HDR_LICMGR_STR         " Sentinel RMS License Manager"
#define VL10N_COM_ERR_LICMGR_STR          " License Manager Error"
#define VL10N_COM_HDR_SLMABOUT_STR        " About - Sentinel RMS Development Kit"
#define VL10N_COM_LBL_INFORMATION_STR        "Information"
/* Do not translate */
#define VL10N_COM_LBL_DIRSEP_STR          "\\"
/* Do not translate */
#define VL10N_COM_LBL_3DOTS_STR          " ..."
#define VL10N_COM_ERR_ILLEGL_VENDRID_STR  "Illegal vendor identification.\n"
/* Do not translate */
#define VL10N_COM_INF_STRING_NEWLINE_STR  "%s\n"

/* { Messages from file lsMsgTable.c } */

#define VL10N_COM_ERR_SPECIFY_FTR_NM_STR       "Feature Name must be specified. It cannot be empty."
#define VL10N_COM_INF_INT_EXPECTED_STR         "%s : Expected a positive integer value, found \"%s\"."
/* Example: lscgen: Expected an integer value, found "abc" */

#define VL10N_COM_ERR_VAL_EXCEEDS_MAX_STR      "%s : Value entered (%s) is greater than the maximum supported value. The maximum (in decimal) value is %u"
/* Example: lscgen: Value entered (2) is less than the minimum supported value.  The minimum (in decimal) value is 3 */

#define VL10N_COM_ERR_VAL_LESS_THAN_MIN_STR    "%s : Value entered (%s) is less than the minimum supported value. The minimum (in decimal) value is %u"
#define VL10N_COM_ERR_LENGTH_EXCEEDS_VAL_STR   "%s : Length of \"%s\" is greater than %d"
#define VL10N_COM_ERR_LENGTH_SMALLER_VAL_STR   "%s : Length of \"%s\" is smaller than %d"
#define VL10N_COM_INF_GIVE_MULTPL_STR          "%s : Value of %s should be a multiple of %d"
#define VL10N_COM_ERR_INVALID_EXPRN_STR        "Expiration year cannot be less than %d"
#define VL10N_COM_ERR_INVALID_START_YR_STR     "Start year cannot be less than %d"
#define VL10N_COM_ERR_INVALID_VAL_STR          "%d is not valid in %s, %d"
#define VL10N_COM_ERR_WRONG_ENTRY_01_STR          "%s : Wrong value entered - \"%s\". ( Should be hexadecimal )"
#define VL10N_COM_ERR_WRONG_ENTRY_02_STR       "%s : Wrong value entered - \"%s\".  IP address should be specified in dot form."
#define VL10N_COM_INVALID_YEAR_STR              "Invalid year entered -  \"%s\"."
#define VL10N_COM_INF_IS_RESERVED_STR           "%s : \"%s\" is a reserved string."
#define VL10N_COM_INF_IS_RESERVED_CHAR          "%s : '%s' is a reserved character."
#define VL10N_COM_INF_VIOLATES_VALID_RANGE_STR  "%s : Value \"%s\" violates the valid range of input."
#define VL10N_COM_ERR_INVALID_CHARS_STR         "%s : Invalid characters - \"%s\"."
#define VL10N_COM_ERR_LIC_STR_SMALL_STR         "License code \"%s\" too small to parse."
#define VL10N_COM_INF_PREMATUR_TERM_STR         "Premature termination of license code.  Please check."
#define VL10N_COM_ERR_REMAP_FALIED_STR          "Failed to remap default strings from configuration file for license \"%s\"."
#define VL10N_COM_ERR_LIC_DECRYPT_FAIL_STR          "Decryption failed for license code."
#define VL10N_COM_ERR_DYN_LIC_DECRYPT_FAIL_STR  "Decryption failed for dynamically added license code."
#define VL10N_COM_ERR_CHKSUM_FAILED_STR         "Either invalid lock code provided or checksum validation failed. Please verify."
#define VL10N_COM_ERR_DEFAULT_FIXED_STRNG_STR    "Default fixed string error."
#define VL10N_COM_ERR_SECRETS_DECRYPT_FAIL_STR  "Decryption failed for secrets.  Verify the configuration file for readable licenses."
#define VL10N_COM_ERR_LIC_STRING_STR             "Error in license code.  Please check."
#define VL10N_COM_ERR_OUT_OF_HEAP_STR            "Out of heap memory."
#define VL10N_COM_ERR_INTERNAL_ERROR_STR         "Internal error."
#define VL10N_COM_ERR_UNKNOWN_LOCK_MECH_STR      "Unknown lock mechanism - \"%s\"."
#define VL10N_COM_ERR_VALUE_TOO_LARG_STR         "Value \"%s\" : too large."
#define VL10N_COM_ERR_INVALID_INPUT_01_STR          "Invalid input - \"%s\"."
#define VL10N_COM_INF_MAX_LIMIT_CROSS_STR        "Maximum limit crossed."
#define VL10N_COM_INF_NO_RESOURCES_STR           "No resources left."
#define VL10N_COM_ERR_BAD_FILE_HADLE_STR         "Bad file handle."
#define VL10N_COM_ERR_OPRN_FAILED_STR            "Operation failed."
#define VL10N_COM_ERR_INVALID_VENDR_CODE_STR     "Invalid Vendor Code.  Please contact your Sentinel RMS Development Kit distributor."
#define VL10N_COM_ERR_VENDOR_CUST_ENCRYPTN_STR   "Vendor-customized encryption failed."
#define VL10N_COM_INF_EXPR_DATE_01_STR           "License Expiration Date must be greater than Start Date"
#define VL10N_COM_INF_EXPR_DATE_02_STR           "License Expiration Year must be greater than Start Year"
#define VL10N_COM_INF_EXPR_DATE_03_STR           "License Expiration Month must be greater than Start Month"
#define VL10N_COM_ERR_EXCEPTION_STR              "Unknown exception (%d) in accessing Sentinel RMS Development Kit license meter(s)."
#define VL10N_COM_INF_LIC_MTR_DECREM_01_STR      "Your Sentinel RMS Development Kit license meter(s) have been decremented by %d units. You now have %d units left out of an initial count of %d units."
#define VL10N_COM_ERR_LIC_MTR_ACCESS_STR         "Error accessing Sentinel RMS Development Kit license meter(s). Please make sure the Sentinel System Driver is properly installed and a license meter is attached to the parallel port or USB port."
#define VL10N_COM_ERR_FEW_LIC_METERS_STR         "Too few units (Normal License Count=%d/ Trial License Count= %d) left in your Sentinel RMS Development Kit license meter(s) to generate requested license. %d units required."
#define VL10N_COM_ERR_METERS_CORRUPTED_STR       "Your Sentinel RMS Development Kit license meter(s) are corrupted."
#define VL10N_COM_ERR_INVALID_METER_VERSN_STR    "Your Sentinel RMS Development Kit license meter has an invalid version (%d.%d). Expected %d.%d."
#define VL10N_COM_ERR_ALL_UNITS_USED_STR         "All %d units of your Sentinel RMS Development Kit license meter(s) have been used up. License generation will fail."
#define VL10N_COM_ERR_ACCESS_SERVER_01_STR       "Unknown exception (%d) in accessing Sentinel RMS Development Kit portable server(s)."
#define VL10N_COM_ERR_ACCESS_SERVER_02_STR       "Error accessing Sentinel RMS Development Kit portable server(s). Please make sure one is attached."
#define VL10N_COM_ERR_INVALID_SERVER_VERSN_STR   "Your Sentinel RMS Development Kit portable server has an invalid version (%d.%d). Expected %d.%d."
#define VL10N_COM_ERR_PORTBL_SRVR_CORRUPT_STR    "Your Sentinel RMS Development Kit portable server(s) are corrupted."
#define VL10N_COM_ERR_LIC_FILE_EXPIRED_STR       "Your SW License File Has Expired."
#define VL10N_COM_ERR_INVALID_LIC_TYPE_STR       "Invalid License Type."
#define VL10N_COM_ERR_INVALID_TRIAL_DAYS_STR     "Invalid Trial Days."
#define VL10N_COM_ERR_INVALID_TRIAL_LIC_COUNT_STR "Invalid Trial License Count."
#define VL10N_COM_INF_ALL_UNITS_USED_UP_STR      "All units of your Sentinel RMS Development Kit Trial license meter(s) have been used up."
#define VL10N_COM_INF_LIC_MTR_DECREM_02_STR      "Your Sentinel RMS Development Kit Trial license meter(s) have been decremented by %d units. You now have %d units left."
#define VL10N_COM_ERR_NO_AUTH_LICGEN_STR         "Your Sentinel RMS Development Kit license meter(s) have No authorization to generate Network Licenses."
#define VL10N_COM_INF_START_CMDLNMODE_STR        "Starting in command line mode\n"
#define VL10N_COM_ERR_NO_ENABLE_FEATURE          "No feature is enabled"
#define VL10N_COM_ERR_VI18N_INITIALIZE_FAIL      "Error in initializing locale"
#define VL10N_COM_ERR_INVALID_NUM_SERVERS        "Invalid number of servers"
#define VL10N_COM_ERR_TERMINAL_SERVER_ERR        "Operating in stand-alone mode using terminal client. This is not allowed by the vendor.\n"
#define VL10N_COM_ERR_RDP_SESSION_FOUND          "Operating in stand-alone mode using remote desktop session. This is not allowed by the vendor.\n"
#define VL10N_COM_ERR_OLD_SERVER                 "The requested operation is not supported on this license server.\n"
#define VL10N_COMM_ERR_MARKED_FOR_DEL            "This feature exists on this machine and it is already marked for check-in.\n"
#define VL10N_COM_ERR_METER_NOT_SUPPORTED_STR    "Your Sentinel RMS Development Kit license meter is not supported."
#define VL10N_COM_ERR_INCORRECT_BASE_FEATURE_STR "%s : The base feature is too high for supporting desired number of multi-features. The maximum value should be %d"
#define VL10N_COM_ERR_MIN_SHORT_NUM_LEN_STR      "Feature name must contain two numeric digits"

/* COMMON CHARACTERS - Acronym [CHR] */
/* will be removed at the time of coding as this requires
change in the logic of the code */
#define VL10N_CHR_LBL_YES_01_STR          'Y'
#define VL10N_CHR_LBL_YES_02_STR          'y'
#define VL10N_CHR_LBL_NO_01_STR           'N'
#define VL10N_CHR_LBL_NO_02_STR           'n'

/* LSMON STRINGS - Acronym [LSM] */


#define VL10N_LSM_LBL_FEATURE_NAME_STR    "Feature name                   : \"%s\"  \t"
#define VL10N_LSM_LBL_SRV_STATUS_STR      "Redundant server status        :"
#define VL10N_LSM_HDR_DIST_CRIT_01_STR    " %36s :  Units           Units        Units"
#define VL10N_LSM_HDR_DIST_CRIT_02_STR    "Server Name/Address"
#define VL10N_LSM_HDR_DIST_CRIT_03_STR    "\n%36s     Allocation      Issued    Commuter"
#define VL10N_LSM_ERR_LSHOST_01_STR       " Server name not set, or monitor cannot talk to the server."
#define VL10N_LSM_ERR_LSHOST_02_STR       " Try setting the LSHOST environment variable."
#define VL10N_LSM_HDR_LSM_STR             "  %s %s Application Monitor"
#define VL10N_LSM_HDR_CONTACT_SRV_STR     " [Contacting %s server on host \"%s\"]\n"
#define VL10N_LSM_HDR_NO_LICENSE_STR      "There is no license in the server %s"
#define VL10N_LSM_ERR_CONTACT_SRV_STR     " Unable to contact any servers. Probably no servers running on this subnet."
#define VL10N_LSM_HDR_DIST_CRIT_STR       "The Distribution Criteria is   :\n"

#define VL10N_RCOMMUTE_HDR_STR             "  %s %s Remote Commuter License Utility \n"
#define VL10N_LCOMMUTE_HDR_STR             "  %s %s Commuter License Utility \n"

#define VL10N_LSM_INF_LIC_TRAIL_STR       "Trial License"
#define VL10N_LSM_LBL_TRIAL_PERIOD_STR    "Trial period                   : %d"
#define VL10N_LSM_LBL_TRIAL_PERIOD_LIC_STR    "Trial period                   : %d"
#define VL10N_LSM_INF_LIC_NORMAL_STR      "Normal License"
#define VL10N_LSM_INF_LIC_PERPETUAL_STR   "Perpetual License"
#define VL10N_LSM_INF_LIC_REPOSITORY_STR  "Repository License"
#define VL10N_LSM_INF_LIC_GRACE_STR       "Grace License"
#define VL10N_LSM_LBL_LIC_REDUNDANT_STR   "Is redundant license?          : YES"
#define VL10N_LSM_LBL_LIC_COMMUTER_STR    "Commuter license allowed       : YES"
#define VL10N_LSM_LBL_LIC_COMMUTER_01_STR "Commuter license allowed       : NO"
#define VL10N_LSM_LBL_FEATURE_VER_STR     "Feature version                : \"%s\""
#define VL10N_LSM_LBL_CONCURRENCY_01_STR  "Maximum concurrent user(s)     : Unlimited"
#define VL10N_LSM_LBL_CONCURRENCY_02_STR  "Maximum concurrent user(s)     : %u"
#define VL10N_LSM_LBL_KEY_USAGE_01_STR    "Unreserved tokens in use       : %u"
#define VL10N_LSM_LBL_KEY_USAGE_01_1_STR  "Available unreserved           : Unlimited"
#define VL10N_LSM_LBL_KEY_USAGE_02_STR    "Commuter tokens in use         : %u"
#define VL10N_LSM_LBL_KEY_USAGE_03_STR    "Commuter tokens left           : Unlimited"
#define VL10N_LSM_LBL_KEY_USAGE_04_STR    "Commuter tokens left           : %u"
#define VL10N_LSM_LBL_KEY_USAGE_05_STR    "Unreserved tokens in queue     : %u"
#define VL10N_LSM_LBL_KEY_USAGE_06_STR    "Commuter tokens in use         : %u"
#define VL10N_LSM_LBL_KEY_USAGE_07_STR    "Commuter tokens left           : Unlimited"
#define VL10N_LSM_LBL_KEY_USAGE_08_STR    "Commuter tokens left           : %u"
#define VL10N_LSM_LBL_KEY_USAGE_09_STR    "Reserved tokens in use         : %u"
#define VL10N_LSM_LBL_KEY_USAGE_09_1_STR  "Available reserved             : %u"
#define VL10N_LSM_LBL_KEY_USAGE_10_STR    "Reserved tokens in queue       : %d"
#define VL10N_LSM_LBL_KEY_USAGE_11_STR    "Unreserved tokens in use       : %u"
#define VL10N_LSM_LBL_KEY_USAGE_11_1_STR  "Available unreserved           : %u "
#define VL10N_LSM_LBL_SOFTLIMIT_01_STR    "Soft limit on users            : Unlimited"
#define VL10N_LSM_LBL_SOFTLIMIT_02_STR    "Soft limit on users            : %u"
#define VL10N_LSM_LBL_START_DATE_01_STR   "License start date             : "
#define VL10N_LSM_LBL_START_DATE_02_STR   "License is current."
#define VL10N_LSM_LBL_START_DATE_03_STR   "No License start date!"
#define VL10N_LSM_LBL_EXP_DATE_01_STR     "Expiration date                : "
#define VL10N_LSM_LBL_EXP_DATE_02_STR     "License has no expiration"
#define VL10N_LSM_LBL_DEF_REV_DATE_01_STR "Deferred revocation date       : "
#define VL10N_LSM_LBL_DEF_REV_DATE_02_STR "License not marked for deferred revocation"
#define VL10N_LSM_LBL_LIC_ELAN_01_STR     "License Code                   : Elan converted license code."
#define VL10N_LSM_LBL_LIC_ELAN_02_STR     "Conversion Time                : %ld"
#define VL10N_LSM_LBL_LIC_ELAN_03_STR     "Elan License status            : %s."
#define VL10N_LSM_LBL_LFE_01_STR          "Log encryption level           : %d"
#define VL10N_LSM_LBL_LIC_SHR_STR         "Redundant servers              : %d"
#define VL10N_LSM_LBL_MAJ_RULE_STR        "Majority rule                  : ON."
#define VL10N_LSM_LBL_LIC_LOCK_01_STR     "Application-server locking     : Client-and-Server-locked license."
#define VL10N_LSM_LBL_LIC_LOCK_02_STR     "Application-server locking     : Server-locked license."
#define VL10N_LSM_LBL_LIC_LOCK_03_STR     "Application-server locking     : Unlocked license."
#define VL10N_LSM_LBL_LIC_LOCK_04_STR     "Application-server locking     : Client-locked license."
#define VL10N_LSM_LBL_LIC_LOCK_05_STR     "Client locking code            : %s"
#define VL10N_LSM_LBL_LIC_LOCK_06_STR     "Client locking code            : %s"
#define VL10N_LSM_LBL_PUBLIC_VENDOR_INFO_STR     "Public vendor information      : %s"
#define VL10N_LSM_LBL_PRIVATE_VENDOR_INFO_STR    "Private vendor information     : %s"
#define VL10N_LSM_LBL_TIME_TAMPER_YES_STR "Check time tamper              : Yes"
#define VL10N_LSM_LBL_TIME_TAMPER_NO_STR  "Check time tamper              : No"




/*************/
/*Deepa*/
#define VL10N_LSM_LBL_CLIENT_LOCK_STR     "Client #%d locking code         : %s "
#define VL10N_LSM_LBL_SERVER_LOCK_PRIMARY_STR   "Server #%d locking code         : Primary   = %s "
#define VL10N_LSM_LBL_SERVER_LOCK_SECONDARY_STR "Server #%d locking code         : Secondary = %s "

#define VL10N_LSM_LBL_VALID_SUBNET_STR    "Valid on subnet(s)             : %s"
#define VL10N_LSM_LBL_ADDEXCL_01_STR      "Additive/exclusive/aggregate   : Additive license(number of tokens will be added)."
#define VL10N_LSM_LBL_ADDEXCL_02_STR      "Additive/exclusive/aggregate   : Unknown."
#define VL10N_LSM_LBL_ADDEXCL_03_STR      "Additive/exclusive/aggregate   : Exclusive license(overrides additive licenses)."
#define VL10N_LSM_LBL_ADDEXCL_04_STR      "Additive/exclusive/aggregate   : Aggregate license(Additive without restrictions)."

#define VL10N_LSM_LBL_SHARING_01_STR      "Sharing criterion              : User name based sharing."
#define VL10N_LSM_LBL_SHARING_02_STR      "Sharing criterion              : Client host name based sharing."
#define VL10N_LSM_LBL_SHARING_03_STR      "Sharing limit                  : %d"
#define VL10N_LSM_LBL_SHARING_04_STR      "Sharing criterion              : X display name based sharing."
#define VL10N_LSM_LBL_SHARING_06_STR      "Sharing criterion              : Vendor defined criteria based sharing."
#define VL10N_LSM_LBL_HOLDING_01_STR      "Held licenses                  : Allowed, hold time set by application."
#define VL10N_LSM_LBL_HOLDING_02_STR      "Held licenses                  : Allowed, hold time set by license."
#define VL10N_LSM_LBL_HOLDING_03_STR      "Hold time                      : %ld min(s)"

#define VL10N_LSM_LBL_KEYLIFE_01_STR      "Token lifetime (heartbeat)     : Infinite."
#define VL10N_LSM_LBL_KEYLIFE_02_STR      "Token lifetime (heartbeat)     : Unknown."
#define VL10N_LSM_LBL_KEYLIFE_03_STR      "Token lifetime (heartbeat)     : %ld secs (%ld min(s))"
#define VL10N_LSM_LBL_QUEUE_01_STR        "Queue Length                   : %ld"
#define VL10N_LSM_LBL_QUEUE_02_STR        "Tokens required by queue       : %d"

#define VL10N_LSM_HDR_USERS_STR           "Users:"
#define VL10N_LSM_LBL_USR_NAME_STR        "User name                      : %s"
#define VL10N_LSM_LBL_HOST_NAME_STR       "Host name                      : %s"
#define VL10N_LSM_LBL_XDISPLAY_NAME_STR   "X display name                 : %s"
#define VL10N_LSM_LBL_GROUP_NAME_STR      "Group name                     : %s"
#define VL10N_LSM_LBL_HOLD_STATUS_STR     "Status                         : This license is being held."
#define VL10N_LSM_LBL_USE_STATUS_STR      "Status                         : Running since %-25.24s"
#define VL10N_LSM_LBL_NUM_KEYS_STR        "Number of tokens used          : %u"
#define VL10N_LSM_LBL_COMMUTER_04_STR     "Is commuter token              : NO"
#define VL10N_LSM_LBL_COMMUTER_05_STR     "Is commuter token              : YES"
#define VL10N_LSM_LBL_KEYSHR_01_STR       "Token shared by                : %d other user(s)"
#define VL10N_LSM_LBL_KEYSHR_02_STR       "ID for token sharing           : %s"

#define VL10N_LSM_HDR_QUSERS_STR          "Queued Client Information:"
#define VL10N_LSM_LBL_QKEYS_01_STR        "No of tokens requested         : %d"
#define VL10N_LSM_LBL_QKEYS_02_STR        "No of tokens reserved          : %d"
#define VL10N_LSM_LBL_QKEYS_03_STR        "Reserved from default group    : %d"
#define VL10N_LSM_LBL_QKEYS_04_STR        "Reserved from native group     : %d"
#define VL10N_LSM_LBL_QSTATUS_01_STR      "Status                         : Queued since %-25.24s"
#define VL10N_LSM_LBL_QSTATUS_02_STR      "License granted, at time       : "

#define VL10N_LSM_ERR_USAGE_STR           "Use %s <hostname> or set the LSHOST environment variable."
#define VL10N_LSM_INF_NOFEATURE_STR       "\n No licensed features."
#define VL10N_LSM_LBL_USAGE_STR           "Usage: %s <filename>"

#define VL11N_LSM_LBL_TRIAL_ELAPSED_HOURS_STR         "Trial elapsed hours            : %d"
#define VL11N_LSM_LBL_TRIAL_EXECUTION_COUNT_STR       "Trial execution count          : %d"
#define VL11N_LSM_LBL_TRIAL_DAYS_STR                  "Trial days                     : %d"
#define VL11N_LSM_LBL_TRIAL_CALENDAR_PERIOD_LEFT_STR  "Trial calendar period left     : %d"
#define VL11N_LSM_LBL_TRIAL_ELAPSED_PERIOD_LEFT_STR   "Trial elapsed period left      : %d"
#define VL11N_LSM_LBL_TRIAL_EXECUTIONS_LEFT_STR       "Trial executions left          : %d"
#define VL11N_LSM_LBL_TRIAL_CURRENT_STATUS_STR        "Trial current status           : %s"
#define VL11N_LSM_LBL_LICENSE_STORAGE_STR             "License storage name           : %s"
#define VL11N_LSM_LBL_LICENSE_STATUS_STR              "License status                 : %s"
#define VL11N_LSM_LBL_LICENSE_PRECEDENCE_STR          "License precedence             : %d"
#define VL10N_LSM_LBL_LIC_HASH_STR                    "License Hash                   : %s"

#define VL11N_LSM_LBL_LICENSE_BEGIN_STR               "License:"
#define VL11N_LSM_LBL_USER_STR                        "Client Information"
#define VL11N_LSM_LBL_LICENSE_STR                     "License Information"
#define VL11N_LSM_LBL_UNUSED_STR                      "Unused"
#define VL11N_LSM_LBL_INACTIVE_STR                    "Inactive"
#define VL11N_LSM_LBL_ACTIVE_STR                      "Active"
#define VL11N_LSM_LBL_UNKNOW_STR                      "Unknow"
#define VL11N_LSM_LBL_EXHAUSTED_STR                   "Exhausted"
#define VL11N_LSM_LBL_FEATURE_STR                     "Feature Information"

/* Capacity licensing. */
#define VL10N_LSM_LBL_CAPACITY_TYPE_STR            "Capacity type                  : "
#define VL10N_LSM_INF_CAPACITY_NON_POOLED_STR      "Non-Pooled"
#define VL10N_LSM_INF_CAPACITY_POOLED_STR          "Pooled"
#define VL10N_LSM_LBL_TOKEN_CAPACITY_STR           "Token Capacity                 : "
#define VL10N_LSM_LBL_POOLED_CAPACITY_STR          "Pooled Capacity                : "
#define VL10N_LSM_LBL_MAX_TEAM_CONCURRENCY_01_STR  "Max concurrent team(s)         : Infinite."
#define VL10N_LSM_LBL_MAX_TEAM_CONCURRENCY_02_STR  "Max concurrent team(s)         : %u"

#define VL10N_LSM_LBL_POOL_CAPACITY_AVAILABLE_STR  "Pooled capacity available      : "

#define VL10N_LSM_LBL_USERS_PER_TEAM_01_STR        "Users per team                 : %d"
#define VL10N_LSM_LBL_USERS_PER_TEAM_02_STR        "Users per team                 : 1"
#define VL10N_LSM_LBL_USERS_PER_TEAM_03_STR        "Users per team                 : Infinite."

#define VL10N_LSM_LBL_SOFTLIMIT_TEAM_01_STR        "Soft limit on teams            : Infinite."
#define VL10N_LSM_LBL_SOFTLIMIT_TEAM_02_STR        "Soft limit on teams            : %d"

#define VL10N_LSM_LBL_TEAM_ID_STR                  "Team Id                        : %s"
#define VL10N_LSM_LBL_TEAM_CAP_ALLOC_STR           "Team capacity                  : %lu"
#define VL10N_LSM_LBL_TEAM_CAP_ALLOC_INFINITE_STR  "Team capacity                  : Infinite."
#define VL10N_LSM_LBL_USER_CAP_IN_USE_STR          "User capacity                  : %u"
#define VL10N_LSM_LBL_USER_CAP_IN_USE_INFINITE_STR "User capacity                  : Infinite."
#define VL10N_LSM_LBL_TEAM_CAP_AVAILABLE_STR       "Team capacity available        : %u"
#define VL10N_LSM_LBL_TEAM_CAP_AVAILABLE_INFINITR_STR "Team capacity available        : Infnite."

#define VL10N_LSM_INF_INFINITE_STR                 "Infinite"
#define VL10N_LSM_LBL_LIC_TYPE_STR                 "License type                   : \"%s\" "

#define VL10N_LSM_LBL_TEAM_CRITERIA_01_STR  "Team criterion                 : User name based."
#define VL10N_LSM_LBL_TEAM_CRITERIA_02_STR  "Team criterion                 : Client host name based."
#define VL10N_LSM_LBL_TEAM_CRITERIA_03_STR  "Team criterion                 : X display name based."
#define VL10N_LSM_LBL_TEAM_CRITERIA_04_STR  "Team criterion                 : Vendor defined criteria based."

#define VL10N_LSM_LBL_TOTAL_RESERVED_CAPACITY_STR              "Total reserved capacity        : %lu"
#define VL10N_LSM_LBL_TOTAL_RESERVED_CAPACITY_INFINITE_STR     "Total reserved capacity        : Infinite."

#define VL10N_LSM_LBL_AVAILABLE_RESERVED_CAPACITY_STR            "Available reserved capacity    : %lu"
#define VL10N_LSM_LBL_AVAILABLE_RESERVED_CAPACITY_INFINITE_STR   "Available reserved capacity    : Infinite."

#define VL10N_LSM_LBL_AVAILABLE_UNRESERVED_CAPACITY_STR          "Available unreserved capacity  : %lu"
#define VL10N_LSM_LBL_AVAILABLE_UNRESERVED_CAPACITY_INFINITE_STR "Available unreserved capacity  : Infinite."

#define VL10N_LSM_LBL_TEAM_CAP_RESERVED_STR                            "Team capacity reserved         : %lu"
#define VL10N_LSM_LBL_TEAM_CAP_RESERVED_INFINITE_STR                   "Team capacity reserved         : Infinite."
#define VL10N_LSM_LBL_RESERVED_TEAM_CAP_RESERVED_IN_USE_STR            "Reserved team capacity in use  : %lu"
#define VL10N_LSM_LBL_RESERVED_TEAM_CAP_RESERVED_IN_USE_INFINITE_STR   "Reserved team capacity in use  : Infinite."
#define VL10N_LSM_LBL_UNRESERVED_TEAM_CAP_RESERVED_IN_USE_STR          "Unreserved team capacity in use: %lu"
#define VL10N_LSM_LBL_UNRESERVED_TEAM_CAP_RESERVED_IN_USE_INFINITE_STR "Unreserved team capacity in use: Infinite."

#define VL10N_LSM_LBL_USER_CAP_FROM_RESERVED_STR                 "User capacity from reserved    : %lu"
#define VL10N_LSM_LBL_USER_CAP_FROM_RESERVED_INFINITE_STR        "User capacity from reserved    : Infinite."
#define VL10N_LSM_LBL_USER_CAP_FROM_UNRESERVED_STR               "User capacity from unreserved  : %lu"
#define VL10N_LSM_LBL_USER_CAP_FROM_UNRESERVED_INFINITE_STR      "User capacity from unreserved  : Infinite."

#define VL10N_LSM_LBL_TOTAL_TEAM_TOKENS_RESERVED_STR             "Total team tokens reserved     : %d"
#define VL10N_LSM_LBL_TOTAL_TEAM_TOKENS_RESERVED_INFINITE_STR    "Total team tokens reserved     : Infinite."

#define VL10N_LSM_LBL_RESERVED_TEAM_TOKENS_IN_USE_STR            "Reserved team tokens in use    : %d"
#define VL10N_LSM_LBL_RESERVED_TEAM_TOKENS_IN_USE_INFINITE_STR   "Reserved team tokens in use    : Infinite."

#define VL10N_LSM_LBL_UNRESERVED_TEAM_TOKENS_IN_USE_STR          "Unreserved team tokens in use  : %d"
#define VL10N_LSM_LBL_UNRESERVED_TEAM_TOKENS_IN_USE_INFINITE_STR "Unreserved team tokens in use  : Infinite."

/*04-Sep-2004: LM800, Grace Period and Max_Commuter_Days*/
#define VL10N_LSM_LBL_GRACE_ALLOWED_STR                          "Is grace period allowed        : YES"
#define VL10N_LSM_LBL_GRACE_NOT_ALLOWED_STR                      "Is grace period allowed        : NO"
#define VL10N_LSM_LBL_GRACE_DAYS_STR                             "Grace period days allowed      : %ld"
#define VL10N_LSM_LBL_GRACE_HOURS_STR                            "Grace period hours allowed     : %ld"
#define VL10N_LSM_LBL_MAX_CHECKOUTDAYS_STR                       "Maximum commuter checkout days : %ld days"
#define VL10N_LSM_LBL_MAX_CHECKOUTDAYS_INFINITE_STR              "Maximum commuter checkout days : Restricted to license expiry date"
#define VL10N_LSM_LBL_OVERDRAFT_ALLOWED_STR                      "Is overdraft allowed           : YES"
#define VL10N_LSM_LBL_OVERDRAFT_HOURS_STR                        "Maximum overdraft hours        : %ld"

#define VL10N_LSM_LBL_UNLIMITED_OVERDRAFT_HOURS_STR              "Maximum overdraft hours        : Unlimited"
#define VL10N_LSM_LBL_OVERDRAFT_USERS_STR                        "Maximum overdraft users        : %ld"
#define VL10N_LSM_LBL_UNLIMITED_OVERDRAFT_USERS_STR              "Maximum overdraft users        : Unlimited"
#define VL10N_LSM_LBL_OVERDRAFT_USERS_IN_USE_STR                 "Number of overdraft users      : %ld"

/*15-Oct-2004: LM800, Actual Commuter Checkout Days */
#define VL10N_LSM_LBL_COMMUTER_CHECKOUTDAYS_01_STR               "Commuter checkout days         : Restricted to license expiry date"
#define VL10N_LSM_LBL_COMMUTER_CHECKOUTDAYS_02_STR               "Commuter checkout days         : %d days"

/*20-Oct-2004: LM800, Local Request Locking Criteria info */
#define VL10N_LSM_LBL_LOCAL_REQ_LOCKREQ_STR    "Local request must lock to     : "
#define VL10N_LSM_LBL_LOCAL_REQ_LOCKFLOAT_STR  "Local request may lock to      : "
#define VL10N_LSM_LBL_LOCAL_REQ_LOCKTOTAL_STR  "Local request lock items       : %d (required & float)"
#define VL10N_LSM_LBL_LOCAL_REQ_LOCKNONE_STR   "None specified"
#define VL10N_LSM_LBL_SPACE_STR                "                                : "

/* License Version */
#define VL10N_LSM_LBL_LIC_VER_STR              "License Version                : "
#define VL10N_LSM_LBL_LIC_VER_TOO_OLD_STR      "Older than Sentinel RMS Development Kit 7.0.0"
#define VL10N_LSM_LBL_LIC_VER_VALUE_STR        "0x%08X"
#define VL10N_LSM_LBL_LIC_VER_NOT_DEFINED_STR  "Not defined"

/*Virtual Machine Detection for lsmon*/
#define VL10N_LSM_LBL_VM_ALLOWED_STR           "Allowed on VM                  : YES"
#define VL10N_LSM_LBL_VM_DISALLOWED_STR        "Allowed on VM                  : NO"

/*Deferred Revoke related strings for lsmon*/
#define VL10N_LSM_LBL_MARKED_FOR_REVOKE_STR       "Marked for deferred revocation : YES"
#define VL10N_LSM_LBL_NOT_MARKED_FOR_REVOKE_STR   "Marked for deferred revocation : NO"


/* Client Library STRINGS - Acronym [CLL] */
#define VL10N_CLL_ERR_LICSYS_AVBL_STR         "Licensing System not available.\n"
#define VL10N_CLL_ERR_LIC_EXPIRED_01_STR      "License server has freed up the resources for feature %s. Need more frequent token renewal??\n"
#define VL10N_CLL_ERR_NOLIC_STR               "No license code is available for feature %s on host %s.\n"
#define VL10N_CLL_ERR_VENDORID_01_STR         "Feature %s is licensed by a different vendor.\n"
#define VL10N_CLL_ERR_VENDORID_02_STR         "Feature %s is licensed by multiple vendors.\n"
#define VL10N_CLL_ERR_LICNOTAVBL_STR          "All licensing tokens with server %s for feature %s are already in use.\n"
#define VL10N_CLL_ERR_NORESOURCES_STR         "Insufficient system resources are available to complete the request.\n"
#define VL10N_CLL_ERR_LIC_NOT_PROCESSED_STR   "The given license code could not be processed.\n"
#define VL10N_CLL_ERR_SMALL_BUFF_STR          "Input buffer too small, string may be truncated.\n"
#define VL10N_CLL_ERR_NO_NETWORK_STR          "Unable to talk to host \"%s\".  Communication problems??\n"
#define VL10N_CLL_ERR_BAD_INDEX_STR           "Bad Index.\n"
#define VL10N_CLL_ERR_LIC_EXPIRED_02_STR      "Feature \"%s\" cannot run anymore because the license expiration date has reached.Check your machine's date, then contact your software source.\n"
#define VL10N_CLL_ERR_NO_LICKEY_01_STR        "Unable to obtain licensing token for feature %s.\n"
#define VL10N_CLL_ERR_FEAT_01_STR             "Feature name or version cannot be NULL.\n"
#define VL10N_CLL_ERR_FEAT_02_STR             "Feature \"%s\" is not licensed to run on this machine due to server/client lock-code mismatch.\n"
#define VL10N_CLL_ERR_NO_SRV_01_STR           "Cannot talk to the license server on host \"%s\". Server not running??\n"
#define VL10N_CLL_ERR_NO_SRV_02_STR           "Failed to resolve the server host \"%s\".\n"
#define VL10N_CLL_ERR_NO_SRV_03_STR           "License server on host \"%s\" is not responding.\n"
#define VL10N_CLL_ERR_NO_LICKEY_02_STR        "Attempt to return a non-existent token for feature %s.\n"
#define VL10N_CLL_ERR_INTERNAL_01_STR         "Internal error in licensing or accessing feature %s.\n"
#define VL10N_CLL_ERR_SRV_HOSTNM_STR          "Failed to figure out the license server correctly. Set environment variable LSHOST to (tilde-separated) name(s) of server(s).\n"
#define VL10N_CLL_ERR_FEAT_03_STR             "Cannot return the token for feature %s.\n"
#define VL10N_CLL_ERR_FUNCT_CALL_STR          "Error in calling API function. Check the calling parameters.\n"
#define VL10N_CLL_ERR_INTERNAL_02_STR         "Severe internal error in licensing or accessing feature %s.\n"
#define VL10N_CLL_ERR_FEAT_04_STR             "This user/machine has been excluded from accessing feature %s.\n"
#define VL10N_CLL_ERR_FEAT_05_STR             "Unknown shared ID specified for feature %s.\n"
#define VL10N_CLL_ERR_NO_SRV_04_STR           "Probably no servers are running on this subnet.\n"
#define VL10N_CLL_ERR_LICADD_FAIL_STR         "The given license code could not be added to the \"%s\" license server.\n"
#define VL10N_CLL_ERR_INVALID_LIC_FAIL_STR    "The given license code is invalid. Hence, it could not be added to the \"%s\" license server.\n"
#define VL10N_CLL_ERR_DUPLICATE_LIC_FAIL_STR  "The given license code is already added to the \"%s\" license server.\n"
#define VL10N_CLL_ERR_FEAT_06_STR             "Failed to delete feature %s from the license server on host %s.\n"
#define VL10N_CLL_ERR_NOLICRESORCES_STR       "Could not locate enough licensing resources to license feature %s.\n"
#define VL10N_CLL_ERR_NOADDITIONAL_UNITS_STR  "No additional units are available for feature %s.\n"
#define VL10N_CLL_ERR_SRV_MSG_STR             "Could not understand the message received from license server on host \"%s\". Client-server version mismatch?\n"
#define VL10N_CLL_ERR_DIFF_LIB_VER_STR        "The license server fails to identify the client application version.\n"
#define VL10N_CLL_ERR_CLK_TAMPER_STR          "Request denied due to clock tamper detection.\n"
#define VL10N_CLL_ERR_UNAUTHORIZED_STR        "You are not authorized to perform the requested operation.\n"
#define VL10N_CLL_ERR_INVALID_DOMAIN_STR      "Cannot perform this operation on the domain name specified.\n"
#define VL10N_CLL_ERR_LOGFILE_01_STR          "The specified log filename not found on License Server \"%s\".\n"
#define VL10N_CLL_ERR_LOGFILE_02_STR          "Cannot change specified log filename on License Server \"%s\".\n"
#define VL10N_CLL_ERR_FNGPRN_MISMATCH_STR     "Machine's Fingerprint Mismatch for feature %s.\n"
#define VL10N_CLL_ERR_TRIALLIC_EXHAUST_STR    "Duration of Trial License of %s is exhausted.\n"
#define VL10N_CLL_ERR_TRIALLIC_DATA_ACCESS        "The Trial feature  \"%s\" is not accessible.\n"
#define VL10N_CLL_ERR_TRIALLIC_DATA_INCONSISTENT  "Trial License data inconsistent for feature %s.\n"
#define VL10N_CLL_ERR_TRIALLIC_DATE_RESTRICTION   "Trial license date restriction error for feature %s.\n"
#define VL10N_CLL_ERR_TRIALLIC_NOT_ACTIVATED  "The Trial licensing for feature \"%s\" is disabled.\n"
#define VL10N_CLL_INF_NOUPDATES_STR           "No Updates have taken place for feature %s so far.\n"
#define VL10N_CLL_INF_KEY_RET_STR             "Returned all the tokens for feature %s.\n"
#define VL10N_CLL_ERR_QREQ_DEN_STR            "Queued request denied for \"%s\" since the queue is already full.\n"
#define VL10N_CLL_ERR_NO_CLIENT_STR           "Could not find the specified client for feature %s.\n"
#define VL10N_CLL_ERR_CL_NOTAUTHORIZED_STR    "Client not authorized for the specified action for feature %s.\n"
#define VL10N_CLL_ERR_DIST_CRIT_STR           "Distribution criteria given is not correct.\n"
#define VL10N_CLL_ERR_LEAD_UNKNOWN_STR        "Current leader server of redundant server pool is not known.\n"
#define VL10N_CLL_ERR_SRV_EXISTS_01_STR       "Server already exists in the redundant server pool.\n"
#define VL10N_CLL_ERR_SRV_EXISTS_02_STR       "The given server name does not exist in the redundant server pool.\n"
#define VL10N_CLL_ERR_BAD_HOSTNAME_STR        "The specified host name is either not valid or cannot be resolved.\n"
#define VL10N_CLL_ERR_NON_REDUNDANT_SRV_STR   "A non-redundant server contacted for redundant server related information.\n"
#define VL10N_CLL_INF_SRV_SYNC_01_STR         "Server synchronization in progress. Please wait...\n"
#define VL10N_CLL_INF_MSG_FORWARD_STR         "Message forwarded to the leader server.\n"
#define VL10N_CLL_ERR_UPDATE_FAIL_STR         "Update Failure. Contact another fail-over server.\n"
#define VL10N_CLL_UNRESOLVED_IPADD_STR        "IP address given cannot be resolved.\n"
#define VL10N_CLL_UNRESOLVED_HOSTNAME_STR     "Hostname given is unresolved.\n"
#define VL10N_CLL_INVALID_IPADD_STR           "Invalid IP address format.\n"
#define VL10N_CLL_INF_SRV_SYNC_02_STR         "Server is synchronizing the distribution table.\n"
#define VL10N_CLL_INF_SRVPOOL_FULL_STR        "The redundant server pool already has the maximum number of servers. No more servers can be added.\n"
#define VL10N_CLL_INF_SRVPOOL_EMPTY_STR       "The redundant server pool has only one server. It cannot be deleted.\n"
#define VL10N_CLL_ERR_FEATURE_INACTIVE_STR    "The feature %s is either unavailable on the server or server is non-redundant.\n"
#define VL10N_CLL_INF_MAJRULE_FAIL_STR        "The token for feature %s cannot be issued because of majority rule failure.\n"
#define VL10N_CLL_ERR_CONFIG_FILE_STR         "Configuration file modifications failed. Check the given parameters.\n"
#define VL10N_CLL_ERR_NON_REDUNDANT_FEATURE_STR "A non-redundant feature given for redundant feature related operation.\n"
#define VL10N_CLL_ERR_COMM_CODE_01_STR        "Commuter code for feature \"%s\" does not exist on the client system.\n"
#define VL10N_CLL_ERR_CL_EXISTS_STR           "Client already exists on server \"%s\".\n"
#define VL10N_CLL_ERR_COMM_INFO_STR           "Failed to get client commuter info on server \"no-net\".\n"
#define VL10N_CLL_ERR_COMM_UNINST_STR         "Unable to uninstall the client commuter license \"%s\".\n"
#define VL10N_CLL_ERR_COMM_ISSUE_STR          "Unable to issue a commuter license to client \"%s\".\n"
#define VL10N_CLL_ERR_NON_COMMUTER_LICENSE_STR   "A non-commuter license is requested for commuter related operation by client \"%s\".\n"
#define VL10N_CLL_ERR_NO_COMM_KEYS_STR        "Not enough commuter tokens available on server \"%s\".\n"
#define VL10N_CLL_ERR_COMM_CLINFO_STR         "Invalid commuter info from Client.\n"
#define VL10N_CLL_ERR_COMM_CODE_02_STR        "Client commuter license does not exist \"no-net\". \n"
#define VL10N_CLL_ERR_COMM_CODE_03_STR        "Client commuter license already exists on  \"no-net\". \n"
/*to indicate commuter expiration is greater than license itself -SourabhB*/
#define VL10N_CLL_ERR_COMM_CODE_04_STR        "Cannot issue commuter code. \nLicense expiration is less than the requested days for commuter code. \n"
#define VL10N_CLL_ERR_NO_APPROPRIATE_LIB_STR  "Feature is not supported in Network-only mode of library.\n"
#define VL10N_CLL_ERR_INVALID_FILETYPE        "The specified file type is not supported.\n"

#define VL10N_CLL_ERR_REM_COMM_LOCK_STR       "Error getting the locking information for the client.\n"
#define VL10N_CLL_ERR_REM_COMM_AUTH_STR       "Error installing the commuter authorization code.\n"
#define VL10N_CLL_ERR_INVLAID_COMM_LOCK_STR   "Invalid locking code string.\n"
#define VL10N_CLL_ERR_EXPIRED_COMMUTER_CODE_STR   "Error installing the commuter code, as the code is expired.\n"
#define VL10N_CLL_ERR_COMMUTER_CODE_DATE_RESTRICTED_STR "Commuter code start date not yet reached.\n"
#define VL10N_CLL_ERR_FINGERPRINT_VALUE_NOT_FOUND_STR   "No fingerprint information is available on the specified index.\n"
#define VL10N_CLL_ERR_COMMUTER_CHECKOUT_STR   "A commuter token has already been checked out for this license.\n"
#define VL10N_CLL_ERR_PERPETUAL_LICENSE_STR   "The contacted feature is a Repository License.\n"
#define VL10N_CLL_ERR_UPGRADE_NOT_ALLOWED_STR "License upgrade feature is not allowed on the specified license type.\n"
#define VL10N_CLL_ERR_AMBIGUOUS_HANDLE_STR    "Ambiguous client handle! The specified client handle is not associated with any request.\n"
#define VL10N_CLL_ERR_ACTIVE_HANDLE_STR       "The specified client handle is associated with an active request.\n"
#define VL10N_CLL_ERR_QUEUED_HANDLE_STR       "The specified client handle is associated with a queued request.\n"
#define VL10N_ERR_BAD_HANDLE_STR              "Client handle refers to an invalid licensing system context.\n"
#define VL10N_ERR_NO_SUCCESS_STR              "Failed in performing the requisite operation.\n"

#define VL10N_CLL_ERR_ERROR_READING_SERVER_CONFIG_FILE    "Error retrieving data from server config (lservrlf) file. Cannot issue commuter token from redundant server.\n"
#define VL10N_CLL_ERR_COMM_CHKOUT_RESTR_NONPRIMARY_LEADER "Cannot checkout commuter token from non-primary leader server in redundant setup.\n"

/* Capacity licensing
 * SLM 7.3.0
 */
#define VL10NN_CLL_ERR_NO_USER_CAPACITY_STR   "Insufficient user capacity available.\n"
#define VL10NN_CLL_ERR_NO_TEAM_CAPACITY_STR   "Insufficient team capacity available.\n"
#define VL10NN_CLL_ERR_TEAM_LIMIT_EXHAUSTED_STR   "Team limit exhausted.\n"
#define VL10NN_CLL_ERR_TEAM_EXCLUDED_STR             "This team has been excluded from accessing feature %s.\n"
#define VL10N_CLL_ERR_CAPACITY_MISMATCH_STR   "The capacity value does not match, or an operation related to capacity licensing is requested for a non-capacity license (or vice-versa).\n"

#define VL10N_CLL_ERR_NETOWRK_SRV_STR      "A network server contacted for standalone server related information.\n"
#define VL10N_CLL_ERR_STANDALONE_SRV_STR   "A standalone server contacted for network server related information.\n"

#define VL10N_LFE_ERR_LOG_FILE_STR            " %s error: Invalid value %ld for usage-log-file-size.\n Minimum usage-log-file-size should be %d.\n"

/* Grace Period License Related Errors */
#define VL10N_CLL_ERR_GRACE_EXPIRED_STR         "Feature %s cannot run as total number of days allowed in grace period have been consumed.\n"
#define VL10N_CLL_ERR_GRACE_HOURS_EXHAUSTED_STR "Feature %s cannot run as total number of hours allowed in grace period have been consumed.\n"
#define VL10N_CLL_ERR_GRACE_INVALID_STATE_STR   "Feature %s cannot run as an unexpected state of grace period is found.\n"
#define VL10N_CLL_ERR_GRACE_LIC_INSTALL_FAIL_STR    "Feature %s cannot run as installation of grace license is failed.\n"
#define VL10N_CLL_ERR_GRACE_TIME_TAMPER_INIT_FAIL_STR    "Time tamper initialization failed for Feature %s.\n"

/*Virtual Machine Error Code*/
#define VL10N_CLL_ERR_VIRT_MACH_DETECTED_STR    "Cannot fulfill request because running on virtual machine.\n"

#define VL10N_CLL_ERR_REVOKE_LIC_DATA_INCONSISTENT_STR "Revoke license records are in inconsistent state"
#define VL10N_CLL_ERR_COMMUTER_CHECKOUT_NOT_ALLOWED_STR "Commuter code checkout not allowed for this feature."
#define VL10N_CLL_ERR_NO_COMM_KEYS_FOR_DURATION_STR          "Not Enough commuter tokens available for the specified duration on server \"%s\".\n"


#define VL10N_CLL_ERR_OP_NOT_ALLOWED_AFTER_DEFERRED_REVOCATION "Commuter/repository checkout not allowed after deferred revocation operation is performed."
#define VL10N_CLL_ERR_DEFERRED_REVOCATION_NOT_ALLOWED "Deferred revocation not allowed because a commuter/repository token is already in use."
#define VL10N_CLL_ERR_NON_INTENDED_SERVER_CONTACTED "Non intended server contacted for this client."
#define VL10N_CLL_ERR_OPERATION_NOT_ALLOWED_ON_FOLLWER "Operation not allowed on follower server in redundant config"
#define VL10N_CLL_ERR_LICENSE_UPGRADE_DENIED "Insufficient tokens available at server to increase during license update."
#define VL10N_CLL_ERR_CLEAN_KEYINFO_NO_KEYS_EXISTS "No keys present for cleaning.\n"
#define VL10N_CLL_ERR_CLEAN_KEYINFO_FAILED         "Deletion of keys failed on server.\n"
#define VL10N_CLL_ERR_CLEAN_KEYINFO_SUCC_EXCL_COMM "Keys excluding commuter deleted successfully.\n"
#define VL10N_CLL_ERR_ZOMBIE_SESSION_HANDLE         "Input handle is associated to zombie session and is invalid to use in this API.\n"
#define VL10N_CLL_ERR_TERMINATED      "Active session is terminated by LM Server and is invalid to use in this API.\n"

#define VL10N_CLL_ERR_LOWER_SIGNING_KEYINDEX_IN_LIC   "The signing key index of the requested license is lower than the one sent by the client."

#define VL10N_CLL_ERR_SNTL_INVALID_APP_CONTEXT         "Invalid application context."
#define VL10N_CLL_ERR_SNTL_INVALID_ATTR_KEY            "Invalid attribute key."
#define VL10N_CLL_ERR_SNTL_INVALID_ATTR_VALUE          "Invalid attribute key value."
#define VL10N_CLL_ERR_DEPRECATED_ATTR                  "Deprecated attribute"
#define VL10N_CLL_ERR_SNTL_SCC_MACHINE_FINGERPRINT_CHANGED "SCC machine fingerprint changed."
#define VL10N_CLL_ERR_SNTL_SCC_MACHINE_NOT_FOUND       "SCC machine not found."
#define VL10N_CLL_ERR_SNTL_SCC_CUSTOMER_NOT_FOUND      "SCC customer not found."
#define VL10N_CLL_ERR_INCOMPATIBLE_COMMUTER_CODE "The feature being returned was commuted by different mechanism."
#define VL10N_CLL_ERR_COMMUTER_CODE_TOO_OLD "Commuter code issued at %s cannot be installed over commuter code issued at %s."
#define VL10N_CLL_ERR_CANCELLED_LEASE_LICENSE "Lease license is already cancelled."
#define VL10N_CLL_ERR_LEASE_INPROGRESS "Sync lease license is in progress."
#define VL10N_CLL_ERR_CANCELLED_FROM_CLIENT "Lease license is cancelled from client only."
#define VL10N_CLL_ERR_SCC_COMMUNICATION_FAILED "SCC communication failure."

/* End of Client Library */

/* LSPOOL STRINGS - Acronym [LSP] */

#define VL10N_LSP_LBL_SRV_ADMIN_01_STR       "\n  %s %s Server Administration Utility\n"
/* Example: Sentinel RMS Development Kit 7.1.0 Server Administration Utility */

#define VL10N_LSP_LBL_SRV_ADMIN_02_STR       "\n  %s Server Administration Utility\n"

#define VL10N_LSP_ERR_SRV_NAME_01_STR        " Please enter IP/IPX address :"

#define VL10N_LSP_LBL_CURRENT_LEADER_NAME_STR "\n Current leader server is %s \n\n"
#define VL10N_LSP_LBL_CURRENT_LEADER_ADD_STR  "\n Current leader server address is %s\n\n"
#define VL10N_LSP_LBL_SRV_NUM_STR             "\n Servers in the pool are :\n\n"
#define VL10N_LSP_INF_SRV_COMMUTE_STR         "\n Successfully communicated to the server\n\n"

#define VL10N_LSP_HDR_DIST_CRIT_01_STR       "\t    Units     Units     Units\n"
#define VL10N_LSP_HDR_DIST_CRIT_02_STR       "                                   Allocation  Issued   Commuted\n"

#define VL10N_LSP_HDR_POOL_OPT_STR           "\n ------ Pool Related Options ---------------------------------------\n"
/*Fix for RMS-7392 (case 1)*/
#define VL10N_LSP_INF_POOL_OPT_01_STR        " -a <Server_Name> <IP_Address>  [Add a Server to the pool\n"\
                                             "                                (Pass Dyn_IPv4 for Dynamic IPv4 and\n"\
                                             "                                 Dyn_IPv6 for Dynamic IPv6 Address)]\n"
#define VL10N_LSP_INF_POOL_OPT_02_STR        " -d <Server_Name>               [Delete a Server from the pool]\n"
#define VL10N_LSP_INF_POOL_OPT_03_STR        " -l                             [Get pool's Leader Server name]\n"
#define VL10N_LSP_INF_POOL_OPT_04_STR        " -p                             [Get pool's servers list]\n"
#define VL10N_LSP_INF_POOL_OPT_05_STR        " -g <feature><version>          [Get distribution criteria]\n"
#define VL10N_LSP_INF_POOL_OPT_06_STR        " -c <feature><version><server1:num_tokens,server2:num_tokens,...> \n"
#define VL10N_LSP_INF_POOL_OPT_07_STR        "                                [Change Distribution criteria]\n\n"

#define VL10N_LSP_HDR_SRV_OPT_STR            " ----- Single Server Options ---------------------------------------\n"
#define VL10N_LSP_INF_SRV_OPT_01_STR         " -b <feature><version><OFF|ON>  [Disable/Enable borrowing for a feature]\n"
#define VL10N_LSP_INF_SRV_OPT_02_STR         " -B <OFF|ON>                    [Disable/Enable borrowing for all features]\n"
#define VL10N_LSP_INF_SRV_OPT_03_STR         " -L <event><OFF|ON>             [Disable/Enable logging for an event]\n\n"
#define VL10N_LSP_ERR_REENTER_01_STR         "\n Please enter IP address: "

/* End of LSPOOL strings */

/* LSCGEN STRINGS - Acronym [CGN] */

#define VL10N_CGN_INF_MAX_CODEGEN_STR       "Codegen version (%d - %d) or <Enter> for maximum [%d] : "
#define VL10N_CGN_LBL_VENDOR_ID_STR         "\nVendor ID that this license code will be used to enable (1..%d): "
#define VL10N_CGN_CFM_VENDOR_SITE_STR       "Can this vendor generate site licenses ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_SHARED_STR     "Can this vendor generate shared licenses ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_HELD_STR       "Can this vendor generate held licenses ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_ADD_STR        "Can this vendor generate additive licenses ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_NORMAL_STR     "Can this vendor generate normal license ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_COMM_STR       "Can this vendor generate commuter license ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_REDUN_STR      "Can this vendor generate redundant license ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_TRIAL_STR      "Can this vendor generate trial license ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_CAPACITY_STR   "Can this vendor generate capacity license ? (Y/N): "

#define VL10N_CGN_CFM_VENDOR_NETWORK_STR      "Can this vendor generate network license ? (Y/N): "
#define VL10N_CGN_CFM_VENDOR_STANDALONE_STR      "Can this vendor generate standalone license ? (Y/N): "


#define VL10N_CGN_INF_FEAT_VER_01_STR       "\nEnter list of features and their versions for which this\n"
#define VL10N_CGN_INF_FEAT_VER_02_STR       "distributor/reseller can generate license codes.\n"
#define VL10N_CGN_INF_FEAT_VER_03_STR       "If you do not specify any features (by hitting Enter on the\n"
#define VL10N_CGN_INF_FEAT_VER_04_STR       "first prompt) all features will be enabled.\n"
#define VL10N_CGN_INF_FEAT_VER_05_STR       "If you do not specify a version for a feature (e.g. APP),\n"
#define VL10N_CGN_INF_FEAT_VER_06_STR       "all versions of that feature will be enabled.\n"
#define VL10N_CGN_INF_FEAT_VER_07_STR       "Enter Feature%cVersion pairs, one pair per line (e.g. APP%c3.2).\n"
#define VL10N_CGN_INF_FEAT_VER_08_STR       "Press Enter on a line by itself to end the list of pairs.\n"

#define VL10N_CGN_LBL_FEAT_VER_STR          " Feature%cVersion #%d: "

#define VL10N_CGN_HDR_LIC_PROP_STR          "Combining property of licensing code - \n"
#define VL10N_CGN_INF_LIC_PROP_01_STR       "  [%d] - Additive license (number of tokens will get added)\n"
#define VL10N_CGN_INF_LIC_PROP_02_STR       "  [%d] - Exclusive license (will override additive licenses)\n"
#define VL10N_CGN_INF_LIC_PROP_03_STR       "  [%d] - Aggregate license (additive without any restrictions)\n"

#define VL10N_CGN_CFM_ELAN_CODE_STR         "Elan converted code?(Y/N)[Y]:"

#define VL10N_CGN_HDR_CONV_TIME_STR         "Conversion Time"
#define VL10N_CGN_INF_DATE_STR              "\nEnter date(dd/mm/yyyy):"
#define VL10N_CGN_INF_TIME_STR              "\nEnter time(hh:mn:sec):"

#define VL10N_CGN_CFM_LIC_GRANT_01_STR      "Do you want to stop granting licenses when a majority of\n"
#define VL10N_CGN_CFM_LIC_GRANT_02_STR      "redundant servers is not running?(Y/N)[Y]:"
#define VL10N_CGN_CFM_LOCK_SERVERS_STR      "Lock to multiple redundant servers? (Y/N)[Y]: "
#define VL10N_CGN_INF_LOG_FILE_STR          "Enter log file encryption Level(%d - %d)[1]:"
/* Example: Enter log file encryption Level(1 - 4)[1]: */

#define VL10N_CGN_INF_COMMUTER_LIC_STR      "Should Commuter licenses be issued?(Y/N)[Y]: "

#define VL10N_CGN_HDR_LIC_FORMAT_STR        "Desired length and options of licensing code format - \n"
#define VL10N_CGN_INF_LIC_FORMAT_01_STR     "  [%d] - Short code\n"
/* Example: [2]  - Short code */

#define VL10N_CGN_INF_LIC_FORMAT_02_STR     "  [%d] - Long code\n"
#define VL10N_CGN_INF_LIC_FORMAT_03_STR     "  [%d] - Short numeric code\n"

//#define VL10N_CGN_HDR_LIC_CAPACITY_STR        "Desired capacity type of licensing code - \n"
#define VL10N_CGN_HDR_LIC_CAPACITY_STR        "Desired licensing code - \n"
#define VL10N_CGN_INF_LIC_CAPACITY_01_STR     "  [%d] - No capacity\n"
//#define VL10N_CGN_INF_LIC_CAPACITY_01_STR     "  [%d] - Normal\n"
#define VL10N_CGN_INF_LIC_CAPACITY_02_STR     "  [%d] - Non-pooled capacity\n"
#define VL10N_CGN_INF_LIC_CAPACITY_03_STR     "  [%d] - Pooled capacity\n"

#define VL10N_CGN_HDR_LIC_FEATURE_STR       " No of features encoded in the license:\n"
#define VL10N_CGN_INF_LIC_FEATURE_01_STR    "  [%d] - Single feature\n"
#define VL10N_CGN_INF_LIC_FEATURE_02_STR    "  [%d] - Multi  feature\n"

#define VL10N_CGN_INF_LIC_MULTIFEAT_NUM_STR "Enter the number of features for multi feature license :"

#define VL10N_CGN_CFM_TRAIL_LIC_STR         "Do you want to generate trial license? (Y/N) :"
#define VL10N_CGN_LBL_TRIAL_LIC_COUNT_STR   "Available trial license count : %d\n"
#define VL10N_CGN_INF_TRIAL_LIC_COUNT_STR   "Trial Lisence Count is 0. Normal License Count will be decremented\n"
#define VL10N_CGN_LBL_NORMAL_LIC_COUNT_STR  "Available normal license count : %d\n"
#define VL10N_CGN_INF_LIC_COUNT_DECRE_STR   "License count that will be decremented from license meter - %d\n"

#define VL10N_CGN_HDR_CLL_STR               "Please enter the desired client library behavior - \n"
#define VL10N_CGN_INF_CLL_01_STR            "  [%d] - Network licensing\n"
#define VL10N_CGN_INF_CLL_02_STR            "  [%d] - Standalone licensing\n"
#define VL10N_CGN_INF_CLL_03_STR            "  [%d] - Perpetual licensing\n"
#define VL10N_CGN_INF_CLL_04_STR            "  [%d] - Repository licensing\n"

#define VL10N_CGN_HDR_HELD_LIC_STR          "Type of held licenses - \n"
#define VL10N_CGN_INF_HELD_LIC_01_STR       "  [%d] - Not allowed\n"
#define VL10N_CGN_INF_HELD_LIC_02_STR       "  [%d] - Client application specifies hold time\n"
#define VL10N_CGN_INF_HELD_LIC_03_STR       "  [%d] - The licensing code specifies hold time\n"

#define VL10N_CGN_HDR_LOCK_MODE_STR         "Application-server locking mode - \n"
#define VL10N_CGN_INF_LOCK_MODE_01_STR      "  [%d] - Server-locked  (server locked to a host, clients not locked)\n"
#define VL10N_CGN_INF_LOCK_MODE_02_STR      "  [%d] - Client-and-Server-locked (server and clients locked to specific hosts)\n"
#define VL10N_CGN_INF_LOCK_MODE_03_STR      "  [%d] - Unlocked        (server and clients not locked)\n"
#define VL10N_CGN_INF_LOCK_MODE_04_STR      "  [%d] - Any Host Type   (server and clients not locked)\n"
#define VL10N_CGN_INF_LOCK_MODE_05_STR      "  [%d] - Client-locked   (clients locked to specific hosts)\n"

#define VL10N_CGN_HDR_SHARED_LIC_STR        "Type of shared licenses - \n"
#define VL10N_CGN_INF_SHARED_LIC_01_STR     "  [%d] - No sharing of licenses\n"
#define VL10N_CGN_INF_SHARED_LIC_02_STR     "  [%d] - User-name based sharing\n"
#define VL10N_CGN_INF_SHARED_LIC_03_STR     "  [%d] - Hostname based sharing\n"
#define VL10N_CGN_INF_SHARED_LIC_04_STR     "  [%d] - X-display based sharing\n"
#define VL10N_CGN_INF_SHARED_LIC_05_STR     "  [%d] - Application-defined sharing\n"

#define VL10N_CGN_INF_SHARING_LIMIT_STR     "  Sharing limit [1 - %ld] or <Enter> for no limit: "

#define VL10N_CGN_HDR_TEAM_LIC_STR          "Team creation criteria  - \n"
#define VL10N_CGN_INF_TEAM_LIC_01_STR       "  [%d] - No team\n"
#define VL10N_CGN_INF_TEAM_LIC_02_STR       "  [%d] - User-name based team\n"
#define VL10N_CGN_INF_TEAM_LIC_03_STR       "  [%d] - Hostname based team\n"
#define VL10N_CGN_INF_TEAM_LIC_04_STR       "  [%d] - X-display based team\n"
#define VL10N_CGN_INF_TEAM_LIC_05_STR       "  [%d] - Application-defined team\n"

#define VL10N_CGN_INF_TEAM_LIMIT_STR        "  Team limit [1 - %ld] or <Enter> for no limit: "

#define VL10N_CGN_HDR_VENDOR_STR            "Proprietary vendor information (any printable characters)\nThis information will be encrypted in the license string: "
#define VL10N_CGN_HDR_VENDOR_PLAIN_STR      "Proprietary public vendor information (any printable characters)\nThis information will remain readable in the readable output license string: "
#define VL10N_CGN_HDR_LICENSE_VENDOR_STR    "Proprietary license vendor information (any printable characters)\nThis information will be encrypted in the license string: "
#define VL10N_CGN_HDR_VM_DETECT_STR         "Enable VM Detection flag? (Y/N)[N]:"
#define VL10N_CGN_ERR_VM_DETECT_STR         " Action on VM detection       : Do not issue license."
#define VL10N_CGN_ERR_VM_DETECT_01_STR      " Action on VM detection       : Issue license."

#define VL10N_CGN_INF_VENDOR_01_STR         "Proprietary vendor information string (2-9/A-Z): "
#define VL10N_CGN_INF_VENDOR_02_STR         "Proprietary vendor information string (0-9/A-Z (other than O/I)): "

#define VL10N_CGN_INF_FEATURE_NAME_STR      "Feature name (numeric chars & maximum length allowed is two ): "
#define VL10N_CGN_INF_FEATURE_NAME_NEW_STR      "Feature name (numeric chars & maximum length allowed is six ): "
#define VL10N_CGN_INF_FEATURE_NAME_01_STR   "Feature name (any printable characters): "
#define VL10N_CGN_INF_FEATURE_NAME_02_STR   "Feature name (2-9/A-Z): "
#define VL10N_CGN_PRM_FEATURE_NAME_STR      "\n Enter the feature name    : "
#define VL10N_CGN_PRM_FEATURE_NAME_01_STR   "Feature name ? "

#define VL10N_CGN_PRM_FEATURE_VER_STR       "Please enter the feature version : "
#define VL10N_CGN_INF_FEATURE_VER_STR       "Feature version number (any printable characters): "
#define VL10N_CGN_INF_FEATURE_VER_01_STR    "Feature version number (0-9/./A-Z (other than O/I/L)): "

#define VL10N_CGN_LBL_LOCK_SELECTOR_STR     "Lock selector for this machine is 0x%lX\n"
#define VL10N_CGN_INF_LOCK_SELECTOR_STR     "Lock selector for this machine in hexadecimal (if known)"

#define VL10N_CGN_INF_LOCK_CODE_STR         "Locking code for this machine (if known)"
#define VL10N_CGN_LBL_LOCK_CODE_STR         "Locking code for this machine is %s\n"
#define VL10N_CGN_LBL_NOT_ALLOW_LOCK_CODE   "The style lock code isn't allowed for current version license, try again.\n\n"
#define VL10N_CGN_LBL_NOT_SUPPORT_LOCK_CODE "The lock code version isn't supported, try again.\n\n"
#define VL10N_CGN_LBL_NOT_SUPPORT_LOCK_SELECTOR "The lock selector isn't supported for current version license, enter again. \n\n"

#define VL10N_CGN_INF_IDPROM_STR            "ID PROM (Unix workstation hostid) in hexadecimal"
#define VL10N_CGN_INF_IPADD_STR             "IPaddress (such as 204.143.137.220)"
#define VL10N_CGN_INF_IP6ADD_STR            "IPv6 address (such as fe80::213:2ff:fe6d:3575)"
#define VL10N_CGN_INF_DISKID_STR            "Disk ID in hexadecimal"
#define VL10N_CGN_INF_HOSTNAME_STR          "Host Name"
#define VL10N_CGN_INF_ETHERADD_STR          "Ethernet Address (such as 00-23-A4-B8-92-FE)"
#define VL10N_CGN_INF_IPXADD_STR            "NetWare IPX Address in hexadecimal"
#define VL10N_CGN_INF_NW_SNO_STR            "NetWare Serial Number in hexadecimal"
#define VL10N_CGN_INF_SLM_COMPID_STR        "Sentinel Computer ID"
#define VL10N_CGN_INF_HOSTID_STR            "Standard Custom in hexadecimal"
#define VL10N_CGN_INF_CPU_SNO_STR           "CPU Serial Number in hexadecimal"
#define VL10N_CGN_INF_HDD_SERIAL_STR      "Hard Disk Serial"
#define VL10N_CGN_INF_CUSTOMEX_STR          "Extended Custom in hexadecimal"
#define VL10N_CGN_INF_CPU_INFO_STR           "CPU Info String"
#define VL10N_CGN_INF_UUID_STR               "UUID"

#define VL10N_CGN_ERR_LOCK_INF_STR          "You must specify non-zero lock information!  Try again.\n"
#define L10N_CGN_LBL_LOCK_SELECTOR_01_STR   "    Note: Lock selector for this machine is 0x%X\n"
#define VL10N_CGN_LBL_LOCK_CODE_01_STR      "    Locking code for this machine is %s\n"

#define VL10N_CGN_HDR_LOCK_PRIMARY_STR      "  Locking information (primary or group 1) for server -\n"
#define VL10N_CGN_HDR_LOCK_SECONDARY_STR    "  Locking information (secondary or group 2) for server -\n"

#define VL10N_CGN_INF_LOCK_SRV_STR          "  Locking information for server\n"
#define VL10N_CGN_LBL_LOCK_SRV_STR          "  Locking information for server #%d\n"

#define VL10N_CGN_INF_NUM_CHALLENGE_STR     "Number of challenge-response secrets or <Enter> for none: "

#define VL10N_CGN_INF_SECRET_TEXT_STR       "  Secret text #%d (Any printable characters): "
#define VL10N_CGN_INF_SECRET_TEXT_01_STR    "  Secret text #%d (2-9/A-Z): "

#define VL10N_CGN_HDR_SITE_LIC_STR          "Site licensing information -\n"
#define VL10N_CGN_INF_SUBNET_NUM_STR        "  Number of subnets specifications for the site or <Enter> for none: "
#define VL10N_CGN_INF_SUBNET_SPEC_STR       "  Subnet specification (such as 123.*.43.*)\n"
#define VL10N_CGN_LBL_SUBNET_STR            "    Subnet #%d: "

#define VL10N_CGN_CFM_NODE_LOCK_SRV_STR     "  Do you wish to lock the clients only to the server host? (Y/N) "
#define VL10N_CGN_INF_NUM_MAC_STR           "  Number of machines the clients will be locked to [1-%ld]: "

#define VL10N_CGN_HDR_CLIENT_LOCK_STR       "  Locking information for client #%d -\n"
#define VL10N_CGN_INF_CLIENT_NUM_KEYS_STR   "    Number of tokens for Client #%d [0 - %d] or <Enter> for no limit: "

#define VL10N_CGN_INF_NUM_REDUNDANT_SRV_STR         "Enter number of redundant servers(%d - %d): "

#define VL10N_CGN_INF_MAX_CONCURRENT_KEYS_STR       "Max number of concurrent tokens [0 - %u]: "
#define VL10N_CGN_INF_MAX_CONCURRENT_KEYS_01_STR    "Max number of concurrent tokens [0 - %u] or <Enter> for no limit: "
#define VL10N_CGN_PRM_SOFT_LIMIT_STR                "Soft limit [0 - %u] or <Enter> for no limit: "
#define VL10N_CGN_INF_MAX_COMMUTER_DAYS_STR    "Max number of commuter days [1 - %ld] or <Enter> for no limit: "
#define VL10N_CGN_INF_LOCAL_REQUEST_LOCKCRIT_STR        "Enter desired commuter, perpetual, and grace period locking criteria.\nSeparate values by colons using the format [required]:[floating]:[minimum req'd].\nPrecede hexadecimal values by a '0x'. Example: 0x4:0x3FF:2\nPlease enter [<Enter> for default criteria]: "
#define VL10N_CGN_INF_LOCAL_REQUEST_LOCKCRIT_STR_V14    "Enter desired commuter, repository, and grace period locking criteria.\nSeparate values by colons using the format [required]:[floating]:[minimum req'd].\nPrecede hexadecimal values by a '0x'. Example: 0x4:0x3FF:2\nPlease enter [<Enter> for default criteria]: "
#define VL10N_CGN_INF_GRACE_PERIOD_OPT_STR     "Grace period options - \n"
#define VL10N_CGN_INF_GRACE_PERIOD_OPT_01_STR  "  [%d] No grace period allowed\n"
#define VL10N_CGN_INF_GRACE_PERIOD_OPT_02_STR  "  [%d] Standard grace period allowed\n"
#define VL10N_CGN_INF_GRACE_PERIOD_DAYS        "Grace period calendar days [1 - %ld] [%d]: "
#define VL10N_CGN_INF_GRACE_PERIOD_HOURS       "Grace period elapsed hours [1 - %ld] [%d]: "
#define VL10N_CGN_INF_OVERDRAFT_OPT_STR        "Overdraft options - \n"
#define VL10N_CGN_INF_OVERDRAFT_OPT_01_STR     "  [%d] No overdraft allowed\n"
#define VL10N_CGN_INF_OVERDRAFT_OPT_02_STR     "  [%d] Standard overdraft allowed\n"
#define VL10N_CGN_INF_OVERDRAFT_HOURS          "Overdraft maximum hours [1 - %ld] or <Enter> for no limit: "
#define VL10N_CGN_INF_OVERDRAFT_USERS          "Overdraft maximum users - <Enter> for no limit, %% for percentage.\nPlease enter [1%% - 100%%, or 1 - %ld]: "
#define VL10N_CGN_INF_GC_LENGTH_OVERFLOW_ERR   "Grace code length exceeds maximum limit.\n"

/* Trial Attributes - Days, Hours and Execution count */
#define VL10N_CGN_INF_TRAIL_ATTRIBUTES_STR     " Enter desired trial licensing attributes.\nSeparate values by colons using the format [days]:[hours].\nExample: 30:400, 30:0\nPlease enter [<Enter> for default values]: "

#define VL10N_CGN_HDR_KEY_LIFETIME_STR          "Token lifetime options - \n"
#define VL10N_CGN_INF_KEY_LIFETIME_STR          "  Lifetime of each token (in minutes only) or <Enter> for no limit: "

//#define VL10N_CGN_INF_KEY_MINUTES_STR           "  [%d] - Multiple of %d minute(s), maximum %ld minutes\n"
#define VL10N_CGN_INF_KEY_MINUTES_STR           "  [%d] - From %d through  %ld minutes (Multiple of %d minute(s))\n"
#define VL10N_CGN_INF_CHOOSE_CAPACITY_STYLE_STR "  Type an option that suits you best(For zero capacity select any one of the above options) : "
#define VL10N_CGN_INF_CHOOSE_STYLE_STR          "  Type an option that suits you best : "

/*#define VL10N_CGN_HDR_CAPACITY_UNITS_STR        "License Capacity specification - \n"
#define VL10N_CGN_INF_CAPACITY_UNITS_STR        "  [%d] - Multiple of %d(s), maximum %ld\n"
#define VL10N_CGN_INF_LIC_CAPACITY_STR          "  Capacity of license or <Enter> for no limit: "
#define VL10N_CGN_INF_TOKEN_CAPACITY_STR        "  Capacity of each token or <Enter> for no limit: "*/

#define VL10N_CGN_HDR_CAPACITY_UNITS_STR        "License Capacity options - \n"
#define VL10N_CGN_INF_CAPACITY_UNITS_STR        "  [%d] From %d through %ld (Multiple of %d) \n"
#define VL10N_CGN_INF_LIC_CAPACITY_STR          "  Type the Capacity of a license or press <Enter> for unlimited capacity or type 0 for zero capacity: "
#define VL10N_CGN_INF_TOKEN_CAPACITY_STR        "  Type the Capacity of a token or press <Enter> for unlimited capacity or type 0 for zero capacity: "

#define VL10N_CGN_HDR_KEY_HOLDTIME_STR          "Token hold time specification - \n"
#define VL10N_CGN_INF_KEY_HOLDTIME_SEC_STR      "  Hold time of each token (in seconds): "
#define VL10N_CGN_INF_KEY_HOLDTIME_MIN_STR      "  Hold time of each token (in minutes only): "

#define VL10N_CGN_CFM_CLOCK_TAMPER_LIC_STR      "Should licenses be issued if clock tampering is detected? (Y/N): "

#define VL10N_CGN_HDR_LIC_START_STR             "License start information -\n"
#define VL10N_CGN_INF_LIC_START_YEAR_STR        "  License start year (%d - %d), <Enter> for a current license: "
#define VL10N_CGN_INF_LIC_START_YEAR_SUPPORT_NEVER_STR        "  License start year (%d - %d), (NEVER) for no start date, or <Enter> for a current license: "
#define VL10N_CGN_INF_LIC_START_MON_STR         "  License start month (1-12 or Jan-Dec): "
#define VL10N_CGN_INF_LIC_START_DAY_MON_STR            "  License start day of the month (1-%d): "
#define VL10N_CGN_INF_LIC_START_MON_01_STR             "License start month [1-12] : "
#define VL10N_CGN_INF_LIC_START_DAY_MON_01_STR         "License start day [1-31]   : "
#define VL10N_CGN_INF_LIC_START_YEAR_01_STR            "License start year         : "
#define VL10N_CGN_INF_LIC_START_HOURS_STR              "  License start hour in UTC (0-23), (NO) or <ENTER> to skip start hour and minute: "
#define VL10N_CGN_INF_LIC_START_MINS_STR               "  License start minute in UTC (0-59): "
#define VL10N_CGN_INF_LIC_DATE_TIME_01_STR               "\n[INFO]Entered License Start date/time is : %d-%d-%d %d:%d [MM-DD-YYYY HH:MI] UTC"
#define VL10N_CGN_INF_LIC_DATE_TIME_02_STR               "\n      Entered License End date/time is   : %d-%d-%d %d:%d [MM-DD-YYYY HH:MI] UTC"
#define VL10N_CGN_INF_LIC_DATE_TIME_03_STR               "\nDo you want to generate license with above start and end date ? \nEnter 'Y' to Continue or 'N' to Re-enter dates : "


#define VL10N_CGN_HDR_LIC_EXPIRY_STR                    "License expiration information -\n"
#define VL10N_CGN_INF_LIC_EXPIRY_YEAR_STR               "  Expiration year (%d - %d) or <Enter> for no expiration: "
#define VL10N_CGN_INF_LIC_EXPIRY_MONTH_01_STR           "  Expiration month (1-11 or Jan-Nov): "
#define VL10N_CGN_INF_LIC_EXPIRY_MONTH_02_STR           "  Expiration month (1-12 or Jan-Dec): "
#define VL10N_CGN_INF_LIC_EXPIRY_MONTH_03_STR           "  Expiration month (7-12 or Aug-Dec): "
#define VL10N_CGN_INF_LIC_EXPIRY_MONTH_04_STR           "  Expiration month (1-12 or Jan-Dec): "
#define VL10N_CGN_INF_LIC_EXPIRY_DAY_STR                "  Expiration day of the month (1-%d): "
#define VL10N_CGN_INF_LIC_EXPIRY_01_STR                 "      1. March 31st \n"
#define VL10N_CGN_INF_LIC_EXPIRY_02_STR                 "      2. June 30th \n"
#define VL10N_CGN_INF_LIC_EXPIRY_03_STR                 "      3. September 30th \n"
#define VL10N_CGN_INF_LIC_EXPIRY_04_STR                 "      4. December 31st \n"
#define VL10N_CGN_INF_LIC_EXPIRY_SEL_01_STR             "Select the option (1-4) or <Enter> for no expiration: "
#define VL10N_CGN_INF_LIC_EXPIRY_SEL_02_STR             "Select the option (3-4) or <Enter> for no expiration: "
#define VL10N_CGN_INF_LIC_EXPIRY_HOURS_STR              "  Expiration hour in UTC (0-23), (NO) or <ENTER> to skip expiration hour and minute : "
#define VL10N_CGN_INF_LIC_EXPIRY_MINS_STR               "  Expiration minute in UTC (0-59): "


#define VL10N_CGN_INF_METER_VALUE_STR                   "Meter value: "

#define VL10N_CGN_CFM_FILENAME_LIC_STR                  "File name to write the license codes    [lservrc] ? "
#define VL10N_CGN_INF_FILENAME_LIC_STR                  "\nInformation written to %s\n"

#define VL10N_CGN_CFM_MORE_LIC_GEN_STR                  "\nDo you want to generate more licenses? (Y/N) "

#define VL10N_CGN_HDR_LIC_STRING_GEN_STR                "Type of license code generated - \n"
#define VL10N_CGN_INF_LIC_STRING_GEN_01_STR             "  [0] - Encrypted license code \n"
#define VL10N_CGN_INF_LIC_STRING_GEN_02_STR             "  [1] - Expanded readable license code \n"
#define VL10N_CGN_INF_LIC_STRING_GEN_03_STR             "  [2] - Concise readable license code \n"

#define VL10N_CGN_HDR_LIC_TYPE_STR                      "Different License Types -\n"
#define VL10N_CGN_INF_LIC_TYPE_01_STR                   "  [%d] - 1 to 30-Day Standalone Demo\n"
#define VL10N_CGN_INF_LIC_TYPE_01_NEW_STR                   "  [%d] - 1 to 1461-Day Standalone Demo\n"
#define VL10N_CGN_INF_LIC_TYPE_02_STR                   "  [%d] - 1 to 30-Day Network Demo\n"
#define VL10N_CGN_INF_LIC_TYPE_02_NEW_STR                   "  [%d] - 1 to 1461-Day Network Demo\n"
#define VL10N_CGN_INF_LIC_TYPE_03_STR                   "  [%d] - Absolute StandAlone Demo\n"
#define VL10N_CGN_INF_LIC_TYPE_04_STR                   "  [%d] - Absolute Network Demo\n"
#define VL10N_CGN_INF_LIC_TYPE_05_STR                   "  [%d] - Locked StandAlone Demo\n"
#define VL10N_CGN_INF_LIC_TYPE_06_STR                   "  [%d] - Buy StandAlone Product\n"
#define VL10N_CGN_INF_LIC_TYPE_07_STR                   "  [%d] - Buy Network Product\n"
#define VL10N_CGN_INF_LIC_TYPE_08_STR                   "  [%d] - Locked Network Demo\n"


#define VL10N_CGN_HDR_FEATURE_CONCURRENT_KEYS_STR       "Enter the option for number of concurrent tokens for feature %d\n"
#define VL10N_CGN_INF_FEATURE_CONCURRENT_KEYS_01_STR    "  [0] - To disable the feature \n"
#define VL10N_CGN_INF_FEATURE_CONCURRENT_KEYS_02_STR    "  [1] - Default Number of Tokens\n"
#define VL10N_CGN_LBL_FEATURE_CONCURRENT_KEYS_STR       "Max number of concurrent tokens for feature %d [0 - %d] or <Enter> for no limit: "

#define VL10N_CGN_ERR_FEATURE_NAME_STR                  "Invalid Feature Name: Must be 2-Digit Numeric Code.\n"
#define VL10N_CGN_ERR_BASE_FEATURE_NAME_STR             "Invalid Base Feature Name:\n"
#define VL10N_CGN_ERR_DECIPHER_LIC_INF_STR              "\nUnable to decipher license information. Aborting...\n"

#define VL10N_CGN_CFM_LIC_GEN_STR                       "Do you want to generate license? (Y/N) :"

#define VL10N_CGN_INF_TRAIL_PERIOD_STR                  " Trial period  [1 - %d] : "
#define VL10N_CGN_INF_TRIAL_PERIO_SHORT_NUM_STR         " Trial period  [1 - %d] -\n You can enter trial days factored to years, months and days.\n (For example, type \"2y3m5d\" for 825 days, \"3m9d\" for \n 99 days, \"30d\" or just \"30\" for 30 days) :"
#define VL10N_CGN_CFM_SHORT_NUM_LIC_GEN_STR             "Do you want to generate short numeric licenses? (Y/N) :"
#define VL10N_CGN_INF_CODE_FORMAT_STR                   "Code Format"

#define VL10N_CGN_ERR_GLOBAL_FILE_STR                   "Global defaults file, line %d: >>%s<< -- Invalid input.\n"
#define VL10N_CGN_ERR_INTERNAL_STR                      "Internal error. Quitting...\n"
#define VL10N_CGN_ERR_COMMANDLINE_STR                   "Command-line option: >>%s<< -- Invalid input.\n"
#define VL10N_CGN_ERR_VENDORID_01_STR               "  Vendor ID is not decimal. Please reenter: "
#define VL10N_CGN_ERR_VENDORID_02_STR               "  Vendor ID is too large. Please reenter: "
#define VL10N_CGN_ERR_ILLEGAL_FORMAT_STR            "Illegal format. Enter one pair per line (e.g. APP%c3.2).\n"
#define VL10N_CGN_ERR_FEATURE_LIST_STR              "Feature list is too long. Can-not accept this pair.\n"

#define VL10N_CGN_ERR_INITIALIZE_STR                "Error : Can-not Initialize..\n"
#define VL10N_CGN_ERR_MORE_LIC_ISSUE_STR            "Error : Not authorized to generate more licenses..\n"

#define VL10N_CGN_INF_BEFORE_FILEREAD_STR           " Before reading options and defaults\n"
#define VL10N_CGN_INF_AFTER_FILEREAD_STR            " After reading options and defaults\n"
#define VL10N_CGN_INF_LOCK_INK_CHK_STR              " Checked lock info from options and defaults \n"
#define VL10N_CGN_INF_GLOBALFILE_INF_STR            "\nReading global code defaults from file \"%s\"...\n"
#define VL10N_CGN_INF_FILE_OUT_LIC_MISSING_STR      "\nUnable to write to Outupt file \"%s\". Using standard output.\n" 
#define VL10N_CGN_ERR_CURR_TIME_STR                 "\nERROR: Unable to get the current system time.\n"
#define VL10N_CGN_ERR_INVALID_LIC_STR               "\nERROR: Invalid license code: \"%s\"\n"
#define VL10N_CGN_ERR_UNAUTHORIZE_LIC_STR           "\nERROR: Unauthorized license code: \"%s\"\n"
#define VL10N_CGN_ERR_OTHERAPP_LIC_STR              "\nERROR: license code is intended for a different application: \"%s\"\n"
#define VL10N_CGN_ERR_UNAUTHORIZE_LICCODE_STR       "\nERROR: License code \"%s\" is not authorized on this machine.\n"
#define VL10N_CGN_ERR_CONFIRM_VALIDITY_LIC_STR      "\nERROR: Unable to confirm current validity of license code \"%s\". (Expired?)\n"
#define VL10N_CGN_ERR_UNAUTHORIZE_SW_STR            "Unauthorized copy of software!\n"
#define VL10N_CGN_ERR_FILE_READ_STR                 "Warning: Unable to read start-up file \"%s\". Aborting...\n"
#define VL10N_CGN_ERR_FILE_WRITE_STR                "Warning: Unable to write to file \"%s\". Using standard output.\n"
#define VL10N_CGN_ERR_FILEWRITE_STR                 "Warning: Unable to write to file \"%s\".\n"
#define VL10N_CGN_ERR_LICCODE_READ_STR              "Unable to read license code from file \"%s\"\n"

/* Do not translate */
#define VL10N_CGN_INF_PROCESS_OPT_STR                ":o:s:g:a:c:e:i:m:n:v:y:b:k:h:j:p:r:w:E:D:G:K:J:R:B:C:H:I:L:M:N:S:F:A:P:Q:T:V:W:Y:X:O:U:ver:enc:maj:nserv:elan:ctime:serv:ktype:nfeat:"
#define VL10N_CGN_INF_COMMAND_USAGE_01_STR          "\nUsage: %s \n\
       [-%c License-file]           [-%c License-configuration-file]\n\
       [-%c New-codes-output-file]  [-%c Global-defaults-file]\n\
       [-%s Code-generator-version(1 - max-code-generator-version)]\n\
       [-%c Feature-name]           [-%c Feature-version]\n\
       [-%c Network/Standalone/Repository]\n\
       [-%c Short/Long/Short Numeric: different License Type]\n\
       [-%s Single/Multi-Feature(for short-numeric only)] \n\
       [-%s No-of-features (valid for multi-feature ]\n\
       [-%c Additive/Exclusive/Aggregate]\n\
       [-%c Private-vendor-info-text(encrypted in license string)]\n\
       [-%s Public-vendor-info-text(readable in license string)]\n\
       [-%s License-vendor-info-text(encrypted in license string)]\n\
       [-%s Encryption-level(0-4)]\n\
       [-%c (y/n) Commuter-license?]\n\
       [-%s commuter_max_checkout_days]\n\
       [-%s local-request-lock-criteria required:float:minimum\n\
                                                           (use 0x for hex) ]\n\
       [-%c (y/n) Redundant-lic?]   [-%s (y/n) Majority-rule?]\n\
       [-%s No-of-servers]"

#define VL10N_CGN_INF_COMMAND_USAGE_02_STR          "\n\
       [-%c Server-locked/Client-and-Server-locked/Unlocked/\n\
            Client-locked Application-server-locking]\n\
       [-%c Server-primary-lock-selector:lock-code (if non-redundant)]\n\
       [-%s1:Server1-lock-selector:lock-code \n\
       [-%s2:Server2-lock-selector:lock-code ....(if redundant)] ]\n\
       [-%c Server-secondary-lock-selector:lock-code ]\n\
       [-%c (y/n) Lock-to-server?]  [-%c Num-client-machines-to-lock-to]\n\
       [-%c1Client1-lock-selector:lock-code  \n\
       [-%c2Client2-lock-selector:lock-code ... ] ]\n\
       [-%c Number-of-subnets | 0]\n\
       [-%c1Subnet1-info            [-%c2Subnet2-info ... ] ]\n\
       [-%c Num-users | NOLIMIT(for Short/Long/ShortNumeric single feature)] \n\
       [-%c1:Num-users | NOLIMIT  \n\
       [-%c2:Num-users | NOLIMIT ...(in case of shortNumeric Multi Feature Lic )]\n\
       [-%c Soft-limit | NOLIMIT]\n\
       [-%c 1-31 Start-day] \n\
       [-%c 1-12 Start-month]       [-%c Start-year | NEVER]\n\
       [-%s 0-23 Start-hour | NO]    [-%s 0-59 Start-minute]\n\
       [-%c 1-31 Expiration-day]    [-%c Tokens-per-client | NOLIMIT]\n\
       [-%c 1-12 Expiration-month]  [-%c Expiration-year:quarter option | NEVER]\n\
       [-%s 0-23 Expiry-hour | NO]    [-%s 0-59 Expiry-minute]\n"

#define VL10N_CGN_INF_COMMAND_USAGE_03_STR      "\n\
       [-%c No-share/User/Host/X-display/App-defined Sharing]\n\
       [-%c Share-limit | NOLIMIT]  [-%c Number-of-secrets | 0]\n\
       [-%c1Secret1-text            [-%c2Secret2-text ... ] ]\n\
       [-%c (y/n) More-features?]   [-%c (y/n) Issue-if-clock-tampered?]\n\
       [-%c No-hold/App-defined/License-defined Hold-type]\n\
       [-%c Key-holdtime]           [-%c 1m/10m/30m/60m Holdtime-units]\n\
       [-%c Key-lifetime | NOLIMIT] [-%c 1m/10m/30m/60m Lifetime-units]\n\
       [-%c Encrypted/Expanded-readable/Concise-readable Out-license-type] \n\
       [-%c Trial License Life (Format- Days:Hours)]\n\
       [-%s Capacity-flag No-Capacity/Non-Pooled/Pooled Capacity]\n\
       [-%s 1s/10s/100s/1k/10k Capacity-units] \n\
       [-%s Capacity] \n"

#define VL10N_CGN_INF_COMMAND_USAGE_035_STR      "\n\
       [-%s 0=no_grace_period 1=standard_grace_period]\n\
       [-%s calendar-days]\n\
       [-%s elapsed-hours]\n\
       [-%s 0=no_overdraft    1=standard_overdraft]\n\
       [-%s overdraft_hours]\n\
       [-%s overdraft_users | overdraft_users%% | NOLIMIT]\n\
       [-%s (y/n) Enable-VM-Detection?]\n"

#define VL10N_CGN_INF_COMMAND_USAGE_04_STR          "\n \
       NOTES: \n\
       (1) Tokens-per-client value applies to all client-and-server locked clients.\n\
       (2) Format X/Y/Z/... takes values as 0/1/2/... respectively.\n\
       (3) Symbol | indicates a choice of two values.\n\
       (4) All client and server lock selector value should be in hexadecimal.\n\
       (5) There should be no spaces between option letters C,S,R and their \n\
           arguments.\n\
       (6) Different License Type is required only when code type is Short \n\
           Numeric \n\
       (7) quarter option is required only when Different License Type is either \n\
           30-Day Standalone Demo or 30-Day Network\n\
       (8) In case of shortNumeric Multi Feature Lic if lic is not  Demo of type \n\
           VLScg_BUY_NETWORK_PROD, valid inputs for num_tokens are\n\
           -n1:0/1 -n2:0/1 ... where 0 is to disable the feature and 1 to  set the \n\
           default no of tokens.\n\
       (9) '!' and '$' are reserved character and cannot be used with feature \n\
           name, feature version, private vendor info, public vendor info and \n\
           secret text.\n\
       (10) ',' (comma) is a reserved character in addition to other reserved \n\
            characters (!#$) for secret in license version 10 onwards \n\
       (11) \"011\" and \"oil\" are reserved string and cannot be used with \n\
            feature name and feature version.\n\
       (12) \"NiL\" and \"Ni\" are reserved string and cannot be used with \n\
            feature name, feature version, private vendor info and public \n\
            vendor info.\n\
       (13) '#' is a comment character and all text appearing after this will \n\
            be ignored.\n"

#define VL10N_CGN_INF_OUPUT_FILENAME_STR    "standard-output"
/* Do not translate */
#define VL10N_CGN_INF_FILENAME_STR           "LSCGEN"

/* Do not translate */
#define VL10N_CGN_HDR_LIC_GEN_STR         "\n    %s %s License Generation System\n"

/* Do not translate */
#define VL10N_CGN_HDR_LIC_GEN_01_STR        "      INTERNAL CODEGEN LICENSING SYSTEM v%s(%d)\n\n"
#define VL10N_CGN_INF_LIC_GEN_02_STR        "Please Wait, Generating License...\n"
#define VL10N_CGN_LBL_LIC_GEN_STR           "code generator: %s\n"
#define VL10N_CGN_ERR_LIC_GEN_STR           "Error: Unable to generate license code. \n"

#define VL10N_CGN_INF_NEW_LICCODE_STR       "Wrote new licensing code(s) to \"%s\".\n\n"
/* Example: Wrote new licensing code(s) to "myfile.dat" */

/* Do not translate */
#define VL10N_CGN_HDR_VENDOR_INF_STR        "\n    Sentinel RMS Development Kit %s(4) Vendor Information Generation System\n"
#define VL10N_CGN_ERR_VENDOR_INF_STR        "\nFailed to  write vendor info file\n"

#define VL10N_CGN_INF_FEATURE_01_STR        "Software expires after (# days)   ? "
#define VL10N_CGN_INF_FEATURE_02_STR        "Tier number for client-and-server locking      ? "
#define VL10N_CGN_INF_FEATURE_03_STR        "Number of Distribution Licenses   ? "
#define VL10N_CGN_INF_FEATURE_04_STR        "Number of soft failures allowed   ? "
#define VL10N_CGN_INF_FEATURE_05_STR        "No. of licenses server can issue  ? "
#define VL10N_CGN_INF_FEATURE_06_STR        "Corporate Message to be given     ? "


#define VL10N_CGN_INF_TRAIL_SW_STR            "Reference : Trial software (Request #1)\n"
#define VL10N_CGN_CFM_TRAIL_SW_STR            "Is this a demo license ?    [y/n] : "
#define VL10N_CGN_INF_NODE_LOCK_STR           "Reference : Client-and-Server Locking (Request #2)\n"
#define VL10N_CGN_CFM_TIER_LIC_STR            "Enable Tier Licensing       [y/n] : "
#define VL10N_CGN_INF_DISTRIBUTION_STR        "Reference : Parteners dist. (Request #6)\n"
#define VL10N_CGN_CFM_DISTRIBUTION_STR        "Enable License Distribution [y/n] : "
#define VL10N_CGN_INF_FAILURE_STR             "Reference : Failure Strategy (Request #7)\n"
#define VL10N_CGN_CFM_FAILURE_STR             "Enable Soft Fail Feature    [y/n] : "
#define VL10N_CGN_INF_SRV_LIC_STR             "Reference : Server Licensing (Model #1)\n"
#define VL10N_CGN_CFM_SRV_LIC_STR             "Enable Server Licensing     [y/n] : "
#define VL10N_CGN_INF_CORPORATE_LIC_STR       "Reference : Corporate Licensing (Model #4)\n"
#define VL10N_CGN_CFM_CORPORATE_LIC_STR       "Enable Corporate Licensing  [y/n] : "
#define VL10N_CGN_INF_COMMAND_USAGE_05_STR    "\
Usage: %s \n\
  -o [Timefix-code-output-file]\n\
 "
/* Example: Usage: cgtamp
  -o [Timefix-code-output-file] */

#define VL10N_CGN_HDR_TIME_TAMPER_STR         "\n%s %s Time Tamper Utility License Generation System\n"
#define VL10N_CGN_INF_TIME_LIMIT_STR          "\n Max no. of times license can be used [1-32766] or <enter> for no Limit : "
#define VL10N_CGN_ERR_INVALID_TIMEVALUE_STR   "\n Invalid input , Please reenter between [1-32766 ]: "
#define VL10N_CGN_ERR_INVALID_DATEVALUE_STR   "\n Invalid input , Your input should be between Jan 1, 1970 and Dec 31, 2037: "
#define VL10N_CGN_INF_LICCODE_FILE_STR        " License code written to file `%s'.\n"
#define VL10N_CGN_INF_LOCK_CODE_01_STR        " Lock code depends on         : "
#define VL10N_CGN_INF_LOCK_CODE_COMREQ_STR    " Local request must lock to   : "
#define VL10N_CGN_INF_LOCK_CODE_COMFLOAT_STR  " Local request may lock to    : "
#define VL10N_CGN_INF_LOCK_CODE_COMTOTAL_STR  "\n Local request lock items     : %d (required & float)\n"
#define VL10N_CGN_INF_LOCK_CODE_COM_NONE_STR  "None specified\n"
#define VL10N_CGN_ERR_LIC_STRING_STR          "\n Lic String does not contain any enabled features\n"
#define VL10N_CGN_ERR_LIC_STRING_01_STR       "\n Error in Lic String"
#define VL10N_CGN_INF_LIC_TYPE_STR          "\n License type                 : Trial "
#define VL10N_CGN_LBL_LIC_TYPE_01_STR       "\n License type                 : Normal "
#define VL10N_CGN_LBL_LIC_TYPE_STR          "\n License type                 : "
#define VL10N_CGN_HDR_NW_LIC_STR              "Network "
#define VL10N_CGN_HDR_STANDALONE_LIC_STR      "Standalone "
#define VL10N_CGN_HDR_PERPETUAL_LIC_STR       "Perpetual "
#define VL10N_CGN_HDR_REPOSITORY_LIC_STR      "Repository "
#define VL10N_CGN_HDR_REDUNDANT_LIC_STR       "Redundant"
#define VL10N_CGN_INF_KEYTYPE_STR           "\n TOKEN TYPE                   : MULTI FEATURE"
#define VL10N_CGN_INF_NUM_FEATURE_STR       "\n No of features               : %d"
#define VL10N_CGN_INF_FEATURENAME_STR       "\n Features                     : %s"
#define VL10N_CGN_INF_CONCURRENT_USERS_STR  "\n Max concurrent users         : "
#define VL10N_CGN_INF_UNLTD_USERS_STR         "Unlimited."
#define VL10N_CGN_INF_ALLFEATURE_STR        "\n  for all features            : "
#define VL10N_CGN_INF_SOFT_LIMIT_STR        "\n Soft limit on users          : "
#define VL10N_CGN_INF_SOFT_LIMIT_01_STR     "\n Soft limit on users          : Same as Hard Limit\n"
#define VL10N_CGN_ERR_LIC_START_STR           " License never starts!\n"
#define VL10N_CGN_LBL_LIC_SHARING_STR         " Redundant servers            : %d\n"
#define VL10N_CGN_INF_MAJORITY_RULE_STR       " Majority rule                : ON.\n"
#define VL10N_CGN_LBL_SRV_LOCKCODE_STR        " Server locking code          : Primary   = \"%s\"\n"
#define VL10N_CGN_LBL_SRV_LOCKCODE_01_STR     " Server #%d locking code       : %s \n"
#define VL10N_CGN_LBL_SRV_LOCKCODE_02_STR     " Server locking code          : Secondary = \"%s\"\n"
#define VL10N_CGN_LBL_CLI_LOCKCODE_STR        " Client #%d locking code       : \"%s\""
#define VL10N_CGN_LBL_NUM_COPIES_STR          " (Max %u concurrent copies)\n"
#define VL10N_CGN_INF_SITE_LOCKING_STR        " Valid on subnet(s)           : "
#define VL10N_CGN_INF_SITE_LOCKING_01_STR     "or %s"
#define VL10N_CGN_INF_LBL_ADDEXCL_STR         " Additive/exclusive/aggregate : "
#define VL10N_CGN_INF_LBL_ADDEXCL_01_STR      "Additive license (number of tokens will be added).\n"
#define VL10N_CGN_INF_LBL_ADDEXCL_02_STR      "Exclusive license (overrides additive licenses).\n"
#define VL10N_CGN_INF_LBL_ADDEXCL_03_STR      "Aggregate license (additive without any restrictions).\n"
#define VL10N_CGN_INF_HELD_LIC_STR            " Held licenses                : Held licenses disabled.\n"
#define VL10N_CGN_ERR_CLOCK_TAMPER_STR        " Action on clock tamper       : No more fresh licenses will be issued.\n"
#define VL10N_CGN_ERR_CLOCK_TAMPER_01_STR     " Action on clock tamper       : No action.\n"
#define VL10N_CGN_LBL_VENDOR_INFO_STR         " Vendor Info                  : \"%s\"\n\n"
#define VL10N_CGN_LBL_CONVERTIME_STR          " Conversion time              : %d\n"

/*strings to display additive property in an encrypted license string*/
#define VL10N_CGN_ADDITIVE_STR                "additive"
#define VL10N_CGN_EXCLUSIVE_STR               "exclusive"
#define VL10N_CGN_AGGREGATE_STR               "aggregate"

/* cloud specific fields */
#define VL10N_CGN_ENTITLEMENT_ID_STR          "entitlement id"
#define VL10N_CGN_PRODUCT_ID_STR              "product id"
#define VL10N_CGN_FEATURE_ID_STR              "feature id"
#define VL10N_CGN_AUTHORIZATION_ID_STR        "authorization id"
#define VL10N_CGN_CLOUD_USAGE_FLAG_STR        "usage(cloud)flag"
#define VL10N_CGN_CLOUD_GENERATED_STR         "license source (cloud generated or not)"
#define VL10N_CGN_ACTIVATION_BIRTH_STR        "Activation start time"
#define VL10N_CGN_ACTIVATION_EXPIRY_STR        "Activation expiry time"


/*{lsdecode STRINGS - Acronym [LSD] }*/
#define VL10N_LSD_HDR_LIC_DECODE_STR          "\n  %s %s License Decoding Utility\n"
#define VL10N_LSD_INF_CMD_OPT_STR             ":s:e:"
#define VL10N_LSD_INF_CMD_USAGE_STR           "\
Usage: %s \n\
       [-%c License-file] \n\
       [-%c License-configuration-file] \n\
       [-pause Pauses before returning to the console]\n\
       [-%s Revocation-Ticket-File]\n"

/* Common strings that should be lsdecode strings -- Derick */
#define VL10N_COM_LBL_FEATURE_NAME_STR      "\n Feature name                 : \"%s\" \n" /* used by lsdecode */
#define VL10N_COM_LBL_LIC_TYPE_STR        "\n\n License type                 : \"%s\" "

#define VL10N_LSD_LBL_TRIAL_PERIOD_STR               "\n Trial period                 : %d days"
#define VL10N_LSD_LBL_TRIAL_PERIOD_DISABLED_STR      "\n Trial period                 : %s"

#define VL10N_LSD_LBL_TRIAL_HOURS_STR                "\n Trial elapsed hours          : %d hours"
#define VL10N_LSD_LBL_TRIAL_HOURS_DISABLED_STR       "\n Trial elapsed hours          : %s"

#define VL10N_LSD_LBL_TRIAL_EXECUTIONCNT_STR          "\n Trial execution count        : %d"
#define VL10N_LSD_LBL_TRIAL_EXECUTIONCNT_DISABLED_STR "\n Trial execution count        : %s"

#define VL10N_LSD_LBL_SHARING_01_STR          " Sharing criterion            : User name based sharing.\n"
#define VL10N_LSD_LBL_SHARING_02_STR          " Sharing criterion            : Client host name based sharing.\n"
#define VL10N_LSD_LBL_SHARING_06_STR          " Sharing criterion            : Vendor defined criteria based sharing.\n"
#define VL10N_LSD_LBL_SHARING_03_STR          " Sharing limit                : %d\n"
#define VL10N_LSD_LBL_FEATURE_VER_STR         " Feature version              : \"%s\"\n"
#define VL10N_LSD_LBL_SHARING_04_STR          " Sharing criterion            : X display name based sharing.\n"
#define VL10N_LSD_LBL_HOLDING_03_STR          " Hold time                    : %ld min(s)\n"
#define VL10N_LSD_LBL_SPACE_STR               "                              : "


#define VL10N_LSD_INF_LICCODE_FILE_STR        "Reading license codes from file: \"%s\"\n\n"
#define VL10N_LSD_INF_LICCODE_STR             "\nLicense code: \"%s\"\n"
#define VL10N_LSD_LBL_HOLDING_01_STR          " Held licenses                : Allowed, hold time set by application.\n"
#define VL10N_LSD_LBL_HOLDING_02_STR          " Held licenses                : Allowed, hold time set by license.\n"
#define VL10N_LSD_LBL_START_DATE_01_STR     "\n License start date           : "
#define VL10N_LSD_LBL_START_DATE_03_STR       " No License start date!\n"
#define VL10N_LSD_LBL_EXP_DATE_01_STR         " Expiration date              : "
#define VL10N_LSD_LBL_EXP_DATE_02_STR         "License has no expiration.\n"
#define VL10N_LSD_LBL_KEYLIFE_01_STR          " Token lifetime (heartbeat)   : Infinite.\n"
#define VL10N_LSD_LBL_KEYLIFE_03_STR          " Token lifetime (heartbeat)   : %ld secs (%ld min(s))\n"
#define VL10N_LSD_LBL_LIC_ELAN_01_STR         " License code                 : Elan converted license code.\n"
#define VL10N_LSD_LBL_LIC_ELAN_02_STR         " Conversion time              : %ld\n"
#define VL10N_LSD_LBL_LFE_01_STR              " Log encryption level         : %d\n"

#define VL10N_LSD_LBL_LIC_COMMUTER_STR      "\n Commuter license allowed     : YES\n"
#define VL10N_LSD_LBL_LIC_COMMUTER_01_STR   "\n Commuter license allowed     : NO\n"
#define VL10N_LSD_LBL_COMMUTER_DAYS_STR       " Max Commuter Checkout Period : %ld days\n"
#define VL10N_LSD_LBL_COMMUTER_DAYS_TILL_LIC_EXPIRY_STR " Max Commuter Checkout Period : Restricted to License Expiry Date.\n"
#define VL10N_LSD_LBL_GRACE_PERIOD_NA_STR   "\n Grace period                 : None.\n"
#define VL10N_LSD_LBL_GRACE_PERIOD_STD_STR  "\n Grace period                 : Standard.\n"
#define VL10N_LSD_LBL_GRACE_PERIOD_DAYS_STR   " Grace period calendar period : %d days\n"
#define VL10N_LSD_LBL_GRACE_PERIOD_HOURS_STR  " Grace period elapsed time    : %d hours\n"
#define VL10N_LSD_LBL_OVERDRAFT_NA_STR      "\n Overdraft                    : None.\n"
#define VL10N_LSD_LBL_OVERDRAFT_STD_STR     "\n Overdraft                    : Standard.\n"
#define VL10N_LSD_LBL_OVERDRAFT_HOURS_STR     " Overdraft session time       : %d hours\n"
#define VL10N_LSD_LBL_OVERDRAFT_HOURS_UNL_STR " Overdraft session time       : Infinite.\n"
#define VL10N_LSD_LBL_OVERDRAFT_USERS_STR     " Overdraft users              : "

/* Information string for license revocation file being read */
#define VL10N_LSD_INF_RVK_TKT_FILE_STR          "\n\nReading license revocation ticket from file: \"%s\"\n"
#define VL10N_LSD_INF_RVK_TKT_STR               "\n\nEncrypted revocation ticket read : \"%s\"\n"
#define VL10N_LSD_INF_RVK_TKT_ENCR_KEY_OPT_STR  "\nDo you want to input the secret (y/n): "
#define VL10N_LSD_INF_RVK_TKT_ENCR_KEY_STR      "\nEnter the encryption key for decoding the revocation ticket: "
#define VL10N_LSD_ERR_RVK_VND_ID_UNMOD_STR      "\nUnauthorised copy of software for this feature."
#define VL10N_LSD_ERR_RVK_TKT_FAIL_STR          "\nFailed to decrypt the revocation ticket."
#define VL10N_LSD_ERR_RVK_TIMESTAMP_FAIL_STR    "\nFailed to calculate the timestamp of the revocation ticket."

/* Fix for defect RMS-5279 */
#define VL10N_LSD_ERR_RVK_TKT_LRT_NOT_SUPPORTED_STR     "\nThis operation is not supported in this product version."

/* Display information strings for displaying the revocation ticket : Rajat Jain */
#define VL10N_LSD_INF_RVK_UTIL_NAME_STR             "\t\t %s %s License Revoke Utility\n"
#define VL10N_LSD_INF_RVK_TKT_DISP_STR                     "\nDisplaying the revocation ticket information.\n"
#define VL10N_LSD_INF_RVK_TKT_DISP_FTR_NAME_STR            "\n   Feature Name                          : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_FTR_VER_STR             "\n   Feature Version                       : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_CAPACITY_STR            "\n   Capacity                              : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_TIME_STR                "\n   Time Stamp Of Ticket Generation       : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_HARD_LIMIT_STR          "   Current License Hard Limit            : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_TOT_UNIT_RVK_STR        "\n   Total Licenses Revoked                : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_CURR_UNIT_RVK_STR       "\n   Licenses Revoked In Current Request   : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_CAPACITY_RVK_STR        "\n   Total Capacity Revoked                : %s"
#define VL10N_LSD_INF_RVK_TKT_DISP_SRVR_LOCK_CRIT_STR      "\n   Server Locking Criteria               : 0x%lX"
#define VL10N_LSD_INF_RVK_TKT_DISP_SRVR_LOCK_INFO_01_STR   "\n   Server Locking Code                   : 0x%s\n"
#define VL10N_LSD_INF_RVK_TKT_DISP_SRVR_LOCK_INFO_02_STR   "\n   Server Locking Code                   : 0x0\n"
#define VL10N_LSD_INF_RVK_TKT_DISP_FTR_NAME_STR_INP        "\n   Input Feature Name                    : "
#define VL10N_LSD_INF_RVK_TKT_DISP_FTR_VER_STR_INP         "\n   Input Feature Version                 : "
#define VL10N_LSD_INF_RVK_TKT_DISP_NUM_LIC_TO_REVOKE       "\n   Input Number of Licenses To Revoke    : "
#define VL10N_LSD_INF_RVK_TKT_DISP_VALD_PWD                "\n   Password length should be between 8 and 22 characters\n"

#define VL10N_LSD_INF_RVK_TKT_DISP_LRT_FILE                "\n   Input File Name To Save LRT           : "
#define VL10N_LSD_INF_RVK_TKT_DISP_SERVER_NAME             "\n   Input Server Name                     : "
#define VL10N_LSD_INF_RVK_TKT_DISP_RVK_PWD                 "\n   Input Password(8-32)                  : "
#define VL10N_LSD_INF_UTIL_ERR_RIGHTS                  "\n   Error: Insufficient Access Rights or Invalid Path."
#define VL10N_LSD_INF_DISP_RVK_TKT_CONTENTS            "\n   Displaying Contents of LRT:\n%s"
#define VL10N_LSD_INF_DISP_MSG_SAVING_LRT_FILE         "\n   Saving LRT to file:%s"
#define VL10N_LSD_INF_DISP_RVK_SUCC_MSG                    "\n   License Revocation has been successfully done.\n"
#define VL10N_LSD_INF_DISP_RVK_FILE_OVWRT_MSG              "\n   Do you want to overwrite file %s (y/n):"
#define VL10N_LSD_INF_RVK_PWD_UTIL_NAME_STR                "\t %s %s Revocation Password Generation Utility\n"
#define VL10N_LSD_INF_RVK_PWD_UTIL_LIB_LOAD_FAILSTR        "\n %s library initialization failed\n"
#define VL10N_LSD_INF_RVK_PWD_UTIL_LIB_INTERNAL_ERR        "\n %s library internal error\n"
#define VL10N_LSD_INF_RVK_PWD_DISP_FILE                    "\n   Input directory location to save password file : "
#define VL10N_LSD_INF_RVK_PWD_SUCCESS                      "\n   Password generated successfully in password file %s \n"

// Added for revocation decode
#define VL10N_LSD_INF_RT_FILE_STR          "\n\nReading Revocation Ticket from file: \"%s\"\n"


/* MAC Address */
#define VL10N_ERR_MAC_ADDR           "\n Incorrect MAC Address "
#define VL10N_LSD_INF_CAPACITY_NON_POOLED_STR      "Non-Pooled"
#define VL10N_LSD_INF_CAPACITY_POOLED_STR          "Pooled"
#define VL10N_LSD_LBL_LIC_CAPACITY_01_STR          "\n License Capacity             : "
#define VL10N_LSD_INF_INFINITE_STR                 "Infinite\n"
#define VL10N_LSD_LBL_LIC_CAPACITY_02_STR          "\n License Capacity             : %d"
#define VL10N_LSD_LBL_TOKEN_CAPACITY_01_STR        "\n Token Capacity               : "
#define VL10N_LSD_LBL_TOKEN_CAPACITY_02_STR        "\n Token Capacity               : %d\n"
#define VL10N_LSD_LBL_CAPACITY_TYPE_STR            "\n Capacity type                : "


/* End of lsdecode strings */

#define VL10N_CGN_INF_EXIT_STATUS_STR         "\nLscgen exited with status %d\n"
/* Do not translate */
#define VL10N_CGN_ERR_QUREY_INDEX_STR         "Error: query index <%d> query = <%08x> got <%08x> expected <%08x> retcode = <%d>"

#define VL10N_CGN_ERR_HARDWARE_KEY_STR        "ERROR: No Sentinel RMS Development Kit hardware key or your Sentinel RMS Development Kit hardware key or code generator are corrupt. Cannot proceed. Exiting.\n"
#define VL10N_CGN_ERR_RESRV_LIC_FILE_STR     "No reserved license file. Assuming defaults\n"
#define VL10N_CGN_ERR_PARSING_LIC_FL_01_STR     "Sentinel RMS Development Kit: Error while parsing file %s\n\t # of licenses reserved exceeds %ld, the maximum you have been licensed\n\t No licenses reserved for group: %s\n"
#define VL10N_CGN_ERR_PARSING_LIC_FL_02_STR     "Sentinel RMS Development Kit: Error while parsing file %s\n\t Number of groups exceeds maximum limit, which is %d\n\t No licenses reserved for group %s\n"
#define VL10N_CGN_ERR_RES_LIC_TOTAL_STR        "Sum of licenses reserved exceed total alloted\n"
#define VL10N_CGN_ERR_PAR_RESERVN_FL_01_STR       "Error in parsing the Reservation file - Could not get vendor feature name\n"
#define VL10N_CGN_ERR_PAR_RESERVN_FL_02_STR     "Error in parsing the Reservation file - version number\n"
#define VL10N_CGN_HDR_PRN_GP_TBL_STR            "---- Printing Grp Table ------\n"
#define VL10N_CGN_LBL_GP_TBL_01_STR             "\tNum Groups = %d\n"
#define VL10N_CGN_ERR_GP_TBL_01_STR             "\t\tStatus:%d TotRes:%ld TotUse:%ld NumUsers:%d GrpName:%s\n"
#define VL10N_CGN_ERR_GP_TBL_02_STR             "\t\t\tUserName:%s Status:%d  Include-Status:%d\n"
#define VL10N_CGN_ERR_CGEN_NOTALLOW_STR         "Not allowed to generate codes for feature %s.\n"
#define VL10N_CGN_INF_FTR_ALLOWED_STR          "The following features%cversions are allowed:\n"
#define VL10N_CGN_ERR_CGEN_NOTALLOW_FEAVER_STR  "Not allowed to generate codes for feature %s version %s.\n"
#define VL10N_CGN_ERR_RES_CAP_TOTAL_STR        "Sum of Capacity reserved exceed total alloted\n"
#define VL10N_CGN_ERR_PAR_RESERVN_FL_03_STR     "Error in parsing the Reservation file - Could not get capacity\n"

/* End of LSCGEN strings */

/*Lscgcln Utility Strings - [LSCG]*/

#define VL10N_LSCG_LBL_USAGE_STR            "\
  Usage: %s \n\
    -p [trial license-code-output-file]       (for RMS versions: 7.x and 8.0.x) \n\
    -c [commuter license-code-output-file]    (for all RMS versions) \n\
    -t [time tamper license-code-output-file] (for all RMS versions) \n\
    -g [grace license-code-output-file]       (for RMS versions:8.0.x, and above)\n\
    -r [revoke license-code-output-file]      (for RMS versions:8.0.x, and above)\n\
    -f [option-file path]                     (for all RMS versions) \n"

#define VL10N_LSCG_HDR_CODEGEN_VER_STR          "\
  Codegen version as per the RMS Dev Kit library used by your application:\n\
  Sentinel RMS Dev Kit 9.1.0 and above: codegen version 17 (default)\n\
  Sentinel RMS Dev Kit 9.0.0          : codegen version 16\n\
  Sentinel RMS Dev Kit 8.6.x          : codegen version 15\n\
  Sentinel RMS Dev Kit 8.5.x          : codegen version 14\n\
  Sentinel RMS Dev Kit 8.4.x          : codegen version 13\n\
  Sentinel RMS Dev Kit 8.2.3, 8.3.0   : codegen version 12\n\
  Sentinel RMS Dev Kit 8.1.x, 8.2.x   : codegen version 11\n\
  Sentinel RMS Dev Kit 8.0.x          : codegen version 10\n\
  SLM 7.3.x                           : codegen version 9\n\
  SLM 7.3.0                           : codegen version 8\n\
  SLM 7.2.x                           : codegen version 7\n"

#define VL10N_LSCG_PRM_CODEGEN_VER_STR          "\n  Enter the codegen version [7/8/9/10/11/12/13/14/15/16/17] (press enter for default): "

#define VL10N_LSCG_HDR_UTIL_STR             "\n  %s %s Cleaning Utility License Generation System\n" /* the %s refers to LS_PRODNAME, LS_VERSION */
#define VL10N_LSCG_PRM_UTIL_STR             "\n  Clean  Standalone or Network Information [s/n]  : "
#define VL10N_LSCG_PRM_LOCKSEL_STR          "\n  Client lock selector (hex)      : "
#define VL10N_LSCG_PRM_LOCKCODE_STR         "\n  Client locking code             : "
#define VL10N_LSCG_PRM_LOCKCODE_STR_OLD     "\n  Client locking code (hex)       : " /* Added as previous versions still require it */
#define VL10N_LSCG_PRM_START_MON_STR        "\n  License Start Month [1-12]      : "
#define VL10N_LSCG_PRM_START_DAY_STR        "\n  License Start Day [1-31]        : "
#define VL10N_LSCG_PRM_START_YEAR_STR       "\n  License Start Year [2006 - 2133]: "
#define VL10N_LSCG_PRM_FEATURE_STR          "\n  License Feature Name            : "
#define VL10N_LSCG_PRM_VERSION_STR          "\n  License Feature Version         : "
#define VL10N_LSCG_PRM_COMM_HOST_NAME_STR   "\
\n  Host name of client to be cleaned\n\
  or <enter> To remove all\n\
 (applicable only for codegen version less than 13): "

#define VL10N_LSCG_PRM_GRACE_FEATURE_STR    "\n  Grace license feature name      : "
#define VL10N_LSCG_PRM_GRACE_VER_STR        "\n  Grace license version           : "
#define VL10N_LSCG_PRM_REV_FEATURE_STR      "\n  Revoke license feature name     : "
#define VL10N_LSCG_PRM_REV_VER_STR          "\n  Revoke license version          : "

/*Lsclean Utility Strings - [LSCL]*/
#define VL10N_LSCL_HDR_UTIL_STR             "\n %s %s License Cleaning Utility\n"
//RMS 8.2:: Standalone Revocation: LPR Recovery- Start
//RMS 8.2:: VTL: LPR Recovery- Start
#ifdef _VMSWIN_
#define VL10N_LSCL_LBL_USAGE_STR "\n Usage: %s  <fix-type> | <license-file> [-p]\n\n\
\t<fix-type>    : -fixtrial: Recover from trial persistence corruption. \n\
\t\t      : -fixrevoke: Recover from standalone revocation persistence corruption. \n\
\t\t      : -fixvtl: Recover from Volume Transaction Licensing persistence corruption. \n\
\t\t      : -fixsercom: Recover from Commuter Licensing persistence corruption. \n\
\t\t      : -fixserrevoke: Recover from network revocation Licensing persistence corruption. \n\
\t\t      : -fixsertrial: Recover from trial LM persistence corruption. \n\
\t\t      : -fixservtl: Recover from Volume Transaction Licensing LM persistence corruption. \n\
\t\t      : -fixserusglog: Recover from Usage Log network persistence corruption. \n\
\t\t      : -fixstdusglog: Recover from Usage standalone persistence corruption. \n\
\t\t      : -fixusglogguid: Recover from Usage Log GUID persistence corruption. \n\
\t\t      : -fixusglogrollover: Recover from Usage log file rollover persistence corruption. \n\
\t\t      : -fixusglog: Recover from Common Usage Log for Network/Standalone persistence corruption. \n\
\t\t      : -fixcancellease: Recover from Cancel Lease persistence corruption. \n\
\t<license-file>: For cleaning commuter, grace, network revocation, timetamper persistence. \n\
\t-p\t      : Pauses before returning to prompt.\n"
#else
#define VL10N_LSCL_LBL_USAGE_STR "\n Usage: %s  <fix-type> | <license-file> [-p]\n\n\
\t<fix-type>    : -fixtrial: Recover from trial persistence corruption. \n\
\t\t      : -fixrevoke: Recover from standalone revocation persistence corruption. \n\
\t\t      : -fixvtl: Recover from Volume Transaction Licensing persistence corruption. \n\
\t\t      : -fixsercom: Recover from Commuter Licensing persistence corruption. \n\
\t\t      : -fixserrevoke: Recover from network revocation Licensing persistence corruption. \n\
\t\t      : -fixserusglog: Recover from Usage Log network persistence corruption. \n\
\t\t      : -fixstdusglog: Recover from Usage standalone persistence corruption. \n\
\t\t      : -fixusglogguid: Recover from Usage Log GUID persistence corruption. \n\
\t\t      : -fixusglogrollover: Recover from Usage log file rollover persistence corruption. \n\
\t\t      : -fixusglog: Recover from Common Usage Log for Network/Standalone persistence corruption. \n\
\t\t      : -fixcancellease: Recover from Cancel Lease persistence corruption. \n\
\t<license-file>: For cleaning commuter, grace, network revocation, timetamper persistence. \n\
\t-p\t      : Pauses before returning to prompt.\n"
#endif
//RMS 8.2:: VTL: LPR Recovery- Ends.
//RMS 8.2:: Standalone Revocation: LPR Recovery- Ends.

#define VL10N_LSCL_INF_COMM_STR             "\n %d. Cleaned commuter information for feature \"%s\" version \"%s\" from the system. \n"
#define VL10N_LSCL_INF_TRIAL_STR            "\n %d. Cleaned trial information for feature \"%s\" version \"%s\" from the system. \n"
#define VL10N_LSCL_INF_TAMP_STR             "\n %d. Cleaned time tampering information from the system\n"
#define VL10N_LSCL_INF_INSTALL_STR          "\n %d. Cleaned install information from the system\n"
#define VL10N_LSCL_ERR_INSTALL_STR          "\n %d. Install information not found in the system\n"
#define VL10N_LSCL_ERR_TRIAL_STR            "\n %d. No trial information with specified feature and version in use. \n"
#define VL10N_LSCL_ERR_LIC_EXHAUST_STR      "\n %d. LSClean license execution is exhausted.\n"
#define VL10N_LSCL_ERR_LIC_EXPIRE_STR       "\n %d. LSClean license is expired.\n"
#define VL10N_LSCL_ERR_LIC_INVALID_STR      "\n %d. Not a valid license code.\n"
#define VL10N_LSCL_ERR_LIC_VENDOR_STR       "\n %d. Licensed by a different vendor.\n"
#define VL10N_LSCL_ERR_LIC_COMMINFO_STR     "\n %d. No Commuter information with specified feature and version in use. \n"
#define VL10N_LSCL_ERR_LIC_UNAUTHORIZE_STR  "\n %d. Not authorized to run on this machine .\n"
#define VL10N_LSCL_ERR_INVALID_INF_STR      "\n %d. Error (%d) : Failed to clean the required information\n"
#define VL10N_LSCL_INF_CLEAN_STR            "\n Please refer to CLEAN.LOG file for details.\n"
#define VL10N_LSCL_ERR_RIGHTS_STR           "\n Insufficient rights to execute the program \n"
#define VL10N_LSCL_ERR_TIMETAMP_STR         "\n %d. Error (%d) : Error cleaning time tamper information from the system.\n"
#define VL10N_LSCL_ERR_RUN_SPECIFIC_DT_STR  "\n %d. Lsclean cannot execute due to date restriction.\n"
#define VL10N_LSCL_PRM_ENTER_CLIENT_PATH_STR        "\n Please enter the Commuter Client path  : "
#define VL10N_LSCL_PRM_REENTER_CLIENT_PATH_STR      "\n \"%s :%s\": Please reenter : "
#define VL10N_LSCL_ERR_REVOKE_STR1     "\n %d. No license revocation information for specified feature and version in use. \n"
#define VL10N_LSCL_ERR_REVOKE_STR2     "\n %d. Cleaned license revocation information for feature \"%s\" version \"%s\" from the system. \n"
#define VL10N_LSCL_ERR_GRACE_STR1     "\n %d. No grace license information with specified feature and version in use. \n"
#define VL10N_LSCL_ERR_GRACE_STR2     "\n %d. Cleaned grace license information for feature \"%s\" version \"%s\" from the system. \n"

#define  VL10N_LSCL_ERR_CLEAN_WRONGFILE_STR             "\n Error[%d] : Database not found for this vendor. \n"
#define  VL10N_LSCL_ERR_CLEAN_REPAIRNOTPERFORMED_STR    "\n Error[%d] : Repair has not been performed on this database. \n"
#define  VL10N_LSCL_ERR_CLEAN_REPAIRFAIL_STR            "\n Error[%d] : Repair failed on this database. \n"
#define  VL10N_LSCL_ERR_CLEAN_REPAIRCOMPLETELOSS_STR    "\n Error[%d] : Database repaired with complete data loss. \n"
#define  VL10N_LSCL_ERR_CLEAN_REPAIRWITHLOSS_STR        "\n Error[%d] : Database repaired with some data loss occurred. \n"
#define  VL10N_LSCL_ERR_CLEAN_MISMATCHFILES_STR         "\n Error[%d] : The magic number on the two files do not match. \n"
#define  VL10N_LSCL_ERR_CLEAN_REPAIRATTEMPTED_STR       "\n Error[%d] : Repair already attempted on this database. \n"
#define  VL10N_LSCL_ERR_CLEAN_BADRECORDSIZE_STR         "\n Error[%d] : Inconsistent record(s) found. \n"
#define  VL10N_LSCL_ERR_INTERNAL_FAIL_STR               "\n Error[%d] : Failed to repair database. \n"
#define  VL10N_LSCL_ERR_PATH_ERROR_STR                  "\n Error[%d] : Unable to get database path. \n"
#define  VL10N_LSCL_ERR_CLEAN_NOTRECOVERABLE_STR        "\n Error[%d] : No data can be recovered from the database. \n"
#define  VL10N_LSCL_ERR_GET_LICENSE_HASH_FAILED_STR     "\n Error[%d] : Unable to generate hash for license string. \n"
#define  VL10N_LSCL_ERR_KEY_INFO_SUCCESS_STR            "\n Successfully repaired the database information. \n"
#define  VL10N_LSCL_ERR_INIT_FILE_READ_FAIL_STR         "\n Error[%d] : Failed to read from database. \n"
#define  VL10N_LSCL_ERR_INIT_FILE_WRITE_FAIL_STR        "\n Error[%d] : Failed to write into database. \n"
#define  VL10N_LSCL_ERR_INIT_FILE_OPEN_FAIL_STR         "\n Error[%d] : Failed to open the database. \n"
#define  VL10N_LSCL_ERR_INSUFFICIENT_PERMISIONS_STR     "\n Error[%d] : Database have insufficient permissions. \n"
#define  VL10N_LSCL_ERR_NORESOURCES_STR                 "\n Error[%d] : Insufficient resources. \n"
#define  VL10N_LSCL_ERR_LOCK_ERROR_STR                  "\n Error[%d] : Failed to set resource lock on database. \n"
#define  VL10N_LSCL_ERR_INIT_CONFIG_FAIL_STR            "\n Error[%d] : Configuration error. \n"
#define  VL10N_LSCL_ERR_FILE_DOES_NOT_EXIST_STR         "\n %s file does not exist. \n"

/*{lsrvdown STRINGS - Acronym [LSD] }*/
#define VL10N_LSD_INF_STOP_STR               " Stopping %s."                 /* _tprintf */
#define VL10N_LSD_INF_STOPD_STR              "\n %s stopped.\n"              /* _tprintf */
#define VL10N_LSD_ERR_FAILSTOP_STR           "\n %s failed to stop.\n"       /* _tprintf */
#define VL10N_LSD_ERR_CTRLSRV_STR            " ControlService failed - %s\n" /* _tprintf */
#define VL10N_LSD_ERR_OPSRV_STR              " OpenService failed - %s\n"    /* _tprintf */
#define VL10N_LSD_ERR_OPSCMAN_STR            " OpenSCManager failed - %s\n"  /* _tprintf */
#define VL10N_LSD_HDR_UTIL_STR               "\n  %s %s Server Shutdown Utility\n"
#define VL10N_LSD_INF_AVAILABLE_STR          " VLSshutDownService() only available on Win32\b"
#define VL10N_LSD_INF_UNKNOWN_STR            " UNKNOWN REASON"
#define VL10N_LSD_INF_USAGE_STR              " Usage : %s hostname\n"
#define VL10N_LSD_INF_SHUTDOWN_STR           "\n lserv has been shut down on host %s.\n"
/* End of lsrvdown strings */


/*{lslic STRINGS - Acronym [LSL] }*/
#define VL10N_LSL_LBL_USAGE_STR             "Usage: %s |\n"
#define VL10N_LSL_HDR_USAGE_01_STR          "Add to license server only- \n"
#define VL10N_LSL_INF_OPT_01_STR            " -a  [license code to add ] |\n"
#define VL10N_LSL_INF_OPT_02_STR            " -f  [filename] \n\n"
#define VL10N_LSL_INF_OPT_03_STR            " -ad [license code with its Distribution Criteria] |\n"
#define VL10N_LSL_INF_OPT_04_STR            " -fd [license filename (containing license codes with Distribution Criteria)] |\n\n"
#define VL10N_LSL_HDR_USAGE_02_STR          "Add to license server and normal/redundant license file- \n"
#define VL10N_LSL_INF_OPT_05_STR            " -A  [license code to add] |\n"
#define VL10N_LSL_INF_OPT_06_STR            " -F  [filename] \n\n"
#define VL10N_LSL_INF_OPT_07_STR            " -Ad [license code with its Distribution Criteria] |\n"
#define VL10N_LSL_INF_OPT_08_STR            " -Fd [license filename (containing license codes with Distribution Criteria) ] |\n\n"
#define VL10N_LSL_HDR_USAGE_03_STR          "Remove feature from license server only- \n"
#define VL10N_LSL_HDR_USAGE_04_STR          "\nRemove license from license server and license file- \n"
#define VL10N_LSL_HDR_USAGE_05_STR          "\nSet trial license precedence- \n"
#define VL10N_LSL_INF_OPT_09_STR            " -df  [feature_name feature_version [capacity | NOLIMIT]] |\n"
#define VL10N_LSL_INF_OPT_10_STR            " -removeall |\n"
#define VL10N_LSL_INF_LICADD_STR            "\nLicense code \"%s\" added on %s\n"
#define VL10N_LSL_PRM_LICFILE_STR           "Enter License File Name> "
#define VL10N_LSL_PRM_FEATUE_STR            "Feature> "
#define VL10N_LSL_PRM_VERSION_STR           "Version> "
#define VL10N_LSL_PRM_HASH_STR              "License Hash> "
#define VL10N_LSL_PRM_PRECEDENCE_STR        "Precedence> "
#define VL10N_LSL_INF_LICDEL_STR            "License for \"%s\" version \"%s\" deleted from %s\n"
#define VL10N_LSL_ERR_LICDEL_STR            "%d:License not deleted from %s\n"
#define VL10N_LSL_PRM_DISTCRIT_STR          "\n Enter Distribution Criteria String: "
#define VL10N_LSL_HDR_UTIL_01_STR           "\n  %s %s License Addition/Deletion Utility\n"
#define VL10N_LSL_HDR_UTIL_02_STR           "\n%s License Addition/Deletion Utility\n"
#define VL10N_LSL_INF_LIC_FORWARD_STR       "\nLicense forwarded to the leader server\n"

 /* WT# 83140: Extra command line option for a pause before returning
    * to the command line
    * Date fixed: June 06 2007
    * Version: 8.0.7
    */
#define VL10N_LSL_INF_OPT_14_STR            " -P  [Pauses before returning to console ] \n\n"

/* Capacity licensing
 * SLM 7.3.0
 */
#define VL10N_LSL_PRM_CAPACITY_STR          "Capacity> "
#define VL10N_LSL_INF_LICDEL_CAP_01_STR     "License for \"%s\" version \"%s\" capacity \"Infinite\" deleted from %s\n"
#define VL10N_LSL_INF_LICDEL_CAP_02_STR     "License for \"%s\" version \"%s\" capacity \"%lu\" deleted from %s\n"
#define VL10N_LSL_INF_LICDEL_01_STR         "\nlicense %s %s are deleted.\n"
#define VL10N_LSL_INF_LICDEL_02_STR         "\ndelete license %s %s failure.\n"
#define VL10N_LSL_INF_LICSETP_01_STR        "\nset license %s %s precedence as %d success.\n"
#define VL10N_LSL_INF_LICSETP_02_STR        "\nset license %s %s precedence failure.\n"
#define VL10N_LSL_INF_LICSETP_03_STR        "precedence value is invalid.\n"
#define VL10N_LSL_INF_LICSETP_04_STR        "License Hash is invalid.\n"
#define VL10N_LSL_INF_OPT_11_STR            " -removeallcap [feature_name feature_version] \n"
#define VL10N_LSL_PRM_DEL_WARNING_STR       "This will delete license(s) from the server, do you want to continue? (Y/N): "

#define VL10N_LSL_INF_OPT_12_STR            " -DL [feature_name feature_version license_hash [capacity | NOLIMIT]] \n\n"
#define VL10N_LSL_INF_OPT_13_STR            " -p [feature_name feature_version license_hash precedence] \n\n"
#define VL10N_LSL_INF_OPT_15_STR            " Use LSFORCEHOST environment variable to specify the contact server.\n"

/* Upgrade licensing.
 * SLM 7.3.0
 */
#define VL10N_LSL_ERR_CANT_DEL_UPGRADED_LIC_STR  "Deletion of upgraded feature/license is not allowed.\n"

/* End of lslic strings */



/*{enctordc STRINGS - Acronym [ETR] }*/
#define VL10N_ETR_HDR_UTIL_STR              "\n  %s %s(%d) Encrypted License To Readable License Converter\n"
#define VL10N_ETR_ERR_REMAP_STR             "Remapped default fixed strings from file %s\n"
#define VL10N_ETR_PRM_LICSTR_STR            "Enter an encrypted license code (or your EOF character to quit):\n"
#define VL10N_ETR_ERR_NOTAUTHR_STR          "Error: Not authorized to convert: \"%s\"\n"
#define VL10N_ETR_ERR_SHORTTOREAD_STR       "Error: Cannot convert \"%s\" , a short code type string to readable form.\n"
#define VL10N_ETR_ERR_CAPACITYTOREAD_STR    "Error: Cannot convert \"%s\" , a capacity license string to readable form.\n"
#define VL10N_ETR_ERR_TOOOLD_STR            "Error: Expired license \"%s\" is too old to be converted.\n"
#define VL10N_ETR_ERR_UNABLE_STR            "Error: Unable to convert license code: \"%s\"\n"
#define VL10N_ETR_LBL_USAGE_STR             "Usage: %s [-F] [In-file-name] \n"
#define VL10N_ETR_INF_USAGE_01_STR          "       If no In-file-name is specified, input is taken from standard input.\n"
#define VL10N_ETR_INF_USAGE_02_STR          "       If the -F option is specified, expanded readable license codes are\n"
#define VL10N_ETR_INF_USAGE_03_STR          "       generated, otherwise concise strings are generated.\n"
#define VL10N_ETR_INF_USAGE_04_STR          "       Output license codes are always written to standard out (stdout).\n"
#define VL10N_ETR_INF_USAGE_05_STR          "       Lines in the input that are not recognized as encrypted strings are\n"
#define VL10N_ETR_INF_USAGE_06_STR          "       written to the output unchanged.\n"
#define VL10N_ETR_INF_USAGE_07_STR          "       Use the environment variable %s to give the location of a\n"
#define VL10N_ETR_INF_USAGE_08_STR          "       license configuration file, if any. \n"
/* End of enctordc strings */


/*{lswhere STRINGS - Acronym [LSW] }*/
#define VL10N_LSW_INF_USAGE_01_STR          "<-d> <-r>\nwhere, \n"
#define VL10N_LSW_INF_USAGE_02_STR          "\t-d: Display Server Details.\n"
#define VL10N_LSW_INF_USAGE_03_STR          "\t-r: Display IP address only."
#define VL10N_LSW_HDR_UTIL_STR              "\n   %s %s Search Tool For License Servers\n"
#define VL10N_LSW_INF_SRV_01_STR            "\n    Server Address     : %s"
#define VL10N_LSW_INF_SRV_02_STR            "\n    Server Name        : %s"
#define VL10N_LSW_INF_SRV_03_STR            "\n    Is Redundant ?     : %s"
#define VL10N_LSW_INF_SRV_04_STR            "\n    Total Clients      : %lu"
#define VL10N_LSW_INF_SRV_05_STR            "\n    Protocols          : "
#define VL10N_LSW_INF_PROTOCOL_01_STR       "TCP/IP only\n"
#define VL10N_LSW_INF_PROTOCOL_02_STR       "IPX only\n"
#define VL10N_LSW_INF_PROTOCOL_03_STR       "Both TCP/IP and IPX\n"
/* End of lswhere strings */

/* LMSRV STRINGS - Acronym [SRV] */
#define VL10N_SRV_INF_MAINMENU_STR          "MainMenu"
#define VL10N_SRV_INF_MAINICON_STR          "MAINICON"

#ifdef _VMSWIN_
#ifdef _ANONYMOUS_USAGE_LOG_ 
#define VL10N_SRV_INF_COMMAND_USUAGE_STR    "\n Usage: %s\n\
       [-%c License-file]     [-%c License-configuration-file] \n\
       [-b  Start the server as service]\n\
       [-%c Usage-log-file]   [-%c Usage-log-file-size] \n\
       [-%c No-log-file-truncation]\n\
       [-%c Error-msg-file]   [-%c Group-reservations-file]\n\
       [-%c Quiet Mode]\n\
       [-%s port-number]   [-%s Redundant-license-file]\n\
       [-%s percentage-of-commuter-keys-allowed]\n\
       [-%s Encryption-level <1..4>]   [-%s Upgrade-license-file]\n\
       [-%s Switch ON Anonymous-logging]\n\
       [-%s Switch ON Extended-logging]\n\
       [-%s Socket-Buffersize-Multiplier]\n\
       %s"
#else
#define VL10N_SRV_INF_COMMAND_USUAGE_STR    "\n Usage: %s\n\
       [-%c License-file]     [-%c License-configuration-file] \n\
       [-b  Start the server as service]\n\
       [-%c Usage-log-file]   [-%c Usage-log-file-size] \n\
       [-%c No-log-file-truncation]\n\
       [-%c Error-msg-file]   [-%c Group-reservations-file]\n\
       [-%c Quiet Mode]\n\
       [-%s port-number]   [-%s Redundant-license-file]\n\
       [-%s percentage-of-commuter-keys-allowed]\n\
       [-%s Encryption-level <1..4>]   [-%s Upgrade-license-file]\n\
       [-%s Socket-Buffersize-Multiplier]\n\
       %s"

#endif  //_ANONYMOUS_USAGE_LOG_
#else   // _VMSWIN_
#ifdef _ANONYMOUS_USAGE_LOG_ 
#define VL10N_SRV_INF_COMMAND_USUAGE_STR    "\n Usage: %s\n\
       [-%c License-file]     [-%c License-configuration-file] \n\
       [-%c Usage-log-file]   [-%c Usage-log-file-size] \n\
       [-%c No-log-file-truncation]\n\
       [-%c Error-msg-file]   [-%c Group-reservations-file]\n\
       [-%c Quiet Mode]       [-%s Trace-level <4|2|1|7>]\n\
       [-%s port-number]   [-%s Redundant-license-file]\n\
       [-%s percentage-of-commuter-keys-allowed]\n\
       [-%s Encryption-level <1..4>]   [-%s Upgrade-license-file UPGRADE_FILE_OPT_STRING]\n\
       [-%s Switch ON Anonymous-logging]\n\
       [-%s Switch ON Extended-logging]\n\
       [-%s Socket-Buffersize-Multiplier]\n\
       %s"
#else
#define VL10N_SRV_INF_COMMAND_USUAGE_STR    "\n Usage: %s\n\
       [-%c License-file]     [-%c License-configuration-file] \n\
       [-%c Usage-log-file]   [-%c Usage-log-file-size] \n\
       [-%c No-log-file-truncation]\n\
       [-%c Error-msg-file]   [-%c Group-reservations-file]\n\
       [-%c Quiet Mode]       [-%s Trace-level <4|2|1|7>]\n\
       [-%s port-number]   [-%s Redundant-license-file]\n\
       [-%s percentage-of-commuter-keys-allowed]\n\
       [-%s Encryption-level <1..4>]   [-%s Upgrade-license-file UPGRADE_FILE_OPT_STRING]\n\
       [-%s Socket-Buffersize-Multiplier]\n\
       %s"

#endif //_ANONYMOUS_USAGE_LOG_
#endif //_VMSWIN_

#define VL10N_TRC_SRV_ERR_HOSTNAME_STR       "gethostname()failed.\n"
#define VL10N_TRC_SRV_ERR_HOSTBYNAME_STR     "gethostbyname(%s) failed. Continuing.\n"
#define VL10N_TRC_SRV_ERR_CFG_FILE_STR       "Couldn't find(read) server configuration file. Acting NON-REDUNDANT.\n"
#define VL10N_TRC_SRV_ERR_IPSOCKET_STR       "Server IP socket open failed\n"
#define VL10N_TRC_SRV_LBL_IPSOCKET_STR       "IP socket: %ld\n"
#define VL10N_TRC_SRV_ERR_IPXSOCKET_STR      "Server IPX socket open failed\n"
#define VL10N_TRC_SRV_LBL_IPXSOCKET_STR      "IPX socket: %ld\n"
#define VL10N_TRC_SRV_ERR_IPBIND_STR         "Server couldn't bind to socket\n"
#define VL10N_TRC_SRV_LBL_IPBIND_STR         "bind: %ld\n"
#define VL10N_TRC_SRV_ERR_IPXBIND_STR        "Server couldn't bind to socket\n"
#define VL10N_TRC_SRV_LBL_IPXBIND_STR        "bind: %ld\n"
#define VL10N_TRC_SRV_LBL_LOGSUSPEND_STR     "%s : File \"%s\" (%lu) exceeds limit of %lu, logging suspended.\n"
#define VL10N_TRC_SRV_LBL_LEN_PACKET_STR     "Length of the packet received = %ld\n"
#define VL10N_TRC_SRV_LBL_TIMEOUT_STR        "Timeout : %d\n"
#define VL10N_TRC_SRV_LBL_NON_REDUNDANT_STR  "NON_REDUNDANT server. Ignoring server msg.\n"

#define VL10N_SRV_ERR_OTHER_LIC_STR           "%s: Another license server running?\n"
/* Example: Sentinel RMS License Manager: Another license server running? */

#define VL10N_SRV_ERR_STARTUP_STR             "%s: Could not start server.\n"

#define VL10N_SRV_LBL_LM_STR                  "  %s Centralized License Manager v%s\n%s"
#define VL10N_SRV_INF_STATUS_STR              "\nSentinel RMS License Manager\n\n  Status:  Running"
#define VL10N_SRV_ERR_UNABLE_BLOCKSIGNAL_STR "signal(): Unable to block signal #%d: "
/* Do not translate signal() */

#define VL10N_SRV_ERR_UNABLE_SET_HANDLR_STR  "signal(): Unable to set up handler for signal #%d: "
/* Do not translate signal() */

#define VL10N_SRV_HDR_LMSRVR_STR             "\n         %s %s License Management Server\n"
/* Example: Sentinel RMS Development Kit 7.1.0 License Management Server */

#define VL10N_SRV_INF_INITIALIZING_STR      "Initializing.  Please wait ...\n"
#define VL10N_SRV_ERR_CNNOTOPN_ERRMSGFL_01_STR "Warning: Cannot open error message file \"%s\". Wrong path?\n"
#define VL10N_SRV_ERR_SGNLHNDLR_NOSETUP_STR "Warning: signal handlers not set up properly.\n"
#define VL10N_SRV_ERR_CNNOTOPN_ERRMSGFL_02_STR "%s: Could not open error message file."
#define VL10N_SRV_INF_LOCLHOST_STR         "Running on localhost\n"
#define VL10N_SRV_INF_HOST_STR             "Running on host %s\n"
#define VL10N_SRV_INF_RCVD_STOP_SERVER     "Received stop server message, lserv has been shut down.\n"
#define VL10N_SRV_INF_START_OPTION_VALUE   "The Sentinel RMS License Manager has started with %s value set to %d %s.\n"

/* End of lmsrv strings */

/*{lcftool STRINGS - Acronym [LCF] }*/

#define VL10N_LCF_INF_NO_SRVNAME_STR        "\nNo Server Names defined in Global Section of \"%s\""
#define VL10N_LCF_INF_NO_POOLNAME_STR       "\nNo Pool Name defined in Global Section of %s."
#define VL10N_LCF_INF_NO_TIMESTAMP_STR      "\nNo Time Stamp defined in Global Section of %s."
#define VL10N_LCF_INF_NO_SEQNUM_STR         "\nNo Sequence number defined in Global Section of %s."
#define VL10N_LCF_INF_SUCCESS_STR           "SUCCESS!"
#define VL10N_LCF_ERR_INV_FILE_STR          " Invalid file open option."
#define VL10N_LCF_ERR_INV_ARG_STR           " Argument passed has invalid value."
#define VL10N_LCF_ERR_INV_SEQNUM_STR        " Invalid Sequence Number."
#define VL10N_LCF_ERR_INV_TIMESTMP_STR      " Invalid Time Stamp."
#define VL10N_LCF_ERR_INV_FILEOPEN_STR      " File Opened in READ MODE."
#define VL10N_LCF_ERR_SECT_NOTFOUND_STR     " Section not found."
#define VL10N_LCF_ERR_INV_FILENAME_STR      " Invalid File Name."
#define VL10N_LCF_ERR_INV_LICSTR_STR        " Invalid license code."
#define VL10N_LCF_ERR_FILE_NOTFOUND_STR     " File Not Found."
#define VL10N_LCF_ERR_INTERR_STR            " Internal Error in Library."
#define VL10N_LCF_ERR_GLBLSECT_STR          " Global Section not present."
#define VL10N_LCF_ERR_NOREAD_PER_STR        " No read permission given."
#define VL10N_LCF_ERR_NOWRITE_PER_STR       " No write permission given."
#define VL10N_LCF_ERR_EOF_STR               " End of File."
#define VL10N_LCF_ERR_SECT_PRESENT_STR      " Section already present."
#define VL10N_LCF_ERR_RLF_LIBINIT_STR       " RLF Library Already Initialized."
#define VL10N_LCF_ERR_MISMATCH_01_STR       " Mismatch in Global Section vs License Section content."
#define VL10N_LCF_ERR_INV_FIELDVAL_STR      " One of the fields has an invalid value."
#define VL10N_LCF_ERR_INV_IPFORMAT_STR      " IP Address Format is invalid."
#define VL10N_LCF_ERR_MISMATCH_02_STR       " Mismatch in server names and IP addresses."
#define VL10N_LCF_ERR_MISMATCH_03_STR       " Checksum Mismatch. File Tampered!"
#define VL10N_LCF_ERR_INV_PREFORD_STR       " Invalid Preference Order in Global Section."
#define VL10N_LCF_ERR_INV_SECMODEL_STR      " Invalid Security Model in Global Section."
#define VL10N_LCF_ERR_INV_MAJFLAG_STR       " Invalid Majority Flag value in Global Section."
#define VL10N_LCF_ERR_INV_POOLNAME_STR      " Invalid Pool Name in Global Section."
#define VL10N_LCF_ERR_INV_RLFHAND_STR       " Invalid RLF Library Handle."
#define VL10N_LCF_ERR_LIC_TYPE_01_STR       " License is STANDALONE type."
#define VL10N_LCF_ERR_LIC_TYPE_02_STR       " License is TRIAL type."
#define VL10N_LCF_ERR_LIC_TYPE_03_STR       " License is Non-redundant type."
#define VL10N_LCF_ERR_INV_ERRCODE_STR       "Error: Invalid Error Code[%d]"
#define VL10N_LCF_INF_INVALID_CONFIGFILE_STR "This file [%s] is not a valid redundant license file."
#define VL10N_LCF_INF_NOWRITE_FILE_STR      "The file %s does not have write permissions."
#define VL10N_LCF_PRM_CONFIG_FILE_STR       "\n Enter another redundant license file name: "
#define VL10N_LCF_PRM_FILENAME_STR          " Enter File Name: "
#define VL10N_LCF_PRM_FILE_CREATE_STR       "File %s does not exist. Create New (Y/N) or Quit(Q): "
#define VL10N_LCF_PRM_FILE_CLOSE_STR        "Do you want to close the previously loaded file [%s]? (Y/N):"
#define VL10N_LCF_INF_FILELOAD_STR          "File [%s] loaded.\n"
#define VL10N_LCF_INF_TAMPERED_REDUNDANTFILE_STR "Error in opening the file [%s].This file is an invalid redundant file."

#define VL10N_LCF_HDR_MENU_01_STR           "\n                 [RLFTOOL MAIN  MENU]\n"
#define VL10N_LCF_INF_MENU_01_STR           "%2d. OPEN                   Open or Create New Redundant License File.\n"
#define VL10N_LCF_INF_MENU_02_STR           "%2d. ADD/DELETE SERVER      Add or Delete Server Names in pool.   \n"
#define VL10N_LCF_INF_MENU_03_STR           "%2d. POOL NAME              Edit pool name.                       \n"
#define VL10N_LCF_INF_MENU_04_STR           "%2d. PREFERENCE ORDER       Change or View Preference Order.      \n"
#define VL10N_LCF_INF_MENU_05_STR           "%2d. SEQUENCE NUMBER        Change file sequence number.          \n"
#define VL10N_LCF_INF_MENU_06_STR           "%2d. TIME STAMP             Change file timestamps.                    \n"
#define VL10N_LCF_INF_MENU_07_STR           "%2d. ADD LICENSE            Add new license.              \n"
#define VL10N_LCF_INF_MENU_08_STR           "%2d. VIEW LICENSE           View Licenses.                \n"
#define VL10N_LCF_INF_MENU_09_STR           "%2d. DELETE LICENSE         Delete license.                       \n"
#define VL10N_LCF_INF_MENU_10_STR           "%2d. CONFLICTS              List or Resolve license Distribution conflicts.\n"
#define VL10N_LCF_INF_MENU_11_STR           "%2d. REPORTS                Generate Licenses reports.            \n"
#define VL10N_LCF_INF_MENU_12_STR           "%2d. EXIT AND SAVE          Terminate tool.                       \n"
#define VL10N_LCF_PRM_SELECTOPT_STR         "\nSelect Option (%d..%d): "

#define VL10N_LCF_INF_SRVNAME_STR           "\nServer Names according to ascending preference order in the Pool:"
//#define VL10N_LCF_HDR_SRVMENU_IPMODE        "Select 1 for ipv6 mode hostname based config else default mode is ipv4"
#define VL10N_LCF_HDR_SRVMENU_01_STR        "\n\n[ SERVER MENU ]"
#define VL10N_LCF_INF_SRVMENU_01_STR        "\n1. Add New Server"
#define VL10N_LCF_INF_SRVMENU_02_STR        "\n2. Delete Server"
#define VL10N_LCF_INF_SRVMENU_03_STR        "\n3. Back to Main Menu"
#define VL10N_LCF_PRM_SELECT_STR            "\nSelect (1..%d): "

#define VL10N_LCF_ERR_SRVEXIST_STR          "[%s] server name already exists in pool.\n"
#define VL10N_LCF_ERR_INV_IPADD_STR         "INVALID IP Address!!!\n"
#define VL10N_LCF_ERR_IPEXIST_STR           "[%s] IP Address already exists in pool.\n"
#define VL10N_LCF_PRM_SRVADD_STR            "Do you want to add this server ?(y/n) :"
#define VL10N_LCF_INF_SRVADD_FAIL_STR       "Fails to add Server"
#define VL10N_LCF_PRM_SRVNAME_01_STR            "\nEnter the Server Name to be deleted: "
#define VL10N_LCF_INF_NOTFOUND_SRVNAME_STR  "\nServer name not found\n"
#define VL10N_LCF_PRM_SRVDEL_STR            "Do you want to delete this server ?(y/n) :"
#define VL10N_LCF_INF_ONE_SRVDEL_STR        "Only One server present in the list. Cannot delete Server\n"
#define VL10N_LCF_INF_SRVDEL_FAIL_STR       "Fails to delete server\n"
#define VL10N_LCF_INF_NOSRV_INPOOL_STR      "No Server Present in the Pool\n"
#define VL10N_LCF_PRM_CHANGE_STR            "Do you want to change it ?(y/n): "
#define VL10N_LCF_PRM_NEW_PREF_STR          "Enter new preference order for %s[1..%d]: "
#define VL10N_LCF_PRM_VAL_AGAIN_01_STR      "Please enter this value again[1..%d]: "
#define VL10N_LCF_PRM_VAL_AGAIN_02_STR      "Please enter again !! Value is repeating\n"
#define VL10N_LCF_PRM_POOLNAME_01_STR       "Enter Pool Name [Max %d chars]: "
#define VL10N_LCF_INF_POOLNAME_STR          "Pool Name: %s"
#define VL10N_LCF_PRM_POOLNAME_02_STR       "\nEnter pool name [Max %d chars] or Press <Enter> to keep %s : "
#define VL10N_LCF_ERR_INV_NAME_STR          "Invalid Name!"
#define VL10N_LCF_INF_PLNM_CHANGED_STR      "Pool name successfully changed."
#define VL10N_LCF_INF_PLNM_FAIL_STR         "Failed to set pool name."
#define VL10N_LCF_INF_SEQNO_01_STR          "\n Warning: If you change the file creation Sequence Number, \nyou have to copy this file to all servers in the pool, otherwise,\neach server will be unsynchronized for a few minutes or hours \n(depending on network speed).\n"
#define VL10N_LCF_INF_TMSTMP_01_STR         "\n Warning: If you change the file creation Time Stamp,\nyou have to copy this file to all servers in the pool, otherwise,\neach server will be unsynchronized for a few minutes or hours \n(depending on network speed).\n"
/* Please ignore SEQNO_02 through SEQNO_04 */
#define VL10N_LCF_INF_SEQNO_02_STR          ""
#define VL10N_LCF_INF_SEQNO_03_STR          ""
#define VL10N_LCF_INF_SEQNO_04_STR          ""
#define VL10N_LCF_INF_SEQNO_STR             "\nThe Sequence Number is %d"
#define VL10N_LCF_PRM_SEQNO_STR             "\nEnter the Sequence Number or Press <Enter> to Keep %d :"
#define VL10N_LCF_INF_SEQNO_05_STR          "Sequence Number should be greater than 0\n"
#define VL10N_LCF_INF_SEQNO_06_STR          "Sequence Number should be numeric\n"
#define VL10N_LCF_PRM_YEAR_STR              "\nEnter Year (e.g. 2000): "
#define VL10N_LCF_PRM_MONTH_STR             "Enter the month (1..12, January = 1): "
#define VL10N_LCF_PRM_DAY_STR               "Enter the day of month (1 - %d): "
#define VL10N_LCF_PRM_HOURS_STR             "Enter the Hours After Midnight (0 - 23): "
#define VL10N_LCF_PRM_MINUTES_STR           "Enter the Minutes after the hour (0 - 59): "
#define VL10N_LCF_PRM_SECONDS_STR           "Enter the Seconds after the minute (0 - 59): "
#define VL10N_LCF_INF_TIMESTAMP_STR         "\nThe time stamp setting (in UTC) is: %s"
#define VL10N_LCF_PRM_TMSTMP_CHANGE_STR     "\nDo you want to change it ?(y/n) : "
#define VL10N_LCF_PRM_TMSTMP_STR            "\nEnter the new time stamp (in UTC)"
#define VL10N_LCF_INF_NEW_TMSTMP_STR        "New Time Set: %s"
#define VL10N_LCF_ERR_FAIL_TMSTMP_STR       "Fails to set new time."
#define VL10N_LCF_INF_FSTLD_CONFIGFILE_STR  "\nFirst load redundant license file and then Try this option"

#define VL10N_LCF_HDR_CONFLICTS_MENU_01_STR "\n\n[ CONFLICTS MENU ]"
#define VL10N_LCF_INF_CONFLICTS_MENU_01_STR "\n1. Resolve Conflicts"
#define VL10N_LCF_INF_CONFLICTS_MENU_02_STR "\n2. Generate conflicts in a file ?"
#define VL10N_LCF_INF_CONFLICTS_MENU_03_STR "\n3. Back to Main Menu"
#define VL10N_LCF_PRM_CONFLICTS_MENU_STR    "\nSelect (1..3): "

#define VL10N_LCF_PRM_REP_FILENME_STR       "\nEnter Report Filename: "
#define VL10N_LCF_ERR_NO_CONFIGFILE_STR     "No redundant license file is selected. Load the file first."
#define VL10N_LCF_PRM_THRESHOLD_LMT_STR     "\nEnter Threshold Limit[Default=%d]: "
#define VL10N_LCF_ERR_SRVNOT_POOL_STR       "\n Server [%s] not present in pool"
/* Example: Server [MyComputer] not present in pool */

#define VL10N_LCF_ERR_INV_DIST_CRIT_STR     "\nINVALID DISTRIBUTION CRITERION"
#define VL10N_LCF_PRM_SRV_SHARING_STR       "Server [%s] is redundant or not ?(Y/N): "
#define VL10N_LCF_PRM_SRV_ALLOCATION_STR    "Enter allocation for Server:%s [0..%u]: "
#define VL10N_LCF_ERR_INV_NORANGE_STR       "Invalid Number Range[0..%u]: "

#define VL10N_LCF_PRM_DISTCRIT_CORRECT_STR  "\n Is this distribution criteria correct ? ( Y )  or ( N ) : "
#define VL10N_LCF_ERR_FAIL_LICADD_STR       "\nFailed to add license. "
#define VL10N_LCF_INF_LICADD_STR            "\nLicense Added to File."

#define VL10N_LCF_HDR_ADDLIC_MENU_01_STR    "\n[ADD LICENSE MENU]"
#define VL10N_LCF_INF_ADDLIC_MENU_01_STR    "\n%2d.              Pick Licenses from file. "
#define VL10N_LCF_INF_ADDLIC_MENU_02_STR    "\n%2d.              License code on command line."
#define VL10N_LCF_INF_ADDLIC_MENU_03_STR    "\n%2d.              Back to main menu."
#define VL10N_LCF_PRM_ADDLIC_MENU_STR       "\nSelect Option (%d..%d): "

#define VL10N_LCF_ERR_NO_LICSECT_STR        "\nNo License Section(s) found\n"
#define VL10N_LCF_PRM_FEATURE_NAME_STR      "Enter the feature name: "
#define VL10N_LCF_PRM_VERNO_STR             "Enter Version Number: "
#define VL10N_LCF_PRM_EDIT_STR              "Do you want to move To Next license/ Quit ?(n/q)"
#define VL10N_LCF_PRM_FEATNAME_STR       "\nEnter the license feature name (Should not be Empty): "

#define VL10N_LCF_HDR_DELLIC_MENU_STR       "\nSelect Option :"
#define VL10N_LCF_INF_DELLIC_MENU_01_STR    "\n[1]             Delete One License."
#define VL10N_LCF_INF_DELLIC_MENU_02_STR    "\n[2]             Delete all matching Licenses."
#define VL10N_LCF_INF_DELLIC_MENU_03_STR    "\n[3]             Back to Main Menu."
#define VL10N_LCF_PRM_DELLIC_MENU_STR       "\nSelect (1..3) :"
#define VL10N_LCF_PRM_DELLIC_STR            "\nDo you want to delete the License ?(y/n) :"
#define VL10N_LCF_INF_DELLIC_STR            "\n Deleting Licenses with Feature: %s, Version : %s "

#define VL10N_LCF_HDR_LICDETAILS_STR        "\n\t\tLICENSE DETAILS:"
#define VL10N_LCF_LBL_VERNO_STR             "\tVersion Number    : %s"
#define VL10N_LCF_LBL_LIC_TYPE_STR          " License type                : "
#define VL10N_LCF_LBL_LOCKCRIT_STR          "\n Locking Criterion\t     : "
#define VL10N_LCF_INF_STDALONE_STR          "Standalone License"
#define VL10N_LCF_INF_NETLIC_STR            "Network License"
#define VL10N_LCF_INF_SRVLCKD_STR           "Server Locked"
#define VL10N_LCF_INF_SRVCLLCKD_STR         "Server/Client Locked"
#define VL10N_LCF_INF_CLLCKD_STR            "Client Locked"
#define VL10N_LCF_INF_ANYHOST_STR           "Any Host"
#define VL10N_LCF_INF_UNKNOWN_STR           "Unknown"
#define VL10N_LCF_PRM_CHANGE_DISTCRIT_STR   "\nDo you want to change Distribution Criteria ?(y/n/q): "
#define VL10N_LCF_ERR_NOTWRI_LICSECT_STR    "Not able to Write License Section"
#define VL10N_LCF_INF_NO_CONFLICTS_STR      "\nNo Conflicts Found"
#define VL10N_LCF_INF_FILE_CLOSE_STR        "The file 'data' was not closed\n"

#define VL10N_LCF_HDR_REPORT_MENU_01_STR    "\nReport Menu"
#define VL10N_LCF_INF_REPORT_MENU_01_STR    "\n%2d         Dump Redundant License File Details To a File"
#define VL10N_LCF_INF_REPORT_MENU_02_STR    "\n%2d         List License Sections Conflicts To a File"
#define VL10N_LCF_INF_REPORT_MENU_03_STR    "\n%2d         Back to Main Menu"
#define VL10N_LCF_PRM_REPORT_MENU_STR       "\nSelect (%d..%d): "
/* Example: Select (1 .. 4) */

#define VL10N_LCF_LBL_USAGE_STR             "\nUsage: "
#define VL10N_LCF_INF_USAGE_01_STR          "\nrlftool -l <filename>           [Load/Create Redundant License File]"
#define VL10N_LCF_INF_USAGE_02_STR          "\n -a <server name> <ip address>  [Add server name and IP Address"\
                                            "\n                                (Pass Dyn_IPv4 for Dynamic IPv4 and"\
											"\n                                 Dyn_IPv6 for Dynamic IPv6 Address)]"
#define VL10N_LCF_INF_USAGE_03_STR          "\n -d <server name>               [Delete server name]"
#define VL10N_LCF_INF_USAGE_04_STR          "\n -p <pool name>                 [Change Pool Name]"
#define VL10N_LCF_INF_USAGE_05_STR          "\n -s <sequence number>           [Change Sequence Number]"
#define VL10N_LCF_INF_USAGE_06_STR          "\n -t <time stamp>                [Modify Time Stamp. Format:YYYYMMDDHHmm]"
#define VL10N_LCF_INF_USAGE_07_STR          "\n -R <report filename>           [Dump Redundant License file contents"\
                                            "\n                                 to given file]"
#define VL10N_LCF_INF_USAGE_08_STR          "\n -A <license code>              [Add new license code ]"
#define VL10N_LCF_INF_USAGE_09_STR          "\n -F <license filename>          [Add license from file]"
#define VL10N_LCF_INF_USAGE_10_STR          "\n -D <feature name> <version>    [Delete License Section ]"
#define VL10N_LCF_INF_USAGE_11_STR          "\n -C <conflicts report filename> [List Conflicts]\n"

#define VL10N_LCF_ERR_SRVNAME_NULL_STR      "\nServer name could not be null"
#define VL10N_LCF_ERR_SRVIP_NULL_STR        "\nServer IP Address could not be null"
#define VL10N_LCF_ERR_FEATURENAME_NULL_STR  "\nFeature name could not be null"
#define VL10N_LCF_ERR_READ_LICSTR_STR       "\n Unable to read license code\n"
#define VL10N_LCF_ERR_MISS_LICFILENAME_STR  "\n Missing license file name.\n"
#define VL10N_LCF_ERR_FLOPEN_CONFIGFILE_STR  "Failed to open %s redundant license file. Exiting..."
#define VL10N_LCF_ERR_FLCREATE_CONFIGFILE_STR "Fails to create %s redundant license file. Exiting..."
#define VL10N_LCF_ERR_FAIL_SRVADD_STR       "\nFailed to add server"
#define VL10N_LCF_ERR_POOLNAME_STR          "\nPool name length should be less than 8"
#define VL10N_LCF_ERR_FAIL_CNGPOOLNAME_STR  "\nFailed to change Pool Name"
#define VL10N_LCF_INF_CNGPOOLNAME_STR       "\nPool Name successfully changed"
#define VL10N_LCF_ERR_FAIL_CNGSEQNO_STR     "\nFailed to change sequence number"
#define VL10N_LCF_INF_CNGSEQNO_STR          "\nSequence number successfully changed"
#define VL10N_LCF_ERR_SEQNO_STR             "\nSequence number cannot be zero/negative/character"
#define VL10N_LCF_ERR_FAIL_CNGTMSTMP_STR    "\nFailed to change Time Stamp"
#define VL10N_LCF_INF_CNGTMSTMP_STR         "\nTime Stamp successfully changed"
#define VL10N_LCF_ERR_NOLICSTR_STR          "No license code found\n"
#define VL10N_LCF_ERR_DELLIC_STR            "\nNot able to delete licenses with feature: %s, version : %s "
#define VL10N_LCF_INF_DELTDLIC_STR          "\nLicenses with Feature: %s, Version : %s successfully deleted "
#define VL10N_LCF_INF_LICADDED_STR          "\nLicense successfully added"
#define VL10N_LCF_HDR_UTIL_STR              "\n  %s %s REDUNDANT LICENSE FILE TOOL\n"
/* Example: Sentinel RMS Development Kit 7.1.0 REDUNDANT LICENSE FILE TOOL */

#define VL10N_LCF_ERR_INV_CONFIGFILE_STR    "%s: Invalid redundant license file. Creating new..."
/* Example: filename.dat: Invalid redundant license file.  Creating new ... */

#define VL10N_LCF_PRM_SRVNAME_02_STR        "Enter server name : "
#define VL10N_LCF_PRM_SRV_IP_STR            "Enter IP address of server (Pass Dyn_IPv4 in case of Dynamic IPv4 and Dyn_IPv6 in case of Dynamic IPv6 Address): "
#define VL10N_LCF_LBL_HARD_LIMIT_STR        "\n Hard Limit\t\t     : %s  \t"

/* End of lcftool strings */

/*{echoid STRINGS - Acronym [ECO] }*/

#define VL10N_ECO_HDR_HOST_LOCKCODE_STR    "%s %s Host Locking Code Information Utility "
#define VL10N_ECO_LBL_USAGE__01_STR         " Usage echoid:\n"
#define VL10N_ECO_LBL_USAGE__02_STR         " [ -d Display detailed information about available/selected locking criteria ]\n"
#define VL10N_ECO_LBL_USAGE__03_STR         " [ -h Display usage ]\n"
 /* WT# 83140: Extra command line option for a pause before returning
    * to the command line
    * Date fixed: June 05 2007
    * Version: 8.0.7
    */
#define VL10N_ECO_LBL_USAGE__04_STR         " [ -p Pauses before returning to the console ]\n"

#define VL10N_ECO_ERR_MACID_STR            "ERROR: Failed to obtain any machine identification elements (%d).\n"
#define VL10N_ECO_LBL_IPADD_STR            "\tIP address                         : %s\n"
#define VL10N_ECO_LBL_IP6ADD_STR           "\tIPv6 address                       : %s\n"
#define VL10N_ECO_LBL_IDPROM_STR           "\tID PROM                            : 0x%lX\n"
#define VL10N_ECO_LBL_DISKID_STR           "\tDisk ID                            : 0x%lX\n"
#define VL10N_ECO_LBL_DISKID_STR_OLD       "\tDisk ID (Old Style)                : 0x%lX\n"
#define VL10N_ECO_LBL_HOSTNAME_STR         "\tHost name                          : %s\n"
#define VL10N_ECO_LBL_ETHERADD_STR         "\tEthernet address[%d]                : %s\n"
#define VL10N_ECO_LBL_IPXADD_STR           "\tNetWare IPX address                : 0x%lX\n"
#define VL10N_ECO_LBL_HDD_SRL_STR          "\tHard Disk Serial[%d]                : %s\n"
#define VL10N_ECO_LBL_NW_SNO_STR           "\tNetWare serial number              : 0x%lX\n"
#define VL10N_ECO_LBL_SLM_COMPID_STR       "\tSentinel computer ID[%d]            : %s\n"
#define VL10N_ECO_LBL_CUSTOM_HID_STR       "\tStandard custom                    : 0x%08lX\n"
#define VL10N_ECO_LBL_CPU_SNO_STR          "\tCPU Serial No                      : 0x%s\n"
#define VL10N_ECO_LBL_CUSTOMEX_STR         "\tExtended custom[%d]                 : 0x%s\n"
#define VL10N_ECO_LBL_CPU_INFO_STR         "\tCPU Info String                    : %s\n"
#define VL10N_ECO_LBL_UUID_STR             "\tUUID                               : %s\n"

#define VL10N_ECO_LBL_LOCKCODE_01_STR      "\n\n\t\tLocking Code 1     : %3lX-%s\n"
#define VL10N_ECO_LBL_LOCKCODE_02_STR      "\n\t\tLocking Code 2             : %3lX-%s\n"
#define VL10N_ECO_LBL_LOCKCODE_STR         "\n\t\tLocking Code 1 [%d] : %3lX-%s\n"

#define VL10N_ECO_LBL_LOCKCODE_01_STR_OLD  "\t\tLocking Code 1 (Old Style) : %3lX-%lX\n"
#define VL10N_ECO_LBL_LOCKCODE_02_STR_OLD  "\t\tLocking Code 2 (Old Style) : %3lX-%lX\n"
#define VL10N_ECO_LBL_LOCKCODE_STR_OLD     "\t\tLocking Code 1 [%d] (Old Style) : %3lX-%lX\n"

#define VL10N_ECO_LBL_INVALID_LOCK_SELECTOR_01_STR "\n\n\t\tLocking Code 1     : Invalid lock selector\n"
#define VL10N_ECO_LBL_INVALID_LOCK_SELECTOR_02_STR "\t\tLocking Code 2     : Invalid lock selector\n"

#define VL10N_ECO_TXT_WECHOID_HELP_ABOUT_STR       "Wechoid Version 9.7.0.0036"

/* End of echoid strings */

/*{rdctoenc STRINGS - Acronym [RTE] }*/

/* Do not translate */
#define VL10N_RTE_HDR_UTIL_STR              "\n  %s %s(%d) Readable License To Encrypted License Converter\n"


#define VL10N_RTE_ERR_REMAP_STR             "Remapped default fixed strings from file %s\n"
#define VL10N_RTE_PRM_LICSTR_STR            "Enter a readable license code (or your EOF character to quit):  \n"
#define VL10N_RTE_LBL_USAGE_STR             "Usage: %s  [In-file-name]\n"
#define VL10N_RTE_INF_USAGE_01_STR          "       If no In-file-name is specified, input is taken from standard input.\n"
#define VL10N_RTE_INF_USAGE_02_STR          "       Output license codes are always written to standard out (stdout).\n"
#define VL10N_RTE_INF_USAGE_03_STR          "       Lines in the input that are not recognized as readable strings are\n       written to the output unchanged. \n"
#define VL10N_RTE_INF_USAGE_04_STR          "       Use the environment variable %s to provide the location of a\n       license configuration file, if any. \n"

/* End of rdctoenc strings */


/*{lsusage and vusage STRINGS - Acronym [USG] }*/

#define VL10N_USG_HDR_UTIL_01_STR           "----------------------Log report for session number [%d]-----------------------\n"
#define VL10N_USG_HDR_LOGREP_STR            "**** LOG REPORT FOR Sessions: %s\n"
#define VL10N_USG_HDR_MISS_LOGSESSION_STR   "**** MISSING LOG SESSIONS: %s\n"
#define VL10N_USG_ERR_LINENO_STR            "On line number : %d  ERROR:   %s \n"
/* Example: On line number: 5 ERROR: Could not read value */

#define VL10N_USG_ERR_UNABLE_OPEN_STR       "Unable to open %s\n"
#define VL10N_USG_ERR_LOGFILE_LONG_STR      "Cannot proceed further.. Log File too long..\n"
#define VL10N_USG_ERR_GEN_INCREP_STR        "WARNING : Generating incomplete report...\n"
#define VL10N_USG_ERR_LINE_FILE_STR         "Error in Line number %d of file %s. Exiting\n"
/* Example: Error in Line number 3 of file filename.dat. Exiting */

#define VL10N_USG_HDR_APPUSAGE_SUMM_STR     "                      Application Usage Summary \n"
#define VL10N_USG_HDR_DATAHEADER_01_STR     " %%age      %%age    Total    %%age     %%age       Min.App.   Avg.App.  Max.App.   Total\n"
/* Special instructions: Strings VL10N_USG_HDR_APPUSAGE_SUMM_STR through VL10N_USG_HDR_DATAHEADER_03_STR
   are displayed one on top of the other.
                         "%%age Denied" is short for "percentage of license requests denied".
                         "%%age Issued"              "percentage of license requests issued.".
                         "%%age Queued Granted"      "percentage of queued license requests granted"
                         "%%age QRequest"            "percentage of license requests were queued"
                         "Min.App. duration(mins)"   "Minimum Application duration in minutes"
                         Similar for Avg.App and Max.App. (average and maximum, respectively  */
#define VL10N_USG_HDR_DATAHEADER_02_STR     " Denied   Issued   Tokens   Queued   QRequest   duration  duration  duration   capacity\n"
#define VL10N_USG_HDR_DATAHEADER_03_STR     "                   Issued   Granted              (mins)    (mins)    (mins)   issued\n"
#define VL10N_USG_ERR_CHKSUM_NOMATCH_STR             " Line is tampered for session number %d \n"
#define VL10N_USG_ERR_LINENUM_NOMATCH_STR            " Line is missing for session number %d \n"
#define VL10N_USG_ERR_UNKNOWN_PROB_STR               " Unknown problem with the log file for session number %d \n"
#define VL10N_USG_ERR_CHKSUM_NOMATCH_OR_LN_MSSNG_STR " Either line is missing or tampered for session number %d \n"
#define VL10N_USG_ERR_LINES_FROM_TO_MSSNG_STR        " Lines from %d to %d are missing for session number %d \n"
#define VL10N_USG_ERR_CHKSUM_FOOTER_STR              "\"Shut down message for the session %d is corrupted \" "
#define VL10N_USG_ERR_FOOTER_LOGRECS_MSSNG_STR       " Entries in shut down message for the session %d is missing \ni.e some log record are missing"
#define VL10N_USG_ERR_FOOTER_MISSING_STR             " Shut down message is missing for session ID %d  ."
#define VL10N_USG_ERR_BEGIN_MISSING_STR              " Start up message is missing"
#define VL10N_USG_ERR_BEGIN_TAMPERED_STR             " Start up message  for session %d is tampered"
#define VL10N_USG_ERR_INVALID_CMDLNOPT_STR           "\n Invalid command-line option specified.\n"
#define VL10N_USG_ERR_INVALID_CMDLNVAL_STR           "\n Invalid command-line parameter value specified.\n"
#define VL10N_USG_ERR_NO_REPRESOURCE_STR             "\n No reporting resources available.\n"
#define VL10N_USG_ERR_NO_MATCHREC_STR                " *** Matching records not found. *** \n\n"
#define VL10N_USG_ERR_OPTLTR_EXPECTED_STR            " : option letter expected"
#define VL10N_USG_ERR_ILLEAGAL_OPT_STR               " : illegal option -- "
#define VL10N_USG_ERR_OPT_NOARGS_STR                 " : option does not accept an argument -- "
#define VL10N_USG_ERR_OPT_ARGS_STR                   " : option requires an argument -- "
#define VL10N_USG_INF_USAGE_01_STR                   " Usage: %s -l Usage-log-file \n\
 [-d New-log-file | -c CSV-Format-New-log-file] \n\
 [-f Feature-Name1,Version:Feature-Name2,Version ...] \n\
 [-y Start-Year(YYYY) [-m Start-Month(MM) [-a Start-Day(DD)]]] \n\
 [-Y End-Year(YYYY)   [-M End-Month(MM)]  [-A End-Day(DD)]]]\n"
#define VL10N_USG_INF_USAGE_02_STR                   " Usage: %s -l Usage-log-file \n\
 [-c CSV-Format-New-log-file]\n\
 [-f Feature-Name1,Version:Feature-Name2,Version ...]\n\
 [-y Start-Year(YYYY) [-m Start-Month(MM) [-a Start-Day(DD)]]] \n\
 [-Y End-Year(YYYY)   [-M End-Month(MM)]  [-A End-Day(DD)]]]\n"

#define VL10N_USG_INF_FEATVERVENDOR_01_STR           " [For Feature Name: %s, Version: %s, Vendor Code: %5s]\n\n"
#define VL10N_USG_INF_FEATVERVENDOR_02_STR           " [For Feature Name: %s, Version: %s]\n\n"
#define VL10N_USG_INF_FEATVERVENDOR_03_STR           " [For Feature Name: %s, Version: %s, Capacity: %s, Vendor Code: %5s]\n\n"
#define VL10N_USG_INF_FEATVERVENDOR_04_STR           " [For Feature Name: %s, Version: %s, Capacity: %s ]\n\n"
#define VL10N_USG_INF_INFINITE_STR                   " Infinite"
/* End of lsusage and vusage strings */

/*{bounce and qbounce STRINGS - Acronym [BNC] }*/

#define VL10N_BNC_LBL_DEMO_NAME_01_STR      "Bounce"
/* Do not translate */
#define VL10N_BNC_LBL_APP_NAME_STR          "Sentinel RMS Development Kit %s  %s"


#define VL10N_BNC_ERR_TMCALL_FAIL_STR       "Sentinel RMS Development Kit time call failed"
#define VL10N_BNC_ERR_INIT_FAIL_STR         "Sentinel RMS Development Kit failed to initialize"
#define VL10N_BNC_ERR_TOO_CLKSTM_STR        "Too many clocks or timers!"
/* Do not translate */
#define VL10N_BNC_ERR_FAIL_OVER_STR         ":FailOver:"

/* Do not translate */
#define VL10N_BNC_LBL_DEMO_NAME_02_STR      "QBounce"
#define VL10N_BNC_INF_CLPUT_QUEUE_STR       "Client put in queue......."
#define VL10N_BNC_INF_AUTH_HANDLE_STR       "The authorization handle no longer remains valid. Terminating the app... !!"

/* End of bounce and qbounce strings */

/*{slmdemo - Acronym [SLD] }*/

#define VL10N_SLD_HDR_APPNAME_STR          "Sentinel RMS Development Kit Demo Program"
#define VL10N_SLD_HDR_APPTITLE_STR         "Sentinel RMS Development Kit Demo"
#define VL10N_SLD_ERR_INITIALIZATION_STR   "Initialization error!"
#define VL10N_SLD_INF_LOGSAVE_STR          "Saving log to "
#define VL10N_SLD_ERR_SETSERVER_FAIL_STR   "> Failed to set server name to [%s]!"
#define VL10N_SLD_ERR_SETOPTION_FAIL_STR   "> Failed to set option for any server!"
#define VL10N_SLD_ERR_FEATNAME_STR         "Feature name required!"
#define VL10N_SLD_ERR_MEMALLOC_STR         "Failed to allocate memory! License returned!"
#define VL10N_SLD_ERR_RELEASELIC_STR        "Failed to release license handle."
#define VL10N_SLD_ERR_RENEWLIC_STR          "Heartbeat stopped! Failed to renew licenses!"
#define VL10N_SLD_INF_INVOKECLEANUP_01_STR  "Invoking VLScleanup() to cleanup the library ..."
#define VL10N_SLD_INF_SETSERVER_01_STR      "> Server name [%s] set for connection!"
#define VL10N_SLD_INF_SETSERVER_02_STR      "> Server name set for connecting with any server!"
#define VL10N_SLD_LBL_NONE_STR              "NONE"
#define VL10N_SLD_INF_INVOKECLEANUP_02_STR    "Invoked VLScleanup().\n"
#define VL10N_SLD_INF_CLEANUP_STATUS_STR  "> The call returned a success status of 0x0.\n"
#define VL10N_SLD_INF_INVOKERELEASE_STR    "Invoked VLS_RELEASE().\n"
#define VL10N_SLD_INF_INVOKEAPI_STR       "Invoked %s()."
#define VL10N_SLD_INF_API_STATUS_STR      "> The call returned a success status of 0x%X."
#define VL10N_SLD_INF_API_ERRSTATUS_STR      "> The call returned an error status of 0x%X."
#define VL10N_SLD_LBL_NOLIMIT_USERS_STR      "No limit"

/* End of slmdemo strings */

/*{ipxecho - Acronym [IXE] }*/

#define VL10N_IXE_HDR_APPNAME_STR            "Sentinel RMS Development Kit %s IPX Address Utility"
#define VL10N_IXE_ERR_COMMUNICATION_STR      "The underlying network subsystem is not ready for network communication."
#define VL10N_IXE_ERR_WINSOCK_SUPPORT_01_STR    "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation."
#define VL10N_IXE_ERR_WINSOCK_SUPPORT_02_STR    "The Windows Sockets version specified by the application is not supported by this DLL."
#define VL10N_IXE_ERR_SOCKET_STR             "socket returned %d"
#define VL10N_IXE_ERR_BIND_STR            "bind returned %d"
#define VL10N_IXE_ERR_GETSOCKNAME_STR          "getsockname returned %d"
#define VL10N_IXE_LBL_IPXADDR_STR         "This computer's IPX network address is %s"
#define VL10N_IXE_ERR_CLOSESOCKET_STR         "closesocket returned %d"
#define VL10N_IXE_ERR_WSACLEANUP_STR         "WSACleanup returned %d"
/* End of ipxecho strings */

/*{WLSGRMGR STRINGS - Acronym [GMR] }*/
#define VL10N_GMR_ERR_EMPTY_STR               " cannot be empty"
#define VL10N_GMR_ERR_LENGTH_STR              " length cannot exceed %d characters"
/*#define VL10N_GMR_ERR_INSTRUCTION_STR         "Please define tokens as positive values" */
#define VL10N_GMR_LBL_PROPERTION_GROUP_STR    "Properties of group : "
#define VL10N_GMR_LBL_REMOVE_GROUP_STR        "Remove group : %s ?"
#define VL10N_GMR_ERR_MAX_GROUP_STR           "Group can't have more than %d members"
#define VL10N_GMR_LBL_PROPERTION_FEATURE_STR  "Properties of feature : "
#define VL10N_GMR_LBL_REMOVE_FEATURE_STR      "Remove feature : %s ?"
#define VL10N_GMR_ERR_MAX_FEATURE_STR         "Feature can't have more than %d groups"
#define VL10N_GMR_LBL_PROPERTION_MEMBER_STR   "Properties of member : "
#define VL10N_GMR_ERR_MIN_MEMBER_STR          "Group must have at least one member"
#define VL10N_GMR_LBL_REMOVEALL_STR           "Removing all groups will remove the feature %s also"
#define VL10N_GMR_LBL_REMOVELAST_STR          "Removing last group will remove the feature also"
#define VL10N_GMR_LBL_CONTINUE_STR            "Do you wish to continue ?"
#define VL10N_GMR_LBL_SAVE_STR                "Do you want to save changes ?"
#define VL10N_GMR_LBL_FEATURE_GROUP_STR       "Group of feature : "
#define VL10N_GMR_ERR_FEATUREVER_STR          "Feature name with same version"
#define VL10N_GMR_ERR_VALUE_EXISTS_STR        " already exists"
#define VL10N_GMR_ERR_FILETYPE_STR            "Not a valid file"
#define VL10N_GMR_ERR_MIN_GROUP_STR           "Feature must have at least one group"
#define VL10N_GMR_LBL_GRP_USER_STR            "User"
#define VL10N_GMR_LBL_GRP_INCLUDED_STR        "Included"
#define VL10N_GMR_LBL_GRP_EXCLUDED_STR        "Excluded"
#define VL10N_GMR_LBL_GRP_MACHINE_STR         "Machine"
#define VL10N_GMR_ERR_MEMBERNAME_STR          "Invalid member name"
/* End of wlsgrmgr */

/*{loadls STRINGS - Acronym [LLS] }*/
#define VL10N_LLS_HDR_APPNAME_STR         L"LoadLS"
#define VL10N_LLS_ERR_FILEASSOC_STR       "No assocation for file 'lservnt.exe'"
#define VL10N_LLS_ERR_FILEPATH_01_STR     "No path for 'lservnt.exe' not found"
#define VL10N_LLS_ERR_FILEPATH_02_STR     "File for 'lservnt.exe' not found"
#define VL10N_LLS_ERR_FILEFORMAT_STR      "'lservnt.exe' has an invalid format"
#define VL10N_LLS_ERR_REGISTRY_01_STR     "Unable to open the System Service Registry.  Error(%x)"
#define VL10N_LLS_ERR_REGISTRY_02_STR     "Unable to add the Sentinel RMS License Manager Service to the System Service Registry. Is the service already installed?  Error(%x)"
#define VL10N_LLS_ERR_REGISTRY_03_STR     "Could not create key in the registry for EventLog service"
#define VL10N_LLS_LBL_INSTALL_SYSSER_STR  "The %s Service has been installed."
/* Example: The Sentinel RMS Development Kit license management System Service has been installed." */

#define VL10N_LLS_ERR_START_SYSSER_STR    "The %s Service could not be started."
#define VL10N_LLS_ERR_HANDLE_SYSSER_STR   "Unable to obtain a handle for the %s system service. Is the service currently installed? (OpenService (0x%02x))"
#define VL10N_LLS_ERR_STOP_SYSSER_STR     "Unable to stop the %s system service."
#define VL10N_LLS_LBL_REMOVE_SYSSER_STR   "The %s Service has been removed."
#define VL10N_LLS_ERR_REGISTRY_04_STR     "Unable to remove the %s the System Service Registry. Is the service installed or running? (DeleteService (0x%02x))"
#define VL10N_LLS_HDR_SERVICENAME_STR     "Sentinel RMS Development Kit NetBEUI"
#define VL10N_LLS_HDR_COMPANYNAME_STR     "SafeNet"
/* End of loadls */

/*{makeserl STRINGS - Acronym [MKS] }*/

/* Do not translate */
#define VL10N_MKS_ERR_UNABLE_LOGFILE_STR    "Unable to create log file \n"
/* Do not translate */
#define VL10N_MKS_INF_USAGE_STR             "Usage: %s vendorID (evaluation only) | -d serial | -dev vendorID (developer option)#\n"
/* Do not translate */
#define VL10N_MKS_ERR_DECODE_SERNO_STR      "Couldn't decode serial number %s\n"
/* Do not translate */
#define VL10N_MKS_INF_VENDOR_ID_STR         "Vendor id is %lu\n"
/* Do not translate */
#define VL10N_MKS_ERR_MAKE_SERNO_STR        "Couldn't make serial number\n"
/* Do not translate */
#define VL10N_MKS_INF_VENID_INOUT_STR       "vendorID in = %lu, vendorID out = %lu\n"
/* Do not translate */
#define VL10N_MKS_INF_DLT_NO_STR            "DLT ID Number is : %s\n"

/* End of makeserl strings */

/*{loadls (lservnt) STRINGS - Acronym [SNT] }*/

#define VL10N_SNT_RMS_SERVICE_NAME_STR      "Sentinel RMS License Manager"
#define VL10N_SNT_INF_HELP_MSG_STR          "-H|-h          (help)\n" \
                                            "-X remove      (remove service)\n" \
                                            "-X install     (install service)\n" \
                                            "-X start       (start service)\n" \
                                            "-X stop        (stop service)\n" \
                                            "-X status      (get server status)\n" \
                                            "-X commline    (start service on command line)\n" \
                                            "\n" \
                                            "NOTE: If you use '-X commline', this must be first on the command line\n"
#define VL10N_SNT_ERR_SRV_STATUS_STR        "Couldn't get server status: %s"
#define VL10N_SNT_MSG_SRV_STATUS_01_STR     "The service is not running."
#define VL10N_SNT_MSG_SRV_STATUS_02_STR     "The service is starting."
#define VL10N_SNT_MSG_SRV_STATUS_03_STR     "The service is stopping."
#define VL10N_SNT_MSG_SRV_STATUS_04_STR     "The service is running."
#define VL10N_SNT_MSG_SRV_STATUS_05_STR     "The service continue is pending."
#define VL10N_SNT_MSG_SRV_STATUS_06_STR     "The service pause is pending."
#define VL10N_SNT_MSG_SRV_STATUS_07_STR     "The service is paused."
#define VL10N_SNT_MSG_SRV_STATUS_08_STR     "Unknown server status."
#define VL10N_SNT_LBL_SRV_STATUS_STR        "Server status: %s\n"
#define VL10N_SNT_ERR_OPEN_SRVCFAIL_STR     "OpenService failed - %s\n"
#define VL10N_SNT_ERR_OPEN_SCMGRFAIL_STR    "OpenSCManager failed - %s\n"
#define VL10N_SNT_INF_START_STR             "Starting %s.\n"
#define VL10N_SNT_INF_STARTED_STR           "%s Started.\n"
#define VL10N_SNT_INF_CLDNOT_START_STR      "Couldn't start %s: %s"
#define VL10N_SNT_INF_STOP_01_STR           "Stopping %s."
#define VL10N_SNT_INF_STOP_02_STR           "\n%s stopped.\n"
#define VL10N_SNT_INF_STOP_FAIL_STR         "\n%s failed to stop.\n"
#define VL10N_SNT_ERR_CTRLSRVC_FAIL_STR     "ControlService failed - %s\n"
#define VL10N_SNT_ERR_UNABLE_INSTALL_STR    "Unable to install %s - %s\n"
#define VL10N_SNT_MSG_INSTALLED_STR         "%s installed.\n"
#define VL10N_SNT_ERR_CREATESRVC_FAIL_STR   "CreateService failed - %s\n"
#define VL10N_SNT_ERR_REG_KEY_STR           "Could not create key in the registry for EventLog service"
#define VL10N_SNT_ERR_SET_REGVAL_STR        "Could not set value in the registry for EventLog service"
#define VL10N_SNT_INF_SRVC_INSTLD_STR       "The %s license management System Service has been installed."
#define VL10N_SNT_INF_SRVC_RMVD_STR         "%s removed.\n"
#define VL10N_SNT_ERR_DELSRVC_FAIL_STR      "DeleteService failed - %s\n"
#define VL10N_SNT_ERR_DOCMD_STR             "Can't give doCommand() a 0 length command word\n"
#define VL10N_SNT_ERR_UNKNW_CMD_STR         "Unknown command '%s'\n"
#define VL10N_SNT_ERR_INVALID_CMD_STR       "Invalid startup options for license server"
/*{End of lservnt strings }*/


/*{lcommute STRINGS - Acronym [CMT] } */
#define VL10N_CMT_PRM_CHK_REMOTE_STR                    " Commuter authorization for remote machine? [y/n] : "
#define VL10N_CMT_PRM_LCKCODE_FILE_STR                  " File path for lock code for remote machine       : "
#define VL10N_CMT_PRM_CHKIO_STR                         " Check-In or Check-Out [i/o]                      : "
#define VL10N_CMT_PRM_AUTH_EXP_DAYS_FINITE_STR          " Number of days until the authorization expires[1 - %ld] : "
#define VL10N_CMT_PRM_AUTH_EXP_DAYS_UNLIMITED_STR       " Number of days until the authorization expires[1 - %d]\n"
#define VL10N_CMT_PRM_CHK_REMOTE_DURATION_EXTENSION_STR " Extend the existing remote commuter token? [y/n] : "
#define VL10N_CMT_PRM_AUTH_EXP_DAYS_UNLIMITED_STR_01    " or <Enter> for expiry till license expiry date         : "
#define VL10N_CMT_PRM_AUTH_EXP_DAYS_UNLIMITED           " or 0 for expiry till license expiry date               : "

#define VL10N_CMT_PRM_AUTH_EXP_DAYS_UNLIMITED_STR_02    " Checkout a perpetual license? [y/n]              : "
#define VL10N_CMT_PRM_AUTH_EXP_DAYS_UNLIMITED_STR_02_1  " Checkout a repository license? [y/n]              : "
#define VL10N_CMT_PRM_SRV_NAME_STR                      " Please enter the server name                     : "
#define VL10N_CMT_PRM_FEATURE_NM_STR                    " Please enter the feature name                    : "
#define VL10N_CMT_PRM_VERSION_NUM_STR                   " Please enter the version number                  : "
/*for remote commuting - SourabhB*/
#define VL10N_CMT_PRM_COMM_LIC_FILE_STR    " Please enter the file name to save commuter authorization         : "
#define VL10N_CMT_INF_CMTLIC_CO_OK_STR     "\n Commuter license Check-Out completed successfully !!\n"
#define VL10N_CMT_INF_CMTLIC_SAVE_OK_STR   "\n Commuter authorization is stored in file %s !!\n"
#define VL10N_CMT_INF_CMTLIC_CI_OK_STR     "\n Commuter license Check-In completed successfully !!\n"
#define VL10N_CMT_ERR_CMTLIC_CHKOUT_NOT_POS "\n Commuter license Check-Out not successful !!\n"
#define VL10N_CMT_ERR_CMD_SW_MISSNG_STR    " One or more of the command switches is/are missing !!"
#define VL10N_CMT_ERR_CMD_USSAGE_STR "\n"\
" Usage :\n" \
" -o: ['n' for normal Check-Out, 'r' for remote Check-Out]\n" \
" -l: [file path for locking code]\n" \
" -c: ['i' for Check-In, 'o' for Check-Out]\n" \
" -s: [name of the server] \n" \
" -f: [name of feature] \n" \
" -v: [version of feature] \n" \
" -d: [number of days the license authorization will be checked out] \n" \
" -e: [number of days remote token is to be extended] \n" \
" -a: [file path to save commuter authorization] \n"

#define VL10N_CMT_ERR_COMM_CODE_NOT_EXIST  "Commuter code does not exist. Re-start the server. \
                                            If problem still persists, execute lsclean utility to clean commuter information  \
                                            for this feature on server system. \n"
#define VL10N_CMT_ERR_UNKNOWN_STR          "Error[%d] : Unknown error code, cannot print error message.\n"

#define VL10N_CMT_INF_TRY_HELP_STR                      "Try '%s -h' for more information.\n"
#define VL10N_CMT_ERR_INVALID_OPT_VAL_STR               "'%s': Invalid value to option '%s'."
#define VL10N_CMT_DUPLICATE_OPT                         " : Duplicate option '%s'"
#define VL10N_CMT_ERR_CHKIN_WITH_REMCHKOUT              "You cannot specify check-in mode with remote check-out option."
#define VL10N_CMT_ERR_CHKOUT_WITH_CHKIN                 "You cannot specify remote check-out option with check-in mode."
#define VL10N_CMT_ERR_COMM_MODE_OPT_MISSING_STR         "Commuter mode option (c) for specifying check-in/check-out is missing."
#define VL10N_CMT_ERR_FEAT_NAME_OPT_MISSING_STR         "Feature name option (f) is missing."
#define VL10N_CMT_ERR_CHK_OUT_TYP_OPT_MISSING_STR       "Check-out type option (o) for specifying local/remote is missing."
#define VL10N_CMT_ERR_LCK_CODE_FILE_OPT_MISSING_STR     "Locking code file option (l) is missing."
#define VL10N_CMT_ERR_ATH_CODE_FILE_OPT_MISSING_STR     "Authorization file option (a) is missing."
#define VL10N_CMT_ERR_SERVER_OPT_MISSING_STR            "Server option (s) for specifying server is missing."
#define VL10N_CMT_ERR_CHK_OUT_DAYS_OPT_MISSING_STR      "Check-out days option (d) is missing."
#define VL10N_CMT_ERR_CHK_OUT_DAYS_EXTENSION_OPT_MISSING_STR "Check-out days option (d|e) is missing."
#define VL10N_CMT_ERR_CHK_OUT_DAYS_RANGE_MSG            "Number of check-out days must be in the range from 1 to %s."
/*{End of lcommute strings }*/


/*{rcommute STRINGS - Acronym [RMT] } - SourabhB*/
#define VL10N_RMT_ERR_CMD_USAGE_STR    "\n Usage :\n" \
                                       " -l: [file path to save locking code]\n" \
                                       " -i: [file path to install commuter authorization]\n"
#define VL10N_RMT_PRM_SAVE_LCK_FILE_STR    " Save locking code to file : "
#define VL10N_RMT_INF_SAVE_LCK_FILE_OK_STR "\n Locking code saved to file %s !!\n"
#define VL10N_RMT_PRM_REM_COMM_AUTH_FILE_STR    " Get remote license authorization from file : "
#define VL10N_RMT_INF_REM_COMM_AUTH_INST_OK_STR "\n Commuter authorization installed successfully !!\n"

/*{End of rcommute strings }*/


/* Do not translate these strings. They are here till get replaced in source code */
#define VL10N_CMT_ERR_CIO_STR              " Please specify 'i' for Check-In and 'o' for Check-Out for the command -c: in the command line.\n"
#define VL10N_CMT_ERR_CONTACT_SRV_STR      " Cannot contact the server \"%s\"! \n"
#define VL10N_CMT_ERR_NOFIND_FTR_INF_STR   " Cannot find feature info for feature \"%s\" version \"%s\" in the server \"%s\"!\n"
#define VL10N_CMT_INF_NOT_COMTR_TYPE_STR   " Feature \"%s\" Version \"%s\" is not allowed to commute !! \n"
#define VL10N_CMT_ERR_MC_FNGRPRINT_STR     " Cannnot acquire the machine fingerprint !! \n"
#define VL10N_CMT_ERR_NOMORE_CMT_LIC_STR   " No more commuter licenses for the feature !! \n"
#define VL10N_CMT_ERR_LIC_CO_BY_CLIENT_STR "\r\n There already is a commuter license authorization for this feature checked out on this machine !! \n"
#define VL10N_CMT_ERR_LIC_NOT_CO_BY_CLIENT_STR " There is no commuter license for this feature checked out on this machine !! \n"
#define VL10N_CMT_ERR_DAYS_OUT_OF_RANGE_STR    " Number of days must be in the range from 1 to %d !! \n"
/* Upto this point only. Continue translation for strings now onwards */



#define VL10N_CMT_ERR_COMMUTER_LICENSE_CHECK_IN_OUT_ERROR_MSG\
            "Please specify 'i' for Check-In and 'o' for Check-Out for the command -c: in the command line.\n"
#define VL10N_CMT_ERR_COMMUTER_CANNOT_CONTACT_SERVER_MSG\
            "Cannot contact the server \"%s\"!\n"
#define VL10N_CMT_ERR_COMMUTER_CANNOT_FIND_FEATURE_INFO_MSG\
            "Cannot find feature info for feature \"%s\" version \"%s\" in the server \"%s\"!\n"
#define VL10N_CMT_ERR_COMMUTER_FEATURE_NOT_COMMUTER_TYPE_MSG\
            "Feature \"%s\" Version \"%s\" is not allowed to commute!\n"
#define VL10N_CMT_ERR_COMMUTER_CANNOT_GET_MACHINE_FINGERPRINT_MSG\
            "Cannnot acquire the machine fingerprint!\n"
#define VL10N_CMT_ERR_NO_MORE_COMMUTER_LICENSE_MSG\
            "No more commuter licenses for the feature!\n"
#define VL10N_CMT_ERR_COMMUTER_LICENSE_CHECKED_BY_CLIENT_MSG\
            "\r\nThere already is a commuter license authorization for this feature checked out on this machine!\n"
#define VL10N_CMT_ERR_COMMUTER_LICENSE_NOT_CHECKED_BY_CLIENT_MSG\
            "There is no commuter license for this feature checked out on this machine!\n"
#define VL10N_CMT_ERR_NUMBER_OF_DAY_OUT_OF_RANGE_MSG\
            "Number of days must be in the range from 1 to %d!\n"
#define VL10N_CMT_ERR_ISSU_COMM_CODE_DAT_REST\
            "Cannot issue commuter code due to date restrictions\n"
/*for remote commuting - SourabhB*/
#define VL10N_CMT_ERR_COMMUTER_LICENSE_IS_CHECKED_REMOTELY_MSG\
            "This commuter license is checked out remotely, so it cannot be checked-in!\n"
#define VL10N_CMT_ERR_OPN_LCK_CODE_FILE_STR   "Unable to open lock code file `%s'.\n"
#define VL10N_CMT_ERR_OPN_LCK_CODE_FILE_STR_1   "Unable to open lock code file `%s' : %s\n"
#define VL10N_CMT_ERR_OPN_LCK_CODE_FILE_STR_2   "Error while reading lock code file `%s' : Either file is empty or problem occurred while reading lock code.\n"

#define VL10N_CMT_ERR_OPN_COMM_CODE_FILE_STR  "Unable to open commuter code file `%s'.\n"
#define VL10N_CMT_ERR_OPN_COMM_CODE_FILE_STR_1  "Unable to open commuter code file `%s' : %s\n"

#define VL10N_CMT_ERR_OPN_EXP_COMM_LIC "\nLicense has expired on the server!\n"

/* End of lcommute strings */


/*{timefix STRINGS - Acronym [TFX} }*/
#define VL10N_TFX_ERR_OPN_LIC_FILE_STR        "Unable to open license file `%s'.\n"
#define VL10N_TFX_ERR_INIT_CGEN_LIB_STR       "Error initializing code generator library.\n"
#define VL10N_TFX_INF_LICSTR_TIME_STR         "Time from license code is <%ld> <%s>"
#define VL10N_TFX_ERR_GETTING_TIME_STR        "Error getting time from license file.\n"
#define VL10N_TFX_LBL_VIOLN_NUM_STR           "File: %s, Violation #  %d\n"
#define VL10N_TFX_LBL_MOD_STR                 "Mod= %s"
#define VL10N_TFX_LBL_ACC_STR                 "Acc= %s"
#define VL10N_TFX_LBL_CHG_STR                 "Chg= %s"
#define VL10N_TFX_LBL_NOW_STR                 "Now= %s"
#define VL10N_TFX_LBL_USAGE_STR               "Usage: %s license-file\n"
#define VL10N_TFX_HDR_UTIL_STR                "\n         %s %s(%s) Time Tamper Reset Utility\n"
#define VL10N_TFX_ERR_TMFXLIC_EXHAUST_STR     "Timefix license execution is exhausted .\n"
#define VL10N_TFX_ERR_RUN_SPECIFIC_DT_STR     "Timefix must be run on the day specified in the license code.\n"
#define VL10N_TFX_ERR_LIC_EXPIRED_STR         "Timefix license is expired.\n"
#define VL10N_TFX_ERR_NOT_TMFX_LIC_TYPE_STR   "Not a timefix license code .\n"
#define VL10N_TFX_ERR_VENDORID_MISMATCH_STR   "licensed by a different vendor.\n"
/*{End of timefix strings} */

/* { Sample Program STRINGS - Acronym[SMP] } */

/* -- Start dots1.c strings */

#define VL10N_SMP_LBL_ADD_BAR_01_STR          "\015  Status: Cycle number   %3d:           "
#define VL10N_SMP_LBL_ADD_BAR_02_STR          "\015  Status: Cycle number   %3d: .....     "
#define VL10N_SMP_LBL_ADD_BAR_03_STR          "\015  Status: Cycle number   %3d: .........."
#define VL10N_SMP_INF_KEY_RELEASE_STR         "\nReceived an interrupt.  Releasing the license key...\n"
#define VL10N_SMP_HDR_DEMO_PROG_STR           "\n       %s %s Demonstration Program\n"
/*#define VL10N_SMP_LBL_CTRLC_TO_STOP_STR       "\n          [ Control-C to stop ]\n\n"*/
#ifdef _SGI_
    #define VL10N_SMP_LBL_CTRLC_TO_STOP_STR       "\n          [ DEL to stop ]\n\n"
#else
    #define VL10N_SMP_LBL_CTRLC_TO_STOP_STR       "\n          [ Control-C to stop ]\n\n"
#endif

#define VL10N_SMP_ERR_INIT_LICMGR_LIB_STR     "Unable to initialize license manager library.\n"
#define VL10N_SMP_LBL_UNDERLINE_01_STR           "---------------------------------------------\n"
#define VL10N_SMP_LBL_UNDERLINE_02_STR        "-------------------------------\n"
#define VL10N_SMP_ERR_RENEW_KEY_01_STR           "Unable to renew the license key."
#define VL10N_SMP_ERR_INSTALL_LIC_STORAGE_STR "Unable to set the install license storage.\n"
#define VL10N_SMP_ERR_SETTING_PRST_PATH_STR   "Fail to set the persistence path location.\n"
#define VL10N_SMP_ERR_SETTING_BACKUP_STR      "Fail to set the backup status for the specified store.\n"
#define VL10N_SMP_ERR_ADD_LICENSE_STR         "Unable to add license to current install storage.\n"
#define VL10N_SMP_ERR_ADDING_LICENSE_STORAGE_STR   "Fail to add the specified license storage.\n"

/* -- End dots1.c strings */

/* -- Start hookdemo.c strings */

#define VL10N_SMP_PRM_SET_ENV_VAR_STR         "\n\n Please set the env. var. LSTST_LOCNAME \n"
#define VL10N_SMP_INF_TEST_SRV_NAME_STR       "\n\n The SERVER NAME for testing : %s \n"
#define VL10N_SMP_INF_SRV_NAME_STR            "\n Server Name : %s "

#define VL10N_SMP_INF_KEY_SUCCESS_01_STR      "\n SUCCESS : Got key for Hook1 feature HOOKDEMO ver 1.0... \n"
#define VL10N_SMP_INF_KEY_SUCCESS_02_STR      "\n SUCCESS : Got the key for Hook2 feature HOOKDEMO ver 1.0... \n"
#define VL10N_SMP_INF_KEY_SUCCESS_03_STR      "\n SUCCESS : Got key for InvalidHook feature  ver 1.0... \n"
#define VL10N_SMP_INF_KEY_SUCCESS_04_STR      "\n SUCCESS : Unable to get key with invalid info \n"

#define VL10N_SMP_INF_KEY_FAILURE_01_STR      " Unable to get key for Hook1 feature HOOKDEMO ver 1.0 \n"
#define VL10N_SMP_INF_KEY_FAILURE_02_STR      " Unable to get key for Hook2 feature HOOKDEMO ver 1.0 \n"
#define VL10N_SMP_INF_KEY_FAILURE_03_STR      " Unable to get key for InvalidHook feature HOOKDEMO ver 1.0 \n"
#define VL10N_SMP_ERR_KEY_FAILURE_STR         "\n Failed : got key with invalid info \n"

#define VL10N_SMP_INF_MSG_MATCHED_01_STR     " Hook1 : The message sent by you matched, hence granted a key."
#define VL10N_SMP_INF_MSG_MATCHED_02_STR     " Hook1 : Good Boy! You sent me a message and it matched."
#define VL10N_SMP_INF_MSG_MATCHED_03_STR     " Hook2 : The message sent by you matched, hence granted a key."
#define VL10N_SMP_INF_MSG_MATCHED_04_STR     " Hook2 : Good Boy! You sent me a message and it matched."

#define VL10N_SMP_INF_REL_SUCCESS_01_STR     "\n SUCCESS : Rel. key for Hook1 feature HOOKDEMO ver 1.0.... \n"
#define VL10N_SMP_INF_REL_SUCCESS_02_STR     "\n SUCCESS : Rel. key for Hook2 feature HOOKDEMO ver 1.0.... \n"
#define VL10N_SMP_INF_REL_SUCCESS_03_STR     "\n SUCCESS : Rel. key for feature HOOKDEMO ver 1.0.... \n"

#define VL10N_SMP_INF_REL_FAILURE_01_STR     "\n Unable to release key for Hook1 feature HOOKDEMO ver 1.0  \n"
#define VL10N_SMP_INF_REL_FAILURE_02_STR     "\n Unable to release key for Hook2 feature HOOKDEMO ver 1.0  \n"
#define VL10N_SMP_INF_REL_FAILURE_03_STR     "\n Unable to release key for feature HOOKDEMO ver 1.0 \n"

#define VL10N_SMP_ERR_MSG_NOMATCH_STR        " Hook2 : The message sent by you did not matched, hence no key."
#define VL10N_SMP_ERR_RENEW_KEY_02_STR       "\n Unable to renew the license key. \n"

/* --  End hookdemo.c strings -- */

/* -- Start stars1.c strings -- */

#define VL10N_SMP_ERR_OBTAIN_KEY_01_STR       " Unable to obtain a license key.\n"
#define VL10N_SMP_ERR_OBTAIN_KEY_02_STR       " Unable to obtain a license key."
#define VL10N_SMP_INF_ERROR_CODE_STR          " Errorcode is %s"
#define VL10N_SMP_INF_TOTAL_LIC_STR           " The total number of licenses are %d"
#define VL10N_SMP_INF_LIC_EXPIRED_STR         " This application is no longer licensed.\n"
#define VL10N_SMP_INF_LIC_EXP_ON_STR          " The license expired on %-25.24s\n"
#define VL10N_SMP_INF_RUNNING_STR             " Running "
#define VL10N_SMP_INF_LIC_UNAVAILABLE_STR     " License not available at this time"
#define VL10N_SMP_INF_NO_APPNAME_STR          " The application does not have a name"
#define VL10N_SMP_INF_SENTLM_NOEXIST_STR      " The Sentinel RMS Development Kit machine does not seem to exist"
#define VL10N_SMP_ERR_MEM_ALLOC_STR           " Memory Allocation Problem, quitting."

#define VL10N_SMP_PRM_LM_HOSTNAME_STR         " Enter Sentinel RMS License Manager Hostname:"
#define VL10N_SMP_ERR_INVALID_INPUT_STR       " Invalid Input, Try again."
#define VL10N_SMP_INF_TRY_LATER_STR           " Too many tries, try later."
#define VL10N_SMP_PRM_START_SENTLM_STR        " Start the Sentinel RMS License Manager and then type resume(r)/quit(q) "
#define VL10N_SMP_INF_TRYING_SENTLM_STR       " Trying for Sentinel RMS License Manager Now..."
#define VL10N_SMP_ERR_INVALID_VAL_STR         " Invalid Value, Quitting now...."
#define VL10N_SMP_INF_SRV_NO_RESPONSE_STR     " The Sentinel RMS License Manager does not respond.  "
#define VL10N_SMP_INF_LIC_TERM_STR            " License Terminated. Needs more frequent renewal"
#define VL10N_SMP_PRM_ALL_LIC_INUSE_01_STR    " All licenses in use. Do you want to wait? (y/n) "
#define VL10N_SMP_PRM_ALL_LIC_INUSE_02_STR    " All licenses still in use. Do you want to wait? (y/n) "
#define VL10N_SMP_INF_RENEW_WAITING_STR       " Waiting. Will try to renew license shortly"
#define VL10N_SMP_INF_QUIT_TEMP_STR           " OK. Will quit momentarily."
#define VL10N_SMP_INF_ALL_LIC_BUSY_STR        " All Licenses are in use "
#define VL10N_SMP_INF_ALIEN_APPNAME_STR       " The Server does not recognize this application name"
#define VL10N_SMP_INF_NODELOCKED_APP_STR      " This is a Nodelocked application"
#define VL10N_SMP_ERR_INTERNAL_ERR_STR        " Found an Internal Error "

/* bounce and dots1 VLSFeatureInfoExt() license version failure error message */
#define VL10N_SMP_ERR_LICENSE_VERSION_OLD_01_STR  "Old license found. Application can not run."

#ifndef _SFNT_CISCO_IOS_

/* {  prsclean STRINGS - Acronym [PRC]  } */
#define VL10N_PRC_ERR_NO_TRIAL_INFO_STR           "%d.   No Trial Info with this Feature:\"%s\"  & Version:\"%s\" In-Use. \n"
#define VL10N_PRC_ERR_NO_TRIAL_INFO_PATH_STR      " No Trial Info found in path %s\n"
#define VL10N_PRC_ERR_NOT_VALID_LIC_STR           "%d.   Feature:\"%s\" Not a valid license  for this Utility. \n"
#define VL10N_PRC_INF_LIC_EXPIRED_STR             "%d.   License is Exipred  \n"
#define VL10N_PRC_ERR_OPN_LIC_FILE_STR            "Unable to open license file `%s'.\n"
#define VL10N_PRC_ERR_INIT_CGEN_LIB_STR           "Error initializing code generator library.\n"
#define VL10N_PRC_INF_TRIAL_INF_CLEANED_STR       "%d. Cleaned Trial Info for Feature \"%s\" version \"%s\" from the system. \n"
#define VL10N_PRC_INF_DEL_PERSISTANCE_FILES_STR   " Deleting persistance Files ......................\n"
#define VL10N_PRC_INF_DEL_REG_ENTRIES_STR         " Deleting Registries Entries......................\n"

#define VL10N_PRC_HDR_HLINE_STR                   "|---------------------------------------------------------------------------|\n"
#define VL10N_PRC_HDR_REGISTRY_DEL_OK_STR         "|                 Successfully Deleted registry entries                     |\n"
#define VL10N_PRC_HDR_UTIL_STR                    "\n  %s %s Persistance Clean Utility\n"
#define VL10N_PRC_LBL_USAGE_STR                   "Usage: %s license-file\n"
#define VL10N_PRC_PRM_ENTER_SRV_PATH_STR          "\n Please enter the Sentinel RMS License Manager path  : "
#define VL10N_PRC_PRM_REENTER_SRV_PATH_STR        "\n \"%s :%s\": Please reenter : "
#define VL10N_PRC_INF_REMOVING_TRIAL_INFO_STR     "\n Removing Trial Info Sentinel RMS License Manager ...........\n\n\n"
#define VL10N_PRC_INF_REMOVED_TR_LIC_OK_STR       "|          Successfully Removed Trial License Info from the System.         |\n"
#define VL10N_PRC_ERR_REMOVE_TR_LIC_STR           "|          Not Able To Remove Trial Info                                    |\n"

/* { End prsclean STRINGS } */

/* { lsver STRINGS - Acronym [LSV] } */

#define VL10N_LSV_HDR_ABOUT_SENTLM_STR           "                  About Sentinel RMS Development Kit"
#define VL10N_LSV_HDR_ABOUT_UNDERLINE_STR        "                  --------------------------------"
#define VL10N_LSV_HDR_UTIL_STR                    "Multi-platform licensing toolkit for software developers"
#define VL10N_LSV_HDR_COPYRIGHT_STR              "    Copyright (C) 2011 SafeNet, Inc."
#define VL10N_LSV_HDR_RIGHTS_STR                 "         All Rights Reserved."
#define VL10N_LSV_LBL_VERSION_STR                "Version      : 8.4"
#define VL10N_LSV_LBL_REVISION_STR               "Revision     : 0"
#define VL10N_LSV_LBL_RELEASE_YR_STR             "Released     : 2011"


#define VL10N_LSV_HDR_UX_ABOUT_SENTLM_STR          "                  About Sentinel RMS Development Kit"
#define VL10N_LSV_HDR_UX_ABOUT_UNDERLINE_STR       "                  --------------------------------"
#define VL10N_LSV_HDR_UX_UTIL_STR                   "Multi-platform Licensing Toolkit for Software Developers"
#define VL10N_LSV_HDR_UX_COPYRIGHT_STR             "          Copyright (C) 2011 SafeNet, Inc."
#define VL10N_LSV_HDR_UX_RIGHTS_STR                "               All Rights Reserved."
#define VL10N_LSV_LBL_UX_VERSION_STR               "Version      : 8.4"
#define VL10N_LSV_LBL_UX_REVISION_STR              "Revision     : 0"
#define VL10N_LSV_LBL_UX_RELEASE_YR_STR            "Released     : 2011"

/* { End of lsver Strings } */

/* { lsmail STRINGS - Acronym [LSA] } */

#define VL10N_LSA_HDR_SENTLM_MAIL_STR           " SendMail Utility "
#define VL10N_LSA_LBL_SYNTAX_STR                "LSMail Syntax is:"
#define VL10N_LSA_LBL_SYNTAX_01_STR             "LSMail -t <Recipient Name>"
#define VL10N_LSA_LBL_SYNTAX_02_STR             "       -m <Message Text>"
#define VL10N_LSA_LBL_SYNTAX_03_STR             "       -s <Subject>"
#define VL10N_LSA_LBL_SYNTAX_04_STR             "       -p <port number>"
#define VL10N_LSA_LBL_SYNTAX_05_STR             "       -n <Sender Name>"
#define VL10N_LSA_LBL_SRVNAME_STR               "\nServer Name: "
#define VL10N_LSA_LBL_PORTNUM_STR               "\nPort Number: "
#define VL10N_LSA_LBL_LOGFILE_STR               "\nLog File   : "
#define VL10N_LSA_LBL_LOGGING_01_STR            "\nLogging    : ON"
#define VL10N_LSA_LBL_LOGGING_02_STR            "\nLogging    : OFF"
#define VL10N_LSA_LBL_SENDERNAME_STR            "\nSender Name: "
#define VL10N_LSA_INF_SRV_STR                   "\nEnter SMTP Server Name or IP Address: "
#define VL10N_LSA_ERR_SRV_STR                   "\nInvalid SMTP Server Name"
#define VL10N_LSA_ERR_SEND_STR                  "\nInvalid Sender Name"
#define VL10N_LSA_INF_SENDERNAME_STR            "\nEnter Sender Name: "
/* { End of lsmail Strings } */

/* strings modified */
/*
#define VL10N_GMR_ERR_INSTRUCTION_STR         "Please define tokens as positive values between 0 and 65535"
*/
#define VL10N_GMR_ERR_EXT_INSTRUCTION_STR     "Please define tokens as positive values between 0 and %u"
#define VL10N_GMR_LBL_REMOVEALL_01_STR        "Removing all features will remove all the groups also"
/*New Strings found on 04/05/2000. These may not be localized in other languages - dk*/
#define VL10N_LCF_HDR_SRVNAME_STR           "\nServer Name%35s "
#define VL10N_LCF_HDR_ADDRESS_STR           "IP Address"
/*the following two strings do not need localization. extracted for the sake of alingment*/
#define VL10N_LCF_HDR_UNDERLINE01_STR           "\n-----------%35s "
#define VL10N_LCF_HDR_UNDERLINE02_STR           "      ----------"
/* end of strings modified */
#endif /* !_SFNT_CISCO_IOS_ */

#define VL10N_CLL_ERR_LINK_LIB_STR  "Application is not linked with integrated  library.\n"
#define VL10N_CLL_ERR_NO_TRIAL_INFO_STR "Can not find trial license information for given feature.\n"
#define VL10N_CLL_ERR_TRIAL_INFO_FAILED_STR "Failure in retrieving the trial usage information for the given feature.\n"


/* Codegen Library strings */
#define VL10N_CGN_ERR_CODEP_NULL_STR        "codeP cannot be NULL."
#define VL10N_CGN_LBL_VERSION_STR           "Codegen Version"
#define VL10N_CGN_LBL_COMB_LIC_STR          "Combining Licenses"

#define VL10N_CGN_LBL_CGENVER_LESS_STR      "For Codegen Version <6"
#define VL10N_CGN_LBL_KEY_TYPE_STR          "Key Type"
#define VL10N_CGN_LBL_NO_OF_FEAT_STR        "No Of Features"
#define VL10N_CGN_LBL_RED_LIC_STR           "Redundant License"
#define VL10N_CGN_LBL_MAJRULE_FLAG_STR      "Majority Rule Flag"
#define VL10N_CGN_LBL_ENCR_LEVEL_STR        "Encryption Level"
#define VL10N_CGN_LBL_ELAN_FLAG_STR         "Elan Key Flag"
#define VL10N_CGN_LBL_COMM_LIC_STR          "Commuter Licenses"
#define VL10N_CGN_LBL_COMM_DAYS_STR         "Commuter Max Checkout Days"
#define VL10N_CGN_LBL_LOCAL_REQUEST_LOCK_FLAG        "Local Request Locking Criteria Flag"
#define VL10N_CGN_LBL_LOCAL_REQUEST_LOCK_CRIT        "Local Request Criteria"
#define VL10N_CGN_LBL_GRACE_PERIOD_DAYS_STR "Grace Period Calendar Days"
#define VL10N_CGN_LBL_GRACE_PERIOD_HRS_STR  "Grace Period Elapsed Hours"
#define VL10N_CGN_LBL_GRACE_PERIOD_FLG_STR  "Grace Period Flag"
#define VL10N_CGN_LBL_OVERDRAFT_FLG_STR     "Overdraft Flag"
#define VL10N_CGN_LBL_OVERDRAFT_HRS_STR     "Overdraft Hours"
#define VL10N_CGN_LBL_OVERDRAFT_USERS_STR   "Overdraft Users"
#define VL10N_CGN_LBL_CONV_TIME_STR         "Conversion Time"
#define VL10N_CGN_LBL_LICTYPE_STR           "License Type"
#define VL10N_CGN_LBL_HELDLIC_TYPES_STR     "Held License Types"
#define VL10N_CGN_LBL_CLTSRV_LOCK_STR       "Client-Server Locking Mode"
#define VL10N_CGN_LBL_CRIT_SHLIC_STR        "Criterion For Shared Licenses"
#define VL10N_CGN_LBL_SRV_NO_STR            "Server number"
#define VL10N_CGN_LBL_SRVLOCK_SEL_STR       "Server Lock Selector"
#define VL10N_CGN_LBL_SRVLOCK_CODE_STR      "Server Lock Code"
#define VL10N_CGN_LBL_CLT_NO_STR            "Client number"
#define VL10N_CGN_LBL_CLTLOCK_SEL_STR       "Client Lock Selector"
#define VL10N_CGN_LBL_CLTLIB_BEH_STR        "Client Library Behavior"
#define VL10N_CGN_ERR_VENDINFO_NONPRT_STR   " Vendor Info contains non printable char"
#define VL10N_CGN_LBL_VENDINFO_NONASC_STR   " Vendor Info contains non ASCII char"
#define VL10N_CGN_ERR_VENDINFO_HASH_STR     " Vendor Info contains one or more '#' char"
#define VL10N_CGN_ERR_LIC_VENDINFO_NONPRT_STR   "License Vendor Information contains non printable character"
#define VL10N_CGN_LBL_LIC_VENDINFO_NONASC_STR   "License Vendor Information contains non ASCII character"
#define VL10N_CGN_ERR_LIC_VENDINFO_HASH_STR     "License Vendor Information contains one or more '#' characters"
#define VL10N_CGN_LBL_PVTVEND_INFO_STR      "Private Vendor Information"
#define VL10N_CGN_LBL_PUBVEND_INFO_STR      "Public Vendor Information"
#define VL10N_CGN_LBL_VENINFO_STR           "Vendor Information"
#define VL10N_CGN_LBL_VENINFO_REDUNDANT_STR           "Vendor Information (Redundant Servers)"
#define VL10N_CGN_LBL_LIC_VENDINFO_STR      "License Vendor Information"
#define VL10N_CGN_LBL_PUB_PLUS_PRI_VENINFO_STR  "(Public + Private Vendor Info)"
#define VL10N_CGN_LBL_FEATNAME_NONPRT_STR   "Feature Name contains non printable char"
#define VL10N_CGN_LBL_FEATNAME_NONASC_STR   "Feature Name contains non ASCII char"
#define VL10N_CGN_LBL_FEAT_NAME_STR         "Feature Name"
#define VL10N_CGN_LBL_FEATVER_NONPRT_STR    "Feature Version contains non printable char"
#define VL10N_CGN_LBL_FEATVER_NONASC_STR    "Feature Version contains non ASCII char"
#define VL10N_CGN_LBL_FEATVER_STR           "Feature Version"
#define VL10N_CGN_LBL_SECTTXT_STR           "Secret text"
#define VL10N_CGN_LBL_LEN_SECRETS_STR       "Length Of Secrets"
#define VL10N_CGN_LBL_SECTTXT_NONPRT_STR    "Secret text contains non printable char"
#define VL10N_CGN_LBL_SECTTXT_NONASC_STR    "Secret text contains non ASCII char"
#define VL10N_CGN_LBL_NO_SECRETS_STR        "Number Of Secrets"
#define VL10N_CGN_LBL_CLLOCK_INFO_STR       "Client Lock Info"
#define VL10N_CGN_LBL_MAXKEYS_PERHOST_STR   "Max Keys Per Host"
#define VL10N_CGN_LBL_NO_SUBNETS_STR        "Number of subnets"
#define VL10N_CGN_LBL_SITELIC_INFO_STR      "Site Lic Info"
#define VL10N_CGN_LBL_NO_SRV_STR            "Number Of Servers"
#define VL10N_CGN_LBL_NO_CLIENTS_STR        "Number Of Clients"
#define VL10N_CGN_LBL_FEAT_NO_STR           "Feature Number"
#define VL10N_CGN_LBL_FEATNO_SINGLEKEY_STR  "Feature No For Single Key"
#define VL10N_CGN_LBL_HARD_LMT_STR          "Hard Limit"
#define VL10N_CGN_LBL_SOFT_LMT_STR          "Soft Limit"
#define VL10N_CGN_LBL_SHARE_LMT_STR         "Sharing Limit"
#define VL10N_CGN_LBL_GROUP_LMT_STR         "Group Limit"
#define VL10N_CGN_LBL_KEY_LIFETM_UNITS_STR  "Key Lifetime Units"
#define VL10N_CGN_LBL_KEY_CAPACITY_UNITS_STR  "Capacity Units"
#define VL10N_CGN_LBL_HLDTM_UNITS_STR       "HoldTime Units"
#define VL10N_CGN_LBL_KEY_LIFETM_STR        "Key Life Time"
#define VL10N_CGN_LBL_CAPACITY_STR          "Capacity"
#define VL10N_CGN_LBL_LIFE_TM_STR           "Life time"
#define VL10N_CGN_LBL_KEY_HLDTM_STR         "Key Hold Time"
#define VL10N_CGN_LBL_LIC_STMON_STR         "License Start Month"
#define VL10N_CGN_LBL_LIC_STDAY_STR         "License Start Day"
#define VL10N_CGN_LBL_LIC_STYR_STR          "License Start Year"
#define VL10N_CGN_LBL_LIC_ENDMON_STR        "License End Month"
#define VL10N_CGN_LBL_LIC_ENDDAY_STR        "License End Day"
#define VL10N_CGN_LBL_LIC_EXPYR_STR         "License Expiration Year"
#define VL10N_CGN_LBL_CLK_TAMPER_STR        "Clock Tampering"
#define VL10N_CGN_LBL_OUTPUT_LICFILE_STR    "Output License Type"
#define VL10N_CGN_LBL_LIC_TYPE_INFO_STR     "License type info"
#define VL10N_CGN_LBL_TRIAL_DYCNT_STR       "Trial Days Count"
#define VL10N_CGN_LBL_TRIAL_HOURS_STR       "Trial Elapsed Hours"
#define VL10N_CGN_LBL_TRIAL_EXECNT_STR      "Trial Execution Count"
#define VL10N_CGN_LBL_LIC_EXPQUART_STR      "License Expiration Quarter "
#define VL10N_CGN_LBL_TRIAL_ATTRIBUTES      "Trial Licensing Attributes"
#define VL10N_CGN_LBL_HOSTNAME_NONPRT_STR   "Host Name contains non printable char"
#define VL10N_CGN_LBL_LIC_STHRS_STR         "License Start Hours"
#define VL10N_CGN_LBL_LIC_STMINS_STR        "License Start Minutes"
#define VL10N_CGN_LBL_LIC_ENDHRS_STR        "License End Hours"
#define VL10N_CGN_LBL_LIC_ENDMINS_STR       "License End Minutes"
#define VL10N_CGN_LBL_LIC_ENTITLEMENTID_STR    "Entitlement Id"
#define VL10N_CGN_LBL_LIC_PRODUCTID_STR        "Product Id"
#define VL10N_CGN_LBL_LIC_FEATUREID_STR        "Feature Id"
#define VL10N_CGN_LBL_LIC_AUTOHRIZATIONID_STR  "Authorization Id"
#define VL10N_CGN_LBL_LIC_CLOUD_USAGE_FLAG_STR         "Cloud Usage Flag"
#define VL10N_CGN_LBL_LIC_SOURCE_STR                   "License Source Flag"
#define VL10N_CGN_LBL_LIC_ACTIVATIONBIRTH_STR          "Activation Start Time"
#define VL10N_CGN_LBL_LIC_ACTIVATIONEXPIRY_STR         "Activation Expiry Time"
#define VL10N_CGN_LBL_LIC_GENERATIONTIME_STR           "License Generation Time"
#define VL10N_CGN_ERR_LIC_GENERATIONTIME_NULL_STR      "License Generation Time cannot be NULL."
#define VL10N_CGN_ERR_LIC_GENERATIONTIME_CANNOT_BE_SET "License Generation Time cannot be set, it is for internal use only."




/* CODEGEN LIB. STRINGS */

/* SENTINELLM SHELL: DO NOT ALTER THIS LINE. Make only shell related changes below this line.
 * All the rest of the changes should be made above this section.
 * Shell does not display any string that require formatting. So these strings
 * have been defined seperately instead of using similar looking ones above.
 *{Shell STRINGS (Win32 only) - Acronym [SHL] }
 */

#ifndef _SFNT_CISCO_IOS_
#define VL10N_SHL_ERR_CUSTOM_01_STR            "The application will be terminated after %d minute(s). Save your work!"
#define VL10N_SHL_ERR_CUSTOM_02_STR            "No license was found for this application.  Would you like to invoke the Sentinel RMS Development Kit Client Activator in order to obtain a license?"
#define VL10N_SHL_ERR_CUSTOM_03_STR            "There is no license available to activate your application. You have automatically been placed in a queue for license."
#define VL10N_SHL_INF_INPUT_FILE_STR           "\n  Input file: %s"
#define VL10N_SHL_INF_OUTPUT_FILE_STR          "\n  Output file: %s"
#define VL10N_SHL_INF_CMD_LINE_VER_STR         "%s version "
#define VL10N_SHL_ERR_CMD_LINE_ERR_PREFIX_STR  "ERROR(%04d) -- "
#define VL10N_SHL_ERR_FEAT_NAME_PREFIX_STR     "Feature:%.24s"
#define VL10N_SHL_ERR_FEAT_VERSION_PREFIX_STR  "Version:%.11s"
#define VL10N_SHL_ERR_PROD_LICENSE_HDR_STR     "\nProduct license: "
#define VL10N_SHL_ERR_GROUP_LICENSE_HDR_STR    "\nGroup license: "
#define VL10N_SHL_HDR_VERSION_INFO_STR         "Version Info..."
#define VL10N_SHL_ERR_ERROR_MSG_PREFIX_STR     "ErrorCode = %i"
#define VL10N_SHL_ERR_LOW_ON_RESOURCES_STR     "Running low in system resources!\nPress OK to exit."
#define VL10N_SHL_HDR_ACTIVATOR_PROMPT_TITLE_STR "Activate License"
#define VL10N_SHL_INF_COMMAND_USAGE_STR       "Usage: %s [options]  [path\\]progname  [[path\\][newname]] {return}\n\
     options:\n\
       /H or /? Displays this help screen.\n\
       /FLname  Licensed feature name (up to 24 characters).\n\
       /CFname  Configuration File name. Supported with Standalone only.\n\
       /VLname  Licensed version (up to 11 characters).\n\
       /O       Overwrites newname if it already exists.\n\
       /Mname   Specifies the message file (default=RMSSHELL.MSG).\n\
       /In      Renews license every 'n' seconds (default=60 seconds).\n\
       /Q       Enables license queuing feature.\n\
       /Y       Allow Terminal server usage.\n\
                (Only in standalone and integrated mode) \n\
       /Tn      Number of tokens (default=1).\n\
       /Pn      Server UDP port (1-65535; default=5093).\n\
       /DLLname Name of the DLL for CustomHostID function.\n\
       /FNname  Function name for CustomHostID\n\
       /KLvers  Option to set value of vers=7000,7300,7301,8000,8100,8230,8400,8500,8600,9000,9100 and 9200.\n\
       /CLvalue Set value=1,2,3,4 and 5 for selecting Protection level.\n\
       /CM      Customize to Window MessageBox to diaplay error string.\n"

#define VL10N_SHL_INF_COMMAND_USAGE_STR1  "       /CC      Customize to Console display of error string.\n\
       /CN      Customize to No display of error string.\n\
       /JD      Allow application to run in presence of debuggers.\n\
       /XI      Disable Hide Imports option.\n\
       /XA      Disable AES Encryption for Additional Data Files.\n\
       /XV      Disable virtualization checks.\n\
       /US      Use CodeCover SDK option.\n\
       /DE      Use DotNet Enhancements.\n\
       /E       Set encryption seed by using two numbers which are seperated by Dot(.).\n\
       /DUpath  Set InputFilePath for additional Files.\n\
       /DPpath  Set OutputFilePath for additional Files.\n\
       /DFfiles Set additional FileNames/Extension.\n\
       /GV      Set option to enable extended custom locking.\n\
       /GDname  Set DLL name for extended custom locking.\n\
       /GFname  Set Function name for extended custom locking.\n\n\
       /S1secret ... /S7secret  Secrets (1 to 7) for challenge-response.\n\n\
       @fname   Reads parameters from fname.\n\
       \n"


#define VL10N_SHL_INF_COMMAND_USAGE_STR2      "To specify a hexadecimal number, prefix the number with a '$'.\n\
       FLname   = Licensed feature name (up to 24 characters).\n\
       path     = Drive and/or subdirectory of the file.\n\
       progname = Program to be protected.\n\
       newname  = Resulting protected file, default newname = progname.\n\
       \n\
      (Please refer to the manual and the release disk(s) for more information.)\n"

/*-------------------------------------------------------------------*
 * Error messages that will be displayed by the shell during run-time
 * of shelled applications.
 *-------------------------------------------------------------------*/
#define VL10N_SHL_ERR_RUN_TIME_01_STR         "Initialization error."
#define VL10N_SHL_ERR_RUN_TIME_02_STR          "Could not get a license."
#define VL10N_SHL_ERR_RUN_TIME_03_STR         "Error loading program."
#define VL10N_SHL_ERR_RUN_TIME_04_STR         "Memory access error."
#define VL10N_SHL_ERR_RUN_TIME_05_STR         "Error loading import library :"
#define VL10N_SHL_ERR_RUN_TIME_06_STR         "Error importing library function."
#define VL10N_SHL_ERR_RUN_TIME_07_STR         "Heartbeat failed."
#define VL10N_SHL_ERR_RUN_TIME_08_STR         "License renewal failed."
#define VL10N_SHL_ERR_RUN_TIME_09_STR         "Memory allocation error."
#define VL10N_SHL_ERR_RUN_TIME_10_STR         "Debugger present."
#define VL10N_SHL_ERR_RUN_TIME_11_STR         "Memory unexpectedly modified."
#define VL10N_SHL_ERR_RUN_TIME_12_STR         "SEH chain unexpectedly modified."
#define VL10N_SHL_ERR_RUN_TIME_13_STR         "Invalid decryption key."
#define VL10N_SHL_ERR_RUN_TIME_14_STR         "File checksum error."
#define VL10N_SHL_ERR_RUN_TIME_15_STR         "Code/data modified (file or memory)."
#define VL10N_SHL_ERR_RUN_TIME_16_STR         "Thread Error."
#define VL10N_SHL_ERR_RUN_TIME_17_STR         "Original Relocation Error."
#define VL10N_SHL_ERR_RUN_TIME_18_STR         "Shell Relocation Error."
#define VL10N_SHL_ERR_RUN_TIME_19_STR         "Tlv Decryption Error."
#define VL10N_SHL_ERR_RUN_TIME_20_STR         "Error in decrypting Original Sections."
#define VL10N_SHL_ERR_RUN_TIME_21_STR         "Error in decrypting NextLayer Sections."
#define VL10N_SHL_ERR_RUN_TIME_22_STR         "Error in decrypting AppInfo."
#define VL10N_SHL_ERR_RUN_TIME_23_STR         "UnLocked License."
#define VL10N_SHL_ERR_RUN_TIME_24_STR         "License Version not supported."
#define VL10N_SHL_ERR_RUN_TIME_25_STR         "UnMatched VendorInfo."
#define VL10N_SHL_ERR_RUN_TIME_26_STR         "License is not issued due to time tampering."
#define VL10N_SHL_ERR_RUN_TIME_27_STR         "Memory CheckSum Failed."
#define VL10N_SHL_ERR_RUN_TIME_28_STR         "Code CheckSum Failed."
#define VL10N_SHL_ERR_RUN_TIME_29_STR         "Create File Failed"
#define VL10N_SHL_ERR_RUN_TIME_30_STR         "File Size Error"
#define VL10N_SHL_ERR_RUN_TIME_31_STR         "Failed to Seek Start of File"
#define VL10N_SHL_ERR_RUN_TIME_32_STR         "Failed to Read File"
#define VL10N_SHL_ERR_RUN_TIME_33_STR         "Failed to Get Module Name."
#define VL10N_SHL_ERR_RUN_TIME_34_STR         "BreakPoint Detected on Win32 API."
#define VL10N_SHL_ERR_RUN_TIME_35_STR         ".NET Framework %s not installed."
#define VL10N_SHL_ERR_RUN_TIME_36_STR         "Virtual Machine is detected."
#define VL10N_SHL_ERR_RUN_TIME_37_STR         "RMS License DLL does not exist."
#define VL10N_SHL_ERR_RUN_TIME_38_STR         "Invalid RMS License DLL."
/*-------------------------------------------------------------------------*
 * Error messages that will be displayed by the shell during the process of
 * protecting an application.
 *-------------------------------------------------------------------------*/
#define VL10N_SHL_ERR_SHELL_TIME_01_STR       "License meter driver initialization error."
#define VL10N_SHL_ERR_SHELL_TIME_02_STR       "License meter version mismatch."
#define VL10N_SHL_ERR_SHELL_TIME_03_STR       "Meter key access error."
#define VL10N_SHL_ERR_SHELL_TIME_04_STR       "Couldn't read file (error=%d)"
#define VL10N_SHL_ERR_SHELL_TIME_05_STR       "Memory allocation error (error=%d)"
#define VL10N_SHL_ERR_SHELL_TIME_06_STR       "Couldn't open file '%s'."
#define VL10N_SHL_ERR_SHELL_TIME_07_STR       "Couldn't write to file (error=%d)"
#define VL10N_SHL_ERR_SHELL_TIME_08_STR       "Cannot overwrite '%s'."
#define VL10N_SHL_ERR_SHELL_TIME_09_STR       "Out of range (%ld)."
#define VL10N_SHL_ERR_SHELL_TIME_10_STR       "Numeric value expected."
#define VL10N_SHL_ERR_SHELL_TIME_11_STR       "Syntax error '%s'."
#define VL10N_SHL_ERR_SHELL_TIME_12_STR       "Bad option '%s'."
#define VL10N_SHL_ERR_SHELL_TIME_13_STR       "Bad file information."
#define VL10N_SHL_ERR_SHELL_TIME_14_STR       "Invalid file attributes."
#define VL10N_SHL_ERR_SHELL_TIME_15_STR       "User's choice to end the program."
#define VL10N_SHL_ERR_SHELL_TIME_16_STR       "Valid product token or demo licenses required!"
#define VL10N_SHL_ERR_SHELL_TIME_17_STR       "Couldn't customize shelled application."
#define VL10N_SHL_ERR_SHELL_TIME_18_STR       "Product serial number required."
#define VL10N_SHL_ERR_SHELL_TIME_19_STR       "Invalid product authorization code."
#define VL10N_SHL_ERR_SHELL_TIME_20_STR       "Cannot create temp file."
#define VL10N_SHL_ERR_SHELL_TIME_21_STR       "Failed in Crypt function."
#define VL10N_SHL_ERR_SHELL_TIME_22_STR       "Import Overwrite not supported with SmartHeap"
#define VL10N_SHL_ERR_SHELL_TIME_23_STR       "Import Overwrite not supported with this Application"
#define VL10N_SHL_ERR_SHELL_TIME_24_STR       ".NET Framework is required to protect .NET DLLs"
#define VL10N_SHL_ERR_SHELL_TIME_25_STR       "Failed to handle this file"
#define VL10N_SHL_ERR_SHELL_TIME_26_STR       "Select Option /DU before %s for setting path for additional File(s)."
#define VL10N_SHL_ERR_SHELL_TIME_27_STR       "Invalid User Capacity value selected.Please select User Capacity value less than Team Capacity value."
#define VL10N_SHL_ERR_SHELL_TIME_28_STR       "Invalid Filter selected.*.* and *notepad.* filters are not allowed."
#define VL10N_SHL_ERR_SHELL_TIME_29_STR       "Not a valid WIN32 file"
#define VL10N_SHL_ERR_SHELL_TIME_30_STR       "Not for I386 CPU"
#define VL10N_SHL_ERR_SHELL_TIME_31_STR       "Section not found"
#define VL10N_SHL_ERR_SHELL_TIME_32_STR       "Invalid relocation (%08lX)"
#define VL10N_SHL_ERR_SHELL_TIME_33_STR       "Cannot handle Win32 files"
#define VL10N_SHL_ERR_SHELL_TIME_34_STR       "Exceed name size limit of %d."
#define VL10N_SHL_ERR_SHELL_TIME_35_STR       "The name of imported DLL is too long"
#define VL10N_SHL_ERR_SHELL_TIME_36_STR       "The name of imported function is too long"
#define VL10N_SHL_ERR_SHELL_TIME_37_STR       ".NET applications signed with strong names can't be protected"
#define VL10N_SHL_ERR_SHELL_TIME_38_STR       ".NET Framework 1.0 is required to protect .NET 1.0 applications"
#define VL10N_SHL_ERR_SHELL_TIME_39_STR       ".NET Framework SDK 1.0 is required to protect .NET 1.0 applications"
#define VL10N_SHL_ERR_SHELL_TIME_40_STR       ".NET Framework 1.1 is required to protect .NET 1.1 applications"
#define VL10N_SHL_ERR_SHELL_TIME_41_STR       ".NET Framework SDK 1.1 is required to protect .NET 1.1 applications"
#define VL10N_SHL_ERR_SHELL_TIME_42_STR       ".NET Framework 2.0/3.0 is required to protect .NET 2.0 or 3.0 applications"
#define VL10N_SHL_ERR_SHELL_TIME_43_STR       ".NET Framework SDK 2.0 is required to protect .NET 2.0 or 3.0 applications"
#define VL10N_SHL_ERR_SHELL_TIME_44_STR       "This version of .NET application is not supported."
#define VL10N_SHL_ERR_SHELL_TIME_45_STR       "Fail to copy file '%s' in the protected directory."
#define VL10N_SHL_ERR_SHELL_TIME_46_STR       "Shelling failed due to file HeaderSize exceeded the limit."
#define VL10N_SHL_ERR_SHELL_TIME_47_STR       "x64 bit applications are not supported."
#define VL10N_SHL_ERR_SHELL_TIME_48_STR       "Configuration File is supported with StandAlone mode only."
#define VL10N_SHL_ERR_SHELL_TIME_49_STR       "Code Morphing not supported with this Application."

/* End of Shell strings */

/* Same as VL10N_SLD_ERR_RENEWLIC_STR */
#define VL10N_SHL_ERR_HRTBT_FAILURE           "Heartbeat stopped! Failed to renew licenses!"
/* Same as VL10N_COM_ERR_SPECIFY_FTR_NM_STR */
#define VL10N_SHL_ERR_APP_UNNAMED             "Feature Name must be specified. It cannot be empty."
/* Same as VL10N_COM_ERR_INTERNAL_ERROR_STR */
#define VL10N_SHL_ERR_INTERNAL_ERROR          "Internal error."
/* Same as VL10N_CLL_ERR_NO_LICKEY_01_STR */
#define VL10N_SHL_ERR_NO_LIC_GIVEN            "Unable to obtain licensing token for feature."
/* Same as VL10N_CLL_ERR_NO_SRV_02_STR */
#define VL10N_SHL_ERR_UNKNOWN_HOST            "Failed to resolve the server host."
/* Same as VL10N_CLL_ERR_SRV_HOSTNM_STR */
#define VL10N_SHL_ERR_NO_SERVER_FILE          "Failed to figure out the license server correctly. Set environment variable LSHOST to (tilde-separated) name(s) of server(s)."

/* Same as VL10N_CLL_ERR_FEAT_02_STR */
#define VL10N_SHL_ERR_APP_NODE_LOCKED         "Feature is not licensed to run on this machine due to server/client lock-code mismatch."

/* Same as VL10N_CLL_ERR_NO_LICKEY_02_STR */
#define VL10N_SHL_ERR_NO_KEY_TO_RETURN        "Attempt to return a non-existent token."
#define VL10N_SHL_ERR_NO_MORE_FEATURE         "No more features to report."
#define VL10N_SHL_ERR_SEVERE_INTRNL_ERR       "Severe internal error in licensing or accessing feature."
/* Same as VL10N_CLL_ERR_NO_SRV_03_STR  */
#define VL10N_SHL_ERR_SRVR_NOT_RESP           "License server is not responding."
/* Same as VL10N_CLL_ERR_FEAT_04_STR  */
#define VL10N_SHL_ERR_USER_EXCLUDED           "This user/machine has been excluded from accessing feature."
/* Same as  VL10N_CLL_ERR_FEAT_05_STR */
#define VL10N_SHL_ERR_UNK_SHARED_ID           "Unknown shared ID specified for feature. "
/* Same as VL10N_CLL_ERR_NO_SRV_04_STR */
#define VL10N_SHL_ERR_NO_RESP_ON_BRDCAST      "Probably no servers are running on this subnet."

/* Same as VL10N_CLL_ERR_NOLIC_STR  */
#define VL10N_SHL_ERR_NO_SUCH_FEATURE         "No license code is available for feature on host."

/* Same as VL10N_CLL_ERR_LICADD_FAIL_STR  */
#define VL10N_SHL_ERR_ADD_LIC_FAILED          "Failed to add license string to the license server."
#define VL10N_SHL_ERR_REMOTE_UPDATE           "The last update was done remotely by the Sentinel RMS License Manager."
/* Same as VL10N_CLL_ERR_VENDORID_01_STR  */
#define VL10N_SHL_ERR_VENDOR_MISMATCH         "Feature is licensed by a different vendor."
/* Same as VL10N_CLL_ERR_VENDORID_02_STR  */
#define VL10N_SHL_ERR_MULT_VENDOR             "Feature is licensed by multiple vendors."
 /* Same as VL10N_CLL_ERR_SRV_MSG_STR  */
#define VL10N_SHL_ERR_BAD_SERVER_MSG         "Could not understand the message received from license server on host.Client-server version mismatch?"
/* Same as VL10N_CLL_ERR_CLK_TAMPER_STR  */
#define VL10N_SHL_ERR_CLK_TAMPER              "Request denied due to clock tamper detection."
 /* Same as VL10N_CLL_ERR_UNAUTHORIZED_STR  */
#define VL10N_SHL_ERR_UNAUTH_OPR              "You are not authorized to perform the requested operation."
/* Same as VL10N_CLL_ERR_INVALID_DOMAIN_STR  */
#define VL10N_SHL_ERR_INVALID_DOMAIN           "Cannot perform this operation on the domain name specified."
 /* Same as VL10N_CLL_ERR_TAGTYPE_01_STR  */
#define VL10N_SHL_ERR_UNKNOWN_TAG_TYPE        "Tag type is not known to server."
/* Same as VL10N_CLL_ERR_TAGTYPE_02_STR  */
#define VL10N_SHL_ERR_INVALID_TAG_TYPE       "Tag type is incompatible with requested operation."
/* Same as VL10N_CLL_ERR_TAGTYPE_03_STR */
#define VL10N_SHL_ERR_UNKNOWN_TAG             "Supplied tag is not known to the license server on host."
 /* Same as VL10N_CLL_ERR_TAGTYPE_04_STR */
#define VL10N_SHL_ERR_UPD_TAG_KEY             "Invalid attempt to update a tagged key."
/* Same as VL10N_CLL_ERR_LOGFILE_01_STR */
#define VL10N_SHL_ERR_LOG_NOT_FOUND           "The specified log filename not found on License Server."
 /* Same as VL10N_CLL_ERR_LOGFILE_02_STR  */
#define VL10N_SHL_ERR_CANNOT_CHANGE_LOG       "Cannot change specified log filename on License Server."
 /* Same as VL10N_CLL_ERR_FNGPRN_MISMATCH_STR */
#define VL10N_SHL_ERR_FNGRPRNT_MISMATCH        "Machine's Fingerprint Mismatch for feature."
/* Same as VL10N_CLL_ERR_TRIALLIC_EXHAUST_STR */
#define VL10N_SHL_ERR_TRIAL_EXHAUSTED         "Duration of Trial License is exhausted."
 /* Same as VL10N_CLL_INF_NOUPDATES_STR */
#define VL10N_SHL_ERR_FEATURE_NOT_UPDT        "No Updates have taken place for feature so far."
#define VL10N_SHL_ERR_OUT_OF_QUE_RES          "Could not locate enough resources to queue for license feature."
/* Same as VL10N_CLL_ERR_NO_CLIENT_STR  */
#define VL10N_SHL_ERR_CLIENT_NOT_FOUND         "Could not find the specified client for feature."
 /* Same as VL10N_CLL_ERR_CL_NOTAUTHORIZED_STR */
#define VL10N_SHL_ERR_CLIENT_NOT_AUTH         "Client not authorized for the specified action for feature."
 /* Same as VL10N_CLL_ERR_DIST_CRIT_STR  */
#define VL10N_SHL_ERR_BAD_DIST_CRIT            "Distribution criteria given is not correct"
 /* Same as VL10N_CLL_ERR_LEAD_UNKNOWN_STR */
#define VL10N_SHL_ERR_LEADER_UNKNOWN            "Current leader server of redundant server pool is not known."
/* Same as VL10N_CLL_ERR_SRV_EXISTS_01_STR */
#define VL10N_SHL_ERR_SRVR_ARDY_PRESENT        "Server already exists in the redundant server pool."
/* Same as VL10N_COM_ERR_FILE_OPEN_01_STR */
#define VL10N_SHL_ERR_FILE_OPEN_ERROR         "File open error."
/* Same as VL10N_CLL_UNRESOLVED_HOSTNAME_STR */
#define VL10N_SHL_ERR_BAD_HOSTNAME           "Hostname given is unresolved."
/* Same as VL10N_CLL_ERR_DIFF_LIB_VER_STR */
#define VL10N_SHL_ERR_DIFF_LIB_VER           "The license server fails to identify the client application version."
/* Same as VL10N_CLL_ERR_NON_REDUNDANT_SRV_STR */
#define VL10N_SHL_ERR_NON_REDUNDANT_SRVR      "A non-redundant server contacted for redundant server related information."
/* Same as VL10N_CLL_INF_MSG_FORWARD_STR  */
#define VL10N_SHL_ERR_MSG_TO_LEADER           "Message forwarded to the leader server."
/* Same as VL10N_CLL_ERR_UPDATE_FAIL_STR  */
#define VL10N_SHL_ERR_CNTCT_SRVR_FAILOV       "Update Failure. Contact another fail-over server."
/* Same as VL10N_CLL_UNRESOLVED_IPADD_STR */
#define VL10N_SHL_ERR_UNRSLVD_IP_ADDR         "IP address given cannot be resolved."
/* Same as VL10N_CLL_INVALID_IPADD_STR  */
#define VL10N_SHL_ERR_INVALID_IP_ADDR         "Invalid IP address format."
/* Same as VL10N_CLL_INF_SRV_SYNC_02_STR  */
#define VL10N_SHL_ERR_SYNCH_SRVR_TABLE        "Server is synchronizing the distribution table."
/* Same as VL10N_CLL_INF_SRVPOOL_FULL_STR */
#define VL10N_SHL_ERR_POOL_FULL               "The redundant server pool already has the maximum number of servers. No more servers can be added."
/* Same as VL10N_CLL_INF_SRVPOOL_EMPTY_STR */
#define VL10N_SHL_ERR_ONLY_SRVR               "The redundant server pool has only one server. It cannot be deleted."
/* Same as VL10N_CLL_ERR_FEATURE_INACTIVE_STR */
#define VL10N_SHL_ERR_FEATURE_INACTIVE        "The feature is either unavailable on the server or server is non-redundant. "
/* Same as VL10N_CLL_INF_MAJRULE_FAIL_STR */
#define VL10N_SHL_ERR_MAJ_RULE_FAIL           "The token for feature cannot be issued because of majority rule failure."
/* Same as VL10N_CLL_ERR_CONFIG_FILE_STR */
#define VL10N_SHL_ERR_CONF_FILE              "Configuration file modifications failed. Check the given parameters."
/* Same as VL10N_CLL_ERR_NON_REDUNDANT_FEATURE_STR */
#define VL10N_SHL_ERR_NON_REDUNDANT_FEA       "A non-redundant feature given for redundant feature related operation."
/* Same as VL10N_CLL_ERR_ELM_LIC_STR */
#define VL10N_SHL_ERR_ELM_LIC_DISABLED         "Elan License of feature is Inactive . "
/* Same as VL10N_CLL_ERR_COMM_CODE_01_STR */
#define VL10N_SHL_ERR_CM_CODE_NOT_EXIST       "Commuter code for this feature does not exist on the client system."
/* Same as VL10N_CLL_ERR_CL_EXISTS_STR */
#define VL10N_SHL_ERR_SL_ALREADY_EXIST        "Client already exists on server. "
/* Same as VL10N_CLL_ERR_COMM_INFO_STR */
#define VL10N_SHL_ERR_CL_PRST_NOT_EXIST       "Failed to get client commuter info on server \"no-net\"."
/* Same as VL10N_CLL_ERR_INTERNAL_01_STR */
#define VL10N_SHL_ERR_INSTALL_FAILED           "Internal error in licensing or accessing feature. "
/* Same as VL10N_CLL_ERR_COMM_UNINST_STR */
#define VL10N_SHL_ERR_UNINST_CM_FAIL          "Unable to uninstall the client commuter license. "
/* Same as VL10N_CLL_ERR_COMM_ISSUE_STR */
#define VL10N_SHL_ERR_ISSUE_CODE_FAIL          "Unable to issue a commuter license to client. "
/* Same as VL10N_CLL_ERR_NON_COMMUTER_LICENSE_STR */
#define VL10N_SHL_ERR_NON_COMMUTER_LICENSE_STR        "A non-commuter license is requested for commuter related operation by the client. "
/* Same as VL10N_CLL_ERR_NO_COMM_KEYS_STR  */
#define VL10N_SHL_ERR_CM_KEYS_UNAVAIL         "Not enough commuter tokens available on server. "
/* Same as VL10N_CLL_ERR_COMM_CLINFO_STR  */
#define VL10N_SHL_ERR_CL_INVALID_INFO         "Invalid commuter info from Client . "
/* Same as VL10N_CLL_ERR_COMM_CODE_03_STR */
#define VL10N_SHL_ERR_CL_ALREADY_EXIST        "Client commuter license already exists on  \"no-net\". "
/* Same as VL10N_CLL_ERR_COMM_CODE_02_STR */
#define VL10N_SHL_ERR_CL_CODE_NOT_EXIST        "Client commuter license does not exist \"no-net\". "
/* Same as VL10N_CLL_INF_SRV_SYNC_01_STR  */
#define VL10N_SHL_ERR_SYNC_IN_PROGRESS        "Server synchronization in progress. Please wait..."
/* Same as VL10N_CMT_ERR_COMMUTER_LICENSE_IS_CHECKED_REMOTELY_MSG  */
#define VL10N_SHL_ERR_REMOTE_CHECKOUT        "This commuter license is checked out remotely, so it cannot be checked-in!\n"
/* Same as VL10N_CLL_ERR_REM_COMM_AUTH_STR  */
#define VL10N_SHL_ERR_FAIL_COMM_CODE        "Error installing the commuter authorization code.\n"
/* Same as VL10N_CLL_ERR_REM_COMM_LOCK_STR  */
#define VL10N_SHL_ERR_GET_MACHINEID        "Error getting the locking information for the client.\n"
/* Same as VL10N_CLL_ERR_INVLAID_COMM_LOCK_STR  */
#define VL10N_SHL_ERR_INVALID_MACHINEID_STR        "Invalid locking code string.\n"
/* Same as VL10N_CLL_ERR_COMM_CODE_04_STR  */
#define VL10N_SHL_ERR_EXCEEDS_LICENSE_LIFE        "Cannot issue commuter code. \nLicense expiration is less than the requested days for commuter code. \n"
/* Same as VL10N_COM_ERR_TERMINAL_SERVER_ERR  */
#define VL10N_SHL_ERR_TERMINAL_SERVER_FOUND        "Operating in stand-alone mode using terminal client. This is not allowed by the vendor.\n"
/* Same as VL10N_COM_ERR_RDP_SESSION_FOUND  */
#define VL10N_SHL_ERR_RDP_SESSION_FOUND            "Operating in stand-alone mode using remote desktop session. This is not allowed by the vendor.\n"
/* Same as VL10N_CLL_ERR_NO_APPROPRIATE_LIB_STR  */
#define VL10N_SHL_ERR_UNAPPROPRIATE_LIBRARY        "Feature is not supported in Network-only mode of library.\n"
/* Same as VL10N_CLL_ERR_INVALID_FILETYPE  */
#define VL10N_SHL_ERR_INVALID_FILETYPE        "The specified file type is not supported.\n"
/* Same as VL10N_COM_ERR_OLD_SERVER  */
#define VL10N_SHL_ERR_NOT_SUPPORTED        "The requested operation is not supported on this license server.\n"
/* Same as VL10N_CLL_ERR_INVALID_LIC_FAIL_STR  */
#define VL10N_SHL_ERR_INVALID_LICENSE        "The given license code is invalid. Hence, it could not be added to the \"%s\" license server.\n"
/* Same as VL10N_CLL_ERR_DUPLICATE_LIC_FAIL_STR  */
#define VL10N_SHL_ERR_DUPLICATE_LICENSE        "The given license code is already added to the \"%s\" license server.\n"
/* Same as VL10NN_CLL_ERR_NO_USER_CAPACITY_STR  */
#define VL10N_SHL_ERR_INSUFF_USER_CAP        "Insufficient user capacity available.\n"
/* Same as VL10NN_CLL_ERR_TEAM_LIMIT_EXHAUSTED_STR  */
#define VL10N_SHL_ERR_TEAM_LIM_EXHAUST        "Team limit exhausted.\n"
/* Same as VL10NN_CLL_ERR_NO_TEAM_CAPACITY_STR  */
#define VL10N_SHL_ERR_INSUFF_TEAM_CAP        "Insufficient team capacity available.\n"
/* Same as VL10N_LSL_ERR_CANT_DEL_UPGRADED_LIC_STR  */
#define VL10N_SHL_ERR_CANNOT_DEL_LIC        "Deletion of upgraded feature/license is not allowed.\n"
/* Same as VL10N_CLL_ERR_UPGRADE_NOT_ALLOWED_STR  */
#define VL10N_SHL_ERR_UPGRADE_FAIL        "License upgrade feature is not allowed on the specified license type."
/* Same as VL10N_COMM_ERR_MARKED_FOR_DEL  */
#define VL10N_SHL_ERR_DEL_MARK_FEAT        "This feature exists on this machine and it is already marked for check-in.\n"
/* Same as VL10NN_CLL_ERR_TEAM_EXCLUDED_STR  */
#define VL10N_SHL_ERR_TEAM_EXCLUDED        "This team has been excluded from accessing feature %s.\n"
/* Same as VL10N_CLL_ERR_NETOWRK_SRV_STR  */
#define VL10N_SHL_ERR_NETWORK_SRVR        "A network server contacted for standalone server related information.\n"
/* Same as VL10N_CLL_ERR_PERPETUAL_LICENSE_STR  */
#define VL10N_SHL_ERR_PERPETUAL_LICENSE        "The contacted feature is a Repository License.\n"
/* Same as VL10N_CLL_ERR_COMMUTER_CHECKOUT_STR  */
#define VL10N_SHL_ERR_COMMUTER_CHECKOUT        "A commuter token has already been checked out for this license.\n"
/* Same as VL10N_VLS_REVOKE_ERR_NO_FEATURE_STR  */
#define VL10N_SHL_ERR_NO_FEATURE        "License with given feature name and feature version does not exist on the server.\n"
/* Same as VL10N_VLS_REVOKE_ERR_CORRUPT_MESSAGE_STR  */
#define VL10N_SHL_ERR_CORRUPT_MESSAGE        "The client message received by the server was corrupted.\n"
/* Same as VL10N_VLS_REVOKE_ERR_OUT_VALID_RANGE_STR  */
#define VL10N_SHL_ERR_OUT_VALID_RANGE        "The received number Of licenses to revoke is out of range.\n"
/* Same as VL10N_VLS_REVOKE_ERR_MD5_PLUGIN_LOAD_FAIL_STR */
#define VL10N_SHL_ERR_MD5_PLUGIN_LOAD_FAIL        "Error loading the MD5 plugin dll at the server.\n"
/* Same as VL10N_VLS_REVOKE_ERR_MD5_PLUGIN_EXEC_FAIL_STR */
#define VL10N_SHL_ERR_MD5_PLUGIN_EXEC_FAIL        "Error in executing the authentication plugin.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INSUFFICIENT_FEATURE_LICENSES_STR  */
#define VL10N_SHL_ERR_INSUFF_FEAT_LIC        "This feature has less number of total licenses.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INSUFFICIENT_DEFAULT_GROUP_STR  */
#define VL10N_SHL_ERR_INSUFF_DEF_GROUP        "Default group does not has sufficient licenses, reconfigure your user reservation file.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INSUFFICIENT_FREE_IN_DEFAULT_STR  */
#define VL10N_SHL_ERR_INSUFF_DEFAULT        "Currently required number of licenses are not free for revocation in the default group.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INVALID_SESSION_ID_STR  */
#define VL10N_SHL_ERR_INVALID_SESSIONID        "Invalid session id received from the client.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INVALID_PASSWORD_STR  */
#define VL10N_SHL_ERR_INVALID_PASSWORD        "Invalid password for revocation.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INTERNAL_SERVER_STR  */
#define VL10N_SHL_ERR_INTERNAL_SERVER        "Revocation failed due to internal server error.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INFINITE_GRP_DIST_STR  */
#define VL10N_SHL_ERR_INFINITE_GRP_DIST        "Infinite revocation not possible with enabled group distribution.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INFINITE_LIC_IN_USE_STR  */
#define VL10N_SHL_ERR_INFINITE_LIC        "All licenses must be free for infinite revocation.\n"
/* Same as VL10N_VLS_REVOKE_ERR_INFINITE_LIC_FINITE_REQ_STR  */
#define VL10N_SHL_ERR_INFINITE_LIC_REQ        "License has infinite keys. Only infinite license revocation request is allowed for this license.\n"
/* Same as VL10N_VLS_REVOKE_ERR_TICKET_GENERATION_STR  */
#define VL10N_SHL_ERR_TICKET_GEN        "Revocation ticket generation failed.\n"
/* Same as VL10N_VLS_REVOKE_ERR_CODGEN_VERSION_UNSUPPORTED_STR  */
#define VL10N_SHL_ERR_VER_UNSUPPORTED        "Revocation feature is not supported for the given license version.\n"
/* Same as VL10N_VLS_REVOKE_ERR_RDNT_LIC_UNSUPPORED_STR  */
#define VL10N_SHL_ERR_RDNT_LIC_UNSUPPORED        "Revocation feature is not supported for redundant licenses.\n"
/* Same as VL10N_VLS_REVOKE_ERR_CAPACITY_LIC_UNSUPPORED_STR  */
#define VL10N_SHL_ERR_CAP_LIC_UNSUPPORED        "Revocation feature is not supported for capacity licenses.\n"
/* Same as VL10N_VLS_REVOKE_ERR_UNEXPECTED_AUTH_CHLG_PKT_STR  */
#define VL10N_SHL_ERR_UNEXPECTED_PKT        "Failure occurred due to unexpected challenge packet received from server.\n"
/* Same as VL10N_VLS_REVOKE_ERR_TRIAL_LIC_UNSUPPORED_STR  */
#define VL10N_SHL_ERR_TRIAL_LIC_UNSUPPORED        "Revocation not supported for trial licenses.\n"
/* Same as VL10N_VLS_REQUIRED_LOCK_FIELDS_NOT_FOUND  */
#define VL10N_SHL_ERR_FIELDS_NOT_FOUND        "Required locking criteria for local request is not available on your machine.\n"
/* Same as VL10N_VLS_NOT_ENOUGH_LOCK_FIELDS  */
#define VL10N_SHL_ERR_NOTENOUGH_FIELDS        "Minimum number of locking criteria for local request is not found.\n"
/* Same as VL10N_VLS_VLS_REMOTE_CHECKOUT_NOT_ALLOWED_FOR_PERPETUAL  */
#define VL10N_SHL_ERR_CHECKOUT        "Remote checkout is not allowed for repository license.\n"
/* Same as VL10N_CLL_ERR_GRACE_LIC_INSTALL_FAIL_STR  */
#define VL10N_SHL_ERR_GRACE_LICFAIL        "Feature %s cannot run as installation of grace license is failed.\n"
#define VL10N_SHL_ERR_NONET_LIBRARY        "The feature is not supported in no-net mode.\n"
#define VL10N_SHL_ERR_INACTIVE_HANDLE        "\n"
/* Same as VL10N_VLS_LIBRARY_NOT_INITIALIZED  */
#define VL10N_SHL_ERR_UNINI_LIB        "Client library not in initialized state.\n"
/* Same as VL10N_VLS_LIBRARY_ALREADY_INITIALIZED  */
#define VL10N_SHL_ERR_ALREADY_INIT        "Client library is already in initialized state.\n"
/* Same as VL10N_VLS_RESOURCE_LOCK_FAILURE  */
#define VL10N_SHL_ERR_RES_LOCK_FAILURE        "Fail to acquire resource lock.\n"
#define VL10N_SHL_ERR_NO_MORE_LIC        ""
/* Same as VL10N_VLS_NO_SUCH_LICENSE  */
#define VL10N_SHL_ERR_NO_SUCH_LIC        "No license found with the specified feature/version/hash.\n"
/* Same as VL10N_VLS_LICENSE_IN_USE  */
#define VL10N_SHL_ERR_LIC_IN_USE        "Specified has active clients.\n"
/* Same as VL10N_VLS_SET_LICENSE_PRECEDENCE_FAILED  */
#define VL10N_SHL_ERR_LIC_PRECEDENCE        "Failure in setting precedence for specified trial license.\n"
/* Same as VL10N_VLS_STORE_ACCESS_ERROR  */
#define VL10N_SHL_ERR_STORE_ACCESS        "Failure in accessing the license or persistence store.\n"
/* Same as VL10N_VLS_LOCK_SELECTOR_INVALID  */
#define VL10N_SHL_ERR_INVALID_LOCKSEL        "Specified lock selector is not valid.\n"
/* Same as VL10N_VLS_LOCK_CODE_NOT_SUPPORTED  */
#define VL10N_SHL_ERR_UNSUPP_LOCKCODE        "The specified lock code is not supported by the library/server.\n"
/* Same as VL10N_VLS_LOCK_CODE_VER_INVALID  */
#define VL10N_SHL_ERR_INVALID_LOCKVER        "Invalid lock code version.\n"
/* Same as VL10N_VLS_LOCK_CODE_INVALID  */
#define VL10N_SHL_ERR_INVALID_LOCKCODE        "The specified lock code is invalid.\n"
/* Same as VL10N_VLS_NO_AVAILABLE_MACHINE_ID  */
#define VL10N_SHL_ERR_UNAVAIL_MACHINEID        "No available machine id for specified lock selector.\n"
/* Same as VL10N_VLS_CODE_GENERATOR_LIBRARY_FAILED  */
#define VL10N_SHL_ERR_CODEGEN_LIB        "Failure in initializing code generator library.\n"
/* Same as VL10N_CLL_ERR_TRIALLIC_DATA_ACCESS  */
#define VL10N_SHL_ERR_TRIAL_ACCESS        "The Trial feature  \"%s\" is not accessible.\n"
/* Same as VL10N_CLL_ERR_TRIALLIC_DATA_INCONSISTENT  */
#define VL10N_SHL_ERR_TRIAL_INCONSIST        "Trial License data inconsistent for feature %s.\n"
/* Same as VL10N_CLL_ERR_TRIALLIC_DATE_RESTRICTION  */
#define VL10N_SHL_ERR_TRIAL_RESTRICT        "Trial license date restriction error for feature %s.\n"
/* Same as VL10N_CLL_ERR_TRIALLIC_NOT_ACTIVATED  */
#define VL10N_SHL_ERR_INACTIVE_TRIAL        "The Trial licensing for feature \"%s\" is disabled.\n"
/* Same as VL10N_VLS_NONET_LIBRARY  */
#define VL10N_SHL_ERR_LIBRARY        "A network request is made to the standalone library.\n"
/* Same as VL10N_VLS_NON_TRIAL_LICENSE  */
#define VL10N_SHL_ERR_NON_TRIAL_LIC        "Active feature on the server is not a trial license.\n"
/* Same as VL10N_VLS_LICENSE_START_DATE_NOT_REACHED  */
#define VL10N_SHL_ERR_START_DATE        "License start date not yet reached.\n"
/* Same as VL10N_CGN_INF_GC_LENGTH_OVERFLOW_ERR  */
#define VL10N_SHL_ERR_GRACE_OVERFLOW        "Grace code length exceeds maximum limit.\n"
#define VL10N_SHL_ERR_NO_MORE_ITEMS        "\n"
/* Same as VL10N_CLL_ERR_FINGERPRINT_VALUE_NOT_FOUND_STR  */
#define VL10N_SHL_ERR_FILE_NOT_FOUND        "No fingerprint information is available on the specified index.\n"
/* Same as V_MSG_VLS_LICENSE_DELETION_NOT_ALLOWED  */
#define VL10N_SHL_ERR_DEL_UNALLOW        "License deletion is not supported for this license.\n"
/* Same as VL10N_CLL_ERR_CAPACITY_MISMATCH_STR  */
#define VL10N_SHL_ERR_CAP_MISMATCH        "Capacity type/value mismatch.\n"

/* Same as VL10N_CLL_ERR_GRACE_EXPIRED_STR  */
#define VL10N_SHL_ERR_GRACE_EXPIRED             "Feature %s cannot run as total number of days allowed in grace period have been consumed.\n"
/* Same as VL10N_CLL_ERR_GRACE_HOURS_EXHAUSTED_STR  */
#define VL10N_SHL_ERR_GRACE_HOURSEXHAUST    "Feature %s cannot run as total number of hours allowed in grace period have been consumed.\n"
/* Same as VL10N_CLL_ERR_GRACE_INVALID_STATE_STR  */
#define VL10N_SHL_ERR_GRACE_INVALIDSTATE      "Feature %s cannot run as an unexpected state of grace period is found.\n"



/* Same as VL10N_CLL_ERR_NOLICRESORCES_STR  */
#define VL10N_SHL_ERR_NO_RESOURCES             "Could not locate enough licensing resources to license feature."
/* Same as VL10N_CLL_ERR_LICSYS_AVBL_STR  */
#define VL10N_SHL_ERR_SYS_UNAVAILABLE          "Licensing System not available"
/* Same as VL10N_CLL_ERR_LIC_EXPIRED_01_STR  */
#define VL10N_SHL_ERR_LICENSE_TIMED_OUT      "License server has freed up the resources for the feature. Need more frequent token renewal??"
/* Same as VL10N_CLL_ERR_LICNOTAVBL_STR  */
#define VL10N_SHL_ERR_ALL_LICS_IN_USE         "All licensing tokens with server for feature are already in use."
/* Same as VL10N_CLL_ERR_NO_NETWORK_STR  */
#define VL10N_SHL_ERR_COMM_PROBLEMS           "Unable to talk to host. Communication problems??"
/* Same as VL10N_COM_ERR_UNKNOWN_STR  */
#define VL10N_SHL_ERR_UNK_ERROR_CODE           "Unknown error code, cannot print error message."
/* Same as VL10N_CLL_ERR_BAD_INDEX_STR  */
#define VL10N_SHL_ERR_BAD_INDEX               "Bad Index."
/* Same as VL10N_CLL_ERR_NOADDITIONAL_UNITS_STR */
#define VL10N_SHL_ERR_NO_ADDL_UNITS        "No additional units are available for feature."
/* Same as VL10N_CLL_ERR_SMALL_BUFF_STR */
#define VL10N_SHL_ERR_STRING_TRUNCATED         "Input buffer too small, string may be truncated."
/* Same as VL10N_ERR_NO_SUCCESS_STR  */
#define VL10N_SHL_ERR_NO_SUCCESS              "Failed in performing the requisite operation."
#define VL10N_SHL_LQ_CANCEL_INST              "Press Cancel to remove yourself from the queue."
#define VL10N_SHL_LQ_CANCEL_INST_DESC         "Instruction on how to cancel a queued task."
#define VL10N_SHL_ERR_TERMINAL_SERVER_ERR        "Standalone application cannot run on terminal server.\n"


/* Same as VL10N_CLL_ERR_LIC_EXPIRED_02_STR */
#define VL10N_SHL_ERR_LIC_EXPIRED             "Feature cannot run anymore because the license expiration date has reached.Check your machine's date, then contact your software source."

/* Same as VL10N_CLL_ERR_NORESOURCES_STR  */
#define VL10N_SHL_ERR_OUT_OF_MEMORY           "Insufficient system resources are available to complete the request."

/* Same as VL10N_COM_ERR_BAD_HANDLE_STR */
#define VL10N_SHL_ERR_BAD_HANDLE              " Bad handle."

/* Same as VL10N_CLL_ERR_SRV_EXISTS_02_STR  */
#define VL10N_SHL_ERR_SRVR_NOT_PRESENT        "The given server name does not exist in the redundant server pool."

/* Same as VL10N_CLL_INF_KEY_RET_STR  */
#define VL10N_SHL_ERR_ALL_UNITS_RELEASE       "Returned all the tokens for feature."

/* Same as VL10N_CLL_ERR_TAGTYPE_05_STR  */
#define VL10N_SHL_ERR_TAG_NOT_SUPPORTED       "License server on host does not support tags."

/* Same as VL10N_LSM_ERR_CONTACT_SRV_STR */
#define VL10N_SHL_ERR_NO_SERVER_RUNNING       " Unable to contact any servers. Probably no servers running on this subnet."

/* Same as VL10N_CLL_ERR_FEAT_03_STR  */
#define VL10N_SHL_ERR_RETURN                  "Cannot return the token for feature."

/* Same as VL10N_CLL_ERR_FUNCT_CALL_STR */
#define VL10N_SHL_ERR_FCTN_CALL               "Error in calling API function. Check the calling parameters."

/* Same as VL10N_CLL_ERR_NO_SRV_01_STR */
#define VL10N_SHL_ERR_NO_SRV_01_STR           "Cannot talk to the license server. Server not running??\n"


/* Same as VL10N_ERR_BAD_HANDLE_STR */
#define VL10N_SHL_ERR_BAD_HANDLE_STR          "Client handle refers to an invalid licensing system context.\n"

/* Same as VL10N_CLL_ERR_FEAT_06_STR  */
#define VL10N_SHL_ERR_DEL_LIC_FAILED          "Failed to delete feature from the license server on host."

#define VL10N_SHL_ERR_RESERVED                " Reserved for internal use."
#define VL10N_SHL_ERR_NO_MORE_CM_CODE         " No more commuter code."
#define VL10N_SHL_ERR_NOT_INTG_LIB            " Not linked to integrated library."
/* Same as VL10N_CLL_ERR_QUEUED_HANDLE_STR  */
#define VL10N_SHL_ERR_QUEUED_HANDLE           " The specified client handle is associated with a queued request."
/* Same as VL10N_CLL_ERR_ACTIVE_HANDLE_STR  */
#define VL10N_SHL_ERR_ACTIVE_HANDLE           " The specified client handle is associated with an active request."
/* Same as VL10N_CLL_ERR_AMBIGUOUS_HANDLE_STR  */
#define VL10N_SHL_ERR_AMBIGUOUS_STATE         " Ambiguous client handle! The specified client handle is not associated with any request."
#define VL10N_SHL_ERR_NO_ERR                  " No error."
#define VL10N_SHL_ERR_NO_MORE_CLIENTS         " No more clients to report."
#define VL10N_SHL_ERR_LOCAL_UPDATE            " The last update was done locally."
#define VL10N_SHL_ERR_NO_TRIAL_INFO           " No user information for given feature."
#define VL10N_SHL_ERR_TRIAL_INFO_FAILED       "Can not find trial license information for given feature."

#define VL10N_LSW_LBL_USAGE_STR             VL10N_COM_LBL_USAGE_STR
#define VL10N_CMT_HDR_LSM_STR               VL10N_LSM_HDR_LSM_STR
#define VL10N_CMT_LBL_LS_COPYRIGHT_STR      VL10N_COM_LBL_LS_COPYRIGHT_STR
#define VL10N_LSP_LBL_LS_COPYRIGHT_STR      VL10N_COM_LBL_LS_COPYRIGHT_STR
#define VL10N_LCF_LBL_FEATURE_NAME_STR      "\n Feature name                : \"%s\"\n"
#define VL10N_LCF_LBL_FEATURE_VER_STR       " Feature version             : \"%s\"\n"
/*#define VL10N_LCF_LBL_LIC_TYPE_STR          VL10N_CGN_LBL_LIC_TYPE_STR*/

/*#define VL10N_CLL_ERR_INVALID_LIC_FAIL_STR  "Invalid license string\n"
#define VL10N_CLL_ERR_DUPLICATE_LIC_FAIL_STR "Duplicate license string\n"*/
/*#define VL10N_LSCG_PRM_COMM_HOST_NAME_STR    "Host name\n"*/

// For upgrade library
#define VL10N_UCGN_HDR_LIC_GEN_STR          "\n    %s %s Upgrade License Generation System\n"
#define VL10N_UCGN_INF_FEATURE_NAME_STR         "\nBase Feature Name (any printable characters): "
#define VL10N_UCGN_INF_FEATURE_VER_STR          "\nBase Feature Version (any printable characters): "
#define VL10N_UCGN_INF_UPGRADE_CODE_STR         "\nUpgrade Code : "
#define VL10N_UCGN_HDR_LIC_UPGRADE_FLAG_STR     "\nUpgrade licensing options - \n"
#define VL10N_UCGN_INF_LIC_UPGRADE_FLAG_01_STR  "  [%d] - Version upgrade\n"
#define VL10N_UCGN_INF_LIC_UPGRADE_FLAG_02_STR  "  [%d] - Capacity upgrade\n"
#define VL10N_UCGN_INF_LIC_UPGRADE_FLAG_03_STR  "  [%d] - All\n"
#define VL10N_UCGN_INF_UPGRADE_VER_01_STR       "\nUpgrade Version (any printable character): "
#define VL10N_UCGN_INF_CAPACITY_STR             "\nType Upgrade Capacity of license or press <Enter> for unlimited capacity or type 0 for zero capacity: "
#define VL10N_UCGN_HDR_CAPACITY_UNITS_STR       "\nUpgrade License Capacity options - \n"
#define VL10N_UCGN_INF_CAPACITY_UNITS_STR       "  [%d] From %d through %ld (Multiple of %d)\n"
#define VL10N_UCGN_INF_LIC_CAPACITY_STR         "  Type Upgrade Capacity of license or press <Enter> for unlimited capacity or type 0 for zero capacity: "
#define VL10N_UCGN_INF_CHOOSE_STYLE_STR         "  Type an option that suits you best: "

#define VL10N_UCGN_LBL_BASE_FEATNAME_NONPRT_STR "Base Feature Name contains non printable char"
#define VL10N_UCGN_LBL_BASE_FEATNAME_NONASC_STR "Base Feature Name contains non ASCII char"
#define VL10N_UCGN_LBL_BASE_FEATNAME_STR        "Base Feature Name"
#define VL10N_UCGN_LBL_BASE_FEATVER_NONPRT_STR  "Base Feature Version contains non printable char"
#define VL10N_UCGN_LBL_BASE_FEATVER_NONASC_STR  "Base Feature Version contains non ASCII char"
#define VL10N_UCGN_LBL_BASE_FEATVER_STR         "Base Feature Version"
#define VL10N_UCGN_LBL_UPDCODE_NONPRT_STR       "Upgrade code contains non printable char"
#define VL10N_UCGN_LBL_UPDCODE_NONASC_STR       "Upgrade code contains non ASCII char"
#define VL10N_UCGN_LBL_UPDCODE_STR              "Upgrade Lock Code"
#define VL10N_UCGN_LBL_UPD_FLAG_STR             "Upgrade licensing option"
#define VL10N_UCGN_LBL_UPD_FEATVER_NONPRT_STR   "Upgrade Feature Version contains non printable char"
#define VL10N_UCGN_LBL_UPD_FEATVER_NONASC_STR   "Upgrade Feature Version contains non ASCII char"
#define VL10N_UCGN_LBL_UPD_FEATVER_STR          "Upgrade Feature Version"
#define VL10N_UCGN_LBL_KEY_CAPACITY_UNITS_STR   "Capacity Units"
#define VL10N_UCGN_LBL_CAPACITY_STR             "Capacity"


#define VL10N_UCGN_INF_COMMAND_USAGE_01_STR         "\nUsage: %s \n\
       [-%c New-upgrade-codes-output-file]  [-%c Global-defaults-file]\n\
       [-%s Code-generator-version(1 - max-code-generator-version)]\n\
       [-%c Base-Feature-name]              [-%c Base-Feature-version]\n\
       [-%s Upgrade-code]\n\
       [-%c Upgrade-type  Version/Capacity/All]\n\
       [-%s Upgrade-version]\n\
       [-%s 1s/10s/100s/1k/10k Upgrade-Capacity-units]\n\
       [-%s Upgrade-Capacity | NOLIMIT ] \n"


#define VL10N_UCGN_INF_COMMAND_USAGE_02_STR         "\n \
       NOTES: \n\
       (1) Format X/Y/Z/... takes values as 0/1/2... respectively.\n\
       (2) Symbol | indicates a choice of two values.\n"

/*{ulsdecode STRINGS - Acronym [LSD] }*/
#define VL10N_ULSD_HDR_LIC_DECODE_STR       "\n  %s %s Upgrade License Decoding Utility\n"
#define VL10N_ULSD_INF_CMD_OPT_STR          ":s:"
#define VL10N_ULSD_INF_CMD_USAGE_STR        "\
Usage: %s \n\
       [-%c Upgrade-License-file]   \n"

#define VL10N_ULSD_INF_LICCODE_FILE_STR     "Reading upgrade license codes from file: \"%s\"\n\n"
#define VL10N_ULSD_INF_LICCODE_STR          "\nUpgrade License code: \"%s\"\n"

#define VL10N_UCGN_LBL_LIC_TYPE_01_STR      "\n License Type \t\t\t\t: Version Upgrade \t\n"
#define VL10N_UCGN_LBL_LIC_TYPE_02_STR      "\n License Type \t\t\t\t: Capacity Upgrade \t\n"
#define VL10N_UCGN_LBL_LIC_TYPE_03_STR      "\n License Type \t\t\t\t: Version Upgrade & Capacity Upgrade\t\n"

/* Do not translate */
#define VL10N_UCGN_HDR_LIC_GEN_01_STR        "      INTERNAL UPGRADE CODEGEN LICENSING SYSTEM v%s(%d)\n\n"
#define VL10N_UCGN_INF_LIC_GEN_02_STR       "\nPlease Wait, Generating Upgrade License...\n"
#define VL10N_UCGN_LBL_LIC_GEN_STR          "\nupgrade code generator: %s\n"
#define VL10N_UCGN_ERR_LIC_GEN_STR          "\nError: Unable to generate upgrade license code. \n"

#define VL10N_UCGN_INF_NEW_LICCODE_STR      "\nSaved new upgrade license code(s) to \"%s\".\n\n"
#define VL10N_UCGN_CFM_MORE_LIC_GEN_STR                  "\nDo you want to generate more upgrade licenses? (Y/N) : "

#define VL10N_UCGN_CFM_LIC_GEN_STR           "\nDo you want to generate upgrade license? (Y/N) : "
#define VL10N_UCGN_LBL_NORMAL_LIC_COUNT_STR  "\nAvailable normal license count : %d\n"
#define VL10N_UCGN_INF_LIC_COUNT_DECRE_STR   "\nLicense count that will be decremented from license meter - %d\n"


#endif /* !_SFNT_CISCO_IOS_ */
/* { Messages from file lsMsgTable.c } */

#define VL10N_UCOM_ERR_FEW_LIC_METERS_STR           "Too few units (Normal License Count=%d ) left in your Sentinel RMS Development Kit license meter(s) to generate requested license. %d units required."
#define VL10N_UCOM_ERR_ALL_UNITS_USED_STR           "All %d units of your Sentinel RMS Development Kit license meter(s) have been used up. License generation will fail."

#define VL10N_UCOM_ERR_NO_CAP_AUTH_ULICGEN_STR      "Your Sentinel RMS Development Kit license meter(s) have no authorization to generate Capacity Licenses."
#define VL10N_UCOM_ERR_NO_UPD_AUTH_ULICGEN_STR      "Your Sentinel RMS Development Kit license meter(s) have no authorization to generate Upgrade Licenses."

#define VL10N_UCOM_ERR_SPECIFY_UPGRADE_CODE_STR     "Upgrade Code must be specified. It cannot be empty."
#define VL10N_UCOM_ERR_BASE_LIC_INFO                "The information-feature name, version and vendor code, provided for  base license is incorrect"
#define VL10N_UCOM_ERR_NON_CAPACITY_UPD_NOT_ALLOWED     "Capacity upgrade is not allowed, as the base lic is a non-capacity license."
#define VL10N_UCOM_ERR_UID_DECODE_FAIL              "Invalid Upgrade Code."
#define VL10N_UCOM_INF_ALL_UNITS_USED_UP_STR        "All units of your Sentinel RMS Development Kit license meter(s) have been used up."
#define VL10N_UCOM_INF_LIC_MTR_DECREM_02_STR        "Your Sentinel RMS Development Kit License meter(s) have been decremented by %d units. You now have %d units left."
#define VL10N_UCOM_ERR_POOLED_CAPACITY_UPD_NOT_ALLOWED     "Capacity upgrade is not allowed, as the base lic is a pooled license."
#define VL10N_UCOM_ERR_METER_NOT_SUPPORTED_STR    "Your Sentinel RMS Development Kit license meter is not supported."

/* Few more added in SLM800 in lsMsgTable.c */
#define VL10N_COM_ERR_ENCRYPTION_FAIL              "License code encryption failed."
#define VL10N_COM_ERR_INVALID_CAPACITY_SETTINGS    "Invalid capacity setting done in code structure."
#define VL10N_COM_ERR_EXP_DATE_BEFORE_START_DATE   "License expiration date-time is set before/equal to the license start date-time."

#define VL10N_COM_ERR_INVALID_TRIALHOURS           "Invalid value of trial elapsed hours."
#define VL10N_COM_ERR_INVALID_TRIAL_EXECUTIONCOUNT "Invalid value of trial execution count."

#define VL10N_COM_ERR_GETLOCK_FAIL                 "Failure to acquire resource lock."
#define VL10N_COM_ERR_GETLOCAK_TIMEOUT             "Time-out interval elapsed while acquiring resource lock."
#define VL10N_COM_ERR_VENDOR_DECRYPTION_FAIL       "A failure occurred in vendor specified decryption."

#ifndef _SFNT_CISCO_IOS_

/* Upgrade Tool SLM 7.3.0 */
/*** Start */
#define VL10N_ULC_HDR_STR                   "\n  %s %s Upgrade Code Generator Tool\n"
#define VL10N_ULC_LBL_USAGE__01_STR         " Usage echouid:\n"
#define VL10N_ULC_LBL_USAGE__02_STR         " [ -s <License-filename>  To specify license file containing base license(s) ]\n"
#define VL10N_ULC_LBL_USAGE__03_STR         " [ -e <cnf filename>      To specify license-configuration-file ]\n"
#define VL10N_ULC_LBL_USAGE__04_STR         " [ -o <Upgrade code file> To specify output file to save upgrade code ]\n"
#define VL10N_ULC_LBL_USAGE__05_STR         " [ -h Display usage ]\n"
#define VL10N_ULC_LBL_USAGE__06_STR         " [ -pause Pauses before exiting the command prompt ]\n"

#define VL10N_ULC_INF_HEADER_MSG_STR        "Select one of the options below to find the base license you want \n"\
                                            "to upgrade. After you have selected a base license, an upgrade code \n"\
                                            "will be generated that you can send to your software vendor so an \n"\
                                            "upgrade license can be created for you.\n"

#define VL10N_ULC_PRM_IN_LIC_FILE_STR       "Specify license file: "
#define VL10N_ULC_ERR_OPEN_FILE_STR         "Error opening file \"%s\". Exiting....\n"

#define VL10N_ULC_HDR_MAIN_MENU_STR         "\n                 [ECHOUID SEARCH MENU]\n"
#define VL10N_ULC_OPT_MAIN_MENU_01_STR      "1. Browse through licenses\n"
#define VL10N_ULC_OPT_MAIN_MENU_02_STR      "2. Search by license details\n"
#define VL10N_ULC_OPT_MAIN_MENU_03_STR      "3. Search by order in license file (line number)\n"
#define VL10N_ULC_OPT_MAIN_MENU_04_STR      "4. Quit. \n"

#define VL10N_ULC_PRM_MAIN_MENU_STR         "Select Option (1-4) :"

#define VL10N_ULC_HDR_SUB_MENU_STR          "                   [License Details Sub-Menu]\n"
#define VL10N_ULC_OPT_NEXT_STR                          "N - Next\t\t"
#define VL10N_ULC_OPT_PREVIOUS_STR                      "P - Previous\t\t"
#define VL10N_ULC_OPT_GENERATE_STR                      "G - Generate upgrade code for this license\n"
#define VL10N_ULC_OPT_RETURN_STR                        "R - Return to search menu \n"
#define VL10N_ULC_QUIT_STR                                      "Q - Quit \n"

#define VL10N_ULC_INF_NO_MORE_LIC_01_STR    "Reached the end of license file. Press \'P\' to view the previous license.\n"
#define VL10N_ULC_INF_NO_MORE_LIC_02_STR    "Reached the beginning of license file. Press \'N\' to view the next license.\n"

#define VL10N_ULC_PRM_SUB_MENU_STR          "Select Option (N/P/G/R/Q) :"
#define VL10N_ULC_LBL_LICENSE_STR           "License Code: \"%s\"\n"

#define VL10N_ULC_HDR_CAPACITY_LIC_STR        " Capacity"
#define VL10N_ULC_LBL_CAPACITY_TYPE_STR       " Capacity Type                : "
#define VL10N_ULC_INF_CAPACITY_POOLED_STR     "Pooled"
#define VL10N_ULC_LBL_LIC_CAPACITY_STR      "\n License Capacity             : "
#define VL10N_ULC_INF_INFINITE_STR          " Infinite\n"
#define VL10N_ULC_INF_CAPACITY_NON_POOLED_STR   "Non-Pooled"
#define VL10N_ULC_LBL_TOKEN_CAPACITY_STR    "\n Token Capacity               : "

#define VL10N_ULC_ERR_LIC_NOT_UPGRADABLE_STR "Error: License is not upgradable\n"
#define VL10N_ULC_ERR_DECODE_FAILED_STR      "Error %d: Unable to decode license\n"
#define VL10N_ULC_ERR_LIC_NOT_FOUND_STR      "Error: License is not found\n"

#define VL10N_ULC_LBL_UPGRADE_CODE_STR      "Upgrade code : "
#define VL10N_ULC_PRM_SAVE_UPGRADE_CODE_STR "Would you like to save this upgrade code to a file (y/n)?"
#define VL10N_ULC_INF_SAVE_UPGRADE_CODE_STR "Upgrade code saved to file \"%s\"\n"

#define VL10N_ULC_PRM_LICENSE_NUM_STR       "License number [1..%d] : "

#define VL10N_ULC_PRM_FEATURE_NAME_STR      "Feature name : "
#define VL10N_ULC_PRM_FEATURE_VERSION_STR   "Feature version : "
#define VL10N_ULC_PRM_CAPACITY_STR          "Capacity : "

#define VL10N_ULC_ERR_LICENSE_NOT_FOUND_STR "License with specified details not found.\n"

#define VL10N_ULC_ERR_GENERATION_FAIL_STR   "Error %d: Unable to generate upgrade code\n"
/*** End */

/* Strings For License Revocation */
#define VL10N_VLS_REVOKE_ERR_NO_FEATURE_STR "License with given feature/version is either not available on the server or belongs to a different vendor.\n"
#define VL10N_VLS_REVOKE_ERR_CORRUPT_MESSAGE_STR "The client message received by the server was corrupted.\n"
#define VL10N_VLS_REVOKE_ERR_OUT_VALID_RANGE_STR "The received number Of licenses to revoke is out of range.\n"
#define VL10N_VLS_REVOKE_ERR_MD5_PLUGIN_LOAD_FAIL_STR "Error loading the MD5 plugin dll at the server.\n"
#define VL10N_VLS_REVOKE_ERR_MD5_PLUGIN_EXEC_FAIL_STR "Error in executing the authentication plugin.\n"
#define VL10N_VLS_REVOKE_ERR_INSUFFICIENT_FEATURE_LICENSES_STR "This feature has less number of total licenses.\n"
#define VL10N_VLS_REVOKE_ERR_INSUFFICIENT_DEFAULT_GROUP_STR "Default group does not has sufficient licenses, reconfigure your user reservation file.\n"
#define VL10N_VLS_REVOKE_ERR_INSUFFICIENT_FREE_IN_DEFAULT_STR "Currently required number of licenses are not free for revocation in the default group.\n"
#define VL10N_VLS_REVOKE_ERR_INVALID_SESSION_ID_STR "Invalid session id received from the client.\n"
#define VL10N_VLS_REVOKE_ERR_INVALID_PASSWORD_STR "Invalid password for revocation.\n"
#define VL10N_VLS_REVOKE_ERR_INTERNAL_SERVER_STR "Revocation failed due to internal server error.\n"
#define VL10N_VLS_REVOKE_ERR_INFINITE_GRP_DIST_STR "Infinite revocation not possible with enabled group distribution.\n"
#define VL10N_VLS_REVOKE_ERR_INFINITE_LIC_IN_USE_STR "All licenses must be free for infinite revocation.\n"
#define VL10N_VLS_REVOKE_ERR_INFINITE_LIC_FINITE_REQ_STR "License has infinite keys. Only infinite license revocation request is allowed for this license.\n"
#define VL10N_VLS_REVOKE_ERR_TICKET_GENERATION_STR "Revocation ticket generation failed.\n"
#define VL10N_VLS_REVOKE_ERR_CODGEN_VERSION_UNSUPPORTED_STR "Revocation feature is not supported for the given license version.\n"
#define VL10N_VLS_REVOKE_ERR_RDNT_LIC_UNSUPPORED_STR "Revocation feature is not supported for redundant licenses.\n"
#define VL10N_VLS_REVOKE_ERR_CAPACITY_LIC_UNSUPPORED_STR "Revocation feature is not supported for capacity licenses.\n"
#define VL10N_VLS_REVOKE_ERR_UNEXPECTED_AUTH_CHLG_PKT_STR "Failure occurred due to unexpected challenge packet received from server.\n"
#define VL10N_VLS_REVOKE_ERR_TRIAL_LIC_UNSUPPORED_STR "Revocation not supported for trial licenses.\n"
#endif /* !_SFNT_CISCO_IOS_ */
#define VL10N_VLS_NOT_SUPPORTED_IN_NONET_MODE_STR "The feature is not supported in no-net mode.\n"

/* Local Request Locking Criteria */
#define VL10N_VLS_REQUIRED_LOCK_FIELDS_NOT_FOUND "Required locking criteria for local request is not available on your machine.\n"
#define VL10N_VLS_NOT_ENOUGH_LOCK_FIELDS         "Minimum number of locking criteria for local request is not found.\n"

/* Remote checkout is not allowed for perpetual licenses*/
#define VL10N_VLS_VLS_REMOTE_CHECKOUT_NOT_ALLOWED_FOR_PERPETUAL "Remote checkout is not allowed for repository license.\n"

/* Added in LM 8.1 */
#define VL10N_VLS_LIBRARY_ALREADY_INITIALIZED "Client library is already in initialized state.\n"
#define VL10N_VLS_INSTALL_STORE_NOT_SET      "No install license store is registered with the server.\n"
#define VL10N_VLS_RESOURCE_LOCK_FAILURE      "Fail to acquire resource lock.\n"
#define VL10N_VLS_NO_SUCH_LICENSE            "No license found with the specified feature/version/hash.\n"
#define VL10N_VLS_LICENSE_IN_USE             "Specified feature/license has active client(s).\n"
#define VL10N_VLS_SET_LICENSE_PRECEDENCE_FAILED    "Failure in setting precedence for specified trial license.\n"
#define VL10N_VLS_NO_SUCH_LICENSE_STORE      "Specified license store doesn't exists.\n"
#define VL10N_VLS_STORE_ACCESS_ERROR         "Failure in accessing the license or persistence store.\n"
#define VL10N_VLS_STORE_DATA_INCONSISTENT    "The specified store is corrupted.\n"
#define VL10N_VLS_LICENSE_STORE_FULL         "License store is full.\n"
#define VL10N_VLS_PERSISTENCE_STORE_FULL     "Persistence store is full.\n"
#define VL10N_VLS_LICENSE_STORE_QUERY_FAILED "Failure in querying license store information.\n"
#define VL10N_VLS_CALL_SEQUENCE_ERROR        "Sequence of API call is incorrect.\n"
#define VL10N_VLS_RECORD_NOT_FOUND           "Record doesn't exists in the store.\n"
#define VL10N_VLS_STORE_SIZE_TOO_SMALL       "The size specified for the store is too small.\n"
#define VL10N_VLS_LIBRARY_NOT_INITIALIZED    "Client library not in initialized state.\n"
#define VL10N_VLS_STORE_OPEN_ERROR           "Failure in creating/opening the store.\n"
#define VL10N_VLS_CODE_GENERATOR_LIBRARY_FAILED   "Failure in initializing code generator library.\n"
#define VL10N_VLS_PERSISTENCE_CONFIGURATION_ERROR "Error in specifying Persistence configuration data.\n"
#define VL10N_VLS_NONET_LIBRARY               "A network request is made to the standalone library.\n"
#define VL10N_VLS_STORE_ALREADY_EXISTS        "A store with specified name already exists.\n"
#define VL10N_VLS_RECORD_CORRUPT              "Specified record in License/Trial/Revocation store is corrupt.\n"

#define VL10N_VLS_CONFIGURATION_NOT_ALLOWED  "Specified configuration is not allowed for this device.\n"
#define VL10N_VLS_EXCEEDS_MAX_SIZE           "Value exceeds the maximum size allowed.\n"
#define VL10N_VLS_BACKUP_CONFIGURATION_ERROR "Incorrect backup information is specified for the specified store type.\n"
#define VL10N_VLS_VALUE_OUT_OF_RANGE         "Value is out of the valid range.\n"
#define VL10N_VLS_LICENSE_RECORD_EMPTY       "Specified record in license store is empty.\n"
#define VL10N_VLS_SAVE_LICENSE_FILE_WRITE_ERROR       "Error in writing to specified backup file\n"
#define VL10N_VLS_SAVE_LICENSE_FILE_ALREADY_EXISTS    "Specified backup file already exists.\n"
#define VL10N_VLS_LICENSE_NOT_LOCKED                  "License is not client locked.\n"
#define VL10N_VLS_LICENSE_NOT_EXPIRED_AND_HAS_VALID_LOCK_CODE "License has not expired and has valid lock code.\n"
#define VL10N_VLS_LOCK_CODE_MISMATCH          "Specified lock code mismatches with the one provided in license string.\n"
#define VL10N_VLS_HANDLER_ALREADY_REGISTERED "Handler function is already registered.\n"
#define VL10N_VLS_NON_TRIAL_LICENSE          "Active feature on the server is not a trial license.\n"
#define VL10N_VLS_ADD_LIC_CANCELLED_BY_USER  "License addition cancelled by policy callback.\n"
#define VL10N_VLS_LICENSE_ALREADY_REVOKED    "Given license has already been revoked.\n"
#define VL10N_VLS_LICENSE_START_DATE_NOT_REACHED  "License start date not yet reached.\n"
#define VL10N_VLS_DEFERRED_REVOCATION_SUCCESS     "Deferred Revocation operation successful.\n"
#define VL10N_VLS_REHOST_UNSUPPORTED_PT_VERSION   "Old PT's (less than 853 ) are not supported in new NR.\n"

#define VL10N_VLS_LOCK_SELECTOR_INVALID      "Specified lock selector is not valid.\n"
#define VL10N_VLS_LOCK_CODE_NOT_SUPPORTED    "The specified lock code is not supported by the library/server.\n"
#define VL10N_VLS_LOCK_CODE_VER_INVALID      "Invalid lock code version.\n"
#define VL10N_VLS_LOCK_CODE_INVALID          "The specified lock code is invalid.\n"
#define VL10N_VLS_NO_AVAILABLE_MACHINE_ID    "No available machine id for specified lock selector.\n"

/* Rehost related error messages */
/* buffer too small */
#define V_MSG_VLS_REHOST_BUFFER_TOO_SMALL            "Insufficient buffer size.\n"

/* buffer too small, it should not happen */
#define V_MSG_VLS_REHOST_BUFFER_TOO_SMALL_UNEXPECTED "Insufficient buffer size.\n"

/* parameters error */
#define V_MSG_VLS_REHOST_PARAMETERS_ERROR            "Invalid parameter value.\n"

/* algorithm not supported */
#define V_MSG_VLS_REHOST_UNSUPPORTED_ALGO            "Algorithm not supported.\n"

/* invalid tlv data format */
#define V_MSG_VLS_REHOST_INVALID_DATA_FORMAT         "Invalid TLV data format.\n"

/* invalid rehost request data */
#define V_MSG_VLS_REHOST_INVALID_REQUEST_DATA        "Invalid rehost request data.\n"

/* operation type not supported */
#define V_MSG_VLS_REHOST_UNSUPPORTED_OPERATION_TYPE  "Operation type not supported.\n"

/* memory allocation failure */
#define V_MSG_VLS_REHOST_ALLOCATE_MEMORY_FAILURE     "Memory allocation failure.\n"

/* tag can not be found in tlv */
#define V_MSG_VLS_REHOST_TAG_NOT_FOUND               "Required tag is not found in tlv data.\n"

/* lock info is not matching */
#define V_MSG_VLS_REHOST_DIFFERENT_LOCK_INFO          "lock info is not matching.\n"

/* the license is used, can not be rovoked */
#define V_MSG_VLS_REHOST_LICENSE_IN_USE               "License being revoked is in use.\n"

/* it should not happen, something unexpected */
#define V_MSG_VLS_REHOST_UNEXPECTED_ERROR             "Unexpected error.\n"

/* the license is already revoked */
#define V_MSG_VLS_REHOST_HAVE_BEEN_REVOKED_BEFORE     "License has been revoked earlier.\n"

/* revocation requested over the capacity of license */
#define V_MSG_VLS_REHOST_REVOKE_OVER_TOTAL            "Revocation requested on capacity of license.\n"

/* license already exist */
#define V_MSG_VLS_REHOST_LICENSE_EXIST                "License already exists.\n"

/* was canceled by callback */
#define V_MSG_VLS_REHOST_CANCELED_BY_USER             "Operation cancelled by user.\n"

/* rehost status is not defined */
#define V_MSG_VLS_REHOST_STATUS_NOT_DEFINED           "Status not defined.\n"

#define V_MSG_VLS_REHOST_LIC_VERSION_NOT_SUPPORTED    "Operation is not supported for given license version.\n"

#define V_MSG_VLS_LICENSE_DELETION_NOT_ALLOWED        "License deletion is not supported for this license.\n"



/* Strings for initialization APIs */
#define VL10N_INIT_DEMO_HDR_STR              "\n  %s %s Initialization Demo Program\n"
#define VL10N_INIT_DEMO_PRM_STR              " Initialize Standalone or Network Information [s/n]   : "
#define VL10N_ERR_KEY_INFO_SUCCESS_STR       " System initialization successfully done.\n"
#define VL10N_ERR_INVALID_INPUT_STR          " Failed to launch the function. Check the calling parameters.\n"
#define VL10N_ERR_LIBRARY_INIT_STR           " Library initialization failure.\n"
#define VL10N_ERR_INVALID_SERIALNUM_STR      " Invalid serial number. Check the calling parameters.\n"
#define VL10N_ERR_KEY_INFO_EXISTS_STR         " The registry information exists on system. No updates will be done.\n"
#define VL10N_ERR_INIT_INFO_EXISTS_STR       " Failed to initialize the system. The function has already been executed on the target system.\n"
#define VL10N_ERR_INIT_SEC_FAIL_STR          " Failed to initialize the secuirty attributes.\n"
#define VL10N_ERR_INIT_KEY_FAIL_STR          " Failed to create security keys in registry.\n"
#define VL10N_ERR_INIT_KEY_SEC_FAIL_STR      " Failed to set security attributes on key.\n"
#define VL10N_ERR_INIT_OPEN_KEY_FAIL_STR     " Failed to open secured information registry key.\n"
#define VL10N_ERR_INIT_SET_VALUE_FAIL_STR    " Failed to store the time information in the registry key.\n"
#define VL10N_ERR_INIT_QUERY_FAIL_STR         " Failed to query the registry key value.\n"
#define VL10N_ERR_INIT_FILE_FAIL_STR         " Failed to create the initial secured files on system.\n"
#define VL10N_ERR_INIT_FILE_OPEN_FAIL_STR    " Failed to open the secured files on system.\n"
#define VL10N_ERR_INIT_FILE_WRITE_FAIL_STR   " Failed to store the secured information in the file.\n"
#define VL10N_ERR_INIT_FILE_SEC_FAIL_STR     " Failed to set the security attributes on file.\n"
#define VL10N_ERR_EXCEPTION_OCCURRED_STR      " Exception error occurred during the function call.\n"
#define VL10N_ERR_INIT_FILE_INFO_EXISTS_STR  " Failed to udpate time information in file. The initial file already exists on system.\n"
#define VL10N_ERR_INTERNAL_FAIL_STR          " Internal error occurred during the function call.\n"
#define VL10N_ERR_REVOKEINIT_FAIL_STR      " License revocation related initialization failed.\n"
#define VL10N_NON_ADMIN_USER_STR             " Administrative rights required to start the Sentinel RMS License Manager."
#define VL10N_NON_ADMIN_USER_STR_APP         " Administrative rights required to run this utility/application."
#define VL10N_ERR_NET_FAIL_STR               " Network Error occurred during the function call. \n"
#define VL10N_ERR_TEMPER_DETECTED_STR        " Tamper Detected. \n"
#define VL10N_ERR_INSUFFICIENT_PERMISIONS_STR " Insufficient permissions error. Only Administrator can run this application. \n"
#define VL10N_ERR_SUCCESS_STR            " Initialization Successfull! \n"
#define VL10N_ERR_ALLREADY_INITIALIZED_STR   " System allready initialized! \n"
#define VL10N_ERR_INVALID_INIT_MODE_STR      " Invalid initialization mode! \n"
#define VL10N_ERR_INIT_PATH_FAILED_STR       " Failed to open persistence file path. \n"
#define VL10N_ERR_INIT_FILE_READ_FAIL_STR    " Failed to read secure information from file. \n"
#define VL10N_ERR_LOCK_ERROR_STR             " Failed to get lock. \n"
#define VL10N_ERR_INIT_CONFIG_FAIL_STR       " Failed to set configuration object. \n"


#ifndef _SFNT_CISCO_IOS_

#define VL10N_UNABLE_DBCONV_UTILITY     "Source File does not contain any information.\n"
#define VL10N_DBCONV_UTILITY            "Administrative rights required to run Database Conversion Utility.\n"
#define VL10N_DBCONV_OPT                " Choose from above options   "
#define VL10N_DBCONV_NET_TRIAL_PRS      " 1. Net Trial Persistence \n"
#define VL10N_DBCONV_NET_COMM_PRS       " 2. Net Commuter Persistence  \n"
#define VL10N_DBCONV_FORMAT_STR         "\t\t*************************************\n"
#define VL10N_DBCONV_UTIL               "\t\t  DATABASE CONVERSION UTILITY\n"
#define VL10N_DBCONV_FAILED             "\t\tDatabase Conversion Failed \n"
#define VL10N_DBCONV_LOC_SOURCE_DIR    " Enter Location of Source directory :"
#define VL10N_DBCONV_LOC_SOURCE_FILE    " Enter Location of Source file :"
#define VL10N_DBCONV_LOC_DEST_FILE      " Enter Location of Destination file :"
#define VL10N_DBCONV_NUM_SEC_APP        "The Number of Record Converted: %d\n"
#define VL10N_DBCONV_SRC_NOT_EXIST      "Source File does not exists\n"




/* Init utility -- lsinit and lsgeninit */
#define VL10N_INIT_HDR_LSINIT_STR            "\n  %s %s Feature Installation\n"
#define VL10N_INIT_HDR_LSGENINIT_STR         "\n  %s %s Feature Install Generator\n"

#define VL10N_INIT_ERR_FEATURE_SUCCESS       " Successfully initialized record %d.\n"
#define VL10N_INIT_ERR_FEATURE_FAIL          " Record %d returned error code 0x%8.8X.\n"
#define VL10N_INIT_ERR_LSINIT_CORRUPT        " Error! The file appears to be corrupted. Unable to install.\n"
#define VL10N_INIT_ERR_EXTRACT_SUCCESS       " Successfully extracted %d record(s).\n"

#define VL10N_INIT_ERR_LSGENINIT_SUCCESS           " Successfully loaded configuration file '%s'.\n"
#define VL10N_INIT_ERR_LSGENINIT_FILENOTFOUND      " Unable to open configuration file '%s'.\n"
#define VL10N_INIT_ERR_LSGENINIT_READ              " Unable to read configuration file '%s'.\n"
#define VL10N_INIT_ERR_LSGENINIT_BLANK             " Configuration file '%s' does not contain any usable data.\n"
#define VL10N_INIT_ERR_LSGENINIT_NOTGIVEN          " No filename was given for configuration file.\n"
#define VL10N_INIT_ERR_LSGENINIT_TOOMANYFIELDS     " Configuration file record #%d has too many fields.\n"
#define VL10N_INIT_ERR_LSGENINIT_NOTENOUGHFIELDS   " Configuration file record #%d does not have all required fields.\n The GUID and feature name fields are required.\n"
#define VL10N_INIT_ERR_LSGENINIT_INVALIDGUID       " Configuration file record #%d has an invalid GUID entry.\n A example of valid GUID entry is '0138BDDD-5BA1-40b1-A6DE-E452FEBFFB16'.\n"
#define VL10N_INIT_ERR_LSGENINIT_INVALIDFEATURE    " Configuration file record #%d has an invalid feature name.\n A valid feature name has 24 or fewer ASCII characters.\n"
#define VL10N_INIT_ERR_LSGENINIT_INVALIDVERSION    " Configuration file record #%d has an invalid version.\n A valid version has 11 or fewer ASCII characters.\n"
#define VL10N_INIT_ERR_LSGENINIT_TOOMANYRECORDS    " Configuration file '%s' has too many records listed.\n Up to %d records are allowed.\n"
#define VL10N_INIT_ERR_LSGENINIT_DUPRECORDS        " Duplicate Record detected. All records must be unique.\n Record #%d and #%d both have same value '%s'.\n"
#define VL10N_INIT_ERR_LSGENINIT_DUPFEATUREVERSION " Duplicate feature name & version detected.\n Record #%d and #%d are both '%s %s'.\n"
#define VL10N_INIT_ERR_LSGENINIT_DEFAULT           " An unexpected error has occurred.\n"

#define VL10N_INIT_INF_LSGENINIT_RANDOMGUID        " Random GUID for record %d, feature %s %s:\n   %s\n"

#define VL10N_INIT_ERR_GENLSINIT_SUCCESS           " Successfully generated installer file '%s'.\n"
#define VL10N_INIT_ERR_GENLSINIT_STUBNOTFOUND      " Unable to open stub installer file '%s'.\n"
#define VL10N_INIT_ERR_GENLSINIT_READ              " Unable to read stub installer file '%s'.\n"
#define VL10N_INIT_ERR_GENLSINIT_WRITE             " Unable to write generated installer file '%s'.\n"
#define VL10N_INIT_ERR_GENLSINIT_NOMEMORY          " Unable to acquire enough memory to generate installer file.\n"
#define VL10N_INIT_ERR_GENLSINIT_INVALIDSTUB       " The stub installer file '%s' is invalid.\n"

/* For vendor isolation */
#define VL10N_SRV_ERR_VENDOR_ISOLATION_STR         "The Sentinel RMS License Manager failed to start, either vendor identifier not set or the server has been built with incorrect server library.\n"

#define VL10N_INIT_INF_LSGENINIT_USAGE    "\n\
Usage: %s <filename> [-np] \n\
\n\
Where <filename> is a configuration file listing the features to install.\n\
\n\
Configuration file can list up to %d records, one record to a line.\n\
Each line in the format: '<GUID> <FeatureName> [Version] # comments'\n\
GUID and Featurename are required, the version field is optional.\n\
Each feature & version should be listed only once. Specify '%s' \n\
keyword in the configuration file to have a GUID automatically \n\
generated for any feature.\n\
[-np] option to execute the tool without pause. \n\
\n\
Example:\n\
# My configuration file:\n\
355A0C6B-1806-474f-BA31-EA22E8E0AE7B RMSDeveloperKit  8.0 # Main feature\n\
%-36s HOOKDEMO    1.2 \n\
518645BB-845B-488b-BCC6-8388FE44F46C 99              # demo feature\n\
etc.\n"
#endif /* !_SFNT_CISCO_IOS_ */

/* MAC Address */
#define VL10N_ERR_MAC_ADDR           "\n Incorrect MAC Address "

/* GETOPT error string _begin*/
#define VL10N_GETOPT_ERR_OPTLTR_EXPECTED_STR            " : option letter expected"
#define VL10N_GETOPT_ERR_ILLEAGAL_OPT_STR               " : illegal option -- "
#define VL10N_GETOPT_ERR_OPT_ARGS_STR                   " : option requires an argument -- "
#define VL10N_GETOPT_ERR_OPT_VAL_RESRV_OPT_STR          " : option value is equal to reserved option names -- "
/* GETOPT error string _end*/

#endif  /* _VINTLSTR_H_ */

