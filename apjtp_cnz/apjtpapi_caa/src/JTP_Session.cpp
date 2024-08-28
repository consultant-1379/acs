/*=================================================================== */
   /**
   @file JTP_Session.cpp

   Class method implementation for session.

   This module contains the implementation of class declared in
   the JTP_Session.h module.

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
#include <JTP_Session.h>
#include <ACS_JTP_CriticalSectionGuard.h>
#include <ACS_PRC_info.h>
#include <fstream>
#include <ace/OS_NS_sys_select.h>
#include <acs_apgcc_omhandler.h>
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: getSessionInformation
=================================================================== */
void JTP_Session::getSessionInformation(char* traceBuf, int traceBufLen)
{
	if (traceBuf)
	{
		ACE_OS::memset(traceBuf,0,traceBufLen);

		char str_intstate[100] = {0};
		char str_jstate[100] = {0};
		char str_jside[100] = {0};

		//Set the state.
		switch(IntState)
		{
			case ServerJob:
			{
				ACE_OS::snprintf(str_intstate, sizeof(str_intstate) - 1, "ServerJob");
				break;
			}
			case ClientJob:
			{
				ACE_OS::snprintf(str_intstate, sizeof(str_intstate) - 1, "ClientJob");
				break;
			}
			case ServerConv:
			{
				ACE_OS::snprintf(str_intstate, sizeof(str_intstate) - 1, "ServerConv");
				break;
			}
			case  ClientConv:
			{
				ACE_OS::snprintf(str_intstate, sizeof(str_intstate) - 1, "ClientConv");
				break;
			}
			default:
			{
				ACE_OS::snprintf(str_intstate, sizeof(str_intstate) - 1, "unknown internal rule");
				break;
			}
		}

		//Set the state
		switch(JState)
		{
			case StateCreated:
			{
				ACE_OS::snprintf(str_jstate, sizeof(str_jstate) - 1, "StateCreated");
				break;
			}
			case StateConnected:
			{
				ACE_OS::snprintf(str_jstate, sizeof(str_jstate) - 1, "StateConnected");
				break;
			}
			case StateAccepted:
			{
				ACE_OS::snprintf(str_jstate, sizeof(str_jstate) - 1, "StateAccepted");
				break;
			}
			case StateJobRunning:
			{
				ACE_OS::snprintf(str_jstate, sizeof(str_jstate) - 1, "StateJobRunning");
				break;
			}
			case StateWaitForData:
			{
				ACE_OS::snprintf(str_jstate, sizeof(str_jstate) - 1, "StateWaitForData");
				break;
			}
			default:
			{
				ACE_OS::snprintf(str_jstate, sizeof(str_jstate) - 1, "unknown session state");
				break;
			}
		}

		//Set the side,
		switch(JSide)
		{
			case JTPClient:
			{
				ACE_OS::snprintf(str_jside, sizeof(str_jside) - 1, "Client");
				break;
			}
			case JTPServer:
			{
				ACE_OS::snprintf(str_jside, sizeof(str_jside) - 1, "Server");
				break;
			}
			default:
			{
				ACE_OS::snprintf(str_jside, sizeof(str_jside) - 1, "unknown side");
				break;
			}
		}

		ACE_OS::snprintf(traceBuf, traceBufLen - 1, "[%p][%s][%s][%s]", mySession, str_jside, str_intstate, str_jstate);

	}
}


/*===================================================================
   ROUTINE: JTP_Session
=================================================================== */
JTP_Session::JTP_Session(JTP_Type jtype, char* Service)
{
	mySession = 0; //pointer to ACS_DSD_Session
	system_id = (unsigned short)INVALID_SYSTEM_ID;

	JSide = JTPServer;
	JType = jtype;
	if (jtype == JTPJob)
	{
		IntState = ServerJob;
	}
	else
	{
		IntState = ServerConv;
	}

	JState = StateCreated;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In JTP_Session::JTP_Session() constructor: Service = '%s', Session Info = '%s'", Service, statusStr);
	}


	DelayTime = JTP_DEFAULT_DELAY_TIME; //0;
	NoOfTries = 1;
	TimeOut = 5;
	ACE_OS::memset(ServiceName,0,sizeof(ServiceName));
	ACE_OS::snprintf(ServiceName, sizeof(ServiceName)-1, "%s", Service);

	pDSDClient = new (std::nothrow) ACS_DSD_Client(); //pointer to new class DSD_Client
	NodeSv = 0; //pointer to Node struct

	NodeSv = new(std::nothrow) ACS_DSD_Node();

	readBufferLen = 0;
	bufferCursor  = 0;

	ABufLen = 0;
	AMaxBuf = 0;
	Ar = 0;
	Au1 = 0;
	Au2 = 0;
	BufLen = 0;
	r = 0;
	u1 = 0;
	u2 = 0;
	ProtocolVersion = 0;
	APNo = 0;
	APRestartCntr = 0;
	PID = 0;
	ConnectionCntr = 0;
	MaxBuf = 0;
	ACE_OS::memset(Buffer, 0, sizeof(Buffer));
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out JTP_Session::JTP_Session() constructor: Service = '%s', Session Info = '%s'", Service, statusStr);
	}
}

/*===================================================================
   ROUTINE: JTP_Session
=================================================================== */
JTP_Session::JTP_Session(JTP_Type jtype, char* Service, short iMaxBuf)
{
	mySession = 0; //pointer to ACS_DSD_Session
	system_id = (unsigned short) INVALID_SYSTEM_ID;

	JSide = JTPClient;
	JType = jtype;
	if (jtype == JTPJob)
	{
		IntState = ClientJob;
	}
	else
	{
		IntState = ClientConv;
	}

	JState = StateCreated;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In JTP_Session::JTP_Session() constructor: Service = '%s', Session Info = '%s', iMaxBuf = %hd", Service, statusStr, iMaxBuf);
	}

	DelayTime = JTP_DEFAULT_DELAY_TIME; //0;
	NoOfTries = 1;
	TimeOut = 5;
	ACE_OS::memset(ServiceName,0, sizeof(ServiceName));
	if( Service != 0)
	{
		ACE_OS::strcpy(ServiceName, Service);
	}

	MaxBuf = iMaxBuf;
	pDSDClient = new (std::nothrow) ACS_DSD_Client(); //pointer to new class DSD_Client 
	NodeSv = 0; //pointer to Node struct
	NodeSv =  new (std::nothrow) ACS_DSD_Node();
	APNo = 0;
	APRestartCntr = 0;
	PID = 0;
	ConnectionCntr = 0;
	readBufferLen = 0;
	bufferCursor  = 0;
	ABufLen = 0;
	AMaxBuf = 0;
	Ar = 0;
	Au1 = 0;
	Au2 = 0;
	BufLen = 0;
	r = 0;
	u1 = 0;
	u2 = 0;
	ACE_OS::memset(Buffer, 0, sizeof(Buffer));
	ProtocolVersion = 0;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out JTP_Session::JTP_Session() constructor: Service = '%s', Session Info = '%s', iMaxBuf = %hd", Service, statusStr, iMaxBuf);
	}
}

/*===================================================================
   ROUTINE: ~JTP_Session
=================================================================== */
JTP_Session::~JTP_Session()
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In JTP_Session::~JTP_Session() destructor: Session Info = '%s'", statusStr);
	}

	if ((JSide == JTPServer) && (JType == JTPConversation))
	{
		JSLRemoveSession(this);
	}

	if( pDSDClient)
	{
		delete pDSDClient;
		pDSDClient = 0; //pointer to new class DSD_Client
	}

	if( NodeSv )
	{
		delete NodeSv;
		NodeSv = 0;
	}

	if(mySession)
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceInformation("Out JTP_Session::~JTP_Session() destructor: Destroying internal session, Session Info = '%s'", statusStr);
		}

		mySession->close();//shutdown global session
		delete mySession;
		mySession = 0;
	}
	else
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("Out JTP_Session::~JTP_Session() destructor ");
		}
	}
}

/*===================================================================
   ROUTINE: JTypeAsString
=================================================================== */
const char* JTP_Session::JTypeAsString()
{
	switch (JType)
	{
		case JTPConversation:
		{
			return "JTPConversation";
			break;
		}
		case JTPJob:
		{
			return "JTPJob";
			break;
		}
		default:
		{
			return "UNDEFINED_JTYPE";
			break;
		}
	}
}

/*===================================================================
   ROUTINE: JStateAsString
=================================================================== */
const char* JTP_Session::JStateAsString()
{
	switch (JState)
	{
		case StateCreated:
		{
			return "StateCreated";
			break;
		}
		case StateConnected:
		{
			return "StateConnected";
			break;
		}
		case StateAccepted:
		{
			return "StateAccepted";
			break;
		}
		case StateJobRunning:
		{
			return "StateJobRunning";
			break;
		}
		case StateWaitForData:
		{
			return "StateWaitForData";
			break;
		}
		default:
		{
			return "UNDEFINED_JSTATE";
			break;
		}
	}
}

/*===================================================================
   ROUTINE: newSessionServer new version
=================================================================== */
bool JTP_Session::newSessionServer(unsigned long MsgLen, char protocolVersion, char* serviceName, ACS_DSD_Session & Session, char* service_Buffer)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"In 'JTP_Session::newSessionServer(...': Session Info = '%s', MsgLen = %lu, protocolVersion = %d, serviceName = '%s'",
				statusStr, MsgLen, static_cast<int>(protocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	ACE_OS::memcpy(Buffer, service_Buffer, MsgLen); //copy buffer service in buffer session
	ProtocolVersion = protocolVersion; //copy protocolVersion

	if(!newSession(serviceName, Session))
	{
		//init mySession pointer (dsd session related to the specific JTP session)
		return false;
	}

	jexinitind_read(); //reading buffer
	JSLCheckSessions(this, MsgLen); //checkSessions

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::newSessionServer(...': Session Info = '%s', protocolVersion = %d", statusStr, static_cast<int>(protocolVersion));
	}

	return true;
}

/*===================================================================
   ROUTINE: newSessionServer
=================================================================== */
bool JTP_Session::newSessionServer(unsigned long MsgLen, char protocolVersion, char* serviceName, ACS_DSD_Session& Session, char* service_Buffer, int service_readBufferLen, int service_bufferCursor)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"In 'JTP_Session::newSessionServer(...': Session Info = '%s', MsgLen = %lu, protocolVersion = %d, serviceName = '%s'",
				statusStr, MsgLen, static_cast<int>(protocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}
	readBufferLen = service_readBufferLen;
	bufferCursor  = service_bufferCursor;
	ACE_OS::memcpy(Buffer, service_Buffer, readBufferLen);

	ProtocolVersion = protocolVersion;
	if (!newSession(serviceName, Session))
	{
		//init mySession pointer (global session)
		return false;
	}

	if (!jexinitind_rcv(MsgLen - 2))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"In 'JTP_Session::newSessionServer(...': 'jexinitind_rcv(MsgLen - 2)' was false. Session Info = '%s', MsgLen = %lu, protocolVersion = %d, serviceName = '%s'",
					statusStr, MsgLen, static_cast<int>(protocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
		}
		mySession = 0;
		return false;
	}

	//recv ok
	JSLCheckSessions(this, MsgLen);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::newSessionServer(...': Session Info = '%s', MsgLen = %lu, protocolVersion = %d, serviceName = '%s'",
				statusStr, MsgLen, static_cast<int>(protocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	return true;
}

/*===================================================================
   ROUTINE: newSession
   copies application and address data into conversation object
=================================================================== */
bool JTP_Session::newSession(char* serviceName, ACS_DSD_Session & Session)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::newSession(...': Session Info = '%s', serviceName = '%s'", statusStr, (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	if(mySession)
	{ //check mySession
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("In 'JTP_Session::newSession(...': Closing and destroying session");
		}
		mySession->close();	//shutdown of the session
		delete mySession;	//close the underlying socket
		mySession = 0;
	}

	//copy local session
	mySession = &Session; 
	JState = StateConnected; //set state

	ACS_DSD_Node remoteNode;
	mySession->get_remote_node(remoteNode);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::newSession(...': Session Info = '%s', Remote Node system_id = %u", statusStr, remoteNode.system_id);
	}

	return true;
}

/*===================================================================
   ROUTINE: newJobServer new version
=================================================================== */
bool JTP_Session::newJobServer(unsigned long MsgLen, char protocolVersion, char* serviceName, ACS_DSD_Session& Session, char* service_Buffer)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::newJobServer(...': Session Info = '%s', MsgLen = %lu, protocolVersion = %d, serviceName = '%s'",
				statusStr, MsgLen, static_cast<int>(protocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	//copy service buffer in session buffer
	ACE_OS::memcpy(Buffer, service_Buffer, MsgLen);
	//copy protocol version
	ProtocolVersion = protocolVersion;
	//calling to newJob to init mySession pointer
	if(!newJob(serviceName, Session))
	{
		return false;
	}
	//reading buffer
	jinitind_read();

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::newJobServer(...': Session Info = '%s', MsgLen = %lu, ProtocolVersion = %d, serviceName = '%s'",
				statusStr, MsgLen, static_cast<int>(ProtocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	return true;
}

/*===================================================================
   ROUTINE: newJobServer
=================================================================== */
bool JTP_Session::newJobServer(unsigned long MsgLen, char protocolVersion, char* serviceName, ACS_DSD_Session& Session, char* service_Buffer, int service_readBufferLen, int service_bufferCursor)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::newJobServer(...': Session Info = '%s', MsgLen = %lu, protocolVersion = %d, serviceName = '%s'",
				statusStr, MsgLen, static_cast<int>(protocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	readBufferLen = service_readBufferLen;
	bufferCursor  = service_bufferCursor;
	ACE_OS::memcpy(Buffer, service_Buffer, readBufferLen);

	ProtocolVersion = protocolVersion;
	if (!newJob(serviceName, Session))
	{
		return false;
	}

	if (!jinitind_rcv(MsgLen - 2))
	{
		//jinitini_rcv failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::newJobServer(...': 'jinitind_rcv(MsgLen - 2)' was false!");
		}
		mySession = 0;
		return false;
	}
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::newJobServer(...': Session Info = '%s', MsgLen = %lu, protocolVersion = %d, serviceName = '%s'",
				statusStr, MsgLen, static_cast<int>(protocolVersion), (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}
	return true;
}

/*===================================================================
   ROUTINE: newJob
=================================================================== */
bool JTP_Session::newJob(char* serviceName, ACS_DSD_Session& Session)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::newJob(...': Session Info = '%s', serviceName = '%s'", statusStr, (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	//check mySession
	if(mySession)
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("In 'JTP_Session::newJob(...': Closing and destroying session");
		}
		mySession->close();//shutdown session
		delete mySession;
		mySession = 0;
	}

	//copy local session
	mySession = &Session; 
	JState = StateConnected; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::newJob(...': Session Info = '%s', serviceName = '%s'", statusStr, (serviceName ? serviceName : "NO_SERVICE_NAME"));
	}

	return true;


}

/*===================================================================
   ROUTINE: pollReply
=================================================================== */
bool JTP_Session::pollReply(ACE_HANDLE handle, bool omittAtZero)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::pollReply(...)");
	}
	if( handle == ACE_INVALID_HANDLE )
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::pollReply. handle is invalid.");
		}
		return false;
	}
	ACE_Time_Value tv( TimeOut, 0);
	if(TimeOut == 0)
	{
		if(omittAtZero)
		{
			if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceInformation("Out 'JTP_Session::pollReply(...) success.");
			}
			return true;
		}
		tv.sec(5);
	}

	//Create a FD set.
	fd_set readHandleSet;
	FD_ZERO(&readHandleSet);
	FD_SET (handle, &readHandleSet);

	int ret = ACE_OS::select ((int)(handle+1), &readHandleSet, 0, 0, &tv);

	//Check for timeout.
	if (ret >= 0)
	{
		//Check if the handle is set.
		if ( FD_ISSET(handle, &readHandleSet))
		{
			if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceInformation("Out 'JTP_Session::pollReply(...) success.");
			}
			return true;
		}
		else
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::pollReply. handle is not signalled.");
			}
			return false;
		}
	}
	else
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::pollReply. select returned failure.");
		}
		return false;
	}
}

/*===================================================================
   ROUTINE: delay
=================================================================== */
void JTP_Session::delay(int sleepTime)
{
	if (sleepTime > 0)
	{
		ACE_OS::sleep(sleepTime);
	}
}

/*===================================================================
   ROUTINE: setNoOfTries
=================================================================== */
void JTP_Session::setNoOfTries(int Tries)
{
	NoOfTries = Tries;
}

/*===================================================================
   ROUTINE: setDelayTime
=================================================================== */
void JTP_Session::setDelayTime(int Sec)
{
	DelayTime = Sec;
}

/*===================================================================
   ROUTINE: setTimeOut
=================================================================== */
void JTP_Session::setTimeOut(int Sec)
{
	TimeOut = Sec;
}

/*===================================================================
   ROUTINE: Side
=================================================================== */
JTP_Session::JTP_Side JTP_Session::Side() const
{
	return JSide;
}

/*===================================================================
   ROUTINE: State
=================================================================== */
JTP_Session::JTP_State JTP_Session::State() const
{
	return JState;
}

/*===================================================================
   ROUTINE: fetch_conf
=================================================================== */
bool JTP_Session::fetch_conf()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::fetch_conf()': Session Info = '%s'", statusStr);
	}

	int nBytes = 0;

	if (!mySession) 
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::fetch_conf()': mySession is NULL.");
		}
		return false; //mySession is NULL.
	}
	else
	{

		nBytes = mySession->recv(Buffer, sizeof(Buffer));
		if( nBytes < 0)
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::fetch_conf()':recv failed.");
			}
			return false; //recv failed
		}
	}

	//Recv true check nBytes
	if(nBytes == 0)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::fetch_conf()':nBytes is equal to zero.");
		}
		return false; //peer session closed
	}

	//recv ok
	char M;
	//read buffer
	ACE_OS::memcpy(&M, &Buffer[0], 1);
	ACE_OS::memcpy(&ProtocolVersion, &Buffer[1], 1);
	ACE_OS::memcpy(&Au1, &Buffer[2], 2);
	ACE_OS::memcpy(&Au2, &Buffer[4], 2);
	ACE_OS::memcpy(&Ar, &Buffer[6], 2);
	u1 = AXE2h(Au1);  // Convert AXE format to Unix short
	u2 = AXE2h(Au2);

	helperTraceInformation("Printing 'JTP_Session::fetch_conf()': U1 = %hu, U2 = %hu, r = %hu", u1, u2, r);

	//check JType
	switch (JType)
	{
		case JTPJob:
		{
			if (M != JINITRSP)
			{
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::fetch_conf()':Message does not correspond to jinitrsp.");
				}
				return false;
			}
			break;
		}
		case JTPConversation:
		{
			if (M != JEXINITRSP)
			{
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::fetch_conf()':Message does not correspond to jexinitrsp.");
				}
				return false;
			}
			break;
		}
		default:
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::fetch_conf()':Message neither corresponds to job nor conversation.");
			}
			return false;
			break;
		}
	}

	//check Protocol version
	if (ProtocolVersion > 1)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::fetch_conf()':Invalid protocol version recieved in the message.");
		}
		return false;
	}

	r = AXE2h(Ar);
	helperTraceInformation("Printing 'JTP_Session::fetch_conf()': r = %hu", r);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::fetch_conf()': Session Info = '%s', r = %hu", statusStr, r);
	}

	return (r ? false : true);
}

/*===================================================================
   ROUTINE: reset
=================================================================== */
void JTP_Session::reset()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::reset()': Session Info = '%s'", statusStr);
	}

	if ((JSide == JTPClient) && (JType == JTPConversation))
	{
		JSLAddDisconnected(this);
	}
	if ((JSide == JTPServer) && (JType == JTPConversation))
	{
		JSLRemoveSession(this);
	}

	if (mySession)
	{
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			ACS_DSD_Node remoteNode;
			mySession->get_remote_node(remoteNode);
			helperTraceWarning("In 'JTP_Session::reset()': Destroying internal global session. Session Info = '%s', Remote Node Id = %u",
					statusStr, remoteNode.system_id);
		}
		mySession->close(); //shutdown session
		delete mySession;	//close the underlying socket
		mySession = 0;
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::reset()': Session Info = '%s'", statusStr);
	}
	JState = StateCreated; //set state
}

/*===================================================================
   ROUTINE: jexinitconf
=================================================================== */
bool JTP_Session::jexinitconf(unsigned short & U1, unsigned short & U2, unsigned short & R)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitconf(...': Session Info = '%s', U1 = %hu, U2 = %hu, R = %hu",
				statusStr, U1, U2, R);
	}

	if ((IntState != ClientConv) || (JState != StateConnected))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexinitconf(...': BAD STATE. Session Info = '%s', Source Line = %u", statusStr, __LINE__);
		}
		reset();
		return false;
	}

	U1 = u1;
	U2 = u2;
	R  = r;
	JState = StateJobRunning;  //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexinitconf(...': Session Info = '%s'", statusStr);
	}

	return true;

}

/*===================================================================
   ROUTINE: jexinitind_read
=================================================================== */
void JTP_Session::jexinitind_read()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitind_read()': %s", statusStr);
	}
	//read buffer
	ACE_OS::memcpy(&Au1, &Buffer[2], 2);
	ACE_OS::memcpy(&Au2, &Buffer[4], 2);
	ACE_OS::memcpy(&AMaxBuf, &Buffer[6], 2);
	ACE_OS::memcpy(&APNo, &Buffer[10], 1);
	ACE_OS::memcpy(&APRestartCntr, &Buffer[11], 1);
	ACE_OS::memcpy(&PID, &Buffer[12], 2);
	ACE_OS::memcpy(&ConnectionCntr, &Buffer[14], 2);
	u1 = AXE2h(Au1);
	u2 = AXE2h(Au2);
	MaxBuf = AXE2h(AMaxBuf);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexinitind_read()': "
				"First data: APNo = %u, APRestartCntr = %u, PID = %hu, ConnectionCntr = %hu, u1 = %hu, u2 = %hu, MaxBuf = %hu. "
				"Session Info = '%s'",
				static_cast<unsigned>(APNo), static_cast<unsigned>(APRestartCntr), PID, ConnectionCntr, u1, u2, MaxBuf, statusStr);
	}

}

/*===================================================================
   ROUTINE: jexinitind_rcv
=================================================================== */
bool JTP_Session::jexinitind_rcv(long Size)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitind_rcv(...': Session Info = '%s', Size = %ld", statusStr, Size);
	}
	unsigned short nbytes = (unsigned short)Size;

	if (!jtpRecvMsg(&Buffer[6], nbytes))
	{
		//recv failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::jexinitind_rcv(...': 'jtpRecvMsg(&Buffer[6], nbytes)' was false!");
		}
		return false;
	}

	//recv ok
	ACE_OS::memcpy(&Au1, &Buffer[6], 2);
	ACE_OS::memcpy(&Au2, &Buffer[8], 2);
	ACE_OS::memcpy(&AMaxBuf, &Buffer[10], 2);
	ACE_OS::memcpy(&APNo, &Buffer[14], 1);
	ACE_OS::memcpy(&APRestartCntr, &Buffer[15], 1);
	ACE_OS::memcpy(&PID, &Buffer[16], 2);
	ACE_OS::memcpy(&ConnectionCntr, &Buffer[18], 2);

	u1 = AXE2h(Au1);
	u2 = AXE2h(Au2);
	MaxBuf = AXE2h(AMaxBuf);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitind_rcv(long Size)': "
				"First data: APNo = %u, APRestartCntr = %u, PID = %hu, ConnectionCntr = %hu, u1 = %hu, u2 = %hu, MaxBuf = %hu. "
				"Session Info = '%s'",
				static_cast<unsigned>(APNo), static_cast<unsigned>(APRestartCntr), PID, ConnectionCntr, u1, u2, MaxBuf, statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexinitrsp_snd
=================================================================== */
bool JTP_Session::jexinitrsp_snd()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitrsp_snd()': Session Info = '%s'", statusStr);
	}

	if (!mySession)
	{
		return false;
	}

	unsigned int nbytes = (ProtocolVersion ? 8 : 9);

	char M = JEXINITRSP;
	int TypeOfDataTrans = 0;
	Au1 = h2AXE(u1);
	Au2 = h2AXE(u2);
	Ar = h2AXE(r);

	//copy in buffer
	ACE_OS::memcpy(&Buffer[0], &M, 1);
	ACE_OS::memcpy(&Buffer[1], &ProtocolVersion, 1);
	ACE_OS::memcpy(&Buffer[2], &Au1, 2);
	ACE_OS::memcpy(&Buffer[4], &Au2, 2);
	ACE_OS::memcpy(&Buffer[6], &Ar, 2);
	ACE_OS::memcpy(&Buffer[8], &TypeOfDataTrans, 1);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitrsp_snd()': Session Info = '%s'", statusStr);
	}

	//sending
	if (!mySession || ((mySession->send(Buffer, nbytes)) <= 0))
	{
		//send failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexinitrsp_snd()': 'mySession->send(Buffer, nbytes)' failed! Session Info = '%s', mySession->last_error() = '%d' ,mySession->last_error_text() = '%s'", statusStr, mySession->last_error(), mySession->last_error_text());
		}
		return false;
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexinitrsp_snd()': 'mySession->send(Buffer, nbytes)' OK! Session Info = '%s'", statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexinitrsp
=================================================================== */
bool JTP_Session::jexinitrsp(short U1, short U2, short R)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitrsp(...': Session Info = '%s', U1 = %hd, U2 = %hd, R = %hd",
				statusStr, U1, U2, R);
	}

	if ((IntState != ServerConv) || (JState != StateAccepted) || ((R > 0) && (R < 4)))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexinitrsp(...': BAD STATE! Session Info = '%s', U1 = %hd, U2 = %hd, R = %hd",
					statusStr, U1, U2, R);
		}

		reset();
		return false;
	}

	u1 = U1;
	u2 = U2;
	r  = R;

	//sending
	if (!jexinitrsp_snd())
	{
		//jexinitrsp failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jexinitrsp(...': 'jexinitrsp_snd()' failed! Session Info = '%s', U1 = %hd, U2 = %hd, R = %hd. SOURCE LINE = %u",
					statusStr, U1, U2, R, __LINE__);
		}

		reset();
		return false;
	}

	if (R != 0)
	{ //negative case
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jexinitrsp(...': CHECK ERROR 'R != 0'! Session Info = '%s', U1 = %hd, U2 = %hd, R = %hd. SOURCE LINE = %u",
					statusStr, U1, U2, R, __LINE__);
		}

		reset();
		return false;
	}

	JState = StateWaitForData;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexinitrsp(...': Session Info = '%s'", statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexdatareq_snd
=================================================================== */
bool JTP_Session::jexdatareq_snd() {

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexdatareq_snd()': Session Info = '%s'", statusStr);
	}


	unsigned int nbytes;
	char M;

	//check protocol version
	if (ProtocolVersion)
	{
		nbytes = 28 + BufLen; 
		M = JEXDATAREQ;
	}
	else
	{
		nbytes = 8 + BufLen; 
		M = JEXDATAREQ_WR;
	}

	Au1 = h2AXE(u1);
	Au2 = h2AXE(u2);
	ABufLen = h2AXE(BufLen);

	//copy
	ACE_OS::memcpy(&Buffer[0], &M, 1);
	ACE_OS::memcpy(&Buffer[2], &Au1, 2);
	ACE_OS::memcpy(&Buffer[4], &Au2, 2);
	ACE_OS::memcpy(&Buffer[6], &ABufLen, 2);

	//sending
	if (!mySession)
	{
		return false;
	}
	else if((mySession->send(Buffer, nbytes)) <= 0 )
        {
                //send failed
                if (ACS_JTP_TRACE_ERROR_ACTIVE)
                {
                        char statusStr[256] = {0};
                        getSessionInformation(statusStr, sizeof(statusStr));
                        helperTraceError("Out 'JTP_Session::jexdatareq_snd()': ERROR ASSERTION(!mySession || !mySession->send(Buffer, nbytes)). Session Info = '%s' ,mySession->last_error() = '%d' , mySession->last_error_text() = '%s' ", statusStr, mySession->last_error(), mySession->last_error_text());
                }
                return false;
        }
	

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexdatareq_snd()': Successful! Session Info = '%s'", statusStr);
	}
	return true;
}

/*===================================================================
   ROUTINE: jexdatareq
=================================================================== */
bool JTP_Session::jexdatareq(short U1, short U2, short BufferLength, char* Buf) {

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexdatareq(...': Session Info = '%s', U1 = %hd, U2 = %hd, BufferLength = %hd", statusStr, U1, U2, BufferLength);
	}

	if ((JType != JTPConversation) || (JState != StateJobRunning))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexdatareq(...': ERROR ASSERTION((JType != JTPConversation) || (JState != StateJobRunning)). Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	u1 = U1;
	u2 = U2;
	BufLen = BufferLength;
	//check protocol version
	if (ProtocolVersion == 1)
	{
		ACE_OS::memcpy(&Buffer[28], Buf, BufLen);
	}
	else
	{
		if (BufLen < 13)
		{
			ACE_OS::memcpy(&Buffer[8], Buf, BufLen);
		}
		else
		{
			char saveBuf[8]; //new version
			ACE_OS::memcpy(saveBuf, Buf, 8); //new version
			ACE_OS::memcpy(&Buffer[8], &Buf[8], BufLen - 8);//new version
			ACE_OS::memcpy(&Buffer[BufLen], saveBuf, 8); //new version
		}
	}

	//sending
	if (!jexdatareq_snd())
	{
		//negative case jexdatareq_snd failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexdatareq(...': ERROR ASSERTION(!jexdatareq_snd()). Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	if (JSide == JTPClient)
	{
		ACE_HANDLE handle = ACE_INVALID_HANDLE;
		//send ok
		if ( mySession != 0)
		{
			int noOfHandles = 1;
			int resHandles = mySession->get_handles(&handle, noOfHandles);
			if( resHandles >= 0)
			{
				if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceInformation("In 'JTP_Session::jexdatareq()': retrieved handle for session", statusStr);
				}
			}
			else
			{
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceError("In 'JTP_Session::jexdatareq()': ERROR ASSERTION (Handle is invalid, Session Info = '%s'", statusStr);
				}
				reset();
				return false;
			}
		}
		if (!pollReply(handle, true))
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError("Out 'JTP_Session::jexdatareq(...': ERROR ASSERTION(!pollReply(handle, true)). Session Info = '%s'. SOURCE LINE = %u",
						statusStr, __LINE__);
			}
			reset();
			return false;
		}
	}

	JState = StateWaitForData; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexdatareq(...': Session Info = '%s'", statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexdataind_read
=================================================================== */
bool JTP_Session::jexdataind_read() {

	bool res = true;
	char M;

	M = 32;
	//reading buffer
	ACE_OS::memcpy(&M, &Buffer[0], 1);
	ACE_OS::memcpy(&Au1, &Buffer[2], 2);
	ACE_OS::memcpy(&Au2, &Buffer[4], 2);
	ACE_OS::memcpy(&ABufLen, &Buffer[6], 2);
	u1 = AXE2h(Au1);	// Convert AXE format to Unix short
	u2 = AXE2h(Au2);

	switch (M)
	{
		case JEXDATAREQ:
		{
			BufLen = AXE2h(ABufLen);
			break;
		}
		case JEXDISCREQ:
		{
			r = AXE2h(ABufLen);
			res = false;
			break;
		}
		default:
		{
			r = TranspErr;
			res = false;
			break;
		}
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexdataind_read()': Session Info = '%s', M: 0x%X, u1: %hu, u2: %hu, BufLen: %hu, r: %hu",
				statusStr, static_cast<int>(M), u1, u2, BufLen, r);
	}

	return res;
}

/*===================================================================
   ROUTINE: jexdataind_rcv
=================================================================== */
bool JTP_Session::jexdataind_rcv(long Size)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexdataind_rcv(long Size)': Session Info = '%s', Size = %ld", statusStr, Size);
	}

	bool res = true;
	char M = 32; //new

	unsigned short nbytes = (unsigned short)Size;

	if (jtpRecvMsg(&Buffer[5], nbytes))
	{
		ACE_OS::memcpy(&M, &Buffer[4], 1);
		ACE_OS::memcpy(&Au1, &Buffer[6], 2);
		ACE_OS::memcpy(&Au2, &Buffer[8], 2);
		ACE_OS::memcpy(&ABufLen, &Buffer[10], 2);

		u1 = AXE2h(Au1);		// Convert AXE format to Unix short
		u2 = AXE2h(Au2);
		//check M
		switch (M)
		{
			case JEXDATAREQ:
			{
				BufLen = AXE2h(ABufLen);
				break;
			}
			case JEXDISCREQ:
			{
				r = AXE2h(ABufLen);
				res = false;
				break;
			}
			default:
			{
				r = TranspErr;
				res = false;
				break;
			}
		}
	}
	else
	{
		r = TranspErr;
		res = false;
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jexdataind_rcv(long Size)': Session Info = '%s', M = 0x%X, u1 = %hu, u2 = %hu, BufLen = %hu, r = %hu",
				statusStr, static_cast<int>(M), u1, u2, BufLen, r);
	}

	return res;
}

/*===================================================================
   ROUTINE: jexdataind
=================================================================== */
bool JTP_Session::jexdataind(unsigned short& U1, unsigned short& U2, unsigned short& BufferLength, char*& Buf)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexdataind(...': Session Info = '%s'", statusStr);
	}

	//reset parameters
	U1 = 0;
	U2 = 0;
	BufferLength = 0;
	Buf = 0;

	if ((JType != JTPConversation) || (JState != StateWaitForData))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jexdataind(...': ERROR ASSERTION((JType != JTPConversation) || (JState != StateWaitForData)). Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	//receiving
	int nbytes = 0;
	if (!mySession )
	{
		return false;
	}
	else if((nbytes = mySession->recv(Buffer, sizeof(Buffer))) < 0 )
	{
			//recv failed
                r = TranspErr;
                if (ACS_JTP_TRACE_ERROR_ACTIVE)
                {
                        char statusStr[256] = {0};
                        getSessionInformation(statusStr, sizeof(statusStr));
                        helperTraceError(
                                        "Out 'JTP_Session::jexdataind(...': ERROR ASSERTION(!mySession || mySession->recv(Buffer,sizeof(Buffer))). Session Info = '%s'. SOURCE LINE = %u, errno= %d, errorText = '%s'",
                                        statusStr, __LINE__, mySession->last_error(), mySession->last_error_text());
                }

                reset();
                return false;
	
	}


	//recv = true check nbytes
	if(nbytes == 0)
	{
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceWarning("Out 'JTP_Session::jexdataind(...': WARNING ASSERTION(nbytes == 0). Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		//session closed
		reset();
		return false;
	}

	//recev ok reading buffer
	if(!jexdataind_read())
	{ //reading failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexdataind(...': ERROR ASSERTION(!jexdataind_read()). Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	//reading ok
	U1 = u1;
	U2 = u2;
	BufferLength = BufLen;
	//check protocol version
	if (ProtocolVersion == 1)
	{
		Buf = &Buffer[28];
	}
	else
	{
		Buf = &Buffer[8]; //new version
	}
	JState = StateJobRunning; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jexdataind(...': Successful. Session Info = '%s', U1 = %hu, U2 = %hu, BufferLength = %hu",
				statusStr, U1, U2, BufferLength);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexdiscreq
=================================================================== */
bool JTP_Session::jexdiscreq(short U1, short U2, short R)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexdiscreq(...': Session Info = '%s', U1 = %hd, U2 = %hd, R = %hd",
				statusStr, U1, U2, R);
	}

	if ((JType != JTPConversation) || ((JState != StateWaitForData) && (JState != StateJobRunning)))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jexdiscreq(...': ERROR ASSERTION((JType != JTPConversation) || ((JState != StateWaitForData) && (JState != StateJobRunning)))! Session Info = '%s', JType = '%s', JState = '%s'",
					statusStr, JTypeAsString(), JStateAsString());
		}
		reset();
		return false;
	}

	u1 = U1;
	u2 = U2;
	r  = R;
	char M = JEXDISCREQ;
	Au1 = h2AXE(u1);
	Au2 = h2AXE(u2);
	Ar = h2AXE(r);
	//copy
	ACE_OS::memcpy(&Buffer[0], &M, 1);
	ACE_OS::memcpy(&Buffer[2], &Au1, 2);
	ACE_OS::memcpy(&Buffer[4], &Au2, 2);
	ACE_OS::memcpy(&Buffer[6], &Ar, 2);

	
	//sending
	unsigned int nbytes = 8; 
	if (!mySession )	
	{
		return false;
	}
	else if((mySession->send(Buffer, nbytes)) <= 0 )	
	{
		return false;
	}
	reset(); //shutdown session
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexdiscreq(...': Session Info = '%s'", statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexdiscind_read
=================================================================== */
bool JTP_Session::jexdiscind_read()
{
	char M;

	//reading
	ACE_OS::memcpy(&M, &Buffer[0], 1);
	ACE_OS::memcpy(&Au1, &Buffer[2], 2);
	ACE_OS::memcpy(&Au2, &Buffer[4], 2);
	ACE_OS::memcpy(&Ar,  &Buffer[6], 2);

	if (M != JEXDISCREQ)
	{
		return false;
	}

	u1 = AXE2h(Au1);		// Convert AXE format to unix short
	u2 = AXE2h(Au2);
	r  = AXE2h(Ar);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jexdiscind_read()': Session Info = '%s', u1 = %hu, u2 = %hu, r = %hu",
				statusStr, u1, u2, r);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexdiscind_rcv
=================================================================== */
bool JTP_Session::jexdiscind_rcv(long Size)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexdiscind_rcv(long Size)': Session Info = '%s', Size = %ld", statusStr, Size);
	}

	char M;
	unsigned short nbytes = (unsigned short)Size;
	if (!jtpRecvMsg(&Buffer[6], nbytes))
	{
		//recv failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Session::jexdiscind_rcv(long Size)': 'jtpRecvMsg(&Buffer[6], nbytes)' was false!");
		}
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceWarning("In 'JTP_Session::jexdiscind_rcv(long Size)': Closing session. Session Info = '%s', Size = %ld", statusStr, Size);
		}
		reset();
		return false;
	}

	//recv ok
	ACE_OS::memcpy(&M, &Buffer[4], 1);
	ACE_OS::memcpy(&Au1, &Buffer[6], 2);
	ACE_OS::memcpy(&Au2, &Buffer[8], 2);
	ACE_OS::memcpy(&Ar,  &Buffer[10], 2);

	if (M != JEXDISCREQ)
	{
		return false;
	}

	u1 = AXE2h(Au1);		// Convert AXE format to Unix short
	u2 = AXE2h(Au2);
	r  = AXE2h(Ar);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jexdiscind_rcv(long Size)': Session Info = '%s', u1 = %hu, u2 = %hu, r = %hu", statusStr, u1, u2, r);
	}

	return true;
}

/*===================================================================
   ROUTINE: jexdiscind
=================================================================== */
bool JTP_Session::jexdiscind(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexdiscind(...': Session Info = '%s'", statusStr);
	}

	if ((JType != JTPConversation) ||
			((JState != StateWaitForData) &&
			 (JState != StateJobRunning)  &&
			 (JState != StateCreated)))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexdiscind(...': ERROR ASSERTION((JType != JTPConversation) || ((JState != StateWaitForData) && (JState != StateJobRunning)  && (JState != StateCreated))). Session Info = '%s'", statusStr);
		}
		reset();
		return false;
	}

	if (JState == StateCreated)
	{
		U1 = u1;
		U2 = u2;
		R  = r;
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceInformation(
					"In 'JTP_Session::jexdiscind(...': SUCCESS ASSERTION(JState == StateCreated). Session Info = '%s', U1 = %hu, U2 = %hu, R = %hu",
					statusStr, U1, U2, R);
		}

		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceWarning(
					"Leaving 'JTP_Session::jexdiscind(...': Closing session. "
					"Session Info = '%s', U1 = %hu, U2 = %hu, R = %hu",
					statusStr, U1, U2, R);
		}

		reset();
		return true;
	}

	//receiving	
	int nbytes = 0;

	if (!mySession || ((nbytes = mySession->recv(Buffer, sizeof(Buffer))) < 0 ))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jexdiscind(...': ERROR ASSERTION(!mySession || !mySession->recv(Buffer, sizeof(Buffer))). Session Info = '%s'", statusStr);
		}
		//Receive failed
		reset();
		return false;
	}

	//recv = true check nbytes
	if(nbytes == 0)
	{
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceWarning("Out 'JTP_Session::jexdiscind(...': WARNING ASSERTION(nbytes == 0), session closed by peer. Session Info = '%s'",
					statusStr);
		}
		//session closed
		reset();
		return false;
	}

	// recv reading buffer
	if(!jexdiscind_read())
	{ //reading failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexdiscind(...': ERROR ASSERTION(!jexdiscind_read()), read failed. Session Info = '%s'",
					statusStr);
		}
		reset();
		return false;
	}

	//reading ok //set parameters
	U1 = u1;
	U2 = u2;
	R  = r;
	reset();

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jexdiscind(...': Success. Session Info = '%s', U1 = %hu, U2 = %hu, R = %hu",
				statusStr, U1, U2, R);
	}

	return true;
}

/*===================================================================
   ROUTINE: jinitconf
=================================================================== */
bool JTP_Session::jinitconf(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jinitconf(...': Session Info = '%s'", statusStr);
	}

	if ((IntState != ClientJob) || (JState != StateConnected))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jinitconf(...': ERROR ASSERTION((IntState != ClientJob) || (JState != StateConnected)), bad state. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	U1 = u1;
	U2 = u2;
	R  = r;
	JState = StateJobRunning; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jinitconf(...': Session Info = '%s', U1 = %hu, U2 = %hu, R = %hu",
				statusStr, U1, U2, R);
	}

	return true;
}

/*===================================================================
   ROUTINE: jinitind_read
=================================================================== */
void JTP_Session::jinitind_read()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"In 'JTP_Session::jinitind_read()':%s ", statusStr);
	}
	//read buffer
	ACE_OS::memcpy(&Au1, &Buffer[2], 2);
	ACE_OS::memcpy(&Au2, &Buffer[4], 2);
	ACE_OS::memcpy(&ABufLen, &Buffer[6], 2);
	ACE_OS::memcpy(&AMaxBuf, &Buffer[8], 2);
	u1 = AXE2h(Au1); //Convert AXE format to Unix short
	u2 = AXE2h(Au2);
	BufLen = AXE2h(ABufLen);
	MaxBuf = AXE2h(AMaxBuf);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jinitind_read()': Session Info = '%s', u1 = %hu, u2 = %hu, BufLen = %hu, MaxBuf = %hu",
				statusStr, u1, u2, BufLen, MaxBuf);
	}
}

/*===================================================================
   ROUTINE: jinitind_rcv
=================================================================== */
bool JTP_Session::jinitind_rcv(long Size)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jinitind_rcv(long Size)': Session Info = '%s', Size = %ld",
				statusStr, Size);
	}

	unsigned short nbytes = (unsigned short)Size;
	if (!jtpRecvMsg(&Buffer[6], nbytes))
	{
		return false; //recv failed
	}

	//recv ok
	ACE_OS::memcpy(&Au1, &Buffer[6], 2);
	ACE_OS::memcpy(&Au2, &Buffer[8], 2);
	ACE_OS::memcpy(&ABufLen, &Buffer[10], 2);
	ACE_OS::memcpy(&AMaxBuf, &Buffer[12], 2);

	u1 = AXE2h(Au1);		// Convert AXE format to Unix short
	u2 = AXE2h(Au2);
	BufLen = AXE2h(ABufLen);
	MaxBuf = AXE2h(AMaxBuf);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jinitind_rcv(long Size)': Session Info = '%s', u1 = %hu, u2 = %hu, BufLen = %hu, MaxBuf = %hu",
				statusStr, u1, u2, BufLen, MaxBuf);
	}

	return true;

}

/*===================================================================
   ROUTINE: jinitrsp_snd
=================================================================== */
bool JTP_Session::jinitrsp_snd()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jinitrsp_snd()': Session Info = '%s'", statusStr);
	}

	char M = JINITRSP;
	Au1 = h2AXE(u1);
	Au2 = h2AXE(u2);
	Ar = h2AXE(r);

	//copy data in buffer
	ACE_OS::memcpy(&Buffer[0], &M, 1);
	ACE_OS::memcpy(&Buffer[1], &ProtocolVersion, 1);
	ACE_OS::memcpy(&Buffer[2], &Au1, 2);
	ACE_OS::memcpy(&Buffer[2], &Au2, 2);
	ACE_OS::memcpy(&Buffer[6], &Ar, 2);
	int nbytes = 8;

	if (!mySession)		
	{
		return false;
	}
	else if((mySession->send(Buffer, nbytes)) <= 0 )	
        {
                //send failed
                if (ACS_JTP_TRACE_ERROR_ACTIVE)
                {
                        char statusStr[256] = {0};
                        getSessionInformation(statusStr, sizeof(statusStr));
                        helperTraceError(
                                        "Out 'JTP_Session::jinitrsp_snd()': ERROR ASSERTION(!mySession || !mySession->send(Buffer, nbytes)), send failed. Session Info = '%s'. SOURCE LINE = %u, mySession->last_error() = %d, mySession->last_error_text() = '%s'",
                                        statusStr, __LINE__, mySession->last_error(),  mySession->last_error_text());
                }
                return false;
        }



	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jinitrsp_snd()': SUCCESSFUL. Session Info = '%s'", statusStr);
	}
	return true;
}

/*===================================================================
   ROUTINE: jinitrsp
=================================================================== */
bool JTP_Session::jinitrsp(short U1, short U2, short R)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"In 'JTP_Session::jinitrsp(...': Session Info = '%s', U1 = %hd, U2 = %hd, R = %hd",
				statusStr, U1, U2, R);
	}

	if ((IntState != ServerJob) || (JState != StateAccepted))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jinitrsp(...': ERROR ASSERTION((IntState != ServerJob) || (JState != StateAccepted)), bad state. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	u1 = U1;
	u2 = U2;
	r  = R;

	//sending
	if (!jinitrsp_snd())
	{
		//jinitrsp_snd failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jinitrsp(...': ERROR ASSERTION(!jinitrsp_snd()), send failed. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	JState = StateJobRunning; //set state

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jinitrsp(...': Session Info = '%s'", statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jresultreq_snd
=================================================================== */
bool JTP_Session::jresultreq_snd()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jresultreq_snd()': Session Info = '%s'", statusStr);
	}
	char M = JRESULTREQ;
	char Zero = 0;
	Au1 = h2AXE(u1);
	Au2 = h2AXE(u2);
	Ar = h2AXE(r);
	ABufLen = h2AXE(BufLen);

	//copy in buffer
	ACE_OS::memcpy(&Buffer[0], &M, 1);
	ACE_OS::memcpy(&Buffer[1], &Zero, 1);
	ACE_OS::memcpy(&Buffer[2], &Au1, 2);
	ACE_OS::memcpy(&Buffer[4], &Au2, 2);
	ACE_OS::memcpy(&Buffer[6], &Ar, 2);
	ACE_OS::memcpy(&Buffer[8], &ABufLen, 2);
	unsigned int nbytes = 28 + BufLen;

	if (!mySession)		
	{
		return false;
	} 
	else if ((mySession->send(Buffer, nbytes)) <= 0 )	
        {
                //send failed
                if (ACS_JTP_TRACE_ERROR_ACTIVE)
                {
                        char statusStr[256] = {0};
                        getSessionInformation(statusStr, sizeof(statusStr));
                        helperTraceError("Out 'JTP_Session::jresultreq_snd()': ERROR ASSERTION(!mySession || !mySession->send(Buffer, nbytes)), send failed. Session Info = '%s', mySession->last_error() = %d, mySession->last_error_text() = '%s'",
                                        statusStr, mySession->last_error(), mySession->last_error_text());
                }
                return false;
        }
	

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jresultreq_snd()': SUCCESSFUL. Session Info = '%s'", statusStr);
	}

	return true;
}

/*===================================================================
   ROUTINE: jresultreq
=================================================================== */
bool JTP_Session::jresultreq(short U1, short U2, short R, short BufferLength, char* Buf)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"In 'JTP_Session::jresultreq(...': Session Info = '%s', U1 = %hd, U2 = %hd, R = %hd, BufferLength = %hd",
				statusStr, U1, U2, R, BufferLength);
	}

	if ((IntState != ServerJob) || (JState != StateJobRunning))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jresultreq(...': ERROR ASSERTION ((IntState != ServerJob) || (JState != StateJobRunning)), bad state. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	u1 = U1;
	u2 = U2;
	r = R;
	BufLen = BufferLength;
	ACE_OS::memcpy(&Buffer[28], Buf, BufLen);

	//sending 
	if (jresultreq_snd())
	{
		reset();
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceInformation("Out 'JTP_Session::jresultreq(...': SUCCESSFUL. Session Info = '%s'", statusStr);
		}
		return true;
	}

	if (ACS_JTP_TRACE_ERROR_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceError(
				"Out 'JTP_Session::jresultreq(...': ERROR ASSERTION (jresultreq_snd()), jresultreq_snd() is false. Session Info = '%s'. SOURCE LINE = %u",
				statusStr, __LINE__);
	}

	return false;
}

/*===================================================================
   ROUTINE: jresultind_read
=================================================================== */
bool JTP_Session::jresultind_read() {

	char M;
	//reading buffer
	ACE_OS::memcpy(&M, &Buffer[0], 1);
	ACE_OS::memcpy(&Au1, &Buffer[2], 2);
	ACE_OS::memcpy(&Au2, &Buffer[4], 2);
	ACE_OS::memcpy(&Ar, &Buffer[6], 2);
	ACE_OS::memcpy(&ABufLen, &Buffer[8], 2);

	if (M != JRESULTREQ)
	{
		return false;
	}

	u1 = AXE2h(Au1);		// Convert AXE format to Unix short
	u2 = AXE2h(Au2);
	r = AXE2h(Ar);
	BufLen = AXE2h(ABufLen);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jresultind_read()': Session Info = '%s', u1 = %hu, u2 = %hu, r = %hu, BufLen = %hu",
				statusStr, u1, u2, r, BufLen);
	}

	return true;
}

/*===================================================================
   ROUTINE: jresultind_rcv
=================================================================== */
bool JTP_Session::jresultind_rcv(long Size) {

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jresultind_rcv(long Size)': Session Info = '%s', Size = %ld", statusStr, Size);
	}

	char M;
	unsigned short nbytes = (unsigned short)Size;

	if (!jtpRecvMsg(&Buffer[5], nbytes))
	{
		return false;
	}

	//recv ok
	ACE_OS::memcpy(&M, &Buffer[4], 1);
	ACE_OS::memcpy(&Au1, &Buffer[6], 2);
	ACE_OS::memcpy(&Au2, &Buffer[8], 2);
	ACE_OS::memcpy(&Ar, &Buffer[10], 2);
	ACE_OS::memcpy(&ABufLen, &Buffer[12], 2);

	if (M != JRESULTREQ)
	{
		return false;
	}

	u1 = AXE2h(Au1);		// Convert AXE format to Unix short
	u2 = AXE2h(Au2);
	r  = AXE2h(Ar);
	BufLen  = AXE2h(ABufLen);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jresultind_rcv(long Size)': SUCCESSFUL. Session Info = '%s', u1 = %hu, u2 = %hu, r = %hu, BufLen = %hu",
				statusStr, u1, u2, r, BufLen);
	}

	return true;
}

/*===================================================================
   ROUTINE: jresultind
=================================================================== */
bool JTP_Session::jresultind(unsigned short& U1, unsigned short& U2, unsigned short& R, unsigned short& BufferLength, char*& Buf)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jresultind(...': Session Info = '%s'", statusStr);
	}

	if ((IntState != ClientJob) || (JState != StateJobRunning))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jresultind(...': ERROR ASSERTION ((IntState != ClientJob) || (JState != StateJobRunning)), bad state. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		reset();
		return false;
	}

	r = TranspErr;

	//receiving	
	int nbytes = 0;

	if (!mySession || ((nbytes = mySession->recv(Buffer,sizeof(Buffer))) < 0 ))
	{
		//recv failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jresultind(...': ERROR ASSERTION (!mySession || mySession->recv(Buffer, sizeof(Buffer))), bad state. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	//recv = true check nbytes
	if(nbytes == 0)
	{
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceWarning("Out 'JTP_Session::jresultind(...': WARNING ASSERTION (nbytes == 0), session closed by peer. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}

		reset();
		return false;
	}

	//recv ok reading buffer
	if(!jresultind_read())
	{
		//read failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jresultind(...': ERROR ASSERTION (!jresultind_read()), read failed. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}

	//reading ok
	U1 = u1;
	U2 = u2;
	R = r;
	BufferLength = BufLen;
	Buf = &Buffer[28];
	JState = StateCreated; //set state


	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jresultind(...': SUCCESSFUL. Session Info = '%s', U1 = %hu, U2 = %hu, R = %hu, BufferLength = %hu",
				statusStr, U1, U2, R, BufferLength);
	}
	reset();
	return true;
}

/*===================================================================
   ROUTINE: jtermreq
=================================================================== */
bool JTP_Session::jtermreq()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jtermreq()': Session Info = '%s'", statusStr);
	}

	if ((IntState != ClientJob) || (JState != StateJobRunning))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"Out 'JTP_Session::jtermreq()': ERROR ASSERTION ((IntState != ClientJob) || (JState != StateJobRunning)), bad state. Session Info = '%s'. SOURCE LINE = %u",
					statusStr, __LINE__);
		}
		reset();
		return false;
	}
	reset();

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("Out 'JTP_Session::jtermreq()': Session Info = '%s'", statusStr);
	}
	return true;
}

/*===================================================================
   ROUTINE: jfault
=================================================================== */
bool JTP_Session::jfault(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jfault(...': Session Info = '%s'", statusStr);
	}

	if ((IntState != ClientJob) || (JState != StateCreated))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jfault(...': ERROR ASSERTION ((IntState != ClientJob) || (JState != StateCreated)), bad state. Session Info = '%s'. SOURCE LINE = %u", statusStr, __LINE__);
		}
		reset();
		return false;
	}

	U1 = u1;
	U2 = u2;
	R  = r;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation(
				"Out 'JTP_Session::jfault(...': Session Info = '%s', U1 = %hu, U2 = %hu, R = %hu",
				statusStr, U1, U2, R);
	}
	return true;
}

/*===================================================================
   ROUTINE: getHandle
=================================================================== */
bool JTP_Session::getHandle(ACE_HANDLE& handle)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("In 'JTP_Session::getHandle");
	}
	if( mySession != 0 )
	{
		int noOfHandles = 1;

		//Call get_handles to get the handles from DSD.
		int result = mySession->get_handles(&handle, noOfHandles);

		if( result < 0)
		{
			helperTraceError("Out 'JTP_Session::getHandle(...':Error occured while getting handle.");
			return false;
		}
		else
		{
			helperTraceInformation("Out 'JTP_Session::getHandle successful.");
			return true;
		}
	}
	else
	{
		helperTraceError("Out 'JTP_Session::getHandle(...':Internal session pointer is NULL.");
		return false;
	}
}

/*===================================================================
   ROUTINE: JTPquery
=================================================================== */
bool JTP_Session::JTPquery(	const char* serviceName,
							const char* domain,
							acs_dsd::SystemTypeConstants scope,
							std::vector<ACS_DSD_Node>& nodes,
							std::vector<ACS_DSD_Node>& notReachNodes)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::JTPquery(...':Session Info = '%s'", statusStr);
	}

	int res = -1;

	if( pDSDClient != 0 )
	{
		res = pDSDClient->query(serviceName, domain, scope, nodes, notReachNodes);
	}
	if( res >= 0)
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceInformation("Out 'JTP_Session::JTPquery(...':query successful Session Info = '%s'", statusStr);
		}
		return true;
	}
	else
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::JTPquery(...': query unsuccessful. Session Info = '%s'", statusStr);
		}
		return false;
	}
}

/*===================================================================
   ROUTINE: JTPquery
=================================================================== */
bool JTP_Session::JTPquery(const char* serviceName,
							const char* domain,
							acs_dsd::SystemTypeConstants scope,
							std::vector<ACS_DSD_Node>& nodes,
							std::vector<ACS_DSD_Node>& notReachNodes,
							int milliseconds)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::JTPquery(...':Session Info = '%s'", statusStr);
	}

	int res = -1;

	if( pDSDClient != 0 )
	{
		res = pDSDClient->query(serviceName, domain, scope, nodes, notReachNodes, milliseconds);
	}
	if( res >= 0)
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceInformation("Out 'JTP_Session::JTPquery(...':query successful Session Info = '%s'", statusStr);
		}
		return true;
	}
	else
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::JTPquery(...': query unsuccessful. Session Info = '%s'", statusStr);
		}
		return false;
	}

}

/*===================================================================
   ROUTINE: jtpRecvMsg
=================================================================== */
bool JTP_Session::jtpRecvMsg(char* buf, unsigned short nbytes)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jtpRecvMsg(...': Session Info = '%s'", statusStr);
	}

	//Fix for the warning while compilation.
	(void)buf;

	if (readBufferLen == 0)
	{
		int msgSize = 0; //new

		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceInformation("In 'JTP_Session::jtpRecvMsg(...': INFORMATION ASSERTION (readBufferLen == 0). Session Info = '%s'", statusStr);
		}

		if (!mySession || ((msgSize = mySession->recv(Buffer, sizeof(Buffer))) <= 0 ))
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError("In 'JTP_Session::jtpRecvMsg(...': ERROR ASSERTION (!mySession || mySession->recv(Buffer, sizeof(Buffer))), receive failed. Session Info = '%s'", statusStr);

			}
			return false;
		}
		else
		{ //recv ok
			readBufferLen = msgSize; //new version
			bufferCursor  = 0;
		}
	}

	bufferCursor += nbytes;
	//check bufferCursor
	if (bufferCursor > readBufferLen)
	{
		return false;
	}
	else if (bufferCursor == readBufferLen)
	{
		readBufferLen = 0;
	}

	return true;
}

/*===================================================================
   ROUTINE: jinitreq_snd
=================================================================== */
int JTP_Session::jinitreq_snd()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jinitreq_snd()': Session Info = '%s'", statusStr);
	}

	int res = TranspErr;

	if (!pDSDClient)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jinitreq_snd()': ERROR ASSERTION (!pDSDClient), catastrophic error, DSD client not allocated. Session Info = '%s'", statusStr);
		}
		return res;
	}

	int moreTries = NoOfTries; //number of connect tries

	while ((moreTries) || (NoOfTries == 0))
	{
		if (moreTries != NoOfTries)
		{
			delay(DelayTime);
		}
		ACS_DSD_Session* S1 = new (std::nothrow) ACS_DSD_Session();

		if (!S1)
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError("Out 'JTP_Session::jinitreq_snd()': ERROR ASSERTION (!S1), catastrophic error, OUT OF MEMORY in DSD session allocation. Session Info = '%s'", statusStr);
			}
			return res;
		}


		// Connect to remote application.
		int resConnect =  pDSDClient->connect(*S1, this->ServiceName, "JTP", NodeSv->system_id, NodeSv->node_state);
		if (resConnect < 0)
		{ //connect failed
			--moreTries;

			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError(
						"In 'JTP_Session::jinitreq_snd()': ERROR ASSERTION [!pDSDClient->connect(...], DSD connect failed. "
						"DSD Last Error: code = %d, text = '%s'. moreTries = %d. Session Info = '%s'",
						pDSDClient->last_error(), pDSDClient->last_error_text(), moreTries, statusStr);
			}
			delete S1;
			S1 = 0;
			continue; //negative case: next iteration loop
		}

		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceInformation("In 'JTP_Session::jinitreq_snd()': Connection established with peer application. Session Info = '%s'", statusStr);
		}

		newJob(ServiceName, *S1);	// Copy Job data (init pointer mySession)
		unsigned int len = (28 + BufLen); //new version (18 bytes for zero)
		ProtocolVersion = 1;
		char M = JINITREQ;
		Au1 = h2AXE(u1);		// Swap Unix short to AXE format
		Au2 = h2AXE(u2);
		ABufLen = h2AXE(BufLen);
		AMaxBuf = h2AXE(MaxBuf);
		//copy in Buffer		
		ACE_OS::memcpy(&Buffer[0], &M, 1);
		ACE_OS::memcpy(&Buffer[1], &ProtocolVersion, 1);
		ACE_OS::memcpy(&Buffer[2], &Au1, 2);
		ACE_OS::memcpy(&Buffer[4], &Au2, 2);
		ACE_OS::memcpy(&Buffer[6], &ABufLen, 2);
		ACE_OS::memcpy(&Buffer[8], &AMaxBuf, 2);
		unsigned int nbytes = len;

		//Sending
		if (!mySession || ((mySession->send(Buffer, nbytes)) <= 0 ))
		{
			if (mySession)
			{ //send failed
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceError("In 'JTP_Session::jinitreq_snd()': ERROR ASSERTION (!mySession->send(Buffer, nbytes)), send failed. moreTries = %d. Session Info = '%s', mySession->last_error()=%d, mySession->last_error_text() = '%s'", moreTries, statusStr, mySession->last_error(),  mySession->last_error_text());
				}

				mySession->close();
				delete mySession;
				mySession = 0;
			}
			else
			{
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceError("In 'JTP_Session::jinitreq_snd()': ERROR ASSERTION (!mySession), mySession is NULL, very strange. moreTries = %d, Session Info = '%s'", moreTries, statusStr);
				}
			}

			--moreTries;
			continue; //negative case: next iteration loop
		}

		ACE_HANDLE handle = ACE_INVALID_HANDLE;
		//send ok
		if ( mySession != 0)
		{
			int noOfHandles = 1;
			int resHandles = mySession->get_handles(&handle, noOfHandles);
			if( resHandles >= 0)
			{
				if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceInformation("In 'JTP_Session::jinitreq_snd()': retrieved handle for session", statusStr);
				}
			}
			else
			{
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceError("In 'JTP_Session::jinitreq_snd()': ERROR ASSERTION (Handle is invalid, Session Info = '%s'", statusStr);
				}
			}
		}

		if (pollReply(handle, false))
		{
			if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceInformation("Out 'JTP_Session::jinitreq_snd()': SUCCESSFUL. Session Info = '%s'", statusStr);
			}
			return 0; //positive case check handle ok 
		}

		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError(
					"In 'JTP_Session::jinitreq_snd()': ERROR ASSERTION (!pollReply(handle, false)), waiting failed. moreTries = %d, Session Info = '%s'",
					moreTries, statusStr);
		}

		if (mySession)
		{
			mySession->close();
			delete mySession;
			mySession = 0;
		}

		--moreTries;
	} //end while


	if (ACS_JTP_TRACE_ERROR_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceError("Out 'JTP_Session::jinitreq_snd()': FAILURE. Session Info = '%s'", statusStr);
	}

	return res;
}


/*===================================================================
   ROUTINE: jexinitreq_snd
=================================================================== */
int JTP_Session::jexinitreq_snd()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceInformation("In 'JTP_Session::jexinitreq_snd()': Session Info = '%s'", statusStr);
	}

	int res = TranspErr;

	if (!pDSDClient)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("Out 'JTP_Session::jexinitreq_snd()': ERROR ASSERTION (!pDSDClient), catastrophic error, DSD client not allocated. Session Info = '%s'", statusStr);
		}
		return res;
	}

	int moreTries = NoOfTries; //number of tries

	while ((moreTries) || (NoOfTries == 0))
	{
		if (moreTries != NoOfTries)
		{
			delay(DelayTime);
		}
		ACS_DSD_Session * S1 = new (std::nothrow) ACS_DSD_Session();

		if (!S1)
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError("Out 'JTP_Session::jexinitreq_snd()': ERROR ASSERTION (!S1), catastrophic error, OUT OF MEMORY in DSD session allocation. Session Info = '%s'", statusStr);
			}
			return res;
		}

		// Connect to remote application.
		int resConnect = pDSDClient->connect(*S1, ServiceName, "JTP", NodeSv->system_id, NodeSv->node_state);
		if( resConnect < 0)
		{
			//connect failed
			--moreTries;

			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceError(
						"In 'JTP_Session::jexinitreq_snd()': ERROR ASSERTION [!pDSDClient->connect(...], connect failed. "
						"Last DSD error: code = %d, text = '%s'. moreTries = %d. Session Info = '%s'",
						pDSDClient->last_error(), pDSDClient->last_error_text(), moreTries, statusStr);
			}

			delete S1;
			S1 = 0;
			continue; //negative case: next iteration loop
		}

		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceInformation("In 'JTP_Session::jexinitreq_snd()': connection established with peer application. Session Info = '%s'", statusStr);
		}

		newSession(ServiceName, *S1); // Copy session data (init mySession pointer)
		ProtocolVersion = 1;
		char M = JEXINITREQ;
		Au1 = h2AXE(u1); // Swap Unix short to AXE format
		Au2 = h2AXE(u2);
		AMaxBuf = h2AXE(MaxBuf);
		int TypeOfDataTrans = 0;
		int TypeOfDataRepr = 0;
		//copy in buffer		
		ACE_OS::memcpy(&Buffer[0], &M, 1);
		ACE_OS::memcpy(&Buffer[1], &ProtocolVersion, 1);
		ACE_OS::memcpy(&Buffer[2], &Au1, 2);
		ACE_OS::memcpy(&Buffer[4], &Au2, 2);
		ACE_OS::memcpy(&Buffer[6], &AMaxBuf, 2);
		ACE_OS::memcpy(&Buffer[8], &TypeOfDataTrans, 1); // For compatibility with version 0
		ACE_OS::memcpy(&Buffer[9], &TypeOfDataRepr, 1);  // For compatibility with version 0

		unsigned int len = JSLAddBufferData(this);

		//sending
		if (!mySession || ((mySession->send(Buffer, len)) <= 0 ))
		{
			if (mySession)
			{ //send failed
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceError("In 'JTP_Session::jexinitreq_snd()': ERROR ASSERTION (!mySession->send(Buffer, len)), send failed. moreTries = %d. Session Info = '%s' , errno = %d, errortext = '%s'", moreTries, statusStr, mySession->last_error(), mySession->last_error_text());
				}

				mySession->close();
				delete mySession;
				mySession = 0;
			}
			else
			{
				//mySession == 0, very strange
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceError("In 'JTP_Session::jexinitreq_snd()': ERROR ASSERTION (!mySession), mySession is NULL, very strange. moreTries = %d, Session Info = '%s'", moreTries, statusStr);
				}
			}

			--moreTries;
			continue; //negative case: next iteration loop
		}
		ACE_HANDLE handle = ACE_INVALID_HANDLE;
		if ( mySession != 0)
		{
			int noOfHandles = 1;
			int resHandles = mySession->get_handles(&handle, noOfHandles);
			if( resHandles >= 0)
			{
				if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceInformation("In 'JTP_Session::jexinitreq_snd()': retrieved handle for session", statusStr);
				}
			}
			else
			{
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					char statusStr[256] = {0};
					getSessionInformation(statusStr, sizeof(statusStr));
					helperTraceError("In 'JTP_Session::jexinitreq_snd()': ERROR ASSERTION (Handle is invalid, Session Info = '%s'", statusStr);
				}
			}
		}

		if (pollReply(handle, false))
		{
			if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
			{
				char statusStr[256] = {0};
				getSessionInformation(statusStr, sizeof(statusStr));
				helperTraceInformation("In 'JTP_Session::jexinitreq_snd()': SUCCESSFUL. Session Info = '%s'", statusStr);
			}
			JSLClearSessions();
			return 0; //positive case check handle ok
		}

		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			char statusStr[256] = {0};
			getSessionInformation(statusStr, sizeof(statusStr));
			helperTraceError("In 'JTP_Session::jexinitreq_snd()': ERROR ASSERTION (!pollReply(handle, false)), waiting failed. moreTries = %d, Session Info = '%s'", moreTries, statusStr);
		}

		if(mySession)
		{
			mySession->close();
			delete mySession;
			mySession = 0;
		}

		--moreTries;
	} //end while


	if (ACS_JTP_TRACE_ERROR_ACTIVE)
	{
		char statusStr[256] = {0};
		getSessionInformation(statusStr, sizeof(statusStr));
		helperTraceError("Out 'JTP_Session::jexinitreq_snd()': FAILURE. Session Info = '%s'", statusStr);
	}

	return res;
}

/*===================================================================
   ROUTINE: JSLAddBufferData
=================================================================== */
unsigned int JTP_Session::JSLAddBufferData(JTP_Session* s)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLAddBufferData(JTP_Session* s)': s = %p", s);
	}

	JSLinit(); //initiate disconnect registrations list
	JSLAddId(s);//Add registration element
	unsigned int len = 0;

	{// synchronized section on Client List
		int NoOfDisconnected = 0;
		ACS_JTP_CriticalSectionGuard _(g_CLSync);
		for (int i = 0; i < CLSize; i++)
		{
			if (!CL[i].Free)
			{
				NoOfDisconnected++;
			}
		}
		len = (10+6+6*NoOfDisconnected); 
		char* Buffer = s->Buffer;
		//copy in Buffer	
		ACE_OS::memcpy(&Buffer[10], &s->APNo, 1);
		ACE_OS::memcpy(&Buffer[11], &s->APRestartCntr, 1);
		ACE_OS::memcpy(&Buffer[12], &s->PID,2);
		ACE_OS::memcpy(&Buffer[14], &s->ConnectionCntr,2);
		NoOfDisconnected = 0;
		for (int i = 0; i < CLSize; i++)
		{
			if (!CL[i].Free)
			{
				Buffer = s->Buffer + 16 + NoOfDisconnected * 6; 
				ACE_OS::memcpy(&Buffer[0], &CL[i].APNo, 1);
				ACE_OS::memcpy(&Buffer[1], &CL[i].APRestartCntr, 1);
				ACE_OS::memcpy(&Buffer[2], &CL[i].PID,2);
				ACE_OS::memcpy(&Buffer[4], &CL[i].ConnectionCntr,2);
				NoOfDisconnected++;
			}
		}
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLAddBufferData(JTP_Session* s)': s = %p", s);
	}

	return len;

}

/*===================================================================
   ROUTINE: JSLClearSessions
=================================================================== */
void JTP_Session::JSLClearSessions()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLClearSessions()'");
	}

	JSLinit();

	{// synchronized section on Client List
		ACS_JTP_CriticalSectionGuard _(g_CLSync);

		for (int i = 0; i < CLSize ; i ++)
		{
			CL[i].Free = 1;
		}
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLClearSessions()'");
	}


}

/*===================================================================
   ROUTINE: JSLAddDisconnected
=================================================================== */
void JTP_Session::JSLAddDisconnected(JTP_Session* s)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLAddDisconnected(JTP_Session* s)': s = %p", s);
	}

	JSLinit();
	if (s->APNo == 0)
	{
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			helperTraceWarning("Out 'JTP_Session::JSLAddDisconnected(JTP_Session* s)': WARNING ASSERTION (s->APNo == 0).");
		}
		return;
	}

	{ // synchronized section on Client List
		ACS_JTP_CriticalSectionGuard _(g_CLSync);
		for (int i = 0; i < CLSize ; i++)
		{
			if (CL[i].Free)
			{
				CL[i].APNo = s->APNo;
				CL[i].APRestartCntr = s->APRestartCntr;
				CL[i].PID = s->PID;
				CL[i].ConnectionCntr = s->ConnectionCntr;
				CL[i].Free = 0;
				break;
			}
			if (i == CLSize - 1)
			{
				JSLCLgrow();
			}
		}
	}

	s->APNo           = 0;
	s->APRestartCntr  = 0;
	s->PID            = 0;
	s->ConnectionCntr = 0;


	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLAddDisconnected(JTP_Session* s)': s = %p", s);
	}


}

/*===================================================================
   ROUTINE: JSLCLgrow
=================================================================== */
void JTP_Session::JSLCLgrow()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLCLgrow()'");
	}

	int tCLSize = CLSize + 10;
	JSLCL* tCL = new (std::nothrow) JSLCL[tCLSize];

	for (int i = 0; i < tCLSize; i++)
	{
		tCL[i].Free = 1;
	}

	{
		ACS_JTP_CriticalSectionGuard _(g_CLSync_new);
		int i= 0;
		while (i < CLSize)
		{
			tCL[i] = CL[i];
			i++;
		}

		if (CL)
		{
			delete[] CL;
			CL = 0;
		}
		CL = tCL;
		CLSize = tCLSize;
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLCLgrow()'");
	}


}

/*===================================================================
   ROUTINE: JSLAddId
=================================================================== */
void JTP_Session::JSLAddId(JTP_Session* s)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLAddId(JTP_Session* s)': s = %p", s);
	}

	s->APNo           = s->getOwnNodeId();
	s->APRestartCntr  = JSLAPRestartCntr;
	s->PID            = JSLPID;
	s->ConnectionCntr = ++JSLConnectionCntr;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("s->APNo = %d, s->APRestartCntr=%d, s->PID =%d, s->ConnectionCntr=%d", s->APNo, s->APRestartCntr, s->PID, s->ConnectionCntr );
		helperTraceInformation("Leaving 'JTP_Session::JSLAddId(JTP_Session* s)': s = %p", s);
	}


}

/*===================================================================
   ROUTINE: JSLCheckSessions
=================================================================== */
void JTP_Session::JSLCheckSessions(JTP_Session* s, unsigned long MsgLen)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLCheckSessions(JTP_Session* s, unsigned long MsgLen)': s = %p, MsgLen = %lu", s, MsgLen);
	}

	JSLinit();
	int i = 0;
	// Can there be multiple registrations on the same s ?
	{
		ACS_JTP_CriticalSectionGuard _(g_SLSync);
		for (int i = 0; i < SLSize; i++)
		{
			if (SL[i] == 0)
			{
				SL[i] = s;
				break;
			}
			if (i == SLSize - 1)
			{
				JSLSLgrow();
			}
		}
	}

	int NoOfDisconnected = (MsgLen - 16)/6;
	if (MsgLen < 16)
	{
		NoOfDisconnected = 0;
	}

	if (ACS_JTP_TRACE_WARNING_ACTIVE)
	{
		helperTraceWarning("In 'JTP_Session::JSLCheckSessions(...': Remote end requested to close %d JTP sessions", NoOfDisconnected);
	}

	i = 0;
	while (i < NoOfDisconnected)
	{
		char* Buffer = s->Buffer + 16 + (i * 6); //new version

		unsigned char APNo = Buffer[0];

		unsigned char APRestartCntr = Buffer[1];

		unsigned short PID;
		ACE_OS::memcpy(&PID, &Buffer[2], 2);

		unsigned short ConnectionCntr;
		ACE_OS::memcpy(&ConnectionCntr, &Buffer[4], 2);

		JSLCloseSession(APNo, APRestartCntr, PID, ConnectionCntr, s->system_id);
		i++;
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLCheckSessions(JTP_Session* s, unsigned long MsgLen)': s = %p, MsgLen = %lu", s, MsgLen);
	}

}

/*===================================================================
   ROUTINE: JSLCloseSession
=================================================================== */
void JTP_Session::JSLCloseSession(unsigned char APNo,  unsigned char APRestartCntr, unsigned short PID, unsigned short ConnectionCntr, unsigned int parentSysId)
{
	ACS_JTP_CriticalSectionGuard _(g_SLSync);

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation(
				"Entering 'JTP_Session::JSLCloseSession(...': "
				"APNo = %u, APRestartCntr = %u, PID = %hu, ConnectionCntr = %hu, parentSysId = %u",
				static_cast<unsigned>(APNo), static_cast<unsigned>(APRestartCntr), PID, ConnectionCntr, parentSysId);
	}

	for (int i = 0; i < SLSize; i++)
	{
		if (SL[i])
		{
			if (((SL[i]->APNo == APNo) && (SL[i]->APRestartCntr != APRestartCntr) && (SL[i]->system_id == parentSysId)) ||
					((SL[i]->APNo == APNo) && (SL[i]->APRestartCntr == APRestartCntr) && (SL[i]->PID == PID) && (SL[i]->ConnectionCntr == ConnectionCntr) && (SL[i]->system_id == parentSysId)))
			{
				JTP_Session* ts = SL[i];

				if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
				{
					helperTraceInformation(
							"In 'JTP_Session::JSLCloseSession(...': "
							"Closing JTP session: [dsdSession = %p] [APNo = %u] [APRestartCntr = %u] [PID = %hu] [ConnectionCntr = %hu]",
							ts->mySession, static_cast<unsigned>(ts->APNo), static_cast<unsigned>(ts->APRestartCntr),
							ts->PID, ts->ConnectionCntr);
				}

				SL[i] = 0; // Remove session

				if (ACS_DSD_Session* pSess = ts->mySession)
				{
					ts->mySession = 0;
					pSess->close(); //shutdown session
					delete pSess;
					pSess = 0;
				}
				ts->JState = StateCreated; //set state
			}
		}
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation(
				"Leaving 'JTP_Session::JSLCloseSession(...': "
				"APNo = %u, APRestartCntr = %u, PID = %hu, ConnectionCntr = %hu, parentSysId = %u",
				static_cast<unsigned>(APNo), static_cast<unsigned>(APRestartCntr), PID, ConnectionCntr, parentSysId);
	}

}

/*===================================================================
   ROUTINE: JSLRemoveSession
=================================================================== */
void JTP_Session::JSLRemoveSession(JTP_Session* s)
{

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLRemoveSession(JTP_Session * s)': s = %p", s);
	}

	JSLinit();

	{
		ACS_JTP_CriticalSectionGuard _(g_SLSync);
		for (int i = 0; i < SLSize; i++)
		{
			if (SL[i] == s)
			{
				SL[i] = 0;
				break;
			}
		}
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLRemoveSession(JTP_Session* s)': s = %p", s);
	}


}

/*===================================================================
   ROUTINE: JSLSLgrow
=================================================================== */
void JTP_Session::JSLSLgrow()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLSLgrow()'");
	}

	int tSLSize = SLSize + 10;
	JTP_Session **tSL = new (std::nothrow) JTP_Session * [tSLSize];

	for (int i = 0; i < tSLSize; i++)
	{
		tSL[i] = 0;
	}

	{
		ACS_JTP_CriticalSectionGuard _(g_SLSync_new);
		int i = 0;
		while (i < SLSize)
		{
			tSL[i] = SL[i];
			i++;
		}

		if (SL)
		{
			delete[] SL;
			SL = 0;
		}
		SL = tSL;
		SLSize = tSLSize;
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLSLgrow()'");
	}
}

/*===================================================================
   ROUTINE: JSLinit
=================================================================== */
void JTP_Session::JSLinit()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering 'JTP_Session::JSLinit()': CLSize = %d, SLSize = %d", CLSize, SLSize);
	}

	ACS_JTP_CriticalSectionGuard _(g_JSL_init_Sync);

	if (JSL_init)
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("Leaving 'JTP_Session::JSLinit()': already initialized. CLSize = %d, SLSize = %d", CLSize, SLSize);
		}
		return;
	}

	JSLCLgrow();
	JSLSLgrow();
	JSLAPRestartCntr = ACS_PRC_info::getSystemLifeId();
	JSLPID = static_cast<unsigned short>(ACE_OS::getpid());
	JSLConnectionCntr = 0;
	JSL_init = 1;

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving 'JTP_Session::JSLinit()': CLSize = %d, SLSize = %d", CLSize, SLSize);
	}
}

/*===================================================================
   ROUTINE: getOwnNodeId
=================================================================== */
unsigned char JTP_Session::getOwnNodeId()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Entering JTP_Session::getOwnNodeId()");
	}

	OmHandler myOmHandler;

	if( myOmHandler.Init() == ACS_CC_FAILURE )
	{ 
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("Error in OmHandler initialize");
			helperTraceInformation("Leaving JTP_Session::getOwnNodeId()");
		}
		return (unsigned char)0;
	}

	//Get the instance of the APzFunctions class.
	
	std::vector<std::string> apzFunctions_dn_list;
	
	myOmHandler.getClassInstances(APZ_FUNCTIONS_CLASS_NM, apzFunctions_dn_list);

	if( apzFunctions_dn_list.size() != 1 )
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("More than one instance exists for ApzFunction's class.");
			helperTraceInformation("Leaving JTP_Session::getOwnNodeId()");
		}
		myOmHandler.Finalize();
		return (unsigned char)0;
	}

	//Get the value of the apNodeNumber attribute from the instance of ApzFunction class.

	ACS_CC_ImmParameter apNodeNumber;
	apNodeNumber.attrName = const_cast<char*>(AP_NODE_NUMBER_ATTR_NAME);

	if( myOmHandler.getAttribute( apzFunctions_dn_list[0].c_str(), &apNodeNumber ) == ACS_CC_FAILURE )
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("Error while getting the value of apNodeNumber from ApzFunctions class.");
			helperTraceInformation("Leaving JTP_Session::getOwnNodeId()");
		}
		myOmHandler.Finalize();
		return (unsigned char)0;
	}

	unsigned char tempApNodeNumber = *(reinterpret_cast<int *>(apNodeNumber.attrValues[0]));		
	unsigned char tempApNodeNumber1 = 2*(tempApNodeNumber - 1);

	myOmHandler.Finalize();

	//Now get the info about node letter A or B.
	
	ifstream ifs;
	ifs.open("/etc/cluster/nodes/this/id");

	string id_name("");

	if (!ifs.good())
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("Error while getting the value of node id.");
			helperTraceInformation("Leaving JTP_Session::getOwnNodeId()");
		}
		return (unsigned char)0;
	}

	getline(ifs, id_name);

	ifs.close();

	if(strcmp(id_name.c_str(), "1") == 0)
	{
		 ++tempApNodeNumber1; //Increment the node number for A node.
		 
	}

	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Leaving JTP_Session::getOwnNodeId()");
	}
	
	return tempApNodeNumber1;
}
