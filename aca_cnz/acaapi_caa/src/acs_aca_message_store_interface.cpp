/*=================================================================== */
   /**
   @file acs_aca_message_store_interface.cpp

   Class method implementationn for MSA API.

   This module contains the implementation of class declared in
   the acs_aca_message_store_interface.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       16/08/2011     XTANAGG      Initial Release    **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <iostream>
#include <sstream>
#include <dirent.h>

#include <ace/Event.h>

#include "acs_aca_utility.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_configuration_helper.h"
#include "acs_aca_message_store_interface.h"

#define MARKUSED(X) ((void)(&(X)))

ACS_ACA_TRACE_DEFINE(ACS_ACA_MessageStoreInterface)


/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
bool ACS_ACA_MessageStoreInterface::_isMultipleCPSystem = false;
std::map<unsigned, std::string> ACS_ACA_MessageStoreInterface::_cpMap;
std::map<std::string, ACE_event_t> ACS_ACA_MessageStoreInterface::_namedEventMap;
const char ACS_ACA_MessageStoreInterface::CP_FIRST_CONTACT_EVENT_PREFIX_NAME[] = "Global\\Pj5hZHJlbSBpZCBvenplcCBudSBpZXMgZWhjIGl0YWRyb2NpUjw8_";
bool ACS_ACA_MessageStoreInterface::firstTimeMulCP = true;
bool ACS_ACA_MessageStoreInterface::firstTimeCPID = true;
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
/*===================================================================
   ROUTINE: getCPsForName
=================================================================== */
bool ACS_ACA_MessageStoreInterface::getCPsForName(const std::string & msName, std::vector<unsigned> & cpNames) 
{
	ACS_ACA_TRACE_FUNCTION;

	if (fxCheckMultipleCPSystem()) {
		ACS_ACA_TRACE_MESSAGE("Call 'fxCheckMultipleCPSystem' failed!");
		return false;
	}

	if (fxGetCpIds())	{
		ACS_ACA_TRACE_MESSAGE("Call 'fxGetCpIds' failed!");
		return false;
	}

	cpNames.clear();
	for (std::map<unsigned, std::string>::iterator it = _cpMap.begin(); it != _cpMap.end(); ++it) {
		//Find all the files in the dataFiles folder and add it to cpNames.
		//std::string fullPath = (it->second).c_str() + "/" + msName.c_str() + "/" + "dataFiles" ;

		std::string fullPath("");

		fullPath.append((it->second).c_str());
		fullPath.append("/");
		fullPath.append(msName.c_str());
		fullPath.append("/");
		fullPath.append("dataFiles");

		DIR *pDir = ACE_OS::opendir ( fullPath.c_str() );
		if (pDir) {
			dirent *pEntry;
			pEntry = ACE_OS::readdir( pDir );
			while( pEntry != 0)	{
				if( ACE_OS::strcmp( pEntry->d_name, ".") && ACE_OS::strcmp( pEntry->d_name, "..") )	{
					cpNames.push_back(it->first); //there is at least one file
					break;
				}
				pEntry = ACE_OS::readdir( pDir );
			}
			ACE_OS::closedir (pDir);
		}
	}

	_errorCode = ACA_NO_ERROR;
	_errorDescription.clear();
	return true;
}

/*===================================================================
   ROUTINE: getHandle
=================================================================== */
ACE_event_t ACS_ACA_MessageStoreInterface::getHandle(const std::string & msName)
{
	ACS_ACA_TRACE_FUNCTION;
	std::map<std::string, ACE_event_t>::iterator it = _namedEventMap.find(msName);

	if (it != _namedEventMap.end()) {
		ACS_ACA_TRACE_MESSAGE("Message Store found in the event map!");
		return it->second;
	}
	
	//Try to open the server published named event.
	std::string eventName = CP_FIRST_CONTACT_EVENT_PREFIX_NAME + msName;

	//created by MSD. To check, below code is not correct.
	ACE_event_t event;
	ACE_INT32 result  = ACE_OS::event_init(&event, 0, 0, ACE_DEFAULT_SYNCH_TYPE, eventName.c_str());
	if (result == -1) {
		_errorCode = ACA_ERR_CANNOT_GET_EVENT_HANDLE;
		ACE_INT32 lastError = ACE_OS::last_error();
		char errMessage[128] = {0};
		ACE_OS::snprintf(errMessage, sizeof(errMessage) - 1, "'event_init(...' system call failed with error code %u! No server published the notification event.", lastError);
		_errorDescription = errMessage;
		//return result;
	}
	
	//Insert the handle into the internal map.
	_namedEventMap.insert(std::map<std::string, ACE_event_t>::value_type(msName, event));
	
	_errorCode = ACA_NO_ERROR;
	_errorDescription.clear();
	return event;
}

/*===================================================================
   ROUTINE: fxCheckMultipleCPSystem
=================================================================== */
int ACS_ACA_MessageStoreInterface::fxCheckMultipleCPSystem() 
{
	ACS_ACA_TRACE_FUNCTION;
	int errorLevel = 0; //CR639

	if(firstTimeMulCP) //CR639
	{
		ACS_ACA_CS_API::CS_API_Result_ACA result;
		bool isMultipleCP = false;
		
		try
		{
			result = (ACS_ACA_CS_API::CS_API_Result_ACA) ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCP);
			if (result == ACS_ACA_CS_API::Result_Success) 
			{
				_isMultipleCPSystem = isMultipleCP;
				_errorCode = ACA_NO_ERROR;
				_errorDescription.clear();
				firstTimeMulCP=false; //CR639
			} 
			else 
			{
				_errorCode = ACA_ERR_CHECKING_MULTIPLE_CP;
				char errDescription[96] = {0};
				ACE_OS::snprintf(errDescription, sizeof(errDescription) - 1, "'ACS_CS_API_NetworkElement::isMultipleCPSystem(...' failed! Error Code = %d.", result);
				_errorDescription = errDescription;
				errorLevel = 3;
				ACS_ACA_TRACE_MESSAGE("Call 'ACS_CS_API_NetworkElement::isMultipleCPSystem' failed, call_result == %d", result);
			}
		} 
		catch (...) 
		{
			_errorCode = ACA_ERR_CHECKING_MULTIPLE_CP;
			_errorDescription = "'ACS_CS_API_NetworkElement::isMultipleCPSystem(...' failed with exception!";
			errorLevel = 3;
		}
	}
	return errorLevel;
}

/*===================================================================
   ROUTINE: fxGetCpIds
=================================================================== */
int ACS_ACA_MessageStoreInterface::fxGetCpIds() 
{
	ACS_ACA_TRACE_FUNCTION;
	int errorLevel = 0;
	if(firstTimeCPID) //CR639
	{
		ACS_CS_API_CP * cp = 0;

		try 
		{
			if (!(cp = ACS_CS_API::createCPInstance())) 
			{
				_errorCode = ACA_ERR_CS_API_FAILURE;
				_errorDescription = "'ACS_CS_API::createCPInstance()' CS API call failed!";
				ACS_ACA_TRACE_MESSAGE("Call 'createCPInstance' failed, returning 5");
				return 5;
			}
		} 
		catch (...)
		{
			_errorCode = ACA_ERR_CS_API_FAILURE;
			_errorDescription = "'ACS_CS_API::createCPInstance()' CS API call failed with exception!";
			ACS_ACA_TRACE_MESSAGE("Call 'createCPInstance' threw an exception, returning 5");
			return 5;
		}
		
		ACS_CS_API_IdList cpList;
		ACS_ACA_CS_API::CS_API_Result_ACA result;

		try
		{
			 result = (ACS_ACA_CS_API::CS_API_Result_ACA) cp->getCPList(cpList); 
		}
		catch (...)
		{
			_errorCode = ACA_ERR_CS_API_FAILURE;
			_errorDescription = "'getCPList()' CS API call failed with exception!";
			ACS_ACA_TRACE_MESSAGE("Call 'getCPList' threw an exception, errorLevel == 5");
			errorLevel = 5;
		}

		if (errorLevel)
		{
			try
			{
				ACS_CS_API::deleteCPInstance(cp);
			}
			catch (...)
			{
				_errorDescription += "Moreover, 'ACS_CS_API::deleteCPInstance(...' failed to destroy its internal objects.";
			}
			ACS_ACA_TRACE_MESSAGE("Call 'getCPList' threw an exception, returning 5");
			return 5;
		}

		if (result != ACS_ACA_CS_API::Result_Success)
		{
			_errorCode = ACA_ERR_CS_API_FAILURE;
			char errDescription[96] = {0};
			ACE_OS::snprintf(errDescription, sizeof(errDescription) - 1, "'getCPList()' CS API call failed to retrieve CP list! Error Code = %d.", result);
			_errorDescription = errDescription;
			try
			{
				ACS_CS_API::deleteCPInstance(cp);
			}
			catch (...)
			{
				_errorDescription += "Moreover, 'ACS_CS_API::deleteCPInstance(...' failed to destroy its internal objects.";
			}
			ACS_ACA_TRACE_MESSAGE("Call 'deleteCPInstance' failed, returning 5");
			return 5;
		}
		
		unsigned cpId = 0;
		std::string cpBasePath;

		for (unsigned i = 0;
				(i < cpList.size()) && (cpId = cpList[i++], !(errorLevel = fxGetPath(cpId, cpBasePath)));
				_cpMap.insert(std::map<unsigned, std::string>::value_type(cpId, cpBasePath))
				) ;

		if (errorLevel)
			 _cpMap.clear();
		else
			firstTimeCPID = false; //CR639 errorLevel 0 means success

		try
		{
			ACS_CS_API::deleteCPInstance(cp);
		}
		catch (...)
		{
			if (errorLevel)
				 _errorDescription += "Moreover, 'ACS_CS_API::deleteCPInstance(...' failed to destroy its internal objects.";
		}
	} //CR639

	ACS_ACA_TRACE_MESSAGE("errorLevel == %d", errorLevel);
	return errorLevel;
}

/*===================================================================
   ROUTINE: fxGetPath
=================================================================== */
int ACS_ACA_MessageStoreInterface::fxGetPath(unsigned cpSystemId, std::string & basePath) {
	ACS_ACA_TRACE_FUNCTION;

	char szPath[512] = {0};
	int dwLen = ACS_ACA_ARRAY_SIZE(szPath);

	if (ACS_ACA_ConfigurationHelper::get_data_disk_path(cpSystemId, szPath, dwLen) < 0) {
		char msg[512] = {0};
		ACE_OS::snprintf(msg, sizeof(msg) - 1, "Failed to get data disk path");
		_errorCode = ACA_ERR_COMMON_DLL_API_FAILURE;
		_errorDescription = msg;
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to get data disk path");
		return 4;
	}

	basePath = szPath;
	return 0;
}

//***********CR639*******************
/*===================================================================
   ROUTINE: update
=================================================================== */
void ACS_ACA_MessageStoreInterface::update (const ACS_CS_API_CpTableChange & aObservee) {
	ACS_ACA_TRACE_FUNCTION;
	ACS_ACA_TRACE_MESSAGE("Received %zd CP Table updates", aObservee.dataSize);

	for (unsigned int i = 0; i < aObservee.dataSize; ++i) {
		unsigned cpID = aObservee.cpData[i].cpId;

		switch(aObservee.cpData[i].operationType) {
			case ACS_CS_API_TableChangeOperation::Add: {
					ACS_ACA_TRACE_MESSAGE("Added CP with id %u", cpID);

					//Get the updated cpidInfo and inert into the CP map
					std::string cpBasePath;
					int errorLevel = fxGetPath(cpID, cpBasePath);

					if (errorLevel)
						ACS_ACA_TRACE_MESSAGE( "ERROR in fxGetPath: %d", errorLevel);
					else
						_cpMap.insert(std::map<unsigned, std::string>::value_type(cpID, cpBasePath));
				}
				break;      

			case ACS_CS_API_TableChangeOperation::Delete:	{
					ACS_ACA_TRACE_MESSAGE("Removed CP with id %u", cpID);
					std::map<unsigned, std::string>::iterator it = _cpMap.find(cpID);
					if (it != _cpMap.end())
						_cpMap.erase(it);
				}
				break;

			case ACS_CS_API_TableChangeOperation::Change:
			case ACS_CS_API_TableChangeOperation::Unspecified:
			default:
				break;
		}
	}     

	// To be sure, erase the stored configuration
	ACS_ACA_TRACE_MESSAGE("Update callback received, forcing the configuration reload!");
	ACS_ACA_ConfigurationHelper::force_configuration_reload();
}

/*===================================================================
   ROUTINE: subscribeToCsServices
=================================================================== */
void ACS_ACA_MessageStoreInterface::subscribeToCsServices ()
{
	ACS_ACA_TRACE_FUNCTION;

	if (theSubscriptionMgr) {
		if ((theSubscriptionMgr->subscribeCpTableChanges((ACS_CS_API_CpTableObserver & )*this)) != ACS_CS_API_NS::Result_Success)
		{
			ACS_ACA_TRACE_MESSAGE("Call subscribeCpTableChanges failed !");	
		}

	}
}
/*===================================================================
   ROUTINE: unSubscribeToCsServices
=================================================================== */
void ACS_ACA_MessageStoreInterface::unSubscribeToCsServices ()
{
	ACS_ACA_TRACE_FUNCTION;

	if (theSubscriptionMgr) {
		if ((theSubscriptionMgr->unsubscribeCpTableChanges((ACS_CS_API_CpTableObserver & )*this)) != ACS_CS_API_NS::Result_Success)
		{	
			ACS_ACA_TRACE_MESSAGE("Call 'unsubscribeCpTableChanges' failed!");
		}
	}
}
