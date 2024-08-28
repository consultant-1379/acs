//  **************************************************************************
//  **************************************************************************
//  ***                                                                    ***
//  ***  COPYRIGHT (C) Ericsson Utvecklings AB 1999                        ***
//  ***  Box 1505, 125 25 Alvsjo, Sweden, Phone: +46 8 7273000             ***
//  ***                                                                    ***
//  ***  The computer program(s) herein may be used and/or copied only     ***
//  ***  with the written permission of Ericsson Utvecklings AB or in      ***
//  ***  accordance with the terms and conditions stipulated in the        ***
//  ***  agreement/contract under which the program(s) have been supplied. ***
//  ***                                                                    ***
//  **************************************************************************
//  **************************************************************************
//
//  File Name ........@(#)fn 
//  Document Number ..@(#)dn 
//  Revision Date ....@(#)rd 
//  Current Version ..@(#)cv 
//  Responsible ............ (start by filling in this field)
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory
//
//< \file
//
//  This file defines functions that are Windows on x86 specific.
//  XXX This file has not been tested. N.B.
//>

#ifndef HALSPECIFICX86WIN32_H
#define HALSPECIFICX86WIN32_H

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")


//----------------------------------------------------------------------------
//  Imported Types & Definitions
//----------------------------------------------------------------------------
#include "halPlatformDefinitions.hxx"

#if defined INTEL_WIN32_


// XXX Any kernel code in Windows that need adaption?
// #if defined __WIN_KERNEL_WHATEVER_
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

#if defined(__cplusplus)
#define INLINE_CPLUSPLUS inline
#else
#define INLINE_CPLUSPLUS static
#endif //__cplusplus

//< \def MB()
//
//  Memory barriers has no equivalence on Intel platforms so it is
//  just defined as a NOP.
//>
#define MB()

//< \def RMB()
//
//  Memory barriers has no equivalence on Intel platforms so it is
//  just defined as a NOP.
//>
#define RMB()

//< \def WMB()
//
//  Memory barriers has no equivalence on Intel platforms so it is
//  just defined as a NOP.
//>
#define WMB()

//< \def RPCC()
//
// Macro used to access the processor cycle counter (aka time-stamp
// counter on Intel platforms)
//>
#define RPCC() rpcc_() 


//
// XXX The inline functions are not defined in kernel code
// #if !defined(_KERNEL)

//----------------------------------------------------------------------------
//< \fn void mb_(void)
//
//  Memory Barrier, inline function
//>
//----------------------------------------------------------------------------
INLINE_CPLUSPLUS void mb_(void)
{
   MB();
}

//----------------------------------------------------------------------------
//< \fn void rmb_(void)
//
//  Read Memory Barrier, inline function
//>
//----------------------------------------------------------------------------
INLINE_CPLUSPLUS void rmb_(void)
{
   RMB();
}

//----------------------------------------------------------------------------
//< \fn void wmb_(void)
//
//  Write Memory Barrier, inline function
//>
//----------------------------------------------------------------------------
INLINE_CPLUSPLUS void wmb_(void)
{
   WMB();
}

//----------------------------------------------------------------------------
//< \fn u_int32 rpcc_(void)
//
//  Function used to access the processor cycle counter (aka time-stamp
//  counter on Intel platforms, 32-bit value)
//
//  \return  Returns the time-stamp counter
//>
//----------------------------------------------------------------------------
INLINE_CPLUSPLUS u_int64 rpcc_(void)
{
   u_int32 counter;

// Conditional for QAC++ G2000 v1.0. It chokes on this asm so
// make it disappear when running QAC++
#ifndef PRL_QACPP 
   __asm
   {
      // RDTSC: Read time-stamp counter (64 bit) into EDX:EAX
      rdtsc
      mov dword ptr [counter], eax
   }
   return ((u_int64) counter);
#else  // Conditional for QAC++ G2000 v1.0
   return 0;
#endif // Conditional for QAC++ G2000 v1.0 
}


#else
#error Including Specific Windows Intel support on non-Windows Intel platform
#endif  //_INTEL_WIN32

#endif  // HALSPECIFICX86WIN32_H
