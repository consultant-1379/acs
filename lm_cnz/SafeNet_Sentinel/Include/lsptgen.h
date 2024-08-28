/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

/*H****************************************************************
* FILENAME    : lsptgen.h
*
* DESCRIPTION :
*     PUBLIC API.
*     This file contains public types, defines and prototypes to be used by
*     applications using the separate permission ticket generation library
*     and verification of revocation ticket. 
*	  
*     Contains functions to check dependencies between user-provided
*     licensing parameters, and functions to test validity of data
*     entered by the user.  Hence, all checking functions accept the
*     data as strings (char*), and convert into integers internally.
*
*  
*
*H*/


#ifndef _LSPTGEN_H_
#define _LSPTGEN_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "lserv.h"

#define VLScg_REHOST_OPERATION_ADD              'A'
#define VLScg_REHOST_OPERATION_REVOKE_FULL      'R'
#define VLScg_REHOST_OPERATION_REVOKE_PARTIAL   'P'      //Applicable for Network Revoke only

/* Error codes */   

/* version value found in revocation ticket does not
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
/* buffer too small */
#define VLScg_RT_BUFFER_TOO_SMALL                   102
/* parameters error */
#define VLScg_RT_PARAMETERS_ERROR                   103
/* memory allocation failure */
#define VLScg_RT_ALLOCATE_MEMORY_FAILURE            104
/* operation type not supported */
#define VLScg_RT_UNSUPPORTED_OPERATION_TYPE         105
/* invalid rehost request data */
#define VLScg_RT_INVALID_REQUEST_DATA               106
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
/* custom defined data tag is not found in revocation ticket */
#define VLScg_RT_CUSTOM_DATA_TAG_MISSING            114
/* custom defined data value found in revocation ticket does not
   match with that specified in request structure / String */
#define VLScg_RT_CUSTOM_DATA_MISMATCH               115
/* Either standalone revoke request is provided to verify with network RT, or network revoke
request is provided to verify with standalone RT */
#define VLScg_RT_REVOCATION_TYPE_MISMATCH           116
/* Request structure has more operations for single PT */
#define VLScg_TOO_MANY_OPERATIONS_FOR_SINGLE_PT     117
/* Different vendor ID license found in request */
#define VLScg_VENDOR_ID_MISMATCH                    118
/* PT generation attaemped for codegen version < 11 licenses*/
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

/* 8.5.3 - Max limit for revoke grace days (deferred revoke)*/
#define VLScg_MAX_REVOKE_GRACE_DAYS       30

//8.5.5 - To generate PT for redundant licenses
#define MAX_REDUNDANT_SERVERS_IN_PT 11

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
      unsigned char * pucTransactionId;
      unsigned long ulLockCodeSelectorArr[MAX_REDUNDANT_SERVERS_IN_PT];
      unsigned char * pucLockInfoArr[MAX_REDUNDANT_SERVERS_IN_PT];
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
      unsigned long          ulNumberOfLicensesRevoked; /* number of licenes revoked */
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
   
   /* generate permission ticket by request */
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSgeneratePermissionTicket(
#ifndef LSNOPROTO
      PVPT_REQUEST           pvRequest,                    /* IN - request data */
      unsigned char        * pucPermissionTicket,          /* OUT the generated permission ticket*/
      unsigned int       * pui16PermissionTicketLength   /* IN/OUT - permission ticket length*/
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
      unsigned char        * pucRevocationTicket,          /* IN - revocation ticket */
      unsigned long          ulRevocationTicketLength,     /* IN - revocation ticket length */
      PVRT_VERIFY_ERRORS     pvErrorInfo,                  /* OUT - error info */
      unsigned long        * pulErrorInfoTotalLength       /* IN/OUT - error line total length */
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
      unsigned long        ulErrorInfoTotalLength;     /* IN/OUT - error line total length - would need to be allocated by the user in case of error.*/
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
      unsigned long                      ulNumRevocationTickets,      /*IN - Number of revocation tickets that are being verified */
      PVRT_VERIFY_INFO_REDUNDANT_LIC     pVerifyInfo,                 /*IN/OUT – array of  verify Info structs, memory allocation user's responsibility */
      unsigned long                      *pulNumRTsAbsent,            /* OUT - number of RTs that were missing as against the PT passed, also reflects the size of the pLockInfoOfAbsentRTs array*/
      PVRT_LOCK_INFO                     pLockInfoOfAbsentRTs,        /* OUT - Lock Info array, to provide details of missing RTs; memory allocation, user’s responsibility. Max possible array size is MAX_REDUNDANT_SERVERS_IN_PT. The intention of this parameter is to help the ISV know the details of the machine for which RT(s) is/are missing.*/
      void                               *unused
#endif /* LSNOPROTO */
      );      

#ifdef __cplusplus
}
#endif


#endif /* _LSPTGEN_H_ */