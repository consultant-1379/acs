/*
 * acs_prc_runlevel.cpp
 *
 *  Created on: Mar 14, 2012
 *      Author: xlucpet
 */

#include "acs_prc_runlevel.h"
#include "acs_prc_reporting.h"
#include "ACS_TRA_Logging.h"
#include "acs_apgcc_omhandler.h"
#include <utmp.h>

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

map<string,int> acs_prc_runlevel::suStateSC1Map;
map<string,int> acs_prc_runlevel::suStateSC2Map;

int AP_Status[6][6] = {{0,0,0,5,5,0},{1,1,1,5,5,1},{1,1,1,5,5,2},{5,5,5,5,5,5},{5,5,5,5,5,5},{3,3,3,5,5,4}};

const char dnapNodeInfoId_node1[] = "apNodeInfoId=1,processControlId=1";
const char dnapNodeInfoId_node2[] = "apNodeInfoId=2,processControlId=1";
extern bool is_swm_2_0; 
runLevelType acs_prc_runlevel::currentRunLevel_local = level_0;
runLevelType acs_prc_runlevel::currentRunLevel_remote = level_0;

namespace RUNLEVEL
{
	const char HALTED = '0'; // System halt
	const char NORMAL = '3'; // Full multiuser mode with network
	const char REBOOT = '6';  // System reboot
}

int acs_prc_runlevel::computeRunLevelSC1(){

	int runLevelSC1 = 2;

	bool RunningSuSC1 = false;
	bool StoppedSuSC1 = false;

	map<string,int>::iterator it;

	for( it= suStateSC1Map.begin(); it != suStateSC1Map.end(); it++ ){

		if ( it->second > 0){
			RunningSuSC1 = true;
		}else{
			StoppedSuSC1= true;
		}
	}/*end for*/

	if ( RunningSuSC1 ){
		if (!StoppedSuSC1)
			runLevelSC1= 5; /*running is true and  StoppedSC1 is false:no SU is not running*/
		else
			runLevelSC1= 2;/*running is true and  StoppedSC1 is true: at least one SU is not running*/
	}
	else
		runLevelSC1 = 1;

	return runLevelSC1;

}/*end getRunLevelSC1 */

int acs_prc_runlevel::computeRunLevelSC2(){

	int runLevelSC2 = 2;

	bool RunningSuSC2=false;
	bool StoppedSuSC2=false;

	map<string,int>::iterator it;

	for(it = suStateSC2Map.begin(); it != suStateSC2Map.end(); it++ ){

		if ( it->second > 0){
			RunningSuSC2 = true;
		}else{
			StoppedSuSC2= true;
		}
	}

	/*compute runlevel*/
	if ( RunningSuSC2 ){
		if (!StoppedSuSC2)
			runLevelSC2= 5; /*running is true and  StoppedSC1 is false:no SU is not running*/
		else
			runLevelSC2= 2;/*running is true and  StoppedSC1 is true: at least one SU is not running*/
	}
	else
		runLevelSC2 = 1;

	return runLevelSC2;

}/*end getRunLevelSC2 */

int acs_prc_runlevel::getRunLevel(string node){

	if (node == "SC-2-1" ){ /*local node requested*/

		return computeRunLevelSC1(); /*local is sc-2-1*/
	}else{

		return computeRunLevelSC2(); /*local is sc-2-2*/
	}
}/*end getRunLevel */

acs_prc_runlevel::acs_prc_runlevel(){
	prc_isp_Event = new Ispapi();
	prc_isp_Event->open();
	counter = 0;
	flag = false;
	data = new ACS_PRC_suInfo();
	data->getInfo();

	data->getMap( &suStateSC1Map, &suStateSC2Map);
}

acs_prc_runlevel::~acs_prc_runlevel() {

	prc_isp_Event->close();

	delete prc_isp_Event;
	delete data;
}

bool acs_prc_runlevel::sendPrcAlarm ( string Node, runLevelType NodeLevel, runLevelType previousRunLevel_remote ) {

	if ( (( NodeLevel == level_2 ) && ( previousRunLevel_remote == level_1 )) ||
		 (( NodeLevel == level_1 ) && ( previousRunLevel_remote == level_2 )) ){
		return true;
	}

	char log[512] = {0};

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	snprintf(log,512,"Enter acs_prc_runlevel::sendPrcAlarm - Node : %s - RunLevel : %i",Node.c_str(),NodeLevel);

	Logging.Write(log,LOG_LEVEL_WARN);

	acs_prc_report report;

	if ( report.alarmAPNotRedundant ( ACS_PRC_ceaseSeverity, ACS_PRC_up, Node.c_str() ) != ACS_PRC_Report_OK ){
		Logging.Write("alarmAPNotRedundant Error - ACS_PRC_ceaseSeverity",LOG_LEVEL_ERROR);
		snprintf(log,512,"AEH API Error : %i - Error Text : %s",report.getError(),report.getErrorText().c_str());
		Logging.Write(log,LOG_LEVEL_ERROR);
	}

	if ( NodeLevel == level_0 ){
		if ( report.alarmAPNotRedundant ( ACS_PRC_a3Severity, ACS_PRC_down, Node.c_str() ) != ACS_PRC_Report_OK )
			Logging.Write("alarmAPNotRedundant Error - ACS_PRC_down",LOG_LEVEL_ERROR);
	}
	else if ( ( NodeLevel >= level_1 ) && ( NodeLevel <= level_4 ) ){
		if ( report.alarmAPNotRedundant ( ACS_PRC_a3Severity, ACS_PRC_degraded, Node.c_str() ) != ACS_PRC_Report_OK )
			Logging.Write("alarmAPNotRedundant Error - ACS_PRC_degraded",LOG_LEVEL_ERROR);
	}

	Logging.Close();

	return true;
}

bool acs_prc_runlevel::sendIspEvent ( string Node, runLevelType NodeLevel, reasonType /*reason*/ ) {

	char log[512] = {0};

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	snprintf(log,512,"Enter acs_prc_runlevel::sendIspEvent - Node : %s - RunLevel : %i",Node.c_str(),NodeLevel);

	Logging.Write(log,LOG_LEVEL_WARN);

	if ( NodeLevel <= level_4 )
		prc_isp_Event->runLevelEvent(Node,(runLevelType)NodeLevel,resourceFailed," ");
	else
		prc_isp_Event->runLevelEvent(Node,(runLevelType)NodeLevel,unknownReason," ");

	Logging.Close();

	return true;
}

bool acs_prc_runlevel::sendIspEvent_APState ( runLevelType Local_NodeLevel, runLevelType Remote_NodeLevel ) {

	char log[512] = {0};

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	snprintf(log,512,"Enter acs_prc_runlevel::sendIspEvent_APState - APStateEvent : %i", AP_Status[Local_NodeLevel][Remote_NodeLevel] );

	Logging.Write(log,LOG_LEVEL_WARN);

	prc_isp_Event->APStateEvent ( AP_Status[Local_NodeLevel][Remote_NodeLevel] );

	Logging.Close();

	return true;
}

bool acs_prc_runlevel::isNodeShutDownOngoing()
{
	bool result = false;
	struct utmp* ut_record;
	// open user accounting database
	setutent();

	// search runlevel record
	while( (ut_record = getutent()) != NULL )
	{
		if(RUN_LVL == ut_record->ut_type)
		{
			// get current system runlevel
			char runLevel = ut_record->ut_pid % 256;
			result = (RUNLEVEL::HALTED == runLevel) || (RUNLEVEL::REBOOT == runLevel);
			break;
		}
	}
	// close user accounting database.
	endutent();

	return result;
}

bool acs_prc_runlevel::calculateRunLevel(){

	int RunLevel_Node_local = -1;
	int RunLevel_Node_remote = -1;

	reasonType reason = unknownReason;


	if (remote_Node.compare("") == 0 ){/* check if remote node has correct value */

		ifstream ifs;
		string m_node_hostname_remote = "";

		ifs.open("/etc/cluster/nodes/peer/hostname");
		if ( ifs.good())
			getline(ifs,m_node_hostname_remote);
		ifs.close();

		remote_Node = m_node_hostname_remote;
	}

	RunLevel_Node_local = getRunLevel(local_Node);

	if( 1 ==  RunLevel_Node_local )	++RunLevel_Node_local;

	ACE_SOCK_Connector serverConnector;
	ACE_SOCK_Stream serverStream;
	ACE_INET_Addr server_addr(8881,remote_Node.c_str());
	ACE_Time_Value val(1);
	char strErr[1024] = {0};      

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	if( serverConnector.connect(serverStream, server_addr, &val) == -1){

		if ( errno == ECONNREFUSED ){
			RunLevel_Node_remote = getRunLevel(remote_Node);
			flag = false;
			if ( RunLevel_Node_remote == 5)
				RunLevel_Node_remote = 2;

			snprintf(strErr,sizeof(strErr),"acs_prcmand %s - acs_prc_runlevel::calculateRunLevel() - Runlevel %i detected for %s - return code = %d (%s)", PRCBIN_REVISION, RunLevel_Node_remote, remote_Node.c_str(), errno, strerror(errno) );
		}
		else
		{
			// Remote node down
			RunLevel_Node_remote = 0;
			flag = true;
			if( isNodeShutDownOngoing() ) RunLevel_Node_local = 0;
			counter++;

			snprintf(strErr,sizeof(strErr),"acs_prcmand %s - acs_prc_runlevel::calculateRunLevel() - Runlevel 0 detected for %s - return code = %d (%s)", PRCBIN_REVISION, remote_Node.c_str(), errno, strerror(errno) );
		}

		Logging.Write(strErr,LOG_LEVEL_WARN);
	}
	else{
		RunLevel_Node_remote = getRunLevel(remote_Node);
		flag = false;
		if ( RunLevel_Node_remote == 1)
			RunLevel_Node_remote++;
	}


	serverStream.close();

	int node_state = ACS_PRC_NODE_ACTIVE;
	// this node is the active when the other node is down
	//Start of TR HX29285
	ACS_PRC_API tmp;
	node_state = tmp.askForNodeState();
	//End of TR HX29285
	bool sendIspEvent_APState = false;

	prc_isp_Event->setlocalNode(local_Node.c_str());
	prc_isp_Event->setremoteNode(remote_Node.c_str());

	if ( RunLevel_Node_local != currentRunLevel_local )
	{

		if ( ( node_state == ACS_PRC_NODE_ACTIVE )||( node_state == ACS_PRC_NODE_PASSIVE ) )	//Fix for TR HZ40239
		{
			currentRunLevel_local = (runLevelType)RunLevel_Node_local;
		}
		else{
			snprintf(strErr,sizeof(strErr),"acs_prcmand %s - acs_prc_runlevel::calculateRunLevel() - node_state is %i", PRCBIN_REVISION, node_state);
			Logging.Write(strErr,LOG_LEVEL_WARN);
		}

		if ( ( node_state == ACS_PRC_NODE_ACTIVE )||( node_state == ACS_PRC_NODE_PASSIVE ) )		//Fix for TR HZ40239
		{
			this->sendIspEvent( local_Node, currentRunLevel_local, reason );
			sendIspEvent_APState = true;
		}
	}

	if ( RunLevel_Node_remote != currentRunLevel_remote )
	{
		runLevelType previousRunLevel_remote;

		if ( ( node_state == ACS_PRC_NODE_ACTIVE )||( node_state == ACS_PRC_NODE_PASSIVE ) )	//Fix for TR HZ40239
		{
			previousRunLevel_remote = (runLevelType)currentRunLevel_remote;
			currentRunLevel_remote = (runLevelType)RunLevel_Node_remote;
		}
		else{
			snprintf(strErr,sizeof(strErr),"acs_prcmand %s - acs_prc_runlevel::calculateRunLevel() - node_state is %i", PRCBIN_REVISION, node_state);
			Logging.Write(strErr,LOG_LEVEL_WARN);
		}

		ACS_PRC_API tmp;
		// start of TR HY79295
		if(node_state == ACS_PRC_NODE_PASSIVE)
		{
			counter = 0;
			snprintf(strErr,sizeof(strErr),"acs_prcmand %s - acs_prc_runlevel::calculateRunLevel() - node state is passive, so counter will be set to 0", PRCBIN_REVISION);
			Logging.Write(strErr,LOG_LEVEL_WARN);
		}//end of TR HY79295
		else if( node_state == ACS_PRC_NODE_ACTIVE ) 
		{
			if(counter < 2 && flag==true)
			{
				snprintf(strErr,sizeof(strErr),"acs_prcmand %s - acs_prc_runlevel::calculateRunLevel() - Counter  %d", PRCBIN_REVISION,counter);
				Logging.Write(strErr,LOG_LEVEL_WARN);
			}

			else {
				counter = 0;
				//this->sendIspEvent( remote_Node, currentRunLevel_remote, reason );
				this->sendPrcAlarm( remote_Node, currentRunLevel_remote, previousRunLevel_remote );
				//sendIspEvent_APState = true;
			}
		}
	}

	if( sendIspEvent_APState )
	{
		this->sendIspEvent_APState( currentRunLevel_local, currentRunLevel_remote );
	}
	Logging.Close();
	return true;
}

void acs_prc_runlevel::refreshSuList ( ) {
	data->getInfo();
	data->getMap( &suStateSC1Map, &suStateSC2Map);
}

void acs_prc_runlevel::updateSUState ( string SuName, int SuState ) {

	string apg_block_name;
	string apg_block_name_side;

	if ( SuName.find("The HA state of SI safSi=") != string::npos ){// 2N
		//The HA state of SI safSi=LCT,safApp=ERIC-APG assigned to SU safSu=1,safSg=2N,safApp=ERIC-APG changed - 2

		size_t start = SuName.find("safSi=") + 6;
		size_t end;
		if (is_swm_2_0 == true){
			end = SuName.find(",safApp=ERIC-apg.nbi.aggregation.service");
		        apg_block_name = SuName.substr ( start, end - start);
		      }	
		else{
			end = SuName.find(",safApp=ERIC-APG");
		        apg_block_name = "safComp=" + SuName.substr ( start, end - start);
		}
		start = SuName.find("safSu=") + 6;
		end = SuName.find(",safSg=2N");

		apg_block_name_side = SuName.substr ( start, end - start);
	}
	else { // NoRed
		size_t end;
		end = SuName.find("-");
		apg_block_name = "safComp=" + SuName.substr ( 0, end );

		apg_block_name_side = SuName.substr ( end + 1 );
	}

	if ( apg_block_name_side == "SC-1" ){
		suStateSC1Map[apg_block_name] = SuState;
	}
	else if ( apg_block_name_side == "SC-2" ){
		suStateSC2Map[apg_block_name] = SuState;
	}
	else if( apg_block_name_side == "1" ){
		suStateSC2Map[apg_block_name] = SuState;
	}
	else {
     	        suStateSC2Map[apg_block_name] = SuState;
	}
}

void acs_prc_runlevel::printMaps() {
	map<string,int>::iterator it;

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	char log[512] = {0};

	snprintf(log,512,"****************** suStateSC1Map ******************");
	Logging.Write(log,LOG_LEVEL_INFO);

	for( it= suStateSC1Map.begin(); it != suStateSC1Map.end(); it++ ){
		memset(log,0,512);
		snprintf(log,512,"%s / %i",it->first.c_str(),it->second);
		Logging.Write(log,LOG_LEVEL_INFO);
	}

	memset(log,0,512);
	snprintf(log,512,"****************** suStateSC2Map ******************");
	Logging.Write(log,LOG_LEVEL_INFO);

	for( it= suStateSC2Map.begin(); it != suStateSC2Map.end(); it++ ){
		memset(log,0,512);
		snprintf(log,512,"%s / %i",it->first.c_str(),it->second);
		Logging.Write(log,LOG_LEVEL_INFO);
	}

	Logging.Close();
}
