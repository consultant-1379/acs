#include "ACS_DSD_ProtocolsDefs.h"
#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_ServiceAcceptorTask.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;


ACS_DSD_ServiceAcceptorTask::ACS_DSD_ServiceAcceptorTask(uint n_threads, ACE_Reactor * reactor)
: ACS_DSD_Task_Base(SERVICE_ACCEPTOR_TASK_NAME,n_threads,reactor),
 _svrapp_acceptor(reactor,0,"SVR-APP"),
 _cpap_acceptor(reactor,0,"CP-AP"),
 _srvsrv_acceptor(reactor,0,"SRV-SRV")
{
}


ACS_DSD_ServiceAcceptorTask::~ACS_DSD_ServiceAcceptorTask()
{
}

/*
int ACS_DSD_ServiceAcceptorTask::svc (void)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptorTask thread starting up"));
	if(!_reactor)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptorTask: reactor is NULL. thread ended"));
		return -1;	// the task is inconsistent; terminate thread execution returning -1
	}

	// execute reactor event loop
	int retVal = _reactor->run_reactor_event_loop();
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptorTask: thread ended"));
	return retVal;
}
*/

int ACS_DSD_ServiceAcceptorTask::init_AppProt_Acceptor(const char *sap_id)
{
	if(!_reactor)
		return -1;

	// register the SVRAPP Acceptor with the reactor, and start listening for connections
	return (_svrapp_acceptor.open(sap_id) == 0) ? 0: -1;
}

int ACS_DSD_ServiceAcceptorTask::init_CpProt_Acceptor(const char *sap_id)
{
	if(!_reactor)
		return -1;

	// register the CPAP Acceptor with the reactor, and start listening for connections
	return (_cpap_acceptor.open(sap_id) == 0) ? 0: -1;
}


int ACS_DSD_ServiceAcceptorTask::init_SrvSrvProt_Acceptor(const char *sap_id)
{
	if(!_reactor)
		return -1;

	// register the SRV-SRV Acceptor with the reactor, and start listening for connections
	return (_srvsrv_acceptor.open(sap_id) == 0) ? 0: -1;
}


int ACS_DSD_ServiceAcceptorTask::start_activity()
{
	int retval = 0;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	if(_state == UP)
		return 0;  // nothing to do

	if(_state == DOWN_NO_MEMORY)
		return -1;

	if(_state == UP_PARTIALLY)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Trying to complete the start of the TASK < %s > ..."), _task_name);
	else /* _state == DOWN */
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Starting TASK < %s > ..."), _task_name);

	// if not already initialized, initialize SVRAPP Acceptor
	if(_svrapp_acceptor.get_state() != ACS_DSD_ServiceAcceptor<ACS_DSD_AppProt_SvcHandler, ACS_DSD_UNIX_SAP>::SAC_STATE_OPEN)
	{
		if(init_AppProt_Acceptor(ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname())< 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > : initialization failed for SERVICE-APPLICATION Acceptor"), _task_name);
			retval = -2;
		}
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > : SVR-APP Acceptor successfully initialized (registered with Reactor and listening for connections)"), _task_name);
	}

	// if not already initialized, initialize CP-AP Acceptor
	if(_cpap_acceptor.get_state() != ACS_DSD_ServiceAcceptor<ACS_DSD_CpProt_SvcHandler, ACS_DSD_INET_SAP>::SAC_STATE_OPEN)
	{
		if(init_CpProt_Acceptor(ACS_DSD_ConfigurationHelper::cpap_inet_socket_sap_id())< 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > : initialization failed for CP-AP Acceptor"), _task_name);
			retval = -2;
		}
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > : CP-AP Acceptor successfully initialized (registered with Reactor and listening for connections)"), _task_name);
	}

	// if not already initialized, initialize SVR-SVR Acceptor
	if(_srvsrv_acceptor.get_state() != ACS_DSD_ServiceAcceptor<ACS_DSD_SrvProt_SvcHandler, ACS_DSD_INET_SAP>::SAC_STATE_OPEN)
	{
		if(init_SrvSrvProt_Acceptor(ACS_DSD_ConfigurationHelper::dsddsd_inet_socket_sap_id())< 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > : initialization failed for SVR-SVR Acceptor"), _task_name);
			retval = -2;
		}
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > : SVR-SVR Acceptor successfully initialized (registered with Reactor and listening for connections)"), _task_name);
	}

	// if not already done, launch threads
	if(!is_active())
	{
		int call_result = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, _n_threads);
		if(call_result < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > NOT STARTED ! ACE_Task_Base::activate() returned < %d >"),_task_name, call_result);
			retval = -3;
		}
		else if(call_result == 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > successfully STARTED !"), _task_name);
		else /* call_result == 1*/
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Task < %s > was already STARTED ('ACE_TASK_Base::activate()' returned '%d') !"), _task_name, retval);
	}

	_state = ((retval == 0) ? UP : UP_PARTIALLY);

	return retval;
}


int ACS_DSD_ServiceAcceptorTask::stop_activity(bool wait_termination)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Stopping TASK < %s > ..."), _task_name);

	if(_state == DOWN)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("TASK < %s > is yet STOPPED !"),_task_name);
		return 0;
	}

	if(_state == STOPPING)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("TASK < %s > is yet STOPPING !"),_task_name);
		return 0;
	}

	// terminate reactor event loop for all threads in the task
	int call_result = _reactor->end_reactor_event_loop();
	if(call_result < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > NOT STOPPED ! ACE_Reactor::end_reactor_event_loop() returned < %d >"),_task_name, call_result);
		return -1;
	}

	// update state
	_state = STOPPING;

	// release Task Mutex
	_mutex.release();

	// shutdown sockets ( this is a brute force operation in order to reject immediately incoming connections avoiding queuing )
	_svrapp_acceptor.shutdown_sockets();
	_cpap_acceptor.shutdown_sockets();
	_srvsrv_acceptor.shutdown_sockets();

	int ret_val = 0;

	if(wait_termination)
	{
		// wait for threads termination
		call_result = this->wait();
		if(call_result == -1)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > : Call 'ACE_Task_Base::wait()' returned < %d > ! errno == %d"),_task_name, call_result, errno);
			ret_val = -1;
		}
	}

	// stop listening for connections on the three SAPs
	call_result =_svrapp_acceptor.stop_listening();
	if(call_result != 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > :  Unable to stop listening for SVR-APP Acceptor !"),_task_name);
		ret_val = -1;
	}

	call_result = _cpap_acceptor.stop_listening();
	if(call_result != 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > :  Unable to stop listening for CP-AP Acceptor !"),_task_name);
		ret_val = -1;
	}

	call_result = _srvsrv_acceptor.stop_listening();
	if(call_result != 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > :  Unable to stop listening for SVR-SVR Acceptor !"),_task_name);
		ret_val = -1;
	}

	// update state
	_state = DOWN;

	if(ret_val == 0)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > successfully  STOPPED !"), _task_name);
	else
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Task < %s > STOPPED with some errors !"), _task_name);

	return ret_val;
}


ACS_DSD_ServiceAcceptorTask::Service_Acceptors_State  ACS_DSD_ServiceAcceptorTask::get_acceptors_state()
{
	unsigned int state_mask = ALL_ACCEPTORS_DOWN;

	if(_svrapp_acceptor.get_state() == ACS_DSD_ServiceAcceptor<ACS_DSD_AppProt_SvcHandler, ACS_DSD_UNIX_SAP>::SAC_STATE_OPEN)
		state_mask |= SVRAPP_ACCEPTOR_UP;

	if(_cpap_acceptor.get_state() == ACS_DSD_ServiceAcceptor<ACS_DSD_CpProt_SvcHandler,  ACS_DSD_INET_SAP>::SAC_STATE_OPEN)
		state_mask |= CPAP_ACCEPTOR_UP;

	if(_srvsrv_acceptor.get_state() == ACS_DSD_ServiceAcceptor<ACS_DSD_SrvProt_SvcHandler, ACS_DSD_INET_SAP>::SAC_STATE_OPEN)
		state_mask |= SVRSVR_ACCEPTOR_UP;

	return static_cast<ACS_DSD_ServiceAcceptorTask::Service_Acceptors_State>(state_mask);
}


int ACS_DSD_ServiceAcceptorTask::check_and_restore_acceptors()
{
	int retval = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Entering 'ACS_DSD_ServiceAcceptorTask::check_and_restore_acceptors()'"));

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	// no check must be performed if the task is DOWN or STOPPING
	if((_state == UP) || (_state == UP_PARTIALLY) )
	{
		// if not already initialized, initialize SVRAPP Acceptor
		if(_svrapp_acceptor.get_state() != ACS_DSD_ServiceAcceptor<ACS_DSD_AppProt_SvcHandler, ACS_DSD_UNIX_SAP>::SAC_STATE_OPEN)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("SVR-APP Acceptor is not open !"));

			if(init_AppProt_Acceptor(ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname())< 0)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > : initialization failed for SVR-APP Acceptor"), _task_name);
				retval = -1;
			}
			else
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > : SVR-APP Acceptor successfully initialized (registered with Reactor and listening for connections)"), _task_name);
		}

		// if not already initialized, initialize CP-AP Acceptor
		if(_cpap_acceptor.get_state() != ACS_DSD_ServiceAcceptor<ACS_DSD_CpProt_SvcHandler, ACS_DSD_INET_SAP>::SAC_STATE_OPEN)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("CP-APP Acceptor is not open !"));

			if(init_CpProt_Acceptor(ACS_DSD_ConfigurationHelper::cpap_inet_socket_sap_id())< 0)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > : initialization failed for CP-AP Acceptor"), _task_name);
				retval = -1;
			}
			else
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > : CP-AP Acceptor successfully initialized (registered with Reactor and listening for connections)"), _task_name);
		}

		// if not already initialized, initialize SVR-SVR Acceptor
		if(_srvsrv_acceptor.get_state() != ACS_DSD_ServiceAcceptor<ACS_DSD_SrvProt_SvcHandler, ACS_DSD_INET_SAP>::SAC_STATE_OPEN)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("SVR-SVR Acceptor is not open !"));

			if(init_SrvSrvProt_Acceptor(ACS_DSD_ConfigurationHelper::dsddsd_inet_socket_sap_id())< 0)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > : initialization failed for SVR-SVR Acceptor"), _task_name);
				retval = -1;
			}
			else
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > : SVR-SVR Acceptor successfully initialized (registered with Reactor and listening for connections)"), _task_name);
		}

		if(retval != 0)
			_state = UP_PARTIALLY;	// some acceptor is not UP
	}
	else
		retval = 1; // a positive return value to indicate that no action has been performed

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Leaving 'ACS_DSD_ServiceAcceptorTask::check_and_restore_acceptors()'"));

	return retval;
}
