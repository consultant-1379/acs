//******************************************************************************
//
// NAME
// acs_rtr_lib_events.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2002.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// DOCUMENT NO
//
// AUTHOR 
// 2012-12-07 by XSAMECH
//
// DESCRIPTION
// This class handles events to be reported to AEH.
//******************************************************************************

#include <acs_rtr_lib_events.h>
#include <stdio.h>
#include "cstring"
//#include "acs_aeh_evreport.h"
class acs_aeh_evreport;

//*************************************************************************
// Constructor
//*************************************************************************
libEvents::libEvents(): evp(new acs_aeh_evreport())
{

	 char pidNo[20];
	 sprintf(pidNo, "PID:%d", getpid());
   	 strcpy(procName, pidNo);
        
        memset(events,0,sizeof(eventList)*100);
	// 10360
	strcpy(events[0].probableCause,"ACS_RTR, configuration fault");
	strcpy(events[0].objectOfRef,"Config");
	strcpy(events[0].problemDataFormat,"setDirConfig(msName,siteName)");
	strcpy(events[0].problemTextFormat,"");

	// 10361
	strcpy(events[1].probableCause,"ACS_RTR, configuration fault");
	strcpy(events[1].objectOfRef,"Config");
	strcpy(events[1].problemDataFormat,"Directory problems");
	strcpy(events[1].problemTextFormat,"");

	// 10362
	strcpy(events[2].probableCause,"ACS_RTR, failure at setting parameter");
	strcpy(events[2].objectOfRef,"Config");
	strcpy(events[2].problemDataFormat,"Default parameter file problems");
	strcpy(events[2].problemTextFormat,"");

	// 10363
	strcpy(events[3].probableCause,"ACS_RTR, RTR service communication");
	strcpy(events[3].objectOfRef,"Config");
	strcpy(events[3].problemDataFormat,"connwritereadRTRpipe()");
	strcpy(events[3].problemTextFormat,"");

	// 10364
	strcpy(events[4].probableCause,"ACS_RTR, RTR key file");
	strcpy(events[4].objectOfRef,"Config");
	strcpy(events[4].problemDataFormat,"Key file problems");
	strcpy(events[4].problemTextFormat,"");
	
}

//*************************************************************************
// Destructor
//*************************************************************************
libEvents::~libEvents()
{
	delete evp;
}


//*************************************************************************
// reportEvent
//
// Report event to AEH.
//
// return value:
//   -
//*************************************************************************
void libEvents::reportEvent(int problemNo,const char* probText)
{
	
	evp->sendEventMessage(procName, 
					 problemNo, 
					 "EVENT", 
					 events[problemNo-10360].probableCause,
					 "APZ", 
					 events[problemNo-10360].objectOfRef,
					 events[problemNo-10360].problemDataFormat,
					 probText);

}



		
