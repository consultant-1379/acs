//****************************************************************************
//
//  NAME
//     ACS_LM_JTP_Job.h
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

#ifndef _ACS_LM_JTP_SERVICE_H_
#define _ACS_LM_JTP_SERVICE_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include<ace/Event.h>
#include "acs_lm_common.h"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_JTP_Service
{
public:
	/*===================================================================
			   				 PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
			   				PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
	 *  @brief accept
	 *
	 *  @return JtpNode
	 **/
	/*=================================================================== */
	virtual JtpNode* accept() = 0;
	/*=================================================================== */
	/**
	 *  @brief getHandles
	 *
	 *  @param noOfHandles 					: int
	 *
	 *  @param handles 						: ACE_HANDLE
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	virtual void getHandles(int& noOfHandles, ACE_HANDLE*& handles) = 0;
	/*=================================================================== */
	/**
	 *  @brief registerService
	 *
	 *  @param apService 					: const string
	 *
	 *  @return bool 	 					: true/false
	 **/
	/*=================================================================== */
	virtual bool registerService(const std::string& apService) = 0;
	/*=================================================================== */
	/**
	 *  @brief create
	 *
	 *  @param hwVersion 					: int
	 *
	 *  @return ACS_LM_JTP_Service
	 **/
	/*=================================================================== */
	static ACS_LM_JTP_Service* create(int hwVersion);
};

#endif
