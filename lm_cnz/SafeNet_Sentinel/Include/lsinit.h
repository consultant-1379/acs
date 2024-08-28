/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

#ifndef _LSINIT_H_
#define _LSINIT_H_

/*H****************************************************************
* FILENAME    : lsinit.h
*
* DESCRIPTION :
*           Contains initialization function prototypes, and error codes
*           needed for setting up the system for using Sentinel RMS
*           Development Kit application.
*
*H*/

#ifdef __cplusplus
extern "C"
{
#endif

   /*------------------------------------------------------------------------*/
   /* Error codes                                      */
   /*------------------------------------------------------------------------*/
/* LSINIT returns success */
#define ERR_KEY_INFO_SUCCESS        0x00000000

/* Parameter passed to LSINIT api are not valid */
#define ERR_INVALID_INPUT           0x00000001

/* Library is not modbined */
#define ERR_INIT_LIB_FAIL           0x00000002

/* Library is not modbined */
#define ERR_NO_PERSISTENCE_DEVICE   0x00000004

/* Initilization already done */
#define ERR_INIT_INFO_EXISTS        0x00000008

/* Initilization of security descriptor */
#define ERR_INIT_SEC_FAIL           0x00000010

/* Error creating registry key */
#define ERR_INIT_KEY_FAIL           0x00000020

/* Error setting registry security for key */
#define ERR_INIT_KEY_SEC_FAIL       0x00000040

/* Error openning registry key */
#define ERR_INIT_OPEN_KEY_FAIL      0x00000080

#define ERR_INIT_SET_VALUE_FAIL     0x00000100

#define ERR_INIT_QUERY_FAIL         0x00000200

/* Error creating file or setting security  */
#define ERR_INIT_FILE_FAIL          0x00000400

/* Error opening file */
#define ERR_INIT_FILE_OPEN_FAIL     0x00000800

/* Error writting into file */
#define ERR_INIT_FILE_WRITE_FAIL    0x00001000

/* Error setting security of file */
#define ERR_INIT_FILE_SEC_FAIL      0x00002000

/* Exception occured */
#define ERR_EXCEPTION_OCCURED       0x00004000
#define ERR_EXCEPTION_OCCURRED      ERR_EXCEPTION_OCCURED

/* File already exists */
#define ERR_INIT_FILE_INFO_EXISTS   0x00008000

#define ERR_INTERNAL_FAIL           0x00010000

/* User dont have sufficient permission to do job */
#define ERR_INSUFFICIENT_PERMISIONS 0x00020000

/* Used in Unix */
#define ERR_TAMPER_DETECTED         0x00040000

/* Error retrieveing persistence database location */
#define ERR_INIT_PATH_FAILED        0x00080000

/* Error reading file */
#define ERR_INIT_FILE_READ_FAIL     0x00100000

#define ERR_NORESOURCES             0x00200000

/* Not able to get lock */
#define ERR_LOCK_ERROR              0x00400000

/* Error setting trial persistence file location */
#define ERR_INIT_CONFIG_FAIL        0x00800000


   /* error code to recognize specific initialize failure */
#define ERR_TIME_TAMPER_INIT_FAIL   0x10000000
#define ERR_COMMUTER_PRS_INIT_FAIL  0x20000000
#define ERR_GRACE_PRS_INIT_FAIL     0x40000000
#define ERR_TRIAL_INIT_FAIL         0x80000000

#define ERR_STDRVK_INIT_FAIL        0xF0000000    /* error code added for standalone revocation in GA*/

#define ERR_CONSUME_INIT_FAIL       0x01000000

#define ERR_USG_LOG_INIT_FAIL       0x02000000
#define ERR_USG_GUID_INIT_FAIL      0x03000000
#define ERR_CANCEL_LEASE_INIT_FAIL  0x04000000
#define ERR_ROLLOVER_COUNT_INIT_FAIL  0x05000000
#define ERR_COMMON_USG_LOG_INIT_FAIL  0x06000000

   /*------------------------------------------------------------------------*/
   /* Function Prototypes                                                    */
   /*------------------------------------------------------------------------*/

   /* Function to initialize the system for Standalone licensing */
   /* Call this function N number of times where N = number of features */

   int sntlInitStandaloneSystem(char *GUID,
                                char *featN,
                                char *featV);


#ifdef __cplusplus
}
#endif
#endif /* _LSINIT_H_ */

