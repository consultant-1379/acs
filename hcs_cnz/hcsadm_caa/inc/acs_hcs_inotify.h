//========================================================================================
// COPYRIGHT (C) Ericsson AB 2011 - All rights reserved
//
// The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written permission from Ericsson
// AB or in accordance with the terms and conditions stipulated in the agreement/contract
// under which the program(s) have been supplied.
//========================================================================================

#ifndef AcsHcs_inotify_h
#define AcsHcs_inotify_h

#include <sys/inotify.h>
#include <string>
#include <cstdlib>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <ACS_TRA_trace.h>
#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_tra.h"

class Inotify : public ACE_Task_Base
{
	public:
		Inotify();
		int svc();
		void watch_dir();
		void ruleSetFileInit(string, string);
		void ruleSetFileRemove(string);
		void ruleSetFileModify(string, string);
		void removeExistingRID();
		static bool stopRequested;
		static bool threadExited;
}; 

#endif

