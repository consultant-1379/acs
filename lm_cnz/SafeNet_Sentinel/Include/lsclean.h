/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

#ifndef _LSCLEAN_H_
#define _LSCLEAN_H_

/*H****************************************************************
* FILENAME    : lsclean.h
*
* DESCRIPTION :
*           Contains cleaning function prototypes, and error codes
*           needed for cleaning the RMS persistence database.
*
*H*/

#ifdef __cplusplus
extern "C"
{
#endif

#define VLS_TRIAL_STORE               1
#define VLS_COMMUTER_STORE            2
#define VLS_REVOKE_STORE              4
#define VLS_GRACE_STORE               5
#define VLS_VTL_STORE                 6
#define VLS_NETWORK_REVOCATION_STORE  7
#define VLS_TIME_TAMPER_STORE         8
#define VLS_USAGE_LOG_STORE           9
#define VLS_USAGE_GUID_STORE          10
#define VLS_USAGE_LOG_NONET_STORE     11
#define VLS_CANCEL_LEASE_STORE        12
#define VLS_ROLLOVER_COUNT_STORE      13
#define VLS_USAGE_LOG_COMMON_STORE    14

#define VLS_VPR_COMMUTER_CLEAN            0 
#define VLS_LPR_COMMUTER_REPAIR           1
#define VLS_LPR_COMMUTER_CLEAN            2

typedef  unsigned long           LS_STATUS_CODE;

#define LIBINFOLEN  32

typedef struct
{
   unsigned long ulInfoCode;
   char          szVersion  [LIBINFOLEN];
   char          szProtocol [LIBINFOLEN];
   char          szPlatform [LIBINFOLEN];
   char          szLocale   [LIBINFOLEN];
   char          szUnused2  [LIBINFOLEN];
} LS_LIBVERSION;


LS_STATUS_CODE VLScleanNetworkPersistenceInfo(
    int    serverVersion,    /* IN */
    int    persistenceType,  /* IN */
    char*  featureName,      /* IN */
    char*  featureVersion,   /* IN */
    char*  hostName,         /* IN */
    char*  unused1,
    int    operationType     /* IN - for LPR Server side clean up/repair */
);

LS_STATUS_CODE VLScleanStandalonePersistenceInfo(
    int    clientLibVersion, /* IN */
    int    persistenceType,  /* IN */
    char*  featureName,      /* IN */
    char*  featureVersion,   /* IN */
    char*  unused1,
    int    unused2
);


LS_STATUS_CODE VLSgetLibInfo(
    LS_LIBVERSION *pInfo
);

/* Error codes */

#define VLScl_ILLEGAL_VENDOR_ID           0xC8010001
#define VLScl_INSUFFICIENT_PERMISIONS     0xC8010002
#define VLScl_INVALID_ARGUMENTS           0xC8010003
#define VLScl_NO_TRIAL_FEATURE_IN_USE     0xC8010004
#define VLScl_NO_COMMUTER_FEATURE_IN_USE  0xC8010005
#define VLScl_NO_GRACE_INFORMATION_FOUND  0xC8010006
#define VLScl_NO_REVOKE_INFORMATION_FOUND 0xC8010007
#define VLScl_NORESOURCES                 0xC8010008
#define VLScl_INTERNAL_ERROR              0xC8010009
#define VLScl_RESOURCE_LOCK_FAILURE       0xC8010010
#define VLScl_NO_COMMUTER_INFO_FOUND      0xC8010011
#define VLScl_NO_PERSISTENCE_DEVICE       0xC8010012
#define VLScl_DEVICE_NOT_FOUND            0xC8010013
#define VLScl_SUCCESS                     0x00000000


#ifdef __cplusplus
}
#endif
#endif  /* _LSCLEAN_H_ */
