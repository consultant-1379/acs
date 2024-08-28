#ifndef ACS_DSD_CONFIGPARAMS_H_
#define ACS_DSD_CONFIGPARAMS_H_

#define DSD_SERVER_CONNECT_TIMEOUT_VALUE 5000							// timeout value in milliseconds
#define DSD_SERVER_SEND_TIMEOUT_VALUE  3000           					// timeout value in milliseconds
#define DSD_SERVER_RECV_TIMEOUT_VALUE  5000           					// timeout value in milliseconds
#define DSD_SERVER_SEND_NOTIFY_INDICATION_TIMEOUT_VALUE  2000			// timeout value in milliseconds, used when notification indications (about CP state changes) are sent to subscribed clients
#define DSD_PROCESS_STATUSFILE_PATH_PREFIX  "/proc/"
#define DSD_PROCESS_STATUSFILE_NAME "status"
#define TASK_NAME_SIZE_MAX 128

#define N_SERVICE_ACCEPTOR_THREADS  8
#define N_SERVICE_HANDLER_THREADS  16
#define DSD_SERVER_IMM_PREFIX  "DSD_SERVER"
#define DSD_SERVER_LOGFILE_PATH_PREFIX  "/var/log/"
#define DSD_SERVER_LOGFILE_NAME  "acs_dsdd.log"
#define DSD_SERVER_LOCKFILE_PATH  "/var/run/ap/acs_dsdd.lck"
#define DSD_UNIX_SAP_NAME_PREFIX   "acs_dsdd_"

#define MAX_NUMBER_CP 68                              // 64 blade + (2 spx * 2 (ex/sb))
#define MAX_IMM_CP_NODE_UPDATE_INTERVAL  600          // seconds
#define MIN_IMM_CP_NODE_UPDATE_INTERVAL  2            // seconds
#define TASK_DEFAULT_MIN_NUM_THREADS_IDLE  2
#define TASK_DEFAULT_MAX_NUM_THREADS_IDLE  16
#define TASK_DEFAULT_MAX_NUM_THREADS 128
#define TASK_ACCEPTOR_MIN_NUM_THREADS_IDLE 4
#define TASK_ACCEPTOR_MAX_NUM_THREADS_IDLE 16
#define TASK_ACCEPTOR_MAX_NUM_THREADS 32
#define TASK_SERVICE_HANDLER_MIN_NUM_THREADS_IDLE 4
#define TASK_SERVICE_HANDLER_MAX_NUM_THREADS_IDLE 32
#define TASK_SERVICE_HANDLER_MAX_NUM_THREADS 128

#define SERVICE_HANDLER_TASK_NAME   "CONN_HDLRS_TP"
#define SERVICE_ACCEPTOR_TASK_NAME  "ACCEPTORS_TP"
#define SIG_AND_TIMERS_TASK_NAME    "SIG_AND_TIMERS_HDLR"

#define IMM_CONN_CHECK_TIMER_NAME    "imm_conn_check_timer"
#define DSD_SERVICE_ACCEPTORS_CHECK_TIMER_NAME	"dsd_service_acceptors_check_timer"

#define N_RETRY_IMM_REGISTER                     5      /// maximum number of retries trying to register with IMM
#define N_RETRY_IMM_FINALIZE                     5      /// maximum number of retries trying to finalize an IMM connection
#define N_SECS_IMM_REGISTER_INTERVAL             1      /// interval between retries trying to register with IMM
#define N_SECS_IMM_FINALIZE_INTERVAL             1      /// interval between retries trying to finalize an IMM connection

#define N_SECS_IMM_CONN_CHECK_INTERVAL          60      	/// interval between two consecutive checks for connections towards IMM
#define N_SECS_DSD_SERVICE_ACCEPTORS_CHECK_INTERVAL   60	/// interval between two consecutive checks for DSD Service Acceptors ( SVRAPP, CPAP and SRVSRV protocols)

#define MAX_GET_NODE_INFO_ATTEMPS               60      // maximum number of attempts used to get local AP node info on DSD Server startup
#define GET_NODE_INFO_DELAY_INTERVAL             2      // interval between two consecutive attempts (in seconds)
#define ACS_DSD_TASK_BASE_EV_LOOP_TIMEOUT        2      // event loop timeout (in seconds) used in ACS_DSD_Task_Base::svc() method
#define IMM_CP_NODES_UPDATER_INTERVAL           30      // check interval (in seconds) used by ACS_DSD_ImmCpNodesUpdater::svc() method

#define IMM_MAX_TEMP_FAILURES					 3		// maximum number of temporary failures (TRY AGAIN) that are tolerated when testing IMM connections

#define ACS_DSD_LOG_APPENDER_NAME  "ACS_DSDD"
#define ACS_DSD_DAEMON_NAME        "acs_dsdd"
#define ACS_DSD_HA_USER            "root"

#define AP_NODE_HOSTNAME_FILEPATH "/etc/cluster/nodes/this/hostname"
#define AP_NODE_ID_FILEPATH "/etc/cluster/nodes/this/id"
#define AP_OTHER_NODE_ID_FILEPATH  "/etc/cluster/nodes/peer/id"
//#define AP_NODE_SU_TEMPLATE	"safSu=SC-%s,safSg=2N,safApp=OpenSAF"
//#define AP_NODE_SU_TEMPLATE	"safSu=APG_SU%s_2N,safSg=APG_2NSG,safApp=APG"
/* EXIT CODES */
#define DSD_SERVER_EXIT_CODE_OK 0
#define DSD_SERVER_EXIT_CODE_INTERNAL_ERROR 1
#define DSD_SERVER_EXIT_CODE_WRONG_USAGE 2
#define DSD_SERVER_EXIT_CODE_CHECK_RUNNING_NOT_OK 3
#define DSD_SERVER_EXIT_CODE_INTERRUPTED 4
#define DSD_SERVER_EXIT_CODE_IMM_FATAL_ERROR 5

#define DSD_SERVER_COMP_RESTART_DELAY_FOR_IMM_FATAL_ERRORS 5


#endif /* ACS_DSD_CONFIGPARAMS_H_ */
