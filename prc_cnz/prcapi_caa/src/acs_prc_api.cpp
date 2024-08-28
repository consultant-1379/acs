//==============================================================================
//
// NAME
//      ACS_PRC_API.cpp
//
//  COPYRIGHT Ericsson AB, Sweden 2003.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
// .DESCRIPTION 
//	See description for ACS_PRC_API.h. 
//
// DOCUMENT NO
//	190 89-CAA 109 0520 Ux
//
// AUTHOR 
// 	2003-05-14 by EAB/UKY/GB LMGR
//
// CHANGE HISTORY
//  2007-07-16	   XCSVAAN	         R3G        	For TR HI20695
// SEE ALSO 
// 	-
//
//==============================================================================
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "acs_prc_api.h"
#include "acs_prc_api_impl.h"
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "ACS_PRC_ispapi.h"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Connector.h>

//using namespace ACS_PRC;
using namespace std;

ACS_PRC_API::ACS_PRC_API(){
	Api_Implementation = new acs_prc_apiImplementation();
} // End of ACS_PRC_API constructor

ACS_PRC_API::~ACS_PRC_API(){
	delete Api_Implementation;
} // End of ACS_PRC_API destructor

bool ACS_PRC_API::isRebootPossible (reasonType reason){

	return Api_Implementation->isRebootPossible( reason );
}

bool ACS_PRC_API::rebootNode (reasonType reason,const char* userInfo, bool reboot)
{
	return Api_Implementation->rebootNode( reason, userInfo, reboot );
}
//changes start for TR HW49302

int ACS_PRC_API::getSuState_new (vector<string>& mySuList, const char* SuName ){

	return Api_Implementation->getSuState_new(mySuList, SuName);
}

int ACS_PRC_API::getSuState_new ( vector<string>& mySuList, const char* SuName, const char* SuSide ){

	return Api_Implementation->getSuState_new(mySuList, SuName, SuSide);
}

int ACS_PRC_API::omHandler_init(){

	return Api_Implementation->omHandler_init();
}

int ACS_PRC_API::omHandler_finalize(){

	return Api_Implementation->omHandler_finalize();
}

bool ACS_PRC_API::getSUList_new(const char* Node_new, vector<string>& SuList_new){

	return Api_Implementation->getSUList_new(Node_new, SuList_new);
}
//changes end for TR HW49302

int ACS_PRC_API::getSuState ( const char* SuName ){

	return Api_Implementation->getSuState( SuName );
}

int ACS_PRC_API::getSuState ( const char* SuName, const char* SuSide ){

	return Api_Implementation->getSuState( SuName, SuSide );
}

time_t ACS_PRC_API::lastRebootTime(){

	return Api_Implementation->lastRebootTime();
}

bool ACS_PRC_API::getSUList(const char* Node, vector<string>& SuList){

	return Api_Implementation->getSUList( Node, SuList );
}

int ACS_PRC_API::getLastError(void) const{

	return Api_Implementation->getLastError();
}

const char* ACS_PRC_API::getLastErrorText(void) const{

	return Api_Implementation->getLastErrorText();
}

int ACS_PRC_API::askForNodeState(){

	return Api_Implementation->askForNodeState();
}

int ACS_PRC_API::askForNodeState( int Node ){

	return Api_Implementation->askForNodeState ( Node );
}

int ACS_PRC_API::askForNodeState( string Node ){

	return Api_Implementation->askForNodeState ( Node );
}
