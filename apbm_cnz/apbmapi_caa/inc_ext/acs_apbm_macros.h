/*
 * acs_apbm_macros.h
 *
 *  Created on: Sep 21, 2011
 *      Author: xgiufer
 */

#ifndef HEADER_GUARD_FILE__acs_apbm_macros
#define HEADER_GUARD_FILE__acs_apbm_macros acs_apbm_macros.h

#include "acs_apbm_macrosconstants.h"
#include <acs_apbm_macrosconfig.h>


/*
 * Macro utilities
 */
#define ACS_APBM_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#define ACS_APBM_CONDITIONAL_PROCEDURE_CALL(cond, proc, ...) if (cond) proc(__VA_ARGS__)

#define ACS_APBM_STRINGIZER(s) #s
#define ACS_APBM_STRINGIZE(s) ACS_APBM_STRINGIZER(s)


/* TODO - Tracing macros */


#endif /* HEADER_GUARD_FILE__acs_apbm_macros */
