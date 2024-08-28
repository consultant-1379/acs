/*=================================================================== */
/**
   @file acs_hcs_periodicscheduling.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A                       XMALRAO       Initial Release 
==================================================================*/

/*==================================================================
                        INCLUDE DECLARATION SECTION
================================================================== */
#include "acs_hcs_calenderScheduling.h"
#include "acs_hcs_calenderPeriodicEvent_objectImpl.h"
#include "acs_hcs_global_ObjectImpl.h"
#include <ctime>
#include <string>
#include <iostream>

using namespace std;

/*===================================================================
        ROUTINE:jobSchedule constructor
=================================================================== */
jobSchedule::jobSchedule(CALENDER_Parameters *request)
{
	DEBUG("%s","Entering jobSchedule::jobSchedule(CALENDER_Parameters *request) ");
	string dtetme;
	time_t rawtime;
	tm * timeinfo;

	dom=request->DayOfMonth;
	dow=request->DayOfWeek;
	mon=request->Month;
	dowo=request->DayOfWeekOccurence;
	startTimeString = request->StartTime;
	stopTimeString = request->StopTime;

	setCurrentTime();
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	dtetme= string(request->Time);
	sscanf(dtetme.c_str(),"%d:%d:%d",&dtme.tm_hour,&dtme.tm_min,&dtme.tm_sec);
	dtme.tm_year=timeinfo->tm_year+1900;
	dtme.tm_mon=timeinfo->tm_mon+1;
	dtme.tm_mday=timeinfo->tm_mday;
	dtetme=getDateTime();
	calTime = dtetme;
	if(compareDates(dtetme,currentTime))
	{
		nextDay();
		dtetme=getDateTime();
		calTime = dtetme;
	}
	DEBUG("%s","Leaving jobSchedule::jobSchedule(CALENDER_Parameters *request ");
}

/*===================================================================
        ROUTINE:calcDelay
=================================================================== */
void jobSchedule::calcDelay()
{
	//DEBUG("%s","Entering jobSchedule::calcDelay() ");
	currentTime = calTime;
	//DEBUG("%s","Leaving jobSchedule::calcDelay() ");
}

/*===================================================================
        ROUTINE:checkTrigger
=================================================================== */
bool jobSchedule::checkTrigger()
{
	//DEBUG("%s","Entering jobSchedule::checkTrigger() ");
	bool trig=true;
        if(!(mon==0 || mon == dtme.tm_mon))
                trig=false;
        cout<<"WeekNoOfTheDay "<<dowo<<endl;
        int wknod;
        wknod= WeekNoOfTheDay();
        if(wknod==6)
        wknod=5;
        if(!(dowo==0 || dowo == wknod))
                trig=false;
        int dn=dayName();
        if (dn==0)
                dn=7;
        cout<<dn<<endl;
        if(!(dow==0|| dow == dn))
                trig=false;
        if(!(dom==0 || dom==dtme.tm_mday))
                trig=false;
        if(compareDates(currentTime,startTimeString))
        {
                cout<<"In check to trigger "<<currentTime<<"  "<<startTimeString<<endl;
                trig=false;
        }
	//DEBUG("%s","LEaving jobSchedule::checkTrigger() ");	
	return(trig);			
}

/*===================================================================
        ROUTINE:updateNextTriggerTime
=================================================================== */
string jobSchedule::updateNextTriggerTime()
{
	//DEBUG("%s","Entering jobSchedule::updateNextTriggerTime() ");
	nextDay();
	string dteme;
	dteme=getDateTime();
	calTime = dteme;
	//DEBUG("%s","Leaving jobSchedule::updateNextTriggerTime() ");
	return(calTime);	
}

/*===================================================================
        ROUTINE:timeToQuit
=================================================================== */
bool jobSchedule::timeToQuit()
{	
	//DEBUG("%s","Leaving jobSchedule::timeToQuit() ");
	return (!(compareDates(calTime,stopTimeString)));
}

/*===================================================================
        ROUTINE:setCurrentTime
=================================================================== */
void jobSchedule::setCurrentTime()
{
	//DEBUG("%s","Entering jobSchedule::setCurrentTime() ");
	time_t time_raw_format;
	struct tm *timeinfo;

	time (&time_raw_format);
	timeinfo = localtime (&time_raw_format);

	dtme.tm_year=timeinfo->tm_year+1900;
	dtme.tm_mon=timeinfo->tm_mon+1;
	dtme.tm_mday=timeinfo->tm_mday;
	dtme.tm_hour=timeinfo->tm_hour;
	dtme.tm_min=timeinfo->tm_min;
	dtme.tm_sec=timeinfo->tm_sec;

	string ctime;
	ctime=getDateTime();
	currentTime = ctime;
	//DEBUG("%s","Leaving jobSchedule::setCurrentTime( ");
}

/*===================================================================
        ROUTINE:isLeapYear
=================================================================== */
bool jobSchedule::isLeapYear()
{
	//DEBUG("%s","Entering jobSchedule::isLeapYear() ");	
	if((dtme.tm_year%4==0 && dtme.tm_year %100!=0 )|| (dtme.tm_year%400==0))
	{
		DEBUG("%s"," ");
		return true;
	}
	//DEBUG("%s","Leaving jobSchedule::isLeapYear() ");
	return(false);	
}

/*===================================================================
        ROUTINE:nextDay
=================================================================== */
void jobSchedule::nextDay()
{
	//DEBUG("%s","Entering jobSchedule::nextDay() ");
	dtme.tm_mday+=1;

	if(dtme.tm_mon == FEB && dtme.tm_mday==29)
	{

		if(!isLeapYear())
		{
			dtme.tm_mday=1;
			dtme.tm_mon=3;
		}
	}
	else if(dtme.tm_mday > lastDay[dtme.tm_mon-1])
	{
		dtme.tm_mday=1;

		if(dtme.tm_mon==DEC)
		{
			dtme.tm_mon=JAN;
			dtme.tm_year++;
		}
		else
			dtme.tm_mon++;
	}
	//DEBUG("%s","Leaving jobSchedule::nextDay() ");
}

/*===================================================================
        ROUTINE:dayname
=================================================================== */
int jobSchedule::dayName() 
{
 	//DEBUG("%s","Entering jobSchedule::dayName() ");
	 size_t JND = dtme.tm_mday \
	 + ((153 * (dtme.tm_mon + 12 * ((14 - dtme.tm_mon) / 12) - 3) + 2) / 5) \
	 + (365 * (dtme.tm_year + 4800 - ((14 - dtme.tm_mon) / 12)))              \
	 + ((dtme.tm_year + 4800 - ((14 - dtme.tm_mon) / 12)) / 4)                \
	 - ((dtme.tm_year + 4800 - ((14 - dtme.tm_mon) / 12)) / 100)              \
	 + ((dtme.tm_year + 4800 - ((14 - dtme.tm_mon) / 12)) / 400)              \
	 - 32045;

	int dname;
	dname=JND%7+1;
	if(dname==7)
		dname=0;
	//DEBUG("%s","Leaving jobSchedule::dayName() ");
	return(dname);	 
}

/*===================================================================
        ROUTINE:lastDayOfMonth
=================================================================== */
int jobSchedule::lastDayOfMonth() 
{
	//DEBUG("%s","Entering jobSchedule::lastDayOfMonth() ");
	int lDay;

	lDay=lastDay[dtme.tm_mon-1];

	if(dtme.tm_mon == FEB && isLeapYear())
		lDay=29;
	//DEBUG("%s","Leaving jobSchedule::lastDayOfMonth() ");
	return(lDay);	
}

/*===================================================================
        ROUTINE:WeekNoOfTheDay
=================================================================== */
int jobSchedule::WeekNoOfTheDay() 
{
	//DEBUG("%s","Entering jobSchedule::WeekNoOfTheDay() ");
	int wkname,tmpDay,wkno,lDOM;
	int tmpday=dtme.tm_mday;

	dtme.tm_mday=1;
	wkname = dayName();
	wkno=1;
	dtme.tm_mday=tmpday;
	tmpDay=1;
	lDOM=lastDayOfMonth();

	while(tmpDay!=dtme.tm_mday && tmpDay<=lDOM)
	{
		while(wkname<7)
		{
			if(tmpDay==dtme.tm_mday)
				return(wkno);
			tmpDay++;	 
			wkname++;
		}
		wkno++;
		wkname=0;
	}
	//DEBUG("%s","Leaving jobSchedule::WeekNoOfTheDay() ");
	return(wkno);
}

/*===================================================================
        ROUTINE:getDateTime
=================================================================== */
string jobSchedule::getDateTime()
{
	//DEBUG("%s","Entering jobSchedule::getDateTime() ");
	char dtm[100];
	sprintf(dtm,"%04d-%02d-%02dT%02d:%02d:%02d",dtme.tm_year,dtme.tm_mon,dtme.tm_mday,dtme.tm_hour,dtme.tm_min,dtme.tm_sec);
	//DEBUG("%s","Leaving jobSchedule::getDateTime() ");
	return(string(dtm));
}

/*===================================================================
        ROUTINE:compareDates
=================================================================== */
bool jobSchedule::compareDates(string d1, string d2)
{
	//DEBUG("%s","Entering jobSchedule::compareDates ");
	acs_hcs_global_ObjectImpl obj;	
	string NearestTime = obj.getNearestTime(d1,d2);

	if( NearestTime.substr(0,19) == d1.substr(0,19) )
		return true;

	else
		return false;
}

/*===================================================================
        ROUTINE:isCombinationPossible
=================================================================== */
bool jobSchedule::isCombinationPossible()
{
	DEBUG("%s","Entering jobSchedule::isCombinationPossible ");
	if(mon != ALL && dom != ALL)
	{
		int lastDateOfMonth = lastDay[mon-1];
		if(mon == 2)
			lastDateOfMonth=lastDateOfMonth+1;			
		if( lastDateOfMonth < dom )
			return false;
	}

	if(dom != ALL && dowo != ALL)
	{
		if ( dom == 1 && dowo != FIRST)				return false;
		else if( dom > 28 && dowo != LAST)			return false;
		else if( dowo == FIRST  && !(dom < 8) )			return false;
		else if( dowo == SECOND && !(dom >1 && dom < 15))	return false;
		else if( dowo == THIRD && !(dom > 7 && dom < 22))	return false;
		else if( dowo == FOURTH && !(dom > 15&& dom < 29))	return false;
		else							return true;
	}

	return true;
}
