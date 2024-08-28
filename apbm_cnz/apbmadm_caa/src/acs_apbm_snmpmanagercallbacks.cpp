#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_snmpmanagercallbacks.h"
#include "acs_apbm_csreader.h"

#define __ACS_APBM_UTILS_MEMCPY_INCREMENT__(dest, src, n) ((::memcpy((dest), (src), (n))), (dest[(n)] = 0), (src += (n)))

namespace {
	void str_trim_right (char * str, size_t size) {
		if (!size) return;

		char * str_tmp = str + (size - 1);

		for (char c = *str_tmp; (str_tmp >= str) && ((c == ' ') || !c); c = *--str_tmp) ;

		if(static_cast<size_t>(++str_tmp - str) < size) *str_tmp = 0;
	}
}


#if 0 // TODO: BEGIN: OLD CODE: TO BE DELETED
int acs_apbm_snmploggercallback::operator() (int /*operation*/, netsnmp_session * /*session*/, int request_id, netsnmp_pdu * pdu) const {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "%s: request_id == %d", (message ?: "SNMP Response received for request"), request_id);
	ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP response pdu data follows");

	char buffer [2 * 1024];

	for (netsnmp_variable_list * var = pdu->variables; var; var = var->next_variable) {
		snprint_variable(buffer, ACS_APBM_ARRAY_SIZE(buffer), var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "    %s", buffer);
	}

	return 1; //OK return for NET-SNMP library
}
#endif // TODO: END: OLD CODE: TO BE DELETED

int acs_apbm_snmploggercallback::read_scx (netsnmp_variable_list * var_list) const {
	if (message) ACS_APBM_LOG(LOG_LEVEL_INFO, message);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP response pdu data follows");

	char buffer [2 * 1024];

	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {
		snprint_variable(buffer, ACS_APBM_ARRAY_SIZE(buffer), var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "    %s", buffer);
	}

	return 1; //OK return for NET-SNMP library
}


const char * acs_apbm_getboarddatacallback::BOARD_DATA_FIELD_NAMES [] = {
		"Product_Number", "Product_Revision_State", "Product_Name", "Serial_Number", "Manufacturing_Date", "Vendor_Name"
};

#if 0 // TODO: BEGIN: OLD CODE: TO BE DELETED
int acs_apbm_getboarddatacallback::operator() (int operation, netsnmp_session * /*session*/, int request_id, netsnmp_pdu * pdu) const {
	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"PDU respose received for the GET-BOARD-DATA request id %d from switch board IP %s: "
			"operation == %d: data requested for board in slot %d of magazine %s",
			request_id, switch_board_ip, operation, board_slot, board_magazine_str);

	netsnmp_variable_list * var_list = pdu->variables;

	// Set the pdu error stat and perform common pdu checks
	if (common_pdu_checks(operation, pdu_error_stat = acs_apbm_snmpmanager::map_net_snmp_error_stat(pdu->errstat), pdu->errstat, var_list)) return 1;

	return is_cba ? read_boarddata_scx(var_list) : read_boarddata_scbrp(var_list);
}
#endif // TODO: END: OLD CODE: TO BE DELETED

//int acs_apbm_getboarddatacallback::read_boarddata_scbrp(netsnmp_variable_list * var_list) const {
int acs_apbm_getboarddatacallback::read_scbrp(netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board data from SCBRP");

	// Check that the variable is an OBJECT-TYPE as specified in the AXE-ETH-SHELF-MIB definition file
	if (var_list->type != ASN_OCTET_STR) { // ERROR: Unexpected type for the PDU variable
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU variable is not an ASN_OCTET_STR type variable: var_list->type == %u", static_cast<unsigned>(var_list->type));
		error_code = acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_BAD_TYPE;
		return 1;
	}

	// Check the size of the data inside the variable
	if (var_list->val_len < acs_apbm_snmp::SNMP_BOARD_DATA_TOTAL_SIZE) { // ERROR: data size not correct
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"The received PDU variable size is less than expected size: var_list->val_len == %zu, expected size == %d",
				var_list->val_len, acs_apbm_snmp::SNMP_BOARD_DATA_TOTAL_SIZE);
		error_code = acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_BAD_SIZE;
		return 1;
	}
	char * octet_string = reinterpret_cast<char *>(var_list->val.string);

	get_field_from_data_record(product_number, octet_string, acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE);

	octet_string += acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE;
	get_field_from_data_record(product_revision_state, octet_string, acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE);

	octet_string += acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE;
	get_field_from_data_record(product_name, octet_string, acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE);

	octet_string += acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE;
	get_field_from_data_record(serial_number, octet_string, acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE);

	octet_string += acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE;
	get_field_from_data_record(manufacture_date, octet_string, acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE);

	octet_string += acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE;
	get_field_from_data_record(vendor, octet_string, acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE);

	error_code = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	return 1; //OK return for NET-SNMP library
}

//int acs_apbm_getboarddatacallback::read_boarddata_scx(netsnmp_variable_list * var_list) const {
int acs_apbm_getboarddatacallback::read_scx(netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board data from SCX or SMX");

	oid data_oid_names [][17] = {
		{ ACS_APBM_SCX_OID_BLADE_PRODUCT_NUMBER, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_PRODUCT_REVISION_STATE, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_PRODUCT_NAME, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_SERIAL_NUMBER, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_MANUFACTURING_DATE, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_VENDOR_NAME, board_slot }
	};

	char tmp_str [2 * 1024];

	char * data_fields_ptr [ACS_APBM_ARRAY_SIZE(data_oid_names)] = {
			product_number, product_revision_state, product_name, serial_number, manufacture_date, vendor
	};

	size_t data_field_sizes [ACS_APBM_ARRAY_SIZE(data_oid_names)] = {
			acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE,
			acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE,
			acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE,
			acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE,
			acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE,
			acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE
	};

	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(data_oid_names); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(data_oid_names[i], ACS_APBM_ARRAY_SIZE(data_oid_names[i]), var->name, var->name_length) == 0) {
				// Reading Board Data value at index i

				int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading board '%s' value : NET-SNMP variable printout --> ", BOARD_DATA_FIELD_NAMES[i]);
				snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
				ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

				if (check_pdu_var_type(var->type, ASN_OCTET_STR)) return 1;

				if (data_fields_ptr[i]) {
					data_fields_ptr[i][0] = 0;
					get_field_from_data_record(data_fields_ptr[i], reinterpret_cast<char *>(var->val.string), data_field_sizes[i] <= var->val_len ? data_field_sizes[i] : var->val_len);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The PDU contains a '%s' board data value but the caller did not requested it",
						BOARD_DATA_FIELD_NAMES[i]);

				break;
			}
		}
	}

	return 1; //OK return for NET-SNMP library
}

void acs_apbm_getboarddatacallback::get_field_from_data_record (char * buffer_dest, const char * buffer_src, size_t buffer_len) const {

	 ::memcpy(buffer_dest, buffer_src, buffer_len);
	 buffer_dest[buffer_len] = 0;

	 // remove backspaces from data record field
	int i =  buffer_len -1;
	while (i >= 0){
		if (buffer_dest[i] == ' ') i--;
		else break;
	}
	if ((size_t) i < buffer_len -1) buffer_dest [i+1]= '\0';

}


#if 0 // TODO: BEGIN: OLD CODE: TO BE DELETED
int acs_apbm_getboardipmidatacallback::operator() (int operation, netsnmp_session * /*session*/, int request_id, netsnmp_pdu * pdu) const {
	uint32_t agent_address = *reinterpret_cast<uint32_t *>(pdu->agent_addr);

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"PDU respose received for the GET-BOARD-IPMI-INFO request id %d from switch board IP %s (NET-SNMP agent address == 0x%08X): "
			"operation == %d: data requested for board in slot %d of magazine %s",
			request_id, switch_board_ip, agent_address, operation, board_slot, board_magazine_str);

	netsnmp_variable_list * var_list = pdu->variables;

	// Set the pdu error stat and perform common pdu checks
	if (common_pdu_checks(operation, pdu_error_stat = acs_apbm_snmpmanager::map_net_snmp_error_stat(pdu->errstat), pdu->errstat, var_list)) return 1;

	return is_cba ? read_data_scx(var_list) : read_data_scbrp(var_list);
}
#endif // TODO: END: OLD CODE: TO BE DELETED


#if 0 // TODO: BEGIN: OLD CODE: TO BE DELETED
//int acs_apbm_getboardipmidatacallback::read_data_scbrp (netsnmp_variable_list * var_list) const {
int acs_apbm_getboardipmidatacallback::read_scbrp (netsnmp_variable_list * var_list) const {
	int var_count = 0;
	char tmp_str [2 * 1024];

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board IPMI data from SCBRP");

	*ipmi_firmware_type = 0;
	::strcpy(ipmi_product_number, "<NOT SUPPORTED>");
	::strcpy(ipmi_product_revision, "00000000");
	*ipmi_upgrade_status = axe_eth_shelf::IPMI_UPGRADE_STATUS_UNDEFINED;

	oid oid_ipmi_data [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_DATA, board_slot };
	oid oid_ipmi_upg_status [] ={ ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_UPGRADE_STATUS, board_slot };

	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {
		if (acs_apbm_snmpmanager::oid_compare(oid_ipmi_data, ACS_APBM_ARRAY_SIZE(oid_ipmi_data), var->name, var->name_length) == 0) {
			// Reading Board IPMI Data

			int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading board IPMI data: NET-SNMP variable printout --> ");
			snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
			ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

			if (check_pdu_var_type(var->type, ASN_OCTET_STR)) return 1;

			if (check_pdu_var_size(var->val_len, acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_TOTAL_SIZE)) return 1;

			char * octet_string = reinterpret_cast<char *>(var->val.string);

			*ipmi_firmware_type = static_cast<int16_t>(*octet_string++);
			__ACS_APBM_UTILS_MEMCPY_INCREMENT__(ipmi_product_number, octet_string, acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE);
			__ACS_APBM_UTILS_MEMCPY_INCREMENT__(ipmi_product_revision, octet_string, acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE);

			var_count++;

		} else if (acs_apbm_snmpmanager::oid_compare(oid_ipmi_upg_status, ACS_APBM_ARRAY_SIZE(oid_ipmi_upg_status), var->name, var->name_length) == 0) {
			// Reading the Board IPMI Upgrade Status

			int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading board IPMI upgrade status: NET-SNMP variable printout --> ");
			snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
			ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

			if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

			*ipmi_upgrade_status = static_cast<axe_eth_shelf::ipmi_upgrade_status_t>(*(var->val.integer));
			var_count++;

		} else { // ERROR: Variable OID unexpected

			int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "This board IPMI data PDU contains an unexpected variable: NET-SNMP variable printout --> ");
			snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
			ACS_APBM_LOG(LOG_LEVEL_ERROR, tmp_str);

			error_code = acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
			return 1;

		}
	}

	if (var_count > 2) { // ERROR: the PDU had too many variable in it
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "This board IPMI data PDU contains too many variable: variable count == %d", var_count);
		error_code = acs_apbm_snmp::ERR_SNMP_PDU_TOO_MANY_VARIABLES;
	}

	return 1; //OK return for NET-SNMP library
}
#endif // TODO: END: OLD CODE: TO BE DELETED

int acs_apbm_getboardipmidatacallback::read_scbrp (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board IPMI data from SCBRP");

	// Arrange the oid names
	const oid ipmi_data_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_DATA, board_slot };
	const oid ipmi_upgrade_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_UPGRADE, board_slot };
	const oid ipmi_upgrade_status_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_UPGRADE_STATUS, board_slot };

	const oid * oid_names [3] = { ipmi_data_name, ipmi_upgrade_name, ipmi_upgrade_status_name };

	size_t oid_names_sizes [3] = { ACS_APBM_ARRAY_SIZE(ipmi_data_name), ACS_APBM_ARRAY_SIZE(ipmi_upgrade_name), ACS_APBM_ARRAY_SIZE(ipmi_upgrade_status_name) };

	return read_values(oid_names, oid_names_sizes, var_list);
}

//int acs_apbm_getboardipmidatacallback::read_data_scx (netsnmp_variable_list * /*var_list*/) const {
int acs_apbm_getboardipmidatacallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board IPMI data from SCX or SMX");

	// Arrange the oid names
	const oid ipmi_data_name [] = { ACS_APBM_SCX_OID_BLADE_IPMI_DATA, board_slot };
	const oid ipmi_upgrade_name [] = { ACS_APBM_SCX_OID_BLADE_IPMI_UPGRADE, board_slot };
	const oid ipmi_upgrade_status_name [] = { ACS_APBM_SCX_OID_BLADE_IPMI_UPGRADE_STATUS, board_slot };

	const oid * oid_names [3] = { ipmi_data_name, ipmi_upgrade_name, ipmi_upgrade_status_name };

	size_t oid_names_sizes [3] = { ACS_APBM_ARRAY_SIZE(ipmi_data_name), ACS_APBM_ARRAY_SIZE(ipmi_upgrade_name), ACS_APBM_ARRAY_SIZE(ipmi_upgrade_status_name) };

	int call_result = read_values(oid_names, oid_names_sizes, var_list);

	if (!call_result && ipmi_upgrade_status) // Check the value in the ipmi_upgrade_status field
		switch (*ipmi_upgrade_status) {
		case 2: *ipmi_upgrade_status = axe_eth_shelf::IPMI_UPGRADE_STATUS_NOT_STARTED; break;
		case 3: *ipmi_upgrade_status = axe_eth_shelf::IPMI_UPGRADE_STATUS_NOT_SUPPORTED; break;
		default: break;
		}

	return call_result;
}

int acs_apbm_getboardipmidatacallback::read_values (const oid * (& oid_names) [3], size_t (& oid_names_sizes) [3], netsnmp_variable_list * var_list) const {
	char tmp_str [2 * 1024];

	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(oid_names); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(oid_names[i], oid_names_sizes[i], var->name, var->name_length) == 0) {
				int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following IPMI info: NET-SNMP variable printout --> ");
				snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
				ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

				switch (i) {
				case 0: read_ipmi_data(var); break;
				case 1: read_ipmi_upgrade(var); break;
				case 2: read_ipmi_upgrade_status(var); break;
				default:
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "An unknow IPMI info was requested: handling of this info is not implemented");
					break;
				}

				break;
			}
		}
	}

	return 1;
}

int acs_apbm_getboardipmidatacallback::read_ipmi_data (netsnmp_variable_list * var) const {
	if (check_pdu_var_type(var->type, ASN_OCTET_STR)) return 1;

	if (ipmi_firmware_type || ipmi_product_number || ipmi_product_revision) { // OK: reading at least one info
		const char * value = reinterpret_cast<char *>(var->val.string);

		// In CBA architecture the data are on the 4th record and so we should move the pointer forward
//		is_cba && (value += 126); //commented to get ipmifwversion properly XKICHAG

		if (ipmi_firmware_type) {
			int16_t val = *value;
			if (val >= '0') val -= '0';
			*ipmi_firmware_type = val;
		}
		value += acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_FIRMWARE_TYPE_SIZE;

		if (ipmi_product_number) {
			ipmi_product_number[0] = ipmi_product_number[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE] = 0;
			::memcpy(ipmi_product_number, value, acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE);
			str_trim_right(ipmi_product_number, acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1);
		}
		value += acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE;

		if (ipmi_product_revision) {
			ipmi_product_revision[0] = ipmi_product_revision[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE] = 0;
			::memcpy(ipmi_product_revision, value, acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE);
		}
	} else // IPMI data fields not requested by the caller
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"The PDU contains an IPMI data info (ipmi firmware type, product number and product revision) but the caller did not requested it");

	return 1;
}

int acs_apbm_getboardipmidatacallback::read_ipmi_upgrade (netsnmp_variable_list * /*var*/) const {
	ACS_APBM_LOG(LOG_LEVEL_WARN, "Getting board IPMI upgrade data not implemented because no one should request this info");

	return 1;
}

int acs_apbm_getboardipmidatacallback::read_ipmi_upgrade_status (netsnmp_variable_list * var) const {
	if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

	if (ipmi_upgrade_status) *ipmi_upgrade_status = static_cast<axe_eth_shelf::ipmi_upgrade_status_t>(*reinterpret_cast<int *>(var->val.integer));
	else ACS_APBM_LOG(LOG_LEVEL_ERROR,
			"The PDU contains an IPMI upgrade status info but the caller did not requested it");

	return 1;
}


const char * acs_apbm_getboardledsstatuscallback::LED_NAMES [] = {"GREEN", "RED", "MIA"};
const char * acs_apbm_getboardledsstatuscallback::FOUR_LED_NAMES [] = {"GREEN", "RED", "MIA", "BLUE"};

#if 0 // TODO: BEGIN: OLD CODE: TO BE DELETED
int acs_apbm_getboardledsstatuscallback::operator() (int operation, netsnmp_session * /*session*/, int request_id, netsnmp_pdu * pdu) const {
	uint32_t agent_address = *reinterpret_cast<uint32_t *>(pdu->agent_addr);

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"PDU respose received for the GET-BOARD-LEDs-STATUS-INFO request id %d from switch board IP %s (NET-SNMP agent address == 0x%08X): "
			"operation == %d: data requested for board in slot %d of magazine %s",
			request_id, switch_board_ip, agent_address, operation, board_slot, board_magazine_str);

	netsnmp_variable_list * var_list = pdu->variables;

	// Set the pdu error stat and perform common pdu checks
	if (common_pdu_checks(operation, pdu_error_stat = acs_apbm_snmpmanager::map_net_snmp_error_stat(pdu->errstat), pdu->errstat, var_list)) return 1;

	return is_cba ? read_data_scx(var_list) : read_data_scbrp(var_list);
}
#endif

//int acs_apbm_getboardledsstatuscallback::read_data_scbrp (netsnmp_variable_list * var_list) const {
int acs_apbm_getboardledsstatuscallback::read_scbrp (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board LEDs status from SCBRP");
	if(board_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
	{
		if(is_new_gea_supported) //for new GEA 
	        {
        	        oid green_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_GREEN_LED, board_slot };
	                oid red_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_RED_LED, board_slot };
        	        oid mia_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_YELLOW_LED, board_slot };
                	oid blue_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_BLUE_LED, board_slot }; //only for 4 LED hw
	                const oid * four_led_oids [4] = {green_name_4led, red_name_4led, mia_name_4led, blue_name_4led};
        	        size_t four_led_oid_sizes [4] = { ACS_APBM_ARRAY_SIZE(green_name_4led), ACS_APBM_ARRAY_SIZE(red_name_4led), ACS_APBM_ARRAY_SIZE(mia_name_4led), ACS_APBM_ARRAY_SIZE(blue_name_4led) };
                	return read_4leds_data(four_led_oids, four_led_oid_sizes, var_list);
	        }
        	else // for old GEA
	        {
        	        oid green_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_GREEN_LED, board_slot };
                	oid red_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_RED_LED, board_slot };
	                oid mia_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_YELLOW_LED, board_slot };
        	        const oid * led_oids [3] = {green_name, red_name, mia_name};
	                size_t led_oid_sizes [3] = { ACS_APBM_ARRAY_SIZE(green_name), ACS_APBM_ARRAY_SIZE(red_name), ACS_APBM_ARRAY_SIZE(mia_name) };
        	        return read_leds_data(led_oids, led_oid_sizes, var_list);
	        }
	}
	else
	{
	        if( is_4led_supported ) //for 4Leds
        	{
        		oid green_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_GREEN_LED, board_slot };
		        oid red_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_RED_LED, board_slot };
        		oid mia_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_YELLOW_LED, board_slot };
			oid blue_name_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_BLUE_LED, board_slot }; //only for 4 LED hw
			const oid * four_led_oids [4] = {green_name_4led, red_name_4led, mia_name_4led, blue_name_4led};
			size_t four_led_oid_sizes [4] = { ACS_APBM_ARRAY_SIZE(green_name_4led), ACS_APBM_ARRAY_SIZE(red_name_4led), ACS_APBM_ARRAY_SIZE(mia_name_4led), ACS_APBM_ARRAY_SIZE(blue_name_4led) };
	    		return read_4leds_data(four_led_oids, four_led_oid_sizes, var_list);
	        }
		else
		{
			oid green_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_GREEN_LED, board_slot };
			oid red_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_RED_LED, board_slot };
			oid mia_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_YELLOW_LED, board_slot };
			const oid * led_oids [3] = {green_name, red_name, mia_name};
			size_t led_oid_sizes [3] = { ACS_APBM_ARRAY_SIZE(green_name), ACS_APBM_ARRAY_SIZE(red_name), ACS_APBM_ARRAY_SIZE(mia_name) };
			return read_leds_data(led_oids, led_oid_sizes, var_list);
		}
	}
}

//int acs_apbm_getboardledsstatuscallback::read_data_scx (netsnmp_variable_list * var_list) const {
int acs_apbm_getboardledsstatuscallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board LEDs status from SCX or SMX");
        if(board_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
        {
                if(is_new_gea_supported) //for new GEA
                {
			oid green_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_GREEN_LED, board_slot };
        	        oid red_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_RED_LED, board_slot };
                	oid mia_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_YELLOW_LED, board_slot };
	                oid blue_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_BLUE_LED, board_slot };
        	        const oid * four_led_oids [4] = {green_name_4led, red_name_4led, mia_name_4led, blue_name_4led};
	                size_t four_led_oid_sizes [4] = { ACS_APBM_ARRAY_SIZE(green_name_4led), ACS_APBM_ARRAY_SIZE(red_name_4led), ACS_APBM_ARRAY_SIZE(mia_name_4led), ACS_APBM_ARRAY_SIZE(blue_name_4led) };
        	        return read_4leds_data(four_led_oids, four_led_oid_sizes, var_list);
	        }
        	else // for old GEA
	        {
        	        oid green_name [] = { ACS_APBM_SCX_OID_BLADE_GREEN_LED, board_slot };
                	oid red_name [] = { ACS_APBM_SCX_OID_BLADE_RED_LED, board_slot };
	                oid mia_name [] = { ACS_APBM_SCX_OID_BLADE_YELLOW_LED, board_slot };
        	        const oid * led_oids [3] = {green_name, red_name, mia_name};
                	size_t led_oid_sizes [3] = { ACS_APBM_ARRAY_SIZE(green_name), ACS_APBM_ARRAY_SIZE(red_name), ACS_APBM_ARRAY_SIZE(mia_name) };
	                return read_leds_data(led_oids, led_oid_sizes, var_list);
        	}
	}
	else
	{
	    	if( is_4led_supported ) //for 4Leds
		{
	    		oid green_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_GREEN_LED, board_slot };
		        oid red_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_RED_LED, board_slot };
        		oid mia_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_YELLOW_LED, board_slot };
			oid blue_name_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_BLUE_LED, board_slot };
			const oid * four_led_oids [4] = {green_name_4led, red_name_4led, mia_name_4led, blue_name_4led};
			size_t four_led_oid_sizes [4] = { ACS_APBM_ARRAY_SIZE(green_name_4led), ACS_APBM_ARRAY_SIZE(red_name_4led), ACS_APBM_ARRAY_SIZE(mia_name_4led), ACS_APBM_ARRAY_SIZE(blue_name_4led) };
			return read_4leds_data(four_led_oids, four_led_oid_sizes, var_list);
		}
		else
		{
			oid green_name [] = { ACS_APBM_SCX_OID_BLADE_GREEN_LED, board_slot };
			oid red_name [] = { ACS_APBM_SCX_OID_BLADE_RED_LED, board_slot };
			oid mia_name [] = { ACS_APBM_SCX_OID_BLADE_YELLOW_LED, board_slot };
			const oid * led_oids [3] = {green_name, red_name, mia_name};
			size_t led_oid_sizes [3] = { ACS_APBM_ARRAY_SIZE(green_name), ACS_APBM_ARRAY_SIZE(red_name), ACS_APBM_ARRAY_SIZE(mia_name) };
			return read_leds_data(led_oids, led_oid_sizes, var_list);
		}
	}
}

//Read 3 LEDs Data
int acs_apbm_getboardledsstatuscallback::read_leds_data (const oid * ( &led_oids) [3], size_t ( &led_oid_sizes) [3], netsnmp_variable_list * var_list) const {
	char tmp_str [2 * 1024];
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "read three leds data");

        
	axe_eth_shelf::led_status_t * led_fields_ptr [ACS_APBM_ARRAY_SIZE(led_oids)] = {green_led, red_led, mia_led};

	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(led_oids); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(led_oids[i], led_oid_sizes[i], var->name, var->name_length) == 0) {
				// Reading Board LED at index i: 0 --> GREEN, 1 --> RED, 2 --> MIA

				int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading board %s LED status : NET-SNMP variable printout --> ", LED_NAMES[i]);
				snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
				ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

				if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

				if (led_fields_ptr[i]) *(led_fields_ptr[i]) = static_cast<axe_eth_shelf::led_status_t>(*(var->val.integer));
				else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The PDU contains a %s LED status value but the caller did not requested it", LED_NAMES[i]);

				break;
			}
		}
	}

	return 1;
}

//Read 4 Leds Data
int acs_apbm_getboardledsstatuscallback::read_4leds_data (const oid * ( &led_oids) [4], size_t ( &led_oid_sizes) [4], netsnmp_variable_list * var_list) const {
	char tmp_str [2 * 1024];
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "read_4leds_data");


	axe_eth_shelf::led_status_t * led_fields_ptr [ACS_APBM_ARRAY_SIZE(led_oids)] = {green_led, red_led, mia_led, blue_led};

	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(led_oids); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(led_oids[i], led_oid_sizes[i], var->name, var->name_length) == 0) {
				// Reading Board LED at index i: 5 --> GREEN, 4 --> RED, 7 --> MIA, 6 --> BLUE(new 4th LED)

				int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading board %s LED status : NET-SNMP variable printout --> ", FOUR_LED_NAMES[i]);
				snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
				ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

				if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

				if (led_fields_ptr[i]) *(led_fields_ptr[i]) = static_cast<axe_eth_shelf::led_status_t>(*(var->val.integer));
				else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The PDU contains a %s LED status value but the caller did not requested it", FOUR_LED_NAMES[i]);

				break;
			}
		}
	}

	return 1;
}

int acs_apbm_getboardbiosrunmodecallback::read_scbrp (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board BIOS run mode data from SCBRP");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_BIOS_RUN_MODE, board_slot };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getboardbiosrunmodecallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board BIOS run mode data from SCX or SMX");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCX_OID_BLADE_BIOS_RUN_MODE, board_slot };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getboardbiosrunmodecallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following BIOS run mode info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

		// The value in the PDU is '0' or '1' character, so it should be converted in integer values 0 or 1
		if (bios_run_mode) {
			int val = *var->val.integer;
			if (val >= '0') val -= '0';
			*bios_run_mode = static_cast<axe_eth_shelf::bios_boot_run_mode_t>(val);
		} else ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"The PDU contains a BIOS run mode info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected BIOS run mode value");

	return 1;
}

int acs_apbm_getshelfmgrcontrolstatecallback::read_scbrp (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading switch board Master/Slave status from SCBRP");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCB_OID_SHELF_MGR_CMD, board_slot };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getshelfmgrcontrolstatecallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading switch board Master/Slave status from SCX or SMX");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCX_OID_SHELF_MGR_CTRL_STATE, 0 };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getshelfmgrcontrolstatecallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following switch board Master/Slave status info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type, is_cba ? ASN_OCTET_STR : ASN_INTEGER)) return 1;

		if (state) *state = is_cba ? (static_cast<int>(*var->val.string)) : (*var->val.integer);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The PDU contains a Master/Slave status info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected Master/Slave status value");

	return 1;
}

int acs_apbm_getshelfneighbourcallback::read_scbrp (netsnmp_variable_list * /* var_list*/) const {
	// This callback should never get !!!
	ACS_APBM_LOG(LOG_LEVEL_WARN, "Trying to reading shelf neighbour presence from SCBRP: this OID is not defined in SCBRP mib!");

	return 1;
}

int acs_apbm_getshelfneighbourcallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading shelf neighbour presence from SCX or SMX");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCX_OID_SHELF_MGR_NEIGHBOUR_PRESENCE, 0 };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}


int acs_apbm_getshelfneighbourcallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following shelf neighbour presence info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type,  ASN_INTEGER)) return 1;

		if (shelfneighbour) *shelfneighbour = static_cast<acs_apbm_snmp::neighbour_t>(*var->val.integer);
			else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The PDU contains a shelf neighbour presence info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the shelf neighbour presence value");

	return 1;
}


int acs_apbm_getboardpowerstatuscallback::read_scbrp (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board POWER status from SCBRP");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_PWR_ON, board_slot };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getboardpowerstatuscallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board POWER status from SCX or SMX");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCX_OID_BLADE_PWR, board_slot };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getboardpowerstatuscallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following POWER status info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

		if (power_status) *power_status = static_cast<acs_apbm_snmp::bladePwr_status_t>(*var->val.integer);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"The PDU contains a board POWER status info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected POWER status value");

	return 1;
}

/*
* Callback function to read the PORT Status of front port from the mib for SCX boards.
*/
int acs_apbm_getfrontportstatuscallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading PORT status from SCX");

	// Arrange the oid names
	const oid oid_name [] = { IF_OPER_STATUS, board_slot };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getfrontportstatuscallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following POER status info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

		if (port_status) *port_status = static_cast<acs_apbm_snmp::frontPort_status_t>(*var->val.integer);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"The PDU contains a PORT status info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected PORT status value");

	return 1;
}


/*
* Callback function to read the value of blade_led_type from the mib for scbrp boards.
*/
int acs_apbm_getbladeledtypecallback::read_scbrp (netsnmp_variable_list * var_list) const {
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board led type data from SCBRP");

        // Arrange the oid names
        const oid oid_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_LED_TYPE, board_slot };

        return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

/*
* Callback function to read the value of blade_led_type from the mib for scx boards.
*/
int acs_apbm_getbladeledtypecallback::read_scx (netsnmp_variable_list * var_list) const {
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading blade led type data from SCX or SMX");

        // Arrange the oid names
        const oid oid_name [] = { ACS_APBM_SCX_OID_BLADE_LED_TYPE, board_slot };

        return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getbladeledtypecallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
        char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
        	int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following blade led type info: NET-SNMP variable printout --> ");
                snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
                ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);
		if(var->type == 128)   // 128 is the snmp error data type for "No Such Object available on this agent at this OID"
		{
			ACS_APBM_LOG(LOG_LEVEL_INFO, "No Such Object available on this agent at this OID");
       	                *blade_led_type = acs_apbm_snmp::BLADE_LED_TYPE_NOT_SUPPORTED;
		 	return 0;	
		}
                if (check_pdu_var_type(var->type, ASN_INTEGER)) 
			return 1;

                if (blade_led_type)
                	*blade_led_type = static_cast<acs_apbm_snmp::blade_led_type_t>(*var->val.integer);
                else 
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "The PDU contains blade led type info but the caller did not request it");
        }
        else
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected blade led type value");

        return 1;
}

int acs_apbm_getboardfataleventlogcallback::read_scbrp (netsnmp_variable_list * var_list) const {
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board FATAL EVENT LOG info from SCBRP");

        // Arrange the oid names
        const oid oid_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_FATAL_EVENT_LOG_INFO, board_slot };

        return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getboardfataleventlogcallback::read_scx (netsnmp_variable_list * var_list) const {
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board FATAL EVENT LOG info from SCX or SMX");

        // Arrange the oid names
        const oid oid_name [] = { ACS_APBM_SCX_OID_FATAL_EVENT_LOG_INFO, board_slot };

        return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getboardfataleventlogcallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
        char tmp_str [2 * 1024];
        int ret = 0;
        if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
                int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following FATAL EVENT LOG info: NET-SNMP variable printout --> ");
                snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
                ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

                if (check_pdu_var_type(var->type, ASN_OCTET_STR)) return 1;


                if (fatalEventLogInfo) {
                    const unsigned char * tmp = reinterpret_cast<unsigned char *>(var->val.string);
                    char value[acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1] = {0};
                    for(unsigned int i=0; i < ACS_APBM_ARRAY_SIZE(tmp); i++)
                        ret += ::sprintf(value + ret, "%02X ", *tmp++);
                    ACS_APBM_LOG(LOG_LEVEL_DEBUG,"FOUND FATAL EVENT LOG STRING: %s", value);
                    fatalEventLogInfo[0] = fatalEventLogInfo[acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE] = 0;
                    ::memcpy(fatalEventLogInfo, value, acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE);
                } else ACS_APBM_LOG(LOG_LEVEL_ERROR,
                      "The PDU contains a board FATAL EVENT LOG info but the caller did not requested it");
        } else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected FATAL EVENT LOG info value");

        return 1;
}

int acs_apbm_getmanagerIpAddresscallback::read_scbrp (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading manager IP address from SCBRP");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCB_OID_XSHMC_MANAGER_IP_ADDRESS, 0 };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getmanagerIpAddresscallback::read_scx (netsnmp_variable_list * /*var_list*/) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading manager IP address from SCX or SMX");
/*
 //TODO
	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCX_OID_SHELF_MGR_CTRL_STATE, 0 };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
	*/
	return 0;
}

int acs_apbm_getmanagerIpAddresscallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Reading the following manager IP address info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type, /*is_cba ? ASN_OCTET_STR :*/ ASN_APPLICATION)) {
			if (var->type == 0x81 && var->val_len == 0){
				ACS_APBM_LOG(LOG_LEVEL_INFO, "The received PDU contains an empty manager IP address value");
				return (error_code = acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_NULL_SIZE);
			}
			ACS_APBM_LOG(LOG_LEVEL_WARN, "The received PDU has a bad type <type ==  %d>\n ", var->type);
			return 1;
		}
		if (managerIpaddr){
			const unsigned char * tmp = reinterpret_cast<unsigned char *>(var->val.string);
			char value[acs_apbm_snmp::SNMP_IPADDRESS_FIELD_SIZE + 1] = {0};
			for(unsigned int i=0; i < acs_apbm_snmp::SNMP_IPADDRESS_FIELD_SIZE && i < var->val_len; i++)
					managerIpaddr[i] = tmp[i];
			snprintf(value, var->val_len + 1, "%s", var->val.string);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"FOUND MANAGER IP ADDRESS");

		}
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The PDU contains a manager IP address info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected manager IP address OID");

	return 1;
}

int acs_apbm_getHWIGenericCallback::read_scbrp (netsnmp_variable_list * /*var_list*/) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading board IPMI data from SCBRP");
	//NOT USED
	return 0;

}

int acs_apbm_getHWIGenericCallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reading H data from SCX or SMX");

	for (netsnmp_variable_list * var = var_list; var; var = var->next_variable) {

		if (isHexStr) {
			char  hexBuf[SNMP_MAXBUF];
			snprint_hexstring( hexBuf, SNMP_MAXBUF, var->val.string, var->val_len);
			::memcpy(hwiOutValue, hexBuf, hwiOutValueSize);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP value from SCX, HEXSTR: %s", hwiOutValue);
		}
		else {
			char  Buf[SNMP_MAXBUF];
			if (var->type == ASN_INTEGER) {
				snprintf(Buf, var->val_len + 1, "%d", (int) (*var->val.integer));
			} else {
				snprintf(Buf, var->val_len + 1, "%s", var->val.string);
			}
			::memcpy(hwiOutValue, Buf, hwiOutValueSize);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP value from SCX, STR or INT: %s", hwiOutValue);
		}
	}
	return 1;
}
int acs_apbm_getscxfrontpartnerportstatecallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Lag partner port:Reading switch board Master/Slave status from SCX");

	// Arrange the oid names
	const oid oid_name [] = { ACS_APBM_SCX_OID_LAG_PARTNER_PORT_ACTOR_STATE, board_slot};

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getscxfrontpartnerportstatecallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "Lag partner port:Reading the following switch board Master/Slave status info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type, ASN_OCTET_STR)) return 1;

		if (partnerPortstate) *partnerPortstate = static_cast<int>(*var->val.string);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "Lag partner port:The PDU contains a Master/Slave status info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "Lag partner port:The received PDU does not contain the expected Master/Slave status value");

	return 1;
}
/*
* Callback function to read the PORT Status of front port from the mib for SCX boards.
*/
int acs_apbm_getadminstatefrontportstatuscallback::read_scx (netsnmp_variable_list * var_list) const {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG startup: Reading PORT status from SCX");

	// Arrange the oid names
	const oid oid_name [] = { IF_ADMIN_STATUS, board_slot };

	return read_value(oid_name, ACS_APBM_ARRAY_SIZE(oid_name), var_list);
}

int acs_apbm_getadminstatefrontportstatuscallback::read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const {
	char tmp_str [2 * 1024];

	if (acs_apbm_snmpmanager::oid_compare(oid_name, oid_names_size, var->name, var->name_length) == 0) {
		int char_printed = ::snprintf(tmp_str, ACS_APBM_ARRAY_SIZE(tmp_str), "LAG startup: Reading the following POER status info: NET-SNMP variable printout --> ");
		snprint_variable(tmp_str + char_printed, ACS_APBM_ARRAY_SIZE(tmp_str) - char_printed, var->name, var->name_length, var);
		ACS_APBM_LOG(LOG_LEVEL_INFO, tmp_str);

		if (check_pdu_var_type(var->type, ASN_INTEGER)) return 1;

		if (port_status) *port_status = static_cast<acs_apbm_snmp::frontPort_status_t>(*var->val.integer);
		else ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"The PDU contains a PORT status info but the caller did not requested it");
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received PDU does not contain the expected PORT status value");

	return 1;
}
