/** @file acs_apbm_snmptrapmessagehandler.cpp

 *      @brief
 *      @author xnicmut (Nicola Muto)
 *      @date 2012-02-15
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
 *      | R-001 | 2012-02-15 | xnicmut      | Creation and first revision.        |
 *      +=======+============+==============+=====================================+
 *      |       | 2014-06-17 | xsunach      | TR HS54933                          |
 *      +=======+============+==============+=====================================+
 */

#include <vector>
#include "acs_apbm_logger.h"
#include "acs_apbm_snmpconstants.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_serverworkingset.h"

#include "acs_apbm_operationpipescheduler.h"

#include "acs_apbm_snmptrapmessagehandler.h"

std::vector< std::pair<std::string, uint32_t> >  scx_cold_warm_trap_ip_list;		// TR HX50455
ACE_Thread_Mutex theLoadMgrMutex;

namespace {
	oid oid_trap_scb_ShelfMgrCtrl []			= { ACS_APBM_SCB_OID_TRAP_XSHMC_SHELF_MGR_CTRL };
	oid oid_trap_scb_BoardPresence []			= { ACS_APBM_SCB_OID_TRAP_XSHMC_BOARD_PRESENCE };
	oid oid_trap_scb_SensorStateChange []	= { ACS_APBM_SCB_OID_TRAP_XSHMC_SENSOR_STATE_CHANGE };
	oid oid_trap_scb_SELEntry []					= { ACS_APBM_SCB_OID_TRAP_XSHMC_SEL_ENTRY };

	oid oid_trap_scx_ShelfMgrCtrl []			= { ACS_APBM_SCX_OID_TRAP_SHELF_MGR_CTRL };
	oid oid_trap_scx_BoardPresence []			= { ACS_APBM_SCX_OID_TRAP_BLADE_PRESENCE };
	oid oid_trap_scx_SensorStateChange []	= { ACS_APBM_SCX_OID_TRAP_SENSOR_STATE_CHANGE };
	oid oid_trap_scx_IpmiUpgradeResult []	= { ACS_APBM_SCX_OID_TRAP_IPMI_UPGRADE_RESULT };
	oid oid_trap_scx_SELEntry []					= { ACS_APBM_SCX_OID_TRAP_SEL_ENTRY };
	oid oid_trap_scx_ColdStart []				= { ACS_APBM_SCX_OID_TRAP_COLD_START };
	oid oid_trap_scx_WarmStart []				= { ACS_APBM_SCX_OID_TRAP_WARM_START };


        oid oid_trap_bsp_BladePresence []                       = { ACS_APBM_BSP_OID_TRAP_BLADE_PRESENCE };
        oid oid_trap_bsp_SensorStateChange []                   = { ACS_APBM_BSP_OID_TRAP_SENSOR_STATE_CHANGE };
        //oid oid_trap_bsp_temperatureChange []                   = { ACS_APBM_BSP_OID_TRAP_SENSOR_STATE_CHANGE };

	oid oid_scb_magPlugNumber []			= { ACS_APBM_SCB_OID_XSHMC_MAG_PLUG_NUMBER };
	oid oid_scb_shelf_mgr_SlotPos []	= { ACS_APBM_SCB_OID_XSHMC_SCBRP_SLOT_POS };
	oid oid_scb_slotPos []						= { ACS_APBM_SCB_OID_XSHMC_SLOT_POS };
	oid oid_scb_hw_board_presence []	= { ACS_APBM_SCB_OID_XSHMC_HW_BOARD_PRESENCE };
	oid oid_scb_bus_type []						= { ACS_APBM_SCB_OID_XSHMC_BUS_TYPE };
	oid oid_scb_sensor_type []				= { ACS_APBM_SCB_OID_XSHMC_SENSOR_TYPE };
	oid oid_scb_sensor_id []					= { ACS_APBM_SCB_OID_XSHMC_SENSOR_ID };
	oid oid_scb_sensor_type_code []		= { ACS_APBM_SCB_OID_XSHMC_SENSOR_TYPE_CODE };
	oid oid_scb_sensor_event_data []	= { ACS_APBM_SCB_OID_XSHMC_SENSOR_EVENT_DATA };
	oid oid_scb_shelfMgrState []			= { ACS_APBM_SCB_OID_XSHMC_SHELF_MGR_STATE };
	oid oid_scb_neighbour []					= { ACS_APBM_SCB_OID_XSHMC_NEIGHBOUR };
	oid oid_scb_selEntry []						= { ACS_APBM_SCB_OID_XSHMC_SEL_ENTRY };


	oid oid_scx_magPlugNumber []							= { ACS_APBM_SCX_OID_SHELF_NUM };
	oid oid_scx_shelf_mgr_SlotPos []					= { ACS_APBM_SCX_OID_SHMC_SLOT_POS };
	oid oid_scx_slotPos []										= { ACS_APBM_SCX_OID_SLOT_POS };
	oid oid_scx_shelfMgrState []							= { ACS_APBM_SCX_OID_SHELF_MGR_STATE };
	oid oid_scx_neighbour []									= { ACS_APBM_SCX_OID_NEIGHBOUR };
	oid oid_scx_hw_board_presence []					= { ACS_APBM_SCX_OID_HW_BLADE_PRESENCE };
	oid oid_scx_bus_type []										= { ACS_APBM_SCX_OID_BUS_TYPE };
	oid oid_scx_sensor_type []								= { ACS_APBM_SCX_OID_SENSOR_TYPE };
	oid oid_scx_sensor_id []									= { ACS_APBM_SCX_OID_SENSOR_ID };
	oid oid_scx_sensor_type_code []						= { ACS_APBM_SCX_OID_SENSOR_TYPE_CODE };
	oid oid_scx_sensor_event_data []					= { ACS_APBM_SCX_OID_SENSOR_EVENT_DATA };
	oid oid_scx_ipmi_upgrade_result_value []	= { ACS_APBM_SCX_OID_IPMI_UPGRADE_RESULT_VALUE };
	oid oid_scx_selEntry []							= { ACS_APBM_SCX_OID_SEL_ENTRY };

        oid oid_bsp_shelfId []                                                  = { ACS_APBM_BSP_OID_SHELF_ID }; 
        oid oid_bsp_slotPos []                                                  = { ACS_APBM_BSP_OID_SLOT_POS };
        oid oid_bsp_blade_admnState []                                          = { ACS_APBM_BSP_OID_BLADE_ADMNSTATE };
        oid oid_bsp_blade_presenceState []                                      = { ACS_APBM_BSP_OID_BLADE_PRESENCESTATE };

        oid oid_bsp_sensor_type []                                              = { ACS_APBM_BSP_OID_SENSOR_TYPE };
        oid oid_bsp_sensor_id []                                                = { ACS_APBM_BSP_OID_SENSOR_ID };
        oid oid_bsp_sensor_type_code []                                         = { ACS_APBM_BSP_OID_SENSOR_TYPE_CODE };
        oid oid_bsp_sensor_event_data []                                        = { ACS_APBM_BSP_OID_SENSOR_EVENT_DATA };
        oid oid_bsp_notification_id []                                          = { ACS_APBM_BSP_OID_NOTIFICATION_ID };
        oid oid_bsp_temperature_event_data []                                   = { ACS_APBM_BSP_OID_TEMPERATURE_EVENT_DATA };
}


int __CLASS_NAME__::handle_trap (ACS_TRAPDS_StructVariable & pdu) {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "A new trap received from trap handle_trap service");
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trap PDU message handler: callback called: a new trap received from trap dispatcher service");

	trapds_var_list_t & var_list = pdu.nvls;
	trapds_var_list_t::iterator var_it = var_list.begin();
	trapds_var_list_t::iterator var_list_end = var_list.end();

	// Check: PDU is empty?
	if (var_it++ == var_list_end) { // ERROR: PDU is empty
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "The received trap PDU message doesn't keep any trap data: an enpty PDU trap message received");
		return acs_apbm_snmp::ERR_SNMP_PDU_NO_VARIABLE_BOUND;
	}

	// Note: At previous if the iterator was forwarded by one position because the trap
	// oid received is at position two.

	const char * source_trap_ip = var_it->ipTrap.c_str();

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP source trap IP == '%s'", source_trap_ip);

	// Check the trap oid received if any.
	if (var_it == var_list_end) { // ERROR: no trap oid in PDU
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No trap oid into the second position of the PDU");
		return acs_apbm_snmp::ERR_SNMP_NOT_AXE_TRAP;
	}

	// Check that the var structure is a trap message
	oid snmp_trap_oid [] = { ACS_APBM_SNMP_TRAP_OID };

	if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, snmp_trap_oid, ACS_APBM_ARRAY_SIZE(snmp_trap_oid))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in the received pdu: the pdu doesn't contain a snmpTrap message");
		return acs_apbm_snmp::ERR_SNMP_NOT_PDU_TRAP;
	}
	// Check the type: it need to be an object id
	if (var_it->type != ASN_OBJECT_ID) { // ERROR: not an object id
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No trap OID is specified in the received pdu");
		return acs_apbm_snmp::ERR_SNMP_NOT_PDU_TRAP;
	}

	char buffer [2 * 1024] = {0};

	oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->val.objid.oids, var_it->val.objid.length);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Received the following trap oid: %s", buffer);

        int env =3;
        
        int call_result=0;

        if ((call_result =(_server_working_set->cs_reader->get_environment(env))) != 0) {
            return 1;
         }
           

	int is_cba = _server_working_set->cs_reader->is_cba_system();
	oid * oid_to_compare = 0;
	size_t oid_to_compare_size = 0;
        
	if (env == 2)
	{
		oid_to_compare = oid_trap_bsp_BladePresence;
		oid_to_compare_size = ACS_APBM_ARRAY_SIZE(oid_trap_bsp_BladePresence);
		if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_to_compare, oid_to_compare_size) == 0)
			return handle_BSPBoardPresenceTrap(++var_it, var_list_end);

		oid_to_compare = oid_trap_bsp_SensorStateChange;
		oid_to_compare_size = ACS_APBM_ARRAY_SIZE(oid_trap_bsp_SensorStateChange);
		if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_to_compare, oid_to_compare_size) == 0)
			return handle_BSPTemperatureChangeTrap(++var_it, var_list_end);
	//		return handle_BSPSensorStateChangeTrap(++var_it, var_list_end);
	}
	else
	{

		oid_to_compare = is_cba ? oid_trap_scx_ShelfMgrCtrl : oid_trap_scb_ShelfMgrCtrl;
		oid_to_compare_size = is_cba ? ACS_APBM_ARRAY_SIZE(oid_trap_scx_ShelfMgrCtrl) : ACS_APBM_ARRAY_SIZE(oid_trap_scb_ShelfMgrCtrl);
		if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_to_compare, oid_to_compare_size) == 0)
			return handle_xshmcShelfMgrCtrl(++var_it, var_list_end, source_trap_ip);

		oid_to_compare = is_cba ? oid_trap_scx_BoardPresence : oid_trap_scb_BoardPresence;
		oid_to_compare_size = is_cba ? ACS_APBM_ARRAY_SIZE(oid_trap_scx_BoardPresence) : ACS_APBM_ARRAY_SIZE(oid_trap_scb_BoardPresence);
		if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_to_compare, oid_to_compare_size) == 0)
			return handle_BoardPresenceTrap(++var_it, var_list_end,source_trap_ip);

		oid_to_compare = is_cba ? oid_trap_scx_SensorStateChange : oid_trap_scb_SensorStateChange;
		oid_to_compare_size = is_cba ? ACS_APBM_ARRAY_SIZE(oid_trap_scx_SensorStateChange) : ACS_APBM_ARRAY_SIZE(oid_trap_scb_SensorStateChange);
		if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_to_compare, oid_to_compare_size) == 0)
			return handle_SensorStateChangeTrap(++var_it, var_list_end);

		oid_to_compare = is_cba ? oid_trap_scx_SELEntry : oid_trap_scb_SELEntry;
		oid_to_compare_size = is_cba ? ACS_APBM_ARRAY_SIZE(oid_trap_scx_SELEntry) : ACS_APBM_ARRAY_SIZE(oid_trap_scb_SELEntry);
		if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_to_compare, oid_to_compare_size) == 0)
			return handle_SELEntryTrap(++var_it, var_list_end);
	}

	//Only SCX switch boards can send the IPMI upgrade result trap
	if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_trap_scx_IpmiUpgradeResult, ACS_APBM_ARRAY_SIZE(oid_trap_scx_IpmiUpgradeResult)) == 0)
	{
                ACS_APBM_LOG(LOG_LEVEL_INFO, "A call to handle_IpmiUpgradeResultTrap is called ip '%s'", source_trap_ip);
		return handle_IpmiUpgradeResultTrap(++var_it, var_list_end);
	}

	//Only SCX switch boards can send COLD START trap
	if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_trap_scx_ColdStart, ACS_APBM_ARRAY_SIZE(oid_trap_scx_ColdStart)) == 0)
	{
                ACS_APBM_LOG(LOG_LEVEL_INFO, "A call to handle_ColdStartTrap is called with ip '%s'",source_trap_ip);
		// In SMX Coldtrap is not applicable for Transport Switch
		if(env==4)
		{
			if((_server_working_set->snmp_manager->isEgem2L2Switch(source_trap_ip)))
			{
				return handle_ColdStartTrap(++var_it, var_list_end, source_trap_ip);
			}
			else
			{
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Skipping Cold trap from TransportSwitch '%s'",source_trap_ip);
			}
		}
		else{
			return handle_ColdStartTrap(++var_it, var_list_end, source_trap_ip);
		}
	}

	//Only SCX switch boards can send WARM START trap
	if (acs_apbm_snmpmanager::oid_compare(var_it->val.objid.oids, var_it->val.objid.length, oid_trap_scx_WarmStart, ACS_APBM_ARRAY_SIZE(oid_trap_scx_WarmStart)) == 0)
	{
                ACS_APBM_LOG(LOG_LEVEL_INFO, "A call to handle_WarmStartTrap is called with ip '%s'",source_trap_ip);
		return handle_WarmStartTrap(++var_it, var_list_end, source_trap_ip);
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Unknown trap within the received pdu");
	return acs_apbm_snmp::ERR_SNMP_UNKNOWN_TRAP;
}

int __CLASS_NAME__::handle_xshmcShelfMgrCtrl(trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end, const char * switch_board_ip) const {
        ACS_APBM_LOG(LOG_LEVEL_INFO, "handle_xshmcShelfMgrCtrl Entering ");
	long fields [4] = {-1, -1, -1, -1}; //magPlugNumber,  switch board SlotPos, shelfMgrState, neighbour
	oid * oid_names [4] = {0};
	size_t oid_name_sizes [4] = {0};
	int is_cba_system = _server_working_set->cs_reader->is_cba_system();

	if (is_cba_system) {
		oid_names[0] = oid_scx_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scx_magPlugNumber);
		oid_names[1] = oid_scx_shelf_mgr_SlotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scx_shelf_mgr_SlotPos);
		oid_names[2] = oid_scx_shelfMgrState; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scx_shelfMgrState);
		oid_names[3] = oid_scx_neighbour; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_scx_neighbour);
	} else {
		oid_names[0] = oid_scb_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scb_magPlugNumber);
		oid_names[1] = oid_scb_shelf_mgr_SlotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scb_shelf_mgr_SlotPos);
		oid_names[2] = oid_scb_shelfMgrState; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scb_shelfMgrState);
		oid_names[3] = oid_scb_neighbour; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_scb_neighbour);
	}

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0; i < ACS_APBM_ARRAY_SIZE(oid_names); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0) {
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Unexpected variable in pdu trap received");
			//return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}

	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, "
			"Switch Board Slot Position == %ld, "
			"Shelf Manager Controller state== %ld, "
			"Neighbour Shelf Manager Controller status  == %ld",
			fields[0], fields[1], fields[2], fields[3]);

	return (is_cba_system) ? _server_working_set->snmp_manager->manage_ShelfMgrCtrl_scx( fields[1], fields[2], fields[3], switch_board_ip):
			_server_working_set->snmp_manager->manage_ShelfMgrCtrl_scbrp(fields[0], fields[1], fields[2], fields[3], switch_board_ip);
}


int __CLASS_NAME__::handle_BoardPresenceTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end, const char * switch_board_ip) const {
                ACS_APBM_LOG(LOG_LEVEL_INFO, "Inside the BoardPresenseTrap");
	int fields [4] = {-1, -1, -1, -1}; //Shelf Number, Board Slot, HW Board Presence, Board Bus Type
	oid * oid_names [4] = {0};
	size_t oid_name_sizes [4] = {0};

	if (_server_working_set->cs_reader->is_cba_system()) {
		oid_names[0] = oid_scx_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scx_magPlugNumber);
		oid_names[1] = oid_scx_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scx_slotPos);
		oid_names[2] = oid_scx_hw_board_presence; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scx_hw_board_presence);
		oid_names[3] = oid_scx_bus_type; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_scx_bus_type);
	} else {
		oid_names[0] = oid_scb_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scb_magPlugNumber);
		oid_names[1] = oid_scb_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scb_slotPos);
		oid_names[2] = oid_scb_hw_board_presence; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scb_hw_board_presence);
		oid_names[3] = oid_scb_bus_type; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_scb_bus_type);
	}

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0; i < ACS_APBM_ARRAY_SIZE(oid_names); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0) {
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Unexpected variable in pdu trap received");
			//return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, "
			"Slot Position == %ld, "
			"HW Board Presence == %ld, "
			"Bus Type == %ld",
			fields[0], fields[1], fields[2], fields[3]);

	return _server_working_set->snmp_manager->manage_BoardPresence(fields[0], fields[1], fields[2], fields[3], switch_board_ip);
}

int __CLASS_NAME__::handle_BSPBoardPresenceTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const {
	int fields [4] = {-1, -1, -1, -1}; 

	//Shelf Number, Board Slot, Blade Administrative State, Blade Operative State,Blade PresenceState
	oid * oid_names [4] = {0};
	size_t oid_name_sizes [4] = {0};
	const char *switch_board_ip = {0};


	oid_names[0] = oid_bsp_shelfId; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_bsp_shelfId);
	oid_names[1] = oid_bsp_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_bsp_slotPos);
	oid_names[2] = oid_bsp_blade_admnState; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_bsp_blade_admnState);
	oid_names[3] = oid_bsp_blade_presenceState; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_bsp_blade_presenceState);

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0; i < ACS_APBM_ARRAY_SIZE(oid_names); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length , oid_names[i], oid_name_sizes[i]) == 0) {
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Unexpected variable in pdu trap received");
			//return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, "
			"Slot Position == %ld, "
			"Blade Administartive State == %ld, "
			"Blade Presence State   == %ld",
			fields[0], fields[1], fields[2], fields[3]);

	return _server_working_set->snmp_manager->manage_BoardPresence(fields[0], fields[1], fields[2], fields[3],switch_board_ip);
}

int __CLASS_NAME__::handle_SensorStateChangeTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const {
        ACS_APBM_LOG(LOG_LEVEL_INFO, "handle_SensorStateChangeTrap Entering ");
	int fields [5] = {-1, -1, -1, -1, -1}; //Shelf Number, Board Slot, sensorType, sensorID, sensorTypeCode
	char sensor_data [acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1] = {0};
	oid * oid_names [6] = {0};
	size_t oid_name_sizes [6] = {0};

	if (_server_working_set->cs_reader->is_cba_system()) {
		oid_names[0] = oid_scx_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scx_magPlugNumber);
		oid_names[1] = oid_scx_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scx_slotPos);
		oid_names[2] = oid_scx_sensor_type; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scx_sensor_type);
		oid_names[3] = oid_scx_sensor_id; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_scx_sensor_id);
		oid_names[4] = oid_scx_sensor_type_code; oid_name_sizes[4] = ACS_APBM_ARRAY_SIZE(oid_scx_sensor_type_code);
		oid_names[5] = oid_scx_sensor_event_data; oid_name_sizes[5] = ACS_APBM_ARRAY_SIZE(oid_scx_sensor_event_data);
	} else {
		oid_names[0] = oid_scb_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scb_magPlugNumber);
		oid_names[1] = oid_scb_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scb_slotPos);
		oid_names[2] = oid_scb_sensor_type; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scb_sensor_type);
		oid_names[3] = oid_scb_sensor_id; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_scb_sensor_id);
		oid_names[4] = oid_scb_sensor_type_code; oid_name_sizes[4] = ACS_APBM_ARRAY_SIZE(oid_scb_sensor_type_code);
		oid_names[5] = oid_scb_sensor_event_data; oid_name_sizes[5] = ACS_APBM_ARRAY_SIZE(oid_scb_sensor_event_data);
	}

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0 ; i < (ACS_APBM_ARRAY_SIZE(oid_names) - 1); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0) {
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if ((i == (ACS_APBM_ARRAY_SIZE(oid_names) - 1)) && (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0)) {
			if (var_it->type == ASN_OCTET_STR) ::memcpy(sensor_data, var_it->val.string.c_str(),
					var_it->val_len < static_cast<size_t>(acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE)
						? var_it->val_len
						: static_cast<size_t>(acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE));
			else {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected a string");
				return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unexpected variable in trap pdu received");
			return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, "
			"Board Slot Position == %ld, "
			"Sensor Type == %ld, "
			"Sensor ID == %ld, "
			"Sensor Type Code == %ld",
			fields[0], fields[1], fields[2], fields[3], fields[4]);

	return _server_working_set->snmp_manager->manage_SensorStateChange(fields[0], fields[1], fields[2], fields[3], fields[4], sensor_data);
}

int __CLASS_NAME__::handle_BSPSensorStateChangeTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const {
	int fields [5] = {-1, -1, -1, -1, -1}; //Shelf Number, Board Slot, sensorEventdata
	char sensor_data [acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1] = {0};
	oid * oid_names [6] = {0};
	size_t oid_name_sizes [6] = {0};

	oid_names[0] = oid_bsp_shelfId; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_bsp_shelfId);
	oid_names[1] = oid_bsp_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_bsp_slotPos);
	oid_names[2] = oid_bsp_sensor_type; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_bsp_sensor_type);
	oid_names[3] = oid_bsp_sensor_id; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_bsp_sensor_id);
	oid_names[4] = oid_bsp_sensor_type_code; oid_name_sizes[4] = ACS_APBM_ARRAY_SIZE(oid_bsp_sensor_type_code);
	oid_names[5] = oid_bsp_sensor_event_data; oid_name_sizes[5] = ACS_APBM_ARRAY_SIZE(oid_bsp_sensor_event_data);

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0; i < (ACS_APBM_ARRAY_SIZE(oid_names) - 1); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0) {
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if ((i == (ACS_APBM_ARRAY_SIZE(oid_names) - 1)) && (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0)) {
			if (var_it->type == ASN_OCTET_STR) ::memcpy(sensor_data, var_it->val.string.c_str(),
					var_it->val_len < static_cast<size_t>(acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE)
						? var_it->val_len
						: static_cast<size_t>(acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE));
			else {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected a string");
				return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unexpected variable in trap pdu received");
			return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, "
			"Board Slot Position == %ld, ",
			fields[0], fields[1]);

	return _server_working_set->snmp_manager->manage_SensorStateChange(fields[0], fields[1], fields[2],fields[3],fields[4],sensor_data);
}

int __CLASS_NAME__::handle_BSPTemperatureChangeTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const {
	int fields [4] = {-1, -1, -1, -1}; //Shelf Number, Board Slot, sensorEventDataValues,notificationId
	char sensor_data [acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1] = {0};
	oid * oid_names [4] = {0};
	size_t oid_name_sizes [4] = {0};

	oid_names[0] = oid_bsp_shelfId; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_bsp_shelfId);
	oid_names[1] = oid_bsp_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_bsp_slotPos);
	oid_names[2] = oid_bsp_temperature_event_data; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_bsp_temperature_event_data);
	oid_names[3] = oid_bsp_notification_id; oid_name_sizes[3] = ACS_APBM_ARRAY_SIZE(oid_bsp_notification_id);

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0; i < ACS_APBM_ARRAY_SIZE(oid_names) ; ++i) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "i == '%d', array size == '%d'", i,ACS_APBM_ARRAY_SIZE(oid_names));
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length , oid_names[i], oid_name_sizes[i]) == 0) {
				
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else if(var_it->type == ASN_OCTET_STR)
				{
					::memcpy(sensor_data, var_it->val.string.c_str(),
					var_it->val_len < static_cast<size_t>(acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE)
                                                ? var_it->val_len
                                                : static_cast<size_t>(acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE));
					break;
				}
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unexpected variable in trap pdu received");
			return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, "
			"Board Slot Position == %ld",
			fields[0], fields[1]);

	return _server_working_set->snmp_manager->manage_TemperatureChange(fields[0], fields[1], fields[3], sensor_data);
}

int __CLASS_NAME__::handle_SELEntryTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const {
        ACS_APBM_LOG(LOG_LEVEL_INFO, "handle_SELEntryTrap Entering ");
	int fields [2] = {-1, -1}; //Shelf Number, Board Slot
	char sel_message [acs_apbm::PCP_FIELD_SIZE_TRAP_MSG + 1] = {0};
	oid * oid_names [3] = {0};
	size_t oid_name_sizes [3] = {0};

	if (_server_working_set->cs_reader->is_cba_system()) {
		oid_names[0] = oid_scx_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scx_magPlugNumber);
		oid_names[1] = oid_scx_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scx_slotPos);
		oid_names[2] = oid_scx_selEntry; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scx_selEntry);
	} else {
		oid_names[0] = oid_scb_magPlugNumber; oid_name_sizes[0] = ACS_APBM_ARRAY_SIZE(oid_scb_magPlugNumber);
		oid_names[1] = oid_scb_slotPos; oid_name_sizes[1] = ACS_APBM_ARRAY_SIZE(oid_scb_slotPos);
		oid_names[2] = oid_scb_selEntry; oid_name_sizes[2] = ACS_APBM_ARRAY_SIZE(oid_scb_selEntry);
	}

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0; i < (ACS_APBM_ARRAY_SIZE(oid_names) - 1); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0) {
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if ((i == (ACS_APBM_ARRAY_SIZE(oid_names) - 1)) && (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0)) {
			if (var_it->type == ASN_OCTET_STR)
			{
				//::memcpy(sel_message, var_it->val.string.c_str(),var_it->val_len < static_cast<size_t>(acs_apbm::PCP_FIELD_SIZE_TRAP_MSG) ? var_it->val_len : static_cast<size_t>(acs_apbm::PCP_FIELD_SIZE_TRAP_MSG));
				// TR HW34181 - handle cases where val.string contains null characters in middle of string (SEL trap)
				int sizeToCopy = var_it->val_len < static_cast<size_t>(acs_apbm::PCP_FIELD_SIZE_TRAP_MSG) ? var_it->val_len : static_cast<size_t>(acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);
				std::copy(var_it->val.string.begin(), (var_it->val.string.begin() + sizeToCopy), sel_message);
			}

			else {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected a string");
				return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unexpected variable in trap pdu received");
			return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, Board Slot Position == %ld", fields[0], fields[1]);

	return _server_working_set->snmp_manager->manage_SELEntry(fields[0], fields[1], sel_message);
}

int __CLASS_NAME__::handle_IpmiUpgradeResultTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const {
         ACS_APBM_LOG(LOG_LEVEL_INFO, "handle_IpmiUpgradeResultTrap Entering ");
	int fields [3] = {-1, -1, -1}; //Shelf Number, Board Slot, IPMI Upgrade Result Value
	oid * oid_names [3] = { oid_scx_magPlugNumber, oid_scx_slotPos, oid_scx_ipmi_upgrade_result_value };
	size_t oid_name_sizes [3] = { ACS_APBM_ARRAY_SIZE(oid_scx_magPlugNumber), ACS_APBM_ARRAY_SIZE(oid_scx_slotPos), ACS_APBM_ARRAY_SIZE(oid_scx_ipmi_upgrade_result_value) };

	for (; var_it != var_list_end; var_it++) {
		char buffer [2 * 1024] = {0};
		oid_to_string(buffer, ACS_APBM_ARRAY_SIZE(buffer), var_it->name.oids, var_it->name.length);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OID variable == '%s'", buffer);

		size_t i = 0;
		for (i = 0; i < ACS_APBM_ARRAY_SIZE(oid_names); ++i) {
			if (acs_apbm_snmpmanager::oid_compare(var_it->name.oids, var_it->name.length - 1, oid_names[i], oid_name_sizes[i]) == 0) {
				if (var_it->type == ASN_INTEGER) { fields[i] = var_it->val.integer; break; }
				else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Bad variable type received: expected an integer");
					return acs_apbm_snmp::ERR_SNMP_BAD_ASN_TYPE;
				}
			}
		}

		if (i >= ACS_APBM_ARRAY_SIZE(oid_names)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unexpected variable in trap pdu received");
			//return acs_apbm_snmp::ERR_SNMP_PDU_UNEXPECTED_VARIABLE;
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Shelf Manager Controller attributes: Address Magazine Number == %ld, "
			"Board Slot Position == %ld, "
			"IPMI Upgrade Result Value == %ld",
			fields[0], fields[1], fields[2]);

	return _server_working_set->snmp_manager->manage_IpmiUpgradeResult(fields[0], fields[1], fields[2]);
}

int __CLASS_NAME__::handle_ColdStartTrap(trapds_var_list_t::iterator & /*var_it*/, trapds_var_list_t::iterator & /*var_list_end*/, const char * switch_board_ip) const {

	int call_result = -1;
	bool skipDuplicateIPNTrap = false;
	uint32_t newTrapTickValue = 0;
	_server_working_set->operation_pipe_scheduler->get_current_tick_counter(newTrapTickValue);		// TR HX50455

	string ipOfNewTrap = std::string(switch_board_ip);
	string lastOctetNewTrapIp = ipOfNewTrap.substr(ipOfNewTrap.find_last_of('.') + 1);		// parse last octet of IP address to later find cold trap entries of same switch board but different IPN

	theLoadMgrMutex.acquire();
	// start of fix for TR HX50455
	if(!scx_cold_warm_trap_ip_list.empty())
	{
		int index = scx_cold_warm_trap_ip_list.size() - 1;
		for(index; index >= 0; index--)
		{
			std::string ipInList = scx_cold_warm_trap_ip_list[index].first;
			std::string lastOctetIpInList = ipInList.substr(ipInList.find_last_of('.') + 1);

			if(lastOctetIpInList == lastOctetNewTrapIp)
			{
				/* An entry for current SCXB is already present in list(maybe IPNs are different) i.e. old cold/warm trap for this board is still to be processed.
				   Compare old trap tick value and new trap tick value to decide if they are duplicates of same instance of switch board restart such as in TR HX50455 case
				   where SCXB was sending 2 traps - one on IPNA and one on IPNB for the same instance of reboot. In such cases, do not add the send IP address of same switch board into the vector*/

				if(scx_cold_warm_trap_ip_list[index].second == newTrapTickValue)
					skipDuplicateIPNTrap = true;

				// Irrespective of whether current SCX has existing trap with same tick value or not, break from traversing through the vector because we are reading the vector from the end
				// i.e if first encounter of cold trap IP for this SCX has different time, then we can be sure the earlier entries will also have different time/tick value as items are stored
				break;
			}
		}
	}

	if(!skipDuplicateIPNTrap)
	{
		scx_cold_warm_trap_ip_list.push_back(make_pair(ipOfNewTrap,newTrapTickValue));
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Adding new cold/warm trap IP %s to vector - tick counter is %u", ipOfNewTrap.c_str(), newTrapTickValue);
	}
	else
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Skipping new cold/warm trap IP %s as it is duplicate received in same tick (%u) on different IPN of same switch board", ipOfNewTrap.c_str(), newTrapTickValue);

	theLoadMgrMutex.release();
	// end of fix for TR HX50455

	if(!skipDuplicateIPNTrap)
		call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SCX_COLD_TRAP_SCHEDULE,30);

	if(call_result < 0){
		if(!skipDuplicateIPNTrap){					// remove last element only if we have added it to vector above
                        //start of TR IA35401
                        call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SCX_COLD_TRAP_SCHEDULE,31);
                        if(call_result < 0){
                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to schedule operation 'SERVER_WORK_OPERATION_SCX_COLD_TRAP_SCHEDULE' after retry. Call 'acs_apbm_operationpipescheduler::schedule_operation()' failed. call_result == %d", call_result);
                        theLoadMgrMutex.acquire();
                        if(!scx_cold_warm_trap_ip_list.empty())                 // TR HX50455
                                scx_cold_warm_trap_ip_list.pop_back();
                        theLoadMgrMutex.release();
                        }
                        else {
                           call_result = acs_apbm::ERR_NO_ERRORS;
                           ACS_APBM_LOG(LOG_LEVEL_ERROR, "scheduled operation 'SERVER_WORK_OPERATION_SCX_COLD_TRAP_SCHEDULE' after retry. Call 'acs_apbm_operationpipescheduler::schedule_operation()'. call_result == %d", call_result);
                              }//end of TR IA35401

		}
	}
	else
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "scheduled operation 'SERVER_WORK_OPERATION_SCX_COLD_TRAP_SCHEDULE'. Call 'acs_apbm_operationpipescheduler::schedule_operation()' success. call_result == %d", call_result);

    return call_result;
}

void __CLASS_NAME__::oid_to_string (char * buf, size_t buf_size, const oid * name, size_t name_size) const {
	char * buf_end = buf + buf_size;

	for (int chars_wrote = 0; (name_size--) && (buf < buf_end); buf += chars_wrote, buf_size -= chars_wrote)
		chars_wrote = ::snprintf(buf, buf_size, ".%lu", *name++);

	buf_end[-1] = 0;
}
