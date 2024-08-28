#ifndef MYIMPLEMENTER_H_
#define MYIMPLEMENTER_H_

#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_paramhandling.h"

class MyImplementer : public acs_apgcc_objectimplementerinterface_V3{
public :

	MyImplementer();

	MyImplementer(string p_impName );

	MyImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );

	~MyImplementer(){};
	int getLastMessageReceived();
	int ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode);

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_nameObj, const char** p_nameAttr);

	/*the callback*/

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
			ACS_APGCC_AdminOperationParamType**paramList);

	void setStatus(int status);

	int m_status;

	int lastMessageReceived;

};
#endif /* MYIMPLEMENTER_H_ */
