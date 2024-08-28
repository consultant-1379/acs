/*=================================================================== 

   @file   ACS_APGCC_CommonLib.h

   @brief .

          
   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
   N/A 		 21/12/2015    xsansud	   HU32763
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_APGCC_COMMONLIB_H_
#define ACS_APGCC_COMMONLIB_H_

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <string.h>

#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_CLibTypes.h"
#include "ACS_CC_Types.h"
#include "ace/OS.h"

#define ACS_APGCC_HWVER_APG40C2_STRING "APG40C/2"
#define ACS_APGCC_HWVER_APG40C4_STRING "APG40C/4"
#define ACS_APGCC_HWVER_APG43_GEP1_STRING "APG43"
#define ACS_APGCC_HWVER_APG43_GEP2_STRING "APG43/2"
#define ACS_APGCC_HWVER_APG43_GEP4_STRING "APG43/3"
#define ACS_APGCC_HWVER_APG43_GEP5_STRING "APG43/3"
#define ACS_APGCC_HWVER_APG43_GEP4_400STRING "APG43/3-400"
#define ACS_APGCC_HWVER_APG43_GEP4_1600STRING "APG43/3-1600"
#define ACS_APGCC_HWVER_APG43_GEP5_400STRING "APG43/3-400"
#define ACS_APGCC_HWVER_APG43_GEP5_1200STRING "APG43/3-1200"
#define ACS_APGCC_HWVER_APG43_GEP7_STRING "APG43/4"
#define ACS_APGCC_HWVER_APG43_GEP7L_400STRING "APG43/4-400"
#define ACS_APGCC_HWVER_APG43_GEP7L_1600STRING "APG43/4-1600"
#define ACS_APGCC_HWVER_APG43_GEP7_128_1600STRING "APG43/4-128-1600"

#define PARENT_OBJECT_DataDisk_DN "dataDiskPathMId=1"
#define PARENT_OBJECT_FTPVirtual_DN "ftpVirtualPathMId=1"
#define INSTANCE_DN_DATADISK_STRING "dataDiskPathId="
#define INSTANCE_DN_FTPVIRTUAL_STRING "ftpVirtualPathId="

using namespace std;

class ACS_APGCC_CommonLib {

public:

	ACS_APGCC_CommonLib();

	~ACS_APGCC_CommonLib();

	ACS_APGCC_CLib_ReturnTypeT SetFilePerm (const char* FilePath, ACS_APGCC_basicLinuxACLT aBasicLinuxACLdata, int p_special_perm, char* MessageBuffer );

	ACS_APGCC_HwVer_ReturnTypeT GetHWVersion (char* hwVersion, int &p_Len ) __attribute__ ((deprecated));

	ACS_APGCC_HwVer_ReturnTypeT GetHWVariant (char* hwVariant, int &p_Len ) __attribute__ ((deprecated));

	ACS_APGCC_DNFPath_ReturnTypeT GetDataDiskPath (const char* p_LogicalName, char* p_Path,	int &p_Len ) ;

	ACS_APGCC_DNFPath_ReturnTypeT GetDataDiskPathForCp (const char* p_LogicalName,unsigned int cpId, char* p_Path, int &p_Len);

	ACS_APGCC_DNFPath_ReturnTypeT GetFTPVirtualPath (const char* p_LogicalName, char* p_VirtualDir,	int &p_Len);
	
	ACS_APGCC_EncrptDecryptData_ReturnTypeT EncryptDecryptData ( const char* p_Input,
                                                                 const char* CommandLine,
                                                                 char* p_Output,
                                                                 const char* p_Key,
                                                                 ACS_APGCC_DIRFlagT p_dFlag,
                                                                 int p_dataLen,
                                                                 int p_kLen );

	ACS_APGCC_DNFPath_ReturnTypeT GetFileMPath(const char* p_FileMFuncName,char* p_Path,int &p_Len);	

	ACS_APGCC_Roles_ReturnTypeT GetRoles(const string &user, string &strOutRoles);
	
	ACS_APGCC_Roles_ReturnTypeT CheckLocalUser(const string &user, const string &password);			//HU32763

	ACS_APGCC_Roles_ReturnTypeT CheckUser(const string &user, const string &password);

	ACS_APGCC_Roles_ReturnTypeT GetCachedRoles(const string &user, string &strOutRoles);

	int GetHwInfo ( ACS_APGCC_HWINFO* p_hwInfo, ACS_APGCC_HWINFO_RESULT* p_hwInfoResult, unsigned int apgcc_hwinfo_flag);

	bool isTsUser(const string &user);

	ACS_APGCC_UserInfo_ReturnTypeT getTsUserInfo(const string &user, string &userInfo);

	ACS_APGCC_IpAddress_ReturnTypeT getIpAddresses(ACS_APGCC_ApgAddresses &IpAddresses);

	/**
	 * Couple of methods needed to enable/disable the support for using the ACS_APGCC_CommonLib for authorization purposes.
	 * The 'enable' method must be called before starting the usage of ACS_APGCC_CommonLib for authorization.
	 */
	static ACS_APGCC_ThreadSupport_ReturnTypeT EnableMultithreadingAuthorizationSupport ();
	static ACS_APGCC_ThreadSupport_ReturnTypeT DisableMultithreadingAuthorizationSupport ();

	/**
	 * Below two methods are needed to set the values of read only attributes in MMLAuthorizationM MO	
	 * These attributes allow to ignore/allow CpRole0 and CpRole1 to CpRole15
	 *
	 */
	ACS_CC_ReturnType ignoreCPSuperUserRole(bool isIgnore);
	ACS_CC_ReturnType ignoreCPUserRoles(bool isIgnore);	 
	ACS_APGCC_IpAddress_ReturnTypeT getIpAddresses(ACS_APGCC_ApgAddresses_R2 &IpAddresses); //Fetch both IPv4 and IPv6 cluster IPs

private:
	static bool _multithreading_support_requested;
	static ACE_Recursive_Thread_Mutex _sync;
} ;


#endif /* end ACS_APGCC_COMMONLIB_H_ */
