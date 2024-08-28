//========================================================================================
/** @file  acs_aeh_error.cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-04
 *	@version 0.9.1
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
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.1 | 2011-05-04 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2011-05-13 | xfabron      | Released for ITSTEAM2 sprint 11     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 *
 * SEE ALSO
 *		-
 *
 */
//========================================================================================

#include "acs_aeh_error.h"
#include "acs_aeh_errorimpl.h"


//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_error::acs_aeh_error()
{
	errorImplp_ = new ACS_AEH::acs_aeh_errorimpl;

}


//========================================================================================
//	Copy constructor
//========================================================================================

acs_aeh_error::acs_aeh_error(const acs_aeh_error& error)
{

	errorImplp_ = new ACS_AEH::acs_aeh_errorimpl(*error.errorImplp_);

}


//========================================================================================
//	Constructor for initializing the event report implementation pointer
//========================================================================================

/*acs_aeh_error::acs_aeh_error(ACS_AEH::acs_aeh_evreportimpl* evReportImplp)
{
	errorImplp_=new ACS_AEH::acs_aeh_errorimpl;
	evReportImplp_=evReportImplp;
	dummy_=0;
}
*/

//========================================================================================
//	Destructor
//========================================================================================

acs_aeh_error::~acs_aeh_error()
{
	delete errorImplp_;
}


//========================================================================================
//	Assignment operator
//========================================================================================

acs_aeh_error& acs_aeh_error::operator=(const acs_aeh_error& error)
{
	*errorImplp_ = *error.errorImplp_;
	return *this;
}


//========================================================================================
//	Set error
//========================================================================================

void acs_aeh_error::setError(const ACS_AEH_ErrorType id)
{
	errorImplp_->setError(id);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_error::setErrorText(const char prefix[],const char message[],const char suffix[])
{
	errorImplp_->setErrorText(prefix, message, suffix);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_error::setErrorText(const char prefix[],const char message[],const long value)
{
	errorImplp_->setErrorText(prefix, message, value);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_error::setErrorText(const char message[])
{
	errorImplp_->setErrorText(message);
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_error::setErrorText(const char prefix[], const char message[])
{
	errorImplp_->setErrorText(prefix, message);
}


//========================================================================================
//	Get error
//========================================================================================

ACS_AEH_ErrorType acs_aeh_error::getError() const
{
	return errorImplp_->getError();
}


//========================================================================================
//	Get error text
//========================================================================================

const char* acs_aeh_error::getErrorText() const
{
	return errorImplp_->getErrorText();
}  
