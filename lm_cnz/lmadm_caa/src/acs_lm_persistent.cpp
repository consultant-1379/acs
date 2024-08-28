//******************************************************************************
//
//  NAME
//     acs_lm_persistent.cpp
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

#include "acs_lm_persistent.h"
#include "acs_lm_tra.h"
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Time_Value.h>
#include "acs_lm_server.h" 
//#include "acs_lm_tracemacro.h"
//LM Maintenance
bool isFromAutoCease;
ACE_INT64 maintenanceEndTime;  

//CRITICAL_SECTION ACS_LM_Persistent::writeMutex;
//ACE_Recursive_Thread_Mutex  ACS_LM_Persistent::writeMutex;

/*=================================================================
	ROUTINE: ACS_LM_Persistent constructor
=================================================================== */

ACS_LM_Persistent::ACS_LM_Persistent()
:hThread(0),
 testLkf(NULL),
 virginMode(true),
 aNodeInitDone(false),
 bNodeInitDone(false),
 lmMode(0),
 emergencyCounter(0),
 lkfChecksum(0),
 emergencyModeTime(0),
 maintenanceModeTime(0),   /* LM Maintenance Mode */ 
 exportFileExpiryTime(0),  /* Export file - House keeping */
 persistChecksum(0)
{
	DEBUG("ACS_LM_Persistent::ACS_LM_Persistent() %s","Entering");
	DEBUG("ACS_LM_Persistent::ACS_LM_Persistent() %s","Leaving");
}//end of constructor

/*=================================================================
	ROUTINE: ACS_LM_Persistent destructor
=================================================================== */
ACS_LM_Persistent::~ACS_LM_Persistent()
{
	DEBUG("ACS_LM_Persistent::~ACS_LM_Persistent() %s","Entering");

	delete testLkf;	

	if(hThread != 0)
	{
		ACE_Thread_Manager::instance()->cancel(hThread);
		ACE_Thread_Manager::instance()->close();
		hThread = 0;
	}
	DEBUG("ACS_LM_Persistent::~ACS_LM_Persistent() %s","Leaving");
}//end of destructor

/*=================================================================
	ROUTINE: setLkfChecksum
=================================================================== */
void ACS_LM_Persistent::setLkfChecksum(ACE_UINT32 lkfChecksum)
{
	DEBUG("ACS_LM_Persistent::setLkfChecksum() %s","Entering");

	this->lkfChecksum = lkfChecksum;

	DEBUG("ACS_LM_Persistent::setLkfChecksum() %s","Leaving");

}//end of setLkfChecksum

/*=================================================================
	ROUTINE: setLmMode
=================================================================== */
void ACS_LM_Persistent::setLmMode(ACS_LM_AppMode appMode, bool activate)
{
	DEBUG("ACS_LM_Persistent::setLmMode() %s","Entering");
	if(activate)
	{
		this->lmMode = this->lmMode | appMode;
		if(appMode == ACS_LM_MODE_GRACE)
		{
			INFO("ACS_LM_Persistent::setLmMode() %s","GRACE MODE");
			//Should not allow same lks in old LKF to be sent to CP
			this->lkfChecksum = 0;
			//Shall erase the test mode it already present.
			this->lmMode = this->lmMode & (0xFFFF ^ ACS_LM_MODE_TEST);
		}
		else
		{
			if(appMode == ACS_LM_MODE_EMERGENCY)
			{
				INFO("ACS_LM_Persistent::setLmMode() %s","EMERGENCY MODE");
				time_t curTime = ::time(NULL);
				//emergency mode to be deactivated in 7 more days. so adding 
				//6 days' time to current date
				struct tm curTimeLocal = *localtime(&curTime);
				curTimeLocal.tm_hour = 0;
				curTimeLocal.tm_min = 0;
				curTimeLocal.tm_sec = 0;
				curTime = ::mktime(&curTimeLocal);
				ACE_INT64 emergencyExpiryDays = (EMERGENCY_EXPIRY_DAYS*24*60*60);
				this->emergencyModeTime = curTime + emergencyExpiryDays;
				INFO("ACS_LM_Persistent::setLmMode() current time=%ld,emergencyMode end Time=%ld",curTime,this->emergencyModeTime);

			}
			/* LM Maintenance Mode */
                        else if(appMode == ACS_LM_MODE_MAINTENANCE )
			{
			        DEBUG("ACS_LM_Persistent::setLmMode() %s\n","MAINTENANCE MODE setting Offset time");
                                time_t curTime = ::time(NULL);
                                //maintenance mode to be deactivated in 25 more days. so adding
                                //25 days' time to current date
                                struct tm curTimeLocal = *localtime(&curTime);
                                curTimeLocal.tm_hour = 0;
                                curTimeLocal.tm_min = 0;
                                curTimeLocal.tm_sec = 0;
                                curTime = ::mktime(&curTimeLocal);
                                ACE_INT64 maintenanceExpiryDays = (MAINTENANCE_EXPIRY_DAYS*24*60*60);
                                this->maintenanceModeTime = curTime + maintenanceExpiryDays;
				INFO("ACS_LM_Persistent::setLmMode() current time=%ld,maintenanceMode end Time=%ld",curTime,this->maintenanceModeTime);
				maintenanceEndTime = this->maintenanceModeTime;					
			}
		}
	}
	else
	{
		this->lmMode = this->lmMode & (0xFFFF ^ appMode);
		if(appMode == ACS_LM_MODE_EMERGENCY)
		{
			this->emergencyModeTime = 0;
		}
		/* LM Maintenance Mode */ 
		else if(appMode == ACS_LM_MODE_MAINTENANCE )
		{
			this->maintenanceModeTime = 0;
		}
	}	
	DEBUG("ACS_LM_Persistent::setLmMode() %s","Leaving");
}//end of setLmMode

/*=================================================================
	ROUTINE: setEmergencyCounter
=================================================================== */
void ACS_LM_Persistent::setEmergencyCounter(ACE_INT16 emCount)
{
	DEBUG("ACS_LM_Persistent::setEmergencyCounter() %s","Entering");
	this->emergencyCounter = emCount;
	DEBUG("ACS_LM_Persistent::setEmergencyCounter() %s","Leaving");
}//end of setEmergencyCounter

/*=================================================================
	ROUTINE: disableVirginMode
=================================================================== */
void ACS_LM_Persistent::disableVirginMode()
{
	DEBUG("ACS_LM_Persistent::disableVirginMode() %s","Entering");
	virginMode = false;
	DEBUG("ACS_LM_Persistent::disableVirginMode() %s","Leaving");
}//end of disableVirginMode
/*=================================================================
	ROUTINE: isLmInVirginMode
=================================================================== */
bool ACS_LM_Persistent::isLmInVirginMode()
{
	DEBUG("ACS_LM_Persistent::isLmInVirginMode() %s","Entering");
	bool lkfActive = false;
	if(virginMode != 0)   
	{
		lkfActive = true;
	}
	DEBUG("ACS_LM_Persistent::isLmInVirginMode() %s","Leaving");
	return lkfActive;
}//end of isLmInVirginMode
/*=================================================================
	ROUTINE: getLmMode
=================================================================== */
ACE_UINT8 ACS_LM_Persistent::getLmMode()
{
	return lmMode;

}//end of getLmMode
/*=================================================================
	ROUTINE: getEmergencyCounter
=================================================================== */
ACE_UINT8 ACS_LM_Persistent::getEmergencyCounter()
{
	return emergencyCounter;
}//end of getEmergencyCounter
/*=================================================================
	ROUTINE: getLkfChecksum
=================================================================== */
ACE_UINT32 ACS_LM_Persistent::getLkfChecksum()
{
	DEBUG("The value of getLkfChecksum : %d",lkfChecksum)
	return lkfChecksum;
}//end of getLkfChecksum

ACE_UINT16 ACS_LM_Persistent::getLmdataChecksum()
{
	DEBUG("ACS_LM_Persistent getLmdataChecksum() persistChecksum = %d",persistChecksum);
	return persistChecksum;
}
/*=================================================================
	ROUTINE: isEmergencyModeExpired
=================================================================== */
bool ACS_LM_Persistent::isEmergencyModeExpired()
{
	DEBUG("ACS_LM_Persistent::isEmergencyModeExpired() %s","Entering");
	bool expired = true;
	bool isEMExpiryDayArrived = false; 

	time_t curTime = ACE_OS::time(NULL);
	ACE_INT64 diffCurrentToEMEndTime = difftime(emergencyModeTime, curTime);
	INFO("ACS_LM_Persistent:: isEmergencyModeExpired() current time=%ld, emergencyModeTime=%ld, difference time=%ld",curTime,emergencyModeTime,diffCurrentToEMEndTime);

	isEMExpiryDayArrived = isModeExpired(emergencyModeTime);

	if(((emergencyModeTime !=0) && ((curTime > emergencyModeTime) || (isEMExpiryDayArrived))))
	{
		setLmMode(ACS_LM_MODE_EMERGENCY, false);
        	INFO("ACS_LM_Persistent::isEmergencyModeExpired() %s","Emergency mode expired");
	}
	else
	{
		expired = false;
        	INFO("ACS_LM_Persistent::isEmergencyModeExpired() %s","Emergency mode not expired");
	}
	DEBUG("ACS_LM_Persistent::isEmergencyModeExpired() %s","Leaving");
	return expired;
}//end of isEmergencyModeExpired
/* LM Maintenance Mode */
/*=================================================================
        ROUTINE: isMaintenanceModeExpired
=================================================================== */
bool ACS_LM_Persistent::isMaintenanceModeExpired( )
{
        INFO("ACS_LM_Persistent::isMaintenanceModeExpired() %s","Entering");
        bool expired = true;
	bool isMMExpiryDayArrived = false; 

        time_t curTime = ACE_OS::time(NULL);
	ACE_INT64 diffCurrentToMMEndTime = difftime(maintenanceModeTime, curTime);
	INFO("ACS_LM_Persistent::isMaintenanceModeExpired() current time=%ld,maintenanceMode end Time=%ld, difference time=%ld",curTime,maintenanceModeTime,diffCurrentToMMEndTime);

	isMMExpiryDayArrived = isModeExpired(maintenanceModeTime);

	if(((maintenanceModeTime !=0) && ((curTime > maintenanceModeTime) || (isMMExpiryDayArrived))))	
        {
        	INFO("ACS_LM_Persistent::isMaintenanceModeExpired() %s","Maintenance mode expired");
                setLmMode(ACS_LM_MODE_MAINTENANCE, false);
		isFromAutoCease = true;  //This var being checked in 'modify' callback.
		//Set the 'maintenanceUnlock' attribute to 'LOCKED'.
		setMaintenanceUnlockToLock();

 	        //Check conditions to start Grace mode. If satisfy, then set LM mode to Grace mode.
                //Checking the FingerPrint
                INFO("acs_lm_clienthandler.cpp() : fpChangeNodeName %d..fpChangeClusterIP %d..graceOnInMaintenance=%d\n",fpChangeNodeName, fpChangeClusterIP,graceOnInMaintenance);
                if( fpChangeNodeName || fpChangeClusterIP || graceOnInMaintenance  )
                {
               		INFO("ACS_LM_Persistent::isMaintenanceModeExpired() %s\n", "FingerPrint corrupted. So setting to Grace mode");
               		setLmMode(ACS_LM_MODE_GRACE, true);
			//graceOnInMaintenance = false;
                }
        }
        else
        {
                expired = false;
        	INFO("ACS_LM_Persistent::isMaintenanceModeExpired() %s","Maintenance mode not expired");
        }
        INFO("ACS_LM_Persistent::isMaintenanceModeExpired() %s","Leaving");
        return expired;
}//end of isMaintenanceModeExpired
/*=================================================================
	ROUTINE: getEmergencyModeEndDate
=================================================================== */
ACE_INT64 ACS_LM_Persistent::getEmergencyModeEndDate()
{
	return emergencyModeTime;
}//end of getEmergencyModeEndDate
/* LM Maintenance Mode */
/*================================================================
        ROUTINE: getMaintenanceModeEndDate                       
================================================================== */
ACE_INT64 ACS_LM_Persistent::getMaintenanceModeEndDate()
{
	return maintenanceModeTime;
}//end of getMaintenanceModeEndDate
/*=================================================================
	ROUTINE: testLkFile
=================================================================== */

ACS_LM_TestLkf* ACS_LM_Persistent::testLkFile()
{
	return testLkf;
}//end of testLkFile
/*=================================================================
	ROUTINE: load
=================================================================== */
ACS_LM_Persistent* ACS_LM_Persistent::load(const std::string& curPath, 
										   const std::string& bkpPath1,
										   const std::string& bkpPath2,
										   bool LoadedBkpFile,
										   bool forceLoad)
{
	DEBUG("ACS_LM_Persistent::load() %s","Entering");
	INFO("load(): curPath = %s, bkpPath1=%s, forceLoad=%d",curPath.c_str(),bkpPath1.c_str(),forceLoad);
	//initially set the lmdataslockstatus to false
	ACS_LM_Common::setLmdataLockStatus(false);
	ACS_LM_Persistent* pFile = NULL;
	std::string myFilePath = curPath;
	bool myFileExists = false;
	if(ACS_LM_Common::isFileExists(myFilePath) == false)
	{
		INFO("ACS_LM_Persistent::load() myFilePath= %s does not exist",myFilePath.c_str());
		myFileExists= false;
	}
	else
	{
		myFileExists= true;
	}
	if(myFileExists == true)
	{
		ACE_UINT64 fileSize = (ACE_UINT64)ACS_LM_Common::getFileSize(curPath);
		ACE_HANDLE handle = ACE_OS::open(curPath.c_str(),O_RDONLY );
		if(handle != ACE_INVALID_HANDLE)
		{

			INFO("ACS_LM_Persistent::load() Open of curPath= %s",curPath.c_str());  
			ACE_UINT64 bytesRead = 0;

			ACE_TCHAR * buffer = new ACE_TCHAR[fileSize];
			bytesRead = ACE_OS::read(handle,(void*)buffer,fileSize);

			if((bytesRead != 0) && (bytesRead == fileSize) && !forceLoad)
			{
				//Load from file data
				ACE_UINT16 calcCheckSum = ACS_LM_Common::getMemoryCheckSum(buffer, fileSize-2);

				pFile = new ACS_LM_Persistent();
				pFile->currentPath = curPath;
				pFile->backupPath1 = bkpPath1;
				pFile->backupPath2 = bkpPath2;
				try
				{
					ACE_UINT64 pos = 0;
					INFO("ACS_LM_Persistent::load() virginMode is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->virginMode = ACS_LM_Common::decode8(buffer, pos);
					INFO("ACS_LM_Persistent::load() aNodeInitDone is going to be stored in buffer during decode in = %d positionand fileSize = %d",pos,fileSize);
					pFile->aNodeInitDone = ACS_LM_Common::decode8(buffer, pos);
					INFO("ACS_LM_Persistent::load() bNodeInitDone is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->bNodeInitDone = ACS_LM_Common::decode8(buffer, pos);
					INFO("ACS_LM_Persistent::load() lmMode is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->lmMode = ACS_LM_Common::decode16(buffer, pos);
					INFO("ACS_LM_Persistent::load() emergencyCounter is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->emergencyCounter = ACS_LM_Common::decode16(buffer, pos);
					INFO("ACS_LM_Persistent::load() lkfChecksum is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->lkfChecksum = ACS_LM_Common::decodechecksum16(buffer, pos);
					INFO("ACS_LM_Persistent::load lkfChecksum in load: %d",pFile->lkfChecksum);
					INFO("ACS_LM_Persistent::load() emergencyModeTime is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->emergencyModeTime = ACS_LM_Common::decode64(buffer, pos);
					/* LM Maintenance Mode */
					/* Backward compatibility: From Release 1 to Release 2 */
					if( (fileSize == PERSIST_FILE_SIZE_MAINTENANCEMODE) || (fileSize == PERSIST_FILE_SIZE_EXPORTLKF) )
					{
                                        	DEBUG("ACS_LM_Persistent::load() maintenanceModeTime is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
                                        	pFile->maintenanceModeTime = ACS_LM_Common::decode64(buffer, pos);
						//Assign Maintenance mode end time during load
						maintenanceEndTime = pFile->maintenanceModeTime; 

						/* Converting Maintenance mode expiry date into "YYYY-MM-DD" format to get displayed in log file */
						pFile->displayExpiryDate(pFile->maintenanceModeTime);
					}
					/* Export file - House keeping */
					/* Backward compatibility: From CM003 to CM004 */
					if( fileSize == PERSIST_FILE_SIZE_EXPORTLKF )
					{
						DEBUG("ACS_LM_Persistent::load() exportFileExpiryTime is going to be stored in buffer during decode in= %d position and fileSize = %d",pos,fileSize);
                                        	pFile->exportFileExpiryTime = ACS_LM_Common::decode64(buffer, pos);

						/* Converting exported file expiry date into "YYYY-MM-DD" format to get displayed in log file */
						pFile->displayExpiryDate(pFile->exportFileExpiryTime);
					}
					INFO("ACS_LM_Persistent::load() testLkf is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->testLkf = ACS_LM_TestLkf::load(pFile, buffer, pos);
					INFO("ACS_LM_Persistent::load() persistChecksum is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
					pFile->persistChecksum = ACS_LM_Common::decode16(buffer, pos);
					INFO("ACS_LM_Persistent::load() persistChecksum: %d,calcCheckSum :%d",pFile->persistChecksum,calcCheckSum);
					INFO("ACS_LM_Persistent::load() pos = %d and fileSize = %d",pos,fileSize);

					INFO("ACS_LM_Persistent::load() ACS_LM_Server::isOldDirectoryDeleted=%d,LoadedBkpFile=%d", ACS_LM_Server::isOldDirectoryDeleted,LoadedBkpFile);
					if(!(ACS_LM_Server::isOldDirectoryDeleted == true))
					{
						if(pFile->persistChecksum != calcCheckSum)
						{
							if(!LoadedBkpFile)
							{
						  		ACS_LM_Server::pp_persistChecksum = pFile->persistChecksum;
								ACS_LM_Server::pp_calcChecksum = calcCheckSum;
								INFO("ACS_LM_Persistent::load() primary path: pp_persistChecksum: %d,pp_calcChecksum:%d",ACS_LM_Server::pp_persistChecksum,ACS_LM_Server::pp_calcChecksum);
							}
							else
							{
						  		ACS_LM_Server::sp_persistChecksum = pFile->persistChecksum;
								ACS_LM_Server::sp_calcChecksum = calcCheckSum;
								INFO("ACS_LM_Persistent::load() Secondary path: sp_persistChecksum: %d,sp_calcChecksum:%d",ACS_LM_Server::sp_persistChecksum,ACS_LM_Server::sp_calcChecksum);
							}

							if ( !((ACS_LM_Server::pp_calcChecksum == ACS_LM_Server::sp_calcChecksum) && (ACS_LM_Server::pp_persistChecksum == ACS_LM_Server::sp_persistChecksum)))
							{
								INFO("ACS_LM_Persistent::load() %s ","pFile->persistChecksum and calcCheckSum are not Equal");
								delete pFile;
								pFile = NULL;
							}
						}
					}
				}
				catch(...)
				{
					delete pFile;
					pFile = NULL;
				}
			}

			delete[] buffer;
			ACE_OS::close(handle);
		}
	}

	if((pFile == NULL) && forceLoad) 
	{
		//CNI30_4_1719(TR NO HN66027/HN40528)
		ACE_UINT64 fileSize = (ACE_UINT64)ACS_LM_Common::getFileSize(bkpPath1);
		ACE_HANDLE handle = ACE_OS::open(bkpPath1.c_str(),O_RDONLY );
		if(handle != ACE_INVALID_HANDLE)
		{

			INFO("ACS_LM_Persistent::load() Open of backupPath= %s ",bkpPath1.c_str()); 
			ACE_UINT64 bytesRead = 0;

			ACE_TCHAR * buffer = new ACE_TCHAR[fileSize];
			bytesRead = ACE_OS::read(handle,(void*)buffer,fileSize);

			if((bytesRead != 0) && (bytesRead == fileSize) && !forceLoad)
			{
				//Load from file data
				ACE_UINT16 calcCheckSum = ACS_LM_Common::getMemoryCheckSum(buffer, fileSize-2);

				pFile = new ACS_LM_Persistent();
				pFile->currentPath = curPath;
				pFile->backupPath1 = bkpPath1;
				pFile->backupPath2 = bkpPath2;
				try
				{
					ACE_UINT64 pos = 0;
					pFile->virginMode = ACS_LM_Common::decode8(buffer, pos);

					pFile->aNodeInitDone = ACS_LM_Common::decode8(buffer, pos);
					pFile->bNodeInitDone = ACS_LM_Common::decode8(buffer, pos);
					pFile->lmMode = ACS_LM_Common::decode16(buffer, pos);
					pFile->emergencyCounter = ACS_LM_Common::decode16(buffer, pos);
					pFile->lkfChecksum = ACS_LM_Common::decodechecksum16(buffer, pos);
					INFO("ACS_LM_Persistent::load from Backupfile, lkfChecksum: %d",pFile->lkfChecksum);
					pFile->emergencyModeTime = ACS_LM_Common::decode64(buffer, pos);
					/* LM Maintenance Mode */
					/* Backward compatibility: From Release 1 to Release 2 */	
					if( (fileSize == PERSIST_FILE_SIZE_MAINTENANCEMODE) || (fileSize == PERSIST_FILE_SIZE_EXPORTLKF) )
                                        {
						pFile->maintenanceModeTime = ACS_LM_Common::decode64(buffer, pos);
					}
					/* Export file - House keeping */
					/* Backward compatibility: From CM003 to CM004 */
                                        if( fileSize == PERSIST_FILE_SIZE_EXPORTLKF )
                                        {
                                                DEBUG("ACS_LM_Persistent::load()exportFileExpiryTime is going to be stored in buffer during decode in = %d position and fileSize = %d",pos,fileSize);
                                                pFile->exportFileExpiryTime = ACS_LM_Common::decode64(buffer, pos);

					}
					pFile->testLkf = ACS_LM_TestLkf::load(pFile, buffer, pos);
					pFile->persistChecksum = ACS_LM_Common::decode16(buffer, pos);
					INFO("ACS_LM_Persistent::load() from backup file, persistChecksum: %d,calcCheckSum :%d",pFile->persistChecksum,calcCheckSum);
					if(pFile->persistChecksum != calcCheckSum)
					{
						INFO("ACS_LM_Persistent::load() %s ","pFile->persistChecksum and calcCheckSum are not Equal");
						delete pFile;
						pFile = NULL;
					}
				}
				catch(...)
				{
					delete pFile;
					pFile = NULL;
				}
			}

			delete[] buffer;
			ACE_OS::close(handle);
		}
		if((pFile == NULL) && forceLoad)
		{
		DEBUG("ACS_LM_Persistent::load() %s ","new persistent file");
		pFile = new ACS_LM_Persistent();
		pFile->currentPath = curPath;
		pFile->backupPath1 = bkpPath1;
		pFile->backupPath2 = bkpPath2;
		//pFile->virginMode = true;
		//CNI30_4_1719(TR NO HN66027/HN40528)
		if(pFile->isLKFExists())  // fix for lm Mode VIRGIN MODE problem
		{
			pFile->virginMode = false;
		}
		else
		{
			pFile->virginMode = true;  
			 DEBUG("reloadOrigPersistentData()..setting virginmode to TRUE..%d",__LINE__);
		}
		INFO("ACS_LM_Persistent::load reloadOrigPersistentData(): Virgin mode: %d",pFile->virginMode);
		pFile->aNodeInitDone = false;
		pFile->bNodeInitDone = false;
		pFile->lmMode = 0;
		pFile->emergencyCounter = 0;
		pFile->lkfChecksum = 0;
		pFile->emergencyModeTime = 0;
		pFile->maintenanceModeTime = 0; /* LM Maintenance Mode */ 
		pFile->exportFileExpiryTime =0; /* Export file - House keeping */
		pFile->persistChecksum = 0;
		pFile->testLkf = ACS_LM_TestLkf::load(pFile);	
		pFile->setMaintenanceUnlockToLock(); 
		}
	}
	DEBUG("ACS_LM_Persistent::load() %s ","Leaving");
	return pFile;
}//end of load
/*=================================================================
	ROUTINE: commit
=================================================================== */
bool ACS_LM_Persistent::commit()
{
	DEBUG("ACS_LM_Persistent::commit() %s ","Entering");
	ACE_INT64 orgFileSize = (ACE_INT64)ACS_LM_Common::getFileSize(currentPath);
	ACE_TCHAR* orgBuffer = new ACE_TCHAR[orgFileSize];
	//CNI30_4_1719(TR NO HN66027/HN40528)

	if(!readOrigPersistentData(currentPath, orgBuffer, orgFileSize ))
	{
		DEBUG("ACS_LM_Persistent::commit() %s ","readOrigPersistentData from backup");
		readOrigPersistentData(backupPath1, orgBuffer, orgFileSize );
		DEBUG("ACS_LM_Persistent::commit() orgFileSize=%d", orgFileSize); 
	}

	ACE_UINT64 pos = 0;
	ACE_UINT64 bufSize = PERSIST_FIXED_RECORD_LEN;
	bufSize += testLkf->sizeInBytes();
 	/* To avoid switching to Grace mode from License Controlled mode
           during UP Installation, adding only 2 bytes for checksum instead of 4 */
	bufSize += 2;	//for checksum
	ACE_TCHAR* buffer = new ACE_TCHAR[bufSize];
	ACE_OS::memset(buffer, 0, bufSize);

	INFO("ACS_LM_Persistent::commit() virginMode is going to be stored in buffers in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encode8(buffer, pos, virginMode);
	INFO("ACS_LM_Persistent::commit() aNodeInitDone is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encode8(buffer, pos, aNodeInitDone);
	INFO("ACS_LM_Persistent::commit() bNodeInitDone is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encode8(buffer, pos, bNodeInitDone);
	INFO("ACS_LM_Persistent::commit() lmMode is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encode16(buffer, pos, lmMode);
	INFO("ACS_LM_Persistent::commit() emergencyCounter is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encode16(buffer, pos, emergencyCounter);
	INFO("ACS_LM_Persistent::commit() ACS_LM_Persistent::commit() Before Encode lkfChecksum = %d ",lkfChecksum);
	std::string curLkfPath(LM_LKF_PRIMARY_PATH);
	lkfChecksum = ACS_LM_Common::getFileChecksum(curLkfPath, 0, 0);
	INFO("ACS_LM_Persistent::commit() Before Encode lkfChecksum = %d ",lkfChecksum);
	INFO("ACS_LM_Persistent::commit() lkfChecksum is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encodechecksum16(buffer, pos, lkfChecksum);
	//bufSize += 1; ////for lkfchecksum
	INFO("ACS_LM_Persistent::commit() emergencyModeTime is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encode64(buffer, pos, emergencyModeTime);
	/* LM Maintenance Mode */
	DEBUG("ACS_LM_Persistent::commit() maintenanceModeTime is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	ACS_LM_Common::encode64(buffer, pos, maintenanceModeTime);
	/* Export file - House keeping */
	DEBUG("ACS_LM_Persistent::commit() exportFileExpiryTime is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
        ACS_LM_Common::encode64(buffer, pos, exportFileExpiryTime);

	INFO("ACS_LM_Persistent::commit() testLkf is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	testLkf->store(buffer, pos);
	ACE_UINT16 persistChecksum = ACS_LM_Common::getMemoryCheckSum(buffer, bufSize-2);
	INFO("ACS_LM_Persistent::commit() Before Encode persistChecksum = %d ",persistChecksum);
	INFO("ACS_LM_Persistent::commit() persistChecksum is going to be stored in buffer in = %d position and bufSize =%d",pos,bufSize);
	this->persistChecksum = persistChecksum;   //TR_HV60364
	ACS_LM_Common::encode16(buffer, pos, persistChecksum);
	INFO("ACS_LM_Persistent::commit() pos = %d ,bufSize = %d",pos,bufSize);
	bool commited = false;
	ACE_HANDLE handle;

	for(int i=0; i<3; i++)
	{
		DEBUG("ACS_LM_Persistent::commit() %d currentpathstring = %s",__LINE__,currentPath.c_str());
		handle = ACE_OS::open(currentPath.c_str(),O_WRONLY|O_CREAT|O_TRUNC);
		if(handle != ACE_INVALID_HANDLE)
		{
			break;
		}
		else
		{
			INFO("ACS_LM_Persistent::commit() ACE_OS::open(currentPath.c_str = %s) Failed ",currentPath.c_str());
			ACE_OS::sleep(1);
		}
	}

	if(handle != ACE_INVALID_HANDLE)
	{
		ACE_UINT64 bytesWritten = 0;
		bytesWritten = ACE_OS::write(handle,buffer,bufSize);
		DEBUG("ACS_LM_Persistent::commit() After written into current lmdata file bytesWritten=%d", bytesWritten);
		ACE_OS::close(handle);
		if(bytesWritten <= 0)
		{
			ERROR("ACS_LM_Persistent::commit()  %s ","ACE_OS::write() failed");
		}
		else
		{
			if(backup())
			{
				commited = true;
			}
			else
			{     
				// Replace   prior to commit lmdata
				writeOrigPersistentData(currentPath, orgBuffer, orgFileSize);
				ERROR("ACS_LM_Persistent::commit()  %s ","backup() failed");
			}
		}			
	}
	else
	{
		DEBUG("ACS_LM_Persistent::commit()  %s ","Opening of file failed even after 10 times");
	}

	delete[] buffer;
	delete[] orgBuffer;
	//ACS_LM_Persistent::writeMutex.release();
	if(commited)
	{
		DEBUG("ACS_LM_Persistent::commit() %s ","ACS_LM_Common::setLmdataLockStatus(false)");
		ACS_LM_Common::setLmdataLockStatus(false);
	}
	else
	{
		DEBUG("ACS_LM_Persistent::commit() %s ","ACS_LM_Common::setLmdataLockStatus(true)");
		ACS_LM_Common::setLmdataLockStatus(true);
	}

	DEBUG("ACS_LM_Persistent::commit() %s ","Leaving");
	return commited;
}//end of commit
/*=================================================================
	ROUTINE: backup
=================================================================== */
bool ACS_LM_Persistent::backup()
{
	DEBUG("ACS_LM_Persistent::backup()  %s ","Entering");
	bool backup=true;
	//TR HP46656
	if(ACS_LM_Common::isFileExists(backupPath1))
	{
		if(ACS_LM_Common::deleteFile(backupPath1))
		{
			//INFO("ACS_LM_Sentinel::installLkf() %s","LKF file removed from the BackupPath");
			INFO("ACS_LM_Persistent::backup() %s","Lmdata removed from the BackupPath");
		}
		else
		{
			INFO("ACS_LM_Persistent::backup() %s","ERROR!!! Lmdata removed from the BackupPath");
		}
	}
	//END OF TR HP46656
	for (int attempt=0;attempt<5;attempt++)
	{
		bool myResult = ACS_LM_Common::backupFile(currentPath, backupPath1);
		if(myResult == false)
		{
			backup = false;
			ERROR("ACS_LM_Persistent::backup() ACS_LM_Common::backupFile(currentPath = %s , backupPath1 = %s ) Failed",currentPath.c_str(),backupPath1.c_str());
		}
		else
		{
			DEBUG("ACS_LM_Persistent::backup() ACS_LM_Common::backupFile(currentPath = %s , backupPath1 = %s ) Passed",currentPath.c_str(),backupPath1.c_str());
			break;
		}
	}
	DEBUG("ACS_LM_Persistent::backup()  %s ","Leaving");
	return backup;
}//end of backup
/*=================================================================
	ROUTINE: restore
=================================================================== */
bool ACS_LM_Persistent::restore()
{
	DEBUG("ACS_LM_Persistent::restore()  %s ","Entering");
	bool restore = true;

	bool ec = ACS_LM_Common::restoreFile(backupPath1, currentPath);
	if(ec != true)
	{
		ERROR("ACS_LM_Persistent::restore() ACS_LM_Common::restoreFile(backupPath1 = %s , currentPath = %s ) Failed",backupPath1.c_str(),currentPath.c_str());
		restore = false;
	}
	DEBUG("ACS_LM_Persistent::restore()  %s ","Leaving");
	return restore;
}//end of restore
/*=================================================================
	ROUTINE: updateInitializedNodeStatus
=================================================================== */
void ACS_LM_Persistent::updateInitializedNodeStatus(bool& aNodeInitialized,bool& bNodeInitialized)
{
	DEBUG("ACS_LM_Persistent::updateInitializedNodeStatus()  %s ","Entering");
	aNodeInitialized = bNodeInitialized =false;
	if(this->aNodeInitDone != 0)
	{
		aNodeInitialized = true;
	}
	if(this->bNodeInitDone != 0)
	{
		bNodeInitialized = true;
	}
	DEBUG("ACS_LM_Persistent::updateInitializedNodeStatus()  %s ","Leaving");
}//end of updateInitializedNodeStatus
/*=================================================================
	ROUTINE: setInitializedNodeStatus
=================================================================== */
void ACS_LM_Persistent::setInitializedNodeStatus(bool aNodeInitialized,bool bNodeInitialized)
{
	DEBUG("ACS_LM_Persistent::setInitializedNodeStatus()  %s ","Entering");
	this->aNodeInitDone = aNodeInitialized;
	this->bNodeInitDone = bNodeInitialized;
	DEBUG("ACS_LM_Persistent::setInitializedNodeStatus()  %s ","Leaving");
}//end of setInitializedNodeStatus
/*=================================================================
	ROUTINE: getInitializedNodeStatus
=================================================================== */
void ACS_LM_Persistent::getInitializedNodeStatus(bool &aNodeInitialized, bool &bNodeInitialized)
{
	DEBUG("ACS_LM_Persistent::getInitializedNodeStatus()  %s ","Entering");
	aNodeInitialized =(bool) this->aNodeInitDone;
	bNodeInitialized = (bool) this->bNodeInitDone;
	DEBUG("ACS_LM_Persistent::getInitializedNodeStatus()  %s ","Leaving");

}//end of getInitializedNodeStatus
/*=================================================================
	ROUTINE: deletePersistentFile
=================================================================== */
void ACS_LM_Persistent::deletePersistentFile()
{
	ACE_OS::unlink(currentPath.c_str());
}//end of deletePersistentFile
/*=================================================================
	ROUTINE: reLoad
=================================================================== */

//To save the status of modes prior to persistent commit so that
void ACS_LM_Persistent::reLoad(ACE_INT16 emergencyCounter, ACE_UINT32 lkfChecksum, ACE_INT16 mode,

		bool isVirginMode, bool aNodeInitialized, bool bNodeInitialized)
{
	DEBUG("ACS_LM_Persistent::reLoad()  %s ","Entering");
	this->emergencyCounter = emergencyCounter;
	INFO("reload lkfChecksum = %d",lkfChecksum);
	this->lkfChecksum = lkfChecksum;
	if((mode & ACS_LM_MODE_EMERGENCY) == ACS_LM_MODE_EMERGENCY)
	{
		DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_EMERGENCY, true)");
		setLmMode(ACS_LM_MODE_EMERGENCY, true);
	}
	else
	{
		DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_EMERGENCY, false)");
		setLmMode(ACS_LM_MODE_EMERGENCY, false);
	}
	/* LM Maintenance Mode */
 	if((mode & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE)
        {
                DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_MAINTENANCE, true)");
                setLmMode(ACS_LM_MODE_MAINTENANCE, true);
        }
        else
        {
                DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_MAINTENANCE, false)");
                setLmMode(ACS_LM_MODE_MAINTENANCE, false);
        }
	if((mode & ACS_LM_MODE_GRACE) == ACS_LM_MODE_GRACE)
	{
		DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_GRACE, true)");
		setLmMode(ACS_LM_MODE_GRACE, true);
	}
	else
	{
		DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_GRACE, false)");
		setLmMode(ACS_LM_MODE_GRACE, false);
	}
	if((mode & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST)
	{
		DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_TEST, true)");
		setLmMode(ACS_LM_MODE_TEST, true);
	}
	else
	{
		DEBUG("ACS_LM_Persistent::reLoad()  %s ","setLmMode(ACS_LM_MODE_TEST, false)");
		setLmMode(ACS_LM_MODE_TEST, false);
	}
	this->virginMode = isVirginMode;
	this->aNodeInitDone = aNodeInitialized;
	this->bNodeInitDone = bNodeInitialized;
	DEBUG("ACS_LM_Persistent::reLoad()  %s ","Leaving");

}//end of reLoad
/*=================================================================
	ROUTINE: writeOrigPersistentData
=================================================================== */
void ACS_LM_Persistent::writeOrigPersistentData(std::string path, ACE_TCHAR * buffer, ACE_UINT64 size)
{
	DEBUG("ACS_LM_Persistent::writeOrigPersistentData()  %s ","Entering");
	INFO(" writeOrigPersistentData() path:%s,size:%d",path.c_str(),size);
	ACE_HANDLE handle ;
	for(ACE_UINT16 i=0; i<10; i++)
	{
		handle = ACE_OS::open(currentPath.c_str(),S_IWUSR);
		if(handle != ACE_INVALID_HANDLE)
		{

			break;
		}
		else
		{
			ERROR("ACS_LM_Persistent::writeOrigPersistentData()   ACE_OS::open(currentPath = %s) Failed ",currentPath.c_str());
			ACE_OS::sleep(50);
		}
	}
	ACE_INT64 bytesWritten = 0;
	bytesWritten = ACE_OS::write(handle,reinterpret_cast<void*>(buffer),size);


	if(bytesWritten <= -1)
	{
		ERROR("writeOrigPersistentData(): ACE_OS::write() failed bytesWritten:%d,size:%d ",bytesWritten,size);
	}
	ACE_OS::close(handle);
	DEBUG("ACS_LM_Persistent::writeOrigPersistentData()   %s ","Leaving");
}//end of writeOrigPersistentData
/*=================================================================
	ROUTINE: readOrigPersistentData
=================================================================== */
bool ACS_LM_Persistent::readOrigPersistentData(std::string path, ACE_TCHAR * buffer, ACE_UINT64 size)
{
	bool read = false;
	DEBUG("ACS_LM_Persistent::readOrigPersistentData()   %s ","Entering");
	INFO(" readOrigPersistentData() path:%s,size:%d",path.c_str(),size);
	ACE_HANDLE handle = ACE_OS::open(path.c_str(),S_IRUSR,S_IROTH);
	if(handle != ACE_INVALID_HANDLE)
	{
		ACE_INT32 bytesRead = 0;
		bytesRead = ACE_OS::read(handle,(void*)buffer,size);
		if(bytesRead <= -1)
		{
			read=true;
			ERROR("ACS_LM_Persistent::readOrigPersistentData() %s","readOrigPersistentData failed");
		}
		INFO("readOrigPersistentData(): bytesRead : %d",bytesRead);
	ACE_OS::close(handle);
	}
	//ACE_OS::close(handle);
	DEBUG("ACS_LM_Persistent::readOrigPersistentData()   %s ","Leaving");
	return read;
}//end of readOrigPersistentData
/*=================================================================
	ROUTINE: reloadOrigPersistentData
=================================================================== */
void ACS_LM_Persistent::reloadOrigPersistentData()
{
	DEBUG("ACS_LM_Persistent::reloadOrigPersistentData()   %s ","Entering");  
	bool defaultSetting = false;
	//if(ACS_LM_Common::getDataDiskPath("ACS_DATA",currentPath) == false)
	std::string currentPath(LM_PERSISTENT_PRIMARY_PATH);

	ACE_HANDLE handle = ACE_OS::open(currentPath.c_str(),S_IRUSR,S_IROTH);
	if(handle != ACE_INVALID_HANDLE)
	{
		ACE_UINT64 bytesRead = 0;
		ACE_UINT64 fileSize = (ACE_UINT64)ACS_LM_Common::getFileSize(currentPath);
		ACE_TCHAR * buffer = new ACE_TCHAR[fileSize];
		bytesRead = ACE_OS::read(handle,(void*)buffer,fileSize);
		if(bytesRead != 0 && bytesRead == fileSize )
		{
			//Load from file data
			ACE_UINT16 calcCheckSum = ACS_LM_Common::getMemoryCheckSum(buffer, fileSize-2);
			try
			{
				ACE_UINT64 pos = 0;
				this->virginMode = ACS_LM_Common::decode8(buffer, pos);

				this->aNodeInitDone = ACS_LM_Common::decode8(buffer, pos);
				this->bNodeInitDone = ACS_LM_Common::decode8(buffer, pos);
				this->lmMode = ACS_LM_Common::decode16(buffer, pos);
				this->emergencyCounter = ACS_LM_Common::decode16(buffer, pos);
				this->lkfChecksum = ACS_LM_Common::decodechecksum16(buffer, pos);
				this->emergencyModeTime = ACS_LM_Common::decode64(buffer, pos);
				/* LM Maintenance Mode */
				/* Backward compatibility: From Release 1 to Release 2 */	
				if( (fileSize == PERSIST_FILE_SIZE_MAINTENANCEMODE) || (fileSize == PERSIST_FILE_SIZE_EXPORTLKF) )
                                {
					this->maintenanceModeTime = ACS_LM_Common::decode64(buffer, pos); 
				}
				/* Export file - House keeping */
				/* Backward compatibility: From CM003 to CM004 */
				if( fileSize == PERSIST_FILE_SIZE_EXPORTLKF )
                                {
                                	DEBUG("ACS_LM_Persistent::reloadOrigPersistentData() exportFileExpiryTime is going to be stored in buffer during decode in = %d position and fileSize = %d", pos,fileSize);
                                	this->exportFileExpiryTime = ACS_LM_Common::decode64(buffer, pos);
                                }
				this->testLkf->reload( buffer, pos);

				persistChecksum = ACS_LM_Common::decode16(buffer, pos);
				INFO("ACS_LM_Persistent::reloadOrigPersistentData(): persistChecksum: %d,calcCheckSum :%d",persistChecksum,calcCheckSum);
				if(persistChecksum != calcCheckSum)
				{
					defaultSetting = true;
				}
				else
				{
					ACS_LM_Common::setLmdataLockStatus(false);
				}
			}
			catch(...)
			{
				WARNING("ACS_LM_Persistent::readOrigPersistentData()   %s ","Exception is caught");
			}
		}

		delete[] buffer;
		ACE_OS::close(handle);
	}
	if (defaultSetting)
	{
		//this->virginMode = true;
		if(this->isLKFExists())  // VIRGIN MODE problem
		{
			this->virginMode = false;
		}
		else
		{
			this->virginMode = true; 
			DEBUG("reloadOrigPersistentData()..setting virginmode to TRUE..%d",__LINE__);
		}
		this->aNodeInitDone = false;
		this->bNodeInitDone = false;
		this->lmMode = 0;
		this->emergencyCounter = 0;
		this->lkfChecksum = 0;
		this->emergencyModeTime = 0;
		this->maintenanceModeTime = 0; /* LM Maintenance Mode */
		this->exportFileExpiryTime =0; /* Export file - House keeping */
		this->persistChecksum = 0;
		this->setMaintenanceUnlockToLock(); 
	}
	DEBUG("ACS_LM_Persistent::reloadOrigPersistentData()   %s ","Leaving"); 
}//end of reloadOrigPersistentData

//Set the 'maintenanceUnlock' attribute to 'LOCKED'.
void ACS_LM_Persistent::setMaintenanceUnlockToLock()
{ 
	OmHandler myOmHandler;
        if (myOmHandler.Init() == ACS_CC_FAILURE)
        {
        	ERROR("ACS_LM_Persistent::setMaintenanceUnlockToLock() %s","OmHandler initalization is failed");
        }
        else
        { 
        	ACS_CC_ImmParameter myParamToChange ;
        	myParamToChange.attrName = (char*) MAINTENANCE_UNLOCK ;
        	myParamToChange.attrType = ATTR_INT32T;
        	myParamToChange.attrValuesNum = 1;
        	ACE_INT32 myVal = 0;
        	void * myValues[1] = { reinterpret_cast<void*>(const_cast<int*>(&myVal)) };
        	myParamToChange.attrValues = myValues;

        	std::string licenseDN = string(ACS_IMM_LM_ROOT_OBJ_DN);

        	DEBUG("ACS_LM_Persistent::setMaintenanceUnlockToLock() The LM RDN is %s", licenseDN.c_str());
        	if (myOmHandler.modifyAttribute(licenseDN.c_str(), &myParamToChange ) == ACS_CC_FAILURE )
        	{
        		ERROR("ACS_LM_Persistent::setMaintenanceUnlockToLock() %s", "'MaintenanceUnlock' attribute updation is failed");
        		int intErr = myOmHandler.getInternalLastError();
        		DEBUG("ACS_LM_Persistent::setMaintenanceUnlockToLock()..Error code=%d...Internal Last error text : %s ",intErr,myOmHandler.getInternalLastErrorText());
        	}
        	else
        	{
        		INFO("ACS_LM_Persistent::setMaintenanceUnlockToLock() %s", " maintenanceUnlock attribute successfully updated during Auto cease...");
       		}	

        	if(myOmHandler.Finalize() == ACS_CC_FAILURE)
        	{
       			ERROR("ACS_LM_Persistent::setMaintenanceUnlockToLock() %s","OmHandler finalize failed");
        	}
        }
}
/* Export file - House keeping */
/* exported file to be deleted after 30 days from it's exported date. 
   So adding 30 days to the current date			  */
void  ACS_LM_Persistent::calculateExportFileExpiryDate()
{
	DEBUG("ACS_LM_Persistent::calculateExportFileExpiryDate() %s\n","setting Offset time for exported file");
	time_t curTime = ::time(NULL);
	struct tm curTimeLocal = *localtime(&curTime);
	curTimeLocal.tm_hour = 0;
	curTimeLocal.tm_min = 0;
	curTimeLocal.tm_sec = 0;
	curTime = ::mktime(&curTimeLocal);
	ACE_INT64 exportFileExpiryDays = (EXPORTFILE_HOUSEKEEPING_EXPIRY_DAYS*24*60*60); 
	this->exportFileExpiryTime = curTime + exportFileExpiryDays;

	/* Converting exported file expiry date into "YYYY-MM-DD" format to get displayed in log file */
	std::string myEndDateforExportFile("");
	time_t exportEndDate = this->exportFileExpiryTime;
	struct tm exportEndTimeLocal = *localtime(&exportEndDate);
	std::ostringstream myOstr;
	myOstr<<std::setw(4)<<std::setfill('0')<<(exportEndTimeLocal.tm_year+1900)<<"-";
	myOstr<<std::setw(2)<<std::setfill('0')<<(exportEndTimeLocal.tm_mon+1)<<"-";
	myOstr<<std::setw(2)<<std::setfill('0')<<(exportEndTimeLocal.tm_mday);
	myEndDateforExportFile = myOstr.str();

	if( !myEndDateforExportFile.empty())
        {
                std::replace(myEndDateforExportFile.begin(),myEndDateforExportFile.end(),'/','-');
        }
	DEBUG("%s\n", "===============================================================================");
	DEBUG("ACS_LM_Persistent::calculateExportFileExpiryDate() end date for exportedfile =%s\n",myEndDateforExportFile.c_str());
	DEBUG("%s\n"," ===============================================================================");
}//end of calculateExportFileExpiryDate

/*=================================================================
        ROUTINE: isExportLKFExpired 
=================================================================== */
bool ACS_LM_Persistent::isExportLKFExpired()
{
        INFO("ACS_LM_Persistent::isExportLKFExpired() %s","Entering");
        bool expired = true;
        time_t curTime = ACE_OS::time(NULL);
        if((exportFileExpiryTime!=0) && (curTime > exportFileExpiryTime))
        {
                DEBUG("ACS_LM_Persistent::isExportLKFExpired() %s","export file expired. Setting exportFileExpiryTime to '0'");
		exportFileExpiryTime = 0;
        }
        else
        {
                expired = false;
        }
        INFO("ACS_LM_Persistent::isExportLKFExpired() %s","Leaving");
        return expired;
}//end of isExportLKFExpired 

/*=================================================================
        ROUTINE: isExportLKFExpired 
=================================================================== */
bool ACS_LM_Persistent::isModeExpired( time_t modeExpiryTime)
{
        INFO("ACS_LM_Persistent::isModeExpired() %s","Entering");
	bool isModeExpiryDayArrived = false;
	//Converting the modeExpiryTime which is in seconds format to tm structure format.  
        struct tm modeEndTimeLocal = *localtime(&modeExpiryTime);
        int modeExpiryYear = modeEndTimeLocal.tm_year+1900;
        int modeExpiryMonth = modeEndTimeLocal.tm_mon+1;
        int modeExpiryDay = modeEndTimeLocal.tm_mday;
        //Get the local time and converting it into tm structure format.
        time_t currentTime = ::time(NULL);
        struct tm currentTimeLocal = *localtime(&currentTime);
        int currentTimeYear = currentTimeLocal.tm_year+1900;
        int currentTimeMonth = currentTimeLocal.tm_mon+1;
        int currentTimeDay = currentTimeLocal.tm_mday;

        INFO("ACS_LM_Persistent::isModeExpired() Mode expiry Year=%d, Mon=%d, Day=%d", modeExpiryYear, modeExpiryMonth, modeExpiryDay);
        INFO("ACS_LM_Persistent::isModeExpired() Current time Year=%d, Mon=%d, Day=%d", currentTimeYear, currentTimeMonth, currentTimeDay);

        if((currentTimeYear >= modeExpiryYear) && (currentTimeMonth >= modeExpiryMonth) && ( currentTimeDay >= modeExpiryDay))
        {
                isModeExpiryDayArrived = true;
        }

	INFO("ACS_LM_Persistent::isModeExpired() isModeExpiryDayArrived=%d %s",isModeExpiryDayArrived, "Leaving");
	return isModeExpiryDayArrived;
}

/*=================================================================
        ROUTINE: isLKFExists
=================================================================== */
bool ACS_LM_Persistent::isLKFExists()
{
	DEBUG("ACS_LM_Persistent::isLKFExists() %s ","Entering");
	bool isLKFPresent = false;

        std::string lkfPrimaryPath(LM_LKF_PRIMARY_PATH);
        std::string lkfBackupPath(LM_LKF_BACKUP_PATH);

	if((ACS_LM_Common::isFileExists(lkfPrimaryPath)) || (ACS_LM_Common::isFileExists(lkfBackupPath)))
	{
		isLKFPresent = true;
	}

	DEBUG("ACS_LM_Persistent::isLKFExists() %s ","Leaving");
	return isLKFPresent;
}

/*=================================================================
        ROUTINE: displayExpiryDate 
=================================================================== */
void ACS_LM_Persistent::displayExpiryDate(ACE_INT64 endDate_Input)
{
	DEBUG("ACS_LM_Persistent::displayExpiryDate() %s","Entering");
	std::string myEndDate("");
	time_t modeEndDate = (time_t) endDate_Input;
	struct tm modeEndTimeLocal = *localtime(&modeEndDate);
	std::ostringstream myOstr;

	myOstr<<std::setw(4)<<std::setfill('0')<<(modeEndTimeLocal.tm_year+1900)<<"-";
	myOstr<<std::setw(2)<<std::setfill('0')<<(modeEndTimeLocal.tm_mon+1)<<"-";
	myOstr<<std::setw(2)<<std::setfill('0')<<(modeEndTimeLocal.tm_mday);
	myEndDate = myOstr.str();

	if( !myEndDate.empty())
	{
		std::replace(myEndDate.begin(),myEndDate.end(),'/','-');
	}
	DEBUG("%s\n", "===============================================================================");
	DEBUG("ACS_LM_Persistent::displayExpiryDate() end date =%s\n",myEndDate.c_str());
	DEBUG("%s\n"," ===============================================================================");
	DEBUG("ACS_LM_Persistent::displayExpiryDate() %s","Leaving");
}
