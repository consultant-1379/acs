#include "ACS_DSD_Macros.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_Imm_functions.h"
#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_HA_AppManager.h"
#include "ACS_DSD_SignalHandler.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"
#include "ACS_DSD_ServiceAcceptorTask.h"


extern ACS_DSD_HA_AppManager *haObj;								// defined in ACS_DSD_Main.cpp
extern int nohaflag;												// defined in ACS_DSD_Main.cpp
extern int gbl_dsdd_must_continue_work;								// defined in ACS_DSD_Main.cpp
extern int is_imm_shared_objects_controller;						// defined in ACS_DSD_Main.cpp
extern ACS_DSD_Logger	dsd_logger;									// defined in ACS_DSD_Main.cpp
extern ACS_DSD_ServiceAcceptorTask * p_serviceAcceptorTask; 		// defined in ACS_DSD_Main.cpp
extern bool dsd_is_stopping;										// defined in ACS_DSD_Main.cpp
extern void stop_dsd_daemon_work();									// defined in ACS_DSD_Main.cpp

int ACS_DSD_SignalHandler::handle_signal (int signum, siginfo_t *,ucontext_t *)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_SignalHandler : received signal < %u > !"), signum);
	switch(signum)
	{
	case SIGPIPE:
		break;
	case SIGINT:
	case SIGTERM:
		if(nohaflag)
			stop_dsd_daemon_work();
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_SignalHandler : signal will be ignored ! You must use 'immadm' commands to terminate a DSD Daemon launched without '--noha' option !"));
			//kill(ACE_OS::getpid(),SIGUSR2);
		}
		break;
	};
	return 0;
}


int ACS_DSD_SignalHandler::handle_timeout ( const ACE_Time_Value &  /*current_time*/,  const void * act)
{
	const char * action_selector = reinterpret_cast<const char *>(act);

	if(dsd_is_stopping) return 0;  // do nothing if we are in stopping phase

	if(! strcmp(action_selector, IMM_CONN_CHECK_TIMER_NAME))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_SignalHandler : checking IMM connections .... "));

		// test IMM connections
		int call_result = check_and_restore_IMM_connections();
		if(call_result >= 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_SignalHandler : IMM connections are UP ! CHECK_RESULT == < %d >"), call_result);
		else if(call_result == -4)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_SignalHandler : temporary failure while testing IMM connections ! CHECK_RESULT == < %d >"), call_result);
		else /* (call_result < 0) && (call_result != -4)*/
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_SignalHandler : an error occurred while testing and restoring IMM connections. CHECK_RESULT == < %d > !"), call_result);
	}

	if(! strcmp(action_selector, DSD_SERVICE_ACCEPTORS_CHECK_TIMER_NAME))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_SignalHandler : checking DSD Service Acceptors (SVRAPP, CPAPP, SRVSRV) and ACCEPTOR task .... "));

		if(p_serviceAcceptorTask)
		{
			// get acceptors state, and acceptor task state (ACTIVE or NOT ACTIVE)
			ACS_DSD_ServiceAcceptorTask::Service_Acceptors_State acceptors_state = p_serviceAcceptorTask->get_acceptors_state();
			bool is_acceptor_task_active = p_serviceAcceptorTask->is_active();

			if(is_acceptor_task_active && (acceptors_state == ACS_DSD_ServiceAcceptorTask::ALL_ACCEPTORS_UP))
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_SignalHandler : DSD Service Acceptors are UP and ACCEPTOR Task is active "));
				return 0;
			}

			if(!is_acceptor_task_active)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_SignalHandler : Service Acceptor Task is not active ! "));

			if(acceptors_state != ACS_DSD_ServiceAcceptorTask::ALL_ACCEPTORS_UP)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_SignalHandler : one or more DSD Service Acceptors are DOWN (MASK == %0X). Trying to bring them up...  "), acceptors_state);

			// Try to open the DSD Service Acceptors that are DOWN
			int call_result = p_serviceAcceptorTask->check_and_restore_acceptors();
			if(call_result < 0)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_SignalHandler : ... attempt failed ! Call 'check_and_restore_acceptors()' returned %d !"), call_result);
			else if(call_result == 0)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_SignalHandler : ... attempt successful ! All DSD Service Acceptors are now UP."));
			else /*call_result == 1*/
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_SignalHandler : ... no action performed (check not applicable in the current task state: '%d' !"), p_serviceAcceptorTask->state());
		}
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_SignalHandler : ... unable to perform check. (p_serviceAcceptorTask == NULL) ! "));
	}

	return 0;
}
