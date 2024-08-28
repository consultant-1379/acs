/*
 * ACS_PRC_IspLogEvent.cpp
 *
 *  Created on: Nov 18, 2010
 *      Author: xgiufer
 */

#include "ACS_PRC_IspLogEvent.h"



ACS_PRC_IspLogEvent::ACS_PRC_IspLogEvent() {
	// TODO Auto-generated constructor stub
	hostname     = "";
	runLevel     = "";
	event        = "";
	reason       = "";
	reasonInfo   = "";
	date         = "";
	miscInfo     = "";
	resourceName = "";
}

ACS_PRC_IspLogEvent::ACS_PRC_IspLogEvent(const ACS_PRC_IspLogEvent &x ) {
	// constructor stub

	hostname     = x.hostname;
	runLevel     = x.runLevel;
	event        = x.event;
	reason       = x.reason;
	reasonInfo   = x.reasonInfo;
	date         = x.date;
	miscInfo 	   = x.miscInfo;
	resourceName = x.resourceName;
}


ACS_PRC_IspLogEvent::~ACS_PRC_IspLogEvent() {
	// destructor stub
	}



