/*
 * adminOperationInvokeUT.cpp
 *
 *  Created on: Sep 20, 2011
 *      Author: xfabron
 *
 *
 *
 *
 *  INFORMATION: This executable test the invocation of Administrative Operation
 *  (mode sync and async). To run it is necessary execute also the executable
 *  AdminiOperationImplementerTest that assumes the role of implementer of class/object
 *  on which the actions are called
 *
 *
 */





#include <iostream>
#include <string>

#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"

#include "AdminOperationAsync_MyImplementer.h"

#include "ace/Task.h"
#include <sys/poll.h>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

using namespace std;


/*******************Values for Attributes *************************************************/

float floatValue = 12.25;
long long int int64Value = -998877;
int int32Value = -112233;
double doubleValue = 11.33;
unsigned long unsInt64Value = 998877;
unsigned int unsInt32Value = 332211;
char* saNameValue = const_cast<char *>("testSaName_1");
char* stringValue = const_cast<char *>("testString_1");
std::string stringValue2("the pen is on the table");
char stringValue3[] = "testString_3";

/*******************************************************************************************/




class MyThreadOM : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThreadOM(){

		isStop = false;
	}

	void AddOMImplementer(AdminOperationAsync_MyImplementer *p_user1){

		implementerOM = p_user1;
	}


	/**Destructor**/
	~MyThreadOM(){
	}


	/**This method is used to stop the thread**/
	void stop(){

		isStop=true;
	}


	/**svc method **/
	int svc(void){

		int ret;

		struct pollfd fds[1];

		fds[0].fd = implementerOM->getSelObj();
		fds[0].events = POLLIN;

		while(!isStop)
		{
			ret = poll(fds, 1, -1);
			if (ret == 0)
			{
				//cout<<"timeout off"<<endl;

			}else if (ret == -1){
				printf("poll error: \n");
			}else{

				//cout<<"OM FD THREAD: "<<fds[0].fd<<endl;

				if (fds[0].revents == POLLIN)
					implementerOM->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		return 0;
	}

private:

	AdminOperationAsync_MyImplementer 	*implementerOM;
	bool isStop;

};





//---------------------------------------------------------------------------------------------------------------------------//
/*******************************************Test invoke Admin Operation sync*************************************************/
//---------------------------------------------------------------------------------------------------------------------------//
void invokeAdminOperationTest1(){

	ACS_CC_ReturnType result;

	acs_apgcc_adminoperation admOp;

	const char *dnObjName1 = "object1,safApp=safImmService";
	int returnValueAction;
	long long int timeOutVal_5sec = 5*(1000000000LL);

	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;

	ACS_APGCC_AdminOperationParamType int32Par;
	ACS_APGCC_AdminOperationParamType unsInt32Par;
	ACS_APGCC_AdminOperationParamType int64Par;
	ACS_APGCC_AdminOperationParamType unsInt64Par;
	ACS_APGCC_AdminOperationParamType floatPar;
	ACS_APGCC_AdminOperationParamType doublePar;
	ACS_APGCC_AdminOperationParamType stringPar;
	ACS_APGCC_AdminOperationParamType nameTPar;
	ACS_APGCC_AdminOperationParamType stringPar2;
	ACS_APGCC_AdminOperationParamType stringPar3;


	/*create first Elemenet of param list*/
	char attName1[]= "int32Par";

	int32Par.attrName = attName1;
	int32Par.attrType = ATTR_INT32T;
	int32Par.attrValues = reinterpret_cast<void*>(&int32Value);

	/*create second Elemenet of param list*/
	char attName2[]= "unsInt32Par";

	unsInt32Par.attrName = attName2;
	unsInt32Par.attrType = ATTR_UINT32T ;
	unsInt32Par.attrValues = reinterpret_cast<void*>(&unsInt32Value);


	char attName3[]= "int64Par";

	int64Par.attrName = attName3;
	int64Par.attrType = ATTR_INT64T ;
	int64Par.attrValues = reinterpret_cast<void*>(&int64Value);


	char attName4[]= "unsInt64Par";

	unsInt64Par.attrName = attName4;
	unsInt64Par.attrType = ATTR_UINT64T ;
	unsInt64Par.attrValues = reinterpret_cast<void*>(&unsInt64Value);


	char attName5[]= "floatPar";

	floatPar.attrName = attName5;
	floatPar.attrType = ATTR_FLOATT ;
	floatPar.attrValues = reinterpret_cast<void*>(&floatValue);


	char attName6[]= "doublePar";

	doublePar.attrName = attName6;
	doublePar.attrType = ATTR_DOUBLET ;
	doublePar.attrValues = reinterpret_cast<void*>(&doubleValue);


	char attName7[]= "stringPar";

	stringPar.attrName = attName7;
	stringPar.attrType = ATTR_STRINGT ;
	stringPar.attrValues = reinterpret_cast<void*>(stringValue);


	char attName8[]= "nameTPar";

	nameTPar.attrName = attName8;
	nameTPar.attrType = ATTR_NAMET ;
	nameTPar.attrValues = reinterpret_cast<void*>(saNameValue);


	char attName9[]= "stringPar2";

	stringPar2.attrName = attName9;
	stringPar2.attrType = ATTR_STRINGT ;
	stringPar2.attrValues = reinterpret_cast<void*>(const_cast<char*>(stringValue2.c_str()));


	char attName10[]= "stringPar";

	stringPar3.attrName = attName10;
	stringPar3.attrType = ATTR_STRINGT ;
	stringPar3.attrValues = reinterpret_cast<void*>(stringValue3);


	/*insert parameter into the vector*/
	vectorIN.push_back(int32Par);
	vectorIN.push_back(unsInt32Par);
	vectorIN.push_back(int64Par);
	vectorIN.push_back(unsInt64Par);
	vectorIN.push_back(floatPar);
	vectorIN.push_back(doublePar);
	vectorIN.push_back(stringPar);
	vectorIN.push_back(nameTPar);
	vectorIN.push_back(stringPar2);
	vectorIN.push_back(stringPar3);


	result = admOp.init();
	if (result != ACS_CC_SUCCESS){
		std::cout <<"ERROR ::admOp.init()FAILED"<< std::endl;
		std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
		//return -1;
	}


	result = admOp.adminOperationInvoke(dnObjName1, 0, 1, vectorIN, &returnValueAction, timeOutVal_5sec);
	if (result != ACS_CC_SUCCESS){
		std::cout <<"ERROR ::admOp.adminOperationInvoke()FAILED"<< std::endl;
		std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
		//return -1;
	}else{
		std::cout <<"AdminOperation invoked with success"<< std::endl;
		std::cout <<"Value passed as parametes: "<< std::endl;
		std::cout << " attrName: " << attName1 << "\t value: " << int32Value << std::endl;
		std::cout << " attrName: " << attName2 << "\t value: " << unsInt32Value << std::endl;
		std::cout << " attrName: " << attName3 << "\t value: " << int64Value << std::endl;
		std::cout << " attrName: " << attName4 << "\t value: " << unsInt64Value << std::endl;
		std::cout << " attrName: " << attName5 << "\t value: " << floatValue << std::endl;
		std::cout << " attrName: " << attName6 << "\t value: " << doubleValue << std::endl;
		std::cout << " attrName: " << attName7 << "\t value: " << stringValue << std::endl;
		std::cout << " attrName: " << attName8 << "\t value: " << saNameValue << std::endl;
		std::cout << " attrName: " << attName9 << "\t value: " << stringValue2 << std::endl;
		std::cout << " attrName: " << attName10 << "\t value: " << stringValue3 << std::endl;

		std::cout << std::endl;
		std::cout << "Return Value action: " << returnValueAction <<std::endl;
	}


	result = admOp.finalize();
	if (result != ACS_CC_SUCCESS){
		std::cout <<"ERROR ::admOp.finalize()FAILED"<< std::endl;
		std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
		//return -1;
	}

}



//---------------------------------------------------------------------------------------------------------------------------//
/*******************************************Test invoke Admin Operation Async*************************************************/
//---------------------------------------------------------------------------------------------------------------------------//
void invokeAdminOperationAsyncTest1(){

	ACS_CC_ReturnType returnCode;

	AdminOperationAsync_MyImplementer objAsync;

	const char *dnObjName1 = "object1,safApp=safImmService";
	ACS_APGCC_AdminOperationIdType operationId = 2;
	ACS_APGCC_InvocationType invocation = 1;

	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;

	ACS_APGCC_AdminOperationParamType int32Par;
	ACS_APGCC_AdminOperationParamType unsInt32Par;
	ACS_APGCC_AdminOperationParamType int64Par;
	ACS_APGCC_AdminOperationParamType unsInt64Par;
	ACS_APGCC_AdminOperationParamType floatPar;
	ACS_APGCC_AdminOperationParamType doublePar;
	ACS_APGCC_AdminOperationParamType stringPar;
	ACS_APGCC_AdminOperationParamType nameTPar;
	ACS_APGCC_AdminOperationParamType stringPar2;
	ACS_APGCC_AdminOperationParamType stringPar3;


	/*create first Elemenet of param list*/
	char attName1[]= "int32Par";

	int32Par.attrName = attName1;
	int32Par.attrType = ATTR_INT32T;
	int32Par.attrValues = reinterpret_cast<void*>(&int32Value);

	/*create second Elemenet of param list*/
	char attName2[]= "unsInt32Par";

	unsInt32Par.attrName = attName2;
	unsInt32Par.attrType = ATTR_UINT32T ;
	unsInt32Par.attrValues = reinterpret_cast<void*>(&unsInt32Value);


	char attName3[]= "int64Par";

	int64Par.attrName = attName3;
	int64Par.attrType = ATTR_INT64T ;
	int64Par.attrValues = reinterpret_cast<void*>(&int64Value);


	char attName4[]= "unsInt64Par";

	unsInt64Par.attrName = attName4;
	unsInt64Par.attrType = ATTR_UINT64T ;
	unsInt64Par.attrValues = reinterpret_cast<void*>(&unsInt64Value);


	char attName5[]= "floatPar";

	floatPar.attrName = attName5;
	floatPar.attrType = ATTR_FLOATT ;
	floatPar.attrValues = reinterpret_cast<void*>(&floatValue);


	char attName6[]= "doublePar";

	doublePar.attrName = attName6;
	doublePar.attrType = ATTR_DOUBLET ;
	doublePar.attrValues = reinterpret_cast<void*>(&doubleValue);


	char attName7[]= "stringPar";

	stringPar.attrName = attName7;
	stringPar.attrType = ATTR_STRINGT ;
	stringPar.attrValues = reinterpret_cast<void*>(stringValue);


	char attName8[]= "nameTPar";

	nameTPar.attrName = attName8;
	nameTPar.attrType = ATTR_NAMET ;
	nameTPar.attrValues = reinterpret_cast<void*>(saNameValue);


	char attName9[]= "stringPar2";

	stringPar2.attrName = attName9;
	stringPar2.attrType = ATTR_STRINGT ;
	stringPar2.attrValues = reinterpret_cast<void*>(const_cast<char*>(stringValue2.c_str()));


	char attName10[]= "stringPar";

	stringPar3.attrName = attName10;
	stringPar3.attrType = ATTR_STRINGT ;
	stringPar3.attrValues = reinterpret_cast<void*>(stringValue3);


	/*insert parameter into the vector*/
	vectorIN.push_back(int32Par);
	vectorIN.push_back(unsInt32Par);
	vectorIN.push_back(int64Par);
	vectorIN.push_back(unsInt64Par);
	vectorIN.push_back(floatPar);
	vectorIN.push_back(doublePar);
	vectorIN.push_back(stringPar);
	vectorIN.push_back(nameTPar);
	vectorIN.push_back(stringPar2);
	vectorIN.push_back(stringPar3);


	returnCode = objAsync.init();
	if (returnCode != ACS_CC_SUCCESS){
		std::cout <<"ERROR ::objAsync.init()FAILED"<< std::endl;
		std::cout <<"ERROR CODE = "<< objAsync.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << objAsync.getInternalLastErrorText()<<std::endl;
		//return -1;
	}

	MyThreadOM th;
	th.AddOMImplementer(&objAsync);
	th.activate();


	returnCode = objAsync.adminOperationInvokeAsync(invocation, dnObjName1, 0, operationId, vectorIN);
	if (returnCode != ACS_CC_SUCCESS){
		std::cout <<"ERROR ::admOp.adminOperationInvoke()FAILED"<< std::endl;
		std::cout <<"ERROR CODE = "<< objAsync.getInternalLastError()<< "ERROR MESSAGE = " << objAsync.getInternalLastErrorText()<<std::endl;
		//return -1;
	}else{
		std::cout <<"AdminOperation invoked with success"<< std::endl;
		std::cout <<"Invocation type: "<< invocation <<std::endl;
		std::cout <<"Value passed as parametes: "<< std::endl;
		std::cout << " attrName: " << attName1 << "\t value: " << int32Value << std::endl;
		std::cout << " attrName: " << attName2 << "\t value: " << unsInt32Value << std::endl;
		std::cout << " attrName: " << attName3 << "\t value: " << int64Value << std::endl;
		std::cout << " attrName: " << attName4 << "\t value: " << unsInt64Value << std::endl;
		std::cout << " attrName: " << attName5 << "\t value: " << floatValue << std::endl;
		std::cout << " attrName: " << attName6 << "\t value: " << doubleValue << std::endl;
		std::cout << " attrName: " << attName7 << "\t value: " << stringValue << std::endl;
		std::cout << " attrName: " << attName8 << "\t value: " << saNameValue << std::endl;
		std::cout << " attrName: " << attName9 << "\t value: " << stringValue2 << std::endl;
		std::cout << " attrName: " << attName10 << "\t value: " << stringValue3 << std::endl;
	}

	sleep(10);

	th.stop();
	returnCode = objAsync.finalize();
	if (returnCode != ACS_CC_SUCCESS){
		std::cout <<"ERROR ::admOp.finalize()FAILED"<< std::endl;
		std::cout <<"ERROR CODE = "<< objAsync.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << objAsync.getInternalLastErrorText()<<std::endl;
		//return -1;
	}

}



void runSuite(){

	cute::suite s;

	s.push_back(CUTE(invokeAdminOperationTest1));

	s.push_back(CUTE(invokeAdminOperationAsyncTest1));

	cute::ide_listener lis;

	cute::makeRunner(lis)(s, "The Suite");
}



//--------------------------------------------------------------------------------------------------------------//
/*************************************MAIN***********************************************************************/
//--------------------------------------------------------------------------------------------------------------//
int main() {


	cout <<"-------------------------------------------------------------------------------------------------------"<<endl;
	cout <<"---------------------------------------START CUTE TEST-------------------------------------------------"<<endl;
	cout <<"-------------------------------------------------------------------------------------------------------"<<endl;

	runSuite();

	cout <<"---------------------------------------END CUTE TEST---------------------------------------------------"<<endl;





	return 0;
}






