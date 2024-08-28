/*
 * acs_acpgcc_omhandler_impl.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: xfabron
 */


#include "acs_apgcc_omhandler_impl.h"
#include <iostream>
#include "ACS_TRA_trace.h"
#include "acs_apgcc_error.h"

#include "ACS_APGCC_Util.H"



namespace {
	ACS_TRA_trace trace(const_cast<char*>("OmHandler"), const_cast<char *>("C300"));
}



acs_apgcc_omhandler_impl::acs_apgcc_omhandler_impl():processName() {

	immHandle = 0;
	ccbHandle = 0;
	ownerHandle = 0;
	errorCode = 0;

	//added for ccb corruption management:init ccbFlag
	ccbFlagVal = 0;

	//Initialize the admistrativeOwner for transactional operations
	memset(&objOwnedName, 0, sizeof(objOwnedName));
	objOwnedNameList[0] = &objOwnedName;
	objOwnedNameList[1] = 0;

	//Initialize the log object
	log.Open("APGCC");

}


acs_apgcc_omhandler_impl::acs_apgcc_omhandler_impl(acs_apgcc_omhandler_impl *omHanlderImp){

	this->immHandle = omHanlderImp->immHandle;
	this->ccbHandle = omHanlderImp->ccbHandle;
	this->ownerHandle = omHanlderImp->ownerHandle;
	this->errorCode = omHanlderImp->errorCode;
	this->accessorHandle = omHanlderImp->accessorHandle;
	this->errorCode = omHanlderImp->errorCode;
	this->transactionList = omHanlderImp->transactionList;
	this->version.releaseCode  = omHanlderImp->version.releaseCode;
	this->version.majorVersion =  omHanlderImp->version.majorVersion;
	this->version.minorVersion =  omHanlderImp->version.minorVersion;
	//added for ccb corruption management:init ccbFlag
	this->ccbFlagVal = omHanlderImp->ccbFlagVal;
	this->processName = omHanlderImp->processName;
	log.Open("APGCC");

	this->objOwnedName = omHanlderImp->objOwnedName;
	this->objOwnedNameList[0] = omHanlderImp->objOwnedNameList[0];
	this->objOwnedNameList[1] = omHanlderImp->objOwnedNameList[1];
	this->transactionListOwnerHandle = omHanlderImp->transactionListOwnerHandle;

}


/**acs_apgcc_omhandler_impl::Init(int p_registeredImpl) method used to initialize the handlers needed to interact with
 * IMM. The following handler are initialized:
 * immHandle
 * accessorHandle
 * This Init method has one parameter that is used to specify if the Application now registering and initializating on IMM intends to performs operations on Objects only if an Object Implementer is registered for it or regardless of Object Implementer running.
 */
ACS_CC_ReturnType acs_apgcc_omhandler_impl::Init(int p_registeredImpl) {

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::Init( )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;
	//the following line commented for ccb corruption management
	//SaImmCcbFlagsT ccbFlag=0;
	ccbFlagVal = 0;

	version.releaseCode  = 'A';
	version.majorVersion =  2;
	//version.minorVersion =  1;
	//version.minorVersion =  11; /*Ticket 1904*/
	version.minorVersion = 14; // Support IMM callback timeout

	/** Set the value for the ccbFlag according to the provided input parameter p_registeredImpl
	* if p_registeredImpl is set to NO_REGISTERED_OI then ccbFlag=0 so any change is performed regardless OI running
	* if p_registeredImpl is set to REGISTERED_OI then ccbFlag=SA_IMM_CCB_REGISTERED_OI so any change is performed only if an OI is registered for the Object to be modified
	*/
	if(p_registeredImpl == REGISTERED_OI )
	{
		ccbFlagVal = SA_IMM_CCB_REGISTERED_OI;
	}

	/**Initialize the IMM connection calling the saImmOmInitialize().
	 *
	 * immHandle is an output parameter where the function will return the handle
	 * to be used for further access to imm.
	 *
	 * the second parameter contains the collbacks
	 *
	 *  Version is an input parameter containing the info about which imm version
	 *  the code is compliant to.
	 *
	 */
	errorCode = saImmOmInitialize(&immHandle, 0, &version);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::Init():saImmOmInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/**Obtain accessorHandle */
	if ((errorCode=saImmOmAccessorInitialize(immHandle, &accessorHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::Init():saImmOmAccessorInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release immhandle*/
		if((errorCode=saImmOmFinalize(immHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::Init():saImmOmFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

		}
		/*end release immhandle*/

		return ACS_CC_FAILURE;

	}/*end if on saImmOmAccessorInitialize*/

	/**The process name will be used as name of Administrative OWner*/
//	char pName[1024];
//	::memset(pName, 0, 1025);
	string pName;
	ACS_APGCC::getProcessName(&pName);
	if(pName.length() != 0){
		processName = pName;
	}else{
		processName = "unknownProcess";
	}

//	char bufferInfo[1024] = {0};
//	sprintf(bufferInfo, "immHandle = %llu ", immHandle);
//	log.Write("Init()::", LOG_LEVEL_INFO);
//	log.Write(bufferInfo, LOG_LEVEL_INFO);

	return ACS_CC_SUCCESS;



}
/* END Added for new Init method*****/



/**acs_apgcc_omhandler_impl::Finalize() method used to release all handler retrieved in Init()
 * method. the following handler have to be released:
 *
 * ccbHandle
 * ownerHandle
 * immHandle
 *
 */
ACS_CC_ReturnType acs_apgcc_omhandler_impl::Finalize() {

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::Finalize( )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	if((errorCode=saImmOmAccessorFinalize(accessorHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::Finalize():saImmOmAccessorFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/* release innHandler*/
	if((errorCode=saImmOmFinalize(immHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::Finalize():saImmOmFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

}



/** acs_apgcc_omhandler_impl::defineClass method. This method define a new class in IMM
 * according to parameter provided as input. Input parameter are:
 *
 * char* className : the name of the class to be created
 *
 * ACS_CC_ClassCategoryType classCategory: the class category. It may asuume the following values
 * 					CONFIGURATION if the class to be defined is related to IMM Configuration Objects
 * 					RUNTIME       if the class to be defined is related to IMM Runtime Object
 *vector<ACS_CC_AttrDefinitionType> attrList: a vector containing the definition of all attributes
 *					that are present in the class
 *
 */
ACS_CC_ReturnType acs_apgcc_omhandler_impl::defineClass(char* m_className,ACS_CC_ClassCategoryType m_classCategory,vector<ACS_CC_AttrDefinitionType> m_attrList) {

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::defineClass( char* m_className,ACS_CC_ClassCategoryType m_classCategory,vector<ACS_CC_AttrDefinitionType> m_attrList )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaImmClassCategoryT classCategory;

	SaNameT *StringDefVal = 0;
	vector<SaNameT*> SaNameVector;

	vector<ACS_CC_AttrDefinitionType>::iterator it;

	SaImmAttrDefinitionT_2 **attributesDefs = 0;

    /*Provided input parameter are here mapped on the  SAF specification
	 *here m_classCategory is mapped on the related SAF enum value*/
	switch (m_classCategory){
		case CONFIGURATION:
			classCategory = SA_IMM_CLASS_CONFIG;
			break;

		case RUNTIME:
			classCategory = SA_IMM_CLASS_RUNTIME;
			break;

		default:
			classCategory = SA_IMM_CLASS_CONFIG;
			break;
	}

	/*
	 * here the list of attributes is built. All attributes provided in
	 * m_attrList are copied in a structure  SaImmAttrDefinitionT_2 and
	 * the structure is  added to a list that will be passed as  input
	 * parameter to the saImmOmClassCreate_2 function
	 *
	 * */
	attributesDefs = new SaImmAttrDefinitionT_2*[m_attrList.size()+1];
	int i = 0;
	SaImmAttrDefinitionT_2 oneMoreAttribute[m_attrList.size()];

	for(it = m_attrList.begin(); it != m_attrList.end(); it++  ){

		/*For each element in the vector a new attribute definitionis provided*/

		oneMoreAttribute[i].attrName = (*it).attrName;

		oneMoreAttribute[i].attrValueType=(SaImmValueTypeT)(*it).attrType;

		oneMoreAttribute[i].attrFlags =(SaImmAttrFlagsT)((*it).attrFlag);

		/*modify*/
		if( (oneMoreAttribute[i].attrValueType == SA_IMM_ATTR_SANAMET)&&
				!(oneMoreAttribute[i].attrFlags & SA_IMM_ATTR_RDN)&&
				(*it).attrDefaultVal !=0 ){

			StringDefVal = new SaNameT;

			StringDefVal->length = strlen ((const char*)((*it).attrDefaultVal));
			memcpy(StringDefVal->value, (*it).attrDefaultVal, StringDefVal->length);

			oneMoreAttribute[i].attrDefaultValue = (void*)StringDefVal;

			/**Used to deallocate memory for name variables **/
			SaNameVector.push_back(StringDefVal);
		}else
			oneMoreAttribute[i].attrDefaultValue = (void*)((*it).attrDefaultVal);
		/*end modify*/

		attributesDefs[i] = &(oneMoreAttribute[i]);

		i++;

	}

	/* Last element in the list of Attributes definition is mandatory NULL */
	attributesDefs[i] = 0;

	/* call the function saImmOmClassCreate_2 to create the class*/
	errorCode = saImmOmClassCreate_2(immHandle, m_className,classCategory, (const SaImmAttrDefinitionT_2**)attributesDefs );

	//to free memory
	if(SaNameVector.size()>0){
		vector<SaNameT *>::iterator it_2;
		for(it_2 = SaNameVector.begin(); it_2 != SaNameVector.end(); it_2++){
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}
	delete[]attributesDefs;


	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::defineClass():saImmOmClassCreate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/*immSave save the imm configuration*/
	if(immSave() != ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILURE\n");
		log.Write("OmHandler::defineClass():: immSave():FAILURE", LOG_LEVEL_ERROR);
	}

	return ACS_CC_SUCCESS;

}



//---------------------------------------------------------------------------------------------------------------------------
// REMOVE CLASS METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::removeClass(char* m_className) {

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::removeClass( char* m_className )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	errorCode = saImmOmClassDelete(immHandle, (SaImmClassNameT) m_className);
	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::removeClass():saImmOmClassDelete:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/*immSave save the imm configuration*/
	if(immSave()!=ACS_CC_SUCCESS){
		//ACE_OS::fprintf(stdout, "OmHandler::removeClass(): immSave():FAILURE\n");
		log.Write("OmHandler::removeClass():immSave():FAILURE", LOG_LEVEL_ERROR);
	}

	return ACS_CC_SUCCESS;

}



//---------------------------------------------------------------------------------------------------------------------------
// CREATE OBJECT METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::createObject (const char* m_className, const char* m_parentName, vector<ACS_CC_ValuesDefinitionType> m_attrValuesList){


	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::createObject (const char* m_className, const char* m_parentName, vector<ACS_CC_ValuesDefinitionType> m_attrValuesList )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;
	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;/*added for tr HR91436*/

	SaNameT* name = 0;
	vector<SaNameT*> SaNameVector;

	/* The parent Name of the creating OBJECT*/
	SaNameT parentName = {0,{0}};

	/*An iterator to access the attributes list*/
	vector<ACS_CC_ValuesDefinitionType>::iterator it;

	/*The attribute list to be passed to the function saImmOmCcbObjectCreate_2*/
	SaImmAttrValuesT_2 **attributesValues = 0;


	/**Initialization Section Start
	 *
	 * initialize ownerHandle and ccbHandle
	 *
	 */

	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;


	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *immHandle is the handle retrieved by the previous call second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler is stored
	 */
	errorCode = saImmOmAdminOwnerInitialize(immHandle, const_cast<char*>(processName.c_str()), SA_TRUE, &ownerHandle);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::createObject():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	const SaNameT *objOwnedNameList[] = {&parentName, 0};

	/*Build the SaNameT variable to be passed to the  saImmOmCcbObjectCreate_2 function*/
	if(m_parentName != 0){
		/*Set in parentName.length the m_parentName string length*/
		parentName.length = strlen(m_parentName);

		/*Copy in parentName.value the m_parentName value*/
		memcpy(parentName.value, m_parentName, parentName.length);

		errorCode = saImmOmAdminOwnerSet(ownerHandle,objOwnedNameList, SA_IMM_ONE );
		if (errorCode != SA_AIS_OK)	{

			setInternalError(errorCode);

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("OmHandler::createObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}
			/*end release ownerHandle*/

			return ACS_CC_FAILURE;
		}

	}


	/*Retrieve the CCB handler to perform operation IMM*/
	errorCode = saImmOmCcbInitialize(ownerHandle, ccbFlagVal ,&ccbHandle);
	if (errorCode != SA_AIS_OK)	{
			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmCcbInitialize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("OmHandler::createObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}
			/*end release ownerHandle*/

			return ACS_CC_FAILURE;
		}

	/*Initialization Section End*/


	/* Create the list of Attribute Values to be provided as input to the saImmOmCcbObjectCreate_2 function*/
	/* allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
	attributesValues = new SaImmAttrValuesT_2*[m_attrValuesList.size()+1];

	int numAttribute = m_attrValuesList.size();

	SaImmAttrValuesT_2 singleAttributeValues[numAttribute];

	int i =0;

	for(it = m_attrValuesList.begin(); it != m_attrValuesList.end(); it++  ){

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


	errorCode = saImmOmCcbObjectCreate_2 ( ccbHandle, (SaImmClassNameT)m_className, &parentName, (const SaImmAttrValuesT_2**)(attributesValues) );

	/* delete allocated memory*/
	i = 0;
	for(it = m_attrValuesList.begin(); it != m_attrValuesList.end(); it++  ){

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

		log.Write("OmHandler::createObject():saImmOmCcbObjectCreate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//return ACS_CC_FAILURE;
		}
		/*End Added for CCB corruption management*/

		return ACS_CC_FAILURE;
	}

	if ((errorCode = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::createObject():saImmOmCcbApply:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//return ACS_CC_FAILURE;
		}
		/*End Added for CCB corruption management*/

		return ACS_CC_FAILURE;
	}


	/*Finalize handle Section Start*/

	/** release ccbHandler**/
	if ( (errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK ){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::createObject():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		//return ACS_CC_FAILURE;
		retVal = ACS_CC_FAILURE;

	}
	/* release ownerHandle*/
	if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::createObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}
	/*Finalize handle Section End*/


	/*ADDED immSave save the imm configuration*/
	if(immSave()!=ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILLURE\n");
		log.Write("OmHandler::createObject():immSave:FAILED", LOG_LEVEL_ERROR);
	}

	//return ACS_CC_SUCCESS;
	return retVal;
}
/*End acs_apgcc_omhandler_impl::createObject */


///**Set the admistrativeOwner on the parent of object**/
//SaNameT objOwnedName = {0,{0}};
//const SaNameT *objOwnedNameList[] = {&objOwnedName, 0};
//---------------------------------------------------------------------------------------------------------------------------
// CREATE OBJECT METHOD TRANSCATIONAL
//Added For Object creation without apply the request to CCB
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::createObject (const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList, std::string p_transactionName ){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::createObject (const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList, std::string p_transactionName )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT error;

	/**Check the name transaction**/
	if (p_transactionName.length() == 0){

		setInternalError(7);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 7);

		log.Write("OmHandler::createObject() : transactionName invalid : FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	/**Get the CCB handle of transaction**/
	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if( returnCode != 0 ){

		setInternalError(returnCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", returnCode);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::createObject():getCcbHandle():FAILED - Transaction %s: during creation of object with parent %s", p_transactionName.c_str(), p_parentName);

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::createObject():getCcbHandle():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

//	ACE_OS::fprintf(stdout,"CCBHandle in createObject %d\n", newCcbHandle);


	SaNameT* name = 0;
	vector<SaNameT*> SaNameVector;

	/* The parent Name of the creating OBJECT*/
	SaNameT parentName = {0,{0}};

	/*An iterator to access the attributes list*/
	vector<ACS_CC_ValuesDefinitionType>::iterator it;

	/*The attribute list to be passed to the function saImmOmCcbObjectCreate_2*/
	SaImmAttrValuesT_2 **attributesValues = 0;



	//cout<<"OWNER NAME BEFORE: "<<objOwnedName.value<<endl;

	/*Set the administrative Owner on the parent of object to create*/
	if(p_parentName != 0){


		/*Set in parentName.length the m_parentName string length*/
		parentName.length = strlen(p_parentName);

		/*Copy in parentName.value the m_parentName value*/
		memcpy(parentName.value, p_parentName, parentName.length);

		objOwnedName.length = sprintf((char*)objOwnedName.value, "%s", p_parentName);
		//cout<<"OWNER NAME: "<<objOwnedName.value<<endl;
		//cout<<"OWNER HANDLE: "<<newownerHandle<<endl;
		//cout<<"CCB HANDLE: "<<newCcbHandle<<endl;

		/*Set the administrative owner on the parent of object*/
		errorCode = saImmOmAdminOwnerSet (newownerHandle, objOwnedNameList, SA_IMM_ONE );
		if (errorCode != SA_AIS_OK)	{

			//ACE_OS::fprintf(stdout, "saImmOmAdminOwnerSet in createobject FAILED, ERROR CODE: %d \n",errorCode );
			setInternalError(errorCode);

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			char bufferMessage[1024] = {0};
			sprintf(bufferMessage, "OmHandler::createObject():saImmOmAdminOwnerSet:FAILED - Transaction %s during creation of object with parent %s", p_transactionName.c_str(), p_parentName);
			//log.Write("OmHandler::createObject():saImmOmAdminOwnerSet:FAILED during creation of object", LOG_LEVEL_ERROR);
			log.Write(bufferMessage, LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}
	}

	/* Create the list of Attribute Values to be provided as input to the saImmOmCcbObjectCreate_2 function*/
	/*allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
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


	error = saImmOmCcbObjectCreate_2 ( newCcbHandle, (SaImmClassNameT)p_className, &parentName, (const SaImmAttrValuesT_2**)(attributesValues) );

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

	/*Report error in APGCC.log if the saImmOmCcbObjectCreate_2 failed*/
	if (error != SA_AIS_OK ){

		setInternalError(error);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::createObject():saImmOmCcbObjectCreate_2:FAILED - Transaction %s: Error during creation of object with parent %s", p_transactionName.c_str(), p_parentName);

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandlerV2::createObject():saImmOmCcbObjectCreate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/*ADDED immSave save the imm configuration*/
	if(immSave()!=ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILLURE\n");
		log.Write("OmHandler::createObject():immSave:FAILED", LOG_LEVEL_ERROR);
	}

	return ACS_CC_SUCCESS;
}/*End acs_apgcc_omhandler_impl::createObject */




//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::deleteObject (const char* m_objectName){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::deleteObject (const char* m_objectName)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;
	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;

	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(m_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, m_objectName, objToDelete.length);


	/*Initialization Section Start*/

	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;

	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *immHandle is the handle retrieved by the previous call second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler is stored
	 */
	errorCode = saImmOmAdminOwnerInitialize(immHandle, const_cast<char*>(processName.c_str()), SA_TRUE, &ownerHandle);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	const SaNameT *objOwnedNameList[] = {&objToDelete, 0};

	errorCode = saImmOmAdminOwnerSet (ownerHandle,objOwnedNameList, SA_IMM_ONE );
	if (errorCode != SA_AIS_OK)	{

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);
		}
		/*end release ownerHandle*/

		return ACS_CC_FAILURE;
	}

	/*Retrieve the CCB handler to perform operation IMM*/
	errorCode = saImmOmCcbInitialize(ownerHandle, ccbFlagVal ,&ccbHandle);
	if (errorCode != SA_AIS_OK)	{
			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmCcbInitialize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("OmHandler::createObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}
			/*end release ownerHandle*/

			return ACS_CC_FAILURE;
		}

	/*Initialization Section End*/

	/*Call the saImmOmCcbObjectDelete */
	if((errorCode = saImmOmCcbObjectDelete(ccbHandle, &objToDelete ))!= SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject ():saImmOmCcbObjectDelete:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject ():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/*End Added for CCB corruption management*/

		return ACS_CC_FAILURE;

	}

	if ((errorCode = saImmOmCcbApply(ccbHandle))!= SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmCcbApply:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject ():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}
		/*End Added for CCB corruption management*/

		return ACS_CC_FAILURE;
	}

	/*Finalize handle Section Start*/

	/** release ccbHandler**/
	if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		//HR91436 return ACS_CC_FAILURE;
		retVal = ACS_CC_FAILURE;
	}
	/* release ownerHandle*/
	if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/*Finalize handle Section End*/

	/*immSave save the imm configuration*/
	if(immSave()!=ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILURE\n");
		log.Write("OmHandler::deleteObject():immSave():FAILURE", LOG_LEVEL_ERROR);
	}


	//HR91436 return ACS_CC_SUCCESS;
	return retVal;
}
/*End acs_apgcc_omhandler_impl::deleteObject */


//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD WITH SCOPE PARAMETER
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::deleteObject (const char* m_objectName, ACS_APGCC_ScopeT p_scope){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::deleteObject (const char* m_objectName, ACS_APGCC_ScopeT p_scope)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;
	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;

	SaNameT objToDelete;



	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(m_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, m_objectName, objToDelete.length);


	/*Initialization Section Start*/

	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;

	/*added for scope management*/
	SaImmScopeT scopeVal = SA_IMM_ONE;

	if (p_scope == ACS_APGCC_ONE)
		scopeVal = SA_IMM_ONE;
	else if (p_scope == ACS_APGCC_SUBLEVEL)
		scopeVal = SA_IMM_SUBLEVEL;
	else if (p_scope == ACS_APGCC_SUBTREE)
		scopeVal = SA_IMM_SUBTREE;
	/*end added for scope management*/

	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *immHandle is the handle retrieved by the previous call second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler is stored
	 */
	errorCode = saImmOmAdminOwnerInitialize(immHandle, const_cast<char*>(processName.c_str()), SA_TRUE, &ownerHandle);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	const SaNameT *objOwnedNameList[] = {&objToDelete, 0};

	errorCode = saImmOmAdminOwnerSet (ownerHandle,objOwnedNameList, scopeVal );
	if (errorCode != SA_AIS_OK)	{

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);
		}
		/*end release ownerHandle*/

		return ACS_CC_FAILURE;
	}

	/*Retrieve the CCB handler to perform operation IMM*/
	errorCode = saImmOmCcbInitialize(ownerHandle, ccbFlagVal ,&ccbHandle);
	if (errorCode != SA_AIS_OK)	{
			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::createObject():saImmOmCcbInitialize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("OmHandler::createObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}
			/*end release ownerHandle*/

			return ACS_CC_FAILURE;
		}

	/*Initialization Section End*/

	/*Call the saImmOmCcbObjectDelete */
	if((errorCode = saImmOmCcbObjectDelete(ccbHandle, &objToDelete ))!= SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject ():saImmOmCcbObjectDelete:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject ():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/*End Added for CCB corruption management*/

		return ACS_CC_FAILURE;

	}

	if ((errorCode = saImmOmCcbApply(ccbHandle))!= SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmCcbApply:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject ():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}
		/*End Added for CCB corruption management*/

		return ACS_CC_FAILURE;
	}

	/*Finalize handle Section Start*/

	/** release ccbHandler**/
	if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		//HR91436 return ACS_CC_FAILURE;
		retVal = ACS_CC_FAILURE;
	}
	/* release ownerHandle*/
	if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::deleteObject():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/*Finalize handle Section End*/

	/*immSave save the imm configuration*/
	if(immSave()!=ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILURE\n");
		log.Write("OmHandler::deleteObject():immSave():FAILURE", LOG_LEVEL_ERROR);
	}


	//HR91436 return ACS_CC_SUCCESS;
	return retVal;
}
/*End acs_apgcc_omhandler_impl::deleteObject with scope*/




//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD TRANSACTIONAL
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::deleteObject (const char* p_objectName, std::string p_transactionName){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::deleteObject (const char* m_objectName, std::string p_transactionName)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT error;

	if (p_transactionName.length() == 0){

		setInternalError(7);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 7);

		log.Write("OmHandler::deleteObject() : transactionName invalid : FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if( returnCode != 0 ){

		setInternalError(returnCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", returnCode);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::deleteObject():getCcbHandle():FAILED - Transaction %s", p_transactionName.c_str());

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::deleteObject():getCcbHandle():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(p_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, p_objectName, objToDelete.length);

	objOwnedName.length = sprintf((char*)objOwnedName.value, "%s", p_objectName);
	//cout<<"OWNER NAME: "<<objOwnedName.value<<endl;
	//cout<<"OWNER HANDLE: "<<newownerHandle<<endl;

	/*Set the administrative owner on the object to delete*/
	errorCode = saImmOmAdminOwnerSet (newownerHandle,objOwnedNameList, SA_IMM_ONE );
	if (errorCode != SA_AIS_OK)	{

		//ACE_OS::fprintf(stdout, "saImmOmAdminOwnerSet in deleteObject FAILED, ERROR CODE: %d \n",errorCode );
		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::deleteObject():saImmOmAdminOwnerSet:FAILED - Transaction %s during deletion of object %s", p_transactionName.c_str(), p_objectName);
		//log.Write("OmHandler::createObject():saImmOmAdminOwnerSet:FAILED during creation of object", LOG_LEVEL_ERROR);
		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/*Call the saImmOmCcbObjectDelete */
	if((error = saImmOmCcbObjectDelete(newCcbHandle, &objToDelete ))!= SA_AIS_OK){

		setInternalError(error);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::deleteObject():saImmOmAdminOwnerSet:FAILED - Transaction %s during deletion of object %s", p_transactionName.c_str(), p_objectName);

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::deleteObject ():saImmOmCcbObjectDelete:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	/*immSave save the imm configuration*/
	if(immSave()!=ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILURE\n");
		log.Write("OmHandler::deleteObject():immSave():FAILURE", LOG_LEVEL_ERROR);
	}


	return ACS_CC_SUCCESS;
}
/*End acs_apgcc_omhandler_impl::deleteObject */

//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD TRANSACTIONAL WITH SCOPE MANAGEMENT
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::deleteObject (const char* p_objectName, std::string p_transactionName, ACS_APGCC_ScopeT p_scope){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::deleteObject (const char* m_objectName, std::string p_transactionName, ACS_APGCC_ScopeT p_scope)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT error;

	if (p_transactionName.length() == 0){

		setInternalError(7);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 7);

		log.Write("OmHandler::deleteObject() : transactionName invalid : FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if( returnCode != 0 ){

		setInternalError(returnCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", returnCode);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::deleteObject():getCcbHandle():FAILED - Transaction %s", p_transactionName.c_str());

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::deleteObject():getCcbHandle():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/*added for scope management*/
	SaImmScopeT scopeVal = SA_IMM_ONE;

	if (p_scope == ACS_APGCC_ONE)
		scopeVal = SA_IMM_ONE;
	else if (p_scope == ACS_APGCC_SUBLEVEL)
		scopeVal = SA_IMM_SUBLEVEL;
	else if (p_scope == ACS_APGCC_SUBTREE)
		scopeVal = SA_IMM_SUBTREE;
	/*end added for scope management*/

	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(p_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, p_objectName, objToDelete.length);

	objOwnedName.length = sprintf((char*)objOwnedName.value, "%s", p_objectName);
	//cout<<"OWNER NAME: "<<objOwnedName.value<<endl;
	//cout<<"OWNER HANDLE: "<<newownerHandle<<endl;

	/*Set the administrative owner on the object to delete*/
	errorCode = saImmOmAdminOwnerSet (newownerHandle,objOwnedNameList, scopeVal );
	if (errorCode != SA_AIS_OK)	{

		//ACE_OS::fprintf(stdout, "saImmOmAdminOwnerSet in deleteObject FAILED, ERROR CODE: %d \n",errorCode );
		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::deleteObject():saImmOmAdminOwnerSet:FAILED - Transaction %s during deletion of object %s", p_transactionName.c_str(), p_objectName);
		//log.Write("OmHandler::createObject():saImmOmAdminOwnerSet:FAILED during creation of object", LOG_LEVEL_ERROR);
		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/*Call the saImmOmCcbObjectDelete */
	if((error = saImmOmCcbObjectDelete(newCcbHandle, &objToDelete ))!= SA_AIS_OK){

		setInternalError(error);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::deleteObject():saImmOmAdminOwnerSet:FAILED - Transaction %s during deletion of object %s", p_transactionName.c_str(), p_objectName);

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::deleteObject ():saImmOmCcbObjectDelete:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}

	/*immSave save the imm configuration*/
	if(immSave()!=ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILURE\n");
		log.Write("OmHandler::deleteObject():immSave():FAILURE", LOG_LEVEL_ERROR);
	}


	return ACS_CC_SUCCESS;
}
/*End acs_apgcc_omhandler_impl::deleteObject transactional with scope management */




//---------------------------------------------------------------------------------------------------------------------------
// GET ATTRIBUTE METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getAttribute (const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue ){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::getAttribute (const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	/*1 objectName. The Name of the object whose attribute has to be retrieved*/
	SaNameT objectName;

	/*2 attToBeRetrievedList. The attribute to be retrieved List must be a null terminated array of attribute names, according
	 * to saImmOmAccessorGet_2 function */
	SaImmAttrNameT attToBeRetrievedList[2];
	/*attToBeRetrieved is the data where is set attribute name for which values must be returned*/


	/*3 valuesRetrieved. The list of output values for saImmOmAccessorGet_2 */
	SaImmAttrValuesT_2** valuesRetrieved;


	/*Prepares the OBJECT NAME to be passed to saImmOmAccessorGet_2()*/
	objectName.length = strlen(m_objectName);
	memcpy(objectName.value, m_objectName, objectName.length);

	/*Prepares the ATTRIBUTES TO BE RETRIEVED LIST it must be a NULL terminated array*/
	attToBeRetrievedList[0] = m_attributeValue->attrName;
	attToBeRetrievedList[1] = 0;

	/*Prepares to Call the saImmOmAccessorGet  to perform get on the m_objectName for the attribute whose name
	 * is set in m_attributeValue->attrName field*/
	if((errorCode = saImmOmAccessorGet_2(accessorHandle, &objectName, attToBeRetrievedList, &valuesRetrieved ))!=SA_AIS_OK) {
	//	ACE_OS::fprintf(stdout, "OmHandler::getAttribute():saImmOmAccessorGet_2:FAILLURE\n");
	//	ACE_OS::fprintf(stdout, "ERROR CODE %u \n", errorCode);

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::getAttribute():saImmOmAccessorGet_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}/*end if on saImmOmAccessorGet_2*/


	/*Prepares data to be returned to calling*/
	/*Set the attribute matching the condition found Type*/
	int i = 0;
	m_attributeValue->attrType = (ACS_CC_AttrValueType)valuesRetrieved[0]->attrValueType;
	m_attributeValue->attrValuesNum = valuesRetrieved[i]->attrValuesNumber;

	// TR HO83231
	if ( m_attributeValue->flag )
		delete [] m_attributeValue->attrValues;

	m_attributeValue->attrValues = new void*[valuesRetrieved[i]->attrValuesNumber];
	m_attributeValue->ACS_APGCC_IMMFreeMemory(1);

	/*only one attribute is returned according to the search criteria otherwise(more att searched) a while
	 * is needed
	 * while (valuesRetrieved[i]){*/
	for (unsigned int j=0; j<valuesRetrieved[i]->attrValuesNumber; j++){

		switch (valuesRetrieved[i]->attrValueType) {

			case SA_IMM_ATTR_SANAMET:{
				SaNameT* localSaName;
				localSaName = reinterpret_cast<SaNameT*>(valuesRetrieved[i]->attrValues[j]);
				m_attributeValue->attrValues[j]=localSaName->value;
				break;
			}/*end case SA_IMM_ATTR_SANAMET*/

			case SA_IMM_ATTR_SASTRINGT:{
				SaStringT stringVal = *(reinterpret_cast<SaStringT*>(valuesRetrieved[i]->attrValues[j]));
				m_attributeValue->attrValues[j] = reinterpret_cast<char*>(stringVal);
				break;
			}

			default:
				m_attributeValue->attrValues[j]=valuesRetrieved[i]->attrValues[j];
			break;
		}/*end switch */

	}
	i++;

	//only one attribute is returned end}
	return ACS_CC_SUCCESS;

}/*End  acs_apgcc_omhandler_impl::getAttribute*/




//---------------------------------------------------------------------------------------------------------------------------
// GET MULTI - ATTRIBUTE METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getAttribute (const char* p_objectName, std::vector<ACS_APGCC_ImmAttribute *> p_attributeList ){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::getAttribute (const char* p_objectName, std::vector<ACS_APGCC_ImmAttribute *> p_attributeList )");
		trace.ACS_TRA_event(1, buffer);
	}

//	printf("SONO IN GET ATTRIBUTE\n");
	SaAisErrorT errorCode;

	int numAttrToRetrive = p_attributeList.size();

	if ( numAttrToRetrive == 0 ){

		setInternalError(45);
		log.Write("OmHandler::getAttribute() - not attribute to retrieve FAILURE", LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

//	printf("NUMERO DI ATTRIBUTI DA CERCARE %d\n", numAttrToRetrive);

	/*2 attToBeRetrievedList. The attribute to be retrieved List must be a null terminated array of attribute names, according
	 * to saImmOmAccessorGet_2 function */
	SaImmAttrNameT attToBeRetrievedList[ (numAttrToRetrive + 1) ];
	/*attToBeRetrieved is the data where is set attribute name for which values must be returned*/


	/*3 valuesRetrieved. The list of output values for saImmOmAccessorGet_2 */
	SaImmAttrValuesT_2** valuesRetrieved;

	/*The Name of the object whose attribute has to be retrieved*/
	SaNameT objectName;

	objectName.length = strlen(p_objectName);
	memcpy(objectName.value, p_objectName, objectName.length);

	/*Prepares the ATTRIBUTES TO BE RETRIEVED LIST it must be a NULL terminated array*/
	for (int i = 0; i < numAttrToRetrive; i++){

		attToBeRetrievedList[i] = const_cast<char*>(p_attributeList[i]->attrName.c_str());
	//	printf("%d. %s\n", i, attToBeRetrievedList[i]);

	}

	attToBeRetrievedList[numAttrToRetrive] = 0;


	/*Prepares to Call the saImmOmAccessorGet  to perform get on the m_objectName for the attribute whose name
	 * is set in m_attributeValue->attrName field*/
	if((errorCode = saImmOmAccessorGet_2(accessorHandle, &objectName, attToBeRetrievedList, &valuesRetrieved ))!=SA_AIS_OK) {
//		ACE_OS::fprintf(stdout, "OmHandler::getAttribute_2():saImmOmAccessorGet_2:FAILLURE\n");
//		ACE_OS::fprintf(stdout, "ERROR CODE %u \n", errorCode);

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::getAttribute():saImmOmAccessorGet_2:FAILURE", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}/*end if on saImmOmAccessorGet_2*/

	/*Prepares data to be returned to calling*/
	/*Set the attribute matching the condition found Type*/
	int i = 0;
	while( *valuesRetrieved != 0 ){

		for(unsigned k=0; k<p_attributeList.size(); k++ ){
			if( strcmp(p_attributeList[k]->attrName.c_str(), (*valuesRetrieved)->attrName) == 0){

				p_attributeList[k]->attrType = (ACS_CC_AttrValueType)(*valuesRetrieved)->attrValueType;
				p_attributeList[k]->attrValuesNum = (*valuesRetrieved)->attrValuesNumber;

		//		printf("attrName %s\n", (*valuesRetrieved)->attrName);
		//		printf("attrType %d\n", (*valuesRetrieved)->attrValueType);
		//		printf("attrValueNumber %d\n", p_attributeList[k]->attrValuesNum);

				for (unsigned int j = 0; j < (*valuesRetrieved)->attrValuesNumber; j++){

					switch ((*valuesRetrieved)->attrValueType) {

						case SA_IMM_ATTR_SANAMET:{
		//					printf("SONO IN SANAME\n");
							SaNameT* localSaName;
							localSaName = reinterpret_cast<SaNameT*>((*valuesRetrieved)->attrValues[j]);

							StringType *val = new (std::nothrow) StringType;
							if ( val != 0 ){
								val->value = reinterpret_cast<char*>(localSaName->value);
								p_attributeList[k]->attrValues[j] = const_cast<char *>((val->value).c_str());

								p_attributeList[k]->pointers.push_back(val);
							}else {
						//		ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
						//		ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);

								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

								return ACS_CC_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SASTRINGT:{
		//					printf("SONO IN STRING\n");
							SaStringT stringVal = *(reinterpret_cast<SaStringT*>((*valuesRetrieved)->attrValues[j]));

							StringType *val = new (std::nothrow) StringType;
							if ( val != 0 ) {
								if (stringVal){
									val->value = stringVal;
								}else {
									log.Write("OmHandler::getObject() -  stringVal not available FAILURE", LOG_LEVEL_ERROR);

									/*set as error code invalid parameter since the attributesValueNum is not 0 but the string is null*/
									setInternalError(7);

									return ACS_CC_FAILURE;

								}

								//val->value = stringVal;
								p_attributeList[k]->attrValues[j] = const_cast<char *>((val->value).c_str());

								p_attributeList[k]->pointers.push_back(val);
							}else {

						//		ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
						//		ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);

								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

								return ACS_CC_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAUINT32T: {
		//					printf("SONO IN UNSINT32\n");
							UnsIntType *val = new (std::nothrow) UnsIntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<unsigned int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {

						//		ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
						//		ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);

								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

								return ACS_CC_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAINT32T: {
		//					printf("SONO IN INT32\n");
							IntType *val = new (std::nothrow) IntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {

						//		ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
						//		ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);

								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

								return ACS_CC_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAUINT64T: {
		//					printf("SONO IN INSINT64\n");
							UnsLongLongIntType *val = new (std::nothrow) UnsLongLongIntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<unsigned long long int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {

						//		ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
						//		ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);

								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

								return ACS_CC_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAINT64T: {
		//					printf("SONO IN INT64\n");
							LongLongIntType *val = new (std::nothrow) LongLongIntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<long long int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {

					//			ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
					//			ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);

								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

								return ACS_CC_FAILURE;
							}
							break;
						}

						case SA_IMM_ATTR_SADOUBLET: {
		//					printf("SONO IN DOUBLE\n");
							DoubleType *val = new (std::nothrow) DoubleType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<double*>((*valuesRetrieved)->attrValues[j]));
		//							printf("attribute value %d \n", (val->value));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {

					//			ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
					//			ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);

								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

								return ACS_CC_FAILURE;
							}
							break;
						}

						case SA_IMM_ATTR_SAFLOATT: {
		//					printf("SONO IN FLOAT\n");
							FloatType *val = new (std::nothrow) FloatType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<float *>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {

						//		ACE_OS::fprintf(stdout, "OmHandler::getObject() FAILLURE\n");
						//		ACE_OS::fprintf(stdout, "Cannot allocate memory %u \n", -1);
								log.Write("OmHandler::getAttribute() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);


								return ACS_CC_FAILURE;
							}
							break;
						}

						default:
		//					printf("SONO IN DEFAULT\n");
							p_attributeList[k]->attrValues[j] = (*valuesRetrieved)->attrValues[j];
						break;
					}/*end switch */

				}/*end for attrValuesNumber */

			}/*end if strcmp(...)*/

		}/*end for p_attributeList.size()*/


		i++;
	//	printf("ITERAZIONE NUMERO: %d\n", i);
		*valuesRetrieved++;
	}
//	printf("NUMERO ITERAZIONI %d\n", i);

	return ACS_CC_SUCCESS;
}



//---------------------------------------------------------------------------------------------------------------------------
// GET OBJECT METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getObject(ACS_APGCC_ImmObject *object){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::getObject(ACS_APGCC_ImmObject *object)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	//get the DN of object to retrieve
	SaNameT objectName = {0,{0}};
	//objectName.length = strlen(const_cast<char*>((object->objName).c_str()));

	string temp_objName = object->objName;
	objectName.length = temp_objName.size();

	ACE_OS::memcpy(objectName.value, const_cast<char*>((temp_objName).c_str()), objectName.length);

	SaImmAttrValuesT_2** valuesRetrieved;

	/* call the saImmOmAccessorGet to perform get all attribute of object */
	errorCode = saImmOmAccessorGet_2(accessorHandle, &objectName, 0, &valuesRetrieved );

	if( errorCode != SA_AIS_OK ) {

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::getObject():saImmOmAccessorGet_2:FAILURE", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/* Prepares data to be returned to calling*/
	ACS_APGCC_ImmAttribute attributeList;

	/* Use to save temporally the attribute name */
	std::string tmp = "";

	while ( *valuesRetrieved != 0 ){
	//	printf("attribute name: %s\n", (*valuesRetrieved)->attrName);

		tmp = reinterpret_cast<char *>((*valuesRetrieved)->attrName);
		attributeList.attrName = tmp;
		attributeList.attrType = (int)(*valuesRetrieved)->attrValueType;
		attributeList.attrValuesNum = (*valuesRetrieved)->attrValuesNumber;

		for (unsigned int j = 0; j < (*valuesRetrieved)->attrValuesNumber; j++){

			/*Set the attribute matching the condition found Type*/
			switch ((*valuesRetrieved)->attrValueType) {

				case SA_IMM_ATTR_SANAMET:{
					SaNameT* localSaName;
					localSaName = reinterpret_cast<SaNameT*>((*valuesRetrieved)->attrValues[j]);

					StringType *val = new (std::nothrow) StringType;
					if ( val != 0 ){
						val->value = reinterpret_cast<char*>(localSaName->value);
						attributeList.attrValues[j] = const_cast<char *>((val->value).c_str());

						object->pointers.push_back(val);
					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SASTRINGT:{
					SaStringT stringVal = *(reinterpret_cast<SaStringT*>((*valuesRetrieved)->attrValues[j]));

					StringType *val = new (std::nothrow) StringType;
					if ( val != 0 ){
//						val->value = stringVal;
//						attributeList.attrValues[j] = const_cast<char *>((val->value).c_str());
//						object->pointers.push_back(val);
						//TR HP35181
						if (stringVal){
							val->value = stringVal;
						}else {
							log.Write("OmHandler::getObject() -  stringVal not available FAILURE", LOG_LEVEL_ERROR);

							/*set as error code invalid parameter since the attributesValueNum is not 0 but the string is null*/
							setInternalError(7);

							return ACS_CC_FAILURE;

						}
						attributeList.attrValues[j] = const_cast<char *>((val->value).c_str());
						object->pointers.push_back(val);

					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SAUINT32T: {

					UnsIntType *val = new (std::nothrow) UnsIntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<unsigned int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SAINT32T: {

					IntType *val = new (std::nothrow) IntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SAUINT64T: {

					UnsLongLongIntType *val = new (std::nothrow) UnsLongLongIntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<unsigned long long int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SAINT64T: {

					LongLongIntType *val = new (std::nothrow) LongLongIntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<long long int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}
					break;
				}

				case SA_IMM_ATTR_SADOUBLET: {

					DoubleType *val = new (std::nothrow) DoubleType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<double*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}
					break;
				}

				case SA_IMM_ATTR_SAFLOATT: {
					FloatType *val = new (std::nothrow) FloatType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<float *>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {

						log.Write("OmHandler::getObject() - Cannot allocate memory FAILURE", LOG_LEVEL_ERROR);

						return ACS_CC_FAILURE;
					}
					break;
				}

				default:
					attributeList.attrValues[j] = (*valuesRetrieved)->attrValues[j];
				break;
			}/*end switch */

		}

		object->attributes.push_back(attributeList);
		*valuesRetrieved++;
	}

	return ACS_CC_SUCCESS;

}



//---------------------------------------------------------------------------------------------------------------------------
// GET CHILDREN METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getChildren(const char* p_rootName, ACS_APGCC_ScopeT p_scope, std::vector<std::string>* p_rdnList) {

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::getChildren(const char* p_rootName, ACS_APGCC_ScopeT p_scope, std::vector<std::string>* p_rdnList)");
		trace.ACS_TRA_event(1, buffer);
	}

	if (p_rootName == 0){

		setInternalError(7);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::getChildren() FAILURE", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	SaAisErrorT errorCode;

	SaNameT rootName;
	rootName.length = strlen(p_rootName);
	ACE_OS::memcpy(rootName.value, p_rootName, rootName.length);

	SaImmScopeT scope;

	if(p_scope == ACS_APGCC_ONE){
		scope = SA_IMM_ONE;
	}else if( p_scope ==ACS_APGCC_SUBLEVEL){
		scope = SA_IMM_SUBLEVEL;
	}else if (p_scope ==ACS_APGCC_SUBTREE ){
		scope = SA_IMM_SUBTREE;
	}else {
		scope = SA_IMM_SUBLEVEL;
	}


	SaImmSearchOptionsT searchOption;
	searchOption = SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR;

	SaImmSearchParametersT_2 searchParam;
	searchParam.searchOneAttr.attrName = 0;
	searchParam.searchOneAttr.attrValue = 0;

	SaImmSearchHandleT searchHandle;

	errorCode = saImmOmSearchInitialize_2(immHandle, &rootName, scope, searchOption, &searchParam, 0, &searchHandle);
	if (errorCode != SA_AIS_OK ){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::getChildren():saImmOmSearchInitialize_2 FAILURE", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	SaNameT objectName;
	SaImmAttrValuesT_2 **attr;
	std::string rdn = "";

	while(( errorCode = saImmOmSearchNext_2(searchHandle, &objectName, &attr)) == SA_AIS_OK){
		rdn = reinterpret_cast<char *>(objectName.value);

		if ( ( strcmp(p_rootName, rdn.c_str()) != 0 ) ){
			p_rdnList->push_back(rdn);
		}
	}

	errorCode = saImmOmSearchFinalize(searchHandle);
	if(errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::getChildren():saImmOmSearchFinalize FAILURE", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}



//---------------------------------------------------------------------------------------------------------------------------
// GET CLASS INSTANCE METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getClassInstances(const char* p_className, std::vector<std::string> &p_dnList){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::getClassInstances(const char* p_className, std::vector<std::string>* p_dnList)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT error;

	SaNameT rootName = { 0, "" };
	SaNameT objectName;
	SaImmAttrValuesT_2 **attributes;

	const char* className = p_className;

	 /* default search scope */
	SaImmScopeT scope = SA_IMM_SUBTREE;

	if(p_className == 0){

		log.Write("OmHandler::getClassInstances() FAILED : ClassName parameter must be a value\n", LOG_LEVEL_ERROR);

		setInternalError(7);

		return ACS_CC_FAILURE;
	}

	SaImmSearchParametersT_2 searchParam;
	searchParam.searchOneAttr.attrName = const_cast<char *>("SaImmAttrClassName");
	searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
	searchParam.searchOneAttr.attrValue = &className;

	SaImmSearchHandleT searchHandle;

	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, 0, &searchHandle);

	if ( error != SA_AIS_OK ) {

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		log.Write("OmHandler::getClassInstances():saImmOmSearchInitialize_2 FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		setInternalError(error);

		return ACS_CC_FAILURE;
	}

	std::string rdnValue = "";

	while( (error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes)) == SA_AIS_OK){

		if ( error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", error);

			log.Write("OmHandler::getClassInstances():saImmOmSearchNext_2 FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			setInternalError(error);

		    error = saImmOmSearchFinalize(searchHandle);

		    if(error != SA_AIS_OK){

				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", error);

				log.Write("OmHandler::getClassInstances():saImmOmSearchFinalize FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}

	    	return ACS_CC_FAILURE;
	    }

		if ( error != SA_AIS_ERR_NOT_EXIST){
			rdnValue = reinterpret_cast<char*>(objectName.value);
			p_dnList.push_back(rdnValue);
		}
	}

	if(p_dnList.size() == 0){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE 41");

		log.Write("OmHandler::getClassInstances():saImmOmSearchNext_2 FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		setInternalError(41);

	    error = saImmOmSearchFinalize(searchHandle);

	    if(error != SA_AIS_OK){

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", error);

			log.Write("OmHandler::getClassInstances():saImmOmSearchFinalize FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);
		}

    	return ACS_CC_FAILURE;
	}


    error = saImmOmSearchFinalize(searchHandle);
	if(error != SA_AIS_OK){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		log.Write("OmHandler::getClassInstances():saImmOmSearchFinalize FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		setInternalError(error);
		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}




//---------------------------------------------------------------------------------------------------------------------------
// MODIFY ATTRIBUTE METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::modifyAttribute(const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue ) {

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::modifyAttribute(const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;
	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;
	SaNameT *parSaName = 0;
	vector<SaNameT*> parSaNameVector;

	/*objectName. The Name of the object whose attribute has to be modified*/
	SaNameT objectName;

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2* attrMods[2] = {0, 0};

	SaImmAttrModificationT_2 attr;


	/*Prepares the OBJECT NAME to be passed to saImmOmCcbObjectModify_2()*/
	objectName.length = strlen(m_objectName);
	memcpy(objectName.value, m_objectName, strlen(m_objectName));


	/*Initialization Section Start*/
	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;

	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *immHandle is the handle retrieved by the previous call
	 *second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership
	 *				  when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler
	 *			  is stored
	 */
	errorCode = saImmOmAdminOwnerInitialize(immHandle, const_cast<char*>(processName.c_str()), SA_TRUE, &ownerHandle);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::modifyAttribute():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	const SaNameT *objOwnedNameList[] = {&objectName, 0};

	errorCode = saImmOmAdminOwnerSet (ownerHandle,objOwnedNameList, SA_IMM_ONE );
	if (errorCode != SA_AIS_OK)	{

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::modifyAttribute():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::modifyAttribute():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);
		}
		/*end release ownerHandle*/

		return ACS_CC_FAILURE;
	}

	/*Retrieve the CCB handler to perform operation IMM*/
	errorCode = saImmOmCcbInitialize(ownerHandle, ccbFlagVal ,&ccbHandle);
	if (errorCode != SA_AIS_OK)	{
			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::modifyAttribute():saImmOmCcbInitialize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("OmHandler::modifyAttribute():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}
			/*end release ownerHandle*/

			return ACS_CC_FAILURE;
		}

	/*Initialization Section End*/

	/*Prepares data of the attribute to be change */
	/*Set the type of modification to apply on the object attribute*/
	attr.modType=SA_IMM_ATTR_VALUES_REPLACE;

	/*Set the attribute name*/
	attr.modAttr.attrName = (*m_attributeValue).attrName;

	/*Set the attribute Type*/
	attr.modAttr.attrValueType = (SaImmValueTypeT)(*m_attributeValue).attrType;

	/*Set the Attribute Values Number*/
	attr.modAttr.attrValuesNumber = (*m_attributeValue).attrValuesNum;

	/*Set the array where is memorize the new value of attribute*/
	void* attrValues[(*m_attributeValue).attrValuesNum];
	attr.modAttr.attrValues = attrValues;

	/*Set the new values of the attributes*/
	if(attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET){

		for(unsigned int i=0; i<(*m_attributeValue).attrValuesNum; i++){
			parSaName = new SaNameT;
			parSaName->length = strlen(reinterpret_cast<char*>((*m_attributeValue).attrValues[i]));
			memcpy(parSaName->value, ((*m_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}

	}else if(attr.modAttr.attrValueType==SA_IMM_ATTR_SASTRINGT){

		for(unsigned int i=0; i<(*m_attributeValue).attrValuesNum; i++){
			attr.modAttr.attrValues[i] = &((*m_attributeValue).attrValues[i]);
		}

	}else{
		attr.modAttr.attrValues = (*m_attributeValue).attrValues;
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
//		ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute():saImmOmCcbObjectModify_2:FAILLURE\n");
//		ACE_OS::fprintf(stdout, "ERROR CODE %u \n", errorCode);

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::modifyAttribute():saImmOmCcbObjectModify_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::modifyAttribute():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/

		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::modifyAttribute():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}


		/*End Added for CCB corruption management*/

		return ACS_CC_FAILURE;
	}

	/*saImmOmCcbApply applies all requests included in the configuration change bundle identified
	 * by its handle ccbHandle*/
	errorCode = saImmOmCcbApply(ccbHandle);
	if(errorCode!=SA_AIS_OK){
	//	ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute():saImmOmCcbApply:FAILLURE\n");
	//	ACE_OS::fprintf(stdout, "ERROR CODE %u \n", errorCode);

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::modifyAttribute():saImmOmCcbApply:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/
		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::modifyAttribute ():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::modifyAttribute():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			//HR91436 return ACS_CC_FAILURE;
		}


		/*End Added for CCB corruption management*/
		return ACS_CC_FAILURE;
	}

	/*Finalize handle Section Start*/

	/** release ccbHandler**/
	if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::modifyAttribute():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		//HR91436 return ACS_CC_FAILURE;
		retVal = ACS_CC_FAILURE;
	}
	/* release ownerHandle*/
	if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::modifyAttribute():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/*Finalize handle Section End*/

	/*immSave save the imm configuration*/
	if(immSave() != ACS_CC_SUCCESS){
		log.Write("OmHandler::modifyAttribute(): immSave():FAILURE", LOG_LEVEL_ERROR);
	}

	//HR91436 return ACS_CC_SUCCESS;
	return retVal;

}
/*End acs_apgcc_omhandler_impl::modifyAttribute*/



//---------------------------------------------------------------------------------------------------------------------------
// MODIFY ATTRIBUTE METHOD TRANSACTIONAL
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::modifyAttribute(const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue, std::string p_transactionName ) {

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::modifyAttribute(const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue, std::string p_transactionName )");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT error;

	if (p_transactionName.length() == 0){

		setInternalError(7);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 7);

		log.Write("OmHandler::modifyAttribute(): transactionName invalid : FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	/*Get the CCB handle of transaction*/
	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if( returnCode != 0 ){

		setInternalError(returnCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", returnCode);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::modifyAttribute():getCcbHandle():FAILED - Transaction %s: during modify of object %s", p_transactionName.c_str(), p_objectName);

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::modifyAttribute():getCcbHandle():FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	SaNameT *parSaName = 0;
	vector<SaNameT*> parSaNameVector;

	SaNameT objectName;

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2* attrMods[2] = {0, 0};

	SaImmAttrModificationT_2 attr;


	/*Prepares the OBJECT NAME to be passed to saImmOmCcbObjectModify_2()*/
	objectName.length = strlen(p_objectName);
	memcpy(objectName.value, p_objectName, strlen(p_objectName));


	objOwnedName.length = sprintf((char*)objOwnedName.value, "%s", p_objectName);
	//cout<<"OWNER NAME: "<<objOwnedName.value<<endl;
	//cout<<"OWNER HANDLE: "<<newownerHandle<<endl;

	/*Set Administrative Owner on the object to modify */
	errorCode = saImmOmAdminOwnerSet (newownerHandle,objOwnedNameList, SA_IMM_ONE );
	if (errorCode != SA_AIS_OK)	{

		//ACE_OS::fprintf(stdout, "saImmOmAdminOwnerSet in modifyObject FAILED, ERROR CODE: %d\n",errorCode );
		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::modifyAttribute():saImmOmAdminOwnerSet:FAILED - Transaction %s during modify of object %s", p_transactionName.c_str(), p_objectName);
		//log.Write("OmHandler::createObject():saImmOmAdminOwnerSet:FAILED during creation of object", LOG_LEVEL_ERROR);
		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

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
	error = saImmOmCcbObjectModify_2(newCcbHandle, &objectName, (const SaImmAttrModificationT_2**)attrMods );

	/*to free allocate memory*/
	if(parSaNameVector.size() > 0){
		vector<SaNameT *>::iterator it_2;
		for(it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); it_2++){
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}


	if(error != SA_AIS_OK){

		setInternalError(error);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::modifyAttribute():saImmOmCcbObjectModify_2():FAILED - Transaction %s: during modify of object %s", p_transactionName.c_str(), p_objectName);

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::modifyAttribute():saImmOmCcbObjectModify_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);


		return ACS_CC_FAILURE;
	}

	/*immSave save the imm configuration*/
	if(immSave() != ACS_CC_SUCCESS){
		//TRA ACE_OS::fprintf(stdout, "OmHandler::modifyAttribute(): immSave():FAILURE\n");
		log.Write("OmHandler::modifyAttribute(): immSave():FAILURE", LOG_LEVEL_ERROR);
	}

	return ACS_CC_SUCCESS;

}/*End acs_apgcc_omhandler_impl::modifyAttribute*/



/**TO BE DEFINE */
/*int acs_apgcc_omhandler_impl::getLastErrorText(char *p_parentDN, int *p_errorId, char *p_errorText){

	p_parentDN = 0;
	p_errorId = 0;
	p_errorText = 0;

	return 0;
}*/


//---------------------------------------------------------------------------------------------------------------------------
// GET LAST ERROR TEXT METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getLastErrorText(std::string p_parentDN, int& p_errorId, std::string& p_errorText){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "OmHandler::getLastErrorText");
		trace.ACS_TRA_event(1, buffer);
	}
	SaAisErrorT errorCode;
	SaNameT objectName;
	SaImmAttrValuesT_2** valuesRetrieved;

	/*define a new string with the first part of the DN for the object containing the
	 * error*/
	std::string errorObjectDN("errorHandlerId=1,");

	/* create the DN of the object containing the error Info by appending p_parentDN
	 * to errorObjectDN
	 */
	errorObjectDN.append(p_parentDN);

/***********************/
	/*prepare the object name according to IMM api*/
	objectName.length = strlen(const_cast<char*>((errorObjectDN).c_str()));
	ACE_OS::memcpy(objectName.value, const_cast<char*>((errorObjectDN).c_str()), objectName.length);

	/*get the attributes*/
	errorCode = saImmOmAccessorGet_2(accessorHandle, &objectName, 0, &valuesRetrieved );

	if( errorCode != SA_AIS_OK ) {

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("OmHandler::getLastErrorText:FAILURE", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

	/*set the error code in p_errorId*/
	p_errorId = 117;

	/*set the error string in p_errorText*/
	p_errorText.assign("Unable to connect to server");

		return ACS_CC_FAILURE;
	}

	SaUint64T ccbValue = 0;
	SaInt32T  errorIdValue = 0;
	SaStringT errorTextValue = 0;

	std::string attName;
	while (*valuesRetrieved !=0 ){
		attName = reinterpret_cast<char *>((*valuesRetrieved)->attrName);
		printf("OmHandler::getLastErrorText::attName= %s", ((*valuesRetrieved)->attrName));
		 if (attName.compare("ccbId") == 0){
			if ((*valuesRetrieved)->attrValuesNumber > 0)
			ccbValue = *(reinterpret_cast <SaUint64T*>((*valuesRetrieved)->attrValues[0]));
		}else if (attName.compare("errorId") == 0){
			if ((*valuesRetrieved)->attrValuesNumber > 0)
			errorIdValue = *(reinterpret_cast <SaInt32T*>((*valuesRetrieved)->attrValues[0]));

		}else if (attName.compare("errorText") == 0) {
			if ((*valuesRetrieved)->attrValuesNumber > 0)
			errorTextValue = *(reinterpret_cast <SaStringT*>((*valuesRetrieved)->attrValues[0]));

		}

	*valuesRetrieved++;
	}/*end while*/

	if (ccbValue == ccbHandle){
		p_errorId = *(reinterpret_cast <int*> (&errorIdValue));
		p_errorText.assign(reinterpret_cast <char*> (errorTextValue));
		return ACS_CC_SUCCESS;

	}else {
		/*set the error code in p_errorId when ccbid is not the same*/
		p_errorId = 1;

		/*set the error string in p_errorText when ccbid is not the same*/
		p_errorText.assign("Error when Executing");
		return ACS_CC_SUCCESS;

	}

/***********************/

}
/*End acs_apgcc_omhandler_impl::getLastErrorText*/




//---------------------------------------------------------------------------------------------------------------------------
// IMMSAVE METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::immSave(){

//	if (trace.ACS_TRA_ON()){
//		char buffer[1024] = {0};
//		sprintf(buffer, "OmHandler::immSave()");
//		trace.ACS_TRA_event(1, buffer);
//	}


//	if(system("cmw-immSave &>/dev/null") != 0){
//		ACE_OS::fprintf(stdout,"ERROR: Command cmw-immSave failed \n");
//
//		setInternalError(42);
//
//		char buffer[1024] = {0};
//		sprintf(buffer, "ERROR CODE %d", 42);
//
//		log.Write("OmHandler::immSave():FAILED", LOG_LEVEL_ERROR);
//		log.Write(buffer, LOG_LEVEL_ERROR);
//		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
//		if( buffer != 0)
//			log.Write(buffer, LOG_LEVEL_ERROR);
//
//		return ACS_CC_FAILURE;
//	}

	return ACS_CC_SUCCESS;


}/*End acs_apgcc_omhandler_impl::immSave*/




//---------------------------------------------------------------------------------------------------------------------------
// APPLY REQUEST METHOD
// applyRequest applies all requests included in the configuration change bundle identified
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::applyRequest(std::string p_transactionName){

	SaAisErrorT error;

	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newOwnerHandle;

	/*Find in the map the transaction to apply*/
	map<std::string, SaImmCcbHandleT>::iterator it;
	it = transactionList.find(p_transactionName);

	if(it == transactionList.end()){

		setInternalError(42);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 42);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler:applyRequest():FAILED - Transaction: %s not present", p_transactionName.c_str());

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::getCcbHandle():applyRequest:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}else{
		/*CCB handle of transaction to abort*/
		newCcbHandle = transactionList.find(p_transactionName)->second;
		/*Owner Handler of transaction to abort*/
		newOwnerHandle = transactionListOwnerHandle.find(p_transactionName)->second;

	}

	/*Apply CCB of transaction*/
	error = saImmOmCcbApply(newCcbHandle);

	if (error != SA_AIS_OK){

		setInternalError(error);

		/*Ticket 1904: start */
		callSaImmOmCcbGetErrStr(newCcbHandle);
		/*ticket 1904: end*/

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler:applyRequest():saImmOmCcbApply:FAILED - Transaction %s ", p_transactionName.c_str());

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::applyRequest():saImmOmCcbApply:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}else{

		/*Finalize ccbHandle*/
		error = saImmOmCcbFinalize(newCcbHandle);

		if (error != SA_AIS_OK){

			setInternalError(error);

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", error);

			char bufferMessage[1024] = {0};
			sprintf(bufferMessage, "OmHandler:applyRequest():saImmOmCcbFinalize:FAILED - Transaction %s ", p_transactionName.c_str());

			log.Write(bufferMessage, LOG_LEVEL_ERROR);
			//log.Write("OmHandler::applyRequest():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);
			sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
			if( buffer != 0)
				log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		/*Remove transaction by transactionList*/
		transactionList.erase(p_transactionName);


//		/*If there aren't other transaction in transactionList finalize the owner handler*/
//		if(transactionList.size() == 0){
//			removeOwnerHandle();
//		}


		/* release ownerHandle*/
		if((errorCode = saImmOmAdminOwnerFinalize(newOwnerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::removeOwnerHandle():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		transactionListOwnerHandle.erase(p_transactionName);

	}

	return ACS_CC_SUCCESS;

}



//---------------------------------------------------------------------------------------------------------------------------
// RESET REQUEST METHOD
// applyRequest applies all requests included in the configuration change bundle identified
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::resetRequest(std::string p_transactionName){

	SaAisErrorT error;

	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newOwnerHandle;

	/*Find in the map the transaction to abort*/
	map<std::string, SaImmCcbHandleT>::iterator it;
	it = transactionList.find(p_transactionName);

	if(it == transactionList.end()){

		setInternalError(42);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 42);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::resetRequest():FAILED - Transaction %s ", p_transactionName.c_str());

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::getCcbHandle():applyRequest:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}else{
		/*CCB handle of transaction to abort*/
		newCcbHandle = transactionList.find(p_transactionName)->second;
		/*Owner Handler of transaction to abort*/
		newOwnerHandle = transactionListOwnerHandle.find(p_transactionName)->second;

	}

	/*finalize CCB handle*/
	error = saImmOmCcbFinalize(newCcbHandle);

	if (error != SA_AIS_OK){

		setInternalError(error);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		char bufferMessage[1024] = {0};
		sprintf(bufferMessage, "OmHandler::resetRequest():saImmOmCcbFinalize:FAILED - Transaction %s ", p_transactionName.c_str());

		log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//log.Write("OmHandler::resetRequest():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

	}else{
		/*Remove transaction by transactionList*/
		transactionList.erase(p_transactionName);

		/*If there aren't other transaction in transactionList finalize the owner handler*/
//		if(transactionList.size() == 0){
//			removeOwnerHandle();
//		}

		/* release ownerHandle*/
		if((errorCode = saImmOmAdminOwnerFinalize(newOwnerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::removeOwnerHandle():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		transactionListOwnerHandle.erase(p_transactionName);


	}

	return ACS_CC_SUCCESS;



}



//---------------------------------------------------------------------------------------------------------------------------
// GET INTERNAL LAST ERROR METHOD
//---------------------------------------------------------------------------------------------------------------------------
int acs_apgcc_omhandler_impl::getInternalLastError(){

	return ((-1)*errorCode);
}



//---------------------------------------------------------------------------------------------------------------------------
// SET INTERNAL LAST ERROR METHOD
//---------------------------------------------------------------------------------------------------------------------------
void acs_apgcc_omhandler_impl::setInternalError(int p_errorCode){

	errorCode = p_errorCode;

}



//---------------------------------------------------------------------------------------------------------------------------
// GET INTERNAL LAST ERROR TEXT METHOD
//---------------------------------------------------------------------------------------------------------------------------
char* acs_apgcc_omhandler_impl::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}




//---------------------------------------------------------------------------------------------------------------------------
// GET EXIT CODE TEXT METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getExitCode( int& p_errorId, std::string& p_errorText){

	/**
	 * TO ADD: related to OpenSaf Ticket 1904
	 * Should retrieve the error code and Message using the saImmOmGetErrorStrings once this API
	 * will be delivered.
	 * This is just a void implementation to be completed when new API will be available.
	 */



	/* Ticket 1904 implemetation: start*/
	ACS_CC_ReturnType rVal = ACS_CC_SUCCESS ;

	/*added for temporary version : COMSA Rule 101
	 * currently the lib returns just the errorString passed by the OI implementer and 1 as default value*/

	string tag = "@ComNbi@";
	size_t lenMessageString = exitCodeStringRetrieved.size() - tag.size();
	if (exitCodeStringRetrieved.find(tag)!=string::npos && lenMessageString > 0){
		p_errorText = exitCodeStringRetrieved.substr(tag.size(), lenMessageString );
		p_errorId = 1;
	}else{/*Not Found */
		setInternalError(44);//set generic error
		//
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 44);
		//
		log.Write("OmHandler::getExitCode:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);
		rVal = ACS_CC_FAILURE;/*--not found*/
	}

	/*end added for temporary version*/

	/*Commented for temporary version: Start*/
//	size_t found;
//	found = exitCodeStringRetrieved.find("__");
//	if (found != string::npos){/*found*/
//
//		/*found the separator __ : copy from 0 to 'found' and retrieve the error text*/
//
//		p_errorText = exitCodeStringRetrieved.substr(0,found);
//
//		/*now get the error value*/
//		p_errorId = atoi ((exitCodeStringRetrieved.substr(found+2).c_str() ));
//
//
//	}else {/*--not found*/
//
//		setInternalError(44);//set generic error
//
//		char buffer[1024] = {0};
//		sprintf(buffer, "ERROR CODE %d", 44);
//
//		log.Write("OmHandler::getExitCode:FAILED", LOG_LEVEL_ERROR);
//		log.Write(buffer, LOG_LEVEL_ERROR);
//		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
//		if( buffer != 0)
//			log.Write(buffer, LOG_LEVEL_ERROR);
//		rVal = ACS_CC_FAILURE;/*--not found*/
//	}

	/*Commented for temporary version : End*/
	/* Ticket 1904 implemetation: end*/

	exitCodeStringRetrieved ="";
	return rVal;

}



//---------------------------------------------------------------------------------------------------------------------------
// GET EXIT CODE METHOD
//---------------------------------------------------------------------------------------------------------------------------
ACS_CC_ReturnType acs_apgcc_omhandler_impl::getExitCode(std::string p_transactionName, int& p_errorId, std::string& p_errorText){

	/**
	 * TO ADD: related to OpenSaf Ticket 1904
	 * Should retrieve the error code and Message using the saImmOmGetErrorStrings once this API
	 * will be delivered.
	 * This is just a void implementation to be completed when new API will be available.
	 */

	/*Ticket 1904 start*/
	/*find ccbId*/
	SaAisErrorT error;

	SaImmCcbHandleT newCcbHandle;

	map<std::string, SaImmCcbHandleT>::iterator it;
	it = transactionList.find(p_transactionName);

	if(it == transactionList.end()){

		setInternalError(42);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", 42);

		log.Write("OmHandler::getExitCode:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}else{

		newCcbHandle = transactionList.find(p_transactionName)->second;
	}

	//search Error string if present
	const SaStringT* errorStringFromOi=0;
	error =  saImmOmCcbGetErrorStrings(newCcbHandle, &errorStringFromOi);

	if (error != SA_AIS_OK){

		setInternalError(error);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", error);

		log.Write("OmHandler::getExitCode():saImmOmCcbGetErrorStrings:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
		if( buffer != 0)
			log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;

		}else{

			if (errorStringFromOi !=0 ){
				//cout << "Ticket 1904: error string = "<< *errorStringFromOi<<endl;
				std::string errorFound(*errorStringFromOi);

				/*added for temporary version : COMSA Rule 101
				 * currently the lib returns just the errorString passed by the OI implementer and 1 as default value*/

				string tag = "@ComNbi@";
				size_t lenMessageString = errorFound.size() - tag.size();
				if (errorFound.find(tag)!=string::npos && lenMessageString > 0){
					p_errorText = errorFound.substr(tag.size(), lenMessageString );
					p_errorId = 1;
				}else{/*Not Found */
					setInternalError(44);//set generic error
					//
					char buffer[1024] = {0};
					sprintf(buffer, "ERROR CODE %d", 44);
					//
					log.Write("OmHandler::getExitCode:FAILED", LOG_LEVEL_ERROR);
					log.Write(buffer, LOG_LEVEL_ERROR);
					sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
					if( buffer != 0)
						log.Write(buffer, LOG_LEVEL_ERROR);
					return ACS_CC_FAILURE;/*--not found*/
				}
				/*end added for COMSA*/

//				size_t found;
//				found = errorFound.find("__");
//				if (found != string::npos){
//
//					/*found the separator __ copy from 0 to found the error text*/
//
//					p_errorText = errorFound.substr(0,found);
//					//cout << "Ticket 1904: p_errorText "<<p_errorText<<endl;
//					p_errorId = atoi ((errorFound.substr(found+2).c_str() ));
//
//				}else {
//					setInternalError(44);//set generic error
//
//					char buffer[1024] = {0};
//					sprintf(buffer, "ERROR CODE %d", 44);
//
//					log.Write("OmHandler::getExitCode:FAILED", LOG_LEVEL_ERROR);
//					log.Write(buffer, LOG_LEVEL_ERROR);
//					sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
//					if( buffer != 0)
//						log.Write(buffer, LOG_LEVEL_ERROR);
//
//					return ACS_CC_FAILURE;/*--not found*/
//				}




			}else /*string null*/{
				setInternalError(44);//set generic error

				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", 44);

				log.Write("OmHandler::getExitCode:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
				sprintf(buffer, "MESSAGE ERROR: %s\n", getInternalLastErrorText());
				if( buffer != 0)
					log.Write(buffer, LOG_LEVEL_ERROR);
				return ACS_CC_FAILURE;
			}


		}




	/*Ticket 1904 end*/

//	p_errorId = 0;
//	p_errorText = "";
//	ACE_UNUSED_ARG(p_transactionName);
	return ACS_CC_SUCCESS;
}




//---------------------------------------------------------------------------------------------------------------------------
// GET Ccb HANDLE METHOD
//---------------------------------------------------------------------------------------------------------------------------
int acs_apgcc_omhandler_impl::getCcbHandle(std::string p_transactionName, SaImmCcbHandleT * p_newCcbHandle, SaImmAdminOwnerHandleT * p_ownerHandle){

	SaAisErrorT error;

//	/*If there aren't other transaction Initialize Owner Handler */
//	if(transactionList.size() == 0){
//		ACE_OS::fprintf(stdout, "Inizializzo nuovo OwnerHandler\n");
//		setOwnerHandle();
//	}


	map<std::string, SaImmCcbHandleT>::iterator it;
	it = transactionList.find(p_transactionName);

	if(it == transactionList.end()){
		/*If the transaction is already defined*/

		/*Initialize new ownerHandle*/
		errorCode = saImmOmAdminOwnerInitialize(immHandle, (char *) "APG_TRANSACTION",SA_TRUE, p_ownerHandle);

		if (errorCode != SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("OmHandler::setOwnerHandle():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		/*Initialize new ccbHandle*/
		//error = saImmOmCcbInitialize((*p_ownerHandle), 0 , p_newCcbHandle);
		// Added ccbFlagVal to permit to choose if oprtation jas to be performed on an object with implemented registered or not
		error = saImmOmCcbInitialize((*p_ownerHandle), ccbFlagVal, p_newCcbHandle);

		if (error != SA_AIS_OK)	{

			setInternalError(error);

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", error);

			char bufferMessage[1024] = {0};
			sprintf(bufferMessage, "OmHandler::getCcbHandle():saImmOmCcbInitialize:FAILED - Transaction %s ", p_transactionName.c_str());

			log.Write(bufferMessage, LOG_LEVEL_ERROR);
		//	log.Write("OmHandler::getCcbHandle():saImmOmCcbInitialize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);
			sprintf(buffer, "MESSAGE ERROR: %s", getInternalLastErrorText());
			if( buffer != 0)
				log.Write(buffer, LOG_LEVEL_ERROR);

			return error;
		}

		/*Add new ccb at the map*/
		transactionList[p_transactionName] = *p_newCcbHandle;
		/*Add new ownerHandle at the map*/
		transactionListOwnerHandle[p_transactionName] = *p_ownerHandle;

	}else{
		//ACE_OS::fprintf(stdout, "Transazione gia presente restituisco il ccbHandle associato\n");
		*p_newCcbHandle = transactionList.find(p_transactionName)->second;
		*p_ownerHandle = transactionListOwnerHandle.find(p_transactionName)->second;
	}

	return 0;
}
/*End Added For Object creation*/




acs_apgcc_omhandler_impl::~acs_apgcc_omhandler_impl() {
	// TODO Auto-generated destructor stub
	log.Close();
}



//---------------------------------------------------------------------------------------------------------------------------
// GET Ccb HANDLE VALUE METHOD
//---------------------------------------------------------------------------------------------------------------------------
unsigned long long int acs_apgcc_omhandler_impl::getCcbHandleValue(){
	return ccbHandle;

}



//---------------------------------------------------------------------------------------------------------------------------
// SET OWNER HANDLE METHOD
//---------------------------------------------------------------------------------------------------------------------------
//int acs_apgcc_omhandler_impl::setOwnerHandle(){
//
//	/**Initialize the Object Administration ownership to perform operations on IMM
//	 *
//	 *immHandle is the handle retrieved by phe previous call
//	 *second parameter is the Administrative Owner name
//	 *third parameter specifies whether to release the Management Ownership
//	 *				  when finalizing the ownerHandle
//	 *ownerHandle is the output parameter where the Management Ownership handler
//	 *			  is stored
//	 */
//	errorCode = saImmOmAdminOwnerInitialize(immHandle, (char *) "APG_TRANSACTION",SA_TRUE, &ownerHandle);
//
//	if (errorCode != SA_AIS_OK){
//
//		setInternalError(errorCode);
//		char buffer[1024] = {0};
//		sprintf(buffer, "ERROR CODE %d", errorCode);
//
//		log.Write("OmHandler::setOwnerHandle():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
//		log.Write(buffer, LOG_LEVEL_ERROR);
//
//		return ACS_CC_FAILURE;
//	}
//
////	SaNameT objOwnedName = {0,{0}};
////	const SaNameT *objOwnedNameList[] = {&objOwnedName, 0};
////
////
////	/*set the Owned tree */
////	objOwnedName.length = sizeof ("safApp=safImmService");
////	memcpy(objOwnedName.value, "safApp=safImmService", objOwnedName.length);
////
//////	objOwnedName.length = sizeof ("apzFunctionsId=1,managedElementId=1");
//////	memcpy(objOwnedName.value, "apzFunctionsId=1,managedElementId=1", objOwnedName.length);
////
////	//For VENKATESH
//////	objOwnedName.length = sizeof ("statisticalCounterMId=1");
//////	memcpy(objOwnedName.value, "statisticalCounterMId=1", objOwnedName.length);
////
////	/* end set the Owned tree*/
////
////	errorCode = saImmOmAdminOwnerSet (ownerHandle,objOwnedNameList, SA_IMM_SUBTREE );
////	if (errorCode != SA_AIS_OK)	{
////
////		setInternalError(errorCode);
////
////		char buffer[1024] = {0};
////		sprintf(buffer, "ERROR CODE %d", errorCode);
////
////		log.Write("OmHandler::Init():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
////		log.Write(buffer, LOG_LEVEL_ERROR);
////
////		/*release ownerHandle*/
////		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){
////
////			setInternalError(errorCode);
////			char buffer[1024] = {0};
////			sprintf(buffer, "ERROR CODE %d", errorCode);
////
////			log.Write("OmHandler::Init():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
////			log.Write(buffer, LOG_LEVEL_ERROR);
////		}
////		/*end release ownerHandle*/
////
////		return ACS_CC_FAILURE;
////	}
//
//
//	return ACS_CC_SUCCESS;
//}



//---------------------------------------------------------------------------------------------------------------------------
// REMOVE OWNER HANDLE METHOD
//---------------------------------------------------------------------------------------------------------------------------
//int acs_apgcc_omhandler_impl::removeOwnerHandle(){
//
//
//	/** release ccbHandler**/
////	if ( (errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK ){
////
////		setInternalError(errorCode);
////		char buffer[1024] = {0};
////		sprintf(buffer, "ERROR CODE %d", errorCode);
////
////		log.Write("OmHandler::removeOwnerHandle():saImmOmCcbFinalize:FAILED", LOG_LEVEL_ERROR);
////		log.Write(buffer, LOG_LEVEL_ERROR);
////
////		return ACS_CC_FAILURE;
////	}
//
//	/* release ownerHandle*/
//	if((errorCode = saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){
//
//		setInternalError(errorCode);
//		char buffer[1024] = {0};
//		sprintf(buffer, "ERROR CODE %d", errorCode);
//
//		log.Write("OmHandler::removeOwnerHandle():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
//		log.Write(buffer, LOG_LEVEL_ERROR);
//
//		return ACS_CC_FAILURE;
//	}
//
//
//	return ACS_CC_SUCCESS;
//}

//---------------------------------------------------------------------------------------------------------------------------
// Method to get the error string from OI and to set the retrieved string in exitCodeStringRetrieved variable Ticket 1904
//---------------------------------------------------------------------------------------------------------------------------


int acs_apgcc_omhandler_impl::callSaImmOmCcbGetErrStr(SaImmCcbHandleT p_ccbVal){

	SaAisErrorT errorCodeForGetErr;
	const SaStringT* errorStringFromOi=0;
	/*following to remove*/
	//const SaStringT* errorStringFromOi_temp=0;
	//int i_count = 0;
	/*following to remove end */

	/*clear the previous value for exitCodeStringRetrieved*/
	exitCodeStringRetrieved.clear();

	/*get the error*/
	errorCodeForGetErr =  saImmOmCcbGetErrorStrings(p_ccbVal, &errorStringFromOi);

	if (errorCodeForGetErr == SA_AIS_OK){
		//cout << "!Ticket 1904: acs_apgcc_omhandler_impl::callSaImmOmCcbGetErrStr OK "<<endl;
//		/*following to remove*/
//		errorStringFromOi_temp = errorStringFromOi;
//		while (errorStringFromOi_temp != 0){
//			cout << "Counting errorStringFromOi_temp :  "<<i_count<<endl;
//			i_count++;
//		}
//		/*following to remove end */

		//while (errorStringFromOi !=0 ){
		if (errorStringFromOi !=0 ){


			//cout << "Ticket 1904: callSaImmOmCcbGetErrStr::errorStringFromOi : "<<(*errorStringFromOi)<<endl;

			/*set the retrieved value in exitCodeStringRetrieved*/
			exitCodeStringRetrieved = (*errorStringFromOi);
			//cout << "Ticket 1904: callSaImmOmCcbGetErrStr::exitCodeStringRetrieved : "<<exitCodeStringRetrieved<<endl;

			errorStringFromOi++;
		}
		//cout << "Ticket 1904: acs_apgcc_omhandler_impl::callSaImmOmCcbGetErrStr::EXITING "<<endl;
		return 0; /*on success*/

	}else{
		//cout << "Ticket 1904: acs_apgcc_omhandler_impl::callSaImmOmCcbGetErrStr KO "<<endl;
		//cout << "Ticket 1904: errorCodeForGetErr : "<<errorCodeForGetErr<<endl;



		setInternalError(errorCodeForGetErr);
		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCodeForGetErr);

		log.Write("OmHandler::callSaImmOmCcbGetErrStr::saImmOmCcbGetErrorStrings::FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return -1; /*on failure*/
	}



}
