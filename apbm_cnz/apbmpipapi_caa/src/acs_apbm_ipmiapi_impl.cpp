#include <sys/epoll.h>

#include "ACS_APGCC_CommonLib.h" //GetHwInfo()

#include "ace/Time_Value_T.h"
#include "ace/Monotonic_Time_Policy.h"
#include "ace/Process_Mutex.h"

#include "eri_ipmi.h"
#include "acs_apbm_trace.h"
#include <stdio.h>
#include "acs_apbm_ipmiapi_common.h"
#include "acs_apbm_ipmiconstants.h"
#include "acs_apbm_ipmiapi_impl.h"
#include "acs_apbm_ipmiapi_level2_impl.h"

static ACE_Process_Mutex * process_mutex = 0;

ACS_APBM_TRACE_DEFINE(acs_apbm_ipmiapi);

acs_apbm_ipmiapi_impl:: acs_apbm_ipmiapi_impl():
_ipmiapi_state (acs_apbm_ipmiapi_ns::IPMIAPI_NOT_INITIALIZED), _communication_mode(acs_apbm_ipmiapi_ns::UNKNOWN_MODE),
_l2_impl(0),_ipmi_fd (-1), _epoll_fd (-1)
{
}

acs_apbm_ipmiapi_impl:: ~acs_apbm_ipmiapi_impl()
{
}

int acs_apbm_ipmiapi_impl::ipmiapi_init(const char* device_name){
    ACS_APBM_TRACE_FUNCTION;

    if(_ipmiapi_state == acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
    	return acs_apbm_ipmiapi_ns::ERR_API_ALREADY_INITIALIZED;
	char dev_name[IPMIAPI_DEVICE_NAME_MAX_LEN] = {0};

	if (device_name) {
		if(!strncmp(device_name, IPMIAPI_DEVICE_NAME_PREFIX, sizeof(IPMIAPI_DEVICE_NAME_PREFIX) - 1))
			strncpy(dev_name, device_name, IPMIAPI_DEVICE_NAME_MAX_LEN - 1);
		else {
			ACS_APBM_TRACE_MESSAGE("Error: invalid device name: %s", device_name);
			return acs_apbm_ipmiapi_ns::ERR_INVALID_DEVICE_NAME;
		}
	}
	else strcpy(dev_name, IPMIAPI_DEFAULT_DEVICE_NAME);

	ACS_APBM_TRACE_MESSAGE("using device name: %s",dev_name);

	if(::access(dev_name, F_OK) == -1) {
		ACS_APBM_TRACE_MESSAGE("Error: the device '%s' is not accessible: errno == %d", dev_name, errno);
		return acs_apbm_ipmiapi_ns::ERR_INVALID_DEVICE_NAME;
	}

	if(strcmp(dev_name, IPMIAPI_DEFAULT_DEVICE_NAME) !=0 ){
		_l2_impl = new (std::nothrow) acs_apbm_ipmiapi_level2_impl(dev_name);

		if (_l2_impl == NULL) return acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE;

		_communication_mode = acs_apbm_ipmiapi_ns::SERIAL_MODE;
	}
	else {
		_communication_mode = acs_apbm_ipmiapi_ns::DRIVER_MODE;

		int call_result = open_ipmi_driver(device_name);
		if(call_result)
		{
			ACS_APBM_TRACE_MESSAGE("Error: unable to initialize IPMI DRIVER DEVICE ('%s') ", dev_name);
			return call_result;
		}
	}

	ACS_APBM_TRACE_MESSAGE("communication mode: %s", _communication_mode == acs_apbm_ipmiapi_ns::SERIAL_MODE ? "SERIAL_MODE": "DRIVER_MODE");

    _ipmiapi_state = acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED;

    return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::ipmiapi_finalize(){
    ACS_APBM_TRACE_FUNCTION;

    if(_ipmiapi_state == acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
    {
    	 close(_epoll_fd);
    	 close(_ipmi_fd);
    	 delete _l2_impl;
    	_ipmiapi_state = acs_apbm_ipmiapi_ns::IPMIAPI_NOT_INITIALIZED;
    }

    return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::get_mac_address (acs_apbm_ipmiapi_ns::mac_adddress_info* mac_address_info) {
        ACS_APBM_TRACE_FUNCTION;

	int call_result;
	unsigned char response[SIZE_L3DATA_GETMACADDRESS_RESPONSE];

	if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	if(mac_address_info == 0)
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	call_result = _ipmifw_query(G_INFO,
						  C_MAC_ADDRESS,
						  SIZE_L3DATA_GETMACADDRESS_REQUEST,
						  NULL,
						  SIZE_L3DATA_GETMACADDRESS_RESPONSE,
						  response,
						  L3TIMEOUT_DEFAULT_MSEC);
	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	memcpy(mac_address_info->boot_mac, &response[0], acs_apbm_ipmiapi_ns::MACADDRESS_SIZE);
	memcpy(mac_address_info->eri_mac,  &response[acs_apbm_ipmiapi_ns::MACADDRESS_SIZE], acs_apbm_ipmiapi_ns::MACADDRESS_SIZE);
	mac_address_info->num = 16 * response[12] + response[13];

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::get_product_id (acs_apbm_ipmiapi_ns::product_id* product_id) {
        ACS_APBM_TRACE_FUNCTION;

	int call_result;
	unsigned char subset;
	unsigned char response[SIZE_L3DATA_GETPRODUCTID_1_RESPONSE] = {0};

	if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	if(product_id == 0)
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	memset(product_id, 0, sizeof(acs_apbm_ipmiapi_ns::product_id));

	subset = 0;
	call_result = _ipmifw_query(G_INFO,
						  C_PRODUCT_ID,
						  SIZE_L3DATA_GETPRODUCTID_REQUEST,
						  &subset,
						  SIZE_L3DATA_GETPRODUCTID_0_RESPONSE  ,
						  response,
						  L3TIMEOUT_DEFAULT_MSEC);
	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;
/*
	printf("response 1: ");
	for (int i =0; i< SIZE_L3DATA_GETPRODUCTID_1_RESPONSE;i++){
		printf("'%02x' ", response[i]);
	}
	printf("\n");
*/
	memcpy(product_id->product_number,
		   response + 1,
		   acs_apbm_ipmiapi_ns::PRODUCT_NUMBER_SIZE);
	memcpy(product_id->product_revision,
		   response + 1 + acs_apbm_ipmiapi_ns::PRODUCT_NUMBER_SIZE,
		   acs_apbm_ipmiapi_ns::PRODUCT_REVISION_SIZE);

	subset = 1;
	call_result = _ipmifw_query(G_INFO,
						  C_PRODUCT_ID,
						  SIZE_L3DATA_GETPRODUCTID_REQUEST,
						  &subset,
						  SIZE_L3DATA_GETPRODUCTID_1_RESPONSE,
						  response,
						  L3TIMEOUT_DEFAULT_MSEC);
	if(call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	memcpy(product_id->product_name,
		   response + 1,
		   acs_apbm_ipmiapi_ns::PRODUCT_NAME_SIZE);
	memcpy(product_id->product_date,
		   response + 1 + acs_apbm_ipmiapi_ns::PRODUCT_NAME_SIZE,
		   acs_apbm_ipmiapi_ns::PRODUCT_DATE_SIZE);
	memcpy(product_id->serial_number,
		   response + 1 + acs_apbm_ipmiapi_ns::PRODUCT_NAME_SIZE + acs_apbm_ipmiapi_ns::PRODUCT_DATE_SIZE,
		   acs_apbm_ipmiapi_ns::PRODUCT_SERIAL_NUMBER_SIZE);

	subset = 2;
	call_result = _ipmifw_query(G_INFO,
						  C_PRODUCT_ID,
						  SIZE_L3DATA_GETPRODUCTID_REQUEST,
						  &subset,
						  SIZE_L3DATA_GETPRODUCTID_2_RESPONSE,
						  response,
						  L3TIMEOUT_DEFAULT_MSEC);
	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	memcpy(product_id->vendor_name, response + 1, acs_apbm_ipmiapi_ns::PRODUCT_VENDOR_NAME_SIZE);
	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::enter_level (unsigned int level){
        ACS_APBM_TRACE_FUNCTION;

	if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	if(level > acs_apbm_ipmiapi_ns::APPL_OP_LEVEL)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER;

	unsigned char level_byte = static_cast<unsigned char> (level);

	int call_result = _ipmifw_query(G_SERVICE,
						  C_ENTER_LEVEL,
						  SIZE_L3DATA_ENTERLEVEL_REQUEST,
						  &level_byte,
						  SIZE_L3DATA_ENTERLEVEL_RESPONSE,
						  NULL,
						  L3TIMEOUT_DEFAULT_MSEC);

	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
			return call_result;

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::get_parameter_tableblock (unsigned int flash_type, unsigned int block_index, unsigned char* buffer){
        ACS_APBM_TRACE_FUNCTION;

	if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

  // check parameter value
	if (buffer == NULL || flash_type > acs_apbm_ipmiapi_ns::BOARD_IPMI_FW_TABLE || block_index > 255)
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	unsigned char response [SIZE_L3DATA_PTABLE_RESPONSE];

	unsigned char req_parameters[2];
	req_parameters[0] = static_cast<unsigned char>(flash_type);
	req_parameters[1] = static_cast<unsigned char>(block_index);

	int call_result = _ipmifw_query(G_SERVICE,
	                          C_PTABLE_BLOCK,
	                          SIZE_L3DATA_PTABLE_REQUEST,
	                          req_parameters,
	                          SIZE_L3DATA_PTABLE_RESPONSE,
	                          response,
	                          L3TIMEOUT_DEFAULT_MSEC);

	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	memcpy(buffer, response, SIZE_L3DATA_PTABLE_RESPONSE);
	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::get_ipmi_fwinfo (acs_apbm_ipmiapi_ns::ipmi_fwinfo* ipmi_fwinfo){
          ACS_APBM_TRACE_FUNCTION;

	  if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		  return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	  // check parameter value
	  if (ipmi_fwinfo == NULL)
		  return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	  unsigned char fw_info_raw[2];

	  int call_result = _ipmifw_query(G_INFO,
					  C_IPMI_FW_INFO,
					  SIZE_L3DATA_GETIPMIFWINFO_REQUEST,
					  NULL,
					  SIZE_L3DATA_GETIPMIFWINFO_RESPONSE,
					  fw_info_raw,
					  L3TIMEOUT_DEFAULT_MSEC);

		if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
			return call_result;

		//TODO: check if it is necessary to do this manipulation !!! perhaps we must only copy received data...
		ipmi_fwinfo->rev_major = fw_info_raw[0] & 0x7F;
		ipmi_fwinfo->rev_minor = (((fw_info_raw[1] & 0xF0) >> 4) % 10) * 10 +
	                            ((fw_info_raw[1] & 0x0F) % 10);

	    return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;

}

int acs_apbm_ipmiapi_impl::get_reset_line_state (unsigned char line_index, unsigned char* state){
         ACS_APBM_TRACE_FUNCTION;

	 if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
			  return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;
	 //check input parameter
	 if (state == NULL)
		 return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	 unsigned char response;

	 int call_result = _ipmifw_query(G_INFO,
	                           C_RESET_STATUS,
	                           SIZE_L3DATA_RESETSTATUS_REQUEST,
	                           &line_index,
	                           SIZE_L3DATA_RESETSTATUS_RESPONSE,
	                           &response,
	                           L3TIMEOUT_DEFAULT_MSEC);

	 if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
	 	return call_result;

	 *state = response >> 7;
	 return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::flash_mode_activate (unsigned int flash_type){
         ACS_APBM_TRACE_FUNCTION;

	 if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		 return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	 unsigned char f_type = static_cast<unsigned char> (flash_type);
	 //unsigned char f_type = flash_type % 256;
	 unsigned char response[SIZE_L3DATA_FLASHMODEACTIVATE_RESPONSE];

	 int call_result = _ipmifw_query(G_SERVICE,
					  C_FLASH_MODE_ACTIVATE,
					  SIZE_L3DATA_FLASHMODEACTIVATE_REQUEST,
					  &f_type,
					  SIZE_L3DATA_FLASHMODEACTIVATE_RESPONSE,
					  response,
					  L3TIMEOUT_DEFAULT_MSEC);

	 if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
	 	 return call_result;

	  // return OK only if activation was successful
	return (response[0] != 0)? acs_apbm_ipmiapi_ns::ERR_OP_NOT_EXECUTED : acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::flash_srecord (const unsigned char* data_record , unsigned char record_size){
        ACS_APBM_TRACE_FUNCTION;

	if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	// Check input parameter
	if (data_record == NULL)
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	int call_result;
	unsigned char response[SIZE_L3DATA_FLASHSREC_RESPONSE];

	call_result = _ipmifw_query(G_SERVICE,
                        C_FLASH_SREC,
                        record_size,
                        data_record,
                        SIZE_L3DATA_FLASHSREC_RESPONSE,
                        response,
                        L3TIMEOUT_FLASHREC_MSEC);

	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	return (response[0] != 0)? acs_apbm_ipmiapi_ns::ERR_OP_NOT_EXECUTED : acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::flash_s0record (const unsigned char* data_record , unsigned char record_size){
	ACS_APBM_TRACE_FUNCTION;

		int ret_code = acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;

	  const unsigned int groupcode = G_SERVICE;
		const unsigned int commandcode = C_FLASH_SREC;

		struct eri_ipmi_message req_message;
		struct eri_ipmi_message rep_message;

		int read_result;
		int resends = IPMIAPI_QUERY_RETRY_NUMBER;

		req_message.groupcode   = groupcode;
		req_message.commandcode = commandcode;
		req_message.result      = 0;
		req_message.size 		= record_size;
		memcpy(req_message.data, data_record, record_size);

		clock_t abstime_l3timeout = ::clock() + (clock_t)L3TIMEOUT_FLASHREC_MSEC;

		switch (_communication_mode) {

		case acs_apbm_ipmiapi_ns::DRIVER_MODE:
		{
			int result;
			int  req_message_len = sizeof(req_message);

			// Send Flash S0 record command to IMC
			result = write(_ipmi_fd, &req_message, req_message_len);
			if (result < 0 || result < req_message_len)
				return acs_apbm_ipmiapi_ns::ERR_QUERY_WRITE_ERROR;

      // After sending S0 on GEP1, sleep for 1 minute so that
      // have enough time to erase its flash
			sleep(60);

			int ret_code;
			while (resends--) {

				struct epoll_event epoll_events[1];
				int epoll_result;
				if ((epoll_result = epoll_wait(_epoll_fd, epoll_events, 1, IPMIAPI_QUERY_EPOLL_TIMEOUT)) < 0) {
					if(errno == EINTR)	continue;
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_ERROR;
					break;
				}

				if (epoll_result == 0) {
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_TIMEOUT;
					break;
				}

				if ((read_result = read(_ipmi_fd, &rep_message, sizeof(rep_message))) < 0){
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_READ_ERROR;
					break;
				}

				if (ipmi_message_is_response(&rep_message) &&
					match_ipmi_response(&rep_message, &req_message)) {
          // In successful response, the result should be 0.
          // l3Data[0], which is falsh status code, should also be 0
						/* expected response from IPMIFW, return it to the api-user */
						ret_code = _convert_ipmifw_result(rep_message.result);
						break;
				}
				else if (abstime_l3timeout < ::clock()) {
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_READ_TIMEOUT;
					break;
				}

				//printf("Call 'match_ipmi_response()' returned false\n");
			}
			return (resends > 0)? ret_code : acs_apbm_ipmiapi_ns::ERR_QUERY_RESPONSE_ERROR;

		break;
		}
		case acs_apbm_ipmiapi_ns::SERIAL_MODE:	//SERIAL MODE
		{
			_lock_device();

			if (const int call_result =_l2_impl->init()) {
				ACS_APBM_TRACE_MESSAGE("Call 'acs_apbm_ipmiapi_level2_impl::init()' Failed: call_result == %d", call_result);
				//printf("Call 'acs_apbm_ipmiapi_level2_impl::init()' Failed: call_result == %d", call_result);
				ret_code = acs_apbm_ipmiapi_ns::ERR_INIT_LEVEL2_ERROR;
			}
			else
				{
				if (_l2_impl->l2_write(req_message) != acs_apbm_ipmiapi_level2_impl::L2_WRITE_OK) {
					_l2_impl->finalize();
					ACS_APBM_TRACE_MESSAGE("Error: cannot write on serial device !");
					//printf("Error: cannot write on serial device !");
					return acs_apbm_ipmiapi_ns::ERR_WRITE_LEVEL2_ERROR;
				}

	      // After sending S0 on GEP1, sleep for 1 minute so that
	      // have enough time to erase its flash
				sleep(60);

				while(1)
				{
				  int readResult = _l2_impl->l2_read();

				  switch(readResult)
				  {
					  case acs_apbm_ipmiapi_level2_impl::L2_READ_FAILED:
						  // L2 has a serious problem
						  ACS_APBM_TRACE_MESSAGE("Error: cannot read from serial device !");
						  //printf("Error: cannot read from serial device !");
						  _l2_impl->finalize();
						  return acs_apbm_ipmiapi_ns::ERR_READ_LEVEL2_ERROR;

					  case acs_apbm_ipmiapi_level2_impl::L2_READ_L3MESSAGE_READY:
					  {
						  // L2 has completed reception, data is ready
						  ACS_APBM_TRACE_MESSAGE("read from device successfully: L2_READ_L3MESSAGE_READY !");
						  //printf("read from device successfully: L2_READ_L3MESSAGE_READY !");
						  eri_ipmi_message l3resp_message;

						  int getpacketResult = _l2_impl->getpacket(& l3resp_message);
						  if (getpacketResult != acs_apbm_ipmiapi_level2_impl::L2_GETPACKET_OK){
							  ACS_APBM_TRACE_MESSAGE("Error : call 'acs_apbm_ipmiapi_level2_impl::getPacket()' failed !");
						  	//printf("Error : call 'acs_apbm_ipmiapi_level2_impl::getPacket()' failed !");
						  }
						  else
						  {
							  if (ipmi_message_is_response(& l3resp_message) && match_ipmi_response(& l3resp_message, & req_message)) {
								  /* expected response from IPMIFW, return it to the api-user */
                  // In successful response, the result should be 0.
                  // l3Data[0], which is falsh status code, should also be 0
								  int ipmiFwResult  = _convert_ipmifw_result(l3resp_message.result);
								  _l2_impl->finalize();
								  return ipmiFwResult;
							  }
							  else {
								  ACS_APBM_TRACE_MESSAGE("Error : Unexpected L3 response received !");
								  //printf("Error : Unexpected L3 response received !");
							  }

						  }
						  continue;
					  }
					  break;

					  case acs_apbm_ipmiapi_level2_impl::L2_READ_CONTINUE:
						  // L2 is expecting more data, L2 must be polled again later
						  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_CONTINUE !");
						  //printf("read from device returned: L2_READ_CONTINUE !");
						  continue;

					  case acs_apbm_ipmiapi_level2_impl::L2_READ_COMPLETE:
						  // L2 is expecting no more data
						  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_COMPLETE !");
						  //printf("read from device returned: L2_READ_COMPLETE !");
					  break;

					  case acs_apbm_ipmiapi_level2_impl::L2_READ_CLOSE:
						  // L2 has learned for some reason the link will be closed
						  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_CLOSE !");
						  //printf("read from device returned: L2_READ_CLOSE !");
					  break;

					  default:
							// L2 returned illegal value
						  ACS_APBM_TRACE_MESSAGE("read from device returned unexpected value ' %d ' !", readResult);
						  //printf("read from device returned unexpected value ' %d ' !", readResult);
						  _l2_impl->finalize();
						  return acs_apbm_ipmiapi_ns::ERR_READ_LEVEL2_ERROR;
				  }  // END of switch

				  // No more data expected by L2, how about L3?

				  // L3 expects no response?
//				  l3timeout_msec = L3TIMEOUT_DEFAULT_MSEC;
//				  if (l3timeout_msec == 0) {
//					  _l2_impl->finalize();
//					  ACS_APBM_TRACE_MESSAGE("read from device completed. (l3timeout_msec == 0)");
//					  return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
//				  }

				  if (abstime_l3timeout < ::clock()) {
					  _l2_impl->finalize();
					  ACS_APBM_TRACE_MESSAGE("Error: timeout expired while waiting for l3 response from device !");
					  //printf("Error: timeout expired while waiting for l3 response from device !");
					  return acs_apbm_ipmiapi_ns::ERR_QUERY_READ_TIMEOUT;
				  }
				} // END of while

				// This point should never be reached but code is left here in case of future changes...
				 ACS_APBM_TRACE_MESSAGE("Error: Unexpected exit from wait loop !");
				 //printf("Error: Unexpected exit from wait loop !");
				 _l2_impl->finalize();

				 return acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;

				}

			_unlock_device();
		break;
		}
		default:
			ret_code = acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;
		break;
		}
 return ret_code;
}

int acs_apbm_ipmiapi_impl::get_led (unsigned int led_type, unsigned char* led_info_record){
        ACS_APBM_TRACE_FUNCTION;
	unsigned char response[SIZE_L3DATA_GETLED_RESPONSE];

	if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	// check input parameter
	if (led_info_record == NULL)
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	//Calling ''
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	bool is_gep5_or_gep7=false;

	myAPGCCCommonLib.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );
	if (hwInfoResult.hwVersionResult != ACS_APGCC_HWINFO_SUCCESS)
	{
		ACS_APBM_TRACE_MESSAGE("call 'GetHWInfo' failed with return code %d.", hwInfoResult.hwVersionResult);
		return hwInfoResult.hwVersionResult;
	}
	else{
		switch(hwInfo.hwVersion){
			case ACS_APGCC_HWVER_GEP5:
			//case ACS_APGCC_HWVER_GEP7:
				is_gep5_or_gep7=true;
				break;
			case ACS_APGCC_HWVER_GEP1:
			case ACS_APGCC_HWVER_GEP2:
			case ACS_APGCC_HWVER_VM:
			default:
				is_gep5_or_gep7=false;
				break;
			}
	}
	
	int call_result;
        unsigned char ipmi_led_type = static_cast <unsigned char>(led_type);

	if (is_gep5_or_gep7)
	{ // pass 'command code' as C_4LED_SET for 4 led hardware
		//Fourth LED compatibility for new hardware GEP4
		if (led_type != acs_apbm_ipmiapi_ns::GREEN_4LED &&
		    led_type != acs_apbm_ipmiapi_ns::RED_4LED &&
		    led_type != acs_apbm_ipmiapi_ns::YELLOW_4LED &&
    		    led_type != acs_apbm_ipmiapi_ns::BLUE_4LED) {//4th BLUE LED
			ACS_APBM_TRACE_MESSAGE("call 'get_led' failed for hwVersion '%d', led type '%d'. ret_code == %d.", hwInfo.hwVersion, led_type, acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER);
			return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;
		}

		call_result = _ipmifw_query(G_LED,
				C_4LED_GET,
				SIZE_L3DATA_GETLED_REQUEST,
				&ipmi_led_type,//LedIndex:4=Red,5=Green,6=Blue,7=Yellow
				SIZE_L3DATA_GETLED_RESPONSE,
				response,
				L3TIMEOUT_DEFAULT_MSEC);
	}
	else { //pass 'command code' as C_LED_SET for 3 led hardware
		if (led_type != acs_apbm_ipmiapi_ns::GREEN_LED &&
	            led_type != acs_apbm_ipmiapi_ns::RED_LED &&
		    led_type != acs_apbm_ipmiapi_ns::YELLOW_LED ) {//4th BLUE LED
			ACS_APBM_TRACE_MESSAGE("call 'get_led' failed for hwVersion '%d', led type '%d'. ret_code == %d.", hwInfo.hwVersion, led_type, acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER);
			return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;
		}

		call_result = _ipmifw_query(G_LED,
				C_LED_GET,
				SIZE_L3DATA_GETLED_REQUEST,
				&ipmi_led_type,//LedIndex:0=Green,1=Yellow,2=Red
				SIZE_L3DATA_GETLED_RESPONSE,
				response,
				L3TIMEOUT_DEFAULT_MSEC);
	}

	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	memcpy(led_info_record, &response[1], SIZE_L3DATA_GETLED_RESPONSE);

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS; 
 
}


int acs_apbm_ipmiapi_impl::get_ext_alarms (acs_apbm_ipmiapi_ns::sensor_index sensor_index, acs_apbm_ipmiapi_ns::sensor_readings* sensor_readings) {
        ACS_APBM_TRACE_FUNCTION;

	unsigned char response[SIZE_L3DATA_GETSENSORREADINGS_RESPONSE];

	if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	if( !sensor_readings || (sensor_index != acs_apbm_ipmiapi_ns::SENSOR_HANDLING_FIRST_32_GEA_CONNECTORS && sensor_index != acs_apbm_ipmiapi_ns::SENSOR_HANDLING_FOLLOWING_32_GEA_CONNECTORS))
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	unsigned char sensor_char = sensor_index;

	int call_result = _ipmifw_query(G_SEL,
						  C_GET_SENSOR_READING,
						  SIZE_L3DATA_GETSENSORREADINGS_REQUEST,
						  & sensor_char,
						  SIZE_L3DATA_GETSENSORREADINGS_RESPONSE,
						  response,
						  L3TIMEOUT_DEFAULT_MSEC);
	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	memcpy(sensor_readings->sensor_values, &response[0], acs_apbm_ipmiapi_ns::SENSOR_VALUES_SIZE);

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::set_alarm_panel (const acs_apbm_ipmiapi_ns::alarm_panel_values* alarm_panel_values) {
        ACS_APBM_TRACE_FUNCTION;

	if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	if(!alarm_panel_values)
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	unsigned char response;
	int call_result = _ipmifw_query(G_SEL,
						  C_REARM_ALARM,
						  SIZE_L3DATA_REARM_PANEL_REQUEST,
						  reinterpret_cast<const unsigned char *>(alarm_panel_values),
						  SIZE_L3DATA_REARM_PANEL_RESPONSE,
						  & response,
						  L3TIMEOUT_DEFAULT_MSEC);

	if(call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	return (response != 0 ?  acs_apbm_ipmiapi_ns::ERR_OP_NOT_EXECUTED : acs_apbm_ipmiapi_ns::ERR_NO_ERRORS);
}

int acs_apbm_ipmiapi_impl::presence_ack() {
        ACS_APBM_TRACE_FUNCTION;

	if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	int call_result = _ipmifw_query(G_WDOG,
						  C_PRESENCE,
						  SIZE_L3DATA_WATCHDOG_REQUEST,
						  NULL,
						  SIZE_L3DATA_WATCHDOG_RESPONSE,
						  NULL,
						  L3TIMEOUT_DEFAULT_MSEC);

	return call_result;
}

int acs_apbm_ipmiapi_impl::ipmi_restart() {
        ACS_APBM_TRACE_FUNCTION;

	if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	int call_result = _ipmifw_query(G_SERVICE,
						  C_IPMI_RESTART,
						  SIZE_L3DATA_IPMIRESTART_REQUEST,
						  NULL,
						  SIZE_L3DATA_IPMIRESTART_REQUEST,
						  NULL,
						  L3TIMEOUT_DEFAULT_MSEC);
	return call_result;
}

int acs_apbm_ipmiapi_impl::get_board_address(unsigned int* board_address) {
        ACS_APBM_TRACE_FUNCTION;

	unsigned char response;

	if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	if(!board_address)
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	int call_result = _ipmifw_query(G_DEBUG,
						  C_GET_ADDRESS,
						  SIZE_L3DATA_GETADDRESS_REQUEST,
						  NULL,
						  SIZE_L3DATA_GETADDRESS_RESPONSE,
						  & response,
						  L3TIMEOUT_DEFAULT_MSEC);
	if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
		return call_result;

	*board_address = response;

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/******************************************************************************/

/**************************************************************************
* l3GetIpmiHwVersion
*
* Description
*    Gets the Device Revision
*
* Parameters
*    hwBlock: 01 = IPMI
*             02 = CPLD
*
*  deviceRev: 0 = ROZ 101 165/1 (GEP1)
*             1 = ROZ 101 166/1 (GEP2)
*
* Returns
*    L3_OK
*    L3_ERR
*************************************************************************/
int acs_apbm_ipmiapi_impl::get_ipmi_hardware_version(const unsigned char hw_block, unsigned char* device_rev)
{
  ACS_APBM_TRACE_FUNCTION;

  unsigned char response[SIZE_L3DATA_GETIPMIHWVERSION_RESPONSE];

  if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
    return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

  int result = _ipmifw_query(G_INFO,
                C_IPMI_HW_VERSION,
                SIZE_L3DATA_GETIPMIHWVERSION_REQUEST,
                &hw_block,
                SIZE_L3DATA_GETIPMIHWVERSION_RESPONSE,
                response,
                L3TIMEOUT_DEFAULT_MSEC);

  if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
          return result;

   // byte 2 of the response is the device revision
        *device_rev = response[1];

        return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/**************************************************************************
* l3GetDefaultBIOSPointer
*
* Description
*    Get default BIOS pointer.
*
* Parameters
*    BIOSArea: Returns the default BIOS pointer
*
* Returns
*    0 or 1
************************************************************************/
int acs_apbm_ipmiapi_impl::get_default_BIOS_pointer(unsigned int * bios_area)
{
        ACS_APBM_TRACE_FUNCTION;

        unsigned char response;

        if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
          return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

        int result = _ipmifw_query(G_SERVICE,
                C_READ_BIOS_POINTER,
                SIZE_L3DATA_READBIOSPOINTER_REQUEST,
                NULL,
                SIZE_L3DATA_READBIOSPOINTER_RESPONSE,
                &response,
                L3TIMEOUT_DEFAULT_MSEC);

        if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
                return result;

        *bios_area = response;

        return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/**************************************************************************
* l3GetDefaultBIOSImage
*
* Description
*    Get default BIOS image.
*
* Parameters
*    BIOSImage: Returns the default BIOS image
*
* Returns
*    0 or 1
************************************************************************/
int acs_apbm_ipmiapi_impl::get_default_BIOS_image(unsigned int* bios_image)
{
        ACS_APBM_TRACE_FUNCTION;

        unsigned char response;

        if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
          return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

        int result = _ipmifw_query(G_SERVICE,
                C_GET_DEFAULT_BIOS_IMAGE,
                SIZE_L3DATA_GETDEFAULTBIOS_REQUEST,
                NULL,
                SIZE_L3DATA_GETDEFAULTBIOS_RESPONSE,
                &response,
                L3TIMEOUT_DEFAULT_MSEC);


        if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
                return result;

        *bios_image = response;

        return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/**************************************************************************
* l3SetDefaultBIOSImage
*
* Description
*    Set default BIOS image.
*
* Parameters
*    BIOSImage: which BIOS area that should be default. Must be 0 or 1
*
* Returns
*    L3_OK
*    L3_ERR
************************************************************************/
int acs_apbm_ipmiapi_impl::set_default_BIOS_image(unsigned int bios_image)
{
        ACS_APBM_TRACE_FUNCTION;

        if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
          return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

        if(/*bios_image < 0 ||*/ bios_image > 1)
        {
                return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;
        }

        unsigned char image = bios_image + 48;

        int result = _ipmifw_query(G_SERVICE,
                C_SET_DEFAULT_BIOS_IMAGE,
                SIZE_L3DATA_SETDEFAULTBIOS_REQUEST,
                &image,
                SIZE_L3DATA_SETDEFAULTBIOS_RESPONSE,
                NULL,//response,
                L3TIMEOUT_DEFAULT_MSEC);

        if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
                return result;

        return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/**************************************************************************
* l3SetDefaultBIOSPointer
*
* Description
*    Set default BIOS pointer.
*
* Parameters
*    BIOSPointer: The BIOS area where to point. Must be 0 or 1
*
* Returns
*    L3_OK
*    L3_ERR
************************************************************************/
int acs_apbm_ipmiapi_impl::set_default_BIOS_pointer(unsigned int bios_pointer)
{
        ACS_APBM_TRACE_FUNCTION;

        if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
          return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

        if(/*bios_pointer < 0 ||*/ bios_pointer > 1)
        {
            return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;
        }

        unsigned char pointer = bios_pointer + 48;

        int result = _ipmifw_query(G_SERVICE,
                C_SET_BIOS_POINTER,
                SIZE_L3DATA_SETBIOSPOINTER_REQUEST,
                &pointer,
                SIZE_L3DATA_SETBIOSPOINTER_RESPONSE,
                NULL,//response,
                L3TIMEOUT_DEFAULT_MSEC);

        if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
                return result;

        return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/**************************************************************************
* l3IpmiFwResetStatus
*
* Description
*    Receives IPMI FW Reset Status and sends the response
*
* Parameters
*    Firmware type: 00 = Exchangeable
*                   01 = Fall-back
*                   02 = Not Exchangeable
*
* Returns
*    L3_OK
*    L3_ERR
************************************************************************/
int acs_apbm_ipmiapi_impl::ipmi_firmware_reset_status(unsigned char* fwType)
{
       ACS_APBM_TRACE_FUNCTION;

        if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
          return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

        int result = _ipmi_command(G_SHMC,
                            C_FW_RESET,
                            SIZE_L3DATA_IPMIFWRESETSTATUS_COMMAND,
                            fwType,
                            SIZE_L3DATA_IPMIFWRESETSTATUS_RESPONSE,
                            NULL);

        if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
                return result;

        return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/**************************************************************************
* l3IpmiFwEraseFinished
*
* Description
*    Receives IPMI FW Erase finished and sends the response
*
* Parameters
*    Channel state
*
* Returns
*    L3_OK
*    L3_ERR
************************************************************************/
int acs_apbm_ipmiapi_impl::ipmi_firmware_erase_finished(unsigned char* chState)
{
  ACS_APBM_TRACE_FUNCTION;

  if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
    return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

        int result = _ipmi_command(G_SEL,
                            C_IPMI_FW_ERASE_FINISHED,
                            SIZE_L3DATA_IPMIFWERASEFINISHED_COMMAND,
                            chState,
                            SIZE_L3DATA_IPMIFWERASEFINISHED_RESPONSE,
                            NULL);

        if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
                       return result;

               return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/**************************************************************************
* l3UpgradeIPMIFirmvare
*
* Description
*    Upgrade IPMI Firmware.
*
* Parameters
*    imagePath: Path to the ipmi firmware mot-file.
*
* Returns
*    L3_OK
*    L3_ERR
************************************************************************/
int acs_apbm_ipmiapi_impl::upgrade_ipmi_firmvare(std::string /*imagePath*/)
{
  /*
   unsigned char deviceRev, fwType, chState;

   TRACE_IN(l3Trace,l3UpgradeIPMIFirmvare,imagePath);

        //Verify that the file can be opened
        ifstream myfile(imagePath.c_str());
        if (!myfile.is_open())
        {
                cerr << "Could not open file '" << imagePath.c_str() << "'" << endl;
                return L3_ERR;
        }


   // get IPMI HW version
        if (L3_ERR == l3GetIpmiHwVersion(1, &deviceRev))
        {
                cout<<"Failed to get IPMI HW version."<<endl;
                return L3_ERR;
        }

        //start ipmi in FB mode in order to upgrade UPG
        if (L3_ERR == l3IpmiRestart(1))
        {
                cout << "Failed to Start IPMI in Fallback-mode." << endl;
                return L3_ERR;
        }
        //give it some time to restart
        Sleep(500);

   // Receive IPMI FW Reset Status
        if (L3_ERR == l3IpmiFwResetStatus(&fwType))
        {
                cout<<"Failed to receive IPMI FW Reset Status."<<endl;
                return L3_ERR;
        }

   TRACE_FLOW(l3Trace, l3UpgradeIPMIFirmvare, "Activating Flashmode");
        //Activate flashmode
        if (L3_ERR == l3FlashModeActivate(4))
        {
                cout << "Failed to activate flashmode" << endl;
                return L3_ERR;
        }

   if (deviceRev == 1) // GEP2
   {
      // wait 1 minute before checking for IPMI FW Erase finished
      Sleep(ONE_MINUTE);

      // Receive IPMI FW Erase finished
           if (L3_ERR == l3IpmiFwEraseFinished(&chState))
           {
                   cout<<"Failed to receive IPMI FW Erase finished."<<endl;
                   return L3_ERR;
           }
   }

        string line;
        int linenumber = 0;
        cout << "Flashing started" << flush;
        while (! myfile.eof() )
        {
                getline (myfile,line);

                //Verify that first character is S to avoid file ending/starting characters
                if(strcmp(line.substr(0,1).c_str(), "S") == 0)
                {
         // The S0 record needs a special handling on GEP1
         if (strcmp(line.substr(0,2).c_str(), "S0") == 0 && deviceRev == 0)
         {
                        if (L3_ERR == l3FlashS0Rec((const unsigned char*)line.c_str(), (const unsigned int)line.size()))
                           {
                                   cout << "\nFlashing failed." << endl;
                                   return L3_ERR;
                           }
            continue; // avoid the processing of S0 record twice on GEP1
         }

                        if (L3_ERR == l3FlashSRec((const unsigned char*)line.c_str(), (const unsigned int)line.size()))
                        {
                                cout << "\nFlashing failed." << endl;
                                return L3_ERR;
                        }

                }
                linenumber++;
                if (linenumber%500 == 0)
                {
                        cout << "." << flush;
                }
        }
        myfile.close();
        cout << "finished" << endl;
        //start ipmi in UPG mode
        if (L3_ERR == l3IpmiRestart(0))
        {
                cout << "Failed to Start IPMI in UPG mode." << endl;
                TRACE_RETURN(l3Trace, l3UpgradeIPMIFirmvare, L3_ERR);
                return L3_ERR;
        }
        //give it some time to restart
        Sleep(1000);

   // Receive IPMI FW Reset Status
        if (L3_ERR == l3IpmiFwResetStatus(&fwType))
        {
                cout<<"Failed to receive IPMI FW Reset Status."<<endl;
                return L3_ERR;
        }

        TRACE_RETURN(l3Trace, l3UpgradeIPMIFirmvare, L3_OK);
        return L3_OK;*/
  return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}


/*****************************************************************************/

int acs_apbm_ipmiapi_impl::open_ipmi_driver(const char *ipmi_device_name){

	ACS_APBM_TRACE_FUNCTION;

	_ipmi_fd = open(ipmi_device_name, O_RDWR | O_NONBLOCK);
	if (_ipmi_fd < 0){
		ACS_APBM_TRACE_MESSAGE("call 'open' failed: error opening device '%s'. ret_code == %d, errno == %d.", ipmi_device_name, _ipmi_fd, errno);
		return acs_apbm_ipmiapi_ns::ERR_OPEN_DEVICE_FAILURE;
	}

	if ((_epoll_fd = epoll_create(1)) == -1) {
		ACS_APBM_TRACE_MESSAGE("call 'epoll_create' failed. ret_code == %d, errno == %d.", _epoll_fd, errno);
		close(_ipmi_fd);
		return acs_apbm_ipmiapi_ns::ERR_EPOLL_CREATE_FAILURE;
	}

	struct epoll_event ipmi_event;
	ipmi_event.data.fd = _ipmi_fd;
	ipmi_event.events = EPOLLIN;
	if (const int ret_code = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _ipmi_fd, &ipmi_event)) {
		ACS_APBM_TRACE_MESSAGE("call 'epoll_ctl' failed. ret_code == %d, errno == %d.", ret_code, errno);
		close(_epoll_fd);
		close(_ipmi_fd);
		return acs_apbm_ipmiapi_ns::ERR_EPOLL_CTL_FAILURE;
	}

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

/****************************************************************************/
int acs_apbm_ipmiapi_impl::_ipmifw_query(
		unsigned int groupcode, unsigned int commandcode, unsigned int req_l3data_size,
		const unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data,
		unsigned int   l3timeout_msec)
{
	ACS_APBM_TRACE_FUNCTION;

	int ret_code = acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;

	switch (_communication_mode) {

	case acs_apbm_ipmiapi_ns::DRIVER_MODE:
		ret_code = _ipmifw_query_on_driver(groupcode, commandcode, req_l3data_size, req_l3data, res_l3data_size, res_l3data, l3timeout_msec);
	break;

	case acs_apbm_ipmiapi_ns::SERIAL_MODE:
		_lock_device();

		// start of TRs - HU25795 & HU57639
		int resInitMutex, resAquireMutex, resReleaseMutex, resAdjustSemaphore;

		resInitMutex = resAquireMutex = resReleaseMutex = resAdjustSemaphore = 0;

		resInitMutex = init_process_mutex();

		if (resInitMutex && (process_mutex == NULL)) {
			ACS_APBM_TRACE_MESSAGE("Call 'init_process_mutex()' failed! Exiting before L2->init()");
			ret_code = acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;
			_unlock_device();
			break;
		}

		resAdjustSemaphore = check_and_adjust_system_V_sem();

		if((resAdjustSemaphore != 0) && (resAdjustSemaphore != 1))
			ACS_APBM_TRACE_MESSAGE("Call 'check_and_adjust_system_V_sem()' failed! Unable to adjust the system SV Semaphore");

		resAquireMutex = acquire_process_mutex();

		if(resAquireMutex != 0)
		{
			ACS_APBM_TRACE_MESSAGE("Call 'acquire_process_mutex()' failed! Unable to acquire process mutex");
			ret_code = acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;
			_unlock_device();
			break;
		}

		if (const int call_result =_l2_impl->init()) {
			ACS_APBM_TRACE_MESSAGE("Call 'acs_apbm_ipmiapi_level2_impl::init()' Failed: call_result == %d", call_result);
			ret_code = acs_apbm_ipmiapi_ns::ERR_INIT_LEVEL2_ERROR;
		}
		else
			ret_code = _ipmifw_query_on_serial_port(groupcode, commandcode, req_l3data_size, req_l3data, res_l3data_size, res_l3data, l3timeout_msec);

		if((resAquireMutex == 0) && ((resReleaseMutex = release_process_mutex()) != 0))
		{
			ACS_APBM_TRACE_MESSAGE("Call 'release_process_mutex()' failed! Unable to release process mutex!");
		}
		// end of TRs - HU25795 & HU57639

		_unlock_device();
	break;

	default:
		ret_code = acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;
	}

	return ret_code;
}


int acs_apbm_ipmiapi_impl::_ipmifw_query_on_serial_port(unsigned int groupcode, unsigned int commandcode, unsigned int req_l3data_size,
        const unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data,
        unsigned int l3timeout_msec)
{
	ACS_APBM_TRACE_FUNCTION;
	int sleep_time = 50 * 1000;
	struct eri_ipmi_message req_message;

	req_message.groupcode   = groupcode;
	req_message.commandcode = commandcode;
	req_message.result      = 0;
	req_message.size 		= req_l3data_size;
	memcpy(req_message.data, req_l3data, req_l3data_size);

	clock_t abstime_l3timeout = ::clock() + (clock_t)l3timeout_msec;

	if (_l2_impl->l2_write(req_message) != acs_apbm_ipmiapi_level2_impl::L2_WRITE_OK) {
		_l2_impl->finalize();
		ACS_APBM_TRACE_MESSAGE("Error: cannot write on serial device !");
		return acs_apbm_ipmiapi_ns::ERR_WRITE_LEVEL2_ERROR;
	}
usleep (sleep_time);

	while(1)
	{
		//Check for answere not needed
		if( res_l3data_size == 0)
		{
			_l2_impl->finalize();
			return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
		}

	  int readResult = _l2_impl->l2_read();
	  sleep_time = 0;
	  switch(readResult)
	  {
		  case acs_apbm_ipmiapi_level2_impl::L2_READ_FAILED:
			  // L2 has a serious problem
			  ACS_APBM_TRACE_MESSAGE("Error: cannot read from serial device !");
			  //printf("Error: cannot read from serial device !\n");
			  sleep_time = sleep_time + (25000 * 1000);
			  _l2_impl->finalize();
			  return acs_apbm_ipmiapi_ns::ERR_READ_LEVEL2_ERROR;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_L3MESSAGE_READY:
		  {
			  // L2 has completed reception, data is ready
			  ACS_APBM_TRACE_MESSAGE("read from device successfully: L2_READ_L3MESSAGE_READY !");
			  //printf("read from device successfully: L2_READ_L3MESSAGE_READY !\n");
			  eri_ipmi_message l3resp_message;

			  int getpacketResult = _l2_impl->getpacket(& l3resp_message);
			  if (getpacketResult != acs_apbm_ipmiapi_level2_impl::L2_GETPACKET_OK){
				  ACS_APBM_TRACE_MESSAGE("Error : call 'acs_apbm_ipmiapi_level2_impl::getPacket()' failed !");
			  //printf("Error : call 'acs_apbm_ipmiapi_level2_impl::getPacket()' failed !\n");
				  }
			  else
			  {
				  if (ipmi_message_is_response(& l3resp_message) && match_ipmi_response(& l3resp_message, & req_message)) {
					  /* expected response from IPMIFW, return it to the api-user */
					  int msg_size = l3resp_message.size < res_l3data_size ? l3resp_message.size : res_l3data_size;
					  memset(res_l3data, 0, res_l3data_size);
					  memcpy(res_l3data, l3resp_message.data, msg_size);
					  int ipmiFwResult  = _convert_ipmifw_result(l3resp_message.result);
					  _l2_impl->finalize();
					  return ipmiFwResult;
				  }
				  else {
					  ACS_APBM_TRACE_MESSAGE("Error : Unexpected L3 response received !");
					  //printf("Error : Unexpected L3 response received !");
				  }

			  }
			  continue;
		  }
		  break;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_CONTINUE:
			  // L2 is expecting more data, L2 must be polled again later
			  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_CONTINUE !");
			  //printf("read from device returned: L2_READ_CONTINUE !\n");
			  continue;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_COMPLETE:
			  // L2 is expecting no more data
			  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_COMPLETE !");
			  //printf("read from device returned: L2_READ_COMPLETE !\n");
		  break;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_CLOSE:
			  // L2 has learned for some reason the link will be closed
			  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_CLOSE !");
			  //printf("read from device returned: L2_READ_CLOSE !\n");
		  break;

		  default:
				// L2 returned illegal value
			  ACS_APBM_TRACE_MESSAGE("read from device returned unexpected value ' %d ' !", readResult);
			  //printf("read from device returned unexpected value ' %d ' !\n", readResult);
			  _l2_impl->finalize();
			  return acs_apbm_ipmiapi_ns::ERR_READ_LEVEL2_ERROR;
	  }  // END of switch

	  // No more data expected by L2, how about L3?

	  // L3 expects no response?
	  if (l3timeout_msec == 0) {
		  _l2_impl->finalize();
		  ACS_APBM_TRACE_MESSAGE("read from device completed. (l3timeout_msec == 0)");
		  //printf("read from device completed. (l3timeout_msec == 0)\n");
		  return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
	  }

	  if (abstime_l3timeout < ::clock()) {
		  _l2_impl->finalize();
		  ACS_APBM_TRACE_MESSAGE("Error: timeout expired while waiting for l3 response from device !");
		  printf("Error: timeout expired while waiting for l3 response from device !\n");
		  return acs_apbm_ipmiapi_ns::ERR_QUERY_READ_TIMEOUT;
	  }
	} // END of while

	// This point should never be reached but code is left here in case of future changes...
	 ACS_APBM_TRACE_MESSAGE("Error: Unexpected exit from wait loop !\n");
	 //printf("Error: Unexpected exit from wait loop !");
	 _l2_impl->finalize();

	 return acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;


}

int acs_apbm_ipmiapi_impl::_ipmifw_query_on_driver(unsigned int groupcode, unsigned int commandcode, unsigned int req_l3data_size,
        const unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data,
        unsigned int l3timeout_msec)
{
	ACS_APBM_TRACE_FUNCTION;

	struct eri_ipmi_message req_message;
	struct eri_ipmi_message rep_message;

	int read_result;
	int resends = IPMIAPI_QUERY_RETRY_NUMBER;
	int write_retry = IPMIAPI_OP_RETRY_NUMBER;

	req_message.groupcode   = groupcode;
	req_message.commandcode = commandcode;
	req_message.result      = 0;
	req_message.size 		= req_l3data_size;
	memcpy(req_message.data, req_l3data, req_l3data_size);

	int result;
	clock_t  abstime_l3timeout = ::clock() + (clock_t) l3timeout_msec;
	int  req_message_len = sizeof(req_message);
	do {
		 result = write(_ipmi_fd, &req_message, req_message_len);
	} while (result < 0 && errno == EINTR && --write_retry);

	if (result < 0 || result < req_message_len)
		return acs_apbm_ipmiapi_ns::ERR_QUERY_WRITE_ERROR;

	struct epoll_event epoll_events[1];
	int epoll_result;
	int ret_code;
	while (resends--) {
		if ((epoll_result = epoll_wait(_epoll_fd, epoll_events, 1, IPMIAPI_QUERY_EPOLL_TIMEOUT)) < 0) {
			if(errno == EINTR)	continue;
			ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_ERROR;
			break;
		}

		if (epoll_result == 0) {
			ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_TIMEOUT;
			break;
		}

		if ((read_result = read(_ipmi_fd, &rep_message, sizeof(rep_message))) < 0){
			ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_READ_ERROR;
			break;
		}

		if (ipmi_message_is_response(&rep_message) &&
			match_ipmi_response(&rep_message, &req_message)) {
				int i;
				/* expected response from IPMIFW, return it to the api-user */
				i = rep_message.size < res_l3data_size ? rep_message.size : res_l3data_size;
				memset(res_l3data, 0, res_l3data_size);
				memcpy(res_l3data, rep_message.data, i);
				ret_code = _convert_ipmifw_result(rep_message.result);
				break;
		}
		else if (abstime_l3timeout < ::clock()) {
			ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_READ_TIMEOUT;
			break;
		}

		//printf("Call 'match_ipmi_response()' returned false\n");
	}
	return  (resends > 0)? ret_code : acs_apbm_ipmiapi_ns::ERR_QUERY_RESPONSE_ERROR;
}

int acs_apbm_ipmiapi_impl::_ipmi_command(const unsigned int groupcode, const unsigned int commandcode, const unsigned int req_l3data_size,
						   unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data){

	ACS_APBM_TRACE_FUNCTION;

		int ret_code = acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;

		switch (_communication_mode) {

		case acs_apbm_ipmiapi_ns::DRIVER_MODE:
			ret_code = _ipmi_command_on_driver(groupcode, commandcode, req_l3data_size, req_l3data, res_l3data_size, res_l3data);
		break;

		case acs_apbm_ipmiapi_ns::SERIAL_MODE:
			_lock_device();

			if (const int call_result =_l2_impl->init()) {
				ACS_APBM_TRACE_MESSAGE("Call 'acs_apbm_ipmiapi_level2_impl::init()' Failed: call_result == %d", call_result);
				ret_code = acs_apbm_ipmiapi_ns::ERR_INIT_LEVEL2_ERROR;
			}
			else
				ret_code = _ipmi_command_on_serial_port(groupcode, commandcode, req_l3data_size, req_l3data, res_l3data_size, res_l3data);

			_unlock_device();
		break;

		default:
			ret_code = acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;
		}

		return ret_code;
}

int acs_apbm_ipmiapi_impl::_ipmi_command_on_serial_port(const unsigned int groupcode, const unsigned int commandcode, const unsigned int req_l3data_size,
							   unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data){

	ACS_APBM_TRACE_FUNCTION;

	struct eri_ipmi_message req_message;
	int sleep_time = 200 * 1000;
	req_message.groupcode   = groupcode;
	req_message.commandcode = commandcode;
	req_message.result      = 0;
	req_message.size 		= req_l3data_size;
	memcpy(req_message.data, req_l3data, req_l3data_size);

	clock_t l3timeout_msec = L3TIMEOUT_DEFAULT_MSEC;
	clock_t abstime_l3timeout = ::clock() + (clock_t)l3timeout_msec;

	usleep (sleep_time);
	while(1)
	{
	  int readResult = _l2_impl->l2_read();
	  //printf("\nDEBUG LIBRARY - read result=%d !\n",readResult);
	  sleep_time = 0;
	  switch(readResult)
	  {
		  case acs_apbm_ipmiapi_level2_impl::L2_READ_FAILED:
			  // L2 has a serious problem
			  ACS_APBM_TRACE_MESSAGE("Error: cannot read from serial device !");
			  sleep_time = sleep_time + (25000 * 1000);
			  _l2_impl->finalize();
			  //printf("\nDEBUG LIBRARY - READ FAILED\n");
			  return acs_apbm_ipmiapi_ns::ERR_READ_LEVEL2_ERROR;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_L3MESSAGE_READY:
		  {
			  // L2 has completed reception, data is ready
			  ACS_APBM_TRACE_MESSAGE("read from device successfully: L2_READ_L3MESSAGE_READY !");
			  //printf("read from device successfully: L2_READ_L3MESSAGE_READY !");
			  eri_ipmi_message l3resp_message;

			  int getpacketResult = _l2_impl->getpacket(& l3resp_message);
			  if (getpacketResult != acs_apbm_ipmiapi_level2_impl::L2_GETPACKET_OK){
			  	//printf("\nDEBUG LIBRARY - call 'acs_apbm_ipmiapi_level2_impl::getPacket()' failed ! result=%d data=%02X \n",getpacketResult, l3resp_message.data);
			  	ACS_APBM_TRACE_MESSAGE("Error : call 'acs_apbm_ipmiapi_level2_impl::getPacket()' failed !");
			  }
			  else
			  {
			  	//printf("Retrieved group code=%d , command code=%d , data =%02X\n",l3resp_message.groupcode,l3resp_message.commandcode, l3resp_message.data);
				  if ((l3resp_message.groupcode == groupcode) && (l3resp_message.commandcode == commandcode)) {
					  /* expected response from IPMIFW, return it to the api-user */
					  int msg_size = l3resp_message.size < res_l3data_size ? l3resp_message.size : res_l3data_size;
					  memset(req_l3data, 0, req_l3data_size);
					  memcpy(req_l3data, l3resp_message.data, msg_size);
					  int ipmiFwResult  = _convert_ipmifw_result(l3resp_message.result);

//						int result;
//						int  req_message_len = sizeof(req_message);
						req_message.groupcode = l3resp_message.groupcode + 0x80;
						req_message.commandcode = l3resp_message.commandcode;
						req_message.result = 0;
						req_message.size = 0;
						//printf("\nDEBUG LIBRARY - Before WRITE!\n");

						 if (res_l3data != NULL)
						{
							memcpy(req_message.data, res_l3data, res_l3data_size);
						}

						if (_l2_impl->l2_write(req_message) != acs_apbm_ipmiapi_level2_impl::L2_WRITE_OK) {
							//printf("\nDEBUG LIBRARY - Error: cannot write on serial device !\n");
							_l2_impl->finalize();
							ACS_APBM_TRACE_MESSAGE("Error: cannot write on serial device !");
							return acs_apbm_ipmiapi_ns::ERR_WRITE_LEVEL2_ERROR;
						}
					  _l2_impl->finalize();
					  return ipmiFwResult;
				  }
				  else {
				  	 //printf("\nDEBUG LIBRARY - Error : Unexpected L3 response received !\n");
					  ACS_APBM_TRACE_MESSAGE("Error : Unexpected L3 response received !");
				  }

			  }
			  continue;
		  }
		  break;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_CONTINUE:
			  // L2 is expecting more data, L2 must be polled again later
			  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_CONTINUE !");
			  continue;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_COMPLETE:
			  // L2 is expecting no more data
			  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_COMPLETE !");
		  break;

		  case acs_apbm_ipmiapi_level2_impl::L2_READ_CLOSE:
			  // L2 has learned for some reason the link will be closed
			  ACS_APBM_TRACE_MESSAGE("read from device returned: L2_READ_CLOSE !");
		  break;

		  default:
				// L2 returned illegal value
			  ACS_APBM_TRACE_MESSAGE("read from device returned unexpected value ' %d ' !", readResult);
			  _l2_impl->finalize();
			  return acs_apbm_ipmiapi_ns::ERR_READ_LEVEL2_ERROR;
	  }  // END of switch

	  // No more data expected by L2, how about L3?

	  // L3 expects no response?
	  if (l3timeout_msec == 0) {
		  _l2_impl->finalize();
		  ACS_APBM_TRACE_MESSAGE("read from device completed. (l3timeout_msec == 0)");
		  return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
	  }

	  if (abstime_l3timeout < ::clock()) {
		  _l2_impl->finalize();
		  ACS_APBM_TRACE_MESSAGE("Error: timeout expired while waiting for l3 response from device !");
		  return acs_apbm_ipmiapi_ns::ERR_QUERY_READ_TIMEOUT;
	  }
	} // END of while

	// This point should never be reached but code is left here in case of future changes...
	 ACS_APBM_TRACE_MESSAGE("Error: Unexpected exit from wait loop !");
	 _l2_impl->finalize();

	 return acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;

}

int acs_apbm_ipmiapi_impl::_ipmi_command_on_driver(const unsigned int groupcode, const unsigned int commandcode, const unsigned int req_l3data_size,
							   unsigned char* req_l3data, unsigned int /*res_l3data_size*/, unsigned char* /*res_l3data*/){


	ACS_APBM_TRACE_FUNCTION;

	struct eri_ipmi_message req_message; //cmd msg
	struct eri_ipmi_message rep_message;

	int read_result;
	int resends = IPMIAPI_QUERY_RETRY_NUMBER;
	int write_retry = IPMIAPI_OP_RETRY_NUMBER;
	clock_t abstime_l3timeout = ::clock() + (clock_t)L3TIMEOUT_DEFAULT_MSEC;
	req_message.groupcode   = groupcode;
	req_message.commandcode = commandcode;
	req_message.result      = 0;
	req_message.size 		= req_l3data_size;
	memcpy(req_message.data, req_l3data, req_l3data_size);

			struct epoll_event epoll_events[1];
			int epoll_result;
			int ret_code;
			while (resends--) {
				if ((epoll_result = epoll_wait(_epoll_fd, epoll_events, 1, IPMIAPI_QUERY_EPOLL_TIMEOUT)) < 0) {
					if(errno == EINTR)	continue;
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_ERROR;
					break;
				}

				if (epoll_result == 0) {
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_TIMEOUT;
					break;
				}

				if ((read_result = read(_ipmi_fd, &rep_message, sizeof(rep_message))) < 0){
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_READ_ERROR;
					break;
				}
				//printf("Retrieved group code=%d , command code=%d\n",rep_message.groupcode,rep_message.commandcode);
				if ((rep_message.groupcode == groupcode) && (rep_message.commandcode == commandcode))
				{
					int result;
					int  req_message_len = sizeof(req_message);
					req_message.groupcode = rep_message.groupcode + 0x80;
					req_message.commandcode = rep_message.commandcode;
					req_message.result = 0;
					req_message.size = 0;

					do {
						 result = write(_ipmi_fd, &req_message, req_message_len);
					} while (result < 0 && errno == EINTR && --write_retry);

					break;
				}
				else if (abstime_l3timeout < ::clock()) {
					ret_code = acs_apbm_ipmiapi_ns::ERR_QUERY_READ_TIMEOUT;
					break;
				}

				//printf("Call 'match_ipmi_response()' returned false\n");
			}

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_impl::_convert_ipmifw_result(char code)
{
    switch (code)
    {
		case 0x00:
			return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
		case 0xc0:
			return acs_apbm_ipmiapi_ns::ERR_NODE_BUSY;
		case 0xd5:
			return acs_apbm_ipmiapi_ns::ERR_OP_NOT_SUPPORTED;
		case 0xff:
			return acs_apbm_ipmiapi_ns::ERR_QUERY_RESPONSE_ERROR;
		default:
        /* unknown resultcode from ipmifw, return IPMIAPI_ERR as best effort */
			return acs_apbm_ipmiapi_ns::ERR_QUERY_UNKNOWN_RESPONSE;
    }
}

int acs_apbm_ipmiapi_impl::_set_led(acs_apbm_ipmiapi_ns:: led_type_t led_type, int status)
{
        ACS_APBM_TRACE_FUNCTION;
	int result;
	if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

	//Calling 'GetHwInfo'
	unsigned int command_code; 	
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	bool is_gep5_or_gep7=false;

	myAPGCCCommonLib.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );
	if(hwInfoResult.hwVersionResult != ACS_APGCC_HWINFO_SUCCESS){
		ACS_APBM_TRACE_MESSAGE("call 'GetHwInfo' failed with return code %d.", hwInfoResult.hwVersionResult);
	}
	else{
		switch(hwInfo.hwVersion){
		case ACS_APGCC_HWVER_GEP1:
		case ACS_APGCC_HWVER_GEP2:
			is_gep5_or_gep7=false;
			break;
		case ACS_APGCC_HWVER_GEP5:
		/*case ACS_APGCC_HWVER_GEP7:*/
			is_gep5_or_gep7=true;
			break;
		case ACS_APGCC_HWVER_VM:
		default:
			ACS_APBM_TRACE_MESSAGE("ERROR: GetHwInfo() - Undefined Hardware Version");
			is_gep5_or_gep7=false;
		}
	}

	if (is_gep5_or_gep7) //GEP5 or GEP7
	{
                if((led_type != acs_apbm_ipmiapi_ns::YELLOW_4LED) &&
                   (led_type != acs_apbm_ipmiapi_ns::RED_4LED) &&
	           (led_type != acs_apbm_ipmiapi_ns::GREEN_4LED) &&
	           (led_type != acs_apbm_ipmiapi_ns::BLUE_4LED)) { 
		ACS_APBM_TRACE_MESSAGE("call '_set_led' failed for hwVersion '%d', led type '%d'. ret_code == %d.", hwInfo.hwVersion, led_type, acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER);
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;
	       }

                //data has byte1=led_type and byte2= on/off/flash
                switch(status){
                    //Actual options passed to set the led
                    //nothing to be done in these cases
                    case C_4LED_STATUS_OFF:
                        break;
                    case C_4LED_STATUS_ON:
                        break;
                    case C_4LED_STATUS_SLOW_BLINK:
                        break;
                    case C_4LED_STATUS_FAST_BLINK:
                        break;
                    case C_4LED_STATUS_BLINK_HOT:
                        break;
                    case C_4LED_STATUS_BLINK_COLD:
                        break;
                    //convert user given options to real options
                    case 1:
                        status =  C_4LED_STATUS_ON;
                        break;
                    case 2:
                        status =  C_4LED_STATUS_SLOW_BLINK;
                        break;
                    case 3:
                        status =  C_4LED_STATUS_FAST_BLINK;
                        break;
                    case 4:
                        status =  C_4LED_STATUS_BLINK_HOT;
                        break;
                    case 5:
                        status =  C_4LED_STATUS_BLINK_COLD;
                        break;
                    default:
		        return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;
                        
                }
                unsigned char data[3]="";
                data[0] =  static_cast<unsigned char>(led_type); //byte 1 
		data[1] = static_cast<unsigned char>(status); //byte 2
		result = _ipmifw_query(G_LED,
				C_4LED_SET,
				SIZE_L3DATA_SETLED_REQUEST+2,//array size 3
				data,
				SIZE_L3DATA_SETLED_RESPONSE,
				NULL,
				L3TIMEOUT_DEFAULT_MSEC);

	}
	else //GEP1/2
	{
                if(status < 0 || status > 1)
                    return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

                if((led_type != acs_apbm_ipmiapi_ns::YELLOW_LED) &&
                   (led_type != acs_apbm_ipmiapi_ns::RED_LED)) {
		ACS_APBM_TRACE_MESSAGE("call '_set_led' failed for hwVersion '%d', led type '%d'. ret_code == %d.", hwInfo.hwVersion, led_type, acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER);
		return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;
        	}

		command_code = (led_type == (acs_apbm_ipmiapi_ns::YELLOW_LED ? C_LED_YELLOW_SET : C_LED_RED_SET));


                unsigned char status_char = static_cast<unsigned char>(status);

		result = _ipmifw_query(G_LED,
				command_code,
				SIZE_L3DATA_SETLED_REQUEST,
				&status_char,
				SIZE_L3DATA_SETLED_RESPONSE,
				NULL,
				L3TIMEOUT_DEFAULT_MSEC);
	}
	return result;
        
}

/****************************************************************************/
// UTILITY METHODS
/****************************************************************************/

bool acs_apbm_ipmiapi_impl::_lock_device()
{
	int acquire_retry = IPMIAPI_OP_RETRY_NUMBER;

	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> tv;
	tv = tv.now();

	int result = 0;

	do {
		tv += ACE_Time_Value(IPMIAPI_MUTEX_ACQUIRE_TIMEOUT_MS / 1000);
		/*
		time_value.tv_sec += (IPMIAPI_MUTEX_ACQUIRE_TIMEOUT_MS / 1000);
		tv.set(time_value);
		*/
		result = _api_mutex.acquire(tv);


	} while (result < 0 && errno == ETIME && --acquire_retry);

	return (result == 0)? true : false;
}

int acs_apbm_ipmiapi_impl::get_pip_protocol_version(unsigned char* ipmi_fw)
{
  ACS_APBM_TRACE_FUNCTION;


  if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
    return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;

  unsigned char fw[3];

  int result = _ipmifw_query(G_INFO,
          04,
          0,
          NULL,
          4,
          fw,
          L3TIMEOUT_DEFAULT_MSEC);


  if (result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
          return result;

  memcpy(ipmi_fw, fw, 3);

  return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;

}

int acs_apbm_ipmiapi_impl::get_ipmi_fwinfo (unsigned char * fw, const unsigned int fw_type ){
          ACS_APBM_TRACE_FUNCTION;

	  if (_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		  return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;
//printf("DEBUG - in get_ipmi_fwinfo fw_type=%d\n",fw_type);
	  // check parameter value
	  if(fw_type != 0 && fw_type != 1 && fw_type != 2 && fw_type != 3 )
	  return acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER;

	  unsigned char subfw = fw_type;
	  unsigned char fw_info_raw[3];

	  int call_result = _ipmifw_query(G_INFO,
					  C_IPMI_FW_INFO,
					  1,
					  &subfw,
					  3,
					  fw_info_raw,
					  L3TIMEOUT_DEFAULT_MSEC);
	  //printf("DEBUG - in get_ipmi_fwinfo - fw= %02X,%02X,%02X \n",fw[0],fw[1],fw[2]);
		if (call_result != acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
			return call_result;

		//TODO: check if it is necessary to do this manipulation !!! perhaps we must only copy received data...
		memcpy(fw, fw_info_raw, 3);

	    return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;

}
int acs_apbm_ipmiapi_impl::ipmi_restart(const unsigned char fw) {
        ACS_APBM_TRACE_FUNCTION;

	if(_ipmiapi_state != acs_apbm_ipmiapi_ns::IPMIAPI_INITIALIZED)
		return acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED;


	int call_result = _ipmifw_query(G_SERVICE,
						  C_IPMI_RESTART,
						  SIZE_L3DATA_IPMIFWRESETSTATUS_COMMAND,
						  &fw,
						  0,
						  NULL,
						  L3TIMEOUT_DEFAULT_MSEC);
	return call_result;
}

// start of TRs - HU25795 & HU57639
int acs_apbm_ipmiapi_impl::init_process_mutex()
{
	ACS_APBM_TRACE_FUNCTION;

	int retval = 0;

	if ((process_mutex == NULL) && (process_mutex = new (std::nothrow) ACE_Process_Mutex(APBMPIP_LIB_PROCESS_MUTEX_NAME, 0, 0777)) == 0)
	{
		ACS_APBM_TRACE_MESSAGE("Unable to create ACE_Process_Mutex");
		retval = -1;
	}

	return retval;
}

int acs_apbm_ipmiapi_impl::acquire_process_mutex()
{
	ACS_APBM_TRACE_FUNCTION;
	int retval = 0;
	int retryCount = 3;

	if(!process_mutex)
	{
		ACS_APBM_TRACE_MESSAGE("Error: named process mutex does not exist. (process_mutex == NULL)");
		retval = -1;
	}

	if(!retval)
	{
		do
		{
			retval = process_mutex->acquire();
			if(!retval)
			{
				ACS_APBM_TRACE_MESSAGE("Successfully acquired named process mutex");
			}
			else
			{
				--retryCount;
				ACS_APBM_TRACE_MESSAGE("Failed to acquire named process mutex -> errno == %d", errno);
			}
		} while(retval && (retryCount > 0));
	}

	return retval;
}

int acs_apbm_ipmiapi_impl::release_process_mutex()
{
	ACS_APBM_TRACE_FUNCTION;
	int retval = 0;
	int retryCount = 3;

	if(!process_mutex)
	{
		ACS_APBM_TRACE_MESSAGE("Error: named process mutex does not exist. (process_mutex == NULL)");
		retval = -1;
	}

	if(!retval)
	{
		do
		{
			retval = process_mutex->release();
			if(!retval)
			{
				ACS_APBM_TRACE_MESSAGE("Successfully released named process mutex ");
			}
			else
			{
				--retryCount;
				ACS_APBM_TRACE_MESSAGE("Failed to release named process mutex -> errno == %d", errno);
			}
		} while(retval && (retryCount > 0));
	}

	return retval;
}

int acs_apbm_ipmiapi_impl::check_and_adjust_system_V_sem()
{
	ACS_APBM_TRACE_FUNCTION;
	// Try to open the System V semaphore used to implement the process mutex
	ACE_SV_Semaphore_Complex sem;
	if (sem.open(APBMPIP_LIB_PROCESS_MUTEX_NAME, ACE_SV_Semaphore_Complex::ACE_OPEN, 1, 1, ACE_DEFAULT_FILE_PERMS))
		return (errno == ENOENT) ? 2 /* the semaphore doesn't exist*/: -2 /* an error occurred while opening the semaphore */;

	// get owner and permissions associated to the semaphore
	semid_ds semInfo;
	semun semArg;
	semArg.buf = &semInfo;
	int op_result = sem.control(IPC_STAT, semArg, 0);
	if(op_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("Unable to retrieve info about the System V semaphore used to implement the ACE Process Mutex, errno == %d", errno);
		return -2;
	}

	if( (semInfo.sem_perm.uid == 0) && (semInfo.sem_perm.gid == 0) && (semInfo.sem_perm.mode == 0777))
		return 0;	// the owner and permissions are right

	// semaphore owner or permissions are wrong. Try to set right values
	semInfo.sem_perm.uid = 0 /*root*/;
	semInfo.sem_perm.gid = 0 /*root*/;
	semInfo.sem_perm.mode = 0777;
	semArg.buf = & semInfo;
	op_result = sem.control(IPC_SET, semArg, 0);
	if(op_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("Unable to set owner or permissions for the System V semaphore used to implement the ACE Process Mutex, errno == %d", errno);
		return -1;
	}

	return 1;	// semaphore ownership and/or permissions successfully adjusted
}

// end of TRs - HU25795 & HU57639
