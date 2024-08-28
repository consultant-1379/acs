/*=================================================================== */
/**
   @file   acs_alog_nodestate.h

   @brief Header file for acs_alog_nodestate type module.

          This module contains all the declarations useful to
          specify the class.

   @version 2.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
   N/A       28/11/2011     xgencol               Final Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef acs_alog_nodestate_H_
#define acs_alog_nodestate_H_

#include <iostream>
#include <string.h>
#include <errno.h>

#include "acs_alog_activeWorks.h"
#include "saAis.h"
#include "saAmf.h"
#include "saNtf.h"

/* Internal structs */
typedef struct {
	SaUint16T numEventTypes;
	SaUint16T numNotificationObjects;
	SaUint16T numNotifyingObjects;
	SaUint16T numNotificationClassIds;
	SaUint32T numProbableCauses;
	SaUint32T numPerceivedSeverities;
	SaUint32T numTrends;
} saNotificationFilterAllocationParamsT;

class acs_alog_nodestate {

	private:

		pthread_t nodeStateThread;
		ACE_thread_t alog_watch_state_thread_id;

	public:

		acs_alog_nodestate() {};
		~acs_alog_nodestate() {};

		static SaNtfHandleT ntfHandle;
		static SaSelectionObjectT selObj;

		static char *error_output(SaAisErrorT);

		static void saNtfNotificationCallback(SaNtfSubscriptionIdT,const SaNtfNotificationsT*);
		static SaAisErrorT waitForNotifications(SaNtfHandleT,int,int);
		static SaAisErrorT subscribeForNotifications(void);

		static void* watchForNodeState(void*);

		int startWatchForNodeState(void);
};

#endif /* acs_alog_nodestate_H_ */

