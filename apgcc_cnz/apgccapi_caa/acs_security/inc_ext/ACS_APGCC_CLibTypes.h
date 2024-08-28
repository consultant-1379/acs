/*=================================================================== */
/**
   @file   ACS_APGCC_CLibTypes.h

   @brief .

          
   @version 1.0.0
*/
/*
   HISTORY
  
   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
==================================================================== */


#ifndef ACS_APGCC_CLIBTYPES_H_
#define ACS_APGCC_CLIBTYPES_H_

#include <iostream>
#include <string.h>

typedef struct {
	bool  readFlag;
	bool  writeFlag;
	bool  executeFlag;
} ACS_APGCC_PermissonT;

typedef struct {
	ACS_APGCC_PermissonT ownerPermission;
	ACS_APGCC_PermissonT groupPermission;
	ACS_APGCC_PermissonT otherPermission;
}ACS_APGCC_basicLinuxACLT;

typedef struct {
	char* groupName;
 	ACS_APGCC_PermissonT groupPermission;
}ACS_APGCC_extendedLinuxACLT;


typedef enum {
	ACS_APGCC_STRING_BUFFER_TOO_SMALL	= 	-2,
	ACS_APGCC_UNDEFINED_HWVER		= 	-1,
	ACS_APGCC_HWVER_APG40C2			=	1,	
	ACS_APGCC_HWVER_APG40C4			=	2,
	ACS_APGCC_HWVER_APG43			=	3,
	ACS_APGCC_HWVER_SUCCESS			= 	0
}ACS_APGCC_HwVer_ReturnTypeT;   	// to retain design base

typedef enum {
	ACS_APGCC_STRING_BUFFER_VERY_SMALL	=  -2,
	ACS_APGCC_UNDEFINED_HWTYPE		=  -1,
	ACS_APGCC_HWTYPE_SUCCESS		=  0
}ACS_APGCC_HwType_ReturnTypeT;           


typedef enum {
	ACS_APGCC_FAULTY_CPID		=	-3,
	ACS_APGCC_STRING_BUFFER_SMALL	= 	-2,
	ACS_APGCC_FAULT_LOGICAL_NAME	= 	-1,
	ACS_APGCC_DNFPATH_SUCCESS	=        0,
	ACS_APGCC_DNFPATH_FAILURE	=	 1	
}ACS_APGCC_DNFPath_ReturnTypeT;   	// to retain design base

typedef enum {
	ACS_APGCC_BAD_IV_MAT		 = -8,
	ACS_APGCC_BAD_PARAMS		 = -7,	
	ACS_APGCC_BAD_ROUNDS		 = -6,
	ACS_APGCC_BAD_CIPHER_STATE       = -5,
	ACS_APGCC_BAD_CIPHER_MODE	 = -4,
	ACS_APGCC_BAD_KEY_INSTANCE	 = -3,
	ACS_APGCC_BAD_KEY_MAT		 = -2,
	ACS_APGCC_BAD_KEY_DIR		 = -1,
	ACS_APGCC_ENCRPT_DECRPT_SUCCESS	 =  0,
	ACS_APGCC_ENCRPT_DECRPT_FAILURE	 =  1
}ACS_APGCC_EncrptDecryptData_ReturnTypeT;   	// to retain design base

typedef enum {
	ACS_APGCC_CLIB_SCCCESS	= 	0,
	ACS_APGCC_CLIB_FAILURE	= 	1
}ACS_APGCC_CLib_ReturnTypeT;		

typedef enum {
	ACS_APGCC_DIR_ENCRYPT=0,
	ACS_APGCC_DIR_DECRYPT=1
}ACS_APGCC_DIRFlagT;

typedef enum {
	ACS_APGCC_ERR_ERROR				=	1,
	ACS_APGCC_ERR_NOUSER			=	2,
	ACS_APGCC_ERR_NOCONFIG			=	3,
	ACS_APGCC_ERR_NO_FILTERCONFIG	=	4,
	ACS_APGCC_ERR_ADMINSTATE		=	5,
	ACS_APGCC_ERR_USER_DOESNOTEXIST	=	6,
	ACS_APGCC_ROLES_SUCCESS			=	0
}ACS_APGCC_Roles_ReturnTypeT;

typedef enum {
	ACS_APGCC_HWVER_NOVALUE = 0,
	ACS_APGCC_HWVER_VM 		= 1,
	ACS_APGCC_HWVER_GEP1 	= 2,
	ACS_APGCC_HWVER_GEP2 	= 3,
	ACS_APGCC_HWVER_GEP5 	= 4,
	ACS_APGCC_HWVER_GEP7	= 5
}ACS_APGCC_HwVer_TypeT;

typedef enum {
	ACS_APGCC_HWTYPE_NOVALUE	= 0,
	ACS_APGCC_HWTYPE_VM			= 1,
	ACS_APGCC_HWTYPE_GEP1		= 2,
	ACS_APGCC_HWTYPE_GEP2		= 3,
	ACS_APGCC_HWTYPE_GEP5		= 4,
	ACS_APGCC_HWTYPE_GEP5_400	= 5,
	ACS_APGCC_HWTYPE_GEP5_1200	= 6,
	ACS_APGCC_HWTYPE_GEP5_64_1200	= 7,
	ACS_APGCC_HWTYPE_GEP7L_400	= 8,
	ACS_APGCC_HWTYPE_GEP7L_1600	= 9,
	ACS_APGCC_HWTYPE_GEP7_128_1600	= 10
}ACS_APGCC_HwType_TypeT;

typedef enum {
	ACS_APGCC_DISKSLOCATION_NOVALUE	 = 0,
	ACS_APGCC_DISKSLOCATION_EXTERNAL = 1,
	ACS_APGCC_DISKSLOCATION_ONBOARD	 = 2
}ACS_APGCC_DiskLocation_TypeT;

typedef enum {
	ACS_APGCC_DISKSNUMBER_NOVALUE = 0,
	ACS_APGCC_DISKSNUMBER_1		  = 1,
	ACS_APGCC_DISKSNUMBER_3	 	  = 2
}ACS_APGCC_DiskNumber_TypeT;

typedef enum {
	ACS_APGCC_DISKSCAPACITY_NOVALUE = 0,
	ACS_APGCC_DISKSCAPACITY_147GB   = 1,
	ACS_APGCC_DISKSCAPACITY_300GB   = 2,
	ACS_APGCC_DISKSCAPACITY_400GB	= 3,
	ACS_APGCC_DISKSCAPACITY_450GB	= 4,
	ACS_APGCC_DISKSCAPACITY_600GB	= 5,
	ACS_APGCC_DISKSCAPACITY_1200GB	= 6,
	ACS_APGCC_DISKSCAPACITY_250GB   = 7,
	ACS_APGCC_DISKSCAPACITY_1600GB	= 8,
	ACS_APGCC_DISKSCAPACITY_700GB   = 9,
	ACS_APGCC_DISKSCAPACITY_1000GB  = 10,
	ACS_APGCC_DISKSCAPACITY_1400GB  = 11,
	ACS_APGCC_DISKSCAPACITY_1450GB  = 12,
    ACS_APGCC_DISKSCAPACITY_1300GB  = 13
}ACS_APGCC_DiskCapacity_TypeT;

typedef enum {
	ACS_APGCC_HWINFO_SUCCESS = 0,
	ACS_APGCC_HWINFO_FAILURE = -1
}ACS_APGCC_HwInfoResult_TypeT;

struct ACS_APGCC_HWINFO {
	ACS_APGCC_HwVer_TypeT hwVersion;
	ACS_APGCC_DiskLocation_TypeT disksLocataion;
	ACS_APGCC_DiskNumber_TypeT disksNumber;
	ACS_APGCC_DiskCapacity_TypeT disksCapacity;
	ACS_APGCC_HwType_TypeT hwType;
};

struct ACS_APGCC_HWINFO_RESULT {
	ACS_APGCC_HwInfoResult_TypeT hwVersionResult;
	ACS_APGCC_HwInfoResult_TypeT disksLocataionResult;
	ACS_APGCC_HwInfoResult_TypeT disksNumberResult;
	ACS_APGCC_HwInfoResult_TypeT disksCapacityResult;
	ACS_APGCC_HwInfoResult_TypeT hwTypeResult;
};

typedef enum {
	ACS_APGCC_TsUserInfo_SUCCES = 0,
	ACS_APGCC_TsUserInfo_ERROR = -1
}ACS_APGCC_UserInfo_ReturnTypeT;

typedef enum {
	ACS_APGCC_IpAddress_SUCCESS = 0,
	ACS_APGCC_IpAddress_FAILURE = -1
}ACS_APGCC_IpAddress_ReturnTypeT;

typedef enum {		// Stack type of the node
	ACS_APGCC_DEFAULT_STACK = -1,
	ACS_APGCC_IPv4_STACK = 0,
	ACS_APGCC_IPv6_STACK = 1,
	ACS_APGCC_DUAL_STACK = 2
} ACS_APGCC_IpAddress_StackT;

struct ACS_APGCC_ApgAddresses{
	std::string clusterIp1;
	std::string clusterIp2;
	std::string clusterIpAddress;
};

/* struct ACS_APGCC_ApgAddresses_R2 - New structure to hold stack type and both IPv4 & IPv6 cluster IPs
 * clusterIp1, clusterIp1_IPv6 - fetched from "apos_hwinfo.sh --clusterIP1"
 * clusterIp2, clusterIp2_IPv6 - fetched from "apos_hwinfo.sh --clusterIP2"
 * clusterIpAddress, clusterIpAddress_IPv6 - fetched from MO "northBoundId=1,networkConfigurationId=1"
*/
struct ACS_APGCC_ApgAddresses_R2 {
	std::string clusterIp1;
	std::string clusterIp1_IPv6;
	std::string clusterIp2;
	std::string clusterIp2_IPv6;
	std::string clusterIpAddress;
	std::string clusterIpAddress_IPv6;
	ACS_APGCC_IpAddress_StackT stackType;
};


enum {
	ACS_APGCC_GET_HWVERSION         = 0x0001,
	ACS_APGCC_GET_DISKLOCATION      = 0x0002,
	ACS_APGCC_GET_DISKNUMBER        = 0x0004,
	ACS_APGCC_GET_DISKCAPACITY      = 0x0008,
	ACS_APGCC_GET_VIRTUALENVPROFILE = 0x0010,
	ACS_APGCC_GET_CLUSTERIP_MODE1   = 0x0020,
	ACS_APGCC_GET_CLUSTERIP_MODE2   = 0x0040,
	ACS_APGCC_GET_HWTYPE		= 0x0080,
	ACS_APGCC_GET_ALL               = 0XFFFF
};

typedef enum {
	ACS_APGCC_THREADSUPPORT_OK               = 0,
	ACS_APGCC_THREADSUPPORT_GENERAL_FAULT    = -1,
	ACS_APGCC_THREADSUPPORT_ALREADY_ENABLED  = -2,
	ACS_APGCC_THREADSUPPORT_ALREADY_DISABLED = -3,
	ACS_APGCC_THREADSUPPORT_NO_ENOUGH_MEMORY = -4,
	ACS_APGCC_THREADSUPPORT_INIT_FAILED      = -5
} ACS_APGCC_ThreadSupport_ReturnTypeT;

#endif /* ACS_APGCC_CLIBTYPES_H_*/
