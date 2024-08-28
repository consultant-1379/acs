//******************************************************************************
//
//  NAME
//     lmtestact.cpp
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
	cerr<< "lmtestact [-e] [-f]"<<endl;
} 
static ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKACT;
//Retrun codes will be given a separte name in def file
static ACS_LM_AppExitCode checkOpt(int argc, char** argv)
{
    bool opt_f = false;
	bool opt_e = false;
    int  optind = 1;
	
//	ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}
    while((optind < argc) && (argc != 1))
	{
	//	cout<<"argv[optind] : "<<argv[optind]<<endl;
		if(argc <= 3)
		{
			if(ACE_OS::strcmp(argv[optind], "-f") == 0)
			{
				if(!opt_f)
				{
					opt_f = true;	
					//cout<<"opt_f true "<<endl;
				}
				else
				{
					return ACS_LM_RC_INCUSAGE;
				}
			}
			else if(ACE_OS::strcmp(argv[optind], "-e") == 0)
			{
				if(!opt_e)
				{
					opt_e = true;	
				}
				else
				{
					return ACS_LM_RC_INCUSAGE;
				}
			}
			else
			{
				return ACS_LM_RC_INCUSAGE;
			}
			optind++;
		}
		else
		{
			return ACS_LM_RC_INCUSAGE;
		}
	}

    if(!opt_f)
    {
        char ch = ' ';
        char str[100]= {0};
        while(true)
        {

            cout<<"Are you sure? (y/n) : ";
            ::fflush(stdout);
            ::fflush(stdin);
            ::fgets(str, 100, stdin);
            ch = str[0];
            // cin>>ch;
            if( (strlen(str) == 2) && (ch == 'y' || ch == 'Y'))
            {
                break;
            }
            else if((strlen(str) == 2) && (ch == 'n' || ch == 'N'))
            {
                return ACS_LM_RC_NOK;
            }
        }
    }
    if(opt_e)
    {
 	cmdCode = ACS_LM_CMD_TESTLKDEACT;
    }
    return ACS_LM_RC_OK;
}

int main(int argc, char** argv)
{
    //PRC process to differentiate AP1 and AP2
#if 0
    AP_InitProcess("lmtestact", AP_COMMAND);
    //Crash Routine
    AP_SetCleanupAndCrashRoutine("lmtestact", NULL);
#endif
    ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;	

	exitCode = checkOpt(argc, argv);
	if(exitCode != ACS_LM_RC_OK)
	{
		if(exitCode != ACS_LM_RC_NOK)
		{
			cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<endl;
			if(exitCode == ACS_LM_RC_INCUSAGE)
			{
				printUsage();		
			}
		}
		else
		{
			exitCode = ACS_LM_RC_OK;
		}
    	}
	else
	{
		exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
		ACS_LM_CmdClient cmdClient;

		//if(cmdClient.connect(".", LM_CMD_SERVICE))
		if(cmdClient.connect())
		{
			//cout<<"connect successfully"<<endl;
			ACS_LM_Cmd cmdSend(cmdCode, argc, argv);
			if(cmdClient.send(cmdSend))
			{
				ACS_LM_Cmd cmdRecv;
				if(cmdClient.receive(cmdRecv))
				{
					//cout<<"Calling cmdRecv.commandCode()"<<endl;
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
