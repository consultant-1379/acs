/*
 * MyImplementer.h
 *
 *  Created on: Sep 15, 2010
 *      Author: xfabron
 */

#ifndef MYIMPLEMENTER_H_
#define MYIMPLEMENTER_H_

#include "ACS_APGCC_ObjectImplementer_EventHandler.h"

#include <iostream>
#include <string>

class MyImplementer : public ACS_APGCC_ObjectImplementer_EventHandler {

public :

	inline MyImplementer(string p_impName ) : ACS_APGCC_ObjectImplementer_EventHandler(p_impName){};

	inline MyImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ) : ACS_APGCC_ObjectImplementer_EventHandler(p_objName, p_impName, p_scope){};

	~MyImplementer(){};

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);


};





#endif /* MYIMPLEMENTER_H_ */
