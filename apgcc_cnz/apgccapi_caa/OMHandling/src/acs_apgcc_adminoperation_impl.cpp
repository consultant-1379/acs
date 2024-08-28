/*
 * acs_apgcc_adminoperation_impl.cpp
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_adminoperation_impl.h"
#include <iostream>
#include "ACS_TRA_trace.h"
#include "acs_apgcc_error.h"

#include "ACS_APGCC_Util.H"

namespace {
	ACS_TRA_trace trace(const_cast<char*>("acs_apgcc_adminoperation"), const_cast<char *>("C300"));
}

/*default constructor*/
acs_apgcc_adminoperation_impl::acs_apgcc_adminoperation_impl() {

	immHandle = 0;

	ownerHandle = 0;

	errorCode = 0;

	log.Open("APGCC");


}

/*constructor*/
acs_apgcc_adminoperation_impl::acs_apgcc_adminoperation_impl(acs_apgcc_adminoperation_impl *adminOpImp){

	this->immHandle = adminOpImp->immHandle;

	this->ownerHandle = adminOpImp->ownerHandle;

	this->errorCode = adminOpImp->errorCode;

	this->version.releaseCode  = adminOpImp->version.releaseCode;

	this->version.majorVersion =  adminOpImp->version.majorVersion;

	this->version.minorVersion =  adminOpImp->version.minorVersion;

	log.Open("APGCC");

}


acs_apgcc_adminoperation_impl::~acs_apgcc_adminoperation_impl() {

	log.Close();
}


/* init ( )*/
ACS_CC_ReturnType acs_apgcc_adminoperation_impl::init ( ){
	/*tbd*/
	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperation::init( )");
		trace.ACS_TRA_event(1, buffer);
	}

	version.releaseCode  = 'A';
	version.majorVersion =  2;
	//version.minorVersion =  11;
	version.minorVersion = 14; // Support IMM callback timeout

	/*init ImmHandle*/
	errorCode = saImmOmInitialize(&immHandle, 0, &version);

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

	/*end to be defined ok */
	return ACS_CC_SUCCESS;

}

/* finalize ();*/
ACS_CC_ReturnType acs_apgcc_adminoperation_impl::finalize  ( ){
	/*tbd*/
	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperation::finalize( )");
		trace.ACS_TRA_event(1, buffer);
	}

//	/* release ownerHandle*/
//	if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){
//
//		setInternalError(errorCode);
//		char buffer[100];
//		sprintf(buffer, "ERROR CODE %d", errorCode);
//
//		log.Write("acs_apgcc_adminoperation::finalize():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
//		log.Write(buffer, LOG_LEVEL_ERROR);
//
//		return ACS_CC_FAILURE;
//	}

	/* release immHandle*/
	if((errorCode=saImmOmFinalize(immHandle))!=SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperation::finalize():saImmOmFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}


	return ACS_CC_SUCCESS;

}

/*adminOperationInvoke*/
ACS_CC_ReturnType acs_apgcc_adminoperation_impl::adminOperationInvoke ( const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector,
		int* returnVal,
		long long int timeoutVal ){

	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperation::adminOperationInvoke( )");
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

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/*set the owner*/
//	SaNameT objOwnedName = {0,{0}};
	const SaNameT *objOwnedNameList[] = {&objNameToSendOp, 0};


	errorCode = saImmOmAdminOwnerSet (ownerHandle,objOwnedNameList, SA_IMM_ONE );
	if (errorCode != SA_AIS_OK)	{

		setInternalError(errorCode);

		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperation::Init():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);
		}
		/*end release ownerHandle*/


		return ACS_CC_FAILURE;
	}


	/*End Initialization Phase*/






	/*tbd*/


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
		operationParamList[i] = &(singleParamVector[i]);

		i++;

	}
	/*last element in the parameter list must be 0*/
	operationParamList[i] = 0;



	errorCode = saImmOmAdminOperationInvoke_2(ownerHandle, &objNameToSendOp,
			(SaImmContinuationIdT) (continuationId),
			(SaImmAdminOperationIdT)(operationId),
			(const SaImmAdminOperationParamsT_2**)operationParamList,
			(SaAisErrorT*)returnVal,
			(SaTimeT)(timeoutVal)
			);



	/*free allocated memory*/
	delete[] operationParamList;

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOperationInvoke_2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);


		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
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

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}






	/*Finalization Phase Handle End*/



	/*endtbd*/
	return ACS_CC_SUCCESS;
}

/*int getInternalLastError();*/
int acs_apgcc_adminoperation_impl::getInternalLastError(){

	return ((-1)*errorCode);
}

/*char* getInternalLastErrorText();*/
char* acs_apgcc_adminoperation_impl::getInternalLastErrorText(){

	if(errorCode > 0 && errorCode < ACS_APGCC_MAX_ERROR_NUMBER){
		return errorArray[errorCode];
	}

	return 0;
}

/*setInternalError*/
void acs_apgcc_adminoperation_impl::setInternalError(int p_errorCode){

	errorCode = p_errorCode;

}

ACS_CC_ReturnType acs_apgcc_adminoperation_impl::adminOperationInvoke ( const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector,
		int* returnVal,
		long long int timeoutVal,
		vector<ACS_APGCC_AdminOperationParamType> &outparamVector){


	SaAisErrorT errorCode;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_adminoperation::adminOperationInvoke( )");
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

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerInitialize:FAILED", LOG_LEVEL_ERROR);
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

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerSet:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/*release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperation::Init():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
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

		singleParamVector[i].paramType = (SaImmValueTypeT)(((*it).attrType));

		if ( (singleParamVector[i].paramType == SA_IMM_ATTR_SANAMET) || (singleParamVector[i].paramType == SA_IMM_ATTR_SASTRINGT)){
			singleParamVector[i].paramType = SA_IMM_ATTR_SASTRINGT;
			singleParamVector[i].paramBuffer = &(*it).attrValues;
		}else{
			/*Set the param value*/
			singleParamVector[i].paramBuffer = (*it).attrValues;
		}

		/*insert the ptr in the list of param to be passed to IMM API*/
		operationParamList[i] = &(singleParamVector[i]);

		i++;
	}

	/*last element in the parameter list must be 0*/
	operationParamList[i] = 0;

	SaImmAdminOperationParamsT_2 **outParamList = 0;

	errorCode = saImmOmAdminOperationInvoke_o2(ownerHandle, &objNameToSendOp,
			(SaImmContinuationIdT) (continuationId),
			(SaImmAdminOperationIdT)(operationId),
			(const SaImmAdminOperationParamsT_2**)operationParamList,
			(SaAisErrorT*)returnVal,
			(SaTimeT)(timeoutVal),
			&outParamList
			);

	/*free allocated memory*/
	delete[] operationParamList;

	if (errorCode != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOperationInvoke_o2:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		/* release ownerHandle*/
		if((errorCode=saImmOmAdminOwnerFinalize(ownerHandle))!=SA_AIS_OK){

			setInternalError(errorCode);
			char buffer[100];
			sprintf(buffer, "ERROR CODE %d", errorCode);

			log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
			log.Write(buffer, LOG_LEVEL_ERROR);

			return ACS_CC_FAILURE;
		}

		return ACS_CC_FAILURE;
	}

	ACS_APGCC_AdminOperationParamType tmp_param;

	if (outParamList !=0){

		i = 0;

		while ( outParamList[i] ){

			outparamVector.push_back ( tmp_param );

			outparamVector[i].attrName = reinterpret_cast<char*>(outParamList[i]->paramName);
			outparamVector[i].attrType = (ACS_CC_AttrValueType)outParamList[i]->paramType;


			switch ( outparamVector[i].attrType ) {
				case ATTR_INT32T :
					outparamVector[i].attrValues = new SaInt32T;
					*(reinterpret_cast<SaInt32T*>(outparamVector[i].attrValues)) = *reinterpret_cast<SaInt32T*>(outParamList[i]->paramBuffer);
					break;
				case ATTR_UINT32T :
					outparamVector[i].attrValues = new SaUint32T;
					*(reinterpret_cast<SaUint32T*>(outparamVector[i].attrValues)) = *reinterpret_cast<SaUint32T*>(outParamList[i]->paramBuffer);
					break;
				case ATTR_INT64T :
					outparamVector[i].attrValues = new SaInt64T;
					*(reinterpret_cast<SaInt64T*>(outparamVector[i].attrValues)) = *reinterpret_cast<SaInt64T*>(outParamList[i]->paramBuffer);
					break;
				case ATTR_UINT64T :
					outparamVector[i].attrValues = new SaUint64T;
					*(reinterpret_cast<SaUint64T*>(outparamVector[i].attrValues)) = *reinterpret_cast<SaUint64T*>(outParamList[i]->paramBuffer);
					break;
				case ATTR_TIMET :
					outparamVector[i].attrValues = new SaTimeT;
					*(reinterpret_cast<SaTimeT*>(outparamVector[i].attrValues)) = *reinterpret_cast<SaTimeT*>(outParamList[i]->paramBuffer);
					break;
				case ATTR_NAMET : {/*manage SA_NAMET received by Implemeter not using apgcc api*/
					SaNameT* localSaName;
					localSaName = reinterpret_cast<SaNameT*>(outParamList[i]->paramBuffer);
					char *nameObj = new char[(localSaName->length) + 1];
					memset( nameObj, '\0', (localSaName->length) +1 );
					memcpy(nameObj, localSaName->value, localSaName->length );
					outparamVector[i].attrValues = nameObj;

				}
					break;
				case ATTR_STRINGT :{
					//outparamVector[i].attrValues = strdup ( reinterpret_cast<const char*>(outParamList[i]->paramBuffer) );

					char *valPr = strdup (reinterpret_cast<const char*>(*reinterpret_cast<SaStringT*>(outParamList[i]->paramBuffer)));
					outparamVector[i].attrValues = valPr;
						}
					break;
				case ATTR_FLOATT :
					outparamVector[i].attrValues = new SaFloatT;
					*(reinterpret_cast<SaFloatT*>(outparamVector[i].attrValues)) = *reinterpret_cast<SaFloatT*>(outParamList[i]->paramBuffer);
					break;
				case ATTR_DOUBLET :
					outparamVector[i].attrValues = new SaDoubleT;
					*(reinterpret_cast<SaDoubleT*>(outparamVector[i].attrValues)) = *reinterpret_cast<SaDoubleT*>(outParamList[i]->paramBuffer);
					break;

				default :
					break;

			}

			i++;

		}

	}else /**no param received from invoke*/
		outparamVector.clear();


	/*Finalization Phase Handle Start*/

	/* release ownerHandle*/
	if( ( errorCode = saImmOmAdminOwnerFinalize ( ownerHandle ) ) != SA_AIS_OK){

		setInternalError(errorCode);
		char buffer[100];
		sprintf(buffer, "ERROR CODE %d", errorCode);

		log.Write("acs_apgcc_adminoperation::adminOperationInvoke():saImmOmAdminOwnerFinalize:FAILED", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);

		return ACS_CC_FAILURE;
	}

	/*Finalization Phase Handle End*/

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_apgcc_adminoperation_impl::freeadminOperationReturnValue ( vector<ACS_APGCC_AdminOperationParamType> &outparamVector ){

	vector<ACS_APGCC_AdminOperationParamType>::iterator it;

	for ( it = outparamVector.begin() ; it < outparamVector.end(); it++ ){
		delete[] ((*it).attrName); // char*
		switch ( (*it).attrType ) {
			case ATTR_INT32T :
				delete reinterpret_cast<SaInt32T*>((*it).attrValues); // void*
				break;
			case ATTR_UINT32T :
				delete reinterpret_cast<SaUint32T*>((*it).attrValues); // void*
				break;
			case ATTR_INT64T :
				delete reinterpret_cast<SaInt64T*>((*it).attrValues); // void*
				break;
			case ATTR_UINT64T :
				delete reinterpret_cast<SaUint64T*>((*it).attrValues); // void*
				break;
			case ATTR_TIMET :
				delete reinterpret_cast<SaTimeT*>((*it).attrValues); // void*
				break;
			case ATTR_NAMET :
				//delete reinterpret_cast<SaNameT*>((*it).attrValues); // void*
				delete[] reinterpret_cast<char*>((*it).attrValues); // void*
				break;
			case ATTR_FLOATT :
				delete reinterpret_cast<SaFloatT*>((*it).attrValues); // void*
				break;
			case ATTR_DOUBLET :
				delete reinterpret_cast<SaDoubleT*>((*it).attrValues); // void*
				break;
			case ATTR_STRINGT :
				delete[] reinterpret_cast<char*>((*it).attrValues); // void*
				break;

			default :
				break;

		}
	}

	outparamVector.clear();

	return ACS_CC_SUCCESS;

}

