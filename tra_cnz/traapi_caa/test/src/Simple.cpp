//============================================================================
// Name        : Prova_TRA.cpp
// Author      : XGIOPAP
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <iostream>
//#include "ACS_TRA_trace.h"
//#include <ctime>
//
//using namespace std;

/*int main()
{
	int i = 1;

	string m_class = "SimplePoint";
	string m_format = "C";

	//ACS_TRA_trace * objClass = new ACS_TRA_trace ( m_class, m_format);

	ACS_TRA_trace objClass = ACS_TRA_trace ( m_class, m_format );

	time_t now = time(NULL);

	tm *ltm = localtime(&now);

	cout << "before " << ltm->tm_sec << endl;

	string traOut = "traperf-test-1234567890-1234567890-1234567890-1234567890";

	while (i <= 10000000000 )
	{

		//if (objClass->ACS_TRA_ON())
		if ( objClass.isOn() )
		{
			//cout << "This is a long character string " << i << endl;
			objClass.trace ( traOut );
//			cout << "This is a long character string " << i << endl;
//			objClass.write(1, "This is a long character string");
//			cout << "This is a long character string " << i << endl;
//			objClass.write(1, "This is a long character string");
//			cout << "My Record Name" << objClass->getRecordName() << endl;

			//sleep(3);
		}

		i++;
	}

	now = time(NULL);

	ltm = localtime(&now);

	cout << "after " << ltm->tm_sec << endl;

	//delete objClass;

	return 0;
}*/


//#include "/vobs/cm4ap/ntacs/tra_cnz/traapi_caa/inc_ext/ACS_TRA_trace.h"
#include "ACS_TRA_trace.h"
//#include "acs_aeh_evreport.h"
//#include "ACS_APGCC_Util.H"
#include <iostream>
#include <sys/time.h>
#include <ace/ACE.h>
//#include "acs_prc_types.h"
//#include "acs_prc_api.h"

using namespace std;

//class Test
//{
//	int i;
//public:
//	Test(int ii){
//		i = ii;
//	}
//	~Test();
//	int  caluculation(int ii)
//	{
//		i = (ii * (ii-1));
//		return i;
//	}
//};

int main( int argc,char *argv[] )
{

	char str_to_trace[1024] = {0};
	string tra_class = argv[1];

	ACS_TRA_trace* objTrace1 = new ACS_TRA_trace(tra_class, "C5000");

	for ( unsigned long long i=0; i < 100000; i++){

		snprintf ( str_to_trace, sizeof(str_to_trace), "Trace %lli", i );

		if(objTrace1->ACS_TRA_ON()){
			objTrace1->ACS_TRA_event(1, str_to_trace );
			cout << "TRA is ON ==> i : " << i << endl;
		}
		else{
			cout << "TRA is OFF ==> i : " << i << endl;
		}

		memset ( str_to_trace, 0, sizeof(str_to_trace));
		usleep(10000);
	}


//	struct timeval tv;
//	struct timezone tz;
//	struct tm *tm;
//	gettimeofday(&tv, &tz);
//	tm=localtime(&tv.tv_sec);
//	char C[100]= "TEST isON  IMplementation";
//
//    // ********************************
//
//	std::cout<<"At the Start, the Time is :";
//	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//	for (unsigned i=0; i<1000000000; i++){
//		if(objTrace1->ACS_TRA_ON())
//			objTrace1->ACS_TRA_event(1, C);
//	}
//
//	std::cout<<"After looping 1.000.000.000 of ACS_TRA_trace::ACS_TRA_ON()..." << std::endl;
//	std::cout<<"At the End, the Time is :";
//	gettimeofday(&tv, &tz);
//    tm=localtime(&tv.tv_sec);
//    printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//    // ********************************
//
//	std::cout<<"At the Start, the Time is :";
//	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//	for (unsigned i=0; i<1000000000; i++){
//		if(objTrace1->ACS_TRA_ON_2())
//			objTrace1->ACS_TRA_event(1, C);
//	}
//
//	std::cout<<"After looping 1.000.000.000 of ACS_TRA_trace::ACS_TRA_ON_2() inline..." << std::endl;
//	std::cout<<"At the End, the Time is :";
//	gettimeofday(&tv, &tz);
//	tm=localtime(&tv.tv_sec);
//	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//
//
//	// ********************************
//
//	std::cout<<"At the Start, the Time is :";
//	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//	for (unsigned i=0; i<1000000000; i++){
//		if(objTrace1->isOn())
//			objTrace1->ACS_TRA_event(1, C);
//	}
//
//	std::cout<<"After looping 1.000.000.000 of ACS_TRA_trace::ACS_TRA_ON()..." << std::endl;
//	std::cout<<"At the End, the Time is :";
//	gettimeofday(&tv, &tz);
//	tm=localtime(&tv.tv_sec);
//	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//	// ********************************
//
//	std::cout<<"At the Start, the Time is :";
//	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//	for (unsigned i=0; i<1000000000; i++){
//		if(objTrace1->isOn_2())
//			objTrace1->ACS_TRA_event(1, C);
//	}
//
//	std::cout<<"After looping 1.000.000.000 of ACS_TRA_trace::ACS_TRA_ON_2() inline..." << std::endl;
//	std::cout<<"At the End, the Time is :";
//	gettimeofday(&tv, &tz);
//	tm=localtime(&tv.tv_sec);
//	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
//
//
//
//    // ********************************
//
////	std::cout<<"At the Start, the Time is :";
////	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////	for (unsigned i=0; i<1000000000; i++){
////		getpid();
////	}
////
////	std::cout<<"After looping 1.000.000.000 of getpid()..." << std::endl;
////	std::cout<<"At the End, the Time is :";
////	gettimeofday(&tv, &tz);
////    tm=localtime(&tv.tv_sec);
////    printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////    // ********************************
////
////    ACS_PRC_API pippo;
////	std::cout<<"At the Start, the Time is :";
////	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////	for (unsigned i=0; i<1000000000; i++){
////		pippo.isRebootPossible ( manualRebootInitiated );
////	}
////
////	std::cout<<"After looping 1.000.000.000 of ACS_PRC_API::isRebootPossible()..." << std::endl;
////	std::cout<<"At the End, the Time is :";
////	gettimeofday(&tv, &tz);
////	tm=localtime(&tv.tv_sec);
////	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////    // ********************************
////
////	std::cout<<"At the Start, the Time is :";
////	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////	for (unsigned i=0; i<1000000000; i++){
////		ACE_OS::last_error();
////	}
////
////	std::cout<<"After looping 1.000.000.000 of ACE_OS::last_error()..." << std::endl;
////	std::cout<<"At the End, the Time is :";
////	gettimeofday(&tv, &tz);
////	tm=localtime(&tv.tv_sec);
////	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////    // ********************************
////
////	acs_aeh_evreport pippo2;
////	std::cout<<"At the Start, the Time is :";
////	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////	for (unsigned i=0; i<1000000000; i++){
////		pippo2.getError();
////	}
////
////	std::cout<<"After looping 1.000.000.000 of acs_aeh_evreport::getError()..." << std::endl;
////	std::cout<<"At the End, the Time is :";
////	gettimeofday(&tv, &tz);
////	tm=localtime(&tv.tv_sec);
////	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////
////    // ********************************
////
////	std::cout<<"At the Start, the Time is :";
////	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////	for (unsigned i=0; i<1000000000; i++){
////		gettimeofday(&tv, &tz);
////	}
////
////	std::cout<<"After looping 1.000.000.000 of gettimeofday ..." << std::endl;
////	std::cout<<"At the End, the Time is :";
////	gettimeofday(&tv, &tz);
////	tm=localtime(&tv.tv_sec);
////	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////    // ********************************
////
////	std::cout<<"At the Start, the Time is :";
////	printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
////
////	for (unsigned i=0; i<1000000000; i++){
////	}
////
////	std::cout<<"After looping 1.000.000.000 of sprintf ..." << std::endl;
////	std::cout<<"At the End, the Time is :";
////	gettimeofday(&tv, &tz);
////	tm=localtime(&tv.tv_sec);
////	printf(" %d:%02d:%02d %d \n\n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);


	return 0;
}


