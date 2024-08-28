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
#include <unistd.h>
#include "acs_prc_api.h"
#include "acs_prc_api_impl.h"
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "ACS_PRC_ispapi.h"
#include <ace/UNIX_Addr.h>
#include "ace/SOCK_Connector.h"
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Connector.h>
#include "ACS_TRA_Logging.h"
#include <acs_apgcc_omhandler.h>
#include <boost/filesystem.hpp>

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif
static SaVersionT immVersion = { 'A', 2, 1 };
static SaVersionT clmVersion = { 'B', 4, 1 };
static char * errorArray[41] = {
	const_cast<char *>(ACS_APGCC_NO_ERR), 									//0
	const_cast<char *>(ACS_APGCC_ERR_UNKNOW),								//1
	const_cast<char *>(ACS_APGCC_ERR_IMM_LIBRARY),							//2
	const_cast<char *>(ACS_APGCC_ERR_IMM_VERSION),							//3
	const_cast<char *>(ACS_APGCC_ERR_IMM_INIT),								//4
	const_cast<char *>(ACS_APGCC_ERR_IMM_TIMEOUT),							//5
	const_cast<char *>(ACS_APGCC_ERR_IMM_TRY_AGAIN),						//6
	const_cast<char *>(ACS_APGCC_ERR_IMM_INVALID_PARAM),					//7
	const_cast<char *>(ACS_APGCC_ERR_IMM_NO_MEMORY),						//8
	const_cast<char *>(ACS_APFCC_ERR_IMM_BAD_HANDLE),						//9
	const_cast<char *>(ACS_APGCC_ERR_IMM_BUSY),								//10
	const_cast<char *>(ACS_APGCC_ERR_IMM_ACCESS),							//11
	const_cast<char *>(ACS_APGCC_ERR_IMM_NOT_EXIST),						//12
	const_cast<char *>(ACS_APGCC_ERR_IMM_NAME_TOO_LONG),					//13
	const_cast<char *>(ACS_APGCC_ERR_IMM_EXIST),							//14
	const_cast<char *>(ACS_APGCC_ERR_IMM_NO_SPACE),							//15
	const_cast<char *>(ACS_APGCC_ERR_IMM_INTERRUPT),						//16
	const_cast<char *>(ACS_APGCC_ERR_NAME_NOT_FOUND),						//17
	const_cast<char *>(ACS_APGCC_ERR_IMM_NO_RESOURCES),						//18
	const_cast<char *>(ACS_APGCC_ERR_IMM_NOT_SUPPORTED),					//19
	const_cast<char *>(ACS_APGCC_ERR_IMM_BAD_OPERATION),					//20
	const_cast<char *>(ACS_APGCC_ERR_IMM_FAILED_OPERATION),					//21
	const_cast<char *>(ACS_APGCC_ERR_IMM_MESSAGE_ERROR),					//22
	const_cast<char *>(ACS_APGCC_ERR_IMM_QUEUE_FULL),						//23
	const_cast<char *>(ACS_APGCC_ERR_IMM_QUEUE_NOT_AVAILABLE),				//24
	const_cast<char *>(ACS_APGCC_ERR_IMM_BAD_FLAGS),						//25
	const_cast<char *>(ACS_APGCC_ERR_IMM_TOO_BIG),							//26
	const_cast<char *>(ACS_APGCC_ERR_IMM_NO_SECTIONS),						//27
	const_cast<char *>(ACS_APGCC_ERR_IMM_NO_OP),							//28
	const_cast<char *>(ACS_APGCC_ERR_IMM_REPAIR_PENDING),					//29
	const_cast<char *>(ACS_APGCC_ERR_IMM_NO_BINDINGS),						//30
	const_cast<char *>(ACS_APGCC_ERR_IMM_UNAVAILABLE),						//31
	const_cast<char *>(ACS_APGCC_ERR_IMM_CAMPAIGN_ERR_DETECTED),			//32
	const_cast<char *>(ACS_APGCC_ERR_IMM_CAMPAIGN_PROC_FAILED),				//33
	const_cast<char *>(ACS_APGCC_ERR_IMM_CAMPAIGN_CANCELED),				//34
	const_cast<char *>(ACS_APGCC_ERR_IMM_CAMPAIGN_FAILED),					//35
	const_cast<char *>(ACS_APGCC_ERR_IMM_CAMPAIGN_SUSPENDED),				//36
	const_cast<char *>(ACS_APGCC_ERR_IMM_CAMPAIGN_SUSPENDING),				//37
	const_cast<char *>(ACS_APGCC_ERR_IMM_ACCESS_DENIED),					//38
	const_cast<char *>(ACS_APGCC_ERR_IMM_NOT_READY),						//39
	const_cast<char *>(ACS_APGCC_ERR_IMM_DEPLOYMENT),						//40
};

const char prcman_pipeName[] = "/var/run/ap/PrcManServer";
using namespace std;
bool acs_prc_apiImplementation::is_swm_2_0 = isSWM20();
string getProcName (){

	char tmp_pName[128] = {0};
	int proc_pid = getpid();

	snprintf(tmp_pName,sizeof(tmp_pName),"%d",proc_pid);
	string path = "/proc/";
	path.append(tmp_pName);
	path+= "/cmdline";

	ifstream ifs;
	char buffer[256] = {0};
	ifs.open(path.c_str());
	if ( ifs.good())
		ifs.getline(buffer, 256);

	ifs.close();

	return string(buffer);
}

int acs_prc_apiImplementation::getHaState ( SaNameT objectName ) {

	SaImmAttrValuesT_2 **attributes = 0;

	SaImmAttrValuesT_2 *attr = 0;

	SaImmAttrNameT attributeNames[2] = {const_cast<char *>("saAmfCSICompHAState"), 0};

	SaAisErrorT error;
	SaImmAccessorHandleT accessorHandle;

	char tmpobj [ SA_MAX_NAME_LENGTH ] = {0};

	error = saImmOmAccessorInitialize(immHandle, &accessorHandle);
	if (error != SA_AIS_OK) {
		setError( ACS_PRC_IMM_ERROR, "getHaState - saImmOmAccessorInitialize error = " + (string)errorArray[error]);
		return ACS_PRC_IMM_ERROR;
	}

	error = saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
	if (error != SA_AIS_OK)	{

		strncpy( tmpobj, (char*)objectName.value, objectName.length);

		setError( ACS_PRC_IMM_ERROR_SEARCH, "getHaState - saImmOmAccessorGet_2 ( " + (string)tmpobj + " ) error = " + errorArray[error]);

		error = saImmOmAccessorFinalize(accessorHandle);

		if (error != SA_AIS_OK) {
			setError( ACS_PRC_IMM_ERROR, "getHaState - saImmOmAccessorFinalize error = " + (string)errorArray[error]);
			return ACS_PRC_IMM_ERROR;
		}

		return ACS_PRC_IMM_ERROR_SEARCH;
	}

	attr = attributes[0];

	int temp = 0;

	if ( attr->attrValuesNumber > 0 )
		temp = *reinterpret_cast<SaInt32T *>((*attributes)->attrValues[0]);
	else {
		temp = ACS_PRC_IMM_ERROR;
		setError( ACS_PRC_IMM_ERROR, "getHaState - HA object exist but saAmfCSICompHAState is empty");
	}

	error = saImmOmAccessorFinalize(accessorHandle);
	if (error != SA_AIS_OK) {
		setError( ACS_PRC_IMM_ERROR, "getHaState - saImmOmAccessorFinalize error = " + (string)errorArray[error]);
		return ACS_PRC_IMM_ERROR;
	}

	return temp;
}

string acs_prc_apiImplementation::addStrings(const char* s1,const char* s2,const char* s3,string s4,const char* s5)
{
	string s = s1;
	s = s + s2;
	s = s + s3;
	s = s + s4;
	s = s + s5;
	return s;
} // End of addStrings

int acs_prc_apiImplementation::load_process_information (pid_t & process_id, char * process_name, size_t size) {
	FILE * proc_stat_handle = fopen("/proc/self/stat", "r");
	if (!proc_stat_handle) return -1;

	int pid = 0;
	char pname[1024];
	int errno_save = 0;
	int return_code = 0;

	errno = 0;

	if (fscanf(proc_stat_handle, "%d (%1023s", &pid, pname) >= 2) {
		process_id = pid;
		if (char * p = reinterpret_cast<char *>(memccpy(process_name, pname, ')', size)))
			*--p = 0;
		process_name[size - 1] = 0;
	} else {
		errno_save = errno;
		return_code = -2;
	}

	fclose(proc_stat_handle);

	return_code && (errno = errno_save);

	return return_code;
}

int acs_prc_apiImplementation::generalReboot (const char* initiator,
		reasonType reason,
		string /*message*/,bool reboot,
		bool /*thisNode*/,bool /*ignoreFCH*/,
		bool /*spawnProcess*/,bool /*log*/)
{
	//initiator = NULL;
	//	thisNode = false;
	//	ignoreFCH = false;
	//	spawnProcess = false;
	//	log = false;

	//return executeReboot(message,reboot,reason);
	return executeReboot(initiator,reboot,reason);
} // End of generalReboot

void catch_alarm(int sig_num) {
	printf("Operation timed out. Exiting...\n%d\n",sig_num);
	exit(1);
}

void stopService(const char service_name[],const char path[]){

	int status;

	// create child
	pid_t pid = fork();
	char srvc_path[50];
	sprintf(srvc_path,"%s%s",path, service_name);
	if (pid == 0)
	{
		signal(SIGALRM, catch_alarm);
		//arg cmd
		string command = "/opt/ap/apos/bin/servicemgmt/servicemgmt stop ";
		command.append ( service_name );
		command.append (" &>/dev/null");
		//char *service[] = { const_cast<char*>(service_name), const_cast<char*>("stop &>/dev/null"), (char *)0 };
		//set timeout
		alarm(30);
		// execute cmd
		//			if (execv(srvc_path, service) < 0)
		//				printf("exec error: %s!\n",strerror(errno));
		if ( execlp("sh","sh", "-c", command.c_str() , (char *) NULL) == -1 )
			printf("Error: execlp: %s!\n",strerror(errno));
		exit(1);
	} else if (pid < 0) printf(" Error: fork() failed. \n");

	waitpid(pid, &status, 0);

}

bool acs_prc_apiImplementation::executeReboot (string message,bool reboot, reasonType reason )
{
	Ispapi ISPLog;
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	char buff_host[16] = {0};

	const char reboot_file_eva[] = "/var/log/prc_reboot_eva";

	string clusterPath = "/cluster/etc/ap/acs/prc/conf/";
	string local_hostname_path = "/etc/cluster/nodes/this/hostname";
	string remote_hostname_path = "/etc/cluster/nodes/peer/hostname";
	string p_local_node_id;
	string remote_hostname;
	FILE * 	file_write;
	ifstream ifs;

	ifs.open(remote_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, remote_hostname);
	ifs.close();

	ifs.open(local_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, p_local_node_id);
	ifs.close();

	clusterPath.append(p_local_node_id);

	if( reason == manualRebootInitiated ){ //TR :HU88264 change
		file_write = fopen(clusterPath.c_str(), "w");
		fclose (file_write);
	}

	if ( reason == causedByEvent ){
		file_write = fopen(reboot_file_eva, "w");
		fclose (file_write);
	}

	ISPLog.open();

	gethostname(buff_host,sizeof(buff_host));
	string tmp = (string)buff_host;

	ISPLog.nodeStateEvent(tmp,passive,reason,message);

	ISPLog.close();

	string rebootcmd;

	if ( reboot ){
		rebootcmd = "/opt/coremw/bin/cmw-node-reboot ";
		rebootcmd.append(buff_host);
	}
	else
		rebootcmd = "/sbin/poweroff";

	int status;
	//create child
	pid_t pid = fork();
	if (pid == 0) {
		signal(SIGALRM, catch_alarm);
		//set timeout
		alarm(30);
		// execute cmd
		uid_t my_uid = getuid();

		setuid ( 0 );

		if ( execlp("sh","sh","-c", rebootcmd.c_str(), (char *) NULL ) == -1){
			printf("Error: execlp: %d \n", errno);
			Logging.Close();
			return false;
		}

		setuid( my_uid );
	}
	else
		if ( pid < 0 )
			printf("Fatal error fork() failed. %d \n",errno);

	waitpid(pid, &status, 0);

	if(WIFEXITED(status))  //TR :HV63606 change start
	{
		Logging.Write("acs_prc_apiImplementation::executeReboot - child process terminated normally",LOG_LEVEL_DEBUG);
		Logging.Close();
		return true;
	}
	else{
		Logging.Close();
		return false;
	} //TR :HV63606 change end

}

acs_prc_apiImplementation::acs_prc_apiImplementation(){
	strProcName = "";
	immHandle = 0;
	error = 0;
}

acs_prc_apiImplementation::~acs_prc_apiImplementation(){
} // End of ACS_PRC_API destructor

bool acs_prc_apiImplementation::isRebootPossible (reasonType reason){

	if ((reason == manualRebootInitiated) ||
			(reason == systemInitiated) ||
			(reason == nsfInitiated) ||
			(reason == resourceFailed) ||
			(reason == hwmResetInitiated) ||
			(reason == hwmBlkInitiated) ||
			(reason == hwmDblkInitiated))
	{
		return true;
	}

	if ((reason == causedByFailover) || (reason == functionChange) ||
			(reason == softFunctionChange) || (reason == causedByEvent) ||
			(reason == referenceFC))
	{
		return true;
	}

	if ((reason == causedByError) || (reason == userInitiated))
	{
		return true;
	}

	return false;
} // End of isRebootPossible

bool acs_prc_apiImplementation::rebootNode (reasonType reason,const char* userInfo, bool reboot)
{

	if (!acs_prc_apiImplementation::isRebootPossible(reason))
	{
		return false;
	}


	string pName = getProcName();
	string rebootCause = addStrings("ui=",userInfo,",proc=",pName,",");

	if ((reason == functionChange) || (reason == softFunctionChange) || (reason == referenceFC))
		return generalReboot(rebootCause.c_str(),reason,"REBOOT INITIATED!",reboot,true,true,true,true);

	if ((reason == manualRebootInitiated) ||
			(reason == systemInitiated) ||
			(reason == nsfInitiated) ||
			(reason == resourceFailed) ||
			(reason == hwmResetInitiated) ||
			(reason == hwmBlkInitiated) ||
			(reason == hwmDblkInitiated))
	{
		return generalReboot(rebootCause.c_str(),reason,"REBOOT INITIATED!",reboot,true,true,true,true);
	}
	return generalReboot(rebootCause.c_str(),reason,"REBOOT INITIATED!",reboot,true,false,true,true);
} // End of rebootNode

//changes start for TR HW49302

//========================================================================================
//	omHandler_init method
//========================================================================================

int acs_prc_apiImplementation::omHandler_finalize()
{
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	char strErr_no[1024] = {0};

	SaAisErrorT error;

	error = saImmOmFinalize(immHandle);

	if (error != SA_AIS_OK) {
		setError( ACS_PRC_IMM_ERROR, "saImmOmFinalize error");
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"acs_prc_apiImplementation::omHandler_finalize() - saImmOmFinalize error ( %i )", error );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}
	Logging.Close();
	return 0;

}


//========================================================================================
//      omHandler_ method
//========================================================================================

int acs_prc_apiImplementation::omHandler_init()
{
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	char strErr_no[1024] = {0};

	SaAisErrorT error;
	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK){
		setError( ACS_PRC_IMM_ERROR, const_cast<char *>("saImmOmInitialize error") );
		snprintf(strErr_no,sizeof(strErr_no),"acs_prc_apiImplementation::omHandler_init() - saImmOmInitialize error ( %i )", error );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	Logging.Close();

	return 0;
}

//========================================================================================
//      getSuState_new method
//========================================================================================

int acs_prc_apiImplementation::getSuState_new (vector<string>& mySuList, const char* SuName ){

	string node_id_path = "/etc/cluster/nodes/this/id";

	ifstream ifs;
	char node_id[8];
	ifs.open(node_id_path.c_str());
	if ( ifs.good())
		ifs.getline(node_id, 8);

	ifs.close();

	return this->getSuState_new( mySuList, SuName, node_id );
}

//========================================================================================
//      getSuState_new method
//========================================================================================

int acs_prc_apiImplementation::getSuState_new ( vector<string>& mySuList, const char* SuName, const char* SuSide ){

	int state = ACS_PRC_IMM_ERROR_SEARCH;

	string temp;
	SaNameT objectName;

	vector<string>::iterator itSu_local_temp;

	ACS_TRA_Logging Logging;
//	char strErr_no[1024] = {0};

	Logging.Open("PRC");

	for (itSu_local_temp = mySuList.begin(); itSu_local_temp < mySuList.end(); itSu_local_temp++ )
	{
		temp = itSu_local_temp->c_str();
		//cout << temp.c_str() <<endl;
		if ( ( temp.find ( SuName ) != std::string::npos ) && ( temp.find ( "safSu=" + string(SuSide) ) != std::string::npos ) ){
			//The SU exist, get HAState

			strncpy((char*)objectName.value ,temp.c_str(),SA_MAX_NAME_LENGTH);
			objectName.length = strlen((char *)objectName.value);
			state = getHaState(objectName);

			/*memset(strErr_no, 0, sizeof(strErr_no));
			  snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getSuState_temp() matching found and the SU is: %s - state: %d", PRCBIN_REVISION, temp.c_str(), state);
			  Logging.Write(strErr_no, LOG_LEVEL_TRACE);*/
			Logging.Close();

			return state;
		}

	}

	Logging.Close();
	return state;

}

//========================================================================================
//      getSUList_new method
//========================================================================================


bool acs_prc_apiImplementation::getSUList_new(const char* Node_new, vector<string>& SuList_new){
	

	ACE_UNUSED_ARG (Node_new);
	string temp;
	SaAisErrorT error;
	SaImmHandleT immHandle;
	SaImmSearchHandleT searchHandle;
	SaImmSearchParametersT_2 searchParam;
	SaNameT objectName;
	SaImmAttrValuesT_2 **attributes;
	SaNameT rootName = { 0, "" };
	SaImmScopeT scope = SA_IMM_SUBTREE;     /* default search scope */
	char classNameBuf[SA_MAX_NAME_LENGTH] = {0};
	const char *className = classNameBuf;


	// Get information about SU and Component
	strncpy(classNameBuf, "SaAmfCSIAssignment", SA_MAX_NAME_LENGTH);

	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, const_cast<char*>("saImmOmInitialize error") ); return false; }

	searchParam.searchOneAttr.attrName = const_cast<char *>("SaImmAttrClassName");
	searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
	searchParam.searchOneAttr.attrValue = &className;

	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope,
			SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
			&searchHandle);

	if (error != SA_AIS_OK)	{ setError( ACS_PRC_IMM_ERROR, "saImmOmSearchInitialize_2 error"); return false; }

	do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);

		if ( error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) { setError( ACS_PRC_IMM_ERROR_SEARCH, "saImmOmSearchNext_2 error"); return false; };

		temp = string((char*)(objectName.value));

		//if (( temp.find_first_of("safComp=APG_Comp_") != string::npos) && (error == SA_AIS_OK) )
		//	SuList.push_back(temp);
                if ( is_swm_2_0 == true ){
			if (( temp.find("safApp=ERIC-apg") != string::npos) && (error == SA_AIS_OK) && ( temp.find("safComp=apos.haagent") == string::npos ) )
				SuList_new.push_back(temp);
   		}
		else{
			if (( temp.find("safApp=ERIC-APG") != string::npos) && (error == SA_AIS_OK) && ( temp.find("safComp=Agent") == string::npos ) )
			SuList_new.push_back(temp);
		}	
	} while (error != SA_AIS_ERR_NOT_EXIST);

	//cout << "saImmOmSearchFinalize"<<endl;
	error = saImmOmSearchFinalize(searchHandle);

	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, "saImmOmSearchFinalize error"); saImmOmFinalize(immHandle); return false; }

	//cout << "saImmOmFinalize"<<endl;
	error = saImmOmFinalize(immHandle);

	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, "saImmOmFinalize error"); return false; }

	return true;
}

//changes End for TR HW49302

int acs_prc_apiImplementation::getSuState ( const char* SuName ){

	string node_id_path = "/etc/cluster/nodes/this/id";

	ifstream ifs;
	char node_id[8];
	ifs.open(node_id_path.c_str());
	if ( ifs.good())
		ifs.getline(node_id, 8);

	ifs.close();

	return this->getSuState( SuName, node_id );
}

int acs_prc_apiImplementation::getSuState ( const char* SuName, const char* SuSide ){

	vector<string>SuList;
	int state = ACS_PRC_IMM_ERROR_SEARCH;

	string temp;
	SaAisErrorT error;
	SaImmSearchHandleT searchHandle;
	SaImmSearchParametersT_2 searchParam;
	SaNameT objectName;
	SaImmAttrValuesT_2 **attributes;
	SaNameT rootName = { 0, "" };
	SaImmScopeT scope = SA_IMM_SUBTREE;     /* default search scope */
	char classNameBuf[SA_MAX_NAME_LENGTH] = {0};
	const char *className = classNameBuf;

	ACS_TRA_Logging Logging;
	char strErr_no[1024] = {0};

	Logging.Open("PRC");

	strncpy(classNameBuf, "SaAmfCSIAssignment", SA_MAX_NAME_LENGTH);

	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK){
		setError( ACS_PRC_IMM_ERROR, const_cast<char *>("saImmOmInitialize error") );
		snprintf(strErr_no,sizeof(strErr_no),"getSuState() - saImmOmInitialize error ( %i )", error );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	searchParam.searchOneAttr.attrName = const_cast<char *>("SaImmAttrClassName");
	searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
	searchParam.searchOneAttr.attrValue = &className;

	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope,
			SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
			&searchHandle);

	if (error != SA_AIS_OK)	{ 
		setError( ACS_PRC_IMM_ERROR, "saImmOmSearchInitialize_2 error"); 
		saImmOmFinalize(immHandle);
		snprintf(strErr_no,sizeof(strErr_no),"getSuState() - saImmOmSearchInitialize_2 error ( %i )", error );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();

		return ACS_PRC_IMM_ERROR; 
	}

	do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);

		if ( error != SA_AIS_OK ) {

			saImmOmSearchFinalize(searchHandle); //TR HU38623
			saImmOmFinalize(immHandle);
			snprintf(strErr_no,sizeof(strErr_no),"getSuState() - saImmOmSearchNext_2 error ( %i )", error );
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();

			if ( error == SA_AIS_ERR_NOT_EXIST ) {
				setError( ACS_PRC_IMM_ERROR_SEARCH, "saImmOmSearchNext_2 error");
				return ACS_PRC_IMM_ERROR_SEARCH;
			}
			else {
				setError( ACS_PRC_IMM_ERROR, "saImmOmSearchNext_2 error");
				return ACS_PRC_IMM_ERROR;
			}
		}

		temp = string((char*)(objectName.value));

		if ( ( temp.find ( SuName ) != std::string::npos ) && ( temp.find ( "safSu=" + string(SuSide) ) != std::string::npos ) ){
			//The SU exist, get HAState
			state = getHaState( objectName );
			error = SA_AIS_ERR_NOT_EXIST; // force to exit
		}

	} while (error != SA_AIS_ERR_NOT_EXIST);

	error = saImmOmSearchFinalize(searchHandle);

	if (error != SA_AIS_OK) { 
		setError( ACS_PRC_IMM_ERROR, "saImmOmSearchFinalize error");
		saImmOmFinalize(immHandle); // TR HU38623
		snprintf(strErr_no,sizeof(strErr_no),"getSuState() - saImmOmSearchFinalize error ( %i )", error );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR; 
	}

	// At this point the list of all SU with the Attribute SaAmfSIAssignment has
	// been stored in the vector SuList

	error = saImmOmFinalize(immHandle);

	if (error != SA_AIS_OK) {
		setError( ACS_PRC_IMM_ERROR, "saImmOmFinalize error");
		snprintf(strErr_no,sizeof(strErr_no),"getSuState() - saImmOmFinalize error ( %i )", error );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	Logging.Close();

	return state;

}

time_t acs_prc_apiImplementation::lastRebootTime(){

	SaClmHandleT clmHandle;
	SaAisErrorT error;
	SaClmClusterNodeT_4 clusterNode;
	SaClmNodeIdT nodeId;
	SaTimeT timeout; /* timeout value for synchronous invocations */

	//cout << "saClmInitialize"<<endl;
	error = saClmInitialize_4(&clmHandle, NULL, &clmVersion);

	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, "saClmInitialize_4 error"); return ACS_PRC_IMM_ERROR; }

	timeout = 100 * SA_TIME_ONE_MILLISECOND; /* 100 milliseconds */
	nodeId = SA_CLM_LOCAL_NODE_ID;

	//cout << "saClmClusterNodeGet"<<endl;
	error = saClmClusterNodeGet_4(clmHandle, nodeId, timeout,	&clusterNode);
	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, "saClmClusterNodeGet_4 error"); return ACS_PRC_IMM_ERROR; }

	time_t temp = clusterNode.bootTimestamp / SA_TIME_ONE_SECOND;

	//cout << "saClmFinalize"<<endl;
	error = saClmFinalize(clmHandle);
	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, "saClmFinalize error"); return ACS_PRC_IMM_ERROR; }

	return temp;
}

bool acs_prc_apiImplementation::getSUList(const char* Node, vector<string>& SuList){
	

	ACE_UNUSED_ARG (Node);
	string temp;
	SaAisErrorT error;
	SaImmHandleT immHandle;
	SaImmSearchHandleT searchHandle;
	SaImmSearchParametersT_2 searchParam;
	SaNameT objectName;
	SaImmAttrValuesT_2 **attributes;
	SaNameT rootName = { 0, "" };
	SaImmScopeT scope = SA_IMM_SUBTREE;     /* default search scope */
	char classNameBuf[SA_MAX_NAME_LENGTH] = {0};
	const char *className = classNameBuf;


	// Get information about SU and Component
	strncpy(classNameBuf, "SaAmfComp", SA_MAX_NAME_LENGTH);

	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, const_cast<char*>("saImmOmInitialize error") ); return false; }

	searchParam.searchOneAttr.attrName = const_cast<char *>("SaImmAttrClassName");
	searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
	searchParam.searchOneAttr.attrValue = &className;

	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope,
			SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
			&searchHandle);

	if (error != SA_AIS_OK)	{ setError( ACS_PRC_IMM_ERROR, "saImmOmSearchInitialize_2 error"); return false; }

	do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);

		if ( error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) { setError( ACS_PRC_IMM_ERROR_SEARCH, "saImmOmSearchNext_2 error"); return false; };

		temp = string((char*)(objectName.value));

		//if (( temp.find_first_of("safComp=APG_Comp_") != string::npos) && (error == SA_AIS_OK) )
		//	SuList.push_back(temp);	
		if ( is_swm_2_0 == true ){
			if (( temp.find("safApp=ERIC-apg") != string::npos) && (error == SA_AIS_OK) && ( temp.find("safComp=apos.haagent") == string::npos ) )
				SuList.push_back(temp);
		}
		else{
			if (( temp.find("safApp=ERIC-APG") != string::npos) && (error == SA_AIS_OK) && ( temp.find("safComp=Agent") == string::npos ) )
				SuList.push_back(temp);
		}

	} while (error != SA_AIS_ERR_NOT_EXIST);

	//cout << "saImmOmSearchFinalize"<<endl;
	error = saImmOmSearchFinalize(searchHandle);

	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, "saImmOmSearchFinalize error"); return false; }

	//cout << "saImmOmFinalize"<<endl;
	error = saImmOmFinalize(immHandle);

	if (error != SA_AIS_OK) { setError( ACS_PRC_IMM_ERROR, "saImmOmFinalize error"); return false; }

	return true;
}

void acs_prc_apiImplementation::setError(int err, string errText)
{
	error = err;
	errorText = errText;
}

int acs_prc_apiImplementation::getLastError(void) const
{
	return error;
}

const char* acs_prc_apiImplementation::getLastErrorText(void) const
{
	//return Api_Implementation->getLastErrorText();

	return errorText.c_str();
}

int acs_prc_apiImplementation::askForNodeState(){
	
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	const char local_node_id_path[] = "/etc/cluster/nodes/this/id";
	const char remote_node_id_path[] = "/etc/cluster/nodes/peer/id";

	bool peer_node_available = true;

	char local_node_id[8] = {0};
	char remote_node_id[8] = {0};
	char strErr_no[1024] = {0};


	FILE* local_node_id_fd = fopen(local_node_id_path, "r");

	if ( local_node_id_fd != NULL ){
		if ( fgets ( local_node_id, sizeof(local_node_id), local_node_id_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_id_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			fclose( local_node_id_fd );
			return ACS_PRC_IMM_ERROR;
		}
		else {
			local_node_id[strlen(local_node_id) - 1] = 0; // Remove a possible \n
		}
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_id_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	fclose( local_node_id_fd );

	memset(strErr_no, 0, sizeof(strErr_no));

	FILE* remote_node_id_fd = fopen(remote_node_id_path, "r");

	if ( remote_node_id_fd != NULL ){
		if ( fgets ( remote_node_id, sizeof(remote_node_id), remote_node_id_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			//Logging.Close();
			//fclose( remote_node_id_fd );
			//return ACS_PRC_IMM_ERROR;
		}
		else {
			remote_node_id[strlen(remote_node_id) - 1] = 0; // Remove a possible \n
		}

		fclose( remote_node_id_fd );
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		//Logging.Close();
		//return ACS_PRC_IMM_ERROR;
	}

	memset(strErr_no, 0, sizeof(strErr_no));

	SaNameT objectNode;
	SaAisErrorT error;
	string tmp;

	if ( is_swm_2_0 == true)
	{
		tmp = "safCSIComp=safComp=apos.haagent\\,safSu=SC-";
		tmp.append(local_node_id);
		tmp.append("\\,safSg=2N\\,safApp=ERIC-apg.nbi.aggregation.service,safCsi=apos.haagent,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service");
	}
	else{
		tmp = "safCSIComp=safComp=Agent\\,safSu=";
		tmp.append(local_node_id);
		tmp.append("\\,safSg=2N\\,safApp=ERIC-APG,safCsi=AGENT,safSi=AGENT,safApp=ERIC-APG");
	}
	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK) {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - saImmOmInitialize error = %s", errorArray[error]);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	strncpy((char*)objectNode.value ,tmp.c_str(),SA_MAX_NAME_LENGTH);

	objectNode.length = strlen((char *)objectNode.value);

	int temp = getHaState(objectNode);

	if (( temp < 0 ) && ( peer_node_available == true )){

		Logging.Write(getLastErrorText(),LOG_LEVEL_ERROR);
		Logging.Write("askForNodeState() - Try to get Node State from the other Node...",LOG_LEVEL_ERROR);
 		if ( is_swm_2_0 == true ){
			tmp = "safCSIComp=safComp=apos.haagent\\,safSu=SC-";
			tmp.append(remote_node_id);
			tmp.append("\\,safSg=2N\\,safApp=ERIC-apg.nbi.aggregation.service,safCsi=apos.haagent,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service");
		}
		else{
			tmp = "safCSIComp=safComp=Agent\\,safSu=";
			tmp.append(remote_node_id);
			tmp.append("\\,safSg=2N\\,safApp=ERIC-APG,safCsi=AGENT,safSi=AGENT,safApp=ERIC-APG");
		}	
                strncpy((char*)objectNode.value ,tmp.c_str(),SA_MAX_NAME_LENGTH);
		objectNode.length = strlen((char *)objectNode.value);

		temp = getHaState(objectNode);

		if ( temp == 1 ){
			temp = 2;
			Logging.Write("askForNodeState() - Success to retrieve the Node state from the other Node",LOG_LEVEL_ERROR);
		}
		else{
			temp = ACS_PRC_IMM_ERROR;
			Logging.Write(getLastErrorText(),LOG_LEVEL_ERROR);
		}
	}

	error = saImmOmFinalize(immHandle);

	memset(strErr_no, 0, sizeof(strErr_no));

	if (error != SA_AIS_OK) {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - saImmOmFinalize error = %s", errorArray[error]);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	Logging.Close();
	return temp;
}

int acs_prc_apiImplementation::askForNodeState( int Node ){


	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	SaNameT objectNode;
	SaAisErrorT error;
	string tmp, strErrorToLog;
	const char local_node_id_path[] = "/etc/cluster/nodes/this/id";
	const char remote_node_id_path[] = "/etc/cluster/nodes/peer/id";

	char local_node_id[8] = {0};
	char remote_node_id[8] = {0};
	char strErr_no[1024] = {0};

	FILE* local_node_id_fd = fopen(local_node_id_path, "r");

	if ( local_node_id_fd != NULL ){
		if ( fgets ( local_node_id, sizeof(local_node_id), local_node_id_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_id_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			fclose( local_node_id_fd );
			return ACS_PRC_IMM_ERROR;
		}
		else {
			local_node_id[strlen(local_node_id) - 1] = 0; // Remove a possible \n
		}
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_id_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	fclose( local_node_id_fd );

	memset(strErr_no, 0, sizeof(strErr_no));

	FILE* remote_node_id_fd = fopen(remote_node_id_path, "r");

	if ( remote_node_id_fd != NULL ){
		if ( fgets ( remote_node_id, sizeof(remote_node_id), remote_node_id_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			fclose( remote_node_id_fd );
			return ACS_PRC_IMM_ERROR;
		}
		else {
			remote_node_id[strlen(remote_node_id) - 1] = 0; // Remove a possible \n
		}
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	fclose( remote_node_id_fd );

	memset(strErr_no, 0, sizeof(strErr_no));
        if ( is_swm_2_0 == true )
		tmp = "safCSIComp=safComp=apos.haagent\\,safSu=SC-";
	else
		tmp = "safCSIComp=safComp=Agent\\,safSu=";
	
	if ( Node == ACS_PRC_REMOTE_NODE ) {
		tmp.append ( remote_node_id );
	}
	else if ( Node == ACS_PRC_LOCAL_NODE ) {
		tmp.append ( local_node_id );
	}
	else {
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%i) - wrong parameter, must be 0 ( ACS_PRC_LOCAL_NODE ) or 1 ( ACS_PRC_REMOTE_NODE )", Node );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}
	
	if ( is_swm_2_0 == true )
		tmp.append("\\,safSg=2N\\,safApp=ERIC-apg.nbi.aggregation.service,safCsi=apos.haagent,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service");
	else
		tmp.append("\\,safSg=2N\\,safApp=ERIC-APG,safCsi=AGENT,safSi=AGENT,safApp=ERIC-APG");
	strncpy((char*)objectNode.value ,tmp.c_str(),SA_MAX_NAME_LENGTH);

	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK) {
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%i) - saImmOmInitialize error = %s", Node, errorArray[error]);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	objectNode.length = strlen((char *)objectNode.value);

	int temp = getHaState(objectNode);

	if ( temp < 0 ){
		Logging.Write(getLastErrorText(),LOG_LEVEL_ERROR);

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%i) - Try to get Node State from the other Node...", Node );
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		
		if( is_swm_2_0 == true )
			tmp = "safCSIComp=safComp=apos.haagent\\,safSu=SC-";
		else
			tmp = "safCSIComp=safComp=Agent\\,safSu=";
		
                if ( Node == ACS_PRC_REMOTE_NODE )
			tmp.append(local_node_id);
		else
			tmp.append(remote_node_id);
		
		if ( is_swm_2_0 == true )
			tmp.append("\\,safSg=2N\\,safApp=ERIC-apg.nbi.aggregation.service,safCsi=apos.haagent,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service");
		
		else
			tmp.append("\\,safSg=2N\\,safApp=ERIC-APG,safCsi=AGENT,safSi=AGENT,safApp=ERIC-APG");
		
		strncpy((char*)objectNode.value ,tmp.c_str(),SA_MAX_NAME_LENGTH);
		objectNode.length = strlen((char *)objectNode.value);

		temp = getHaState(objectNode);

		memset(strErr_no, 0, sizeof(strErr_no));
		if ( temp == 1 ){
			temp = 2;
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%i) - Success to retrieve the Node state from the other Node", Node );
		}
		else{
			temp = ACS_PRC_IMM_ERROR;
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%i) - %s", Node, getLastErrorText());
		}
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
	}

	error = saImmOmFinalize(immHandle);

	if (error != SA_AIS_OK) {
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%i) - saImmOmInitialize error = %s", Node, errorArray[error]);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	Logging.Close();
	return temp;
}

int acs_prc_apiImplementation::askForNodeState( string Node ){


	SaNameT objectNode;
	SaAisErrorT error;
	string tmp;

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	const char local_node_id_path[] = "/etc/cluster/nodes/this/id";
	const char remote_node_id_path[] = "/etc/cluster/nodes/peer/id";
	const char local_node_hostname_path[] = "/etc/cluster/nodes/this/hostname";
	const char remote_node_hostname_path[] = "/etc/cluster/nodes/peer/hostname";

	char strErr_no[1024] = {0};
	char local_node_id[8] = {0};
	char remote_node_id[8] = {0};
	char local_node_hostname[32] = {0};
	char remote_node_hostname[32] = {0};

	FILE* local_node_hostname_fd = fopen(local_node_hostname_path, "r");

	if ( local_node_hostname_fd != NULL ){
		if ( fgets ( local_node_hostname, sizeof(local_node_hostname), local_node_hostname_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_hostname_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			fclose( local_node_hostname_fd );
			return ACS_PRC_IMM_ERROR;
		}
		else {
			local_node_hostname[strlen(local_node_hostname) - 1] = 0; // Remove a possible \n
		}
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_hostname_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	fclose( local_node_hostname_fd );

	memset(strErr_no, 0, sizeof(strErr_no));

	FILE* remote_node_hostname_fd = fopen(remote_node_hostname_path, "r");

	if ( remote_node_hostname_fd != NULL ){
		if ( fgets ( remote_node_hostname, sizeof(remote_node_hostname), remote_node_hostname_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			fclose( remote_node_hostname_fd );
			return ACS_PRC_IMM_ERROR;
		}
		else {
			remote_node_hostname[strlen(remote_node_hostname) - 1] = 0; // Remove a possible \n
		}
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	fclose( remote_node_hostname_fd );

	memset(strErr_no, 0, sizeof(strErr_no));

	FILE* local_node_id_fd = fopen(local_node_id_path, "r");

	if ( local_node_id_fd != NULL ){
		if ( fgets ( local_node_id, sizeof(local_node_id), local_node_id_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_id_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			fclose( local_node_id_fd );
			return ACS_PRC_IMM_ERROR;
		}
		else {
			local_node_id[strlen(local_node_id) - 1] = 0; // Remove a possible \n
		}
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", local_node_id_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	fclose( local_node_id_fd );

	memset(strErr_no, 0, sizeof(strErr_no));

	FILE* remote_node_id_fd = fopen(remote_node_id_path, "r");

	if ( remote_node_id_fd != NULL ){
		if ( fgets ( remote_node_id, sizeof(remote_node_id), remote_node_id_fd ) == NULL ){
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to read file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			fclose( remote_node_id_fd );
			return ACS_PRC_IMM_ERROR;
		}
		else {
			remote_node_id[strlen(remote_node_id) - 1] = 0; // Remove a possible \n
		}
	}
	else {
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState() - Failed to open file ( %s ) - errno = %s ( %i ) - Return ACS_PRC_IMM_ERROR ( -1 )", remote_node_id_path, strerror(errno), errno);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	fclose( remote_node_id_fd );

	memset(strErr_no, 0, sizeof(strErr_no));

	if ( is_swm_2_0 == true )
		tmp = "safCSIComp=safComp=apos.haagent\\,safSu=SC-";
	else
		tmp = "safCSIComp=safComp=Agent\\,safSu=";	
	
	if ( Node == local_node_hostname ) {
		tmp.append ( local_node_id );
	}else{
		if ( Node == remote_node_hostname ) {
			tmp.append ( remote_node_id );
		}
		else {
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%s) - Node not exist", Node.c_str() );
			Logging.Write(strErr_no,LOG_LEVEL_ERROR);
			Logging.Close();
			return ACS_PRC_IMM_ERROR;
		}
	}
	
	if ( is_swm_2_0 == true )
		tmp.append("\\,safSg=2N\\,safApp=ERIC-apg.nbi.aggregation.service,safCsi=apos.haagent,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service");
	else
		tmp.append("\\,safSg=2N\\,safApp=ERIC-APG,safCsi=AGENT,safSi=AGENT,safApp=ERIC-APG");
		
	error = saImmOmInitialize(&immHandle, NULL, &immVersion);

	if (error != SA_AIS_OK) {
		memset( strErr_no,0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%s) - saImmOmInitialize error = %s", Node.c_str(), errorArray[error]);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	strncpy((char*)objectNode.value ,tmp.c_str(),SA_MAX_NAME_LENGTH);

	objectNode.length = strlen((char *)objectNode.value);

	int temp = getHaState(objectNode);

	if ( temp < 0 ){

		Logging.Write(getLastErrorText(),LOG_LEVEL_ERROR);
		Logging.Write("askForNodeState(string) - Try to get Node State from the other Node...", LOG_LEVEL_ERROR);
                
       		if ( is_swm_2_0 == true )
			tmp = "safCSIComp=safComp=apos.haagent\\,safSu=SC-";
		else
			tmp = "safCSIComp=safComp=Agent\\,safSu=";
		
		if ( Node == local_node_hostname ) {
			tmp.append ( remote_node_id );
		}else{
			if ( Node == remote_node_hostname ) {
				tmp.append ( remote_node_id );
			}
			else {
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%s) - Node not exist", Node.c_str() );
				Logging.Write(strErr_no,LOG_LEVEL_ERROR);
				Logging.Close();
				return ACS_PRC_IMM_ERROR;
			}
		}
		if ( is_swm_2_0 == true)
			tmp.append("\\,safSg=2N\\,safApp=ERIC-apg.nbi.aggregation.service,safCsi=apos.haagent,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service");
		else
			tmp.append("\\,safSg=2N\\,safApp=ERIC-APG,safCsi=AGENT,safSi=AGENT,safApp=ERIC-APG");
		
		strncpy((char*)objectNode.value ,tmp.c_str(),SA_MAX_NAME_LENGTH);
		objectNode.length = strlen((char *)objectNode.value);

		temp = getHaState(objectNode);

		memset(strErr_no, 0, sizeof(strErr_no));
		if ( temp == 1 ){
			temp = 2;
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%s) - Success to retrieve the Node state from the other Node", Node.c_str() );
		}
		else{
			temp = ACS_PRC_IMM_ERROR;
			snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%s) - %s", Node.c_str(), getLastErrorText());
		}
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
	}

	error = saImmOmFinalize(immHandle);

	if (error != SA_AIS_OK) {
		memset( strErr_no,0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"askForNodeState(%s) - saImmOmInitialize error = %s", Node.c_str(), errorArray[error]);
		Logging.Write(strErr_no,LOG_LEVEL_ERROR);
		Logging.Close();
		return ACS_PRC_IMM_ERROR;
	}

	Logging.Close();

	return temp;
}
bool isSWM20()
{

 ACS_TRA_Logging Logging;
 Logging.Open("PRC");
 char strErr_no[1024] = {0};

    if((boost::filesystem::exists(boost::filesystem::path(SWMVERSION)))== true)
    {
        snprintf(strErr_no,1024,"isSWM20(): swm_version file found ");
	Logging.Write(strErr_no,LOG_LEVEL_ERROR);
        Logging.Close();
	return true;
    }	
    else {
        snprintf(strErr_no,1024,"isSWM20(): swm_version file not found ");
	Logging.Write(strErr_no,LOG_LEVEL_ERROR);
        Logging.Close();
	return false;	
    }

}
