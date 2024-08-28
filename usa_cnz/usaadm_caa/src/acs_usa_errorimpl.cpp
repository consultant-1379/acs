//========================================================================================
/** @file  acs_usa_errorimpl.cpp
 *	@brief
 *	@author
 *	@date 
 *	@version 
 *
 *	COPYRIGHT Ericsson AB, 2012
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
 *
 * SEE ALSO
 *		-
 */
//========================================================================================

#include "acs_usa_errorimpl.h"

#include <sstream>

using namespace std;

namespace ACS_USA {

const char acs_usa_errorimpl::dash_[]  = " - ";		// Used to compose error string
const char acs_usa_errorimpl::colon_[] = " : ";		// Used to compose error string
const char acs_usa_errorimpl::prefix_[] = "ACS_USA_EvReport"; // Used to specify where the error occurred.

//========================================================================================
//	Constructor
//========================================================================================

acs_usa_errorimpl::acs_usa_errorimpl() : errorId_(ACS_USA_NoError), errorText_("No Error")
{
}


//========================================================================================
//	Destructor
//========================================================================================

acs_usa_errorimpl::~acs_usa_errorimpl()
{
}


//========================================================================================
//	Set error
//========================================================================================

void acs_usa_errorimpl::setError(const ACS_USA_ErrorType id)
{
	errorId_ = id;				// Set the errorId
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_errorimpl::setErrorText(const char prefix[],const char message[],const char suffix[])
{
	ostringstream ostr;
	ostr << prefix << colon_ << message << dash_ << suffix;
	errorText_ = ostr.str ();
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_errorimpl::setErrorText(const char prefix[],const char message[],const long value)
{
	ostringstream ostr;
	ostr << prefix << colon_ << message << dash_ << value;
	errorText_ = ostr.str ();
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_errorimpl::setErrorText(const char message[])
{
	errorText_ = message;
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_usa_errorimpl::setErrorText(const char prefix[], const char message[])
{
	ostringstream ostr;
	ostr << prefix << colon_ << message;
	errorText_ = ostr.str ();
}


//========================================================================================
//	Get error
//========================================================================================

ACS_USA_ErrorType acs_usa_errorimpl::getError() const
{
	return errorId_;				// Get the error type
}


//========================================================================================
//	Get error text
//========================================================================================

const char* acs_usa_errorimpl::getErrorText() const
{
	return errorText_.data();	 	// Get the error text string
}

};

