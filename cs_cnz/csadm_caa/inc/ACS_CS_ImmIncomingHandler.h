/*
 * ACSCSInCreateHandler.h
 *
 *  Created on: 07/mag/2012
 *      Author: renato
 */

#ifndef ACS_CS_ImmIncomingHandler_H_
#define ACS_CS_ImmIncomingHandler_H_

#include "acs_apgcc_objectimplementerinterface_V2.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "ACS_CS_Trace.h"

class ACS_CS_ImmIncomingHandler  : public acs_apgcc_objectimplementerinterface_V3 {


public:

	virtual int create()=0;

	virtual int remove()=0;

	virtual int modify()=0;

	virtual ~ACS_CS_ImmIncomingHandler()
	{

	}


	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle, ACS_APGCC_CcbId, const char*, const char*, ACS_APGCC_AttrValues**){
			return ACS_CC_SUCCESS;
		};
		virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle, ACS_APGCC_CcbId, const char*){return ACS_CC_SUCCESS;};
		virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle, ACS_APGCC_CcbId, const char*, ACS_APGCC_AttrModification**){return ACS_CC_SUCCESS;};
		virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle, ACS_APGCC_CcbId){return ACS_CC_SUCCESS;};
		virtual void abort(ACS_APGCC_OiHandle, ACS_APGCC_CcbId){};
		virtual void apply(ACS_APGCC_OiHandle, ACS_APGCC_CcbId){};
		virtual ACS_CC_ReturnType updateRuntime(const char*, const char**){return ACS_CC_SUCCESS;};
		virtual void adminOperationCallback(ACS_APGCC_OiHandle, ACS_APGCC_InvocationType, const char*, ACS_APGCC_AdminOperationIdType, ACS_APGCC_AdminOperationParamType**){};
};

#endif /* ACS_CS_ImmIncomingHandler_H_ */
