#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <set>

#include <ace/Guard_T.h>

#include <ACS_CS_API.h>
#include <ACS_CS_API_Set.h>
#include <ACS_DSD_Server.h>

#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_paramhandling.h>

#include "acs_apbm_logger.h"
#include <acs_apbm_cmdoptionparser.h>

#include "acs_apbm_csreader.h"

int __CLASS_NAME__::_is_cba_system = -1;
int __CLASS_NAME__::_is_multicp_system = -1;
int __CLASS_NAME__::_environment = -1;
int __CLASS_NAME__::_oam_access_type = -1;


int __CLASS_NAME__::bind_to_cs () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (_hwc) return acs_apbm::ERR_NO_ERRORS;

	ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
	if (!hwc) { // ERROR: creating HWC CS instance
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createHWCInstance' failed: cannot create the HWC instance to access CS configuration data");
		return acs_apbm::ERR_CS_CREATE_HWC_INSTANCE;
	}

	_hwc = hwc;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unbind () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (!_hwc) return acs_apbm::ERR_NO_ERRORS;

	ACS_CS_API::deleteHWCInstance(_hwc);

	_hwc = 0;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_ap_node (ACS_DSD_Node & node) {
	if (_my_ap_node.system_id == acs_dsd::SYSTEM_ID_UNKNOWN) {
		ACS_DSD_Server dsd_server;
		ACS_DSD_Node my_ap_node;

		if (dsd_server.get_local_node(my_ap_node)) { // ERROR: getting my AP node by DSD
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'get_local_node' failed: cannot retrieve my AP node info from DSD: DSD last error == %d, DSD laste error text == '%s'",
					dsd_server.last_error(), dsd_server.last_error_text());
			return acs_apbm::ERR_DSD_GET_LOCAL_NODE;
		}

		_my_ap_node = my_ap_node;
	}

	node = _my_ap_node;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_ipn_addresses (char (& ipna_str) [16], char (& ipnb_str) [16]) {
	if (const int call_result = get_my_ap_all_info()) return call_result;

	::strncpy(ipna_str, _my_ipna_str, ACS_APBM_ARRAY_SIZE(ipna_str));
	::strncpy(ipnb_str, _my_ipnb_str, ACS_APBM_ARRAY_SIZE(ipnb_str));

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_bgci_addresses (char (& bgcia_str) [16], char (& bgcib_str) [16]) {
	if (const int call_result = get_my_ap_all_info()) return call_result;


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "BGCI Addresses  = %s - %s", _my_bgcia_str, _my_bgcib_str);

	::strncpy(bgcia_str, _my_bgcia_str, ACS_APBM_ARRAY_SIZE(bgcia_str));
	::strncpy(bgcib_str, _my_bgcib_str, ACS_APBM_ARRAY_SIZE(bgcib_str));

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_magazine (uint32_t & magazine) {
	if (const int call_result = get_my_ap_all_info()) return call_result;

	magazine = _my_magazine;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_magazine (char (& magazine) [16]) {
	if (const int call_result = get_my_ap_all_info()) return call_result;

	::strncpy(magazine, _my_magazine_str, ACS_APBM_ARRAY_SIZE(magazine));

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_system_number (uint16_t &sys_no) {
	if (const int call_result = get_my_ap_all_info()) return call_result;

	sys_no = _my_system_number;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_other_magazines (std::vector <uint32_t> & magazines) {
	std::vector <acs_apbm_switchboardinfo> other_boards;

	if (const int call_result = get_other_switch_boards_info(other_boards)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_other_switch_boards_info' failed: cannot get other magazine address from CS: call_result == %d", call_result);
		return call_result;
	}

	std::set <uint32_t> values;

	for (size_t i = 0; i < other_boards.size(); ++i) values.insert(other_boards[i].magazine);

	magazines.assign(values.begin(), values.end());

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_all_magazines (std::vector <uint32_t> & magazines) {
	uint32_t my_magazine = 0;

	if (const int call_result = get_my_magazine(my_magazine)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_magazine' failed: cannot get my magazine address from CS: call_result == %d", call_result);
		return call_result;
	}

	std::vector <acs_apbm_switchboardinfo> other_boards;

	if (const int call_result = get_other_switch_boards_info(other_boards)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_other_switch_boards_info' failed: cannot get other magazine address from CS: call_result == %d", call_result);
		return call_result;
	}

	std::set <uint32_t> values;
	values.insert(my_magazine);

	for (size_t i = 0; i < other_boards.size(); ++i) values.insert(other_boards[i].magazine);

	magazines.assign(values.begin(), values.end());

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::map_magazine_plug_number (uint32_t & magazine, int mag_plug_number) {
	std::vector <uint32_t> magazines;
	if (const int call_result = get_all_magazines(magazines)) { //ERROR: retrieving magazine addresses
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_all_magazines' failed: cannot get all magazine address from CS: "
				"call_result == %d", call_result);
		return call_result;
	}

	int return_code = acs_apbm::ERR_NO_ERRORS;
	size_t i = 0;

	mag_plug_number &= 0x0F;
	for (i = 0; (i < magazines.size()) && ((magazines[i] & 0x0F) != static_cast<uint32_t>(mag_plug_number)); ++i) ;

	if (i < magazines.size()) magazine = magazines[i];
	else return_code = acs_apbm::ERR_CS_GET_MAGAZINE;

	return return_code;
}

int __CLASS_NAME__::get_my_slot (int32_t & slot) {
	if (const int call_result = get_my_ap_all_info()) return call_result;

	slot = _my_slot_position;

	return acs_apbm::ERR_NO_ERRORS;
}
int __CLASS_NAME__::get_my_gea_slot (int32_t & slot) {

	if (const int call_result = bind_to_cs()) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'bind_to_cs' failed: cannot bind the CS API to read configuration data: call_result == %d", call_result);
                return acs_apbm::ERR_CS_API_BINDING;
        }

        ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
        if (!bs) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
                return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
        }
	uint16_t sysno;
        if (const int call_result = get_my_system_number(sysno)) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_system_number' failed: cannot get my magazine address from CS: call_result == %d", call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
                unbind();
                return call_result;
        }


	bs->reset();
        bs->setFBN(ACS_CS_API_HWC_NS::FBN_GEA);
        bs->setSysType(ACS_CS_API_HWC_NS::SysType_AP);
	bs->setSysNo(sysno);

	ACS_CS_API_IdList board_list;
        ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getBoardIds(board_list, bs);
        if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getBoardIds' failed: cannot get AP board ids from CS: cs_call_result == %d", cs_call_result);
                ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
                unbind();
                return acs_apbm::ERR_CS_GET_BOARD_ID;
        }

	if (board_list.size() <= 0) { // ERROR: no boards found
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "No AP boards found in the CS system configuration table");
                ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
                unbind();
                return acs_apbm::ERR_CS_NO_BOARD_FOUND;
        }
	for (size_t i = 0; i < board_list.size(); ++i) 
	{
                BoardID board_id = board_list[i];
		uint16_t slot_position;	
        	if ((cs_call_result = _hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) 
		{ // ERROR: getting the board slot
        		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSlot' failed: cannot get my partner slot number from CS: cs_call_result == %d", cs_call_result);
	        	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
        		unbind();
	        	return acs_apbm::ERR_CS_GET_SLOT;
        	}
		slot = slot_position;
	}

	ACS_APBM_LOG(LOG_LEVEL_ERROR, "Read Ipmi gea SLOT:  == %d", slot);
	
	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);

        return acs_apbm::ERR_NO_ERRORS;
}


int __CLASS_NAME__::get_my_side(uint16_t & side) {
	if (const int call_result = get_my_ap_all_info()) return call_result;

	side = _my_side;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_partner_slot (int32_t & slot) {
	if (const int call_result = get_my_ap_all_info()) return call_result;

	slot = _my_partner_slot_position;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::is_cba_system () {
	if (_is_cba_system >= 0) return _is_cba_system;

//TEST
	int env;
	int call_result = get_environment(env);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (call_result  == acs_apbm::ERR_NO_ERRORS)
	{
		_is_cba_system = ((env == 1) | (env == 4))? 1: 0;
	} else
	{
		_is_cba_system = 1;
	}
//END TEST

//	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
//	if (_is_cba_system >= 0) return _is_cba_system;
//
//	// CBA architecture not yet retrieved. Retrive it
//
//	if ((_is_cba_system = _server_working_set->cmdoption_parser->cba()) < 0) { // No CBA option (-c) provided. We should query to IMM directly
//		acs_apgcc_paramhandling pha;
//    int node_architecture = 0;
//
//  	OmHandler omManager;
//  	ACS_CC_ReturnType result;
//
//  	/*try initialize imm connection to IMM*/
//  	result = omManager.Init();
//
//  	if (result != ACS_CC_SUCCESS){
//  		ACS_APBM_LOG(LOG_LEVEL_ERROR,
//  							"Call 'OmHandler Init' failed: cannot get the class name!");
//  		_is_cba_system = 1;
//  		return _is_cba_system;
//  	}/*exiting on IMM init failure: generic error*/
//
//  	string class_instance_name;
//  	std::vector<std::string> p_dnList;
//  	result = omManager.getClassInstances("AxeFunctions", p_dnList);
//  	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) {
//  		/*exiting on error either an erroe occurred or more than one instance found */
//  		ACS_APBM_LOG(LOG_LEVEL_ERROR,
//  							"Call 'getClassInstances' failed: cannot get the class name!");
//  		_is_cba_system = 1;
//  	omManager.Finalize();
//  	return _is_cba_system;
//  	}
//  	class_instance_name = p_dnList[0];
//  	omManager.Finalize();
//    const char * ap_node_architecture_attrib_name = "apgShelfArchitecture";
//
//		ACS_CC_ReturnType imm_result = pha.getParameter(class_instance_name.c_str(), ap_node_architecture_attrib_name, &node_architecture);
//		if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting node architecture from ApzFunction IMM object
//			ACS_APBM_LOG(LOG_LEVEL_ERROR,
//					"Call 'getParameter' failed: cannot get the attribute '%s' from IMM: internal last error == %d, internal last error text  == %s",
//					ap_node_architecture_attrib_name, pha.getInternalLastError(), pha.getInternalLastErrorText());
//
//			// Here we assume a CBA architecture as default value
//			_is_cba_system = 1; // return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
//		} else _is_cba_system = (node_architecture ? 1 : 0); // OK: getting the parameter
//
//  	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "class_instance_name=%s - architecture= %d==%s ", class_instance_name.c_str(), _is_cba_system, _is_cba_system ? "CBA": "NOT-CBA" );
//	}


	return _is_cba_system;
}

int __CLASS_NAME__::get_environment(int & environment)
{
	if (_environment >= 0) { environment = _environment; return acs_apbm::ERR_NO_ERRORS; }
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (_environment >= 0) { environment = _environment; return acs_apbm::ERR_NO_ERRORS; }

	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;

	ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture);

	if (cs_call_result != ACS_CS_API_NS::Result_Success) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getNodeArchitecture' failed: cannot get the NodeArchitecture from CS: cs_call_result == %d", cs_call_result);
		return acs_apbm::ERR_CS_ERROR;
	}

	_environment = (environment = nodeArchitecture);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_oam_access_type(int & oam_access_type)
{
        if (_oam_access_type >= 0) { oam_access_type = _oam_access_type; return acs_apbm::ERR_NO_ERRORS; }
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
        if (_oam_access_type >= 0) { oam_access_type = _oam_access_type; return acs_apbm::ERR_NO_ERRORS; }

        /* start connecting to IMM to get info*/
        OmHandler omManager;
        ACS_CC_ReturnType result;
        string class_instance_name;

        /*try initialize imm connection to IMM*/
        result = omManager.Init();
        if (result != ACS_CC_SUCCESS){
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'omManager.Init(..' failed: "
                "cannot get_oam_access_type: result == %d", result);
            return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
        }

        std::vector<std::string> p_dnList;
        result = omManager.getClassInstances("AxeFunctions", p_dnList);
        if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'omManager.getClassInstances(..' failed: "
                "cannot get_oam_access_type: result == %d", result);
	    omManager.Finalize();
            return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
        }

        class_instance_name = p_dnList[0];
        omManager.Finalize();

        const std:: string apg_oam_access_type_attr = "apgOamAccess";
        acs_apgcc_paramhandling pha;
        ACS_CC_ReturnType imm_result;
        imm_result = pha.getParameter(class_instance_name, apg_oam_access_type_attr, &_oam_access_type);
        if (imm_result != ACS_CC_SUCCESS)
        {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'pha.getParameter(..' failed: "
                "cannot get_oam_access_type: result == %d", imm_result);
          return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
        }

        oam_access_type = _oam_access_type;
        ACS_APBM_LOG(LOG_LEVEL_INFO, "Successfully retrieved the APG oam access type: current status: %d (%s)",
            oam_access_type, (oam_access_type == 1) ? "NOCABLE" : (oam_access_type == 0) ? "FRONTCABLE" : "UNDEFINED");

        return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::is_multicp_system (bool & is_multicp_system) {
	if (_is_multicp_system >= 0) { is_multicp_system = (_is_multicp_system ? true : false); return acs_apbm::ERR_NO_ERRORS; }

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (_is_multicp_system >= 0) { is_multicp_system = (_is_multicp_system ? true : false); return acs_apbm::ERR_NO_ERRORS; }

	bool b_res;

	ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NetworkElement::isMultipleCPSystem(b_res);

	if (cs_call_result != ACS_CS_API_NS::Result_Success) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'isMultipleCPSystem' failed: cannot get the IsMultipleCPSystem flag from CS: cs_call_result == %d", cs_call_result);
		return acs_apbm::ERR_CS_ERROR;
	}

	_is_multicp_system = ((is_multicp_system = b_res) ? 1 : 0);
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_switch_board_fbn (uint16_t & fbn) {
	int is_cba = 0;

	if ((is_cba = is_cba_system()) < 0) { // ERROR: querying the system
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'is_cba_system' failed: querying the configuration about the system architecture: call_result == %d", is_cba);
		return is_cba; // The error code is in is_cba variable
	}

	// TODO: In the following line the correct SCX FBC enumeration constant should replace
	// the 340 constants
	fbn = is_cba ? ((_environment == 1)? SWITCH_BOARD_FBN_SCXB : SWITCH_BOARD_FBN_SMXB) : SWITCH_BOARD_FBN_SCBRP;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_my_switch_boards_info (std::vector <acs_apbm_switchboardinfo> & boards) {
	uint32_t my_magazine;

	if (const int call_result = get_my_magazine(my_magazine)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_magazine' failed: cannot get my magazine address from CS: call_result == %d", call_result);
		return call_result;
	}

	uint16_t fbn = 0;

	if (const int call_result = get_switch_board_fbn(fbn)) { // ERROR: getting the switch board FBN
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_switch_board_fbn' failed: cannot get switch board FBN code: call_result == %d", call_result);
		return call_result;
	}

	return get_switch_boards_info(boards, my_magazine, fbn, true);
}

int __CLASS_NAME__::get_other_switch_boards_info (std::vector <acs_apbm_switchboardinfo> & boards) {
	uint32_t my_magazine;

	if (const int call_result = get_my_magazine(my_magazine)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_magazine' failed: cannot get my magazine address from CS: call_result == %d", call_result);
		return call_result;
	}

	uint16_t fbn = 0;

	if (const int call_result = get_switch_board_fbn(fbn)) { // ERROR: getting the switch board FBN
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_switch_board_fbn' failed: cannot get switch board FBN code: call_result == %d", call_result);
		return call_result;
	}

	return get_switch_boards_info(boards, my_magazine, fbn, false);
}

int __CLASS_NAME__::get_apbm_board (int ap_board_name, acs_apbm_csboardinfo & ap_board) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (const int call_result = bind_to_cs()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'bind_to_cs' failed: cannot bind the CS API to read configuration data: call_result == %d", call_result);
		return call_result;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
		return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}

	// search criteria
	bs->reset();

	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getBoardIds' failed: cannot get board ids from CS: cs_call_result == %d", cs_call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_CS_GET_BOARD_ID;
	}

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board_list.size() == %zu", board_list.size());

	// Clear ap_board attributes
	ap_board.slot_position = -1;

	if (board_list.size() <= 0) { // NO switch board found in the system
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No boards found in the CS configuration table");
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_NO_ERRORS;
	}

	// Retrieving board info
	uint16_t ap_board_fbn = ap_boardname_to_fbn (ap_board_name);
	bool board_found = false;
	uint16_t slot_position;
	uint16_t fbn;
	int return_code = acs_apbm::ERR_NO_ERRORS;

	for (size_t i = 0; i < board_list.size(); ++i) {
		BoardID board_id = board_list[i];

		if ((cs_call_result = _hwc->getFBN(fbn, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board fbn
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getFBN' failed: cannot get the fbn type for the current board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
			return_code = acs_apbm::ERR_CS_GET_FBN;
			break;
		}

		// Is fbn == ap_board_fbn
		if (fbn  == ap_board_fbn) { // YES
			if ((cs_call_result = _hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board slot
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSlot' failed: cannot get the slot number for the current board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
				return_code = acs_apbm::ERR_CS_GET_SLOT;
				continue;
			}
			ap_board.slot_position = slot_position;
			ap_board.fbn = fbn;
			board_found = true;
			break;
		}
	}

	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
	unbind();

	if (!board_found && return_code == acs_apbm::ERR_CS_GET_SLOT) return return_code;// ERROR: in CS get functions

	if (ap_board.slot_position == -1) { // ERROR: No APBM boards found in the configuration table
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No  board with fbn == '%d' found in the CS configuration table", ap_board_fbn);
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_apbm_boards (std::vector<acs_apbm_csboardinfo> & boards) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (const int call_result = bind_to_cs()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'bind_to_cs' failed: cannot bind the CS API to read configuration data: call_result == %d", call_result);
		return call_result;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
		return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}

	// search criteria
	bs->reset();

	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getBoardIds' failed: cannot get board ids from CS: cs_call_result == %d", cs_call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_CS_GET_BOARD_ID;
	}

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board_list.size() == %zu", board_list.size());

	// Clear the vector
	boards.clear();

	if (board_list.size() <= 0) { // NO switch board found in the system
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No boards found in the CS configuration table");
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_NO_ERRORS;
	}

	// Retrieving board info
	int undo = 0;
	uint16_t slot_position;
	uint16_t fbn;
	uint32_t magazine;
	acs_apbm_csboardinfo board_info;
	int return_code = acs_apbm::ERR_NO_ERRORS;

	for (size_t i = 0; i < board_list.size(); ++i) {
		BoardID board_id = board_list[i];

		if ((cs_call_result = _hwc->getFBN(fbn, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board fbn
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getFBN' failed: cannot get the fbn type for the current board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
			return_code = acs_apbm::ERR_CS_GET_FBN;
			undo = 1; break;
		}

		// Is an AP board: APUB, GEA, GED, Disk?
		if (is_ap_board_fbn(fbn)) { // YES
			if ((cs_call_result = _hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board slot
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSlot' failed: cannot get the slot number for the current board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
				return_code = acs_apbm::ERR_CS_GET_SLOT;
				undo = 1; break;
			}

			if ((cs_call_result = _hwc->getMagazine(magazine, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting magazine info
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getMagazine' failed: cannot get the magazine number for the current board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
				return_code = acs_apbm::ERR_CS_GET_MAGAZINE;
				undo = 1; break;
			}

			board_info.slot_position = slot_position;
			board_info.magazine = magazine;
			board_info.fbn = fbn;

			// Add the board info into the vector
			boards.push_back(board_info);
		}
	}

	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);

	if (undo) { // ERROR: in CS get functions
		boards.clear();
		unbind();
		return return_code;
	}

	if (boards.size() <= 0) { // ERROR: No APBM boards found in the configuration table
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No APBM boards found in the CS configuration table");
		unbind();
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_apbm_boards (std::vector<acs_apbm_csboardinfo> & boards, uint32_t magazine) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (const int call_result = bind_to_cs()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'bind_to_cs' failed: cannot bind the CS API to read configuration data: call_result == %d", call_result);
		return call_result;
	}
	uint16_t sysno;	
	if (const int call_result = get_my_system_number(sysno)) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_system_number' failed: cannot get my magazine address from CS: call_result == %d", call_result);
                return call_result;
        }
	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
		return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}

	// search criteria
	bs->reset();
	bs->setMagazine(magazine);
	bs->setSysNo(sysno);
	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getBoardIds' failed: cannot get board ids from CS in the magazine == 0x%08X: cs_call_result == %d", magazine, cs_call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_CS_GET_BOARD_ID;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board_list.size() == %zu", board_list.size());

	// Clear the vector
	boards.clear();

	if (board_list.size() <= 0) { // NO switch board found in the system
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No boards found for the magazine 0x%08X in the CS configuration table", magazine);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_NO_ERRORS;
	}

	// Retrieving board info
	int undo = 0;
	uint16_t slot_position;
	uint16_t fbn;
	acs_apbm_csboardinfo board_info;
	int return_code = acs_apbm::ERR_NO_ERRORS;

	for (size_t i = 0; i < board_list.size(); ++i) {
		BoardID board_id = board_list[i];

		if ((cs_call_result = _hwc->getFBN(fbn, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board fbn
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getFBN' failed: cannot get the fbn type for the current board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
			return_code = acs_apbm::ERR_CS_GET_FBN;
			undo = 1; break;
		}

		// Is an AP board: APUB, GEA, GED, Disk?
		if (is_ap_board_fbn(fbn)) { // YES
			if ((cs_call_result = _hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board slot
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSlot' failed: cannot get the slot number for the current board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
				return_code = acs_apbm::ERR_CS_GET_SLOT;
				undo = 1; break;
			}

			board_info.slot_position = slot_position;
			board_info.magazine = magazine;
			board_info.fbn = fbn;

			// Add the board info into the vector
			boards.push_back(board_info);
		}
	}

	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);

	if (undo) { // ERROR: in CS get functions
		boards.clear();
		unbind();
		return return_code;
	}

	if (boards.size() <= 0) { // ERROR: No APBM boards found in the configuration table
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No APBM boards found for the magazine 0x%08X in the CS configuration table", magazine);
		unbind();
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::reset_info () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	_my_ap_node.system_id = acs_dsd::SYSTEM_ID_UNKNOWN;
	*_my_ipna_str = 0;
	*_my_ipnb_str = 0;
	*_my_bgcia_str = 0;
	*_my_bgcib_str = 0;
	_my_magazine = 0;
	*_my_magazine_str = 0;
	_my_slot_position = -1;
	_my_partner_slot_position = -1;
	_is_cba_system = -1;
	_is_multicp_system = -1;
	_my_ap_info_loaded = 0;
	_my_side = 0;

	unbind();

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::uint32_to_ip_format (char (& value_ip) [16], uint32_t value) {
	// Converting value address into IP dotted-quad format "ddd.ddd.ddd.ddd"
	in_addr addr;
	addr.s_addr = value;
	errno = 0;
	if (!::inet_ntop(AF_INET, &addr, value_ip, ACS_APBM_ARRAY_SIZE(value_ip))) { // ERROR: in conversion
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR,
				"Call 'inet_ntop' failed: cannot convert uint32 address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", value);
		value_ip[0] = 0;
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::uint32_to_ip_format (char (& value_ip) [16], uint32_t value, char separator) {
	uint8_t ip_fields [4];

	uint32_to_ip_fields(ip_fields, value);

	::snprintf(value_ip, ACS_APBM_ARRAY_SIZE(value_ip), "%u%c%u%c%u%c%u",
			ip_fields[0], separator, ip_fields[1], separator, ip_fields[2], separator, ip_fields[3]);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::ip_format_to_uint32 (uint32_t & value, const char * value_ip) {
	in_addr addr;

	errno = 0;
	if (const int call_result = ::inet_pton(AF_INET, value_ip, &addr) <= 0) { // ERROR: in conversion
		if (call_result) ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'inet_pton' failed");
		else ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'inet_pton' failed: value_ip parameter does not contain a valid network string address");

		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;
	}

	value = addr.s_addr;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::uint32_to_ip_fields (uint8_t & ip1, uint8_t & ip2, uint8_t & ip3, uint8_t & ip4, uint32_t value) {
	uint8_t * p = reinterpret_cast<uint8_t *>(&value);

	ip1 = *p++; ip2 = *p++; ip3 = *p++; ip4 = *p++;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::uint32_to_ip_fields (uint8_t (& ip_fields) [4], uint32_t value) {
	uint8_t * p = reinterpret_cast<uint8_t *>(&value);

	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(ip_fields); ip_fields[i++] = *p++) ;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::ip_format_to_ip_fields (uint8_t & ip1, uint8_t & ip2, uint8_t & ip3, uint8_t & ip4, const char * value_ip) {
	uint32_t ip = 0;

	if (const int call_result = ip_format_to_uint32(ip, value_ip)) return call_result;

	return uint32_to_ip_fields(ip1, ip2, ip3, ip4, ip);
}

int __CLASS_NAME__::ip_format_to_ip_fields (uint8_t (& ip_fields) [4], const char * value_ip) {
	uint32_t ip = 0;

	if (const int call_result = ip_format_to_uint32(ip, value_ip)) return call_result;

	return uint32_to_ip_fields(ip_fields, ip);
}

int __CLASS_NAME__::get_my_ap_all_info () {
	if (_my_ap_info_loaded) return acs_apbm::ERR_NO_ERRORS;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (_my_ap_info_loaded) return acs_apbm::ERR_NO_ERRORS;

	if (const int call_result = bind_to_cs()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'bind_to_cs' failed: cannot bind the CS API to read configuration data: call_result == %d", call_result);
		return acs_apbm::ERR_CS_API_BINDING;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
		return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}

	ACS_DSD_Node my_node;

	if (const int call_result = get_my_ap_node(my_node)) { // ERROR: getting my ap node info from DSD
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_ap_node' failed: cannot retrieve my AP node information from configuration: call_result == %d", call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return call_result;
	}

	// Check the my_node side.
	if ((my_node.node_side != acs_dsd::NODE_SIDE_A) && (my_node.node_side != acs_dsd::NODE_SIDE_B)) { // WARNING: undefined my_node state. Cannot continue
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Undefined my node side: there are problems in the system: cannot continue to get my AP info");
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_NODE_SIDE_UNDEFINED;
	}

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Searching criteria: my_node.system_id == %d, my_node.node_side == %d", my_node.system_id, my_node.node_side);

	// search criteria
	bs->reset();
	bs->setFBN(ACS_CS_API_HWC_NS::FBN_APUB);
	bs->setSysType(ACS_CS_API_HWC_NS::SysType_AP);
	bs->setSysId(my_node.system_id);
//	bs->setSide(my_node.node_side == acs_dsd::NODE_SIDE_A ? ACS_CS_API_HWC_NS::Side_A : ACS_CS_API_HWC_NS::Side_B);

	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getBoardIds' failed: cannot get AP board ids from CS: cs_call_result == %d", cs_call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_CS_GET_BOARD_ID;
	}

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board_list.size() == %zu", board_list.size());

	// Should there be at least one board
	if (board_list.size() <= 0) { // ERROR: no boards found
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No AP boards found in the CS system configuration table");
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	for (size_t i = 0; i < board_list.size(); ++i) {
		BoardID board_id = board_list[i];
		uint16_t side = 0;

		if ((cs_call_result = _hwc->getSide(side, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting side
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSide' failed: cannot retrieve the side of the board id == %u from CS: cs_call_result == %d",
					board_id, cs_call_result);
			ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
			unbind();
			return acs_apbm::ERR_CS_GET_SIDE;
		}
		if (((side == ACS_CS_API_HWC_NS::Side_A) && (my_node.node_side == acs_dsd::NODE_SIDE_B)) || ((side == ACS_CS_API_HWC_NS::Side_B) && (my_node.node_side == acs_dsd::NODE_SIDE_A))) {
			// This is my partner node. I must save the slot position only

			//... my partner slot position
			uint16_t slot_position;
			if ((cs_call_result = _hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board slot
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSlot' failed: cannot get my partner slot number from CS: cs_call_result == %d", cs_call_result);
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				unbind();
				return acs_apbm::ERR_CS_GET_SLOT;
			}

			_my_partner_slot_position = slot_position;
		} else if (((side == ACS_CS_API_HWC_NS::Side_A) && (my_node.node_side == acs_dsd::NODE_SIDE_A)) || ((side == ACS_CS_API_HWC_NS::Side_B) && (my_node.node_side == acs_dsd::NODE_SIDE_B))) {
			// This is my node. I must save a lot of info
			_my_side = side;
			// Now I can get my IPN addresses ...
			char ipna_address [16];
			char ipnb_address [16];
			if (const int call_result = get_ip_eth_addresses(ipna_address, ipnb_address, board_id)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_ip_eth_addresses' failed: cannot get IPN addresses for my AP: call_result == %d", call_result);
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				unbind();
				return call_result;
			}

			//... my magazine address ...
			uint32_t magazine;
			if ((cs_call_result = _hwc->getMagazine(magazine, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting magazine info
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getMagazine' failed: cannot retrieve my AP magazine address from CS: cs_call_result == %d", cs_call_result);
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				unbind();
				return acs_apbm::ERR_CS_GET_MAGAZINE;
			}

			//... my slot position
			uint16_t slot_position;
			if ((cs_call_result = _hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board slot
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSlot' failed: cannot get my slot number from CS: cs_call_result == %d", cs_call_result);
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				unbind();
				return acs_apbm::ERR_CS_GET_SLOT;
			}

			//... my system number
			uint16_t system_no;
			if ((cs_call_result = _hwc->getSysNo(system_no, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the system number
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSysNo' failed: cannot get my system number from CS: cs_call_result == %d", cs_call_result);
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				unbind();
				return acs_apbm::ERR_CS_GET_SYSNO;
			}

			// Converting magazine address into IP dotted-quad format "ddd.ddd.ddd.ddd"
			char magazine_str[16];

			if (const int call_result = uint32_to_ip_format(magazine_str, magazine)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'uint32_to_ip_format' failed: cannot convert magazine address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", magazine);
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				return call_result;
			}


			// Now I can get my BGCI addresses if we're in BSP environment
			int env = -1;
			if (const int call_result = get_environment(env))
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_environment' failed: cannot get my BGCI addresses from CS.");
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				unbind();
				return call_result;
			}

			if (env == ACS_CS_API_CommonBasedArchitecture::DMX)
			{
				char bgci_ip_str [2][16];
				uint32_t  bgci_addresses[2] = {0};

				for (int i = 0; i < 2; ++i){

					cs_call_result = (i ? _hwc->getBgciIPEthB(bgci_addresses[i], board_id)
							: _hwc->getBgciIPEthA(bgci_addresses[i], board_id));
					if (cs_call_result != ACS_CS_API_NS::Result_Success) {
						// ERROR: getting the IPN addresses
						ACS_APBM_LOG(LOG_LEVEL_ERROR,
								"Call 'getBgciIPEth%c' failed: cannot get an BGCI-%c IP address from CS: cs_call_result == %d", 'A' + i, 'A' + i, cs_call_result);
						ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
						unbind();
						return acs_apbm::ERR_CS_GET_IP_ETH;
					}
				}


				for (int i = 0; i < 2; ++i) {
					uint32_t address = htonl(bgci_addresses[i]);
					if (const int call_result = uint32_to_ip_format(bgci_ip_str[i], address)) {
						ACS_APBM_LOG(LOG_LEVEL_ERROR,
								"Call 'uint32_to_ip_format' failed: cannot convert IP address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", address);
						ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
						unbind();
						return call_result;
					}
				}
				::strncpy(_my_bgcia_str, bgci_ip_str[0], ACS_APBM_ARRAY_SIZE(_my_bgcia_str));
				::strncpy(_my_bgcib_str, bgci_ip_str[1], ACS_APBM_ARRAY_SIZE(_my_bgcib_str));
			}

			//===================================================================================

			// Saving all info into the internal class fields
			::strncpy(_my_ipna_str, ipna_address, ACS_APBM_ARRAY_SIZE(_my_ipna_str));
			::strncpy(_my_ipnb_str, ipnb_address, ACS_APBM_ARRAY_SIZE(_my_ipnb_str));

			_my_magazine = magazine;
			::strncpy(_my_magazine_str, magazine_str, ACS_APBM_ARRAY_SIZE(_my_magazine_str));
			_my_slot_position = slot_position;
			_my_system_number = system_no;
		}
	}

	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);

	_my_ap_info_loaded = 1;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_switch_boards_info (std::vector <acs_apbm_switchboardinfo> & boards, uint32_t magazine, uint16_t switch_board_fbn, bool match) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (const int call_result = bind_to_cs()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'bind_to_cs' failed: cannot bind the CS API to read configuration data: call_result == %d", call_result);
		return call_result;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
		return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}

	// search criteria
	bs->reset();
	bs->setFBN(switch_board_fbn);
	if (match) bs->setMagazine(magazine);

	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getBoardIds' failed: cannot get switch board ids from CS: cs_call_result == %d", cs_call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_CS_GET_BOARD_ID;
	}

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board_list.size() == %zu", board_list.size());

	// Clear the vector
	boards.clear();

	if (board_list.size() <= 0) { // NO switch board found in the system
		if (match)
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No Switch boards found for the magazine 0x%08X in the CS system configuration table", magazine);
		else
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No Switch boards found in all other magazines but the one 0x%08X in the CS system configuration table", magazine);

		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_NO_ERRORS;
	}

	// Retrieving switch board info
	int undo = 0;
	uint16_t slot_position;
	char ipna_address [16];
	char ipnb_address [16];
	char ipaliasa_address [16];
	char ipaliasb_address [16];
	uint32_t sb_magazine = magazine;
	acs_apbm_switchboardinfo switch_board_info;
	int return_code = acs_apbm::ERR_NO_ERRORS;

	// Retrieving information for each switch board in my magazine
	for (size_t i = 0; i < board_list.size(); ++i) {
		BoardID board_id = board_list[i];

		if (!match) {
			if ((cs_call_result = _hwc->getMagazine(sb_magazine, board_id)) != ACS_CS_API_NS::Result_Success) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getMagazine' failed: cannot get the magazine address for the current switch board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
				return_code = acs_apbm::ERR_CS_GET_MAGAZINE;
				undo = 1; break;
			}

			if (sb_magazine == magazine) continue;
		}

/*
		// Discard all switch board that do not solve the match criterion
		if (match) {
			if (sb_magazine != magazine) continue;
		} else {

			if (sb_magazine == magazine) continue;
		}
*/

		if ((cs_call_result = _hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board slot
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSlot' failed: cannot get the slot number for the current switch board searched (i = %zu): cs_call_result == %d", i, cs_call_result);
			return_code = acs_apbm::ERR_CS_GET_SLOT;
			undo = 1; break;
		}

		if(switch_board_fbn == SWITCH_BOARD_FBN_SMXB)
		{
			if (const int call_result = get_ip_eth_addresses(ipna_address, ipnb_address,ipaliasa_address, ipaliasb_address, board_id)) {
                                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_ip_eth_addresses' failed: cannot get IP addresses for the current switch board searched (i = %zu): call_result == %d", i, call_result);
                                return_code = call_result;
                                undo = 1; break;
                        }

                        switch_board_info.slot_position = slot_position;
                        switch_board_info.magazine = sb_magazine;
                        ::strncpy(switch_board_info.ipna_str, ipna_address, ACS_APBM_ARRAY_SIZE(switch_board_info.ipna_str));
                        ::strncpy(switch_board_info.ipnb_str, ipnb_address, ACS_APBM_ARRAY_SIZE(switch_board_info.ipnb_str));
                        ::strncpy(switch_board_info.ipaliasa_str, ipaliasa_address, ACS_APBM_ARRAY_SIZE(switch_board_info.ipaliasa_str));
                        ::strncpy(switch_board_info.ipaliasb_str, ipaliasb_address, ACS_APBM_ARRAY_SIZE(switch_board_info.ipaliasb_str));

		}
		else
		{

			if (const int call_result = get_ip_eth_addresses(ipna_address, ipnb_address, board_id)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_ip_eth_addresses' failed: cannot get IPN addresses for the current switch board searched (i = %zu): call_result == %d", i, call_result);
				return_code = call_result;
				undo = 1; break;
			}

			switch_board_info.slot_position = slot_position;
			switch_board_info.magazine = sb_magazine;
			::strncpy(switch_board_info.ipna_str, ipna_address, ACS_APBM_ARRAY_SIZE(switch_board_info.ipna_str));
			::strncpy(switch_board_info.ipnb_str, ipnb_address, ACS_APBM_ARRAY_SIZE(switch_board_info.ipnb_str));
		}

		// Add the switch board info into the vector
		boards.push_back(switch_board_info);
	}

	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);

	if (undo) { // ERROR: in CS get functions
		boards.clear();
		unbind();
		return return_code;
	}

	if (boards.size() <= 0) { // ERROR: No Switch boards found in the CS configuration table
		if (match){
			ACS_APBM_LOG(LOG_LEVEL_WARN, "No SWITCH boards found for the magazine 0x%08X in the CS configuration table", magazine);
			unbind();
		}
		else ACS_APBM_LOG(LOG_LEVEL_INFO, "No SWITCH boards found in all other magazine but the one 0x%08X in the CS configuration table", magazine);  // TR HV75981

	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_ip_eth_addresses (char (& ipna_address) [16], char (& ipnb_address) [16], BoardID board_id) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	uint32_t ipn_addresses[2];

	for (int i = 0; i < 2; ++i){
                
		int cs_call_result = (i ? _hwc->getIPEthB(ipn_addresses[i], board_id)
 : _hwc->getIPEthA(ipn_addresses[i], board_id));
                if (cs_call_result != ACS_CS_API_NS::Result_Success) {
			// ERROR: getting the IPN addresses
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'getIPEth%c' failed: cannot get an IPN%c address from CS: cs_call_result == %d", 'A' + i, 'A' + i, cs_call_result);
			return acs_apbm::ERR_CS_GET_IP_ETH;
		}
        }

	// Converting IP addresses in the dotted-quad format "ddd.ddd.ddd.ddd"
	char ip_str[2][16];

	for (int i = 0; i < 2; ++i) {
		uint32_t address = htonl(ipn_addresses[i]);
		if (const int call_result = uint32_to_ip_format(ip_str[i], address)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'uint32_to_ip_format' failed: cannot convert IP address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", address);
			return call_result;
		}
	}

	::strncpy(ipna_address, ip_str[0], ACS_APBM_ARRAY_SIZE(ipna_address));
	::strncpy(ipnb_address, ip_str[1], ACS_APBM_ARRAY_SIZE(ipnb_address));

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_ap_sys_no(int ap_board_name, std::vector<uint16_t> & ap_sys_no_list){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (const int call_result = bind_to_cs()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'bind_to_cs' failed: cannot bind the CS API to read configuration data: call_result == %d", call_result);
		return call_result;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'createBoardSearchInstance' failed: cannot create a CS board seach instance to perform a configuration search");
		return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}
	uint16_t ap_board_fbn = ap_boardname_to_fbn (ap_board_name);
	// search criteria
	bs->reset();
	bs->setFBN(ap_board_fbn);

	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getBoardIds' failed: cannot get switch board ids from CS: cs_call_result == %d", cs_call_result);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_CS_GET_BOARD_ID;
	}

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board_list.size() == %zu", board_list.size());

	if (board_list.size() <= 0) { // NO switch board found in the system
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No '%d' boards found in the CS system configuration table", ap_board_fbn);
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		unbind();
		return acs_apbm::ERR_NO_ERRORS;
	}

	int return_code = acs_apbm::ERR_NO_ERRORS;
	// Clear the list
	ap_sys_no_list.clear();
	uint16_t sysNo;

	// Retrieving information
	for (size_t i = 0; i < board_list.size(); ++i) {
		BoardID board_id = board_list[i];
		// Get boardid from boardList
		ACS_CS_API_NS::CS_API_Result cs_call_result = _hwc->getSysNo(sysNo, board_id);
		if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting  SysNo from CS
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSysNo' failed: cannot get systems number from CS: cs_call_result == %d", cs_call_result);
			return_code = acs_apbm::ERR_CS_GET_BOARD_ID;
			break;
		}
		ap_sys_no_list.push_back(sysNo);
	}

	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
	unbind();

	return return_code;
}

int __CLASS_NAME__::get_dmxc_addresses(char (& dmxc_ipa_str) [16], char (& dmxc_ipb_str) [16])
{
	if (strlen(_dmxc_ipa_str) > 0 && strlen(_dmxc_ipb_str) > 0)
	{
		::strncpy(dmxc_ipa_str, _dmxc_ipa_str, ACS_APBM_ARRAY_SIZE(dmxc_ipa_str));
		::strncpy(dmxc_ipb_str, _dmxc_ipa_str, ACS_APBM_ARRAY_SIZE(dmxc_ipb_str));
		return acs_apbm::ERR_NO_ERRORS;
	}
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if (strlen(_dmxc_ipa_str) > 0 && strlen(_dmxc_ipb_str) > 0)
	{
		::strncpy(dmxc_ipa_str, _dmxc_ipa_str, ACS_APBM_ARRAY_SIZE(dmxc_ipa_str));
		::strncpy(dmxc_ipb_str, _dmxc_ipb_str, ACS_APBM_ARRAY_SIZE(dmxc_ipb_str));
		return acs_apbm::ERR_NO_ERRORS;
	}

	uint32_t dmxc_ip[2] = {0};
	ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NetworkElement::getDmxcAddress(dmxc_ip[0], dmxc_ip[1]);

	if (cs_call_result == ACS_CS_API_NS::Result_Success)
	{
		uint32_t address = htonl(dmxc_ip[0]);
		if (const int call_result = uint32_to_ip_format(_dmxc_ipa_str, address)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'uint32_to_ip_format' failed: cannot convert IP address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", address);
			return call_result;
		}

		address = htonl(dmxc_ip[1]);
		if (const int call_result = uint32_to_ip_format(_dmxc_ipb_str, address)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'uint32_to_ip_format' failed: cannot convert IP address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", address);
			return call_result;
		}
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getDmxcAddress' failed: cannot get DMXC addresses from CS: cs_call_result == %d", cs_call_result);
		return acs_apbm::ERR_CS_ERROR;
	}

	::strncpy(dmxc_ipa_str, _dmxc_ipa_str, ACS_APBM_ARRAY_SIZE(dmxc_ipa_str));
	::strncpy(dmxc_ipb_str, _dmxc_ipb_str, ACS_APBM_ARRAY_SIZE(dmxc_ipb_str));
	return acs_apbm::ERR_NO_ERRORS;

}


int __CLASS_NAME__::get_ip_eth_addresses (char (& ipna_address) [16], char (& ipnb_address) [16],char (& ipaliasa_address) [16], char (& ipaliasb_address) [16], BoardID board_id) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	uint32_t ipn_addresses[2];
	uint32_t ipalias_addresses[2];

	for (int i = 0; i < 2; ++i){
                
		int cs_call_result = (i ? _hwc->getIPEthB(ipn_addresses[i], board_id)
			: _hwc->getIPEthA(ipn_addresses[i], board_id));
                if (cs_call_result != ACS_CS_API_NS::Result_Success) {
			// ERROR: getting the IPN addresses
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'getIPEth%c' failed: cannot get an IPN%c address from CS: cs_call_result == %d", 'A' + i, 'A' + i, cs_call_result);
			return acs_apbm::ERR_CS_GET_IP_ETH;
		}
		
		cs_call_result = (i ? _hwc->getAliasEthB(ipalias_addresses[i], board_id)
			: _hwc->getAliasEthA(ipalias_addresses[i], board_id));
                if (cs_call_result != ACS_CS_API_NS::Result_Success) {
                        // ERROR: getting the IPalias addresses
                        ACS_APBM_LOG(LOG_LEVEL_ERROR,
                                        "Call 'getIPAliasEth%c' failed: cannot get an transport IP%c address from CS: cs_call_result == %d", 'A' + i, 'A' + i, cs_call_result);
                        return acs_apbm::ERR_CS_GET_IP_ETH;
		
		}
        }

	// Converting IP addresses in the dotted-quad format "ddd.ddd.ddd.ddd"
	char ip_str[2][16];
	char ipalias_str[2][16];

	for (int i = 0; i < 2; ++i) {
		uint32_t address = htonl(ipn_addresses[i]);
		if (const int call_result = uint32_to_ip_format(ip_str[i], address)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'uint32_to_ip_format' failed: cannot convert IP address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", address);
			return call_result;
		}

		uint32_t alias_address = htonl(ipalias_addresses[i]);
		if (const int call_result = uint32_to_ip_format(ipalias_str[i], alias_address)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'uint32_to_ip_format' failed: cannot convert IP address (0x%08X) in the dotted-quad format 'ddd.ddd.ddd.ddd'", alias_address);
			return call_result;
		}
	}

	::strncpy(ipna_address, ip_str[0], ACS_APBM_ARRAY_SIZE(ipna_address));
	::strncpy(ipnb_address, ip_str[1], ACS_APBM_ARRAY_SIZE(ipnb_address));

	::strncpy(ipaliasa_address, ipalias_str[0], ACS_APBM_ARRAY_SIZE(ipaliasa_address));
	::strncpy(ipaliasb_address, ipalias_str[1], ACS_APBM_ARRAY_SIZE(ipaliasb_address));

	return acs_apbm::ERR_NO_ERRORS;
}
