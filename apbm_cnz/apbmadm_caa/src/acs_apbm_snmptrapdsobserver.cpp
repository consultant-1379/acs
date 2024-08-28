#include <ace/Guard_T.h>

#include "acs_apbm_logger.h"
#include "acs_apbm_snmpconstants.h"
#include "acs_apbm_snmptrapmessagehandler.h"
#include "acs_apbm_serverworkingset.h"

#include "acs_apbm_snmptrapdsobserver.h"

void __CLASS_NAME__::handleTrap (ACS_TRAPDS_StructVariable var) {
	// Check the program running state: APBM server handles traps only when
	// it is running in NODE ACTIVE mode
	if (_server_working_set->program_state == acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) {
		acs_apbm_snmptrapmessagehandler trap_handler(_server_working_set);

		if (const int call_result = trap_handler.handle_trap(var))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'handle_trap' failed: handling the PDU trap message: call_result == %d", call_result);
	} else ACS_APBM_LOG(LOG_LEVEL_WARN, "APBM server is not running in ACTIVE NODE mode: this trap message notification will be ignored: program_state == %d", _server_working_set->program_state);
}

int __CLASS_NAME__::subscribe (std::string serviceName) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Subscribing observer " ACS_APBM_STRINGIZE(__CLASS_NAME__) " into APBM server to the Trap Dispatcher Service...");

	if (_subscribed) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Observer " ACS_APBM_STRINGIZE(__CLASS_NAME__) " already subscribed");
		return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	}

	ACS_TRAPDS::ACS_TRAPDS_API_Result trapds_call_result = ACS_TRAPDS::Result_Failure;
	for(int i =0;i<5 && trapds_call_result != ACS_TRAPDS::Result_Success; ++i )
	{
		trapds_call_result = ACS_TRAPDS_API::subscribe(serviceName);
		if(trapds_call_result != ACS_TRAPDS::Result_Success)
		{
			sleep(1);
                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "APBM failed to subscribe with TRAPDS trying again");
			continue;
		}
	}
	if (trapds_call_result != ACS_TRAPDS::Result_Success) { // ERROR: subscribing myself to the trap dispatcher service
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'ACS_TRAPDS_API::subscribe' failed: cannot subscribe APBM server to the trap dispatcher service: trapds_call_result == %d",
				trapds_call_result);
		return acs_apbm_snmp::ERR_SNMP_TRAP_SUBSCRIPTION;
	}

	_subscribed = 1;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Observer " ACS_APBM_STRINGIZE(__CLASS_NAME__) " into APBM server successfully subscribed");

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::unsubscribe () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "UnSubscribing observer " ACS_APBM_STRINGIZE(__CLASS_NAME__) " into APBM server to the Trap Dispatcher Service...");

	if (!_subscribed) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Observer " ACS_APBM_STRINGIZE(__CLASS_NAME__) " already unsubscribed");
		return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	}

	ACS_TRAPDS::ACS_TRAPDS_API_Result trapds_call_result = ACS_TRAPDS_API::unsubscribe();

	if (trapds_call_result != ACS_TRAPDS::Result_Success) { // ERROR: subscribing myself to the trap dispatcher service
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'ACS_TRAPDS_API::unsubscribe' failed: cannot unsubscribe APBM server to the trap dispatcher service: trapds_call_result == %d",
				trapds_call_result);
		return acs_apbm_snmp::ERR_SNMP_TRAP_SUBSCRIPTION;
	}

	_subscribed = 0;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Observer " ACS_APBM_STRINGIZE(__CLASS_NAME__) " into APBM server successfully unsubscribed");

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}
