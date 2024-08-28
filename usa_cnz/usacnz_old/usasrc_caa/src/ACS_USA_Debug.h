//******************************************************************************//
// .NAME 
//	ACS_USA_Debug.h
// .LIBRARY 3C++
// .PAGENAME
// .HEADER  TMOS/ACS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE ACS_USA_Debug.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-1999.
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
//      Macros for debug and test support

// DOCUMENT NO
//	190 89-CAA 109 0259

// AUTHOR
//      1995-08-01 by ETX/TX/T XKKHEIN

// .REVISION 
//	PA2	90915

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO  DATE    NAME    DESCRIPTION
//		PA1		950829	XKKHEIN	First Revision
//		PA2		990915	UABDMT	Ported to Windows NT4.0

// .LINKAGE
//

// .SEE ALSO
//

//******************************************************************************

#ifndef ACS_USA_DEBUG_H
#define ACS_USA_DEBUG_H

#ifdef DEBUG

#pragma warning (disable: 4786)      // Turn of annoying warning

#include  <fstream>


#include  <stdio.h>
#include  <stdarg.h>
#include  <stdlib.h>

#define DEBUG_DECLARE 

static	std::ofstream logstream("debug.log");	

static void logMsg(const char *format, ...){ 	
	    va_list ap;				
	    char    szMessage[1024];		
	    va_start(ap, format);		
	    vsprintf(szMessage, format, ap);	
	    va_end(ap);				
	    logstream << szMessage << std::flush;	
	}




void    logMsg(const char * format, ...);  // Write message to log file


#define   DEBUG_DumpObjects(a) dumpAll(a)
#define   USA_DEBUG(x)         x
#else
#define	  USA_DEBUG(x)
#define	  DEBUG_DECLARE
#define   DEBUG_DumpObjects(a)
#endif	// DEBUG

#endif
