#ifndef HEADER_GUARD_CLASS__acs_apbm_immrepositoryhandler
#define HEADER_GUARD_CLASS__acs_apbm_immrepositoryhandler acs_apbm_immrepositoryhandler

/** @file acs_apbm_immrepositoryhandler.h
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
 *	| R-002 | 2011-10-18 | xgiufer      | Second revision. 					  |
 *	+=======+============+==============+=====================================+
 *	| R-002 | 2015-03-18 | xsunach      | Disk integration changes            |
 *	+=======+============+==============+=====================================+
 */


#include <string>
#include <map>
#include <stdint.h>

#include <ace/Event_Handler.h>
#include <ace/Recursive_Thread_Mutex.h>

#include "acs_apbm_programconfiguration.h"

//#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
#	include "acs_apgcc_oihandler_V3.h"
#	include "acs_apgcc_objectimplementerinterface_V3.h"
//#else
#	include "ACS_APGCC_RuntimeOwner_V2.h"
//#endif

#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_paramhandling.h>

#include "acs_apbm_apboardinfo.h"
#include "acs_apbm_serverworkingset.h"

namespace axe_eth_shelf = acs_apbm_snmp::axe::eth::shelf;
#define ATTRIBUTE_HANDLER OK

#define IMM_DH_ATTR_NAME_MAX_SIZE 128
#define IMM_ACTION_PARAMETER_NAME_MAX_SIZE 128

#define EMPTY_ATTRIBUTE 0
#define SINGLE_ATTRIBUTE 1
#define FIVE_ATTRIBUTE 5
#define SEVEN_ATTRIBUTE 7
#define NINE_ATTRIBUTE 9
#define IMM_OBJECT_NOT_EXIST -12

#define ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX 512
#define ACS_APBM_CONFIG_IMM_RDN_PATH_SIZE_MAX 256
#define ACS_APBM_IMM_ACTION_TEXT_MAX_SIZE 512

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND

#define ACS_ACA_CONFIG_CACHING_ENABLED  "osCachingEnabled"
#define ACS_ACA_RECORD_DATA_MOM_NAME    "AxeDataRecorddataRecordMId=1"

#define ACS_ACA_SET_CACHING_ENABLED     1
#define ACS_ACA_RESET_CACHING_ENABLED   0
#endif


#define ACS_APBM_CONFIG_PREFIX_MOM_NAME "AxeEquipment"
#define ACS_APBM_CONFIG_PREFIX_ACTION_ANSWER "@ComNbi@"

#define ACS_APBM_CONFIG_IMM_HARDWARE_MANAGEMENT_CLASS_ID "hardwareMgmtId=1"
#define ACS_APBM_CONFIG_IMM_EQUIPMENT_CLASS_ID ACS_APBM_CONFIG_PREFIX_MOM_NAME"equipmentMId=1"
#define ACS_APBM_CONFIG_MAGAZINE_CLASS_ID "shelfId"
#define ACS_APBM_CONFIG_AP_BLADE_CLASS_ID "apBladeId"
#define ACS_APBM_CONFIG_AP_BLADE_HARDWARE_INFO_CLASS_ID "apBladeInfoId"
#define ACS_APBM_CONFIG_IMM_ROOT_DN ACS_APBM_CONFIG_IMM_HARDWARE_MANAGEMENT_CLASS_ID","ACS_APBM_CONFIG_IMM_EQUIPMENT_CLASS_ID

#define ACS_APBM_CONFIG_IMM_CLASS_NAME_AP_BLADE_HARDWARE_INFO ACS_APBM_CONFIG_PREFIX_MOM_NAME"ApBladeInfo"
#define ACS_APBM_CONFIG_IMM_CLASS_NAME_PRODUCT_INFO ACS_APBM_CONFIG_PREFIX_MOM_NAME"ProductInfo"
#define ACS_APBM_CONFIG_IMM_CLASS_NAME_NIC_INFO ACS_APBM_CONFIG_PREFIX_MOM_NAME"NicInfo"
#define ACS_APBM_CONFIG_IMM_CLASS_NAME_OPERATIONAL_LED ACS_APBM_CONFIG_PREFIX_MOM_NAME"OperationalLed"
#define ACS_APBM_CONFIG_IMM_CLASS_NAME_MAINTENANCE_LED ACS_APBM_CONFIG_PREFIX_MOM_NAME"MaintenanceLed"
#define ACS_APBM_CONFIG_IMM_CLASS_NAME_FAULT_LED ACS_APBM_CONFIG_PREFIX_MOM_NAME"FaultLed"
#define ACS_APBM_CONFIG_IMM_CLASS_NAME_STATUS_LED ACS_APBM_CONFIG_PREFIX_MOM_NAME"StatusLed"

#define ACS_APBM_CONFIG_IMM_NIC_INFO_ID_VALUE "eth"
#define STRING_ATTRIBUTE ATTR_STRINGT
#define INTEGER_ATTRIBUTE ATTR_INT32T
#define SA_NAME_ATTRIBUTE ATTR_NAMET

class acs_apbm_action_timer;

// IMM Action identifiers
enum IMM_Actions {
	IMM_ACTION_APBOARD_BLOCK = 0,
	IMM_ACTION_APBOARD_DEBLOCK = 1,
	IMM_ACTION_APBOARD_RESET = 2,
	IMM_ACTION_APBOARD_SETWATCHDOG = 3,
	IMM_ACTION_APBOARD_IPMIUPGRADE_LOCAL = 4,
	IMM_ACTION_APBOARD_IPMIUPGRADE_SNMP = 5,
	IMM_ACTION_APBOARD_IPMIUPGRADE_STATUS = 6,
	IMM_ACTION_MIN_ID = IMM_ACTION_APBOARD_BLOCK,
	IMM_ACTION_MAX_ID = IMM_ACTION_APBOARD_IPMIUPGRADE_SNMP
};


enum imm_action_exit_code{

	/*GENERAL*/
	IMM_ACTION_EXIT_CODE_NOT_RECOGNIZED = -99, // Not recognized
	IMM_ACTION_EXIT_CODE_NOT_IMPLEMENTED = -98,
	IMM_ACTION_EXIT_CODE_NO_ERROR = 0,
	IMM_ACTION_EXIT_CODE_ERROR_EXECUTING = 1, //Error when executing ( general fault )
	IMM_ACTION_EXIT_CODE_ERROR_SERVER_NOT_RESPONDING = 9, // The APBM Server not responding
	IMM_ACTION_EXIX_CODE_ERROR_BOARD_NOT_AVAILABLE = 10,//The board is not available at the moment
	IMM_ACTION_EXIT_CODE_ERROR_INVALID_PLATFORM = 30,//The command is not valid for this platform.
	IMM_ACTION_EXIT_CODE_ERROR_OPERATION_ONGOING = 31,//Ongoing Operation

	/*BLOCK*/
	IMM_ACTION_EXIT_CODE_ERROR_BOARD_ALREADY_BLOCKED = 15, //Board already blocked
	IMM_ACTION_EXIT_CODE_ERROR_BLOCKING_LAST_DISK = 28,//Last working DISK can not be blocked
	IMM_ACTION_EXIT_CODE_ERROR_OWN_SLOT_BLOCK = 29, //Cannot block own slot
	IMM_ACTION_EXIT_CODE_ERROR_RAID_STATUS_FAULTY = 34, //RAID status of both disk's are faulty

	/*DEBLOCK*/
	IMM_ACTION_EXIT_CODE_ERROR_BOARD_ALREADY_DEBLOCKED = 16,//Board already deblocked

	/*RESET*/
	IMM_ACTION_EXIT_CODE_ERROR_OWN_SLOT_RESET = 33, //Cannot block own slot
	IMM_ACTION_EXIT_CODE_ERROR_RESET_BLOCKED_BOARD = 32 //Cannot reset blocked board
};

/**************************************************************** *
 * *********************** ACTION RESUL ************************* *
 * ************************************************************** */
#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_apbm_immactionresult

class __CLASS_NAME__ : public acs_apgcc_objectimplementerinterface_V3{

public:

/*GENERAL ACTION*/
static const char *	imm_action_exit_code_no_error;
static const char *	imm_action_exit_code_error_not_recognized;

static const char *	imm_action_exit_code_error_executing;
static const char *	imm_action_exit_code_server_not_responding;
static const char *	imm_action_exit_code_not_available;
static const char *	imm_action_exit_code_invalid_platform;
static const char *	imm_action_exit_code_operation_ongoing;

/*BLOCK ACTION*/
static const char *	imm_action_exit_code_error_board_already_blocked;
static const char *	imm_action_exit_code_error_blocking_last;
static const char *	imm_action_exit_code_error_own_slot_block;
static const char * 	imm_action_exit_code_error_raid_status_faulty;
/* DEBLOCK ACTION*/
static const char *	imm_action_exit_code_error_board_already_deblocked;
/* RESET ACTION*/
static const char *	imm_action_exit_code_error_own_slot_reset;
static const char *	imm_action_exit_code_error_reset_blocked_board;

	/* CONSTRUCTOR*/
	__CLASS_NAME__(){}

	/* DESTRUCTOR*/
	inline virtual ~__CLASS_NAME__ () {}

	static int get_exit_code_text(const ACS_APGCC_AdminOperationIdType operation_id, const int16_t operation_exit_code, char * exit_code_text);

protected:
	/*GENERAL EXIT CODE*/
	static int get_general_action_exit_code_text(const int16_t operation_exit_code,char * exit_code_text);
	/* ACTION CODE*/
	static int get_block_action_exit_code_text(const int16_t operation_exit_code,char * exit_code_text);
	static int get_deblock_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text);
	static int get_reset_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text);
	/*int get_setwatchdog_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text);*/
	/*int get_ipmiupgrade_action_exit_code_text(const int16_t operation_exit_code, char * exit_code_text);*/

};


/**************************************************************** *
 * *********************** IMM HANDLER ************************* *
 * ************************************************************** */
//#ifndef HEADER_GUARD_CLASS__acs_apbm_immrepositoryhandler
//#define HEADER_GUARD_CLASS__acs_apbm_immrepositoryhandler acs_apbm_immrepositoryhandler
#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_apbm_immrepositoryhandler


class __CLASS_NAME__ :
	public ACE_Event_Handler,
//#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	public acs_apgcc_objectimplementerinterface_V3,
//#else
	public ACS_APGCC_RuntimeOwner_V2
//#endif
{
	//==========//
	// Typedefs //
	//==========//
public:

	typedef enum imm_management_state_constants {
		ACS_APBM_IMM_MANAGEMENT_STATE_CLOSED = 0,
		ACS_APBM_IMM_MANAGEMENT_STATE_OPEN = 1
	} imm_management_state_t;

	typedef enum imm_ownership_state_constants {
		ACS_APBM_IMM_OWNERSHIP_STATE_CLOSED = 0,
		ACS_APBM_IMM_OWNERSHIP_STATE_OPEN = 1,
		ACS_APBM_IMM_OWNERSHIP_STATE_EVENT_HANDLED = 2
	} imm_ownership_state_t;


public:

	// IMM action parameter
	struct imm_action_parameter {
		const char * name;
		ACS_CC_AttrValueType imm_type;
	};

	// IMM action signature
	struct imm_action_signature	{
		IMM_Actions id;
		uint8_t n_params;
		const imm_action_parameter * params;   // an array of <n_params> elements
	};

	// IMM Action parameter names
	static const char * IPMIUPGRADE_SNMP_PATH_PARAM_NAME;
	static const char * IPMIUPGRADE_LOCAL_PATH_PARAM_NAME;
	static const char * IPMIUPGRADE_LOCAL_PORT_PARAM_NAME;
	static const char * IPMIUPGADE_DEFAULT_DEVICE;

	// IMM action signatures
	static imm_action_signature imm_actions[IMM_ACTION_MAX_ID + 1];

	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_immrepositoryhandler Default constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	:
//#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	acs_apgcc_objectimplementerinterface_V3(acs_apbm_programconfiguration::imm_implementer_name_manager),
//#else
	ACS_APGCC_RuntimeOwner_V2(),
//#endif
	_om_handler(),
//#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	_oi_handler(),
//#endif
	_attr_list(), _server_working_set(server_working_set),
	_pending_action_timers(), _pending_action_timers_mutex(),
	_imm_management_state(ACS_APBM_IMM_MANAGEMENT_STATE_CLOSED),
	_imm_ownership_state(ACS_APBM_IMM_OWNERSHIP_STATE_CLOSED),
	_imm_operation_ongoing_counter(0), _sync_object() {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_immrepositoryhandler Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}

	//=================//
	// Field Accessors //
	//=================//
public:

#ifdef ATTRIBUTE_HANDLER

	static const char *	ap_blade_hardware_info_id_attr_name;

	static const char *	product_info_attr_name;
	static const char *	product_info_id_attr_name;
	static const char *	product_revision_attr_name;
	static const char *	product_name_att_name;
	static const char *	product_number_attr_name;
	static const char *	serial_number_attr_name;
	static const char *	manufacturing_date_attr_name;
	static const char *	vendor_attr_name;

	//IMM CLASS - nicInfo
	static const char *	nic_info_attr_name;
	static const char *	nic_info_id_attr_name;
	static const char *	nic_name_attr_name;
	static const char *	nic_status_attr_name;
	static const char *	nic_mac_address_attr_name;
	static const char *	nic_ip_address_attr_name;

	//IMM CLASS - OperationalLed, StatusLed, FaultLed, MaintenanceLed
	static const char *	operational_led_attr_name;
	static const char *	fault_led_attr_name;
	static const char *	maintenance_led_attr_name;
	static const char *	status_led_attr_name;
	static const char *	led_id_attr_name;
	static const char *	led_color_attr_name;
	static const char *	led_status_attr_name;
	static const char *	led_supported_attr_name;

	static const char *	ipmi_firmware_type_attr_name;
	static const char *	ipmi_product_number_attr_name;
	static const char *	ipmi_product_revision_name;
	static const char *	ipmi_upgrade_result_attr_name;
	static const char *	ipmi_upgrade_time_attr_name;
	static const char * bios_running_mode_attr_name;

	static const char *	fatal_event_log_info_name;
	static const char *	temperature_status_attr_name;
	static const char *	alarm_interface_status_attr_name;
	static const char *	raid_disk_status_attr_name;
	static const char *	drbd_disk_status_attr_name;
	static const char *	board_status_attr_name;
	static const char *	bios_version_attr_name;

	typedef enum attributeName {

		IMM_ATTRIBUTE_BOARD_BIOS_RUNNING_MODE,
		IMM_ATTRIBUTE_BOARD_FATAL_EVENT_LOG_INFO,
		IMM_ATTRIBUTE_BOARD_TEMPERATURE_STATUS,
		IMM_ATTRIBUTE_BOARD_RAID_DISK_STATUS,
		IMM_ATTRIBUTE_BOARD_DRBD_DISK_STATUS,
		IMM_ATTRIBUTE_BOARD_ALARM_DISK_INTERFACE,
		IMM_ATTRIBUTE_BOARD_STATUS,
		IMM_ATTRIBUTE_BOARD_BIOS_VERSION,
		IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_RESULT,
		IMM_ATTRIBUTE_BOARD_IPMI_UPGRADE_TIME,

		//internal struct attributes
		IMM_ATTRIBUTE_BOARD_NIC_INFO,
		IMM_ATTRIBUTE_BOARD_PRODUCT_INFO,
		IMM_ATTRIBUTE_BOARD_OPERATIONAL_LED,
		IMM_ATTRIBUTE_BOARD_FAULT_LED,
		IMM_ATTRIBUTE_BOARD_MAINTENANCE_LED,
		IMM_ATTRIBUTE_BOARD_STATUS_LED

	} attributes_names_t;


	typedef enum ledAttributeName{

		IMM_LED_ATTRIBUTE_COLOR = 0,
		IMM_LED_ATTRIBUTE_STATUS,
		IMM_LED_ATTRIBUTE_SUPPORTED
	} leds_attributes_name_t;

	typedef enum productAttributeName{

		IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_REVISION 		= 0,
		IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_NAME,
		IMM_PRODUCT_ATTRIBUTE_BOARD_PRODUCT_NUMBER,
		IMM_PRODUCT_ATTRIBUTE_BOARD_SERIAL_NUMBER,
		IMM_PRODUCT_ATTRIBUTE_BOARD_MANUFACTURING_DATE,
		IMM_PRODUCT_ATTRIBUTE_BOARD_VENDOR,
	} products_attributes_name_t;

	typedef enum nicAttributeName{

		IMM_NIC_ATTRIBUTE_NAME = 0,
		IMM_NIC_ATTRIBUTE_STATUS,
		IMM_NIC_ATTRIBUTE_MAC_ADDRESS,
		IMM_NIC_ATTRIBUTE_IP_ADDRESS
	}nics_attributes_name_t;

	typedef enum attributesType {

		IMM_ATTRIBUTE_TYPE_INTEGER = INTEGER_ATTRIBUTE,
		IMM_ATTRIBUTE_TYPE_STRING = STRING_ATTRIBUTE,
		IMM_ATTRIBUTE_TYPE_SA_NAME = SA_NAME_ATTRIBUTE
	} attributes_types_t;

#endif


	//===========//
	// Functions //
	//===========//
public:
// BEGIN: Base class interface implementation //
//#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
	virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);
//#else
	virtual ACS_CC_ReturnType updateCallback (const char * /*obj_name*/, const char ** /*attr_name*/);
//#endif

	virtual void adminOperationCallback(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_InvocationType /*invocation*/, const char* /*p_objName*/, ACS_APGCC_AdminOperationIdType /*operationId*/,  ACS_APGCC_AdminOperationParamType** /*paramList*/);

private:
	ACS_CC_ReturnType update_callback_impl (const char * /*obj_name*/, const char ** /*attr_name*/);
// END: Base class interface implementation //


public:
	int open_management_session ();

	int close_management_session ();

	int take_ownership (std::string implementer_name);

	int release_ownership ();

	const char * blade_configuration_dn () const;

	virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

#ifdef ATTRIBUTE_HANDLER
	int set_ap_board_info(acs_apbm_apboardinfo & board);
	int set_ap_board_info_for_scb(acs_apbm_apboardinfo & board);
	int add_ap_board_info(acs_apbm_apboardinfo & board);
	int add_ap_board_info_for_scb(acs_apbm_apboardinfo & board);

	int get_board_ipmi_upgrade_last_time(char (&ipmifw_lastupgtime)[30],int32_t slot,uint32_t magazine);
	int get_board_ipmi_upgrade_status(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, int32_t slot, uint32_t magazine);

	//UPDATE - extern use
	int update_green_led_status_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_mia_led_status_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_red_led_status_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_blue_led_status_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_bios_boot_run_mode_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_fatal_event_log_info_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_snmp_ap_board_info(acs_apbm_apboardinfo *board);
	int update_led_status_on_ap_board_info( acs_apbm_apboardinfo *board);
	int update_temperature_status_on_ap_board_info( acs_apbm_apboardinfo *board);
	int update_network_interfaces_cards_status_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_network_interfaces_card_info_on_ap_board_info( axe_eth_shelf::nic_name_t nic,
                                                                  acs_apbm_apboardinfo *board,
                                                                  const axe_eth_shelf::nic_status_t nic_status,
                                                                  const char * nic_name,
                                                                  const char * nic_ipv4_address,
                                                                  const char * nic_mac_address);
	int update_raid_status_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_drbd_status_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_alarm_interface_status_on_ap_board_info( acs_apbm_apboardinfo *board);
	int update_bios_version_on_ap_board_info(acs_apbm_apboardinfo *board);

	int update_nic_name_on_ap_board_info( const axe_eth_shelf::nic_name_t nic , acs_apbm_apboardinfo * board, const char * nic_name);
	int update_mac_address_on_ap_board_info( const axe_eth_shelf::nic_name_t nic , acs_apbm_apboardinfo * board, const char * mac_address);
	int update_ip_address_on_ap_board_info( const axe_eth_shelf::nic_name_t nic , acs_apbm_apboardinfo * board, const char * ip_address);
	int update_all_default_nic_name_on_ap_board_info(acs_apbm_apboardinfo * board, const char * default_nic_name = "");
	int update_all_default_mac_address_on_ap_board_info(acs_apbm_apboardinfo * board, const char * default_mac_address = "");
	int update_all_default_ip_address_on_ap_board_info(acs_apbm_apboardinfo * board, const char * default_ip_address = "" );

	int update_default_led_values_on_ap_board_info(acs_apbm_apboardinfo *board, const axe_eth_shelf::led_type_t led_type, axe_eth_shelf::led_color_t color, bool supported);
	int update_ipmi_upgrade_result(acs_apbm_apboardinfo *board);
	int update_ipmi_upgrade_time(acs_apbm_apboardinfo *board);

	int update_ipmi_upgrade_result_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_ipmi_upgrade_time_on_ap_board_info(acs_apbm_apboardinfo *board);
	int update_ipmi_upgrade_result_on_scb(acs_apbm_apboardinfo *board);
	int update_ipmi_upgrade_time_on_scb(acs_apbm_apboardinfo *board);

	//MODIFY - base method
	int modify_ap_board_status_attribute(const acs_apbm_apboardinfo * board);

	int modify_ap_board_dn_attribute(acs_apbm_apboardinfo *board, attributes_names_t attribute);

	int modify_ap_board_led_attribute(int & imm_error_code, const char * imm_dn, leds_attributes_name_t attr_name, const char * value);
	int modify_ap_board_led_attribute(int & imm_error_code, const char * imm_dn, leds_attributes_name_t attr_name, int32_t value );

	int modify_ap_board_product_info_attribute(int & imm_error_code, const char * imm_dn, products_attributes_name_t attr_name, const char * value );
	int modify_ap_board_product_info_attribute(int & imm_error_code, const char * imm_dn, products_attributes_name_t attr_name, int32_t value );

	int modify_ap_board_nic_attribute(int & imm_error_code, const char * imm_dn, nics_attributes_name_t attr_name, const char * value );
	int modify_ap_board_nic_attribute(int & imm_error_code, const char * imm_dn, nics_attributes_name_t attr_name, int32_t value );

	int modify_ap_board_attribute(int & imm_error_code, const char * imm_dn, attributes_names_t attr_name, const char * value );
	int modify_ap_board_attribute(int & imm_error_code, const char * imm_dn, attributes_names_t attr_name, int32_t value );

	int reset_ap_board_info(acs_apbm_apboardinfo * board);
	int remove_ap_board_info(const acs_apbm_apboardinfo & board);
	int remove_ap_board_info(const char * imm_full_path);
	int remove_all_ap_board_info();
#endif

	//======================== //
	// Action handling methods //
	//======================== //
	int handle_block_action(uint32_t slot, int32_t magazine);
	int handle_deblock_action(uint32_t slot, int32_t magazine);
	int handle_reset_action(uint32_t slot, int32_t magazine);
//int handle_ipmiupgrade_action(uint32_t slot, int32_t magazine, const char* p_objName, ACS_APGCC_InvocationType invocation, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType** paramList);
	int handle_ipmiupgrade_operation(const char * firmware_image_path, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED, uint8_t comport = acs_apbm::NO_COM);
        // Notify that an asynch action has been completed
	int notify_asynch_action_completion(int32_t slot, uint32_t magazine);

	// remove an entry from <_pending_action_timers> map
	int clear_asynch_action_timer(int32_t slot, uint32_t magazine);

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	int  update_osCaching (int caching);
#endif

	//===========//
	// Operators //
	//===========//
protected:
	int add_ap_board_product_info(acs_apbm_apboardinfo & board);
	int add_ap_board_nic_info(acs_apbm_apboardinfo & board);
	int add_ap_board_led_info(acs_apbm_apboardinfo & board);

private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);
	int add_ap_board_led_info_by_type(acs_apbm_apboardinfo & board, axe_eth_shelf::led_type_t led);
	int add_ap_board_nic_info_by_name(acs_apbm_apboardinfo & board, axe_eth_shelf::nic_name_t nic);
	int modify_led_attribute(int & imm_error_code, const char * imm_dn, leds_attributes_name_t attr_name, attributes_types_t type, void **attr_value);
	int modify_product_info_attribute(int & imm_error_code, const char * imm_dn, products_attributes_name_t attr_name, attributes_types_t type, void **attr_value);
	int modify_nic_attribute(int & imm_error_code, const char * imm_dn, nics_attributes_name_t attr_name, attributes_types_t type, void **attr_value);
	int modify_attribute(int & imm_internal_last_error, const char * imm_dn, attributes_names_t attr_name, attributes_types_t type, void **value);

	int _check_parameter(int32_t slot, uint32_t magazine);
	int get_magazine_and_slot_from_DN(uint32_t & magazine, int32_t & slot, const char *imm_dn);
	int get_board_ipmi_upgrade_status_on_scb(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, int32_t slot, uint32_t magazine);
	int get_board_ipmi_upgrade_status_on_ap(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, int32_t slot,uint32_t magazine);
	int get_board_ipmi_upgrade_last_time_on_scb(char (&ipmifw_lastupgtime)[30],int32_t slot,uint32_t magazine);
	int get_board_ipmi_upgrade_last_time_on_ap(char (&ipmifw_lastupgtime)[30],int32_t slot,uint32_t magazine);

	int build_DN_from_magazine_and_slot(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot,  uint32_t magazine);
	int build_DN_from_magazine_slot_and_led_type(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot, uint32_t magazine, axe_eth_shelf::led_type_t led);
	int build_DN_from_magazine_slot_and_product_info(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot, uint32_t magazine, const char * product_struct_name);
	int build_DN_from_magazine_slot_and_nic_info(char (& dn)[ACS_APBM_CONFIG_IMM_DN_PATH_SIZE_MAX], int32_t slot, uint32_t magazine, axe_eth_shelf::nic_name_t nic);

	int retrieve_led_info_from_ap_board_by_led_type(axe_eth_shelf::led_color_t & color, axe_eth_shelf::led_status_t & status,
																									bool & supported, const acs_apbm_apboardinfo & board, const axe_eth_shelf::led_type_t led);
	int retrieve_nic_info_from_ap_board_by_nic_name(char * name, size_t name_size, axe_eth_shelf::nic_status_t & status, char * mac_address,
																									size_t mac_size, char * ip_address,size_t ip_size, const acs_apbm_apboardinfo & board, const axe_eth_shelf::nic_name_t nic_name);
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	int  get_osCaching (int & caching);
#endif
	//========//
	// Fields //
	//========//

private:
	OmHandler _om_handler;

//#if defined (ACS_APBM_USE_IMM_CONFIGURATION_OBJECTS)
	acs_apgcc_oihandler_V3 _oi_handler;
//#endif

	vector<ACS_CC_ValuesDefinitionType> _attr_list;
	acs_apbm_serverworkingset * _server_working_set;

	typedef std::map<std::string,acs_apbm_action_timer*> PendingActionTimersMap;
	PendingActionTimersMap  _pending_action_timers;
	ACE_Recursive_Thread_Mutex _pending_action_timers_mutex;

	imm_management_state_t _imm_management_state;
	imm_ownership_state_t _imm_ownership_state;
	int _imm_operation_ongoing_counter;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
};
#endif // HEADER_GUARD_CLASS__acs_apbm_immrepositoryhandler
