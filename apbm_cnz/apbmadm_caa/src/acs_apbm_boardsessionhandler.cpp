#include "acs_apbm_boardsessionhandler.h"
#include "acs_apbm_primitivedatahandler.h"
#include "acs_apbm_programmacros.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_trapmessage.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_action_timer.h"
#include "acs_apbm_shelvesdatamanager.h"
#include <acs_apbm_macrosconstants.h>
uint32_t acs_apbm_boardsessionhandler::_next_available_ID = 1;
ACE_Thread_Mutex acs_apbm_boardsessionhandler::_next_available_ID_mutex;

static const char * GET_SCB_DATA_REPLY_PRIMITIVE_TEMPL = "primitive_id == %d, primitive_version == %d, error_code == %d, slot == %d, ip1 == %s, ip2 == %s, master_status == %d, neighbour_status == %d";
static const char * GET_EGEM2_SWITCH_DATA_REPLY_PRIMITIVE_TEMPL = "primitive_id == %d, primitive_version == %d, error_code == %d, magazine == %d, slot == %d, ip1 == %s, ip2 == %s, master_status == %d, neighbour_status == %d, fbn == %d";


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler(ACE_Reactor * reactor, acs_apbm_serverworkingset * server_working_set)
: _server_working_set(server_working_set)
{
	_reactor = reactor;
	_object_ID =  acs_apbm_boardsessionhandler::sync_get_ID();
	_subscribed = false;
	_subscription_bitmap = 0;
	_pending_trap_notification = 0;
}


ACE_HANDLE acs_apbm_boardsessionhandler::get_handle (void) const
{
	int handle_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1;
	acs_dsd::HANDLE handles [acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1];
	if(_session.get_handles(handles, handle_count) < 0)
		return -1;	// an error occurred while getting handles

	return handles[0];
}


int acs_apbm_boardsessionhandler::open()
{
	if(!_reactor)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error opening BOARD SESSION HANDLER object. Description: (this->_reactor == 0) ! (this == %p)", this);
		return ACS_APBM_BSH_INVALID_REACTOR;  // object is in a bad state
	}

	// register object as event handler for "data ready" events on the I/O handle that is associated to the DSD API Session object
	if(_reactor->register_handler(this, ACE_Event_Handler::READ_MASK) < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "acs_apbm_boardsessionhandler:  error registering with reactor for READ events ! (this == %p) ", this);
		return ACS_APBM_BSH_REGISTER_HANDLE_ERROR;
	}

	return ACS_APBM_BSH_SUCCESS;
}


int acs_apbm_boardsessionhandler::close()
{
	if(_reactor)
	{
		int op_res = _reactor->remove_handler(this, ACE_Event_Handler::READ_MASK);
		if(!op_res)
			ACS_APBM_LOG(LOG_LEVEL_INFO, "BOARD SESSION HANDLER object unregistered from reactor. (this == %p)", this);
		else
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "an error occurred trying to unregister BOARD SESSION HANDLER object from reactor. Error code is < %d > ! (this == %p)", op_res, this);
			return  ACS_APBM_BSH_UNREGISTER_HANDLE_ERROR;
		}

	}

	return ACS_APBM_BSH_SUCCESS;
}


int acs_apbm_boardsessionhandler::handle_input (ACE_HANDLE /*fd*/)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %u >. Data ready to be read. ", this->getID());

	// allocate an object to receive the primitive sent by the peer
	acs_apbm_primitive received_primitive(_session);

	// receive the primitive from the peer
	if(recvPrimitive(received_primitive) < 0)
		return -1;	// an error occurred ! Connection has to be closed.

	// process the primitive
	acs_apbm_boardsessionhandler_constants hdl_prim_res = handlePrimitive(received_primitive);
	if((hdl_prim_res < 0)  || (hdl_prim_res == ACS_APBM_BSH_SUCCESS_CLOSE_CONN))
		return -1;	// an error occurred or the connection must be closed.

	return 0;
}


int  acs_apbm_boardsessionhandler::handle_close(ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %u >. Closing connection.", this->getID());
	if(_subscribed)
	{
		int call_result = _server_working_set->trap_subscription_manager->removeSubscriber(this);

		if(call_result < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %u >. Call 'acs_apbm_trapsubscriptionmanager::removeSubscriber(..)' failed ! call_result == %d", this->getID(), call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %u >. Successfully unregistered from SUBSCRIPTION MANAGER. ", this->getID(), call_result);
	}

	delete this;	// commit suicide, because this session handler has terminated it's work
	return 0;
}

/*
int acs_apbm_boardsessionhandler::handle_timeout ( const ACE_Time_Value &  current_time,  const void *  act)
{
	static int cnt = 0;

	if(!_subscribed)
		return 0;

	trapdata trapMsg;

	trapMsg.trapType = acs_apbm_trapmessage::SEL_ENTRY;
	trapMsg.values.push_back(0);
	trapMsg.values.push_back(cnt % 26);
	trapMsg.message_len = 11;
	strncpy(trapMsg.message,"SELTRAPRECV",11);

	//notifyTrap(trapMsg);
	subscr_mngr.notifyTrapInfo( cnt++ % 26,trapMsg);
	return 0;
}
*/

acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::recvPrimitive(acs_apbm_primitive & pdh, uint32_t timeout_ms)
{
	ssize_t bytes_received;
	if(timeout_ms == 0)
		bytes_received = pdh.recv_primitive();
	else
	{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		bytes_received = pdh.recv_primitive(& timeout);
	}

	if(bytes_received <= 0)
	{
		// check if the connection has been closed by the peer
		if((bytes_received == 0) || (bytes_received == acs_dsd::ERR_PEER_CLOSED_CONNECTION))
		{
			ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Peer closed connection.", this->getID());
			return ACS_APBM_BSH_RECEIVE_PEER_CLOSED;
		}

		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error receiving data. bytes_received = %d, errno == %d !", this->getID(), bytes_received, errno);
		return ACS_APBM_BSH_RECEIVE_GENERIC_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CONN_ID = < %d >. (%d) bytes received.", this->getID(), bytes_received);
	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::sendPrimitive(const acs_apbm_primitive & pdh, uint32_t timeout_ms)
{
	ssize_t bytes_transferred;

	if(timeout_ms == 0)
		bytes_transferred = pdh.send_primitive();
	else
	{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		bytes_transferred = pdh.send_primitive(& timeout);
	}

	if(bytes_transferred <= 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error sending data. bytes_transferred = %d !", this->getID(), bytes_transferred);
		return ACS_APBM_BSH_SEND_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CONN_ID = < %d >. (%d) bytes sent.", this->getID(), bytes_transferred);
	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::sendNotifyPrimitive(uint32_t n_pending_traps, uint32_t timeout_ms)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Sending NOTIFY_TRAP primitive. N_TRAPS == %d", this->getID(), n_pending_traps);

	acs_apbm_primitive pdh(_session);
	int call_result = pdh.make_primitive(acs_apbm::PCP_NOTIFICATION_TRAP_MSG_ID, 1, n_pending_traps);
	if(call_result)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Send NOTIFY_TRAP failed ! Details: Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed.  call_result  == %d !", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	return sendPrimitive(pdh,timeout_ms);
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::sendSelTrapReplyPrimitive(const trapdata & trap, uint32_t timeout_ms)
{
	// ensure we received the correct TRAP type
	if(trap.trapType != acs_apbm_trapmessage::SEL_ENTRY)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Invalid TRAP parameter (trap_type == %d) ! We expected a TRAP of type 'SEL' !", this->getID(), trap.trapType);
		return ACS_APBM_BSH_INVALID_TRAP_MESSAGE;
	}

	// read TRAP data
	int magPlugNumber = trap.values.at(0);					// get MAGAZINE PLUG NUMBER
	int slotPos = trap.values.at(1);						// get SLOT POSITION
	size_t nBytesToCopy = trap.message_len; 				// get MESSAGE LENGTH

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Preparing to send to the peer a TRAP of type 'SEL'. DETAILS: magPlugNumber == %d, slotPos == %d, message_len == %u ", this->getID(), magPlugNumber, slotPos, nBytesToCopy);

	// get TRAP MESSAGE, performing truncation if necessary
	if(nBytesToCopy > acs_apbm::PCP_FIELD_SIZE_TRAP_MSG)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "CONN_ID = < %d >. TRAP 'message' field will be truncated to %d bytes !", this->getID(), acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);
		nBytesToCopy = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
	}
	char message[acs_apbm::PCP_FIELD_SIZE_TRAP_MSG] = {0};
	memcpy(message, trap.message, nBytesToCopy);

	// prepare and send the SEL_TRAP_REPLY primitive to the peer
	acs_apbm_primitive pdh(_session);
	int op_res = pdh.make_primitive(acs_apbm::PCP_GET_SEL_TRAP_REPLY_ID, 1, magPlugNumber, slotPos, message);
	if(op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! op_res == %d", this->getID(), op_res);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// and now send the primitive
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CONN_ID = < %d >. Trying to send SELTRAP_REPLY primitive: primitive_id == %d, primitive_version == %u, magPlugNumber == %d, slopPos == %d, message_len = %u", this->getID(), acs_apbm::PCP_GET_SEL_TRAP_REPLY_ID, 1, magPlugNumber, slotPos, nBytesToCopy);
	op_res = sendPrimitive(pdh, timeout_ms);
	if(op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error sending primitive SELTRAP_REPLY ! op_res == %d", this->getID(), op_res);
		return ACS_APBM_BSH_SEND_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. SELTRAP_REPLY primitive successfully sent.", this->getID());
	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::sendBoardPresenceTrapReplyPrimitive(const trapdata & trap, uint32_t timeout_ms)
{
	// ensure we received the correct TRAP type
	if(trap.trapType != acs_apbm_trapmessage::BOARD_PRESENCE)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Invalid TRAP parameter (trap_type == %d) ! We expected a TRAP of type 'BOARD_PRESENCE' !", this->getID(), trap.trapType);
		return ACS_APBM_BSH_INVALID_TRAP_MESSAGE;
	}

	// read TRAP data
	int magPlugNumber = trap.values.at(0);					// get MAGAZINE PLUG NUMBER
	int slotPos = trap.values.at(1);						// get SLOT POSITION
	int hwBoardPresence = trap.values.at(2);				// get HARDWARE BOARD PRESENCE
	int busType = trap.values.at(3);						// get BUS TYPE
	size_t nBytesToCopy = trap.message_len; 				// get MESSAGE LENGTH

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Preparing to send to the peer a TRAP of type 'BOARD_PRESENCE'. DETAILS: magPlugNumber == %d, slotPos == %d, hwBoardPresence == %d, busType == %d, message_len == %u ", this->getID(), magPlugNumber, slotPos, hwBoardPresence, busType, nBytesToCopy);

	// get TRAP MESSAGE, performing truncation if necessary
	if(nBytesToCopy > acs_apbm::PCP_FIELD_SIZE_TRAP_MSG)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "CONN_ID = < %d >. TRAP 'message' field will be truncated to %d bytes !", this->getID(), acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);
		nBytesToCopy = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
	}

	if(!trap.message || !*trap.message)
	ACS_APBM_LOG(LOG_LEVEL_WARN," *** trap.message is empty! ***");
	char message[acs_apbm::PCP_FIELD_SIZE_TRAP_MSG] = {0};
	memcpy(message, trap.message, nBytesToCopy);

	// prepare and send the BOARD_PRESENCE_TRAP_RAPLY primitive to the peer
	acs_apbm_primitive pdh(_session);
	int op_res = pdh.make_primitive(acs_apbm::PCP_GET_BOARD_PRESENCE_TRAP_REPLY_ID, 1, magPlugNumber, slotPos, hwBoardPresence, busType, message);
	if(op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! op_res == %d", this->getID(), op_res);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// and now send the primitive
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CONN_ID = < %d >. Trying to send BOARD_PRESENCE_TRAP_RAPLY primitive: primitive_id == %d, primitive_version == %u, magPlugNumber == %d, slopPos == %d, hwBoardPresence == %d, busType == %d, message_len = %u", this->getID(), acs_apbm::PCP_GET_BOARD_PRESENCE_TRAP_REPLY_ID, 1, magPlugNumber, slotPos, hwBoardPresence, busType, nBytesToCopy);
	op_res = sendPrimitive(pdh, timeout_ms);
	if(op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error sending primitive BOARD_PRESENCE_TRAP_RAPLY ! op_res == %d", this->getID(), op_res);
		return ACS_APBM_BSH_SEND_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. BOARD_PRESENCE_TRAP_RAPLY primitive successfully sent.", this->getID());
	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::sendSensorStateChangeTrapReplyPrimitive(const trapdata & trap, uint32_t timeout_ms){
	// ensure we received the correct TRAP type
	if(trap.trapType != acs_apbm_trapmessage::SENSOR_STATE_CHANGE)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Invalid TRAP parameter (trap_type == %d) ! We expected a TRAP of type 'SENSOR_STATE_CHANGE' !", this->getID(), trap.trapType);
		return ACS_APBM_BSH_INVALID_TRAP_MESSAGE;
	}

	// read TRAP data
	int magPlugNumber = trap.values.at(0);					// get MAGAZINE PLUG NUMBER
	int slotPos = trap.values.at(1);						// get SLOT POSITION
	int sensorType = trap.values.at(2);						// get SENSOR TYPE
	int sensorID = trap.values.at(3);						// get SENSOR ID
	int sensorTypeCode = trap.values.at(4);					// get SENSOR TYPE CODE
	size_t nBytesToCopy = trap.message_len; 				// get MESSAGE LENGTH - SENSOR EVENT DATA

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Preparing to send to the peer a TRAP of type 'SENSOR_STATE_CHANGE'. DETAILS: magPlugNumber == %d, slotPos == %d, sensorType == %d, sensorID == %d, sensorTypeCode == %d, message_len == %u ", this->getID(), magPlugNumber, slotPos, sensorType, sensorID, sensorTypeCode, nBytesToCopy);

	// get TRAP MESSAGE, performing truncation if necessary
	if(nBytesToCopy > acs_apbm::PCP_FIELD_SIZE_TRAP_MSG)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "CONN_ID = < %d >. TRAP 'message' field will be truncated to %d bytes !", this->getID(), acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);
		nBytesToCopy = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
	}
	char SensorEventData[acs_apbm::PCP_FIELD_SIZE_TRAP_MSG] = {0}; //SensorEventData
	memcpy(SensorEventData, trap.message, nBytesToCopy);

	// prepare and send the GET_SENSOR_STATE_CHANGE_TRAP_REPLY primitive to the peer
	acs_apbm_primitive pdh(_session);
	int op_res = pdh.make_primitive(acs_apbm::PCP_GET_SENSOR_STATE_CHANGE_TRAP_REPLY_ID, 1, magPlugNumber, slotPos, sensorType, sensorID, sensorTypeCode, SensorEventData);
	if(op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! op_res == %d", this->getID(), op_res);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// and now send the primitive
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CONN_ID = < %d >. Trying to send SENSOR_STATE_CHANGE_TRAP_REPLY primitive: primitive_id == %d, primitive_version == %u, magPlugNumber == %d, slopPos == %d, sensorType == %d, sensorID == %d, sensorTypeCode == %d, message_len = %u", this->getID(), acs_apbm::PCP_GET_SENSOR_STATE_CHANGE_TRAP_REPLY_ID, 1, magPlugNumber, slotPos, sensorType, sensorID, sensorTypeCode, nBytesToCopy);
	op_res = sendPrimitive(pdh, timeout_ms);
	if(op_res < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error sending primitive SENSOR_STATE_CHANGE_TRAP_REPLY ! op_res == %d", this->getID(), op_res);
		return ACS_APBM_BSH_SEND_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. SENSOR_STATE_CHANGE_TRAP_REPLY primitive successfully sent.", this->getID());
	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handlePrimitive(const acs_apbm_primitive & primitive)
{
	int op_code = primitive.primitive_id();
	switch(op_code)
	{
		case acs_apbm::PCP_SUBSCRIBE_REQUEST_ID:
			return handle_subscribetrap_request(primitive);
		case acs_apbm::PCP_UNSUBSCRIBE_REQUEST_ID:
			return handle_unsubscribetrap_request(primitive);
		case acs_apbm::PCP_GET_TRAP_REQUEST_ID:
			return handle_gettrap_request(primitive);
		case acs_apbm::PCP_SWITCH_BOARD_DATA_REQUEST_ID:
			return handle_switchboarddata_request(primitive);
		case acs_apbm::PCP_GET_BIOS_VERSION_REQUEST_ID:
			return handle_getbiosversion_request(primitive);
		case acs_apbm::PCP_GET_DISK_STATUS_REQUEST_ID:
			return handle_getdiskstatus_request(primitive);
		case acs_apbm::PCP_GET_OWN_SLOT_REQUEST_ID:
			return handle_getownslot_request(primitive);
		case acs_apbm::PCP_BOARD_PRESENCE_REQUEST_ID:
			return handle_board_presence_request(primitive);
		case acs_apbm::PCP_BOARD_STATUS_REQUEST_ID:
			return handle_board_status_request(primitive);
		case acs_apbm::PCP_BOARD_LOCATION_REQUEST_ID:
			return handle_board_location_request(primitive);
		case acs_apbm::PCP_SET_BOARD_STATUS_REQUEST_ID:
			return handle_set_board_status_request(primitive);
		case acs_apbm::PCP_GET_IPMIUPG_STATUS_REQUEST_ID:
			return handle_get_ipmifw_status_request(primitive);
		case acs_apbm::PCP_IPMIUPG_UPGRADE_REQUEST_ID:
			return handle_get_ipmifw_upgrade_request(primitive);
		case acs_apbm::PCP_GET_IPMIFW_DATA_REQUEST_ID:
			return handle_get_ipmifw_data_request(primitive);
		case acs_apbm::PCP_GET_THUMBDRIVE_STATUS_REQUEST_ID: //Redesign as per TR-HS30773
                        return handle_getthumbdrivestatus_request(primitive);
		case acs_apbm::PCP_SET_NIC_INFO_REQUEST_ID:
		        return handle_set_nic_info_request(primitive);
		default:
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Received unexpected or unknown primitive. primitive_id == %d !", this->getID(), op_code);
			return ACS_APBM_BSH_RECEIVE_UNKNOWN_PRIMITIVE;
	}

	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_subscribetrap_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned primitive_version;
	unsigned bitmap;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & bitmap);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected SUBSCRIBE_TRAP request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive SUBSCRIBETRAP_REQUEST: primitive_id == %d, primitive_version == %u, bitmap == 0x%08X", this->getID(), primitive_id, primitive_version, bitmap);

	// check if there's already a subscription on this connection. If so, close connection !
	if(_subscribed)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Already registered with SUBSCRIPTION MANAGER (with bitmap == 0x%08X) ! ", this->getID(), _subscription_bitmap);
		return ACS_APBM_BSH_SUBSCRIBE_ERROR;
	}

	// register with APBM SUBSCRIPTIONS MANAGER to receive traps
	call_result = _server_working_set->trap_subscription_manager->addSubscriber(bitmap, this);

	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error subscribing for traps ! call 'acs_apbm_trapsubscriptionmanager::addSubscriber()' failed ! call_result == %d !", this->getID(), call_result);
		return ACS_APBM_BSH_SUBSCRIBE_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Successfully registered with SUBSCRIPTION MANAGER with bitmap == 0x%08X)", this->getID(), bitmap);
	_subscribed = true;
	_subscription_bitmap = bitmap;

	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_unsubscribetrap_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned primitive_version;
	int call_result = pdh. unpack_primitive(primitive_id, primitive_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected UNSUBSCRIBETRAP_REQUEST) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive UNSUBSCRIBETRAP_REQUEST:  primitive_id == %d, primitive_version == %u", this->getID(), primitive_id, primitive_version);

	// unregister from APBM SUBSCRIPTIONS MANAGER
	call_result = _server_working_set->trap_subscription_manager->removeSubscriber(this);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error canceling subscription for traps ! Call 'acs_apbm_trapsubscriptionmanager::removeSubscriber(..)' failed ! call_result == %d !", this->getID(), call_result);
		return ACS_APBM_BSH_UNSUBSCRIBE_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Successfully unregistered from SUBSCRIPTION MANAGER.", this->getID());

	// remove pending traps
	_received_traps.clear();
	_subscribed = false;
	_subscription_bitmap = 0;

	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_gettrap_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned primitive_version;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected GETTRAP_REQUEST) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive GETTRAP_REQUEST: primitive_id == %d, primitive_version == %u", this->getID(), primitive_id, primitive_version);

	// ... there MUST be a pending NOTIFY on this session. Check it !
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_1(_pending_trap_notification_mutex);
	if(!_pending_trap_notification)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "CONN_ID = < %d >. No notification pending on the session. Ignore the request !", this->getID());
		return ACS_APBM_BSH_SUCCESS;
	}

	// since we received a GETTRAP_REQUEST from the peer, we assume that the NOTIFY_TRAP primitive has been consumed
	_pending_trap_notification = 0;
	guard_1.release();

	// get the first trap and remove it from the queue
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_2(_received_traps_mutex);
	if( _received_traps.size() == 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "CONN_ID = < %d >. No pending TRAPs for this subscriber ! We expected at least one pending TRAP !", this->getID());
		return  ACS_APBM_BSH_ERROR;
	}

	trapdata trap = _received_traps.front();
	_received_traps.pop_front();
	guard_2.release();

	// prepare and send the GETTRAP_REPLY primitive
	switch(trap.trapType)
	{
	case acs_apbm_trapmessage::SEL_ENTRY:
		{
			acs_apbm_boardsessionhandler_constants send_result = sendSelTrapReplyPrimitive(trap);
			if(send_result != ACS_APBM_BSH_SUCCESS)
				return send_result;
			break;
		}
	case acs_apbm_trapmessage::APBM_READY:
	case acs_apbm_trapmessage::SENSOR_STATE_CHANGE:
	{
		acs_apbm_boardsessionhandler_constants send_result = sendSensorStateChangeTrapReplyPrimitive(trap);
		if(send_result != ACS_APBM_BSH_SUCCESS)
			return send_result;
		break;
	}
	case acs_apbm_trapmessage::BOARD_PRESENCE:
	{
		acs_apbm_boardsessionhandler_constants send_result = sendBoardPresenceTrapReplyPrimitive(trap);
		if(send_result != ACS_APBM_BSH_SUCCESS)
			return send_result;
		break;
	}
	case acs_apbm_trapmessage::NIC:
	case acs_apbm_trapmessage::RAID:
	case acs_apbm_trapmessage::DISKCONN:
	default:
		break;
	}

	// now check if there are other TRAPs to be notified. If so, send a NOTIFY_TRAP primitive to the peer
	guard_2.acquire();
	int n_traps = _received_traps.size();
	guard_2.release();
	if(n_traps > 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Found %d pending TRAPs in the queue associated to this session.", this->getID(), n_traps);
		acs_apbm_boardsessionhandler_constants  ret_val = _notifyTrapToPeer(n_traps);
		if(ret_val != ACS_APBM_BSH_SUCCESS)
			return ret_val;
	}

	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_switchboarddata_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int8_t architecture_type;
	uint32_t magazine_num;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & architecture_type, & magazine_num);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a SWITCH_BOARD_DATA request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive SWITCH_BOARD_DATA request: primitive_id == %d, primitive_version == %u, architecture_type == %u, magazine_num == %u", this->getID(), primitive_id, primitive_version, architecture_type, magazine_num);
	// reconstruct the command sent by the client
	char client_cmd[32] = {0};
	if((call_result = _reconstruct_client_commamd_from_request(client_cmd, sizeof(client_cmd), architecture_type, magazine_num)) < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Call '_reconstruct_client_commamd_from_request()' failed  ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_INVALID_PRIMITIVE_PARAM;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Executing command < %s > sent by the client ... ", this->getID(), client_cmd);

	// get system architecture and compare it to the value received in the request
	uint8_t my_architecture_type = (_server_working_set->cs_reader->is_cba_system() ? acs_apbm::ARCHITECTURE_TYPE_CBA : acs_apbm::ARCHITECTURE_TYPE_NOT_CBA);


	if(my_architecture_type != architecture_type)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. The command sent by the client cannot be executed on this architecture type !", this->getID());
		return send_invalid_arch_switchboarddata_reply_primitive(architecture_type);
	}
	// get from SNMP Manager the info about the switch boards
	std::vector<acs_apbm_switchboardinfo> switch_boards_info_collection;
	if(architecture_type == acs_apbm::ARCHITECTURE_TYPE_NOT_CBA)
	{
		call_result = _server_working_set->snmp_manager->get_my_switch_board_info(switch_boards_info_collection);
		if(call_result < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Call 'acs_apbm_csreader::get_my_switch_board_info()' failed ! (call_result == %d)", this->getID(), call_result);
			return ACS_APBM_BSH_ERROR;
		}
	}
	else /* acs_apbm::ARCHITECTURE_TYPE_CBA */
	{
		call_result = _server_working_set->snmp_manager->get_all_switch_board_info(switch_boards_info_collection);
		if(call_result < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Call 'acs_apbm_csreader::get_switch_boards_info()' failed ! (call_result == %d)", this->getID(), call_result);
			return ACS_APBM_BSH_ERROR;
		}
	}

	// send response primitives to the client
	char str_response_prim[2048] = {0};
	int env;
	int call_res = _server_working_set->cs_reader->get_environment(env);
	for(std::vector<acs_apbm_switchboardinfo>::iterator it = switch_boards_info_collection.begin(); it != switch_boards_info_collection.end(); ++it)
	{


		acs_apbm_primitive resp_prim(_session);
		uint16_t fbn;

		if(architecture_type == acs_apbm::ARCHITECTURE_TYPE_CBA)
		{
		    if(magazine_num == it->magazine)
		    {
		    	if(env == ACS_CS_API_CommonBasedArchitecture::SCX)
		    			fbn = acs_apbm::SWITCH_BOARD_FBN_SCXB;
		    	else if(env == ACS_CS_API_CommonBasedArchitecture::SMX)
		    		fbn = acs_apbm::SWITCH_BOARD_FBN_SMXB;

		    	snprintf(str_response_prim, sizeof(str_response_prim), GET_EGEM2_SWITCH_DATA_REPLY_PRIMITIVE_TEMPL, acs_apbm::PCP_GET_EGEM2L2_SWITCH_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_SUCCESS, it->magazine, it->slot_position, it->ipna_str, it->ipnb_str, it->state, it->neighbour_state, fbn);
		        call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_EGEM2L2_SWITCH_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_SUCCESS, it->magazine, it->slot_position, it->ipna_str, it->ipnb_str, it->state, it->neighbour_state, fbn);

		    }
		    else
		    {
		      continue;
		    }
		}
		else  /* acs_apbm::ARCHITECTURE_TYPE_NOT_CBA */
		{
			snprintf(str_response_prim, sizeof(str_response_prim), GET_SCB_DATA_REPLY_PRIMITIVE_TEMPL, acs_apbm::PCP_GET_SCB_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_SUCCESS, it->slot_position, it->ipna_str, it->ipnb_str, it->state, it->neighbour_state);
			call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_SCB_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_SUCCESS, it->slot_position, it->ipna_str, it->ipnb_str, it->state, it->neighbour_state);
		}

		if(call_result < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
			return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
		}

		ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: %s  ...", this->getID(), str_response_prim);
		call_result = sendPrimitive(resp_prim);
		if(call_result < 0)
			return ACS_APBM_BSH_SEND_ERROR;

		ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());
	}

	// send a last response primitive to indicate the end of data
	char ipna_str[acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS] = {0};
	char ipnb_str[acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS] = {0};
	//char ipaliasa_str[acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS] = {0};
       // char ipaliasb_str[acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS] = {0};
	acs_apbm_primitive end_of_data_resp_prim(_session);
	if(architecture_type == acs_apbm::ARCHITECTURE_TYPE_CBA)
	{
		if(env == ACS_CS_API_CommonBasedArchitecture::SCX || env == ACS_CS_API_CommonBasedArchitecture::SMX)
			call_result = end_of_data_resp_prim.make_primitive(acs_apbm::PCP_GET_EGEM2L2_SWITCH_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_END_OF_DATA, acs_apbm::BOARD_MAGAZINE_UNDEFINED, acs_apbm::BOARD_SLOT_UNDEFINED, ipna_str, ipnb_str, acs_apbm::SWITCH_BOARD_STATE_UNINTT_STATUS, acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_UNINTT, acs_apbm::SWITCH_BOARD_FBN_UNKNOWN);
	}
	else  /* acs_apbm::ARCHITECTURE_TYPE_NOT_CBA */
		call_result = end_of_data_resp_prim.make_primitive(acs_apbm::PCP_GET_SCB_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_END_OF_DATA, acs_apbm::BOARD_SLOT_UNDEFINED, ipna_str, ipna_str, acs_apbm::SWITCH_BOARD_STATE_UNINTT_STATUS, acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_UNINTT);

	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing 'end of data' response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send 'end of data' response primitive", this->getID());
	call_result = sendPrimitive(end_of_data_resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. End of data response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS_CLOSE_CONN;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_getbiosversion_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a GET_BIOS_VERSION request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive GET_BIOS_VERSION request: primitive_id == %d, primitive_version == %u", this->getID(), primitive_id, primitive_version);

	int error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;

	// get BIOS info concerning the local AP board
	char bios_product_name[acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME] = {0};
	char bios_product_version[acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_VERSION] = {0};
	call_result = acs_apbm_shelvesdatamanager::get_local_ap_board_bios_version(bios_product_name, bios_product_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting BIOS info. Call 'acs_apbm_shelvesdatamanager::get_local_ap_board_bios_version()' failed ! call_result == %d", this->getID(), call_result);
		error_code = acs_apbm::PCP_ERROR_CODE_END_OF_DATA;
	        //TODO error_code = acs_apbm::PCP_ERROR_CODE_GENERIC_FAILURE;
	}

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_BIOS_VERSION_REPLY_ID, 1, error_code, bios_product_name, bios_product_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, bios_product_name == %s, bios_product_version == %s", this->getID(), acs_apbm::PCP_GET_BIOS_VERSION_REPLY_ID, 1, error_code, bios_product_name, bios_product_version);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_getdiskstatus_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a GET_DISK_STATUS request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive GET_DISK_STATUS request: primitive_id == %d, primitive_version == %u", this->getID(), primitive_id, primitive_version);

	int error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;

	// get DISKS status info concerning the local AP board
	unsigned disks_status;
	call_result = acs_apbm_shelvesdatamanager::get_local_ap_board_disks_status(disks_status);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting DISKS status info. Call 'acs_apbm_shelvesdatamanager::get_local_ap_board_disks_status()' failed ! call_result == %d", this->getID(), call_result);
                error_code = acs_apbm::PCP_ERROR_CODE_END_OF_DATA;
                //TODO error_code = acs_apbm::PCP_ERROR_CODE_GENERIC_FAILURE;
	}

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_DISK_STATUS_REPLY_ID, 1, error_code, disks_status);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, disks_status == %u", this->getID(), acs_apbm::PCP_GET_DISK_STATUS_REPLY_ID, 1, error_code, disks_status);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}

//Redesign as per TR-HS30773
acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_getthumbdrivestatus_request(const acs_apbm_primitive & pdh)
{
        // unpack the received primitive
        int primitive_id;
        unsigned int primitive_version;
        int call_result = pdh.unpack_primitive(primitive_id, primitive_version);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a GET_THUMBDRIVE_STATUS request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
                return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
        }
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive GET_THUMBDRIVE_STATUS request: primitive_id == %d, primitive_version == %u", this->getID(), primitive_id, primitive_version);

        int error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;

        // get THUMBDRIVE status info concerning the local AP board
        axe_eth_shelf::thumbdrive_status_t thumbdrive_status ;
        call_result = acs_apbm_shelvesdatamanager::get_local_ap_board_thumbdrive_status(thumbdrive_status);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting THUMBDRIVE status info. Call 'acs_apbm_shelvesdatamanager::get_local_ap_board_thumbdrive_status()' failed ! call_result == %d", this->getID(), call_result);
                error_code = acs_apbm::PCP_ERROR_CODE_END_OF_DATA;
        }

        // build response primitive
        acs_apbm_primitive resp_prim(_session);
        call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_THUMBDRIVE_STATUS_REPLY_ID, 1, error_code, thumbdrive_status);
        if(call_result < 0)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
                return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
        }

        // send response primitive
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, thumbdrive_status == %u", this->getID(), acs_apbm::PCP_GET_THUMBDRIVE_STATUS_REPLY_ID, 1, error_code, thumbdrive_status);
        call_result = sendPrimitive(resp_prim);
        if(call_result < 0)
                return ACS_APBM_BSH_SEND_ERROR;

        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

        return ACS_APBM_BSH_SUCCESS;
}
//Redesign as per TR-HS30773

acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_set_nic_info_request(const acs_apbm_primitive & pdh)
{
        // unpack the received primitive
        int primitive_id;
        int32_t other_slot;
        unsigned int primitive_version;
        axe_eth_shelf::nic_status_t nic_status;
        char nic_name [acs_apbm::PCP_FIELD_SIZE_NIC_NAME + 1] = {0};
        char nic_ipv4_address [acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS + 1] = {0};
        char nic_mac_address [acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS + 1] = {0};
        int error_code = 0;
        _server_working_set->cs_reader->get_my_partner_slot(other_slot);
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "other slot found = %d", other_slot);

        int call_result = pdh.unpack_primitive(primitive_id, primitive_version,& error_code, & nic_status, nic_name, nic_ipv4_address, nic_mac_address);

        if(call_result < 0)
          {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a SET_NIC_INFO request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
            return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
          }
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive SET_NIC_INFO request: primitive_id == %d, primitive_version == %u", this->getID(), primitive_id, primitive_version);
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Verifying error code in order to handle special events");

        if (error_code == acs_apbm::PCP_ERROR_CODE_RESET_NIC_INFO)
        {
        	ACS_APBM_LOG(LOG_LEVEL_INFO, "Received RESET_NIC_INFO. All the network information of board on slot %d will be reset",other_slot);
        	call_result = _server_working_set->shelves_data_manager->update_ap_board_default_nic_values(other_slot);


        	acs_apbm_primitive resp_prim(_session);
        	call_result = resp_prim.make_primitive(acs_apbm::PCP_SET_NIC_INFO_REPLY_ID, 1, error_code);
        	if(call_result < 0)
        	{
        		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
        		call_result = sendPrimitive(resp_prim);
        		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
        	}
        	// send response primitive
        	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d", this->getID(), acs_apbm::PCP_SET_NIC_INFO_REPLY_ID, 1, error_code);
        	call_result = sendPrimitive(resp_prim);
        	if(call_result < 0)
        		return ACS_APBM_BSH_SEND_ERROR;

        	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

        	return ACS_APBM_BSH_SUCCESS;
        }

        error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;

        call_result = _server_working_set->shelves_data_manager->update_ap_board_network_interface_card_info( nic_status, nic_name, nic_ipv4_address, nic_mac_address, true, other_slot);
        if(call_result < 0)
          {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error updating NIC INFO. Call 'acs_apbm_shelvesdatamanager::update_ap_board_network_interface_card_info()' failed ! call_result == %d", this->getID(), call_result);
            error_code = acs_apbm::PCP_ERROR_CODE_GENERIC_FAILURE;
          }

        // Get APG shelf architecture (SCB, SCX, DMX, VIRTUALIZED, ...)
		int env = -1;
		if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");  // not a critical error for the purpose of this method
		}

        if(!_server_working_set->shelves_data_manager->is_cs_operation_completed
        		|| ((env != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED) && !_server_working_set->shelves_data_manager->is_snmp_data_operation_completed))
        {
        	ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error during update, server is not up&running yet. "
        			"Load CS operation not complete or Load Snmp data not completed", this->getID());
        	error_code = acs_apbm::PCP_ERROR_CODE_GENERIC_FAILURE;
        }
        // build response primitive
        acs_apbm_primitive resp_prim(_session);
        call_result = resp_prim.make_primitive(acs_apbm::PCP_SET_NIC_INFO_REPLY_ID, 1, error_code);
        if(call_result < 0)
          {
            ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
            return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
          }

        // send response primitive
        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d", this->getID(), acs_apbm::PCP_SET_NIC_INFO_REPLY_ID, 1, error_code);
        call_result = sendPrimitive(resp_prim);
        if(call_result < 0)
          return ACS_APBM_BSH_SEND_ERROR;

        ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

        return ACS_APBM_BSH_SUCCESS;
}
acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_getownslot_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a GET_OWN_SLOT request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive GET_OWN_SLOT request: primitive_id == %d, primitive_version == %u", this->getID(), primitive_id, primitive_version);

	int error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;

	// get own slot
	int32_t own_slot;
	call_result = _server_working_set->cs_reader->get_my_slot(own_slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting own slot. Call 'acs_apbm_csreader::get_my_slot()' failed ! call_result == %d", this->getID(), call_result);
                error_code = acs_apbm::PCP_ERROR_CODE_END_OF_DATA;
                //TODO error_code = acs_apbm::PCP_ERROR_CODE_GENERIC_FAILURE;
	}

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_OWN_SLOT_REPLY_ID, 1, error_code, own_slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, own_slot == %d", this->getID(), acs_apbm::PCP_GET_DISK_STATUS_REPLY_ID, 1, error_code, own_slot);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}

acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_board_presence_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int8_t board_name;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & board_name);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a BOARD PRESENCE request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive BOARD PRESENCE request: primitive_id == %d, primitive_version == %u,  board_name == %d", this->getID(), primitive_id, primitive_version, board_name);

	acs_apbm_csboardinfo ap_board;

	call_result = _server_working_set->cs_reader->get_apbm_board(board_name, ap_board);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting apbm board. Call 'acs_apbm_csreader::get_apbm_board()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_CANNOT_RETRIEVE_INFO_FROM_CS;
	}
	axe_eth_shelf::status_t apbm_board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
	// check if AP board exists in current configuration
	if (ap_board.slot_position == -1) {
		apbm_board_status = axe_eth_shelf::BOARD_STATUS_NOT_CONFIGURED;
	}
	else {
		// check if AP board exists (i.e. it's physically present) and get its status
		if((call_result =_server_working_set->shelves_data_manager->get_ap_board_status(apbm_board_status, ap_board.slot_position))){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'shelves_data_manager->get_ap_board_status (slot_position == %d)' failed !  call_result == %d", ap_board.slot_position, call_result);
		}
	}
	int8_t resp_board_state;
	// check AP board status
	switch(apbm_board_status){
		case axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED:
		case axe_eth_shelf::BOARD_STATUS_MISSING:
		case axe_eth_shelf::BOARD_STATUS_NOT_APPLICABLE:
		case axe_eth_shelf::BOARD_STATUS_FAULTY:
			resp_board_state = acs_apbm::BOARD_PRESENCE_BOARD_NOT_AVAILABLE;
			break;
		case axe_eth_shelf::BOARD_STATUS_WORKING:
			resp_board_state = acs_apbm::BOARD_PRESENCE_BOARD_PRESENT;
			break;
		case axe_eth_shelf::BOARD_STATUS_NOT_CONFIGURED:
			resp_board_state = acs_apbm::BOARD_PRESENCE_BOARD_NOT_CONFIGURED;
			break;
		default:
			// AP board is already unblocked !
			//ACS_APBM_LOG(LOG_LEVEL_ERROR, "'DEBLOCk' action failed because the AP board is NOT blocked ! The state of the requested AP board ( MAG == 0x%08X, SLOT == %d ) is '%d'", magazine, slot, apboard_status);
			resp_board_state = acs_apbm::BOARD_PRESENCE_ERROR;
			break;
	}
	int error_code = acs_apbm::ERR_NO_ERRORS;
	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_BOARD_PRESENCE_REPLY_ID, 1, error_code, resp_board_state);
	if(call_result < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, resp_board_state == %d", this->getID(), acs_apbm::PCP_BOARD_PRESENCE_REPLY_ID, 1, error_code, resp_board_state);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}

acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_board_status_request(const acs_apbm_primitive & pdh)
{
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int8_t board_name;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & board_name);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a BOARD STATUS request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive BOARD STATUS request: primitive_id == %d, primitive_version == %u,  board_name == %d", this->getID(), primitive_id, primitive_version, board_name);

	acs_apbm_csboardinfo ap_board;

	call_result = _server_working_set->cs_reader->get_apbm_board(board_name, ap_board);
	if(call_result < 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting apbm board. Call 'acs_apbm_csreader::get_apbm_board()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_CANNOT_RETRIEVE_INFO_FROM_CS;
	}

	int error_code = acs_apbm::ERR_NO_ERRORS;
	axe_eth_shelf::status_t apbm_board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;

	// check if AP board exists in current configuration
	if (ap_board.slot_position == -1) {
		apbm_board_status = axe_eth_shelf::BOARD_STATUS_NOT_CONFIGURED;
		ACS_APBM_LOG(LOG_LEVEL_WARN, "the board '%d' in not in the current configuration!", board_name);
	}
	else {
		// check if AP board exists (i.e. it's physically present) and get its status
		if((call_result =_server_working_set->shelves_data_manager->get_ap_board_status(apbm_board_status, ap_board.slot_position))){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'shelves_data_manager->get_ap_board_status (slot_position == %d)' failed !  call_result == %d", ap_board.slot_position, call_result);
		}
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'shelves_data_manager->get_ap_board_status(slot_position == %d)' returns <apbm_board_status == %d>", ap_board.slot_position, apbm_board_status);

	int8_t resp_board_state;
	// check AP board status
	switch(apbm_board_status){
		case axe_eth_shelf::BOARD_STATUS_MANUALLY_BLOCKED:
			 resp_board_state = acs_apbm::BOARD_STATUS_BOARD_BLOCKED;
			 break;
		case axe_eth_shelf::BOARD_STATUS_MISSING:
			resp_board_state = acs_apbm::BOARD_STATUS_BOARD_MISSING;
			break;
		case axe_eth_shelf::BOARD_STATUS_NOT_APPLICABLE:
			 resp_board_state = acs_apbm::BOARD_STATUS_ERROR;
			 break;
		case axe_eth_shelf::BOARD_STATUS_FAULTY:
			resp_board_state = acs_apbm::BOARD_STATUS_BOARD_FAULTY;
			break;
		case axe_eth_shelf::BOARD_STATUS_WORKING:
			resp_board_state = acs_apbm::BOARD_STATUS_BOARD_WORKING;
			break;
		case axe_eth_shelf::BOARD_STATUS_NOT_CONFIGURED:
			resp_board_state = acs_apbm::BOARD_STATUS_ERROR;
			break;
		default:
			resp_board_state = acs_apbm::BOARD_STATUS_ERROR;
			break;
		}

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_BOARD_STATUS_REPLY_ID, 1, error_code, resp_board_state);
	if(call_result < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, resp_board_state == %d", this->getID(), acs_apbm::PCP_BOARD_STATUS_REPLY_ID, 1, error_code, resp_board_state);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}

acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_board_location_request(const acs_apbm_primitive & pdh){
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int8_t board_name;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & board_name);

	if(call_result < 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected a BOARD LOCATION request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive BOARD LOCATION request: primitive_id == %d, primitive_version == %u,  board_name == %d", this->getID(), primitive_id, primitive_version, board_name);

	acs_apbm_csboardinfo ap_board;
	call_result = _server_working_set->cs_reader->get_apbm_board(board_name, ap_board);
	if(call_result < 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting apbm board. Call 'acs_apbm_csreader::get_apbm_board()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_CANNOT_RETRIEVE_INFO_FROM_CS;
	}
	int error_code = acs_apbm::ERR_NO_ERRORS;
	// check if AP board exists in current configuration
	if (ap_board.slot_position == -1) {
		error_code = axe_eth_shelf::BOARD_STATUS_NOT_CONFIGURED;
		ACS_APBM_LOG(LOG_LEVEL_WARN, "the board '%d' in not in the current configuration!", board_name);
	}
	else {
		std::vector<uint16_t> ap_sys_no_list;
		call_result = _server_working_set->cs_reader->get_ap_sys_no(board_name, ap_sys_no_list);
		if(call_result < 0){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting ap_sys_no. Call 'acs_apbm_csreader::get_ap_sys_no()' failed ! call_result == %d", this->getID(), call_result);
			return ACS_APBM_BSH_CANNOT_RETRIEVE_INFO_FROM_CS;
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, ap_sys_no_list.size == %u", this->getID(), acs_apbm::PCP_BOARD_LOCATION_REPLY_ID, 1, error_code, ap_sys_no_list.size());
		for(size_t i=0; i< ap_sys_no_list.size();i++ ){
			// build response primitive
			acs_apbm_primitive resp_prim(_session);
			call_result = resp_prim.make_primitive(acs_apbm::PCP_BOARD_LOCATION_REPLY_ID, 1, error_code, ap_sys_no_list[i]);
			if(call_result < 0) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
				return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
			}
			// send response primitive
			ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, ap_sys_no == %u", this->getID(), acs_apbm::PCP_BOARD_LOCATION_REPLY_ID, 1, error_code, ap_sys_no_list[i]);
			call_result = sendPrimitive(resp_prim);
			if(call_result < 0)
				return ACS_APBM_BSH_SEND_ERROR;
		}
	}
	acs_apbm_primitive end_of_data_resp_prim(_session);
	call_result = end_of_data_resp_prim.make_primitive(acs_apbm::PCP_BOARD_LOCATION_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_END_OF_DATA, 0);

	if(call_result < 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing 'end of data' response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send 'end of data' response primitive", this->getID());
	call_result = sendPrimitive(end_of_data_resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. End of data response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_set_board_status_request(const acs_apbm_primitive & pdh){
	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	int8_t board_name, reported_board_status;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & board_name, & reported_board_status);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected  SET BOARD STATUS request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive SET BOARD STATUS request: primitive_id == %d, primitive_version == %u,  board_name == %d reported_status == %d", this->getID(), primitive_id, primitive_version, board_name, reported_board_status);

	acs_apbm_csboardinfo ap_board;

	call_result = _server_working_set->cs_reader->get_apbm_board(board_name, ap_board);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting apbm board. Call 'acs_apbm_csreader::get_apbm_board()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_CANNOT_RETRIEVE_INFO_FROM_CS;
	}

	acs_apbm_shelvesdatamanager::sdm_constant ret_code = acs_apbm_shelvesdatamanager::SDM_OK;
	axe_eth_shelf::status_t current_board_status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;
	// check if AP board exists in current configuration
	if (ap_board.slot_position == -1) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, " AP Board <board_name == %d> is not in current configuration!", board_name);
		return send_set_board_status_reply(acs_apbm::ERR_CS_BOARD_NOT_FOUND);
	}

	// check if AP board exists (i.e. it's physically present) and get its status
	if((ret_code = _server_working_set->shelves_data_manager->get_ap_board_status(current_board_status, ap_board.slot_position))){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'shelves_data_manager->get_ap_board_status (slot_position == %d)' failed !  call_result == %d", ap_board.slot_position, ret_code);
		return send_set_board_status_reply(acs_apbm::ERR_GENERIC_ERROR);
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, " CURRENT BOARD STATUS : %d",current_board_status); 
	if (current_board_status == axe_eth_shelf::BOARD_STATUS_MISSING){
		ACS_APBM_LOG(LOG_LEVEL_INFO, " AP Board <board_name == %d> is MISSING !", board_name);
		return send_set_board_status_reply(acs_apbm::ERR_SNMP_BOARD_NOT_FOUND);
	}

	axe_eth_shelf::status_t new_board_status = (reported_board_status == acs_apbm::REPORTED_BOARD_STATUS_FAULTY)? axe_eth_shelf::BOARD_STATUS_FAULTY : axe_eth_shelf::BOARD_STATUS_WORKING;
	ACS_APBM_LOG(LOG_LEVEL_INFO, " REPORTED BOARD STATUS : %d",reported_board_status);
	switch (reported_board_status){
		case acs_apbm::REPORTED_BOARD_STATUS_WORKING :
			//REMOVE ALARM( ALARM_BOARD_FAULTY)
			_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDFAULT, ap_board.slot_position);
			if (current_board_status == axe_eth_shelf::BOARD_STATUS_FAULTY){
				ret_code =_server_working_set->shelves_data_manager->set_ap_board_status(new_board_status, true, ap_board.slot_position);
			}
		break;
		case acs_apbm::REPORTED_BOARD_STATUS_FAULTY:
			 if (current_board_status == axe_eth_shelf::BOARD_STATUS_WORKING){
				 //ADD ALARM (ALARM_BOARD_FAULTY)
				 _server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDFAULT, ap_board.slot_position);
				 ret_code =_server_working_set->shelves_data_manager->set_ap_board_status(new_board_status, true, ap_board.slot_position);
			  }
		break;
		default:
			// should not occur!
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid reported_status value received  <reported_board_status == %d>!", reported_board_status);
			return send_set_board_status_reply(acs_apbm::ERR_GENERIC_ERROR);
			break;
	}

	if(ret_code != acs_apbm_shelvesdatamanager::SDM_OK){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'shelvessend_primitive_board_presence_request_data_manager->set_ap_board_status' failed! Cannot update reported AP board status <ret_code == %d>!", ret_code);
		return send_set_board_status_reply(acs_apbm::ERR_GENERIC_ERROR);
	}

	return  send_set_board_status_reply(acs_apbm::ERR_NO_ERRORS);

}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_get_ipmifw_status_request(const acs_apbm_primitive & pdh){

	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	uint32_t magazine;
	int32_t slot;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & magazine, & slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive == %d  version == %d (we expected  IPMIFW STATUS request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(),primitive_id,  primitive_version, call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive IPMIFW STATUS request: primitive_id == %d, primitive_version == %u,  magazine == %u slot == %d", this->getID(), primitive_id, primitive_version, magazine, slot);

	int error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;

	// get IPMIFW UPG status info concerning the  AP board
	axe_eth_shelf::ipmi_upgrade_status_t  ipmiupg_status = axe_eth_shelf::IPMI_UPGRADE_STATUS_UNDEFINED;
	char ipmifw_lastupgtime[acs_apbm::PCP_FIELD_SIZE_IPMIFW_DATETIME] = {0};

	call_result = _server_working_set->shelves_data_manager->get_ipmifw_status(ipmiupg_status, ipmifw_lastupgtime, slot, magazine);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting IPMIFW status info. Call 'acs_apbm_shelvesdatamanager::get_ipmifw_status()' failed ! call_result == %d", this->getID(), call_result);
				//error_code = acs_apbm::PCP_ERROR_CODE_END_OF_DATA;
				error_code = acs_apbm::PCP_ERROR_CODE_GENERIC_FAILURE;
	}
	uint8_t tmp_ipmi_status = ipmiupg_status;
	//ipmiupg_status = 0;
	//std::strcpy(ipmifw_lastupgtime, "2012-07-12 10:00:34");
	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_IPMIUPG_STATUS_REPLY_ID, 1, error_code, tmp_ipmi_status, ipmifw_lastupgtime);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, ipmifw_status == %u, ipmifw_lastupgtime == %s", this->getID(), acs_apbm::PCP_GET_IPMIUPG_STATUS_REPLY_ID, 1, error_code, tmp_ipmi_status, ipmifw_lastupgtime);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());
	// Code below to solve the RED LED ON issue after IPMI UPG for 3LED HW
	if (tmp_ipmi_status == axe_eth_shelf::IPMI_UPGRADE_STATUS_READY)
	{
		if(!_server_working_set->shelves_data_manager->is_4led_supported || !_server_working_set->shelves_data_manager->is_new_gea_supported)
	        {	
		        ACS_APBM_LOG(LOG_LEVEL_INFO,"IPMI UPGRADE DONE!.... SETTING RED LED TO OFF ! slot == %d, magazine = 0x%08X ", slot, magazine);
                	if(_server_working_set->snmp_manager->set_board_REDled(magazine,slot,axe_eth_shelf::LED_STATUS_OFF) < 0)
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
	}
	return ACS_APBM_BSH_SUCCESS;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_get_ipmifw_upgrade_request(const acs_apbm_primitive & pdh){

	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	uint32_t magazine;
	int32_t slot;
	uint8_t comport;
	char ipmipkg_name[PATH_MAX] = {0};

	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & magazine, & slot, & comport, ipmipkg_name);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive (we expected  IPMIFW UPGRADE request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(), call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive IPMIFW UPGRADE request: primitive_id == %d, primitive_version == %u,  magazine == %u slot == %d comport == %d ipmipkg_name == %s",
			   this->getID(), primitive_id, primitive_version, magazine, slot, comport, ipmipkg_name);

	int error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;


	//  invoke operation
	call_result = _server_working_set->imm_repository_handler->handle_ipmiupgrade_operation(ipmipkg_name, slot, magazine, comport);

	error_code = call_result;

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_IPMIUPG_UPGRADE_REPLY_ID, 1, error_code);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d", this->getID(), acs_apbm::PCP_IPMIUPG_UPGRADE_REPLY_ID, 1, error_code);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;

}

acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::handle_get_ipmifw_data_request(const acs_apbm_primitive & pdh){

	// unpack the received primitive
	int primitive_id;
	unsigned int primitive_version;
	uint32_t magazine;
	int32_t slot;
	int call_result = pdh.unpack_primitive(primitive_id, primitive_version, & magazine, & slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error unpacking received primitive == %d  version == %d (we expected  IPMIFW DATA request) ! acs_apbm_primitivedatahandler<>::unpack_primitive() call returned < %d >", this->getID(),primitive_id,  primitive_version, call_result);
		return ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Received primitive IPMIFW DATA request: primitive_id == %d, primitive_version == %u,  magazine == %u slot == %d", this->getID(), primitive_id, primitive_version, magazine, slot);

	int error_code = acs_apbm::PCP_ERROR_CODE_SUCCESS;

	// get IPMIFW UPG data info concerning the  AP board
	int16_t ipmifw_type;
	char product_number [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1] = {0};
	char revision [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1] ={0};
	axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status = axe_eth_shelf::IPMI_UPGRADE_STATUS_UNDEFINED;
	const unsigned int timeout = 2000;
	call_result = _server_working_set->snmp_manager->get_board_ipmi_data(ipmifw_type, product_number, revision, ipmi_upgrade_status, slot, magazine, &timeout);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"The IPMI ipmifw_type,product number and revision retrieved are %d, %s %s",ipmifw_type,product_number,revision);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error getting IPMIFW data info. Call 'snmp_manager::get_board_ipmi_data()' failed ! call_result == %d", this->getID(), call_result);
		//error_code = acs_apbm::PCP_ERROR_CODE_GENERIC_FAILURE;
		error_code = call_result;
	//	error_code = acs_apbm::ERR_IPMIUPG_SNMP_ERROR;
	}
	
	if(( call_result = _server_working_set->imm_repository_handler->get_board_ipmi_upgrade_status(ipmi_upgrade_status, slot, magazine)) < 0){
                ACS_APBM_LOG(LOG_LEVEL_ERROR,"Cannot get_board ipmi upgrade status ( MAG == %u, SLOT == %d ). Call 'imm_repository_handler->get_board_ipmi_upgrade_status(..)' failed. call_result == %d !", magazine, slot, call_result);
        }
	if (ipmi_upgrade_status == axe_eth_shelf::IPMI_UPGRADE_STATUS_ONGOING)
        {
                ACS_APBM_LOG(LOG_LEVEL_WARN,"Update ON GOING, Could not update IPMIFW DATA!");
                error_code= acs_apbm::ERR_IPMIUPG_UPGRADE_ALREADY_ONGOING;
        }

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_IPMIFW_DATA_REPLY_ID, 1, error_code, ipmifw_type, product_number, revision);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d, ipmifw_type == %d, product_number == %s, revision == %s", this->getID(), acs_apbm::PCP_GET_IPMIUPG_STATUS_REPLY_ID, 1, error_code, ipmifw_type, product_number, revision);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;

}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::notifyTrap(const trapdata & trap)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Got a TRAP (from the SUBSCRIBTION MANAGER) ", this->getID());
	uint32_t n_pending_traps;

	// this operation can be executed only if the object is subscribed for receiving  AP boards TRAPs
	if(!_subscribed)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Cannot perform 'notifyTrap' operation on this object ! _subscribed == %d, _subscription_bitmap == 0x%08X", this->getID(), _subscribed, _subscription_bitmap);
		return ACS_APBM_BSH_OP_NOT_PERMITTED;
	}

	// add the trap to the session queue
	_received_traps_mutex.acquire();
	_received_traps.push_back(trap);
	n_pending_traps = _received_traps.size();
	_received_traps_mutex.release();

	return _notifyTrapToPeer(n_pending_traps);
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::_notifyTrapToPeer(uint32_t n_pending_traps)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_pending_trap_notification_mutex);
	if(!_pending_trap_notification)
	{
		// send a NOTIFY_TRAP primitive to the peer
		acs_apbm_boardsessionhandler_constants call_result = sendNotifyPrimitive(n_pending_traps, SEND_PRIMITIVE_TIMEOUT);
		if(call_result != ACS_APBM_BSH_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. 'notifyTrap()' call failed ! call_result == %d >", this->getID(), call_result);
			return ACS_APBM_BSH_ERROR;
		}

		// take note that now there is a pending notification on the session
		_pending_trap_notification = 1;
	}
	else
		ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. There's YET a pending notification on this session", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}


uint32_t acs_apbm_boardsessionhandler::sync_get_ID()
{
	ACE_Guard<ACE_Thread_Mutex> guard(acs_apbm_boardsessionhandler::_next_available_ID_mutex);
	return _next_available_ID++;
}


int acs_apbm_boardsessionhandler::_reconstruct_client_commamd_from_request(char * buf, int len, int8_t architecture_type, uint32_t magazine)
{
	int tmp ;
	if(architecture_type == acs_apbm::ARCHITECTURE_TYPE_CBA)
	{
		if(magazine != acs_apbm::BOARD_MAGAZINE_UNDEFINED)
		{
			char mag_str[16] = {0};
			acs_apbm_csreader::uint32_to_ip_format(mag_str, magazine);
			tmp = snprintf(buf, len, "hwmxls -m %s", mag_str);
		}
		else
			tmp = snprintf(buf, len, "hwmxls");
	}
	else if(architecture_type == acs_apbm::ARCHITECTURE_TYPE_NOT_CBA)
		tmp = snprintf(buf, len, "hwmscbls");
	else
		return -1;

	if(tmp >= len)
		return -2;	// buffer overflow

	return 0;
}


acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::send_invalid_arch_switchboarddata_reply_primitive(int8_t architecture_type)
{
	int call_result = 0;
	char dummy_ip_str[16] = {0};

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	if(architecture_type == acs_apbm::ARCHITECTURE_TYPE_NOT_CBA)
		call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_SCB_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_INVALID_ARCHITECTURE,  acs_apbm::BOARD_SLOT_UNDEFINED, dummy_ip_str, dummy_ip_str, acs_apbm::SWITCH_BOARD_STATE_UNINTT_STATUS, acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_UNINTT);
	else
		call_result = resp_prim.make_primitive(acs_apbm::PCP_GET_EGEM2L2_SWITCH_DATA_REPLY_ID, 1, acs_apbm::PCP_ERROR_CODE_INVALID_ARCHITECTURE,  acs_apbm::BOARD_MAGAZINE_UNDEFINED, acs_apbm::BOARD_SLOT_UNDEFINED, dummy_ip_str, dummy_ip_str, acs_apbm::SWITCH_BOARD_STATE_UNINTT_STATUS, acs_apbm::SWITCH_BOARD_NEIGHBOUR_STATE_UNINTT, acs_apbm::SWITCH_BOARD_FBN_UNKNOWN);

	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive to the client
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	return ACS_APBM_BSH_SUCCESS;
}

acs_apbm_boardsessionhandler::acs_apbm_boardsessionhandler_constants acs_apbm_boardsessionhandler::send_set_board_status_reply(int error_code){

	// build response primitive
	acs_apbm_primitive resp_prim(_session);
	int call_result = resp_prim.make_primitive(acs_apbm::PCP_SET_BOARD_STATUS_REPLY_ID, 1, error_code);
	if(call_result < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CONN_ID = < %d >. Error preparing response primitive. Call 'acs_apbm_primitivedatahandler<>::make_primitive()' failed ! call_result == %d", this->getID(), call_result);
		return ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR;
	}

	// send response primitive
	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Trying to send response primitive: primitive_id == %d, primitive_version == %u, error_code == %d", this->getID(), acs_apbm::PCP_SET_BOARD_STATUS_REPLY_ID, 1, error_code);
	call_result = sendPrimitive(resp_prim);
	if(call_result < 0)
		return ACS_APBM_BSH_SEND_ERROR;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "CONN_ID = < %d >. Response primitive successfully sent", this->getID());

	return ACS_APBM_BSH_SUCCESS;
}
