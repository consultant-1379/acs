//******************************************************************************
//
//  NAME
//     acs_lm_cmdclient.cpp
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
#include <ace/ACE.h>
#include "acs_lm_server.h"
#include "acs_lm_dsdserver.h"
#include "acs_lm_cmd.h"
#include "acs_lm_testlkf.h"
#include "acs_lm_lksender.h"
#include "acs_lm_tra.h"
#include "acs_lm_electronickey_runtimeowner.h"
#include "acs_lm_haservice.h"
#include <acs_prc_api.h>
#include <acs_lm_brfc_interface.h>
using namespace std;
ACE_HANDLE stopThreadFds[2];
bool fpChangeClusterIP;
bool fpChangeNodeName;
bool ACS_LM_Server::isShutdown=false;
bool ACS_LM_Server::isOldDirectoryDeleted=false;
ACS_LM_DSDServer* theDSDServerPtr = NULL;
//The below Variables used to fix the Grace Issue occured during the upgrade from 3.0.5 to 3.2.0
ACE_UINT32 ACS_LM_Server::pp_LKF_Checksum=0;
ACE_UINT16 ACS_LM_Server::pp_persistChecksum=0;
ACE_UINT16 ACS_LM_Server::pp_calcChecksum=0;
ACE_UINT16 ACS_LM_Server::sp_persistChecksum=0;
ACE_UINT16 ACS_LM_Server::sp_calcChecksum=0;


/*=================================================================
	ROUTINE: ACS_LM_Server constructor
=================================================================== */
ACS_LM_Server::ACS_LM_Server()
:theLMRootObjectImplementer(0),
 theLMManagedObjectImplementer(0),
 theLMNorthboundObjectImplementer(0),
 thePersistFileValid(true),
 theValidLkfFound(true),
 isShutdownSignalled(false),
 persistFile(NULL),
 sentinelFile(NULL),
 eventHandler(NULL),
 cpMonitor(NULL),
 scheduler(NULL),
 clientHandler(NULL),
 theLMRuntimeOwnerPtr(0),
 theLMRuntimeHandlerThreadPtr(0),
 theElectronicKeyLMRuntimeOwnerPtr(0),
 theElectronicKeyLMRuntimeHandlerThreadPtr(0),
 theOmHandlerPtr(0),
 theLMBrfcRuntimeOwnerPtr(0),
 theLMBrfcRuntimeHandlerThreadPtr(0)
{
	DEBUG("ACS_LM_Server::ACS_LM_Server() %s","Entering");
	//serviceStopEvent = new ACE_Event();
	ACE_OS::pipe(stopThreadFds);
	theEndEvent = new ACE_Event(true,false,USYNC_THREAD,"EV_ACS_LM_TERMINATE_EVENT");
	DEBUG("ACS_LM_Server::ACS_LM_Server() %s","Leaving");


}//end of constructor

/*=================================================================
	ROUTINE: ACS_LM_Server destructor
=================================================================== */
ACS_LM_Server::~ACS_LM_Server()
{
	DEBUG("ACS_LM_Server::~ACS_LM_Server() %s","Entering");
	delete theEndEvent;
	theEndEvent = NULL;
	DEBUG("ACS_LM_Server::~ACS_LM_Server() %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: stop
=================================================================== */
/*void ACS_LM_Server::stop()
{
	DEBUG("ACS_LM_Server::stop() %s","Entering");
	theEndEvent->pulse();
	DEBUG("ACS_LM_Server::stop() %s","Leaving");
}//end of stop*/

/*=================================================================
        ROUTINE: StopInteractiveMode
=================================================================== */
void ACS_LM_Server::stopService()
{
	DEBUG("%s","Entering ACS_LM_Server::StopService LM Service...");
	ACS_LM_Server::isShutdown=true;
	INFO("Entering ACS_LM_Server::StopService LM Service after isShutdown is : %d", ACS_LM_Server::isShutdown);
	ACS_LM_LkSender::cancel();
	DEBUG("%s", "Signal the other threads to stop.");

	char buf[] = { 1 , 1};
	int bytes = 0;

	bytes = ACE_OS::write(stopThreadFds[1], buf, sizeof(buf ));
	if( bytes <= 0 )
	{
		ERROR("%s","Error occurred while signaling stop event ");
		ERROR("%s","Leaving ACS_LM_Server::StopService...");
		ACS_LM_Server::isShutdown=false;	
		return;
	}

	//theLMRootObjectImplementer->shutdown();

	if(theDSDServerPtr != NULL)
	{

		//		DEBUG("%s","ACS_LM_Server::stopService() Before theDSDServerPtr->stop() ");
		theDSDServerPtr->stop();
		//		DEBUG("%s","ACS_LM_Server::stopService() After theDSDServerPtr->stop() ");
		theDSDServerPtr->wait();
	}
	if(scheduler != 0)
	{
		scheduler->stop();
	}
	deleteParticipant();
	finaliseOMHandler();
	if ( theLMRuntimeHandlerThreadPtr != 0 && theLMRuntimeOwnerPtr!=0)
	{
		theLMRuntimeHandlerThreadPtr->stop();
		theLMRuntimeOwnerPtr->finalizeLMRunTimeOwner();
		delete theLMRuntimeHandlerThreadPtr;
		theLMRuntimeHandlerThreadPtr = 0;
		delete theLMRuntimeOwnerPtr;
		theLMRuntimeOwnerPtr = 0;
	}
	if(theElectronicKeyLMRuntimeHandlerThreadPtr != 0 && theElectronicKeyLMRuntimeOwnerPtr!=0 )
	{
		theElectronicKeyLMRuntimeHandlerThreadPtr->stop();
		theElectronicKeyLMRuntimeOwnerPtr->finalizeElectronicKeyLMRunTimeOwner();
		delete theElectronicKeyLMRuntimeHandlerThreadPtr;
		theElectronicKeyLMRuntimeHandlerThreadPtr = 0;
		delete theElectronicKeyLMRuntimeOwnerPtr;
		theElectronicKeyLMRuntimeOwnerPtr = 0;

	}

	if (theLMRootObjectImplementer != 0)
	{
		delete theLMRootObjectImplementer;
		theLMRootObjectImplementer = 0;
	}




	if (theLMManagedObjectImplementer != 0)
	{
		delete theLMManagedObjectImplementer;
		theLMManagedObjectImplementer = 0;
	}

	if (theLMNorthboundObjectImplementer != 0)
	{
		delete theLMNorthboundObjectImplementer;
		theLMNorthboundObjectImplementer = 0;
	}

	if(cpMonitor != 0)
	{
		cpMonitor->stop();
		cpMonitor->wait();
		delete cpMonitor;
		cpMonitor = 0;
	}
	delete scheduler;
	scheduler = 0;

	delete clientHandler;
	clientHandler = 0;
	delete sentinelFile;
	sentinelFile = 0;
	delete eventHandler;
	eventHandler = 0;
	delete persistFile;
	persistFile = 0;
	delete theOmHandlerPtr;
	theOmHandlerPtr = 0;

	if(theDSDServerPtr != NULL)
	{
		delete theDSDServerPtr;
		theDSDServerPtr = NULL;
	}

	close(stopThreadFds[0]);
	close(stopThreadFds[1]);
	ACS_LM_Server::isShutdown=false;

	DEBUG("%s","Leaving ACS_LM_Server::StopService LM Service...");
}

/*=================================================================
	ROUTINE: start
=================================================================== */
/*void ACS_LM_Server::start()
{
	DEBUG("ACS_LM_Server::start() %s","Entering");
	activate();
	DEBUG("ACS_LM_Server::start() %s","Leaving");
}//end of start*/
/*=================================================================
	ROUTINE: svc
=================================================================== */
int ACS_LM_Server::svc()
{
	DEBUG("ACS_LM_Server::svc() %s","Entering");
	run();
	DEBUG("ACS_LM_Server::svc() %s","Leaving");
	return 0;
}//end of start
/*=================================================================
	ROUTINE: InitializeRunTimeOwner
=================================================================== */
bool ACS_LM_Server::InitializeRunTimeOwner()
{
	DEBUG("ACS_LM_Server::InitializeRunTimeOwner() %s","Entering");
	theLMRuntimeOwnerPtr = new ACS_LM_RuntimeOwner();
	theLMRuntimeHandlerThreadPtr = new ACS_LM_RuntimeHandlerThread();
	if (theLMRuntimeOwnerPtr->initLMRunTimeOwner())
	{
		theLMRuntimeHandlerThreadPtr->setImpl(theLMRuntimeOwnerPtr);
		theLMRuntimeHandlerThreadPtr->activate();
		return true;
	}
	else
	{
		ERROR("ACS_LM_Server::InitializeRunTimeOwner() %s","RuntimeOwner for LicInitParam class initialization failed");
		return false;
	}
	DEBUG("ACS_LM_Server::InitializeRunTimeOwner() %s","Leaving");
}//end of InitializeRunTimeOwner

/*=================================================================
	ROUTINE: InitializeElectronicKeyRunTimeOwner
=================================================================== */
bool ACS_LM_Server::InitializeElectronicKeyRunTimeOwner()
{
	INFO("ACS_LM_Server::InitializeElectronicKeyRunTimeOwner()  %s", "Entering" );

	while(!getShudownSignalFlag())
	{
		if (theElectronicKeyLMRuntimeOwnerPtr->initElectronicKeyLMRunTimeOwner())
		{
			theElectronicKeyLMRuntimeHandlerThreadPtr->setImpl(theElectronicKeyLMRuntimeOwnerPtr);
			theElectronicKeyLMRuntimeHandlerThreadPtr->activate();
			break;
		}
		else
		{
			ERROR("ACS_LM_Server::InitializeElectronicKeyRunTimeOwner() %s", "Failed... Retrying");
			usleep(500000);
		}
	}

	if(getShudownSignalFlag())
	{
		ERROR("ACS_LM_Server::InitializeElectronicKeyRunTimeOwner() %s", "Failed after retry also - shutdown is signalled");
		return false;;
	}

	INFO("ACS_LM_Server::InitializeElectronicKeyRunTimeOwner()  %s", "Leaving" );
	return true;
}//end of InitializeElectronicKeyRunTimeOwner
/*=================================================================
	ROUTINE: stop
=================================================================== */
void ACS_LM_Server::createElectronicKeyRunTimeOwner()
{
	INFO("ACS_LM_Server::createElectronicKeyRunTimeOwner()  %s", "Entering" );
	theElectronicKeyLMRuntimeOwnerPtr = new ACS_LM_ElectronicKey_RuntimeOwner();
	theElectronicKeyLMRuntimeHandlerThreadPtr = new ACS_LM_ElectronicKey_RuntimeHandlerThread();
	INFO("ACS_LM_Server::createElectronicKeyRunTimeOwner()  %s", "Leaving" );
}//end of InitializeElectronicKeyRunTimeOwner
/*=================================================================
	ROUTINE: createAndInitialiseOMHandler
=================================================================== */
bool ACS_LM_Server::createAndInitialiseOMHandler()
{
	INFO("ACS_LM_Server::createAndInitialiseOMHandler()  %s", "Entering" );
	theOmHandlerPtr = new OmHandler();
	if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		ERROR("ACS_LM_Server::createAndInitialiseOMHandler() %s","theOmHandlerPtr->Init() failed");
		return false;
	}
	INFO("ACS_LM_Server::createAndInitialiseOMHandler()  %s", "Leaving" );
	return true;
}//end of createAndInitialiseOMHandler
/*=================================================================
	ROUTINE: finaliseOMHandler
=================================================================== */
bool ACS_LM_Server::finaliseOMHandler()
{
	if( theOmHandlerPtr != 0)
	{
		INFO("ACS_LM_Server::finaliseOMHandler()  %s", "Entering" );
		if( theOmHandlerPtr->Finalize() == ACS_CC_FAILURE )
		{
			ERROR("ACS_LM_Server::finaliseOMHandler()  %s", "Finalize() - Failed" );
			return false;
		}
		INFO("ACS_LM_Server::finaliseOMHandler()  %s", "Leaving" );
	}
	return true;
}//end of finaliseOMHandler

/*=================================================================
        ROUTINE: createParticipant
=================================================================== */
/* to be integrate into your setup code if object is not exits */
int ACS_LM_Server::createParticipant(void)
{
	char attrdn[]= "brfPersistentDataOwnerId";
	char attrVersion[]="version";
	char attrBackupType[]="backupType";
	//char attrRebootAfterRestore[]="rebootAfterRestore";

	char *className = const_cast<char*>("BrfPersistentDataOwner");

	ACS_CC_ReturnType returnCode;

	//OmHandler immHandler;

	/*returnCode=immHandler.Init();
	if(returnCode!=ACS_CC_SUCCESS)
	{
		DEBUG("%s", "Error in Init() function for BrfPersistentDataOwner Object") ;
		return -1;
	}*/

	char* nomeParent = const_cast<char*>("brfParticipantContainerId=1");

	//Create attributes list
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeVersion;
	ACS_CC_ValuesDefinitionType attributeBackupType;
	//ACS_CC_ValuesDefinitionType attributeRebootAfterRestore;

	/*Fill the rdn Attribute */
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ERIC-APG-ACS-LM");
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;

	attributeVersion.attrName = attrVersion;
	attributeVersion.attrType = ATTR_STRINGT;
	attributeVersion.attrValuesNum = 1;
	char* strValue = const_cast<char*>("1.0");
	void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
	attributeVersion.attrValues = valueStr;

	attributeBackupType.attrName = attrBackupType;
	attributeBackupType.attrType = ATTR_INT32T;
	attributeBackupType.attrValuesNum = 1;
	int intAttributeBackupType = 1;
	void* valueInt[1]={ reinterpret_cast<void*>(&intAttributeBackupType)};
	attributeBackupType.attrValues = valueInt;

	//attributeRebootAfterRestore.attrName = attrRebootAfterRestore;
	//attributeRebootAfterRestore.attrType = ATTR_INT32T;
	//attributeRebootAfterRestore.attrValuesNum = 1;
	//int intRebootAfterRestore = 0;
	//void* valueInt2[1]={ reinterpret_cast<void*>(&intRebootAfterRestore)};
	//attributeRebootAfterRestore.attrValues = valueInt2;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeVersion);
	AttrList.push_back(attributeBackupType);
	//AttrList.push_back(attributeRebootAfterRestore);


	returnCode=theOmHandlerPtr->createObject(className, nomeParent, AttrList);
	if(returnCode == ACS_CC_SUCCESS)
	{
		DEBUG("%s", "BrfPartecipantDataOwner Object Creation Successful");
	}
	else
	{
		DEBUG("%s", "BrfPartecipantDataOwner Object Creation Failure");
		DEBUG(" Internal Last error text : %s ",theOmHandlerPtr->getInternalLastErrorText());
		//	returnCode=immHandler.Finalize();
		return -1;
	}

	DEBUG("%s", "BrfPartecipantDataOwner Object Created");
	return 0;
}

/*=================================================================
        ROUTINE: InitializeBrfcRunTimeOwner
=================================================================== */
bool ACS_LM_Server::initializeBrfcRunTimeOwner(acs_lm_root_objectImplementer* theLMRootObjectImplementer)
{
	DEBUG("ACS_LM_Server::InitializeBrfcRunTimeOwner()  %s", "Entering" );
	theLMBrfcRuntimeOwnerPtr->setLmRootObject(theLMRootObjectImplementer);
	theLMBrfcRuntimeHandlerThreadPtr->setImpl(theLMBrfcRuntimeOwnerPtr);
	theLMBrfcRuntimeHandlerThreadPtr->activate();
	DEBUG("ACS_LM_Server::InitializeBrfcRunTimeOwner()  %s", "Leaving" );
	return true;

}//end of InitializeBrfcRunTimeOwner


bool ACS_LM_Server::copyFileToBuffer(std::string srcPath, char* destBuf)
{
	INFO("ACS_LM_Server::copyFileToBuffer()  %s","Entering");

	ACE_HANDLE src;
	ssize_t bytesToBeWritten;
	bool myResult = true;

	ACE_stat stat_buf;  /* hold information about input file */

	if ((src = ACE_OS::open(srcPath.c_str(), O_RDONLY)) == -1)
	{
		DEBUG("ACS_LM_Server::copyFileToBuffer() Cant open the file SrcPath- %s",srcPath.c_str());
		myResult =  false;
		return myResult;
	}

	ACE_OS::fstat(src, &stat_buf);

	ACE_TCHAR buf[stat_buf.st_size];

	bytesToBeWritten = ACE_OS::read(src, buf, stat_buf.st_size);

	DEBUG("ACS_LM_Server::copyFileToBuffer() - buf = %s", buf);

	for(int i=0; i<bytesToBeWritten-1; i++)
		destBuf[i] = buf[i];

	DEBUG("ACS_LM_Server::copyFileToBuffer() - destBuf = %s", destBuf);

	INFO("ACS_LM_Server::copyFileToBuffer()  %s","Leaving");

	return true;
}

/*=================================================================
        ROUTINE: createElectronicKeyRunTimeOwner
=================================================================== */
//void ACS_LM_Server::createBrfcRunTimeOwner(acs_lm_root_objectImplementer theLMRootObjectImplementer)
void ACS_LM_Server::createBrfcRunTimeOwner()
{
	INFO("ACS_LM_Server::createBrfcRunTimeOwner()  %s", "Entering" );
	theLMBrfcRuntimeOwnerPtr = 0;
	theLMBrfcRuntimeHandlerThreadPtr =0;
	theLMBrfcRuntimeOwnerPtr = new(nothrow) ACS_LM_BrfImplementer();
	theLMBrfcRuntimeHandlerThreadPtr = new(nothrow) ACS_LM_BrfcThread();
	INFO("ACS_LM_Server::createBrfcRunTimeOwner()  %s", "Leaving" );
}//end of createBrfcRunTimeOwner

/*=================================================================
        ROUTINE: createBrfcThread
=================================================================== */
void ACS_LM_Server::createBrfcThread(acs_lm_root_objectImplementer* theLMRootObjectImplementer)
{
	INFO("ACS_LM_Server::createBrfcThread()  %s", "Entering" );

	createParticipant();

	createBrfcRunTimeOwner();

	initializeBrfcRunTimeOwner(theLMRootObjectImplementer);

	INFO("ACS_LM_Server::createBrfcThread()  %s", "Leaving" );
}

/*=================================================================
        ROUTINE: deleteParticipant
=================================================================== */
void ACS_LM_Server::deleteParticipant()
{
	INFO("ACS_LM_Server::deleteParticipant :%s", "ENTERING") ;
	//ACS_CC_ReturnType returnCode;

	theLMBrfcRuntimeHandlerThreadPtr->stop();

	theLMBrfcRuntimeHandlerThreadPtr->wait();

	theLMBrfcRuntimeHandlerThreadPtr->removeImpl(theLMBrfcRuntimeOwnerPtr);

#if 0
	OmHandler immHandler;
	returnCode=immHandler.Init();
	if( returnCode == ACS_CC_FAILURE )
	{
		ERROR("ACS_LM_Server::deleteParticipant %s","immHandler.Init() failed");

	}
	else
	{
		const char *ObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-LM,brfParticipantContainerId=1";
		try
		{
			returnCode = immHandler.deleteObject(ObjName);
		}
		catch(...)
		{

		}
	}
	if (returnCode == ACS_CC_SUCCESS)
	{
		DEBUG("ACS_LM_Server::deleteParticipant :%s", "Successful in deleting the object") ;
	}
	else
	{
		ERROR("ACS_LM_Server::deleteParticipant :%s", "Failure in deleting the object") ;
	}
	immHandler.Finalize();

#endif
	/* Close the connection with IMM, and reset all handlers */

	if (theLMBrfcRuntimeHandlerThreadPtr != 0)
	{
		DEBUG("%s", "theLMBrfcRuntimeHandlerThreadPtr != 0 Entering");
		delete 	theLMBrfcRuntimeHandlerThreadPtr;
		theLMBrfcRuntimeHandlerThreadPtr = 0 ;
		DEBUG("%s", "theLMBrfcRuntimeHandlerThreadPtr != 0 Leaving");
	}
	if (theLMBrfcRuntimeOwnerPtr != 0)
	{
		DEBUG("%s", "theLMBrfcRuntimeOwnerPtr != 0 Entering");
		delete theLMBrfcRuntimeOwnerPtr;
		theLMBrfcRuntimeOwnerPtr = 0 ;
		DEBUG("%s", "theLMBrfcRuntimeOwnerPtr != 0 Leaving");
	}

}

bool ACS_LM_Server::initialDirectorySettings()
{
	INFO("ACS_LM_Server::initialDirectorySettings() %s", "Entering");

	//Create LM directory at Primary path.	
	if(!ACS_LM_Common::createLmDir(LM_PRIMARY_DIRECTORY_PATH))
	{
		ERROR("ACS_LM_Server::initialDirectorySettings() creating primary directory at %s is failed",LM_PRIMARY_DIRECTORY_PATH);
		return false;
	}

	//Create LM directory at Secondary path. 
	if(!ACS_LM_Common::createLmDir(LM_BACKUP_DIRECTORY_PATH))
	{
		ERROR("ACS_LM_Server::initialDirectorySettings() creating backup directory at %s is failed",LM_BACKUP_DIRECTORY_PATH);
		return false;
	}

	//Delete the directory presents at previous primary path (i.e, /storage/system/config/) 
	if(!deleteDirectoryAtOldPrimaryPath(LM_OLD_PRIMARY_PATH))
	{
		ERROR("ACS_LM_Server::initialDirectorySettings() directory deletion at %s is failed,",LM_OLD_PRIMARY_PATH);
	}

	DEBUG("ACS_LM_Server::initialDirectorySettings() %s", "Leaving");

	return true;
}

bool ACS_LM_Server::loadLicenseFile()
{
	INFO("ACS_LM_Server::loadLicenseFile()  %s", "Entering" );

	//In APG/L both paths are same. It is kept to avoid changes in the Structure
	//as any change will disturb the Persistent file reading when APG/W is upgraded to APG/L
	std::string curLkfPath(LM_LKF_PRIMARY_PATH);
	std::string bkpLkfPath1(LM_LKF_BACKUP_PATH);
	std::string bkpLkfPath2(LM_LKF_BACKUP_PATH);
	

	INFO("ACS_LM_Server::loadLicenseFile() Curent LKF path=%s, Backup LKF path=%s", curLkfPath.c_str(),bkpLkfPath1.c_str());
	sentinelFile = new ACS_LM_Sentinel(curLkfPath, bkpLkfPath1,bkpLkfPath2);

	eventHandler = new ACS_LM_EventHandler();

	if((!persistFile->isLmInVirginMode()) && (persistFile->getEmergencyCounter() == 0))
	{
		DEBUG("ACS_LM_Server::loadLicenseFile(): %s", "Raising Emergency state unavailable alarm");
		eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
	}
	/* During service start, check Maintenance mode and if it has been activated, then raise alarm */
	DEBUG("ACS_LM_Server::loadLicenseFile(): LM mode=%x", persistFile->getLmMode() );
	if((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE)
        {
		DEBUG("ACS_LM_Server::loadLicenseFile(): %s", "Raising Maintenance observation alarm");
                eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE);
        }

	//Initialize the license keys
	bool aNodeInitialized, bNodeInitialized;
	persistFile->updateInitializedNodeStatus(aNodeInitialized,bNodeInitialized);

	ACE_UINT32 lkfCheckSum = persistFile->getLkfChecksum();
	//ACS_LM_Common::setLkfLockStatus(false);
	DEBUG("ACS_LM_Server::loadLicenseFile()lkfCheckSum %d",lkfCheckSum);
	sentinelFile->setInternalOMhandler(theOmHandlerPtr);
	theValidLkfFound = sentinelFile->init(aNodeInitialized, bNodeInitialized, false, lkfCheckSum);

	if(!theValidLkfFound)
	{
		ERROR("ACS_LM_Server::loadLicenseFile() %s","VALID LKF File not found in /data/acs/data/lm");
		//Remove the tampered file if existing and restore it with backup
		if(ACS_LM_Common::isFileExists(curLkfPath))
		{
			if(ACS_LM_Common::deleteFile(curLkfPath))
			{
				DEBUG("ACS_LM_Server::loadLicenseFile() %s","LKF file removed from /data/acs/data/lm Successfully");
			}
			else
			{
				ERROR("ACS_LM_Server::loadLicenseFile() %s","ERROR!!! LKF file not removed from /data/acs/data/lm");
			}
		}
		bool myRRestoreResult = ACS_LM_Common::restoreFile(bkpLkfPath1, curLkfPath);
		if(myRRestoreResult)
		{
			sentinelFile->clean();
			theValidLkfFound = sentinelFile->init(aNodeInitialized, bNodeInitialized, false, lkfCheckSum, true);
		}	
		else
		{
			DEBUG("ACS_LM_Server::loadLicenseFile() ACS_LM_Common::restoreFile(bkpLkfPath1, curLkfPath) Failed  bkpLkfPath1 = %s , curLkfPath = %s ",bkpLkfPath1.c_str(), curLkfPath.c_str() );
		}
		//CNI30_4_1761
		if(!theValidLkfFound)
		{

			sentinelFile->clean();
			bool validLkfBackupFound = sentinelFile->initFromBackup(aNodeInitialized, bNodeInitialized, false, lkfCheckSum);
			if(validLkfBackupFound)
			{
				DEBUG("ACS_LM_Server::loadLicenseFile() %s","InitFromBackup() of sentinel passed" );
				//ACS_LM_Common::setLkfLockStatus(true);
			}
			else
			{
				ERROR("ACS_LM_Server::loadLicenseFile() %s","validLkf File not found even after backup" );
				if(!persistFile->isLmInVirginMode())
				{
					eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
				}
				DEBUG("ACS_LM_Server::loadLicenseFile() %s","Going to Grace mode" );
				/* LM Maintenance Mode */
				//Do not set LM to Grace even install invlid LKF while LM in Maintenance mode.
			        if(!((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE))
				{	
					INFO("ACS_LM_Server::loadLicenseFile() %s\n", "setting to Grace mode");
					persistFile->setLmMode(ACS_LM_MODE_GRACE, true);
				}
				else
				{
					graceOnInMaintenance = TRUE;
					DEBUG("ACS_LM_Server::loadLicenseFile() %s", " LKF tampered during MM. Avoiding LM set to Grace mode during Maintenance mode"); 
				}
			}
		}	
	}
	else
	{
		DEBUG("ACS_LM_Server::loadLicenseFile() %s","Init() of sentinel passed" );
	}

	if(theValidLkfFound && thePersistFileValid)
	{
		DEBUG("ACS_LM_Server::loadLicenseFile() %s","theValidLkfFound && thePersistFileValid passed" );
		persistFile->setInitializedNodeStatus(aNodeInitialized, bNodeInitialized);
		DEBUG("ACS_LM_Server::loadLicenseFile() %s","Before sentinelFile->backup()" );
		sentinelFile->backup();
	}
	if(!persistFile->commit())
	{
		DEBUG("ACS_LM_Server::loadLicenseFile() %s","persistFile->commit() failed");
	}

	if((!persistFile->isLmInVirginMode()) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) == ACS_LM_MODE_GRACE) )
	{
		DEBUG("ACS_LM_Server::loadLicenseFile() %s","Virgin Mode and Grace Mode" );
		eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
		eventHandler->clearAll();
	}
	else
	{
		// need to clear the list for failover case
		DEBUG("ACS_LM_Server::loadLicenseFile() %s","clearing lists for failover");
		eventHandler->clearLists();  
	}

	ACS_LM_Server::isOldDirectoryDeleted = false;  

	INFO("ACS_LM_Server::loadLicenseFile()  %s", "Leaving" );
	return true;

}
bool ACS_LM_Server::loadPersistentFile()
{
	INFO("ACS_LM_Server::loadPersistentFile()  %s", "Entering" );
	//In APG/L both paths are same. It is kept to avoid changes in the Structure
	// as any change will disturb the Persistent file reading when APG/W is upgraded to APG/L
	std::string theCurPersistPath(LM_PERSISTENT_PRIMARY_PATH);
	std::string bkpPersistPath1(LM_PERSISTENT_BACKUP_PATH);
	std::string bkpPersistPath2 (LM_PERSISTENT_BACKUP_PATH);

	persistFile = ACS_LM_Persistent::load(theCurPersistPath, bkpPersistPath1,bkpPersistPath2);
	if(persistFile == NULL)
	{
		//Remove the tampered file if existing and restore it with backup
		if(ACS_LM_Common::isFileExists(theCurPersistPath))
		{
			if(ACS_LM_Common::deleteFile(theCurPersistPath))
			{
				DEBUG("ACS_LM_Server::loadPersistentFile() %s","Persistant file removed from PSA Path Successfully");
			}
			else
			{
				DEBUG("ACS_LM_Server::loadPersistentFile() %s","ERROR!!! persistant file not removed from PSA Path");
			}
		}

		bool myRestoreResult = ACS_LM_Common::restoreFile(bkpPersistPath1, theCurPersistPath);
		if(myRestoreResult)
		{
			persistFile = ACS_LM_Persistent::load(theCurPersistPath, bkpPersistPath1,bkpPersistPath2,true);
		}
		else
		{
			ERROR("ACS_LM_Server::loadPersistentFile(): restoreFile(%s,%s)failed ",bkpPersistPath1.c_str(),theCurPersistPath.c_str());
		}

		if(persistFile == NULL)
		{
			persistFile = ACS_LM_Persistent::load(theCurPersistPath, bkpPersistPath1,bkpPersistPath2, true, true);  
			//In case
			if(persistFile == NULL)
			{
				ERROR("ACS_LM_Server::loadPersistentFile() ACS_LM_Persistent::load(theCurPersistPath, bkpPersistPath1,bkpPersistPath2, true) - RETURN NULL curPersistPath = %s bkpPersistPath1 = %s",theCurPersistPath.c_str(),bkpPersistPath1.c_str());
				//cannot proceed without persistent file
				ACS_LM_EventHandler::LmEvent pfFault;
				pfFault.specificProblem = ACS_LM_EventHandler::ACS_LM_ALARM_PERSISTENT_FAULT;//need to confirm
				pfFault.probableCause = "PERSISTENT FILE CANNOT BE CREATED";
				pfFault.problemData = "";
				pfFault.problemText = "";
				pfFault.perceivedSeverity = "EVENT";
				ACS_LM_EventHandler::reportEvent(pfFault);
				ERROR("ACS_LM_Server::loadPersistentFile() ACS_LM_EventHandler::reportEvent(pfFault) %s","Returning");
				return false;
			}
			else
			{
				//CNI30_4_1761
				if(persistFile->getLkfChecksum() == 0)
				{
					DEBUG("ACS_LM_Server::loadPersistentFile()%s","Going to GRACE Mode");
					persistFile->setLmMode(ACS_LM_MODE_GRACE, true);
				}
			}
			thePersistFileValid = false;
		}
	}
	INFO("ACS_LM_Server::loadPersistentFile()  %s", "Leaving" );
	return true;
}
/*=================================================================
	ROUTINE: CreateComponents
=================================================================== */
bool ACS_LM_Server::createComponents()
{
	DEBUG("ACS_LM_Server::createComponents()  %s", "Entering" );

	clientHandler = new ACS_LM_ClientHandler(sentinelFile, persistFile, eventHandler);
	scheduler = new(nothrow) ACS_LM_Scheduler();
	//cpMonitor = new(nothrow) ACS_LM_CpRestartMonitor(stopThreadFds[0]);
	cpMonitor = new(nothrow) ACS_LM_CpRestartMonitor();

	theDSDServerPtr = new ACS_LM_DSDServer(stopThreadFds[0]);
	theDSDServerPtr->setClienthandler(clientHandler);

	createElectronicKeyRunTimeOwner();

	if(!getShudownSignalFlag())
	{
		if(createAndInitialiseRootOIHandler(clientHandler, theElectronicKeyLMRuntimeOwnerPtr) == false)
		{
			ERROR("ACS_LM_Server::createComponents() %s","createAndInitialiseRootOIHandler() failed");
			return false;
		}
	}
	else
	{
		ERROR("ACS_LM_Server::createComponents() %s","shutdown is signalled - do not start other inits");
		return false;
	}

	if(!getShudownSignalFlag())
	{
		if(createAndInitialiseManagedOIHandler(clientHandler, persistFile,eventHandler) == false)
		{
			ERROR("ACS_LM_Server::createComponents() %s","createAndInitialiseManagedOIHandler() failed");
			return false;
		}
	}
	else
	{
		ERROR("ACS_LM_Server::createComponents() %s","shutdown is signalled - do not start other inits");
		return false;
	}

	if(!getShudownSignalFlag())
	{
		if(createAndInitialiseNorthBouudOIHandler(clientHandler, persistFile,eventHandler) == false)
		{
			ERROR("ACS_LM_Server::createComponents() %s","createAndInitialiseNorthBouudOIHandler() failed");
			return false;
		}
	}
	else
	{
		ERROR("ACS_LM_Server::createComponents() %s","shutdown is signalled - do not start other inits");
		return false;
	}

	if(!getShudownSignalFlag())
	{
		if(InitializeElectronicKeyRunTimeOwner() == false)
		{
			ERROR("ACS_LM_Server::createComponents() %s","InitializeElectronicKeyRunTimeOwner() failed");
			return false;
		}
	}
	else
	{
		ERROR("ACS_LM_Server::createComponents() %s","shutdown is signalled - do not start other inits");
		return false;
	}

	ERROR("ACS_LM_Server::createComponents() %s","InitializeElectronicKeyRunTimeOwner() Success");

	if(!getShudownSignalFlag())
	{
		theLMRootObjectImplementer->setClienthandler(clientHandler);
		theLMRootObjectImplementer->setElectronicKeyhandler(theElectronicKeyLMRuntimeOwnerPtr);
		theLMRootObjectImplementer->setOmHandler(theOmHandlerPtr);

		theLMManagedObjectImplementer->setOmHandler(theOmHandlerPtr);
		theLMManagedObjectImplementer->setClienthandler(clientHandler);
		theLMManagedObjectImplementer->setPersistant(persistFile);
		theLMManagedObjectImplementer->setEventHandler(eventHandler);

		theLMNorthboundObjectImplementer->setOmHandler(theOmHandlerPtr);
		theLMNorthboundObjectImplementer->setClienthandler(clientHandler);
		theLMNorthboundObjectImplementer->setPersistant(persistFile);
		theLMNorthboundObjectImplementer->setEventHandler(eventHandler);


		theElectronicKeyLMRuntimeOwnerPtr->setSentinelObjectPtr(sentinelFile);
		theElectronicKeyLMRuntimeOwnerPtr->setInternalOMhandler(theOmHandlerPtr);
		theElectronicKeyLMRuntimeOwnerPtr->updateElectronicKeyRDNList();
		theElectronicKeyLMRuntimeOwnerPtr->createRuntimeObjectElectronicKeyObjects();


		//Activate DSD Communication Thread
		theDSDServerPtr->start();

		//Start 24 Hours timer
		scheduler->start(sentinelFile, persistFile, eventHandler);

		//Start CP restart monitor
		cpMonitor->setComponents(sentinelFile, persistFile, eventHandler);
		cpMonitor->start();

		createBrfcThread(theLMRootObjectImplementer);
		DEBUG("ACS_LM_Server::createComponents()  %s", "Leaving" );
		return true;
	}
	else
	{
		ERROR("ACS_LM_Server::createComponents() %s","shutdown is signalled");
		return false;
	}
}
/*=================================================================
	ROUTINE: run
=================================================================== */
void ACS_LM_Server::run()
{
	DEBUG("ACS_LM_Server::run()  %s", "Entering" );
	thePersistFileValid = true;
	theValidLkfFound = true;
	fpChangeClusterIP = false;
	fpChangeNodeName = false;
        
	ACS_LM_Server::isShutdown=false;
	ACS_LM_Server::isOldDirectoryDeleted=false;
	//Variables defined to fix the Grace Issue occured during the upgrade from 3.0.5 to 3.2.0
	ACS_LM_Server::pp_LKF_Checksum=0;
	ACS_LM_Server::pp_persistChecksum=0;
	ACS_LM_Server::pp_calcChecksum=0;
	ACS_LM_Server::sp_persistChecksum=0;
	ACS_LM_Server::sp_calcChecksum=0;

	INFO("In ACS_LM_Server run isShutdown is : %d", ACS_LM_Server::isShutdown);
	
	if(createAndInitialiseOMHandler() == false)
	{
		ERROR("ACS_LM_Server::loadLicenseFile() %s","createAndInitialiseOMHandler() failed");
		return ;
	}

	if (ACS_LM_Common::fetchDnOfRootObjFromIMM (theOmHandlerPtr) == -1)
	{
		ERROR("%s","LM Related XML were not loaded into IMM.");
		return;
	}

	if(initialDirectorySettings() == false)
	{
		ERROR("ACS_LM_Server::run()  %s", "initialDirectorySettings failed" );
		return;
	}
	if(loadPersistentFile() == false)
	{
		ERROR("ACS_LM_Server::run()  %s", "loadPersistentFile failed" );
		return;
	}
	if(loadLicenseFile() == false)
	{
		ERROR("ACS_LM_Server::run()  %s", "loadLicenseFile failed" );
		return;
	}
	if(createComponents() == false)
	{
		ERROR("ACS_LM_Server::run()  %s", "createComponents failed" );
		return;
	}
	DEBUG("ACS_LM_Server::run()  %s", "Waiting for Stop signal from HA" );

	bool myContinue = true;

	while(myContinue)
	{
		ACE_Message_Block * myMessageBlock = 0;
		int myRet = this->getq(myMessageBlock);
		if(myRet != -1)
		{
			if(myMessageBlock != 0 )
			{
				if(myMessageBlock->msg_type() == ACE_Message_Block::MB_HANGUP )
				{
					DEBUG("ACS_LM_Server::run()  %s", "Stop signal came from HA" );
					myContinue = false;
					DEBUG("ACS_LM_Server::run()  %s", "myContinue is made false" );
				}
				myMessageBlock->release();
			}
		}

	}

	stopService();
	DEBUG("ACS_LM_Server::run()  %s", "Leaving" );
}//end of run
/*=================================================================
	ROUTINE: createLMModeRO
=================================================================== */
bool ACS_LM_Server::createLMModeRO(ACE_TCHAR* attrRDN )
{
	DEBUG("ACS_LM_Server::createLMModeRO() %s","Entering");
	ACE_INT32 numAttr = 7;
	bool flag = true;
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	//the attributes
	ACS_CC_ValuesDefinitionType *attributes = new ACS_CC_ValuesDefinitionType[numAttr];

	DEBUG("ACS_LM_Server::createLMModeRO()  RDN is %s",attrRDN);

	//Fill the theLicInitParamId Attribute
	attributes[0].attrName = (char *)LICINITRDN_ATTRIBUTE;
	attributes[0].attrType = ATTR_STRINGT;
	attributes[0].attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(attrRDN)};
	attributes[0].attrValues = valueRDN;
	AttrList.push_back(attributes[0]);

	//Fill the attribute FINGERPRINT_ATTRIBUTE
	attributes[1].attrName =  (char *)FINGERPRINT_ATTRIBUTE;
	attributes[1].attrType = ATTR_STRINGT;
	attributes[1].attrValuesNum = 0;
	attributes[1].attrValues = 0;
	AttrList.push_back(attributes[1]);

	//Fill the attribute LMMODE_ATTRIBUTE
	attributes[2].attrName =  (char *)LMMODE_ATTRIBUTE;
	attributes[2].attrType = ATTR_STRINGT;
	attributes[2].attrValuesNum = 0;
	attributes[2].attrValues = 0;
	AttrList.push_back(attributes[2]);

	//Fill the attribute EMERGENCYENDDATE_ATTRIBUTE
	attributes[3].attrName = (char *)EMERGENCYENDDATE_ATTRIBUTE;
	attributes[3].attrType = ATTR_STRINGT;
	attributes[3].attrValuesNum = 0;
	attributes[3].attrValues = 0;
	AttrList.push_back(attributes[3]);

	//Fill the attribute EMERGENCYACTIVATIONCOUNT_ATTRIBUTE
	attributes[4].attrName = (char *)EMERGENCYACTIVATIONCOUNT_ATTRIBUTE;
	attributes[4].attrType = ATTR_UINT32T;
	attributes[4].attrValuesNum = 0;
	attributes[4].attrValues = 0;
	AttrList.push_back(attributes[4]);


	if (theLMRuntimeOwnerPtr->createRuntimeObj(LICENSEMODE_CLASSNAME, ACS_LM_Common::dnOfLMRoot.c_str() , AttrList ) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_Server::createLMModeRO() - Creation of Runtime object for Class - %s  ParentRDN - %s is Failed",LICENSEMODE_CLASSNAME,ACS_LM_Common::dnOfLMRoot.c_str());
		flag = false;
	}


	AttrList.clear();
	delete[] attributes;

	DEBUG("ACS_LM_Server::createLMModeRO() %s","Leaving");
	return flag;
}//end of createLMModeRO
/*=================================================================
	ROUTINE: createAndInitialiseRootOIHandler
=================================================================== */
bool ACS_LM_Server::createAndInitialiseRootOIHandler(ACS_LM_ClientHandler* clientHandler, ACS_LM_ElectronicKey_RuntimeOwner* electronicKeyLMRuntimeOwnerPtr)
{
	(void)clientHandler;
	(void)electronicKeyLMRuntimeOwnerPtr;
	DEBUG("ACS_LM_Server::createAndInitialiseRootOIHandler %s","Entering");
	theLMRootObjectImplementer = new acs_lm_root_objectImplementer(ACS_IMM_LM_ROOT_IMPLEMENTER_NAME);

	ACS_CC_ReturnType myReturnErrorCode;

	if(theLMRootObjectImplementer != 0)
	{
		DEBUG("%s", "Starting Root ObjectImplementer");

		while(!getShudownSignalFlag())
		{
			ERROR("%s","LM Root OI - Calling Root OI handler addClassImpl()!");

			if(theLMRootObjectImplementer->setLMRootObjectImplementer() == ACS_CC_FAILURE)
			{
				ERROR("%s","LM Root OI - addClassImpl() failed! Sleeping 0.5 seconds");
				usleep(500000);
			}
			else
			{
				ERROR("%s","LM Root OI - addClassImpl() successful!");
				break;
			}
		}

		if(getShudownSignalFlag())
		{
			ERROR("%s","createAndInitialiseRootOIHandler() - shutdown is signalled - do not start Root OI thread!");
			return ACS_APGCC_FAILURE;
		}

		myReturnErrorCode = theLMRootObjectImplementer->setupLMOIThread(theLMRootObjectImplementer);
		if(myReturnErrorCode == ACS_CC_FAILURE)
		{
			ERROR( "%s", "Leaving createAndInitialiseRootOIHandler");
			delete theLMRootObjectImplementer;
			theLMRootObjectImplementer = 0;
			kill(getpid(), SIGTERM);
			return ACS_APGCC_FAILURE;
		}

	}
	else
	{
		ERROR( "%s", "Memory allocation failed for theLMRootObjectImplementer");
		ERROR( "%s", "Leaving  createAndInitialiseRootOIHandler");
		kill(getpid(), SIGTERM);
		return ACS_APGCC_FAILURE;
	}

	//theLMRootObjectImplementer->svc(m_poReactor);
	DEBUG("ACS_LM_Server::createAndInitialiseRootOIHandler() %s","Leaving");
	return true;
}//end of createAndInitialiseRootOIHandler

/*=================================================================
	ROUTINE: createAndInitialiseManagedOIHandler
=================================================================== */
bool ACS_LM_Server::createAndInitialiseManagedOIHandler(ACS_LM_ClientHandler* clientHandler, ACS_LM_Persistent* persistant,ACS_LM_EventHandler* eventHandler)
{
	(void)clientHandler;
	(void)persistant;
	(void)eventHandler;
	DEBUG("ACS_LM_Server::createAndInitialiseManagedOIHandler %s","Entering");
	theLMManagedObjectImplementer = new acs_lm_managed_objectImplementer(ACS_IMM_LM_MANAGED_IMPLEMENTER_NAME);

	ACS_CC_ReturnType myReturnErrorCode;

	if(theLMManagedObjectImplementer != 0)
	{
		DEBUG("%s", "Starting Managed ObjectImplementer");

		while(!getShudownSignalFlag())
		{
			ERROR("%s","LM Managed OI - Calling Managed OI handler addClassImpl()!");

			if(theLMManagedObjectImplementer->setLMManagedObjectImplementer() == ACS_CC_FAILURE)

			{
				ERROR("%s","LM Managed OI - addClassImpl() failed! Sleeping 0.5 seconds");
				usleep(500000);
			}
			else
			{
				ERROR("%s","LM Managed OI - addClassImpl() successful!");
				break;
			}
		}

		if(getShudownSignalFlag())
		{
			ERROR("%s","createAndInitialiseManagedOIHandler() - shutdown is signalled - do not start Managed OI thread!");
			return ACS_APGCC_FAILURE;
		}

		myReturnErrorCode = theLMManagedObjectImplementer->setupLMManagedOIThread(theLMManagedObjectImplementer);
		if(myReturnErrorCode == ACS_CC_FAILURE)
		{
			ERROR( "%s", "Leaving createAndInitialiseManagedOIHandler");
			delete theLMManagedObjectImplementer;
			theLMManagedObjectImplementer = 0;
			kill(getpid(), SIGTERM);
			return ACS_APGCC_FAILURE;
		}

	}
	else
	{
		ERROR( "%s", "Memory allocation failed for theLMRootObjectImplementer");
		ERROR( "%s", "Leaving  createAndInitialiseManagedOIHandler");
		kill(getpid(), SIGTERM);
		return ACS_APGCC_FAILURE;
	}

	//theLMRootObjectImplementer->svc(m_poReactor);
	DEBUG("ACS_LM_Server::createAndInitialiseManagedOIHandler() %s","Leaving");
	return true;
}//end of createAndInitialiseManagedOIHandler


/*=================================================================
	ROUTINE: createAndInitialiseNorthBouudOIHandler
=================================================================== */
bool ACS_LM_Server::createAndInitialiseNorthBouudOIHandler(ACS_LM_ClientHandler* clientHandler, ACS_LM_Persistent* persistant,ACS_LM_EventHandler* eventHandler)
{
	(void)clientHandler;
	(void)persistant;
	(void)eventHandler;
	DEBUG("ACS_LM_Server::createAndInitialiseNorthBouudOIHandler %s","Entering");
	theLMNorthboundObjectImplementer = new acs_lm_northbound_objectImplementer(ACS_IMM_LM_NORTHBOUND_IMPLEMENTER_NAME);

	ACS_CC_ReturnType myReturnErrorCode;

	if(theLMNorthboundObjectImplementer != 0)
	{
		DEBUG("%s", "Starting North Bound ObjectImplementer");

		while(!getShudownSignalFlag())
		{
			ERROR("%s","LM North Bound OI - Calling Root OI handler addClassImpl()!");

			if(theLMNorthboundObjectImplementer->setLMNorthBoundObjectImplementer() == ACS_CC_FAILURE)

			{
				ERROR("%s","LM North Bound OI - addClassImpl() failed! Sleeping 0.5 seconds");
				usleep(500000);
			}
			else
			{
				ERROR("%s","LM North Bound OI - addClassImpl() successful!");
				break;
			}
		}

		if(getShudownSignalFlag())
		{
			ERROR("%s","createAndInitialiseNorthBouudOIHandler() - shutdown is signalled - do not start North Bound OI thread!");
			return ACS_APGCC_FAILURE;
		}

		myReturnErrorCode = theLMNorthboundObjectImplementer->setupLMNorthboundOIThread(theLMNorthboundObjectImplementer);
		if(myReturnErrorCode == ACS_CC_FAILURE)
		{
			ERROR( "%s", "Leaving createAndInitialiseNorthBouudOIHandler");
			delete theLMNorthboundObjectImplementer;
			theLMNorthboundObjectImplementer = 0;
			kill(getpid(), SIGTERM);
			return ACS_APGCC_FAILURE;
		}

	}
	else
	{
		ERROR( "%s", "Memory allocation failed for theLMNorthboundObjectImplementer");
		ERROR( "%s", "Leaving  createAndInitialiseNorthBouudOIHandler");
		kill(getpid(), SIGTERM);
		return ACS_APGCC_FAILURE;
	}

	//theLMRootObjectImplementer->svc(m_poReactor);
	DEBUG("ACS_LM_Server::createAndInitialiseNorthBouudOIHandler() %s","Leaving");
	return true;
}//end of createAndInitialiseNorthBouudOIHandler

/*=======================================================================
	ROUTINE: deleteDirectoryAtOldPrimaryPath
	Description: This method deletes the directory(ACS-LM) presents
		     at "/storage/system/config/" path, as this directory
		     is not used anymore. 
========================================================================= */
bool ACS_LM_Server::deleteDirectoryAtOldPrimaryPath( std::string path )
{
	DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() Entering  path = %s",path.c_str());
	bool isDirectoryDeleted = false;
	DIR *dir = NULL;
	struct dirent *entry = 0;

        dir = opendir(path.c_str());

        if (dir == NULL)
        {
                DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() %s ","Directory is not present" );
		isDirectoryDeleted = false;
        }
        else
        {
                DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() %s ","Directory is present" );
		bool fileRemoveStatus = true; 

		while((entry = readdir(dir))!= NULL)
                {
                        std::string fileName(entry->d_name);
                        if( fileName != "." && fileName != "..")
                        {
                                std::string fileFullPath = path + std::string("/") + fileName;
                        	if(remove(fileFullPath.c_str())!= 0)
                        	{	
                                	DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() Could not delete File %s",fileFullPath.c_str());
                                	fileRemoveStatus = false;
                        	}
			}
                }

		if (fileRemoveStatus)
                {
                        int retVal = rmdir(path.c_str());
                        if(retVal != 0)
                        {
                                DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() Could not delete directory %s",path.c_str());
                                isDirectoryDeleted = false;
                        }
                        else
                        {
                                DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() Deleted the directory %s",path.c_str());
                                isDirectoryDeleted = true;
				ACS_LM_Server::isOldDirectoryDeleted=true;
                        }
                }
                else
                {
                        DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() Error in removing one of the file in directory %s",path.c_str());
                        isDirectoryDeleted = false;
                }

                closedir (dir);
	}
	DEBUG("ACS_LM_Server::deleteDirectoryAtOldPrimaryPath() %s","Leaving");
 	return isDirectoryDeleted;
}

bool ACS_LM_Server::getShudownSignalFlag()
{
	ACS_LM_Server::lmServerMutex.acquire();
	bool myStatus = isShutdownSignalled;
	ACS_LM_Server::lmServerMutex.release();
	return myStatus;
}

void ACS_LM_Server::setShudownSignalFlag(bool myValue)
{
	ACS_LM_Server::lmServerMutex.acquire();
	isShutdownSignalled = myValue;
	ACS_LM_Server::lmServerMutex.release();
}



