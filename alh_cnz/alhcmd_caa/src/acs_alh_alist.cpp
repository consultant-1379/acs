/*
 *
 * NAME: acs_dsd_alist.cpp
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
 *  This is the main program for the alist command.
 *
 * DOCUMENT NO
 *	-
 *
 * AUTHOR
 * 	2011-11-30 TEI/XSD  XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	       	20111130	XGIOPAP		First Revision
 *
 */

// Module Include Files
#include "acs_alh_util.h"
#include "acs_alh_signal_handler.h"
#include "acs_alh_common.h"
#include "acs_apgcc_omhandler.h"
#include "ace/Signal.h"
#include "ace/Sig_Handler.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cstdarg>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>

using namespace std;

// Global object
acs_alh_util util("ALIST");  //Trace object

static int siglist[] = {SIGHUP, SIGQUIT, SIGINT, SIGTERM};      		// list of the signals that we must catch and handle.
OmHandler om_handler_;
bool omHandlerInitialized_;

template <class BidirectionalIterator>
void Myreverse(BidirectionalIterator first, BidirectionalIterator last)
{
	while((first != last) && (first != --last))
		swap (*first++, *last);
}

bool isAlhServerRunning( std::string local_node ){

	OmHandler om_handler;

	if ( om_handler.Init() != ACS_CC_SUCCESS)
		return false;

	std::vector<std::string> dn_list;
	std::string local_alarm_list_dn;

	if ( om_handler.getClassInstances(ACS_ALH_CONFIG_IMM_ALARM_LIST_NODE_CLASS_NAME, dn_list) != ACS_CC_SUCCESS) {
		om_handler.Finalize();
		return false;
	}
	else {
		if ( dn_list.size() == 0 ) { //WARNING: 0 (zero) or more than one node found
			om_handler.Finalize();
			return false;
		}
		else { //OK: Only one root node
			if ( dn_list[0].find(local_node) == std::string::npos ){ // local hostname is not included in the first dn
				if ( dn_list[1].find(local_node) == std::string::npos ){ // local hostname is not included in the second dn
					om_handler.Finalize();
					return false;
				}
				else {
					local_alarm_list_dn = dn_list[1];
				}
			}
			else {
				local_alarm_list_dn = dn_list[0];
			}
		}
	}

	ACS_APGCC_ImmObject imm_object;
	imm_object.objName =  local_alarm_list_dn.c_str();

	om_handler.getObject(&imm_object);

	unsigned attributes_num = imm_object.attributes.size();

	char ImplementerName[256] = {0};

	for ( unsigned j=0; j < attributes_num; j++){

		const char *attr_name = imm_object.attributes[j].attrName.c_str();

		if ( strcmp(attr_name, "SaImmAttrImplementerName") == 0 ){

			if ( imm_object.attributes[j].attrValuesNum != 0 )
				strncpy(ImplementerName, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), sizeof(ImplementerName));

			break;
		}
	}

	if ( strlen ( ImplementerName ) && strstr ( ImplementerName, ALHD_IMM_PREFIX) ){
		om_handler.Finalize();
		return true;
	}

	om_handler.Finalize();

	return false;
}

//========================================================================================
//	Print alist usage
//========================================================================================
void usage()
{
	cout << "Usage: alist [-p] [-r] [-i specificProblem[:indexNumber]]" << endl;
	cout << "       [-s alarmClass] [-x probableCause] [numberOfAlarms]" << endl << endl;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//isNumeric method
//----------------------------------------------------------------------------------------------------------------------------------------------------
bool isNumeric(const char *s)
{
	int l = strlen(s);

	//Call trace function
	util.trace("ALIST isNumeric method enter");

	for(int i = 0; i < l; i++)
	{
		if((s[i] < '0') || (s[i] > '9'))
			return false;
	}

	return true;
}

//========================================================================================
//	Convert a string to an integer value
//========================================================================================
int strToInt(const string str)
{
	//Call trace function
	util.trace("ALIST strToInt method enter");

	if (str.empty())
	{
		fprintf(stderr, "Parameter error: Numeral expected.\n\n");
		exit(16);
	}

	char* p;
	errno = 0;
	int var = strtol(str.data(), &p, 10);

	if (*p != ' ' && *p != '\t' && *p != 0)
	{
		fprintf(stderr, "Parameter error: Illegal character.\n\n");
		exit(16);
	}

	if (errno == ERANGE)
	{
		fprintf(stderr, "Parameter error: Overflow.\n\n");
		exit(16);
	}

	//Call trace function
	util.trace("ALIST strToInt method Returning var: %i", var);

	return var;
}

//========================================================================================
//  Get the local node Name
//========================================================================================
std::string getlocalNodeName()
{
	//Call trace function
	util.trace("ALIST getlocalNodeName() enter");

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
	util.trace("ALIST getremoteNodeName() enter");

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
	util.trace("ALIST get_alh_imm_root_dn() enter");

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
	util.trace("ALIST get_alh_imm_root_dn() Returning code: %i", return_code);

	return return_code;
}


//========================================================================================
//	omHandler_init method
//========================================================================================
int omHandler_init()
{
	ACS_CC_ReturnType imm_result;

	//Call trace function
	util.trace("ALIST omHandler_init() enter");

	if ((imm_result = om_handler_.Init()) != ACS_CC_SUCCESS)
	{
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in acs_alh_imm_data_handler: OmHandler::Init()' failed: return code == %d", imm_result);
		//Call trace function
		util.trace("ALIST omHandler_init() ERROR - %s", problemData);

		return -1;
	}

	omHandlerInitialized_ = true;

	return 0;
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
	util.trace("ALIST getAlarmInToList() enter");

	/*Searching the parent name of the class to be defined*/
	call_result = get_alh_imm_root_dn(imm_alh_root_dn);
	if (call_result)
	{
		char problemData[DEF_MESSIZE] = {0};
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d", call_result);
		//Call trace function
		util.trace("ALIST getAlarmInToList() ERROR - %s", problemData);
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}


	if(!omHandlerInitialized_ && (ret_code = omHandler_init()) != acs_alh::ERR_NO_ERRORS){
		char problemData[DEF_MESSIZE] = {0};
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...) - omHandler_init() failed: return code == %d", ret_code);
		//Call trace function
		util.trace("ALIST getAlarmInToList() ERROR - %s", problemData);
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
		util.trace("ALIST getAlarmInToList() ERROR - %s", problemData);
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
			util.trace("ALIST getAlarmInToList() ERROR - %s", problemData);
			return acs_alh::ERR_SAF_IMM_OM_GET;
		}

		alarmList->push_back(alarm);
	}

	return ret_code;
}

//========================================================================================
// Print the Alarm
//========================================================================================
bool printAlarm(AllRecord&	    allRec,
				pair<bool, int> specificProblem,
                pair<bool, int> index,
                string		    alarmClass,
                string		    probableCause)
{
	// Description:
	//   This routine compares an alarm in allRec with the options specified
	//   in the command. If match, the alarm is printed.
	//   The parameters are the same as in ACS_ALH_doList except for the
	//   allRec parameter which is a pointer to an alarm-record
	// Return values:
	//   false                 The alarm does not match. Not printed
	//   true                  The alarm is printed

	// First, check if any options for restriction of printing
	bool print(true);

	//Call trace function
	util.trace("ALIST printAlarm() enter");

	if (specificProblem.first)
	{
		if (specificProblem.second != allRec.event.specificProblem)
			print = false;	// Do not print
		if (index.first && (index.second != allRec.identity[2]))
			print = false;	// Do not print
	}

	if (alarmClass.empty() == false)
	{
		if (alarmClass != allRec.event.percSeverity)
			print = false; // Do not print
	}

	if (probableCause.empty() == false)
	{
		// strncmp used to avoid problem if file-data updating during read
		if ((strstr(allRec.event.probableCause, probableCause.data())) == 0)
		{
			print = false; // Do not print
		}
	}

	if ( ( allRec.event.specificProblem <= 0 ) || ( strlen(allRec.event.percSeverity) == 0 ) || ( strlen ( allRec.event.objClassOfReference ) == 0 ) ||
		 strlen ( allRec.event.time ) == 0 || strlen ( allRec.event.probableCause ) == 0)
		print = false;

	// Print alarm if not restricted by options
	if (print)
	{
		// Set terminating 0 to avoid problem if file-data updating during read
		allRec.event.percSeverity[PERC_SEVERITY_MAX_LEN-1] = 0;
		allRec.event.probableCause[PROBABLE_CAUSE_MAX_LEN-1] = 0;
		allRec.event.objClassOfReference[OBJ_CLASS_OF_REF_MAX_LEN-1] = 0;
		allRec.event.objectOfReference[OBJECT_OF_REF_MAX_LEN-1] = 0;
		allRec.event.problemText[PROBLEM_TEXT_MAX_LEN-1] = 0;
		allRec.event.problemData[PROBLEM_DATA_MAX_LEN-1] = 0;

		// Printout
		cout << left;
		cout << endl;
		cout << "Alarm Identifier    Class    Category    Time" << endl;
		ostringstream s;
		s << allRec.event.specificProblem << ":" << allRec.identity[2];
		cout << setw(20) << s.str()
			 << setw(9) << allRec.event.percSeverity
			 << setw(12) << allRec.event.objClassOfReference
			 << setw(20) << allRec.event.time;
		cout << endl << endl;
		cout << "Object of Reference" << endl;
		cout << allRec.event.objectOfReference << endl;
		cout << endl;
		cout << "Alarm Text" << endl;
		cout << allRec.event.probableCause << endl;
		cout << allRec.event.problemText << endl;
		cout << endl;
		cout << "Problem Data" << endl;
		cout << allRec.event.problemData << endl;
		cout << endl;
	}

	//Call trace function
	util.trace("ALIST printAlarm() exit");

	return print;
}

bool sortVectorByRules(AllRecord objA, AllRecord objB)
{
	bool bRet;
	int iCompare;
	// Init
	bRet = false;
	// Compare time-stamps.
	iCompare = strcmp(objA.event.time,objB.event.time);
	// Check time-stamps
	if(iCompare == 0){
		// Compare specificProblem
		if(objA.event.specificProblem < objB.event.specificProblem){
			// Oggetto A viene prima di B
			bRet = true;
		}else if (objA.event.specificProblem == objB.event.specificProblem){
			// Order by ID2
			if(objA.identity[2] < objB.identity[2]){
				// Oggetto A viene prima di B
				bRet = true;
			}
		}
	}else if(iCompare < 0){
		// Oggetto B viene prima di A
		bRet = true;
	}
	// Exit from method
	return bRet;
}

// main program
int main(int argc, char *argv[])
{
	// This program reads the AlarmListFile and list the alarms.
	//
	// How to use it:
	// alist [-r] [-i specificProblem[:indexNumber]]
	//		 [-s alarmClass] [-x pobableCause] [numberOfAlarms]
	//
	// options:
	// -p				List the alarms of the partner node
	// -r				Reverse order. Search alarmList from the end
	// -i specificProblem[:indexNumber]
	//					List only alarms with specificProblem. Can be further
	//					restricted by index. SpecificProblem and index must be
	//					separated by a ':'.
	// -s alarmclass	List only alarms with specified AlarmClass
	// -x probableCause	List only alarms with Probable Cause
	// Num				List not more than Num commands
	//

	int optIndex;
	bool reverse(false);
	bool passive(false);
	string alarmIdentifier;
	string alarmClass;
	string probableCause;
	int temp;
	int numberOfAlarms = 100000;
	int num_p = 0;
	int num_r = 0;
	int num_i = 0;
	int num_s = 0;
	int num_x = 0;
	int num_num = 0;

	//Call trace and logging function
	char traceText[255] = {0};
	char name[] = "alist";
	snprintf(traceText, sizeof(traceText) - 1, "ALIST Main called - argc = %d, y = %s", argc, name);
	//Call trace function
	util.trace(traceText);

	// we have to handle "termination signals" (listed above in siglist array). Install our signal handler
	ACE_Sig_Handler ace_sigdispatcher;
	acs_alh_signal_handler alist_sighandler;
	int numsigs = sizeof(siglist) / sizeof(int);

	for(int i = 0; i < numsigs; ++i)
		ace_sigdispatcher.register_handler(siglist[i], & alist_sighandler);

	//Call trace function
	util.trace("ALIST Main called - Handle CTRL C");

	for (optIndex = 1; optIndex < argc; optIndex++)
	{
		//Call trace function
		util.trace("ALIST Main called - For statement parse command");

		if (strlen(argv[optIndex]) < 2)
		{
			if (isNumeric(argv[optIndex]) == false)
			{
				fprintf(stderr, "Parameter error: Illegal character\n\n");
				return 16;
			}
		}

		if(strcmp (argv[optIndex], "-p") != 0 && strcmp (argv[optIndex], "-r") != 0 && strcmp (argv[optIndex], "-i") != 0 &&
		   strcmp (argv[optIndex], "-s") != 0 && strcmp (argv[optIndex], "-x") != 0 && isNumeric(argv[optIndex]) == false)
		{
			fprintf(stderr, "alist: illegal option %s\n", argv[optIndex]);
			usage();
			return 2;
		}

		if (strcmp (argv[optIndex], "-p") == 0)	// Fetch parameter -p
		{
			if (passive == false)
			{
				num_p++;
				//optIndex++;
				passive = true;
			}
			else
			{
				//Incorrect usage
				usage();
				return 2;
			}
		}
		else if (strcmp (argv[optIndex], "-r") == 0)	// Fetch parameter -r
		{
			if (reverse == false)
			{
				num_r++;
				//optIndex++;
				reverse = true;
			}
			else
			{
				//Incorrect usage
				usage();
				return 2;
			}
		}
		else if (strcmp (argv[optIndex], "-i") == 0)	// Fetch parameter -i
		{
			// Fetch the value of the specific problem
			if (argv[optIndex + 1])
			{
				alarmIdentifier = argv[optIndex + 1];
				optIndex++;
				num_i++;
			}
			else
			{
				fprintf(stderr, "alist: option requires an argument %s\n", argv[optIndex]);
				usage();
				return 2;
			}
		}
		else if (strcmp (argv[optIndex], "-s") == 0)	// Fetch parameter -s
		{
			// Fetch the value of the alarm class
			if (argv[optIndex + 1])
			{
				if(strcmp(argv[optIndex + 1], "A1") != 0 && strcmp(argv[optIndex + 1], "A2") != 0 && strcmp(argv[optIndex + 1], "A3") != 0 &&
 				   strcmp(argv[optIndex + 1], "O1") != 0 && strcmp(argv[optIndex + 1], "O2") != 0 && strcmp(argv[optIndex + 1], "CEASING") != 0)
				{
					fprintf(stderr, "Parameter error: Illegal alarm class.\n\n");
					return 16;
				}
				else
				{
					alarmClass = argv[optIndex + 1];
					optIndex++;
					num_s++;
				}
			}
			else
			{
				fprintf(stderr, "alist: option requires an argument %s\n", argv[optIndex]);
				usage();
				return 2;
			}
		}
		else if (strcmp (argv[optIndex], "-x") == 0)	// Fetch parameter -x
		{
			// Fetch the value of the probable cause
			if (argv[optIndex + 1])
			{
				probableCause = argv[optIndex + 1];
				optIndex++;
				num_x++;
			}
			else
			{
				fprintf(stderr, "alist: option requires an argument %s\n", argv[optIndex]);
				usage();
				return 2;
			}
		}
		else if (isNumeric(argv[optIndex]))
		{
			temp = ::atoi(argv[optIndex]);

			if (temp < 1 || temp  > 99999)
			{
				fprintf(stderr, "Parameter error: Illegal value\n\n");
				numberOfAlarms = 0;
				return 16;
			}

			// check if exist the next option
			if (!argv[optIndex + 1])
			{
				optIndex++;
			}

			numberOfAlarms = temp;
			num_num++;
		}
	} //end for

	//Call trace function
	util.trace("ALIST Main called - After for statement");


	//Check usage of options
	//Same option may not be used more than once
	if((num_p > 1) || (num_r > 1) || (num_i > 1) || (num_s > 1) || (num_x > 1) || (num_num > 1))
	{
		usage();
		return 2;
	}

	// Analyze alarm identifier
	pair<bool, int> specificProblem(false, 0);
	pair<bool, int> index(false, 0);
	if (alarmIdentifier.empty() == false)
	{
		//Call trace function
		util.trace("ALIST Main called - Analyze alarm identifier");

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

	int checkNumber = 0;
	int call_res;
	AllRecord allRec;
	std::vector<AllRecord> alarmList;

	bool server_running = isAlhServerRunning( getlocalNodeName() );

	if ( !server_running ){ // ALH server is not running
		cout << "Internal error: Impossible to retrieve the alarm list" << endl << endl;
		return 18;
	}

	if (passive == false)
	{
		call_res = getAlarmInToList(&alarmList, 0, getlocalNodeName());	//secondo parametro per il cpSide, terzo parametro local or remote node name
	}
	else
	{
		call_res = getAlarmInToList(&alarmList, 0, getremoteNodeName()); //secondo parametro per il cpSide, terzo parametro local or remote node name
	}

	if(call_res == 0)
	{
		sort(alarmList.begin(), alarmList.end(), sortVectorByRules);

		int numOfAlarmIntoList = (int)alarmList.size();

		if(reverse == true)
		{
			int i = numOfAlarmIntoList - 1;
			while(i >= 0 && numberOfAlarms > 0)
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

					if(printAlarm(allRec, specificProblem, index, alarmClass, probableCause) == 1)
					{
						numberOfAlarms--;
						checkNumber = 1;
					}

					i--;
			}//end while
		}
		else
		{
			int i = 0;
			while(i < numOfAlarmIntoList && numberOfAlarms > 0)
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

					if(printAlarm(allRec, specificProblem, index, alarmClass, probableCause) == 1)
					{
						numberOfAlarms--;
						checkNumber = 1;
					}

					i++;
			}//end while
		}
	}
	else
	{
		// Error! Impossible to retrieve the alarm list from IMM
		fprintf(stderr, "Internal error: Impossible to retrieve the alarm list\n\n");
		//Call trace function
		util.trace("ALIST Main called - Analyze alarm identifier - exit - Returning false!!!");
		return 18;
	}

	if ((checkNumber == 0) && ((specificProblem.first == true) || (index.first == true) || (alarmClass.empty() == false) || (probableCause.empty() == false)))
	{
		cout << "No match found." << endl << endl;
		return 0;
	}

	// remove our actions from ACE Signal Dispatcher, and restore default actions
	for(int j = 0; j < numsigs; ++j)
		ace_sigdispatcher.remove_handler(siglist[j]);

	return 0;
}
