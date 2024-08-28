//******************************************************************************
//
//  NAME
//     acs_lm_trace.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ACS_LM_Trace.h>

#include <cstring>
/*=================================================================
	ROUTINE: ACS_LM_Trace
=================================================================== */
ACS_LM_Trace::ACS_LM_Trace(const char* aName)
:std::ostrstream(theBuffer, 1024, std::ios::out)
{
	unsigned len = (unsigned)::strlen(aName);
	unsigned maxSize = (len < (nameSize)) ? len : (nameSize);
	::strncpy(theName, aName, maxSize);

	const char* tmpString = "C";
	ACS_TRA_trace tmp = ACS_TRA_DEF(theName, (char*)tmpString);
	theTrace = tmp;
}//end of ACS_LM_Trace
/*=================================================================
	ROUTINE: print
=================================================================== */
void ACS_LM_Trace::print()
{
	//put end of the string
    *this << std::ends;

	// report buffer contents
	ACS_TRA_event(&theTrace, theBuffer);

	// reset buffer
	if(fail())
	{
		clear();
	}
   seekp(std::ios::beg);
}//end of print
