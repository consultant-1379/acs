//****************************************************************************
//
//  NAME
//     ACS_LM_JTP_Job_R2.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson  AB, Sweden.
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
//	    190 89-CAA XXX XXXX
//
//  AUTHOR 
//     2008-12-08 by XCSRPAD PA1
//
//  SEE ALSO 
//     -
//
//****************************************************************************

#ifndef _ACS_LM_JTP_SERVICE_R2_H_
#define _ACS_LM_JTP_SERVICE_R2_H_

#include <ace/ACE.h>
#include<ace/Event.h>
#include "acs_lm_jtp_service.h"
#include <ACS_JTP_Service_R2A.h>


class ACS_LM_JTP_Service_R2 : public ACS_LM_JTP_Service
{
public:
	ACS_LM_JTP_Service_R2();
	~ACS_LM_JTP_Service_R2();

    virtual void getHandles(int& noOfHandles, ACE_HANDLE*& handles);
    virtual JtpNode* accept();
    virtual bool registerService(const std::string& apService);

private:
	ACS_JTP_Service_R2A* jtpService;
};

#endif
