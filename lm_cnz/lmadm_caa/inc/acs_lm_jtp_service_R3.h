//****************************************************************************
//
//  NAME
//     ACS_LM_JTP_Job_R3.h
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

#ifndef _ACS_LM_JTP_SERVICE_R3_H_
#define _ACS_LM_JTP_SERVICE_R3_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include<ace/Event.h>
#include "acs_lm_jtp_service.h"
#include <ACS_JTP_Service_R3A.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_JTP_Service_R3 : public ACS_LM_JTP_Service
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*===================================================================
	   						  PUBLIC ATTRIBUTE
	 =================================================================== */

	/*===================================================================
	   							   PUBLIC METHOD
	 =================================================================== */
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_LM_JTP_Service_R3

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_JTP_Service_R3();
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_JTP_Service_R3

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_JTP_Service_R3();
	/*=================================================================== */
	/**
	 *  @brief getHandles
	 *
	 *  @param noOfHandles						: int
	 *
	 *  @param handles 							: ACE_HANDLE
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	virtual void getHandles(int& noOfHandles, ACE_HANDLE*& handles);
	/*=================================================================== */
	/**
	 *  @brief accept
	 *
	 *  @return JtpNode
	 **/
	/*=================================================================== */
	virtual JtpNode* accept();
	/*=================================================================== */
	/**
	 *  @brief registerService
	 *
	 *  @param apService					: const string
	 *
	 *  @return bool
	 **/
	/*=================================================================== */
	virtual bool registerService(const std::string& apService);
	/*=================================================================== */

private:
	/*===================================================================
									 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
									 PRIVATE METHOD
	=================================================================== */
	ACS_JTP_Service_R3A* jtpService;
};

#endif
