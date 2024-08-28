//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_Main
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************
#include <sstream>
#include <syslog.h>
#include <ace/OS.h>

//#include "ACS_PRC_Process.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
//#include "ACS_PHA_Tables.h"
//#include "ACS_PHA_NamedTable.h"
//#include "ACS_PHA_Parameter.h"

#include "ACS_APSESH_CriticalSectionGuard.h"
#include "ACS_APSESH_TestableJtpMgr.h"
#include "ACS_APSESH_Service.h"
#include "ACS_APSESH_Main.h"
#include "ACS_APSESH_Trace.h"

#define RUNNING_DIR	"/tmp"
#define LOCK_FILE	"apsesh.lock"

// This JTP read interval is used when Quorum dissolve event is detected
static const unsigned int JTP_QD_READ_INTERVAL_MSEC = 200;

static const int QUORUM_DISSOLVE_TIMER_SEC = 2;
static const int ONE_HUNDRED_NANOSECONDS_PER_SECOND = 10000000;

const char *ACS_APSESH_Main::ACS_APSESH_PROCESS_NAME = "ACS_APSESH";
const char *ACS_APSESH_Main::ACS_APSESH_DAEMON_NAME = "acs_apseshd";
const char *ACS_APSESH_Main::ACS_APSESH_USER_NAME = "root";

const short ACS_APSESH_Main::DEF_TRACE_MSG_SIZE = 512;
static bool ignoreQuorumDissolveEvent = false;

ACS_APSESH_Main::ACS_APSESH_Main()
{
   ACE_NEW_NORETURN(m_jtpMgr, ACS_APSESH_JtpMgr());
   m_timerActive = false;
   m_ActiveTimerId = 0;

}

ACS_APSESH_Main::~ACS_APSESH_Main()
{
   //CloseHandle(m_QuorumDissolveTimer);

   //if (m_timerActive) {
	//   int returnVal = activeTimer.cancel(m_ActiveTimerId);
   //}

   ACE_Reactor::instance()->cancel_timer(this);

   delete m_jtpMgr;

}

void ACS_APSESH_Main::StartActiveTimer()
{
	APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered...");

    const ACE_Time_Value curr_tv = ACE_OS::gettimeofday();
    m_ActiveTimerId = m_ActiveTimer.schedule(this, NULL, curr_tv + ACE_Time_Value(QUORUM_DISSOLVE_TIMER_SEC));
    m_ActiveTimer.activate();
    m_timerActive = true;

}

int ACS_APSESH_Main::runApsesh()
{
   ACS_APSESH_JtpReturnType resultJtp;

   keepRunning = true;
   const unsigned int RETRY_TIME = 5;

   // Initialize the JtpMgr.
   while(keepRunning)
   {
      resultJtp = m_jtpMgr->init();
      if (resultJtp.returnCode == ACS_APSESH_JtpReturnType::JTP_OK)
      {
    	  // JTP initialization succeeded.
    	  APSESH_Tracer::instance()->trace(__FUNCTION__, "JTP initialization succeeded");
    	  break;
      }
      else
      {
    	  char msg[DEF_TRACE_MSG_SIZE] = { 0 };
          snprintf(msg, sizeof(msg) - 1, "jtpMgr.getData returned code %d: %s", resultJtp.returnCode, resultJtp.returnMsg.c_str());
          APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true);
          m_jtpMgr->unregister();
          sleep(RETRY_TIME);
      }
   }

   while(keepRunning)
   {

      // Get a data buffer from SESH via JTP.
      char* buffer;
      // The buffer used to retrieve data from SESH. This
      // pointer is passed by reference to JTP in the
      // jexdataind() function.  Memory MUST NOT be allocated
      // for this buffer here, and must not be free'd, because
      // JTP reassigns it to a statically allocated memory block.

      int bufLen; // The length of the data buffer returned.

      ACS_APSESH_EventMgr::ACS_APSESH_Event apseshEvent = ACS_APSESH_EventMgr::NONE;

      resultJtp = m_jtpMgr->getData(buffer, bufLen);

      {// trace
         char msg[DEF_TRACE_MSG_SIZE] = { 0 };
         snprintf(msg, sizeof(msg) - 1, "jtpMgr.getData returned code %d: %s\n", resultJtp.returnCode, resultJtp.returnMsg.c_str());
         APSESH_Tracer::instance()->trace(__FUNCTION__, msg);
      }

      if (resultJtp.returnCode == ACS_APSESH_JtpReturnType::JTP_OK) // This should mean that we have a valid, non-zero length buffer.
      {
         // log an event when we received data from SESH
         APSESH_Tracer::instance()->trace(__FUNCTION__, "INFO: QuorumData received");

         // cancel the timer if active
         if (m_timerActive)
         {
	    if(cancelQuorumDissolveTimer() == false){
		ignoreQuorumDissolveEvent  = true; // TR IA75890
	    }
         }

         // Convert the raw data buffer into a QuorumData object.

         QuorumData quorumData;
         memcpy(&quorumData, buffer, min((int) bufLen, (int) sizeof(quorumData)));

         // Create a ACS_CS_API_QuorumData object and fill it with the data in quorumData.

         ACS_CS_API_QuorumData quorumDataCsApi;

         quorumDataCsApi.trafficIsolatedCpId = static_cast<CPID> (quorumData.trafficIsolatedCpId);
         quorumDataCsApi.trafficLeaderCpId = static_cast<CPID> (quorumData.trafficLeaderCpId);

         quorumDataCsApi.automaticQuorumRecovery = (quorumData.aqr != 0); // use != 0 here to eliminate int->bool compile warning

         // Concatenate the high and low order bytes of the APZ profile value, and assign it to the CS API quorum data object.
         quorumDataCsApi.apzProfile = quorumData.profileApzHigh << 8 | quorumData.profileApzLow; // Shift the high-order bits up by 8, then OR with the low-order bits.

         // Do the same for the APT profile value.
         quorumDataCsApi.aptProfile = quorumData.profileAptHigh << 8 | quorumData.profileAptLow;

         { // trace
            char msg[DEF_TRACE_MSG_SIZE] = { 0 };
            snprintf(
                  msg,
                  sizeof(msg) - 1,
                  "---------------------------------------------\n\
                      Quorum data:\n\
                      \ttrafficIsolatedCpId = %d\n\
                      \ttrafficLeaderCpId = %d\n\
                      \taqr = %d\n\
                      \tprofileApz = %d\n\
                      \tprofileApt = %d\n\
                      \tctCps = %d\n",
                  quorumDataCsApi.trafficIsolatedCpId, quorumDataCsApi.trafficLeaderCpId, quorumDataCsApi.automaticQuorumRecovery, quorumDataCsApi.apzProfile, quorumDataCsApi.aptProfile, quorumData.ctCps);

            APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_WARN);
            // uncomment the following line if APSESH needs to log an event with the actual data.
            // APSESH_Tracer::instance()->trace(__FUNCTION__, ACS_APSESH_EventMgr::EVENT_APSESH_INFO , "INFO: QuorumData received.", msg);
         }

         for (int i = 0; i < quorumData.ctCps; ++i)
         {
            CpData cpDataCurrent = quorumData.cpData[i];
            ACS_CS_API_CpData cpDataCsApi;

            cpDataCsApi.id = static_cast<CPID> (cpDataCurrent.id);
            cpDataCsApi.state = static_cast<ACS_CS_API_NS::CpState> (cpDataCurrent.state);
            cpDataCsApi.applicationId = static_cast<ACS_CS_API_NS::ApplicationId> (cpDataCurrent.applicationId);
            cpDataCsApi.apzSubstate = static_cast<ACS_CS_API_NS::ApzSubstate> (cpDataCurrent.apzSubstate);
            cpDataCsApi.stateTransition = static_cast<ACS_CS_API_NS::StateTransition> (cpDataCurrent.stateTransition);
            cpDataCsApi.aptSubstate = static_cast<ACS_CS_API_NS::AptSubstate> (cpDataCurrent.aptSubstate);
            cpDataCsApi.blockingInfo = cpDataCurrent.blockingInfoHigh << 8 | cpDataCurrent.blockingInfoLow;

            //            cout << "B0: "  << (int)cpDataCurrent.cpCapacityB0 << endl;
            //            cout << "B1: "  << (int)cpDataCurrent.cpCapacityB1 << endl;
            //            cout << "B2: "  << (int)cpDataCurrent.cpCapacityB2 << endl;
            //            cout << "B3: "  << (int)cpDataCurrent.cpCapacityB3 << endl;

            cpDataCsApi.cpCapacity = cpDataCurrent.cpCapacityB0 | cpDataCurrent.cpCapacityB1 << 8 | cpDataCurrent.cpCapacityB2 << 16 | cpDataCurrent.cpCapacityB3 << 24;

            {// trace
               char msg[DEF_TRACE_MSG_SIZE] = { 0 };
               snprintf(
                     msg,
                     sizeof(msg) - 1,
                     "CP data:\n\
                         \tid = %d\n\
                         \tstate = %d\n\
                         \tapplicationId = %d\n\
                         \tapzSubstate = %d\n\
                         \tstateTransition = %d\n\
                         \taptSubstate = %d\n\
                         \tblockingInfo = %u\n\
                         \tcpCapacity = %u\n",
                     cpDataCsApi.id, cpDataCsApi.state, cpDataCsApi.applicationId, cpDataCsApi.apzSubstate, cpDataCsApi.stateTransition, cpDataCsApi.aptSubstate, cpDataCsApi.blockingInfo, cpDataCsApi.cpCapacity);
               APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_WARN);

               // uncomment the following line if APSESH needs to log an event with the actual data.
               // APSESH_Tracer::instance()->trace(__FUNCTION__, ACS_APSESH_EventMgr::EVENT_APSESH_INFO, "INFO: QuorumData received.", msg);
            }

            quorumDataCsApi.cpData.push_back(cpDataCsApi);
         }

         usleep(500000); // Sleep a little to prevent CS from being flooded with quorum data updates.  See TR HL31175.


         // Send data to CS
         ACS_CS_API_SET_NS::CS_API_Set_Result resultCs = ACS_CS_API_SET_NS::Result_Success;

         APSESH_Tracer::instance()->trace(__FUNCTION__, "Calling ACS_CS_API_Set::setQuorumData...");
         resultCs = ACS_CS_API_Set::setQuorumData(quorumDataCsApi);

         { //trace
            char msg[DEF_TRACE_MSG_SIZE] = { 0 };

            switch (resultCs) {
               case ACS_CS_API_SET_NS::Result_Success:
                  snprintf(msg, sizeof(msg) - 1, "ACS_CS_API_Set::setQuorumData successfully called (return code %d).", resultCs);
                  break;
               case ACS_CS_API_SET_NS::Result_Incorrect_CP_Id:
                  snprintf(msg, sizeof(msg) - 1, "ACS_CS_API_Set::setQuorumData failed with return code %d: Incorrect_CP_Id", resultCs);
                  break;
               case ACS_CS_API_SET_NS::Result_NoAccess:
                  snprintf(msg, sizeof(msg) - 1, "ACS_CS_API_Set::setQuorumData failed with return code %d: No Access", resultCs);
                  break;
               case ACS_CS_API_SET_NS::Result_Failure:
                  snprintf(msg, sizeof(msg) - 1, "ACS_CS_API_Set::setQuorumData failed with return code %d: Failure", resultCs);
                  break;
               default:
                  snprintf(msg, sizeof(msg) - 1, "ACS_CS_API_Set::setQuorumData failed with return code %d:", resultCs);
            }

            APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_WARN);

         }

         // Send status to SESH.

         short statusCs = (resultCs == ACS_CS_API_SET_NS::Result_Success ? 0 : 1); // Convert to a boolean.

         resultJtp = m_jtpMgr->sendStatusCs(statusCs);

         {// trace
            char msg[DEF_TRACE_MSG_SIZE] = { 0 };
            snprintf(msg, sizeof(msg) - 1, "jtpMgr.sendStatusCs returned code %d: %s\n", resultJtp.returnCode, resultJtp.returnMsg.c_str());
            APSESH_Tracer::instance()->trace(__FUNCTION__, msg);
         }

      }

      std::string problemText;

      switch (resultJtp.returnCode)
      {
         case ACS_APSESH_JtpReturnType::JTP_OK:
            break;

         case ACS_APSESH_JtpReturnType::JTP_NO_CONNECTION:
            // No SESH connection.
            break;

         case ACS_APSESH_JtpReturnType::JTP_NO_DATA:
            // A connection is currently established, but no Quorum Data has been
            // received.  If a Quorum Dissolve Timer is extant, this means that
            // a new connection has been established, in which case this code
            // assumes that a Quorum is now in place, and cancels the timer
            // accordingly

            if (m_timerActive)
            {
		if(cancelQuorumDissolveTimer() == false){
	       		ignoreQuorumDissolveEvent  = true; //TR IA75890
	        }
            }
            break;

         case ACS_APSESH_JtpReturnType::JTP_QUORUM_DISSOLVE_DISCONNECT:
            // This means SESH has disconnected and it could be due to quorum
            // dissolve event in CP. Start a timer and expect to see new JTP
            // connection to cancel the timer. On timer expiry assume quorum
            // is dissolved in CP and so send a message to CS to clear OPGROUP
            StartActiveTimer();

            apseshEvent = ACS_APSESH_EventMgr::EVENT_JTP_DISCONNECT;
            problemText = "SESH has disconnected.";
            break;

         case ACS_APSESH_JtpReturnType::JTP_DISCONNECT:
            // This means SESH has disconnected, due to either a change in
            // leader CP or a CP restart.
            // This behavior is expected as part of the system's normal operation.
            // We'll raise an event, and then continue receiving data.

            apseshEvent = ACS_APSESH_EventMgr::EVENT_JTP_DISCONNECT;
            problemText = "SESH has disconnected.";
            break;

         case ACS_APSESH_JtpReturnType::JTP_APSESH_SHUTDOWN:
            // The operator has ordered the APSESH service to be shut down. Immediately return.
            keepRunning = false;
            apseshEvent = ACS_APSESH_EventMgr::EVENT_APSESH_SHUTDOWN;
            problemText = "APSESH halted by operator action.";
            break;

         case ACS_APSESH_JtpReturnType::JTP_FAIL:
         case ACS_APSESH_JtpReturnType::JTP_FAIL_UNEXPECTED:
            // Currently, all failures internal to SESH/APSESH will be logged at the same event level.
            apseshEvent = ACS_APSESH_EventMgr::EVENT_APSESH_FAIL;
            problemText = "Internal APSESH JTP failure.";
            break;

         default:
            break;
      }

      if (apseshEvent != ACS_APSESH_EventMgr::NONE)
      {
    	  char msg[DEF_TRACE_MSG_SIZE] = { 0 };
          snprintf(msg, sizeof(msg) - 1, "jtpMgr.getData() returned code %d: %s", resultJtp.returnCode, resultJtp.returnMsg.c_str());

          APSESH_Tracer::instance()->trace(__FUNCTION__, apseshEvent, resultJtp.returnMsg.c_str(), problemText.c_str());
      }

   }

   m_jtpMgr->unregister();

   return 0;
}

/*
 * What should happen when the timer has expired
 *
 */
int ACS_APSESH_Main::handle_timeout(const ACE_Time_Value &tv, const void *arg)
{
   ACE_UNUSED_ARG(tv);
   ACE_UNUSED_ARG(arg);

   APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered...", true, LOG_LEVEL_WARN);

   char msg[DEF_TRACE_MSG_SIZE] = { 0 };

   // Start of TR IA75890
   if (ignoreQuorumDissolveEvent  == false){
   	ACS_CS_API_SET_NS::CS_API_Set_Result result = ACS_CS_API_Set::setOpGroupDissolved();

   	// Cancel the timer
   	cancelQuorumDissolveTimer();

   	if (ACS_CS_API_SET_NS::Result_Success == result)
   	{
        	snprintf(msg, sizeof(msg) - 1, "Quorum dissolved successfully");
      		APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_WARN);
   	}
   	else
   	{
      		snprintf(msg, sizeof(msg) - 1, "Quorum dissolve FAILED");
      		APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_ERROR);
   	}
   }
   else{
	cancelQuorumDissolveTimer();
   	ignoreQuorumDissolveEvent = false;
	snprintf(msg, sizeof(msg) - 1, "Perviously cancelQuorumDissolveTimer has failed, but JTP connection is reestablished. So Quorum is not dissolved");
	APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_ERROR);
   }
   // End of TR IA75890

   APSESH_Tracer::instance()->trace(__FUNCTION__, "...Leave", true, LOG_LEVEL_WARN);
   return 0;
}

bool ACS_APSESH_Main::cancelQuorumDissolveTimer()
{
	APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered...");

    char msg[DEF_TRACE_MSG_SIZE] = { 0 };

    int returnVal = m_ActiveTimer.cancel(m_ActiveTimerId);

    // Cancellation failed
    if (returnVal != 0)
    {
        ACE_OS::snprintf(msg, sizeof(msg) - 1, "Failed to cancel quorum dissolve Timer with error code : %d", returnVal);
        APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_ERROR);
	
	// Start of TR IA75890
	if(ignoreQuorumDissolveEvent == true)
	{
		m_timerActive = false;

		// reset the JTP read interval
		m_jtpMgr->resetJtpWaitTimeout();
		ACE_OS::snprintf(msg, sizeof(msg) - 1, "Internal timer flag is set to false; JTP interval is reset since timer expired & JTP connection is reestablished");
		APSESH_Tracer::instance()->trace(__FUNCTION__, msg, true, LOG_LEVEL_ERROR);
		//"Internal timer flag is set to false; JTP interval is reset since timer expired & JTP connection is reestablished"
	}
	// End of TR IA75890

        return false;
    }
   // Cancellation succeeded
   else
   {
      m_timerActive = false;

      // reset the JTP read interval
      m_jtpMgr->resetJtpWaitTimeout();

      snprintf(msg, sizeof(msg) - 1, "Timer handle is canceled successfully");
      APSESH_Tracer::instance()->trace(__FUNCTION__, msg);
   }
   APSESH_Tracer::instance()->trace(__FUNCTION__, "...Leave");
   return true;
}

void signal_handler(int sig)
{
   switch (sig) {
      case SIGHUP:
    	  syslog(LOG_INFO, "Handled SIGHUP in APSESH");
         break;
      case SIGTERM:
      case SIGINT:
      case SIGTSTP:
    	  syslog(LOG_INFO, "Handled SIGTERM in APSESH");
         exit(0);
         break;
      case SIGPIPE:
		{
			syslog(LOG_INFO, "Handled SIGPIPE in APSESH");
		}
		break;
      default:
         char msg[512] = { 0 };
         snprintf(msg, sizeof(msg) - 1, "Unhandled signal caught: %d \n", sig);
         //tracer->trace(__FUNCTION__, msg);
   }
}

void daemonize() {
   //tracer->trace(__FUNCTION__, "Entered");

   int i, lfp;
   char str[10];

   if (getppid() == 1)
      return; /*  already a daemon */

   i = fork();

   if (i < 0)
      exit(1); /*fork error */

   if (i > 0)
      exit(0); /* parent exits */
   /* child (daemon) continues */

   setsid(); /* obtain a new process group */

   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   close(STDERR_FILENO);

   i = open("/dev/null", O_RDWR);
   dup(i);
   dup(i); /* handle standard I/O */

   umask(027); /* set newly created file permissions */

   chdir(RUNNING_DIR); /* change running directory */

   lfp = open(LOCK_FILE, O_RDWR | O_CREAT, 0640);

   if (lfp < 0)
      exit(1); /* can not open */

   if (lockf(lfp, F_TLOCK, 0) < 0) {
      exit(0); /* can not lock */
   }
   /* first instance continues */
   sprintf(str, "%d\n", getpid());

   write(lfp, str, strlen(str)); /* record pid to lockfile */
   signal(SIGCHLD, SIG_IGN); /* ignore child */
   signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
   signal(SIGTTOU, SIG_IGN);
   signal(SIGTTIN, SIG_IGN);
   signal(SIGHUP, signal_handler); /* catch hangup signal */
   signal(SIGTERM, signal_handler); /* catch kill signal */
}

int setupSignalHandler(const struct sigaction* sigAction)
{
	if( sigaction(SIGINT, sigAction, NULL ) == -1)
	{
		syslog(LOG_INFO, "Error occurred while handling SIGINT in acs_apseshd");
		return -1;
	}

	if( sigaction(SIGTERM, sigAction, NULL ) == -1)
	{
		syslog(LOG_INFO, "Error occurred while handling SIGTERM in acs_apseshd");
		return -1;
	}

	if( sigaction(SIGTSTP, sigAction, NULL ) == -1)
	{
		syslog(LOG_INFO, "Error occurred while handling SIGTSTP in acs_apseshd");
		return -1;
	}

	if( sigaction(SIGPIPE, sigAction, NULL ) == -1)
	{
		syslog(LOG_INFO, "Error occurred while handling SIGPIPE in acs_apseshd");
		return -1;
	}
    return 0;
}

int main(int argc, char *argv[]) {

   ACE_UNUSED_ARG(argc);
   ACE_UNUSED_ARG(argv);

   struct sigaction sa;
	sa.sa_handler = signal_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask );

	// set the signal handler for the main
	setupSignalHandler(&sa);

   ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

   bool noHa = false;

   char * envVar = getenv("NO_HA");

   if (envVar != 0 && !strcmp(envVar, "1")) {
      noHa = true;
   }
   else if((argc == 2 && strcmp(argv[1], "--noha") == 0)){
	   noHa = true;
   }
   else if((argc == 2 && strcmp(argv[1], "--run-foreground") == 0)){
	   noHa = true;
   }
   else if(argc > 1){
   		fprintf(stderr, "This application should be started by HA command!\n   USAGE: acs_apseshd [--noha] \n");
   		return  2;
   	}

   if (noHa)
   {
	   if (!(argc == 2 && strcmp(argv[1], "--run-foreground") == 0))
		   daemonize();

	   APSESH_Tracer::instance()->trace(__FUNCTION__, "Starting APSESH WITHOUT HA");

	   ACS_APSESH_Main workerMainThrd;
	   workerMainThrd.runApsesh();

	   APSESH_Tracer::instance()->trace(__FUNCTION__, "Exit");
   }
   else
   {
	   HAClass *haObj = new HAClass(ACS_APSESH_Main::ACS_APSESH_DAEMON_NAME, ACS_APSESH_Main::ACS_APSESH_USER_NAME);

	   errorCode = haObj->activate();

	   if (errorCode == ACS_APGCC_HA_FAILURE) {
		   //syslog(LOG_ERR, "APSESH, HA Activation Failed!!");
		   APSESH_Tracer::instance()->trace(__FUNCTION__, "APSESH, HA Activation Failed!! ", true, LOG_LEVEL_WARN);
		   delete haObj;
		   return ACS_APGCC_FAILURE;
	   }

	   if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE) {
		   //syslog(LOG_ERR, "APSESH, HA Application Failed to Gracefully closed!!");
		   APSESH_Tracer::instance()->trace(__FUNCTION__, "APSESH, HA Application Failed to Gracefully closed!!", true, LOG_LEVEL_WARN);
		   delete haObj;
		   return ACS_APGCC_FAILURE;
	   }

	   if (errorCode == ACS_APGCC_HA_SUCCESS) {
		   //syslog(LOG_INFO, "APSESH, HA Application Gracefully closed!!");
		   APSESH_Tracer::instance()->trace(__FUNCTION__, "APSESH, HA Application Gracefully closed!!", true, LOG_LEVEL_WARN);
		   ACE_Thread_Manager::instance()->join(haObj->get_application_thread_handle());
		   //syslog(LOG_INFO, "APSESH, HA Application - thread is finished!!");
		   APSESH_Tracer::instance()->trace(__FUNCTION__, "APSESH, HA Application - thread is finished!!", true, LOG_LEVEL_WARN);
		   delete haObj;
		   return ACS_APGCC_SUCCESS;
	   }

	   //syslog(LOG_INFO, "APSESH, service terminated.");
	   APSESH_Tracer::instance()->trace(__FUNCTION__, "APSESH, service terminated.", true, LOG_LEVEL_WARN);
	   delete haObj;
   }

   return errorCode;
}


