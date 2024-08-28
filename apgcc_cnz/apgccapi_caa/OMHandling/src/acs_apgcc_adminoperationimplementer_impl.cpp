/*
 * acs_apgcc_adminoperationimplementer_impl.cpp
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_adminoperationimplementer_impl.h"
#include "acs_apgcc_adminoperationimplementer.h"

#include <stdio.h>
#include <stdlib.h>
#include "ACS_TRA_trace.h"
#include "acs_apgcc_error.h"

namespace {
	ACS_TRA_trace trace(const_cast<char*>("acs_apgcc_adminoperationimplementer"), const_cast<char *>("C300"));
}

//map<SaUint64T,ACS_APGCC_RuntimeOwner *> acs_apgcc_runtimeowner_impl::implementerList;
map<SaUint64T,acs_apgcc_adminoperationimplementer *> acs_apgcc_adminoperationimplementer_impl::implementerList;



acs_apgcc_adminoperationimplementer_impl::acs_apgcc_adminoperationimplementer_impl() {

	immOiHandle = 0;

	selObj = 0;

	errorCode = 0;

	runtimeOwn = 0;

	log.Open("APGCC");


}

acs_apgcc_adminoperationimplementer_impl::acs_apgcc_adminoperationimplementer_impl(acs_apgcc_adminoperationimplementer_impl *adminOperationImpl){

	this->immOiHandle = adminOperationImpl->immOiHandle;

	this->selObj = adminOperationImpl->selObj;

	this->errorCode = adminOperationImpl->errorCode;

	this->runtimeOwn = adminOperationImpl->runtimeOwn;

	log.Open("APGCC");
}

ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::init(std::string impName, acs_apgcc_adminoperationimplementer *p_adminOperationImpl){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationimplementer::init(std::string impName)");
		trace.ACS_TRA_event(1, buffer);
	}


	SaAisErrorT errorCode;

	SaVersionT version;

	runtimeOwn = p_adminOperationImpl;

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
	immCallbacks.saImmOiRtAttrUpdateCallback = 0;

	immCallbacks.saImmOiAdminOperationCallback = acs_apgcc_adminoperationimplementer_impl::adminOperationCallback;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks , &version);

	if ( errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationimplementer::init():saImmOiInitialize_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

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

		log.Write("acs_apgcc_adminoperationimplementer::init():saImmOiImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


/**Release immOihandle*/

		errorCode = saImmOiFinalize(immOiHandle);

			if (errorCode != SA_AIS_OK){

				setInternalError(errorCode);

				char buffer[1024] = {0};
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("acs_apgcc_adminoperationimplementer::init():saImmOiFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);


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

		log.Write("acs_apgcc_adminoperationimplementer::init():saImmOiSelectionObjectGet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


		return ACS_CC_FAILURE;
	}

	/**Add implementer at the list**/
	implementerList[immOiHandle] = runtimeOwn;

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::setObjectImplemented(std::string p_objName){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationimplementer::setObjectImplemented(std::string p_objName");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaNameT objName;

	objName.length = p_objName.size();
	memcpy(objName.value, p_objName.c_str(), objName.length);

	errorCode = saImmOiObjectImplementerSet(immOiHandle, &objName, SA_IMM_ONE);

	if( errorCode != SA_AIS_OK ){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationimplementer::setObjectImplemented:saImmOiObjectImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

}

ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::setClassImplemented(std::string p_className){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_adminoperationimplementer::setClassImplemented(std::string p_objName");
		trace.ACS_TRA_event(1, buffer);
	}



	SaAisErrorT errorCode;

	SaImmClassNameT className;

	className = const_cast<char*>(p_className.c_str());

	errorCode = saImmOiClassImplementerSet (immOiHandle, className);

	if( errorCode != SA_AIS_OK ){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationimplementer::setClassImplemented:saImmOiClassImplementerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

}

ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::dispatch(ACS_APGCC_DispatchFlags p_flag){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_adminoperationimplementer::dispatch(ACS_APGCC_DispatchFlags p_flag)");
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

		log.Write("acs_apgcc_adminoperationimplementer::dispatch()::saImmOiDispatch:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

}

ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::releaseObjectImplemented(std::string p_objName){

	SaAisErrorT errorCode;

	SaNameT objName;

//	objName.length = p_objName.size();
//	memcpy(objName.value, p_objName.c_str(), objName.length);

	objName.length = p_objName.length();
	objName.value[p_objName.copy(reinterpret_cast<char *>(objName.value), SA_MAX_NAME_LENGTH)] = '\0';


	if (trace.ACS_TRA_ON()){
			char buffer[1024] = {0};
			sprintf(buffer, "acs_apgcc_adminoperationimplementer::releaseObjectImplemented(std::string p_objName) ");
			trace.ACS_TRA_event(1, buffer);
		}


	errorCode = saImmOiObjectImplementerRelease(immOiHandle, &objName, SA_IMM_ONE);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("1acs_apgcc_adminoperationimplementer::releaseObjectImplemented:saImmOiObjectImplementerRelease:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		char buffer2[1024] = {0};
		sprintf(buffer2, "2acs_apgcc_adminoperationimplementer::releaseObjectImplemented(%s)\n objName.value =%s, objName.len=%d",p_objName.c_str(),objName.value, objName.length);
		log.Write(buffer2, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/*finalize-*/
	ACS_CC_ReturnType errorCode2;

	errorCode2 = finalize();

	if (errorCode2 != ACS_CC_SUCCESS){

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode2);

		log.Write("acs_apgcc_adminoperationimplementer::releaseObjectImplemented:finalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}
	/*end finalize*/

	return ACS_CC_SUCCESS;
}
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::releaseClassImplemented(std::string p_className){

	SaAisErrorT errorCode;
	SaImmClassNameT className;

	className = const_cast<char*>(p_className.c_str());

	errorCode = saImmOiClassImplementerRelease(immOiHandle, className);

		if (errorCode != SA_AIS_OK){

			setInternalError(errorCode);

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperationimplementer::releaseClassImplemented:saImmOiClassImplementerRelease:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		/*finalize*/
		ACS_CC_ReturnType errorCode2;
		errorCode2 = finalize();

		if (errorCode2 != ACS_CC_SUCCESS){

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode2);

			log.Write("acs_apgcc_adminoperationimplementer::releaseClassImplemented:finalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}
		/*end finalize*/

		return ACS_CC_SUCCESS;

}

//ACS_CC_ReturnType finalize();
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::finalize(){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_adminoperationimplementer::finalize()");
		trace.ACS_TRA_event(1, buffer);
	}
	SaAisErrorT errorCode;

	errorCode = saImmOiImplementerClear(immOiHandle);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationimplementer::finalize()::saImmOiImplementerClear:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


		return ACS_CC_FAILURE;
	}


	implementerList.erase(immOiHandle);

	errorCode = saImmOiFinalize(immOiHandle);

		if (errorCode != SA_AIS_OK){

			setInternalError(errorCode);

			char buffer[1024] = {0};
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperationimplementer::finalize()::saImmOiFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		return ACS_CC_SUCCESS;

}
int acs_apgcc_adminoperationimplementer_impl::getSelObj() const {

	return (int)selObj;
}

int acs_apgcc_adminoperationimplementer_impl::getInternalLastError(){

	return ((-1)*errorCode);
}


void acs_apgcc_adminoperationimplementer_impl::setInternalError(int p_errorCode){

	errorCode = p_errorCode;

}


char* acs_apgcc_adminoperationimplementer_impl::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}

/*adminOperationResult*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer_impl::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_adminoperationimplementer::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	errorCode = saImmOiAdminOperationResult(oiHandle, invocation, (SaAisErrorT)result );

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationimplementer::adminOperationResult::saImmOiAdminOperationResult:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;


}



void acs_apgcc_adminoperationimplementer_impl::adminOperationCallback(
		SaImmOiHandleT immOiHandle,
		SaInvocationT invocation,
		const SaNameT *objectName,
		SaImmAdminOperationIdT operationId,
		const SaImmAdminOperationParamsT_2 **params){

	if (trace.ACS_TRA_ON()){
		char buffer[1024] = {0};
		sprintf(buffer, "acs_apgcc_adminoperationimplementer::adminOperationCallback(SaImmOiHandleT immOiHandle,SaInvocationT invocation,const SaNameT *objectName,SaImmAdminOperationIdT operationId,const SaImmAdminOperationParamsT_2 **params)");
		trace.ACS_TRA_event(1, buffer);
	}


	/**Find in the map the implementer associated at the immOiHandle**/
	acs_apgcc_adminoperationimplementer *obj = (acs_apgcc_adminoperationimplementer *)(acs_apgcc_adminoperationimplementer_impl::implementerList.find(immOiHandle)->second);
/*from here to control*/

	/*set the object name to be passed*/
	char *nameObj = new char[(objectName->length) + 1];
	memset( nameObj, '\0', (objectName->length) +1 );
	memcpy(nameObj, objectName->value, objectName->length );

	/*create the list of params*/

	/**Calculate the size of array of attribute descriptor**/
	int dim = 0;
	int i = 0;
	while(params[i]){
		i++;
		dim++;
	}

	/*Prepares data to be provided to adminOperationCallback function*/
	ACS_APGCC_AdminOperationParamType *myAttributeList[dim+1];
	ACS_APGCC_AdminOperationParamType oneAttr[dim];

	i=0;
	while(params[i]){

		oneAttr[i].attrName = params[i]->paramName;

		oneAttr[i].attrType = (ACS_CC_AttrValueType)(params[i]->paramType);



		if(params[i]->paramType == SA_IMM_ATTR_SASTRINGT){

				SaStringT stringVal = *(reinterpret_cast<SaStringT *>(params[i]->paramBuffer));
				oneAttr[i].attrValues = reinterpret_cast<char*>(stringVal);


		}else if (params[i]->paramType == SA_IMM_ATTR_SANAMET){
			//for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				SaNameT* localSaName;
				localSaName = reinterpret_cast<SaNameT*>(params[i]->paramBuffer);
				oneAttr[i].attrValues = localSaName->value;
			//}

		}else {
			//for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				oneAttr[i].attrValues = params[i]->paramBuffer;
			//}
		}

		myAttributeList[i] = &(oneAttr[i]);
		i++;
	}

	/**the myAttributeList must be a null terminated array**/
	myAttributeList[dim] = 0;

	/*end create list of params*/




	obj->adminOperationCallback(immOiHandle, (ACS_APGCC_InvocationType) invocation, nameObj, (ACS_APGCC_AdminOperationIdType)operationId, myAttributeList);

	delete []nameObj;

	/*to here*/
}
/*Start*/

/*end*/

acs_apgcc_adminoperationimplementer_impl::~acs_apgcc_adminoperationimplementer_impl() {

	log.Close();
}
