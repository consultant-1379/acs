//============================================================================
/** @file ut_drainer.cpp
 *	@brief 
 *  @author xanttro
 *	@date Jun 29, 2011
 *  @version
 *	
 *  COPYRIGHT Ericsson AB, 2011
 *	All right reserved.
 *
 *
 *
 *
 *
 *
 */

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "string"

#include "acs_alog_drainer.h"

using namespace std;

void test_logData_1() {
	cout <<"=================================================" << endl;
	cout <<" test_logData_1()" << endl;
	cout <<" TEST: send a text message to the event pipe throw the communicationhandler class." << endl;
	cout <<" CALLED METHOD: logData()." << endl;
	cout <<" RESULT: the text message is correctly stored in event log" <<endl;

	acs_alog_Drainer drainer;
	acs_alog_returnType returnValue;

	returnValue = drainer.logData(not_defined,"test_logData_1","","CUTE test");

	ASSERT_EQUALM("check retCode for logData() method",acs_alog_ok,returnValue );

}


void test_logData_2() {
	cout <<"=================================================" << endl;
		cout <<" test_logData_2()" << endl;
		cout <<" TEST: send a test message with a throble date." << endl;
		cout <<" CALLED METHOD: logData()." << endl;
		cout <<" RESULT: the text message is correctly stored in event log" <<endl;

		acs_alog_Drainer drainer;
		acs_alog_returnType returnValue;

		returnValue = drainer.logData(not_defined,"test_logData_2","","CUTE test",0,"2011-07-22"," 221221");

		ASSERTM("retCode for logData() method is acs_alog_ok",(acs_alog_ok == returnValue) );

}



void test_append_commit_Data_1() {
	cout <<"=================================================" << endl;
	cout <<" test_append_commit_Data_1()" << endl;
	cout <<" TEST: send a text message in a buffer and later write it in the event pipe" << endl;
	cout <<" CALLED METHOD: appendData(), commit()." << endl;
	cout <<" RESULT: the text message is correctly stored in the event pipe." <<endl;

	acs_alog_Drainer drainer;
	acs_alog_returnType returnValue;
	drainer.appendData("test_append_commit_Data_1");
	returnValue = drainer.commit(not_defined,"","CUTE test");

	ASSERT_EQUALM("check retCode for commit() method",acs_alog_ok,returnValue );
}

void test_rollback_1() {
	cout <<"=================================================" << endl;
	cout <<" test_rollback_1()" << endl;
	cout <<" TEST: cancel the message in the queue writed with the append method " << endl;
	cout <<" CALLED METHOD: appendData(),rollback(), commit()." << endl;
	cout <<" RESULT: find in the stored event pipe, the second message and not the first." <<endl;

	acs_alog_Drainer drainer;
	acs_alog_returnType returnValue;


	drainer.appendData("test_appendData_1 first message");
	drainer.rollback();

	drainer.appendData("test_appendData_1 second message");
	returnValue = drainer.commit(not_defined,"","CUTE test");

	ASSERT_EQUALM("check retCode for commit() method",acs_alog_ok,returnValue );
	cout<<endl;
	cout<< "ATTENTION!! Verify in the stored event pipe if you find only the message 'test_appendData_1 second message' " <<endl;
}

void test_newLoggingSession_1() {
	cout <<"=================================================" << endl;
	cout <<" test_newLoggingSession_1()" << endl;
	cout <<" TEST: cancel the message in the queue writed with the append method " << endl;
	cout <<" CALLED METHOD: appendData(),newLoggingSession(), commit()." << endl;
	cout <<" RESULT: find in the stored event pipe, the second message and not the first." <<endl;

	acs_alog_Drainer drainer;
	acs_alog_returnType returnValue;


	drainer.appendData("test_appendData_1 first message");
	drainer.newLoggingSession();

	drainer.appendData("test_appendData_1 second message");
	returnValue = drainer.commit(not_defined,"","CUTE test");

	ASSERT_EQUALM("check retCode for commit() method",acs_alog_ok,returnValue );
	cout<<endl;
	cout<< "ATTENTION!! Verify in the stored event pipe if you find only the message 'test_appendData_1 second message' " <<endl;
}


void runSuite(){
	cute::suite s;

	s.push_back(CUTE(test_logData_1));
	s.push_back(CUTE(test_logData_2));
	s.push_back(CUTE(test_append_commit_Data_1));
	s.push_back(CUTE(test_rollback_1));
	s.push_back(CUTE(test_newLoggingSession_1));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test");
}


int main() {
	cout << "START Test acs_alog_drainer Class" << endl;
	runSuite();
	cout << "STOP Test acs_alog_drainer Class" << endl;
	return 0;
}
