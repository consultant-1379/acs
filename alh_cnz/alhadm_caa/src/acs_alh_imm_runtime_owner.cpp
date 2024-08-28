/*
 * acs_alh_imm_util.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: efabron
 */

#include "acs_alh_imm_runtime_owner.h"
#include "acs_alh_macroconfig.h"
#include "acs_alh_imm_data_handler.h"


//========================================================================================
//	Constructor
//========================================================================================

acs_alh_imm_runtime_owner::acs_alh_imm_runtime_owner(acs_alh_imm_connection_handler *immConnHandler) : util_("acs_alh_imm_runtime_owner"), log_() {

	immConnHandlerObj_ = immConnHandler;
}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_imm_runtime_owner::~acs_alh_imm_runtime_owner()
{
}




int acs_alh_imm_runtime_owner::init_IMM(bool cleanImmStructure)
{

	util_.trace("TRACE: acs_alh_imm_runtime_owner.init_IMM() entered");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_imm_runtime_owner.init_IMM() entered");
	log_.write(LOG_LEVEL_DEBUG, "Connecting to OpenSAF IMM Service .... !");


	// register alhd with IMM service for AlarmInfo objects handling
	char alhd_imm_name[1024];
	std::string nodeName = util_.getlocalNodeName();
	snprintf(alhd_imm_name, sizeof(alhd_imm_name) - 1, "%s_%s", ALHD_IMM_PREFIX, nodeName.c_str());
	alhd_imm_name[sizeof(alhd_imm_name) - 1] = 0;


	log_.write(LOG_LEVEL_DEBUG, "Registering alhd with IMM using name < %s > .... !", alhd_imm_name);

	ACS_CC_ReturnType imm_result = immConnHandlerObj_->init(alhd_imm_name);
	if(imm_result != ACS_CC_SUCCESS)
	{
		log_.write(LOG_LEVEL_ERROR, "acs_alh_imm_connection_handler.init() failure ! return code is < %d >", imm_result);
		util_.trace("TRACE: acs_alh_imm_runtime_owner.init_IMM() exit. Returning -1");
		return -1;
	}

	log_.write(LOG_LEVEL_DEBUG, "alhd successfully registered with IMM using name < %s > !", alhd_imm_name);



	acs_alh_imm_data_handler immDataHandler(immConnHandlerObj_);

	// if requested, clean existing IMM entry for local node before proceeding
	if( cleanImmStructure )
	{
		log_.write(LOG_LEVEL_DEBUG, "alhd invoked with 'c' option or the AP node is rebooted: trying to remove IMM subtree (eventually) rooted at < alarmListNodeId=%s >", nodeName.c_str());

		char rdn_obj_name[1024];
		const char * imm_dnName = 0;

		/*Searching the parent name of the class to be defined*/
		if (int call_result = util_.get_alh_imm_root_dn(imm_dnName)) {
			log_.write(LOG_LEVEL_ERROR, "util_.get_alh_imm_root_dn(...) failure ! return code is < %d >", call_result);
			util_.trace("TRACE: acs_alh_imm_runtime_owner.init_IMM() exit. Returning -1");
			return -1;
		}

		snprintf(rdn_obj_name, sizeof(rdn_obj_name) - 1, "%s=%s,%s", ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, nodeName.c_str(), imm_dnName);
		int remove_node_res = immDataHandler.removeAlarmListNode(rdn_obj_name);
		if( remove_node_res == 0 ) {
			log_.write(LOG_LEVEL_DEBUG, "the IMM subtree rooted at < %s > has been successfully REMOVED!", rdn_obj_name);
		}else{
			log_.write(LOG_LEVEL_ERROR, "acs_alh_imm_data_handler::removeAlarmListNode( %s ) failure  ! return code is < %d >", rdn_obj_name, remove_node_res);
		}
	}

	// now create a AlarmListNode object for the current node
	log_.write(LOG_LEVEL_DEBUG, "Creating AlarmListNode: --- NODE.name = %s  !", nodeName.c_str());

	if( !cleanImmStructure )
	{
		// the node should be already existing ...
		char rdn_obj_name[1024];
		const char * imm_dnName = 0;

		/*Searching the parent name of the class to be defined*/
		if (int call_result = util_.get_alh_imm_root_dn(imm_dnName)) {
			log_.write(LOG_LEVEL_ERROR, "util_.get_alh_imm_root_dn(...) failure ! return code is < %d >", call_result);
			util_.trace("TRACE: acs_alh_imm_runtime_owner.init_IMM() exit. Returning -1");
			return -1;
		}


		snprintf(rdn_obj_name, sizeof(rdn_obj_name) - 1, "%s=%s,%s", ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, nodeName.c_str(),imm_dnName);
		int check_ret = immDataHandler.checkIfObjectIsPresent(rdn_obj_name, ACS_ALH_CONFIG_IMM_ALARM_LIST_NODE_CLASS_NAME);
		if(check_ret == 1){
			log_.write(LOG_LEVEL_DEBUG, "the AlarmListNode < %s > associated at current local node already EXIST!", rdn_obj_name);
			log_.write(LOG_LEVEL_TRACE, "acs_alh_imm_runtime_owner.init_IMM() exit");
			util_.trace("TRACE: acs_alh_imm_runtime_owner.init_IMM() exit");
			return 0;
		}else if(check_ret == 0){
			log_.write(LOG_LEVEL_ERROR, "No AlarmListNode < %s > associated at current local node is present in IMM, try to create it!",
					rdn_obj_name);
		}else{
			log_.write(LOG_LEVEL_ERROR, "Impossible to check if the AlarmListNode < %s > associated at current local node already exist, try to create it!",
					rdn_obj_name);
		}
	}


	int create_node_res = immDataHandler.createAlarmListNode(nodeName.c_str());
	if(create_node_res < 0)
	{
		log_.write(LOG_LEVEL_ERROR, "acs_alh_imm_data_handler::createAlarmListNode() failure  ! return code is < %d >", create_node_res);
		util_.trace("TRACE: acs_alh_imm_data_handler.init_IMM() exit. Returning -1");
		return -1;
	}
	log_.write(LOG_LEVEL_DEBUG, "AlarmListNode has been successfully created!");


	log_.write(LOG_LEVEL_TRACE, "acs_alh_imm_runtime_owner.init_IMM() exit");
	util_.trace("TRACE: acs_alh_imm_runtime_owner.init_IMM() exit");

	return 0;
}



int acs_alh_imm_runtime_owner::finalize_IMM(){

	util_.trace("TRACE: acs_alh_imm_runtime_owner.finalize_IMM() entered");
	log_.write(LOG_LEVEL_TRACE, "acs_alh_imm_runtime_owner.finalize_IMM() entered");

	log_.write(LOG_LEVEL_DEBUG, "Finalize alhd interaction with IMM.... !");

	ACS_CC_ReturnType imm_result = immConnHandlerObj_->finalize();
	if(imm_result != ACS_CC_SUCCESS)
	{
		log_.write(LOG_LEVEL_ERROR, "acs_alh_imm_connection_handler.finalize_IMM() failure ! return code is < %d >", imm_result);
		util_.trace("TRACE: acs_alh_imm_runtime_owner.finalize_IMM() exit. Returning -1");
		return -1;
	}

	log_.write(LOG_LEVEL_DEBUG, "alhd successfully finalize interaction with IMM !");

	log_.write(LOG_LEVEL_TRACE, "acs_alh_imm_runtime_owner.finalize_IMM() exit");
	util_.trace("TRACE: acs_alh_imm_runtime_owner.finalize_IMM() exit");

	return 0;

}
