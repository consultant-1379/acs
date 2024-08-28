#ifndef HEADER_GUARD_CLASS__ACS_DSD_Communicator
#define HEADER_GUARD_CLASS__ACS_DSD_Communicator ACS_DSD_Communicator

/** @file ACS_DSD_Communicator.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
 *	@version 1.0.0
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.0 | 2010-09-28 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <cstring>

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_Trace.h"

class ACS_DSD_TraTracer;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Communicator

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_Communicator ACS_DSD_Communicator.h
 *	@brief ACS_DSD_Communicator class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
 *	@version 1.0.0
 *
 *	ACS_DSD_Communicator Class detailed description
 */
class __CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
protected:
	enum {
		LAST_ERROR_TEXT_SIZE	=	512
	};

	//==============//
	// Constructors //
	//==============//
protected:
	/** @brief ACS_DSD_Communicator Default constructor
	 *
	 *	ACS_DSD_Communicator Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : _last_error(0), _last_error_text() {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		set_error_info(_last_error);
	}

	/** @brief ACS_DSD_Communicator Copy constructor
	 *
	 *	ACS_DSD_Communicator Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : _last_error(rhs._last_error), _last_error_text() {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		memccpy(_last_error_text, rhs._last_error_text, 0, LAST_ERROR_TEXT_SIZE);
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Communicator Destructor
	 *
	 *	ACS_DSD_Communicator Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief get_handles method
	 *
	 *	get_handles method detailed description
	 *
	 *	@param[out] handles Description
	 *	@param[in,out] handle_count Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline virtual int get_handles (acs_dsd::HANDLE * /*handles*/, int & handle_count) const = 0;

	/** @brief get_local_node method
	 *
	 *	get_local_node method detailed description
	 *
	 *	@param[out] node Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_local_node (ACS_DSD_Node & node) const;

	/** @brief get_partner_node method
	 *
	 *	get_partner_node method detailed description
	 *
	 *	@param[out] node Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_partner_node (ACS_DSD_Node & node) const;

	/** @brief last_error method
	 *
	 *	last_error method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int last_error () const { return _last_error; }

	/** @brief last_error_text method
	 *
	 *	last_error_text method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline const char * last_error_text () const { return _last_error_text; }

protected:
	int set_error_info (int error_code) const;
	int set_error_info (int error_code, int system_error) const { return set_error_info(error_code, system_error, 0); }
	int set_error_info (int error_code, int system_error, const char * format, ...) const;
	int get_node_state (acs_dsd::SystemIDConstants system_id, acs_dsd::NodeStateConstants & node_state) const;

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ &) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	mutable int _last_error;
	mutable char _last_error_text[LAST_ERROR_TEXT_SIZE];
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Communicator
