//============================================================================
// Name        : ut_aeh_error.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "testErrorClass.h"

using namespace std;



void test_setGetError_1(){

	testErrorClass error;

	error.mySetError(ACS_AEH_noErrorType);
	ASSERT(error.getError() == ACS_AEH_noErrorType);

	error.mySetError(ACS_AEH_eventDeliveryFailure);
	ASSERT(error.getError() == ACS_AEH_eventDeliveryFailure);

	error.mySetError(ACS_AEH_syntaxError);
	ASSERT(error.getError() == ACS_AEH_syntaxError);

	error.mySetError(ACS_AEH_genericError);
	ASSERT(error.getError() == ACS_AEH_genericError);

}

void test_setGetError_2(){


	testErrorClass error;
	char* errorMsg = const_cast<char*>("CUTE TEST: message error");

	error.mySetTextError("CUTE TEST: system error");

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(error.getErrorText(), "CUTE TEST: system error") , 0);

	error.mySetTextError(errorMsg);

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(error.getErrorText(), errorMsg) , 0);



}


void test_setGetError_3(){

	testErrorClass error;

	char* errorMsg = const_cast<char*>("message error");
	char* prefix = const_cast<char*>("CUTE TEST");
	ostringstream ostr;

	error.mySetTextError(prefix, errorMsg);

	ostr << prefix << " : " << errorMsg;

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(error.getErrorText(), ostr.str ().c_str()) , 0);


}


void test_setGetError_4(){

	testErrorClass error;

	char* errorMsg = const_cast<char*>("message error");
	char* prefix = const_cast<char*>("CUTE TEST");
	char* suffix = const_cast<char*>("TEST_4");
	ostringstream ostr;

	error.mySetTextError(prefix, errorMsg, suffix);

	ostr << prefix << " : " << errorMsg<< " - "<<suffix;

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(error.getErrorText(), ostr.str ().c_str()) , 0);


}


void test_setGetError_5(){

	testErrorClass error;

	char* errorMsg = const_cast<char*>("message error");
	char* prefix = const_cast<char*>("CUTE TEST");
	long suffix = 200;
	ostringstream ostr;

	error.mySetTextError(prefix, errorMsg, suffix);

	ostr << prefix << " : " << errorMsg<< " - "<<suffix;

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(error.getErrorText(), ostr.str ().c_str()) , 0);


}

void test_copyConstructor(){

	testErrorClass error;

	error.mySetError(ACS_AEH_noErrorType);
	ASSERT(error.getError() == ACS_AEH_noErrorType);

	testErrorClass errorcopy(error);
	ASSERT(errorcopy.getError() == ACS_AEH_noErrorType);

}

void test_operator(){

	testErrorClass error;
	error.mySetError(ACS_AEH_syntaxError);
	ASSERT(error.getError() == ACS_AEH_syntaxError);

	error.mySetTextError("CUTE TEST: system error");
	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(error.getErrorText(), "CUTE TEST: system error") , 0);

	acs_aeh_error error_1;

	testErrorClass *errorPtr=&error;

	error_1 = *(static_cast<acs_aeh_error *>(errorPtr));

	ASSERT(error_1.getError() == ACS_AEH_syntaxError);
	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(error_1.getErrorText(), "CUTE TEST: system error") , 0);

}


void runSuite(){
	cute::suite s;
	//TODO add your test here

	s.push_back(CUTE(test_setGetError_1));
	s.push_back(CUTE(test_setGetError_2));
	s.push_back(CUTE(test_setGetError_3));
	s.push_back(CUTE(test_setGetError_4));
	s.push_back(CUTE(test_setGetError_5));
	s.push_back(CUTE(test_copyConstructor));
	s.push_back(CUTE(test_operator));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "thi_is_errorimpl_Test");


}


int main() {


	cout << "Start Test acs_aeh_error class" << endl;
	runSuite();
	cout << "End Test acs_aeh_error class " << endl;
	return 0;
}
