#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_

#include "ACS_APGCC_RuntimeOwner_V2.h"
#include <iostream>

class ObjectHandler : public ACS_APGCC_RuntimeOwner_V2 {

public:

	ObjectHandler();

	inline ~ObjectHandler(){}

	/**Implementation of virtual method **/
	ACS_CC_ReturnType updateCallback(const char* p_objName, const char** p_attrName);

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);


};


#endif /* OBJECTHANDLER_H_ */
