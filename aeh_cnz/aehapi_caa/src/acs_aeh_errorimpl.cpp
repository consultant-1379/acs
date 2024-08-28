//========================================================================================
/** @file  acs_aeh_errorimpl.cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-05
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
 *	| P0.9.1 | 2011-05-05 | xfabron      | File created.                       |
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

#include "acs_aeh_errorimpl.h"

#include <sstream>

using namespace std;

namespace ACS_AEH {

const char acs_aeh_errorimpl::dash_[]  = " - ";		// Used to compose error string
const char acs_aeh_errorimpl::colon_[] = " : ";		// Used to compose error string
const char acs_aeh_errorimpl::prefix_[] = "ACS_AEH_EvReport";
											// Used to specify where the error occurred.

//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_errorimpl::acs_aeh_errorimpl() : errorId_(ACS_AEH_noErrorType), errorText_(ACS_AEH_noError)
{
}


//========================================================================================
//	Destructor
//========================================================================================

acs_aeh_errorimpl::~acs_aeh_errorimpl()
{
}


//========================================================================================
//	Set error
//========================================================================================

void acs_aeh_errorimpl::setError(const ACS_AEH_ErrorType id)
{
	errorId_ = id;				// Set the errorId
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_errorimpl::setErrorText(const char prefix[],const char message[],const char suffix[])
{
	ostringstream ostr;
	ostr << prefix << colon_ << message << dash_ << suffix;
	errorText_ = ostr.str ();
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_errorimpl::setErrorText(const char prefix[],const char message[],const long value)
{
	ostringstream ostr;
	ostr << prefix << colon_ << message << dash_ << value;
	errorText_ = ostr.str ();
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_errorimpl::setErrorText(const char message[])
{
	errorText_ = message;
}


//========================================================================================
//	Set error text
//========================================================================================

void acs_aeh_errorimpl::setErrorText(const char prefix[], const char message[])
{
	ostringstream ostr;
	ostr << prefix << colon_ << message;
	errorText_ = ostr.str ();
}


//========================================================================================
//	Get error
//========================================================================================

ACS_AEH_ErrorType acs_aeh_errorimpl::getError() const
{
	return errorId_;				// Get the error type
}


//========================================================================================
//	Get error text
//========================================================================================

const char* acs_aeh_errorimpl::getErrorText() const
{
	return errorText_.data();	 	// Get the error text string
}

}
