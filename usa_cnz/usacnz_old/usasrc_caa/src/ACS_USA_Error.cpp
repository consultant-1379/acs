//******************************************************************************
//
// NAME
//      ACS_USA_Error.cpp
//
// COPYRIGHT
//      Ericsson AB 2005-2006 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the 
//      written permission from Ericsson AB or in accordance 
//      with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//	    190 89-CAA 109 0545
//
// AUTHOR 
// 	    1995-08-10 by ETX/TX/T XKKHEIN
//
// REV  DATE    NAME     DESCRIPTION
// A    950829  XKKHEIN  First version.
// B    990915  UABDMT   Ported to Windows NT4.0.
// C    060426  UABPEK   EVENT STRING ERROR added and PARAMETER ERROR removed.
//
//******************************************************************************

#include	"ACS_USA_Error.h"

//******************************************************************************
// Problem texts in USA
//******************************************************************************

const char* const problemTable[] = {
	"",
	"ERROR IN ACS_USA_SyslogAnalyser",
	"I/O ERROR",
	"SYSTEM CALL FAILED",
	"BAD OBJECT STATE",
	"OUT OF MEMORY",
	"INVALID TIME STAMP",
	"INVALID TIME STAMP FORMAT",
	"CANNOT EXTRACT TIME",
	"POSSIBLY INVALID TIME FORMAT",
	"ALARM QUEUE FULL",
	"CANNOT DELIVER EVENTS",
	"EVENT STRING ERROR",
	"SERVICE EXIT",
	"TOO HIGH EVENT FREQUENCY"
};		    


//******************************************************************************
//	ACS_USA_Error()
//******************************************************************************
ACS_USA_Error::ACS_USA_Error()
 : ACS_AEH_Error()
{
}



//******************************************************************************
//	getProblemText()
//******************************************************************************
const char*
ACS_USA_Error::getProblemText(const ACS_USA_ErrorType idx) const
{
    return problemTable[idx];
}



//******************************************************************************
//	~ACS_USA_Error()
//******************************************************************************
ACS_USA_Error::~ACS_USA_Error()
{
}

//******************************************************************************
