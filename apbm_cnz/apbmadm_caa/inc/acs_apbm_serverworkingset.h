#ifndef HEADER_GUARD_CLASS__acs_apbm_serverworkingset
#define HEADER_GUARD_CLASS__acs_apbm_serverworkingset acs_apbm_serverworkingset

/** @file acs_apbm_serverworkingset.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-07-12
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2011-08-09 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ace/TP_Reactor.h>
#include <ace/Reactor.h>
#include <ace/Signal.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Task.h>

#include "acs_apbm_macros.h"
#include <acs_apbm_programmacros.h>
#include <acs_apbm_programconstants.h>
//#include "acs_apbm_snmpsessionhandler.h"
#include "acs_apbm_boardserviceacceptor.h"
//#include "acs_apbm_snmptrapeventhandler.h"


/*
 * Forward declarations
 */
class acs_apbm_cmdoptionparser;
class acs_apbm_operationpipeeventhandler;
class acs_apbm_procsignalseventhandler;
class acs_apbm_snmpmanager;
class acs_apbm_csreader;
class acs_apbm_trapsubscriptionmanager;
class acs_apbm_shelvesdatamanager;
class acs_apbm_immrepositoryhandler;
class acs_apbm_sanotifiereventhandler;
class ACS_APBM_HWIHandler;
class acs_apbm_monitoringservicehandler;
class acs_apbm_operationpipescheduler;
class acs_apbm_snmptrapdsobserver;
class acs_apbm_cshwctablechangeobserver;
class acs_apbm_alarmeventhandler;
class acs_apbm_haappmanager;
class acs_apbm_scxlagimmhandler;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_serverworkingset


/** @class acs_apbm_serverworkingset acs_apbm_serverworkingset.h
 *	@brief acs_apbm_serverworkingset class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-08-09
 *
 *	acs_apbm_serverworkingset <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_serverworkingset constructor
	 */
	inline __CLASS_NAME__ ()
	: operation_pipe(), program_state(acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED), //program_state(acs_apbm::PROGRAM_STATE_RUNNING),
	  cmdoption_parser(0), ha_app_manager(0),
	  operation_pipe_event_handler(0), main_reactor(0), proc_signals_event_handler(0),
	  imm_repository_handler(0), /*snmp_traps_acceptor(0),*/ board_service_acceptor_api_unix(0),
	  board_service_acceptor_api_inet(0), /*snmp_trap_message_handler(0),*/ snmp_manager(0), cs_reader(0),
	  trap_subscription_manager(0), shelves_data_manager(0), sa_notifier_event_handler(0), hwi_handler(0),
	  monitoring_service_event_handler(0), operation_pipe_scheduler(0), snmp_trapds_observer(0),
	  cs_hwc_table_change_observer(0), alarmevent_handler (0),server_work_task(0),imm_lag_handler(0),
	  _proc_catched_signals(), _initialization_sync(ACS_APBM_STRINGIZE(__CLASS_NAME__)"::"ACS_APBM_STRINGIZE(_initialization_sync)),
	  _pipe_write_sync() {
		operation_pipe[0] = operation_pipe[1] = ACE_INVALID_HANDLE;
	}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_serverworkingset Destructor
	 */
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	int init_operation_pipe_and_handler ();
	int reset_operation_pipe_and_handler ();

	int start_operation_pipe_scheduler ();
	int stop_operation_pipe_scheduler ();

	int init_signals_handler ();
	int reset_signals_handler ();

	int open_api_service_acceptor_unix ();
	int close_api_service_acceptor_unix ();

	int open_api_service_acceptor_inet ();
	int close_api_service_acceptor_inet ();

	int init_sa_notifier_manager_and_handler ();
	int reset_sa_notifier_manager_and_handler ();

	int get_node_state (acs_apbm::NodeStateConstants & node_state, unsigned retries = 0);
	inline const char * node_state_str (acs_apbm::NodeStateConstants node_state) const {
		return (acs_apbm::NODE_STATE_UNDEFINED <= node_state) && (node_state <= acs_apbm::NODE_STATE_PASSIVE)
				? _node_state_names[node_state]
				: "<NOT KNOWN>";
	}

	int operation_send (acs_apbm::ServerWorkOperationCodeConstants op);
//	int operation_send (acs_apbm::ServerWorkOperationCodeConstants op, const uint32_t & param);
	int operation_send (acs_apbm::ServerWorkOperationCodeConstants op, uint32_t param);
	int operation_send (acs_apbm::ServerWorkOperationCodeConstants op, int32_t par_1, uint32_t par_2);

	void set_object_pointers (
		acs_apbm_cmdoptionparser *,
		acs_apbm_haappmanager *,
		acs_apbm_operationpipeeventhandler *,
		ACE_Reactor *,
		acs_apbm_procsignalseventhandler *,
		acs_apbm_immrepositoryhandler *,
		/*acs_apbm_snmpsessionhandler *,*/
		acs_apbm_boardserviceacceptor *,
		acs_apbm_boardserviceacceptor *,
		/*acs_apbm_snmptrapeventhandler *,*/
		acs_apbm_snmpmanager *,
		acs_apbm_csreader *,
		acs_apbm_trapsubscriptionmanager *,
		acs_apbm_shelvesdatamanager *,
		acs_apbm_sanotifiereventhandler *,
		ACS_APBM_HWIHandler *,
		acs_apbm_monitoringservicehandler *,
		acs_apbm_operationpipescheduler *,
		acs_apbm_snmptrapdsobserver *,
		acs_apbm_cshwctablechangeobserver *,
		acs_apbm_alarmeventhandler *,
		ACE_Task_Base *,
		acs_apbm_scxlagimmhandler *
	);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
public:
	ACE_HANDLE operation_pipe[2];
	acs_apbm::ProgramStateConstants program_state;

	acs_apbm_cmdoptionparser * cmdoption_parser;
	acs_apbm_haappmanager * ha_app_manager;
	acs_apbm_operationpipeeventhandler * operation_pipe_event_handler;
	ACE_Reactor * main_reactor;
	acs_apbm_procsignalseventhandler * proc_signals_event_handler;
	acs_apbm_immrepositoryhandler * imm_repository_handler;
//	acs_apbm_snmpsessionhandler * snmp_traps_acceptor;
	acs_apbm_boardserviceacceptor * board_service_acceptor_api_unix;
	acs_apbm_boardserviceacceptor * board_service_acceptor_api_inet;
//	acs_apbm_snmptrapeventhandler * snmp_trap_message_handler;
	acs_apbm_snmpmanager * snmp_manager;
	acs_apbm_csreader * cs_reader;
	acs_apbm_trapsubscriptionmanager * trap_subscription_manager;
	acs_apbm_shelvesdatamanager * shelves_data_manager;
	acs_apbm_sanotifiereventhandler * sa_notifier_event_handler;
	ACS_APBM_HWIHandler * hwi_handler;
	acs_apbm_monitoringservicehandler * monitoring_service_event_handler;
	acs_apbm_operationpipescheduler * operation_pipe_scheduler;
	acs_apbm_snmptrapdsobserver * snmp_trapds_observer;
	acs_apbm_cshwctablechangeobserver * cs_hwc_table_change_observer;
	acs_apbm_alarmeventhandler *alarmevent_handler;
	ACE_Task_Base * server_work_task;
	acs_apbm_scxlagimmhandler * imm_lag_handler;

private:
	ACE_Sig_Set _proc_catched_signals;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _initialization_sync;
	ACE_Recursive_Thread_Mutex _pipe_write_sync;

	static const char * _node_state_names [];
	static int _proc_signals_to_catch [];
};

#endif // HEADER_GUARD_CLASS__acs_apbm_serverworkingset
