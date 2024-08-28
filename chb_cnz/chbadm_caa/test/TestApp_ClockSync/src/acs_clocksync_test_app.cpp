/*=================================================================== */
/**
@file acs_clocksync_test_app.cpp

This file contains the implementation of the test application.

@version 1.0.0

@copyright Ericsson AB, Sweden 2010. All rights reserved.

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       01/21/2011     XCHEMAD        APG43 on Linux.
**/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_JTP.h"

#include <iostream>
using namespace std;

#include "sys/select.h"
#include "time.h"

/*===================================================================
   ROUTINE: Server
		    This routine acts as a destinator in a communication.
=================================================================== */
int Server()
{
	char* ServiceName = const_cast<char*>("CLOCKSYN");
	cout<< "\nCreating a ACS_JTP_Service object\n";
	// Create a JTP_Service object with name
	ACS_JTP_Service S1(ServiceName);

	ACS_JTP_Conversation myC1;

	cout << "\nACS_JTP_Service object created successfully\n";

	//Register the application with DSD Daemon.
	if (ServiceName != 0)
	{
		cout << "\nRegistering application " << ServiceName << " with DSD\n";
	}

	cout << "\nCalling jidrepreq \n";

	if (S1.jidrepreq() == false)
	{
		cout << "\njidrepreq Failed\n";
		return -1;
	}
	cout << "\njidrepreq is successful\n";

	if( ServiceName != 0)
	{
		cout << "\nService: " <<  ServiceName <<" is successfully registered\n";
	}

	while(1)
	{	

	    cout<<"Waiting for the requests"<<endl;
	    S1.accept(0, &myC1);
	    if (myC1.State() == ACS_JTP_Conversation::StateConnected)
	    {
		cout << "\nNew conversation request has arrived\n";

		unsigned short U1 =0 , U2 = 0, R=0;

		//Fetch jexinitind.
		cout << "\nFetching jexinitind\n";

		if ( myC1.jexinitind(U1, U2) == false)
		{
			cout << "\njexinitind failed C1\n";
			return -1;
		}

		if (myC1.jexinitrsp(U1, U2, R) == false)
		{
			cout << "\njexinitrsp failed C1\n";
			cout << "\nR= "<< R << "\n";
			return -1;
		}

	     while( 1)
	     {

		unsigned short RLen = 100;
		char* RMsg = 0;

	        cout<<"Waiting for a request"<<endl;
		if (myC1.jexdataind(U1, U2, RLen, RMsg) == false)
		{
			myC1.jexdiscind(U1, U2, R);
			cout << "(jexdiscind) the conversation broke down" << endl;
			cout << "jexdatareq was expected, " << endl;
			cout << "R = " << R << endl;
			return -1;
		}

		cout << "\nSending jexdatareq\n";
		char timeBuffer[100];
                memset(&timeBuffer, 0, sizeof(timeBuffer));


	        time_t sysTime;
                struct timeval tv;
                gettimeofday(&tv, NULL);
		sysTime = time(NULL);
                struct tm * timeStruct;
                timeStruct = localtime(&sysTime);

		int usec = tv.tv_usec;
		unsigned char sec1, sec2, sec3, sec4;
		sec1 = (char)(usec & 0xFF);
		sec2 = (char)(usec>>8 & 0xFF);
		sec3 = (char)(usec>>16 & 0xFF);
		sec4 = (char)(usec>>24 & 0xFF);
		cout<<"Current system time is "<<tv.tv_sec<<" "<<tv.tv_usec<<endl;
                timeBuffer[0] = (char)(timeStruct->tm_year);
                timeBuffer[1] = (char)(timeStruct->tm_mon);
                timeBuffer[2] = (char)(timeStruct->tm_mday);

                timeBuffer[3] = (char)(timeStruct->tm_hour);
                timeBuffer[4] = (char)(timeStruct->tm_min) - 5;
                timeBuffer[5] = (char)(timeStruct->tm_sec);
                timeBuffer[6] = usec/10000;
		int timeBufferLen = sizeof(timeBuffer) + 1;
		char* SMsg = 0;
		SMsg = new char[100];
		memset(SMsg, 0, 100);
		memcpy(SMsg, timeBuffer, timeBufferLen);



		cout<<"Sending the requested data"<<endl;
		if (myC1.jexdatareq(U1, U2, timeBufferLen+1, SMsg) == false)
		{
			myC1.jexdiscind(U1, U2, R);
			cout << "(jexdiscind) the conversation broke down" << endl;
			return -1;
		}

	    }
	}
	}
	return 0;

}

/*===================================================================
   ROUTINE: main
		    This is the main routine
=================================================================== */
int main()
{
	cout<<"Initiating Destinator to fetch CP data"<<endl;
	int ret_status = Server();
	cout<<"Return status is "<<ret_status<<endl;
	return 0;
}

