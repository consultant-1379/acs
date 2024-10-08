
/*=================================================================== */
   /**
   @file JTP_Service_R3A.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the JTP_Service_R3A.h module.

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
#include "jtp.h"
#include "JTP_Service_R3A.h"
#include "JTP_Session_R3A.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: accept_new
=================================================================== */
bool JTP_Service_R3A::accept_new(ACS_JTP_Job_R3A* j, ACS_JTP_Conversation_R3A* c)
{
	if (!pServer)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!pServer], internal DSD server is a NULL pointer!");
		}
		return false;
	}

	ACS_DSD_Session* S1 = 0; //local object ACD_DSD_Session
	if (!(S1 = new (std::nothrow) ACS_DSD_Session()))
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!(new ACS_DSD_Session())], memory allocation failed");
		}
		return false;
	}
	int result = pServer->accept(*S1);

	if(result < 0)
	{
		//accept failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!pServer->accept(*S1)], accept failed");
		}

		S1->close();
		delete S1;
		S1 = 0;
		return false;
	}
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("In 'JTP_Service_R3A::accept_new(ACS_JTP_Job_R3A* j,...': connected to remote application");
	}

	int nBytes = S1->recv(Buffer, sizeof(Buffer));

	if(nBytes  < 0)
	{
		//recv failed
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [S1->recv(Buffer, sizeof(Buffer))], No data received");
		}
		S1->close(); //shutdown local session
		delete S1;
		S1 = 0;
		return false;
	}

	if(nBytes == 0)
	{
		//session closed
		if (ACS_JTP_TRACE_WARNING_ACTIVE)
		{
			helperTraceWarningSource(__FILE__, __LINE__, "WARNING ASSERTION [nBytes == 0], Peer closed connection");
		}
		S1->close();
		delete S1;
		S1 = 0;
		return false;
	}

	unsigned int MsgLen = nBytes;
	bool res = true;
	char M = Buffer[0];
	int Pv = Buffer[1];

	switch (Buffer[0])
	{
		case JTP_Session::JEXINITREQ:
		{
			//conversation
			if (c)
			{
				if (!c->Internal)
				{
					//Internal implementation object NULL pointer.
					if (ACS_JTP_TRACE_ERROR_ACTIVE)
					{
						helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!c->Internal], Implementation internal object is NULL.");
					}
					S1->close(); //shutdown local session
					delete S1;
					S1 = 0;
					return false;
				}

				ACS_DSD_Node remoteNode;
				S1->get_remote_node(remoteNode);
				c->Internal->system_id = remoteNode.system_id;

				//newSessionServer calls newSession where pointer mySession is init
				if (!(res = c->Internal->newSessionServer(MsgLen, Buffer[1], ServiceName, *S1, Buffer)))
				{
					if (ACS_JTP_TRACE_ERROR_ACTIVE)
					{
						helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!(res = c->Internal->newSessionServer(MsgLen, Buffer[1], ServiceName, *S1, Buffer))], newSessionServer was false.");
					}
					S1->close(); //shutdown local session
					delete S1;
					S1 = 0;
					return false;
				}
			}
			else
			{
				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Service_R3A::accept_new(ACS_JTP_Job_R3A* j,...': NULL Conversation object parameter c received");
				}
				S1->close(); //shutdown local session
				delete S1;
				S1 = 0;
				return false;
			}
			break;
		}
		case JTP_Session::JINITREQ:
		{//job
			if (j)
			{
				if (!j->Internal)
				{
					//Internal implementation object NULL pointer.
					if (ACS_JTP_TRACE_ERROR_ACTIVE)
					{
						helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!j->Internal], Implementation internal object is NULL.");
					}
					S1->close(); //shutdown local session
					delete S1;
					S1 = 0;
					return false;
				}

				if (!(res = j->Internal->newJobServer(MsgLen, Buffer[1], ServiceName, *S1, Buffer)))
				{
					if (ACS_JTP_TRACE_ERROR_ACTIVE)
					{
						helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!(res = j->Internal->newJobServer(MsgLen, Buffer[1], ServiceName, *S1, Buffer)], newJobServer was false.");
					}

					S1->close(); //shutdown local session
					delete S1;
					S1 = 0;
					return false;
				}
			}
			else
			{

				if (ACS_JTP_TRACE_ERROR_ACTIVE)
				{
					helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Service_R3A::accept_new(ACS_JTP_Job_R3A* j,...': NULL Job object parameter j received");
				}
				S1->close(); //shutdown local session
				delete S1;
				S1 = 0;
				return false;
			}
			break;
		}
		default:
		{
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				helperTraceError("Out 'JTP_Service_R3A::accept_new(ACS_JTP_Job_R3A * j,...': Unknown code M from remote peer. M = 0x%X, Pv = %d", static_cast<unsigned>(M), Pv);
			}
			S1->close(); //shutdown local session
			delete S1;
			S1 = 0;
			return false;
		}
	}
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("Out 'JTP_Service_R3A::accept_new(ACS_JTP_Job_R3A* j,...': Peer connection %s accepted. M = 0x%X, Pv = %d", (res ? "" : "not "), static_cast<unsigned>(M), Pv);
	}

	return res;
}
