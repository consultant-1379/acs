#ifndef HEADER_GUARD_FILE__acs_nclib_macros
#define HEADER_GUARD_FILE__acs_nclib_macros

/** @file acs_nclib_macros.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-10-01
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-10-01 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

/*
 * Macro utilities
 */
#ifndef ACS_NCLIB_ARRAY_SIZE
#	define ACS_NCLIB_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif

#ifndef ACS_NCLIB_STRINGIZE
#	define ACS_NCLIB_STRINGIZER(s) #s
#	define ACS_NCLIB_STRINGIZE(s) ACS_NCLIB_STRINGIZER(s)
#endif

#ifndef ACS_NCLIB_TRY_CALL_IMPLEMENTATION
#	define ACS_NCLIB_TRY_CALL_IMPLEMENTATION(impl_pointer, func, ...) ((impl_pointer) ? ((impl_pointer)->func(__VA_ARGS__)) : acs_nclib::ERR_SIGNALS_ERROR)
#endif

#ifndef ACS_NCLIB_ERROR_INFO_DECLARE_ACCESSORS
#define ACS_NCLIB_ERROR_INFO_DECLARE_ACCESSORS \
	int last_error_code () const; \
	const char * last_error_text () const
#endif

#ifndef ACS_NCLIB_ERROR_INFO_DEFINE_ACCESSORS
#	define ACS_NCLIB_ERROR_INFO_DEFINE_ACCESSORS(impl_pointer, impl_pointer2, impl_pointer3) \
	int __CLASS_NAME__::last_error_code () const { \
		ACS_NCLIB_TRACE_FUNCTION; \
		if ((impl_pointer)) return (impl_pointer)->last_error_code(); \
		else if ((impl_pointer2)) return (impl_pointer2)->last_error_code(); \
		else if ((impl_pointer3)) return (impl_pointer3)->last_error_code(); \
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE; \
	} \
	const char * __CLASS_NAME__::last_error_text () const { \
		ACS_NCLIB_TRACE_FUNCTION; \
		if ((impl_pointer)) return (impl_pointer)->last_error_text(); \
		else if ((impl_pointer2)) return (impl_pointer2)->last_error_text(); \
		else if ((impl_pointer3)) return (impl_pointer3)->last_error_text(); \
		return acs_nclib::ERRTEXT_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE; \
	}
#endif

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

#endif /* HEADER_GUARD_FILE__acs_nclib_macros */
