//========================================================================================
/** @file  acs_usa_error.cpp
 *	@brief
 *	@author
 *	@date 
 *	@version 
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *
 *
 * SEE ALSO
 *		-
 *
 */
//========================================================================================

#include "acs_usa_error.h"
#include "acs_usa_errorimpl.h"

//******************************************************************************
// Problem texts in USA
//******************************************************************************

const char* problemTable[] = {
        "",
        "ERROR IN acs_usa_sysloganalyser",
        "I/O ERROR",
        "SYSTEM CALL FAILED",
        "BAD OBJECT STATE",
        "OUT OF MEMORY",
        "INVALID TIME STAMP",
        "INVALID TIME STAMP FORMAT",
        "CANNOT EXTRACT TIME",
        "POSSIBLY INVALID TIME FORMAT",
        "ALARM QUEUE FULL",
        "CANNOT DELIVER EVENTS",
        "EVENT STRING ERROR",
        "SERVICE EXIT",
        "TOO HIGH EVENT FREQUENCY"
};

//========================================================================================
//	Constructor
//========================================================================================

acs_usa_error::acs_usa_error()
{
	errorImplp_ = new ACS_USA::acs_usa_errorimpl;

}


//========================================================================================
//	Copy constructor
//========================================================================================

acs_usa_error::acs_usa_error(const acs_usa_error& error)
{

	errorImplp_ = new ACS_USA::acs_usa_errorimpl(*error.errorImplp_);

}

//========================================================================================
//	Destructor
//========================================================================================

acs_usa_error::~acs_usa_error()
{
	delete errorImplp_;
}


//========================================================================================
//	Assignment operator
//========================================================================================

acs_usa_error& acs_usa_error::operator=(const acs_usa_error& error)
{
	*errorImplp_ = *error.errorImplp_;
	return *this;
}


//========================================================================================
//	Set error
//========================================================================================

void acs_usa_error::setError(const ACS_USA_ErrorType id)
{
	errorImplp_->setError(id);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_error::setErrorText(const char prefix[],const char message[],const char suffix[])
{
	errorImplp_->setErrorText(prefix, message, suffix);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_error::setErrorText(const char prefix[],const char message[],const long value)
{
	errorImplp_->setErrorText(prefix, message, value);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_error::setErrorText(const char message[])
{
	errorImplp_->setErrorText(message);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_error::setErrorText(const char prefix[], const char message[])
{
	errorImplp_->setErrorText(prefix, message);
}


//========================================================================================
//	Get error
//========================================================================================

ACS_USA_ErrorType acs_usa_error::getError() const
{
	return errorImplp_->getError();
}


//========================================================================================
//	Get error text
//========================================================================================

const char* acs_usa_error::getErrorText() const
{
	return errorImplp_->getErrorText();
}  

//******************************************************************************
//      getProblemText()
//******************************************************************************
const char*
acs_usa_error::getProblemText(const ACS_USA_ErrorType idx) const
{
    return problemTable[idx];
}


