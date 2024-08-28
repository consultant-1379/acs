/*=================================================================== */
/**
   @file   acs_ssu_monitor.cpp

   @brief cpp file for acs_ssu_monitor.cpp

          This module is the entry point of SSU service.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/06/2010     XSIRKUM        APG43 on Linux.

==================================================================== */
#include <ace/Log_Msg.h>
#include <ace/Handle_Set.h>
#include <acs_ssu_svc_loader.h>
#include <acs_ssu_types.h>
#include "acs_ssu_common.h"

#include "acs_ssu_service.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <acs_aeh_signalhandler.h>

bool theInteractiveMode = false;

ACS_SSU_Service *pSSUObj  = 0;

SSU_Service_Loader *theSSUMonitorService = 0;
std::string ACS_SSU_Common::dnOfSSURoot = "";
std::map<std::string,std::string> ACS_SSU_Common::ssuStringInitialParamsMap;
std::map<std::string,int> ACS_SSU_Common::ssuIntegerInitialParamsMap;

ACS_TRA_trace  ACS_SSU_Common::ACS_SSU_InformationTrace("SSUINFO","C1024");
ACS_TRA_trace  ACS_SSU_Common::ACS_SSU_DebugTrace("SSUDEBUG","C1024");
ACS_TRA_trace  ACS_SSU_Common::ACS_SSU_WarningTrace("SSUWARN","C1024");
ACS_TRA_trace  ACS_SSU_Common::ACS_SSU_ErrorTrace("SSUERROR","C1024");
ACS_TRA_trace  ACS_SSU_Common::ACS_SSU_FatalTrace("SSUFATAL","C1024");
ACS_TRA_Logging ACS_SSU_Common::ACS_SSU_Logging;
ACE_Mutex      ACS_SSU_Common::log_mutex;
void printUsage()
{
	cout << "Usage: \nacs_ssumonitord -d <timeoutValue> in Interactive mode" << endl;
}


ACE_INT32 ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	//Event will be generated whenever acs_ssumonitord service is crash.
	acs_aeh_setSignalExceptionHandler("acs_ssumonitord");
	if(argc > 1)
	{
		// TO CHECK SSU is running in DEBUG mode.
		if( (argc == 3) && !strcmp(argv[1],"-d")) 
		{
			for(unsigned int i=0; i < strlen(argv[2]); i++)
			{
				if(!(isdigit(argv[2][i])))
				{
					cout<<"ERROR : timeoutValue should be Positive Integer"<<endl;
					return -1;
				}

			}

			int timeOut = atoi(argv[2]);
			if( timeOut > 0 ) 
			{
				ACS_SSU_Common::ACS_SSU_Logging.Open("SSU");
				theInteractiveMode = true;
				ACE_INT32 result = 0;
				theSSUMonitorService = new SSU_Service_Loader();
				result = theSSUMonitorService->parseArgs(argc,argv);
				if (result < 0)
				{
					delete theSSUMonitorService;
					exit(EXIT_FAILURE);  // Error in parsing arguments
				}
				else
				{
					if(result > 0)
					{
						delete theSSUMonitorService;
						exit(EXIT_SUCCESS);
					}
				}
				DEBUG("%s", "User started the acs_ssumonitord service in debug mode");

				//! Initialize self pointer used later for terminate
				theSSUMonitorService->initService();
				result = theSSUMonitorService->execute();
				delete theSSUMonitorService;
				return result;
			}
			else
			{
				cout<<"ERROR : timeoutValue should be Positive Integer"<<endl;
			}
		}
		else
		{
			printUsage();
		}
	}
	else
	{
		// SSU in HA mode
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
		pSSUObj = new ACS_SSU_Service("acs_ssumonitord", "root");

		if( pSSUObj == 0 )
		{
			return -1;
		}
		else
		{
			// SSU HA object is created successfully.
			ACS_SSU_Common::ACS_SSU_Logging.Open("SSU");
			DEBUG("%s", "Starting acs_ssumonitord service with HA.. ");
			errorCode = pSSUObj->activate();
			if (errorCode == ACS_APGCC_HA_FAILURE)
			{
				DEBUG("%s", "HA Activation Failed for acs_ssumonitord");
				delete pSSUObj;
				pSSUObj = 0;
				DEBUG("%s", "Leaving acs_ssumonitord");
				return -1;
			}
			else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			{
				DEBUG("%s", "HA Failed to close acs_ssumonitord gracefully");
				delete pSSUObj;
				pSSUObj = 0;
				DEBUG("%s", "Leaving acs_ssumonitord");
				return -1;
			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS)
			{
				DEBUG("%s", "HA Gracefully closed acs_ssumonitord!!");
				delete pSSUObj;
				pSSUObj = 0;
				DEBUG("%s", "Leaving acs_ssumonitord");
				return 0;
			}
			else
			{
				DEBUG("%s", "Error occured while integrating acs_ssumonitord with HA");
				DEBUG("%s", "Leaving acs_ssumonitord");
				delete pSSUObj;
				pSSUObj = 0;
				return -1;
			}
		}
	}
	return 0;
}
