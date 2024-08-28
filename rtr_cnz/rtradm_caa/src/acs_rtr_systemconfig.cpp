//********************************************************************************
//
// NAME
// ACS_RTR_SystemConfig.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2007.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2007-05-10 by GP/AP/A QPAOELE
//
// DESCRIPTION 
// This class is responsible for the communication with CS.
// It implements a cache of the CS data.
//
//********************************************************************************
#include "acs_rtr_systemconfig.h"
#include "acs_rtr_macros.h"
namespace {
	//----------
	// Constants
	//----------
	const char * const _SYSCFG_NO_ERROR = "<NO ERRORS>";
	const char * const _SYSCFG_NO_DESCRIPTION = "<NO DESCRIPTION>";
	const char * const _SYSCFG_DERR_ = "Memory Error, cannot create ACS_DSD_Server object";
	const char * const _SYSCFG_DERR_NOENTRY_CS = "Cannot find the entry in the CS table.";
	const char * const _SYSCFG_DERR_NOVALUE_CS = "Cannot retrieve any value from the CS table.";
	const char * const _SYSCFG_DERR_NOACCESS_CS = "Cannot access to CS.";
	const char * const _SYSCFG_DERR_RESULT_FAILURE_CS = "Result Failure from CS.";
	const char * const _SYSCFG_DERR_UNKNOWN_CS = "Unknown error from CS.";
	const char * const _SYSCFG_DERR_BAD_PARAMETER = "Bad input parameter.";
	const char * const _SYSCFG_DERR_NEW_INSTANCE = "Memory Error, cannot create the ACS_RTR_SystemConfig instance.";
	const char * const _SYSCFG_DERR_INSERT_FAILED = "Failed insert in configuration map.";
}


ACS_RTR_SystemConfig* ACS_RTR_SystemConfig::_instance = 0;
bool ACS_RTR_SystemConfig::_initialized = false;
int ACS_RTR_SystemConfig::_lastError = SYSCFG_NO_ERROR;
const char * ACS_RTR_SystemConfig::_lastErrorDescription = _SYSCFG_NO_ERROR;
char * ACS_RTR_SystemConfig::_buffer = 0;
size_t ACS_RTR_SystemConfig::_bufferSize = 0;

//------------------------------------------------------------------------------
//      Instantiate a single object
//------------------------------------------------------------------------------
ACS_RTR_SystemConfig* ACS_RTR_SystemConfig::instance () {
	if (!_instance) {
        
		if (!(_instance = new ACS_RTR_SystemConfig()))
			setLastError(SYSCFG_ERR_NEW_INSTANCE, _SYSCFG_DERR_NEW_INSTANCE);
	}
	return _instance;
}

//------------
// Constructor
//------------
ACS_RTR_SystemConfig::ACS_RTR_SystemConfig() : _isBC(false), _numCP(0),_criticalSection() {
}

//-----------
// Destructor
//-----------
ACS_RTR_SystemConfig::~ACS_RTR_SystemConfig() {
}

//----------------
// fxIsValidCPName
//----------------
bool ACS_RTR_SystemConfig::isValidCPName(const char * cpName) {
	if (cpName &&	(strcmp(cpName, "") != 0)) return true;
	else return false;
}

//----------------
// fxIsValidCPId
//----------------
bool ACS_RTR_SystemConfig::isValidCPId(short id) {
	if (id != ~0) return true;
	else return false;
}

//-------------
// setLastError
//-------------
bool ACS_RTR_SystemConfig::setLastError(int error, const char * description, bool allocate) {
	if (description) {
		if (allocate) {
			size_t requestedSize = strlen(description) + 1;
			
			if (requestedSize > _bufferSize) {
				char * p = new char[requestedSize];
				if (!p) return false;
				delete[] _buffer;
				_bufferSize = requestedSize;
				_buffer = p;
			}

			_lastErrorDescription = strcpy(_buffer, description);
		} else _lastErrorDescription = description;
	} else _lastErrorDescription = (error == SYSCFG_NO_ERROR ? _SYSCFG_NO_ERROR : _SYSCFG_NO_DESCRIPTION);

	_lastError = error;
	return true;
}

//----------------------------------
// init
// get the CPs configuration from CS
//----------------------------------
bool ACS_RTR_SystemConfig::init() {
	if (_initialized) return true;
	_isBC = cs_isMultipleCPSystem();
	if (_isBC) {
		std::list<short> cpIdList = cs_getCPList();
		_numCP = cpIdList.size();
		if (_numCP <= 0) return false; // invalid list

		for (std::list<short>::iterator it = cpIdList.begin(); it != cpIdList.end(); it++) {
			short id = *it;
			string default_name = cs_getDefaultCPName(id);
			if (isValidCPName(default_name.c_str()))
				if (!_configurationMap.insert(map<short, string>::value_type(id, string(default_name))).second)
				{
					char trace[512] = {0};
					ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s Key: %d Value: %s", _SYSCFG_DERR_INSERT_FAILED, id, default_name.c_str());
					if (!setLastError(SYSCFG_ERR_INSERT_FAILED, trace, true)) setLastError(SYSCFG_ERR_INSERT_FAILED, _SYSCFG_DERR_INSERT_FAILED);
					_configurationMap.clear();
					return (_initialized = false);
				}
		}
	}
	return (_initialized = true);
}

//-------------------------------------------------
// getCP_List
// get the CP name list with no interaction with CS
//-------------------------------------------------
std::list<string> ACS_RTR_SystemConfig::getCP_List()
{
	std::list<string> cpNameList;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);

	if (!_configurationMap.empty())
	{
		maptype::iterator it;
		for (it = _configurationMap.begin(); it != _configurationMap.end(); it++)
		{
			cpNameList.push_back(((*it).second));
		}
	}
	return cpNameList;
}

//-----------------------------------------------
// getCP_IdList
// get the CP id list with no interaction with CS
//-----------------------------------------------
std::list<short> ACS_RTR_SystemConfig::getCP_IdList()
{
	std::list<short> cpIdList;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);
	if (!_configurationMap.empty())
	{
		for (maptype::iterator it = _configurationMap.begin(); it != _configurationMap.end(); it++)
		{
			cpIdList.push_back(((*it).first));
		}
	}

	return cpIdList;
}

//---------------------------------------------------
// fxSetCsError
// check the error code and set the error description
//---------------------------------------------------
int ACS_RTR_SystemConfig::fxSetCsError(ACS_CS_API_NS::CS_API_Result result) {
	int error = 0;
	char trace[512] = {0};
	switch (result) {
		case ACS_CS_API_NS::Result_Success: 		
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s CS Error: %d", _SYSCFG_NO_ERROR, result);
			if (!setLastError(SYSCFG_NO_ERROR, trace, true)) setLastError(SYSCFG_ERR_NOENTRY_CS, _SYSCFG_NO_ERROR);
			error = SYSCFG_NO_ERROR;
			break;
		case ACS_CS_API_NS::Result_NoEntry: 		
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s CS Error: %d", _SYSCFG_DERR_NOENTRY_CS, result);
			if (!setLastError(SYSCFG_ERR_NOENTRY_CS, trace, true)) setLastError(SYSCFG_ERR_NOENTRY_CS, _SYSCFG_DERR_NOENTRY_CS);
			error = SYSCFG_ERR_NOENTRY_CS;
			break;
		case ACS_CS_API_NS::Result_NoValue:
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s CS Error: %d", _SYSCFG_DERR_NOVALUE_CS, result);
			if (!setLastError(SYSCFG_ERR_NOVALUE_CS, trace, true)) setLastError(SYSCFG_ERR_NOVALUE_CS, _SYSCFG_DERR_NOENTRY_CS);
			error = SYSCFG_ERR_NOVALUE_CS;
			break;
		case ACS_CS_API_NS::Result_NoAccess:
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s CS Error: %d", _SYSCFG_DERR_NOACCESS_CS, result);
			if (!setLastError(SYSCFG_ERR_NOACCESS_CS, trace, true)) setLastError(SYSCFG_ERR_NOACCESS_CS, _SYSCFG_DERR_NOACCESS_CS);
			error = SYSCFG_ERR_NOACCESS_CS;
			break;
		case ACS_CS_API_NS::Result_Failure:
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s CS Error: %d", _SYSCFG_DERR_RESULT_FAILURE_CS, result);
			if (!setLastError(SYSCFG_ERR_RESULT_FAILURE_CS, trace, true)) setLastError(SYSCFG_ERR_RESULT_FAILURE_CS, _SYSCFG_DERR_RESULT_FAILURE_CS);
			error = SYSCFG_ERR_RESULT_FAILURE_CS;
			break;
		default:
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s CS Error: %d", _SYSCFG_DERR_UNKNOWN_CS, result);
			if (!setLastError(SYSCFG_ERR_UNKNOWN_CS, trace, true)) setLastError(SYSCFG_ERR_UNKNOWN_CS, _SYSCFG_DERR_UNKNOWN_CS);
			error = SYSCFG_ERR_UNKNOWN_CS;
	}
	return error;
}

/////////////////////////////////////////////////////////
//CS INTERACTION METHODS
/////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
// cs_isMultipleCPSystem
// check if it's a multiple CP system with a query on the Network Element Information
//-----------------------------------------------------------------------------------
bool ACS_RTR_SystemConfig::cs_isMultipleCPSystem() {
	bool isMultipleCPSystem = false;
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);
	
	fxSetCsError(result);
	if (result == ACS_CS_API_NS::Result_Success) return isMultipleCPSystem;
	else	return false;
}

//---------------------------------------------------------------------
// cs_getDefaultCPName
// Get the default name with a query on the Network Element Information
//---------------------------------------------------------------------
string ACS_RTR_SystemConfig::cs_getDefaultCPName(short id)
{
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);

		//access to configuration map
		if (!_configurationMap.empty()) {
			maptype::iterator it = _configurationMap.find(id);
			if (it != _configurationMap.end()) {
				string value = (*it).second;
				return value;
			}
		}
	}

	//access to configuration service
	string dname = "";
	if (_isBC)
	{
		CPID cpid(id);
		ACS_CS_API_Name CPName("");

		ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getDefaultCPName(cpid, CPName);
		fxSetCsError(result);
		
		if (result == ACS_CS_API_NS::Result_Success)
		{
			char name[256];
//			unsigned int length = CPName.length();//check the correct usage of this cs convenience class
			size_t length = sizeof(name);
			CPName.getName(name, length);
			dname = name;
			return dname;
		}
	}
	return dname;
}

//------------------------------------------------------------
// cs_getCPID
// Get the cp id with a query on the CP Name and Version Table
//------------------------------------------------------------
short ACS_RTR_SystemConfig::cs_getCPID(const char * name)
{
	if (name == 0) {
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s", _SYSCFG_DERR_BAD_PARAMETER);
		if (!setLastError(SYSCFG_ERR_BAD_PARAMETER, trace, true)) setLastError(SYSCFG_ERR_BAD_PARAMETER, _SYSCFG_DERR_BAD_PARAMETER);
		return -1;
	}

	//access to configuration map
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);

		if (!_configurationMap.empty()) {
			maptype::iterator it;
			for (it = _configurationMap.begin(); it != _configurationMap.end(); it++) {
				string value = (*it).second;
				if (strcmp(value.c_str(), name) == 0) {
					short key = (*it).first;
					return key;
				}
			}
		}
	}

	//access to configuration service
	ACS_CS_API_CP * CPNameVer = ACS_CS_API::createCPInstance();
	ACS_CS_API_Name CPName(name);
	CPID cpid;
	ACS_CS_API_NS::CS_API_Result result = CPNameVer->getCPId(CPName, cpid);
	ACS_CS_API::deleteCPInstance(CPNameVer);
	fxSetCsError(result);

	if (result == ACS_CS_API_NS::Result_Success) return cpid;
	else return -1;

}

//-------------------------------------------------
// cs_getCPName
// Get the CP Name on the CP Name and Version Table
//-------------------------------------------------
string ACS_RTR_SystemConfig::cs_getCPName(short id)
{
	string cpname = "";
	//access to configuration map
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);

		if (!_configurationMap.empty()) {
			maptype::iterator it;
			for (it = _configurationMap.begin(); it != _configurationMap.end(); it++) {
				short key = (*it).first;
				if (key == id) {
					string value = (*it).second;
					return value;
				}
			}
		}
	}

	//access to configuration service
	CPID cpid(id);
	ACS_CS_API_Name CPName;
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getDefaultCPName(cpid, CPName);  //TRHI55381
	
	fxSetCsError(result);

	if (result == ACS_CS_API_NS::Result_Success) {
//		unsigned int length1 = CPName.length();//check the correct usage of this cs convenience class
		char str[256] ;
		size_t length1 = sizeof(str);
		CPName.getName(str, length1);
		cpname = str;
	}

	return cpname;
}

//----------------------
// cs_getCPList
// get the list of cp id
//----------------------
std::list<short> ACS_RTR_SystemConfig::cs_getCPList()
{
	std::list<short> cplist;
	cplist.clear();
	if (_isBC) {
		ACS_CS_API_IdList idlist;
		ACS_CS_API_CP * CPNameVer = ACS_CS_API::createCPInstance();
		ACS_CS_API_NS::CS_API_Result result = CPNameVer->getCPList(idlist);//retrieve a list of all CP identities
		ACS_CS_API::deleteCPInstance(CPNameVer);
		fxSetCsError(result);
		
		if (result == ACS_CS_API_NS::Result_Success) {
			for (unsigned int i = 0; i < idlist.size(); i++) 
				try { cplist.push_back(idlist[i]); }
				catch(...) { //Called function throws an exception of type "class std::length_error"
					cplist.clear();
					return cplist;
				}
		}
	}
	return cplist;
}

//-------------------------
// dbgPrintConfigurationMap
//-------------------------
void ACS_RTR_SystemConfig::dbgPrintConfigurationMap() {
	printf("\n:::Internal System Configuration Table:::\n");
	_isBC ? printf("Multiple CP System\n") : printf("Single CP System\n");
	printf("KEY\tVALUE\n");
	for (maptype::iterator it = _configurationMap.begin(); it != _configurationMap.end(); it++) {
		short key = (*it).first;
		string value = (*it).second;
		printf("%u\t%s\n", key, value.c_str());
	}
	printf(":::::::::::::::::::::::::::::::::::::::::\n");
}
