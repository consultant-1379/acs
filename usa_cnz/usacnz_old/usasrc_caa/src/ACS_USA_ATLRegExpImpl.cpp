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
#pragma warning (disable : 4018)
#include "ACS_USA_ATLRegExpImpl.h"
#include <assert.h>

using namespace std;

//========================================================================================
//	Constructors
//========================================================================================

Regexp_impl::Regexp_impl() :
rex_(),
err_(REPARSE_ERROR_INVALID_INPUT),
pat_()
{
}

Regexp_impl::Regexp_impl(const char* pat) :
rex_(),
err_(),
pat_(pat)
{
	err_ = rex_.Parse(pat);
}

Regexp_impl::Regexp_impl(const string& pat) :
rex_(),
err_(),
pat_(pat.data())
{
	err_ = rex_.Parse(pat.data());
}

Regexp_impl::Regexp_impl(const Regexp_impl& reg) :
rex_(),
err_(reg.err_),
pat_(reg.pat_)
{
	err_ = rex_.Parse(reg.pat_);
}


//========================================================================================
//	Destructor
//========================================================================================

Regexp_impl::~Regexp_impl() 
{
}


//========================================================================================
//	Assignment operators
//========================================================================================

Regexp_impl&
Regexp_impl::operator=(const Regexp_impl& reg)
{
	pat_ = reg.pat_;
	err_ = rex_.Parse(pat_);
	return *this;
}

Regexp_impl&
Regexp_impl::operator=(const char* pat)
{
	pat_ = pat;
	err_ = rex_.Parse(pat_);
	return *this;
}

Regexp_impl&
Regexp_impl::operator=(const std::string& pat)
{
	pat_ = pat.data();
	err_ = rex_.Parse(pat_);
	return *this;
}


//========================================================================================
//	Search for a match
//========================================================================================

size_t 
Regexp_impl::index(const string& str, size_t* len, size_t start) const
{
	assert(err_ == REPARSE_ERROR_OK);
	size_t tlen;
	if (len == NULL) len = &tlen;

	if (start >= str.length())
	{
		*len = 0;
		return std::string::npos;
	}
	const char* str1 = str.data();

	CAtlREMatchContext<> mcRex;
		if (rex_.Match(&str1[start], &mcRex) == TRUE)
	{
		*len = mcRex.m_Match.szEnd - mcRex.m_Match.szStart;
		return (size_t)(mcRex.m_Match.szStart - str1);
	}
	else
	{
		// No match
		*len = 0;
		return std::string::npos;
	}
}


//========================================================================================
//	Get status
//========================================================================================

Regexp::statusType
Regexp_impl::status() const
{
	Regexp::statusType state;
	switch (err_)
	{
	case REPARSE_ERROR_OK:				 state = Regexp::OK;				break;
	case REPARSE_ERROR_OUTOFMEMORY:		 state = Regexp::OUTOFMEMORY;		break;
	case REPARSE_ERROR_BRACE_EXPECTED:	 state = Regexp::BRACE_EXPECTED;	break;
	case REPARSE_ERROR_PAREN_EXPECTED:	 state = Regexp::PAREN_EXPECTED;	break;
	case REPARSE_ERROR_BRACKET_EXPECTED: state = Regexp::BRACKET_EXPECTED;	break;
	case REPARSE_ERROR_UNEXPECTED:		 state = Regexp::UNEXPECTED;		break;
	case REPARSE_ERROR_EMPTY_RANGE:		 state = Regexp::EMPTY_RANGE;		break;
	case REPARSE_ERROR_INVALID_GROUP:	 state = Regexp::INVALID_GROUP;		break;
	case REPARSE_ERROR_INVALID_RANGE:	 state = Regexp::INVALID_RANGE;		break;
	case REPARSE_ERROR_EMPTY_REPEATOP:	 state = Regexp::EMPTY_REPEATOP;	break;
	case REPARSE_ERROR_INVALID_INPUT:	 state = Regexp::INVALID_INPUT;		break;
	default: assert(!"Illegal regexp state");
	}
	return state;
}
