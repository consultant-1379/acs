#include "acs_apbm_trace.h"
#include "acs_apbm_primitivedatahandler.h"
#include "acs_apbm_api_imp.h"
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_CLibTypes.h>

#ifndef ACS_APBM_PRIMITIVE_RECEIVE_DSD_TIMEOUT
#define ACS_APBM_PRIMITIVE_RECEIVE_DSD_TIMEOUT 7 * 1000 // 7 sec
#endif

ACS_APBM_TRACE_DEFINE(acs_apbm_api);

ACS_DSD_Node __CLASS_NAME__::_node;

int get_switch_board_primitive_sent = 0;


__CLASS_NAME__::__CLASS_NAME__ () {
gep5_gep7 = false;
hwtype_gep7 = false;
}

__CLASS_NAME__::__CLASS_NAME__ (const __CLASS_NAME__ & /*rhs*/) {}

__CLASS_NAME__::~__CLASS_NAME__ () { _session.close(); }

__CLASS_NAME__ & __CLASS_NAME__::operator=(const __CLASS_NAME__ & /*rhs*/)  { 
gep5_gep7 = false;
hwtype_gep7 = false;
return *this; }

int __CLASS_NAME__::connect(unsigned timeout) {
        ACS_APBM_TRACE_FUNCTION;

        int call_result = 0;
	const char * apbm_unix_sap_address = acs_apbm_configurationhelper::apbmapi_unix_socket_sap_pathname();
	const char * apbm_inet_sap_address = acs_apbm_configurationhelper::apbmapi_inet_socket_sap();

	_client_connector.get_local_node(_node);

	ACS_DSD_Node other_node;
	call_result = _client_connector.get_partner_node(other_node);

	/**********************************************************************/

	//char buffer [1024] = {0};
	//ACS_APBM_TRACE_DUMP(buffer, ACS_APBM_ARRAY_SIZE(buffer), 512);

	//ACS_APBM_TRACE_MESSAGE("try trace message");
	//ACS_APBM_TRACE_MESSAGE("tray trace message with parameter: call_result == %d", call_result);

	/**********************************************************************/

	//check session state
	if ( (_session.state() && acs_dsd::SESSION_STATE_CONNECTED) )
		return 0; //printf("session already initialized, connection no needed \n");
	else
	{
		switch(_node.node_state)
		{
		case acs_dsd::NODE_STATE_ACTIVE: /*ACTIVE*/
			ACS_APBM_TRACE_MESSAGE("trying to connect with unix socket %s \n", apbm_unix_sap_address);
			call_result = _client_connector.connect(_session, apbm_unix_sap_address, timeout);
			if(call_result < 0)
			{
				ACS_APBM_TRACE_MESSAGE("connection with unix socket result == %d \n", call_result);
				return call_result;
			}
			break;
		case acs_dsd::NODE_STATE_PASSIVE: /*PASSIVE*/
			ACS_APBM_TRACE_MESSAGE("trying to connect with inet socket %s \n", apbm_inet_sap_address);
		  	call_result = _client_connector.connect(_session, apbm_inet_sap_address, _node.system_id, other_node.node_name);
			if(call_result < 0)
			{
				ACS_APBM_TRACE_MESSAGE("connection with inet socket result == %d TEXT: %s \n", call_result, _client_connector.last_error_text() );
				return call_result;
			}
			break;
		default:
			return acs_apbm::ERR_GETTING_NODE_STATE;
		}
	}
	ACS_APBM_TRACE_MESSAGE("SERVICE CONNECTED\n");
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::connect_unix() {
	ACS_APBM_TRACE_FUNCTION;

	const char * apbm_sap_address = acs_apbm_configurationhelper::apbmapi_unix_socket_sap_pathname();

	//check session state
	if (_session.state() && acs_dsd::SESSION_STATE_CONNECTED)
	{
		ACS_APBM_TRACE_MESSAGE("session already initialized, connection no needed \n");
		return 0;
	}
	else
	{
	int call_result = 0;
		ACS_APBM_TRACE_MESSAGE("connecting to the service throw apbm_sap_address %s",apbm_sap_address);
		call_result = _client_connector.connect(_session,apbm_sap_address);
		if (call_result < 0)
		{
			ACS_APBM_TRACE_MESSAGE("ERROR: 'init_connection >> _client_connector.connect(...' failed!\n"  "      last error == %d\n" "      last error text == '%s'\n", _session.last_error(), _session.last_error_text());
			return _session.last_error();
		}
	}
	ACS_APBM_TRACE_MESSAGE("SERVICE CONNECTED\n");
	return acs_apbm::ERR_NO_ERRORS;
}


int __CLASS_NAME__::send_primitive_subscribe_request(const int primitive_id,const int primitive_version,const int bitmap ) {
  ACS_APBM_TRACE_FUNCTION;

	int call_result;
	int byte_sent;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id,primitive_version,bitmap);
	if (call_result < 0) {
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_subscribe_request >> pdh.make_primitive(...' failed!\n"	"      last error == %d\n"	, call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_subscribe_request >> make_primitive...OK! - result: %d \n", call_result);
	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0)
	{
		ACS_APBM_TRACE_MESSAGE(	"ERROR: 'send_primitive_subscribe_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"	, byte_sent);
		return acs_apbm::ERR_SEND_PRIMITIVE_SUBSCRIBE;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_subscribe_request >> send_primitive...OK! - byte sent: %d \n",byte_sent);
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_unsubscribe_request(const int primitive_id,const int primitive_version) {
        ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id,primitive_version);
	if (call_result < 0) {
		ACS_APBM_TRACE_MESSAGE(	"ERROR: 'send_primitive_unsubscribe_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"	, call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_unsubscribe_request >> make_primitive...OK! - result: %d \n", call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0)
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_unsubscribe_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"	, byte_sent);
		return acs_apbm::ERR_SEND_PRIMITIVE_UNSUBSCRIBE;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_unsubscribe_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_get_trap_request(const int primitive_id,const int primitive_version) {
  ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int call_result;
	int byte_sent;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id,primitive_version);
	if (call_result < 0) {
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_get_trap_request >> pdh.make_primitive(...' failed!\n" "     last error == %d\n" , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_trap_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if ( byte_sent <= 0) {
		ACS_APBM_TRACE_MESSAGE(	"ERROR: 'send_primitive_get_trap_request >> pdh.send_primitive(...' failed!\n"	"      last error == %d\n"	, byte_sent);
		return acs_apbm::ERR_SEND_PRIMITIVE_GET_TRAP;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_trap_request >> send_primitive...OK! - result: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_switch_board_data_request(const int primitive_id, const int primitive_version, acs_apbm::architecture_type_t architecture, uint32_t magazine) {
        ACS_APBM_TRACE_FUNCTION;

	int call_result;
	int byte_sent;
	acs_apbm_primitivedatahandler<> pdh(_session);
	//make primitive
	call_result = pdh.make_primitive(primitive_id, primitive_version, architecture, magazine);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_switch_board_data_request >> pdh.make_primitive(...' failed!\n" "     last error == %d\n" , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_switch_board_data_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if ( byte_sent <= 0)
	{
		ACS_APBM_TRACE_MESSAGE(	"ERROR: 'send_primitive_switch_board_data_request >> pdh.send_primitive(...' failed!\n"	"      last error == %d\n"	, byte_sent);
		return acs_apbm::ERR_SEND_PRIMITIVE_SWITCH_BOARD_DATA;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_switch_board_data_request >> send_primitive...OK! - result: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_get_own_slot_request(const int primitive_id,const int primitive_version) {
	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id,primitive_version);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_get_own_slot_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_own_slot_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0) {
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_get_own_slot_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
			//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_own_slot_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_board_presence_request(const int primitive_id,const int primitive_version, const acs_apbm::board_name_t board_name){

	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id, primitive_version, board_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_board_presence_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_board_presence_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_board_presence_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_board_presence_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_board_location_request(const int primitive_id, const int primitive_version, const acs_apbm::board_name_t board_name){

	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id, primitive_version, board_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_board_location_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_board_location_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_board_location_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_board_location_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;

}

int __CLASS_NAME__::send_primitive_board_status_request(const int primitive_id, const int primitive_version, const acs_apbm::board_name_t board_name){

	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id, primitive_version, board_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_board_status_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_board_status_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_board_status_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_board_status_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;


}

int __CLASS_NAME__::send_primitive_set_board_status_request(const int primitive_id, const int primitive_version, const acs_apbm::board_name_t board_name, int8_t board_status){

	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;

	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id, primitive_version, board_name, board_status);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_set_board_status_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_board_status_request: board_name == %d board status == %d >> make_primitive...OK! - result: %d \n",board_name, board_status, call_result);


	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_set_board_status_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_set_board_status_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;

}


int __CLASS_NAME__::send_primitive_get_ipmifw_status_request(const int primitive_id, const int primitive_version, uint32_t magazine, int32_t slot)
{
	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id, primitive_version, magazine, slot);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_get_ipmifw_status_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_ipmifw_status_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_get_ipmifw_status_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
		return acs_apbm::ERR_SEND_PRIMITIVE;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_ipmifw_status_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_ipmifw_upgrade_request(const int primitive_id, const int primitive_version, uint32_t magazine, int32_t slot, uint8_t comport, const char ipmipkg_name [ACS_APBM_IPMIPKG_NAME_MAX_SIZE]){

	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id, primitive_version, magazine, slot, comport, ipmipkg_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_ipmifw_upgrade_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_ipmifw_upgrade_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_ipmifw_upgrade_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
		return acs_apbm::ERR_SEND_PRIMITIVE;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_ipmifw_upgrade_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::send_primitive_get_ipmifw_data_request(const int primitive_id, const int primitive_version, uint32_t magazine, int32_t slot)
{
	ACS_APBM_TRACE_FUNCTION;

	//make primitive
	int byte_sent;
	int call_result;
	acs_apbm_primitivedatahandler<> pdh(_session);
	call_result = pdh.make_primitive(primitive_id, primitive_version, magazine, slot);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_get_ipmifw_data_request >> pdh.make_primitive(...' failed!\n"  "     last error == %d\n"    , call_result);
		return call_result;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_ipmifw_data_request >> make_primitive...OK! - result: %d \n",call_result);

	//send primitive
	byte_sent = pdh.send_primitive();
	if (byte_sent <= 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'send_primitive_get_ipmifw_data_request >> pdh.send_primitive(...' failed!\n" "      last error == %d\n"   , byte_sent);
		return acs_apbm::ERR_SEND_PRIMITIVE;
	}
	else
		ACS_APBM_TRACE_MESSAGE("send_primitive_get_ipmifw_data_request >> send_primitive...OK! - byte sent: %d \n", byte_sent);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unpack_primitive_get_sel_trap(const acs_apbm_primitivedatahandler<> & pdh,
                                                  int & mag_plug_number,int & slot_pos,
                                                  char messages[acs_apbm::PCP_FIELD_SIZE_TRAP_MSG]){
  ACS_APBM_TRACE_FUNCTION;

	//unpack primitive
	int call_result;
	int primitive_id = acs_apbm::PCP_GET_SEL_TRAP_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id,primitive_version, & mag_plug_number, & slot_pos, messages);
	if(call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_sel_trap >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;		// an error occurred while getting handles
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_sel_trap >> pdh.unpack_primitive ...OK! - result: %d \n",call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: mag_plug: (%d) - slot_pos: (%d) - messages:(%s) \n", mag_plug_number , slot_pos , messages);
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unpack_primitive_get_board_presence_trap(const acs_apbm_primitivedatahandler<> & pdh,
                                                            int & mag_plug_number, int & slot_pos,
                                                            int & hwBoardPresence, int & busType,
                                                            char messages[acs_apbm::PCP_FIELD_SIZE_TRAP_MSG]){
        ACS_APBM_TRACE_FUNCTION;

	//unpack primitive
	int call_result;
	int primitive_id = acs_apbm::PCP_GET_BOARD_PRESENCE_TRAP_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id,primitive_version, & mag_plug_number, & slot_pos, & hwBoardPresence, & busType, messages);
	if(call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_board_presence_trap >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;		// an error occurred while getting handles
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_board_presence_trap >> pdh.unpack_primitive ...OK! - result: %d \n",call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: mag_plug: (%d) - slot_pos: (%d) - messages:(%s) \n",mag_plug_number, slot_pos, messages);
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unpack_primitive_get_sensor_state_change_trap(const acs_apbm_primitivedatahandler<> & pdh,
                                                                  int & mag_plug_number, int & slot_pos,
                                                                  int & sensorType, int & sensorID,
                                                                  int & sensorTypeCode, char sensorEventData[acs_apbm::PCP_FIELD_SIZE_TRAP_MSG]){
        ACS_APBM_TRACE_FUNCTION;

	//unpack primitive
	int call_result = -1;
	int primitive_id = acs_apbm::PCP_GET_SENSOR_STATE_CHANGE_TRAP_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & mag_plug_number, & slot_pos,
										& sensorType, & sensorID, & sensorTypeCode, sensorEventData);
	if(call_result < 0){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_sensor_state_change_trap >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;		// an error occurred while getting handles
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_sensor_state_change_trap >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: mag_plug: (%d) - slot_pos: (%d) \n",mag_plug_number, slot_pos);
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unpack_primitive_get_scb_data(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, int32_t & slot, char (& ipna_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], char (& ipnb_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state){
        ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_GET_SCB_DATA_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & err_code, & slot, ipna_str, ipnb_str, & shelf_mgr_state, & neighbour_state);
	if(call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_scb_data >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_scb_data >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: err_code = %d, slot %d, ipna_str %s, ipnb_str %s, shelf_mgr_state %d, neighbour_state %d result == %d \n", err_code, slot, ipna_str, ipnb_str, shelf_mgr_state,  neighbour_state, call_result);
	}
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unpack_primitive_get_Egem2L2Switch_data(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, uint32_t & magazine, int32_t & slot, char (& ipna_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], char (& ipnb_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn){
        ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = pdh.primitive_id();
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & err_code, & magazine, & slot, ipna_str, ipnb_str, & shelf_mgr_state, & neighbour_state, & fbn);
	if(call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_scxb_data >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_scxb_data >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: err_code = %d, magazine %d ,slot %d, ipna_str %s, ipnb_str %s, shelf_mgr_state %d, neighbour_state %d, fbn %d,  result == %d \n",
				 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 err_code, magazine, slot, ipna_str, ipnb_str, shelf_mgr_state,  neighbour_state, fbn, call_result);
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unpack_primitive_get_own_slot(const acs_apbm_primitivedatahandler<> & pdh, int32_t & slot){
        ACS_APBM_TRACE_FUNCTION;

        int call_result = -1;
        int primitive_id = acs_apbm::PCP_GET_OWN_SLOT_REPLY_ID;
        unsigned primitive_version = 1;
        int error_code = 0;
        call_result = pdh.unpack_primitive(primitive_id, primitive_version, & error_code, & slot);
        if(call_result < 0)
        {
        	ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_own_slot >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
          return call_result;
        }
        else
        {
      		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_scxb_data >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
      		ACS_APBM_TRACE_MESSAGE("...received values: error_code %d, slot %d \n", error_code, slot);
        }

        return error_code;
}

int __CLASS_NAME__::unpack_primitive_board_presence_reply(const acs_apbm_primitivedatahandler<> & pdh, int8_t & board_presence){
	ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_BOARD_PRESENCE_REPLY_ID;
	unsigned primitive_version = 1;
	int error_code = 0;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & error_code, & board_presence);
	if(call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_board_presence_reply >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;
	}
	else {
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_board_presence_reply >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: error_code %d, board_presence %d \n", error_code, board_presence);
	}

	return error_code;
}

int __CLASS_NAME__::unpack_primitive_board_status_reply(const acs_apbm_primitivedatahandler<> & pdh, int8_t & board_status){

	ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_BOARD_STATUS_REPLY_ID;
	unsigned primitive_version = 1;
	int error_code = 0;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & error_code, & board_status);
	if(call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_board_status_reply >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;
	}
	else {
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_board_status_reply >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: error_code %d, board_status %d \n", error_code, board_status);
	}

	return error_code;

}

int __CLASS_NAME__::unpack_primitive_board_location_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, int32_t & ap_sys_no){

	ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_BOARD_LOCATION_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & err_code, & ap_sys_no);
	if(call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_board_location_reply >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;
	}
	else {
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_board_location_reply >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: error_code %d, ap_sys_no %d \n", err_code, ap_sys_no);
	}

	return err_code;
}

int __CLASS_NAME__::unpack_primitive_set_board_status_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code){
	ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_SET_BOARD_STATUS_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & err_code);
	if(call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_set_board_status_reply >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n", call_result);
		return call_result;
	}
	else {
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_set_board_status_reply >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: error_code %d\n", err_code);
	}

	return call_result;
}

int __CLASS_NAME__::unpack_primitive_get_ipmifw_status_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE]){
    ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_GET_IPMIUPG_STATUS_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & err_code, & ipmifw_status, lastupg_date);
	if(call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_ipmifw_status_reply >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_ipmifw_status_reply >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: err_code = %d, ipmifw_status %d, lastupg_date %s result == %d \n", err_code, ipmifw_status, lastupg_date, call_result);
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unpack_primitive_ipmifw_upgrade_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code){

	ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_IPMIUPG_UPGRADE_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & err_code);
	if(call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_ipmifw_upgrade_reply >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n", call_result);
		return call_result;
	}
	else {
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_ipmifw_upgrade_reply >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: error_code %d\n", err_code);
	}

	return call_result;
}

int __CLASS_NAME__::unpack_primitive_get_ipmifw_data_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER], char (& revision) [ACS_APBM_IPMI_REVISION]){
    ACS_APBM_TRACE_FUNCTION;

	int call_result = -1;
	int primitive_id = acs_apbm::PCP_GET_IPMIFW_DATA_REPLY_ID;
	unsigned primitive_version = 1;
	call_result = pdh.unpack_primitive(primitive_id, primitive_version, & err_code, & ipmifw_type, product_number, revision);
	if(call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: 'unpack_primitive_get_ipmifw_data_reply >> pdh.unpack_primitive(...' failed!\n" "      last error == %d\n",call_result);
		return call_result;
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("  unpack_primitive_get_ipmifw_data_reply >> pdh.unpack_primitive ...OK! - result: %d \n", call_result);
		ACS_APBM_TRACE_MESSAGE("...received values: err_code = %d, ipmifw_type %d, product_number %s revision %s ...result == %d \n", err_code, ipmifw_type, product_number, revision, call_result);
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::receive_primitive_notification(){
        ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	//receive primitive acs_apbm::PCP_GET_SEL_TRAP_REPLY_ID
	ACS_APBM_TRACE_MESSAGE("wait for receive primitive notification trap\n");
	if(timeout_ms == 0)
	{
		if ((bytes_received = pdh.recv_primitive()) <= 0)
		{
			ACS_APBM_TRACE_MESSAGE( "ERROR: 'receive_primitive__notification >> pdh.recv_primitive(...' failed!\n"	"      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;		// an error occurred while getting handles
		}
		else
			ACS_APBM_TRACE_MESSAGE("receive_primitive_notification >> pdh.recv_primitive...OK! - bytes received: %zd \n",bytes_received);
	}
	else
	{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout))<= 0)
		{
			ACS_APBM_TRACE_MESSAGE(	"ERROR: 'receive_primitive__notification >> pdh.recv_primitive(...' failed!\n"	"      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;		// an error occurred while getting handles
		}
		else
			ACS_APBM_TRACE_MESSAGE("receive_primitive__notification >> pdh.recv_primitive...OK! - bytes received: %zd \n",bytes_received);
	}

	if ( (pdh.primitive_id()) == acs_apbm::PCP_NOTIFICATION_TRAP_MSG_ID )
	{
		ACS_APBM_TRACE_MESSAGE("received primitive notification \n");
		return acs_apbm::ERR_NO_ERRORS;
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("received none primitive notification \n");
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
}
int __CLASS_NAME__::receive_primitive_get_trap(acs_apbm_trapmessage & trap_msg) {
        ACS_APBM_TRACE_FUNCTION;

        //primitive variable
	int primitive_id = -1;
	//int call_result = -1;

	//traps variable
	int mag_plug_number = -1;
	int slot_pos = -1;
	int hwBoardPresence = -1;
	int busType = -1;

	int sensorType = -1;
	int sensorID = -1;
	int sensorTypeCode = -1;

	char messages[acs_apbm::PCP_FIELD_SIZE_TRAP_MSG];

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	//receive primitive acs_apbm::PCP_GET_SEL_TRAP_REPLY_ID
	if(timeout_ms == 0)
	{
		if ((bytes_received = pdh.recv_primitive()) <= 0)
		{
			ACS_APBM_TRACE_MESSAGE( "ERROR: 'receive_primitive_get_trap >> pdh.recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_TRAP;		// an error occurred while getting handles
		}
		else
			ACS_APBM_TRACE_MESSAGE("receive_primitive_get_trap >> pdh.recv_primitive...OK! - bytes received: %zd \n", bytes_received);
	}
	else
	{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout))<= 0)
		{
			ACS_APBM_TRACE_MESSAGE(	"ERROR: 'receive_primitive_get_trap >> pdh.recv_primitive(...' failed!\n"	"      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_TRAP;		// an error occurred while getting handles
		}
		else
			ACS_APBM_TRACE_MESSAGE("receive_primitive_get_trap >> pdh.recv_primitive...OK! - bytes received: %zd \n",bytes_received);
	}
	//check primitive id
	primitive_id = pdh.primitive_id();
        int call_result;

		switch(primitive_id)
		{
			case (acs_apbm::PCP_GET_SEL_TRAP_REPLY_ID):{
                            call_result = unpack_primitive_get_sel_trap(pdh, mag_plug_number, slot_pos, messages);
				if (call_result == 0)
				{
					ACS_APBM_TRACE_MESSAGE("receive_primitive_get_trap >> unpack_primitive_get_sel_trap ...OK! - result: %d \n",call_result);
					trap_msg.set_OID(trap_msg.SEL_ENTRY);
					trap_msg.set_message(messages, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);
					std::vector<int> tmp;
					tmp.push_back(mag_plug_number); // at(0)
					tmp.push_back(slot_pos);		// at(1)
					trap_msg.set_values(tmp);
					tmp.clear();
					return acs_apbm::ERR_NO_ERRORS;
				}
			break;
}
			case (acs_apbm::PCP_GET_BOARD_PRESENCE_TRAP_REPLY_ID):{
                                 call_result = unpack_primitive_get_board_presence_trap(pdh, mag_plug_number, slot_pos, hwBoardPresence, busType, messages);
				if(call_result == 0)
				{
					ACS_APBM_TRACE_MESSAGE("receive_primitive_get_trap >> unpack_primitive_get_board_presence_trap ...OK! - result: %d \n",call_result);
					trap_msg.set_OID(trap_msg.BOARD_PRESENCE);
					trap_msg.set_message(messages, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);
					std::vector<int> tmp;
					tmp.push_back(mag_plug_number);	// at(0)
					tmp.push_back(slot_pos);		// at(1)
					tmp.push_back(hwBoardPresence); // at(2)
					tmp.push_back(busType);			// at(3)
					trap_msg.set_values(tmp);
					tmp.clear();
					return acs_apbm::ERR_NO_ERRORS;
				}
			break;
}
			case (acs_apbm::PCP_GET_SENSOR_STATE_CHANGE_TRAP_REPLY_ID):{
                           call_result = unpack_primitive_get_sensor_state_change_trap(pdh, mag_plug_number, slot_pos,sensorType, sensorID, sensorTypeCode, messages);
				if(call_result == 0)
				{
					ACS_APBM_TRACE_MESSAGE("receive_primitive_get_trap >> unpack_primitive_get_sensor_state_change_trap ...OK! - result: %d \n",call_result);
					trap_msg.set_OID(trap_msg.SENSOR_STATE_CHANGE);
					trap_msg.set_message(messages, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG); //message == SensorEventData
					std::vector<int> tmp;
					tmp.push_back(mag_plug_number);	// at(0)
					tmp.push_back(slot_pos);		// at(1)
					tmp.push_back(sensorType); 		// at(2)
					tmp.push_back(sensorID);		// at(3)
					tmp.push_back(sensorTypeCode);	// at(4)
					trap_msg.set_values(tmp);
					tmp.clear();
					return acs_apbm::ERR_NO_ERRORS;
				}
				break;
 }
		}
		//log error
		ACS_APBM_TRACE_MESSAGE("ERROR: receive_primitive_get_trap >> unpack_primitive_xxx_trap(...' failed! or primitive unrecognized!\n");
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_TRAP;
}

int __CLASS_NAME__::receive_primitive_switch_board_data(uint32_t magazine, int32_t & slot, char (& ipna_str) [16], char (& ipnb_str) [16], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn){
        ACS_APBM_TRACE_FUNCTION;
	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = 0;// making recv as blocking call inorder not to get DSD timeout

	//receive primitive acs_apbm::PCP_GET_SEL_TRAP_REPLY_ID
	if(timeout_ms == 0)
	{
		if ((bytes_received = pdh.recv_primitive()) <= 0)
		{
			ACS_APBM_TRACE_MESSAGE( "ERROR: 'receive_primitive_switch_board_data >> pdh.recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_SWITCH_BOARD_DATA;		// an error occurred while getting handles
		}
		else
			ACS_APBM_TRACE_MESSAGE("receive_primitive_switch_board_data >> pdh.recv_primitive...OK!! - bytes received: %zd \n",bytes_received);
	}
	else
	{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(&timeout)) <= 0)
		{
			if (_session.last_error() == acs_dsd::ERR_SYSTEM_RECEIVE /*timeout-expired*/)//tr hl97410
			{
			     ACS_APBM_TRACE_MESSAGE("WARNING: 'receive_primitive_switch_board_data >> recv_primitive(...' ! error == %zd - last_error_test=%s  - last_error=%d\n", bytes_received, _session.last_error_text(),_session.last_error());
			     return acs_apbm::PCP_ERROR_CODE_DSD_REPLY_TIME_OUT;
			}
			else
			{
			ACS_APBM_TRACE_MESSAGE( "ERROR: 'receive_primitive_switch_board_data >> pdh.recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_SWITCH_BOARD_DATA;		// an error occurred while getting handles
			}
		}
		else
			ACS_APBM_TRACE_MESSAGE("receive_primitive_switch_board_data >> pdh.recv_primitive...OK! - bytes received: %zd \n",bytes_received);
	}
	//check primitive id
	int primitive_id = -1;
	primitive_id = pdh.primitive_id();

	int err_code = -1;
    int call_result;
	switch(primitive_id)
	{
		case (acs_apbm::PCP_GET_SCB_DATA_REPLY_ID):{
								    call_result = unpack_primitive_get_scb_data(pdh, err_code, slot, ipna_str, ipnb_str, shelf_mgr_state, neighbour_state);
								   if(call_result< 0)
								   {
									   ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_switch_board_data >> unpack_primitive_get_scb_data(...' failed!\n" "      last error == %d\n",call_result);
									   return acs_apbm::ERR_RECEIVE_PRIMITIVE_SWITCH_BOARD_DATA;
								   }
								   break;
							   }
		case (acs_apbm::PCP_GET_EGEM2L2_SWITCH_DATA_REPLY_ID):{
								     call_result = unpack_primitive_get_Egem2L2Switch_data(pdh, err_code, magazine, slot, ipna_str, ipnb_str, shelf_mgr_state, neighbour_state, fbn);
								    if(call_result < 0)
								    {
									    ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_switch_board_data >> unpack_primitive_get_scxb_data(...' failed!\n" "      last error == %d\n",call_result);
									    return acs_apbm::ERR_RECEIVE_PRIMITIVE_SWITCH_BOARD_DATA;
								    }
								    if (err_code == acs_apbm::PCP_ERROR_CODE_END_OF_DATA)
									    get_switch_board_primitive_sent = 0;
								    break;
							    }
	}
	return err_code;
}

int __CLASS_NAME__::receive_primitive_get_own_slot(int32_t & slot){
	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	//receive primitive acs_apbm::PCP_GET_OWN_SLOT_REPLY_ID
	if(timeout_ms == 0)
	{
		if ((bytes_received = pdh.recv_primitive()) <= 0)
		{
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_own_slot >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
		//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;
	}
	else
	{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0)
		{
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_own_slot >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
		//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;
	}

	//check primitive id
	int primitive_id = -1;
	int call_result;
	primitive_id = pdh.primitive_id();

	call_result = unpack_primitive_get_own_slot(pdh, slot);
	if(primitive_id == acs_apbm::PCP_GET_OWN_SLOT_REPLY_ID)
		if( call_result < 0 )
		{
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_own_slot >> unpack_primitive_get_own_slot(...' failed!\n" "      last error == %d\n",call_result);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
	//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::receive_primitive_board_presence(int8_t & board_presence){
	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	//receive primitive acs_apbm::PCP_BOARD_PRESENCE_REPLY_ID
	if(timeout_ms == 0){
		if ((bytes_received = pdh.recv_primitive()) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_presence >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
		//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_BOARD_PRESENCE;
	}
	else {
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_presence >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
		//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_BOARD_PRESENCE;
	}

	//check primitive id
	int primitive_id = -1;
	int call_result;
	primitive_id = pdh.primitive_id();

	call_result = unpack_primitive_board_presence_reply(pdh, board_presence);
	if(primitive_id == acs_apbm::PCP_BOARD_PRESENCE_REPLY_ID)
		if( call_result < 0 ){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_presence >> unpack_primitive_board_presence(...' failed!\n" "      last error == %d\n",call_result);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
	//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::receive_primitive_board_status(int8_t & board_status){

	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	//receive primitive acs_apbm::PCP_BOARD_PRESENCE_REPLY_ID
	if(timeout_ms == 0){
		if ((bytes_received = pdh.recv_primitive()) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_status >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
		//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_BOARD_PRESENCE;
	}
	else {
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_status >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
		//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_BOARD_PRESENCE;
	}

	//check primitive id
	int primitive_id = -1;
	int call_result;
	primitive_id = pdh.primitive_id();

	call_result = unpack_primitive_board_status_reply(pdh, board_status);
	if(primitive_id == acs_apbm::PCP_BOARD_STATUS_REPLY_ID)
		if( call_result < 0 ){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_status >> unpack_primitive_board_status(...' failed!\n" "      last error == %d\n",call_result);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
	//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::receive_primitive_board_location(int & ap_sys_no){
	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	if(timeout_ms == 0){
		if ((bytes_received = pdh.recv_primitive()) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_location >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
	}
	else {
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_location >> recv_primitive(...' failed!\n" "      last error == %zd\n",bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
	}

	//check primitive id
	int primitive_id = -1;
	primitive_id = pdh.primitive_id();

	if(primitive_id != acs_apbm::PCP_BOARD_LOCATION_REPLY_ID){
		ACS_APBM_TRACE_MESSAGE("ERROR: Unexpected primitive received <primitive_id == %d>", primitive_id);
		return acs_apbm::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	int call_result;
	int err_code = -1;
	call_result = unpack_primitive_board_location_reply(pdh, err_code, ap_sys_no);
	if( call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_board_location >> unpack_primitive_board_location(...' failed!\n" "      last error == %d\n",call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}

	if (err_code == acs_apbm::PCP_ERROR_CODE_END_OF_DATA) return err_code;

	return acs_apbm::ERR_NO_ERRORS;;
}

int __CLASS_NAME__::receive_primitive_set_board_status_reply(int32_t & err_code){
	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	if(timeout_ms == 0){
		if ((bytes_received = pdh.recv_primitive()) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_set_board_status_reply >> recv_primitive(...' failed!\n" "      last error == %zd\n", bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
	}
	else {
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_set_board_status_reply >> recv_primitive(...' failed!\n" "      last error == %zd\n", bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
	}

	//check primitive id
	int primitive_id = -1;
	primitive_id = pdh.primitive_id();

	if(primitive_id != acs_apbm::PCP_SET_BOARD_STATUS_REPLY_ID){
		ACS_APBM_TRACE_MESSAGE("ERROR: Unexpected primitive received <primitive_id == %d>", primitive_id);
		return acs_apbm::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	int call_result;
	call_result = unpack_primitive_set_board_status_reply(pdh, err_code);
	if( call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_set_board_status_reply >> unpack_primitive_set_board_status_reply(...' failed!\n" "      last error == %d\n",call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}

	return acs_apbm::ERR_NO_ERRORS;

}

int __CLASS_NAME__::receive_primitive_get_ipmifw_status_reply(uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE]){
	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	if(timeout_ms == 0){
		if ((bytes_received = pdh.recv_primitive()) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_ipmifw_status_reply >> recv_primitive(...' failed!\n" "      last error == %zd\n", bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE;
		}
	}
	else {
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_ipmifw_status_reply >> recv_primitive(...' failed!\n" "      last error == %zd\n", bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE;
		}
	}

	//check primitive id
	int primitive_id = -1;
	primitive_id = pdh.primitive_id();

	if(primitive_id != acs_apbm::PCP_GET_IPMIUPG_STATUS_REPLY_ID){
		ACS_APBM_TRACE_MESSAGE("ERROR: Unexpected primitive received <primitive_id == %d>", primitive_id);
		return acs_apbm::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	int call_result;
	int32_t err_code;
	call_result = unpack_primitive_get_ipmifw_status_reply(pdh, err_code, ipmifw_status, lastupg_date);
	if( call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_ipmifw_status_reply >> unpack_primitive_get_ipmifw_status_reply(...' failed!\n" "      last error == %d\n",call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE;
	}

	// TODO: if(err_code) return ????
	return acs_apbm::ERR_NO_ERRORS;

}

int __CLASS_NAME__::receive_primitive_ipmifw_upgrade_reply(int32_t & err_code, uint8_t comport){

	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	if(timeout_ms == 0){
		if ((bytes_received = pdh.recv_primitive()) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_ipmifw_upgrade_reply >> recv_primitive(...' failed!\n" "      last error == %zd\n", bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE;
		}
	}
	else {
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0){
			if ((comport != acs_apbm::NO_COM) && (_session.last_error() == acs_dsd::ERR_SYSTEM_RECEIVE /*timeout-expired*/))
			{
				ACS_APBM_TRACE_MESSAGE("WARNING: 'receive_primitive_ipmifw_upgrade_reply >> recv_primitive(...' ! error == %zd - last_error_test=%s  - last_error=%d\n", bytes_received, _session.last_error_text(),_session.last_error());
				err_code = 0;
				return acs_apbm::ERR_NO_ERRORS;
			}
			else{
				ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_ipmifw_upgrade_reply >> recv_primitive(...' failed! error == %zd - last_error_test=%s  - last_error=%d\n", bytes_received, _session.last_error_text(),_session.last_error());
				return acs_apbm::ERR_RECEIVE_PRIMITIVE;
			}
		}
	}
	//check primitive id
	int primitive_id = -1;
	primitive_id = pdh.primitive_id();

	if(primitive_id != acs_apbm::PCP_IPMIUPG_UPGRADE_REPLY_ID){
		ACS_APBM_TRACE_MESSAGE("ERROR: Unexpected primitive received <primitive_id == %d>", primitive_id);
		return acs_apbm::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	int call_result;
	call_result = unpack_primitive_ipmifw_upgrade_reply(pdh, err_code);
	if( call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_ipmifw_upgrade_reply >> unpack_primitive_ipmifw_upgrade_reply(...' failed!\n" "      last error == %d\n",call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE;
	}

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::receive_primitive_get_ipmifw_data_reply(int32_t & err_code, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER], char (& revision) [ACS_APBM_IPMI_REVISION]){
	ACS_APBM_TRACE_FUNCTION;

	acs_apbm_primitivedatahandler<> pdh(_session);
	ssize_t bytes_received;
	uint32_t timeout_ms = acs_apbm_configurationhelper::primitive_receive_timeout();

	if(timeout_ms == 0){
		if ((bytes_received = pdh.recv_primitive()) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_ipmifw_status_reply >> recv_primitive(...' failed!\n" "      last error == %zd\n", bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE;
		}
	}
	else {
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		if ((bytes_received = pdh.recv_primitive(& timeout)) <= 0){
			ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_ipmifw_status_reply >> recv_primitive(...' failed!\n" "      last error == %zd\n", bytes_received);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE;
		}
	}

	//check primitive id
	int primitive_id = -1;
	primitive_id = pdh.primitive_id();

	if(primitive_id != acs_apbm::PCP_GET_IPMIFW_DATA_REPLY_ID){
		ACS_APBM_TRACE_MESSAGE("ERROR: Unexpected primitive received <primitive_id == %d>", primitive_id);
		return acs_apbm::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	int call_result;
	call_result = unpack_primitive_get_ipmifw_data_reply(pdh, err_code, ipmifw_type, product_number, revision);
	if( call_result < 0 ){
		ACS_APBM_TRACE_MESSAGE("ERROR: 'receive_primitive_get_ipmifw_status_reply >> unpack_primitive_get_ipmifw_status_reply(...' failed!\n" "      last error == %d\n",call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE;
	}

	// TODO: if(err_code) return ????
	ACS_APBM_TRACE_MESSAGE("ERROR: %d>", err_code );

	return acs_apbm::ERR_NO_ERRORS; 
}

int __CLASS_NAME__::subscribe_trap (int bitmap, acs_apbm::trap_handle_t & trap_handle/*trap_handle*/) {
	ACS_APBM_TRACE_FUNCTION;

	int call_result = 0;
	int handle_count = 1;
	acs_apbm::trap_handle_t
		handles[handle_count];

	//if(bitmap & 0xFE000001U) /*SLOT 1 < bitmap < SLOT 24*/
	ACS_APBM_TRACE_MESSAGE("subscription with bitmap: %02X", bitmap);
	if(bitmap & 0xE8000000U) /* SLOT  0 > bitmap > SLOT 25 */
		return acs_apbm::ERR_BAD_SLOT;
	//check session state
	call_result = connect_unix();
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE(	"ERROR: 'subscribe_trap >> connect_unix(...' failed!\n"	"      last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_UNIX_CONNECTION_FAILED;		// an error occurred while getting handles
	}
	//session handling
	call_result = _session.get_handles(handles, handle_count);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE(	"ERROR: '_session.get_handles(...' failed!\n"	"      last error == %d\n"	"      last error text == '%s'\n", _session.last_error(), _session.last_error_text());
		return acs_apbm::ERR_GET_HANDLE_FAILED;		// an error occurred while getting handles
	}
	else
		ACS_APBM_TRACE_MESSAGE("connection handler := %d - result: %d \n", handles[0], call_result);

	trap_handle = handles[0];

	//send subscribe primitive
	call_result = send_primitive_subscribe_request(acs_apbm::PCP_SUBSCRIBE_REQUEST_ID,1,bitmap);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE(	"ERROR: 'subscribe_trap >> send_primitive_subscribe_request(...' failed!\n"	"      last error == %d\n", call_result);
		return acs_apbm::ERR_SEND_PRIMITIVE_SUBSCRIBE;		// an error occurred while getting handles
	}
	ACS_APBM_TRACE_MESSAGE("subscribe_trap(... SUCCESSFULLY SUBSCRIBED TO RECEIVE TRAPS MESSAGES!\n");
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::unsubscribe_trap (acs_apbm::trap_handle_t trap_handle) {
	ACS_APBM_TRACE_FUNCTION;

	int handle_count = 1;
	acs_apbm::trap_handle_t handles[handle_count];

	if (_session.state() ^ acs_dsd::SESSION_STATE_DISCONNECTED)
	{
		int call_result = 0;
		call_result = _session.get_handles(handles, handle_count);
		if (call_result < 0)
		{
			ACS_APBM_TRACE_MESSAGE(	"ERROR: '_session.get_handles(...' failed!\n" "      last error == %d\n" "      last error text == '%s'\n", _session.last_error(), _session.last_error_text());
			return acs_apbm::ERR_GET_HANDLE_FAILED;		// an error occurred while getting handles
		}
		else
			ACS_APBM_TRACE_MESSAGE("connection trap handler - handle: %d == trap_handle %d - result: %d \n", handles[0], trap_handle, call_result);

		call_result = send_primitive_unsubscribe_request( acs_apbm::PCP_UNSUBSCRIBE_REQUEST_ID,1);
		if (call_result < 0)
		{
			ACS_APBM_TRACE_MESSAGE(	"ERROR: 'unsubscribe_trap >> send_primitive_unsubscribe_request(...' failed!\n"	"      last error == %d\n", call_result);
			return acs_apbm::ERR_SEND_PRIMITIVE_UNSUBSCRIBE;		// an error occurred while getting handles
		}
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("internal session object error, invalid session state or disconnected\n");
		return acs_apbm::ERR_DSD_SESSION_STATE_DISCONNECTED;
	}
	ACS_APBM_TRACE_MESSAGE("unsubscribe_trap(... SUCCESSFULLY DELETED SUBSCRIPTION! \n");
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_trap (acs_apbm::trap_handle_t trap_handle, acs_apbm_trapmessage & trap_message) {
	ACS_APBM_TRACE_FUNCTION;

	int handle_count = 1;
	acs_apbm::trap_handle_t handles[handle_count];

	//check session state
	if (_session.state() ^ acs_dsd::SESSION_STATE_DISCONNECTED)
	{
		int call_result = 0;
		call_result = _session.get_handles(handles, handle_count);
		if (call_result < 0)
		{
			ACS_APBM_TRACE_MESSAGE(	"ERROR: '_session.get_handles(...' failed!\n"	"      last error == %d\n"	"      last error text == '%s'\n", _session.last_error(), _session.last_error_text());
			return acs_apbm::ERR_GET_HANDLE_FAILED;		// an error occurred while getting handles
		}
		else
			ACS_APBM_TRACE_MESSAGE("connection trap handler = (%d = %d) - result: %d \n", handles[0], trap_handle, call_result);

		if(receive_primitive_notification() == 0)
		{
			ACS_APBM_TRACE_MESSAGE("receive_primitive_notification()...OK!\n");
			//send primitive acs_apbm::PCP_GET_TRAP_REQUEST_ID
			call_result = send_primitive_get_trap_request( acs_apbm::PCP_GET_TRAP_REQUEST_ID,1);
			if (call_result < 0)
			{
				ACS_APBM_TRACE_MESSAGE(	"ERROR: 'get_trap >> send_primitive_get_trap_request(...' failed!\n" "      last error == %d\n", call_result);
				return acs_apbm::ERR_SEND_PRIMITIVE_GET_TRAP;		// an error occurred while getting handles
			}
			//receive primitive acs_apbm::PCP_GET_TRAP_REPLY_ID
			call_result = receive_primitive_get_trap(trap_message);
			if (call_result < 0)
			{
				ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_trap >> send_primitive_get_trap_request(...' failed!\n" "      last error == %d\n", call_result);
				return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_TRAP;		// an error occurred while getting handles
			}
		}
		else
		{
			ACS_APBM_TRACE_MESSAGE("WARNING - no notification trap received. get_trap(...  ignored!\n");
			return acs_apbm::ERR_NO_TRAP_RECEIVED;
		}
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("Internal session object error, invalid session state or disconnected\n");
		return acs_apbm::ERR_RECEIVER_NOT_AVAILABLE;
	}
	ACS_APBM_TRACE_MESSAGE("get_trap(... SUCCESSFULLY GET TRAP!\n");
	//ACS_APBM_TRACE_MESSAGE("trap_message: OID: %d  -  magazine value(1): %d - slot value(2): %d  - message: %s", trap_message.OID(), trap_message.values().at(0) /*magazine*/, trap_message.values().at(1) /*slot*/, trap_message.message());
	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_own_slot () {
	ACS_APBM_TRACE_FUNCTION;

	int32_t slot = -1;
	int call_result = connect_unix();
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_own_slot >> connect_unix(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_UNIX_CONNECTION_FAILED;               // an error occurred while getting handles
	}
	//send request
	call_result = send_primitive_get_own_slot_request(acs_apbm::PCP_GET_OWN_SLOT_REQUEST_ID,1);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_own_slot >> send_primitive_get_own_slot_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;         // an error occurred while getting handles
	//receive REPLY
	call_result = receive_primitive_get_own_slot(slot);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_own_slot >> receive_primitive_get_own_slot(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;

	ACS_APBM_TRACE_MESSAGE("get_own_slot(... SUCCESSFULLY RETRIEVED OWN SLOT POSISTION, found slot %d \n", slot);
	return slot;
}

int __CLASS_NAME__::get_switch_board_info(const acs_apbm::architecture_type_t architecture, uint32_t magazine, int32_t & slot, char (&ipna_str) [16], char (&ipnb_str) [16], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn ){
	ACS_APBM_TRACE_FUNCTION;

	int call_result = 0;
	unsigned conn_timeout=4000;
	//check session state
	call_result = connect(conn_timeout);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_switch_board_info >> connect(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_CONNECTION_FAILED;		// an error occurred while getting handles
	}

	//send subscribe primitive
	if(!get_switch_board_primitive_sent)
	{
		call_result = send_primitive_switch_board_data_request(acs_apbm::PCP_SWITCH_BOARD_DATA_REQUEST_ID, 1, architecture, magazine);
		if (call_result < 0)
		{
			ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_switch_board_info >> send_primitive_switch_board_data_request(...' failed!\n" "      last error == %d\n", call_result);
			return acs_apbm::ERR_SEND_PRIMITIVE_SWITCH_BOARD_DATA;		// an error occurred while getting handles
		}
		get_switch_board_primitive_sent = 1;
	}
	call_result = receive_primitive_switch_board_data(magazine, slot, ipna_str, ipnb_str, shelf_mgr_state, neighbour_state, fbn);
	if (call_result < 0)
	{
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_switch_board_info >> receive_primitive_switch_board_data(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_SWITCH_BOARD_DATA;		// an error occurred while getting handles
	}
	ACS_APBM_TRACE_MESSAGE("get_switch_board_info(... SUCCESSFULLY RETRIEVED SWITCH BOARD INFORMATION\n");
	return call_result;  // call_result >= 0 - needed to manage primitive error_code value
}




///  methods from HWMAPI

int __CLASS_NAME__:: get_board_presence (const acs_apbm::board_name_t board_name){

	ACS_APBM_TRACE_FUNCTION;

	int call_result = connect_unix();
	if (call_result < 0){
		// an error occurred while getting handles
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'check_board_presence >> connect_unix(...' failed!\n last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_UNIX_CONNECTION_FAILED;
	}
	//send request
	call_result = send_primitive_board_presence_request(acs_apbm::PCP_BOARD_PRESENCE_REQUEST_ID,1,board_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'check_board_presence >> send_primitive_board_presence_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	//TODO return acs_apbm::ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT;
	//receive REPLY
	int8_t board_presence = -1;
	call_result = receive_primitive_board_presence(board_presence);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'check_board_presence >> receive_primitive_board_presence(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}


	ACS_APBM_TRACE_MESSAGE("check_board_presence()... SUCCESSFULLY EXECUTED!, board_presence == %d \n", board_presence);

	return  board_presence;
}

int __CLASS_NAME__:: get_board_location (const acs_apbm::board_name_t board_name, std::list<int> & ap_ids){
	ACS_APBM_TRACE_FUNCTION;

	int call_result = connect_unix();
	if (call_result < 0){
		// an error occurred while getting handles
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_location >> connect_unix(...' failed!\n last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_UNIX_CONNECTION_FAILED;
	}

	//send request
	call_result = send_primitive_board_location_request(acs_apbm::PCP_BOARD_LOCATION_REQUEST_ID,1,board_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_location >> send_primitive_board_location_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}
	//receive REPLY
	ap_ids.clear();
	do {
		int  ap_sys_no = -1;
		call_result = receive_primitive_board_location(ap_sys_no);
		if (call_result < 0){
			ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_location >> receive_primitive_board_status(...' failed!\n" "      last error == %d\n", call_result);
			return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
		}
		ap_ids.push_back(ap_sys_no);
	}while (call_result != acs_apbm::PCP_ERROR_CODE_END_OF_DATA);

	ACS_APBM_TRACE_MESSAGE("get_board_location()... SUCCESSFULLY EXECUTED! \n");

	return 0;
}

int __CLASS_NAME__::get_board_status (const acs_apbm::board_name_t board_name){

	ACS_APBM_TRACE_FUNCTION;

	int call_result = connect_unix();
	if (call_result < 0){
		// an error occurred while getting handles
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_status >> connect_unix(...' failed!\n last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_UNIX_CONNECTION_FAILED;
	}
	//send request
	call_result = send_primitive_board_status_request(acs_apbm::PCP_BOARD_STATUS_REQUEST_ID,1,board_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_status >> send_primitive_board_status_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}

	//receive REPLY
	int8_t board_status = -1;
	call_result = receive_primitive_board_status(board_status);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_status >> receive_primitive_board_status(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}


	ACS_APBM_TRACE_MESSAGE("get_board_status()... SUCCESSFULLY EXECUTED!, board_status == %d \n", board_status);

	return  board_status;
}

int __CLASS_NAME__::set_board_status (const acs_apbm::board_name_t  board_name, const acs_apbm::reported_board_status_t board_status){
	ACS_APBM_TRACE_FUNCTION;

	int call_result = connect_unix();
	if (call_result < 0){
		// an error occurred while getting handles
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_status >> connect_unix(...' failed!\n last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_UNIX_CONNECTION_FAILED;
	}

	//	reported_board_status_t rep_board_status = (board_status == -1) ? REPORTED_BOARD_STATUS_FAULTY: REPORTED_BOARD_STATUS_WORKING;
	//send request
	call_result = send_primitive_set_board_status_request(acs_apbm::PCP_SET_BOARD_STATUS_REQUEST_ID,1,board_name, board_status);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_status >> send_primitive_board_status_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}

	//receive REPLY
	int32_t err_code = -1;
	call_result = receive_primitive_set_board_status_reply(err_code);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_board_status >> receive_primitive_board_status(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE_NOTIFY;
	}

	ACS_APBM_TRACE_MESSAGE("set_board_status()... SUCCESSFULLY EXECUTED!, err_code == %d \n", err_code);

	return  err_code;
}

int  __CLASS_NAME__::ipmifw_upgrade(uint32_t magazine, int32_t slot, const char ipmipkg_name [ACS_APBM_IPMIPKG_NAME_MAX_SIZE], uint8_t comport){

	ACS_APBM_TRACE_FUNCTION;
	int call_result = connect_unix();
	if (call_result < 0){
		// an error occurred while getting handles
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'ipmifw_upgrade >> connect_unix(...' failed!\n last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_CONNECTION_FAILED;
	}

	//send request
	call_result = send_primitive_ipmifw_upgrade_request(acs_apbm::PCP_IPMIUPG_UPGRADE_REQUEST_ID, 1 , magazine, slot, comport, ipmipkg_name);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'ipmifw_upgrade >> send_primitive__ipmifw_upgrade_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_SEND_PRIMITIVE;
	}

	//receive REPLY // TO avoid timeout-expiration

	int32_t err_code = -1;
	call_result = receive_primitive_ipmifw_upgrade_reply(err_code, comport);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'ipmifw_upgrade >> receive_primitive_ipmifw_upgrade_reply(...' failed!\n" "   last error == %d\n", call_result);
		return acs_apbm::ERR_GENERIC_ERROR;

	}
	ACS_APBM_TRACE_MESSAGE("ipmifw_upgrade()... SUCCESSFULLY EXECUTED!, err_code == %d \n", err_code);

	return err_code;
}


int __CLASS_NAME__:: get_ipmifw_status(uint32_t magazine, int32_t  slot, uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE] ){

	ACS_APBM_TRACE_FUNCTION;
	int call_result = connect_unix();
	if (call_result < 0){
		// an error occurred while getting handles
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_ipmifw_status >> connect_unix(...' failed!\n last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_CONNECTION_FAILED;
	}


	//send request
	call_result = send_primitive_get_ipmifw_status_request(acs_apbm::PCP_GET_IPMIUPG_STATUS_REQUEST_ID, 1 , magazine, slot);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_ipmifw_status >> send_primitive_get_ipmifw_status_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_SEND_PRIMITIVE;
	}

	//receive REPLY
	int32_t err_code = -1;
	call_result = receive_primitive_get_ipmifw_status_reply(ipmifw_status, lastupg_date);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_ipmifw_status >> receive_primitive_get_ipmifw_status_reply(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE;
	}

	ACS_APBM_TRACE_MESSAGE("get_ipmifw_status()... SUCCESSFULLY EXECUTED!, err_code == %d \n", err_code);

	return  call_result;
}

int __CLASS_NAME__::get_ipmifw_data(uint32_t magazine, int32_t  slot, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER] ,  char (& revision) [ACS_APBM_IPMI_REVISION]){

	ACS_APBM_TRACE_FUNCTION;
	int call_result = connect_unix();
	if (call_result < 0){
		// an error occurred while getting handles
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_ipmifw_data >> connect_unix(...' failed!\n last error == %d\n", call_result);
		return acs_apbm::ERR_INIT_CONNECTION_FAILED;
	}

	//send request
	call_result = send_primitive_get_ipmifw_data_request(acs_apbm::PCP_GET_IPMIFW_DATA_REQUEST_ID, 1 , magazine, slot);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_ipmifw_data >> send_primitive_get_ipmifw_data_request(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_SEND_PRIMITIVE;
	}

	//receive REPLY
	int32_t err_code = -1;
	call_result = receive_primitive_get_ipmifw_data_reply(err_code, ipmifw_type, product_number, revision);
	if (call_result < 0){
		ACS_APBM_TRACE_MESSAGE( "ERROR: 'get_ipmifw_data >> receive_primitive_get_ipmifw_data_reply(...' failed!\n" "      last error == %d\n", call_result);
		return acs_apbm::ERR_RECEIVE_PRIMITIVE;
	}

	ACS_APBM_TRACE_MESSAGE("get_ipmifw_data()... SUCCESSFULLY EXECUTED!, err_code == %d \n", err_code);

	return  err_code;

}

bool __CLASS_NAME__::is_gep5_gep7()
{
	ACS_APGCC_CommonLib commonlib_obj;
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;

	commonlib_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );
	if(hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
		switch(hwInfo.hwVersion){
			case ACS_APGCC_HWVER_GEP1:
			case ACS_APGCC_HWVER_GEP2:
				gep5_gep7=false;
				break;
			case ACS_APGCC_HWVER_GEP5:
			case ACS_APGCC_HWVER_GEP7:
				gep5_gep7=true;
				break;
			case ACS_APGCC_HWVER_VM:
			default:
				ACS_APBM_TRACE_MESSAGE("ERROR: GetHwInfo() - Undefined Hardware Version");
				return gep5_gep7;
			}
	}	
	else
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: GetHwInfo() - Undefined Hardware Version");
		return gep5_gep7;
	}
	return gep5_gep7;
}
bool __CLASS_NAME__::is_hwtype_gep7()
{
	ACS_APGCC_CommonLib commonlib_obj;
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;

	commonlib_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );
	if(hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
		switch(hwInfo.hwVersion){
			case ACS_APGCC_HWVER_GEP1:
			case ACS_APGCC_HWVER_GEP2:
			case ACS_APGCC_HWVER_GEP5:
				hwtype_gep7=false;
				break;
			case ACS_APGCC_HWVER_GEP7:
				hwtype_gep7=true;
				break;
			case ACS_APGCC_HWVER_VM:
			default:
				ACS_APBM_TRACE_MESSAGE("ERROR: GetHwInfo() - Undefined Hardware Version");
				return hwtype_gep7;
			}
	}
	else
	{
		ACS_APBM_TRACE_MESSAGE("ERROR: GetHwInfo() - Undefined Hardware Version");
		return hwtype_gep7;
	}
	return hwtype_gep7;
}

