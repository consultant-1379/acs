/*
 * ut_aeh_exception.cpp
 *
 *  Created on: Jun 27, 2011
 *      Author: xfabron
 */



#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <errno.h>


#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "acs_aeh_exception.h"


using namespace std;

void testgetErrorCode(){

	cout << " ---------------------------------------------------------------------------------------------" << endl;
	cout << " testgetErrorCode()" << endl;
	cout << " TEST: test the method getErrorCode" << endl;
	cout << " RESULT: the method return the error code passed as parameter in the constructor of class \n" << endl;
	cout << " -------------------------------------------------------------------------------------------- " << endl;

	acs_aeh_exception ex(SYSTEMERROR);

	errorType error = ex.getErrorCode();

	ASSERTM("CUTE: check value of getErroCode()",  error = SYSTEMERROR);

}


void testGetResultCode(){

	cout << " ---------------------------------------------------------------------------------------------" << endl;
	cout << " testGetResultCode()" << endl;
	cout << " TEST: test the method getResultCode" << endl;
	cout << " RESULT: the method return the linux error code passed as parameter in the constructor of class \n" << endl;
	cout << " -------------------------------------------------------------------------------------------- " << endl;

	acs_aeh_exception ex_1(GENERAL_FAULT, 23);

	errorType error = ex_1.getErrorCode();
	int lerrno = ex_1.getResultCode();

	ASSERTM("CUTE: check value of getErroCode()",  error = GENERAL_FAULT);
	ASSERT_EQUALM("CUTE: check value of getResultCode()",  lerrno, 23 );
}


void testDetailInfo(){

	cout << " ---------------------------------------------------------------------------------------------" << endl;
	cout << " testDetailInfo()" << endl;
	cout << " TEST: test the method detailInfo" << endl;
	cout << " RESULT: the method return the error message passed as parameter in the constructor of class \n" << endl;
	cout << " -------------------------------------------------------------------------------------------- " << endl;

	acs_aeh_exception ex_2(PARAMERROR, "Cute Test of class", 23);
	errorType error = ex_2.getErrorCode();
	int lerrno = ex_2.getResultCode();
	std::string errorString = ex_2.detailInfo();
	ASSERTM("CUTE: check value of getErroCode()",  error = PARAMERROR);
	ASSERT_EQUALM("CUTE: check value of getResultCode()",  lerrno, 23 );
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), "Cute Test of class") , 0);

}


void testCopyConstructor(){

	cout << " ---------------------------------------------------------------------------------------------" << endl;
	cout << " testCopyConstructor()" << endl;
	cout << " TEST: test the copy constructor of class " << endl;
	cout << " RESULT: the two object acs_aeh_exception are correctly initialized \n" << endl;
	cout << " -------------------------------------------------------------------------------------------- " << endl;

	acs_aeh_exception ex_2(PARAMERROR, "Cute Test of class", 23);
	errorType error = ex_2.getErrorCode();
	int lerrno = ex_2.getResultCode();
	std::string errorString = ex_2.detailInfo();
	ASSERTM("CUTE: check value of getErroCode()",  error = PARAMERROR);
	ASSERT_EQUALM("CUTE: check value of getResultCode()",  lerrno, 23 );
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), "Cute Test of class") , 0);


	acs_aeh_exception ex_3(ex_2);
	error = ex_3.getErrorCode();
	lerrno = ex_3.getResultCode();
	errorString = ex_3.detailInfo();
	ASSERTM("CUTE: check value of getErroCode()",  error = PARAMERROR);
	ASSERT_EQUALM("CUTE: check value of getResultCode()",  lerrno, 23 );
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), "Cute Test of class") , 0);

}



void testErrorText(){

	cout << " ---------------------------------------------------------------------------------------------" << endl;
	cout << " testErrorText()" << endl;
	cout << " TEST: test the method errorText " << endl;
	cout << " RESULT: the two object acs_aeh_exception are correctly initialized \n" << endl;
	cout << " -------------------------------------------------------------------------------------------- " << endl;

	std::string errorString;

	acs_aeh_exception ex_1(GENERAL_FAULT);
	errorString = ex_1.errorText();
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), GENERAL_FAULT_STR) , 0);

	acs_aeh_exception ex_2(INCORRECT_USAGE);
	errorString = ex_2.errorText();
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), INCORRECT_USAGE_STR) , 0);

	acs_aeh_exception ex_3(PARAMERROR);
	errorString = ex_3.errorText();
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), PARAMERROR_STR) , 0);

	acs_aeh_exception ex_4(SYSTEMERROR);
	errorString = ex_4.errorText();
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), SYSTEMERROR_STR) , 0);


	acs_aeh_exception ex_5(INTERNALERROR);
	errorString = ex_5.errorText();
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), INTERNALERROR_STR) , 0);

}


void testOperator(){

	cout << " ---------------------------------------------------------------------------------------------" << endl;
	cout << " testOperator()" << endl;
	cout << " TEST: test the overloading of operator << " << endl;
	cout << " RESULT: the two object acs_aeh_exception are correctly initialized \n" << endl;
	cout << " -------------------------------------------------------------------------------------------- " << endl;


	acs_aeh_exception ex(PARAMERROR, "Cute Test of class", 1);
	std::stringstream s;
	s << ex.errorText() <<": "<<ex.detailInfo()<<"\n"<<ex.strError(1);
	std::stringstream str;
	str << ex;
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(str.str().c_str(), s.str().c_str()) , 0);

}

void testOperator_2(){

	cout << " ---------------------------------------------------------------------------------------------" << endl;
	cout << " testOperator_2()" << endl;
	cout << " TEST: test the overloading of operator () " << endl;
	cout << " RESULT: the two object acs_aeh_exception are correctly initialized \n" << endl;
	cout << " -------------------------------------------------------------------------------------------- " << endl;

	acs_aeh_exception ex(PARAMERROR);

	errorType error = ex.getErrorCode();
	ex() << "CUTE TEST: Illegal value for parameter";
	std::string errorString = ex.detailInfo();

	ASSERTM("CUTE: check value of getErroCode()",  error = PARAMERROR);
	ASSERT_EQUALM("CUTE: check value of detailInfo()", strcmp(errorString.c_str(), "CUTE TEST: Illegal value for parameter") , 0);
}


void runSuite(){
	cute::suite s;

	s.push_back(CUTE(testgetErrorCode));
	s.push_back(CUTE(testGetResultCode));
	s.push_back(CUTE(testDetailInfo));
	s.push_back(CUTE(testCopyConstructor));
	s.push_back(CUTE(testErrorText));
	s.push_back(CUTE(testOperator));
	s.push_back(CUTE(testOperator_2));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "thi_is_errorimpl_Test");


}





int main() {


	cout << "Start Test acs_aeh_exception class" << endl;

	runSuite();

	cout << "End Test acs_aeh_exception class " << endl;

	return 0;
}
