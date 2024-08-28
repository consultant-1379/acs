/* =================================================================== 
  
   @file ACS_APGCC_CommonLib.cpp

   header

   @version 1.0.0

   HISTOR
   

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
   N/A 		 21/12/2015    xsansud	   HU32763
   N/A           18/10/2022    zpsxsou     Added 1300gb disk capacity.
  =================================================================== */

#include <grp.h>
#include <algorithm>

#include <ace/Guard_T.h>
#include <ACS_TRA_Logging.h>

#include "sha.h"
#include "twofish.h"
#include "ACS_APGCC_CommonUtil.h"
#include "ACS_APGCC_OpenSSLThreading.h"
#include "ACS_APGCC_CommonLib.h"

#define	KEY_BITS_0	128
#define	STEP_KEY_BITS	((MAX_KEY_BITS-KEY_BITS_0)/2)

#define APGCC_LOG_BUFFER 512

const char GetHwInfo_Path[] = "/opt/ap/apos/conf/apos_hwinfo.sh";
const char GetHwInfo_ALL_Option[] = " --all";
const char GetHwInfo_IP_Mode1[] = " --clusterIP1";
const char GetHwInfo_IP_Mode2[] = " --clusterIP2";
const char GetHwInfo_HWversion[] = " --hwversion";
const char GetHwInfo_HWtype[] = " --hwtype";
const char GetHwInfo_DisksLocation[] = " --diskslocation";
const char GetHwInfo_DisksNumber[] = " --disksnumber";
const char GetHwInfo_DisksCapacity[] = " --diskscapacity";
const char GetHwInfo_VirtualEnvProfile[] = " --virtualenvprofile";

bool ACS_APGCC_CommonLib::_multithreading_support_requested = false;
ACE_Recursive_Thread_Mutex ACS_APGCC_CommonLib::_sync;

ACS_APGCC_CommonLib::ACS_APGCC_CommonLib () {

}


ACS_APGCC_CommonLib::~ACS_APGCC_CommonLib() {

}


ACS_APGCC_CLib_ReturnTypeT  ACS_APGCC_CommonLib::SetFilePerm ( 	const char* p_FilePath,
								ACS_APGCC_basicLinuxACLT p_basicLinuxACLdata,
								int p_special_perm,
								char* p_MessageBuffer ) {


		mode_t receivedACLs = 0000000 ;
		
		if ( p_FilePath == NULL ) {
			syslog (LOG_ERR, "[%s:] Invalid File path received at Line [%d]",__FUNCTION__,__LINE__);
			return ACS_APGCC_CLIB_FAILURE;
		}

		if (p_basicLinuxACLdata.ownerPermission.readFlag) {
			receivedACLs = receivedACLs|S_IRUSR;
		}
   
		if (p_basicLinuxACLdata.ownerPermission.writeFlag)
		{
			receivedACLs = receivedACLs|S_IWUSR;
		}
		
		if (p_basicLinuxACLdata.ownerPermission.executeFlag)
		{
			receivedACLs = receivedACLs|S_IXUSR;
		}
		
		if (p_basicLinuxACLdata.groupPermission.readFlag)
		{
			receivedACLs = receivedACLs|S_IRGRP;
		}
		
		if (p_basicLinuxACLdata.groupPermission.writeFlag)
		{
			receivedACLs = receivedACLs|S_IWGRP;
		}
	
		if (p_basicLinuxACLdata.groupPermission.executeFlag)
		{
			receivedACLs = receivedACLs|S_IXGRP;
		}
		
		if (p_basicLinuxACLdata.otherPermission.readFlag)
		{
			receivedACLs = receivedACLs|S_IROTH;
		}
		
		if (p_basicLinuxACLdata.otherPermission.writeFlag)
		{
			receivedACLs = receivedACLs|S_IWOTH;
		}
		
		if (p_basicLinuxACLdata.otherPermission.executeFlag)
		{
			receivedACLs = receivedACLs|S_IXOTH;
		}

		switch(p_special_perm){
		
		case 1:
			receivedACLs = receivedACLs|S_ISVTX;
			break;
			
		case 2:
			receivedACLs = receivedACLs|S_ISGID;
                        break;
		case 3:
			receivedACLs = receivedACLs|S_ISVTX;
			receivedACLs = receivedACLs|S_ISGID;
			break;
		case 4:
			receivedACLs = receivedACLs|S_ISUID;
			break;
		case 5:
			receivedACLs = receivedACLs|S_ISVTX;
			receivedACLs = receivedACLs|S_ISUID;
			break;

		case 6:
			receivedACLs = receivedACLs|S_ISGID;
			receivedACLs = receivedACLs|S_ISUID;
			break;

		case 7:
			receivedACLs = receivedACLs|S_ISVTX;
			receivedACLs = receivedACLs|S_ISGID;
			receivedACLs = receivedACLs|S_ISUID;
			break;

		default:
			break;

		}

		int res = chmod(p_FilePath,receivedACLs);

		char msg[80] ="";

		if ( res != 0 ) {
			syslog (LOG_ERR, "chmod failed!" );
			ACE_OS::strcpy(p_MessageBuffer,msg);
			return ACS_APGCC_CLIB_FAILURE;
		} 	

		return ACS_APGCC_CLIB_SCCCESS;
}


ACS_APGCC_HwVer_ReturnTypeT ACS_APGCC_CommonLib::GetHWVersion( char* p_hwVersion, int &p_Len ){

	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	char value[1024] = {0};

	GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );

	if(hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
		switch(hwInfo.hwVersion){
		case ACS_APGCC_HWVER_GEP1:
			strncpy(value,ACS_APGCC_HWVER_APG43_GEP1_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP1_STRING));
			break;

		case ACS_APGCC_HWVER_GEP2:
			strncpy(value,ACS_APGCC_HWVER_APG43_GEP2_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP2_STRING));
			break;

		case ACS_APGCC_HWVER_GEP5:
			strncpy(value,ACS_APGCC_HWVER_APG43_GEP5_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP5_STRING));
			break;
		
		case ACS_APGCC_HWVER_GEP7:
			strncpy(value,ACS_APGCC_HWVER_APG43_GEP7_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP7_STRING));
			break;

		case ACS_APGCC_HWVER_VM:
		default:
			syslog(LOG_ERR, "ERROR: GetHWVersion() - Undefined Hardware version. Hardware Version received: [%s]", value);
			return ACS_APGCC_UNDEFINED_HWVER;
		}

		int HwVer_IMM_Len = ACE_OS::strlen(value);

		if (p_Len < HwVer_IMM_Len){
			p_Len = HwVer_IMM_Len;
			syslog(LOG_ERR,"Output buffer length too small to hold the required value");
			return ACS_APGCC_STRING_BUFFER_TOO_SMALL;
		}

		ACE_OS::strcpy(p_hwVersion,value);
		p_Len= HwVer_IMM_Len;

	}else{
		syslog(LOG_ERR, "ERROR: GetHWVersion() - Undefined Hardware version. Hardware Version received: [%s]", value);
		return ACS_APGCC_UNDEFINED_HWVER;
	}

	return ACS_APGCC_HWVER_SUCCESS;
}


ACS_APGCC_HwVer_ReturnTypeT ACS_APGCC_CommonLib::GetHWVariant( char* p_hwVariant, int &p_Len ){

	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	char value[1024] = {0};

	GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION | ACS_APGCC_GET_DISKCAPACITY );

	if(hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
		switch(hwInfo.hwVersion){
		case ACS_APGCC_HWVER_GEP1:
			strncpy(value,ACS_APGCC_HWVER_APG43_GEP1_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP1_STRING));
			break;

		case ACS_APGCC_HWVER_GEP2:
			strncpy(value,ACS_APGCC_HWVER_APG43_GEP2_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP2_STRING));
			break;

		case ACS_APGCC_HWVER_GEP5:
			if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_400GB)
				strncpy(value,ACS_APGCC_HWVER_APG43_GEP5_400STRING,strlen(ACS_APGCC_HWVER_APG43_GEP5_400STRING));
			else
				strncpy(value,ACS_APGCC_HWVER_APG43_GEP5_1200STRING,strlen(ACS_APGCC_HWVER_APG43_GEP5_1200STRING));
			break;
		case ACS_APGCC_HWVER_GEP7:
			if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_400GB)
				strncpy(value,ACS_APGCC_HWVER_APG43_GEP7L_400STRING,strlen(ACS_APGCC_HWVER_APG43_GEP7L_400STRING));
			else
				strncpy(value,ACS_APGCC_HWVER_APG43_GEP7L_1600STRING,strlen(ACS_APGCC_HWVER_APG43_GEP7L_1600STRING));
			break;

		case ACS_APGCC_HWVER_VM:
		default:
			syslog(LOG_ERR, "ERROR: GetHWVersion() - Undefined Hardware version. Hardware Version received: [%s]", value);
			return ACS_APGCC_UNDEFINED_HWVER;
		}

		int HwVer_IMM_Len = ACE_OS::strlen(value);

		if (p_Len < HwVer_IMM_Len){
			p_Len = HwVer_IMM_Len;
			syslog(LOG_ERR,"Output buffer length too small to hold the required value");
			return ACS_APGCC_STRING_BUFFER_TOO_SMALL;
		}

		p_Len= HwVer_IMM_Len;
		ACE_OS::strcpy(p_hwVariant,value);

	}else{
		syslog(LOG_ERR, "ERROR: GetHWVersion() - Undefined Hardware version. Hardware Version received: [%s]", value);
		return ACS_APGCC_UNDEFINED_HWVER;
	}

	return ACS_APGCC_HWVER_SUCCESS;
}


ACS_APGCC_DNFPath_ReturnTypeT ACS_APGCC_CommonLib::GetDataDiskPath(const char* p_LogicalName, char* p_Path,int &p_Len){

	ACS_CC_ImmParameter paramToFind;
	OmHandler omHandler;
	char dn[1024] = {0};

	ACE_OS::strcpy(dn,INSTANCE_DN_DATADISK_STRING);
	ACE_OS::strcat(dn,p_LogicalName);
	ACE_OS::strcat(dn,",");
	ACE_OS::strcat(dn,PARENT_OBJECT_DataDisk_DN);

	/*dn is now generated based on logical name*/

	syslog(LOG_INFO,"the generated logical name is : [ %s]",dn);

	if ( omHandler.Init() == ACS_CC_FAILURE) {
		syslog(LOG_ERR,"ERROR: omHandler.Init FAILURE");
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	//fetching instances and their attributes

	paramToFind.attrName = const_cast<char*>("dataDiskLogName");
	
	if( omHandler.getAttribute(dn,&paramToFind ) != ACS_CC_SUCCESS ) {
		syslog(LOG_ERR,"ERROR: 	omHandler.getAttribute 1 Failed!");
		omHandler.Finalize();
		return ACS_APGCC_FAULT_LOGICAL_NAME;
	}

	if ( paramToFind.attrValuesNum == 0) {
		syslog(LOG_ERR,"ERROR: dataDiskLogName Attribute has not been set the value!");
		omHandler.Finalize();
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	char *value = (reinterpret_cast<char*>(*(paramToFind.attrValues)));

	int lname_IMM_Len = ACE_OS::strlen(value);
	int lname_input_Len = ACE_OS::strlen(p_LogicalName);

	if( lname_IMM_Len == lname_input_Len ){

		if ((ACE_OS::strcmp(p_LogicalName,value)) == 0 ) {

			paramToFind.attrName = const_cast<char*>("dataDiskPath");

			if( omHandler.getAttribute(dn,&paramToFind ) != ACS_CC_SUCCESS ) {
				syslog(LOG_ERR,"ERROR: 	omHandler.getAttribute 2 Failed!");
				omHandler.Finalize();
				return ACS_APGCC_DNFPATH_FAILURE;
			}

			if ( paramToFind.attrValuesNum == 0) {
				syslog(LOG_ERR,"ERROR: dataDiskPath Attribute has not been set the value!");
				omHandler.Finalize();
				return ACS_APGCC_DNFPATH_FAILURE;
			}

			char *value2 = (reinterpret_cast<char *>(*(paramToFind.attrValues)));

			int lname_IMM_Len  = ACE_OS::strlen(value2);
				
			syslog(LOG_INFO,"retrieved value: [ %s]",value2);
			syslog(LOG_INFO,"value length: [ %d]",lname_IMM_Len);

			//checking if length of output buffer is less than the required buffer length
			if (p_Len < lname_IMM_Len){
				p_Len = lname_IMM_Len; //Assign length of buffer required to hold the output path string
				syslog(LOG_ERR,"ERROR: Output String Buffer Length too small to hold the retrieved value");
				omHandler.Finalize();
				return ACS_APGCC_STRING_BUFFER_SMALL; //if buffer length is less, then return -2
			}
			/*Assign length of the attribute to p_Len */
			p_Len = lname_IMM_Len;

			/* Assign value of the attribute to p_Path */
			ACE_OS::strncpy(p_Path,value2,lname_IMM_Len);
			p_Path[lname_IMM_Len]='\0';
			syslog(LOG_INFO,"value assigned: [ %s]",p_Path);

		}//end of if block

	}//end of if block
				

	if (omHandler.Finalize() == ACS_CC_FAILURE) {
		syslog(LOG_ERR,"ERROR: Finalization FAILURE");
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	return ACS_APGCC_DNFPATH_SUCCESS;
}


ACS_APGCC_DNFPath_ReturnTypeT ACS_APGCC_CommonLib::GetDataDiskPathForCp(const char* p_LogicalName, unsigned int cpId, char* p_Path, int &p_Len){

	ACS_CC_ImmParameter paramToFind;
	OmHandler omHandler;
	char dn[1024] = {0};

	/* get the cp id string */
	char str[200] = {0};
	//retrieve default CP Name from cpId

	if ( ((int)cpId >= 0) && (cpId < 1000) )			// Single sided CP (blade)
	{
		char strCpId[6] = {0};
		sprintf(strCpId,"%u",cpId);
		strcpy(str,"/bc");
		ACE_OS::strcat(str,strCpId);
	}
	else if ( (cpId >= 1000) && (cpId < 2000) )	// Double sided CP (SPX)
	{
		char strCpId[6];
		sprintf(strCpId,"%u",(cpId - 1000));
		strcpy(str,"/cp");
		ACE_OS::strcat(str,strCpId);
	}
	else
	{
		// Failure if this is reached
		return ACS_APGCC_FAULTY_CPID;
	}

	ACE_OS::strcpy(dn,INSTANCE_DN_DATADISK_STRING);
	ACE_OS::strcat(dn,p_LogicalName);
	ACE_OS::strcat(dn,",");
	ACE_OS::strcat(dn,PARENT_OBJECT_DataDisk_DN);

	/*dn is now generated based on logical name*/
	syslog(LOG_INFO,"the generated logical name is : [ %s]",dn);

	if (omHandler.Init() == ACS_CC_FAILURE) {
		syslog(LOG_ERR,"ERROR: omHandler.Init FAILURE");
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	//fetching instances and their attributes

	paramToFind.attrName = const_cast<char*>("dataDiskLogName");
	
	if( omHandler.getAttribute(dn,&paramToFind ) != ACS_CC_SUCCESS ) {
		syslog(LOG_ERR,"ERROR: 	omHandler.getAttribute 1 Failed!");
		omHandler.Finalize();
		return ACS_APGCC_FAULT_LOGICAL_NAME;
	}

	if ( paramToFind.attrValuesNum == 0) {
		syslog(LOG_ERR,"ERROR: dataDiskLogName Attribute has not been set the value!");
		omHandler.Finalize();
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	char *value = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
	int lname_IMM_Len = ACE_OS::strlen(value);
	int lname_input_Len = ACE_OS::strlen(p_LogicalName);

	if ( lname_IMM_Len==lname_input_Len ){

		if ((ACE_OS::strcmp(p_LogicalName,value)) == 0)	{

			paramToFind.attrName = const_cast<char*>("dataDiskPath");

			if( omHandler.getAttribute(dn,&paramToFind ) != ACS_CC_SUCCESS ) {
				syslog(LOG_ERR,"ERROR: 	omHandler.getAttribute 2 Failed!");
				omHandler.Finalize();
				return ACS_APGCC_DNFPATH_FAILURE;
			}

			if ( paramToFind.attrValuesNum == 0) {
				syslog(LOG_ERR,"ERROR: dataDiskPath Attribute has not been set the value!");
				omHandler.Finalize();
				return ACS_APGCC_DNFPATH_FAILURE;
			}

			char *value2 = (reinterpret_cast<char *>(*(paramToFind.attrValues)));
			syslog(LOG_INFO,"retrieved value: [ %s]",value2);

			//checking if length of output buffer is less than the required buffer length

			ACE_OS::strcat(value2,str);
			int lname_IMM_Len = ACE_OS::strlen(value2);
			syslog(LOG_INFO,"value length: [ %d]",lname_IMM_Len);

			if (p_Len < lname_IMM_Len){
				p_Len = lname_IMM_Len; //Assign length of buffer required to hold the output path string
				syslog(LOG_ERR,"ERROR: Output String Buffer Length too small to hold the retrieved value");
				omHandler.Finalize();
				return ACS_APGCC_STRING_BUFFER_SMALL; //if buffer length is less, then return -2
			}
			/*Assign length of the attribute to p_Len */
			p_Len = lname_IMM_Len;

			/* Assign value of the attribute to p_Path */
			ACE_OS::strncpy(p_Path,value2,lname_IMM_Len);

			p_Path[lname_IMM_Len]='\0';
			syslog(LOG_INFO,"value assigned: [ %s]",p_Path);

		}//end of if block
	}//end of if block
				

	if (omHandler.Finalize() == ACS_CC_FAILURE) {
		syslog(LOG_ERR,"ERROR: Finalization FAILURE");
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	return ACS_APGCC_DNFPATH_SUCCESS;
}


ACS_APGCC_DNFPath_ReturnTypeT ACS_APGCC_CommonLib::GetFTPVirtualPath(const char* p_LogicalName,
								  char* p_VirtualDir, int &p_Len) {

                char hwVersion[200];
                int p_hw_Len=10;
		int hwver_status = 1;
                ACS_APGCC_HwVer_ReturnTypeT errorCode;
                syslog(LOG_INFO,"Calling GetHWVersion API");
                //log.Write("ACS_APGCC_CommonLib:INFO: Calling GetHWVersion API",LOG_LEVEL_INFO);
		errorCode = GetHWVersion (  hwVersion,
                                                p_hw_Len );

		if ( errorCode == ACS_APGCC_HWVER_SUCCESS )
                {
                        int len = strlen(hwVersion);
			if (((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP1_STRING,len)) == 0) || 
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP2_STRING,len)) == 0) ||
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP4_400STRING,len))==0)||
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP4_1600STRING,len))==0) ||
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP5_400STRING,len))==0) ||
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP5_1200STRING,len))==0) ||
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP7L_400STRING,len))==0) ||
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP7L_1600STRING,len))==0) ||
                            ((ACE_OS::strncmp(hwVersion,ACS_APGCC_HWVER_APG43_GEP7_128_1600STRING,len))==0)){

			        hwver_status = 0;
                        }
                        else
                        {
                                syslog(LOG_INFO,"this is not GEP1,GEP2,GEP4,GEP5 or GEP7 the hardware version is [ %s]",hwVersion);
                                //sprintf(buffer,"ACS_APGCC_CommonLib:INFO: this is not GEP1 or GEP2, the hardware version is [ %s]",hwVersion);
                                //log.Write(buffer,LOG_LEVEL_INFO);
				return ACS_APGCC_DNFPATH_FAILURE;
                        }
                }
                else
                {
                        syslog(LOG_INFO,"error in retrieving hardware version");
                        //log.Write("ACS_APGCC_CommonLib:ERROR: error in retrieving hardware version",LOG_LEVEL_ERROR);
			return ACS_APGCC_DNFPATH_FAILURE;
                }

               	ACS_CC_ImmParameter paramToFind;
               	OmHandler omHandler;
               	char dn[1024] = {0};

               	ACE_OS::strcpy(dn,INSTANCE_DN_FTPVIRTUAL_STRING);
               	ACE_OS::strcat(dn,p_LogicalName);
               	ACE_OS::strcat(dn,",");
               	ACE_OS::strcat(dn,PARENT_OBJECT_FTPVirtual_DN);

               	/*dn is now generated based on logical name*/
               	syslog(LOG_INFO,"the generated logical name is : [ %s]",dn);
		//sprintf(buffer,"ACS_APGCC_CommonLib:INFO: the generated logical name is : [ %s]",dn);
		//log.Write(buffer,LOG_LEVEL_INFO);

               	// OmHandler init.
               	if (omHandler.Init() == ACS_CC_FAILURE) {
                       	syslog(LOG_ERR,"ERROR: omHandler.Init FAILURE");
                       	//log.Write("ACS_APGCC_CommonLib:ERROR: omHandler.Init FAILURE",LOG_LEVEL_ERROR);
			return ACS_APGCC_DNFPATH_FAILURE;
               	}

               	//fetching instances and their attributes

               	paramToFind.attrName = const_cast<char*>("ftpLogicalName");

               	if( omHandler.getAttribute(dn,&paramToFind ) != ACS_CC_SUCCESS ) {
                       	syslog(LOG_ERR,"ERROR:  omHandler.getAttribute 1 Failed!");
			//log.Write("ACS_APGCC_CommonLib:ERROR: omHandler.getAttribute 1 Failed!",LOG_LEVEL_ERROR);
                       	omHandler.Finalize();
                       	return ACS_APGCC_FAULT_LOGICAL_NAME;
               	}

               	if ( paramToFind.attrValuesNum == 0) {
                       	syslog(LOG_ERR,"ERROR: ftpvirtualpath Attribute has not been set the value!");
                       	//log.Write("ACS_APGCC_CommonLib:ERROR: ftpvirtualpath Attribute has not been set the value!",LOG_LEVEL_ERROR);
			omHandler.Finalize();
                       	return ACS_APGCC_DNFPATH_FAILURE;
               	}

               	char *value = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
               	int lname_IMM_Len = ACE_OS::strlen(value);
               	int lname_input_Len = ACE_OS::strlen(p_LogicalName);


            	if(lname_IMM_Len==lname_input_Len)
                {

                       	if ((ACE_OS::strcmp(p_LogicalName,value)) == 0)
 			{
                               	paramToFind.attrName = const_cast<char*>("ftpVirtualDirPath");

                               	if( omHandler.getAttribute(dn,&paramToFind ) != ACS_CC_SUCCESS ) {
                                       	syslog(LOG_ERR,"ERROR:  omHandler.getAttribute 2 Failed!");
                                       	//log.Write("ACS_APGCC_CommonLib:ERROR: omHandler.getAttribute 2 Failed!",LOG_LEVEL_ERROR);
					omHandler.Finalize();
                                     	return ACS_APGCC_DNFPATH_FAILURE;
                               	}

                               	char *value2 = (reinterpret_cast<char *>(*(paramToFind.attrValues)));
                               	int lname_IMM_Len = ACE_OS::strlen(value2);
			
				syslog(LOG_INFO,"retrieved value: [ %s]",value2);
       		        	syslog(LOG_INFO,"value length: [ %d]",lname_IMM_Len);
				//sprintf(buffer,"ACS_APGCC_CommonLib:INFO: retrieved value: [ %s]",value2);
                                //log.Write(buffer,LOG_LEVEL_INFO);


                               	//checking if length of output buffer is less than the required buffer length
                               	if (p_Len < lname_IMM_Len)
                               	{
                               		p_Len = lname_IMM_Len; //Assign length of buffer required to hold the output path string
                                	syslog(LOG_ERR,"ERROR: Output String Buffer Length too small to hold the retrieved value");
                                   	//log.Write("ACS_APGCC_CommonLib:ERROR:  Output String Buffer Length too small to hold the retrieved value",LOG_LEVEL_ERROR);
					omHandler.Finalize();
                                   	return ACS_APGCC_STRING_BUFFER_SMALL; //if buffer length is less, then return -2
                                }
                                /*Assign length of the attribute to p_Len*/
                                p_Len = lname_IMM_Len;

                               	/* Assign value of the attribute to p_VirtualDir */
                               	ACE_OS::strncpy(p_VirtualDir,value2,lname_IMM_Len);
				p_VirtualDir[lname_IMM_Len]='\0';
				
				syslog(LOG_INFO,"value assigned : [ %s]",p_VirtualDir);

                        }//end of if block

                }//end of if block


                if (omHandler.Finalize() == ACS_CC_FAILURE) {
                       	syslog(LOG_ERR,"ERROR: Finalization FAILURE");
			//log.Write("ACS_APGCC_CommonLib:ERROR: Finalization FAILURE",LOG_LEVEL_ERROR);
                       	return ACS_APGCC_DNFPATH_FAILURE;
                }
                return ACS_APGCC_DNFPATH_SUCCESS;
		

}

		     	
ACS_APGCC_DNFPath_ReturnTypeT ACS_APGCC_CommonLib::GetFileMPath(const char* p_FileMFuncName,char* p_Path,int &p_Len){

	char dn[] = "AxeNbiFoldersnbiFoldersMId=1";
	char attributename[1024] = {0};
	char baseattribute[]="internalRoot";

	ACS_CC_ImmParameter paramToFind1;
	ACS_CC_ImmParameter paramToFind2;

	OmHandler omHandler;
	ACE_OS::strcpy(attributename,p_FileMFuncName);
	
	if (omHandler.Init() == ACS_CC_FAILURE) {
		syslog(LOG_ERR,"ERROR: omHandler.Init FAILURE");
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	paramToFind1.attrName = baseattribute;

	if( omHandler.getAttribute(dn,&paramToFind1 ) != ACS_CC_SUCCESS ) {
		syslog(LOG_ERR,"ERROR:  omHandler.getAttribute 1 Failed!");
		omHandler.Finalize();
		return ACS_APGCC_FAULT_LOGICAL_NAME;
	}

	if ( paramToFind1.attrValuesNum == 0) {
		syslog(LOG_ERR,"ERROR: internalRoot Attribute has not been set the value!");
		omHandler.Finalize();
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	char internalpathvalue[100] = {0};
	int baseattrib_len = ACE_OS::strlen(reinterpret_cast<char*>(*(paramToFind1.attrValues)));
	strncpy(internalpathvalue,(reinterpret_cast<char*>(*(paramToFind1.attrValues))),baseattrib_len);
	internalpathvalue[baseattrib_len]='\0';

	syslog(LOG_INFO,"INFO:	internal root path is [%s] and length is [%d]",internalpathvalue,baseattrib_len);

	//check if attribute needed is "internalroot" itself

	if (strncmp(attributename,baseattribute,strlen(baseattribute)) == 0) {
		syslog(LOG_INFO,"INFO:	attribute name same as internal root");

		/*Assign length of the attribute to p_Len*/
		p_Len = baseattrib_len;

		/* Assign value of the attribute to p_Path */
		ACE_OS::strcpy(p_Path,internalpathvalue);
		syslog(LOG_INFO,"INFO:  p_path value is [%s]",p_Path);

		omHandler.Finalize();
		return ACS_APGCC_DNFPATH_SUCCESS;
	}

	paramToFind2.attrName = attributename;

	if( omHandler.getAttribute(dn,&paramToFind2 ) != ACS_CC_SUCCESS ) {
		syslog(LOG_ERR,"ERROR:  omHandler.getAttribute 2 Failed!");
		omHandler.Finalize();
		return ACS_APGCC_FAULT_LOGICAL_NAME;
	}

	if ( paramToFind2.attrValuesNum == 0) {
		syslog(LOG_ERR,"ERROR: FileM path Attribute has not been set the value!");
		omHandler.Finalize();
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	char pathvalue[100] = {0};
	int fileM_imm_len = ACE_OS::strlen(reinterpret_cast<char*>(*(paramToFind2.attrValues)));
	strncpy(pathvalue,(reinterpret_cast<char*>(*(paramToFind2.attrValues))),fileM_imm_len);
	pathvalue[fileM_imm_len]='\0';

	int tot_len = fileM_imm_len + baseattrib_len;

	syslog(LOG_INFO,"INFO:  path value is [%s] and length is [%d]",pathvalue,fileM_imm_len);

	if (p_Len < tot_len){
		p_Len = tot_len; //Assign length of buffer required to hold the output path string
		syslog(LOG_ERR,"ERROR: Output String Buffer Length too small to hold the retrieved value");
		omHandler.Finalize();
		return ACS_APGCC_STRING_BUFFER_SMALL; //if buffer length is less, then return -2
	}

	/*Assign length of the attribute to p_Len*/
	p_Len = tot_len;

	/* Assign value of the attribute to p_Path */

	ACE_OS::strcpy(p_Path,internalpathvalue);
	ACE_OS::strcat(p_Path,pathvalue);

	syslog(LOG_INFO,"value assigned : [ %s]",p_Path);

	if (omHandler.Finalize() == ACS_CC_FAILURE) {
		syslog(LOG_ERR,"ERROR: Finalization FAILURE");
		return ACS_APGCC_DNFPATH_FAILURE;
	}

	return ACS_APGCC_DNFPATH_SUCCESS;
}


ACS_APGCC_EncrptDecryptData_ReturnTypeT ACS_APGCC_CommonLib::EncryptDecryptData(const char* p_Input, 
								   const char* CommandLine,
								   char* p_Output,
								   const char* p_Key,
								   ACS_APGCC_DIRFlagT p_dFlag,
							 	   int p_dataLen, 
							   int p_kLen) {

	keyInstance    *ki = new keyInstance;
	cipherInstance ci, ci1;
	int i,j,k,len;
	unsigned char *p1, *p2;
	//unsigned long randSeed=0x12345678;
	SHAContext sha;
	char const *hash;
	int keySize, errnum;
	//int size = 1048576;
	char ivString[] = "1E98F9F663374FA20039EC07F1467F37", *point;
	char commandLine[PATH_MAX] = {0};
	char szCXCNo[PATH_MAX] = {0};

	ACE_OS::sprintf(szCXCNo, "EncryptDecryptData");
	//OpenLogFile();

	
	if (p_dFlag != ACS_APGCC_DIR_ENCRYPT && p_dFlag != ACS_APGCC_DIR_DECRYPT)
	{
		syslog (LOG_ERR, "Bad direction: [ %d ]",p_dFlag);
		//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: Bad direction: [ %d ]",p_dFlag);
		//log.Write(buffer,LOG_LEVEL_ERROR);
		if(ki != NULL){
			delete ki;
			ki = NULL;
		}
		return ACS_APGCC_BAD_KEY_DIR;
	}

	if (!p_dataLen)
	{
		syslog (LOG_ERR, "No data sent");
		//log.Write("ACS_APGCC_CommonLib:ERROR: No data sent",LOG_LEVEL_ERROR);
		if(ki != NULL){
			delete ki;
			ki = NULL;
		}
		return ACS_APGCC_BAD_PARAMS;
	}

	/* A MD of the command line will be used as part of the key */

        /* For now we will just use a fixed string */
	ACE_OS::strcpy(commandLine, CommandLine);
	point = commandLine;
	while (*point != '.' && *point != '\0')
		point++;

	*point = '\0';

#ifdef ACSSEC_ENABLE_DEBUG
	if (p_kLen < 500)
	{
		syslog (LOG_ERR,"User supplied keydata: %s", p_Key );
		//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: User supplied keydata: %s", p_Key );
		//log.Write(buffer.LOG_LEVEL_ERROR);
	}
	else
	{
		syslog (LOG_ERR,"User supplied keydata: To big to print");
		//log.Write("ACS_APGCC_CommonLib:ERROR: User supplied keydata: To big to print",LOG_LEVEL_ERROR);
	}
	syslog (LOG_ERR,"Commandline supplied keydata: %s", commandLine);
	if (p_dFlag == ACS_APGCC_DIR_ENCRYPT)
		syslog (LOG_ERR,"Encrypting");
		//log.Write("ACS_APGCC_CommonLib:INFO: Encrypting",LOG_LEVEL_INFO);
	else if (p_dFlag == ACS_APGCC_DIR_DECRYPT)
		syslog (LOG_ERR,"Decrypting");
		//log.Write("ACS_APGCC_CommonLib:INFO: Decrypting",LOG_LEVEL_INFO);
	else
	{
		syslog (LOG_ERR,"dir ==[ %d]", p_dFlag);
		//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: dir ==[ %d]", p_dFlag);
		//log.Write(buffer,LOG_LEVEL_ERROR);
	}
	syslog (LOG_ERR,"Datalength:[ %d], Keylength:[ %d]", p_dataLen, p_kLen);
	//sprinf(buffer,"ACS_APGCC_CommonLib:INFO: Datalength:[ %d], Keylength:[ %d]", p_dataLen, p_kLen);
	//log.Write(buffer,LOG_LEVEL_ERROR);
#endif

	for (k=KEY_BITS_0;k<=MAX_KEY_BITS;k+=STEP_KEY_BITS)
	{
		if (setRounds(k,16) != TRUE)
		{
			syslog (LOG_ERR,"Invalid value for numRounds");
			//log.Write("ACS_APGCC_CommonLib:ERROR: Invalid value for numRounds",LOG_LEVEL_ERROR);
			if(ki != NULL){
				delete ki;
				ki = NULL;
			}
			return ACS_APGCC_BAD_ROUNDS;
		}
	}

	/* Used for even multiples of 128 bit blocks */
	if ((errnum = cipherInit(&ci, MODE_CBC, ivString)) != TRUE)
	{
		syslog (LOG_ERR,"cipherInit[ci] error:[ %d]", errnum);
		//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: cipherInit[ci] error:[ %d]", errnum);
		//log.Write(buffer,LOG_LEVEL_ERROR);
		if(ki != NULL){
			delete ki;
			ki = NULL;
		}
		return (ACS_APGCC_EncrptDecryptData_ReturnTypeT)errnum;
	}

	/* Used for the remaining bits, if any */
	if ((errnum = cipherInit(&ci1, MODE_CFB1, ivString)) != TRUE)
	{
		syslog (LOG_ERR,"cipherInit[ci] error:[ %d]", errnum);
		//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: cipherInit[ci] error:[ %d]", errnum);
		//log.Write(buffer,LOG_LEVEL_ERROR);
		if(ki != NULL){
			delete ki;
			ki = NULL;
		}
		return (ACS_APGCC_EncrptDecryptData_ReturnTypeT)errnum;
	}

	keySize = MAX_KEY_BITS;
	memset(ki->keyMaterial, 0, sizeof(ki->keyMaterial));

	/* Calculate 160 bits of key-material */
	shaInit(&sha);
	shaUpdate(&sha, p_Key, p_kLen);
	hash = (char const *) shaFinal(&sha);

	for (j = 0; j < SHA_HASHBYTES; j++)
		ACE_OS::sprintf(ki->keyMaterial, "%s%02x", ki->keyMaterial, hash[j]);

	/* Calculate the remaining 96 bits */
	shaInit(&sha);
	shaUpdate(&sha, commandLine, strlen(commandLine));
	hash = (char const *) shaFinal(&sha);

	i = 0;
	for (j = ACE_OS::strlen(ki->keyMaterial); j*4 < keySize; j += 2)
	{
		ACE_OS::sprintf(ki->keyMaterial, "%s%02x", ki->keyMaterial, hash[i]);
		i++;
	}

#ifdef ACSSEC_ENABLE_DEBUG
	syslog (LOG_ERR,"Final SHA-1 keymaterial:[ %s]", ki->keyMaterial);
	//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: Final SHA-1 keymaterial:[ %s]", ki->keyMaterial);
	//log.Write(buffer,LOG_LEVEL_ERROR);
#endif

	if ((errnum = makeKey(ki, p_dFlag, keySize, ki->keyMaterial)) != TRUE)
	{
		syslog (LOG_ERR,"Error parsing key,[ %d]", errnum);
		//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: Error parsing key,[ %d]", errnum);
		//log.Write(buffer,LOG_LEVEL_ERROR);
		if(ki != NULL){
			delete ki;
			ki = NULL;
		}
		return (ACS_APGCC_EncrptDecryptData_ReturnTypeT)errnum;
	}

	reKey(ki);

	/* Calculate the number of bytes to encrypt/decrypt in CBC mode, multiple of 128-bit
	   blocks */
	len = p_dataLen-(p_dataLen % (BLOCK_SIZE/8));
	i = p_dataLen - len;
	p1 = (unsigned char *) p_Input;
	p2 = (unsigned char *) p_Output;
	if (len)
	{
		#ifdef ACSSEC_ENABLE_DEBUG
			syslog (LOG_ERR,"Mode CBC for block of[ %d] bytes", len);
			//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: Mode CBC for block of[ %d] bytes", len);
			//log.Write(buffer,LOG_LEVEL_ERROR);
		#endif
		if (p_dFlag == ACS_APGCC_DIR_ENCRYPT)
		{
			if ((j = blockEncrypt(&ci, ki, p1, len*8, p2)) != len*8)
			{
				syslog (LOG_ERR,"blockEncrypt:[ %d], len =[ %d], i =[ %d], datalenth [ %d]", j, len, i, p_dataLen);
				//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: blockEncrypt:[ %d], len =[ %d], i =[ %d], datalenth [ %d]", j, len, i, p_dataLen);
				//log.Write(buffer,LOG_LEVEL_ERROR);
				if(ki != NULL){
					delete ki;
					ki = NULL;
				}
				return (ACS_APGCC_EncrptDecryptData_ReturnTypeT)j;
			}
		}
		else
		{
			if ((j = blockDecrypt(&ci, ki, p1, len*8, p2)) != len*8)
			{
				syslog (LOG_ERR,"blockDecrypt:[ %d], len =[ %d], i =[ %d], datalenth [ %d]", j, len, i, p_dataLen);
				//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: blockDecrypt:[ %d], len =[ %d], i =[ %d], datalenth [ %d]", j, len, i, p_dataLen);
				//log.Write(buffer,LOG_LEVEL_ERROR);
				if(ki != NULL){
					delete ki;
					ki = NULL;
				}
				return (ACS_APGCC_EncrptDecryptData_ReturnTypeT)j;
			}
		}
	}
	if (i)
	{
#ifdef ACSSEC_ENABLE_DEBUG
		syslog (LOG_ERR,"Mode CFB-1 for block of [ %d] bytes", i);
		//sprintf(buffer,"Mode CFB-1 for block of [ %d] bytes", i);
		//log.Write(buffer,LOG_LEVEL_ERROR);
#endif
		while (len)
		{
			*p1++; *p2++; len--;
		}
		if (p_dFlag == ACS_APGCC_DIR_ENCRYPT)
		{
			if ((j = blockEncrypt(&ci1, ki, p1, i*8, p2)) != i*8)
			{
				syslog (LOG_ERR,"blockEncrypt:[ %d], i =[ %d], datalength =[ %d]", j, i, p_dataLen);
				//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: blockEncrypt:[ %d], i =[ %d], datalength =[ %d]", j, i, p_dataLen);
                		//log.Write(buffer,LOG_LEVEL_ERROR);
				if(ki != NULL){
					delete ki;
					ki = NULL;
				}
				return (ACS_APGCC_EncrptDecryptData_ReturnTypeT)j;
			}
		}
		else
		{
			if ((j = blockDecrypt(&ci1, ki, p1, i*8, p2)) != i*8)
			{
				syslog (LOG_ERR,"blockDecrypt:[ %d], i =[ %d], datalength =[ %d]", j, i, p_dataLen);
				//sprintf(buffer,"ACS_APGCC_CommonLib:ERROR: blockEncrypt:[ %d], i =[ %d], datalength =[ %d]", j, i, p_dataLen);
				//log.Write(buffer,LOG_LEVEL_ERROR);
				if(ki != NULL){
					delete ki;
					ki = NULL;
				}
				return (ACS_APGCC_EncrptDecryptData_ReturnTypeT)j;
			}
		}
	}
	if(ki != NULL){
		delete ki;
		ki = NULL;
	}
	return ACS_APGCC_ENCRPT_DECRPT_SUCCESS;
}

ACS_APGCC_Roles_ReturnTypeT ACS_APGCC_CommonLib::GetRoles(const string &user, string &strOutRoles){

	ACS_APGCC_Roles_ReturnTypeT retCode = ACS_APGCC_ERR_ERROR;
	ACS_APGCC_CommonUtil *pCommonUtil = NULL;

	ACS_TRA_Logging _log;
	char bufferLog[APGCC_LOG_BUFFER] = {0};

	pCommonUtil = new ACS_APGCC_CommonUtil();

	/* open log */
    _log.Open("APGCC");

	sprintf(bufferLog, "[%s:][%d:] user: %s",__FUNCTION__,__LINE__, user.c_str());
	_log.Write(bufferLog,LOG_LEVEL_DEBUG);

	if (pCommonUtil->getUserRoles(user, strOutRoles))
	{
		retCode = ACS_APGCC_ROLES_SUCCESS;
	}
	if(pCommonUtil != NULL){
		delete pCommonUtil;
		pCommonUtil = NULL;
	}

	/* close log */
	_log.Close();
	// exit
	return retCode;
}


ACS_APGCC_Roles_ReturnTypeT ACS_APGCC_CommonLib::CheckUser(const string &user, const string &password){

	ACS_APGCC_Roles_ReturnTypeT retCode = ACS_APGCC_ROLES_SUCCESS;

	ACS_APGCC_CommonUtil *pCommonUtil = NULL;

	ACS_TRA_Logging _log;
	char bufferLog[APGCC_LOG_BUFFER] = {0};


    _log.Open("APGCC");

	sprintf(bufferLog,"[%s:][%d:] user: %s",__FUNCTION__,__LINE__, user.c_str());
	_log.Write(bufferLog,LOG_LEVEL_DEBUG);

	pCommonUtil = new ACS_APGCC_CommonUtil();

	retCode= pCommonUtil->authenticate("sshd", user.c_str(), password.c_str());

	if(pCommonUtil != NULL){
		delete pCommonUtil;
		pCommonUtil = NULL;
	}


	_log.Close();

	return retCode;
}

ACS_APGCC_Roles_ReturnTypeT ACS_APGCC_CommonLib::GetCachedRoles(const string &user, string &strOutRoles){

	ACS_APGCC_Roles_ReturnTypeT retCode = ACS_APGCC_ERR_ERROR;
	ACS_APGCC_CommonUtil *pCommonUtil = NULL;

	ACS_TRA_Logging _log;
	char bufferLog[APGCC_LOG_BUFFER] = {0};

	pCommonUtil = new ACS_APGCC_CommonUtil();

	/* open log */
    _log.Open("APGCC");

	sprintf(bufferLog, "[%s:][%d:] user: %s",__FUNCTION__,__LINE__, user.c_str());
	_log.Write(bufferLog,LOG_LEVEL_DEBUG);

	retCode = pCommonUtil->getRolesfromMapUser(user,strOutRoles);

	if(pCommonUtil != NULL){
		delete pCommonUtil;
		pCommonUtil = NULL;
	}

	/* close log */
	_log.Close();
	// exit
	return retCode;
}

int ACS_APGCC_CommonLib::GetHwInfo ( ACS_APGCC_HWINFO* p_hwInfo, ACS_APGCC_HWINFO_RESULT* p_hwInfoResult, unsigned int apgcc_hwinfo_flag ){

	std::string apos_command = GetHwInfo_Path;

	p_hwInfoResult->disksCapacityResult = ACS_APGCC_HWINFO_FAILURE;
	p_hwInfoResult->disksLocataionResult = ACS_APGCC_HWINFO_FAILURE;
	p_hwInfoResult->disksNumberResult = ACS_APGCC_HWINFO_FAILURE;
	p_hwInfoResult->hwVersionResult = ACS_APGCC_HWINFO_FAILURE;
	p_hwInfoResult->hwTypeResult = ACS_APGCC_HWINFO_FAILURE;

	p_hwInfo->disksCapacity = ACS_APGCC_DISKSCAPACITY_NOVALUE;
	p_hwInfo->disksLocataion = ACS_APGCC_DISKSLOCATION_NOVALUE;
	p_hwInfo->disksNumber = ACS_APGCC_DISKSNUMBER_NOVALUE;
	p_hwInfo->hwVersion = ACS_APGCC_HWVER_NOVALUE;
	p_hwInfo->hwType = ACS_APGCC_HWTYPE_NOVALUE;

	if (( apgcc_hwinfo_flag < ACS_APGCC_GET_HWVERSION ) ||
	    (( apgcc_hwinfo_flag > ACS_APGCC_GET_HWTYPE ) &&
	    ( apgcc_hwinfo_flag != ACS_APGCC_GET_ALL ) ) ){
		syslog (LOG_ERR, "ACS_APGCC_CommonLib::GetHwInfo - Wrong hwinfo_flag");
		return -1;
	}

	std::map <string,int> _hwVer, _DisksLocation, _DisksNumber, _DisksCapacity, _hwType;

	_hwVer["NO_HWTYPE"] = ACS_APGCC_HWVER_NOVALUE;
	_hwVer["VM"] = ACS_APGCC_HWVER_VM;
	_hwVer["GEP1"] = ACS_APGCC_HWVER_GEP1;
	_hwVer["GEP2"] = ACS_APGCC_HWVER_GEP2;
	_hwVer["GEP5"] = ACS_APGCC_HWVER_GEP5;
	_hwVer["GEP7"] = ACS_APGCC_HWVER_GEP7;

	_DisksLocation["ONBOARD"] = ACS_APGCC_DISKSLOCATION_ONBOARD;
	_DisksLocation["EXTERNAL"] = ACS_APGCC_DISKSLOCATION_EXTERNAL;

	_DisksNumber["1"] = ACS_APGCC_DISKSNUMBER_1;
	_DisksNumber["3"] = ACS_APGCC_DISKSNUMBER_3;

	_DisksCapacity["147"] = ACS_APGCC_DISKSCAPACITY_147GB;
	_DisksCapacity["250"] = ACS_APGCC_DISKSCAPACITY_250GB;
	_DisksCapacity["300"] = ACS_APGCC_DISKSCAPACITY_300GB;
	_DisksCapacity["400"] = ACS_APGCC_DISKSCAPACITY_400GB;
	_DisksCapacity["450"] = ACS_APGCC_DISKSCAPACITY_450GB;
	_DisksCapacity["600"] = ACS_APGCC_DISKSCAPACITY_600GB;
	_DisksCapacity["700"] = ACS_APGCC_DISKSCAPACITY_700GB;
	_DisksCapacity["1000"] = ACS_APGCC_DISKSCAPACITY_1000GB;
	_DisksCapacity["1200"] = ACS_APGCC_DISKSCAPACITY_1200GB;
        _DisksCapacity["1300"] = ACS_APGCC_DISKSCAPACITY_1300GB;
	_DisksCapacity["1400"] = ACS_APGCC_DISKSCAPACITY_1400GB;
	_DisksCapacity["1450"] = ACS_APGCC_DISKSCAPACITY_1450GB;
	_DisksCapacity["1600"] = ACS_APGCC_DISKSCAPACITY_1600GB;

	_hwType["NO_HW"]	= ACS_APGCC_HWTYPE_NOVALUE;
	_hwType["VM"]		= ACS_APGCC_HWTYPE_VM;
	_hwType["GEP1"]		= ACS_APGCC_HWTYPE_GEP1;   
	_hwType["GEP2"]		= ACS_APGCC_HWTYPE_GEP2;   
	_hwType["GEP5"]		= ACS_APGCC_HWTYPE_GEP5;   
	_hwType["GEP5-400"]     = ACS_APGCC_HWTYPE_GEP5_400; 
	_hwType["GEP5-1200"]    = ACS_APGCC_HWTYPE_GEP5_1200;
	_hwType["GEP5-64-1200"] = ACS_APGCC_HWTYPE_GEP5_64_1200;
	_hwType["GEP7L-400"]    = ACS_APGCC_HWTYPE_GEP7L_400;
	_hwType["GEP7L-1600"]	= ACS_APGCC_HWTYPE_GEP7L_1600; 
	_hwType["GEP7-128-1600"] =  ACS_APGCC_HWTYPE_GEP7_128_1600;

	if ( (apgcc_hwinfo_flag & ACS_APGCC_GET_ALL) == ACS_APGCC_GET_ALL )
			apos_command += GetHwInfo_ALL_Option;
	else {
		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_HWVERSION )
			apos_command += GetHwInfo_HWversion;

		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_HWTYPE )
				apos_command += GetHwInfo_HWtype;

		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_DISKLOCATION )
			apos_command += GetHwInfo_DisksLocation;

		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_DISKNUMBER )
			apos_command += GetHwInfo_DisksNumber;

		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_DISKCAPACITY )
			apos_command += GetHwInfo_DisksCapacity;

		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_VIRTUALENVPROFILE )
			apos_command += GetHwInfo_VirtualEnvProfile;

		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_CLUSTERIP_MODE1 )
			apos_command += GetHwInfo_IP_Mode1;

		if ( apgcc_hwinfo_flag & ACS_APGCC_GET_CLUSTERIP_MODE2 )
			apos_command += GetHwInfo_IP_Mode2;

		if ( (apgcc_hwinfo_flag & ACS_APGCC_GET_ALL) == ACS_APGCC_GET_ALL )
			apos_command += GetHwInfo_ALL_Option;
	}

	char line[255] = {0};
	char hwinfotype[16] = {0}, hwinfovalue[16] = {0}, hwinforesult[16] = {0};

	FILE* GetHwInfo_fd = popen (apos_command.c_str(),"r");

	if ( GetHwInfo_fd == NULL ){
		syslog (LOG_ERR, "ACS_APGCC_CommonLib::GetHwInfo - popen failed error = %i (%s)",errno, strerror(errno));
		return -1;
	}

	while (fgets(line, sizeof(line), GetHwInfo_fd) != NULL) {

		memset(hwinfotype,0,sizeof(hwinfotype));
		memset(hwinfovalue,0,sizeof(hwinfovalue));
		memset(hwinforesult,0,sizeof(hwinforesult));

		int ret_code = sscanf(line,"%[^'=']=%[^';'];%s",hwinfotype,hwinfovalue,hwinforesult);
		if ( ret_code > 0 ){

			if ( !strlen(hwinfotype) || !strlen(hwinfovalue) || !strlen(hwinforesult) )
				continue;

			if ( strncmp(hwinforesult,"NO_ERROR",sizeof(hwinforesult)) )
				continue;

			if ( strncmp(hwinfotype,"hwversion",sizeof(hwinfotype)) == 0 ){
				map<string,int>::iterator it = _hwVer.find(hwinfovalue);

				if (it != _hwVer.end()){
					p_hwInfo->hwVersion = (ACS_APGCC_HwVer_TypeT)it->second;
					p_hwInfoResult->hwVersionResult = ACS_APGCC_HWINFO_SUCCESS;
				}
			}
			else if ( strncmp(hwinfotype,"hwtype",sizeof(hwinfotype)) == 0 ){
				map<string,int>::iterator it = _hwType.find(hwinfovalue);

				if (it != _hwType.end()){
					p_hwInfo->hwType = (ACS_APGCC_HwType_TypeT)it->second;
					p_hwInfoResult->hwTypeResult = ACS_APGCC_HWINFO_SUCCESS;
				}
			}

			else if ( strncmp(hwinfotype,"diskslocation",sizeof(hwinfotype)) == 0 ){
				map<string,int>::iterator it = _DisksLocation.find(hwinfovalue);

				if (it != _DisksLocation.end()){
					p_hwInfo->disksLocataion = (ACS_APGCC_DiskLocation_TypeT)it->second;
					p_hwInfoResult->disksLocataionResult = ACS_APGCC_HWINFO_SUCCESS;
				}
			}

			else if ( strncmp(hwinfotype,"disksnumber",sizeof(hwinfotype)) == 0 ){
				map<string,int>::iterator it = _DisksNumber.find(hwinfovalue);

				if (it != _DisksNumber.end()){
					p_hwInfo->disksNumber = (ACS_APGCC_DiskNumber_TypeT)it->second;
					p_hwInfoResult->disksNumberResult = ACS_APGCC_HWINFO_SUCCESS;
				}
			}

			else if ( strncmp(hwinfotype,"diskscapacity",sizeof(hwinfotype)) == 0 ){
				map<string,int>::iterator it = _DisksCapacity.find(hwinfovalue);

				if (it != _DisksCapacity.end()){
					p_hwInfo->disksCapacity = (ACS_APGCC_DiskCapacity_TypeT)it->second;
					p_hwInfoResult->disksCapacityResult = ACS_APGCC_HWINFO_SUCCESS;
				}
			}
		}

		memset(line,0,sizeof(line));
	}

	pclose(GetHwInfo_fd);

	return 0;
}

bool ACS_APGCC_CommonLib::isTsUser(const string &user){

	bool isMember = false;

	int j, ngroups;
	gid_t *groups;
	struct passwd *pw;
	struct group *gr;

	const string strTsGroup("tsgroup");

	string str_member;

	ACS_TRA_Logging _log;
	char bufferLog[APGCC_LOG_BUFFER] = {0};

	// open log
	_log.Open("APGCC");

	ngroups = sysconf(_SC_NGROUPS_MAX) + 1;

	groups = (gid_t *)malloc(ngroups * sizeof (gid_t));
	if (groups == NULL) {

		sprintf(bufferLog, " Error to allocate buffer for groups!");
		_log.Write(bufferLog,LOG_LEVEL_ERROR);

	}else{

		// Fetch passwd structure (contains first group ID for user)
		pw = getpwnam(user.c_str());
		if(pw != NULL) {
			// Retrieve group list
			if (getgrouplist(user.c_str(), pw->pw_gid, groups, &ngroups) != -1){

				/* Display list of retrieved groups, along with group names */

				for (j = 0; j < ngroups; j++) {
					gr = getgrgid(groups[j]);
					if (gr != NULL){
						if (strcmp(gr->gr_name, strTsGroup.c_str()) == 0) {
							isMember = true;
							sprintf(bufferLog, "Success, the user : %s , is a member of the group 'tsgroup'. ", user.c_str());
							_log.Write(bufferLog,LOG_LEVEL_INFO);
							break;
						}
					}
				}

			}else{

				sprintf(bufferLog, " Error to retrive grouplist !");
				_log.Write(bufferLog,LOG_LEVEL_ERROR);
			}

		}else{
			//ERROR
			sprintf(bufferLog, " Error to retrive User entry !");
			_log.Write(bufferLog,LOG_LEVEL_ERROR);
		}

		free(groups);
	}

	// close log
	_log.Close();
	// exit
	return isMember;
}


ACS_APGCC_UserInfo_ReturnTypeT ACS_APGCC_CommonLib::getTsUserInfo(const string &user, string &userInfo){

	ACS_APGCC_UserInfo_ReturnTypeT retCode = ACS_APGCC_TsUserInfo_ERROR;
	string ts_user = user;

	struct passwd pwd;
	struct passwd *result = NULL;
	const int bufLength = 1024; // Length of the buffer
	char buf[bufLength] = {0};

	ACS_TRA_Logging _log;
	char bufferLog[APGCC_LOG_BUFFER] = {0};

	/* open log */
	_log.Open("APGCC");

	// Inizialize userInfo
	userInfo = "";

	sprintf(bufferLog, "user to retrive info = %s", ts_user.c_str());
	_log.Write(bufferLog,LOG_LEVEL_DEBUG);
	//Check if user to retrive information is a member of tsUser group
	if(isTsUser(ts_user)){

		sprintf(bufferLog, "retrive local data for user= %s", ts_user.c_str());
		_log.Write(bufferLog,LOG_LEVEL_DEBUG);

		// retrive local data for ts_user
		getpwnam_r(ts_user.c_str(), &pwd, buf, bufLength, &result);


		if( result != NULL) {

			sprintf(bufferLog, "[%s:] user found!" , ts_user.c_str());
			_log.Write(bufferLog,LOG_LEVEL_INFO);

			userInfo = (pwd.pw_gecos);
			sprintf(bufferLog, "user info in string format = %s", userInfo.c_str());
			_log.Write(bufferLog,LOG_LEVEL_INFO);

			retCode = ACS_APGCC_TsUserInfo_SUCCES;

		}else{
			// TS_USER not found
			sprintf(bufferLog, "[%s:] user local not found!" , ts_user.c_str());
			_log.Write(bufferLog,LOG_LEVEL_INFO);

		}

	  }else{
		// in not TS_USER
		sprintf(bufferLog, "[%s:] user isn't a member of tsgroup!",ts_user.c_str());
		_log.Write(bufferLog,LOG_LEVEL_INFO);
	}

	/* close log */
	_log.Close();
	// exit
	return retCode;
}

ACS_APGCC_IpAddress_ReturnTypeT ACS_APGCC_CommonLib::getIpAddresses(ACS_APGCC_ApgAddresses &IpAddresses)
{
	/* To make this method backward compatible for Dual Stack IPs after IPv6 adaptation and to reduce number of IMM calls,
	we use the new overloaded method getIpAddresses() by passing the new struct ACS_APGCC_ApgAddresses_R2 which can hold
	both IPv4 and IPv6 addresses. We then populate the existing single stack structure IpAddresses with either
	IPv4 addresses(IPv4 only stack and dual stack) or IPv6 addresses(IPv6 only stack) fetched by new getIpAddresses()  method . */
	ACS_APGCC_ApgAddresses_R2 tmpIpAddr;
	IpAddresses.clusterIp1 = "";
	IpAddresses.clusterIp2 = "";
	IpAddresses.clusterIpAddress = "";
	ACS_TRA_Logging _log;
	_log.Open("APGCC");
	string errLog("");
	ACS_APGCC_IpAddress_ReturnTypeT retCode = getIpAddresses(tmpIpAddr);
	if(retCode == ACS_APGCC_IpAddress_FAILURE)
	{
		errLog.assign("ACS_APGCC_CommonLib::getIpAddresses(ACS_APGCC_ApgAddresses &IpAddresses) - getIpAddresses(ACS_APGCC_ApgAddresses_R2 &IpAddresses) returned failure!");
		_log.Write(errLog.c_str(),LOG_LEVEL_ERROR);
		return ACS_APGCC_IpAddress_FAILURE;
	}
	if((tmpIpAddr.stackType == ACS_APGCC_IPv4_STACK) || (tmpIpAddr.stackType == ACS_APGCC_DUAL_STACK))
	{
		errLog.assign("ACS_APGCC_CommonLib::getIpAddresses(ACS_APGCC_ApgAddresses &IpAddresses) - Returning IPv4 addresses - SUCCESS");
		_log.Write(errLog.c_str(),LOG_LEVEL_INFO);
		IpAddresses.clusterIp1 = tmpIpAddr.clusterIp1;
		IpAddresses.clusterIp2 = tmpIpAddr.clusterIp2;
		IpAddresses.clusterIpAddress = tmpIpAddr.clusterIpAddress;
	}
	else if(tmpIpAddr.stackType == ACS_APGCC_IPv6_STACK)
	{
		errLog.assign("ACS_APGCC_CommonLib::getIpAddresses(ACS_APGCC_ApgAddresses &IpAddresses) - Returning IPv6 addresses - SUCCESS");
		_log.Write(errLog.c_str(),LOG_LEVEL_INFO);
		IpAddresses.clusterIp1 = tmpIpAddr.clusterIp1_IPv6;
		IpAddresses.clusterIp2 = tmpIpAddr.clusterIp2_IPv6;
		IpAddresses.clusterIpAddress = tmpIpAddr.clusterIpAddress_IPv6;
	}
	return ACS_APGCC_IpAddress_SUCCESS;
}

ACS_APGCC_IpAddress_ReturnTypeT ACS_APGCC_CommonLib::getIpAddresses(ACS_APGCC_ApgAddresses_R2 &IpAddresses)
{
	// New overloaded method to fetch both IPv4 and/or IPv6 cluster IP addresses from northBoundId=1 MO and apos_hwinfo.sh printout
	ACS_APGCC_IpAddress_ReturnTypeT retCode = ACS_APGCC_IpAddress_FAILURE;
	ACS_APGCC_ImmObject imm_Object;
	OmHandler imm_Handle;
	ACS_CC_ReturnType result;

	// DN
	string strNorthBoundNetwork("northBoundId=1,networkConfigurationId=1");

	// Parameter Name
	string strClusterIp1("clusterIP1");
	string strClusterIp2("clusterIP2");
	string strClusterIpAddress("clusterIpAddress");

	ACS_TRA_Logging _log;
	char bufferLog[APGCC_LOG_BUFFER] = {0};

	//init struct of addresses
	IpAddresses.clusterIp1 = "";
	IpAddresses.clusterIp2 = "";
	IpAddresses.clusterIpAddress = "";
	IpAddresses.clusterIp1_IPv6 = "";
	IpAddresses.clusterIp2_IPv6 = "";
	IpAddresses.clusterIpAddress_IPv6 = "";
	_log.Open("APGCC");

	result = imm_Handle.Init();
	if ( result == ACS_CC_SUCCESS ){
		imm_Object.objName = strNorthBoundNetwork;
		result = imm_Handle.getObject(&imm_Object);
		if ( result != ACS_CC_SUCCESS ){
			sprintf(bufferLog, "getIpAddresses() - ERROR: to retrieve object : %s --- %d", strNorthBoundNetwork.c_str(), imm_Handle.getInternalLastError());
			_log.Write(bufferLog,LOG_LEVEL_ERROR);
		}else{
			sprintf(bufferLog, "getIpAddresses() - SUCCESS: to retrieve object : %s --- %d", strNorthBoundNetwork.c_str(), imm_Handle.getInternalLastError());
			_log.Write(bufferLog,LOG_LEVEL_INFO);
		}
		string errLog;
		if ( result == ACS_CC_SUCCESS ){
			unsigned int attributes_num = imm_Object.attributes.size();

			//Search for  attributes
			for (unsigned int i = 0; i < attributes_num; ++i)
			{
				const char * attr_name = imm_Object.attributes[i].attrName.c_str();
				unsigned values_count = imm_Object.attributes[i].attrValuesNum;

				// retrieve clusterIpAddress attr
				if ((values_count) && strcmp(attr_name,strClusterIpAddress.c_str())== 0 )
				{
					string tmp_IP("");
					for(unsigned valId = 0; valId < values_count; valId++)
					{
						char * tmpPtr = reinterpret_cast<char *>(imm_Object.attributes[i].attrValues[valId]);
						if(tmpPtr != NULL)
						{
							tmp_IP.assign(tmpPtr);
							//Dual stack IPs are stored in attribute clusterIpAddress in format: "<IPv4>,<IPv6>"
							ACS_APGCC_CommonUtil::parseIPv4andIPv6AddrFromString(tmp_IP,IpAddresses.clusterIpAddress,IpAddresses.clusterIpAddress_IPv6,IpAddresses.stackType);
						}
						break;		// break after reading 1 value of the attribute clusterIpAddress
					}
					sprintf(bufferLog, "getIpAddresses() - clusterIpAddress : %s ; clusterIpAddress_IPv6 : %s ", IpAddresses.clusterIpAddress.c_str(),IpAddresses.clusterIpAddress_IPv6.c_str());
					_log.Write(bufferLog,LOG_LEVEL_INFO);
					break;		// break to avoid unnecessary iterations as we are only interested in clusterIp attribute
				}
			}
			char line[255] = {0};
			char hwinfotype[16] = {0}, hwinfovalue[60] = {0}, hwinforesult[16] = {0};
			std::string GetHwInfo_IP = GetHwInfo_Path + std::string(GetHwInfo_IP_Mode1) + std::string(GetHwInfo_IP_Mode2);
			FILE* GetHwInfo_fd = popen (GetHwInfo_IP.c_str(),"r");
			while (fgets(line, sizeof(line), GetHwInfo_fd) != NULL) {
				memset(hwinfotype,0,sizeof(hwinfotype));
				memset(hwinfovalue,0,sizeof(hwinfovalue));
				memset(hwinforesult,0,sizeof(hwinforesult));
				int ret_code = sscanf(line,"%[^'=']=%[^';'];%s",hwinfotype,hwinfovalue,hwinforesult);

				if ( ret_code > 0 ){

					if ( !strlen(hwinfotype) || !strlen(hwinfovalue) || !strlen(hwinforesult) )
						continue;

					if ( strncmp(hwinforesult,"NO_ERROR",sizeof(hwinforesult)) )
						continue;

					if ( strncmp(hwinfotype,strClusterIp1.c_str(),sizeof(hwinfotype)) == 0 )
					{
						string tmpIP(hwinfovalue);
						ACS_APGCC_IpAddress_StackT type;		// use and discard for using method parseIPv4andIPv6AddrFromString()
						ACS_APGCC_CommonUtil::parseIPv4andIPv6AddrFromString(tmpIP,IpAddresses.clusterIp1,IpAddresses.clusterIp1_IPv6, type);
					}
					else if ( strncmp(hwinfotype,strClusterIp2.c_str(),sizeof(hwinfotype)) == 0 )
					{
						string tmpIP(hwinfovalue);
						ACS_APGCC_IpAddress_StackT type;
						ACS_APGCC_CommonUtil::parseIPv4andIPv6AddrFromString(tmpIP,IpAddresses.clusterIp2,IpAddresses.clusterIp2_IPv6, type);
					}
				}
				memset(line,0,sizeof(line));
			}
			pclose(GetHwInfo_fd);

			errLog.assign("getIpAddresses() - ");
			errLog += "clusterIPAddress == " + IpAddresses.clusterIpAddress  +"\n";
			errLog += "clusterIPAddress_IPv6 == " + IpAddresses.clusterIpAddress_IPv6  +"\n";
			errLog += "clusterIp1 == " + IpAddresses.clusterIp1  +"\n";
			errLog += "clusterIp1_IPv6 == " + IpAddresses.clusterIp1_IPv6  +"\n";
			errLog += "clusterIp2 == " + IpAddresses.clusterIp2  +"\n";
			errLog += "clusterIp2_IPv6 == " + IpAddresses.clusterIp2_IPv6  +"\n";
			errLog += "stackType == ";
			if(IpAddresses.stackType == ACS_APGCC_IPv4_STACK) {
				errLog += "IPv4 Stack";
				if((IpAddresses.clusterIp1.compare("") != 0) && (IpAddresses.clusterIp2.compare("") != 0) && (IpAddresses.clusterIpAddress.compare("") != 0))
					retCode = ACS_APGCC_IpAddress_SUCCESS;
			}
			else if(IpAddresses.stackType == ACS_APGCC_IPv6_STACK) {
				errLog += "IPv6 Stack";
				if((IpAddresses.clusterIp1_IPv6.compare("") != 0) && (IpAddresses.clusterIp2_IPv6.compare("") != 0) && (IpAddresses.clusterIpAddress_IPv6.compare("") != 0))
					retCode = ACS_APGCC_IpAddress_SUCCESS;
			}
			else if(IpAddresses.stackType == ACS_APGCC_DUAL_STACK) {
				errLog += "DUAL Stack";
				if((IpAddresses.clusterIp1.compare("") != 0) && (IpAddresses.clusterIp2.compare("") != 0) && (IpAddresses.clusterIpAddress.compare("") != 0)
						&& (IpAddresses.clusterIp1_IPv6.compare("") != 0) && (IpAddresses.clusterIp2_IPv6.compare("") != 0) && (IpAddresses.clusterIpAddress_IPv6.compare("") != 0))
					retCode = ACS_APGCC_IpAddress_SUCCESS;
			}
			else if(IpAddresses.stackType == ACS_APGCC_DEFAULT_STACK) {
				errLog += "UNDETERMINDED Stack!!! ERROR!!!";
				retCode = ACS_APGCC_IpAddress_FAILURE;
			}
			_log.Write(errLog.c_str(),LOG_LEVEL_DEBUG);

			if(retCode == ACS_APGCC_IpAddress_SUCCESS)
			{
				sprintf(bufferLog, "getIpAddresses() - SUCCESS: Retrieved all IP addresses of the APG node!");
				_log.Write(bufferLog,LOG_LEVEL_INFO);
			}
			else
			{
				sprintf(bufferLog, "getIpAddresses() - ERROR: Not retrieved all IP addresses of the APG node!");
				_log.Write(bufferLog,LOG_LEVEL_ERROR);
			}
		}
		else {
			sprintf(bufferLog, "getIpAddresses() - ERROR: OmHandler.getObject() FAILURE!!!");
			_log.Write(bufferLog,LOG_LEVEL_ERROR);
		}
		//Finalize Imm
		result = imm_Handle.Finalize();
		if(result == ACS_CC_FAILURE)
		{
			sprintf(bufferLog, "getIpAddresses() - ERROR: OmHandler.finalize() FAILURE!!!");
			_log.Write(bufferLog,LOG_LEVEL_ERROR);
		}
	}else{
		sprintf(bufferLog, "getIpAddresses() - ERROR: OmHandler.init() FAILURE!!!");
		_log.Write(bufferLog,LOG_LEVEL_ERROR);
	}

	//close log
	_log.Close();
	// exit
	return retCode;
}

ACS_APGCC_ThreadSupport_ReturnTypeT ACS_APGCC_CommonLib::EnableMultithreadingAuthorizationSupport () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync);

	/*
	 * The support for multithreading has been already requested:
	 * first finalize the previous request and then enable it again!
	 */
	if (_multithreading_support_requested) {
		::syslog(LOG_ERR, "The support for multi threading in APGCC Common Library has been already requested!");
		return ACS_APGCC_THREADSUPPORT_ALREADY_ENABLED;
	}

	// Setup the callbacks for OpenSSL library (for now it should be enough!)
	if (const ACS_APGCC_ThreadSupport_ReturnTypeT call_result = ACS_APGCC_OpenSSLThreading::register_threading_callbacks_for_openssl()) {
		::syslog(LOG_ERR, "Call 'ACS_APGCC_OpenSSLThreading::register_threading_callbacks_for_openssl' failed with error code == %d", call_result);
		return call_result;
	}

	_multithreading_support_requested = true;
	return ACS_APGCC_THREADSUPPORT_OK;
}

ACS_APGCC_ThreadSupport_ReturnTypeT ACS_APGCC_CommonLib::DisableMultithreadingAuthorizationSupport () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync);

	// The support for multithreading wasn't requested yet (or was already disabled)!!!
	if (!(_multithreading_support_requested)) {
		::syslog(LOG_ERR, "The support for multi threading in APGCC Common Library is not enabled!");
		return ACS_APGCC_THREADSUPPORT_ALREADY_DISABLED;
	}

	// Cleanup the callbacks for OpenSSL library (for now it should be enough!)
	if (const ACS_APGCC_ThreadSupport_ReturnTypeT call_result = ACS_APGCC_OpenSSLThreading::unregister_threading_callbacks_for_openssl()) {
		::syslog(LOG_ERR, "Call 'ACS_APGCC_OpenSSLThreading::unregister_threading_callbacks_for_openssl' failed with error code == %d", call_result);
		return call_result;
	}

	_multithreading_support_requested = false;
	return ACS_APGCC_THREADSUPPORT_OK;
}

ACS_CC_ReturnType ACS_APGCC_CommonLib::ignoreCPSuperUserRole(bool isIgnore) {
	OmHandler omHandler;
	ACS_TRA_Logging _log;
	char bufferLog[APGCC_LOG_BUFFER] = {0};
	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	std::string mmlAuthDn;
	mmlAuthDn.assign("AxeMmlAuthorizationmmlAuthorizationMId=1");


	ACE_INT32 inputValue = isIgnore;
	ACS_CC_ImmParameter paramToChange;
	paramToChange.attrName = (char *)("ignoreCpSuperUserRole");
	paramToChange.attrType = ATTR_INT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	void* valueString[1] =  {(void*)&inputValue};
	paramToChange.attrValues[0] = reinterpret_cast<void*>(valueString[0]);


	_log.Open("APGCC");

        if ( ACS_CC_FAILURE == omHandler.Init())
        {
            sprintf(bufferLog, "ERROR: Initialization FAILURE!!!");
            _log.Write(bufferLog,LOG_LEVEL_ERROR);
        }


	if (ACS_CC_FAILURE == omHandler.modifyAttribute(mmlAuthDn.c_str(), &paramToChange))
	{
		sprintf(bufferLog, "ERROR: Modify FAILURE. Reason: %s!!!",omHandler.getInternalLastErrorText());		
		_log.Write(bufferLog,LOG_LEVEL_ERROR);

	}
	else
	{
		if (ACS_CC_FAILURE == omHandler.Finalize())
		{
		    sprintf(bufferLog, "ERROR: Finalize FAILURE!!");
		    _log.Write(bufferLog,LOG_LEVEL_ERROR);
		}
		else
		{
		    result = ACS_CC_SUCCESS;
		}
	}
	    
	
	return result;
}

ACS_CC_ReturnType ACS_APGCC_CommonLib::ignoreCPUserRoles(bool isIgnore) {
        OmHandler omHandler;
        ACS_TRA_Logging _log;
        char bufferLog[APGCC_LOG_BUFFER] = {0};
        ACS_CC_ReturnType result = ACS_CC_FAILURE;

        std::string mmlAuthDn;
	mmlAuthDn.assign("AxeMmlAuthorizationmmlAuthorizationMId=1");

        ACE_INT32 inputValue = isIgnore;
        ACS_CC_ImmParameter paramToChange;
        paramToChange.attrName = (char *)("ignoreCpUserRoles");
        paramToChange.attrType = ATTR_INT32T;
        paramToChange.attrValuesNum = 1;
        paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
        void* valueString[1] =  {(void*)&inputValue};
        paramToChange.attrValues[0] = reinterpret_cast<void*>(valueString[0]);


        _log.Open("APGCC");


        if ( ACS_CC_FAILURE == omHandler.Init())
        {
            sprintf(bufferLog, "ERROR: Initialization FAILURE!!!");
            _log.Write(bufferLog,LOG_LEVEL_ERROR);
        }

        if (ACS_CC_FAILURE == omHandler.modifyAttribute(mmlAuthDn.c_str(), &paramToChange))
        {
                sprintf(bufferLog, "ERROR: Modify FAILURE. Reason: %s !!!",omHandler.getInternalLastErrorText());
                _log.Write(bufferLog,LOG_LEVEL_ERROR);

        }
        else
        {
                if (ACS_CC_FAILURE == omHandler.Finalize())
                {
                    sprintf(bufferLog, "ERROR: Finalize FAILURE!! %s","tyu");
                    _log.Write(bufferLog,LOG_LEVEL_ERROR);
                }
                else
                {
                    result = ACS_CC_SUCCESS;
                }
        }

	_log.Close();
        return result;

}

