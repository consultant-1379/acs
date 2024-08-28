/** @file bios_set_handler.cpp
 *	@brief
 *	@author xassore
 *	@date 2011-11-11
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
 *	+=====+============+==============+=====================================+
 *	| REV | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=====+============+==============+=====================================+
 *	| 001 | 2011-11-11 | xassore      | File created.                       |
 *	+=====+============+==============+=====================================+
 */
#include <new>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <ACS_CS_API.h>
#include <ACS_CS_API_Set.h>
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_paramhandling.h>
#include <acs_apbm_ipmiapi_impl.h>
#include <acs_apbm_ipmiconstants.h>
#include <acs_apbm_ipmiapi_types.h>
#include <acs_apbm_snmpmibdefinitions.h>
#include <acs_apbm_macros.h>
#include <acs_apbm_api.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_CLibTypes.h>

#include <bios_set_snmpmanager.h>
#include <bios_set_handler.h>
bios_set_handler::~bios_set_handler()
{
        if(ipmi_api != NULL)
        {
                delete ipmi_api;
                ipmi_api=NULL;
        }
}
#include "acs_apbm_cmd_ironsidemanager.h"

int bios_set_handler:: initialize(bios_set_cmd * bs_cmd)
{
	_cmd_exec_mode = bs_cmd->cmd_execution_mode();
	if(_cmd_exec_mode == bios_set_ns::SNMP_MODE)
        {

		OmHandler omManager;
		ACS_CC_ReturnType result;

		result = omManager.Init();
		if (result != ACS_CC_SUCCESS){
    	BIOSSET_LOG( "ERROR - bios_set_handler:: initialize () - error in getting node Architecture from IMM - error == %d\n", result);
        return (bios_set_handler_ns::ERR_GETTING_NODE_ARCHTECTURE);
		}/*exiting on IMM init failure: generic error*/

		std::vector<std::string> p_dnList;
		result = omManager.getClassInstances(IMMROOT, p_dnList);
		if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) {
    	BIOSSET_LOG( "ERROR - bios_set_handler:: initialize () - error in getting node Architecture from IMM - error == %d\n", result);
        return (bios_set_handler_ns::ERR_GETTING_NODE_ARCHTECTURE);
		}

		string class_instance_name = p_dnList[0];
		omManager.Finalize();

		const std:: string ap_node_architecture_attr = "apgShelfArchitecture";
		acs_apgcc_paramhandling pha;
		ACS_CC_ReturnType imm_result;
		int nodeArchitecture = 0;
		imm_result = pha.getParameter(class_instance_name,ap_node_architecture_attr, &nodeArchitecture);
    if (result != ACS_CC_SUCCESS){
     	BIOSSET_LOG( "ERROR - bios_set_handler:: initialize () - error in getting node Architecture from IMM - error == %d\n", result);
         return (bios_set_handler_ns::ERR_GETTING_NODE_ARCHTECTURE);
     }

        BIOSSET_LOG("DEBUG - bios_set_handler::initialize - getParameter() returns <%d>)\n", nodeArchitecture);
        isCBA = ((nodeArchitecture == bios_set_handler_ns::SCX_CBA_ARCHITECTURE) | (nodeArchitecture == bios_set_handler_ns::SMX_CBA_ARCHITECTURE)) ? true: false ;
		BIOSSET_LOG("DEBUG - bios_set_handler::initialize (isCBA == %d)\n", nodeArchitecture);
		char magazine[MAGAZINENAME_MAXLEN]= {0};
		strncpy(magazine, bs_cmd->magazine(), MAGAZINENAME_MAXLEN);
		if ( const int ret_code = get_switch_boards_info_from_cs (magazine) ){
			BIOSSET_LOG( "ERROR - bios_set_handler:: initialize () - error in getting switch board info from CS - error == %d\n", ret_code);
			return set_command_result(ret_code);
		}
		for (int i=0; i <2; i++){
			BIOSSET_LOG("DEBUG - bios_set_handler:: initialize() - switch board infos  --> ipna = '%s', ipnb = '%s', mag_num = %u, slot_num = %d\n",
			this->_switch_boards[i].ipna_str, this->_switch_boards[i].ipnb_str, this->_switch_boards[i].magazine, this->_switch_boards[i].slot_position);
		}
	}
	else if(_cmd_exec_mode == bios_set_ns::COM_MODE)
	{
		char com_port[32] = {0};
		strncpy(com_port, bs_cmd->com_port(),32);
		BIOSSET_LOG("DEBUG - bios_set_ns::COM_MODE with port %s \n",com_port);
		if(init_ipmi(com_port) != bios_set_handler_ns::ERR_NO_ERROR)
			return bios_set_handler_ns::ERR_IN_EXECUTION;
	}

	return bios_set_handler_ns::ERR_NO_ERROR;
}

int bios_set_handler::init_ipmi(const char* device_name)
{
	int result_ = bios_set_handler_ns::ERR_NO_ERROR;
	if(!device_name || !*device_name)
		return bios_set_handler_ns::ERR_IPMIAPI_NOT_INIZIALIZED;

	if(!ipmi_api && !(ipmi_api = new (std::nothrow) acs_apbm_ipmiapi_impl())){
		BIOSSET_LOG("DEBUG - wrong initialization of ipmiapi object! /n",0);
		return bios_set_handler_ns::ERR_IPMIAPI_NOT_EXIST;
	}
        result_ = ipmi_api->ipmiapi_init(device_name);
	if(result_ != bios_set_handler_ns::ERR_NO_ERROR){
		BIOSSET_LOG("DEBUG - ipmi_api->ipmiapi_init(...! - result %d /n" , result_);
		return bios_set_handler_ns::ERR_IPMIAPI_NOT_INIZIALIZED;
	}
	return bios_set_handler_ns::ERR_NO_ERROR;
}

int  bios_set_handler::get_bios_pointer(int slot, /*char *magazine,*/ int & bios_value){
	int result_ = bios_set_handler_ns::ERR_NO_ERROR;

	if(_cmd_exec_mode == bios_set_ns::SNMP_MODE){
		bios_set_snmpmanager snmp_manager;
		if (const int ret_code = snmp_manager.initialize(_switch_boards, isCBA)) {
			BIOSSET_LOG( "ERROR - bios_set_handler:: snmp_get_bios_pointer () - error in snmp_manager.initialize (error_code == %d)\n", ret_code);
			return set_command_result(ret_code);
		}

		if((result_ = snmp_manager.GetBIOSPointer(slot, bios_value)) == bs_snmpmanager_ns::ERR_NO_ERROR){
			_bios_pointer = bios_value;
		}
		BIOSSET_LOG( "DEBUG - bios_set_handler:: snmp_manager.GetBIOSPointer() - ret_code == %d\n", result_);
		snmp_manager.close();
	}
	else{
		if(!ipmi_api)
			return set_command_result(bios_set_handler_ns::ERR_IN_EXECUTION);
		unsigned int val = 2;
		if((ipmi_api->get_default_BIOS_pointer(& val)) < 0)
			result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
		BIOSSET_LOG("DEBUG -  bios_set_handler::get_bios_pointer - ipmi_api->get_default_BIOS_pointer(... ! - result %d\n", val);
		if (val == 2)
		{
			result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
			return set_command_result(result_);
		}
		acs_apbm_ipmiapi_ns::ipmi_fwinfo ipmi_fwinfo;
		if(ipmi_api->get_ipmi_fwinfo (& ipmi_fwinfo) !=0)
		{
			result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
			return set_command_result(result_);
		}
		//TODO CHECK IPMI REVISION
		//::printf("MINOR: (%d) - MAJOR: (%d) \n\n",ipmi_fwinfo.rev_minor, ipmi_fwinfo.rev_major);

		bios_value = val - 48;
		_bios_pointer = bios_value;

		result_ = bios_set_handler_ns::ERR_NO_ERROR;
	}
	return set_command_result(result_);
}

int  bios_set_handler:: get_bios_image(int slot, /*char *magazine,*/ int & bios_image){
	int result_ = 0;

	if(_cmd_exec_mode == bios_set_ns::SNMP_MODE){
		bios_set_snmpmanager snmp_manager;
		if (const int ret_code = snmp_manager.initialize(_switch_boards, isCBA)) {
			BIOSSET_LOG( "ERROR - bios_set_handler:: GetBIOSDefaultImage() - error IN snmp_manager.initialize error = %d\n", ret_code);
			return set_command_result(ret_code);
		}

		if((result_ = snmp_manager.GetBIOSDefaultImage(slot, bios_image)) == bs_snmpmanager_ns::ERR_NO_ERROR){
			_bios_default_image = bios_image;
		}
		BIOSSET_LOG("DEBUG - bios_set_handler:: snmp_manager.GetBIOSDefaultImage() - ret_code == %d\n", result_);
		snmp_manager.close();
	}
	else{
			if(!ipmi_api)
				return set_command_result(bios_set_handler_ns::ERR_IN_EXECUTION);
			unsigned int val = 2; //bios image only 0-1 value
			if((ipmi_api->get_default_BIOS_image(& val)) < 0)
				result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
			BIOSSET_LOG("DEBUG - bios_set_handler:: get_bios_image - ipmi_api->get_default_BIOS_image(... ! - result %d\n", val);
			if (val == 2)
			{
				result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
				return set_command_result(result_);
			}
			acs_apbm_ipmiapi_ns::ipmi_fwinfo ipmi_fwinfo;
			if(ipmi_api->get_ipmi_fwinfo (& ipmi_fwinfo) !=0)
			{
				result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
				return set_command_result(result_);
			}
			//TODO CHECK IPMI REVISION
			//::printf("MINOR: (%d) - MAJOR: (%d) \n\n",ipmi_fwinfo.rev_minor, ipmi_fwinfo.rev_major);
			bios_image = val - 48;
			_bios_default_image = bios_image;
			result_ = bios_set_handler_ns::ERR_NO_ERROR;
		}
	return set_command_result(result_);
}


int bios_set_handler::set_bios_image(int slot, /*char *magazine,*/ int  bios_image){

	int result_ = 0;
	bool gep5_gep7;
	/*if(!_api_imp && !(_api_imp = new (std::nothrow) acs_apbm_api_imp())){
                BIOSSET_LOG("DEBUG - wrong initialization of apbm_api_imp object! /n",0);
                return bios_set_handler_ns::ERR_IPMIAPI_NOT_EXIST;
        }
        gep4_gep5 = _api_imp->is_gep4_gep5();*/

	acs_apbm_api api; 
	gep5_gep7 = api.is_gep5_gep7();
	
	if(_cmd_exec_mode == bios_set_ns::SNMP_MODE)
	{
		bios_set_snmpmanager snmp_manager;
		if (const int ret_code = snmp_manager.initialize(_switch_boards, isCBA))
		{
			BIOSSET_LOG( "ERROR - bios_set_handler:: SetBIOSDefaultImage() - error IN snmp_manager.initialize error = %d\n", ret_code);
			return set_command_result(ret_code);
		}
	
		oid oid_name_scx[]= { ACS_APBM_SCX_OID_BLADE_LED_TYPE, slot };
       		oid oid_name_scb[]= { ACS_APBM_SCB_OID_XSHMC_BOARD_LED_TYPE, slot };
	        oid * oid_name = 0;
       		size_t oid_name_size = 0;
        	if(isCBA)
        	{
	                oid_name = oid_name_scx;
               		oid_name_size = BS_ARRAY_SIZE(oid_name_scx);
       		}
        	else
	        {
               		oid_name = oid_name_scb;
       	        	oid_name_size = BS_ARRAY_SIZE(oid_name_scb);
	        }
		int32_t blade_led_type;
		int ret_value;
		ret_value=snmp_manager.sendGetRequest(oid_name, oid_name_size,blade_led_type);
		if(ret_value<0)
		{
			BIOSSET_LOG( "ERROR - bios_set_handler::  - error IN snmp_manager.sendGetRequest() error = %d\n", ret_value);	
		}
		BIOSSET_LOG("BIOS_SET: SNMP blade_led_type ---- %d \n",blade_led_type); 
		if((gep5_gep7) && (blade_led_type == acs_apbm_snmp::BLADE_LED_TYPE_ON))
		{
			
			result_ = snmp_manager.SetBIOSDefaultImage(slot, bios_image);
			
			BIOSSET_LOG("DEBUG - bios_set_handler:: snmp_manager.SetBIOSDefaultImage() - ret_code == %d\n", result_);
			
			oid oid_scbrp_mia_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_YELLOW_LED, slot };
			oid oid_scx_mia_led [] = { ACS_APBM_SCX_OID_BLADECOMMON_YELLOW_LED, slot };

			if(isCBA )
 			{
				oid_name = oid_scx_mia_led;
				oid_name_size = BS_ARRAY_SIZE(oid_scx_mia_led);
	 		}
	 		else
 			{
        			oid_name = oid_scbrp_mia_led,
				oid_name_size = BS_ARRAY_SIZE(oid_scbrp_mia_led);
			}

			if(bios_image == 0)
			{
				
       	               		BIOSSET_LOG("BIOS_SET COMMAND... SETTING YELLOW LED TO FAST BLINK using SNMP COMMUNICATION on slot %d",slot);
				ret_value=snmp_manager.sendSetRequest(oid_name,oid_name_size,acs_apbm_snmp::axe::eth::shelf::FOUR_LED_STATUS_FASTBLINK );
		           	//set led status to fastblink  while setting the bios_image to fallback
       		        	if(ret_value<0)
               			{
                       			BIOSSET_LOG( "ERROR - UNABLE TO SET YELLOW LED TO FASTBLINK bios_set_handler::  - error IN snmp_manager.sendSetRequest() error = %d\n", ret_value);
                		}
			}		
			else if (bios_image == 1)
			{
				//set the yellow led to OFF when the bios_image is UPG
				
       	               		BIOSSET_LOG("BIOS_SET COMMAND... SETTING YELLOW LED TO OFF using SNMP COMMUNICATION on slot %d",slot);
				ret_value=snmp_manager.sendSetRequest(oid_name,oid_name_size,acs_apbm_snmp::axe::eth::shelf::LED_STATUS_OFF );
				if(ret_value<0)
                                {
                                        BIOSSET_LOG( "ERROR - UNABLE TO SET YELLOW LED TO OFF bios_set_handler::  - error IN snmp_manager.sendSetRequest() error = %d\n", ret_value);
                                }
			}
			


		}
               	else
               	{
			result_ = snmp_manager.SetBIOSDefaultImage(slot, bios_image);

			BIOSSET_LOG("DEBUG - bios_set_handler:: snmp_manager.SetBIOSDefaultImage() - ret_code == %d\n", result_);
       	        }

		snmp_manager.close();
	}
	else
	{
		if(!ipmi_api)
			return set_command_result(bios_set_handler_ns::ERR_IN_EXECUTION);

	
		if((ipmi_api->set_default_BIOS_image(bios_image)) < 0)
		{
			BIOSSET_LOG( "ERROR - set_default_BIOS_image(...  - result %d \n",result_);
			result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
			return set_command_result(result_);
		}
		BIOSSET_LOG( "DEBUG - set_default_BIOS_image(...  - result %d \n",result_);
		 if(gep5_gep7)
                {
                        if(bios_image == 0)
                        {
                                BIOSSET_LOG("BIOS_SET COMMAND... SETTING YELLOW LED TO FAST BLINK using IPMI IMPLEMENTATION on slot %d \n",slot);
                                if(ipmi_api->set_led(acs_apbm_ipmiapi_ns::YELLOW_4LED,C_4LED_STATUS_FAST_BLINK)<0)
                                BIOSSET_LOG("BIOS_SET COMMAND... SETTING YELLOW LED TO FAST BLINK FAILED using IPMI IMPLEMENTATION on slot %d \n",slot);
                        }
                        if(bios_image ==1)
                        {
                                BIOSSET_LOG("BIOS_SET COMMAND... SETTING YELLOW LED TO OFF using IPMI IMPLEMENTATION on slot %d \n",slot);
                                if(ipmi_api->set_led(acs_apbm_ipmiapi_ns::YELLOW_4LED,C_4LED_STATUS_OFF)<0)
                                BIOSSET_LOG("BIOS_SET COMMAND... SETTING YELLOW LED TO OFF FAILED using IPMI IMPLEMENTATION on slot %d \n",slot);
			}
                }


		result_ = bios_set_handler_ns::ERR_NO_ERROR;
	}
	return set_command_result(result_);
}

int bios_set_handler::set_bios_pointer(int slot, /*char *magazine,*/ int bios_pointer){

	int result_ = bios_set_handler_ns::ERR_NO_ERROR;

	if(_cmd_exec_mode == bios_set_ns::SNMP_MODE){
		bios_set_snmpmanager snmp_manager;
		if (const int ret_code = snmp_manager.initialize(_switch_boards, isCBA)) {
			BIOSSET_LOG( "ERROR - bios_set_handler:: set_bios_pointer() - error in snmp_manager.initialize <error_code == %d>\n", ret_code);
			return set_command_result(ret_code);
		}

		result_ = snmp_manager.SetBIOSPointer(slot, bios_pointer);
		BIOSSET_LOG( "DEBUG - bios_set_handler:: snmp_manager.SetBIOSPointer() - ret_code == %d\n", result_);
		snmp_manager.close();
	}
	else
	{
		if(!ipmi_api)
			return set_command_result(bios_set_handler_ns::ERR_IN_EXECUTION);
		unsigned int val = bios_pointer;
		BIOSSET_LOG( "enterint in set_bios_pointer(...",0);
		if((result_ = ipmi_api->set_default_BIOS_pointer(val)) < 0){
			BIOSSET_LOG( "ERROR - set_default_BIOS_pointer(...  - result %d \n",result_);
			result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
			return set_command_result(result_);
		}
		BIOSSET_LOG( "DEBUG - set_default_BIOS_pointer(...  - result %d \n",result_);
		result_ = bios_set_handler_ns::ERR_NO_ERROR;
	}
	return set_command_result(result_);
}

int bios_set_handler::send_set_ipmi_command(const char * ipmi_cmd, const char * opt, int value){

        int result_ = bios_set_handler_ns::ERR_NO_ERROR;
        char tmp_cmd[IPMI_CMD_MAX_SIZE] = {0};
        // create command with option
        ::snprintf(tmp_cmd, IPMI_CMD_MAX_SIZE, "%s %s %d",ipmi_cmd, opt, value);
        int pipefd[2];
        pipe(pipefd);
        if (fork() == 0)
        {
            close(pipefd[0]);    // close reading end in the child

            uid_t my_uid = getuid();
            setuid (0);  //root permission

            dup2(pipefd[1], STDOUT_FILENO);  // send stdout to the pipe
            dup2(pipefd[1], STDERR_FILENO);  // send stderr to the pipe

            close(pipefd[1]);    // this descriptor is no longer needed

            execlp("sh", "sh", "-c", tmp_cmd, NULL);
            setuid( my_uid );
        }
        else
        {
            // parent
            char buffer[256] = {0};
            char tmp[32] = {0};

            close(pipefd[1]);  // close the write end of the pipe in the parent

            while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
            {
               //::printf("str %s", buffer);
               if(::sscanf(buffer, "%s %*s", tmp) && (strcmp(tmp, "ERROR:") == 0))
               {
                   result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
                   return set_command_result(result_);
               }
            }
        }
        return set_command_result(result_);
}
int bios_set_handler::send_get_ipmi_command(const char * ipmi_cmd, const char * opt, int & value)
{
      int result_ = bios_set_handler_ns::ERR_NO_ERROR;

      char tmp_cmd[IPMI_CMD_MAX_SIZE] = {0};

      // get command
      // create command with option
      ::snprintf(tmp_cmd, IPMI_CMD_MAX_SIZE, "%s %s", ipmi_cmd, opt);

      FILE  * fd_ipmi = popen(tmp_cmd,"r");
      if (fd_ipmi < 0)
      {
           result_ = bios_set_handler_ns::ERR_IN_EXECUTION;
           ::pclose(fd_ipmi);
           return set_command_result(result_);
      }
      char * line = NULL;
      size_t len = 0;
      ssize_t read;
      int res = 0;

      while ((read = getline(&line, &len, fd_ipmi)) != -1)
      {
          if( (res = ::sscanf(line, "Default BIOS image is: %d", & value)) ) break;
          if( (res = ::sscanf(line, "BIOS pointer is: %d", & value)) ) break;
      }
        //printf("result = %d,  bios found: %d \n", res, value);

        if (line)
            free(line);

      ::pclose(fd_ipmi);


  return set_command_result(result_);
}

int bios_set_handler::get_switch_boards_info_from_cs ( char *magazine)
{
	BIOSSET_LOG( "DEBUG - bios_set_handler:: get_switch_boards_info_from_cs (magazine == '%s')  --->\n", magazine );

	uint32_t input_mag_num, mag_num, local_mag_num;
	if(*magazine && ip_format_to_uint32 (input_mag_num, magazine) < 0){
			BIOSSET_LOG( "ERROR - bios_set_handler:: get_switch_boards_info_from_cs() - error in converting magazine parameter from string to integer.\n", 0);
			return bios_set_handler_ns::ERR_IN_EXECUTION;
	}

        int error_code = get_local_magazine_from_cs(local_mag_num);
	//if ((*magazine == NULL_STRING ) && (( error_code = get_local_magazine_from_cs(local_mag_num))< 0)) {
	if (error_code < 0) {
		BIOSSET_LOG( "ERROR - bios_set_handler:: get_switch_boards_info_from_cs() - error in getting local magazine = %d\n", error_code);
		return bios_set_handler_ns::ERR_CS_GET_MAGAZINE;
	}

	BIOSSET_LOG( "DEBUG - bios_set_handler:: get_switch_boards_info_from_cs() - local magazine found = %u\n", local_mag_num);

	if (!isCBA  && (*magazine != NULL_STRING && input_mag_num != local_mag_num)) return bios_set_handler_ns::ERR_LOCAL_MAGAZINE_NUMBER_EXEPECTED;



#ifdef CSNOTWORKINGFORCBA
	if(isCBA){
		_switch_boards[0].slot_position = 0;
		_switch_boards[0].magazine = local_mag_num;
		::strncpy(_switch_boards[0].ipna_str, "192.168.169.56", IPNADDRESS_MAXLEN);
		::strncpy(_switch_boards[0].ipnb_str, "192.168.170.56", IPNADDRESS_MAXLEN);
		_switch_boards[1].slot_position = 25;
		_switch_boards[1].magazine = local_mag_num;
		::strncpy(_switch_boards[1].ipna_str, "192.168.169.57", IPNADDRESS_MAXLEN);
		::strncpy(_switch_boards[1].ipnb_str, "192.168.170.57", IPNADDRESS_MAXLEN);
		return 0;
	}
#endif
	bios_set_ns::EnvironmentType m_environment;
        m_environment = bios_set_ns::getEnvironment();

	mag_num = ((*magazine != NULL_STRING)? input_mag_num: local_mag_num);

	ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
	if (!hwc) { // ERROR: creating HWC CS instance
		return bios_set_handler_ns::ERR_CS_CREATE_HWC_INSTANCE;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_CS_API::deleteHWCInstance(hwc);
		return  bios_set_handler_ns::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;;
	}
	uint16_t switch_board_fbn= -1;
	if ((m_environment == bios_set_ns::MULTIPLECP_SMX)||(m_environment == bios_set_ns::SINGLECP_SMX)) {
		switch_board_fbn=ACS_CS_API_HWC_NS::FBN_SMXB;
	} else {
		switch_board_fbn = isCBA ? ACS_CS_API_HWC_NS::FBN_SCXB : ACS_CS_API_HWC_NS::FBN_SCBRP;
	}
	//uint16_t switch_board_fbn = isCBA ? 0 : ACS_CS_API_HWC_NS::FBN_SCBRP;
	// search criteria
	bs->reset();
	bs->setFBN(switch_board_fbn);
	bs->setMagazine(mag_num);


	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return bios_set_handler_ns::ERR_CS_GET_BOARD_ID;
	}



	if (board_list.size() <= 0) { // NO switch board found in the system
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return bios_set_handler_ns::ERR_CS_NO_SWITCH_BOARDS;
	}
	// Retrieving switch board info

	uint16_t slot_position;
	uint32_t ipn_addresses[2];
	// Retrieving information for each switch board in my magazine
	for (size_t i = 0; i < board_list.size(); ++i) {
		BoardID board_id = board_list[i];

		if ((cs_call_result = hwc->getSlot(slot_position, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting the board slot
			ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
			ACS_CS_API::deleteHWCInstance(hwc);
			return  bios_set_handler_ns::ERR_CS_GET_SLOT;
		}

		ACS_CS_API_NS::CS_API_Result get_ipna_result = hwc->getIPEthA(ipn_addresses[0], board_id);
		ACS_CS_API_NS::CS_API_Result get_ipnb_result = hwc->getIPEthB(ipn_addresses[1], board_id);
		if (get_ipna_result != ACS_CS_API_NS::Result_Success &&
				get_ipnb_result != ACS_CS_API_NS::Result_Success) {
			// ERROR: getting the IPN addresses
			ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
			ACS_CS_API::deleteHWCInstance(hwc);
			return bios_set_handler_ns::ERR_CS_GET_IP_ETH;
		}

		// Converting IP addresses in the dotted-quad format "ddd.ddd.ddd.ddd"
		char ip_str[2][IPNADDRESS_MAXLEN];
		for (int j = 0; j < 2; ++j) {
			uint32_t address = htonl(ipn_addresses[j]);
			if (const int call_result = uint32_to_ip_format(ip_str[j], address)){
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
				ACS_CS_API::deleteHWCInstance(hwc);
				return  bios_set_handler_ns::ERR_IN_EXECUTION;;
			}
		}
		_switch_boards[i].slot_position = slot_position;
		_switch_boards[i].magazine = mag_num;
		::strncpy(_switch_boards[i].ipna_str, ip_str[0], IPNADDRESS_MAXLEN);
		::strncpy(_switch_boards[i].ipnb_str, ip_str[1], IPNADDRESS_MAXLEN);
	}
	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
	ACS_CS_API::deleteHWCInstance(hwc);
	return bios_set_handler_ns::ERR_NO_ERROR;
}


int bios_set_handler::get_board_fbn_id(uint16_t & fbn_id, int slot, char * magazine)
{
	BIOSSET_LOG("DEBUG - bios_set_handler::get_board_fbn() --->  slot == '%d', magazine == '%s'. \n", slot, magazine ? magazine : "NULL");

	uint32_t mag_num = 0;
	char mag_addr[16];

	if (!magazine)
	{
		BIOSSET_LOG( "ERROR - bios_set_handler::get_board_fbn() - error in execution: uninitialized magazine string.\n", 0);
		return bios_set_handler_ns::ERR_IN_EXECUTION;
	}

	// if magazine parameter has not been provided by caller, use local magazine
	if(!*magazine) { // use local magazine
		int error_code = get_local_magazine_from_cs(mag_num);
		if(error_code < 0)  {
			BIOSSET_LOG( "ERROR - bios_set_handler::get_board_fbn() - error in getting local magazine = %d\n", error_code);
			return bios_set_handler_ns::ERR_CS_GET_MAGAZINE;
		}

		BIOSSET_LOG( "DEBUG - bios_set_handler::get_board_fbn() - local magazine found = %u\n", mag_num);
		if(uint32_to_ip_format(mag_addr,mag_num) < 0){
		BIOSSET_LOG( "ERROR - bios_set_handler::get_board_fbn() - error in converting magazine parameter from integer to string.\n", 0);
                        return bios_set_handler_ns::ERR_IN_EXECUTION;
		}
		//magazine = mag_addr;
		strcpy(magazine , mag_addr);
	}
	else { // convert provided magazine parameter from IP format to number
		if(ip_format_to_uint32(mag_num, magazine) < 0) {
			BIOSSET_LOG( "ERROR - bios_set_handler::get_board_fbn() - error in converting magazine parameter from string to integer.\n", 0);
			return bios_set_handler_ns::ERR_IN_EXECUTION;
		}
	}


	// get info from CS about the board identified by slot and magazine
	ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
	if (!hwc) { // ERROR: creating HWC CS instance
		return bios_set_handler_ns::ERR_CS_CREATE_HWC_INSTANCE;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_CS_API::deleteHWCInstance(hwc);
		return  bios_set_handler_ns::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}

	// search criteria
	bs->reset();
	bs->setMagazine(mag_num);
	bs->setSlot(slot);

	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return  bios_set_handler_ns::ERR_CS_GET_BOARD_ID;
	}

	if (board_list.size() <= 0) { // board not found in the system
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return bios_set_handler_ns::ERR_BOARD_NOT_FOUND;
	}

	if(board_list.size() != 1) {
		BIOSSET_LOG("DEBUG - bios_set_handler::get_board_fbn() - unexpected info received from CS : '%d' boards found ! \n", board_list.size());
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return bios_set_handler_ns::ERR_UNEXPECTED_SYSTEM_CONFIGURATION;
	}

	// Get board FBN
	uint16_t fbn_id_temp;
	if(hwc->getFBN(fbn_id_temp, board_list[0]) != ACS_CS_API_NS::Result_Success) {
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return bios_set_handler_ns::ERR_CS_GET_FBN;
	}

	fbn_id  = fbn_id_temp;
	BIOSSET_LOG("DEBUG - bios_set_handler::get_board_fbn() - board FBN is '%d'\n", fbn_id);

	ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
	ACS_CS_API::deleteHWCInstance(hwc);

	return bios_set_handler_ns::ERR_NO_ERROR;
}

int bios_set_handler::set_command_result(int err_code)
{
	int ret_code;
	switch (err_code)
	{
		case bios_set_handler_ns::ERR_NO_ERROR :
			ret_code = bios_set_ns::BS_NO_ERROR;
		break;
		case bios_set_handler_ns::ERR_IN_EXECUTION:
			ret_code = bios_set_ns::BS_GENERIC_ERROR;
		break;
		case bios_set_handler_ns::ERR_CS_CREATE_HWC_INSTANCE:
		case bios_set_handler_ns::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE:
			ret_code = bios_set_ns::BS_UNABLE_TO_CONNECT_TO_CS;
		break;
		case bios_set_handler_ns::ERR_CS_GET_BOARD_ID:
		case bios_set_handler_ns::ERR_CS_GET_SLOT:
		case bios_set_handler_ns::ERR_CS_GET_IP_ETH:
		case bios_set_handler_ns::ERR_CS_GET_MAGAZINE:
		case bios_set_handler_ns::ERR_CS_NO_SWITCH_BOARDS:
		case bios_set_handler_ns::ERR_CS_NO_APUB_BOARDS:
			ret_code = bios_set_ns::BS_CANNOT_GET_INFORMATION_FROM_CS;
			break;
		case bios_set_handler_ns::ERR_NO_ECHO:
			ret_code = bios_set_ns::BS_NO_ECHO;
			break;
		case bios_set_handler_ns::ERR_BOARD_NOT_FOUND:
			ret_code = bios_set_ns::BS_BOARD_NOT_FOUND;
		break;
		case bios_set_handler_ns::ERR_LOCAL_MAGAZINE_NUMBER_EXEPECTED:
			ret_code = bios_set_ns::ILLEGAL_OPT_IN_SYSTEM_CONFIG;
		break;
		case bs_snmpmanager_ns::ERR_SNMP_CREATE_PDU :
		case bs_snmpmanager_ns::ERR_SNMP_OPENING_SESSION:
		case bs_snmpmanager_ns::ERR_SNMP_PDU_ADD_VARIABLE_VALUE:
			ret_code = bios_set_ns::BS_SNMP_GENERIC_FAILURE;
		break;
		case bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_TIMEOUT:
			ret_code = bios_set_ns::BS_NO_RESPONSE_FROM_SNMP_AGENT;
		break;
		case bs_snmpmanager_ns::ERR_SNMP_SEND_SYNC_ERROR:
				ret_code = bios_set_ns::BS_SNMP_ERR_IN_SENDING_REQUEST;
		break;
		case bs_snmpmanager_ns::ERR_SNMP_PDU_GENERIC_ERR:
		case bs_snmpmanager_ns::ERR_SNMP_PDU_VARIABLE_BAD_TYPE:
		case bs_snmpmanager_ns::ERR_SNMP_PDU_OID_UNEXPECTED:
		case bs_snmpmanager_ns::ERR_SNMP_PDU_COMMAND_BAD_TYPE:
			ret_code = bios_set_ns::BS_SNMP_BAD_RESPONSE_RECEIVED;
		break;
		case bs_snmpmanager_ns::ERR_SNMP_PDU_RESOURCE_UNAVAIL:
			ret_code = bios_set_ns::BS_SNMP_RESOURCEUNAVAILABLE_ERR;
		break;
		case bs_snmpmanager_ns::ERR_SNMP_PDU_SET_RESOURCE_UNAVAIL:
					ret_code = bios_set_ns::BS_SNMP_SET_UNAVAIL_ERR;
		break;
		case bs_snmpmanager_ns::ERR_SNMP_GETTING_IPMI_MASTER:
			ret_code = bios_set_ns::BS_ERR_SNMP_GETTING_IPMI_MASTER;
		break;
		case bs_ironside_ns::IRONSIDE_FAILURE:
		        ret_code = bios_set_ns::BS_IRONSIDE_FAILURE;
		break;
		default:
			ret_code = bios_set_ns::BS_ERR_UNKNOWN;
		break;
	}

	return ret_code;
}





int bios_set_handler::uint32_to_ip_format (char (& value_ip) [IPNADDRESS_MAXLEN], uint32_t value) {
	// Converting value address into IP dotted-quad format "ddd.ddd.ddd.ddd"
	in_addr addr;
	addr.s_addr = value;
	if (!::inet_ntop(AF_INET, &addr, value_ip, IPNADDRESS_MAXLEN)) { // ERROR: in conversion
		value_ip[0] = 0;
		return -1;
	}
	return 0;
}

int bios_set_handler::ip_format_to_uint32 (uint32_t & value, const char * value_ip) {
	in_addr addr;

	if (const int call_result = ::inet_pton(AF_INET, value_ip, &addr) <= 0) { // ERROR: in conversion
		return -1;
	}

	value = addr.s_addr;

	return 0;
}

int bios_set_handler::get_local_magazine_from_cs (uint32_t & magazine) {

	BIOSSET_LOG("DEBUG - bios_set_handler::get_local_magazine_from_cs() ---> \n", 0);

	ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
	if (!hwc) { // ERROR: creating HWC CS instance
		return bios_set_handler_ns::ERR_CS_CREATE_HWC_INSTANCE;
	}

	ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
	if (!bs) {
		ACS_CS_API::deleteHWCInstance(hwc);
		return  bios_set_handler_ns::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
	}

	// search criteria
	bs->reset();
	bs->setSysType(ACS_CS_API_HWC_NS::SysType_AP);
	bs->setFBN(ACS_CS_API_HWC_NS::FBN_APUB);

	// Now I can search
	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result cs_call_result = hwc->getBoardIds(board_list, bs);
	if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return  bios_set_handler_ns::ERR_CS_GET_BOARD_ID;
	}
	if (board_list.size() <= 0) { // NO APUB board found in the system
		ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		ACS_CS_API::deleteHWCInstance(hwc);
		return bios_set_handler_ns::ERR_CS_NO_APUB_BOARDS;
	}
	// Retrieving ap board info
	BIOSSET_LOG("DEBUG - bios_set_handler::get_local_magazine_from_cs() - %d APUB boards found by CS\n", board_list.size());

	uint32_t ipn_addresses[2]= {0};
	int my_ap_index = -1;
	int check_socket = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;

	// Retrieving information for each AP board in my magazine
	for (size_t board_idx = 0; board_idx < board_list.size(); ++board_idx) {
		BoardID board_id = board_list[board_idx];

		if(((cs_call_result =hwc->getIPEthA(ipn_addresses[0], board_id)) != ACS_CS_API_NS::Result_Success) ||
					((cs_call_result =hwc->getIPEthB(ipn_addresses[1], board_id))!= ACS_CS_API_NS::Result_Success) ) {
				// ERROR: getting the IPN addresses
			ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
			ACS_CS_API::deleteHWCInstance(hwc);
			return bios_set_handler_ns::ERR_CS_GET_IP_ETH;
		}
		my_ap_index = -1;
		for (int ipn = 0; (my_ap_index < 0) && (ipn < CONFIG_NETWORKS_SUPPORTED); ++ipn){
			addr.sin_addr.s_addr = htonl(ipn_addresses[ipn]);
			bind(check_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr_in)) || (my_ap_index = board_idx);
		}
		if (my_ap_index > -1)  break;
	}
	::close(check_socket);
	//  now get  my magazine address
	BIOSSET_LOG ("DEBUG - bios_set_handler::get_local_magazine_from_cs() - local APUB  found my_ap_index == %d!\n", my_ap_index);
	BoardID board_id = board_list[my_ap_index];
	uint32_t mag_num = ~0U;
	if ((cs_call_result = hwc->getMagazine(mag_num, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting magazine info
				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
			ACS_CS_API::deleteHWCInstance(hwc);
			return bios_set_handler_ns::ERR_CS_GET_MAGAZINE;
	}
	magazine = mag_num;
	BIOSSET_LOG("DEBUG - bios_set_handler::get_local_magazine_from_cs() - local magazine found mag_num == %u!\n", mag_num);
	return bios_set_handler_ns::ERR_NO_ERROR;
}




int  bios_set_handler:: set_ironside_bios_pointer(int slot, char *magazine, int & bios_pointer){
	int result_ = 0;
	acs_apbm_cmd_ironsidemanager *ironsideManager = new acs_apbm_cmd_ironsidemanager();
	if(_cmd_exec_mode == bios_set_ns::IRONSIDE_MODE)
	{

		result_ = set_ironside_bios_pointer(ironsideManager,magazine,slot, bios_pointer);
		BIOSSET_LOG("DEBUG - bios_set_handler::ironsidemanager.setBiosPointer() - ret_code == %d\n", result_);
	}

	delete (ironsideManager);
	return set_command_result(result_);

}

int  bios_set_handler:: set_ironside_bios_image(int slot, char *magazine, int & bios_image){
	int result_ = 0;
	acs_apbm_cmd_ironsidemanager *ironsideManager = new acs_apbm_cmd_ironsidemanager();
	if(_cmd_exec_mode == bios_set_ns::IRONSIDE_MODE)
	{

		result_ = set_ironside_bios_default_image(ironsideManager,magazine,slot, bios_image);
		BIOSSET_LOG("DEBUG - bios_set_handler::ironsidemanager.setBiosDefaultImage() - ret_code == %d\n", result_);
	}

	delete (ironsideManager);
	return set_command_result(result_);
}



int  bios_set_handler:: get_ironside_bios_image(int slot, char *magazine, int & bios_image){
	int result_ = 0;
	acs_apbm_cmd_ironsidemanager *ironsideManager = new acs_apbm_cmd_ironsidemanager();

	if(_cmd_exec_mode == bios_set_ns::IRONSIDE_MODE)
	{
		result_ = get_ironside_bios_default_image(ironsideManager,magazine,slot, bios_image);

		BIOSSET_LOG("DEBUG - bios_set_handler:: ironsidemanager.GetIronsideBIOSDefaultImage() - ret_code == %d\n", result_);

	}

	delete (ironsideManager);
	return set_command_result(result_);
}


int  bios_set_handler:: get_ironside_bios_pointer(int slot, char *magazine, int & bios_pointer)
{
	int result_ = bios_set_handler_ns::ERR_NO_ERROR;
	acs_apbm_cmd_ironsidemanager *ironsideManager = new acs_apbm_cmd_ironsidemanager();
	if(_cmd_exec_mode == bios_set_ns::IRONSIDE_MODE)
	{
		result_ = get_ironside_bios_pointer(ironsideManager,magazine,slot, bios_pointer);

		//std::cout<<" result_= "<<result_<<std::endl;
		BIOSSET_LOG("DEBUG - bios_set_handler:: ironsidemanager.GetIronsideBIOSPointer() - ret_code == %d\n", result_);

	}

	delete (ironsideManager);
	return set_command_result(result_);
}

int bios_set_handler::get_ironside_bios_default_image(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int &bios_image)
{

  string shelf_id("");
  int result =bs_ironside_ns::ERR_NO_ERROR;
  stringstream phyAddr;
  stringstream slotstr ;
  slotstr << slot;
  int physAddr = mag_addr_to_physical_addr(magazine);
  phyAddr << physAddr;
 
  result = ironsidemanager->getShelfID(phyAddr.str(),shelf_id);
  switch (result)
  {
  case bs_ironside_ns::ERR_IN_EXECUTION:
	return result;
  case bs_ironside_ns::IRONSIDE_FAILURE:
	return result;
  case bios_set_handler_ns::ERR_BOARD_NOT_FOUND:
        return result;
  default: break;
  }
  result = ironsidemanager->getBiosDefaultImage(shelf_id,slot,bios_image);

  return result;
}

int bios_set_handler::get_ironside_bios_pointer(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int &bios_pointer)
{

  string shelf_id("");
  int result =bs_ironside_ns::ERR_NO_ERROR;
  stringstream phyAddr;
  stringstream slotstr ;
  slotstr << slot;
  int physAddr = mag_addr_to_physical_addr(magazine);
  phyAddr << physAddr;


  result = ironsidemanager->getShelfID(phyAddr.str(),shelf_id);
  switch (result)
  {
  case bs_ironside_ns::ERR_IN_EXECUTION:
        return result;
  case bs_ironside_ns::IRONSIDE_FAILURE:
        return result;
  case bios_set_handler_ns::ERR_BOARD_NOT_FOUND:
        return result;
  default: break;
  }

  result = ironsidemanager->getBiosPointer(shelf_id,slot,bios_pointer);

  return result;
}

int bios_set_handler::set_ironside_bios_default_image(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int bios_image)
{
  string shelf_id("");
  int result =bs_ironside_ns::ERR_NO_ERROR;
  stringstream phyAddr;
  stringstream slotstr ;
  slotstr << slot;
  int physAddr = mag_addr_to_physical_addr(magazine);
  phyAddr << physAddr;


  result = ironsidemanager->getShelfID(phyAddr.str(),shelf_id);
  switch (result)
  {
  case bs_ironside_ns::ERR_IN_EXECUTION:
        return result;
  case bs_ironside_ns::IRONSIDE_FAILURE:
        return result;
  case bios_set_handler_ns::ERR_BOARD_NOT_FOUND:
        return result;
  default: break;
  }

	
  result = ironsidemanager->setBiosDefaultImage(shelf_id,slot,bios_image);

  return result;
}

int bios_set_handler::set_ironside_bios_pointer(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int bios_pointer)
{
  string shelf_id("");
  int result =bs_ironside_ns::ERR_NO_ERROR;
  stringstream phyAddr;
  stringstream slotstr ;
  slotstr << slot;
  int physAddr = mag_addr_to_physical_addr(magazine);
  phyAddr << physAddr;


  result = ironsidemanager->getShelfID(phyAddr.str(),shelf_id);
  switch (result)
  {
  case bs_ironside_ns::ERR_IN_EXECUTION:
        return result;
  case bs_ironside_ns::IRONSIDE_FAILURE:
        return result;
  case bios_set_handler_ns::ERR_BOARD_NOT_FOUND:
        return result;
  default: break;
  }
 
  result = ironsidemanager->setBiosPointer(shelf_id,slot,bios_pointer);

  return result;
  }

int bios_set_handler::mag_addr_to_physical_addr(const char* shelfaddr)
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
