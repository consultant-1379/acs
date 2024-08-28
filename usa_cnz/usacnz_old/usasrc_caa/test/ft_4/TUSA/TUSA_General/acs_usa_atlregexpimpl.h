////////////////////////////////////////////////////////////////////////////
// NAME - 
//
// COPYRIGHT Ericsson AB, Sweden 2004
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden. 
// The program(s) may be used and/or copied only with the written 
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//		Regular Expression Implementation using ATL classes. 
//
//
// DOCUMENT NO
//	----
//
// AUTHOR
//	2004-04-13  UAB/UZ/DH   UABTSO 
//
// CHANGES
//	20040413	UABTSO		First Release
//
////////////////////////////////////////////////////////////////////////////
#ifndef REGEXP_IMPL_H
#define REGEXP_IMPL_H

#include <atlrx.h>
#include <string>
#include "ACS_USA_ATlRegExp.h"

//using namespace ACS_USA;

class Regexp_impl 
{
public:

	Regexp_impl();
	// Description:
	//		Default constructor. You must assign a pattern to the regular 
	//		expression before you use it.
	// Parameters:
	// 		None
	
	Regexp_impl(const char* pat);
	Regexp_impl(const std::string& pat);
	// Description:
	//		Construct a regular expression from the pattern given by pat. 
	//		The status of the results can be found by using member  
	//		function status().
	// Parameters:
	// 		pat				String pattern
	
	Regexp_impl(const Regexp_impl& reg);
	// Description:
	//		Copy constructor. Uses value semantics -- self will be a copy of r.
	// Parameters:
	// 		reg				Regexp_impl object
	// Additional information:
	//		None
	
	~Regexp_impl();
	// Description:
	//		Destructor. Releases any allocated memory.
		
	Regexp_impl& operator=(const Regexp_impl& reg);
	// Description:
	//		Assignment operator. Recompiles self to pattern found in reg.
	// Parameters:
	// 		reg				Regexp_impl object
	// Return value
	//		Reference to self
	
	Regexp_impl& operator=(const char* pat);
	Regexp_impl& operator=(const std::string& pat);
	// Description:
	//		Recompiles self to the pattern given by pat. The status of 
	//		the results can be found by using member function status().
	// Parameters:
	// 		pat				String pattern
	// Return value
	//		Reference to self
	
	size_t index(const std::string& str, size_t* len, size_t start) const;
	// Description:
	//		Match the string towards the pattern. If an invalid regular 
	//		expression is used for the search, assertion will fail.
	// Parameters:
	// 		str				String to match
	//		len				Length of the matching pattern
	//		start			Start index for the search
	// Return value
	//		Index of the first instance in the string that
	//		matches the regular expression. If no match was 
	//		found str::string ::npos is returned.
	
	Regexp::statusType status() const;
	
private:
		
	mutable CAtlRegExp<> rex_;
	REParseError err_;
	const char* pat_;
};

#endif
