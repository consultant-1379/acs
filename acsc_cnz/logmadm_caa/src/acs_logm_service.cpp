
/*=================================================================== */
   /**
   @file acs_logm_service.cpp

   Class method implementation for LOGM Service module.

   This program takes a decision for Logmaint to execute in debug mode
   or execute along with HA on node.

   @version N.N.N

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       2010/10/21     XKUSATI  Initial Release
==================================================================== */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <poll.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <ace/Handle_Set.h>
#include <ace/Log_Msg.h>
#include <acs_logm_svc_loader.h>
#include <acs_logm_types.h>
#include <acs_logm_common.h>


/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define SELECT_TIMEOUT_SEC 100
#define SELECT_TIMEOUT_USEC 0


bool theInteractiveMode = false;

/*===================================================================
   ROUTINE: printUsage
=================================================================== */
void printUsage()
{
	cout<<"Usage : \nacs_logmaintd -d <timeoutvalue in seconds>  for debug mode"<<endl;
}

ACS_LOGM_AMFService *theAMFLogmAppl = 0;
/*===================================================================
   ROUTINE: ACE_TMAIN
=================================================================== */
ACE_INT32 ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{

	ACS_LOGM_Common::ACS_LOGM_InformationTrace = new ACS_TRA_trace("LOGMINFO","C1024");

	ACS_LOGM_Common::ACS_LOGM_WarningTrace = new ACS_TRA_trace("LOGMWARNING","C1024");

	ACS_LOGM_Common::ACS_LOGM_ErrorTrace = new ACS_TRA_trace("LOGMError","C1024");

	ACS_LOGM_Common::ACS_LOGM_Logging = new ACS_TRA_Logging();

	ACS_LOGM_Common::ACS_LOGM_Logging->ACS_TRA_Log_Open("LOGM");

	LOGM_Service_Loader oLOGMService;

	//Execute Logmaint in Debug mode
	if(argc > 1)
	{
              if((!strcmp(argv[1],"-d")) && (argc == 3))
              {
	              for(unsigned int i=0; i < strlen(argv[2]); i++)
                      {
                              if(!(isdigit(argv[2][i])))
                              {
                                      cout<<"ERROR : timeoutValue should be Positive Integer"<<endl;
                                      return 0;
                              }

                      }

                      int timeOut = atoi(argv[2]);
		      if( timeOut > 0 )
		      {
			
			      DEBUG("%s","Starting acs_logmaintd in Debug mode");

        	              theInteractiveMode = true;

			      //Start the service in debug mode.

                	      oLOGMService.executeDebug(argc,argv);
			
			      //Delete the tracing objects.

			      delete ACS_LOGM_Common::ACS_LOGM_InformationTrace;
			      ACS_LOGM_Common::ACS_LOGM_InformationTrace = NULL;	

			      delete ACS_LOGM_Common::ACS_LOGM_WarningTrace; 
	                      ACS_LOGM_Common::ACS_LOGM_WarningTrace = NULL;

			      delete ACS_LOGM_Common::ACS_LOGM_ErrorTrace ;
			      ACS_LOGM_Common::ACS_LOGM_ErrorTrace = NULL;

	                      ACS_LOGM_Common::ACS_LOGM_Logging->Close();
		              delete ACS_LOGM_Common::ACS_LOGM_Logging ;

			      ACS_LOGM_Common::ACS_LOGM_Logging = NULL;

			      return 0;
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
	      theInteractiveMode = false;

	      ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

	      theAMFLogmAppl = new ACS_LOGM_AMFService("acs_logmaintd", "root");

	      INFO("%s", "Starting acs_logmaintd service in HA mode.. ");

	      if(theAMFLogmAppl !=0)
	      {
		     errorCode = theAMFLogmAppl->activate();
	      }
	      else
	      {
	             ERROR("%s","theAMFLogmAppl is NULL");

		     delete ACS_LOGM_Common::ACS_LOGM_InformationTrace;
	             ACS_LOGM_Common::ACS_LOGM_InformationTrace = NULL;

          	     delete ACS_LOGM_Common::ACS_LOGM_WarningTrace;
                     ACS_LOGM_Common::ACS_LOGM_WarningTrace = NULL;

	             delete ACS_LOGM_Common::ACS_LOGM_ErrorTrace ;
          	     ACS_LOGM_Common::ACS_LOGM_ErrorTrace = NULL;

                     ACS_LOGM_Common::ACS_LOGM_Logging->Close();
		     delete ACS_LOGM_Common::ACS_LOGM_Logging ;
                     ACS_LOGM_Common::ACS_LOGM_Logging = NULL;

		     return 0;
	      }

	      if (errorCode == ACS_APGCC_HA_FAILURE)
	      {
		     ERROR("%s", "acs_logmaintd, HA Activation Failed!!");
	      }

	      else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
	      {
	             ERROR("%s", "acs_logmaintd, HA Application Failed to Gracefullly closed!!");
	      }

	      else if (errorCode == ACS_APGCC_HA_SUCCESS)
	      {
	             DEBUG("%s", "acs_logmaintd, HA Application Gracefully closed!!");
	      }
	      else
	      {
	             ERROR("%s", "Error occured while activating acs_logmaintd HA application!!");
	      }

	}
	delete theAMFLogmAppl;
        theAMFLogmAppl = 0;

        delete ACS_LOGM_Common::ACS_LOGM_InformationTrace;
        ACS_LOGM_Common::ACS_LOGM_InformationTrace = NULL;
        delete ACS_LOGM_Common::ACS_LOGM_WarningTrace;
        ACS_LOGM_Common::ACS_LOGM_WarningTrace = NULL;
        delete ACS_LOGM_Common::ACS_LOGM_ErrorTrace ;
        ACS_LOGM_Common::ACS_LOGM_ErrorTrace = NULL;
        ACS_LOGM_Common::ACS_LOGM_Logging->Close();
        delete ACS_LOGM_Common::ACS_LOGM_Logging ;
        ACS_LOGM_Common::ACS_LOGM_Logging = NULL;

	return 0;
}



