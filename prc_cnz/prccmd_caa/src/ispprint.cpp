//========================================================================================
// 
// NAME
//      ispprint.cpp
//
// COPYRIGHT
//		Eric sson AB 2004 - All Rights Reserved.
//
//		The Copyright to the computer program(s) herein is the	
//		property of Ericsson AB, Sweden. The program(s) may be
//		used and/or copied only with the written permission from
//		Ericsson AB or in accordance with the terms and conditions 
//		stipulated in the agreement/contract under which the 
//		program(s) have been supplied.
//
// DOCUMENT NO
//		190 89-CAA 109 0520 Ux
//
// AUTHOR 
//		2004-02-10 by EAB/UZ/DH UABTSO
//
// REVISION
//		R1A
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//
//		REV NO	DATE 	NAME		DESCRIPTION
//		R1A		040210	UABTSO		1:st revision
//		R1B		050726	QVALLIK		For TR HF78438
//		R3E		061227	XCSVAAN		For TR HH48545
// SEE ALSO 
//		-
//
//========================================================================================

#include "getopt.h"
#include "ACS_PRC_ispapi.h"
#include "acs_prc_api.h"
#include <assert.h>
#include <iomanip>
#include <string>
#include <map>
#include <bitset>
#include "ace/Signal.h"
#include "ace/Sig_Handler.h"
#include "ispprintSignalHandler.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>
#include <arpa/inet.h>

const char pipeName[] = "/var/run/ap/AcsIspprintServer";

int openPipe(char* pipeName);
int receiveResponse(int pipeH, char* response,int& size,int& err);
bool sendCommand(int pipeH,string order);

static int siglist[] = {SIGHUP, SIGQUIT, SIGINT, SIGTERM, SIGPIPE };

const char node_a('A');
const char node_b('B');
const int ownNode(0);
const int remoteNode(1);
const int ownNodeG(2);
const int remoteNodeG(3);

char activeNode = ' ';
int hPipenext;

//========================================================================================
//Prototype
//========================================================================================

bool short_print(false);
string getValidDate(char*);
using namespace std;

//========================================================================================
//	Type declaration
//========================================================================================

enum {e_start, e_end};

//enum objectType
//{
//	e_runlevel,
//	e_nodestate,
//	e_servicestate,
//	e_resourcestate,
//	e_othernodeshutdown,
//	e_synchronize
//};

enum listopt_t
{
	e_apstate =			0x01,
	e_runlevel =		0x02,
	e_nodestate =		0x04,
	e_resourcestate =	0x08,
	e_reason =			0x10,
	e_details =			0x20
};

int printOldBackup(){
	return 0;
}

char
getActiveNode(nodeStateType state, char node)
{
	
	switch (state)
	{
	case active:
		activeNode = node; 
		break;
		
	case passive:
	case unknownNodeState:
		if (node == activeNode)
		{
			// Cannot deduce active node
			activeNode = ' ';
		}
		break;
		
	default: 
		assert("!Illegal node state");
	}

	return activeNode;
}

void 
usage(bool verbose = false)
{
	cout << "Usage: " << "ispprint" << " [-a [-l][-n][-s][-r [-d]]][-g][-t starttime][-x endtime]" << endl;
	cout << "       " << "ispprint" << " [-z][-m][-c][-g][-t starttime][-x endtime]"<<endl;
	cout << "       " << "ispprint" << " [-h]" <<endl;
	if (verbose)
	{
		cout << "Where" << endl;
		cout << "  -a              Analysis option" << endl;
		cout << "  -l              Include runlevel" << endl;
		cout << "  -n              Include node state" << endl;
		cout << "  -s              Include LBB service and resource state" << endl;
		cout << "  -r              Include reason" << endl;
		cout << "  -d              Include reason detailed information" << endl;
		cout << "  -g              Representation in Universal Time Coordinates (UTC)" << endl; 
		cout << "  -t starttime    Start time expressed as YYMM[DD[-hh[mm]]]" << endl;
		cout << "  -x endtime      End time expressed as YYMM[DD[-hh[mm]]]" << endl;
		cout << "  -c              Enabling shortest printout which includes only node and runlevel events" << endl;
		cout << "  -z              listing event-log size" << endl;
		cout << "  -m              listing own node only" << endl;
		cout << "  -h              Help" << endl;
	}
		cout << endl;
}

//========================================================================================
//	Main program
//========================================================================================

int 
main(int argc, char *argv[])
{
	ACE_Sig_Handler ace_sigdispatcher;

	ispprintSignalHandler ispprint_sighandler;
	int numsigs = sizeof ( siglist ) / sizeof(int);
	for ( int i=0 ; i<numsigs ; ++i )
		ace_sigdispatcher.register_handler(siglist[i], & ispprint_sighandler);

	bool analysis(false);
	bool utc(false);
	bool help(false);
	string timestr[2];

	time_t ftime = time(NULL);
	char now [80];
	strftime (now,80,"%Y-%m-%d %X",localtime ( &ftime ));

	string begin_time;// = " 2001-01-01 00:00:00";
	string start_time = begin_time;
	string end_time = (string)now;
	string current_time = (string)now;

	int listopt(0);

	bool ok(true);
	int arg;
	
	bool globalEvent = true;
	bool localEvent = true;
	bool sizeOpt = false;
	bool extendedSize = false;
	bool oldBackup = false;
//	bool extendedHelp = false;
	bool myEventOnly = false;

	// Parse command
	string cmd = "ISP;";
	//while (((arg = getopt(argc, argv, "aclnsrdgt:x:hzmkpeobfiquwx")) != EOF) && ok)	{
	while (((arg = getopt(argc, argv, "aclnsrdgt:x:hzmkpeobfiquwx")) >= 0) && ok)	{

		switch (arg)
		{
			case 'a':	// Analysis option
				if (analysis == true)
				{
					ok = false;
				}
				else
				{
					analysis = true;
					listopt |= e_apstate;
					cmd.append("a;");
				}
				break;

			case 'l':	// Runlevel
				if (listopt & e_runlevel)
				{
					ok = false;
				}
				else
				{
					cmd.append("l;");
					listopt |= e_runlevel;
				}
				break;

			case 'n':	// Node State
				if (listopt & e_nodestate)
				{
					ok = false;
				}
				else
				{
					cmd.append("n;");
					listopt |= e_nodestate;
				}
				break;

			case 's':	// Resource state
				if (listopt & e_resourcestate)
				{

					ok = false;
				}
				else
				{
					cmd.append("s;");
					listopt |= e_resourcestate;
				}
				break;

			case 'r':	// Reason
				if (listopt & e_reason)
				{
					cmd.append("r;");
					ok = false;
				}
				else
				{
					cmd.append("r;");
					listopt |= e_reason;
				}
				break;

			case 'd':	// Details
				if (listopt & e_details)
				{
					cmd.append("d;");
					ok = false;
				}
				else
				{
					cmd.append("d;");
					listopt |= e_details;
				}
				break;

			case 'g':	// UTC option
				cmd.append("g;");
				ok = utc? false: utc = true;
				break;

			case 't':	// Get start time
				ok = timestr[e_start].empty()? timestr[e_start] = optarg, true: false;
				cmd.append("t@");
				cmd.append(getValidDate(optarg).c_str());
				cmd.append(";");
				start_time = getValidDate(optarg).c_str();
				break;

			case 'x':	// Get end time
				ok = timestr[e_end].empty()? timestr[e_end] = optarg, true: false;
				cmd.append("x@");
				cmd.append(getValidDate(optarg).c_str());
				cmd.append(";");
				end_time = getValidDate(optarg).c_str();
				break;

			case 'h':	// Get help
				if (help == true)
				{
					ok = false;
				}
				else
				{
					help = true;
				}
				break;

			case 'm':
				myEventOnly = true;
				cmd.append("m;");
				break;

			case 'z':
				cmd.append("z;");
				sizeOpt = true;
				break;

			case 'c':	// For TR  HH48545 New option '-c' added for Shortest printout
				if (short_print)
				{
					ok = false;
				}
				else
				{
					short_print=true;
					cmd.append("c;");
				}
				break;

			default:
				ok = false;
		}
	}

	if ( arg == -2 )
		ok = false;

	if (cmd == "ISP;")
	{
		cmd.append("N;");
	}

	if( (analysis == true && help == true) || (short_print && help) ||
		(utc && help) || ((!timestr[e_start].empty() || !timestr[e_end].empty()) && help) )
	{
		usage();
		return 2;
	}
	if (!ok || (argc - optind != 0))
	{
		usage();
		return 2;
	}

	if (analysis == false && listopt) 
	{
		usage();
		return 2;
	}

	if (!(listopt & e_reason) && (listopt & e_details))
	{
		usage();
		return 2;
	}
	
	if( analysis && short_print)
	{
		usage();
		return 2;
	}

//	if((!timestr[e_start].empty() || !timestr[e_end].empty()) && (short_print))
//	{
//		usage();
//		return 2;
//	}

	if( myEventOnly && ((analysis) || (help) /*|| (short_print)*/ ) )
	{
		usage();
		return 2;
	}

	//if( sizeOpt && ((analysis) || (help) || (short_print) || (utc) || !timestr[e_start].empty() || !timestr[e_end].empty() ) )
	//if( sizeOpt && ((analysis) || (help) || (short_print) || (utc) ) )

	if( sizeOpt && ((analysis) || (help) /*|| (short_print)*/ ) )
	{
		usage();
		return 2;
	}

	if ( ok == true && help == true )
	{
		usage(true);
		return 0;
	}

	if (help)
	{
		usage(true);
		return 0;
	}

	if (oldBackup)
	{
		if (analysis || utc || help || (listopt != 0) || 
			(!globalEvent) || (!localEvent) || sizeOpt || 
			extendedSize)
		{
			usage();
			return 2;
		}
		int r = printOldBackup();
		return r;
	}

	Ispapi p;
	p.open();
	int result=0;
	if ((result = p.getFirstEventLogTime(begin_time)) <0) { p.close(); exit(1); };
	p.close();

	if (strcmp(current_time.c_str(), start_time.c_str()) < 0)
	{
		cout<<"Start time must not be greater than current time."<<endl;
		exit(7);
	}
	else if (strcmp(start_time.c_str(), end_time.c_str()) > 0)
		{
			cout<<"Start time must not be greater than end time."<<endl;
			exit(6);
		}
	else if (strcmp(begin_time.c_str(), end_time.c_str())> 0)
	{
		cout<<"The oldest event was created after the end time."<<endl;
		exit(8);
	}

	char buffer[4096] = {0};
	ACE_LSOCK_Stream Stream;
	ACE_LSOCK_Connector Connector;
	ACE_UNIX_Addr addr (pipeName);
	size_t byte_read = 0;

	if (Connector.connect (Stream, addr) == -1){
		cout<<"Error - ISP Server not running"<<endl;
		exit(3);
	}

	if ( Stream.send_n (cmd.c_str(), cmd.size()+1) == -1 ){
		cout<<"Error - Impossible to contact the ISP Server"<<endl;
		Stream.close();
		exit(4);
	}

	int Error = 1;

	int byte_sent = 0;

	do {

		memset(buffer,0,sizeof(buffer));
		byte_read = Stream.recv (buffer, 4);

		if ( byte_read > 0 ){

			byte_sent = ntohl(*reinterpret_cast<uint32_t *>(buffer));

			if ( byte_sent ){
				byte_read = Stream.recv (buffer+4, byte_sent);

				if ( byte_read > 0){
					if ( strncmp(buffer+4,"END",3) == 0 ){
						Error = 0;
					}
					else if ( strncmp(buffer+4,"NOT_FOUND",9) == 0 ){
						Error = -1;
					}
					else {
						if ( byte_read < 4092 )
							buffer[byte_read+4] = 0;

						cout << buffer+4<<endl;
					}
				}
				else {
					Error = -2;
				}
			}
		}
		else {
			Error = -2;
		}
	} while ( Error > 0);

	if ( Error == -1 ){
		cout<<"Error - ISP Log file not found"<<endl;
		Stream.close();
		exit(5);
	}
	else if ( Error == -2 ){
		Stream.close();
		exit(9);
	}

	Stream.close();

	return 0;
} // End of main

//========================================================================================
//	String to Number
//========================================================================================

int strToNumb(string numbStr)
{
	int num[2];
	for (int i = 0; i < 2; i++)
	{
		num[i] = numbStr[i] - 0x30;
		if (num[i] < 0 || num[i] > 10)
		{
			//cout<<"Error STRTONUMB"<<endl;
			usage();
			exit(2);
		}
	}

	return num[0] * 10 + num[1];
}

//========================================================================================
//	Time format creation
//========================================================================================

string getValidDate(char temp[]){

	char YY[3]= "00";
	char MM[3]= "01";
	char DD[3]= "01";
	char hh[3]= "00";
	char mm[3]= "00";
	char ss[3]= "00";
	string tok;
	string tok0 = "20";
	string tok1 = ":";
	string tok2 = "-";
	string tok3 = " ";

	switch (strlen(temp)){
		   case (4):
				strncpy(YY , &temp[0],2);
				strncpy(MM , &temp[2],2);
				break;
		   case (6):
				 strncpy(YY , &temp[0],2);
				 strncpy(MM , &temp[2],2);
				 strncpy(DD , &temp[4],2);
				break;
		   case (9):
				 if (temp[6] != '-'){
					 cout<<"Parameter error: Error in time format."<<endl;
					 exit(2);
				 }

				 strncpy(YY , &temp[0],2);
				 strncpy(MM , &temp[2],2);
				 strncpy(DD , &temp[4],2);
				 strncpy(hh , &temp[7],2);

				break;
		   case (11):
				 if (temp[6] != '-'){
					 cout<<"Parameter error: Error in time format."<<endl;
					 exit(2);
				 }

				 strncpy(YY , &temp[0],2);
				 strncpy(MM , &temp[2],2);
				 strncpy(DD , &temp[4],2);
				 strncpy(hh , &temp[7],2);
				 strncpy(mm , &temp[9],2);
				break;
		   default:
			   cout<<"Parameter error: Error in time format."<<endl;exit(2);
	}
	//check numbers

	int year = 2000;
	if ( strToNumb(hh) < 0 || strToNumb(hh) > 23){
		cout<<"Parameter error: Illegal date and/or time."<<endl;
		exit(9);
	}

	if (strToNumb(mm) < 0 || strToNumb(mm) > 59){
		cout<<"Parameter error: Illegal date and/or time."<<endl;
		exit(9);
	}

	year += strToNumb(YY);

	if ( strToNumb(MM) >= 1 && strToNumb(MM) <= 12 && strToNumb(DD) >= 1 && strToNumb(DD) <= 31 )
	{
		if (strToNumb(MM) == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
		{
			if ( strToNumb(DD) >= 1 && strToNumb(DD) <= 29)
				tok = tok0 + YY + tok2 + MM + tok2 + DD + tok3 + hh + tok1 + mm + tok1 + ss;
			else {
				cout<<"Parameter error: Illegal date and/or time."<<endl;
				exit(9);
			}
		}
		else if(strToNumb(MM) == 2 && !((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
		{
			 if ( strToNumb(DD) >= 1 && strToNumb(DD) <= 28)
				 tok = tok0 + YY + tok2 + MM + tok2 + DD + tok3 + hh + tok1 + mm + tok1 + ss;
			 else {
				 cout<<"Parameter error: Illegal date and/or time."<<endl;
				 exit(9);
			 }
		}
		 else if( strToNumb(MM) == 4 || strToNumb(MM) == 6 || strToNumb(MM) == 9 || strToNumb(MM) == 11 )
		 {
			  if ( strToNumb(DD) >= 1 && strToNumb(DD) <= 30)
				 tok = tok0 + YY + tok2 + MM + tok2 + DD + tok3 + hh + tok1 + mm + tok1 + ss;
			  else {
				  cout<<"Parameter error: Illegal date and/or time."<<endl;
				  exit(9);
			  }
		 }
		 else
			 tok = tok0 + YY + tok2 + MM + tok2 + DD + tok3 + hh + tok1 + mm + tok1 + ss;
	} else {
	   cout<<"Parameter error: Illegal date and/or time."<<endl;
	   exit(9);
	}

	return tok;
}
