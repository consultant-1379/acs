#ifndef HEADER_GUARD_FILE__acs_aca_macros
#define HEADER_GUARD_FILE__acs_aca_macros acs_aca_macros.h

/** @file acs_aca_macros.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2013-05-10
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
 *	| R-001 | 2013-05-10 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#if !defined (ACS_ACA_ARRAY_SIZE)
#	define ACS_ACA_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif

#define ACS_ACA_CONDITIONAL_PROCEDURE_CALL(cond, proc, ...) if (cond) proc(__VA_ARGS__)

#if !defined (ACS_ACA_STRINGIZER)
#	define ACS_ACA_STRINGIZER(s) #s
#	define ACS_ACA_STRINGIZE(s) ACS_ACA_STRINGIZER(s)
#endif


#if !defined ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN
#	define	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(sync) { ACE_Guard<ACE_Recursive_Thread_Mutex> ___guard__(sync, true)
#	define	ACS_ACA_CRITICAL_SECTION_GUARD_END()	}
#endif

#endif // HEADER_GUARD_FILE__acs_aca_macros

