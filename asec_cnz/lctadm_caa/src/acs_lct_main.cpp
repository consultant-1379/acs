//******************************************************************************
//
// NAME
//      acs_lct_main.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//      It acts as main for hardening service
//              
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//      2011-08-03 XSATDEE
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//			2014-06-06      XCSRAKI        	Rebase from APG431.0 for
//                                                      welcomemessage system 
//							improvement(OP#345) on all ports
//
//******************************************************************************  

#include <ace/Log_Msg.h>
#include <ace/Handle_Set.h>
#include <acs_lct_command_handler_hardening.h>
#include <acs_lct_command_handler_tsusers.h>
#include <acs_lct_server.h>
#include <acs_lct_dsdserver.h>
#include <acs_lct_tra.h>
#include <ACS_TRA_Logging.h>

ACS_LCT_Server *ha_sfgen = 0;
bool theInteractiveMode = false;

ACS_TRA_trace * ACS_LCT_TRA::ACS_LCT_InformationTrace=0;
ACS_TRA_trace * ACS_LCT_TRA::ACS_LCT_DebugTrace=0;
ACS_TRA_trace * ACS_LCT_TRA::ACS_LCT_WarningTrace=0;
ACS_TRA_trace * ACS_LCT_TRA::ACS_LCT_ErrorTrace=0;
ACS_TRA_trace * ACS_LCT_TRA::ACS_LCT_FatalTrace=0;
ACS_TRA_Logging * ACS_LCT_TRA::ACS_LCT_Logging=0;

void signal_handler(int signum)
{
    if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP || signum == SIGKILL )
    {
        if((!theInteractiveMode)&& ha_sfgen != 0)
        {
                syslog(LOG_INFO, "Calling performComponentRemoveJobs for acs_lct_hardeningd");
                DEBUG("%s", "Calling performComponentRemoveJobs for acs_lct_hardeningd");
                ha_sfgen->performComponentRemoveJobs();
        } else 
	{
		DEBUG("%s", "Invoking shutdown_lct_debug");
		ha_sfgen->shutdown_lct_debug();
	}

    	DEBUG("%s", "Stopping hardening service");
    	delete ha_sfgen;
    	ha_sfgen=0;
    	if (dsdObj != 0)
    	{
		dsdObj->ShutdownDSD();
    		delete dsdObj;
    	}
    	exit(EXIT_SUCCESS);
    }
}
int main(int argc, char **argv)
{
	//const char * name="Hardening";
	//ACS_LCT_TRA::ACS_LCT_InformationTrace= new ACS_TRA_trace("HARDENING INFO","C512");
	//ACS_LCT_TRA::ACS_LCT_DebugTrace = new ACS_TRA_trace("HARDENING DEBUG","C512");
	//ACS_LCT_TRA::ACS_LCT_WarningTrace= new ACS_TRA_trace("HARDENING WARNING","C512");
	//ACS_LCT_TRA::ACS_LCT_ErrorTrace=new ACS_TRA_trace("HARDENING ERROR TRACE","C512");
	//ACS_LCT_TRA::ACS_LCT_FatalTrace=new ACS_TRA_trace("HARDENING FATAL TRACE","C512");
	//ACS_LCT_TRA::ACS_LCT_Logging=new ACS_TRA_Logging();
	//ACS_LCT_TRA::ACS_LCT_Logging->Open("HARDENING");

	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;	
	int rCode=0;
	(void)CONFIG_IMM_CLASS_NAME;	
	(void)CONFIG_IMM_CLASS_NAME1;
	(void)CONFIG_IMM_CLASS_NAME2;

	signal(SIGUSR1,signal_handler);
	if (argc > 1 && !strcmp(argv[1],"-d") )
	{
			signal(SIGTERM,signal_handler);
			signal(SIGTSTP,signal_handler);
			signal(SIGINT,signal_handler);
			signal(SIGKILL,signal_handler);

			ha_sfgen = new ACS_LCT_Server();
			theInteractiveMode = true;

                        bool dn_ret= ha_sfgen->fetchDnOfRootObjFromIMM();
                        if (dn_ret == false)
                        {
                                ERROR("%s","Fetching DN from IMM failed");
                                ERROR("%s","Internal program error. Exit code: 26");
                                return 1;
                        }

			bool ret1=ha_sfgen->startup();
			if (ret1 == false)
			{
				ERROR("%s","Fetching objects from IMM failed");
				ERROR("%s","Internal program error. Exit code: 26");
				return 1;
			}

			INFO("%s","Starting acs_lct_hardeningd application in debug mode");
			bool ret_val = ha_sfgen -> startDebugMode();
			if (ret_val)
			{
				INFO("%s","acs_lct_hardeningd service started successfully in debug mode");
				return 0;
			}
			else
			{
				INFO("%s","acs_lct_hardeningd service failed in debug mode");
				return 1;
			}

		}
		else
		{
			ACE_NEW_NORETURN(ha_sfgen, ACS_LCT_Server("acs_lct_hardeningd","root"));
			if (!ha_sfgen) {
				syslog(LOG_ERR, "LCT Server class: ha_sfgen Creation FAILED");
				rCode=-2;
				return rCode;
			}
			//Added to spawn TRA log thread
			ACS_LCT_TRA::ACS_LCT_InformationTrace= new ACS_TRA_trace("HARDENING INFO","C512");
			ACS_LCT_TRA::ACS_LCT_DebugTrace = new ACS_TRA_trace("HARDENING DEBUG","C512");
			ACS_LCT_TRA::ACS_LCT_WarningTrace= new ACS_TRA_trace("HARDENING WARNING","C512");
			ACS_LCT_TRA::ACS_LCT_ErrorTrace=new ACS_TRA_trace("HARDENING ERROR TRACE","C512");
			ACS_LCT_TRA::ACS_LCT_FatalTrace=new ACS_TRA_trace("HARDENING FATAL TRACE","C512");
			ACS_LCT_TRA::ACS_LCT_Logging=new ACS_TRA_Logging();
			ACS_LCT_TRA::ACS_LCT_Logging->Open("HARDENING");

			bool dn_ret= ha_sfgen->fetchDnOfRootObjFromIMM();

			if (dn_ret == false)
			{
				//syslog(LOG_INFO, "Fetching DN from IMM failed");
				ERROR("%s","Fetching DN from IMM failed");
				ERROR("%s","Internal program error. Exit code: 26");
				return 1;
			}
			bool ret1=ha_sfgen->startup();
			if (ret1 == false)
			{
				//syslog(LOG_INFO, "Fetching objects from IMM failed");
				ERROR("%s","Fetching objects from IMM failed");
				ERROR("%s","Internal program error. Exit code: 26");
				return 1;
			}
			syslog(LOG_INFO, "Starting acs_lct_hardeningd service.. ");
			errorCode = ha_sfgen->activate();
			switch (errorCode) {

				case ACS_APGCC_HA_FAILURE: {
					syslog(LOG_ERR, "acs_lct_hardeningd, HA Activation Failed!");
					rCode=-1;
					break;
				}
				case ACS_APGCC_HA_FAILURE_CLOSE: {
					syslog(LOG_ERR, "acs_lct_hardeningd, HA Application Failed to Gracefullly closed!");
					rCode=-2;
					break;
				}
				case ACS_APGCC_HA_SUCCESS: {
					syslog(LOG_ERR, "acs_lct_hardeningd, HA Application Gracefully closed!");
					rCode=0;
					break;
				}
				default: {
					syslog(LOG_ERR, "acs_lct_hardeningd, Unknown Application error detected!");
					rCode=-2;
					break;
				}
			}                	
                delete ha_sfgen;
		}
        return rCode;
}


