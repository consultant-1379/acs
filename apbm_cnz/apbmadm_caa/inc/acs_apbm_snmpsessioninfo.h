#ifndef HEADER_GUARD_CLASS__acs_apbm_snmpsessioninfo
#define HEADER_GUARD_CLASS__acs_apbm_snmpsessioninfo acs_apbm_snmpsessioninfo

/** @file acs_apbm_snmpsessioninfo.h
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

#include <stdlib.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/transform_oids.h> // To get using SNMP Version 3


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_snmpsessioninfo


/** @class acs_apbm_snmpsessioninfo acs_apbm_snmpsessioninfo.h
 *	@brief acs_apbm_snmpsessioninfo class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-09-20
 *
 *	acs_apbm_snmpsessioninfo <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : protected snmp_session {
	//=========//
	// Friends //
	//=========//
	friend class acs_apbm_snmpsessionhandler;


	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_snmpsessioninfo constructor
	 */
	inline __CLASS_NAME__ () : snmp_session(), _last_snmp_error_text(0) { snmp_sess_init(this); }

	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : snmp_session(rhs), _last_snmp_error_text(0) {
		if (rhs._last_snmp_error_text) _last_snmp_error_text = ::strdup(rhs._last_snmp_error_text);
	}


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_snmpsessioninfo Destructor
	 */
	inline ~__CLASS_NAME__ () { ::free(const_cast<char *>(_last_snmp_error_text)); }


	//================//
	// Field accessor //
	//================//
public:
	inline long version () const { return snmp_session::version; }
	inline void version (long new_value) { snmp_session::version = new_value; }

	inline const char * peername () const { return snmp_session::peername; }
	void peername (char * new_value);

	inline const char * community () const { return reinterpret_cast<char *>(snmp_session::community); }
	void community (char * new_value);

	inline u_short local_port () const { return snmp_session::local_port; }
	inline void local_port (u_short new_value) { snmp_session::local_port = new_value; }

	inline int last_sys_error () const { return snmp_session::s_errno; }
	inline int last_snmp_error () const { return snmp_session::s_snmp_errno; }
	inline const char * last_snmp_error_text () const { return _last_snmp_error_text ?: "OK"; }
	inline void reset_errors () { snmp_session::s_errno = 0; snmp_session::s_snmp_errno = 0; last_snmp_error_text(0); }

protected:
	inline void last_snmp_error_text (const char * new_ptr) { ::free(const_cast<char *>(_last_snmp_error_text)); _last_snmp_error_text = new_ptr; }
	inline void last_snmp_error_text_dup (const char * str_to_copy) {
		::free(const_cast<char *>(_last_snmp_error_text));
		_last_snmp_error_text = (str_to_copy ? ::strdup(str_to_copy) : 0);
	}


	//============//
	// Functions  //
	//============//
public:


	//===========//
	// Operators //
	//===========//
public:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		if (this != &rhs) {
			last_snmp_error_text_dup(rhs._last_snmp_error_text);
			snmp_session::operator=(rhs);
		}
		return *this;
	}


	//========//
	// Fields //
	//========//
private:
	const char * _last_snmp_error_text;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_snmpsessioninfo
