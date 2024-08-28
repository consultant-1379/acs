/*
 * ACS_APBM_HWIImmHandler.h
 *
 *  Created on: Oct 25, 2011
 *      Author: xlucdor
 */


#ifndef ACS_APBM_HWIIMMCONFOBJHANDLER_H_
#define ACS_APBM_HWIIMMCONFOBJHANDLER_H_

//#include "ACS_APGCC_RuntimeOwner_V2.h"
#include <ACS_CC_Types.h>
#include <acs_apgcc_adminoperationtypes.h>
#include <acs_apgcc_omhandler.h>
#include "acs_apbm_hwidata.h"
#include <vector>

#include <acs_apgcc_objectimplementerinterface_V2.h>

const std::string IMPLEMENTER_NAME_CFG = "hwiImplementer";
const std::string IMPLEMENTER_NAME_PFM = "hwiImplementerPFM";
//1const std::string IMMCLASSNAME_HWISHELF = "ShelfInventory";
//7 const std::string IMMCLASSNAME_HWISHELF = "ShelfInfo";
const std::string IMMCLASSNAME_HWISHELF = "AxeHardwareInventoryShelfInfo";
//2const std::string IMMCLASSNAME_BOARD = "BoardInventory";
//7 const std::string IMMCLASSNAME_BOARD = "BladeInfo";
const std::string IMMCLASSNAME_BOARD = "AxeHardwareInventoryBladeInfo";

//3const std::string IMMCLASSNAME_PFM = "PowerFanModuleInventory";
//7 const std::string IMMCLASSNAME_PFM = "PowerFanModuleInfo";
const std::string IMMCLASSNAME_PFM = "AxeHardwareInventoryPowerFanModuleInfo";

const std::string IMMCLASSNAME_PRODUCTIDENTITY = "AxeHardwareInventoryHwProductIdentity";
//7 const std::string IMMCLASSNAME_HWI = "HardwareInventory";
const std::string IMMCLASSNAME_HWI = "AxeHardwareInventoryHardwareInventoryM";

class ACS_APBM_HWIImmConfObjHandler : public acs_apgcc_objectimplementerinterface_V2 {

public:

	ACS_APBM_HWIImmConfObjHandler();
	virtual ~ACS_APBM_HWIImmConfObjHandler();

	/*inheritance methods*/

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


	void setIMMOperationOnGoing(bool op = true){immOperationOnGoing = op;};

	void createIMMTables();

private:

	bool immOperationOnGoing;

	ACS_APBM_HWIData *m_HWIData;

	void hwiDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes);
	void shelfDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes);
	void boardDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes);
	void pfmDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes);

};

#endif /* ACS_APBM_HWIIMMHANDLER_H_ */
