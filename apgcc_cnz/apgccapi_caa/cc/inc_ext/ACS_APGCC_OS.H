/*
 * APGCC_OS.H
 *
 *  Created on: Apr 2, 2010
 *      Author: root
 */

#ifndef ACS_APGCC_OS_H_
#define ACS_APGCC_OS_H_


#define ACS_APGCC_Export

#define ACS_APGCC_NL			"\n"		// NewLine constants
#define ACS_APGCC_NL_SIZE	1
#define ACS_APGCC_INVALID_HANDLE -1		// Not valid file descriptor

typedef int APGCC_HANDLE;

#include /**/ "ace/pre.h"
#include "ace/Event_Handler.h"


//typedef siginfo siginfo_t;

// data types
typedef unsigned int u_int;


#endif /* ACS_APGCC_OS_H_ */
