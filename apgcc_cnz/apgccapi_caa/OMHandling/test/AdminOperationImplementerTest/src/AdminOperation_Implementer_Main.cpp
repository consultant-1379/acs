/*
 * AdminOperation_Implementer_Main.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */



#include <iostream>

#include "ace/Task.h"
#include <sys/poll.h>

#include "AdminOperation_MyImplementer.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "ACS_APGCC_OiHandler.h"

#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

using namespace std;


class MyThread : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThread(AdminOperation_MyImplementer *p_user1){

		cout<<"Costruttore Thread"<<endl;

		implementer = p_user1;

		isStop = false;
	}


	/**Destructor**/
	~MyThread(){
		cout<<"Distruttore Thread"<<endl;
	}


	/**This method is used to stop the thread**/
	void stop(){

		cout<<"Method Stop invocated"<<endl;

		isStop=true;
	}


	/**svc method **/
	int svc(void){


		//cout<<"Thread AVVIATO"<<endl;

		int ret;

		//cout<<"getSelObj: "<<implementer->getSelObj()<<endl;

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

				//cout<<"FD THREAD: "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		//cout<<"Thread TERMINATO"<<endl;

		return 0;
	}

private:

	AdminOperation_MyImplementer *implementer;

	bool isStop;

};


//----------------------------------------------------------------------------------------------------------------------------------//
/********************************************************create Test Class************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
int createClassTest(OmHandler omHandler, const char *p_className){

	cout << " -> Try to create a new configuration class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char*>("ATTRIBUTE_1"), ATTR_FLOATT, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char*>("ATTRIBUTE_2"),ATTR_INT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("ATTRIBUTE_3"),ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {const_cast<char*>("ATTRIBUTE_4"),ATTR_DOUBLET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {const_cast<char*>("ATTRIBUTE_5"),ATTR_UINT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {const_cast<char*>("ATTRIBUTE_6"),ATTR_UINT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeNAME = {const_cast<char*>("ATTRIBUTE_7"), ATTR_NAMET, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char*>("ATTRIBUTE_8"),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char*>("ATTR_TIME"), ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeUnI32);
	classAttributes.push_back(attributeI64);
	classAttributes.push_back(attributeUnI64);
	classAttributes.push_back(attributeTIME);
	classAttributes.push_back(attributeNAME);
	classAttributes.push_back(attributeFLOAT);
	classAttributes.push_back(attributeDOUBLE);
	classAttributes.push_back(attributeSTRING);

	result = omHandler.defineClass( const_cast<char *>(p_className),categoryClassToBeDefined,classAttributes);
	if( result == ACS_CC_SUCCESS ){
		cout << " -> Class " << p_className << " created" << endl;
		cout << endl;

		return 0;
	}else {
		cout << " -> ERROR!!! Class " << p_className << " NOT created errCode = " << omHandler.getInternalLastError() << endl;
		cout << endl;

		return -1;
	}

}



//----------------------------------------------------------------------------------------------------------------------------------//
/*****************************************************create Object Test************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
int createObjectTest(OmHandler omHandler, const char* p_className, const char *p_objName){


	cout << " -> Try to create a new configuration object " << p_objName << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	char* nomeParent = const_cast<char *>("safApp=safImmService");


	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;

	/*Fill the rdn Attribute */
	char attrdn[]= "RDN_Attribute";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_NAMET;
	attrRdn.attrValuesNum = 1;
	char* rdnValue = const_cast<char *>(p_objName);
	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

	/*Fill the ATTRIBUTE_1 */
	char name_attFloat[]="ATTRIBUTE_1";
	attr2.attrName = name_attFloat;
	attr2.attrType = ATTR_FLOATT;
	attr2.attrValuesNum = 1;
	float floatValue = 222.9;
	attr2.attrValues = new void*[1];
	attr2.attrValues[0] = reinterpret_cast<void*>(&floatValue);

	/*Fill the ATTRIBUTE_6 */
	char name_attUint32[]="ATTRIBUTE_6";
	attr3.attrName = name_attUint32;
	attr3.attrType = ATTR_UINT32T;
	attr3.attrValuesNum = 1;
	unsigned int intValueU32 = 123;
	attr3.attrValues = new void*[1];
	attr3.attrValues[0] = reinterpret_cast<void*>(&intValueU32);


	char name_attrSaString[]= "ATTRIBUTE_8";
	attr4.attrName = name_attrSaString;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 1;
	char* stringValue = const_cast<char*>("test");
	attr4.attrValues = new void*[1];
	attr4.attrValues[0] =	reinterpret_cast<void*>(stringValue);
/**/
	AttrList.push_back(attrRdn);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);

	result = omHandler.createObject(p_className, nomeParent, AttrList );


	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << "," << nomeParent << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR !!! Object  " << p_objName << "," << nomeParent << " NOT created errCode= " << omHandler.getInternalLastError()<< endl;
		cout << endl;
		return -1;
	}

}


//----------------------------------------------------------------------------------------------------------------------------------//
/*****************************************************PrepareEnvironment function************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
void PrepareEnvironment(){

	OmHandler omHandler;
	ACS_CC_ReturnType result;

	result = omHandler.Init();
	if(result != ACS_CC_SUCCESS){
		cout << " -> OmHandler.init() failure\n";
		cout << " -> ERROR CODE: "<<omHandler.getInternalLastError()<< endl;
	}

	/**create a class **/
	char nameClass[20] = "Test_config";

	int res;
	res = createClassTest(omHandler, nameClass);

	if(res == 0){
		//cout<<" -> PrepareEnvironment:createClassTest:Test_config ok "<<endl;
	} else {
		cout<<" -> PrepareEnvironment:createClassTest:Test_config ko "<<endl;
	}

	/** create object test **/
	char *rdnObjectName = const_cast<char *>("object1");

	res = createObjectTest(omHandler, nameClass, rdnObjectName);

	if (res == 0){
		//cout<<" -> PrepareEnvironment:createObjectTest:Test_config ok "<<endl;
	}else {
		cout<<" -> PrepareEnvironment:createObjectTest:Test_config ko "<<endl;
	}

	result = omHandler.Finalize();
	if(result != ACS_CC_SUCCESS){
		cout << " -> OmHandler.Finalize() failure\n";
		cout << " -> ERROR CODE: "<<omHandler.getInternalLastError()<< endl;
	}

	return;
}



//----------------------------------------------------------------------------------------------------------------------------------//
/*****************************************************Delete Test class function*****************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
int deleteClassTest(OmHandler omHandler, const char *p_className){

	cout << " -> Try to delete a class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;


	result = omHandler.removeClass(const_cast<char *>(p_className));

	if( result == ACS_CC_SUCCESS ){
		cout<< " -> Class " << p_className << " deleted." << endl;
		cout<<endl;
		return 0;
	}else{
		cout << " -> ERROR!!! Class " << p_className << " NOT deleted errCode = " << omHandler.getInternalLastError() << endl;
		cout << endl;
		return -1;
	}
}



//----------------------------------------------------------------------------------------------------------------------------------//
/*****************************************************Delete Test object function*****************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
int deleteObjectTest(OmHandler omHandler, const char *p_objName){

	cout << " -> Try to delete a object " << p_objName <<endl;
	cout << endl;

	ACS_CC_ReturnType result;

	result = omHandler.deleteObject(const_cast<char*>(p_objName));



	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << " deleted" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR !!! Object  " << p_objName << " NOT deleted errCode= " << omHandler.getInternalLastError()<< endl;
		cout << endl;
		return -1;
	}

}


//----------------------------------------------------------------------------------------------------------------------------------//
/*****************************************************DeleteEnvironment function************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
void DeleteEnvironment(){

	OmHandler omHandler;
	ACS_CC_ReturnType result;

	result = omHandler.Init();
	if(result != ACS_CC_SUCCESS){
		cout << " -> OmHandler.init() failure\n";
		cout << " -> ERROR CODE: "<<omHandler.getInternalLastError()<< endl;
	}

	int res;

	/** delete object test **/
	char *objectName = const_cast<char *>("object1,safApp=safImmService");

	res = deleteObjectTest(omHandler, objectName);

	if (res == 0){
		//cout<<" -> DeleteEnvironment:deleteObjectTest ok "<<endl;
	}else {
		cout<<" -> DeleteEnvironment:deleteObjectTest ko "<<endl;
	}


	/**delete a class **/
	char nameClass[20] = "Test_config";

	res = deleteClassTest(omHandler, nameClass);

	if(res == 0){
		//cout<<" -> DeleteEnvironment:deleteClassTest:Test_config ok "<<endl;
	} else {
		cout<<" -> DeleteEnvironment:deleteClassTest:Test_config ko "<<endl;
	}

	result = omHandler.Finalize();
	if(result != ACS_CC_SUCCESS){
		cout << " -> OmHandler.Finalize() failure\n";
		cout << " -> ERROR CODE: "<<omHandler.getInternalLastError()<< endl;
	}

	return;
}








int main ( ) {


	PrepareEnvironment();

	ACS_CC_ReturnType returnCode;
	AdminOperation_MyImplementer obj;

	char choice;

	string implName = "AdminImplementer";
	string objImplemented = "object1,safApp=safImmService";
	string classImplemented = "Test_config";


	cout<<"INIT-------"<<endl;

	returnCode = obj.init(implName);
	if (returnCode != ACS_CC_SUCCESS){

		cout<<"INIT FAILURE"<<endl;
		cout<<"obj.getLastError()"<<obj.getInternalLastError()<<endl;
		cout<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
		return -1;
	}



	std::cout << " - insert C char to be Implementer of class "<<classImplemented<<endl;
	std::cout << " - insert O char to be Implementer of object "<<objImplemented<<endl;
	std::cin >> choice;

	if (choice == 'O'){
		std::cout << " - Object Implementer";

		returnCode = obj.setObjectImplemented(objImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"FAILURE::obj.setObjectImplemented"<<endl;
			cout<<"obj.getLastError()"<<obj.getInternalLastError()<< endl;
			cout<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}

	}else if (choice == 'C'){
		std::cout << " - Class Implementer";

		returnCode = obj.setClassImplemented(classImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"FAILURE::obj.setClasstImplemented"<<endl;
			cout<<"obj.getLastError()"<<obj.getInternalLastError()<< endl;
			cout<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}


	}else {
		cout << "ERROR: Option not recognize!!! " <<  endl;
		return -1;
	}

	/********************************/


	MyThread th(&obj);

	th.activate();

	char newChoice;

	std::cout << " - insert a char to stop thread and release";
	std::cin >> newChoice;



	if (choice == 'O'){

		returnCode = obj.releaseObjectImplemented(objImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"obj.releaseObjectImplemented"<<endl;
			cout<<"obj.releaseObjectImplemented"<<obj.getInternalLastError()<< endl<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}


	}else if (choice == 'C'){

		returnCode = obj.releaseClassImplemented(classImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"obj.releaseClassImplemented"<<endl;
			cout<<"obj.releaseClassImplemented"<<obj.getInternalLastError()<< endl<< "obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}

	}
	th.stop();
	std::cout << " - insert a char to exit "<<endl;
	std::cin >> newChoice;

	DeleteEnvironment();

	return 0;
}
