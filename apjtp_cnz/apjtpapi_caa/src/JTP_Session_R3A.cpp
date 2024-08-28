/*=================================================================== */
   /**
   @file JTP_Session_R3A.cpp

   Class method implementation for session.

   This module contains the implementation of class declared in
   the JTP_Session_R3A.h module.

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
#include <jtp.h>
#include <JTP_Session_R3A.h>
#include <new>
using namespace std;


/*===================================================================
   ROUTINE: jexinitreq
=================================================================== */
bool JTP_Session_R3A::jexinitreq(ACS_DSD_Node* Node, unsigned short U1, unsigned short U2)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Entering 'JTP_Session_R3A::jexinitreq(...': Node System ID = %u, U1 = %hu, U2 = %hu. Session Info = '%s'",
				(Node ? Node->system_id : 0), U1, U2, statusStr);
	}

	if ((IntState != ClientConv) || (JState != StateCreated))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("In 'JTP_Session_R3A::jexinitreq(...': ERROR ASSERTION ((IntState != ClientConv) || (JState != StateCreated)), bad state. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
	}

	u1 = U1;
	u2 = U2;
	//initialization of the pointer NodeSv by input parameter Node
	if(Node != NULL)	
	{
		NodeSv->set( Node->system_id, Node->system_name, Node->system_type, Node->node_state,
			Node->node_name, Node->node_side );
	}
	
	int res;

	//performs conversation connection
	if ((res = jexinitreq_snd()) != 0)
	{ //negative case jexinitreq_snd failed
		r = res;

		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jexinitreq(...': ERROR ASSERTION ((res = jexinitreq_snd()) != 0), send failure. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		reset(); //closing channel and clearing state
		return false;
	}

	//fetches and validates initiation response data
	if (!fetch_conf())
	{
		//negative case fetch failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jexinitreq(...': ERROR ASSERTION (!fetch_conf()), failure. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		reset(); //closing channel and clearing state
		return false;
	}

	JState = StateConnected; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Leaving 'JTP_Session_R3A::jexinitreq(...': SUCCESSFUL. Session Info = '%s'", statusStr);
	}

	return true;
}


/*===================================================================
   ROUTINE: jexinitind
=================================================================== */
bool JTP_Session_R3A::jexinitind(ACS_DSD_Node& Node, unsigned short& U1, unsigned short& U2)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Entering 'JTP_Session_R3A::jexinitind(...': Session Info = '%s'", statusStr);
	}

	if ((IntState != ServerConv) || (JState != StateConnected))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jexinitind(...': "
					"ERROR ASSERTION [(IntState != ServerConv) || (JState != StateConnected)], bad state. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		reset();
		return false;
	}

	//check protocol version
	if (ProtocolVersion > 1)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jexinitind(...': "
					"ERROR ASSERTION [ProtocolVersion > 1], bad protocol version. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	// The message was already read in newSession during accept
	U1 = u1;
	U2 = u2;

	if (mySession)
	{
		mySession->get_remote_node(Node);
	}
	JState = StateAccepted; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Leaving 'JTP_Session_R3A::jexinitind(...': Node System ID = %u, U1 = %hu, U2 = %hu"
				"Session Info = '%s'",
				Node.system_id, U1, U2, statusStr);
	}

	return true;
}


/*===================================================================
   ROUTINE: jinitreq
=================================================================== */
bool JTP_Session_R3A::jinitreq(ACS_DSD_Node* Node, unsigned short U1, unsigned short U2, unsigned short BufferLength, char* Buf)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Entering 'JTP_Session_R3A::jinitreq(...': "
				"Node System ID = %u, U1 = %hu, U2 = %hu, BufferLength = %hu. Session Info = '%s'",
				(Node ? Node->system_id : 0), U1, U2, BufferLength, statusStr);
	}

	if ((IntState != ClientJob) || (JState != StateCreated))
	{
		JState = StateCreated;
	}

	u1 = U1;
	u2 = U2;
	BufLen = BufferLength;
	ACE_OS::memcpy(&Buffer[28], Buf, BufLen);//new version
	//NodeSv = Node; //set NodeSv used in connect
	if(Node != NULL)	
		NodeSv->set( Node->system_id, Node->system_name, Node->system_type, Node->node_state,
                        Node->node_name, Node->node_side );

	int res;

	//performs job connection
	if ((res = jinitreq_snd()) != 0)
	{
		//negative case jinitreq_snd failed
		r = res;
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jinitreq(...': "
					"ERROR ASSERTION [(res = jinitreq_snd()) != 0], send failed, res = %d. "
					"Session Info = '%s'. SOURCE LINE = %u",
					res, statusStr, __LINE__);
		}
		reset(); //Closing channel and clearing state
		return false;
	}

	//fetches and validates initiation response data
	if (!fetch_conf())
	{ //negative case
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jinitreq(...': "
					"ERROR ASSERTION [!fetch_conf()], fetch failure. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset(); //Closing channel and clearing state
		return false;
	}

	JState = StateConnected;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Leaving 'JTP_Session_R3A::jinitreq(...': SUCCESSFUL. Session Info = '%s'", statusStr);
	}
	return true;
}


/*===================================================================
   ROUTINE: jinitind
=================================================================== */
bool JTP_Session_R3A::jinitind(ACS_DSD_Node& Node, unsigned short& U1, unsigned short& U2, unsigned short& BufferLength, char*& Buf)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Entering 'JTP_Session_R3A::jinitind(...': Session Info = '%s'", statusStr);
	}

	if ((IntState != ServerJob) || (JState != StateConnected))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jinitind(...': "
					"ERROR ASSERTION [(IntState != ServerJob) || (JState != StateConnected)], bad state."
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	if (ProtocolVersion > 1)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R3A::jinitind(...': "
					"ERROR ASSERTION [ProtocolVersion > 1], bad protocol version, ProtocolVersion = %d."
					"Session Info = '%s'. SOURCE LINE = %u",
					static_cast<int>(ProtocolVersion), statusStr, __LINE__);
		}
		reset();
		return false;
	}

	// The message was already read in newJob during accept
	U1 = u1;
	U2 = u2;
	//calling to getRemoteNode
	if (mySession)
	{
		mySession->get_remote_node(Node);//remote node identity
	}
	BufferLength  = BufLen;
	Buf = &Buffer[28];
	JState = StateAccepted; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Leaving 'JTP_Session_R3A::jinitind(...': SUCCESSFUL. "
				"Node System ID = %u, U1 = %hu, U2 = %hu, BufferLength = %hu. "
				"Session Info = '%s'",
				Node.system_id, U1, U2, BufferLength, statusStr);
	}
	return true;
}
