//******************************************************************************
//
// NAME
//      acs_lct_server.h
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
//      Header file for lct server
//              
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//      10/26/2010  
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//			2012-25-04      XFURULL         welcomemessage system 
//							improvement(OP#345) on all ports
//******************************************************************************  


/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#if !defined(_ACS_LCT_Server_H_)
#define _ACS_LCT_Server_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <string>
#include <vector>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include "acs_lct_common.h"
#include "acs_lct_command_handler_hardening.h"
#include "acs_lct_command_handler_tsusers.h"
#include "acs_lct_command_handler_securityhandling.h"
#include <acs_apgcc_oihandler_V3.h>
#include <ace/ACE.h>
#include <ace/Process.h>
#include <ace/Task.h>
#include <ace/Event.h>
//#include <algorithm>
#include <sys/stat.h>
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_ApplicationManager.h"
#include "acs_lct_adm.h"
//#include "acs_lct_struct_define.h"

using namespace std;
#define LINE_SIZE 200
#define SIG_SHUTDOWN_HARDENING SIGRTMIN+1
#define SIG_SHUTDOWN_TSUSERS SIGRTMIN+2
#define SIG_SHUTDOWN_BANNER SIGRTMIN+3
#define COM_LOCK "/tmp/com_lock"
/*====================================================================
                        VARIABLE DECLARATION SECTION
==================================================================== */

// static const char* CONFIG_IMM_CLASS_NAME = "AxeHardeningHardeningM";
static const char* CONFIG_IMM_CLASS_NAME = "AxeApSessionManagementApSessionM";
static const char* CONFIG_IMM_CLASS_NAME1 = "AxeLocalTsUsersPolicyLocalTsUsersPolicyM";
static const char* CONFIG_IMM_CLASS_NAME2 = "SecurityHandling";
extern std::string hardeningDnName;
extern std::string tsUsersDnName;
extern std::string securityhandlingDnName;
struct imm_replica
{
	ACE_UINT32 maximumPasswordAge;
	ACE_UINT32 maximumAccountAge;
	ACE_UINT32 minimumPasswordAge;
	ACE_UINT32 minimumPasswordLength;
	ACE_UINT32 passwordHistorySize;
	ACE_UINT32 lockoutDuration;
	ACE_UINT32 lockoutBadCount;
	ACE_UINT32 inactivityTimer;
	string welcomeMessage;
	ACE_INT32 auditUserLevelMessages;
	ACE_INT32 auditSystemDaemons;
	ACE_INT32 auditSyslog;
	ACE_INT32 auditCronMessages;
	ACE_INT32 auditFTPServer;
	ACE_INT32 auditSSHServer;
	ACE_INT32 auditTelnetServer;
};

extern imm_replica immrep;

static string attributes[]={"maximumPasswordAge","maximumAccountAge","minimumPasswordAge","minimumPasswordLength","passwordHistorySize","lockoutBadCount","lockoutDuration","inactivityTimer","welcomeMessage","auditUserLevelMessages","auditSystemDaemons","auditSyslog","auditCronMessages","auditFTPServer","auditSSHServer","auditTelnetServer"};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
    @class              ACS_LCT_Server

    @brief              This class implements the Hardening service functionality
**/
 /*=================================================================== */

class ACS_LCT_Server : public ACS_APGCC_ApplicationManager
{
/*=============================================================================
            PUBLIC DECLARATION SECTION
  =============================================================================*/

   public:
   ACS_APGCC_BOOL Is_Active;
   ACE_UINT32 passiveToActive;
   static void handle_signal(int signum);
/*=====================================================================
                                CLASS CONSTRUCTORS
 ==================================================================== */

        /*=================================================================== */

                /**

                        @brief                  Server constructor

                        @pre

                        @post

                        @return                 none

                        @exception              none
                */

    ACS_LCT_Server();
    ACS_LCT_Server(const char* daemon_name, const char* username);

        /*===================================================================
                                CLASS DESTRUCTOR
        =================================================================== */
        /*=================================================================== */
                /**

                        @brief                  Destructor

                        @pre                    none

                        @post                   none

                        @return                 none

                        @exception              none
                */
        /*=================================================================== */
    ~ACS_LCT_Server();

    /* ===================================================================
				   @brief  startFunctionality() method

                           It is the method used to initialise the hardening service.

                   @return      ACS_CC_ReturnType

                   @exception   none
    =========================================================================*/
    ACS_CC_ReturnType startFunctionality();

    /* ===================================================================
  				   @brief  startDebug() method

                             It is the method used to start the hardening service in debug mode.

                     @return      bool

                     @exception   none
      =========================================================================*/
	bool startDebugMode();
	ACE_INT32 shutdown_lct_debug();

	  /* ===================================================================
					   @brief  shutdown() method

	                           It is the method used to shutdown the reactor event loop.

	                   @return      ACE_INT32

	                   @exception   none
	    =========================================================================*/
	ACE_INT32 shutdown();

	  /* ===================================================================
					   @brief  setupIMMCallBacks() method

	                           It is the method used to setup IMM callbacks to OI.

	                   @return     bool 

	                   @exception   none
	    =========================================================================*/
	bool setupIMMCallBacksForHardening();
	bool setupIMMCallBacksForTsUsers();
	bool setupIMMCallBacksForsecurityhandling();
	//bool setupIMMCallBacksForTsUsers( ACE_Reactor *poReactor );

	  /* ===================================================================
					   @brief  getDnName() method

	                           It is the method used to get Domain Name.

	                   @return      string

	                   @exception   none
	    =========================================================================*/
	string getDnName ();

	/*====================================================
	 * Methods related to HA
	 * ===================================================*/
    ACS_APGCC_ReturnType svc();
    ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
    ACS_APGCC_ReturnType performComponentHealthCheck(void);
    ACS_APGCC_ReturnType performComponentTerminateJobs(void);
    ACS_APGCC_ReturnType performComponentRemoveJobs (void);
    ACS_APGCC_ReturnType performApplicationShutdownJobs();

          /* ===================================================================
                           @brief  startup() method

                           This method is called at the beginning of the service to update all parameters from IMM initially.

                           @return      bool

                           @exception   none
            =========================================================================*/

	bool startup();

          /* ===================================================================
                           @brief  startup() method

                           This method is used to update parameters from IMM in config files.

                           @return      bool

                           @exception   none
            =========================================================================*/

    bool updateFromImm(string attr_name);

          /* ===================================================================
                           @brief  fetchDnOfRootObjFromIMM() method

                           This method is used to DN of root from IMM.

                           @return      bool

                           @exception   none
            =========================================================================*/

    bool fetchDnOfRootObjFromIMM ();
    bool configapCheck();
    ACS_APGCC_ReturnType startHardeningFunctionality();	
    ACS_APGCC_ReturnType startTsUsersFunctionality();	
    ACS_APGCC_ReturnType startsecurityhandlingFunctionality();	
    ACE_thread_t hardeningThreadId;
    ACE_thread_t tsUsersThreadId;
    ACE_thread_t securityhandlingThreadId;
    //void static sighup_handler(int sig);
    //void static sig_hardening_handler(int sig);
    //void static sig_tsusers_handler(int sig);
    ACE_INT32 shutdown_hardening();
    ACE_INT32 shutdown_tsusers();
    ACE_INT32 shutdown_securityhandling();
  //  void removeObjects();
/*=============================================================================
            PRIVATE DECLARATION SECTION
  =============================================================================*/
   
private:

/*=============================================================================
            PRIVATE ATTRIBUTES 
  =============================================================================*/

    ACE_TP_Reactor *m_poTPReactorImpl;
    //ACE_TP_Reactor *m_poTPReactorImpl1;
    ACS_LCT_CommandHandler_Hardening* m_poCommandHandler;
    ACS_LCT_CommandHandler_TSUsers* m_poCommandHandler1;
    ACS_LCT_CommandHandler_securityhandling* m_poCommandHandler2;
    ACE_Reactor* m_poReactor;
    //ACE_Reactor* m_poReactor1;
    int readWritePipe[2];
    int is_hardening_reactor;
    int is_tsuser_reactor;
    int is_securityhandling_reactor;

    //ACS_APGCC_BOOL Is_terminated;
    //char* CONFIG_IMM_CLASS_NAME;
    
    ACS_LCT_Adm *m_AdmObj;
    //ACE_UINT32 passiveToActive;
    ACS_APGCC_BOOL m_ShutdownInprogress;

    ACS_APGCC_ReturnType activateApp();
    ACS_APGCC_ReturnType passifyApp();
    ACS_APGCC_ReturnType shutdownApp();
						    
    
};

#endif /* _ACS_LCT_Server_H_ */

