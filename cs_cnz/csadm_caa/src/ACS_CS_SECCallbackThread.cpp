#include "ACS_CS_SECCallbackThread.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Event.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_SECCallbackThread_TRACE);

using namespace ACS_CS_NS;


ACS_CS_SECCallbackThread::ACS_CS_SECCallbackThread():
		ldapSecApi(0)
{
	ldapProxyHandler = new ACS_CS_LdapProxyHandler();
	activate();
}

ACS_CS_SECCallbackThread::~ACS_CS_SECCallbackThread()
{
    ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO, "SEC callback thread destructor entering"));
    if (ldapProxyHandler)
    {
        ldapProxyHandler->stop();
        delete ldapProxyHandler;
        ldapProxyHandler = 0;
    }
    ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO, "SEC callback thread destructor exiting"));
}


int ACS_CS_SECCallbackThread::exec ()
{
	// Open event used to signal service shutdown
	ACS_CS_EventHandle shutdownAllEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);
	ACS_CS_EventHandle immDispatch = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SEC);		//TR HW99171

	ldapSecApi = ACS_CS_SecApi::getInstance();

	if (shutdownAllEvent == ACS_CS_EVENT_RC_ERROR) {
		ACS_CS_TRACE((ACS_CS_SECCallbackThread_TRACE,
				"(%t) ACS_CS_SECCallbackThread::exec()\n"
				"Failed to open shutdown event"));

		return -1;
	}

	if (immDispatch == ACS_CS_EVENT_RC_ERROR) {
		ACS_CS_TRACE((ACS_CS_SECCallbackThread_TRACE,
				"(%t) ACS_CS_SECCallbackThread::exec()\n"
				"Failed to open imm Dispatch Failed event"));
		return -1;
	}

	ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"Sec Handler thread started"));
	int sizeHandle = 2;
	int handles[3] = {0};
	//int selection_object;

	handles[0] = shutdownAllEvent;
	handles[1] = immDispatch;					//TR HW99171

	int ldapProxyStatus = -1;
	int implementerRegister = 0;

	while (!exit_)
	{
		if (0 != ldapProxyStatus)
		{
			ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"ldapProxyStatus != 0 -> start ldapPorxyHandler!"));
			ldapProxyStatus = ldapProxyHandler->start();
		}


		if (implementerRegister != 0)
		{
			ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"Implementer has to be added again !!!"));
			implementerRegister = ldapProxyHandler->implementerRegister();		//TR HW99171
		}


		if((!ldapSecApi->isSubscribed()) && (!ldapSecApi->trustCategoryisSubscribed()))
		{
			ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"UnSubscribed to SEC so handle is 2 !!!"));
			sizeHandle = 2;
		}
		else
		{
			if (2 == sizeHandle)
			{
				ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"Subscribed to SEC !!!"));
				handles[2] = ldapSecApi->getFd();
				sizeHandle = 3;				//TR HW99171
			}

		}
		
		int eventIndex = ACS_CS_Event::WaitForEvents(sizeHandle, handles, 5000);

		if (eventIndex == ACS_CS_EVENT_RC_ERROR)
		{
			ACS_CS_TRACE((ACS_CS_SECCallbackThread_TRACE,
					"(%t) ACS_CS_SECCallbackThread_TRACE::exec()\n"
					"WaitForEvents() failed"));
			return -1;
		}
		else if (eventIndex == ACS_CS_EVENT_RC_TIMEOUT)   // Timeout event
		{
			if (0 == ldapProxyStatus)
			{
				//ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO, "handle from SECThread = %d",handles[1]));

				if (!ldapSecApi->isLdapConfigurationChanged())
				{
					ldapSecApi->updateLdapConfigurationClass();
				}
			}
		}
		else if (eventIndex == 0)   // Shutdown event
		{
			break;
		}
		else if (eventIndex == 1)   // IMM dispatch failed event - TR HW99171
		{
			ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"Dispatch Failed Event received"));
			int result = ldapProxyHandler->implementerRegister();
			if (result == 0)
			{
				implementerRegister = 0;
				ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"Implementer Register is successful"));
			}
			else
			{
				implementerRegister = -1;
				ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO,"Implementer Registration has been failed"));
			}
		}

		else if (eventIndex == 2)   // IMM callback
		{
			ACS_CS_TRACE((ACS_CS_SECCallbackThread_TRACE,
					"(%t) ACS_CS_SECCallbackThread_TRACE:exec()\n"
					"Dispatched SEC callback"));

			// Dispatch to IMM callback
			if(ldapSecApi->dispatch())
			{
				ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_INFO, " dispatch done"));

				int errorCode = ldapSecApi->writeToSlapd();
				if(errorCode != 0)
				{
					ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_ERROR,"writing data to slapd.conf failed"));
					//return errorCode;
				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_SECCallbackThread_TRACE,LOG_LEVEL_ERROR,"writing data to slapd.conf success"));
				}
			}
		}
	}
	//ldapProxyHandler.stop();   - TR HW67630

	return 0;
}

