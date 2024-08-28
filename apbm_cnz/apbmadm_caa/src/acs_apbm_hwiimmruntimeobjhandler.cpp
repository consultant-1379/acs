/*
 * ACS_APBM_HWIImmRuntimeObjHandler.cpp
 *
 *  Created on: Nov 21, 2011
 *      Author: xlucdor
 */

#include "acs_apbm_hwiimmruntimeobjhandler.h"


ACS_CC_ReturnType ACS_APBM_HWIImmRuntimeObjHandler::updateCallback(
										const char* /*p_objName*/, const char** /*p_attrName*/){

	//Empty
	return ACS_CC_SUCCESS;

}

void ACS_APBM_HWIImmRuntimeObjHandler:: adminOperationCallback(
		ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_InvocationType /*invocation*/,
		const char* /*p_objName*/, ACS_APGCC_AdminOperationIdType /*operationId*/,
		ACS_APGCC_AdminOperationParamType** /*paramList*/) {

	//Empty
}
