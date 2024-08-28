/*
 *
 * NAME: acs_dsd_acease.cpp
 *
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 * All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Telecom AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 *  This is the main program for the acease command.
 *
 * DOCUMENT NO
 *	-
 *
 * AUTHOR
 * 	2011-12-02 TEI/XSD  XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	       	20111202	XGIOPAP		First Revision
 *
 */

// Module Include Files
#include "acs_alh_util.h"
#include "acs_alh_common.h"
#include "acs_alh_acease_signal_handler.h"
#include "acs_apgcc_omhandler.h"
#include <ACS_APGCC_Util.H>
#include "ACS_APGCC_CommonLib.h"
#include "ace/Signal.h"
#include "ace/Sig_Handler.h"
#include <acs_aeh_evreport.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <ctype.h>
#include <sys/types.h>
#include <grp.h>
#include <list>

#define INCONSISTENCY_ALARM_ID 24154

#define INCONSISTENCY_ALARM_FILE_NAME "/data/opt/ap/internal_root/cp/printouts/adh/alarm/alarmInfo"/*HY16787*/

using namespace std;

// Global object
acs_alh_util util("ACEASE");  //Trace object
static int siglist[] = {SIGHUP, SIGQUIT, SIGINT, SIGTERM};      		// list of the signals that we must catch and handle.
OmHandler om_handler_;
bool omHandlerInitialized_;
char dialoguePrompt[] = "\x03:";
bool passive(false);
//ENM
bool customRoleUser = false , systemadminstrator=false;  // global declarataion
// TR HR45341
acs_alh_acease_signal_handler acease_sighandler;

//========================================================================================
//	Print acease usage
//========================================================================================
void usage()
{
	cout << "Usage: acease [-p] specificProblem:indexNumber" << endl << endl;
}

//========================================================================================
//	Convert a string to an integer value
//========================================================================================
int strToInt(const string str)
{
	//Call trace function
	util.trace("ACEASE strToInt method enter");

	if (str.empty())
	{
		cout << "Parameter error: Numeral expected." << endl << endl;
		exit(16);
	}

	char* p;
	errno = 0;
	int var = strtol(str.data(), &p, 10);

	if (*p != ' ' && *p != '\t' && *p != 0)
	{
		cout << "Parameter error: Illegal character." << endl << endl;
		exit(16);
	}

	if (errno == ERANGE)
	{
		cout << "Parameter error: Overflow." << endl << endl;
		exit(16);
	}

	//Call trace function
	util.trace("ACEASE strToInt method Returning var: %i", var);

	return var;
}

//========================================================================================
//  Get the local node Name
//========================================================================================
std::string getlocalNodeName()
{
	//Call trace function
	util.trace("ACEASE getlocalNodeName() enter");

	std::string local_hostname_path = "/etc/cluster/nodes/this/hostname";
	std::string p_local_node_id("");
	ifstream ifs;

	ifs.open(local_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, p_local_node_id);

	ifs.close();

	return p_local_node_id;
}

//========================================================================================
//  Get the remote node Name
//========================================================================================
std::string getremoteNodeName()
{
	//Call trace function
	util.trace("ACEASE getremoteNodeName() enter");

	std::string remote_hostname_path = "/etc/cluster/nodes/peer/hostname";
	std::string p_remote_node_id("");
	ifstream ifs;

	ifs.open(remote_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, p_remote_node_id);

	ifs.close();

	return p_remote_node_id;
}

//========================================================================================
//  get_alh_imm_root_dn method
//========================================================================================
int get_alh_imm_root_dn(const char * & dn_path)
{
	static int info_already_load = 0;
	static char alh_imm_root_dn[512] = {0};

	//Call trace function
	util.trace("ACEASE get_alh_imm_root_dn() enter");

	if (info_already_load) { dn_path = alh_imm_root_dn; return 0; }

	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS)
		return acs_alh::ERR_SAF_IMM_OM_INIT;

	std::vector<std::string> dn_list;
	int return_code = 0;

	if ((imm_call_result = om_handler.getClassInstances(ACS_ALH_CONFIG_IMM_ALH_CLASS_NAME, dn_list)) ^ ACS_CC_SUCCESS) { //ERROR
		return_code = acs_alh::ERR_SAF_IMM_OM_GET;
		util.trace("ERROR: call 'om_handler.getClassInstances(...' failed: retrieving ALH parameters root from IMM: IMM error code == %d",
				om_handler.getInternalLastError());
	} else { //OK: Checking how much dn items was found in IMM
		if (dn_list.size() ^ 1) { //WARNING: 0 (zero) or more than one node found
			return_code = acs_alh::ERR_SAF_IMM_OM_GET;
			util.trace("WARNING: calling 'om_handler.getClassInstances(...': %s",
					dn_list.size() ? "more than one ALH parameters root node was found in IMM" : "ALH parameters root node not found in IMM");
		} else { //OK: Only one root node
			strncpy(alh_imm_root_dn, dn_list[0].c_str(), ACS_ALH_ARRAY_SIZE(alh_imm_root_dn));
			alh_imm_root_dn[ACS_ALH_ARRAY_SIZE(alh_imm_root_dn) - 1] = 0;
			info_already_load = 1;
			dn_path = alh_imm_root_dn;
		}
	}

	om_handler.Finalize();

	//Call trace function
	util.trace("ACEASE get_alh_imm_root_dn() Returning code: %i", return_code);

	return return_code;
}


//========================================================================================
//	omHandler_init method
//========================================================================================
int omHandler_init()
{
	ACS_CC_ReturnType imm_result;

	//Call trace function
	util.trace("ACEASE omHandler_init() enter");

	if ((imm_result = om_handler_.Init()) != ACS_CC_SUCCESS)
	{
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in acs_alh_imm_data_handler: OmHandler::Init() failed: return code == %d", imm_result);
		//Call trace function
		util.trace("ACEASE ERROR - %s", problemData);

		return -1;
	}

	omHandlerInitialized_ = true;

	return 0;
}

//========================================================================================
//	omHandler_finalize method
//========================================================================================
int omHandler_finalize()
{
	ACS_CC_ReturnType imm_result;

	//Call trace function
	util.trace("ACEASE omHandler_Finalize() enter");

	if ((imm_result = om_handler_.Finalize()) != ACS_CC_SUCCESS)
	{
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in acs_alh_imm_data_handler: OmHandler::Finalize() failed: return code == %d", imm_result);
		//Call trace function
		util.trace("ACEASE ERROR - %s", problemData);

		return -1;
	}

	omHandlerInitialized_ = false;

	return 0;
}

bool isRole(std ::string output, std::string Roleclass){
	OmHandler immHandler;
	bool result = false;
	if (immHandler.Init() != ACS_CC_SUCCESS) {
		util.trace("IMM Initialization failure");
		return result;
	}

	//keeping string output(containg roles) in to vector res
	char delim =',';
	std::size_t current, previous = 0;
	std::vector<std::string> res;
	current = output.find(delim);
	while (current != std::string::npos) {
		res.push_back(output.substr(previous, current - previous));
		previous = current + 1;
		current = output.find(delim, previous);
	}
	res.push_back(output.substr(previous, current - previous));

	std::vector<std::string> dnListRole,listRole;
	immHandler.getClassInstances(Roleclass.c_str(), dnListRole);
	//getting custom or system roles in to vector
	size_t equPos,commaPos;
	string RoleId;
	for (size_t i=0; i< dnListRole.size(); i++)
	{
		equPos = dnListRole[i].find_first_of("=");
		commaPos = dnListRole[i].find_first_of(",");
		RoleId = dnListRole[i].substr((equPos+1),(commaPos-equPos-1));
		listRole.push_back(RoleId);
	}//for
	//Comparing vector res(containg roles of user) with vector list_customRole(containg custon roles or system roles based on role class)
	for(size_t i=0; i<res.size(); i++){
		string temp1 = res[i];
		for(size_t j=0;j<listRole.size();j++)
		{
			std::string cRole =listRole[j];
			if(temp1.compare(cRole.c_str())== 0)
			{
				result=true;
				break;
			}
		}
	}//for
	if (immHandler.Finalize() != ACS_CC_SUCCESS)
	{
		util.trace("IMM finalization failure");
	}
	return result;
}
//========================================================================================
//	getAlarmInToList method
//========================================================================================
int getAlarmInToList(std::vector<AllRecord>* alarmList, int cpSide, string local_remote_node)
{

	int ret_code = 0;
	int call_result = 0;
	const char *imm_alh_root_dn = 0;

	//Call trace function
	util.trace("ACEASE getAlarmInToList() enter");

	/*Searching the parent name of the class to be defined*/
	call_result = get_alh_imm_root_dn(imm_alh_root_dn);
	if (call_result)
	{
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d", call_result);
		//Call trace function
		util.trace("ACEASE getAlarmInToList() ERROR - %s", problemData);
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}


	if(!omHandlerInitialized_ && (ret_code = omHandler_init()) != acs_alh::ERR_NO_ERRORS){
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList() - omHandler_init() failed: return code == %d", ret_code);
		//Call trace function
		util.trace("ACEASE getAlarmInToList() ERROR - %s", problemData);
		return acs_alh::ERR_SAF_IMM_OM_INIT;
	}

	alarmList->clear();

	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };
	/*The DN name of the parent of object to be defined*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1, ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN"=%s,%s", local_remote_node.c_str(), imm_alh_root_dn);

	std::vector<std::string> alarmInfo_name_list;

	// try to fetch the DN of AlarmInfo objects from IMM
	if ((ret_code = om_handler_.getChildren(imm_dnName, ACS_APGCC_SUBLEVEL, & alarmInfo_name_list)) != ACS_CC_SUCCESS)
	{
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...): om_handler.getChildren' failed: return code == %d", ret_code);
		//Call trace function
		util.trace("ACEASE getAlarmInToList() ERROR - %s", problemData);
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	// try to fetch the AlarmInfo objects from IMM
	for(int i=0; i<(int)alarmInfo_name_list.size(); i++)
	{
		ACS_APGCC_ImmObject imm_object;
		imm_object.objName =  alarmInfo_name_list[i].c_str();
		AllRecord alarm;

		if ((ret_code = om_handler_.getObject(&imm_object)) == ACS_CC_SUCCESS)
		{
			unsigned attributes_num = imm_object.attributes.size();

			for(unsigned j=0; j<attributes_num; j++)
			{
				const char *attr_name = imm_object.attributes[j].attrName.c_str();

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_NAME_RDN))
				{
					char identityString[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = {0};

					alarm.identity[0] = alarm.identity[1] = alarm.identity[2] = 0;

					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						strncpy(identityString, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX);

						alarm.identity[0] = atoi(strtok(identityString, ":"));
						alarm.identity[1] = 0;
						alarm.identity[2] = atoi(strtok(0, ":"));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROCESS_NAME))
				{
					memset ( alarm.event.processName, 0, ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.processName, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX);

					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBL_CAUSE))
				{
					memset ( alarm.event.probableCause, 0, ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.probableCause, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX);

					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CATEGORY))
				{
					memset ( alarm.event.objClassOfReference, 0, ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.objClassOfReference, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_OBJ_REF))
				{
					memset ( alarm.event.objectOfReference, 0, ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.objectOfReference, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_DATA))
				{
					memset ( alarm.event.problemData, 0, ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.problemData, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_TEXT))
				{
					memset ( alarm.event.problemText, 0, ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.problemText, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_SEVERITY))
				{
					memset ( alarm.event.percSeverity, 0, ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.percSeverity, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SPEC_PROBL))
				{
					alarm.event.specificProblem = 0;
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						alarm.event.specificProblem = (long)(*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[0]));
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_TIME))
				{
					memset ( alarm.event.time, 0, ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX );

					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						if ( strncpy((alarm.event.time), reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX) == 0 ){
							strncpy((alarm.event.time), reinterpret_cast<char *>(imm_object.attributes[j].attrValues[1]), ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX);
						}
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF))
				{
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						alarm.cpAlarmRef = (unsigned short)(*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[cpSide]));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER))
				{
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						alarm.retransCnt = (unsigned short)(*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[cpSide]));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SEND_PRIORITY))
				{
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						alarm.sendPriority = (*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[0]));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CEASE_PENDING))
				{
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						int cease_pending = (*reinterpret_cast<int *>(imm_object.attributes[j].attrValues[cpSide]));
						if(cease_pending == 1)
						{
							alarm.ceasePending = true;
						}
						else
						{
							alarm.ceasePending = false;
						}
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE))
				{
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						int ack = (*reinterpret_cast<int *>(imm_object.attributes[j].attrValues[cpSide]));
						if(ack == 1)
						{
							alarm.acknowledge = true;
						}
						else
						{
							alarm.acknowledge = false;
						}
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_MANUAL_CEASE))
				{
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						int manual_cease = (*reinterpret_cast<int *>(imm_object.attributes[j].attrValues[cpSide]));
						if( manual_cease == 1 )
						{
							alarm.manualCease = true;
						}
						else
						{
							alarm.manualCease = false;
						}
					}
					continue;
				}
			}
		}
		else
		{
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...): om_handler.getObject(...' failed to retrieve the object '%s': return code == %d", imm_object.objName.c_str(), ret_code);
			//Call trace function
			util.trace("ACEASE getAlarmInToList() ERROR - %s", problemData);
			return acs_alh::ERR_SAF_IMM_OM_GET;
		}

		alarmList->push_back(alarm);
	}

	return ret_code;
}

//========================================================================================
//	Match a string towards a keyword
//========================================================================================
bool matchKeyWord(string keyWord, string str)
{
	//Call trace function
	util.trace("ACEASE matchKeyWord() enter");

	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))toupper);

	return !str.empty() && !keyWord.find(str);
}

//========================================================================================
//	Strip leading and trailing spaces from a string buffer
//========================================================================================
string strip(const char buf[])
{
	string str(buf);
	string::size_type first = str.find_first_not_of(" \t");

	//Call trace function
	util.trace("ACEASE strip() enter");

	if (first != string::npos)
	{
		string::size_type last = str.find_last_not_of(" \t");
		return str.substr(first, last - first + 1);
	}
	else
	{
		return "";
	}
}

//========================================================================================
//	Expect "YES" or "NO" answer
//========================================================================================
bool affirm()
{
	char ibuf[128] = {0};
	string answer;

	//Call trace function
	util.trace("ACEASE affirm() enter");


	//TR HR45341 added check about ctrl-C received
	while ( 1 && !acease_sighandler.isSignalStopReceived )
	{
		cout << "[Y(ES)/N(O)]" << dialoguePrompt;
		cin.clear();
		cin.getline(ibuf, 128);
		sleep(1);

		answer = strip(ibuf);

		std::transform(answer.begin(), answer.end(), answer.begin(), (int(*)(int))toupper);

		if (matchKeyWord("YES", answer))
		{
			return true;
		}
		if (matchKeyWord("NO", answer))
		{
			return false;
		}
		else
		{
			//TR HR45341 cout only if not received signal stop
			if ( !acease_sighandler.isSignalStopReceived )
				cout << "Enter ";
		}
	}
	//TR HR45341 when received signal stop exit
	exit(4);

}

//========================================================================================
//	Check if the alarm identity exists
//========================================================================================
int checkIdentity(AllRecord&	    allRec,
		pair<bool, int>  specificProblem,
		pair<bool, int>  index)
{
	// return code
	int ret_code = 0;

	//Call trace function
	util.trace("ACEASE checkIdentity() enter");

	if (specificProblem.first)
	{
		if (specificProblem.second != allRec.event.specificProblem)
			ret_code = 1;

		if (index.first && (index.second != allRec.identity[2]))
			ret_code = 1;

		if (strcmp(allRec.event.percSeverity, "CEASING") == 0)
			ret_code = 2;
	}

	//Call trace function
	util.trace("ACEASE checkIdentity() exit - return: %i", ret_code);

	return ret_code;
}

//========================================================================================
//  Get the IP address
//========================================================================================
int getPeerIPAddress(string &ip_address)
{
	ifstream ifs;

	//Call trace function
	util.trace("ACEASE getPeerIPAddress() enter");

	ifs.open("/etc/cluster/nodes/peer/networks/internal/primary/address");

	if (ifs.good())
	{
		getline(ifs, ip_address);
	}
	else
	{
		util.trace("ACEASE getPeerIPAddress() exit - Returning -1");
		return -1;
	}

	ifs.close();

	return 0;
}

//========================================================================================
//	Send on Socket
//========================================================================================
int sendOnSocket(std::string message)
{
	int sockfd;
	int portno = 49153;
	int n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int ip_result;
	struct in_addr addr;
	char buffer[4026];
	string id_name;
	string ip_address;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cout << "System error: Failed to opening socket." << endl << endl;
		exit (17);
	}

	ip_result = getPeerIPAddress(ip_address);

	if(ip_result == 0)
	{
		addr.s_addr = inet_addr(ip_address.c_str());
	}
	else
	{
		cout << "System error: Failed to retrieve the IP address." << endl << endl;
		exit (17);
	}

	server = gethostbyaddr(&addr, 4, AF_INET);

	if (server == NULL)
	{
		switch (h_errno)
		{
		case HOST_NOT_FOUND:
			cout << "System error: Host not found." << endl << endl;
			break;
		case NO_ADDRESS:
			cout << "System error: No address." << endl << endl;
			break;
			/* case NO_DATA: print ("NO_DATA\n"); break; */
		case NO_RECOVERY:
			cout << "System error: No recovery." << endl << endl;
			break;
		case TRY_AGAIN:
			cout << "System error: Try again." << endl << endl;
			break;
		}
		exit (17);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

	/* Port Reservation : START 																 */
	/* Read the port number to be used from /etc/services file if not found uses the default one */
	struct servent *serv = 0;
	string name = "acs_alh_sl-ng";
	string protocol = "tcp";

	serv = getservbyname(name.c_str() , protocol.c_str());

	if (serv == NULL){

		/*Not found in file using default one*/
		serv_addr.sin_port = htons(portno);
	}else {
		/*Found using retrieved value*/
		serv_addr.sin_port = serv->s_port;

	}
	/* Port Reservation :END 																	 */

	//serv_addr.sin_port = htons(portno);


	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		cout << "System error: Failed to connection." << endl << endl;
		exit (17);
	}

	//printf("Send Message on Socket: ");
	bzero(buffer,4026);
	snprintf(buffer, sizeof(buffer) - 1, message.c_str());
	n = write(sockfd,buffer,strlen(buffer));

	if (n < 0)
	{
		cout << "System error: Failed to writing to socket." << endl << endl;
		exit (17);
	}

	bzero(buffer,256);
	close(sockfd);
	return 0;
}

//========================================================================================
//	Send Remote Message
//========================================================================================
void sendRemoteMessage(char* processName, long specificProblem, char* percSeverity, char* probableCause, char* objClassOfReference, char* objectOfReference, char* problemData, char* problemText)
{
	//bool send_event = true;
	//int retVal = 0;

	//Process name

	//Perceived severity

	// Probable cause
	if(!probableCause || !*probableCause )
	{
		snprintf(probableCause, sizeof(probableCause) - 1, "-");
	}

	// Object class of reference
	if(!objClassOfReference || !*objClassOfReference )
	{
		snprintf(objClassOfReference, sizeof(objClassOfReference) - 1, "-");
	}

	// Object of reference
	if(!objectOfReference || !*objectOfReference )
	{
		snprintf(objectOfReference, sizeof(objectOfReference) - 1, "-");
	}

	// problem data
	if(!problemData || !*problemData )
	{
		snprintf(problemData, sizeof(problemData) - 1, "-");
	}

	// problem text
	if(!problemText || !*problemText )
	{
		snprintf(problemText, sizeof(problemText) - 1, "-");
	}

	// user name
	uid_t userUID;
	passwd *userInfo = 0;
	char username[32];
	userUID = getuid();

	userInfo = getpwuid(userUID);

	if(userInfo == 0)
	{
		snprintf(username, sizeof(username) - 1, "-");
	}
	else
	{
		snprintf(username, sizeof(username) - 1, userInfo->pw_name);
	}

	//Node Name
	char nodeName[16] = {0};
	snprintf(nodeName, sizeof(nodeName) - 1, getremoteNodeName().c_str());

	if(!*nodeName)
	{
		snprintf(nodeName, sizeof(nodeName) - 1, "-");
	}

	// Node Status
	char nodeStatus[16] = "PASSIVE";

	std::stringstream s;
	s << " " << nodeName << " " << nodeStatus << " "
			<< "error" << " " << username << " "
			<< processName << " " << specificProblem <<" "
			<< percSeverity << " " << "0"
			<< " P_CAUSE: " << probableCause
			<< " CLASS_REF: " << objClassOfReference
			<< " OBJ_REF: " << objectOfReference
			<< " P_DATA: " << problemData
			<< " P_TEXT: " << problemText<<"\n";//added for issue on SLES12 on ceasing remote alarm

	std::string eventMessage = s.str();
	//cout<<"MESSAGGIO DA INVIARE:\n"<<eventMessage<<endl;
	sendOnSocket(eventMessage);
}

//========================================================================================
//	Send a cease alarm request to AEH
//========================================================================================
void sendEventMessage(apevent_mess& event, int index)
{
	// Parameters:
	//   allRec:              Alarm record (in)
	//   index:               The alarm-index (in)

	//Call trace function
	util.trace("ACEASE sendEventMessage() enter");

	// Set terminating 0 to avoid problem if file-data updating during read
	event.processName[PROCESS_NAME_MAX_LEN-1] = 0;
	event.probableCause[PROBABLE_CAUSE_MAX_LEN-1] = 0;
	event.objClassOfReference[OBJ_CLASS_OF_REF_MAX_LEN-1] = 0;
	event.objectOfReference[OBJECT_OF_REF_MAX_LEN-1] = 0;

	acs_aeh_evreport EvReport;

	char textBuffer[256] = {0};
	snprintf(textBuffer, sizeof(textBuffer) - 1, "Ceased by acease command index %d", index);

	char addBuffer[256] = {0};
	snprintf(addBuffer, sizeof(addBuffer) - 1, "Alarm originated from %s", event.processName);

	char myProcName[PROCESS_NAME_MAX_LEN] = {0};
	snprintf(myProcName, sizeof(myProcName) - 1, "acease:%d", getpid());

	int rcode = 0;

	const char* alarmClass = "CEASING";

	if(passive)
	{
		sendRemoteMessage(
				myProcName,
				event.specificProblem,
				const_cast<char*>(alarmClass),
				event.probableCause,
				event.objClassOfReference,
				event.objectOfReference,
				textBuffer,
				addBuffer
		);
	}
	else
	{
		rcode = EvReport.sendEventMessage(
				myProcName,
				event.specificProblem,
				"CEASING",
				event.probableCause,
				event.objClassOfReference,
				event.objectOfReference,
				textBuffer,
				addBuffer
		);
	}

	//Call trace function
	util.trace("ACEASE sendEventMessage() exit");

	if (rcode != ACS_AEH_ok)
	{
		cout << "Internal error: Event reporting failed." << endl << endl;
		exit (18);
	}
}

//========================================================================================
//	Reset Inconsistency Alarm
//========================================================================================
void resetInconsistencyAlarm()
{
	util.trace("ACEASE resetInconsistencyAlarm() enter");

	std::string inconsistencyAlarmFile = INCONSISTENCY_ALARM_FILE_NAME;

	if (ACE_OS::unlink(inconsistencyAlarmFile.c_str()) < 0)
	{
		util.trace("ACEASE resetInconsistencyAlarm() failed to delete file: %s, errno = %d", inconsistencyAlarmFile.c_str(), errno);
	}
}

//========================================================================================
//	Check if user is a member of com emergency
//========================================================================================
bool isMemberOfComEmergency(const std::string user)
{
	const std::string COM_EM_GRP = "com-emergency";
	bool isMember = false;
	/* get the information about the "strange" group. */
	struct group* group_info = getgrnam(COM_EM_GRP.c_str());
	/* make sure this group actually exists. */
	if (!group_info) {

		util.trace("'com-emergency' does not exist. user=%s", user.c_str());

	}
	else {
		char** p_member;
		string str_member;
		for (p_member = group_info->gr_mem; *p_member; p_member++) {
			util.trace("COM Emergency member = <%s>", *p_member );
			str_member = *p_member;
			if (strcmp(user.c_str(), str_member.c_str()) == 0){
				util.trace("The user <%s> belongs to com-emergency group", user.c_str());
				isMember = true;
				break;
			}
		}
	}

	return isMember;
}

bool checkUserRole(const string userName)
{
	bool result =false;
	ACS_APGCC_CommonLib  apgccLib;
	string  output = "" , tmpRoles = "";
	if(apgccLib.GetRoles (userName , output) == ACS_APGCC_ROLES_SUCCESS)
	{
		if (output.length() > 0)
		{
			string tmpRoles = output;
			for (int j=0; j<(int) tmpRoles.length(); j++)  tmpRoles[j] = toupper(tmpRoles[j]);
			if (tmpRoles.find("SYSTEMADMINISTRATOR") != string::npos)   //  the user has the SystemSecurityAdministrator authorization
			{
				systemadminstrator = true;
				result = true;
			}
			if (tmpRoles.find("CPROLE") != string::npos)    //  the user has a CpRole<i> authorization
			{
				customRoleUser = true;
				result = true;
			}
			if (!customRoleUser)
			{
				if(isRole(output,"AxeMmlAuthorizationMmlRole"))
				{
					customRoleUser = true;
					result = true;
				}
				if(!customRoleUser && isRole(output,"AxeMmlAuthorizationMmlSystemRole"))
				{
					customRoleUser = true;
					result = true;
				}
			}
		}//if lenght >0
		else
		{
			util.trace("Empty list...");
		} 
	} //if agpcc method
	return result;
}//function checkUserRole

//========================================================================================
//	Check if user is a normal user
//========================================================================================
bool isNormalUser(string user)
{
	util.trace("user = %s", user.c_str());

	if (isMemberOfComEmergency(user.c_str()))
	{

		util.trace("User authority set to EXPERT");
		return false;

	}
	if (checkUserRole(user.c_str()))
	{
		if(customRoleUser && systemadminstrator){
			util.trace("User authority set to EXPERT");
			return false;
		}
		if(customRoleUser){
			util.trace("User authority set to NORMAL");
			return true;
		}
		else if(systemadminstrator){
			util.trace("User Role: '%s' set as NORMAL", user.c_str());
			return true;
    }
	} 
	else
	{
		//To be reviewed
		util.trace("User Role: '%s' set as NORMAL", user.c_str());
		return true;
	}
}   //isNormalUser()


// main program
int main(int argc, char *argv[])
{
	// This program is used to cease a specific alarm
	// Usage: acease [-p] [-o] specificProblem:index
	// Only alarms that are not automatically ceased can be ceased,
	// however by specifying option -o an expert user may
	// cease any alarm.;

	int optIndex;
	string alarmIdentifier;
	string user("");
	int num_o = 0;
	int num_i = 0;
	bool override(false);
	int num_p = 0;

	// HANDLE OF CTRL+C
	// we have to handle "termination signals" (listed above in siglist array). Install our signal handler
	ACE_Sig_Handler ace_sigdispatcher;
	// TR HR45341
	//acs_alh_signal_handler acease_sighandler;
	acease_sighandler.isSignalStopReceived = false;


	int numsigs = sizeof(siglist) / sizeof(int);

	for(int i = 0; i < numsigs; ++i)
		ace_sigdispatcher.register_handler(siglist[i], & acease_sighandler);

	//Call trace function
	util.trace("ACEASE Main called enter");

	//get User value
	user = argv[1];

	for (optIndex = 2; optIndex < argc; optIndex++)
	{

		//Call trace function
		util.trace("ACEASE Main called - For statement parse command");
		if (strcmp (argv[optIndex], "-p") == 0)	// Fetch parameter -p
		{
			if (passive == false)
			{
				// Fetch the value
				if (argv[optIndex + 1])
				{
					if (strcmp (argv[optIndex + 1], "-o") != 0)
					{
						alarmIdentifier = argv[optIndex + 1];
						optIndex++;
					}
				}
				else
				{
					if(num_i == 0 && num_o == 0)
					{
						usage();
						return 2;
					}
				}
				num_p++;
				passive = true;
			}
			else
			{
				usage();
				return 2;
			}
		}
		else if (strcmp (argv[optIndex], "-o") == 0)	// Fetch parameter -o
		{
			// Fetch the value
			if (argv[optIndex + 1])
			{
				if (strcmp (argv[optIndex + 1], "-p") != 0)
				{
					alarmIdentifier = argv[optIndex + 1];
					optIndex++;
				}
			}
			else
			{
				if(num_i == 0 && num_p == 0)
				{
					usage();
					return 2;
				}
			}
			override = true;
			num_o++;
		}
		else
		{
			alarmIdentifier = argv[optIndex];
			num_i++;
		}

	} //end for

	//Call trace function
	util.trace("ACEASE Main called - After for statement");

	//Check usage of options
	//Same option may not be used more than once
	if(num_p > 1 || num_o > 1)
	{
		usage();
		return 2;
	}
	if(optIndex == 2)
	{
		usage();
		return 2;
	}
	else if(optIndex == 3)
	{
		override = false;
		passive = false;
	}
	if (passive == false && override == false)
	{
		string::size_type dash = alarmIdentifier.find_first_of("-");
		if (dash != string::npos)
		{
			usage();
			return 2;
		}

		if(optIndex != 3)
		{
			usage();
			return 2;
		}
	}

	string::size_type pos = alarmIdentifier.find_first_of(":");
	if (pos == string::npos)
	{
		cout << "Parameter error: Alarm identifier has illegal format." << endl << endl;
		return 16;
	}

	// Analyze alarm identifier
	pair<bool, int> specificProblem(false, 0);
	pair<bool, int> index(false, 0);
	if (alarmIdentifier.empty() == false)
	{
		//Call trace function
		util.trace("ACEASE Main called - Analyze alarm identifier");

		string::size_type pos = alarmIdentifier.find_first_of(":");

		if (pos == string::npos)
		{
			specificProblem.first = true;
			specificProblem.second = strToInt(alarmIdentifier);
		}
		else
		{
			specificProblem.first = true;
			string tstr = alarmIdentifier.substr(0, pos);
			specificProblem.second = strToInt(tstr);
			index.first = true;
			tstr = alarmIdentifier.substr(pos+1);
			index.second = strToInt(tstr);
		}
	}

	//cout << "alarmIdentifier: " << alarmIdentifier << endl;
	std::vector<AllRecord> alarmList;
	int result;

	if (passive == false)
	{
		result = getAlarmInToList(&alarmList, 0, getlocalNodeName());	//secondo parametro per il cpSide, terzo parametro local or remote node name
	}
	else
	{
		result = getAlarmInToList(&alarmList, 0, getremoteNodeName()); //secondo parametro per il cpSide, terzo parametro local or remote node name
	}

	int checkNumber = 0;
	bool cease(false);
	bool reset(false);
	AllRecord allRec;

	if(result == 0)
	{
		int numOfAlarmIntoList = (int)alarmList.size();

		int i = 0;
		while(i < numOfAlarmIntoList)
		{
			// Set record for alarm
			allRec.event = alarmList[i].event;

			allRec.identity[0] = alarmList[i].identity[0];
			allRec.identity[1] = alarmList[i].identity[1];
			allRec.identity[2] = alarmList[i].identity[2];
			allRec.cpAlarmRef = alarmList[i].cpAlarmRef;
			allRec.retransCnt = alarmList[i].retransCnt;
			allRec.sendPriority = alarmList[i].sendPriority;
			allRec.ceasePending = alarmList[i].ceasePending;
			allRec.acknowledge = alarmList[i].acknowledge;
			allRec.manualCease = alarmList[i].manualCease;

			checkNumber = checkIdentity(allRec, specificProblem, index);
			if(checkNumber == 0)
			{
				i++;

				// Set record for alarm
				allRec.event = alarmList[i].event;

				allRec.identity[0] = alarmList[i].identity[0];
				allRec.identity[1] = alarmList[i].identity[1];
				allRec.identity[2] = alarmList[i].identity[2];
				allRec.cpAlarmRef = alarmList[i].cpAlarmRef;
				allRec.retransCnt = alarmList[i].retransCnt;
				allRec.sendPriority = alarmList[i].sendPriority;
				allRec.ceasePending = alarmList[i].ceasePending;
				allRec.acknowledge = alarmList[i].acknowledge;
				allRec.manualCease = alarmList[i].manualCease;

				checkNumber = checkIdentity(allRec, specificProblem, index);

				if (checkNumber == 1)
				{
					i--;
					// Set record for alarm
					allRec.event = alarmList[i].event;

					allRec.identity[0] = alarmList[i].identity[0];
					allRec.identity[1] = alarmList[i].identity[1];
					allRec.identity[2] = alarmList[i].identity[2];
					allRec.cpAlarmRef = alarmList[i].cpAlarmRef;
					allRec.retransCnt = alarmList[i].retransCnt;
					allRec.sendPriority = alarmList[i].sendPriority;
					allRec.ceasePending = alarmList[i].ceasePending;
					allRec.acknowledge = alarmList[i].acknowledge;
					allRec.manualCease = alarmList[i].manualCease;

					checkNumber = 0;
				}

				//				//============================================================================================================
				//				//TO TEST
				//				//===========================================================================================================
				//				printf("Identity %d:%d:%d\n", allRec.identity[0],  allRec.identity[1], allRec.identity[2]);
				//				printf("processName: %s\n", allRec.event.processName);
				//
				//				printf("Time: %s\n", allRec.event.time);
				//				printf("Severity of Alarm: %s\n", allRec.event.percSeverity);
				//				printf("Specific Problem of Alarm: %ld\n", allRec.event.specificProblem);
				//				printf("Probable cause of Alarm: %s\n", allRec.event.probableCause);
				//				printf("Category of Alarm: %s\n", allRec.event.objClassOfReference);
				//				printf("Object of reference of Alarm: %s\n", allRec.event.objectOfReference);
				//				printf("Problem data of Alarm: %s\n", allRec.event.problemData);
				//				printf("Problem text of Alarm: %s\n", allRec.event.problemText);
				//				printf("cpAlarmRef of Alarm: %u\n", allRec.cpAlarmRef);
				//				printf("Retransmission counter of Alarm: %u\n", allRec.retransCnt);
				//				printf("Send Priority of Alarm: %u\n", allRec.sendPriority);
				//				printf("Cease pending of Alarm: %d\n", allRec.ceasePending);
				//				printf("Manual ceasing of Alarm: %d\n", allRec.manualCease);
				//				printf("Ack of Alarm: %d\n", allRec.acknowledge);
				//				//============================================================================================================

				break;
			}

			i++;
		}//end for

		if ( numOfAlarmIntoList == 0 ){
			checkNumber = 1;
		}
	}
	else
	{
		// Error! Impossible to retrieve the alarm list from IMM
		cout << "Internal error: Impossible to retrieve the alarm list" << endl << endl;
		//Call trace function
		util.trace("ACEASE Main called - Analyze alarm identifier - exit - Returning false!!!");
		return 18;
	}

	if(checkNumber == 0)
	{	//Alarm found

		// Check if user is normal user
		bool isNormal = isNormalUser(user);

		if (allRec.manualCease)
		{
			cease = true;

			if (!isNormal && INCONSISTENCY_ALARM_ID == specificProblem.second)
			{
				reset = true;
			}
			else if(!isNormal)
			{
				//Do nothing
			}
			else if (INCONSISTENCY_ALARM_ID == specificProblem.second)
			{
				reset = true;
			}
			else
			{
				cout << "Not allowed." << endl;
				cout << "Only expert user may cease this alarm." << endl << endl;
				return 6;
			}
		}
		else if (override)
		{
			// Check if is expert user
			if (!isNormal)
			{
				cout << "Warning - this alarm should be manually ceased only if the" << endl;
				cout << "automatic cease have failed. Are you sure that you want to continue? " << endl;

				if (affirm() == true)
				{
					cease = true;
				}
				else
				{
					cout << "Cancelled" << endl << endl;
				}
			}
			else
			{
				cout << "Not allowed." << endl;
				cout << "Only expert user may cease this alarm." << endl << endl;
				return 6;
			}
		}
		else
		{
			cout << "This alarm cannot be manually ceased." << endl << endl;
			return 5;
		}

		if (cease)
		{
			sendEventMessage(allRec.event, index.second);
			cout << "Alarm ceased." << endl << endl;
			if (reset)
			{
				resetInconsistencyAlarm();
			}
		}
	}
	else if(checkNumber == 1)
	{
		cout << "Alarm not found in alarm list." << endl << endl;
		return 1;
	}
	else if(checkNumber == 2)
	{
		cout << "Alarm already ceased in alarm list." << endl << endl;
		return 3;
	}
	else
	{
		cout << "System error: Failed to read alarm list file." << endl << endl;
		return 17;
	}


	// remove our actions from ACE Signal Dispatcher, and restore default actions
	for(int j = 0; j < numsigs; ++j)
		ace_sigdispatcher.remove_handler(siglist[j]);

	return 0;
}


