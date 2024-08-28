/** @file acs_apbm_shelvesdatamanager.cpp
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
 *      |       | 2015-04-21 | xkaikum      | TR HT64065                          |
 *      +=======+============+==============+=====================================+
 */

#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_ipmiupgradehandler.h"
#include "acs_apbm_immrepositoryhandler.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_snmpmanager.h"
#include <ACS_DSD_Client.h>
#include "acs_apbm_boardsessionhandler.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_logger.h"
#include <acs_apbm_api.h>
#include <acs_apbm_primitivedatahandler.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Guard_T.h>
#include <time.h>
#include <utility>
#include <list>
#include <new>
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/stat.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_CLibTypes.h>
const char OP_State[] = "operationalState";

acs_apbm_shelvesdatamanager::~acs_apbm_shelvesdatamanager()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_apboards(_apboards_map_mutex);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_switchboards(_switchboards_map_mutex);

	for(MapMagazineToAPBoards::iterator it = _apboards_map.begin(); it != _apboards_map.end(); ++it)
	{
		APBoardsCollection * pApBoards = (*it).second;
		if(!pApBoards)
			continue;
		for(APBoardsCollection::iterator it_1 = pApBoards->begin(); it_1 != pApBoards->end(); ++it_1)
			delete (*it_1);
	}

	for(MapMagazineToSwitchBoards::iterator it = _switchboards_map.begin(); it != _switchboards_map.end(); ++it)
	{
		SwitchBoardsCollection * pSwitchBoards = (*it).second;
		if(!pSwitchBoards)
			continue;
		for(SwitchBoardsCollection::iterator it_1 = pSwitchBoards->begin(); it_1 != pSwitchBoards->end(); ++it_1)
			delete (*it_1);
	}
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::load_from_CS(bool /*add_info_to_IMM*/)
{
	//FETCH CS INFO
	is_cs_operation_completed = 0;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "is_cs_operation_completed = 0");
	int call_result = -1;
	uint32_t magazine = 0;
	std::vector<acs_apbm_csboardinfo> boards;

	// Get APG shelf architecture (SCB, SCX, DMX, VIRTUALIZED,SMX, ...)
	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");  // not a critical error for the purpose of this method
	}

/*
//	ACS_APBM_LOG(LOG_LEVEL_WARN, "Removing existing ap board info from IMM\n");
//	_server_working_set->imm_repository_handler->remove_all_ap_board_info(); // return 0 forever
	_server_working_set->imm_repository_handler->remove_ap_board_info("apBladeInfoId=0");
	_server_working_set->imm_repository_handler->remove_ap_board_info("apBladeInfoId=25");

	::exit(0);

	return SDM_OK;
*/


	_server_working_set->cs_reader->get_my_magazine(magazine);
	if (magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "failed to get magazine == 0x%08X\n", magazine);
		return SDM_INVALID_MAGAZINE_DATA;
	} else	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MAGAZINE == 0x%08X\n", magazine);
	call_result = add_magazine(magazine);
	if ( (call_result != SDM_MAGAZINE_ALREADY_PRESENT) && (call_result < 0))
	{ // ADD ONLY MY MAGAZINE
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "add magazine failed! call_result == %d\n", call_result);
		return SDM_INVALID_MAGAZINE_DATA;
	}
	_server_working_set->cs_reader->get_apbm_boards(boards,magazine);
	if (boards.size() == 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "failed in magazine 0x%08X ==>> apbm_boards.size == %zu\n", magazine, boards.size());
		return SDM_APBOARD_NOT_FOUND;
	} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "FETCHED: in magazine 0x%08X ==>> apbm_boards.size == %zu\n", magazine, boards.size());
	for (size_t i = 0; i < boards.size(); ++i){
		acs_apbm_apboardinfo board;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  %zu) magazine == 0x%08X, slot == %d, fbn == %u\n", i, boards[i].magazine, boards[i].slot_position, boards[i].fbn);

		board.magazine = boards[i].magazine;
		board.slot_position = boards[i].slot_position;			// copy the input argument <apboardinfo> into the new AP board info object
		board.fbn = boards[i].fbn;

		if ( (board.fbn == ACS_CS_API_HWC_NS::FBN_APUB) || (board.fbn == ACS_CS_API_HWC_NS::FBN_GEA) )
			board.bus_type = axe_eth_shelf::BUS_TYPE_IPMI;
		else if ( (board.fbn == ACS_CS_API_HWC_NS::FBN_Disk) || (board.fbn == ACS_CS_API_HWC_NS::FBN_DVD))
			board.bus_type = axe_eth_shelf::BUS_TYPE_MBUS;
		else board.bus_type = axe_eth_shelf::BUS_TYPE_UNDEFINED;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"AFTER LOAD CS ==> magazine == 0x%08X - slot == %d - board.fbn: %d - board.bus_type == %d",board.magazine, board.slot_position, board.fbn, board.bus_type);
	 
		if ((call_result = set_ap_board_info(board, true, boards[i].slot_position, boards[i].magazine)) < 0){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "failed set board in magazine 0x%08X at slot == %zu\n error == '%d'\n",boards[i].magazine, boards[i].slot_position,call_result);
			return SDM_ERROR;
		}

		/*
		 *  To be checked !! At the moment, for virtualized enviromnment, the following call to 'update_ap_board_default_nic_values()'
		 *  is necessary in order to add in IMM the info about the IP addresses and MAC addresses associated to NICs.
		 *  Instead, for SCB, SCX and DMX enviroments, this activity is performed by "acs_apbm_shelvesdatamanager::load_snmp_data()'.
		 */
		if (((env == -1) || (env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)) && ((call_result = update_ap_board_default_nic_values(board.slot_position, board.magazine)) <0))
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR,"update_ap_board_default_nic_values - result %d, env= %d",call_result,env);
			return SDM_ERROR;
		}

		if((call_result = update_ap_board_default_led_values(board.slot_position, board.magazine)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "failed update_ap_board_default_led_values in magazine 0x%08X at slot == %zu\n error == '%d'\n",boards[i].magazine, boards[i].slot_position,call_result);
			return SDM_ERROR;
		}
	}

	//TO STORE IPMI-UPGRADE-RESULT of SCB/RP4
	acs_apbm_apboardinfo scb_board;
	scb_board.magazine = magazine;
	scb_board.slot_position = 0;
	_server_working_set->imm_repository_handler->set_ap_board_info_for_scb(scb_board);
	scb_board.slot_position = 25;
	_server_working_set->imm_repository_handler->set_ap_board_info_for_scb(scb_board);

	is_cs_operation_completed = 1;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "is_cs_operation_completed = 1");
	sdm_constant ret_val = SDM_OK;
	return ret_val;
}
/*
* This function initialises the variable is_4led_supported to true if the HW supports 4 leds.
* Else is_4led_supported is set to false;
*/
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_board_environment(int32_t slot)
{
	acs_apbm_api api;
	if(api.is_gep5_gep7())
	{
		// Checks for the MIB type
		unsigned timeout=1000;
		acs_apbm_snmp::blade_led_type_t blade_led_type;
		uint32_t magazine = 0;
		int call_result = _server_working_set->cs_reader->get_my_magazine(magazine);
		if(call_result < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own magazine. Call 'acs_apbm_csreader::get_my_magazine(..)' failed ! (call_result == %d)", call_result);
			return SDM_ERROR;
		}

		_server_working_set->snmp_manager->get_blade_led_type(blade_led_type, slot, &timeout);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "get_blade_led_type() ---- %d \n",blade_led_type);
		int env =_server_working_set->cs_reader->get_environment(env); //SCB = 0, SCX = 1, DMX = 2
		std::vector<acs_apbm_switchboardinfo> board_infos;
		acs_apbm_apboardinfo board;
		_server_working_set->snmp_manager->get_my_switch_board_info(board_infos);

		if (!board_infos.size()) { // WARNING: No switch board found in my magazine
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "No switch boards found in my magazine: cannot ping any switch board ");
		}

		if (!call_result)
		{
			// Checks if the HW supports 4 leds.
			if (blade_led_type ==  acs_apbm_snmp::BLADE_LED_TYPE_ON)
			{	
				is_4led_supported=true;
			}
			else if ((blade_led_type == acs_apbm_snmp::BLADE_LED_TYPE_NOT_SUPPORTED) && (env == ACS_CS_API_CommonBasedArchitecture::SCB))
			{
				for (size_t i = 0; i < board_infos.size(); ++i)
				{
					_server_working_set->snmp_manager->get_board_data(board.product_number, board.product_revision_state, board.product_name, board.serial_number, board.manufacture_date, board.vendor, board_infos[i].slot_position, magazine, &timeout);
					if ((strcmp(board.product_revision_state,SCB_R1A)==0) || (strcmp(board.product_revision_state,SCB_R2A)==0) || (strcmp(board.product_revision_state,SCB_R3A)==0))
					{
						ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Board Product Revision state is: %s  ",board.product_revision_state);
						int callresult=_server_working_set->alarmevent_handler->raise_unsupported_config_event();
						ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "Unsupported Configuration! callresult == %d", callresult);
						if(callresult != 0)
						{
							ACS_APBM_LOG(LOG_LEVEL_ERROR, "Failed to raise the event for unsupported configuration.");
						}
						return SDM_ERROR;
					}
				}
			}
		}
		else
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error: get_blade_led_type()");
			return SDM_ERROR;
		}
	}
	else
		is_4led_supported=false;    // Doesnot support 4 leds.

	return SDM_OK;
}
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_gea_environment(int32_t slot)
{
	unsigned timeout=1000;
	acs_apbm_snmp::blade_led_type_t blade_led_type;
	uint32_t magazine = 0;  
	bool raise_event = false;
	int call_result = _server_working_set->cs_reader->get_my_magazine(magazine);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own magazine. Call 'acs_apbm_csreader::get_my_magazine(..)' failed ! (call_result == %d)", call_result);
		return SDM_ERROR;
	}

	int env =_server_working_set->cs_reader->get_environment(env); //SCB = 0, SCX = 1, DMX = 2, VIRTUALIZED =3,SMX = 4
	std::vector<acs_apbm_switchboardinfo> board_infos;
	acs_apbm_apboardinfo board;
	_server_working_set->snmp_manager->get_my_switch_board_info(board_infos);

	if (!board_infos.size()) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No switch boards found in my magazine: cannot ping any switch board ");
	}

	call_result= _server_working_set->snmp_manager->get_blade_led_type(blade_led_type, slot, &timeout);
	ACS_APBM_LOG(LOG_LEVEL_INFO, "get_blade_led_type() in get_gea_environment() is ---- %d \n",blade_led_type);

	if (!call_result)
	{
		// Checks if the GEA board is new GEA.
		if (blade_led_type ==  acs_apbm_snmp::BLADE_LED_TYPE_ON)
		{
			is_new_gea_supported=true;
		}
		else if (blade_led_type == acs_apbm_snmp::BLADE_LED_TYPE_OLD)
		{
			is_new_gea_supported=false;
		}
		else if ((blade_led_type == acs_apbm_snmp::BLADE_LED_TYPE_NOT_SUPPORTED) && (env == ACS_CS_API_CommonBasedArchitecture::SCB)) 
		{
			//Unable to identify the GEA type checking blade led type (SCB-RP does not support it).
			//Check product number instead
			ACS_APBM_LOG(LOG_LEVEL_INFO, "BLADE LED TYPE NOT SUPPORTED!");
			_server_working_set->snmp_manager->get_board_data(board.product_number, board.product_revision_state, board.product_name, board.serial_number,board.manufacture_date, board.vendor, slot, magazine, &timeout);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Board Product Number: %s  ",board.product_number);

			if(strcmp(board.product_number,NEW_GEA_ROJ)==0)
			{
				//It's the new GEA, but SCB-RP does not support it. Raise unsupported configuration alarm.
				for (size_t i = 0; i < board_infos.size(); ++i)
				{
					_server_working_set->snmp_manager->get_board_data(board.product_number, board.product_revision_state, board.product_name, board.serial_number, board.manufacture_date, board.vendor, board_infos[i].slot_position, magazine, &timeout);
					if ((strcmp(board.product_revision_state,SCB_R1A)==0) || (strcmp(board.product_revision_state,SCB_R2A)==0) || (strcmp(board.product_revision_state,SCB_R3A)==0))
					{
						ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Board Product Revision state is: %s  ",board.product_revision_state);
						raise_event = true;
						break;
					}
				}
			}

			if (raise_event)
			{
				int callresult=_server_working_set->alarmevent_handler->raise_unsupported_config_event();
				ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "Unsupported Configuration! callresult == %d", callresult);
				if(callresult != 0)
				{
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Failed to raise the event for unsupported configuration.");
				}
			}
		}
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error: get_blade_led_type()");
		return SDM_ERROR;
	}
	return SDM_OK;
}
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::load_snmp_data(bool /*add_info_to_IMM*/)
{
	is_snmp_data_operation_completed = 0;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "is_snmp_data_operation_completed = 0");
	int call_result = -1;
        //FETCH CS INFO
        uint32_t magazine = 0;

        //TODO - CHECK BOARD STATUS IF TRUE OK ELSE RETURN

        _server_working_set->cs_reader->get_my_magazine(magazine);
        if (magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED){
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "failed to get magazine == 0x%08X\n", magazine);
                return SDM_INVALID_MAGAZINE_DATA;
        }else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MAGAZINE == 0x%08X\n", magazine);

        int env = 0;
        call_result = _server_working_set->cs_reader->get_environment(env);
        if(call_result != acs_apbm::ERR_NO_ERRORS)
        {
        	ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error in getting Shelf Environment");
        	return SDM_ERROR;
        }

        ACE_Guard<ACE_Recursive_Thread_Mutex> guard_apboards(_apboards_map_mutex);

        MapMagazineToAPBoards::const_iterator it = _apboards_map.find(magazine);
        if(it == _apboards_map.end())
                return SDM_MAGAZINE_NOT_FOUND;

        // get the AP Boards collection associated to the magazine
        APBoardsCollection * const & p_apboards_collection = (*it).second;
        if(!p_apboards_collection)
        	return SDM_INVALID_MAGAZINE_DATA;

        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"COLLECTION SIZE: %u ",p_apboards_collection->size());

        int exist_valid_board = 0;
        for(uint32_t slot=0; slot < p_apboards_collection->size(); slot++)
        {
        	acs_apbm_apboardinfo * board = p_apboards_collection->at(slot);

        	if (board)
        	{
        		exist_valid_board = 1;
        		acs_apbm_shelvesdatamanager::sdm_constant ret = SDM_OK;

        		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  %u) magazine == 0x%08X, slot == %d, fbn == %u\n", slot, board->magazine, board->slot_position, board->fbn );

        		//CHECK ARCHITECTURE - GET BOARD DATA OR BOARD PRESENTE UPDATE IMM
        		unsigned timeout = 1000;

        		if (env == ACS_CS_API_CommonBasedArchitecture::DMX)
        		{
        			axe_eth_shelf::hw_installation_status_t status;
        			//On result of this snmp get, it will be decided the logic state of board
        			call_result = _server_working_set->snmp_manager->get_hw_install_status(status,
        					board->slot_position,
        					board->magazine, &timeout);

        			if (status == axe_eth_shelf::NOT_INSTALLED)
        			{
        				board->board_status = axe_eth_shelf::BOARD_STATUS_MISSING;
        				ret = set_ap_board_status(board->board_status, true,board->slot_position,board->magazine);

        				if (ret == SDM_IMM_ERROR){
        					ACS_APBM_LOG(LOG_LEVEL_FATAL,"Error. IMM problem, trying to set board status");
        					return ret;
        				}

        				_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, board->slot_position);
        				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Board Missing - get_board_data result %d -- set_ap_board_status: result %d",call_result, ret);
        				continue;

        			}
				else if(status == axe_eth_shelf::INSTALLED_AND_ACCEPTABLE)
				{

				call_result = _server_working_set->snmp_manager->get_board_data(board->product_number, board->product_revision_state, board->product_name, board->serial_number,
                                        board->manufacture_date, board->vendor, board->slot_position, board->magazine, &timeout);
				if (call_result < 0)
				return SDM_SNMP_ERROR;

				}

        			else if (status == axe_eth_shelf::UNKNOWN)
        			{
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Generic error on get board snmp data - result %d",call_result);
        				return SDM_SNMP_ERROR;
        			}
        		}
        		else
        		{
				call_result = _server_working_set->snmp_manager->get_board_data(board->product_number, board->product_revision_state, board->product_name, board->serial_number,
                                        board->manufacture_date, board->vendor, board->slot_position, board->magazine, &timeout);
        			if( call_result == acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_RESOURCEUNAVAILABLE ||
        					call_result == acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_NO_SUCH_INSTANCE ||
        					call_result == acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_BAD_TYPE ||
        					call_result == acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_AUTHORIZATIONERROR){
        				board->board_status = axe_eth_shelf::BOARD_STATUS_MISSING;

        				ret = set_ap_board_status(board->board_status, true,board->slot_position,board->magazine);

        				if (ret == SDM_IMM_ERROR){
        					ACS_APBM_LOG(LOG_LEVEL_FATAL,"Error. IMM problem, trying to set board status");
        					return ret;
        				}
        				_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, board->slot_position);
        				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Board Missing - get_board_data result %d -- set_ap_board_status: result %d",call_result, ret);
        				continue;
        			}
        			if (call_result < 0)
        			{
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Generic error on get board snmp data - result %d",call_result);
        				return SDM_SNMP_ERROR;
        			}
        		}
        		if(board->fbn == ACS_CS_API_HWC_NS::FBN_APUB)
        		{

        			// GET THE BOARD ENVIRONMENT
        			// This function call initialises the variable is_4led_supported.
        			ACS_APBM_LOG(LOG_LEVEL_INFO,"Getting the board environment - get_board_environment.....");
        			int returnCode = get_board_environment(board->slot_position);
        			ACS_APBM_LOG(LOG_LEVEL_INFO,"get_board_environment in load_snmp_data called.....");
        			ACS_APBM_LOG(LOG_LEVEL_INFO,"is_4led_supported value - %d .....",is_4led_supported);
        			if(returnCode != SDM_OK)
        			{
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error: get_board_environment, error %d", returnCode);
        				return SDM_SNMP_ERROR;
        			}
        		}
        		else if(board->fbn == ACS_CS_API_HWC_NS::FBN_GEA)
        		{
        			int returnCode = get_gea_environment(board->slot_position);
        			if(returnCode != SDM_OK)
        			{
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error: get_gea_environment, error %d", returnCode);
        				return SDM_SNMP_ERROR;
        			}
        		}


        		//SNMP UPDATE
        		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE magazine 0x%08X ==>> slot_position == %d\n"
        				"product_number=%s - product_revision_state=%s - product_name=%s - serial_number=%s"
        				" - manufacture_date=%s - vendor ", board->magazine, board->slot_position,
        				board->product_number, board->product_revision_state, board->product_name, board->serial_number,
        				board->manufacture_date, board->vendor);
        		if( (call_result = _server_working_set->imm_repository_handler->update_snmp_ap_board_info(board)) <0 ){
        			ACS_APBM_LOG(LOG_LEVEL_ERROR,"update result %d",call_result);
        			//continue;
        		}
        		//NIC INFO - added NIC information to aligned with snmp data
        		if ( (call_result = update_ap_board_default_nic_values(board->slot_position, board->magazine)) <0 ){
        			ACS_APBM_LOG(LOG_LEVEL_ERROR,"update_ap_board_default_nic_values - result %d",call_result);
        			//continue;
        		}
        		//Temperature information INFORMATION
        		if ( (call_result = update_ap_board_temperature_status(board->temperature_status, true, board->slot_position, board->magazine)) < 0)
        			ACS_APBM_LOG(LOG_LEVEL_ERROR,"update_temperature_status_on_ap_board_info - result %d",call_result);
        		//Alarm interface status
        		if ( (call_result = update_ap_board_alarm_interface_status(board->alarm_interface_status, true, board->slot_position, board->magazine)) < 0)
        			ACS_APBM_LOG(LOG_LEVEL_ERROR,"update_ap_board_alarm_interface_status - result %d",call_result);

        		if(is_4led_supported)
        		{
        			if ( (call_result = update_ap_board_drbd_status(board->drbd_disk_status, true, board->slot_position, board->magazine)) < 0)
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"update_ap_board_drbd_status - result %d",call_result);
        		}
        		else
        		{
        			if ( (call_result = update_ap_board_raid_status(board->raid_disk_status, true, board->slot_position, board->magazine)) < 0)
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"update_ap_board_raid_status - result %d",call_result);
        		}
	
        		//Check whether we need to read four or three leds
        		bool has_4_leds = (board->fbn == ACS_CS_API_HWC_NS::FBN_GEA)? is_new_gea_supported: is_4led_supported;

        		//Update LEDS
        		if(has_4_leds)   // If HW supports four LEDS
        		{
        			if((call_result = _server_working_set->snmp_manager->get_board_leds_status(board->green_led,board->red_led,board->mia_led, board->blue_led, board->slot_position, board->magazine, &timeout)) < 0 )
        			{
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"get_board_leds_status - result %d",call_result);
        				//continue;
        			}
        			else
        			{
        				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "In shelves data manager: UPDATE LED STATUS: green_led == %d - red_led == %d - mia_led == %d - blue_led = %d",board->green_led,board->red_led,board->mia_led,board->blue_led);
        				if((call_result = set_ap_board_LEDS_status(board->green_led,board->red_led,board->mia_led,board->blue_led,true, board->slot_position, board->magazine)) < 0 )
        				{
        					ACS_APBM_LOG(LOG_LEVEL_ERROR,"UPDATE LED STATUS FAILED - result %d",call_result);
        					//continue;
        				}
        			}
        		}
        		else                          // If HW supports only three LEDS
        		{
        			if((call_result = _server_working_set->snmp_manager->get_board_leds_status(board->green_led,board->red_led,board->mia_led, board->slot_position, board->magazine, &timeout)) == 0 )
        			{
        				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE LED STATUS: green_led == %d - red_led == %d - mia_led == %d",board->green_led,board->red_led,board->mia_led);
        				if((call_result = set_ap_board_LEDS_status(board->green_led,board->red_led,board->mia_led,true, board->slot_position, board->magazine)) < 0 )
        				{
        					ACS_APBM_LOG(LOG_LEVEL_ERROR,"UPDATE LED STATUS FAILED - result %d",call_result);
        					//continue;
        				}
        			}
        			else
        			{
        				ACS_APBM_LOG(LOG_LEVEL_ERROR,"get_board_leds_status - result %d",call_result);
        				//continue;
        			}
        		}

        		//TODO - IPMI SNMP INFORMATION - DELETED FUNCTIONALITY
        		timeout = 1000;
        		//                      if ( (call_result = _server_working_set->snmp_manager->get_board_ipmi_data(board->ipmi_firmware_type, board->ipmi_product_number, board->ipmi_product_revision,
        		//                                                                                                      board->ipmi_upgrade_status,board->slot_position, board->magazine, &timeout)) < 0 )
        		//                      {
        		//                              ACS_APBM_LOG(LOG_LEVEL_ERROR,"get_board_ipmi_data - result %d",call_result);
        		//                              //continue;
        		//                      }
        		//                      ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE IPMI DATA: ipmi_firmware_type == '%d' - ipmi_product_number == '%s' - "
        		//                                              "ipmi_product_revision == '%s' - ipmi_upgrade_status == '%d'",board->ipmi_firmware_type, board->ipmi_product_number, board->ipmi_product_revision, board->ipmi_upgrade_status);
        		//                      if( (call_result = update_ap_board_ipmi_data(board->ipmi_firmware_type, board->ipmi_product_number, board->ipmi_product_revision, false, board->slot_position, board->magazine)) <0 ){
        		//                              ACS_APBM_LOG(LOG_LEVEL_ERROR,"UPDATE IPMI DATA FAILED - result %d",call_result);
        		//                              //continue;
        		//                      }
        		if( (call_result = update_ap_board_ipmi_upgrade_status(board->ipmi_upgrade_status, true, board->slot_position, board->magazine)) <0 ){
        			ACS_APBM_LOG(LOG_LEVEL_ERROR,"UPDATE IPMI DATA FAILED - result %d",call_result);
        			//continue;
        		}


        		acs_apbm_snmp::bladePwr_status_t power_status = acs_apbm_snmp::BLADE_PWR_ON;
        		call_result = _server_working_set->snmp_manager->get_board_power_status( power_status, board->slot_position, board->magazine, &timeout);

        		if (call_result < 0)
        		{
        			ACS_APBM_LOG(LOG_LEVEL_ERROR,"get_board_power_status FAILED - result %d",call_result);
        			return SDM_SNMP_ERROR;
        		}

        		if  (/*board->mia_led == axe_eth_shelf::LED_STATUS_ON && */ power_status == acs_apbm_snmp::BLADE_PWR_OFF /*OFF*/)
        		{
        			board->board_status = axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED;
        			ret = set_ap_board_status(board->board_status, true, board->slot_position, board->magazine);
        			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"board data result %d -- set_ap_board_status: result %d",call_result,ret);
        			if (ret == SDM_IMM_ERROR){
        				ACS_APBM_LOG(LOG_LEVEL_FATAL,"Error. IMM problem, trying to set board status");
        				return ret;
        			}
        			//continue;
        		}
        		else
        		{
        			//TR_HT64065 -- Start
        			if(has_4_leds)
        			{
            			//DO NOT TURN RED LED OFF!!!
        				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Skipping slot == %d, fbn == %u magazine = 0x%08X with out setting RED LED to OFF",board->slot_position, board->fbn, board->magazine);
        			}
        			else
        			{
            			//TURN RED LED OFF!!!
        				ACS_APBM_LOG(LOG_LEVEL_INFO,"SETTING RED LED TO OFF ! slot == %d, magazine = 0x%08X  fbn = %u \n", board->slot_position, board->magazine, board->fbn);
        				if(_server_working_set->snmp_manager->set_board_REDled(magazine, slot, false) < 0)
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
        			//TR_HT64065 -- End
        			//BOARD EXIST AND STATUS IS WORKING
        			board->board_status = axe_eth_shelf::BOARD_STATUS_WORKING;
        			ret = set_ap_board_status(board->board_status, true, board->slot_position, board->magazine);
        			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"board data result %d -- set_ap_board_status: result %d",call_result,ret);
        			if (ret == SDM_IMM_ERROR){
        				ACS_APBM_LOG(LOG_LEVEL_FATAL,"Error. IMM problem, trying to set board status");
        				return ret;
        			}
        			_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
        		}


        		//TODO - IMM UPDATE OTHER ATTR
        	}
        }


        if(!exist_valid_board){
        	ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error not exist valid board in the collection!");
        	return SDM_DATA_UNAVAILABLE;
        }

        is_snmp_data_operation_completed = 1;
        ACS_APBM_LOG(LOG_LEVEL_INFO, "is_snmp_data_operation_completed = 1");
        sdm_constant ret_val = SDM_OK;
        return ret_val;

}


int acs_apbm_shelvesdatamanager::get_local_ap_board_bios_version(char (& bios_product_name) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME], char (& bios_product_version) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME])
{
       const char * prod_name = "/sys/devices/virtual/dmi/id/product_name";
       const char * prod_version = "/sys/devices/virtual/dmi/id/product_version";
       FILE *fs_n, * fs_v;
       fs_n = ::fopen(prod_name,"r"); // file to read
       if(fs_n == 0)
       {
           ACS_APBM_LOG(LOG_LEVEL_ERROR,"error on fopen %s!", prod_name);
           return SDM_ERROR;
       }
       fs_v = ::fopen(prod_version,"r"); // file to read
       if(fs_v == 0)
       {
           ACS_APBM_LOG(LOG_LEVEL_ERROR,"error on fopen %s!", prod_version);
           return SDM_ERROR;
       }

      char ch;
      int j=0,i=0;
      for(i = 0; (ch = ::getc(fs_n) ) != EOF; i++ )
      {
          (ch == '\n') ? bios_product_name[i]='\0'  : bios_product_name[i] = ch;
      }
      for(j = 0; (ch = getc(fs_v) ) != '\n'; j++ )
          bios_product_version[j] = ch;
	  bios_product_version[j]='\0';

      ::fclose(fs_n);
      ::fclose(fs_v);

      return 0;
}


int acs_apbm_shelvesdatamanager::get_local_ap_board_disks_status(unsigned & disks_status)
{
	acs_apbm_api api;
	
	// launch a Unix command to get the list of connected SCSI disk devices
	FILE * fd;
	const char * command = "lsscsi";
	errno = 0;
	if(!(fd = popen(command, "r")))
	{
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'popen()' failed !");
		return SDM_ERROR;
	}

	// read command output and count DISK devices
	char buffer[1024];
	char tmpstr[1024];

        char tmp[1024];
	unsigned n_connected_disks = 0;
	while(fgets(buffer, sizeof(buffer), fd))
	{
		// got a command output line ! Check if it refers to a DISK device
		tmpstr[0] = 0;
		sscanf(buffer, "[%*d:%*d:%*d:%*d] %s", tmpstr);
		sscanf(buffer, "%[^/]s", tmp);//Extracts string until a delimiter '/' is reached (/dev/sdb)
                int len = strlen(tmp);
                const char *dskpath = buffer+len;
                    //count disks other than flash and nanausb for gep5 or gep7
                    if(api.is_gep5_gep7()) {
		        if((strncmp(dskpath,"/dev/sda",8) != 0) &&   //flash disk
		           (strncmp(dskpath,"/dev/sdm",8) != 0)) {   //nano usb
	                   ACS_APBM_LOG(LOG_LEVEL_DEBUG, "GEP5 or GEP7 disk path : %s", dskpath);
		            ++n_connected_disks;	
                        }
                    }
                    else {//gep1 or gep2
		        if(!strncmp(tmpstr,"disk",4)) {
	                   ACS_APBM_LOG(LOG_LEVEL_DEBUG, "GEP1 or GEP2 disk path : %s", dskpath);
		            ++n_connected_disks;	// found a DISK device !
                        }
                    }
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Number of disks connected : %d", n_connected_disks);
	// check for errors
	if(ferror(fd))
	{
		pclose(fd);
		return SDM_ERROR;
	}

	disks_status = n_connected_disks;
	pclose(fd);

	return SDM_OK;
}
//Redesign as per TR-HS30773
int acs_apbm_shelvesdatamanager::get_local_ap_board_thumbdrive_status(axe_eth_shelf::thumbdrive_status_t & thumbdrive_status)
{
	struct stat buffer ={0};
        int status=0;
	errno= 0;
        if ( (stat("/dev/eri_thumbdrive", &buffer) == 0 ) && S_ISBLK(buffer.st_mode))
        {
                thumbdrive_status = axe_eth_shelf::THUMBDRIVE_STATUS_AVAILABLE;
        }
        else
        {
                if (errno == ENOENT)
                {
                        thumbdrive_status = axe_eth_shelf::THUMBDRIVE_STATUS_NOT_AVAILABLE;
                        ACS_APBM_LOG(LOG_LEVEL_WARN, "Thumb drive not available\n");
                }
		else
                {
                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Enexpected error ocurred with error number %d\n", errno);
                        return SDM_ERROR;
                }
        }
        return SDM_OK;

}

int acs_apbm_shelvesdatamanager::getoperationalStateFromIMM()
{
        OmHandler immHandle;
        int operState = 0;
        ACS_CC_ReturnType result;
        ACS_CC_ImmParameter* opState = new ACS_CC_ImmParameter();

        result = immHandle.Init();
        if (result != ACS_CC_SUCCESS)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "immHandle INIT failure ");
                cout << "ERROR: Initialization FAILURE!!!\n";
                return -1;
        }

	opState->attrName = (char*)OP_State;

        std::string dnName = ACS_EMF_DN;
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN %s", dnName.c_str());
        result = immHandle.getAttribute(const_cast<char*>(dnName.c_str()), opState);
        if (result == ACS_CC_SUCCESS)
        {
		if (opState->attrValuesNum != 0)
		{
                operState = *(reinterpret_cast<int*>(*(opState->attrValues)));
                ACS_APBM_LOG(LOG_LEVEL_DEBUG, "operState value is : %d", operState);
        	}
	}
        else
        {
	        int exitCode = 0;    
		exitCode = immHandle.getInternalLastError();
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "getAttribute error %d on DN %s", exitCode, dnName.c_str());
        }

	delete opState;
	immHandle.Finalize();
        return operState;
}
//Redesign as per TR-HS30773

int acs_apbm_shelvesdatamanager::fcopy(const char * input, const char * output){

	if(!input || !*input)
	{
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Empty Input path!");
		return SDM_ERROR;
	}
	if(!output || !*output)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Empty Output path! ");
		return SDM_ERROR;
	}

	FILE* source = fopen(input, "rb");
	if (source == 0)
	{
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen()' failed !");
		return SDM_ERROR;
	}

	const char * tmp_name = basename(input);//"/only_ipmiupgrade_filename.mot";
	ACS_APBM_LOG(LOG_LEVEL_INFO,"Call basename(...). from path: '%s' saved basename '%s'",input, tmp_name);
	char tmp_path[1024] = {0};
	::snprintf(tmp_path,1024,"%s%s", output, tmp_name);
	FILE* dest = fopen(tmp_path, "wb");
	if (dest == 0)
	{
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen()' failed !");
		fclose(source);
		return SDM_ERROR;
	}

  char buf[BUFSIZ];
  size_t size;
  // feof(FILE* stream) returns non-zero if the end of file indicator for stream is set
  while ((size = fread(buf, 1, BUFSIZ, source))) {
      fwrite(buf, 1, size, dest);
  }
  fclose(source);
  fclose(dest);

	return 0;

}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::add_magazine(uint32_t magazine)
{
	// get exclusive access to internal maps
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_apboards(_apboards_map_mutex);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_switchboards(_switchboards_map_mutex);

	// check if magazine is already present
	MapMagazineToAPBoards::const_iterator it = _apboards_map.find(magazine);
	if(it != _apboards_map.end())
		return SDM_MAGAZINE_ALREADY_PRESENT;

	// create a new APBoards collection
	APBoardsCollection * p_apboards = new (std::nothrow) APBoardsCollection(MAX_SLOT_NUM + 1, static_cast<acs_apbm_apboardinfo *>(0));
	if(!p_apboards)
		return SDM_MEM_ALLOC_ERROR;

	// create a new SwitchBoards collection
	SwitchBoardsCollection * p_switchboards = new (std::nothrow) SwitchBoardsCollection(NUM_SWITCH_BOARDS_IN_A_MAGAZINE, 0);
	if(!p_switchboards)
		return SDM_MEM_ALLOC_ERROR;

	try
	{
		// associate the new APBoards collection to the magazine
		_apboards_map.insert(MapMagazineToAPBoards::value_type(magazine, p_apboards));

		// associate the new SwitchBoards collection to the magazine
		_switchboards_map.insert(MapMagazineToSwitchBoards::value_type(magazine, p_switchboards));
	}
	catch(std::bad_alloc & excp)
	{
		return SDM_MEM_ALLOC_ERROR;
	}

	return SDM_OK;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_ap_board_info(acs_apbm_apboardinfo & apboardinfo, int32_t slot, uint32_t magazine)  const
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if <magazine> argument has not been supplied, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// get the address of the object containing the AP board info
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// check result
	if(call_result == SDM_OK)
	apboardinfo = *p_apboardinfo; 	// copy AP board info into the output parameter

	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::set_ap_board_info(const acs_apbm_apboardinfo & apboardinfo, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		return call_result;
	}
	// if <magazine> argument has not been supplied, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();	
	
	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// search magazine
	MapMagazineToAPBoards::const_iterator it = _apboards_map.find(magazine);
	if(it == _apboards_map.end())
		return SDM_MAGAZINE_NOT_FOUND;

	// get the AP Boards collection associated to the magazine
	APBoardsCollection * const & p_apboards_collection = (*it).second;
	if(!p_apboards_collection)
		return SDM_INVALID_MAGAZINE_DATA;

	// start updating IMM, if requested
	if (update_IMM)
	{
		int op_res = _server_working_set->imm_repository_handler->set_ap_board_info(const_cast<acs_apbm_apboardinfo &>(apboardinfo));
		if(op_res < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot add AP board info into IMM ( MAG == %u, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::set_ap_board_info(..)' failed. op_res == %d !", magazine, slot, op_res);
			return SDM_IMM_ERROR;
		}
	}

	// destroy the (eventually) existing AP board info object
	acs_apbm_apboardinfo * p_current_apboard_info = p_apboards_collection->at(slot);
	if(p_current_apboard_info)
	{
		call_result = SDM_APBOARD_REPLACED;		// take note that we're going to replace the existing AP board info object
		delete p_current_apboard_info;

	}

	// create a new AP board info object
	acs_apbm_apboardinfo * p_new_apboardinfo = new (std::nothrow) acs_apbm_apboardinfo();
	if(p_new_apboardinfo)
	{
		// copy the input argument <apboardinfo> into the new AP board info object
		*p_new_apboardinfo = apboardinfo;

		// update the AP boards collection
		p_apboards_collection->at(slot) = p_new_apboardinfo;
	}
	else
		call_result = SDM_MEM_ALLOC_ERROR;

	return call_result;
}
acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::reset_ap_board_info(bool update_IMM, int32_t slot, uint32_t magazine)
{
        acs_apbm_shelvesdatamanager::sdm_constant update_result = SDM_OK;
        sdm_constant call_result = SDM_OK;

		int environment=0;
		_server_working_set->cs_reader->get_environment(environment);
		if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
		{
			return call_result;
		}
        // if the method has been called without passing <magazine> argument, own magazine is assumed
        if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
                magazine = own_magazine();		
		
        // check parameters
        if((update_result = _check_parameters(slot, magazine)) != SDM_OK)
                return update_result;

        // get exclusive access to AP boards collection
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

        // retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
        acs_apbm_apboardinfo * board = 0;
        update_result = _retrieve_ap_board_info_ptr(board, slot, magazine);
        // update info
        if(update_result == SDM_OK){

            uint16_t old_fbn = board->fbn;
            axe_eth_shelf::bus_type_t old_bus_type = board->bus_type;

            acs_apbm_apboardinfo new_board_info;

            * board = new_board_info;

            board->slot_position = slot;
            board->magazine = magazine;
            board->fbn = old_fbn;
            board->bus_type = old_bus_type;


            if (update_IMM)
            {
                int op_res = _server_working_set->imm_repository_handler->reset_ap_board_info(board);
                if(op_res < 0)
                {
                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot reset AP board info into IMM ( MAG == %u, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::reset_ap_board_info(..)' failed. op_res == %d !", magazine, slot, op_res);
                        return SDM_IMM_ERROR;
                }
            }

        }
        return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_info(bool update_IMM, int32_t slot, uint32_t magazine)
{
	sdm_constant update_result = SDM_OK;
	int call_result = -1;
	uint32_t timeout = 500;

	// if SNMP adta collection is still on going skip the whole method
	if (!is_snmp_data_operation_completed)
		return update_result;

	int env = 0;
	call_result = _server_working_set->cs_reader->get_environment(env);
	if(call_result != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error in getting Shelf Environment");
		return SDM_ERROR;
	}

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if ((update_result = _check_parameters(slot, magazine)) != SDM_OK)
		return update_result;

/*========================================================================*/
	acs_apbm_apboardinfo board;
	sdm_constant get_ap_board_info_result = get_ap_board_info(board, slot, magazine);
	if (get_ap_board_info_result != SDM_OK) return get_ap_board_info_result;

	//CHECK ARCHITECTURE - GET BOARD DATA OR BOARD PRESENTE UPDATE IMM
	int call_result_get_board_data = _server_working_set->snmp_manager->get_board_data(board.product_number,
			board.product_revision_state, board.product_name, board.serial_number,
			board.manufacture_date, board.vendor, board.slot_position,
			board.magazine, &timeout);
	int call_result_get_board_leds_status=0;
	sdm_constant ret_val = SDM_OK;
	if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(board.fbn, slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
		return ret_val;
	}

	//Check whether we need to read four or three leds
	bool has_4_leds = (board.fbn == ACS_CS_API_HWC_NS::FBN_GEA)? is_new_gea_supported: is_4led_supported;

	if(has_4_leds) // If HW supports four LEDS
	{
		if((call_result_get_board_leds_status = _server_working_set->snmp_manager->get_board_leds_status(board.green_led,board.red_led,board.mia_led, board.blue_led, board.slot_position, board.magazine, &timeout)) == 0 )
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE LED STATUS: green_led == %d - red_led == %d - mia_led == %d - blue_led == %d",board.green_led,board.red_led,board.mia_led,board.blue_led);
			if( (call_result = set_ap_board_LEDS_status(board.green_led, board.red_led, board.mia_led, board.blue_led, true, board.slot_position, board.magazine)) < 0 )
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"UPDATE LED STATUS FAILED - result %d", call_result_get_board_leds_status);
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_ERROR,"get_board_leds_status - result %d", call_result_get_board_leds_status);
	}
	else   // If HW supports three LEDS
	{
		if ( (call_result_get_board_leds_status = _server_working_set->snmp_manager->get_board_leds_status(board.green_led,board.red_led,board.mia_led, board.slot_position, board.magazine, &timeout)) == 0 )
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE LED STATUS: green_led == %d - red_led == %d - mia_led == %d",board.green_led,board.red_led,board.mia_led);
			if( (call_result_get_board_leds_status = set_ap_board_LEDS_status(board.green_led, board.red_led, board.mia_led, true, board.slot_position, board.magazine)) < 0 )
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"UPDATE LED STATUS FAILED - result %d", call_result_get_board_leds_status);
		} else
			ACS_APBM_LOG(LOG_LEVEL_ERROR,"get_board_leds_status - result %d", call_result_get_board_leds_status);
	}


	int call_result_get_board_ipmi_data = _server_working_set->snmp_manager->get_board_ipmi_data(
			board.ipmi_firmware_type, board.ipmi_product_number, board.ipmi_product_revision,
			board.ipmi_upgrade_status, board.slot_position, board.magazine, &timeout);
/*========================================================================*/


	// update info
	int ret = 0;

	if (env == ACS_CS_API_CommonBasedArchitecture::DMX)
	{
		axe_eth_shelf::hw_installation_status_t status;
		//On result of this snmp get, it will be decided the logic state of board
		call_result = _server_working_set->snmp_manager->get_hw_install_status(status,
				board.slot_position,
				board.magazine, &timeout);

		if (status == axe_eth_shelf::NOT_INSTALLED)
		{
			board.board_status = axe_eth_shelf::BOARD_STATUS_MISSING;
			ret = set_ap_board_status(board.board_status, update_IMM, board.slot_position,board.magazine);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board HW installation status result %d -- set_ap_board_status: result %d",
					call_result_get_board_data, ret);
			_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, board.slot_position);
			return SDM_OK;
		}
		else if (status == axe_eth_shelf::UNKNOWN)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "generic error on get board HW installation status - result %d", call_result_get_board_data);
			board.board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
			ret = set_ap_board_status(board.board_status, update_IMM, board.slot_position, board.magazine);
			return SDM_SNMP_ERROR;
		}
	}
	else
	{
		if (call_result_get_board_data == acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_RESOURCEUNAVAILABLE ||
				call_result_get_board_data == acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_NO_SUCH_INSTANCE ||
				call_result_get_board_data == acs_apbm_snmp::ERR_SNMP_PDU_VARIABLE_BAD_TYPE ||
				call_result_get_board_data == acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_AUTHORIZATIONERROR) {
			board.board_status = axe_eth_shelf::BOARD_STATUS_MISSING;
			ret = set_ap_board_status(board.board_status, update_IMM, board.slot_position,board.magazine);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board data result %d -- set_ap_board_status: result %d",
					call_result_get_board_data, ret);
			_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, board.slot_position);
			return SDM_OK;
		}

		if (call_result_get_board_data < 0) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "generic error on get board snmp data - result %d", call_result_get_board_data);
			board.board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
			ret = set_ap_board_status(board.board_status, update_IMM, board.slot_position, board.magazine);
			return SDM_SNMP_ERROR;
		}
	}

	if ((call_result = update_ap_board_snmp_data(board.product_number, board.product_revision_state,
			board.product_name, board.serial_number, board.manufacture_date, board.vendor, true,
			board.slot_position, board.magazine)) < 0)
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"update result %d", call_result);
	else
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE magazine 0x%08X ==>> slot_position == %d\n"
			"product_number=%s - product_revision_state=%s - product_name=%s - serial_number=%s"
			" - manufacture_date=%s - vendor", board.magazine, board.slot_position,
			board.product_number, board.product_revision_state, board.product_name, board.serial_number,
			board.manufacture_date, board.vendor);


	//IPMI SNMP INFORMATION
	if (call_result_get_board_ipmi_data == 0) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPDATE IPMI DATA: ipmi_firmware_type == '%d' - ipmi_product_number == '%s' - "
				"ipmi_product_revision == '%s' - ipmi_upgrade_status == '%d'", board.ipmi_firmware_type,
				board.ipmi_product_number, board.ipmi_product_revision, board.ipmi_upgrade_status);

		if ((call_result = update_ap_board_ipmi_data(board.ipmi_firmware_type, board.ipmi_product_number,
				board.ipmi_product_revision, false, board.slot_position, board.magazine)) < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "UPDATE IPMI DATA FAILED - result %d", call_result);

		if ((call_result = update_ap_board_ipmi_upgrade_status(board.ipmi_upgrade_status, true, board.slot_position,
				board.magazine)) < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR,"UPDATE IPMI DATA FAILED - result %d", call_result);
	} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "get_board_ipmi_data - result %d", call_result_get_board_ipmi_data);

		acs_apbm_snmp::bladePwr_status_t power_status = acs_apbm_snmp::BLADE_PWR_ON;
		int set_power_result = _server_working_set->snmp_manager->set_board_Power(magazine, slot, power_status);
	        if(set_power_result < 0)
        	{
                	ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting POWER ON for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_Power(..)' failed ! set_power_result == %d'", magazine, slot, set_power_result);
		}
                _server_working_set->snmp_manager->get_board_power_status(power_status, board.slot_position, board.magazine, &timeout);

                if  (power_status == acs_apbm_snmp::BLADE_PWR_OFF)
                {
                        board.board_status = axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED;
                        ret = set_ap_board_status(board.board_status, true, board.slot_position, board.magazine);
                        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"set_ap_board_status: result %d",ret);
                        return SDM_OK;
                }
                else
                {
                		//no need to update IMM, action of IMM Update CallBack
                		//TR_HT64065 -- Start
        				if(has_4_leds)
        				{
        					//DO NOT TURN RED LED OFF!!!
        					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Skipping slot == %d, fbn == %u magazine = 0x%08X with out setting RED LED to OFF",board.slot_position, board.fbn, board.magazine);
        				}
        				else
        				{
        					//TURN RED LED OFF!!!
        					ACS_APBM_LOG(LOG_LEVEL_INFO,"SETTING RED LED TO OFF ! slot == %d, magazine = 0x%08X  fbn = %u \n", board.slot_position, board.magazine, board.fbn);
        					if(_server_working_set->snmp_manager->set_board_REDled(magazine, slot, false) < 0)
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
        				//TR_HT64065 -- End
                        //BOARD EXIST AND STATUS IS WORKING
                        board.board_status = axe_eth_shelf::BOARD_STATUS_WORKING;
                        ret = set_ap_board_status(board.board_status, update_IMM, board.slot_position, board.magazine);
                        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"set_ap_board_status: result %d",ret);
                }
        return SDM_OK;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::remove_ap_board_info(bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{		
		return call_result;
	}
	
	// if <magazine> argument has not been supplied, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();
	
	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// search magazine
	MapMagazineToAPBoards::const_iterator it = _apboards_map.find(magazine);
	if(it == _apboards_map.end())
		return SDM_MAGAZINE_NOT_FOUND;

	// get the AP Boards collection associated to the magazine
	APBoardsCollection * const & p_apboards_collection = (*it).second;
	if(!p_apboards_collection)
		return SDM_INVALID_MAGAZINE_DATA;

	// check if AP board info are present
	acs_apbm_apboardinfo * p_apboard_info = p_apboards_collection->at(slot);
	if(!p_apboard_info)
		return SDM_APBOARD_NOT_FOUND;

	// start updating IMM, if requested
	if(update_IMM)
	{
		int op_res = _server_working_set->imm_repository_handler->remove_ap_board_info(*p_apboard_info);
		if(op_res < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot remove AP board info from IMM ( MAG == %u, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::remove_ap_board_info(..)' failed. op_res == %d !", magazine, slot, op_res);
			return SDM_IMM_ERROR;
		}
	}

	// delete the AP board info object
	delete p_apboard_info;

	// clear slot position
	p_apboards_collection->at(slot) = 0;

	return call_result;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_ap_board_status(axe_eth_shelf::status_t  & apboard_status, int32_t slot, uint32_t magazine) const
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	if(call_result == SDM_OK)
		{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "p_apboardinfo->board_status: %d" , p_apboardinfo->board_status);
		apboard_status = p_apboardinfo->board_status;	// copy AP board status into the output parameter
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "apboard_status: %d", apboard_status);
		}
	return call_result;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_ap_board_bus_type(axe_eth_shelf::bus_type_t & apboard_bus_type, int32_t slot, uint32_t magazine) const
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	if (slot ==0 || slot==25){
		apboard_bus_type = axe_eth_shelf::BUS_TYPE_IPMI;
		return SDM_OK;
	}

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	if(call_result == SDM_OK)
		apboard_bus_type = p_apboardinfo->bus_type;

	return call_result;

}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_ap_board_fbn_and_status(uint16_t & apboard_fbn, axe_eth_shelf::status_t  & apboard_status, int32_t slot, uint32_t magazine) const
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	if(call_result == SDM_OK)
	{
		apboard_status = p_apboardinfo->board_status;	// copy AP board info into the output parameter
		apboard_fbn = p_apboardinfo->fbn;
	}

	return call_result;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_ap_board_fbn(uint16_t & apboard_fbn, int32_t slot, uint32_t magazine) const
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();
	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;
	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	if(call_result == SDM_OK)
		apboard_fbn = p_apboardinfo->fbn;

	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_ipmifw_status(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, char (&ipmifw_lastupgtime) [acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME],int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT)
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	//unsigned timeout = 1000;
	int op_res = 0;
//	if((op_res =_server_working_set->snmp_manager->get_board_ipmi_upgrade_status(ipmi_upg_status, p_apboardinfo->slot_position, p_apboardinfo->magazine, &timeout)) < 0)
//	{
//		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot get board ipmi upgrade status ( MAG == %u, SLOT == %d ). Call 'snmp_manager->get_board_ipmi_upgrade_status(..)' failed. op_res == %d !", magazine, slot, op_res);
//	}

	op_res =_server_working_set->imm_repository_handler->get_board_ipmi_upgrade_status(ipmi_upg_status, slot, magazine);
	if (op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot get board ipmi upgrade last time ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_last_time(..)' failed. op_res == %d !", magazine, slot, op_res);
		return SDM_IMM_ERROR;
	}

	op_res =_server_working_set->imm_repository_handler->get_board_ipmi_upgrade_last_time(ipmifw_lastupgtime, slot, magazine);
	if(op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot get board ipmi upgrade last time ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_last_time(..)' failed. op_res == %d !", magazine, slot, op_res);
		return SDM_IMM_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Correctly retrieved get_ipmifw_status(..! ipmi_upg_status=%d - ipmifw_lastupgtime=%s", ipmi_upg_status, ipmifw_lastupgtime );

//	if(call_result == SDM_OK){
//		ipmi_upg_status = p_apboardinfo->ipmi_upgrade_status;
//		strncpy(ipmifw_lastupgtime, p_apboardinfo->ipmi_upgrade_last_time, acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME);
//	}

	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::request_block_on_ap_board(int16_t & imm_error_code, int32_t slot, uint32_t magazine)
{
	sdm_constant ret_val = SDM_OK;
	imm_error_code = IMM_ACTION_EXIT_CODE_NO_ERROR;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Invoked 'BLOCK' action on AP board identified by: MAGAZINE == 0x%08X, SLOT == %u", magazine, slot);
	//might not be required since apBladeInfo obj is not available
	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_INVALID_PLATFORM;
		return SDM_OP_NOT_PERMITTED;
	}
	
	// check <slot> and <magazine> parameters
	if((ret_val = _check_parameters(slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid AP board parameters. Call '_check_parameters(..)' returned  '%d')", ret_val);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return ret_val;
	}

	// 'BLOCK' action cannot be requested on the APUP board we're running on
	int32_t my_slot;
	uint32_t my_magazine;
	int call_result = _server_working_set->cs_reader->get_my_slot(my_slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own slot. Call 'acs_apbm_csreader::get_my_slot(..)' failed ! (call_result == %d)", call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}
	call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own magazine. Call 'acs_apbm_csreader::get_my_magazine(..)' failed ! (call_result == %d)", call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}
	if((magazine == my_magazine) && (slot == my_slot))
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot execute 'BLOCK' action on APUB board we're running on !");
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_OWN_SLOT_BLOCK;
		return SDM_OP_NOT_PERMITTED;
	}

	// check if AP board exists (i.e. it's physically present) and get its 'fbn' and its status
	axe_eth_shelf::status_t apboard_status;
	uint16_t apboard_fbn;
	if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn_and_status(apboard_fbn, apboard_status, slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn and status. Call 'get_ap_board_fbn_and_status(..) failed ! call_result == %d", ret_val);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return ret_val;
	}

	// check AP board status
	switch(apboard_status)
	{
		case axe_eth_shelf::BOARD_STATUS_MISSING:
			// AP board not physically present in the slot
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'BLOCk' action failed : the requested AP board ( MAG == 0x%08X, SLOT == %d ) is missing !", magazine, slot);
			imm_error_code = IMM_ACTION_EXIX_CODE_ERROR_BOARD_NOT_AVAILABLE;
			return SDM_APBOARD_STATE_MISSING;
		case axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED:
			// AP board is already blocked
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'BLOCk' action failed : the requested AP board ( MAG == 0x%08X, SLOT == %d )is already blocked !", magazine, slot);
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_BOARD_ALREADY_BLOCKED;
			return SDM_APBOARD_STATE_BLOCKED;
		case axe_eth_shelf::BOARD_STATUS_NOT_APPLICABLE:
			// AP board state cannot be retrieved
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'BLOCk' action failed : the state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) cannot be retrieved !", magazine, slot);
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_INVALID_PLATFORM;
			return SDM_APBOARD_STATE_NOT_APPLICABLE;
		case axe_eth_shelf::BOARD_STATUS_UNDEFINED:
			// AP board state cannot be retrieved
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'BLOCk' action failed : the state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) is undefined !", magazine, slot);
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return SDM_OP_NOT_PERMITTED;
		default: //OK
			// AP board status is compatible with the requested action. We can proceed !
			ACS_APBM_LOG(LOG_LEVEL_INFO, "the current state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) is '%d'", magazine, slot, apboard_status);
			break;
	}

	// check AP board type and invoke the appropriate helper method

	if(_action_execution_state)
	  {
	    ACS_APBM_LOG(LOG_LEVEL_ERROR, "Action Ongoing! Operation will be denied!");
	    imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_OPERATION_ONGOING;
	    return SDM_ERROR;
	  } _action_execution_state = 1;

	switch(apboard_fbn)
	{
		case  ACS_CS_API_HWC_NS::FBN_APUB:
			ret_val = _request_block_on_apub_board(slot, magazine);
			break;
		case  ACS_CS_API_HWC_NS::FBN_DVD:
			ret_val = _request_block_on_dvd_board(slot, magazine);
			break;
		case  ACS_CS_API_HWC_NS::FBN_Disk:
			ret_val = _request_block_on_disk_board(slot, magazine);
			break;
		case ACS_CS_API_HWC_NS::FBN_GEA:
			ret_val = _request_block_on_gea_board(slot, magazine);
			break;
		default:
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'BLOCk' action failed : operation not allowed on the the requested AP board ( MAG == 0x%08X, SLOT == %d ). FBN == %u !", magazine, slot, apboard_fbn);
			ret_val = SDM_OP_NOT_PERMITTED;
	}
	_action_execution_state = 0;

	if(ret_val == SDM_OK)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "'BLOCK' action SUCCESSFULLY executed on AP board identified by: MAGAZINE == 0x%08X, SLOT == %u. ret_val == %d", magazine, slot, ret_val);
	else if (ret_val == SDM_RAID_ERROR)
	{
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_RAID_STATUS_FAULTY;
	}
	else if ((apboard_fbn == ACS_CS_API_HWC_NS::FBN_Disk) && (ret_val == SDM_OP_NOT_PERMITTED))
	{
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_BLOCKING_LAST_DISK;
	} else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "'BLOCK' action FAILED on AP board identified by: MAGAZINE == 0x%08X, SLOT == %u. ret_val == %d", magazine, slot, ret_val);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
	}

	return ret_val;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::request_deblock_on_ap_board(int16_t & imm_error_code, int32_t slot, uint32_t magazine)
{
	sdm_constant ret_val = SDM_OK;
	imm_error_code = IMM_ACTION_EXIT_CODE_NO_ERROR;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Invoked 'DEBLOCK' action on AP board identified by: MAGAZINE == 0x%8X, SLOT == %u", magazine, slot);

	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_INVALID_PLATFORM;
		return SDM_OP_NOT_PERMITTED;
	}
	// check <slot> and <magazine> parameters
	if((ret_val = _check_parameters(slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid AP board parameters. Call '_check_parameters(..)' returned  '%d')", ret_val);
		imm_error_code = IMM_ACTION_EXIX_CODE_ERROR_BOARD_NOT_AVAILABLE;
		return ret_val;
	}

	// 'DEBLOCK' action cannot be requested on the APUP board we're running on
	int32_t my_slot;
	uint32_t my_magazine;
	int call_result = _server_working_set->cs_reader->get_my_slot(my_slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own slot. Call 'acs_apbm_csreader::get_my_slot(..)' failed ! (call_result == %d)", call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}
	call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own magazine. Call 'acs_apbm_csreader::get_my_magazine(..)' failed ! (call_result == %d)", call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}
//	if((magazine == my_magazine) && (slot == my_slot))
//	{
//		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot execute 'DEBLOCK' action on APUB board we're running on !");
//		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_OWN_SLOT_BLOCK;
//		return SDM_OP_NOT_PERMITTED;
//	}

	// check if AP board exists (i.e. it's physically present) and get its status
	axe_eth_shelf::status_t apboard_status;
	if((_server_working_set->shelves_data_manager->get_ap_board_status(apboard_status, slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board status !");
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}

	// check AP board status
	switch(apboard_status)
	{
		case axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED:
			break; 	// this is the expected state
		case axe_eth_shelf::BOARD_STATUS_MISSING:
			// AP board not physically present in the slot
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'DEBLOCk' action failed : the requested AP board ( MAG == 0x%08X, SLOT == %d ) is missing !", magazine, slot);
			imm_error_code = IMM_ACTION_EXIX_CODE_ERROR_BOARD_NOT_AVAILABLE;
			return SDM_APBOARD_STATE_MISSING;
		case axe_eth_shelf::BOARD_STATUS_NOT_APPLICABLE:
			// AP board state cannot be retrieved
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'DEBLOCk' action failed : the state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) cannot be retrieved !", magazine, slot);
			imm_error_code = IMM_ACTION_EXIX_CODE_ERROR_BOARD_NOT_AVAILABLE;
			return SDM_APBOARD_STATE_NOT_APPLICABLE;
		default:
			// AP board is already unblocked !
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "'DEBLOCk' action failed because the AP board is NOT blocked ! The state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) is '%d'", magazine, slot, apboard_status);
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_BOARD_ALREADY_DEBLOCKED;
			return SDM_APBOARD_STATE_UNBLOCKED;
	}

	// set AP board status to "WORKING'....
	axe_eth_shelf::status_t old_apboard_status;
	call_result = set_ap_board_status(axe_eth_shelf::BOARD_STATUS_WORKING, true, slot, magazine, old_apboard_status);
	if(call_result != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting status 'WORKING' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", magazine, slot, call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_SET_STATE_ERROR;
	}

	// ask SNMP manager to turn off maintenance led on the specified AP board
        int result;
        uint16_t apboard_fbn;
        if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, slot, magazine)) != SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
                return ret_val;
        }

        //Check whether we need to read four or three leds
        bool has_4_leds = (apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)? is_new_gea_supported: is_4led_supported;

        if(has_4_leds)
        {
        	result = _server_working_set->snmp_manager->set_board_BLUEled(magazine, slot, axe_eth_shelf::LED_STATUS_OFF);
        	if(result < 0)
        	{
        		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting BLUE led OFF for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_BLUEled(..)' failed ! call_result == %d'", magazine, slot, result);

        		// try to restore previous AP board status
        		if((call_result = set_ap_board_status(old_apboard_status, true, slot, magazine)) != SDM_OK)
        			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", magazine, slot, call_result);
        		else
        			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", magazine, slot, call_result);

        		return SDM_SNMP_ERROR;
        	}

        }
        else
        {
        	result = _server_working_set->snmp_manager->set_board_MIAled(magazine, slot, axe_eth_shelf::LED_STATUS_OFF);
        	if(result < 0)
        	{
        		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting MIA led OFF for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_MIAled(..)' failed ! call_result == %d'", magazine, slot, result);

        		// try to restore previous AP board status
        		if((call_result = set_ap_board_status(old_apboard_status, true, slot, magazine)) != SDM_OK)
        			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", magazine, slot, call_result);
        		else
        			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", magazine, slot, call_result);

        		return SDM_SNMP_ERROR;
        	}
        }

	// ask SNMP manager to set POWER ON on the specified AP board
	int set_power_result = _server_working_set->snmp_manager->set_board_Power(magazine, slot, true);

	if(set_power_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting POWER ON for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_Power(..)' failed ! call_result == %d'", magazine, slot, set_power_result);

		// try to restore Maintenance led
		if(has_4_leds)
		{
			result = _server_working_set->snmp_manager->set_board_BLUEled(magazine, slot, axe_eth_shelf::LED_STATUS_ON);
			if(result < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error re-setting BLUE led ON for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_BLUEled(..)' failed ! call_result == %d'", magazine, slot, result);
			else
				ACS_APBM_LOG(LOG_LEVEL_INFO, "BLUE led reset to ON for AP board identified by MAG == 0x%08X, SLOT == %d", magazine, slot);
		}
		else
		{
			result = _server_working_set->snmp_manager->set_board_MIAled(magazine, slot, axe_eth_shelf::LED_STATUS_ON);
			if(result < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error re-setting MIA led ON for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_MIAled(..)' failed ! call_result == %d'", magazine, slot, result);
			else
				ACS_APBM_LOG(LOG_LEVEL_INFO, "MIA led reset to ON for AP board identified by MAG == 0x%08X, SLOT == %d", magazine, slot);
		}

		// try to restore previous AP board status
		if((call_result = set_ap_board_status(old_apboard_status, true, slot, magazine)) != SDM_OK)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", old_apboard_status, magazine, slot, call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", old_apboard_status, magazine, slot, call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_SNMP_ERROR;
	}

	if(ret_val == SDM_OK)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "'DEBLOCK' action SUCCESSFULLY executed on AP board identified by: MAGAZINE == 0x%08X, SLOT == %u. ret_val == %d", magazine, slot, ret_val);
	else
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "'DEBLOCK' action FAILED on AP board identified by: MAGAZINE == 0x%08X, SLOT == %u. ret_val == %d", magazine, slot, ret_val);

	return ret_val;
}
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::request_reset_on_ap_board(int16_t & imm_error_code, int32_t slot, uint32_t magazine)
{
	sdm_constant ret_val = SDM_OK;
	acs_apbm_apboardinfo * board_ptr;

	imm_error_code = IMM_ACTION_EXIT_CODE_NO_ERROR;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Invoked 'RESET' action on AP board identified by: MAGAZINE == 0x%8X, SLOT == %u", magazine, slot);

	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_INVALID_PLATFORM;
		return SDM_OP_NOT_PERMITTED;
	}
	// check <slot> and <magazine> parameters
	if((ret_val = _check_parameters(slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid AP board parameters. Call '_check_parameters(..)' returned  '%d')", ret_val);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return ret_val;
	}

	// 'RESET' action cannot be requested on the APUP board we're running on
	int32_t my_slot;
	uint32_t my_magazine;
	int call_result = _server_working_set->cs_reader->get_my_slot(my_slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own slot. Call 'acs_apbm_csreader::get_my_slot(..)' failed ! (call_result == %d)", call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}
	call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own magazine. Call 'acs_apbm_csreader::get_my_magazine(..)' failed ! (call_result == %d)", call_result);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}
	if((magazine == my_magazine) && (slot == my_slot))
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot execute 'RESET' action on APUB board we're running on !");
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_OWN_SLOT_RESET;
		return SDM_OP_NOT_PERMITTED;
	}

	// check if AP board exists (i.e. it's physically present) and get its status
	axe_eth_shelf::status_t apboard_status;
	if((_server_working_set->shelves_data_manager->get_ap_board_status(apboard_status, slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board status !");
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_ERROR;
	}
	// check AP board status
	switch(apboard_status)
	{
	case axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED:
		// AP board blocked in the slot
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "'RESET' action failed : the requested AP board ( MAG == 0x%08X, SLOT == %d ) is blocked !", magazine, slot);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_RESET_BLOCKED_BOARD;
		return SDM_APBOARD_STATE_BLOCKED;

	case axe_eth_shelf::BOARD_STATUS_MISSING:
		// AP board not physically present in the slot
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "'RESET' action failed : the requested AP board ( MAG == 0x%08X, SLOT == %d ) is missing !", magazine, slot);
		imm_error_code = IMM_ACTION_EXIX_CODE_ERROR_BOARD_NOT_AVAILABLE;
		return SDM_APBOARD_STATE_MISSING;

	case axe_eth_shelf::BOARD_STATUS_NOT_APPLICABLE:
		// AP board state cannot be retrieved
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "'RESET' action failed : the state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) cannot be retrieved !", magazine, slot);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_APBOARD_STATE_NOT_APPLICABLE;
	case axe_eth_shelf::BOARD_STATUS_WORKING:
		//needed state to perform action
		break;
	case (axe_eth_shelf::BOARD_STATUS_UNDEFINED || axe_eth_shelf::BOARD_STATUS_FAULTY):
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_APBOARD_STATE_NOT_APPLICABLE;
	default:
		// AP board is already unblocked !
		// ACS_APBM_LOG(LOG_LEVEL_ERROR, "'RESET' action failed because the AP board is NOT blocked ! The state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) is '%d'", magazine, slot, apboard_status);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return SDM_APBOARD_STATE_NOT_APPLICABLE;
	}

	if ((ret_val = _retrieve_ap_board_info_ptr(board_ptr, slot, magazine)) != SDM_OK)
	{
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return ret_val;
	}

	//check for disk board
	if (board_ptr->fbn == ACS_CS_API_HWC_NS::FBN_Disk)
	{
		if ((ret_val = _check_disks_raid_status(slot, magazine)) != SDM_OK)
		{
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return ret_val;
		}
	}
	else if (board_ptr->fbn == ACS_CS_API_HWC_NS::FBN_APUB)
	{
		if((ret_val = _check_disks_drbd_status(slot, magazine)) != SDM_OK)
		{
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return ret_val;
		}
	}

	//TODO - SET BOARD STATUS TO N/A
	if(const int reset_result = _server_working_set->snmp_manager->set_board_reset(magazine, slot) != 0)
	{
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return ret_val;
	}

	//SCHEDULE OPERATION


	return ret_val;
}

int  acs_apbm_shelvesdatamanager::request_ipmi_upgrade_on_ap_board(int16_t & imm_error_code, const char * firmware_image_path, int32_t slot, uint32_t magazine, uint8_t comport)
{
	int call_result = -1;
	int  ret_val = acs_apbm::ERR_NO_ERRORS;
	imm_error_code = IMM_ACTION_EXIT_CODE_NO_ERROR;
	axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status;
	const unsigned timeout_ms = 2000;
	acs_apbm_api api;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED || magazine == 0)
		magazine = own_magazine();

	char device[256] = {0};
	//if (comport == acs_apbm::NO_COM)	strcpy(device, "NO_COM");
	if (comport == acs_apbm::COM2)
		{
		_server_working_set->cs_reader->get_my_slot(slot);
			strcpy(device, DEFAULT_IPMI_DEVICE/*"COM2"*/);
		}
	else if (comport == acs_apbm::COM3) {
		_server_working_set->cs_reader->get_my_gea_slot(slot);	
		strcpy(device, DEFAULT_EXT_IPMI_DEVICE/*"COM3"*/);
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Invoked 'IPMI firmware upgrade' action on AP board identified by: MAGAZINE == 0x%08X, SLOT == %u. Parameters: PATH == '%s', PORT == '%u'", magazine, slot, firmware_image_path, comport);

	// check <slot> and <magazine> parameters
	axe_eth_shelf::ipmi_upgrade_status_t ipmi_upg_status;
	int op_res =0;

	if((op_res = _server_working_set->imm_repository_handler->get_board_ipmi_upgrade_status(ipmi_upg_status, slot, magazine)) < 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot get_board ipmi upgrade status ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_status(..)' failed. op_res == %d !", magazine, slot, op_res);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return acs_apbm::ERR_IMM_GET_ATTRIBUTE;
	}

	if (ipmi_upg_status == axe_eth_shelf::IPMI_UPGRADE_STATUS_ONGOING)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Update ON GOING, deleting request!");
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_OPERATION_ONGOING;
		return acs_apbm::ERR_IPMIUPG_UPGRADE_ALREADY_ONGOING;
	}

	if((comport == acs_apbm::NO_COM) && (slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT ))
	if((ret_val = _check_parameters(slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid AP board parameters. Call '_check_parameters(..)' returned  '%d')", ret_val);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return  (ret_val == SDM_INVALID_SLOT_NUM) ? acs_apbm::ERR_IPMIUPG_INVALID_SLOT : acs_apbm::ERR_IPMIUPG_INVALID_MAGAZINE;
	}

	if (access(firmware_image_path, R_OK) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call access(...) returned '%s' 'IPMI firmware path' not found !",firmware_image_path);
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return acs_apbm::ERR_IPMIUPG_INVALID_FILE_NAME;
	} ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call access(...) ok...'IPMI firmware path' '%s'",firmware_image_path);


    FILE *fs;
    fs = ::fopen(firmware_image_path,"r"); // file to read
    if(fs==0)
    {
    	ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call fopen(...). Cannot open source file '%s'!",firmware_image_path);
    	imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
    	return acs_apbm::ERR_IPMIUPG_INVALID_FILE_NAME;
    }
    ::fclose(fs);

    const char * tmp_name = basename(firmware_image_path);//"/temp_ipmiupgrade.mot";
    ACS_APBM_LOG(LOG_LEVEL_INFO,"Call basename(...). from path: '%s' saved basename '%s'",firmware_image_path, tmp_name);
    const char * tfpt_path = "/data/APZ/data/boot/";
    const char * boot_folder = "boot/";
    char tmp_path[1024] = {0};
    char relative_path[1024] = {0};

    //This is the full path
    ::snprintf(tmp_path,1024,"%s%s", tfpt_path, tmp_name);

    //This is the relative path starting from tftp root
    ::snprintf(relative_path,1024,"%s%s", boot_folder, tmp_name);
    string filenametype,filename;
    filename = string(tmp_name);
    filenametype = filename.substr(filename.size()-4, 4);
    if(filenametype == ".bin") // If filename is in bin format
    {
        if(api.is_gep5_gep7())
	{
	    if ((!_server_working_set->cs_reader->is_cba_system()) && (comport == acs_apbm::NO_COM))
            {
	        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Filename is in bin format!");
		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
		return acs_apbm::ERR_IPMIUPG_INVALID_FILE_NAME;
	    }
	}
	else
	{
	    ACS_APBM_LOG(LOG_LEVEL_ERROR, "Filename is in bin format!");
	    imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
	    return acs_apbm::ERR_IPMIUPG_INVALID_FILE_NAME;
	}
    }
	//start of TR HU79549 - create /data/apz/data/boot folder on AP2
	ACS_CS_API_CommonBasedArchitecture::ApgNumber APtype;
	ACS_CS_API_NetworkElement::getApgNumber(APtype);

	if(APtype == 2)
	{
		ACS_APBM_LOG(LOG_LEVEL_TRACE, "This is AP2, so create /data/apz/data/boot folder for ipmi upgrade");
		if(ACE_OS::mkdir(ACS_APBM_IPMIUPG_BOOT_PATH,ACE_DEFAULT_DIR_PERMS))
		{
			if(errno != EEXIST)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Failed to create '%s' directory, error code of ACE_OS:mkdir() == %d",ACS_APBM_IPMIUPG_BOOT_PATH, errno);
		}
	}
	//end of TR HU79549
    if (access(tmp_path, R_OK) < 0) //file not exist! It will be copied
    {
    	ACS_APBM_LOG(LOG_LEVEL_INFO,"Creation in TFTP area filename: '%s' ", tmp_path);
    	ret_val = fcopy(firmware_image_path,tfpt_path);
    	if (ret_val < 0)
    	{
    		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call fcopy(...). Cannot copy file '%s'!",firmware_image_path);
    		imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
    		return IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
    	}
    } else
    	ACS_APBM_LOG(LOG_LEVEL_INFO,"File in TFTP area already exists: '%s' ", tmp_path);
	//if(!local_port || !*local_port){//SNMP
    if(comport == acs_apbm::NO_COM){
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Selected impi_upg via SNMP!");
		// update AP board info about IPMI upgrade status
		//TODO -"***********************CORREGGI upgrade_ip4 ********************"
		const char * upgrade_ip4 = ACS_APBM_CLUSTER_IP2;

		if(APtype == 2)
		{
			upgrade_ip4 = ACS_APBM_CLUSTER_IP4;
		}
		if((ret_val =  _server_working_set->snmp_manager->set_board_ipmi_upgrade(relative_path, upgrade_ip4, slot, &timeout_ms)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot set AP board IPMI status. Call 'set_board_ipmi_upgrade(..) failed ! call_result == %d", call_result);
			if((ret_val = update_ap_board_ipmi_upgrade_status(axe_eth_shelf::IPMI_UPGRADE_STATUS_FAIL,true,slot,magazine)) < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'update_ap_board_ipmi_upgrade_status(..)' call result == '%d'", ret_val);
			}
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return acs_apbm::ERR_IPMIUPG_SNMP_ERROR;
		}
		if((ret_val = _server_working_set->snmp_manager->get_board_ipmi_upgrade_status(ipmi_upgrade_status, slot, &timeout_ms)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'get_board_ipmi_upgrade_status(..)' failed! call result == '%d' ", ret_val);
			if((call_result = update_ap_board_ipmi_upgrade_status(axe_eth_shelf::IPMI_UPGRADE_STATUS_FAIL,true,slot,magazine)) < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'update_ap_board_ipmi_upgrade_status(..)' call result == '%d'", ret_val);
			}
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return  acs_apbm::ERR_IPMIUPG_SNMP_ERROR;
		}
		if((ret_val = update_ap_board_ipmi_upgrade_status(ipmi_upgrade_status,true,slot,magazine)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'update_ap_board_ipmi_upgrade_status(..)' call result == '%d'", ret_val);
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return  acs_apbm::ERR_IPMIUPG_SNMP_ERROR;
		}

   }else{ //COM

		// TODO  UNCOMMENT
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Selected impi_upg via COM port!");
		//ACS_APBM_LOG(LOG_LEVEL_WARN,"WARNING: %s PORT NOT YET SUPPORTED!\n", device);

		acs_apbm_ipmiupgradehandler ipmi;
		int result=0;
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Initialize ipmi port on device %s, with file %s", (comport == acs_apbm::COM2)  ? DEFAULT_IPMI_DEVICE : DEFAULT_EXT_IPMI_DEVICE, tmp_path);
		if( (result = ipmi.initialize(device, tmp_path )) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR,"Initialize failed on port %s, with file %s - result = %d", (comport == acs_apbm::COM2)  ? DEFAULT_IPMI_DEVICE : DEFAULT_EXT_IPMI_DEVICE, tmp_path, result);
			return acs_apbm::ERR_IPMIUPG_UPGRADE_ALREADY_ONGOING;
		}	
		ACS_APBM_LOG(LOG_LEVEL_INFO,"update status of ipmi ");
		if((call_result = update_ap_board_ipmi_upgrade_status(axe_eth_shelf::IPMI_UPGRADE_STATUS_ONGOING, true, slot, magazine)) < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'update_ap_board_ipmi_upgrade_status(..)' call result == '%d'", ret_val);

		ACS_APBM_LOG(LOG_LEVEL_INFO,"Execute ipmi upgrade command!");
	
		if((result = ipmi.execute()) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error in execute ipmi upgrade...ipmi.execute(.. Failed! - result =%d", result);
			ipmi.close();
			if((call_result = update_ap_board_ipmi_upgrade_status(axe_eth_shelf::IPMI_UPGRADE_STATUS_FAIL, true, slot, magazine)) < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'update_ap_board_ipmi_upgrade_status(..)' call result == '%d'", ret_val);
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return acs_apbm::ERR_GENERIC_ERROR;
		}
		//update last update time
		if((call_result = update_ap_board_ipmi_upgrade_status(axe_eth_shelf::IPMI_UPGRADE_STATUS_READY, true, slot, magazine)) < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'update_ap_board_ipmi_upgrade_status(..)' call result == '%d'", ret_val);

		call_result = update_ap_board_ipmi_upgrade_last_time(true, slot, magazine);
		if(call_result < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error updating IPMI last time attribute for the specified AP board (slot == %d  -  magazine == 0x%08X)", slot, magazine);
			imm_error_code = IMM_ACTION_EXIT_CODE_ERROR_EXECUTING;
			return acs_apbm::ERR_GENERIC_ERROR;
		}

		ACS_APBM_LOG(LOG_LEVEL_INFO,"Upgrade Completed! Closing resource");
		ipmi.close();

//		int pipefd[2];
//		pipe(pipefd);
//		char tmp_cmd[1024] = {0};
//		const char * cmd = "eri-ipmitool fupg";
//
//		::snprintf(tmp_cmd, 1024, "%s%s", cmd, tmp_path);
//		if (fork() == 0)
//		{
//		    close(pipefd[0]);    // close reading end in the child
//
//		    uid_t my_uid = getuid();
//		    setuid (0);
//
//		    dup2(pipefd[1], STDOUT_FILENO);  // send stdout to the pipe
//		    dup2(pipefd[1], STDERR_FILENO);  // send stderr to the pipe
//
//		    close(pipefd[1]);    // this descriptor is no longer needed
//
//		    //using eri-ipmitool
//		    execlp("sh", "sh", "-c", tmp_cmd, NULL);
//		    setuid( my_uid );
//		}
//		else
//		{
//		    // parent
//		    char buffer[256] = {0};
//		    char tmp[32] = {0};
//
//		    close(pipefd[1]);  // close the write end of the pipe in the parent
//
//		    while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
//		    {
//		       ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%s", buffer);
//		       if(::sscanf(buffer, "%s %*s", tmp) && (strcmp(tmp, "ERROR:") == 0))
//		         {
//		           ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in request_ipmi_upgrade_on_ap_board(...) during execution of"
//		                                         "eri-ipmitool fupg %s command", tmp_cmd);
//		           return SDM_IPMI_ERROR;
//		         }
//		    }
//		}
	}

	return ret_val;
}


int acs_apbm_shelvesdatamanager::get_other_ap_board_bios_version( char (& bios_product_name) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME], char (& bios_product_version) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_VERSION ] )
{
	int call_result = 0;
	ACS_DSD_Client client;
	ACS_DSD_Node other_node;
	acs_apbm_boardsessionhandler session_handler;

	// get info about other AP node
	call_result = client.get_partner_node(other_node);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "cannot get info concerning other AP node. Call 'ACS_DSD_Client;;get_parter_node()' failed ! error_code == < %d >, error_description == < %s >)", call_result, client.last_error_text());
		return SDM_ERROR;
	}

	// try to connect to other AP node
	ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to connect to APBM Server running on other AP node ...");
	call_result = client.connect(session_handler.peer(), ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID, other_node.system_id, other_node.node_name, 1000);
	if(call_result == 0)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "successfully connected to other AP node. CONN_ID == < %u >", session_handler.getID());
	else if (call_result > 0)
		ACS_APBM_LOG(LOG_LEVEL_WARN, "successfully connected (with WARNINGS) to other AP node. error_code == < %d >, error_description == < %s >. CONN_ID == < %u >", call_result, client.last_error_text(), session_handler.getID());
	else /* call_result < 0 */
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "unable to connect to other AP node. error_code == < %d >, error_description == < %s >", call_result, client.last_error_text());
		return SDM_CONNECT_ERROR;
	}

	// prepare request primitive
	acs_apbm_primitive request_prim(session_handler.peer());
	call_result = request_prim.make_primitive(acs_apbm::PCP_GET_BIOS_VERSION_REQUEST_ID, 1);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error preparing GET_BIOS_VERSION request primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
		return SDM_ERROR;
	}

	// send primitive to other APBM Server
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send primitive: PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u", session_handler.getID(), acs_apbm::PCP_GET_BIOS_VERSION_REQUEST_ID, 1);
	call_result = session_handler.sendPrimitive(request_prim);
	if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
		return SDM_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID == < %u >. GET_BIOS_VERSION request primitive successfully sent", session_handler.getID());

	// now wait for response
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Waiting for response primitive ...", session_handler.getID());
	acs_apbm_primitive response_prim(session_handler.peer());
	call_result = session_handler.recvPrimitive(response_prim, 500/*acs_apbm_boardsessionhandler::RECEIVE_PRIMITIVE_TIMEOUT*/);
	if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error receiving response primitive. error_code == < %d >", session_handler.getID(), call_result);
		return SDM_RECEIVE_ERROR;
	}

	// check if we received the expected response, that is: a GET_BIOS_VERSION response primitive
	int primitive_id = response_prim.primitive_id();
	uint primitive_version = response_prim.primitive_version();
	if(primitive_id != acs_apbm::PCP_GET_BIOS_VERSION_REPLY_ID)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Protocol Error ! Received an unexpected primitive (primitive_id == %d, primitive_version == %u)", session_handler.getID(), primitive_id, primitive_version);
		return SDM_UNEXPECTED_RESPONSE_ERROR;
	}

	// reconstruct the GET_BIOS_VERSION response primitive
	int error_code;
	char received_product_name[acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME] = {0};
	char received_product_version[acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_VERSION] = {0};
	call_result = response_prim.unpack_primitive(primitive_id, primitive_version, & error_code, received_product_name, received_product_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error unpacking GET_BIOS_VERSION response primitive. Call 'acs_apbm_primitivedatahandler<>::unpack_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
		return SDM_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received response primitive. PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u, ERROR_CODE == %d, PRODUCT_NAME == %s, PRODUCT_VERSION == %s", session_handler.getID(), primitive_id, primitive_version, error_code, received_product_name, received_product_version);

	if(error_code != acs_apbm::PCP_ERROR_CODE_SUCCESS)
		return SDM_DATA_UNAVAILABLE;

	// copy the BIOS version info into output parameters
	strncpy(bios_product_name, received_product_name, sizeof(bios_product_name));
	strncpy(bios_product_version, received_product_version, sizeof(bios_product_version));

	return SDM_OK;
}


int acs_apbm_shelvesdatamanager::get_other_ap_board_disks_status(unsigned & other_ap_board_disks_status )
{
	int call_result = 0;
	ACS_DSD_Client client;
	ACS_DSD_Node other_node;
	acs_apbm_boardsessionhandler session_handler;

	// get info about other AP node
	call_result = client.get_partner_node(other_node);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "cannot get info concerning other AP node. Call 'ACS_DSD_Client;;get_parter_node()' failed ! error_code == < %d >, error_description == < %s >)", call_result, client.last_error_text());
		return SDM_ERROR;
	}

	// try to connect to other AP node
	ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to connect to APBM Server running on other AP node ...");
	call_result = client.connect(session_handler.peer(), ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID, other_node.system_id, other_node.node_name, 2000);
	if(call_result == 0)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "successfully connected to other AP node. CONN_ID == < %u >", session_handler.getID());
	else if (call_result > 0)
		ACS_APBM_LOG(LOG_LEVEL_WARN, "successfully connected (with WARNINGS) to other AP node. error_code == < %d >, error_description == < %s >. CONN_ID == < %u >", call_result, client.last_error_text(), session_handler.getID());
	else /* call_result < 0 */
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "unable to connect to other AP node. error_code == < %d >, error_description == < %s >", call_result, client.last_error_text());
		return SDM_CONNECT_ERROR;
	}

	// prepare request primitive
	acs_apbm_primitive request_prim(session_handler.peer());
	call_result = request_prim.make_primitive(acs_apbm::PCP_GET_DISK_STATUS_REQUEST_ID, 1);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error preparing GET_DISK_STATUS request primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
		return SDM_ERROR;
	}

	// send primitive to other APBM Server
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send primitive: PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u", session_handler.getID(), acs_apbm::PCP_GET_DISK_STATUS_REQUEST_ID, 1);
	call_result = session_handler.sendPrimitive(request_prim);
	if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
		return SDM_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID == < %u >. GET_DISK_STATUS request primitive successfully sent", session_handler.getID());

	// now wait for response
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Waiting for response primitive ...", session_handler.getID());
	acs_apbm_primitive response_prim(session_handler.peer());
	call_result = session_handler.recvPrimitive(response_prim, acs_apbm_boardsessionhandler::RECEIVE_PRIMITIVE_TIMEOUT);
	if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error receiving response primitive. error_code == < %d >", session_handler.getID(), call_result);
		return SDM_RECEIVE_ERROR;
	}

	// check if we received the expected response, that is: a GET_DISK_STATUS response primitive
	int primitive_id = response_prim.primitive_id();
	uint primitive_version = response_prim.primitive_version();
	if(primitive_id != acs_apbm::PCP_GET_DISK_STATUS_REPLY_ID)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Protocol Error ! Received an unexpected primitive (primitive_id == %d, primitive_version == %u)", session_handler.getID(), primitive_id, primitive_version);
		return SDM_UNEXPECTED_RESPONSE_ERROR;
	}

	// reconstruct the GET_BIOS_VERSION response primitive
	int error_code;
	uint8_t received_disks_status;
	call_result = response_prim.unpack_primitive(primitive_id, primitive_version, & error_code, & received_disks_status);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error unpacking GET_DISK_STATUS response primitive. Call 'acs_apbm_primitivedatahandler<>::unpack_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
		return SDM_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received response primitive. PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u, ERROR_CODE == %d, DISK_STATUS == %u", session_handler.getID(), primitive_id, primitive_version, error_code, received_disks_status);

	if(error_code != acs_apbm::PCP_ERROR_CODE_SUCCESS)
		return SDM_DATA_UNAVAILABLE;

	// copy DISK status into output parameter
	other_ap_board_disks_status = received_disks_status;

	return SDM_OK;
}

int acs_apbm_shelvesdatamanager::set_other_ap_board_network_interface_card_info(
    const int error_code,
    const axe_eth_shelf::nic_status_t nic_status,
    const char nic_name [acs_apbm::PCP_FIELD_SIZE_NIC_NAME],
    const char nic_ipv4_address [acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS],
    const char nic_mac_address [acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS])
{
        int call_result = 0;
        ACS_DSD_Client client;
        ACS_DSD_Node other_node;
        acs_apbm_boardsessionhandler session_handler;

        // get info about other AP node
        call_result = client.get_partner_node(other_node);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "cannot get info concerning other AP node. Call 'ACS_DSD_Client;;get_parter_node()' failed ! error_code == < %d >, error_description == < %s >)", call_result, client.last_error_text());
                return SDM_ERROR;
        }

        // try to connect to other AP node
        ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to connect to APBM Server running on other AP node ...");
        call_result = client.connect(session_handler.peer(), ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID, other_node.system_id, other_node.node_name, 2000);
        if(call_result == 0)
                ACS_APBM_LOG(LOG_LEVEL_INFO, "successfully connected to other AP node. CONN_ID == < %u >", session_handler.getID());
        else if (call_result > 0)
                ACS_APBM_LOG(LOG_LEVEL_WARN, "successfully connected (with WARNINGS) to other AP node. error_code == < %d >, error_description == < %s >. CONN_ID == < %u >", call_result, client.last_error_text(), session_handler.getID());
        else /* call_result < 0 */
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "unable to connect to other AP node. error_code == < %d >, error_description == < %s >", call_result, client.last_error_text());
                return SDM_CONNECT_ERROR;
        }

        // prepare request primitive
        acs_apbm_primitive request_prim(session_handler.peer());
        call_result = request_prim.make_primitive(acs_apbm::PCP_SET_NIC_INFO_REQUEST_ID, 1, error_code, nic_status, nic_name, nic_ipv4_address, nic_mac_address);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error preparing SET_NIC_INFO_REQUEST request primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
                return SDM_ERROR;
        }

        // send primitive to other APBM Server
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send primitive: PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u", session_handler.getID(), acs_apbm::PCP_SET_NIC_INFO_REQUEST_ID, 1);
        call_result = session_handler.sendPrimitive(request_prim);
        if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
                return SDM_SEND_ERROR;

        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID == < %u >. SET_NIC_INFO request primitive successfully sent", session_handler.getID());

        // now wait for response
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Waiting for response primitive ...", session_handler.getID());
        acs_apbm_primitive response_prim(session_handler.peer());
        call_result = session_handler.recvPrimitive(response_prim, acs_apbm_boardsessionhandler::RECEIVE_PRIMITIVE_TIMEOUT);
        if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error receiving response primitive. error_code == < %d >", session_handler.getID(), call_result);
                return SDM_RECEIVE_ERROR;
        }

        // check if we received the expected response, that is: a SET_NIC_INFO_REPLY response primitive
        int primitive_id = response_prim.primitive_id();
        uint primitive_version = response_prim.primitive_version();
        if(primitive_id != acs_apbm::PCP_SET_NIC_INFO_REPLY_ID)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Protocol Error ! Received an unexpected primitive (primitive_id == %d, primitive_version == %u)", session_handler.getID(), primitive_id, primitive_version);
                return SDM_UNEXPECTED_RESPONSE_ERROR;
        }

        // reconstruct the SET_NIC_INFO_REQUEST_ID response primitive
        call_result = response_prim.unpack_primitive(primitive_id, primitive_version, & error_code);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error unpacking SET_NIC_INFO response primitive. Call 'acs_apbm_primitivedatahandler<>::unpack_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
                return SDM_ERROR;
        }

        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received response primitive. PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u, ERROR_CODE == %d",
                                      session_handler.getID(), primitive_id, primitive_version, error_code);

        if(error_code != acs_apbm::PCP_ERROR_CODE_SUCCESS)
                return SDM_DATA_UNAVAILABLE;

        return SDM_OK;
}
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::get_other_ap_board_network_interface_card_status(const axe_eth_shelf::nic_type_t type, axe_eth_shelf::nic_status_t & nic_status){

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	uint32_t magazine = own_magazine();
	int32_t other_slot;
	_server_working_set->cs_reader->get_my_partner_slot(other_slot);

	// check parameters
	if((call_result = _check_parameters(other_slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, other_slot, magazine);

	if(call_result == SDM_OK)
	{
		switch(type){
		case axe_eth_shelf::ETH_0:
			nic_status = p_apboardinfo->nic_0;
			break;
		case axe_eth_shelf::ETH_1:
			nic_status = p_apboardinfo->nic_1;
			break;
		case axe_eth_shelf::ETH_2:
			nic_status = p_apboardinfo->nic_2;
			break;
		case axe_eth_shelf::ETH_3:
			nic_status = p_apboardinfo->nic_3;
			break;
		case axe_eth_shelf::ETH_4:
			nic_status = p_apboardinfo->nic_4;
			break;
		case axe_eth_shelf::ETH_5:
			nic_status = p_apboardinfo->nic_5;
			break;
		case axe_eth_shelf::ETH_6:
			nic_status = p_apboardinfo->nic_6;
			break;
		case axe_eth_shelf::BOND_1:
			nic_status = p_apboardinfo->bond_1;
			break;
		case axe_eth_shelf::ETH_7:
			nic_status = p_apboardinfo->nic_7;
			break;
		case axe_eth_shelf::ETH_8:
			nic_status = p_apboardinfo->nic_8;
			break;

		default:
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Type not recognized");
			break;
		}

		//check to verify other services state
		ACS_DSD_Client client;
		ACS_DSD_Node other_node;
		acs_apbm_boardsessionhandler session_handler;

		// get info about other AP node
		int conn_result = 0;
		conn_result = client.get_partner_node(other_node);
		if(conn_result < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "cannot get info concerning other AP node. It will be supposed to have not updated info."
					"Call 'ACS_DSD_Client;;get_parter_node()' failed ! error_code == < %d >, error_description == < %s >)", call_result, client.last_error_text());
			nic_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
			update_ap_board_default_nic_values(other_slot, magazine);
			return SDM_ERROR;
		}

		// try to connect to other AP node
		ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to connect to APBM Server running on other AP node ...");
		conn_result = client.connect(session_handler.peer(), ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID, other_node.system_id, other_node.node_name, 1000);
		if(conn_result == 0)
			ACS_APBM_LOG(LOG_LEVEL_INFO, "successfully connected to other AP node. CONN_ID == < %u >", session_handler.getID());
		else if (conn_result > 0)
			ACS_APBM_LOG(LOG_LEVEL_WARN, "successfully connected (with WARNINGS) to other AP node. error_code == < %d >, error_description == < %s >. CONN_ID == < %u >", call_result, client.last_error_text(), session_handler.getID());
		else /* call_result < 0 */
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "unable to connect to other AP node. "
					"It will be supposed to have not updated info. "
					"Error_code == < %d >, error_description == < %s >", call_result, client.last_error_text());
			nic_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
			//resetting nics info
			update_ap_board_default_nic_values(other_slot, magazine);
			return SDM_CONNECT_ERROR;
		}

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "p_apboardinfo->nic_%d status: %d", type, nic_status);

	}
	return call_result;

}

//Redesign as per TR-HS30773
int acs_apbm_shelvesdatamanager::get_other_ap_board_thumbdrive_status(axe_eth_shelf::thumbdrive_status_t & other_ap_board_thumbdrive_status )
{
        int call_result = 0;
        ACS_DSD_Client client;
        ACS_DSD_Node other_node;
        acs_apbm_boardsessionhandler session_handler;

        // get info about other AP node
        call_result = client.get_partner_node(other_node);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "cannot get info concerning other AP node. Call 'ACS_DSD_Client;;get_parter_node()' failed ! error_code == < %d >, error_description == < %s >)", call_result, client.last_error_text());
                return SDM_ERROR;
        }

        // try to connect to other AP node
        ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to connect to APBM Server running on other AP node ...");
        call_result = client.connect(session_handler.peer(), ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID, other_node.system_id, other_node.node_name, 2000);
        if(call_result == 0)
                ACS_APBM_LOG(LOG_LEVEL_INFO, "successfully connected to other AP node. CONN_ID == < %u >", session_handler.getID());
        else if (call_result > 0)
                ACS_APBM_LOG(LOG_LEVEL_WARN, "successfully connected (with WARNINGS) to other AP node. error_code == < %d >, error_description == < %s >. CONN_ID == < %u >", call_result, client.last_error_text(), session_handler.getID());
        else /* call_result < 0 */
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "unable to connect to other AP node. error_code == < %d >, error_description == < %s >", call_result, client.last_error_text());
                return SDM_CONNECT_ERROR;
        }

        // prepare request primitive
        acs_apbm_primitive request_prim(session_handler.peer());
        call_result = request_prim.make_primitive(acs_apbm::PCP_GET_THUMBDRIVE_STATUS_REQUEST_ID, 1);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error preparing GET_THUMBDRIVE_STATUS request primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
                return SDM_ERROR;
        }
// send primitive to other APBM Server
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send primitive: PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u", session_handler.getID(), acs_apbm::PCP_GET_THUMBDRIVE_STATUS_REQUEST_ID, 1);
        call_result = session_handler.sendPrimitive(request_prim);
        if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
                return SDM_SEND_ERROR;

        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID == < %u >. GET_THUMBDRIVE_STATUS request primitive successfully sent", session_handler.getID());

        // now wait for response
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Waiting for response primitive ...", session_handler.getID());
        acs_apbm_primitive response_prim(session_handler.peer());
        call_result = session_handler.recvPrimitive(response_prim, acs_apbm_boardsessionhandler::RECEIVE_PRIMITIVE_TIMEOUT);
        if(call_result != acs_apbm_boardsessionhandler::ACS_APBM_BSH_SUCCESS)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error receiving response primitive. error_code == < %d >", session_handler.getID(), call_result);
                return SDM_RECEIVE_ERROR;
        }

        // check if we received the expected response, that is: a GET_THUMBDRIVE_STATUS response primitive
        int primitive_id = response_prim.primitive_id();
        uint primitive_version = response_prim.primitive_version();
        if(primitive_id != acs_apbm::PCP_GET_THUMBDRIVE_STATUS_REPLY_ID)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Protocol Error ! Received an unexpected primitive (primitive_id == %d, primitive_version == %u)", session_handler.getID(), primitive_id, primitive_version);
                return SDM_UNEXPECTED_RESPONSE_ERROR;
        }

        int error_code;
        axe_eth_shelf::thumbdrive_status_t received_thumbdrive_status;
        call_result = response_prim.unpack_primitive(primitive_id, primitive_version, & error_code, & received_thumbdrive_status);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID == < %u >. Error unpacking GET_THUMBDRIVE_STATUS response primitive. Call 'acs_apbm_primitivedatahandler<>::unpack_primitive()' failed ! call_result == %d", session_handler.getID(), call_result);
                return SDM_ERROR;
        }

	 ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received response primitive. PRIMITIVE_ID == %d, PRIMITIVE_VERSION == %u, ERROR_CODE == %d, THUMBDRIVE_STATUS == %u", session_handler.getID(), primitive_id, primitive_version, error_code, received_thumbdrive_status);

        if(error_code != acs_apbm::PCP_ERROR_CODE_SUCCESS)
                return SDM_DATA_UNAVAILABLE;

        other_ap_board_thumbdrive_status = received_thumbdrive_status;

        return SDM_OK;
}
//Redesign as per TR-HS30773

int acs_apbm_shelvesdatamanager::retrieve_nic_type_by_name(axe_eth_shelf::nic_name_t & type, const char * name){

	if(strcmp(name,"eth0") == 0)
		type = axe_eth_shelf::IMM_NIC_0;
	else if(strcmp(name,"eth1") == 0)
		type = axe_eth_shelf::IMM_NIC_1;
	else if(strcmp(name,"eth2") == 0)
		type = axe_eth_shelf::IMM_NIC_2;
	else if(strcmp(name,"eth3") == 0)
		type = axe_eth_shelf::IMM_NIC_3;
	else if(strcmp(name,"eth4") == 0)
		type = axe_eth_shelf::IMM_NIC_4;
        //ETH5 and ETH6 SUPPORT
	else if(strcmp(name,"eth5") == 0)
		type = axe_eth_shelf::IMM_NIC_5;
	else if(strcmp(name,"eth6") == 0)
		type = axe_eth_shelf::IMM_NIC_6;
	else if(strcmp(name,"eth7") == 0)
		type = axe_eth_shelf::IMM_NIC_7;
	else if(strcmp(name,"eth8") == 0)
		type = axe_eth_shelf::IMM_NIC_8;
	else
		type = axe_eth_shelf::IMM_NIC_UNDEFINED;
	return 0;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::_check_parameters(int32_t slot, uint32_t magazine) const
{
	// check <slot> parameter
	if (slot < MIN_SLOT_NUM ||  slot > MAX_SLOT_NUM)
		return SDM_INVALID_SLOT_NUM;

	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		return SDM_INVALID_MAGAZINE_NUM;

	return SDM_OK;
}
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::_check_disks_raid_status(int32_t slot, uint32_t magazine)
{
        sdm_constant ret_val = SDM_OK;
        std::list<int32_t> slots;
        int32_t other_disk_slot = -1;
	int32_t own_disk_slot = -1;

        // search for DISK boards slot numbers
        ret_val = search_ap_boards_by_fbn(slots, ACS_CS_API_HWC_NS::FBN_Disk, magazine);
        if(ret_val != SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'search_ap_boards_by_fdn' failed ! call_result == %d", ret_val);
                return SDM_ERROR;
        }
	slots.sort();
        // iterate on the 'slot number list' until we found the other DISK slot number
	for(std::list<int32_t>::const_iterator it = slots.begin(); it != slots.end(); ++it)
	{
		if(*it != slot)
		{
			other_disk_slot = *it;
			//  break;
		}
		else{
			own_disk_slot = *it;
		}
	}

	if(other_disk_slot == -1)
	{
		// can't retrieve slot number of the other DISK AP board
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve the slot of the other DISK AP board");
                return SDM_ERROR;
        }

        // get info about other DISK AP board
        acs_apbm_apboardinfo other_disk_info;
        ret_val = get_ap_board_info(other_disk_info, other_disk_slot, magazine);
        if(ret_val != SDM_OK)
        {
                // an error occurred retrieving other DISK status
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve info about other DISK AP board. Call 'acs_apbm_shelvesdatamanager::get_ap_board_info(..)' failed ! call_result == %d", ret_val);
                return SDM_ERROR;
        }

        // be sure that other DISK AP board state is 'working'
        if(other_disk_info.board_status != axe_eth_shelf::BOARD_STATUS_WORKING)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Last working DISK AP board cannot be blocked ! The state of other DISK AP board is '%d'", other_disk_info.board_status);
                return SDM_OP_NOT_PERMITTED;
        }

        // now check RAID status of other DISK AP board ( must be 'ACTIVE' or 'UNDEFINED' to proceed )
        /*if((other_disk_info.raid_disk_status != axe_eth_shelf::RAID_STATUS_ACTIVE)  && (other_disk_info.raid_disk_status != axe_eth_shelf::RAID_STATUS_UNDEFINED))
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "RAID status of other DISK AP board is not OK ! (other) raid_disk_status == '%d'", other_disk_info.raid_disk_status);
                return SDM_OP_NOT_PERMITTED;
        }*/
	 acs_apbm_apboardinfo own_disk_info;
        ret_val = get_ap_board_info(own_disk_info, own_disk_slot, magazine);

	if(ret_val != SDM_OK)
	{
                // an error occurred retrieving own DISK status
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve info about own DISK AP board. Call 'acs_apbm_shelvesdatamanager::get_ap_board_info(..)' failed ! call_result == %d", ret_val);
		return SDM_ERROR;
	}

        // now check RAID status of own and other DISK AP board ( must be 'ACTIVE' or 'UNDEFINED' to proceed )
	if(other_disk_info.raid_disk_status == axe_eth_shelf::RAID_STATUS_NOT_ACTIVE)
	{
		if (own_disk_info.raid_disk_status == axe_eth_shelf::RAID_STATUS_NOT_ACTIVE)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "RAID status of both AP board DISK'S are not OK ! (other) raid_disk_status == '%d', raid_disk_status1 == '%d' ", own_disk_info.raid_disk_status, other_disk_info.raid_disk_status);
			return SDM_RAID_ERROR;
		}
		else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "RAID status of other DISK AP board is not OK ! (other) raid_disk_status == '%d'", other_disk_info.raid_disk_status);
			return SDM_OP_NOT_PERMITTED;
		}
	}
	return SDM_OK;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::_check_disks_drbd_status(int32_t slot, uint32_t magazine)
{
        sdm_constant ret_val = SDM_OK;
        std::list<int32_t> slots;
        int32_t other_disk_slot = -1;

        // search for DISK boards slot numbers
        ret_val = search_ap_boards_by_fbn(slots, ACS_CS_API_HWC_NS::FBN_APUB, magazine);
        if(ret_val != SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'search_ap_boards_by_fdn' failed ! call_result == %d", ret_val);
                return SDM_ERROR;
        }

        // iterate on the 'slot number list' until we found the other DISK slot number
        for(std::list<int32_t>::const_iterator it = slots.begin(); it != slots.end(); ++it)
        {
                if(*it != slot)
                {
                        other_disk_slot = *it;
                        break;
                }
        }

        if(other_disk_slot == -1)
        {
                // can't retrieve slot number of the other DISK AP board
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve the slot of the other DISK AP board");
                return SDM_ERROR;
        }

        // get info about other DISK AP board
        acs_apbm_apboardinfo other_disk_info;
        ret_val = get_ap_board_info(other_disk_info, slot, magazine);
        if(ret_val != SDM_OK)
        {
                // an error occurred retrieving other DISK status
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve info about other DISK AP board. Call 'acs_apbm_shelvesdatamanager::get_ap_board_info(..)' failed ! call_result == %d", ret_val);
                return SDM_ERROR;
        }
 // be sure that other DISK AP board state is 'working'
        if(other_disk_info.board_status != axe_eth_shelf::BOARD_STATUS_WORKING)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Last working DISK AP board cannot be blocked ! The state of other DISK AP board is '%d'", other_disk_info.board_status);
                return SDM_OP_NOT_PERMITTED;
        }

        // now check DRBD status of other DISK AP board ( must be 'ACTIVE' or 'UNDEFINED' to proceed )
        if((other_disk_info.drbd_disk_status != (axe_eth_shelf::DRBD_STATUS_CONNECTED || axe_eth_shelf::DRBD_STATUS_UPTODATE)) && (other_disk_info.drbd_disk_status != axe_eth_shelf::DRBD_STATUS_UNDEFINED))
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "DRBD status of other DISK AP board is not OK ! (other) drbd_disk_status == '%d'", other_disk_info.drbd_disk_status);
                return SDM_OP_NOT_PERMITTED;
        }
        return SDM_OK;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::_retrieve_ap_board_info_ptr(acs_apbm_apboardinfo * & apboardinfo_ptr, int32_t slot, uint32_t magazine) const
{
	// search magazine
	MapMagazineToAPBoards::const_iterator it = _apboards_map.find(magazine);
	if(it == _apboards_map.end())
		return SDM_MAGAZINE_NOT_FOUND;

	// get the AP Boards collection associated to the magazine
	APBoardsCollection * const & p_apboards_collection = (*it).second;
	if(!p_apboards_collection)
		return SDM_INVALID_MAGAZINE_DATA;

	// get a pointer to the AP board object having slot number == <slot>
	acs_apbm_apboardinfo * p_apboard_info = p_apboards_collection->at(slot);
	if(!p_apboard_info)
		return SDM_APBOARD_NOT_FOUND;	// AP Board info not present

	// fill in the output parameter
	apboardinfo_ptr = p_apboard_info;
	return SDM_OK;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::_exec_snmp_common_work_to_block_ap_board(int32_t slot, uint32_t magazine)
{
	// set AP board status to "MANUALLY BLOCKED'....
	axe_eth_shelf::status_t old_apboard_status;
	int call_result = set_ap_board_status(axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED, true, slot, magazine, old_apboard_status);
	if(call_result != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting status 'MANUALLY BLOCKED' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", magazine, slot, call_result);
		return SDM_SET_STATE_ERROR;
	}

	// ask SNMP manager to turn on maintenance led on the specified AP board
	int result;
	sdm_constant ret_val = SDM_OK;
	uint16_t apboard_fbn;
	if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, slot, magazine)) != SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
		return ret_val;
	}

	//Check whether we need to set MIA or BLUE LED
	bool has_4_leds = (apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)? is_new_gea_supported: is_4led_supported;

	if(has_4_leds)
	{
		if ((result = _server_working_set->snmp_manager->set_board_BLUEled(magazine, slot, axe_eth_shelf::LED_STATUS_ON)) < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting BLUE led ON for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_BLUEled(..)' failed ! call_result == %d'", magazine, slot, result);
	}
	else
	{
		if ((result = _server_working_set->snmp_manager->set_board_MIAled(magazine, slot, axe_eth_shelf::LED_STATUS_ON)) < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting MIA led ON for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_BLUEled(..)' failed ! call_result == %d'", magazine, slot, result);
	}

	if(result < 0)
	{
		// try to restore previous AP board status
		if((call_result = set_ap_board_status(old_apboard_status, true, slot, magazine)) != SDM_OK)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", old_apboard_status, magazine, slot, call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Successfully restored status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d", old_apboard_status, magazine, slot);

		return SDM_SNMP_ERROR;
	}

	// ask SNMP manager to set POWER OFF on the specified AP board
	int set_power_result = _server_working_set->snmp_manager->set_board_Power(magazine, slot, false);
	if(set_power_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error setting POWER OFF for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_Power(..)' failed ! call_result == %d'", magazine, slot, set_power_result);
		// try to restore Maintenance led

		if(has_4_leds)
		{
			result = _server_working_set->snmp_manager->set_board_BLUEled(magazine, slot, axe_eth_shelf::LED_STATUS_OFF);
			if(result < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error re-setting BLUE led OFF for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_BLUEled(..)' failed ! call_result == %d'", magazine, slot, result);
			else
				ACS_APBM_LOG(LOG_LEVEL_INFO, "BLUE led reset to OFF for AP board identified by MAG == 0x%08X, SLOT == %d",magazine,slot);
		}
		else
		{
			result = _server_working_set->snmp_manager->set_board_MIAled(magazine, slot, axe_eth_shelf::LED_STATUS_OFF);
			if(result < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error re-setting MIA led OFF for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_snmpmanager::set_board_MIAled(..)' failed ! call_result == %d'", magazine, slot, result);
			else
				ACS_APBM_LOG(LOG_LEVEL_INFO, "MIA led reset to OFF for AP board identified by MAG == 0x%08X, SLOT == %d",magazine,slot);
		}

		// try to restore previous AP board status
		if((call_result = set_ap_board_status(old_apboard_status, true, slot, magazine)) != SDM_OK)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", old_apboard_status, magazine, slot, call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error restoring status '%d' for AP board identified by MAG == 0x%08X, SLOT == %d. Details: call 'acs_apbm_shelvesdatamanager::set_ap_board_status(..)' failed ! call_result == %d'", old_apboard_status, magazine, slot, call_result);

		return SDM_SNMP_ERROR;
	}


	return SDM_OK;
}


acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::_request_block_on_disk_board(int32_t slot, uint32_t magazine)
{
	sdm_constant ret_val = SDM_OK;
	if(is_4led_supported)
		ret_val = _check_disks_drbd_status(slot, magazine);
	else
		ret_val = _check_disks_raid_status(slot, magazine);
	if (ret_val != SDM_OK)
	        return ret_val;

	// send appropriate commands to SNMP manager (to block the AP board)
	ret_val = _exec_snmp_common_work_to_block_ap_board(slot, magazine);
	if(ret_val != SDM_OK)
		return ret_val;

	return SDM_OK;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::set_ap_board_status(axe_eth_shelf::status_t apboard_status, bool update_IMM, int32_t slot, uint32_t magazine)
{
	axe_eth_shelf::status_t dummy;

	return set_ap_board_status(apboard_status, update_IMM, slot, (magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED) ? own_magazine(): magazine, dummy);
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::set_ap_board_status(axe_eth_shelf::status_t apboard_status, bool update_IMM, int32_t slot, uint32_t magazine, axe_eth_shelf::status_t & old_apboard_status)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	if(call_result == SDM_OK)
	{
		axe_eth_shelf::status_t tmp = p_apboardinfo->board_status;

		// update AP board status
		p_apboardinfo->board_status = apboard_status;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IN SET_AP_BOARD_STATUS FUNC() :%d ----- and %d",p_apboardinfo->board_status, apboard_status);
		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->modify_ap_board_status_attribute(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'board status' attribute for AP board identified by  ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::modify_ap_board_status_attribute(..)' failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board status
				p_apboardinfo->board_status = tmp;
				return SDM_IMM_ERROR;
			}
			if (apboard_status == axe_eth_shelf::BOARD_STATUS_MISSING)
			{
				int result;
				if( (result = _server_working_set->imm_repository_handler->update_snmp_ap_board_info(p_apboardinfo)) <0 ){
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update result %d",call_result);
					return SDM_IMM_ERROR;
				}
				//NIC INFO - added NIC information to aligned with snmp data
				if ( (result = update_ap_board_default_nic_values(p_apboardinfo->slot_position, p_apboardinfo->magazine)) <0 ){
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update_ap_board_default_nic_values - result %d",call_result);
						return SDM_IMM_ERROR;
					}
				//Temperature information INFORMATION
				if ( (result = update_ap_board_temperature_status(p_apboardinfo->temperature_status, true, p_apboardinfo->slot_position, p_apboardinfo->magazine)) < 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update_temperature_status_on_ap_board_info - result %d",call_result);
						return SDM_IMM_ERROR;
					}
				//Alarm interface status
				if ( (result = update_ap_board_alarm_interface_status(p_apboardinfo->alarm_interface_status, true, p_apboardinfo->slot_position, p_apboardinfo->magazine)) < 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update_ap_board_alarm_interface_status - result %d",call_result);
						return SDM_IMM_ERROR;
					}
				if(is_4led_supported)
				{
					if ( (result = update_ap_board_drbd_status(p_apboardinfo->drbd_disk_status, true, p_apboardinfo->slot_position, p_apboardinfo->magazine)) < 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update_ap_board_drbd_status - result %d",call_result);
						return SDM_IMM_ERROR;
					}
				}
				else
				{
					if ( (result = update_ap_board_raid_status(p_apboardinfo->raid_disk_status, true, p_apboardinfo->slot_position, p_apboardinfo->magazine)) < 0)
					{
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot update_ap_board_raid_status - result %d",call_result);
						return SDM_IMM_ERROR;
					}
				}
				}
		}

		// return current AP board status
		old_apboard_status = tmp;
	}

	return call_result;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::set_ap_board_LEDS_status(axe_eth_shelf::led_status_t green_led, axe_eth_shelf::led_status_t red_led, axe_eth_shelf::led_status_t mia_led, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::led_status_t old_green_led = p_apboardinfo->green_led;
		axe_eth_shelf::led_status_t old_red_led = p_apboardinfo->red_led;
		axe_eth_shelf::led_status_t old_mia_led = p_apboardinfo->mia_led;

		// update LEDS status
		p_apboardinfo->green_led = green_led;
		p_apboardinfo->red_led = red_led;
		p_apboardinfo->mia_led = mia_led;

		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_led_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'LEDS' attributes for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_led_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->green_led = old_green_led;
				p_apboardinfo->red_led = old_red_led;
				p_apboardinfo->mia_led = old_mia_led;
				return SDM_IMM_ERROR;
			}
		}
	}

	return call_result;
}

//4 LEDs support
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::set_ap_board_LEDS_status(axe_eth_shelf::led_status_t green_led, axe_eth_shelf::led_status_t red_led, axe_eth_shelf::led_status_t mia_led, axe_eth_shelf::led_status_t blue_led, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::led_status_t old_green_led = p_apboardinfo->green_led;
		axe_eth_shelf::led_status_t old_red_led = p_apboardinfo->red_led;
		axe_eth_shelf::led_status_t old_mia_led = p_apboardinfo->mia_led;
		axe_eth_shelf::led_status_t old_blue_led = p_apboardinfo->blue_led;

		// update LEDS status
		p_apboardinfo->green_led = green_led;
		p_apboardinfo->red_led = red_led;
		p_apboardinfo->mia_led = mia_led;
		p_apboardinfo->blue_led = blue_led;

		// update IMM, if requested
		if(update_IMM)
		{
                    switch(red_led)
                    {
                                case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
                                                                        p_apboardinfo->red_led = static_cast<axe_eth_shelf::led_status_t>(2);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
                                                                        p_apboardinfo->red_led = static_cast<axe_eth_shelf::led_status_t>(3);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
                                                                        p_apboardinfo->red_led = static_cast<axe_eth_shelf::led_status_t>(4);
                                                                        break;
                                default:
                                                                        break;
                        }
                        switch(green_led)
                        {
                                case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
                                                                        p_apboardinfo->green_led = static_cast<axe_eth_shelf::led_status_t>(2);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
                                                                        p_apboardinfo->green_led = static_cast<axe_eth_shelf::led_status_t>(3);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
                                                                        p_apboardinfo->green_led = static_cast<axe_eth_shelf::led_status_t>(4);
                                                                        break;
                                default:
                                                                        break;
                        }
                        switch(blue_led)
                        {
                                case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
                                                                        p_apboardinfo->blue_led = static_cast<axe_eth_shelf::led_status_t>(2);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
                                                                        p_apboardinfo->blue_led = static_cast<axe_eth_shelf::led_status_t>(3);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
                                                                        p_apboardinfo->blue_led = static_cast<axe_eth_shelf::led_status_t>(4);
                                                                        break;
                                default:
                                                                        break;
                        }
                        switch(mia_led)
                        {
                                case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
                                                                        p_apboardinfo->mia_led = static_cast<axe_eth_shelf::led_status_t>(2);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
                                                                        p_apboardinfo->mia_led = static_cast<axe_eth_shelf::led_status_t>(3);
                                                                        break;
                                case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
                                                                        p_apboardinfo->mia_led = static_cast<axe_eth_shelf::led_status_t>(4);
                                                                        break;
                                default:
                                                                        break;
                        }

			int op_res = _server_working_set->imm_repository_handler->update_led_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'LEDS' attributes for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_led_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->green_led = old_green_led;
				p_apboardinfo->red_led = old_red_led;
				p_apboardinfo->mia_led = old_mia_led;
				p_apboardinfo->blue_led = old_blue_led;
				return SDM_IMM_ERROR;
			}
		}
	}

	return call_result;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_green_led(axe_eth_shelf::led_status_t green_led, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::led_status_t old_green_led = p_apboardinfo->green_led;

		p_apboardinfo->green_led = green_led;
		// update IMM, if requested
		if(update_IMM)
		{
			if(is_4led_supported || is_new_gea_supported)
			{
				switch(green_led)
				{
				case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
					p_apboardinfo->green_led = static_cast<axe_eth_shelf::led_status_t>(2);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
					p_apboardinfo->green_led = static_cast<axe_eth_shelf::led_status_t>(3);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
					p_apboardinfo->green_led = static_cast<axe_eth_shelf::led_status_t>(4);
					break;
				default:
					break;
				}
			}
			int op_res = _server_working_set->imm_repository_handler->update_green_led_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'green led' attributes for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_green_led_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->green_led = old_green_led;
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_mia_led(axe_eth_shelf::led_status_t mia_led, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::led_status_t old_mia_led = p_apboardinfo->mia_led;

		p_apboardinfo->mia_led = mia_led;
		// update IMM, if requested
		if(update_IMM)
		{
			if(is_4led_supported || is_new_gea_supported)
			{
				switch(mia_led)
				{
				case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
					p_apboardinfo->mia_led = static_cast<axe_eth_shelf::led_status_t>(2);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
					p_apboardinfo->mia_led = static_cast<axe_eth_shelf::led_status_t>(3);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
					p_apboardinfo->mia_led = static_cast<axe_eth_shelf::led_status_t>(4);
					break;
				default:
					break;
				}
			}
			int op_res = _server_working_set->imm_repository_handler->update_mia_led_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'mia led' attributes for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_mia_led_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->mia_led = old_mia_led;
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_red_led(axe_eth_shelf::led_status_t red_led, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::led_status_t old_red_led = p_apboardinfo->red_led;

		p_apboardinfo->red_led = red_led;
		// update IMM, if requested
		if(update_IMM)
		{
			if(is_4led_supported || is_new_gea_supported)
			{
				switch(red_led)
				{
				case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
					p_apboardinfo->red_led = static_cast<axe_eth_shelf::led_status_t>(2);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
					p_apboardinfo->red_led = static_cast<axe_eth_shelf::led_status_t>(3);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
					p_apboardinfo->red_led = static_cast<axe_eth_shelf::led_status_t>(4);
					break;
				default:
					break;
				}
			}
			int op_res = _server_working_set->imm_repository_handler->update_red_led_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'red led' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_red_led_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->red_led = old_red_led;
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

/*******************************************************
* This function is used to update the blue LED 
********************************************************/
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_blue_led(axe_eth_shelf::led_status_t blue_led, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::led_status_t old_blue_led = p_apboardinfo->blue_led;

		p_apboardinfo->blue_led = blue_led;
		// update IMM, if requested
		if(update_IMM)
		{
			if(is_4led_supported || is_new_gea_supported)
			{
				switch(blue_led)
				{
				case axe_eth_shelf::LED_STATUS_NOT_AVAILABLE:
					p_apboardinfo->blue_led = static_cast<axe_eth_shelf::led_status_t>(2);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK:
					p_apboardinfo->blue_led = static_cast<axe_eth_shelf::led_status_t>(3);
					break;
				case axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK:
					p_apboardinfo->blue_led = static_cast<axe_eth_shelf::led_status_t>(4);
					break;
				default:
					break;
				}
			}
			int op_res = _server_working_set->imm_repository_handler->update_blue_led_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'blue led' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_blue_led_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->blue_led = old_blue_led;
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_default_led_values(int32_t slot, uint32_t magazine){

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{	
	ACS_APBM_LOG(LOG_LEVEL_ERROR, "VIrtualized so returning witout updating leds");// to be removed
		return call_result;
	}
	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		int op_res0,op_res1,op_res2,op_res3;
		op_res0 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_FAULT, axe_eth_shelf::LED_COLOR_RED, true);
		op_res1 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_OPERATIONAL, axe_eth_shelf::LED_COLOR_GREEN, true);
		if( is_4led_supported ) // If HW supports four leds.
		{
			op_res2 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_MAINTENANCE, axe_eth_shelf::LED_COLOR_BLUE, true);
			op_res3 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_STATUS, axe_eth_shelf::LED_COLOR_YELLOW, true);
		}
		else   // If HW support three leds.
		{
			op_res2 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_MAINTENANCE, axe_eth_shelf::LED_COLOR_YELLOW, true);
			op_res3 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_STATUS, axe_eth_shelf::LED_COLOR_BLUE, false);
		}
		if((op_res0 < 0) || (op_res1 < 0) || (op_res2 < 0) || (op_res3 < 0))
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'update_default_led_values_on_ap_board_info' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_default_led_values_on_ap_board_info(..) failed. op_res0 == %d op_res1 == %d op_res2 == %d op_res3 == %d !",
					magazine, slot, op_res0, op_res1, op_res2, op_res3);
			// restore previous AP board LEDS status
			return SDM_IMM_ERROR;
		}
	}
	return call_result;
}
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_default_gea_led_values(int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "In update_ap_board_default_gea_led_values function");
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "GEA product number is : %s", p_apboardinfo->product_number);
	// set LEDS status
	if(call_result == SDM_OK)
	{
		int op_res0,op_res1,op_res2,op_res3;
		op_res0 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_FAULT, axe_eth_shelf::LED_COLOR_RED, true);
		op_res1 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_OPERATIONAL, axe_eth_shelf::LED_COLOR_GREEN, true);
		if(is_new_gea_supported) // If HW supports four leds.
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "New GEA value is: %d ", is_new_gea_supported);
			op_res2 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_MAINTENANCE, axe_eth_shelf::LED_COLOR_BLUE, true);
			op_res3 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_STATUS, axe_eth_shelf::LED_COLOR_YELLOW, true);
		}
		else   // If HW support three leds.
		{
			op_res2 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_MAINTENANCE, axe_eth_shelf::LED_COLOR_YELLOW, true);
			op_res3 = _server_working_set->imm_repository_handler->update_default_led_values_on_ap_board_info(p_apboardinfo, axe_eth_shelf::LED_TYPE_STATUS, axe_eth_shelf::LED_COLOR_BLUE, false);
		}
		if((op_res0 < 0) || (op_res1 < 0) || (op_res2 < 0) || (op_res3 < 0))
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'update_default_led_values_on_ap_board_info' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_default_led_values_on_ap_board_info(..) failed. op_res0 == %d op_res1 == %d op_res2 == %d op_res3 == %d !",
					magazine, slot, op_res0, op_res1, op_res2, op_res3);
			// restore previous AP board LEDS status
			return SDM_IMM_ERROR;
		}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_default_nic_values(int32_t slot, uint32_t magazine){

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		return call_result;
	}
	
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();
	
	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	int32_t my_slot;

	if( _server_working_set->cs_reader->get_my_slot(my_slot) != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_INFO,"No needed NIC update on board at slot %d", slot);
		return SDM_ERROR;
	}

	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return SDM_UNKNOWN_APG_SHELF_ARCHITECTURE;
	}

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);
	
	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);
	
	if (call_result != SDM_OK)
		return call_result;
	//Upgrade only for local board
	//set nic structure to default value	
	_server_working_set->imm_repository_handler->update_all_default_nic_name_on_ap_board_info(p_apboardinfo);
	_server_working_set->imm_repository_handler->update_network_interfaces_cards_status_on_ap_board_info(p_apboardinfo);
	_server_working_set->imm_repository_handler->update_all_default_mac_address_on_ap_board_info(p_apboardinfo);
	_server_working_set->imm_repository_handler->update_all_default_ip_address_on_ap_board_info(p_apboardinfo);
	
	if (my_slot != slot)
	return SDM_OK;

	/*IP ADDRESS*/
	/* Walk through linked list, maintaining head pointer so we
			 can free list later */
	char host[NI_MAXHOST]={0};
	axe_eth_shelf::nic_name_t nic_type;
	struct ifaddrs *ifaddr;
	int s, op_res;
	if (getifaddrs(&ifaddr) == -1) {
	    ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call 'getifaddrs' failed!");
	    return SDM_ERROR;
	}

	for (ifaddrs * ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	  {
	    if (ifa->ifa_addr == NULL)
	      continue;

	    int family = ifa->ifa_addr->sa_family;

	    const char * nic_names [9] = {"eth0", "eth1", "eth2", "eth3", "eth4", "eth5", "eth6", "eth7", "eth8"};

	    for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(nic_names); ++i)
	    {
	    	if((env != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) && (i >= 7))
	    		break;

	    	if((env != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) && (is_10G() == false) && (i>=5)) // exit loop when in non-10G env. for eth5,eth6
	    		break;

	        if (!::strcasecmp(ifa->ifa_name, nic_names[i]))
	          {
	            /* Display interface name and family (including symbolic
					 form of the latter for the common families) */

	            ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%s  address family: %d%s \n",
	                ifa->ifa_name, family,
	                (family == AF_PACKET) ? " (AF_PACKET)" :
	                    (family == AF_INET) ?   " (AF_INET)" :
	                        (family == AF_INET6) ?  " (AF_INET6)" : "");

	            //			if (family == AF_INET || family == AF_INET6) {
	            switch(family)
	            {
	            case AF_INET : /*IP ADDRESS*/
	              s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
	                  host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
	              if (s != 0) {
	                  ACS_APBM_LOG(LOG_LEVEL_DEBUG," Call 'getnameinfo' failed: %s\n", gai_strerror(s));
	                  ::freeifaddrs(ifaddr);
	                  return SDM_ERROR;
	              }
	              ACS_APBM_LOG(LOG_LEVEL_DEBUG,"in %s retrieved ip address: < %s >\n", ifa->ifa_name , host);
	              retrieve_nic_type_by_name(nic_type, ifa->ifa_name);
	              op_res = _server_working_set->imm_repository_handler->update_ip_address_on_ap_board_info(nic_type, p_apboardinfo, host);
	              if(op_res < 0)
	                {
	                  ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'ip address' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_bios_boot_run_mode_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
	                  ::freeifaddrs(ifaddr);
	                  return SDM_IMM_ERROR;
	                }
	              break;

	            case AF_PACKET: /*MAC ADDRESS*/
	              char path [1024];
	              ::snprintf(path, (sizeof(path)/sizeof(*(path))), "/sys/class/net/%s/address", ifa->ifa_name);
	              char mac_addr[32]={0};
	              if (FILE * f = ::fopen(path, "r"))  // OK: file exists and was opened successfully
	                {
	                  ::fscanf(f, "%50s", mac_addr);
	                  if(*mac_addr != 0)
	                    ACS_APBM_LOG(LOG_LEVEL_DEBUG, "mac address of %s found: %s\n",ifa->ifa_name, mac_addr);
	                  ::fclose(f);
	                } else  // ERROR: opening file. I report media disconnected
	                  {
	                    ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call 'fopen' failed: cannot open the file '%s'", path);
	                    ::freeifaddrs(ifaddr);
	                    return SDM_ERROR;
	                  }

	              ACS_APBM_LOG(LOG_LEVEL_DEBUG,"in %s retrieved mac address: < %s >\n", ifa->ifa_name , mac_addr);
	              retrieve_nic_type_by_name(nic_type, ifa->ifa_name);

	              op_res = _server_working_set->imm_repository_handler->update_mac_address_on_ap_board_info(nic_type, p_apboardinfo, mac_addr);
	              if(op_res < 0)
	                {
	                  ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'mac address' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_bios_boot_run_mode_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
	                  ::freeifaddrs(ifaddr);
	                  return SDM_IMM_ERROR;
	                }
	              break;
	            }
	          }
	      }
	  }
	_server_working_set->monitoring_service_event_handler->set_nic_status_data(true); //TR HV50762
	::freeifaddrs(ifaddr);
	return SDM_OK;
}


acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_bios_boot_run_mode(axe_eth_shelf::bios_boot_run_mode_t bios_running_mode,bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	
	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{	
		return call_result;
	}
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::bios_boot_run_mode_t old_bios_running_mode = p_apboardinfo->bios_running_mode;

		p_apboardinfo->bios_running_mode = bios_running_mode;
		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_bios_boot_run_mode_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				//char path [1024];
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'boot run mode' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_bios_boot_run_mode_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->bios_running_mode = old_bios_running_mode;
				return SDM_IMM_ERROR;
			}
		}
	}

	return call_result;
}


acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_fatal_event_log(char (& fatal_event_log_info)[acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1], bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	int environment=0;
	_server_working_set->cs_reader->get_environment(environment);
	if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{	
		return call_result;
	}
	
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();	
	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);
	if(call_result == SDM_OK)
	{
		char old_fatal_event_log_info[acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1] ={0};
		::strncpy(old_fatal_event_log_info, p_apboardinfo->fatalEventLogInfo, acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE);

		::strncpy(p_apboardinfo->fatalEventLogInfo, fatal_event_log_info, acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE);
		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_fatal_event_log_info_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'boot run mode' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_bios_boot_run_mode_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				::strncpy(p_apboardinfo->fatalEventLogInfo, old_fatal_event_log_info, acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE);
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_status(axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status, bool update_IMM, int32_t slot, uint32_t magazine){

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT )
	{
		if((call_result = update_ap_board_ipmi_upgrade_status_on_ap(ipmi_upgrade_status, update_IMM, slot, magazine)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for AP board. Call 'update_ap_board_ipmi_upgrade_status_on_ap(..)' call result == '%d'", call_result);
			return  SDM_IMM_ERROR;
		}
	}else if (!_server_working_set->cs_reader->is_cba_system())
	{
		if((call_result = update_ap_board_ipmi_upgrade_status_on_scb(ipmi_upgrade_status, update_IMM, slot, magazine)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot get IMPI upgrade status for SCB board. Call 'update_ap_board_ipmi_upgrade_status_on_scb(..)' call result == '%d'", call_result);
			return  SDM_IMM_ERROR;
		}
	} else
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid Request, check slot position %d or architecture!",slot);


	return  call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_status_on_ap(axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set ipmi upgrade status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		axe_eth_shelf::ipmi_upgrade_status_t old_ipmi_upgrade_status = p_apboardinfo->ipmi_upgrade_status;

		p_apboardinfo->ipmi_upgrade_status = ipmi_upgrade_status;
		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_ipmi_upgrade_result_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'ipmi_upgrade_status' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_bios_boot_run_mode_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				p_apboardinfo->ipmi_upgrade_status = old_ipmi_upgrade_status;
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_status_on_scb(axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status, bool update_IMM, int32_t slot, uint32_t magazine)
{
	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	acs_apbm_apboardinfo board;

	board.magazine = magazine;
	board.slot_position = slot;

	board.ipmi_upgrade_status = ipmi_upgrade_status;

	// update IMM, if requested
	if(update_IMM)
	{
		int op_res = _server_working_set->imm_repository_handler->update_ipmi_upgrade_result_on_scb(&board);
		if(op_res < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'ipmi_upgrade_status' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_ap_board_ipmi_upgrade_status_on_scb(..) failed. op_res == %d !", magazine, slot, op_res);
			// restore previous AP board LEDS status
			return SDM_IMM_ERROR;
		}
	}
	return SDM_OK;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_last_time( bool update_IMM, int32_t slot, uint32_t magazine){

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	if(slot != acs_apbm::BOARD_SCB_LEFT && slot != acs_apbm::BOARD_SCB_RIGHT )
	{
		if((call_result = update_ap_board_ipmi_upgrade_last_time_on_ap(update_IMM, slot, magazine)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update_ap_board_ipmi_upgrade_last_time for AP board. Call 'update_ap_board_ipmi_upgrade_last_time_on_ap(..)' call result == '%d'", call_result);
			return  SDM_IMM_ERROR;
		}
	}else if (!_server_working_set->cs_reader->is_cba_system())
	{
		if((call_result = update_ap_board_ipmi_upgrade_last_time_on_scb(update_IMM, slot, magazine)) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update_ap_board_ipmi_upgrade_last_time for SCB board. Call 'update_ap_board_ipmi_upgrade_last_time_on_scb(..)' call result == '%d'", call_result);
			return  SDM_IMM_ERROR;
		}
	} else
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Invalid Request, check slot position %d or architecture!",slot);

	return  call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_last_time_on_ap( bool update_IMM, int32_t slot, uint32_t magazine){

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set ipmi upgrade status

	char ipmifw_lastupgtime [acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME] = {0};
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime( &rawtime );

  strftime (ipmifw_lastupgtime,acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME, "%Y-%m-%d %X" ,timeinfo);

	if(call_result == SDM_OK)
	{
		// cache current upgrate time status
		char old_ipmifw_lastupgtime [acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME] = {0};
		::strncpy(old_ipmifw_lastupgtime, p_apboardinfo->ipmi_upgrade_last_time, acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME);

		::strncpy(p_apboardinfo->ipmi_upgrade_last_time, ipmifw_lastupgtime, acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME);
		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_ipmi_upgrade_time_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'ipmi_last_time' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_ap_board_ipmi_upgrade_last_time_on_ap(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board LEDS status
				::strncpy(p_apboardinfo->ipmi_upgrade_last_time, old_ipmifw_lastupgtime, acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME);
				return SDM_IMM_ERROR;
			}
			//TODO - DELETE METHOD
		}
	}

	return call_result;

}

acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_ipmi_upgrade_last_time_on_scb( bool update_IMM, int32_t slot, uint32_t magazine){

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	acs_apbm_apboardinfo board;

	board.magazine = magazine;
	board.slot_position = slot;

	// set ipmi upgrade status

	char ipmifw_lastupgtime [acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME] = {0};
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime( &rawtime );

  strftime (ipmifw_lastupgtime,acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME, "%Y-%m-%d %X" ,timeinfo);

	::strncpy(board.ipmi_upgrade_last_time, ipmifw_lastupgtime, acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME);
	// update IMM, if requested
	if(update_IMM)
	{
		int op_res = _server_working_set->imm_repository_handler->update_ipmi_upgrade_time_on_scb(& board);
		if(op_res < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'ipmi_last_time' attribute for SCB board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_ap_board_ipmi_upgrade_last_time_on_scb(..) failed. op_res == %d !", magazine, slot, op_res);
			return SDM_IMM_ERROR;
		}
	}
	return SDM_OK;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_ipmi_data(
		int16_t & ipmi_firmware_type,
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1],
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],
		bool update_IMM,
		int32_t slot,
		uint32_t  magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	/*
	 * * DELETED FUNCTIONALITY
	 */
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current LEDS status
		char old_ipmi_product_number[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1] = {0};
		char old_ipmi_product_revision[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1] = {0};

/*		int16_t old_ipmi_firmware_type = p_apboardinfo->ipmi_firmware_type;
		::strcpy(old_ipmi_product_number, p_apboardinfo->ipmi_product_number);
		::strcpy(old_ipmi_product_revision, p_apboardinfo->ipmi_product_revision);*/

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"old_ipmi_product_number == '%s' - old_ipmi_product_revision == '%s' ", old_ipmi_product_number, old_ipmi_product_revision );

		::strcpy(p_apboardinfo->ipmi_product_number, ipmi_product_number);
		::strcpy(p_apboardinfo->ipmi_product_revision, ipmi_product_revision);
		p_apboardinfo->ipmi_firmware_type = ipmi_firmware_type;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"NEW_ipmi_product_number == '%s' -NEW ipmi_product_revision == '%s' ", p_apboardinfo->ipmi_product_number, p_apboardinfo->ipmi_product_revision );

		// update IMM, if requested
		if(update_IMM)
		{
//	TODO - DELETE METHOD
		}
	}

	return call_result;
}

// update the snmp data of an AP board
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_snmp_data (
		char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
		char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
		char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
		char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
		char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
		char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
		bool update_IMM,
		int32_t slot,
		uint32_t magazine) {
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	/*
	 * * DELETED FUNCTIONALITY
	 */
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if (call_result == SDM_OK) {
		// cache current LEDS status
		char old_product_number [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1];
		char old_product_revision_state [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1];
		char old_product_name [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1];
		char old_serial_number [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1];
		char old_manufacture_date [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1];
		char old_vendor [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1];

		::strncpy(old_product_number, p_apboardinfo->product_number, ACS_APBM_ARRAY_SIZE(old_product_number));
		::strncpy(old_product_revision_state, p_apboardinfo->product_revision_state, ACS_APBM_ARRAY_SIZE (old_product_revision_state));
		::strncpy(old_product_name, p_apboardinfo->product_name, ACS_APBM_ARRAY_SIZE(old_product_name));
		::strncpy(old_serial_number, p_apboardinfo->serial_number, ACS_APBM_ARRAY_SIZE (old_serial_number));
		::strncpy(old_manufacture_date, p_apboardinfo->manufacture_date, ACS_APBM_ARRAY_SIZE (old_manufacture_date));
		::strncpy(old_vendor, p_apboardinfo->vendor, ACS_APBM_ARRAY_SIZE(old_vendor));

//		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"old_ipmi_product_number == '%s' - old_ipmi_product_revision == '%s' ", old_ipmi_product_number, old_ipmi_product_revision );

		::strncpy(p_apboardinfo->product_number, product_number, ACS_APBM_ARRAY_SIZE(old_product_number));
		::strncpy(p_apboardinfo->product_revision_state, product_revision_state, ACS_APBM_ARRAY_SIZE (old_product_revision_state));
		::strncpy(p_apboardinfo->product_name, product_name, ACS_APBM_ARRAY_SIZE(old_product_name));
		::strncpy(p_apboardinfo->serial_number, serial_number, ACS_APBM_ARRAY_SIZE (old_serial_number));
		::strncpy(p_apboardinfo->manufacture_date, manufacture_date, ACS_APBM_ARRAY_SIZE (old_manufacture_date));
		::strncpy(p_apboardinfo->vendor, vendor, ACS_APBM_ARRAY_SIZE(old_vendor));

//		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"NEW_ipmi_product_number == '%s' -NEW ipmi_product_revision == '%s' ", p_apboardinfo->ipmi_product_number, p_apboardinfo->ipmi_product_revision );

		// update IMM, if requested
		if (update_IMM) {
			int op_res = _server_working_set->imm_repository_handler->update_snmp_ap_board_info(p_apboardinfo);

			if(op_res < 0) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Cannot update, in IMM, SNMP attributes for AP board identified by ( MAG == 0x%08X, SLOT == %d ). "
						"Call 'acs_apbm_immrepositoryhandler::update_temperature_status_on_ap_board_info(..) failed. op_res == %d !",
						magazine, slot, op_res);

				::strncpy(p_apboardinfo->product_number, old_product_number, ACS_APBM_ARRAY_SIZE(old_product_number));
				::strncpy(p_apboardinfo->product_revision_state, old_product_revision_state, ACS_APBM_ARRAY_SIZE (old_product_revision_state));
				::strncpy(p_apboardinfo->product_name, old_product_name, ACS_APBM_ARRAY_SIZE(old_product_name));
				::strncpy(p_apboardinfo->serial_number, old_serial_number, ACS_APBM_ARRAY_SIZE (old_serial_number));
				::strncpy(p_apboardinfo->manufacture_date, old_manufacture_date, ACS_APBM_ARRAY_SIZE (old_manufacture_date));
				::strncpy(p_apboardinfo->vendor, old_vendor, ACS_APBM_ARRAY_SIZE(old_vendor));

				return SDM_IMM_ERROR;
			}
		}
	}

	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_temperature_status(char (& temperature_status) [acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE +1], bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
	    char old_temperature_status[acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1] = {0};
	    ::memcpy(old_temperature_status, p_apboardinfo->temperature_status, acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1);

	    ::memcpy(p_apboardinfo->temperature_status, temperature_status, acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1);
            // update IMM, if requested
			if(update_IMM)
			{
				int op_res = _server_working_set->imm_repository_handler->update_temperature_status_on_ap_board_info(p_apboardinfo);
				if(op_res < 0)
				{
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'temperature_status' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_temperature_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
					// restore previous AP board LEDS status
					::memcpy(p_apboardinfo->temperature_status, old_temperature_status, acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1);
					return SDM_IMM_ERROR;
				}
			}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_alarm_interface_status(axe_eth_shelf::alarm_status_t alarm_status, bool update_IMM, int32_t slot, uint32_t magazine){

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set alarm_interface_status
	if(call_result == SDM_OK)
	{
		// cache current alarm_interface_status
		axe_eth_shelf::alarm_status_t old_alarm_interface_status = p_apboardinfo->alarm_interface_status;

		p_apboardinfo->alarm_interface_status = alarm_status;
		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_alarm_interface_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'alarm_interface_status' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_ap_board_alarm_interface_status(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board raid_disk_status
				p_apboardinfo->alarm_interface_status = old_alarm_interface_status;
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_network_interfaces_cards_status(
		axe_eth_shelf::nic_status_t nic0,
		axe_eth_shelf::nic_status_t nic1,
		axe_eth_shelf::nic_status_t nic2,
		axe_eth_shelf::nic_status_t nic3,
		axe_eth_shelf::nic_status_t nic4,
		axe_eth_shelf::nic_status_t nic5,
                axe_eth_shelf::nic_status_t nic6,
		bool update_IMM,
		int32_t slot,
		uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current network_interfaces_cards status
		axe_eth_shelf::nic_status_t old_nic0 = p_apboardinfo->nic_0;
		axe_eth_shelf::nic_status_t old_nic1 = p_apboardinfo->nic_1;
		axe_eth_shelf::nic_status_t old_nic2 = p_apboardinfo->nic_2;
		axe_eth_shelf::nic_status_t old_nic3 = p_apboardinfo->nic_3;
		axe_eth_shelf::nic_status_t old_nic4 = p_apboardinfo->nic_4;
		//ETH5 and ETH6 SUPPORT
		axe_eth_shelf::nic_status_t old_nic5 = p_apboardinfo->nic_5;
		axe_eth_shelf::nic_status_t old_nic6 = p_apboardinfo->nic_6;
		
		p_apboardinfo->nic_0 = nic0;
		p_apboardinfo->nic_1 = nic1;
		p_apboardinfo->nic_2 = nic2;
		p_apboardinfo->nic_3 = nic3;
		p_apboardinfo->nic_4 = nic4;
		//ETH5 and ETH6 SUPPORT
		p_apboardinfo->nic_5 = nic5;
		p_apboardinfo->nic_6 = nic6;

		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_network_interfaces_cards_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'network_interfaces_cards' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_network_interfaces_cards_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board network_interfaces_cards status
				p_apboardinfo->nic_0 = old_nic0;
				p_apboardinfo->nic_1 = old_nic1;
				p_apboardinfo->nic_2 = old_nic2;
				p_apboardinfo->nic_3 = old_nic3;
				p_apboardinfo->nic_4 = old_nic4;
				//ETH5 and ETH6 SUPPORT		
				p_apboardinfo->nic_5 = old_nic5;
				p_apboardinfo->nic_6 = old_nic6;
			
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_network_interfaces_cards_status(
		axe_eth_shelf::nic_status_t nic0,
		axe_eth_shelf::nic_status_t nic1,
		axe_eth_shelf::nic_status_t nic2,
		axe_eth_shelf::nic_status_t nic3,
		axe_eth_shelf::nic_status_t nic4,
		axe_eth_shelf::nic_status_t nic5,
        axe_eth_shelf::nic_status_t nic6,
        axe_eth_shelf::nic_status_t nic7,
        axe_eth_shelf::nic_status_t nic8,
		bool update_IMM,
		int32_t slot,
		uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current network_interfaces_cards status
		axe_eth_shelf::nic_status_t old_nic0 = p_apboardinfo->nic_0;
		axe_eth_shelf::nic_status_t old_nic1 = p_apboardinfo->nic_1;
		axe_eth_shelf::nic_status_t old_nic2 = p_apboardinfo->nic_2;
		axe_eth_shelf::nic_status_t old_nic3 = p_apboardinfo->nic_3;
		axe_eth_shelf::nic_status_t old_nic4 = p_apboardinfo->nic_4;
		//ETH5 and ETH6 SUPPORT
		axe_eth_shelf::nic_status_t old_nic5 = p_apboardinfo->nic_5;
		axe_eth_shelf::nic_status_t old_nic6 = p_apboardinfo->nic_6;
		// ETH7 and ETH8 SUPPORT
		axe_eth_shelf::nic_status_t old_nic7 = p_apboardinfo->nic_7;
		axe_eth_shelf::nic_status_t old_nic8 = p_apboardinfo->nic_8;

		p_apboardinfo->nic_0 = nic0;
		p_apboardinfo->nic_1 = nic1;
		p_apboardinfo->nic_2 = nic2;
		p_apboardinfo->nic_3 = nic3;
		p_apboardinfo->nic_4 = nic4;
		//ETH5 and ETH6 SUPPORT
		p_apboardinfo->nic_5 = nic5;
		p_apboardinfo->nic_6 = nic6;
		// ETH7 and ETH8 SUPPORT
		p_apboardinfo->nic_7 = nic7;
		p_apboardinfo->nic_8 = nic8;

		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_network_interfaces_cards_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'network_interfaces_cards' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_network_interfaces_cards_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board network_interfaces_cards status
				p_apboardinfo->nic_0 = old_nic0;
				p_apboardinfo->nic_1 = old_nic1;
				p_apboardinfo->nic_2 = old_nic2;
				p_apboardinfo->nic_3 = old_nic3;
				p_apboardinfo->nic_4 = old_nic4;
				//ETH5 and ETH6 SUPPORT
				p_apboardinfo->nic_5 = old_nic5;
				p_apboardinfo->nic_6 = old_nic6;
				//ETH7 and ETH8 SUPPORT
				p_apboardinfo->nic_7 = old_nic7;
				p_apboardinfo->nic_8 = old_nic8;

				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}


acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_network_interfaces_cards_status(
		axe_eth_shelf::nic_status_t nic0,
		axe_eth_shelf::nic_status_t nic1,
		axe_eth_shelf::nic_status_t nic2,
		axe_eth_shelf::nic_status_t nic3,
		axe_eth_shelf::nic_status_t nic4,
		bool update_IMM,
		int32_t slot,
		uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current network_interfaces_cards status
		axe_eth_shelf::nic_status_t old_nic0 = p_apboardinfo->nic_0;
		axe_eth_shelf::nic_status_t old_nic1 = p_apboardinfo->nic_1;
		axe_eth_shelf::nic_status_t old_nic2 = p_apboardinfo->nic_2;
		axe_eth_shelf::nic_status_t old_nic3 = p_apboardinfo->nic_3;
		axe_eth_shelf::nic_status_t old_nic4 = p_apboardinfo->nic_4;
		
		p_apboardinfo->nic_0 = nic0;
		p_apboardinfo->nic_1 = nic1;
		p_apboardinfo->nic_2 = nic2;
		p_apboardinfo->nic_3 = nic3;
		p_apboardinfo->nic_4 = nic4;

		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_network_interfaces_cards_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'network_interfaces_cards' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_network_interfaces_cards_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board network_interfaces_cards status
				p_apboardinfo->nic_0 = old_nic0;
				p_apboardinfo->nic_1 = old_nic1;
				p_apboardinfo->nic_2 = old_nic2;
				p_apboardinfo->nic_3 = old_nic3;
				p_apboardinfo->nic_4 = old_nic4;
			
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;
}

// update the network_interfaces_cards status of an AP board
acs_apbm_shelvesdatamanager::sdm_constant acs_apbm_shelvesdatamanager::update_ap_board_network_interface_card_info(
                axe_eth_shelf::nic_status_t nic,
                char (& nic_name) [acs_apbm::PCP_FIELD_SIZE_NIC_NAME +1],
                char (& nic_ipv4_address) [acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS +1],
                char (& nic_mac_address) [acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS +1],
                bool update_IMM,
                int32_t slot,
                uint32_t magazine)
{

	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
		return call_result;

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		axe_eth_shelf::nic_name_t nic_type;
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Retrieving nic type with nic name:%s",nic_name);
		retrieve_nic_type_by_name(nic_type, nic_name);
		axe_eth_shelf::nic_status_t old_nic;

		// cache current network_interfaces_cards status
		switch(nic_type){
		case axe_eth_shelf::IMM_NIC_0:
			old_nic = p_apboardinfo->nic_0;
			p_apboardinfo->nic_0 = nic;
			break;
		case axe_eth_shelf::IMM_NIC_1:
			old_nic = p_apboardinfo->nic_1;
			p_apboardinfo->nic_1 = nic;
			break;
		case axe_eth_shelf::IMM_NIC_2:
			old_nic = p_apboardinfo->nic_2;
			p_apboardinfo->nic_2 = nic;
			break;
		case axe_eth_shelf::IMM_NIC_3:
			old_nic = p_apboardinfo->nic_3;
			p_apboardinfo->nic_3 = nic;
			break;
		case axe_eth_shelf::IMM_NIC_4:
			old_nic = p_apboardinfo->nic_4;
			p_apboardinfo->nic_4 = nic;
			break;
		case axe_eth_shelf::IMM_NIC_5:
			old_nic = p_apboardinfo->nic_5;
			p_apboardinfo->nic_5 = nic;
			break;
		case axe_eth_shelf::IMM_NIC_6:
			old_nic = p_apboardinfo->nic_6;
			p_apboardinfo->nic_6 = nic;
			break;
		case axe_eth_shelf::IMM_NIC_7:
			old_nic = p_apboardinfo->nic_7;
			p_apboardinfo->nic_7 = nic;
		  break;
		case axe_eth_shelf::IMM_NIC_8:
			old_nic = p_apboardinfo->nic_8;
			p_apboardinfo->nic_8 = nic;
		  break;
		default:
			ACS_APBM_LOG(LOG_LEVEL_WARN,"NIC not recognized! - It's possible it's trying to"
					"update a NIC not present in IMM");
			return SDM_OK;
		}

		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_network_interfaces_card_info_on_ap_board_info(nic_type, p_apboardinfo, nic, nic_name, nic_ipv4_address, nic_mac_address);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'network_interfaces_cards' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_network_interfaces_cards_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board network_interfaces_cards status
				switch(nic_type){
				case axe_eth_shelf::IMM_NIC_0:
					p_apboardinfo->nic_0 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_1:
					p_apboardinfo->nic_1 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_2:
					p_apboardinfo->nic_2 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_3:
					p_apboardinfo->nic_3 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_4:
					p_apboardinfo->nic_4 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_5:
					p_apboardinfo->nic_5 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_6:
					p_apboardinfo->nic_6 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_7:
					p_apboardinfo->nic_7 = old_nic;
					break;
				case axe_eth_shelf::IMM_NIC_8:
				 	p_apboardinfo->nic_8 = old_nic;
					break;
				default:
					break;
				}
				return SDM_IMM_ERROR;
			}
		}
	}
	return call_result;

}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_raid_status(axe_eth_shelf::raid_status_t raid_status, bool update_IMM, int32_t slot, uint32_t magazine)
{
	acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;

	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// check parameters
	if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
	{
		is_raid_updated =true;
		return call_result;
	}

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
	acs_apbm_apboardinfo * p_apboardinfo = 0;
	call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

	// set LEDS status
	if(call_result == SDM_OK)
	{
		// cache current raid_disk_status
		axe_eth_shelf::raid_status_t old_raid_status = p_apboardinfo->raid_disk_status;

		p_apboardinfo->raid_disk_status = raid_status;
		// update IMM, if requested
		if(update_IMM)
		{
			int op_res = _server_working_set->imm_repository_handler->update_raid_status_on_ap_board_info(p_apboardinfo);
			if(op_res < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'raid_disk_status' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_raid_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
				// restore previous AP board raid_disk_status
				p_apboardinfo->raid_disk_status = old_raid_status;
				is_raid_updated = true;
				return SDM_IMM_ERROR;
			}
		}
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "It Needed to update 'raid_disk monitoring");
	is_raid_updated = false;
	return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_drbd_status(axe_eth_shelf::drbd_status_t drbd_status, bool update_IMM, int32_t slot, uint32_t magazine)
{
        acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
        // if the method has been called without passing <magazine> argument, own magazine is assumed
        if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
                magazine = own_magazine();

        // check parameters
        if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
                return call_result;

        // get exclusive access to AP boards collection
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

        // retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
        acs_apbm_apboardinfo * p_apboardinfo = 0;
        call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

        // set LEDS status
        if(call_result == SDM_OK)
        {
                // cache current drbd_disk_status
                axe_eth_shelf::drbd_status_t old_drbd_status = p_apboardinfo->drbd_disk_status;
                p_apboardinfo->drbd_disk_status = drbd_status;
                // update IMM, if requested
                if(update_IMM)
                {
                        int op_res = _server_working_set->imm_repository_handler->update_drbd_status_on_ap_board_info(p_apboardinfo);
		        if(op_res < 0)
                        {
                                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'drbd_disk_status' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_drbd_status_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
                                // restore previous AP board drbd_disk_status
                                p_apboardinfo->drbd_disk_status = old_drbd_status;
                                return SDM_IMM_ERROR;
                        }
                }
        }
        return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::update_ap_board_bios_version(bool update_IMM, int32_t slot, uint32_t magazine)
{
        acs_apbm_shelvesdatamanager::sdm_constant call_result = SDM_OK;
		int environment=0;
		_server_working_set->cs_reader->get_environment(environment);
		if (environment == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
		{	
			return call_result;
		}
        // if the method has been called without passing <magazine> argument, own magazine is assumed
        if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
                magazine = own_magazine();		
        // check parameters
        if((call_result = _check_parameters(slot, magazine)) != SDM_OK)
                return call_result;

        // get exclusive access to AP boards collection
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

        // retrieve the acs_apbm_apboardinfo object containing the info about the requested AP board
        acs_apbm_apboardinfo * p_apboardinfo = 0;
        call_result = _retrieve_ap_board_info_ptr(p_apboardinfo, slot, magazine);

        int32_t my_slot = -1;
        int32_t other_slot = -2;
        _server_working_set->cs_reader->get_my_slot(my_slot);
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "my slot found = %d", my_slot);
        _server_working_set->cs_reader->get_my_partner_slot(other_slot);
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "other slot found = %d", other_slot);

        char tmp[64 + 1] = {0};
        char t1 [16] ={0}, t2 [16] ={0}, t3 [16] ={0};

        if(slot == my_slot)
          {
            char pn[16] = {0},vers[16] = {0};
            get_local_ap_board_bios_version(pn, vers);
            // Local Bios Version
            ::sscanf(pn,"%s %s %s",t1, t2, t3);

            int sscanf_result = ::sscanf(pn,"%s %s %s",t1, t2, t3);
            if(sscanf_result != 3)
                ACS_APBM_LOG(LOG_LEVEL_WARN,"Found only %d value/values!",sscanf_result);
            ::snprintf(tmp, 64, "Pn. %s %s %s v. %s", t1, t2, t3, vers);
            ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found local bios version : %s on slot %d", tmp, slot);
          }
        else if (slot == other_slot)
          {
            char pn_remote[16] = {0},vers_remote[16] = {0};
            get_other_ap_board_bios_version(pn_remote,vers_remote);
            // Remote Bios Version
            ::sscanf(pn_remote,"%s %s %s",t1, t2, t3);
            int sscanf_result = ::sscanf(pn_remote,"%s %s %s",t1, t2, t3);
            if(sscanf_result != 3)
                ACS_APBM_LOG(LOG_LEVEL_WARN,"Found only %d value/values!",sscanf_result);
            ::snprintf(tmp, 64, "Pn. %s %s %s v. %s", t1, t2, t3, vers_remote);
            ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found other bios version : %s on slot %d", tmp, slot);
          }
        else
            ::strcpy(tmp, "UNDEFINED");

     if(call_result == SDM_OK)
      {
          char old_bios_version[64 + 1] = {0};
              // cache current raid_disk_status
          ::memcpy(old_bios_version, p_apboardinfo->bios_version, 64 + 1);

          ::memcpy(p_apboardinfo->bios_version, tmp, 64);
          // update IMM, if requested
          if(update_IMM)
          {
              int op_res = _server_working_set->imm_repository_handler->update_bios_version_on_ap_board_info(p_apboardinfo);
              if(op_res < 0)
              {
                      ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot update, in IMM, 'bios_version' attribute for AP board identified by ( MAG == 0x%08X, SLOT == %d ). Call 'acs_apbm_immrepositoryhandler::update_bios_version_on_ap_board_info(..) failed. op_res == %d !", magazine, slot, op_res);
                      // restore previous AP board raid_disk_status
                      ::memcpy(p_apboardinfo->bios_version, old_bios_version, 64 + 1 );
                      return SDM_IMM_ERROR;
              }
          }
      }
      return call_result;
}

acs_apbm_shelvesdatamanager::sdm_constant  acs_apbm_shelvesdatamanager::search_ap_boards_by_fbn(std::list<int32_t> & slots, uint16_t fbn, uint32_t magazine)
{
	// if the method has been called without passing <magazine> argument, own magazine is assumed
	if(magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		magazine = own_magazine();

	// get exclusive access to AP boards collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_apboards_map_mutex);

	// search magazine
	MapMagazineToAPBoards::const_iterator it = _apboards_map.find(magazine);
	if(it == _apboards_map.end())
		return SDM_MAGAZINE_NOT_FOUND;

	// get the AP boards collection associated to the magazine
	APBoardsCollection * const & p_apboards_collection = (*it).second;
	if(!p_apboards_collection)
		return SDM_INVALID_MAGAZINE_DATA;

	// clear the output list parameter
	slots.empty();

	// iterate on AP boards collection and put in the list output parameter the slots of the AP boards whose FBN is <fbn>
	for(APBoardsCollection::const_iterator it = p_apboards_collection->begin(); it != p_apboards_collection->end(); ++it)
		if((*it) && ((*it)->fbn == fbn))
			slots.push_back((*it)->slot_position);

	return SDM_OK;
}
void acs_apbm_shelvesdatamanager:: is_10G_feature() //kvm
{
	FILE * fs;
        char tmp[512] = {0};
        fs = ::fopen(ACS_APBM_10G_FILE , "r");
        if (fs == NULL)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error popen(..) failed! No execution of command \n");
                return;
        }
        fscanf(fs, "%18s", tmp);
        if (strcmp(tmp, "10G") == 0)
        {
                is10G = true;
                ACS_APBM_LOG(LOG_LEVEL_DEBUG,"10G FEATURE! %s\n", tmp);
        }
        else
        {
                is10G = false;
                ACS_APBM_LOG(LOG_LEVEL_DEBUG,"NOT 10G FEATURE! \n");
        }
	::fclose(fs);
}

bool acs_apbm_shelvesdatamanager:: is_10G()
{
	return is10G; 
}
bool acs_apbm_shelvesdatamanager:: is_hardwaretype_gep7()
{
        acs_apbm_api api;
        is_gep7 = api.is_hwtype_gep7();
        return is_gep7;
}


