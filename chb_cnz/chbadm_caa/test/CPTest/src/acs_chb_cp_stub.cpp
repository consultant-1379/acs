#include<cstdio>
#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<cstring>
#include "ACS_JTP.h"
#include "ACS_CS_API.h"
#include "ACS_DSD_Client.h"
using namespace std;
#define MAX_BUFFER_SIZE 100
#define ACS_CHB_NUMBER_OF_TZ 23
#define ACS_CHB_HEARTBEAT 0
#define ACS_CHB_EX_EXCHANGE_IDENTITY 6
int main(int argc, char*argv[])
{
	char serviceName[100] = {0 };
	if( argc == 2 )
	{
		if( strcmp(argv[1], "ACTIVE" ) == 0 )
		{
			strcpy(serviceName, "AP01AHEARTBEAT");
		}
		else if( strcmp(argv[1], "PASSIVE") == 0)
		{
			strcpy(serviceName, "AP01BHEARTBEAT");
		}
		else
		{
			cout << " Incorrect node state!! " << endl;
			cout << " Please try again!!" << endl;
		}
	}
	else
	{
		cout << "Usage: CHB_Heartbeat_Test ACTIVE for active node" << endl;
		cout << "Usage: CHB_Heartbeat_Test PASSIVE for passive node" << endl;
		return 0;
	}
		
		
	CPID cpId;
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getClockMaster(cpId);

	ACS_DSD_Client myDSDClient;
	ACS_DSD_Node DSDNode;
    cpId=2001;
	myDSDClient.get_local_node(DSDNode);

	if(!(result==ACS_CS_API_NS::Result_Success))
	{
		cout<<"failed in getting cp id";
		return -1;
	}

	cout << " Received CP Id as: " << cpId << endl;

	unsigned short  U1=0, U2=0, R=0, Len=0, RU1= 0, RU2= 0 , RLen = 0;
	unsigned short  AU1=0, AU2=0, AR=0;
	 char *RMsg = 0;

	 char* Msg = 0;

	ACS_JTP_Conversation C1(serviceName, 1234);

	cout << " query for the  node providing service ";

	std::vector<ACS_JTP_Conversation::JTP_Node> nodes;

	std::vector<ACS_JTP_Conversation::JTP_Node>  notReachNodes;

	C1.query( ACS_JTP_Conversation::SYSTEM_TYPE_AP, nodes, notReachNodes );	

	cout<<"\nSending conversation request \n";
	ACS_JTP_Conversation::JTP_Node Node;

	if( nodes.size() > 0 )
	{
		cout << " query returned results ";
		Node.system_id = nodes[0].system_id;
		Node.system_type = nodes[0].system_type;
		Node.node_state = nodes[0].node_state;
		strcpy( Node.system_name, nodes[0].system_name );
		Node.node_side = nodes[0].node_side;
	}
	else
	{
		Node.system_id = ACS_JTP_Conversation::SYSTEM_ID_THIS_NODE;
		Node.system_type = ACS_JTP_Conversation::SYSTEM_TYPE_AP;
		//	Node.node_state = ACS_JTP_Conversation::NODE_STATE_PASSIVE;
		Node.node_state = ACS_JTP_Conversation::NODE_STATE_ACTIVE;
		//strcpy( Node.system_name, "AP1" );
		//Node.node_side = ACS_JTP_Conversation::NODE_SIDE_A;
	}

	cout << " Node Details: " <<  endl;
	cout << " Node.system_id = " << Node.system_id  << endl;
	cout << " Node.system_type = " << Node.system_type << endl;
	cout << " Node.node_state = " << Node.node_state << endl;
	//cout << " Node.system_name = " << Node.system_name << endl;

	cout << "U1 = " << U1 <<  endl;
	cout << "U2 = " << U2 << endl;

	if (C1.jexinitreq(&Node, AU1, AU2) == false)
	{
		C1.jexdiscreq(AU1, AU2, AR);
		cout << " Unable to connect " << endl;
		return -1;
	}
	C1.jexinitconf(AU1, AU2, AR);

	cout << " AU1= " << AU1 << endl;
	cout <<  " AU2 = " << AU2 << endl;
	cout << " AR = " << AR << endl;

	while( 1 )
	{
		cout  << " Please enter the value of U1`: ";
		cin  >> U1;

		cout << " Please enter the value of U2 : ";
		cin >> U2;


		if( U1 == 0x02 && U2 == 0 && R == 0 )
		{
			//We are not the Master CP. 

			C1.jexdiscreq( U1, U2, R );
			cout << " This node is not a Master CP " << endl;
			cout << " Sending disconnection! " <<  endl;
			return 0;

		}
		else if( U1 == ACS_CHB_HEARTBEAT )
		{
			struct tm * CP_system_time;
			time_t now = 0;
			time(&now);
			//Time Zone Information.
			CP_system_time = localtime(&now);

			cout << "CP_system_time.tm_zone = "<< CP_system_time->tm_zone << endl;
			cout << "CP_system_time.tm_year = "<< CP_system_time->tm_year << endl;
			cout << "CP_system_time.tm_yday = "<< CP_system_time->tm_yday << endl;
			cout << "CP_system_time.tm_wday = "<< CP_system_time->tm_wday << endl;
			cout << "CP_system_time.tm_sec = "<< CP_system_time->tm_sec << endl;
			cout << "CP_system_time.tm_mon = "<< CP_system_time->tm_mon << endl;
			cout << "CP_system_time.tm_min = "<< CP_system_time->tm_min << endl;
			cout << "CP_system_time.tm_mday = "<< CP_system_time->tm_mday << endl;
			cout << "CP_system_time.tm_isdst = "<< CP_system_time->tm_isdst << endl;
			cout << "CP_system_time.tm_hour = "<< CP_system_time->tm_hour << endl;
			cout << "CP_system_time.tm_gmtoff = "<< CP_system_time->tm_gmtoff << endl;

			//cout << "CP_system_time = " << asctime(CP_system_time) << endl;

			char bufptr[100];
			memset(&bufptr, 0, sizeof(bufptr));

			bufptr[0] = (char)(CP_system_time->tm_year);
			bufptr[1] = (char)(CP_system_time->tm_mon);
			bufptr[2] = (char)(CP_system_time->tm_mday);
			bufptr[3] = (char)(CP_system_time->tm_hour);
			bufptr[4] = (char)(CP_system_time->tm_min);
			bufptr[5] = (char)(CP_system_time->tm_sec);

			int16_t  DST=0,offset=0,timezone_info=1,offsetdir=0;
			// To do, check IWD for CP, and fix the message for offset, timezone, offsetdir and DST.
			bufptr[8] = (char)(offset & 0x00ff);
			bufptr[9] = (char)((offset >> 8) & 0x000f);
			bufptr[9] = (char)((( timezone_info << 4 ) & 0x0010) | bufptr[9]);
			bufptr[9] = (char)((( offsetdir << 5 ) & 0x0020) | bufptr[9]);
			bufptr[9] = (char)(((DST << 6 ) & 0x0040) | bufptr[9]);

			printf("bufptr[0] = %d\n", bufptr[0]);
			printf("bufptr[1] = %d\n", bufptr[1]);
			printf("bufptr[2] = %d\n", bufptr[2]);
			printf("bufptr[3] = %d\n", bufptr[3]);
			printf("bufptr[4] = %d\n", bufptr[4]);
			printf("bufptr[5] = %d\n", bufptr[5]);
			printf("bufptr[6] = %d\n", bufptr[6]);
			printf("bufptr[7] = %d\n", bufptr[7]);
			printf("bufptr[8] = %d\n", bufptr[8]);
			printf("bufptr[9] = %d\n", bufptr[9]);
			bufptr[10] = '\0';
			#if 0
			cout << "bufptr[1] = " << bufptr[1] << endl;
			cout << "bufptr[2] = " << bufptr[2] << endl;
			cout << "bufptr[3] = " << bufptr[3] << endl;
			cout << "bufptr[4] = " << bufptr[4] << endl;
			cout << "bufptr[5] = " << bufptr[5] << endl;
			cout << "bufptr[6] = " << bufptr[6] << endl;
			cout << "bufptr[7] = " << bufptr[7] << endl;
			cout << "bufptr[8] = " << bufptr[8] << endl;
			cout << "bufptr[9] = " << bufptr[9] << endl;
			#endif

			Len = sizeof(bufptr)+1;
			cout << "Len = " << Len;

			Msg  =  new char[100];

			memset(Msg, 0, 100);

			memcpy(Msg, bufptr, Len);

			cout<<"Msg="<<Msg;

			if( U2 == 0xCA01 )
			{
				//We need to send such a message, that would correspond to a 
				// failover request.
				// For FailOver U1 = ACS_CHB_HEARTBEAT
				//U2 = 0xCA01,
				//Node should be passive.

				if( C1.jexdatareq( U1, U2, Len, Msg ) == false )
				{
					C1.jexdiscreq( U1, U2, R );
					cout << " Unable to send the data to AP " << endl;
					cout << " Disconnection sent " <<  endl;
					return 0;
				}

				if( C1.jexdataind( RU1, RU2, RLen, RMsg) == false )
				{
					C1.jexdiscind(U1, U2, R);
					cout << " jexdatareq was expected !! ";
					return 0;
				}
				cout<<"jexdatareq was sent by AP";
			}
			else if (  U2 ==  0xCC01 )
			{
				
				//Passive node is faulty.
				   if( C1.jexdatareq( U1, U2, Len, Msg) == false )
				   {
					C1.jexdiscreq( U1, U2, R );
					cout << " Unable to send the data to AP " << endl;
					cout << " Disconnection sent " <<  endl;
					return 0;
				}
				if( C1.jexdataind( RU1, RU2, RLen, RMsg) == false )
				{
					C1.jexdiscind(U1, U2, R);
					cout << " jexdatareq was expected !! ";
					return 0;
				}
			}
			else if (  U2 == 0xCC00 ||  U2 == 0xCC02 )
			{

				if( C1.jexdatareq( U1, U2, Len, Msg ) == false )
				{
					C1.jexdiscreq( U1, U2, R );
					cout << " Unable to send the data to AP " << endl;
					cout << " Disconnection sent " <<  endl;
					 return 0;
				}
				if( C1.jexdataind( RU1, RU2, RLen, RMsg) == false )
				{
					C1.jexdiscind(U1, U2, R);
					cout << " jexdatareq was expected !! ";
					return 0;
				}
			}

		}
		else if( U1 == ACS_CHB_EX_EXCHANGE_IDENTITY )
		{
			char EMsg[MAX_BUFFER_SIZE]="5301R1FREF260CM608 1043";
			int EMsg_Len ;
			EMsg_Len= strlen(EMsg)+1;
			if( C1.jexdatareq( U1, U2, EMsg_Len, EMsg ) == false )
			{
				C1.jexdiscreq( U1, U2, R );
				cout << " Unable to send the data to AP " << endl;
				cout << " Disconnection sent " <<  endl;
				return 0;
			}
			if( C1.jexdataind( RU1, RU2, RLen, RMsg) == false )
			{
				C1.jexdiscind(U1, U2, R);
				cout << " jexdatareq was expected !! ";
				return 0;
			}
		}
		else if(( U1 != ACS_CHB_HEARTBEAT ) && ( U1 != ACS_CHB_EX_EXCHANGE_IDENTITY))
		{
			
			//CP Object Data.
			#if 0
			const int ACS_CHB_APZ_TYPE      = 1;    // The type of APZ
				const int   ACS_CHB_APZ210      = 1;
				const int   ACS_CHB_APZ211      = 2;
				const int   ACS_CHB_APZ212      = 3;
				const int   ACS_CHB_APZ213      = 4;
				const int   ACS_CHB_APZ214      = 5;
			const int ACS_CHB_EX_SIDE       = 2;    // The side of the APZ that is executable 
				const int   ACS_CHB_SIDE_A      = 1;
				const int   ACS_CHB_SIDE_B      = 2;
			const int ACS_CHB_SB_STATUS     = 3;    // The status of the standby side.
				const int   ACS_CHB_SBSE        = 1;
				const int   ACS_CHB_SBHA        = 2;
				const int   ACS_CHB_SBUP        = 3;
				const int   ACS_CHB_SBWO        = 4;
				const int   ACS_CHB_NOINFO      = 5;
			const int ACS_CHB_CP_STATUS     = 4;    // the status of the EX side.
				const int   ACS_CHB_CP_PARWO    = 1;
				const int   ACS_CHB_CP_SINEX    = 2;
				const int   ACS_CHB_CP_SBSE    = 3;
			const int ACS_CHB_CP_CONNECTION = 5;    // The status of the CP-AP connection
											// for object service.
				const int   ACS_CHB_CONNECTED   = 1;
				const int   ACS_CHB_NOT_CONNECTED = 2;
			const int ACS_CHB_EX_EXCHANGE_IDENTITY = 6;     
											// The exchange identity in the EX-Side
			const int ACS_CHB_SB_EXCHANGE_IDENTITY = 7;
											// The exchange identity in the SB-Side
			const int ACS_CHB_LAST_RESTART_VALUE = 8;
											// The last restart value.
				const int   ACS_CHB_SMALL_C      = 1;
				const int   ACS_CHB_SMALL_F      = 2;
				const int   ACS_CHB_LARGE_C      = 3;
				const int   ACS_CHB_LARGE_F      = 4;
				const int   ACS_CHB_RELOAD_C     = 5;
				const int   ACS_CHB_RELOAD_F     = 6;

			const int ACS_CHB_TMZ_ALARM =9;
			#endif

			//ACS_CHB_APZ_TYPE
			if( C1.jexdatareq( U1, U2, 0, 0 ) == false )
			{
				C1.jexdiscreq( U1, U2, R );
				cout << " Unable to send the data to AP " << endl;
				cout << " Disconnection sent " <<  endl;
				return 0;
			}	
			if( C1.jexdataind( RU1, RU2, RLen, RMsg) == false )
			{
				C1.jexdiscind(U1, U2, R);
				cout << " jexdatareq was expected !! ";
				return 0;
			}

		}	
	}

	return 0;
}
		

