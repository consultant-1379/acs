//******************************************************************************
//
//  NAME
//     acs_lm_sentinel.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
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
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */



#include "acs_lm_sentinel.h"
#include "acs_lm_tra.h"
#include <lsinit.h>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Thread_Manager.h>
#include <ace/Time_Value.h>
#include "acs_lm_server.h" 
#include "acs_lm_common_util.h" /* HW-Decoupling: Fetching cluster IP address from APGCC  */
#include <ACS_CS_API.h>

using namespace std;


 VLScustomEx ACS_LM_Sentinel::customExList[VLS_MAX_CUSTOMEX_COUNT];
 int ACS_LM_Sentinel::cached =0;
 int ACS_LM_Sentinel::customExSize =0;
 bool ACS_LM_Sentinel::immIPv4FPRequested = false;
 bool ACS_LM_Sentinel::immIPv6FPRequested = false;
 bool ACS_LM_Sentinel::isVirtualNode = false;
 OmHandler*  ACS_LM_Sentinel::theOmHandlerPtr = 0;
 /*=================================================================
 	ROUTINE: ACS_LM_Sentinel constructor
 =================================================================== */
ACS_LM_Sentinel::ACS_LM_Sentinel(const std::string& curPath,
								 const std::string& bkpPath1,
								 const std::string& bkpPath2)
:hThread(NULL),
 bkpThreadStopEvent(NULL)
{
	DEBUG("ACS_LM_Sentinel::ACS_LM_Sentinel() %s","Entering");
	this->theCurrentPath = curPath;
	this->backupPath1 = bkpPath1;
	this->backupPath2 = bkpPath2;
	lkfCheckSum = 0;

	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;

	if(ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture) == ACS_CS_API_NS::Result_Success)
	{
		if (nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED){
			isVirtualNode = true;
			DEBUG("ACS_LM_Sentinel::ACS_LM_Sentinel() - %s","Node is VIRTUAL");
		}
	}
	else
		WARNING("ACS_LM_Sentinel::ACS_LM_Sentinel() - %s","Failed to get node architecture using getNodeArchitecture() - assuming NATIVE");
	DEBUG("ACS_LM_Sentinel::ACS_LM_Sentinel() %s","Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_Sentinel destructor
=================================================================== */
ACS_LM_Sentinel::~ACS_LM_Sentinel()
{
	DEBUG("ACS_LM_Sentinel::~ACS_LM_Sentinel() %s","Entering");

	if(hThread != 0)
	{
		ACE_Thread_Manager::instance()->cancel(hThread);
		ACE_Thread_Manager::instance()->close();
		hThread = NULL;
	}
	clean();
	DEBUG("ACS_LM_Sentinel::~ACS_LM_Sentinel() %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: restore
=================================================================== */
bool ACS_LM_Sentinel::restore()
{
	DEBUG("ACS_LM_Sentinel::restore() %s","Entering");

	bool restore = true;

	if(ACS_LM_Common::isFileExists(theCurrentPath))
        {
                if(ACS_LM_Common::deleteFile(theCurrentPath))
                {
                        DEBUG("ACS_LM_Sentinel::restore() %s","LKF file removed from the CurrentPath"); 
                }
                else
                {
                        DEBUG("ACS_LM_Sentinel::restore() %s","ERROR!!! LKF file not removed from the CurrentPath");
                }
        }
	bool myRestoreResult = ACS_LM_Common::restoreFile(backupPath1, theCurrentPath);
	if(!myRestoreResult)
	{
		restore = false;
	}

	DEBUG("ACS_LM_Sentinel::restore() %s","Leaving");
	return restore;
}//end of restore
/*=================================================================
	ROUTINE: backup
=================================================================== */
bool ACS_LM_Sentinel::backup()
{
	DEBUG("ACS_LM_Sentinel::backup() %s","Entering");
		bool backup = true;

	//TR HP46656
	if(ACS_LM_Common::isFileExists(backupPath1))
	{
		if(ACS_LM_Common::deleteFile(backupPath1))
		{
			DEBUG("ACS_LM_Sentinel::installLkf() %s","LKF file removed from the BackupPath");
		}
		else
		{
			DEBUG("ACS_LM_Sentinel::installLkf() %s","ERROR!!! LKF file not removed from the BackupPath");
		}
	}
	//END OF TR HP46656


	 //CNI30_4_1719(TR NO HN66027/HN40528)
	for(int attempt =0;attempt<5;attempt++)
	{
		bool myResult = ACS_LM_Common::backupFile(theCurrentPath, backupPath1);
		if(myResult == false)
			{
				DEBUG("ACS_LM_Sentinel::backup() ACS_LM_Common::backupFile(theCurrentPath= %s, backupPath1= %s  ) Failed",theCurrentPath.c_str(),backupPath1.c_str());
				backup = false;
			}
		else
		{
			backup = true;
			break;
		}
	}

	DEBUG("ACS_LM_Sentinel::backup() %s","Leaving");
	return backup;
}//end of backup
/*=================================================================
	ROUTINE: getCheckSum
=================================================================== */
ACE_UINT32 ACS_LM_Sentinel::getCheckSum()
{
	DEBUG("ACS_LM_Sentinel::getCheckSum() %s, current path=%s","Entering",theCurrentPath.c_str());
	lkfCheckSum = ACS_LM_Common::getFileChecksum(theCurrentPath, 0, 0);
	DEBUG("ACS_LM_Sentinel::getCheckSum() primary path Lkf checksum=%d", lkfCheckSum);
	if(lkfCheckSum == 0)
	{
		lkfCheckSum = ACS_LM_Common::getFileChecksum(theCurrentPath, 0, 0);
		DEBUG("The value of  lkfCheckSum from sentinel->getCheckSum()Inside condition: %d",lkfCheckSum);
	}
	DEBUG("ACS_LM_Sentinel::getCheckSum() %s","Leaving");
	return lkfCheckSum;

}//end of getCheckSum
/*=================================================================
	ROUTINE: getCheckSumBackup
=================================================================== */
//CNI30_4_1719(TR NO HN66027/HN40528)
ACE_UINT32 ACS_LM_Sentinel::getCheckSumBackup()
{
	DEBUG("ACS_LM_Sentinel::getCheckSumBackup() %s, backup path=%s","Entering", backupPath1.c_str());
	lkfCheckSum = ACS_LM_Common::getFileChecksum(backupPath1, 0, 0);
	DEBUG("ACS_LM_Sentinel::getCheckSumBackup() backup path Lkf checksum=%d", lkfCheckSum);
	if(lkfCheckSum == 0)
	{
		lkfCheckSum = ACS_LM_Common::getFileChecksum(backupPath1, 0, 0);
	}
	DEBUG("ACS_LM_Sentinel::getCheckSumBackup() %s","Leaving");
	return lkfCheckSum;

}//end of getCheckSumBackup
/*=================================================================
	ROUTINE: init
=================================================================== */
bool ACS_LM_Sentinel::init(bool& aNodeInitialized, bool& bNodeInitialized, bool newLkf, ACE_UINT32 checkSum, bool LoadedBkpFile)
{
	DEBUG("ACS_LM_Sentinel::init() %s","Entering");
	bool initialized = false;
	ACE_UINT32 lkf_CheckSum = 0;

	LS_STATUS_CODE lsStatusCode = LS_SUCCESS;

	//To set the LSERRV file name path
	lsStatusCode = VLSsetFileName(VLS_LSERVRC, (unsigned char*)theCurrentPath.c_str(), NULL, NULL);
	//DEBUG("init() VLSsetFileName() lsStatusCode = %d",lsStatusCode);
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLSsetFileName() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLSsetFileName() Failed");
	}
	std::string myStrNoNet("NO_NET");
	lsStatusCode = VLSsetContactServer((char*)myStrNoNet.c_str());
	//DEBUG("init() VLSsetFileName() lsStatusCode = %d",lsStatusCode);
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLSsetContactServer() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLSsetContactServer() Failed");
	}

	//To disable the Sentinle Error Popups and Printouts.
	VLSerrorHandle(VLS_OFF);
	// lsStatusCode = ::VLSsetUserErrorFile(VLS_STDERR,(char*)"/data/acs/data/lm/lm.log");

	lsStatusCode = VLScontrolRemoteSession(VLS_OFF);
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLScontrolRemoteSession() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLScontrolRemoteSession() Failed");
	}
	//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "init() VLS_INITIALIZE() lsStatusCode="<<lsStatusCode);

	//Initializes the SentinelRMS LM
	lsStatusCode = VLSinitialize();
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLSinitialize() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::init() %s","VLSinitialize() Failed");
	}

	lsStatusCode = VLSsetCustomExFunc(&ACS_LM_Sentinel::getCustomExValue);
	//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "init() VLSsetCustomExFunc() lsStatusCode="<<lsStatusCode);
	if(lsStatusCode == LS_SUCCESS)
	{		
		DEBUG("ACS_LM_Sentinel::init() %s","VLSsetCustomExFunc() Passed");
		if(newLkf)
		{
			INFO("ACS_LM_Sentinel::init() %s","New LKF");
			if(isLkfValid(aNodeInitialized, bNodeInitialized))
			{
				initialized = true;
			}
		}
		else
		{
			DEBUG("ACS_LM_Sentinel::init() %s,checkSum=%d","newLkf = false",checkSum);
			if(checkSum == 0)
			{
				initialized = true;
				INFO("ACS_LM_Sentinel::init() %s","checksum is zero");
			}
			else
			{
				lkf_CheckSum = getCheckSum();
				DEBUG("ACS_LM_Sentinel::init() ACS_LM_Server::isOldDirectoryDeleted=%d, lkf_Checksum=%d, LoadedBkpFile=%d",ACS_LM_Server::isOldDirectoryDeleted,lkf_CheckSum,LoadedBkpFile);
				if((ACS_LM_Server::isOldDirectoryDeleted == true)||(checkSum == lkf_CheckSum))	
				{
					if(isLkfValid(aNodeInitialized, bNodeInitialized))
					{
						initialized = true;
					}
					else
					{
						ERROR("ACS_LM_Sentinel::init() %s","isLkfValid() failed");
					}
					(void)isEmergencyModeAllowed();                    
				}
				else
				{
					INFO("ACS_LM_Sentinel::init() %s","checkSum is not equal to getCheckSum()");
					if(!LoadedBkpFile )
					{
						ACS_LM_Server::pp_LKF_Checksum = lkf_CheckSum;
					}
				}
			}
		}
	}
	else
	{
		if (lsStatusCode == LS_NORESOURCES)
		{
			ERROR("ACS_LM_Sentinel::init() %s","VLSsetCustomExFunc() failed LS_NORESOURCES");
		}
		else if(lsStatusCode == VLS_RESOURCE_LOCK_FAILURE)
		{
			ERROR("ACS_LM_Sentinel::init() %s","VLSsetCustomExFunc() failed VLS_RESOURCE_LOCK_FAILURE");

		}
	}
	DEBUG("ACS_LM_Sentinel::init() %s","Leaving");
	return initialized;

}//end of init
/*=================================================================
	ROUTINE: initFromBackup
=================================================================== */
 //CNI30_4_1719(TR NO HN66027/HN40528)
bool ACS_LM_Sentinel::initFromBackup(bool& aNodeInitialized, bool& bNodeInitialized, bool newLkf, ACE_UINT32 checkSum)
{
	DEBUG("ACS_LM_Sentinel::initFromBackup() %s","Entering");
	bool initialized = false;
	ACE_UINT32 lkf_CheckSum = 0;

	LS_STATUS_CODE lsStatusCode = LS_SUCCESS;

	//To set the LSERRV file name path
	lsStatusCode = VLSsetFileName(VLS_LSERVRC, (unsigned char*)backupPath1.c_str(), NULL, NULL);
	//DEBUG("init() VLSsetFileName() lsStatusCode = %d",lsStatusCode);
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLSsetFileName() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLSsetFileName() Failed");
	}
	std::string myStrNoNet("NO_NET");
	lsStatusCode = VLSsetContactServer((char*)myStrNoNet.c_str());
	//DEBUG("init() VLSsetFileName() lsStatusCode = %d",lsStatusCode);
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLSsetContactServer() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLSsetContactServer() Failed");
	}

	//To disable the Sentinle Error Popups and Printouts.
	VLSerrorHandle(VLS_OFF);
	// lsStatusCode = ::VLSsetUserErrorFile(VLS_STDERR,(char*)"/data/acs/data/lm/lm.log");

	lsStatusCode = VLScontrolRemoteSession(VLS_OFF);
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLScontrolRemoteSession() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLScontrolRemoteSession() Failed");
	}
	//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "init() VLS_INITIALIZE() lsStatusCode="<<lsStatusCode);

	//Initializes the SentinelRMS LM
	lsStatusCode = VLSinitialize();
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLSinitialize() Passed");
	}
	else
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLSinitialize() Failed");
	}

	lsStatusCode = VLSsetCustomExFunc(&ACS_LM_Sentinel::getCustomExValue);
	//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "init() VLSsetCustomExFunc() lsStatusCode="<<lsStatusCode);
	if(lsStatusCode == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::initFromBackup() %s","VLSsetCustomExFunc() Passed");
		if(newLkf)
		{
			INFO("ACS_LM_Sentinel::initFromBackup() %s","New LKF");
			if(isLkfValidBackup(aNodeInitialized, bNodeInitialized))
			{
				initialized = true;
			}
		}
		else
		{
			DEBUG("ACS_LM_Sentinel::initFromBackup() %s,checkSum=%d","newLkf = false",checkSum);
			if(checkSum == 0)
			{
				initialized = true;
				INFO("ACS_LM_Sentinel::initFromBackup() %s","checksum is zero");
			}
			else
			{
				 //CNI30_4_1719(TR NO HN66027/HN40528)
				lkf_CheckSum = getCheckSumBackup();
		DEBUG("ACS_LM_Sentinel::initFromBackup() ACS_LM_Server::isOldDirectoryDeleted=%d,ACS_LM_Server::pp_LKF_Checksum=%d,lkf_CheckSum=%d",ACS_LM_Server::isOldDirectoryDeleted,ACS_LM_Server::pp_LKF_Checksum,lkf_CheckSum);
                                if((ACS_LM_Server::isOldDirectoryDeleted == true)||(checkSum == lkf_CheckSum)||(ACS_LM_Server::pp_LKF_Checksum == lkf_CheckSum)) 
				{
					if(isLkfValidBackup(aNodeInitialized, bNodeInitialized))
					{
						initialized = true;
					}
					else
					{
						DEBUG("ACS_LM_Sentinel::initFromBackup() %s","isLkfValidBackup() failed");
					}
					(void)isEmergencyModeAllowed();
				}
				else
				{
					INFO("ACS_LM_Sentinel::initFromBackup() %s","checkSum is not equal to ()");
				}
			}
		}
	}
	else
	{
		if (lsStatusCode == LS_NORESOURCES)
		{
			ERROR("ACS_LM_Sentinel::initFromBackup() %s","VLSsetCustomExFunc() failed LS_NORESOURCES");
		}
		else if(lsStatusCode == VLS_RESOURCE_LOCK_FAILURE)
		{
			ERROR("ACS_LM_Sentinel::initFromBackup() %s","VLSsetCustomExFunc() failed VLS_RESOURCE_LOCK_FAILURE");

		}
	}
	DEBUG("ACS_LM_Sentinel::initFromBackup() %s","Leaving");
	return initialized;

}//end of initFromBackup

/*=================================================================
	ROUTINE: clean
=================================================================== */
void ACS_LM_Sentinel::clean()
{
	DEBUG("ACS_LM_Sentinel::clean() %s","Entering");
	try
	{
		::VLScleanup();
		lkfCheckSum = 0;
	}
	catch(...)
	{
		DEBUG("%s","Exception is occurred in ::VLScleanup()");
	}
	DEBUG("ACS_LM_Sentinel::clean() %s","Leaving");
}//end of clean
/*=================================================================
	ROUTINE: getCustomExValue
=================================================================== */
long   ACS_LM_Sentinel::getCustomExValue(VLScustomEx* pTable, unsigned long* pCount)
{
	// Frequently called function. Traces are therefore disabled for performance improvement
	char customEx[VLS_CUSTOMEX_SIZE + 1];
	int  i, len;
	// Recommend to cache custom ex values to speed. In SafeNet sample, cache behavior doens't make sense as 
	// custom ex values are defined in static buffer. In order to explain how to cache data for developers, 
	// cache behavior still is leaved behind.

	::memset(customEx, 0, sizeof(customEx));

	if(pCount == NULL)
	   return 1;

	if(!cached)
	{
		if(!isVirtualNode)
		{
			std::string sid;
			(void)ACS_LM_Sentinel::fetchSid(sid);
			memset(customExList, 0, sizeof(customExList));
			len = (int)sid.size();
			strncpy((char*)(customExList[0].customEx), sid.c_str(), len);
			customExList[0].len = len;
			customExSize++;
		}
		else
		{
			/* In Dual Stack virtual node,
			 customExList[0] will always have IPv4 FP and customExList[1] will always have IPv6 FP*/
			int index = 0;
			std::string sid_ipv4, sid_ipv6;
			(void)ACS_LM_Sentinel::fetchSid(sid_ipv4,sid_ipv6);
			memset(customExList, 0, sizeof(customExList));
			if(!sid_ipv4.empty())
			{
				len = (int)sid_ipv4.size();
				strncpy((char*)(customExList[index].customEx), sid_ipv4.c_str(), len);
				customExList[index].len = len;
				customExSize++;
				index++;
			}
			if(!sid_ipv6.empty())
			{
				len = (int)sid_ipv6.size();
				strncpy((char*)(customExList[index].customEx), sid_ipv6.c_str(), len);
				customExList[index].len = len;
				customExSize++;
			}
		}
		cached = 1;
	}

	if(customExSize == 0)
		return 1;

	// Return size of the buffer receive custom ex value. 
	if(pTable == NULL)
	{
		if(isVirtualNode && (immIPv4FPRequested || immIPv6FPRequested))	// DualStack - Return only 1 FP to Sentinel due to Runtime Callback for LicenseM MO
			*pCount = 1;
		else
			*pCount = customExSize;
		return 0;
	}

	if(isVirtualNode && (immIPv4FPRequested || immIPv6FPRequested))	// DualStack
	{
		if(*pCount < 1)
		{
			*pCount = 1;
			return 1;
		}
	}
	else
	{
		// The size of allocated buffer can't be less than customExSize
		if(*pCount < (unsigned int)customExSize)
		{
			*pCount = customExSize;
			return 1;
		}
	}

	if(isVirtualNode && (immIPv4FPRequested || immIPv6FPRequested))
	{	// DualStack - Return only 1 FP to Sentinel due to Runtime Callback for LicenseM MO
		if(immIPv4FPRequested)
			*pTable = customExList[0];		// return only IPv4 FP
		else if(immIPv6FPRequested)
		{
			if(customExSize == 2)
				*pTable = customExList[1];		// return only IPv6 FP
			else
				*pTable = customExList[0];	// something wrong - size should be 2 because we are in Dual Stack config. Return FP in index 0.
		}
		*pCount = 1;
	}
	else
	{
		for(i = 0; i < customExSize; i++)
		{
			*(pTable + i) = customExList[i];
		}
		*pCount = customExSize;
	}
	return 0;
}//end of getCustomExValue

/*=================================================================
	ROUTINE: setInternalOMhandler
=================================================================== */
void ACS_LM_Sentinel::setInternalOMhandler(OmHandler*  aOmHandlerPtr)
{
	theOmHandlerPtr = aOmHandlerPtr;
}//end of setInternalOMhandler
/*=================================================================
	ROUTINE: fetchSid
=================================================================== */
bool ACS_LM_Sentinel::fetchSid(std::string& sid)
{

#ifdef FPSTUB
	DEBUG("ACS_LM_Sentinel::fetchSid() INSIDE FPSTUB - %s",FPSTUB);
	sid = FPSTUB;
	return true;
#endif

	DEBUG("ACS_LM_Sentinel::fetchSid() %s","Entering");
	bool fetched = false;
	/* HW-Decoupling: Ignoring "generateFingerPrint" method defined on "ACS_LM_Common" class.*/
	bool myFingerPrintFlag= ACS_LM_Common_Util::generateFingerPrint(sid,theOmHandlerPtr);
	if(!myFingerPrintFlag)
	{
		ERROR("ACS_LM_Sentinel::fetchSid() %s","ACS_LM_Common::generateFingerPrint() failed");
	}
	else
	{
		fetched = true;
	}
	DEBUG("ACS_LM_Sentinel::fetchSid() %s","Leaving");
	return fetched;

}//end of fetchSid
/*=================================================================
 * 	ROUTINE: fetchSid
=================================================================== */
bool ACS_LM_Sentinel::fetchSid(std::string& IPv4Fingerprint, std::string& IPv6Fingerprint)
{

#ifdef FPSTUB
	DEBUG("ACS_LM_Sentinel::fetchSid() INSIDE FPSTUB - %s",FPSTUB);
	sid = FPSTUB;
	return true;
#endif

	DEBUG("ACS_LM_Sentinel::fetchSid() %s","Entering");
	bool fetched = false;
	/* HW-Decoupling: Ignoring "generateFingerPrint" method defined on "ACS_LM_Common" class.*/
	bool myFingerPrintFlag= ACS_LM_Common_Util::generateFingerPrintForLicenseM(IPv4Fingerprint,IPv6Fingerprint,theOmHandlerPtr);
	if(!myFingerPrintFlag)
	{
		ERROR("ACS_LM_Sentinel::fetchSid() %s","ACS_LM_Common::generateFingerPrint() failed");
	}
	else
	{
		fetched = true;
	}
	DEBUG("ACS_LM_Sentinel::fetchSid() IPv4Fingerprint[%s] && IPv6Fingerprint[%s] - Leaving",IPv4Fingerprint.c_str(),IPv6Fingerprint.c_str());
	return fetched;

}//end of fetchSid
/*=================================================================
	ROUTINE: generateFingerPrint
=================================================================== */
bool ACS_LM_Sentinel::generateFingerPrint()
{
	DEBUG("ACS_LM_Sentinel::generateFingerPrint() %s","Entering");
	//bool generated = false;
//	bool generated;
	::VLSmachineID machineID;
	unsigned long actualMask;

	// Support for new style lock code
	char lockCode[VLS_LOCK_CODE_SIZE] = "";

	// Get the machine ID using  the API function 
	if(::VLSgetMachineID(VLS_LOCK_CUSTOMEX, &machineID, &actualMask) == LS_SUCCESS)
	{
		DEBUG("ACS_LM_Sentinel::generateFingerPrint() - actualMask == %d",actualMask);
		if(::VLSmachineIDToLockCodeEx(&machineID, actualMask, lockCode, sizeof(lockCode), 0) == LS_SUCCESS)
		{
			//bool generated;
			char a[16] = {0};
			ACE_OS::itoa(actualMask,a,16);
			fingerPrint = a;
			fingerPrint += "-";
			fingerPrint += formatFingerPrint(lockCode);
			//generated = true;
		}
	}
	fingerPrint = fingerPrint.substr(0,fingerPrint.length() - 1);
	DEBUG("ACS_LM_Sentinel::generateFingerPrint() %s","Leaving");
	return true;
}//end of generateFingerPrint

/*=================================================================
	ROUTINE: generateFingerPrint
=================================================================== */
bool ACS_LM_Sentinel::generateFingerprint_DualStack()
{
	immIPv4FPRequested = false;
	immIPv6FPRequested = false;
	DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack() %s","Entering");
	DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack(1) - immIPv4FPRequested == %d, immIPv6FPRequested == %d",immIPv4FPRequested,immIPv6FPRequested);
	::VLSmachineID machineID_1,machineID_2;
	unsigned long actualMask;
	string fingerprintIPv4 = "", fingerprintIPv6 = "";

	// Support for new style lock code
	char lockCode_IPv4[VLS_LOCK_CODE_SIZE] = "";
	char lockCode_IPv6[VLS_LOCK_CODE_SIZE] = "";

	immIPv4FPRequested = true;
	DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack(2) - Flag for IPv4 hashed fingerprint set to %s", "TRUE");
	// Get the machine ID using  the API function
	if(::VLSgetMachineID(VLS_LOCK_CUSTOMEX, &machineID_1, &actualMask) == LS_SUCCESS)
	{
		if(::VLSmachineIDToLockCodeEx(&machineID_1, actualMask, lockCode_IPv4, sizeof(lockCode_IPv4), 0) == LS_SUCCESS)
		{
			char a[16] = {0};
			ACE_OS::itoa(actualMask,a,16);
			fingerprintIPv4 = a;
			fingerprintIPv4 += "-";
			fingerprintIPv4 += formatFingerPrint(lockCode_IPv4);
		}
	}
	immIPv4FPRequested = false;
	immIPv6FPRequested = true;
	DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack(3) - Flag for IPv4 FP set to FALSE. Flag for hashed fingerprint set to %s", "TRUE");
	if(!fingerprintIPv4.empty())
	{
		fingerprintIPv4 = fingerprintIPv4.substr(0,fingerprintIPv4.length() - 1);
		DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack() - Hashed Fingerprint IPv4 is %s",fingerprintIPv4.c_str());
		fingerPrint.assign(fingerprintIPv4);
	}

	actualMask = 0;
	if(::VLSgetMachineID(VLS_LOCK_CUSTOMEX, &machineID_2, &actualMask) == LS_SUCCESS)
	{
		if(::VLSmachineIDToLockCodeEx(&machineID_2, actualMask, lockCode_IPv6, sizeof(lockCode_IPv6), 0) == LS_SUCCESS)
		{
			char a[16] = {0};
			ACE_OS::itoa(actualMask,a,16);
			fingerprintIPv6 = a;
			fingerprintIPv6 += "-";
			fingerprintIPv6 += formatFingerPrint(lockCode_IPv6);
		}
	}
	immIPv6FPRequested = false;
	immIPv4FPRequested = false;
	if(!fingerprintIPv6.empty())
	{
		fingerprintIPv6 = fingerprintIPv6.substr(0,fingerprintIPv6.length() - 1);
		DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack() - Hashed Fingerprint IPv6 is %s",fingerprintIPv6.c_str());
                //IPV6 SpillOver. Commented below code so that only IPv4 hashed fingerprint is reported in case of Dual stack.
		//fingerPrint += "," + fingerprintIPv6;
	}
	DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack() - Dual Stack hashed fingerprints:  [%s]",fingerPrint.c_str());
	DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack(8) - immIPv4FPRequested == %d, immIPv6FPRequested == %d",immIPv4FPRequested,immIPv6FPRequested);
	DEBUG("ACS_LM_Sentinel::generateFingerprint_DualStack() %s","Leaving");
	return true;
}//end of generateFingerPrint

/*=================================================================
	ROUTINE: formatFingerPrint
=================================================================== */
std::string ACS_LM_Sentinel::formatFingerPrint(const char* lockCode)
{
	DEBUG("ACS_LM_Sentinel::formatFingerPrint() %s","Entering");

	std::string formattedFp = "";	

	if (lockCode == NULL)
	{
		return NULL;
	}
	int len = (int)strlen(lockCode);
	if (lockCode[0] == '\0')
	{
		formattedFp.assign("0");
	}

	// 4 chars one group 
	for (int i = 0; i < len; i++)
	{
		formattedFp += lockCode[i];
		if ((i + 1) % 4 == 0)
		{
			formattedFp += ' ';
		}
	}
	DEBUG("ACS_LM_Sentinel::formatFingerPrint() %s","Leaving");
	return formattedFp;
}//end of formatFingerPrint
/*=================================================================
	ROUTINE: getFingerPrint
=================================================================== */
std::string ACS_LM_Sentinel::getFingerPrint()
{
	DEBUG("ACS_LM_Sentinel::getFingerPrint() %s","Entering");
	generateFingerPrint();
	DEBUG("ACS_LM_Sentinel::getFingerPrint() %s","Leaving");
	return fingerPrint;
}//end of getFingerPrint
/*=================================================================
	ROUTINE: getFingerPrint(bool isDualStack)
=================================================================== */
std::string ACS_LM_Sentinel::getFingerprint(bool isDualStack)
{
	DEBUG("ACS_LM_Sentinel::getFingerprint() %s","Entering");
	if(isDualStack)
		generateFingerprint_DualStack();
	else
		generateFingerPrint();
	DEBUG("ACS_LM_Sentinel::getFingerprint() %s","Leaving");
	return fingerPrint;
}//end of getFingerPrint(bool isDualStack)
/*=================================================================
	ROUTINE: isLkfValid
=================================================================== */
bool ACS_LM_Sentinel::isLkfValid(bool& aNodeInitialized, bool& bNodeInitialized)
{
	DEBUG("ACS_LM_Sentinel::isLkfValid() %s","Entering");

	bool valid = false;
	std::string thisNodeIP = ACS_LM_Common::getThisNodeIp();

	std::vector<ACS_LM_Sentinel::Key> lkeys;

	getKeysFromLservrc(theCurrentPath, lkeys);

	unsigned int vlsKeyCount = 0;
	unsigned int manualKeyCount = (unsigned int)lkeys.size();

	::VLSfeatureInfo fInfo;
	fInfo.structSz = sizeof(fInfo);
	//GUID      : Use unique GUID for each feature installation.
	const char* GUID = "01866F6D-A1C4-4681-BEE5-4DD6F3688912";

	for(std::vector<ACS_LM_Sentinel::Key>::iterator it = lkeys.begin();
			it != lkeys.end(); ++it)
	{
		ACS_LM_Sentinel::Key key = (*it);

		LS_STATUS_CODE lsLicenseStatus = ::VLSgetFeatureInfo((unsigned char*)(key.id).c_str(), (unsigned char*)(key.ver).c_str(), 0, NULL, &fInfo);
		INFO("ACS_LM_Sentinel::isLkfValid() VLSgetFeatureInfo is %ul",lsLicenseStatus);
		if((LS_SUCCESS == lsLicenseStatus) || (LS_INSUFFICIENTUNITS == lsLicenseStatus))
		{
			vlsKeyCount++;

		}

	}

	WARNING("ACS_LM_Sentinel::isLkfValid() vlsKeyCount is %d",vlsKeyCount);
	WARNING("ACS_LM_Sentinel::isLkfValid() manualKeyCount is %d",manualKeyCount);
	if(aNodeInitialized)
		INFO("ACS_LM_Sentinel::isLkfValid() - %s","Node A init ALREADY TRUE");
	if(bNodeInitialized)
		INFO("ACS_LM_Sentinel::isLkfValid() - %s","Node B init ALREADY TRUE");

	if((vlsKeyCount ==  manualKeyCount) && (vlsKeyCount != 0))
	{
		for(std::vector<ACS_LM_Sentinel::Key>::iterator it = lkeys.begin();
				it != lkeys.end(); ++it)
		{
			ACS_LM_Sentinel::Key key = (*it);
			LS_STATUS_CODE lsLicenseStatus = ::VLSgetFeatureInfo((unsigned char*)(key.id).c_str(), (unsigned char*)(key.ver).c_str(), 0, NULL, &fInfo); 
			if((LS_SUCCESS == lsLicenseStatus) || (LS_INSUFFICIENTUNITS == lsLicenseStatus))
			{		
				if(!aNodeInitialized || !bNodeInitialized)
				{
					//should be called during installation of LK file to solve clock tamper problem
					LS_STATUS_CODE rc = ::sntlInitStandaloneSystem((char*)GUID, (char*)(key.id).c_str(), (char*)(key.ver).c_str());
					//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isLkfValid() rc="<<rc);
					WARNING("ACS_LM_Sentinel::isLkfValid() - #1 Invoked sntlInitStandaloneSystem() - retCode[%lu]",rc);
				}
				else
				{
					INFO("ACS_LM_Sentinel::isLkfValid() - %s","Sentinel already initialized on both nodes - check for CLK_TAMP err");
					LS_STATUS_CODE lsRequestStatus = LS_SUCCESS;
					LS_HANDLE lsHandle = NULL;
					lsRequestStatus = ::VLS_REQUEST((unsigned char*)key.id.c_str(), (unsigned char*)key.ver.c_str(), &lsHandle);
					INFO("ACS_LM_Sentinel::isLkfValid() - VLS_REQUEST(%s,%s), retCode[%lu].",key.id.c_str(),key.ver.c_str(),lsRequestStatus);
					if(lsRequestStatus == VLS_CLK_TAMP_FOUND)
					{
						ERROR("ACS_LM_Sentinel::isLkfValid() - VLS_REQUEST(%s,%s), time tamper error found!.",key.id.c_str(),key.ver.c_str());
						//should be called during installation of LK file to solve clock tamper problem
						LS_STATUS_CODE rc = ::sntlInitStandaloneSystem((char*)GUID, (char*)(key.id).c_str(), (char*)(key.ver).c_str());
						WARNING("ACS_LM_Sentinel::isLkfValid() - #2 Invoked sntlInitStandaloneSystem(%s,%s), retCode[%lu]",key.id.c_str(),key.ver.c_str(),rc);
					}
					::VLS_RELEASE(lsHandle);
				}
			}
		}
		if((thisNodeIP.compare(AP1_NET1_NODEA)== 0) || (thisNodeIP.compare(AP1_NET2_NODEA)== 0))
		{
			aNodeInitialized = true;						
		}
		else if((thisNodeIP.compare(AP1_NET1_NODEB)== 0) || (thisNodeIP.compare(AP1_NET2_NODEB)== 0))
		{
			bNodeInitialized = true;						
		}    
		valid = true;
	}
	else
	{
		customExSize =0;
		cached =0;
		ERROR("ACS_LM_Sentinel::isLkfValid() %s","(vlsKeyCount ==  manualKeyCount) && (vlsKeyCount != 0)) Failed");
	}
	lkeys.clear();
	DEBUG("ACS_LM_Sentinel::isLkfValid() %s","Leaving");
	return valid;

}//end of isLkfValid
/*=================================================================
	ROUTINE: isLkfValidBackup
=================================================================== */
//CNI30_4_1719(TR NO HN66027/HN40528)
bool ACS_LM_Sentinel::isLkfValidBackup(bool& aNodeInitialized, bool& bNodeInitialized)
{
	DEBUG("ACS_LM_Sentinel::isLkfValidBackup() %s","Entering");

	bool valid = false;
	std::string thisNodeIP = ACS_LM_Common::getThisNodeIp();

	std::vector<ACS_LM_Sentinel::Key> lkeys;

	getKeysFromLservrc(backupPath1, lkeys);

	unsigned int vlsKeyCount = 0;
	unsigned int manualKeyCount = (unsigned int)lkeys.size();

	::VLSfeatureInfo fInfo;
	fInfo.structSz = sizeof(fInfo);
	//GUID      : Use unique GUID for each feature installation.
	const char* GUID = "01866F6D-A1C4-4681-BEE5-4DD6F3688912";

	for(std::vector<ACS_LM_Sentinel::Key>::iterator it = lkeys.begin();
			it != lkeys.end(); ++it)
	{
		ACS_LM_Sentinel::Key key = (*it);

		LS_STATUS_CODE lsLicenseStatus = ::VLSgetFeatureInfo((unsigned char*)(key.id).c_str(), (unsigned char*)(key.ver).c_str(), 0, NULL, &fInfo);
		INFO("ACS_LM_Sentinel::isLkfValidBackup() VLSgetFeatureInfo is %ul",lsLicenseStatus);
		if((LS_SUCCESS == lsLicenseStatus) || (LS_INSUFFICIENTUNITS == lsLicenseStatus))
		{
			vlsKeyCount++;

		}

	}
	WARNING("ACS_LM_Sentinel::isLkfValidBackup() vlsKeyCount is %d",vlsKeyCount);
	WARNING("ACS_LM_Sentinel::isLkfValidBackup() manualKeyCount is %d",manualKeyCount);
	if(aNodeInitialized)
		INFO("ACS_LM_Sentinel::isLkfValidBackup() - %s","Node A init ALREADY TRUE");
	if(bNodeInitialized)
		INFO("ACS_LM_Sentinel::isLkfValidBackup() - %s","Node B init ALREADY TRUE");

	if((vlsKeyCount ==  manualKeyCount) && (vlsKeyCount != 0))
	{
		for(std::vector<ACS_LM_Sentinel::Key>::iterator it = lkeys.begin();
				it != lkeys.end(); ++it)
		{
			ACS_LM_Sentinel::Key key = (*it);

			LS_STATUS_CODE lsLicenseStatus = ::VLSgetFeatureInfo((unsigned char*)(key.id).c_str(), (unsigned char*)(key.ver).c_str(), 0, NULL, &fInfo);
			INFO("ACS_LM_Sentinel::isLkfValidBackup() VLSgetFeatureInfo is %ul",lsLicenseStatus);
			if((LS_SUCCESS == lsLicenseStatus) || (LS_INSUFFICIENTUNITS == lsLicenseStatus))
			{
				if(!aNodeInitialized || !bNodeInitialized)
				{
					//should be called during installation of LK file to solve clock tamper problem
					LS_STATUS_CODE rc;
					rc = ::sntlInitStandaloneSystem((char*)GUID, (char*)(key.id).c_str(), (char*)(key.ver).c_str());
					WARNING("ACS_LM_Sentinel::isLkfValidBackup() - #1 Invoked sntlInitStandaloneSystem() - retCode[%lu]",rc);
				}
				else
				{
					INFO("ACS_LM_Sentinel::isLkfValidBackup() - %s","Sentinel already initialized on both nodes - check for CLK_TAMP err");
					LS_STATUS_CODE lsRequestStatus = LS_SUCCESS;
					LS_HANDLE lsHandle = NULL;
					lsRequestStatus = ::VLS_REQUEST((unsigned char*)key.id.c_str(), (unsigned char*)key.ver.c_str(), &lsHandle);
					INFO("ACS_LM_Sentinel::isLkfValidBackup() - VLS_REQUEST(%s,%s), retCode[%lu].",key.id.c_str(),key.ver.c_str(),lsRequestStatus);
					if(lsRequestStatus == VLS_CLK_TAMP_FOUND)
					{
						ERROR("ACS_LM_Sentinel::isLkfValidBackup() - VLS_REQUEST(%s,%s), time tamper error found!.",key.id.c_str(),key.ver.c_str());
						//should be called during installation of LK file to solve clock tamper problem
						LS_STATUS_CODE rc = ::sntlInitStandaloneSystem((char*)GUID, (char*)(key.id).c_str(), (char*)(key.ver).c_str());
						WARNING("ACS_LM_Sentinel::isLkfValidBackup() - #2 Invoked sntlInitStandaloneSystem(%s,%s), retCode[%lu]",key.id.c_str(),key.ver.c_str(),rc);
					}
					::VLS_RELEASE(lsHandle);
				}
			}
		}
		if((thisNodeIP.compare(AP1_NET1_NODEA)== 0) || (thisNodeIP.compare(AP1_NET2_NODEA)== 0))
		{
			aNodeInitialized = true;
		}
		else if((thisNodeIP.compare(AP1_NET1_NODEB)== 0) || (thisNodeIP.compare(AP1_NET2_NODEB)== 0))
		{
			bNodeInitialized = true;
		}
		valid = true;
	}
	else
	{
		customExSize =0;
		cached =0;
		ERROR("ACS_LM_Sentinel::isLkfValidBackup() %s","(vlsKeyCount ==  manualKeyCount) && (vlsKeyCount != 0)) Failed");
	}
	lkeys.clear();
	DEBUG("ACS_LM_Sentinel::isLkfValidBackup() %s","Leaving");
	return valid;

}//end of isLkfValidBackup
/*=================================================================
	ROUTINE: getAllLks
=================================================================== */
bool ACS_LM_Sentinel::getAllLks(std::list<LkData*>& lklist, const int daysToExpire, bool mapList)//Lm_CleanUp
{
	DEBUG("ACS_LM_Sentinel::getAllLks() %s","Entering");
	//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "getAllLks() ");
	LS_STATUS_CODE lsLicenseStatus = LS_SUCCESS;  
	int nFeatureIndex=0;
	int licIndex = 0;
	bool success = true;
	//char parameterSetName[396]={0};
	time_t curTime = time(NULL);
	struct tm curTimeLocal = *localtime(&curTime);
	curTimeLocal.tm_hour = 0;
	curTimeLocal.tm_min = 0;
	curTimeLocal.tm_sec = 0;
	ACE_INT64 curDate = ::mktime(&curTimeLocal);

	while(lsLicenseStatus == LS_SUCCESS)
	{
		VLSlicenseInfo licenseInfo;

		memset(&licenseInfo, 0, sizeof(licenseInfo));
		licenseInfo.structSz = sizeof(licenseInfo);

		lsLicenseStatus = ::VLSgetLicenseInfo(NULL,
				NULL,
				nFeatureIndex++,
				NULL,
				0,
				licIndex,
				&licenseInfo);

		if (lsLicenseStatus == LS_SUCCESS)
		{
			::VLSfeatureInfo fInfo;
			fInfo.structSz = sizeof(fInfo);
			lsLicenseStatus = ::VLSgetFeatureInfo((unsigned char *)licenseInfo.feature_name, (unsigned char *)licenseInfo.version, 0, NULL, &fInfo);
			if ((LS_SUCCESS == lsLicenseStatus ))
			{
				//Lm_clean_Up_Start
				if((isitConnfeature(licenseInfo.feature_name) && mapList) || (isitDisconnfeature(licenseInfo.feature_name) && mapList))
				{
					continue ;
				}
				else
				{//Lm_clean_Up_End
					ACE_INT64 curTime = curDate;
					ACE_INT64 lkExpTime = licenseInfo.death_day;
					ACE_INT64 expTime = daysToExpire*24*60*60;

					if((daysToExpire == 0) ||
							((daysToExpire != 0) && ((curTime+expTime) > lkExpTime)))
					{
						if((daysToExpire != 0) && (lkExpTime == -1))
						{
							continue;
						}
						if((daysToExpire != 0) && (curTime > lkExpTime))
						{
							continue;
						}

						LkData* lkdata = new LkData();
						if(ACE_OS::strcmp(licenseInfo.feature_name,"")!=0)
						{
							if(ACE_OS::strcasecmp(licenseInfo.feature_name,CXCEMERGENCYMODE)!= 0)
							{
								LS_STATUS_CODE lsRequestStatus = LS_SUCCESS;
								LS_HANDLE lsHandle = NULL;

								lsRequestStatus = ::VLS_REQUEST((unsigned char*)licenseInfo.feature_name, (unsigned char*)licenseInfo.version, &lsHandle);
								//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "getAllLks() lsRequestStatus"<<lsRequestStatus);
								if((lsRequestStatus != LS_SUCCESS))
								{
									WARNING("ACS_LM_Sentinel::getAllLks() - VLS_REQUEST(%s,%s) failed with error code: %lu",licenseInfo.feature_name,licenseInfo.version,lsRequestStatus);
								}
								if (LS_SUCCESS == lsRequestStatus)
								{
									lkdata->status = 1;
								}
								else
								{
									lkdata->status = 0;
								}
								::VLS_RELEASE(lsHandle);
								ACS_LM_Common::toUpperCase(licenseInfo.feature_name);
								lkdata->lkId = licenseInfo.feature_name;
							}
							else
							{
								delete lkdata;
								continue;
							}
						}
						DEBUG ("licenseInfo.vendor_info= %s",licenseInfo.vendor_info);
						if(::strcmp(licenseInfo.vendor_info,"")!=0)
						{
							DEBUG ("ACS_LM_Sentinel::getAllLks: strcmp(licenseInfo.vendor_info)","Entering");
							std::string parameterSetName;
							ACS_LM_Common::toUpperCase(licenseInfo.vendor_info);
							parameterSetName = licenseInfo.vendor_info;
							DEBUG ("The value of parameterSetName= %s ",licenseInfo.vendor_info);
							size_t pos = parameterSetName.find_first_of("_");
							if(pos != std::string::npos)
							{
								lkdata->paramName = parameterSetName.substr(0, pos);
								lkdata->setName = parameterSetName.substr(pos+1, parameterSetName.size());
							}
							else
							{
								lkdata->paramName = parameterSetName;
								lkdata->setName = parameterSetName;
							}
						}
						if(ACE_OS::strcmp(licenseInfo.plain_vendor_info,"")!=0)
						{
							INFO ("licenseInfo.plain_vendor_info=%s",licenseInfo.plain_vendor_info);
							lkdata->vendorInfo = licenseInfo.plain_vendor_info;
						}
						else
						{
							lkdata->vendorInfo = "-";
						}
						lkdata->value = licenseInfo.num_licenses;
						lkdata->startDate = ACS_LM_Common::constructDate(licenseInfo.birth_day);
						if(licenseInfo.death_day != -1)
						{
							lkdata->endDate = ACS_LM_Common::constructDate(licenseInfo.death_day);
						}
						else
						{
							lkdata->endDate = ACS_LM_Common::constructDate();
						}
						lklist.push_back(lkdata);
					}
				}
			}
			else
			{
				success = false;
				break;
			}
		}
	}

	DEBUG("ACS_LM_Sentinel::getAllLks() %s","Leaving");
	return success;
}//end of getAllLks
/*=================================================================
	ROUTINE: getValidLks
=================================================================== */
bool ACS_LM_Sentinel::getValidLks(std::list<LkData*>& lklist, bool install,bool isLminst, std::list<LkData*>& discLklist, std::list<LkData*>& connLklist)//Lm_clean_up
{
	DEBUG("ACS_LM_Sentinel::getValidLks() %s","Entering");
	LS_STATUS_CODE lsLicenseStatus = LS_SUCCESS;  
	int nFeatureIndex=0;
	int licIndex = 0;
	bool success = true;

	//Get the apt type using CS API
	string aptType="MSC"; //assign aptType with default value
	if(ACS_CS_API_NetworkElement::getAPTType(aptType) != ACS_CS_API_NS::Result_Success)
	{
		DEBUG("%s","Unable to fetch the apt type");
	}

	do
	{
		VLSlicenseInfo licenseInfo;

		ACE_OS::memset(&licenseInfo, 0, sizeof(licenseInfo));
		licenseInfo.structSz = sizeof(licenseInfo);

		lsLicenseStatus = ::VLSgetLicenseInfo(NULL,
				NULL,
				nFeatureIndex++,
				NULL,
				0,
				licIndex,
				&licenseInfo);
		if((lsLicenseStatus == LS_SUCCESS) || (lsLicenseStatus ==  VLS_NO_MORE_LICENSES))
		{
			LkData* lkdata = new LkData();
			if(ACE_OS::strcmp(licenseInfo.feature_name, "") != 0)
			{
				//LMcleanup start
				bool bConn = isitConnfeature(licenseInfo.feature_name);
				DEBUG ("bConn == %d",bConn);
				DEBUG ("isLminst == %d",isLminst);

				bool bDisconn = false ;
				if(!bConn)
				{
					INFO ("%s","ACS_LM_Sentinel::getValidLks:isitDisconnfeature(licenseInfo.feature_name)");
					bDisconn = isitDisconnfeature(licenseInfo.feature_name);
				}
				if(!bConn && !bDisconn) //LMcleanup
				{
					//TR_HW48901 - for IPSTP APT type
					if(((!install) && (ACE_OS::strcasecmp(licenseInfo.feature_name, CXCTESTMODE)== 0))
							||((!install) && (ACE_OS::strcasecmp(licenseInfo.feature_name, CXCEMERGENCYMODE)== 0))
							||((!install) && (ACE_OS::strcasecmp(licenseInfo.feature_name, CXC_VIRT_MSC_IPSTP_DF)== 0) && ((aptType.compare("MSC")  == 0) ||(aptType.compare("IPSTP")  == 0)))	// vMSC & vIPSTP
							||((!install) && (ACE_OS::strcasecmp(licenseInfo.feature_name, CXC_VIRT_HLR_DF)== 0) && (aptType.compare("HLR") == 0))	// vHLR
							||((!install) && (ACE_OS::strcasecmp(licenseInfo.feature_name, CXC_VIRT_BSC_DF)== 0) && (aptType.compare("BSC") == 0)))	// vBSC
					{
					// skip sending virtual node application special/dummy LKs to CP
						delete lkdata;
						continue;
					}
					else
					{
						LS_STATUS_CODE lsRequestStatus = LS_SUCCESS;
						LS_HANDLE lsHandle = NULL;
						lsRequestStatus = ::VLS_REQUEST((unsigned char*)licenseInfo.feature_name, (unsigned char*)licenseInfo.version, &lsHandle);
						if((lsRequestStatus != LS_SUCCESS))
						{
							WARNING("ACS_LM_Sentinel::getValidLks() Returned code for lsRequestStatus: %d",lsRequestStatus);
						}
						if((lsRequestStatus == LS_SUCCESS))
						{
							lkdata->status = 1;
						}
						else if(LS_INSUFFICIENTUNITS == lsRequestStatus)
						{
							if(licenseInfo.death_day == -1)
							{
								lkdata->status = 1;
							}
							else
							{
								bool lkExpired = isLKExpired(licenseInfo.death_day);
								if(lkExpired)
								{
									lkdata->status = 0;
								}
								else
								{
									lkdata->status = 1;
								}
							}
						}
						else
						{
							lkdata->status = 0;
						}
						::VLS_RELEASE(lsHandle);
						ACS_LM_Common::toUpperCase(licenseInfo.feature_name);
						lkdata->lkId = licenseInfo.feature_name;
					}


				if(ACE_OS::strcmp(licenseInfo.vendor_info,"")!=0)
				{

					std::string parameterSetName;
					ACS_LM_Common::toUpperCase(licenseInfo.vendor_info);
					parameterSetName = licenseInfo.vendor_info;
					size_t pos = parameterSetName.find_first_of("_");
					if(pos != std::string::npos)
					{
						lkdata->paramName = parameterSetName.substr(0, pos);
						lkdata->setName = parameterSetName.substr(pos+1, parameterSetName.size());
					}
					else
					{
						lkdata->paramName = parameterSetName;
						lkdata->setName = parameterSetName;
					}
				}

				if(ACE_OS::strcmp(licenseInfo.plain_vendor_info,"")!=0)
				{
					lkdata->vendorInfo.assign(licenseInfo.plain_vendor_info);
				}
				else
				{
					lkdata->vendorInfo.assign("-");
				}
				lkdata->value = licenseInfo.num_licenses;
				lkdata->startDate = ACS_LM_Common::constructDate(licenseInfo.birth_day);
				if(licenseInfo.death_day != -1)
				{
					lkdata->endDate = ACS_LM_Common::constructDate(licenseInfo.death_day);
				}
				else
				{
					lkdata->endDate = ACS_LM_Common::constructDate();
				}
				lklist.push_back(lkdata);
				DEBUG("Added LK = %s,status = %d",lkdata->lkId.c_str(),lkdata->status);

			}
				//} //lm_clean_up
			else if(isLminst && bDisconn)
			{
				INFO("ACS_LM_Sentinel::getValidLks() it is a disconnection key, featureName is :%s",licenseInfo.feature_name);
				if((::strcmp(licenseInfo.feature_name,"")!=0) && (::strcmp(licenseInfo.vendor_info,"")!=0)
						&& (::strlen(licenseInfo.vendor_info) <= 395 ))
				{
					ACS_LM_Common::toUpperCase(licenseInfo.feature_name);
					ACS_LM_Common::toUpperCase(licenseInfo.vendor_info);

					std::list<std::pair<std::string, std::string> > vendorsInfoList = getAllvendorInfo(licenseInfo.vendor_info);
					std::list<std::pair<std::string, std::string> >::iterator itr;
					for(itr = vendorsInfoList.begin();itr != vendorsInfoList.end(); ++itr)
					{
						LkData* disclkeydata = new LkData();

						disclkeydata->lkId = licenseInfo.feature_name;
						disclkeydata->status = 0;
						disclkeydata->value = 0 ;
						disclkeydata->paramName = (*itr).first;
						disclkeydata->setName = (*itr).second;
						discLklist.push_back(disclkeydata);
						INFO("ACS_LM_Sentinel::getValidLks() Size of  disconnLK:%d",discLklist.size());
						INFO("ACS_LM_Sentinel::getValidLks() Added disconnLK:%s",disclkeydata->lkId.c_str());
						INFO("ACS_LM_Sentinel::getValidLks() Added paramname:%s",disclkeydata->paramName.c_str());
						INFO("ACS_LM_Sentinel::getValidLks() Added setname:%s",disclkeydata->setName.c_str());
					}
					vendorsInfoList.clear();
				}

			}
			else if(isLminst && bConn)
			{
				INFO("ACS_LM_Sentinel::getValidLks() it is a Connection key, featureName is :%s",licenseInfo.feature_name);
				if((::strcmp(licenseInfo.feature_name,"")!=0) && (::strcmp(licenseInfo.vendor_info,"")!=0)
						&&  (::strlen(licenseInfo.vendor_info) <= 395 ))
				{
					ACS_LM_Common::toUpperCase(licenseInfo.feature_name);
					ACS_LM_Common::toUpperCase(licenseInfo.vendor_info);

					std::list<std::pair<std::string, std::string> > vendorsInfoList = getAllvendorInfo(licenseInfo.vendor_info);
					std::list<std::pair<std::string, std::string> >::iterator itr;
					for(itr = vendorsInfoList.begin();itr != vendorsInfoList.end(); ++itr)
					{
						LkData* connlkeydata = new LkData();

						connlkeydata->lkId = licenseInfo.feature_name;
						connlkeydata->status = 0;
						connlkeydata->value = 0;
						connlkeydata->paramName = (*itr).first;
						connlkeydata->setName = (*itr).second;
						connLklist.push_back(connlkeydata);
						INFO("ACS_LM_Sentinel::getValidLks() Size of  connLklist:%d",connLklist.size());
						INFO("ACS_LM_Sentinel::getValidLks() Added connLKList:%s",connlkeydata->lkId.c_str());
						INFO("ACS_LM_Sentinel::getValidLks() Added paramname:%s",connlkeydata->paramName.c_str());
						INFO("ACS_LM_Sentinel::getValidLks() Added setname:%s",connlkeydata->setName.c_str());
					}
					vendorsInfoList.clear();
				}

			}
			else
				{

				}
		}

	}//LMcleanup end
	else
	{
		if((lsLicenseStatus !=  VLS_NO_MORE_FEATURES))
		{
			success = false;
			INFO("ACS_LM_Sentinel::getValidLks() %s","VLS_NO_MORE_FEATURES");
		}
	}
}while((lsLicenseStatus == LS_SUCCESS));

DEBUG("ACS_LM_Sentinel::getValidLks() %s","Leaving");
return success;
}//end of getValidLks
//LMcleanup start
/*=================================================================
	ROUTINE: getAllvendorInfo
=================================================================== */
std::list<pair<std::string, std::string> > ACS_LM_Sentinel::getAllvendorInfo(char* vendorInfo)
{
	DEBUG("ACS_LM_Sentinel::getAllvendorInfo() %s","Entering");
        std::pair<std::string, std::string> paramSetnames ;
        std::list<std::pair<std::string, std::string> > vendorsInfoList ;
        char* pch ;
        pch = strtok(vendorInfo,",");

        while (pch != NULL)
        {
            //printf ("%s\n",pch);
                INFO("ACS_LM_Sentinel::getAllvendorInfo().Vendor value is : %s",pch);
                std::string info = pch ;
                size_t pos = info.find_first_of("_");
                if(pos!=std::string::npos)
                {
                        paramSetnames.first = info.substr(0,pos);
                        paramSetnames.second = info.substr(pos+1,info.size());
                }
                else
                {
                        paramSetnames.first = info;
                        paramSetnames.second = info;
                }

                vendorsInfoList.push_back(paramSetnames);
                pch = strtok(NULL, ",");
        }
        DEBUG("ACS_LM_Sentinel::getAllvendorInfo() %s","Leaving");
 return vendorsInfoList ;
}//end of getAllvendorInfo
/*=================================================================
	ROUTINE: isitConnfeature
=================================================================== */
bool ACS_LM_Sentinel::isitConnfeature(const char* feature)
{
	DEBUG("ACS_LM_Sentinel::isitConnfeature() %s","Entering");
        std::string featureName = feature ;

        if( featureName.find("FAT1022002") == 0|| featureName.find("FAT1022004") == 0 ||
                featureName.find("FAT1022006") == 0 || featureName.find("FAT1022008") == 0 )
        {
        	DEBUG("ACS_LM_Sentinel::isitConnfeature().Returning true: %s","Leaving");
        	return true ;
        }
        else
        {
        	DEBUG("ACS_LM_Sentinel::isitConnfeature().Returning false:%s","Leaving");
                return false ;
        }
}//end of isitConnfeature
/*=================================================================
	ROUTINE: isitDisconnfeature
=================================================================== */
bool ACS_LM_Sentinel::isitDisconnfeature(const char* feature)
{
	DEBUG("ACS_LM_Sentinel::isitDisconnfeature() %s","Entering");
        std::string featureName = feature ;

        if( featureName.find("FAT1022001") == 0|| featureName.find("FAT1022003") == 0 ||
                featureName.find("FAT1022005") == 0 || featureName.find("FAT1022007") == 0 )
        {
        	DEBUG("ACS_LM_Sentinel::isitDisconnfeature().Returning true %s","Leaving");
            return true ;

        }
        else
        {
        	DEBUG("ACS_LM_Sentinel::isitDisconnfeature().Returning false:%s","Leaving");
            return false ;

        }

}//end of isitDisconnfeature
//LMcleanup end


/*=================================================================
	ROUTINE: installLkf
=================================================================== */
ACS_LM_AppExitCode ACS_LM_Sentinel::installLkf(const std::string& fromLkfPath, bool& aNodeInitialized, bool& bNodeInitialized )
{
	DEBUG("ACS_LM_Sentinel::installLkf() %s","Entering");
	//bool installed = false;
//	bool installed;
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_NOK;
	std::string tmpLkfPath = theCurrentPath + ".tmp";
	if(!ACS_LM_Common::deleteFile(tmpLkfPath))
	{
		DEBUG("ACS_LM_Sentinel::installLkf() Deletion of tmplkffile - %s failed", tmpLkfPath.c_str());
	}


	if(!ACS_LM_Common::backupFile(theCurrentPath,tmpLkfPath))
	{
		DEBUG("ACS_LM_Sentinel::installLkf() Backup of source - %s  target - %s failed", theCurrentPath.c_str(),tmpLkfPath.c_str());

	}

        if(ACS_LM_Common::isFileExists(theCurrentPath))
        {
              if(ACS_LM_Common::deleteFile(theCurrentPath))
              {
		    DEBUG("ACS_LM_Sentinel::installLkf() %s","LKF file removed from the theCurrentPath");
              }
              else
              {
		    ERROR("ACS_LM_Sentinel::installLkf() %s","ERROR!!! LKF file not removed from the theCurrentPath");
              }
        }
        //TR : HP28932,HP38205,HP38199
        if(ACS_LM_Common::isFileExists(backupPath1))
        {
        	if(ACS_LM_Common::deleteFile(backupPath1))
        	{
        		DEBUG("ACS_LM_Sentinel::installLkf() %s","LKF file removed from the BackupPath");
        	}
        	else
        	{
        		ERROR("ACS_LM_Sentinel::installLkf() %s","ERROR!!! LKF file not removed from the BackupPath");
        	}
        }
        //End of HP28932,HP38205,HP38199
	if(ACS_LM_Common::backupFile(fromLkfPath.c_str(), theCurrentPath.c_str())) //CopyFile
	{
		clean();
		if(init(aNodeInitialized, bNodeInitialized, true))
		{
			//bool installed;
			exitCode = ACS_LM_RC_OK;
			//installed = true;							
		}
		else
		{
			ERROR("ACS_LM_Sentinel::installLkf() %s","init() failed");
			exitCode = ACS_LM_RC_INVALIDLKF;
		}
	}
	else
	{
		ERROR("ACS_LM_Sentinel::installLkf() Backup of source - %s  target - %s failed", fromLkfPath.c_str(),theCurrentPath.c_str());
		exitCode = ACS_LM_RC_PHYFILEERROR;
	}
	DEBUG("ACS_LM_Sentinel::installLkf() %s","Leaving");
	return exitCode;

}//end of installLkf
/*=================================================================
	ROUTINE: finishInstallation
=================================================================== */
void ACS_LM_Sentinel::finishInstallation(bool success, bool& aNodeInitialized, bool& bNodeInitialized,ACE_UINT32 checkSum)
{

	DEBUG("ACS_LM_Sentinel::finishInstallation() %s","Entering");
	std::string tmpLkfPath = theCurrentPath + ".tmp";
	//std::string tmpLkfBackupPath = backupPath1 + ".tmp";
	if(success)
	{
		if(!ACS_LM_Common::isFileExists(tmpLkfPath))
		{
			ERROR("ACS_LM_Sentinel::finishInstallation() tmpLkfPath %s  does not exist ",tmpLkfPath.c_str());
			return;
		}
		if(!ACS_LM_Common::deleteFile(tmpLkfPath))
		{
			ERROR("ACS_LM_Sentinel::finishInstallation() deletion of tmpLkfPath %s  failed ",tmpLkfPath.c_str());
		}
	}
	else
	{
		if(!ACS_LM_Common::deleteFile(theCurrentPath))
		{
			ERROR("ACS_LM_Sentinel::finishInstallation() deletion of file in theCurrentPath %s  failed ",theCurrentPath.c_str());
		}

		if(ACE_OS::rename(tmpLkfPath.c_str(), theCurrentPath.c_str()))
		{
			INFO("ACS_LM_Sentinel::finishInstallation() rename of files source - %s  target - %s passed ",tmpLkfPath.c_str(),theCurrentPath.c_str());
		}
		 //TR : HP28932,HP38205,HP38199
		if(ACS_LM_Common::backupFile(theCurrentPath, backupPath1))
		{
			INFO("ACS_LM_Sentinel::finishInstallation() backup of files source - %s  target - %s passed ",theCurrentPath.c_str(),backupPath1.c_str());
		}
		else
		{
			ERROR("ACS_LM_Sentinel::finishInstallation() backup of files source - %s  target - %s failed ",theCurrentPath.c_str(),backupPath1.c_str());
		}//End of TR : HP28932,HP38205,HP38199

		clean();
		 //CNI30_4_1719(TR NO HN66027/HN40528)
		bool validlkf = init(aNodeInitialized, bNodeInitialized, false,checkSum);
		if(!validlkf)
		{
			initFromBackup(aNodeInitialized, bNodeInitialized, false,checkSum);
			INFO("ACS_LM_Sentinel::finishInstallation() initFromBackup() %s ","Validation is OK !!");
		}
		else
		{
			INFO("ACS_LM_Sentinel::finishInstallation() init() %s ","Validation is OK !!");
		}

	}
	DEBUG("ACS_LM_Sentinel::finishInstallation() %s","Leaving");
}//end of finishInstallation
/*=================================================================
	ROUTINE: isTestModeAllowed
=================================================================== */

bool ACS_LM_Sentinel::isTestModeAllowed()
{

	DEBUG("ACS_LM_Sentinel::isTestModeAllowed() %s","Entering");

	bool testModeAllowed = false;
	LS_HANDLE lsHandle = NULL;

	LS_STATUS_CODE lsStatusCode = ::VLS_REQUEST((unsigned char*)CXCTESTMODE, (unsigned char*)"", &lsHandle);
	INFO("ACS_LM_Sentinel::isTestModeAllowed() Returned code for lsStatusCode: %d",lsStatusCode);
	if(LS_SUCCESS == lsStatusCode)
	{
		DEBUG("ACS_LM_Sentinel::isTestModeAllowed() %s","VLS_REQUEST returned Passed");
		testModeAllowed = true;
	}
	else if(LS_INSUFFICIENTUNITS == lsStatusCode)
	{
		VLSlicenseInfo licenseInfo;
		ERROR("ACS_LM_Sentinel::isTestModeAllowed() %s","VLS_REQUEST returned LS_INSUFFICIENTUNITS");
		// Fill the size parameter of the featureInfo struct.Else, the query
		//may not obtain complete data. 
		licenseInfo.structSz = sizeof(licenseInfo);

		LS_STATUS_CODE rc = VLSgetLicenseInfo((unsigned char *)CXCTESTMODE,
				(unsigned char *)"",
				-1,
				NULL,
				0,
				0,
				&licenseInfo);

//		 //  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isTestModeAllowed() VLSgetLicenseInfo"<<rc);
		if(rc == LS_SUCCESS)
		{
			INFO("ACS_LM_Sentinel::isTestModeAllowed() %s","VLSgetLicenseInfo returned Success");
			if(licenseInfo.death_day == -1)
			{
				INFO("ACS_LM_Sentinel::isTestModeAllowed() %s","licenseInfo.death_day is equal to -1");
				testModeAllowed = true;
			}
			else
			{
				DEBUG("ACS_LM_Sentinel::isTestModeAllowed() %s","licenseInfo.death_day is not equal to -1");
				bool lkExpired = isLKExpired(licenseInfo.death_day);
				if(lkExpired)
				{
					ERROR("ACS_LM_Sentinel::isTestModeAllowed() %s","testmode not allowed");
					testModeAllowed = false;
				}
				else
				{
					INFO("ACS_LM_Sentinel::isTestModeAllowed() %s","testmode allowed");
					testModeAllowed = true;
				}
			}
		}
	}
	VLS_RELEASE(lsHandle);
	DEBUG("ACS_LM_Sentinel::isTestModeAllowed() %s","Leaving");
	return testModeAllowed;


}//end of isTestModeAllowed
/*=================================================================
	ROUTINE: isEmergencyKeyValid
=================================================================== */
bool ACS_LM_Sentinel::isEmergencyKeyValid()
{

	DEBUG("ACS_LM_Sentinel::isEmergencyKeyValid() %s","Entering");
	bool emergencyKeyValid = false;
	LS_HANDLE lsHandle = NULL;
	LS_STATUS_CODE lsStatusCode = ::VLS_REQUEST((unsigned char*)CXCEMERGENCYMODE,(unsigned char*)"", &lsHandle);
	INFO("ACS_LM_Sentinel::isEmergencyKeyValid() Returned code for lsStatusCode: %d",lsStatusCode);
	if(LS_SUCCESS == lsStatusCode)
	{
		emergencyKeyValid = true;
		INFO("ACS_LM_Sentinel::isEmergencyKeyValid() %s","EmergencyKey is valid");
	}
	else
	{
		ERROR("ACS_LM_Sentinel::isEmergencyKeyValid() EmergencyKey is not valid %d ",lsStatusCode);
	}
	::VLS_RELEASE(lsHandle);
	DEBUG("ACS_LM_Sentinel::isEmergencyKeyValid() %s","Leaving");
	return emergencyKeyValid;
}//end of isEmergencyKeyValid
/*=================================================================
	ROUTINE: isEmergencyModeAllowed
=================================================================== */

bool ACS_LM_Sentinel::isEmergencyModeAllowed()
{
	DEBUG("ACS_LM_Sentinel::isEmergencyModeAllowed() %s","Entering");
	bool emergencyModeValid = false;
	if(isEmergencyKeyValid()) 
	{
		CONSUME_OPERATION_TYPE optype;
		CONSUME_LIMIT_TYPE type;
		VLSlicenseInfo licenseInfo;
		LS_HANDLE handle = NULL;
		long currval, val;
		char policy_data[VLS_VENINFOLEN + 1 ]={0};
	//	int volume_limit = 0;
	//	int warning_amount = 0;
		//int grace_amount = 0;
		// Fill the size parameter of the featureInfo struct.Else, the query
		//may not obtain complete data. 
		licenseInfo.structSz = sizeof(licenseInfo);
		LS_STATUS_CODE rc = VLSgetLicenseInfo((unsigned char *)CXCEMERGENCYMODE,
				(unsigned char *)"",
				-1,
				NULL,
				0,
				0,
				&licenseInfo);
		//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isEmergencyModeAllowed() VLSgetLicenseInfo"<<rc);
		// If there are more than 1 license for a particular feature/version,
		//iterate through all the licenses using this API and determine the correct
		//license using the identifier specified in vendor-info while generating
		//the license 
		if(rc == LS_SUCCESS)
		{
			int volume_limit = 0;
			int warning_amount = 0;
			int grace_amount = 0;
			// Obtains the policy information. This should be interpreted
			//by your application
			::strncpy(policy_data, licenseInfo.vendor_info, VLS_VENINFOLEN);
			char* tok = ::strtok( policy_data, ";" );
			std::string vol_info = licenseInfo.vendor_info;
			if(tok != NULL)
			{
				vol_info = tok;	
			}

			// Obtains the individual limits 
			size_t pos1 = vol_info.find_first_of(":");
			std::string temp = vol_info.substr(0,pos1);
			volume_limit = atoi(temp.c_str());

			//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isEmergencyModeAllowed() volume_limit: "<<volume_limit);
			INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() volume_limit - %d",volume_limit);
			temp = "";

			vol_info = vol_info.substr(pos1+1,(int)vol_info.size());
			pos1 = vol_info.find_first_of(":");
			// Obtains the alert level. Determines prior
			//to how many transactions the warnings are generated
			temp = vol_info.substr(0,pos1);
			warning_amount = atoi(temp.c_str());

			//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isEmergencyModeAllowed() warning_amount: "<<warning_amount);
			INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() warning_amount - %d",warning_amount);

			vol_info = vol_info.substr(pos1+1,(int)vol_info.size());
			grace_amount = atoi(vol_info.c_str());

			//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isEmergencyModeAllowed() grace_amount: "<<grace_amount);

			LS_STATUS_CODE lsStatusCode = VLS_REQUEST( (unsigned char *)CXCEMERGENCYMODE, (unsigned char *)"", &handle);
			if((lsStatusCode != LS_SUCCESS))
			{
				WARNING("ACS_LM_Sentinel::isEmergencyModeAllowed() Returned code for lsStatusCode: %d",lsStatusCode);
			}
			// Obtains a license 
			if(lsStatusCode == LS_SUCCESS)
			{
				INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","VLS_REQUEST Passed");
				currval=0;
				type=VLS_LIMIT_TYPE_VOLUME;
				rc = VLSgetConsumeLimit(handle,type,&currval,NULL);
				switch(rc)
				{
					case LS_SUCCESS:
					{
						INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","VLSgetConsumeLimit Passed");
						break;
					}
					case VLS_NO_RECORDS_FOUND:
					{
						//No limit set in the database for this feature & version
						INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","VLSgetConsumeLimit() returned VLS_NO_RECORDS_FOUND ");
						break;
					}
					case VLS_OPERATION_NOT_SUCCESSFUL:
					{
						ERROR("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","VLSgetConsumeLimit() returned VLS_OPERATION_NOT_SUCCESSFUL ");
						break;
					}
				}
				INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() volume_limit %d ",volume_limit);
				INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() currval %d ",currval);
				INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() warning_amount %d ",warning_amount);
				if( (volume_limit > currval) && ((volume_limit - currval) <= warning_amount))
				{
					//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isEmergencyModeAllowed() Reached warning limit");
					INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","Reached warning limit ");

				}
				// Check if the current limit has exceeded then allow the feature to
				//run till the grace limit 
				if(currval != (volume_limit + grace_amount))
				{
					// Renew license key (license heartbeat)
					VLS_UPDATE(handle);
					val = volume_limit;
					type = VLS_LIMIT_TYPE_VOLUME;
					optype = VLS_SET;
					if(val > ((volume_limit + grace_amount)- currval))
					{
						val = (volume_limit + grace_amount)- currval;
					}
					if(val != 0)
					{		
						//Resetting the volume limit
						rc = VLSsetConsumeLimit(handle,type,optype,&val,NULL);
						//  ACS_LM_PrintTrace(ACS_LM_Sentinel, "isEmergencyModeAllowed() VLSsetConsumeLimit() rc:"<<rc);
						switch(rc)
						{
							case LS_SUCCESS:
							{
								//Limit set Successfully
								emergencyModeValid = true;
								INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","VLSgetConsumeLimit() second time returned Passed ");
								break;
							}
							case VLS_NEW_RECORD_FOUND:
							{

								INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","VLSgetConsumeLimit() second time returned VLS_NEW_RECORD_FOUND ");
								while( rc == VLS_NEW_RECORD_FOUND )
								{
									type=VLS_LIMIT_TYPE_VOLUME;
									optype=VLS_SET;
									rc = VLSsetConsumeLimit(handle,type,optype,&val,NULL);
									if( rc == VLS_NEW_RECORD_FOUND)
									{
										//A newer value has been found in database since the last
										//read operation was performed. The new value is returned in the
										//"val" parameter. The application should check if the current limit
										//does not exceed the policy limits and again try to increment the
										//database with the required value
										//Check if the current limit does not exceed the policy limits
										if(val < volume_limit)
										{
											emergencyModeValid = true;
										}
									}
								}
								break;
							}
							case VLS_OPERATION_NOT_SUCCESSFUL:
							{
								INFO("ACS_LM_Sentinel::isEmergencyModeAllowed() %s ","VLSgetConsumeLimit() second time returned VLS_OPERATION_NOT_SUCCESSFUL ");
								break;
							}
						}
					}
				}
			}
		}			
		::VLS_RELEASE(handle);	
	}
	DEBUG("ACS_LM_Sentinel::isEmergencyModeAllowed() %s","Leaving");
	return emergencyModeValid;

}//end of isEmergencyModeAllowed

/*=================================================================
	ROUTINE: getKeysFromLservrc
=================================================================== */
void ACS_LM_Sentinel::getKeysFromLservrc(const std::string& lservrcPath, std::vector<ACS_LM_Sentinel::Key>& lkeys)
{
	DEBUG("ACS_LM_Sentinel::getKeysFromLservrc() %s","Entering");
	std::ifstream lservrcFile(lservrcPath.c_str(), std::ios::in);
	if(lservrcFile.is_open())
	{
		std::string lineread;
		while(std::getline(lservrcFile, lineread))
		{
			ACS_LM_Sentinel::Key lkey;
			std::vector<std::string> tokens;
			tokenize(lineread, tokens);
			if(tokens.size() == 0)
			{
				continue;
			}
			else if((tokens.at(0)).at(0) == '#')
			{
				continue;
			}
			else
			{
				if(tokens.size() < 3)
				{
					lkey.id = "Invalid";
					lkey.ver = "";
					ERROR("ACS_LM_Sentinel::getKeysFromLservrc() %s","Invalid key");
				}
				else
				{
					lkey.id = tokens.at(1);
					if(ACE_OS::strcasecmp(tokens.at(2).c_str(),"Ni")== 0)
					{
						lkey.ver = "";
					}
					else
					{
						lkey.ver = tokens.at(2);
					}
				}
				DEBUG("ACS_LM_Sentinel::getKeysFromLservrc() %s","Pushing the License key");
				lkeys.push_back(lkey);
			}
		}
		lservrcFile.close();
	}
	DEBUG("ACS_LM_Sentinel::getKeysFromLservrc() %s","Leaving");

} //end of getKeysFromLservrc
/*=================================================================
	ROUTINE: tokenize
=================================================================== */
void ACS_LM_Sentinel::tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
	DEBUG("ACS_LM_Sentinel::tokenize() %s","Entering");
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
	int count = 0;
	while ((std::string::npos != pos || std::string::npos != lastPos)&& count <= 2)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
		count++;
	}
	DEBUG("ACS_LM_Sentinel::tokenize() %s","Leaving");
}//end of tokenize
/*=================================================================
	ROUTINE: isLKExpired
=================================================================== */
bool ACS_LM_Sentinel::isLKExpired(ACE_INT64 lkExpTime)
{
	DEBUG("ACS_LM_Sentinel::isLKExpired() %s","Entering");
	bool expired = false;
	time_t currentTime = time(NULL);
	struct tm curTimeLocal = *localtime(&currentTime);
	curTimeLocal.tm_hour = 0;
	curTimeLocal.tm_min = 0;
	curTimeLocal.tm_sec = 0;
	ACE_INT64 curDate = mktime(&curTimeLocal);
	ACE_INT64 curTime = curDate;

	if(curTime > lkExpTime)
	{
		INFO("ACS_LM_Sentinel::isLKExpired() %s","Expired is true");
		expired = true;
	}		

	return expired;
	DEBUG("ACS_LM_Sentinel::isLKExpired() %s","Leaving");

}//end of isLKExpired

/* Read the Hashed Fingerprint from LKF */
std::string ACS_LM_Sentinel::getHashedFingerprintFromLKF(const std::string lkfPath)
{
	DEBUG("ACS_LM_Sentinel::getHashedFingerprintFromLKF() - %s","Entering");
	string lkfFPToCompare("");
	std::ifstream lservrcFile(lkfPath.c_str(), std::ios::in);
	if(lservrcFile.is_open())
	{
		std::string lineread;
		while(std::getline(lservrcFile, lineread))
		{
			size_t posStartOfFP = lineread.find("CL_ND_LCK");
			if(posStartOfFP == string::npos)
				continue;

			posStartOfFP += 10;
			size_t posEndOfFP = lineread.find(" ",posStartOfFP);
			size_t lenFP = posEndOfFP - posStartOfFP;
			string fpInLKF = lineread.substr(posStartOfFP,lenFP);
			DEBUG("ACS_LM_Sentinel::getHashedFingerprintFromLKF() - FP from LKF file is [%s]",fpInLKF.c_str());
			//example: fpInLKF == *19Q2TB75KT5VRNX400 - first 16 characters are actual FP and last part is Lock code - fixed to 400
			if(lenFP > 16)		// FP length without lock code
			{
				string fpWithoutLockCode = fpInLKF.substr(0,16);
				string lockCodeSubstr = fpInLKF.substr(16,fpInLKF.length()-16);
				// transform to License MO format. example: 400-*19Q 2TB7 5KT5 VRNX
				lkfFPToCompare = lockCodeSubstr + "-" + ACS_LM_Sentinel::formatFingerPrint(fpWithoutLockCode.c_str());
				lkfFPToCompare = lkfFPToCompare.substr(0, lkfFPToCompare.length() - 1);
				DEBUG("ACS_LM_Sentinel::getHashedFingerprintFromLKF() Formatted FP from LKF file is [%s]",lkfFPToCompare.c_str());
				break;		// successfully fetched the hashed fingerprint from license key
			}
			else	// Something wrong with FP length, try next license key in file - will not happen with proper LKF
				continue;
		}
		lservrcFile.close();
	}
	DEBUG("ACS_LM_Sentinel::getHashedFingerprintFromLKF() - Leaving - FP from LKF is [%s]",lkfFPToCompare.c_str());
	return lkfFPToCompare;
}
