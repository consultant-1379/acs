#ifndef ACS_PRC_OBSERVERIMPLEMENTER_H_
#define ACS_PRC_OBSERVERIMPLEMENTER_H_

#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "ACS_TRA_Logging.h"
#include "ace/Task.h"
#include <sys/epoll.h>
#include <string>

class acs_prc_observerImplementer: public acs_apgcc_objectimplementerinterface_V3, public ACE_Task_Base {
public:
	acs_prc_observerImplementer();
	virtual ~acs_prc_observerImplementer();

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType**paramList);

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);

	virtual int svc ( void );

	void stop(){ sleep = true; };

	void start(){ sleep = false; };

private :

	bool sleep, attribute_modified;
	std::string MEId_attribute;

};

#endif /* ACS_PRC_OBSERVERIMPLEMENTER_H_ */
