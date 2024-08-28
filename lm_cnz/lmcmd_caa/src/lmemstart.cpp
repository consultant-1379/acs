//******************************************************************************
//
//  NAME
//     lmemstart.cpp
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
//     2009-01-27 by XCSVEMU PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#include "acs_lm_common.h"
#include "acs_lm_cmd.h"
#include "acs_lm_cmdclient.h"


static void printUsage()
{
	std::cerr<< "lmemstart [-f]"<<std::endl;
} 

//Retrun codes will be given a separte name in def file
static ACS_LM_AppExitCode checkOpt(int argc, char** argv)
{
    bool opt_f = false;
    //int  optind = 1;
	
	//ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}
    if(argc == 2)
    {
    int  optind = 1;
		if(::strcmp(argv[optind], "-f") == 0)
		{
			if(!opt_f)
			{
				opt_f = true;	
			}			
		}
        else
        {
            return ACS_LM_RC_INCUSAGE;
        }
    }
    else if(argc > 2)
    {
        return ACS_LM_RC_INCUSAGE;
    }

    if(!opt_f)
    {
        char str[4] = {0};
        char ch = 'n';
        while(true)
        {
            std::cout<<"Are you sure? (y/n) : ";
            ::fflush(stdout);
            //::fflush(stdin);
            ::fgets(str, 4, stdin);
            ch = str[0];
            if((strlen(str) == 2) && (ch == 'y' || ch == 'Y'))
            {
                break;
            }
            else if((strlen(str) == 2) && (ch == 'n' || ch == 'N'))
            {
                return ACS_LM_RC_NOK;
            }
        }
	}	
	return ACS_LM_RC_OK;
}

int main(int argc, char** argv)
{
    ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
//	ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_LKEMSTART;

	exitCode = checkOpt(argc, argv);
	if(exitCode != ACS_LM_RC_OK)
	{
		if(exitCode != ACS_LM_RC_NOK)
		{
			std::cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
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
		ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_LKEMSTART;
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
