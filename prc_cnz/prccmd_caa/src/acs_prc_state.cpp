//******************************************************************************
//
// NAME
//      acs_prc_state.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// DOCUMENT NO
//	19089-CAA 109 0520

// AUTHOR 
//	2003-05-19 by EAB/UKY/GB UABLMGR

// REVISION
//  -	
// 

// CHANGES

//
//	REV NO		DATE		NAME 		DESCRIPTION
//			2003-05-19	UABLMGR
//                      2014-04-22	XSARSES         Fix for HS53075 	

// SEE ALSO 
// 	
//
//******************************************************************************

#include "acs_prc_api.h"
#include "acs_prc_api_impl.h"
#include "acs_apgcc_omhandler.h"
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <string>
#include <map>
#include <algorithm>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>
#include "ace/SOCK_Connector.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include "ACS_TRA_Logging.h"
#include "ACS_PRC_suInfo.h"
#include <ACS_CS_API.h>

#define SOFTWARE_INVENTORY  "CmwSwMUpgradePackage" //HS53075

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif
bool isSWM20_cmd();
const char pipeName[] = "/var/run/ap/PrcManServer";
const char refPath[] = "/opt/ap/acs/conf/res.conf";
bool is_swm_2_0_cmd = true ;
const char * const COMMAND = "/opt/coremw/bin/cmw-status instantiation 2>&1";
using namespace std;

map<string,int> NodeInformation;

/*-----------------------------------------------------------*/
/*  This method analyzes a cmw-status printout like this :   */
/*                                                           */
/*  tp212ap1a-SC-2-1:# cmw-status si                         */
/*  safSi=2N,safApp=ERIC-Brf                                 */
/*      AdminState=LOCKED(2)                                 */
/*      AssignmentState=UNASSIGNED(1)                        */
/*  safSi=ACA,safApp=ERIC-APG                                */
/*      AdminState=LOCKED(2)                                 */
/*      AssignmentState=UNASSIGNED(1)                        */
/*                                                           */
/*  to establish if a CBA component is not running           */
/*-----------------------------------------------------------*/

void  evaluateRunLevelForCBAs (int &RLlocal, int &RLremote)
{

	string  cmdString = "/opt/coremw/bin/cmw-status si";
	FILE   *fp = popen(cmdString.c_str(), "r");

	if (fp == NULL)  return;

	char    *line = NULL;
	size_t   pos, len = 0;
	ssize_t  read;
	string   strLine("");
	while ((read = getline(&line, &len, fp)) != -1)
	{
			strLine.assign(line);
			if (strLine.length() == 0)  continue;
		     if (is_swm_2_0_cmd == true){
			if ((pos = strLine.find("safApp="))  == string::npos)  continue;	//  select a line containing 'safApp=' and
			if ((pos = strLine.find("ERIC-apg")) != string::npos)  continue;	//  verify its value :  if is different by ERIC-APG,
		     }				    										//  a component is not running
		     else{
			if ((pos = strLine.find("safApp="))  == string::npos)  continue; 
			if ((pos = strLine.find("ERIC-APG")) != string::npos)  continue; 
		       	}	
		    	
			if (RLlocal  == 5)  RLlocal  = 2;		//  adjust the Run Level values
			if (RLremote == 5)  RLremote = 2;
			break;
	}
	pclose(fp);

	if (line)  free (line);
}


int getRestoreOnGoing (){

	//BrfPersistentStorageOwner

	OmHandler immHandle;
	vector<ACS_APGCC_ImmAttribute *> attributes;
	ACS_CC_ReturnType result;
	vector<string> tmp_vector;

	result = immHandle.Init();

	if ( result != ACS_CC_SUCCESS ){
		return 0;
	}

	result = immHandle.getClassInstances("BrMBrmBackup",tmp_vector); //TR HT43004 fix

	if ( result != ACS_CC_SUCCESS ){
		immHandle.Finalize();
		return 0;
	}

	string rdn_Backup;
	int bur = 0;

	ACS_CC_ImmParameter Backup_DN;
//	Backup_DN.attrName = const_cast<char*>("asyncActionProgress");	//  TR :  HT89827
	Backup_DN.attrName = const_cast<char*>("progressReport");		//  asyncActionProgress is deprecated

	ACS_CC_ImmParameter Backup_actionId;
	Backup_actionId.attrName = const_cast<char*>("actionId");

	ACS_CC_ImmParameter Backup_result;
	Backup_result.attrName = const_cast<char*>("result");

	for ( unsigned int i = 0; i < tmp_vector.size(); i++){

		result = immHandle.getAttribute(tmp_vector[i].c_str(), &Backup_DN);

		if ( result != ACS_CC_SUCCESS ){

			if ( immHandle.getInternalLastError() == -12 ){ // Invalid parameter
				bur = -1;
				break;
			}

			continue;
		}

		if (Backup_DN.attrValues[0] == 0) continue;		//  TR :  HT89153

		rdn_Backup = reinterpret_cast<char*>(Backup_DN.attrValues[0]);

		result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_actionId);

		if ( result != ACS_CC_SUCCESS ){
			continue;
		}

		if ( *(reinterpret_cast<int*>(Backup_actionId.attrValues[0])) == 3 ){ // Restore ongoing

			result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_result);

			if ( result != ACS_CC_SUCCESS ){
				continue;
			}

			if ( *(reinterpret_cast<int*>(Backup_result.attrValues[0])) == 3 ) { // result is NOT_AVAILABLE
				bur = 1;
				break;
			}
		}
	}

	if (bur == -1){		//  No restore in progress, this could be due to the progressReport attribute
						//  not defined in IMM. To assure the Backward Compatibility,
						//  a check on the asyncActionProgress attribute is executed.

		Backup_DN.attrName = const_cast<char*>("asyncActionProgress");

		for ( unsigned int i = 0; i < tmp_vector.size(); i++){

			result = immHandle.getAttribute(tmp_vector[i].c_str(), &Backup_DN);

			if ( result != ACS_CC_SUCCESS )  continue;

			if ( Backup_DN.attrValues[0] == 0 )  continue;		//  TR :  HT89153

			rdn_Backup = reinterpret_cast<char*>(Backup_DN.attrValues[0]);

			result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_actionId);

			if ( result != ACS_CC_SUCCESS )  continue;

			if ( *(reinterpret_cast<int*>(Backup_actionId.attrValues[0])) == 3 ){     // Restore ongoing

				result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_result);

				if ( result != ACS_CC_SUCCESS )  continue;

				if ( *(reinterpret_cast<int*>(Backup_result.attrValues[0])) == 3 ) {   // result is NOT_AVAILABLE
					bur = 1;
					break;
				}
			}
		}
	}
	immHandle.Finalize();

	return bur;
}

//Changes start for HS53075

int getSwUpgradeOnGoing (){
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	char strErr_no[1024] = {0};

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"prcstate %s - Entry in getSwUpgradeOnGoing()", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_INFO);

	ACS_CC_ReturnType returnCode;
	int last_error = 0;
	OmHandler immHandler;
	vector<string> tmp_vector;

	returnCode=immHandler.Init();

	if( returnCode != ACS_CC_SUCCESS ){
		last_error = immHandler.getInternalLastError();
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"prcstate %s - getSwUpgradeOnGoing function. Init fails (%i)", PRCBIN_REVISION, last_error);
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		return 0;
	}

	returnCode = immHandler.getClassInstances(SOFTWARE_INVENTORY,tmp_vector);

	if( (returnCode != ACS_CC_SUCCESS) || (tmp_vector.size()==0) ){
		last_error = immHandler.getInternalLastError();
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"prcstate %s - Couldn't find CmwSwMUpgradePackage list (%i)", PRCBIN_REVISION, last_error);
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		immHandler.Finalize();
		Logging.Close();
		return 0;
	}


	string rdn_Software;
	int Swr = 0;

	ACS_CC_ImmParameter Software_DN;
	Software_DN.attrName = const_cast<char*>("reportProgress");

	ACS_CC_ImmParameter Software_actionId;
	Software_actionId.attrName = const_cast<char*>("actionId");


	for ( unsigned int i = 0; i < tmp_vector.size(); i++){

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"prcstate %s -  CmwSwMUpgradePackage list %s", PRCBIN_REVISION, tmp_vector[i].c_str());
		Logging.Write(strErr_no, LOG_LEVEL_INFO);

		returnCode = immHandler.getAttribute(tmp_vector[i].c_str(), &Software_DN);

		if ( returnCode != ACS_CC_SUCCESS ){
			last_error = immHandler.getInternalLastError();
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"prcstate %s - getAttribute of reportProgress DN is failed (%i)", PRCBIN_REVISION, last_error);
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			continue;
		}

		if(Software_DN.attrValues[0] != 0){
			rdn_Software = reinterpret_cast<char*>(Software_DN.attrValues[0]);
		}

		returnCode = immHandler.getAttribute( rdn_Software.c_str(), &Software_actionId);

		if ( returnCode != ACS_CC_SUCCESS ){
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"prcstate %s - getAttribute of actionId is not successful", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			continue;
		}

		if (Software_actionId.attrValues[0] != 0){
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"prcstate %s -  CmwSwMUpgradePackage Software actionId %d", PRCBIN_REVISION, *(reinterpret_cast<int*>(Software_actionId.attrValues[0])));
			Logging.Write(strErr_no, LOG_LEVEL_DEBUG);

			if ( *(reinterpret_cast<int*>(Software_actionId.attrValues[0])) == 1 ){ // Software Upgrade ongoing

				Swr = 2;
				break;
			}
		}
	}

	returnCode=immHandler.Finalize();

	if( returnCode != ACS_CC_SUCCESS){
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"prcstate %s - ImmHandler.Finalize is not successful", PRCBIN_REVISION);
		Logging.Write(strErr_no, LOG_LEVEL_WARN);

		Logging.Close();
		return 0;
	}

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"prcstate %s - Exit from getSwUpgradeOnGoing()", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	Logging.Close();

	return Swr;

}
//Changes End for HS53075

int getInstantiationStatus(){

    ACS_TRA_Logging Logging;
    Logging.Open("PRC");
    char strErr_no[1024] = {0};
    FILE * ins_status_fd = 0;

    memset(strErr_no, 0, sizeof(strErr_no));
    snprintf(strErr_no,1024,"prcstate %s - Entry in getInstantiationStatus()", PRCBIN_REVISION);
    Logging.Write(strErr_no, LOG_LEVEL_INFO);
    char ins_status[75] = {0};
    if(!(ins_status_fd = popen(COMMAND, "r"))){
        int errno_copy = errno;
        memset(strErr_no, 0, sizeof(strErr_no));
        snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Call 'popen' returned a NULL pointer for instantiation status, errno = %d", PRCBIN_REVISION, errno_copy);
        Logging.Write(strErr_no, LOG_LEVEL_ERROR);
        Logging.Close();
        return 0;
       }
     if(!(fgets(ins_status, 75, ins_status_fd))){
         int errno_copy = errno;
         memset(strErr_no, 0, sizeof(strErr_no));
         snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Call 'fgets' failed for instantiation status, errno == %d", PRCBIN_REVISION, errno_copy);
         Logging.Write(strErr_no, LOG_LEVEL_ERROR);
         Logging.Close();
         pclose(ins_status_fd);
         return 0;
         }
        pclose(ins_status_fd);
        if(strstr(ins_status, "BOOTSTRAP_INSTANTIATION")){
            memset( strErr_no, 0, sizeof(strErr_no));
            snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Preparing for bootstrap", PRCBIN_REVISION);
            Logging.Write(strErr_no, LOG_LEVEL_WARN);
            Logging.Close();
            return 2;
        }
        else if(strstr(ins_status, "GENERATING_CONFIGURATION")){
            memset( strErr_no, 0, sizeof(strErr_no));
            snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Create node configuration", PRCBIN_REVISION);
            Logging.Write(strErr_no, LOG_LEVEL_WARN);
            Logging.Close();
            return 2;
         }

          else if(strstr(ins_status, "WAITING_FOR_CLUSTER")){
             memset( strErr_no, 0, sizeof(strErr_no));
             snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Waiting for cluster", PRCBIN_REVISION);
             Logging.Write(strErr_no, LOG_LEVEL_WARN);
             Logging.Close();
             return 2;
         }

         else if(strstr(ins_status, "PREPARING_STARTUP")){
             memset( strErr_no, 0, sizeof(strErr_no));
             snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Generating campaign for startup", PRCBIN_REVISION);
             Logging.Write(strErr_no, LOG_LEVEL_WARN);
             Logging.Close();
             return 2;
         }

        else if(strstr(ins_status, "STARTING_SYSTEM")){
            memset( strErr_no, 0, sizeof(strErr_no));
            snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Executing campaign", PRCBIN_REVISION);
            Logging.Write(strErr_no, LOG_LEVEL_WARN);
            Logging.Close();
            return 2;
         }
  
         else if(strstr(ins_status, "Status OK")){
             memset( strErr_no, 0, sizeof(strErr_no));
             snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. Instantiation Successful", PRCBIN_REVISION);
             Logging.Write(strErr_no, LOG_LEVEL_WARN);
             Logging.Close();
             return 0;
         }
         else if(strstr(ins_status, "INSTANTIATION")){
             memset( strErr_no, 0, sizeof(strErr_no));
             snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function. There is an error during the execution", PRCBIN_REVISION);
             Logging.Write(strErr_no, LOG_LEVEL_WARN);
             Logging.Close();
             return 2;
         }

         else{
             memset(strErr_no, 0, sizeof(strErr_no));
             snprintf(strErr_no,1024,"prcstate %s - getInstantiationStatus function failed.", PRCBIN_REVISION);
             Logging.Write(strErr_no, LOG_LEVEL_ERROR);
             Logging.Close();
             return 0;
          }

        memset(strErr_no, 0, sizeof(strErr_no));
        snprintf(strErr_no,1024,"prcstate %s - Exit from getInstantiationStatus function", PRCBIN_REVISION);
        Logging.Write(strErr_no, LOG_LEVEL_INFO);
        Logging.Close();
        return 0;
}

bool file_exist(int side)
{
	string clusterPath = "/cluster/etc/ap/acs/prc/conf/";
	string local_hostname_path = "/etc/cluster/nodes/this/hostname";
	string remote_hostname_path = "/etc/cluster/nodes/peer/hostname";
	string p_local_node_id;
	string p_remote_node_id;
	ifstream ifs;

	if(side == 0){
		ifs.open(local_hostname_path.c_str());
		if (ifs.good())
			getline(ifs, p_local_node_id);
		ifs.close();

		clusterPath.append(p_local_node_id);

		ifs.open(clusterPath.c_str());

		if (ifs.good())
		{
			ifs.close();
			return true;
		}else{
			ifs.close();
			return false;
		}
	}else{
		ifs.open(remote_hostname_path.c_str());
		if (ifs.good())
			getline(ifs, p_remote_node_id);
		ifs.close();

		clusterPath.append(p_remote_node_id);

		ifs.open(clusterPath.c_str());

		if (ifs.good())
		{
			ifs.close();
			return true;
		}else{
			ifs.close();
			return false;
		}
	}

}

void printRunLevel(const char* st, uint32_t rl, int side)
{
	switch (rl)
	{
		case 0:
			if(file_exist(side))
				cout << st << " node is rebooting.";
			else
				cout << st << " node is down";
			break;
		case 1: cout << st << " node is up (OS only)"; break;
		case 2: cout << st << " node is up but not all resources are running"; break;
		case 5: cout << st << " node is up and working"; break;
		default:cout << st << " node, unknown state"; break;
	} // End of switch
	cout << endl;
	return;
} // End of printRunLevel

void printLocalNodeState(int local, int remote){

	if (local == 1){

		cout << "active";

	}else {

		if (remote == 1 )

			cout << "passive";

		else

			cout << "undefined"; /*if neither the local nor the remote node is active the status is undefined*/
	}

}

void printRemoteNodeState(int local, int remote){

	if (remote == 1){

		cout << "active";

	}else {

		if (local == 1 ){

			cout << "passive";
		}

		else
			cout << "undefined"; /*if neither the local nor the remote node is active the status is undefined*/
	}


}

void printLocalNodeState(int local, int remote, uint32_t rl ){

	if (local == 1){

		printRunLevel("active",rl, 0);

	}else {

		if (remote == 1 ){

			printRunLevel("passive",rl, 0);
		}

		else
			cout << "undefined" << endl; /*if neither the local nor the remote node is active the status is undefined*/
	}

}

void printRemoteNodeState(int local, int remote, uint32_t rl ){

	if (remote == 1){

		printRunLevel("active",rl, 1);

	}else {

		if (local == 1 ){

			printRunLevel("passive",rl, 1);
		}

		else
			cout << "undefined" << endl; /*if neither the local nor the remote node is active the status is undefined*/
	}


}

void printActivities ( int state ){

	int bur = state & 0X0001;
	int swu = state & 0X0002;
        int ins = state & 0X0002;
	
        if ( (swu != 0) | (ins != 0) ){
                cout << "software update in progress" << endl;
        }


	if ( bur != 0 ){
		cout << "backup/restore in progress" << endl;
	}

	return;
} // End of printActivities

void GetRunLevelWithoutServer ( int local_node_state, const char* node_id ){

	int remote_node_state = -1;
	int local_runlevel = -1;
	int remote_runlevel = -1;

	string local_hostname, remote_hostname;

	ifstream ifs;

	ifs.open("/etc/cluster/nodes/this/hostname");
	if (ifs.good())
		getline(ifs, local_hostname);
	ifs.close();

	ifs.open("/etc/cluster/nodes/peer/hostname");
	if (ifs.good())
		getline(ifs, remote_hostname);
	ifs.close();

	ACS_PRC_suInfo data;

	// Calculate the two node state

	if ( local_node_state == ACS_PRC_NODE_ACTIVE )
		remote_node_state = ACS_PRC_NODE_PASSIVE;
	else
		remote_node_state = ACS_PRC_NODE_ACTIVE;

	data.getInfo();

	// Calculate local runlevel

	local_runlevel = 1; // If you are executing PRCSTATE then the local runlevel is at least 1

	ACE_SOCK_Connector serverConnector;
	ACE_SOCK_Stream serverStream;
	ACE_INET_Addr server_addr(8881,local_hostname.c_str());
	ACE_Time_Value val(1);

	if (serverConnector.connect (serverStream, server_addr, &val  ) == -1){

		// Local PRCMAN doesn't answer
		local_runlevel = data.getRunLevel( local_hostname.c_str() );

		if ( local_runlevel == 5 )
			local_runlevel = 2;
	}
	else {

		// COnnection to the local PRCMAN is OK
		local_runlevel = data.getRunLevel( local_hostname.c_str() );

		if ( local_runlevel == 1 )
			local_runlevel = 2;
	}

	serverStream.close();

	// Calculate remote runlevel

	remote_runlevel = 0; // Start to 0, PRCSTATE doesn't know the status of the remote node

	server_addr.set(8881,remote_hostname.c_str());

	if (serverConnector.connect (serverStream, server_addr, &val  ) == -1){

		// Remote PRCMAN doesn't answer
		int lastErr = ACE_OS::last_error();
		if ( lastErr == ECONNREFUSED ){ //REMOTE PRCMAN DOWN

			remote_runlevel = data.getRunLevel( remote_hostname.c_str() );

			if ( remote_runlevel == 5 )
				remote_runlevel = 2;
		}
		else // Remote node down
			remote_runlevel = 0;
	}
	else {

		// COnnection to the remote PRCMAN is OK
		remote_runlevel = data.getRunLevel( remote_hostname.c_str() );

		if ( remote_runlevel == 1 )
			remote_runlevel = 2;
	}

	serverStream.close();

	if ( strcmp( node_id, "1" ) == 0 ){
		NodeInformation["nodeStateA"] = local_node_state;
		NodeInformation["nodeStateB"] = remote_node_state;
		NodeInformation["nodeRunLevelA"] = local_runlevel;
		NodeInformation["nodeRunLevelB"] = remote_runlevel;
	}
	else {
		NodeInformation["nodeStateB"] = local_node_state;
		NodeInformation["nodeStateA"] = remote_node_state;
		NodeInformation["nodeRunLevelB"] = local_runlevel;
		NodeInformation["nodeRunLevelA"] = remote_runlevel;
	}

}

int GetNodeStatus ( int node ){

	char strErr_no[1024] = {0};

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	OmHandler immHandle;
	vector<ACS_APGCC_ImmAttribute *> attributes;
	ACS_CC_ReturnType result;
	vector<string> tmp_vector;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		snprintf(strErr_no,1024,"prcstate %s - immHandle.Init failed - Error Code : %i - Error Text : %s", PRCBIN_REVISION, immHandle.getInternalLastError(), immHandle.getInternalLastErrorText() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		return -1;
	}

	result = immHandle.getClassInstances("ProcessControl",tmp_vector);

	if ( result != ACS_CC_SUCCESS ){
		snprintf(strErr_no,1024,"prcstate %s - immHandle.getClassInstances failed - Error Code : %i - Error Text : %s", PRCBIN_REVISION, immHandle.getInternalLastError(), immHandle.getInternalLastErrorText() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		immHandle.Finalize();
		return -1;
	}

	ACS_APGCC_ImmAttribute attribute_1;
	ACS_APGCC_ImmAttribute attribute_2;
	ACS_APGCC_ImmAttribute attribute_3;
	ACS_APGCC_ImmAttribute attribute_4;

	attribute_1.attrName = const_cast<char*>("nodeStateA");
	attribute_2.attrName = const_cast<char*>("nodeStateB");
	attribute_3.attrName = const_cast<char*>("nodeRunLevelA");
	attribute_4.attrName = const_cast<char*>("nodeRunLevelB");

	attributes.push_back(&attribute_1);
	attributes.push_back(&attribute_2);
	attributes.push_back(&attribute_3);
	attributes.push_back(&attribute_4);

	string node_id_path;

	ifstream ifs;

	string nameObject;

	if ( node == ACS_PRC_LOCAL_NODE ){
		node_id_path = "/etc/cluster/nodes/this/id";
	}
	else if ( node == ACS_PRC_REMOTE_NODE ) {
		node_id_path = "/etc/cluster/nodes/peer/id";
	}
	else {
		snprintf(strErr_no,1024,"prcstate %s - Invalid Node", PRCBIN_REVISION );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		immHandle.Finalize();
		return -1;
	}

	char node_id[8] = {0};

	ifs.open(node_id_path.c_str());
	if ( ifs.good())
		ifs.getline(node_id, 8);
	ifs.close();

	nameObject = "apNodeInfoId=" + (string)node_id + "," + tmp_vector[0];

	result = immHandle.getAttribute(nameObject.c_str(), attributes);

	if ( result != ACS_CC_SUCCESS ){
		snprintf(strErr_no,1024,"prcstate %s - immHandle.getAttribute failed - Error Code : %i - Error Text : %s", PRCBIN_REVISION, immHandle.getInternalLastError(), immHandle.getInternalLastErrorText() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		int Imm_Error = immHandle.getInternalLastError();
		immHandle.Finalize();
		return Imm_Error;
	}

	for ( int i = 0; i <= 3 ; i++ ){
		if ( attributes[i]->attrValuesNum > 0 ){
			NodeInformation[attributes[i]->attrName] = *(reinterpret_cast<int*>(attributes[i]->attrValues[0]));
		}
		else {
			NodeInformation[attributes[i]->attrName] = -1;
		}
	}

	if ( NodeInformation["nodeStateA"] == -1 && NodeInformation["nodeStateB"] == -1){
		snprintf(strErr_no,1024,"prcstate %s - Both nodeStateA and nodeStateB are -1 ( impossible for acs_prcmand to retrieve the HA state )", PRCBIN_REVISION );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		immHandle.Finalize();
		return -1;
	}

	immHandle.Finalize();

	snprintf(strErr_no,1024,"prcstate %s - GetNodeStatus OK", PRCBIN_REVISION );
	Logging.Write(strErr_no, LOG_LEVEL_WARN);
	Logging.Close();

	return 0;
}

std::string upperCase(std::string input) {

	for (std::string::iterator it = input.begin(); it != input.end(); ++ it)
		*it = toupper(*it);

	return input;
}

void check_SI ( std::vector<string> myvector, std::vector<string>& ServiceInstance ){

	fstream inFile;
	inFile.open(refPath,ios::in);

	char buffer[256] = {0};
	std::vector<string> tmp;

	while ( inFile.good() ){

		inFile.getline(buffer,256);

		tmp.push_back ( buffer );

		memset(buffer, 0, sizeof(buffer));
	}

	inFile.close();

	for ( size_t i = 0; i < tmp.size(); i++){
		for ( size_t j = 0; j < myvector.size(); j++){

			if ( upperCase(tmp[i]).find(upperCase(myvector[j])) != std::string::npos ){
				ServiceInstance.push_back(tmp[i]);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	int localNodeState = NodeInformation["nodeStateA"] = -1;
	int remoteNodeState = NodeInformation["nodeStateB"] = -1;
	int runLevelLocal = NodeInformation["nodeRunLevelA"] = -1;
	int runLevelRemote = NodeInformation["nodeRunLevelB"] = -1;
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	char strErr_no[1024] = {0};

	bool ok = true, lock = false, unlock = false, prcboot = false;
	int large = 0;

	std::vector<string> myvector;
	std::vector<string> ServiceInstance;
	std::string command = "amf-adm ";


        is_swm_2_0_cmd = isSWM20();
	int i = 1;

	if ( argc > 1) {
		for ( i = 1; i < argc; i++){
			if ( strcmp ( argv[i], "-f") == 0 ){
				prcboot = true;
			}
			else if ( strcmp ( argv[i], "--lock") == 0 ) {
				command += "lock ";
				lock = true;
				break;
			}
			else if ( strcmp ( argv[i], "--unlock") == 0 ){
				command += "unlock ";
				unlock = true;
				break;
			}
		}
	}

	if ( lock || unlock ) {
		string SI;

		for ( int j = i+1; j < argc; j++ ){

			SI = argv[j];
			if (( strcmp ( argv[j], "CS" ) == 0 ) || ( strcmp ( argv[j], "CPTASPC" ) == 0 ) || ( strcmp ( argv[j], "2N" ) == 0 )) {
				SI += ",";
			}

			myvector.push_back(SI);
		}

		check_SI(myvector, ServiceInstance);

		for ( size_t size = 0; size < ServiceInstance.size(); size++){

			string real_command = command + ServiceInstance[size] + " 2>/dev/null";

			if ( lock )
				printf("Locking %s...",ServiceInstance[size].c_str());
			else
				printf("Unlocking %s...",ServiceInstance[size].c_str());

			int retcode = system(real_command.c_str());

			if ( !retcode )
				printf("done\n");
			else
				printf("failed ( exit code %d )\n",WEXITSTATUS(retcode));
		}

		if ( prcboot )
			system("prcboot -f");

		exit(0);
	}

	if (argc > 2)
	{
		ok = false;
	}

	if (argc == 2)
	{
		if (strcmp(argv[1],"-l") == 0)
		{
			large = 1;
		}
		else
		{
			ok = false;
		}
	}

	if (!ok)
	{
		cout << "Usage: prcstate [-l]" << endl << endl;
		exit(2);
	}

	string node_id_path = "/etc/cluster/nodes/this/id";

	ifstream ifs;
	char node_id[8] = {0};
	ifs.open(node_id_path.c_str());
	if ( ifs.good())
		ifs.getline(node_id, 8);

	ifs.close();

	ACS_PRC_API prc_api;

	int local_node_state = prc_api.askForNodeState();
	int remote_node_state = prc_api.askForNodeState(ACS_PRC_REMOTE_NODE);
	int state = -1;

	if ( local_node_state == ACS_PRC_NODE_ACTIVE )
		// If PRCSTATE is executed on the ACTIVE node then the GetNodeStatus have to retrieve the info about node state and
		// node runlevel from the LOCAL node
		state = GetNodeStatus( ACS_PRC_LOCAL_NODE );
	else
		// If PRCSTATE is executed on the PASSIVE node then the GetNodeStatus have to retrieve the info about node state and
		// node runlevel from the REMOTE node ( this one should be ACTIVE )
		state = GetNodeStatus( ACS_PRC_REMOTE_NODE );

	if ( state < 0 ){

		// If GetNodeState fails for any reasons, try to retrieve the info from the PASSIVE node
		snprintf(strErr_no,1024,"prcstate %s - GetNodeStatus on ACTIVE node return -1, trying to execute GetNodeStatus on PASSIVE node", PRCBIN_REVISION );
		Logging.Write(strErr_no, LOG_LEVEL_WARN);

		if ( local_node_state == ACS_PRC_NODE_ACTIVE )
			// If PRCSTATE is executed on the ACTIVE node then the GetNodeStatus have to retrieve the info about node state and
			// node runlevel from the REMOTE node ( this one should be PASSIVE )
			state = GetNodeStatus( ACS_PRC_REMOTE_NODE );
		else
			// If PRCSTATE is executed on the PASSIVE node then the GetNodeStatus have to retrieve the info about node state and
			// node runlevel from the local node
			state = GetNodeStatus( ACS_PRC_LOCAL_NODE );
	}

	if (state < 0){
		// If GetNodeState fails for the second time, this means that it's impossible to retrieve the node status and the node runlevel
		// from both the PRC object. In this case PRCSTATE tries to calculate the runlevel of both nodes by itself

		memset( strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"prcstate %s - GetNodeStatus fails two time, trying to execute GetRunLevelWithoutServer", PRCBIN_REVISION );
		Logging.Write(strErr_no, LOG_LEVEL_WARN);

		GetRunLevelWithoutServer( local_node_state, node_id );
	}

	if ( ( local_node_state != ACS_PRC_NODE_ACTIVE) && ( local_node_state != ACS_PRC_NODE_PASSIVE) &&
			( remote_node_state != ACS_PRC_NODE_ACTIVE) && ( remote_node_state != ACS_PRC_NODE_PASSIVE) ){

		// If both node are not ACTIVE or PASSIVE then PRCSTATE have to show "undefined" printout

		memset( strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"prcstate %s - Local Node State : %i - Remote Node State %i", PRCBIN_REVISION, local_node_state, remote_node_state );
		Logging.Write(strErr_no, LOG_LEVEL_WARN);

		if (large == 1){
			cout << "undefined (Unable to retrieve Node state)" << endl;
			cout << "undefined (Unable to retrieve Node state)" << endl << endl;
		}
		else
			cout << "undefined" << endl << endl;

		Logging.Close();

		return 0;
	}

	if ( strcmp(node_id,"1") == 0 ){
		localNodeState = NodeInformation["nodeStateA"];
		remoteNodeState = NodeInformation["nodeStateB"];
		runLevelLocal = NodeInformation["nodeRunLevelA"];
		runLevelRemote = NodeInformation["nodeRunLevelB"];
	}else {
		localNodeState = NodeInformation["nodeStateB"];
		remoteNodeState = NodeInformation["nodeStateA"];
		runLevelLocal = NodeInformation["nodeRunLevelB"];
		runLevelRemote = NodeInformation["nodeRunLevelA"];
	}

	if ( large )
	{
		if (runLevelLocal == 5  ||  runLevelRemote == 5)		//  a further evaluation must be
		{														//  done for the CBAs running
				evaluateRunLevelForCBAs (runLevelLocal, runLevelRemote);
		}

		/*FOR local node state*/
		printLocalNodeState(localNodeState, remoteNodeState, runLevelLocal);

		/*FOR remote node state*/
		printRemoteNodeState(localNodeState, remoteNodeState, runLevelRemote);

		ACE_LSOCK_Stream Stream;
		ACE_LSOCK_Connector Connector;
		ACE_UNIX_Addr addr ( pipeName );

		char buffer[16] = {0};
		int bur = 0, swu = 0, ins = 0; //camp = 0; //HX54219

		ACE_Time_Value val(1);

		if ( Connector.connect ( Stream, addr, &val) >= 0 ){
			if ( Stream.send_n ( "GET_BACKUP_RESTORE", 19 ) >= 0 ){
				if ( Stream.recv (buffer, 16) >= 0 ){

					if ( strncmp(buffer,"TRUE",4) == 0 ){
						bur = 1;
					}
					memset( buffer, 0, sizeof(buffer));
				}
			}
			else {
				Stream.close();
			}
			Stream.close();
		}

		val.sec(1);

		if ( Connector.connect ( Stream, addr, &val) >= 0 ){
			if ( Stream.send_n ( "GET_SOFTWARE_UPDATE", 20 ) >= 0 ){
				if ( Stream.recv (buffer, 16) >= 0 ){

					if ( strncmp(buffer,"TRUE",4) == 0 ){
						swu = 2;
					}
					//Changes start for HS53075
					else {
						swu = getSwUpgradeOnGoing();
						memset( strErr_no, 0, sizeof(strErr_no));
						snprintf(strErr_no,1024,"After getSwUpgradeOnGoing where swu=%d", swu );
						Logging.Write(strErr_no, LOG_LEVEL_WARN);
					}
					//Changes End for HS53075
					memset( buffer, 0, sizeof(buffer));
				}
			}
			else {
				Stream.close();
			}
			Stream.close();
		}
 
         ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;
         if(ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture) == ACS_CS_API_NS::Result_Success)
         {
             if (nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
             {
                 ins = getInstantiationStatus();
                 memset( strErr_no, 0, sizeof(strErr_no));
                 snprintf(strErr_no,1024,"After getInstantiationStatus where ins=%d", ins);
                 Logging.Write(strErr_no, LOG_LEVEL_WARN);
             }
          }

        if ( bur == 0 ) // No backup ongoing, check is a restore is ongoing
        	bur = getRestoreOnGoing();

        printActivities ( bur | swu | ins );

	}
	else // Short
	{
		printLocalNodeState (localNodeState, remoteNodeState);
		cout << endl;
	}
	cout << endl;

	Logging.Close();

	return 0;
} // End of main prcstate
