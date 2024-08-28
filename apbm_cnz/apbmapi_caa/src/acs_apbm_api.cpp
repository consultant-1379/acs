#include "acs_apbm_api_imp.h"

#include <acs_apbm_api.h>

__CLASS_NAME__::__CLASS_NAME__ () : _api_imp(new acs_apbm_api_imp()) {}

__CLASS_NAME__::__CLASS_NAME__ (const __CLASS_NAME__ & /*rhs*/) : _api_imp(0) {}

__CLASS_NAME__ & __CLASS_NAME__::operator= (const __CLASS_NAME__ & /*rhs*/) { return *this; }

__CLASS_NAME__::~__CLASS_NAME__ () { delete _api_imp; }

int __CLASS_NAME__::subscribe_trap (int bitmap, acs_apbm::trap_handle_t & trap_handle) {
	return _api_imp->subscribe_trap (bitmap, trap_handle);
}

int __CLASS_NAME__::unsubscribe_trap (acs_apbm::trap_handle_t trap_handle) {
	return _api_imp->unsubscribe_trap(trap_handle);
}

int __CLASS_NAME__::get_trap (acs_apbm::trap_handle_t trap_handle, acs_apbm_trapmessage & trap_message) {
	return _api_imp->get_trap(trap_handle, trap_message);
}


int __CLASS_NAME__::get_own_slot () {
	return _api_imp->get_own_slot();
}

int __CLASS_NAME__::get_switch_board_info(const acs_apbm::architecture_type_t architecture, uint32_t magazine, int32_t & slot, char (& ipna_str) [16], char (& ipnb_str) [16], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn) {
	return _api_imp->get_switch_board_info(architecture, magazine, slot, ipna_str, ipnb_str, shelf_mgr_state, neighbour_state, fbn);
}


int  __CLASS_NAME__::check_board_presence (const acs_apbm::board_name_t board_name){
	return _api_imp->get_board_presence ( board_name);
}

int  __CLASS_NAME__::get_board_location (const acs_apbm::board_name_t board_name, std::list<int> & ap_ids){
	return _api_imp->get_board_location ( board_name, ap_ids);
}

int  __CLASS_NAME__::get_board_status (const acs_apbm::board_name_t board_name){
	return _api_imp->get_board_status ( board_name);
}

int  __CLASS_NAME__::set_board_status (const acs_apbm::board_name_t board_name, const acs_apbm::reported_board_status_t status){
	return _api_imp->set_board_status ( board_name, status);
}

int __CLASS_NAME__::get_ipmifw_status(uint32_t /*magazine*/, int32_t /*slot*/, uint8_t /*ipmifw_status*/, char (& /*lastupg_date*/) [ACS_APBM_DATA_TIME_SIZE] ){
	return 0;
}

int __CLASS_NAME__::ipmifw_upgrade(uint32_t magazine, int32_t slot, const char ipmipkg_name [ACS_APBM_IPMIPKG_NAME_MAX_SIZE], uint8_t comport  ){
	return _api_imp->ipmifw_upgrade( magazine, slot, ipmipkg_name, comport);
}

int __CLASS_NAME__::get_ipmi_firmware_status(uint32_t magazine, int32_t slot, uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE] ){
	return _api_imp->get_ipmifw_status( magazine, slot, ipmifw_status, lastupg_date);
}

int __CLASS_NAME__::get_ipmifw_data(uint32_t magazine, int32_t  slot, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER] ,  char (& revision) [ACS_APBM_IPMI_REVISION]){
	return _api_imp->get_ipmifw_data(magazine, slot, ipmifw_type, product_number, revision);
}

bool __CLASS_NAME__::is_gep4_gep5()
{
	return _api_imp->is_gep5_gep7();
}

bool __CLASS_NAME__::is_gep5_gep7()
{
	return _api_imp->is_gep5_gep7();
}
bool __CLASS_NAME__::is_hwtype_gep7()
{
	return _api_imp->is_hwtype_gep7();
}

