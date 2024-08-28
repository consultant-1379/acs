/*=================================================================== */
/**
   @file acs_hcs_jobscheduler.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       		     XMALRAO       Initial Release */

/*================================================================== 
                        INCLUDE DECLARATION SECTION
================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <ACS_APGCC_CommonLib.h>
#include "acs_hcs_jobscheduler.h"
#include "acs_hcs_jobScheduler_objectImpl.h"
#include "acs_hcs_healthcheckservice.h"
#include "acs_hcs_global_ObjectImpl.h"

using namespace std;

/*==================================================================
	STATIC VARIABLES INITIALIZATION
================================================================== */
bool jobScheduler::stopRequested = false;
bool jobScheduler::threadExited  = false;
bool jobScheduler::jobExecution  = true;
bool jobScheduler::eventCallbackFlag = false;

/*===================================================================
		   ROUTINE: svc
=================================================================== */
int jobScheduler::svc()
{
        DEBUG("%s","Entering jobScheduler::svc() ");
	cout<<"In jobScheduler thread"<<endl;
        jobScheduler job;
	job.scheduledExecution();
        cout << "jobScheduler exiting" << endl;
        threadExited = true;
        DEBUG("%s","Leaving jobScheduler::svc() ");
        return 0;
}

/*===================================================================
		   ROUTINE: scheduledExecution
=================================================================== */
void jobScheduler::scheduledExecution()
{
	DEBUG("%s","Entering  jobScheduler::getJobForExecution()");
	acs_hcs_global_ObjectImpl obj;
	string jobName; 
	string scheduledTime;
	string currentTime;
	unsigned int adminStateValue = 0;

	while(!jobScheduler::stopRequested)
	{
		jobName = acs_hcs_jobScheduler_ObjectImpl::getJobRdn_execution();
		jobScheduler::eventCallbackFlag = false;
		
		if(strcmp(jobName.c_str(),NOJOB)==0)
		{
			//No job for execution
			ACE_OS::sleep(1);	
			continue;
		}
		else
		{
			string jobSchedulerName = JOBSCHEDULER + jobName;
			acs_hcs_global_ObjectImpl obj;
			scheduledTime =  obj.getImmAttribute(jobSchedulerName.c_str(),nextScheduledTimeAttr);
			currentTime = obj.getCurrentTime();

			adminStateValue = obj.getImmIntAttribute(jobSchedulerName.c_str(),adminStateAttr);
			
			while(!jobScheduler::eventCallbackFlag && !jobScheduler::stopRequested)
			{
				if(obj.compareDates(scheduledTime,currentTime))//scheduledTime <= currentTime
				{
					if(adminStateValue == UNLOCK)
					{
						char tempJob[256]={0};
						strncpy(tempJob,jobName.c_str(),sizeof(tempJob)-1);
						void *arg = reinterpret_cast<void*>(tempJob);
						while(!jobScheduler::eventCallbackFlag && !jobScheduler::stopRequested)
						{
						        jobScheduler::jobExecution  = true;	
							AcsHcs::HealthCheckService::scheduledExecute(arg);
							if(!jobScheduler::jobExecution && !jobScheduler::stopRequested)
							{
								//job execution was not successfully done 
								bool sleep_flag = obj.waitForSomeTime(10); //Waiting for some time to try again
								if(sleep_flag == true)
									continue;	
								else
									break;
							}
							//Job executed successfully
							if(!jobScheduler::stopRequested)
								acs_hcs_jobScheduler_ObjectImpl::updationAfterJobExecution(jobName,scheduledTime);

							break;
						}
					}
					else if(adminStateValue == LOCK )
					{
						acs_hcs_jobScheduler_ObjectImpl::updationAfterJobExecution(jobName,scheduledTime);
						break;
					}
					break;
				}
				else
				{
					if(!jobScheduler::stopRequested)
					{
						ACE_OS::sleep(1);	
						currentTime = obj.getCurrentTime();
						continue;
					}
					else  //Call for graceful termination is received
						break;
				}
			}
			if(!jobScheduler::stopRequested)
			{
				ACE_OS::sleep(1);	
	                	continue;
			}
			else //Call for graceful termination is received
				break;
		}
		if(!jobScheduler::stopRequested)
		{
			ACE_OS::sleep(1);	
			continue;
		}
		else //Call for graceful termination is received
			break;
	}
        DEBUG("%s","Leaving  jobScheduler::getJobForExecution()");	
}
