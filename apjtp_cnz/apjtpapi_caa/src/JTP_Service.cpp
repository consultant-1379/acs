 /*=================================================================== */
   /**
   @file JTP_Service.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the JTP_Service.h module.

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
#include "JTP_Service.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: JTP_Service
=================================================================== */
JTP_Service::JTP_Service(char* Service) : pServer(0), readBufferLen(0), bufferCursor(0)
{
	ACE_OS::snprintf(ServiceName, SERVICE_NAME_MAX_LENGTH, "%s", Service ? Service : "NO_SERVICE_NAME");
	ServiceName[SERVICE_NAME_MAX_LENGTH] = 0;

	ACE_OS::memset(Buffer, 0, sizeof(Buffer));
	if (ACS_DSD_Server* p = new (std::nothrow) ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET))
	{
		int result = p->open();
		//open server
		if (result >= 0)
		{
			pServer = p;
		}
		else
		{
                        delete p;
			if (ACS_JTP_TRACE_ERROR_ACTIVE)
			{
				helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!p->open()], DSD server open failed.");
			}
		}
	}
	else if (ACS_JTP_TRACE_ERROR_ACTIVE)
	{
		helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!(new ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET))], memory allocation failed.");
	}
}

/*===================================================================
   ROUTINE: ~JTP_Service
=================================================================== */
JTP_Service::~JTP_Service()
{
	if (pServer)
	{
		//pServer->close(); Fix for HBEAT Crash
		delete pServer;
		pServer = 0;
	}
}

/*===================================================================
   ROUTINE: jidrepreq
=================================================================== */
bool JTP_Service::jidrepreq()
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		helperTraceInformation("In 'JTP_Service::jidrepreq()': 'pServer->publish(...'");
	}

	if (!pServer)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE) helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!pServer], pServer is a NULL pointer");
		return false;
	}

	//calling to publish method (DSD_Server)
	int result = pServer->publish(ServiceName, "JTP", acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE);

	if (result >= 0)
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("Out 'JTP_Service::jidrepreq()': 'pServer->publish(...' successful");
		}
		return true;
	}
	else
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceError("Out 'JTP_Service::jidrepreq()': 'pServer->publish(...' failed! Error Description = %s", pServer->last_error_text());
		}
		return false;
	}

}

/*===================================================================
   ROUTINE: getHandles
=================================================================== */
bool JTP_Service::getHandles(int& noOfHandles, ACE_HANDLE*& handle)
{
	if (!pServer)
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "ERROR ASSERTION [!pServer], pServer is a NULL pointer");
		}
		return false;
	}

	//calling to getHandles method (DSD_Server)
	int result = pServer->get_handles(handle, noOfHandles );

	if(result >= 0)
	{
		if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
		{
			helperTraceInformation("Out 'JTP_Service::getHandles()': 'pServer->getHandles(...' successful");
		}
		return true;
	}
	else
	{
		if (ACS_JTP_TRACE_ERROR_ACTIVE)
		{
			helperTraceErrorSource(__FILE__, __LINE__, "Out 'JTP_Service::getHandles()': ERROR ASSERTION [!get_handles], error while getting handles.");
		}

		return false;
	}
}

/*===================================================================
   ROUTINE: jtpRecvMsg
=================================================================== */
bool JTP_Service::jtpRecvMsg(char* buf, unsigned short nbytes, ACS_DSD_Session& S1)
{
	if (ACS_JTP_TRACE_INFORMATION_ACTIVE)
	{
		ACS_DSD_Node remoteNode;
		S1.get_remote_node(remoteNode);
		helperTraceInformation("Starting 'JTP_Service::jtpRecvMsg(...'. ServiceName = '%s', remoteNode sysId = %u, remoteNode name = %s", ServiceName, remoteNode.system_id, remoteNode.node_name);
	}

	//Fix for the warning while compilation.
	(void)buf;

	//parameter buf is not used!?
	if (readBufferLen == 0)
	{
		int msgSize = S1.recv(Buffer, sizeof(Buffer));

		if (msgSize > 0)
		{
			readBufferLen = msgSize + 4;
			bufferCursor = 0;
		}
		else
		{
			helperTraceErrorSource(__FILE__, __LINE__, "'S1.recv(Buffer, sizeof(Buffer))' failed!");
			return false;
		}
	}

	bufferCursor += nbytes;
	if (bufferCursor > readBufferLen)
	{
		return false;
	}
	if (bufferCursor == readBufferLen)
	{
		readBufferLen = 0;
	}
	return true;
}
