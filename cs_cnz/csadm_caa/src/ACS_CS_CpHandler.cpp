
#include "ACS_CS_CpHandler.h"
#include "ACS_CS_Registry.h"

ACS_CS_Trace_TDEF(ACS_CS_CpHandler_TRACE);

ACS_CS_CpHandler::ACS_CS_CpHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
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

ACS_CS_CpHandler::~ACS_CS_CpHandler()
{
	std::cout<<"DISTRUTTORE ACS_CS_CpHandler\n";
}

int ACS_CS_CpHandler::create()
{
	ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	std::cout<<"create from ACS_CS_CpHandler\n";

	int result = 0;
	SystemTypeEnum sysType;

	if (base->type != CPBLADE_T)
		return 0;

	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);
	if (!cpBlade)
		return 1;

	if (cpBlade->functionalBoardName == RPBI_S) //No Logical augmentation required for this boards!
		return 0;

	sysType = cpBlade->systemType;

	switch(sysType) {
	case SINGLE_SIDED_CP:
		result = createClusterCp();
		break;

	case DOUBLE_SIDED_CP:
		result = createDualSidedCp();
		break;

	default:
		//Invalid systemType: do nothing
		break;

	}

	return result;
}

int ACS_CS_CpHandler::remove()
{

	std::cout<<"remove from ACS_CS_CpHandler\n";

	int result = 0;
	SystemTypeEnum sysType;

	if (base->type != CPBLADE_T)
		return 0;

	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);
	if (!cpBlade)
		return 1;

	if (cpBlade->functionalBoardName == RPBI_S) //No Logical augmentation required for this boards!
		return 0;

	sysType = cpBlade->systemType;

	switch(sysType) {
	case SINGLE_SIDED_CP:
		result = removeClusterCp();
		break;

	case DOUBLE_SIDED_CP:
		result = removeDualSidedCp();
		break;

	default:
		//Invalid systemType: do nothing
		break;

	}

	return result;
}

int ACS_CS_CpHandler::modify()
{
	std::cout<<"modify from ACS_CS_CpHandler\n";

	int result = 0;

	if (base->type != CPBLADE_T)
		return 0;

	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);
	if (!cpBlade)
		return 1;

	if (cpBlade->functionalBoardName == RPBI_S) //No Logical augmentation required for this boards!
		return 0;

	switch(cpBlade->systemType)
	{
	case DOUBLE_SIDED_CP:
		//Let's treat modify as create operation
		result = createDualSidedCp();
		break;

	default:
		//Invalid systemType or modify operation not supported for this system type: do nothing
		break;

	}

	return result;
}

int ACS_CS_CpHandler::createClusterCp() {
	ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	int16_t seqNo;
	std::string cpName;
	string cpClass = ACS_CS_ImmMapper::CLASS_CLUSTER_CP;
	bool cpFound = false;
	ACS_CS_ImBase *baseCp = 0;

	if (base->type != CPBLADE_T)
		return 0;

	int architecture;
	if (!ACS_CS_Registry::getNodeArchitecture(architecture))
	{
		return 1;
	}


	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);
	if (!cpBlade)
		return 1;

	seqNo = cpBlade->sequenceNumber;

	if (seqNo == -1 || seqNo > 63)
		return 0;

	cpName = ACS_CS_ImUtils::getRoleLabel(cpBlade->systemType, UNDEF_SIDE, cpBlade->sequenceNumber);

	string cpRdn = ACS_CS_ImmMapper::ATTR_CLUSTER_CP_ID + "=" + cpName + "," + ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseCp = subset->getObject(cpRdn.c_str());

	if (baseCp)
		cpFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseCp = model->getObject(cpRdn.c_str());
		if (baseCp)
			cpFound = true;
	}

	if (cpFound == true) {
		//cp blade already exists
		return TC_CPSEQNOFAULTY;
	}

	else {
		ACS_CC_ReturnType modResult;
		ACS_CS_ImClusterCp* cp = new ACS_CS_ImClusterCp();

		cp->rdn = cpRdn;
		cp->axeClusterCpId = ACS_CS_ImmMapper::ATTR_CLUSTER_CP_ID + "=" + cpName;
		cp->axeCpBladeRefTo = (NODE_VIRTUALIZED == architecture)? getComputeResourceDN(cpBlade): cpBlade->rdn;
		cp->alias = "-";
		cp->apzSystem = "0";
		cp->cpType = 0;
		cp->systemIdentifier = (uint16_t) seqNo;

		//Set Default values
		cp->cpState = 0;
		cp->applicationId = 0;
		cp->apzSubstate = 0;
		cp->aptSubstate = 0;
		cp->stateTransition = 0;
		cp->blockingInfo = 0;
		cp->cpCapacity = 0;


		ACS_CS_ImImmObject *immObject = cp->toImmObject();

		vector<ACS_CC_ValuesDefinitionType> cpAttr;

		vector<ACS_CS_ImValuesDefinitionType> cpAttrTemp = immObject->getAttributes();


		for (unsigned int j = 0; j < cpAttrTemp.size(); j++) {
			cpAttr.push_back((ACS_CC_ValuesDefinitionType) cpAttrTemp[j]);
		}

		delete cp;

		delete immObject;

		//Add AxeClusterCp object to IMM transaction
		modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str(), cpAttr);

		if (modResult != ACS_CC_SUCCESS) {
			ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - FAILED Augment - Unable to CREATE DN == %s", ccbId, cpRdn.c_str()));
			return 1;
		}
		ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - Augment - CREATE DN == %s", ccbId, cpRdn.c_str()));

		ACS_APGCC_AttrValues **cpAttrVal = new ACS_APGCC_AttrValues*[cpAttrTemp.size() + 2];

		for (unsigned int j = 0; j < cpAttrTemp.size(); j++) {
			cpAttrVal[j] = new ACS_APGCC_AttrValues;
			cpAttrVal[j]->attrName = cpAttrTemp[j].getAttrName();
			cpAttrVal[j]->attrType = cpAttrTemp[j].getAttrType();
			cpAttrVal[j]->attrValuesNum = cpAttrTemp[j].getAttrValuesNum();
			cpAttrVal[j]->attrValues = cpAttrTemp[j].getAttrValues();
		}

		cpAttrVal[cpAttrTemp.size()] = new ACS_APGCC_AttrValues;
		cpAttrVal[cpAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		cpAttrVal[cpAttrTemp.size()]->attrType = ATTR_STRINGT;
		cpAttrVal[cpAttrTemp.size()]->attrValuesNum = 1;
		void *className[1] ={reinterpret_cast<void*> (const_cast<char*> (cpClass.c_str())) };
		cpAttrVal[cpAttrTemp.size()]->attrValues = className;

		cpAttrVal[cpAttrTemp.size()+1] = NULL;

		//Add AxeClusterCp object to internal transaction
		ACS_CS_ImBase *object = ACS_CS_ImRepository::instance()->createObject(ccbId, cpClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str(), cpAttrVal);

		for (unsigned int j = 0; j < cpAttrTemp.size()+1; j++) {
			delete cpAttrVal[j];
		}

		delete [] cpAttrVal;

		if (!object)
			return 1;
	}


	return 0;
}

int ACS_CS_CpHandler::removeClusterCp() {
	ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	int16_t seqNo;
	std::string cpName;
	std::string cpClass = ACS_CS_ImmMapper::CLASS_CLUSTER_CP;
	bool cpFound = false;
	ACS_CS_ImBase *baseCp = 0;
	//ACS_CS_ImBase *baseCluster = 0;

	if (base->type != CPBLADE_T)
		return 0;

	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);
	if (!cpBlade)
		return 1;

	seqNo = cpBlade->sequenceNumber;

	if (seqNo == -1 || seqNo > 63)
		return 0;

	cpName = ACS_CS_ImUtils::getRoleLabel(cpBlade->systemType, UNDEF_SIDE, cpBlade->sequenceNumber);

	string cpRdn = ACS_CS_ImmMapper::ATTR_CLUSTER_CP_ID + "=" + cpName + "," + ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset) {
		baseCp = subset->getObject(cpRdn.c_str());
		//baseCluster = subset->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());
	}

	if (baseCp)
		cpFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseCp = model->getObject(cpRdn.c_str());
		if (baseCp)
			cpFound = true;
	}



	if (cpFound == true) {
		ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - Augment - DELETE DN == %s", ccbId, cpRdn.c_str()));
		ACS_CC_ReturnType modResult;

		modResult = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpRdn.c_str());

		if (modResult != ACS_CC_SUCCESS) {
			ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - FAILED Augment - Unable to delete DN == %s", ccbId, cpRdn.c_str()));
			//cout << "deleteObjectAugmentCcb failed" << endl;
			//cout << "RC: " << getInternalLastError() << " " << getInternalLastErrorText() << endl;
			return 1;
		}

		bool res = ACS_CS_ImRepository::instance()->deleteObject(ccbId, cpRdn.c_str());

		if (!res) {
			cout << "ACS_CS_ImRepository::instance()->deleteObject failed" << endl;
			return 1;
		}

	}
	return 0;
}

int ACS_CS_CpHandler::createDualSidedCp() {

	uint16_t sysNo;
	int architecture;

	if(!ACS_CS_Registry::getNodeArchitecture(architecture))
	{
		return 1;
	}

	std::string cpName;
	string cpClass = ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP;
	bool cpFound = false;
	ACS_CS_ImBase *baseCp = 0;

	if (base->type != CPBLADE_T)
		return 0;

	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);
	if (!cpBlade)
		return 1;

	sysNo = cpBlade->systemNumber;

	if (sysNo != 1 && sysNo!= 2) //Invalid system number, do nothing (validation will do the job)
		return 0;

	cpName = ACS_CS_ImUtils::getRoleLabel(cpBlade->systemType, UNDEF_SIDE, cpBlade->systemNumber);

	string cpRdn = ACS_CS_ImmMapper::ATTR_DUAL_SIDED_CP_ID + "=" + cpName + "," + ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseCp = subset->getObject(cpRdn.c_str());

	if (baseCp)
		cpFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseCp = model->getObject(cpRdn.c_str());
		if (baseCp)
			cpFound = true;
	}


	if (cpFound == true) {
		ACS_CC_ReturnType modResult;

		ACS_CS_ImDualSidedCp* cp = dynamic_cast<ACS_CS_ImDualSidedCp*>(baseCp);

		if (!cp)
			return 1;

		std::set<string> cpRef = cp->axeCpBladeRefTo;

		//Erase the DN from the set first in case it is present (this might happen in case of upgrade scenario in virtualized environment)
		cpRef.erase(cpBlade->rdn);
		cpRef.insert((NODE_VIRTUALIZED == architecture)? getComputeResourceDN(cpBlade): cpBlade->rdn);

		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO.c_str(), cpRef);

		ACS_CC_ImmParameter axeBladeRef; //systemIdentifier

		axeBladeRef.attrName = attr.getAttrName();
		axeBladeRef.attrType = attr.getAttrType();
		axeBladeRef.attrValuesNum = attr.getAttrValuesNum();
		axeBladeRef.attrValues = attr.getAttrValues();

		cout << "========================== AttrValuesNumber = " << axeBladeRef.attrValuesNum << endl;
		modResult = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpRdn.c_str(), &axeBladeRef);

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

		bool res = ACS_CS_ImRepository::instance()->modifyObject(ccbId, cpRdn.c_str(), attrMods);

		delete attrMods[0];

		if (!res)
			return 1;

	}

	else {
		ACS_CC_ReturnType modResult;
		ACS_CS_ImDualSidedCp* cp = new ACS_CS_ImDualSidedCp();

		cp->rdn = cpRdn;
		cp->axeDualSidedCpId = ACS_CS_ImmMapper::ATTR_DUAL_SIDED_CP_ID + "=" + cpName;
		cp->axeCpBladeRefTo.insert((NODE_VIRTUALIZED == architecture)? getComputeResourceDN(cpBlade): cpBlade->rdn);
		//cp->alias = cpName.str();
		cp->alias = "-";
		cp->apzSystem = "0";
		cp->cpType = 0;
		cp->systemIdentifier = 1000 + sysNo;
		cp->mauType = ACS_CS_Registry::getDefaultMauType();		
		ACS_CS_ImImmObject *immObject = cp->toImmObject();

		vector<ACS_CC_ValuesDefinitionType> cpAttr;

		vector<ACS_CS_ImValuesDefinitionType> cpAttrTemp = immObject->getAttributes();


		for (unsigned int j = 0; j < cpAttrTemp.size(); j++) {
			cpAttr.push_back((ACS_CC_ValuesDefinitionType) cpAttrTemp[j]);
		//	cout << "attribute name==" << cpAttrTemp[j].getAttrName() << endl;
		//	cout << "attribute value==" << cpAttrTemp[j].getAttrValues() << endl;
		}

		delete cp;

		delete immObject;

		//Add AxeDualSided object to IMM transaction
		modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT.c_str(), cpAttr);

		if (modResult != ACS_CC_SUCCESS)
		{
			return 1;
		}

		ACS_APGCC_AttrValues **cpAttrVal = new ACS_APGCC_AttrValues*[cpAttrTemp.size() + 2];

		for (unsigned int j = 0; j < cpAttrTemp.size(); j++) {
			cpAttrVal[j] = new ACS_APGCC_AttrValues;
			cpAttrVal[j]->attrName = cpAttrTemp[j].getAttrName();
			cpAttrVal[j]->attrType = cpAttrTemp[j].getAttrType();
			cpAttrVal[j]->attrValuesNum = cpAttrTemp[j].getAttrValuesNum();
			cpAttrVal[j]->attrValues = cpAttrTemp[j].getAttrValues();
		}

		cpAttrVal[cpAttrTemp.size()] = new ACS_APGCC_AttrValues;
		cpAttrVal[cpAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		cpAttrVal[cpAttrTemp.size()]->attrType = ATTR_STRINGT;
		cpAttrVal[cpAttrTemp.size()]->attrValuesNum = 1;
		void *className[1] ={reinterpret_cast<void*> (const_cast<char*> (cpClass.c_str())) };
		cpAttrVal[cpAttrTemp.size()]->attrValues = className;

		cpAttrVal[cpAttrTemp.size()+1] = NULL;

		//Add AxeDualSided object to internal transaction
		ACS_CS_ImBase *object = ACS_CS_ImRepository::instance()->createObject(ccbId, cpClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT.c_str(), cpAttrVal);

		for (unsigned int j = 0; j < cpAttrTemp.size()+1; j++) {
			delete cpAttrVal[j];
		}

		delete [] cpAttrVal;

		if (!object)
			return 1;
	}


	return 0;
}

int ACS_CS_CpHandler::removeDualSidedCp() {
	ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	uint16_t sysNo;
	std::string cpName;
	string cpClass = ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP;
	bool cpFound = false;
	ACS_CS_ImBase *baseCp = 0;

	int architecture;
	if (!ACS_CS_Registry::getNodeArchitecture(architecture))
	{
		return 1;
	}

	if (base->type != CPBLADE_T)
		return 0;

	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);
	if (!cpBlade)
		return 1;

	sysNo = cpBlade->systemNumber;

	if (sysNo != 1 && sysNo!= 2) //Invalid system number, do nothing (validation will do the job)
		return 0;

	cpName = ACS_CS_ImUtils::getRoleLabel(cpBlade->systemType, UNDEF_SIDE, cpBlade->systemNumber);

	string dualSidedCpRdn = ACS_CS_ImmMapper::ATTR_DUAL_SIDED_CP_ID + "=" + cpName + "," + ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseCp = subset->getObject(dualSidedCpRdn.c_str());

	if (baseCp)
		cpFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseCp = model->getObject(dualSidedCpRdn.c_str());
		if (baseCp)
			cpFound = true;
	}

	if (cpFound == true) {

		ACS_CC_ReturnType modResult;

		ACS_CS_ImDualSidedCp* cp = dynamic_cast<ACS_CS_ImDualSidedCp*>(baseCp);

		if (!cp)
			return 1;

		std::set<string> cpRef = cp->axeCpBladeRefTo;
		cpRef.erase(getComputeResourceDN(cpBlade));
		cpRef.erase(cpBlade->rdn);

//		if((architecture != NODE_VIRTUALIZED) || ( architecture == NODE_VIRTUALIZED && cpRef.size() > 0)) {
		ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] -  Augment - MODIFY DN [%s]", ccbId, dualSidedCpRdn.c_str()));
		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO.c_str(), cpRef);

		ACS_CC_ImmParameter axeBladeRef; //systemIdentifier

		axeBladeRef.attrName = attr.getAttrName();
		axeBladeRef.attrType = attr.getAttrType();
		axeBladeRef.attrValuesNum = attr.getAttrValuesNum();
		axeBladeRef.attrValues = attr.getAttrValues();

		cout << "========================== AttrValuesNumber = " << axeBladeRef.attrValuesNum << endl;
		modResult = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, dualSidedCpRdn.c_str(), &axeBladeRef);

		if (modResult != ACS_CC_SUCCESS) {
			ACS_CS_FTRACE((ACS_CS_CpHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - FAILED Augment - Unable to modify DN [%s]", ccbId, base->rdn.c_str()));
			return 1;
		}

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

		ACS_CS_ImRepository::instance()->modifyObject(ccbId, dualSidedCpRdn.c_str(), attrMods);

		delete attrMods[0];
//		}
//		else {
//			ACS_CC_ReturnType modResult;
//			std::string swMauDn = ACS_CS_ImmMapper::ATTR_SW_MAU_ID + "=1" + "," + dualSidedCpRdn;
//			ACS_CS_DEBUG(("CcbId[%d] -  Augment - DELETE DN [%s] && [%s]", ccbId, dualSidedCpRdn.c_str(),swMauDn.c_str()));
//
//			modResult = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, swMauDn.c_str());
//			if (modResult != ACS_CC_SUCCESS) {
//				ACS_CS_DEBUG(("CcbId[%d] - FAILED Augment - Unable to delete DN [%s]", ccbId, swMauDn.c_str()));
//				return 1;
//			}
//
//			modResult = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, dualSidedCpRdn.c_str());
//			if (modResult != ACS_CC_SUCCESS) {
//				ACS_CS_DEBUG(("CcbId[%d] - FAILED Augment - Unable to delete DN [%s]", ccbId, dualSidedCpRdn.c_str()));
//				return 1;
//			}
//
//			ACS_CS_ImRepository::instance()->deleteObject(ccbId, swMauDn.c_str());
//			ACS_CS_ImRepository::instance()->deleteObject(ccbId, dualSidedCpRdn.c_str());
//		}

//		} else {
//			ACS_CC_ReturnType modResult;
//
//			modResult = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpRdn.c_str());
//
//			if (modResult != ACS_CC_SUCCESS) {
//				cout << "deleteObjectAugmentCcb failed" << endl;
//				cout << "RC: " << getInternalLastError() << " " << getInternalLastErrorText() << endl;
//				return 1;
//			}
//
//			bool res = ACS_CS_ImRepository::instance()->deleteSubTree(ccbId, cpRdn.c_str());
//
//			if (!res) {
//				cout << "ACS_CS_ImRepository::instance()->deleteSubTree failed" << endl;
//				return 1;
//			}
//
//			const ACS_CS_ImModel* tempSubset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
//
//			if (tempSubset) {
//				ACS_CS_ImBase *tempBaseCp = subset->getObject(cpRdn.c_str());
//				if (tempBaseCp) {
//					ACS_CS_ImDualSidedCp* tempCp = dynamic_cast<ACS_CS_ImDualSidedCp*>(baseCp);
//					if (tempCp)
//						tempCp->axeCpBladeRefTo.clear();
//				}
//			}
//		}

	}

	return 0;

}

std::string ACS_CS_CpHandler::getComputeResourceDN(const ACS_CS_ImCpBlade* blade) const
{
	return ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID + "=" + getRoleLabel(blade) + "," + ACS_CS_ImmMapper::RDN_CRM_EQUIPMENT;
}



std::string ACS_CS_CpHandler::getRoleLabel(const ACS_CS_ImCpBlade* blade) const
{
	if (blade && CPUB == blade->functionalBoardName)
	{
		if (DOUBLE_SIDED_CP == blade->systemType)
		{
			return ACS_CS_ImUtils::getRoleLabel(blade->systemType, blade->side, blade->systemNumber);
		}
		else if (SINGLE_SIDED_CP == blade->systemType)
		{
			return ACS_CS_ImUtils::getRoleLabel(blade->systemType, blade->side, blade->sequenceNumber);
		}
	}

	return "";
}
