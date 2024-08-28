#include "ACS_PRC_ispapi.h"
#include "acs_prc_ispapi_impl.h"
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <fstream>
#include <iostream>
//namespace ACS_PRC {

using namespace std;

//========================================================================================
//	Constructor     
//========================================================================================

Ispapi::Ispapi(){
	Isp_Implementation = new Ispapi_Implementation();
}

//========================================================================================
//	Destructor     
//========================================================================================

Ispapi::~Ispapi(){
	delete Isp_Implementation;
}

//========================================================================================
//	Open the log file     
//========================================================================================

bool Ispapi::open(){
	return Isp_Implementation->open();
}


//========================================================================================
//	Close the log file     
//========================================================================================

bool Ispapi::close(){
    return Isp_Implementation->close();
}


//========================================================================================
//	Report a node runlevel event     
//========================================================================================

bool Ispapi::runLevelEvent(string node, runLevelType level, reasonType reason, string info, time_t ftime){

	if ( info.size() == 0)
		info = " ";

	return Isp_Implementation->runLevelEvent(node, level, reason, info, ftime);
}

//========================================================================================
//	Report a node state event     
//========================================================================================

bool Ispapi::nodeStateEvent(string node, nodeStateType state, reasonType reason, string info, time_t ftime){

	if ( info.size() == 0)
		info = " ";

	return Isp_Implementation->nodeStateEvent(node, state, reason, info, ftime);
}

//========================================================================================
//	Report a resource state event    
//========================================================================================

bool Ispapi::SuStateEvent(string resourceName, string node, resourceStateType state, reasonType reason, string info, time_t ftime){

	if ( info.size() == 0)
		info = " ";

	return Isp_Implementation->SuStateEvent(resourceName, node, state, reason, info, ftime);
}

//========================================================================================
//	Reports that the other node has shut down    
//========================================================================================

bool Ispapi::otherNodeShutDown(std::string node, reasonType reason, std::string info, time_t ftime){

	if ( info.size() == 0)
		info = " ";

	return Isp_Implementation->otherNodeShutDown( node, reason, info, ftime);
}

int Ispapi::getFirstEventLogTime( std::string& time ){
	return Isp_Implementation->getFirstEventLogTime( time );
}

bool Ispapi::APStateEvent( int APstate, time_t ftime){
	return Isp_Implementation->APStateEvent( APstate, ftime );
}

void Ispapi::setremoteNode( const char* remoteNode ){
	Isp_Implementation->setremoteNode( remoteNode );
}

void Ispapi::setlocalNode( const char* localNode ){
	Isp_Implementation->setlocalNode( localNode );
}

bool Ispapi::node_State_Runlevel_Sync ( std::string node, nodeStateType state, reasonType node_reason, runLevelType RunLEvel, reasonType runlevel_reason){

	return Isp_Implementation->node_State_Runlevel_Sync(node, state, node_reason, RunLEvel, runlevel_reason);
}
