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
#include "acs_hcs_periodicscheduling.h" 
#include <iostream>
#include <string>
#include "acs_hcs_calenderScheduling.h"

using namespace std;

/*===================================================================
                   ROUTINE: constructor
=================================================================== */
acs_hcs_periodicscheduling::acs_hcs_periodicscheduling()
{
}

/*===================================================================
                   ROUTINE: destructor
=================================================================== */
acs_hcs_periodicscheduling::~acs_hcs_periodicscheduling()
{
}

/*===================================================================
                   ROUTINE: nextSchedule
=================================================================== */
string acs_hcs_periodicscheduling::nextSchedule(string d1,string d2,int day)
{
	DEBUG("%s","Entering  acs_hcs_periodicscheduling::nextSchedule");
	cout<<"In acs_hcs_periodicscheduling::nextSchedule()"<<endl;
	DT dt1;
	TM t1;

	char nextDate[100];
	string nxtDay;

	sscanf(d1.c_str(),"%4d-%2d-%2dT%2d:%2d:%2d",&dt1.yy,&dt1.mon,&dt1.dd,&dt1.hh,&dt1.mm,&dt1.ss);
	sscanf(d2.c_str(),"%d-%d-%dT%d:%d:%d",&t1.mon,&t1.wk,&t1.dd,&t1.hh,&t1.mm,&t1.ss);

	dt1.yy = dt1.yy + (t1.mon / 12);
	dt1.mon = dt1.mon + (t1.mon % 12);
	if( dt1.mon > 12 )
	{
		dt1.mon = dt1.mon % 12;
		dt1.yy++;
	}

	/*int lday;

	lday = lastDayOfMonth(dt1);
	if( day > lday)
	dt1.dd = lday; */
	/*This if block will execute only when operator provides some value for months attribute and a startdate with a value greater then 28.
	Ex : If periodicity is 1 month and starttime is jan 31 then scheduling would be as follows feb 28/29, mar 31, apr 30 ....
	To handle the above peculiar case this code snippet was written.*/
	if(t1.mon != 0)
	{
		int lastDay = lastDayOfMonth(dt1);
		switch(day)
		{
			case 31 :
				dt1.dd = lastDay;
				break;
		
			case 30 :
			case 29 :
				if( dt1.mon == 2)
					dt1.dd = lastDay;
				else
					dt1.dd = day;
				break;

			default :
				break;
		}
	} 

	addWeeks(dt1,t1.wk);

	addDays(dt1,t1.dd);
		
	addTime(dt1,t1); 
		
	if(dt1.yy>9999)
		nxtDay=NONEXTSCHEDULE;
	else      
	{
		sprintf(nextDate,"%04d-%02d-%02dT%02d:%02d:%02d",dt1.yy,dt1.mon,dt1.dd,dt1.hh,dt1.mm,dt1.ss);
		nxtDay=string(nextDate);
	}

	cout<<"Next scheduled date is "<<nxtDay<<endl;	
	DEBUG("%s","Leaving  acs_hcs_periodicscheduling::nextSchedule");
	return(string(nxtDay));
}

/*===================================================================
                   ROUTINE: addDates
=================================================================== */
void acs_hcs_periodicscheduling::addDates(DT &dt,DT dt1)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::addDates");
	int dd;
	int hh;
	int mm;
	int ss;
	
	ss = dt.ss+dt1.ss;
	dt.ss = ss % 60;
	dt.mm = dt.mm + (ss /60);
	
	mm = dt.mm + dt1.mm;

	dt.mm = mm % 60;
	dt.hh = dt.hh + (mm / 60);

	hh= dt.hh + dt1.hh;

	dt.hh = hh % 24;
	dd= hh / 24;

	dd = dd+dt1.dd;
	
	addDays(dt,dd);
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::addDates");	
}

/*===================================================================
                   ROUTINE:convHour 
=================================================================== */
void acs_hcs_periodicscheduling::convHour(DT &dt,TM t1)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::convHour");
	DT dt1;

	dt1.yy=0;
	dt1.mon=0;
	dt1.dd=0;
	dt1.hh=0;
	dt1.mm=0;
	dt1.ss=0;
	
	dt1.dd = t1.hh / 24;
	dt1.hh = t1.hh % 24;

	addDates(dt,dt1);
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::convHour");
}

/*===================================================================
                   ROUTINE:convMin 
=================================================================== */
void acs_hcs_periodicscheduling::convMin(DT &dt,TM t1)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::convMin");
	DT dt1;

	dt1.yy=0;
	dt1.mon=0;
	dt1.dd=0;
	dt1.hh=0;
	dt1.mm=0;
	dt1.ss=0;
	
	dt1.mm = t1.mm %60;
	dt1.hh = t1.mm / 60;

	dt1.dd = dt1.hh / 24;
	dt1.hh = dt1.hh % 24;

	addDates(dt,dt1);
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::convMin");
}

/*===================================================================
                   ROUTINE:convSec 
=================================================================== */
void acs_hcs_periodicscheduling::convSec(DT &dt,TM t1)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::convSec");
	DT dt1;

	dt1.yy=0;
	dt1.mon=0;
	dt1.dd=0;
	dt1.hh=0;
	dt1.mm=0;
	dt1.ss=0;

	dt1.ss = t1.ss % 60;
	dt1.mm = dt1.mm + (t1.ss / 60);
	
	dt1.hh = dt1.mm / 60;
	dt1.mm = dt1.mm % 60;

	dt1.dd = dt1.hh / 24;
	dt1.hh = dt1.dd % 24;

	addDates(dt,dt1);
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::convSec");
}

/*===================================================================
                   ROUTINE: addTime
=================================================================== */
void acs_hcs_periodicscheduling::addTime(DT &dt,TM t1)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::addTime");
	convSec(dt,t1);
	convMin(dt,t1);
	convHour(dt,t1);
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::addTime");
}

/*===================================================================
                   ROUTINE:addWeeks 
=================================================================== */
void acs_hcs_periodicscheduling::addWeeks(DT &dt,int wk)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::addWeeks");
	int i=1;
	while(i<=wk)
	{
		addDays(dt,WK);
		i++;
	}
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::addWeeks");
}

/*===================================================================
                   ROUTINE:addDays 
=================================================================== */
void acs_hcs_periodicscheduling::addDays(DT &dt,int days)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::addDays");
	DT dt2;
	int i=1;
	while(i<=days)
	{
		nextDay(dt,dt2);

		dt.yy=dt2.yy; /* Copy dt2 to dt */
		dt.mon=dt2.mon;
		dt.dd=dt2.dd;
		i++;	
	}
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::addDays");
}

/*===================================================================
                   ROUTINE:isLeapYear 
=================================================================== */
bool acs_hcs_periodicscheduling::isLeapYear(DT dt)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::isLeapYear");
	if((dt.yy%4==0 && dt.yy %100!=0) || (dt.yy%400==0))
		return true;
	return(false);	
}

/*===================================================================
                   ROUTINE: nextDay
=================================================================== */
void acs_hcs_periodicscheduling::nextDay(DT crntDay, DT &nxtDay)
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::nextDay");
	nxtDay.dd=crntDay.dd+1;
	nxtDay.mon=crntDay.mon;
	nxtDay.yy=crntDay.yy;

	if(nxtDay.dd > lastDay[nxtDay.mon-1])
	{
		if( nxtDay.mon == FEB )
		{
			if(isLeapYear(nxtDay) && nxtDay.dd == lastDay[nxtDay.mon-1] +1 )
			{
				nxtDay.dd=29;
			}
			else
			{
				nxtDay.dd=1;
				nxtDay.mon=3;
			}
		}
		else
		{
			nxtDay.dd=1;

			if(nxtDay.mon==DEC)
			{
				nxtDay.mon=JAN;
				nxtDay.yy++;
			}
			else
			{
				nxtDay.mon++;
			}
		}
	}
	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::nextDay");
}

/*===================================================================
                   ROUTINE:lastDayOfMonth 
=================================================================== */
int acs_hcs_periodicscheduling::lastDayOfMonth(DT &dt) 
{
	//DEBUG("%s","Entering  acs_hcs_periodicscheduling::lastDayOfMonth");
	int lDay;
	
	lDay=lastDay[dt.mon-1];
	
	if(dt.mon == FEB && isLeapYear(dt))
		lDay=29;

	//DEBUG("%s","Leaving  acs_hcs_periodicscheduling::lastDayOfMonth");
	return(lDay);	
}
