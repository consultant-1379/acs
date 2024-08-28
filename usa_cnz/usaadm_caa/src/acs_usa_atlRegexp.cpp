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
//		Tokenizer class.
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
//#pragma warning (disable : 4018)
#include "acs_usa_atlRegexp.h"
#include "acs_usa_atlRegexpImpl.h"
#include <assert.h>

using namespace std;
class Regexp_impl;


/******************* Class implementation : Begin ************/
//========================================================================================
//	Constructors
//========================================================================================
Regexp::Regexp() 
{
	impl_ = new Regexp_impl();
}

Regexp::Regexp(const char* pat) 
{
	impl_ = new Regexp_impl(pat);
}

Regexp::Regexp(const string& pat)
{
	impl_ = new Regexp_impl(pat);
}

Regexp::Regexp(const Regexp& reg) 
{
	impl_ = new Regexp_impl(*reg.impl_);
}


//========================================================================================
//	Destructor
//========================================================================================

Regexp::~Regexp() 
{
	if( NULL != impl_ )
	{
		delete impl_;
		impl_ = NULL;
	}
}


//========================================================================================
//	Assignment operators
//========================================================================================

Regexp&
Regexp::operator=(const Regexp& reg)
{
    if (this != &reg)
    {			
	    delete impl_;
	    impl_ = new Regexp_impl(*reg.impl_);
    }
    return *this;
}

Regexp&
Regexp::operator=(const char* pat)
{
	*impl_ = pat;
	return *this;
}

Regexp&
Regexp::operator=(const std::string& pat)
{
	*impl_ = pat;
	return *this;
}


//========================================================================================
//	Search for a match
//========================================================================================

size_t 
Regexp::index(const string& str, size_t* len, size_t start) const
{
	return impl_->index(str, len, start);
}


//========================================================================================
//	Get status
//========================================================================================

Regexp::statusType
Regexp::status() const
{
	return impl_->status();
}
/******************* Class implementation : End ************/


