/*
 * getErrorUT_Main.cpp
 *
 *  Created on: Jan 10, 2011
 *      Author: xpaomaz
 */

#include <iostream>
#include <string>

#include "ACS_APGCC_OiHandler.h"
#include "MyObjectHandler.h"

#include "OmHandlerV2.h"
#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"
#include "ace/Task.h"
#include <sys/poll.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"



/*General values defined for test*/
unsigned long long ccbinit=1;
int errorIdValueTest1 = 2;
std::string errorTextValueTest1("AP is not defined");
ACS_APGCC_OiHandler handler;
char* stringValue_1 = const_cast<char *>("Incorrect Usage");

MyObjectHandler obj;
char objName[]="errorHandlerId=1,serviceId=1,safApp=safImmService";

using namespace std;
/*start thread*/
class MyThread : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThread(MyObjectHandler *p_user1){

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


		cout<<"Thread AVVIATO..................."<<endl;

		int ret;

		cout<<"getSelObj: "<<implementer->getSelObj()<<endl;

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

				cout<<"FD THREAD: "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		cout<<"Thread TERMINATO"<<endl;

		return 0;
	}

private:

	MyObjectHandler *implementer;

	bool isStop;

};

/*end thread*/





/*
 * Prepare environment. An instance of IMM class Service id defined under this node the Runtime object
 * holding error info is located.
 */
void PrepareEnvironment(){

	ACS_CC_ReturnType result;
	OmHandlerV2 omHandler;
	char nameClass[20] = "Service";
	char nameErrorClass[20] = "ErrorHandler";
	char* nomeParent = const_cast<char *>("safApp=safImmService");
	char* nomeParentRuntime = const_cast<char *>("serviceId=1,safApp=safImmService");
	char *objectRDN = "serviceId=1";
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	ACS_CC_ValuesDefinitionType attrRdn;

	/*Fill the rdn Attribute */
	char attrdn[]= "serviceId";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_STRINGT;
	attrRdn.attrValuesNum = 1;
	void* valRDN[1]= {reinterpret_cast<void*>(objectRDN)};
	//char* rdnValue = const_cast<char *>(p_objName);
	//attrRdn.attrValues = new void*[1];
	attrRdn.attrValues = valRDN;

	AttrList.push_back(attrRdn);

	/*create the instance of Service classserviceId=1,safApp=safImmService */
	omHandler.Init();
	result = omHandler.createObject(nameClass, nomeParent, AttrList );
	//ASSERTM("PrepareEnvironment:", result == ACS_CC_SUCCESS);

	/*define oi*/

	result = obj.init("ImplementerForService1");
	ASSERTM("PrepareEnvironment:init Runtime Owner Failed ", result == ACS_CC_SUCCESS);


	//MyThread th(&obj);

	//th.activate();


	/*the list of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrListRuntime;
	ACS_CC_ValuesDefinitionType attributeRuntimeRDN;
	ACS_CC_ValuesDefinitionType attributeRuntimeCCB;
	ACS_CC_ValuesDefinitionType attributeRuntimeERRORID;
	ACS_CC_ValuesDefinitionType attributeRuntimeERRORTEXT;

	/*rdn*/
	char attrRuntimeRdn[]= "errorHandlerId";
	char objectRuntimeRDN[] = "errorHandlerId=1";
	attributeRuntimeRDN.attrName = attrRuntimeRdn;
	attributeRuntimeRDN.attrType = ATTR_STRINGT;
	attributeRuntimeRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(objectRuntimeRDN)};
	attributeRuntimeRDN.attrValues = valueRDN;

	/*ccb*/
	char attCCB[]= "ccbId";
	attributeRuntimeCCB.attrName = attCCB;
	attributeRuntimeCCB.attrType =ATTR_UINT64T;
//	attributeRuntimeCCB.attrValuesNum = 1;
//	void* valuesccb[1] = {reinterpret_cast<void*>(&ccbinit) };
//	attributeRuntimeCCB.attrValues = valuesccb;
	attributeRuntimeCCB.attrValuesNum = 0;
	attributeRuntimeCCB.attrValues = 0;

	/*errorId*/
	char attErrorId[]= "errorId";
	attributeRuntimeERRORID.attrName = attErrorId;
	attributeRuntimeERRORID.attrType =ATTR_INT32T;
	//attributeRuntimeERRORID.attrValuesNum = 1;
	attributeRuntimeERRORID.attrValuesNum = 0;


	//attributeRuntimeERRORID.attrValues = valuesERR;
	attributeRuntimeERRORID.attrValues = 0;

	/*errtext*/
	char attErrorText[]= "errorText";

	attributeRuntimeERRORTEXT.attrName = attErrorText;
	attributeRuntimeERRORTEXT.attrType =ATTR_STRINGT;
//	attributeRuntimeERRORTEXT.attrValuesNum = 1;
//	char* stringValue_1 = const_cast<char *>("provaString_1");
//	void* new_valueString[1]={reinterpret_cast<void*>(stringValue_1)};
//	attributeRuntimeERRORTEXT.attrValues = new_valueString;
	attributeRuntimeERRORTEXT.attrValuesNum = 0;
	attributeRuntimeERRORTEXT.attrValues = 0;


	AttrListRuntime.push_back(attributeRuntimeRDN);
	AttrListRuntime.push_back(attributeRuntimeCCB);
	AttrListRuntime.push_back(attributeRuntimeERRORID);
	AttrListRuntime.push_back(attributeRuntimeERRORTEXT);
	cout << "....................................................."<<endl;
	result = obj.createRuntimeObj(nameErrorClass, nomeParentRuntime, AttrListRuntime);
	if (result != ACS_CC_SUCCESS ){
		cout << "ERROR getInternalLastErrorText()="<< obj.getInternalLastErrorText()<<endl;
	}
	ASSERTM("PrepareEnvironment:obj.createRuntimeObj Failed ", result == ACS_CC_SUCCESS);


	/*************added*/
	/*Set a new value for ATTRIBUTE_1*/
	ACS_CC_ImmParameter parToModify_1;
	parToModify_1.attrName = attCCB;
	parToModify_1.attrType = ATTR_UINT64T;
	parToModify_1.attrValuesNum = 1;
	void* valuesccb[1] = {reinterpret_cast<void*>(&ccbinit) };
	parToModify_1.attrValues =valuesccb;

	result = obj.modifyRuntimeObj(objName, &parToModify_1);
	ASSERTM("PrepareEnvironment:obj.modifyRuntimeObj1 Failed ", result == ACS_CC_SUCCESS);

	/*Set a new value for ATTRIBUTE_2*/
	ACS_CC_ImmParameter parToModify_2;
	parToModify_2.attrName = attErrorId;
	parToModify_2.attrType = ATTR_INT32T;
	parToModify_2.attrValuesNum = 1;
	void* valuesERR[1] = {reinterpret_cast<void*>(&errorIdValueTest1)};
	parToModify_2.attrValues =valuesERR;

	result = obj.modifyRuntimeObj(objName, &parToModify_2);
	ASSERTM("PrepareEnvironment:obj.modifyRuntimeObj2 Failed ", result == ACS_CC_SUCCESS);

	/*Set a new value for ATTRIBUTE_3*/
	ACS_CC_ImmParameter parToModify_3;
	parToModify_3.attrName = attErrorText;
	parToModify_3.attrType = ATTR_STRINGT;
	parToModify_3.attrValuesNum = 1;
	void* new_valueString[1]={reinterpret_cast<void*>(stringValue_1)};
	parToModify_3.attrValues =new_valueString;

	result = obj.modifyRuntimeObj(objName, &parToModify_3);
	ASSERTM("PrepareEnvironment:obj.modifyRuntimeObj3 Failed ", result == ACS_CC_SUCCESS);
	sleep (1);

	/************end added*/
	omHandler.Finalize();


}

/*clear Environment the Obj1 is deleted and the class too is deleted*/
void ClearEnvironment(){
	ACS_CC_ReturnType result;
	std::string val;
	int errVal= 118;
	val = handler.getErrorText(errVal);
	std::cout<<"handler.getErrorText"<<val<<std::endl;
	ASSERTM("ClearEnvironment :handler.getErrorText118", val == "CP is not defined" );

	errVal= 0;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText0", val == "OK" );

	errVal= 1;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText 1", val == "Error when executing (general fault)" );

	errVal= 2;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText 2", val == "Incorrect Usage" );

	errVal= 113;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText 113", val == "AP is not defined" );

	errVal= 115;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText 114", val == "Illegal command in this system configuration" );

	errVal= 116;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText 114", val == "Illegal option in this system configuration" );

	errVal= 117;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText 114", val == "Unable to connect to server" );

	errVal= 119;
	val = handler.getErrorText(errVal);
	ASSERTM("ClearEnvironment :handler.getErrorText 114", val == "Command partially executed" );



	sleep (1);
	ASSERTM("ClearEnvironment :omHandler.removeClass", result == ACS_CC_SUCCESS );
	result = obj.deleteRuntimeObj(const_cast<char*>(objName));
	ASSERTM("ClearEnvironment", result == ACS_CC_SUCCESS);



}
/*
 * this is the first test
 */
void Test_1(){

	ACS_CC_ReturnType result;
	OmHandlerV2 omHandler;


	MyThread th(&obj);

	th.activate();

	std:: string parentDN("serviceId=1,safApp=safImmService");
	std:: string parentDN2("serviceId=2,safApp=safImmService");
	int errorId;
	std::string errorText;

	omHandler.Init();
	result = omHandler.getLastErrorText(parentDN, errorId, errorText);
	std::cout<<"\nomHandler.getLastErrorText "<< errorText<<"id="<<errorId<<std::endl;
	int errorIdValueTest1Expected0k = 1;
	std::string errorTextValueTestExpectedOK= "Error when Executing";

	ASSERT_EQUALM("CUTE: Test_1 check on values errorId 1", errorIdValueTest1Expected0k , errorId  );
	ASSERT_EQUALM("CUTE: Test_1 check on values errorText 1", errorTextValueTestExpectedOK , errorText  );

	int errorIdValueTest1Expected = 117;
	std::string errorTextValueTestExpected= "Unable to connect to server";
	result = omHandler.getLastErrorText(parentDN2, errorId, errorText);
	std::cout<<"omHandler.getLastErrorText "<< errorText<<"id="<<errorId<<std::endl;
	ASSERT_EQUALM("CUTE: Test_1 check on values errorId  2", errorIdValueTest1Expected , errorId  );
	ASSERT_EQUALM("CUTE: Test_1 check on values errorText 2", errorTextValueTestExpected , errorText  );


	ClearEnvironment();
	th.stop();
	obj.finalize();
	omHandler.Finalize();
}
/*end Test_1*/

/*SUITE*/
void runSuite(){

	cute::suite s;

	s.push_back(CUTE(PrepareEnvironment));
	s.push_back(CUTE(Test_1));
	//s.push_back(CUTE(ClearEnvironment));

	cute::ide_listener lis;

	cute::makeRunner(lis)(s, "The Suite");
}


/*Main*/
int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "Get Error Text CUTE Test!!!" << endl;

	runSuite();

	cout << "!!!Get Error TextCUTE Test!! END!!!" << endl;

	return 0;
}


