/*
 * ACS_CS_OmProfileHandler.cpp
 *
 *  Created on: Feb 5, 2013
 *      Author: eanform
 */

#include "ACS_CS_OmProfileHandler.h"



ACS_CS_OmProfileHandler::ACS_CS_OmProfileHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
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



ACS_CS_OmProfileHandler::~ACS_CS_OmProfileHandler() {
	// TODO Auto-generated destructor stub
	std::cout<<"DISTRUTTORE ACS_CS_OmProfileHandler\n";
}



int ACS_CS_OmProfileHandler::create()
{
	std::cout<<"create from ACS_CS_OmProfileHandler\n";
	if (base->type != OMPROFILE_T)
		return 0;

	ACS_CS_ImOmProfile *omProfileBase = dynamic_cast<ACS_CS_ImOmProfile *> (base);

	if (!omProfileBase)
		return 1;

	cout <<  " omProfileBase->omProfileId: "<< omProfileBase->omProfileId.c_str() << endl;

	string omProfileRdn = omProfileBase->omProfileId + "," + ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER;
	string ccFileRdn = ACS_CS_ImmMapper::ATTR_CCFILE_ID + "=" + ACS_CS_NEHandler::getXmlProperty(IDENTITY) + "," + omProfileRdn;
	string omProfileClass = ACS_CS_ImmMapper::CLASS_OM_PROFILE;
	string ccFileClass = ACS_CS_ImmMapper::CLASS_CCFILE;

	///////////////////////////////////////////////////
	//	create ccFile Object
	///////////////////////////////////////////////////
	ACS_CC_ReturnType modResult;
	ACS_CS_ImCcFile* ccFile = new ACS_CS_ImCcFile();

	ccFile->rdn = ccFileRdn;
	ccFile->ccFileId = ACS_CS_ImmMapper::ATTR_CCFILE_ID + "=" + ACS_CS_NEHandler::getXmlProperty(IDENTITY);

	//Set default values
	ccFile->type = CCFILE_T;
	ccFile->rulesVersion = ACS_CS_NEHandler::getXmlProperty(RULESVERSION);
	ccFile->state = NEW_CCF;


	ACS_CS_ImImmObject *immObjectCcf = ccFile->toImmObject();
	vector<ACS_CC_ValuesDefinitionType> ccFileAttr;
	vector<ACS_CS_ImValuesDefinitionType> ccFileAttrTemp = immObjectCcf->getAttributes();


	for (unsigned int j = 0; j < ccFileAttrTemp.size(); j++) {
		ccFileAttr.push_back((ACS_CC_ValuesDefinitionType) ccFileAttrTemp[j]);
	}

	delete ccFile;
	delete immObjectCcf;

	//Add CcFile object to IMM transaction
	std::cout<<"createObjectAugmentCcb: Add CcFile object to IMM transaction"<< endl;
	modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, ccFileClass.c_str(), omProfileRdn.c_str(), ccFileAttr);

	if (modResult != ACS_CC_SUCCESS){
		std::cout<<"failed to createObjectAugmentCcb for CcFile"<< endl;
		return 1;
	}

	ACS_APGCC_AttrValues **ccFileAttrVal = new ACS_APGCC_AttrValues*[ccFileAttrTemp.size() + 2];

	for (unsigned int j = 0; j < ccFileAttrTemp.size(); j++) {
		ccFileAttrVal[j] = new ACS_APGCC_AttrValues;
		ccFileAttrVal[j]->attrName = ccFileAttrTemp[j].getAttrName();
		ccFileAttrVal[j]->attrType = ccFileAttrTemp[j].getAttrType();
		ccFileAttrVal[j]->attrValuesNum = ccFileAttrTemp[j].getAttrValuesNum();
		ccFileAttrVal[j]->attrValues = ccFileAttrTemp[j].getAttrValues();
	}

	ccFileAttrVal[ccFileAttrTemp.size()] = new ACS_APGCC_AttrValues;
	ccFileAttrVal[ccFileAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
	ccFileAttrVal[ccFileAttrTemp.size()]->attrType = ATTR_STRINGT;
	ccFileAttrVal[ccFileAttrTemp.size()]->attrValuesNum = 1;
	void *classNameCcf[1] ={reinterpret_cast<void*> (const_cast<char*> (ccFileClass.c_str())) };
	ccFileAttrVal[ccFileAttrTemp.size()]->attrValues = classNameCcf;

	ccFileAttrVal[ccFileAttrTemp.size()+1] = NULL;

	//Add CcFile object to internal transaction
	std::cout<<"createObject: Add CcFile object to IMM transaction"<< endl;
	ACS_CS_ImBase *objectCcf = ACS_CS_ImRepository::instance()->createObject(ccbId, ccFileClass.c_str(), omProfileRdn.c_str(), ccFileAttrVal);

	for (unsigned int j = 0; j < ccFileAttrTemp.size()+1; j++) {
		delete ccFileAttrVal[j];
	}

	delete [] ccFileAttrVal;

	if (!objectCcf){
		std::cout<<"failed to createObject for CcFile"<< endl;
		return 1;
	}

	///////////////////////////////////////////////////////////
	/// copy xml file
	///////////////////////////////////////////////////////////
	if(!ACS_CS_NEHandler::moveMmlFileToAdhFolder(false))
	{
		std::cout<<"failed to copy xml CcFile"<< endl;
		return 1;
	}

	return 0;
}

int ACS_CS_OmProfileHandler::remove()
{
	if (base->type != OMPROFILE_T)
		return 0;

	return 0;
}

int ACS_CS_OmProfileHandler::modify()
{
	return 0;
}
