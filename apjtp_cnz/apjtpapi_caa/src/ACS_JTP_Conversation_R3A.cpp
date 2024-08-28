/*=================================================================== */
   /**
   @file ACS_JTP_Conversation_R3A.cpp

   Class method implementation for conversation.

   This module contains the implementation of class declared in
   the ACS_JTP_Conversation_R3A.h module

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/04/2010     XTANAGG        APG43 on Linux.
   **/
/*===================================================================*/


/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_JTP_Conversation_R3A.h"
#include "JTP_Session_R3A.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: ACS_JTP_Conversation_R3A()
=================================================================== */
ACS_JTP_Conversation_R3A::ACS_JTP_Conversation_R3A() : ServiceName(""), Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R3A(JTP_Session::JTPConversation, 0);
}

/*===================================================================
   ROUTINE: ACS_JTP_Conversation_R3A
=================================================================== */
ACS_JTP_Conversation_R3A::ACS_JTP_Conversation_R3A(char* Service, unsigned short MaxBufLen) : ServiceName(Service), Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R3A(JTP_Session::JTPConversation, Service, MaxBufLen);
}

/*===================================================================
   ROUTINE: ~ACS_JTP_Conversation_R3A
=================================================================== */
ACS_JTP_Conversation_R3A::~ACS_JTP_Conversation_R3A()
{
	if( Internal)
	{
		delete Internal;
		Internal = 0;
	}
}

/*===================================================================
   ROUTINE: getHandle
=================================================================== */
JTP_HANDLE ACS_JTP_Conversation_R3A::getHandle()
{
	JTP_HANDLE handle = ACE_INVALID_HANDLE;
	if (Internal)
	{
		Internal->getHandle(handle);
	}
	return handle;
}

/*===================================================================
   ROUTINE: setNoOfTries
=================================================================== */
void ACS_JTP_Conversation_R3A::setNoOfTries(int Tries)
{
	if (Internal)
	{
		Internal->setNoOfTries(Tries);
	}
}

/*===================================================================
   ROUTINE: setDelayTime
=================================================================== */
void ACS_JTP_Conversation_R3A::setDelayTime(int Sec)
{
	if (Internal)
	{
		Internal->setDelayTime(Sec);
	}
}

/*===================================================================
   ROUTINE: setTimeOut
=================================================================== */
void ACS_JTP_Conversation_R3A::setTimeOut(int Sec)
{
	if (Internal)
	{
		Internal->setTimeOut(Sec);
	}
}

/*===================================================================
   ROUTINE: Side
=================================================================== */
ACS_JTP_Conversation_R3A::JTP_Side ACS_JTP_Conversation_R3A::Side()
{
	return (JTP_Side)Internal->Side();
}

/*===================================================================
   ROUTINE: State
=================================================================== */
ACS_JTP_Conversation_R3A::JTP_State ACS_JTP_Conversation_R3A::State()
{
	return (JTP_State)Internal->State();
}

/*===================================================================
   ROUTINE: jexinitreq old version
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexinitreq(unsigned short U1, unsigned short U2)
{
	return Internal ? Internal->jexinitreq(U1, U2) : false;
}

/*===================================================================
   ROUTINE: jexinitreq new version
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexinitreq(JTP_Node* Node,  unsigned short U1, unsigned short U2)
{
	bool resJexinitReq =  false;
	ACS_DSD_Node *dsdNodePtr =  0;

	//Allocate memory for DSD Node.
	dsdNodePtr = new ACS_DSD_Node();

	if( dsdNodePtr == 0 )
	{
		return false;
	}

	dsdNodePtr->system_id = Node->system_id;
	ACE_OS::strcpy(dsdNodePtr->system_name, Node->system_name);

	dsdNodePtr->system_type = (acs_dsd::SystemTypeConstants) Node->system_type;

	dsdNodePtr->node_state = (acs_dsd::NodeStateConstants)Node->node_state;
	ACE_OS::strcpy(dsdNodePtr->node_name, Node->node_name);
	dsdNodePtr->node_side = (acs_dsd::NodeSideConstants)Node->node_side;

	if( Internal != 0 )
	{
		resJexinitReq = Internal->jexinitreq(dsdNodePtr, U1, U2);
	}
	if( dsdNodePtr != 0 )
	{
		delete dsdNodePtr;
		dsdNodePtr = 0;
	}
	return resJexinitReq;
		
}

/*===================================================================
   ROUTINE: query
=================================================================== */
bool ACS_JTP_Conversation_R3A::query(JTP_SystemType scope, std::vector<JTP_Node>& nodes, std::vector<JTP_Node>& notReachNodes)
{
	//declarations
	std::vector<ACS_DSD_Node> DSDNodes;
	std::vector<ACS_DSD_Node> DSDNotReachNodes;
	acs_dsd::SystemTypeConstants SCOPE; //input
	bool bquery= false;
	//initialization
	SCOPE = (acs_dsd::SystemTypeConstants)scope;
	//calling to the jtp query
	bquery = Internal ? Internal->JTPquery(ServiceName, "JTP", SCOPE, DSDNodes, DSDNotReachNodes) : false;

	if( bquery)
	{
		for (unsigned int i = 0; i < DSDNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNodes[i].system_id;
			ACE_OS::strcpy(jNode.system_name, DSDNodes[i].system_name);
			jNode.system_type = (JTP_SystemType)DSDNodes[i].system_type;
			jNode.node_state = (JTP_NodeState)DSDNodes[i].node_state;
			ACE_OS::strcpy(jNode.node_name, DSDNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide) DSDNodes[i].node_side;
			//Insert the node into reachable nodes vector nodes.

			nodes.push_back(jNode);
		}
		for (unsigned int i = 0; i < DSDNotReachNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNotReachNodes[i].system_id;
			ACE_OS::strcpy(jNode.system_name, DSDNotReachNodes[i].system_name);
			jNode.system_type = (JTP_SystemType)DSDNotReachNodes[i].system_type;
			jNode.node_state = (JTP_NodeState)DSDNotReachNodes[i].node_state;
			ACE_OS::memset(jNode.node_name,0,sizeof(jNode.node_name));
			ACE_OS::strcpy(jNode.node_name, DSDNotReachNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide) DSDNotReachNodes[i].node_side;

			//Insert the node into un reachable nodes vector notReachNodes.

			notReachNodes.push_back(jNode);
		}
	}

	return bquery;
}

/*===================================================================
   ROUTINE: query
=================================================================== */
bool ACS_JTP_Conversation_R3A::query(JTP_SystemType scope, std::vector<JTP_Node>& nodes, std::vector<JTP_Node>& notReachNodes, int milliseconds)
{
	std::vector<ACS_DSD_Node> DSDNodes;
	std::vector<ACS_DSD_Node> DSDNotReachNodes;
	//declarations
	acs_dsd::SystemTypeConstants SCOPE; //input
	bool bquery = false;
	//initializations
	SCOPE = (acs_dsd::SystemTypeConstants)scope;

	//calling to the jtp query
	bquery = Internal ? Internal->JTPquery(ServiceName, "JTP", SCOPE, DSDNodes, DSDNotReachNodes, milliseconds) : false;
	//copy output data

	if(bquery)
	{
		for (unsigned int i = 0; i < DSDNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNodes[i].system_id;
			ACE_OS::strcpy(jNode.system_name, DSDNodes[i].system_name);
			jNode.system_type = (JTP_SystemType)DSDNodes[i].system_type;
			jNode.node_state = (JTP_NodeState)DSDNodes[i].node_state;
			ACE_OS::memset(jNode.node_name,0,sizeof(jNode.node_name));
			ACE_OS::strcpy(jNode.node_name, DSDNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide)DSDNodes[i].node_side;

			//Insert the node into reachable nodes vector.
			nodes.push_back(jNode);
		}
		for (unsigned int i = 0; i < DSDNotReachNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNotReachNodes[i].system_id;
			ACE_OS::strcpy(jNode.system_name, DSDNotReachNodes[i].system_name);
			jNode.system_type = (JTP_SystemType)DSDNotReachNodes[i].system_type;
			jNode.node_state = (JTP_NodeState)DSDNotReachNodes[i].node_state;
			ACE_OS::memset(jNode.node_name,0,sizeof(jNode.node_name));
			ACE_OS::strcpy(jNode.node_name, DSDNotReachNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide) DSDNotReachNodes[i].node_side;

			//Insert the un reachable node into unreachable nodes vector.
			notReachNodes.push_back(jNode);
		}
	}
	return bquery;
}

/*===================================================================
   ROUTINE: jexinitind old version
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexinitind(unsigned short& U1, unsigned short& U2)
{
	return Internal ? Internal->jexinitind(U1, U2) : false;
}

/*===================================================================
   ROUTINE: jexinitind
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexinitind(JTP_Node& Node, unsigned short& U1, unsigned short& U2)
{
	bool b = false;
	ACS_DSD_Node *acsDSDNodePtr = 0;
	acsDSDNodePtr = new ACS_DSD_Node();
	if( acsDSDNodePtr == 0 )
	{
		return b;
	}
	b = Internal ? Internal->jexinitind(*acsDSDNodePtr, U1, U2) : false;
	if( b == true )
	{
		Node.system_id = acsDSDNodePtr->system_id;
		ACE_OS::strcpy(Node.system_name, acsDSDNodePtr->system_name );
		Node.system_type = (JTP_SystemType)acsDSDNodePtr->system_type;
		Node.node_state = (JTP_NodeState)acsDSDNodePtr->node_state;
		ACE_OS::snprintf(Node.node_name, sizeof(Node.node_name) - 1, "%s",acsDSDNodePtr->node_name);
		Node.node_side = (JTP_NodeSide)acsDSDNodePtr->node_side;
	}
	if( acsDSDNodePtr != 0 )
	{
		delete acsDSDNodePtr;
		acsDSDNodePtr = 0;
	}
	return b;
}

/*===================================================================
   ROUTINE: jexinitrsp
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexinitrsp(unsigned short U1, unsigned short U2, unsigned short R)
{
	return Internal ? Internal->jexinitrsp(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jexinitconf
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexinitconf(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return Internal ? Internal->jexinitconf(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jexdatareq
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexdatareq(unsigned short U1, unsigned short U2, unsigned short BufLen, char* Buf)
{
	return Internal ? Internal->jexdatareq(U1, U2, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jexdataind
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexdataind(unsigned short& U1, unsigned short& U2, unsigned short& BufLen, char*& Buf)
{
	return Internal ? Internal->jexdataind(U1, U2, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jexdiscreq
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexdiscreq(unsigned short U1, unsigned short U2, unsigned short R)
{
	return Internal ? Internal->jexdiscreq(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jexdiscind
=================================================================== */
bool ACS_JTP_Conversation_R3A::jexdiscind(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return Internal ? Internal->jexdiscind(U1, U2, R) : false;
}
