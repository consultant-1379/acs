/** @file acs_apbm_operationpipeeventhandler.cpp
 *      @brief
 *      @author xnicmut (Nicola Muto)
 *      @date 2011-08-02
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 *      REVISION INFO
 *      +=======+============+==============+=====================================+
 *      | REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *      +=======+============+==============+=====================================+
 *      | R-001 | 2011-08-02 | xnicmut      | Creation and first revision.        |
 *      +=======+============+==============+=====================================+
 *      |       | 2014-06-17 | xsunach      | TR HS54933                          |
 *      +=======+============+==============+=====================================+
 *      |       | 2015-03-19 | xsunach      | CNI 33/6-441                        +         |
 *      +=======+============+==============+=====================================+
 *      |       | 2015-06-12 | xsunach      | CNI 33/6-532                        |
 *      +=======+============+==============+=====================================+
 */

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <map>
#include <set>

#include <ace/Handle_Set.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Guard_T.h>

#include "acs_apbm_macros.h"
#include "acs_apbm_cmdoptionparser.h"
#include "acs_apbm_programconstants.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_programconfiguration.h"
#include <acs_apbm_snmpsessioninfo.h>
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_immrepositoryhandler.h"
#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_hwihandler.h"
#include "acs_apbm_monitoringservicehandler.h"
#include "acs_apbm_operationpipescheduler.h"
#include "acs_apbm_snmptrapdsobserver.h"
#include "acs_apbm_cshwctablechangeobserver.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_scxlagimmhandler.h"
#include "acs_apbm_operationpipeeventhandler.h"

extern std::vector< std::pair<std::string, uint32_t> >  scx_cold_warm_trap_ip_list;		// TR HX50455
extern ACE_Thread_Mutex theLoadMgrMutex;
extern bool scxColdRestartFlag;

//start of HX62204
typedef std::pair <int32_t,uint32_t> BladeInfo; 			// pair<slot,magazine>
typedef std::pair <BladeInfo , int > boardMissingElement;	// pair<BladeInfo, retryCount>
std::vector <boardMissingElement> boardMissingElementList;
ACE_Recursive_Thread_Mutex boardMissingElementList_mutex;
static bool findElementInBoardMissingList(const boardMissingElement board);
//end of HX62204

namespace {
	template <typename param_t>
	int read_operation_param (param_t & param, ACE_HANDLE fd) {
		uint8_t read_buf [sizeof(param)] = {0};
		ssize_t total_bytes_read = 0;
		size_t bytes_to_read = sizeof(param);

		while (bytes_to_read) {
			errno = 0;
			const ssize_t read_result = ::read(fd, read_buf + total_bytes_read, bytes_to_read);

			if (read_result < 0) { // ERROR: on reading from pipe
				int errno_save = errno;

				// I need to check the errno code to view the root cause of the failed read operation
				if ((errno_save == EAGAIN) || (errno_save == EWOULDBLOCK)) { // Data are not yet ready on the pipe
					ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_WARN, "Operation pipe: data are not yet ready on the pipe: ignoring this error retrying the read operation");
				} else if (errno_save == EINTR) { // WARNING: System call read was interrupted by a signal
					ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_WARN, "Operation pipe: '::read' system call interrupted by a signal before any data was read: ignoring this error retrying the read operation");
				} else { // ERROR: all other error will return -1
					ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call '::read' failed: reading a parameter value from server operation pipe");
					break;

					//call_result = -1;

					// TODO:
					// ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE
					// Errore catastrofico: l'operation pipe potrebbe essere corrotta e inutilizzabile
					// Pensare a come gestire la cosa: per esempio lascio il -1 di ritorno e chiamo anche
					// l'end_reactor_event_loop per far terminare il programma e aspettare che CoreMW mi faccia
					// ripartire.
					// Per adesso ignoro l'errore. Ma si potrebbe mettere anche un counter e un timer (max 2 minuti) alla
					// scadenza di uno dei due uscire interrompendo il loop del reactor (end_reactor_event_loop)
				}
			} else if (read_result == 0) {
				// WARNING: reading beyond the end-of-file on the pipe. The pipe seems to be empty; for a NON-BLOCKING pipe this could be normal
				ACS_APBM_LOG(LOG_LEVEL_WARN, "Operation pipe: zero bytes read, ignoring this warning continuing to read");
			} else { // OK: Interpret the operation code and act accordingly
				total_bytes_read += read_result;
				bytes_to_read -= read_result;
			}
		}

		if (!bytes_to_read) { // OK: interpret the parameter value
			param = *reinterpret_cast<param_t *>(read_buf);
		}

		return 0;
	}

	inline bool fx_compare_magazine_slot_ascending (acs_apbm_switchboardinfo left, acs_apbm_switchboardinfo right) {
		return (left.magazine < right.magazine) || ((left.magazine == right.magazine) && (left.slot_position < right.slot_position));
	}
}

int __CLASS_NAME__::handle_input (ACE_HANDLE fd) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation pipe signaled: ready to be read");

	int call_result = 0;
	uint8_t op = acs_apbm::SERVER_WORK_OPERATION_NOP;
	ssize_t read_call_result = 0;
	if ((read_call_result = ::read(fd, &op, 1)) < 0) { // ERROR: reading a byte from the operation pipe
		int errno_save = errno;

		// I need to check the errno code to view the root cause of the failed read operation
		if ((errno_save == EAGAIN) || (errno_save == EWOULDBLOCK)) { // Data are not yet ready on the pipe
			ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_WARN, "Operation pipe: data are not yet ready on the pipe: ignoring this error waiting data to be handled by a thread");
		} else if (errno_save == EINTR) { // WARNING: System call read was interrupted by a signal
			ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_WARN, "Operation pipe: '::read' system call interrupted by a signal before any data was read: ignoring this error, maybe the program is ending");
		} else { // ERROR: all other error will return -1
			ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call '::read' failed: reading a code byte from server operation pipe");

			//call_result = -1;

			// TODO:
			// ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE ATTENZIONE
			// Errore catastrofico: l'operation pipe potrebbe essere corrotta e inutilizzabile
			// Pensare a come gestire la cosa: per esempio lascio il -1 di ritorno e chiamo anche
			// l'end_reactor_event_loop per far terminare il programma e aspettare che CoreMW mi faccia
			// ripartire.
			// Per adesso ignoro l'errore. Ma si potrebbe mettere anche un counter e un timer (max 2 minuti) alla
			// scadenza di uno dei due uscire interrompendo il loop del reactor (end_reactor_event_loop)
		}
	} else if (read_call_result == 0) {
		// WARNING: reading beyond the end-of-file on the pipe. The pipe seems to be empty; for a NON-BLOCKING pipe this could be normal
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Operation pipe: zero bytes read, maybe another thread has already handled data: ignoring this warning");
	} else { // OK: Interpret the operation code and act accordingly
		call_result = handle_pipe_operation(fd, op);
	}
	return call_result;
}

int __CLASS_NAME__::handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/) {
	return acs_apbm::ERR_NO_ERRORS;
}

bool findElementInBoardMissingList(const boardMissingElement board)			// HX62204
{
	std::vector<boardMissingElement>::iterator it;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
	for(it = boardMissingElementList.begin(); it != boardMissingElementList.end(); it++)
	{
		if((board.first.first == it->first.first) && (board.first.second == it->first.second))
			return true;
	}
	return false;
}

int __CLASS_NAME__::handle_pipe_operation (ACE_HANDLE fd, int op) {
	int return_code = 0;
	int (__CLASS_NAME__::*	op_function) (uint32_t) = 0;
	int32_t int32_param = -1;
	uint32_t uint32_param = 0;

	switch (op) {
	case acs_apbm::SERVER_WORK_OPERATION_NOP: return_code = op_nop(); break;
	case acs_apbm::SERVER_WORK_OPERATION_EXIT_PROGRAM: return_code = op_exit_program(); break;
	case acs_apbm::SERVER_WORK_OPERATION_INIT_SIGNALS_HANDLER: return_code = op_init_signals_handler(); break;
	case acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_ACTIVE: return_code = op_start_work_node_active(); break;
	case acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_PASSIVE: return_code = op_start_work_node_passive(); break;
	case acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_UNDEFINED: return_code = op_start_work_node_undefined(); break;

	case acs_apbm::SERVER_WORK_OPERATION_START_WORK_QUIESCED: return_code = op_start_work_quiesced(); break;

	case acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_EXIT_PROGRAM: return_code = op_stop_work_to_exit_program(); break;
	case acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_ACTIVE:
		return_code = op_stop_work_to_change_state_to(acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE,
				acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_ACTIVE);
		break;
	case acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_PASSIVE:
		return_code = op_stop_work_to_change_state_to(acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE,
				acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_PASSIVE);
		break;
	case acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_UNDEFINED:
		return_code = op_stop_work_to_change_state_to(acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED,
				acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_UNDEFINED);
		break;
	case acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_QUIESCED:
		return_code = op_stop_work_to_change_state_to(acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED,
				acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_QUIESCED);
		break;
	case acs_apbm::SERVER_WORK_OPERATION_ACTIVATE_SERVER_WORK_TASK: return_code = op_activate_server_work_task(); break;
	case acs_apbm::SERVER_WORK_OPERATION_IMM_OPEN_MANAGEMENT_SESSION: return_code = op_imm_open_management_session(); break;
	case acs_apbm::SERVER_WORK_OPERATION_IMM_TAKE_OWNERSHIP: return_code = op_imm_take_ownership(); break;
	case acs_apbm::SERVER_WORK_OPERATION_IMM_CLOSE_MANAGEMENT_SESSION: return_code = op_imm_close_management_session(); break;
	case acs_apbm::SERVER_WORK_OPERATION_IMM_RELEASE_OWNERSHIP: return_code = op_imm_release_ownership(); break;

	case acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_OPEN: return_code = op_api_service_acceptor_unix_open(); break;
	case acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_OPEN: return_code = op_api_service_acceptor_inet_open(); break;

	case acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_CLOSE: return_code = op_api_service_acceptor_unix_close(); break;
	case acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_CLOSE: return_code = op_api_service_acceptor_inet_close(); break;

	case acs_apbm::SERVER_WORK_OPERATION_CS_LOAD_BOARD_DATA: return_code = op_cs_load_board_data(false); break;
	case acs_apbm::SERVER_WORK_OPERATION_CS_LOAD_BOARD_DATA_IMM: return_code = op_cs_load_board_data(true); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_SUBSCRIBE: return_code = op_snmp_trap_subscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE: return_code = op_snmp_trap_unsubscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_LEFT: return_code = op_snmp_trap_unsubscribe_left(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_RIGHT: return_code = op_snmp_trap_unsubscribe_right(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE: return_code = op_snmp_trap_unsubscribe_subscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA: return_code = op_snmp_load_board_data(false); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM: return_code = op_snmp_load_board_data(true); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_FIND_SWITCH_BOARD_MASTER: return_code = op_snmp_find_switch_board_master(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_OPEN: return_code = op_snmp_hwi_handler_open(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_RELOAD_DATA: return_code = op_snmp_hwi_handler_reload(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_CLOSE: return_code = op_snmp_hwi_handler_close(); break;
	case acs_apbm::SERVER_WORK_OPERATION_START_MONITORING_SERVICE: return_code = op_start_monitoring_service(); break;
	case acs_apbm::SERVER_WORK_OPERATION_STOP_MONITORING_SERVICE: return_code = op_stop_monitoring_service(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAPDS_SUBSCRIBE: return_code = op_snmp_trapds_subscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAPDS_UNSUBSCRIBE: return_code = op_snmp_trapds_unsubscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_CS_HWC_TABLE_CHANGE_OBSERVER_START: return_code = op_cs_hwc_table_change_observer_start(); break;
	case acs_apbm::SERVER_WORK_OPERATION_CS_HWC_TABLE_CHANGE_OBSERVER_STOP: return_code = op_cs_hwc_table_change_observer_stop(); break;
	case acs_apbm::SERVER_WORK_OPERATION_WORK_UNDEFINED_NODE_STATE_CHECK: return_code = op_work_undefined_node_state_check(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SHELF_MNGCTRL_TRAP_COUNTER_CHECK: return_code = op_shelf_mngctrl_trap_counter_check(); break;

	case acs_apbm::SERVER_WORK_OPERATION_SNMP_MAGAZINE_TRAP_SUBSCRIBE:
		op_function = &__CLASS_NAME__::op_snmp_magazine_trap_subscribe;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_MAGAZINE_TRAP_UNSUBSCRIBE:
		op_function || (op_function = &__CLASS_NAME__::op_snmp_magazine_trap_unsubscribe);
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_MAGAZINE_TRAP_UNSUBSCRIBE_SUBSCRIBE:
		op_function || (op_function = &__CLASS_NAME__::op_snmp_magazine_trap_unsubscribe_subscribe);

		// Get the operation parameter
		errno = 0;
		if (const int call_result = read_operation_param(uint32_param, fd)) {
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_FATAL, "Call 'read_operation_param' failed: cannot read the operation parameter from the operation pipe: "
					"cannot execute the operation %d: call_result == %d", op, call_result);
			break;
		}

		// OK: I can call the operation handler function;
		return_code = (this->*op_function)(uint32_param);
		break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE: return_code = op_snmp_other_magazines_trap_subscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE: return_code = op_snmp_other_magazines_trap_unsubscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE: return_code = op_snmp_other_magazines_trap_unsubscribe_subscribe(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCHEDULE_UPDATE_BOARD_STATUS:
	{
		// Start of HX62204
		BladeInfo tmp_blade;
		boardMissingElement tmp_element;
		bool boardFoundInList = false;
		// Get the operation parameters
		int32_param = -1; // The slot parameter
		uint32_param = 0; // The magazine parameter
		errno = 0;
		if (const int call_result = read_operation_param(int32_param, fd)) {
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_FATAL,
					"Call 'read_operation_param' failed: cannot read the operation parameter 'slot' from the operation pipe: "
					"cannot execute the operation %d: call_result == %d", op, call_result);
			break;
		}
		if (const int call_result = read_operation_param(uint32_param, fd)) {
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_FATAL,
					"Call 'read_operation_param' failed: cannot read the operation parameter 'magazine' from the operation pipe: "
					"cannot execute the operation %d: call_result == %d", op, call_result);
			break;
		}
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Handling operation SERVER_WORK_OPERATION_SCHEDULE_UPDATE_BOARD_STATUS - Board Missing alarm raised earlier in update_callback_impl()"
				"  for slot == %d and magazine == 0x%08X",int32_param,uint32_param);
		tmp_blade.first = int32_param;
		tmp_blade.second = uint32_param;
		tmp_element.first = tmp_blade;
		tmp_element.second = 0;
		boardFoundInList = findElementInBoardMissingList(tmp_element);

		if(!boardFoundInList)
		{
			{
				ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
				boardMissingElementList.push_back(tmp_element);
			}

			if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS,60))
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'schedule_operation' failed: cannot reschedule the same operation SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS call_result == %d", call_result);
				{
					ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
					if(!boardMissingElementList.empty())
						boardMissingElementList.pop_back();
				}
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Scheduling operation SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS FAILED for slot == %d and magazine == 0x%08X, removing this board from boardMissingElementList",tmp_element.first.first,tmp_element.first.second);
			}
			else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS scheduled successfully for slot == %d and magazine == 0x%08X; operation delay is 60 seconds",tmp_element.first.first,tmp_element.first.second);
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Board status update operation already scheduled for the slot == %d and magazine == 0x%08X",tmp_element.first.first,tmp_element.first.second);

		return_code = 0;
		break;
		// End of HX62204
	}
	case acs_apbm::SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS:
	{
		// Start of HX62204
		std::vector<boardMissingElement>::iterator it_b;
		boardMissingElement tmp_element;
		BladeInfo tmp_blade;
		int32_t slot;
		uint32_t magazine;
		int retryCount = 0;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Handling operation SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS");
		{
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
			if(boardMissingElementList.empty())
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "In operation SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS, boardMissingElementList is empty - nothing to do!"); // should not happen
				return_code = 0;
				break;
			}
		}

		// boardMissingElementList is not empty. Update board status for first element in the vector
		{
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
			ACS_APBM_LOG(LOG_LEVEL_TRACE, "boardMissingElementList is not empty, size of boardMissingElementList is %d",boardMissingElementList.size());
			it_b = boardMissingElementList.begin();
			slot = it_b->first.first ;
			magazine = it_b->first.second;
			retryCount = it_b->second;
		}
		ACS_APBM_LOG(LOG_LEVEL_WARN, "boardMissingElementList: Board Status Update is going to be performed for slot == %d; magazine == 0x%08X; retryCount == %d",slot,magazine,retryCount);

		if(op_update_board_status(slot, magazine) == 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Successfully updated board status for slot == %d, magazine == 0x%08X. Removing from boardMissingElementList",slot,magazine);
			{
				ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
				boardMissingElementList.erase(boardMissingElementList.begin());
			}
		}
		else
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "op_update_board_status() failed - slot == %d and magazine == 0x%08X; retryCount == %d",slot,magazine,retryCount);
			if(retryCount < 2)
			{
				retryCount++;
				tmp_blade.first = slot;
				tmp_blade.second = magazine;
				tmp_element.first = tmp_blade;
				tmp_element.second = retryCount;

				{
					ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
					boardMissingElementList.erase(boardMissingElementList.begin());
					boardMissingElementList.push_back(tmp_element);
				}
				int new_ticks_to_wait = 300;
				if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS,
						new_ticks_to_wait))
				{
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'schedule_operation' failed: cannot reschedule the same operation SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS call_result == %d. Removing from boardMissingElementList", call_result);
					{
						ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
						boardMissingElementList.pop_back();
						ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Board Missing (board status update op) - Number of boards in boardMissingElementList == %d",boardMissingElementList.size());
					}

				}
				else
					ACS_APBM_LOG(LOG_LEVEL_WARN, "SERVER_WORK_OPERATION_UPDATE_BOARD_STATUS scheduled for slot == %d; mag == 0x%08X; retryCount == %d; operation delay is 300 seconds",tmp_blade.first,tmp_blade.second,tmp_element.second);
			}
			else if (retryCount == 2)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Board Missing - Board status update failed 3 times! Do not try again! slot == %d, magazine == 0x%08X. Removing from boardMissingElementList", slot, magazine);
				{
					ACE_Guard<ACE_Recursive_Thread_Mutex> guard(boardMissingElementList_mutex);
					boardMissingElementList.erase(boardMissingElementList.begin());
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Removed board from list. Number of boards in boardMissingElementList == %d",boardMissingElementList.size());
				}
			}
		}
		return_code = 0;
		break;
		// End of HX62204
	}
	case acs_apbm::SERVER_WORK_OPERATION_TESTING_NODE_ACTIVE_1: return_code = op_testing_node_active_1(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCX_COLD_TRAP_SCHEDULE:
		theLoadMgrMutex.acquire();
			// start of fix for TR HX50455
		if(!scx_cold_warm_trap_ip_list.empty())
		{
			std::string ipToPerformColdTrapOp = scx_cold_warm_trap_ip_list.front().first;
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Handling cold restart trap operation for SCX IP - %s, received at tick - %u", ipToPerformColdTrapOp.c_str(), scx_cold_warm_trap_ip_list.front().second);
			scx_cold_warm_trap_ip_list.erase(scx_cold_warm_trap_ip_list.begin());
			theLoadMgrMutex.release();
			// end of fix for TR HX50455
			return_code = _server_working_set->snmp_manager->manage_ColdStart(ipToPerformColdTrapOp.c_str());
		}
		else
		{
			theLoadMgrMutex.release();
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "scx_cold_warm_trap_ip_list vector is empty even though cold restart trap operation was scheduled!");
		}
	    break;
    case acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM_COLD: return_code = op_snmp_load_board_data(true); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_IMM_HANDLER_OPEN: return_code = op_scx_lag_imm_handler_open(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_IMM_HANDLER_CLOSE: return_code = op_scx_lag_imm_handler_close(); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_CONFIGURE_FAILURE_SIDEA: return_code = _server_working_set->snmp_manager->configure_front_port_scx_lag(acs_apbm::BOARD_SCXB_LEFT,acs_apbm_snmp::ENABLE_LAG); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_CONFIGURE_FAILURE_SIDEB: return_code = _server_working_set->snmp_manager->configure_front_port_scx_lag(acs_apbm::BOARD_SCXB_RIGHT,acs_apbm_snmp::ENABLE_LAG); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_DISABLE_FAILURE_SIDEA: return_code = _server_working_set->snmp_manager->configure_front_port_scx_lag(acs_apbm::BOARD_SCXB_LEFT,acs_apbm_snmp::DISABLE_LAG); break;
	case acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_DISABLE_FAILURE_SIDEB: return_code =_server_working_set->snmp_manager->configure_front_port_scx_lag(acs_apbm::BOARD_SCXB_RIGHT,acs_apbm_snmp::DISABLE_LAG); break;

	 //TR HX65505
	case acs_apbm::SERVER_WORK_OPERATION_CEASE_ALL_ALARMS_SHUTDOWN: return_code = op_cease_all_alarms_shutdown(); break;		 //TR HX65505
	default: break;
	}
	return return_code;
}

int __CLASS_NAME__::op_nop () {
	return 0;
}

int __CLASS_NAME__::op_exit_program () {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Program termination requested");

	// Signal the program termination
	_server_working_set->program_state = acs_apbm::PROGRAM_STATE_EXIT_PROGRAM;

	// TODO: Qui si potrebbe mettere del codice di controllo per verificare se tutto e` stato
	// chiuso correttemente e tutte le risorse sono state liberate prima di uscire dal
	// reactor event loop.

	// Ending the main reactor event loop
	if (!reactor()->reactor_event_loop_done()) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ending the main reactor event loop...");
		if (reactor()->end_reactor_event_loop()) { // ERROR
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'end_reactor_event_loop' failed: trying to end the main reactor event loop upon 'PROGRAM EXIT' operation received");

			// This event handler cannot be deleted from the reactor because operation will be pending for ever.

			// So cannot return -1. What could be done is re-send an EXIT_PROGRAM operation
			// Retry to send the termination request operation onto the pipe
			if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_EXIT_PROGRAM)) // ERROR: sending the operation
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot send the EXIT_PROGRAM operation to the server work object: you must brutally killing the APBM server");
		}
	}

	return 0;
}

int __CLASS_NAME__::op_init_signals_handler () {
	ACS_APBM_LOG(LOG_LEVEL_WARN, "Trying to re-initialize the signals handler");

	if (_server_working_set->init_signals_handler()) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'init_signals_handler' failed: process signal handler initialization failure: APBM server will retry asap to reinitialize signals handler");

		// If process signals handler initialization failed then try to put an INIT_SIGNALS_HANDLER operation on the operation pipe
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_INIT_SIGNALS_HANDLER))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an INIT_SIGNALS_HANDLER operation to re-initialize the process signals handler");
	} else ACS_APBM_LOG(LOG_LEVEL_WARN, "Signals handler successfully initialized");

	return 0;
}

int __CLASS_NAME__::op_start_work_node_active () {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Starting work in ACTIVE NODE STATE mode");

	// Signal that I'm going to work active
	_server_working_set->program_state = acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE;

	
	// TODO: Increment here the thread count

	// Order the operation to open the UNIX socket to accept application requests through the APBM-API
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_OPEN)) // ERRORE
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_UNIX_OPEN operation to open a UNIX socket listening session");

	// Order the operation to open the INET socket to accept application requests through the APBM-API
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_OPEN)) // ERRORE
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_INET_OPEN operation to open an INET socket listening session");

	// Order an 'open IMM management session' operation to read IMM object attributes
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_IMM_OPEN_MANAGEMENT_SESSION)) // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an IMM_OPEN_MANAGEMENT_SESSION operation to open an IMM management session");

	// Order a 'take IMM ownership' operation to be able to create IMM server runtime objects
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_IMM_TAKE_OWNERSHIP))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an IMM_TAKE_OWNERSHIP operation to take ownership of APBM runtime IMM objects");

	// Order a 'load board data from CS with IMM' operation to load and create boards in memory and IMM
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_CS_LOAD_BOARD_DATA_IMM))
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'operation_send' failed: cannot order a CS_LOAD_BOARD_DATA_IMM operation to load board data from CS and update them in IMM");

	int environment = 0;
	_server_working_set->cs_reader->get_environment(environment);

	if (environment != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{	
		// Order an 'subscribe process' operation to subscribe to the SNMP trap dispatcher service
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAPDS_SUBSCRIBE))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an SNMP_TRAPDS_SUBSCRIBE operation to subscribe APBM the the SNMP trap dispatcher service");
	// Order an 'unsubscribe-subscribe process' operation to unsubscribe and then subscribe to receive SNMP trap messages
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to do the unsubscribe-subscribe process");

	}

	// Operations ordered in a CBA architecture only
	if (_server_working_set->cs_reader->is_cba_system()) {
		// Order an 'unsubscribe-subscribe process' operation to unsubscribe and then subscribe to all switch boards in all other magazines
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE))
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'operation_send' failed: cannot order a SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to do the unsubscribe-subscribe process for all switch boards in all other magazines");

		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_FIND_SWITCH_BOARD_MASTER))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a SNMP_FIND_SWITCH_BOARD_MASTER operation to find the master switch board");
		else { // OK: I can load snmp data
			if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM))
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'operation_send' failed: cannot order a SNMP_LOAD_BOARD_DATA_IMM operation to load board data from SNMP agent and update them in IMM");
		}

		// Order an HWI-Handler-Open operation
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_OPEN))
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a SNMP_HWI_HANDLER_OPEN operation to open the HWI handler and start to manage hardware inventory");
        // scx lag imm handler
		if(environment == ACS_CS_API_CommonBasedArchitecture::SCX)
		{
			if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_IMM_HANDLER_OPEN))
							ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order SCX_LAG_IMM_HANDLER_OPEN operation to open SCX LAG imm handler");
		}
	}
	else if (environment == ACS_CS_API_CommonBasedArchitecture::DMX)
	{
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM))
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'operation_send' failed: cannot order a SNMP_LOAD_BOARD_DATA_IMM operation to load board data from SNMP agent and update them in IMM");
	}
	// Order a 'start observing' operation to start the CS HWC table change observing CS notification
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_CS_HWC_TABLE_CHANGE_OBSERVER_START))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a CS_HWC_TABLE_CHANGE_OBSERVER_START operation to subscribe the CS HWC table change observer to receive CS notifications");

	// order starting the Monitoring Service
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_START_MONITORING_SERVICE))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a  START_MONITORING_SERVICE operation");

	// TODO: To be continued


/* Uncomment these following lines to launch a test execution
 * just after the switch into the active node state
	// Testing session operation called when the node become active
	// Order a 'testing node active 1' operation to perform some testing operation when the node become active
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_TESTING_NODE_ACTIVE_1))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a TESTING_NODE_ACTIVE_1 operation to perform some testing operation when node become active");
*/


	return 0;
}

int __CLASS_NAME__::op_start_work_node_passive () {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Starting work in PASSIVE NODE STATE mode");

	// Signal that I'm going to work passive
	_server_working_set->program_state = acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE;

	// Order the operation to open the UNIX socket to accept application requests through the APBM-API
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_OPEN)) // ERRORE
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_UNIX_OPEN operation to open a UNIX socket listening session");

	// Order the operation to open the INET socket to accept application requests through the APBM-API
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_OPEN)) // ERRORE
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_INET_OPEN operation to open an INET socket listening session");

	int environment = 0;
	_server_working_set->cs_reader->get_environment(environment);
        if (environment == ACS_CS_API_CommonBasedArchitecture::DMX || 1)
        {
            if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_START_MONITORING_SERVICE))
              ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a  START_MONITORING_SERVICE operation");
        }
        // order starting the Monitoring Service

	return 0;
}

int __CLASS_NAME__::op_start_work_quiesced () {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Starting work in QUIESCED mode");

	// Signal that I'm going to work quiesced
	_server_working_set->program_state = acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED;

	return 0;
}

int __CLASS_NAME__::op_start_work_node_undefined () {
	// Signal that I'm going to work passive
	_server_working_set->program_state = acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Starting work in UNDEFINED NODE STATE mode");

	//reset the node state check retry counter
	_work_undefined_node_state_check_retry_count = 0;

	// Order a WORK_UNDEFINED_NODE_STATE_CHECK operation to check periodically my node state
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_WORK_UNDEFINED_NODE_STATE_CHECK)) // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a WORK_UNDEFINED_NODE_STATE_CHECK operation to check periodically my node state");

	return 0;
}

int __CLASS_NAME__::op_stop_work_to_exit_program () {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Stopping the work to exit program...");

	if (const int call_result = stop_working()) { // ERROR: stopping activities
		// The APBM server is exiting so we do not worry about stopping resource errors
		// Only logging
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'stop_working' failed: some problems stopping activities: ignoring these issues because APBM server is exiting: call_result == %d",
				call_result);
	}


	// TODO: If needed put here some checks on closing.


	// Order a 'PROGRAM_EXIT' operation to exit program
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_EXIT_PROGRAM)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an EXIT_PROGRAM operation to exit APBM server");

		if (_server_working_set->cmdoption_parser->noha()) {
			// Resend a termination signal to this process to restart the exit work flow
			if (::kill(::getpid(), SIGTERM)) ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call '::kill' failed: sending the termination signal to myself");
		}

	}

	return 0;
}

int __CLASS_NAME__::op_stop_work_to_change_state_to (acs_apbm::ProgramStateConstants new_state, acs_apbm::ServerWorkOperationCodeConstants operation) {
	// Save the current program state
	acs_apbm::ProgramStateConstants program_state_save = _server_working_set->program_state;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Stopping the work to change APBM working state: switching working from current state %d to the new state %d",
			program_state_save, new_state);

	// Is the server going to run in quiesced mode
	if (new_state == acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) { // YES: Order a stop on API listening sockets
		// Order the operation to close the INET socket
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_CLOSE)) // ERRORE
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_INET_CLOSE operation to close the INET socket listening session");

		// Order the operation to close the UNIX socket
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_CLOSE)) // ERRORE
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_UNIX_CLOSE operation to close the UNIX socket listening session");
	}

	if (new_state == program_state_save) { // WARNING: we are switching in the same working state
		ACS_APBM_LOG(LOG_LEVEL_WARN, "APBM was requested to switch in the same working state: this working state transition is stopped");
		return 0;
	}

	if (const int call_result = stop_working()) { // ERROR: stopping activities
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'stop_working' failed: some problems stopping activities: call_result == %d", call_result);

		// Order the same 'SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_*' operation to retry to change working state
		if (_server_working_set->operation_send(operation))
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Stop Workign To Change State: Call 'operation_send' failed: cannot re-send the STOP-WORKING-TO-CHANGE-STATE-TO operation (%d) "
					"to retry changing working state");
	}

	// Now the right operation to change working state and activities can be dispatched
	acs_apbm::ServerWorkOperationCodeConstants op;
	switch (new_state) {
	case acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_ACTIVE; break;
	case acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_PASSIVE; break;
	case acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_QUIESCED; break;
	default: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_UNDEFINED;
	}

	if (_server_working_set->operation_send(op)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: trying to send the operation code '%d' to "
				"change the APBM working state and activities job", op);
	}

	return 0;
}

int __CLASS_NAME__::op_activate_server_work_task () {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Re-activating the main worker task base...");

	if (_server_working_set->server_work_task->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, acs_apbm_programconfiguration::working_passive_thread_count) == -1) {
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'activate' failed: cannot activate the main worker task: the server will try to reactivate this worker task asap");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_ACTIVATE_SERVER_WORK_TASK,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule the same operation ACTIVATE_SERVER_WORK_TASK "
					"to re-activate the server work task: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "ACTIVATE_SERVER_WORK_TASK operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main worker task base successfully activated");

	return 0;
}

int __CLASS_NAME__::op_imm_open_management_session () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Opening the IMM management session...");
	if (_server_working_set->imm_repository_handler->open_management_session()) {
		// ERROR: opening the IMM management session
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_management_session' failed: IMM management session open failure! The APBM server will retry later");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_IMM_OPEN_MANAGEMENT_SESSION,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an IMM_OPEN_MANAGEMENT_SESSION operation to "
					"retry opening an IMM management session: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "IMM_OPEN_MANAGEMENT_SESSION operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "IMM management session opened successfully");

	return 0;
}

int __CLASS_NAME__::op_imm_take_ownership () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Taking ownership of APBM runtime IMM objects...");
	if (_server_working_set->imm_repository_handler->take_ownership(acs_apbm_programconfiguration::imm_implementer_name_runtime)) {
		// ERROR: taking IMM objects ownership
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'take_ownership' failed: taking server IMM runtime objects ownership failed! The APBM server will retry later");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_IMM_TAKE_OWNERSHIP,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an IMM_TAKE_OWNERSHIP operation to retry taking "
					"APBM IMM objects ownership: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "IMM_TAKE_OWNERSHIP operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "The APBM server took IMM runtime objects ownership successfully");

	return 0;
}

int __CLASS_NAME__::op_imm_close_management_session () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Closing the IMM management session...");
	if (_server_working_set->imm_repository_handler->close_management_session()) {
		// ERROR: closing the IMM management session
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'close_management_session' failed: IMM management session close failure! The APBM server will retry later");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_IMM_CLOSE_MANAGEMENT_SESSION,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an IMM_CLOSE_MANAGEMENT_SESSION operation to "
					"retry closing an IMM management session: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "IMM_CLOSE_MANAGEMENT_SESSION operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "IMM management session closed successfully");

	return 0;
}

int __CLASS_NAME__::op_imm_release_ownership () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Releasing ownership of APBM runtime IMM objects...");
	if (_server_working_set->imm_repository_handler->release_ownership()) {
		// ERROR: releasing IMM objects ownership
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'release_ownership' failed: releasing server IMM runtime objects ownership failed! The APBM server will retry later");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_IMM_RELEASE_OWNERSHIP,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an IMM_RELEASE_OWNERSHIP operation to retry "
					"releasing APBM IMM objects ownership: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "IMM_RELEASE_OWNERSHIP operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "The APBM server released IMM runtime objects ownership successfully");

	return 0;
}

int __CLASS_NAME__::op_api_service_acceptor_unix_open () {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: Trying to reopen the API service acceptor UNIX object (sap == '%s')...", ACS_APBM_CONFIG_APBMAPI_UNIX_SOCKET_SAP_ID);

	if (_server_working_set->open_api_service_acceptor_unix()) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service acceptor: Call 'open_api_service_acceptor_unix' failed: API service open failure: APBM server will retry asap to reopen the service");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_OPEN,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an API_SERVICE_ACCEPTOR_UNIX_OPEN operation to "
					"retry starting listening for clients throught APBM API: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "API_SERVICE_ACCEPTOR_UNIX_OPEN operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: API service acceptor UNIX object opened successfully!");

	return 0;
}

int __CLASS_NAME__::op_api_service_acceptor_inet_open () {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: Trying to reopen the API service acceptor INET object (sap == '%s')...", ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID);

	if (_server_working_set->open_api_service_acceptor_inet()) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service acceptor: Call 'open_api_service_acceptor_inet' failed: API service open failure: APBM server will retry asap to reopen the service");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_OPEN,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an API_SERVICE_ACCEPTOR_INET_OPEN operation "
					"to retry starting listening for clients throught APBM API: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "API_SERVICE_ACCEPTOR_INET_OPEN operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: API service acceptor INET object opened successfully!");

	return 0;
}

int __CLASS_NAME__::op_api_service_acceptor_unix_close () {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: Trying to close the API service acceptor UNIX object (sap == '%s')...", ACS_APBM_CONFIG_APBMAPI_UNIX_SOCKET_SAP_ID);

	if (_server_working_set->close_api_service_acceptor_unix()) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service acceptor: Call 'close_api_service_acceptor_unix' failed: API service close failure: APBM server will retry asap to re-close the service");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_CLOSE,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an API_SERVICE_ACCEPTOR_UNIX_CLOSE operation to "
					"retry stop listening for clients throught APBM API: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "API_SERVICE_ACCEPTOR_UNIX_CLOSE operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: API service acceptor UNIX object closed successfully!");

	return 0;
}

int __CLASS_NAME__::op_api_service_acceptor_inet_close () {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: Trying to close the API service acceptor INET object (sap == '%s')...", ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID);

	if (_server_working_set->close_api_service_acceptor_inet()) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service acceptor: Call 'close_api_service_acceptor_inet' failed: API service close failure: APBM server will retry asap to re-close the service");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_CLOSE,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an API_SERVICE_ACCEPTOR_INET_CLOSE operation to "
					"retry stop listening for clients throught APBM API: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "API_SERVICE_ACCEPTOR_INET_CLOSE operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service acceptor: API service acceptor INET object closed successfully!");

	return 0;
}

int __CLASS_NAME__::op_cs_load_board_data (bool add_info_to_IMM) {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Loading board data information from CS...");
	if (const int call_result = _server_working_set->shelves_data_manager->load_from_CS(add_info_to_IMM)) { // ERROR: loading board data from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'load_from_CS' failed: loading board data from CS! The APBM server will retry later: call_result == %d",
				call_result);

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					add_info_to_IMM ? acs_apbm::SERVER_WORK_OPERATION_CS_LOAD_BOARD_DATA_IMM : acs_apbm::SERVER_WORK_OPERATION_CS_LOAD_BOARD_DATA,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a CS_LOAD_BOARD_DATA%s operation to "
					"retry loading boar data from CS: call_result == %d", add_info_to_IMM ? "_IMM" : "", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "CS_LOAD_BOARD_DATA%s operation rescheduled successfully", add_info_to_IMM ? "_IMM" : "");
	} else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Board data successfully loaded from CS!");
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_trap_subscribe () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	// Reset the shelf manager control trap counter into the SNMP manager
	_server_working_set->snmp_manager->shelf_manager_control_left_trap_counter(0);
	_server_working_set->snmp_manager->shelf_manager_control_right_trap_counter(0);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to subscribe to SNMP switch board agent for receiving SNMP trap messages...");
	if (_server_working_set->snmp_manager->send_trap_subscription_request()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_subscription_request' failed: cannot subscribe to receive SNMP trap messages");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_SUBSCRIBE,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_SUBSCRIBE operation to retry "
					"the subscription process: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_SUBSCRIBE operation rescheduled successfully");
	} else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully subscribed to SNMP switch board agent to receive SNMP trap messages");

		ACS_APBM_LOG(LOG_LEVEL_INFO, "Scheduling the SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation to check the trap received counter");
		const int call_result =
				_server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SHELF_MNGCTRL_TRAP_COUNTER_CHECK, 20);

		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation: call_result == %d",
					call_result);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation rescheduled successfully");
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_trap_unsubscribe () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to unsubscribe receiving SNMP trap messages from SNMP switch board agent...");
	int call_result =_server_working_set->snmp_manager->send_trap_unsubscription_request();

	if (call_result == acs_apbm_snmp::ERR_SNMP_UNSUBSCRIBE_BOTH_SB) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_unsubscription_request' failed: cannot unsubscribe from receiving SNMP trap messages");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE operation to retry "
					"the unsubscription process: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE operation rescheduled successfully");
	}
	else if (call_result == acs_apbm_snmp::ERR_SNMP_UNSUBSCRIBE_LEFT_SB) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_unsubscription_request' failed: cannot unsubscribe from receiving SNMP trap messages from switch board on the left");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_LEFT,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_LEFT operation to retry "
					"the unsubscription process: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE_LEFT operation rescheduled successfully");
	}
	else if (call_result == acs_apbm_snmp::ERR_SNMP_UNSUBSCRIBE_RIGHT_SB) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_unsubscription_request' failed: cannot unsubscribe from receiving SNMP trap messages from switch board on the right");

			// Reschedule the same operation after a default timeout
			if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
						acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_RIGHT,
						acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_RIGHT operation to retry "
						"the unsubscription process: call_result == %d", call_result);
			} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE_RIGHT operation rescheduled successfully");
	 }
	else ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully unsubscribed receiving SNMP trap messages from SNMP switch board agent");

	return 0;
}

int __CLASS_NAME__::op_snmp_trap_unsubscribe_left () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to unsubscribe receiving SNMP trap messages from SNMP switch board left agent...");
	if (_server_working_set->snmp_manager->send_trap_unsubscription_request(acs_apbm_programconfiguration::slot_position_switch_board_left) == acs_apbm::ERR_SEND_PRIMITIVE_UNSUBSCRIBE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_unsubscription_request' failed: cannot unsubscribe from receiving SNMP trap messages");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_LEFT,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_LEFT operation to retry "
					"the unsubscription process: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully unsubscribed receiving SNMP trap messages from SNMP switch board agent");

	return 0;
}

int __CLASS_NAME__::op_snmp_trap_unsubscribe_right () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to unsubscribe receiving SNMP trap messages from SNMP switch board right agent...");
	if (_server_working_set->snmp_manager->send_trap_unsubscription_request(acs_apbm_programconfiguration::slot_position_switch_board_right) == acs_apbm::ERR_SEND_PRIMITIVE_UNSUBSCRIBE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_unsubscription_request' failed: cannot unsubscribe from receiving SNMP trap messages");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_RIGHT,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_LEFT operation to retry "
					"the unsubscription process: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE RIGHT operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully unsubscribed receiving SNMP trap messages from SNMP switch board right agent");

	return 0;
}


int __CLASS_NAME__::op_snmp_trap_unsubscribe_subscribe () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	/*APBM has to send NO SCAN before unsubscribe because it can come from transition state
	// and it has to reset IPMI status of both switch boards
	if(!_server_working_set->cs_reader->is_cba_system())
		if (_server_working_set->snmp_manager->send_no_scan_scbrp()) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'send_no_scan_scbrp' failed: cannot send NO SCAN to switch_board");
		}
		*/
	_server_working_set->snmp_manager->initialize_switch_board_state();

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to unsubscribe receiving SNMP trap messages from SNMP switch board agent before subscribe...");
	if (_server_working_set->snmp_manager->send_trap_unsubscription_request()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_unsubscription_request' failed: cannot unsubscribe from receiving SNMP trap messages");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to "
					"retry the unsubscribe-subscribe process: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation rescheduled successfully");

		return 0;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully unsubscribed receiving SNMP trap messages from SNMP switch board agent");

	// Reset the shelf manager control trap counter into the SNMP manager
	_server_working_set->snmp_manager->shelf_manager_control_left_trap_counter(0);
	_server_working_set->snmp_manager->shelf_manager_control_right_trap_counter(0);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to subscribe receiving SNMP trap messages from SNMP switch board agent...");
	if (_server_working_set->snmp_manager->send_trap_subscription_request()) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_trap_subscription_request' failed: cannot subscribe to receive SNMP trap messages");

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_SUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_SUBSCRIBE operation "
					"to retry the subscribe process: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_SUBSCRIBE operation rescheduled successfully");
	} else { // OK
		ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully subscribed receiving SNMP trap messages from SNMP switch board agent");

		ACS_APBM_LOG(LOG_LEVEL_INFO, "Scheduling the SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation to check the trap received counter");
		const int call_result =
				_server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SHELF_MNGCTRL_TRAP_COUNTER_CHECK, 20);

		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation: call_result == %d",
					call_result);
		else ACS_APBM_LOG(LOG_LEVEL_INFO, "SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation rescheduled successfully");
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_load_board_data (bool add_info_to_IMM) {
	
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;
	int environment = 0;
	_server_working_set->cs_reader->get_environment(environment);
	
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "skipping load_snmp_data in Virtualized env");		
		return 0;	
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Loading board data information from SNMP agent...");
	if (const int call_result = _server_working_set->snmp_manager->load_snmp_data(add_info_to_IMM)) {
		// ERROR: loading board data from SNMP Agent
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'load_snmp_data' failed: loading board data from SNMP agent! The APBM server will retry later: call_result == %d",
				call_result);
		uint32_t ticks_to_wait = (environment == ACS_CS_API_CommonBasedArchitecture::DMX)? acs_apbm_programconfiguration::operation_pipe_scheduler_bsp_default_ticks_to_wait :acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait;


		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				add_info_to_IMM ? acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM : acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA,ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_LOAD_BOARD_DATA%s operation to retry loading boar "
					"data from SNMP agent: call_result == %d", add_info_to_IMM ? "_IMM" : "", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP_LOAD_BOARD_DATA%s operation rescheduled successfully", add_info_to_IMM ? "_IMM" : "");
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Board data successfully loaded from SNMP agent!");
		scxColdRestartFlag = true;
	}
	return 0;
}

int __CLASS_NAME__::op_snmp_find_switch_board_master () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	unsigned timeout_ms = 1000;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Finding the switch board master in my magazine...");
	int find_master_op_result = _server_working_set->snmp_manager->find_switch_board_master(&timeout_ms);
	int set_neighbour_op_result = _server_working_set->snmp_manager->set_shelf_neighbour_presence();
	if(find_master_op_result || set_neighbour_op_result) {
		// ERROR: finding the master switch board or getting neighbour presence value
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'find_switch_board_master' or 'set_shelf_neighbour_presence' failed ! < find_master_op_result == %d, set_neighbour_op_result == %d >.  The APBM server will retry later",
				find_master_op_result, set_neighbour_op_result);

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_FIND_SWITCH_BOARD_MASTER,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_FIND_SWITCH_BOARD_MASTER operation to "
					"retry finding the master switch board: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP_FIND_SWITCH_BOARD_MASTER operation rescheduled successfully");
	} else
		// TODO: Inserire nel log le informazioni relative alla switch board master trovata
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Master status and neighbour presence found for switch boards in local magazine !");

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Updating all switch boards states...");
	int call_result = _server_working_set->snmp_manager->update_my_switch_board_states(&timeout_ms);
	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'update_my_switch_board_states' failed: cannot update states of my switch boards: call_result == %d", call_result);
	}

	call_result = _server_working_set->snmp_manager->update_other_switch_board_states(&timeout_ms);
	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"Call 'update_other_switch_board_states' failed: cannot update states of other switch boards: call_result == %d", call_result);
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "All switch boards states updated");

	int environment = 0;
        _server_working_set->cs_reader->get_environment(environment);

	if(environment == ACS_CS_API_CommonBasedArchitecture::SCX)
	{ 
		call_result = _server_working_set->snmp_manager->set_warm_restart_on_all_scx();
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'set_warm_restart_on_all_scx' failed: cannot set next restart type to WARM: call_result == %d", call_result);
		}
		ACS_APBM_LOG(LOG_LEVEL_INFO, "NEX RESTART TYPE set to WARM for all SCX");
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_hwi_handler_open () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;


	//TODO to be delete
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "=====================================================================");
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "=====================================================================");

	int resend_operation = 1; // Pessimistic assumption
	//_server_working_set->snmp_manager->configure_front_port_scx_lag(); // Anil remove this from here just to test
	//TODO To be remove !!
	_server_working_set->hwi_handler->createIMMClasses();

	do {
		/*
		 * BEGIN: IMM activation phase
		 */

		if (const int call_result = _server_working_set->hwi_handler->openHwiHandler()) { // ERROR: cannot install the HWI configuration object implementer
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'openHwiHandler' failed: call_result == %d", call_result);
			break;
		}
		/*
		 * END: IMM activation phase
		 */

		resend_operation = 0;
	} while (0);

	// Should I send the same operation?
	if (resend_operation) {
		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_OPEN,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_HWI_HANDLER_OPEN operation to "
					"retry open the hardware inventory SNMP manager: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_HWI_HANDLER_OPEN operation rescheduled successfully");

		return 0;
	}

	//TODO To be delete !!!!!!!!!!!!!!!
	_server_working_set->hwi_handler->createHWIRootClass();


	std::vector <acs_apbm_switchboardinfo> my_switch_boards;
	std::vector <acs_apbm_switchboardinfo> other_switch_boards;
	int call_result_my = 0;
	int call_result_other = 0;

	resend_operation = 1; // Pessimistic assumption

	do {
		// Retrieving switch board info in all magazines
		(call_result_my = _server_working_set->cs_reader->get_my_switch_boards_info(my_switch_boards)) ||
			(call_result_other = _server_working_set->cs_reader->get_other_switch_boards_info(other_switch_boards));

		if (call_result_my || call_result_other) { // ERROR: loading switch board info from CS
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call '%s' failed: cannot get switch board info: call_result_my == %d, call_result_other == %d",
					(call_result_my ? "get_my_switch_boards_info" : "get_other_switch_boards_info"), call_result_my, call_result_other);
			break;
		}

		// Data debug logging for switch boards in my magazine
		if (my_switch_boards.size() <= 0) // WARNING: no switch board found in my magazine into the CS. Only logging
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch board found in my magazine from the CS configuration repository");
		else {
			std::sort(my_switch_boards.begin(), my_switch_boards.end(), fx_compare_magazine_slot_ascending);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "In my magazine there are %zu switch board(s). Info follows:", my_switch_boards.size());
			for ( std::vector<acs_apbm_switchboardinfo>::const_iterator cit = my_switch_boards.begin(); cit != my_switch_boards.end(); ++cit)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  Magazine == 0x%08X, Slot == %d, IPN-A == '%s', IPN-B == '%s'", cit->magazine, cit->slot_position, cit->ipna_str, cit->ipnb_str);
		}

		// Data debug logging for switch boards in other magazines
		if (other_switch_boards.size() <= 0) // WARNING: no switch board found in other magazines into the CS. Only logging
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch board found in other magazines from the CS configuration repository");
		else {
			std::sort(other_switch_boards.begin(), other_switch_boards.end(), fx_compare_magazine_slot_ascending);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "In other magazines there are %zu switch board(s). Info follows:", other_switch_boards.size());
			for ( std::vector<acs_apbm_switchboardinfo>::const_iterator cit = other_switch_boards.begin(); cit != other_switch_boards.end(); ++cit)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  Magazine == 0x%08X, Slot == %d, IPN-A == '%s', IPN-B == '%s'", cit->magazine, cit->slot_position, cit->ipna_str, cit->ipnb_str);
		}

		if ((my_switch_boards.size() + other_switch_boards.size()) <= 0) { // Very strange no switch board found
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"No switch boards found in my magazine neither in other magazines: this is very strange and should always not arise: "
					"APBM server cannot manages the hardware inventory");
			break;
		}

		// Adding magazines to the HWI handler object
		typedef std::multimap<uint32_t, std::vector<acs_apbm_switchboardinfo>::const_iterator> switch_boards_multimap_t;
		typedef std::set<switch_boards_multimap_t::key_type> magazine_set_t;
		switch_boards_multimap_t switch_boards_map;
		magazine_set_t magazine_set;

		for (std::vector<acs_apbm_switchboardinfo>::const_iterator cit = my_switch_boards.begin(); cit != my_switch_boards.end(); ++cit) {
			switch_boards_map.insert(switch_boards_multimap_t::value_type(cit->magazine, cit));
			magazine_set.insert(cit->magazine);
		}

		for (std::vector<acs_apbm_switchboardinfo>::const_iterator cit = other_switch_boards.begin(); cit != other_switch_boards.end(); ++cit) {
			switch_boards_map.insert(switch_boards_multimap_t::value_type(cit->magazine, cit));
			magazine_set.insert(cit->magazine);
		}

		// BEGIN: For debugging purposes only
		ACS_APBM_LOG(LOG_LEVEL_TRACE, "Internal multimap dump:");
		for (switch_boards_multimap_t::const_iterator cit = switch_boards_map.begin(); cit != switch_boards_map.end(); ++cit) {
			switch_boards_multimap_t::key_type magazine = cit->first;
			switch_boards_multimap_t::mapped_type sb_it = cit->second;

			ACS_APBM_LOG(LOG_LEVEL_TRACE, "  [KEY Magazine] 0x%08X: [ITEM SB interator]: magazine == 0x%08X, slot pos == %d, IPNA == '%s', IPNB == '%s'",
					magazine, sb_it->magazine, sb_it->slot_position, sb_it->ipna_str, sb_it->ipnb_str);
		}
		// END: For debugging purposes only

		char magazine_addr_str [16] = {0};

		for (magazine_set_t::const_iterator mag_cit = magazine_set.begin(); mag_cit != magazine_set.end(); ++mag_cit) {
			switch_boards_multimap_t::key_type magazine = *mag_cit;
			acs_apbm_csreader::uint32_to_ip_format(magazine_addr_str, magazine, acs_apbm_programconfiguration::magazine_address_imm_separator);

			// BEGIN: For debugging purposes only
			ACS_APBM_LOG(LOG_LEVEL_TRACE, "Internal multimap dump: querying map on magazine 0x%08X", magazine);
			// END: For debugging purposes only

			::std::pair<switch_boards_multimap_t::const_iterator, switch_boards_multimap_t::const_iterator> group_pair
			 	 = switch_boards_map.equal_range(magazine);

			// Check if empty range
			if (group_pair.second == group_pair.first) { // ERROR: Yes the range is empty
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"ERROR_ASSERTION{group_pair.second == group_pair.first}: equal_range is empty: no switch board found in "
						"the magazine '%s' (0x%08X)", magazine_addr_str, magazine);
			} else {
				const char * sb_0_ipna = 0;
				const char * sb_0_ipnb = 0;
				const char * sb_25_ipna = 0;
				const char * sb_25_ipnb = 0;

				for (switch_boards_multimap_t::const_iterator cit = group_pair.first; cit != group_pair.second; ++cit) {
					// BEGIN: For debugging purposes only
					ACS_APBM_LOG(LOG_LEVEL_TRACE, "QUERY ITEM BEFORE: SB iterator: magazine == 0x%08X, slot pos == %d, IPNA == '%s', IPNB == '%s'",
							cit->second->magazine, cit->second->slot_position, cit->second->ipna_str, cit->second->ipnb_str);
					// END: For debugging purposes only

					if (cit->second->slot_position) { sb_25_ipna = cit->second->ipna_str; sb_25_ipnb = cit->second->ipnb_str; }
					else { sb_0_ipna = cit->second->ipna_str; sb_0_ipnb = cit->second->ipnb_str; }
					// BEGIN: For debugging purposes only
					ACS_APBM_LOG(LOG_LEVEL_TRACE, "QUERY ITEM AFTER: sb_0_ipna == '%s', sb_0_ipnb == '%s', sb_25_ipna == '%s', sb_25_ipnb == '%s'",
							sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
					// END: For debugging purposes only
				}

				if (sb_0_ipna && sb_0_ipnb && sb_25_ipna && sb_25_ipnb) { // OK
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Adding to the HWI handler the magazine '%s' (0x%08X) with these switch board addresses: "
							"sb_0_ipna == '%s', sb_0_ipnb == '%s', sb_25_ipna == '%s', sb_25_ipnb == '%s'", magazine_addr_str, magazine,
							sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
					_server_working_set->hwi_handler->addMagazine(magazine_addr_str, sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
				} else { // ERROR: some switch board IPN addresses are empty
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Some switch board IPN addresses are empty for the magazine '%s' (0x%08X): "
							"sb_0_ipna == '%s', sb_0_ipnb == '%s', sb_25_ipna == '%s', sb_25_ipnb == '%s'", magazine_addr_str, magazine,
							sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
				}
			}
		}

		if (const int call_result = _server_working_set->hwi_handler->startCollection()) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'startCollection' failed: call_result == %d", call_result);
			break;
		}

		if (const int call_result = _server_working_set->hwi_handler->storeAllData())
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'storeAllData' failed: call_result == %d", call_result);
			break;
		}

		resend_operation = 0; // All right
	} while (0);

	// Should I send the same operation?
	if (resend_operation) {
		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_OPEN,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_HWI_HANDLER_OPEN operation to "
					"retry open the hardware inventory SNMP manager: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_HWI_HANDLER_OPEN operation rescheduled successfully");
	}

	//TODO to be delete
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "=====================================================================");
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "=====================================================================");

	return 0;
}
int __CLASS_NAME__::op_snmp_hwi_handler_reload (){

	std::vector <acs_apbm_switchboardinfo> my_switch_boards;
	std::vector <acs_apbm_switchboardinfo> other_switch_boards;
	int call_result_my = 0;
	int call_result_other = 0;

	int resend_operation = 1; // Pessimistic assumption


	//Data will not delete from IMM !!
		if(_server_working_set->hwi_handler->removeAllMagazine() != 0)
		{
			// Reschedule the same operation after a default timeout
			if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_RELOAD_DATA,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'schedule_operation' failed: cannot reschedule an SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_RELOAD operation to "
						"retry open the hardware inventory SNMP manager: call_result == %d", call_result);
			} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_RELOAD operation rescheduled successfully");
			return 0;
		}


	do {
		// Retrieving switch board info in all magazines
		(call_result_my = _server_working_set->cs_reader->get_my_switch_boards_info(my_switch_boards)) ||
			(call_result_other = _server_working_set->cs_reader->get_other_switch_boards_info(other_switch_boards));

		if (call_result_my || call_result_other) { // ERROR: loading switch board info from CS
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call '%s' failed: cannot get switch board info: call_result_my == %d, call_result_other == %d",
					(call_result_my ? "get_my_switch_boards_info" : "get_other_switch_boards_info"), call_result_my, call_result_other);
			break;
		}

		// Data debug logging for switch boards in my magazine
		if (my_switch_boards.size() <= 0) // WARNING: no switch board found in my magazine into the CS. Only logging
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch board found in my magazine from the CS configuration repository");
		else {
			std::sort(my_switch_boards.begin(), my_switch_boards.end(), fx_compare_magazine_slot_ascending);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "In my magazine there are %zu switch board(s). Info follows:", my_switch_boards.size());
			for ( std::vector<acs_apbm_switchboardinfo>::const_iterator cit = my_switch_boards.begin(); cit != my_switch_boards.end(); ++cit)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  Magazine == 0x%08X, Slot == %d, IPN-A == '%s', IPN-B == '%s'", cit->magazine, cit->slot_position, cit->ipna_str, cit->ipnb_str);
		}

		// Data debug logging for switch boards in other magazines
		if (other_switch_boards.size() <= 0) // WARNING: no switch board found in other magazines into the CS. Only logging
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch board found in other magazines from the CS configuration repository");
		else {
			std::sort(other_switch_boards.begin(), other_switch_boards.end(), fx_compare_magazine_slot_ascending);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "In other magazines there are %zu switch board(s). Info follows:", other_switch_boards.size());
			for ( std::vector<acs_apbm_switchboardinfo>::const_iterator cit = other_switch_boards.begin(); cit != other_switch_boards.end(); ++cit)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  Magazine == 0x%08X, Slot == %d, IPN-A == '%s', IPN-B == '%s'", cit->magazine, cit->slot_position, cit->ipna_str, cit->ipnb_str);
		}

		if ((my_switch_boards.size() + other_switch_boards.size()) <= 0) { // Very strange no switch board found
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"No switch boards found in my magazine neither in other magazines: this is very strange and should always not arise: "
					"APBM server cannot manages the hardware inventory");
			break;
		}

		// Adding magazines to the HWI handler object
		typedef std::multimap<uint32_t, std::vector<acs_apbm_switchboardinfo>::const_iterator> switch_boards_multimap_t;
		typedef std::set<switch_boards_multimap_t::key_type> magazine_set_t;
		switch_boards_multimap_t switch_boards_map;
		magazine_set_t magazine_set;

		for (std::vector<acs_apbm_switchboardinfo>::const_iterator cit = my_switch_boards.begin(); cit != my_switch_boards.end(); ++cit) {
			switch_boards_map.insert(switch_boards_multimap_t::value_type(cit->magazine, cit));
			magazine_set.insert(cit->magazine);
		}

		for (std::vector<acs_apbm_switchboardinfo>::const_iterator cit = other_switch_boards.begin(); cit != other_switch_boards.end(); ++cit) {
			switch_boards_map.insert(switch_boards_multimap_t::value_type(cit->magazine, cit));
			magazine_set.insert(cit->magazine);
		}

		// BEGIN: For debugging purposes only
		ACS_APBM_LOG(LOG_LEVEL_TRACE, "Internal multimap dump:");
		for (switch_boards_multimap_t::const_iterator cit = switch_boards_map.begin(); cit != switch_boards_map.end(); ++cit) {
			switch_boards_multimap_t::key_type magazine = cit->first;
			switch_boards_multimap_t::mapped_type sb_it = cit->second;

			ACS_APBM_LOG(LOG_LEVEL_TRACE, "  [KEY Magazine] 0x%08X: [ITEM SB interator]: magazine == 0x%08X, slot pos == %d, IPNA == '%s', IPNB == '%s'",
					magazine, sb_it->magazine, sb_it->slot_position, sb_it->ipna_str, sb_it->ipnb_str);
		}
		// END: For debugging purposes only

		char magazine_addr_str [16] = {0};

		for (magazine_set_t::const_iterator mag_cit = magazine_set.begin(); mag_cit != magazine_set.end(); ++mag_cit) {
			switch_boards_multimap_t::key_type magazine = *mag_cit;
			acs_apbm_csreader::uint32_to_ip_format(magazine_addr_str, magazine, acs_apbm_programconfiguration::magazine_address_imm_separator);

			// BEGIN: For debugging purposes only
			ACS_APBM_LOG(LOG_LEVEL_TRACE, "Internal multimap dump: querying map on magazine 0x%08X", magazine);
			// END: For debugging purposes only

			::std::pair<switch_boards_multimap_t::const_iterator, switch_boards_multimap_t::const_iterator> group_pair
			 	 = switch_boards_map.equal_range(magazine);

			// Check if empty range
			if (group_pair.second == group_pair.first) { // ERROR: Yes the range is empty
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"ERROR_ASSERTION{group_pair.second == group_pair.first}: equal_range is empty: no switch board found in "
						"the magazine '%s' (0x%08X)", magazine_addr_str, magazine);
			} else {
				const char * sb_0_ipna = 0;
				const char * sb_0_ipnb = 0;
				const char * sb_25_ipna = 0;
				const char * sb_25_ipnb = 0;

				for (switch_boards_multimap_t::const_iterator cit = group_pair.first; cit != group_pair.second; ++cit) {
					// BEGIN: For debugging purposes only
					ACS_APBM_LOG(LOG_LEVEL_TRACE, "QUERY ITEM BEFORE: SB iterator: magazine == 0x%08X, slot pos == %d, IPNA == '%s', IPNB == '%s'",
							cit->second->magazine, cit->second->slot_position, cit->second->ipna_str, cit->second->ipnb_str);
					// END: For debugging purposes only

					if (cit->second->slot_position) { sb_25_ipna = cit->second->ipna_str; sb_25_ipnb = cit->second->ipnb_str; }
					else { sb_0_ipna = cit->second->ipna_str; sb_0_ipnb = cit->second->ipnb_str; }
					// BEGIN: For debugging purposes only
					ACS_APBM_LOG(LOG_LEVEL_TRACE, "QUERY ITEM AFTER: sb_0_ipna == '%s', sb_0_ipnb == '%s', sb_25_ipna == '%s', sb_25_ipnb == '%s'",
							sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
					// END: For debugging purposes only
				}

				if (sb_0_ipna && sb_0_ipnb && sb_25_ipna && sb_25_ipnb) { // OK
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Adding to the HWI handler the magazine '%s' (0x%08X) with these switch board addresses: "
							"sb_0_ipna == '%s', sb_0_ipnb == '%s', sb_25_ipna == '%s', sb_25_ipnb == '%s'", magazine_addr_str, magazine,
							sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
					_server_working_set->hwi_handler->addMagazine(magazine_addr_str, sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
				} else { // ERROR: some switch board IPN addresses are empty
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Some switch board IPN addresses are empty for the magazine '%s' (0x%08X): "
							"sb_0_ipna == '%s', sb_0_ipnb == '%s', sb_25_ipna == '%s', sb_25_ipnb == '%s'", magazine_addr_str, magazine,
							sb_0_ipna, sb_0_ipnb, sb_25_ipna, sb_25_ipnb);
				}
			}
		}

		if (const int call_result = _server_working_set->hwi_handler->startCollection()) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'startCollection' failed: call_result == %d", call_result);
			break;
		}

		if (const int call_result = _server_working_set->hwi_handler->storeAllData()){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'storeAllData' failed: call_result == %d", call_result);
			break;
		}

		resend_operation = 0; // All right
	} while (0);

	// Should I send the same operation?
	if (resend_operation) {
		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_RELOAD_DATA,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_RELOAD operation to "
					"retry open the hardware inventory SNMP manager: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_RELOAD operation rescheduled successfully");
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_hwi_handler_close () {

	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
		(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
		(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
		(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	//Remove all magazine from internal structure.
	//Data will not delete from IMM !!
	_server_working_set->hwi_handler->removeAllMagazine();

	//Remove all implementer from IMM Classes
	if (const int call_result = _server_working_set->hwi_handler->closeHwiHandler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'closeHwiHandler' failed: call_result == %d", call_result);
	}

	return 0;
}

int __CLASS_NAME__::op_start_monitoring_service () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) &&
	   (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE))
	  return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Opening the Monitoring Service object ...");

	if (int call_result = _server_working_set->monitoring_service_event_handler->open()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open' failed: cannot open Monitoring Service! The APBM server will retry later: call_result == %d", call_result);

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_START_MONITORING_SERVICE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a START_MONITORING_SERVICE operation "
					"to retry opening Monitoring Service: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "START_MONITORING_SERVICE operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "Monitoring Service object opened successfully!");

	return 0;
}

int __CLASS_NAME__::op_stop_monitoring_service () {
	// Check if we are in the right node state to proceed
	if (/*(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE)*/1 &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Closing the Monitoring Service object ...");

	if (int call_result = _server_working_set->monitoring_service_event_handler->close()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open' failed: cannot close Monitoring Service! The APBM server will retry later: call_result == %d", call_result);

		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_STOP_MONITORING_SERVICE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a STOP_MONITORING_SERVICE operation to "
					"retry closing Monitoring Service: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "STOP_MONITORING_SERVICE operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "Monitoring Service object closed successfully!");

	return 0;
}

int __CLASS_NAME__::op_snmp_trapds_subscribe () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP Trap Dispatcher Interface: Subscribing to the SNMP trap dispatcher service...");

	int call_result = 0;

	if ((call_result = _server_working_set->snmp_trapds_observer->subscribe(acs_apbm_programconfiguration::trapds_subscription_service_name))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'subscribe' failed: cannot subscribe APBM server to the trap dispatcher service: call_result == %d", call_result);

		// Rescheduling the same operation
		if ((call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAPDS_SUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAPDS_SUBSCRIBE operation to "
					"retry subscribing APBM server to trap dispatcher service: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAPDS_SUBSCRIBE operation rescheduled successfully");
	}
	else ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP Trap Dispatcher Interface: APBM server successfully subscribed to the SNMP trap dispatcher service");

	return 0;
}

int __CLASS_NAME__::op_snmp_trapds_unsubscribe () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	// Unsubscribing APBM server from the SNMP trap dispatcher service
	ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP Trap Dispatcher Interface: Unsubscribing APBM server from the SNMP trap dispatcher service...");

	int call_result = 0;

		if ((call_result = _server_working_set->snmp_trapds_observer->unsubscribe())) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'unsubscribe' failed: cannot unsubscribe APBM server from the trap dispatcher service: call_result == %d", call_result);

			// Rescheduling the same operation
			if ((call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAPDS_UNSUBSCRIBE,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAPDS_UNSUBSCRIBE operation to "
						"retry subscribing APBM server to trap dispatcher service: call_result == %d", call_result);
			} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAPDS_UNSUBSCRIBE operation rescheduled successfully");

		}ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP Trap Dispatcher Interface: APBM server successfully unsubscribed from the SNMP trap dispatcher service");

	return 0;
}

int __CLASS_NAME__::op_cs_hwc_table_change_observer_start () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CS HWC table changes observer: starting observing for HWC table change notification from CS interface...");

	int call_result = _server_working_set->cs_hwc_table_change_observer->start();

	if (call_result) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'start' failed: cannot start the observer to receive notifications from CS for HWC table change notifications");

		// Rescheduling the same operation
		call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_CS_HWC_TABLE_CHANGE_OBSERVER_START,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait);

		if (call_result) ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a CS_HWC_TABLE_CHANGE_OBSERVER_START operation to "
					"retry subscribing APBM server to receive CS HWC table change notifications: call_result == %d", call_result);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "CS_HWC_TABLE_CHANGE_OBSERVER_START operation rescheduled successfully");
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "CS HWC table changes observer: observer started successfully"); // OK

	return 0;
}

int __CLASS_NAME__::op_cs_hwc_table_change_observer_stop () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	// Unsubscribing APBM server from receiving CS HWC table change notifications
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CS HWC table changes observer: Unsubscribing APBM server from receiving CS HWC table change notifications...");

	int call_result = _server_working_set->cs_hwc_table_change_observer->stop();

	if (call_result) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'stop' failed: cannot stop the observer to receive notifications from CS for HWC table change notifications");

		// Rescheduling the same operation
		call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_CS_HWC_TABLE_CHANGE_OBSERVER_STOP,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait);

		if (call_result) ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a CS_HWC_TABLE_CHANGE_OBSERVER_STOP operation to "
					"retry unsubscribing APBM server from receiving CS HWC table change notifications: call_result == %d", call_result);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "CS_HWC_TABLE_CHANGE_OBSERVER_STOP operation rescheduled successfully");
	} else // OK
		ACS_APBM_LOG(LOG_LEVEL_INFO, "CS HWC table changes observer: APBM server successfully unsubscribed from receiving CS HWC table change notifications");

	return 0;
}

int __CLASS_NAME__::op_work_undefined_node_state_check () {
	// Check if we are in the right program state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) return 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server is working in node-state-undefined mode: checking my node state: retry counter == %u", ++_work_undefined_node_state_check_retry_count);

	int call_result = 0;

	// Retrieving my node state
	acs_apbm::NodeStateConstants node_state = acs_apbm::NODE_STATE_UNDEFINED;
	if ((call_result = _server_working_set->get_node_state(node_state, acs_apbm_programconfiguration::get_node_state_retries)))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_node_state' failed: retrieving my node state: call_result == %d", call_result);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "My node state is '%s'", _server_working_set->node_state_str(node_state));

	// Check the program state again
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"APBM server has changed its program state unexpectedly: new program state == %d: this is right: stopping checking my node state",
				_server_working_set->program_state);
		return 0;
	}

	if (node_state == acs_apbm::NODE_STATE_UNDEFINED) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "My AP node is still in undefined state: rescheduling a new node state check...");

		// Rescheduling the same operation
		call_result =
				_server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_WORK_UNDEFINED_NODE_STATE_CHECK, 3);

		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule a WORK_UNDEFINED_NODE_STATE_CHECK operation to recheck my node state: call_result == %d",
					call_result);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "WORK_UNDEFINED_NODE_STATE_CHECK operation rescheduled successfully");
	} else {
		// Check the program state again
		if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"APBM server has changed its program state unexpectedly: new program state == %d: this is right: stopping checking my node state",
					_server_working_set->program_state);
			return 0;
		}

		// Dispatch a change working operation signal depending on the node state.
		acs_apbm::ServerWorkOperationCodeConstants op = ((node_state == acs_apbm::NODE_STATE_ACTIVE)
				? acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_ACTIVE
				: acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_PASSIVE);

		if ((call_result = _server_working_set->operation_send(op))) {
			ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "Call 'operation_send' failed: trying to send the operation code '%d' to activate the working "
					"job depending upon the node state '%s': call_result == %d", op, _server_working_set->node_state_str(node_state), call_result);
		}
	}

	return 0;
}

int __CLASS_NAME__::op_shelf_mngctrl_trap_counter_check () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	//operation needed only in not cba architecture
	if (_server_working_set->cs_reader->is_cba_system())
			return 0;
	// Check the shelf manager control trap counter into the SNMP manager
	if (_server_working_set->snmp_manager->shelf_manager_control_left_trap_counter() && _server_working_set->snmp_manager->shelf_manager_control_right_trap_counter()) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server has already received %u shelf-manager-control trap(s), %u by left switch board and %u by right: this check is dropped for ever "
				"because the subscription startup phase was passed",
				_server_working_set->snmp_manager->shelf_manager_control_trap_counter(),
				_server_working_set->snmp_manager->shelf_manager_control_left_trap_counter(),
				_server_working_set->snmp_manager->shelf_manager_control_right_trap_counter());
		return 0;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Invalid number of traps received - %u by left switch board and %u by right board",
																_server_working_set->snmp_manager->shelf_manager_control_left_trap_counter(),
																_server_working_set->snmp_manager->shelf_manager_control_right_trap_counter());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reordering an SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to retry the SNMP agent subscription");

	// Order an 'unsubscribe-subscribe process' operation to unsubscribe and then subscribe to receive SNMP trap messages
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to do the unsubscribe-subscribe process");
	else
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation successfully reordered");

/* The following will be done into the right operation context
	// Rescheduling the check again
	const int call_result =
			_server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SHELF_MNGCTRL_TRAP_COUNTER_CHECK, 20);

	if (call_result)
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'schedule_operation' failed: cannot reschedule a SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation: call_result == %d",
				call_result);
	else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SHELF_MNGCTRL_TRAP_COUNTER_CHECK operation rescheduled successfully");
*/

	return 0;
}

int __CLASS_NAME__::op_snmp_magazine_trap_subscribe (uint32_t magazine) {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to subscribe to SNMP switch board agents within the magazine 0x%08X for receiving SNMP trap messages...",
			magazine);

	if (const int call_result = _server_working_set->snmp_manager->send_trap_subscription_request(magazine)) {
		// Check call_result
		if (call_result == acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch boards found in the magazine 0x%08X: no trap subscription will be requested", magazine);
		} else {

			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'send_trap_subscription_request' failed for the magazine 0x%08X: cannot subscribe to receive SNMP trap messages", magazine);

			// TODO: Add operation parameter handling in the pipe scheduler also. Once done, uncomment the following lines and
			// change them to use the new parameter feature

			// In the meantime we reschedule only in case of SCX environment, where state attrbute is used to recognize if the board is subscribed.


			// Reschedule the same operation after a default timeout
			if (_server_working_set->cs_reader->is_cba_system())
			{
				if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
						acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE,
						acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_SUBSCRIBE operation to retry "
							"the subscription process: call_result == %d", call_result);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_SUBSCRIBE operation rescheduled successfully");
			}

		}
	} else {
		ACS_APBM_LOG(LOG_LEVEL_INFO,
				"APBM server successfully subscribed to SNMP switch board agents within the magazine 0x%08X to receive SNMP trap messages", magazine);
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_magazine_trap_unsubscribe (uint32_t magazine) {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"Trying to unsubscribe receiving SNMP trap messages from SNMP switch board agents within the magazine 0x%08X...", magazine);

	if (const int call_result = _server_working_set->snmp_manager->send_trap_unsubscription_request(magazine)) {
		// Check call_result
		if (call_result == acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch boards found in the magazine 0x%08X: no trap unsubscription will be requested", magazine);
		} else {

			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'send_trap_unsubscription_request' failed for the magazine 0x%08X: cannot unsubscribe from receiving SNMP trap messages", magazine);

			// TODO: Add operation parameter handling in the pipe scheduler also. Once done, uncomment the following lines and
			// change them to use the new parameter feature

			if (_server_working_set->cs_reader->is_cba_system())
			{
				// Reschedule the same operation after a default timeout
				if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
						acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE,
						acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE operation to retry "
							"the unsubscription process: call_result == %d", call_result);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE operation rescheduled successfully");
			}


		}
	} else ACS_APBM_LOG(LOG_LEVEL_INFO,
			"APBM server successfully unsubscribed receiving SNMP trap messages from SNMP switch board agents within the magazine 0x%08X", magazine);

	return 0;
}

int __CLASS_NAME__::op_snmp_magazine_trap_unsubscribe_subscribe (uint32_t magazine) {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"Trying to unsubscribe receiving SNMP trap messages from switch board agents within the magazine 0x%08X before subscribe...", magazine);

	int dont_subscribe = 0;

	if (const int call_result = _server_working_set->snmp_manager->send_trap_unsubscription_request(magazine)) {
		//Check call_result
		if (call_result == acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch boards found in the magazine 0x%08X: no trap subscription will be requested", magazine);
			dont_subscribe = 1;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'send_trap_unsubscription_request' failed for the magazine 0x%08X: cannot unsubscribe from receiving SNMP trap messages", magazine);

			// TODO: Add operation parameter handling in the pipe scheduler also. Once done, uncomment the following lines and
			// change them to use the new parameter feature


			if (_server_working_set->cs_reader->is_cba_system())
			{
				// Reschedule the same operation after a default timeout
				if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
						acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE,
						acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to "
							"retry the unsubscribe-subscribe process: call_result == %d", call_result);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation rescheduled successfully");
			}

			return 0;
		}
	} else ACS_APBM_LOG(LOG_LEVEL_INFO,
			"APBM server successfully unsubscribed receiving SNMP trap messages from SNMP switch board agents into the magazine 0x%08X", magazine);

	if (dont_subscribe) return 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to subscribe receiving SNMP trap messages from SNMP switch board agents within the magazine 0x%08X...",
			magazine);

	if (const int call_result = _server_working_set->snmp_manager->send_trap_subscription_request(magazine)) { // ERROR
		// Check call_result
		if (call_result == acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND)
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch boards found in the magazine 0x%08X: no trap subscription request sent", magazine);
		else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'send_trap_subscription_request' failed for the magazine 0x%08X: cannot subscribe to receive SNMP trap messages", magazine);

			// TODO: Add operation parameter handling in the pipe scheduler also. Once done, uncomment the following lines and
			// change them to use the new parameter feature


			if (_server_working_set->cs_reader->is_cba_system())
			{
				// Reschedule the same operation after a default timeout
				if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
						acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE,
						acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_SUBSCRIBE operation "
							"to retry the subscribe process: call_result == %d", call_result);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_SUBSCRIBE operation rescheduled successfully");
			}

		}
	} else { // OK
		ACS_APBM_LOG(LOG_LEVEL_INFO,
				"APBM server successfully subscribed receiving SNMP trap messages from SNMP switch board agents within the magazine 0x%08X", magazine);
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_other_magazines_trap_subscribe () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	std::vector <uint32_t> magazines;

	int call_result = _server_working_set->cs_reader->get_other_magazines(magazines);

	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_other_magazines' failed: call_result == %d", call_result);

		// Reschedule the same operation after a default timeout
		if ((call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE operation to "
					"retry the subscription process for all other magazines: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE operation rescheduled successfully");

		return 0;
	}

	// Check the other magazines count
	if (magazines.size() <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No other magazines found in the system: no trap subscriptions will be done");
		return 0;
	}

	// For each magazine a SUBSCRIPTION operation will be scheduled
	for (size_t i = 0; i < magazines.size(); ++i) {
		uint32_t magazine = magazines[i];

		call_result = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_MAGAZINE_TRAP_SUBSCRIBE, magazine);
		if (call_result) { // ERROR: ordering the operation
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'operation_send' failed: cannot order an SNMP_MAGAZINE_TRAP_SUBSCRIBE operation on the magazine 0x%08X: call_result == %d",
					magazine, call_result);
		}
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_other_magazines_trap_unsubscribe () {
	// Check if we are in the right node state to proceed
	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
			(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

	std::vector <uint32_t> magazines;

	int call_result = _server_working_set->cs_reader->get_other_magazines(magazines);

	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_other_magazines' failed: call_result == %d", call_result);

		// Reschedule the same operation after a default timeout
		if ((call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE operation to "
					"retry the unsubscription process for all other magazines: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE operation rescheduled successfully");

		return 0;
	}

	// Check the other magazines count
	if (magazines.size() <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No other magazines found in the system: no trap unsubscriptions will be done");
		return 0;
	}

	// For each magazine an UNSUBSCRIPTION operation will be scheduled
	for (size_t i = 0; i < magazines.size(); ++i) {
		uint32_t magazine = magazines[i];

		call_result = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_MAGAZINE_TRAP_UNSUBSCRIBE, magazine);
		if (call_result) { // ERROR: ordering the operation
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'operation_send' failed: cannot order an SNMP_MAGAZINE_TRAP_UNSUBSCRIBE operation on the magazine 0x%08X: call_result == %d",
					magazine, call_result);
		}
	}

	return 0;
}

int __CLASS_NAME__::op_snmp_other_magazines_trap_unsubscribe_subscribe () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	std::vector <uint32_t> magazines;

	int call_result = _server_working_set->cs_reader->get_other_magazines(magazines);
	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_other_magazines' failed: call_result == %d", call_result);

		// Reschedule the same operation after a default timeout
		if ((call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'schedule_operation' failed: cannot reschedule an SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to "
					"retry the unsubscribe-subscribe process for all other magazines: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE operation rescheduled successfully");

		return 0;
	}

	// Check the other magazines count
	if (magazines.size() <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No other magazines found in the system: no trap subscriptions will be done");
		return 0;
	}

	// For each magazine an UNSUBSCRIPTION-SUBSCRIPTION operation will be scheduled
	for (size_t i = 0; i < magazines.size(); ++i) {
		uint32_t magazine = magazines[i];

		call_result = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_MAGAZINE_TRAP_UNSUBSCRIBE_SUBSCRIBE, magazine);
		if (call_result) { // ERROR: ordering the operation
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'operation_send' failed: cannot order an SNMP_MAGAZINE_TRAP_UNSUBSCRIBE_SUBSCRIBE operation on the magazine 0x%08X: call_result == %d",
					magazine, call_result);
		}
	}

	return 0;
}

int __CLASS_NAME__::op_update_board_status (int32_t slot, uint32_t magazine) {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;
	int retCode = 0;
	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"Updating board status by operation engine: slot == %d, magazine == 0x08X", slot, magazine);

	acs_apbm_apboardinfo board;
	int imm_call_result = 0;
	int call_result = -1;
	unsigned timeout = 500;

	_server_working_set->shelves_data_manager->get_ap_board_status(board.board_status, slot, magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Board in slot %d has status: '%d'", slot, board.board_status);

	if(board.board_status == axe_eth_shelf::BOARD_STATUS_WORKING ||
		 board.board_status == axe_eth_shelf::BOARD_STATUS_MISSING ||
		 board.board_status == axe_eth_shelf::BOARD_STATUS_UNDEFINED )
	{
			board.slot_position = slot;
			board.magazine = magazine;
			//On result of this snmp get, it will be decided the logic state of board
			call_result = _server_working_set->snmp_manager->get_board_data(board.product_number, board.product_revision_state,
					board.product_name, board.serial_number,
					board.manufacture_date, board.vendor, board.slot_position,
					board.magazine, &timeout);

			if( call_result == acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_RESOURCEUNAVAILABLE ||
					call_result == acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_NO_SUCH_INSTANCE ||
					call_result == acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_BAD_TYPE ||
					call_result == acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_AUTHORIZATIONERROR)
			{
					if (board.board_status == axe_eth_shelf::BOARD_STATUS_WORKING ||
							board.board_status == axe_eth_shelf::BOARD_STATUS_UNDEFINED) //UPDATE STATUS TO MISSING
					{
						board.board_status = axe_eth_shelf::BOARD_STATUS_MISSING;
						imm_call_result = _server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
						ACS_APBM_LOG(LOG_LEVEL_WARN,"Update Board to 'MISSING' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
						_server_working_set->shelves_data_manager->reset_ap_board_info(true, slot);
						_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
						retCode = -1;
					}
			}
			else if (call_result == acs_apbm_snmp::ERR_SNMP_NO_ERRORS && //get return 0
									(board.board_status == axe_eth_shelf::BOARD_STATUS_MISSING ||
									  board.board_status == axe_eth_shelf::BOARD_STATUS_UNDEFINED)) //UPDATE STATUS TO WORKING
			{
				board.board_status = axe_eth_shelf::BOARD_STATUS_WORKING;
				_server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
				ACS_APBM_LOG(LOG_LEVEL_WARN,"Board board to 'WORKING' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
				 _server_working_set->shelves_data_manager->update_ap_board_info(true, slot);
				 _server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
			}
			else if (call_result != acs_apbm_snmp::ERR_SNMP_NO_ERRORS)//impossible to retrive status...it will be set to undefined
			{
				board.board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
				_server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
				ACS_APBM_LOG(LOG_LEVEL_WARN," Impossible to get board status...it will be set to 'UNDEFINED' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
				_server_working_set->shelves_data_manager->reset_ap_board_info(true, slot);
				_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
			}

	}	//Other state are controlled by other operation

	return retCode;
}

int __CLASS_NAME__::stop_working () {
	// Save the current program state to decide what to do after
	acs_apbm::ProgramStateConstants program_state_save = _server_working_set->program_state;

	// Signal that I'm going to stop working
	_server_working_set->program_state = acs_apbm::PROGRAM_STATE_STOP_WORKING;

	int return_code = acs_apbm::ERR_NO_ERRORS;
	int environment = 0;
	switch (program_state_save) {
	case acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE:
		// order stopping the Monitoring Service
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_MONITORING_SERVICE))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a STOP_MONITORING_SERVICE operation");

		// Order a 'stop observing' operation to stop the CS HWC table change observing CS notification
		if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_CS_HWC_TABLE_CHANGE_OBSERVER_STOP))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a CS_HWC_TABLE_CHANGE_OBSERVER_STOP operation to unsubscribe the CS HWC table change observer from receive CS notifications");

		// Order an 'SNMP_HWI_HANDLER_CLOSE' operation to close the hardware inventory handler (only in CBA architecture)
		if (_server_working_set->cs_reader->is_cba_system()) {
			if ((return_code = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_IMM_HANDLER_CLOSE))) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order SNMP_HWI_HANDLER_CLOSE operation to close SCX LAG imm handler");
				break;
			}

			if ((return_code = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_HANDLER_CLOSE))) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a SNMP_HWI_HANDLER_CLOSE operation to close the hardware inventory handler");
				break;
			}

			// Order an 'unsubscribe process' operation to unsubscribe from all switch boards in all other magazines
			if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE))
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'operation_send' failed: cannot order a SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE operation to do the unsubscribe process for all switch boards in all other magazines");
		}

		_server_working_set->cs_reader->get_environment(environment);

		if (environment != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
		{
		// Order a 'SNMP_TRAP_UNSUBSCRIBE' operation to unsubscribe from receiving SNMP trap messages
		if ((return_code = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a SNMP_TRAP_UNSUBSCRIBE operation to unsubscribe from receiving SNMP trap messages");
			break;
		}

			// Order a 'SNMP_TRAPDS_UNSUBSCRIBE' operation to unsubscribe from the SNMP trap dispatcher service
			if ((return_code = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAPDS_UNSUBSCRIBE))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an SNMP_TRAPDS_UNSUBSCRIBE operation to unsubscribe from the SNMP trap dispatcher service");
			break;
			}
		}
		// Order a 'release IMM ownership' operation to release IMM runtime objects
		if ((return_code = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_IMM_RELEASE_OWNERSHIP))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an IMM_RELEASE_OWNERSHIP operation to release ownership of APBM runtime IMM objects");
			break;
		}

		// Order a 'close IMM management session' operation
		if ((return_code = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_IMM_CLOSE_MANAGEMENT_SESSION))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an IMM_CLOSE_MANAGEMENT_SESSION operation to close an IMM management session");
			break;
		}

		//TR HX65505
		// Order a cease all alarms operation as service is shutting down
		ACS_APBM_LOG(LOG_LEVEL_INFO, "stop_working() -> send cease all alarms operation to pipe");
		if ((return_code = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_CEASE_ALL_ALARMS_SHUTDOWN))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an SERVER_WORK_OPERATION_CEASE_ALL_ALARMS_SHUTDOWN operation to cease alarms");
			op_cease_all_alarms_shutdown();
		}

		break;
	case acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE:
          // order stopping the Monitoring Service
            if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_MONITORING_SERVICE))
                    ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order a STOP_MONITORING_SERVICE operation");

		break;
	case acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED:
		break;
	default: break;
	}

	// In case of errors reset the program state to the previous state
	return_code && (_server_working_set->program_state = program_state_save);

	return return_code;
}

int __CLASS_NAME__::op_testing_node_active_1 () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Testing_1 operation session started!");


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Getting my switch board info...");
	std::vector <acs_apbm_switchboardinfo> switch_boards;
	int call_result = _server_working_set->cs_reader->get_my_switch_boards_info(switch_boards);
	if (call_result) // ERROR: getting my switch board info
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_my_switch_boards_info' failed: call_result == %d", call_result);
	else { // OK: listing my switch board info
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "My switch board count == %zu", switch_boards.size());

		for (size_t i = 0; i < switch_boards.size(); ++i) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  %zu) magazine == 0x%08X, slot == %d, IPNA == '%s', IPNB == '%s'",
					i, switch_boards[i].magazine, switch_boards[i].slot_position, switch_boards[i].ipna_str, switch_boards[i].ipnb_str);
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Getting other switch board info...");
	switch_boards.clear();
	call_result = _server_working_set->cs_reader->get_other_switch_boards_info(switch_boards);

	if (call_result) // ERROR: getting other switch board info
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_other_switch_boards_info' failed: call_result == %d", call_result);
	else { // OK: listing other switch board info
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Other switch board count == %zu", switch_boards.size());

		for (size_t i = 0; i < switch_boards.size(); ++i) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  %zu) magazine == 0x%08X, slot == %d, IPNA == '%s', IPNB == '%s'",
					i, switch_boards[i].magazine, switch_boards[i].slot_position, switch_boards[i].ipna_str, switch_boards[i].ipnb_str);
		}
	}


//	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Loading board data information from CS");
//	int call_result = _server_working_set->shelves_data_manager->load_from_CS();
//	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'load_from_CS' failed: cannot read board data from CS");
//	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Configuration board data successfully read from CS");

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Testing_1 operation session ended!");

	return 0;
}
//  SCX LAG IMM handler operation begin from here
int __CLASS_NAME__::op_scx_lag_imm_handler_open () {
	// Check if we are in the right node state to proceed
	if (_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) return 0;

	int resend_operation = 1;
	uint32_t  myMagazine;
	char myMagazine_str [16] = {0};

	do {
	    if (const int call_result =_server_working_set->cs_reader->get_my_magazine(myMagazine))
	        break; //CNI 33/6-532

		if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(myMagazine_str, myMagazine)) // ERROR: while converting
			break;  //CNI 33/6-532

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANLDING: get_my_magazine collected magazine address,myMagazine:%s",myMagazine_str);
		/*
		 * BEGIN: IMM activation phase
		 */

		if (const int call_result = _server_working_set->imm_lag_handler->openScxLagHandler()) { // object implementer failed
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call openScxLagHandler failed: call_result == %d", call_result);
			break;
		}
		/*
		 * END: IMM activation phase
		 */

		resend_operation = 0;
	} while (0);

	if (resend_operation) {
		// Reschedule the same operation after a default timeout
		if (const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_IMM_HANDLER_OPEN,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call schedule_operation failed: cannot reschedule an SCX LAG IMM operation to retry open LAG IMM handler: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SCX_LAG_IMM_HANDLER_OPEN operation rescheduled successfully");

		return 0;
	}

	_server_working_set->imm_lag_handler->createLAGRootClass(myMagazine_str,acs_apbm::BOARD_SCXB_LEFT);
	_server_working_set->imm_lag_handler->createLAGRootClass(myMagazine_str,acs_apbm::BOARD_SCXB_RIGHT);
	(void)_server_working_set->monitoring_service_event_handler->set_scx_lag_monitoring_flags();

	return 0;
}
int __CLASS_NAME__::op_scx_lag_imm_handler_close () {

	if ((_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) &&
		(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED) &&
		(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED) &&
		(_server_working_set->program_state != acs_apbm::PROGRAM_STATE_STOP_WORKING)) return 0;

		//Remove scx lag implementer from IMM Classes
	if (const int call_result = _server_working_set->imm_lag_handler->closeScxLagHandler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call closeScxLagHandler failed: call_result == %d", call_result);
	}

	return 0;
}

int __CLASS_NAME__::op_cease_all_alarms_shutdown()			 //TR HX65505
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering op_cease_all_alarms_shutdown()");
	_server_working_set->alarmevent_handler->cease_all_alarms();
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering op_cease_all_alarms_shutdown()");
	return 0;
}
