#include "ACS_DSD_Utils.h"
#include "ace/Time_Value.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Server.h"
#include "ACS_DSD_ConfigParams.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace  ACS_DSD_Utils
{

int get_node_info(ACS_DSD_Node & node_info, std::string & error_descr)
{
	ACS_DSD_Server svr;
	int op_res = svr.get_local_node(node_info);
	if(op_res < 0)
		error_descr = svr.last_error_text();
	return op_res;
}

int get_partner_node_info(ACS_DSD_Node & node_info, std::string & error_descr)
{
	ACS_DSD_Server svr;
	int op_res = svr.get_partner_node(node_info);
	if(op_res < 0)
		error_descr = svr.last_error_text();
	return op_res;
}

std::string get_AP_name(char CP_APidentity)
{
	char nodeLetter=(0x0f & CP_APidentity) + char('A');
	char nodeNumber=(0xf0 & CP_APidentity) >> 4;
	char apn[16];
	sprintf(apn,"AP%d%c",(int)nodeNumber,nodeLetter);
	return apn;
}

acs_dsd::SystemTypeConstants get_system_type(int32_t system_id)
{
	acs_dsd::SystemTypeConstants system_type = acs_dsd::SYSTEM_TYPE_UNKNOWN;

	if (((acs_dsd::CONFIG_AP_ID_SET_START <= system_id)
				&& (system_id <= acs_dsd::CONFIG_AP_ID_SET_END))  ||
					(system_id == acs_dsd::SYSTEM_ID_FRONT_END_AP)||
					(system_id == acs_dsd::SYSTEM_ID_PARTNER_NODE)||
					(system_id == acs_dsd::SYSTEM_ID_THIS_NODE)) system_type = acs_dsd::SYSTEM_TYPE_AP;
	else if ((acs_dsd::CONFIG_BC_ID_SET_START <= system_id)
				&& (system_id <= acs_dsd::CONFIG_BC_ID_SET_END)) system_type = acs_dsd::SYSTEM_TYPE_BC;
		 else if(((acs_dsd::CONFIG_CP_ID_SET_START <= system_id)
				 && (system_id <= acs_dsd::CONFIG_CP_ID_SET_END))||
					(system_id == acs_dsd::SYSTEM_ID_CP_ALARM_MASTER)||
					(system_id == acs_dsd::SYSTEM_ID_CP_CLOCK_MASTER)) system_type = acs_dsd::SYSTEM_TYPE_CP;
	return system_type;
}

// This method retrieves an AP node Name from the Cluster specified by system_id
int get_ApNodesName(int32_t system_id, char node_name[][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX])
{
	ACS_DSD_ConfigurationHelper::HostInfo node_info;

	srand(time(0));
	int index = rand() % acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED;

    int ret_code = ACS_DSD_ConfigurationHelper::get_node_by_side (system_id, acs_dsd::NODE_SIDE_A, node_info);
    if(ret_code != acs_dsd::ERR_NODE_NOT_FOUND)
    		strncpy(node_name[index],node_info.node_name,acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);

	index = (index + 1) % acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED;

    ret_code = ACS_DSD_ConfigurationHelper::get_node_by_side (system_id, acs_dsd::NODE_SIDE_B, node_info);
    if(ret_code != acs_dsd::ERR_NODE_NOT_FOUND)
        		strncpy(node_name[index],node_info.node_name,acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);

    return ret_code;
}

int get_ip_addr_str(uint32_t ip_addr, char *buff, int len)
{
	if(*buff || (len < INET_ADDRSTRLEN))
		return -1;

	in_addr inet_addr;
	inet_addr.s_addr = ip_addr;

	const char *p = inet_ntop(AF_INET, & inet_addr, buff, len);
	return (p ? 0: -1);
}


int clean_dsd_saps(const char * saps_path)
{
	// try to open the directory having pathname <saps_path>
	DIR *dp;
	struct dirent *ep;
	if( (dp = opendir(saps_path)) == NULL )
		return -1;

	// iterate on directory items
	while((ep = readdir(dp)) != NULL)
	{
		if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..") && (strstr(ep->d_name, DSD_UNIX_SAP_NAME_PREFIX) != NULL) )
		{
			// the current directory item starts with the string <DSD_UNIX_SAP_NAME_PREFIX>, delete it !
			char file_path_name[1024] = {0};
			snprintf(file_path_name, 1024, "%s%s", ACS_DSD_ConfigurationHelper::unix_socket_root_path(), ep->d_name);
			file_path_name[1023] = '\0';
			unlink(file_path_name);		// ignore the result and go ahead
		}
	}

	// close directory and exit
	closedir(dp);
	return 0;
}

} // end namespace
