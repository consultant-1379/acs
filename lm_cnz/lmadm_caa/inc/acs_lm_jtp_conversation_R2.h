//****************************************************************************
//
//  NAME
//     ACS_LM_JTP_Conversation_R2.h
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

#ifndef _ACS_LM_JTP_CONVERSATION_R2_H_
#define _ACS_LM_JTP_CONVERSATION_R2_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_jtp_conversation.h"
#include <ACS_JTP_Conversation_R2A.h>
#include <ACS_JTP_Conversation_R3A.h>
#include "acs_lm_eventhandler.h"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_JTP_Conversation_R2 : public ACS_LM_JTP_Conversation
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
					@brief       Default constructor for ACS_LM_JTP_Conversation_R2

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	ACS_LM_JTP_Conversation_R2();
	/*=================================================================== */
	/**
					@brief       Default destructor for ACS_LM_JTP_Conversation_R2

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_JTP_Conversation_R2();
	/*=================================================================== */
	/**
	 *  @brief connect
	 *
	 *  @param cpService					: string
	 *
	 *  @param node 						: JtpNode
	 *
	 *  @return bool						: true/false
	 **/
	/*=================================================================== */
	virtual bool connect(const std::string& cpService, JtpNode* node=NULL);
	/*=================================================================== */
	/**
	 *  @brief send
	 *
	 *  @param lkDataList					: list

	 *  @param lmMode 						: int
	 *
	 *  @return bool						: true/false
	 **/
	/*=================================================================== */
	//virtual bool send(const std::list<LkData*>& lkDataList, int lmMode);
	 virtual bool send(const std::list<LkData*>& lkDataList, int lmMode,const std::list<LkData*>& discLklist = std::list<LkData*>(),const std::list<LkData*>& connLklist = std::list<LkData*>());//Lmcleanup

	/*=================================================================== */
	/**
	 *  @brief disconnect
	 *
	 *  @return bool						: true/false
	 **/
	/*=================================================================== */
	virtual bool disconnect();



private:
	/*===================================================================
										 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
										 PRIVATE METHOD
	=================================================================== */
	ACS_JTP_Conversation_R2A* jtpConv;
	//LMcleaup  Start
	std::string cpService;
	/*=================================================================== */
	/**
	 *  @brief connect
	 *
	 *  @return bool						: true/false
	 **/
	/*=================================================================== */
	bool connect(unsigned short &proto);
	/*=================================================================== */
	/**
	 *  @brief senddata
	 *
	 * 	@param  lkDataList					:list
	 *
	 * 	@param  discLklist					:list
	 *
	 *  @return bool						: true/false
	 **/
	/*=================================================================== */
	bool senddata(std::list<Buffer32*>& lkDataList,std::list<Buffer32*> discLklist = std::list<Buffer32*>(), std::list<Buffer32*> connLklist = std::list<Buffer32*>());
	/*=================================================================== */
	/**
	 *  @brief clearbuffers
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void clearbuffers();
	std::list<Buffer32*> buffer32List ;
	std::list<Buffer32*> discbuffer32List ;   //LMcleaup
	std::list<Buffer32*> connbuffer32List ;   //LMcleaup

	//LMcleaup  start

	//Define functionCodes
	enum functionCode
	{
		Notification,
		Connection,
		Disconnection,
		Not_Used
	};

	/*=================================================================== */
	/**
	 *  @brief send
	 *
	 * 	@param  bufferList					:list
	 *
	 *  @return bool						: true/false
	 **/
	/*=================================================================== */
	bool send(std::list<Buffer32*>& bufferList, functionCode code = Notification);
	//LMcleaup  End

};
#endif
