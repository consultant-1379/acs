#include <new>
#include "acs_apbm_ipmiapi_impl.h"
#include "acs_apbm_ipmiapi_r1.h"


acs_apbm_ipmiapi_r1::acs_apbm_ipmiapi_r1():_ipmiapi_impl(0), _last_impl_error(acs_apbm_ipmiapi_ns::ERR_NO_ERRORS)
{
}


acs_apbm_ipmiapi_r1::~acs_apbm_ipmiapi_r1()
{
	delete _ipmiapi_impl;
}

int acs_apbm_ipmiapi_r1::ipmiapi_init(const char* device_name){

	if(!_ipmiapi_impl && !(_ipmiapi_impl = new (std::nothrow) acs_apbm_ipmiapi_impl()))
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->ipmiapi_init(device_name));
}

int acs_apbm_ipmiapi_r1::ipmiapi_finalize(){

	if(_ipmiapi_impl)
		_ipmiapi_impl->ipmiapi_finalize();

	return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS;
}

int acs_apbm_ipmiapi_r1::get_mac_address(acs_apbm_ipmiapi_ns::mac_adddress_info* mac_address_info) {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_mac_address(mac_address_info));
}

int acs_apbm_ipmiapi_r1::get_product_id (acs_apbm_ipmiapi_ns::product_id* product_id) {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_product_id(product_id));
}

int acs_apbm_ipmiapi_r1::enter_level (unsigned int level){

	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->enter_level(level));
}

int acs_apbm_ipmiapi_r1::get_parameter_tableblock (unsigned int flash_type, unsigned int block_index, unsigned char* buffer){

	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_parameter_tableblock(flash_type, block_index, buffer));
}


int acs_apbm_ipmiapi_r1::get_ipmi_fwinfo (acs_apbm_ipmiapi_ns::ipmi_fwinfo* ipmi_fwinfo){

	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_ipmi_fwinfo(ipmi_fwinfo));
}

int acs_apbm_ipmiapi_r1::get_reset_line_state (unsigned char line_index, unsigned char* state){

	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_reset_line_state(line_index, state));
}

int acs_apbm_ipmiapi_r1::flash_mode_activate (unsigned int flash_type){

	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->flash_mode_activate(flash_type));
}

int acs_apbm_ipmiapi_r1::flash_srecord (const unsigned char* data_record, unsigned char record_size){

	if(!_ipmiapi_impl)
			return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->flash_srecord(data_record, record_size));
}

int acs_apbm_ipmiapi_r1::get_led (unsigned int led_type, unsigned char* led_info_record){

	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_led(led_type, led_info_record));
}

//4LEDs Hardware compatibility
int acs_apbm_ipmiapi_r1::set_led (acs_apbm_ipmiapi_ns::led_type_t led_type, int status) {
        if(!_ipmiapi_impl)
                return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

        return _error_code(_ipmiapi_impl->set_led(led_type, status));

}


int acs_apbm_ipmiapi_r1::set_redled (int status) {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->set_redled(status));
}

int acs_apbm_ipmiapi_r1::set_yellowled (int status) {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->set_yellowled(status));
}

int acs_apbm_ipmiapi_r1::get_ext_alarms (acs_apbm_ipmiapi_ns::sensor_index sensor_index, acs_apbm_ipmiapi_ns::sensor_readings* sensor_readings) {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_ext_alarms(sensor_index, sensor_readings));
}

int acs_apbm_ipmiapi_r1::set_alarm_panel (const acs_apbm_ipmiapi_ns::alarm_panel_values* alarm_panel_values) {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->set_alarm_panel(alarm_panel_values));
}

int acs_apbm_ipmiapi_r1::presence_ack() {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->presence_ack());
}

int acs_apbm_ipmiapi_r1::ipmi_restart() {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->ipmi_restart());
}

int acs_apbm_ipmiapi_r1::get_board_address(unsigned int* board_address) {
	if(!_ipmiapi_impl)
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE;

	return _error_code(_ipmiapi_impl->get_board_address(board_address));
}

int acs_apbm_ipmiapi_r1::last_impl_error() {
	return _last_impl_error;
}


acs_apbm_ipmiapi_ns::error_code_t
acs_apbm_ipmiapi_r1::_error_code(int impl_err)
{
	_last_impl_error = impl_err;

	switch(impl_err)
	{
	case acs_apbm_ipmiapi_ns::ERR_NO_ERRORS:
		return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS;

	case acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC;

	case acs_apbm_ipmiapi_ns::ERR_OP_NOT_SUPPORTED:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED;

	case acs_apbm_ipmiapi_ns::ERR_NODE_BUSY:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY;

	case acs_apbm_ipmiapi_ns::ERR_API_NOT_INITIALIZED:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED;

	case acs_apbm_ipmiapi_ns::ERR_API_ALREADY_INITIALIZED:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_ALREADY_INITIALIZED;

	case acs_apbm_ipmiapi_ns::ERR_INVALID_DEVICE_NAME:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_DEVICE_NAME;

	case acs_apbm_ipmiapi_ns::ERR_OPEN_DEVICE_FAILURE:
	case acs_apbm_ipmiapi_ns::ERR_INIT_LEVEL2_ERROR:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OPEN_DEVICE;

	case acs_apbm_ipmiapi_ns::ERR_EPOLL_CREATE_FAILURE:
	case acs_apbm_ipmiapi_ns::ERR_EPOLL_CTL_FAILURE:
	case acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_ERROR:
	case acs_apbm_ipmiapi_ns::ERR_QUERY_EPOLL_TIMEOUT:
	case acs_apbm_ipmiapi_ns::ERR_QUERY_READ_ERROR:
	case acs_apbm_ipmiapi_ns::ERR_QUERY_RESPONSE_ERROR:
	case acs_apbm_ipmiapi_ns::ERR_QUERY_UNKNOWN_RESPONSE:
	case acs_apbm_ipmiapi_ns::ERR_QUERY_READ_TIMEOUT:
	case acs_apbm_ipmiapi_ns::ERR_QUERY_WRITE_ERROR:
	case acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE:
	case acs_apbm_ipmiapi_ns::ERR_WRITE_LEVEL2_ERROR:
	case acs_apbm_ipmiapi_ns::ERR_READ_LEVEL2_ERROR:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC;

	case acs_apbm_ipmiapi_ns::ERR_INVALID_PARAMETER:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER;

	default:
		return acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC;
	}
}
