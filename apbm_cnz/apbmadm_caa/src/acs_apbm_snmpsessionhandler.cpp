#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ace/Guard_T.h>

// TODO: BEGIN: Calcellare i seguenti include, messi solo per debugging
#include "acs_apbm_logger.h"
// TODO: END

#include "acs_apbm_snmpsessionhandler.h"

int __CLASS_NAME__::close () {
	// Check the session state: if already closed return with error
	if (state() == acs_apbm_snmp::SNMP_SESSION_STATE_CLOSED) return acs_apbm_snmp::ERR_SNMP_SESSION_NOT_OPEN;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (state() == acs_apbm_snmp::SNMP_SESSION_STATE_CLOSED) return acs_apbm_snmp::ERR_SNMP_SESSION_NOT_OPEN;

	if (_session_handle) {
		if (!snmp_sess_close(_session_handle)) //ERROR: closing the underlying net-snmp session
			return acs_apbm_snmp::ERR_SNMP_SESSION_CLOSE;
		_session_handle = 0;
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::get_fd_set (int & fd_count, int & fd_sup, fd_set & set) const {
	int block = 1;
	struct timeval timeout;

	FD_ZERO(&set);

	fd_count = snmp_sess_select_info(_session_handle, &fd_sup, &set, &timeout, &block);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::read () const {
	fd_set set;
	int fd_count = 0;

	get_fd_set(fd_count, set);

	return read(set);
}

int __CLASS_NAME__::send_synch (netsnmp_pdu * & request, const unsigned * timeout_ms) const {
	/*
	 * Send the pdu to the target identified by this session.
	 * Return on success:
	 *   The request id of the pdu is returned, and the pdu is freed.
	 * Return on failure:
	 *   Zero (0) is returned.
	 *   The caller must call snmp_free_pdu if 0 is returned.
	 */
	int request_id = snmp_sess_send(_session_handle, request);

	if (!request_id) return acs_apbm_snmp::ERR_SNMP_SESSION_SEND; // ERROR: sending the request

	// OK: request sent. The pdu pointed to by request parameter was freed, so I have to notify the
	// caller setting the parameter to NULL
	request = 0;

	int fd_count = 0;
	int fd_sup = 0;
	fd_set set;

	get_fd_set(fd_count, fd_sup, set);

	if (fd_count <= 0) // ERROR: no file descriptor found for this session handler. This session is invalid
		return acs_apbm_snmp::ERR_SNMP_SESSION_NOT_VALID;

	timeval tv;
	timeval * tv_ptr = 0;

	if (timeout_ms) {
		tv.tv_sec = *timeout_ms / 1000;
		tv.tv_usec = 1000 * (*timeout_ms % 1000);
		tv_ptr = &tv;
	}

	// I can select
	int call_result = ::select(fd_sup, &set, 0, 0, tv_ptr);
	if (call_result < 0) return acs_apbm_snmp::ERR_SNMP_SELECT; // ERROR: calling select
	if (call_result == 0) return acs_apbm_snmp::ERR_SNMP_TIMEOUT;

	// Now I will read the response so the NET_SNMP library can dispatch it to the callback.
	return read(set);
}

int __CLASS_NAME__::open_ (acs_apbm_snmpsessioninfo & session_info, acs_apbm_snmpsessioncallback * callback) {
	// Check the session state: if already open return with error
	if (state() != acs_apbm_snmp::SNMP_SESSION_STATE_CLOSED) return acs_apbm_snmp::ERR_SNMP_SESSION_NOT_CLOSED;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (state() != acs_apbm_snmp::SNMP_SESSION_STATE_CLOSED) return acs_apbm_snmp::ERR_SNMP_SESSION_NOT_CLOSED;

	acs_apbm_snmpsessioninfo session_info_ = session_info;

	// Have the session a callback to set?
	if (callback) { //YES
		session_info_.callback = session_callback_dispatcher;

		// TODO: nel campo 'callback_magic' dovrebbe essere memorizzato il puntatore this a questo oggetto
		// per avere maggiori informazioni e collegare le cose a livello di questo net-snmp wrapper.
		// E l'attuale memorizzazione del parametro 'callback' va in un campo interno a questa classe (vedi
		// il commento successivo).

		session_info_.callback_magic = callback;
	}


	// TODO: l'argomento 'callback' (puntatore alla acs_apbm_snmpsessioncallback) dovrebbe essere memorizzato
	// in un campo di questa classe collegata one-to-one alla sessione di comunicazione di net-snmp sottostante.
	// Nella funzione 'session_callback_dispatcher' sotto poi, bisogna dispatchare seguendo il campo interno, e non
	// direttamente dal callback_magic pointer.


	void * session_handle = 0;

	// Try to open the session
	if (!(session_handle = snmp_sess_open(&session_info_))) { // ERROR: opening the underlying net-snmp session. Return the error code
		// Retrieve errors from NET_SNMP library
		char * snmp_error_text = 0;
		snmp_error(&session_info_, &session_info.s_errno, &session_info.s_snmp_errno, &snmp_error_text);
		session_info.last_snmp_error_text(snmp_error_text);

		return acs_apbm_snmp::ERR_SNMP_SESSION_OPEN;
	}

	_session_handle = session_handle;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::session_callback_dispatcher (int operation, netsnmp_session * netsnmp_session, int request_id, netsnmp_pdu * netsnmp_pdu, void * /*session_callback*/) {
	// References: NET_SNMP API: snmp_api.h::699

	// Dispatching data to the callback
	acs_apbm_snmpsessioncallback * snmp_session_callback = reinterpret_cast<acs_apbm_snmpsessioncallback *>(netsnmp_session->callback_magic);

	return snmp_session_callback->operator()(operation, netsnmp_session, request_id, netsnmp_pdu);
}
