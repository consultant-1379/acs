#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include "ace/ACE.h"
#include "acs_chb_cp_object.h"
using namespace std;


int main()
{
	ACS_CHB_CP_object APZType(APZ_type);
	ACS_CHB_CP_object EXSide(EX_side);
	ACS_CHB_CP_object SBStatus(SB_status);
	ACS_CHB_CP_object CPStatus(CP_status);
	ACS_CHB_CP_object CPConnection(CP_connection);
	ACS_CHB_CP_object EXExchangeIdentity(EX_Exchange_identity);
	ACS_CHB_CP_object LastRestartValue(Last_Restart_Value);
	time_t now;
	struct tm * lpTime;
	char lpTimeString[24];
	int rc;
	ACE_HANDLE client;
	fd_set sRead;
	struct timeval sTime;
	char* val;
	memset((char*) &sTime, 0, sizeof(sTime));
	sTime.tv_sec = 100;
	sTime.tv_usec = 0;
	int CPObject;
	cout<<"\n1-APZ_type\n2-EX_side\n3-SB_status\n4-CP_status\n5-CP_connection\n6-EX_Exchange_identity\n8-Last_Restart_Value"<<endl;
	cout<<"Enter the type of CP Object : \n";
	cin>>CPObject;
	char in = 'N';
	     do
	     {

		switch (CPObject)
		{
		case 1:
				//
				// Check for APZ_type objects
				//
				if(APZType.get_status() == Disconnected)
				{
					if(APZType.connect(10,6) == ACS_CHB_FALSE)
					{

						cout << APZType.getErrorText() << endl;

						cout<<"Error in connecting to cp, errno: "<<ACE_OS::last_error()<<endl;
						//exit(1);
						return -1;
					}
				}
				client  = APZType.get_fileDescriptor();
				if( client != ACE_INVALID_HANDLE )
				{
					FD_ZERO(&sRead);
					FD_SET(client, &sRead);
					rc = select(client+1, &sRead, 0, 0, &sTime);
					if( rc > 0 )
					{
						if( FD_ISSET( client, &sRead))
						{
							if((val = APZType.get_value()) != (char *) NULL)
							{
								//GetLocalTime(&lpTime);
								time(&now);
								lpTime=localtime(&now);
								sprintf(lpTimeString,"%02d:%02d:%02d",lpTime->tm_hour,lpTime->tm_min,lpTime->tm_sec);
								cout <<  lpTimeString << "  " << APZType.get_name() << "=" << val << endl;
							}
							else
							{
								cout << APZType.getErrorText() << endl;
								//exit(1);
								return -1;
							}
						}
						else
						{
							cout << "FD is not signalled " << endl;
						}
					}
					else if ( rc == 0)
					{
						cout << "\nNo request came within 100 seconds\n";
						return -1;
					}
					else
					{
						//exit(1);
						cout << "\nError occurred in select..\n";
						return -1;
					}
				}
				break ;

		case 2:
				//
				// Check for EX_side objects
				//
				if(EXSide.get_status() == Disconnected)
				{
					if(EXSide.connect(10,6) == ACS_CHB_FALSE)
					{
						cout << EXSide.getErrorText() << endl;
						//exit(2);
						return -1;
					}
				}
				client  = EXSide.get_fileDescriptor();
				FD_ZERO(&sRead);
				FD_SET(client, &sRead);
				rc = select(client+1, &sRead, 0, 0, &sTime);
				if( rc > 0)
				{
					if((val = EXSide.get_value()) != (char *) NULL)
					{
						//GetLocalTime(&lpTime);
						time(&now);
						lpTime=localtime(&now);
						sprintf(lpTimeString,"%02d:%02d:%02d",lpTime->tm_hour,lpTime->tm_min,lpTime->tm_sec);
						cout <<  lpTimeString << "  " << EXSide.get_name() << "=" << val << endl;
					}
					else
					{
						cout << EXSide.getErrorText() << endl;
						//exit(1);
						return -1;
					}
				}
				else if ( rc == 0)
				{
					cout << "\nNo request came within 100 seconds\n";
					return -1;
				}
				else
				{
					cout << "\nError occurred in select..\n";
					//exit(1);
					return -1;
				}
				break ;

		case 3:

				//
				// Check for SB_status objects
				//
				if(SBStatus.get_status() == Disconnected)
				{
					cout<<"Connecting.....";
					if(SBStatus.connect(10,6) == ACS_CHB_FALSE)
					{
						cout << SBStatus.getErrorText() << endl;
						//exit(3);
						return -1;
					}
				}
				cout<<"Connected";
				client  = SBStatus.get_fileDescriptor();
				FD_ZERO(&sRead);
				FD_SET(client, &sRead);
				rc = select(client+1, &sRead, 0, 0, &sTime);
				if( rc > 0 )
				{
					if( FD_ISSET( client, &sRead ))
					{
						if((val = SBStatus.get_value()) != (char *) NULL)
						{
							//GetLocalTime(&lpTime);
							time(&now);
							lpTime=localtime(&now);
							sprintf(lpTimeString,"%02d:%02d:%02d",lpTime->tm_hour,lpTime->tm_min,lpTime->tm_sec);
							cout <<  lpTimeString << "  " << SBStatus.get_name() << "=" << val << endl;
						}
						else
						{
							cout << SBStatus.getErrorText() << endl;
							//exit(1);
							return -1;
						}
					}
					else
					{
						cout << " fd is not set " << endl;
					}
				}
				else if ( rc == 0)
				{
					cout << "\nNo request came within 100 seconds\n";
					return -1;
				}
				else
				{
					cout << "\nError occurred in select..\n";
					//exit(1);
					return -1;
				}
				break ;

		case 4:
				//
				// Check for CP_status objects
				//
				if(CPStatus.get_status() == Disconnected)
				{

					if(CPStatus.connect(10,6) == ACS_CHB_FALSE)
					{

						cout << CPStatus.getErrorText() << endl;
						//exit(4);
						return -1;
					}
				}

				client  = CPStatus.get_fileDescriptor();
				FD_ZERO(&sRead);
				FD_SET(client, &sRead);
				rc = select(client+1, &sRead, 0, 0, &sTime);
				if( rc > 0 )
				{
					if((val = CPStatus.get_value()) != (char *) NULL)
					{
						//GetLocalTime(&lpTime);
						time(&now);
						lpTime=localtime(&now);
						sprintf(lpTimeString,"%02d:%02d:%02d",lpTime->tm_hour,lpTime->tm_min,lpTime->tm_sec);
						cout <<  lpTimeString << "  " << CPStatus.get_name() << "=" << val << endl;
					}
					else
					{
						cout << CPStatus.getErrorText() << endl;
						//exit(1);
						return -1;
					}
				}
				else if ( rc == 0)
				{
					cout << "\nNo request came within 100 seconds\n";
					return -1;
				}
				else
				{
					cout << "\nError occurred in select..\n";
					//exit(1);
					return -1;
				}
				break ;

		case 5:

				//
				// Check for CP_connection objects
				//
				if(CPConnection.get_status() == Disconnected)
				{
					cout<<"Status of cp object is 'Disconnected'\n";
					cout<<"Trying to connect cp\n";
					if(CPConnection.connect(10,6) == ACS_CHB_FALSE)
					{
						cout << CPConnection.getErrorText() << endl;
						//exit(5);
						cout<<"Unable to connect to cp, errno : "<<ACE_OS::last_error()<<endl;
						return -1;
					}
				}
				client  = CPConnection.get_fileDescriptor();
				FD_ZERO(&sRead);
				FD_SET(client, &sRead);
				rc = select(client+1, &sRead, 0, 0, &sTime);
				if( rc > 0 )
				{
					if((val = CPConnection.get_value()) != (char *) NULL)
					{
						//GetLocalTime(&lpTime);
						time(&now);
						lpTime=localtime(&now);
						sprintf(lpTimeString,"%02d:%02d:%02d",lpTime->tm_hour,lpTime->tm_min,lpTime->tm_sec);
						cout <<  lpTimeString << "  " << CPConnection.get_name() << "=" << val << endl;
					}
					else
					{
						cout << CPConnection.getErrorText() << endl;
						//exit(1);
						return -1;
					}
				}
				else if ( rc == 0)
				{
					cout << "\nNo request came within 100 seconds\n";
					return -1;
				}
				else
				{
					cout << "\nError occurred in select..\n";
					//exit(1);
					return -1;
				}
				break ;

		case 6:
				//
				// Check for EX_Exchange_identity objects
				//
				if(EXExchangeIdentity.get_status() == Disconnected)
				{
					if(EXExchangeIdentity.connect(10,6) == ACS_CHB_FALSE)
					{
						cout << EXExchangeIdentity.getErrorText() << endl;
						//exit(6);
						return -1;
					}
				}

				client  = EXExchangeIdentity.get_fileDescriptor();
				FD_ZERO(&sRead);
				FD_SET(client, &sRead);
				rc = select(client+1, &sRead, 0, 0, &sTime);
				if( rc > 0 )
				{
					if((val = EXExchangeIdentity.get_value()) != (char *) NULL)
					{
						//GetLocalTime(&lpTime);
						time(&now);
						lpTime=localtime(&now);
						sprintf(lpTimeString,"%02d:%02d:%02d",lpTime->tm_hour,lpTime->tm_min,lpTime->tm_sec);
						cout <<  lpTimeString << "  " << EXExchangeIdentity.get_name() << "=" << val << endl;
					}
					else
					{
						cout << EXExchangeIdentity.getErrorText() << endl;
						//exit(1);
						return -1;
					}
				}
				else if ( rc == 0)
				{
					cout << "\nNo request came within 100 seconds\n";
					return -1;
				}
				else
				{
					cout << "\nError occurred in select..\n";
					//exit(1);
					return -1;
				}
				break ;

		case 8:
				//
				// Check for Last_Restart_Value objects
				//
				if(LastRestartValue.get_status() == Disconnected)
				{
					if(LastRestartValue.connect(10,6) == ACS_CHB_FALSE)
					{
						cout << LastRestartValue.getErrorText() << endl;
						//exit(8);
						return -1;
					}
				}
				client  = LastRestartValue.get_fileDescriptor();
				FD_ZERO(&sRead);
				FD_SET(client, &sRead);
				rc = select(client+1, &sRead, 0, 0, &sTime);
				if( rc > 0 )
				{
					if((val = LastRestartValue.get_value()) != (char *) NULL)
					{
						//GetLocalTime(&lpTime);
						time(&now);
						lpTime=localtime(&now);
						sprintf(lpTimeString,"%02d:%02d:%02d",lpTime->tm_hour,lpTime->tm_min,lpTime->tm_sec);
						cout <<  lpTimeString << "  " << LastRestartValue.get_name() << "=" << val << endl;
					}
					else
					{
						cout << LastRestartValue.getErrorText() << endl;
						//				exit(1);
						return -1;
					}
				}
				else if ( rc == 0)
				{
					cout << "\nNo request came within 100 seconds\n";
					return -1;
				}
				else
				{
					cout << "\nError occurred in select..\n";
					//exit(1);
					return -1;
				}
				break ;

		default : cout<<"Invalid CP Object"<<endl;
				  return 0;
		}
		cout << " Do you wish to continue(Y/N) : ";
		          cin >> in;



#if 0
		if(_kbhit()) break;
		Sleep(10);
#endif
	}while( in == 'Y' || in == 'y');


	// Disconnect session.
	APZType.disconnect();
	EXSide.disconnect();
	SBStatus.disconnect();
	CPStatus.disconnect();
	CPConnection.disconnect();
	EXExchangeIdentity.disconnect();
	LastRestartValue.disconnect();
	return 0;
}
