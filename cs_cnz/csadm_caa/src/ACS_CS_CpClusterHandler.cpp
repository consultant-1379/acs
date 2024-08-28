

#include "ACS_CS_CpClusterHandler.h"

ACS_CS_Trace_TDEF(ACS_CS_CpClusterHandler_TRACE);

ACS_CS_CpClusterHandler::ACS_CS_CpClusterHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
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



ACS_CS_CpClusterHandler::~ACS_CS_CpClusterHandler() {
}



int ACS_CS_CpClusterHandler::create()
{
	ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	std::cout<<"create from ACS_CS_CpClusterHandler\n";

	if (base->type != CPBLADE_T)
		return 0;

	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *> (base);

	if (!cpBlade)
		return 1;

	if (cpBlade->systemType != SINGLE_SIDED_CP || cpBlade->functionalBoardName != CPUB)
		return 0;

	if (cpBlade->sequenceNumber > 63 || cpBlade->sequenceNumber == -1) //Invalid sequence number: do nothing
		return 0;

	ostringstream bladeName;
	bladeName << "BC" << cpBlade->sequenceNumber;

	string cpClusterRdn = ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER;
	string cpClusterClass = ACS_CS_ImmMapper::CLASS_CP_CLUSTER;
	string cpClusterStructRdn = ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER_STRUCT;
	string cpClusterStructClass = ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT;

	ACS_CS_ImBase *baseCpCluster = 0;
	bool cpClusterFound = false;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseCpCluster = subset->getObject(cpClusterRdn.c_str());

	if (baseCpCluster)
		cpClusterFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseCpCluster = model->getObject(cpClusterRdn.c_str());
		if (baseCpCluster)
			cpClusterFound = true;
	}

	if (cpClusterFound) {
		ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - CpCluster object already exists!", ccbId));
		ACS_CC_ReturnType modResult;

		ACS_CS_ImCpCluster* cpCluster = dynamic_cast<ACS_CS_ImCpCluster*>(baseCpCluster);

		if (!cpCluster)
			return 1;

		std::set<string> allBcRef = cpCluster->allBcGroup;
		allBcRef.insert(bladeName.str());

		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createMultipleStringType("allBcGroup", allBcRef);

		ACS_CC_ImmParameter allBcGroup;

		allBcGroup.attrName = attr.getAttrName();
		allBcGroup.attrType = attr.getAttrType();
		allBcGroup.attrValuesNum = attr.getAttrValuesNum();
		allBcGroup.attrValues = attr.getAttrValues();

		cout << "========================== AttrValuesNumber = " << allBcGroup.attrValuesNum << endl;
		modResult = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpClusterRdn.c_str(), &allBcGroup);

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

		bool res = ACS_CS_ImRepository::instance()->modifyObject(ccbId, cpClusterRdn.c_str(), attrMods);

		delete attrMods[0];

		if (!res)
			return 1;
	}

	else {
		ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - CREATING CpCluster object if it DOES NOT EXIST!", ccbId));
		ACS_CC_ReturnType modResult;

		///////////////////////////////////////////////////
		//	create CpCluster Object
		///////////////////////////////////////////////////
		ACS_CS_ImCpCluster* cpCluster = new ACS_CS_ImCpCluster();

		cpCluster->rdn = cpClusterRdn;
		cpCluster->axeCpClusterId = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_ID + "=1";
		cpCluster->allBcGroup.insert(bladeName.str());

		//Set default values
		cpCluster->alarmMaster = 0;
		cpCluster->clockMaster = 0;
//		cpCluster->omProfile = "PROFILE 100";
//		cpCluster->phase = 	IDLE;
		cpCluster->clusterOpMode = 	NORMAL;
		cpCluster->clusterOpModeType = UNDEF_CLUSTEROPMODETYPE;
		cpCluster->frontAp = AP1;
		cpCluster->ogClearCode = UNDEF_CLEARCODE;
//		cpCluster->aptProfile.clear();
//		cpCluster->apzProfile.clear();
		cpCluster->reportProgress = cpClusterStructRdn;
//		cpCluster->activeCcFile.clear();

		ACS_CS_ImImmObject *immObject = cpCluster->toImmObject();

		vector<ACS_CC_ValuesDefinitionType> cpClusterAttr;

		vector<ACS_CS_ImValuesDefinitionType> cpClusterAttrTemp = immObject->getAttributes();


		for (unsigned int j = 0; j < cpClusterAttrTemp.size(); j++) {
			cpClusterAttr.push_back((ACS_CC_ValuesDefinitionType) cpClusterAttrTemp[j]);
		}

		delete cpCluster;

		delete immObject;

		//Add CpCluster object to IMM transaction
		modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpClusterClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT.c_str(), cpClusterAttr);

		if (modResult != ACS_CC_SUCCESS)
			return 1;

		ACS_APGCC_AttrValues **cpClusterAttrVal = new ACS_APGCC_AttrValues*[cpClusterAttrTemp.size() + 2];

		for (unsigned int j = 0; j < cpClusterAttrTemp.size(); j++) {
			cpClusterAttrVal[j] = new ACS_APGCC_AttrValues;
			cpClusterAttrVal[j]->attrName = cpClusterAttrTemp[j].getAttrName();
			cpClusterAttrVal[j]->attrType = cpClusterAttrTemp[j].getAttrType();
			cpClusterAttrVal[j]->attrValuesNum = cpClusterAttrTemp[j].getAttrValuesNum();
			cpClusterAttrVal[j]->attrValues = cpClusterAttrTemp[j].getAttrValues();
		}

		cpClusterAttrVal[cpClusterAttrTemp.size()] = new ACS_APGCC_AttrValues;
		cpClusterAttrVal[cpClusterAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		cpClusterAttrVal[cpClusterAttrTemp.size()]->attrType = ATTR_STRINGT;
		cpClusterAttrVal[cpClusterAttrTemp.size()]->attrValuesNum = 1;
		void *className[1] ={reinterpret_cast<void*> (const_cast<char*> (cpClusterClass.c_str())) };
		cpClusterAttrVal[cpClusterAttrTemp.size()]->attrValues = className;

		cpClusterAttrVal[cpClusterAttrTemp.size()+1] = NULL;

		//Add CpCluster object to internal transaction
		ACS_CS_ImBase *object = ACS_CS_ImRepository::instance()->createObject(ccbId, cpClusterClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT.c_str(), cpClusterAttrVal);

		for (unsigned int j = 0; j < cpClusterAttrTemp.size()+1; j++) {
			delete cpClusterAttrVal[j];
		}

		delete [] cpClusterAttrVal;

		if (!object)
			return 1;

		///////////////////////////////////////////////////
		//	create CpClusterStruct Object
		///////////////////////////////////////////////////

		ACS_CS_ImCpClusterStruct* cpClusterStruct = new ACS_CS_ImCpClusterStruct();

		cpClusterStruct->rdn = cpClusterStructRdn;
		cpClusterStruct->axeCpClusterStructId = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID + "=cpCluster";
		cpClusterStruct->state = UNDEFINED_STATE;
		cpClusterStruct->result = NOT_AVAILABLE;
		cpClusterStruct->actionId = UNDEFINED_TYPE;
		cpClusterStruct->reason.clear();
		cpClusterStruct->timeOfLastAction.clear();

		ACS_CS_ImImmObject *immObjectStruct = cpClusterStruct->toImmObject();
		vector<ACS_CC_ValuesDefinitionType> cpClusterStructAttr;
		vector<ACS_CS_ImValuesDefinitionType> cpClusterStructAttrTemp = immObjectStruct->getAttributes();

		for (unsigned int j = 0; j < cpClusterStructAttrTemp.size(); j++) {
			cpClusterStructAttr.push_back((ACS_CC_ValuesDefinitionType) cpClusterStructAttrTemp[j]);
		}

		delete cpClusterStruct;
		delete immObjectStruct;

		modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpClusterStructClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str(), cpClusterStructAttr);

		if (modResult != ACS_CC_SUCCESS)
			return 1;

		ACS_APGCC_AttrValues **cpClusterStructAttrVal = new ACS_APGCC_AttrValues*[cpClusterStructAttrTemp.size() + 2];

		for (unsigned int j = 0; j < cpClusterStructAttrTemp.size(); j++) {
			cpClusterStructAttrVal[j] = new ACS_APGCC_AttrValues;
			cpClusterStructAttrVal[j]->attrName = cpClusterStructAttrTemp[j].getAttrName();
			cpClusterStructAttrVal[j]->attrType = cpClusterStructAttrTemp[j].getAttrType();
			cpClusterStructAttrVal[j]->attrValuesNum = cpClusterStructAttrTemp[j].getAttrValuesNum();
			cpClusterStructAttrVal[j]->attrValues = cpClusterStructAttrTemp[j].getAttrValues();
		}

		cpClusterStructAttrVal[cpClusterStructAttrTemp.size()] = new ACS_APGCC_AttrValues;
		cpClusterStructAttrVal[cpClusterStructAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		cpClusterStructAttrVal[cpClusterStructAttrTemp.size()]->attrType = ATTR_STRINGT;
		cpClusterStructAttrVal[cpClusterStructAttrTemp.size()]->attrValuesNum = 1;
		void *classNameStruct[1] ={reinterpret_cast<void*> (const_cast<char*> (cpClusterStructClass.c_str())) };
		cpClusterStructAttrVal[cpClusterStructAttrTemp.size()]->attrValues = classNameStruct;

		cpClusterStructAttrVal[cpClusterStructAttrTemp.size()+1] = NULL;

		//Add CpCluster object to internal transaction
		ACS_CS_ImBase *objectStruct = ACS_CS_ImRepository::instance()->createObject(ccbId, cpClusterStructClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str(), cpClusterStructAttrVal);

		for (unsigned int j = 0; j < cpClusterStructAttrTemp.size()+1; j++) {
			delete cpClusterStructAttrVal[j];
		}

		delete [] cpClusterStructAttrVal;

		if (!objectStruct)
			return 1;
	}

	return 0;
}

int ACS_CS_CpClusterHandler::remove()
{
	ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
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

	if (cpBlade->systemType != SINGLE_SIDED_CP || cpBlade->functionalBoardName != CPUB)
		return 0;

	if (cpBlade->sequenceNumber > 63 || cpBlade->sequenceNumber == -1) //Invalid sequence number: do nothing
		return 0;

	ostringstream bladeName;
	bladeName << "BC" << cpBlade->sequenceNumber;

	string cpClusterRdn = ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER;
	string cpClusterClass = ACS_CS_ImmMapper::CLASS_CP_CLUSTER;

	ACS_CS_ImBase *baseCpCluster = 0;
	bool cpClusterFound = false;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseCpCluster = subset->getObject(cpClusterRdn.c_str());

	if (baseCpCluster)
		cpClusterFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseCpCluster = model->getObject(cpClusterRdn.c_str());
		if (baseCpCluster)
			cpClusterFound = true;
	}

	if (cpClusterFound == true) {

		ACS_CC_ReturnType modResult;

		ACS_CS_ImCpCluster* cpCluster = dynamic_cast<ACS_CS_ImCpCluster*>(baseCpCluster);

		if (!cpCluster)
			return 1;

		std::string bladeNameStr = bladeName.str();
		ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - BladeName[%s] - cpCluster->allBcGroup - size == %d", ccbId, bladeNameStr.c_str(), cpCluster->allBcGroup.size()));
		ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - BladeName[%s] - cpCluster->operativeGroup - size == %d", ccbId, bladeNameStr.c_str(), cpCluster->operativeGroup.size()));

		std::set<string> allBcRef = cpCluster->allBcGroup;
		if(allBcRef.size() > 0)
			ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - allBcRef[0] == [%s]", ccbId, (*(allBcRef.begin())).c_str()));
		allBcRef.erase(bladeNameStr);

		std::set<string> opGroupRef = cpCluster->operativeGroup;
		if(opGroupRef.size() > 0)
			ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - opGroupRef[0] == [%s]", ccbId, (*(opGroupRef.begin())).c_str()));
		opGroupRef.erase(bladeNameStr);

		ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - BladeName[%s] - allBcRef - size == %d", ccbId, bladeNameStr.c_str(), allBcRef.size()));
		ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - BladeName[%s] - opGroupRef - size == %d", ccbId, bladeNameStr.c_str(), opGroupRef.size()));

		if(allBcRef.size() == 0)
			allBcRef.clear();
		if(opGroupRef.size() == 0)
			opGroupRef.clear();

		//		if((architecture != NODE_VIRTUALIZED) || (architecture == NODE_VIRTUALIZED && allBcRef.size() > 0)) {
		ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - Augment MODIFY DN [%s]", ccbId, cpClusterRdn.c_str()));
		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createMultipleStringType("allBcGroup", allBcRef);
		ACS_CS_ImValuesDefinitionType attr_op = ACS_CS_ImUtils::createMultipleStringType("operativeGroup", opGroupRef);
		ACS_CC_ImmParameter allBcGroup;
		ACS_CC_ImmParameter opGroup;

		allBcGroup.attrName = attr.getAttrName();
		allBcGroup.attrType = attr.getAttrType();
		if(allBcRef.size() == 0) {
			allBcGroup.attrValuesNum = 0;
			allBcGroup.attrValues = NULL;
		}
		else {
			allBcGroup.attrValuesNum = attr.getAttrValuesNum();
			allBcGroup.attrValues = attr.getAttrValues();
		}

		cout << "========================== AttrValuesNumber = " << allBcGroup.attrValuesNum << endl;
		modResult = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpClusterRdn.c_str(), &allBcGroup);

		if (modResult != ACS_CC_SUCCESS) {
			ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - FAILED to augment MODIFY allBcGroup DN [%s]", ccbId, cpClusterRdn.c_str()));
			return 1;
		}

		opGroup.attrName = attr_op.getAttrName();
		opGroup.attrType = attr_op.getAttrType();
		if(opGroupRef.size() == 0) {
			opGroup.attrValuesNum = 0;
			opGroup.attrValues = NULL;
		}
		else {
			opGroup.attrValuesNum = attr_op.getAttrValuesNum();
			opGroup.attrValues = attr_op.getAttrValues();
		}

		cout << "========================== AttrValuesNumber = " << opGroup.attrValuesNum << endl;
		modResult = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, cpClusterRdn.c_str(), &opGroup);

		if (modResult != ACS_CC_SUCCESS) {
			ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - FAILED to augment MODIFY operativeGroup DN [%s]", ccbId, cpClusterRdn.c_str()));
			return 1;
		}
		ACS_APGCC_AttrModification *attrMods[3];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		if(allBcRef.size() == 0) {
			attrVal.attrValuesNum = 0;
			attrVal.attrValues = NULL;
		}
		else {
			attrVal.attrValuesNum = attr.getAttrValuesNum();
			attrVal.attrValues = attr.getAttrValues();
		}

		ACS_APGCC_AttrValues attrVal_op;
		attrVal_op.attrName = attr_op.getAttrName();
		attrVal_op.attrType = attr_op.getAttrType();
		if(opGroupRef.size() == 0) {
			attrVal_op.attrValuesNum = 0;
			attrVal_op.attrValues = NULL;
		}
		else {
			attrVal_op.attrValuesNum = attr_op.getAttrValuesNum();
			attrVal_op.attrValues = attr_op.getAttrValues();
		}


		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType=ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = new ACS_APGCC_AttrModification;
		attrMods[1]->modType=ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[1]->modAttr = attrVal_op;

		attrMods[2] = NULL;

		if(!ACS_CS_ImRepository::instance()->modifyObject(ccbId, cpClusterRdn.c_str(), attrMods))
			ACS_CS_FTRACE((ACS_CS_CpClusterHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - FAILED ACS_CS_ImRepository::modify() for DN [%s]", ccbId, cpClusterRdn.c_str()));

		delete attrMods[0];
		delete attrMods[1];
	}
	return 0;
}

int ACS_CS_CpClusterHandler::modify()
{
	return 0;
}



