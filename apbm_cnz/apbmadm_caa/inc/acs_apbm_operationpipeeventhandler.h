#ifndef HEADER_GUARD_CLASS__acs_apbm_operationpipeeventhandler
#define HEADER_GUARD_CLASS__acs_apbm_operationpipeeventhandler acs_apbm_operationpipeeventhandler

/** @file acs_apbm_operationpipeeventhandler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-08-02
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
 *	| R-001 | 2011-08-02 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ace/Event_Handler.h>

#include <ace/Thread_Mutex.h>

#include "acs_apbm_programconstants.h"


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_operationpipeeventhandler

/** @class acs_apbm_operationpipeeventhandler acs_apbm_operationpipeeventhandler.h
 *	@brief acs_apbm_operationpipeeventhandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-08-02
 *
 *	acs_apbm_operationpipeeventhandler <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACE_Event_Handler {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_operationpipeeventhandler constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set, ACE_HANDLE handle = ACE_INVALID_HANDLE)
	: ACE_Event_Handler(), _server_working_set(server_working_set), _handle(handle), _work_undefined_node_state_check_retry_count(0) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_operationpipeeventhandler Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:

	// BEGIN: ACE_Event_Handler interface //
	inline virtual ACE_HANDLE get_handle () const { return _handle; }

  inline virtual void set_handle (ACE_HANDLE handle) { _handle = handle; }

	virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

	virtual int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	// END: ACE_Event_Handler interface //

private:
	int handle_pipe_operation (ACE_HANDLE fd, int op);
	int op_nop ();
	int op_exit_program ();
	int op_init_signals_handler ();
	int op_start_work_node_active ();
	int op_start_work_node_passive ();
	int op_start_work_quiesced ();
	int op_start_work_node_undefined ();
	int op_stop_work_to_exit_program ();
	int op_stop_work_to_change_state_to (acs_apbm::ProgramStateConstants new_state, acs_apbm::ServerWorkOperationCodeConstants operation);
	int op_activate_server_work_task ();
	int op_imm_open_management_session ();
	int op_imm_take_ownership ();
	int op_imm_close_management_session ();
	int op_imm_release_ownership ();

	int op_api_service_acceptor_unix_open ();
	int op_api_service_acceptor_inet_open ();

	int op_api_service_acceptor_unix_close ();
	int op_api_service_acceptor_inet_close ();

	int op_cs_load_board_data (bool add_info_to_IMM);
	int op_snmp_trap_subscribe ();
	int op_snmp_trap_unsubscribe ();
	int op_snmp_trap_unsubscribe_left ();
	int op_snmp_trap_unsubscribe_right ();
	int op_snmp_trap_unsubscribe_subscribe ();
	int op_snmp_load_board_data (bool add_info_to_IMM);
	int op_snmp_find_switch_board_master ();
	int op_snmp_hwi_handler_open ();
	int op_snmp_hwi_handler_reload ();
	int op_snmp_hwi_handler_close ();
	int op_start_monitoring_service ();
	int op_stop_monitoring_service ();
	int op_snmp_trapds_subscribe ();
	int op_snmp_trapds_unsubscribe ();
	int op_cs_hwc_table_change_observer_start ();
	int op_cs_hwc_table_change_observer_stop ();
	int op_work_undefined_node_state_check ();
	int op_shelf_mngctrl_trap_counter_check ();

	int op_snmp_magazine_trap_subscribe (uint32_t magazine);
	int op_snmp_magazine_trap_unsubscribe (uint32_t magazine);
	int op_snmp_magazine_trap_unsubscribe_subscribe (uint32_t magazine);
	int op_snmp_other_magazines_trap_subscribe ();
	int op_snmp_other_magazines_trap_unsubscribe ();
	int op_snmp_other_magazines_trap_unsubscribe_subscribe ();
	int op_update_board_status (int32_t slot, uint32_t magazine);

	int stop_working ();

	int op_testing_node_active_1 ();

    int op_scx_lag_imm_handler_open();
    int op_scx_lag_imm_handler_close();

    int op_cease_all_alarms_shutdown();		 //TR HX65505
	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	acs_apbm_serverworkingset * _server_working_set;
	ACE_HANDLE _handle;
	uint16_t _work_undefined_node_state_check_retry_count;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_operationpipeeventhandler
