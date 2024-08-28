#ifndef RUNTIMEOBJECTHANDLER_H_
#define OBJECTHANDLER_H_

#include "ACS_APGCC_RuntimeOwner.h"
#include "ACS_CC_Types.h"
#include <iostream>

class RuntimeObjectHandler : public ACS_APGCC_RuntimeOwner {

public:

	RuntimeObjectHandler();

	inline ~RuntimeObjectHandler(){}

	/**Implementation of virtual method **/
	ACS_CC_ReturnType updateCallback(const char* p_objName, const char* p_attrName);


};


#endif /* RUNTIMEOBJECTHANDLER_H_ */
