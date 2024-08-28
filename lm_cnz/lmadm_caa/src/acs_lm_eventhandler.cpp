//******************************************************************************CS_LM_ALARM_MAINTENANCE_MODE_ACT
//
//  NAME
//     acs_lm_eventhandler.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

//#include "acs_lm_tracemacro.h"
#include "acs_lm_eventhandler.h"
#include <ace/ACE.h>
acs_aeh_evreport * ACS_LM_EventHandler::theAEHObjPtr = 0;


#define ALARMDATAFILE "lmalarmdata"
#define LKEXP_A2ALARM_PROBLEM_TEXT "ONE OR MORE LICENSE KEYS ARE ABOUT TO EXPIRE IN 7 DAYS"
#define LKEXP_A3ALARM_PROBLEM_TEXT "ONE OR MORE LICENSE KEYS ARE ABOUT TO EXPIRE IN 60 DAYS"
#define MAINTENANCE_O1ALARM_PROBLEM_TEXT "LICENSE MANAGEMENT, AP, MAINTENANCE MODE ACTIVATED"

ACS_LM_EventHandler::LmEvent ACS_LM_EventHandler::lmEvents[] = 
{
	{ACS_LM_ALARM_LK_EXPIRE_A2, "A2", "LICENSE KEY ABOUT TO EXPIRE", LKEXP_A2ALARM_PROBLEM_TEXT, ""},
	{ACS_LM_ALARM_LK_FILE_MISS, "A2", "LICENSE KEY FILE MISSING", "LICENSE KEY FILE IS MISSING OR INVALID", ""},
	{ACS_LM_ALARM_EM_STATE_UNAVAIL, "A3", "EMERGENCY STATE NOT AVAILABLE", "ACTIVATION OF EMERGENCY STATE NOT POSSIBLE", ""},
    {ACS_LM_ALARM_LK_EXPIRE_A3, "A3", "LICENSE KEY ABOUT TO EXPIRE", LKEXP_A3ALARM_PROBLEM_TEXT, ""},
	{ACS_LM_ALARM_EM_STATE_LOCK_RESET, "EVENT", "EMERGENCY STATE ACTIVATION LOCK RESET", "", ""},
	{ACS_LM_ALARM_NEW_LK_INSTALL, "EVENT", "NEW LICENSE KEY FILE INSTALLED", "", ""},
	{ACS_LM_ALARM_EM_MODE_ACT, "EVENT", "EMERGENCY STATE ACTIVATED", "", ""},
	{ACS_LM_ALARM_EM_MODE_DEACT, "EVENT", "EMERGENCY STATE DEACTIVATED", "", ""},
	{ACS_LM_ALARM_TEST_MODE_ACT, "EVENT", "TEST MODE ACTIVATED", "", ""},
	{ACS_LM_ALARM_TEST_MODE_DEACT, "EVENT", "TEST MODE DEACTIVATED", "", ""},
	{ACS_LM_ALARM_PERSISTENT_FAULT, "EVENT", "PERSISTENT FILE CANNOT BE CREATED", "", ""},
    {ACS_LM_ALARM_JTP_CONN_FAULT, "EVENT", "JTP CONNECTION PROBLEM", "", ""},
	/* LM Maintenance Mode */
        {ACS_LM_ALARM_MAINTENANCE_MODE_ACT, "EVENT", "MAINTENANCE STATE ACTIVATED", "", ""},
        {ACS_LM_ALARM_MAINTENANCE_MODE_DEACT, "EVENT", "MAINTENANCE STATE DEACTIVATED", "", ""},
	{ACS_LM_ALARM_MAINTENANCE_MODE, "O1", MAINTENANCE_O1ALARM_PROBLEM_TEXT, "MAINTENANCE MODE ACTIVATED", ""},
};

/*=================================================================
	ROUTINE: ACS_LM_EventHandler constructor
=================================================================== */
ACS_LM_EventHandler::ACS_LM_EventHandler()
:alarmStore(NULL)
{	
	INFO("%s","In ACS_LM_EventHandler::ACS_LM_EventHandler()");
	alarmStore = ACS_LM_AlarmStore::load(ALARMDATAFILE);
	theAEHObjPtr = new acs_aeh_evreport();
}//end of constructor

/*=================================================================
	ROUTINE: ACS_LM_EventHandler destructor
=================================================================== */
ACS_LM_EventHandler::~ACS_LM_EventHandler()
{
	ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
	ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
	ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE);  //Cease Maintenance mode Observation alarm during service down.
	clearAll();		//FIX for TR HX92267
	delete alarmStore;
	delete theAEHObjPtr;
}//end of destructor
/*=================================================================
	ROUTINE: reportEvent
=================================================================== */
void  ACS_LM_EventHandler::reportEvent(LmEvent event, bool manualCease)
{	
	INFO("ACS_LM_EventHandler::reportEvent() %s, manualCease=%d","Entering",manualCease);

	ACE_TCHAR szProcName[64];
    	ACE_OS::sprintf(szProcName,ACE_TEXT(("%s:%d")),SERVICE_NAME,ACE_OS::getpid());
       
       try
       { 
        if( theAEHObjPtr != 0 )
        {
	 if ( !strcmp( event.probableCause.c_str(), MAINTENANCE_O1ALARM_PROBLEM_TEXT) )
	 {
		INFO("ACS_LM_EventHandler::reportEvent() event problem text=%s,manualCease=%d, %s", event.problemText.c_str(),manualCease, "Updating Maintenance with Date");
		std::string maintenanceModeEndString(". EXPIRY DATE:");
		std::string maintenanceModeEndTime("");
		/* To get dispalyed in the alarm printout, as LM triggers at mid night scheduling time(i.e, 00:05:00) */
		std::string strTemp = LM_MIDNIGHT_TRIGGERING_TIME; 

		time_t myMaintenanceEndDate = maintenanceEndTime;
		struct tm maintEndTimeLocal = *localtime(&myMaintenanceEndDate);
		std::ostringstream myOstr;
		myOstr<<std::setw(4)<<std::setfill('0')<<(maintEndTimeLocal.tm_year+1900)<<"-";
		myOstr<<std::setw(2)<<std::setfill('0')<<(maintEndTimeLocal.tm_mon+1)<<"-";
		myOstr<<std::setw(2)<<std::setfill('0')<<(maintEndTimeLocal.tm_mday);
		maintenanceModeEndTime = myOstr.str();
		std::replace(maintenanceModeEndTime.begin(),maintenanceModeEndTime.end(),'/','-');  //yyyy-mm-dd
		maintenanceModeEndTime.append(strTemp); //yyyy-mm-ddT00:00:00
		maintenanceModeEndString.append(maintenanceModeEndTime); //EXPIRY DATE:yyyy-mm-ddT00:00:00
		event.problemText.append(maintenanceModeEndString);
	 } 


	ACS_AEH_ReturnType status = theAEHObjPtr->sendEventMessage(szProcName,					// process name
								event.specificProblem,					// specific problem
								event.perceivedSeverity.c_str(),					// perceived severity
								event.probableCause.c_str(),	    // probably cause
								OBJECT_CLASS_NAME,						// object class of reference
								OBJECT_REFERENCE,					// object of reference
								event.problemData.c_str(),	// problem data
								event.problemText.c_str(),
								manualCease);	// problem text
        

	if (status == ACS_AEH_error)
	{
		ACE_INT32 err_code = theAEHObjPtr->getError();
		switch (err_code)
		{
		case ACS_AEH_syntaxError:

			INFO("Eventhandler:ACS_AEH_syntaxError with EventNumber: %d",event.specificProblem);
			//std::cout<<"Syntax Error in the event"<<endl;
			break;

		case ACS_AEH_eventDeliveryFailure:

			INFO("ACS_AEH_eventDeliveryFailure for process name :%s",SERVICE_NAME);
			//std::cout<<"ACS_AEH_eventDeliveryFailure for Event"<<endl;
			break;
		}
	}
	else if(status == ACS_AEH_ok)
	{
		INFO("ACS_LM_EventHandler::reportEvent() Event/Alarm sent successfully to AEH for the process name : %s",SERVICE_NAME);
	}
         }
       } 
catch(...)
{ 
WARNING("Exception has been caught in reportEvent : %s",SERVICE_NAME);
}
#if 0
	ACS_AEH_EvReport eventReporter;
	int status = ACS_AEH_error;
	
	status = eventReporter.sendEventMessage(SERVICE_NAME,
											event.specificProblem,
											event.perceivedSeverity.c_str(),
											event.probableCause.c_str(),
											OBJECT_CLASS_NAME,
											OBJECT_REFERENCE,
											event.problemData.c_str(),
											event.problemText.c_str(),
                                            manualCease);
	if(status == ACS_AEH_error)
	{
		int errorCode = eventReporter.getError();
		switch(errorCode)
		{
		case ACS_AEH_syntaxError:
			{
				ACS_LM_PrintTrace(ACS_LM_EventHandler, "reportEvent() ACS_AEH_syntaxError");
				break;
			}
		case ACS_AEH_eventDeliveryFailure:
			{
				ACS_LM_PrintTrace(ACS_LM_EventHandler, "reportEvent() ACS_AEH_eventDeliveryFailure");
				break;
			}
		default:
			{
				ACS_LM_PrintTrace(ACS_LM_EventHandler, "reportEvent() Unknown Error");
				break;
			}
			return;
		}
	}
#endif

	INFO("ACS_LM_EventHandler::reportEvent() Event has been raised for %s, %s",event.probableCause.c_str(),event.problemText.c_str());
	INFO("ACS_LM_EventHandler::reportEvent() %s","Leaving"); 
}//end of reportEvent


/*=================================================================
	ROUTINE: verifyAndRaiseAlarm
=================================================================== */
void ACS_LM_EventHandler::verifyAndRaiseAlarm(std::list<LkData*>& lkList, bool isServiceStartup)
{
	INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm() isServiceStartup = %d",isServiceStartup);

	if(isServiceStartup)
	{
		alarmStore->validate(lkList);
	}

	// should not be called for test mode LK as it does not apply for it
//	time_t now = ::time(NULL);
	LmEvent evtLkToExpireA2 = lmEvents[ACS_LM_ALARM_LK_EXPIRE_A2-ACS_LM_EVENT_ID_BASE];
    LmEvent evtLkToExpireA3 = lmEvents[ACS_LM_ALARM_LK_EXPIRE_A3-ACS_LM_EVENT_ID_BASE];
	

	lkList.sort(compare);

    for(std::list<LkData*>::iterator it=lkList.begin();
    		it!=lkList.end(); ++it)
    {
    	std::string lkId = (*it)->lkId;
    	std::string expDate = (*it)->endDate;
    	std::string startDate = (*it)->startDate;

    	//cout << "ID = "<< lkId << " endDate " << expDate << " StartDate " << startDate << endl;
    }
	bool raiseA3 = false;
	bool raiseA2 = false;
	bool lkInsertedIntoA3 = false;
	bool lkInsertedIntoA2 = false;
	bool ceaseA2 = false;

	for(std::list<LkData*>::iterator it=lkList.begin();
		it!=lkList.end(); ++it)
	{       
        std::string lkId = (*it)->lkId;
		std::string expDate = (*it)->endDate;

		INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): lkId = %s , expDate = %s",lkId.c_str(),expDate.c_str());

		if((expDate.compare("NEVER") != 0 ) && 
		   (lkId.compare(CXCEMERGENCYMODE)!= 0) && 
		   (lkId.compare(CXCTESTMODE)!= 0))
		{	
			std::string year = expDate.substr(0, 4);
			std::string monthAndDate = expDate.substr(5);
			size_t mmPos = monthAndDate.find_first_of("/");
			std::string month = monthAndDate.substr(0, mmPos);
			std::string day = monthAndDate.substr(mmPos+1);

			time_t curTimeUtc = ::time(NULL); 
			struct tm curTimeLocal = *localtime(&curTimeUtc);
			curTimeLocal.tm_year = ::atoi(year.c_str()) - 1900;
			curTimeLocal.tm_mon = ::atoi(month.c_str()) - 1;
			curTimeLocal.tm_mday = ::atoi(day.c_str());
			curTimeLocal.tm_hour = 0;
			curTimeLocal.tm_min = 0;
			curTimeLocal.tm_sec = 0;
							
			// convert date in string format to systemtime - end			
			long daysLeft = daysLeftToExpire(curTimeLocal);
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): daysLeft = %d ",daysLeft);

			if(daysLeft > 7 && daysLeft <= 60)
			{
				//A3 Alarm category.
				if(!alarmStore->isLkPresentInA3List(lkId))
				{	
					INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): raiseA3=true @ line %d",__LINE__);
					alarmStore->insertIntoA3List(lkId);
					lkInsertedIntoA3 = true;
				}
				/* A3 Alarm needs to be raised */
				if ( isServiceStartup || lkInsertedIntoA3 )
				{
					raiseA3=true;
					INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): isServiceStartup=%d lkInsertedIntoA3=%d raiseA3=%d",isServiceStartup,lkInsertedIntoA3,raiseA3);
				}
			}
			else if(daysLeft > 0 && daysLeft <= 7)
			{
				//A2 Alarm category.
				if(!alarmStore->isLkPresentInA2List(lkId))
				{	
					INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): raiseA2=true @ line %d",__LINE__);
					alarmStore->insertIntoA2List(lkId);
					lkInsertedIntoA2 = true;
				}
				/* A2 Alarm needs to be raised */
                                if ( isServiceStartup || lkInsertedIntoA2 )
                                {
                                        raiseA2=true;
					INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): isServiceStartup=%d lkInsertedIntoA2=%d raiseA2=%d",isServiceStartup,lkInsertedIntoA2,raiseA2);
                                }

				// removing from A3 list whether it exists or not
				alarmStore->removeFromA3List(lkId);				
			}
			else if(daysLeft <= 0) 
			{
				INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): ceaseA2=true @ line %d",__LINE__);

				if(alarmStore->isLkPresentInA2List(lkId))
				{
					ceaseA2=true;
					INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): ceaseA2=true @ line %d",__LINE__);
					// removing from A2 list whether it exists or not
					alarmStore->removeFromA2List(lkId);

                		}
		        	// removing from A3 list whether it exists or not
				alarmStore->removeFromA3List(lkId);
			}
		}  
	}  

	if (raiseA2)
	{
		// A2 alarm condition has priority
		// Cease A3. Always cease A3 before to raise an A2 to avoid both alarms concurrently
		evtLkToExpireA3.perceivedSeverity = "CEASING";
		INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Ceasing A3 Alarm @ %d",__LINE__);
		reportEvent(evtLkToExpireA3);
		// raise A2		
		evtLkToExpireA2.perceivedSeverity = "A2";
		INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Raising A2 Alarm @ %d",__LINE__);
		reportEvent(evtLkToExpireA2, true);
	} 
	else if (raiseA3)
	{
		if(!alarmStore->isA2ListEmpty())
		{
			// An A2 alarm condition exists so A2 must be the active alarm 
			
			// Cease A3. Always cease A3 before to raise an A2 to avoid both alarms concurrently
			//evtLkToExpire.problemText = LKEXP_A3ALARM_PROBLEM_TEXT;
			evtLkToExpireA3.perceivedSeverity = "CEASING";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Ceasing A3 Alarm @ %d",__LINE__);
			reportEvent(evtLkToExpireA3);
	
			// raise A2
			//evtLkToExpire.problemText = LKEXP_A2ALARM_PROBLEM_TEXT;
			evtLkToExpireA2.perceivedSeverity = "A2";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Raising A2 Alarm @ %d",__LINE__);
			reportEvent(evtLkToExpireA2, true);
		}
		else
		{// There isn't A2 alarm condition. Can raise A3.
			
//			if (ceaseA2)
//			{// Cease A2 (if any). A2 must be ceased to avoid both alarms concurrently
				//evtLkToExpire.problemText = LKEXP_A2ALARM_PROBLEM_TEXT;
				evtLkToExpireA2.perceivedSeverity = "CEASING";
				INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Ceasing A2 Alarm @ %d",__LINE__);
				reportEvent(evtLkToExpireA2);
//			}
			// Raise A3
			evtLkToExpireA3.problemText = LKEXP_A3ALARM_PROBLEM_TEXT;
			evtLkToExpireA3.perceivedSeverity = "A3";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Raising A3 Alarm@ %d",__LINE__);
			reportEvent(evtLkToExpireA3, true);
		}
	}
	
	else if (ceaseA2)
	{
		if (!alarmStore->isA2ListEmpty())
		{	//An A2 alarm condition is still valid. Raise A2 again.
			// Cease A3. A3 (if any) must be ceased to avoid both alarms concurrently
			evtLkToExpireA3.perceivedSeverity = "CEASING";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Ceasing A3 Alarm @ %d",__LINE__);
			reportEvent(evtLkToExpireA3);
	
			// raise A2
			evtLkToExpireA2.perceivedSeverity = "A2";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Raising A2 Alarm @ %d",__LINE__);
			reportEvent(evtLkToExpireA2, true);
		}
		else if (!alarmStore->isA3ListEmpty())
		{	// An A3 alarm condition is still valid. Raise an A3 alarm
		
			// Cease A2. A2 (if any) must be ceased to avoid both alarms concurrently
			evtLkToExpireA2.perceivedSeverity = "CEASING";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Ceasing A2 Alarm @ %d",__LINE__);
			reportEvent(evtLkToExpireA2);
	
			// raise A3
			evtLkToExpireA3.perceivedSeverity = "A3";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Raising A3 Alarm @ %d",__LINE__);
			reportEvent(evtLkToExpireA3, true);
		}       
		else 
		{	// No LK expiring anymore. Cease the alarm.            
			// Cease A2			
			evtLkToExpireA2.perceivedSeverity = "CEASING";
			INFO("ACS_LM_EventHandler::verifyAndRaiseAlarm(): Ceasing A2 Alarm @ @ %d",__LINE__);
			reportEvent(evtLkToExpireA2);
		}			
	} 
	//CNI55_4_578
//	else
//	{
//	//This could be ralated to a new LKF intallation. Where all LK are ok and then we must be sure we cease all previous alarms
//	// alternatively we could cease these alarms at LKF installation before to invoke this procedure and then remove the this else statement
//        //cease A2
//        if(alarmStore->isA2ListEmpty())
//        {
//			evtLkToExpireA2.perceivedSeverity = "CEASING";
//			DEBUG("verifyAndRaiseAlarm(): Ceasing A2 Alarm @ @ %d",__LINE__);
//			reportEvent(evtLkToExpireA2);
//	    }
//
//        //cease A3
//        if(alarmStore->isA3ListEmpty())
//        {
//            evtLkToExpireA3.perceivedSeverity = "CEASING";
//			DEBUG("verifyAndRaiseAlarm(): Ceasing A3 Alarm @ @ %d",__LINE__);
//			reportEvent(evtLkToExpireA3);
//	        }
//	}
    
	//if(!lkList.empty())
	{
		if(!alarmStore->commit())
		{
			ERROR("ACS_LM_EventHandler::verifyAndRaiseAlarm() %s","verifyAndRaiseAlarm(): commit() failed ");
		}
	}
	DEBUG("ACS_LM_EventHandler::verifyAndRaiseAlarm() %s"," Exiting from verifyAndRaiseAlarm() ");
}//end of verifyAndRaiseAlarm
/*=================================================================
	ROUTINE: compare
=================================================================== */
ACE_INT16 ACS_LM_EventHandler::compare(LkData* first, LkData* second)
{
	std::string  firstLkDate;
	firstLkDate = first->endDate;
	std::string  secondLkDate;
	secondLkDate = second->endDate;
	ACE_INT16 firstLkYear = ACE_OS::atoi((firstLkDate.substr(0,4)).c_str());
	std::string firstLkMonth1 = firstLkDate.substr(5);
	size_t firstLkPos = firstLkMonth1.find_first_of("/");
	ACE_INT16 firstLkMonth = ACE_OS::atoi((firstLkMonth1.substr(0,firstLkPos)).c_str());
	ACE_INT16 firstLkDay = ACE_OS::atoi((firstLkMonth1.substr(firstLkPos+1)).c_str());

	ACE_INT16 secondLkYear = ACE_OS::atoi((secondLkDate.substr(0,4)).c_str());
	std::string secondLkMonth1 = secondLkDate.substr(5);
	size_t pos = secondLkMonth1.find_first_of("/");
	ACE_INT16 secondLkMonth = ACE_OS::atoi((secondLkMonth1.substr(0,pos).c_str()));
	ACE_INT16 secondLkDay = ACE_OS::atoi((secondLkMonth1.substr(pos+1)).c_str());

	if(firstLkYear > secondLkYear)
	{
		return 1;
	}
	else if(firstLkYear == secondLkYear)
	{
		if(firstLkMonth > secondLkMonth)
		{
			return 1;
		}
		else if(firstLkMonth == secondLkMonth)
		{
			if(firstLkDay > secondLkDay)
			{
				return 1;
			}
			else if(firstLkDay == secondLkDay)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}
}//end of compare
/*=================================================================
	ROUTINE: raiseAlarm
=================================================================== */
void ACS_LM_EventHandler::raiseAlarm(ACE_UINT16 alarmId)
{
	DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","Entering");
	if(alarmId == ACS_LM_ALARM_LK_FILE_MISS)
	{
		DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","Raising LKF missing alarm");
		if(!alarmStore->getLkfMissingAlarmState())
		{
			LmEvent evtLkfMissing = lmEvents[ACS_LM_ALARM_LK_FILE_MISS-ACS_LM_EVENT_ID_BASE];
			reportEvent(evtLkfMissing);

			alarmStore->setLkfMissingAlarmState(true);
		}
		else
		{
			DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","LKF missing alarm has already been raised");
		}
	}
	else if(alarmId == ACS_LM_ALARM_EM_STATE_UNAVAIL)
	{
		if(!alarmStore->getEmergencyAlarmState())
		{
			DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","Raising Emergency state not availiable alarm");
			LmEvent evtEmStateNotAvailable = lmEvents[ACS_LM_ALARM_EM_STATE_UNAVAIL-ACS_LM_EVENT_ID_BASE];
			reportEvent(evtEmStateNotAvailable);	     	

			alarmStore->setEmergencyAlarmState(true);
		}
		else
		{
			DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","Emergency state not availiable alarm has already been raised");
		}
	}
	/* LM Maintenance Mode */
	else if(alarmId == ACS_LM_ALARM_MAINTENANCE_MODE)
        {
                if(!alarmStore->getMaintenanceAlarmState())
                {
			DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","Raising Maintenance mode Observation alarm");
                        LmEvent evtMaintenanceActivate = lmEvents[ACS_LM_ALARM_MAINTENANCE_MODE-ACS_LM_EVENT_ID_BASE];
                        reportEvent(evtMaintenanceActivate);

                        alarmStore->setMaintenanceAlarmState(true);
                }
                else
                {
			DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","Maintenance mode Observation alarm has already been raised");
                }
        }
	DEBUG("ACS_LM_EventHandler::raiseAlarm() %s","Leaving");
}//end of raiseAlarm
/*=================================================================
	ROUTINE: ceaseAlarm
=================================================================== */
void ACS_LM_EventHandler::ceaseAlarm(ACE_UINT16 alarmId)
{
	if(alarmId == ACS_LM_ALARM_LK_FILE_MISS)
	{
		if(alarmStore->getLkfMissingAlarmState())
		{
			LmEvent evtLkfMissing = lmEvents[ACS_LM_ALARM_LK_FILE_MISS-ACS_LM_EVENT_ID_BASE];
			evtLkfMissing.perceivedSeverity = "CEASING";
			reportEvent(evtLkfMissing);
			
			DEBUG("ACS_LM_EventHandler::ceaseAlarm() %s","Ceased LKF missing alarm");
			alarmStore->setLkfMissingAlarmState(false);
		}
	}
	else if(alarmId == ACS_LM_ALARM_EM_STATE_UNAVAIL)
	{
		if(alarmStore->getEmergencyAlarmState())
		{
			LmEvent evtEmStateNotAvailable = lmEvents[ACS_LM_ALARM_EM_STATE_UNAVAIL-ACS_LM_EVENT_ID_BASE];
			evtEmStateNotAvailable.perceivedSeverity = "CEASING";
			reportEvent(evtEmStateNotAvailable);
			DEBUG("ACS_LM_EventHandler::ceaseAlarm() %s","Ceased emergency alarm");
			alarmStore->setEmergencyAlarmState(false);
		}
	}
	/* LM Maintenance Mode */
	else if(alarmId == ACS_LM_ALARM_MAINTENANCE_MODE)
        {
                if(alarmStore->getMaintenanceAlarmState())
                {
                        LmEvent evtMaintenanceDeactivate = lmEvents[ACS_LM_ALARM_MAINTENANCE_MODE-ACS_LM_EVENT_ID_BASE];
                        evtMaintenanceDeactivate.perceivedSeverity = "CEASING";
                        reportEvent(evtMaintenanceDeactivate);
			DEBUG("ACS_LM_EventHandler::ceaseAlarm() %s","Ceased Maintenance mode observation alarm");
                        alarmStore->setMaintenanceAlarmState(false);
                }
        }

}//end of ceaseAlarm

/*=================================================================
	ROUTINE: clearAll
=================================================================== */
void ACS_LM_EventHandler::clearAll()
{	
	DEBUG("ACS_LM_EventHandler::clearAll() %s","Entering");
    LmEvent evtLkToExpireA2 = lmEvents[ACS_LM_ALARM_LK_EXPIRE_A2-ACS_LM_EVENT_ID_BASE];
    LmEvent evtLkToExpireA3 = lmEvents[ACS_LM_ALARM_LK_EXPIRE_A3-ACS_LM_EVENT_ID_BASE];
	
	//cease A2
    if(!alarmStore->isA2ListEmpty())
    {
        evtLkToExpireA2.problemText = LKEXP_A2ALARM_PROBLEM_TEXT;
		evtLkToExpireA2.perceivedSeverity = "CEASING";
		INFO("ACS_LM_EventHandler::clearAll() Ceasing A2 Alarm @ %d",__LINE__);
		reportEvent(evtLkToExpireA2);
	}

    //cease A3
    if(!alarmStore->isA3ListEmpty())
    {
        evtLkToExpireA3.problemText = LKEXP_A3ALARM_PROBLEM_TEXT;
		evtLkToExpireA3.perceivedSeverity = "CEASING";
		INFO("ACS_LM_EventHandler::clearAll() Ceasing A3 Alarm @ %d",__LINE__);
		reportEvent(evtLkToExpireA3);
	}	      
	alarmStore->clearA2List();
	alarmStore->clearA3List();
	if(!alarmStore->commit())
	{
		ERROR("ACS_LM_EventHandler::clearAll() %s","commit() failed");
	}

	DEBUG("ACS_LM_EventHandler::clearAll() %s","Leaving");
}//end of clearAll
/*=================================================================
	ROUTINE: daysLeftToExpire
=================================================================== */
long ACS_LM_EventHandler::daysLeftToExpire(struct tm& lkExpTime)
{
	time_t curTimeUtc = time(NULL);
	struct tm curTimeLocal = *localtime(&curTimeUtc);
	curTimeLocal.tm_hour = 0;
	curTimeLocal.tm_min = 0;
	curTimeLocal.tm_sec = 0;
	time_t curTime = mktime(&curTimeLocal);
	time_t expTime = mktime(&lkExpTime);
	ACE_INT64 daysToExpire = ((ACE_INT64)(difftime(expTime, curTime)/(60*60*24))) + 1;

	return daysToExpire;
}//end of daysLeftToExpire
/*=================================================================
	ROUTINE: clearLists
=================================================================== */
void ACS_LM_EventHandler::clearLists()
{
    DEBUG("ACS_LM_EventHandler::clearLists() %s","Entering");

    std::string nodeIp = ACS_LM_Common::getThisNodeIp();
    std::string ownerFile = alarmStore->getOwnerOfFile();
    DEBUG("ACS_LM_EventHandler::clearLists(): nodeIp=%s, Alarm_File_Owner=%s",nodeIp.c_str(),ownerFile.c_str() );

    bool clearList = false;
    if((nodeIp.compare(AP1_NET1_NODEA)!=0) || (nodeIp.compare(AP1_NET2_NODEA)!=0))
    {
        if((ownerFile.compare(AP1_NET1_NODEA)!=0) || (ownerFile.compare(AP1_NET2_NODEA)!=0))
        {
            clearList = false;
    	    DEBUG("ACS_LM_EventHandler::clearLists(): %s, clearList=%d", "In First block",clearList);
        }
        else
        {
            clearList = true;
    	    DEBUG("ACS_LM_EventHandler::clearLists(): %s, clearList=%d", "In First block",clearList);
        }
    }
    if((nodeIp.compare(AP2_NET1_NODEA)!=0) || (nodeIp.compare(AP2_NET2_NODEA)!=0))
    {
        if((ownerFile.compare(AP2_NET1_NODEA)!=0) || (ownerFile.compare(AP2_NET2_NODEA)!=0))
        {
            clearList = false;
    	    DEBUG("ACS_LM_EventHandler::clearLists(): %s, clearList=%d", "In Second block",clearList);
        }
        else
        {
            clearList = true;
    	    DEBUG("ACS_LM_EventHandler::clearLists(): %s, clearList=%d", "In Second block",clearList);
        }
    }

    if(clearList)
    {
        alarmStore->clearA2List();
	alarmStore->clearA3List();        
    }
    alarmStore->setOwnerOfFile(nodeIp);
    if(!alarmStore->commit())
    {
	ERROR("ACS_LM_EventHandler::clearLists() %s","commit() failed");
    }

    DEBUG("ACS_LM_EventHandler::clearLists() %s","Leaving");
}//end of clearLists
