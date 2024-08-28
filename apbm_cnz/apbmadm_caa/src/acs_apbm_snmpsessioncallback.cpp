#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_snmpmanager.h"

#include "acs_apbm_snmpsessioncallback.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_snmpsessioncallback

int __CLASS_NAME__::check_pdu_operation (int operation) const {
	if (operation != NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE) { // ERROR: no message received
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No message contained in this response PDU");
		return (error_code = acs_apbm_snmp::ERR_SNMP_CALLBACK_NO_MESSAGE_RECEIVED);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::check_pdu_error_stat (int error_stat, int error_stat_in_pdu) const {
	if (error_stat != acs_apbm_snmp::ERR_SNMP_NO_ERRORS) { // ERROR:
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU has an error flag indication: errstat in PDU == %ld", error_stat_in_pdu);
		return (error_code = acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::chech_null_var_list (netsnmp_variable_list * var_list) const {
	if (!var_list) { // ERROR: Very strange no variables was bound to the response pdu
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU has no variable and no value bound to it");
		return (error_code = acs_apbm_snmp::ERR_SNMP_PDU_NO_VARIABLE_BOUND);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::check_pdu_var_type (int var_type, int expected) const {

	if(var_type == 0x81){
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "The received PDU variable refers: 'No Such Instance currently exists at this OID type' variable type == %d", var_type);
		return (error_code = acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_NO_SUCH_INSTANCE);
	}
	if (var_type != expected) { // ERROR: unexpected variable type
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU variable is not of expected type: variable type == %d, expected type == %d", var_type, expected);
		return (error_code = acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_BAD_TYPE);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::check_pdu_var_size (size_t var_size, size_t min_expected) const {
	if (var_size < min_expected) { // ERROR: data size not correct
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU variable size is less than expected minimum size: variable size == %zu, expected min size = %zu",
				var_size, min_expected);
		return (error_code = acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_BAD_SIZE);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::common_pdu_checks (int operation, int error_stat, int error_stat_in_pdu, netsnmp_variable_list * var_list) const {


	if (const int call_result = check_pdu_operation(operation)) return call_result;

	if (const int call_result = check_pdu_error_stat(error_stat, error_stat_in_pdu)) return call_result;

	if (const int call_result = chech_null_var_list(var_list)) return call_result;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::read_scbrp (netsnmp_variable_list * /*var_list*/) const {
	// Empty implementation
	return 1; //OK return for NET-SNMP library
}

int __CLASS_NAME__::read_scx (netsnmp_variable_list * /*var_list*/) const {
	// Empty implementation
	return 1; //OK return for NET-SNMP library
}

int __CLASS_NAME__::operator() (int operation, netsnmp_session * /*session*/, int request_id, netsnmp_pdu * pdu) const {
	char log_msg [1024];
	int chars_printed = ::snprintf(log_msg, ACS_APBM_ARRAY_SIZE(log_msg), "PDU response received for the request id %d", request_id);
	switch_board_ip() && (chars_printed += ::snprintf(log_msg + chars_printed, ACS_APBM_ARRAY_SIZE(log_msg) - chars_printed,
			" from switch board '%s'", switch_board_ip()));
	chars_printed += ::snprintf(log_msg + chars_printed, ACS_APBM_ARRAY_SIZE(log_msg) - chars_printed, ": operation == %d", operation);
	if (board_slot != -1) {
		chars_printed += ::snprintf(log_msg + chars_printed, ACS_APBM_ARRAY_SIZE(log_msg) - chars_printed,
				", data exchanged for board in slot %d", board_slot);
		board_magazine_str() && (chars_printed += ::snprintf(log_msg + chars_printed, ACS_APBM_ARRAY_SIZE(log_msg) - chars_printed,
				" of magazine %s", board_magazine_str()));
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, log_msg);

#if 0 // TODO: BEGIN: OLD CODE: TO BE DELETED
	ACS_APBM_LOG(LOG_LEVEL_INFO, "PDU response received for the request id %d from switch board IP %s: "
			"operation == %d: data exchanged for board in slot %d of magazine %s",
			request_id, switch_board_ip, operation, board_slot, board_magazine_str);
#endif // TODO: END: OLD CODE: TO BE DELETED

	netsnmp_variable_list * var_list = pdu->variables;


	char buffer [2 * 1024] = {0};  //ONLY FOR DEBUG SCOPE
	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {
		snprint_variable(buffer, ACS_APBM_ARRAY_SIZE(buffer), var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "PDU response received < errstat == %ld  pdu_data == '%s'>\n", pdu->errstat, buffer);
	}

	// Set the pdu error stat and perform common pdu checks
	if (common_pdu_checks(operation, pdu_error_stat = acs_apbm_snmpmanager::map_net_snmp_error_stat(pdu->errstat), pdu->errstat, var_list)) return 1;

	return is_cba ? read_scx(var_list) : read_scbrp(var_list);
}
