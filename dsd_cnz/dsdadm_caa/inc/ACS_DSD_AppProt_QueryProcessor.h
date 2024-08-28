#ifndef HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryProcessor
#define HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryProcessor AppProt_QueryProcessor

/** @file ACS_DSD_AppProt_QueryProcessor.h
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

#include "ace/Task.h"
#include "ace/Reactor.h"

#include "ACS_DSD_Macros.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryProcessor

/** @class AppProt_QueryProcessor ACS_DSD_AppProt_QueryProcessor.h
 *	@brief AppProt_QueryProcessor class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
 *	@version 1.0.0
 *
 *	AppProt_QueryProcessor Class detailed description
 */
class __CLASS_NAME__ : public ACE_Task_Base {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief AppProt_QueryProcessor Default constructor
	 *
	 *	AppProt_QueryProcessor Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	explicit inline __CLASS_NAME__ (ACE_Reactor & reactor, uint32_t timeout_ms = 0xFFFFFFFFU)
	: ACE_Task_Base(), _reactor(reactor), _timeout_ms(timeout_ms) {}

private:
	/** @brief AppProt_QueryProcessor Copy constructor
	 *
	 *	AppProt_QueryProcessor Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACE_Task_Base(), _reactor(rhs._reactor), _timeout_ms(rhs._timeout_ms) {}

	//============//
	// Destructor //
	//============//
public:
	/** @brief AppProt_QueryProcessor Destructor
	 *
	 *	AppProt_QueryProcessor Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	/** @brief svc method
	 *
	 *	svc method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int svc ();

protected:
	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & /*rhs*/) {
		//this->ACE_Task_Base::operator=(rhs); //The assignment operator is private in ACS_Task_Base
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	ACE_Reactor & _reactor;
	uint32_t _timeout_ms;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryProcessor
