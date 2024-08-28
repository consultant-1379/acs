/*
 * AdminOperation_MyImplementer.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

#include "AdminOperation_MyImplementer.h"
#include "acs_apgcc_omhandler.h"


int createObjectResult(OmHandler &omHandler, const char * rdnObject, const char *operationName );
int saveResult(OmHandler &omHandler, const char *objectName, const char *operation, int value);
int deleteObject(OmHandler &immHandler, const char * nomeObject);

AdminOperation_MyImplementer::AdminOperation_MyImplementer() {
	// TODO Auto-generated constructor stub

}


void AdminOperation_MyImplementer:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
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

	/*start*/
	cout << endl;

	int dim=0;
	int i=0;
	while(paramList[i]){
		i++;
		dim++;
	}

	vector<int> vettoreInteri;

	i = 0;
	while( paramList[i] ){
		switch ( paramList[i]->attrType ) {
		case ATTR_INT32T:{
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<int *>(paramList[i]->attrValues) << endl;
			vettoreInteri.push_back(*reinterpret_cast<int *>(paramList[i]->attrValues));
		}

			break;
		case ATTR_UINT32T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_FLOATT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<float *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_DOUBLET:
			cout << " paramListName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<double *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_NAMET:
		{
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;

		}
			break;
		case ATTR_STRINGT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;
			break;
		default:
			break;

		}
		i++;
	}

	cout << " Implementing Admin Operation with ID = " << operationId << endl;
	int retVal;

	if (vettoreInteri.size() == 2){/*se ci sono 2 interi fa operazioni*/

		switch ( operationId ){
				case 1:
				{

					char* nomeObject = const_cast<char *>("resultOperation,safApp=safImmService");

					cout << " SUM: "<< vettoreInteri.at(0) << " + " << vettoreInteri.at(1) <<endl;
					cout << " RESULT: "<< (vettoreInteri.at(1)+vettoreInteri.at(0)) << endl;
					cout <<endl;
					cout << " Save the result in IMM into object "<< nomeObject << endl;

					ACS_CC_ReturnType result;
					OmHandler omHandler;

					result = omHandler.Init();
					if ( result != ACS_CC_SUCCESS ){
						cout << "ERROR: Initialization FAILURE!!!\n";
					}


					int ret = createObjectResult( omHandler, "resultOperation", "sum" );
					if ( ret == 0 || ret == -14){
						//cout<<"Object created"<<endl;
					}else {
						cout<<"Object not created"<<endl;
						cout<<"ERROR CODE: "<<ret<<endl;
					}

					ret = saveResult(omHandler, nomeObject, "sum", (vettoreInteri.at(1)+vettoreInteri.at(0)));
					if ( ret == 0 || ret == -1){
						//cout<<"Result saved in IMM"<<endl;
					}else {
						cout<<"Impossible to save result in IMM"<<endl;
						cout<<"ERROR CODE: "<<ret<<endl;
					}

					result = omHandler.Finalize();
					if ( result != ACS_CC_SUCCESS ){
						cout << "ERROR: Finalize FAILURE!!!\n";
					}

					retVal = 1;

					this->adminOperationResult( oiHandle , invocation, retVal );



				}
				break;

				case 2:
				{

					char* nomeObject = const_cast<char *>("resultOperation,safApp=safImmService");

					cout << " SUB: "<< vettoreInteri.at(0) << " - " << vettoreInteri.at(1) <<endl;
					cout << " RESULT: "<< (vettoreInteri.at(0)-vettoreInteri.at(1)) << endl;
					cout <<endl;
					cout << " Save the result in IMM into object "<< nomeObject << endl;

					ACS_CC_ReturnType result;
					OmHandler omHandler;

					result = omHandler.Init();
					if ( result != ACS_CC_SUCCESS ){
						cout << "ERROR: Initialization FAILURE!!!\n";
					}


					int ret = createObjectResult( omHandler, "resultOperation", "subtraction" );
					if ( ret == 0 || ret == -14){
						//cout<<"Object created"<<endl;
					}else {
						cout<<"Object not created"<<endl;
						cout<<"ERROR CODE: "<<ret<<endl;
					}

					ret = saveResult(omHandler, nomeObject, "subtraction", (vettoreInteri.at(0)-vettoreInteri.at(1)));
					if ( ret == 0 || ret == -1){
						//cout<<"Result saved in IMM"<<endl;
					}else {
						cout<<"Impossible to save result in IMM"<<endl;
						cout<<"ERROR CODE: "<<ret<<endl;
					}

					result = omHandler.Finalize();
					if ( result != ACS_CC_SUCCESS ){
						cout << "ERROR: Finalize FAILURE!!!\n";
					}

					retVal = 1;
					this->adminOperationResult( oiHandle , invocation, retVal );
				}
				break;

				case 3:
				{

					char* nomeObject = const_cast<char *>("resultOperation,safApp=safImmService");

					cout << " MULT: "<< vettoreInteri.at(0) << " * " << vettoreInteri.at(1) <<endl;
					cout << " RESULT: "<< (vettoreInteri.at(1)*vettoreInteri.at(0)) << endl;
					cout <<endl;
					cout << " Save the result in IMM into object "<< nomeObject << endl;

					ACS_CC_ReturnType result;
					OmHandler omHandler;

					result = omHandler.Init();
					if ( result != ACS_CC_SUCCESS ){
						cout << "ERROR: Initialization FAILURE!!!\n";
					}


					int ret = createObjectResult( omHandler, "resultOperation", "multiplication" );
					if ( ret == 1 || ret == -14){
						//cout<<"Object created"<<endl;
					}else {
						cout<<"Object not created"<<endl;
						cout<<"ERROR CODE: "<<ret<<endl;
					}

					ret = saveResult(omHandler, nomeObject, "multiplication", (vettoreInteri.at(1)*vettoreInteri.at(0)));
					if ( ret == 0 || ret == -1){
						//cout<<"Result saved in IMM"<<endl;
					}else {
						cout<<"Impossible to save result in IMM"<<endl;
						cout<<"ERROR CODE: "<<ret<<endl;
					}

					result = omHandler.Finalize();
					if ( result != ACS_CC_SUCCESS ){
						cout << "ERROR: Finalize FAILURE!!!\n";
					}

					retVal = 1;
					this->adminOperationResult( oiHandle , invocation, retVal );
				}
				break;

//				case 5:
//				{
//					cout << " CASE OF TIMEOUT opId: " << operationId << endl;
//					sleep(10);
//					retVal = 1;
//					this->adminOperationResult( oiHandle , invocation, retVal );
//				}
//				break;
				default:{
					cout << " .... : " << operationId << endl;
					retVal = 28;
					this->adminOperationResult( oiHandle , invocation, retVal );

				}
					break;
			}
		return;

	}else if (vettoreInteri.size() == 1){

		cout << " Number of parameter for operationId " << operationId <<" wrong"<<endl;
		retVal = 20;
		this->adminOperationResult( oiHandle , invocation, retVal );

	}else if (vettoreInteri.size() == 0 && ( (operationId == 4) || (operationId == 5) ) ){


		switch(operationId){
		case 4:
		{
			char* nomeObject = const_cast<char *>("resultOperation,safApp=safImmService");

			cout << " Remove object "<< nomeObject << " from IMM" <<endl;

			ACS_CC_ReturnType result;
			OmHandler omHandler;

			result = omHandler.Init();
			if ( result != ACS_CC_SUCCESS ){
				cout << "ERROR: Initialization FAILURE!!!\n";
			}

			//cout<<"chiamo la deleteObject"<<endl;
			int ret = deleteObject( omHandler, nomeObject );
			if ( ret == -1 || ret == 0 ){
				cout<<"Object deleted"<<endl;
			}else {
				cout<<"Object not deleted"<<endl;
				cout<<"ERROR CODE: "<<ret<<endl;
			}

			result = omHandler.Finalize();
			if ( result != ACS_CC_SUCCESS ){
				cout << "ERROR: Finalize FAILURE!!!\n";
			}

			retVal = 1;
			this->adminOperationResult( oiHandle , invocation, retVal );
		}
		break;

		case 5:
		{
			cout << "CASE OF TIMEOUT opId: " << operationId << endl;
			cout << "attend 10 seconds "<< endl;
			for (int i = 0; i<10; i++){
				cout<<".";
				cout.flush();
				sleep(1);
			}
			cout<<endl;

			retVal = 1;
			this->adminOperationResult( oiHandle , invocation, retVal );
		}
		break;

		default:
		{
			cout << " .... : " << operationId << endl;
			retVal = 28;
			this->adminOperationResult( oiHandle , invocation, retVal );

		}
		break;
		}//end switch

	}else if (vettoreInteri.size() == 0 && ( (operationId == 1) || (operationId == 2) ||  (operationId == 3)) ){/* se non ci sono due interi ritorna successo dopo 11 secondi*/

		cout << " Number of parameter for operationId " << operationId <<" wrong"<<endl;
		retVal = 20;
		this->adminOperationResult( oiHandle , invocation, retVal );
		return;
		retVal = 1;
//		sleep (11);
//		cout << " No arithmetical operation to perform RETURNING after 11 sec.... " << retVal << endl;
//		this->adminOperationResult( oiHandle , invocation, retVal );
//		return;
	}else if (operationId > 5){
		cout << " No action defined for operationId: " << operationId << endl;
		retVal = 28;
		this->adminOperationResult( oiHandle , invocation, retVal );

	}

}



/***************crea oggetto*/
int createObjectResult(OmHandler &omHandler, const char * rdnObject, const char *operationName ){


	ACS_CC_ReturnType result;

	char nameClassResult[20] = "Test_Result";
	char* nomeParent = const_cast<char *>("safApp=safImmService");

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr4;

	/*Fill the rdn Attribute */
	char attrdn[]= "RDN_Attribute";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_NAMET;
	attrRdn.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdnObject);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attrRdn.attrValues = value;


	/*Fill the ATTRIBUTE_3 */
	char name_attInt[]="operationResult";
	attr2.attrName = name_attInt;
	attr2.attrType = ATTR_INT32T;
	attr2.attrValuesNum = 1;
	int intValue = 0;
	void* valueInt[1]={ reinterpret_cast<void*>(&intValue)};
	attr2.attrValues = valueInt;


	char name_attrSaString[]= "operationName";
	attr4.attrName = name_attrSaString;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 1;
	char* strValue = const_cast<char*>(operationName);
	void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
	attr4.attrValues = valueStr;


	AttrList.push_back(attrRdn);
	AttrList.push_back(attr2);
	AttrList.push_back(attr4);

	result = omHandler.createObject(nameClassResult, nomeParent, AttrList );

	if( result == ACS_CC_SUCCESS ){
		//cout << " -> Object  " << p_objName << "," << nomeParent << " created" << endl;
		//cout << endl;
		return omHandler.getInternalLastError();
	}else {
		//cout << " -> ERROR !!! Object  " << p_objName << "," << nomeParent << " NOT created errCode= " << omHandler_NO_OI.getInternalLastError()<< endl;
		//cout << endl;
		return omHandler.getInternalLastError();
	}

}
/***************end crea oggetto*/


/**************1 modify object*/
int saveResult(OmHandler &omHandler,  const char *objectName, const char *operation, int value){

	ACS_CC_ReturnType result;
	//char* objectName = const_cast<char *>("result2,result,provaConfObj1,safApp=safImmService");

	int valueInt = value;

	char name_attInt[]="operationResult";


	ACS_CC_ImmParameter par;
	par.attrName = name_attInt;
	par.attrType = ATTR_INT32T;
	par.attrValuesNum = 1;
	void* new_valuesInt32[1] = {reinterpret_cast<void*>(&valueInt)};
	par.attrValues = new_valuesInt32;


	/*Set a new value for ATT_STRING*/
	ACS_CC_ImmParameter par2;
	par2.attrName=const_cast<char*>("operationName");
	par2.attrType=ATTR_STRINGT;
	par2.attrValuesNum=1;


	void* new_valueString[1]={reinterpret_cast<void*>(const_cast<char*>(operation))};
	par2.attrValues = new_valueString;


	if ((result = omHandler.modifyAttribute(objectName, &par) )!=ACS_CC_SUCCESS){
		cout << " ERROR modify par1" << endl;
		cout << endl;
		return omHandler.getInternalLastError();
	}

	if ((result = omHandler.modifyAttribute(objectName, &par2) )!=ACS_CC_SUCCESS){
		cout << " ERROR modify par2" << endl;
		cout << endl;
		return omHandler.getInternalLastError();
	}

	return 0;


}
/**************end modify object*/




int deleteObject(OmHandler &immHandler, const char * nomeObject){

	ACS_CC_ReturnType returnCode;

	returnCode=immHandler.deleteObject(nomeObject) ;
	if(returnCode == ACS_CC_SUCCESS){
		//printf("Cancellazione oggetto %s eseguita\n", nomeObject);
		return immHandler.getInternalLastError();
	}else{
		printf("Cancellazione oggetto %s fallita\n", nomeObject);
		return immHandler.getInternalLastError();
	}


}
