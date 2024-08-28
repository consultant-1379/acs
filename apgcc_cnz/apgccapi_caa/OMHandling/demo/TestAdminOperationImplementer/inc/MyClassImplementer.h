/*
 * MyClassImplementer.h
 *
 *  Created on: Dec 22, 2010
 *      Author: xpaomaz
 */

#ifndef MYCLASSIMPLEMENTER_H_
#define MYCLASSIMPLEMENTER_H_

#include "ACS_APGCC_ObjectImplementer_EventHandler.h"
#include "acs_apgcc_omhandler.h"
#include <iostream>
#include <string>


class MyClassImplementer : public ACS_APGCC_ObjectImplementer_EventHandler {
public:
	//MyClassImplementer();
	virtual ~MyClassImplementer();

public :
	

	inline MyClassImplementer(string p_impName ) : ACS_APGCC_ObjectImplementer_EventHandler(p_impName){};

	inline MyClassImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ) : ACS_APGCC_ObjectImplementer_EventHandler(p_objName, p_impName, p_scope){};



	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_nameObj, const char* p_nameAttr);


};

#endif /* MYCLASSIMPLEMENTER_H_ */
