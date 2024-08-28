/*
 * ACS_CS_ApHandler.cpp
 *
 *  Created on: 07/mag/2012
 *      Author: renato
 */

#include "ACS_CS_ApHandler.h"

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImEntryIdHelper.h"


ACS_CS_ApHandler::ACS_CS_ApHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
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

ACS_CS_ApHandler::~ACS_CS_ApHandler()
{
	std::cout<<"DISTRUTTORE ACS_CS_ApHandler\n";
}


int ACS_CS_ApHandler::create()
{
	std::cout<<"create from ACS_CS_ApHandler\n";
	ACS_CS_DEBUG(("CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	uint16_t sysNo;
	std::string apName;
	string apClass = ACS_CS_ImmMapper::CLASS_APG;
	bool apFound = false;
	ACS_CS_ImBase *baseAp = 0;

	int architecture;
	if (!ACS_CS_Registry::getNodeArchitecture(architecture))
	{
		return 1;
	}

	if (base->type != APBLADE_T)
		return 0;

	ACS_CS_ImApBlade *apBlade = dynamic_cast<ACS_CS_ImApBlade *> (base);
	if (!apBlade)
		return 1;

	sysNo = apBlade->systemNumber;

	if (sysNo != 1 && sysNo!= 2) //Invalid system number, do nothing (validation will do the job)
		return 0;

	apName = ACS_CS_ImUtils::getRoleLabel(apBlade->systemType, UNDEF_SIDE, apBlade->systemNumber);

	string apRdn = ACS_CS_ImmMapper::ATTR_APG_ID + "=" + apName + "," + ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseAp = subset->getObject(apRdn.c_str());

	if (baseAp)
		apFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseAp = model->getObject(apRdn.c_str());
		if (baseAp)
			apFound = true;
	}


	if (apFound == true) {
		ACS_CC_ReturnType modResult;

		ACS_CS_ImAp* ap = dynamic_cast<ACS_CS_ImAp*>(baseAp);

		if (!ap)
			return 1;

		std::set<string> apRef = ap->axeApBladeRefTo;

		//Erase ApBlade DN from the set in case it is present. This might happen in case of virtual environment during upgrade scenario.
		apRef.erase(apBlade->rdn);
		apRef.insert((NODE_VIRTUALIZED == architecture)? getComputeResourceDN(apBlade): apBlade->rdn);

		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_AP_BLADE_REF_TO.c_str(), apRef);

		ACS_CC_ImmParameter axeBladeRef; //systemIdentifier

		axeBladeRef.attrName = attr.getAttrName();
		axeBladeRef.attrType = attr.getAttrType();
		axeBladeRef.attrValuesNum = attr.getAttrValuesNum();
		axeBladeRef.attrValues = attr.getAttrValues();

		cout << "========================== AttrValuesNumber = " << axeBladeRef.attrValuesNum << endl;
		modResult = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, apRdn.c_str(), &axeBladeRef);

		if (modResult != ACS_CC_SUCCESS)
			return 1;


		ACS_APGCC_AttrModification *attrMods[2];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		attrVal.attrValuesNum = attr.getAttrValuesNum();
		attrVal.attrValues = attr.getAttrValues();

		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType=ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = NULL;

		bool res = ACS_CS_ImRepository::instance()->modifyObject(ccbId, apRdn.c_str(), attrMods);

		delete attrMods[0];

		if (!res)
			return 1;

	}

	else {
		ACS_CC_ReturnType modResult;
		ACS_CS_ImAp* ap = new ACS_CS_ImAp();

		int eId = 0;
		eId = ACS_CS_ImEntryIdHelper::getNewEntryId();

		ap->rdn = apRdn;
		ap->axeApgId = ACS_CS_ImmMapper::ATTR_APG_ID + "=" + apName;
		ap->axeApBladeRefTo.insert((NODE_VIRTUALIZED == architecture)? getComputeResourceDN(apBlade): apBlade->rdn);
		ap->entryId=eId;

		ACS_CS_ImImmObject *immObject = ap->toImmObject();

		vector<ACS_CC_ValuesDefinitionType> apAttr;

		vector<ACS_CS_ImValuesDefinitionType> apAttrTemp = immObject->getAttributes();


		for (unsigned int j = 0; j < apAttrTemp.size(); j++) {
			apAttr.push_back((ACS_CC_ValuesDefinitionType) apAttrTemp[j]);
		}

		delete ap;

		delete immObject;

		//Add AxeApg object to IMM transaction
		modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, apClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT.c_str(), apAttr);

		if (modResult != ACS_CC_SUCCESS)
			return 1;

		ACS_APGCC_AttrValues **apAttrVal = new ACS_APGCC_AttrValues*[apAttrTemp.size() + 2];

		for (unsigned int j = 0; j < apAttrTemp.size(); j++) {
			apAttrVal[j] = new ACS_APGCC_AttrValues;
			apAttrVal[j]->attrName = apAttrTemp[j].getAttrName();
			apAttrVal[j]->attrType = apAttrTemp[j].getAttrType();
			apAttrVal[j]->attrValuesNum = apAttrTemp[j].getAttrValuesNum();
			apAttrVal[j]->attrValues = apAttrTemp[j].getAttrValues();
		}

		apAttrVal[apAttrTemp.size()] = new ACS_APGCC_AttrValues;
		apAttrVal[apAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		apAttrVal[apAttrTemp.size()]->attrType = ATTR_STRINGT;
		apAttrVal[apAttrTemp.size()]->attrValuesNum = 1;
		void *className[1] ={reinterpret_cast<void*> (const_cast<char*> (apClass.c_str())) };
		apAttrVal[apAttrTemp.size()]->attrValues = className;

		apAttrVal[apAttrTemp.size()+1] = NULL;

		//Add AxeApg object to internal transaction
		ACS_CS_ImBase *object = ACS_CS_ImRepository::instance()->createObject(ccbId, apClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT.c_str(), apAttrVal);

		for (unsigned int j = 0; j < apAttrTemp.size()+1; j++) {
			delete apAttrVal[j];
		}

		delete [] apAttrVal;

		if (!object)
			return 1;
	}

	return 0;
}

int ACS_CS_ApHandler::remove()
{
	ACS_CS_DEBUG(("CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	uint16_t sysNo;
	std::string apName;
	string apClass = ACS_CS_ImmMapper::CLASS_APG;
	bool apFound = false;
	ACS_CS_ImBase *baseAp = 0;

	int architecture;
	if (!ACS_CS_Registry::getNodeArchitecture(architecture))
	{
		return 1;
	}

	if (base->type != APBLADE_T)
		return 0;

	ACS_CS_ImApBlade *apBlade = dynamic_cast<ACS_CS_ImApBlade *> (base);
	if (!apBlade)
		return 1;

	sysNo = apBlade->systemNumber;

	if (sysNo != 1 && sysNo!= 2) //Invalid system number, do nothing (validation will do the job)
		return 0;

	apName = ACS_CS_ImUtils::getRoleLabel(apBlade->systemType, UNDEF_SIDE, apBlade->systemNumber);

	string apRdn = ACS_CS_ImmMapper::ATTR_APG_ID + "=" + apName + "," + ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);

	if (subset)
		baseAp = subset->getObject(apRdn.c_str());

	if (baseAp)
		apFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseAp = model->getObject(apRdn.c_str());
		if (baseAp)
			apFound = true;
	}

	if (apFound == true) {

		ACS_CC_ReturnType modResult;

		ACS_CS_ImAp* ap = dynamic_cast<ACS_CS_ImAp*>(baseAp);

		if (!ap)
			return 1;

		std::set<string> apRef = ap->axeApBladeRefTo;
		apRef.erase(apBlade->rdn);
		apRef.erase(getComputeResourceDN(apBlade));

		//if (apRef.size() >= 0 || (apName.str()).compare("ap1") == 0 || (apName.str()).compare("AP1") == 0) {
		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_AP_BLADE_REF_TO.c_str(), apRef);

		ACS_CC_ImmParameter axeBladeRef; //systemIdentifier

		axeBladeRef.attrName = attr.getAttrName();
		axeBladeRef.attrType = attr.getAttrType();
		axeBladeRef.attrValuesNum = attr.getAttrValuesNum();
		axeBladeRef.attrValues = attr.getAttrValues();

		cout << "========================== AttrValuesNumber = " << axeBladeRef.attrValuesNum << endl;
		modResult = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, apRdn.c_str(), &axeBladeRef);

		if (modResult != ACS_CC_SUCCESS)
			return 1;
		ACS_CS_DEBUG(("CcbId[%d] - Augmented OBJ == %s", ccbId, apRdn.c_str()));
		ACS_APGCC_AttrModification *attrMods[2];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		attrVal.attrValuesNum = attr.getAttrValuesNum();
		attrVal.attrValues = attr.getAttrValues();

		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType=ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = NULL;

		ACS_CS_ImRepository::instance()->modifyObject(ccbId, apRdn.c_str(), attrMods);

		delete attrMods[0];
//		} else {
//			ACS_CC_ReturnType modResult;
//
//			modResult = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, apRdn.c_str());
//
//			if (modResult != ACS_CC_SUCCESS) {
//				cout << "deleteObjectAugmentCcb failed" << endl;
//				cout << "RC: " << getInternalLastError() << " " << getInternalLastErrorText() << endl;
//				return 1;
//			}
//
//			bool res = ACS_CS_ImRepository::instance()->deleteSubTree(ccbId, apRdn.c_str());
//
//			if (!res) {
//				cout << "ACS_CS_ImRepository::instance()->deleteSubTree failed" << endl;
//				return 1;
//			}
//
//			const ACS_CS_ImModel* tempSubset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
//
//			if (tempSubset) {
//				ACS_CS_ImBase *tempBaseAp = subset->getObject(apRdn.c_str());
//				if (tempBaseAp) {
//					ACS_CS_ImAp* tempAp = dynamic_cast<ACS_CS_ImAp*>(baseAp);
//					if (tempAp)
//						tempAp->axeApBladeRefTo.clear();
//				}
//			}
//		}

	}

	return 0;
}

int ACS_CS_ApHandler::modify()
{
	return create();
}

std::string ACS_CS_ApHandler::getComputeResourceDN(const ACS_CS_ImApBlade* blade) const
{
	return ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID + "=" + getRoleLabel(blade) + "," + ACS_CS_ImmMapper::RDN_CRM_EQUIPMENT;
}


std::string ACS_CS_ApHandler::getRoleLabel(const ACS_CS_ImApBlade* blade) const
{
	return ACS_CS_ImUtils::getRoleLabel(blade->systemType, blade->side, blade->systemNumber);
}
