/*
 * acs_apgcc_objectimplementerinterface_impl.cpp
 *
 *  Created on: Mar 3, 2011
 *      Author: xfabron
 */


#include "acs_apgcc_objectimplementerinterface_impl.h"
#include "ace/Reactor.h"
#include <iostream>
#include <vector>
#include "ACS_TRA_trace.h"



namespace {
	ACS_TRA_trace trace(const_cast<char*>("ACS_APGCC_ObjectImplementerInterface"), const_cast<char *>("C300"));
}



acs_apgcc_objectimplementerinterface_impl::acs_apgcc_objectimplementerinterface_impl():objName(), impName(){

	immOiHandle = 0;
	selObj = ACE_INVALID_HANDLE;
	log.Open("APGCC");
}



acs_apgcc_objectimplementerinterface_impl::acs_apgcc_objectimplementerinterface_impl(string p_impName ):objName(), impName(p_impName){

	immOiHandle = 0;
	selObj = ACE_INVALID_HANDLE;
	log.Open("APGCC");
}



acs_apgcc_objectimplementerinterface_impl::acs_apgcc_objectimplementerinterface_impl(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ) {

	objName = p_objName;
	impName = p_impName;
	scope = p_scope;
	immOiHandle = 0;
	selObj = ACE_INVALID_HANDLE;
	log.Open("APGCC");


}



acs_apgcc_objectimplementerinterface_impl::acs_apgcc_objectimplementerinterface_impl(acs_apgcc_objectimplementerinterface_impl &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;
	this->immOiHandle = objImp.immOiHandle;
	this->selObj = objImp.selObj;

}



acs_apgcc_objectimplementerinterface_impl& acs_apgcc_objectimplementerinterface_impl::operator=(const acs_apgcc_objectimplementerinterface_impl &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;
	this->immOiHandle = objImp.immOiHandle;
	this->selObj = objImp.selObj;

	return *this;
}



std::string acs_apgcc_objectimplementerinterface_impl::getObjName(){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::getObjName()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->objName;
}



void acs_apgcc_objectimplementerinterface_impl::setObjName(std::string p_objName){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::setObjName(std::string p_objName)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->objName = p_objName;
}



std::string acs_apgcc_objectimplementerinterface_impl::getImpName(){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::getImpName()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->impName;
}



void acs_apgcc_objectimplementerinterface_impl::setImpName(std::string p_impName){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::setImpName(std::string p_impName)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->impName = p_impName;
}



SaUint64T acs_apgcc_objectimplementerinterface_impl::getImmOiHandle(){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::getImmOiHandle()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->immOiHandle;

}



void acs_apgcc_objectimplementerinterface_impl::setImmOiHandle(SaUint64T immOiHandle){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::setImmOiHandle(SaUint64T immOiHandle)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->immOiHandle =  immOiHandle;
}



ACS_APGCC_ScopeT acs_apgcc_objectimplementerinterface_impl::getScope(){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::getScope()");
		trace.ACS_TRA_event(1, buffer);
	}

	return this->scope;
}



void acs_apgcc_objectimplementerinterface_impl::setScope(ACS_APGCC_ScopeT p_scope){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::setScope(ACS_APGCC_ScopeT p_scope)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->scope = p_scope;
}



int acs_apgcc_objectimplementerinterface_impl::getSelObj() const{

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::getSelObj() const");
		trace.ACS_TRA_event(1, buffer);
	}

	return (int)selObj;
}



void acs_apgcc_objectimplementerinterface_impl::setSelObj(SaSelectionObjectT p_selObj){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::setSelObj(SaSelectionObjectT p_selObj)");
		trace.ACS_TRA_event(1, buffer);
	}

	this->selObj = p_selObj;
}



ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_impl::dispatch(ACS_APGCC_DispatchFlags p_flag){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::dispatch(ACS_APGCC_DispatchFlags p_flag)");
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

		log.Write("ACS_APGCC_ObjectImplementerInterface::dispatch():saImmOiDispatch:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}



/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_impl::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue)");
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

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_ObjectImplementerInterface::modifyRuntimeObj():saImmOiRtObjectUpdate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}


void acs_apgcc_objectimplementerinterface_impl::setExitCode(int p_exitCode, string p_exitCodeMessage ) {

	//exitCode = p_exitCode;

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::setExitCode(int p_exitCode, string p_exitCodeMessage )");
		trace.ACS_TRA_event(1, buffer);
	}

	char buffer[1024] = {0};
	sprintf(buffer, "Error Code Set %d", p_exitCode);

	log.Write("ACS_APGCC_ObjectImplementerInterface::setExitCode():", LOG_LEVEL_ERROR);
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

void acs_apgcc_objectimplementerinterface_impl::setExitCode(int p_exitCode ) {

	/*to be deleted*/
	exitCode = p_exitCode;


}

string acs_apgcc_objectimplementerinterface_impl::getExitCodeString( ) {

	if (trace.ACS_TRA_ON()){
			char buffer[1024] = {0};
			sprintf(buffer, "ACS_APGCC_ObjectImplementerInterface::getExitCodeString()");
			trace.ACS_TRA_event(1, buffer);
		}

	return exitCodeString;
}

SaAisErrorT acs_apgcc_objectimplementerinterface_impl::getExitCode( ) {


		return SA_AIS_OK;
}



acs_apgcc_objectimplementerinterface_impl::~acs_apgcc_objectimplementerinterface_impl(){

	log.Close();

}
