/*=================================================================== */
   /**
   @file ACS_JTP_Job_R3A.cpp

   Class method implementation for job.

   This module contains the implementation of class declared in
   the ACS_JTP_Job_R3A.h module

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/04/2010     XTANAGG        APG43 on Linux.
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_JTP_Job_R3A.h"
#include "JTP_Session_R3A.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: ACS_JTP_Job_R3A with Service and MaxBufLen
=================================================================== */
ACS_JTP_Job_R3A::ACS_JTP_Job_R3A(char* Service, unsigned short MaxBufLen) : ServiceName(Service), Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R3A(JTP_Session::JTPJob, Service, MaxBufLen);
}

/*===================================================================
   ROUTINE: ACS_JTP_Job_R3A
=================================================================== */
ACS_JTP_Job_R3A::ACS_JTP_Job_R3A() : ServiceName(""), Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R3A(JTP_Session::JTPJob, 0);
}

/*===================================================================
   ROUTINE: ~ACS_JTP_Job_R3A
=================================================================== */
ACS_JTP_Job_R3A::~ACS_JTP_Job_R3A()
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
JTP_HANDLE ACS_JTP_Job_R3A::getHandle()
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
void ACS_JTP_Job_R3A::setNoOfTries(int Tries)
{
	if (Internal)
	{
		Internal->setNoOfTries(Tries);
	}
}

/*===================================================================
   ROUTINE: setDelayTime
=================================================================== */
void ACS_JTP_Job_R3A::setDelayTime(int Sec)
{
	if (Internal)
	{
		Internal->setDelayTime(Sec);
	}
}

/*===================================================================
   ROUTINE: setTimeOut
=================================================================== */
void ACS_JTP_Job_R3A::setTimeOut(int Sec)
{
	if (Internal)
	{
		Internal->setTimeOut(Sec);
	}
}

/*===================================================================
   ROUTINE: Side
=================================================================== */
 ACS_JTP_Job_R3A::JTP_Side ACS_JTP_Job_R3A::Side()
{
	return ACS_JTP_Job_R3A::JTPClient;
}

/*===================================================================
   ROUTINE: State
=================================================================== */
 ACS_JTP_Job_R3A::JTP_State ACS_JTP_Job_R3A::State()
{
	return (JTP_State)Internal->State();
}

/*===================================================================
   ROUTINE: jinitreq
=================================================================== */
bool ACS_JTP_Job_R3A::jinitreq(unsigned short U1, unsigned short U2, unsigned short BufLen, char* Buf)
{
	return Internal ? Internal->jinitreq(U1, U2, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jinitreq new version
=================================================================== */
bool ACS_JTP_Job_R3A::jinitreq(JTP_Node* Node, unsigned short U1, unsigned short U2, unsigned short BufLen, char* Buf)
{
	bool resJinitReq =  false;
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
		resJinitReq = Internal->jinitreq(dsdNodePtr, U1, U2, BufLen, Buf) ;
	}
	if( dsdNodePtr != 0 )
        {
                delete dsdNodePtr;
                dsdNodePtr = 0;
        }
        return resJinitReq;

}

/*===================================================================
   ROUTINE: query new version
=================================================================== */
bool ACS_JTP_Job_R3A::query(JTP_SystemType scope, std::vector<JTP_Node>& nodes, std::vector<JTP_Node>& notReachNodes)
{
	//declarations
	std::vector<ACS_DSD_Node> DSDNodes;
	std::vector<ACS_DSD_Node> DSDNotReachNodes;
	acs_dsd::SystemTypeConstants SCOPE; //input
	bool bquery = false;
	//initializations
	SCOPE = (acs_dsd::SystemTypeConstants)scope;
	//calling to the jtp query
	bquery = Internal ? Internal->JTPquery(ServiceName, "JTP", SCOPE, DSDNodes, DSDNotReachNodes) : false;

	if( bquery )
	{
		for (unsigned int i = 0; i < DSDNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNodes[i].system_id;
			ACE_OS::strcpy(jNode.system_name, DSDNodes[i].system_name);
			jNode.system_type = (JTP_SystemType)DSDNodes[i].system_type;
			jNode.node_state = (JTP_NodeState)DSDNodes[i].node_state;
			ACE_OS::memset(jNode.node_name, 0, sizeof(jNode.node_name));
			ACE_OS::strcpy(jNode.node_name, DSDNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide) DSDNodes[i].node_side;

			//Insert the node into vector.
			nodes.push_back(jNode);
		}

		for (unsigned int i = 0; i < DSDNotReachNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNotReachNodes[i].system_id;
			ACE_OS::strcpy(jNode.system_name, DSDNotReachNodes[i].system_name);
			jNode.system_type = (JTP_SystemType)DSDNotReachNodes[i].system_type;
			jNode.node_state = (JTP_NodeState)DSDNotReachNodes[i].node_state;
			ACE_OS::memset(jNode.node_name, 0, sizeof(jNode.node_name));
			ACE_OS::strcpy(jNode.node_name, DSDNotReachNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide) DSDNotReachNodes[i].node_side;
			//Insert the not Reachable nodes into vector.
			notReachNodes.push_back(jNode);
		}
	}
	return bquery;
}

/*===================================================================
   ROUTINE: query
=================================================================== */
bool ACS_JTP_Job_R3A::query(JTP_SystemType scope, std::vector<JTP_Node>& nodes, std::vector<JTP_Node>& notReachNodes,  int milliseconds)
{
	//declarations

	std::vector<ACS_DSD_Node> DSDNodes;
	std::vector<ACS_DSD_Node> DSDNotReachNodes;

	acs_dsd::SystemTypeConstants SCOPE; //input
	bool bquery = false;
	//initializations
	SCOPE = (acs_dsd::SystemTypeConstants)scope;

	//calling to the jtp query
	bquery = Internal ? Internal->JTPquery(ServiceName, "JTP", SCOPE, DSDNodes, DSDNotReachNodes, milliseconds) : false;

	if( bquery )
	{
		for (unsigned int i = 0; i < DSDNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNodes[i].system_id;
			jNode.system_type = (JTP_SystemType)DSDNodes[i].system_type;
			ACE_OS::strcpy(jNode.system_name, DSDNodes[i].system_name);
			jNode.node_state = (JTP_NodeState)DSDNodes[i].node_state;
			ACE_OS::memset(jNode.node_name, 0, sizeof(jNode.node_name));
			ACE_OS::strcpy(jNode.node_name, DSDNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide)DSDNodes[i].node_side;

			//Insert the node into vector nodes.
			nodes.push_back(jNode);
		}
		for (unsigned int i = 0; i < DSDNotReachNodes.size() ; i++)
		{
			JTP_Node jNode;
			jNode.system_id = DSDNotReachNodes[i].system_id;
			ACE_OS::strcpy(jNode.system_name, DSDNotReachNodes[i].system_name);
			jNode.system_type = (JTP_SystemType)DSDNotReachNodes[i].system_type;
			jNode.node_state = (JTP_NodeState)DSDNotReachNodes[i].node_state;
			ACE_OS::memset(jNode.node_name, 0, sizeof(jNode.node_name));
			ACE_OS::strcpy(jNode.node_name, DSDNotReachNodes[i].node_name);
			jNode.node_side = (JTP_NodeSide) DSDNotReachNodes[i].node_side;

			//Insert the not reachable node into vector notReachNodes.

			notReachNodes.push_back(jNode);
		}
	}
	return bquery;
}

/*===================================================================
   ROUTINE: jinitind old version
=================================================================== */
bool ACS_JTP_Job_R3A::jinitind(unsigned short& U1, unsigned short& U2, unsigned short& BufLen, char*& Buf)
{
	return Internal ? Internal->jinitind(U1, U2, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jinitind new version
=================================================================== */
bool ACS_JTP_Job_R3A::jinitind(JTP_Node& Node, unsigned short& U1, unsigned short& U2, unsigned short& BufLen, char*& Buf)
{
	bool b = false;
        ACS_DSD_Node *acsDSDNodePtr = 0;
        acsDSDNodePtr = new ACS_DSD_Node();
        if( acsDSDNodePtr == 0 )
        {
                return b;
        }
        b = Internal ? Internal->jinitind(*acsDSDNodePtr, U1, U2, BufLen, Buf) : false;
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
   ROUTINE: jinitrsp
=================================================================== */
bool ACS_JTP_Job_R3A::jinitrsp(unsigned short U1, unsigned short U2, unsigned short R)
{
	return Internal ? Internal->jinitrsp(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jinitconf
=================================================================== */
bool ACS_JTP_Job_R3A::jinitconf(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return Internal ? Internal->jinitconf(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jresultreq
=================================================================== */
bool ACS_JTP_Job_R3A::jresultreq(unsigned short U1, unsigned short U2, unsigned short R, unsigned short BufLen, char* Buf)
{
	return Internal ? Internal->jresultreq(U1, U2, R, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jresultind
=================================================================== */
bool ACS_JTP_Job_R3A::jresultind(unsigned short& U1, unsigned short& U2, unsigned short& R, unsigned short& BufLen, char*& Buf)
{
	return Internal ? Internal->jresultind(U1, U2, R, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jtermreq
=================================================================== */
bool ACS_JTP_Job_R3A::jtermreq()
{
	return Internal ? Internal->jtermreq() : false;
}

/*===================================================================
   ROUTINE: jfault
=================================================================== */
bool ACS_JTP_Job_R3A::jfault(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return Internal ? Internal->jfault(U1, U2, R) : false;
}
