/*=================================================================== */
/**
   @file   global.h

   @brief  Header file for DSD stubs for APJTP.

		   This file contains global definitions used within DSD.
           .
   @version N.N.N

   @documentno CAA 109 0870

   @copyright Ericsson AB, Sweden 2010. All rights reserved.

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/10/2010     TA        APG43 on Linux.
==================================================================== */

#ifndef GLOBAL_H_
#define GLOBAL_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "ace/ACE.h"


const ACE_UINT32 MAX_NAME_LENGTH = 31;
const ACE_UINT32 MAX_DOMAIN_LENGTH = 7;

const ACE_TCHAR* const OWN_IPADDR_FAILURE = "Cannot translate AP name into IP-Addresses";
const ACE_TCHAR* const ILLEGAL_USE_API = "Illegal use of API";
const ACE_TCHAR* const INTERNAL_ERROR = "Internal Expected Error";
const ACE_TCHAR* const PUBLISH_FAILED = "Publish of service in daemon failed";
const ACE_TCHAR* const CONNECT_TIMEOUT = "Connect attempt timed out";
const ACE_TCHAR* const NULL_ARGUMENT_ERROR = "Null argument received in parameters";




/*=================================================================== */
	/**

		@brief			This method converts the short value to AXE format.

		@pre			None

		@post			None

		@param			ACE_INT16 I
						Short value which is to be converted into AXE format.

		@return			AXE value.

		@exception		None
	*/
/*=================================================================== */
inline ACE_INT16 short2AXE(ACE_INT16 I){
	I = htons(I);
	ACE_INT16 O;
	ACE_INT8* In = (ACE_INT8*) &I;
	ACE_INT8* Out= (ACE_INT8*) &O;

	Out[0] = In[1];
	Out[1] = In[0];

	return O;
}



#endif /* GLOBAL_H_ */
