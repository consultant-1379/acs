/*
 * ACS_CS_OmProfileManager.cpp
 *
 *  Created on: Apr 9, 2013
 *      Author: eanform
 */

#include "ACS_CS_OmProfileManager.h"
#include "ACS_CS_Trace.h"

ACS_CS_Trace_TDEF(ACS_CS_OmProfileManager_TRACE);
ACS_CS_OmProfileManager::ACS_CS_OmProfileManager(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
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



ACS_CS_OmProfileManager::~ACS_CS_OmProfileManager() {
	// TODO Auto-generated destructor stub
	std::cout<<"DISTRUTTORE ACS_CS_OmProfileManager\n";
}



int ACS_CS_OmProfileManager::create()
{
	std::cout<<"create from ACS_CS_OmProfileManager\n";

//	if (base->type != OMPROFILEMANAGER_T)
//		return 0;

	string omProfileMgrRdn = ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER;
	string omProfileMgrClass = ACS_CS_ImmMapper::CLASS_OM_PROFILE_MANAGER;
	string omProfileMgrStructRdn = ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER_STRUCT;
	string omProfileMgrStructClass = ACS_CS_ImmMapper::CLASS_OM_PROFILE_STRUCT;


	ACS_CS_ImBase *baseOmProfileMgr = 0;
	bool omProfileMgrFound = false;

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
		baseOmProfileMgr = subset->getObject(omProfileMgrRdn.c_str());

	if (baseOmProfileMgr)
		omProfileMgrFound = true;
	else {
		ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
		baseOmProfileMgr = model->getObject(omProfileMgrRdn.c_str());
		if (baseOmProfileMgr)
			omProfileMgrFound = true;
	}

	if (!baseOmProfileMgr) {

		ACS_CC_ReturnType modResult;

		///////////////////////////////////////////////////
		//	create OmProfileManager Object
		///////////////////////////////////////////////////

		ACS_CS_ImOmProfileManager* omProfileManager = new ACS_CS_ImOmProfileManager();

		omProfileManager->rdn = omProfileMgrRdn;
		omProfileManager->omProfileManagerId = ACS_CS_ImmMapper::ATTR_OM_PROFILE_MANAGER_ID + "=1";
		omProfileManager->omProfile = "PROFILE 100";
		omProfileManager->scope = CP_AP;
		omProfileManager->reportProgress = omProfileMgrStructRdn;
		omProfileManager->activeCcFile.clear();

		ACS_CS_ImImmObject *immOmProfileM = omProfileManager->toImmObject();
		vector<ACS_CC_ValuesDefinitionType> omProfileManagerAttr;
		vector<ACS_CS_ImValuesDefinitionType> omProfileManagerAttrTemp = immOmProfileM->getAttributes();

		for (unsigned int j = 0; j < omProfileManagerAttrTemp.size(); j++) {
			omProfileManagerAttr.push_back((ACS_CC_ValuesDefinitionType) omProfileManagerAttrTemp[j]);
		}

		delete omProfileManager;
		delete immOmProfileM;
		
		for(int i=0; i<5; i++)
                {
			 modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, omProfileMgrClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str(), omProfileManagerAttr);

			if (modResult != ACS_CC_SUCCESS)
                	{	
				ACS_CS_FTRACE((ACS_CS_OmProfileManager_TRACE,LOG_LEVEL_ERROR,"OmProfileManager object creation failed, re-trying...."));
			
				ACE_OS::sleep(1);

			}
			else
			{
				ACS_CS_FTRACE((ACS_CS_OmProfileManager_TRACE,LOG_LEVEL_INFO,"OmProfileManager object created successfully"));
				break;
			}
                }

		if (modResult != ACS_CC_SUCCESS)
			return 1;

		ACS_APGCC_AttrValues **omProfileManagerAttrVal = new ACS_APGCC_AttrValues*[omProfileManagerAttrTemp.size() + 2];

		for (unsigned int j = 0; j < omProfileManagerAttrTemp.size(); j++) {
			omProfileManagerAttrVal[j] = new ACS_APGCC_AttrValues;
			omProfileManagerAttrVal[j]->attrName = omProfileManagerAttrTemp[j].getAttrName();
			omProfileManagerAttrVal[j]->attrType = omProfileManagerAttrTemp[j].getAttrType();
			omProfileManagerAttrVal[j]->attrValuesNum = omProfileManagerAttrTemp[j].getAttrValuesNum();
			omProfileManagerAttrVal[j]->attrValues = omProfileManagerAttrTemp[j].getAttrValues();
		}

		omProfileManagerAttrVal[omProfileManagerAttrTemp.size()] = new ACS_APGCC_AttrValues;
		omProfileManagerAttrVal[omProfileManagerAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		omProfileManagerAttrVal[omProfileManagerAttrTemp.size()]->attrType = ATTR_STRINGT;
		omProfileManagerAttrVal[omProfileManagerAttrTemp.size()]->attrValuesNum = 1;
		void *classNameOmProfile[1] ={reinterpret_cast<void*> (const_cast<char*> (omProfileMgrClass.c_str())) };
		omProfileManagerAttrVal[omProfileManagerAttrTemp.size()]->attrValues = classNameOmProfile;

		omProfileManagerAttrVal[omProfileManagerAttrTemp.size()+1] = NULL;

		//Add OmProfileMgr object to internal transaction
		ACS_CS_ImBase *objectOmProfileM = ACS_CS_ImRepository::instance()->createObject(ccbId, omProfileMgrClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str(), omProfileManagerAttrVal);

		for (unsigned int j = 0; j < omProfileManagerAttrTemp.size()+1; j++) {
			delete omProfileManagerAttrVal[j];
		}

		delete [] omProfileManagerAttrVal;

		if (!objectOmProfileM)
			return 1;



		///////////////////////////////////////////////////
		//	create OmProfileMgrStruct Object
		///////////////////////////////////////////////////

		ACS_CS_ImOmProfileStruct* omProfileMgrStruct = new ACS_CS_ImOmProfileStruct();

		omProfileMgrStruct->rdn = omProfileMgrStructRdn;
		omProfileMgrStruct->axeCpClusterStructId = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID + "=omProfile";
		omProfileMgrStruct->state = UNDEFINED_STATE;
		omProfileMgrStruct->result = NOT_AVAILABLE;
		omProfileMgrStruct->actionId = UNDEFINED_TYPE;
		omProfileMgrStruct->reason.clear();
		omProfileMgrStruct->timeOfLastAction.clear();

		ACS_CS_ImImmObject *immObjectStruct = omProfileMgrStruct->toImmObject();
		vector<ACS_CC_ValuesDefinitionType> omProfileMgrStructAttr;
		vector<ACS_CS_ImValuesDefinitionType> omProfileMgrStructAttrTemp = immObjectStruct->getAttributes();

		for (unsigned int j = 0; j < omProfileMgrStructAttrTemp.size(); j++) {
			omProfileMgrStructAttr.push_back((ACS_CC_ValuesDefinitionType) omProfileMgrStructAttrTemp[j]);
		}

		delete omProfileMgrStruct;
		delete immObjectStruct;

		
		for(int i=0; i<5; i++)
                {
			modResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, omProfileMgrStructClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER.c_str(), omProfileMgrStructAttr);
			if (modResult != ACS_CC_SUCCESS)
                        {	
				ACS_CS_FTRACE((ACS_CS_OmProfileManager_TRACE,LOG_LEVEL_ERROR,"OmProfileManagerStruct object creation failed, re-trying...."));
                               	ACE_OS::sleep(1);
                       	}
                        else
                        {	ACS_CS_FTRACE((ACS_CS_OmProfileManager_TRACE,LOG_LEVEL_INFO,"OmProfileManagerStruct object created successfully"));
                                break;
                        }
                }


		if (modResult != ACS_CC_SUCCESS)
			return 1;

		ACS_APGCC_AttrValues **omProfileMgrStructAttrVal = new ACS_APGCC_AttrValues*[omProfileMgrStructAttrTemp.size() + 2];

		for (unsigned int j = 0; j < omProfileMgrStructAttrTemp.size(); j++) {
			omProfileMgrStructAttrVal[j] = new ACS_APGCC_AttrValues;
			omProfileMgrStructAttrVal[j]->attrName = omProfileMgrStructAttrTemp[j].getAttrName();
			omProfileMgrStructAttrVal[j]->attrType = omProfileMgrStructAttrTemp[j].getAttrType();
			omProfileMgrStructAttrVal[j]->attrValuesNum = omProfileMgrStructAttrTemp[j].getAttrValuesNum();
			omProfileMgrStructAttrVal[j]->attrValues = omProfileMgrStructAttrTemp[j].getAttrValues();
		}

		omProfileMgrStructAttrVal[omProfileMgrStructAttrTemp.size()] = new ACS_APGCC_AttrValues;
		omProfileMgrStructAttrVal[omProfileMgrStructAttrTemp.size()]->attrName = const_cast<char*> ("SaImmAttrClassName");
		omProfileMgrStructAttrVal[omProfileMgrStructAttrTemp.size()]->attrType = ATTR_STRINGT;
		omProfileMgrStructAttrVal[omProfileMgrStructAttrTemp.size()]->attrValuesNum = 1;
		void *classNameomPMgrStruct[1] ={reinterpret_cast<void*> (const_cast<char*> (omProfileMgrStructClass.c_str())) };
		omProfileMgrStructAttrVal[omProfileMgrStructAttrTemp.size()]->attrValues = classNameomPMgrStruct;

		omProfileMgrStructAttrVal[omProfileMgrStructAttrTemp.size()+1] = NULL;

		//Add CpCluster object to internal transaction
		ACS_CS_ImBase *objectomPMgrStruct = ACS_CS_ImRepository::instance()->createObject(ccbId, omProfileMgrStructClass.c_str(), ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER.c_str(), omProfileMgrStructAttrVal);

		for (unsigned int j = 0; j < omProfileMgrStructAttrTemp.size()+1; j++) {
			delete omProfileMgrStructAttrVal[j];
		}

		delete [] omProfileMgrStructAttrVal;

		if (!objectomPMgrStruct)
			return 1;

	}

	return 0;
}

int ACS_CS_OmProfileManager::remove()
{

	return 0;
}

int ACS_CS_OmProfileManager::modify()
{
	return 0;
}



