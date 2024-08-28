//******************************************************************************
//
//  NAME
//     acs_lm_dsdserver.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2011. All rights reserved.
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
//      2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>
#include "acs_lm_dsdserver.h"
#include "acs_lm_server.h"
//#include "acs_lm_cmd.h"
//#include "acs_lm_testlkf.h"
#include "acs_lm_lksender.h"
#include "acs_lm_tra.h"
//#include "acs_lm_electronickey_runtimeowner.h"
//#include "acs_lm_haservice.h"
//#include "acs_prc_api.h"
using namespace std;
//ACE_HANDLE stopThreadFds[2];


/*=================================================================
	ROUTINE: ACS_LM_DSDServer constructor
=================================================================== */
ACS_LM_DSDServer::ACS_LM_DSDServer(ACE_HANDLE endEvent)
:isStop(false),
 theClienthandlerPtr(NULL),
 //: clientHandler(NULL),
 //serviceStopEvent(NULL),
 stopThreadFds(endEvent)
{
	DEBUG("ACS_LM_DSDServer::ACS_LM_DSDServer() %s","Entering");
	//        serviceStopEvent = new ACE_Event();
	//        serviceStopEvent = new ACE_Event();
	//        ACE_OS::pipe(stopThreadFds);

	DEBUG("ACS_LM_DSDServer::ACS_LM_DSDServer() %s","Leaving");
}//end of constructor

/*=================================================================
	ROUTINE: ACS_LM_DSDServer destructor
=================================================================== */
ACS_LM_DSDServer::~ACS_LM_DSDServer()
{
	DEBUG("ACS_LM_DSDServer::~ACS_LM_DSDServer() %s","Entering");

	//        delete clientHandler;
	//        clientHandler = 0;

	DEBUG("ACS_LM_Server::~ACS_LM_Server() %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: stop
=================================================================== */
void ACS_LM_DSDServer::stop()
{
	DEBUG("ACS_LM_DSDServer::stop() %s","Entering");
//	if(serviceStopEvent != NULL)
//	{
//		serviceStopEvent->signal();
//		serviceStopEvent->remove();
//		serviceStopEvent = NULL;
//	}
	cmdServer.setDSDServerStopSignal(true);
	cmdServer.close();
	DEBUG("ACS_LM_DSDServer::stop() %s","Leaving");
}//end of stop
/*=================================================================
	ROUTINE: start
=================================================================== */
void ACS_LM_DSDServer::start(void)
{
	DEBUG("ACS_LM_DSDServer::start() %s","Entering");

	DEBUG("ACS_LM_Server::start() %s","Before activate()");
	activate();
	DEBUG("ACS_LM_Server::start() %s","Leaving");
}//end of start
/*=================================================================
	ROUTINE: setClienthandler
=================================================================== */
void ACS_LM_DSDServer::setClienthandler(ACS_LM_ClientHandler* aClienthandler)
{
	DEBUG("ACS_LM_DSDServer::setClienthandler() %s","Entering");
	theClienthandlerPtr = aClienthandler;
	DEBUG("ACS_LM_DSDServer::setClienthandler() %s","Leaving");
}//end of setClienthandler

/*=================================================================
	ROUTINE: svc
=================================================================== */
int ACS_LM_DSDServer::svc(void)
{
	DEBUG("ACS_LM_DSDServer::svc() %s","Entering" );
	if(cmdServer.listen())
	{
		DEBUG("ACS_LM_DSDServer::svc()  %s","cmdServer.listen() - Success");
		while(true)
		{
			ACE_INT32 ret = cmdServer.accept();
			if(ret == 1)
			{
				DEBUG("ACS_LM_DSDServer::svc()  %s","client connected");

				theClienthandlerPtr->handleClient(cmdServer);

				DEBUG("ACS_LM_DSDServer::svc()  %s","After theClientHandler->handleClient(cmdServer) ");

				cmdServer.close();
				DEBUG("ACS_LM_DSDServer::svc()  %s","cmdServer.close()");
			}
			else if(ret == -1)
			{

				DEBUG("%s","ACS_LM_DSDServer::svc(): Service stop requested");
				break;
			}
		}

	}
	else
	{
		DEBUG("ACS_LM_DSDServer::svc()  %s","cmdServer.listen() failed");
	}

	DEBUG("ACS_LM_DSDServer::svc() %s","Leaving" );

	return 0;
}//end of svc
