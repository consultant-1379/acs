#include "ACS_DSD_HA_AppManager.h"
#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_PrimitiveFactory.h"
#include "ACS_DSD_ServiceAcceptorTask.h"
#include "ACS_DSD_Task_Base.h"
#include "ACS_DSD_SignalHandler.h"
#include "ACS_DSD_Imm_functions.h"
#include "ACS_DSD_ImmConnectionHandler.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_ImmCpNodesUpdater.h"
#include "ACS_DSD_NotificationThread.h"
#ifdef ACS_DSD_TEST
#include "ACS_DSD_Test.h"
#endif
#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"
#include "ace/Condition_T.h"
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <fstream>
#include <string>
#include <getopt.h>
#ifdef ACS_DSD_SERVER_MCHECK_TRACE_ENABLED
#include <stdio.h>
#include <mcheck.h>
#endif

#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_RemoteApNodesManager.h"
#include "ACS_DSD_CSHwcTableChangeObserver.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"
#include <boost/filesystem.hpp>
// Global objects
ACS_DSD_HA_AppManager *haObj;										// used to integrate DSD Server with HA framework
ACS_DSD_CpNodesManager cpNodesManager;                              // used to keep information about the CP nodes currently connected to DSD Server
ACS_DSD_ImmCpNodesUpdater immCpNodesUpdater;                        // thread dedicated to update the IMM objects associated to currently connected CP nodes
ACS_DSD_Node dsd_local_node;                                        // local node info (set during startup)
ACS_DSD_PrimitiveFactory dsd_primitive_factory;                     // used to create DSD_ServicePrimitive objects
ACS_DSD_SignalHandler dsd_sig_handler;                              // used to handle signals and to handle timers
ACE_TP_Reactor * service_acceptor_reactor_impl = 0;                 // Service acceptor reactor implementation
ACE_Reactor * sa_reactor = 0;                                       // Service acceptor reactor interface
ACE_TP_Reactor * service_handler_reactor_impl = 0;                  // Service handling reactor implementation
ACE_Reactor * sh_reactor = 0;                                       // Service handling reactor interface
ACE_TP_Reactor *sig_and_timers_reactor_impl = 0;                    // Implementation of signal and timers reactor
ACE_Reactor *sig_and_timers_reactor = 0;                            // Reactor used for catching signals and handling timers
ACS_DSD_ServiceAcceptorTask * p_serviceAcceptorTask = 0;            // Task (thread pool) dedicated to accept service connections
ACS_DSD_Task_Base * p_serviceHandlerTask = 0;                       // Task (thread pool) dedicated to handle established service connections
ACS_DSD_Task_Base * p_SignalHandlerTask = 0;						// Task (thread pool) used to handle signals and timers
ACS_DSD_Logger dsd_logger;
ACE_Thread_Mutex dsd_stop_mutex;									// mutex used to stop DSD Server
ACE_Condition<ACE_Thread_Mutex> *p_dsd_stop_condition = 0;			// condition used to stop DSD Server
bool dsd_is_stopping = false;										// indicates if the DSD Server has entered the "stopping" phase
ACS_DSD_NotificationThread notification_thread;						// thread used to receive notifications from NTF Service
ACS_DSD_StartupSessionsGroup * startup_session_group = 0; // Groups all the startup session handlers
ACS_DSD_RemoteApNodesManager remote_AP_nodes_manager;		// Used to keep information about the startup-sessions opened with the remote DSD servers
ACS_DSD_CSHwcTableChangeObserver configuration_change_observer;			// Used to be notified of HWC Table changes
const char* const SWMVERSION="/cluster/storage/system/config/apos/swm_version";
bool isSWM20();

#ifdef ACS_DSD_TEST
ACS_DSD_Test * p_testTask = 0;																			// Task (thread pool) dedicated only to test purposes
int is_AP1 = 0;																											// The test task will be created only for AP1
#endif
ACE_Recursive_Thread_Mutex run_dsd_daemon_work_func_mutex;			// this mutex is used to ensure that only one thread is in the "run_dsd_daemon_work()" function

#ifdef ACS_DSD_SERVER_MCHECK_TRACE_ENABLED
char mcheck_file_name[1024] = {0};
FILE * mcheck_file_p = 0;
#endif

/* Memory buffers to keep ACS_DSD_ImmConnectionHandler and ACS_DSD_StartupSessionsGroup global instances */
namespace {
	char immConnHandlerObj_membuffer [sizeof(ACS_DSD_ImmConnectionHandler)];
	char imm_shared_objects_conn_membuffer [sizeof(ACS_DSD_ImmConnectionHandler)];
	char startup_session_group_membuffer [sizeof(ACS_DSD_StartupSessionsGroup)];
	pid_t parent_pid = -1;
}

// ... global objects used to handle access to IMM
ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr = 0;           // connection to IMM service used for operations on PRIVATE OBJECTS
ACS_DSD_ImmConnectionHandler * imm_shared_objects_conn_ptr = 0;     // connection to IMM service used for operations on SHARED OBJECTS

ACE_Recursive_Thread_Mutex imm_conn_mutex;                          // mutex used for controlling access to <immConnHandlerObj> global variable
ACE_Recursive_Thread_Mutex imm_shared_objects_conn_mutex;           // mutex used for controlling access to <imm_shared_objects_conn> global variable
ACE_Recursive_Thread_Mutex imm_manteinance_op_mutex	;				// mutex used for critical operations requiring exclusive access to all IMM connections
int is_imm_shared_objects_controller = 0;                           // indicates if this DSD Server instance is the controller of shared IMM objects

// ... global objects used to represents command-line options
int cflag = 0;
int nohaflag = 0;

// forward declarations
ACE_THR_FUNC_RETURN run_dsd_daemon_work(void *);
void stop_dsd_daemon_work();
void cleanup();
int parse_command_line(int argc, char **argv);
int check_for_multiple_instances_running();
bool is_swm_2_0 = true;

int main(int argc, char **argv)
{
	parent_pid = ACE_OS::getpid();

#ifdef ACS_DSD_SERVER_MCHECK_TRACE_ENABLED
	// Call the mcheck function in order to check the presence of memory inconsistency
	::mcheck(0);

	::snprintf(mcheck_file_name, ACS_DSD_ARRAY_SIZE(mcheck_file_name),
			"/var/log/acs/acs_dsdd_memcheck_%05d.txt", parent_pid);
	mcheck_file_p = ::fopen(mcheck_file_name, "w+t");

	// Call the mtrace function in order to trace the memory allocation/deallocation
	::mtrace();
#endif
	is_swm_2_0 = isSWM20();
	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

	// parse command line
	if(parse_command_line(argc, argv) < 0)
	{
		fprintf(stderr, "USAGE: acs_dsdd [-c] [--noha] \n");
		return  DSD_SERVER_EXIT_CODE_WRONG_USAGE;
	}

	// if the DSD Server has been launched with "--noha" option, we must avoid registration with AMF
	if(nohaflag)
		return (long)run_dsd_daemon_work(0);

	/**
	 *  instantiate the Application Manager object that must be used to integrate DSD Server with HA framework.
	 *  This instantiation daemonizes DSD Server application.
	 */
	haObj = new (std::nothrow) ACS_DSD_HA_AppManager(ACS_DSD_DAEMON_NAME);
	if(!haObj)
	{
		syslog(LOG_ERR, "acs_dsdd, Application Manager object creation failed !");
		return DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
	}

	ACS_DSD_SERVER_MCHECK_TRACE_PRINT("Application Manager object creation", ::mprobe(haObj));

	syslog(LOG_INFO, "Starting acs_dsdd service.. ");

	/**
	 * initialize DSD daemon towards AMF and start work (it's a blocking call). In this method the following actions are essentially executed:
	 * 	1- initialization of DSD Daemon towards AMF;
	 *  2- an event loop is performed, waiting for command events from AMF and invoking appropriate callbacks to handle them. For example,
	 *  when AMF assigns ACTIVE state to the DSD Server service instance, the ACS_DSD_HA_AppManager::performStateTransitionToActiveJobs() method is invoked
	 *  to handle this command event.
	 */
	errorCode = haObj->activate();

	if (errorCode == ACS_APGCC_HA_FAILURE)
	{
		syslog(LOG_ERR, "acs_dsdd, HA Activation Failed!!");
		delete haObj;
		return DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
	}

	if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
	{
		syslog(LOG_ERR, "acs_dsdd, HA Application Failed to Gracefully closed!!");
		delete haObj;
		return DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
	}

	if (errorCode == ACS_APGCC_HA_SUCCESS)
	{
		syslog(LOG_INFO, "acs_dsdd, HA Application Gracefully closing ... waiting for HA Application Thread termination !");

		// wait for HA Application Thread termination
		//ACE_Thread_Manager::instance()->join(haObj->get_application_thread_handle());
		haObj->wait_ha_application_thread_termination();

		syslog(LOG_INFO, "acs_dsdd, HA Application Gracefully closed!!");
	    delete haObj;
		return DSD_SERVER_EXIT_CODE_OK;
	}

	ACS_DSD_SERVER_MCHECK_TRACE_PRINT("Application Manager object dtor", ::mprobe(haObj));

	delete haObj;

#ifdef ACS_DSD_SERVER_MCHECK_TRACE_ENABLED
	::fclose(mcheck_file_p);
	::muntrace();
#endif

	return DSD_SERVER_EXIT_CODE_OK;
}


/**
 * This function represents the application logic of DSD Daemon. If the DSD Daemon is launched by AMF, this function is executed by the DSD Worker Thread,
 * that is in turn instantiated by the HA Application Thread when AMF assigns for the first time the ACTIVE role to the DSD daemon application component.
 */
ACE_THR_FUNC_RETURN run_dsd_daemon_work(void *)
{
	int exit_code = DSD_SERVER_EXIT_CODE_OK;

	// We must be sure that only one thread is executing the activities performed in this function
	ACE_Guard<ACE_Recursive_Thread_Mutex> run_dsd_daemon_work_func_guard(run_dsd_daemon_work_func_mutex);

	// Check if there's another DSD Server instance running, but only if we're running in 'NOHA' mode
	if(nohaflag && check_for_multiple_instances_running())
	{
		exit_code = DSD_SERVER_EXIT_CODE_CHECK_RUNNING_NOT_OK;
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// initialize logging system
#ifdef ACS_DSD_LOG_USE_LOG4CPLUS
	//open the Log4cplus configuration
	ACS_DSD_Logger::configure("/cluster/ACS/TRA/conf/log4cplus.properties");
#endif
	ACS_DSD_LOGGER_CLASS::open(ACS_DSD_LOG_APPENDER_NAME);

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("DSD Server started (PID = %d, PPID = %d)"),
			ACE_OS::getpid(), parent_pid);

#ifdef ACS_DSD_SERVER_MCHECK_TRACE_ENABLED
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("Mcheck File Name: '%s', File Pointer: %p"),
			mcheck_file_name, mcheck_file_p);
#endif

	// remove DSD Unix domain sockets (eventually) left by previous executions
	ACS_DSD_Utils::clean_dsd_saps(ACS_DSD_ConfigurationHelper::unix_socket_root_path());

	/* Create here the two ACS_DSD_ImmConnectionHandler's instances */
	immConnHandlerObj_ptr = new (immConnHandlerObj_membuffer) ACS_DSD_ImmConnectionHandler();
	imm_shared_objects_conn_ptr = new (imm_shared_objects_conn_membuffer) ACS_DSD_ImmConnectionHandler();

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("IMM connection objects successfully created: PRIVATE OBJECTS conn (%p) - SHARED OBJECTS conn (%p)"), immConnHandlerObj_ptr, imm_shared_objects_conn_ptr);

	// we're going to enable the "NTF notifications handling" feature. NTF Service of CoreMW sends a notification to Notification thread (via a callback)
	// when an AP node changes its role (from ACTIVE to STANDBY or vice versa). If the "NTF notifications handling" feature is enabled, the Notification thread
	// reacts to such notifications executing some actions on IMM connections (more precisely, on the one used to handle the DSD IMM SHARED objects).
	// However, the initialization of IMM connections is performed more forward; so it could happen that an NTF notification arrives to DSD Server when IMM connections have not yet been initialized.
	// In this scenario the notification thread must wait until IMM connection initialization is complete.  We obtain our goal by setting a manteinance lock on IMM connections
	ACE_Guard<ACE_Recursive_Thread_Mutex> imm_conn_manteinance_guard(imm_manteinance_op_mutex);

	// enable handling of NTF notifications (note that the thread is started only at the first invocation of the "start_activity()" method, while
	// the following invocations only enable the NTF notification handling feature (previously disabled by "stop_activity()" method invocation
	notification_thread.start_activity();

	// get info about the AP Node we're running on. Such info are essential, so try until success ...
	std::string error_descr;
	int op_res = -1;
	ACE_Sig_Handler tmp_sig_hdlr; // register a temporary signal handler to handle SIGTERM signal
	tmp_sig_hdlr.register_handler(SIGTERM, & dsd_sig_handler);
	while(op_res < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("Trying to get info about local AP node ... "));
		op_res = ACS_DSD_Utils::get_node_info(dsd_local_node, error_descr);
		if(op_res < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Utils::get_node_info() failure  ! return code is < %d > - description is < %s > !"), op_res, error_descr.c_str());
			ACE_OS::sleep(GET_NODE_INFO_DELAY_INTERVAL);
		}

		// Maybe that in the meantime DSD Server has received a STOP request (SIGTERM signal). If so, exit function
		if(dsd_is_stopping)
		{
			exit_code = DSD_SERVER_EXIT_CODE_INTERRUPTED;
			if(!nohaflag)
				haObj->notify_worker_thread_termination(exit_code);	// notify worker thread termination to the HA Application thread

			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("DSD Server exiting with code < %d >"), exit_code);
			tmp_sig_hdlr.remove_handler(SIGTERM);
			notification_thread.stop_activity();
			return (ACE_THR_FUNC_RETURN) exit_code;
		}
	}
	tmp_sig_hdlr.remove_handler(SIGTERM);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("got info about local AP node !"));

	//Start of TR HY22649
	int init_ret_code = 0;
	int retry = 0;

	do
	{
		// register with IMM and create an SRTNode entry for the AP node we're running on.
		init_ret_code = init_IMM(dsd_local_node, cflag);
		if(init_ret_code < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("init_IMM() failed ! ret_code < %d > retrying..."), init_ret_code);
			usleep(500000);
			finalize_IMM(dsd_local_node, cflag);
			retry++;
		}
	}while((init_ret_code < 0) && retry <= 10);

	//End of TR HY22649

	if(init_ret_code < 0)
	{
		exit_code = DSD_SERVER_EXIT_CODE_IMM_FATAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("init_IMM() failed ! Exiting with code < %d >"), exit_code);
		stop_dsd_daemon_work();		//Fix for TR HY65585
		notification_thread.stop_activity();
		cleanup();
		ACE_OS::sleep(DSD_SERVER_COMP_RESTART_DELAY_FOR_IMM_FATAL_ERRORS);	// wait few seconds before the DSD Server is started again by AMF
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
        return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// now we can release the manteinance lock on IMM connections
	imm_conn_manteinance_guard.release();

	// remove from IMM the SRVInfo objects representing AP services no longer running
	if(unregisterOldServicesfromIMM(dsd_local_node)< 0)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("unregisterOldServicesfromIMM() failed !"));

	// create ACE_Condition object to be used to stop DSD Daemon
	p_dsd_stop_condition = new (std::nothrow) ACE_Condition<ACE_Thread_Mutex>(dsd_stop_mutex);
	if(!p_dsd_stop_condition)
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object : DSD STOP CONDITION ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// create REACTOR to be used to accept service connections
	service_acceptor_reactor_impl = new (std::nothrow) ACE_TP_Reactor();
	if(!service_acceptor_reactor_impl)
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object : SERVICE ACCEPTOR REACTOR IMPLEMENTATION ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}
	sa_reactor = new (std::nothrow) ACE_Reactor(service_acceptor_reactor_impl, true);
	if(!sa_reactor)
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object : SERVICE ACCEPTOR REACTOR ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// create REACTOR to be used for handling service requests on established connections
	service_handler_reactor_impl = new (std::nothrow) ACE_TP_Reactor();
	if(!service_handler_reactor_impl)
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object: SERVICE HANDLER REACTOR IMPLEMENTATION ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}
	sh_reactor = new (std::nothrow) ACE_Reactor(service_handler_reactor_impl, true);
	if(!sh_reactor)
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object: SERVICE HANDLER REACTOR ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// Create the StartupSessionsGroup instance
	startup_session_group = new (startup_session_group_membuffer) ACS_DSD_StartupSessionsGroup();

	// Here we can try to set up the keeped alive communication sessions
	// with remote DSD servers running on other active AP nodes.
	startup_session_group->reactor(sh_reactor);

	// create REACTOR to be used for catching signals and for handling timers
	sig_and_timers_reactor_impl = new (std::nothrow) ACE_TP_Reactor();
	if(!sig_and_timers_reactor_impl)
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object : SIGNALS_AND_TIMERS REACTOR IMPLEMENTATION ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}
	sig_and_timers_reactor = new (std::nothrow) ACE_Reactor(sig_and_timers_reactor_impl, true);
	if(!sig_and_timers_reactor)
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object : SIGNALS_AND_TIMERS REACTOR ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// create Task for accepting connections
	p_serviceAcceptorTask = new (std::nothrow) ACS_DSD_ServiceAcceptorTask(N_SERVICE_ACCEPTOR_THREADS, sa_reactor);
	if(!p_serviceAcceptorTask )
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object: SERVICE ACCEPTOR TASK ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// set the reactor to be used for handling service requests on established connections
	p_serviceAcceptorTask->set_service_handler_reactor(sh_reactor);

	// create Task for handling service requests on established connections
	p_serviceHandlerTask = new (std::nothrow) ACS_DSD_Task_Base(SERVICE_HANDLER_TASK_NAME, N_SERVICE_HANDLER_THREADS, sh_reactor);
	if(!p_serviceHandlerTask )
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object: SERVICE HANDLER TASK ! Exiting with code < %d >"), exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// enable dynamic threads handling feature on ServiceAcceptor task
	p_serviceAcceptorTask->set_dynamic_threads_handling_params(true,TASK_ACCEPTOR_MIN_NUM_THREADS_IDLE, TASK_ACCEPTOR_MAX_NUM_THREADS_IDLE, TASK_ACCEPTOR_MAX_NUM_THREADS);
	// start listening and accepting service connections
	p_serviceAcceptorTask->start_activity();

	// enable dynamic threads handling feature on ServiceHandler task
	p_serviceHandlerTask->set_dynamic_threads_handling_params(true,TASK_SERVICE_HANDLER_MIN_NUM_THREADS_IDLE, TASK_SERVICE_HANDLER_MAX_NUM_THREADS_IDLE, TASK_SERVICE_HANDLER_MAX_NUM_THREADS);
	// start handling service requests on established connections
	p_serviceHandlerTask->start_activity();

	// start the thread that will update IMM for the CP Nodes services
	immCpNodesUpdater.set_cp_nodes_manager(& cpNodesManager);
	immCpNodesUpdater.start_activity();

#ifdef ACS_DSD_TEST
	if((is_AP1 = (dsd_local_node.system_id == acs_dsd::CONFIG_AP_ID_SET_START + 1) ? 1 : 0))
	{ // My node is AP1
		p_testTask = new (std::nothrow) ACS_DSD_Test("TestTask", 1);
		p_testTask->start_activity();
	}
#endif

	// create Task to be used to handle signals and timers
	p_SignalHandlerTask = new (std::nothrow) ACS_DSD_Task_Base( SIG_AND_TIMERS_TASK_NAME, 1, sig_and_timers_reactor);
	if(!p_SignalHandlerTask )
	{
		exit_code = DSD_SERVER_EXIT_CODE_INTERNAL_ERROR;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Memory error allocating object:  TASK  < %s > ! Exiting with code < %d >"),  SIG_AND_TIMERS_TASK_NAME, exit_code);
		notification_thread.stop_activity();
		cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// register for signals and schedule timers
	sig_and_timers_reactor->register_handler(SIGPIPE,&dsd_sig_handler);
	sig_and_timers_reactor->register_handler(SIGINT,&dsd_sig_handler);
	sig_and_timers_reactor->register_handler(SIGTERM,&dsd_sig_handler);
	ACE_Time_Value immconn_chk_interval(N_SECS_IMM_CONN_CHECK_INTERVAL);
	ACE_Time_Value dsd_service_acceptors_chk_interval(N_SECS_DSD_SERVICE_ACCEPTORS_CHECK_INTERVAL);
	sig_and_timers_reactor->schedule_timer(& dsd_sig_handler, IMM_CONN_CHECK_TIMER_NAME, immconn_chk_interval, immconn_chk_interval);
	sig_and_timers_reactor->schedule_timer(& dsd_sig_handler, DSD_SERVICE_ACCEPTORS_CHECK_TIMER_NAME, dsd_service_acceptors_chk_interval, dsd_service_acceptors_chk_interval);

	// start the task dedicated to handle signals and timers
	p_SignalHandlerTask->start_activity();

	// Start to be notified about changes of CS HWC Table
	op_res = configuration_change_observer.start();
	if ( op_res != 0 )
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN,
				ACS_DSD_TEXT ("Call 'configuration_change_observer.start' failed: op_res == %d"), op_res);

	// Open the startup sessions group only if node active
	if (dsd_local_node.node_state == acs_dsd::NODE_STATE_ACTIVE) {
		const int call_result = startup_session_group->connect();

		if (call_result == ACS_DSD_StartupSessionsGroup::SSG_ERROR_NO_MULTI_AP_SYSTEM) {
			// WARNING: DSD Server is running on a NO-MULTI-AP system
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN,
					ACS_DSD_TEXT ("Call 'startup_session_group->connect' failed: NO MULTI AP SYSTEM"));
		} else if (call_result) {
			// ERROR: connecting towards my remote DSD Server friends.
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT ("Call 'startup_session_group->connect' failed: call_result == %d"), call_result);
		} else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT ("StartupSessionsGroup successfully connected towards remote DSD Active Servers: "
					"StartupSessionsGroup state == %d"), startup_session_group->state());
		}
	}

	// Register the StartupSessionsGroup as a timer handler to check start-up
	// DSD connection status and, if needed, reconnect.
	sig_and_timers_reactor->schedule_timer(startup_session_group, ACS_DSD_STARTUP_CONNECTION_CHECK_TIMER_CONTEXT,
			ACE_Time_Value(11), ACE_Time_Value(11));
	sig_and_timers_reactor->schedule_timer(startup_session_group, ACS_DSD_IMM_COPY_POLLING_CONTEXT,
			ACE_Time_Value(181), ACE_Time_Value(181));
	sig_and_timers_reactor->schedule_timer(startup_session_group, ACS_DSD_STATE_UPDATE_POLLING_CONTEXT,
			ACE_Time_Value(191), ACE_Time_Value(191));

	// wait for DSD Server stop condition
	if(dsd_stop_mutex.acquire() == -1)	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Unable to acquire 'dsd_stop_mutex'! errno == %d"), errno);
	while(!dsd_is_stopping) {
		if(p_dsd_stop_condition->wait() == -1)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("An error occurred while waiting for 'dsd_stop_condition' to be signaled ! errno == %d"), errno);
	}
	if(dsd_stop_mutex.release() == -1) 	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Unable to release 'dsd_stop_mutex'! errno == %d"), errno);

	// stop NTF notifications handling
	notification_thread.stop_activity();

	// cancel the 2 timers used to check periodically the integrity of IMM connections and DSD Service Acceptors
	sig_and_timers_reactor->cancel_timer(& dsd_sig_handler);

	// stop activities and wait for termination (PAY ATTENTION : ACCEPTOR TASK must be stopped before CONNECTION HANDLING TASK !)
	immCpNodesUpdater.stop_activity(true);
	p_serviceAcceptorTask->stop_activity(true);
	p_serviceHandlerTask->stop_activity(true);
	p_SignalHandlerTask->stop_activity(true);

#ifdef ACS_DSD_TEST
	is_AP1 && p_testTask->stop_activity(true);
#endif

	// Remove all the startup group timers
	sig_and_timers_reactor->cancel_timer(startup_session_group);

	// remove signal dispositions
	sig_and_timers_reactor->remove_handler(SIGPIPE,(ACE_Sig_Action *)0);
	sig_and_timers_reactor->remove_handler(SIGINT,(ACE_Sig_Action *) 0);
	sig_and_timers_reactor->remove_handler(SIGTERM,(ACE_Sig_Action *) 0);

	// Stop to be notified about changes of CS HWC Table
	configuration_change_observer.stop();

	// Close all opened startup sessions
	startup_session_group->close(1);

	// Delete all the objects used to keep information about the startup-sessions with remote DSD servers opened
	remote_AP_nodes_manager.remove_all_ap_nodes_connections();

	// finalize IMM components
	if(finalize_IMM(dsd_local_node, cflag)< 0)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("finalize_IMM() failed !"));

	// reset flags used to handle pending operations on IMM connections
	reset_IMM_op_pending_flags();

	// eventually delete lock file
	if(nohaflag && (unlink(DSD_SERVER_LOCKFILE_PATH) < 0))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Error deleting DSD Server lock file < %s >. Error description: < %s >"), DSD_SERVER_LOCKFILE_PATH, strerror(errno));

	// release objects dynamically allocated
	cleanup();

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("DSD Server exiting"));

	if(!nohaflag)
		haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread

	ACS_DSD_LOGGER_CLASS::close();

	return (ACE_THR_FUNC_RETURN) exit_code;
}


void stop_dsd_daemon_work()
{
	if(dsd_stop_mutex.acquire() == -1)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Unable to acquire 'dsd_stop_mutex'! errno == %d "), errno);

	dsd_is_stopping = true;

	if(dsd_stop_mutex.release() == -1)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("Unable to release 'dsd_stop_mutex'! errno == %d"), errno);

	if(p_dsd_stop_condition)
		p_dsd_stop_condition->signal();
}


void cleanup()
{
	// cleanup. The order is important !

	// Remember that each ACE_DSD_Task_Base object  has an associated reactor.
	// the ACE_Reactor destructor remove all the "Event Handlers" that are still registered with reactor, invoking "handle_close()" on them.
	// Since handle_close() method of an Event handler notifies the associated TASK about the "Start of Work" and "End of Work events", it's important that
	// the TASK is still active when the handle_close() is invoked. We ensure that this condition is true destroying reactor before destroying the associated TASK.
	delete sh_reactor;
	sh_reactor = 0;

	if ( startup_session_group )
	{
		startup_session_group->reactor(0);
		startup_session_group->~ACS_DSD_StartupSessionsGroup();
		startup_session_group = 0;
	}

	delete p_serviceHandlerTask;
	p_serviceHandlerTask = 0;
	delete sa_reactor;
	sa_reactor = 0;
	delete p_serviceAcceptorTask;
	p_serviceAcceptorTask = 0;
	delete sig_and_timers_reactor;
	sig_and_timers_reactor = 0;
	delete p_SignalHandlerTask;
	p_SignalHandlerTask = 0;
	delete p_dsd_stop_condition;
	p_dsd_stop_condition = 0;

#ifdef ACS_DSD_TEST
	if (is_AP1) delete p_testTask;
#endif

	ACE_Guard<ACE_Recursive_Thread_Mutex> imm_conn_manteinance_guard(imm_manteinance_op_mutex);
	if(immConnHandlerObj_ptr)
	{
		immConnHandlerObj_ptr->~ACS_DSD_ImmConnectionHandler();
		immConnHandlerObj_ptr = 0;
	}

	if(imm_shared_objects_conn_ptr)
	{
		imm_shared_objects_conn_ptr->~ACS_DSD_ImmConnectionHandler();
		imm_shared_objects_conn_ptr = 0;
	}
}


int parse_command_line(int argc, char **argv)
 {
   int index;
   int c;
   int noha_opt_cnt = 0;

   static struct option long_options[] =
   {
     /* These options set a flag. */
	 {"noha", no_argument, & nohaflag, 1},
	 {0, 0, 0, 0}
   };

   /* getopt_long stores the option index here. */
   int option_index = 0;

   while ((c = getopt_long(argc, argv, "c", long_options, & option_index)) != -1)
	 switch (c)
	 {
	   case 0:
		   // we've found a match with a long option, but we mustn't accept abbreviations (e.g. '--noh' instead of '--noha'). So check for this
		   if(strcmp(long_options[option_index].name, argv[optind-1] + 2))
		   {
			   fprintf(stderr,"acs_dsdd: Unrecognized option '%s'\n",argv[optind-1]);
			   return -1;
		   }

		   /* found --noha option */
		   if(noha_opt_cnt > 0)
		   {
			   fprintf(stderr,"acs_dsdd: duplicated long option 'noha'\n");
			   return -1;
		   }
		   ++noha_opt_cnt;
		   break;
	   case 'c':
		   if(cflag)
		   {
			   fprintf(stderr,"acs_dsdd: duplicated option 'c'\n");
			   return -1;
		   }
		   cflag = 1;
		 break;
	   case '?':
		   return -1;
	   default:
		 abort ();
	 }

   /*
    * We must reject the command line  "acs_dsdd --".
    * Note that if "--" command line argument is found, getopt_long() increments <optind> variable and returns -1.
   */
   if((optind > 1) && !strcmp(argv[optind-1],"--"))
   {
	   fprintf(stderr,"acs_dsdd: Unrecognized option '%s'\n",argv[optind-1]);
	   return -1;
   }

   if(optind < argc)
   {
	   for (index = optind; index < argc; index++)
		   printf ("Incorrect usage: found non-option argument '%s'\n", argv[index]);
	   return -1;
   }

   return 0;
}


int check_for_multiple_instances_running()
{
	int fdlock = open(DSD_SERVER_LOCKFILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0664);
	if(fdlock < 0)
	{
		fprintf(stderr, "%s: Error opening DSD SERVER LOCK FILE ! errno == '%d'\n", __PRETTY_FUNCTION__, errno);
		return -2;
	}

	if(flock(fdlock, LOCK_EX | LOCK_NB) < 0)
	{
		if(errno == EWOULDBLOCK)
		{
			fprintf(stderr, "another DSD Server instance running\n");
			return 1;
		}

		fprintf(stderr, "%s: Call 'flock()' failed ! errno == '%d'\n", __PRETTY_FUNCTION__, errno);
		return -3;
	}

	return 0;
}
bool isSWM20()
{
    if((boost::filesystem::exists(boost::filesystem::path(SWMVERSION)))== true)
    {
        return true;
    }
    else {
        return false;
    }
}

