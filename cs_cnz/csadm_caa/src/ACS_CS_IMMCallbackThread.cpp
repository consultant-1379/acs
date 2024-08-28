//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#include "ACS_CS_IMMCallbackThread.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Event.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_IMMCallbackThread_TRACE);

using namespace ACS_CS_NS;


ACS_CS_IMMCallbackThread::ACS_CS_IMMCallbackThread(int fd, acs_apgcc_objectimplementerinterface_V3 *handle)
{
    this->fd_ = fd;
    this->handle_ = handle;
}


int ACS_CS_IMMCallbackThread::exec ()
{
    // Open event used to signal service shutdown
    ACS_CS_EventHandle shutdownAllEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);

    if (shutdownAllEvent == ACS_CS_EVENT_RC_ERROR) {
        ACS_CS_TRACE((ACS_CS_IMMCallbackThread_TRACE,
                "(%t) ACS_CS_IMMCallbackThread::exec()\n"
                "Failed to open shutdown event"));

        return -1;
    }


    const int handleLen = 2;
    int *handles = new int[handleLen];
    ACS_CC_ReturnType result = ACS_CC_FAILURE;

    handles[0] = shutdownAllEvent;
    handles[1] = this->fd_;

    while (!exit_)
    {
        int eventIndex = ACS_CS_Event::WaitForEvents(handleLen, handles, 0);

        if (eventIndex == ACS_CS_EVENT_RC_ERROR)
        {
            ACS_CS_TRACE((ACS_CS_IMMCallbackThread_TRACE,
                    "(%t) ACS_CS_IMMCallbackThread::exec()\n"
                    "WaitForEvents() failed"));

            delete[] handles;
            return -1;
        }
        else if (eventIndex == 0)   // Shutdown event
        {
            break;
        }
        else if (eventIndex == 1)   // IMM callback
        {
        	ACS_CS_TRACE((ACS_CS_IMMCallbackThread_TRACE,
        			"(%t) ACS_CS_IMMCallbackThread::exec()\n"
        			"Dispatched IMM callback"));

        	// Dispatch to IMM callback
        	result = handle_->dispatch(ACS_APGCC_DISPATCH_ONE);
        	if (result != ACS_CC_SUCCESS)
        	{
        		ACS_CS_FTRACE((ACS_CS_IMMCallbackThread_TRACE,
        				LOG_LEVEL_ERROR,
						"(%t) ACS_CS_IMMCallbackThread::exec()\n"
						"Dispatched IMM Failed"));
        		ACS_CS_EventHandle handleService = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SERVICE);
        		ACS_CS_EventHandle handleSec = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SEC);

        		if (handleService >= 0)
        		{
        			if (!ACS_CS_Event::SetEvent(handleService))
        			{
        				ACS_CS_FTRACE((ACS_CS_IMMCallbackThread_TRACE,
        						LOG_LEVEL_ERROR,
								"ACS_CS_ServiceHandler::exec() "
								"Failed to set event"));
        			}
        		}
        		if (handleSec >= 0)
        		{
        			if (!ACS_CS_Event::SetEvent(handleSec))
        			{
        				ACS_CS_FTRACE((ACS_CS_IMMCallbackThread_TRACE,
        						LOG_LEVEL_ERROR,
								"ACS_CS_ServiceHandler::exec() "
								"Failed to set Sec thread notify event"));
        			}
        		}

        		ACS_CS_FTRACE((ACS_CS_IMMCallbackThread_TRACE,
        				LOG_LEVEL_INFO,
						"(%t) ACS_CS_IMMCallbackThread::exec()\n"
						"Exit thread"));

        		exit_ = true;
        	}

        }
    }

    delete[] handles;
    return 0;
}
