//======================================================================
//
// NAME
//      Exception.h
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
//      Implementation of the basic template for exception handling in HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Exception_h
#define AcsHcs_Exception_h

#include <iostream>
#include <sstream>
#include <string>

#include "acs_hcs_tra.h"
namespace AcsHcs
{
	/**
	* Exception - Base class of exceptions.
	* Exception is the base class of all exceptions
	* thrown by all classes in namespace AcsHcs.
	*/
	class Exception : public std::exception
	{
		friend std::ostream& operator<<(std::ostream& os, const Exception& ex);

	public:
		const std::string& where() const;
		const std::string& cause() const;
		std::string        info () const;

	protected:
		Exception(const std::string& where, const std::string& what, const std::string& cause);
		Exception(const std::string& where, const std::string& what, const Exception& cause);
		virtual ~Exception() throw();

		std::string _where;
		std::string _cause;
	};
}

#define ACS_HCS_THROW(exception,where,what,cause) { std::stringstream swhat; swhat << what; std::stringstream scause; scause << cause; throw exception(where, swhat.str(), scause.str()); }

#endif // AcsHcs_Exception_h
