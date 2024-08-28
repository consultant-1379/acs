#include <cstdio>
#include <errno.h>
#include <stdarg.h>

#include "acs_prc_api.h"

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_Communicator.h"

extern const char * ACS_DSD_WarningTexts[];
extern const char * ACS_DSD_ErrorTexts[];

extern const char * const _repeated_messages[] = {
	"",
	". Try to reuse internals created acceptors leaving 'reuse_internals' parameter to its default value 1",
	". Internal acceptor objects seem corrupted, destroy this ACS_DSD_Server instance and try using a new one",
	"This object is probably corrupted, try to use a new one",
	"Cannot connect to the DSD server; the server is unavailable onto the sap address '%s'. System error follows",
	"The I/O stream used to connect to the DSD server is already in use; this is a dangerous and strange error that should not be occurred on the private dsd interface"
};

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

int __CLASS_NAME__::get_local_node (ACS_DSD_Node & node) const {
	static ACS_DSD_Node local_node;

	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;

	if (local_node.system_id == acs_dsd::SYSTEM_ID_UNKNOWN) {
		errno = 0;
		if ((call_result = ACS_DSD_ConfigurationHelper::load_ap_nodes_from_cs()))
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result < 0 ? call_result : acs_dsd::ERR_LOAD_AP_NODES_FROM_CS, call_result < 0 ? errno : 0,
					"Cannot load system AP nodes network configuration from CS");

		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t my_ap_host_ptr = 0;

		errno = 0;
		if ((call_result = ACS_DSD_ConfigurationHelper::load_my_ap_node()) || (call_result = ACS_DSD_ConfigurationHelper::get_my_ap_node(my_ap_host_ptr)))
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result < 0 ? call_result : acs_dsd::ERR_FIND_MY_AP_NODE_HOST_INFO, call_result < 0 ? errno : 0,
					"Cannot find configuration information about the AP node DSD API is running on: calling 'load_my_ap_node()' and 'get_my_ap_node(...'");

		local_node.system_id = my_ap_host_ptr->system_id;
		memccpy(local_node.system_name, my_ap_host_ptr->system_name, 0, acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX);
		local_node.system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX - 1] = 0;
		local_node.system_type = my_ap_host_ptr->system_type;
		local_node.node_state = acs_dsd::NODE_STATE_UNDEFINED;
		memccpy(local_node.node_name, my_ap_host_ptr->node_name, 0, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);
		local_node.node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX - 1] = 0;
		local_node.node_side = my_ap_host_ptr->side;
	}

	node = local_node;

	call_result = ((node.system_id ^ acs_dsd::SYSTEM_ID_UNKNOWN) ? get_node_state(acs_dsd::SYSTEM_ID_THIS_NODE, node.node_state) : 0);

	return call_result;
}

int __CLASS_NAME__::set_error_info (int error_code) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int error_index = 0;
	const char ** texts = 0;
	(error_code >= 0) ? (error_index = error_code, texts = ACS_DSD_WarningTexts) : (error_index = -error_code, texts = ACS_DSD_ErrorTexts);

	memccpy(_last_error_text, texts[error_index], 0, LAST_ERROR_TEXT_SIZE);
	_last_error_text[LAST_ERROR_TEXT_SIZE - 1] = 0;

	return (_last_error = error_code);
}

int __CLASS_NAME__::set_error_info (int error_code, int system_error, const char * format, ...) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int error_index = 0;
	const char ** texts = 0;
	(error_code >= 0) ? (error_index = error_code, texts = ACS_DSD_WarningTexts) : (error_index = -error_code, texts = ACS_DSD_ErrorTexts);

	int index = snprintf(_last_error_text, LAST_ERROR_TEXT_SIZE, "%s", texts[error_index]);
	if (format && *format) {
		va_list ap;
		va_start(ap, format);
		index += snprintf(_last_error_text + index, LAST_ERROR_TEXT_SIZE - index, ": ");
		index += vsnprintf(_last_error_text + index, LAST_ERROR_TEXT_SIZE - index, format, ap);
		va_end(ap);
	}

	system_error && ({char buf[256]; snprintf(_last_error_text + index, LAST_ERROR_TEXT_SIZE - index, ": SYSTEM_ERR[%d, %s]", system_error, strerror_r(system_error, buf, 256));});
	errno = system_error;

	return (_last_error = error_code);
}

int __CLASS_NAME__::get_node_state (acs_dsd::SystemIDConstants system_id, acs_dsd::NodeStateConstants & node_state) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	node_state = acs_dsd::NODE_STATE_UNDEFINED;

	int prc_node_state = 0; //By PRC: 0 == state undefined

	if ((system_id ^ acs_dsd::SYSTEM_ID_THIS_NODE) && (system_id ^ acs_dsd::SYSTEM_ID_PARTNER_NODE))
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0, "Parameter system_id == %d", system_id);

	ACS_PRC_API prc_api;

	prc_node_state = prc_api.askForNodeState(system_id ^ acs_dsd::SYSTEM_ID_THIS_NODE ? ACS_PRC_REMOTE_NODE : ACS_PRC_LOCAL_NODE);

	if (prc_node_state < 0)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_GET_NODE_STATE, 0, "ACS PRC API: prc_node_state == %d, last error == %d, last error text == '%s'",
				prc_node_state, prc_api.getLastError(), prc_api.getLastErrorText());

	switch (prc_node_state) {
	case 1: node_state = acs_dsd::NODE_STATE_ACTIVE; break;
	case 2: node_state = acs_dsd::NODE_STATE_PASSIVE; break;
	default: node_state = acs_dsd::NODE_STATE_UNDEFINED; break;
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::get_partner_node (ACS_DSD_Node & node) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	static ACS_DSD_Node partner_node;

	int call_result = 0;

	if (partner_node.system_id == acs_dsd::SYSTEM_ID_UNKNOWN) {
		errno = 0;
		if ((call_result = ACS_DSD_ConfigurationHelper::load_ap_nodes_from_cs()))
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result < 0 ? call_result : acs_dsd::ERR_LOAD_AP_NODES_FROM_CS, call_result < 0 ? errno : 0,
					"Cannot load system AP nodes network configuration from CS");

		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t my_ap_partner_host_ptr = 0;

		errno = 0;
		if ((call_result = ACS_DSD_ConfigurationHelper::load_my_ap_node()) ||
				(call_result = ACS_DSD_ConfigurationHelper::get_my_ap_partner_node(my_ap_partner_host_ptr)))
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result < 0 ? call_result : acs_dsd::ERR_FIND_MY_AP_PARTNER_NODE_HOST_INFO, call_result < 0 ? errno : 0,
					"Cannot find configuration information about the AP partner node of the AP cluster DSD API is running on: "
					"calling 'load_my_ap_node()' and 'get_my_ap_partner_node(...'");

		partner_node.system_id = my_ap_partner_host_ptr->system_id;
		memccpy(partner_node.system_name, my_ap_partner_host_ptr->system_name, 0, acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX);
		partner_node.system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX - 1] = 0;
		partner_node.system_type = my_ap_partner_host_ptr->system_type;
		partner_node.node_state = acs_dsd::NODE_STATE_UNDEFINED;
		memccpy(partner_node.node_name, my_ap_partner_host_ptr->node_name, 0, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);
		partner_node.node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX - 1] = 0;
		partner_node.node_side = my_ap_partner_host_ptr->side;
	}

	node = partner_node;

	call_result = ((node.system_id ^ acs_dsd::SYSTEM_ID_UNKNOWN) ? get_node_state(acs_dsd::SYSTEM_ID_PARTNER_NODE, node.node_state) : 0);

	return call_result;
}
