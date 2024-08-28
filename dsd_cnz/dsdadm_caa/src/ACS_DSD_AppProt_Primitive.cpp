#include <vector>

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_MacrosUtils.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_AppProt_Primitive.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_Logger dsd_logger;


int AppProt_Primitive::notify_to_all_AP() const
{
	unsigned ap_nodes_count = ACS_DSD_ConfigurationHelper::system_ap_nodes_count();
	if (ap_nodes_count == 0){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_ConfigurationHelper::system_ap_nodes_count() returned < %d >"),ap_nodes_count);
		return NOTIFICATION_OK;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ConfigurationHelper::system_ap_nodes_count() returned < %d >"),ap_nodes_count);
	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t  node_info_ptr;
	int32_t ap_system_id;
	std::vector<int32_t> ap_notified_list;
	bool ap_already_notified = false;
	//Search in AP nodes
	int call_result, resultCode ;
	for (unsigned idx = 0; idx < ap_nodes_count; ++idx){
		if((call_result = ACS_DSD_ConfigurationHelper::get_ap_node(idx,node_info_ptr))) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Error in  get_ap_node(%u,..) <errcode = %d>"), idx, call_result);
			continue;
		}
		else{
			ap_system_id=node_info_ptr->system_id;
			std::vector<int32_t>::iterator it;
			for(it= ap_notified_list.begin(); it!= ap_notified_list.end(); it++){
				if(ap_system_id == *it) ap_already_notified = true;
			}
			// skip the local ap system and the ap system already notified
			if(ap_system_id == dsd_local_node.system_id || ap_already_notified == true)
				continue;
			if ((resultCode = notify_to_AP(ap_system_id))) {
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Notification to AP %d failed! <errcode = %d>"), ap_system_id, resultCode);
			}
			ap_notified_list.push_back(ap_system_id);
		}
	}
	return NOTIFICATION_OK;
}
