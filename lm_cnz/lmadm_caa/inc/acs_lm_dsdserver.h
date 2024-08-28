//******************************************************************************
//
//  NAME
//     ACS_LM_DSDSERVER.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
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
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2008-11-28 by XCSSAGO PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
#ifndef _ACS_LM_DSDSERVER_H_
#define _ACS_LM_DSDSERVER_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
//Include file for Sentinel RMS
#include <iostream>
#include <ace/ACE.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include "acs_lm_common.h"
#include "acs_lm_cmdserver.h"
//#include "acs_lm_scheduler.h"
//#include "acs_lm_cprestartmonitor.h"
#include "acs_lm_eventhandler.h"
#include "acs_lm_clienthandler.h"
//#include "acs_lm_runtimeowner.h"
//#include "acs_lm_electronickey_runtimeowner.h"
//#include "acs_lm_root_objectImplementer.h"
////#include "acs_prc_process.h"  //Include file for PRC functions
////#include "acs_exceptionhandler.h"   //Include file for Crash routines
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_DSDServer: public ACE_Task_Base
{
public :
	/*===================================================================
			   				 PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
			   				PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_LM_DSDServer

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_DSDServer(ACE_HANDLE stopThreadFds);

	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_DSDServer

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_DSDServer();
	/*=================================================================== */
	/**
	 *  @brief start
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void start();
	/*=================================================================== */
	/**
	 *  @brief stop
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void stop();

	//svc method
	/*=================================================================== */
	/**
	 *  @brief svc
	 *
	 *  @return int
	 **/
	/*=================================================================== */
	int svc(void);
	/*=================================================================== */
	/**
	 *  @brief setClienthandler
	 *
	 *  @param aClienthandler      			: ACS_LM_ClientHandler
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void setClienthandler(ACS_LM_ClientHandler* aClienthandler);
	/*=================================================================== */

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */

	bool isStop;

	ACS_LM_ClientHandler* theClienthandlerPtr;

	ACS_LM_CmdServer cmdServer;
	
	//ACE_Event * serviceStopEvent;
	
	ACE_HANDLE stopThreadFds;
};
// ACS_LM_DSDServer END
#endif

