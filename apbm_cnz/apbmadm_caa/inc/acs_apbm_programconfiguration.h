#ifndef HEADER_GUARD_CLASS__acs_apbm_programconfiguration
#define HEADER_GUARD_CLASS__acs_apbm_programconfiguration acs_apbm_programconfiguration

/** @file acs_apbm_programconfiguration.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-07-18
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
 *	| R-001 | 2011-07-18 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <stdint.h>

#include <string>


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_programconfiguration


/** @class acs_apbm_programconfiguration acs_apbm_programconfiguration.h
 *	@brief acs_apbm_programconfiguration class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-07-18
 *
 *	acs_apbm_programconfiguration <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
private:
	/** @brief acs_apbm_programconfiguration Default constructor
	 */
	__CLASS_NAME__ ();
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_programconfiguration Destructor
	 */
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	static int load ();


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
public:
	static const char *	apbm_server_lock_file_path;
	static uint8_t			AP_InitProcess_call_enabled;
	static const char *	logger_appender_name;
	static unsigned			get_node_state_delay;
	static unsigned			get_node_state_retries;
	static unsigned			open_imm_management_session_delay;
	static unsigned			open_imm_management_session_retries;
	static unsigned			close_imm_management_session_delay;
	static unsigned			close_imm_management_session_retries;
	static unsigned			take_imm_objects_ownership_delay;
	static unsigned			take_imm_objects_ownership_retries;
	static unsigned			release_imm_objects_ownership_delay;
	static unsigned			release_imm_objects_ownership_retries;
	static unsigned			send_start_work_operation_on_startup_delay;
	static unsigned			main_thread_enter_reactor_loop_delay;
	static unsigned			task_thread_enter_reactor_loop_delay;
	static unsigned			init_operation_pipe_and_handler_delay;
	static unsigned			init_sa_notifier_and_handler_delay;
	static const char *	imm_implementer_name_manager;
	static const char *	imm_implementer_name_runtime;
	static unsigned			sleep_before_sending_exit_program_delay;
	static const char *	netsnmp_initializer_name;
	static unsigned			api_service_acceptor_open_delay;
	static unsigned			snmp_trap_handling_open_delay;
	static unsigned			snmp_trap_handling_close_delay;
	static int16_t			slot_position_switch_board_left;
	static int16_t			slot_position_switch_board_right;
	static int					working_active_thread_count;
	static int					working_passive_thread_count;
	static const char *	cluster_node_id_filepath;
	static const char *	cluster_other_node_id_filepath;
	static const char *	ha_daemon_name;
	static char					magazine_address_imm_separator;
	static unsigned			ntf_dispatch_callbacks_delay;
	static const char *	ap_node_change_state_su_template;
	static const char *	ap_node_change_state_si_template;
	static unsigned			monitoring_service_initial_delay;
	static unsigned 		monitoring_service_interval;
	static int                      monitoring_service_on_passive_threshold;
	static unsigned			operation_pipe_scheduler_initial_delay_ms;
	static unsigned 		operation_pipe_scheduler_interval_ms;
	static unsigned 		operation_pipe_scheduler_default_ticks_to_wait;
	static unsigned 		operation_pipe_scheduler_bsp_default_ticks_to_wait;
	static std::string	trapds_subscription_service_name;
	static unsigned			scxb_coldstart_cnt_threshold;
	static int					front_port_check_trigger_threshold;
	static int					update_switchboard_states_trigger_threshold;
	static unsigned			switch_board_link_down_max_time;
	static const char *	imm_oi_class_name;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_programconfiguration
