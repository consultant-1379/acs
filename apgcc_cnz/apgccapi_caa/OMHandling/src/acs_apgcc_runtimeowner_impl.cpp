/*
 * ACS_APGCC_RuntimeOwnerV2.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: xfabron
 */

#include "acs_apgcc_runtimeowner_impl.h"
#include "ACS_APGCC_RuntimeOwner.h"
#include <stdio.h>
#include <stdlib.h>
#include "ACS_TRA_trace.h"
#include "acs_apgcc_error.h"

//#define DEBUG

namespace {
	ACS_TRA_trace trace(const_cast<char*>("ACS_APGCC_RuntimeOwner"), const_cast<char *>("C300"));
}

map<SaUint64T,ACS_APGCC_RuntimeOwner *> acs_apgcc_runtimeowner_impl::implementerList;



acs_apgcc_runtimeowner_impl::acs_apgcc_runtimeowner_impl() {

	immOiHandle = 0;

	selObj = 0;

	errorCode = 0;

	runtimeOwn = 0;

	log.Open("APGCC");

}


acs_apgcc_runtimeowner_impl::acs_apgcc_runtimeowner_impl(acs_apgcc_runtimeowner_impl *runtimeImp){


	this->immOiHandle = runtimeImp->immOiHandle;
	this->selObj = runtimeImp->selObj;
	this->errorCode = runtimeImp->errorCode;
	this->errorCode = runtimeImp->errorCode;
	this->runtimeOwn = runtimeImp->runtimeOwn;

	log.Open("APGCC");

}


ACS_CC_ReturnType acs_apgcc_runtimeowner_impl::init(std::string impName, ACS_APGCC_RuntimeOwner *p_runtimeOwn){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_RuntimeOwner::init(std::string impName)");
		trace.ACS_TRA_event(1, buffer);
	}


	SaAisErrorT errorCode;

	SaVersionT version;

	runtimeOwn = p_runtimeOwn;

	/** Set the IMM version: **/
	version.releaseCode  = 'A';
	version.majorVersion =  2;
	version.minorVersion =  1;

	SaImmOiCallbacksT_2 immCallbacks;

	/** Set the SaImmOiCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/

	immCallbacks.saImmOiCcbObjectCreateCallback = 0;
	immCallbacks.saImmOiCcbObjectModifyCallback = 0;
	immCallbacks.saImmOiCcbObjectDeleteCallback = 0;
	immCallbacks.saImmOiCcbCompletedCallback = 0;
	immCallbacks.saImmOiCcbApplyCallback = 0;
	immCallbacks.saImmOiCcbAbortCallback = 0;

	immCallbacks.saImmOiRtAttrUpdateCallback = acs_apgcc_runtimeowner_impl::OiRtAttUpdateCallback;


	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks , &version);

	if ( errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::init():saImmOiInitialize_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			printf(" IMM Version Actually Installed:"
				" Major Version %c, Minor Version %u, Relese Code %u",
				version.releaseCode,version.minorVersion,version.majorVersion);
			printf("ACS_APGCC_RuntimeOwner::init():saImmOiInitialize_2:FAILED\n");
			printf("ERROR CODE %u\n",errorCode);
		#endif


		return ACS_CC_FAILURE;
	}

	SaImmOiImplementerNameT implementerName = const_cast<char*>(impName.c_str());

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object implementer having the name which is
	 * specified in the implementerName parameter.  **/
	errorCode = saImmOiImplementerSet(immOiHandle, implementerName);

	if( errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::init():saImmOiImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			printf("ACS_APGCC_RuntimeOwner::init():saImmOiImplementerSet:FAILED\n");
			printf("ERROR CODE %u\n",errorCode);
		#endif

/**Release immOihandle*/

		errorCode = saImmOiFinalize(immOiHandle);

			if (errorCode != SA_AIS_OK){

				setInternalError(errorCode);

				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("ACS_APGCC_RuntimeOwner::init():saImmOiFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);

				#ifdef DEBUG
					printf("ACS_APGCC_RuntimeOwner::init():saImmOiFinalize:FAILED\n");
					printf("ERROR CODE %u\n",errorCode);
				#endif

				return ACS_CC_FAILURE;
			}
/**End release immOiHandle*/

		return ACS_CC_FAILURE;
	}


	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj);

	if( errorCode != SA_AIS_OK ){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::init():saImmOiSelectionObjectGet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			printf("ACS_APGCC_RuntimeOwner::init():saImmOiSelectionObjectGet:FAILED\n");
			printf("ERROR CODE %u\n",errorCode);
		#endif

		return ACS_CC_FAILURE;
	}

	/**Add implementer at the list**/
	implementerList[immOiHandle] = runtimeOwn;

	return ACS_CC_SUCCESS;
}


ACS_CC_ReturnType acs_apgcc_runtimeowner_impl::finalize(){


	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_RuntimeOwner::finalize()");
		trace.ACS_TRA_event(1, buffer);
	}


	SaAisErrorT errorCode;

	/* clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer for the name previously associated
	 *
	 * immOiHandle is the handle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 function
	 * */
	errorCode = saImmOiImplementerClear(immOiHandle);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::finalize():saImmOiImplementerClear:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			printf("ACS_APGCC_RuntimeOwner::finalize():saImmOiImplementerClear:FAILED\n");
			printf("ERROR CODE %u\n",errorCode);
		#endif

		return ACS_CC_FAILURE;
	}


	implementerList.erase(immOiHandle);


	/* closes the association represented by the immOiHandle parameter between the
	 * invoking process and the information Model
	 *
	 * immOiHandle is the ahandle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 and witch identifies this particular initization of the
	 * IMM service
	 * */
	errorCode = saImmOiFinalize(immOiHandle);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::finalize():saImmOiFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			printf("ACS_APGCC_RuntimeOwner::finalize():saImmOiFinalize:FAILED\n");
			printf("ERROR CODE %u\n",errorCode);
		#endif

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}



/**This method creates a runtime Object**/
ACS_CC_ReturnType acs_apgcc_runtimeowner_impl::createRuntimeObj(const char* p_className, const char* p_parentName,
		vector<ACS_CC_ValuesDefinitionType> m_attrValuesList){


	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_RuntimeOwner::createRuntimeObj(const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> m_attrValuesList)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaNameT* name = 0;
	vector<SaNameT*> SaNameVector;

	/* The name of the class*/
	SaImmClassNameT className = 0;

	/* The parent Name of the creating OBJECT*/
	SaNameT parentName = {0,{0}};

	/*An iterator to access the attributes list*/
	vector<ACS_CC_ValuesDefinitionType>::iterator it;

	/*The attribute list to be passed to the function saImmOiRtObjectCreate_2*/
	SaImmAttrValuesT_2 **attributesValues = 0;

	/*Build the SaNameT variable to be passed to the  saImmOiRtObjectCreate_2 function*/

	className = const_cast<char*>(p_className);

	/*Set in parentName.length the m_parentName string length*/
	parentName.length = strlen(p_parentName);

	/*Copy in parentName.value the m_parentName value*/
	memcpy(parentName.value, p_parentName, parentName.length);

	/* Create the list of Attribute Values to be provided as input to the saImmOiRtObjectCreate_2 function*/
	/*allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
	attributesValues = new SaImmAttrValuesT_2*[m_attrValuesList.size()+1];

	int i = 0;

	int numAttribute = m_attrValuesList.size();

	#ifdef DEBUG
		cout << "NUMERO DI ATTRIBUTI: " << numAttribute << endl;
	#endif

	SaImmAttrValuesT_2 _singleAttributeValues[numAttribute];

	for(it = m_attrValuesList.begin(); it != m_attrValuesList.end(); it++  ){


		/*Set the attribute Name*/
		_singleAttributeValues[i].attrName = (SaImmAttrNameT)((*it).attrName);

		#ifdef DEBUG
		//	cout<<"AttrName: "<<singleAttributeValues->attrName<<endl;
		#endif

		/*Set the attribute Type*/
		_singleAttributeValues[i].attrValueType = (SaImmValueTypeT)((*it).attrType);

		/*Set the Attribute Values Number*/
		unsigned int numElem = (*it).attrValuesNum;

		_singleAttributeValues[i].attrValuesNumber = (SaUint32T)(numElem);

		/*Set attribute values To be done*/

		_singleAttributeValues[i].attrValues = new void*[numElem];

		for( unsigned int j=0; j<numElem; j++){

			switch ((*it).attrType){
			case ATTR_INT32T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT32T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_INT64T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_TIMET:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT64T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_DOUBLET:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_FLOATT:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_STRINGT:
				_singleAttributeValues[i].attrValues[j] = &((*it).attrValues[j]);
				break;

			case ATTR_NAMET:

				name = new SaNameT;
				unsigned int howlong;
				howlong = strlen ( reinterpret_cast<char*>((*it).attrValues[j]));

				name->length =  howlong ;
				memcpy(name->value, (*it).attrValues[j], name->length);

				_singleAttributeValues[i].attrValues[j] = reinterpret_cast<void*>(name);

				/**Used to deallocate memory for name variables **/
				SaNameVector.push_back(name);
				break;

			default:
				break;

			}/*end switch case on the attribute Type*/

		}/**End for on each multiple value for a single attribute*/
		/*END set Attribute values*/

		/*add the singleAttributeValues to the vector attributesValues*/
		attributesValues[i] = &(_singleAttributeValues[i]);

		i++;

	}/*end for that goes through the input vector and builds the list to be passed to the function*/
	/*Last element in attributes Value list must be NULL*/

	attributesValues[i] = 0;

	#ifdef DEBUG
		cout<<"className: "<<className<<endl;
		cout<<"parentName: "<<parentName.value<<endl;
	#endif

	errorCode = saImmOiRtObjectCreate_2(immOiHandle, className, &parentName, (const SaImmAttrValuesT_2**)attributesValues);

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


	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[20] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::createRuntimeObj():saImmOiRtObjectCreate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			ACE_OS::fprintf(stdout, "ACS_APGCC_RuntimeOwner::createRuntimeObj():saImmOiRtObjectCreate_2:FAILLURE\n");
			ACE_OS::fprintf(stdout, "ERROR CODE %u \n", errorCode);
		#endif

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}




/**This method deletes the runtime Object**/
ACS_CC_ReturnType acs_apgcc_runtimeowner_impl::deleteRuntimeObj(const char *p_objName){


	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_RuntimeOwner::deleteRuntimeObj(const char *p_objName)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;
	SaNameT objToDelete = {0,{0}};

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(p_objName);

	/*Copy in objToDelete.value the m_objectName value*/

	memcpy(objToDelete.value, p_objName, objToDelete.length);

	/*Call the saImmOiRtObjectDelete */
	if((errorCode = saImmOiRtObjectDelete(immOiHandle, &objToDelete ))!= SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::deleteRuntimeObj():saImmOiRtObjectDelete:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			ACE_OS::fprintf(stdout, "ACS_APGCC_RuntimeOwner::deleteRuntimeObj():saImmOiRtObjectDelete:FAILLURE\n");
			ACE_OS::fprintf(stdout, "ERROR CODE %u \n", errorCode);
		#endif

		return ACS_CC_FAILURE;

	}

	return ACS_CC_SUCCESS;
}


/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType acs_apgcc_runtimeowner_impl::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_RuntimeOwner::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue)");
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
	errorCode = saImmOiRtObjectUpdate_2(immOiHandle, &objectName, (const SaImmAttrModificationT_2**)attrMods );

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

		log.Write("ACS_APGCC_RuntimeOwner::modifyRuntimeObj():saImmOiRtObjectUpdate_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			ACE_OS::fprintf(stdout, "ACS_APGCC_RuntimeOwner::modifyRuntimeObj():saImmOiRtObjectUpdate_2:FAILED\n");
			ACE_OS::fprintf(stdout, "ERROR CODE %u \n", errorCode);
		#endif

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}




/**This method invokes pending callbacks for the IMM handle **/
ACS_CC_ReturnType acs_apgcc_runtimeowner_impl::dispatch(ACS_APGCC_DispatchFlags p_flag){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_RuntimeOwner::dispatch(ACS_APGCC_DispatchFlags p_flag)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaDispatchFlagsT flag = (SaDispatchFlagsT) p_flag;

	/** This function invokes pending callbacks for handle immOiHandle in the way
	 * that is specified by the flag parameter**/
	errorCode = saImmOiDispatch(immOiHandle, flag);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("ACS_APGCC_RuntimeOwner::dispatch():saImmOiDispatch:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		#ifdef DEBUG
			printf("ACS_APGCC_RuntimeOwner::dispatch():saImmOiDispatch:FAILED\n");
			printf("ERROR CODE %u",errorCode);
		#endif

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;
}


SaAisErrorT acs_apgcc_runtimeowner_impl::OiRtAttUpdateCallback(SaImmOiHandleT immOiHandle,
			const SaNameT *objectName,
			const SaImmAttrNameT *attributeNames){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "ACS_APGCC_RuntimeOwner::OiRtAttUpdateCallback(SaImmOiHandleT immOiHandle, const SaNameT *objectName, const SaImmAttrNameT *attributeNames)");
		trace.ACS_TRA_event(1, buffer);
	}

	ACS_CC_ReturnType returnCode;

	/**Find in the map the implementer associated at the immOiHandle**/
	ACS_APGCC_RuntimeOwner *obj = (ACS_APGCC_RuntimeOwner *)(acs_apgcc_runtimeowner_impl::implementerList.find(immOiHandle)->second);

	char *nameObj = new char[(objectName->length) + 1];
	memset( nameObj, '\0', (objectName->length) +1 );
	memcpy(nameObj, objectName->value, objectName->length );

	char *nameAttr = *attributeNames;

	returnCode = obj->updateCallback(nameObj, nameAttr);

	delete []nameObj;

	return SA_AIS_OK;
}




/**This method is used to get the operating system handle that invoking
	 * process can use to detect pending callbacks**/
int acs_apgcc_runtimeowner_impl::getSelObj() const{

	return (int)selObj;
}


int acs_apgcc_runtimeowner_impl::getInternalLastError(){

	return ((-1)*errorCode);
}


void acs_apgcc_runtimeowner_impl::setInternalError(int p_errorCode){

	errorCode = p_errorCode;

}


char* acs_apgcc_runtimeowner_impl::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}


acs_apgcc_runtimeowner_impl::~acs_apgcc_runtimeowner_impl(){

	log.Close();
}
