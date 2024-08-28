//============================================================================
// Name        : Prova_TRA.cpp
// Author      : XGIOPAP
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "ACS_TRA_Logging.h"
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
//	string Subsystem;
//
//	cout<<"DEMO"<<endl;
//
//	Subsystem = argv[1];
//	Demo *m_demo = new Demo( Subsystem );
//	//ACS_TRA_LogWatch *MyLog = new ACS_TRA_LogWatch();
//	m_demo->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,1000);
//	//MyLog->activate();
//
//	while (1)
//	{
//		//n'adda muri'
//	}

	ACS_TRA_Logging* log1 = new ACS_TRA_Logging();
	ACS_TRA_Logging* log2 = new ACS_TRA_Logging();
	ACS_TRA_Logging* log3 = new ACS_TRA_Logging();
	ACS_TRA_Logging* log4 = new ACS_TRA_Logging();

	log1->Open("ABC");
	log2->Open("DEF");
	log3->Open("GHI");
	log4->Open("LMN");

	delete log1;
	delete log2;
	delete log3;
	delete log4;

	cout << " wait " << endl;
	sleep(10);

	cout << " create " << endl;

	log3 = new ACS_TRA_Logging();

	log3->Open("xxxx");

	sleep(10);

	return 0;
}
