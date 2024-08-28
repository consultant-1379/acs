#include <string>
#include <vector>

#include <ace/Guard_T.h>

#include "acs_apbm_logger.h"
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_programconstants.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_action_timer.h"
#include "acs_apbm_immrepositoryhandler.h"


namespace {
/*
	const char * ACS_APBM_IMM_CLASS_NAMES [] = {
		ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO,
		ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO,
		ACS_APBM_CONFIG_IMM_CLASS_NAME_NIC_INFO,
		ACS_APBM_CONFIG_IMM_CLASS_NAME_OPERATIONAL_LED,
		ACS_APBM_CONFIG_IMM_CLASS_NAME_MAINTENANCE_LED,
		ACS_APBM_CONFIG_IMM_CLASS_NAME_FAULT_LED,
		ACS_APBM_CONFIG_IMM_CLASS_NAME_STATUS_LED
	};
*/
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_apbm_immactionresult

/*OK ACTION*/
const char *	__CLASS_NAME__::imm_action_exit_code_no_error = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"OK";
const char *	__CLASS_NAME__::imm_action_exit_code_error_not_recognized = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Error not recognized!";
/* GENERAL ACTION */
const char *	__CLASS_NAME__::imm_action_exit_code_error_executing = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Error when executing ( general fault )";
const char *	__CLASS_NAME__::imm_action_exit_code_not_available = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Blade not present";
const char *	__CLASS_NAME__::imm_action_exit_code_server_not_responding = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Service not responding";
const char *  __CLASS_NAME__::imm_action_exit_code_invalid_platform = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"The command is not valid for this platform.";
const char *  __CLASS_NAME__::imm_action_exit_code_operation_ongoing = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Another operation ongoing";

/* BLOCK ACTION */
const char *	__CLASS_NAME__::imm_action_exit_code_error_board_already_blocked = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Blade already blocked";
const char *	__CLASS_NAME__::imm_action_exit_code_error_blocking_last = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Last working DISK can not be blocked";
const char *	__CLASS_NAME__::imm_action_exit_code_error_own_slot_block = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Active APUB cannot be blocked";
const char *	__CLASS_NAME__::imm_action_exit_code_error_raid_status_faulty = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"RAID status of both the disk's are faulty";

/* DEBLOCK ACTION */
const char *	__CLASS_NAME__::imm_action_exit_code_error_board_already_deblocked = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Blade already deblocked";

/* RESET ACTION */
const char *	__CLASS_NAME__::imm_action_exit_code_error_own_slot_reset = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Active APUB cannot be reset";
const char *	__CLASS_NAME__::imm_action_exit_code_error_reset_blocked_board = ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER"Cannot reset blocked blade";

int __CLASS_NAME__::get_general_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text){

	switch(operation_exit_code)
	{
	case IMM_ACTION_EXIT_CODE_NO_ERROR:
		strncpy(exit_code_text, imm_action_exit_code_no_error, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIT_CODE_ERROR_EXECUTING:
		strncpy(exit_code_text, imm_action_exit_code_error_executing, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIT_CODE_ERROR_SERVER_NOT_RESPONDING:
		strncpy(exit_code_text, imm_action_exit_code_server_not_responding, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIX_CODE_ERROR_BOARD_NOT_AVAILABLE:
			strncpy(exit_code_text, imm_action_exit_code_not_available, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
			break;
	case IMM_ACTION_EXIT_CODE_ERROR_INVALID_PLATFORM:
		strncpy(exit_code_text, imm_action_exit_code_invalid_platform, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIT_CODE_ERROR_OPERATION_ONGOING:
		strncpy(exit_code_text, imm_action_exit_code_operation_ongoing, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	default:
		//strncpy(exit_code_text, imm_action_exit_code_error_not_recognized, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		return 1;
		break;
	}
	return 0;
}

int __CLASS_NAME__::get_block_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text ){


	if(get_general_action_exit_code_text(operation_exit_code, exit_code_text) == 0)
		return 0;

	switch(operation_exit_code)
	{
	case IMM_ACTION_EXIT_CODE_ERROR_BOARD_ALREADY_BLOCKED:
		strncpy(exit_code_text, imm_action_exit_code_error_board_already_blocked, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIT_CODE_ERROR_BLOCKING_LAST_DISK:
		strncpy(exit_code_text, imm_action_exit_code_error_blocking_last, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIT_CODE_ERROR_OWN_SLOT_BLOCK:
		strncpy(exit_code_text, imm_action_exit_code_error_own_slot_block, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIT_CODE_ERROR_RAID_STATUS_FAULTY:
		strncpy(exit_code_text, imm_action_exit_code_error_raid_status_faulty, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	default:
		strncpy(exit_code_text, imm_action_exit_code_error_executing, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		//strncpy(exit_code_text, imm_action_exit_code_error_not_recognized, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		return IMM_ACTION_EXIT_CODE_NOT_RECOGNIZED;
	}
	return 0;
}

int __CLASS_NAME__::get_deblock_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text){

	if(get_general_action_exit_code_text(operation_exit_code, exit_code_text) == 0)
		return 0;

	switch(operation_exit_code)
	{
	case IMM_ACTION_EXIT_CODE_ERROR_BOARD_ALREADY_DEBLOCKED:
		strncpy(exit_code_text, imm_action_exit_code_error_board_already_deblocked, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	default:
		//strncpy(exit_code_text, imm_action_exit_code_error_not_recognized, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		strncpy(exit_code_text, imm_action_exit_code_error_executing, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		return IMM_ACTION_EXIT_CODE_NOT_RECOGNIZED;
	}
	return 0;
}

int __CLASS_NAME__::get_reset_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text){

	if(get_general_action_exit_code_text(operation_exit_code, exit_code_text) == 0)
		return 0;

	switch(operation_exit_code)
	{
	case IMM_ACTION_EXIT_CODE_ERROR_OWN_SLOT_RESET:
		strncpy(exit_code_text, imm_action_exit_code_error_own_slot_reset, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	case IMM_ACTION_EXIT_CODE_ERROR_RESET_BLOCKED_BOARD:
		strncpy(exit_code_text, imm_action_exit_code_error_reset_blocked_board, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		break;
	default:
		strncpy(exit_code_text, imm_action_exit_code_error_executing, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		//strncpy(exit_code_text, imm_action_exit_code_error_not_recognized,  ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		return IMM_ACTION_EXIT_CODE_NOT_RECOGNIZED;
	}
	return 0;
}

int __CLASS_NAME__::get_exit_code_text(const ACS_APGCC_AdminOperationIdType operation_id, const int16_t operation_exit_code, char * exit_code_text){

	switch(operation_id){

	case IMM_ACTION_APBOARD_BLOCK:
		get_block_action_exit_code_text(operation_exit_code, exit_code_text);
		break;
	case IMM_ACTION_APBOARD_DEBLOCK:
		get_deblock_action_exit_code_text(operation_exit_code, exit_code_text);
		break;
	case IMM_ACTION_APBOARD_RESET:
		get_reset_action_exit_code_text(operation_exit_code, exit_code_text);
		break;
	case IMM_ACTION_APBOARD_SETWATCHDOG:
		return IMM_ACTION_EXIT_CODE_NOT_IMPLEMENTED;
	case IMM_ACTION_APBOARD_IPMIUPGRADE_LOCAL:
		return IMM_ACTION_EXIT_CODE_NOT_IMPLEMENTED;
	case IMM_ACTION_APBOARD_IPMIUPGRADE_SNMP:
		return IMM_ACTION_EXIT_CODE_NOT_IMPLEMENTED;
	case IMM_ACTION_APBOARD_IPMIUPGRADE_STATUS:
		return IMM_ACTION_EXIT_CODE_NOT_IMPLEMENTED;
	default:
		strncpy(exit_code_text, imm_action_exit_code_error_executing, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		//strncpy(exit_code_text, imm_action_exit_code_error_not_recognized, ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE);
		return IMM_ACTION_EXIT_CODE_NOT_RECOGNIZED;
	}
	//ACS_APBM_LOG(LOG_LEVEL_ERROR, "TEST:%s exit_code=%d",exit_code_text,operation_exit_code);
	return 0;
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_apbm_immrepositoryhandler


#ifdef ATTRIBUTE_HANDLER
	const char * __CLASS_NAME__::ap_blade_hardware_info_id_attr_name = ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID;

	//IMM CLASS - productInfo
	const char * __CLASS_NAME__::product_info_attr_name = "productInfo";
	const char * __CLASS_NAME__::product_info_id_attr_name = "id";
	const char * __CLASS_NAME__::product_revision_attr_name = "productRevision";
	const char * __CLASS_NAME__::product_name_att_name = "productName";
	const char * __CLASS_NAME__::product_number_attr_name = "productNumber";
	const char * __CLASS_NAME__::serial_number_attr_name = "serialNumber";
	const char * __CLASS_NAME__::manufacturing_date_attr_name = "manufacturingDate";
	const char * __CLASS_NAME__::vendor_attr_name = "productVendor";

	//IMM CLASS - nicInfo
	const char * __CLASS_NAME__::nic_info_attr_name = "nicInfo";
	const char * __CLASS_NAME__::nic_info_id_attr_name = "id";
	const char * __CLASS_NAME__::nic_name_attr_name = "name";
	const char * __CLASS_NAME__::nic_status_attr_name = "status";//nic status
	const char * __CLASS_NAME__::nic_mac_address_attr_name = "macAddress";
	const char * __CLASS_NAME__::nic_ip_address_attr_name = "ipAddress";

	//IMM CLASS - OperationalLed, StatusLed, FaultLed, MaintenanceLed
	const char * __CLASS_NAME__::operational_led_attr_name = "operationalLed";
	const char * __CLASS_NAME__::fault_led_attr_name = "faultLed";
	const char * __CLASS_NAME__::maintenance_led_attr_name = "maintenanceLed";
	const char * __CLASS_NAME__::status_led_attr_name = "statusLed";
	const char * __CLASS_NAME__::led_id_attr_name = "id";
	const char * __CLASS_NAME__::led_color_attr_name = "colour"; //led colour
	const char * __CLASS_NAME__::led_status_attr_name = "status"; //led status
	const char * __CLASS_NAME__::led_supported_attr_name = "supported";

	const char * __CLASS_NAME__::ipmi_firmware_type_attr_name = "ipmiFirmwareType";
	const char * __CLASS_NAME__::ipmi_product_number_attr_name = "ipmiProductNumber";
	const char * __CLASS_NAME__::ipmi_product_revision_name = "ipmiProductRevision";
	const char * __CLASS_NAME__::ipmi_upgrade_result_attr_name = "ipmiUpgradeResult";
	const char * __CLASS_NAME__::ipmi_upgrade_time_attr_name = "ipmiUpgradeTime";
	const char * __CLASS_NAME__::bios_running_mode_attr_name = "biosRunningMode";

	const char * __CLASS_NAME__::fatal_event_log_info_name = "fatalEvent";
	const char * __CLASS_NAME__::temperature_status_attr_name = "temperatureStatus";
	const char * __CLASS_NAME__::alarm_interface_status_attr_name = "alarmInterfaceStatus";
	const char * __CLASS_NAME__::raid_disk_status_attr_name = "storageStatus";
	const char * __CLASS_NAME__::drbd_disk_status_attr_name = "storageStatus";
	const char * __CLASS_NAME__::board_status_attr_name = "boardStatus";
	const char * __CLASS_NAME__::bios_version_attr_name = "biosVersion";

#endif

// initialization of static members representing IMM Action Parameter names
const char * __CLASS_NAME__::IPMIUPGRADE_LOCAL_PATH_PARAM_NAME = "path";
const char * __CLASS_NAME__::IPMIUPGRADE_LOCAL_PORT_PARAM_NAME = "port";
const char * __CLASS_NAME__::IPMIUPGRADE_SNMP_PATH_PARAM_NAME = "path";
const char * __CLASS_NAME__::IPMIUPGADE_DEFAULT_DEVICE = "eri_ipmi";

// initialization of static members representing IMM Action parameters

// IPMIUPGRADE_LOCAL Action parameters
static __CLASS_NAME__::imm_action_parameter IPMIUPGRADE_LOCAL_PATH_PARAMETER = {  __CLASS_NAME__::IPMIUPGRADE_LOCAL_PATH_PARAM_NAME, ATTR_STRINGT };
static __CLASS_NAME__::imm_action_parameter IPMIUPGRADE_LOCAL_PORT_PARAMETER = {  __CLASS_NAME__::IPMIUPGRADE_LOCAL_PORT_PARAM_NAME, ATTR_STRINGT };
static __CLASS_NAME__::imm_action_parameter IPMIUPGRADE_LOCAL_ACTION_PARAMS[] = { IPMIUPGRADE_LOCAL_PATH_PARAMETER, IPMIUPGRADE_LOCAL_PORT_PARAMETER };

// IPMIUPGRADE_SNMP Action parameters
static __CLASS_NAME__::imm_action_parameter IPMIUPGRADE_SNMP_PATH_PARAMETER = {  __CLASS_NAME__::IPMIUPGRADE_SNMP_PATH_PARAM_NAME, ATTR_STRINGT };
static __CLASS_NAME__::imm_action_parameter IPMIUPGRADE_SNMP_ACTION_PARAMS[] = { IPMIUPGRADE_SNMP_PATH_PARAMETER};


// initialization of static member 'acs_apbm_immrepository::imm_actions'
__CLASS_NAME__::imm_action_signature  __CLASS_NAME__::imm_actions[] =
{
	{IMM_ACTION_APBOARD_BLOCK, 0,  0},
	{IMM_ACTION_APBOARD_DEBLOCK, 0, 0},
	{IMM_ACTION_APBOARD_RESET, 0, 0},
	{IMM_ACTION_APBOARD_SETWATCHDOG, 0, 0},
	{IMM_ACTION_APBOARD_IPMIUPGRADE_LOCAL, 2, IPMIUPGRADE_LOCAL_ACTION_PARAMS},
	{IMM_ACTION_APBOARD_IPMIUPGRADE_SNMP, 1, IPMIUPGRADE_SNMP_ACTION_PARAMS},
};

int __CLASS_NAME__::_check_parameter(int32_t slot, uint32_t magazine){
	if (slot == acs_apbm::BOARD_SLOT_UNDEFINED){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "SLOT_POSITION == '%d' ", slot);
		return acs_apbm::ERR_IMM_SLOT_POSITION_NOT_FOUND;
	}
	if ( (slot <= 0) || (slot >= 25) ){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "INVALID SLOT_POSITION == '%d'", slot);
		return acs_apbm::ERR_IMM_SLOT_POSITION_NOT_FOUND;
	}
	if (magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "MAGAZINE == '%d' ", magazine);
		return acs_apbm::ERR_IMM_MAGAZINE_NOT_FOUND;
	}
	return 0;
}

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND

int __CLASS_NAME__::update_osCaching (int caching)
{
	int  valueInIMM;

	if (get_osCaching (valueInIMM) != 0)  return  acs_apbm::ERR_IMM_GET_ATTRIBUTE;

	if (valueInIMM == caching)
	{
			ACS_APBM_LOG (LOG_LEVEL_WARN, "update_osCaching :  the setting value is already present in IMM");
			return  acs_apbm::ERR_NO_ERRORS;
	}

	ACS_CC_ImmParameter param2modify;

	void  *attr_value [1] =  {reinterpret_cast<void *>(& caching)};

	param2modify.attrName      = const_cast<char*> (ACS_ACA_CONFIG_CACHING_ENABLED);
	param2modify.attrType      = (ACS_CC_AttrValueType) IMM_ATTRIBUTE_TYPE_INTEGER;
	param2modify.attrValuesNum = SINGLE_ATTRIBUTE;
	param2modify.attrValues    = attr_value;

	if (_om_handler.modifyAttribute (ACS_ACA_RECORD_DATA_MOM_NAME, &param2modify) != ACS_CC_SUCCESS)
	{
				int  imm_error_code;
				char  *imm_error_text = 0;

				imm_error_code = _om_handler.getInternalLastError();
				imm_error_text = _om_handler.getInternalLastErrorText();

				ACS_APBM_LOG (LOG_LEVEL_ERROR, "Internal IMM error in update_osCaching :  error-test: %s  error: %d", imm_error_text, imm_error_code);
				return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}

	return acs_apbm::ERR_NO_ERRORS;
}


int __CLASS_NAME__::get_osCaching (int & caching)
{
		ACS_CC_ImmParameter  retStatus;

		retStatus.attrName = (char *) ACS_ACA_CONFIG_CACHING_ENABLED;

		if (_om_handler.getAttribute (ACS_ACA_RECORD_DATA_MOM_NAME, &retStatus) != ACS_CC_SUCCESS)
		{
				int  imm_error_code;
				char  *imm_error_text = 0;

				imm_error_code = _om_handler.getInternalLastError();
				imm_error_text = _om_handler.getInternalLastErrorText();
				ACS_APBM_LOG (LOG_LEVEL_ERROR, "Internal IMM error in get_osCaching :  error-test: %s  error: %d", imm_error_text, imm_error_code);
				return  -1;
		}

		caching = *(reinterpret_cast<int*>(retStatus.attrValues[0]));
		ACS_APBM_LOG (LOG_LEVEL_INFO, "Read ACA caching value :  osCachingEnabled == %d ", caching);
		return  0;
}

#endif

int __CLASS_NAME__::get_magazine_and_slot_from_DN(uint32_t & magazine, int32_t & slot, const char *imm_dn)
{
	if(!imm_dn || !*imm_dn)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid DN !");
		return -1;  // invalid DN passed !
	}

	char mag_str[16] = {0};
	int32_t apBladeHardwareInfoId, tmp_slot;
	char tmp_str[512] = {0};

	int sscanf_res = sscanf(imm_dn, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d,"ACS_APBM_CONFIG_AP_BLADE_CLASS_ID"=%d,"ACS_APBM_CONFIG_MAGAZINE_CLASS_ID"=%s", & apBladeHardwareInfoId, & tmp_slot, tmp_str);
	if(sscanf_res != 3)
	{
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Error parsing DN using 'sscanf(..)' ! call_result == %d.", sscanf_res);
		return -2;	// sscanf error. Something wrong in the passed DN !
	}

	char * p = index(tmp_str, ',');
	if(!p)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid DN: cannot retrieve MAGAZINE string. Details: comma character not found ! ");
		return -3;	// expected comma character not found !
	}

	size_t n_bytes_to_copy = p - tmp_str;
	if(n_bytes_to_copy > sizeof(mag_str) - 1)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "MAGAZINE string found in the DN is too long (len == %ul). It will be trunked to %d characters ! ", n_bytes_to_copy, sizeof(mag_str) - 1);
		n_bytes_to_copy = sizeof(mag_str) - 1;
	}
	memcpy(mag_str, tmp_str, n_bytes_to_copy);

/*
	// this code is temporary ! We ignore parsed MAGAZINE string and assume a fixed MAGAZINE string: we do so because in IMM the magazine string is NOT yet in IP format
	if(strncmp(mag_str, ACS_APBM_DEFAULT_MAGAZINE_STR, sizeof(mag_str)) != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "in the DN, <ShelfId> is '%s', but we ignore such value and assume that <ShelfId> == '%s' ", mag_str, ACS_APBM_DEFAULT_MAGAZINE_STR);
		strncpy(mag_str, ACS_APBM_DEFAULT_MAGAZINE_STR, sizeof(mag_str));
		mag_str[sizeof(mag_str)-1] = 0;
	}
*/
	// substitute IMM magazine separator with the character defined by ACS_APBM_MAGAZINE_STRING_SEPARATOR
	char sep = acs_apbm_programconfiguration::magazine_address_imm_separator;
	for(p = strchr(mag_str, sep); p; p = strchr(p+1, sep))
		*p = ACS_APBM_MAGAZINE_STRING_SEPARATOR;

	//  convert MAGAZINE string to network format
	uint32_t tmp_magazine;
	int call_result = acs_apbm_csreader::ip_format_to_uint32(tmp_magazine, mag_str);
	if(call_result != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error converting MAGAZINE string (%s) to network format ! call_result == %d.", mag_str, call_result);
		return -4;
	}

	slot = tmp_slot;
	magazine = tmp_magazine;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Parsed values: magazine == 0x%08X, slot == %d ", tmp_magazine, tmp_slot);

	return 0;
}


int __CLASS_NAME__::build_DN_from_magazine_and_slot(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot, uint32_t magazine)
{
	char tmp_magazine_str[16] = {0};

	if(int call_result = acs_apbm_csreader::uint32_to_ip_format(tmp_magazine_str, magazine, acs_apbm_programconfiguration::magazine_address_imm_separator))
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error building MAGAZINE string in IP format. magazine == = 0x%08X, call_result == %d", magazine, call_result);
		return -1;
	}

	::snprintf(dn, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d,"ACS_APBM_CONFIG_AP_BLADE_CLASS_ID"=%d,"ACS_APBM_CONFIG_MAGAZINE_CLASS_ID"=%s,%s", slot, slot, tmp_magazine_str, blade_configuration_dn());
	return 0;
}

int __CLASS_NAME__::build_DN_from_magazine_slot_and_led_type(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot, uint32_t magazine, axe_eth_shelf::led_type_t led)
{
	char tmp_magazine_str[16] = {0};

	if(int call_result = acs_apbm_csreader::uint32_to_ip_format(tmp_magazine_str, magazine, acs_apbm_programconfiguration::magazine_address_imm_separator))
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error building MAGAZINE string in IP format. magazine == = 0x%08X, call_result == %d", magazine, call_result);
		return -1;
	}

	char led_type_str[32] = {0};

	switch(led){
	case axe_eth_shelf::LED_TYPE_FAULT :
			::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_FAULT_LED);
			break;
	case axe_eth_shelf::LED_TYPE_MAINTENANCE :
		::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_MAINTENANCE_LED);
			break;
	case axe_eth_shelf::LED_TYPE_OPERATIONAL :
		::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_OPERATIONAL_LED);
			break;
	case axe_eth_shelf::LED_TYPE_STATUS :
		::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_STATUS_LED);
			break;
	}

	::snprintf(dn, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, "%s=%s,"ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d,"ACS_APBM_CONFIG_AP_BLADE_CLASS_ID"=%d,"ACS_APBM_CONFIG_MAGAZINE_CLASS_ID"=%s,%s",led_id_attr_name, led_type_str, slot, slot, tmp_magazine_str, blade_configuration_dn());
	return 0;
}

int __CLASS_NAME__::build_DN_from_magazine_slot_and_product_info(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot, uint32_t magazine, const char * product_struct_name){

	char tmp_magazine_str[16] = {0};

	if(int call_result = acs_apbm_csreader::uint32_to_ip_format(tmp_magazine_str, magazine, acs_apbm_programconfiguration::magazine_address_imm_separator))
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error building MAGAZINE string in IP format. magazine == = 0x%08X, call_result == %d", magazine, call_result);
		return -1;
	}

	::snprintf(dn, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, "%s=%s,"ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d,"ACS_APBM_CONFIG_AP_BLADE_CLASS_ID"=%d,"ACS_APBM_CONFIG_MAGAZINE_CLASS_ID"=%s,%s", product_info_id_attr_name, product_struct_name, slot, slot, tmp_magazine_str, blade_configuration_dn());
	return 0;
}

int __CLASS_NAME__::build_DN_from_magazine_slot_and_nic_info(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot, uint32_t magazine, axe_eth_shelf::nic_name_t nic){

	char tmp_magazine_str[16] = {0};

	if(int call_result = acs_apbm_csreader::uint32_to_ip_format(tmp_magazine_str, magazine, acs_apbm_programconfiguration::magazine_address_imm_separator))
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error building MAGAZINE string in IP format. magazine == = 0x%08X, call_result == %d", magazine, call_result);
		return -1;
	}

	char nic_str[32] = {0};
	switch(nic){
	case axe_eth_shelf::IMM_NIC_0 :
			::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_0);
			break;
	case axe_eth_shelf::IMM_NIC_1 :
			::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_1);
				break;
	case axe_eth_shelf::IMM_NIC_2 :
			::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_2);
				break;
	case axe_eth_shelf::IMM_NIC_3 :
			::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_3);
				break;
	case axe_eth_shelf::IMM_NIC_4 :
			::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_4);
				break;
	//ETH5 and ETH6 SUPPORT
	case axe_eth_shelf::IMM_NIC_5 :
                        ::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_5);
                                break;
	case axe_eth_shelf::IMM_NIC_6 :
                        ::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_6);
                                break;
    // ETH7 and ETH8 SUPPORT
	case axe_eth_shelf::IMM_NIC_7 :
		::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_7);
		break;
	case axe_eth_shelf::IMM_NIC_8 :
		::snprintf(nic_str, 32, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_8);
		break;

	default:
		break;
	}

	::snprintf(dn, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, "%s=%s,"ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d,"ACS_APBM_CONFIG_AP_BLADE_CLASS_ID"=%d,"ACS_APBM_CONFIG_MAGAZINE_CLASS_ID"=%s,%s", nic_info_id_attr_name, nic_str, slot, slot, tmp_magazine_str, blade_configuration_dn());
	return 0;

}

int __CLASS_NAME__::retrieve_led_info_from_ap_board_by_led_type(axe_eth_shelf::led_color_t & color, axe_eth_shelf::led_status_t & status,
		bool & supported, const acs_apbm_apboardinfo & board, const axe_eth_shelf::led_type_t led_type){

	switch(led_type){

	case axe_eth_shelf::LED_TYPE_FAULT :
		color = axe_eth_shelf::LED_COLOR_RED;
		status = board.red_led;
		supported = board.red_led_supported;
		break;
	case axe_eth_shelf::LED_TYPE_MAINTENANCE :
                if(_server_working_set->shelves_data_manager->is_4led_supported || _server_working_set->shelves_data_manager->is_new_gea_supported) 
		{
			color = axe_eth_shelf::LED_COLOR_BLUE;
			status = board.blue_led;
			supported = board.blue_led_supported;
                }
		else
		{
			color = axe_eth_shelf::LED_COLOR_YELLOW;
	                status = board.mia_led;
        	        supported = board.mia_led_supported;
		}
		break;
	case axe_eth_shelf::LED_TYPE_OPERATIONAL :
		color = axe_eth_shelf::LED_COLOR_GREEN;
		status = board.green_led;
		supported = board.green_led_supported;
		break;
        case axe_eth_shelf::LED_TYPE_STATUS : // only for 4 LEDs hardware
		color = axe_eth_shelf::LED_COLOR_YELLOW;
		status = board.mia_led;
		supported = board.mia_led_supported;
		break;
        default:
                ACS_APBM_LOG(LOG_LEVEL_ERROR,"Invalid led type=%d", led_type);
	        return -1;
	}
ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Values retrieved from led: color=%d, status=%d, supported=%d", color, status, supported);
	return 0;
}

int __CLASS_NAME__::retrieve_nic_info_from_ap_board_by_nic_name(char * nic_name, size_t name_size, axe_eth_shelf::nic_status_t & status, char * mac_address,
																								size_t mac_size, char * ip_address,size_t ip_size, const acs_apbm_apboardinfo & board, const axe_eth_shelf::nic_name_t nic_type){

	if(!board.ip_address || !board.mac_address)
		return acs_apbm::ERR_IMM_CREATE_OBJ;

	switch(nic_type){
	case axe_eth_shelf::IMM_NIC_0 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_0);
			status = board.nic_0;
			break;
	case axe_eth_shelf::IMM_NIC_1 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_1);
			status = board.nic_1;
			break;
	case axe_eth_shelf::IMM_NIC_2 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_2);
			status = board.nic_2;
			break;
	case axe_eth_shelf::IMM_NIC_3 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_3);
			status = board.nic_3;
				break;
	case axe_eth_shelf::IMM_NIC_4 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_4);
			status = board.nic_4;
			break;
	//ETH5 and ETH6 SUPPORT
	case axe_eth_shelf::IMM_NIC_5 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_5);
			status = board.nic_5;
			break;
	case axe_eth_shelf::IMM_NIC_6 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_6);
			status = board.nic_6;
			break;
	//ETH7 and ETH8 SUPPORT
	case axe_eth_shelf::IMM_NIC_7 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_7);
			status = board.nic_7;
			break;
	case axe_eth_shelf::IMM_NIC_8 :
			::snprintf(nic_name, name_size, ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE"%d", axe_eth_shelf::IMM_NIC_8);
			status = board.nic_8;
			break;

	default:
		break;

}

	::snprintf(mac_address, mac_size, board.mac_address);
	::snprintf(ip_address, ip_size, board.ip_address);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Retrieved nic values: nic_name=%s, nic status=%d, mac_address=%s, ip_address=%s", nic_name, status, mac_address, ip_address);
	return 0;
}

void __CLASS_NAME__::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** /*paramList*/)
{
	ACS_CC_ReturnType imm_res = ACS_CC_SUCCESS;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "IMM Action requested: oiHandle == %llu, invocation == %llu, p_objName == '%s' , operationId == %llu", oiHandle, invocation, p_objName, operationId);

	// Parse DN to get MAGAZINE number and SLOT number
	uint32_t magazine;
	int32_t slot;
	char exit_code_text[ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE] = {0};
	ACS_APGCC_AdminOperationParamType param;
	vector<ACS_APGCC_AdminOperationParamType> outParamVector;
	param.attrName = const_cast<char*>("GENERIC");
	param.attrType = STRING_ATTRIBUTE;

	int call_result = get_magazine_and_slot_from_DN(magazine, slot, p_objName);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error executing IMM Action: Call 'get_magazine_and_slot_from_DN(..)' failed ! call_result == %d !", call_result);
		// set operation result
		acs_apbm_immactionresult::get_exit_code_text(operationId, IMM_ACTION_EXIT_CODE_ERROR_EXECUTING, exit_code_text);
		param.attrValues = reinterpret_cast<void *>(const_cast<char*>(exit_code_text));
		outParamVector.push_back(param);
		imm_res = acs_apgcc_objectimplementerinterface_V3::adminOperationResult(oiHandle, invocation, imm_res, outParamVector);

		if(imm_res != ACS_CC_SUCCESS)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'acs_apgcc_objectimplementerinterface_V3::adminOperationResult(..)' failed ! call_result == %d, imm last error == %d, imm last error text == '%s'",
				imm_res, acs_apgcc_objectimplementerinterface_V3::getInternalLastError(), acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());
		return;
	}

	// check <operationId> argument (we expect a valid IMM ACTION ID)
	if(/*operationId < IMM_ACTION_MIN_ID  ||*/ operationId > IMM_ACTION_MAX_ID)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error executing IMM Action: invalid ACTION ID (action_id == %llu) !", operationId);
		// set operation result
		acs_apbm_immactionresult::get_exit_code_text(operationId, IMM_ACTION_EXIT_CODE_ERROR_EXECUTING, exit_code_text);
		param.attrValues = reinterpret_cast<void *>(const_cast<char*>(exit_code_text));
		outParamVector.push_back(param);
		imm_res = acs_apgcc_objectimplementerinterface_V3::adminOperationResult(oiHandle, invocation, imm_res, outParamVector);
		if(imm_res != ACS_CC_SUCCESS)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'acs_apgcc_objectimplementerinterface_V3::adminOperationResult(..)' failed ! call_result == %d, imm last error == %d, imm last error text == '%s'",
				imm_res, acs_apgcc_objectimplementerinterface_V3::getInternalLastError(), acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());
		return;
	}

	ACS_APBM_CRITICAL_SECTION_GUARD_BEGIN(_pending_action_timers_mutex);
		// check for pending actions on the same IMM object
		//ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_pending_action_timers_mutex);
		PendingActionTimersMap::iterator it = _pending_action_timers.find(p_objName);
		if(it != _pending_action_timers.end())
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error executing IMM Action: there's a pending action [%d] on DN %s!", it->second->operation_id(), p_objName);
			// set operation result
			acs_apbm_immactionresult::get_exit_code_text(operationId, IMM_ACTION_EXIT_CODE_ERROR_OPERATION_ONGOING, exit_code_text);

			param.attrValues = reinterpret_cast<void *>(const_cast<char*>(exit_code_text));
			outParamVector.push_back(param);

			imm_res = acs_apgcc_objectimplementerinterface_V3::adminOperationResult(oiHandle, invocation, imm_res, outParamVector);
			if(imm_res != ACS_CC_SUCCESS)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'acs_apgcc_objectimplementerinterface_V3::adminOperationResult(..)' failed ! call_result == %d, imm last error == %d, imm last error text == '%s'",
					imm_res, acs_apgcc_objectimplementerinterface_V3::getInternalLastError(), acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());
			return;
		}
		//guard.release();
	ACS_APBM_CRITICAL_SECTION_GUARD_END();

	// now we have the coordinates of the AP board ( magazine number and slot number ). Invoke on the AP board the requested operation
	int action_result = SA_AIS_OK;
	switch(operationId)
	{
	case IMM_ACTION_APBOARD_BLOCK:
		action_result = handle_block_action(slot, magazine);
		break;
	case IMM_ACTION_APBOARD_DEBLOCK:
		action_result = handle_deblock_action(slot, magazine);
		break;
	case IMM_ACTION_APBOARD_IPMIUPGRADE_SNMP:
	case IMM_ACTION_APBOARD_IPMIUPGRADE_LOCAL:
	//	action_result = handle_ipmiupgrade_action(slot, magazine, p_objName, invocation, operationId, paramList);
		break;
	case IMM_ACTION_APBOARD_RESET:
	  action_result = handle_reset_action(slot, magazine);
	  break;
	default:
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "ATTENTION: the requested action has not yet been implemented !");
		action_result = SA_AIS_ERR_NOT_SUPPORTED;
		break;
	}
	acs_apbm_immactionresult::get_exit_code_text(operationId, action_result, exit_code_text);
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Action Result:%d - Action Text result:%s", action_result, exit_code_text);
	// set operation result
	if (action_result == IMM_ACTION_EXIT_CODE_NO_ERROR)
	{
		imm_res = ACS_CC_SUCCESS;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "OK.Setting result:%d, text:%s", action_result, exit_code_text);
		imm_res = acs_apgcc_objectimplementerinterface_V3::adminOperationResult(oiHandle, invocation, SA_AIS_OK);
	}
	else
	{
		imm_res = ACS_CC_FAILURE;
		param.attrValues = reinterpret_cast<void *>(const_cast<char*>(exit_code_text));
			outParamVector.push_back(param);
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Failed.Setting result:%d, text:%s", action_result, exit_code_text);
			imm_res = acs_apgcc_objectimplementerinterface_V3::adminOperationResult(oiHandle, invocation, SA_AIS_ERR_FAILED_OPERATION, outParamVector);
	}
	if(imm_res != ACS_CC_SUCCESS){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'acs_apgcc_objectimplementerinterface_V3::adminOperationResult(..)' failed ! call_result == %d, imm last error == %d, imm last error text == '%s'",
			imm_res, acs_apgcc_objectimplementerinterface_V3::getInternalLastError(), acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());
		return;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "IMM Action executed. Result == %d. ACTION text result: %s", action_result, exit_code_text);
}


int __CLASS_NAME__::open_management_session () {
	// Check if already open
	if (_imm_management_state != ACS_APBM_IMM_MANAGEMENT_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS; // Already open
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (_imm_management_state != ACS_APBM_IMM_MANAGEMENT_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS; // Already open

	if (_om_handler.Init() != ACS_CC_SUCCESS) {
		// ERROR initialilzing imm internal om handler
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal OM IMM handler init failure: imm last error == %d, imm last error text == '%s'",
				_om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
		return acs_apbm::ERR_OM_HANDLER_INIT_FAILURE;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Init...om_handler...ok!");
	_imm_management_state = ACS_APBM_IMM_MANAGEMENT_STATE_OPEN;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::close_management_session () {

	// Check if already closed
	if (_imm_management_state == ACS_APBM_IMM_MANAGEMENT_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS; // Already closed
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (_imm_management_state == ACS_APBM_IMM_MANAGEMENT_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS; // Already closed

	if (_om_handler.Finalize() != ACS_CC_SUCCESS) {
		// ERROR finalizing imm internal om handler
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal OM IMM handler finilize failure: imm last error == %d, imm last error text == '%s'",
				_om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
		return acs_apbm::ERR_OM_HANDLER_FINALIZE_FAILURE;
	}

	_imm_management_state = ACS_APBM_IMM_MANAGEMENT_STATE_CLOSED;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::take_ownership (std::string implementer_name) {
	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_EVENT_HANDLED) return acs_apbm::ERR_NO_ERRORS;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_EVENT_HANDLED) return acs_apbm::ERR_NO_ERRORS;

	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_CLOSED) {
		ACS_CC_ReturnType cc_call_result = ACS_CC_SUCCESS;

		cc_call_result = _oi_handler.addClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Taking ownership: Class Name == '%s', cc_call_result == %d",
				ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO, cc_call_result);

		if (cc_call_result != ACS_CC_SUCCESS) {
			// ERROR initialilzing imm internal runtime owner implementer handler
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Taking ownership: Call 'addClassImpl()' failed: "
					"imm error text == '%s', cc_call_result == %d",
					_oi_handler.getErrorText(cc_call_result).c_str(), cc_call_result);

			return acs_apbm::ERR_OI_HANDLER_INIT_FAILURE;
		}

		cc_call_result = init(implementer_name);

		if (cc_call_result != ACS_CC_SUCCESS) {
			// ERROR initialilzing imm internal runtime owner implementer handler
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Taking ownership: Call '"
					"init()' failed: imm last error == %d, imm last error text == '%s', cc_call_result == %d",
					ACS_APGCC_RuntimeOwner_V2::getInternalLastError(), ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText(),
					cc_call_result);

			_oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO);

			return acs_apbm::ERR_OI_HANDLER_INIT_FAILURE;
		}


/*
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
		std::vector<std::string> acs_apbm_imm_class_names_vector;

		for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(ACS_APBM_IMM_CLASS_NAMES)); i++)
			acs_apbm_imm_class_names_vector.push_back(ACS_APBM_IMM_CLASS_NAMES[i]);

		cc_call_result = _oi_handler.addMultipleClassImpl(this, acs_apbm_imm_class_names_vector);

		static_cast<void>(implementer_name);
#else
		cc_call_result = init(implementer_name);
#endif

		if (cc_call_result != ACS_CC_SUCCESS) {
			// ERROR initialilzing imm internal runtime owner implementer handler
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Taking ownership: Call '"
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
					"addClassImpl()"
#else
					"init()"
#endif
					"' failed: "
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
					"imm error text == '%s'"
#else
					"imm last error == %d, imm last error text == '%s'"
#endif
					", cc_call_result == %d",
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
					_oi_handler.getErrorText(cc_call_result).c_str()
#else
					getInternalLastError(), getInternalLastErrorText()
#endif
					, cc_call_result);

			return acs_apbm::ERR_OI_HANDLER_INIT_FAILURE;
		}
*/


		_imm_ownership_state = ACS_APBM_IMM_OWNERSHIP_STATE_OPEN;
	}


	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_OPEN) {
		// register the object into the main reactor to receive IMM event notifications
		if(_server_working_set->main_reactor->register_handler(acs_apgcc_objectimplementerinterface_V3::getSelObj(), this, ACE_Event_Handler::READ_MASK) < 0) {
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Taking ownership: Call 'register_handler' failed: cannot register the IMM repository handler into the main reactor");
			return acs_apbm::ERR_REACTOR_HANDLER_REGISTER_FAILURE;
		} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IMM repository handler successfully added to the main reactor");

		_imm_ownership_state = ACS_APBM_IMM_OWNERSHIP_STATE_EVENT_HANDLED;
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::release_ownership () {
	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS;
//TODO - TO REMOVE
//        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Removing existing ap board info from IMM\n");
//        remove_all_ap_board_info(); // return 0 forever

	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_EVENT_HANDLED) {
		// remove the object from the main reactor
		if(_server_working_set->main_reactor->remove_handler(acs_apgcc_objectimplementerinterface_V3::getSelObj(), ACE_Event_Handler::READ_MASK) < 0) {
			// ERROR: Removing the event handler from the reactor
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Releasing ownership: Call 'remove_handler' failed: trying to remove the IMM repository handler from the main reactor");
			return acs_apbm::ERR_REACTOR_HANDLER_REMOVE_FAILURE;
		} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IMM repository handler successfully removed from the main reactor");

		_imm_ownership_state = ACS_APBM_IMM_OWNERSHIP_STATE_OPEN;
	}

	if (_imm_ownership_state == ACS_APBM_IMM_OWNERSHIP_STATE_OPEN) {
		ACS_CC_ReturnType cc_call_result = ACS_CC_SUCCESS;











		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO);

		if (cc_call_result != ACS_CC_SUCCESS) {
			// ERROR finalizing imm internal runtime owner implementer handler
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Releasing ownership: Call 'removeClassImpl()' failed: "
					"imm error text == '%s', cc_call_result == %d", _oi_handler.getErrorText(cc_call_result).c_str(),
					cc_call_result);

			//return acs_apbm::ERR_OI_HANDLER_FINALIZE_FAILURE;
		}


		cc_call_result = finalize();

		if (cc_call_result != ACS_CC_SUCCESS) {
			// ERROR finalizing imm internal runtime owner implementer handler
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Releasing ownership: Call 'finalize()' failed: "
					"imm last error == %d, imm last error text == '%s', cc_call_result == %d",
					ACS_APGCC_RuntimeOwner_V2::getInternalLastError(), ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText(),
					cc_call_result);

			//return acs_apbm::ERR_OI_HANDLER_FINALIZE_FAILURE;
		}



/*
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
		std::vector<std::string> acs_apbm_imm_class_names_vector;

		for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(ACS_APBM_IMM_CLASS_NAMES)); i++)
			acs_apbm_imm_class_names_vector.push_back(ACS_APBM_IMM_CLASS_NAMES[i]);

		cc_call_result = _oi_handler.removeMultipleClassImpl(this, acs_apbm_imm_class_names_vector);

//		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO);
//		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO);
//		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_NIC_INFO);
//		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_OPERATIONAL_LED);
//		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_MAINTENANCE_LED);
//		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_FAULT_LED);
//		cc_call_result = _oi_handler.removeClassImpl(this, ACS_APBM_CONFIG_IMM_CLASS_NAME_STATUS_LED);
#else
		cc_call_result = finalize();
#endif

		if (cc_call_result != ACS_CC_SUCCESS) {
			// ERROR finalizing imm internal runtime owner implementer handler
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Releasing ownership: Call '"
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
					"removeClassImpl()"
#else
					"finalize()"
#endif
					"' failed: "
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
					"imm error text == '%s'"
#else
					"imm last error == %d, imm last error text == '%s'"
#endif
					", cc_call_result == %d",
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
					_oi_handler.getErrorText(cc_call_result).c_str()
#else
					getInternalLastError(), getInternalLastErrorText()
#endif
					, cc_call_result);

			return acs_apbm::ERR_OI_HANDLER_FINALIZE_FAILURE;
		}
*/


		_imm_ownership_state = ACS_APBM_IMM_OWNERSHIP_STATE_CLOSED;
	}

	return acs_apbm::ERR_NO_ERRORS;
}

const char * __CLASS_NAME__::blade_configuration_dn () const {
		return ACS_APBM_CONFIG_IMM_ROOT_DN;
}


int __CLASS_NAME__::handle_input (ACE_HANDLE fd)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "IMM event notified ! Start dispatching ... ");

	errno = 0;

	/* start of TR HV23320
	if (reactor()->resume_handler(fd)) {
		int errno_save = errno;
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'reactor()->resume_handler' failed");
	}
	   end of TR HV23320 */

	if (acs_apgcc_objectimplementerinterface_V3::dispatch(ACS_APGCC_DISPATCH_ALL) == ACS_CC_SUCCESS)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "IMM event successfully dispatched ! ");
	else
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "IMM event NOT dispatched ! IMM error_code == %d ",
				acs_apgcc_objectimplementerinterface_V3::getInternalLastError());

	return 0;
}


#ifdef ATTRIBUTE_HANDLER

int __CLASS_NAME__::add_ap_board_info(acs_apbm_apboardinfo & board){
	_attr_list.clear();

	int call_result = -1;
	if ((call_result = _check_parameter(board.slot_position, board.magazine)) < 0)
		return call_result;

	char magazine_str[16] = {0};
	acs_apbm_csreader::uint32_to_ip_format(magazine_str, board.magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"magazine string %s",magazine_str);

	void * attr_values [16] = {0};
	char rdn_value[ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX]={0};
	snprintf (rdn_value, ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX,"%s=%d", ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID, board.slot_position);

	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	const char * root_dn = blade_configuration_dn();
	snprintf (imm_full_path, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%d,%s=%s,%s", ACS_APBM_CONFIG_AP_BLADE_CLASS_ID, board.slot_position, ACS_APBM_CONFIG_MAGAZINE_CLASS_ID, magazine_str, root_dn);

	attr_values [0] = reinterpret_cast<void *>(const_cast<char*>(rdn_value));
	//attr_values [0] = reinterpret_cast<void *>(rdn_value);
	ACS_CC_ValuesDefinitionType attribute0;
	attribute0.attrName = const_cast<char*>(ap_blade_hardware_info_id_attr_name);
	attribute0.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute0.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute0.attrValues = attr_values;
	_attr_list.push_back(attribute0);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute0.attrName, reinterpret_cast<char *>(*attribute0.attrValues));

	ACS_CC_ValuesDefinitionType attribute1;
	attribute1.attrName = const_cast<char*>(fatal_event_log_info_name);
	attribute1.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute1.attrValuesNum = EMPTY_ATTRIBUTE;//SINGLE_ATTRIBUTE;
	attribute1.attrValues = EMPTY_ATTRIBUTE;//attr_values + 1;
	_attr_list.push_back(attribute1);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute1.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute1.attrValues)*/);

	//runtime attribute CACHED
	int tmp = 0;
	attr_values [2] = reinterpret_cast<void *>( & tmp);
	ACS_CC_ValuesDefinitionType attribute2;
	attribute2.attrName = const_cast<char*>(temperature_status_attr_name);
	attribute2.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute2.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute2.attrValues = EMPTY_ATTRIBUTE; //attr_values + 2;
	_attr_list.push_back(attribute2);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute2.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute2.attrValues)*/);

	//runtime attribute NOT_CACHED
	//attr_values [3] = reinterpret_cast<void *>(& board.bios_running_mode);
	ACS_CC_ValuesDefinitionType attribute3;
	attribute3.attrName = const_cast<char*>(bios_running_mode_attr_name);
	attribute3.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute3.attrValuesNum = EMPTY_ATTRIBUTE;//SINGLE_ATTRIBUTE;
	attribute3.attrValues = EMPTY_ATTRIBUTE;// attr_values + 3;
	_attr_list.push_back(attribute3);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute3.attrName, EMPTY_ATTRIBUTE /* *reinterpret_cast<int32_t *>(*attribute3.attrValues)*/);

	//runtime attribute CACHED
	attr_values [4] = reinterpret_cast<void *>(& board.raid_disk_status);
	ACS_CC_ValuesDefinitionType attribute4;
	attribute4.attrName = const_cast<char*>(raid_disk_status_attr_name);
	attribute4.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute4.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute4.attrValues = EMPTY_ATTRIBUTE; //attr_values + 4;
	_attr_list.push_back(attribute4);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute4.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute4.attrValues)*/);

	//runtime attribute CACHED
        attr_values [13] = reinterpret_cast<void *>(& board.drbd_disk_status);
        ACS_CC_ValuesDefinitionType attribute15;
        attribute15.attrName = const_cast<char*>(drbd_disk_status_attr_name);
        attribute15.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
        attribute15.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
        attribute15.attrValues = EMPTY_ATTRIBUTE; //attr_values + 4;
        _attr_list.push_back(attribute15);
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute15.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute4.attrValues)*/);

	//runtime attribute CACHED
	attr_values [5] =  reinterpret_cast<void *>(& board.board_status);
	ACS_CC_ValuesDefinitionType attribute5;
	attribute5.attrName = const_cast<char*>(board_status_attr_name);
	attribute5.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute5.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute5.attrValues = EMPTY_ATTRIBUTE; //attr_values + 5;
	_attr_list.push_back(attribute5);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute5.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute5.attrValues)*/);

	//runtime attribute CACHED
	attr_values [6] = reinterpret_cast<void *>(& board.alarm_interface_status);
	ACS_CC_ValuesDefinitionType attribute6;
	attribute6.attrName = const_cast<char*>(alarm_interface_status_attr_name);
	attribute6.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute6.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute6.attrValues = EMPTY_ATTRIBUTE; //attr_values + 6;
	_attr_list.push_back(attribute6);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute6.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute6.attrValues)*/);

	//runtime attribute NOT_CACHED
	//attr_values [7] = reinterpret_cast<void *>(const_cast<char*>(board.bios_version));
	ACS_CC_ValuesDefinitionType attribute7;
	attribute7.attrName = const_cast<char*>(bios_version_attr_name);
	attribute7.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute7.attrValuesNum = EMPTY_ATTRIBUTE;//SINGLE_ATTRIBUTE;
	attribute7.attrValues = EMPTY_ATTRIBUTE;//attr_values + 7;
	_attr_list.push_back(attribute7);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute7.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute7.attrValues)*/);

	//STRUCT PRODUCT INFO DN *** id=ProductInfo,axeApBladeInfo='slot', ..., ... ***
	char product_info_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	build_DN_from_magazine_slot_and_product_info(product_info_path, board.slot_position,board.magazine, ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO);
	attr_values [8] = reinterpret_cast<void *>(const_cast<char*>(product_info_path));
	ACS_CC_ValuesDefinitionType attribute8;
	attribute8.attrName = const_cast<char*>(product_info_attr_name);
	attribute8.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute8.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute8.attrValues = EMPTY_ATTRIBUTE; //attr_values + 8;
	_attr_list.push_back(attribute8);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute8.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute8.attrValues)*/);

	//OPERATIONAL LED DN
	char operational_led_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	build_DN_from_magazine_slot_and_led_type(operational_led_path, board.slot_position, board.magazine, axe_eth_shelf::LED_TYPE_OPERATIONAL);
	//ACS_APBM_LOG(LOG_LEVEL_INFO,"OPERATIONAL LED path :  %s", operational_led_path);
	attr_values [9] = reinterpret_cast<void *>(const_cast<char*>(operational_led_path));
	ACS_CC_ValuesDefinitionType attribute9;
	attribute9.attrName = const_cast<char*>(operational_led_attr_name);
	attribute9.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute9.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute9.attrValues = EMPTY_ATTRIBUTE; //attr_values + 9;
	_attr_list.push_back(attribute9);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute9.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute9.attrValues)*/);

	//FAULT LED DN
	char fault_led_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	build_DN_from_magazine_slot_and_led_type(fault_led_path, board.slot_position, board.magazine, axe_eth_shelf::LED_TYPE_FAULT);
	attr_values [10] = reinterpret_cast<void *>(const_cast<char*>(fault_led_path));
	ACS_CC_ValuesDefinitionType attribute10;
	attribute10.attrName = const_cast<char*>(fault_led_attr_name);
	attribute10.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute10.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute10.attrValues = EMPTY_ATTRIBUTE; //attr_values + 10;
	_attr_list.push_back(attribute10);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute10.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute10.attrValues)*/);

	//MAINTENANCE LED
	char maintenance_led_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	build_DN_from_magazine_slot_and_led_type(maintenance_led_path, board.slot_position, board.magazine, axe_eth_shelf::LED_TYPE_MAINTENANCE);
	attr_values [11] = reinterpret_cast<void *>(const_cast<char*>(maintenance_led_path));
	ACS_CC_ValuesDefinitionType attribute11;
	attribute11.attrName = const_cast<char*>(maintenance_led_attr_name);
	attribute11.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute11.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute11.attrValues = EMPTY_ATTRIBUTE; //attr_values + 11;
	_attr_list.push_back(attribute11);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute11.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute11.attrValues)*/);

	//STAUTS LED
	char status_led_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	build_DN_from_magazine_slot_and_led_type(status_led_path, board.slot_position, board.magazine, axe_eth_shelf::LED_TYPE_STATUS);
	attr_values [12] = reinterpret_cast<void *>(const_cast<char*>(status_led_path));
	ACS_CC_ValuesDefinitionType attribute12;
	attribute12.attrName = const_cast<char*>(status_led_attr_name);
	attribute12.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute12.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute12.attrValues = EMPTY_ATTRIBUTE; //attr_values + 12;
	_attr_list.push_back(attribute12);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute12.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute12.attrValues)*/);

	//runtime attribute CACHED
	ACS_CC_ValuesDefinitionType attribute13;
	attribute13.attrName = const_cast<char*>(ipmi_upgrade_result_attr_name);
	attribute13.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute13.attrValuesNum = SINGLE_ATTRIBUTE;
	void* int32_attr[1] = {reinterpret_cast<void*>(&board.ipmi_upgrade_status)};
	attribute13.attrValues = int32_attr;
	_attr_list.push_back(attribute13);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute13.attrName, *reinterpret_cast<int32_t *>(*attribute13.attrValues));

	//runtime attribute CACHED
	ACS_CC_ValuesDefinitionType attribute14;
	attribute14.attrName = const_cast<char*>(ipmi_upgrade_time_attr_name);
	attribute14.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute14.attrValuesNum = SINGLE_ATTRIBUTE;
	void* valueString[1]={reinterpret_cast<void*>(const_cast<char *>(board.ipmi_upgrade_last_time))};
	attribute14.attrValues = valueString;
	_attr_list.push_back(attribute14);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute14.attrName, reinterpret_cast<char *>(*attribute14.attrValues));

	// NICINFO
	ACS_CC_ValuesDefinitionType nic_attribute;
	nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
	nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	nic_attribute.attrValuesNum = EMPTY_ATTRIBUTE;
	nic_attribute.attrValues = EMPTY_ATTRIBUTE;
	_attr_list.push_back(nic_attribute);
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", nic_attribute.attrName, "(null)");
	
	int imm_call_result = -1;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IMM_PARENT_PATH '%s' - IMM_RDN_PATH == '%s'", imm_full_path, rdn_value);

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	++_imm_operation_ongoing_counter;
	imm_call_result = _om_handler.createObject(ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO, imm_full_path, _attr_list);
	--_imm_operation_ongoing_counter;
#else
	imm_call_result = createRuntimeObj(ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO, imm_full_path, _attr_list);
#endif
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "run time object created");
	int imm_internal_last_error = 0;
	const char * imm_internal_last_error_text = 0;

	if(imm_call_result != ACS_CC_SUCCESS){
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = getInternalLastError();
		imm_internal_last_error_text = getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_info failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);

		return acs_apbm::ERR_IMM_CREATE_OBJ;
	} else
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Object correctly created!");

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "**************** CREATION PRODUCT **************** \n\n");
	imm_call_result = add_ap_board_product_info(board);
	if(imm_call_result != ACS_CC_SUCCESS) {

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_PRODUCT_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_product_info failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "**************** CREATION LED ****************\n\n");
	imm_call_result = add_ap_board_led_info(board);
	if(imm_call_result != ACS_CC_SUCCESS) {

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_LED_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_product_info failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "**************** CREATION NIC\n\n ****************");
	imm_call_result = add_ap_board_nic_info(board);
	if(imm_call_result != ACS_CC_SUCCESS) {

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_NIC_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_product_info failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::add_ap_board_info_for_scb(acs_apbm_apboardinfo & board){
	_attr_list.clear();

	char mag[16] = {0};
	acs_apbm_csreader::uint32_to_ip_format(mag,board.magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"magazine string %s",mag);

	void * attr_values [16] = {0};
	char rdn_value[ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX]={0};
	snprintf (rdn_value, ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX,"%s=%d", ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID, board.slot_position);

	attr_values [0] = reinterpret_cast<void *>(const_cast<char*>(rdn_value));
	//attr_values [0] = reinterpret_cast<void *>(rdn_value);
	ACS_CC_ValuesDefinitionType attribute0;
	attribute0.attrName = const_cast<char*>(ap_blade_hardware_info_id_attr_name);
	attribute0.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute0.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute0.attrValues = attr_values;
	_attr_list.push_back(attribute0);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute0.attrName, reinterpret_cast<char *>(*attribute0.attrValues));

	ACS_CC_ValuesDefinitionType attribute1;
	attribute1.attrName = const_cast<char*>(fatal_event_log_info_name);
	attribute1.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute1.attrValuesNum = EMPTY_ATTRIBUTE;//SINGLE_ATTRIBUTE;
	attribute1.attrValues = EMPTY_ATTRIBUTE;//attr_values + 1;
	_attr_list.push_back(attribute1);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute1.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute1.attrValues)*/);

	//runtime attribute CACHED
	int pppp = 0;
	attr_values [2] = reinterpret_cast<void *>( & pppp);
	ACS_CC_ValuesDefinitionType attribute2;
	attribute2.attrName = const_cast<char*>(temperature_status_attr_name);
	attribute2.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute2.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute2.attrValues = EMPTY_ATTRIBUTE; //attr_values + 2;
	_attr_list.push_back(attribute2);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute2.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute2.attrValues)*/);

	//runtime attribute NOT_CACHED
	//attr_values [3] = reinterpret_cast<void *>(& board.bios_running_mode);
	ACS_CC_ValuesDefinitionType attribute3;
	attribute3.attrName = const_cast<char*>(bios_running_mode_attr_name);
	attribute3.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute3.attrValuesNum = EMPTY_ATTRIBUTE;//SINGLE_ATTRIBUTE;
	attribute3.attrValues = EMPTY_ATTRIBUTE;// attr_values + 3;
	_attr_list.push_back(attribute3);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute3.attrName, EMPTY_ATTRIBUTE /* *reinterpret_cast<int32_t *>(*attribute3.attrValues)*/);

	//runtime attribute CACHED
	attr_values [4] = reinterpret_cast<void *>(& board.raid_disk_status);
	ACS_CC_ValuesDefinitionType attribute4;
	attribute4.attrName = const_cast<char*>(raid_disk_status_attr_name);
	attribute4.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute4.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute4.attrValues = EMPTY_ATTRIBUTE; //attr_values + 4;
	_attr_list.push_back(attribute4);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute4.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute4.attrValues)*/);
	
	//runtime attribute CACHED
	attr_values [13] = reinterpret_cast<void *>(& board.drbd_disk_status);
	ACS_CC_ValuesDefinitionType attribute15;
	attribute15.attrName = const_cast<char*>(drbd_disk_status_attr_name);
	attribute15.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute15.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute15.attrValues = EMPTY_ATTRIBUTE; //attr_values + 4;
	_attr_list.push_back(attribute15);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute15.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute4.attrValues)*/);

	//runtime attribute CACHED
	attr_values [5] =  reinterpret_cast<void *>(& board.board_status);
	ACS_CC_ValuesDefinitionType attribute5;
	attribute5.attrName = const_cast<char*>(board_status_attr_name);
	attribute5.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute5.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute5.attrValues = EMPTY_ATTRIBUTE; //attr_values + 5;
	_attr_list.push_back(attribute5);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute5.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute5.attrValues)*/);

	//runtime attribute CACHED
	attr_values [6] = reinterpret_cast<void *>(& board.alarm_interface_status);
	ACS_CC_ValuesDefinitionType attribute6;
	attribute6.attrName = const_cast<char*>(alarm_interface_status_attr_name);
	attribute6.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute6.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute6.attrValues = EMPTY_ATTRIBUTE; //attr_values + 6;
	_attr_list.push_back(attribute6);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute6.attrName, EMPTY_ATTRIBUTE /**reinterpret_cast<int32_t *>(*attribute6.attrValues)*/);

	//runtime attribute NOT_CACHED
	//attr_values [7] = reinterpret_cast<void *>(const_cast<char*>(board.bios_version));
	ACS_CC_ValuesDefinitionType attribute7;
	attribute7.attrName = const_cast<char*>(bios_version_attr_name);
	attribute7.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute7.attrValuesNum = EMPTY_ATTRIBUTE;//SINGLE_ATTRIBUTE;
	attribute7.attrValues = EMPTY_ATTRIBUTE;//attr_values + 7;
	_attr_list.push_back(attribute7);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute7.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute7.attrValues)*/);

	attr_values [8] = reinterpret_cast<void *>(const_cast<char*>(""));
	ACS_CC_ValuesDefinitionType attribute8;
	attribute8.attrName = const_cast<char*>(product_info_attr_name);
	attribute8.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute8.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute8.attrValues = EMPTY_ATTRIBUTE; //attr_values + 8;
	_attr_list.push_back(attribute8);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute8.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute8.attrValues)*/);

	attr_values [9] = reinterpret_cast<void *>(const_cast<char*>(""));
	ACS_CC_ValuesDefinitionType attribute9;
	attribute9.attrName = const_cast<char*>(operational_led_attr_name);
	attribute9.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute9.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute9.attrValues = EMPTY_ATTRIBUTE; //attr_values + 9;
	_attr_list.push_back(attribute9);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute9.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute9.attrValues)*/);

	attr_values [10] = reinterpret_cast<void *>(const_cast<char*>(""));
	ACS_CC_ValuesDefinitionType attribute10;
	attribute10.attrName = const_cast<char*>(fault_led_attr_name);
	attribute10.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute10.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute10.attrValues = EMPTY_ATTRIBUTE; //attr_values + 10;
	_attr_list.push_back(attribute10);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute10.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute10.attrValues)*/);

	attr_values [11] = reinterpret_cast<void *>(const_cast<char*>(""));
	ACS_CC_ValuesDefinitionType attribute11;
	attribute11.attrName = const_cast<char*>(maintenance_led_attr_name);
	attribute11.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute11.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute11.attrValues = EMPTY_ATTRIBUTE; //attr_values + 11;
	_attr_list.push_back(attribute11);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute11.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute11.attrValues)*/);

	attr_values [12] = reinterpret_cast<void *>(const_cast<char*>(""));
	ACS_CC_ValuesDefinitionType attribute12;
	attribute12.attrName = const_cast<char*>(status_led_attr_name);
	attribute12.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	attribute12.attrValuesNum = EMPTY_ATTRIBUTE; //SINGLE_ATTRIBUTE;
	attribute12.attrValues = EMPTY_ATTRIBUTE; //attr_values + 12;
	_attr_list.push_back(attribute12);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%s' ", attribute12.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute12.attrValues)*/);

	//runtime attribute CACHED
	ACS_CC_ValuesDefinitionType attribute13;
	attribute13.attrName = const_cast<char*>(ipmi_upgrade_result_attr_name);
	attribute13.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute13.attrValuesNum = SINGLE_ATTRIBUTE;
	void* int32_attr[1] = {reinterpret_cast<void*>(&board.ipmi_upgrade_status)};
	attribute13.attrValues = int32_attr;
	_attr_list.push_back(attribute13);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute13.attrName, *reinterpret_cast<int32_t *>(*attribute13.attrValues));

	//runtime attribute CACHED
	ACS_CC_ValuesDefinitionType attribute14;
	attribute14.attrName = const_cast<char*>(ipmi_upgrade_time_attr_name);
	attribute14.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute14.attrValuesNum = SINGLE_ATTRIBUTE;
	void* valueString[1]={reinterpret_cast<void*>(const_cast<char *>(board.ipmi_upgrade_last_time))};
	attribute14.attrValues = valueString;
	_attr_list.push_back(attribute14);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s->%d' ", attribute14.attrName, reinterpret_cast<char *>(*attribute14.attrValues));
	
	/*
	if(_server_working_set->shelves_data_manager->is_10G())
	{
		void * nic_attr_values [SEVEN_ATTRIBUTE] = {0};
        	// *******-STRUCT NIC INFO DN - VERIFY 7 NIC*************
        	for (int i=0; i < SEVEN_ATTRIBUTE; ++i){
			nic_attr_values [i] = reinterpret_cast<void *>(const_cast<char*>(""));
		}
	}
	else
	{
		void * nic_attr_values [FIVE_ATTRIBUTE] = {0};
        	// *******-STRUCT NIC INFO DN - VERIFY 5 NIC*************
        	for (int i=0; i < FIVE_ATTRIBUTE; ++i){
			nic_attr_values [i] = reinterpret_cast<void *>(const_cast<char*>(""));
		}
	}
	*/
	ACS_CC_ValuesDefinitionType nic_attribute;
	nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
	nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
	nic_attribute.attrValuesNum = EMPTY_ATTRIBUTE;
	nic_attribute.attrValues = EMPTY_ATTRIBUTE; //nic_attr_values;
	_attr_list.push_back(nic_attribute);

	int imm_call_result = -1;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	++_imm_operation_ongoing_counter;
	imm_call_result = _om_handler.createObject(ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO, 0, _attr_list);
	--_imm_operation_ongoing_counter;
#else
	imm_call_result = createRuntimeObj(ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO, "", _attr_list);
#endif
	if(imm_call_result != ACS_CC_SUCCESS) {
		int imm_internal_last_error = 0;
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_info failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		//return acs_apbm::ERR_IMM_CREATE_OBJ;
	} else ACS_APBM_LOG(LOG_LEVEL_INFO,"Object for SCB correctly created!");

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_last_time(char (&ipmifw_lastupgtime)[30],int32_t slot,uint32_t magazine){
	int op_res = 0;
	int cba = 0;
	if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT)
		op_res = get_board_ipmi_upgrade_last_time_on_ap(ipmifw_lastupgtime, slot, magazine);
	else if (!(cba = _server_working_set->cs_reader->is_cba_system()))
		op_res = get_board_ipmi_upgrade_last_time_on_scb(ipmifw_lastupgtime, slot, magazine);
	else {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid Request, check slot position %d or architecture %d",slot, cba);
	}
	if (op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot get board ipmi upgrade last time ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_last_time(..)' failed. op_res == %d !", magazine, slot, op_res);
		return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_status(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, int32_t slot, uint32_t magazine){
	int op_res = 0;
	int cba = 0;
	if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT)
		op_res = get_board_ipmi_upgrade_status_on_ap(ipmi_upg_status, slot, magazine);
	else if (!(cba = _server_working_set->cs_reader->is_cba_system()))
		op_res = get_board_ipmi_upgrade_status_on_scb(ipmi_upg_status, slot, magazine);
	else {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid Request, check slot position %d or architecture %d",slot, cba);
	}
	if (op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot get board ipmi upgrade last time ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_last_time(..)' failed. op_res == %d !", magazine, slot, op_res);
		return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_last_time_on_ap(char (&ipmifw_lastupgtime)[30],int32_t slot,uint32_t magazine)
{
		acs_apgcc_paramhandling pha;

		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		build_DN_from_magazine_and_slot(imm_full_path, slot, magazine);
		char tmp_upg_time[256] = {0};
		ACS_CC_ReturnType imm_result = pha.getParameter<256>(imm_full_path, ipmi_upgrade_time_attr_name, tmp_upg_time);
		if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting node architecture from ApzFunction IMM object
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'getParameter' failed: cannot get the attribute '%s' from IMM: internal last error == %d, internal last error text  == %s",
					ipmi_upgrade_time_attr_name, pha.getInternalLastError(), pha.getInternalLastErrorText());
			return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
		}
		else{
		strncpy(ipmifw_lastupgtime, tmp_upg_time,30);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Retrieved time: '%s' from object with dn: %s",ipmifw_lastupgtime, imm_full_path);
		}
			return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_status_on_ap(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, int32_t slot, uint32_t magazine)
{
		acs_apgcc_paramhandling pha;

		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		build_DN_from_magazine_and_slot(imm_full_path, slot, magazine);
		int tmp_status;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "PATH RETRIEVED : %s",imm_full_path);
		ACS_CC_ReturnType imm_result = pha.getParameter(imm_full_path, ipmi_upgrade_result_attr_name, &tmp_status);
		if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting node architecture from ApzFunction IMM object
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'getParameter' failed: cannot get the attribute '%s' from IMM: internal last error == %d, internal last error text  == %s",
					ipmi_upgrade_result_attr_name, pha.getInternalLastError(), pha.getInternalLastErrorText());
			ipmi_upg_status = axe_eth_shelf::IPMI_UPGRADE_STATUS_FAIL;
			return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
		}
		ipmi_upg_status = (axe_eth_shelf::ipmi_upgrade_status_t)tmp_status;
			return acs_apbm::ERR_NO_ERRORS;
}
//TO HANDLE SCB-RP4 UPGRADE
int __CLASS_NAME__::get_board_ipmi_upgrade_last_time_on_scb(char (&ipmifw_lastupgtime)[30],int32_t slot,uint32_t /*magazine*/)
{
		acs_apgcc_paramhandling pha;

		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		::snprintf(imm_full_path, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d", slot);

		char tmp_upg_time[256] = {0};
		ACS_CC_ReturnType imm_result = pha.getParameter<256>(imm_full_path, ipmi_upgrade_time_attr_name, tmp_upg_time);
		if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting node architecture from ApzFunction IMM object
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'getParameter' failed: cannot get the attribute '%s' from IMM: internal last error == %d, internal last error text  == %s",
					ipmi_upgrade_time_attr_name, pha.getInternalLastError(), pha.getInternalLastErrorText());
			return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
		}
		else{
		strncpy(ipmifw_lastupgtime, tmp_upg_time,30);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Retrieved time: '%s' from object with dn: %s",ipmifw_lastupgtime, imm_full_path);
		}
			return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_status_on_scb(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, int32_t slot, uint32_t /*magazine*/)
{
		acs_apgcc_paramhandling pha;

		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		::snprintf(imm_full_path, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d", slot);

		int tmp_status;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "PATH RETRIEVED : %s",imm_full_path);
		ACS_CC_ReturnType imm_result = pha.getParameter(imm_full_path, ipmi_upgrade_result_attr_name, &tmp_status);
		if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting node architecture from ApzFunction IMM object
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'getParameter' failed: cannot get the attribute '%s' from IMM: internal last error == %d, internal last error text  == %s",
					ipmi_upgrade_result_attr_name, pha.getInternalLastError(), pha.getInternalLastErrorText());
			ipmi_upg_status = axe_eth_shelf::IPMI_UPGRADE_STATUS_FAIL;
			return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
		}
		ipmi_upg_status = (axe_eth_shelf::ipmi_upgrade_status_t)tmp_status;
			return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::add_ap_board_product_info(acs_apbm_apboardinfo & board){
	int imm_internal_last_error = 0;

	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

	if (const int call_result = build_DN_from_magazine_and_slot(imm_full_path, board.slot_position, board.magazine) < 0)
		return call_result;

	char dn [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX];
	build_DN_from_magazine_slot_and_product_info(dn, board.slot_position, board.magazine,
			ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO);

	if (modify_ap_board_attribute(imm_internal_last_error, imm_full_path, IMM_ATTRIBUTE_BOARD_PRODUCT_INFO, dn)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_product_info failure: IMM error: %d", imm_internal_last_error);

		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	// PRODUCT INFO
	_attr_list.clear();

	int call_result = -1;
	if ((call_result = _check_parameter(board.slot_position, board.magazine)) < 0)
		return call_result;

	char mag[16] = {0};
	acs_apbm_csreader::uint32_to_ip_format(mag,board.magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"magazine string %s",mag);

	void * attr_values [8] = {0};

	char rdn_value[ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX]={0};
	snprintf (rdn_value, ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX,"%s=%s", product_info_id_attr_name, ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO );

	attr_values [0] = reinterpret_cast<void *>(const_cast<char*>(rdn_value));
	//attr_values [0] = reinterpret_cast<void *>(rdn_value);
	ACS_CC_ValuesDefinitionType attribute0;
	attribute0.attrName = const_cast<char*>(product_info_id_attr_name);
	attribute0.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute0.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute0.attrValues = attr_values;
	_attr_list.push_back(attribute0);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute0.attrName, reinterpret_cast<char *>(*attribute0.attrValues));

	attr_values [1] = reinterpret_cast<void *>(const_cast<char*>(board.product_revision_state));
	ACS_CC_ValuesDefinitionType attribute1;
	attribute1.attrName = const_cast<char*>(product_revision_attr_name);
	attribute1.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute1.attrValuesNum =  SINGLE_ATTRIBUTE; // EMPTY_ATTRIBUTE; // for configuration object use EMPTY_ATTRIBUTE
	attribute1.attrValues = attr_values + 1; // EMPTY_ATTRIBUTE; // for configuration object use EMPTY_ATTRIBUTE
	_attr_list.push_back(attribute1);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute1.attrName, reinterpret_cast<char *>(*attribute1.attrValues));

	attr_values [2] = reinterpret_cast<void *>(const_cast<char*>(board.serial_number));
	ACS_CC_ValuesDefinitionType attribute2;
	attribute2.attrName = const_cast<char*>(serial_number_attr_name);
	attribute2.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute2.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute2.attrValues = attr_values + 2;
	_attr_list.push_back(attribute2);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute2.attrName, reinterpret_cast<char *>(*attribute2.attrValues));

	attr_values [3] = reinterpret_cast<void *>(const_cast<char*>(board.product_name));
	ACS_CC_ValuesDefinitionType attribute3;
	attribute3.attrName = const_cast<char*>(product_name_att_name);
	attribute3.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute3.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute3.attrValues = attr_values + 3;
	_attr_list.push_back(attribute3);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute3.attrName, reinterpret_cast<char *>(*attribute3.attrValues));

	attr_values [4] = reinterpret_cast<void *>(const_cast<char*>(board.manufacture_date));
	ACS_CC_ValuesDefinitionType attribute4;
	attribute4.attrName = const_cast<char*>(manufacturing_date_attr_name);
	attribute4.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute4.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute4.attrValues = attr_values + 4;
	_attr_list.push_back(attribute4);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute4.attrName, reinterpret_cast<char *>(*attribute4.attrValues));

	attr_values [5] = reinterpret_cast<void *>(const_cast<char*>(board.vendor));
	ACS_CC_ValuesDefinitionType attribute5;
	attribute5.attrName = const_cast<char*>(vendor_attr_name);
	attribute5.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute5.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute5.attrValues = attr_values + 5;
	_attr_list.push_back(attribute5);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute5.attrName, reinterpret_cast<char *>(*attribute5.attrValues));

	attr_values [6] = reinterpret_cast<void *>(const_cast<char*>(board.product_number));
	ACS_CC_ValuesDefinitionType attribute6;
	attribute6.attrName = const_cast<char*>(product_number_attr_name);
	attribute6.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute6.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute6.attrValues = attr_values + 6;
	_attr_list.push_back(attribute6);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute6.attrName, reinterpret_cast<char *>(*attribute6.attrValues));

	int imm_call_result = -1;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IMM_PARENT_PATH '%s' - IMM_RDN_PATH == '%s'", imm_full_path, rdn_value);


#if defined (ACS_APBM_USE_IMM_CONFIGURATION_PRODUCT_OBJECTS)
	++_imm_operation_ongoing_counter;
	imm_call_result = _om_handler.createObject(ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO,imm_full_path, _attr_list); //CONFIGURATION
	--_imm_operation_ongoing_counter;
#else
	imm_call_result = createRuntimeObj(ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO, imm_full_path, _attr_list); //RUNTIME
#endif

	if (imm_call_result != ACS_CC_SUCCESS) {
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_PRODUCT_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_product_info failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	return 0;
}

int __CLASS_NAME__::add_ap_board_nic_info(acs_apbm_apboardinfo & board){
	int call_result = 0;

	char magazine_str[16] = {0};
	acs_apbm_csreader::uint32_to_ip_format(magazine_str, board.magazine);

	const char * root_dn = blade_configuration_dn();



	// NIC info to be stored in IMM depend from APG shelf architecture
	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return acs_apbm::ERR_UNKNOWN_ARCHITECTURE;
	}

	ACS_CC_ImmParameter nic_attribute;
	// Allocate memory for max size of possible NICs (currently NINE).
	char nic_path[NINE_ATTRIBUTE][ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {{0}};
	void* nic_attr_values[NINE_ATTRIBUTE] = {0};

	if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		for (int i=0; i < NINE_ATTRIBUTE; i++){
			snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s",
							nic_info_id_attr_name,
							ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i, //NIC0 - NIC1 -NIC2 - NIC3 - NIC4 - NIC5 - NIC6 - NIC7 - NIC8
							ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
							magazine_str,
							root_dn);
			nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC_PATH[%d] value is : %s \n ", i, nic_path[i]);
		}
		nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
		nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
		nic_attribute.attrValuesNum = NINE_ATTRIBUTE;
		nic_attribute.attrValues = nic_attr_values;
	}
	else if(_server_working_set->shelves_data_manager->is_hardwaretype_gep7())
	{ 
		for (int i=0; i < FIVE_ATTRIBUTE; i++){
			snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s",
							nic_info_id_attr_name,
							ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i+2, //NIC2 - NIC3 - NIC4 - NIC5 - NIC6
							ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
							magazine_str,
							root_dn);
			nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC_PATH[%d] value is : %s \n ", i, nic_path[i]);
                }
		nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
		nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
		nic_attribute.attrValuesNum = FIVE_ATTRIBUTE;
		nic_attribute.attrValues = nic_attr_values;
	}
	else if(_server_working_set->shelves_data_manager->is_10G())
	{
		for (int i=0; i < SEVEN_ATTRIBUTE; i++){
			snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s",
							nic_info_id_attr_name,
							ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i, //NIC0 - NIC1 -NIC2 - NIC3 - NIC4 - NIC5 - NIC6
							ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
							magazine_str,
							root_dn);
			nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC_PATH[%d] value is : %s \n ", i, nic_path[i]);
		}
		nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
		nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
		nic_attribute.attrValuesNum = SEVEN_ATTRIBUTE;
		nic_attribute.attrValues = nic_attr_values;
	}
	else
	{
		for (int i=0; i < FIVE_ATTRIBUTE; i++){
			snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s",
							nic_info_id_attr_name,
							ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i, //NIC0 - NIC1 -NIC2 - NIC3 - NIC4
							ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
							board.slot_position,
							ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
							magazine_str,
							root_dn);
			nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC_PATH[%d] value is : %s \n ", i, nic_path[i]);
		}
		nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
		nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
		nic_attribute.attrValuesNum = FIVE_ATTRIBUTE;
		nic_attribute.attrValues = nic_attr_values;
	}

	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	build_DN_from_magazine_and_slot(imm_full_path, board.slot_position, board.magazine);

	/*
	 * To refresh any link in the APBM blade root object that was empty for some strange reasons
	 */
	ACS_CC_ReturnType imm_call_result = acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(imm_full_path, &nic_attribute);
	if (imm_call_result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_nic_info failure: IMM error = %d, IMM error text == '%s'",
				acs_apgcc_objectimplementerinterface_V3::getInternalLastError(),
				acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());

		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Adding NICs to ap board ");
	bool isgep7hw = _server_working_set->shelves_data_manager->is_hardwaretype_gep7();
	int error = 0;
	if(!isgep7hw)
	{
		//CREATE NIC0
		if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_0)) < 0)
			error++;

		//CREATE NIC1
		if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_1)) < 0)
			error++;
	}
	//CREATE NIC2
	if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_2)) < 0)
		error++;

	//CREATE NIC3
	if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_3)) < 0)
		error++;

	//CREATE NIC4
	if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_4)) < 0)
		error++;
	
	//ETH5 and ETH6 SUPPORT	
	if((env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) || _server_working_set->shelves_data_manager->is_10G())
	{
		//CREATE NIC5
		if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_5)) < 0) {
				error++;
		}
		//CREATE NIC6
		if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_6)) < 0) {
				error++;
		}
	}

	if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		//CREATE NIC7
		if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_7)) < 0) {
				error++;
		}
		//CREATE NIC8
		if((call_result = add_ap_board_nic_info_by_name(board, axe_eth_shelf::IMM_NIC_8)) < 0) {
				error++;
		}
	}

	if(!error)
		return 0;
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error creating nic info, %d structure failed", error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}
	
}

int __CLASS_NAME__::add_ap_board_nic_info_by_name(acs_apbm_apboardinfo & board, axe_eth_shelf::nic_name_t nic_name){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "In add_ap_board_nic_info_by_name for adding NICS");
	_attr_list.clear();
	int call_result = -1;
	if ((call_result = _check_parameter(board.slot_position, board.magazine)) < 0)
		return call_result;

	char mag[16] = {0};
	acs_apbm_csreader::uint32_to_ip_format(mag,board.magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"magazine string %s",mag);

	void * attr_values [8] = {0};
	char rdn_value[ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX]={0};

	char nic_name_str[32] = {0};
	axe_eth_shelf::nic_status_t status;
	char mac_address[32] = {0};
	char ip_address[32] = {0};

	retrieve_nic_info_from_ap_board_by_nic_name(nic_name_str, 32, status, mac_address, 32, ip_address, 32, board,nic_name );

	snprintf (rdn_value, ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX,"%s=%s", nic_info_id_attr_name, nic_name_str );

	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	build_DN_from_magazine_and_slot(imm_full_path, board.slot_position, board.magazine);

	attr_values [0] = reinterpret_cast<void *>(const_cast<char*>(rdn_value));
	//attr_values [0] = reinterpret_cast<void *>(rdn_value);
	ACS_CC_ValuesDefinitionType attribute0;
	attribute0.attrName = const_cast<char*>(nic_info_id_attr_name);
	attribute0.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute0.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute0.attrValues = attr_values;
	_attr_list.push_back(attribute0);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute0.attrName, reinterpret_cast<char *>(*attribute0.attrValues));

	attr_values [1] = reinterpret_cast<void *>(const_cast<char*>(nic_name_str));
	ACS_CC_ValuesDefinitionType attribute1;
	attribute1.attrName = const_cast<char*>(nic_name_attr_name);
	attribute1.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute1.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute1.attrValues = attr_values + 1;
	_attr_list.push_back(attribute1);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute1.attrName, reinterpret_cast<char *>(*attribute1.attrValues));

	attr_values [2] = reinterpret_cast<void *>(& status);
	ACS_CC_ValuesDefinitionType attribute2;
	attribute2.attrName = const_cast<char*>(nic_status_attr_name);
	attribute2.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute2.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute2.attrValues = attr_values + 2;
	_attr_list.push_back(attribute2);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", attribute2.attrName, *reinterpret_cast<int32_t *>(*attribute2.attrValues));

	attr_values [3] = reinterpret_cast<void *>(const_cast<char*>(mac_address));
	ACS_CC_ValuesDefinitionType attribute3;
	attribute3.attrName = const_cast<char*>(nic_mac_address_attr_name);
	attribute3.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute3.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute3.attrValues = attr_values + 3;
	_attr_list.push_back(attribute3);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute3.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute3.attrValues)*/);

	attr_values [4] = reinterpret_cast<void *>(const_cast<char*>(ip_address));
	ACS_CC_ValuesDefinitionType attribute4;
	attribute4.attrName = const_cast<char*>(nic_ip_address_attr_name);
	attribute4.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute4.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute4.attrValues = attr_values + 4;
	_attr_list.push_back(attribute4);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute4.attrName, EMPTY_ATTRIBUTE /*reinterpret_cast<char *>(*attribute4.attrValues)*/);

	int imm_call_result = -1;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IMM_PARENT_PATH '%s' - IMM_RDN_PATH == '%s'", imm_full_path, rdn_value);


#if defined (ACS_APBM_USE_IMM_CONFIGURATION_NIC_OBJECTS)
	++_imm_operation_ongoing_counter;
	imm_call_result = _om_handler.createObject(ACS_APBM_CONFIG_IMM_CLASS_NAME_NIC_INFO, imm_full_path, _attr_list); //CONFIGURATION
	--_imm_operation_ongoing_counter;
#else
	imm_call_result = createRuntimeObj(ACS_APBM_CONFIG_IMM_CLASS_NAME_NIC_INFO, imm_full_path, _attr_list); //RUNTIME
#endif

	if(imm_call_result != ACS_CC_SUCCESS){
		int imm_internal_last_error = 0;
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_NIC_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_nic_info_by_name failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	return 0;
}

int __CLASS_NAME__::add_ap_board_led_info(acs_apbm_apboardinfo & board){

	int call_result = 0;
	int error = 0;

	//CREATE FAULT LED
	if((call_result = add_ap_board_led_info_by_type(board, axe_eth_shelf::LED_TYPE_FAULT)) < 0)
		error++;

	//CREATE OPERATIONAL LED
	if((call_result = add_ap_board_led_info_by_type(board, axe_eth_shelf::LED_TYPE_OPERATIONAL)) < 0)
		error++;

	//CREATE MAINTENANCE LED
	if((call_result = add_ap_board_led_info_by_type(board, axe_eth_shelf::LED_TYPE_MAINTENANCE)) < 0)
		error++;

	//CREATE STATUS LED
	if((call_result = add_ap_board_led_info_by_type(board, axe_eth_shelf::LED_TYPE_STATUS)) < 0)
		error++;

	if(!error)
		return 0;
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error creating led info, %d structure failed", error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	};
}

int __CLASS_NAME__::add_ap_board_led_info_by_type(acs_apbm_apboardinfo & board, axe_eth_shelf::led_type_t led){
	int imm_internal_last_error = 0;

	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	if (const int call_result = build_DN_from_magazine_and_slot(imm_full_path, board.slot_position, board.magazine) < 0)
		return call_result;

	char dn [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX];
	build_DN_from_magazine_slot_and_led_type(dn, board.slot_position, board.magazine, led);

	attributes_names_t led_attr_name;
	switch (led) {
	 case axe_eth_shelf::LED_TYPE_FAULT: led_attr_name = IMM_ATTRIBUTE_BOARD_FAULT_LED; break;
	 case axe_eth_shelf::LED_TYPE_MAINTENANCE: led_attr_name = IMM_ATTRIBUTE_BOARD_MAINTENANCE_LED; break;
	 case axe_eth_shelf::LED_TYPE_OPERATIONAL: led_attr_name = IMM_ATTRIBUTE_BOARD_OPERATIONAL_LED; break;
	 case axe_eth_shelf::LED_TYPE_STATUS: led_attr_name = IMM_ATTRIBUTE_BOARD_STATUS_LED; break;
	 }

	if (modify_ap_board_attribute(imm_internal_last_error, imm_full_path, led_attr_name, dn)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_led_info_by_type failure: IMM error: %d", imm_internal_last_error);

		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	_attr_list.clear();
	int call_result = -1;
	if ((call_result = _check_parameter(board.slot_position, board.magazine)) < 0)
		return call_result;

	char mag[16] = {0};
	acs_apbm_csreader::uint32_to_ip_format(mag,board.magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"magazine string %s",mag);

	char led_type_str[32] = {0};

	switch(led){
	case axe_eth_shelf::LED_TYPE_FAULT :
			::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_FAULT_LED);
			break;
	case axe_eth_shelf::LED_TYPE_MAINTENANCE :
		::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_MAINTENANCE_LED);
			break;
	case axe_eth_shelf::LED_TYPE_OPERATIONAL :
		::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_OPERATIONAL_LED);
			break;
	case axe_eth_shelf::LED_TYPE_STATUS :
		::snprintf(led_type_str, 32, ACS_APBM_CONFIG_IMM_CLASS_NAME_STATUS_LED);
			break;
	}

	axe_eth_shelf::led_color_t color;
	axe_eth_shelf::led_status_t status;
	bool supported = false;

	retrieve_led_info_from_ap_board_by_led_type(color, status, supported, board, led);

	void * attr_values [8] = {0};
	char rdn_value[ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX]={0};

	snprintf (rdn_value, ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX,"%s=%s", led_id_attr_name, led_type_str );

	attr_values [0] = reinterpret_cast<void *>(const_cast<char*>(rdn_value));
	//attr_values [0] = reinterpret_cast<void *>(rdn_value);
	ACS_CC_ValuesDefinitionType attribute0;
	attribute0.attrName = const_cast<char*>(led_id_attr_name);
	attribute0.attrType = (ACS_CC_AttrValueType) STRING_ATTRIBUTE;
	attribute0.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute0.attrValues = attr_values;
	_attr_list.push_back(attribute0);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", attribute0.attrName, reinterpret_cast<char *>(*attribute0.attrValues));

	//runtime attribute CACHED
	attr_values [1] = reinterpret_cast<void *>(& status);
	ACS_CC_ValuesDefinitionType attribute1;
	attribute1.attrName = const_cast<char*>(led_status_attr_name);
	attribute1.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute1.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute1.attrValues = attr_values + 1;
	_attr_list.push_back(attribute1);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", attribute1.attrName, *reinterpret_cast<int32_t *>(*attribute1.attrValues));

	attr_values [2] = reinterpret_cast<void *>(& supported);
	ACS_CC_ValuesDefinitionType attribute2;
	attribute2.attrName = const_cast<char*>(led_supported_attr_name);
	attribute2.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute2.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute2.attrValues = attr_values + 2;
	_attr_list.push_back(attribute2);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", attribute2.attrName, *reinterpret_cast<int32_t *>(*attribute2.attrValues));

	attr_values [3] = reinterpret_cast<void *>(& color);
	ACS_CC_ValuesDefinitionType attribute3;
	attribute3.attrName = const_cast<char*>(led_color_attr_name);
	attribute3.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	attribute3.attrValuesNum = SINGLE_ATTRIBUTE;
	attribute3.attrValues = attr_values + 3;
	_attr_list.push_back(attribute3);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", attribute3.attrName, *reinterpret_cast<int32_t *>(*attribute3.attrValues));

	int imm_call_result = -1;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IMM_PARENT_PATH '%s' - IMM_RDN_PATH == '%s'", imm_full_path, rdn_value);


#if defined (ACS_APBM_USE_IMM_CONFIGURATION_LED_OBJECTS)
	++_imm_operation_ongoing_counter;
	imm_call_result = _om_handler.createObject(led_type_str, imm_full_path, _attr_list); //CONFIGUTATION
	--_imm_operation_ongoing_counter;
#else
	imm_call_result = createRuntimeObj(led_type_str, imm_full_path, _attr_list); //RUNTIME
#endif

	if(imm_call_result != ACS_CC_SUCCESS){
		int imm_internal_last_error = 0;
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_LED_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in add_ap_board_led_info_by_type failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_CREATE_OBJ;
	}

	return 0;
}

int __CLASS_NAME__::modify_ap_board_status_attribute(const acs_apbm_apboardinfo * board) {
	ACS_CC_ImmParameter param2modify;
	int call_result = -1;
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	void * attr_value [1] = { const_cast<void *>(reinterpret_cast<const void *>(&board->board_status)) };
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, " attribute value == %d", attr_value);
	
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	//snprintf (imm_full_path, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%d,%s=%d,%s=%s,%s", ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID, board->slot_position, ACS_APBM_CONFIG_AP_BLADE_CLASS_ID, board->slot_position, ACS_APBM_CONFIG_MAGAZINE_CLASS_ID, ACS_APBM_DEFAULT_MAGAZINE_NUM/*board.magazine*/, root_dn);
	build_DN_from_magazine_and_slot(imm_full_path, board->slot_position, board->magazine);

	int imm_attribute_is_configuration = 0;

	param2modify.attrName = const_cast<char*>(board_status_attr_name);
	param2modify.attrType = (ACS_CC_AttrValueType) INTEGER_ATTRIBUTE;
	param2modify.attrValuesNum = SINGLE_ATTRIBUTE;
	param2modify.attrValues = attr_value;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", param2modify.attrName, *reinterpret_cast<int32_t *>(*param2modify.attrValues));

	int imm_call_result = -1;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	if (imm_attribute_is_configuration) {
		++_imm_operation_ongoing_counter;
		imm_call_result = _om_handler.modifyAttribute(imm_full_path, &param2modify);
		--_imm_operation_ongoing_counter;
	} else
		imm_call_result = acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(imm_full_path, &param2modify);

//	++_imm_operation_ongoing_counter;
//	imm_call_result = _om_handler.modifyAttribute(imm_full_path, &param2modify);
//	--_imm_operation_ongoing_counter;
#else
	imm_call_result = modifyRuntimeObj(imm_full_path, &param2modify);
#endif

	if(imm_call_result != ACS_CC_SUCCESS){
		int imm_internal_last_error = 0;
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
		if (imm_attribute_is_configuration) {
			imm_internal_last_error = _om_handler.getInternalLastError();
			imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
		} else {
			imm_internal_last_error = acs_apgcc_objectimplementerinterface_V3::getInternalLastError();
			imm_internal_last_error_text = acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText();
		}
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal IMM error in set_ap_board_attribute failure: call_result"
					" == '%d' error-test: %s error: %d",imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}

int __CLASS_NAME__::modify_ap_board_dn_attribute(acs_apbm_apboardinfo *board, attributes_names_t attribute ){

	int call_result = -1;

	if(!board){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error! It's empty object");
		return call_result;
	}


//	if (attribute != IMM_ATTRIBUTE_BOARD_MAINTENANCE_LED ||
//			attribute != IMM_ATTRIBUTE_BOARD_FAULT_LED ||
//			attribute != IMM_ATTRIBUTE_BOARD_OPERATIONAL_LED ||
//			attribute != IMM_ATTRIBUTE_BOARD_STATUS_LED ||
//			attribute != IMM_ATTRIBUTE_BOARD_PRODUCT_INFO ||
//			attribute != IMM_ATTRIBUTE_BOARD_NIC_INFO)
//		return call_result;

		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		char imm_dn_attr_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

		int imm_error_code = 0;
		//check parameter
		if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
			return call_result;
		//build DN
		axe_eth_shelf::led_type_t led;
		int led_change = 0,product_change = 0, nic_change = 0;
		switch(attribute){
		case IMM_ATTRIBUTE_BOARD_MAINTENANCE_LED:
			led = axe_eth_shelf::LED_TYPE_MAINTENANCE;
			led_change = 1;
			break;
		case IMM_ATTRIBUTE_BOARD_FAULT_LED:
			led = axe_eth_shelf::LED_TYPE_FAULT;
			led_change = 1;
			break;
		case IMM_ATTRIBUTE_BOARD_OPERATIONAL_LED:
			led = axe_eth_shelf::LED_TYPE_OPERATIONAL;
			led_change = 1;
			break;
		case IMM_ATTRIBUTE_BOARD_STATUS_LED:
			led = axe_eth_shelf::LED_TYPE_STATUS;
			led_change = 1;
			break;
		case IMM_ATTRIBUTE_BOARD_PRODUCT_INFO:
			product_change = 1;
			break;
		case IMM_ATTRIBUTE_BOARD_NIC_INFO:
			nic_change = 1;
			break;
		default:
			return -1;
		}

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Trying to update %s DN.",
				(led_change == 1) ? "led" : (nic_change == 1) ? "NIC" :
						(product_change == 1) ? "PRODUCT" : "UNDEFINED" );

		if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board->slot_position, board->magazine)) < 0)
			return call_result;

		//RETRIVE STRING DN TO INSERT IN ATTRIBUTE
		if (nic_change){

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Modifying NIC DN attribute");



			char magazine_str[16] = {0};
				acs_apbm_csreader::uint32_to_ip_format(magazine_str, board->magazine);

			const char * root_dn = blade_configuration_dn();
	
			int env = -1;
			if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
				return acs_apbm::ERR_UNKNOWN_ARCHITECTURE;
			}

			ACS_CC_ImmParameter nic_attribute;
			// Allocate memory for max size of possible NICs (currently NINE).
			char nic_path[NINE_ATTRIBUTE][ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {{0}};
			void* nic_attr_values[NINE_ATTRIBUTE] = {0};

			if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
			{
				for (int i=0; i < NINE_ATTRIBUTE; i++){
					snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s", nic_info_id_attr_name,
						ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i, //NIC0 - NIC1 -NIC2 - NIC3 - NIC4 - NIC5 - NIC6 - NIC7 - NIC8
						ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
						magazine_str,
						root_dn);
						nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
				}
				nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
				nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
				nic_attribute.attrValuesNum = NINE_ATTRIBUTE;
				nic_attribute.attrValues = nic_attr_values;
			}
			else if(_server_working_set->shelves_data_manager->is_hardwaretype_gep7())    
			{
				for (int i=0; i < FIVE_ATTRIBUTE; i++){
					snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s", nic_info_id_attr_name,
						ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i+2, //NIC2 - NIC3 - NIC4 - NIC5 - NIC6
						ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
						magazine_str,
						root_dn);
						nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
				}
				nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
				nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
				nic_attribute.attrValuesNum = FIVE_ATTRIBUTE;
				nic_attribute.attrValues = nic_attr_values;
                        }
			else if(_server_working_set->shelves_data_manager->is_10G())	//ETH5 and ETH6 SUPPORT
			{

				for (int i=0; i < SEVEN_ATTRIBUTE; i++){
					snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s", nic_info_id_attr_name,
						ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i, //NIC0 - NIC1 -NIC2 - NIC3 - NIC4 - NIC5 - NIC6
						ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
						magazine_str,
						root_dn);
						nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
				}
				nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
				nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
				nic_attribute.attrValuesNum = SEVEN_ATTRIBUTE;
				nic_attribute.attrValues = nic_attr_values;
			}
			else
			{
				for (int i=0; i < FIVE_ATTRIBUTE; i++){
					snprintf (nic_path[i], ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%s%d,%s=%d,%s=%d,%s=%s,%s",
						nic_info_id_attr_name,
						ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE, i, //NIC0 - NIC1 -NIC2 - NIC3 - NIC4
						ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_AP_BLADE_CLASS_ID,
						board->slot_position,
						ACS_APBM_CONFIG_MAGAZINE_CLASS_ID,
						magazine_str,
						root_dn);
						nic_attr_values [i] = reinterpret_cast<void *>(nic_path[i]);
						ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC_PATH[%d] value is : %s \n ", i, nic_path[i]);
				}


				nic_attribute.attrName = const_cast<char*>(nic_info_attr_name);
				nic_attribute.attrType = (ACS_CC_AttrValueType) SA_NAME_ATTRIBUTE;
				nic_attribute.attrValuesNum = FIVE_ATTRIBUTE;
				nic_attribute.attrValues = nic_attr_values;
			}

			char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
			build_DN_from_magazine_and_slot(imm_full_path, board->slot_position, board->magazine);

			ACS_CC_ReturnType imm_call_result = acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(imm_full_path, &nic_attribute);
			if (imm_call_result != ACS_CC_SUCCESS) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Internal IMM error in add_ap_board_nic_info failure: IMM error = %d, IMM error text == '%s'",
							acs_apgcc_objectimplementerinterface_V3::getInternalLastError(), acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());
				return acs_apbm::ERR_IMM_MODIFY_OBJ;
			}

			return 0;
		}

		if (led_change){
			if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_dn_attr_path, board->slot_position, board->magazine, led)) < 0)
				return call_result;
			else
						ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Modifying LED DN attribute");
		}
		if (product_change){
			if ( (call_result = build_DN_from_magazine_slot_and_product_info(imm_dn_attr_path, board->slot_position, board->magazine, ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO)) < 0)
				return call_result;
			else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Modifying PRODUCT DN attribute");
		}

		//MODIFY DN ATTRIBUTE
		call_result = modify_ap_board_attribute(imm_error_code, imm_full_path, attribute, imm_dn_attr_path);
		if (call_result != 0){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
		return 0;
}


//STRING TYPE - LED
int __CLASS_NAME__::modify_ap_board_led_attribute(int & imm_error_code, const char * imm_dn, leds_attributes_name_t attr_name, const char * value){
	int result = -1;
	void * attr_value [1] = {reinterpret_cast<void *>(const_cast<char*>(value))};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%s' ",
			imm_dn, attr_name, reinterpret_cast<char *>(*attr_value));
	if ((result = modify_led_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_STRING, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;

	return acs_apbm::ERR_NO_ERRORS;
}
//INTEGER TYPE - LED
int __CLASS_NAME__::modify_ap_board_led_attribute(int & imm_error_code, const char * imm_dn, leds_attributes_name_t attr_name, int32_t value ){
	int result = -1;
	void * attr_value [1] =  {reinterpret_cast<void *>(& value)};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%d' ",imm_dn, attr_name, *reinterpret_cast<int32_t *>(*attr_value));
	if ( (result = modify_led_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_INTEGER, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;

	return acs_apbm::ERR_NO_ERRORS;
}

//STRING TYPE - PRODUCTINFO
int __CLASS_NAME__::modify_ap_board_product_info_attribute(int & imm_error_code, const char * imm_dn, products_attributes_name_t attr_name, const char * value ){
	int result = -1;
	void * attr_value [1] = {reinterpret_cast<void *>(const_cast<char*>(value))};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%s' ",imm_dn, attr_name, reinterpret_cast<char *>(*attr_value));
	if ((result = modify_product_info_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_STRING, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;

	return acs_apbm::ERR_NO_ERRORS;
}
//INTEGER TYPE - PRODUCTINFO
int __CLASS_NAME__::modify_ap_board_product_info_attribute(int & imm_error_code, const char * imm_dn, products_attributes_name_t attr_name, int32_t value ){
	int result = -1;
	void * attr_value [1] =  {reinterpret_cast<void *>(& value)};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%d' ",imm_dn, attr_name, *reinterpret_cast<int32_t *>(*attr_value));
	if ( (result = modify_product_info_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_INTEGER, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;

	return acs_apbm::ERR_NO_ERRORS;
}

//STRING TYPE - NIC
int __CLASS_NAME__::modify_ap_board_nic_attribute(int & imm_error_code, const char * imm_dn, nics_attributes_name_t attr_name, const char * value ){
	int result = -1;
	void * attr_value [1] = {reinterpret_cast<void *>(const_cast<char*>(value))};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%s' ",imm_dn, attr_name, reinterpret_cast<char *>(*attr_value));
	if ((result = modify_nic_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_STRING, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
	return 0;
}
//INTEGER TYPE - NIC
int __CLASS_NAME__::modify_ap_board_nic_attribute(int & imm_error_code, const char * imm_dn, nics_attributes_name_t attr_name, int32_t value ){
	int result = -1;
	void * attr_value [1] =  {reinterpret_cast<void *>(& value)};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%d' ",imm_dn, attr_name, *reinterpret_cast<int32_t *>(*attr_value));
	if ( (result = modify_nic_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_INTEGER, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;

	return acs_apbm::ERR_NO_ERRORS;
}

//INTEGER TYPE
int __CLASS_NAME__::modify_ap_board_attribute(int & imm_error_code, const char * imm_dn, attributes_names_t attr_name, int32_t value ){
	int result = -1;
	void * attr_value [1] =  {reinterpret_cast<void *>(& value)};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%d' ",imm_dn, attr_name, *reinterpret_cast<int32_t *>(*attr_value));
	if ( (result = modify_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_INTEGER, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
	return 0;
}
//STRING TYPE
int __CLASS_NAME__::modify_ap_board_attribute(int & imm_error_code, const char * imm_dn, attributes_names_t attr_name, const char * value ){
	int result = -1;
	void * attr_value [1] = {reinterpret_cast<void *>(const_cast<char*>(value))};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN == '%s' NAME == '%d' VALUE == '%s' ",imm_dn, attr_name, reinterpret_cast<char *>(*attr_value));
	if ((result = modify_attribute(imm_error_code, imm_dn, attr_name, IMM_ATTRIBUTE_TYPE_STRING, attr_value)) < 0)
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
	return 0;
}

int __CLASS_NAME__::update_green_led_status_on_ap_board_info(acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path,board->slot_position, board->magazine, axe_eth_shelf::LED_TYPE_OPERATIONAL)) < 0)
		return call_result;

	call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->green_led);
	if (call_result != 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}
int __CLASS_NAME__::update_mia_led_status_on_ap_board_info(acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
        if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(board->fbn, board->slot_position, board->magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
                return ret_val;
        }

	if(board->fbn == ACS_CS_API_HWC_NS::FBN_GEA)
	{
		if(_server_working_set->shelves_data_manager->is_new_gea_supported)
		{
			 if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path,board->slot_position, board->magazine, axe_eth_shelf::LED_TYPE_STATUS)) < 0)
                	return call_result;
		}	
		else
		{
                	if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path,board->slot_position, board->magazine, axe_eth_shelf::LED_TYPE_MAINTENANCE)) < 0)
	                return call_result;
	        }
	}
	else
	{
		if(_server_working_set->shelves_data_manager->is_4led_supported)
		{
			if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path,board->slot_position, board->magazine, axe_eth_shelf::LED_TYPE_STATUS)) < 0)
				return call_result;
		}
		else
		{
			if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path,board->slot_position, board->magazine, axe_eth_shelf::LED_TYPE_MAINTENANCE)) < 0)
				return call_result;
		}
	}
	call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->mia_led);
	if (call_result != 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}
int __CLASS_NAME__::update_red_led_status_on_ap_board_info(acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path,board->slot_position, board->magazine, axe_eth_shelf::LED_TYPE_FAULT)) < 0)
		return call_result;

	call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->red_led);
	if (call_result != 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}
int __CLASS_NAME__::update_blue_led_status_on_ap_board_info(acs_apbm_apboardinfo *board){
        int call_result = -1;
        char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
        int imm_error_code = 0;
        //check parameter
        if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
                return call_result;
        //build DN
        if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path,board->slot_position, board->magazine, axe_eth_shelf::LED_TYPE_MAINTENANCE)) < 0)
                return call_result;

        call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->blue_led);
        if (call_result != 0){
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
                return acs_apbm::ERR_IMM_MODIFY_OBJ;
        }
        return 0;
}
int __CLASS_NAME__::update_bios_boot_run_mode_on_ap_board_info(acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board->slot_position, board->magazine)) < 0)
		return call_result;
	call_result = modify_ap_board_attribute(imm_error_code, imm_full_path,IMM_ATTRIBUTE_BOARD_BIOS_RUNNING_MODE, board->bios_running_mode);
	if (call_result != 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}

int __CLASS_NAME__::update_fatal_event_log_info_on_ap_board_info(acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board->slot_position, board->magazine)) < 0)
		return call_result;
	call_result = modify_ap_board_attribute(imm_error_code, imm_full_path,IMM_ATTRIBUTE_BOARD_FATAL_EVENT_LOG_INFO, board->fatalEventLogInfo);
	if (call_result != 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}

int __CLASS_NAME__::update_snmp_ap_board_info(acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	char imm_dn_path[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_dn_path, board->slot_position, board->magazine)) < 0)
		return call_result;
	if ( (call_result = build_DN_from_magazine_slot_and_product_info(imm_full_path,board->slot_position, board->magazine, ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO)) < 0)
		return call_result;
	//update product info DN
	call_result = modify_ap_board_attribute(imm_error_code, imm_dn_path, IMM_ATTRIBUTE_BOARD_PRODUCT_INFO, imm_full_path);
	//update product attribute
	call_result = modify_ap_board_product_info_attribute(imm_error_code, imm_full_path, IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_REVISION, board->product_revision_state);
	call_result = modify_ap_board_product_info_attribute(imm_error_code, imm_full_path, IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_NAME, board->product_name);
	call_result = modify_ap_board_product_info_attribute(imm_error_code, imm_full_path, IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_NUMBER, board->product_number);
	call_result = modify_ap_board_product_info_attribute(imm_error_code, imm_full_path, IMM_PRODUCT_ATTRIBUTE_BOARD_SERIAL_NUMBER, board->serial_number);
	call_result = modify_ap_board_product_info_attribute(imm_error_code, imm_full_path, IMM_PRODUCT_ATTRIBUTE_BOARD_MANUFACTURING_DATE, board->manufacture_date);
	call_result = modify_ap_board_product_info_attribute(imm_error_code, imm_full_path, IMM_PRODUCT_ATTRIBUTE_BOARD_VENDOR, board->vendor);

	//runtime not cached attribute not need update
	//call_result = modify_ap_board_attribute(imm_error_code,imm_full_path,IMM_ATTRIBUTE_BOARD_BIOS_RUNNING_MODE, board->bios_running_mode);
	//call_result = modify_ap_board_attribute(imm_error_code,imm_full_path,IMM_ATTRIBUTE_BOARD_FATAL_EVENT_LOG_INFO, board->fatalEventLogInfo);
	if (call_result != 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}

int __CLASS_NAME__::update_led_status_on_ap_board_info( acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;

	//GREEN LED
	if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path, board->slot_position,board->magazine,axe_eth_shelf::LED_TYPE_OPERATIONAL)) < 0)
		return call_result;
	call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->green_led);

	//RED LED
	if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path, board->slot_position,board->magazine,axe_eth_shelf::LED_TYPE_FAULT)) < 0)
		return call_result;
	call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->red_led);

	//MIA LED
	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
	if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(board->fbn, board->slot_position, board->magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
		return ret_val;
	}

	//Check whether we need to read four or three leds
	bool has_4_leds = (board->fbn == ACS_CS_API_HWC_NS::FBN_GEA)? _server_working_set->shelves_data_manager->is_new_gea_supported: _server_working_set->shelves_data_manager->is_4led_supported;

	if(!has_4_leds)
	{
		if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path, board->slot_position,board->magazine,axe_eth_shelf::LED_TYPE_MAINTENANCE)) < 0)
			return call_result;
		call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->mia_led);
	}
	else
	{
		if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path, board->slot_position,board->magazine,axe_eth_shelf::LED_TYPE_STATUS)) < 0)
			return call_result;
		call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->mia_led);

		//BLUE LED
		if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path, board->slot_position,board->magazine,axe_eth_shelf::LED_TYPE_MAINTENANCE)) < 0)
			return call_result;
		call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_STATUS, board->blue_led);
	}

	if (call_result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}

int __CLASS_NAME__::update_temperature_status_on_ap_board_info( acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board->slot_position,board->magazine)) < 0)
		return call_result;
	axe_eth_shelf::temperature_status_t temperature;
	if (strcmp(board->temperature_status,"N/A") ==0)
		temperature = axe_eth_shelf::TEMPERATURE_STATUS_NORMAL_TEMPERATURE;
		//temperature = axe_eth_shelf::TEMPERATURE_STATUS_UNDEFINED; assume normal state at start
	else if (strcmp(board->temperature_status,"OK") ==0)
		temperature = axe_eth_shelf::TEMPERATURE_STATUS_NORMAL_TEMPERATURE;
	else
		temperature = axe_eth_shelf::TEMPERATURE_STATUS_CRITICAL_TEMPERATURE;

	call_result = modify_ap_board_attribute(imm_error_code, imm_full_path,IMM_ATTRIBUTE_BOARD_TEMPERATURE_STATUS, temperature);
	if (call_result != 0){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
		return 0;
}

int __CLASS_NAME__::update_network_interfaces_cards_status_on_ap_board_info( acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	bool isGep7 =  _server_working_set->shelves_data_manager->is_hardwaretype_gep7();
	if(!isGep7)
	{
		//build DN
		if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_0)) < 0)
			return call_result;
		call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_0);

		if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_1)) < 0)
			return call_result;
		call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_1);
	}
	if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_2)) < 0)
		return call_result;
	call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_2);

	if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_3)) < 0)
		return call_result;
	call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_3);

	if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_4)) < 0)
		return call_result;
	call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_4);

	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return -1;
	}

	if((env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) || _server_working_set->shelves_data_manager->is_10G())
	{
		if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_5)) < 0)
			return call_result;
		call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_5);

		if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_6)) < 0)
			return call_result;
        call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_6);
	}

	if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_7)) < 0)
			return call_result;
		call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_7);

		if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, axe_eth_shelf::IMM_NIC_8)) < 0)
			return call_result;
		call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, board->nic_8);
	}

	if (call_result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}


int __CLASS_NAME__::update_network_interfaces_card_info_on_ap_board_info(axe_eth_shelf::nic_name_t nic ,
                                                                         acs_apbm_apboardinfo * board,
                                                                         const axe_eth_shelf::nic_status_t nic_status,
                                                                         const char * nic_name,
                                                                         const char * nic_ipv4_address,
                                                                         const char * nic_mac_address)
{

        int call_result = -1;
        char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
        int imm_error_code = 0;
        //check parameter
        if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
          return call_result;

        //build DN
        if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, nic)) < 0)
          return call_result;

        call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_STATUS, nic_status);
        if (call_result != 0)
         {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
            return acs_apbm::ERR_IMM_MODIFY_OBJ;
         }
        call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_NAME, nic_name);
        if (call_result != 0)
         {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
            return acs_apbm::ERR_IMM_MODIFY_OBJ;
         }
        call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_IP_ADDRESS, nic_ipv4_address);
        if (call_result != 0)
         {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
            return acs_apbm::ERR_IMM_MODIFY_OBJ;
         }
        call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_MAC_ADDRESS, nic_mac_address);
        if (call_result != 0)
         {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
            return acs_apbm::ERR_IMM_MODIFY_OBJ;
         }
        return acs_apbm::ERR_NO_ERRORS;

}

int __CLASS_NAME__::update_raid_status_on_ap_board_info( acs_apbm_apboardinfo *board){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"update_raid_status_on_ap_board_info!");
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board->slot_position,board->magazine)) < 0)
		return call_result;

	call_result = modify_ap_board_attribute(imm_error_code,imm_full_path,IMM_ATTRIBUTE_BOARD_RAID_DISK_STATUS, board->raid_disk_status);
	if (call_result != 0)
	{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"update_raid_status_on_ap_board_info success!");
	return 0;
}

int __CLASS_NAME__::update_drbd_status_on_ap_board_info( acs_apbm_apboardinfo *board){
        int call_result = -1;
        char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
        int imm_error_code = 0;
        //check parameter
        if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
                return call_result;
        //build DN
        if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board->slot_position,board->magazine)) < 0)
                return call_result;

        call_result = modify_ap_board_attribute(imm_error_code,imm_full_path,IMM_ATTRIBUTE_BOARD_RAID_DISK_STATUS, board->drbd_disk_status);
        if (call_result != 0)
        {
                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
                        return acs_apbm::ERR_IMM_MODIFY_OBJ;
        }
        return 0;
}

int __CLASS_NAME__::update_alarm_interface_status_on_ap_board_info( acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board->slot_position,board->magazine)) < 0)
		return call_result;

	call_result = modify_ap_board_attribute(imm_error_code,imm_full_path, IMM_ATTRIBUTE_BOARD_ALARM_DISK_INTERFACE, board->alarm_interface_status);
	if (call_result != 0)
	{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}

int __CLASS_NAME__::update_bios_version_on_ap_board_info( acs_apbm_apboardinfo *board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
					return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path, board->slot_position, board->magazine)) < 0)
					return call_result;

	call_result = modify_ap_board_attribute(imm_error_code,imm_full_path, IMM_ATTRIBUTE_BOARD_BIOS_VERSION, board->bios_version);
	if (call_result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return 0;
}

int __CLASS_NAME__::update_default_led_values_on_ap_board_info(acs_apbm_apboardinfo * board, const axe_eth_shelf::led_type_t led_type,  axe_eth_shelf::led_color_t color, bool supported){
	int call_result = -1;
		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		int imm_error_code = 0;
		//check parameter
		if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
			return call_result;

		attributes_names_t led_attr_name;
		switch(led_type){
		case axe_eth_shelf::LED_TYPE_FAULT :
			led_attr_name = IMM_ATTRIBUTE_BOARD_FAULT_LED;
				break;
		case axe_eth_shelf::LED_TYPE_MAINTENANCE :
			led_attr_name = IMM_ATTRIBUTE_BOARD_MAINTENANCE_LED;
				break;
		case axe_eth_shelf::LED_TYPE_OPERATIONAL :
			led_attr_name = IMM_ATTRIBUTE_BOARD_OPERATIONAL_LED;
				break;
		case axe_eth_shelf::LED_TYPE_STATUS :
			led_attr_name = IMM_ATTRIBUTE_BOARD_STATUS_LED;
				break;
		default:
			return -1;
		}

		if((call_result = modify_ap_board_dn_attribute(board,led_attr_name)) < 0)
		{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify DN attribute == '%d' ", call_result);
				return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}

		if ( (call_result = build_DN_from_magazine_slot_and_led_type(imm_full_path, board->slot_position,board->magazine, led_type)) < 0)
			return call_result;
		call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_COLOR, color);
		call_result = modify_ap_board_led_attribute(imm_error_code, imm_full_path, IMM_LED_ATTRIBUTE_SUPPORTED, supported);

		if (call_result != 0)
		{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
				return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
		return acs_apbm::ERR_NO_ERRORS;

}

int __CLASS_NAME__::update_ipmi_upgrade_result(acs_apbm_apboardinfo *board){

	int op_res = 0;
	int cba = 0;
	if(board->slot_position != acs_apbm::BOARD_SCB_LEFT && board->slot_position != acs_apbm::BOARD_SCB_RIGHT)
		op_res = update_ipmi_upgrade_result_on_ap_board_info(board);
	else if (!(cba = _server_working_set->cs_reader->is_cba_system()))
		op_res = update_ipmi_upgrade_result_on_scb(board);
	else {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid Request, check slot position %d or architecture %d",board->slot_position, cba);
	}
	if (op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update board ipmi upgrade last time ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_last_time(..)' failed. op_res == %d !", board->magazine, board->slot_position, op_res);
		return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
	}
	return acs_apbm::ERR_NO_ERRORS;

}

int __CLASS_NAME__::update_ipmi_upgrade_time(acs_apbm_apboardinfo *board){

	int op_res = 0;
	int cba = 0;
	if(board->slot_position != acs_apbm::BOARD_SCB_LEFT && board->slot_position != acs_apbm::BOARD_SCB_RIGHT)
		op_res = update_ipmi_upgrade_time_on_ap_board_info(board);
	else if (!(cba = _server_working_set->cs_reader->is_cba_system()))
		op_res = update_ipmi_upgrade_time_on_scb(board);
	else {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid Request, check slot position %d or architecture %d",board->slot_position, cba);
	}
	if (op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update board ipmi upgrade last time ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_last_time(..)' failed. op_res == %d !", board->magazine, board->slot_position, op_res);
		return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::update_ipmi_upgrade_result_on_ap_board_info(acs_apbm_apboardinfo *board){

	int call_result = -1;
		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		int imm_error_code = 0;
		//check parameter
		if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
						return call_result;
		//build DN
		if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path, board->slot_position, board->magazine)) < 0)
						return call_result;

		call_result = modify_ap_board_attribute(imm_error_code,imm_full_path, IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_RESULT, board->ipmi_upgrade_status);
		if (call_result != 0)
		{
										ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
										return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
		return 0;
}
int __CLASS_NAME__::update_ipmi_upgrade_time_on_ap_board_info(acs_apbm_apboardinfo *board){

		int call_result = -1;
		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		int imm_error_code = 0;
		//check parameter
		if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
						return call_result;
		//build DN
		if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path, board->slot_position, board->magazine)) < 0)
						return call_result;

		call_result = modify_ap_board_attribute(imm_error_code,imm_full_path, IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_TIME, board->ipmi_upgrade_last_time);
		if (call_result != 0)
		{
										ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
										return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
		return 0;
}

int __CLASS_NAME__::update_ipmi_upgrade_result_on_scb(acs_apbm_apboardinfo *board){

	int call_result = -1;
		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		int imm_error_code = 0;
		::snprintf(imm_full_path, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d", board->slot_position);

		call_result = modify_ap_board_attribute(imm_error_code,imm_full_path, IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_RESULT, board->ipmi_upgrade_status);
		if (call_result != 0)
		{
										ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
										return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
		return 0;
}
int __CLASS_NAME__::update_ipmi_upgrade_time_on_scb(acs_apbm_apboardinfo *board){

		int imm_error_code = 0;
		int call_result = -1;
		char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
		::snprintf(imm_full_path, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d", board->slot_position);

		call_result = modify_ap_board_attribute(imm_error_code,imm_full_path, IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_TIME, board->ipmi_upgrade_last_time);
		if (call_result != 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
		return 0;
}

int __CLASS_NAME__::update_all_default_nic_name_on_ap_board_info(acs_apbm_apboardinfo * p_apboardinfo, const char * /*default_nic_name*/){
	int call_result = -1;

	//UPDATE NIC DN
	call_result = modify_ap_board_dn_attribute(p_apboardinfo, IMM_ATTRIBUTE_BOARD_NIC_INFO);
	if (call_result < 0) return call_result;

	if(!_server_working_set->shelves_data_manager->is_hardwaretype_gep7())
	{
		call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_0, p_apboardinfo,"eth0");
		if (call_result < 0) return call_result;
		call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_1, p_apboardinfo,"eth1");
		if (call_result < 0) return call_result;
	}
	call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_2, p_apboardinfo,"eth2");
	if (call_result < 0) return call_result;
	call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_3, p_apboardinfo,"eth3");
	if (call_result < 0) return call_result;
	call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_4, p_apboardinfo,"eth4");
	if (call_result < 0) return call_result;
	
	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return acs_apbm::ERR_UNKNOWN_ARCHITECTURE;
	}

    if((env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) || _server_working_set->shelves_data_manager->is_10G())
	{
		call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_5, p_apboardinfo,"eth5");
		if (call_result < 0) return call_result;
 		call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_6, p_apboardinfo,"eth6");
 		if (call_result < 0) return call_result;
	}

    if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_7, p_apboardinfo,"eth7");
		if (call_result < 0) return call_result;
		call_result = update_nic_name_on_ap_board_info(axe_eth_shelf::IMM_NIC_8, p_apboardinfo,"eth8");
		if (call_result < 0) return call_result;
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::update_all_default_mac_address_on_ap_board_info(acs_apbm_apboardinfo * p_apboardinfo, const char * default_mac){
	int call_result = -1;
	
	bool isGep7 = _server_working_set->shelves_data_manager->is_hardwaretype_gep7();	
	if(!isGep7)
	{
		call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_0, p_apboardinfo, default_mac);
		if (call_result < 0) return call_result;
		call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_1, p_apboardinfo, default_mac);
		if (call_result < 0) return call_result;
	}
	call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_2, p_apboardinfo, default_mac);
	if (call_result < 0) return call_result;
	call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_3, p_apboardinfo, default_mac);
	if (call_result < 0) return call_result;
	call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_4, p_apboardinfo, default_mac);
	if (call_result < 0) return call_result;
	
	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return acs_apbm::ERR_UNKNOWN_ARCHITECTURE;
	}

	if((env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) || _server_working_set->shelves_data_manager->is_10G())
	{
		call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_5, p_apboardinfo, default_mac);
		if (call_result < 0) return call_result;
		call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_6, p_apboardinfo, default_mac);
		if (call_result < 0) return call_result;
	}

	if((env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED))
	{
		call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_7, p_apboardinfo, default_mac);
		if (call_result < 0) return call_result;
		call_result = update_mac_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_8, p_apboardinfo, default_mac);
		if (call_result < 0) return call_result;
	}
	

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::update_all_default_ip_address_on_ap_board_info(acs_apbm_apboardinfo * p_apboardinfo, const char * default_ip){
	int call_result = -1;
	bool isGep7 = _server_working_set->shelves_data_manager->is_hardwaretype_gep7();

	if(!isGep7)
	{
		call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_0, p_apboardinfo, default_ip);
		if (call_result < 0) return call_result;
		call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_1, p_apboardinfo, default_ip);
		if (call_result < 0) return call_result;
	}
	call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_2, p_apboardinfo, default_ip);
	if (call_result < 0) return call_result;
	call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_3, p_apboardinfo, default_ip);
	if (call_result < 0) return call_result;
	call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_4, p_apboardinfo, default_ip);
	if (call_result < 0) return call_result;

	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return acs_apbm::ERR_UNKNOWN_ARCHITECTURE;
	}
	
	if((env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) || _server_working_set->shelves_data_manager->is_10G())
	{
		call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_5, p_apboardinfo, default_ip);
		if (call_result < 0) return call_result;
		call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_6, p_apboardinfo, default_ip);
		if (call_result < 0) return call_result;
	}

	if((env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED))
	{
		call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_7, p_apboardinfo, default_ip);
		if (call_result < 0) return call_result;
		call_result = update_ip_address_on_ap_board_info(axe_eth_shelf::IMM_NIC_8, p_apboardinfo, default_ip);
		if (call_result < 0) return call_result;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::update_nic_name_on_ap_board_info(axe_eth_shelf::nic_name_t nic ,acs_apbm_apboardinfo * board, const char * nic_name){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;

	//build DN
	if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, nic)) < 0)
		return call_result;
	call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_NAME, nic_name);
	if (call_result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::update_mac_address_on_ap_board_info(axe_eth_shelf::nic_name_t nic ,acs_apbm_apboardinfo * board, const char * mac_address){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;

	//build DN
	if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, nic)) < 0)
		return call_result;
	call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_MAC_ADDRESS, mac_address);
	if (call_result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::update_ip_address_on_ap_board_info(axe_eth_shelf::nic_name_t nic ,acs_apbm_apboardinfo * board, const char * ip_address){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	int imm_error_code = 0;
	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;

	//build DN
	if ( (call_result = build_DN_from_magazine_slot_and_nic_info(imm_full_path,board->slot_position,board->magazine, nic)) < 0)
		return call_result;
	call_result = modify_ap_board_nic_attribute(imm_error_code, imm_full_path, IMM_NIC_ATTRIBUTE_IP_ADDRESS, ip_address);
	if (call_result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::reset_ap_board_info(acs_apbm_apboardinfo * board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

	//check parameter
	if ((call_result = _check_parameter(board->slot_position, board->magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path, board->slot_position, board->magazine)) < 0)
		return call_result;

	((call_result = update_snmp_ap_board_info(board)) == 0)
			? ACS_APBM_LOG(LOG_LEVEL_ERROR, "reset snmp ap board info OK ")
			: ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in update_snmp_ap_board_info == '%d' ", call_result);

	((call_result = update_temperature_status_on_ap_board_info(board)) == 0)
			? ACS_APBM_LOG(LOG_LEVEL_ERROR, "reset temperature status on ap board info OK ")
			: ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in update_temperature_status_on_ap_board_info == '%d' ", call_result);

	if (call_result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}

	return 0;
}

int __CLASS_NAME__::modify_attribute(int & imm_internal_last_error, const char * imm_dn, attributes_names_t attr_name, attributes_types_t type, void **attr_value){
	ACS_CC_ImmParameter param2modify;
	imm_internal_last_error = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "imm_dn == %s - attr_name == %d - type == %d",imm_dn, attr_name , type);

	int imm_attribute_is_configuration = 0;

	switch (attr_name) {
	case IMM_ATTRIBUTE_BOARD_BIOS_RUNNING_MODE :
		param2modify.attrName = const_cast<char*>(bios_running_mode_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", bios_running_mode_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_FATAL_EVENT_LOG_INFO :
		param2modify.attrName = const_cast<char*>(fatal_event_log_info_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", fatal_event_log_info_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_TEMPERATURE_STATUS :
		param2modify.attrName = const_cast<char*>(temperature_status_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", temperature_status_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_ALARM_DISK_INTERFACE :
		param2modify.attrName = const_cast<char*>(alarm_interface_status_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", alarm_interface_status_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_RAID_DISK_STATUS :
		param2modify.attrName = const_cast<char*>(raid_disk_status_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", raid_disk_status_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_DRBD_DISK_STATUS :
                param2modify.attrName = const_cast<char*>(drbd_disk_status_attr_name);
                ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", drbd_disk_status_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
                break;
	case IMM_ATTRIBUTE_BOARD_STATUS :
		param2modify.attrName = const_cast<char*>(board_status_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", board_status_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_BIOS_VERSION :
		param2modify.attrName = const_cast<char*>(bios_version_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", bios_version_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_PRODUCT_INFO :
		param2modify.attrName = const_cast<char*>(product_info_attr_name);
		type = IMM_ATTRIBUTE_TYPE_SA_NAME;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", product_info_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_RESULT :
		param2modify.attrName = const_cast<char*>(ipmi_upgrade_result_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", ipmi_upgrade_result_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
		imm_attribute_is_configuration = 1;
		break;
	case IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_TIME :
		param2modify.attrName = const_cast<char*>(ipmi_upgrade_time_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", ipmi_upgrade_time_attr_name, reinterpret_cast<char *>(*attr_value));
		imm_attribute_is_configuration = 1;
		break;
	case IMM_ATTRIBUTE_BOARD_NIC_INFO :
		param2modify.attrName = const_cast<char*>(nic_info_attr_name);
		type = IMM_ATTRIBUTE_TYPE_SA_NAME;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", nic_info_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_OPERATIONAL_LED :
		param2modify.attrName = const_cast<char*>(operational_led_attr_name);
		type = IMM_ATTRIBUTE_TYPE_SA_NAME;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", operational_led_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_FAULT_LED :
		param2modify.attrName = const_cast<char*>(fault_led_attr_name);
		type = IMM_ATTRIBUTE_TYPE_SA_NAME;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", fault_led_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_MAINTENANCE_LED :
		param2modify.attrName = const_cast<char*>(maintenance_led_attr_name);
		type = IMM_ATTRIBUTE_TYPE_SA_NAME;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", maintenance_led_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_ATTRIBUTE_BOARD_STATUS_LED :
		param2modify.attrName = const_cast<char*>(status_led_attr_name);
		type = IMM_ATTRIBUTE_TYPE_SA_NAME;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", status_led_attr_name, reinterpret_cast<char *>(*attr_value));
		break;

	default :
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid parameter - failure: %d not exist", attr_name);
		return acs_apbm::ERR_IMM_ATTRIBUTE_NOT_FOUND;
		break;
	}

	param2modify.attrType = (ACS_CC_AttrValueType) type;
	param2modify.attrValuesNum = SINGLE_ATTRIBUTE;
	param2modify.attrValues = attr_value;
	//USED TO TEST
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", param2modify.attrName,  reinterpret_cast<char *>(*param2modify.attrValues) );
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", param2modify.attrName,  *reinterpret_cast<int32_t *>(*param2modify.attrValues));

	int imm_call_result = -1;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS is DEFINED");

	if (imm_attribute_is_configuration) {
		++_imm_operation_ongoing_counter;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CALLING _om_handler.modifyAttribute()");
		imm_call_result = _om_handler.modifyAttribute(imm_dn, &param2modify);
		--_imm_operation_ongoing_counter;
	} else {
		imm_call_result = acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(imm_dn, &param2modify);
	}
#else
	imm_call_result = ACS_APGCC_RuntimeOwner_V2::modifyRuntimeObj(imm_dn, &param2modify);
#endif

	if (imm_call_result != ACS_CC_SUCCESS) {
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
		imm_internal_last_error = (imm_attribute_is_configuration
				? _om_handler.getInternalLastError()
				: acs_apgcc_objectimplementerinterface_V3::getInternalLastError());

		imm_internal_last_error_text = (imm_attribute_is_configuration
				? _om_handler.getInternalLastErrorText()
				: acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());
#else
		imm_internal_last_error = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		if(imm_internal_last_error == IMM_OBJECT_NOT_EXIST){
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Attribute successfully modified");
	return 0;
}

int __CLASS_NAME__::modify_product_info_attribute(int & imm_error_code, const char * imm_dn, products_attributes_name_t attr_name, attributes_types_t type, void **attr_value){
	ACS_CC_ImmParameter param2modify;
	imm_error_code = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "imm_dn == %s - attr_name == %d - type == %d",imm_dn, attr_name , type);

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_PRODUCT_OBJECTS)
	int imm_attribute_is_configuration = 0;
#endif

	switch(attr_name) {
	case IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_REVISION :
		param2modify.attrName = const_cast<char*>(product_revision_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", product_revision_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_NUMBER :
		param2modify.attrName = const_cast<char*>(product_number_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", product_number_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_NAME :
		param2modify.attrName = const_cast<char*>(product_name_att_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", product_name_att_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_PRODUCT_ATTRIBUTE_BOARD_SERIAL_NUMBER :
		param2modify.attrName = const_cast<char*>(serial_number_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", serial_number_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_PRODUCT_ATTRIBUTE_BOARD_MANUFACTURING_DATE :
		param2modify.attrName = const_cast<char*>(manufacturing_date_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", manufacturing_date_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	case IMM_PRODUCT_ATTRIBUTE_BOARD_VENDOR :
		param2modify.attrName = const_cast<char*>(vendor_attr_name);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", vendor_attr_name, reinterpret_cast<char *>(*attr_value));
		break;
	}

		//TODO - -HANDLE MODIFY OBJ
	param2modify.attrType = (ACS_CC_AttrValueType) type;
	param2modify.attrValuesNum = SINGLE_ATTRIBUTE;
	param2modify.attrValues = attr_value;
	//USED TO TEST
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", param2modify.attrName,  reinterpret_cast<char *>(*param2modify.attrValues) );
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", param2modify.attrName,  *reinterpret_cast<int32_t *>(*param2modify.attrValues));

	int imm_call_result = -1;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_PRODUCT_OBJECTS)
	if (imm_attribute_is_configuration) {
		++_imm_operation_ongoing_counter;
		imm_call_result = _om_handler.modifyAttribute(imm_dn, &param2modify);
		--_imm_operation_ongoing_counter;
	} else
		imm_call_result = acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(imm_dn, &param2modify);
#else
	imm_call_result = ACS_APGCC_RuntimeOwner_V2::modifyRuntimeObj(imm_dn, &param2modify);
#endif

	if(imm_call_result != ACS_CC_SUCCESS) {
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_PRODUCT_OBJECTS)
		imm_error_code = (imm_attribute_is_configuration
				? _om_handler.getInternalLastError()
				: acs_apgcc_objectimplementerinterface_V3::getInternalLastError());

		imm_internal_last_error_text = (imm_attribute_is_configuration
				? _om_handler.getInternalLastErrorText()
				: acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText());
#else
		imm_error_code = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		if(imm_error_code == IMM_OBJECT_NOT_EXIST){
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_error_code);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_error_code);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
	}

	return 0;
}

int __CLASS_NAME__::modify_led_attribute(int & imm_error_code, const char * imm_dn, leds_attributes_name_t attr_name, attributes_types_t type, void **attr_value){
	ACS_CC_ImmParameter param2modify;
	imm_error_code = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "imm_dn == %s - attr_name == %d - type == %d",imm_dn, attr_name , type);

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_LED_OBJECTS)
	int imm_attribute_is_configuration = 0;
#endif

	switch(attr_name){
	case IMM_LED_ATTRIBUTE_COLOR :
			param2modify.attrName = const_cast<char*>(led_color_attr_name);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", led_color_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
			break;
	case IMM_LED_ATTRIBUTE_STATUS :
			param2modify.attrName = const_cast<char*>(led_status_attr_name);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", led_status_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
			break;
	case IMM_LED_ATTRIBUTE_SUPPORTED :
			param2modify.attrName = const_cast<char*>(led_supported_attr_name);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", led_supported_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
			break;
	}

	//TODO- HANDLE LED MODIFY OBJ
	param2modify.attrType = (ACS_CC_AttrValueType) type;
	param2modify.attrValuesNum = SINGLE_ATTRIBUTE;
	param2modify.attrValues = attr_value;
	//USED TO TEST
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", param2modify.attrName,  reinterpret_cast<char *>(*param2modify.attrValues) );
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", param2modify.attrName,  *reinterpret_cast<int32_t *>(*param2modify.attrValues));

	int imm_call_result = -1;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_LED_OBJECTS)
	if (imm_attribute_is_configuration) {
		++_imm_operation_ongoing_counter;
		imm_call_result = _om_handler.modifyAttribute(imm_dn, &param2modify);
		--_imm_operation_ongoing_counter;
	} else
		imm_call_result = acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(imm_dn, &param2modify);
#else
	imm_call_result = ACS_APGCC_RuntimeOwner_V2::modifyRuntimeObj(imm_dn, &param2modify);
#endif

	if(imm_call_result != ACS_CC_SUCCESS){
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_LED_OBJECTS)
		if (imm_attribute_is_configuration) {
			imm_error_code = _om_handler.getInternalLastError();
			imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
		} else {
			imm_error_code = acs_apgcc_objectimplementerinterface_V3::getInternalLastError();
			imm_internal_last_error_text = acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText();
		}
#else
		imm_error_code = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		if(imm_error_code == IMM_OBJECT_NOT_EXIST){
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_error_code);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_error_code);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
	}

	return 0;
}

int __CLASS_NAME__::modify_nic_attribute(int & imm_error_code, const char * imm_dn, nics_attributes_name_t attr_name, attributes_types_t type, void **attr_value){
	ACS_CC_ImmParameter param2modify;
	imm_error_code = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "imm_dn == %s - attr_name == %d - type == %d",imm_dn, attr_name , type);

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_NIC_OBJECTS)
	int imm_attribute_is_configuration = 0;
#endif

	switch(attr_name){
	case IMM_NIC_ATTRIBUTE_NAME :
			param2modify.attrName = const_cast<char*>(nic_name_attr_name);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", nic_name_attr_name, reinterpret_cast<char *>(*attr_value));
			break;
	case IMM_NIC_ATTRIBUTE_STATUS :
			param2modify.attrName = const_cast<char*>(nic_status_attr_name);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%d' ", nic_status_attr_name, *reinterpret_cast<int32_t *>(*attr_value));
			break;
	case IMM_NIC_ATTRIBUTE_MAC_ADDRESS :
			param2modify.attrName = const_cast<char*>(nic_mac_address_attr_name);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", nic_mac_address_attr_name, reinterpret_cast<char *>(*attr_value));
			break;
	case IMM_NIC_ATTRIBUTE_IP_ADDRESS :
			param2modify.attrName = const_cast<char*>(nic_ip_address_attr_name);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Selected modify of attribute  == '%s' with value == '%s' ", nic_ip_address_attr_name, reinterpret_cast<char *>(*attr_value));
			break;
	}

	//TODO - HANDLE NIC MODIFY OBJ
	param2modify.attrType = (ACS_CC_AttrValueType) type;
	param2modify.attrValuesNum = SINGLE_ATTRIBUTE;
	param2modify.attrValues = attr_value;
	//USED TO TEST
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%s' ", param2modify.attrName,  reinterpret_cast<char *>(*param2modify.attrValues) );
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NAME=VALUE == '%s=%d' ", param2modify.attrName,  *reinterpret_cast<int32_t *>(*param2modify.attrValues));

	int imm_call_result = -1;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_NIC_OBJECTS)
	if (imm_attribute_is_configuration) {
		++_imm_operation_ongoing_counter;
		imm_call_result = _om_handler.modifyAttribute(imm_dn, &param2modify);
		--_imm_operation_ongoing_counter;
	} else
		imm_call_result = acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(imm_dn, &param2modify);
#else
	
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "imm_call_result value for %s :%d", imm_dn, imm_call_result); 
	imm_call_result = ACS_APGCC_RuntimeOwner_V2::modifyRuntimeObj(imm_dn, &param2modify);
#endif
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "imm_call_result value for %s :%d", imm_dn, imm_call_result); 
	if(imm_call_result != ACS_CC_SUCCESS){
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_NIC_OBJECTS)
		if (imm_attribute_is_configuration) {
			imm_error_code = _om_handler.getInternalLastError();
			imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
		} else {
			imm_error_code = acs_apgcc_objectimplementerinterface_V3::getInternalLastError();
			imm_internal_last_error_text = acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText();
		}
#else		
		imm_error_code = ACS_APGCC_RuntimeOwner_V2::getInternalLastError();
		imm_internal_last_error_text = ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText();
#endif

		if(imm_error_code == IMM_OBJECT_NOT_EXIST){
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_error_code);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal IMM error in modify_attribute: call_result"
					" == '%d' error-test: %s error: %d", imm_call_result, imm_internal_last_error_text, imm_error_code);
			return acs_apbm::ERR_IMM_MODIFY_OBJ;
		}
	}

	return 0;
}


int __CLASS_NAME__::set_ap_board_info(acs_apbm_apboardinfo & board){

	// Check if APBM has the ownership of IMM  objects
	if (_imm_ownership_state != ACS_APBM_IMM_OWNERSHIP_STATE_EVENT_HANDLED) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot add AP board info into IMM because APBM is not yet the OWNER of IMM!");
		return acs_apbm::ERR_TAKING_IMM_OBJECTS_OWNERSHIP;
	}

	int call_result = -1;
	ACS_CC_ReturnType imm_call_result;

	char imm_dnName [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	char led_dn [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	char nic_dn [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	char product_dn [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

	// some data (for example NIC data) to be stored in IMM are dependent from APG shelf architecture
	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return acs_apbm::ERR_UNKNOWN_ARCHITECTURE;
	}

	build_DN_from_magazine_slot_and_led_type(led_dn,board.slot_position, board.magazine, axe_eth_shelf::LED_TYPE_STATUS);

	if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
		build_DN_from_magazine_slot_and_nic_info(nic_dn,board.slot_position, board.magazine, axe_eth_shelf::IMM_NIC_8);
	else if(_server_working_set->shelves_data_manager->is_10G())
		build_DN_from_magazine_slot_and_nic_info(nic_dn,board.slot_position, board.magazine, axe_eth_shelf::IMM_NIC_6);
	else
		build_DN_from_magazine_slot_and_nic_info(nic_dn,board.slot_position, board.magazine, axe_eth_shelf::IMM_NIC_4);

    build_DN_from_magazine_slot_and_product_info(product_dn, board.slot_position, board.magazine,ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO);
	build_DN_from_magazine_and_slot(imm_dnName, board.slot_position, board.magazine);

	int imm_error_code = 0;

	call_result = modify_ap_board_attribute(imm_error_code, imm_dnName, IMM_ATTRIBUTE_BOARD_STATUS, board.board_status);

	if ((call_result < 0) && (imm_error_code == -12)){ // -12 == IMM OBJECT NOT_EXISTS
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Object %s not found! - imm result == %d. Trying to create object.", imm_dnName, imm_error_code);
		if ((call_result = add_ap_board_info(board)) < 0) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error: object %s not created! - result == %d ", imm_dnName, call_result);
			return call_result;
		} else
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Object %s successfully created! - result == %d ", imm_dnName, imm_call_result);
		return 0;
	}
	else if (call_result < 0) { //ERROR on parent object...it will retry
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
		return acs_apbm::ERR_IMM_MODIFY_OBJ;
	}
	else { // (call_result == 0)
		//successfully to modify parent attribute, it needed to check the others object exist!
		//if it miss the last one led object created, we assume that any others is missing so the server try to create
		call_result = modify_ap_board_led_attribute(imm_error_code, led_dn, IMM_LED_ATTRIBUTE_STATUS, board.green_led);
		if ((call_result < 0) && (imm_error_code == -12)){ // -12 == IMM OBJECT NOT_EXISTS
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Led Object %s not found! - imm result == %d. Trying to create object.", led_dn, imm_error_code);
			if ((call_result = add_ap_board_led_info(board)) < 0) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error: object %s not created! - result == %d ", led_dn, call_result);
				return call_result;
			}
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Led Object %s successfully created! - result == %d ", led_dn, imm_call_result);

		}
		call_result = modify_ap_board_product_info_attribute(imm_error_code, product_dn, IMM_PRODUCT_ATTRIBUTE_BOARD_SERIAL_NUMBER, board.serial_number);
		if ((call_result < 0) && (imm_error_code == -12)){ // -12 == IMM OBJECT NOT_EXISTS
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Product Object %s not found! - imm result == %d. Trying to create object.", product_dn, imm_error_code);
			if ((call_result = add_ap_board_product_info(board)) < 0) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error: object %s not created! - result == %d ", product_dn, call_result);
				return call_result;
			}
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Product Object %s successfully created! - result == %d ", product_dn, imm_call_result);

		}
		//if it miss the last one nic object created,we assume that any others is missing so the server try to create
		if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
			call_result = modify_ap_board_nic_attribute(imm_error_code, nic_dn, IMM_NIC_ATTRIBUTE_STATUS, board.nic_8);
		else if(_server_working_set->shelves_data_manager->is_10G())
			call_result = modify_ap_board_nic_attribute(imm_error_code, nic_dn, IMM_NIC_ATTRIBUTE_STATUS, board.nic_6);
		else
        	call_result = modify_ap_board_nic_attribute(imm_error_code, nic_dn, IMM_NIC_ATTRIBUTE_STATUS, board.nic_4);

        if ((call_result < 0) && (imm_error_code == -12)){ // -12 == IMM OBJECT NOT_EXISTS
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Nic Object %s not found! - imm result == %d. Trying to create object.", nic_dn, imm_error_code);
			if ((call_result = add_ap_board_nic_info(board)) < 0) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error: object %s not created! - result == %d ", nic_dn, call_result);
				return call_result;
			}
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Nic Object %s successfully created! - result == %d ", nic_dn, imm_call_result);
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Object %s successfully updated! - result == %d ", imm_dnName, imm_call_result);

	return 0;
}

int __CLASS_NAME__::set_ap_board_info_for_scb(acs_apbm_apboardinfo & board){
  int call_result = -1;
	ACS_CC_ReturnType imm_call_result;
	char imm_dnName [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

	::snprintf (imm_dnName, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX,"%s=%d", ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID, board.slot_position);

	int imm_error_code = 0;
	call_result = modify_ap_board_attribute(imm_error_code, imm_dnName, IMM_ATTRIBUTE_BOARD_STATUS, board.board_status);
	call_result = modify_ap_board_attribute(imm_error_code, imm_dnName, IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_RESULT, board.ipmi_upgrade_status); //to reset value when occurs a reboot during upgrade
        if (call_result < 0 && imm_error_code == -12){ // -12 == IMM OBJECT NOT_EXISTS
            ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Object %s not found! - imm result == %d. Trying to create object.", imm_dnName, imm_error_code);
            if ( (call_result = add_ap_board_info_for_scb(board)) < 0){
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error: object %s not created! - result == %d ", imm_dnName, call_result);
                return call_result;
            }
             ACS_APBM_LOG(LOG_LEVEL_INFO, "Object %s successfully created! - result == %d ", imm_dnName, imm_call_result);
              return 0;
         }
        if ( call_result < 0 ){
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in modify attribute == '%d' ", call_result);
            return acs_apbm::ERR_IMM_MODIFY_OBJ;
        }
        ACS_APBM_LOG(LOG_LEVEL_INFO, "Object %s successfully updated! - result == %d ", imm_dnName, imm_call_result);
	return 0;
}

int __CLASS_NAME__::remove_ap_board_info(const acs_apbm_apboardinfo & board){
	int call_result = -1;
	char imm_full_path [ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

	//check parameter
	if ((call_result = _check_parameter(board.slot_position, board.magazine)) < 0)
		return call_result;
	//build DN
	if ( (call_result = build_DN_from_magazine_and_slot(imm_full_path,board.slot_position, board.magazine)) < 0)
		return call_result;

	int imm_call_result = -1;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Imm dn path == '%s' ", imm_full_path);

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	++_imm_operation_ongoing_counter;
	imm_call_result = _om_handler.deleteObject(imm_full_path);
	--_imm_operation_ongoing_counter;
#else
	imm_call_result = deleteRuntimeObj(imm_full_path);
#endif

	if(imm_call_result != ACS_CC_SUCCESS){
		int imm_internal_last_error = 0;
		const char * imm_internal_last_error_text = 0;

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
		imm_internal_last_error = _om_handler.getInternalLastError();
		imm_internal_last_error_text = _om_handler.getInternalLastErrorText();
#else
		imm_internal_last_error = getInternalLastError();
		imm_internal_last_error_text = getInternalLastErrorText();
#endif

		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Internal IMM error in remove_ap_board_info failure: call_result == '%d' error-test: %s error: %d",
				imm_call_result, imm_internal_last_error_text, imm_internal_last_error);
		return acs_apbm::ERR_IMM_DELETE_OBJ;
	}

	return 0;
}

int __CLASS_NAME__::remove_ap_board_info(const char * imm_full_path){
	int imm_call_result = -1;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Removing AP Board: Imm dn path == '%s' ", imm_full_path);

#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	++_imm_operation_ongoing_counter;
	imm_call_result = _om_handler.deleteObject(imm_full_path);
	--_imm_operation_ongoing_counter;
#else
	imm_call_result = deleteRuntimeObj(imm_full_path);
#endif

	if(imm_call_result != ACS_CC_SUCCESS){
		//ACS_APBM_LOG(LOG_LEVEL_ERROR, "Internal IMM error in remove_ap_board_info failure: call_result == '%d' error-test: %s error: %d",imm_call_result, getInternalLastErrorText(), getInternalLastError());
		return acs_apbm::ERR_IMM_DELETE_OBJ;
	}

	return 0;
}


int __CLASS_NAME__::remove_all_ap_board_info() {
	int result = 0;
	char dn_imm[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	uint32_t magazine = 0;
	_server_working_set->cs_reader->get_my_magazine(magazine);

	//TODO: Use macro or constants for the following 25 literal
	for(int32_t slot=1; slot < 25; slot++)
	{
		build_DN_from_magazine_and_slot(dn_imm, slot, magazine);
		result = remove_ap_board_info(dn_imm);
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "remove_all_ap_board_info result =  %d", result);

	return 0;
}

int __CLASS_NAME__::handle_block_action(uint32_t slot, int32_t magazine)
{
	int16_t imm_error_code = 0;
	/*acs_apbm_shelvesdatamanager::sdm_constant op_res =*/ _server_working_set->shelves_data_manager->request_block_on_ap_board(imm_error_code, slot, magazine);
	return imm_error_code;
}


int __CLASS_NAME__::handle_deblock_action(uint32_t slot, int32_t magazine)
{
	int16_t imm_error_code = 0;
	/*acs_apbm_shelvesdatamanager::sdm_constant op_res = */_server_working_set->shelves_data_manager->request_deblock_on_ap_board(imm_error_code, slot, magazine);
	return imm_error_code;
}

int __CLASS_NAME__::handle_reset_action(uint32_t slot, int32_t magazine)
{
	int16_t imm_error_code = 0;
	/*acs_apbm_shelvesdatamanager::sdm_constant op_res = */_server_working_set->shelves_data_manager->request_reset_on_ap_board(imm_error_code, slot, magazine);
	return imm_error_code;
}



int __CLASS_NAME__::handle_ipmiupgrade_operation(const char * firmware_image_path, int32_t slot, uint32_t magazine, uint8_t comport)
//int __CLASS_NAME__::handle_ipmiupgrade_action(uint32_t slot, int32_t magazine, const char* p_objName, ACS_APGCC_InvocationType invocation, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType** paramList)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE.... acs_apbm_immrepositoryhandler::handle_ipmiupgrade_operation() firmware_image_path = %s , slot = %d , comport = %d\n",firmware_image_path,slot,comport);
	ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE.... acs_apbm_immrepositoryhandler::handle_ipmiupgrade_operation() is_4led_supported --- %d \n",_server_working_set->shelves_data_manager->is_4led_supported);

	if (comport == acs_apbm::COM2)
	{
		_server_working_set->cs_reader->get_my_slot(slot);
	}
	else if (comport == acs_apbm::COM3)
	{
		_server_working_set->cs_reader->get_my_gea_slot(slot);
	}
	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
	uint16_t apboard_fbn;
	if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, slot, magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
		// return ret_val;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO,"FBN Value in IPMI Operation is : %d", apboard_fbn);

	if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
	{
		if(_server_working_set->shelves_data_manager->is_new_gea_supported)
		{
			//			ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE.... SETTING YELLOW LED TO FASTBLINK ! slot == %d, magazine = 0x%08X ", slot, magazine);
			//			if(_server_working_set->snmp_manager->set_board_MIAled(magazine,slot,axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK) < 0)
			//				ACS_APBM_LOG(LOG_LEVEL_INFO,"Error setting MIA led to FAST BLINK during ipmi upgrade");
			//        	        else
			_server_working_set->shelves_data_manager->update_ap_board_mia_led(axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK,true,slot,magazine);
		}
	}
	else
	{
		if(_server_working_set->shelves_data_manager->is_4led_supported)
		{
			//                	ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE.... SETTING YELLOW LED TO FASTBLINK ! slot == %d, magazine = 0x%08X ", slot, magazine);
			//	               if(_server_working_set->snmp_manager->set_board_MIAled(magazine,slot,axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK) < 0)
			//				ACS_APBM_LOG(LOG_LEVEL_INFO,"Error setting MIA led to FAST BLINK during ipmi upgrade");
			//			else
			_server_working_set->shelves_data_manager->update_ap_board_mia_led(axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK,true,slot,magazine);
		}
	}
	int16_t imm_error_code = 0;
	int op_res = _server_working_set->shelves_data_manager->request_ipmi_upgrade_on_ap_board(imm_error_code, firmware_image_path, slot, magazine, comport);

	if(op_res == acs_apbm::ERR_NO_ERRORS)
	{
#if 0
		// Test code : activate a thread to simulate action execution
		acs_apbm_action_thread * p_test_thread = new acs_apbm_action_thread(_server_working_set, magazine, slot);
		p_test_thread->activate();
#endif
		// instantiate a timer to wait for action completion

		if (comport == acs_apbm::NO_COM)
		{
			acs_apbm_action_timer * p_action_timer = acs_apbm_action_timer::make_instance(acs_apbm_action_timer::IPMIUPGRADE_TIMER_DELAY, acs_apbm_action_timer::IPMIUPGRADE_TIMER_INTERVAL, slot, magazine,IMM_ACTION_APBOARD_IPMIUPGRADE_SNMP , 1, _server_working_set);

			if(p_action_timer)
			{
				// activate timer
				if(p_action_timer->activate() < 0)
				{
					delete p_action_timer;

				}
				char object_dn[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
				if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT){
					if(build_DN_from_magazine_and_slot(object_dn, slot, magazine) < 0)
						return -1;
				}else {
					::snprintf(object_dn, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d", slot);
				}

				// update the map of pending action timers
				_pending_action_timers_mutex.acquire();
				_pending_action_timers.insert(PendingActionTimersMap::value_type(object_dn, p_action_timer));
				_pending_action_timers_mutex.release();
			}
			else
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Failure instantiating action timer. No memory available !");
			}
		}

	}
	else if ((op_res == acs_apbm::ERR_IPMIUPG_SNMP_ERROR) || (op_res == acs_apbm::ERR_GENERIC_ERROR)) // IF IPMI UPG is NOT OK
	{
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
		{
			if(_server_working_set->shelves_data_manager->is_new_gea_supported)
			{
				ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE FAILED.... SETTING YELLOW LED TO ON ! slot == %d, magazine = 0x%08X ", slot, magazine);
				if(_server_working_set->snmp_manager->set_board_MIAled(magazine,slot,axe_eth_shelf::LED_STATUS_ON) < 0)
					ACS_APBM_LOG(LOG_LEVEL_INFO,"Error setting MIA led to ON after ipmi upgrade failure");
				else
					_server_working_set->shelves_data_manager->update_ap_board_mia_led(axe_eth_shelf::LED_STATUS_ON,true,slot,magazine);
			}
			else
			{ // Code below to solve the RED LED ON issue after IPMI UPG for 3LED HW in case of IPMI FAILS

				ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE FAILED!.... SETTING RED LED TO OFF ! slot == %d, magazine = 0x%08X ", slot, magazine);
				if(_server_working_set->snmp_manager->set_board_REDled(magazine,slot,axe_eth_shelf::LED_STATUS_OFF) < 0)
				{
					ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "OaM board is not be able to communicate with the IPMI processor!");
					int callresult=_server_working_set->alarmevent_handler->raise_no_communication_event();
					if(callresult != 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "Failed to raise the event for no communication.");
					}
				}
				else
					_server_working_set->shelves_data_manager->update_ap_board_red_led(axe_eth_shelf::LED_STATUS_OFF,true,slot,magazine);
			}
		}
		else
		{
			if(_server_working_set->shelves_data_manager->is_4led_supported)
			{
				ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE FAILED.... SETTING YELLOW LED TO ON ! slot == %d, magazine = 0x%08X ", slot, magazine);
				if(_server_working_set->snmp_manager->set_board_MIAled(magazine,slot,axe_eth_shelf::LED_STATUS_ON) < 0)
					ACS_APBM_LOG(LOG_LEVEL_INFO,"Error setting MIA led to ON after ipmi upgrade failure");
				else
					_server_working_set->shelves_data_manager->update_ap_board_mia_led(axe_eth_shelf::LED_STATUS_ON,true,slot,magazine);
			}
			else
			{ // Code below to solve the RED LED ON issue after IPMI UPG for 3LED HW in case of IPMI FAILS
				ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE FAILED!.... SETTING RED LED TO OFF ! slot == %d, magazine = 0x%08X ", slot, magazine);
				if(_server_working_set->snmp_manager->set_board_REDled(magazine,slot,axe_eth_shelf::LED_STATUS_OFF) < 0)
				{
					ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "OaM board is not be able to communicate with the IPMI processor!");
					int callresult=_server_working_set->alarmevent_handler->raise_no_communication_event();
					if(callresult != 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "Failed to raise the event for no communication.");
					}
				}
				else
					_server_working_set->shelves_data_manager->update_ap_board_red_led(axe_eth_shelf::LED_STATUS_OFF,true,slot,magazine);					}	
		}
	}
	return op_res;
}


int __CLASS_NAME__::notify_asynch_action_completion(int32_t slot, uint32_t magazine)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Received action completion notification ! slot == %d, magazine = 0x%08X ", slot, magazine);

	// get DN of the object on which the action has been completed
	char object_dn[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

	if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT){
		if(build_DN_from_magazine_and_slot(object_dn, slot, magazine) < 0)
			return -1;
	}else {
		::snprintf(object_dn, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d", slot);
	}

	// retrieve timer associated to the action
	ACE_Guard<ACE_Recursive_Thread_Mutex>_guard(_pending_action_timers_mutex);

	PendingActionTimersMap::iterator it = _pending_action_timers.find(object_dn);
	if(it == _pending_action_timers.end())
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve pending action timer for DN '%s': such timer should exist !", object_dn);
		return -2;
	}


	acs_apbm_action_timer *p_action_timer = (*it).second;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Found pending action timer for DN '%s': action_id == %llu !", object_dn, p_action_timer->operation_id());

	// get action results, and use them to update IMM and internal data structures
	int check_res = p_action_timer->check_action();
	if(check_res != acs_apbm_action_timer::CHECK_ACTION_COMPLETED)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot deactivate action timer ! Call 'acs_apbm_action_timer::check_action(..) has returned  %d",  check_res);
		return -3;
	}

	// deactivate timer and destroy it
	if(p_action_timer->deactivate() < 0)
		return -4;

	// remove the timer from the map of pending action timers
	// fix for TR HS44150
	//_pending_action_timers.erase(it);

	return 0;
}


int __CLASS_NAME__::clear_asynch_action_timer(int32_t slot, uint32_t magazine)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Removing from IMM repository pending action timer (slot == %d, magazine = 0x%08X) ...", slot, magazine);

	// get DN of the object on which the action has been completed
	char object_dn[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT){
		if(build_DN_from_magazine_and_slot(object_dn, slot, magazine) < 0)
			return -1;
	}else {
		::snprintf(object_dn, ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d", slot);
	}

	// retrieve timer associated to the action
	ACE_Guard<ACE_Recursive_Thread_Mutex>_guard(_pending_action_timers_mutex);

	PendingActionTimersMap::iterator it = _pending_action_timers.find(object_dn);
	if(it == _pending_action_timers.end())
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve pending action timer for DN '%s' !", object_dn);
		return -2;
	}

	// remove the timer from the map of pending action timers
	_pending_action_timers.erase(it);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Successfully removed from IMM repository pending action timer for DN '%s' !", object_dn);
	return 0;
}


//UPDATE CALLBACKs
//#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
ACS_CC_ReturnType __CLASS_NAME__::updateRuntime (const char * p_objName, const char ** p_attrName) {
	return update_callback_impl(p_objName, p_attrName);
}
//#else
ACS_CC_ReturnType __CLASS_NAME__::updateCallback(const char * obj_name, const char ** attr_name) {
	return update_callback_impl(obj_name, attr_name);
}
//#endif

ACS_CC_ReturnType __CLASS_NAME__::update_callback_impl (const char * obj_name, const char ** attr_name)
{
	// parse DN to get MAGAZINE string (in IP format : XXX.XXX.XXX.XXX) and SLOT number
	char mag_str[16] = {0};
	int32_t apBladeHardwareInfoId, slot;
	char tmp_str[512] = {0};
	int call_result = -1;
	bool board_missing_alarm_flag = false; 		//HX62204
	int sscanf_res = sscanf(obj_name, ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID"=%d,"ACS_APBM_CONFIG_AP_BLADE_CLASS_ID"=%d,"ACS_APBM_CONFIG_MAGAZINE_CLASS_ID"=%s", & apBladeHardwareInfoId, & slot, tmp_str);
	if(sscanf_res != 3)
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Error parsing DN using 'sscanf(..)' ! call_result == %d.", sscanf_res);
	if(char * p = index(tmp_str, ','))
	{
		size_t n_bytes_to_copy = p - tmp_str;
		if(n_bytes_to_copy > sizeof(mag_str) - 1)
		{
			ACS_APBM_LOG(LOG_LEVEL_WARN, "MAGAZINE string is too long (len == %ul). It will be trunked to %d characters ! ", n_bytes_to_copy, sizeof(mag_str) - 1);
			n_bytes_to_copy = sizeof(mag_str) - 1;
		}
		memcpy(mag_str, tmp_str, n_bytes_to_copy);
	}
	else
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid DN: cannot retrieve MAGAZINE string. Details: comma character not found ! ");


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Parsed values : apBladeInfoId == %d, apBladeId (slot) == %d, apShelfId == %s", apBladeHardwareInfoId, slot, mag_str);

	/*
	 * TO BE DELETED
	 *
	// this code is temporary ! We ignore parsed MAGAZINE string and assume a fixed MAGAZINE string: we do so because in IMM the magazine string is NOT yet in IP format
	if(strncmp(mag_str, ACS_APBM_DEFAULT_MAGAZINE_STR, sizeof(mag_str)) != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "<ShelfId> is '%s', but we ignore such value and assume that <ShelfId> == '%s' ", mag_str, ACS_APBM_DEFAULT_MAGAZINE_STR);
		strncpy(mag_str, ACS_APBM_DEFAULT_MAGAZINE_STR, sizeof(mag_str));
		mag_str[sizeof(mag_str)-1] = 0;
	}
	 */
	uint32_t magazine;
	call_result = acs_apbm_csreader::ip_format_to_uint32(magazine, mag_str);
	if(call_result != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR,"Error in ip_format_to_uint32 conversion '%s'",mag_str);
		return ACS_CC_FAILURE;
	}

	int env = 0;
	call_result = _server_working_set->cs_reader->get_environment(env);	
	if(call_result != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error in getting Shelf Environment");
		return ACS_CC_FAILURE;
	}
	if (env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		return ACS_CC_SUCCESS;
	}
	
	unsigned timeout = 500;
	bool update_led = false; //to solve problem of leds attributes cached
	//bool update_hw_data = (env == ACS_CS_API_CommonBasedArchitecture::DMX)? false: true;

	acs_apbm_apboardinfo board;


	/*
	 * Here there are some problems on the new model
	 * These lines should not be here; they should be called in other place, maybe into an operation,
	 * or also into the monitoring process.
	 *
	 */
	int imm_call_result = 0;


	//update board status - to be sure to show valid board status
	_server_working_set->shelves_data_manager->get_ap_board_fbn_and_status(board.fbn,board.board_status, slot, magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Board in slot %d has fbn: %d and status: '%d'", slot, board.fbn, board.board_status);


	for(int i=0; attr_name[i]; ++i) {
		ACS_APBM_LOG(LOG_LEVEL_INFO,"OK. Found attribute '%s' in position '%d'.", attr_name[i], i);

		//BIOS VERSION
		if((strcmp(attr_name[i], bios_version_attr_name)) == 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_INFO, "UPDATE BIOS VERSION: on slot %d", slot);
			if(( call_result = _server_working_set->shelves_data_manager->update_ap_board_bios_version(true, slot)) < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error updating bios version. Call 'update_ap_board_bios_version'(..) failed! - result %d",call_result);
		}

		// TODO - FATAL LOG EVENT INFO
		else if ((strcmp(attr_name[i], fatal_event_log_info_name)) == 0)
		{
			char fatal_event_log[acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1] = {0};
			unsigned timeout = 500;
			if ((call_result = _server_working_set->snmp_manager->get_board_fatal_event_log(fatal_event_log, slot, magazine, &timeout) ) < 0 )
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to get board fatal event log status. Call 'get_board_fatal_event_log'(..) failed! - result %d",call_result);
				::strncpy(fatal_event_log,"N/A",ACS_APBM_ARRAY_SIZE(fatal_event_log));
				//return ACS_CC_FAILURE;
			}

			//TODO - get data from snmpmanager
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE FATAL EVENT LOG: fatal_event_log_info_name == %s ", fatal_event_log);
			if((call_result = _server_working_set->shelves_data_manager->update_ap_board_fatal_event_log(fatal_event_log, true, slot, magazine)) < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to update fatal_event_log_info_name in IMM. Call 'fatal_event_log_info_name(...)' failed! - result %d",call_result);
				//return ACS_CC_FAILURE;
			}
		}
		// BIOS BOOT RUN MODE INFO
		else if(strcmp(attr_name[i], bios_running_mode_attr_name) == 0)
		{

			unsigned timeout = 500;
			if ( (call_result = _server_working_set->snmp_manager->get_board_bios_run_mode(board.bios_running_mode, slot, &timeout) < 0 ))
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to get bios_running_mode from snmp. Call 'get_board_bios_boot_run_mode(...)' failed! - result %d",call_result);
				//return ACS_CC_FAILURE;
			}

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE BIOS BOOR RUN MODE: bios_running_mode == %d", board.bios_running_mode);
			if((call_result = _server_working_set->shelves_data_manager->update_ap_board_bios_boot_run_mode(board.bios_running_mode, true, slot, magazine)) < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to update bios_running_mode in IMM. Call 'update_ap_board_bios_boot_run_mode(...)' failed! - result %d",call_result);
				//return ACS_CC_FAILURE;
			}

			update_led = true; //to solve problem of leds attributes cached

		}
	}


	// In BSP get_hw_install_status is getting used
	if (env == ACS_CS_API_CommonBasedArchitecture::DMX)
	{
		if(board.board_status == axe_eth_shelf::BOARD_STATUS_WORKING ||
				board.board_status == axe_eth_shelf::BOARD_STATUS_MISSING ||
				board.board_status == axe_eth_shelf::BOARD_STATUS_UNDEFINED )
		{
			board.slot_position = slot;
			board.magazine = magazine;
			axe_eth_shelf::hw_installation_status_t status;
			//On result of this snmp get, it will be decided the logic state of board
			call_result = _server_working_set->snmp_manager->get_hw_install_status(status,
					board.slot_position,
					board.magazine, &timeout);

			if (call_result == acs_apbm::ERR_NO_ERRORS)
			{
				if (status == axe_eth_shelf::NOT_INSTALLED)
				{
					if (board.board_status == axe_eth_shelf::BOARD_STATUS_WORKING ||
							board.board_status == axe_eth_shelf::BOARD_STATUS_UNDEFINED) //UPDATE STATUS TO MISSING
					{
						update_led = false;
						board.board_status = axe_eth_shelf::BOARD_STATUS_MISSING;
						imm_call_result = _server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
						ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Update Board to 'MISSING' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
						//reset LEDs
						if(board.fbn == ACS_CS_API_HWC_NS::FBN_GEA)
							_server_working_set->shelves_data_manager->update_ap_board_default_gea_led_values(slot, magazine);
						else
							_server_working_set->shelves_data_manager->update_ap_board_default_led_values(slot, magazine);
						//reset SNMP info
						_server_working_set->shelves_data_manager->reset_ap_board_info(true, slot);
						_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
					}
				}
				else if (status == axe_eth_shelf::INSTALLED_AND_ACCEPTABLE)
				{
					if (board.board_status == axe_eth_shelf::BOARD_STATUS_MISSING ||
							board.board_status == axe_eth_shelf::BOARD_STATUS_UNDEFINED) //UPDATE STATUS TO WORKING
					{
						update_led = false;
						board.board_status = axe_eth_shelf::BOARD_STATUS_WORKING;
						_server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
						ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Board board to 'WORKING' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
						_server_working_set->shelves_data_manager->update_ap_board_info(true, slot);
						_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
					}
				}

			}
			else
			{
				update_led = false;
				board.board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
				_server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG," Impossible to get board status...it will be set to 'UNDEFINED' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
				//reset LEDs
				if(board.fbn == ACS_CS_API_HWC_NS::FBN_GEA)
					_server_working_set->shelves_data_manager->update_ap_board_default_gea_led_values(slot, magazine);
				else
					_server_working_set->shelves_data_manager->update_ap_board_default_led_values(slot, magazine);
				//reset SNMP info
				_server_working_set->shelves_data_manager->reset_ap_board_info(true, slot);
				_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
			}


		}
	}
	else //Not BSP
	{

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
					update_led = false;
					board.board_status = axe_eth_shelf::BOARD_STATUS_MISSING;
					imm_call_result = _server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
					ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Raise BOARD MISSING alarm & Update Board to 'MISSING' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
					//reset LEDs
					if(board.fbn == ACS_CS_API_HWC_NS::FBN_GEA)
						_server_working_set->shelves_data_manager->update_ap_board_default_gea_led_values(slot, magazine);
					else
						_server_working_set->shelves_data_manager->update_ap_board_default_led_values(slot, magazine);
					//reset SNMP info
					_server_working_set->shelves_data_manager->reset_ap_board_info(true, slot);
					_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
					board_missing_alarm_flag = true; 	//HX62204
				}
			}
			else if (call_result == acs_apbm_snmp::ERR_SNMP_NO_ERRORS && //get return 0
					(board.board_status == axe_eth_shelf::BOARD_STATUS_MISSING ||
							board.board_status == axe_eth_shelf::BOARD_STATUS_UNDEFINED)) //UPDATE STATUS TO WORKING
			{
				update_led = false;
				board.board_status = axe_eth_shelf::BOARD_STATUS_WORKING;
				_server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Cease BOARD MISSING alarm & Board board to 'WORKING' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
				_server_working_set->shelves_data_manager->update_ap_board_info(true, slot);
				_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
			}
			else if (call_result != acs_apbm_snmp::ERR_SNMP_NO_ERRORS)//impossible to retrive status...it will be set to undefined
			{
				update_led = false;
				board.board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
				_server_working_set->shelves_data_manager->set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Cease BOARD MISSING alarm & Impossible to get board status...it will be set to 'UNDEFINED' - get_board_data result %d - set_board_status %d", call_result, imm_call_result);
				//reset LEDs
				if(board.fbn == ACS_CS_API_HWC_NS::FBN_GEA)
					_server_working_set->shelves_data_manager->update_ap_board_default_gea_led_values(slot, magazine);
				else
					_server_working_set->shelves_data_manager->update_ap_board_default_led_values(slot, magazine);
				//reset SNMP info
				_server_working_set->shelves_data_manager->reset_ap_board_info(true, slot);
				_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
			}
		}
	}

	if (update_led) { //Other state are controlled by other operation
		//TODO - It will be introduced some different check
		//RED LED - MIA LED - GREEN LED
		// 4 led support for GEP4

		//Check whether we need to read four or three leds
		bool has_4_leds = (board.fbn == ACS_CS_API_HWC_NS::FBN_GEA)? _server_working_set->shelves_data_manager->is_new_gea_supported: _server_working_set->shelves_data_manager->is_4led_supported;

		if(has_4_leds)
		{

			board.red_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;//static_cast<axe_eth_shelf::led_status_t>(2);
			board.mia_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;//static_cast<axe_eth_shelf::led_status_t>(2);
			board.green_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;//static_cast<axe_eth_shelf::led_status_t>(2);
			board.blue_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;//static_cast<axe_eth_shelf::led_status_t>(2);

			if ((call_result = _server_working_set->snmp_manager->get_board_leds_status(board.green_led, board.red_led, board.mia_led, board.blue_led, slot, magazine, &timeout)) < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error getting board leds status. Call 'get_board_leds_status'(..) failed! - result %d",call_result);
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "UPDATE LED STATUS: green_led == %d - red_led == %d - mia_led == %d - blue_led == %d", board.green_led, board.red_led, board.mia_led, board.blue_led);

				call_result = _server_working_set->shelves_data_manager->update_ap_board_red_led(board.red_led, true, slot, magazine);
				if(call_result < 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error updating attribute red led. Call 'update_ap_board_red_led'(..) failed! - result %d", call_result);
				call_result = _server_working_set->shelves_data_manager->update_ap_board_mia_led(board.mia_led, true, slot, magazine);
				if(call_result < 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to update attribute mia led. Call 'update_ap_board_mia_led'(..) failed! - result %d", call_result);
				call_result = _server_working_set->shelves_data_manager->update_ap_board_green_led(board.green_led, true, slot, magazine);
				if(call_result < 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to update attribute green led. Call 'update_ap_board_green_led'(..) failed! - result %d", call_result);
				call_result = _server_working_set->shelves_data_manager->update_ap_board_blue_led(board.blue_led, true, slot, magazine);
				if(call_result < 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to update attribute blue led. Call 'update_ap_board_blue_led'(..) failed! - result %d", call_result);
			}
		}
		else
		{
			board.red_led = static_cast<axe_eth_shelf::led_status_t>(2); //axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
			board.mia_led = static_cast<axe_eth_shelf::led_status_t>(2); //axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
			board.green_led = static_cast<axe_eth_shelf::led_status_t>(2); //axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;

			if ((call_result = _server_working_set->snmp_manager->get_board_leds_status(board.green_led, board.red_led, board.mia_led, slot, magazine, &timeout)) < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error getting board leds status. Call 'get_board_leds_status'(..) failed! - result %d",call_result);
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "UPDATE LED STATUS: green_led == %d - red_led == %d - mia_led == %d", board.green_led, board.red_led, board.mia_led);
				call_result = _server_working_set->shelves_data_manager->update_ap_board_red_led(board.red_led, true, slot, magazine);
				if(call_result < 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error updating attribute red led. Call 'update_ap_board_red_led'(..) failed! - result %d", call_result);
				call_result = _server_working_set->shelves_data_manager->update_ap_board_mia_led(board.mia_led, true, slot, magazine);
				if(call_result < 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to update attribute mia led. Call 'update_ap_board_mia_led'(..) failed! - result %d", call_result);

				call_result = _server_working_set->shelves_data_manager->update_ap_board_green_led(board.green_led, true, slot, magazine);
				if(call_result < 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error to update attribute green led. Call 'update_ap_board_green_led'(..) failed! - result %d", call_result);
			}
		}
	}


	// Scheduling an update for the board status as BOARD MISSING alarm is raised during 'show' of ApBladeInfo
	if(board_missing_alarm_flag)  //start of HX62204
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"BOARD MISSING alarm raised - scheduling an update of board status - operation SERVER_WORK_OPERATION_SCHEDULE_UPDATE_BOARD_STATUS for slot == %d, magazine == 0x%08X", slot, magazine);
		call_result = _server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SCHEDULE_UPDATE_BOARD_STATUS, slot, magazine);
		if (call_result) { 			// ERROR: unable to send the operation
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'operation_send' failed: cannot order SERVER_WORK_OPERATION_SCHEDULE_UPDATE_BOARD_STATUS operation "
					"on the board: slot == %d, magazine == 0x%08X: call_result == %d", slot, magazine, call_result);
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_WARN,"Operation SERVER_WORK_OPERATION_SCHEDULE_UPDATE_BOARD_STATUS for slot == %d, magazine == 0x%08X scheduled successfully", slot, magazine);
	}								 //end of HX62204


	return ACS_CC_SUCCESS;
}

#endif

/* Configuration object callbacks */
#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
ACS_CC_ReturnType __CLASS_NAME__::create (
		ACS_APGCC_OiHandle /*oiHandle*/,
		ACS_APGCC_CcbId /*ccbId*/,
		const char * /*className*/,
		const char * /*parentname*/,
		ACS_APGCC_AttrValues ** /*attr*/
) {
	if (_imm_operation_ongoing_counter <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"IMM CREATE OPERATION CALLED BACK: An IMM CREATE management operation requested but APBM is not calling it: refusing the create");
		return ACS_CC_FAILURE;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"IMM CREATE OPERATION CALLED BACK: APBM is calling it: accepting the create");

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType __CLASS_NAME__::deleted (
		ACS_APGCC_OiHandle /*oiHandle*/,
		ACS_APGCC_CcbId /*ccbId*/,
		const char * /*objName*/) {
/*
	if (_imm_operation_ongoing_counter <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"IMM DELETE OPERATION CALLED BACK: An IMM DELETE management operation requested but APBM is not calling it: refusing the delete");
		return ACS_CC_FAILURE;
	}
*/

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"IMM DELETE OPERATION CALLED BACK: APBM is calling it: accepting the delete");

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType __CLASS_NAME__::modify (
		ACS_APGCC_OiHandle /*oiHandle*/,
		ACS_APGCC_CcbId /*ccbId*/,
		const char * /*objName*/,
		ACS_APGCC_AttrModification ** /*attrMods*/
) {
	if (_imm_operation_ongoing_counter <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"IMM MODIFY OPERATION CALLED BACK: An IMM MODIFY management operation requested but APBM is not calling it: refusing the modify");
		return ACS_CC_FAILURE;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"IMM MODIFY OPERATION CALLED BACK: APBM is calling it: accepting the modify");

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType __CLASS_NAME__::complete(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/) {
/*
	if (_imm_operation_ongoing_counter <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"IMM COMPLETE OPERATION CALLED BACK: An IMM COMPLETE management operation requested but APBM is not calling it: refusing the complete");
		return ACS_CC_FAILURE;
	}
*/

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"IMM COMPLETE OPERATION CALLED BACK: APBM is calling it: accepting the complete");

	return ACS_CC_SUCCESS;
}

void __CLASS_NAME__::abort(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/) {
	if (_imm_operation_ongoing_counter <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"IMM ABORT OPERATION CALLED BACK: An IMM ABORT management operation requested but APBM is not calling it: refusing the abort");
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"IMM ABORT OPERATION CALLED BACK: APBM is calling it: accepting the abort");
}

void __CLASS_NAME__::apply(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/) {
/*
	if (_imm_operation_ongoing_counter <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"IMM APPLY OPERATION CALLED BACK: An IMM APPLY management operation requested but APBM is not calling it: refusing the apply");
	}
*/

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"IMM APPLY OPERATION CALLED BACK: APBM is calling it: accepting the apply");
}
#endif
