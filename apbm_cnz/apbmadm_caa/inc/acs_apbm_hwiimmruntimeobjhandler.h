/*
 * ACS_APBM_HWIImmRuntimeObjHandler.h
 *
 *  Created on: Nov 21, 2011
 *      Author: xlucdor
 */

#ifndef ACS_APBM_HWIIMMRUNTIMEOBJHANDLER_H_
#define ACS_APBM_HWIIMMRUNTIMEOBJHANDLER_H_

#include <ACS_APGCC_RuntimeOwner_V2.h>

class ACS_APBM_HWIImmRuntimeObjHandler : public ACS_APGCC_RuntimeOwner_V2 {

	/**Implementation of virtual method **/
	ACS_CC_ReturnType updateCallback(const char* p_objName, const char** p_attrName);

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

};

#endif /* ACS_APBM_HWIIMMRUNTIMEOBJHANDLER_H_ */
