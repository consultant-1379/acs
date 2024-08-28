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
//>

#ifndef PLATFORMTYPES_HXX
#define PLATFORMTYPES_HXX

//#pragma comment (user, "@(#)filerevision ")

#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4503 'identifier' : decorated name length exceeded, name was truncated
// 4786 'identifier' : identifier was truncated to 'number' characters in the
//                     debug information.
#pragma warning ( disable : 4503 4786 )
#endif // _MSC_VER
#ifdef __DECCXX
// Suppress the following information-level messages when compiling with cxx
// together with the -w0 switch:
// - "incompatible enum comparison 'identifier'"
//   Comparing enums of different types is improper by definition.
//   However, due to constraints in some compilers (e.g. MSVC6), the construct
//   'static const type name = value' may not be fully supported for declaring
//   class specific compile-time constant values, and thus enums must still be used.
#pragma message disable incenucom
// - "conversion to integral type of smaller size could lose data"
//   In most cases reduction of size is intended (=safe) and does not need
//   to be reported. Trying to resolve each and every occurance of this
//   using casts etc. might not be wise either (e.g. if an interface changed
//   to a greater size, the loss of data would never be detected).
//   This message defaults to being disabled. When debugging known issues
//   it may be enabled as a tool to add in the process of trouble-shooting.
#pragma message disable intconlosbit

// XXX The below needs to further investigated (suppressed until then)
#pragma message disable calfunredinl
#pragma message disable basclsnondto
#endif // __DECCXX


#include "ACS_CS_halPlatformDefinitions.h"


//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

// Macro for supporting Compile-Time-Assertions, use just like a regular 
// assert, e.g. CompileTimeAssert(sizeof(int) == 4);.
// This code below is "stolen with pride" from the 'C++ Boost' source
// library distributions, see copyright notice below.
//
//    (C) Copyright Steve Cleary & John Maddock 2000.
//    Permission to copy, use, modify, sell and
//    distribute this software is granted provided this copyright notice
//    appears in all copies. This software is provided "as is" without 
//    express or implied warranty, and with no claim as to its suitability
//    for any purpose.
//
// The implementation supplies a single macro BOOST_STATIC_ASSERT(expr),
// which generates a compile time error message if the 
// integral-constant-expression 'expr' is not true. In other words it is the
// compile time equivalent of the assert macro; this is sometimes known as a 
// "Compile-Time-Assertion". Note that if the condition is true, then the
// macro will generate neither code nor data - and the macro can also be used
// at either namespace, class or function scope. When used in a template, the
// static assertion will be evaluated at the time the template is instantiated;
// this is particularly useful for validating template parameters.
//
// The key feature is that the error message triggered by the undefined 
// expression sizeof(STATIC_ASSERTION_FAILURE<0>), tends to be consistent
// across a wide variety of compilers. The rest of the machinery of
// BOOST_STATIC_ASSERT is just a way to feed the sizeof expression into a
// typedef. The use of a macro here is somewhat ugly; however boost members
// have spent considerable effort trying to invent a static assert that
// avoided macros, all to no avail. The general conclusion was that the good
// of a static assert working at namespace, function, and class scope 
// outweighed the ugliness of a macro.
//
// For further information see www.boost.org.
//
// Known Bugs/Limitations:
// When using strings in expression a variety of compilation errors may
// be generated depending of what compiler is being used and is thus not 
// officially supported by this macro. It may work but on the other hand
// it might not!

// We use "COMPILE_TIME_ASSERTION_FAILURE" as the type name here to generate
// an eye catching error message.
#if defined ( __cplusplus )
//namespace boost
//{
//   //<
//   // Template struct that is part of machinery for compiletime assert
//   //>
//   template<bool> struct COMPILE_TIME_ASSERTION_FAILURE;
//   //<
//   // Template struct specialization that is part of machinery for compiletime assert
//   //>
//   template<> struct COMPILE_TIME_ASSERTION_FAILURE<true>{};
//   //<
//   // Template struct that is part of machinery for compiletime assert
//   //>
//   template<int> struct static_assert_test{};
//}

#ifndef BOOST_BUGGY_INTEGRAL_CONSTANT_EXPRESSIONS
#ifndef _MSC_VER
//< \def BOOST_STATIC_ASSERT
// Macro that is part of machimery for compile time assert
//>
#ifndef BOOST_STATIC_ASSERT
#define BOOST_STATIC_ASSERT(expr)\
   typedef ::boost::static_assert_test<\
      sizeof(::boost::COMPILE_TIME_ASSERTION_FAILURE<(expr)>)>\
         BOOST_ASSERT_JOIN(boost_static_assert_typedef_, __LINE__)
#endif
#else
// __LINE__ macro broken when -ZI is used see Q199057
// fortunately MSVC ignores duplicate typedef's:
//< \def BOOST_STATIC_ASSERT
// Macro that is part of machimery for compile time assert
//>
#define BOOST_STATIC_ASSERT(expr)\
   typedef ::boost::static_assert_test<\
      sizeof(::boost::COMPILE_TIME_ASSERTION_FAILURE<(expr)>)\
      > boost_static_assert_typedef_
#endif
#else
// Alternative enum based implementation; does *not* work very well
// together with MSVC
//< \def BOOST_STATIC_ASSERT
// Macro that is part of machimery for compile time assert
//>
#define BOOST_STATIC_ASSERT(expr)\
   enum { BOOST_ASSERT_JOIN(boost_static_assert_enum_, __LINE__)\
      = sizeof(::boost::COMPILE_TIME_ASSERTION_FAILURE<(expr)>) }
#endif

//< \def CompileTimeAssert
//
// Macro for supporting Compile-Time-Assertions, use just like a regular 
// assert, e.g. CompileTimeAssert(sizeof(int) == 4);.
//
// When using strings in expression a variety of compilation errors may
// be generated depending of what compiler is being used and is thus not 
// officially supported by this macro. It may work but on the other hand
// it might not!
//>
#define CompileTimeAssert BOOST_STATIC_ASSERT

// The piece of macro magic joins the two arguments together, even
// when one of the arguments is itself a macro (see 16.3.1 in C++ standard). 
// The key is that macro expantion of macro arguments does not occur in 
// BOOST_DO_ASSERT_JOIN but does in BOOST_ASSERT_JOIN provided it is called 
// from within another macro.
//< \def BOOST_ASSERT_JOIN
// Macro that is part of machimery for compile time assert
//>
#define BOOST_ASSERT_JOIN(X, Y) BOOST_DO_ASSERT_JOIN(X, Y)
//< \def BOOST_DO_ASSERT_JOIN
// Macro that is part of machimery for compile time assert
//>
#define BOOST_DO_ASSERT_JOIN(X, Y) X##Y
#else // #if defined( __cplusplus )

//< \def CompileTimeAssert
//
//  Empty macro for code compatibility in non C++ environments
//>
#define CompileTimeAssert(expr)
#ifdef _MSC_VER
#pragma message("Warning: CompileTimeAssertion is supported by C++ only")
#endif
#endif // #if defined( __cplusplus )

#if defined ( __cplusplus )
// To simplify most other header files we include iostream and iomanip here.
// We also do a  "use namespace std;" statement as the APZ_VM was
// originally written using non-std iostream for compatibility with the old
// AXE VM code. Adding all the needed std:: qualifications is a lot of work
// for little gain. The VM is quite small so name collisions can be avoided
// in spite of importing the std names into the global namespace.
//
// This centralizes the statements needed to get access to iostreams
// and the idividual hxx/cxx files do not need to include iostream (if
// they include PlatformTypes.hxx).
// Other stream header files need to be included when needed.
#include <iostream> 
#include <iomanip>
using namespace std;

// Debug printout macros. Printouts produced if _DEBUG is defined,
// NOP if _debug is not defined.
#ifdef _DEBUG
//< \def APZcout
//
//  Alias for the standard C++ "cout", printout is enabled.
//>
#define APZcout cout

//< \def APZcerr
//
//  Alias for the standard C++ "cerr", printout is enabled.
//>
#define APZcerr cerr
#else // _DEBUG
// The cout (iostream) definitions are still needed even if we 
// will never call the functions/methods.
#ifdef __DECCXX
#pragma message disable codcauunr
#pragma message disable boolexprconst
#endif // __DECCXX

//< \def APZcout
//
//  Alias for the standard C++ "cout", printout is disabled.
//>
#define APZcout while(0) cout

//< \def APZcerr
//
//  Alias for the standard C++ "cerr", printout is disabled.
//>
#define APZcerr while(0) cerr

#endif // _DEBUG

//<
//  Part of machinery to implement DEPRECATED__
//>
inline void deprecated_dummy__(char){}
//< \def DEPRECATED__(v)
//
//  Produce an anoying "truncation" warning when invoked. The exact
//  message displayed (if any) may vary depending on compiler and
//  current configuration. 
//  \pin    v  Value to be assigned to a possible 'lvalue'
//>
#define DEPRECATED__(v) (deprecated_dummy__(256), (v))
#endif // #if defined( __cplusplus )

//< \def SUPRESS_NON_REF_ARGn(a, b)
//  Macro that performs a dummy assignment to avoid information-level message 
//  about a parameter/argument never being referenced. This saves us from a
//  ugly/incompatible pragma directive.
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
#define SUPRESS_NON_REF_ARGn(a, b) 
//< \def SUPRESS_NON_REF_ARG1(a, b)
//  See description of SUPRESS_NON_REF_ARGn
//>
#define SUPRESS_NON_REF_ARG1(a, b) a dummy1__ = b; dummy1__ = dummy1__
//< \def SUPRESS_NON_REF_ARG2(a, b)
//  See description of SUPRESS_NON_REF_ARGn
//>
#define SUPRESS_NON_REF_ARG2(a, b) a dummy2__ = b; dummy2__ = dummy2__
//< \def SUPRESS_NON_REF_ARG3(a, b)
//  See description of SUPRESS_NON_REF_ARGn
//>
#define SUPRESS_NON_REF_ARG3(a, b) a dummy3__ = b; dummy3__ = dummy3__
//< \def SUPRESS_NON_REF_ARG4(a, b)
//  See description of SUPRESS_NON_REF_ARGn
//>
#define SUPRESS_NON_REF_ARG4(a, b) a dummy4__ = b; dummy4__ = dummy4__
//< \def SUPRESS_NON_REF_ARG5(a, b)
//  See description of SUPRESS_NON_REF_ARGn
//>
#define SUPRESS_NON_REF_ARG5(a, b) a dummy5__ = b; dummy5__ = dummy5__
//< \def SUPRESS_NON_REF_ARG6(a, b)
//  See description of SUPRESS_NON_REF_ARGn
//>
#define SUPRESS_NON_REF_ARG6(a, b) a dummy6__ = b; dummy6__ = dummy6__

#endif // PLATFORMTYPES_HXX
