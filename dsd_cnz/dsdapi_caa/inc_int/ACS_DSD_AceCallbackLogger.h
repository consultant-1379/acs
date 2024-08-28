#ifndef HEADER_GUARD_CLASS__ACS_DSD_AceCallbackLogger
#define HEADER_GUARD_CLASS__ACS_DSD_AceCallbackLogger ACS_DSD_AceCallbackLogger

/** @file ACS_DSD_AceCallbackLogger.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
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
 *	| P1.0.1 | 2010-11-09 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ace/Log_Msg_Callback.h"

#include "ACS_DSD_Macros.h"

#include "ACS_DSD_Logger.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_AceCallbackLogger

/** @class ACS_DSD_AceCallbackLogger ACS_DSD_AceCallbackLogger.h
 *	@brief ACS_DSD_AceCallbackLogger class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_AceCallbackLogger Class detailed description
 */
class __CLASS_NAME__ : public ACS_DSD_Logger, public ACE_Log_Msg_Callback {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_AceCallbackLogger Default constructor
	 *
	 *	ACS_DSD_AceCallbackLogger Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	explicit inline __CLASS_NAME__ (bool verbose_logging = false) : ACS_DSD_Logger(), ACE_Log_Msg_Callback(), _verbose_logging(verbose_logging) {}

private:
	/** @brief ACS_DSD_AceCallbackLogger Copy constructor
	 *
	 *	ACS_DSD_AceCallbackLogger Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACS_DSD_Logger(rhs), ACE_Log_Msg_Callback(rhs), _verbose_logging(rhs._verbose_logging) {}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_AceCallbackLogger Destructor
	 *
	 *	ACS_DSD_AceCallbackLogger Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	virtual void log (ACE_Log_Record & log_record);

	inline bool verbose () const { return _verbose_logging; }
	inline void verbose (bool be_verbose) { _verbose_logging = be_verbose; }

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_Logger::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	bool _verbose_logging;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_AceCallbackLogger
