//******************************************************************************//
//  NAME
//     ACS_SCH_IMMCallbackThread.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
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
//
//
//  AUTHOR
//     2017-04-14  XBHABHE
//
//  SEE ALSO
//     -
//
//******************************************************************************

#include "ACS_SCH_IMMCallbackThread.h"
#include "ACS_SCH_Util.h"
#include "ACS_SCH_Event.h"
#include "ACS_SCH_Trace.h"

ACS_SCH_Trace_TDEF(ACS_SCH_IMMCallbackThread_TRACE);

/*============================================================================
	ROUTINE: ACS_SCH_IMMCallbackThread
 ============================================================================ */
ACS_SCH_IMMCallbackThread::ACS_SCH_IMMCallbackThread(int fd, acs_apgcc_objectimplementerinterface_V3 *handle)
{
	this->fd_ = fd;
	this->handle_ = handle;
	shutdownService=false;
	ACS_SCH_TRACE((ACS_SCH_IMMCallbackThread_TRACE,"(%t) ACS_SCH_IMMCallbackThread::inside constructor\n"));
}


/*============================================================================
	ROUTINE: svc
 ============================================================================ */
int ACS_SCH_IMMCallbackThread::svc ()
{
	ACS_SCH_TRACE((ACS_SCH_IMMCallbackThread_TRACE,"(%t) ACS_SCH_IMMCallbackThread::inside immcallback thread\n"));
	shutdownEvent = ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SHUTDOWN);
	if (shutdownEvent == ACS_SCH_Event_RC_ERROR) 
	{
		ACS_SCH_FTRACE((ACS_SCH_IMMCallbackThread_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_IMMCallbackThread] ACS_SCH_Event_RC_ERROR. Unkown Error.. !!!"));
		return -1;
	}
	const int handleLen = 2;
	int *handles = new int[handleLen];

	handles[0] = shutdownEvent;
	handles[1] = this->fd_;

	while (!shutdownService)
	{
		int eventIndex = ACS_SCH_Event::WaitForEvents(handleLen, handles, 0);

		if (eventIndex == ACS_SCH_Event_RC_ERROR)
		{
			ACS_SCH_FTRACE((ACS_SCH_IMMCallbackThread_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_IMMCallbackThread] Received Event error..!!!"));
			delete[] handles;
			return -1;
		}
		else if (eventIndex == 0)   // Shutdown event
		{
			ACS_SCH_FTRACE((ACS_SCH_IMMCallbackThread_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_IMMCallbackThread] Received shutdown event! Shutting Down..."));
			shutdownService = true;		
			break;
		}
		else if (eventIndex == 1)   // IMM callback
		{
			//ACS_SCH_Logger::log("[ACS_SCH_IMMCallbackThread] Received IMM dispatch request...",SCH_makeOrigin(),LOG_LEVEL_INFO);
			// Dispatch to IMM callback
			handle_->dispatch(ACS_APGCC_DISPATCH_ONE);
		}
	}

	delete[] handles;
	return 0;
}

/*============================================================================
  ROUTINE: ACS_SCH_ClusterCPIMMCallbackThread
 ============================================================================ */
/*ACS_SCH_ClusterCPIMMCallbackThread::ACS_SCH_ClusterCPIMMCallbackThread(int fd, acs_apgcc_objectimplementerinterface_V3 *handle)
{
  this->fd_ = fd;
  this->handle_ = handle;
  shutdownService=false;
  ACS_SCH_TRACE((ACS_SCH_IMMCallbackThread_TRACE,"(%t) ACS_SCH_IMMCallbackThread::inside constructor\n"));
}*/

/*============================================================================
  ROUTINE: svc
 ============================================================================ */
/*int ACS_SCH_ClusterCPIMMCallbackThread::svc ()
{
  ACS_SCH_Logger::log("In IMMcallback thread",SCH_makeOrigin(),LOG_LEVEL_ERROR);

	shutdownEvent = ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SHUTDOWN);
  if (shutdownEvent == ACS_SCH_Event_RC_ERROR)
  {
    ACS_SCH_Logger::log("[ACS_SCH_IMMCallbackThread] ACS_SCH_Event_RC_ERROR. Unkown Error.. !!!",SCH_makeOrigin(),LOG_LEVEL_ERROR);
    return -1;
  }

	scalingShutdownEvent = ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SCALING);
  if (scalingShutdownEvent == ACS_SCH_Event_RC_ERROR)
  {
    ACS_SCH_Logger::log("[ACS_SCH_IMMCallbackThread] ACS_SCH_Event_RC_ERROR. Unkown Error.. !!!",SCH_makeOrigin(),LOG_LEVEL_ERROR);
    return -1;
  }

	const int handleLen = 3;
  int *handles = new int[handleLen];
	
  handles[0] = this->fd_;
  handles[1] = shutdownEvent;
  handles[2] = scalingShutdownEvent;

  while (!shutdownService)
  {
    int eventIndex = ACS_SCH_Event::WaitForEvents(handleLen,handles,0);

    if (eventIndex == ACS_SCH_Event_RC_ERROR)
    {
      ACS_SCH_Logger::log("[ACS_SCH_ClusterCPIMMCallbackThread] Received Event error..!!!",SCH_makeOrigin(),LOG_LEVEL_ERROR);
      delete[] handles;
      return -1;
    }
    else if (eventIndex == 0)   // IMM callback
    {
      //ACS_SCH_Logger::log("[ACS_SCH_ClusterCPIMMCallbackThread] Received IMM dispatch request...",SCH_makeOrigin(),LOG_LEVEL_INFO);
      // Dispatch to IMM callback
      handle_->dispatch(ACS_APGCC_DISPATCH_ONE);
    }
		else if (eventIndex == 1)   // Shutdown event
    {
      ACS_SCH_Logger::log("[ACS_SCH_ClusterCPIMMCallbackThread] Received shutdown event! Shutting Down...",SCH_makeOrigin(),LOG_LEVEL_INFO);
      shutdownService = true;
      break;
    }
		else if (eventIndex == 2)   // scalingShutdown event
    {
      ACS_SCH_Logger::log("[ACS_SCH_ClusterCPIMMCallbackThread] Received scaling shutdown event! Shutting Down...",SCH_makeOrigin(),LOG_LEVEL_INFO);
      shutdownService = true;
      break;
    }
  }

  ACS_SCH_Logger::log("Deleting the event handles for clusterCPIMMCallback thread",SCH_makeOrigin(),LOG_LEVEL_ERROR);
  delete[] handles;
  return 0;
}*/
