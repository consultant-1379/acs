/*
 * ACS_TRAPDS_OI_TrapService.h
 *
 *  Created on: Mar 7, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_OI_TRAPSERVICE_H_
#define ACS_TRAPDS_OI_TRAPSERVICE_H_

#include "ACS_TRAPDS_Imm_Util.h"

#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"

#include <iostream>
#include <string>

/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class ACS_TRAPDS_OI_TrapService : public acs_apgcc_objectimplementerinterface_V3
{
 public:

	ACS_TRAPDS_OI_TrapService();


	virtual ~ACS_TRAPDS_OI_TrapService();


	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);


	virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);


	virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);


	virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);


	virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);


	virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);


	virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);


	virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);


	const char* getIMMClassName() const {return m_ImmClassName.c_str();};


 private:

	string m_ImmClassName;

};


#endif /* ACS_TRAPDS_OI_TRAPSERVICE_H_ */
