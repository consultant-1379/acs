/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

#ifndef _LSRLF_H_
#define _LSRLF_H_

/*H****************************************************************
* FILENAME    : lsrlf.h
*
* DESCRIPTION :
*           Contains public function prototypes, macros and defines
*           needed for generating and reading the LM Server license
*           redundant configuration file. This configuration file is
*           used by the LM Server for supporting Redundancy feature.
*           For more details, please refer to the documentation of 
*           Sentinel RMS Developer Kit
*******************************************************************
*H*/

#ifdef __cplusplus
extern "C"
{
#endif
#include "lserv.h"


#define VLS_MAX_SERVER_NAME_LEN                      65		/* Including null character */    	
#define VLS_MAX_SERVER_IP_LEN                        64		/* Including null character */
#define VLS_MAX_SERVER_NUM_IN_POOL                   11
#define VLS_RLF_MAX_POOL_NAME_LEN                    8      /* Including null character */
#define VLS_RLF_LIBINFO_LEN                          32     /* Including null character */


/* Open modes for configuration file */
 /* Open file for reading  */
#define VLS_RLF_OPEN_READ                            1     

 /* Open file for writing */
#define VLS_RLF_OPEN_WRITE                           2     

/* Same as VLS_RLF_OPEN_WRITE. However, creates the file 
   if file does not exist */
#define VLS_RLF_OPEN_CREATE                          3      
                                                               


/*Error Codes.*/

/* Needs no introduction :-) Every thing alright! */
#define VLS_RLF_SUCCESS                              0

/* Internal error in library */
#define VLS_RLF_INTERNAL_ERROR                       1

/* Library could not locate enough resources to complete the
 * requested operation */
#define VLS_RLF_NO_RESOURCES                         2

/* General error by vendor in calling function etc. */
#define VLS_RLF_CALLING_ERROR                        3

/* Time stamp provided to the API is invalid  */
#define VLS_RLF_TIMESTAMP_INVALID                    4

/* Preference order provided to the API is invalid */
#define VLS_RLF_PREF_ORDER_INVALID                   5

/* Specified pool name is invalid */
#define VLS_RLF_POOLNAME_INVALID                     6

/* Specified feature name is invalid */
#define VLS_RLF_FEATURE_INVALID                      7

/* Specified version name is invalid */
#define VLS_RLF_VERSION_INVALID                      8

/* Library is already initialized */
#define VLS_RLF_LIBRARY_ALREADY_INITIALIZED          9

/* Specified sequence number is invalid */
#define VLS_RLF_SEQNO_INVALID                        10

/* Specified file name is invalid */
#define VLS_RLF_FILENAME_INVALID                     11

/* Specified IP is invalid */
#define VLS_RLF_IP_INVALID                           12

/* Specified server name  is invalid */
#define VLS_RLF_SERVER_NAME_INVALID                  13

/* Configuration file is invalid */
#define VLS_RLF_CONFIG_FILE_INVALID                  14

/* Configuration file not found */
#define VLS_RLF_FILE_NOT_FOUND                       15

/* Operation is not supported for the type of license
   specified */
#define VLS_RLF_LICENSE_NOT_SUPPORTED                16

/*  No permissions to perform the read operation */
#define VLS_RLF_PERM_READ_DENIED                     17

/*  No permissions to perform the write operation */
#define VLS_RLF_PERM_WRITE_DENIED                    18

/* Opened for reading but trying to write */
#define VLS_RLF_OPERATION_NOT_PERMITTED              19

/* Specified license is not a valid license */
#define VLS_RLF_LICENSE_STRING_INVALID               20

/* The server is already present in the redundant 
   configuration file */
#define VLS_RLF_SERVER_ALREADY_PRESENT               21   

/* The license string is already present in the
   redundant configuration file */
#define VLS_RLF_LICENSE_ALREADY_PRESENT              22

/* The IP is already present in the
   redundant configuration file */
#define VLS_RLF_IP_ALREADY_PRESENT                   23

/* The specified server is not present in the
   redundant configuration file */
#define VLS_RLF_SERVER_NOT_PRESENT                   24

/* The limit on the maximum number of servers
   in configuration is already reached */
#define VLS_RLF_MAX_SERVER_LIMIT_REACHED             25

/* No server present in the configuration file */   
#define VLS_RLF_NO_SERVER_PRESENT                    26

/* Only one server present in the configuration file */
#define VLS_RLF_ONLY_ONE_SERVER_PRESENT              27

/* Library is not initialized */
#define VLS_RLF_LIBRARY_NOT_INITIALIZED              28

/* A configuration file is already loaded */
#define VLS_RLF_FILE_ALREADY_LOADED                  29

/* No configuration file is loaded */
#define VLS_RLF_FILE_NOT_LOADED                      30 

/* Feature name not found in the configuration file */  
#define VLS_RLF_FEATURE_NOT_FOUND                    31 

/* License string not found in the configuration file */
#define VLS_RLF_LICENSE_STRING_NOT_FOUND             32

/* Failed to acquire API lock. API call should 
   be re-tried */
#define VLS_RLF_RESOURCE_LOCK_FAILURE                33

/* The size of the buffer supplied is too small*/
#define VLS_RLF_BUFFER_TOO_SMALL                     34
        
        
/*  Struct types*/

/* Server names, IP and corresponding preference order */
typedef struct _vlsserverdetail
    {
        unsigned char       serverName[VLS_MAX_SERVER_NAME_LEN];
        unsigned char       serverIP[VLS_MAX_SERVER_IP_LEN];
        int                 serverPrefOrder;
                        
    } VLSrlfServerDetail;

/* For retrieving and modifying the preference order */
typedef struct _vlsserverandpreforder
    {
        long                structSz;
        long                numElement;
        VLSrlfServerDetail     serverDetail[VLS_MAX_SERVER_NUM_IN_POOL];

    }VLSrlfServerAndPrefOrder;

/* Time stamp related information */
typedef  struct _vlsrlftimestampinfo
    {
        long                structSz;
        int                 hour;
        int                 minute;
        int                 second;
        int                 day;
        int                 month;
        int                 year;
    } VLSrlfTimeStampInfo;
    
/* For retrieving feature related information */    
typedef struct  _vlsrlffeatureinfo
    {
        unsigned long       structSz;
        unsigned char       feature_name[VLS_MAXFEALEN];
        unsigned char       version[VLS_MAXFEALEN];
    }VLSrlfFeatureInfo;
	
/* For retrieving library version information */
typedef struct _vlsrlflibversion
	{
		unsigned long        structSz;
		unsigned char    	 szVersion  [VLS_RLF_LIBINFO_LEN];
	}VLSrlfLibInfo;
	
    
/* Public APIs */

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfInitialize(
#ifndef LSNOPROTO
      unsigned char         *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long         *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfLoadFile(
#ifndef LSNOPROTO
      unsigned char         *file_name,     /* IN - File name to be loaded*/
      int                    open_mode,     /* IN - How to open this file */
      unsigned char         *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long         *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );

 
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfAddServer(
#ifndef LSNOPROTO
      unsigned char         *server_name,   /* IN - Server name to be added in the file*/
      unsigned char         *server_ip,     /* IN - Server IP against the server name */
      unsigned char         *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long         *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfDeleteServer(
#ifndef LSNOPROTO
      unsigned char        *server_name,   /* IN - Name of the server that needs to be deleted*/
      unsigned char        *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long        *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfChangePoolName(
#ifndef LSNOPROTO
      unsigned char        *pool_name,     /* IN - The new pool name*/
      unsigned char        *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long        *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfChangeSequenceNumber(
#ifndef LSNOPROTO
      unsigned long        seq_num,        /* IN - New seq number  */
      unsigned long       *seq_num_old,    /* OUT- If not NULL, then current/old sequence number 
                                                  is returned in this argument*/
      unsigned char       *reserved1,      /* IN - Not used. Pass NULL to ignore */
      unsigned long       *reserved2       /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfChangeTimeStamp(
#ifndef LSNOPROTO
      VLSrlfTimeStampInfo *time_stamp_info_new,  /* IN - timestamp */
      VLSrlfTimeStampInfo *time_stamp_info_old,  /*OUT- If not NULL, then current/old    
                                                         information is returned.
							 Memory allocation need to done by caller */
      unsigned char       *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long       *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );
   

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfAddLicense(
#ifndef LSNOPROTO
      unsigned char       *lic_str,       /* IN - License string to be added to the config file*/
      unsigned char       *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long       *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfReadFeatureAtIndex(
#ifndef LSNOPROTO
    unsigned long          index,        /* IN - Index at which Feature info is to be read */
    VLSrlfFeatureInfo     *feature_info, /* OUT- Feature information. Allocated by caller */
    unsigned char         *reserved1,    /* IN - Not used. Pass NULL to ignore */
    unsigned long         *reserved2     /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
    );
    
  VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfReadLicenseAtIndex(
#ifndef LSNOPROTO
    unsigned long          index,        /* IN - Index w.r.t the feature_name and version_name
                                                 section in the configuration file */
    unsigned char         *feature_name, /* IN - Feature name. Cant be NULL */
    unsigned char         *version_name, /* IN - Version name. Cant be NULL */
    unsigned char         *lic_str,      /* OUT- On return, contains the license string
                                                 corresponding to the index. Memory allocated by caller*/
    int                    size,         /* IN - Size of the memory allocated in lic_str*/
    unsigned char         *reserved1,    /* IN - Not used. Pass NULL to ignore */
    unsigned long         *reserved2     /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
     
    ); 


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfDeleteLicense(
#ifndef LSNOPROTO
      unsigned char        *lic_str,      /* IN - License string to be deleted from file */
      unsigned char        *reserved1,    /* IN - Not used. Pass NULL to ignore */
      unsigned long        *reserved2     /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );

   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfModifyPreferenceOrder(
#ifndef LSNOPROTO
                                        
      VLSrlfServerAndPrefOrder *server_preforder, /* IN - New pref order */
                                
      unsigned char       *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long       *reserved2      /* IN - Not used. Pass NULL to ignore */
       
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfGetPoolInfo(
#ifndef LSNOPROTO
      unsigned char       *pool_info,     /* OUT- pool_info will be returned in char array
                                                  allocated by caller */
      int                  size,          /* IN - Size of the allocation done for pool_info */
      VLSrlfServerAndPrefOrder  *server_preforder, /* OUT- On return, contains the pref order */
      unsigned char       *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long       *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfCloseFile(
#ifndef LSNOPROTO
      unsigned char       *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long       *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );


   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfCleanUp(
#ifndef LSNOPROTO
      unsigned char       *reserved1,     /* IN - Not used. Pass NULL to ignore */
      unsigned long       *reserved2      /* IN - Not used. Pass NULL to ignore */
#endif /* LSNOPROTO */
   );
	
   VDLL32 LS_STATUS_CODE VMSWINAPI VLSrlfGetLibInfo(
#ifndef LSNOPROTO	
      VLSrlfLibInfo 		*lib_info 		/* OUT: Memory allocated by caller */
#endif /* LSNOPROTO */
   );	

#ifdef __cplusplus
}
#endif


#endif /* _LSRLF_H_ */
