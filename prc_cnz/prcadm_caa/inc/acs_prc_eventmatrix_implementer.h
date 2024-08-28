#ifndef ACS_PRC_EVENTMATRIXIMPLEMENTER_H_
#define ACS_PRC_EVENTMATRIXIMPLEMENTER_H_

#include <iostream>
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_adminoperationtypes.h"

class acs_prc_eventmatrix_implementer: public acs_apgcc_objectimplementerinterface_V3 {
public:

	acs_prc_eventmatrix_implementer();

	virtual ~acs_prc_eventmatrix_implementer();

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType**paramList);

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);

};

#endif /* ACS_PRC_EVENTMATRIXIMPLEMENTER_H_ */
