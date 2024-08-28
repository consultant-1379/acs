//========================================================================================
/** @file  acs_aeh_exception..cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-23
 *	@version 1.0.1
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
 *	| P1.0.1 | 2011-05-23 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-06-08 | xfabron      | Released for ITSTEAM2 sprint 11     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 *
 * SEE ALSO
 * -
 *
 */
//========================================================================================


#include <stdio.h>
#include <errno.h>
#include "acs_aeh_exception.h"


//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_exception::acs_aeh_exception(errorType p_error) : error_(p_error), s_()
{
	lerrno = 0;
}


//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_exception::acs_aeh_exception(errorType p_error, int p_lerrno) : error_(p_error), s_()
{
	lerrno = p_lerrno;
}


//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_exception::acs_aeh_exception(errorType p_error, std::string p_detailInfo, int p_lerrno) : error_(p_error), s_()
{
	s_ << p_detailInfo;
	lerrno = p_lerrno;
}


//========================================================================================
//	Copy constructor
//========================================================================================

acs_aeh_exception::acs_aeh_exception(const acs_aeh_exception& ex) : error_(ex.error_), s_()
{
	lerrno = ex.lerrno;
	s_ << ex.s_.str();
}


//========================================================================================
//	Destructor
//========================================================================================

acs_aeh_exception::~acs_aeh_exception()
{
}


//========================================================================================
//	Get error code
//========================================================================================

errorType acs_aeh_exception::getErrorCode() const
{
	return error_;
}


//========================================================================================
//	Get Linux error number code
//========================================================================================

int acs_aeh_exception::getResultCode() const
{
	return lerrno;
}


//========================================================================================
//	Get error text
//========================================================================================

const std::string acs_aeh_exception::errorText() const
{
	std::string text;

	switch (error_)
	{
    case GENERAL_FAULT:
    	text = GENERAL_FAULT_STR;
    	break;
    case INCORRECT_USAGE:
    	text = INCORRECT_USAGE_STR;
    	break;
    case PARAMERROR:
    	text = PARAMERROR_STR;
    	break;
    case SYSTEMERROR:
    	text = SYSTEMERROR_STR;
    	break;
    case INTERNALERROR:
    	text = INTERNALERROR_STR;
    	break;
    default:
    	text = "***";
	}
	return text;

}


//========================================================================================
//	Get detailed info about the error
//========================================================================================

const std::string acs_aeh_exception::detailInfo() const
{
	return s_.str();
}


//========================================================================================
//	Stream detailed information
//========================================================================================

std::stringstream& acs_aeh_exception::operator()()
{
	return s_;
}


//=============================================================================
//	strError ()
//=============================================================================

std::string acs_aeh_exception::strError(int p_lerrno) const
{
	// Routine to translate an Linux error number into a fault string

	std::string test = strerror(p_lerrno);

	return test;
}


//========================================================================================
//	Send error message to the output stream
//========================================================================================

std::ostream& operator<<(std::ostream& s, const acs_aeh_exception& ex)
{
	s.clear();
	s << ex.errorText();
	std::string str = ex.detailInfo();
	if (str.empty() == false)
	{
		s << ": " << str;
	}
	if (ex.lerrno != 0)
	{
		s << std::endl;
		s << ex.strError(ex.lerrno);
	}
	return s;
}
