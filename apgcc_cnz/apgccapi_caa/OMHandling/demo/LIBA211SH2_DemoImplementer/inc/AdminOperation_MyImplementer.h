/*
 * AdminOperation_MyImplementer.h
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

#ifndef ADMINOPERATION_MYIMPLEMENTER_H_
#define ADMINOPERATION_MYIMPLEMENTER_H_
//#include "acs_apgcc_adminoperationimplementer.h"
#include "acs_apgcc_objectimplementerinterface_V2.h"

#include "ACS_CC_Types.h"

#include "acs_apgcc_adminoperationtypes.h"

#include <iostream>
class AdminOperation_MyImplementer: public acs_apgcc_objectimplementerinterface_V2 {
public:

	AdminOperation_MyImplementer();

	inline ~AdminOperation_MyImplementer(){};

	/*the callback*/

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
			ACS_APGCC_AdminOperationParamType**paramList);


	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_nameObj, const char* p_nameAttr);



};

#endif /* ADMINOPERATION_MYIMPLEMENTER_H_ */
