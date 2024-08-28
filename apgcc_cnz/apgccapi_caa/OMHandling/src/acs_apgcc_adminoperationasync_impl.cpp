/*
 * acs_apgcc_adminoperationasync_impl.cpp
 *
 *  Created on: Jul 10, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_adminoperationasync_impl.h"
#include "acs_apgcc_adminoperationasync.h"


#include <iostream>
#include "ACS_TRA_trace.h"
#include "acs_apgcc_error.h"

#include "ACS_APGCC_Util.H"

namespace {
	ACS_TRA_trace trace(const_cast<char*>("acs_apgcc_adminoperationasync"), const_cast<char *>("C300"));
}

/*1**/

map<SaUint64T,acs_apgcc_adminoperationasync *> acs_apgcc_adminoperationasync_impl::adminOpList;


acs_apgcc_adminoperationasync* objImp= 0;

acs_apgcc_adminoperationasync_impl::acs_apgcc_adminoperationasync_impl() {

	immHandle = 0;

	ownerHandle = 0;

	errorCode = 0;

	selObj = 0;

	log.Open("APGCC");

}

acs_apgcc_adminoperationasync_impl::acs_apgcc_adminoperationasync_impl(acs_apgcc_adminoperationasync_impl* obj){

	this->immHandle = obj->immHandle;

	this->ownerHandle = obj->ownerHandle;

	this->errorCode = obj->errorCode;

	this->selObj = obj->selObj;

	this->processName = obj->processName;

	this->version.releaseCode  = obj->version.releaseCode;

	this->version.majorVersion =  obj->version.majorVersion;

	this->version.minorVersion =  obj->version.minorVersion;

	this->omAdminOp = obj->omAdminOp;

	log.Open("APGCC");

}

/*init*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_impl::init ( acs_apgcc_adminoperationasync *p_callBackImpl){


	SaAisErrorT errorCode;

	/*for callback*/
	omAdminOp = p_callBackImpl;



	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperation::init( )");
		trace.ACS_TRA_event(1, buffer);
	}

	version.releaseCode  = 'A';
	version.majorVersion =  2;
	version.minorVersion =  1;


	/*start*/
	SaImmCallbacksT immCallbacks;

	/** Set the SaImmCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/

	immCallbacks.saImmOmAdminOperationInvokeCallback = acs_apgcc_adminoperationasync_impl::objectManagerAdminOperationCallback;
	/*end*/

	/*init ImmHandle*/
	errorCode = saImmOmInitialize(&immHandle, &immCallbacks, &version);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperation::init():saImmOmInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

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

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	errorCode = saImmOmSelectionObjectGet(immHandle, &selObj);

	if( errorCode != SA_AIS_OK ){

		setInternalError(errorCode);

		char buffer[1024] = {0};
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationasync::init():saImmOiSelectionObjectGet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


		return ACS_CC_FAILURE;
	}

	/**1 Add implementer at the list**/
	//adminOpList[immHandle] = omAdminOp;
	objImp = omAdminOp;

	return ACS_CC_SUCCESS;

}

/*finalize*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_impl::finalize (){


	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationasync::finalize( )");
		trace.ACS_TRA_event(1, buffer);
	}

	/* release immHandle*/
	if((errorCode=saImmOmFinalize(immHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationasync::finalize():saImmOmFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	return ACS_CC_SUCCESS;

}
/* */
ACS_CC_ReturnType acs_apgcc_adminoperationasync_impl::adminOperationInvokeAsync ( ACS_APGCC_InvocationType invocation ,
		const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector){


	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationasync::adminOperationInvokeAsync( )");
		trace.ACS_TRA_event(1, buffer);
	}

	/*prepare the object name to be passed*/
	SaNameT objNameToSendOp = {0,{0}};

	objNameToSendOp.length = strlen (p_objName);

	memcpy(objNameToSendOp.value, p_objName, objNameToSendOp.length);

	/*Start Initialization Phase*/
		SaImmAdminOwnerHandleT ownerHandle;

		errorCode = saImmOmAdminOwnerInitialize(immHandle, const_cast<char*>(processName.c_str()),SA_TRUE, &ownerHandle);

		if (errorCode != SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperationasync::adminOperationInvokeAsync::saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		/*set the owner*/

		const SaNameT *objOwnedNameList[] = {&objNameToSendOp, 0};

		errorCode = saImmOmAdminOwnerSet (ownerHandle,objOwnedNameList, SA_IMM_ONE );
		if (errorCode != SA_AIS_OK)	{

			setInternalError(errorCode);

			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperationasync::adminOperationInvokeAsync():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[100];
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("acs_apgcc_adminoperationasync::adminOperationInvokeAsync():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);
			}
			/*end release ownerHandle*/


			return ACS_CC_FAILURE;
		}


		/*End Initialization Phase*/


		/*prepare the list of parameters*/

		SaImmAdminOperationParamsT_2 **operationParamList = 0;

		/*allocate memory for the list of ptr*/

		int numOfParam = paramVector.size();

		operationParamList = new SaImmAdminOperationParamsT_2*[numOfParam + 1];

		SaImmAdminOperationParamsT_2 singleParamVector[numOfParam];

		vector<ACS_APGCC_AdminOperationParamType>::iterator it;

		int i = 0;

		for (it = paramVector.begin(); it != paramVector.end(); it++){

			/*Set the attribute Name*/
			singleParamVector[i].paramName = reinterpret_cast<SaStringT>(((*it).attrName));

			/*Set param Type*/
			//singleParamVector[i].paramType = reinterpret_cast<SaImmValueTypeT>(((*it).attrType));

			singleParamVector[i].paramType = (SaImmValueTypeT)(((*it).attrType));
			/*start*/
			if ( (singleParamVector[i].paramType == SA_IMM_ATTR_SANAMET) || (singleParamVector[i].paramType == SA_IMM_ATTR_SASTRINGT)){
				singleParamVector[i].paramType = SA_IMM_ATTR_SASTRINGT;

				singleParamVector[i].paramBuffer = &(*it).attrValues;
			}else{
			/*end*/

				/*Set the param value*/
				singleParamVector[i].paramBuffer = (*it).attrValues;
			}

			/*insert the ptr in the list of param to be passed to IMM API*/
			operationParamList[i] = &singleParamVector[i];

			i++;

		}
		/*last element in the parameter list must be 0*/
		operationParamList[i] = 0;
		/*end prepare parameter list*/

		errorCode = saImmOmAdminOperationInvokeAsync_2(ownerHandle,
				(SaInvocationT) (invocation),
				&objNameToSendOp,
				(SaImmContinuationIdT) (continuationId),
				(SaImmAdminOperationIdT)(operationId),
				(const SaImmAdminOperationParamsT_2**)operationParamList

				);


		/*free allocated memory*/
		delete[] operationParamList;

		if (errorCode != SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperationasync::adminOperationInvokeAsync():saImmOmAdminOperationInvokeAsync_2:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);


			/* release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[100];
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("acs_apgcc_adminoperationasync::adminOperationInvokeAsync():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
				log.Write(buffer, LOG_LEVEL_ERROR);

				return ACS_CC_FAILURE;
			}

			return ACS_CC_FAILURE;

		}


		/*Finalization Phase Handle Start*/
		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperationasync::adminOperationInvokeAsync():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}
		/*Finalization Phase Handle End*/

/*1 manage callback */
		/**1 Add implementer at the list**/
		//TR FMS adminOpList[(SaInvocationT)invocation] = omAdminOp;
/*end manage callback*/

		return ACS_CC_SUCCESS;


}

/*int getInternalLastError();*/
int acs_apgcc_adminoperationasync_impl::getInternalLastError(){

	return ((-1)*errorCode);
}

/*char* getInternalLastErrorText();*/
char* acs_apgcc_adminoperationasync_impl::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}

/*setInternalError*/
void acs_apgcc_adminoperationasync_impl::setInternalError(int p_errorCode){

	errorCode = p_errorCode;

}

/*dispatch*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_impl::dispatch(ACS_APGCC_DispatchFlags p_flag){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationasync::dispatch(ACS_APGCC_DispatchFlags p_flag)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaAisErrorT errorCode;

	SaDispatchFlagsT flag = (SaDispatchFlagsT) p_flag;

	/** This function invokes pending callbacks for handle immOiHandle in the way
	 * that is specified by the flag parameter**/
	errorCode = saImmOmDispatch(immHandle, flag);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);

		char buffer[200];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationasync::dispatch()::saImmOiDispatch:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

}

/*get set obj*/
int acs_apgcc_adminoperationasync_impl::getSelObj() const {

	return (int)selObj;
}
/*objectManagerAdminOperationCallback*/
void acs_apgcc_adminoperationasync_impl::objectManagerAdminOperationCallback( SaInvocationT invocation,
			SaAisErrorT operationReturnValue,
			SaAisErrorT error){




	/**1 Find in the map the obj associated at the immHandle**/

	//TR FMS acs_apgcc_adminoperationasync *obj = (acs_apgcc_adminoperationasync *)(acs_apgcc_adminoperationasync_impl::adminOpList.find(invocation)->second);

	//TR FMS obj->objectManagerAdminOperationCallback( (ACS_APGCC_InvocationType)invocation, (int)operationReturnValue, (int)error );
	objImp->objectManagerAdminOperationCallback( (ACS_APGCC_InvocationType)invocation, (int)operationReturnValue, (int)error );

}



/*destructor*/
acs_apgcc_adminoperationasync_impl::~acs_apgcc_adminoperationasync_impl() {

	log.Close();
}
