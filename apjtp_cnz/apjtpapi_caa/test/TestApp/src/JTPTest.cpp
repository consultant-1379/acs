/*=================================================================== */
/**
@file JTPTest.cpp

This file contains the implementation of the test application.

@version 1.0.0

@documentno CAA 109 0870

@copyright Ericsson AB, Sweden 2010. All rights reserved.

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       08/10/2010     XTANAGG        APG43 on Linux.
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

//#include "ace/Log_Msg.h"
//#include "ace/ACE.h"
//#include "ace/OS_NS_sys_select.h"

/*===================================================================
   ROUTINE: Help
=================================================================== */
void Help()
{
	cout << "Please use the format as mentioned below\n";
	cout << "For destinator s = Server ; Format: s ServiceName\n";
	cout << "Initiator JOB j = JOB ; Format: j ServiceName MaxBufferLength \n";
	cout << "Initiator CONVERSATION c = CONVERSATION ; Format: c ServiceName MaxBufferLength \n";
}

/*===================================================================
   ROUTINE: Server
		    This routine acts as a destinator in a communication.
=================================================================== */
int Server(char* ServiceName)
{
	cout<< "\nCreating a ACS_JTP_Service object\n";
	// Create a JTP_Service object with name
	ACS_JTP_Service S1(ServiceName);

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

	//Create handles.
	int NoOfFd = 4;
	JTP_HANDLE *Fd = new JTP_HANDLE[4];

	//Get the handles for waiting on incoming message.
	cout << "\nGet the handles for waiting on incoming messages\n";
	S1.getHandles(NoOfFd, Fd);

    cout << "\nNo. Of Handles = " << NoOfFd << "\n";

	if( NoOfFd == 0)
	{
		cout << "\nHandle count is zero\n";
		delete[] Fd;
		Fd = 0;
		return -1;
	}

	//Create a FD set.
	fd_set readHandleSet;
	FD_ZERO(&readHandleSet);

	FD_SET (Fd[0], &readHandleSet);
	FD_SET (Fd[1], &readHandleSet);
	FD_SET (Fd[2], &readHandleSet);
	FD_SET (Fd[3], &readHandleSet);

	struct timeval tv;
	tv.tv_sec = 100;
	tv.tv_usec = 0;
	
	JTP_HANDLE maxFd  = -1;	

	maxFd = Fd[0];
	for( int i =1 ; i < 4 ; i++)
	{
		if( maxFd < Fd[i] )
		{
			maxFd = Fd[i];
		}
	}
		
	int ret = select (((int)( maxFd + 1)), &readHandleSet, 0, 0, &tv);
	if ( ret == 0)
	{
		cout << "\nNo request came within 100 seconds\n";
		delete[] Fd;
		Fd = 0;
		return -1;
	}
	else if ( ret > 0)
	{
		if ( FD_ISSET(Fd[0], &readHandleSet) || FD_ISSET(Fd[1], &readHandleSet) || FD_ISSET(Fd[2], &readHandleSet) || FD_ISSET(Fd[3], &readHandleSet))
		{
			cout << "\nRequest came from client\n";
		}
		else
		{
			cout << "\nNo Handle is set in select..\n";
			delete[] Fd;
			Fd = 0;
			return -1;
		}
	}
	else if ( ret < 0)
	{
		cout << "\nError occurred in select..\n";
		delete[] Fd;
		Fd = 0;
		return -1;
	}
	//Create an object for incoming job or conversation
	cout << "\nCreating a job object\n";
	ACS_JTP_Job myJ1;

	cout << "\nCreating a conversation object\n";
	ACS_JTP_Conversation myC1;

	cout << "\nCalling accept to accept an incoming call from initiator.\n";
	//Accept an incoming call from remote side.
	S1.accept(&myJ1, &myC1);

	//Check if it is a job.
	if (myJ1.State() == ACS_JTP_Job::StateConnected)
	{
		cout << "\nNew job request has arrived\n";
		unsigned short U1=0, U2=0, Len=0, R= 0;
		char* Msg = 0;
		//Fetch jinitind.
		cout << "\nFetching jinitind\n";

		if ( myJ1.jinitind(U1, U2, Len, Msg) == false)
		{
			cout << "\njinitind failed\n";
			delete[] Fd;
			Fd = 0;
			return -1;

		}
		//It could be response to jinitreq. Fetch jinitrsp.
		cout << "\nSending jinitrsp\n";
		if (myJ1.jinitrsp(U1, U2, 0) == false)
		{
			cout << "\njinitrsp failed\n";
			delete[] Fd;
			Fd = 0;
			return -1;
		}


		char* msg = (char*) new (std::nothrow) char[strlen("test") + 1];
		memset(msg,0, strlen("test") + 1);
		strcpy(msg,"test");

		Len = strlen("test");
		cout << "\nSending jresultreq\n\n";
		//Send jresultreq.

		if ( myJ1.jresultreq(U1, U2, R, Len, msg) == false)
		{
			cout << "\njresultreq failed\n";
			delete[] msg;
			msg = 0;
			delete[] Fd;
			Fd = 0;
			return -1;
		}

		delete[] msg;
		msg = 0;
	}

	//Check if it is a conversation.
	if (myC1.State() == ACS_JTP_Conversation::StateConnected)
	{
		cout << "\nNew conversation request has arrived\n";

		unsigned short U1 =0 , U2 = 0, R=0;

		//Fetch jexinitind.
		cout << "\nFetching jexinitind\n";

		if ( myC1.jexinitind(U1, U2) == false)
		{
			cout << "\njexinitind failed C1\n";
			delete[] Fd;
			Fd = 0;
			return -1;
		}

		if (myC1.jexinitrsp(U1, U2, R) == false)
		{
			cout << "\njexinitrsp failed C1\n";
			cout << "\nR= "<< R << "\n";
			delete[] Fd;
			Fd = 0;
			return -1;
		}

		cout << "\nRetrieving Handle for conversation\n";
		JTP_HANDLE handle1 = myC1.getHandle();

		//Create a FD set.
		struct timeval tv1;
		tv1.tv_sec = 5;
		tv1.tv_usec = 0;
		fd_set readHandleSet;
		FD_ZERO(&readHandleSet);
		FD_SET (handle1, &readHandleSet);

		cout << "\nWaiting on handle for incoming jexdatareq\n";
		int ret = select ((int)(handle1+1), &readHandleSet, 0, 0, &tv1);

		//Check for timeout.
		if (ret >= 0)
		{
			if ( !FD_ISSET(handle1, &readHandleSet))
			{
				myC1.jexdiscind(U1, U2, R);
				cout << "(jexdiscind) the conversation broke down" << endl;
				cout << "jexdatareq, was expected, " << endl;
				cout << "R = " << R << endl;
				delete[] Fd;
				Fd = 0;
				return -1;
			}
			else
			{

				cout << "\nFetching jexdataind for jexdatareq\n";
			}
		}
		else
		{
			myC1.jexdiscind(U1, U2, R);
			cout << "(jexdiscind) the conversation broke down" << endl;
			cout << "jexdatareq, was expected, " << endl;
			cout << "R = " << R << endl;
			delete[] Fd;
			Fd = 0;
			return -1;
		}

		unsigned short RLen = 0;
		char *RMsg = 0;
		if (myC1.jexdataind(U1, U2, RLen, RMsg) == false)
		{
			myC1.jexdiscind(U1, U2, R);
			cout << "(jexdiscind) the conversation broke down" << endl;
			cout << "jexdatareq was expected, " << endl;
			cout << "R = " << R << endl;
			delete[] Fd;
			Fd = 0;
			return -1;
		}

		cout << "\nSending jexdatareq\n";

		if (myC1.jexdatareq(U1, U2, RLen, RMsg) == false)
		{
			myC1.jexdiscind(U1, U2, R);
			cout << "(jexdiscind) the conversation broke down" << endl;
			delete[] Fd;
			Fd = 0;
			return -1;
		}

	}
	delete[] Fd;
	Fd = 0;
	return 0;

}

/*===================================================================
   ROUTINE: Job
		    This routine acts as an initiator for communication in job mode.
=================================================================== */
int Job(char* ServiceName, unsigned short MsgSize)
{
	char*	Msg = 0;
	char*	RMsg = 0;
	unsigned short  U1=0, U2=0, R=0, RU1=0, RU2=0, Len=0, RLen=0;

	U1 = 10;
	U2 = 11;

	//Max buffer length.
	//Allocate memory for the buffer.
	Msg = (char*) new (std::nothrow) char[strlen("Test message") + 1];

	Len  = strlen("Test message") + 1;

	//Initialize the message buffer.
	memset(Msg, 0, Len);

	strcpy(Msg,"Test message");

	cout << "\nCreating a job object with Service Name:" << ServiceName << " and Maximum Buffer Size:" << MsgSize << "\n";

	// Create a JTP_Job object with service name and maxbuffer
	ACS_JTP_Job J1(ServiceName, MsgSize);

	cout << "\nSetting the timeout to 3 seconds\n";
	// Override the default timeout which is 5 seconds
	J1.setTimeOut(3);

	cout << "\nSetting the number of retries to 2 seconds\n";
	// Override the default no of tries.
	J1.setNoOfTries(2);

	cout << "\nSetting the default time between delays to 1 second\n";
	// Override the default time between retries (which is 0 seconds)
	J1.setDelayTime(1);

	//Create vector for reachable nodes.
	std::vector<ACS_JTP_Job::JTP_Node> nodes;

	//Create vector for unreachable nodes.
	std::vector<ACS_JTP_Job::JTP_Node> notReachNodes;

	cout << "\nCalling query method to get the reachable and unreachable nodes from DSD\n";
	//Find out the set on reachable and unreachable nodes.
	J1.query( ACS_JTP_Job::SYSTEM_TYPE_AP, nodes, notReachNodes);

	cout << "\nPrinting the resulting of query method\n";
	//printing the set of reachable nodes.
	for ( unsigned int ctr = 0 ; ctr < nodes.size() ; ctr++)
	{
		cout << "\nReachable Node:"<< ctr << "\n";
		cout << "Node.system_id = "<< nodes[ctr].system_id << "\n";
		cout << "Node.system_type = "<< nodes[ctr].system_type << "\n";
		cout << "Node.node_state = " << nodes[ctr].node_state << "\n";
		cout << "Node.node_name = " << nodes[ctr].node_name << "\n";
		cout << "Node.system_name = " << nodes[ctr].system_name << "\n";
		cout << "Node.node_side = " <<  nodes[ctr].node_side << "\n";
	}

	//printing the set of unreachable nodes.
	for(unsigned int ctr = 0 ; ctr < notReachNodes.size(); ctr++)
	{
		cout << "\nUnReachable Node: " << ctr <<"\n";
		cout << "Node.system_id = " << notReachNodes[ctr].system_id << "\n";
		cout << "Node.system_type = " << notReachNodes[ctr].system_type << "\n";
		cout << "Node.node_state = " << notReachNodes[ctr].node_state << "\n";
		cout << "Node.node_name = " << notReachNodes[ctr].node_name << "\n";
		cout << "Node.system_name = " << notReachNodes[ctr].system_name << "\n";
		cout << "Node.node_side = " << notReachNodes[ctr].node_side << "\n";
	}

	cout << "\nCalling jinitreq with U1: "<< U1 << " U2:"<<U2 << "Len:" << Len << " Msg:" << Msg << "\n";

	//Call jinitreq
	ACS_JTP_Job::JTP_Node node;

	if( nodes.size() == 0 )
	{
		cout <<"\n Query did not return any results, doing a workaround \n";

		node.system_id = ACS_JTP_Job::SYSTEM_ID_THIS_NODE;
		node.node_state = ACS_JTP_Job::NODE_STATE_ACTIVE;
		node.system_type = ACS_JTP_Job::SYSTEM_TYPE_AP;
		strcpy(node.node_name,"AP1A");
		strcpy(node.system_name, "AP1");
		node.node_side = ACS_JTP_Job::NODE_SIDE_A;
	}
	else
	{
		node.system_id = nodes[0].system_id;
		node.node_state = nodes[0].node_state;
		node.system_type = nodes[0].system_type;
		strcpy(node.node_name , nodes[0].node_name);
		strcpy( node.system_name, nodes[0].system_name );
		node.node_side = nodes[0].node_side ;
	}

	if (J1.jinitreq(&node, U1, U2, Len, Msg) == false)
	{
		J1.jfault(RU1, RU2, R);
		cout << "(jfault) The job did not initiate\n";
		cout << "U1 = " << U1 << " RU1 = " << RU1 << "\n";
		cout << "U2 = " << U2 << " RU2 = " << RU2 << "\n";
		cout << "R = "<< R << "\n";
		return -1;
	}

	cout << "\nFetching jinitconf \n";
	// jinitconf can not fail if jinitreq returns true
	J1.jinitconf(RU1, RU2, R);

	// Do something with the result if you like
	if ((U1 != RU1) || (U2 != RU2))
	{
		cout << "U1 = " << U1 << " RU1 = " << RU1 << "\n";
		cout << "U2 = " << U2 << " RU2 = " << RU2 << "\n";
		cout << "jinitconf, The data is modified\n";
	}

	// The fd is valid from when jinitconf is called
	// until jresultind is called
	cout << "\nGet the handle to wait for incoming data request.\n";

	JTP_HANDLE fd ;
	fd = J1.getHandle();

	cout << "\nWaiting on handle, recieved signal\n";

	//Create a FD set.
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	fd_set readHandleSet;
	FD_ZERO(&readHandleSet);
	FD_SET (fd, &readHandleSet);

	int ret = select ((int)(fd+1), &readHandleSet, 0, 0, &tv);

	//Check for timeout.
	if (ret >= 0)
	{
		cout << "\nFetching jresultind for jresultreq\n";
		if ( !FD_ISSET(fd, &readHandleSet))
		{
			cout << "\nTimeout occurred for jresultreq\n";
			J1.jtermreq();
			cout << "jresultind, The call timed out" << endl;
			delete[] Msg;
			Msg = 0;
			return -1;
		}
	}
	else
	{
		J1.jtermreq();
		cout << "Error occurred in select system call." << endl;
		delete[] Msg;
		Msg = 0;
		return -1;
	}

	if (J1.jresultind(RU1, RU2, R, RLen, RMsg) == false)
	{
		J1.jfault(RU1, RU2, R);
		cout << "(jfault) The job did not complete\n";
		cout << "jresultind, was expected\n";
		cout << "R =" << R << "\n";
		return -1;
	}
	// Do something with the data

	if (	(U1  != RU1)  ||
			(U2  != RU2)  ||
			(Len != RLen) ||
			(memcmp(Msg, RMsg, Len) != 0))
	{
		cout << "\nU1 = "<< U1 << " RU1 = "<< RU1 << "\n";
		cout << "U2 = " << U2 << " RU2 = " << RU2 << "\n";
		cout << "Len = " << Len << " RLen = "<< RLen << "\n";
		cout << "Fetched jresultind, The data is modified\n";
	}
	delete[] Msg;
	Msg = 0;

	return 0;

}

/*===================================================================
   ROUTINE: Conversation
		    This routine acts as an initiator for communication in conversation mode.
=================================================================== */
int Conversation(char* ServiceName, unsigned short MsgSize)
{
	char*	Msg = 0;
	char*	RMsg = 0;
	unsigned short  U1=0, U2=0, R=0, RU1=0, RU2=0, Len=0, RLen=0;
	U1 = 10;
	U2 = 11;

	//Max buffer length.
	//Allocate memory for the buffer.
	Msg = (char*) new (std::nothrow) char[strlen("Test message") + 1];

	Len  = strlen("Test message") + 1;

	//Initialize the message buffer.
	memset(Msg, 0, Len);

	strcpy(Msg,"Test message");

	cout << "\nCreating a conversation object with"
			" Service Name: " << ServiceName << " and Maximum Buffer Size: " << MsgSize << "\n";

	// Create a JTP_Conversation object with name and maxbuffer
	 ACS_JTP_Conversation C1(ServiceName, MsgSize);

	cout << "\nSetting the timeout to 3 seconds\n";
	// Override the default timeout which is 5 seconds
	C1.setTimeOut(3);

	cout << "\nSetting the number of retries to 2 seconds\n";
	// Override the default no of tries.
	C1.setNoOfTries(2);

	cout << "\nSetting the default time between delays to 1 second\n";
	// Override the default time between retries (which is 0 seconds)
	C1.setDelayTime(1);

	//Create vector for reachable nodes.
	std::vector<ACS_JTP_Conversation::JTP_Node> nodes;

	//Create vector for unreachable nodes.
	std::vector<ACS_JTP_Conversation::JTP_Node> notReachNodes;

	cout << "\nCalling query method to get the reachable and unreachable nodes from DSD\n";
	//Find out the set on reachable and unreachable nodes.
	C1.query(ACS_JTP_Conversation::SYSTEM_TYPE_AP , nodes, notReachNodes);

	cout << "\nPrinting the resulting of query method\n";
	//printing the set of reachable nodes.
	for ( unsigned int ctr = 0 ; ctr < nodes.size() ; ctr++)
	{
		cout << "\nReachable Node: "<< ctr << "\n";
		cout << "Node.system_id = " << nodes[ctr].system_id << "\n";
		cout << "Node.system_type = "<< nodes[ctr].system_type << "\n";
		cout << "Node.node_state = " << nodes[ctr].node_state << "\n";
		cout << "Node.node_name = " << nodes[ctr].node_name << "\n";
		cout << "Node.system_name = " << nodes[ctr].system_name << "\n";
		cout << "Node.node_side = " << nodes[ctr].node_side << "\n";
	}

	//printing the set of unreachable nodes.
	for(unsigned int ctr = 0 ; ctr< notReachNodes.size(); ctr++)
	{
		cout << "\nUnReachable Node: " << ctr << "\n";
		cout << "Node.system_id = " << notReachNodes[ctr].system_id << "\n";
		cout << "Node.system_type = " << notReachNodes[ctr].system_type << "\n";
		cout << "Node.node_state = " << notReachNodes[ctr].node_state << "\n";
		cout << "Node.node_name = " << notReachNodes[ctr].node_name << "\n";
		cout << "Node.system_name = " << notReachNodes[ctr].system_name << "\n";
		cout << "Node.node_side = " << notReachNodes[ctr].node_side << "\n";
	}

	cout << "\nCalling jexinitreq with U1:"<< U1 << " U2:" << U2 << "\n";

	ACS_JTP_Conversation::JTP_Node node;
	if( nodes.size() == 0 )
	{
		cout << "\n Query did not return any results, adding workaround \n";

		node.system_id = ACS_JTP_Conversation::SYSTEM_ID_THIS_NODE;
		node.node_state = ACS_JTP_Conversation::NODE_STATE_ACTIVE;
		node.system_type = ACS_JTP_Conversation::SYSTEM_TYPE_AP;
		strcpy(node.node_name,"AP1A");
		strcpy(node.system_name, "AP1");
		node.node_side = ACS_JTP_Conversation::NODE_SIDE_A ;
	}
	else
	{
		node.system_id = nodes[0].system_id;
		node.node_state = nodes[0].node_state;
		node.system_type = nodes[0].system_type;
		strcpy(node.node_name, nodes[0].node_name);
		strcpy(node.system_name, nodes[0].system_name);
		node.node_side = nodes[0].node_side;
	}

		//Call jexinitreq
	if (C1.jexinitreq(&node, U1, U2) == false)
	{
		C1.jexdiscind(RU1, RU2, R);
		cout << "(jexdiscind) The conversation did not initiate\n";
		cout << "U1 = "<< U1 <<" RU1 = "<< RU1 << "\n";
		cout << "U2 = "<< U1 <<" RU2 = "<< RU2 << "\n";
		cout << "R = "<< R << "\n";
		delete[] Msg;
		Msg = 0;
		return -1;
	}

	cout << "\nFetching jexinitconf \n";
	// jexinitconf can not fail if jexinitreq returns true
	C1.jexinitconf(RU1, RU2, R);

	// Do something with the result if you like
	if ((U1 != RU1) || (U2 != RU2))
	{
		cout << "U1 = " << U1 << " RU1 = " << RU1 << "\n";
		cout << "U2 = " << U2 << " RU2 = " << RU2 << "\n";
		cout << "jexinitconf, The data is modified\n";
	}

	// The fd is valid from when jexinitconf is called
	// until jresultind is called
	cout << "\nGet the handle to wait for incoming data request.\n";
	JTP_HANDLE fd = C1.getHandle();

	cout << "\nSending jexdatareq\n";

	if (C1.jexdatareq(U1, U2, Len, Msg) == false)
	{
		cout << "jexdatareq was not sent\n";
		delete[] Msg;
		Msg = 0;
		return -1;
	}

	cout << "\nWaiting on handle, recieved signal\n";

	//Create a FD set.
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	fd_set readHandleSet;
	FD_ZERO(&readHandleSet);
	FD_SET (fd, &readHandleSet);

	int ret = select ((int)(fd+1), &readHandleSet, 0, 0, &tv);

	//Check for timeout.
	if (ret >= 0)
	{
		cout << "\nHandle is signalled\n";
		if ( !FD_ISSET(fd, &readHandleSet))
		{
			C1.jexdiscind(RU1, RU2, R);
			cout << "(jexdiscind) the conversation broke down" << endl;
			cout << "jexdataind, was expected, " << endl;
			cout << "R = " << R << endl;
			delete[] Msg;
			Msg = 0;
			return -1;
		}
		else
		{
			cout << "\nRecieved jexdatareq\n";
		}
	}
	else
	{
		C1.jexdiscind(RU1, RU2, R);
		cout << "(jexdiscind) the conversation broke down" << endl;
		cout << "jexdataind, was expected, " << endl;
		cout << "R = " << R << endl;
		delete[] Msg;
		Msg = 0;
		return -1;
	}

	if (C1.jexdataind(RU1, RU2, RLen, RMsg) == false)
	{
		C1.jexdiscind(RU1, RU2, R);
		cout << "(jexdiscind) the conversation broke down" << endl;
		cout << "jexdataind, was called " << endl;
		cout << "R = " << R << endl;
		delete[] Msg;
		Msg = 0;
		return -1;
	}

	// Do something with the data

	if (	(U1  != RU1)  ||
			(U2  != RU2)  ||
			(Len != RLen) )
	{
		cout << "\nU1 = "<< U1 << " RU1 = " << RU1 << "\n";
		cout << "U2 = "<< U2 << " RU2 = " << RU2 << "\n";
		cout << "Len = " << Len << " RLen = " << RLen << "\n";
		cout << "jexdataind, The data is modified\n";
	}

	R = 0;
	//Terminate the conversation session.
	C1.jexdiscreq(U1, U2, R);

	delete[] Msg;
	Msg = 0;
	return 0;

}


/*===================================================================
   ROUTINE: main
		    This routine invokes the destinator,
			initiator for job mode.
=================================================================== */
int main(int argc, char* argv[])
{
	if (argc == 3 && argv[1][0] == 's')
	{
		cout << "\nStarting destinator...\n";
		return ((int)Server(argv[2]));

	}
	else if( argc == 4 && argv[1][0] == 'j')
	{
		cout << "Creating Initiator for JOB mode...\n";
		return ((int)Job(argv[2], (unsigned short)atoi(argv[3])));
	}
	else if(argc == 4 && argv[1][0] == 'c')
	{
		cout << "Creating Initiator for CONVERSATION mode...\n";
		return ((int)Conversation(argv[2], (unsigned short)atoi(argv[3])));
	}
	else
	{
		Help();
		return 0;
	}

}

