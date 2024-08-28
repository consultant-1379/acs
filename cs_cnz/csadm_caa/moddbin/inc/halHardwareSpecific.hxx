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
//  This file is general include file for specific architecture dependent
//  include files (single point of include).
//>

#ifndef HALHARDWARESPECIFIC_HXX
#define HALHARDWARESPECIFIC_HXX

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")


//----------------------------------------------------------------------------
//  Imported Types & Definitions
//----------------------------------------------------------------------------
#include "halPlatformDefinitions.hxx"


//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------
#if defined(__cplusplus)
#define INLINE_CPLUSPLUS inline
#else
#define INLINE_CPLUSPLUS static
#endif //__cplusplus

#if defined(__cplusplus)
extern "C" {
#endif

//
// Exclude the inline functions in kernel code
//
#if ( !defined(__KERNEL__) && !defined(_KERNEL) && !defined(KERNEL) )
INLINE_CPLUSPLUS void mb_(void);
INLINE_CPLUSPLUS void rmb_(void);
INLINE_CPLUSPLUS void wmb_(void);
INLINE_CPLUSPLUS u_int64 rpcc_(void);
#endif //!__KERNEL__

#if defined(ALPHA_TRU64_)
#include "halSpecificAlphaTru64.h"
#elif defined(ALPHA_WIN32_)
#include "halSpecificAlphaWin32.h"
#elif defined(INTEL_WIN32_)
#include "halSpecificX86Win32.h"
#elif defined(SPARC_SOLARIS_)
#include "halSpecificSparcSolaris.h"
#elif defined(IPF_LINUX_)
#include "halSpecificIPFLinux.h"
#elif defined(X86_LINUX_)
#include "halSpecificX86Linux.h"
#else
#error Architechture is not supported in the file __FILE__, __LINE__
#endif //ALPHA_TRU64_


#if defined(__cplusplus)
}
#endif

#endif   // HALHARDWARESPECIFIC_HXX
