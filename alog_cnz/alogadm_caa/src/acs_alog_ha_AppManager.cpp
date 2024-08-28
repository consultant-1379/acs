/*=================================================================== */
/**
  @file acs_alog_ha_AppManager.cpp

  Class method implementationn for acs_alog_ha_AppManager type module.

  This module contains the implementation of class declared in
  the acs_alog_ha_AppManager.h module

  @version 1.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       02/02/2011     xgencol/xgaeerr       Initial Release
  N/A       28/11/2011     xgencol               Final Release
  =================================================================== */

#include <acs_alog_ha_AppManager.h>

#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/Task.h>
#include <ace/LSOCK_Connector.h>
#include <ace/ACE.h>
#include <sys/prctl.h>
#include <sys/capability.h>
#include <fstream>


extern ACE_THR_FUNC_RETURN run_alog_daemon_HA(void *);   	 	// defined in acs_alog_main.cpp
extern ACE_THR_FUNC_RETURN HandleCmd(void *);
extern void stop_alog_daemon_HA();			                // defined in acs_alog_main.cpp
extern ACS_ALOG_ReturnType removeImplementer();
extern ACS_APGCC_ReturnType  setWorkingPath (void);

int  State = 0;

acs_alog_BrfImplementer  BRFCimpl;
acs_alog_BrfcThread  BRFCthread(&BRFCimpl);

extern int  APtype;            //  AP1 = 1   AP2 = 2

extern acs_alog_activeWorks  oWorksObj;
extern string  CurrentNodeState;
extern int     Failure;
const char pipeName[] = "/var/run/ap/ACS_ALOG_MainCmdPipe";



ACS_ALOG_HA_AppManager::ACS_ALOG_HA_AppManager(const char* daemon_name)
: ACS_APGCC_ApplicationManager(daemon_name)
{
//	Is_terminated = FALSE;
	alog_worker_thread_id = 0;
	alog_command_thread_id = 0;
//	alog_ha_application_thread_id = 0;

	initCapability();

	/* create the pipe for shutdown handler */
	if ( (pipe(readWritePipe)) < 0) {
		syslog(LOG_ERR, "pipe creation FAILED");
	}

	if ( (fcntl(readWritePipe[0], F_SETFL, O_NONBLOCK)) < 0) {
		syslog(LOG_ERR, "pipe fcntl on readn");
	}

	if ( (fcntl(readWritePipe[1], F_SETFL, O_NONBLOCK)) < 0) {
		syslog(LOG_ERR, "pipe fcntl on writen");
	}
}


void  ACS_ALOG_HA_AppManager::initCapability ()
{

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Entering in ACS_ALOG_HA_AppManager::initCapability()"));
	cap_t  cap = NULL;
	cap_value_t cap_list[1];

	cap_list[0] = CAP_SYS_RESOURCE;

	if (prctl(PR_CAPBSET_DROP, CAP_SYS_RESOURCE) == 0 )		    //  Set inheritance capability propagation
	{
			cap = cap_get_proc();			// allocates a capability state in working storage

			if (cap != NULL)
			{
				if ((cap_set_flag(cap, CAP_EFFECTIVE, 1, cap_list, CAP_CLEAR) == -1)   ||
					(cap_set_flag(cap, CAP_INHERITABLE, 1, cap_list, CAP_CLEAR) == -1) ||
					(cap_set_flag(cap, CAP_PERMITTED, 1, cap_list, CAP_CLEAR) == -1))
				{
						ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("initCapability() - Error to remove capability flag"));
				}
				else {
						if (cap_set_proc(cap) == 0)			// Change capability
						{
								ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("initCapability() - Capability set correctly"));
						}
						else {
								ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("initCapability() - Error to change capability"));
						}
				}
				if (cap_free(cap) == -1)
				{
						ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("initCapability() - Error to free capability"));
				}
			}
			else  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("initCapability() - Error to allocates a capability"));
	}
	else  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("initCapability() - Error to set capability inheritance propagation"));

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Exiting from ACS_ALOG_HA_AppManager::initCapability()"));
}


int  ACS_ALOG_HA_AppManager::createThreads()
{

		int  ret = 0;

		if (alog_worker_thread_id == 0)			/* create ALOG worker executor thread */
		{
				const ACE_TCHAR* thread_name = "WorkerThread";
				int call_res = ACE_Thread_Manager::instance()->spawn(& run_alog_daemon_HA,
										(void *)this,
										THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
										& alog_worker_thread_id,
										0,
										ACE_DEFAULT_THREAD_PRIORITY,
										-1,
										0,
										ACE_DEFAULT_THREAD_STACKSIZE,
										&thread_name);
				if (call_res == -1)
				{
						syslog(LOG_ERR, "Error creating ALOG Worker Thread");
						Failure = 1;
						return ret;
				}

				ret = 1;
				syslog(LOG_INFO, "HA Application : the ALOG Worker Thread has been successfully created !");
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("The ALOG Worker Thread has been successfully created"));
		}

		if (alog_command_thread_id == 0)			/* create PLOG commands executor thread */
		{
				const ACE_TCHAR* thread_name = "HandleCmdThread";
				int call_res = ACE_Thread_Manager::instance()->spawn(
										&HandleCmd, (void *) this,
										THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
										&alog_command_thread_id, 0, ACE_DEFAULT_THREAD_PRIORITY,
										-1, 0, ACE_DEFAULT_THREAD_STACKSIZE, &thread_name);
				if (call_res == -1)
				{
						syslog(LOG_ERR, "Error creating PLOG Command Thread");
						Failure = 1;
//						return ACS_APGCC_FAILURE;
						return ret;
				}

				syslog(LOG_INFO, "HA Application : the PLOG Commands Thread has been successfully created !");
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("The PLOG Commands Thread has been successfully created"));
		}

		return  ret;
}


void  ACS_ALOG_HA_AppManager::stopActivities()
{

			//unlink ("/var/run/ap/ALOG_API_pipe");		//  TR HQ55046 :  the ALOG server is locked

			if (State)
			{
					if (removeImplementer())  syslog(LOG_INFO, "Remove implementers failed !");
					if (APtype != 2)  deleteParticipant();
			}

			// wait Worker Thread termination
			if (alog_worker_thread_id != 0)
			{
						stop_alog_daemon_HA();
						oWorksObj.setnoWork(0);
						ACE_Thread_Manager::instance()->join(alog_worker_thread_id);
//						ACE_Thread::cancel (alog_worker_thread_id);
						syslog(LOG_INFO, "HA Application Thread: stop thread Worker done!");
						ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("HA Application Thread: stop thread Worker done"));
						alog_worker_thread_id = 0;
			}

			if (alog_command_thread_id != 0)
			{
					ACE_LSOCK_Stream Stream;
					ACE_LSOCK_Connector Connector;
					ACE_UNIX_Addr addr(pipeName);

					Connector.connect(Stream, addr);

					CmdData m_srctCmdData;
					const ACE_TCHAR * lpszData1 = "SHUTDOWN";
					strcpy(m_srctCmdData.Data1,lpszData1);
					void *dest;
					dest = malloc(sizeof(m_srctCmdData));
					memcpy(dest, &m_srctCmdData, sizeof(m_srctCmdData));
					Stream.send_n(dest, sizeof(m_srctCmdData));
					Stream.close();
					free (dest);

					ACE_Thread_Manager::instance()->join(alog_command_thread_id);
					alog_command_thread_id = 0;
			}
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	char str_to_log[1024] = {0};

	snprintf(str_to_log,sizeof(str_to_log),"acs_alog_ha_AppManager::performStateTransitionToActiveJobs() -> previous state is :%i",previousHAState);

	syslog(LOG_INFO, str_to_log );
	ACSALOGLOG(LOG_LEVEL_WARN,str_to_log);
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("acs_alog_ha_AppManager::performStateTransitionToActiveJobs() invoked"));

	/* Check if we have received the ACTIVE State again.
	 * This means that, our application is already Active and
	 * again we have got a callback from AMF to go active.
	 * Ignore this case anyway. This case should rarely happen
	 */
	if(ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
		return ACS_APGCC_SUCCESS;

	/* Our application has received state ACTIVE from AMF.
	 * Start off with the activities needs to be performed
	 * on ACTIVE
	 */

	syslog(LOG_INFO, "ALOG Daemon received ACTIVE state assignment!!!");
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("ALOG Daemon received ACTIVE state assignment"));

//	int  fd = ::open("/var/run/ap/ALOG_API_pipe", O_RDWR);		//  TR HQ55046 :  the ALOG server is NOT locked
//
//	if (fd == -1)  system ("mkfifo -m 0666 /var/run/ap/ALOG_API_pipe");
//	else		   ::close (fd);

	createThreads();

	CurrentNodeState = "active";
	State = 1;

	oWorksObj.setnoWork(1);

	if (registerImplementers())  syslog(LOG_ERR,"RegisterImplementers failure !!!");
	if (oWorksObj.alogInit())    syslog(LOG_ERR,"AlogInit failure !!!");

	setWorkingPath ();

	if (APtype != 2)  createBrfcThread();

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	syslog(LOG_INFO, "acs_alog_ha_AppManager::performStateTransitionToPssiveJobs() invoked !" );
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("acs_alog_ha_AppManager::performStateTransitionToPssiveJobs() invoked"));

	/* Check if we have received the PASSIVE State Again.
	 * This means that, our application was already Passive and
	 * again we have got a callback from AMF to go passive.
	 * Ignore this case anyway. This case should rarely happen.
	 */

	if(ACS_APGCC_AMF_HA_STANDBY == previousHAState)
		return ACS_APGCC_SUCCESS;

	stopActivities();

	State = 0;
	CurrentNodeState = "passive";

	syslog(LOG_INFO, "HA Application : Application is Passive");
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("HA Application : Application is Passive"));

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{

	(void)previousHAState;

	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	syslog(LOG_INFO, "ALOG Daemon received QUIESING state assignment!!!");
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("ALOG Daemon received QUIESING state assignment"));

	stopActivities();

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{

	(void)previousHAState;

	/* We were Active and now losting Active state due to Lock admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	syslog(LOG_INFO, "ALOG Daemon received QUIESCED state assignment!");
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("ALOG Daemon received QUIESCED state assignment"));

	stopActivities();

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performComponentHealthCheck(void)
{

	/* Application has received health check callback from AMF. Check the
	 * sanity of the application and reply to AMF that you are ok.
	 */
	syslog(LOG_INFO, "ALOG Daemon received healthcheck query!!!");

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performComponentTerminateJobs(void)
{
	/* Application has received terminate component callback due to
	 * LOCK-INST admin operation performed on SU. Terminate the thread if
	 * we have not terminated in performComponentRemoveJobs case or double
	 * check if we are done so.
	 */
	syslog(LOG_INFO, "ALOG Daemon received terminate callback!!!");
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("ALOG Daemon received terminate callback"));

	stopActivities();

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performComponentRemoveJobs(void)
{

	/* Application has received Removal callback. State of the application
	 * is neither Active nor Standby. This is with the result of LOCK admin operation
	 * performed on our SU. Terminate the thread by informing the thread to go "stop" state.
	 */

	syslog(LOG_INFO, "Application Assignment is removed now");
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Application Assignment is removed now"));

	stopActivities();

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType ACS_ALOG_HA_AppManager::performApplicationShutdownJobs() {

	syslog(LOG_ERR, "Shutting down the application");
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Shutting down the application"));

	stopActivities();

	return ACS_APGCC_SUCCESS;
}


/*=================================================================
ROUTINE: createBrfcThread
=================================================================== */
void ACS_ALOG_HA_AppManager::createBrfcThread()
{
	if (BRFCthread.isRunning() == true)  return;

ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("PLOG :  createBrfcThread"));

/*
	createParticipant();

	const char *dnObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-ALOG,brfParticipantContainerId=1";
	const char *impName = "ACS_ALOGBrfc_Impl";
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	BRFCimpl.setObjName(dnObjName);
	BRFCimpl.setImpName(impName);
	BRFCimpl.setScope(scope);

	BRFCthread.setImpl(&BRFCimpl);
*/
	BRFCthread.start();
	BRFCthread.activate();
}

/*=================================================================
ROUTINE: createParticipant
=================================================================== */
/* to be integrate into your setup code if object is not exits */
int ACS_ALOG_HA_AppManager::createParticipant(int * apgcc_error_code_p, char * error_message_buf, size_t error_message_buf_size)
{
	char attrdn[] = "brfPersistentDataOwnerId";
	char attrVersion[] = "version";
	char attrBackupType[] = "backupType";
	char *className = const_cast<char*>("BrfPersistentDataOwner");

	ACS_CC_ReturnType returnCode;

	OmHandler immHandler;

	returnCode = immHandler.Init();
	if (returnCode != ACS_CC_SUCCESS) {
		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("Error in Init() function for BrfPersistentDataOwner Object"));
		if(apgcc_error_code_p)
			*apgcc_error_code_p = immHandler.getInternalLastError();
		if(error_message_buf && error_message_buf_size)
			snprintf(error_message_buf, error_message_buf_size, "%s", immHandler.getInternalLastErrorText());
		return -1;
	}

	char* nomeParent = const_cast<char*>("brfParticipantContainerId=1");

	//Create attributes list
	vector < ACS_CC_ValuesDefinitionType > AttrList;
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeVersion;
	ACS_CC_ValuesDefinitionType attributeBackupType;

	/*Fill the rdn Attribute */
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ERIC-APG-ACS-ALOG");
	void* value[1] = { reinterpret_cast<void*>(rdnValue) };
	attributeRDN.attrValues = value;

	attributeVersion.attrName = attrVersion;
	attributeVersion.attrType = ATTR_STRINGT;
	attributeVersion.attrValuesNum = 1;
	char* strValue = const_cast<char*>("1.0");
	void* valueStr[1] = { reinterpret_cast<void*>(strValue) };
	attributeVersion.attrValues = valueStr;

	attributeBackupType.attrName = attrBackupType;
	attributeBackupType.attrType = ATTR_INT32T;
	attributeBackupType.attrValuesNum = 1;
	int intAttributeBackupType = 1;
	void* valueInt[1] = { reinterpret_cast<void*>(&intAttributeBackupType) };
	attributeBackupType.attrValues = valueInt;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeVersion);
	AttrList.push_back(attributeBackupType);

//  The following nested IF solve the TR HR24286 :
//  it was reproduced and tested applying the cmw-cluster-reboot command

	ACS_CC_ReturnType createObjectRes = immHandler.createObject(className, nomeParent, AttrList);
	if (createObjectRes != ACS_CC_SUCCESS)
	{
		usleep (700000);
		createObjectRes = immHandler.createObject(className, nomeParent, AttrList);
		if (createObjectRes != ACS_CC_SUCCESS)
		{
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("CreateParticipant :  Object Creation Failure"));
			if(apgcc_error_code_p)
			{
				*apgcc_error_code_p = immHandler.getInternalLastError();
				if(error_message_buf && error_message_buf_size)
						snprintf(error_message_buf, error_message_buf_size, "( %d ) --- %s", *apgcc_error_code_p, immHandler.getInternalLastErrorText());
			}
		}
	}			//  end TR HR24286 solution

	returnCode = immHandler.Finalize();
	if (returnCode != ACS_CC_SUCCESS)
	{
		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("Error in Finalize() function for BrfPartecipantDataOwner CreateObject"));
		// if also previous 'createObject()' operation failed, we prefer to not override error code and error message returned to caller.
		if((createObjectRes == ACS_CC_SUCCESS) && apgcc_error_code_p)
			*apgcc_error_code_p = immHandler.getInternalLastError();
		if((createObjectRes == ACS_CC_SUCCESS) && error_message_buf && error_message_buf_size)
			snprintf(error_message_buf, error_message_buf_size, "%s", immHandler.getInternalLastErrorText());
		return -1;
	}

	return (createObjectRes == ACS_CC_SUCCESS) ?  0 : -1;
}


int ACS_ALOG_HA_AppManager::deleteParticipant()
{
	if (BRFCthread.isRunning() == false) return 0;

	char error_message[256] = {0};
	unsigned int attempt_interval_usec = 1000000;	// interval between consecutive operation attempts (in microseconds)

	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("PLOG :  Entering  'deleteParticipant'"));

	while(BRFCthread.deleteImpl(&BRFCimpl))
		::usleep(attempt_interval_usec);

	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("PLOG : stopping BRFC thread and waiting termination' ..."));

	BRFCthread.stop();
	BRFCthread.wait();

	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("PLOG : BRFC thread terminated !"));

	const char* dn = "brfPersistentDataOwnerId=ERIC-APG-ACS-ALOG,brfParticipantContainerId=1";

	int n_attempts = 5;
	int n_timeout_errors = 0;
	bool stop_loop = false;
	for(int i = 0; (i < n_attempts) && !stop_loop; ++i)
	{
		OmHandler omHandler;
		if (omHandler.Init() == ACS_CC_FAILURE)
		{
			::snprintf(error_message, sizeof(error_message), "DeletePartecipant :  Init error ! Details : %s ", omHandler.getInternalLastErrorText());
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(error_message));
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("PLOG :  Exiting  'deleteParticipant'"));
			return -1;
		}

		if(omHandler.deleteObject(dn) != ACS_CC_SUCCESS)
		{
			::snprintf(error_message, sizeof(error_message), "DeletePartecipant :  deleteObject error ! Details : %s ", omHandler.getInternalLastErrorText());
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(error_message));
			int apgcc_error_code = omHandler.getInternalLastError();
			if(apgcc_error_code == -5 /* ERR_IMM_TIMEOUT */)
				++ n_timeout_errors;

			if( (apgcc_error_code == -12 /* ERR_IMM_NOT_EXIST */) ||
				(apgcc_error_code == -9 /* ERR_IMM_BAD_HANDLE */) ||
				(n_timeout_errors == 2) )
				stop_loop = true;	// exit loop, It's useless to continue
		}
		else
			stop_loop = true;

		if (omHandler.Finalize() == ACS_CC_FAILURE)
		{
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("DeleteParticipant :  Finalize error"));
		}

		::usleep(attempt_interval_usec); // wait a few before next attempt
	}

	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("PLOG :  Exiting  'deleteParticipant'"));
	return 0;
}

