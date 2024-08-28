/*=================================================================== */
   /**
   @file acs_chb_tz.cpp

   Class method implementation for CHB module.

   This module contains the implementation of
   the acs_chbtimezone.

   @version 1.0.0
        */
        /*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       06/06/2011   XTANAGG   Initial Release
        */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <time.h>
#include <ace/ACE.h>
#include <acs_chb_mtz_common.h>
using namespace std;

/*=================================================================== */
/**
        @brief          This function calculates the difference between
                                two time structures.

        @param          newtm
                                Time 1.

        @param          oldtm
                                Time 2.

        @return         ACE_INT32
                                Difference between the two time structures in seconds.
*/
/*=================================================================== */
ACE_INT32 diff(struct tm &newtm, struct tm &oldtm);

/*=================================================================== */
/**

        @brief          This function gives the nearest time when the day light
                                saving changes.

        @param          lot
                                Time 1

        @param          hit
                                Time 2.

        @return         time_t
*/
/*=================================================================== */
time_t getLargestTime(time_t lot, time_t hit);


/*=================================================================== 
	ROUTINE: main
********************************************************************* */
int main(int argc, char *argv[])
{
	if( argc != 2 )
	{
		//return -1;
		exit(1);
	}
	

	TIME_ZONE_INFORMATION *tzi = 0;
	tzi = new TIME_ZONE_INFORMATION();
	if( tzi == 0 )
	{
		//return -1;
		exit(1);
	}
	
	memset( tzi , 0, sizeof(TIME_ZONE_INFORMATION));

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

	char* zoneString = 0;
	zoneString = new char[strlen(argv[1]) + 1];
	if( zoneString == 0 )
	{
		delete tzi;
		tzi = 0;
		//return -1;
		exit(1);
	}
	memset(zoneString, 0, strlen(argv[1]) + 1 );
	strcpy(zoneString, argv[1]);

	char sign = '-';
	char* TZString=0, *p=0;
	char pHours[10];
	memset(pHours,0, sizeof(pHours));
	char pMins[10];
	memset(pMins,0, sizeof(pMins));

	//Time zone name should start with (.
	if( zoneString[0] != '(')
	{
		delete tzi;
		tzi = 0;
		delete[] zoneString;
		zoneString = 0;
		//return -1;
		exit(1);
	}

	if(( p = strchr(zoneString, ')')) != 0)
	{
		TZString = p+1;
	}
	else
	{
		delete tzi;
		tzi = 0;
		delete[] zoneString;
		zoneString = 0;
		//return -1;
		exit(1);
	}

	//Calculate the offset.
	char* pBias = 0;
	pBias = strstr(zoneString, "GMT");

	pBias = pBias + strlen("GMT");

	//Get the sign of time zone.
	if( *pBias == '-' || *pBias == '+')
	{
		sign = *pBias;
	}

	//Increment pBias
	pBias = pBias + 1;

	int j = 0;
	//Time Zone will be in the format
	//(GMT+01:00)America/New_York
	//Copy the hours.
	while(pBias != 0 && *pBias !=  ':' && *pBias != ')' )
	{
		if( !isdigit(*pBias))
		{
			delete tzi;
			tzi = 0;
			delete[] zoneString;
			zoneString = 0;
			//return -1;
			exit(1);

		}
		else
		{
			pHours[j] = *pBias;
			j++;
		}
		pBias = pBias + 1;
	}

	pBias = pBias + 1;
	j =0;
	//Copy the Minutes.
	while( pBias != 0 && *pBias != ')')
	{
		if( !isdigit(*pBias))
		{
			delete tzi;
			tzi = 0;

			delete[] zoneString;
			zoneString = 0;
		
			//return -1;
			exit(1);
		

		}
		else
		{
			pMins[j] = *pBias;
			pBias = pBias + 1;
			j++;
		}

	}
	int hours = atol(pHours);

	int mins = atol(pMins);

	ACE_INT32 StandardBias = hours*MINSPERHOUR + mins;

	if( sign != '-')
	{
		StandardBias = -StandardBias;
	}

	//Set the environment variable
	char *tz = getenv("TZ");
	setenv("TZ",TZString,1);

	time_t currTimeSec = 0;
	time(&currTimeSec);
	time_t cTime = 0;
	struct tm* epochTime = localtime(&currTimeSec);
	long currYear = epochTime->tm_year;
	for (long y = 1970; y < (epochTime->tm_year + TM_YEAR_BASE ); ++y)
		cTime += DAYSPERNYEAR + isleap(y);
	cTime *= SECSPERHOUR * HOURSPERDAY;
	epochTime = localtime(&cTime);
	time_t t = cTime - SECSPERHOUR*epochTime->tm_hour - SECSPERMIN*epochTime->tm_min - epochTime->tm_sec;
	time_t t1 =t;
	int  year = 0;
	struct tm newtm;
	struct tm tm;
	bool isDST = false;
	if( epochTime->tm_isdst == 0)
	{
		isDST = false;
	}
	else
	{
		isDST = true;
	}
	for( ; ; )
	{
		time_t newt;
		tm = *localtime(&t);
		newt = t + SECSPERHOUR * HOURSPERDAY;
		newtm = *localtime(&newt);
		year = newtm.tm_year;
		if( year > currYear)
		{
			break;
		}

		if( newtm.tm_isdst != tm.tm_isdst)
	       {
			newt = getLargestTime(t, newt);
			newtm = *localtime(&newt);
			isDST = true;
			if( newtm.tm_isdst == 0 )
			{
				struct tm  mt1 = *gmtime(&newt);
				tzi->Bias = diff(mt1, newtm)/SECSPERMIN;

				//Copy the date fields
				tzi->StandardDate.wDay = newtm.tm_mday;
				tzi->StandardDate.wDayOfWeek = newtm.tm_wday;
				tzi->StandardDate.wHour = newtm.tm_hour;
				tzi->StandardDate.wMilliseconds = 0;
				tzi->StandardDate.wMinute = newtm.tm_min;
				tzi->StandardDate.wMonth = newtm.tm_mon + 1; //Month is linux are from 0 to 11 in wind 1-12
				tzi->StandardDate.wSecond = newtm.tm_sec;
				tzi->StandardDate.wYear = newtm.tm_year + TM_YEAR_BASE;

				ACE_OS::strcpy(tzi->StandardName, newtm.tm_zone);

			}
			else if( newtm.tm_isdst == 1)
			{
				//tzi.DaylightDate = newtm;
				struct tm  mt1 = *gmtime(&newt);
				tzi->DaylightBias = diff(mt1, newtm)/SECSPERMIN;
				ACE_OS::strcpy(tzi->DaylightName, newtm.tm_zone);

				//Copy the date fields
				tzi->DaylightDate.wDay = newtm.tm_mday;
				tzi->DaylightDate.wDayOfWeek = newtm.tm_wday;
				tzi->DaylightDate.wHour = newtm.tm_hour;
				tzi->DaylightDate.wMilliseconds = 0;
				tzi->DaylightDate.wMinute = newtm.tm_min;
				tzi->DaylightDate.wMonth = newtm.tm_mon + 1; //Month is linux are from 0 to 11 in wind 1-12
				tzi->DaylightDate.wSecond = newtm.tm_sec;
				tzi->DaylightDate.wYear = newtm.tm_year + TM_YEAR_BASE;
			}
		}
		t = newt;
	}
	if( !isDST)
	{
		struct tm  mt1 = *gmtime(&t1);
		time_t tt1 = mktime(&mt1);
		int biasSec = static_cast<int>(difftime(tt1,t1));
		struct tm localtm1 = *localtime(&t1);
		tzi->Bias= biasSec/SECSPERMIN;

		strcpy(tzi->StandardName, localtm1.tm_zone);
	}
	else
	{
                        tzi->DaylightBias =  tzi->DaylightBias - tzi->Bias;
                                                                                    
	 }

	if( tzi->Bias != StandardBias )
	{
		delete[] zoneString;
		zoneString = 0;

		delete[] zoneString;
		zoneString = 0;

		if( tz )
		{
			setenv("TZ",tz,1);
		}
		else
		{
			unsetenv("TZ");
		}
		//return -1;
		exit(1);
	}

	delete[] zoneString;
	zoneString = 0;
	if( tz )
	{
		setenv("TZ",tz,1);
	}
	else
	{
		unsetenv("TZ");
	}
	tzset();

	sprintf( buffer, "%d,%s,%u,%u,%u,%u,%u,%u,%u,%u,%d,%s,%u,%u,%u,%u,%u,%u,%u,%u,%d,",
		tzi->Bias, tzi->StandardName, tzi->StandardDate.wYear, tzi->StandardDate.wMonth,
		tzi->StandardDate.wDayOfWeek, tzi->StandardDate.wDay, tzi->StandardDate.wHour,
		tzi->StandardDate.wMinute,  tzi->StandardDate.wSecond,  tzi->StandardDate.wMilliseconds,
		tzi->StandardBias,  tzi->DaylightName,  tzi->DaylightDate.wYear,  tzi->DaylightDate.wMonth,
		tzi->DaylightDate.wDayOfWeek,  tzi->DaylightDate.wDay,  tzi->DaylightDate.wHour,
		tzi->DaylightDate.wMinute,  tzi->DaylightDate.wSecond,  tzi->DaylightDate.wMilliseconds,
		tzi->DaylightBias) ;

	delete tzi;
	tzi = 0;

	#if 0

	ACE_stat exists;
	if (ACE_OS::stat (ACE_TEXT(ACS_CHB_TIMEZONEFILE_DIRPATH), &exists) != 0)
	{
		int status = ACE_OS::mkdir(ACE_TEXT(ACS_CHB_TIMEZONEFILE_DIRPATH), S_IRWXU | S_IRWXG );
		if ( status != 0 )
		{
			//return -1;
			exit(1);
		}
	}


	ACE_HANDLE fd = ACE_OS::open(ACE_TEXT(ACS_CHB_TIMEZONEFILE), O_CREAT | O_TRUNC | O_WRONLY );

	if (fd != ACE_INVALID_HANDLE)
	{
		ACE_OS::write(fd, buffer, sizeof(buffer));
		ACE_OS::close(fd);
        }
        else
        {
                //return -1;
		exit(1);
        }

	#endif

	cout << buffer;

	//return 0;
	exit(0);
}

/*==============================================================
	ROUTINE: diff
================================================================*/
ACE_INT32 diff(struct tm &newtm, struct tm &oldtm)
{
        ACE_INT32       result = 0;
        ACE_INT32       yrCntr  = 0;

        if (newtm.tm_year < oldtm.tm_year)
        {
             return -diff(oldtm, newtm);
        }
        result = 0;
        for (yrCntr = oldtm.tm_year; yrCntr < newtm.tm_year; ++yrCntr)
	{
                result += DAYSPERNYEAR + isleap(yrCntr + TM_YEAR_BASE);
	}
        result += newtm.tm_yday - oldtm.tm_yday;
        result *= HOURSPERDAY;
        result += newtm.tm_hour - oldtm.tm_hour;
        result *= MINSPERHOUR;
        result += newtm.tm_min - oldtm.tm_min;
        result *= SECSPERMIN;
        result += newtm.tm_sec - oldtm.tm_sec;
        return result;
}

/*====================================================================
	ROUTINE: getLargestTime
======================================================================*/
time_t getLargestTime(time_t lot, time_t hit)
{
        time_t          t;
        struct tm       lotm;
        struct tm       tm;

        lotm = *localtime(&lot);
        while ((hit - lot) >= 2) {
                t = lot / 2 + hit / 2;
                if (t <= lot)
                        ++t;
                else if (t >= hit)
                        --t;
                tm = *localtime(&t);
                if (diff(tm, lotm) == (t - lot) &&
                                tm.tm_isdst == lotm.tm_isdst ) {
                        lot = t;
                        lotm = tm;
                } else  hit = t;
        }
        return hit;
}



