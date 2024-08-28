/*
 * MyObjectHandler.h
 *
 *  Created on: Jan 10, 2011
 *      Author: xpaomaz
 */

#ifndef MYOBJECTHANDLER_H_
#define MYOBJECTHANDLER_H_

#include "ACS_APGCC_RuntimeOwnerV2.h"
#include "ACS_CC_Types.h"
#include <iostream>

class MyObjectHandler : public ACS_APGCC_RuntimeOwnerV2 {
public:
	MyObjectHandler();
	//virtual ~MyObjectHandler();
	/**Implementation of virtual method **/
	ACS_CC_ReturnType updateCallback(const char* p_objName, const char* p_attrName);
};

#endif /* MYOBJECTHANDLER_H_ */
