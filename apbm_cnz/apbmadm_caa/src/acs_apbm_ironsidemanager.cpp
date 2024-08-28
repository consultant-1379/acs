/*
 * acs_apbm_ironsidemanager.cpp
 *
 *  Created on: Dec 7, 2012
 *      Author: estevol
 */

#include "acs_apbm_ironsidemanager.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_programconstants.h"
#include <acs_apbm_commandbuilder.h>

#include <acs_apbm_ironsidecommand.h>
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_operationpipescheduler.h"

#include "acs_apbm_trapsubscriptionmanager.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <acs_nclib_factory.h>
#include <acs_nclib_session.h>

int acs_apbm_ironsidemanager::send_trap_subscription_request ()
{

	//Trap handling according to New BSP Implementation
	
	int call_result = 0;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	int errorcode=0;
	char my_bgci_addresses [2][16];

	if ((call_result = _server_working_set->cs_reader->get_my_bgci_addresses(my_bgci_addresses[0], my_bgci_addresses[1]))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'get_my_bgci_addresses' failed: cannot load my AP bgci addresses : call_result == %d", call_result);
		return call_result;
	}
	int octet;

	std::string bgci_ipaddress_left_str(my_bgci_addresses[0]);
	std::string bgci_ipaddress_right_str(my_bgci_addresses[1]);
	get_octet( bgci_ipaddress_left_str,octet);	
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "getoctet %d  ",octet);
	

	uint16_t my_sys_no;
	call_result = _server_working_set->cs_reader->get_my_system_number(my_sys_no);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain own magazine. Call 'acs_apbm_csreader::get_my_magazine(..)' failed ! (call_result == %d)", call_result);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}
	
	string cmd;
	stringstream ss;
	ss<<my_sys_no;
	cmd="AP";
	cmd+=ss.str();
	errorcode=setbgci(cmd,octet);
	if(errorcode ==0){
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "successfully trap subscription");
		_shelf_manager_control_right_scb_trap_counter_++;
		_shelf_manager_control_left_scb_trap_counter_++;
		return errorcode;
	}

	return errorcode ;
}   

int acs_apbm_ironsidemanager::send_trap_unsubscription_request ()
{
return 0;
}


int acs_apbm_ironsidemanager::get_board_ipmi_data (
		int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms)
{
	UNUSED(timeout_ms);
	unsigned timeout = 3000;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;
	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	//TODO: Rework needed for BSP
	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> ipmi_data((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP,&timeout));
	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	//get_shelfId_from_address(mag_phys_addr,shelfId);
         
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!",
				__FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}


	char filter[16] = {0};
	std::string data = "";
	snprintf(filter, 13, "BLD:%02d%02dIPM-",atoi(shelfId.c_str()),board_slot);
	//unsigned int value_count = 0;
		if ((call_result = ipmi_data->executeUdp(filter, data)) == 0)
        	{
			data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

                if ((call_result = Check_error_ironside(data)) != 0)
                {
			return acs_apbm::ERR_SNMP_BOARD_NOT_FOUND;

		}
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "call_result %d", call_result);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IPMI Data %s", data.c_str());

			std::string prodNum,rev;
			int type = 0;
			getRunningIpmi(data.c_str(),prodNum,rev,type);
			memset(ipmi_product_number,0,acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1);
			memcpy(ipmi_product_number,prodNum.c_str(),acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE);
			memset(ipmi_product_revision,0,acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1);
                       	memcpy(ipmi_product_revision,rev.c_str(),acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE);
			ipmi_firmware_type=type;
			
		}
		else
		{
			 return acs_apbm::ERR_SNMP_BOARD_NOT_FOUND;
		}



	return call_result;
}
int acs_apbm_ironsidemanager::get_board_ipmi_upgrade_status (
		axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms)
{
	UNUSED(ipmi_upgrade_status);
	UNUSED(board_slot);
	UNUSED(timeout_ms);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	return call_result;
}
int acs_apbm_ironsidemanager::set_board_ipmi_upgrade (
		const char * upgrade_path,
		const char * upgrade_ip4,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * /*timeout_ms*/)
{
	unsigned timeout = 3000;
	UNUSED(upgrade_path);
	UNUSED(upgrade_ip4);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	//get_shelfId_from_address(mag_phys_addr,shelfId);
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!",
				__FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	//TODO: Currently not supported by BSP. Just skip it for the moment
	call_result = acs_apbm::ERR_DMX_GET_IPMI_UPG_STATUS_FAILURE;

	return call_result;

}

int acs_apbm_ironsidemanager::get_board_data (
		char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
		char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
		char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
		char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
		char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
		char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * /*timeout_ms*/)
{
        unsigned timeout = 5000;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending get_board_data, magazine %s, physical address %d", board_magazine_str, mag_phys_addr);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;
	unsigned int value_count = 0;
	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_data ((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP, &timeout));

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
        
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};
	std::string line;
	std::string data;
	std::string buf;
	vector<string> tokens;
	
	snprintf(filter, 13, "BLD:%02d%02dHWI-",atoi(shelfId.c_str()),board_slot);
	call_result=board_data->executeUdp(filter,data);
	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "board data is %s",data.c_str());
	istringstream ss(data);
	
	if(call_result==0)
	{
		while (std::getline(ss, line ))
		{
			tokens.push_back(line);
		} 	
		for(int i=0;i<tokens.size();i++)
		{
			string::size_type pos_start = 0, pos_end;
			pos_end = tokens[i].find(":",pos_start);
		if (pos_end != string::npos)
		{
			std::string str="";
			str = tokens[i].substr(0, pos_end);
		if(str.compare("productNumber")==0)
		{
			memcpy(product_number,(tokens[i].substr(pos_end+1)).c_str(),acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "product num is== %s",product_number);
			if (product_number[0] == 10)
			{
				memset(product_number,0,acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE);
				memcpy(product_number,"-",1);
			}
			else
			{
				value_count++;
			}
		}

		else if(str.compare("productRevisionState")==0)
		{
			memcpy(product_revision_state,(tokens[i].substr(pos_end+1)).c_str(),acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE);
			if (product_revision_state[0] == 10)
			{
				memset(product_revision_state,0,acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE);
				memcpy(product_revision_state,"-",1);
			}
			else
			{
				value_count++;
			}
		}

		else if(str.compare("productName")==0)
		{
			memcpy(product_name,(tokens[i].substr(pos_end+1)).c_str(),acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE);
			if (product_name[0] == 10)
			{
				memset(product_name,0,acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE);
				memcpy(product_name,"-",1);
			}
			else
			{
				value_count++;
			}
		}

		else if(str.compare("serialNumber")==0)
		{
			memcpy(serial_number,(tokens[i].substr(pos_end+1)).c_str(),acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE);
			if (serial_number[0] == 10)
			{
				memset(serial_number,0,acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE);
				memcpy(serial_number,"-",1);
			}
			else
			{
				value_count++;
			}
		}

		else if(str.compare("manufacturingDate")==0)
		{
			std::string tmp = tokens[i].substr(pos_end+1);
			size_t pos = 0;
			while((pos = tmp.find("-", pos)) != std::string::npos)
			{
				tmp.replace(pos, 1, "");
			}

			memcpy(manufacture_date,tmp.c_str(),acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE);
			//Empty
			if (manufacture_date[0] == 10)
			{
				memset(manufacture_date,0,acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE);
				memcpy(manufacture_date,"-",1);
			}
			else
			{
				value_count++;
			}
		}

		else if(str.compare("vendorName")==0)
		{

			string ven_name = tokens[i].substr(pos_end+1);
			if (ven_name.empty())
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Vendor name is empty");
				memset(vendor,0,acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE);
				memcpy(vendor,"-",1);
			}
			else
			{
				memcpy(vendor,(tokens[i].substr(pos_end+1)).c_str(),acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE);

				//Invalid Vendor Name
				if (vendor[0] == 10)
				{
					memset(vendor,0,acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE);
					memcpy(vendor,"-",1);
				}
				else
				{
					value_count++;
				}
	 	        }
  		   }
              }
        }
  }
	if (value_count == 0)
		call_result = acs_apbm::ERR_DMX_GET_BOARD_DATA_FAILURE;


	return call_result;
}

int acs_apbm_ironsidemanager::get_blade_led_type (
                  acs_apbm_snmp::blade_led_type_t & blade_led_type,
                  int32_t board_slot,
                  const unsigned * timeout_ms)
{
        uint32_t my_magazine = 0;

        if (const int call_result =_server_working_set->cs_reader->get_my_magazine(my_magazine))
          // ERROR: getting my magazine number
                return call_result;

        return get_blade_led_type(blade_led_type, board_slot, my_magazine, timeout_ms);
} 

int acs_apbm_ironsidemanager::get_blade_led_type (
                  acs_apbm_snmp::blade_led_type_t & blade_led_type,
                  int32_t board_slot,
                  uint32_t board_magazine,
                  const unsigned * /*timeout_ms*/)
{
	unsigned timeout =3000;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);
	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> blade_ledtype ((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP, &timeout));
	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");

	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};
        
	snprintf(filter, 13, "BLD:%02d%02dLDT",atoi(shelfId.c_str()),board_slot);

        std::string data;

        if ((call_result = blade_ledtype->executeUdp(filter, data)) == 0)
        {
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)
		{
			return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;	
		}
                if (data.compare("OLD") == 0)
                {
                        blade_led_type = acs_apbm_snmp::BLADE_LED_TYPE_OLD;
                }
                else if (data.compare("NEW") == 0)
                {
                 	blade_led_type = acs_apbm_snmp::BLADE_LED_TYPE_ON;
                }
                else
                {
			blade_led_type = acs_apbm_snmp::BLADE_LED_TYPE_NOT_SUPPORTED;
                }
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Led type is not avaiable from DMX");
		call_result = acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }

	return call_result;
}

int acs_apbm_ironsidemanager::get_board_leds_status (
		axe_eth_shelf::led_status_t & green_led,
		axe_eth_shelf::led_status_t & red_led,
		axe_eth_shelf::led_status_t & mia_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * /*timeout_ms*/)
{
	unsigned timeout = 3000;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_leds ((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP, &timeout));
	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};
        snprintf(filter, 13, "OLD:%02d%02dGET_",atoi(shelfId.c_str()),board_slot);

        std::string data;

        if ((call_result = board_leds->executeUdp(filter, data)) == 0)
        {
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)
		{
			return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
		}
                if (data.compare("on") == 0)
                {
                        green_led = axe_eth_shelf::LED_STATUS_ON;
                }
                else if (data.compare("off") == 0)
                {
                        green_led = axe_eth_shelf::LED_STATUS_OFF;
                }
                else
                {
                        green_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
                }
        
	}
	else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
                return  acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }


	data="";

	snprintf(filter, 13, "FLD:%02d%02dGET_",atoi(shelfId.c_str()),board_slot);
        if ((call_result = board_leds->executeUdp(filter, data)) == 0)
	{
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
                if ((call_result = Check_error_ironside(data)) != 0)
		{
                        return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
                }

                if (data.compare("on") == 0)
                {
          	 	red_led = axe_eth_shelf::LED_STATUS_ON;
                }
                else if (data.compare("off") == 0)
                {
                        red_led = axe_eth_shelf::LED_STATUS_OFF;
                }
                else
                {
                        red_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
                }
        }
	else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
                return  acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }

        
	data="";

        snprintf(filter, 13, "MLD:%02d%02dGET_",atoi(shelfId.c_str()),board_slot);
        if ((call_result = board_leds->executeUdp(filter, data)) == 0)
	{
	  	data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)
	        {
                        return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
                }

                if (data.compare("on") == 0)
                {
                        mia_led = axe_eth_shelf::LED_STATUS_ON;
                }
                else if (data.compare("off") == 0)
                {
                        mia_led = axe_eth_shelf::LED_STATUS_OFF;
                }
                else
                {
                        mia_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;

                }
        }
	else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
                return  acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }


	return call_result;
}

int acs_apbm_ironsidemanager::get_board_leds_status (
		axe_eth_shelf::led_status_t & green_led,
		axe_eth_shelf::led_status_t & red_led,
		axe_eth_shelf::led_status_t & mia_led,
		axe_eth_shelf::led_status_t & blue_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * /*timeout_ms*/) {
	unsigned timeout =3000;
        
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine))
		// ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_leds_status ((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP, &timeout));
	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	//TO REMOVE get_shelfId_from_address(mag_phys_addr,shelfId);

	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};
        snprintf(filter, 13, "OLD:%02d%02dGET_",atoi(shelfId.c_str()),board_slot);

        std::string data;

        if ((call_result = board_leds_status->executeUdp(filter, data)) == 0)
        {
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)
		{
			return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
		}

                if (data.compare("on") == 0)
                {
                        green_led = axe_eth_shelf::LED_STATUS_ON;
                }
                else if (data.compare("off") == 0)
                {
                        green_led = axe_eth_shelf::LED_STATUS_OFF;
                }
                else
                {
                        green_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
                }
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
                return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }

	
	data="";
	
	snprintf(filter, 13, "SLD:%02d%02dGET_",atoi(shelfId.c_str()),board_slot);
	
	if ((call_result = board_leds_status->executeUdp(filter, data)) == 0)
        {
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)
                {
                        return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
                }

                if (data.compare("on") == 0)
                {
                        mia_led = axe_eth_shelf::LED_STATUS_ON;
                }
                else if (data.compare("off") == 0)
                {
                        mia_led = axe_eth_shelf::LED_STATUS_OFF;
                }
                else
                {
                        mia_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
                }
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
                return  acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }


	data="";
	
	snprintf(filter, 13, "FLD:%02d%02dGET_",atoi(shelfId.c_str()),board_slot);
	if ((call_result = board_leds_status->executeUdp(filter, data)) == 0)
        {
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)
                {
                        return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
                }

                if (data.compare("on") == 0)
                {
                        red_led = axe_eth_shelf::LED_STATUS_ON;
                }
                else if (data.compare("off") == 0)
                {
                        red_led = axe_eth_shelf::LED_STATUS_OFF;
                }
                else
                {
                        red_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
                }
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
               	return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }


	data="";
	
	snprintf(filter, 13, "MLD:%02d%02dGET_",atoi(shelfId.c_str()),board_slot);
	if ((call_result = board_leds_status->executeUdp(filter, data)) == 0)
        {
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)    
                {
                        return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
                }

                if (data.compare("on") == 0)
                {
                        blue_led = axe_eth_shelf::LED_STATUS_ON;
                }
                else if (data.compare("off") == 0)
                {
                        blue_led = axe_eth_shelf::LED_STATUS_OFF;
                }
                else
                {
                        blue_led = axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
                }
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
                return acs_apbm::ERR_DMX_GET_BOARD_LED_STATUS_FAILURE;
        }
	return call_result;
}


int acs_apbm_ironsidemanager::get_board_bios_run_mode (
		axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * /*timeout_ms*/)
{
	unsigned timeout=3000;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> bios_run ((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP, &timeout));
	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);


	std::string shelfId("");

	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		//bios_run_mode = axe_eth_shelf::BIOS_BOOT_RUN_MODE_UNDEFINED;
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "TO REMOVE: print the shelfid in the bios method: %s",shelfId.c_str());


	char filter[16] = {0};
	snprintf(filter, 13, "BIOS%02d%02dRMGT",atoi(shelfId.c_str()),board_slot);

        std::string data;

        if ((call_result = bios_run->executeUdp(filter, data)) == 0)
        {
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((call_result = Check_error_ironside(data)) != 0)
		{
		return acs_apbm::ERR_DMX_GET_BOARD_BIOS_RUN_MODE_FAILURE;
		}

                if (data.compare("BIOS1") == 0)
                {
                        bios_run_mode = axe_eth_shelf::BIOS_BOOT_RUN_MODE_BIOS_1;
                }
                else if (data.compare("BIOS2") == 0)
                {
                 	bios_run_mode = axe_eth_shelf::BIOS_BOOT_RUN_MODE_BIOS_2;
                }
                else
                {
			bios_run_mode = axe_eth_shelf::BIOS_BOOT_RUN_MODE_UNDEFINED;
                }
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No Bios Run Mode avaiable from DMX");
		call_result = acs_apbm::ERR_DMX_GET_BOARD_BIOS_RUN_MODE_FAILURE;
        }


	

	return call_result;
}


int acs_apbm_ironsidemanager::get_board_power_status (
		acs_apbm_snmp::bladePwr_status_t & power_status,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * /*timeout_ms*/)
{
	unsigned timeout = 3000;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> power_state((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP, &timeout));

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	//TO REMOVE get_shelfId_from_address(mag_phys_addr,shelfId);
          
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};
	snprintf(filter, 13, "BLD:%02d%02dPWR-",atoi(shelfId.c_str()),board_slot);

	std::string data;

	if ((call_result = power_state->executeUdp(filter, data)) == 0)
	{
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if((call_result = Check_error_ironside(data)) != 0)
		{
        		return	acs_apbm::ERR_DMX_GET_BOARD_POWER_STATUS_FAILURE;
		}
		
		if (data.compare("ON") == 0)
		{
			power_status = acs_apbm_snmp::BLADE_PWR_ON;
		}
		else if (data.compare("OFF") == 0)
		{
			power_status = acs_apbm_snmp::BLADE_PWR_OFF;
		}
		else
		{
			power_status = acs_apbm_snmp::BLADE_PWR_NOT_SUPPORTED;
		}
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
		call_result = acs_apbm::ERR_DMX_GET_BOARD_POWER_STATUS_FAILURE;
	}

	return call_result;
}

int acs_apbm_ironsidemanager::manage_SensorStateChange (
		int /*magPlugNumber*/,
		int slotPos,
                int sensorType,
                int sensorID,
                int sensorTypeCode,
		const char * sensorEventData)
{
	UNUSED(slotPos);
	UNUSED(sensorType);
	UNUSED(sensorID);
	UNUSED(sensorTypeCode);
	UNUSED(sensorEventData);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}


int acs_apbm_ironsidemanager::manage_TemperatureChange (
		int magPlugNumber,
		int slotPos,
                int notificationID,
		const char * sensorEventData)
{

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

      	// notify the TRAP to the SUBSCRIPTION MANAGER
	
         // prepare TRAP to be notified
        trapdata trap;
        trap.trapType = acs_apbm_trapmessage::SENSOR_STATE_CHANGE;
        trap.values.push_back(magPlugNumber);
        trap.values.push_back(slotPos);
        trap.values.push_back(notificationID);
        trap.message_len = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
        sensorEventData && memcpy(trap.message, sensorEventData, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);
	_server_working_set->trap_subscription_manager->notifyTrapInfo(slotPos, trap);

	int temp_status = 1;
	char temperature[acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1] = {0};
	memcpy(temperature, sensorEventData, acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SENSOR_EVENT_DATA =='%02X'", ((unsigned char *)temperature)[0]);

	// Set to critical when SensorEventData1 value is 0x02,0x09,0x0B,0x04,0x07 else OK
	if(( *sensorEventData == 0x09)||( *sensorEventData == 0x0B)||( *sensorEventData == 0x04)||( *sensorEventData == 0x02)||( *sensorEventData == 0x07))
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SENSOR_EVENT_DATA CRITICAL");
		strcpy(temperature,"CRT");
		if(( *sensorEventData == 0x02)||( *sensorEventData == 0x07))
			temp_status = 2;
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SENSOR_EVENT_DATA OK");
		strcpy(temperature,"OK");
		temp_status = 0;
	}

	//CHECK VALUE
	_server_working_set->snmp_manager->manage_APUB_temperature(magPlugNumber,slotPos,temp_status);
	_server_working_set->shelves_data_manager->update_ap_board_temperature_status(temperature, true, slotPos);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int acs_apbm_ironsidemanager::manage_BoardPresence (
		int magPlugNumber,
		int slotPos,
		int BoardadmnState,
		int BoardPresenceState,
		const char * switch_board_ip,
		 const char * message
		)
{
		UNUSED(BoardadmnState);
		UNUSED(switch_board_ip);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

		axe_eth_shelf::status_t status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
        trapdata trap;
        trap.trapType = acs_apbm_trapmessage::BOARD_PRESENCE;
        trap.values.push_back(magPlugNumber);
        trap.values.push_back(slotPos);
        trap.values.push_back(BoardPresenceState);
        trap.message_len = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
        message && memcpy(trap.message, message, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);

        // notify the TRAP to the SUBSCRIPTION MANAGER
        _server_working_set->trap_subscription_manager->notifyTrapInfo(slotPos, trap);
        ACS_APBM_LOG(LOG_LEVEL_INFO, "Trap notify about the board at slot position %d was successfully sent!", slotPos);

	switch(BoardPresenceState)
	{
        case axe_eth_shelf::UNKNOWN:
                ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'NOT_ACCESSIBLE' in slot '%d'", slotPos);
                break;

	case axe_eth_shelf::INSTALLED_AND_ACCEPTABLE:
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'INSTALLED AND ACCEPTABLE' in slot '%d'", slotPos);

		if((_server_working_set->shelves_data_manager->get_ap_board_status(status,slotPos)) != 0 ) break;
		if(status != axe_eth_shelf::BOARD_STATUS_WORKING){
			_server_working_set->shelves_data_manager->update_ap_board_info(true, slotPos);
			_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slotPos);
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No needed update board status in slot '%d'", slotPos);
		break;
	case axe_eth_shelf::INSTALLED_AND_NOT_ACCEPTABLE:
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'INSTALLED_AND_NOT_ACCETABLE' in slot '%d'", slotPos);
		break;
	case axe_eth_shelf::NOT_INSTALLED:
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'REMOVED' in slot '%d'", slotPos);
		//******************Modification done for TR HV21933*************************************
		uint32_t my_magazine = 0;
		int32_t slot =0;
		int32_t partner_slot=0;
		if (const int call_result= _server_working_set->cs_reader->get_my_slot (slot)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_magazine' failed: cannot get my slot from CS: call_result == %d", call_result);
			break;
		} 
		if (const int call_result= _server_working_set->cs_reader->get_my_partner_slot (partner_slot)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_magazine' failed: cannot get my parten slot from CS: call_result == %d", call_result);
			break;
		}
		if((slotPos != slot) && (slotPos != partner_slot)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Slot matching failed Trapslot== %d,slot==%d,partner_slot=%d",slotPos,slot,partner_slot);
			break;
		}
		if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_magazine' failed: cannot get my magazine address from CS: call_result == %d", call_result);
			return call_result;
		}
		
		magPlugNumber &= 0x0F;
		if((my_magazine & 0x0F) == static_cast<uint32_t>(magPlugNumber)) { //******************Modification done for TR HV21933************
			if((_server_working_set->shelves_data_manager->get_ap_board_status(status,slotPos)) != 0 ) break;
			if(status != axe_eth_shelf::BOARD_STATUS_MISSING){
				_server_working_set->shelves_data_manager->reset_ap_board_info(true, slotPos);
				//  The board has been removed raise  BOARD MISSING ALARM !!
				_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slotPos);

			} else {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No needed reset board status in slot '%d'", slotPos);
			}
		}
		break;
	}
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int acs_apbm_ironsidemanager::manage_ColdStart (const char */* switch_board_IP*/)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	int call_result = 0;

	// schedule operation in order to update AP boards info
	call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM,2);
	if(call_result < 0)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to schedule operation 'SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM'. Call 'acs_apbm_operationpipescheduler::schedule_operation()' failed. call_result == %d", call_result);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

}

int acs_apbm_ironsidemanager::set_board_REDled (
		uint32_t board_magazine,
		int32_t board_slot,
		bool led_status)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	int call_result = 0;
	
	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine))
        // ERROR: while converting

		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;


	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_REDled((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP));
	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
    	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}
	char filter[16] = {0};
	if (led_status == axe_eth_shelf::LED_STATUS_ON)
        {
		snprintf(filter, 13, "FLD:%02d%02dON_-",atoi(shelfId.c_str()),board_slot);       
        }
	else if (led_status == axe_eth_shelf::LED_STATUS_OFF)
        {
		snprintf(filter, 13, "FLD:%02d%02dOFF-",atoi(shelfId.c_str()),board_slot);
        }
	std::string data;
	if ((call_result = board_REDled->executeUdp(filter, data)) == 0)
        {
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
                if (data.compare("OK") != 0)
		{
			return acs_apbm::ERR_DMX_SET_BOARD_LED_STATUS_FAILURE;
		}
	}
	else 
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
		return acs_apbm::ERR_DMX_SET_BOARD_LED_STATUS_FAILURE;
	}

	return call_result;

}
int acs_apbm_ironsidemanager::set_board_MIAled (
		uint32_t board_magazine,
		int32_t board_slot,
		int led_status)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	int call_result = 0;

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine))
		// ERROR: while converting

		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;


	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_MIAled((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP));

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};

	if (led_status == axe_eth_shelf::LED_STATUS_ON)
	{
		snprintf(filter, 13, "MLD:%02d%02dON_-",atoi(shelfId.c_str()),board_slot);
	}
	else if (led_status == axe_eth_shelf::LED_STATUS_OFF)
	{
		snprintf(filter, 13, "MLD:%02d%02dOFF-",atoi(shelfId.c_str()),board_slot);
	}


	std::string data;

	if ((call_result = board_MIAled->executeUdp(filter, data)) == 0)
	{
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

		if (data.compare("OK") != 0)
		{
			call_result = acs_apbm::ERR_DMX_SET_BOARD_MIA_LED_FAILURE;
		}
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
		call_result = acs_apbm::ERR_DMX_SET_BOARD_MIA_LED_FAILURE;
	}

	return call_result;

}

int acs_apbm_ironsidemanager::set_board_BLUEled (
		uint32_t board_magazine,
		int32_t board_slot,
		int led_status)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	int call_result = 0;
	
	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine))
        // ERROR: while converting

		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;


	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_MIAled((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP));

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
        
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}
	char filter[16] = {0};
	if (led_status == axe_eth_shelf::LED_STATUS_ON)
	{
		snprintf(filter, 13, "MLD:%02d%02dON_-",atoi(shelfId.c_str()),board_slot);
	}
	else if (led_status == axe_eth_shelf::LED_STATUS_OFF)
	{
		snprintf(filter, 13, "MLD:%02d%02dOFF-",atoi(shelfId.c_str()),board_slot);
	}
	string data;
	if ((call_result = board_MIAled->executeUdp(filter, data)) == 0)
        {
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

                if (data.compare("OK") != 0)
                {
                	 return acs_apbm::ERR_DMX_SET_BOARD_MIA_LED_FAILURE;
                }
        }  
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
	        return acs_apbm::ERR_DMX_SET_BOARD_MIA_LED_FAILURE;
        }
	return call_result;

}

int acs_apbm_ironsidemanager::set_board_Power (
		uint32_t board_magazine,
		int32_t board_slot,
		bool pwr_status)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_pwr((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP));

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	std::string pwr_status_val = "";

	//TO REMOVE: get_shelfId_from_address(mag_phys_addr,shelfId);
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};

	if (pwr_status)
	{
		snprintf(filter, 13, "PWR:%02d%02dON_-",atoi(shelfId.c_str()),board_slot);
	}
	else
	{
		snprintf(filter, 13, "PWR:%02d%02dOFF-",atoi(shelfId.c_str()),board_slot);
	}


	std::string data;

	if ((call_result = board_pwr->executeUdp(filter, data)) == 0)
	{
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

		if (data.compare("OK") != 0)
		{
			call_result = acs_apbm::ERR_DMX_SET_BOARD_POWER_STATUS_FAILURE;
		}
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
		call_result = acs_apbm::ERR_DMX_SET_BOARD_POWER_STATUS_FAILURE;
	}

	return call_result;

}

int acs_apbm_ironsidemanager::set_board_reset(
		uint32_t board_magazine,
		int32_t board_slot)
{

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> board_pwr((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP));

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	//TO REMOVE:get_shelfId_from_address(mag_phys_addr,shelfId);
	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};

	snprintf(filter, 13, "RST:%02d%02dHARD",atoi(shelfId.c_str()),board_slot);

	std::string data;

	if ((call_result = board_pwr->executeUdp(filter, data)) == 0)
	{
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

		if (data.compare("OK") != 0)
		{
			call_result = acs_apbm::ERR_DMX_RESET_BOARD_FAILURE;
		}
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
		call_result = acs_apbm::ERR_DMX_RESET_BOARD_FAILURE;
	}

	return call_result;

}

int acs_apbm_ironsidemanager::get_my_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	int call_result = 0;

	board_infos.clear();
	board_infos.push_back(_my_dmx_board_infos);

	return call_result;
}

int acs_apbm_ironsidemanager::get_all_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	return get_my_switch_board_info(board_infos);
}

int acs_apbm_ironsidemanager::set_scb_link_down_time (int /*slot*/, uint32_t /*magazine*/, int64_t link_down_time)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if(!_my_dmx_board_infos.snmp_link_down_time)
		_my_dmx_board_infos.snmp_link_down_time = link_down_time;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "DMX link status updated: link down time is == %ld", link_down_time);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int acs_apbm_ironsidemanager::reset_scb_link_down_time (int /*slot*/, uint32_t /*magazine*/)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	_my_dmx_board_infos.snmp_link_down_time = 0;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "DMX link status updated: link down time is == 0");

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int64_t acs_apbm_ironsidemanager::get_scb_link_down_time (int /*slot*/, uint32_t /*magazine*/)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	int64_t link_down_time = _my_dmx_board_infos.snmp_link_down_time;
	return link_down_time;
}

/*********************************************************************************
 * Private Methods
 * *******************************************************************************/


int acs_apbm_ironsidemanager::shelf_addr_to_physical_addr(const char* shelfaddr)
{

	std::stringstream ss(shelfaddr);
	std::string item;
	char delim = '.';

	std::vector<std::string> elems;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	char tmp[32] = {0};
	if (elems.size() == 4)
	{
		int plug0 = atoi(elems.at(0).c_str());
		int plug1 = atoi(elems.at(1).c_str());
		int plug3 = atoi(elems.at(3).c_str());

		sprintf(tmp,"%x%x%x",plug3,plug1,plug0);

	}
	else
		return -1;

	char* endPtr;
	int mag = strtol(tmp, &endPtr,16);

	return mag;

}

int acs_apbm_ironsidemanager::initialize_dmx_boardinfo()
{

	_server_working_set->cs_reader->get_dmxc_addresses(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);

	_my_dmx_board_infos.slot_position = 0;
	_my_dmx_board_infos.magazine = 0;
//	memcpy(_my_dmx_board_infos.ipna_str,"192.254.15.1",12);
//	memcpy(_my_dmx_board_infos.ipnb_str,"192.254.16.1",12);
	_my_dmx_board_infos.state = axe_eth_shelf::SHELF_MGR_STATE_ACTIVE;
	_my_dmx_board_infos.neighbour_state = axe_eth_shelf::SHELF_MGR_STATE_PASSIVE;

	_my_dmx_board_infos.snmp_link_down_time = 0;

	return 0;
}

int acs_apbm_ironsidemanager::get_shelfId_from_address_loop(const char* shelf_addr, std::string& shelfId)
{

	int mag_phys_addr = -1;

	mag_phys_addr = shelf_addr_to_physical_addr(shelf_addr);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	return get_shelfId_from_address_loop(mag_phys_addr, shelfId);
}

int acs_apbm_ironsidemanager::get_shelfId_from_address_loop(int ph_addr, std::string& shelfId)
{
	std::map<int, std::string>::iterator it;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	int call_result = acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;

	it = _shelf_addr_id_map.find(ph_addr);

	if (it != _shelf_addr_id_map.end())
	{
		shelfId = it->second;
		call_result = acs_apbm::ERR_NO_ERRORS;
	}
	else
	{
		acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
		auto_ptr<acs_apbm_ironsidecommand> comShelf((acs_apbm_ironsidecommand*)commandBuilder.make(ironsideBuilder::UDP));
		std::string data;
		std::vector<std::string> tokens;
		std::string buf;

		std::string filter("SHLF--------");

		if (comShelf->executeUdp(filter, data) == 0)
		{

			std::stringstream ss(data);

			ACS_APBM_LOG(LOG_LEVEL_INFO, "Data received");

			while (ss >> buf){
				tokens.push_back(buf);
			}

			ACS_APBM_LOG(LOG_LEVEL_INFO, "Tokens size %d", tokens.size());

			for(uint32_t i = 0; i < tokens.size(); i++)
			{
				std::string shelfid, physadd;
				string::size_type pos_start = 0, pos_end;
				pos_end = tokens[i] .find(":",pos_start);

				ACS_APBM_LOG(LOG_LEVEL_INFO, "Tokens[%d] = %s", i, tokens[i].c_str());

				if (pos_end != string::npos)
				{

					shelfid = tokens[i].substr(0, pos_end);
					physadd = tokens[i].substr(pos_end+1);

					ACS_APBM_LOG(LOG_LEVEL_INFO, "Physical Address %s, Shelf ID %s", physadd.c_str(), shelfid.c_str());

				}

				int tmp = atoi(physadd.c_str());
				_shelf_addr_id_map.insert(std::pair<int, std::string>(tmp, shelfid));
			}

			it = _shelf_addr_id_map.find(ph_addr);

			if (it != _shelf_addr_id_map.end())
			{
				shelfId = it->second;
				call_result = acs_apbm::ERR_NO_ERRORS;
			}

		}



	}

	return call_result;
}

int acs_apbm_ironsidemanager::dec_to_hex_fatalEvent(const char* int_fatalEvent, char (& hex_fatalEvent) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1])
{

	std::string tmp(int_fatalEvent);

	size_t index = tmp.find_first_of("]");
	if (index != string::npos)
		tmp = tmp.substr(0, index);

	index = tmp.find_first_of("[");
	if (index != string::npos)
		tmp = tmp.substr(index+1);

	std::stringstream ss(tmp);
	std::string item;
	char delim = ',';

	std::vector<int> elems;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(atoi(item.c_str()));
	}

	int ret = 0;

	for(unsigned int i=0; i < elems.size(); i++)
		ret += ::sprintf(hex_fatalEvent + ret, "%02X ", elems.at(i));

	return acs_apbm::ERR_NO_ERRORS;

}

int acs_apbm_ironsidemanager::getRunningIpmi(std::string strBuf, string &prod, std::string &rev, int &type)
{
	std::string upgRev,fbRev,nonupgRev,runRev,runRevType;

	string::size_type pos_start = 0, pos_end;

	pos_end = strBuf.find ("Upg:",pos_start);
	pos_start = pos_end +5;
	pos_end =  strBuf.find (';',pos_start);
	if (pos_end != string::npos)
	{
		upgRev = strBuf.substr (pos_start, pos_end - pos_start);
	}

	pos_end = strBuf.find ("Fb:",pos_start);
	pos_start = pos_end +4;
	pos_end =  strBuf.find (';',pos_start);
	if (pos_end != string::npos)
	{
		fbRev = strBuf.substr (pos_start, pos_end - pos_start);
	}

	pos_end = strBuf.find ("NonUpg",pos_start);   
	pos_start = pos_end +8;
	pos_end =  strBuf.find (';',pos_start);
	if (pos_end != string::npos)
	{
		nonupgRev = strBuf.substr (pos_start, pos_end - pos_start);
	}
	pos_end = strBuf.find ("Run:",pos_start);
	pos_start = pos_end +5;

	runRev = strBuf.substr (pos_start);


	if(strcmp(runRev.c_str(),upgRev.c_str())!=0)
	{
		if(strcmp(runRev.c_str(),fbRev.c_str())!=0)
		{
			if(strcmp(runRev.c_str(),nonupgRev.c_str())!=0)
			{
				type = -1;
				//type = "NONUPG";
			}
		}
		else
		{
			type = 1; //"FB";
		}
	}
	else
	{
		type = 0;// "UPG";
	}
	size_t found1,found2 =0;

	found2 = runRev.find_last_of (",");

	found1 = runRev.find_first_of (",");
	if ((found1 != string::npos) && (found2 != string::npos))
	{
		prod = runRev.substr (0, found1);
		rev =  runRev.substr((found1+1),(found2-found1)-1);
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int acs_apbm_ironsidemanager::setsubscriber(std::string query)
{

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s ", __FUNCTION__);
	int result =-1;
	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str,_my_dmx_board_infos.ipnb_str);
	acs_apbm_ironsidecommand *com=((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP));

	std::string data="";
	if(!com)
	{
		return result ;
	}
	result=com->executeUdp(query,data);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "after executing udp, result=%d",result);
	if(result ==0)
	{
		if(!data.empty())
		{
			if (!data.empty() && data[data.length()-1] == '\n') {
				data.erase(data.length()-1);
			}
			if((data.compare("OK"))==0)
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, " data is ok" );
				result=0;
			}
			else
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, " data is Nok" );
				result=-1;
			}
		}
	}
	delete com;
	com=NULL;
	return result;

}
int acs_apbm_ironsidemanager::setbgci(std::string cmd,int octet)
{
	int result=-1;
	std::string query;
	std::string ipv4con;
	ipv4con=cmd;
	ipv4con+="A";
	query=setmbmcomand(ipv4con,octet);
	result=setsubscriber(query);

	if(!result)
	{
		ipv4con=cmd;
		ipv4con+="B";
		query=setmbmcomand(ipv4con,octet);
	        result=setsubscriber(query);
		return result;	
	}
	return result;
}
std::string acs_apbm_ironsidemanager::setmbmcomand(std::string ipv4con ,int octet)
{
	std::string cmd="MBM:";
	cmd+=ipv4con;
	ostringstream convert;
	std::string query="";
	std::string plug4 ="";
	if(octet <=99)
	{	
         convert << setw( 3 ) << setfill( '0' ) << octet;
         plug4 = convert.str();
	}
	else
	{
	 convert << octet;
         plug4 = convert.str();
	}

	query=(cmd+plug4);
	query+="-";
	
	return query;
}
int acs_apbm_ironsidemanager::get_octet(std::string bgci,int &octet)
{
	std::stringstream ss(bgci);

	std::string item;

	char delim = '.';

	std::vector<std::string> elems;

	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	//char tmp[32] = {0};

	if (elems.size() == 4)
	{
		octet = atoi(elems.at(3).c_str());
		return 0;
	}
	else
		return -1;

}

int acs_apbm_ironsidemanager::get_hw_install_status(
		axe_eth_shelf::hw_installation_status_t &status,
		int32_t board_slot,
		uint32_t board_magazine,
		unsigned int*)
{
	unsigned timeout = 3000;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	char board_magazine_str [16] = {0};
	int mag_phys_addr = -1;

	//Here we convert Shelf Address in a.b.c.d format
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	mag_phys_addr = shelf_addr_to_physical_addr(board_magazine_str);

	if (mag_phys_addr == -1)
		return acs_apbm::ERR_IP_ADDRESS_CONVERSION;

	int call_result = 0;

	acs_apbm_commandbuilder commandBuilder(_my_dmx_board_infos.ipna_str, _my_dmx_board_infos.ipnb_str);
	auto_ptr<acs_apbm_ironsidecommand> power_state((acs_apbm_ironsidecommand*) commandBuilder.make(ironsideBuilder::UDP, &timeout));

	//Convert shelf and slot info from integer to char
	char shelf[16] = {0};
	char slot[16] = {0};

	sprintf(shelf,"%d",mag_phys_addr);
	sprintf(slot,"%d",board_slot);

	std::string shelfId("");
	//TO REMOVE get_shelfId_from_address(mag_phys_addr,shelfId);

	if (get_shelfId_from_address_loop(mag_phys_addr,shelfId) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, magazine %s (physical address %d) not found!", __FUNCTION__, board_magazine_str, mag_phys_addr);
		return acs_apbm::ERR_DMX_MAGAZINE_NOT_FOUND;
	}

	char filter[16] = {0};
	snprintf(filter, 13, "BLD:%02d%02dHIS-",atoi(shelfId.c_str()),board_slot);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Method %s: HWI filter: %s", __FUNCTION__, filter);

	std::string data;

	if ((call_result = power_state->executeUdp(filter, data)) == 0)
	{
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if ((strcmp(data.c_str(),ACS_APBM_NOT_READY)== 0) || (strcmp(data.c_str(),ACS_APBM_UNSPEC) == 0))
		{
			 return acs_apbm::ERR_DMX_GET_BOARD_HWI_STATUS_FAILURE;
		}

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Method %s: HWI Status: %s", __FUNCTION__, data.c_str());

		if (data.compare("NOT_INSTALLED") == 0 || data.compare("ACCESS") == 0)
		{
			status = axe_eth_shelf::NOT_INSTALLED;
		}
		else if (data.compare("INSTALLED_AND_ACCEPTABLE") == 0)
		{
			status = axe_eth_shelf::INSTALLED_AND_ACCEPTABLE;
		}
		else if (data.compare("INSTALLED_AND_NOT_ACCEPTABLE") == 0)
		{
			status = axe_eth_shelf::INSTALLED_AND_NOT_ACCEPTABLE;
		}
		else
		{
			status = axe_eth_shelf::UNKNOWN;
		}

	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Exiting Method %s, Execute failure!", __FUNCTION__);
		call_result = acs_apbm::ERR_DMX_GET_BOARD_HWI_STATUS_FAILURE;
	}

	return call_result;
}
int acs_apbm_ironsidemanager::get_managerTrapSubscrIp(
		acs_apbm_switchboardinfo & /*switch_board*/,
		const unsigned * timeout_ms)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);

	int call_result = 0;

	return call_result;
}

int acs_apbm_ironsidemanager::get_board_fatal_event_log (
		char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * /*timeout_ms*/)
{
	int call_result=0;
	return call_result;
}

int acs_apbm_ironsidemanager::Check_error_ironside(std::string data)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Inside check error where data is %s",data.c_str());	
	int result = 0;
	if ((strcmp(data.c_str(),ACS_APBM_NOT_READY)== 0) || (strcmp(data.c_str(),ACS_APBM_UNSPEC) == 0) || (strcmp(data.c_str(),ACS_APBM_ACCESS) == 0) || (strcmp(data.c_str(),ACS_APBM_ACTION)== 0) || (strcmp(data.c_str(),ACS_APBM_ABSENT)== 0))	
	{
	 	result = -1;
	}
	return result;
}	
