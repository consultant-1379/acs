//******************************************************************************
//
//  NAME
//     acs_lm_jtp_service_R2.cpp
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
#include "acs_lm_jtp_service_R2.h"
#include <iostream>
using namespace std;
#if 0
ACS_LM_DefineTrace(ACS_LM_JTP_Service_R2);
#endif
/*=================================================================
	ROUTINE: ACS_LM_JTP_Service_R2 constructor
=================================================================== */
ACS_LM_JTP_Service_R2::ACS_LM_JTP_Service_R2()
:jtpService(NULL)
{
#if 0
    ACS_LM_PrintTrace(ACS_LM_JTP_Service_R2, "ACS_LM_JTP_Service_R2()");
#endif
    //cout<<"In ACS_LM_JTP_Service_R2::ACS_LM_JTP_Service_R2() constructor "<<endl;
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_JTP_Service_R2 destructor
=================================================================== */
ACS_LM_JTP_Service_R2::~ACS_LM_JTP_Service_R2()
{
//	if (jtpService != NULL)
//	{
//	delete jtpService;
//	jtpService= NULL;
//	}
#if 0
    ACS_LM_PrintTrace(ACS_LM_JTP_Service_R2, "~ACS_LM_JTP_Service_R2()");
#endif
    //cout<<"In ACS_LM_JTP_Service_R2::~ACS_LM_JTP_Service_R2() destructor "<<endl;
}//end of destructor
/*=================================================================
	ROUTINE: registerService
=================================================================== */
bool ACS_LM_JTP_Service_R2::registerService(const std::string &apService)
{
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Service_R2, "registerService()");
#endif
	//cout<<"In ACS_LM_JTP_Service_R2::registerService method "<<endl;
	bool registered = false;
	if(jtpService == NULL)
	{
		jtpService = new ACS_JTP_Service_R2A((char*)apService.c_str());

		if(jtpService != NULL)
		{
			//DEBUG("%s","registerService() jtpService is not null ");
			if(!jtpService->jidrepreq())
			{
				//cout<<"registerService() jtpService->jidrepreq() failed"<<endl;
				//DEBUG("%s","registerService() jtpService->jidrepreq() failed");
				delete jtpService;
				jtpService=NULL;
				return registered;
			}
			else
			{
				//DEBUG("%s","registerService() jtpService->jidrepreq() success");
				registered = true;
			}
		}
		else
		{
			//DEBUG("%s","registerService() jtpService is null - return false");
			registered = false;
		}
	}
	else
	{
		//DEBUG("%s","registerService() jtpService is already registered- return true");
		registered = true;
	}

#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Service_R2, "registerService() registered="<<registered);
#endif
	//cout<<"registerService() registered="<<registered<<endl;
	return registered;
}//end of registerService
/*=================================================================
	ROUTINE: getHandles
=================================================================== */
void ACS_LM_JTP_Service_R2::getHandles(int& noOfHandles, ACE_HANDLE*& handles)
{
    jtpService->getHandles(noOfHandles, handles);
}//end of getHandles
/*=================================================================
	ROUTINE: accept
=================================================================== */
JtpNode* ACS_LM_JTP_Service_R2::accept()
{
#if 0
    ACS_LM_PrintTrace(ACS_LM_JTP_Service_R2, "accept()");
#endif
    //cout<<"In ACS_LM_JTP_Service_R2::accept method"<<endl;

    ACS_JTP_Job_R2A* jtpJob = new ACS_JTP_Job_R2A();
    //bool accepted =
	jtpService->accept(jtpJob, 0);
	unsigned short ud1 = 0;
	unsigned short ud2 = 0;
    //unsigned short ret = 0;
    //unsigned short rret = 0;
//	unsigned short rud1 = 0;
//	unsigned short rud2 = 0;
    unsigned short len = 512;
    char* msg;
	if(jtpJob->jinitind(ud1, ud2, len, msg))
	{
		if(jtpJob->jinitrsp(ud1, ud2, 0))
		{
//			ACS_LM_PrintTrace(ACS_LM_JTP_Service_R2, "accept(): jinitrsp success" );
		}
	}
    delete jtpJob;
    return NULL;
}//end of accept
