/*=================================================================== */
/**
@file acs_nsf_common.cpp

Class method implementationn for ACS_NSF_Common class and
other common utilities.

@version 1.0.0

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/06/2010   XCHVKUM     Initial Release
N/A       25/04/2014   XQUYDAO     Add ThreadID into log info
N/A       05/05/2014   XQUYDAO     Update for cable-less environment
**/
/*=================================================================== */

/*===================================================================
                INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_nsf_common.h>
#include <ACS_APGCC_Util.H>
#include <acs_prc_api.h>
#include <iostream>

#define ACS_NSF_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
using namespace std;

ACS_TRA_trace ACS_NSF_Common::ACS_NSF_InformationTrace("NSFINFO","C1024");
ACS_TRA_trace ACS_NSF_Common::ACS_NSF_DebugTrace("NSFDEBUG","C1024");
ACS_TRA_trace ACS_NSF_Common::ACS_NSF_WarningTrace("NSFWARNING","C1024");
ACS_TRA_trace ACS_NSF_Common::ACS_NSF_ErrorTrace("NSFError","C1024");
ACS_TRA_trace ACS_NSF_Common::ACS_NSF_FatalTrace("NSFFATAL","C1024");
ACS_TRA_Logging ACS_NSF_Common::ACS_NSF_Logging;
std::string ACS_NSF_Common::parentObjDNofNSF = "";

/*===================================================================
   ROUTINE: ACS_NSF_Common
=================================================================== */
ACS_NSF_Common :: ACS_NSF_Common() 
{
	//Don't add any IMM specific operation over here.
	// The start of NSF service wont work with HA mode.
	theActiveNodeFlag = false;
	theStartStopValue = 0;
	theInitialSetFlag = false;
	theCurrentNode = UNDEFINED;
	theFailOverSetFlag = false;
	isLoadingParamsRequired = false;
	failoverCount = 0;
	failoverPriority = 0;
	isSurveillanceActive = 0;
	isSurveillanceEnabled = 0;
	maxFailoverAttempts = 0;
	pingPeriod = 0;
	resetTime = 0;
	routerResponse = 0; 
}

/*===================================================================
   ROUTINE: ~ACS_NSF_Common
=================================================================== */
ACS_NSF_Common::~ACS_NSF_Common()
{
	//Add code to delete the files, if required.	
	theOmHandler.Finalize();
}

/*===================================================================
   ROUTINE: reportError
=================================================================== */
void ACS_NSF_Common::reportError(ACE_UINT32 specificProblem,string s1)
{
	DEBUG("reportError called %s", s1.c_str());

	string objRef("");
	string problemTxt("");

	switch (specificProblem)
	{
	case 1:
		objRef = "ICMP";
		problemTxt = "ICMP Problem";
		break;
	case 2:
		objRef = "NSF";
		problemTxt = "NSF ordered failover";
		break;
	case 3:
		objRef = "CONF";
		problemTxt = "Configuration problem";
		break;
	case 4: //TR:HG26029
		objRef = "TIMESTAMP";
		problemTxt = "NSF Time stamp File reading problem";
		break;
	case 5: //TR HK74557
		objRef = "NSF DIRECTORY";
		problemTxt = "Failed to create Directory";
		break;
	} // End of switch

} // End of reportError

/*===================================================================
   ROUTINE: reportError
=================================================================== */
void ACS_NSF_Common::reportError(ACE_UINT32 specificProblem,string s1,string s2)
{
	string tot = s1 + s2;
	reportError(specificProblem,tot);
} // End of reportError

/*===================================================================
   ROUTINE: reportError
=================================================================== */
void ACS_NSF_Common::reportError(ACE_UINT32 specificProblem, string s1, string s2, string s3)
{
	string tot = s1 + s2 +s3;
	reportError(specificProblem, tot);
}

/*===================================================================
   ROUTINE: reportError
=================================================================== */
void ACS_NSF_Common::reportError(ACE_UINT32 specificProblem,
								 string s1,
								 ACE_UINT32 dw2,
								 string s3)
{
	char str[16];
	sprintf(str, "%d", dw2);
	string tot = s1 + str + s3;
	reportError(specificProblem, tot);
}

/*===================================================================
   ROUTINE: reportError
=================================================================== */
void ACS_NSF_Common::reportError(ACE_UINT32 specificProblem,
								 string s1,
								 string s2,
								 string s3,
								 ACE_UINT32 s4)
{
	char help[16];
	sprintf(help,"%d",s4);
	string tot = s1 + s2 + s3 + help;
	reportError(specificProblem,tot);
} // End of reportError


/*===================================================================
   ROUTINE: reportError
=================================================================== */
void ACS_NSF_Common::reportError(ACE_UINT32 specificProblem,
								string s1,
								wstring s2,
								string s3,
								ACE_UINT32 s4)
{
	int len = s2.size() + 1;

	char* ts2 = new char[len];
	if( ts2 == 0  )
	{
		ERROR("%s", "Error occured while allocating memory for len");
		ERROR("%s", "Leaving reportError");
		return;
	}

	len = wcstombs(ts2,s2.data(),len);
	if (len < 0)
	{
		len = 0;
	}
	ts2[len] = 0;

	string ss2 = ts2;

	reportError(specificProblem,s1,ss2,s3,s4);

	delete [] ts2;
	ts2 = 0;
} // End of reportError


/*===================================================================
   ROUTINE: reportError
=================================================================== */
void ACS_NSF_Common::reportError(ACE_UINT32 specificProblem,string s1,ACE_UINT32 s2)
{
	reportError(specificProblem,s1,"","",s2);
} // End of reportError

/*===================================================================
   ROUTINE: setRegWord
=================================================================== */
int ACS_NSF_Common::setRegWord(string valueName, ACE_UINT32 val, char** msg)
{
	DEBUG("%s", "Entering setRegWord");

	if (valueName == NSF_SURVELLIENCEACTIVEFLAG)
	{
		ACS_CC_ImmParameter paramToChange ;

		paramToChange.attrName = (char*) NSF_SURVELLIENCEACTIVEFLAG ;
		paramToChange.attrType = ATTR_INT32T ;
		paramToChange.attrValuesNum = 1 ;
		if (msg != NULL)
		{
			delete[] *msg;
			*msg = new char[128];
		}

		void * values[1] = { reinterpret_cast<void*>(const_cast<unsigned int*>(&val)) };
		paramToChange.attrValues = values;
		//if ( theOmHandler.modifyAttribute( dnObject.c_str(), &paramToChange)  == ACS_CC_FAILURE )

		// start of TR HU34140
		// retry 3 times in case modifyAttribute() call fails

		int retryCount = 1;
		do {
			if (theOmHandler.modifyAttribute((ACS_NSF_Common::parentObjDNofNSF).c_str(), &paramToChange)  == ACS_CC_FAILURE)
			{
				ERROR("%s", "Error occurred while modifying NSF_SURVELLIENCEACTIVEFLAG");

				if(retryCount == 3) {					// if even the 3rd retry fails, then exit this function with proper error code
					int nExitCode = -1;
					string strErrorMessage;
					theOmHandler.getExitCode(nExitCode, strErrorMessage);
					ERROR("modifyAttribute() error code = %d, error text = %s", nExitCode, strErrorMessage.c_str());
					if (msg != NULL)
					{
						ACE_OS::strcpy(*msg, strErrorMessage.c_str());
					}
					ERROR("%s", "Exiting setRegWord");
					return nExitCode;
				}
			}
			else if (val == 1)
			{
				if (msg != NULL)
				{
					ACE_OS::strcpy(*msg, "Network surveillance started");
				}
				break;			// exit retry loop in case modifyAttribute() call was successful
			}
			else if (val == 0)
			{
				if (msg != NULL)
				{
					ACE_OS::strcpy(*msg, "Network surveillance stopped");
				}
				break;			// exit retry loop in case modifyAttribute() call was successful
			}

			sleep(1);			// sleep for 1 second before calling modifyAttribute() again
			++retryCount;
		} while(retryCount <= 3);
		// end of TR HU34140

		DEBUG("%s", "Exiting setRegWord");
		return 0;
	}
	else if (valueName == NSF_ALARMSTATE_A)
	{
		theConfigFileMutex.acquire();
		ifstream infile (NSF_CONFIG_FILE_NAME_A, ios::in|ios::binary|ios::ate);

		ACE_UINT32 myAlarmStateA = 0;
		ACE_UINT32 myAlarmState2A = 0;
		ACE_UINT32 myLastFailoverTimeA = 0;

		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char * ptrmemblock = new char [size];
			if (ptrmemblock == 0)
			{
				ERROR("%s", "Error occurred while modifying alarmStateA.");
				ERROR("%s", "Exiting setRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(ptrmemblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(ptrmemblock, size);
			infile.close();

			char *tmp  = 0;
			int count = 0;
			tmp = strtok(ptrmemblock, ";");

			while (count < 3)
			{
				if (tmp != 0)
				{
					std::stringstream ss;
					ss << std::hex << tmp;
					switch (count)
					{
						case 0:
							ss >> myAlarmStateA;
							break;
						case 1:
							ss >> myAlarmState2A;
							break;
						case 2:
							ss >> myLastFailoverTimeA;
							break;
					}
					tmp = strtok(0, ";");
					count++;
				}
			}
			delete[] ptrmemblock;
			ptrmemblock = 0;
		}
		//Update the value of alarm state A.
		myAlarmStateA = val;
		DEBUG("New value for alarmState A = %u", myAlarmStateA);

		//Write the data into the file.
		ofstream outfile;
		outfile.open(NSF_CONFIG_FILE_NAME_A, ios::binary | ios::out);
		if (outfile.is_open())
		{
			ifstream::pos_type size = 1024;
			char * ptrmemblock = new char [size];
			if (ptrmemblock)
			{
				memset(ptrmemblock, 0 , sizeof(char)*size);
				sprintf(ptrmemblock, "%x;%x;%x;", myAlarmStateA, myAlarmState2A, myLastFailoverTimeA);
				outfile.write(ptrmemblock, size);
				outfile.close();
				delete[] ptrmemblock;
				ptrmemblock = 0;

				theConfigFileMutex.release();
				DEBUG("%s", "Exiting setRegWord");
				return 0;
			}
		}
		outfile.close();
		ERROR("%s", "Error occurred while modifying alarmStateA.");
		ERROR("%s", "Exiting setRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_ALARMSTATE_NET2_A)
	{
		theConfigFileMutex.acquire();
		ifstream infile (NSF_CONFIG_FILE_NAME_A, ios::in|ios::binary|ios::ate);

		ACE_UINT32 myAlarmStateA = 0;
		ACE_UINT32 myAlarmState2A = 0;
		ACE_UINT32 myLastFailoverTimeA = 0;

		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char * ptrmemblock = new char [size];
			if (ptrmemblock == 0)
			{
				ERROR("%s", "Error occurred while modifying alarmState2A.");
				ERROR("%s", "Exiting setRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(ptrmemblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(ptrmemblock, size);
			infile.close();

			char *tmp  = 0;
			int count = 0;
			tmp = strtok(ptrmemblock, ";");

			while (count < 3)
			{
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					switch (count)
					{
						case 0:
							ss >> myAlarmStateA;
							break;
						case 1:
							ss >> myAlarmState2A;
							break;
						case 2:
							ss >> myLastFailoverTimeA;
							break;
					}
					tmp = strtok(0, ";");
					count++;
				}
			}
			delete[] ptrmemblock;
			ptrmemblock = 0;
		}
		// Update the value of alarm state 2A.
		myAlarmState2A = val;
		DEBUG("New value for alarmState2A = %u", myAlarmState2A);

		//Write the data into the file.
		ofstream outfile;
		outfile.open(NSF_CONFIG_FILE_NAME_A, ios::binary | ios::out);
		if (outfile.is_open())
		{
			ifstream::pos_type size = 1024;
			char * ptrmemblock = new char [size];
			if (ptrmemblock)
			{
				memset(ptrmemblock, 0, sizeof(char)*size);
				sprintf(ptrmemblock, "%x;%x;%x;", myAlarmStateA, myAlarmState2A, myLastFailoverTimeA);
				outfile.write(ptrmemblock, size);
				outfile.close();
				delete[] ptrmemblock;
				ptrmemblock = 0;

				theConfigFileMutex.release();
				DEBUG("%s", "Exiting setRegWord");
				return 0;
			}
		}
		outfile.close();
		ERROR("%s", "Error occurred while modifying alarmState2A.");
		ERROR("%s", "Exiting setRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_ALARMSTATE_B)
	{
		theConfigFileMutex.acquire();
		ifstream infile(NSF_CONFIG_FILE_NAME_B, ios::in|ios::binary|ios::ate);

		ACE_UINT32 myAlarmStateB = 0;
		ACE_UINT32 myAlarmState2B = 0;
		ACE_UINT32 myLastFailoverTimeB = 0;

		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char * ptrmemblock = new char [size];
			if (ptrmemblock == 0)
			{
				ERROR("%s", "Error occurred while modifying alarmStateB.");
				ERROR("%s", "Exiting setRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(ptrmemblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(ptrmemblock, size);
			infile.close();

			char *tmp  = 0;
			int count = 0;
			tmp = strtok(ptrmemblock, ";");

			while (count < 3)
			{
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					switch (count)
					{
						case 0:
							ss >> myAlarmStateB;
							break;
						case 1:
							ss >> myAlarmState2B;
							break;
						case 2:
							ss >> myLastFailoverTimeB;
							break;
					}
					tmp = strtok(0, ";");
					count++;
				}
			}
			delete[] ptrmemblock;
			ptrmemblock = 0;
		}
		// Update the value of alarm state B.
		myAlarmStateB = val;
		DEBUG("New value for alarmStateB = %d", myAlarmStateB );

		// Write the data into the file.
		ofstream outfile;
		outfile.open(NSF_CONFIG_FILE_NAME_B, ios::binary | ios::out);
		if (outfile.is_open())
		{
			ifstream::pos_type size = 1024;
			char * ptrmemblock = new char [size];
			if (ptrmemblock)
			{
				memset(ptrmemblock, 0, sizeof(char)*size);

				sprintf(ptrmemblock, "%x;%x;%x;", myAlarmStateB, myAlarmState2B, myLastFailoverTimeB);
				outfile.write(ptrmemblock, size);
				outfile.close();
				delete[] ptrmemblock;
				ptrmemblock = 0;

				theConfigFileMutex.release();
				DEBUG("%s", "Exiting setRegWord");
				return 0;
			}
		}
		outfile.close();
		ERROR("%s", "Error occurred while modifying alarmStateB.");
		ERROR("%s", "Exiting setRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_ALARMSTATE_NET2_B)
	{
		theConfigFileMutex.acquire();
		ifstream infile(NSF_CONFIG_FILE_NAME_B, ios::in|ios::binary|ios::ate);

		ACE_UINT32 myAlarmStateB = 0;
		ACE_UINT32 myAlarmState2B = 0;
		ACE_UINT32 myLastFailoverTimeB = 0;

		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char * ptrmemblock = new char [size];
			if (ptrmemblock == 0)
			{
				ERROR("%s", "Error occurred while modifying alarmState2B.");
				ERROR("%s", "Exiting setRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(ptrmemblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(ptrmemblock, size);
			infile.close();

			char *tmp  = 0;
			int count = 0;
			tmp = strtok(ptrmemblock, ";");

			while (count < 3)
			{
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					switch (count)
					{
						case 0:
							ss >> myAlarmStateB;
							break;
						case 1:
							ss >> myAlarmState2B;
							break;
						case 2:
							ss >> myLastFailoverTimeB;
							break;
					}
					tmp = strtok(0, ";");
					count++;
				}
			}
			delete[] ptrmemblock;
			ptrmemblock = 0;
		}
		// Update the value of alarm state2B.
		myAlarmState2B = val;
		DEBUG("New value for alarmState2B = %u", myAlarmState2B );

		// Write the data into the file.
		ofstream outfile;
		outfile.open(NSF_CONFIG_FILE_NAME_B, ios::binary | ios::out);
		if (outfile.is_open())
		{
			ifstream::pos_type size = 1024;
			char * ptrmemblock = new char [size];
			if (ptrmemblock)
			{
				memset(ptrmemblock, 0, sizeof(char)*size);

				sprintf(ptrmemblock, "%x;%x;%x;", myAlarmStateB, myAlarmState2B, myLastFailoverTimeB);
				outfile.write(ptrmemblock, size);
				outfile.close();
				delete[] ptrmemblock;
				ptrmemblock = 0;

				theConfigFileMutex.release();
				DEBUG("%s", "Exiting setRegWord");
				return 0;
			}
		}
		outfile.close();
		ERROR("%s", "Error occurred while modifying alarmState2B.");
		ERROR("%s", "Exiting setRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_NOOFFAILOVER)
	{
		ACS_CC_ImmParameter paramToChange ;
		paramToChange.attrName = (char*) NSF_NOOFFAILOVER ;
		paramToChange.attrType = ATTR_UINT32T ;
		paramToChange.attrValuesNum = 1 ;

		void * values[1] = { reinterpret_cast<void*>(const_cast<unsigned int*>(&val)) };
		paramToChange.attrValues = values ;
		
		std::string nsfParamDN = ACS_NSF_PARAM_OBJ_DN;
		nsfParamDN.append(",");
		nsfParamDN.append(ACS_NSF_Common::parentObjDNofNSF);
		
		OmHandler myOmHandler;
		if (myOmHandler.Init() == ACS_CC_FAILURE)
		{
			ERROR("%s","OmHandler initialization is failed!!");
			return 1;
		}
		//if ( theOmHandler.modifyAttribute( nsfParamDN.c_str(), &paramToChange ) == ACS_CC_FAILURE )
		if (myOmHandler.modifyAttribute( nsfParamDN.c_str(), &paramToChange ) == ACS_CC_FAILURE)
		{
			ERROR("%s", "Error occurred while modifying NSF_NOOFFAILOVER");
			ERROR("%s", "Exiting setRegWord");
			return 1;
		}
		DEBUG("NSF_NOOFFAILOVER value set to %d ",val);
		myOmHandler.Finalize();
		DEBUG("%s", "Exiting setRegWord");
		return 0;
	}
	else if (valueName == NSF_CBL_BONDINGSTATE_A || valueName == NSF_CBL_BONDINGSTATE_B || NSF_CBL_NUMOFTIMES_A || NSF_CBL_NUMOFTIMES_B)
	{
		theConfigFileMutex.acquire();
		string fileName = "";
		if (valueName == NSF_CBL_BONDINGSTATE_A || valueName == NSF_CBL_NUMOFTIMES_A)
		{
			fileName = NSF_CBL_CONFIG_FILE_NAME_A;
		}
		else
		{
			fileName = NSF_CBL_CONFIG_FILE_NAME_B;
		}
		ifstream infile(fileName.c_str(), ios::in|ios::binary|ios::ate);

		ACE_UINT32 pingState = 0;
		ACE_UINT32 numOfTime = 0;
		ACE_UINT32 lastFailOverTime = 0;

		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char * ptrmemblock = new char [size];
			if (ptrmemblock == 0)
			{
				ERROR("Error occurred while modifying %s", valueName.c_str());
				ERROR("%s", "Exiting setRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(ptrmemblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(ptrmemblock, size);
			infile.close();

			char *tmp  = 0;
			int count = 0;
			tmp = strtok(ptrmemblock, ";");

			while (count < 3)
			{
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					switch (count)
					{
						case 0:
							ss >> pingState;
							break;
						case 1:
							ss >> numOfTime;
							break;
						case 2:
							ss >> lastFailOverTime;
							break;
					}
					tmp = strtok(0, ";");
					count++;
				}
			}
			delete[] ptrmemblock;
			ptrmemblock = 0;
		}
		// Update the value 
		if (valueName == NSF_CBL_BONDINGSTATE_A || valueName == NSF_CBL_BONDINGSTATE_B)
		{
			pingState = val;
		}
		else if (valueName == NSF_CBL_NUMOFTIMES_A || valueName == NSF_CBL_NUMOFTIMES_B)
		{
			numOfTime = val;
		}
		DEBUG("New value for %s = %u", valueName.c_str(), val);		

		// Write the data into the file.
		ofstream outfile;
		outfile.open(fileName.c_str(), ios::binary | ios::out);
		if (outfile.is_open())
		{
			ifstream::pos_type size = 1024;
			char * ptrmemblock = new char [size];
			if (ptrmemblock)
			{
				memset(ptrmemblock, 0, sizeof(char)*size);

				sprintf(ptrmemblock, "%x;%x;%x;", pingState, numOfTime, lastFailOverTime);
				outfile.write(ptrmemblock, size);
				outfile.close();
				delete[] ptrmemblock;
				ptrmemblock = 0;

				theConfigFileMutex.release();
				DEBUG("%s", "Exiting setRegWord");
				return 0;
			}
		}
		outfile.close();
		ERROR("Error occurred while modifying %s", valueName.c_str());
		ERROR("%s", "Exiting setRegWord");
		theConfigFileMutex.release();
		return 1;		
	}
	else
	{
		ERROR("%s", "Invalid parameter provided in setRegWord");
		ERROR("%s", "Exiting setRegWord" );
		return -2;
	}
	DEBUG("%s", "Exiting setRegWord");
	return 0;
} // End of setRegWord

/*===================================================================
   ROUTINE: getRegWord
=================================================================== */
int ACS_NSF_Common::getRegWord(string valueName, ACE_UINT32 &val)
{
	//DEBUG("%s", "Entering getRegWord");

	if ((valueName == NSF_SURVELLIENCEACTIVEFLAG))
	{
		ACS_CC_ImmParameter paramToFind;
		paramToFind.attrName = (char*) NSF_SURVELLIENCEACTIVEFLAG;

		//DEBUG("DN: %s PARAM: %s", dnObject.c_str(), paramToFind.attrName); 
		//DEBUG("DN: %s PARAM: %s", (ACS_NSF_Common::parentObjDNofNSF).c_str(), paramToFind.attrName); 

		if (theOmHandler.getAttribute((ACS_NSF_Common::parentObjDNofNSF).c_str(), &paramToFind) == ACS_CC_SUCCESS)
		{
			val = *( reinterpret_cast < int *> (*(paramToFind.attrValues)));
			DEBUG("NSF_SURVELLIENCEACTIVEFLAG : %d", val);
		}
		else
		{
			ERROR("%s", "Fetching of NSF_SURVELLIENCEACTIVEFLAG from IMM Failed.");
			ERROR("%s", "Exiting getRegWord");
			return 1;
		}
	//	DEBUG("%s", "Exiting getRegWord");
		return 0;
	}
	else if (valueName == NSF_ALARMSTATE_A)
	{
		theConfigFileMutex.acquire();

		ifstream infile;
		infile.open(NSF_CONFIG_FILE_NAME_A, ios::in|ios::binary|ios::ate);
		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char *memblock = new char [size];
			if (memblock == 0)
			{
				ERROR("%s", "Error occured while reading alarmStateA.");
				ERROR("%s", "Exiting getRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(memblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(memblock, size);
			infile.close();

			char *tmp  = 0;
			tmp = strtok(memblock, ";");
			if (tmp != 0)
			{
				stringstream ss;
				ss << std::hex << tmp;
				ss >> val;
				INFO("IMM alarmstateA in getRegWord is %u", val);
				delete[] memblock;
				memblock = 0;
				theConfigFileMutex.release();
				//	DEBUG("%s", "Exiting getRegWord");
				return 0;
			}
		}
		ERROR("%s", "Error occurred while reading alarmStateA.");
		ERROR("%s", "Exiting getRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_ALARMSTATE_B)
	{
		theConfigFileMutex.acquire();

		ifstream infile;
		infile.open(NSF_CONFIG_FILE_NAME_B, ios::in|ios::binary|ios::ate);
		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char *memblock = new char [size];
			if (memblock == 0)
			{
				ERROR("%s", "Error occured while reading alarmStateB.");
				ERROR("%s", "Exiting getRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset( memblock, 0, sizeof(char)*size);

			infile.seekg (0, ios::beg);
			infile.read (memblock, size);
			infile.close();

			char *tmp  = 0;
			tmp = strtok( memblock, ";");
			if (tmp != 0)
			{
				stringstream ss;
				ss << std::hex << tmp;
				ss >> val;
				INFO("IMM alarmstateB in getRegWord is %u", val);
				delete[] memblock;
				memblock = 0;
				theConfigFileMutex.release();
				//DEBUG("%s", "Exiting getRegWord");
				return 0;
			}
			delete[] memblock;
			memblock = 0;
		}
		ERROR("%s", "Error occurred while reading alarmStateB.");
		ERROR("%s", "Exiting getRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_ALARMSTATE_NET2_A)
	{
		theConfigFileMutex.acquire();

		ifstream infile;
		infile.open(NSF_CONFIG_FILE_NAME_A, ios::in|ios::binary|ios::ate);
		if( infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char *memblock = new char [size];
			if (memblock == 0)
			{
				ERROR("%s", "Error occurred while reading alarmState2A.");
				ERROR("%s", "Exiting getRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(memblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(memblock, size);
			infile.close();

			char *tmp  = 0;
			tmp = strtok(memblock, ";");
			if (tmp != 0)
			{
				tmp = strtok(0, ";");
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					ss >> val;
					INFO("IMM alarmstate2A in getRegWord is %u", val);
					delete[] memblock;
					memblock = 0;
					theConfigFileMutex.release();
					//DEBUG("%s", "Exiting getRegWord");
					return 0;
				}
			}
			delete[] memblock;
			memblock = 0;
		}
		ERROR("%s", "Error occurred while reading alarmState2A.");
		ERROR("%s", "Exiting getRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_ALARMSTATE_NET2_B)
	{
		theConfigFileMutex.acquire();

		ifstream infile;
		infile.open(NSF_CONFIG_FILE_NAME_B, ios::in|ios::binary|ios::ate);
		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char *memblock = new char [size];
			if (memblock == 0)
			{
				ERROR("%s", "Error occurred while reading alarmState2B.");
				ERROR("%s", "Exiting getRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset( memblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(memblock, size);
			infile.close();

			char *tmp  = 0;
			tmp = strtok(memblock, ";");
			if (tmp != 0)
			{
				tmp = strtok(0, ";");
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					ss >> val;
					INFO("IMM alarmstate2B in getRegWord is %u", val);
					delete[] memblock;
					memblock = 0;
					theConfigFileMutex.release();
					//DEBUG("%s", "Exiting getRegWord");
					return 0;
				}
			}
			delete[] memblock;
			memblock = 0;
		}
		ERROR("%s", "Error occurred while reading alarmState2B.");
		ERROR("%s", "Exiting getRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	else if (valueName == NSF_NOOFFAILOVER)
	{
		ACS_CC_ImmParameter paramToFind ;

		paramToFind.attrName = (char*) NSF_NOOFFAILOVER ;
		
		std::string nsfParamDN = ACS_NSF_PARAM_OBJ_DN;
		nsfParamDN.append(",");
		nsfParamDN.append(ACS_NSF_Common::parentObjDNofNSF);
		DEBUG("DN: %s PARAM: %s", nsfParamDN.c_str(), paramToFind.attrName); 
		if (theOmHandler.getAttribute(nsfParamDN.c_str(), &paramToFind)  ==  ACS_CC_SUCCESS)
		{
			val = *( reinterpret_cast < ACE_UINT32 *> (*(paramToFind.attrValues))) ;
			INFO("No of Failovers are : %d", val );
			return 0;
		}
		else
		{
			ERROR("%s", "Fetching of NSF_NOOFFAILOVER from IMM Failed.");
			ERROR("%s", "Exiting getRegWord");
			return 1;
		}
	}
	else if (valueName == NSF_CBL_BONDINGSTATE_A || valueName == NSF_CBL_BONDINGSTATE_B || NSF_CBL_NUMOFTIMES_A || NSF_CBL_NUMOFTIMES_B)
	{
		theConfigFileMutex.acquire();

		ifstream infile;
		string fileName = "";
		if (valueName == NSF_CBL_BONDINGSTATE_A || valueName == NSF_CBL_NUMOFTIMES_A)
		{
			fileName = NSF_CBL_CONFIG_FILE_NAME_A;
		}
		else
		{
			fileName = NSF_CBL_CONFIG_FILE_NAME_B;
		}
		infile.open(fileName.c_str(), ios::in|ios::binary|ios::ate);
		if (infile.is_open())
		{
			ifstream::pos_type size = infile.tellg();
			char *memblock = new char [size];
			if (memblock == 0)
			{
				ERROR("%s", "Error occurred while reading alarmStateA.");
				ERROR("%s", "Exiting getRegWord");
				infile.close();
				theConfigFileMutex.release();
				return 1;
			}
			memset(memblock, 0, sizeof(char)*size);

			infile.seekg(0, ios::beg);
			infile.read(memblock, size);
			infile.close();

			char *tmp  = 0;
			tmp = strtok(memblock, ";");
			if (tmp != 0)
			{
				if (valueName == NSF_CBL_NUMOFTIMES_A || valueName == NSF_CBL_NUMOFTIMES_B)
				{
					tmp = strtok(0, ";");
					if (tmp == 0)
					{
						ERROR("Error occurred while reading %s in file %s", valueName.c_str(), fileName.c_str());
						return 1;
					}
				}
				stringstream ss;
				ss << std::hex << tmp;
				ss >> val;
				INFO("IMM %s in getRegWord is %lu", valueName.c_str(), val);
				delete[] memblock;
				memblock = 0;
				theConfigFileMutex.release();
				//	DEBUG("%s", "Exiting getRegWord");
				return 0;
			}
		}
		ERROR("Error occurred while reading %s", valueName.c_str());
		ERROR("%s", "Exiting getRegWord");
		theConfigFileMutex.release();
		return 1;
	}
	
	return 1;
} // End of getRegWord

/*===================================================================
   ROUTINE: setStartStopValue
=================================================================== */
void ACS_NSF_Common::setStartStopValue(unsigned int aStartStopValue)
{
	DEBUG("%s", "Entering setStartStopValue");

	theStartStopValue = aStartStopValue;

	DEBUG("%s", "Exiting setStartStopValue");

}

/*===================================================================
   ROUTINE: getStartStopValue
=================================================================== */
unsigned int ACS_NSF_Common::getStartStopValue()
{
	return theStartStopValue;
}

/*===================================================================
   ROUTINE: getNode
=================================================================== */
NODE_NAME ACS_NSF_Common::getNode()
{
	//DEBUG("%s", "Entering getNode");

	#if 0

	ACS_DSD_Client myDSDClient;
	ACS_DSD_Node DSDNode;

	ACE_TCHAR curNode[32];

	int result = myDSDClient.get_local_node(DSDNode);
	if( result < 0 )
	{
		ERROR("%s", "Error occured while getting node name");
		ERROR("%s", "Exiting getNode");
		return UNDEFINED;
	}

	ACE_OS::strcpy( curNode, DSDNode.node_name);

	if(curNode[3] == 'A')
	{
		DEBUG("%s", "Node A");
		DEBUG("%s", "Exiting getNode");
		return NODE_A;
	}
	else if(curNode[3] == 'B')
	{
		DEBUG("%s", "Node B");
		DEBUG("%s", "Exiting getNode");
		return NODE_B;
	}
	else
	{
		DEBUG("%s", "Undefined Node Name");
		ERROR("%s", "Exiting getNode");
		return UNDEFINED;
	}
	#endif
	FILE *fp = 0;
        char buff[512];
        int nodeId  = 0;

        /* popen creates a pipe so we can read the output
        of the program we are invoking */
        if (!(fp = popen("cat /etc/opensaf/slot_id" ,"r")))
        {
                ERROR("%s", "Error while reading from popen in getNode");
                return UNDEFINED;
        }

        /* read the output of route, one line at a time */
        if(fgets(buff, sizeof(buff), fp) != NULL )
        {
                nodeId = atoi(&buff[0]);
        }
        /* close the pipe */
        pclose(fp);

	if (nodeId == 1)
	{
	//	DEBUG("%s", "Exiting getNode");
		return NODE_A;
	}
	else if (nodeId == 2)
	{
	//	DEBUG("%s", "Exiting getNode");
		return NODE_B;
	}
//	DEBUG("%s", "Exiting getNode");
	return UNDEFINED;

}


/*===================================================================
   ROUTINE: getRegTime
=================================================================== */
int ACS_NSF_Common::getRegTime(string valueName, ACE_UINT32 &diffTime)
{
	DEBUG("%s", "Entering getRegTime");

	// Fetching the current system time in Unix Epoch format
	time_t sysTime;
	sysTime = time(NULL);

	int systemTime = (uintmax_t)sysTime;
	int currentNodeTime = 0 , otherNodeTime = 0;

	int immTime = 0 ;

	if (valueName != NSF_LASTFAILOVERTIME)
	{
		ERROR("Invalid Attribute name. Expecting %s", NSF_LASTFAILOVERTIME);
		ERROR("%s", "Exiting getRegTime");
		return -1;
	}
	theConfigFileMutex.acquire();

	ACE_UINT32 myLastFailoverTimeA = 0;
	ACE_UINT32 myLastFailoverTimeB = 0;
	bool isLastFailoverTimeAPresent = false;
	bool isLastFailoverTimeBPresent = false;

	ifstream infile;
	infile.open((GetApgOamAccess() == 1) ? NSF_CBL_CONFIG_FILE_NAME_A : NSF_CONFIG_FILE_NAME_A, ios::in | ios::binary | ios::ate);
	
	if (infile.is_open())
	{
		fstream::pos_type size = infile.tellg();
		char *memblock = new char [size];
		if (memblock == 0)
		{
			ERROR("%s", "Error occurred while reading lastFailoverTimeA.");
			ERROR("%s", "Exiting getRegTime");
			infile.close();
			theConfigFileMutex.release();
			return 1;
		}
		memset(memblock, 0, sizeof(char)*size);

		infile.seekg(0, ios::beg);
		infile.read(memblock, size);
		infile.close();

		char *tmp  = 0;
		tmp = strtok(memblock, ";");
		if (tmp != 0) 
		{
			tmp = strtok(0, ";");
			if (tmp != 0)
			{
				tmp = strtok(0, ";");
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					ss >> myLastFailoverTimeA;
					isLastFailoverTimeAPresent = true;	
				}
			}
		}
		delete[] memblock;
		memblock = 0;		
	}
	infile.close();

	infile.open((GetApgOamAccess() == 1) ? NSF_CBL_CONFIG_FILE_NAME_B : NSF_CONFIG_FILE_NAME_B, ios::in | ios::binary | ios::ate);
	
	if (infile.is_open())
	{
		fstream::pos_type size = infile.tellg();
		char *memblock = new char [size];
		if (memblock == 0)
		{
			ERROR("%s", "Error occurred while reading lastFailoverTimeB.");
			ERROR("%s", "Exiting getRegTime");
			infile.close();
			theConfigFileMutex.release();
			return 1;
		}
		memset(memblock, 0, sizeof(char)*size);

		infile.seekg (0, ios::beg);
		infile.read (memblock, size);
		infile.close();

		char *tmp  = 0;
		tmp = strtok( memblock, ";");
		if (tmp != 0) 
		{
			tmp = strtok(0, ";");
			if (tmp != 0)
			{
				tmp = strtok(0, ";");
				if (tmp != 0)
				{
					stringstream ss;
					ss << std::hex << tmp;
					ss >> myLastFailoverTimeB;
					isLastFailoverTimeBPresent = true;	
				}
			}
		}
		delete[] memblock;
		memblock = 0;
	}

	if (theCurrentNode == NODE_A)
	{
		if (isLastFailoverTimeAPresent == false)
		{
			ERROR("%s", "Error occurred while fetching lastFailoverTime from current node");
			ERROR("%s", "Exiting getRegTime");
			theConfigFileMutex.release();
			return 1;
		}

		if (isLastFailoverTimeBPresent == false)
		{
			ERROR("%s", "Error occurred while fetching lastFailoverTime from other node");
			ERROR("%s", "Exiting getRegTime");
			theConfigFileMutex.release();
			return 1;
		}

		currentNodeTime = myLastFailoverTimeA;
		otherNodeTime = myLastFailoverTimeB;		
	}
	else if (theCurrentNode == NODE_B)
	{
		if (isLastFailoverTimeBPresent == false)
		{
			ERROR("%s", "Error occurred while fetching lastFailoverTime from current node");
			ERROR("%s", "Exiting getRegTime");
			theConfigFileMutex.release();
			return 1;
		}

		if (isLastFailoverTimeAPresent == false)
		{
			ERROR("%s", "Error occured while fetching lastFailoverTime from other node");
			ERROR("%s", "Exiting getRegTime");
			theConfigFileMutex.release();
			return 1;
		}

		currentNodeTime  = myLastFailoverTimeB;
		otherNodeTime = myLastFailoverTimeA;
	}
	INFO("LastFailover time in current node is %d", currentNodeTime);
	INFO("LastFailover time in Other node is %d", otherNodeTime );

	if (otherNodeTime > currentNodeTime)
	{
		immTime = otherNodeTime;
               
		// TR HQ57433 BEGIN 
		// modifying the last failover time to current time is casuing reset is not happening
		// in 60 mins. This in turn causing continous chages on both nodes. This change is not 
		// present in design base code.
		//	setRegTime(NSF_LASTFAILOVERTIME);
		// TR HQ57433 END
	}
	else
	{
		immTime = currentNodeTime;
	}
	uintmax_t fileTime = (uintmax_t)(immTime);
	int mydiffTime = systemTime - fileTime ;

	INFO("Difference in time is %d", mydiffTime);

	diffTime = abs(mydiffTime);

	DEBUG("%s", "Exiting getRegTime");
	theConfigFileMutex.release();
	return 0;
} // End of getRegTime

/*===================================================================
   ROUTINE: setRegTime
=================================================================== */
int ACS_NSF_Common::setRegTime(string valueName)
{
	DEBUG("%s", "Entering setRegTime");

	if (valueName != NSF_LASTFAILOVERTIME)
	{
		ERROR("%s", "valueName is not valid");
		ERROR("%s", "Leaving setRegTime");
		return 1;
	}

	// Fetching the current system time in Unix Epoch format
	time_t sysTime;

	sysTime = time(NULL);

	int systemTime = (uintmax_t)sysTime;
	int otherNodeTime = 0;

	ACE_UINT32 myLastFailoverTimeA = 0;
	ACE_UINT32 myLastFailoverTimeB = 0;
	ACE_UINT32 myAlarmStateA = 0;
	ACE_UINT32 myAlarmState2A = 0;
	ACE_UINT32 myAlarmStateB = 0;
	ACE_UINT32 myAlarmState2B = 0;
	bool isLastFailoverTimeAPresent = false;
	bool isLastFailoverTimeBPresent = false;

	theConfigFileMutex.acquire();

	ifstream infile;	
	infile.open((GetApgOamAccess() == 1) ? NSF_CBL_CONFIG_FILE_NAME_A : NSF_CONFIG_FILE_NAME_A, ios::in | ios::binary | ios::ate);
	
	if (infile.is_open())
	{
		fstream::pos_type size = infile.tellg();
		char *memblock = new char [size];
		if (memblock == 0)
		{
			ERROR("%s", "Error occurred while reading lastFailoverTimeA.");
			ERROR("%s", "Exiting getRegTime");
			infile.close();
			theConfigFileMutex.release();
			return 1;
		}
		memset(memblock, 0, sizeof(char)*size);

		infile.seekg(0, ios::beg);
		infile.read(memblock, size);
		infile.close();

		char *tmp  = 0;
		tmp = strtok( memblock, ";");
		if (tmp != 0) 
		{
			stringstream ss;
			ss << std::hex << tmp;
			ss >> myAlarmStateA;
			tmp = strtok(0, ";");
			if (tmp != 0)
			{
				ss << std::hex << tmp;
				ss >> myAlarmState2A;

				tmp = strtok(0, ";");
				if (tmp != 0)
				{
					ss << std::hex << tmp;
					ss >> myLastFailoverTimeA;
					isLastFailoverTimeAPresent = true;	
				}
			}
		}
		delete[] memblock;
		memblock = 0;
	}
	infile.close();
	
	infile.open((GetApgOamAccess() == 1) ? NSF_CBL_CONFIG_FILE_NAME_B :NSF_CONFIG_FILE_NAME_B, ios::in | ios::binary | ios::ate);
	
	if (infile.is_open())
	{
		fstream::pos_type size = infile.tellg();
		char *memblock = new char [size];
		if (memblock == 0)
		{
			ERROR("%s", "Error occurred while reading lastFailoverTimeB.");
			ERROR("%s", "Exiting getRegTime");
			infile.close();
			theConfigFileMutex.release();
			return 1;
		}
		memset(memblock, 0, sizeof(char)*size);

		infile.seekg(0, ios::beg);
		infile.read(memblock, size);
		infile.close();

		char *tmp  = 0;
		tmp = strtok(memblock, ";");
		if (tmp != 0)
		{
			stringstream ss;
			ss << std::hex << tmp;
			ss >> myAlarmStateB;
			tmp = strtok(0, ";");
			if (tmp != 0)
			{
				ss << std::hex << tmp;
				ss >> myAlarmState2B;
				tmp = strtok(0, ";");
				if (tmp != 0)
				{
					ss << std::hex << tmp;
					ss >> myLastFailoverTimeB;
					isLastFailoverTimeBPresent = true;	
				}
			}
		}
		delete[] memblock;
		memblock = 0;
	}
	
	if (theCurrentNode == NODE_A)
	{
		if (isLastFailoverTimeBPresent == false)
		{
			ERROR("%s", "Error occurred while reading last failover time in other node");
			ERROR("%s", "Exiting getRegTime");
			theConfigFileMutex.release();
			return 1;
		}
		otherNodeTime = myLastFailoverTimeB;
	}
	else if (theCurrentNode == NODE_B)
	{
		if (isLastFailoverTimeAPresent == false)
		{
			ERROR("%s", "Error occurred while reading last failover time in other node");
			ERROR("%s", "Exiting getRegTime");
			theConfigFileMutex.release();
			return 1;
		}
		otherNodeTime = myLastFailoverTimeA;
	}

	unsigned int failOverTime = 0;

	if (systemTime >= otherNodeTime)
	{
		failOverTime = systemTime;
	}
	else
	{
		failOverTime = otherNodeTime;
	}
	
	//Write the data into the file.
	if (theCurrentNode == NODE_A)
	{
		myLastFailoverTimeA = failOverTime;
		ofstream outfile;
		outfile.open((GetApgOamAccess() == 1) ? NSF_CBL_CONFIG_FILE_NAME_A : NSF_CONFIG_FILE_NAME_A, ios::binary | ios::out);
		
		if (outfile.is_open())
		{
			ifstream::pos_type size = 1024;
			char * ptrmemblock = new char [size];
			if( ptrmemblock )
			{
				memset(ptrmemblock, 0, sizeof(char)*size);
				sprintf(ptrmemblock, "%x;%x;%x;", myAlarmStateA, myAlarmState2A, myLastFailoverTimeA);
				outfile.write(ptrmemblock, size);
				outfile.close();
				delete[] ptrmemblock;
				ptrmemblock = 0;
				DEBUG("%s", "Exiting setRegTime");
				theConfigFileMutex.release();
				return 0;
			}
		}
	}
	else if (theCurrentNode == NODE_B)
	{
		myLastFailoverTimeB = failOverTime;
		ofstream outfile;
		outfile.open((GetApgOamAccess() == 1) ? NSF_CBL_CONFIG_FILE_NAME_B : NSF_CONFIG_FILE_NAME_B, ios::binary | ios::out);
		if (outfile.is_open())
		{
			ifstream::pos_type size = 1024;
			char * ptrmemblock = new char [size];
			if (ptrmemblock)
			{
				memset(ptrmemblock, 0, sizeof(char)*size);
				sprintf(ptrmemblock, "%x;%x;%x;", myAlarmStateB, myAlarmState2B, myLastFailoverTimeB);                                
				outfile.write(ptrmemblock, size);
				outfile.close();
				delete[] ptrmemblock;
				ptrmemblock = 0;
				DEBUG("%s", "Exiting setRegTime");
				theConfigFileMutex.release();
				return 0;
			}
		}
	}
		
	theConfigFileMutex.release();
	ERROR("%s","Exiting setRegTime");
	return 1;
}

/*===================================================================
   ROUTINE: getPublicdnName
=================================================================== */
string ACS_NSF_Common :: getPublicdnName ( ) 
{
	return dnObject;
}

/*===================================================================
   ROUTINE: helperTraceInformation
=================================================================== */
void ACS_NSF_Common :: helperTraceInformation(const ACE_TCHAR* messageFormat, ...) 
{
	const ACE_UINT32 TRACE_BUF_SIZE = 1024;
	ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
	ACE_TCHAR traceOutput[TRACE_BUF_SIZE];
	ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
	ACE_OS::memset( traceOutput, 0, sizeof( TRACE_BUF_SIZE ));
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';
		ACE_OS::snprintf(traceOutput, TRACE_BUF_SIZE - 1, " 0x%lx %s", ACE_Thread::self(), traceBuffer);
		traceOutput[TRACE_BUF_SIZE-1]='\0';
		//! If Trace if ON .Send trace to TRA
		if( ACS_NSF_InformationTrace.ACS_TRA_ON() )
		{
			ACS_NSF_InformationTrace.ACS_TRA_event(1,traceOutput);
		}

		ACS_NSF_Logging.Write(traceOutput,LOG_LEVEL_INFO);
		va_end(params);
	}
}


/*===================================================================
   ROUTINE: helperTraceDebug
=================================================================== */
void ACS_NSF_Common :: helperTraceDebug(const ACE_TCHAR* messageFormat, ...) 
{
	const ACE_UINT32 TRACE_BUF_SIZE = 1024;
	ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
	ACE_TCHAR traceOutput[TRACE_BUF_SIZE];
	ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
	ACE_OS::memset( traceOutput, 0, sizeof( TRACE_BUF_SIZE ));
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';
		ACE_OS::snprintf(traceOutput, TRACE_BUF_SIZE - 1, " 0x%lx %s", ACE_Thread::self(), traceBuffer);
		traceOutput[TRACE_BUF_SIZE-1]='\0';
		//! If Trace if ON .Send trace to TRA
		if( ACS_NSF_DebugTrace.ACS_TRA_ON() )
		{
			ACS_NSF_DebugTrace.ACS_TRA_event(1,traceOutput);
		}

		ACS_NSF_Logging.Write(traceOutput,LOG_LEVEL_DEBUG);
		va_end(params);
	}
}



/*===================================================================
   ROUTINE: helperTraceWarning
=================================================================== */
void ACS_NSF_Common :: helperTraceWarning(const ACE_TCHAR* messageFormat, ...)
{
	static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
	ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
	ACE_TCHAR traceOutput[TRACE_BUF_SIZE];
	ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
	ACE_OS::memset( traceOutput, 0, sizeof( TRACE_BUF_SIZE ));
	if ( messageFormat && *messageFormat )
	{

		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';
		ACE_OS::snprintf(traceOutput, TRACE_BUF_SIZE - 1, " 0x%lx %s", ACE_Thread::self(), traceBuffer);
		traceOutput[TRACE_BUF_SIZE-1]='\0';		
		//! If Trace if ON .Send trace to TRA
		if( ACS_NSF_WarningTrace.ACS_TRA_ON() )
		{
			ACS_NSF_WarningTrace.ACS_TRA_event(1,traceOutput);
		}
		ACS_NSF_Logging.Write(traceOutput,LOG_LEVEL_WARN);
		va_end(params);
	}
}

/*===================================================================
   ROUTINE: helperTraceError
=================================================================== */
void ACS_NSF_Common :: helperTraceError(const ACE_TCHAR* messageFormat, ...) 
{
	static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
	ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
	ACE_TCHAR traceOutput[TRACE_BUF_SIZE];
	ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
	ACE_OS::memset( traceOutput, 0, sizeof( TRACE_BUF_SIZE ));
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';
		ACE_OS::snprintf(traceOutput, TRACE_BUF_SIZE - 1, " 0x%lx %s", ACE_Thread::self(), traceBuffer);
		traceOutput[TRACE_BUF_SIZE-1]='\0';		
		//! If Trace if ON .Send trace to TRA
		if( ACS_NSF_ErrorTrace.ACS_TRA_ON() )
		{
			ACS_NSF_ErrorTrace.ACS_TRA_event(1,traceOutput);
		}
		ACS_NSF_Logging.Write(traceOutput,LOG_LEVEL_ERROR);

		va_end(params);
	}
}

/*===================================================================
   ROUTINE: helperTraceFatal
=================================================================== */
void ACS_NSF_Common :: helperTraceFatal(const ACE_TCHAR* messageFormat, ...) 
{
	const ACE_UINT32 TRACE_BUF_SIZE = 1024;
	ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
	ACE_TCHAR traceOutput[TRACE_BUF_SIZE];
	ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
	ACE_OS::memset( traceOutput, 0, sizeof( TRACE_BUF_SIZE ));
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';		
		ACE_OS::snprintf(traceOutput, TRACE_BUF_SIZE - 1, " 0x%lx %s", ACE_Thread::self(), traceBuffer);
		traceOutput[TRACE_BUF_SIZE-1]='\0';
		//! If Trace if ON .Send trace to TRA
		if( ACS_NSF_FatalTrace.ACS_TRA_ON() )
		{
			ACS_NSF_FatalTrace.ACS_TRA_event(1,traceOutput);
		}

		ACS_NSF_Logging.Write(traceOutput,LOG_LEVEL_FATAL);
		va_end(params);
	}
}

/*===================================================================
   ROUTINE: getFailOverSetFlag
=================================================================== */
bool ACS_NSF_Common::getFailOverSetFlag()
{
	return theFailOverSetFlag;
}

/*===================================================================
   ROUTINE: setFailOverSetFlag
=================================================================== */
void ACS_NSF_Common::setFailOverSetFlag(bool aFailOverSetFlag)
{
	theFailOverSetFlag = aFailOverSetFlag;
}

/*===================================================================
   ROUTINE: getInitialSetFlag
=================================================================== */
bool ACS_NSF_Common::getInitialSetFlag()
{
	return theInitialSetFlag;
}

/*===================================================================
   ROUTINE: setInitialSetFlag
=================================================================== */
void ACS_NSF_Common::setInitialSetFlag(bool aInitialSetFlag)
{
	theInitialSetFlag = aInitialSetFlag;
}

/*===================================================================
   ROUTINE: calculateStartStopValue
=================================================================== */
bool ACS_NSF_Common::calculateStartStopValue(unsigned int aActiveValue)
{
	//DEBUG("%s", "Entering calculateStartStopValue");
	//DEBUG("isSurveillanceActive = %d",isSurveillanceActive);
	//DEBUG("aActiveValue = %d",aActiveValue);

	if(isSurveillanceActive == 1)
	{
		if(aActiveValue)
		{
			theStartStopValue = 3;
		}
		else
		{
			theStartStopValue = 2;
		}
	}
	else
	{
		if(aActiveValue)
		{
			theStartStopValue = 1;
		}
		else
		{
			theStartStopValue = 0;
		}
	}

	//DEBUG("%s", "Exiting calculateStartStopValue");
	return true;
}

/*===================================================================
   ROUTINE: getActiveNodeFlag
=================================================================== */
bool ACS_NSF_Common::getActiveNodeFlag()
{
	//DEBUG("%s", "Entering getActiveNodeFlag");

	#ifdef  NONODESTATEPRC

		theActiveNodeFlag = false;

		std::string cmd("/opt/ap/acs/bin/");
		cmd.append(ACS_NSF_NODE_STATE_FILE_NAME);
		char nodeState[50] = { 0 };

		FILE *fp = NULL;
		fp = popen( cmd.c_str(), "r");
		if( fp != NULL)
		{	
			if ( fscanf(fp, "%16s", nodeState) != 0 )
			{
				DEBUG("Node State is %s", nodeState);

				if ( ACE_OS::strcmp("ACTIVE", nodeState ) == 0 )
				{
					theActiveNodeFlag = true;
			
				}
				else if ( ACE_OS::strcmp("STANDBY", nodeState ) == 0 )
				{
					theActiveNodeFlag = false;
				}
			}
			pclose(fp);
		}
	#else

		ACS_PRC_API prcObj;

		int result = 0;
		result = prcObj.askForNodeState();

		if( result == 1 )
		{
			//Node is active
			theActiveNodeFlag = true;
		}
		else
		{
			theActiveNodeFlag = false;
		}

	#endif
	
	//DEBUG("%s", "Exiting getActiveNodeFlag");
	return theActiveNodeFlag;
}

/*===================================================================
   ROUTINE: setLoadParamsFlag
=================================================================== */
void ACS_NSF_Common::setLoadParamsFlag(bool flag)
{
//	DEBUG("%s", "Entering setLoadParamsFlag");
	isLoadingParamsRequired = flag;
//	DEBUG("%s", "Exiting setLoadParamsFlag");
}

/*===================================================================
   ROUTINE: isLoadingOfParamRequired
=================================================================== */
bool ACS_NSF_Common::isLoadingOfParamsRequired()
{
	//DEBUG("%s", "Entering isLoadingOfParamsRequired");
	//DEBUG("%s", "Exiting isLoadingOfParamsRequired");
	return isLoadingParamsRequired;
}

/*===================================================================
   ROUTINE: fetchDnOfRootObjFromIMM
=================================================================== */
int ACS_NSF_Common::fetchDnOfRootObjFromIMM ()
{

	DEBUG("%s", "Entering fetchDnOfRootObjFromIMM");

	static int infoAlreadyLoad = 0;
	static char dnOfRootObj[512] = {0};
	int returnCode = 0;

	OmHandler omhandler;
	if( omhandler.Init() == ACS_CC_FAILURE )
	{
		ERROR("%s", "Omhandler initialization failed!!");
		ERROR("%s", "Exiting fetchDnOfRootObjFromIMM");
                return -1;
	}
		

	if (infoAlreadyLoad)
	{
		parentObjDNofNSF = std::string(dnOfRootObj);
		DEBUG("%s", "Parent DN already loaded");
		DEBUG("%s", "Exiting fetchDnOfRootObjFromIMM");
		return 0;
	}
	std::vector<std::string> dnList;
	DEBUG("NSF class name  = %s",ACS_CONFIG_IMM_NSF_CLASS_NAME);

	if (omhandler.getClassInstances(ACS_CONFIG_IMM_NSF_CLASS_NAME, dnList) != ACS_CC_SUCCESS)
	{   //ERROR
		ERROR("%s", "Error occured in getting class instances");
		ERROR("%s", "Exiting fetchDnOfRootObjFromIMM");
		returnCode = -1;
	}
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() ^ 1)
		{
			//WARNING: 0 (zero) or more than one node found
			ERROR("%s", "More than one object found.");
			ERROR("%s", "Exiting fetchDnOfRootObjFromIMM");
			returnCode = -1;
		}
		else
		{
			//OK: Only one root node
			strncpy(dnOfRootObj, dnList[0].c_str(), ACS_NSF_ARRAY_SIZE(dnOfRootObj));
			dnOfRootObj[ACS_NSF_ARRAY_SIZE(dnOfRootObj) - 1] = 0;
			infoAlreadyLoad = 1;
			parentObjDNofNSF = std::string(dnOfRootObj);

		}
	}
	omhandler.Finalize();

	DEBUG("%s", "Exiting fetchDnOfRootObjFromIMM");

	return returnCode;
}

/*===================================================================
   ROUTINE: setNodeSpecificParams
=================================================================== */
int  ACS_NSF_Common::setNodeSpecificParams()
{
	DEBUG("%s", "Entering setNodeSpecificParams");

	theCurrentNode  = getNode();
	
	if (theCurrentNode == UNDEFINED)
	{
		ERROR("%s", "Node State is UNDEFINED");
		return -1;
	}

	if (ACS_NSF_Common::fetchDnOfRootObjFromIMM () == -1)
	{
		ERROR("%s", "Unable to fetch Parent DN of NSF from XML file!!!");
		return -1;
	}

	if (theOmHandler.Init() != ACS_CC_SUCCESS)
	{
		ERROR("%s", "Initialization of OmHandler failed");
		return -1;
	}

	dnObject.append(ACS_NSF_Common::parentObjDNofNSF);

	DEBUG("%s", "Leaving setNodeSpecificParams");

	return 0;
}

/*===================================================================
   ROUTINE: GetPSState
=================================================================== */
ACE_INT32 ACS_NSF_Common::GetPSState()
{
	//According to APG43L
	//	0	-> Not Configured
	//	1	-> Configured

	ACE_INT32 physicalSepStatus = -1;

	ACS_CC_ImmParameter attrPSStatus;
	attrPSStatus.attrName = (char *)PHYSICAL_SEPARATION_ATTR_NAME;
	std::string dnName = NORTHBOUND_OBJ_DN;
	
	if ( theOmHandler.getAttribute( dnName.c_str(), &attrPSStatus) == ACS_CC_SUCCESS )
	{
		if (attrPSStatus.attrName == (char*)PHYSICAL_SEPARATION_ATTR_NAME )
		{
			physicalSepStatus = *(reinterpret_cast<ACE_INT32*>(*(attrPSStatus.attrValues)));	
		}
	}
	else
	{
		ERROR("Failed to get Physical separation state - %s", theOmHandler.getInternalLastErrorText());
	}
   
	INFO("Physical separation state %d", physicalSepStatus);

	return physicalSepStatus;
}

/*===================================================================
   ROUTINE: GetApgOamAccess
=================================================================== */
ACE_INT32 ACS_NSF_Common::GetApgOamAccess()
{
	static ACE_INT32 apgOamAccess = -1;
	
	if (apgOamAccess != -1)
	{
		// Fetch the apgOamAccess already, no need to fetch again just return the value		
		return apgOamAccess;
	}

	OmHandler omHandler;
	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		ERROR("OmHandler initialization failed - %s", omHandler.getInternalLastErrorText());
		return -1;
	}
	
	ACS_CC_ImmParameter attrApgOamAccess;
	attrApgOamAccess.attrName = (char *)APG_OAM_ACCESS_ATTR_NAME;
	std::string dnName = AXEFUNCTIONS_OBJ_DN;
	
	if (omHandler.getAttribute(dnName.c_str(), &attrApgOamAccess) == ACS_CC_SUCCESS)
	{
		if (attrApgOamAccess.attrName == (char*)APG_OAM_ACCESS_ATTR_NAME)
		{
			apgOamAccess = *(reinterpret_cast<ACE_INT32*>(*(attrApgOamAccess.attrValues)));	
		}
	}
	else
	{
		ERROR("Failed to get value of %s - %s", APG_OAM_ACCESS_ATTR_NAME, omHandler.getInternalLastErrorText());
	}
   
	omHandler.Finalize();
	
	INFO("APG OAM Access value %d", apgOamAccess);
	return apgOamAccess;
}
