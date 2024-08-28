/*
 * ACS_TRAPDS_Imm_Util.h
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_IMM_UTIL_H_
#define ACS_TRAPDS_IMM_UTIL_H_


#include <string>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "ACS_APGCC_OiHandler.h"
#include "ACS_CC_Types.h"
#include "ACS_APGCC_Util.H"
#include "acs_apgcc_omhandler.h"

namespace IMM_TRAPDS_Util
{

	//CLASS NAME
	static const char classTrapSubscriber[] = "TrapSubscriber";
	static const char classTrapDispatcherService[] = "TrapDispatcherService";

	//CLASS IMPLEMENTERS
	static const char IMPLEMENTER_TRAPSUBSCRIBER[] = "ACS_TRAPDS";
	static const char IMPLEMENTER_TRAPSERVICE[] = "ACS_TRAPDS_Service";

	//ApzFunction CLASS
	static const char DN_APZ[] = "axeFunctionsId=1";
	static const char ATT_APZ_RDN[] = "axeFunctionsId";
	static const char ATT_APZ_SYSTEM[] = "systemType";
	static const char ATT_APZ_CBA[] = "apgShelfArchitecture";

	//TrapDispatcherService
	static const char ATT_TRAPDS_PARENT_RDN[] = "trapDispatcherServiceId=1";
	static const char ATT_TRAPDS_STATUS[] = "serviceState";

	//TrapSubscriber CLASS
	static const char ATT_TRAPDS_KEY[] = "trapSubscriberId";
	static const char ATT_TRAPDS_NAME[] = "name";
	static const char ATT_TRAPDS_PORT[] = "port";
	static const char ATT_TRAPDS_RDN[] = "trapSubscriberId=";

	enum NodeArchitecture {

		EGEM_SCB_RP	 = 0,
		EGEM2_SCX_SA = 1
	};

	enum SystemType {

		SINGLE_CP_SYSTEM = 0,
		MULTI_CP_SYSTEM  = 1
	};

	//METHODS
	bool getImmAttributeString (std::string object, std::string attribute, std::string &value);
	bool getImmAttributeInt(std::string object, std::string attribute, int &value);
	bool getChildrenObject(std::string dn, std::vector<std::string> & list );
	bool getClassObjectsList(std::string className, std::vector<std::string> & list );
	bool deleteImmObject(std::string dn);
	bool getObject(std::string dn, ACS_APGCC_ImmObject *object);
	void printDebugAllObjectClass (std::string className);
	void printDebugObject (std::string dn);
	bool getDnParent (std::string object_dn, std::string &parent);
	bool getRdnObject (std::string object_dn, std::string &rdn);
	bool getRdnAttribute (std::string object_dn, std::string &rdn);
	ACS_CC_ValuesDefinitionType defineAttributeString(const char* attributeName, ACS_CC_AttrValueType type, const char *value, unsigned int numValue);
	ACS_CC_ValuesDefinitionType defineAttributeInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue);

	ACS_CC_ImmParameter defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue);
	ACS_CC_ImmParameter defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue);
	bool modify_OM_ImmAttr(const char *object, ACS_CC_ImmParameter parameter);
	bool createClassTrapSubscriber();

}; // End of namespace


#endif /* ACS_TRAPDS_IMM_UTIL_H_ */
