#include <string>

#include "acs_apbm_programconfiguration.h"

/*
 * Internal program default configuration
 */
#define ACS_APBM_CONFIG_AP_InitProcess_call_enabled 0
#define ACS_APBM_CONFIG_get_node_state_delay 1
#define ACS_APBM_CONFIG_get_node_state_retries 3
#define ACS_APBM_CONFIG_open_imm_management_session_delay 1
#define ACS_APBM_CONFIG_open_imm_management_session_retries 3
#define ACS_APBM_CONFIG_close_imm_management_session_delay 1
#define ACS_APBM_CONFIG_close_imm_management_session_retries 3
#define ACS_APBM_CONFIG_take_imm_objects_ownership_delay 1
#define ACS_APBM_CONFIG_take_imm_objects_ownership_retries 3
#define ACS_APBM_CONFIG_release_imm_objects_ownership_delay 1
#define ACS_APBM_CONFIG_release_imm_objects_ownership_retries 3
#define ACS_APBM_CONFIG_send_start_work_operation_on_startup_delay 1
#define ACS_APBM_CONFIG_main_thread_enter_reactor_loop_delay 1
#define ACS_APBM_CONFIG_task_thread_enter_reactor_loop_delay 1
#define ACS_APBM_CONFIG_init_operation_pipe_and_handler_delay 1
#define ACS_APBM_CONFIG_init_sa_notifier_and_handler_delay 1
#define ACS_APBM_CONFIG_sleep_before_sending_exit_program_delay 3
#define ACS_APBM_CONFIG_api_service_acceptor_open_delay 1
#define ACS_APBM_CONFIG_snmp_trap_handling_open_delay 1
#define ACS_APBM_CONFIG_snmp_trap_handling_close_delay 1
#define ACS_APBM_CONFIG_slot_position_switch_board_left 0
#define ACS_APBM_CONFIG_slot_position_switch_board_right 25
#define ACS_APBM_CONFIG_working_active_thread_count 12
#define ACS_APBM_CONFIG_working_passive_thread_count 12
#define ACS_APBM_CONFIG_magazine_address_imm_separator '.'
#define ACS_APBM_CONFIG_ntf_dispatch_callbacks_delay 1
#define ACS_APBM_CONFIG_monitoring_service_initial_delay 10
#define ACS_APBM_CONFIG_monitoring_service_interval	3
#define ACS_APBM_CONFIG_monitoring_service_on_passive_threshold     3
#define ACS_APBM_CONFIG_operation_pipe_scheduler_initial_delay_ms 3000
#define ACS_APBM_CONFIG_operation_pipe_scheduler_interval_ms 1000
#define ACS_APBM_CONFIG_operation_pipe_scheduler_default_ticks_to_wait 2
#define ACS_APBM_CONFIG_operation_pipe_scheduler_bsp_default_ticks_to_wait 5 
#define ACS_APBM_CONFIG_scxb_coldstart_cnt_threshold 2
#define ACS_APBM_CONFIG_front_port_check_trigger_threshold 19
#define ACS_APBM_CONFIG_update_switchboard_states_trigger_threshold 13
#define ACS_APBM_CONFIG_scb_link_down_max_time_before_raising_alarm 16
/*#define is_swm_2_0
#ifdef is_swm_2_0
#define AP_NODE_SU_TEMPLATE     "safSu=SC-%s,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service"
#define AP_NODE_SI_TEMPLATE "safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service"
#else
#define AP_NODE_SU_TEMPLATE     "safSu=%s,safSg=2N,safApp=ERIC-APG"
#define AP_NODE_SI_TEMPLATE "safSi=AGENT,safApp=ERIC-APG"
#endif
*/
namespace {
	std::string gs_apbm_server_lock_file_path("/var/run/ap/acs_apbm.lck");
	std::string gs_logger_appender_name("ACS_APBMD");
	std::string gs_imm_implementer_name_manager("ACS_APBMD_M");
	std::string gs_imm_implementer_name_runtime("ACS_APBMD_R");
	std::string gs_netsnmp_initializer_name("ACS_APBM_NMS");
	std::string gs_cluster_node_id_filepath("/etc/cluster/nodes/this/id");
	std::string gs_cluster_other_node_id_filepath("/etc/cluster/nodes/peer/id");
	std::string gs_ha_daemon_name("acs_apbmd");

/* FOR GIW-1.1:	std::string gs_ap_node_change_state_su_template("safSu=SC-%s,safSg=2N,safApp=OpenSAF"); */
/* FOR GIW-2.2: */	
//        std::string gs_ap_node_change_state_su_template(AP_NODE_SU_TEMPLATE);
//	std::string gs_ap_node_change_state_si_template(AP_NODE_SI_TEMPLATE);
	std::string gs_imm_oi_class_name("AxeEquipmentApBladeInfo");
}


const char *	__CLASS_NAME__::apbm_server_lock_file_path = gs_apbm_server_lock_file_path.c_str();
uint8_t				__CLASS_NAME__::AP_InitProcess_call_enabled = ACS_APBM_CONFIG_AP_InitProcess_call_enabled;
const char *	__CLASS_NAME__::logger_appender_name = gs_logger_appender_name.c_str();
unsigned			__CLASS_NAME__::get_node_state_delay = ACS_APBM_CONFIG_get_node_state_delay;
unsigned			__CLASS_NAME__::get_node_state_retries = ACS_APBM_CONFIG_get_node_state_retries;
unsigned			__CLASS_NAME__::open_imm_management_session_delay = ACS_APBM_CONFIG_open_imm_management_session_delay;
unsigned			__CLASS_NAME__::open_imm_management_session_retries = ACS_APBM_CONFIG_open_imm_management_session_retries;
unsigned			__CLASS_NAME__::close_imm_management_session_delay = ACS_APBM_CONFIG_close_imm_management_session_delay;
unsigned			__CLASS_NAME__::close_imm_management_session_retries = ACS_APBM_CONFIG_close_imm_management_session_retries;
unsigned			__CLASS_NAME__::take_imm_objects_ownership_delay = ACS_APBM_CONFIG_take_imm_objects_ownership_delay;
unsigned			__CLASS_NAME__::take_imm_objects_ownership_retries = ACS_APBM_CONFIG_take_imm_objects_ownership_retries;
unsigned			__CLASS_NAME__::release_imm_objects_ownership_delay = ACS_APBM_CONFIG_release_imm_objects_ownership_delay;
unsigned			__CLASS_NAME__::release_imm_objects_ownership_retries = ACS_APBM_CONFIG_release_imm_objects_ownership_retries;
unsigned			__CLASS_NAME__::send_start_work_operation_on_startup_delay = ACS_APBM_CONFIG_send_start_work_operation_on_startup_delay;
unsigned			__CLASS_NAME__::main_thread_enter_reactor_loop_delay = ACS_APBM_CONFIG_main_thread_enter_reactor_loop_delay;
unsigned			__CLASS_NAME__::task_thread_enter_reactor_loop_delay = ACS_APBM_CONFIG_task_thread_enter_reactor_loop_delay;
unsigned			__CLASS_NAME__::init_operation_pipe_and_handler_delay = ACS_APBM_CONFIG_init_operation_pipe_and_handler_delay;
unsigned			__CLASS_NAME__::init_sa_notifier_and_handler_delay = ACS_APBM_CONFIG_init_sa_notifier_and_handler_delay;
const char *	__CLASS_NAME__::imm_implementer_name_manager = gs_imm_implementer_name_manager.c_str();
const char *	__CLASS_NAME__::imm_implementer_name_runtime = gs_imm_implementer_name_runtime.c_str();
unsigned			__CLASS_NAME__::sleep_before_sending_exit_program_delay = ACS_APBM_CONFIG_sleep_before_sending_exit_program_delay;
const char *	__CLASS_NAME__::netsnmp_initializer_name = gs_netsnmp_initializer_name.c_str();
unsigned			__CLASS_NAME__::api_service_acceptor_open_delay = ACS_APBM_CONFIG_api_service_acceptor_open_delay;
unsigned			__CLASS_NAME__::snmp_trap_handling_open_delay = ACS_APBM_CONFIG_snmp_trap_handling_open_delay;
unsigned			__CLASS_NAME__::snmp_trap_handling_close_delay = ACS_APBM_CONFIG_snmp_trap_handling_close_delay;
int16_t				__CLASS_NAME__::slot_position_switch_board_left = ACS_APBM_CONFIG_slot_position_switch_board_left;
int16_t				__CLASS_NAME__::slot_position_switch_board_right = ACS_APBM_CONFIG_slot_position_switch_board_right;
int						__CLASS_NAME__::working_active_thread_count = ACS_APBM_CONFIG_working_active_thread_count;
int						__CLASS_NAME__::working_passive_thread_count = ACS_APBM_CONFIG_working_passive_thread_count;
const char *	__CLASS_NAME__::cluster_node_id_filepath = gs_cluster_node_id_filepath.c_str();
const char *	__CLASS_NAME__::cluster_other_node_id_filepath = gs_cluster_other_node_id_filepath.c_str();
const char *	__CLASS_NAME__::ha_daemon_name = gs_ha_daemon_name.c_str();
char					__CLASS_NAME__::magazine_address_imm_separator = ACS_APBM_CONFIG_magazine_address_imm_separator;
unsigned			__CLASS_NAME__::ntf_dispatch_callbacks_delay = ACS_APBM_CONFIG_ntf_dispatch_callbacks_delay;
//const char *	__CLASS_NAME__::ap_node_change_state_su_template = gs_ap_node_change_state_su_template.c_str();
//const char *	__CLASS_NAME__::ap_node_change_state_si_template = gs_ap_node_change_state_si_template.c_str();
unsigned			__CLASS_NAME__::monitoring_service_initial_delay = ACS_APBM_CONFIG_monitoring_service_initial_delay;
unsigned			__CLASS_NAME__::monitoring_service_interval = ACS_APBM_CONFIG_monitoring_service_interval;
int                        __CLASS_NAME__::monitoring_service_on_passive_threshold = ACS_APBM_CONFIG_monitoring_service_on_passive_threshold;
unsigned			__CLASS_NAME__::operation_pipe_scheduler_initial_delay_ms = ACS_APBM_CONFIG_operation_pipe_scheduler_initial_delay_ms;
unsigned			__CLASS_NAME__::operation_pipe_scheduler_interval_ms = ACS_APBM_CONFIG_operation_pipe_scheduler_interval_ms;
unsigned			__CLASS_NAME__::operation_pipe_scheduler_default_ticks_to_wait = ACS_APBM_CONFIG_operation_pipe_scheduler_default_ticks_to_wait;
unsigned			__CLASS_NAME__::operation_pipe_scheduler_bsp_default_ticks_to_wait = ACS_APBM_CONFIG_operation_pipe_scheduler_bsp_default_ticks_to_wait;
std::string		__CLASS_NAME__::trapds_subscription_service_name("ACS_APBMD");
unsigned			__CLASS_NAME__::scxb_coldstart_cnt_threshold = ACS_APBM_CONFIG_scxb_coldstart_cnt_threshold;
int						__CLASS_NAME__::front_port_check_trigger_threshold = ACS_APBM_CONFIG_front_port_check_trigger_threshold;
int						__CLASS_NAME__::update_switchboard_states_trigger_threshold = ACS_APBM_CONFIG_update_switchboard_states_trigger_threshold;
unsigned int	__CLASS_NAME__::switch_board_link_down_max_time = ACS_APBM_CONFIG_scb_link_down_max_time_before_raising_alarm;
const char *	__CLASS_NAME__::imm_oi_class_name = gs_imm_oi_class_name.c_str();

int __CLASS_NAME__::load () { //Just believe it
	return 0;
}
