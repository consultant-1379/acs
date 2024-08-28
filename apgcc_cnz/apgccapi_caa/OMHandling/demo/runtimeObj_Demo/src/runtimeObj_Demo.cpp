//============================================================================
// Name        : runtimeObj_Demo.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sys/poll.h>

#include "ace/Task.h"

#include "OmHandler.h"
#include "ACS_CC_Types.h"
#include "runtimeObjectHandler.h"

using namespace std;

/*******************Name of Attributes *************************************************/

char attrdn[] 				= 	"RDN_Attribute";
char name_attrFloatC[] 		= 	"ATTR_FLOAT_C";
char name_attrInt64C[]		= 	"ATTR_INT64_C";
char name_attrInt32C[] 		= 	"ATTR_INT32_C";
char name_attrDoubleC[] 	= 	"ATTR_DOUBLE_C";
char name_attrUnsInt64C[]	= 	"ATTR_UNSI64_C";
char name_attrUnsInt32C[] 	= 	"ATTR_UNSI32_C";
char name_attrSaNameC[] 	= 	"ATTR_NAMET_C";
char name_attrStringC[] 	= 	"ATTR_STRING_C";

char name_attrFloatNC[] 	= 	"ATTR_FLOAT_NC";
char name_attrInt64NC[]		= 	"ATTR_INT64_NC";
char name_attrInt32NC[] 	= 	"ATTR_INT32_NC";
char name_attrDoubleNC[] 	= 	"ATTR_DOUBLE_NC";
char name_attrUnsInt64NC[]	= 	"ATTR_UNSI64_NC";
char name_attrUnsInt32NC[] 	= 	"ATTR_UNSI32_NC";
char name_attrSaNameNC[] 	= 	"ATTR_NAMET_NC";
char name_attrStringNC[] 	= 	"ATTR_STRING_NC";

/***************************************************************************************/



class MyThread : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThread(RuntimeObjectHandler *p_user1){

		implementer = p_user1;

		isStop = false;
	}


	/**Destructor**/
	inline ~MyThread(){}


	/**This method is used to stop the thread**/
	void stop(){

		isStop=true;
	}


	/**svc method **/
	int svc(void){

		int ret;

	//	cout<<"getSelObj: "<<implementer->getSelObj()<<endl;

		struct pollfd fds[1];

		fds[0].fd = implementer->getSelObj();
		fds[0].events = POLLIN;

		while(!isStop)
		{
			ret = poll(fds, 1, 1);
			if (ret == 0)
			{
				//cout<<"timeout off"<<endl;

			}else if (ret == -1){
				printf("poll error: \n");
			}else{

				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		return 0;
	}

private:

	RuntimeObjectHandler *implementer;

	bool isStop;

};



int printMenu();
int printListAttribute();
void createRuntimeObject(RuntimeObjectHandler *p_obj);
int getAttributeValue(const char *p_dn, int p_attrId);
void modifyRuntimeAttribute(RuntimeObjectHandler *p_obj, const char*  p_dn, int p_attrId);
void deleteRuntimeObject(RuntimeObjectHandler *p_obj);



int main() {


	cout<<"*--------------------------------------------------------------------*\n"
		<<"*                                                                    *\n"
		<<"*                     IMM RUNTIME OBJECT DEMO                        *\n"
		<<"*                                                                    *\n"
		<<"*--------------------------------------------------------------------*\n"
		<<endl;


	int choice = 0;

	ACS_CC_ReturnType returnCode;

	RuntimeObjectHandler runObj;

	/*The implementer name of the object*/
	std::string implementerName("Implementer_1");

	returnCode = runObj.init(implementerName);

	if(returnCode != ACS_CC_SUCCESS){
		printf("Initialization FAILURE \n");
		return -1;
	}


	MyThread th(&runObj);
	th.activate();

	do{

		/**Print the choice menu**/
		choice = printMenu();

		if((choice<0) || (choice>5)){
			cout<<"Incorrect value"<<endl;
			break;
		}

		if( choice == 1 ){

			cout<<endl;
			createRuntimeObject(&runObj);

		}else if( choice ==2 ){

			int attrId = 0;
			char dn[40];
			char cont;


			cout<<"\nInsert DN of the object containing the value to be retrieved\n - ";
			cin>>dn;

			do{
				cout<<endl;
				attrId = printListAttribute();
				getAttributeValue(dn, attrId);

				cout<<"Retrieve the value for another attribute? (y/n)\n - ";
		//		cin>>cont;
				do{
					cin.clear();
					cin>>cont;
					cin.clear();
					if(cont != 'y' && cont != 'n')
						cout<<"Incorrect Value. Retry\n - ";
				}while(cont != 'y' && cont != 'n');


			}while(cont == 'y');

			sleep(1);


		}else if( choice == 3){


			int attr = 0;
			char dn[40];
			char cont;

			cout<<"\nInsert DN of the object containing the value to change\n - ";
			cin>>dn;

			do{
				cout<<endl;
				attr = printListAttribute();

				modifyRuntimeAttribute(&runObj, dn,  attr);

				cout<<"change the value of another attribute? (y/n)\n - ";
			//	cin>>cont;
				do{
					cin.clear();
					cin>>cont;
					cin.clear();
					if(cont != 'y' && cont != 'n')
						cout<<"Incorrect Value. Retry\n - ";
				}while(cont != 'y' && cont != 'n');

			}while(cont == 'y');

			sleep(1);

		}else if( choice == 4){

			cout<<endl;
			deleteRuntimeObject(&runObj);

		}

		cout<<endl;

	}while( (choice < 5) );


	th.stop();
	th.wait();


	returnCode = runObj.finalize();

	if(returnCode != ACS_CC_SUCCESS){
		printf("Finalize FAILURE \n");
		return -1;
	}


	return 0;
}



int printMenu(){
	int value;

	cout<<"Select an operation: "
		<<"\n 1. create a new IMM runtime object "
		<<"\n 2. retrieve the value of an attribute to an IMM object"
		<<"\n 3. modify the value of an attribute to an IMM object"
		<<"\n 4. remove a IMM runtime object"
		<<"\n 5. exit"
		<<"\n - ";
	cin>>value;

	return value;
}


int printListAttribute(){

	int value;
	cout<<endl;
	cout<<"Select an attribute: "
		<<"\n*----------------------------------------------*"
		<<"\n*\t Name Attribute\t\t Type          *"
		<<"\n*----------------------------------------------*"
		<<"\n 1.\t"<< name_attrFloatC <<"\t\t SA_FLOAT_T"
		<<"\n 2.\t"<< name_attrInt64C <<"\t\t SA_INT64_T"
		<<"\n 3.\t"<< name_attrInt32C <<"\t\t SA_INT32_T"
		<<"\n 4.\t"<< name_attrDoubleC <<"\t\t SA_DOUBLE_T"
		<<"\n 5.\t"<< name_attrUnsInt64C <<"\t\t SA_UINT64_T"
		<<"\n 6.\t"<< name_attrUnsInt32C <<"\t\t SA_UINT32_T"
		<<"\n 7.\t"<< name_attrSaNameC <<"\t\t SA_NAME_T"
		<<"\n 8.\t"<< name_attrStringC <<"\t\t SA_STRING_T"

		<<"\n 9.\t"<< name_attrFloatNC <<"\t\t SA_FLOAT_T"
		<<"\n 10.\t"<< name_attrInt64NC <<"\t\t SA_INT64_T"
		<<"\n 11.\t"<< name_attrInt32NC <<"\t\t SA_INT32_T"
		<<"\n 12.\t"<< name_attrDoubleNC <<"\t\t SA_DOUBLE_T"
		<<"\n 13.\t"<< name_attrUnsInt64NC <<"\t\t SA_UINT64_T"
		<<"\n 14.\t"<< name_attrUnsInt32NC <<"\t\t SA_UINT32_T"
		<<"\n 15.\t"<< name_attrSaNameNC <<"\t\t SA_NAME_T"
		<<"\n 16.\t"<< name_attrStringNC <<"\t\t SA_STRING_T"
		<<"\n - ";
	cin>>value;

	return value;


}



/*This method instances an object of the class above defined and provide values for all attribute*/
void createRuntimeObject(RuntimeObjectHandler *p_obj) {

	/* Name of the class to be defined*/
	char *nameClass = const_cast<char *>("runtimeClass_test");

	/*The Name of parent name of teh class to be defined*/
	char* nomeParent = const_cast<char*>("safApp=safImmService");


	/*******************Values for Attributes *************************************************/

	//char* rdnValue = const_cast<char *>("runtimeObj_1");

	float floatValue_1 = 12.25;

	long long int int64Value = -998877;

	int int32Value = -112233;

	double doubleValue_1 = 11.33;

	unsigned long unsInt64Value = 998877;

	unsigned int unsInt32Value = 332211;

	char* saNameValue_1 = const_cast<char *>("provaSaName_1");

	char* stringValue_1 = const_cast<char *>("provaString_1");


	/***************************************************************************************/

	ACS_CC_ReturnType returnCode;

	/*The list of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/*the attributes*/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attrFLOAT_C;
	ACS_CC_ValuesDefinitionType attrINT64_C;
	ACS_CC_ValuesDefinitionType attrINT32_C;
	ACS_CC_ValuesDefinitionType attrDOUBLE_C;
	ACS_CC_ValuesDefinitionType attrUNSI64_C;
	ACS_CC_ValuesDefinitionType attrUNSI32_C;
	ACS_CC_ValuesDefinitionType attrNAMET_C;
	ACS_CC_ValuesDefinitionType attrSTRING_C;

	ACS_CC_ValuesDefinitionType attrFLOAT_NC;
	ACS_CC_ValuesDefinitionType attrINT64_NC;
	ACS_CC_ValuesDefinitionType attrINT32_NC;
	ACS_CC_ValuesDefinitionType attrDOUBLE_NC;
	ACS_CC_ValuesDefinitionType attrUNSI64_NC;
	ACS_CC_ValuesDefinitionType attrUNSI32_NC;
	ACS_CC_ValuesDefinitionType attrNAMET_NC;
	ACS_CC_ValuesDefinitionType attrSTRING_NC;

	char rdnValue[30];

	cout<<"Insert the name of the object\n - ";
	cin>>rdnValue;



	/*Fill the rdn Attribute */
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = valueRDN;


	/*Fill the attribute attrFLOAT_C*/
	attrFLOAT_C.attrName = name_attrFloatC;
	attrFLOAT_C.attrType =ATTR_FLOATT;
	attrFLOAT_C.attrValuesNum = 1;
	void* valueFLOAT[1] = { reinterpret_cast<void*>(&floatValue_1) };
	attrFLOAT_C.attrValues = valueFLOAT;


	/*Fill the attribute attrINT64_C*/
	attrINT64_C.attrName = name_attrInt64C;
	attrINT64_C.attrType =ATTR_INT64T;
	attrINT64_C.attrValuesNum = 1;
	void* valuesInt64[1] = {reinterpret_cast<void*>(&int64Value)};
	attrINT64_C.attrValues = valuesInt64;


	/*Fill the attribute attrINT32_C*/
	attrINT32_C.attrName = name_attrInt32C;
	attrINT32_C.attrType =ATTR_INT32T;
	attrINT32_C.attrValuesNum = 1;
	void* valuesInt32[1] = {reinterpret_cast<void*>(&int32Value)};
	attrINT32_C.attrValues = valuesInt32;


	/*Fill the attribute attrDOUBLE_C*/
	attrDOUBLE_C.attrName = name_attrDoubleC;
	attrDOUBLE_C.attrType =ATTR_DOUBLET;
	attrDOUBLE_C.attrValuesNum = 1;
	void* valuesDouble[1] = {reinterpret_cast<void*>(&doubleValue_1)};
	attrDOUBLE_C.attrValues = valuesDouble;


	/*Fill the attribute attrUNSI64_C*/
	attrUNSI64_C.attrName = name_attrUnsInt64C;
	attrUNSI64_C.attrType =ATTR_UINT64T;
	attrUNSI64_C.attrValuesNum = 1;
	void* valuesUnsInt64[1] = {reinterpret_cast<void*>(&unsInt64Value) };
	attrUNSI64_C.attrValues = valuesUnsInt64;


	/*Fill the attribute attrUNSI32_C*/
	attrUNSI32_C.attrName = name_attrUnsInt32C;
	attrUNSI32_C.attrType =ATTR_UINT32T;
	attrUNSI32_C.attrValuesNum = 1;
	void* valuesUnsInt32[1] = {reinterpret_cast<void*>(&unsInt32Value) };
	attrUNSI32_C.attrValues = valuesUnsInt32;


	/*Fill the attribute attrNAMET_C*/
	attrNAMET_C.attrName = name_attrSaNameC;
	attrNAMET_C.attrType =ATTR_NAMET;
	attrNAMET_C.attrValuesNum = 1;
	void* valueNameT[1]={reinterpret_cast<void*>(saNameValue_1)};
	attrNAMET_C.attrValues = valueNameT;



	/*Fill the attribute attrSTRING_C*/
	attrSTRING_C.attrName = name_attrStringC;
	attrSTRING_C.attrType =ATTR_STRINGT;
	attrSTRING_C.attrValuesNum = 1;
	void* valueString[1]={reinterpret_cast<void*>(stringValue_1)};
	attrSTRING_C.attrValues = valueString;


	/********************runtime non-cached attribute*******************************/


	/*Fill the attribute attrFLOAT_C*/
	attrFLOAT_NC.attrName = name_attrFloatNC;
	attrFLOAT_NC.attrType = ATTR_FLOATT;
	attrFLOAT_NC.attrValuesNum = 0;
	attrFLOAT_NC.attrValues = 0;


	/*Fill the attribute attrINT64_NC*/
	attrINT64_NC.attrName = name_attrInt64NC;
	attrINT64_NC.attrType =ATTR_INT64T;
	attrINT64_NC.attrValuesNum = 0;
	attrINT64_NC.attrValues = 0;


	/*Fill the attribute attrINT32_NC*/
	attrINT32_NC.attrName = name_attrInt32NC;
	attrINT32_NC.attrType =ATTR_INT32T;
	attrINT32_NC.attrValuesNum = 0;
	attrINT32_NC.attrValues = 0;


	/*Fill the attribute attrDOUBLE_NC*/
	attrDOUBLE_NC.attrName = name_attrDoubleNC;
	attrDOUBLE_NC.attrType =ATTR_DOUBLET;
	attrDOUBLE_NC.attrValuesNum = 0;
	attrDOUBLE_NC.attrValues = 0;


	/*Fill the attribute attrUNSI64_NC*/
	attrUNSI64_NC.attrName = name_attrUnsInt64NC;
	attrUNSI64_NC.attrType =ATTR_UINT64T;
	attrUNSI64_NC.attrValuesNum = 0;
	attrUNSI64_NC.attrValues = 0;


	/*Fill the attribute attrUNSI32_NC*/
	attrUNSI32_NC.attrName = name_attrUnsInt32NC;
	attrUNSI32_NC.attrType =ATTR_UINT32T;
	attrUNSI32_NC.attrValuesNum = 0;
	attrUNSI32_NC.attrValues = 0;


	/*Fill the attribute attrNAMET_NC*/
	attrNAMET_NC.attrName = name_attrSaNameNC;
	attrNAMET_NC.attrType =ATTR_NAMET;
	attrNAMET_NC.attrValuesNum = 0;
	attrNAMET_NC.attrValues = 0;


	/*Fill the attribute attrSTRING_NC*/
	attrSTRING_NC.attrName = name_attrStringNC;
	attrSTRING_NC.attrType =ATTR_STRINGT;
	attrSTRING_NC.attrValuesNum = 0;
	attrSTRING_NC.attrValues = 0;


	/*Add the atributes to vector*/
	AttrList.push_back(attributeRDN);
	AttrList.push_back(attrFLOAT_C);
	AttrList.push_back(attrINT64_C);
	AttrList.push_back(attrINT32_C);
	AttrList.push_back(attrDOUBLE_C);
	AttrList.push_back(attrUNSI64_C);
	AttrList.push_back(attrUNSI32_C);
	AttrList.push_back(attrNAMET_C);
	AttrList.push_back(attrSTRING_C);

	AttrList.push_back(attrFLOAT_NC);
	AttrList.push_back(attrINT64_NC);
	AttrList.push_back(attrINT32_NC);
	AttrList.push_back(attrDOUBLE_NC);
	AttrList.push_back(attrUNSI64_NC);
	AttrList.push_back(attrUNSI32_NC);
	AttrList.push_back(attrNAMET_NC);
	AttrList.push_back(attrSTRING_NC);


	returnCode = p_obj->createRuntimeObj(nameClass, nomeParent, AttrList);

	if(returnCode == ACS_CC_SUCCESS){
		printf("Creation object %s completed\n", rdnValue);
	}else{
		printf("error - Creation object %s failed \n", rdnValue);
	}

	sleep(1);

}




int getAttributeValue(const char* p_dn, int p_attrId){


	char *dnObject = const_cast<char*>(p_dn);

	ACS_CC_ReturnType returnCode;

	ACS_CC_ImmParameter parameter;

	OmHandler omHand;

	returnCode = omHand.Init();
	if(returnCode != ACS_CC_SUCCESS){
		cout<<"error - Initialization of OmHandler failure"<<endl;
		return -1;
	}


	switch(p_attrId){
	case 1:
		parameter.attrName = name_attrFloatC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(float*)parameter.attrValues[i]<<endl;
			}
		}

		break;
	case 2:
		parameter.attrName = name_attrInt64C;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(long long int*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 3:
		parameter.attrName = name_attrInt32C;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(int*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 4:
		parameter.attrName = name_attrDoubleC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(double*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 5:
		parameter.attrName = name_attrUnsInt64C;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(unsigned long*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 6:
		parameter.attrName = name_attrUnsInt32C;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(unsigned int*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 7:
		parameter.attrName = name_attrSaNameC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<(char*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 8:
		parameter.attrName = name_attrStringC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<(char*)parameter.attrValues[i]<<endl;
			}
		}

		break;
	case 9:
		parameter.attrName = name_attrFloatNC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(float*)parameter.attrValues[i]<<endl;
			}
		}

		break;
	case 10:
		parameter.attrName = name_attrInt64NC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(long long int*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 11:
		parameter.attrName = name_attrInt32NC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(int*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 12:
		parameter.attrName = name_attrDoubleNC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(double*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 13:
		parameter.attrName = name_attrUnsInt64NC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(unsigned long*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 14:
		parameter.attrName = name_attrUnsInt32NC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<*(unsigned int*)parameter.attrValues[i]<<endl;
			}
		}

		break;

	case 15:
		parameter.attrName = name_attrSaNameNC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<(char*)parameter.attrValues[i]<<endl;
			}
		}



		break;

	case 16:
		parameter.attrName = name_attrStringNC;
		returnCode = omHand.getAttribute(dnObject, &parameter );

		if( returnCode == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<parameter.attrValuesNum; i++){
				if(parameter.attrValues[i] != 0){
					cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: "<<(char*)parameter.attrValues[i]<<endl;
				}else{
					cout<<"Attribute name:"<<parameter.attrName<<"\nvalue: 0"<<endl;
				}
			}
		}

		break;
	}//end switch


	returnCode = omHand.Finalize();
	if(returnCode !=ACS_CC_SUCCESS){
		cout<<"error -  Finalize of OmHandler failure"<<endl;
		return -1;
	}

	return 0;

}



void modifyRuntimeAttribute(RuntimeObjectHandler *p_obj, const char* p_dn, int p_attrId){


	/* The DN of the object */
//	char *dnObject = const_cast<char*>("runtimeObj_1,safApp=safImmService");

	ACS_CC_ReturnType returnCode;

	/*Set a new value for ATTRIBUTE*/
	ACS_CC_ImmParameter parToModify;


	void* new_value[1] = {0};

	char* nameAttribute = 0;

	float new_floatValue = 0;
	long long int new_int64Value = 0;
	int new_int32Value = 0;
	double new_doubleValue = 0;
	unsigned long new_unsInt64Value = 0;
	unsigned int new_unsInt32Value = 0;
	char new_saNameValue[30];
	char new_stringValue[30];

	cout<<endl;

	switch(p_attrId){
	case 1:

		cout<<"Insert new value for attribute "<< name_attrFloatC <<"\n - ";
		cin>> new_floatValue;

		nameAttribute = name_attrFloatC;
		parToModify.attrName = name_attrFloatC;
		parToModify.attrType = ATTR_FLOATT;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_floatValue);
		parToModify.attrValues = new_value;

		break;
	case 2:

		cout<<"Insert new value for attribute "<< name_attrInt64C <<"\n - ";
		cin>> new_int64Value;

		nameAttribute = name_attrInt64C;
		parToModify.attrName = name_attrInt64C;
		parToModify.attrType = ATTR_INT64T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_int64Value);
		parToModify.attrValues = new_value;

		break;
	case 3:

		cout<<"Insert new value for attribute "<< name_attrInt32C <<"\n - ";
		cin>> new_int32Value;

		nameAttribute = name_attrInt32C;
		parToModify.attrName = name_attrInt32C;
		parToModify.attrType = ATTR_INT32T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_int32Value);
		parToModify.attrValues = new_value;

		break;
	case 4:

		cout<<"Insert new value for attribute "<< name_attrDoubleC <<"\n - ";
		cin>> new_doubleValue;

		nameAttribute = name_attrDoubleC;
		parToModify.attrName = name_attrDoubleC;
		parToModify.attrType = ATTR_DOUBLET;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_doubleValue);
		parToModify.attrValues = new_value;

		break;
	case 5:

		cout<<"Insert new value for attribute "<< name_attrUnsInt64C <<"\n - ";
		cin>> new_unsInt64Value;

		nameAttribute = name_attrUnsInt64C;
		parToModify.attrName = name_attrUnsInt64C;
		parToModify.attrType = ATTR_UINT64T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_unsInt64Value);
		parToModify.attrValues = new_value;

		break;
	case 6:

		cout<<"Insert new value for attribute "<< name_attrUnsInt32C <<"\n - ";
		cin>> new_unsInt32Value;

		nameAttribute = name_attrUnsInt32C;
		parToModify.attrName = name_attrUnsInt32C;
		parToModify.attrType = ATTR_UINT32T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_unsInt32Value);
		parToModify.attrValues = new_value;

		break;
	case 7:

		cout<<"Insert new value for attribute "<< name_attrSaNameC <<"\n - ";
		cin>> new_saNameValue;

		nameAttribute = name_attrSaNameC;
		parToModify.attrName = name_attrSaNameC;
		parToModify.attrType = ATTR_NAMET;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(new_saNameValue);
		parToModify.attrValues = new_value;

		break;
	case 8:

		cout<<"Insert new value for attribute "<< name_attrStringC <<"\n - ";
		cin>> new_stringValue;

		nameAttribute = name_attrStringC;
		parToModify.attrName = name_attrStringC;
		parToModify.attrType = ATTR_STRINGT;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(new_stringValue);
		parToModify.attrValues = new_value;

		break;
	case 9:

		cout<<"Insert new value for attribute "<< name_attrFloatNC <<"\n - ";
		cin>> new_floatValue;

		nameAttribute = name_attrFloatNC;
		parToModify.attrName = name_attrFloatNC;
		parToModify.attrType = ATTR_FLOATT;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_floatValue);
		parToModify.attrValues = new_value;

		break;
	case 10:

		cout<<"Insert new value for attribute "<< name_attrInt64NC <<"\n - ";
		cin>> new_int64Value;

		nameAttribute = name_attrInt64NC;
		parToModify.attrName = name_attrInt64NC;
		parToModify.attrType = ATTR_INT64T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_int64Value);
		parToModify.attrValues = new_value;

		break;
	case 11:

		cout<<"Insert new value for attribute "<< name_attrInt32NC <<"\n - ";
		cin>> new_int32Value;

		nameAttribute = name_attrInt32NC;
		parToModify.attrName = name_attrInt32NC;
		parToModify.attrType = ATTR_INT32T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_int32Value);
		parToModify.attrValues = new_value;

		break;
	case 12:

		cout<<"Insert new value for attribute "<< name_attrDoubleNC <<"\n - ";
		cin>> new_doubleValue;

		nameAttribute = name_attrDoubleNC;
		parToModify.attrName = name_attrDoubleNC;
		parToModify.attrType = ATTR_DOUBLET;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_doubleValue);
		parToModify.attrValues = new_value;

		break;
	case 13:

		cout<<"Insert new value for attribute "<< name_attrUnsInt64NC <<"\n - ";
		cin>> new_unsInt64Value;

		nameAttribute = name_attrUnsInt64NC;
		parToModify.attrName = name_attrUnsInt64NC;
		parToModify.attrType = ATTR_UINT64T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_unsInt64Value);
		parToModify.attrValues = new_value;

		break;
	case 14:

		cout<<"Insert new value for attribute "<< name_attrUnsInt32NC <<"\n - ";
		cin>> new_unsInt32Value;

		nameAttribute = name_attrUnsInt32NC;
		parToModify.attrName = name_attrUnsInt32NC;
		parToModify.attrType = ATTR_UINT32T;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(&new_unsInt32Value);
		parToModify.attrValues = new_value;

		break;
	case 15:

		cout<<"Insert new value for attribute "<< name_attrSaNameNC <<"\n - ";
		cin>> new_saNameValue;

		nameAttribute = name_attrSaNameNC;
		parToModify.attrName = name_attrSaNameNC;
		parToModify.attrType = ATTR_NAMET;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(new_saNameValue);
		parToModify.attrValues = new_value;

		break;
	case 16:

		cout<<"Insert new value for attribute "<< name_attrStringNC <<"\n - ";
		cin>> new_stringValue;

		nameAttribute = name_attrStringNC;
		parToModify.attrName = name_attrStringNC;
		parToModify.attrType = ATTR_STRINGT;

		parToModify.attrValuesNum = 1;
		new_value[0] = reinterpret_cast<void*>(new_stringValue);
		parToModify.attrValues = new_value;

		break;

	}//end switch

	returnCode = p_obj->modifyRuntimeObj(const_cast<char*>(p_dn), &parToModify);

	if(returnCode == ACS_CC_SUCCESS){
		printf("Attribute %s changed\n", nameAttribute);
	}else{
		printf("Attribute %s not changed\n", nameAttribute);
	}

}


void deleteRuntimeObject(RuntimeObjectHandler *p_obj){


	ACS_CC_ReturnType returnCode;

	/* The DN of the object */
	//char *dnObject = const_cast<char*>("runtimeObj_1,safApp=safImmService");

	char dnObject[40];
	cout<<"\nInsert DN of the object to be delete\n - ";
	cin>>dnObject;

	returnCode = p_obj->deleteRuntimeObj(const_cast<char*>(dnObject));


	if(returnCode == ACS_CC_SUCCESS){
		printf("cancellation of object %s completed\n", dnObject);
	}else{
		printf("error - cancellation of object %s failed\n", dnObject);
	}

}
