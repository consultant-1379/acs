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

#ifndef ACS_SCH_IMMCallbackThread_h_
#define ACS_SCH_IMMCallbackThread_h_ 1

#include <ace/Task.h>
#include "saImmOi.h"
#include "saImmOm.h"
#include <acs_apgcc_objectimplementerinterface_V2.h>
#include <acs_apgcc_objectimplementerinterface_V3.h>
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Event.h"
#include "ACS_SCH_Util.h"

class ACS_SCH_IMMCallbackThread : public ACE_Task_Base
{
    public:
			ACS_SCH_IMMCallbackThread(int fd, acs_apgcc_objectimplementerinterface_V3 *handle);
			int svc ();
			void deActivate();
			bool exit_;
    private:
			int fd_;
			ACS_SCH_EventHandle shutdownEvent;
			bool shutdownService;
			acs_apgcc_objectimplementerinterface_V3 *handle_;
};

/*class ACS_SCH_ClusterCPIMMCallbackThread : public ACE_Task_Base
{
    public:
      ACS_SCH_ClusterCPIMMCallbackThread(int fd, acs_apgcc_objectimplementerinterface_V3 *handle);
      int svc ();
			bool shutdownService;
    private:
      int fd_;
			ACS_SCH_EventHandle shutdownEvent;
			ACS_SCH_EventHandle scalingShutdownEvent;
      acs_apgcc_objectimplementerinterface_V3 *handle_;
};*/

#endif // ACS_SCH_IMMCallbackThread_h_
