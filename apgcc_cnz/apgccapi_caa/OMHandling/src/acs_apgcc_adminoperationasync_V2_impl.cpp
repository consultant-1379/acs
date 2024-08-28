/*
 * acs_apgcc_adminoperationasync_V2_impl.cpp
 *
 *  Created on: Nov 15, 2011
 *      Author: xpaomaz
 */



#include "acs_apgcc_adminoperationasync_V2_R1_impl.h"
#include "acs_apgcc_adminoperationasync_V2_R1.h"


#include <iostream>
#include "ACS_TRA_trace.h"
#include "acs_apgcc_error.h"

#include "ACS_APGCC_Util.H"

namespace {
	ACS_TRA_trace trace(const_cast<char*>("acs_apgcc_adminoperationasync_V2"), const_cast<char *>("C300"));
}

/*1**/

map<SaUint64T,acs_apgcc_adminoperationasync_V2_R1 *> acs_apgcc_adminoperationasync_V2_R1_impl::adminOpList;


acs_apgcc_adminoperationasync_V2_R1* objImp_V2= 0;

acs_apgcc_adminoperationasync_V2_R1_impl::acs_apgcc_adminoperationasync_V2_R1_impl() {

	immHandle = 0;

	ownerHandle = 0;

	errorCode = 0;

	selObj = 0;

	log.Open("APGCC");

}

acs_apgcc_adminoperationasync_V2_R1_impl::acs_apgcc_adminoperationasync_V2_R1_impl(acs_apgcc_adminoperationasync_V2_R1_impl* obj){

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
ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1_impl::init ( acs_apgcc_adminoperationasync_V2_R1 *p_callBackImpl){


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
	//version.minorVersion =  1;
	//version.minorVersion =  11;
	version.minorVersion = 14; // Support IMM callback timeout


	/*start*/
	//SaImmCallbacksT immCallbacks;
	SaImmCallbacksT_o2 immCallbacks;

	/** Set the SaImmCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/

	//immCallbacks.saImmOmAdminOperationInvokeCallback = acs_apgcc_adminoperationasync_V2_impl::objectManagerAdminOperationCallback;
	immCallbacks.saImmOmAdminOperationInvokeCallback = acs_apgcc_adminoperationasync_V2_R1_impl::objectManagerAdminOperationCallback;

	/*end*/

	/*init ImmHandle*/
	//errorCode = saImmOmInitialize(&immHandle, &immCallbacks, &version);
	errorCode = saImmOmInitialize_o2(&immHandle, &immCallbacks, &version);

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[300];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperation::init():saImmOmInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/**The process name will be used as name of Administrative OWner*/
	char pName[1024];
	::memset(pName, 0, 1024);
	ACS_APGCC::getProcessName(pName);
	if(pName != 0){
		processName = pName;
	}else{
		processName = "unknownProcess";
	}

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	errorCode = saImmOmSelectionObjectGet(immHandle, &selObj);

	if( errorCode != SA_AIS_OK ){

		setInternalError(errorCode);

		char buffer[300];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationasync_V2::init():saImmOiSelectionObjectGet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


		return ACS_CC_FAILURE;
	}

	/**1 Add implementer at the list**/
	//adminOpList[1] = omAdminOp;
	objImp_V2 = omAdminOp;

	return ACS_CC_SUCCESS;

}

/*finalize*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1_impl::finalize (){


	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationasync_V2::finalize( )");
		trace.ACS_TRA_event(1, buffer);
	}

	/* release immHandle*/
	if((errorCode=saImmOmFinalize(immHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperationasync_V2::finalize():saImmOmFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	return ACS_CC_SUCCESS;

}
/* */
ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1_impl::adminOperationInvokeAsync ( ACS_APGCC_InvocationType invocation ,
		const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector){


	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationasync_V2::adminOperationInvokeAsync( )");
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

			log.Write("acs_apgcc_adminoperationasync_V2::adminOperationInvokeAsync::saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
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

			log.Write("acs_apgcc_adminoperationasync_V2::adminOperationInvokeAsync():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			/*release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[100];
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("acs_apgcc_adminoperationasync_V2::adminOperationInvokeAsync():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
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

			log.Write("acs_apgcc_adminoperationasync_V2::adminOperationInvokeAsync():saImmOmAdminOperationInvokeAsync_2:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);


			/* release ownerHandle*/
			if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

				setInternalError(errorCode);
				char buffer[100];
				sprintf(buffer, "ERROR CODE %d", errorCode);

				log.Write("acs_apgcc_adminoperationasync_V2::adminOperationInvokeAsync():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
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

			log.Write("acs_apgcc_adminoperationasync_V2::adminOperationInvokeAsync():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
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
int acs_apgcc_adminoperationasync_V2_R1_impl::getInternalLastError(){

	return ((-1)*errorCode);
}

/*char* getInternalLastErrorText();*/
char* acs_apgcc_adminoperationasync_V2_R1_impl::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}

/*setInternalError*/
void acs_apgcc_adminoperationasync_V2_R1_impl::setInternalError(int p_errorCode){

	errorCode = p_errorCode;

}

/*dispatch*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1_impl::dispatch(ACS_APGCC_DispatchFlags p_flag){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperationasync_V2::dispatch(ACS_APGCC_DispatchFlags p_flag)");
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

		log.Write("acs_apgcc_adminoperationasync_V2::dispatch()::saImmOiDispatch:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	return ACS_CC_SUCCESS;

}

/*get set obj*/
int acs_apgcc_adminoperationasync_V2_R1_impl::getSelObj() const {

	return (int)selObj;
}
/*objectManagerAdminOperationCallback*/
void acs_apgcc_adminoperationasync_V2_R1_impl::objectManagerAdminOperationCallback( SaInvocationT invocation,
			SaAisErrorT operationReturnValue,
			SaAisErrorT error,
			const SaImmAdminOperationParamsT_2 **outParamVector){




	/**1 prepare to call the related callback**/
	/**Calculate the size of array of attribute descriptor**/

	if ( !outParamVector )
		objImp_V2->objectManagerAdminOperationCallback( (ACS_APGCC_InvocationType)invocation, (int)operationReturnValue, (int)error, 0 );
	else {
		int dim = 0;
		int i = 0;

		while(outParamVector[i]){
			i++;
			dim++;
		}
		/*Prepares data to be returned to the OM administrative operation implementer*/

		ACS_APGCC_AdminOperationParamType *myOutParamList[dim+1];
		ACS_APGCC_AdminOperationParamType oneAttr[dim];

		i=0;
		while(outParamVector[i]){

			oneAttr[i].attrName = outParamVector[i]->paramName;
			oneAttr[i].attrType = (ACS_CC_AttrValueType)(outParamVector[i]->paramType);
			oneAttr[i].attrValues = new void *[1];

			/*only SA_IMM_ATTR_SASTRINGT is rreturned by the return oeration by APG implementer*/
			if((outParamVector[i]->paramType == SA_IMM_ATTR_SASTRINGT)  ){

					SaStringT stringVal = *(reinterpret_cast<SaStringT *>(outParamVector[i]->paramBuffer));
					oneAttr[i].attrValues = reinterpret_cast<char*>(stringVal);


				}else if (outParamVector[i]->paramType  == SA_IMM_ATTR_SANAMET){

						SaNameT* localSaName;
						localSaName = reinterpret_cast<SaNameT*>(outParamVector[i]->paramBuffer);
						oneAttr[i].attrValues = localSaName->value;


			}else {
				//for (unsigned int j = 0; j <  outParamVector[i]->attrValuesNumber; j++){
					oneAttr[i].attrValues = outParamVector[i]->paramBuffer;
				//}
			}

			myOutParamList[i] = &(oneAttr[i]);
			i++;
		}

		/**the myOutParamList must be a null terminated array**/
		myOutParamList[dim] = 0;

		/*now call the callback registered*/
		objImp_V2->objectManagerAdminOperationCallback( (ACS_APGCC_InvocationType)invocation, (int)operationReturnValue, (int)error, myOutParamList );
	}

//	for(int i=0; i<dim; i++){
//		delete[] oneAttr[i].attrValues;
//	}
}



/*destructor*/
acs_apgcc_adminoperationasync_V2_R1_impl::~acs_apgcc_adminoperationasync_V2_R1_impl() {

	log.Close();
}
