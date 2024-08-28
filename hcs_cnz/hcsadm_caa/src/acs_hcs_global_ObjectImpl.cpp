/*=================================================================== */
/**
   @file acs_hcs_global_ObjectImpl.cpp

   Class method implementation for HC module.

   This module contains the functions which are commonly used across all OI handlers
   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A		         XMALRAO       Initial Release
=========================================================================*/

/*==================================================================
                INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <ctime>
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_hcs_global_ObjectImpl.h"
#include "acs_hcs_calenderScheduling.h"
#include "acs_hcs_periodicscheduling.h"
#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_progressReportHandler.h"
#include "acs_hcs_jobscheduler.h"

using namespace std;

/*===================================================================
        ROUTINE: acs_hcs_global_ObjectImpl
=================================================================== */
acs_hcs_global_ObjectImpl::acs_hcs_global_ObjectImpl()
{
}

/*===================================================================
        ROUTINE: ~acs_hcs_global_ObjectImpl
=================================================================== */
acs_hcs_global_ObjectImpl::~acs_hcs_global_ObjectImpl()
{
}

/*===================================================================
        ROUTINE: date_format_validation
=================================================================== */
ACE_INT32 acs_hcs_global_ObjectImpl::date_format_validation(string startTime)
{
	DEBUG("%s"," Entering  acs_hcs_global_ObjectImpl::date_format_validation(string startTime)");
	int dayDD,monthMM,yearYY;
	yearYY = atoi((startTime.substr(0,4)).c_str());
	dayDD = atoi((startTime.substr(8,2)).c_str());
	monthMM = atoi((startTime.substr(5,2)).c_str());

	if(monthMM == 2)
	{
	 	 // To check for number of days in leap year
		if(((yearYY % 4 == 0) && !(yearYY % 100 == 0))|| (yearYY % 400 == 0))
 		{
     			if((dayDD < 1) || (dayDD > 29))
     			{
				DEBUG("%s"," returning from  acs_hcs_global_ObjectImpl::date_format_validation  no of days");
		        	 return ACS_HC_INVALID_TIME;
     			}
 		}
	 	else if((dayDD < 1) || (dayDD > 28))
 		{
			DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::date_format_validation after no of days");
		    	 return ACS_HC_INVALID_TIME;
 		}
	}

	if(monthMM == 1 || monthMM == 3 || monthMM == 5 || monthMM == 7 || monthMM == 8 || monthMM == 10 || monthMM == 12)
	{
		if( dayDD < 1 || dayDD > 31)
        	{
			 DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::date_format_validation days");
               		return ACS_HC_INVALID_TIME;
        	}
	}
	else
	{
        	if( dayDD < 1 || dayDD > 30)
        	{
			 DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::date_format_validation(string startTime)");
	                return ACS_HC_INVALID_TIME;
        	}
	}

	DEBUG("%s","Leaving  acs_hcs_global_ObjectImpl::date_format_validation(string startTime) ");
	return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE: getCurrentTime
=================================================================== */
string acs_hcs_global_ObjectImpl::getCurrentTime()
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getCurrentTime() ");	
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	char currentTime[32];
    	sprintf(currentTime, "%04d-%02d-%02dT%02d:%02d:%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    	string temp(currentTime);
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getCurrentTime() ");
	return temp;
}

/*===================================================================
        ROUTINE: timezone_validation
=================================================================== */
ACE_INT32 acs_hcs_global_ObjectImpl::timezone_validation(string startTime)
{
	DEBUG("%s","Entering  acs_hcs_global_ObjectImpl::timezone_validation(string startTime) ");
	string sec_str = startTime.substr(startTime.size()-2,2);
	string sign = startTime.substr(startTime.size()-6,1);
	int hour_str = atoi(startTime.substr(startTime.size()-5,2).c_str());

	if( sec_str == "45")
	{
		if( !(sign == "+" && ( hour_str == 5 || hour_str == 12 || hour_str == 8 )) )
		{
			 DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::timezone_validation after invalid time zone ");
			return ACS_HC_INVALID_TIMEZONE;
		}
	}
	else if( sec_str == "30")
	{
		if(sign == "-")
		{
			if( !(hour_str == 3 || hour_str ==4 || hour_str == 9) )
			{	
				DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::timezone_validation after time zone ");
				return ACS_HC_INVALID_TIMEZONE;
			}
		}
		else
		{
			if(!(hour_str == 3 || hour_str ==4 || hour_str == 5 || hour_str == 6 || hour_str == 9 || hour_str == 10 || hour_str == 11))
			{
				DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::timezone_validation invalid time zone ");
				return ACS_HC_INVALID_TIMEZONE;
			}
		}
	}
	else if( sec_str == "00" )
	{
		if( sign == "+" && !( hour_str >=0 && hour_str <=14) )
		{
			DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::timezone_validation after invalid time zone ");	
			return ACS_HC_INVALID_TIMEZONE;
		}
		else if( sign == "-" && !( hour_str >=0 && hour_str <=12 ))
		{
			DEBUG("%s"," return from  acs_hcs_global_ObjectImpl::timezone_validation after invalid time zone ");
			return ACS_HC_INVALID_TIMEZONE;
		}
	}
	else
	{
		DEBUG("%s","return from  acs_hcs_global_ObjectImpl::timezone_validation after invalid time zone ");
		return ACS_HC_INVALID_TIMEZONE;
	}
	DEBUG("%s","Leaving  acs_hcs_global_ObjectImpl::timezone_validation(string startTime) ");
	return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE: updateImmAttribute
=================================================================== */
ACS_CC_ReturnType acs_hcs_global_ObjectImpl::updateImmAttribute(string rdn,string attr,ACS_CC_AttrValueType type,void** value)
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::updateImmAttribute");
        char *attribute = const_cast<char *>(attr.c_str());
        ACS_CC_ReturnType returnCode;
        OmHandler immHandler;
        ACS_CC_ImmParameter ImmParameter;
        ImmParameter.attrName=attribute;
        ImmParameter.attrType=type;
        ImmParameter.attrValuesNum=1;
        ImmParameter.attrValues = value;

        returnCode = immHandler.Init();
        if(returnCode!=ACS_CC_SUCCESS)
        {
                cout<<"ERROR: init FAILURE!!!\n";
        }
        returnCode = immHandler.modifyAttribute(rdn.c_str(),&ImmParameter);
        if(returnCode == ACS_CC_FAILURE)
        {
		DEBUG("%s","IMM modify attribute failure ");
		DEBUG("%s%s","Creation of progress failed", immHandler.getInternalLastErrorText());
                DEBUG("%s%d","error code is: ", immHandler.getInternalLastError());
                cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
                cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
        }
        else
        {
		DEBUG("%s","Imm object is updated");
                cout<<"Imm object is updated"<<endl;
        }
	
        immHandler.Finalize();
	//DEBUG("%s","acs_hcs_global_ObjectImpl::updateImmAttribute");
	return returnCode;
}

/*===================================================================
        ROUTINE: timeValidation
=================================================================== */
ACE_INT32 acs_hcs_global_ObjectImpl::timeValidation(string &time, string attributeType)
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::timeValidation(string time, string attributeType) ");
	int returnCode = -1;
	size_t Z_pos,plus_pos,minus_pos;
	string currentTime = getCurrentTime();

	returnCode=date_format_validation(time); //check whether format is correct or not
	if(returnCode !=0)
	{
		DEBUG("%s"," return after invalid date format");
		return returnCode;
	}

	if( time.size() > 19)
	{
		string temp = time.substr(19,time.size()-19);
		Z_pos 	 = temp.find('Z');
		plus_pos  = temp.find('+'); 	
		minus_pos = temp.find('-');	
	
		if( !(Z_pos != std::string::npos) && ((minus_pos != std::string::npos) || (plus_pos != std::string::npos)))
		{
			returnCode=timezone_validation(time);
			if(returnCode != 0)
			{
				DEBUG("%s","return after invalid timezone in timeValidation ");
				return ACS_HC_INVALID_TIMEZONE;
			}
			time = localZoneTime(time);
		}
	}

	if( attributeType != stopTimeAttr)
        {
		//DEBUG("From time validation %s", time);
                //time = localZoneTime(time);
                if( compareDates(currentTime,time) == false ) //condition to check provided time is a future time
                {
                        DEBUG("%s"," return after starttime expired in timeValidation");
                        return ACS_HC_STARTTIME_EXPIRED;
                }
        }

	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::timeValidation(string time, string attributeType) ");
	return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE: getImmAttribute
=================================================================== */
string acs_hcs_global_ObjectImpl::getImmAttribute(string dnName,string attributeName)
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getImmAttribute(string dnName,string attributeName ");
	char *attribute = const_cast<char *>(attributeName.c_str());
	char time[128]={0};
	OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
        ACS_CC_ImmParameter ImmParameter;
	ImmParameter.attrName = attribute;

	returnCode = immHandler.Init();
        if(returnCode != ACS_CC_SUCCESS)
        {
                cout<<"Error in getting stopTime"<<endl;
        }
	else
	{
		returnCode = immHandler.getAttribute(dnName.c_str(), &ImmParameter);
		if(returnCode == ACS_CC_SUCCESS)
        	{
			if(ImmParameter.attrValuesNum)
                                        strncpy(time,reinterpret_cast<char*>(ImmParameter.attrValues[0]),sizeof(time)-1);

                }
	}
	string Time(time);

	immHandler.Finalize();
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getImmAttribute(string dnName,string attributeName ");
	return Time;
}

/*===================================================================
        ROUTINE: getImmIntAttribute
=================================================================== */
unsigned int acs_hcs_global_ObjectImpl::getImmIntAttribute(string dnName,string attributeName)
{
        //DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getImmIntAttribute(string dnName,string attributeName ");
        char *attribute = const_cast<char *>(attributeName.c_str());
        OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
        ACS_CC_ImmParameter ImmParameter;
        ImmParameter.attrName = attribute;
	int value = 0;

        returnCode = immHandler.Init();
        if(returnCode != ACS_CC_SUCCESS)
        {
                cout<<"Error in getting stopTime"<<endl;
        }
        else
        {
                returnCode = immHandler.getAttribute(dnName.c_str(), &ImmParameter);
                if(returnCode == ACS_CC_SUCCESS)
                {
                        value = *(reinterpret_cast<int*>(ImmParameter.attrValues[0]));
                }
        }

        immHandler.Finalize();
        //DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getImmAttribute(string dnName,string attributeName ");
        return value;
}

/*===================================================================
        ROUTINE: updateNextScheduledTime
=================================================================== */
void acs_hcs_global_ObjectImpl::updateNextScheduledTime(string eventRDN,vector<eventDetails> events,int eventType, int changeType)
{
        DEBUG("%s","Entering acs_hcs_global_ObjectImpl::updateNextScheduledTime(string eventRDN) ");
        size_t pos = eventRDN.find_first_of(',');
        string nextScheduleTime;
        string jobSchedulerRDN = eventRDN.substr(pos+1,eventRDN.size());
        eventRDN = parseRDN(eventRDN)+ (string)COMMA_STR;
	ACS_CC_ReturnType returnCode;
        set<std::string> scheduledTimes;

	int size = events.size();
	if( size != 0 && changeType == CREATE)
	{
		cout<<"In updateNextScheduledTime"<<endl;
		DEBUG("%s","updateNextScheduledTime called after event creation");
		for( int i=0; i<size; i++)
		{
			if(events[i].scheduleDate != NONEXTSCHEDULE) 
				scheduledTimes.insert(events[i].scheduleDate);
		}

		getSchedulesOfSingleEvents(scheduledTimes,eventRDN);
	        getSchedulesOfPeriodicEvents(scheduledTimes,eventRDN);
        	getSchedulesOfCalendarEvents(scheduledTimes,eventRDN);

		cout<<"number of events "<<scheduledTimes.size()<<endl;
	}
	else if( size != 0 && changeType == MODIFY)
	{
		getSchedulesOfSingleEvents(scheduledTimes,eventRDN,events,eventType);
	        getSchedulesOfPeriodicEvents(scheduledTimes,eventRDN,events,eventType);
        	getSchedulesOfCalendarEvents(scheduledTimes,eventRDN,events,eventType);
	}
	else if( size == 0)
	{
		getSchedulesOfSingleEvents(scheduledTimes,eventRDN);
                getSchedulesOfPeriodicEvents(scheduledTimes,eventRDN);
                getSchedulesOfCalendarEvents(scheduledTimes,eventRDN);
	}

        if(scheduledTimes.size()!= 0)
        {
                set<string>::iterator it = scheduledTimes.begin();
                nextScheduleTime = *it;
                char* temp =  const_cast<char*>(nextScheduleTime.c_str());
                void* value[1]={ reinterpret_cast<void*>(temp)};
		cout<<"job scheduler rdn is "<<jobSchedulerRDN<<endl;
		for(int i=0; i<20 ;i++)
		{
         		returnCode=updateImmAttribute(jobSchedulerRDN,nextScheduledTimeAttr,ATTR_STRINGT,value);
			if(returnCode == ACS_CC_FAILURE)
			{
				if(!jobScheduler::stopRequested)
				{
					ACE_OS::sleep(.5);
					continue;
				}
				else
					break;
			}
			else
				break;
		}
        }

        int scheduler_state = getImmIntAttribute(jobSchedulerRDN,schedulerStateAttr);

        //update the scheduler state correspondingly
        if(scheduledTimes.size() == 0 && scheduler_state != DISABLE )
        {
	        scheduler_state = DISABLE;
        	void* value[1]={reinterpret_cast<void*>(&scheduler_state)};

		for(int i=0; i<20 ;i++)
                {
                        returnCode=updateImmAttribute(jobSchedulerRDN,schedulerStateAttr,ATTR_INT32T,value);
                        if(returnCode == ACS_CC_FAILURE)
                        {
                                if(!jobScheduler::stopRequested)
                                {
                                        ACE_OS::sleep(.5);
                                        continue;
                                }
                                else
                                        break;
                        }
                        else
                                break;
                }

                //update next schedule date with "0000-00-00T00:00:00"
                char* value1 = (char*)NONEXTSCHEDULE;
                void* value2[1]={reinterpret_cast<void*>(value1)};
                updateImmAttribute(jobSchedulerRDN,nextScheduledTimeAttr,ATTR_STRINGT,value2);
        }
	else if(scheduledTimes.size() != 0 && scheduler_state == DISABLE)
        {
                scheduler_state = ENABLE;
                void* value[1]={reinterpret_cast<void*>(&scheduler_state)};
		for(int i=0; i<20 ;i++)
                {
                        returnCode=updateImmAttribute(jobSchedulerRDN,schedulerStateAttr,ATTR_INT32T,value);
                        if(returnCode == ACS_CC_FAILURE)
                        {
                                if(!jobScheduler::stopRequested)
                                {
                                        ACE_OS::sleep(.5);
                                        continue;
                                }
                                else
                                        break;
                        }
                        else
                                break;
                }
        }
        DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::updateNextScheduledTime(string eventRDN) ");
}

/*===================================================================
        ROUTINE:getSchedulesOfSingleEvents 
=================================================================== */
void acs_hcs_global_ObjectImpl::getSchedulesOfSingleEvents(set<string>&scheduledTimes,string eventRDN,vector<eventDetails> events, int eventType)
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getSchedulesOfSingleEvents ");
    	std::vector<std::string> status_pd_dnList;
	std::vector<std::string> pd_dnList_single;
	int returnCode;
	int numDef = 0;

	returnCode = getImmInstances(ACS_HC_SINGLEEVENT_CLASS_NAME, pd_dnList_single);
	if(returnCode == ACS_CC_SUCCESS)
	{
		int events_size = events.size() ;
		if(eventType == SINGLE)
		{
			for( int i=0; i<events_size; i++)
			{
				string temp = events[i].event_rdn;
				int size = pd_dnList_single.size();
				for(int j=0; j<size; j++)
				{
					size_t Pos = pd_dnList_single[j].find(temp);
					if( Pos != std::string::npos)
					{
						pd_dnList_single.erase(pd_dnList_single.begin()+j);
						scheduledTimes.insert(events[i].scheduleDate);
						break;
					}
				}
			}
		}
				
		numDef  = pd_dnList_single.size();
		for(int i = 0; i < numDef; i++)
		{
			if( jobScheduler::stopRequested == false)
			{
				int executionStatus = getImmIntAttribute(pd_dnList_single[i],executionStatusAttr);
				if( executionStatus == NOTEXECUTED)
					status_pd_dnList.push_back( pd_dnList_single[i]);
			}
			else
				return;
		}

		numDef =  status_pd_dnList.size();
		for(int i = 0; i < numDef; i++)
		{
			if( jobScheduler::stopRequested == false )
			{
				size_t dnPos = status_pd_dnList[i].find(eventRDN);
				if(dnPos != std::string::npos)
				{
					string scheduledTime = getImmAttribute(status_pd_dnList[i],LASTSCHEDULEDATE);
					if( scheduledTime != "")
						scheduledTimes.insert(scheduledTime);	
				}
			}
			else
				return;
		}

	}
	else
	{
		DEBUG("%s"," Error in getting single event instances");
		cout<<"Error in getting single event instances"<<endl;
	}

	DEBUG("%s"," Leaving acs_hcs_global_ObjectImpl::getSchedulesOfSingleEvents");
}

/*===================================================================
        ROUTINE:getSchedulesOfPeriodicEvents 
=================================================================== */
void acs_hcs_global_ObjectImpl::getSchedulesOfPeriodicEvents(set<string> &scheduledTimes, string eventRDN,vector<eventDetails> events, int eventType)
{
	DEBUG("%s","Entering  acs_hcs_global_ObjectImpl::getSchedulesOfPeriodicEvents ");
	std::vector<std::string> pd_dnList_periodic;
	int numDef = 0;
	int returnCode;

	returnCode = getImmInstances(ACS_HC_PERIODICEVENT_CLASS_NAME, pd_dnList_periodic);
	if(returnCode == ACS_CC_SUCCESS)
	{
		int events_size = events.size();
		if(eventType == PERIODIC)
                {
                        for( int i=0; i<events_size; i++)
                        {
                                string temp = events[i].event_rdn;
                                int size = pd_dnList_periodic.size();
                                for(int j=0; j<size; j++)
                                {
                                        size_t Pos = pd_dnList_periodic[j].find(temp);
                                        if( Pos != std::string::npos)
                                        {
                                                pd_dnList_periodic.erase(pd_dnList_periodic.begin()+j);
                                                scheduledTimes.insert(events[i].scheduleDate);
                                                break;
                                        }
                                }
                        }
                }

		numDef  = pd_dnList_periodic.size();
		cout<<"periodic "<<numDef<<endl;
		for(int i = 0; i < numDef; i++)
		{
			if( jobScheduler::stopRequested == false )
			{
				size_t dnPos = pd_dnList_periodic[i].find(eventRDN);
				if(dnPos != std::string::npos)
				{
					string scheduledTime = getImmAttribute(pd_dnList_periodic[i],LASTSCHEDULEDATE);
					if(scheduledTime != NONEXTSCHEDULE && scheduledTime !="" )//To check whether event reached stop time
						scheduledTimes.insert(scheduledTime);
				}
			}
			else
				return;
		}
	}
	else
	{
		cout<<"Error in getting periodic event instances"<<endl;
	}
	DEBUG("%s","Leaving  acs_hcs_global_ObjectImpl::getSchedulesOfPeriodicEvents ");
}

/*===================================================================
        ROUTINE:getSchedulesOfCalendarEvents
=================================================================== */
void acs_hcs_global_ObjectImpl::getSchedulesOfCalendarEvents(set<string> &scheduledTimes, string eventRDN,vector<eventDetails> events, int eventType)
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getSchedulesOfCalendarEvents ");
	std::vector<std::string> pd_dnList_cal;
	int numDef = 0;
	int returnCode;

	returnCode = getImmInstances(ACS_HC_CALENDERPERIODICEVENT_CLASS_NAME, pd_dnList_cal);
	if(returnCode == ACS_CC_SUCCESS)
	{
		int events_size = events.size();
                if(eventType == CALENDAR)
                {
                        for( int i=0; i<events_size; i++)
                        {
                                string temp = events[i].event_rdn;
                                int size = pd_dnList_cal.size();
                                for(int j=0; j<size; j++)
                                {
                                        size_t Pos = pd_dnList_cal[j].find(temp);
                                        if( Pos != std::string::npos)
                                        {
                                                pd_dnList_cal.erase(pd_dnList_cal.begin()+j);
						
						if(events[i].scheduleDate != NONEXTSCHEDULE)
						{
	                                                scheduledTimes.insert(events[i].scheduleDate);
						}

                                                break;
                                        }
                                }
                        }
                }


		numDef  = pd_dnList_cal.size();
		cout<<"calendar "<<numDef<<endl;
		for( int i = 0; i < numDef; i++)
		{
			if( jobScheduler::stopRequested == false )
			{
				size_t dnPos = pd_dnList_cal[i].find(eventRDN);
				if(dnPos != std::string::npos)
				{
					string scheduledTime = getImmAttribute(pd_dnList_cal[i],LASTSCHEDULEDATE);
					if(scheduledTime != NONEXTSCHEDULE && scheduledTime !="" )
						scheduledTimes.insert(scheduledTime);
				}
			}
			else
				return;
		}
	}
	else
	{
		DEBUG("%s","Error in getting calendar event instances");
		cout<<"Error in getting calendar event instances"<<endl;
	}
	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getSchedulesOfCalendarEvents ");
}

/*===================================================================
        ROUTINE: parseRDN
=================================================================== */
string acs_hcs_global_ObjectImpl::parseRDN(string eventRDN)
{	
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::parseRDN(string eventRDN) ");
        size_t pos = eventRDN.find_last_of(',');
        eventRDN = eventRDN.substr(0,pos);
        pos = eventRDN.find_last_of(',');
        eventRDN = eventRDN.substr(pos+1,eventRDN.size()-pos);
	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::parseRDN(string eventRDN) ");
        return eventRDN;
}

/*===================================================================
        ROUTINE: periodic_updationAfterJobExecution
=================================================================== */
void acs_hcs_global_ObjectImpl::periodic_updationAfterJobExecution(string rdn)
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::periodic_updationAfterJobExecution(string rdn) ");
	char *objName = const_cast<char*>(rdn.c_str());
	OmHandler immHandler;
	ACS_CC_ReturnType returnCode;
	vector<ACS_APGCC_ImmAttribute *> attributes ;
	acs_hcs_periodicscheduling p_obj;

	char startTime[32]		={0};
	char lastScheduleDate[32]	={0};
	char stopTime[32]       	={0};
	int days_value         	 	=  0;
	int hours_value         	=  0;
	int weeks_value         	=  0;
	int months_value        	=  0;
	int minutes_value       	=  0;
	char periodicity[32]		={0};
	int day				=  0;
	int month			=  0;
	int year			=  0;

	returnCode=immHandler.Init();
	if(returnCode!=ACS_CC_SUCCESS)
	{
	 	cout<<"ERROR: init FAILURE!!!\n";
	}

	ACS_APGCC_ImmAttribute LastScheduleDate;
	ACS_APGCC_ImmAttribute StartTime; 
	ACS_APGCC_ImmAttribute StopTime;
	ACS_APGCC_ImmAttribute Months;
	ACS_APGCC_ImmAttribute Weeks;
	ACS_APGCC_ImmAttribute Hours;
	ACS_APGCC_ImmAttribute Days;
        ACS_APGCC_ImmAttribute Minutes;

	StartTime.attrName		= startTimeAttr;
        LastScheduleDate.attrName   	= LASTSCHEDULEDATE;
        StopTime.attrName       	= stopTimeAttr;
        Months.attrName         	= monthsAttr;
        Days.attrName           	= daysAttr;
        Hours.attrName          	= hoursAttr;
        Weeks.attrName          	= weeksAttr;
        Minutes.attrName        	= minutesAttr;

	attributes.push_back(&LastScheduleDate);
        attributes.push_back(&StopTime);
        attributes.push_back(&Months);
        attributes.push_back(&Minutes);
        attributes.push_back(&Hours);
        attributes.push_back(&Weeks);
        attributes.push_back(&Days);
	attributes.push_back(&StartTime);
	
	returnCode = immHandler.getAttribute(objName, attributes);
        if(returnCode == ACS_CC_FAILURE)
        {
		DEBUG("%s","IMM modify attribute failure in periodic_updateJobAfterExecution");
                cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
                cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
        }
        else
        {
		strncpy(lastScheduleDate,(reinterpret_cast<char*>(attributes[0]->attrValues[0])),sizeof(lastScheduleDate)-1);
		strncpy(stopTime,(reinterpret_cast<char*>(attributes[1]->attrValues[0])),sizeof(stopTime) - 1);
		months_value    =  *(reinterpret_cast<int*>(attributes[2]->attrValues[0]));
                weeks_value     =  *(reinterpret_cast<int*>(attributes[5]->attrValues[0]));
                minutes_value   =  *(reinterpret_cast<int*>(attributes[3]->attrValues[0]));
                days_value      =  *(reinterpret_cast<int*>(attributes[6]->attrValues[0]));
                hours_value     =  *(reinterpret_cast<int*>(attributes[4]->attrValues[0]));
		strncpy(startTime,(reinterpret_cast<char*>(attributes[7]->attrValues[0])),sizeof(startTime) - 1);
        }
        immHandler.Finalize();
	
	string nextExecutionDate = "";
	if( (months_value+weeks_value+days_value+hours_value+minutes_value) == 0)
			nextExecutionDate = NONEXTSCHEDULE ;

	else
	{
		sprintf(periodicity,"%d-%d-%dT%d:%d:%d",months_value ,weeks_value,days_value,hours_value ,minutes_value,0);
		sscanf(startTime,"%d-%d-%dT",&year,&month,&day);
		nextExecutionDate=p_obj.nextSchedule(lastScheduleDate,periodicity,day);
		string time = localZoneTime(stopTime); 
		if( compareDates(time,nextExecutionDate))
			nextExecutionDate = NONEXTSCHEDULE ;
	}

	char* value = const_cast<char*>(nextExecutionDate.c_str());
	void* value1[1] = { reinterpret_cast<char *>(value)};	
	updateImmAttribute(rdn,LASTSCHEDULEDATE,ATTR_STRINGT,value1);
	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::periodic_updationAfterJobExecution(string rdn) ");
}

/*===================================================================
        ROUTINE:cal_updationAfterJobExecution 
=================================================================== */
void acs_hcs_global_ObjectImpl::cal_updationAfterJobExecution(string rdn,bool time_flag)
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::cal_updationAfterJobExecution(string rdn,bool time_flag) ");
	cout<<"Calender event rdn in cal_updationAfterJobExecution: "<<rdn<<endl;
        OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
        vector<ACS_APGCC_ImmAttribute *> attributes ;
	CALENDER_Parameters cal;
	memset(&cal, 0 , sizeof(CALENDER_Parameters));

	char lastScheduleTime[32]		= {0};
	char stopTime[32]               	= {0};
	char startTime[32]			= {0};
        char time[10]                   	= {0};
        int dayOfMonth_value            	=   0;
        int dayOfWeek_value             	=   0;
        int dayOfWeekOccurence_value    	=   0;
        int month_value 			=   0;

        returnCode=immHandler.Init();
        if(returnCode!=ACS_CC_SUCCESS)
        {
                cout<<"ERROR: init FAILURE!!!\n";
        }
	
	ACS_APGCC_ImmAttribute StartTime;
        ACS_APGCC_ImmAttribute LastScheduleDate;
        ACS_APGCC_ImmAttribute StopTime;
        ACS_APGCC_ImmAttribute DayOfMonth;
        ACS_APGCC_ImmAttribute DayOfWeek;
        ACS_APGCC_ImmAttribute DayOfWeekOccurence;
        ACS_APGCC_ImmAttribute Month;
        ACS_APGCC_ImmAttribute Time;

        LastScheduleDate.attrName       = LASTSCHEDULEDATE;
       	StopTime.attrName               = stopTimeAttr;
        DayOfMonth.attrName             = dayOfMonthAttr;
        DayOfWeek.attrName              = dayOfWeekAttr;
        DayOfWeekOccurence.attrName     = dayOfWeekOccurenceAttr;
        Month.attrName                  = monthAttr;
        Time.attrName                   = timeAttr;
	StartTime.attrName		= startTimeAttr;

	if(time_flag)
		attributes.push_back(&StartTime);
	else
		attributes.push_back(&LastScheduleDate);

        attributes.push_back(&StopTime);
        attributes.push_back(&DayOfMonth);
        attributes.push_back(&DayOfWeek);
        attributes.push_back(&DayOfWeekOccurence);
        attributes.push_back(&Month);
        attributes.push_back(&Time);

        for( int i=0; i<10 ; i++)
	{
		returnCode = immHandler.getAttribute(rdn.c_str(), attributes);
		if(returnCode == ACS_CC_FAILURE)
		{
			DEBUG("%s","IMM modify attributr failure in cal_updatejobafterexecution");
			cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
			cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
			continue;
		}
		else
		{
			cout<<"In else loop"<<endl<<endl;
			if(time_flag) //During the time of event creation
			{
				strncpy(startTime,(reinterpret_cast<char*>(attributes[0]->attrValues[0])),sizeof(startTime) - 1);
				DEBUG("Sending start date from cal %s",startTime);
				string time = localZoneTime(startTime);
				strncpy(cal.StartTime,time.c_str(),sizeof(startTime)-1);
			}
			else //caliculating next schedule time after execution
			{
			       strncpy(lastScheduleTime,(reinterpret_cast<char*>(attributes[0]->attrValues[0])),sizeof(lastScheduleTime)-1);
				strncpy(cal.StartTime,lastScheduleTime,sizeof(lastScheduleTime)-1);
			}
			
			strncpy(stopTime,(reinterpret_cast<char*>(attributes[1]->attrValues[0])),sizeof(stopTime) - 1);
			dayOfMonth_value        =  *(reinterpret_cast<int*>(attributes[2]->attrValues[0]));
			dayOfWeek_value         =  *(reinterpret_cast<int*>(attributes[3]->attrValues[0]));
			dayOfWeekOccurence_value=  *(reinterpret_cast<int*>(attributes[4]->attrValues[0]));
			month_value             =  *(reinterpret_cast<int*>(attributes[5]->attrValues[0]));
			strncpy(time,(reinterpret_cast<char*>(attributes[6]->attrValues[0])),sizeof(time) - 1);

			break;
		}
	}
        immHandler.Finalize();

	cal.DayOfMonth	= dayOfMonth_value;
        cal.DayOfWeek     = dayOfWeek_value;
        cal.Month               = month_value;
        cal.DayOfWeekOccurence  = dayOfWeekOccurence_value;
	DEBUG("Sending stop date from cal %s",stopTime);
	string temp_stopTime = localZoneTime(stopTime);
	strncpy(cal.StopTime,temp_stopTime.c_str(),sizeof(stopTime)-1);
        strncpy(cal.Time,time,sizeof(time) - 1);

	CALENDER_Parameters *request_cal = new CALENDER_Parameters(cal);
	string nextScheduleDate = getCalNextScheduledTime(request_cal);
	delete request_cal;

	if(compareDates(stopTime,nextScheduleDate))
		nextScheduleDate = NONEXTSCHEDULE; 
	char* value = const_cast<char*>(nextScheduleDate.c_str());	
	void* value1[1] = { reinterpret_cast<char *>(value)};
	ACE_Time_Value tv(0, 50000);	
	updateImmAttribute(rdn,LASTSCHEDULEDATE,ATTR_STRINGT,value1);	

	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::cal_updationAfterJobExecution(string rdn,bool time_flag) ");
}

/*===================================================================
        ROUTINE:getCalNextScheduledTime
=================================================================== */
string acs_hcs_global_ObjectImpl::getCalNextScheduledTime(CALENDER_Parameters *request)
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getCalNextScheduledTime(CALENDER_Parameters *request) ");
        jobSchedule jsch(request);

	bool flag = jsch.isCombinationPossible();
	cout<<endl<<endl<<"        "<<flag<<endl;
	if(!flag)
		return NONEXTSCHEDULE;
        do
        {
		if(jobScheduler::stopRequested == true ) //call received for graceful termination
			break;

                jsch.calcDelay();
                if(jsch.checkTrigger())
                {
			string date = jsch.getDateTime();
			return date;
                }
                jsch.updateNextTriggerTime();
        }while(!jsch.timeToQuit());

	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getCalNextScheduledTime(CALENDER_Parameters *request) ");
	return NONEXTSCHEDULE;
}

/*===================================================================
        ROUTINE:isLeapYear
=================================================================== */
bool acs_hcs_global_ObjectImpl::isLeapYear(int year)
{
        // No debug here because it overflow the log file
	if(((year % 4 == 0) && !(year % 100 == 0))|| (year % 400 == 0))
	{       
		return true;
	}
        else
        {
	      	return false;
	}
}

/*===================================================================
        ROUTINE:parseFirstString
=================================================================== */
string acs_hcs_global_ObjectImpl::parseFirstString(string eventRDN)
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::isLeapYear(int year) ");
	size_t pos = eventRDN.find_first_of(',');
        eventRDN = eventRDN.substr(0,pos);
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::isLeapYear(int year) ");
        return eventRDN;
}

/*==================================================================
        ROUTINE:getImmInstances
=================================================================== */
int acs_hcs_global_ObjectImpl::getImmInstances(const char* className, vector<string> &dnList)
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::isLeapYear(int year) ");
	OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
	int errorCode;

	returnCode=immHandler.Init();
        if(returnCode!=ACS_CC_SUCCESS)
        {
		return ACS_CC_FAILURE;
        }
	
	if(jobScheduler::stopRequested == true)
			return -1;
	
	returnCode = immHandler.getClassInstances(className, dnList);
	if(returnCode != ACS_CC_SUCCESS)
        {
		errorCode = immHandler.getInternalLastError();	
		immHandler.Finalize();
		//DEBUG("%s","Error in getting class instances");
		return errorCode;
	}
	immHandler.Finalize();
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getImmInstances");
	return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE:getNearestTime
=================================================================== */
string acs_hcs_global_ObjectImpl::getNearestTime(string d1, string d2)
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::isLeapYear(int year) ");	
        int year1,mon1,day1,hh1,mm1,ss1;
        int year2,mon2,day2,hh2,mm2,ss2;
        int res;

        sscanf(d1.c_str(),"%4d-%2d-%2dT%2d:%2d:%2d",&year1,&mon1,&day1,&hh1,&mm1,&ss1);
        sscanf(d2.c_str(),"%4d-%2d-%2dT%2d:%2d:%2d",&year2,&mon2,&day2,&hh2,&mm2,&ss2);

	res = (year1==year2) ? BOTH : ( year1<year2 ? FRST : SCND);

	if( res == BOTH)
           res = (mon1==mon2) ? BOTH : (mon2< mon1 ? SCND : FRST);

        if( res == BOTH)
           res = (day1==day2) ? BOTH : (day2<day1 ? SCND : FRST);

        if( res == BOTH)
           res = (hh1==hh2) ? BOTH : ( hh2<hh1 ? SCND : FRST);

        if( res == BOTH)
           res = (mm1==mm2) ? BOTH : (mm2<mm1 ? SCND : FRST);

        if( res == BOTH)
           res = ss1==ss2 ? BOTH : (ss2< ss1 ? SCND : FRST);


        if(res == FRST)
	{
		//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getNearestTime");
              	return (d1);
	}
        else 
	{
		//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getNearestTime");
	        return (d2);
	}
}

/*===================================================================
        ROUTINE:compareDates
=================================================================== */
bool acs_hcs_global_ObjectImpl::compareDates(string d1, string d2)
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::compareDates");
	string NearestTime = getNearestTime(d1,d2);
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::compareDates");

	if( NearestTime.substr(0,19) == d1.substr(0,19) )
		return true;

	else
		return false;
}

/*===================================================================
        ROUTINE:isAdminStateLOCKED
=================================================================== */
bool acs_hcs_global_ObjectImpl::isAdminStateLOCKED(string rdn)
{
	//DEBUG("%s","Entering acs_hcs_global_ObjectImpl::isAdminStateLOCKED");
	size_t pos = rdn.find_first_of(',');
        rdn = rdn.substr(pos+1,rdn.size()-1);
	int state = getImmIntAttribute(rdn,adminStateAttr);
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::isAdminStateLOCKED");
	if(state)
		return false;
	
	return true;
}

/*===================================================================
        ROUTINE:getProgressReportRunningState
=================================================================== */
int acs_hcs_global_ObjectImpl::getProgressReportRunningState()
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getReportRunningState"); 
        unsigned int j;
        string subDN;
        for(j = 0; j < acs_hcs_progressReport_ObjectImpl::progressReport.size(); j++)
        {
                size_t pos = ((acs_hcs_progressReport_ObjectImpl::progressReport[j].DN).find_first_of('='));
                subDN = ((acs_hcs_progressReport_ObjectImpl::progressReport[j].DN).substr(0,pos));
                if((jobSchedulerAttr == subDN) || (jobIdAttr == subDN))
                {
                        if(acs_hcs_progressReport_ObjectImpl::progressReport[j].state == acs_hcs_progressReport_ObjectImpl::RUNNING)
                        {
                                cout<<"state is RUNNING"<<endl;
				DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getProgressReportRunningState");
                                return STATE_RUNNING;
                        }
                }
        }
	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getProgressReportRunningState");
        return STATE_NOT_RUNNING;
}

/*===================================================================
        ROUTINE:getProgressReportState
=================================================================== */
int acs_hcs_global_ObjectImpl::getProgressReportState(string eventRDN)
{
	DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getProgressReportRunningState");
        unsigned int j,index,state,temp=0;
	int size = acs_hcs_progressReport_ObjectImpl::progressReport.size();
        for(j = 0; j < size; j++)
        {
                if(eventRDN == acs_hcs_progressReport_ObjectImpl::progressReport[j].DN)
                {
			temp=1;
                        break;
                }
        }
        index = j;

	if(temp==1)
        	state = acs_hcs_progressReport_ObjectImpl::progressReport[index].state;
	else
		state = STATE_CANCELLED ; //In case if  dn is not available

        return state;
}

/*===================================================================
        ROUTINE:removeTrailingSpaces
=================================================================== */
string acs_hcs_global_ObjectImpl::removeTrailingSpaces(string job_categories)
{
        DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::removeTrailingSpaces(string job_categories) ");
        size_t spacePos=0;
        for( ; spacePos != std::string::npos ; )
        {
                spacePos = job_categories.find_first_of(" ");
                if(spacePos != std::string::npos)
                {
                        job_categories.erase(spacePos,1);
                }
                else
                        break;
        }
        DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::removeTrailingSpaces(string job_categories) ");
        return job_categories;
}

/*===================================================================
        ROUTINE:tokenizeCategories
=================================================================== */
void acs_hcs_global_ObjectImpl::tokenizeCategories(set<std::string> &categoryList, std::string category)
{
	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::tokenizeCategories(string job_categories) ");
	char *temp = strtok(const_cast<char *>(category.c_str()),COMMA_STR);
	while(temp != NULL)
	{
		string id = categoriesAttr_with_EQU;
		id = id + temp;
		categoryList.insert(id);
		temp = strtok(NULL,COMMA_STR);
	}
}

/*===================================================================
        ROUTINE:getTqDN
=================================================================== */
string acs_hcs_global_ObjectImpl::getTqDN(string tq)
{
        DEBUG("%s","Entering :cs_hc_global_ObjectImpl:getTqDN ");
        string dn;
        string actualTQ = tq;
        if(strcmp(tq.c_str(),EMPTY_STR) != 0)
        {
                std::vector<std::string> pd_dnList;
                tq = (string)TQID+EQUALTO_STR+tq ;
                int returnCode;
                returnCode = getImmInstances(ACS_HC_TQ_CLASS_NAME, pd_dnList);
                if(returnCode == ACS_CC_SUCCESS)
                {
                        int numDef  = pd_dnList.size();
                        string temp;
                        int i;
                        for( i = 0; i < numDef; i++)
                        {
                                size_t commaPos = pd_dnList[i].find_first_of(COMMA_STR);
                                string parseTQValue = pd_dnList[i].substr(0,commaPos);
                                if(strcmp(tq.c_str(),parseTQValue.c_str())== 0)
                                {
                                        cout<<"Found rdn for tq"<<endl;
                                        break;
                                }
                        }
                        //dn = pd_dnList[i];
                        //Changes made to display the destinationDN in moreuseful way for operator
                        dn = (string)MANAGED_ELEMENT + (string)EQUALTO_STR + getManagedElementId() + (string)destinationUpdate + actualTQ ;
                        cout<<"Dn to update is "<<dn<<endl;
                }
                else
                {
                        cout<<"Unable to get class instances"<<endl;
                }
	}
        else
        {
                dn = EMPTY_STR;
        }
        DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getTqDN(string tq ");
        return dn;
}	

/*===================================================================
        ROUTINE:getManagedElementId
=================================================================== */
string acs_hcs_global_ObjectImpl::getManagedElementId()
{
        OmHandler myOmHandler;
        ACS_CC_ImmParameter paramToFind;
        std::vector<std::string> ListManagedElement;
        string managedElement("");
        if(myOmHandler.Init() == ACS_CC_SUCCESS)
        {
                if (myOmHandler.getClassInstances(MANAGED_ELEMENT,ListManagedElement) == ACS_CC_SUCCESS)
                {
                        string dn = ListManagedElement[0];
                        managedElement = getImmAttribute(dn.c_str(),networkElementIdAttr);
                }
                myOmHandler.Finalize();
        }
        cout<<"managed element id for node is "<<managedElement<<endl;
        return managedElement;
}

/*===================================================================
        ROUTINE:getLocalTimeZone
=================================================================== */
string acs_hcs_global_ObjectImpl::getLocalTimeZone()
{
	//DEBUG("%s","Entering :cs_hc_global_ObjectImpl::getLocalTimeZone");
	vector<string> vec;
	int numDef = 0;
        int returnCode;
	string timeZone = "";
	
	returnCode = getImmInstances(TIME_ZONE_LINK,vec);
        if(returnCode == ACS_CC_SUCCESS)
        {
                numDef  = vec.size();
                for(int i = 0; i < numDef; i++)
                {
			int id = getImmIntAttribute(vec[i],timeZoneIdAttr);

                        if( id == 0)
				timeZone = getImmAttribute(vec[i],timeZoneStringAttr);
                }
        }
        else
        {
                //DEBUG("%s"," Error in getting single event instances");
                cout<<"Error in getting single event instances"<<endl;
        }
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getLocalTimeZone");
	return timeZone;
}

/*===================================================================
        ROUTINE:localZoneTime
=================================================================== */
string acs_hcs_global_ObjectImpl::localZoneTime(string date)
{
	cout << "Entered localZoneTime" << endl;
	if(date.size() <= 19)
	{
		cout << "Date length is less than 19" << endl;
		//DEBUG("Received date without timezone %s", date);
		DEBUG("%s", "Date length is less than 20");
		return date;
	}
	else
	{
		cout << "Date length is greater than 19" << endl;
		//DEBUG("Date received with time zone %s", date);
		acs_hcs_periodicscheduling obj;
		string temp             = date.substr(19,date.size()-19);
		char periodicity[32]    = {0};
		size_t z_pos            = temp.find('Z');
		size_t plus_pos         = temp.find('+');
		int oneday              = 1440; //i.e 24 hours
		int sign1,sign2,minutesDifference1,minutesDifference2;

		string localTimeZone = getLocalTimeZone(); //It would be in "(GMT+01:00) Europe/Rome" format
		if( localTimeZone != "" )
		{
	        	size_t gmt_pos = localTimeZone.find(GMT);
        		string timezoneDifference = localTimeZone.substr(gmt_pos+3,6); //In +01:00 format
			string diff = timezoneDifference.substr(0,1);
			sign1 = (diff == "+") ? 1 : -1 ;
			sign2 = (z_pos != std::string::npos) ? 0 : (plus_pos != std::string::npos) ? -1 : 1 ;
			minutesDifference1 = atoi((timezoneDifference.substr(1,2)).c_str())*60 + atoi((timezoneDifference.substr(4,2)).c_str());
			minutesDifference2 = 0;
		}
		else
		{
			return date;
		}

		if( sign2 != 0 )
		{
			int hour =  atoi( (date.substr(date.size()-5,2)).c_str());
	                int minutes = atoi( (date.substr(date.size()-2,2)).c_str());
			minutesDifference2 = (hour*60)+minutes;
		}

		if( (sign1 == 1) && ( sign2 == 0 || sign2 == 1))
		{
			sprintf(periodicity,"%d-%d-%dT%d:%d:%d",0,0,0,0,minutesDifference1+minutesDifference2,0);
			int day = atoi((date.substr(8,2)).c_str());
                        date = obj.nextSchedule(date,periodicity,day);
		}
		else
		{
			date = decreaseOneDay(date);
			int minutesDifference = oneday + (sign1*minutesDifference1) + (sign2*minutesDifference2);
			sprintf(periodicity,"%d-%d-%dT%d:%d:%d",0,0,0,0,minutesDifference,0);
                        int day = atoi((date.substr(8,2)).c_str());
			date = obj.nextSchedule(date,periodicity,day);
		}
	}
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::localZoneTime");
	return date;
}

/*===================================================================
        ROUTINE:decreaseOneDay
=================================================================== */
string acs_hcs_global_ObjectImpl::decreaseOneDay(string date)
{
	DEBUG("%s","Entering :cs_hc_global_ObjectImpl::decreaseOneDay");
	char temp[32]={0};
	int yy,mm,day;
	string time = date.substr(11,date.size()-1);
	yy = atoi((date.substr(0,4)).c_str());
        mm = atoi((date.substr(5,2)).c_str());
        day = atoi((date.substr(8,2)).c_str());

	if(day != 1)
		day = day-1;
	else
	{
		if(mm != 1)
		{
			mm = mm-1;
			if( isLeapYear(yy) == true && mm == 2)
				day = 29;			
			else
				day = lastDay[mm-1];
		}
		else
		{
			yy = yy-1;
			mm = 12;
			day = 31;
		}
	}
	sprintf(temp,"%04d-%02d-%02dT%s",yy,mm,day,time.c_str());
	string modifiedDate(temp);

	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::decreaseOneDay");
	return modifiedDate;
}

/*===================================================================
        ROUTINE:getProgressReportIndex
=================================================================== */
int acs_hcs_global_ObjectImpl::getProgressReportIndex(string parentObject)
{
	//DEBUG("%s","Entering :acs_hcs_global_ObjectImpl::getProgressReportIndex");
	unsigned int index=0,i;
 	for(i = 0; i < acs_hcs_progressReport_ObjectImpl::progressReport.size(); i++)
	{
		if(parentObject == acs_hcs_progressReport_ObjectImpl::progressReport[i].DN)
		{
			index = i;
			break;
		}
	}
	if(i == acs_hcs_progressReport_ObjectImpl::progressReport.size())
                        index=-1;   //There is no progress report with the provided parentObject
	
	//DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::getProgressReportIndex");
	return index;
}

/*===================================================================
        ROUTINE:createJobScheduler
=================================================================== */
void acs_hcs_global_ObjectImpl::createJobScheduler(string rdn)
{
	DEBUG("%s","Entering :acs_hcs_global_ObjectImpl::createJobScheduler");
	OmHandler immHandler;
	ACS_CC_ReturnType returnCode;
	returnCode=immHandler.Init();
	if(returnCode!=ACS_CC_SUCCESS)
	{
		cout<<"ERROR: init FAILURE while creating job scheduler\n";
	}
	vector<ACS_CC_ValuesDefinitionType> jobSchedulerAttrList;
	ACS_CC_ValuesDefinitionType attributeRDN;
	char *className = const_cast<char*>(ACS_HC_JOBSCHEDULER_CLASS_NAME);

	attributeRDN.attrName = (char*)jobSchedulerAttr;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = (char*)JOBSCHEDULERID;
	void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value2;

	jobSchedulerAttrList.push_back(attributeRDN);

	char parent[128]= {0};
	strcpy(parent,rdn.c_str());
	do
	{
		returnCode = immHandler.createObject(className, parent, jobSchedulerAttrList);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout<<"IMM CreateObject failure"<<std::endl;
			cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
			ACE_OS::sleep(1);
		}
	}while(returnCode == ACS_CC_FAILURE && immHandler.getInternalLastError() != -12);

	immHandler.Finalize();
	DEBUG("%s","Leaving acs_hcs_global_ObjectImpl::createJobScheduler");
}

/*===================================================================
	        ROUTINE:waitForSomeTime
=================================================================== */
bool acs_hcs_global_ObjectImpl::waitForSomeTime(int seconds)
{	
	for(int i=0; i<seconds; i++)
	{
		if(!jobScheduler::stopRequested) //Call for graceful termination is not received
			ACE_OS::sleep(1);
		else
			return false;
	}
	return true;
}

/*===================================================================
                ROUTINE:isParameterValid
=================================================================== */
bool acs_hcs_global_ObjectImpl::isParameterValid(string parameter, string dn)
{
	acs_hcs_global_ObjectImpl obj;
	vector<string> ruleFileInstances;
	vector<string> dnList;
	int returnCode;

	if( parameter.size() > 12)
	{
		string substring = parameter.substr(0,12);

		if( strcmp(substring.c_str(),RuleSetFile) == 0 )
		{
			size_t len = parameter.size();
			parameter = parameter.substr(12,len-12);
		}
		else
			return false;
	}
	else
	{
		return false;
	}

	string temp_parm = parameter + (string)COMMA_STR ;
	cout<<"parameter is : "<<temp_parm<<endl;

	returnCode = obj.getImmInstances(ACS_HC_RULESETFILE_CLASS_NAME,ruleFileInstances);
	if( returnCode == ACS_CC_SUCCESS )
	{
		int noOfInstances = ruleFileInstances.size();
		int i;
		for( i=0; i<noOfInstances; i++)
		{
			cout<<"Instance of rulesetfile : "<<ruleFileInstances[i]<<endl;
			string temp = ruleFileInstances[i];
			cout<<"rdn of rule set file is : "<<temp<<endl;
			size_t found = ruleFileInstances[i].find(dn);
			if( found != std::string::npos)
				dnList.push_back(temp);
		}
		int size = dnList.size();
		if(size == 0)
			return false;
		else
		{
			int m;
			for(m=0; m<size; m++)
			{
				string temp1 = dnList[m];
				size_t found1 = dnList[m].find(temp_parm.c_str());
				if( found1 != std::string::npos)
					break;
			}
			if( m == size )
				return false;
		}
		return true;

	}
	else
	{
		return false;
	}
}

/*===================================================================
        ROUTINE:getCalendarEventAttributes
=================================================================== */
ACS_CC_ReturnType acs_hcs_global_ObjectImpl::getCalendarEventAttributes(string rdn,CALENDER_Parameters &cal)
{
        DEBUG("%s","Entering acs_hcs_global_ObjectImpl::getCalendarEventAttributes(string rdn,CALENDER_Parameters &cal)");
        OmHandler immHandler;
        ACS_CC_ReturnType returnCode;
        vector<ACS_APGCC_ImmAttribute *> attributes ;
        memset(&cal, 0 , sizeof(CALENDER_Parameters));

        char stopTime[32]                       = {0};
        char startTime[32]                      = {0};
        char time[10]                           = {0};
        int dayOfMonth_value                    =   0;
        int dayOfWeek_value                     =   0;
        int dayOfWeekOccurence_value            =   0;
        int month_value                         =   0;

        returnCode=immHandler.Init();
        if(returnCode!=ACS_CC_SUCCESS)
        {
                cout<<"ERROR: init FAILURE!!!\n";
        }

        ACS_APGCC_ImmAttribute StartTime;
        ACS_APGCC_ImmAttribute StopTime;
        ACS_APGCC_ImmAttribute DayOfMonth;
        ACS_APGCC_ImmAttribute DayOfWeek;
        ACS_APGCC_ImmAttribute DayOfWeekOccurence;
        ACS_APGCC_ImmAttribute Month;
        ACS_APGCC_ImmAttribute Time;

        StopTime.attrName               = stopTimeAttr;
        DayOfMonth.attrName             = dayOfMonthAttr;
        DayOfWeek.attrName              = dayOfWeekAttr;
        DayOfWeekOccurence.attrName     = dayOfWeekOccurenceAttr;
        Month.attrName                  = monthAttr;
	Time.attrName                   = timeAttr;
        StartTime.attrName              = startTimeAttr;

        attributes.push_back(&StartTime);
        attributes.push_back(&StopTime);
        attributes.push_back(&DayOfMonth);
        attributes.push_back(&DayOfWeek);
        attributes.push_back(&DayOfWeekOccurence);
        attributes.push_back(&Month);
        attributes.push_back(&Time);

	returnCode = immHandler.getAttribute(rdn.c_str(), attributes);
	if(returnCode == ACS_CC_FAILURE)
	{
		DEBUG("%s","Failure in getCalendarEventAttributes(string rdn,CALENDER_Parameters cal)");
		return ACS_CC_FAILURE;
	}
	else
	{
		strncpy(startTime,(reinterpret_cast<char*>(attributes[0]->attrValues[0])),sizeof(startTime) - 1);
		strncpy(stopTime,(reinterpret_cast<char*>(attributes[1]->attrValues[0])),sizeof(stopTime) - 1);
		dayOfMonth_value        =  *(reinterpret_cast<int*>(attributes[2]->attrValues[0]));
		dayOfWeek_value         =  *(reinterpret_cast<int*>(attributes[3]->attrValues[0]));
		dayOfWeekOccurence_value=  *(reinterpret_cast<int*>(attributes[4]->attrValues[0]));
		month_value             =  *(reinterpret_cast<int*>(attributes[5]->attrValues[0]));
		strncpy(time,(reinterpret_cast<char*>(attributes[6]->attrValues[0])),sizeof(time) - 1);
	}
        immHandler.Finalize();

        cal.DayOfMonth  = dayOfMonth_value;
        cal.DayOfWeek     = dayOfWeek_value;
        cal.Month               = month_value;
        cal.DayOfWeekOccurence  = dayOfWeekOccurence_value;
	string temp_startTime = localZoneTime(startTime);
	strncpy(cal.StartTime,temp_startTime.c_str(),sizeof(startTime)-1);
        string temp_stopTime = localZoneTime(stopTime);
        strncpy(cal.StopTime,temp_stopTime.c_str(),sizeof(stopTime)-1);
        strncpy(cal.Time,time,sizeof(time) - 1);

        DEBUG("%s","Leaving acs_hcs_global_ObjectImpl:: getCalendarEventAttributes(string rdn,CALENDER_Parameters cal);");
	return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE:getAllCategories
=================================================================== */
string acs_hcs_global_ObjectImpl::getAllCategories()
{
	string all_categories="";
	std::vector<std::string> dnList_categories;

	int count,returnCode;
	returnCode  = getImmInstances(CATEGORY_NAME,dnList_categories);
	if(returnCode == ACS_CC_SUCCESS)
	{
		count = dnList_categories.size();
		for( int i=0; i<count; i++)
		{
			string temp = getImmAttribute(dnList_categories[i],categoriesIdAttr);  //temp would be in the form categoryId=POSTUPDATE
			all_categories = all_categories + temp.substr(11); //substr will parse the string and returns POSTUPGRADE
			if( i != count-1)
				all_categories = all_categories + ",";
		}
	}
	
	return all_categories;
}
