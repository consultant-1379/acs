//*****************************************************************************
//
// .NAME
//     ACS_APGCC_Trace
// .LIBRARY 3C++
// .PAGENAME ACS_APGCC_MCC
// .HEADER  ACS_APGCC
// .LEFT_FOOTER Ericsson Utvecklings ABThread
// .INCLUDE ACS_APGCC_Trace.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2001.
//  All rights reserved.
//va_list
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson Utvecklings AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// .DESCRIPTION
//  Implements trace functions.
//
//  It does not make use of any signals; nor will it throw any
//  exceptions.
//  Over- and underflow will not be notified.

// .ERROR HANDLING
//
//  .fill_off
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.
//  .fill_on

// AUTHOR
//      2001-06-19 by UAB/S/AU  QABSJEN

// CHANGES
//
//       RELEASE REVISION HISTORY
//
//       DATE           NAME        DESCRIPTION
//       2001-06-19     QABSJEN     First release
//       2001-08-28     QABSJEN     Added mcc_trace(). Deactivated old functionality.
//
//*****************************************************************************
#ifndef ACS_APGCC_TRACE_H
#define ACS_APGCC_TRACE_H

#pragma once

// PR_O1 BEGIN
#ifndef ACE_SYNCH_H
#  include <ace/Synch.h>
#endif

// PR_O1 BEGIN
#ifndef ACE_THREAD_H
#  include <ace/Thread.h>
#endif
/**/
#ifndef INCLUDED_MAP
#  include <map>
#  define INCLUDED_MAP
#endif

#ifndef INCLUDED_STRING
#  include <string>
#  define INCLUDED_STRING
#endif

// PR_O1 BEGIN
#define MCC_Export ;

// PR_O1 BEGIN
#ifndef INCLUDE_STDARG_H
#	include <stdarg.h>
#	define INCLUDE_STDARG_H
#endif

// PR_O1 BEGIN
#ifndef INCLUDED_STRING_H
#	include <string.h>
#  define INCLUDED_STRING_H
#endif

// PR_O1 BEGIN
#ifndef INCLUDE_STDIO_H
#	include <stdio.h>
#	define INCLUDE_STDIO_H
#endif

// PR_O1 BEGIN
#ifndef INCLUDE_STDLIB_H
#	include <stdlib.h>
#	define INCLUDE STDLIB_H
#endif
#include "ACS_APGCC_Common_Define.h"
/* PR_01 BEGIN
#ifndef MCS_UTIL_H
#  include "MCS_Util.h"
#endif

#ifndef INCLUDED_ACS_TRA_TRACE_H
#  include "ACS_TRA_t#ifndef INCLUDED_ACS_TRA_TRACE_H
#  include "ACS_TRA_trace.h"
#  define INCLUDED_ACS_TRA_TRACE_H
#endifrace.h"
#  define INCLUDED_ACS_TRA_TRACE_H
#endif
END */
/**/
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//
// Trace functions available from MCC. Should only be used from Services.
//
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------



//MCC_Export ACE_Thread_Mutex __g_mcc_trace_lock;


//----------------------------------------------------------------------------------------
// Format given arguments into the tracemsg string.
//----------------------------------------------------------------------------------------
/**/

MCC_Export void mcc_format_tracemsg(char* tracemsg, va_list argp, const char* format_str);


//---MCC_Export-------------------------------------------------------------------------------------
// Send trace to TRA.
//----------------------------------------------------------------------------------------
/* PR_01 BEGIN
inline void mcc_trace(ACS_TRA_trace& tracePoint, const char* format_str, ...)
{
#if defined (NDEBUG)
   // In Release mode we need to test this flag immediately to
   // avoid unnecesarry overhead. We can't do that in Debug mode
   // since this code is used to print on stdout as well.
   if (ACS_TRA_ON(tracePoint)) {
#endif

   OSF_Guard<OSF_Thread_Mutex> guard(__g_mcc_trace_lock);
   va_list argp;

   if ( !format_str ) {
      return;
   }

   // Start of variable args section.
   va_start(argp, format_str);
   mcc_format_tracemsg(__g_mcc_trace_msg, argp, format_str);
   va_end(argp);


#if defined (NDEBUG)
   ACS_TRA_event(&tracePoinMCC_Export t, __g_mcc_trace_msg);
#else
   OSF_DEBUG((LM_DEBUG,"%s %s\n",tracePoint.recordname,__g_mcc_trace_msg));
   if (ACS_TRA_ON(tracePoint)) {
      ACS_TRA_event(&tracePoint, __g_mcc_trace_msg);
   }
#endif

#if defined (NDEBUG)
   }
#endif
}

*/


//=========================================================================================
// NOT USED BELOW
//=========================================================================================
#if 0

#if defined (_DEBUG) && defined (OSF_NLOGGING)
#  define __SET_OSF_NLOGGING
#  undef OSF_NLOGGING
#endif

#include <ace/Synch.h>
#include "MCS_OS.h"
#include "ACS_TRA_trace.H"



/*
 * Trace Macro that generates a TRA trace as well as a OSF_DEBUG printout.
 *
 */
#define TRACE(X) \
   do { \
      int __win_error = ::GetLastError(); \
      MCS_Trace& mcs___ = MCS_Trace::instance(); \
      mcs___.set(__FILE__, __LINE__, __win_error); \
      mcs___.log X; \
   } while (0)



class MCC_Export MCS_Trace
{
   enum { MAXLOGMSGLEN = 300 };

public:
   MCS_Trace();

   class MCS_Log_Msg;

   static MCS_Trace& instance();

   /*
     void log (const char *format, ...);
     // Format ACS_TRA_tra message to the thread-safe OSF logging mechanism.  Valid
     // options (prefixed by '%', as in printf format strings) include:
     // 'c': print a character
     // 'i', 'd': print a decimal number
     // 'I', indent according to nesting depth
     // 'e', 'E', 'f', 'F', 'g', 'G': print a double
     // 'l', print line number where an error occurred.
     // 'o': print as an octal number
     // 'P': print out the current process id
     // 'p': print out the appropriate errno value from sys_errlist
     // 'R': print return status
     // 's': print out a character string
     // 'T': print timestamp in hour:minute:sec:usec format.
     // 'D': print timestamp in month/day/year hour:minute:sec:usec format.
     // 't': print thread id (1 if single-threaded)
     // 'u': print as unsigned int
     // 'X', 'x': print as a hex number
     // '%': print out a single percent sign, '%'
   */
   void log(ACS_TRA_trace& tracePoint, const char* format_str, ...);
   void log(ACS_TRA_trace& tracePoint, const char* format_str, va_list argp);

   void set(const char* file, int linenum, DWORD last_error);

private:
   int linenum_;
   int last_error_;
   char msg_[MAXLOGMSGLEN];
   OSF_Thread_Mutex lock_;
};


#if defined (__SET_OSF_NLOGGING)
#  undef __SET_OSF_NLOGGING
#  define OSF_NLOGGING
#endif

#endif // if 0

#endif // ACS_APGCC_TRACE_H
