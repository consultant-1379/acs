/*
 * ACS_PRC_RunTimeOwner.cpp
 *
 *  Created on: Dec 28, 2010
 *      Author: xlucpet
 */

#include "ACS_PRC_RunTimeOwner.h"
#include "ACS_PRC_suInfo.h"
#include "acs_prc_api.h"
#include "ace/SOCK_Connector.h"
#include <fstream>
#include <string>
#include "acs_apgcc_omhandler.h"

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

ACS_PRC_RunTimeOwner::ACS_PRC_RunTimeOwner() {
	// TODO Auto-generated constructor stub
	p_objName_private = 0;

	p_local_node_id_path = "/etc/cluster/nodes/this/id";
	p_remote_node_id_path = "/etc/cluster/nodes/peer/id";
	p_local_node_hostname_path = "/etc/cluster/nodes/this/hostname";
	p_remote_node_hostname_path = "/etc/cluster/nodes/peer/hostname";
	p_node_1_hostname_path = "/etc/cluster/nodes/all/1/hostname";
	p_node_2_hostname_path = "/etc/cluster/nodes/all/2/hostname";

}

ACS_PRC_RunTimeOwner::~ACS_PRC_RunTimeOwner() {
	// TODO Auto-generated destructor stub
}

ACS_CC_ReturnType ACS_PRC_RunTimeOwner::updateCallback(const char* p_objName, const char* p_attrName){

	ACE_UNUSED_ARG(p_attrName);

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	char strErr_no[1024] = {0};

	snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_RunTimeOwner::updateCallback Begin", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	ACS_CC_ReturnType returnCode;
	ACS_PRC_suInfo data;
	ACS_PRC_API internal;
	data.getInfo();
	char node_id[8] = {0};

	string node_1_hostname, node_2_hostname;
	ifstream ifs;

	ACS_CC_ImmParameter parToModify;
	int tmp;
	void *temp[1];

	ifs.open(p_node_1_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, node_1_hostname);
	ifs.close();

	ifs.open(p_node_2_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, node_2_hostname);
	ifs.close();

	parToModify.attrType = ATTR_INT32T;
	parToModify.attrValuesNum = 1;
	temp[0]=reinterpret_cast<void *>(&tmp);
	parToModify.attrValues = temp;

	tmp = internal.askForNodeState ( node_1_hostname );
	parToModify.attrName = const_cast<char*>("nodeStateA");
	returnCode = this->modifyRuntimeObj(p_objName, &parToModify);

	tmp = internal.askForNodeState ( node_2_hostname );
	parToModify.attrName = const_cast<char*>("nodeStateB");
	returnCode = this->modifyRuntimeObj(p_objName, &parToModify);

	uint32_t RunLevel_Node_A = 0;
	uint32_t RunLevel_Node_B = 0;

	char other_Node_serverName[16];

	ifs.open(p_local_node_id_path.c_str());

	if ( ifs.good())
		ifs.getline(node_id, 8);

	ifs.close();

	ifs.open(p_remote_node_hostname_path.c_str());

	if ( ifs.good())
		ifs.getline(other_Node_serverName, 16);

	ifs.close();

	if ( strcmp(node_id,"1") == 0){ // Nodo A
		RunLevel_Node_A = data.getRunLevel("SC-2-1");
		if ( RunLevel_Node_A == 1) RunLevel_Node_A++;
		//strcpy(other_Node_serverName,"SC-2-2");
	}else{ // Nodo B
		RunLevel_Node_B = data.getRunLevel("SC-2-2");
		if ( RunLevel_Node_B == 1) RunLevel_Node_B++;
		//strcpy(other_Node_serverName,"SC-2-1");
	}

	ACE_SOCK_Connector  serverConnector;
	ACE_SOCK_Stream serverStream;
	ACE_INET_Addr server_addr(8881,other_Node_serverName);
	ACE_Time_Value val(3);

	if (serverConnector.connect (serverStream, server_addr, &val  ) == -1){
		int lastErr = ACE_OS::last_error();
		if ( lastErr == ECONNREFUSED ){ //REMOTE PRCMAN DOWN
			if ( strcmp(node_id,"1") == 0){
				RunLevel_Node_B = data.getRunLevel("SC-2-2");
				if ( RunLevel_Node_B == 5) RunLevel_Node_B = 2;
			}
			else {
				RunLevel_Node_A = data.getRunLevel("SC-2-1");
				if ( RunLevel_Node_A == 5) RunLevel_Node_A = 2;
			}
		}
		else // Remote node down
			if ( strcmp(node_id,"1") == 0)
				RunLevel_Node_B = 0;
			else
				RunLevel_Node_A = 0;
	}
	else {

		if ( strcmp(node_id,"1") == 0){
			RunLevel_Node_B = data.getRunLevel("SC-2-2");
			if ( RunLevel_Node_B == 1) RunLevel_Node_B++;
		}
		else {
			RunLevel_Node_A = data.getRunLevel("SC-2-1");
			if ( RunLevel_Node_A == 1) RunLevel_Node_A++;
		}
	}

	serverStream.close();

	tmp = RunLevel_Node_A;
	parToModify.attrName = const_cast<char*>("nodeRunLevelA");
	returnCode = this->modifyRuntimeObj(p_objName, &parToModify);

	tmp = RunLevel_Node_B;
	parToModify.attrName = const_cast<char*>("nodeRunLevelB");
	returnCode = this->modifyRuntimeObj(p_objName, &parToModify);

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_RunTimeOwner::updateCallback End", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	Logging.Close();

	return returnCode;
}

ACS_CC_ReturnType ACS_PRC_RunTimeOwner::create_object(){

    /*The list of attributes*/
    vector<ACS_CC_ValuesDefinitionType> AttrList;

    /*the attributes*/
    ACS_CC_ValuesDefinitionType attributeRDN;
    ACS_CC_ValuesDefinitionType attribute_NodeState_A;
    ACS_CC_ValuesDefinitionType attribute_NodeState_B;
    ACS_CC_ValuesDefinitionType attribute_RunLevel_A;
    ACS_CC_ValuesDefinitionType attribute_RunLevel_B;


    /*Fill the rdn Attribute */
    attributeRDN.attrName = const_cast<char*>("apNodeInfoId");
    attributeRDN.attrType = ATTR_STRINGT;
    attributeRDN.attrValuesNum = 1;

	string p_local_node_id;
	ifstream ifs;
	string local_id_path = "/etc/cluster/nodes/this/id";

	ifs.open(local_id_path.c_str());
	if (ifs.good())
		getline(ifs, p_local_node_id);
	ifs.close();

	void* valueRDN[1] = {0};

	if ( p_local_node_id == "1" ){
	    valueRDN[0] = reinterpret_cast<void*>(const_cast<char*>("apNodeInfoId=1"));
	}
	else {
	    valueRDN[0] = reinterpret_cast<void*>(const_cast<char*>("apNodeInfoId=2"));
	}

    attributeRDN.attrValues = valueRDN;

    /*Fill the attribute ATTRIBUTE_2*/
    attribute_NodeState_A.attrName = const_cast<char*>("nodeStateA");
    attribute_NodeState_A.attrType =ATTR_INT32T;
    attribute_NodeState_A.attrValuesNum = 0;
    attribute_NodeState_A.attrValues = 0;

    /*Fill the attribute ATTRIBUTE_2*/
    attribute_NodeState_B.attrName = const_cast<char*>("nodeStateB");
    attribute_NodeState_B.attrType =ATTR_INT32T;
    attribute_NodeState_B.attrValuesNum = 0;
    attribute_NodeState_B.attrValues = 0;

    /*Fill the attribute ATTRIBUTE_2*/
    attribute_RunLevel_A.attrName = const_cast<char*>("nodeRunLevelA");
    attribute_RunLevel_A.attrType =ATTR_INT32T;
    attribute_RunLevel_A.attrValuesNum = 0;
    attribute_RunLevel_A.attrValues = 0;

    /*Fill the attribute ATTRIBUTE_2*/
    attribute_RunLevel_B.attrName = const_cast<char*>("nodeRunLevelB");
    attribute_RunLevel_B.attrType =ATTR_INT32T;
    attribute_RunLevel_B.attrValuesNum = 0;
    attribute_RunLevel_B.attrValues = 0;

    AttrList.push_back(attributeRDN);
    AttrList.push_back(attribute_NodeState_A);
    AttrList.push_back(attribute_NodeState_B);
    AttrList.push_back(attribute_RunLevel_A);
    AttrList.push_back(attribute_RunLevel_B);

    //OmHandler prc_OmHandler;
    //vector<string> tmp_vector;

    //prc_OmHandler.Init();

    //prc_OmHandler.getClassInstances("ProcessControl",tmp_vector);

    //prc_OmHandler.Finalize();

    //ACS_CC_ReturnType apgcc_error = createRuntimeObj("ApNodeInfo", tmp_vector[0].c_str(), AttrList);

    ACS_CC_ReturnType apgcc_error = createRuntimeObj("ApNodeInfo", "processControlId=1", AttrList);		//Fix for TR HY60796

    return apgcc_error;
}
