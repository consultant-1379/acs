//****************************************************************************
//
//  NAME
//   acs_lm_common.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA XXX XXXX
//
//  AUTHOR 
//     2008-11-28 by XCSSAGO PA1
//
//  SEE ALSO 
//     -
//
//****************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>
#include "acs_lm_common.h"
#include <stdio.h>
#include <sys/sendfile.h>
#include <iomanip>
#include <ace/ACE.h>
#include <ace/OS_NS_sys_stat.h>
#include "acs_lm_tra.h"
#include <ACS_APGCC_CommonLib.h>
#include <acs_prc_api.h>
#include <sstream>
#include <iostream>
#include <string.h>
#define AP_ACS_REGKEY               "SOFTWARE\\Ericsson\\Adjunct Processor\\ACS\\common"
#define REGKEY_NAME                 "acsaplogs"
#define DEFAULT_LOGFILE_PATH        "K:\\ACS\\LOGS"

#define NAME_LEN 32
#define HW_VER_NOT_FETCHED -1
#define MAX_FILE_PATH 4096
#define LM_HW_VERSION_APG43 0
#define ACS_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#define ACS_AXEFUNCTIONS_CLASS_NAME "AxeFunctions"

int ACS_LM_Common::hwVersion = HW_VER_NOT_FETCHED;
std::string ACS_LM_Common::otherNodeName = "";
bool ACS_LM_Common::lmdataLocked = true;
bool ACS_LM_Common::lkfLocked = false;
std::string ACS_LM_Common::dnOfLMRoot = "";
map<ACE_thread_t, bool> ThrExitHandler::theThrStatusMap;
ACE_Recursive_Thread_Mutex ThrExitHandler::theThrMutex;
/*=================================================================
	ROUTINE: ACS_LM_Common constructor
=================================================================== */
ACS_LM_Common::ACS_LM_Common()
{


}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_Common destructor
=================================================================== */
ACS_LM_Common::~ACS_LM_Common()
{

}//end of destructor
const ACE_UINT32 ACS_LM_Common::crc16table[256]=
{
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/*=================================================================
	ROUTINE: getNodeName
	Get the NodeName from the IMM
=================================================================== */
bool ACS_LM_Common::getNodeName(std::string &aNodeName,OmHandler*  theOmHandlerPtr)
{
	DEBUG("ACS_LM_Common::getNodeName() %s","Entering");
	std::vector<std::string> dnList;
	bool returnCode = true;

	//get the Instance for the class ManagedElement
	if (theOmHandlerPtr->getClassInstances(ACS_APZ_IMM_APZ_CLASS_NAME, dnList) != ACS_CC_SUCCESS)
	{   //ERROR
		returnCode = false;
		ERROR("ACS_LM_Common::getNodeName()- %s","Error in Reading the getClassInstances");
		return returnCode;
	}
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() ^ 1)
		{
			//WARNING: 0 (zero) or more than one node found
			returnCode = false;
			ERROR("ACS_LM_Common::getNodeName()- %s","Error in size of dnList[]");
			return returnCode;
		}
	}
	std::string myDnName = dnList[0];
	std::string myManagedElementRdn(myDnName);
	ACS_CC_ImmParameter paramToFind;
	DEBUG("Fetching %s from IMM", NETWORK_MANAGEDELEMENT_ATTR );
	paramToFind.attrName = (char*)NETWORK_MANAGEDELEMENT_ATTR;
	std::string pszAttrValue;
	//Fetch the networkManagedElementId from the IMM
	if(!myManagedElementRdn.empty())
	{
		if(theOmHandlerPtr->getAttribute( myManagedElementRdn.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
		{

			if(paramToFind.attrValuesNum > 0)
			{
				pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
				std::string mynodename(pszAttrValue);
				DEBUG(" In ACS_LM_Common::getNodeName() %s","NetworkManagedElement attribute is not empty");
				aNodeName= mynodename;
			}
			else
			{
				//get the availability of ',' in the string '0' of the dnList.
				if ((dnList[0].find(","))!= string::npos )
				{//Find the pattern and store the rdn value
					DEBUG(" In ACS_LM_Common::getNodeName() %s","Fetching node name from class instance");
					aNodeName =	dnList[0].substr(dnList[0].find_first_of('=')+1,
							dnList[0].find_first_of(',')-dnList[0].find_first_of('=')-1);

				}
				else
				{//Find the pattern and store the rdn value
					DEBUG(" In ACS_LM_Common::getNodeName()in else %s","Fetching node name from class instance");
					int pos=dnList[0].find("=");
					aNodeName = dnList[0].substr(pos+1,strlen(dnList[0].c_str()));
				}
			}

		}
		else if(!dnList.size() != 0)
		{
			if ((dnList[0].find(","))!= string::npos )
			{//Find the pattern and store the rdn value

				DEBUG(" In ACS_LM_Common::getNodeName()oustside getAttribute %s","Fetching node name from class instance");
				aNodeName =	dnList[0].substr(dnList[0].find_first_of('=')+1,
						dnList[0].find_first_of(',')-dnList[0].find_first_of('=')-1);
			}
			else
			{//Find the pattern and store the rdn value
				DEBUG(" In ACS_LM_Common::getNodeName()in else oustside getAttribute %s","Fetching node name from class instance");
				int pos=dnList[0].find("=");
				aNodeName = dnList[0].substr(pos+1,strlen(dnList[0].c_str()));
			}
		}
		else
		{
			//ERROR :Not able to read the parameter from the IMM
			ERROR("ACS_LM_Common::getNodeName()  %s","Error Reading from IMM");
			return false;
		}
	}
	else
	{
		//ERROR :Not able to read the parameter from the IMM
		ERROR("ACS_LM_Common::getNodeName()  %s","myManagedElementRdn is empty");
		return false;
	}
	DEBUG("ACS_LM_Common::getNodeName() NodeName = %s",aNodeName.c_str());
	DEBUG("ACS_LM_Common::getNodeName() %s","Leaving");
	return returnCode;
}//end of getNodeName

/*=================================================================
	ROUTINE: getClusterIPAddress
=================================================================== */
bool ACS_LM_Common::getClusterIPAddress(std::string &aClusterIPAddress,OmHandler*  theOmHandlerPtr)
{
	DEBUG("ACS_LM_Common::getClusterIPAddress()  %s","Entering");
	std::string myClusterIPAddressParameter(NETWORK_MAIN_OBJ_DN);
	ACS_CC_ImmParameter paramToFind;
	DEBUG("Fetching %s from IMM", NETWORK_CLUSTER_IP_ADDRESS );
	paramToFind.attrName = (char*)NETWORK_CLUSTER_IP_ADDRESS;
	std::string ClusterIPfromConfFile = ACS_LM_Common::getClusterIPfromConfFile();
	//Fetch the ClusterIPAddress from the IMM
	if(theOmHandlerPtr->getAttribute( myClusterIPAddressParameter.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
	{
		char* pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
		DEBUG(" IpAddress Value :%s",pszAttrValue);
		//Check the fetched IPAddress is not NULL
		if(ACE_OS::strcmp(pszAttrValue , "") != 0)
		{
			std::string myStr(pszAttrValue);
			aClusterIPAddress= myStr;
			
		}
		if(aClusterIPAddress != ClusterIPfromConfFile)
		{
			aClusterIPAddress = ClusterIPfromConfFile;
			INFO("ACS_LM_Common::getClusterIPAddress IMM clusterIP aligned with cluster conf file =%s", aClusterIPAddress.c_str());
		}
	}
	else
	{//ERROR :Not able to read the parameter from the IMM
		ERROR("ACS_LM_Common::getClusterIPAddress()  %s","Error Reading from IMM");
		return false;
	}
	INFO("ACS_LM_Common::getClusterIPAddress()  aClusterIPAddress = %s",aClusterIPAddress.c_str());
	DEBUG("ACS_LM_Common::getClusterIPAddress()  %s","Leaving");
	return true;
}//end of getClusterIPAddress
/*=================================================================
	ROUTINE: generateFingerPrint
=================================================================== */
bool ACS_LM_Common::generateFingerPrint(std::string & aFingerPrint,OmHandler* theOmHandlerPtr)
{
	DEBUG("ACS_LM_Common::generateFingerPrint()  %s","Entering");
	std::string myIPClusterAdress("");
	std::string myNodeName("");
	bool myClusterIPAdressFlag= getClusterIPAddress(myIPClusterAdress,theOmHandlerPtr);
	if(!myClusterIPAdressFlag)
	{//ERROR: Flag is false when not able to fetch the clusterIpAddress
		return false;
	}
	bool myNodeNameFlag=getNodeName(myNodeName,theOmHandlerPtr);
	if(!myNodeNameFlag)
	{//ERROR: Flag is false   not able to fetch the nodeName
		return false;
	}
	//Compare and append the IPAddress and NodeName separated with a '-'
	if(!((strcmp(myIPClusterAdress.c_str(),"")== 0) && (strcmp(myNodeName.c_str(),"") == 0)))
	{
		aFingerPrint = myNodeName;
		aFingerPrint.append("-");
		aFingerPrint.append(myIPClusterAdress);
		INFO("ACS_LM_Common::generateFingerPrint() aFingerPrint =  %s",aFingerPrint.c_str());
	}
	else
	{//ERROR :Not able to print the finger print value
		ERROR("ACS_LM_Common::generateFingerPrint()  %s","Failed to get Fingerpring");
		return false;
	}
	DEBUG("ACS_LM_Common::generateFingerPrint()  %s","Leaving");
	return true;

}//end of generateFingerPrint

/*=================================================================
        ROUTINE: getPsaPath
=================================================================== */
bool ACS_LM_Common::getPsaPath(const std::string& pszLogicalName,std::string& pszPath)
{
	std::string pszReadLine;
	//ifstream * fileStream = new ifstream(srcPath.c_str(),ios::in);
	ifstream fileStream(pszLogicalName.c_str(),ios::in);
	if (std::getline( fileStream, pszReadLine) <= 0)
	{
		return false;
	}
	INFO("ACS_LM_Server::copyFileToBuffer() - buf = %s",pszReadLine.c_str());
	if(pszReadLine.size() != 0)
	{
		pszPath = pszReadLine;
	}
	else
	{
		INFO("ACS_LM_Server::copyFileToBuffer() %s","pszReadLine.size() == 0");
		return false;
	}
	INFO("ACS_LM_Server::getPsaPath() - pszPath = %s", pszPath.c_str());
	DEBUG("ACS_LM_Common::getPsaPath()  %s","Leaving");
	return true;


	//  return myResult;
}//end of getPsaPath

/*=================================================================
	ROUTINE: getDataDiskPath
=================================================================== */
bool ACS_LM_Common::getDataDiskPath(const std::string& pszLogicalName,std::string& pszPath)
{
	DEBUG("ACS_LM_Common::getDataDiskPath()  %s","Entering");

	ACS_APGCC_CommonLib myAPGCCCommonLib;

	ACS_APGCC_DNFPath_ReturnTypeT  errorCode;
	char* pszPath1 = new char[1000];
	int myLen = 100;
	INFO("The logical name is = %s",pszLogicalName.c_str());
	errorCode = myAPGCCCommonLib.GetDataDiskPath(pszLogicalName.c_str(),pszPath1,myLen) ;
	bool myResult = false;
	if ( errorCode == ACS_APGCC_STRING_BUFFER_SMALL )
	{
		INFO("%s","The return code is : ACS_APGCC_STRING_BUFFER_SMALL ");
		delete []pszPath1;
	}
	else if ( errorCode == ACS_APGCC_FAULT_LOGICAL_NAME )
	{
		INFO("%s","The return code is : ACS_APGCC_FAULT_LOGICAL_NAME ");
		delete []pszPath1;
	}
	else if ( errorCode == ACS_APGCC_DNFPATH_SUCCESS )
	{
		INFO("%s"," The return code is : ACS_APGCC_DNFPATH_SUCCESS ");
		std::string myStr1(pszPath1);
		pszPath = myStr1;
		delete []pszPath1;
		myResult = true;
	}
	else if ( errorCode == ACS_APGCC_DNFPATH_FAILURE )
	{
		INFO("%s"," The return code is  ACS_APGCC_DNFPATH_FAILURE ");
		delete []pszPath1;

	}
	DEBUG("ACS_LM_Common::getDataDiskPath()  %s","Leaving");

	return myResult;
}//end of getDataDiskPath
/*=================================================================
	ROUTINE: isDir
=================================================================== */
bool ACS_LM_Common::isDir(std::string path)
{
	bool returnStatus = false;
	int status;
	struct stat statbuf;
	if ( path.empty())
	{
		return returnStatus;
	}
	status = stat(path.c_str(), &statbuf);
	if( status != 0)
	{
		return returnStatus;
	}
	else
	{
		if( S_ISDIR(statbuf.st_mode)) 
		{	
			returnStatus = true;
			//return true;
		}
	}	
	return returnStatus;
}//end of isDir
/*=================================================================
	ROUTINE: getlatestFile
=================================================================== */
bool  ACS_LM_Common::getlatestFile(std::string path, std::string &lservrc)
{
	DEBUG("In getlatestFile path = %s",path.c_str());
	bool returnStatus = false;
	DIR *dir = 0;
	dir = opendir(path.c_str());
	struct dirent *entry;
	std::vector<time_t> myTimeList;
	std::vector<string> myFileList;

	DEBUG("%s","getLastFile Entered");
	if (dir == 0)
	{
		DEBUG("%s","dir == 0");
		returnStatus = false;
	}
	else
	{
		DEBUG("%s","dir != 0");
		std::string fullPath;
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			{
				DEBUG("entry name %s",entry->d_name);
				fullPath.clear();
				fullPath =path;
				fullPath.append("/");
				fullPath.append(entry->d_name);
				bool isdir = isDir(fullPath);
				if( isdir == false)
				{
					ACE_HANDLE src = NULL;
					struct stat stat_buf;

					src = ACE_OS::open(entry->d_name, O_RDONLY);
					if(!src)
					{
						closedir(dir);
						return false;
					}	
					stat(fullPath.c_str(), &stat_buf);
					myTimeList.push_back(stat_buf.st_mtime);
					myFileList.push_back(entry->d_name);
					ACE_OS::close(src);
				}
			}
		}
		if( (int) myFileList.size() == 0 )
		{
			DEBUG("myFileList.size() %d",(int) myFileList.size());
			closedir(dir);
			return false;
		}	
		std::vector<time_t>::iterator myIter = myTimeList.begin();
		std::vector<string>::iterator myStrIter = myFileList.begin();
		std::vector<time_t>::iterator myIterEnd = myTimeList.end();
		time_t myLatestTime;
		std::string myLatestFile(*myStrIter);
		if(myIter != myIterEnd)
		{
			myLatestTime = *myIter;
			++myIter;
			++myStrIter;
			for(;myIter != myIterEnd;++myIter,++myStrIter)
			{
				double varDiff = difftime(*myIter, myLatestTime);
				if(varDiff > 0)
				{
					myLatestTime = *myIter;
					myLatestFile = *myStrIter;
				}
			}
			lservrc = myLatestFile;
			returnStatus = true;
		}
		else
		{
			returnStatus = false;
		}
		closedir(dir);
	}
	return returnStatus;
}//end of getlatestFile

/*=================================================================
	ROUTINE: getApplicationErrorText
=================================================================== */
std::string ACS_LM_Common::getApplicationErrorText(const ACS_LM_AppExitCode appExitCode)
{
	DEBUG("ACS_LM_Common::getApplicationErrorText()  %s","Entering");
	std::string eText;
	switch(appExitCode)
	{
	case ACS_LM_RC_OK:
		eText = "";
		break;
	case ACS_LM_RC_INCUSAGE:
		eText = "Incorrect usage";
		break;
	case ACS_LM_RC_SERVERNOTRESPONDING:
		eText = "Unable to connect to server";
		break;
	case ACS_LM_RC_NOK:
		eText = "Error when executing (general fault)";
		break;
	case ACS_LM_RC_PHYFILEERROR:
		eText = "Physical file error";
		break;
	case ACS_LM_RC_CMD_NOTALLOWED:
		eText = "Command allowed only on AP1 active node";
		break;
	case ACS_LM_RC_LKEYALREADYEXISTS:
		eText = "License key already exists";
		break;
	case ACS_LM_RC_LKEYNOTFOUND:
		eText = "License key not found";
		break;
	case ACS_LM_RC_UNREAS:
		eText = "Unreasonable value";
		break;
	case ACS_LM_RC_INVALIDPATH:
		eText = "Invalid license key file path";
		break;
	case ACS_LM_RC_INVALIDLKF:
		eText = "Invalid license key file";
		break;
	case ACS_LM_RC_TESTMODENOTALLOWED:
		eText = "Test Mode not allowed";
		break;
	case ACS_LM_RC_EMERGENCYMODENOTALLOWED:
		eText = "Emergency State Mode not allowed";
		break;
	case ACS_LM_RC_EMERGENCYMODEALREADYACTIVATED:
		eText = "Emergency State Mode already activated";
		break;
	case ACS_LM_RC_MISSINGLKF:
		eText = "Missing License Key File";
		break;
	case ACS_LM_RC_BRFINPROGRESS:
		eText = "Command not executed, AP backup in progress";
		break;
	default:
		char buf[11];
		ACE_OS::itoa(appExitCode, buf, 10 );
		eText = "Unknown fault code ";
		eText += buf;
	}
	DEBUG("ACS_LM_Common::getApplicationErrorText()  %s","Leaving");
	return eText;
}//end of getApplicationErrorText


/*=================================================================
	ROUTINE: getApplicationErrorText
=================================================================== */
std::string ACS_LM_Common::getApplicationErrorText(const int appExitCode)
{
	DEBUG("ACS_LM_Common::getApplicationErrorText()  %s","Entering");
	std::string eText;
	switch(appExitCode)
	{
	case 0:
		eText = "";
		break;
	case 2:
		eText = "Incorrect usage";
		break;
	case 117:
		eText = "Unable to connect to server";
		break;
	case 1:
		eText = "Error when executing (general fault)";
		break;
	case 8:
		eText = "Physical file error";
		break;
	case 19:
		eText = "Command allowed only on AP1 active node";
		break;
	case 20:
		eText = "License key already exists";
		break;
	case 21:
		eText = "License key not found";
		break;
	case 3:
		eText = "Unreasonable value";
		break;
	case 10:
		eText = "Invalid license key file path";
		break;
	case 23:
		eText = "Invalid license key file";
		break;
	case 26:
		eText = "Test Mode not allowed";
		break;
	case 27:
		eText = "Emergency State Mode not allowed";
		break;
	case 28:
		eText = "Emergency State Mode already activated";
		break;
	case 24:
		eText = "Missing License Key File";
		break;
	default:
		char buf[11];
		ACE_OS::itoa(appExitCode, buf, 10 );
		eText = "Unknown fault code ";
		eText += buf;
	}
	DEBUG("ACS_LM_Common::getApplicationErrorText()  %s","Leaving");
	return eText;
}//end of getApplicationErrorText

/*=================================================================
	ROUTINE: createLmDir
=================================================================== */
bool ACS_LM_Common::createLmDir(const std::string& dir)
{
	DEBUG("ACS_LM_Common::createLmDir()  %s","Entering");
	bool dirCreated = false;
	ACE_stat statBuff;
	ACE_INT16 result = -1;
	if( ACE_OS::stat(dir.c_str(), &statBuff)  != 0 )
	{
		result = ACE_OS::mkdir(dir.c_str());
	}
	else
	{
		result = 0;
	}
	if(result == EEXIST || result == 0)
	{
		dirCreated = true;
		INFO(" %s Directory created ",dir.c_str());
	}
	else
	{
		ERROR("ACS_LM_Common::createLmDir()  %s","Failed");
	}
	DEBUG("ACS_LM_Common::createLmDir()  %s","Leaving");
	return dirCreated;
}//end of createLmDir

/*=================================================================
	ROUTINE: getFileType
=================================================================== */

ACS_LM_FileType ACS_LM_Common::getFileType(const std::string& file)
{
	DEBUG("ACS_LM_Common::getFileType()  %s","Entering");
	ACE_stat fileStat;
	ACE_INT32 s32Type = 0 ;
	ACS_LM_FileType fileType = ACS_LM_FILE_ACCESSDENIED;
	if ( ACE_OS::stat(file.c_str(),&fileStat) != 0 )
	{
		s32Type = -1;	//Invalid File/Directory
		DEBUG("ACS_LM_Common::getFileType()  %s","Invalid File/Directory");
	}
	if( s32Type !=-1 )
	{
		if ( S_ISDIR(fileStat.st_mode) )
		{
			s32Type = 2; //Directory
			fileType = ACS_LM_FILE_ISDIR;
			DEBUG("ACS_LM_Common::getFileType()  %s","ACS_LM_FILE_ISDIR");
		}
		else if( S_ISREG(fileStat.st_mode) )
		{
			s32Type = 1; //File
			fileType = ACS_LM_FILE_ISFILE;
			DEBUG("ACS_LM_Common::getFileType()  %s","ACS_LM_FILE_ISFILE");
		}
		else
		{
			s32Type = 0; //Others
			DEBUG("ACS_LM_Common::getFileType()  %s","Others");
		}
	}
	DEBUG("ACS_LM_Common::getFileType()  %s","Leaving");
	return fileType;

}//end of getFileType


/*=================================================================
	ROUTINE: getFileSize
=================================================================== */
ACE_INT64 ACS_LM_Common::getFileSize(const std::string& fileName)
{
	ACE_INT64 value = 0;
	ACE_stat  myStat;
	int myRet = ACE_OS::stat(fileName.c_str(),&myStat);
	if(myRet == 0)
	{
		value = myStat.st_size;
	}
	else
	{
		return 0;
	}
	return value;
}//end of getFileSize

//========================================================================================
//construct the date string from minutes
//========================================================================================
/*=================================================================
	ROUTINE: constructDate
=================================================================== */
std::string ACS_LM_Common::constructDate(const ACE_UINT64 minutes)
{
	struct tm* tmPtrStart = NULL;
	char date[11]={0};
	std::string strDate = "NEVER";
	if(minutes != 0)
	{
		if((tmPtrStart = localtime((time_t *) & minutes)) != NULL )
		{
			ostringstream dateStr(date, ios_base::out);
			dateStr<<std::setw(4)<<std::setfill('0')<<((int)(tmPtrStart->tm_year+1900))<<"/";
			dateStr<<std::setw(2)<<std::setfill('0')<<((int)(tmPtrStart->tm_mon+1))<<"/";
			dateStr<<std::setw(2)<<std::setfill('0')<<((int)(tmPtrStart->tm_mday))<<std::ends;	
			strDate = dateStr.str();
		}
	}
	return strDate;	
}//end of constructDate
/*=================================================================
	ROUTINE: isCommandAllowed
=================================================================== */
bool ACS_LM_Common::isCommandAllowed()
{
	DEBUG("ACS_LM_Common::isCommandAllowed()  %s","Entering");
	bool allowed = false;   
	if((apNumber() == 1) && (nodeStatus() == 1))
	{
		DEBUG("ACS_LM_Common::isCommandAllowed()  %s","Command Allowed");
		allowed = true;
	}
	else
	{
		DEBUG("ACS_LM_Common::isCommandAllowed()  %s","Command Not Allowed");
	}
	DEBUG("ACS_LM_Common::isCommandAllowed()  %s","Leaving");
	return allowed;
}//end of isCommandAllowed
/*=================================================================
	ROUTINE: apNumber
=================================================================== */
int ACS_LM_Common::apNumber()
{	
	DEBUG("ACS_LM_Common::apNumber()  %s","Entering");
	int type = 0; 
	std::string thisNodeIP = getThisNodeIp();
	if((thisNodeIP.compare(AP1_NET1_NODEA)== 0) || (thisNodeIP.compare(AP1_NET2_NODEA)== 0) 
			|| (thisNodeIP.compare(AP1_NET1_NODEB)== 0) || (thisNodeIP.compare(AP1_NET2_NODEB)== 0))
	{
		type = 1;
		DEBUG("ACS_LM_Common::apNumber()  %s","type = 1");
	}
	else if((thisNodeIP.compare(AP2_NET1_NODEA)== 0) || (thisNodeIP.compare(AP2_NET2_NODEA)== 0) 
			|| (thisNodeIP.compare(AP2_NET1_NODEB)== 0) || (thisNodeIP.compare(AP2_NET2_NODEB)== 0))
	{
		type = 2;
		DEBUG("ACS_LM_Common::apNumber()  %s","type = 2");
	}
	DEBUG("ACS_LM_Common::apNumber()  %s","Leaving");
	return type;
}//end of apNumber
/*=================================================================
	ROUTINE: nodeStatus
=================================================================== */
int ACS_LM_Common::nodeStatus()
{
	DEBUG("ACS_LM_Common::nodeStatus()  %s","Entering");
	ACS_PRC_API prcObj;
	int nodeState = 0;
	nodeState = prcObj.askForNodeState();
	DEBUG("ACS_LM_Common::nodeStatus()  %s","Leaving");
	return nodeState;
}//end of nodeStatus

//=========================================================================================
/* Calculate the CRC in a file from ini_ofs for a number of bytes given by len.
   If len is zero continues until the end of the file, the file is opened
   to allow the reading (r, r + w +) */
//=========================================================================================
/*=================================================================
	ROUTINE: getFileChecksum
=================================================================== */
ACE_UINT32 ACS_LM_Common::getFileChecksum(const std::string& file, long offset, ACE_UINT64 len)
{
	DEBUG("ACS_LM_Common::getFileChecksum()  %s","Entering");
	FILE *fp = ACE_OS::fopen(file.c_str(), "r");
	if(!fp)
	{
		//	DEBUG("ACS_LM_Common::getFileChecksum()  ACE_OS::fopen( file = %s) Failed",file.c_str());
		return 0;
	}
	ACE_INT64 oldOffset = ACE_OS::ftell(fp);
	INFO("ACS_LM_Common::getFileChecksum()  %d",oldOffset);
	ACE_OS::fseek(fp, offset,SEEK_SET);

	ACE_TCHAR ch;
	ACE_UINT32 crc16 = 0;
	if(len)
	{
		for(; len && (ch = (char)ACE_OS::getc(fp)) != EOF; len--)
		{
		    //DEBUG("ACS_LM_Common::getFileChecksum()  %s, crc16=%d","IF Block Before calculateCRC",crc16);
			crc16 = calculateLKfChecksumCRC(crc16, ch);
		}
	}
	else
	{
		while((ch = (char)ACE_OS::getc(fp)) != EOF)
		{
			//DEBUG("ACS_LM_Common::getFileChecksum()  %s,crc16=%d","Else block Before calculateCRC",crc16);
			crc16 = calculateLKfChecksumCRC(crc16, ch);
		}
	}
	if ( oldOffset!= -1)
	ACE_OS::fseek(fp, oldOffset,SEEK_SET);

	crc16 = crc16 ^ 0X1234;
	ACE_OS::fclose(fp);
	//INFO("ACS_LM_Common::getFileChecksum() crc32 = %d",crc16);
	DEBUG("ACS_LM_Common::getFileChecksum()  %s","Leaving");
	return crc16;
}//end of getFileChecksum
/*=================================================================
	ROUTINE: getMemoryCheckSum
=================================================================== */
ACE_UINT16 ACS_LM_Common::getMemoryCheckSum(void* startAddress, ACE_UINT64 len)
{
	DEBUG("ACS_LM_Common::getMemoryCheckSum() %s,len= %d","Entering",len);
	ACE_UINT16 crc16 = 0;
	ACE_TCHAR * bp = (ACE_TCHAR *)startAddress;

	if(len != 0)
	{
		for (crc16 = 0; len; len--)
		{
			//DEBUG("ACS_LM_Common""getMemoreyCheckSum() crc16=%d",crc16);
			crc16 = calculateCRC(crc16, *bp++);
		}
	}
	DEBUG("ACS_LM_Common::getMemoryCheckSum()  %s","Leaving");
	return crc16;
}//end of getMemoryCheckSum

//Start of TR HV60364
// The below given method will calculate the checksum of the LMDATA file during the midnight.
ACE_UINT16 ACS_LM_Common::getLmdataMemoryCheckSum(std::string lmdataPath)
{
	DEBUG("ACS_LM_Common::getLmdataMemoryCheckSum()  %s","Entering");
	ACE_UINT16 crc16 = 0;
	if(ACS_LM_Common::isFileExists(lmdataPath))
	{
	ACE_UINT64 fileSize = (ACE_UINT64)ACS_LM_Common::getFileSize(lmdataPath);
	ACE_HANDLE handle = ACE_OS::open(lmdataPath.c_str(),O_RDONLY );
	if(handle != ACE_INVALID_HANDLE)
	{
		ACE_UINT64 bytesRead = 0;
		ACE_TCHAR * buffer = new ACE_TCHAR[fileSize];
		bytesRead = ACE_OS::read(handle,(void*)buffer,fileSize);
		if((bytesRead != 0) && (bytesRead == fileSize))
		{
			ACE_UINT64 len = fileSize - 2;
			crc16 = ACS_LM_Common::getMemoryCheckSum(buffer, len);
		}
		delete[] buffer;
		ACE_OS::close(handle);
	}
	}
	else
	{
		DEBUG("%s","Lmdata file does not exist");
	}
	DEBUG("ACS_LM_Common::getLmdataMemoryCheckSum()  %s","Leaving");
	return crc16;
}
//End of TR HV60364
/*=================================================================
	ROUTINE: calculateCRC
=================================================================== */
//==================================================================================
/* update crc reverse referring the CRC table */
//==================================================================================
ACE_UINT16 ACS_LM_Common::calculateCRC(ACE_UINT16 crc, ACE_UINT32 c)
{
	//DEBUG("ACS_LM_Common::calculateCRC()  %s","Entering");
	register ACE_INT16 tmp;
	tmp = (ACE_TCHAR)(crc ^ c);
	crc = (crc >> 8) ^ crc16table[tmp];
	//DEBUG("ACS_LM_Common::calculateCRC()  %s","Leaving");
	return crc;
}//end of calculateCRC

//==================================================================================
/* update crc reverse referring the CRC table */
//==================================================================================
ACE_UINT32 ACS_LM_Common::calculateLKfChecksumCRC(ACE_UINT32 crc, ACE_UINT32 c)
{
	//DEBUG("ACS_LM_Common::calculateCRC()  %s","Entering");
	register ACE_INT32 tmp;
	tmp = (ACE_TCHAR)(crc ^ c);
	crc = (crc >> 8) ^ crc16table[tmp];
	//DEBUG("ACS_LM_Common::calculateCRC()  %s","Leaving");
	return crc;
}//end of calculateCRC


/*=================================================================
	ROUTINE: getThisNodeIp
=================================================================== */
std::string ACS_LM_Common::getThisNodeIp()
{
	DEBUG("ACS_LM_Common::getThisNodeIp()  %s","Entering");
	std::string thisNodeIp = "";
	ACE_TCHAR nodeHostName[32];
	if(gethostname(nodeHostName, sizeof(nodeHostName)) == 0)
	{
		struct hostent *remoteHost;
		remoteHost = ACE_OS::gethostbyname(nodeHostName);
		if(remoteHost != NULL)
		{
			struct in_addr ipAddr;
			for(int i=0; remoteHost->h_addr_list[i] != NULL; i++)
			{
				ipAddr.s_addr = *(u_long *) remoteHost->h_addr_list[i]; //Fix for LM commands are not working in AP1 node
				char* ip = inet_ntoa(ipAddr);
				if(ACE_OS::strcmp(AP1_NET1_NODEA, ip) == 0)
				{
					thisNodeIp = AP1_NET1_NODEA;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP1_NET1_NODEA");
					break;
				}
				else if(ACE_OS::strcmp(AP1_NET1_NODEB, ip) == 0)
				{
					thisNodeIp = AP1_NET1_NODEB;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP1_NET1_NODEB");
					break;
				}
				else if(ACE_OS::strcmp(AP1_NET2_NODEA, ip) == 0)
				{
					thisNodeIp = AP1_NET2_NODEA;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP1_NET2_NODEA");
					break;
				}
				else if(ACE_OS::strcmp(AP1_NET2_NODEB, ip) == 0)
				{
					thisNodeIp = AP1_NET2_NODEB;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP1_NET2_NODEB");
					break;
				}
				else if(ACE_OS::strcmp(AP2_NET1_NODEA, ip) == 0)
				{
					thisNodeIp = AP2_NET1_NODEA;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP2_NET1_NODEA");
					break;
				}
				else if(ACE_OS::strcmp(AP2_NET1_NODEB, ip) == 0)
				{
					thisNodeIp = AP2_NET1_NODEB;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP2_NET1_NODEB");
					break;
				}
				else if(ACE_OS::strcmp(AP2_NET2_NODEA, ip) == 0)
				{
					thisNodeIp = AP2_NET2_NODEA;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP2_NET2_NODEA");
					break;
				}
				else if(ACE_OS::strcmp(AP2_NET2_NODEB, ip) == 0)
				{
					thisNodeIp = AP2_NET2_NODEB;
					DEBUG("ACS_LM_Common::getThisNodeIp()  %s","AP2_NET2_NODEB");
					break;
				}
			}
		}
	}
	DEBUG("ACS_LM_Common::getThisNodeIp()  %s","Leaving");
	return thisNodeIp.c_str();
}//end of getThisNodeIp
/*=================================================================
	ROUTINE: getOtherNodeIp
=================================================================== */
std::string ACS_LM_Common::getOtherNodeIp()
{
	DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","Entering");
	std::string otherNodeIp = "";
	std::string thisNodeIp = getThisNodeIp();
	if(thisNodeIp.compare(AP1_NET1_NODEA) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP1_NET1_NODEB");
		otherNodeIp = AP1_NET1_NODEB;
	}
	else if(thisNodeIp.compare(AP1_NET1_NODEB) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP1_NET1_NODEA");
		otherNodeIp = AP1_NET1_NODEA;
	}
	else if(thisNodeIp.compare(AP1_NET2_NODEA) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP1_NET2_NODEB");
		otherNodeIp = AP1_NET2_NODEB;						
	}
	else if(thisNodeIp.compare(AP1_NET2_NODEB) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP1_NET2_NODEA");
		otherNodeIp = AP1_NET2_NODEA;
	}
	else if(thisNodeIp.compare(AP2_NET1_NODEA) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP2_NET1_NODEB");
		otherNodeIp = AP2_NET1_NODEB;
	}
	else if(thisNodeIp.compare(AP2_NET1_NODEB) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP2_NET1_NODEA");
		otherNodeIp = AP2_NET1_NODEA;
	}
	else if(thisNodeIp.compare(AP2_NET2_NODEA) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP2_NET2_NODEB");
		otherNodeIp = AP2_NET2_NODEB;						
	}
	else if(thisNodeIp.compare(AP2_NET2_NODEB) == 0)
	{
		DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","AP2_NET2_NODEA");
		otherNodeIp = AP2_NET2_NODEA;
	}
	DEBUG("ACS_LM_Common::getOtherNodeIp()  %s","Leaving");
	return otherNodeIp;
}//end of getOtherNodeIp
/*=================================================================
	ROUTINE: toUpperCase
=================================================================== */
void ACS_LM_Common::toUpperCase(char* str)
{
	int len = (int)strlen(str);
	for(int i = 0; i < len; i++)
	{		
		if(isalpha(str[i]))
		{
			str[i] = toupper(str[i]);
		}			
	}
}//end of toUpperCase
/*=================================================================
	ROUTINE: getFullPath
=================================================================== */
std::string ACS_LM_Common::getFullPath(const std::string& path)
{
	std::string myFullPathStr;

	if (ACE_OS::strncmp(path.c_str(), "/", 1) == 0)
	{
		//		 given path is FULL Path ... no need to expand
		myFullPathStr = path;
	}
	else
	{
		//		the relative Path ...
		ACE_TCHAR szTemp[1024];
		//ACE_TCHAR szTemp[];
		ACE_TCHAR* CurrDirectory= ACE_OS::getcwd(szTemp, 1024);
		ACE_INT32 dwSize = ACE_OS::atoi(CurrDirectory);
		if (ACE_OS::strncmp(path.c_str(), "./", 2) == 0)
		{
			ACE_OS::strncat(szTemp, path.c_str()+1, 1023-dwSize);
		}
		else
		{
			ACE_TCHAR temp[3] = "/";
			ACE_OS::strcat(szTemp, temp);
			ACE_OS::strncat(szTemp, path.c_str(), 1023-dwSize-1);
		}
		myFullPathStr = szTemp;
		// Make sure that the path does not ends with an "\" character
	}
	return myFullPathStr;

}//end of getFullPath
/*=================================================================
	ROUTINE: isFileExists
=================================================================== */
bool ACS_LM_Common::isFileExists(std::string& aFilePath)
{
	ACE_stat statBuff;
	if( ACE_OS::stat(aFilePath.c_str(), &statBuff)  != 0 )
	{
		return false;
	}
	return true;
}//end of isFileExists
/*=================================================================
	ROUTINE: isLKFExists
=================================================================== */
bool ACS_LM_Common::isLKFExists()
{
	std::string thePSAPath;
	std::string lkfBackupPath(LM_LKF_BACKUP_PATH);
	if(ACS_LM_Common::getPsaPath(LM_PSA_PATH,thePSAPath)== false)
	{
	ERROR("ACS_LM_Common::isLKFExists() %s","ACS_LM_Common::getPsaPath(ACS_DATA) FAILED");
	}
	thePSAPath += "/ACS-LM/lservrc";

	        if(ACS_LM_Common::isFileExists(thePSAPath))
			{
				return true;
			}
			else if(ACS_LM_Common::isFileExists(lkfBackupPath))
			{
				return true;
			}
			else
			{
				return false;
			}
}
/*=================================================================
	ROUTINE: deleteFile
=================================================================== */
bool ACS_LM_Common::deleteFile(std::string & srcPath)
{
	if(ACE_OS::unlink(srcPath.c_str()) == -1)
	{
		return false;
	}
	return true;
}//end of deleteFile
/*=================================================================
	ROUTINE: restoreFile
=================================================================== */
bool ACS_LM_Common::restoreFile(const std::string& srcPath, const std::string& destPath)
{
	DEBUG("ACS_LM_Common::restoreFile()  %s","Entering");
	ACE_HANDLE src;
	ACE_HANDLE dest;
	ssize_t bytesToBeWritten;
	ssize_t bytesWrote;
	bool myResult = true;

	ACE_stat stat_buf;  /* hold information about input file */

	if ((src = ACE_OS::open(srcPath.c_str(), O_RDONLY)) == -1)
	{
		INFO("ACS_LM_Common::restoreFile() Cant open the file SrcPath- %s",srcPath.c_str());
		myResult =  false;
		return myResult;
	}

	ACE_OS::fstat(src, &stat_buf);

	ACE_TCHAR buf[stat_buf.st_size];

	bytesToBeWritten = ACE_OS::read(src, buf, stat_buf.st_size);
	INFO("ACS_LM_Common::restoreFile() bytesToBeWritten from SrcPath to DesPath- %d",bytesToBeWritten);

	if ((dest = ACE_OS::open(destPath.c_str(), O_WRONLY|O_CREAT|O_TRUNC)) == -1)
	{
		DEBUG("ACS_LM_Common::restoreFile()  %s","Cant open the file destPath");
		myResult =  false;
		ACE_OS::close(src);
		return myResult;
	}
	if ( bytesToBeWritten != -1)
	{
	bytesWrote = ACE_OS::write(dest, buf, bytesToBeWritten);

	ACE_OS::close(dest);
	ACE_OS::close(src);
	if(bytesWrote != bytesToBeWritten)
	{
		ERROR("ACS_LM_Common::restoreFile()  %s","Restore Failed");
		myResult = false;
	}
	}
	DEBUG("ACS_LM_Common::restoreFile()  %s","Leaving");
	return myResult;
}//end of restoreFile
/*=================================================================
	ROUTINE: backupFile
=================================================================== */
bool ACS_LM_Common::backupFile(const std::string& srcPath, const std::string& destPath)
{
	return restoreFile(srcPath,destPath);
}//end of backupFile
/*=================================================================
	ROUTINE: encode8
=================================================================== */
void ACS_LM_Common::encode8(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_TCHAR value)
{
	destBuf[destPos] = value;
	destPos += 1;
}//end of encode8
/*=================================================================
	ROUTINE: decode8
=================================================================== */
ACE_TCHAR ACS_LM_Common::decode8(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos)
{
	ACE_INT16 decodedValue = srcBuf[srcPos];
	srcPos += 1;
	return decodedValue;
}//end of decode8
/*=================================================================
	ROUTINE: encode16
=================================================================== */
void ACS_LM_Common::encode16(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT16 value)
{
	DEBUG ("ACS_LM_Common::encode16 destPos= %d and value = %d",destPos,value);
	destBuf[destPos] = (0xFF & value);
	destBuf[destPos+1] = ((0xFF00 & value) >> 8);
	destPos += 2;



}//end of encode16
/*=================================================================
	ROUTINE: encodechecksum16
=================================================================== */
void ACS_LM_Common::encodechecksum16(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT32 value)
{
	DEBUG ("ACS_LM_Common::encodechecksum16 destPos= %d and value = %d",destPos,value);
	destBuf[destPos] = (0xFF & value);
	destBuf[destPos+1] = ((0xFF00 & value) >> 8);
	destBuf[destPos+2] = ((0xFF0000 & value) >> 16);
	destBuf[destPos+3] = ((0xFF000000 & value) >> 24);
	destPos += 4;
}//end of encode16
/*=================================================================
	ROUTINE: decode16
=================================================================== */
ACE_UINT16 ACS_LM_Common::decode16(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos)
{
	DEBUG ("ACS_LM_Common::decode16 srcPos= %d",srcPos);
	ACE_INT16 decodedValue = (srcBuf[srcPos] & 0x000000FF);
	decodedValue = decodedValue + ((srcBuf[srcPos+1] & 0x000000FF) <<8);
	srcPos += 2;
	return decodedValue;
}//end of decode16

/*=================================================================
	ROUTINE: decode16
=================================================================== */
ACE_UINT32 ACS_LM_Common::decodechecksum16(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos)
{
	DEBUG ("ACS_LM_Common::decode16 srcPos= %d",srcPos);
	ACE_INT32 decodedValue = (srcBuf[srcPos] & 0x000000FF);
	decodedValue = decodedValue + ((srcBuf[srcPos+1] & 0x000000FF) <<8);
	decodedValue = decodedValue + ((srcBuf[srcPos+2] & 0x000000FF) <<16);
	decodedValue = decodedValue + ((srcBuf[srcPos+3] & 0x000000FF) <<24);
	srcPos += 4;
	return decodedValue;
}//end of decode16
/*=================================================================
	ROUTINE: encode32
=================================================================== */
/* Input argumnet (value) data type changed from ACE_UNIT64 to ACE_UINT32,
   to avoid switching from License Controlled mode to Grace mode during UP installation*/
void ACS_LM_Common::encode32(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT32 value)
{
	destBuf[destPos]   = (0xFF & (ACE_TCHAR)(value));
	destBuf[destPos+1] = (0xFF & (ACE_TCHAR)(value >> 8));
	destBuf[destPos+2] = (0xFF & (ACE_TCHAR)(value >> 16));
	destBuf[destPos+3] = (0xFF & (ACE_TCHAR)(value >> 24));
	destPos += 4;
}//end of encode32
/*=================================================================
	ROUTINE: decode32
=================================================================== */
ACE_UINT64 ACS_LM_Common::decode32(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos)
{
	ACE_UINT64 decodedValue = 0;
	decodedValue = decodedValue | (srcBuf[srcPos] & 0x000000FF);
	decodedValue = decodedValue | ((srcBuf[srcPos+1] << 8) & 0x0000FF00);
	decodedValue = decodedValue | ((srcBuf[srcPos+2] << 16) & 0x00FF0000);
	decodedValue = decodedValue | ((srcBuf[srcPos+3] << 24) & 0xFF000000);
	srcPos += 4;
	return decodedValue;
}//end of decode32
/*=================================================================
	ROUTINE: encode64
=================================================================== */
void ACS_LM_Common::encode64(ACE_TCHAR* destBuf, ACE_UINT64& destPos, ACE_UINT64 value)
{
	destBuf[destPos]   = (0xFF & (ACE_TCHAR)(value));
	destBuf[destPos+1] = (0xFF & (ACE_TCHAR)(value >> 8));
	destBuf[destPos+2] = (0xFF & (ACE_TCHAR)(value >> 16));
	destBuf[destPos+3] = (0xFF & (ACE_TCHAR)(value >> 24));
	destBuf[destPos+4] = (0xFF & (ACE_TCHAR)(value >> 32));
	destBuf[destPos+5] = (0xFF & (ACE_TCHAR)(value >> 40));
	destBuf[destPos+6] = (0xFF & (ACE_TCHAR)(value >> 48));
	destBuf[destPos+7] = (0xFF & (ACE_TCHAR)(value >> 56));
	destPos += 8;
}//end of encode64
/*=================================================================
	ROUTINE: decode64
=================================================================== */
ACE_UINT64 ACS_LM_Common::decode64(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos)
{
	ACE_UINT64 decodedValue = 0;
	decodedValue = decodedValue | (srcBuf[srcPos] & 0x00000000000000FF);
	decodedValue = decodedValue | ((srcBuf[srcPos+1] << 8) & 0x000000000000FF00);
	decodedValue = decodedValue | ((srcBuf[srcPos+2] << 16) & 0x0000000000FF0000);
	decodedValue = decodedValue | ((srcBuf[srcPos+3] << 24) & 0x00000000FF000000);
	decodedValue = decodedValue | (((unsigned long)(srcBuf[srcPos+4]) << 32) & 0x000000FF00000000);
	decodedValue = decodedValue | (((unsigned long)(srcBuf[srcPos+5]) << 40) & 0x0000FF0000000000);
	decodedValue = decodedValue | (((unsigned long)(srcBuf[srcPos+6]) << 48) & 0x00FF000000000000);
	decodedValue = decodedValue | (((unsigned long)(srcBuf[srcPos+7]) << 56) & 0xFF00000000000000);
	srcPos += 8;
	return decodedValue;
}//end of decode64
/*=================================================================
	ROUTINE: encodeString120
=================================================================== */
void ACS_LM_Common::encodeString120(ACE_TCHAR* destBuff, ACE_UINT64& destPos, const std::string& srcStr)
{
	const ACE_TCHAR* strBuff = srcStr.c_str();
	for(ACE_UINT16 i=0; i<srcStr.size() && i < 15; i++)
	{
		destBuff[destPos+i] = strBuff[i];
	}
	destPos += 15;
}//end of encodeString120
/*=================================================================
	ROUTINE: decodeString120
=================================================================== */
std::string ACS_LM_Common::decodeString120(const ACE_TCHAR* srcBuf, ACE_UINT64& srcPos)
{
	ACE_TCHAR buf[16];
	buf[15] = '\0';

	for(ACE_UINT16 i=0; i<15; i++)
	{
		buf[i] = (ACE_TCHAR)srcBuf[srcPos+i];
	}
	srcPos += 15;

	std::string str = buf;

	return str;
}//end of decodeString120
/*=================================================================
	ROUTINE: copyBuffer
=================================================================== */
void ACS_LM_Common::copyBuffer(ACE_TCHAR* destBuf, ACE_UINT64 destPos, const ACE_TCHAR* srcBuf, ACE_UINT64 srcPos, ACE_UINT64 srcLen)
{
	int destIndex =0;
	for(ACE_UINT16 i=srcPos; i<srcPos+srcLen; i++)
	{
		destBuf[destPos+destIndex] = srcBuf[i];
		destIndex++;
	}
}//end of copyBuffer
/*=================================================================
	ROUTINE: isStringValid
=================================================================== */
bool ACS_LM_Common::isStringValid(char* srcStr)
{
	bool valid = false;
	std::string str(srcStr);
	if((str.compare("011") != 0) && (str.compare("oil") != 0)
			&& (str.compare("Nil") != 0 )&& (str.compare("Ni") != 0))
	{
		for(int i = 0; i<(int)strlen(str.c_str());i++)
		{
			ACE_UINT32 myValue = srcStr[i];
			if((myValue >=37) && (myValue  <=127)  && (myValue !=34))
			{
				valid = true;
				continue;

			}
			else
			{
				valid = false;
				break;
			}
		}
	}

	else
	{
		valid = false;
	}
	return valid;
}//end of isStringValid
/*=================================================================
	ROUTINE: isParnameValid
=================================================================== */
bool ACS_LM_Common::isParnameValid(char* srcStr)
{
	bool valid = false;
	std::string str = srcStr;
	if((str.compare("011") != 0) && (str.compare("oil") != 0)
			&& (str.compare("Nil") != 0 )&& (str.compare("Ni") != 0))
	{
		for(int i = 0; i<(int)strlen(str.c_str());i++)
		{
			ACE_UINT32 mySrcStr = srcStr[i];

			if((mySrcStr>=37 &&  mySrcStr<=127) && (mySrcStr!=95) && (mySrcStr!=34 ))
			{
				valid = true;
				continue;
			}
			else
			{
				valid = false;
				break;
			}
		}
	}

	else
	{
		valid = false;
	}
	return valid;
}//end of isParnameValid
/*=================================================================
	ROUTINE: isAlphanumeric
=================================================================== */
bool ACS_LM_Common::isAlphanumeric(char* srcStr)
{
	bool valid = false;
	for(int i = 0; i<(int)strlen(srcStr);i++)
	{
		if((srcStr[i]>=48 && srcStr[i] <=57) || (srcStr[i]>=65 && srcStr[i] <=90) ||
				(srcStr[i]>=97 && srcStr[i] <=122))
		{
			valid = true;
			continue;
		}
		else
		{
			valid = false;
			break;
		}
	}
	return valid;
}//end of isAlphanumeric
/*=================================================================
	ROUTINE: getLmdataLockStatus
=================================================================== */

bool ACS_LM_Common::getLmdataLockStatus()
{
	return lmdataLocked;
}//end of getLmdataLockStatus
/*=================================================================
	ROUTINE: setLmdataLockStatus
=================================================================== */
void ACS_LM_Common::setLmdataLockStatus(bool lmdataLock)
{
	lmdataLocked = lmdataLock;
}//end of setLmdataLockStatus
/*=================================================================
	ROUTINE: getLkfLockStatus
=================================================================== */
//CNI30_4_1719(TR NO HN66027/HN40528)
bool ACS_LM_Common::getLkfLockStatus()
{
	return lkfLocked;
}//end of getLkfLockStatus
/*=================================================================
	ROUTINE: setLkfLockStatus
=================================================================== */
void ACS_LM_Common::setLkfLockStatus(bool lkfLock)
{
	lkfLocked = lkfLock;
}//end of setLkfLockStatus

/*=================================================================
	ROUTINE: fetchDnOfRootObjFromIMM
=================================================================== */
int ACS_LM_Common::fetchDnOfRootObjFromIMM (OmHandler* aOMHandler)
{
	DEBUG("ACS_LM_Common::fetchDnOfRootObjFromIMM()  %s","Entering");
	if (aOMHandler == 0)
	{
		return -1;
	}
	static int infoAlreadyLoad = 0;
	static char dnOfRootClass[512] = {0};

	if (infoAlreadyLoad) { dnOfLMRoot = std::string(dnOfRootClass); return 0; }

	std::vector<std::string> dnList;
	int returnCode = 0;

	if (aOMHandler->getClassInstances(ACS_IMM_LM_ROOT_CLASS_NAME, dnList) != ACS_CC_SUCCESS)
	{
		//ERROR
		returnCode = -1;
		ERROR("ACS_LM_Common::fetchDnOfRootObjFromIMM()  %s","Error found");
	}
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() ^ 1)
		{
			//WARNING: 0 (zero) or more than one node found
			WARNING("ACS_LM_Common::fetchDnOfRootObjFromIMM()  %s","0 (zero) or more than one node found");
			returnCode = -1;
		}
		else
		{
			//OK: Only one root node
			strncpy(dnOfRootClass, dnList[0].c_str(), ACS_ARRAY_SIZE(dnOfRootClass));
			dnOfRootClass[ACS_ARRAY_SIZE(dnOfRootClass) - 1] = 0;
			infoAlreadyLoad = 1;
			dnOfLMRoot = std::string(dnOfRootClass);
		}
	}
	DEBUG("ACS_LM_Common::fetchDnOfRootObjFromIMM()  %s","Leaving");
	return returnCode;
}//end of fetchDnOfRootObjFromIMM
/*=================================================================
	ROUTINE: StartThread
=================================================================== */
ACE_INT32 ACS_LM_Common::StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
		void* Arglist,
		const ACE_TCHAR* lpszThreadName,
		ACE_INT32 s32GrpId)
{
	DEBUG("%s", "Entering StartThread");
	ACE_INT32 s32Result =  s32Spawn( ThreadFunc,
			Arglist,
			lpszThreadName,
			s32GrpId);

	if ( s32Result == -1 )
	{
		ERROR("%s", "Thread start failed");
	}
	else
	{
		DEBUG("Thread is created successfully %s", lpszThreadName);
	}
	DEBUG("%s", "Leaving StartThread");

	return s32Result;

}//end of StartThread
/*=================================================================
	ROUTINE: s32Spawn
=================================================================== */
ACE_INT32 ACS_LM_Common::s32Spawn( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
		void * Arglist,
		const ACE_TCHAR* lpszThreadName,
		ACE_INT32 s32GrpId)
{
	DEBUG("%s", "Entering s32Spawn");

	ACE_INT32 s32Result =  ACE_Thread_Manager::instance()->spawn( ThreadFunc,
			Arglist,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			0,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			s32GrpId,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName);

	DEBUG("%s","Leaving s32Spawn");

	return s32Result;
}//end of s32Spawn

/*===================================================================
   ROUTINE:getLicenseKeyFilePath
=================================================================== */
bool ACS_LM_Common::getLicenseKeyFilePath(string &aLicenseKeyFilePath )
{
        DEBUG ("%s","Entered ACS_LM_Common::getLicenseKeyFilePath()");

        ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;

        string fileMFuncName = LM_FILEMPATH_ATTRIBUTE;

        char folderPath[1024] = { 0 };

        int folderPathLen = 1024;

        ACS_APGCC_CommonLib objCommonLib;

        returnType = objCommonLib.GetFileMPath( fileMFuncName.c_str(),
                        folderPath,
                        folderPathLen );

        if( returnType != ACS_APGCC_DNFPATH_SUCCESS )
        {
                DEBUG("%s", " Error occured while getting the FileMPath.");
                DEBUG("%s", "Leaving ACS_LM_Common::getLicenseKeyFilePath()");
                return false;
        }


        aLicenseKeyFilePath = folderPath;

        INFO("aLicenseKeyFilePath = %s", aLicenseKeyFilePath.c_str());

        DEBUG ("%s","Leaving ACS_LM_Common::getLicenseKeyFilePath");

        return true;
}//end of getLicenseKeyFilePath
/*=================================================================
        ROUTINE: getObsoluteFilePath
=================================================================== */
bool ACS_LM_Common::getObsoluteFilePath(std::string lkfFilePath, std::string &lkfObsolutePath)
{
	DEBUG ("%s","Entered acs_lm_common::getObsoluteFilePath()");
	string licenseKeyFilePath("");

	DEBUG("acs_lm_common::getObsoluteFilePath : lkfFilePath = %s", lkfFilePath.c_str());
	if(getLicenseKeyFilePath(licenseKeyFilePath) == false)
	{
		WARNING("%s", "acs_lm_common::getObsoluteFilePath - Error occurred while getting obsolute license key file path");
		return false;
	}
	std::string templkfFolderPath = "";
	templkfFolderPath += licenseKeyFilePath;
	templkfFolderPath += "/";
	templkfFolderPath += lkfFilePath;
	lkfObsolutePath = templkfFolderPath;
	INFO("acs_lm_common::getObsoluteFilePath : lkfObsolutePath = %s ", lkfObsolutePath.c_str());
	DEBUG ("%s","Leaving acs_lm_common::getObsoluteFilePath()");
	return true;
}//end of getObsoluteFilePath

bool ACS_LM_Common::doFileDelete(std::string path)
{

	DEBUG ("%s","ACS_LM_Common::doDirDelete-Entering");
	DIR *dir;
	struct dirent * finddata;
	std::string searchPattern = path;
	std::string fileN;
	dir = opendir(searchPattern.c_str());
	//char * problemdata = new char[50];
	DEBUG ("path = %s", path.c_str());
	if (dir == NULL)
	{
		//ACE_OS::sprintf(problemdata,"Not able to open file or directory: %s" , searchPattern.c_str());
		std::string problemdata;
		problemdata = "Not able to open file or directory" + searchPattern;
		DEBUG("ACS_LM_Common::doFileDelete : %s",problemdata.c_str());
		return false;
	}
	else
	{
		while ((finddata = readdir(dir)) != NULL)
		{
			if (strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, ".."))

			{
				fileN = path +"/"+ finddata->d_name;
				if(fileN.c_str()!=NULL)
				{
					int status  = ACE_OS::unlink(fileN.c_str());
					if(status !=0)
					{
						ERROR( "Failed to delete file , file path : %s , errno : %d",fileN.c_str(),ACE_OS::last_error());
						//Event::report(EVENT_FILENOTDEL,"REMOVE FAULT",fileN, "Not able to delete file or directory : "+fileN);
						if(dir != NULL)
							closedir(dir);
						//delete[] problemdata;
						return false;
					}
					else
					{
						DEBUG("file deleted , file path : %s",fileN.c_str());
					}


				}


			}
		}

		if(dir != NULL)
			closedir(dir);
	}
	//delete[] problemdata;
	return true;
}

void ACS_LM_Common::getClusterIPfromConfFile(std::string &clusterIPv4, std::string &clusterIPv6)
{
	DEBUG("ACS_LM_Common::getClusterIPfromConfFile() %s","Entering");
	std::string clusterIPfromConfFile("");
	std::string compareString_IPv4("mip control nbi");
	std::string compareString_IPv6("mip control nbi_v6");
	fstream clusterConfFile;
	std::string buffer("");
	clusterConfFile.open(LM_CLUSTER_CONF_FILE,ios::in);
	while(std::getline(clusterConfFile,buffer))
	{
		if ((compareString_IPv4 == buffer.substr(0,15)) || (compareString_IPv6 == buffer.substr(0,18)))
		{
			int lineLength;
			lineLength = buffer.size();
			size_t lineLastSpaceOffset;
			lineLastSpaceOffset = buffer.find_last_of(" ");
			clusterIPfromConfFile = buffer.substr(lineLastSpaceOffset+1, lineLength);
			if(clusterIPfromConfFile.find(".") != std::string::npos)
			{
				clusterIPv4.assign(clusterIPfromConfFile);
				DEBUG("ACS_LM_Common::getClusterIPfromConfFile() Cluster IPv4 from cluster.conf file = %s",clusterIPv4.c_str());
			}
			else if(clusterIPfromConfFile.find(":") != std::string::npos)
			{
				clusterIPv6.assign(clusterIPfromConfFile);
				DEBUG("ACS_LM_Common::getClusterIPfromConfFile() Cluster IPv6 from cluster.conf file = %s",clusterIPv6.c_str());
			}
		}
	}
	if(clusterConfFile.is_open())
	{
		clusterConfFile.close();
	}
	DEBUG("ACS_LM_Common::getClusterIPfromConfFile() %s","Leaving");
}

std::string ACS_LM_Common::getClusterIPfromConfFile()
{
        DEBUG("ACS_LM_Common::getClusterIPfromConfFile() %s","Entering");
        std::string clusterIPfromConfFile("");
        // Fix to support Reliable Ethernet feature
        //std::string compareString1("mip control nbi eth");
        //std::string compareString2("mip control nbi bond");

        //for TR HS73538
        std::string compareString("mip control nbi");

        fstream clusterConfFile;
        std::string buffer("");
        clusterConfFile.open(LM_CLUSTER_CONF_FILE,ios::in);
        while(std::getline(clusterConfFile,buffer))
        {
                //for TR HS73538
                if (compareString == buffer.substr(0,15) )
                {
                        int lineLength;
                        lineLength = buffer.size();
                        size_t lineLastSpaceOffset;
                        lineLastSpaceOffset = buffer.find_last_of(" ");
                        clusterIPfromConfFile = buffer.substr(lineLastSpaceOffset+1, lineLength);
                        DEBUG("ACS_LM_Common::getClusterIPfromConfFile() cluster IP from Conf file= %s",clusterIPfromConfFile.c_str());
                        break;
                }

                //Earlier Implementation
                #if 0
                if( compareString1 == buffer.substr(0,19) )
                {
                        clusterIPfromConfFile = buffer.substr(30);
                        break;
                }
                else if( compareString2 == buffer.substr(0,20) )
                {
                        clusterIPfromConfFile = buffer.substr(31);
                        break;
                }
                #endif
        }
        if(clusterConfFile.is_open())
        {
                clusterConfFile.close();
        }
        DEBUG("ACS_LM_Common::getClusterIPfromConfFile() %s","Leaving");
        return clusterIPfromConfFile;
}

int ACS_LM_Common::getApt()
{
        DEBUG("%s","Entering ACS_LM_Common::getApt()");
        const string axeApplicationAttr = "axeApplication";
        int* APT;
        ACS_APGCC_ImmAttribute attr1;
        ACS_CC_ReturnType returnCode;
        OmHandler immHandle;
        std::vector<std::string> pd_dnList;
        char *classNamepd = const_cast<char*>(ACS_AXEFUNCTIONS_CLASS_NAME);

        returnCode=immHandle.Init();

        if(returnCode!=ACS_CC_SUCCESS)
        {
        	return -1;
        	DEBUG("%s","Error In initializing Handle");
        }

        returnCode = immHandle.getClassInstances(classNamepd, pd_dnList);
        if(returnCode == ACS_CC_SUCCESS)
        {
                int numDef  = pd_dnList.size();
                if(numDef == 1)
                {
                        vector<string>::iterator it = pd_dnList.begin();
                        string DN = (*it);
                        /* Parse the above string here if needed to narrow down the DN to AxeFunctions=1*/
                        std::vector<ACS_APGCC_ImmAttribute *> attributes;
                        attr1.attrName = axeApplicationAttr;
                        attributes.push_back(&attr1);
                        returnCode = immHandle.getAttribute(DN.c_str(), attributes);
                        if(returnCode == ACS_CC_FAILURE)
                        {
                                DEBUG("%s","Leaving ACS_LM_Common::getApt()");
                                immHandle.Finalize();
                                return -1;
                        }
                        else
                        {

                                APT = reinterpret_cast<int*>(*(attr1.attrValues));
                                immHandle.Finalize();
                                DEBUG("%s","Leaving ACS_LM_Common::getApt()");
                                return *APT;
                        }

                }
                else
                {
                        DEBUG("%s","Leaving ACS_LM_Common::getApt()");
                        immHandle.Finalize();
                        return -1;
                }
        }
        else
        {
                DEBUG("%s","Leaving ACS_LM_Common::getApt()");
                immHandle.Finalize();
                return -1;
        }

}


