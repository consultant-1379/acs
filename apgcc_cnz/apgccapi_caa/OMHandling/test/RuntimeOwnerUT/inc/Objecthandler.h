#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_

#include "ACS_APGCC_RuntimeOwner.h"
#include "ACS_CC_Types.h"
#include <iostream>

class ObjectHandler : public ACS_APGCC_RuntimeOwner {

public:

	ObjectHandler();

	inline ~ObjectHandler(){}

	/**Implementation of virtual method **/
	ACS_CC_ReturnType updateCallback(const char* p_objName, const char* p_attrName);


};


#endif /* OBJECTHANDLER_H_ */
