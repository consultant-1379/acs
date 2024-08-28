//********************************************************************************
//
// NAME
// acs_rtr_systemconfig.h
//
// COPYRIGHT Ericsson AB, 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-10 by XLANSRI
//
//********************************************************************************
#ifndef _ACS_RTR_SYSTEMCONFIG_H_
#define _ACS_RTR_SYSTEMCONFIG_H_

//#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>
#include <map>

#include <ace/ACE.h>
#include "ace/Task.h"
#include <ace/Recursive_Thread_Mutex.h>

#include "ACS_CS_API.h"

using namespace std;

class ACS_RTR_SystemConfig
{
	//-----------
	// data types
	//-----------
	typedef map<short, string> maptype;
	
	//----------
	// Constants
	//----------
public:
	static const int SYSCFG_NO_ERROR = 0; //NO error

	//Error constants
	static const int SYSCFG_ERR_NOENTRY_CS = -1;
	static const int SYSCFG_ERR_NOVALUE_CS = -2;
	static const int SYSCFG_ERR_NOACCESS_CS = -3;
	static const int SYSCFG_ERR_RESULT_FAILURE_CS = -3;
	static const int SYSCFG_ERR_UNKNOWN_CS = -4;
	static const int SYSCFG_ERR_BAD_PARAMETER = -5;
	static const int SYSCFG_ERR_NEW_INSTANCE = -6;
	static const int SYSCFG_ERR_INSERT_FAILED = -7;

	//------------
	// Constructor
	//------------
private:
	ACS_RTR_SystemConfig();

	//-----------
	// Destructor
	//-----------
private:
	~ACS_RTR_SystemConfig();

	//----------
	// Functions
	//----------
public:
	static ACS_RTR_SystemConfig* instance();
	bool init();
	inline static bool isInitialized() { return _initialized; }
	
	inline bool isMultipleCPSystem() { return _isBC; }
	inline size_t GetNumCP() { return _numCP; }

	string cs_getDefaultCPName(short id);
	short	cs_getCPID(const char* name);
	string cs_getCPName(short id);

	inline static int lastError() { return _lastError; }
	inline static const char * lastErrorDescription() { return _lastErrorDescription; }

	void dbgPrintConfigurationMap();
	
	bool isValidCPName(const char * cpName);
	bool isValidCPId(short id);
	
private:
	bool cs_isMultipleCPSystem();	
	std::list<short> cs_getCPList();
	std::list<string> getCP_List();
	std::list<short> getCP_IdList();
	
	static bool setLastError(int error = SYSCFG_NO_ERROR, const char * description = 0, bool allocate = false);
	int fxSetCsError(ACS_CS_API_NS::CS_API_Result result);

	//-------
	// Fields
	//-------
private:
	static ACS_RTR_SystemConfig	* _instance;
	static bool _initialized;
	bool _isBC;
	size_t _numCP;
	maptype _configurationMap; // [key = cpID, value = cpDefaultName]

	ACE_Recursive_Thread_Mutex _criticalSection;

	static int _lastError;
	static const char * _lastErrorDescription;
	static char * _buffer;
	static size_t _bufferSize;
};

#endif
