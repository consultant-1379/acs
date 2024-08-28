/*
 * ACS_CS_CandidateCcFile.cpp
 *
 *  Created on: Apr 15, 2013
 *      Author: eanform
 */

#include "ACS_CS_CandidateCcFile.h"


ACS_CS_CandidateCcFile::ACS_CS_CandidateCcFile(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
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



ACS_CS_CandidateCcFile::~ACS_CS_CandidateCcFile() {
	// TODO Auto-generated destructor stub
	std::cout<<"DISTRUTTORE ACS_CS_CandidateCcFile\n";
}



int ACS_CS_CandidateCcFile::create()
{

	std::cout<<"create from ACS_CS_CandidateCcFile\n";

	if (base->type != CANDIDATECCFILE_T)
		return 0;

	string fileName = ACS_CS_ImUtils::getIdValueFromRdn(base->rdn);
	string candidateCcFileRdn = base->rdn;
	string candidateCcFileStructRdn = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID + "=" + fileName + "," + candidateCcFileRdn;
	string candidateCcFileStructClass = ACS_CS_ImmMapper::CLASS_CANDIDATE_STRUCT;


	ACS_CC_ReturnType modResult;

//	///////////////////////////////////////////////////
//	//	create CandidateCcFile Object
//	///////////////////////////////////////////////////
//
//	ACS_CS_ImCandidateCcFile *candidateCcFile = new ACS_CS_ImCandidateCcFile();
//
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//
//	candidateCcFile->rdn = candidateCcFileRdn;
//	candidateCcFile->candidateCcFileId = ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID + "=" + fileName;
//	candidateCcFile->reportProgress = "";
//	candidateCcFile->action = ACS_CS_ImBase::CREATE;
//
//	ACS_CS_ImImmObject *immCandCcf = candidateCcFile->toImmObject();
//	vector<ACS_CC_ValuesDefinitionType> candidateCcFileAttr;
//	vector<ACS_CS_ImValuesDefinitionType> candidateCcFileAttrTemp = immCandCcf->getAttributes();
//
//	for (unsigned int j = 0; j < candidateCcFileAttrTemp.size(); j++) {
//		candidateCcFileAttr.push_back((ACS_CC_ValuesDefinitionType) candidateCcFileAttrTemp[j]);
//	}
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//	delete candidateCcFile;
//	delete immCandCcf;
////	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
////	modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, candidateCcFileClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CCFILE_MANAGER.c_str(), candidateCcFileAttr);
////	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
////	if (modResult != ACS_CC_SUCCESS)
////		return 1;
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//	ACS_APGCC_AttrValues **candidateCcFileAttrVal = new ACS_APGCC_AttrValues*[candidateCcFileAttrTemp.size() + 2];
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//	for (unsigned int j = 0; j < candidateCcFileAttrTemp.size(); j++) {
//		candidateCcFileAttrVal[j] = new ACS_APGCC_AttrValues;
//		candidateCcFileAttrVal[j]->attrName = candidateCcFileAttrTemp[j].getAttrName();
//		candidateCcFileAttrVal[j]->attrType = candidateCcFileAttrTemp[j].getAttrType();
//		candidateCcFileAttrVal[j]->attrValuesNum = candidateCcFileAttrTemp[j].getAttrValuesNum();
//		candidateCcFileAttrVal[j]->attrValues = candidateCcFileAttrTemp[j].getAttrValues();
//	}
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//	candidateCcFileAttrVal[candidateCcFileAttrTemp.size()] = new ACS_APGCC_AttrValues;
//	candidateCcFileAttrVal[candidateCcFileAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
//	candidateCcFileAttrVal[candidateCcFileAttrTemp.size()]->attrType = ATTR_STRINGT;
//	candidateCcFileAttrVal[candidateCcFileAttrTemp.size()]->attrValuesNum = 1;
//	void *classNameCandidateCcF[1] ={reinterpret_cast<void*> (const_cast<char*> (candidateCcFileClass.c_str())) };
//	candidateCcFileAttrVal[candidateCcFileAttrTemp.size()]->attrValues = classNameCandidateCcF;
//
//	candidateCcFileAttrVal[candidateCcFileAttrTemp.size()+1] = NULL;
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//	//Add CandidateCcFile object to internal transaction
//	ACS_CS_ImBase *objectCandidateccFile = ACS_CS_ImRepository::instance()->createObject(ccbId, candidateCcFileClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CCFILE_MANAGER.c_str(), candidateCcFileAttrVal);
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//	for (unsigned int j = 0; j < candidateCcFileAttrTemp.size()+1; j++) {
//		delete candidateCcFileAttrVal[j];
//	}
//	std::cout<< __LINE__<< "create from ACS_CS_CandidateCcFile\n";
//	delete [] candidateCcFileAttrVal;
//
//	if (!objectCandidateccFile)
//		return 1;


	///////////////////////////////////////////////////
	//	create CandidateCcFileStruct Object
	///////////////////////////////////////////////////

	ACS_CS_ImCandidateCcFileStruct* candidateCcFileStruct = new ACS_CS_ImCandidateCcFileStruct();

	candidateCcFileStruct->rdn = candidateCcFileStructRdn;
	candidateCcFileStruct->axeCpClusterStructId = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID + "=" + fileName;
	candidateCcFileStruct->state = UNDEFINED_STATE;
	candidateCcFileStruct->result = NOT_AVAILABLE;
	candidateCcFileStruct->actionId = UNDEFINED_TYPE;
	candidateCcFileStruct->reason.clear();
	candidateCcFileStruct->timeOfLastAction.clear();

	candidateCcFileStruct->action = ACS_CS_ImBase::CREATE;


	ACS_CS_ImImmObject *immObjectStruct = candidateCcFileStruct->toImmObject();
	vector<ACS_CC_ValuesDefinitionType> candidateCcFileStructAttr;
	vector<ACS_CS_ImValuesDefinitionType> candidateCcFileStructAttrTemp = immObjectStruct->getAttributes();

	for (unsigned int j = 0; j < candidateCcFileStructAttrTemp.size(); j++) {
		candidateCcFileStructAttr.push_back((ACS_CC_ValuesDefinitionType) candidateCcFileStructAttrTemp[j]);
	}

	delete candidateCcFileStruct;
	delete immObjectStruct;

	modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, candidateCcFileStructClass.c_str(), candidateCcFileRdn.c_str(), candidateCcFileStructAttr);

	if (modResult != ACS_CC_SUCCESS)
		return 1;

	ACS_APGCC_AttrValues **candidateCcFileStructAttrVal = new ACS_APGCC_AttrValues*[candidateCcFileStructAttrTemp.size() + 2];

	for (unsigned int j = 0; j < candidateCcFileStructAttrTemp.size(); j++) {
		candidateCcFileStructAttrVal[j] = new ACS_APGCC_AttrValues;
		candidateCcFileStructAttrVal[j]->attrName = candidateCcFileStructAttrTemp[j].getAttrName();
		candidateCcFileStructAttrVal[j]->attrType = candidateCcFileStructAttrTemp[j].getAttrType();
		candidateCcFileStructAttrVal[j]->attrValuesNum = candidateCcFileStructAttrTemp[j].getAttrValuesNum();
		candidateCcFileStructAttrVal[j]->attrValues = candidateCcFileStructAttrTemp[j].getAttrValues();
	}

	candidateCcFileStructAttrVal[candidateCcFileStructAttrTemp.size()] = new ACS_APGCC_AttrValues;
	candidateCcFileStructAttrVal[candidateCcFileStructAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
	candidateCcFileStructAttrVal[candidateCcFileStructAttrTemp.size()]->attrType = ATTR_STRINGT;
	candidateCcFileStructAttrVal[candidateCcFileStructAttrTemp.size()]->attrValuesNum = 1;
	void *classNameCcFileStruct[1] ={reinterpret_cast<void*> (const_cast<char*> (candidateCcFileStructClass.c_str())) };
	candidateCcFileStructAttrVal[candidateCcFileStructAttrTemp.size()]->attrValues = classNameCcFileStruct;

	candidateCcFileStructAttrVal[candidateCcFileStructAttrTemp.size()+1] = NULL;

	//Add CandidateCcFileStruct object to internal transaction
	ACS_CS_ImBase *objectCandCcfStruct = ACS_CS_ImRepository::instance()->createObject(ccbId, candidateCcFileStructClass.c_str(), candidateCcFileRdn.c_str(), candidateCcFileStructAttrVal);

	for (unsigned int j = 0; j < candidateCcFileStructAttrTemp.size()+1; j++) {
		delete candidateCcFileStructAttrVal[j];
	}

	delete [] candidateCcFileStructAttrVal;

	if (!objectCandCcfStruct)
		return 1;

	return 0;
}

int ACS_CS_CandidateCcFile::remove()
{

	return 0;
}

int ACS_CS_CandidateCcFile::modify()
{
	return 0;
}



