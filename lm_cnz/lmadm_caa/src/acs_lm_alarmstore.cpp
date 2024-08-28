//******************************************************************************
//
//  NAME
//     acs_lm_alarmstore.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
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
#include <acs_lm_alarmstore.h>
//#include "acs_lm_tracemacro.h"

#include "acs_lm_tra.h"
/*=================================================================
	ROUTINE: ACS_LM_AlarmStore constructor
=================================================================== */
ACS_LM_AlarmStore::ACS_LM_AlarmStore()
:alarmFile(""),
emergencyAlarmState(false),
maintenanceAlarmState(false),  /* LM Maintenance Mode */ 
lkfMissingAlarmState(false)
{	
	INFO("%s","ACS_LM_AlarmStore::ACS_LM_AlarmStore()-Entering ");
	if(!ACS_LM_Common::createLmDir(ALARM_HOME))
	{
		DEBUG("ACS_LM_AlarmStore() createLmDir:%s Failed",ALARM_HOME);
	}
	INFO("%s","ACS_LM_AlarmStore::ACS_LM_AlarmStore()-Leaving ");
}//end of constructor
/*=================================================================
	ROUTINE: validate
=================================================================== */
void ACS_LM_AlarmStore::validate(const std::list<LkData*>& lkList)
{
	INFO("ACS_LM_AlarmStore::validate() %s","Entering");

	for(std::list<std::string>::iterator alIt2 = alarm2LkList.begin();
		alIt2 != alarm2LkList.end(); ++alIt2)
	{
		bool lkUsed = false;
		std::string lkId = (*alIt2);

		for(std::list<LkData*>::const_iterator lkIt=lkList.begin();
			lkIt!=lkList.end(); ++lkIt)
		{
            std::string lk = (*lkIt)->lkId;
			if(lk.compare(lkId) == 0)
			{
				lkUsed = true;
				INFO("ACS_LM_AlarmStore::validate() in A2 list lkused = %s", "true"); 
				break;
			}
		}

		if(!lkUsed)
		{
			alarm2LkList.erase(alIt2);
			alIt2 = alarm2LkList.begin();
		}
	}

	for(std::list<std::string>::iterator alIt3 = alarm3LkList.begin();
		alIt3 != alarm3LkList.end(); ++alIt3)
	{
		bool lkUsed = false;
		std::string lkId = (*alIt3);

		for(std::list<LkData*>::const_iterator lkIt=lkList.begin();
			lkIt!=lkList.end(); ++lkIt)
		{
            std::string lk = (*lkIt)->lkId;
			if(lk.compare(lkId) == 0)
			{
				lkUsed = true;
				INFO("ACS_LM_AlarmStore::validate() in A3 list lkused = %s", "true"); 
				break;
			}
		}

		if(!lkUsed)
		{
			alarm3LkList.erase(alIt3);
			alIt3 = alarm3LkList.begin();
		}
	}
	INFO("ACS_LM_AlarmStore::validate() %s","Leaving");
}//end of validate
/*=================================================================
	ROUTINE: ACS_LM_AlarmStore destructor
=================================================================== */
ACS_LM_AlarmStore::~ACS_LM_AlarmStore()
{	
	DEBUG("ACS_LM_AlarmStore::~ACS_LM_AlarmStore() %s","Entering");
	if(!commit())
	{
		INFO("%s","~ACS_LM_AlarmStore() commit() failed");
	}
	DEBUG("ACS_LM_AlarmStore::~ACS_LM_AlarmStore() %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: insertIntoA2List
=================================================================== */
bool ACS_LM_AlarmStore::insertIntoA2List(const std::string& lkId)
{
	DEBUG("ACS_LM_AlarmStore::insertIntoA2List() %s","Entering");

	INFO("insertIntoA2List() lkId = %s",lkId.c_str());
	bool inserted = false;
	bool lkFound = false;

	for(std::list<std::string>::iterator itr=alarm2LkList.begin();
		itr!=alarm2LkList.end(); ++itr)
	{
		if(lkId.compare((*itr)) == 0)
		{
			lkFound = true;
			DEBUG("ACS_LM_AlarmStore::insertIntoA2List() lkId=%s %s",lkId.c_str(),"is already present in A2 list");
			break;
		}
	}

	if(!lkFound)
	{
		alarm2LkList.push_back(lkId);			
		inserted = true;
		DEBUG("ACS_LM_AlarmStore::insertIntoA2List() lkId=%s %s",lkId.c_str(),"is inserted into A2 list");
	}
	DEBUG("ACS_LM_AlarmStore::insertIntoA2List() %s","Leaving");

	return inserted;
}//end of insertIntoA2List
/*=================================================================
	ROUTINE: insertIntoA3List
=================================================================== */
bool ACS_LM_AlarmStore::insertIntoA3List(const std::string& lkId)
{
	INFO("ACS_LM_AlarmStore::insertIntoA3List() %s","Entering");

	INFO("insertIntoA3List() lkId = %s",lkId.c_str());
	bool inserted = false;
	bool lkFound = false;

	for(std::list<std::string>::iterator itr=alarm3LkList.begin();
		itr!=alarm3LkList.end(); ++itr)
	{
		if(lkId.compare((*itr)) == 0)
		{
			lkFound = true;
			DEBUG("ACS_LM_AlarmStore::insertIntoA3List() lkId=%s, %s",lkId.c_str(),"is already present in A3 list");
			break;
		}
	}

	if(!lkFound)
	{
		alarm3LkList.push_back(lkId);			
		DEBUG("ACS_LM_AlarmStore::insertIntoA3List() lkId=%s, %s",lkId.c_str(), "is inserted into A3 list");
		inserted = true;
	}

	DEBUG("ACS_LM_AlarmStore::insertIntoA3List() %s","Leaving");
	return inserted;
}//end of insertIntoA3List
/*=================================================================
	ROUTINE: removeFromA2List
=================================================================== */
bool ACS_LM_AlarmStore::removeFromA2List(const std::string& lkId)
{
	DEBUG("ACS_LM_AlarmStore::removeFromA2List() %s","Entering");

	INFO("removeFromA2List() lkId = %s",lkId.c_str());

	bool removed = false;

	for(std::list<std::string>::iterator itr=alarm2LkList.begin();
		itr!=alarm2LkList.end(); ++itr)
	{	
		if(lkId.compare((*itr)) == 0)
		{
			std::string lk_Id = (*itr);	
			DEBUG("ACS_LM_AlarmStore::removeFromA2List() removed lkId=%s",lk_Id.c_str());
			alarm2LkList.erase(itr);
			removed = true;
			break;
		}
	}
	DEBUG("ACS_LM_AlarmStore::removeFromA2List() %s","Leaving");
	return removed;
}//end of removeFromA2List
/*=================================================================
	ROUTINE: removeFromA3List
=================================================================== */
bool ACS_LM_AlarmStore::removeFromA3List(const std::string& lkId)
{
	DEBUG("ACS_LM_AlarmStore::removeFromA3List() %s","Entering");

	INFO("removeFromA3List() lkId = %s",lkId.c_str());
	bool removed = false;

	for(std::list<std::string>::iterator itr=alarm3LkList.begin();
		itr!=alarm3LkList.end(); ++itr)
	{	
		if(lkId.compare((*itr)) == 0)
		{
			std::string lk_Id = (*itr);
			DEBUG("ACS_LM_AlarmStore::removeFromA3List() removed lkId=%s",lk_Id.c_str());
			alarm3LkList.erase(itr);
			removed = true;
			break;
		}
	}
	DEBUG("ACS_LM_AlarmStore::removeFromA3List() %s","Leaving");
	return removed;
}//end of removeFromA3List
/*=================================================================
	ROUTINE: isLkPresentInA2List
=================================================================== */
bool ACS_LM_AlarmStore::isLkPresentInA2List(const std::string& lkId)
{
	DEBUG("ACS_LM_AlarmStore::isLkPresentInA2List() %s, lkId=%s","Entering",lkId.c_str() );

	bool lkFound = false;

	for(std::list<std::string>::iterator itr=alarm2LkList.begin();
		itr!=alarm2LkList.end(); ++itr)
	{
		std::string lk_id = (*itr);
		DEBUG("ACS_LM_AlarmStore::isLkPresentInA2List() in for loop: lkid = %s",lk_id.c_str());	
		if(lkId.compare((*itr)) == 0)
		{
			lkFound = true;
			DEBUG("ACS_LM_AlarmStore::isLkPresentInA2List()  lkid = %s, %s",lk_id.c_str(), "is present in A2 List");	
			break;
		}
	}
	DEBUG("ACS_LM_AlarmStore::isLkPresentInA2List() %s, lkFound=%d","Leaving",lkFound);
	return lkFound;
}//end of isLkPresentInA2List
/*=================================================================
	ROUTINE: isLkPresentInA3List
=================================================================== */
bool ACS_LM_AlarmStore::isLkPresentInA3List(const std::string& lkId)
{
	DEBUG("ACS_LM_AlarmStore::isLkPresentInA3List() %s, lkId=%s","Entering",lkId.c_str());

	bool lkFound = false;

	for(std::list<std::string>::iterator itr=alarm3LkList.begin();
		itr!=alarm3LkList.end(); ++itr)
	{
		std::string lk_id = (*itr);
		DEBUG("ACS_LM_AlarmStore::isLkPresentInA3List() in for loop: lkid = %s",lk_id.c_str())
		if(lkId.compare((*itr)) == 0)
		{
			lkFound = true;
			DEBUG("ACS_LM_AlarmStore::isLkPresentInA3List()  itr_lkid = %s, %s",lk_id.c_str(), "is present in A3 List");	
			break;
		}
	}
	DEBUG("ACS_LM_AlarmStore::isLkPresentInA3List() %s, lkFound=%d","Leaving",lkFound);
	return lkFound;
}//end of isLkPresentInA3List
/*=================================================================
	ROUTINE: clearA2List
=================================================================== */
void ACS_LM_AlarmStore::clearA2List()
{
	DEBUG("ACS_LM_AlarmStore::clearA2List() %s","Entering");

	alarm2LkList.clear();

	DEBUG("ACS_LM_AlarmStore::clearA2List() %s","Leaving");
}//end of clearA2List
/*=================================================================
	ROUTINE: clearA3List
=================================================================== */
void ACS_LM_AlarmStore::clearA3List()
{
	DEBUG("ACS_LM_AlarmStore::clearA3List() %s","Entering");

	alarm3LkList.clear();

	DEBUG("ACS_LM_AlarmStore::clearA3List() %s","Leaving");
}//end of clearA3List
/*=================================================================
	ROUTINE: getLkfMissingAlarmState
=================================================================== */
bool ACS_LM_AlarmStore::getLkfMissingAlarmState()
{
	DEBUG("ACS_LM_AlarmStore::getLkfMissingAlarmState() %s","Entering");

	bool state = false;
	if(lkfMissingAlarmState != 0)
	{
		state = true;
	}

	DEBUG("ACS_LM_AlarmStore::getLkfMissingAlarmState() %s, state=%d","Leaving",state);
	return state;
}//end of getLkfMissingAlarmState
/*=================================================================
	ROUTINE: setLkfMissingAlarmState
=================================================================== */
void ACS_LM_AlarmStore::setLkfMissingAlarmState(bool state)
{
	DEBUG("ACS_LM_AlarmStore::setLkfMissingAlarmState() %s, state=%d","Entering",state);

	lkfMissingAlarmState = state;

	DEBUG("ACS_LM_AlarmStore::setLkfMissingAlarmState() %s","Leaving");
}//end of setLkfMissingAlarmState
/*=================================================================
	ROUTINE: getEmergencyAlarmState
=================================================================== */
bool ACS_LM_AlarmStore::getEmergencyAlarmState()
{
	DEBUG("ACS_LM_AlarmStore::getEmergencyAlarmState() %s","Entering");

	bool state = false;
	if(emergencyAlarmState != 0)
	{
		state = true;
	}
	DEBUG("ACS_LM_AlarmStore::getEmergencyAlarmState() %s, emergencyAlarmState=%d","Leaving",emergencyAlarmState);

	return state;	
}//end of getEmergencyAlarmState
/*=================================================================
	ROUTINE: setEmergencyAlarmState
=================================================================== */
void ACS_LM_AlarmStore::setEmergencyAlarmState(bool state)
{
	DEBUG("ACS_LM_AlarmStore::setEmergencyAlarmState() %s","Entering");

	emergencyAlarmState = state;

	DEBUG("ACS_LM_AlarmStore::setEmergencyAlarmState() %s, emergencyAlarmState=%d","leaving",emergencyAlarmState);
}//end of setEmergencyAlarmState
/* LM Maintenance Mode */
/*=================================================================
        ROUTINE: getMaintenanceAlarmState
=================================================================== */
bool ACS_LM_AlarmStore::getMaintenanceAlarmState()
{
        DEBUG("ACS_LM_AlarmStore::getMaintenanceAlarmState() %s","Entering");

        bool state = false;
        if(maintenanceAlarmState != 0)
        {
                state = true;
        }
	DEBUG("ACS_LM_AlarmStore::getMaintenanceAlarmState() %s, maintenanceAlarmState=%d ","Leaving",maintenanceAlarmState);

        return state;
}//end of getMaintenanceAlarmState
/*=================================================================
        ROUTINE: setMaintenanceAlarmState
=================================================================== */
void ACS_LM_AlarmStore::setMaintenanceAlarmState(bool state)
{
        DEBUG("ACS_LM_AlarmStore::setMaintenanceAlarmState() %s","Entering");

        maintenanceAlarmState = state;

        DEBUG("ACS_LM_AlarmStore::setMaintenanceAlarmState() %s, maintenanceAlarmState=%d","leaving",maintenanceAlarmState);
}//end of setMaintenanceAlarmState

/*=================================================================
	ROUTINE: store
=================================================================== */
void ACS_LM_AlarmStore::store(ACE_TCHAR* destBuf, ACE_UINT64& destPos)
{	
        DEBUG("ACS_LM_AlarmStore::store() %s","Entering");

        std::string nodeIp = ACS_LM_Common::getThisNodeIp();
        ACS_LM_Common::encodeString120(destBuf, destPos, nodeIp);

	ACE_UINT64 a2Count = (ACE_UINT64)alarm2LkList.size();
	ACS_LM_Common::encode32(destBuf, destPos, a2Count);

	for(std::list<std::string>::iterator it = alarm2LkList.begin();
		it != alarm2LkList.end(); ++it)
	{
		std::string lkId = (*it);
		ACS_LM_Common::encodeString120(destBuf, destPos, lkId);
	}

	ACE_UINT64 a3Count = (ACE_UINT64)alarm3LkList.size();
	ACS_LM_Common::encode32(destBuf, destPos, a3Count);

	for(std::list<std::string>::iterator it = alarm3LkList.begin();
		it != alarm3LkList.end(); ++it)
	{
		std::string lkId = (*it);
		ACS_LM_Common::encodeString120(destBuf, destPos, lkId);
	}
        DEBUG("ACS_LM_AlarmStore::store() %s","Leaving");
}//end of store
/*=================================================================
	ROUTINE: sizeInBytes
=================================================================== */
ACE_INT64 ACS_LM_AlarmStore::sizeInBytes()
{
	ACE_UINT64 size = 8; // 4+4 for list sizes
      size+= 15; // for node name 
	if(!alarm2LkList.empty())
	{
		size += (ACE_UINT64)alarm2LkList.size()*15;
	}
	if(!alarm3LkList.empty())
	{
		size += (ACE_UINT64)alarm3LkList.size()*15;
	}
	return size;
}//end of sizeInBytes
/*=================================================================
	ROUTINE: load
=================================================================== */
ACS_LM_AlarmStore* ACS_LM_AlarmStore::load(const std::string& alarmFile)
{
        DEBUG("ACS_LM_AlarmStore::load(const std::string&) %s","Entering");

	std::string almFile = ALARM_HOME;
	almFile += "/";
	almFile += alarmFile;
	INFO("In ACS_LM_AlarmStore::load(const std::string&) alarmFile=%s, alarmFile path = %s",alarmFile.c_str(),almFile.c_str());
	ACS_LM_AlarmStore* alarmStore = NULL;
	/* CNI 10922-APZ 21250/8-286 */
	if(ACS_LM_Common::isFileExists(almFile))
	{
		ACE_UINT64 fileSize = (ACE_UINT64)ACS_LM_Common::getFileSize(almFile);
		DEBUG("ACS_LM_AlarmStore::load(const std::string&) Alarm file size=%d",fileSize);
		if(fileSize != 0)
		{
			ACE_HANDLE handle= ACE_OS::open(almFile.c_str(),S_IRUSR);
    			if(handle != ACE_INVALID_HANDLE)
			{
				ACE_UINT64 bytesRead = 0;
				const ACE_TCHAR * buffer = new ACE_TCHAR[fileSize];
				/* Check memory allocated. if not, ignore the loading of Alarm file */
				if (buffer != NULL)
				{	
					bytesRead = ACE_OS::read(handle,(void*)(buffer),fileSize);
					DEBUG("ACS_LM_AlarmStore::load(const std::string&) Alarm file bytes read=%d",bytesRead);
					if(bytesRead == fileSize)
					{
						ACE_UINT64 pos = 0;
						alarmStore = ACS_LM_AlarmStore::load(buffer, pos);
						alarmStore->alarmFile = almFile;
					}
					delete[] buffer;	
				}	
				ACE_OS::close(handle);
			}
		} //if(fileSize != 0)
	}
	else
	{
		INFO("ACS_LM_AlarmStore::load(const std::string&) alarmFile=%s %s path = %s",alarmFile.c_str(),"does not exist at",almFile.c_str());
	}	
	if(!alarmStore)
	{
		alarmStore = new ACS_LM_AlarmStore();
		alarmStore->alarmFile = almFile;
	}

        DEBUG("ACS_LM_AlarmStore::load(const std::string&) %s","leaving");
	return alarmStore;
}//end of load
/*=================================================================
	ROUTINE: commit
=================================================================== */
bool ACS_LM_AlarmStore::commit()
{
        DEBUG("ACS_LM_AlarmStore::commit() %s","Entering");

	bool committed = false;
	ACE_UINT64 pos = 0;
	
	ACE_UINT64 bufSize = sizeInBytes();
	ACE_TCHAR * buffer = new ACE_TCHAR[bufSize];
	ACE_OS::memset(buffer, 0, bufSize);

	store(buffer, pos);

	ACE_HANDLE handle = ACE_INVALID_HANDLE;

	for(int attempt=0; attempt<5; attempt++)
	{
		/*handle = ::CreateFile(alarmFile.c_str(),
        					  GENERIC_WRITE,
                              0,
                              NULL,
                              CREATE_ALWAYS,
                              0,
                              NULL);*/
		handle= ACE_OS::open(alarmFile.c_str(),O_CREAT | O_TRUNC | O_WRONLY);

		if(handle != ACE_INVALID_HANDLE)
		{
			break;
		}
		else
		{
			DEBUG( "commit() FILE creation attempt %d failed", attempt);
		}
	}

    if(handle != ACE_INVALID_HANDLE)
	{
    	ACE_UINT64 bytesWritten = 0;
		
//		(void)::WriteFile(handle, buffer, bufSize, &bytesWritten, NULL);
    	bytesWritten = ACE_OS::write(handle, buffer,bufSize);
		//(void)::FlushFileBuffers(handle); //TBR
//		::CloseHandle(handle);
		ACE_OS::close(handle);

		if(bytesWritten == bufSize)
		{
			committed = true;
		}
		else
		{
			DEBUG("commit() WriteFile failed, bytesWritten = %d ",bytesWritten);
		}			
	}
    else
    {
        //ACE_UINT64 ec = ::GetLastError();
    	DEBUG("commit() CreateFile() failed, errorno: %d",errno);
//        ACS_LM_PrintTrace(ACS_LM_AlarmStore, "commit() CreateFile() failed, ec="<<ec);
    }

    delete[] buffer;
	
//	ACS_LM_PrintTrace(ACS_LM_AlarmStore, "commit() commited="<<committed);
	DEBUG("commit() commited = %d",committed);
    
    DEBUG("ACS_LM_AlarmStore::commit() %s","Leaving");
    return committed;	
}//end of commit
/*=================================================================
	ROUTINE: load
=================================================================== */
ACS_LM_AlarmStore* ACS_LM_AlarmStore::load(const ACE_TCHAR* srcBuff, ACE_UINT64& srcPos)
{
        DEBUG("ACS_LM_AlarmStore::load(const std::string&, ACE_UINT64&) %s srcPos =%d","Entering",srcPos);

	ACS_LM_AlarmStore* alarmStore = new ACS_LM_AlarmStore();
	bool checkFile = false;
	/* CNI 10922-APZ 21250/8-286 */
	/* Check memory allocated. if not, ignore the A2/A3 alarm's list preparation */
	if (alarmStore != NULL)
	{
    		alarmStore->ownerOfFile = ACS_LM_Common::decodeString120(srcBuff, srcPos);
    		ACE_UINT64 a2Count = ACS_LM_Common::decode32(srcBuff, srcPos);
		INFO("ACS_LM_AlarmStore::load(const std::string&, ACE_UINT64&) load() a2Count = %d",a2Count);
		if( a2Count > 0 && !checkFile )
		{
			for(ACE_UINT64 i=0; i<a2Count; i++)
			{
				DEBUG("ACS_LM_AlarmStore::load(const std::string&, ACE_UINT64&) %s","A2 Alarm Load");
				std::string lkId = ACS_LM_Common::decodeString120(srcBuff, srcPos);
				//FIX TR HV27652
				string temp = lkId.substr(0, 3);
				if(ACE_OS::strcasecmp(temp.c_str(),"CXC") != 0)
				{
					checkFile = true;
					break;
				} //FIX
				alarmStore->alarm2LkList.push_back(lkId);
			}
		}

		ACE_UINT64 a3Count = ACS_LM_Common::decode32(srcBuff, srcPos);
		INFO("ACS_LM_AlarmStore::load(const std::string&, ACE_UINT64&) load() a3Count = %d",a3Count);
		if( a3Count > 0 && !checkFile )
		{
			for(ACE_UINT64 j=0; j<a3Count; j++)
			{
				DEBUG("ACS_LM_AlarmStore::load(const std::string&, ACE_UINT64&) %s","A3 Alarm Load");
				std::string lkId = ACS_LM_Common::decodeString120(srcBuff, srcPos);
				//FIX TR HV27652
				string temp = lkId.substr(0, 3);
				if(ACE_OS::strcasecmp(temp.c_str(),"CXC") != 0)
				{
					checkFile = true;
					break;
				} //FIX
				alarmStore->alarm3LkList.push_back(lkId);
			}

		}
		//FIX TR HV27652
		if (checkFile)
		{
			DEBUG("ACS_LM_AlarmStore::load(const std::string&, ACE_UINT64&) %s","Clear Alarm List");
			alarmStore->alarm2LkList.clear();
			alarmStore->alarm3LkList.clear();
		} //FIX
	}
    DEBUG("ACS_LM_AlarmStore::load(const std::string&, ACE_UINT64&) %s","Leaving");
    return alarmStore;
}//end of load
/*=================================================================
	ROUTINE: isA3ListEmpty
=================================================================== */
bool ACS_LM_AlarmStore::isA3ListEmpty()
{
        DEBUG("ACS_LM_AlarmStore::isA3ListEmpty() %s","Entering");

	ACE_INT16 a3ListSize = (ACE_INT16)alarm3LkList.size();
	INFO("ACS_LM_AlarmStore::isA3ListEmpty() a3ListSize = %d",a3ListSize);

        DEBUG("ACS_LM_AlarmStore::isA3ListEmpty() %s","Leaving");
	return a3ListSize==0;
}//end of isA3ListEmpty

/*=================================================================
	ROUTINE: isA2ListEmpty
=================================================================== */
bool ACS_LM_AlarmStore::isA2ListEmpty()
{
        DEBUG("ACS_LM_AlarmStore::isA2ListEmpty() %s","Entering");

	ACE_INT16 a2ListSize = (ACE_INT16)alarm2LkList.size();
	INFO("ACS_LM_AlarmStore::isA2ListEmpty() a2ListSize = %d",a2ListSize);

        DEBUG("ACS_LM_AlarmStore::isA2ListEmpty() %s","Leaving");
	return a2ListSize==0;
}//end of isA2ListEmpty
/*=================================================================
	ROUTINE: getOwnerOfFile
=================================================================== */
std::string ACS_LM_AlarmStore::getOwnerOfFile()
{
    DEBUG("ACS_LM_AlarmStore::getOwnerOfFile() - ownerOfFile = %s",ownerOfFile.c_str()); 
    return ownerOfFile;
}//end of getOwnerOfFile
/*=================================================================
	ROUTINE: setOwnerOfFile
=================================================================== */
void ACS_LM_AlarmStore::setOwnerOfFile(std::string nodeIp)
{
    DEBUG("ACS_LM_AlarmStore::setOwnerOfFile() - ownerOfFile = %s",nodeIp.c_str()); 
    ownerOfFile = nodeIp;
}//end of setOwnerOfFile
