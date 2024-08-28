#include "acs_apbm_action_timer.h"
#include "acs_apbm_immrepositoryhandler.h"
#include "acs_apbm_snmpmanager.h"
#include <acs_apbm_shelvesdatamanager.h>
#include <ace/Guard_T.h>
#include <ace/Recursive_Thread_Mutex.h>


acs_apbm_action_timer::acs_apbm_action_timer(uint32_t delay, uint32_t interval, int32_t slot_id, uint32_t magazine_id, ACS_APGCC_AdminOperationIdType operation_id, ACS_APGCC_InvocationType invocation_id, acs_apbm_serverworkingset * server_working_set)
: _delay(delay),
  _interval(interval),
  _slot_id(slot_id),
  _magazine_id(magazine_id),
  _operation_id(operation_id),
  _invocation_id(invocation_id),
  _server_working_set(server_working_set)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Created action timer: delay == %d, interval == %d, action_id ==  %llu, invocation_id == %llu, slot == %d, magazine == 0x%08X", delay, interval, operation_id, invocation_id, slot_id, magazine_id);
}


int acs_apbm_action_timer::handle_timeout ( const ACE_Time_Value &  /*current_time*/,  const void * /*act*/)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "periodic check for action completion... : action_id ==  %llu, invocation_id == %llu, slot == %d, magazine == %08X", _operation_id, _invocation_id, _slot_id, _magazine_id);
	int res = check_action();
	return ((res == CHECK_ACTION_COMPLETED) || (res == CHECK_ACTION_INTERNAL_ERROR) ? -1 : 0);
}


acs_apbm_action_timer::check_action_result acs_apbm_action_timer::check_action()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_check_action_mutex);

	int call_result = 0;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "checking action state (action_id == %llu,  slot == %d,  magazine == 0x%08X ) ... " , _operation_id, _slot_id, _magazine_id);
	switch(_operation_id)
	{
		case IMM_ACTION_APBOARD_IPMIUPGRADE_SNMP:
		{
			// get IPMI upgrade status
			unsigned timeout_ms = 2000;
			axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status;

			call_result = _server_working_set->snmp_manager->get_board_ipmi_upgrade_status(ipmi_upgrade_status, _slot_id, _magazine_id, & timeout_ms);
			if(call_result < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error getting 'ipmi_upgrade_status' attribute for AP board ( slot == %d  -  magazine == 0x%08X ). Call 'update_ipmiupgrade_status_on_ap_board_info(..)' returned %d" , _slot_id, _magazine_id, call_result);
				return CHECK_ACTION_GET_IPMI_STATUS_ERROR;
				// update <shelves_data_manager> object and IMM
				call_result =_server_working_set->shelves_data_manager->update_ap_board_ipmi_upgrade_status(axe_eth_shelf::IPMI_UPGRADE_STATUS_UNDEFINED, true,_slot_id, _magazine_id);
				if(call_result < 0)
				{
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error updating 'ipmi_upgrade_status' attribute for AP board ( slot == %d  -  magazine == 0x%08X ). Call 'acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_status(..)' returned %d" , _slot_id, _magazine_id, call_result);
					return CHECK_ACTION_UPDATE_IPMI_STATUS_ERROR;
				}
			}
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Got from SNMP Manager the 'ipmi_upgrade_status' attribute for the specified AP board (slot == %d  -  magazine == %08X). IPMI_UPGRADE_STATUS == %d", _slot_id, _magazine_id, ipmi_upgrade_status);

			// update <shelves_data_manager> object and IMM
			call_result =_server_working_set->shelves_data_manager->update_ap_board_ipmi_upgrade_status(ipmi_upgrade_status, true,_slot_id, _magazine_id);
			if(call_result < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error updating 'ipmi_upgrade_status' attribute for AP board ( slot == %d  -  magazine == 0x%08X ). Call 'acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_status(..)' returned %d" , _slot_id, _magazine_id, call_result);
				return CHECK_ACTION_UPDATE_IPMI_STATUS_ERROR;
			}
			ACS_APBM_LOG(LOG_LEVEL_INFO, "'ipmi_upgrade_status' attribute successfully updated for the specified AP board (slot == %d  -  magazine == 0x%08X). IPMI_UPGRADE_STATUS == %d", _slot_id, _magazine_id, ipmi_upgrade_status);

			// check if the IPMI upgrade action has terminated
			if(ipmi_upgrade_status != axe_eth_shelf::IPMI_UPGRADE_STATUS_ONGOING)
			{
				// get IPMI firmware data
				int16_t ipmi_firmware_type;
				char ipmi_product_number[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1];
				char ipmi_product_revision[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1];

				if (_slot_id != 25 && _slot_id != 0)
				{
					call_result = _server_working_set->snmp_manager->get_board_ipmi_data(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, ipmi_upgrade_status, _slot_id, _magazine_id);
					if(call_result < 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get from SNMP Manager the IPMI data for the specified AP board (slot == %d  -  magazine == 0x%08X)", _slot_id, _magazine_id);
						return CHECK_ACTION_GET_IPMI_DATA_ERROR;
					}
					ACS_APBM_LOG(LOG_LEVEL_INFO, "Got from SNMP Manager the IPMI data attributes for the specified AP board (slot == %d  -  magazine == 0x%08X)", _slot_id, _magazine_id);
				}
				// update <shelvesdatamanager> object and IMM
				if (_slot_id != acs_apbm::BOARD_SCB_LEFT && _slot_id != acs_apbm::BOARD_SCB_RIGHT)
				{
					call_result = _server_working_set->shelves_data_manager->update_ap_board_ipmi_data(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, true, _slot_id, _magazine_id);
					if(call_result < 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error updating IPMI data attributes for the specified AP board (slot == %d  -  magazine == 0x%08X)", _slot_id, _magazine_id);
						return CHECK_ACTION_UPDATE_IPMI_DATA_ERROR;
					}

					ACS_APBM_LOG(LOG_LEVEL_INFO, "IPMI data attributes successfully updated for the specified AP board (slot == %d  -  magazine == 0x%08X). IPMI_FIRMWARE_TYPE == %d, IPMI_PRODUCT_NUMBER == %s, IPMI_PRODUCT_REVISION == %s", _slot_id, _magazine_id, ipmi_firmware_type, ipmi_product_number, ipmi_product_revision);
					ACS_APBM_LOG(LOG_LEVEL_INFO, "the action (action_id == %llu,  slot == %d,  magazine == 0x%08X ) has been successfully completed !" , _operation_id, _slot_id, _magazine_id);
				}
				//update last update time
				if( ipmi_upgrade_status == axe_eth_shelf::IPMI_UPGRADE_STATUS_UNDEFINED ||
						ipmi_upgrade_status == axe_eth_shelf::IPMI_UPGRADE_STATUS_READY )
				{
					call_result = _server_working_set->shelves_data_manager->update_ap_board_ipmi_upgrade_last_time(true, _slot_id, _magazine_id);
					if(call_result < 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error updating IPMI last time attribute for the specified AP board (slot == %d  -  magazine == 0x%08X)", _slot_id, _magazine_id);
						return CHECK_ACTION_UPDATE_IPMI_DATA_ERROR;
					}
				}
				return CHECK_ACTION_COMPLETED;  // operation successfully completed !
			}
		}
		break;
	default:
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unexpected operation_id (%llu)", _operation_id);
		return CHECK_ACTION_INTERNAL_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "the action (action_id == %llu,  slot == %d,  magazine == 0x%08X ) is still pending ! " , _operation_id, _slot_id, _magazine_id);

	return CHECK_ACTION_PENDING;
}


int acs_apbm_action_timer::handle_close(ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Destroying action timer: action_id ==  %llu, invocation_id == %llu, slot == %d, magazine == 0x%08X", _operation_id, _invocation_id, _slot_id, _magazine_id);
	_server_working_set->imm_repository_handler->clear_asynch_action_timer(_slot_id, _magazine_id);
	delete this;
	return 0;
}


int acs_apbm_action_timer::activate()
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Activating action timer: action_id == %llu, invocation_id == %llu, slot == %d, magazine == 0x%08X", _operation_id, _invocation_id, _slot_id, _magazine_id);

	ACE_Time_Value delay(_delay);
	ACE_Time_Value interval(_interval);
	int call_result = _server_working_set->main_reactor->schedule_timer(this, 0, delay, interval);
	if(call_result != -1)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "action timer successfully activated. timer_id == %d", call_result);
	else {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "action timer activation failed ! Call 'ACE_Reactor::schedule_timer(..)' returned %d !", call_result);
	}

	return call_result;
}


int acs_apbm_action_timer::deactivate()
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Canceling action timer: action_id == %llu, invocation_id == %llu, slot == %d, magazine == 0x%08X", _operation_id, _invocation_id, _slot_id, _magazine_id);

	int n_cancelled = _server_working_set->main_reactor->cancel_timer(this, 0);
	if(n_cancelled == 1)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "action timer successfully canceled.");
	else {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "action timer NOT found ! This is not necessarily an error. It could have been canceled due to a negative value returned by 'acs_apbm_timer::handle_timeout()' method !");
	}

	return 0;
}
