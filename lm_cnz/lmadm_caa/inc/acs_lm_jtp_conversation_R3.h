//****************************************************************************
//
//  NAME
//     ACS_LM_JTP_Conversation_R3.h
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

#ifndef _ACS_LM_JTP_CONVERSATION_R3_H_
#define _ACS_LM_JTP_CONVERSATION_R3_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include<ace/Event.h>
#include "acs_lm_common.h"
#include <ACS_JTP.h>
#include "acs_lm_jtp_conversation.h"
#include <ACS_JTP_Conversation_R3A.h>
#include "acs_lm_eventhandler.h"
#include <acs_lm_cpdata.h>
#include <acs_lm_senddata.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_JTP_Conversation_R3 : public ACS_LM_JTP_Conversation
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
	struct SendThreadData
	{
		ACE_Event * stopEvent;
		std::list<Buffer32*> bufferList;
		std::list<Buffer32*> discbufferList; //LMcleaup
		std::list<Buffer32*> connbufferList; //LMcleaup
		ACS_JTP_Conversation_R3A::JTP_Node* node;
		int retries;
		std::string cpService;
	};
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_LM_JTP_Conversation_R3

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_JTP_Conversation_R3(void);
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_JTP_Conversation_R3

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	virtual ~ACS_LM_JTP_Conversation_R3(void);
	/*=================================================================== */
	/**
	 *  @brief connect
	 *
	 *  @param cpService						: const string
	 *
	 *  @param node 							: JtpNode
	 *
	 *  @return bool							: true/false
	 **/
	/*=================================================================== */
	virtual bool connect(const std::string& cpService, JtpNode* node=NULL);
	/*=================================================================== */
	/**
         *  @param cpService                                         : const string
         *  @return short 
         **/
        /*=================================================================== */
         unsigned short getProtocolFromCP (const std::string& cpService);
        /*=================================================================== */
	/**
	 *
	 *  @brief send
	 *
	 *  @param lkDataList						: const list
	 *
	 *  @param lmMode 							: int
	 *
	 *  @param discLklist 							: list
	 *
	 *	@param connLklist 							: list
	 *
	 *  @return bool							: true/false
	 **/
	/*=================================================================== */
    //LMcleaup
      //virtual bool send(const std::list<LkData*>& lkDataList, int lmMode);
      virtual bool send(const std::list<LkData*>& lkDataList, int lmMode, const std::list<LkData*>& discLklist = std::list<LkData*>(), const std::list<LkData*>& connLklist = std::list<LkData*>());
      //LMcleaup

	/*=================================================================== */
	/**
	 *  @brief disconnect
	 *
	 *  @return bool	: true/false
	 **/
	/*=================================================================== */
	virtual bool disconnect();

	//virtual bool connect(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node, int lmMode);
	/*=================================================================== */

	/**
	 *  @brief send
	 *
	 *  @param jtpConv		: ACS_JTP_Conversation_R3A pointer
	 *
	 *  @param node 		: ACS_JTP_Conversation_R3A::JTP_Node*
*
	 *  @param lkDataList 	:const list

	 *  @param lmMode 		: int
	 *
	 *  @return bool		: true/false
	 **/
	/*=================================================================== */

	bool send(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node,const std::list<LkData*>& lkDataList, int lmMode);
	/*=================================================================== */

	/**
	 *  @brief disconnect
	 *
	 *  @param jtpConv		: ACS_JTP_Conversation_R3A
	 *
	 *  @param node 		: ACS_JTP_Conversation_R3A::JTP_Node*
	 *
	 *  @return bool		: true/false
	 **/
	/*=================================================================== */

	static bool disconnect(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node);
	/*=================================================================== */

	/**
	 *  @brief sendDataThreadFunction
	 *
	 *  @param threadData					: void pointer
	 *
	 *  @return ACE_THR_FUNC_RETURN
	 **/
	/*=================================================================== */

	void clearCPSendDataList();
	void sendStopSignalCPSendDataList();
	void setStopSignal(bool);
	bool getStopSignal();


private:
	/*===================================================================
										 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
										 PRIVATE METHOD
	=================================================================== */

	ACS_JTP_Conversation_R3A::JTP_Node* activeNodes;
	int noOfActiveNodes;
	ACE_Thread_Manager *pThrMgr;
	ACE_Event * stopEvent;
	std::string cpService;
	SendThreadData** threadData;
	std::list<Buffer32*> buffer32List;
	std::list<Buffer32*> buffer32List_Job;
	std::list<Buffer32*> discbuffer32List ;   //LMcleaup
	std::list<Buffer32*> connbuffer32List ;   //LMcleaup
	std::list<acs_lm_senddata*> theCPSendDataList;
	static ACE_Recursive_Thread_Mutex theStopMutex;
	bool theStopSignal;


    //LMcleaup  start

    //Define functionCodes
    enum functionCode
    {
            Notification,
            Connection,
            Disconnection,
            Not_Used
    };
    //LMcleaup  End

	/**
	 *  create method:
	 *  @param node			: JtpNode
	 *  @param cpService 	: const string
	 *  @return bool		: true/false
	 **/
	bool updateActiveNodes(JtpNode* node, const std::string& cpService);

	/**
	 *  create method:
	 *  @param jtpConv		: ACS_JTP_Conversation_R3A
	 *  @param node 		: ACS_JTP_Conversation_R3A::JTP_Node*
	 *  @return bool		: true/false
	 **/
	//static bool connect(ACS_JTP_Conversation_R3A* jtpConv,  ACS_JTP_Conversation_R3A::JTP_Node* node);
	static bool connect(ACS_JTP_Conversation_R3A* jtpConv,  ACS_JTP_Conversation_R3A::JTP_Node* node, unsigned short &proto);//LMcleaup
	/**
	 *  create method:
	 *  @param jtpConv		: ACS_JTP_Conversation_R3A
	 *  @param node 		: ACS_JTP_Conversation_R3A::JTP_Node*
	 *  @param bufferList 	: const list<Buffer32*>&
	 *  @return bool		: true/false
	 **/

	//static bool send(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node, const std::list<Buffer32*>& bufferList);
	static bool send(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node, const std::list<Buffer32*>& bufferList,functionCode code = Notification);//LMcleaup
	/**
	 *  create method		:clearbuffers
	 *  @param list			:
	 *  @param list 		:
	 *  @param list 		:
	 *  @return void		:
	 **/
	static void clearbuffers(std::list<Buffer32*>& , std::list<Buffer32*>& , std::list<Buffer32*>& );//LmcleanUp

	//static bool disconnect(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node);
	/**	Check equivalent of _stdcall */
#if 0
	//    static unsigned long _stdcall sendDataThread(void* threadData);
#endif
	/**
	 *  create method:
	 *  @param threadData		: void pointer
	 *  @return unsigned long	: true/false
	 **/
};

#endif
