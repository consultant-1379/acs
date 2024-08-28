/* *****************************************************************
*                                                                 
*               Copyright (C) 2020 SafeNet, Inc.                  
*                      All Rights Reserved                        
*                                                                 
*     This Module contains Proprietary Information of SafeNet     
*          Inc., and should be treated as Confidential.           
****************************************************************** */

#ifndef _LS_CPD_HEADER_
#define _LS_CPD_HEADER_

/*H****************************************************************
* FILENAME    : lscpdif.h
*
* DESCRIPTION :
*           Contains public callback function prototypes, macros and defines
*           needed for customizing the RMS licensing system by using the 
*           "Custom Persistence Device" (i.e. CPD).
* USAGE       :
*           This file should be included by all users of Sentinel RMS
*           Client/Init/Clean library who wants to use "Custom Persistence
*           Device" i.e. CPD offered by RMS.
*H***************************************************************** */


#ifdef __cplusplus
extern "C"
{
#endif

/* system includes */
#include <stdio.h>     /* For definition of basic types */



#define MAX_ERROR_BUF_SIZE         512

/* Below enum lets user to select which persistence model to use.
*  User can select 'No Persistence' or can use 'customized Persistence model' */
typedef enum {
	VLS_CPD_NO_DEVICE=0,             /* Disable Persistence*/
	VLS_CPD_CUSTOM_DEVICE            /* Use Custom Persistence Device */
}VLScpdPersistenceDeviceT;

/* Below struct will be used by user to pass pointers to 
*  'call back functions' implemented by user to RMS*/
typedef struct {
   int   szStruct;                  /* size of the struct; this member must be set to sizeof(VLScpdIfImpl) before passing to set device api. */
   void *implInitDevice;            /* pointer to call back function VLScpdInitializeDevice() */
   void *implExitDevice;            /* pointer to call back function VLScpdExitDevice() */
   void *implCreateObject;          /* pointer to call back function VLScpdCreateObject() */
   void *implWriteRecord;           /* pointer to call back function VLScpdWriteRecord() */
   void *implReadRecord;            /* pointer to call back function VLScpdReadRecord() */
   void *implReadNextRecord;        /* pointer to call back function VLScpdReadNextRecord() */
   void *implDeleteRecord;          /* pointer to call back function VLScpdDeleteRecord() */
} VLScpdIfImpl;


int VLSsetPersistenceDevice(
   VLScpdPersistenceDeviceT  persistenceDevice,
   VLScpdIfImpl              *deviceIoImpl,
   unsigned long             *reserved1,
   unsigned char             *reserved2 
);

int sntlSetPersistenceDevice(
   VLScpdPersistenceDeviceT  persistenceDevice,
   VLScpdIfImpl              *deviceIoImpl,
   unsigned long             *reserved1,
   unsigned char             *reserved2 
);

int VLScleanSetPersistenceDevice(
   VLScpdPersistenceDeviceT  persistenceDevice,
   VLScpdIfImpl              *deviceIoImpl,
   unsigned long             *reserved1,
   unsigned char             *reserved2 
);

/* Below enum will be used as input parameter in VLScpdWriteRecord() callback api.
*  to distinguish whether to perform write or modify operation on the device */
typedef enum {
      CPD_WRITE_RECORD =0,        /* Write to the device */
      CPD_MODIFY_RECORD           /* Modify existing data */
} VLS_WRITE_OP_MODE;


/*******************************************************************
* Error/Status Code to be returned from call back api              *
***************************************************************** */
/* generic error codes */
#define VLS_CPD_SUCCESS                     0           /* callback function executed successfully */
#define VLS_CPD_BAD_INPUT                   1           /* Invalid arguments passed by RMS in the callback apis */
/* Record related errors */
#define VLS_CPD_NO_MORE_RECORD              10          /* Should be returned only in case of VLScpdReadNextRecord() api. It indicates end-of-resultset or when resultset is empty */
#define VLS_CPD_NO_RECORD_FOUND             11          /* Requested record does not exist or Result Set is empty */
/* Objects related errors */
#define VLS_CPD_NO_OBJECT_FOUND             21          /* ISV should return this when doing any CPD operation it is detected that the CPD object doesn't exist.*/
#define VLS_CPD_OBJECT_ALREADY_EXISTS       22          /* Request is made to create object but object already exists*/
/* Device related errors */
#define VLS_CPD_DEVICE_FATAL_ERROR          31          /* Device fatal error - reading/writing failed etc. Can also be thrown in case any other unknown/un-identifiable error occured */
#define VLS_CPD_DEVICE_NO_PERM              32          /* Insufficient permission */
/*****************SECTION END ****************************************/


/*********************************************************************
* Types of persistence                                               *
*********************************************************************/
#define CPD_TYPE_TRIAL              1
#define CPD_TYPE_REVOKE             2
#define CPD_TYPE_COMMUTER_CLIENT    3
#define CPD_TYPE_COMMUTER_SERVER    4
#define CPD_TYPE_GRACE              5
#define CPD_TYPE_CONSUME            6
#define CPD_TYPE_NETWORK_REVOKE     7
#define CPD_TYPE_TIME_TAMPER        8
#define CPD_TYPE_USAGE_LOG          9
#define CPD_TYPE_USAGE_GUID         10
#define CPD_TYPE_USAGE_LOG_NONET    11
#define CPD_TYPE_CANCEL_LEASE_STORE 12
#define CPD_TYPE_ROLLOVER_COUNT     13
#define CPD_TYPE_USAGE_LOG_COMMON   14
/*****************SECTION END ****************************************/
 

/*********************************************************************
* Prototypes of callback APIs in the CPD interface to be implemented *
* by ISV.                                                            *
*********************************************************************/

/* Below callback will be called by the RMS to initiate the initialization
*  of the CPD. Task such as opening device or making DB connection etc
*  should be performed during this callback api. This is just for the indication
*  purposes to the ISV's implementation and ISV may choose to ignore if they have
*  initialized the device already 
*  Returns:
*     VLS_CPD_SUCCESS         - When there is no error
*     VLS_CPD_BAD_INPUT       - If arguments passed by RMS in this callback is not as per specification.
*     VLS_CPD_DEVICE_NO_PERM  - When ISV detects some permission related error 
*     VLS_CPD_DEVICE_FATAL_ERROR    - In case of any read/write failure to the device or any unforeseen errors */
int VLScpdInitializeDevice (
   char     *pcErrorInfoOut            /* OUT param; Max buffer size will be MAX_ERROR_BUF_SIZE */
);
typedef int (*VLScpdInitializeDeviceT)(char *);


/* Below callback initiates the cleanup of resources on device. After this call
*  from RMS, CPD life-cycle will only start again with a call to 'VLScpdInitializeDevice'
*  callback api. This is just for the indication purposes to the ISV's implementation and
*  ISV may choose to ignore if they have initialized the device already 
*  Returns:
*     VLS_CPD_SUCCESS         - When there is no error
*     VLS_CPD_BAD_INPUT       - If arguments passed by RMS in this callback is not as per specification.
*     VLS_CPD_DEVICE_NO_PERM  - When ISV detects some permission related error 
*     VLS_CPD_DEVICE_FATAL_ERROR    - In case of any read/write failure to the device or any unforeseen errors */
int VLScpdExitDevice (
   char     *pcErrorInfoOut            /* OUT param; Max buffer size will be MAX_ERROR_BUF_SIZE */
);
typedef int (*VLScpdExitDeviceT)(char *);


/* Below callback will be called by the RMS to create persistence object based on type
*  of the Licensing-model being used. Just to mention, object here is being referred in
*  a very abstract context. Actually an object may be a DB table if device is RDBMS, may
*  be an xml file if device is flat file or may be some memory area in case of some
*  embedded device etc. If an object doesn't exist and is needed by RMS then it should
*  be created and if object already exists then it MUST NOT recreate the object and
*  should return VLS_CPD_OBJECT_ALREADY_EXISTS .
*  Returns:
*     VLS_CPD_SUCCESS         - When there is no error
*     VLS_CPD_BAD_INPUT       - If arguments passed by RMS in this callback is not as per specification.
*     VLS_CPD_DEVICE_NO_PERM  - When ISV detects some permission related error 
*     VLS_CPD_OBJECT_ALREADY_EXISTS - When object already exists. ISV should not create object again in this case.
*     VLS_CPD_DEVICE_FATAL_ERROR    - In case of any read/write failure to the device or any unforeseen errors */
int VLScpdCreateObject(
   int      iPersistType,             /* IN param */
   char     *pcErrorInfoOut           /* OUT param */
);
typedef int (*VLScpdCreateObjectT)(int, char *);


/* Below callback will be called by the RMS to read a single record from device.
*  Care must be taken to read the provide the correct record identifiable by
*  the unique comibination of 'iPersistType' and 'pcCpdKeyIn'.
*  Returns:
*     VLS_CPD_SUCCESS         - When there is no error
*     VLS_CPD_BAD_INPUT       - If arguments passed by RMS in this callback is not as per specification.
*     VLS_CPD_DEVICE_NO_PERM  - When ISV detects some permission related error 
*     VLS_CPD_NO_RECORD_FOUND - No record exists for the unique combination of 'iPersistType' & 'pcCpdKeyIn' 
*     VLS_CPD_NO_OBJECT_FOUND - When doing any CPD operation and it is detected that the CPD object doesn't exist.
*     VLS_CPD_DEVICE_FATAL_ERROR    - In case of any read/write failure to the device or any unforeseen errors */
int VLScpdReadRecord(
   int      iPersistType,          /* IN param */
   char     *pcCpdKeyIn,           /* IN param */
   int      iSzKey,                /* IN param */
   char     *pcCpdValueOut,        /* OUT param */
   int      iSzValue,              /* IN param */
   char     *pcErrorInfoOut        /* OUT param */
);
typedef int (*VLScpdReadRecordT)(int, char *, int, char *, int , char *);


/* Below callback api will be called by RMS to insert/Modify a single record
*  in an CPD object on the device. Care must be taken to insert/Modify the
*  record in the correct object identifiable by 'iPersistType'. Which Operation
*  to perform will be governed by input parameter 'iOperationMode'.
*   if 'iOperationMode' == CPD_WRITE_RECORD then data should be inserted
*   if 'iOperationMode' == CPD_MODIFY_RECORD then data should be modified
*  Returns:
*     VLS_CPD_SUCCESS         - When there is no error
*     VLS_CPD_BAD_INPUT       - If arguments passed by RMS in this callback is not as per specification.
*     VLS_CPD_DEVICE_NO_PERM  - When ISV detects some permission related error 
*     VLS_CPD_NO_OBJECT_FOUND - When doing any CPD operation and it is detected that the CPD object doesn't exist.
*     VLS_CPD_DEVICE_FATAL_ERROR  - In case of any read/write failure to the device or any unforeseen errors */
int VLScpdWriteRecord(
   int      iOperationMode,        /* IN param */
   int      iPersistType,          /* IN param */
   char     *pcCpdKeyIn,           /* IN param */
   int      iSzKey,                /* IN param */
   char     *pcCpdValueIn,         /* IN param */
   int      iSzValue,              /* IN param */
   char     *pcErrorInfoOut        /* OUT param */
);
typedef int (*VLScpdWriteRecordT)(int, int, char *, int, char *, int , char *);


/* Below callback api initiates the deletion of a single record from
*  CPD object on the device. Record is searched based on the unique combination
*  of 'iPersistType' & 'pcCpdKeyIn' after that should be deleted from the object.
*  Returns:
*     VLS_CPD_SUCCESS         - When there is no error
*     VLS_CPD_BAD_INPUT       - If arguments passed by RMS in this callback is not as per specification.
*     VLS_CPD_DEVICE_NO_PERM  - When ISV detects some permission related error 
*     VLS_CPD_NO_RECORD_FOUND - No record exists for the unique combination of 'iPersistType' & 'pcCpdKeyIn' 
*     VLS_CPD_NO_OBJECT_FOUND - When doing any CPD operation and it is detected that the CPD object doesn't exist.
*     VLS_CPD_DEVICE_FATAL_ERROR  - In case of any read/write failure to the device or any unforeseen errors */
int VLScpdDeleteRecord(
   int      iPersistType,          /* IN param */
   char     *pcCpdKeyIn,           /* IN param */
   int      iSzKey,                /* IN param */
   char     *pcErrorInfoOut        /* OUT param */
);
typedef int (*VLScpdDeleteRecordT)(int, char *, int, char *);


/* Below callback api will be called by RMS to read all the records from CPD object
*  based on 'iPersistType' passed as input parameter,. This api will be called
*  repeatedly by RMS in a loop until VLS_CPD_NO_MORE_RECORD is returned .
*  If this api is called with iIsFirstInvocation == 1 then api should return the first
*  record from set of records( result-set).
*  Returns:
*     VLS_CPD_SUCCESS         - When there is no error
*     VLS_CPD_BAD_INPUT       - If arguments passed by RMS in this callback is not as per specification.
*     VLS_CPD_DEVICE_NO_PERM  - When ISV detects some permission related error 
*     VLS_CPD_NO_MORE_RECORD  - After all records of 'iPersistType' are read by one by one or there is not even a single record of this 'iPersistType'
*     VLS_CPD_NO_OBJECT_FOUND - When doing any CPD operation and it is detected that the CPD object doesn't exist.
*     VLS_CPD_DEVICE_FATAL_ERROR  - In case of any read/write failure to the device or any unforeseen errors */
int VLScpdReadNextRecord(
   int      iIsFirstInvocation,    /* IN param */
   int      iPersistType,          /* IN param */
   char     *pcCpdKeyOut,          /* OUT param */
   int      iSzKey,                /* IN param */
   char     *pcCpdValueOut,        /* OUT param */
   int      iSzValue,              /* IN param */
   char     *pcErrorInfoOut        /* OUT param */
);
typedef int (*VLScpdReadNextRecordT)(int, int, char *, int, char *, int , char *);

/*****************SECTION END ****************************************/

#ifdef __cplusplus
}
#endif
#endif //_LS_CPD_HEADER_
