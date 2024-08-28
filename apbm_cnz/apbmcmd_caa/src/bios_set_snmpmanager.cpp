/*
 * bios_set_snmpmanager.cpp
 *
 *  Created on: Nov 18, 2011
 *      Author: xassore
 */

#include "bios_set_common.h"
#include "bios_set_snmpmanager.h"


int bios_set_snmpmanager:: initialize(shelfmgr_info ( & switch_boards)[SHELFMGRBOARD_MAXNUM], bool iscba_system){

	snmp_session session_info;
	snmp_session *session_handler;
	char * community;

	BIOSSET_LOG("DEBUG - BIOS_SET_SNMPMANAGER::initialize (isCBA == %d)\n", iscba_system);
	_isCBA = iscba_system;

	snmp_sess_init(&session_info);			/* initialize session */
	session_info.version = SNMP_VERSION_2c;
	if (_isCBA ) community = const_cast<char *>(GEN_SHELF_COMMUNITY) ;
	else 		 community = const_cast<char *>(AHE_ETH_COMMUNITY) ;
	session_info.community = reinterpret_cast<u_char  *> (community);
	session_info.community_len = strlen((const char *)session_info.community);

	oid oid_name_mgr_cmd_scb_0 [] = {BS_SCB_OID_SHELF_MGR_CMD_0};
	oid oid_name_mgr_cmd_scb_25 [] = {BS_SCB_OID_SHELF_MGR_CMD_25};

	oid oid_name_mgr_cmd_scx [] = {BS_SCX_OID_SHELF_MGR_CTRL_STATE};

	bs_oids scb_shelfmgrctr_oids[] = {
				{oid_name_mgr_cmd_scb_0, BS_ARRAY_SIZE(oid_name_mgr_cmd_scb_0)},
				{oid_name_mgr_cmd_scb_25, BS_ARRAY_SIZE(oid_name_mgr_cmd_scb_25)}
				};

	bs_oids scx_shelfmgrctr_oids[] = {
				{oid_name_mgr_cmd_scx, BS_ARRAY_SIZE(oid_name_mgr_cmd_scx)},
				{oid_name_mgr_cmd_scx, BS_ARRAY_SIZE(oid_name_mgr_cmd_scx)}
				};

	bs_oids  *shelmgrctrl_oids;

	if (_isCBA) {
		shelmgrctrl_oids = scx_shelfmgrctr_oids;
	} else {
		shelmgrctrl_oids = scb_shelfmgrctr_oids;
	}

#ifdef BS_DEBUG
	char buf [2 * 1024];
	for (int i= 0; i <2; i++){
		if (!oid_to_str (buf, BS_ARRAY_SIZE(buf), shelmgrctrl_oids[i].Oid, shelmgrctrl_oids[i].OidLen))
			BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: initialize () - switch board oids for scb in slot %d  oid = %s oidlen =%d\n", i, buf, shelmgrctrl_oids[i].OidLen );
		else
			BIOSSET_LOG ("ERROR - bios_set_snmpmanager:: initialize () - error in oid_to_str function %d\n", 0);
	}
#endif

	bool master_found = false;
	for (size_t i = 0; master_found == false && i < SHELFMGRBOARD_MAXNUM; i++) {
		char * switch_board_ipn_str_addresses [BS_CONFIG_IPN_COUNT] = { switch_boards[i].ipna_str, switch_boards[i].ipnb_str };
		int slot_pos = switch_boards[i].slot_position ;

        bool exit_flag = false;
		for (size_t j = 0; exit_flag != true && j < BS_CONFIG_IPN_COUNT; ++j) {
			session_info.peername = switch_board_ipn_str_addresses[j];

			if (!(session_handler = snmp_open(&session_info)))   { // ERROR: opening the communication session towards the switch board
			   /// TODO:get error_code
				continue;
			}
			// Prepare the PDU subscription request APBM will send to the current switch board
			netsnmp_pdu * request_pdu = 0, *response;
			if (!(request_pdu = snmp_pdu_create(SNMP_MSG_GET))) { // ERROR: creating a new PDU structure
				snmp_close(session_handler);
///  TODO:get the error
				continue;
			}
			// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
			request_pdu->variables = 0;

			int idx = (slot_pos == 0)? 0: 1;
#ifdef BS_DEBUG
			if (!oid_to_str (buf, BS_ARRAY_SIZE(buf), shelmgrctrl_oids[idx].Oid, shelmgrctrl_oids[idx].OidLen))
				BIOSSET_LOG("DEBUG - sending request for scb in slot %d  ipaddr == '%s' (oid == %s oidlen == %d)\n", slot_pos, session_info.peername, buf, shelmgrctrl_oids[idx].OidLen );
			else
				BIOSSET_LOG ("ERROR - error in oid_to_str function %d \n",0);
#endif
			snmp_add_null_var(request_pdu, shelmgrctrl_oids[idx].Oid, shelmgrctrl_oids[idx].OidLen);
			int shelf_mgr_state = bios_set_snmp_ns::SHELMGRSTATE_NOT_ACTIVE;
			int status = snmp_synch_response(session_handler, request_pdu, &response);
			if (!read_shelf_mgr_state_result(status, shelmgrctrl_oids[idx].Oid, shelmgrctrl_oids[idx].OidLen, response, shelf_mgr_state)) {
				// sending request was successful
				exit_flag = true;
				if (shelf_mgr_state == bios_set_snmp_ns::SHELMGRSTATE_ACTIVE){
					_shelf_mgr_ipmi_master = slot_pos;
					master_found = true;
					BIOSSET_LOG("DEBUG - switch board with IPMI MASTER STATUS found in slot %d\n", slot_pos);
				}
			}
			snmp_free_pdu(response);
			if (master_found == false) snmp_close(session_handler);
			else _bs_session_handler = session_handler;
		}
	}
	if (master_found == false) {
		BIOSSET_LOG("ERROR - bios_set_snmpmanager:: initialize () - no switch board with IPMI MASTER STATUS found ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_GETTING_IPMI_MASTER);
		return bs_snmpmanager_ns::ERR_SNMP_GETTING_IPMI_MASTER;
	}
	return bs_snmpmanager_ns::ERR_NO_ERROR;
}

int bios_set_snmpmanager::read_shelf_mgr_state_result(int status, oid * oid_name, size_t oid_name_len, struct snmp_pdu *pdu, int & result)
{
	int ret_code = bs_snmpmanager_ns::ERR_NO_ERROR;
	struct variable_list *var_list /*, *vp*/ ;

     switch (status) {
     case STAT_SUCCESS:
#ifdef BS_DEBUG
		 { 	char buffer [2 * 1024] = {0};  //ONLY FOR DEBUG SCOPE
			 for (netsnmp_variable_list * var = pdu->variables; var; var = var->next_variable) {
				 snprint_variable(buffer, BS_ARRAY_SIZE(buffer), var->name, var->name_length, var);
				 BIOSSET_LOG("DEBUG - PDU response received < errstat == %ld  pdu_data == '%s'>\n", pdu->errstat, buffer);
			 }
		 }
#endif
    	 switch(pdu->errstat){
			case SNMP_ERR_NOERROR:
				if( pdu->command == SNMP_MSG_RESPONSE) {
					var_list = pdu->variables;
					if (oid_compare(oid_name, oid_name_len, var_list->name, var_list->name_length) == 0) {
						if (!_isCBA){
							if (var_list->type == ASN_INTEGER){
								// The value in the PDU is '0' or '1' character, so it should be converted in integer values 0 or 1
								int shelf_mgr_state = *var_list->val.integer;
								BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: get_result --- > found shelf_mgr_state = %d\n ", shelf_mgr_state );
								result = (shelf_mgr_state == bs_snmpmanager_ns::SCB_SHELF_MGR_STATE_SCAN)? bios_set_snmp_ns::SHELMGRSTATE_ACTIVE: bios_set_snmp_ns::SHELMGRSTATE_NOT_ACTIVE;
							}
							else {
								BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result --- > ERR_BAD_SNMP_TYPE_VALUE = %d\n ", bs_snmpmanager_ns::ERR_SNMP_PDU_VARIABLE_BAD_TYPE);
								return bs_snmpmanager_ns::ERR_SNMP_PDU_VARIABLE_BAD_TYPE;
							}
						}
						else{
							if (var_list->type == ASN_OCTET_STR){
								// The value in the PDU is '0' or '1' character, so it should be converted in integer values 0 or 1
								if (var_list->val_len && var_list->val.string){
									unsigned shelf_mgr_state = static_cast<unsigned>(*var_list->val.string);
									BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: get_result() - shelf_mgr_state = %u\n ", shelf_mgr_state );
									result = (shelf_mgr_state == bs_snmpmanager_ns::SCX_SHELF_MGR_STATE_ACTIVE)? bios_set_snmp_ns::SHELMGRSTATE_ACTIVE: bios_set_snmp_ns::SHELMGRSTATE_NOT_ACTIVE;
								}
							}
							else {
								BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result --- > ERR_BAD_SNMP_TYPE_VALUE = %d\n ", bs_snmpmanager_ns::ERR_SNMP_PDU_VARIABLE_BAD_TYPE);
								return bs_snmpmanager_ns::ERR_SNMP_PDU_VARIABLE_BAD_TYPE;
							}
						}
					}
					else {
						char buffer [2 * 1024] = {0};
						oid_to_str(buffer, 1024, var_list->name, var_list->name_length) ;
						BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()- OID unexpected! name = %s, name_lenght = %d, type = %u\n", buffer, (int)var_list->name_length, var_list->type);
						return bs_snmpmanager_ns::ERR_SNMP_PDU_OID_UNEXPECTED;
					}
				 }
				 else{
					 BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()- Error in received PDU COMMAND TYPE ret_code == %d\n",bs_snmpmanager_ns::ERR_SNMP_PDU_COMMAND_BAD_TYPE);
					 return bs_snmpmanager_ns::ERR_SNMP_PDU_COMMAND_BAD_TYPE;
				 }
			break;
		    case SNMP_ERR_RESOURCEUNAVAILABLE:
			  BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()-  Error in received PDU response: Resource not available! ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_PDU_OID_UNEXPECTED);
			  return bs_snmpmanager_ns::ERR_SNMP_PDU_RESOURCE_UNAVAIL;
		    default:
			  BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()-  Error in received PDU response: error_code == %d - ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_PDU_GENERIC_ERR);
			  return bs_snmpmanager_ns::ERR_SNMP_PDU_GENERIC_ERR;
		  }
	  break;
	  case STAT_TIMEOUT:
		  ret_code = bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_TIMEOUT;
		  BIOSSET_LOG( "ERROR - bios_set_snmpmanager:: get_result()-  Timeout %d\n", bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_TIMEOUT);
	  break;
      case STAT_ERROR:
	     ret_code = bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_ERROR;
	     BIOSSET_LOG( "ERROR - bios_set_snmpmanager:: get_result()-  STAT_ERROR %d \n", bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_ERROR);
       break;
	}
    return ret_code;
}

int bios_set_snmpmanager:: get_result (int status, oid * oid_name, size_t oid_name_len, struct snmp_pdu *pdu, int & result)
{
  int ret_code = bs_snmpmanager_ns::ERR_NO_ERROR;
  struct variable_list *var_list /*, *vp*/ ;

     switch (status) {
     case STAT_SUCCESS:
#ifdef BS_DEBUG
		{ char buffer [2 * 1024] = {0};  //ONLY FOR DEBUG SCOPE
    	  for (netsnmp_variable_list * var = pdu->variables; var; var = var->next_variable) {
    		snprint_variable(buffer, BS_ARRAY_SIZE(buffer), var->name, var->name_length, var);
    		BIOSSET_LOG("DEBUG - PDU response received < errstat == %ld  pdu_data == '%s'>\n", pdu->errstat, buffer);
    	  }
		}
#endif
    	switch(pdu->errstat){
			case SNMP_ERR_NOERROR:
				if( pdu->command == SNMP_MSG_RESPONSE) {
					var_list = pdu->variables;
					if (oid_compare(oid_name, oid_name_len, var_list->name, var_list->name_length) == 0) {
						if (var_list->type == ASN_INTEGER){
							// The value in the PDU is '0' or '1' character, so it should be converted in integer values 0 or 1
							int  value = *var_list->val.integer;
							BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: get_result()  value = %d\n ", value );
							result = ( (value >= '0')?  value - '0': value);
						}
						else {
							if (var_list->type == SNMP_NOSUCHINSTANCE) {
								BIOSSET_LOG("ERROR - The received PDU variable is not an ASN_INTEGER type variable: var_list->type == %u\n", static_cast<unsigned>(var_list->type));
								return bs_snmpmanager_ns::ERR_SNMP_PDU_RESOURCE_UNAVAIL;
							}
							else{
								BIOSSET_LOG("ERROR - The received PDU variable is not an ASN_INTEGER type variable: var_list->type == %u\n", static_cast<unsigned>(var_list->type));
								return bs_snmpmanager_ns::ERR_SNMP_PDU_VARIABLE_BAD_TYPE;
							}
						}
					}
					else {
						 char buffer [2 * 1024] = {0};
						 oid_to_str(buffer, 1024, var_list->name, var_list->name_length) ;
						 BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()- OID unexpected! name = %s, name_lenght = %d, type = %u\n", buffer, (int)var_list->name_length, var_list->type);
						 return bs_snmpmanager_ns::ERR_SNMP_PDU_OID_UNEXPECTED;
					}
				 }
				 else{
					 BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()- Error in received PDU COMMAND TYPE ret_code == %d\n",bs_snmpmanager_ns::ERR_SNMP_PDU_COMMAND_BAD_TYPE);
					 return bs_snmpmanager_ns::ERR_SNMP_PDU_COMMAND_BAD_TYPE;
				 }
			break;
		    case SNMP_ERR_RESOURCEUNAVAILABLE:
		    case SNMP_ERR_NOCREATION:
			  BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()-  Error in received PDU response: Resource not available! ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_PDU_OID_UNEXPECTED);
			  return bs_snmpmanager_ns::ERR_SNMP_PDU_RESOURCE_UNAVAIL;
		    default:
			  BIOSSET_LOG("ERROR - bios_set_snmpmanager:: get_result()-  Error in received PDU response: <error_code == %ld> - ret_code == %d\n", pdu->errstat, bs_snmpmanager_ns::ERR_SNMP_PDU_GENERIC_ERR);
			  return bs_snmpmanager_ns::ERR_SNMP_PDU_GENERIC_ERR;
		  }
	  break;
	  case STAT_TIMEOUT:
		  ret_code = bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_TIMEOUT;
		  BIOSSET_LOG( "ERROR - bios_set_snmpmanager:: get_result()-  Timeout %d\n", bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_TIMEOUT);
	  break;
      case STAT_ERROR:
	     ret_code = bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_ERROR;
	     BIOSSET_LOG( "ERROR - bios_set_snmpmanager:: get_result()-  STAT_ERROR %d \n", bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_ERROR);
       break;
	}
    return ret_code;
}

int bios_set_snmpmanager::GetBIOSPointer(int board_slot, int32_t & bs_value)
{

	BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: GetBIOSPointer( %d )\n", board_slot);
	 // Arrange the oid names for SCB
	  oid scb_oid_name_bios_pointer [] = { BS_SCB_OID_BOARD_BIOS_POINTER, board_slot };
	  size_t scb_oid_name_bios_pointer_size = BS_ARRAY_SIZE(scb_oid_name_bios_pointer);

#ifdef BS_DEBUG
		char buf [2 * 1024];
		if (!oid_to_str (buf, BS_ARRAY_SIZE(buf), scb_oid_name_bios_pointer, scb_oid_name_bios_pointer_size))
				BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: GetBIOSPointer() bios_pointer oid = %s oid_len == %d \n", buf, scb_oid_name_bios_pointer_size);
		else
				BIOSSET_LOG ("ERROR - bios_set_snmpmanager:: GetBIOSPointer()  error in oid_to_str function %d\n", 0);
#endif
	  // Arrange the oid names for SCX
	  oid scx_oid_name_bios_pointer [] = { BS_SCX_OID_BLADE_BIOS_POINTER, board_slot };
	  size_t scx_oid_name_bios_pointer_size = BS_ARRAY_SIZE(scx_oid_name_bios_pointer);

	  oid * oid_bios_pointer = 0;
	  size_t oid_bp_size = 0;

	  if (_isCBA) {
		 oid_bios_pointer = scx_oid_name_bios_pointer;
		 oid_bp_size = scx_oid_name_bios_pointer_size;
	  } else {
		 oid_bios_pointer = scb_oid_name_bios_pointer;
		 oid_bp_size = scb_oid_name_bios_pointer_size;
	  }

	  if (int const ret_code = sendGetRequest(oid_bios_pointer, oid_bp_size, bs_value)){
				BIOSSET_LOG("ERROR - bios_set_snmpmanager::sendSetRequest() - ERROR IN GET_RESULT ret_code == %d\n", ret_code);
				return ret_code;
	  }

	  BIOSSET_LOG("ERROR bios_set_snmpmanager::GetDefaultBIOSImage() - sending request was successful %d \n\n",bs_snmpmanager_ns::ERR_NO_ERROR);
	  _biosPointer = bs_value;
	  BIOSSET_LOG("bios_default_image_value ==  %d\n", bs_value);

	  return bs_snmpmanager_ns::ERR_NO_ERROR;
}

int bios_set_snmpmanager::GetBIOSDefaultImage(int board_slot, int32_t & bimage_value)
{
	 BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: GetDefaultBIOSImage( %d )\n", board_slot);
	 // Arrange the oid names for SCB
	  oid scb_oid_name_bios_image [] = { BS_SCB_OID_BOARD_BIOS_DEFAULT_IMAGE, board_slot };
	  size_t scb_oid_name_bios_image_size = BS_ARRAY_SIZE(scb_oid_name_bios_image);

#ifdef BS_DEBUG
		char buf [2 * 1024];
		if (!oid_to_str (buf, BS_ARRAY_SIZE(buf), scb_oid_name_bios_image, scb_oid_name_bios_image_size))
				BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: GetDefaultBIOSImage() - bios_pointer oid = %s oid_len == %d \n", buf, scb_oid_name_bios_image_size);
		else
				BIOSSET_LOG ("ERROR - bios_set_snmpmanager:: GetDefaultBIOSImage() - error in oid_to_str function %d\n", 0);
#endif
	  // Arrange the oid names for SCX
	  oid scx_oid_name_bios_image [] = { BS_SCX_OID_BLADE_BIOS_IMAGE_DEFAULT, board_slot };
	  size_t scx_oid_name_bios_image_size = BS_ARRAY_SIZE(scx_oid_name_bios_image);

	  oid * oid_bios_image = 0;
	  size_t oid_bi_size = 0;

	  if (_isCBA) {
		 oid_bios_image = scx_oid_name_bios_image;
		 oid_bi_size = scx_oid_name_bios_image_size;
	  } else {
		  oid_bios_image = scb_oid_name_bios_image;
		  oid_bi_size = scb_oid_name_bios_image_size;
	  }

	  if (int const ret_code = sendGetRequest(oid_bios_image, oid_bi_size, bimage_value)){
			BIOSSET_LOG("ERROR - bios_set_snmpmanager::sendSetRequest() - ERROR IN GET_RESULT ret_code == %d\n", ret_code);
			return ret_code;
	  }

	  BIOSSET_LOG("ERROR bios_set_snmpmanager::GetDefaultBIOSImage() - sending request was successful %d \n\n",bs_snmpmanager_ns::ERR_NO_ERROR);
		_biosDefaultImage = bimage_value;
	  BIOSSET_LOG("bios_default_image_value ==  %d\n", bimage_value);

	  return bs_snmpmanager_ns::ERR_NO_ERROR;
}

int bios_set_snmpmanager::SetBIOSDefaultImage(int board_slot, int bios_image_value)
{
	BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: SetDefaultBIOSImage( slot == %d, bios_image == %d )\n", board_slot, bios_image_value );
	// Arrange the oid names for SCB
	oid scb_oid_name_bios_image [] = { BS_SCB_OID_BOARD_BIOS_DEFAULT_IMAGE, board_slot };
	size_t scb_oid_name_bios_image_size = BS_ARRAY_SIZE(scb_oid_name_bios_image);
	// Arrange the oid names for SCX
	oid scx_oid_name_bios_image [] = { BS_SCX_OID_BLADE_BIOS_IMAGE_DEFAULT, board_slot };
	size_t scx_oid_name_bios_image_size = BS_ARRAY_SIZE(scx_oid_name_bios_image);

	oid * oid_bios_image = 0;
	size_t oid_bi_size = 0;

	if (_isCBA) {
		oid_bios_image = scx_oid_name_bios_image;
		oid_bi_size = scx_oid_name_bios_image_size;
	} else {
		oid_bios_image = scb_oid_name_bios_image;
		oid_bi_size = scb_oid_name_bios_image_size;
	}
	return sendSetRequest(oid_bios_image, oid_bi_size, bios_image_value);
}

int bios_set_snmpmanager::SetBIOSPointer(int board_slot, int bios_pointer_value)
{
	BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: SetBIOSPointer( slot == %d, bios_image == %d )\n", board_slot, bios_pointer_value );

	int snmp_bp_value = bios_pointer_value;
	// Arrange the oid names for SCB
	oid scb_oid_name_board_reset [] = { BS_SCB_OID_BOARD_RESET, board_slot };
	size_t scb_oid_name_size = BS_ARRAY_SIZE(scb_oid_name_board_reset);

	// Arrange the oid names for SCX
	oid scx_oid_name_bios_pointer [] = { BS_SCX_OID_BLADE_BIOS_POINTER, board_slot };
	size_t scx_oid_name_bios_pointer_size = BS_ARRAY_SIZE(scx_oid_name_bios_pointer);

	oid * oid_bios_pointer = 0;
	size_t oid_bp_size = 0;

	if (_isCBA) {
	  oid_bios_pointer = scx_oid_name_bios_pointer;
	 oid_bp_size = scx_oid_name_bios_pointer_size;
	} else {
	  oid_bios_pointer = scb_oid_name_board_reset;
	  oid_bp_size = scb_oid_name_size;
	  snmp_bp_value += 48;
	}

	return sendSetRequest(oid_bios_pointer, oid_bp_size, snmp_bp_value);
}

int bios_set_snmpmanager::sendGetRequest(oid *oid_name, size_t oid_name_len, int &result)
{

	if (!(_bs_session_handler))
	{
		BIOSSET_LOG( "ERROR - bios_set_snmpmanager::GetDefaultBIOSImage() Failed! ERR_OPENING_SNMP_SESSION ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_OPENING_SESSION);
		// ERROR: opening the communication session towards the switch board
		return bs_snmpmanager_ns::ERR_SNMP_OPENING_SESSION;
	}

#ifdef BS_DEBUG
	char buf [2 * 1024] = {0};
	if (!oid_to_str (buf, BS_ARRAY_SIZE(buf), oid_name, oid_name_len))
			BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: sendGetRequest() - bios_pointer oid = %s oid_len == %d \n", buf, oid_name_len);
	else
			BIOSSET_LOG ("ERROR - bios_set_snmpmanager:: sendGetRequest() - error in oid_to_str function %d\n", 0);
#endif

	// Prepare the PDU subscription request APBM will send to the current switch board
	netsnmp_pdu * request_pdu = 0, *response;
	if (!(request_pdu = snmp_pdu_create(SNMP_MSG_GET))) {
		// ERROR: creating a new PDU structure
		BIOSSET_LOG( "ERROR - bios_set_snmpmanager::sendGetRequest() - Failed! ERR_CREATE_PDU ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_CREATE_PDU);
		return bs_snmpmanager_ns::ERR_SNMP_CREATE_PDU;
	}
	// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
	request_pdu->variables = 0;

	snmp_add_null_var(request_pdu, oid_name, oid_name_len);
	int oid_value = -1;
	int status = snmp_synch_response(_bs_session_handler, request_pdu, &response);
	BIOSSET_LOG("DEBUG - bios_set_snmpmanager::sendGetRequest() - sending get request  oid == '%s'\n\n", buf);
	if (int const ret_code = get_result(status, oid_name, oid_name_len, response, oid_value)){
		BIOSSET_LOG("ERROR - bios_set_snmpmanager::sendGetRequest() - ERROR IN GET_RESULT ret_code == %d\n", ret_code);
		return ret_code;
	}
	BIOSSET_LOG("DEBUG - bios_set_snmpmanager::sendGetRequest() - sending request was successful %d \n\n",bs_snmpmanager_ns::ERR_NO_ERROR);
	result = oid_value;
	BIOSSET_LOG("DEBUG - oid_value ==  %d\n", oid_value);
	snmp_free_pdu(response);
	return bs_snmpmanager_ns::ERR_NO_ERROR;
}

int bios_set_snmpmanager::sendSetRequest(oid *oid_name, size_t oid_name_len, int value) {

	if (!(_bs_session_handler))
		{
			BIOSSET_LOG( "ERROR - bios_set_snmpmanager::sendSetRequest() Failed! ERR_OPENING_SNMP_SESSION ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_OPENING_SESSION);
			// ERROR: opening the communication session towards the switch board
			return bs_snmpmanager_ns::ERR_SNMP_OPENING_SESSION;
		}

#ifdef BS_DEBUG
		char buf [2 * 1024] = {0};
		if (!oid_to_str (buf, BS_ARRAY_SIZE(buf), oid_name, oid_name_len))
				BIOSSET_LOG("DEBUG - bios_set_snmpmanager:: sendSetRequest()bios_pointer oid = %s oid_len == %d \n", buf, oid_name_len);
		else
				BIOSSET_LOG ("ERROR - bios_set_snmpmanager:: sendSetRequest()  error in oid_to_str function %d\n", 0);
#endif
		// Prepare the PDU subscription request APBM will send to the current switch board
		netsnmp_pdu * request_pdu = 0, *response;
		if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) {
			// ERROR: creating a new PDU structure
			BIOSSET_LOG( "ERROR - bios_set_snmpmanager::sendSetRequest() Failed! ERR_CREATE_PDU ret_code == %d\n", bs_snmpmanager_ns::ERR_SNMP_CREATE_PDU);
			return bs_snmpmanager_ns::ERR_SNMP_CREATE_PDU;
		}
		// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
		request_pdu->variables = 0;

		long oid_value_ = value;
		// Add the OID name and value into the PDU request
		if (!snmp_pdu_add_variable(request_pdu, oid_name, oid_name_len, ASN_INTEGER, reinterpret_cast<u_char *>(&oid_value_), sizeof(oid_value_))) {
			snmp_free_pdu(request_pdu);
			BIOSSET_LOG("ERROR - Call 'snmp_pdu_add_variable' failed in NET-SNMP library\n", 0);
			return bs_snmpmanager_ns::ERR_SNMP_PDU_ADD_VARIABLE_VALUE;
		}

		int status = snmp_synch_response(_bs_session_handler, request_pdu, &response);
		BIOSSET_LOG("DEBUG bios_set_snmpmanager::sendSetRequest() - sending set request for oid  == '%s'\n\n", buf);

		int request_result;
		if (int const ret_code = get_result(status, oid_name, oid_name_len, response, request_result)){
			BIOSSET_LOG("ERROR - bios_set_snmpmanager::sendSetRequest() - ERROR IN GET_RESULT ret_code == %d\n", ret_code);
			return ((ret_code == bs_snmpmanager_ns::ERR_SNMP_PDU_RESOURCE_UNAVAIL)? bs_snmpmanager_ns::ERR_SNMP_PDU_SET_RESOURCE_UNAVAIL : ret_code);
		}
		BIOSSET_LOG("DEBUG - bios_set_snmpmanager::sendSetRequest() - set request was successful sent %d \n\n",bs_snmpmanager_ns::ERR_NO_ERROR);
		snmp_free_pdu(response);
		return bs_snmpmanager_ns::ERR_NO_ERROR;
}


void bios_set_snmpmanager:: close()
{
	if(_bs_session_handler != NULL){
		snmp_close(_bs_session_handler);
		_bs_session_handler = NULL;
	}
}
////////////  UTILITY FUNCTIONS ////////////////////

int bios_set_snmpmanager:: oid_to_str (char * buf, size_t size, const oid * id, size_t id_length) {
	if (!buf) return -1;

	*buf = 0;

	if (id)
		for (size_t chars_wrote = 0; id_length-- && (size > 1); buf += chars_wrote, size -= chars_wrote)
			if ((chars_wrote = ::snprintf(buf, size, ".%lu", *id++)) >= size) return -1;

	return 0;
}

int bios_set_snmpmanager::oid_compare (const oid * id1, size_t id1_length, const oid * id2, size_t id2_length) {
	if (id1) { if (!id2) return 1;}
	else return id2 ? -1: 0; // id1 is null

	// Here id1 and id2 are both not null.
	size_t length = (id1_length < id2_length ? id1_length : id2_length) + 1;

	while (--length && (*id1++ == *id2++)) ;

	return (length ? ((*(id1 - 1) < *(id2 - 1)) ? -1 : 1) : 0);
}
