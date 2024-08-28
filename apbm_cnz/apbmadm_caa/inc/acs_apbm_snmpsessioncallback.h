#ifndef HEADER_GUARD_CLASS__acs_apbm_snmpsessioncallback
#define HEADER_GUARD_CLASS__acs_apbm_snmpsessioncallback acs_apbm_snmpsessioncallback

/** @file acs_apbm_snmpsessioncallback.h
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

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/transform_oids.h> // To get using SNMP Version 3

#include "acs_apbm_snmpconstants.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_snmpsessioncallback


/** @class acs_apbm_snmpsessioncallback acs_apbm_snmpsessioncallback.h
 *	@brief acs_apbm_snmpsessioncallback class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-09-20
 *
 *	acs_apbm_snmpsessioncallback <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__ () : error_code(acs_apbm_snmp::ERR_SNMP_NO_ERRORS), pdu_error_stat(acs_apbm_snmp::ERR_SNMP_NO_ERRORS),
	board_slot(-1),  is_cba(0), _board_magazine_str(0), _switch_board_ip(0){ board_fbn=-1; is_4led_supported=false; is_new_gea_supported=false; }

	inline __CLASS_NAME__ (const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_, uint16_t board_fbn_=-1, bool is_4led_supported_=false, bool is_new_gea_supported_=false)
	: error_code(acs_apbm_snmp::ERR_SNMP_NO_ERRORS), pdu_error_stat(acs_apbm_snmp::ERR_SNMP_NO_ERRORS),
	  board_slot(board_slot_), is_cba(is_cba_), board_fbn(board_fbn_), is_4led_supported(is_4led_supported_), is_new_gea_supported(is_new_gea_supported_), _board_magazine_str(board_magazine_str_), _switch_board_ip(switch_board_ip_) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_snmpsessioncallback Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	inline const char * board_magazine_str () const { return _board_magazine_str; }
	inline void board_magazine_str (const char * new_value) { _board_magazine_str = new_value; }

	inline const char * switch_board_ip () const { return _switch_board_ip; }
	inline void switch_board_ip (const char * new_value) { _switch_board_ip = new_value; }

protected:
	int check_pdu_operation (int operation) const;

	int check_pdu_error_stat (int error_stat, int error_stat_in_pdu) const;

	int chech_null_var_list (netsnmp_variable_list * var_list) const;

	int check_pdu_var_type (int var_type, int expected) const;

	int check_pdu_var_size (size_t var_size, size_t min_expected) const;

	int common_pdu_checks (int operation, int error_stat, int error_stat_in_pdu, netsnmp_variable_list * var_list) const;

	virtual int read_scbrp (netsnmp_variable_list * var_list) const;
	virtual int read_scx (netsnmp_variable_list * var_list) const;

	//===========//
	// Operators //
	//===========//
public:
  /*
   * int operation;
   * netsnmp_session *session;    The session authenticated under.
   * int reqid;                       The request id of this pdu (0 for TRAP)
   * netsnmp_pdu *pdu;        The pdu information.
   *
   * Returns 1 if request was successful, 0 if it should be kept pending.
   * Any data in the pdu must be copied because it will be freed elsewhere.
   */
//	virtual int operator() (int operation, netsnmp_session * netsnmp_session, int request_id, netsnmp_pdu * netsnmp_pdu) const = 0;
	virtual int operator() (int operation, netsnmp_session * netsnmp_session, int request_id, netsnmp_pdu * netsnmp_pdu) const;

private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
public:
	mutable int error_code;
	mutable int pdu_error_stat;

	int32_t board_slot;
	int is_cba;
	uint16_t board_fbn;
	bool is_4led_supported;
	bool is_new_gea_supported;

private:
	const char * _board_magazine_str;
	const char * _switch_board_ip;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_snmpsessioncallback
