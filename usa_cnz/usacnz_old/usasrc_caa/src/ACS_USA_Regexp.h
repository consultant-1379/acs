//*****************************************************************************
// 
// .NAME
//  	ACS_USA_Regexp - Regular expression handling
// .LIBRARY 3C++
// .PAGENAME ACS_USA_Regexp
// .HEADER  TMOS/ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE ACS_USA_Regexp.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1998-1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	ACS_USA_Regexp class is intended for matching character strings
//	against regular expressions. This class has to be constructed
//	by passing a regular expression pattern. After successful
//	different oveloaded methods are available to perform different
//	of matching.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0259

// AUTHOR  
// 	1998 by UAB/I/GM UABDMT

// .REVISION
//	PA1	981022

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO	DATE 	NAME	DESCRIPTION
//	PA1		981022	UABDMT	First Revision
//	PA2		040429	QVINKAL	  Removal of RougeWave Tools.h++

// .LINKAGE
//	

// .SEE ALSO 
// 	

//******************************************************************************

#ifndef ACS_USA_REGEXP_H 
#define ACS_USA_REGEXP_H
#include <string>
#include "ACS_USA_ATLRegExp.h"
#include "ACS_USA_Global.h"


using namespace std;
typedef string String;

//******************************************************************************
// Type definitions local to this class
//******************************************************************************
typedef	unsigned Regexp_MatchType;		// Type returned from Match

typedef Regexp CompiledExpression;	// Type returned from Match

typedef	int  ExpressionLength;			// Type returned from Match
typedef int  RegexpSystemError;			// Type for regular expr errors 
typedef int  Status;					// Type for status


//******************************************************************************
// Constants used in this class
//******************************************************************************
// Return values from matching routines

const	Regexp_MatchType	ACS_USA_Regexp_error	= 0;
const	Regexp_MatchType	ACS_USA_Regexp_noMatch	= 1;
const	Regexp_MatchType	ACS_USA_Regexp_match	= 2;

const	Status				Status_compiled			= 0;
const	Status				Status_notCompiled		= 1;

// Regular expression errors after compilation
//
const char* const Regex_prefix		= "regexpr";
const char* const Regex_failed		= "compile failed. Error code: ";
const char* const Regexp_BadState	= "ACS_USA_Regexp object in bad state";

// Forward declarations
//


//******************************************************************************
// Member functions, constructors, destructors, operators
//******************************************************************************

class  ACS_USA_Regexp
{
 public:

  ACS_USA_Regexp();
  // Description:
  // 	default constructor. Initialises the object data members
  // Parameters: 
  //    none 
  // Return value: 
  //    none 

  ACS_USA_Regexp(const ACS_USA_Regexp& other);
  // Description:
  // 	Copy constructor. Initialises the object with data from other 
  // Parameters: 
  //    other			Reference to an existing Regexp object, in
  // Return value: 
  //    none 

  virtual ~ACS_USA_Regexp();
  // Description:
  // 	Destructor.		Releases all allocated memory
  // Parameters: 
  //    none 
  // Return value: 
  //    none 
  
  ACS_USA_StatusType compilePattern(const String& pattern);
  // Description:
  // 	Compiles the regular expression pattern
  // Parameters: 
  //    pattern			RE pattern, in
  // Return value: 
  //    ACS_USA_error	Something went wrong
  //	ACS_USA_ok		OK 

  Regexp_MatchType checkMatch(const ACS_USA_RecordType record) const;
  // Description:
  // 	Matches the input string against RE 
  // Parameters: 
  //    string			character string to match, in
  // Return value: 
  //	ACS_USA_Regexp_error		error 
  //    ACS_USA_Regexp_noMatch 		didn't match 
  //    ACS_USA_Regexp_match 		matched 

  Regexp_MatchType checkMatch(
			const ACS_USA_RecordType record, 
			ACS_USA_ByteCount* unMatched) const;
  // Description:
  // 	Matches the input string against RE 
  // Parameters: 
  //    string			character string to match, in
  //	unMatched		position of first char that didn't match (after the match), out
  // Return value: 
  //	ACS_USA_Regexp_error		error 
  //    ACS_USA_Regexp_noMatch 		didn't match 
  //    ACS_USA_Regexp_match 		matched 

  Regexp_MatchType checkMatch(
			const ACS_USA_RecordType record, 
			ACS_USA_ByteCount* begin, 
			ACS_USA_ByteCount* extent) const;
  // Description:
  // 	Matches the input string against RE 
  // Parameters: 
  //    record			character string to match, in
  //	begin			position of the first matched char
  //	extent			extent of the match
  // Return value: 
  //	ACS_USA_Regexp_error		error 
  //    ACS_USA_Regexp_NoMatch 		didn't match 
  //    ACS_USA_Regexp_Match 		matched 

  const ACS_USA_Regexp& operator=(const ACS_USA_Regexp& other);
  // Description:
  // 	Assignment operator 
  // Parameters: 
  //    other			reference to an existing Regexp object, in
  // Return value: 
  //    none 

private:
	
	CompiledExpression* compiled; 	// ptr to compiled expression (RWCRExpr)
	Status		status;		        // Status
};


#endif

