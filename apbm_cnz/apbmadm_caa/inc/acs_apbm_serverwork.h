#ifndef HEADER_GUARD_CLASS__acs_apbm_serverwork
#define HEADER_GUARD_CLASS__acs_apbm_serverwork acs_apbm_serverwork

/** @file acs_apbm_serverwork.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-08-01
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
 *	| R-001 | 2011-08-01 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ace/TP_Reactor.h>
#include <ace/Reactor.h>
#include <ace/Task.h>

#include "acs_apbm_macros.h"
#include "acs_apbm_cmdoptionparser.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_operationpipeeventhandler.h"
#include "acs_apbm_procsignalseventhandler.h"
#include "acs_apbm_immrepositoryhandler.h"
#include "acs_apbm_boardserviceacceptor.h"
#include <acs_apbm_configurationhelper.h>
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_trapsubscriptionmanager.h"
#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_sanotifiereventhandler.h"
#include "acs_apbm_hwihandler.h"
#include "acs_apbm_monitoringservicehandler.h"
#include "acs_apbm_operationpipescheduler.h"
#include "acs_apbm_snmptrapdsobserver.h"
#include "acs_apbm_cshwctablechangeobserver.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_ironsidemanager.h"
#include "acs_apbm_scxlagimmhandler.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_serverwork


/** @class acs_apbm_serverwork acs_apbm_serverwork.h
 *	@brief acs_apbm_serverwork class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-08-01
 *
 *	acs_apbm_serverwork <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACE_Task_Base {
	//==============//
	// Constructors //
	//==============//
public:
	inline explicit __CLASS_NAME__ (acs_apbm_cmdoptionparser & cmdoption_parser)
	: ACE_Task_Base(), _cmdoption_parser(cmdoption_parser), _server_working_set() {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	int work ();

#if 0 // TODO: BEGIN: OLD CODE: To be deleted
//	int work_noha ();
//	int work_ha ();
#endif // TODO: END: OLD CODE: To be deleted

	// BEGIN: ACE_Task_Base interface //
	virtual int svc ();
	// END: ACE_Task_Base interface //

private:
	int multiple_process_instance_running_check ();

	void send_start_work_operation_by_node_state ();

	void enter_reactor_event_loop (ACE_Reactor & reactor);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//=============//
	// Sub Classes //
	//=============//
private:
	class acs_apbm_serverworkobjects {
	public:
		inline explicit acs_apbm_serverworkobjects (acs_apbm_serverworkingset * server_working_set)
		: operation_pipe_event_handler(server_working_set), main_reactor_impl(), main_reactor(&main_reactor_impl),
		  proc_signals_event_handler(server_working_set), imm_repository_handler(server_working_set), /*snmp_traps_acceptor(),*/
		  board_service_acceptor_api_unix(&main_reactor, &main_reactor, acs_apbm_boardserviceacceptor::UNIX_SAP, acs_apbm_configurationhelper::apbmapi_unix_socket_sap_pathname(), server_working_set),
		  board_service_acceptor_api_inet(&main_reactor, &main_reactor, acs_apbm_boardserviceacceptor::INET_SAP, acs_apbm_configurationhelper::apbmapi_inet_socket_sap(), server_working_set),
		  /*snmp_trap_message_handler(server_working_set),*/ snmp_manager(server_working_set), cs_reader(server_working_set), trap_subscription_manager(),
		  shelves_data_manager(server_working_set), sa_notifier_event_handler(), hwi_handler(server_working_set),
		  monitoring_service_event_handler(acs_apbm_programconfiguration::monitoring_service_initial_delay, acs_apbm_programconfiguration::monitoring_service_interval, server_working_set),
		  operation_pipe_scheduler(server_working_set), snmp_trapds_observer(server_working_set), cs_hwc_table_change_observer(server_working_set),alarmevent_handler(server_working_set),
		  ironside_manager(server_working_set),imm_lag_handler(server_working_set)
		  {}
	private:
		acs_apbm_serverworkobjects (const acs_apbm_serverworkobjects & rhs);

	public:
		~acs_apbm_serverworkobjects () { main_reactor.close(); }

	private:
		acs_apbm_serverworkobjects & operator= (const acs_apbm_serverworkobjects & rhs);

	public:
		acs_apbm_operationpipeeventhandler operation_pipe_event_handler;
		ACE_TP_Reactor main_reactor_impl;
		ACE_Reactor main_reactor;
		acs_apbm_procsignalseventhandler proc_signals_event_handler;
		acs_apbm_immrepositoryhandler imm_repository_handler;
//		acs_apbm_snmpsessionhandler snmp_traps_acceptor;
		acs_apbm_boardserviceacceptor board_service_acceptor_api_unix;
		acs_apbm_boardserviceacceptor board_service_acceptor_api_inet;
//		acs_apbm_snmptrapeventhandler snmp_trap_message_handler;
		acs_apbm_snmpmanager snmp_manager;
		acs_apbm_csreader cs_reader;
		acs_apbm_trapsubscriptionmanager trap_subscription_manager;
		acs_apbm_shelvesdatamanager shelves_data_manager;
		acs_apbm_sanotifiereventhandler sa_notifier_event_handler;
		ACS_APBM_HWIHandler hwi_handler;
		acs_apbm_monitoringservicehandler monitoring_service_event_handler;
		acs_apbm_operationpipescheduler operation_pipe_scheduler;
		acs_apbm_snmptrapdsobserver snmp_trapds_observer;
		acs_apbm_cshwctablechangeobserver cs_hwc_table_change_observer;
		acs_apbm_alarmeventhandler alarmevent_handler;
		acs_apbm_ironsidemanager ironside_manager;
		acs_apbm_scxlagimmhandler imm_lag_handler;
	};


	//========//
	// Fields //
	//========//
private:
	acs_apbm_cmdoptionparser & _cmdoption_parser;
	acs_apbm_serverworkingset _server_working_set;
};
#endif // HEADER_GUARD_CLASS__acs_apbm_serverwork
