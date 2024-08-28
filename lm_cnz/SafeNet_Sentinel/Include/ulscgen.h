/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

/*H****************************************************************
* FILENAME    : ulscgen.h
*
* DESCRIPTION :
*     PUBLIC API.
*     This file contains public types, defines and prototypes to be used by
*     applications using the upgrade-licensing mechanism of license code
*     generation library.
*
*     Contains functions to check dependencies between user-provided
*     licensing parameters, and functions to test validity of data
*     entered by the user.  Hence, all checking functions accept the
*     data as strings (char*), and convert into integers internally.
*
* USAGE       :
*
*      To license an application the first call should be to
*      VLSucgInitialize() API. This API does the following tasks :
*
*      a) It first checks for the max. limit of handles.
*
*      b) Allocates memory for internal data structures.
*
*      c) Initializes the error list and sets error count to
*         zero and sets the maximum severity of error to the lowest
*         possible value i.e. V_MSG
*
*  NOTE :
*         The handle is of type int and  provides an index
*         to our internal data structures which are further used
*         for error processing, etc.
*
*       The last call in an application should be to VLSucgCleanup()
*       API. In fact corresponding to every VLSucgInitialize() there
*       should be a VLSucgCleanup() call.  This API function performs the
*       following tasks :
*
*     a) It frees the memory allocated for internal data structures.
*
*
*     Typical sequence of calls would be:
*     First allocate (memory for) a ucodeT struct.
*     Call VLSucgReset on each new ucodeT struct, before filling
*     values into it.
*     Obtain input from the user.  Sequence of input is important.
*     Should call the VLSucgAllow functions to check feature-dependencies
*     between various codegen capabilities.
*     Call the VLSucgSet functions to test-and-set values given by
*     the user. These functions also record errors, which can be
*     printed out using API call VLSucgPrintError() after every VLSucgSet
*     call.
*     After all input is received, call VLSucgGenerateLicense()
*     which generates the license string.
*
*
*
* NOTES       :
*     Hook for vendor encryption is called while generating the license.
*     Never directly modify the codeT struct, always use VLSucgSet
*     functions.
*
*
*H*/
#define _ULSCGEN_UNIX_

#ifndef _ULSCGEN_H_
#define _ULSCGEN_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

/* Below time related defines taken from lserv.h as is and should be updated as and when modified */
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


#if defined(_VMSWIN_) || defined (WIN32)
#pragma pack(push, 1)
#endif /*_VMSWIN_*/

   typedef int VLSucg_HANDLE;


   /* Some standard return/error codes: */
#define VLSucg_SUCCESS                        0      /* Success */
#define VLSucg_NO_FEATURE_NAME                2
#define VLSucg_INVALID_INT_TYPE               3
#define VLSucg_EXCEEDS_MAX_VALUE              4
#define VLSucg_LESS_THAN_MIN_VALUE            5
#define VLSucg_EXCEEDS_MAX_STRLEN             6
#define VLSucg_NOT_MULTIPLE                   7
#define VLSucg_INVALID_HEX_TYPE               11
#define VLSucg_RESERV_STR_ERR                 14
#define VLSucg_INVALID_CHARS                  16
#define VLSucg_DECRYPT_FAIL                   20
#define VLSucg_INVALID_CHKSUM                 22
#define VLSucg_MALLOC_FAILURE                 26
#define VLSucg_INTERNAL_ERROR                 27
#define VLSucg_INVALID_INPUT                  30
#define VLSucg_MAX_LIMIT_CROSSED              31
#define VLSucg_NO_RESOURCES                   32
#define VLSucg_BAD_HANDLE                     33
#define VLSucg_FAIL                           34
#define VLSucg_INVALID_VENDOR_CODE            35
#define VLSucg_VENDOR_ENCRYPTION_FAIL         36
   /*
 * RMS Development Kit LicenseMeter related error codes.
    */
#define VLSucg_LICMETER_EXCEPTION     40
#define VLSucg_LICMETER_DECREMENT_OK    41
#define VLSucg_LICMETER_ACCESS_ERROR    42

#define VLSucg_LICMETER_CORRUPT      44
#define VLSucg_LICMETER_VERSION_MISMATCH   45
#define VLSucg_LICMETER_EMPTY      46

#define VLSucg_INVALID_LICTYPE      52
#define VLSucg_VI18N_INITIALIZE_FAIL    59

#define VLSucg_NO_CAPACITY_AUTHORIZATION   61
#define VLSucg_NO_UPGRADE_AUTHORIZATION    62

#define VLSucg_NO_UPGRADE_CODE                      63
#define VLSucg_INVALID_BASE_LIC_INFO                64
#define VLSucg_NON_CAPACITY_UPD_NOT_ALLOWED         65
#define VLSucg_INVALID_UPGRADE_CODE                 66
#define VLSucg_LICMETER_COUNTER_TOOLOW              67
#define VLSucg_POOLED_CAPACITY_UPD_NOT_ALLOWED      68
#define VLSucg_LICMETER_NOT_SUPPORTED               69

#define VLSucg_ENCRYPTION_FAIL                        72

#define VLSucg_INVALID_VENDOR_INFO                   125 
   /*
 * RMS Development Kit LicenseMeter related constants.
   */
#define VLSucg_LICMETER_UNITS_INFINITE      (long) (-1)
#define VLSucg_LICMETER_UNITS_UNAVAILABLE   (long) (-2)
#define VLSucg_TRIALMETER_UNITS_UNAVAILABLE (long) (-3)



#define  VLSucg_INVALID_HANDLE    ((VLSucg_HANDLE) (-1))
#define  VLSucg_MAX_CODE_COMP_LEN           128
#define  VLSucg_MAX_LOCK_INFO_LEN   16   /* chars */
#define  VLSucg_INFINITE_CAPACITY   0xffffffff
#define  VLSucg_MAX_NUM_HANDLES    1000



#define VLSucg_UPGRADE_VERSION     1
#define VLSucg_UPGRADE_CAPACITY    2
#define VLSucg_UPGRADE_ALL         3



   /* int standalone_flag : */
#define VLSucg_NETWORK        0
#define VLSucg_STANDALONE     1

   /******** Special strings accepted by certain VLSucgSet functions: ********/
#define VLSucg_NOLIMIT_STRING  "NOLIMIT"
#define VLSucg_NEVER_STRING   "NEVER"

   /* unsigned num_keys, soft_limit */
#define VLSucg_INFINITE_KEYS        VLS_INFINITE_KEYS

#define VLSucg_INFINITE_YEARS  2500

#define  DEFAULT_BASE_FEATURE_VERSION  ""

#define VLSucg_CAPACITY_UNITS_MAX_VALUE  4
#define VLSucg_CAPACITY_UNITS_MIN_VALUE  0


   typedef struct
   {
      long   structSz;         /* Size of the structure */
      unsigned int vendor_code;      /* Internal use          */
      unsigned int version_num;     /* upgrade license code generation library
                                                               version */

      /* Feature/Version of the base license that needs to be upgraded */
      char   base_feature_name[VLSucg_MAX_CODE_COMP_LEN+1];
      char   base_feature_version[VLSucg_MAX_CODE_COMP_LEN+1];

      char   base_lock_code[VLSucg_MAX_CODE_COMP_LEN+1];
      /* Stores information in ascii                  */
      /* +1 needed for null termination               */

      /* The following two values would automatically be set during the license generation time. */
      
      Time_T generation_time; /* in GMT. To know as to which license was generated when */
      unsigned long generation_sequence; /* To ensure that on a fast system, even if two licenses are generated at the same time, this value should be different. This can be picked up either from the registry or incremental in an instance */

      /* Bit wise flag. Will control what will be updated */
      unsigned long upd_flags;
      /* VLSucg_UPGRADE_VERSION, VLSucg_UPGRADE_CAPACITY */
      char   upd_version[VLSucg_MAX_CODE_COMP_LEN+1];
      /* New version for this feature*/
      int    capacity_units;
      /* Flag which determines capacity least count   */
      unsigned long capacity_increment ;
      unsigned long unused1;
      unsigned long unused2;
   }
   ucodeT;


   /******************************************************************
    * DESCRIPTION :
    * Initialize the handle, which may be useful in multithreaded
    * systems. Initialize the decrypt keys also.
    *
    * RETURN VALUES :
    * returns 0 on successful return.
    * returns VLSucg_MAX_LIMIT_CROSSED on failure.
    *
    * On failure returns, the handle returned MAY be valid and contain error
    * messages. Caller can check if the handle is (equal to)
    * VLSucg_INVALID_HANDLE, and if not, can use the VLSucgXXXError() routines
    * to aquire the error messages from the handle. In this case the caller
    * should call VLSucgCleanup() to free the resources associated with the handle.
    *
    * NOTES :
    */

   int VLSucgInitialize
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE * handle   /* Instance handle for this library
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

   int VLSucgCleanup
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE *handle
#endif
   );

#undef _ULSCGEN_UNIX_

   /******************************************************************
    * DESCRIPTION :
    * Resets the ucodeP. It must be called before calling VLSucgSet
    * functions.
    *
    * RETURN VALUES :
    * returns 0 on successful return.
    *
    * NOTES :
    */

   int VLSucgReset
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE  handle,
      ucodeT *ucodeP
#endif
   );

#define _ULSCGEN_UNIX_
   /****** These functions can be used to retrieve or print errors: ******/



   /******************************************************************
    * DESCRIPTION :
    * This function retrieves number of messages recorded in the handle.
    *
    * RETURN VALUES :
    * returns VLSucg_NO_RESOURCES if no resources available.
    * returns VLSucg_FAIL on failure.
    * returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgGetNumErrors
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE  handle,    /* IN  */
      int    *numMsgsP         /* OUT */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    * This function retrieves the length of msg # msgNum recorded in the
    * handle. It includes the space required for NULL termination.
    *
    * RETURN VALUES :
    * returns VLSucg_NO_RESOURCES if no resources available.
    * returns VLSucg_FAIL on failure.
    * returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgGetErrorLength
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE  handle, /* IN  */
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
    * returns VLSucg_NO_RESOURCES if no resources available.
    * returns VLSucg_FAIL on failure.
    * returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgGetErrorMessage
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE  handle,   /* IN  */
      char   *msgBuf,         /* INOUT */
      int     bufLen          /* IN */
#endif
   );



   /******************************************************************
    * DESCRIPTION :
    * This function spills the error struct to the file given.
    *
    * RETURN VALUES :
    * returns VLSucg_NO_RESOURCES if no resources available.
    * returns VLSucg_FAIL on failure.
    * returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgPrintError
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE  handle,    /* IN  */
      FILE  *file              /* INOUT */
#endif
   );


#undef _ULSCGEN_UNIX_

   /*****************************************************************
    * Begin functions that set fields of the code struct:
    * The struct contains info independent of short/long codes.
    * These functions return 0 on success.
    */

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value ucodeP->base_feature_name to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLSucg_NO_FEATURE_NAME if the name is NULL
    *   returns VLSucg_RESERV_STR_ERROR if the string is a reserved string
    *   returns VLSucg_INVALID_CHARS if the string characters are not printable.
    *   returns VLSucg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgSetBaseFeatureName
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *   ucodeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Any printable ASCII except # */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value ucodeP->base_feature_version to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLSucg_RESERV_STR_ERROR if the string is a reserved string
    *   returns VLSucg_INVALID_CHARS if the string characters are not printable.
    *   returns VLSucg_EXCEEDS_MAX_VALUE if string exceeds maximum no. of chars.
    *   returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgSetBaseFeatureVersion
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *   ucodeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Any printable ASCII except # */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value ucodeP->lock_info to the value of lockCode.
    *
    * RETURN VALUES :
    *   returns VLSucg_EXCEEDS_MAX_VALUE if value is too big.
    *   returns VLSucg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLSucg_INVALID_HEX_TYPE if value is not in hexadecimal format.
    *   returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgSetUpgradeCode
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *   ucodeP,    /* INOUT - the license code structure */
      char  *   lockCode /* IN - The lock code to be checked and set */
#endif
   );




   /******************************************************************
    * DESCRIPTION :
    *   Sets the value ucodeP->upd_flag to the value of flag.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLSucg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLSucg_EXCEEDS_MAX_VALUE if value exceeds VLSucg_UPGRADE_ALL.
    *   returns VLSucg_LESS_THAN_MIN_VALUE if value is lower than
    *     VLSucg_UPGRADE_VERSION.
    *   returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgSetUpgradeFlag
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *   ucodeP,  /* INOUT - the license code structure */
      char  *   flag    /* IN - The value of flag is used to set the
                  upd_flag of ucodeT struct.  Legal values are
                  bit combinations of :
                  VLSucg_UPGRADE_VERSION
                  VLSucg_UPGRADE_CAPACITY
                  */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value ucodeP->upd_version to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLSucg_RESERV_STR_ERROR if the string is a reserved string
    *   returns VLSucg_INVALID_CHARS if the string characters are not printable.
    *   returns VLSucg_EXCEEDS_MAX_VALUE if string exceeds maximum no. of chars.
    *   returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgSetUpgradeVersion
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *   ucodeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Any printable ASCII except # */
#endif
   );

   /******************************************************************
    * DESCRIPTION :
    *   Sets the value ucodeP->capacity_units to the value of info.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLSucg_INVALID_INT_TYPE if info is not a non-negative integer.
    *   returns VLSucg_EXCEEDS_MAX_VALUE if value exceeds 4
    *   returns VLSucg_LESS_THAN_MIN_VALUE if value is less than 0
    *   returns VLSucg_SUCCESS on successful return.
    * NOTES :
    */
   int VLSucgSetUpgradeCapacityUnits
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *   ucodeP,  /* INOUT - the license code structure */
      char  *   info    /* IN - Capacity specification units: from
                                  0 to 4.  The semantics are:
                             "0" - Multiple of 1(s), maximum 1023
                             "1" - Multiple of 10(s), maximum 10230
                             "2" - Multiple of 100(s), maximum 102300
                             "3" - Multiple of 1000(s), maximum 1023000
                             "4" - Multiple of 10000(s), maximum 10230000
                           */
#endif
   );


   /******************************************************************
    * DESCRIPTION :
    *   Sets the value ucodeP->capacity to the value of decimalNum.
    *   Checks the user input and saves the value in the code struct.
    *
    * RETURN VALUES :
    *   returns VLSucg_INVALID_INT_TYPE if value is not numeric.
    *   returns VLSucg_EXCEEDS_MAX_VALUE if value exceeds maximum .
    *   returns VLSucg_LESS_THAN_MIN_VALUE if value is lower than minimum.
    *   returns VLSucg_SUCCESS on successful return.
    *
    * NOTES :
    */

   int VLSucgSetUpgradeCapacity
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *  ucodeP,     /* INOUT - the license code structure */
      char  *  decimalNum /* IN - Controls the Capacity.
                 Use a numeric decimal value.
                 */
#endif
   );

   /******************************************************************
   * DESCRIPTION :  VLSucgAllowxx() FUNCTIONS
   *
   *  Boolean functions: Return 1 on TRUE, 0 on FALSE.
   *
   *  These functions test whether the corresponding VLSucgSetxx()
   *  should be called or not.  If VLSucgAllowxx() returns 1 only then
   *  corresponding VLSucgSetxx() function should be called.
   **********************************************************/

   /* Allowed if upd_flag has its VLSucg_UPGRADE_VERSION bit enabled. */
   int VLSucgAllowBaseFeatureName
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *ucodeP    /* INOUT - The Pointer to ucodeT struct   */
#endif
   );

   int VLSucgAllowBaseFeatureVersion
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *ucodeP    /* INOUT - The Pointer to ucodeT struct   */
#endif
   );

   int VLSucgAllowUpgradeCode
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *ucodeP    /* INOUT - The Pointer to ucodeT struct   */
#endif
   );

   int VLSucgAllowUpgradeFlag
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *ucodeP    /* INOUT - The Pointer to ucodeT struct   */
#endif
   );

   int VLSucgAllowUpgradeVersion
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *ucodeP    /* INOUT - The Pointer to ucodeT struct   */
#endif
   );

   /* Allowed if upd_flag has its VLSucg_UPGRADE_CAPACITY bit enabled. */
   int VLSucgAllowUpgradeCapacity
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *ucodeP    /* INOUT - The Pointer to ucodeT struct   */
#endif
   );


   /**********************************************************
    * DESCRIPTION :
    *  It generates the license string for the given ucodeT struct.
    *  It should be called after all the VLSucgSet functions are called.
    *  Memory allocation and free for ucodeT are the responsibilities
    *  of the caller of function.
    *  Memory allocation for the license string is being done by the
    *  function.  Pointer to a char will hold the string and its address
    *  is to be passed by caller of this function in second argument.
    * RETURN VALUES:
    *  returns VLSucg_SUCCESS on successful return.
    *  returns VLSucg_INVALID_VENDOR_CODE if vendor identification is illegal.
    *  returns VLSucg_VENDOR_ENCRYPTION_FAIL if vendor-customized encryption fails.
    * NOTE :
    */

   int VLSucgGenerateLicense
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      ucodeT *ucodeP,              /* IN */
      char *upgradeCode,
      char ** result             /* OUT */
#endif
   );
#define _ULSCGEN_UNIX_
   /**********************************************************
    * DESCRIPTION :
    *  It decodes the license string "AnyLicenseString" and puts the
    *  corresponding CodeT struct in the last argument.Address of a
    *  pointer to ucodeT struct is to be passed as the last argument.
    *  This pointer will contain the ucodeT of the input license string.
    *  This function takes care of all memory allocations it uses.
    *
    * RETURN VALUES:
    *  returns VLSucg_SUCCESS on successful return.
    *  returns error codes for errors.
    * NOTE :
    */




   int VLSucgDecodeLicense
   (
#ifndef LSNOPROTO
      VLSucg_HANDLE iHandle,
      char * AnyLicenseString,    /* IN  license string*/
      char * lic_string,          /* OUT license string after removing comment chars and white spaces
                                     *     NOTE:if this parameter "lic_string"
                                     *           is set as NULL then set third parameter "lic_string_length"
                                     *           as 0 else set third parameter lic_string_length  as
                                     *           " (sizeof(lic_string)+1)"
                                     */
      int  lic_string_length,     /* IN length of lic_string*/
      ucodeT ** ucodePP           /* OUT */
#endif
   );
#undef _ULSCGEN_UNIX_
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
   int VLSucgGetLicenseMeterUnits(
#ifndef LSNOPROTO
      VLSucg_HANDLE  iHandle,         /* IN */
      long         *initialUnitsP,   /* OUT */
      long         *unitsLeftP,      /* OUT */
      int          ucodegen_version   /* IN  */
#endif
   );
#define _ULSCGEN_UNIX_

#if defined(_VMSWIN_) || defined (WIN32)
#pragma pack(pop)
#endif /*_VMSWIN_*/

#ifdef __cplusplus
}
#endif
#endif

#undef _ULSCGEN_UNIX_
