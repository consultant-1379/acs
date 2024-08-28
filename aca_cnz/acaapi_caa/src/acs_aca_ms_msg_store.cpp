/*=================================================================== */
   /**
   @file acs_aca_ms_msg_store.cpp

   Class method implementationn for MSA API

   This module contains the implementation of class declared in
   the acs_aca_ms_msg_store.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       17/08/2011     XTANAGG       Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <sstream>
#include <stdexcept>

#include "acs_aca_ms_const_values.h"
#include "acs_aca_ms_parameters.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_configuration_helper.h"
#include "acs_aca_ms_msg_store.h"

using namespace std;

ACS_ACA_TRACE_DEFINE(ACS_ACA_ACAMS_MsgStore);


/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: ACAMS_MsgStore
=================================================================== */
ACAMS_MsgStore::ACAMS_MsgStore(const string & msName, const string & cpSite, int initiator)
	: myHomePath(), myName() {
	ACS_ACA_TRACE_FUNCTION;
	fxInitialize(false, 0, msName.c_str(), cpSite.c_str(), initiator);
}

ACAMS_MsgStore::ACAMS_MsgStore(const string & msName, unsigned cpSystemId, int initiator)
	: myHomePath(), myName() {
	ACS_ACA_TRACE_FUNCTION;
	fxInitialize(true, cpSystemId, msName.c_str(), 0, initiator);
}

/*===================================================================
   ROUTINE: fxInitialize
=================================================================== */
void ACAMS_MsgStore::fxInitialize(bool isMultipleCPSystem, unsigned cpSystemId, const char * msName, const char * cpSite,	int initiator) {
	ACS_ACA_TRACE_FUNCTION;

	char szPath[512] = {0};
	int dwLen = ACS_ACA_ARRAY_SIZE(szPath);

	int call_result = (isMultipleCPSystem)
			? ACS_ACA_ConfigurationHelper::get_data_disk_path(cpSystemId, szPath, dwLen)
			: ACS_ACA_ConfigurationHelper::get_data_disk_path(szPath, dwLen);

	if (call_result < 0) {
		char msg[512] = {0};
		ACE_OS::snprintf(msg, sizeof(msg) - 1, "Failed to get data disk path [ERROR CODE = %d, IsMultipleCP = %s]",
				call_result, (isMultipleCPSystem ? "TRUE" : "FALSE"));
		ACS_ACA_TRACE_MESSAGE("ERROR: %s", msg);
		throw std::runtime_error(msg);
	}

	myHomePath = szPath;

	if (initiator) {
		// client called the constructor
		myName = myHomePath + "/" + msName;
		if (!isMultipleCPSystem)
			myName.append("/").append(cpSite ? cpSite : "");
	
		ACS_ACA_TRACE_MESSAGE("[initiator == true], returning");
		return;
	}

	bool dirCreated = true;
	if (ACE_OS::mkdir(myHomePath.c_str(), S_IRWXU | S_IRWXG ) == -1 )	{
		if (ACE_OS::last_error() != EEXIST ) {
			// Failed to create 'ACA' directory.
			// The parent directories might not exist, try to create them.
			string temp;
			std::list<string> lst = findDir(myHomePath);
			while (lst.size()) {
				temp = lst.front();
				lst.pop_front();
			
				if (ACE_OS::mkdir( temp.c_str(), S_IRWXU | S_IRWXG ) == -1)	{
					if (ACE_OS::last_error() != EEXIST)	{
						// Failed to create directory
						dirCreated = false;
						break;
					}
				}
			}
			// try again.
			if (dirCreated && (ACE_OS::mkdir( myHomePath.c_str(), S_IRWXU | S_IRWXG) == -1 ))	{
				if (ACE_OS::last_error() != EEXIST)
					dirCreated = false;
			}
		}
		else { // ACA directory already exists.
			myName = myHomePath + "/" + msName;
			if (!isMultipleCPSystem)
				myName.append("/").append(cpSite ? cpSite : "");

			ACS_ACA_TRACE_MESSAGE("ACA directory already exists, returning");
			return;
		}
	}

	if (!dirCreated) {
		// Failed to create 'ACA' directory
		char msg[512] = {0};
		ACE_OS::snprintf(msg, sizeof(msg) - 1, "Failed to create ACA directory [%s] ", myHomePath.c_str());
		ACS_ACA_TRACE_MESSAGE("ERROR: %s", msg);
		throw std::runtime_error(msg);
	}

	// Add 'name' to the MS home path
	myName = myHomePath + "/" + msName;
	if (!isMultipleCPSystem)
		myName.append("/").append(cpSite ? cpSite : "");
}

/*===================================================================
   ROUTINE: ~ACAMS_MsgStore
=================================================================== */
ACAMS_MsgStore::~ACAMS_MsgStore() {
	ACS_ACA_TRACE_FUNCTION;
}

/*===================================================================
   ROUTINE: getCommitFileName
=================================================================== */
string ACAMS_MsgStore::getCommitFileName() const {
	ACS_ACA_TRACE_FUNCTION;
	return (myName + "/" + ACAMS_NameOfCommitFile);
}

/*===================================================================
   ROUTINE: getDataFileDirectoryName
=================================================================== */
string ACAMS_MsgStore::getDataFileDirectoryName() const {
	ACS_ACA_TRACE_FUNCTION;
	return (myName + "/" + ACAMS_NameOfDataFileDir);
}

/*===================================================================
   ROUTINE: getMS_Name
=================================================================== */
string ACAMS_MsgStore::getMS_Name() const {
	ACS_ACA_TRACE_FUNCTION;
	return myName;
}

/*===================================================================
   ROUTINE: getMSD_Home
=================================================================== */
string ACAMS_MsgStore::getMSD_Home() const {
	ACS_ACA_TRACE_FUNCTION;
	return myHomePath;
}

/*===================================================================
   ROUTINE: findNextFile
=================================================================== */
string ACAMS_MsgStore::findNextFile(const string & previousFile, bool * foundDir) {
	ACS_ACA_TRACE_FUNCTION;
	string oldestFound = "";
	string ffile = this->getDataFileDirectoryName();
	*foundDir = false;

	DIR *pDir = 0;
	pDir = ACE_OS::opendir ( ffile.c_str() );
	bool found = false;

	if( pDir != 0 ) {
		dirent *pEntry;
		int countFiles = 0;
		pEntry = ACE_OS::readdir( pDir );
		while( pEntry != 0 ) {
			countFiles++;
			if( (ACE_OS::strcmp( pEntry->d_name, ".")) != 0 && (ACE_OS::strcmp( pEntry->d_name, "..")) !=0 ) {
				ACS_ACA_TRACE_MESSAGE("Found files in dataFiles directory");
				found = true;
				break;
			} 
			pEntry = ACE_OS::readdir( pDir );
				
		}

		if (!found)	{
			//Every directory will have 2 entries
			ACS_ACA_TRACE_MESSAGE("ERROR: Cannot find first file in %s", this->getDataFileDirectoryName().c_str());
			ACE_OS::closedir(pDir);
			return oldestFound;
		}
		else {
			*foundDir = true;
			
			unsigned long long currentFileMsgNumber = 0;
			unsigned long long previousFileMsgNumber = 0;
			unsigned long long oldestFoundFileMsgNumber = ~0;
			
			istringstream(previousFile) >> previousFileMsgNumber;
			do {
				if ((ACE_OS::strcmp(pEntry->d_name, ".")  != 0) && (ACE_OS::strcmp(pEntry->d_name, "..") != 0))	{
					istringstream(pEntry->d_name) >> currentFileMsgNumber;

					if ((previousFileMsgNumber < currentFileMsgNumber) && (currentFileMsgNumber < oldestFoundFileMsgNumber)) {
						oldestFoundFileMsgNumber = currentFileMsgNumber;
						oldestFound = pEntry->d_name;
					}
				}
				pEntry = ACE_OS::readdir( pDir );
			}

			while (pEntry != 0);
			ACE_OS::closedir(pDir);
			ACS_ACA_TRACE_MESSAGE("File found, returning %s", oldestFound.c_str());
			return oldestFound;
		}
	}
	else {
		ACS_ACA_TRACE_MESSAGE("ERROR: Cannot find directory %s", this->getDataFileDirectoryName().c_str());
		return oldestFound;
	}
}

/*===================================================================
   ROUTINE: getFirstMsgNumber
=================================================================== */
unsigned long long ACAMS_MsgStore::getFirstMsgNumber(const string & fileName) const {
	ACS_ACA_TRACE_FUNCTION;
	unsigned long long  msgNumber = 0;
	istringstream(fileName) >> msgNumber;
	return msgNumber;
}

/*===================================================================
   ROUTINE: findDir
=================================================================== */
std::list<std::string> ACAMS_MsgStore::findDir(string s) {
	ACS_ACA_TRACE_FUNCTION;
	std::list<std::string> subPaths;
	string::size_type pos;

	for (pos = 0; (pos = s.find('/', pos)) != string::npos;pos++) {
		if((pos > 0) && (s[pos - 1] == ':'))
			 subPaths.push_back(s.substr(0, pos));
	}

	return subPaths;
}
