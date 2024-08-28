/*
 * acs_bur_trace.h
 *
 *  Created on: Jul 22, 2011
 *      Author: enungai
 */

#ifndef ACS_BUR_TRACE_H_
#define ACS_BUR_TRACE_H_
#include <ace/ACE.h>

class ACS_TRA_trace;


// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

#define TRACE(TRACE_CLASS, FMT, ...) \
		trautil_trace(TRACE_CLASS, FMT, __VA_ARGS__);

/*
 typedef enum {
	FMS_CPF_SUCCESS = 0,
	FMS_CPF_FAILURE = -1
}FMS_CPF_ReturnType;
*/

void trautil_trace(ACS_TRA_trace* trace_class, const ACE_TCHAR* messageFormat, ...);

#endif /* ACS_BUR_TRACE_H_ */
