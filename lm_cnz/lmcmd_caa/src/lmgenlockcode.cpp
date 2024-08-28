/*=================================================================== */
   /**
   @file lmgenlockcode.cpp

   This file contains the code for the lmgenlockcode command. This command
   is used to create Locking code for a given Fingerprint string. The generated
   Locking code is to be given to Supply team for generation of LicenseKey File.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11-NOV-2011   XCSSATA       Initial Release
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_lm_common.h"
#include <lsinit.h>
//#include "lsinit.h"
#include <lserv.h>
//#include "lserv.h"
#include<string>
#include<iostream>
using namespace std;

static std::string theSID;
static std::string fingerPrint;
static VLScustomEx customExList[VLS_MAX_CUSTOMEX_COUNT];
static int cached =0;
static int customExSize =0;


static void printUsage()
{
	std::cerr<< "lmgenlockcode FingerPrintString"<<std::endl;
}

static std::string  formatFingerPrint(const char* lockCode)
{
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
	return formattedFp;
}

static bool generateFingerPrint()
{

	//bool generated = false;
//	bool generated;
	::VLSmachineID machineID;
	//unsigned long actualMask;

	// Support for new style lock code
	char lockCode[VLS_LOCK_CODE_SIZE] = "";

	unsigned long actualMask;
	// Get the machine ID using  the API function
	if(::VLSgetMachineID(VLS_LOCK_CUSTOMEX, &machineID, &actualMask) == LS_SUCCESS)
	{
		if(::VLSmachineIDToLockCodeEx(&machineID, actualMask, lockCode, sizeof(lockCode), 0) == LS_SUCCESS)
		{
			//bool generated;
			char a[16] = {0};
			ACE_OS::itoa(actualMask,a,16);
			fingerPrint = a;
			fingerPrint += "-";
			fingerPrint += formatFingerPrint(lockCode);
			//bool generated = true;
		}
		else
		{
			cout<<endl<<"VLSmachineIDToLockCodeEx Failed "<<endl;
		}
	}
	else
	{
		cout<<endl<<"VLSgetMachineID Failed "<<endl;
	}

	return true;
}



static long getCustomExValue(VLScustomEx* pTable, unsigned long* pCount)
{
	// Frequently called function. Traces are therefore disabled for performance improvement
	char customEx[VLS_CUSTOMEX_SIZE + 1];
	int  i, len;
	// Recommend to cache custom ex values to speed. In SafeNet sample, cache behavior doens't make sense as
	// custom ex values are defined in static buffer. In order to explain how to cache data for developers,
	// cache behavior still is leaved behind.

	::memset(customEx, 0, sizeof(customEx));

	if(pCount == NULL)
	{
	   return 1;
	}

	if(!cached)
	{
		memset(customExList, 0, sizeof(customExList));
		len = (int)theSID.size();
		strncpy((char*)(customExList[0].customEx), theSID.c_str(), len);
		customExList[0].len = len;
		customExSize++;
		cached = 1;

	}

	if(customExSize == 0)
	{
		return 1;
	}

	// Return size of the buffer receive custom ex value.
	if(pTable == NULL)
	{
		*pCount = customExSize;
		return 0;
	}

	// The size of allocated buffer can't be less than customExSize
	if(*pCount < (unsigned int)customExSize)
	{
		*pCount = customExSize;
		return 1;
	}

	for(i = 0; i < customExSize; i++)
	{
		*(pTable + i) = customExList[i];
	}

	*pCount = customExSize;
	return 0;
}


static bool init()
{
	bool initialized = true;
	LS_STATUS_CODE lsStatusCode = LS_SUCCESS;
	std::string myStrNoNet("NO_NET");
	lsStatusCode = VLSsetContactServer((char*)myStrNoNet.c_str());
	if(lsStatusCode == LS_SUCCESS)
	{

	}
	else
	{
		cout<<endl<<"VLSsetContactServer Failed "<<endl;
		return false;
	}

	//To disable the Sentinle Error Popups and Printouts.
	VLSerrorHandle(VLS_OFF);

	lsStatusCode = VLScontrolRemoteSession(VLS_OFF);
	if(lsStatusCode == LS_SUCCESS)
	{

	}
	else
	{
		cout<<endl<<"VLScontrolRemoteSession Failed "<<endl;
		return false;
	}
	//Initializes the SentinelRMS LM
	lsStatusCode = VLSinitialize();
	if(lsStatusCode == LS_SUCCESS)
	{

	}
	else
	{
		cout<<endl<<"VLSinitialize Failed "<<endl;
		return false;
	}

	lsStatusCode = VLSsetCustomExFunc(&getCustomExValue);

	if(lsStatusCode == LS_SUCCESS)
	{
			initialized = true;
	}
	else
	{
		if (lsStatusCode == LS_NORESOURCES)
		{
			cout<<endl<<"VLSsetCustomExFunc returned LS_NORESOURCES "<<endl;
		}
		else if(lsStatusCode == VLS_RESOURCE_LOCK_FAILURE)
		{
			cout<<endl<<"VLSsetCustomExFunc returned VLS_RESOURCE_LOCK_FAILURE "<<endl;

		}
		initialized = false;
	}
	return initialized;

}
int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printUsage();
	}
	else
	{
		std::string mySIDString(argv[1]);
		theSID = mySIDString;
		bool myResult = init();
		if(myResult)
		{
			myResult =  generateFingerPrint();
			if(myResult)
			{
				cout<<endl<<"Locking Code is  "<<fingerPrint<<endl;
			}
			else
			{
				cout<<endl<<"Error in Generating the Locking Code"<<endl;
			}
		}
		else
		{
			cout<<endl<<"Error in Initializing Sentinel Library"<<endl;
		}


	}
	return 0;
}

