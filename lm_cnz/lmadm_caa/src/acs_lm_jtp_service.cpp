//******************************************************************************
//
//  NAME
//     acs_lm_jtp_service.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_lm_jtp_service.h"
#include "acs_lm_jtp_service_R2.h"
#include "acs_lm_jtp_service_R3.h"
#if 0
ACS_LM_DefineTrace(ACS_LM_JTP_Service);
#endif
/*=================================================================
	ROUTINE: create
=================================================================== */
ACS_LM_JTP_Service* ACS_LM_JTP_Service::create(int jtpVersion)
{	
	ACS_LM_JTP_Service* jtpService = NULL;

	if(jtpVersion == 3)
    {
#if 0
        ACS_LM_PrintTrace(ACS_LM_JTP_Service, "create(): ACS_LM_JTP_Service_R3 is created");
#endif
      //cout<< "create(): ACS_LM_JTP_Service_R3 is created";
        jtpService = new ACS_LM_JTP_Service_R3();
    }
    else
    {
#if 0
        ACS_LM_PrintTrace(ACS_LM_JTP_Service, "create(): ACS_LM_JTP_Service_R2 is created");
#endif
        //cout<< "create(): ACS_LM_JTP_Service_R2 is created"<<endl;
        jtpService = new ACS_LM_JTP_Service_R2();
    }

    return jtpService;
}//end of create
