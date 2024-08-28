/*
 * ACS_APBM_HWIImmHandler.cpp
 *
 *  Created on: Oct 25, 2011
 *      Author: xlucdor
 */

#include "acs_apbm_hwiimmconfobjhandler.h"


ACS_APBM_HWIImmConfObjHandler::ACS_APBM_HWIImmConfObjHandler() 
         {
	// TODO Auto-generated constructor stub

}

ACS_APBM_HWIImmConfObjHandler::~ACS_APBM_HWIImmConfObjHandler() {
	// TODO Auto-generated destructor stub
}

void ACS_APBM_HWIImmConfObjHandler::hwiDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes) {

	std::cout << __FUNCTION__ << "@" << __LINE__ << std::endl;
	ACS_CC_AttrDefinitionType rdnHWIData = {const_cast<char *>("hardwareInventoryId"),ATTR_STRINGT,ATTR_RDN|ATTR_CONFIG,0} ;

	classAttributes.push_back(rdnHWIData);

}

void ACS_APBM_HWIImmConfObjHandler::shelfDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes) {

	ACS_CC_AttrDefinitionType rdnShelfMagazineName = {const_cast<char *>("shelfId"),ATTR_STRINGT,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType strShelfRow = {const_cast<char*>("row"),ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType strShelfNumber = {const_cast<char*>("number"),ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType strShelfXPos = {const_cast<char*>("xPosition"),ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType strShelfYPos = {const_cast<char*>("yPosition"),ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType strShelfProdName = {const_cast<char*>("productName"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strShelfProdNo = {const_cast<char*>("productNumber"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strShelfProdRev = {const_cast<char*>("productRevision"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strShelfSerialNo = {const_cast<char*>("serialNumber"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strShelfSupplier = {const_cast<char*>("supplier"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strShelfManDate = {const_cast<char*>("manufacturingDate"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strShelfName = {const_cast<char*>("name"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};


	classAttributes.push_back(rdnShelfMagazineName);
	classAttributes.push_back(strShelfRow);
	classAttributes.push_back(strShelfNumber);
	classAttributes.push_back(strShelfXPos);
	classAttributes.push_back(strShelfYPos);
	classAttributes.push_back(strShelfProdName);
	classAttributes.push_back(strShelfProdNo);
	classAttributes.push_back(strShelfProdRev);
	classAttributes.push_back(strShelfSerialNo);
	classAttributes.push_back(strShelfSupplier);
	classAttributes.push_back(strShelfManDate);
	classAttributes.push_back(strShelfName);

}

void ACS_APBM_HWIImmConfObjHandler::boardDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes) {

	ACS_CC_AttrDefinitionType rdnBoardSlot = {const_cast<char *>("boardId"),ATTR_STRINGT,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType strBoardXPos = {const_cast<char*>("xPosition"),ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType strBoardYPos = {const_cast<char*>("yPosition"),ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType strBoardProdName = {const_cast<char*>("productName"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strBoardProdNo = {const_cast<char*>("productNumber"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strBoardProdRev = {const_cast<char*>("productRevision"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strBoardSerialNo = {const_cast<char*>("serialNumber"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strBoardSupplier = {const_cast<char*>("supplier"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strBoardBusType = {const_cast<char*>("busType"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strBoardManDate = {const_cast<char*>("manufacturingDate"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};

	classAttributes.push_back(rdnBoardSlot);
	classAttributes.push_back(strBoardXPos);
	classAttributes.push_back(strBoardYPos);
	classAttributes.push_back(strBoardProdName);
	classAttributes.push_back(strBoardProdNo);
	classAttributes.push_back(strBoardProdRev);
	classAttributes.push_back(strBoardSerialNo);
	classAttributes.push_back(strBoardSupplier);
	classAttributes.push_back(strBoardBusType);
	classAttributes.push_back(strBoardManDate);
}

void ACS_APBM_HWIImmConfObjHandler::pfmDataSkeleton (vector<ACS_CC_AttrDefinitionType> &classAttributes) {

	ACS_CC_AttrDefinitionType rdnPFMInstance = {const_cast<char *>("powerFanModuleId"),ATTR_STRINGT,ATTR_RDN|ATTR_RUNTIME|ATTR_CACHED,0} ;
	ACS_CC_AttrDefinitionType strPFMProdName = {const_cast<char*>("productName"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strPFMProdNo = {const_cast<char*>("productNumber"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strPFMProdRev = {const_cast<char*>("productRevision"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strPFMSerialNo = {const_cast<char*>("productSerialNumber"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strPFMDevType = {const_cast<char*>("deviceType"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strPFMHWVer = {const_cast<char*>("hardwareVersion"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType strPFMManDate = {const_cast<char*>("manufacturingDate"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED,0};


	classAttributes.push_back(rdnPFMInstance);
	classAttributes.push_back(strPFMProdName);
	classAttributes.push_back(strPFMProdNo);
	classAttributes.push_back(strPFMProdRev);
	classAttributes.push_back(strPFMSerialNo);
	classAttributes.push_back(strPFMDevType);
	classAttributes.push_back(strPFMHWVer);
	classAttributes.push_back(strPFMManDate);
}

void ACS_APBM_HWIImmConfObjHandler::createIMMTables() {

	ACS_CC_ReturnType result;
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	OmHandler omHandler;
	result = omHandler.Init();
	if( result != ACS_CC_SUCCESS ){
		std::cout << __FUNCTION__ << "@" << __LINE__ << " INIT failed " << std::endl;
		return;
	}

	vector<ACS_CC_AttrDefinitionType> vecClass;

	hwiDataSkeleton(vecClass);
	result = omHandler.defineClass( const_cast<char *> (IMMCLASSNAME_HWI.c_str()), categoryClassToBeDefined, vecClass);


	if( result != ACS_CC_SUCCESS ){
		std::cout << " Class: " << IMMCLASSNAME_HWI << " not created " << std::endl;
		return;
	}
	std::cout << " Class: " << IMMCLASSNAME_HWI << " successfully created" << std::endl;

	vecClass.clear();

	shelfDataSkeleton(vecClass);
	result = omHandler.defineClass( const_cast<char *> (IMMCLASSNAME_HWISHELF.c_str()), categoryClassToBeDefined, vecClass);


	if( result != ACS_CC_SUCCESS ){
		std::cout << " Class: " << IMMCLASSNAME_HWISHELF << " not created " << std::endl;
		return;
	}
	std::cout << " Class: " << IMMCLASSNAME_HWISHELF << " successfully created" << std::endl;

	vecClass.clear();

	boardDataSkeleton(vecClass);
	result = omHandler.defineClass(const_cast<char *> (IMMCLASSNAME_BOARD.c_str()), categoryClassToBeDefined, vecClass);

	if( result != ACS_CC_SUCCESS ){
		std::cout << " Class: " << IMMCLASSNAME_BOARD << " not created " << std::endl;
		return;
	}
	std::cout << " Class: " << IMMCLASSNAME_BOARD << " successfully created" << std::endl;
	vecClass.clear();

	pfmDataSkeleton(vecClass);
	result = omHandler.defineClass(const_cast<char *> (IMMCLASSNAME_PFM.c_str()), RUNTIME, vecClass);

	if( result != ACS_CC_SUCCESS ){
		std::cout << " Class: " << IMMCLASSNAME_PFM << " not created " << std::endl;
		return;
	}
	std::cout << " Class: " << IMMCLASSNAME_PFM << " successfully created" << std::endl;


	result = omHandler.Finalize();

}


void ACS_APBM_HWIImmConfObjHandler::adminOperationCallback(
		ACS_APGCC_OiHandle /*oiHandle*/,
		ACS_APGCC_InvocationType /*invocation*/,
		const char* /*p_objName*/,
		ACS_APGCC_AdminOperationIdType /*operationId*/,
		ACS_APGCC_AdminOperationParamType** /*paramList*/) {

	//NOT USED
}


ACS_CC_ReturnType ACS_APBM_HWIImmConfObjHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " class Name: " << className << endl;
	cout << " parent Name: " << parentName << endl;

	cout << endl;

	if (!immOperationOnGoing){

		cout << " no operation ongoing !!!! - operation refused"<< endl;
		return ACS_CC_FAILURE;
	}

	int toReturn = 0;

	int dim=0;
	int i=0;
	while(attr[i]){
		i++;
		dim++;
	}

	i = 0;
	while( attr[i] ){
		if (attr[i]->attrValuesNum > 0) {
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
					if(s.compare("provaConfObj2") == 0)
						//toReturn = -1;
					cout << " S string: " << s << endl;

					//if(s.compare("objNameCreateTransactional2") == 0)
						//general=100;
				}
			}
				break;
			case ATTR_STRINGT:
				cout << " attrName: " << attr[i]->attrName << "\t value: " << reinterpret_cast<char *>(attr[i]->attrValues[0]) << endl;
				break;
			default:
				break;

			}
		}
		i++;
	}

	/*added*/
	//setExitCode(10, "CODE_SET_10");
	cout << endl;

	if (toReturn < 0){
		setExitCode(101, "CreateFailure");
		cout << " ACS_CC_FAILURE " << endl;
		return ACS_CC_FAILURE;
	}


	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_APBM_HWIImmConfObjHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	cout << endl;

	if (!immOperationOnGoing){

		cout << " no operation ongoing !!!! - operation refused"<< endl;
		return ACS_CC_FAILURE;
	}

	//setExitCode(11);
	//setExitCode(11, "CODE_SET_11");
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_APBM_HWIImmConfObjHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){

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
		if (attrMods[i]->modAttr.attrValuesNum > 0) {
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
	}


	cout << endl;


	//setExitCode(13);
	//setExitCode(13, "CODE_SET_13");
	if (toReturn < 0){
		setExitCode(102, "ModifyFailure");
		cout << "AdminOperation_MyImplementer::modify:: ACS_CC_FAILURE " << endl;
		return ACS_CC_FAILURE;
	}
	cout << "AdminOperation_MyImplementer::modify:: ACS_CC_Success " << endl;
	return ACS_CC_SUCCESS;


}

ACS_CC_ReturnType ACS_APBM_HWIImmConfObjHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
	/*
	if (general == 100){
		setExitCode(567, "CompleteFailure");
		general =0;
		return ACS_CC_FAILURE;

	}else{
	setExitCode(0, "SUCCESS_EXIT_CODE");
	}
	*/
	setExitCode(0, "SUCCESS_EXIT_CODE");
	return ACS_CC_SUCCESS;


}

void ACS_APBM_HWIImmConfObjHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}

void ACS_APBM_HWIImmConfObjHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){


	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}


ACS_CC_ReturnType ACS_APBM_HWIImmConfObjHandler::updateRuntime(
									const char* /*p_objName*/,
									const char* /*p_attrName*/){

	//NOT USED
	return ACS_CC_SUCCESS;
}


