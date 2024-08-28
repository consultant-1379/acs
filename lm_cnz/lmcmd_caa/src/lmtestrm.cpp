//******************************************************************************
//
//  NAME
//     lmtestrm.cpp
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
using namespace std;

static void printUsage()
{
    cerr<< "lmtestrm -n LKID"<<endl;
} 


//Retrun codes will be given a separte name in def file
static ACS_LM_AppExitCode checkOpt(int argc, char** argv)
{
    ACS_LM_AppExitCode exitCode = ACS_LM_RC_INCUSAGE;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}

	if(argc == 3)
	{
	    if(strcmp(argv[1], "-n") == 0 )            
		{
            if((int)strlen(argv[2]) > 15)
            {
                exitCode = ACS_LM_RC_UNREAS;
            }
            else
            {
                  if(ACS_LM_Common::isStringValid(argv[2]))
                  {
					    ACS_LM_Common::toUpperCase(argv[2]);
						exitCode = ACS_LM_RC_OK;
                  }
                  else
                  {
                       exitCode = ACS_LM_RC_UNREAS;
                  }
            }
		}
     }

	return exitCode;
}

int main(int argc, char** argv)
{
#if 0
    //PRC process to differentiate AP1 and AP2
    AP_InitProcess("lmtestrm", AP_COMMAND);
    //Crash Routine
    AP_SetCleanupAndCrashRoutine("lmtestrm", NULL);
#endif
    ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
//	ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKREM;

	exitCode = checkOpt(argc, argv);
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
		//ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKREM;
		exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
		ACS_LM_CmdClient cmdClient;
		//if(cmdClient.connect(".", LM_CMD_SERVICE))
		if(cmdClient.connect())
		{
			ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKREM;
			ACS_LM_Cmd cmdSend(cmdCode, argc, argv);
			if(cmdClient.send(cmdSend))
			{
				ACS_LM_Cmd cmdRecv;
				if(cmdClient.receive(cmdRecv))
				{
					exitCode = (ACS_LM_AppExitCode)cmdRecv.commandCode();
				}
			}
		}
		if(exitCode != ACS_LM_RC_OK)
		{
			std::cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		}
		cmdClient.disconnect();
	}
	return exitCode;
}
