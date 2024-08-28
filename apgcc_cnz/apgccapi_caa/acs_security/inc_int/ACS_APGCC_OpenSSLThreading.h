#ifndef ACS_APGCC_OPENSSLTHREADING_H_
#define ACS_APGCC_OPENSSLTHREADING_H_
/**
 *	@file ACS_APGCC_OpenSSLThreading.h
 *	@brief
 *	@author ealocae (Alessio Cascone)
 *	@date 2016-04-05
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2016
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| PA1    | 2016-04-05 | ealocae      | First draft.                        |
 *	+========+============+==============+=====================================+
 */

#include <pthread.h>

#include "ACS_APGCC_CLibTypes.h"

class ACS_APGCC_OpenSSLThreading {
private:
	// IMPORTANT: Making private the constructors and the assignment operator, since this class cannot be instantiated!
	ACS_APGCC_OpenSSLThreading ();
	ACS_APGCC_OpenSSLThreading (const ACS_APGCC_OpenSSLThreading & rhs);
	ACS_APGCC_OpenSSLThreading & operator= (const ACS_APGCC_OpenSSLThreading & rhs);

public:
    static ACS_APGCC_ThreadSupport_ReturnTypeT register_threading_callbacks_for_openssl ();
    static ACS_APGCC_ThreadSupport_ReturnTypeT unregister_threading_callbacks_for_openssl ();

private:
    static unsigned long openssl_threading_id_callback ();
    static void openssl_threading_locking_callback (int mode, int i, const char * file, int line);

private:
	static pthread_mutex_t * _openssl_threading_mutexes_pool;
};

#endif
