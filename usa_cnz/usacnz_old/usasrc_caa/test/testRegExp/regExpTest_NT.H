	

//******************************************************************************

#ifdef _WIN32  // NT

#ifndef ACS_USA_Regexp_H 
#define ACS_USA_Regexp_H

#include "ACS_USA_Global.H"


//class RWCRegexp; // Forward declaration 

//******************************************************************************
// Type definitions local to this class
//******************************************************************************
typedef	unsigned Regexp_MatchType;	 // Type returned from Match

// On NT RWCRegexp is used for regexp (on Unix: char)
typedef RWCRegexp CompiledExpression; // Type returned from Match
typedef	int  ExpressionLength;		 // Type returned from Match
typedef int  RegexpSystemError;		 // Type for regular expr errors 
typedef int  Status;			     // Type for status


//******************************************************************************
// Constants used in this class
//******************************************************************************
// Return values from matching routines

const	Regexp_MatchType	ACS_USA_Regexp_error   = 0;
const	Regexp_MatchType	ACS_USA_Regexp_noMatch = 1;
const	Regexp_MatchType	ACS_USA_Regexp_match   = 2;

const	Status	Status_compiled = 0;
const	Status	Status_notCompiled = 1;

// Regular expression errors after compilation
const char* const Regex_prefix = "regexpr";
const char* const Regex_failed = "compile failed. Error code: ";
const char* const Regexp_BadState = "ACS_USA_Regexp object in bad state";

// Forward declarations
class RWCString;

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
  //    other			reference to an existing Regexp object, in
  // Return value: 
  //    none 

  virtual ~ACS_USA_Regexp();
  // Description:
  // 	Destructor. Releases all allocated memory
  // Parameters: 
  //    none 
  // Return value: 
  //    none 
  
  ACS_USA_StatusType compilePattern(const RWCString& pattern);
  // Description:
  // 	Compiles the regular expression pattern
  // Parameters: 
  //    pattern			RE pattern, in
  // Return value: 
  //    ACS_USA_error		Something went wrong
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
	
	CompiledExpression* compiled; 	// ptr to compiled expression (RWCRegexp)
	Status		status;		        // Status
};


#endif
#endif  // _WIN32

