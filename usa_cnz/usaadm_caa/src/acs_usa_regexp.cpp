//******************************************************************************
//
// NAME
//      ACS_USA_Regexp.cpp
//
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1998-1999.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// DOCUMENT NO
// 190 89-CAA 109 0259

// AUTHOR  
// 	1998-10-30 by UAB/I/GM UABDMT

// REVISION
//	PA1	 	981030

// CHANGES

//
//	REV NO		DATE		NAME 		DESCRIPTION
//	PA1	 		981030		UABDMT		First Revision
//  PA2			040429		QVINKAL	  Removal of RougeWave Tools.h++
//******************************************************************************


#include <string.h>
#include <stdlib.h>
#include <string>

#include "acs_usa_atlRegexp.h"
#include "acs_usa_regexp.h"
#include "acs_usa_error.h"

using namespace std;
//typedef string String;

//******************************************************************************
//	ACS_USA_Regexp()
//******************************************************************************
ACS_USA_Regexp::ACS_USA_Regexp()
    : compiled(0),
      status(Status_notCompiled)  
{
}  

//******************************************************************************
//	ACS_USA_Regexp() - Copy constructor
//******************************************************************************
ACS_USA_Regexp::ACS_USA_Regexp(const ACS_USA_Regexp& other)
    : compiled(0),
      status(other.status)  
{
    // If the expression is compiled, make a copy of the regexp
	//
	if (status == Status_compiled) 
	{
		compiled =	new Regexp(*(other.compiled));
		if (compiled == NULL) 
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Regexp::ACS_USA_Regexp().");
		}
	}	
}

//******************************************************************************
//	~ACS_USA_Regexp()
//******************************************************************************
ACS_USA_Regexp::~ACS_USA_Regexp()
{
	if (compiled != NULL)
	{
		delete compiled;
		compiled = NULL;
	}
}


//******************************************************************************
//	compilePattern
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Regexp::compilePattern(const String& pattern)
{
    // "Compile" (construct) the regular expression from incoming pattern
	//
	Regexp* re = new Regexp(pattern.data());
	
	if (re == NULL) 
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Regexp::compilePattern(). Criterion:");
		return ACS_USA_Error;	
	}
	if (re->status() == Regexp::OK)
	{
		compiled = re;
	}
	else									// failure
    {		
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(Regex_prefix, Regex_failed, re->status());
		delete re;
		return ACS_USA_Error;
    }
    status = Status_compiled;
    return ACS_USA_Ok;
}  


//******************************************************************************
//	checkMatch()
//******************************************************************************
Regexp_MatchType
ACS_USA_Regexp::checkMatch(const ACS_USA_RecordType record) const
{
    if (status == Status_notCompiled)		// Something may be wrong
	{
		error.setError(ACS_USA_BadState);
		error.setErrorText(Regexp_BadState);
		return ACS_USA_Regexp_error;
    }
   
	String recordstr(record);
	try
	{ 
		// Check if the incoming string (record) matches the regexp
		//
		size_t tmp;
		if (compiled->index(recordstr,&tmp, (size_t)0) != string::npos)
		{
			return ACS_USA_Regexp_match;
		}
	}
	catch(...)								// Probably RWxmsg exception
	{			
		//error.setError(ACS_USA_SyntaxError);
		//error.setErrorText(Regex_prefix, Regex_failed, compiled->status());
		//return ACS_USA_Error;
		return ACS_USA_Regexp_noMatch;
	}

    return ACS_USA_Regexp_noMatch;
}


//******************************************************************************
//	checkMatch()
//******************************************************************************
Regexp_MatchType
ACS_USA_Regexp::checkMatch(
			const ACS_USA_RecordType record,
			ACS_USA_ByteCount* unMatched) const
{
    if (status == Status_notCompiled)		// Something may be wrong
	{
		error.setError(ACS_USA_BadState);
		error.setErrorText(Regexp_BadState);
		return ACS_USA_Regexp_error;
    }

	String recordstr (record);  	
	size_t matchlength;
	size_t matchstart;

	// Check if incoming record matches the regexp
	//
	matchstart = compiled->index(recordstr, &matchlength, (size_t)0);
	
	if (matchstart != string::npos)
	{
		// Length of match + start of match is 
		// first non-matching char after matched text
		//
		*unMatched = matchlength + matchstart; 

		return ACS_USA_Regexp_match;
    }
    return ACS_USA_Regexp_noMatch;
}


//******************************************************************************
//	checkMatch()
//******************************************************************************
Regexp_MatchType
ACS_USA_Regexp::checkMatch(
			const ACS_USA_RecordType record,
			ACS_USA_ByteCount* begin,
			ACS_USA_ByteCount* extent) const
{
    if (status == Status_notCompiled)		// Something may be wrong 
	{
		error.setError(ACS_USA_BadState);
		error.setErrorText(Regexp_BadState);
		return ACS_USA_Regexp_error;
    }

	String recordstr = String(record);  	
	size_t matchlength;
	size_t matchstart;

	// Check if incoming record matches the regexp
	//
	try
	{
		matchstart = compiled->index(recordstr, &matchlength, (size_t)0);		
	}
	catch(...)						// If an error occurs, we don't have a match 
	{
		return ACS_USA_Regexp_noMatch;
	}

	if (matchstart != string::npos)
	{
		*begin = matchstart;        // First char matched
		*extent = matchlength;		// Length of match 
    	return ACS_USA_Regexp_match;
    }
    return ACS_USA_Regexp_noMatch;
}


//******************************************************************************
//	operator =
//******************************************************************************
const ACS_USA_Regexp&
ACS_USA_Regexp::operator=(const ACS_USA_Regexp& other)
{
    if (this != &other)				// if not myself
	{			
		status = other.status;
		
		if (status == Status_compiled) 
		{
			delete compiled;
			compiled =	new Regexp(*(other.compiled)); 
			if (compiled == NULL) 
			{
				error.setError(ACS_USA_Memory);
				error.setErrorText("new() failed in ACS_USA_Regexp::operator=(). Criterion:");
			}
		}
    }
    return *this;
}

//******************************************************************************



