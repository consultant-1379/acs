/*
 * ACS_CS_LogicalMgmtHandler.cpp
 *
 *  Created on: 08/mag/2012
 *      Author: renato
 */

#include "ACS_CS_LogicalMgmtHandler.h"




ACS_CS_LogicalMgmtHandler::ACS_CS_LogicalMgmtHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
		ACS_APGCC_CcbHandle &ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal)
{
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->className=0;
	this->parentName=0;
	this->attr=attr;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->base=base;

}



ACS_CS_LogicalMgmtHandler::~ACS_CS_LogicalMgmtHandler() {
	// TODO Auto-generated destructor stub

	std::cout<<"DISTRUTTORE ACS_CS_LogicalMgmtHandler\n";

}



int ACS_CS_LogicalMgmtHandler::create()
{

	if (base->type != CPBLADE_T && base->type != APBLADE_T)
		return 0;

	string LmRdn = ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT;
	string lmClass = ACS_CS_ImmMapper::CLASS_LOGICAL;

	ACS_CS_ImBase *baseLm = 0;
	bool LmFound = false;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseLm = subset->getObject(LmRdn.c_str());

	if (baseLm)
		return 0;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseLm = model->getObject(LmRdn.c_str());
		if (baseLm)
			LmFound = true;
	}

	if (LmFound)
		return 0;
	else {
		ACS_CC_ReturnType modResult;
		ACS_CS_ImLogicalMgmt* lm = new ACS_CS_ImLogicalMgmt();

		lm->rdn = LmRdn;
		lm->axeLogicalMgmtId = ACS_CS_ImmMapper::ATTR_LOGICAL_ID + "=1";

		ACS_CS_ImImmObject *immObject = lm->toImmObject();

		vector<ACS_CC_ValuesDefinitionType> lmAttr;

		vector<ACS_CS_ImValuesDefinitionType> lmAttrTemp = immObject->getAttributes();


		for (unsigned int j = 0; j < lmAttrTemp.size(); j++) {
			lmAttr.push_back((ACS_CC_ValuesDefinitionType) lmAttrTemp[j]);
		}

		delete lm;

		delete immObject;

		//Add AxeApg object to IMM transaction
		modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, lmClass.c_str(), ACS_CS_ImmMapper::RDN_EQUIPMENT.c_str(), lmAttr);

		if (modResult != ACS_CC_SUCCESS)
			return 1;

		ACS_APGCC_AttrValues **lmAttrVal = new ACS_APGCC_AttrValues*[lmAttrTemp.size() + 2];

		for (unsigned int j = 0; j < lmAttrTemp.size(); j++) {
			lmAttrVal[j] = new ACS_APGCC_AttrValues;
			lmAttrVal[j]->attrName = lmAttrTemp[j].getAttrName();
			lmAttrVal[j]->attrType = lmAttrTemp[j].getAttrType();
			lmAttrVal[j]->attrValuesNum = lmAttrTemp[j].getAttrValuesNum();
			lmAttrVal[j]->attrValues = lmAttrTemp[j].getAttrValues();
		}

		lmAttrVal[lmAttrTemp.size()] = new ACS_APGCC_AttrValues;
		lmAttrVal[lmAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		lmAttrVal[lmAttrTemp.size()]->attrType = ATTR_STRINGT;
		lmAttrVal[lmAttrTemp.size()]->attrValuesNum = 1;
		void *className[1] ={reinterpret_cast<void*> (const_cast<char*> (lmClass.c_str())) };
		lmAttrVal[lmAttrTemp.size()]->attrValues = className;

		lmAttrVal[lmAttrTemp.size()+1] = NULL;

		//Add AxeApg object to internal transaction
		ACS_CS_ImBase *object = ACS_CS_ImRepository::instance()->createObject(ccbId, lmClass.c_str(), ACS_CS_ImmMapper::RDN_EQUIPMENT.c_str(), lmAttrVal);

		for (unsigned int j = 0; j < lmAttrTemp.size()+1; j++) {
			delete lmAttrVal[j];
		}

		delete [] lmAttrVal;

		if (!object)
			return 1;
	}


	return 0;

}

int ACS_CS_LogicalMgmtHandler::modify()
{

	return 0;
}

int ACS_CS_LogicalMgmtHandler::remove()
{

	return 0;

}
