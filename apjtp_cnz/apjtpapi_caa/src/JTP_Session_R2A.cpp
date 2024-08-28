/*=================================================================== */
   /**
   @file JTP_Session_R2A.cpp

   Class method implementation for session.

   This module contains the implementation of class declared in
   the JTP_Session_R2A.h module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/04/2010     XTANAGG        APG43 on Linux.
   N/A	     10/11/2010		XTANAGG		   Sprint 5 changes.
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "jtp.h"
#include "JTP_Session_R2A.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: jexinitreq
=================================================================== */
bool JTP_Session_R2A::jexinitreq(short U1, short U2)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Entering 'JTP_Session_R2A::jexinitreq(short U1, short U2)': Session Info = '%s', U1 = %hd, U2 = %hd",
				statusStr, U1, U2);
	}

	if ((IntState != ClientConv) || (JState != StateCreated))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"In 'JTP_Session_R2A::jexinitreq(short U1, short U2)': "
					"ERROR ASSERTION [(IntState != ClientConv) || (JState != StateCreated)],  bad state. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		reset();
	}

	if (!NodeSv)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Leaving 'JTP_Session_R2A::jexinitreq(short U1, short U2)': "
					"ERROR ASSERTION [!NodeSv], OUT OF MEMORY allocating DSD Node structure. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	//set systemId to default value and nodestate
	NodeSv->system_id = acs_dsd::SYSTEM_ID_CP_ALARM_MASTER;
	NodeSv->node_state = acs_dsd::NODE_STATE_ACTIVE;
	int res;

	u1 = U1;
	u2 = U2;

	//performs conversation connection
	if ((res = jexinitreq_snd()) != 0)
	{
		//Active node conn/send failed, try passive
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceWarning(
					"In 'JTP_Session_R2A::jexinitreq(short U1, short U2)': "
					"WARNING ASSERTION [(res = jexinitreq_snd()) != 0], Active node conn/send failed, try passive. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		NodeSv->node_state = acs_dsd::NODE_STATE_PASSIVE;

		if ((res = jexinitreq_snd()) != 0)
		{
			//Passive node conn/send failed,exit
			r = res;
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError(
						"Leaving 'JTP_Session_R2A::jexinitreq(short U1, short U2)': "
						"ERROR ASSERTION [(res = jexinitreq_snd()) != 0], Passive node conn/send failed, res = %d, exit. "
						"Session Info = '%s'. SOURCE LINE = %u",
						res, statusStr, __LINE__);
			}
			reset(); //closing channel and clearing state
			return false;
		}
	}

	//fetches and validates initiation response data
	if (!fetch_conf())
	{ //negative case fetch failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Leaving 'JTP_Session_R2A::jexinitreq(short U1, short U2)': "
					"ERROR ASSERTION [!fetch_conf()], fetching failure. "
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
		helperTraceInformation("Leaving 'JTP_Session_R2A::jexinitreq(short U1, short U2)': SUCCESSFUL. Session Info = '%s'", statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexinitind
=================================================================== */
bool JTP_Session_R2A::jexinitind(unsigned short& U1, unsigned short& U2)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Entering 'JTP_Session_R2A::jexinitind(unsigned short& U1, unsigned short& U2)': Session Info = '%s'",
				statusStr);
	}

	if ((IntState != ServerConv) || (JState != StateConnected))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Leaving 'JTP_Session_R2A::jexinitind(unsigned short& U1, unsigned short& U2)': "
					"ERROR ASSERTION [(IntState != ServerConv) || (JState != StateConnected)], bad state. "
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
			helperTraceError(
					"Leaving 'JTP_Session_R2A::jexinitind(unsigned short& U1, unsigned short& U2)': "
					"ERROR ASSERTION [ProtocolVersion > 1], bad protocol version, ProtocolVersion = %d. "
					"Session Info = '%s'. SOURCE LINE = %u",
					static_cast<int>(ProtocolVersion), statusStr, __LINE__);
		}

		reset();
		return false;
	}

	// The message was already read in newSession during accept
	U1 = u1;
	U2 = u2;
	JState = StateAccepted; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Leaving 'JTP_Session_R2A::jexinitind(unsigned short& U1, unsigned short& U2)': "
				"Session Info = '%s', U1 = %hu, U2 = %hu",
				statusStr, U1, U2);
	}
	return true;
}

/*===================================================================
   ROUTINE: jinitreq
=================================================================== */
bool JTP_Session_R2A::jinitreq(short U1, short U2, short BufferLength, char* Buf)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Entering 'JTP_Session_R2A::jinitreq(...': "
				"Session Info = '%s', U1 = %hd, U2 = %hd, BufferLength = %hd",
				statusStr, U1, U2, BufferLength);
	}

	if ((IntState != ClientJob) || (JState != StateCreated))
		JState = StateCreated;

	if (!NodeSv)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Leaving 'JTP_Session_R2A::jinitreq(...': "
					"ERROR ASSERTION [!NodeSv], OUT OF MEMORY allocating DSD Node structure. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();

		return false;
	}

	u1 = U1;
	u2 = U2;
	BufLen = BufferLength;
	ACE_OS::memcpy(&Buffer[28], Buf, BufLen);

	//set systemId and state in Node struct to default value
	NodeSv->system_id = acs_dsd::SYSTEM_ID_CP_ALARM_MASTER;
	NodeSv->node_state = acs_dsd::NODE_STATE_ACTIVE;
	int res;

	//performs job connection
	if ((res = jinitreq_snd()) != 0)
	{
		//jinitreq failed towards ACTIVE. Try PASSIVE
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"In 'JTP_Session_R2A::jinitreq(...': "
					"ERROR ASSERTION [(res = jinitreq_snd()) != 0], jinitreq failed towards ACTIVE. Try PASSIVE. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		NodeSv->node_state = acs_dsd::NODE_STATE_PASSIVE;
		if ((res = jinitreq_snd()) != 0)
		{ //negative case jinitreq failed towards PASSIVE too
			r = res;

			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError(
						"Leaving 'JTP_Session_R2A::jinitreq(...': "
						"ERROR ASSERTION [(res = jinitreq_snd()) != 0], jinitreq failed towards PASSIVE too, EXIT. "
						"Session Info = '%s'. SOURCE LINE = %u",
						statusStr, __LINE__);
			}

			reset(); //closing channel and clearing state
			return false;
		}
	}

	//fetches and validates initiation response data
	if (!fetch_conf())
	{ //negative case fetch failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Leaving 'JTP_Session_R2A::jinitreq(...': "
					"ERROR ASSERTION [!fetch_conf()], fetch failure. "
					"Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset(); //closing channel and clearing state
		return false;
	}

	//req OK
	JState = StateConnected; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Leaving 'JTP_Session_R2A::jinitreq(...': SUCCESSFUL. Session Info = '%s'", statusStr);
	}


	return true;
}

/*===================================================================
   ROUTINE: jinitind
=================================================================== */
bool JTP_Session_R2A::jinitind(unsigned short& U1, unsigned short& U2, unsigned short& BufferLength, char*& Buf)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Entering 'JTP_Session_R2A::jinitind(...': Session Info = '%s'", statusStr);
	}

	if ((IntState != ServerJob) || (JState != StateConnected))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Leaving 'JTP_Session_R2A::jinitind(...': "
					"ERROR ASSERTION [(IntState != ServerJob) || (JState != StateConnected)], bad state. "
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
			helperTraceError("Leaving 'JTP_Session_R2A::jinitind(...': "
					"ERROR ASSERTION [ProtocolVersion > 1], bad protocol version, ProtocolVersion = %d. "
					"Session Info = '%s'. SOURCE LINE = %u",
					static_cast<int>(ProtocolVersion), statusStr, __LINE__);
		}
		reset();
		return false;
	}

	// The message was already read in newJob during accept
	U1 = u1;
	U2 = u2;
	BufferLength = BufLen;
	Buf = &Buffer[28];
	JState = StateAccepted; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Leaving 'JTP_Session_R2A::jinitind(...': "
				"U1 = %hu, U2 = %hu, BufferLength = %hu, Session Info = '%s'",
				U1, U2, BufferLength, statusStr);
	}

	return true;
}
