//========================================================================================
//
// NAME
//		ACS_TRAPDS_API.H - API towards the TRAPDS functionality
//
// COPYRIGHT
//		 Ericsson AB 2007 - All Rights Reserved.
//
//		The Copyright to the computer program(s) herein is the
//		property of Ericsson AB, Sweden. The program(s) may be
//		used and/or copied only with the written permission from
//		Ericsson AB or in accordance with the terms and conditions
//		stipulated in the agreement/contract under which the
//		program(s) have been supplied.
//
// DESCRIPTION
//		This header file contains the public interface to TRAPDS.

//
// ERROR HANDLING
//		Errors are indicated by the return value of the functions in the API
//
// DOCUMENT NO
//
//
// AUTHOR
//		2012-02-01 by EANFORM
//
// REVISION
//		R1A
//
// CHANGES
//
// RELEASE REVISION HISTORY
//		REV NO	DATE 	 NAME		     DESCRIPTION
//		R1A		061218 EANFORM      First version for APG43 on Linux
//
// LINKAGE
//		libACS_TRAPDSAPI.so
//
// SEE ALSO
//		-
//******************************************************************************

#ifndef ACS_TRAPDS_API_H_
#define ACS_TRAPDS_API_H_


//#include "ACS_TRAPDS_API_R1.h"
#include "ACS_TRAPDS_API_R2.h"

//typedef ACS_TRAPDS_API_R1 ACS_TRAPDS_API;
typedef ACS_TRAPDS_API_R2 ACS_TRAPDS_API;


#endif /* ACS_TRAPDS_API_H_ */
