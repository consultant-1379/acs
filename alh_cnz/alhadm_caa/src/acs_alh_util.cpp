/*
 * acs_alh_util.cpp
 *
 *  Created on: Oct 31, 2011
 *      Author: efabron
 */
#include <assert.h>
#include <poll.h>


#include <cstdarg>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "acs_alh_util.h"
#include "acs_alh_macroconfig.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_CS_API.h"


#define ACS_ALH_TRACE_MESSAGE_SIZE_MAX 4096
#define ACS_ALH_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))


//========================================================================================
//	Constructor
//========================================================================================

acs_alh_util::acs_alh_util(const char* class_name) : trace_obj_(class_name, "C"), log_()
{

}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_util::~acs_alh_util()
{
}


//========================================================================================
//  trace method
//========================================================================================
int acs_alh_util::trace (const char * format, ...){

	if (trace_obj_.ACS_TRA_ON()) {
		char trace_msg[ACS_ALH_TRACE_MESSAGE_SIZE_MAX];

		va_list argp;
		va_start(argp, format);
		vsnprintf(trace_msg, ACS_ALH_ARRAY_SIZE(trace_msg), format, argp);
		trace_msg[ACS_ALH_ARRAY_SIZE(trace_msg) - 1] = 0;
		va_end(argp);

		trace_obj_.ACS_TRA_event(1, trace_msg);
	}

	return 0;
}


//========================================================================================
//  get_alh_imm_root_dn method
//========================================================================================

int acs_alh_util::get_alh_imm_root_dn(const char * & dn_path){
	static int info_already_load = 0;
	static char alh_imm_root_dn[512] = {0};

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
		trace("ERROR: call 'om_handler.getClassInstances(...' failed: retrieving ALH parameters root from IMM: IMM error code == %d",
				om_handler.getInternalLastError());
	} else { //OK: Checking how much dn items was found in IMM
		if (dn_list.size() ^ 1) { //WARNING: 0 (zero) or more than one node found
			return_code = acs_alh::ERR_SAF_IMM_OM_GET;
			trace("WARNING: calling 'om_handler.getClassInstances(...': %s",
					dn_list.size() ? "more than one ALH parameters root node was found in IMM" : "ALH parameters root node not found in IMM");
		} else { //OK: Only one root node
			strncpy(alh_imm_root_dn, dn_list[0].c_str(), ACS_ALH_ARRAY_SIZE(alh_imm_root_dn));
			alh_imm_root_dn[ACS_ALH_ARRAY_SIZE(alh_imm_root_dn) - 1] = 0;
			info_already_load = 1;
			dn_path = alh_imm_root_dn;
		}
	}

	om_handler.Finalize();

	return return_code;
}


//========================================================================================
//  getlocalNodeName method
//========================================================================================

std::string acs_alh_util::getlocalNodeName(){

	trace("TRACE: acs_alh_util::getlocalNodeName() entered");

	char hostname[128] = {0};

	int res = gethostname( hostname, 128);

	if ( res == -1 ){

		trace("TRACE: acs_alh_util::getlocalNodeName() fails - exit");
		return "";
	}

	trace("TRACE: acs_alh_util::getlocalNodeName() exit");
	return hostname;

}


//========================================================================================
//  getLastTimeServiceStart method
//========================================================================================

int acs_alh_util::get_last_alh_start_time(char *p_lastStartTime){

	int ret_code = 0;

	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;

	const char * imm_alh_root_dn = 0;
	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

	/*Searching the parent name of the class to be defined*/
	ret_code = get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_last_alh_start_time - Call to 'get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d");
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS)
		return acs_alh::ERR_SAF_IMM_OM_INIT;


	/*The DN name of the parent of object to be defined*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1,
				ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN"=%s,%s", getlocalNodeName().c_str(), imm_alh_root_dn);


	ACS_CC_ImmParameter lastStartTime;

	ifstream ifs;
	string id_name;

	ifs.open("/etc/cluster/nodes/this/id");

	if (ifs.good())
		getline(ifs, id_name);
	else{
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_last_alh_start_time - Impossible to read information from file /etc/cluster/nodes/this/id");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_last_alh_start_time - exit - Returning -1");
		trace("acs_alh_util::set_last_alh_start_time - exit - Returning -1");
		return -1;
	}

	ifs.close();

	if(strcmp(id_name.c_str(), "1") == 0)
		lastStartTime.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_LAST_STARTED_TIME_NODE_1);

	if(strcmp(id_name.c_str(), "2") == 0)
		lastStartTime.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_LAST_STARTED_TIME_NODE_2);





	ret_code = om_handler.getAttribute(imm_alh_root_dn, &lastStartTime );
	if(ret_code != ACS_CC_SUCCESS){
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_last_alh_start_time - Call to getAttribute %s failed returnCode =%d", lastStartTime.attrName, ret_code);
		ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		return ret_code;
	}

	if(lastStartTime.attrValuesNum == 1){
		strncpy(p_lastStartTime, reinterpret_cast<char *>(lastStartTime.attrValues[0]), ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX);
	}
	//strncpy(p_lastStartTime, reinterpret_cast<char *>(lastStartTime.attrValues[0]), ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX);


	om_handler.Finalize();

	return ret_code;
}



//========================================================================================
//  set_last_alh_start_time method
//========================================================================================

int acs_alh_util::set_last_alh_start_time(char *p_lastStartTime){

	int ret_code = 0;

	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;

	const char * imm_alh_root_dn = 0;
//	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

	/*Searching the parent name of the class to be defined*/
	ret_code = get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_last_alh_start_time - Call to 'get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d");
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS)
		return acs_alh::ERR_SAF_IMM_OM_INIT;


//	/*The DN name of the parent of object to be defined*/
//	snprintf(imm_dnName, sizeof(imm_dnName) - 1,
//				ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN"=%s,%s", getlocalNodeName().c_str(), imm_alh_root_dn);

	//printf("DN of object: %s\n", imm_alh_root_dn);

	ACS_CC_ImmParameter lastStartTime;

	ifstream ifs;
	string id_name;

	ifs.open("/etc/cluster/nodes/this/id");

	if (ifs.good())
		getline(ifs, id_name);
	else{
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_last_alh_start_time - Impossible to read information from file /etc/cluster/nodes/this/id");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_last_alh_start_time - exit - Returning -1");
		trace("acs_alh_util::set_last_alh_start_time - exit - Returning -1");
		return -1;
	}

	ifs.close();

	if(strcmp(id_name.c_str(), "1") == 0)
		lastStartTime.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_LAST_STARTED_TIME_NODE_1);

	if(strcmp(id_name.c_str(), "2") == 0)
		lastStartTime.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_LAST_STARTED_TIME_NODE_2);


	lastStartTime.attrType = ATTR_STRINGT;
	lastStartTime.attrValuesNum = 1;
	void *valueAttr[1] = {reinterpret_cast<void*>(p_lastStartTime)};
	lastStartTime.attrValues = valueAttr;


	ret_code = om_handler.modifyAttribute(imm_alh_root_dn, &lastStartTime);
	if(ret_code != ACS_CC_SUCCESS){
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_last_alh_start_time - Call to modifyAttribute(%s, %s) failed returnCode =%d", imm_alh_root_dn, p_lastStartTime, ret_code);
		ret_code = acs_alh::ERR_SAF_IMM_OM_MODIFY;
		return ret_code;
	}


	om_handler.Finalize();

	return ret_code;


}

//========================================================================================
//	Find and replace in a string
//========================================================================================
void acs_alh_util::myReplace(std::string &str, const std::string &oldStr, const std::string &newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

//========================================================================================
//  get_AP_node_number method
//========================================================================================
int acs_alh_util::get_AP_node_number(int &p_apNodeNumber){


	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;
	int ret_code = 0;


	char dn_object[512] = {0};

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS)
		return acs_alh::ERR_SAF_IMM_OM_INIT;

	std::vector<std::string> dn_list;
	//int return_code = 0;

	if ((imm_call_result = om_handler.getClassInstances(ACS_ALH_CONFIG_IMM_APZ_FUNCTIONS_CLASS_NAME, dn_list)) ^ ACS_CC_SUCCESS) { //ERROR
		//return_code = acs_alh::ERR_SAF_IMM_OM_GET;
		trace("ERROR: call 'om_handler.getClassInstances(...' failed: retrieving ALH parameters root from IMM: IMM error code == %d",
				om_handler.getInternalLastError());
	} else { //OK: Checking how much dn items was found in IMM
		if (dn_list.size() ^ 1) { //WARNING: 0 (zero) or more than one node found
			//return_code = acs_alh::ERR_SAF_IMM_OM_GET;
			trace("WARNING: calling 'om_handler.getClassInstances(...': %s",
					dn_list.size() ? "more than one ALH parameters root node was found in IMM" : "ALH parameters root node not found in IMM");
		} else { //OK: Only one root node
			strncpy(dn_object, dn_list[0].c_str(), ACS_ALH_ARRAY_SIZE(dn_object));
			dn_object[ACS_ALH_ARRAY_SIZE(dn_object) - 1] = 0;
		}
	}



	ACS_CC_ImmParameter apNodeNumber;
	apNodeNumber.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_AP_NODE_NUMBER_ATTR_NAME);

	ret_code = om_handler.getAttribute(dn_object, &apNodeNumber );
	if(ret_code != ACS_CC_SUCCESS){
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_last_alh_start_time - Call to getAttribute %s failed returnCode =%d", apNodeNumber.attrName, ret_code);
		ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		return ret_code;
	}



	p_apNodeNumber = *(reinterpret_cast<char *>(apNodeNumber.attrValues[0]));



	om_handler.Finalize();

	return ret_code;


}


//========================================================================================
//  get_cluster_name method
//========================================================================================
int acs_alh_util::get_cluster_name(char *p_clusterName)
{
	ACS_CS_API_Name NetworkElementID;
	ACS_CS_API_NS::CS_API_Result ret_code;

	ret_code = ACS_CS_API_NetworkElement::getNEID(NetworkElementID);
	if (ret_code == ACS_CS_API_NS::Result_Success)
	{
		size_t length = ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX;
		NetworkElementID.getName(p_clusterName, length);
	}

	return ret_code;
}
//int acs_alh_util::get_cluster_name(char *p_clusterName){
//
//	OmHandler om_handler;
//	int ret_code = 0;
//	char dn_object[512] = {0};
//	std::vector<std::string> dn_list;
//
//	if ((ret_code = om_handler.Init()) ^ ACS_CC_SUCCESS)
//		return acs_alh::ERR_SAF_IMM_OM_INIT;
//
//
//	if ((ret_code = om_handler.getClassInstances(ACS_ALH_CONFIG_IMM_MANAGED_ELEMENT_CLASS_NAME, dn_list)) ^ ACS_CC_SUCCESS) { //ERROR
//		ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
//		trace("ERROR: call 'om_handler.getClassInstances(...' failed: retrieving ALH parameters root from IMM: IMM error code == %d",
//				om_handler.getInternalLastError());
//	} else { //OK: Checking how much dn items was found in IMM
//		if (dn_list.size() ^ 1) { //WARNING: 0 (zero) or more than one node found
//			ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
//			trace("WARNING: calling 'om_handler.getClassInstances(...': %s",
//					dn_list.size() ? "more than one ALH parameters root node was found in IMM" : "ALH parameters root node not found in IMM");
//		} else { //OK: Only one root node
//			strncpy(dn_object, dn_list[0].c_str(), ACS_ALH_ARRAY_SIZE(dn_object));
//			dn_object[ACS_ALH_ARRAY_SIZE(dn_object) - 1] = 0;
//		}
//	}
//
//	//printf("get_cluster_name: DN of object: %s\n", dn_object);
//
//	//Get from IMM the value of networkManagedElementId attribute
//	ACS_CC_ImmParameter networkElement;
//	networkElement.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_NETWORK_MANAGED_ELEMENT_ID_ATTR_NAME);
//
//	ret_code = om_handler.getAttribute(dn_object, &networkElement );
//	if(ret_code != ACS_CC_SUCCESS){
//		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_last_alh_start_time - Call to getAttribute %s failed returnCode =%d", networkElement.attrName, ret_code);
//		ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
//		return ret_code;
//	}
//
//	//Check if the attribute is empty
//	if(networkElement.attrValuesNum == 1){
//		strncpy(p_clusterName, reinterpret_cast<char *>(networkElement.attrValues[0]), ACS_ALH_CONFIG_CLUSTER_NAME_SIZE_MAX);
//		//printf("get_cluster_name: value of networkManagedElementId present: %s", p_clusterName);
//	}else{
//		//printf("get_cluster_name: value of networkManagedElementId not present");
//		//printf("get_cluster_name: try to get value of managedElementId attribute");
//
//		//Get from IMM the value of managedElementId attribute
//		ACS_CC_ImmParameter managedElement;
//		managedElement.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_MANAGED_ELEMENT_ID_ATTR_NAME);
//
//		ret_code = om_handler.getAttribute(dn_object, &managedElement );
//		if(ret_code != ACS_CC_SUCCESS){
//			log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_last_alh_start_time - Call to getAttribute %s failed returnCode =%d", managedElement.attrName, ret_code);
//			ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
//			return ret_code;
//		}
//
//		if(managedElement.attrValuesNum == 1){
//			strncpy(p_clusterName, reinterpret_cast<char *>(managedElement.attrValues[0]), ACS_ALH_CONFIG_CLUSTER_NAME_SIZE_MAX);
//			//printf("get_cluster_name: value of managedElementId present: %s", p_clusterName);
//		}else{
//			//printf("get_cluster_name: value of managedElementId not present");
//		}
//
//	}
//
//
//	om_handler.Finalize();
//
//	return ret_code;
//}




//========================================================================================
//  set_cp_alignment_flag method - ADD for TR HP54702
//========================================================================================

int acs_alh_util::set_cp_alignment_flag(int *flags){

	int ret_code = 0;

	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;

	const char * imm_alh_root_dn = 0;


	/*Searching the parent name of the class to be defined*/
	ret_code = get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_cp_alignment_flag - Call to 'get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d");
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS)
		return acs_alh::ERR_SAF_IMM_OM_INIT;


	ACS_CC_ImmParameter cpAlignmentFlag;

	ifstream ifs;
	string id_name;

	ifs.open("/etc/cluster/nodes/this/id");

	if (ifs.good())
		getline(ifs, id_name);
	else{
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_cp_alignment_flag - Impossible to read information from file /etc/cluster/nodes/this/id");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_cp_alignment_flag - exit - Returning -1");
		trace("acs_alh_util::set_cp_alignment_flag - exit - Returning -1");
		return -1;
	}

	ifs.close();

	if(strcmp(id_name.c_str(), "1") == 0)
		cpAlignmentFlag.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALN_CP_ALIGNMENT_FOR_NODE_1);

	if(strcmp(id_name.c_str(), "2") == 0)
		cpAlignmentFlag.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALN_CP_ALIGNMENT_FOR_NODE_2);


	cpAlignmentFlag.attrType = ATTR_INT32T;
	cpAlignmentFlag.attrValuesNum = 2;
	void *valueAttr[2] = {reinterpret_cast<void*>(&(flags[0])), reinterpret_cast<void*>(&(flags[1]))};
	cpAlignmentFlag.attrValues = valueAttr;


	ret_code = om_handler.modifyAttribute(imm_alh_root_dn, &cpAlignmentFlag);
	if(ret_code != ACS_CC_SUCCESS){
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::set_cp_alignment_flag - Call to modifyAttribute(%s, %s) failed returnCode =%d", imm_alh_root_dn, cpAlignmentFlag.attrName, ret_code);
		ret_code = acs_alh::ERR_SAF_IMM_OM_MODIFY;
		return ret_code;
	}


	om_handler.Finalize();

	return ret_code;

}


//========================================================================================
//  get_cp_alignment_flag method - ADD for TR HP54702
//========================================================================================
int acs_alh_util::get_cp_alignment_flag(int *flag){

	int ret_code = 0;

	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;

	const char * imm_alh_root_dn = 0;
	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

	/*Searching the parent name of the class to be defined*/
	ret_code = get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_cp_alignment_flag - Call to 'get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d");
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS)
		return acs_alh::ERR_SAF_IMM_OM_INIT;


	/*The DN name of the parent of object to be defined*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1,
				ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN"=%s,%s", getlocalNodeName().c_str(), imm_alh_root_dn);


	ACS_CC_ImmParameter cpAlignmentFlag;

	ifstream ifs;
	string id_name;

	ifs.open("/etc/cluster/nodes/this/id");

	if (ifs.good())
		getline(ifs, id_name);
	else{
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_cp_alignment_flag - Impossible to read information from file /etc/cluster/nodes/this/id");
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_cp_alignment_flag - exit - Returning -1");
		trace("acs_alh_util::get_cp_alignment_flag - exit - Returning -1");
		return -1;
	}

	ifs.close();

	if(strcmp(id_name.c_str(), "1") == 0)
		cpAlignmentFlag.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALN_CP_ALIGNMENT_FOR_NODE_1);

	if(strcmp(id_name.c_str(), "2") == 0)
		cpAlignmentFlag.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALN_CP_ALIGNMENT_FOR_NODE_2);


	ret_code = om_handler.getAttribute(imm_alh_root_dn, &cpAlignmentFlag );
	if(ret_code != ACS_CC_SUCCESS){
		log_.write(LOG_LEVEL_ERROR, "acs_alh_util::get_cp_alignment_flag - Call to getAttribute %s failed returnCode =%d", cpAlignmentFlag.attrName, ret_code);
		ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		return ret_code;
	}


	if(cpAlignmentFlag.attrValuesNum == 2){
		flag[0] = *reinterpret_cast<int *>(cpAlignmentFlag.attrValues[0]);
		flag[1] = *reinterpret_cast<int *>(cpAlignmentFlag.attrValues[1]);
	}


	om_handler.Finalize();

	return ret_code;
}

bool acs_alh_util::isAlhServerRunning( std::string local_node ){

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

    if(om_handler.getObject(&imm_object)){
    	om_handler.Finalize();
    	return false;
    }

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





