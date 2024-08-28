/*
 * AdminOperation_MyImplementer.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

#include "AdminOperation_MyImplementer.h"
int general;

AdminOperation_MyImplementer::AdminOperation_MyImplementer() {
	// TODO Auto-generated constructor stub

}


void AdminOperation_MyImplementer:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList) {

	printf("------------------------------------------------------------\n");
	printf("                   adminOperationCallback called             \n");
	printf("------------------------------------------------------------\n");

	cout<<"oiHandle   : "<< oiHandle <<endl;
	cout<<"invocation : "<< invocation <<endl;
	cout<<"p_objName  : "<< p_objName <<endl;
	cout<<"operationId: "<< operationId <<endl;

	/*start*/
	cout << endl;

	int dim=0;
	int i=0;
	while(paramList[i]){
		i++;
		dim++;
	}

	i = 0;
	while( paramList[i] ){
		switch ( paramList[i]->attrType ) {
		case ATTR_INT32T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<int *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_UINT32T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_FLOATT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<float *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_DOUBLET:
			cout << " paramListName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<double *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_NAMET:
		{
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;

		}
			break;
		case ATTR_STRINGT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;
			break;
		default:
			break;

		}
		i++;
	}

	cout << " Implementing Admin Operation with ID = " << operationId << endl;
	int retVal =0;
	if (operationId > 10){
		sleep (60);
		retVal =1;
		cout << " ..... returning  = " << retVal << endl;

		this->adminOperationResult( oiHandle , invocation, retVal );
		return;
	}
		sleep (3);

//	ACS_CC_ReturnType  adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result);
		retVal = 28;
		cout << " ..... returning  = " << retVal << endl;

	this->adminOperationResult( oiHandle , invocation, retVal );
	/*end*/


	printf("------------------------------------------------------------\n");


}

///////////////////
ACS_CC_ReturnType AdminOperation_MyImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " class Name: " << className << endl;
	cout << " parent Name: " << parentName << endl;
	std::string rdnNewObj;


	const char* nomeClass2 = "Test_config2";
	const char* nomeClass3 = "Test_config3";
	cout << endl;

	int toReturn = 0;

	int dim=0;
	int i=0;
	while(attr[i]){
		i++;
		dim++;
	}

	i = 0;
	while( attr[i] ){
		switch ( attr[i]->attrType ) {
		case ATTR_INT32T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<int *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_UINT32T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<long long *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_FLOATT:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<float *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_DOUBLET:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<double *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_NAMET:
		{
			cout << " attrName: " << attr[i]->attrName << "\t value: " << reinterpret_cast<char *>(attr[i]->attrValues[0]) << endl;
			std::string nome(reinterpret_cast<const char *>(attr[i]->attrName));
			if (nome.compare("RDN_Attribute") ==0 )
			{
				std::string s(reinterpret_cast<const char *>(attr[i]->attrValues[0]));
				rdnNewObj = s;
//				if(s.compare("provaConfObj2") == 0)
//					toReturn = -1;
				cout << " S string: " << s << endl;

				if(s.compare("objNameCreateTransactional2") == 0)
				general=100;
			}
		}
			break;
		case ATTR_STRINGT:
			cout << " attrName: " << attr[i]->attrName << "\t value: " << reinterpret_cast<char *>(attr[i]->attrValues[0]) << endl;
			break;
		default:
			break;

		}
		i++;
	}

	/*added*/
	//setExitCode(10, "CODE_SET_10");

//	ACS_APGCC_CcbHandle ccbHandleVal;
//	ACS_APGCC_AdminOwnerHandle adminOwnerHandlerVal;
//
//	SaAisErrorT er;
//	er = saImmOiAugmentCcbInitialize(oiHandle,ccbId,&ccbHandleVal, &adminOwnerHandlerVal  );
//	if (er == SA_AIS_OK){
//		cout << " CcbAugmentationInitialize::SUCCESS " << endl;
//
//
//		SaNameT rdn = {strlen("Obj1"), "Obj1"};
//		SaNameT* nameValues[] = {&rdn};
//		SaImmAttrValuesT_2 v2 = {"ACS_BLOCK1",  SA_IMM_ATTR_SANAMET, 1, (void**)nameValues};
//		SaUint32T  int1Value1 = 7;
//		SaUint32T* int1Values[] = {&int1Value1};
//		SaImmAttrValuesT_2 v1 = {"attr1", SA_IMM_ATTR_SAUINT32T, 1, (void**)int1Values};
//		const SaImmAttrValuesT_2 * attrValues[] = {&v1, &v2, NULL};
//
//		/*costruisce dn oggetto creato*/
//		std::string dnOBJ = rdnNewObj;
//		dnOBJ.append(",");
//		dnOBJ.append(parentName);
//		cout << " ----------dnOBJ " <<dnOBJ<< endl;
//
//
//		//const SaNameT parentName = {strlen("opensafImm=opensafImm,safApp=safImmService"), "opensafImm=opensafImm,safApp=safImmService"};
//		//const SaNameT *objectNames[] = {&parentName, NULL};
//
//		SaNameT parentName;
//		parentName.length =dnOBJ.size();
//		memcpy(parentName.value, dnOBJ.c_str(), dnOBJ.size());
//
//		const SaNameT *objectNames[] = {&parentName, NULL};
//
//		er = saImmOmCcbObjectCreate_2(ccbHandleVal, (SaImmClassNameT)className, &parentName, attrValues );
//		if ( er == SA_AIS_OK ){
//			cout << " saImmOmCcbObjectCreate_2 OK " << endl;
//			if ((er = saImmOmCcbApply(ccbHandleVal))!= SA_AIS_OK){
//				cout << " saImmOmCcbObjectCreate_2 OK " << endl;
//				cout << "  erorcode er =  " <<er<< endl;
//			}
//			return ACS_CC_SUCCESS;/*che succede se ritornano success**/
//
//		}else{
//			cout << " saImmOmCcbObjectCreate_2 KO " << endl;
//			cout << "  erorcode er =  " <<er<< endl;
//			return ACS_CC_SUCCESS;/*che succede se ritornano success**/
//		}
//	}

/*1963*/
	/*as a consequence of creating provaConfObj1 crete a child */
	if (rdnNewObj.compare("provaConfObj1")== 0){
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	std::string dnActuallyCreatingObj = rdnNewObj;
	dnActuallyCreatingObj.append(","); /*prepare the DN of the actually creating obj*/
	dnActuallyCreatingObj.append(parentName);/*now dnActuallyCreatingObj holds the name of the actually creating obj*/

	std::string dnFirstChild;
	std::string dnSecondtChild;

	/*First elem*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr2;
	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/*Fill the rdn Attribute */
	char attrdn[]= "RDN_Attribute";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_NAMET;
	attrRdn.attrValuesNum = 1;
	char* rdnValue = const_cast<char *>("FirstChild");
	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

	/*Fill the ATTRIBUTE_1 */
	char name_attFloat[]="ATTRIBUTE_1";
	attr2.attrName = name_attFloat;
	attr2.attrType = ATTR_FLOATT;
	attr2.attrValuesNum = 1;
	float floatValue = 123.678;
	attr2.attrValues = new void*[1];
	attr2.attrValues[0] = reinterpret_cast<void*>(&floatValue);

	AttrList.push_back(attrRdn);
	AttrList.push_back(attr2);



	ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
	if (retVal != ACS_CC_SUCCESS){
		cout << " getCcbAugmentationInitialize::ACS_CC_FAILURE " << endl;
	}else{
		cout << " getCcbAugmentationInitialize::ACS_CC_SUCCESS " << endl;

		/*prepare to create first son*/
		retVal = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, className, dnActuallyCreatingObj.c_str(), AttrList);
		if (retVal != ACS_CC_SUCCESS){
			cout << " createObjectAugmentCcb::ACS_CC_FAILURE " << endl;

		}else {
			/*prepare to apply ccb*/
			retVal = applyAugmentCcb (ccbHandleVal);
			if (retVal != ACS_CC_SUCCESS){
				cout << " applyAugmentCcb::ACS_CC_FAILURE " << endl;
			}else{
				cout << " applyAugmentCcb::ACS_CC_SUCCESS " << endl;
			}

		}

	}


	cout << endl;
	}else if (rdnNewObj.compare("provaConfObj3")== 0){/*as a consequence of creating obj3 modify the same element*/

		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
		std::string dnActuallyCreatingObj = rdnNewObj;
		dnActuallyCreatingObj.append(","); /*prepare the DN of the actually creating obj*/
		dnActuallyCreatingObj.append(parentName);/*now dnActuallyCreatingObj holds the name of the actually creating obj*/

		float valueFlo=11111.222;
		char name_attFloat[]="ATTRIBUTE_1";
		ACS_CC_ImmParameter par;
		par.attrName=name_attFloat;
		par.attrType=ATTR_FLOATT;
		par.attrValuesNum=1;
		par.attrValues=new void*[par.attrValuesNum];
		par.attrValues[0] =reinterpret_cast<void*>(&valueFlo);

		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
		if (retVal != ACS_CC_SUCCESS){
			cout << " getCcbAugmentationInitialize::ACS_CC_FAILURE " << endl;
		}else{
			cout << " getCcbAugmentationInitialize::ACS_CC_SUCCESS " << endl;

			/*prepare to modify */
			retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, dnActuallyCreatingObj.c_str(), &par);

			if (retVal != ACS_CC_SUCCESS){
				cout << " modifyObjectAugmentCcb::ACS_CC_FAILURE " << endl;

			}else {
				/*prepare to apply ccb*/
				retVal = applyAugmentCcb (ccbHandleVal);

				if (retVal != ACS_CC_SUCCESS){
					cout << " applyAugmentCcb::ACS_CC_FAILURE " << endl;
				}else{
					cout << " applyAugmentCcb::ACS_CC_SUCCESS " << endl;
					cout << " modifyObjectAugmentCcb::SUCCESS value for ATTRIBUTE_1 =  " <<valueFlo<< endl;
				}

			}

		}
	}else if (rdnNewObj.compare("provaConfObj4")== 0){

//		cout << " 1 provaConfObj4" << endl;
//		ACS_APGCC_CcbHandle ccbHandleVal;
//		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
//		std::string dnActuallyToDelete = "provaConfObj2,safApp=safImmService";
//
//
//		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
//		if (retVal != ACS_CC_SUCCESS){
//			cout << " getCcbAugmentationInitialize::ACS_CC_FAILURE " << endl;
//		}else{
//			cout << " 2 provaConfObj4:getCcbAugmentationInitialize_SUCCESS" << endl;
//			retVal = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, dnActuallyToDelete.c_str() );
//			if (retVal != ACS_CC_SUCCESS){
//				cout << " deleteObjectAugmentCcb::ACS_CC_FAILURE  error code: " << getInternalLastError()<< "getInternalLastErrorText:"<<getInternalLastErrorText()<<endl;
//
//			}else {
//				/*prepare to apply ccb*/
//				cout << " 3 provaConfObj4:before  applyAugmentCcb_SUCCESS" << endl;
//				retVal = applyAugmentCcb (ccbHandleVal);
//
//				if (retVal != ACS_CC_SUCCESS){
//					cout << " deleteObjectAugmentCcb::applyAugmentCcb::ACS_CC_FAILURE " << endl;
//				}else{
//					cout << " deleteObjectAugmentCcb::applyAugmentCcb::ACS_CC_SUCCESS " << endl;
//				}
//			}
//		}


/* *****************************************   directlyIMM ********** */
		SaAisErrorT errorCode;
		SaImmCcbHandleT ccbHandleVal_IMM;
		SaImmAdminOwnerHandleT adminOwnwrHandlerVal_IMM;
		std::string dnActuallyToDelete = "provaConfObj2,safApp=safImmService";

		SaNameT objToDelete;
//		objToDelete.length = strlen(dnActuallyToDelete.c_str());
//		memcpy(objToDelete.value, dnActuallyToDelete.c_str(), objToDelete.length);
		cout << " ----Setting directly------" << endl;
		objToDelete.length = strlen("provaConfObj2,safApp=safImmService");
		memcpy(objToDelete.value, "provaConfObj2,safApp=safImmService", objToDelete.length);
		const SaNameT *objOwnedNameList[] = {&objToDelete, 0};


		errorCode =  saImmOiAugmentCcbInitialize(oiHandle, ccbId, &ccbHandleVal_IMM, &adminOwnwrHandlerVal_IMM);
		if (errorCode != SA_AIS_OK ){
			cout << " saImmOiAugmentCcbInitialize FAILURE" << endl;


			}else {
				cout << " saImmOiAugmentCcbInitialize _SUCCESS" << endl;
				/*set ownership*/
				errorCode = saImmOmAdminOwnerSet (adminOwnwrHandlerVal_IMM,objOwnedNameList, SA_IMM_ONE );
				if (errorCode != SA_AIS_OK ){
					cout << " saImmOmAdminOwnerSet FAILURE" << endl;

					}else {
						cout << " saImmOmAdminOwnerSet SUCCESS" << endl;
						errorCode = saImmOmCcbObjectDelete(ccbHandleVal_IMM, &objToDelete );
						if (errorCode != SA_AIS_OK ){
						 cout << " saImmOmCcbObjectDelete FAILURE" << endl;
						}else{
							cout << " saImmOmCcbObjectDelete SUCCESS" << endl;
						}


					}


			}

/* *****************************************   end directly IMM****** */
	}else if (rdnNewObj.compare("provaConfObj5")== 0){
		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
		char* dnParent = 0;


		/*First elem*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType attr2;
		/*The vector of attributes*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;

		/*Fill the rdn Attribute */
		char attrdn[]= "RDN_Attribute";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_NAMET;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = const_cast<char *>("CreatedbyprovaConfObj5");
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

		/*Fill the ATTRIBUTE_1 */
		char name_attFloat[]="ATTRIBUTE_1";
		attr2.attrName = name_attFloat;
		attr2.attrType = ATTR_FLOATT;
		attr2.attrValuesNum = 1;
		float floatValue = 123.678;
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] = reinterpret_cast<void*>(&floatValue);

		AttrList.push_back(attrRdn);
		AttrList.push_back(attr2);



		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
		if (retVal != ACS_CC_SUCCESS){
			cout << " getCcbAugmentationInitialize::ACS_CC_FAILURE " << endl;
		}else{
			cout << " getCcbAugmentationInitialize::ACS_CC_SUCCESS " << endl;

			/*prepare to create first son*/
			retVal = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, nomeClass2, dnParent, AttrList);
			if (retVal != ACS_CC_SUCCESS){
				cout << " createObjectAugmentCcb::ACS_CC_FAILURE " << endl;

			}else {
				/*prepare to apply ccb*/
				retVal = applyAugmentCcb (ccbHandleVal);
				if (retVal != ACS_CC_SUCCESS){
					cout << " applyAugmentCcb::ACS_CC_FAILURE " << endl;
				}else{
					cout << " applyAugmentCcb::ACS_CC_SUCCESS " << endl;
				}

			}

		}
	}else if (rdnNewObj.compare("provaConfObj6")== 0){
		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
		char* dnParent = 0;


		/*First elem*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType attr2;
		/*The vector of attributes*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;

		/*Fill the rdn Attribute */
		char attrdn[]= "RDN_Attribute";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_NAMET;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = const_cast<char *>("CreatedbyprovaConfObj6");
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

		/*Fill the ATTRIBUTE_1 */
		char name_attFloat[]="ATTRIBUTE_1";
		attr2.attrName = name_attFloat;
		attr2.attrType = ATTR_FLOATT;
		attr2.attrValuesNum = 1;
		float floatValue = 123.678;
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] = reinterpret_cast<void*>(&floatValue);

		AttrList.push_back(attrRdn);
		AttrList.push_back(attr2);



		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
		if (retVal != ACS_CC_SUCCESS){
			cout << " getCcbAugmentationInitialize::ACS_CC_FAILURE " << endl;
		}else{
			cout << " getCcbAugmentationInitialize::ACS_CC_SUCCESS " << endl;

			/*prepare to create first son*/
			retVal = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, nomeClass3, dnParent, AttrList);
			if (retVal != ACS_CC_SUCCESS){
				cout << " createObjectAugmentCcb::ACS_CC_FAILURE " <<nomeClass3<< endl;


			}else {
				/*prepare to apply ccb*/
				retVal = applyAugmentCcb (ccbHandleVal);
				if (retVal != ACS_CC_SUCCESS){
					cout << " applyAugmentCcb::ACS_CC_FAILURE " << endl;
				}else{
					cout << " applyAugmentCcb::ACS_CC_SUCCESS " << endl;
				}

			}

		}
	}


/*1963*/
	if (toReturn < 0){
		setExitCode(101, "CreateFailure");
		cout << " ACS_CC_FAILURE " << endl;
		return ACS_CC_FAILURE;
	}


	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AdminOperation_MyImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	cout << endl;

/*if deleting obj provaConfObj5,safApp=safImmService modify CreatedbyprovaConfObj5*/
	if (strcmp(objName, "provaConfObj5,safApp=safImmService") ==0)
	{
		/* the object CreatedbyprovaConfObj5 will be modified too*/
		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
		std::string dnToModify = "CreatedbyprovaConfObj5";

		float valueFlo=44444.4444;
		char name_attFloat[]="ATTRIBUTE_1";
		ACS_CC_ImmParameter par;
		par.attrName=name_attFloat;
		par.attrType=ATTR_FLOATT;
		par.attrValuesNum=1;
		par.attrValues=new void*[par.attrValuesNum];
		par.attrValues[0] =reinterpret_cast<void*>(&valueFlo);

		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
		if (retVal != ACS_CC_SUCCESS){
			cout << " getCcbAugmentationInitialize::ACS_CC_FAILURE " << endl;
		}else{
			cout << " getCcbAugmentationInitialize::ACS_CC_SUCCESS " << endl;

			/*prepare to modify */
			retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, dnToModify.c_str(), &par);

			if (retVal != ACS_CC_SUCCESS){
				cout << " modifyObjectAugmentCcb::ACS_CC_FAILURE " << endl;

			}else {
				/*prepare to apply ccb*/
				retVal = applyAugmentCcb (ccbHandleVal);

				if (retVal != ACS_CC_SUCCESS){
					cout << " applyAugmentCcb::ACS_CC_FAILURE " << endl;
				}else{
					cout << " applyAugmentCcb::ACS_CC_SUCCESS " << endl;
					cout << " modifyObjectAugmentCcb::SUCCESS value for ATTRIBUTE_1 =  " <<valueFlo<< endl;
				}

			}

		}
		/*end modify too*/

	}
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AdminOperation_MyImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	int toReturn = 0;

	cout << endl;

	int i = 0;
	while( attrMods[i] ){
		switch ( attrMods[i]->modAttr.attrType ) {
		case ATTR_INT32T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_UINT32T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<long long *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_FLOATT:
		{
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<float *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			std::string nome(reinterpret_cast<const char *>(attrMods[i]->modAttr.attrName ));
			if (nome.compare("ATTRIBUTE_1") ==0 )
			{
				float s(*(reinterpret_cast<float *>(attrMods[i]->modAttr.attrValues[0])));
				if(s >  100)
					toReturn = -1;
				cout << " S float: " << s << endl;
			}
		}

			break;
		case ATTR_DOUBLET:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<double *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_NAMET:

			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " << reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]) << endl;

			break;

		case ATTR_STRINGT:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " << reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		default:
			break;

		}
		i++;
	}


	cout << endl;


	if (toReturn < 0){
		setExitCode(102, "ModifyFailure");
		cout << "AdminOperation_MyImplementer::modify:: ACS_CC_FAILURE " << endl;
		return ACS_CC_FAILURE;
	}


	/*modify ok  the object CreatedbyprovaConfObj5 will be modified too*/
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	std::string dnToModify = "CreatedbyprovaConfObj5";

	float valueFlo=3333.333;
	char name_attFloat[]="ATTRIBUTE_1";
	ACS_CC_ImmParameter par;
	par.attrName=name_attFloat;
	par.attrType=ATTR_FLOATT;
	par.attrValuesNum=1;
	par.attrValues=new void*[par.attrValuesNum];
	par.attrValues[0] =reinterpret_cast<void*>(&valueFlo);

	ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
	if (retVal != ACS_CC_SUCCESS){
		cout << " getCcbAugmentationInitialize::ACS_CC_FAILURE " << endl;
	}else{
		cout << " getCcbAugmentationInitialize::ACS_CC_SUCCESS " << endl;

		/*prepare to modify */
		retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, dnToModify.c_str(), &par);

		if (retVal != ACS_CC_SUCCESS){
			cout << " modifyObjectAugmentCcb::ACS_CC_FAILURE " << endl;

		}else {
			/*prepare to apply ccb*/
			retVal = applyAugmentCcb (ccbHandleVal);

			if (retVal != ACS_CC_SUCCESS){
				cout << " applyAugmentCcb::ACS_CC_FAILURE " << endl;
			}else{
				cout << " applyAugmentCcb::ACS_CC_SUCCESS " << endl;
				cout << " modifyObjectAugmentCcb::SUCCESS value for ATTRIBUTE_1 =  " <<valueFlo<< endl;
			}

		}

	}
	/*end modify too*/



	cout << "AdminOperation_MyImplementer::modify:: ACS_CC_Success " << endl;
	return ACS_CC_SUCCESS;


}

ACS_CC_ReturnType AdminOperation_MyImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
	if (general == 100){
		setExitCode(567, "CompleteFailure");
		return ACS_CC_FAILURE;

	}else{
	setExitCode(0, "SUCCESS_EXIT_CODE");
	}
	return ACS_CC_SUCCESS;


}

void AdminOperation_MyImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}

void AdminOperation_MyImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){


	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}


ACS_CC_ReturnType AdminOperation_MyImplementer::updateRuntime(const char* p_objName, const char* p_attrName){

	printf("------------------------------------------------------------\n");
	printf("                   updateRuntime called                     \n");
	printf("------------------------------------------------------------\n");

	cout<<"Object Name: "<<p_objName<<endl;
	cout<<"Attribute Name: "<<p_attrName<<endl;


	printf("------------------------------------------------------------\n");
	return ACS_CC_SUCCESS;

}

