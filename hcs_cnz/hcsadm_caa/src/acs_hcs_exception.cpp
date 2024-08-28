//======================================================================
//
// NAME
//      Exception.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-05-30 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <sstream>

#include "acs_hcs_exception.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Friends
	//================================================================================

	ostream& operator<<(ostream& os, const Exception& ex)
	{
		stringstream ss;
		ss << ex.info();

		return os << ss.str();
	}

	//================================================================================
	// Class Exception
	//================================================================================

	Exception::Exception(const string& where, const string& what, const string& cause)
		: exception(/*what.c_str()*/)
		, _where   (where)
		, _cause   (cause)
	{
		//DEBUG("%s","entering Exception's constructor ");
		if (*_where.rend() != '.')
			_where += ".";

		if (!_cause.empty() && *_cause.rend() != '.')
			_cause += ".";
		//DEBUG("%s","Leaving exception's constructor ");
	}

	Exception::~Exception() throw ()
	{
		//DEBUG("%s","In Exception's destructor ");
	}

	const string& Exception::where() const
	{
		//DEBUG("%s","In Exception::where() ");
		return _where;
	}

	const string& Exception::cause() const
	{
		//DEBUG("%s","In Exception::cause() ");
		return _cause;
	}

	string Exception::info() const
	{
		//DEBUG("%s","Entering Exception::info() ");
		stringstream s;
		s << this->where() << ": " << this->what() << (this->cause().empty() ? "" : " Cause: ") << this->cause();
		//DEBUG("%s","Leaving Exception::info() ");
		return s.str();
	}
}
