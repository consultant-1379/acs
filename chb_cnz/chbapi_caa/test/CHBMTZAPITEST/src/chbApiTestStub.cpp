/*=================================================================== */
   /**
   @file chbApiTestStub.cpp

   Test application for MTZ API.

   @version N.N.N

   @copyright  Ericsson AB, Sweden 2010. All rights reserved.

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/10/2010     XKUSATI       Initial Release
   **/
/*=================================================================== */

//Include header files.
#include<iostream>
#include<ctype.h>// for the isdigit test
#include<string.h>// for the strlen test
#include<time.h>
#include "acs_chb_mtzclass.h"
using namespace std;


void getCurrentTime(struct tm *timeinfo)
{
	/* Current system date time    */
	time_t rawtime(NULL);
	time(&rawtime);

	struct tm tmTime;
	tmTime = *localtime(&rawtime);

	cout<<"Please enter the daylight saving time status(0/1):\t";
	cin >> tmTime.tm_isdst;

	timeinfo->tm_sec = tmTime.tm_sec;
	timeinfo->tm_min = tmTime.tm_min;
	timeinfo->tm_hour = tmTime.tm_hour;
	timeinfo->tm_mday = tmTime.tm_mday;
	timeinfo->tm_mon = tmTime.tm_mon;
	timeinfo->tm_year = tmTime.tm_year;
	timeinfo->tm_wday = tmTime.tm_wday;
	timeinfo->tm_yday = tmTime.tm_yday;
	timeinfo->tm_isdst = tmTime.tm_isdst;

	cout<<" LOCAL TIME : "<<asctime(timeinfo)<<endl;

}

int main(int argc, char* argv[])
{

	cout<<"\n\n\n'";
	cout<<"************************************************\n";
	cout<<"1.TimeZoneAlarm  TMZValue\n";
	cout<<"2.UTCtoLocalTime UTCtime TMZ\n";
	cout<<"3.LocalTimeToUTC LocalTime TMZ\n";
	cout<<"4.CalculateTheOffsetTime\n";
	cout<<"5.CPTime TimeOfCP\n";
	cout<<"*************************************************\n";
	cout<<"\nPlease enter option : ";
	int option =0;

	cin>>option;

	if ( ACS_CHB_mtzclass::MTZInit() == false )
        {
		cout << "Error occured while initialising MTZInit" << endl;
                return 0;
        }


	ACS_CHB_mtzclass MTZObj;

	switch(option)
	{

		case 1:
		{
			int TMZ;
			cout << "Please enter the time zone(0-23) : ";
		        cin >> TMZ;	
			int r = MTZObj.TimeZoneAlarm(TMZ);
			if( r == 0 )
			{
				cout << " No alarm for Time Zone " << TMZ << endl;
			}
			else if ( r == 1)
			{
				cout << " Alarm for Time Zone " << TMZ << endl;
			}
			else
			{
				cout << "Error detected!!" << endl;
			}
			break;
		}
		case 2:
		{
			time_t UTCTime(NULL);

			int Tz_val = 0;
			struct tm LocalTime;
			cout<<"Converting current time to UTC format..\n";
			time(&UTCTime);

			struct tm *timeinfo;
			timeinfo = gmtime(&UTCTime);
			cout<<"UTC time in string : "<<asctime(timeinfo)<<endl;

			cout<<"\nPlease enter Time Zone Number:\t";
			cin>>Tz_val;

			cout<<"Calling UTCtoLocalTime...\n";
			cout<<"Arguments to function : UTCtoLocalTime("<<UTCTime<<", LocalTime, "<<Tz_val<<")"<<endl;

			int result = MTZObj.UTCtoLocalTime(UTCTime, LocalTime, Tz_val);

			cout<<"Printing the result...\n";
			if( result == 0 )
			{
				cout<<"\n..........Conversion succeed...........\n";
				cout<<"\n\nLocalTime --->\n\n";
				cout<<"\ntm_sec  =   \t"<<LocalTime.tm_sec;
				cout<<"\ntm_min  =   \t"<<LocalTime.tm_min;
				cout<<"\ntm_hour =   \t"<<LocalTime.tm_hour;
				cout<<"\ntm_mday =   \t"<<LocalTime.tm_mday;
				cout<<"\ntm_mon  =   \t"<<LocalTime.tm_mon;
				cout<<"\ntm_year =   \t"<<LocalTime.tm_year;
				cout<<"\ntm_wday =   \t"<<LocalTime.tm_wday;
				cout<<"\ntm_yday =   \t"<<LocalTime.tm_yday;
				cout<<"\ntm_isdst=   \t"<<LocalTime.tm_isdst<<"\n\n";
			}
			else if(result == -1)
			{
				cout<<"\n.........Internal Error Occurred.........\n";
			}
			break;

		}
		case 3:
		{
			time_t UTCTime(NULL);

			int Tz_val = 0;
			struct tm LocalTime;

			getCurrentTime(&LocalTime);

			cout<<"\nPlease enter Time Zone Number:\t";
			cin>>Tz_val;

			UTCTime = MTZObj.LocalTimeToUTC(&LocalTime, Tz_val);

			if(UTCTime != -1)
			{
				cout<<"UTC Time = "<<UTCTime<<endl;

				struct tm timeinfo;

				timeinfo = *gmtime(&UTCTime);
				cout<<"UTC time in string : "<<asctime(&timeinfo)<<endl;
				cout<<"UTC time in ctime string : "<<ctime(&UTCTime)<<endl;
			}
			else if(UTCTime == -1)
			{
				cout<<"\n.........Internal Error Occurred.........\n";
			}

			break;

		}
		case 4:
		{

			int TMZValue, DSSValue, adjustValue;
		time_t offset;

			cout<<"Enter TMZ Value (0 to 23): \t"<<endl;
			cin>>TMZValue;

			cout<<"Arguments to function : CalculateTheOffsetTime("<<TMZValue<<")"<<endl;
			offset = MTZObj.CalculateTheOffsetTime(TMZValue, DSSValue, adjustValue);

			if(offset!=-1)
			{
				cout<<"Offset Time (In Seconds) = "<<offset<<endl;
				cout<<"DSS Value = "<<DSSValue<<endl;
				cout<<"adjustValue value = "<<adjustValue<<endl;
			}
			else if(offset==-1)
			{
				cout<<"\n.........Internal Error Occured.........\n";
			}

			break;
		}
		case 5:
		{
			struct tm cpTime;
			int  result = MTZObj.CPTime(cpTime);

			if(result != -2)
			{
				cout<<"\n..........CP Time Read succeeded...........\n";
				cout<<"\n\nCP Time --->\n\n";
				cout<<"\ntm_sec   =  \t"<<cpTime.tm_sec;
				cout<<"\ntm_min   =  \t"<<cpTime.tm_min;
				cout<<"\ntm_hour  =  \t"<<cpTime.tm_hour;
				cout<<"\ntm_mday  =  \t"<<cpTime.tm_mday;
				cout<<"\ntm_mon   =  \t"<<cpTime.tm_mon;
				cout<<"\ntm_year  =  \t"<<cpTime.tm_year;
				cout<<"\ntm_wday  =  \t"<<cpTime.tm_wday;
				cout<<"\ntm_yday  =  \t"<<cpTime.tm_yday;
				cout<<"\ntm_isdst =  \t"<<cpTime.tm_isdst<<"\n\n";
			}
			else if(result == -2)
			{
				cout<<"\n.........Internal Error Occurred.........\n";
			}

			break;
		}
		case 6:
		{
			#if 0
			ACS_CHB_filehandler objFileHandler;
			time_t CPtime, writtenSec;
			time(&CPtime);
			time(&writtenSec);
			objFileHandler.WriteZoneFileCPtime(CPtime, writtenSec);
			#endif
		}
		default:
		{
			cout<< " Invalid option\n";
			break;
		}
	}

	ACS_CHB_mtzclass::MTZDestroy();
	return 0;
}


