#ifndef HEADER_GUARD_CLASS__acs_apbm_snmpsessionhandler
#define HEADER_GUARD_CLASS__acs_apbm_snmpsessionhandler acs_apbm_snmpsessionhandler

/** @file acs_apbm_snmpsessionhandler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-09-20
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
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2011-09-20 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <ace/Recursive_Thread_Mutex.h>

#include "acs_apbm_macros.h"
#include "acs_apbm_programmacros.h"
#include "acs_apbm_snmpconstants.h"
#include "acs_apbm_snmpsessioninfo.h"
#include "acs_apbm_snmpsessioncallback.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_snmpsessionhandler


/** @class acs_apbm_snmpsessionhandler acs_apbm_snmpsessionhandler.h
 *	@brief acs_apbm_snmpsessionhandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-09-20
 *
 *	acs_apbm_snmpsessionhandler <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_snmpsessionhandler constructor
	 */
	inline __CLASS_NAME__ ()
	: _session_handle(0), _sync_object(ACS_APBM_STRINGIZE(__CLASS_NAME__)"::"ACS_APBM_STRINGIZE(_sync_object)) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_snmpsessionhandler Destructor
	 */
	inline ~__CLASS_NAME__ () { close(); }


	//============//
	// Functions  //
	//============//
public:
	inline acs_apbm_snmp::SnmpSessionStateConstants state () const { return (_session_handle ? acs_apbm_snmp::SNMP_SESSION_STATE_OPEN : acs_apbm_snmp::SNMP_SESSION_STATE_CLOSED); }
	inline int open (acs_apbm_snmpsessioninfo & session_info) { return open_(session_info); }

	inline int open (acs_apbm_snmpsessioninfo & session_info, acs_apbm_snmpsessioncallback & callback) { return open_(session_info, &callback); }

	int close ();

	inline int get_fd_set (int & fd_count, fd_set & set) const { int dummy = 0; return get_fd_set(fd_count, dummy, set); }
	int get_fd_set (int & fd_count, int & fd_sup, fd_set & set) const;

	int read () const;
	inline int read (fd_set & set) const {
		return snmp_sess_read(_session_handle, &set) ? acs_apbm_snmp::ERR_SNMP_SESSION_READ : acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	}

	int send_synch (netsnmp_pdu * & request, const unsigned * timeout_ms) const;

private:
	int open_ (acs_apbm_snmpsessioninfo & session_info, acs_apbm_snmpsessioncallback * callback = 0);

	static int session_callback_dispatcher (int operation, netsnmp_session * netsnmp_session, int request_id, netsnmp_pdu * netsnmp_pdu, void * session_callback);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	void * _session_handle;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_snmpsessionhandler
