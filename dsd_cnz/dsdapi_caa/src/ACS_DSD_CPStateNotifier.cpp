#include <poll.h>

#include "ACS_DSD_MessageSender.h"
#include "ACS_DSD_MessageReceiver.h"
#include "ACS_DSD_PrimitiveDataHandler.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_CPStateNotifier.h"

extern const char * const _repeated_messages[];
#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

int __CLASS_NAME__::attach_to (int32_t system_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check state: if already attached report the error
	if (state() ^ acs_dsd::CP_NOTIFIER_STATE_DETACHED) ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_CP_STATE_NOTIFIER_NOT_DETACHED);

	//Check the parameter system_id
	if (ACS_DSD_ConfigurationHelper::system_id_to_system_type(system_id) ^ acs_dsd::SYSTEM_TYPE_CP)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_SYSTEM_ID, 0, "system_id (== %d) parameter doesn't indicate a CP system", system_id);

	//Do the CP specified by system_id exist? Check the system configuration
	if ((acs_dsd::CONFIG_CP_ID_SET_START <= system_id) && (system_id <= acs_dsd::CONFIG_CP_ID_SET_END) &&
			!ACS_DSD_ConfigurationHelper::system_exist(system_id))
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_SYSTEM_ID, 0,
				"system_id (== %d) parameter refers to a CP system that doesn't exist in the system configuration", system_id);

	//Try to connect to the DSD server.
	ACS_DSD_Client client_connector;

	int call_result = 0;
	const char * dsd_sap_address = ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname();
	int errno_save = 0;

	errno = 0;
	if ((call_result = client_connector.connect(_session, dsd_sap_address))) //ERROR: problems while connecting to the DSD server
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, call_result < 0 ? errno : 0, call_result < 0 ? _repeated_messages[4] : _repeated_messages[5], dsd_sap_address);

	ACS_DSD_ProtocolHandler ph(_session);

	errno = 0;
	if ((call_result = ph.send_44(system_id)) < 0) {
		errno_save = errno;
		_session.close();
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save, "Notification request error: cannot request notification about state changes for the system id %d",
				system_id);
	}

	(call_result = receive_cp_state_indication(ph)) ? _session.close() : _system_id = system_id;

	return call_result;
}

int __CLASS_NAME__::detach () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	_session.close();
	_system_id = acs_dsd::SYSTEM_ID_UNKNOWN;
	_cp_state = acs_dsd::CP_STATE_UNKNOWN;

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::get_cp_state (acs_dsd::CPStateConstants & cp_state) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check state: if not attached report the error
	if (state() ^ acs_dsd::CP_NOTIFIER_STATE_ATTACHED) ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_CP_STATE_NOTIFIER_NOT_ATTACHED);

	int handles_count = 0;
	int call_result = 0;

	errno = 0;
	if ((call_result = _session.get_handles(0, handles_count)) && (call_result ^ acs_dsd::ERR_NOT_ENOUGH_SPACE))
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno, "Asking the handles count to the session object");

	if (handles_count <= 0) //Internal session object seems corrupted
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_OBJECT_CORRUPTED, 0,
				"Internal session object seems corrupted: it doesn't have any valid communication handles (handle count == %d): "
				"session last error == %d, session last error text == '%s'", handles_count, _session.last_error(), _session.last_error_text());

	acs_dsd::HANDLE handles[handles_count];

	_session.get_handles(handles, handles_count);

	pollfd poll_set[handles_count];
	for (int i = 0; i < handles_count; poll_set[i++].events = POLLIN | POLLPRI
#ifdef __USE_XOPEN
		| POLLRDNORM | POLLRDBAND
#endif

#ifdef __USE_GNU
		| POLLMSG
#endif
	) poll_set[i].fd = handles[i];

	call_result = set_error_info(acs_dsd::ERR_NO_ERRORS);
	int poll_result = 0;

	for (errno = 0; (poll_result = poll(poll_set, handles_count, 0)) > 0; errno = 0)
		if ((errno = 0, call_result = receive_cp_state_indication(_session))) break;

	if (poll_result < 0) //ERROR on poll interface
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_POLL, errno, "checking presence of readable data on the session sockets connected to the DSD server");

	cp_state = _cp_state;

	return call_result;
}

int __CLASS_NAME__::get_handles (acs_dsd::HANDLE * handles, int & handle_count) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if attached. A CP state notifier should be attached before it can retrieve internal handles.
	if (acs_dsd::CP_NOTIFIER_STATE_DETACHED == state()) ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_CP_STATE_NOTIFIER_NOT_ATTACHED);

	return _session.get_handles(handles, handle_count);
}

int __CLASS_NAME__::receive_cp_state_indication (ACS_DSD_ProtocolHandler & ph) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;
	int prim_id_received = 0;
	unsigned prim_version_received = 0;
	uint8_t cp_state = 0;

	prim_id_received = 0;
	errno = 0;
	call_result = ph.recv_prim(acs_dsd::PCP_DSDAPI_NOTIFICATION_INDICATION_ID, prim_id_received, prim_version_received, cp_state);
	int errno_save = errno;

	if (acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED == call_result)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0,
				"CP state indication error: the DSD server replied with the primitive number %d instead of the primitive number %d",
				prim_id_received, acs_dsd::PCP_DSDAPI_NOTIFICATION_INDICATION_ID);
	else if (call_result < 0) ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save, "CP state indication error: DSD server error");

	_cp_state = static_cast<acs_dsd::CPStateConstants>(cp_state);

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::receive_cp_state_indication (const ACS_DSD_Session & session) const {
	ACS_DSD_ProtocolHandler ph(session);
	return receive_cp_state_indication(ph);
}
