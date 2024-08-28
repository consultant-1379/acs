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
//  This file defines a number of data types that must be used instead of
//  those defined by the development environment in order to minimize the
//  platform dependencies on the developed code.
//
//  The below mentioned defines are set by this file when it has determined
//  what platform and compiler that is used. 
//
//  \verbatim
//  Define           Platform           Supported compilers
//----------------------------------------------------------------------------
//  ALPHA_TRU64_     Alpha/Tru64        gnuc, cxx, cc
//  ALPHA_WIN32_     Alpha/Win32        gnuc, MS Visual C++
//  INTEL_WIN32_     Intel/Win32        gnuc, MS Visual C++
//  SPARC_SOLARIS_   SPARC/Solaris      gnuc, CC, cc
//  IPF_LINUX_       IPF/Linux          icc
//  X86_LINUX_       Intel/Linux        gnuc
//
//  The below definitions are deprecated: 
//  _ALPHA_TRU64
//  _ALPHA_WIN32
//  _INTEL_WIN32
//  _SPARC_SOLARIS
//  _IPF_LINUX
//  _X86_LINUX
//  \endverbatim
//
//  The following defines can be used to determine which
//  operating system that is used on the current platform
//
//  \verbatim
//  Define           Operating system
//----------------------------------------------------------------------------
//  _TRU64           Tru64
//  _WIN32           Win32
//  \endverbatim
//
//  The following defines can be used to determine the byte
//  order (i.e big- or little-endian) on the current
//  platform.
//
//  \verbatim
//  Define           Platform
//----------------------------------------------------------------------------
//  BIG_ENDIAN_      SPARC/Solaris
//  LITTLE_ENDIAN_   Alpha/Tru64 (big-endian version not supported)
//                   Alpha/Win32
//                   Intel/Win32
//                   IPF/Linux
// 
//  The below definitions are deprecated: 
//  BIG_ENDIAN__
//  LITTLE_ENDIAN__
//  _BIG_ENDIAN
//  _LITTLE_ENDIAN      
//  \endverbatim
//
//  To indicate a generic unix system the following define can be used
//  If not already defined by the compiler this file defines it for
//  platforms that it consider to be "unix" systems.
//
//  \verbatim
//  Define           Platform       
//----------------------------------------------------------------------------
//  __unix           Alpha/Tru64
//                   Sparc/Solaris
//                   IPF/Linux
//  \endverbatim
//
//  The following definition may be used to determine if a platform/compiler
//  places the virtual function table pointer before or after other data 
//  members in a class declaration.
//
//  \verbatim
//  Define                        
//----------------------------------------------------------------------------
//  VFPTR_PLACED_LAST_           
//
//  The below definitions are deprecated:
//  VFPTR_PLACED_LAST__
//  \endverbatim
//>

#ifndef HALPLATFORMDEFINITIONS_HXX
#define HALPLATFORMDEFINITIONS_HXX

//#pragma ident "@(#)filerevision "
//#pragma comment (user, "@(#)filerevision ")

#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

// Note: all names beginning with _<upper case letter> and
// __<lower case letter> are formally reserved for run time library
// implementors!

// Set up internal defines to use instead of always testing
// on combinations of defines.
#if defined(_M_ALPHA) || defined(__alpha)
#if defined(_WIN32)   // Defined by MS Visual C++ and gnuc

//< \def ALPHA_WIN32_
//
//  Defines Windows on Alpha.
//>
#define ALPHA_WIN32_

//< \def _ALPHA_WIN32
//
//  Defines Windows on Alpha.
//>
#define _ALPHA_WIN32_

#else

//< \def ALPHA_TRU64_
//
//  Defines Compaq Tru64 on Alpha.
//>
#define ALPHA_TRU64_

//< \def _ALPHA_TRU64
//
//  Defines Compaq Tru64 on Alpha.
//>
#define _ALPHA_TRU64

#ifndef _TRU64 // Set the define _TRU64 if it has not been defined.

// Using a \def _TRU64 in doxygen comment below makes doxygen complaín.
// I do not know why. 
//<
//
//  Defines running on Compaq Tru64 on Alpha.
//>
#define _TRU64
#endif // _TRU64
//  Also define the "generic unix" symbol (__unix) if not already defined.
#ifndef __unix

//< \def __unix
//
//  Defines running a Unix system (it is defined by at least the compaq
//  Tru64 C++ compiler)  
//>
#define __unix
#endif
#endif 
#elif defined(_M_IX86) || defined(_X86_)

//< \def INTEL_WIN32_
//
//  Defines Windows on Intel.
//>
#define INTEL_WIN32_

//< \def _INTEL_WIN32
//
//  Defines Windows on Intel.
//>
#define _INTEL_WIN32

#elif defined(__sparc) || defined(sparc)

//< \def SPARC_SOLARIS_
//
//  Defines Solaris on Sparc.
//>
#define SPARC_SOLARIS_

//< \def _SPARC_SOLARIS
//
//  Defines Solaris on Sparc.
//>
#define _SPARC_SOLARIS

// also define the "generic unix" symbol (__unix) if not already defined.
#ifndef __unix

//< \def __unix
//
//  Defines running a Unix system (it is defined by at least the Sun Forte
//  C++ compiler).
//>
#define __unix
#endif
// The test for ia64 is before the __GNU?__ tests because the Intel C++
// compiler (version 8, icc) defines __GNU?__ symbols. To avoid the assumption
// that defined __GNUG__ implies gpp on windows we test for IA64 before
// testing for __GNU?__
#elif defined(__ia64__) || defined(__ia64) || defined(ia64)

//< \def IPF_LINUX_
//
//  Defines Linux on IPF
//>
#define IPF_LINUX_

//< \def _IPF_LINUX
//
//  Defines Linux on IPF
//>
#define _IPF_LINUX

// also define the "generic unix" symbol (__linux) if not already defined.
#ifndef __linux

//< \def __linux
//
//  Defines running a Linux system
//>
#define __linux
#endif

// also define the "generic unix" symbol (__unix) if not already defined.
#ifndef __unix

//< \def __unix
//
//  Defines running a Unix system (it is defined by at least the Intel 
//  C++ compiler).
//>
#define __unix
#endif
#elif defined(__linux) && (defined(__i386) || defined(i386))

//< \def X86_LINUX_
//
//  Defines Linux on x86
//>
#define X86_LINUX_

//< \def _X86_LINUX
//
//  Defines Linux on x86
//>
#define _X86_LINUX

// also define the "generic unix" symbol (__unix) if not already defined.
#ifndef __unix

//< \def __unix
//
//  Defines running a Unix system (it is defined by at least the Intel 
//  C++ compiler).
//>
#define __unix
#endif
#elif defined(__GNUG__) && !defined(__unix) // (djgpp only defines __GNUG__!?)
// Assume that this is a djgpp environment on a Win32/Intel platform
//< \def INTEL_WIN32_
//
//  Defines Windows on Intel.
//>
#define INTEL_WIN32_

//< \def _INTEL_WIN32
//
//  Defines Windows on Intel.
//>
#define _INTEL_WIN32

#endif // _M_ALPHA || __alpha


//
// Define a common base of definitions for each of the platform types defined
// above.
//
#if defined(ALPHA_WIN32_) // Platforms: Win32 on Alpha

//< \def LITTLE_ENDIAN_
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN_

//< \def LITTLE_ENDIAN__
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN__

//< \def _LITTLE_ENDIAN
//
//  This is a little-endian platform.
//>
#define _LITTLE_ENDIAN

#if defined(_MSC_VER)  // MS Visual C++
//< \typedef uint8_t
//
//  Defines a general and shorter name.
//>
typedef unsigned char  uint8_t;

//< \typedef uint16_t
//
//  Defines a general and shorter name.
//>
typedef unsigned short uint16_t;

//< \typedef uint32_t
//
//  Defines a general and shorter name.
//>
typedef unsigned int   uint32_t;

//< \typedef uint64_t
//
//  Defines a general and shorter name.
//>
typedef unsigned __int64  uint64_t;

//< \typedef int8_t
//
//  Defines a general and shorter name.
//>
typedef signed char       int8_t;

//< \typedef int16_t
//
//  Defines a general and shorter name.
//>
typedef short          int16_t;

//< \typedef int32_t
//
//  Defines a general and shorter name.
//>
typedef int            int32_t;

//< \typedef int64_t
//
//  Defines a general and shorter name.
//>
typedef __int64        int64_t;

#if _MSC_VER <= 1200
//< \typedef intptr_t
//
//  A integral type large enough to hold an address (arbitrary pointer).
//>
typedef signed long  intptr_t;

//< \typedef uintptr_t
//
//  A type large enough to hold an address.
//>
typedef unsigned long uintptr_t;
#else // _MSC_VER <= 1200
#include <cstddef>
#endif // _MSC_VER <= 1200

//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms that does not
//  support it.
//>
#define __unaligned

#elif defined(__GNUC__) || defined(__GNUG__) // gnuc compiler 
                                             // (djgpp only defines __GNUG__!?)

//< \typedef uint8_t
//
//  Defines a general and shorter name.
//>
typedef unsigned char  uint8_t;

//< \typedef uint16_t
//
//  Defines a general and shorter name.
//>
typedef unsigned short uint16_t;

//< \typedef uint32_t
//
//  Defines a general and shorter name.
//>
typedef unsigned int   uint32_t;

//< \typedef uint64_t
//
//  Defines a general and shorter name.
//>
typedef unsigned long long  uint64_t;

//< \typedef int8_t
//
//  Defines a general and shorter name.
//>
typedef signed char    int8_t;

//< \typedef int16_t
//
//  Defines a general and shorter name.
//>
typedef short          int16_t;

//< \typedef int32_t
//
//  Defines a general and shorter name.
//>
typedef int            int32_t;

//< \typedef int64_t
//
//  Defines a general and shorter name.
//>
typedef long long      int64_t;

//< \typedef intptr_t
//
//  A integral type large enough to hold an address (arbitrary pointer).
//>
typedef signed long  intptr_t;

//< \typedef uintptr_t
//
//  A type large enough to hold an address.
//>
typedef unsigned long  uintptr_t;

//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms that does not
//  support it.
//>
#define __unaligned

#else
#error Supported compilers on Win32 for Alpha are MS Visual C++ and gnuc
#endif // _MSC_VER


#elif defined(ALPHA_TRU64_) // Platforms: Tru64 on Alpha

// Get the Alpha specific boolean_t
#include "/usr/include/mach/machine/boolean.h"

#if defined(_KERNEL)
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;
typedef unsigned long  uintptr_t;

typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;
typedef signed long  int64_t;
typedef signed long  intptr_t;

#else
#include "/usr/include/inttypes.h"
#endif //_KERNEL


#if defined(__DECCXX)
//< \def VFPTR_PLACED_LAST_
//
//  The 'cxx' compiler is placing the virtual funtion table
//  pointer after all other data members in a class.
//>
#define VFPTR_PLACED_LAST_
//< \def VFPTR_PLACED_LAST__
//
//  The 'cxx' compiler is placing the virtual funtion table
//  pointer after all other data members in a class.
//>
#define VFPTR_PLACED_LAST__
#else
//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms/compilers that
//  does not support it.
//>
#define __unaligned 
#endif // __DECCXX

//< \def LITTLE_ENDIAN_
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN_

//< \def LITTLE_ENDIAN__
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN__

//< \def _LITTLE_ENDIAN
//
//  This is a little-endian platform.
//>
#define _LITTLE_ENDIAN


#elif defined(INTEL_WIN32_) // Platforms: Win32 on Intel

//< \def LITTLE_ENDIAN_
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN_

//< \def LITTLE_ENDIAN__
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN__

//< \def _LITTLE_ENDIAN
//
//  This is a little-endian platform.
//>
#define _LITTLE_ENDIAN


#if defined(_MSC_VER)  // MS Visual C++

//< \typedef uint8_t
//
//  Defines a general and shorter name.
//>
typedef unsigned char  uint8_t;

//< \typedef uint16_t
//
//  Defines a general and shorter name.
//>
typedef unsigned short uint16_t;

//< \typedef uint32_t
//
//  Defines a general and shorter name.
//>
typedef unsigned int   uint32_t;

//< \typedef uint64_t
//
//  Defines a general and shorter name.
//>
typedef unsigned __int64  uint64_t;

//< \typedef int8_t
//
//  Defines a general and shorter name.
//>
typedef signed char    int8_t;

//< \typedef int16_t
//
//  Defines a general and shorter name.
//>
typedef short          int16_t;

//< \typedef int32_t
//
//  Defines a general and shorter name.
//>
typedef int            int32_t;

//< \typedef int64_t
//
//  Defines a general and shorter name.
//>
typedef __int64        int64_t;

#if _MSC_VER <= 1200
//< \typedef intptr_t
//
//  A integral type large enough to hold an address (arbitrary pointer).
//>
typedef signed long  intptr_t;

//< \typedef uintptr_t
//
//  A type large enough to hold an address.
//>
typedef unsigned long  uintptr_t;
#else // _MSC_VER <= 1200
#include <cstddef>
#endif // _MSC_VER <= 1200

//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms that does not
//  support it.
//>
#define __unaligned

#elif defined(__GNUC__) || defined(__GNUG__) // gnuc compiler
                                             // (djgpp only defines __GNUG__!?)

//< \typedef uint8_t
//
//  Defines a general and shorter name.
//>
typedef unsigned char  uint8_t;

//< \typedef uint16_t
//
//  Defines a general and shorter name.
//>
typedef unsigned short uint16_t;

//< \typedef uint32_t
//
//  Defines a general and shorter name.
//>
typedef unsigned int   uint32_t;

//< \typedef uint64_t
//
//  Defines a general and shorter name.
//>
typedef unsigned long long  uint64_t;

//< \typedef int8_t
//
//  Defines a general and shorter name.
//>
typedef signed char    int8_t;

//< \typedef int16_t
//
//  Defines a general and shorter name.
//>
typedef short          int16_t;

//< \typedef int32_t
//
//  Defines a general and shorter name.
//>
typedef int            int32_t;

//< \typedef int64_t
//
//  Defines a general and shorter name.
//>
typedef long long      int64_t;

//< \typedef intptr_t
//
//  A integral type large enough to hold an address (arbitrary pointer).
//>
typedef signed long  intptr_t;

//< \typedef uintptr_t
//
//  A type large enough to hold an address.
//>
typedef unsigned long  uintptr_t;

//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms that does not
//  support it.
//>
#define __unaligned  

#else
#error Supported compilers on Win32 for Intel are MS Visual C++ and gnuc
#endif // _MSC_VER

#elif defined(SPARC_SOLARIS_) // Platforms: Solaris on SPARC

#if defined(_KERNEL)
// XXX define the types-_t for kernel mode
#else
#include "/usr/include/inttypes.h"
#endif //_KERNEL

//< \def BIG_ENDIAN_
//
//  This is a big-endian platform.
//>
#define BIG_ENDIAN_

//< \def BIG_ENDIAN__
//
//  This is a big-endian platform.
//>
#define BIG_ENDIAN__

//< \def _BIG_ENDIAN
//
//  This is a big-endian platform.
//>
#define _BIG_ENDIAN

//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms that does not
//  support it.
//>
#define __unaligned


#elif defined(IPF_LINUX_) // Platforms: Linux on IPF

#if defined(__KERNEL__)
#include <linux/types.h>
   typedef unsigned long uintptr_t;
   typedef signed long intptr_t;
#else

#include "/usr/include/inttypes.h"
#endif //__KERNEL__

/* icc 8.1 compiler seems to support __unaligned. Do NOT define
   __unaligned to an empty macro! 

//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms/compilers that
//  does not support it.
//>
#define __unaligned 
*/

//< \def LITTLE_ENDIAN_
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN_

//< \def LITTLE_ENDIAN__
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN__

//< \def _LITTLE_ENDIAN
//
//  This is a little-endian platform.
//>
#define _LITTLE_ENDIAN


#elif defined(X86_LINUX_)  // Platforms: Linux on i386

#if defined(__KERNEL__)
#include <linux/types.h>
   typedef unsigned long uintptr_t;
   typedef signed long intptr_t;
#else
#include "/usr/include/inttypes.h"

#endif //__KERNEL__

//< \def LITTLE_ENDIAN_
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN_

//< \def LITTLE_ENDIAN__
//
//  This is a little-endian platform.
//>
#define LITTLE_ENDIAN__

//< \def _LITTLE_ENDIAN
//
//  This is a little-endian platform.
//>
#define _LITTLE_ENDIAN

#if defined(__GNUC__)

//< \def __unaligned
// 
//  The __unaligned data-type specifier is used in pointer definitions to
//  indicate to the compiler that the data pointed to is not properly aligned
//  on a correct address. Since this is a non-standard extension it is
//  defined as a macro that expands to nothing for platforms/compilers that
//  does not support it.
//>
#define __unaligned 
#else
#error Supported compiler(s) on Linux for x86 are gcc/g++
#endif

#else
//#error The platform or the compiler that you are using is not supported
#define LITTLE_ENDIAN_
#undef BIG_ENDIAN_
#endif // Platforms:


//
// Defining General types
//

//< \typedef u_int64
//
//  Defines a general and shorter name.
//>
typedef uint64_t  u_int64;

//< \typedef int64
//
//  Defines a general and shorter name.
//>
typedef int64_t   int64;

//< \typedef u_int32
//
//  Defines a general and shorter name.
//>
typedef uint32_t  u_int32;

//< \typedef int32
//
//  Defines a general and shorter name.
//>
typedef int32_t   int32;

//< \typedef u_int16
//
//  Defines a general and shorter name.
//>
typedef uint16_t  u_int16;

//< \typedef int16
//
//  Defines a general and shorter name.
//>
typedef int16_t   int16;

//< \typedef u_int8
//
//  Defines a general and shorter name.
//>
typedef uint8_t   u_int8;

//< \typedef int8
//
//  Defines a general and shorter name.
//>
typedef int8_t    int8;

// u_char exists for kernel code
#if !defined(__KERNEL__) && !defined(_KERNEL) && !defined(KERNEL)
//< \typedef u_char
//
//  Defines a general and shorter name.
//>
typedef unsigned char  u_char;
#endif

//< \typedef PointerAsInteger
//
//  An integral type large enough to hold an address (arbitrary pointer).
//>
typedef intptr_t  PointerAsInteger;

//< \typedef NativeAddress
//
//  A type large enough to hold an address.
//>
typedef intptr_t  NativeAddress;

// dma_addr_t already exist for Linux kernel code (exclude it)
#if !((defined(__linux)) && defined(__KERNEL__))
//< \typedef dma_addr_t
//
//  Defines a general and shorter name.
//>
typedef unsigned long dma_addr_t;
#endif

//< \typedef mem_addr_t
//
//  Defines a general and shorter name.
//>
typedef unsigned long mem_addr_t;

// The header file below (Tru64) includes a typedef of boolean_t
// For Solaris boolean_t is defined in sys/types.h. 
#if !defined(_MACH_ALPHA_BOOLEAN_H_) && !defined(SPARC_SOLARIS_)
//< \typedef boolean_t
//
//  Defines a general and shorter name.
//>
typedef unsigned char boolean_t;
#endif

#if !defined(true)
//< \def true
//
//  Defines what is true
//>
#define true  1
//< \def false
//
//  Defines what is false
//>
#define false 0
#endif


// Verify byte order definitions before we continue...
#if !defined( BIG_ENDIAN_ ) && !defined( LITTLE_ENDIAN_ )
#error Target platform is missing byte order definition
#else
#if defined( BIG_ENDIAN_ ) && defined( LITTLE_ENDIAN_ )
#error Ambiguous byte order definition for target platform. Multiple definitions found.
#endif
#endif

//< \typedef Bit_32
//
//  XXX Some desc.
//>
typedef unsigned int Bit_32;

//< \typedef Bit
//
//  XXX Some desc.
//>
typedef Bit_32 Bit;

// The default size of bit fields is normally 'unsigned int'.
// Some compilers however, such as CC and g++, can handle bit fields
// less than this by reducing its size dynamically using pragma pack.
// To avoid "padding" by compilers that does not implement this behavour
// the following additional bit field sizes are defined.
// Note that they must only be used if the total bit field size does
// not reach the size of an unsigned integer.
//< \typedef Bit_8
//
//  XXX Some desc.
//>
typedef u_char Bit_8;

//< \typedef Bit_16
//
//  XXX Some desc.
//>
typedef u_int16 Bit_16;

//< \def MAX_U_INT64
//
//  Max value a u_int64 can hold
//>
#if defined(__cplusplus)
#define MAX_U_INT64 ~static_cast<u_int64>(0)
#else
#define MAX_U_INT64 ~((u_int64) 0)
#endif //__cplusplus

#if defined(X86_LINUX_) || defined(SPARC_SOLARIS_)
//< \def INT64_C_
//
// Wrapper to mask differences in 64 bit representation of constants between
// 64 bit and 32 bit platforms; as presented in GnuC
//>
#define INT64_C_(x) x##LL

//< \def UINT64_C_
//
// Wrapper to mask differences in 64 bit representation of constants between
// 64 bit and 32 bit platforms; as presented in GnuC
//>
#define UINT64_C_(x) x##ULL
#else // defined ( X86_LINUX_ ) || defined ( SPARC_SOLARIS_ )
//< \def INT64_C_
//
// Wrapper to mask differences in 64 bit representation of constants between
// 64 bit and 32 bit platforms; as presented in GnuC
//>
#define INT64_C_(x) x##L

//< \def UINT64_C_
//
// Wrapper to mask differences in 64 bit representation of constants between
// 64 bit and 32 bit platforms; as presented in GnuC
//>
#define UINT64_C_(x) x##UL
#endif // defined ( X86_LINUX_ ) || defined ( SPARC_SOLARIS_ )

#if defined(_MSC_VER)
#include <malloc.h>  // _alloca()
//< \def alloca
//
//  'alloca' is a non ANSI C++ function that is a de-facto standard for most
//  unix compilers on the market, but not for MSVC++. It is implemented but
//  with an alternative name ('_alloca') and header file.
//>
#define alloca _alloca

//< \def snprintf
//
//  'snprintf' is a non ANSI C++ function that is a de-facto standard for most
//  unix compilers on the market, but not for MSVC++. It is implemented but 
//  with an alternative name ('_snprintf').
//>
#define snprintf _snprintf

//< \def vsnprintf
//
//  'vsnprintf' is a non ANSI C++ function that is a de-facto standard for most
//  unix compilers on the market, but not for MSVC++. It is implemented but 
//  with an alternative name ('_vsnprintf').
//>
#define vsnprintf _vsnprintf
#else
#if !defined(__GNUC__) && !defined(__GNUG__) // (djgpp only defines __GNUG__!?)
#if defined(ALPHA_TRU64_)
#include <alloca.h>
#else
#ifdef __cplusplus
#include <cstdlib>    // alloca()
#else
#include <stdlib.h>
#endif //__cplusplus
#endif
#endif
#endif


//
// SWAP functions in C++, else defined as macros
//
#if defined(__cplusplus)
//----------------------------------------------------------------------------
//< \fn u_int16 SWAP16(const u_int16 a)
//
//  Byte swap a 16 bit entity (eg from/to big/little endian)
//
//  \pin       a     The bytes to be swapped
//  \return          Returns a new word with the bytes swapped from a
//>
//----------------------------------------------------------------------------
inline u_int16 SWAP16(const u_int16 a) 
{
   return static_cast<u_int16>(((a >> 8) & 0xff) | ((a & 0xff) << 8));
}

//----------------------------------------------------------------------------
//< \fn u_int32 SWAP32(const u_int32 a) 
//
//  Byte swap a 32 bit entity (eg from/to big/little endian)
//
//  \pin       a     The bytes to be swapped
//  \return          Returns a new word with the bytes swapped from a
//>
//----------------------------------------------------------------------------
inline u_int32 SWAP32(const u_int32 a)
{
   return
      (((a >> 24) & 0xff)    |
       ((a & 0xff0000) >> 8) | 
       ((a & 0xff00) << 8)   |
       ((a & 0xff) << 24)
      );
}

//----------------------------------------------------------------------------
//< \fn u_int64 SWAP64(const u_int64 a) 
//
//  Byte swap a 64 bit entity (eg from/to big/little endian)
//
//  \pin       a     The bytes to be swapped
//  \return          Returns a new word with the bytes swapped from a
//>
//----------------------------------------------------------------------------
inline u_int64 SWAP64(const u_int64 a)
{
   return 
      (((a >> 56) & 0xff)                        |
       ((a & UINT64_C_(0xff000000000000)) >> 40) | 
       ((a & UINT64_C_(0x00ff0000000000)) >> 24) |
       ((a & UINT64_C_(0x0000ff00000000)) >> 8)  |
       ((a & UINT64_C_(0x000000ff000000)) << 8)  |
       ((a & UINT64_C_(0x00000000ff0000)) << 24) | 
       ((a & UINT64_C_(0x0000000000ff00)) << 40) |
       ((a & 0xff) << 56)
      );
}

#else
//< \def SWAP16(a)
//
//  Byte swap a 16 bit entity (eg from/to big/little endian)
//  \pin       a     The bytes to be swapped
//>
#define SWAP16(a)  ((u_int16) (((a >> 8) & 0xff) | ((a & 0xff) << 8)))


//< \def SWAP32(a)
//
//  Byte swap a 32 bit entity (eg from/to big/little endian)
//  \pin       a     The bytes to be swapped
//>
#define SWAP32(a)              \
      (((a >> 24) & 0xff)    | \
       ((a & 0xff0000) >> 8) | \
       ((a & 0xff00) << 8)   | \
       ((a & 0xff) << 24))

//< \def SWAP64(a)
//
//  Byte swap a 64 bit entity (eg from/to big/little endian)
//  \pin       a     The bytes to be swapped
//>
#define SWAP64(a) \
      (((a >> 56) & 0xff)                        | \
       ((a & UINT64_C_(0xff000000000000)) >> 40) | \
       ((a & UINT64_C_(0x00ff0000000000)) >> 24) | \
       ((a & UINT64_C_(0x0000ff00000000)) >> 8)  | \
       ((a & UINT64_C_(0x000000ff000000)) << 8)  | \
       ((a & UINT64_C_(0x00000000ff0000)) << 24) | \
       ((a & UINT64_C_(0x0000000000ff00)) << 40) | \
       ((a & 0xff) << 56))

#endif //__cplusplus


// Helper macros to convert to/from big-/little-endian formats.
// TO_XX_END converts from natural to either big or litle endian.
// FROM_XX_END converts from either big or litle endian to natural.
#if defined(LITTLE_ENDIAN_)
//< \def TO_BIG_END16(a)
//
//  Convert little-endian to big-endian format
//  \pin    a  16 bit source value
//>
#define TO_BIG_END16(a)       SWAP16(a)
//< \def TO_LITTLE_END16(a)
//
//  Convert little-endian to little-endian format.
//  This is a NOP for little-endian platforms.
//  \pin    a  16 bit source value
//>
#define TO_LITTLE_END16(a)    (a)
//< \def FROM_BIG_END16(a)
//
//  Convert big-endian to natural.
//  \pin    a  16 bit source value
//>
#define FROM_BIG_END16(a)     SWAP16(a)
//< \def FROM_LITTLE_END16(a)
//
//  Convert little-endian to natural.
//  This is a NOP for little-endian platforms.
//  \pin    a  16 bit source value
//>
#define FROM_LITTLE_END16(a)  (a)
//< \def TO_BIG_END32(a)
//
//  Convert little-endian to big-endian format.
//  \pin    a  32 bit source value
//>
#define TO_BIG_END32(a)       SWAP32(a)
//< \def TO_LITTLE_END32(a)
//
//  Convert little-endian to little-endian format.
//  This is a NOP for little-endian platforms.
//  \pin    a  32 bit source value
//>
#define TO_LITTLE_END32(a)    (a)
//< \def FROM_BIG_END32(a)
//
//  Convert big-endian to natural.
//  \pin    a  32 bit source value
//>
#define FROM_BIG_END32(a)     SWAP32(a)
//< \def FROM_LITTLE_END32(a)
//
//  Convert little-endian to natural.
//  This is a NOP for little-endian platforms.
//  \pin    a  32 bit source value
//>
#define FROM_LITTLE_END32(a)  (a)
//< \def TO_BIG_END64(a)
//
//  Convert little-endian to big-endian format.
//  \pin    a  64 bit source value
//>
#define TO_BIG_END64(a)       SWAP64(a)
//< \def TO_LITTLE_END64(a)
//
//  Convert little-endian to little-endian format.
//  This is a NOP for little-endian platforms.
//  \pin    a  64 bit source value
//>
#define TO_LITTLE_END64(a)    (a)
//< \def FROM_BIG_END64(a)
//
//  Convert big-endian to natural.
//  \pin    a  64 bit source value
//>
#define FROM_BIG_END64(a)     SWAP64(a)
//< \def FROM_LITTLE_END64(a)
//
//  Convert little-endian to natural.
//  This is a NOP for little-endian platforms.
//  \pin    a  64 bit source value
//>
#define FROM_LITTLE_END64(a)  (a)

#else // LITTLE_ENDIAN_

//< \def TO_BIG_END16(a)
//
//  Convert big-endian to big-endian format.
//  This is a NOP for big-endian platforms.
//  \pin    a  16 bit source value
//>
#define TO_BIG_END16(a)       (a)
//< \def TO_LITTLE_END16(a)
//
//  Convert big-endian to little-endian format.
//  \pin    a  16 bit source value
//>
#define TO_LITTLE_END16(a)    SWAP16(a)
//< \def FROM_BIG_END16(a)
//
//  Convert big-endian to natural.
//  This is a NOP for big-endian platforms.
//  \pin    a  16 bit source value
//>
#define FROM_BIG_END16(a)     (a)
//< \def FROM_LITTLE_END16(a)
//
//  Convert little-endian to natural.  
//  \pin    a  16 bit source value
//>
#define FROM_LITTLE_END16(a)  SWAP16(a)
//< \def TO_BIG_END32(a)
//
//  Convert big-endian to big-endian format.
//  This is a NOP for big-endian platforms.
//  \pin    a  32 bit source value
//>
#define TO_BIG_END32(a)       (a)
//< \def TO_LITTLE_END32(a)
//
//  Convert big-endian to little-endian format.
//  \pin    a  32 bit source value
//>
#define TO_LITTLE_END32(a)    SWAP32(a)
//< \def FROM_BIG_END32(a) 
//
//  Convert big-endian to natural.
//  This is a NOP for big-endian platforms.
//  \pin    a  32 bit source value
//>
#define FROM_BIG_END32(a)     (a)
//< \def FROM_LITTLE_END32(a)
//
//  Convert little-endian to natural.  
//  \pin    a  32 bit source value
//>
#define FROM_LITTLE_END32(a)  SWAP32(a)
//< \def TO_BIG_END64(a)
//
//  Convert big-endian to big-endian format.
//  This is a NOP for big-endian platforms.
//  \pin    a  64 bit source value
//>
#define TO_BIG_END64(a)       (a)
//< \def TO_LITTLE_END64(a)
//
//  Convert big-endian to little-endian format.  
//  \pin    a  64 bit source value
//>
#define TO_LITTLE_END64(a)    SWAP64(a)
//< \def FROM_BIG_END64(a)
//
//  Convert big-endian to natural.
//  This is a NOP for big-endian platforms.
//  \pin    a  64 bit source value
//>
#define FROM_BIG_END64(a)     (a)
//< \def FROM_LITTLE_END64(a)
//
//  Convert little-endian to natural.  
//  \pin    a  64 bit source value
//>
#define FROM_LITTLE_END64(a)  SWAP64(a)

#endif // LITTLE_ENDIAN_


//
//  Definitions to support both user and kernel modes
//


#if !defined(__WORDSIZE)         //Sometimes defined in inttypes.h
#if defined(ALPHA_TRU64_) || defined(SPARC_SOLARIS_) || defined(IPF_LINUX_)
//< \def __WORDSIZE
//
//  Word size is 64 on this platform
//>
#define __WORDSIZE  64
#elif defined(ALPHA_WIN32_) || defined(INTEL_WIN32_) || defined(X86_LINUX_)
//< \def __WORDSIZE
//
//  Word size is 32 on this platform
//>
#define __WORDSIZE  32
#else
#error The platform that you are using is not supported
#endif
#endif


#if !defined(__PRI64_PREFIX) && !defined(__PRI64_PREFIX)
/* the following are not defined in linux kernel, but sometimes in usermode */
/* temp definitions, from c99 (inttypes.h) */
/* The ISO C99 standard specifies that these macros must only be
   defined if explicitly requested.  */
#if !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS)

# if __WORDSIZE == 64
#  define __PRI64_PREFIX    "l"
#  define __PRIPTR_PREFIX    "l"
# else
#  define __PRI64_PREFIX    "ll"
#  define __PRIPTR_PREFIX
# endif

/* Macros for printing format specifiers.  */

/* Decimal notation.  */
# define PRId8        "d"
# define PRId16        "d"
# define PRId32        "d"
# define PRId64        __PRI64_PREFIX "d"

# define PRIdLEAST8    "d"
# define PRIdLEAST16    "d"
# define PRIdLEAST32    "d"
# define PRIdLEAST64    __PRI64_PREFIX "d"

# define PRIdFAST8    "d"
# define PRIdFAST16    "d"
# define PRIdFAST32    "d"
# define PRIdFAST64    __PRI64_PREFIX "d"


# define PRIi8        "i"
# define PRIi16        "i"
# define PRIi32        "i"
# define PRIi64        __PRI64_PREFIX "i"

# define PRIiLEAST8    "i"
# define PRIiLEAST16    "i"
# define PRIiLEAST32    "i"
# define PRIiLEAST64    __PRI64_PREFIX "i"

# define PRIiFAST8    "i"
# define PRIiFAST16    "i"
# define PRIiFAST32    "i"
# define PRIiFAST64    __PRI64_PREFIX "i"

/* Octal notation.  */
# define PRIo8        "o"
# define PRIo16        "o"
# define PRIo32        "o"
# define PRIo64        __PRI64_PREFIX "o"

# define PRIoLEAST8    "o"
# define PRIoLEAST16    "o"
# define PRIoLEAST32    "o"
# define PRIoLEAST64    __PRI64_PREFIX "o"

# define PRIoFAST8    "o"
# define PRIoFAST16    "o"
# define PRIoFAST32    "o"
# define PRIoFAST64    __PRI64_PREFIX "o"

/* Unsigned integers.  */
# define PRIu8        "u"
# define PRIu16        "u"
# define PRIu32        "u"
# define PRIu64        __PRI64_PREFIX "u"

# define PRIuLEAST8    "u"
# define PRIuLEAST16    "u"
# define PRIuLEAST32    "u"
# define PRIuLEAST64    __PRI64_PREFIX "u"

# define PRIuFAST8    "u"
# define PRIuFAST16    "u"
# define PRIuFAST32    "u"
# define PRIuFAST64    __PRI64_PREFIX "u"

/* lowercase hexadecimal notation.  */
# define PRIx8        "x"
# define PRIx16        "x"
# define PRIx32        "x"
# define PRIx64        __PRI64_PREFIX "x"

# define PRIxLEAST8    "x"
# define PRIxLEAST16    "x"
# define PRIxLEAST32    "x"
# define PRIxLEAST64    __PRI64_PREFIX "x"

# define PRIxFAST8    "x"
# define PRIxFAST16    "x"
# define PRIxFAST32    "x"
# define PRIxFAST64    __PRI64_PREFIX "x"

/* UPPERCASE hexadecimal notation.  */
# define PRIX8        "X"
# define PRIX16        "X"
# define PRIX32        "X"
# define PRIX64        __PRI64_PREFIX "X"

# define PRIXLEAST8    "X"
# define PRIXLEAST16    "X"
# define PRIXLEAST32    "X"
# define PRIXLEAST64    __PRI64_PREFIX "X"

# define PRIXFAST8    "X"
# define PRIXFAST16    "X"
# define PRIXFAST32    "X"
# define PRIXFAST64    __PRI64_PREFIX "X"


/* Macros for printing `intmax_t' and `uintmax_t'.  */
# define PRIdMAX    __PRI64_PREFIX "d"
# define PRIiMAX    __PRI64_PREFIX "i"
# define PRIoMAX    __PRI64_PREFIX "o"
# define PRIuMAX    __PRI64_PREFIX "u"
# define PRIxMAX    __PRI64_PREFIX "x"
# define PRIXMAX    __PRI64_PREFIX "X"


/* Macros for printing `intptr_t' and `uintptr_t'.  */
# define PRIdPTR    __PRIPTR_PREFIX "d"
# define PRIiPTR    __PRIPTR_PREFIX "i"
# define PRIoPTR    __PRIPTR_PREFIX "o"
# define PRIuPTR    __PRIPTR_PREFIX "u"
# define PRIxPTR    __PRIPTR_PREFIX "x"
# define PRIXPTR    __PRIPTR_PREFIX "X"


/* Macros for scanning format specifiers.  */

/* Signed decimal notation.  */
# define SCNd8        "hhd"
# define SCNd16        "hd"
# define SCNd32        "d"
# define SCNd64        __PRI64_PREFIX "d"

# define SCNdLEAST8    "hhd"
# define SCNdLEAST16    "hd"
# define SCNdLEAST32    "d"
# define SCNdLEAST64    __PRI64_PREFIX "d"

# define SCNdFAST8    "hhd"
# define SCNdFAST16    __PRIPTR_PREFIX "d"
# define SCNdFAST32    __PRIPTR_PREFIX "d"
# define SCNdFAST64    __PRI64_PREFIX "d"

/* Signed decimal notation.  */
# define SCNi8        "hhi"
# define SCNi16        "hi"
# define SCNi32        "i"
# define SCNi64        __PRI64_PREFIX "i"

# define SCNiLEAST8    "hhi"
# define SCNiLEAST16    "hi"
# define SCNiLEAST32    "i"
# define SCNiLEAST64    __PRI64_PREFIX "i"

# define SCNiFAST8    "hhi"
# define SCNiFAST16    __PRIPTR_PREFIX "i"
# define SCNiFAST32    __PRIPTR_PREFIX "i"
# define SCNiFAST64    __PRI64_PREFIX "i"

/* Unsigned decimal notation.  */
# define SCNu8        "hhu"
# define SCNu16        "hu"
# define SCNu32        "u"
# define SCNu64        __PRI64_PREFIX "u"

# define SCNuLEAST8    "hhu"
# define SCNuLEAST16    "hu"
# define SCNuLEAST32    "u"
# define SCNuLEAST64    __PRI64_PREFIX "u"

# define SCNuFAST8    "hhu"
# define SCNuFAST16    __PRIPTR_PREFIX "u"
# define SCNuFAST32    __PRIPTR_PREFIX "u"
# define SCNuFAST64    __PRI64_PREFIX "u"

/* Octal notation.  */
# define SCNo8        "hho"
# define SCNo16        "ho"
# define SCNo32        "o"
# define SCNo64        __PRI64_PREFIX "o"

# define SCNoLEAST8    "hho"
# define SCNoLEAST16    "ho"
# define SCNoLEAST32    "o"
# define SCNoLEAST64    __PRI64_PREFIX "o"

# define SCNoFAST8    "hho"
# define SCNoFAST16    __PRIPTR_PREFIX "o"
# define SCNoFAST32    __PRIPTR_PREFIX "o"
# define SCNoFAST64    __PRI64_PREFIX "o"

/* Hexadecimal notation.  */
# define SCNx8        "hhx"
# define SCNx16        "hx"
# define SCNx32        "x"
# define SCNx64        __PRI64_PREFIX "x"

# define SCNxLEAST8    "hhx"
# define SCNxLEAST16    "hx"
# define SCNxLEAST32    "x"
# define SCNxLEAST64    __PRI64_PREFIX "x"

# define SCNxFAST8    "hhx"
# define SCNxFAST16    __PRIPTR_PREFIX "x"
# define SCNxFAST32    __PRIPTR_PREFIX "x"
# define SCNxFAST64    __PRI64_PREFIX "x"


/* Macros for scanning `intmax_t' and `uintmax_t'.  */
# define SCNdMAX    __PRI64_PREFIX "d"
# define SCNiMAX    __PRI64_PREFIX "i"
# define SCNoMAX    __PRI64_PREFIX "o"
# define SCNuMAX    __PRI64_PREFIX "u"
# define SCNxMAX    __PRI64_PREFIX "x"

/* Macros for scaning `intptr_t' and `uintptr_t'.  */
# define SCNdPTR    __PRIPTR_PREFIX "d"
# define SCNiPTR    __PRIPTR_PREFIX "i"
# define SCNoPTR    __PRIPTR_PREFIX "o"
# define SCNuPTR    __PRIPTR_PREFIX "u"
# define SCNxPTR    __PRIPTR_PREFIX "x"

#endif    /* C++ && format macros */

#endif //__PRI64_PREFIX && __PRI64_PREFIX

//
//  End of user/kernel modes definitions
//

//< \def SUPRESS_NON_REF_ARGn(a, b)
//  Macro that performs a dummy assignment to avoid information-level message
//  about a parameter/argument never being referenced. This saves us from a
//  ugly/incompatible #pragma.
//
//                          >>> WARNING! <<<
//  In non-debug builds this does (or at least should) not generate any
//  extranous code overhead, unless 'a' is a class type with a ctor that is
//  non-inline and/or has other side effects. If this is the case, use this
//  macro with care, thoroughly analysing the generated code.
//  The more common approach to avoid these kinds of messages is to omit the
//  parameter name itself, leaving only the type. However, when using
//  automatic documentation generators, such as 'doxygen', that might not be
//  desired.
//
//  Currently the macros SUPRESS_NON_REF_ARG[1,2...6] are supported.
//
//  \pin a  Data type (might need qualifiers such as 'const' -- try!)
//  \pin b  Parameter/argument name
//>
#define SUPPRESS_NON_REF_ARGn(a, b)
#define SUPPRESS_NON_REF_ARG1(a, b) a dummy1__ = b; dummy1__ = dummy1__
#define SUPPRESS_NON_REF_ARG2(a, b) a dummy2__ = b; dummy2__ = dummy2__
#define SUPPRESS_NON_REF_ARG3(a, b) a dummy3__ = b; dummy3__ = dummy3__
#define SUPPRESS_NON_REF_ARG4(a, b) a dummy4__ = b; dummy4__ = dummy4__
#define SUPPRESS_NON_REF_ARG5(a, b) a dummy5__ = b; dummy5__ = dummy5__
#define SUPPRESS_NON_REF_ARG6(a, b) a dummy6__ = b; dummy6__ = dummy6__


#ifdef __cplusplus
}
#endif

#endif //HALPLATFORMDEFINITIONS_HXX
