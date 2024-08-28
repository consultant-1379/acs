/*
 * MyImplementer.cpp
 *
 *  Created on: May 23, 2012
 *      Author: egimarr
 */

#include "MyImplementer.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "acs_bur_BrfcAdminOperationId.h"
#include <iostream>
using namespace std;

MyImplementer::MyImplementer(){
	m_status = BRF_SUCCESS;
}

MyImplementer::MyImplementer(string p_impName ):acs_apgcc_objectimplementerinterface_V3(p_impName){
	m_status = BRF_SUCCESS;
}

MyImplementer::MyImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ): acs_apgcc_objectimplementerinterface_V3(p_objName, p_impName, p_scope){
	m_status = BRF_SUCCESS;
}

int MyImplementer::getLastMessageReceived(){
	return lastMessageReceived;
}

int MyImplementer::ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode){

	//std::cout << "Preparing response to  BrfParticipantContainer" << endl;
	acs_apgcc_adminoperation admOp;
	ACS_CC_ReturnType result1;


	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;

	ACS_APGCC_AdminOperationParamType firstElem;

	ACS_APGCC_AdminOperationParamType secondElem;

	if(responseCode == BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT){

		ACS_APGCC_AdminOperationParamType thirdElem;

		//create first Element of parameter list

		char attName1[]= "requestId";
		firstElem.attrName =attName1;
		firstElem.attrType=ATTR_UINT64T;

		// return requestId previously checked

		unsigned long long intValue1 = requestId;
		//cout << "Prepare parameter 1 requestId = "<< intValue1 << endl;

		firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

		// create second Element of parameter list
		char attName2[]= "resultCode";
		secondElem.attrName =attName2;
		secondElem.attrType=ATTR_INT32T ;


		int intValue2 = brfStatus; // BRF_SUCCESS

		//cout << "Prepare parameter 2 resultCode = "<< intValue2 << endl;

		secondElem.attrValues=reinterpret_cast<void*>(&intValue2);

		//create the third Element of parameter list
		char attName3[]= "message";
		thirdElem.attrName =attName3;
		thirdElem.attrType=ATTR_STRINGT ;

		char* strValue = const_cast<char*>("");
		void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
		thirdElem.attrValues=valueStr;
		//cout << "Prepare parameter 3 message = "<< valueStr << endl;

		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);
		vectorIN.push_back(thirdElem);

	}
	else if(responseCode == BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS){

		//create first Element of parameter list

		char attName1[]= "requestId";
		firstElem.attrName =attName1;
		firstElem.attrType=ATTR_UINT64T;
		unsigned long long intValue1 = requestId;
		firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

		// create second Element of parameter list
		char attName3[]= "progressCount";
		secondElem.attrName =attName3;
		secondElem.attrType=ATTR_UINT32T ;

		int intValue3 = 100; // 100 %

		secondElem.attrValues=reinterpret_cast<void*>(&intValue3);
		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);

	}

	const char *dnObjName1 = "brfParticipantContainerId=1";
	long long int timeOutVal_30sec = 30*(1000000000LL);

	int res1 = admOp.init();

	if (res1 != ACS_CC_SUCCESS){
		std::cout <<"ERROR ::admOp.init()FAILED"<< std::endl;
		std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;

		return -1;
	}

	int retry = 0;
	int returnValue1 = 1;
	// BRF_PARTECIPANT_CONTAINER__REPORT_ACTION_RESULT 22
	// Shipment 5
	// #define BRF_PARTECIPANT_CONTAINER__REPORT_ACTION_PROGRESS 21
	// #define BRF_PARTECIPANT_CONTAINER__REPORT_ACTION_RESULT 22

	//Invoke the action reportActionResult or reportActionProgress on the brfParticipantContainerId=1 MO.
	//A retry mechanism is implemented in order to solve the problem of BrfParticipant concurrency.
	//In case of SA_AIS_ERR_EXIST or SA_AIS_ERR_BAD_OPERATION a concurrent BrfParticipant
	//acquired the Administrative Ownership of the brfParticipantContainerId=1 MO.
	//Retry attempts are made each second in a time slot of one minute.
	//If no attempts succeed in this period BRF send a cancel backup request

		while (admOp.adminOperationInvoke(dnObjName1, 0 , responseCode , vectorIN , &returnValue1, timeOutVal_30sec) == ACS_CC_FAILURE)
			{
				std::cout <<"ERROR ::admOp.adminOperationInvoke()FAILED"<< std::endl;
				std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;

				if ((admOp.getInternalLastError()!=((-1)*SA_AIS_ERR_EXIST)) && (admOp.getInternalLastError()!=((-1)*SA_AIS_ERR_BAD_OPERATION))) break;

				sleep(1);
				if (++retry > 60) break;
			}

	// acs_apgcc_adminoperation object finalize.
	res1 = admOp.finalize();

		if (res1 != ACS_CC_SUCCESS)
			{

		    std::cout <<"ERROR ::admOp.finalize()FAILED"<< std::endl;
		    std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
		    return -1;

			}

	return 0;
}

void MyImplementer:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
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


	switch (operationId){
	case BRF_PARTICIPANT__PERMIT_BACKUP:

		cout << "RECV: BRF_PARTICIPANT__PERMIT_BACKUP " <<endl;

		break;
	case BRF_PARTICIPANT__COMMIT_BACKUP:

		cout << "RECV: BRF_PARTICIPANT__COMMIT_BACKUP"<<endl;
		break;
	case BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP:

		cout << "RECV: BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP " <<endl;

		break;
	case BRF_PARTICIPANT__CANCEL_BACKUP:

		cout << "RECV: BRF_PARTICIPANT__CANCEL_BACKUP" <<endl;
		break;

	}
	/*start*/
	cout << endl;

	int dim=0;
	int i=0;
	while(paramList[i]){
		i++;
		dim++;
	}
	unsigned long long requestId;
	i = 0;
	while( paramList[i] ){
		switch ( paramList[i]->attrType ) {
		case ATTR_UINT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(paramList[i]->attrValues) << endl;
			if(strcmp(paramList[i]->attrName,"requestId")==0){
				requestId = *reinterpret_cast<unsigned long long *>(paramList[i]->attrValues);
				cout << "requestId assigned " << requestId << endl;
			}
			break;
		default:
			break;

		}
		i++;
	}

	cout << " Received Operation with Identification = " << operationId << endl;
	int retVal =0;

	retVal = 1;
	int retAdminOperationResult = this->adminOperationResult( oiHandle , invocation, retVal );

	cout << "Called adminOperationResult with retVal = " << retVal << " returned = " << retAdminOperationResult << endl;

	lastMessageReceived = operationId;

	retVal = this->ResponseToBrfc(requestId, m_status , BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);

	/*end*/

}


ACS_CC_ReturnType MyImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType MyImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType MyImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType MyImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){


	return ACS_CC_SUCCESS;

}

void MyImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

}

void MyImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

}


ACS_CC_ReturnType MyImplementer::updateRuntime(const char* p_objName, const char** p_attrName){
	return ACS_CC_SUCCESS;
}
/* */
void MyImplementer::setStatus(int status){
	m_status = status;
}



