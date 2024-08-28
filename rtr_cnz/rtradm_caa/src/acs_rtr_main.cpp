//********************************************************************************
//
// NAME
// ACS_RTR_main.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2007.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2007-05-10 by GP/AP/A QPAOELE
//
// DESCRIPTION 
// Entry point for RTR service.
//
//********************************************************************************

//#include <winsock2.h>


#include "acs_rtr_global.h"
#include "acs_rtr_server.h"
#include "acs_rtr_tracer.h"

// The RTR Main Server
ACS_RTR_Server *server = 0;

ACS_RTR_TRACE_DEFINE(ACS_RTR_Main)

int StartUpRTR(int /*argc*/,char** /*argv*/,int /*startMode*/)
{
	ACS_RTR_TRACE_FUNCTION;
	int exitCode = 0;
	server = new ACS_RTR_Server();

	if(server->open() == 0)
	{
		server->wait();
	}
	else
		exitCode = internalRestart;

	delete server;
	server = 0;
	return exitCode;
}
