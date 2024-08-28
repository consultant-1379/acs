/*
 * acs_apgcc_objectimplementerinterface_V3_impl.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_objectimplementerinterface_V3_impl.h"


#include "ace/Reactor.h"
#include <iostream>
#include <vector>
#include "ACS_TRA_trace.h"
#include "acs_apgcc_error.h"



namespace {
	ACS_TRA_trace trace(const_cast<char*>("acs_apgcc_objectimplementerinterface_V3"), const_cast<char *>("C300"));
}



acs_apgcc_objectimplementerinterface_V3_impl::acs_apgcc_objectimplementerinterface_V3_impl():objName(), impName(){

	immOiHandle = 0;
	selObj = ACE_INVALID_HANDLE;
	log.Open("APGCC");
	errorCode = 0;
}



acs_apgcc_objectimplementerinterface_V3_impl::acs_apgcc_objectimplementerinterface_V3_impl(string p_impName ):objName(), impName(p_impName){

	immOiHandle = 0;
	selObj = ACE_INVALID_HANDLE;
	log.Open("APGCC");
	errorCode = 0;
}



acs_apgcc_objectimplementerinterface_V3_impl::acs_apgcc_objectimplementerinterface_V3_impl(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ) {

	objName = p_objName;
	impName = p_impName;
	scope = p_scope;
	immOiHandle = 0;
	selObj = ACE_INVALID_HANDLE;
	log.Open("APGCC");
	errorCode = 0;


}



acs_apgcc_objectimplementerinterface_V3_impl::acs_apgcc_objectimplementerinterface_V3_impl(acs_apgcc_objectimplementerinterface_V3_impl &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;
	this->immOiHandle = objImp.immOiHandle;
	this->selObj = objImp.selObj;
	errorCode = 0;

}



acs_apgcc_objectimplementerinterface_V3_impl& acs_apgcc_objectimplementerinterface_V3_impl::operator=(const acs_apgcc_objectimplementerinterface_V3_impl &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;
	this->immOiHandle = objImp.immOiHandle;
	this->selObj = objImp.selObj;
	errorCode = 0;

	return *this;
}



std::string acs_apgcc_objectimplementerinterface_V3_impl::getObjName(){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::getObjName()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->objName;
}



void acs_apgcc_objectimplementerinterface_V3_impl::setObjName(std::string p_objName){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::setObjName(std::string p_objName)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->objName = p_objName;
}



std::string acs_apgcc_objectimplementerinterface_V3_impl::getImpName(){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::getImpName()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->impName;
}



void acs_apgcc_objectimplementerinterface_V3_impl::setImpName(std::string p_impName){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::setImpName(std::string p_impName)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->impName = p_impName;
}



SaUint64T acs_apgcc_objectimplementerinterface_V3_impl::getImmOiHandle(){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::getImmOiHandle()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->immOiHandle;

}



void acs_apgcc_objectimplementerinterface_V3_impl::setImmOiHandle(SaUint64T immOiHandle){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::setImmOiHandle(SaUint64T immOiHandle)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->immOiHandle =  immOiHandle;
}



ACS_APGCC_ScopeT acs_apgcc_objectimplementerinterface_V3_impl::getScope(){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::getScope()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->scope;
}



void acs_apgcc_objectimplementerinterface_V3_impl::setScope(ACS_APGCC_ScopeT p_scope){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::setScope(ACS_APGCC_ScopeT p_scope)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->scope = p_scope;
}



int acs_apgcc_objectimplementerinterface_V3_impl::getSelObj() const{

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::getSelObj() const");
		trace.ACS_TRA_event(1, buffer);
	}

	return (int)selObj;
}



void acs_apgcc_objectimplementerinterface_V3_impl::setSelObj(SaSelectionObjectT p_selObj){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::setSelObj(SaSelectionObjectT p_selObj)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->selObj = p_selObj;
}



ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::dispatch(ACS_APGCC_DispatchFlags p_flag){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::dispatch(ACS_APGCC_DispatchFlags p_flag)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaDispatchFlagsT flag = (SaDispatchFlagsT) p_flag;

	/** This function invokes pending callbacks for handle immOiHandle in the way
	 * that is specified by the flag parameter**/
	errorCode = saImmOiDispatch(this->immOiHandle, flag);

	if (errorCode != SA_AIS_OK){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::dispatch():saImmOiDispatch:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}



/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaNameT *parSaName = 0;
	vector<SaNameT*> parSaNameVector;

	/*objectName. The Name of the object whose attribute has to be modified*/
	SaNameT objectName  = {0,{0}};

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2* attrMods[2] = {0, 0} ;

	SaImmAttrModificationT_2 attr;


	/*Prepares the OBJECT NAME to be passed to saImmOiRtObjectUpdate_2()*/
	objectName.length = strlen(p_objName);
	memcpy(objectName.value, p_objName, strlen(p_objName));

	/*Prepares data of the attribute to be change */
	/*Set the type of modification to apply on the object attribute*/
	attr.modType=SA_IMM_ATTR_VALUES_REPLACE;

	/*Set the attribute name*/
	attr.modAttr.attrName = (*p_attributeValue).attrName;

	/*Set the attribute Type*/
	attr.modAttr.attrValueType = (SaImmValueTypeT)(*p_attributeValue).attrType;

	/*Set the Attribute Values Number*/
	attr.modAttr.attrValuesNumber = (*p_attributeValue).attrValuesNum;

	void* attrValues[(*p_attributeValue).attrValuesNum];
	//attr.modAttr.attrValues = new void*[(*p_attributeValue).attrValuesNum];
	attr.modAttr.attrValues = attrValues;

	/*Set the new values of the attributes*/
	if(attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET){

		for(unsigned int i = 0; i < (*p_attributeValue).attrValuesNum; i++){
			parSaName = new SaNameT;
			parSaName->length = strlen(reinterpret_cast<char*>((*p_attributeValue).attrValues[i]));
			memcpy(parSaName->value, ((*p_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}

	}else if(attr.modAttr.attrValueType == SA_IMM_ATTR_SASTRINGT){

		for(unsigned int i = 0; i<(*p_attributeValue).attrValuesNum; i++){
			attr.modAttr.attrValues[i] = &((*p_attributeValue).attrValues[i]);
		}

	}else{
		attr.modAttr.attrValues = (*p_attributeValue).attrValues;
	}


	/*Prepares the LIST OF THE ATTRIBUTES TO BE CHANGE. It must be a NULL terminated array*/
	attrMods[0] = &attr;
	attrMods[1] = 0;

	/* saImmOiRtObjectUpdate_2 update runtime object of a configuration or runtime object.*/
	errorCode = saImmOiRtObjectUpdate_2(this->immOiHandle, &objectName, (const SaImmAttrModificationT_2**)attrMods );

	/*to free allocate memory*/
	if(parSaNameVector.size() > 0){
		vector<SaNameT *>::iterator it_2;
		for(it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); it_2++){
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}


	if(errorCode!=SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[20] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj():saImmOiRtObjectUpdate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}

/*adminOperationResult*/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	errorCode = saImmOiAdminOperationResult(oiHandle, invocation, (SaAisErrorT)result );

	if (errorCode != SA_AIS_OK){

		//setInternalError(errorCode);

		char buffer[200];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::adminOperationResult::saImmOiAdminOperationResult:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;


}

ACS_CC_ReturnType  acs_apgcc_objectimplementerinterface_V3_impl::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result, vector<ACS_APGCC_AdminOperationParamType> outParamVector){

	if (trace.ACS_TRA_ON()){
			char buffer[1024] = {0};
			sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result, vector<ACS_APGCC_AdminOperationParamType> outParamVector)");
			trace.ACS_TRA_event(1, buffer);
		}

		SaAisErrorT errorCode;

		/**Prepare the list of return parameters*/

		SaImmAdminOperationParamsT_2 **operationOutParamList = 0;

		/*allocate memory for the list of ptr*/

		int numOfParam = outParamVector.size();

		operationOutParamList = new SaImmAdminOperationParamsT_2*[numOfParam + 1];

		SaImmAdminOperationParamsT_2 singleParamVector[numOfParam];

		vector<ACS_APGCC_AdminOperationParamType>::iterator it;

		int i = 0;

		for (it = outParamVector.begin(); it != outParamVector.end(); it++){

			/*Set the attribute Name*/
			//(*operationOutParamList[i]).paramName = reinterpret_cast<SaStringT>(((*it).attrName));

			/*Set param Type*/
			//(*operationOutParamList[i]).paramType = (SaImmValueTypeT)(((*it).attrType));

			if ( (((SaImmValueTypeT)(*it).attrType) == ATTR_NAMET) || (((SaImmValueTypeT)(*it).attrType) == ATTR_STRINGT) ){
//				(*operationOutParamList[i]).paramType = SA_IMM_ATTR_SASTRINGT;
//				(*operationOutParamList[i]).paramBuffer = &((*it).attrValues);

				singleParamVector[i].paramType = SA_IMM_ATTR_SASTRINGT;
				singleParamVector[i].paramName = reinterpret_cast<SaStringT>(((*it).attrName));
				singleParamVector[i].paramBuffer = &(*it).attrValues;
				operationOutParamList[i] = &(singleParamVector[i]);
				i++;
			}else{

				/*Set the param value*/
				singleParamVector[i].paramName = reinterpret_cast<SaStringT>(((*it).attrName));
				singleParamVector[i].paramBuffer = (*it).attrValues;
				singleParamVector[i].paramType = (SaImmValueTypeT)(((*it).attrType));
				operationOutParamList[i] = &(singleParamVector[i]);
				i++;
			}

			//i++;
		}

		/*last element in the parameter list must be 0*/
		operationOutParamList[i] = 0;
		/*END Prepare the list of return parameters*/

		errorCode = saImmOiAdminOperationResult_o2(
					oiHandle,
					invocation,
					(SaAisErrorT)result,
					(const SaImmAdminOperationParamsT_2**) operationOutParamList
					);
		/*free allocated memory*/
		delete[] operationOutParamList;

		if (errorCode != SA_AIS_OK){

			//setInternalError(errorCode);

			char buffer[200];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_objectimplementerinterface_V3::adminOperationResult::saImmOiAdminOperationResult:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		return ACS_CC_SUCCESS;
}


void acs_apgcc_objectimplementerinterface_V3_impl::setExitCode(int p_exitCode, string p_exitCodeMessage ) {

	//exitCode = p_exitCode;

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::setExitCode(int p_exitCode, string p_exitCodeMessage )");
		trace.ACS_TRA_event(1, buffer);
	}

	char buffer[1024] = {0};
	sprintf(buffer, "Error Code Set %d", p_exitCode);

	log.Write("acs_apgcc_objectimplementerinterface_V3::setExitCode():", LOG_LEVEL_ERROR);
	log.Write(buffer, LOG_LEVEL_ERROR);


	/*create the string to be passed to CMW API*/
	stringstream concatenateString;
	/*added for COMSA Rule 101 */
	concatenateString << "@ComNbi@";
	/*end added for rule 101*/
	concatenateString << p_exitCodeMessage;
	/* the following two lines commented since COMSA Rule 101 must be applied and COM has no way to split
	 * string and int: temporary solution
	concatenateString << "__";
	concatenateString << p_exitCode;
	 */
	exitCodeString = concatenateString.str();


}

void acs_apgcc_objectimplementerinterface_V3_impl::setExitCode(int p_exitCode ) {

	/*to be deleted*/
	exitCode = p_exitCode;


}

string acs_apgcc_objectimplementerinterface_V3_impl::getExitCodeString( ) {

	if (trace.ACS_TRA_ON()){
			char buffer[1024] = {0};
			sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::getExitCodeString()");
			trace.ACS_TRA_event(1, buffer);
		}

	return exitCodeString;
}

SaAisErrorT acs_apgcc_objectimplementerinterface_V3_impl::getExitCode( ) {


		return SA_AIS_OK;
}
//*1963*/

//---------------------------------------------------------------------------------------------------------------------------
// GET INTERNAL LAST ERROR METHOD
//---------------------------------------------------------------------------------------------------------------------------
int acs_apgcc_objectimplementerinterface_V3_impl::getInternalLastError(){

	return ((-1)*errorCode);
}



//---------------------------------------------------------------------------------------------------------------------------
// SET INTERNAL LAST ERROR METHOD
//---------------------------------------------------------------------------------------------------------------------------
void acs_apgcc_objectimplementerinterface_V3_impl::setInternalError(int p_errorCode){

	errorCode = p_errorCode;

}



//---------------------------------------------------------------------------------------------------------------------------
// GET INTERNAL LAST ERROR TEXT METHOD
//---------------------------------------------------------------------------------------------------------------------------
const char* acs_apgcc_objectimplementerinterface_V3_impl::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------
// getCcbAugmentationInitialize
//---------------------------------------------------------------------------------------------------------------------------

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::getCcbAugmentationInitialize(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, ACS_APGCC_CcbHandle* ccbHandle, ACS_APGCC_AdminOwnerHandle* adminOwnerHandler){
	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::getCcbAugmentationInitialize()");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	errorCode =  saImmOiAugmentCcbInitialize(oiHandle, ccbId, ccbHandle, adminOwnerHandler);
	if (errorCode != SA_AIS_OK ){
		char buffer[200];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::getCcbAugmentationInitialize::saImmOiAugmentCcbInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;


	}

	return ACS_CC_SUCCESS;
}
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::createObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList){
	/*start*/
	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::createObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaNameT* name = 0;
	vector<SaNameT*> SaNameVector;

	/* The parent Name of the creating OBJECT*/
	SaNameT parentName = {0,{0}} ;

	/*An iterator to access the attributes list*/
	vector<ACS_CC_ValuesDefinitionType>::iterator it;

	/*The attribute list to be passed to the function saImmOmCcbObjectCreate_2*/
	SaImmAttrValuesT_2 **attributesValues = 0;

	/*to set the adminowner create the owned obj list*/
	const SaNameT *objOwnedNameList[] = {&parentName, 0};

	/*Build the SaNameT variable to be passed to the  saImmOmCcbObjectCreate_2 function*/
	if (p_parentName != 0){

		/*Set in parentName.length the m_parentName string length*/
		parentName.length = strlen(p_parentName);

		/*Copy in parentName.value the m_parentName value*/
		memcpy(parentName.value, p_parentName, parentName.length);

		/*set the admin ownwer to the obj to be created*/
		errorCode = saImmOmAdminOwnerSet(adminOwnerHandler,objOwnedNameList, SA_IMM_ONE );

		if (errorCode != SA_AIS_OK)	{

			setInternalError(errorCode);

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_objectimplementerinterface_V3::createObjectAugmentCcb():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release adminOwnerHandler is  not required since it has to be performed when releasing the main one*/

			return ACS_CC_FAILURE;
		}
	}


	/*no initialization requested for ccb and adminowner since a previous ccb is being augmented*/

	/* Create the list of Attribute Values to be provided as input to the saImmOmCcbObjectCreate_2 function*/
	/* allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
	attributesValues = new SaImmAttrValuesT_2*[p_attrValuesList.size()+1];

	int numAttribute = p_attrValuesList.size();

	SaImmAttrValuesT_2 singleAttributeValues[numAttribute];

	int i =0;

	for(it = p_attrValuesList.begin(); it != p_attrValuesList.end(); it++  ){

		/*Set the attribute Name*/
		singleAttributeValues[i].attrName = (SaImmAttrNameT)((*it).attrName);

		/*Set the attribute Type*/
		singleAttributeValues[i].attrValueType = (SaImmValueTypeT)((*it).attrType);

		/*Set the Attribute Values Number*/
		unsigned int numElem = (*it).attrValuesNum;
		singleAttributeValues[i].attrValuesNumber = (SaUint32T)(numElem);

		/*Set attribute values To be done*/
		singleAttributeValues[i].attrValues = new void*[numElem];

		for( unsigned int j=0; j<numElem; j++){

			switch ((*it).attrType){
			case ATTR_INT32T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT32T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_INT64T:
			case ATTR_TIMET:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT64T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_DOUBLET:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_FLOATT:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_STRINGT:
				singleAttributeValues[i].attrValues[j] = &((*it).attrValues[j]);
				break;

			case ATTR_NAMET:
				name = new SaNameT;
				unsigned int howlong ;

				howlong = strlen ( reinterpret_cast<char*>((*it).attrValues[j]));
				name->length =  howlong ;

				memcpy(name->value, (*it).attrValues[j], name->length);
				singleAttributeValues[i].attrValues[j] = reinterpret_cast<void*>(name);

				/**Used to deallocate memory for name variables **/
				SaNameVector.push_back(name);
				break;

			default:
				break;

			}/*end switch case on the attribute Type*/

		}/**End for on each multiple value for a single attribute*/
		/*END set Atribute values*/

		/*add the singleAttributeValues to the vector attributesValues*/
		attributesValues[i] = &(singleAttributeValues[i]);

		i++;

	}/*end for that goes through the input vector and builds the list to be passed to the function*/

	/*LAst element in attributes Value list must be NULL*/
	attributesValues[i] = 0;


	errorCode = saImmOmCcbObjectCreate_2 ( ccbHandle, (SaImmClassNameT)p_className, &parentName, (const SaImmAttrValuesT_2**)(attributesValues) );

	/* delete allocated memory*/
	i = 0;
	for(it = p_attrValuesList.begin(); it != p_attrValuesList.end(); it++  ){

		delete[] attributesValues[i]->attrValues;
		i++;

	}

	if(SaNameVector.size()>0){
		vector<SaNameT *>::iterator it_2;
		for(it_2 = SaNameVector.begin(); it_2 != SaNameVector.end(); it_2++){
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}
	delete[] attributesValues;


	if (errorCode != SA_AIS_OK ){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::createObjectAugmentCcb:saImmOmCcbObjectCreate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

	/*end*/


}
//---------------------------------------------------------------------------------------------------------------------------
//	 modifyObjectAugmentCcb
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::modifyObjectAugmentCcb( ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue){
	/*start*/

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface::modifyObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;
	SaNameT *parSaName = 0;
	vector<SaNameT*> parSaNameVector;

	/*objectName. The Name of the object whose attribute has to be modified*/
	SaNameT objectName;

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2* attrMods[2] = {0, 0};

	SaImmAttrModificationT_2 attr;


	/*Prepares the OBJECT NAME to be passed to saImmOmCcbObjectModify_2()*/
	objectName.length = strlen(p_objectName);
	memcpy(objectName.value, p_objectName, strlen(p_objectName));

	/*for set the Admin Owner prepare the list of obj*/
	const SaNameT *objOwnedNameList[] = {&objectName, 0};

	/*set the admin owner*/
	errorCode = saImmOmAdminOwnerSet (adminOwnerHandler,objOwnedNameList, SA_IMM_ONE );

	if (errorCode != SA_AIS_OK)	{

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::modifyObjectAugmentCcb()::saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release augmented ownerHandle  is not required it is performed when performing the main handle */

		return ACS_CC_FAILURE;
	}

	/*Prepares data of the attribute to be change */
	/*Set the type of modification to apply on the object attribute*/
	attr.modType=SA_IMM_ATTR_VALUES_REPLACE;

	/*Set the attribute name*/
	attr.modAttr.attrName = (*p_attributeValue).attrName;

	/*Set the attribute Type*/
	attr.modAttr.attrValueType = (SaImmValueTypeT)(*p_attributeValue).attrType;

	/*Set the Attribute Values Number*/
	attr.modAttr.attrValuesNumber = (*p_attributeValue).attrValuesNum;

	/*Set the array where is memorize the new value of attribute*/
	void* attrValues[(*p_attributeValue).attrValuesNum];
	attr.modAttr.attrValues = attrValues;

	/*Set the new values of the attributes*/
	if(attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET){

		for(unsigned int i=0; i<(*p_attributeValue).attrValuesNum; i++){
			parSaName = new SaNameT;
			parSaName->length = strlen(reinterpret_cast<char*>((*p_attributeValue).attrValues[i]));
			memcpy(parSaName->value, ((*p_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}

	}else if(attr.modAttr.attrValueType==SA_IMM_ATTR_SASTRINGT){

		for(unsigned int i=0; i<(*p_attributeValue).attrValuesNum; i++){
			attr.modAttr.attrValues[i] = &((*p_attributeValue).attrValues[i]);
		}

	}else{
		attr.modAttr.attrValues = (*p_attributeValue).attrValues;
	}


	/*Prepares the LIST OF THE ATTRIBUTES TO BE CHANGE. It must be a NULL terminated array*/
	attrMods[0] = &attr;
	attrMods[1] = 0;

	/* saImmOmCcbObjectModify_2 adds to the CCB identified by its handle ccbHandle a request to modify
	 * configuration attributes of an IMM Service object.*/
	errorCode=saImmOmCcbObjectModify_2(ccbHandle, &objectName, (const SaImmAttrModificationT_2**)attrMods );

	/*to free allocate memory*/
	if(parSaNameVector.size() > 0){
		vector<SaNameT *>::iterator it_2;
		for(it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); it_2++){
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}

	if(errorCode!=SA_AIS_OK){


		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface::modifyObjectAugmentCcb::saImmOmCcbObjectModify_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	return ACS_CC_SUCCESS;

	/*end*/
}


//---------------------------------------------------------------------------------------------------------------------------
// 	 deleteObjectAugmentCcb
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::deleteObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* m_objectName){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::deleteObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* m_objectName)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(m_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, m_objectName, objToDelete.length);

	/*prepare to set the admin owner create the list of obj*/
	const SaNameT *objOwnedNameList[] = {&objToDelete, 0};

	/*set the adminowner*/
	errorCode = saImmOmAdminOwnerSet (adminOwnerHandler,objOwnedNameList, SA_IMM_ONE );

	if (errorCode != SA_AIS_OK)	{

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3_impl::deleteObjectAugmentCcb()::saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release ownerHandler is not required it is performed when releasing the main handle*/

		return ACS_CC_FAILURE;
	}



	/*Call the saImmOmCcbObjectDelete */
	if((errorCode = saImmOmCcbObjectDelete(ccbHandle, &objToDelete ))!= SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::deleteObjectAugmentCcb::saImmOmCcbObjectDelete:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	return ACS_CC_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// applyAugmentCcb
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3_impl::applyAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_objectimplementerinterface_V3::applyAugmentCcb(ACS_APGCC_CcbHandle ccbHandle");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	if ((errorCode = saImmOmCcbApply(ccbHandle))!= SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_objectimplementerinterface_V3::applyAugmentCcb::saImmOmCcbApply:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}
	return ACS_CC_SUCCESS;
}
/*end 1963*/



acs_apgcc_objectimplementerinterface_V3_impl::~acs_apgcc_objectimplementerinterface_V3_impl(){

	log.Close();

}

