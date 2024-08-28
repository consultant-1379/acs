//******************************************************************************
//
//  NAME
//     lmfpprint.cpp
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
//     2009-01-05 by XCSVEMU PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************




static void printUsage()
{
	std::cerr<< "lmfpprint"<<std::endl;
} 

int main(int argc, char** argv)
{
    //PRC process to differentiate AP1 and AP2
    AP_InitProcess("lmfpprint", AP_COMMAND);
    //Crash Routine
    AP_SetCleanupAndCrashRoutine("lmfpprint", NULL);

	ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
	if(!ACS_LM_Common::isCommandAllowed())
	{
        exitCode = ACS_LM_RC_CMD_NOTALLOWED;
        std::cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		return exitCode;
	}
	ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_FPPRINT;
	if(argc > 1)
	{
		exitCode = ACS_LM_RC_INCUSAGE;
		std::cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		printUsage();	
    }
	else
	{
		exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
		ACS_LM_CmdClient cmdClient;
		//if(cmdClient.connect(".", LM_CMD_SERVICE))
		if(cmdClient.connect())
		{
			ACS_LM_Cmd cmdSend(cmdCode, argc, argv);
			if(cmdClient.send(cmdSend))
			{
				ACS_LM_Cmd cmdRecv;
				if(cmdClient.receive(cmdRecv))
				{
					if((exitCode=(ACS_LM_AppExitCode)cmdRecv.commandCode()) == ACS_LM_RC_OK)
					{
						std::list<std::string> args = cmdRecv.commandArguments();
						for(std::list<std::string>::iterator it = args.begin(); it != args.end(); ++it)
						{
							std::string str = (*it);
							std::cout<<"CUSTOM FINGERPRINT: "<<str.c_str()<<std::endl;			
						}			
					}				
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

