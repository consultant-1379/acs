//******************************************************************************
//
//  NAME
//     lmlkls.cpp
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

#include "acs_lm_common.h"
#include "acs_lm_cmd.h"
#include "acs_lm_cmdclient.h"

struct LmLkLs
{
	std::string lkid;
	std::string beginDate;
	std::string endDate;
	std::string value;
	std::string vendorInfo;
};

static std::list<LmLkLs*> outList;

static void printUsage()
{
	std::cerr<< "lmlkls [-e daystoexpire]"<<std::endl;
} 

static ACS_LM_AppExitCode checkOpt(const int argc, char** argv)
{
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_INCUSAGE;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}
    if(argc == 3)
    {
        if(strcmp(argv[1], "-e") == 0)
        {
			for(int i = 0; i<(int)::strlen(argv[2]); i++)
			{
				if(!::isdigit(argv[2][i]))
				{
					return ACS_LM_RC_UNREAS;
					break;
				}
			}
			if((atoi(argv[2]) < 1) || (atoi(argv[2]) > 60))
			{
				exitCode = ACS_LM_RC_UNREAS;
			}
			else
			{
				exitCode = ACS_LM_RC_OK;
			}
        }
	}
	else if(argc == 1)
	{
		exitCode = ACS_LM_RC_OK;
    }

	return exitCode;
}

static void printResult(std::list<LmLkLs*>& outList)
{
	char outStr[512];
	::memset(outStr,0,512);

	std::cout<<"LKID             START       END         VALUE  DESCRIPTION"<<std::endl;
	
	for(std::list<LmLkLs*>::iterator it=outList.begin();
		it != outList.end(); ++it)
	{
		LmLkLs* lk = (*it);
		::memset(outStr, 0, 511);	
		snprintf(outStr, 511, "%-17s%-12s%-12s%-7s%s",
			        lk->lkid.c_str(),
				    lk->beginDate.c_str(),
                    lk->endDate.c_str(),
				    lk->value.c_str(),
				    lk->vendorInfo.c_str());
		delete lk;
		std::cout<<outStr<<std::endl;
	}
	outList.clear();
}

//int status,value;
int main(int argc, char** argv)
{
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
//	ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_LKLIST;
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
		ACS_LM_CmdClient cmdClient;
		//if(cmdClient.connect(".", LM_CMD_SERVICE))
		if(cmdClient.connect())
		{
			ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_LKLIST;
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
							LmLkLs* lk = new LmLkLs();
							std::list<std::string>::iterator itr = argList.begin();
							lk->lkid = (*(itr++));
							lk->beginDate = (*(itr++));
							lk->endDate = (*(itr++));
							lk->value = (*(itr++));
							lk->vendorInfo = (*(itr++));
							outList.push_back(lk);
							argList.clear();
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
			std::cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		}
		cmdClient.disconnect();
	}
	return exitCode;
}
