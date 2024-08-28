//******************************************************************************
//
//  NAME
//     lmtestls.cpp
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
//     2008-12-01 by XCSSAPC PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#include "acs_lm_common.h"
#include "acs_lm_cmd.h"
#include "acs_lm_cmdclient.h"
#include <iostream>
using namespace std;

struct LmTestLs
{
	std::string lkid;
	std::string parameterName;
	std::string setName;
	std::string status;
	std::string value;
};

static std::list<LmTestLs*> outList;

static void printUsage()
{
    cerr<< "lmtestls"<<endl;
} 

//Return codes will be given a separte name in def file
static ACS_LM_AppExitCode checkOpt(int argc)
{
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}
		
    	if(argc > 1)
    	{
		exitCode = ACS_LM_RC_INCUSAGE;
    	}

	return exitCode;
}

static void printResult(std::list<LmTestLs*>& outList)
{
	char outStr[512];
	ACE_OS::memset(outStr, 0, 512);
	
	ACE_OS::sprintf(outStr,  "LKID             STATUS  PARNAME          SETNAME          VALUE");
	cout<<outStr<<endl;
	
    	for(std::list<LmTestLs*>::reverse_iterator it=outList.rbegin();
		it != outList.rend(); ++it)
	{
		LmTestLs* lk = (*it);
		ACE_OS::memset(outStr, 0, 512);	
		ACE_OS::sprintf(outStr, "%-17s%-8s%-17s%-17s%s", 
            	lk->lkid.c_str(),
                lk->status.c_str(),
		lk->parameterName.c_str(),
                lk->setName.c_str(), 
                lk->value.c_str());
		delete lk;
		cout<<outStr<<endl;
	}
	outList.clear();
}

int main(int argc, char** argv)
{
#if 0
    //PRC process to differentiate AP1 and AP2
    AP_InitProcess("lmtestls", AP_COMMAND);
    //Crash Routine
    AP_SetCleanupAndCrashRoutine("lmtestls", NULL);
#endif

    ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
//	ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKLIST;

	exitCode = checkOpt(argc);
	if(exitCode != ACS_LM_RC_OK)
	{
	    std::cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		if(exitCode == ACS_LM_RC_INCUSAGE)
		{
			printUsage();		
		}		
    }
	else
	{
	    exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
		ACS_LM_CmdClient cmdClient;
		//if(cmdClient.connect(".", LM_CMD_SERVICE))
		if(cmdClient.connect())
		{
			ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKLIST;
			ACS_LM_Cmd cmdSend(cmdCode, argc, argv);
			if(cmdClient.send(cmdSend))
			{
				do
				{
					ACS_LM_Cmd cmdRecv;
					if(cmdClient.receive(cmdRecv))
					{
						exitCode=(ACS_LM_AppExitCode)cmdRecv.commandCode();
						std::list<std::string> argList = cmdRecv.commandArguments();
						
						if(argList.size() == 5)
						{
							LmTestLs* lk = new LmTestLs();
							std::list<std::string>::iterator itr = argList.begin();
							lk->lkid = (*(itr++));
							lk->parameterName = (*(itr++));
							lk->setName = (*(itr++));
							lk->status = (*(itr++));
							lk->value = (*(itr++));
							outList.push_back(lk);
						}
					}
					else
					{	
						break;
					}
				}while(exitCode == ACS_LM_RC_MOREDATA);
			}
		}
		if(exitCode == ACS_LM_RC_OK)
		{
			printResult(outList);
		}
		else
		{
			cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<endl;
		}
		cmdClient.disconnect();
	}

	return exitCode;
}
