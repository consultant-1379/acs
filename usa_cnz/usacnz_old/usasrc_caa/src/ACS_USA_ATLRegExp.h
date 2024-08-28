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
//		Regular Expression class (wrapper around ATL class). 
//		Tokenizer class
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

#ifndef REGEXP_H
#define REGEXP_H

#include <string>

using namespace std;
class Regexp_impl;



/********************Class definition : Begin **********************/
class Regexp 
{
public:

	 enum statusType{
		OK = 0,
		OUTOFMEMORY,
		BRACE_EXPECTED,
		PAREN_EXPECTED,
		BRACKET_EXPECTED,
		UNEXPECTED,
		EMPTY_RANGE,
		INVALID_GROUP,
		INVALID_RANGE,
		EMPTY_REPEATOP,
		INVALID_INPUT
	};

	Regexp();
	// Description:
	//		Default constructor. You must assign a pattern to the regular 
	//		expression before you use it.
	// Parameters:
	// 		None
	
	Regexp(const char* pat);
	Regexp(const std::string& pat);
	// Description:
	//		Construct a regular expression from the pattern given by pat. 
	//		The status of the results can be found by using member  
	//		function status().
	// Parameters:
	// 		pat				String pattern
	
	Regexp(const Regexp& reg);
	// Description:
	//		Copy constructor. Uses value semantics -- self will be a copy of reg.
	// Parameters:
	// 		reg				Regexp object
	// Additional information:
	//		None
	
	~Regexp();
	// Description:
	//		Destructor. Releases any allocated memory.
		
	Regexp& operator=(const Regexp& reg);
	// Description:
	//		Assignment operator. Recompiles self to pattern found in reg.
	// Parameters:
	// 		reg				Regexp object
	// Return value
	//		Reference to self
	
	Regexp& operator=(const char* pat);
	Regexp& operator=(const std::string& pat);
	// Description:
	//		Recompiles self to the pattern given by pat. The status of 
	//		the results can be found by using member function status().
	// Parameters:
	// 		pat				String pattern
	// Return value
	//		Reference to self
	
	size_t index(const std::string& str, size_t* len = NULL, size_t start = 0) const;
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
	
	statusType status() const;
	
private:

	Regexp_impl* impl_;
};
/********************Class definition : End **********************/



#endif
