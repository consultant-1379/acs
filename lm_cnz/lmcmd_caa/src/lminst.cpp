//******************************************************************************
//
//  NAME
//     lminst.cpp
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

static void printUsage()
{
    std::cerr<< "lminst lkfpath"<<std::endl;
} 

//Retrun codes will be given a separte name in def file
static ACS_LM_AppExitCode checkOpt(const int argc, char** argv)
{
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_INCUSAGE;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}
    if(argc == 2)
	{
        if(ACS_LM_Common::getFileType(argv[1]) == ACS_LM_FILE_ISFILE)
		{            
            exitCode = ACS_LM_RC_OK;
		}
		else
		{
            exitCode = ACS_LM_RC_INVALIDPATH;		
    	}
	}

	return exitCode;
}

int main(int argc, char** argv)
{

    ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
	//ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_LKINST;

	exitCode = checkOpt(argc,argv);
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
		std::string fp = ACS_LM_Common::getFullPath(argv[1]);
		argv[1] = (char*)fp.c_str();
        ACS_LM_CmdClient cmdClient;
        //if(cmdClient.connect(".", LM_CMD_SERVICE))
        if(cmdClient.connect())
        {
	ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_LKINST;
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
