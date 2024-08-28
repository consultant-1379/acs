//******************************************************************************
//
//  NAME
//     lmtestadd.cpp
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
#include "acs_lm_cmd.h"
#include "acs_lm_common.h"
#include "acs_lm_cmdclient.h"
using namespace std;

static void printUsage()
{
	cerr<< "lmtestadd -n LKID -t Status -p ParameterName -s SetName -v Value" << endl;
} 

static ACS_LM_AppExitCode checkOpt(int argc, char** argv)
{
    bool opt_n = false;
    bool opt_t = false;
    bool opt_p = false;
    bool opt_s = false;
    bool opt_v = false;
            
    int  optind = 1;
    ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
		if(!ACS_LM_Common::isCommandAllowed())
		{
			return ACS_LM_RC_CMD_NOTALLOWED;
		}
    if(argc != 11)
		{
			return ACS_LM_RC_INCUSAGE;
		}
		while( (optind+1) < argc )
    {
        if(strcmp(argv[optind], "-n") == 0)
        {
            if(opt_n)
            {
								exitCode = ACS_LM_RC_INCUSAGE;
            }
            else
            {
                opt_n = true;		          
                if((int)::strlen(argv[optind+1])>15)
		        		{
    								exitCode = ACS_LM_RC_UNREAS;
		        		}
			    			else
                {
                    if(ACS_LM_Common::isStringValid(argv[optind+1]))
                    {
					    					ACS_LM_Common::toUpperCase(argv[optind+1]);
                    }
                    else
                    {
                        exitCode = ACS_LM_RC_UNREAS;
                    }
                }                 
            }
	     }
       else if(strcmp((char*)argv[optind], "-t") == 0)
       {
           if(opt_t)
           {
								exitCode = ACS_LM_RC_INCUSAGE;
           }
           else
           {
                opt_t = true;
                if((strcmp(argv[optind+1], "0") == 0) || (strcmp(argv[optind+1], "1")==0) )
                {
                  
                }
                else
                {
                    exitCode = ACS_LM_RC_UNREAS;
                }
            }
        }
        else if(strcmp((char*)argv[optind], "-p") == 0)
        {
            if(opt_p)
            {
		          exitCode = ACS_LM_RC_INCUSAGE;        
            }
            else
            {
                opt_p = true;
                if((int)::strlen(argv[optind+1]) > 15)
		        		{
    								exitCode = ACS_LM_RC_UNREAS;	
	     					}
								else
								{
             				if(ACS_LM_Common::isParnameValid(argv[optind+1]))
             				{
							    			ACS_LM_Common::toUpperCase(argv[optind+1]);
             				}
             				else
             				{
                  			exitCode = ACS_LM_RC_UNREAS;
             				}
        				}	
            }
        }
	     	else if(strcmp((char*)argv[optind], "-s") == 0)
        {
            if(opt_s)
            {
		          exitCode = ACS_LM_RC_INCUSAGE;
            }
            else
            {
                opt_s = true;
                if((int)::strlen(argv[optind+1])>15)  //2 bytes
                {
						        exitCode = ACS_LM_RC_UNREAS;
    		    		}
								else
								{
                    if(ACS_LM_Common::isParnameValid(argv[optind+1]))
                    {
					    					ACS_LM_Common::toUpperCase(argv[optind+1]);
                    }
                    else
                    {
                        exitCode = ACS_LM_RC_UNREAS;
                    }
								}
            }
        }
	     else if(strcmp((char*)argv[optind], "-v") == 0)
	     	{
            if(opt_v) 
            {
	             exitCode = ACS_LM_RC_INCUSAGE;
            }
            else 
            {
                opt_v = true;
								for(int i = 0; i<(int)::strlen(argv[optind+1]); i++)
								{
										if(!::isdigit(argv[optind+1][i]))
										{
												exitCode = ACS_LM_RC_UNREAS;
										}
								}
                if(( atoi((char*)argv[optind+1])<0) || ( atoi((char*)argv[optind+1])>65535))  //2 bytes of data
                {
	                 exitCode = ACS_LM_RC_UNREAS;
                }              
            }
	     }
	     optind = optind + 2;
    }
    if ( opt_n && opt_t && opt_s && opt_p && opt_v )
    {
        if ( exitCode !=  ACS_LM_RC_UNREAS )
	     {
            exitCode = ACS_LM_RC_OK;
	     }

    }
    else 
    {
	     exitCode = ACS_LM_RC_INCUSAGE;
    }

    return exitCode;
}

int main(int argc, char** argv)
{
#if 0
    //PRC process to differentiate AP1 and AP2
    AP_InitProcess("lmtestadd", AP_COMMAND);
    //Crash Routine
    AP_SetCleanupAndCrashRoutine("lmtestadd", NULL);
#endif
		ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
//		ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKADD;

		exitCode = checkOpt(argc, argv);
		if(exitCode != ACS_LM_RC_OK)
		{
				cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
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
					ACS_LM_AppCmdCode cmdCode = ACS_LM_CMD_TESTLKADD;
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
						cout<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
				}
				cmdClient.disconnect();
		}
		return exitCode;
}
