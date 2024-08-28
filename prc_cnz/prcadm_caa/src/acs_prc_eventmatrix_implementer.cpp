#include "acs_prc_eventmatrix_implementer.h"
#include <vector>
#include <fstream>
#include <string>
#include <iterator>
#include "ace/SOCK_Connector.h"
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Connector.h>

acs_prc_eventmatrix_implementer::acs_prc_eventmatrix_implementer() {

}

acs_prc_eventmatrix_implementer::~acs_prc_eventmatrix_implementer() {

}

void acs_prc_eventmatrix_implementer::adminOperationCallback(ACS_APGCC_OiHandle /*oiHandle*/,
		ACS_APGCC_InvocationType /*invocation*/,
		const char* /*p_objName*/,
		ACS_APGCC_AdminOperationIdType /*operationId*/,
		ACS_APGCC_AdminOperationParamType** /*paramList*/) {

}

ACS_CC_ReturnType acs_prc_eventmatrix_implementer::create(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*className*/, const char* parentName, ACS_APGCC_AttrValues **attr){

	if ( strcmp ( parentName, "processControlId=1" ) != 0)
		return ACS_CC_FAILURE;

	int i = 0;

	while ( attr[i] != 0){
		if (( strcmp ( attr[i]->attrName, "eventAction" ) == 0 ) && ( attr[i]->attrValuesNum > 0 )) {
			std::string tmp = reinterpret_cast<char *>( attr[i]->attrValues[0]);
			if ( ( tmp == "failover") || ( tmp == "changeactivenode") || ( tmp == "restart") || ( tmp == "stop") )
				return ACS_CC_SUCCESS;
			else
				return ACS_CC_FAILURE;
		}
		i++;
	}

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_eventmatrix_implementer::deleted(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*objName*/){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_eventmatrix_implementer::modify(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*objName*/, ACS_APGCC_AttrModification **attrMods){

	int i = 0;
	while ( attrMods[i] != 0){
		if ( strcmp ( attrMods[i]->modAttr.attrName, "eventAction" ) == 0 ){
			std::string tmp = reinterpret_cast<char *>( attrMods[i]->modAttr.attrValues[0]);
			if ( ( tmp == "failover") || ( tmp == "changeactivenode") || ( tmp == "restart") || ( tmp == "stop") )
				return ACS_CC_SUCCESS;
			else
				return ACS_CC_FAILURE;
		}
		i++;
	}
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_eventmatrix_implementer::complete(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

	return ACS_CC_SUCCESS;
}

void acs_prc_eventmatrix_implementer::abort(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

}

void acs_prc_eventmatrix_implementer::apply(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

}

ACS_CC_ReturnType acs_prc_eventmatrix_implementer::updateRuntime(const char* /*p_objName*/, const char** /*p_attrName*/){

	return ACS_CC_SUCCESS;
}
