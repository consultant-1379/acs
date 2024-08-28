/*
 * ACS_PRC_RunTimeOwner.h
 *
 *  Created on: Dec 28, 2010
 *      Author: xlucpet
 */

#ifndef ACS_PRC_RUNTIMEOWNER_H_
#define ACS_PRC_RUNTIMEOWNER_H_

#include "ACS_APGCC_RuntimeOwner.h"

class ACS_PRC_RunTimeOwner : public ACS_APGCC_RuntimeOwner {
public:
	ACS_PRC_RunTimeOwner();
	~ACS_PRC_RunTimeOwner();

	ACS_CC_ReturnType updateCallback(const char* p_objName, const char* p_attrName);

	ACS_CC_ReturnType create_object();

private:
	char* p_objName_private;
	string p_local_node_id_path;
	string p_remote_node_id_path;
	string p_local_node_hostname_path;
	string p_remote_node_hostname_path;
	string p_node_1_hostname_path;
	string p_node_2_hostname_path;
};

#endif /* ACS_PRC_RUNTIMEOWNER_H_ */
