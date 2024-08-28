//============================================================================
/** @file ut_communicationhandler.cpp
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

#include "acs_alog_communicationhandler.h"

using namespace std;

/**
 * first test case.
 * Send event message to a Pipe
 * The event correctly stored in pipe.
 *
 */
void test_sendMsg_1(){
	cout <<"=================================================" << endl;
	cout <<" test_sendMesg_1()" << endl;
	cout <<" TEST: send a text message to the syslog input pipe." << endl;
	cout <<" CALLED METHOD: open(); sendMsg(),close(). " << endl;
	cout <<" RESULT: the text message is correctly stored in event log" <<endl;

	int retCode = -1;
	acs_alog_communicationhandler comm;

	retCode = comm.open();
	ASSERT_EQUALM("check retCode for open() method",0,retCode );
	retCode = comm.sendMsg("test send Mesg 1, OK ");
	ASSERT_EQUALM("check retCode for sendMsg() method",0,retCode );
	retCode = comm.close();
	ASSERT_EQUALM("check retCode for close() method",0,retCode);

}

void test_sendMsg_2() {
	cout <<"=================================================" << endl;
	cout <<" test_sendMesg_2()" << endl;
	cout <<" TEST: send a text message without call open and close method" << endl;
	cout <<" CALLED METHOD: sendMsg() " << endl;
	cout <<" RESULT: Error code returned" <<endl;

	int retCode = -1;
	acs_alog_communicationhandler comm;


	retCode = comm.sendMsg(" UNIT Test; test sendMesg_2 " );
	ASSERT_EQUALM("check retCode for sendMsg() method",-1,retCode );

	cout << "Message not send" << endl;
	cout << "error code: "<< comm.getErrorCode() << endl;
	cout << "error Desc: "<< comm.getErrorText() << endl;
}

void test_sendMsg_3() {
	cout <<"=================================================" << endl;
	cout <<" test_sendMesg_3()" << endl;
	cout <<" TEST: send a text message with dimension over the max size allow" << endl;
	cout <<" CALLED METHOD: sendMsg() " << endl;
	cout <<" RESULT: Error code returned" <<endl;

	int retCode = -1;
	acs_alog_communicationhandler comm;

	int size = 8000;

	char errorTestString[size];

	for (int i = 0;i<size - 1; i++)
	{
		errorTestString[i] = '0';
	}
	retCode = comm.open();
	ASSERT_EQUALM("check retCode for open() method",0,retCode );
	retCode = comm.sendMsg(errorTestString);
	ASSERT_EQUALM("check retCode for sendMsg() method",-1,retCode );
	retCode = comm.close();
	ASSERT_EQUALM("check retCode for close() method",0,retCode);

	cout << "error code: "<< comm.getErrorCode() << endl;
	cout << "error Desc: "<< comm.getErrorText() << endl;

}

void test_close_1() {
	cout <<"=================================================" << endl;
	cout <<" test_close_1()" << endl;
	cout <<" TEST: call close method without call open method" << endl;
	cout <<" CALLED METHOD: close() " << endl;
	cout <<" RESULT: Error code returned" <<endl;

	int retCode = -1;
	ACE_HANDLE handle;
	acs_alog_communicationhandler comm;
	handle = comm.getHandle();
	retCode = comm.close();
	ASSERT_EQUALM("chech retCode for close method",-1,retCode);
	cout << "error code: "<< comm.getErrorCode() << endl;
	cout << "error Desc: "<< comm.getErrorText() << endl;
}



void runSuite(){
	cute::suite s;

	s.push_back(CUTE(test_sendMsg_1));
	s.push_back(CUTE(test_sendMsg_2));
	s.push_back(CUTE(test_sendMsg_3));
	s.push_back(CUTE(test_close_1));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test");
}


int main() {
	cout << "START Test acs_alog_communicationhandler Class" << endl;
	runSuite();
	cout << "STOP Test acs_alog_communicationhandler Class" << endl;
	return 0;
}
