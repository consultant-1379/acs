//======================================================================
//
// NAME
//      HealthCheckService.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-04-30 by EGINSAN
// CHANGES
//     
//======================================================================

#include "acs_hcs_healthcheckservice.h"

#include "acs_hcs_cache.h"
#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_exitcodes.h"
#include "acs_hcs_synchron.h"
#include "acs_hcs_environment.h"
#include "acs_hcs_scheduler.h"
#include "acs_hcs_haservice.h"
#include "acs_hcs_inotify.h"
#include "acs_aeh_signalhandler.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_Util.H"
#include <ACS_CS_API.h>
#include <syslog.h>
#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_progressReportHandler.h"
#include "acs_hcs_jobscheduler.h"
#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_logger.h"
#include "acs_hcs_jobbased_objectImpl.h"
#include <ACS_APGCC_AmfTypes.h>
#include <sstream>
#include "acs_hcs_jobexecutor.h"
using namespace std;
using namespace AcsHcs;

ACS_HC_HAService *theHCApplPtr = 0;
void printUsage();
bool AcsHcs::HealthCheckService::jobCreated = false;
bool AcsHcs::HealthCheckService::theInteractiveMode = false;
bool AcsHcs::HealthCheckService::stopRequested = false;
bool AcsHcs::HealthCheckService::diskFull = false;
bool AcsHcs::HealthCheckService::folderDeleted = false;
string AcsHcs::HealthCheckService::sameJob = EMPTY_STR;
bool AcsHcs::HealthCheckService::progressDeleted = false;  
bool unload_flag = false;
int triggerJobCount = 0;
bool AcsHcs::HealthCheckService::isLoadOngoing = false;
bool AcsHcs::HealthCheckService::isUnloadOngoing = false;
bool AcsHcs::HealthCheckService::isShutdownOngoing = false;

std::list<string> loadvect;


/*==========================================================================
        ROUTINE: sighandler
========================================================================== */
void sighandler(int signum)
{
        DEBUG("%s","Entering sighandler");
        if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
        {
                if(!AcsHcs::HealthCheckService::theInteractiveMode)
                {
                        if(theHCApplPtr != NULL)
                        {
                                theHCApplPtr->performComponentRemoveJobs();
                                theHCApplPtr->performComponentTerminateJobs();
                        }
                }
                else
                {
        		AcsHcs::HealthCheckService::stopRequested = true;               	 
                }
        }
        DEBUG("%s","Leaving sighandler");
}

//*************************************************************************
// main

// It all starts here.
//*************************************************************************

int ACE_TMAIN(int argc, char *argv[])
{
	cout << "In main" << endl;
        struct sigaction sa;
        sa.sa_handler = sighandler;
        sa.sa_flags = SA_RESTART;
        sigemptyset(&sa.sa_mask );

        if( sigaction(SIGINT, &sa, NULL ) == -1)
        {
                ERROR("%s", "Error occured while handling SIGINT in acs_hcd");
                return -1;
        }
        if( sigaction(SIGTERM, &sa, NULL ) == -1)
        {
                ERROR( "%s", "Error occured while handling SIGTERM in acs_hcd");
                return -1;
        }
        if( sigaction(SIGTSTP, &sa, NULL ) == -1)
        {
                ERROR( "%s", "Error occured while handling SIGTSTP in acs_hcd");
                return -1;
        }
        if( argc > 1)
        {
                // If -d flag is specified, then the user has requested to start the
                // service in debug mode.

		int res;
		cout << "Log opening" << endl;
		res = acs_hcs_logger::open("HC");
		cout << "Log opened" << endl;

                if( argc == 2 && (!strcmp(argv[1],"-d")) )
                {
                        AcsHcs::HealthCheckService::theInteractiveMode = true;
			cout << "In Debug mode" << endl;
                        DEBUG("%s", "acs_hcd started in debug mode...");
			AcsHcs::HealthCheckService service;
			service.start();
                }
                else
                {
                        printUsage();
                }
		acs_hcs_logger::close();
        }
        else
        {
                DEBUG("%s","Starting acs_hcd in HA mode");
                AcsHcs::HealthCheckService::theInteractiveMode = false;

                ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

                theHCApplPtr = new ACS_HC_HAService("acs_hcd", "root");
		acs_hcs_logger::open("HC");
                int rCode = 0;
                if (!theHCApplPtr)
                {
                        ERROR("%s", "Unable to allocate memory for ACS_HC_HA_Service object");
                        rCode=-2;
			acs_hcs_logger::close();
                        return rCode;
                }

                DEBUG("%s", "Starting acs_hcd with HA...");

                errorCode = theHCApplPtr->activate();

                if (errorCode == ACS_APGCC_HA_FAILURE)
                {
                        ERROR("%s", "HA Activation Failed for acs_hcd");
                }
                else if( errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
                {
                        ERROR("%s", "HA Application Failed to close gracefully for acs_hcd");
                }
                else if (errorCode == ACS_APGCC_HA_SUCCESS)
                {
                        DEBUG("%s", "HA Application Gracefully closed for acs_hcd");
                }
                else
                {
                        ERROR("%s", "Error occured while starting acs_hcd with HA");
                }
                delete theHCApplPtr;
                theHCApplPtr = 0;
		acs_hcs_logger::close();
        }
        DEBUG("%s","Leaving ACE_TMAIN");
        return 0;
}

void printUsage()
{
	DEBUG("%s","In printUsage ");
        cout<<"Usage: acs_hcd [-d]" <<endl;
}

namespace AcsHcs
{
#define SAMPLING_PERIOD 5000
#define NUMBER_OF_SAMPLES 6

	// Constructor
	HealthCheckService::HealthCheckService()
	{
		DEBUG("%s"," Entering HealthCheckService::HealthCheckService()(constructor)");
		ReadyToUseHandler = 0;
                InUseHandler = 0;
                JobBasedHandler = 0;
		JobSchedulerHandler = 0;
		SingleEventHandler = 0;
		PeriodicEventHandler = 0;
		CalenderPeriodicEventHandler = 0;
		ProgressReportImpl = 0;
		m_threadGroupId =0;
		DEBUG("%s"," Leaving HealthCheckService::HealthCheckService()(constructor)");
	}

	// Destructor
	HealthCheckService::~HealthCheckService()
	{
	}

	int HealthCheckService::setupHCThread(AcsHcs::HealthCheckService*)
	{	
		DEBUG("%s","Entering HealthCheckService::setupHCThread");
		m_threadGroupId =0;
		m_threadGroupId = ACE_Thread_Manager::instance ()->spawn(ACE_THR_FUNC (&HealthCheckService::svc),
		NULL,
		THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
		0,
		0,
		ACE_DEFAULT_THREAD_PRIORITY,
		-1,
		0,
		ACE_DEFAULT_THREAD_STACKSIZE,
		0);
		if (m_threadGroupId == -1)
			
		{
			DEBUG("%s","In  if (ACE_Thread_Manager::instance ()->spawn");
			return -1;
		}
		DEBUG("%s","Leaving HealthCheckService::setupHCThread");
		return 0;
	}

	bool HealthCheckService::isBrfcPermitted()
	{
		if( (!HealthCheckService::isLoadOngoing) && (!HealthCheckService::isUnloadOngoing ))
			return true;
		else
			return false;
	}
	
	// To create HCS directories and assign appropriate ACLs
	bool HealthCheckService::createHCSDirectories()
	{
		DEBUG("%s","Entering HealthCheckService::createHCSDirectories()");
		try
		{
			// For SUGAR implementation folders have to be created in LCTBIN.
			/*Directory::createDir(Configuration::Singleton::get()->getRulesDir());
			Directory::createDir(Configuration::Singleton::get()->getReportsDir());*/
			Directory::createDir(Configuration::Singleton::get()->getBackupPath());
			Cache::Singleton::get()->create();
		}
		catch(...)
		{
			DEBUG("%s","Leaving HealthCheckService::createHCSDirectories() with false");
			return false;
		}
		DEBUG("%s","Leaving HealthCheckService::createHCSDirectories()");
		return true;
	}

	void HealthCheckService::svc(void)
	{
		DEBUG("%s","Entering HealthCheckService::svc()");
		HealthCheckService service;
		DEBUG("%s","Calling service.start()");	
		service.start();
		DEBUG("%s","Leaving HealthCheckService::svc()");
	}

	bool HealthCheckService::setupIMMCallBacks()
	{
		DEBUG("%s","Entering HealthCheckService::setupIMMCallBacks()");
	        ACE_TCHAR readytouseImplName[100];
        	ACE_OS::memset(readytouseImplName, 0, sizeof(readytouseImplName));
	        ACE_OS::strcpy(readytouseImplName, "ReadyToUseImplementer");

                ACE_TCHAR inuseImplName[100];
                ACE_OS::memset(inuseImplName, 0, sizeof(inuseImplName));
                ACE_OS::strcpy(inuseImplName, "InUseImplementer");

                ACE_TCHAR jobBasedImplName[100];
                ACE_OS::memset(jobBasedImplName, 0, sizeof(jobBasedImplName));
                ACE_OS::strcpy(jobBasedImplName, "JobBasedImplementer");

                ACE_TCHAR jobSchedulerImplName[100];
                ACE_OS::memset(jobSchedulerImplName, 0, sizeof(jobSchedulerImplName));
                ACE_OS::strcpy(jobSchedulerImplName, "JobSchedulerImplementer");

                ACE_TCHAR singleEventImplName[100];
                ACE_OS::memset(singleEventImplName, 0, sizeof(singleEventImplName));
                ACE_OS::strcpy(singleEventImplName, "SingleEventImplementer");

                ACE_TCHAR periodicEventImplName[100];
                ACE_OS::memset(periodicEventImplName, 0, sizeof(periodicEventImplName));
                ACE_OS::strcpy(periodicEventImplName, "PeriodicEventImplementer");

                ACE_TCHAR calenderPeriodicEventImplName[100];
                ACE_OS::memset(calenderPeriodicEventImplName, 0, sizeof(calenderPeriodicEventImplName));
                ACE_OS::strcpy(calenderPeriodicEventImplName, "CalendarPeriodicEventImplementer");

		DEBUG("%s","Creating acs_hcs_readyToUse_ObjectImpl");
        	ReadyToUseHandler = new acs_hcs_readyToUse_ObjectImpl(ACS_HC_READYTOUSE_CLASS_NAME, readytouseImplName, ACS_APGCC_ONE);
		DEBUG("%s","Creating acs_hcs_inUse_ObjectImpl");
        	InUseHandler = new acs_hcs_inUse_ObjectImpl(ACS_HC_INUSE_CLASS_NAME, inuseImplName, ACS_APGCC_ONE);
		DEBUG("%s","Creating acs_hcs_jobbased_ObjectImpl");
		JobBasedHandler = new acs_hcs_jobbased_ObjectImpl(ACS_HC_JOBBASED_CLASS_NAME, jobBasedImplName, ACS_APGCC_ONE);	
		DEBUG("%s","Creating acs_hcs_jobScheduler_ObjectImpl");
		JobSchedulerHandler = new acs_hcs_jobScheduler_ObjectImpl(ACS_HC_JOBSCHEDULER_CLASS_NAME, jobSchedulerImplName, ACS_APGCC_ONE);
		DEBUG("%s","Creating acs_hcs_singleEvent_ObjectImpl");
		SingleEventHandler = new acs_hcs_singleEvent_ObjectImpl(ACS_HC_SINGLEEVENT_CLASS_NAME, singleEventImplName, ACS_APGCC_ONE);
		DEBUG("%s","Creating acs_hcs_periodicEvent_ObjectImpl");
		PeriodicEventHandler = new acs_hcs_periodicEvent_ObjectImpl(ACS_HC_PERIODICEVENT_CLASS_NAME, periodicEventImplName, ACS_APGCC_ONE);
		DEBUG("%s","Creating acs_hcs_calendarPeriodicEvent_ObjectImpl");
		CalenderPeriodicEventHandler = new acs_hcs_calenderPeriodicEvent_ObjectImpl(ACS_HC_CALENDERPERIODICEVENT_CLASS_NAME, calenderPeriodicEventImplName, ACS_APGCC_ONE);
		DEBUG("%s","Creating acs_hcs_progressReport_ObjectImpl");	
		ProgressReportImpl = new acs_hcs_progressReport_ObjectImpl(); 

	        if(( ReadyToUseHandler == 0) || (InUseHandler == 0) || (JobBasedHandler == 0) || (JobSchedulerHandler == 0) 
						|| (SingleEventHandler ==0) || (PeriodicEventHandler == 0) || (CalenderPeriodicEventHandler == 0) 
						|| ProgressReportImpl == 0)
        	{
			DEBUG("%s","Leaving HealthCheckService::setupIMMCallBacks() returning false");
        	        return false;
	        }
		
                AcsHcs::HealthCheckService hcService;

		DEBUG("%s","Setting Class Implementer for ReadyToUse");
		ReadyToUseHandler->setImpl();
		DEBUG("%s","Activating ReadyToUse Handler");
        	ReadyToUseHandler->activate();

		DEBUG("%s","Setting Class Implementer for InUse");
		InUseHandler->setImpl();
	       	DEBUG("%s","Activating InUse Handler");
		InUseHandler->activate();

		DEBUG("%s","Setting Class Implementer for JobBased Handler")
		JobBasedHandler->setImpl();
		DEBUG("%s","Activating JobBased Handler");
		JobBasedHandler->activate();

		DEBUG("%s","Setting Class Implementer for JobSchedular Handler")
		JobSchedulerHandler->setImpl();
		DEBUG("%s","Activating JobScheduler Handler");
		JobSchedulerHandler->activate();

		DEBUG("%s","Setting Class Implementer for SingleEvent Handler")
		SingleEventHandler->setImpl();
		DEBUG("%s","Activating SingleEvent Handler");
		SingleEventHandler->activate();

		DEBUG("%s","Setting Class Implementer for PeriodicEvent Handler")
		PeriodicEventHandler->setImpl();
		DEBUG("%s","Activating PeriodicEvent Handler");
		PeriodicEventHandler->activate();
		
		DEBUG("%s","Setting Class Implementer for CalendarPeriodicEvent Handler")
		CalenderPeriodicEventHandler->setImpl();
		DEBUG("%s","Activating CalendarPeriodicEvent Handler");
		CalenderPeriodicEventHandler->activate();

		hcService.createProgressReport(READYTOUSEDN);
	        ACE_OS::sleep(1);	
		hcService.createProgressReport(INUSEDN);

		OmHandler immHandler;
		ACS_CC_ReturnType returnCode;
		std::vector<std::string> jobInstances, schedulerInstances;

		returnCode = immHandler.Init();
		if(returnCode != ACS_CC_SUCCESS)
		{       
			DEBUG("%s","ERROR: init FAILURE!!! in setupIMMCallBacks ");
			cout<<"ERROR: init FAILURE!!!\n";
			return false;
		}
	
		/* Create progress reports for all the existing jobs*/
		returnCode = immHandler.getClassInstances(ACS_HC_JOBBASED_CLASS_NAME, jobInstances);
		if(returnCode != ACS_CC_SUCCESS)
		{
			cout << "getClassInstances failed" << endl;
			DEBUG("%s","getClassInstances failed");
			if(immHandler.getInternalLastError() != -41)
			{
			 	DEBUG("%s","immHandler.getInternalLastError() != -41 returning false");	
				return false;
			}
		}
		else
		{
			for(unsigned int i = 0; i < jobInstances.size(); i++)
			{
				DEBUG("%s","createProgressReport for job instances");
				hcService.createProgressReport(jobInstances[i]);
			}
		}

		/* Create progress reports for all the existing job schedulers*/
		returnCode = immHandler.getClassInstances(ACS_HC_JOBSCHEDULER_CLASS_NAME, schedulerInstances);
		if(returnCode != ACS_CC_SUCCESS)
		{
			cout << "getClassInstances failed" << endl;
			DEBUG("%s","getClassInstances for scheduler failed");
			if(immHandler.getInternalLastError() != -41)
			{
				DEBUG("%s","immHandler.getInternalLastError() != -41 for scheduler returning false");
				return false;
			}
		}
		else
		{
			for(unsigned int i = 0; i < schedulerInstances.size(); i++)
			{
				DEBUG("%s","createProgressReport for job scheduler instances");
				hcService.createProgressReport(schedulerInstances[i]);
			}
		}
		DEBUG("%s","Leaving HealthCheckService::setupIMMCallBacks() returning true");
        	return true;
	}

	void HealthCheckService::updateProgress(int state, int result, string resultInfo, int index, bool completed, string addInfo)
	{
		//DEBUG("%s","Updating Progress Report");
		// Introduced progressDeleted variable for TR HS37486
		if(AcsHcs::HealthCheckService::progressDeleted == false)
		{
			acs_hcs_global_ObjectImpl obj;
			acs_hcs_progressReport_ObjectImpl::progressReport[index].state = state;
			acs_hcs_progressReport_ObjectImpl::progressReport[index].result = result;
			acs_hcs_progressReport_ObjectImpl::progressReport[index].resultInfo = resultInfo;	
			if( resultInfo != "")
		
				acs_hcs_progressReport_ObjectImpl::progressReport[index].resultInfo = resultInfo;
			else
				acs_hcs_progressReport_ObjectImpl::progressReport[index].resultInfo = "";
		

			{
				string currentTime = obj.getCurrentTime();
				if(!completed)
				{		
					acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionStarted = currentTime;
					acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionCompleted = "";
				}	
				else
				{	
					acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionCompleted = currentTime;		
				}	
			
				acs_hcs_progressReport_ObjectImpl::progressReport[index].timeOfLastStatusUpdate = currentTime;
				if(addInfo!="")
					acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo = addInfo;
				else
					acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo = "";
			}
		}
		else
			DEBUG("%s", "No progress report to update");
			
		//DEBUG("%s","Updated Progress Report");
	}

	void HealthCheckService::scheduledExecute(void* arg)
	{
		DEBUG("%s"," Entering HealthCheckService::scheduledExecute(void* arg) ");
		cout << "Entering execute thread" << endl;
//		char* DN = reinterpret_cast<char*>(arg);
                char DN[256];
                memset(DN,0,256);
                strcpy(DN,(char*)arg);
		DEBUG("%s", "statement 1");
                cout<<"DN in strtng is :"<<DN<<endl;

		OmHandler immHandler;
		acs_hcs_global_ObjectImpl obj;
		ACS_CC_ReturnType returnCode;
		string executeArg(DN);
		DEBUG("%s", "statement 2");
		string scheduler = JOBSCHEDULER + executeArg;
		DEBUG("%s", "statement 3");
		string RDN;
		std::vector<ACS_APGCC_ImmAttribute *> attributes;
		 
		returnCode = immHandler.Init();
		
		if(returnCode != ACS_CC_SUCCESS)
		{
			DEBUG("%s","ERROR: init FAILURE!!! in execute");
			cout<<"ERROR: init FAILURE!!!\n";
			return;
		}
		int index;
		cout<<"scheduler: " << scheduler <<endl;
		
		/* Check if job can be executed*/

		/* Condition 1: High usage of CPU and memory on system*/
		
		try
		{
			//Fix for TR HV33576
      //In case if CPU or memory usage is high, exception is thrown but index is unknown as it is not calculated yet
      //To fix this problem, get the index value before verifying memory and CPU usages

      index = obj.getProgressReportIndex(scheduler);
      if(index == -1)
        return;

      Environment env;
      env.assertEnvSetHcjdef(executeArg);
      DEBUG("%s","CPU/Memory usage test passed")

                  /* All conditions met, now job execution starts*/
      /* Update the progress*/

      //Recalculating the index value. Because memory and CPU usage will take sometime around 10 seconds
      //In the mean time if any job is deleted, then the index value may change.
      index = obj.getProgressReportIndex(scheduler);
      if(index == -1)
        return;

			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::RUNNING, 
			                                   acs_hcs_progressReport_ObjectImpl::NOT_AVAILABLE, 
							   "", index, false);

			/* Evaluate the job name*/
			DEBUG("%s","Evaluating job name");
			string jobName;
			size_t found_equal, found_comma;
			found_equal = executeArg.find_first_of("=");
			if(found_equal != std::string::npos)
			{
				found_comma = executeArg.find_first_of(",", found_equal+1);
				if(found_comma != std::string::npos)
				{
					jobName = executeArg.substr(found_equal+1, found_comma - (found_equal+1));
				}
			}

			cout << "Jobname: " << jobName << endl;
			JobExecutor::Job job(jobName);
			job.setRuleSetIds("ALL");
			//get the categories from MML
			string categoryName = "";
			string target = "";
			string transferQueue = "";
			string jobToTrigger = "";
			int compressionEnable = 1;
			int suceessLogsEnable = 1 ;
			 
			ACS_APGCC_ImmAttribute catName;
			ACS_APGCC_ImmAttribute cpName;
			ACS_APGCC_ImmAttribute tqName;
			ACS_APGCC_ImmAttribute jobToTriggerName;
			ACS_APGCC_ImmAttribute compressionStatus;
			ACS_APGCC_ImmAttribute logsEnable;	
			
			catName.attrName      = categoriesAttr ;
			cpName.attrName       = targetAttr ;
			tqName.attrName       = transferQueueAttr;
			jobToTriggerName.attrName = jobToTriggerAttr;
			compressionStatus.attrName = compressionAttr;	
			logsEnable.attrName = successPrintoutsAttr;

			attributes.push_back(&catName);
			attributes.push_back(&cpName);
			attributes.push_back(&tqName);
			attributes.push_back(&jobToTriggerName);
			attributes.push_back(&compressionStatus);
			attributes.push_back(&logsEnable);
			
			string temp = "";	
			
			returnCode = immHandler.getAttribute(DN, attributes);
			if(returnCode == ACS_CC_FAILURE)
                	{
				 DEBUG("%s","GetAttribute failed in Job..Updating Progress Report");
				 index = obj.getProgressReportIndex(scheduler);
				 HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                            acs_hcs_progressReport_ObjectImpl::FAILURE, 
					                   	    "Internal Error", index, true);
				 return;	 									 
			}
			else
			{
				 DEBUG("%s","GetAttribute success in job..");
				 cout << "GetAttribute success" << endl;
				 if(attributes[0]->attrValues[0]  && attributes[0]->attrValuesNum)
				 {	
				 	categoryName = reinterpret_cast<char*>(attributes[0]->attrValues[0]);
					cout<<"Category is "<<categoryName<<endl;	
					DEBUG("%s","Category name updated");
				 }
				 else
				 {		
				 	cout<<"No categoryName to execute a job " << endl;
					DEBUG("%s","No Category to execute job");
				 }
				 if( attributes[1]->attrValues[0] && attributes[1]->attrValuesNum)
				 { 			
				 	target = reinterpret_cast<char*>(attributes[1]->attrValues[0]);
				 	cout<<"target"<<target<<endl;
					DEBUG("%s","target name updated");
				 }
				 else
				 {
					cout<<"No target name "<<endl;
					DEBUG("%s","No target name ");
				 }
			 	 if( attributes[2]->attrValues[0] && attributes[2]->attrValuesNum)  
				 {							
				 	transferQueue = reinterpret_cast<char*>(attributes[2]->attrValues[0]); 	
				 	cout<<"transfer queue"<<transferQueue<<endl;
					DEBUG("%s","DestinationURI name updated");
				 }
				 else
				 {
					cout<<"No transfer queue"<<endl;
					DEBUG("%s","No transfer queue name ");
				 }			
				 if( attributes[3]->attrValues[0] && attributes[3]->attrValuesNum)	
				 {	
				 	jobToTrigger = reinterpret_cast<char*>(attributes[3]->attrValues[0]);
				 	cout<<"jobToTrigger"<<jobToTrigger<<endl;
					DEBUG("%s","jobToTrigger updated");
				 }
				 else
				 {
					cout<<"No job to trigger"<<endl;
					DEBUG("%s","No jobToTirgger name ");;
				 }
				 					
				 compressionEnable = *(reinterpret_cast<int*>(attributes[4]->attrValues[0]));	
				 cout<<"compressionEnable"<<compressionEnable<<endl;
			       	 DEBUG("%s","Compression updated");
				 suceessLogsEnable = *(reinterpret_cast<int*>(attributes[5]->attrValues[0]));
				 DEBUG("%s","Success Logs updated");			
				 cout<<"successLogs"<<suceessLogsEnable<<endl;
				


			}
			
			DEBUG("%s","job.set methods called ");
			job.setRuleCats(categoryName);
			job.setTransferQueue(transferQueue);
			job.setCpName(target);
			job.setCompression(compressionEnable);
			job.setSuccessLogs(suceessLogsEnable);
			job.setjobToTrigger(jobToTrigger);
		
			JobExecutor::Singleton::get()->start(job,0,scheduler);
			AcsHcs::HealthCheckService::sameJob = EMPTY_STR;
			
			if(!diskFull && !folderDeleted )
			{
				DEBUG("%s","Job Execution done ");	
				cout<<"job execution done"<<endl;

				//Newly Added
				bool Waiting = obj.waitForSomeTime(10);//sleeping for 10 seconds, exit in case if thread is closed
				if(Waiting == false && jobScheduler::stopRequested)
					return;

				//ACE_OS::sleep(10);
				bool triggerJobFlag = isJobNeedToTrigger(jobToTrigger, categoryName);	

				if( jobName == jobToTrigger )
					triggerJobCount = triggerJobCount +1;

				DEBUG("%s","GetAttribute failed in Job..Updating Progress Report");
				jobScheduler::jobExecution  = true;
				index = obj.getProgressReportIndex(scheduler);
				// TR HS37486.. checking whether progress report exists or not before updating it
				if(AcsHcs::HealthCheckService::progressDeleted == false)
				{
					HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::FINISHED, 
						acs_hcs_progressReport_ObjectImpl::SUCCESS, 
						"Successfully Executed", index, true,acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo);
				}


				cout<<"job to trigger is : "<<jobToTrigger<<endl;
				//if(isJobNeedToTrigger(jobToTrigger, categoryName))
				if(triggerJobFlag == true && triggerJobCount < 2)
				{
					if(AcsHcs::HealthCheckService::progressDeleted == false)
						acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo +="job to trigger is triggered;";
					spawnExecutionThread(jobToTrigger,true);
				}
				else
					triggerJobCount = 0;

			}
			else
			{
				index = obj.getProgressReportIndex(scheduler);
				if(index == -1)
					return;
				DEBUG("%s","In else part of error");
				HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
								   acs_hcs_progressReport_ObjectImpl::FAILURE,
								   "Directory/File could not be created", index, false); 
			}


                        //Checking whether hard limit got crossed while executing by creating a temp directory
                        bool hardlimit = false;
                        try
                        {
                               Directory dirReports((Configuration::Singleton::get()->getReportsDirName())+"temp");
                               dirReports.create();
                        }
                        catch (const Directory::ExceptionIo& ex)
                        {
                        //      HealthCheckService::diskFull = true;
                                hardlimit = true;
                        }


                        if((hardlimit == true) && (diskFull == false))
                        {
				 // TR HS37486.. checking whether progress report exists or not before updating it
				if(AcsHcs::HealthCheckService::progressDeleted == false)
				{
	                                acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo += "Quota Limit crossed during execution;\n";

                               		HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::FINISHED,
                                                acs_hcs_progressReport_ObjectImpl::SUCCESS,
                                                "Successfully Executed", index, true, acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo);
				}

                        }

                        string cmdString1 = "rm -r " + (Configuration::Singleton::get()->getReportsDirName())+"temp"+  " " +"2>/dev/null";
                        system(cmdString1.c_str());
                        hardlimit = false;
			diskFull = false;	
			folderDeleted =false;		
			DEBUG("%s","Calling immHandler.Finalize() ");
			immHandler.Finalize();	
		}

		catch(const JobExecutor::ExceptionJobInProgress& ex) 
		{
			jobScheduler::jobExecution = false;
			syslog(LOG_INFO, "Got JobExecutor::ExceptionJobInProgress exception %s", ex.what());	
			index = obj.getProgressReportIndex(scheduler);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                   acs_hcs_progressReport_ObjectImpl::FAILURE, 
							  ANOTHERJOBINPRGRES, index, true);
		}
                catch(const JobExecutor::ExceptionJobExecution &ex)
                {
                        HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                                           acs_hcs_progressReport_ObjectImpl::FAILURE,
                                                           "Initial execution context failed", index, true);
                }

                catch(const JobExecutor::ExceptionInitialRuleFile &ex)
                {
                        HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                                           acs_hcs_progressReport_ObjectImpl::FAILURE,
                                                           "Initial Rules File not found", index, true);

                }
		
                catch (const JobExecutor::ExceptionDirectoryFileCreation &ex)
                {
			 DEBUG("%s","In catch1 part of error");
                        HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                              acs_hcs_progressReport_ObjectImpl::FAILURE,
                                              "Directory/File could not be created", index, true);
                }


		catch(...)
		{
			jobScheduler::jobExecution = false;
                	cout<<"caught exception"<<endl;
             		index = obj.getProgressReportIndex(scheduler);
             		if(index == -1)
             			return;
			 HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                                           acs_hcs_progressReport_ObjectImpl::FAILURE,
                                                           "Internal Error", index, true);	
		}

		 DEBUG("%s"," Leaving HealthCheckService::scheduledExecute(void* arg) ");
	}

	void HealthCheckService::execute(void* arg)
	{
		DEBUG("%s"," Entering HealthCheckService::execute(void* arg) ");
		cout << "Entering execute thread" << endl;
		char DN[256];
		memset(DN,0,256);
		//DN = reinterpret_cast<char*>(arg);
		strcpy(DN,(char*)arg);
		cout<<"DN in strtng is :"<<DN<<endl;	
		OmHandler immHandler;
		acs_hcs_global_ObjectImpl obj;
		ACS_CC_ReturnType returnCode;
		string executeArg(DN);
		string RDN;
		std::vector<ACS_APGCC_ImmAttribute *> attributes;
		 
		returnCode = immHandler.Init();
		
		if(returnCode != ACS_CC_SUCCESS)
		{
			DEBUG("%s","ERROR: init FAILURE!!! in execute");
			cout<<"ERROR: init FAILURE!!!\n";
			return;
		}
		int index=0;
		cout<<"executeArg: "<<executeArg<<endl;
	

		/* Check if job can be executed*/

		/* Condition 1: High usage of CPU and memory on system*/
		
		try
		{	
			//Fix for TR HV33576
			//In case if CPU or memory usage is high, exception is thrown but index is unknown as it is not calculated yet
			//To fix this problem, get the index value before verifying memory and CPU usages

			index = obj.getProgressReportIndex(executeArg);
			if(index == -1)
				return;

			Environment env;
			env.assertEnvSetHcjdef(executeArg);
			DEBUG("%s","CPU/Memory usage test passed")
								 	
               		/* All conditions met, now job execution starts*/
			/* Update the progress*/
		
			//Recalculating the index value. Because memory and CPU usage will take sometime around 10 seconds
			//In the mean time if any job is deleted, then the index value may change.
			index = obj.getProgressReportIndex(executeArg);
			if(index == -1)
				return;

			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::RUNNING, 
			                                   acs_hcs_progressReport_ObjectImpl::NOT_AVAILABLE, 
							   "", index, false);

			/* Evaluate the job name*/
			DEBUG("%s","Evaluating job name");
			string jobName;
			size_t found_equal, found_comma;
			found_equal = executeArg.find_first_of("=");
			if(found_equal != std::string::npos)
			{
				found_comma = executeArg.find_first_of(",", found_equal+1);
				if(found_comma != std::string::npos)
				{
					jobName = executeArg.substr(found_equal+1, found_comma - (found_equal+1));
				}
			}

			cout << "Jobname: " << jobName << endl;
			JobExecutor::Job job(jobName);
			job.setRuleSetIds("ALL");
			//get the categories from MML
			string categoryName = "";
			string target = "";
			string transferQueue = "";
			string jobToTrigger = "";
			int compressionEnable = 1;
			int suceessLogsEnable = 1 ;
			 
			ACS_APGCC_ImmAttribute catName;
			ACS_APGCC_ImmAttribute cpName;
			ACS_APGCC_ImmAttribute tqName;
			ACS_APGCC_ImmAttribute jobToTriggerName;
			ACS_APGCC_ImmAttribute compressionStatus;
			ACS_APGCC_ImmAttribute logsEnable;	
			
			catName.attrName      = categoriesAttr ;
			cpName.attrName       = targetAttr ;
			tqName.attrName       = transferQueueAttr;
			jobToTriggerName.attrName = jobToTriggerAttr;
			compressionStatus.attrName = compressionAttr;	
			logsEnable.attrName = successPrintoutsAttr;

			attributes.push_back(&catName);
			attributes.push_back(&cpName);
			attributes.push_back(&tqName);
			attributes.push_back(&jobToTriggerName);
			attributes.push_back(&compressionStatus);
			attributes.push_back(&logsEnable);
			
			string temp = "";	
			cout<<"In excute DN is: "<<DN<<endl;
			returnCode = immHandler.getAttribute(DN, attributes);
			cout<<"return code in execute is: "<<returnCode<<endl;
			if(returnCode == ACS_CC_FAILURE)
                	{
				cout<<"IMM CreateObject failure"<<std::endl;
                                cout<<"getInternalLastError :"<< immHandler.getInternalLastError()<<endl;
                                cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl; 
				DEBUG("%s","GetAttribute failed in Job..Updating Progress Report");
				index = obj.getProgressReportIndex(executeArg);
				HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                            acs_hcs_progressReport_ObjectImpl::FAILURE, 
					                   	    "Internal Error", index, true);
				 return;	 									 
			}
			else
			{
				 DEBUG("%s","GetAttribute success in job..");
				 cout << "GetAttribute success" << endl;
				 if(attributes[0]->attrValues[0]  && attributes[0]->attrValuesNum)
				 {	
				 	categoryName = reinterpret_cast<char*>(attributes[0]->attrValues[0]);
					if( strcmp(categoryName.c_str(),(const char*)ALL_CATEGORIES ) == 0)
					{
						categoryName = obj.getAllCategories();
					}
					cout<<"Category is "<<categoryName<<endl;	
					DEBUG("%s","Category name updated");
				 }
				 else
				 {		
				 	cout<<"No categoryName to execute a job " << endl;
					DEBUG("%s","No Category to execute job");
				 }
				 if( attributes[1]->attrValues[0] && attributes[1]->attrValuesNum)
				 { 			
				 	target = reinterpret_cast<char*>(attributes[1]->attrValues[0]);
				 	cout<<"target"<<target<<endl;
					DEBUG("%s","target name updated");
				 }
				 else
				 {
					cout<<"No target name "<<endl;
					DEBUG("%s","No target name ");
				 }
			 	 if( attributes[2]->attrValues[0] && attributes[2]->attrValuesNum)  
				 {							
				 	transferQueue = reinterpret_cast<char*>(attributes[2]->attrValues[0]); 	
				 	cout<<"transfer queue"<<transferQueue<<endl;
					DEBUG("%s","DestinationURI name updated");
				 }
				 else
				 {
					cout<<"No transfer queue"<<endl;
					DEBUG("%s","No transfer queue name ");
				 }			
				 if( attributes[3]->attrValues[0] && attributes[3]->attrValuesNum)	
				 {	
				 	jobToTrigger = reinterpret_cast<char*>(attributes[3]->attrValues[0]);
				 	cout<<"jobToTrigger"<<jobToTrigger<<endl;
					DEBUG("%s","jobToTrigger updated");
				 }
				 else
				 {
					cout<<"No job to trigger"<<endl;
					DEBUG("%s","No jobToTirgger name ");;
				 }
				 					
				 compressionEnable = *(reinterpret_cast<int*>(attributes[4]->attrValues[0]));	
				 cout<<"compressionEnable"<<compressionEnable<<endl;
			       	 DEBUG("%s","Compression updated");
				 suceessLogsEnable = *(reinterpret_cast<int*>(attributes[5]->attrValues[0]));
				 DEBUG("%s","Success Logs updated");			
				 cout<<"successLogs"<<suceessLogsEnable<<endl;
			}
			
			DEBUG("%s","job.set methods called ");
			job.setRuleCats(categoryName);
			job.setTransferQueue(transferQueue);
			job.setCpName(target);
			job.setCompression(compressionEnable);
			job.setSuccessLogs(suceessLogsEnable);
			job.setjobToTrigger(jobToTrigger);
		
			JobExecutor::Singleton::get()->start(job,0,executeArg);
			AcsHcs::HealthCheckService::sameJob = EMPTY_STR;
			
			if(!diskFull && !folderDeleted )
			{
				DEBUG("%s","Job Execution done ");	
				cout<<"job execution done"<<endl;
				
				bool Waiting = obj.waitForSomeTime(10);//sleeping for 10 seconds, exit in case if thread is closed
                                if(Waiting == false && jobScheduler::stopRequested)
                                        return;
				//ACE_OS::sleep(10);
				bool triggerJobFlag = isJobNeedToTrigger(jobToTrigger, categoryName);

				if( jobName == jobToTrigger )
					triggerJobCount = triggerJobCount +1;

				DEBUG("%s","GetAttribute failed in Job..Updating Progress Report");
				index = obj.getProgressReportIndex(executeArg);
				 // TR HS37486.. checking whether progress report exists or not before updating it
				if(AcsHcs::HealthCheckService::progressDeleted == false)
				{
					HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::FINISHED, 
						acs_hcs_progressReport_ObjectImpl::SUCCESS, 
						"Successfully Executed", index, true, acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo);
				}

				cout<<"job to trigger is : "<<jobToTrigger<<endl;
				//if(isJobNeedToTrigger(jobToTrigger, categoryName))
				if(triggerJobFlag == true && triggerJobCount < 2)
				{
					if(AcsHcs::HealthCheckService::progressDeleted == false)
						acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo +="job to trigger is triggered;\n";
					spawnExecutionThread(jobToTrigger);
				}
				else
					triggerJobCount = 0;

			}
                        else
                        {
                                index = obj.getProgressReportIndex(executeArg);
                                if(index == -1)
                                        return;
				 DEBUG("%s","In else2 part of error");
                                HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                                                   acs_hcs_progressReport_ObjectImpl::FAILURE,
                                                                   "Directory/File could not be created", index, false);
                        }

			//Checking whether hard limit got crossed while executing by creating a temp directory
			bool hardlimit = false;
			try
                	{
        	               Directory dirReports((Configuration::Singleton::get()->getReportsDirName())+"temp");
                	       dirReports.create();
                	}
	                catch (const Directory::ExceptionIo& ex)
        	        {
			//	HealthCheckService::diskFull = true;
				hardlimit = true;
                	}


                        if((hardlimit == true) && (diskFull == false))
                        {
				 // TR HS37486.. checking whether progress report exists or not before updating it
				if(AcsHcs::HealthCheckService::progressDeleted == false)
				{
					acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo += "Quota Limit crossed during execution;\n";

                        	       HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::FINISHED,
                                                acs_hcs_progressReport_ObjectImpl::SUCCESS,
                                                "Successfully Executed", index, true, acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo);
				}

                        }

			string cmdString1 = "rm -r " + (Configuration::Singleton::get()->getReportsDirName())+"temp"+  " " +"2>/dev/null";
                        system(cmdString1.c_str());
			hardlimit = false;
			diskFull = false;
			folderDeleted = false;
			DEBUG("%s","Calling immHandler.Finalize() ");
			immHandler.Finalize();	
		}

		catch(const JobExecutor::ExceptionJobInProgress& ex) 
		{
			syslog(LOG_INFO, "Got JobExecutor::ExceptionJobInProgress exception %s", ex.what());	
			index = obj.getProgressReportIndex(executeArg);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                   acs_hcs_progressReport_ObjectImpl::FAILURE, 
							   ANOTHERJOBINPRGRES, index, true);
		}
		catch(const JobExecutor::ExceptionJobExecution &ex)
		{
			index = obj.getProgressReportIndex(executeArg);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
							   acs_hcs_progressReport_ObjectImpl::FAILURE,
							   "Initial execution context failed", index, true);	
		}
		
		catch(const JobExecutor::ExceptionInitialRuleFile &ex)
		{
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                                           acs_hcs_progressReport_ObjectImpl::FAILURE,
                                                           "Initial Rules File not found", index, true);
		
		}
		catch (const JobExecutor::ExceptionDirectoryFileCreation &ex)
		{
			 DEBUG("%s","In catch2 part of error");
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                              acs_hcs_progressReport_ObjectImpl::FAILURE,
                                              "Directory/File could not be created", index, true);	
		}
		catch(...)
		{
			cout<<"caught exception"<<endl;
			index = obj.getProgressReportIndex(executeArg);
			if(index == -1)
				return;
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                                           acs_hcs_progressReport_ObjectImpl::FAILURE,
                                                           "Internal Error", index, true);		
		}
		DEBUG("%s"," Leaving HealthCheckService::execute(void* arg) ");
	}

	void HealthCheckService::unload(void* arg)
	{
		DEBUG("%s","Entering HealthCheckService::unload(void* arg)");
		cout << "UNLOAD thread" << endl;
		acs_hcs_global_ObjectImpl obj;
		int index;
		//char* DN = reinterpret_cast<char*>(arg);
		char DN[256];
                memset(DN,0,256);
		strcpy(DN,(char*)arg);	
		string temp_dn = DN;	
		cout<<"provided unload parameter is "<<temp_dn<<endl;
		bool isValid = obj.isParameterValid(DN, INUSEDN);
		cout<<"valus of isValid is : "<<isValid<<endl;
                if( isValid == false ) //provided parameter for load is not valid
                {
			index = obj.getProgressReportIndex((string)INUSEDN);
	                if(index == -1)
        	                return;

			 string currentTime = obj.getCurrentTime();
			 acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionStarted = currentTime;	
                         HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                        acs_hcs_progressReport_ObjectImpl::FAILURE,
                                        "Invalid parameter, cannot be unloaded", index, true);
			
                         return;
                }


		//preparing dn for unload operation
                string unloadParam(DN);
                size_t equal_pos = unloadParam.find("=");
		size_t len = unloadParam.size();
                unloadParam = unloadParam.substr(equal_pos+1,len-12);
                unloadParam = (string)"ruleSetFileId=" + unloadParam + (string)COMMA_STR + (string)INUSEDN ;
               // DN = const_cast<char *>(unloadParam.c_str());
        	strncpy(DN,unloadParam.c_str(),256);

		OmHandler immHandler;
		ACS_CC_ReturnType returnCode;
		char fileValue[64] = "";
		string unloadArg(DN);
		cout<<"dn in unload is: "<<unloadArg<<endl;
		string RDN;
		std::vector<ACS_APGCC_ImmAttribute *> attributes;
		ACS_APGCC_ImmAttribute fileName;
		size_t found_equal, found_comma;
		
		returnCode = immHandler.Init();
		if(returnCode != ACS_CC_SUCCESS)
		{
			DEBUG("%s","ERROR: init FAILURE!!! in unload ");
			cout<<"ERROR: init FAILURE!!!\n";
			return;
		}

                string object = DN;
                string parentObject;

                found_comma = unloadArg.find_first_of(",");
                if(found_comma != std::string::npos)
                {
                        parentObject = unloadArg.substr(found_comma+1, unloadArg.size()-1);
                }

 
		index = obj.getProgressReportIndex(parentObject);
		if(index == -1)
			return;

		if(acs_hcs_brfimplementer::getBrfStatus())
		{
			string currentTime = obj.getCurrentTime();
                        acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionStarted = currentTime;

			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
		    		    acs_hcs_progressReport_ObjectImpl::FAILURE,
		                    "APG Backup in progress, cannot be unloaded", index, true);
		        return;
		    		
		}

		HealthCheckService::isUnloadOngoing = true;

		cout << "In unload permit flag=======" << HealthCheckService::isUnloadOngoing << endl;
	
                /* Update the progress*/
		HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::RUNNING, 
			                                   acs_hcs_progressReport_ObjectImpl::NOT_AVAILABLE, 
							   "", index, false);
		string readyDN;
                readyDN="readyToUseId=1,ruleFileManagerId=1,HealthCheckhealthCheckMId=1";
		DEBUG("%s","Evaluating RDN in unload");
		/* Evaluate the RDN*/
		found_equal = 0;
 		found_comma = 0;
		found_equal = unloadArg.find_first_of("=");
		if(found_equal != std::string::npos)
		{
			found_comma = unloadArg.find_first_of(",", found_equal+1);
			if(found_comma != std::string::npos)
		  	{
		       		RDN = unloadArg.substr(found_equal+1, found_comma - (found_equal+1));
	                }
		}

                
		/* Get the XML file name*/
		DEBUG("%s","Retrieving XML file name");
                fileName.attrName = "fileName";
                attributes.push_back(&fileName);

                returnCode = immHandler.getAttribute(DN, attributes);
                if(returnCode == ACS_CC_FAILURE)
                {
			DEBUG("%s","GetAttribute failed in Unload");
                        cout << "GetAttribute failed" << immHandler.getInternalLastErrorText() << endl;
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                   acs_hcs_progressReport_ObjectImpl::FAILURE, 
							   "Internal Error", index, true);
			DEBUG("%s","return after GetAttribute failed in Unload ");
			HealthCheckService::isUnloadOngoing = false;
                        return;
                }
                else
                {
			DEBUG("%s","GetAttribute success in Unload");
                        cout << "GetAttribute success" << endl;
                        strcpy(fileValue, "");
			string temp = (const char*)attributes[0]->attrValues[0];
                        strcpy(fileValue, temp.c_str());
                        cout << "FileName: " << fileValue << endl;
                }

                /* Get the map of categories and rules*/

                char tempPath[256];
                std::vector<string>::const_iterator it;
                AcsHcs::DocumentXml::MultiMap theMap;
                std::vector<std::string> CategoryList;
                multimap<string, AcsHcs::DocumentXml::rule>::const_iterator pos,end;
                int CategoryCount = 0;

                strcpy(tempPath, "/cluster/storage/system/config/acs_hc/");
                strcat(tempPath, fileValue);

                string ruleFilePath(tempPath);
                try
                {
                        AcsHcs::DocumentXml doc(ruleFilePath);
                        /* Update the map with categories and rules*/
			 DEBUG("%s","Update the map with categories and rules");
                        doc.updateCategoriesAndRules(theMap);
                }
                catch (...)
                {
			DEBUG("%s","ERROR while loading : Error reading XML file");
                        cout << "ERROR while loading : Error reading XML file" << endl;
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                   acs_hcs_progressReport_ObjectImpl::FAILURE, 
							   "Error reading XML file", index, true);
			HealthCheckService::isUnloadOngoing = false;
                        return;
                }

                /* Get the list of categories from the map*/
                for(pos = theMap.begin(), end = theMap.end(); pos != end; pos = theMap.upper_bound(pos->first))
                {
                        CategoryList.push_back(pos->first);
                }
                CategoryCount = CategoryList.size();

		/* Check if unload can be performed*/
		
		/* Condition 1: Same RSF object must not be under ReadyToUse*/
		std::vector<std::string> RTU;
		returnCode = immHandler.getChildren(READYTOUSEDN, ACS_APGCC_SUBTREE, &RTU);
		if(returnCode != ACS_CC_SUCCESS)
		{
			DEBUG("%s","getChildren failed in Unload");
			cout << "getChildren failed" << immHandler.getInternalLastErrorText() << endl;
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                   acs_hcs_progressReport_ObjectImpl::FAILURE, 
							   "Internal error", index, true);
			DEBUG("%s"," return after getchildren fail in unload");
			HealthCheckService::isUnloadOngoing = false;
			return;
		}
		else
		{
			char fileNameRTU[256]={0};
				string ruleSetRDN = "ruleSetFileId=";
			ruleSetRDN = ruleSetRDN + RDN;

			std::vector<std::string> childrenRTU;
			for(unsigned int i = 0; i < RTU.size(); i++)
			{
				if(RTU[i].find("ruleSetFileId") != std::string::npos)
				{
					childrenRTU.push_back(RTU[i]);
				}
			}

			for(unsigned int i = 0; i < childrenRTU.size(); i++)
			{
				cout<<"ruleset rdn is "<<ruleSetRDN<<endl;
				cout<<"childrenRTU is "<<childrenRTU[i]<<endl;
				//acs_hcs_global_ObjectImpl obj;	
				string temp = obj.parseFirstString(childrenRTU[i]);
				cout<<"After Parsing"<<temp<<endl;
				//if(childrenRTU[i].find(ruleSetRDN) != string::npos) // RDN found under RTU
				if(strcmp(temp.c_str(),ruleSetRDN.c_str())==0)
				{
					DEBUG("%s","ERROR unloading: Cannot be unloaded");
					cout << "ERROR unloading: Cannot be unloaded" << endl;
					index = obj.getProgressReportIndex(parentObject);
					HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
									   acs_hcs_progressReport_ObjectImpl::FAILURE, 
		                                                           SAMERID_UNLOAD_ERR_PR, index, true,RULESETFILE_ALREADY_EXIST);
					DEBUG("%s"," after ruleSetFileId != std::string::npos");
					HealthCheckService::isUnloadOngoing = false;
					return;
				}

				returnCode = immHandler.getAttribute(childrenRTU[i].c_str(), attributes);	
	                        strcpy(fileNameRTU, "");
	                        strcpy(fileNameRTU, (const char*)attributes[0]->attrValues[0]);
				if(!strcmp(fileNameRTU, fileValue)) // Same file name found for an instance under RTU
				{
					DEBUG("%s","In Same file name and different RSID");
					cout << "Same file name and different RSID" << endl;
					unload_flag = true;
				}

			}
		}




                /* Condition 3: No job must be ongoing*/


              	//acs_hcs_global_ObjectImpl obj;
                int jobStatus = obj.getProgressReportRunningState();
                if(jobStatus == STATE_RUNNING)
                {
				
                                DEBUG("%s","ERROR unloading, Job ongoing");
                                cout << "ERROR unloading, Job ongoing" << endl;
				index = obj.getProgressReportIndex(parentObject);
                                HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                                acs_hcs_progressReport_ObjectImpl::FAILURE,
                                                JOB_ONGOING, index, true);
                                DEBUG("%s"," return after ERROR unloading, Job ongoing");
				HealthCheckService::isUnloadOngoing = false;
                                return;
                }
                cout << "No job is ongoing" << endl;


		/* Condition 2: No job must have references to categories to be unloaded*/
		
		std::vector<std::string> jobInstances;
		char attrCat[] = "categories";
		ACS_APGCC_ImmAttribute categories;
		categories.attrName = attrCat;
		std::vector<ACS_APGCC_ImmAttribute*> jobVector;
		jobVector.push_back(&categories);
		returnCode = immHandler.getClassInstances(ACS_HC_JOBBASED_CLASS_NAME, jobInstances);
		if(returnCode != ACS_CC_SUCCESS)
		{
			if(immHandler.getInternalLastError() != -41)
			{
				DEBUG("%s","getClassInstances failed ");
				cout << "getClassInstances failed" << immHandler.getInternalLastError() << endl;
				index = obj.getProgressReportIndex(parentObject);
				HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
						acs_hcs_progressReport_ObjectImpl::FAILURE, 
						"Internal Error", index, true);
				DEBUG("%s","return after getClassInstances failed ");
				//HealthCheckService::isUnloadOngoing = false;
				return;
			}
		}
		else
		{
			DEBUG("%s","All job instances retrieved ");		
			cout << "All job instances retrieved" << endl;
			for(unsigned int i = 0; i < jobInstances.size(); i++)
			{
				returnCode = immHandler.getAttribute(jobInstances[i].c_str(), jobVector);
				if(returnCode == ACS_CC_FAILURE)
				{
					DEBUG("%s"," Categories could not be retrieved");
					cout << "Categories could not be retrieved" << endl;
					index = obj.getProgressReportIndex(parentObject);
					HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
							acs_hcs_progressReport_ObjectImpl::FAILURE, 
							"Internal Error", index, true);
					DEBUG("%s","after categories not retrieved ");
					HealthCheckService::isUnloadOngoing = false;
					return;
				}
				else
				{
					cout << "Category of the job: " << (const char*)jobVector[0]->attrValues[0] << endl;
					string categoriesTobeSearched = (const char*)jobVector[0]->attrValues[0];
					for(int j = 0; j < CategoryCount; j++)					
					{
						if((categoriesTobeSearched.find(CategoryList[j]) != string::npos) || (categoriesTobeSearched == "ALL"))
						{
							ACS_CC_ImmParameter ruleReferences;
							ruleReferences.attrName = (char*)ruleReferenceAttr;
							string catDN = "categoryId=" + CategoryList[j] + ","+ ACS_HC_ID;
							returnCode = immHandler.getAttribute(catDN.c_str(), &ruleReferences);
							if(returnCode != ACS_CC_SUCCESS)
							{
								DEBUG("%s"," Failed in retrieving category rule refererences");
								cout << "Failed in retreiving category rule references: " << immHandler.getInternalLastError() << endl;
								index = obj.getProgressReportIndex(parentObject);
								HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
										acs_hcs_progressReport_ObjectImpl::FAILURE, 
										"Internal Error", index, false);
								DEBUG("%s","return after  Failed in retrieving category rule references ");
								HealthCheckService::isUnloadOngoing = false;
								return;
							}
							else
							{	DEBUG("%s"," Category referenced by a job");	
								cout<<" Category referenced by a job "<<endl;
								cout << "Category Reference : " << (const char*)ruleReferences.attrValues[0] << endl;	
								char ruleRef[256]={0};
								strcpy(ruleRef, "");
								cout<<"strcpy(ruleRef, "") is"<<ruleRef<<endl;
								string temp_ruleRef = (const char*)ruleReferences.attrValues[0];
								cout<<"temp_ruleRef =  "<<temp_ruleRef<<endl;
								strcpy(ruleRef, temp_ruleRef.c_str());
								cout<<"  strcpy(ruleRef, temp_ruleRef.c_str()) is" <<ruleRef<<endl;
								cout<<"RDN.c_str() is "<<RDN.c_str()<<endl;
								cout<<"strcmp(ruleRef, RDN.c_str()) is"<<strcmp(ruleRef, RDN.c_str())<<endl;
								//if(!strcmp(ruleRef, RDN.c_str()))
								size_t rdn_pos = temp_ruleRef.find(RDN.c_str());
								if (rdn_pos != std::string::npos)
								{
									DEBUG("%s"," ERROR unloading : Categories to be unloaded are referenced in the job");
									cout << "ERROR unloading : Categories to be unloaded are referenced in the job " << jobInstances[i] << endl;
									index = obj.getProgressReportIndex(parentObject);
									HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
											acs_hcs_progressReport_ObjectImpl::FAILURE, 
											CATEGORYUSED_UNLOAD_ERR_PR, index, true);
									DEBUG("%s"," return after categories referenced in a job");
									HealthCheckService::isUnloadOngoing = false;
									return;
								}
							}
						}
					}
				}
			}
		}

		DEBUG("%s","No references of category in jobs");
		cout << "No references of category in jobs" << endl;
		DEBUG("%s","Entering no job must be ongoing");
		/* All set, now get the instances of rules and their references*/
		char *classNamep =const_cast<char*>(ACS_HC_RULE_CLASS_NAME);
		std::vector<std::string> p_dnList;
      
		returnCode = immHandler.getClassInstances(classNamep, p_dnList);
		if(returnCode != ACS_CC_SUCCESS)
		{	
			DEBUG("%s","Method OmHandler::getCLassInstances Failure!!");
			cout << "Method OmHandler::getCLassInstances Failure!!!" << endl;
			cout << "ERROR MESSAGE: " << immHandler.getInternalLastError() << endl;
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			                                   acs_hcs_progressReport_ObjectImpl::FAILURE, 
							   "Internal Error", index, true);
			DEBUG("%s","return after Method OmHandler::getCLassInstances Failure!!");
			HealthCheckService::isUnloadOngoing = false;
			return;
		}
	
		char attrRef[] = ruleReferenceAttr;
		string ruleToTrim = "";
		ACS_APGCC_ImmAttribute ruleReferences;
		ruleReferences.attrName = attrRef;
		std::vector<ACS_APGCC_ImmAttribute*> ruleVector;
		ruleVector.push_back(&ruleReferences);
		for(unsigned int i = 0; i < p_dnList.size(); i++)
		{
			//cout << "Rule: " << p_dnList[i].c_str() << endl;
			ruleToTrim = "";
			returnCode = immHandler.getAttribute(p_dnList[i].c_str(), ruleVector);
			if(returnCode == ACS_CC_FAILURE)
			{
				DEBUG("%s"," reference couldn't be retieved");
				cout << "Reference could not be retrieved" << endl;
				index = obj.getProgressReportIndex(parentObject);
				HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			        	                           acs_hcs_progressReport_ObjectImpl::FAILURE, 
								   "Internal Error", index, true);
				DEBUG("%s","return after reference not retireved ");
				return;
			}
			else
			{	
				DEBUG("%s"," Retireved Rule Reference");
			//	cout << "Retrieved rule reference" << (const char*)ruleVector[0]->attrValues[0] << endl;
				ruleToTrim = (const char*)ruleVector[0]->attrValues[0];
				trimRuleReference(ruleToTrim, RDN);
			}
			if(ruleToTrim.size() != 0)
			{
				ACS_CC_ImmParameter refToModify;
				refToModify.attrName = attrRef;
				refToModify.attrType = ATTR_STRINGT;
				refToModify.attrValuesNum = 1;
				char ruleRefValue[256] = "";
				strcpy(ruleRefValue, ruleToTrim.c_str());
				void* ruleReferencesValue[1] = {reinterpret_cast<void*>(ruleRefValue)};
				refToModify.attrValues = ruleReferencesValue;
				
				/* Update reference of rule*/
				returnCode = immHandler.modifyAttribute(p_dnList[i].c_str(), &refToModify);
				if(returnCode == ACS_CC_FAILURE)
				{
					DEBUG("%s"," Modifying rule ref fail!");
					cout << "Modify of rule Ref failed" << endl;
					index = obj.getProgressReportIndex(parentObject);
					HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
							                   acs_hcs_progressReport_ObjectImpl::FAILURE, 
  									   "Internal Error", index, true);
					DEBUG("%s"," return after modify rule ref failed");
					HealthCheckService::isUnloadOngoing = false;
					return;
	
			}
				else
				{
					DEBUG("%s"," rule ref updated");
				//	cout << "Rule ref updated" << endl;
				}
			}
			else
			{
				DEBUG("%s","returnCode = immHandler.deleteObject(p_dnList[i].c_str()) ");
				returnCode = immHandler.deleteObject(p_dnList[i].c_str());
			}
		}
		DEBUG("%s","Checking if categories are empty ");
		/* Check if categories are empty. If yes, remove them*/
                char *classNameCat =const_cast<char*>(ACS_HC_CATEGORY_CLASS_NAME);
                std::vector<std::string> catList;

                returnCode = immHandler.getClassInstances(classNameCat, catList);
                if(returnCode != ACS_CC_SUCCESS)
                {
			DEBUG("%s"," Method OmHandler::getCLassInstances Failure!!!");
	                cout << "Method OmHandler::getCLassInstances Failure!!!" << endl;
                        cout << "ERROR MESSAGE: " << immHandler.getInternalLastError() << endl;
			index = obj.getProgressReportIndex(parentObject);	
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
							acs_hcs_progressReport_ObjectImpl::FAILURE, 
							"Internal Error", index, true);
			return;
                }
		else
		{
			for(unsigned int i = 0; i < catList.size(); i++)
			{
				std::vector<std::string> catChildren;
				returnCode = immHandler.getChildren(catList[i].c_str(), ACS_APGCC_SUBTREE, &catChildren);
				if(returnCode != ACS_CC_SUCCESS)
				{
					DEBUG("%s"," getChildren failed:");
					cout << "getChildren failed: " << immHandler.getInternalLastError() << endl;
					index = obj.getProgressReportIndex(parentObject);
					HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
									acs_hcs_progressReport_ObjectImpl::FAILURE, 
									"Internal Error", index, true);
					DEBUG("%s","return after get children failed ");
					HealthCheckService::isUnloadOngoing = false;
					return;
				}
				else
				{
					if(catChildren.size() == 0) /* Empty category*/
					{
						DEBUG("%s","Empty Category ");
						returnCode = immHandler.deleteObject(catList[i].c_str());
					}
					else /* Categories references to RSID must be updated*/
					{
						DEBUG("%s","  Categories references to RSID must be updated");
						ruleToTrim = "";
						returnCode = immHandler.getAttribute(catList[i].c_str(), ruleVector);
						if(returnCode == ACS_CC_FAILURE)
						{
							DEBUG("%s"," Reference could not be retirved");
							cout << "Reference could not be retrieved" << endl;
							index = obj.getProgressReportIndex(parentObject);
							HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
											acs_hcs_progressReport_ObjectImpl::FAILURE, 
											"Internal Error", index, true);
							DEBUG("%s"," return after Reference could not be retirved");
							HealthCheckService::isUnloadOngoing = false;
							return;
						}
						else
						{
							DEBUG("%s"," Reference  retirved");
			                                //cout << "Retrieved rule reference" << (const char*)ruleVector[0]->attrValues[0] << endl;
			                                ruleToTrim = (const char*)ruleVector[0]->attrValues[0];
                        				trimRuleReference(ruleToTrim, RDN);
							if(ruleToTrim.size() != 0)
							{
								ACS_CC_ImmParameter refToModify;
								refToModify.attrName = attrRef;
								refToModify.attrType = ATTR_STRINGT;
								refToModify.attrValuesNum = 1;
								char ruleRefValue[256] = "";
								strcpy(ruleRefValue, ruleToTrim.c_str());
								void* ruleReferencesValue[1] = {reinterpret_cast<void*>(ruleRefValue)};
								refToModify.attrValues = ruleReferencesValue;

								/* Update reference of rule*/
								returnCode = immHandler.modifyAttribute(catList[i].c_str(), &refToModify);
								if(returnCode == ACS_CC_FAILURE)
								{
									DEBUG("%s"," Modify of rule Reference failed");
									cout << "Modify of rule Ref failed" << endl;
									index = obj.getProgressReportIndex(parentObject);
									HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
											acs_hcs_progressReport_ObjectImpl::FAILURE, 
											"Internal Error", index, true);
									DEBUG("%s"," return after modify of rule Reference failed");
									HealthCheckService::isUnloadOngoing = false;
									return;
								}
								else
								{
									DEBUG("%s"," category Reference updated");
									//cout << "category ref updated" << endl;
								}
							}
							else
							{	
								 DEBUG("%s","immHandler.deleteObject(p_dnList[i].c_st        r())");
								returnCode = immHandler.deleteObject(p_dnList[i].c_str());
							}

						}
						
					}
				}
			}
		}
		
		DEBUG("%s"," Deletion of object under InUse");
		/* Delete the object under InUse*/
		returnCode = immHandler.deleteObject(unloadArg.c_str());
		cout<<"This is" <<unloadArg.c_str()<<endl;
		
		
		
		if(returnCode == ACS_CC_FAILURE)
		{
			DEBUG("%s"," Deletion of object under InUse failed");
			cout << "Deletion under inuse failed " << DN << endl;
			cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastError()<<endl;	
			index = obj.getProgressReportIndex(parentObject);

			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
							acs_hcs_progressReport_ObjectImpl::FAILURE, 
							"Internal Error", index, true);
			DEBUG("%s"," return after Deletion of object under InUse failed");
			return;
		}		
		DEBUG("%s"," Placing ruleset file back in NBI");
                /* Move the rule set file from internal path to NBI path*/
                string ruleFile(fileValue);
		string completePath = NBI_RULES;
                string internalPath = INTERNAL_PATH + ruleFile;
                cout << "source: " << internalPath << endl;
                cout << "destination: " <<  completePath << endl;


		if(unload_flag == true)
		{
			string fileName = ruleFile.substr(0,ruleFile.size()-4);
			cout<<"fileName = "<<fileName<<endl;
			time_t rawTime = time(0);
                        struct tm formattedTime;
                        char currentTime[20];
	                formattedTime = *localtime(&rawTime);
                        sprintf(currentTime, "%d%d%d", formattedTime.tm_year+1900,formattedTime.tm_mon+1, formattedTime.tm_mday);
                        string timestamp(currentTime);
                        string command = "mv " + internalPath + " " + completePath+fileName+"_"+timestamp+".xml";
                        cout << "command: " << command.c_str() << endl;
                       system(command.c_str());

		}		
		else
		{
			string command = "mv " + internalPath + " " + completePath;
			cout << "command: " << command.c_str() << endl;
			system(command.c_str());
		}
		unload_flag =false;
		
                /* Update the progress*/
		index = obj.getProgressReportIndex(parentObject);	
		HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::FINISHED, 
				acs_hcs_progressReport_ObjectImpl::SUCCESS, 
				"Successfully unloaded", index, true);
		immHandler.Finalize();

		HealthCheckService::isUnloadOngoing = false;
		cout << "After load permit flag=======" << HealthCheckService::isUnloadOngoing << endl;
		
        	DEBUG("%s"," Exiting HealthCheckService::unload(void* arg)!!");
	}
	
	void HealthCheckService::trimRuleReference(string &ruleToTrim, string RDN)
	{
		DEBUG("%s"," Entering HealthCheckService::trimRuleReference");
		string pattern1 = "," + RDN + ",";
		string pattern2 = "," + RDN;
		string pattern3 = RDN + ",";
		size_t pos;
	/*	cout << "ruleToTrim: " << ruleToTrim << endl;
		cout << "pattern1: " << pattern1 << endl;
		cout << "patern2: " << pattern2 << endl;
		cout << "pattern3: " << pattern3 << endl;
	*/
		pos = ruleToTrim.find(pattern1);
		if(pos != string::npos)
		{
			ruleToTrim.replace(pos, pattern1.size(), ",");
			//cout << "Rule trimmed: " << ruleToTrim << " at "<< pos << endl;
		}
		else
		{
			pos = ruleToTrim.find(pattern2);
			if(pos != string::npos)
			{
				ruleToTrim.replace(pos, pattern2.size(), "");
			//	cout << "Rule trimmed: " << ruleToTrim << " at "<< pos << endl;
			}
			else
			{
				pos = ruleToTrim.find(pattern3);
				if(pos != string::npos)
				{
					ruleToTrim.replace(pos, pattern3.size(), "");
			//		cout << "Rule trimmed: " << ruleToTrim << " at "<< pos << endl;
				}
				else
				{
					if(ruleToTrim == RDN)
					{
						ruleToTrim = "";
						cout << "RuleToTrim emptied..." << endl;
					}
				}
			}
		}
		DEBUG("%s"," Exiting HealthCheckService::trimRuleReference");
	}

	ACS_CC_ReturnType HealthCheckService::updateCallback(const char*, const char**)
	{
		DEBUG("%s"," Entering HealthCheckService::updateCallback");
		cout << "update call back" << endl;
		 DEBUG("%s"," Exiting HealthCheckService::updateCallback");
		return ACS_CC_SUCCESS;
	}

	void HealthCheckService::adminOperationCallback(ACS_APGCC_OiHandle, ACS_APGCC_InvocationType, const char*, ACS_APGCC_AdminOperationIdType, ACS_APGCC_AdminOperationParamType**)
	{
		DEBUG("%s"," In HealthCheckService::adminOperationCallback");
	}

	void HealthCheckService::load(void* arg)
	{	
		DEBUG("%s","Entering HealthCheckService::load thread! ");
		cout << "LOAD thread" << endl;
		acs_hcs_global_ObjectImpl obj;
		int index;
		char DN[256];
		memset(DN,0,256);
                strcpy(DN,(char*)arg);
	//	char* DN = reinterpret_cast<char*>(arg);
		cout<<"provided load parameter is "<<DN<<endl;

		bool isValid = obj.isParameterValid(DN, READYTOUSEDN);
		cout<<"value of isvalid is : "<<isValid<<endl;
		if( isValid == false )
		{
			index = obj.getProgressReportIndex((string)READYTOUSEDN);
                        if(index == -1)
                                return;

			 string currentTime = obj.getCurrentTime();
                         acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionStarted = currentTime;

			 HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
                                        acs_hcs_progressReport_ObjectImpl::FAILURE,
                                        "Invalid parameter, cannot be loaded", index, true);
                         return;
		}

		//preparing dn for load operation
		string loadParam(DN);
		size_t equal_pos = loadParam.find("=");
		size_t len = loadParam.size();
		loadParam = loadParam.substr(equal_pos+1,len-12);
		loadParam = (string)"ruleSetFileId=" + loadParam + (string)COMMA_STR + (string)READYTOUSEDN ; 
	        strncpy(DN,loadParam.c_str(),256);				

		OmHandler immHandler;
		ACS_CC_ReturnType returnCode;
		char fileValue[64] = "";
		string loadArg(DN);
		cout<<"Dn in load is: "<<loadArg<<endl;

		string RDN;
		std::vector<ACS_APGCC_ImmAttribute *> attributes;
		ACS_APGCC_ImmAttribute fileName;
		size_t found_equal, found_comma;
		
		returnCode = immHandler.Init();
		if(returnCode != ACS_CC_SUCCESS)
		{
			cout<<"ERROR: init FAILURE!!!\n";
			return;
		}

                string object = DN;
		string parentObject;
		
		found_comma = loadArg.find_first_of(",");
		if(found_comma != std::string::npos)
		{
			parentObject = loadArg.substr(found_comma+1,loadArg.size()-1);
		}

		index = obj.getProgressReportIndex(parentObject);
		if(index == -1)	
		{	
			return;
		}

		if(acs_hcs_brfimplementer::getBrfStatus())
		{
			string currentTime = obj.getCurrentTime();
                        acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionStarted = currentTime;

			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED,
					acs_hcs_progressReport_ObjectImpl::FAILURE,
					"APG Backup in progress, cannot be loaded", index, true);
			return;
		}

		HealthCheckService::isLoadOngoing = true;

		cout << "In load permit flag=======" << HealthCheckService::isLoadOngoing << endl;

		/* Update the progress*/
		DEBUG("%s","Updating Progress report for loading ");
		HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::RUNNING, 
				acs_hcs_progressReport_ObjectImpl::NOT_AVAILABLE, 
				"", index, false);
	
		/* Evaluate the RDN*/
		found_equal = 0;
		found_comma = 0;
		found_equal = loadArg.find_first_of("=");
		if(found_equal != std::string::npos)
		{
			found_comma = loadArg.find_first_of(",", found_equal+1);
			if(found_comma != std::string::npos)
		  	{
		       		RDN = loadArg.substr(found_equal+1, found_comma - (found_equal+1));
	                }
		}

		/* Get the XML file name*/
		fileName.attrName = "fileName";
		attributes.push_back(&fileName);

		returnCode = immHandler.getAttribute(DN, attributes);
		if(returnCode == ACS_CC_FAILURE)
		{
			DEBUG("%s"," GetAttibute failed in load ");
			cout << "GetAttribute failed" << endl;
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
					acs_hcs_progressReport_ObjectImpl::FAILURE, 
					"Internal Error", index, true);
			 DEBUG("%s","return after GetAttibute failed in load ")
			 HealthCheckService::isLoadOngoing = false;
			return;
		}
		else
		{
			DEBUG("%s"," GetAttibute success in load ")
			cout << "GetAttribute success" << endl;
			strcpy(fileValue, "");
			string temp_fileValue = (const char*)attributes[0]->attrValues[0];
			strcpy(fileValue, temp_fileValue.c_str());
			cout << "FileName: " << fileValue << endl;
		}


                /* Get the map of categories and rules*/

                char tempPath[256]={0};
                std::vector<string>::const_iterator it;
                AcsHcs::DocumentXml::MultiMap theMap;
                std::vector<std::string> CategoryList;
                multimap<string, AcsHcs::DocumentXml::rule>::const_iterator pos,end;
                int CategoryCount = 0;

                strcpy(tempPath, NBI_RULES);
                strcat(tempPath, fileValue);

                string completePath(tempPath);
                try
                {
                        AcsHcs::DocumentXml doc(completePath);
                        /* Update the map with categories and rules*/
                        doc.updateCategoriesAndRules(theMap);
                }
                catch (...)
                {
                        cout << "ERROR while loading : Error reading XML file" << endl;
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
					acs_hcs_progressReport_ObjectImpl::FAILURE, 
					"Error reading XML file", index, true);
			DEBUG("%s","return in catch block of load ");
			HealthCheckService::isLoadOngoing = false;
                	return;
                }
	
		
		/* Create object under InUse*/

		/* Check if already instances under InUse have the same rule set file name. If same file name is used then load must not be performed*/
	
		vector<ACS_CC_ValuesDefinitionType> ruleSetFileAttrList;
		ACS_CC_ValuesDefinitionType attributeRDN;
		ACS_CC_ValuesDefinitionType attributeFileName;
		char *className = const_cast<char*>(ACS_HC_RULESETFILE_CLASS_NAME);
		
		std::vector<std::string> inUseChildren;
		std::vector<ACS_APGCC_ImmAttribute*> rsfAttributes;
		ACS_APGCC_ImmAttribute fileAttr;
		fileAttr.attrName = "fileName";
		rsfAttributes.push_back(&fileAttr);
		char fileAttrValue[256];

		returnCode = immHandler.getChildren(INUSEDN, ACS_APGCC_SUBTREE, &inUseChildren);
		if(returnCode != ACS_CC_SUCCESS)
		{
			cout << "getChildren failed" << endl;
		}
		else
		{
			cout << "inUseChildren.size(): " << inUseChildren.size() << endl;
			if(inUseChildren.size() > 0)
			{
				for(unsigned int i = 0; i < inUseChildren.size(); i++)	
				{
					if(inUseChildren[i].find("ruleSetFileId") != string::npos)
					{
						returnCode = immHandler.getAttribute(inUseChildren[i].c_str(), rsfAttributes);
						if(returnCode != ACS_CC_SUCCESS)		
						{
							cout << "getAttribute of RSF failed for " << inUseChildren[i] << " with error: " << immHandler.getInternalLastErrorText() << endl;
							DEBUG("%s","return after getAttribute of RSF failed  ");
							HealthCheckService::isLoadOngoing = false;
							return;
						}
						else
						{
							string temp_fileAttrValue = (const char*)rsfAttributes[0]->attrValues[0];
							strcpy(fileAttrValue, temp_fileAttrValue.c_str());		
							if(!strcmp(fileAttrValue, fileValue))
							{
								cout << "Cannot load as an instance with same file name exists" << endl;
								index = obj.getProgressReportIndex(parentObject);
								HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
										acs_hcs_progressReport_ObjectImpl::FAILURE, 
										CANNOT_LOAD_ERR_PR, index, true,RULESETFILE_EXIST_ALREADY);
								DEBUG("%s"," return after Cannot load as instance with with same file name exists");
								HealthCheckService::isLoadOngoing = false;
								return;
							}
						}
					}
				}
			}
		}

		attributeFileName.attrName = (char*)fileNameAttr;
		attributeFileName.attrType = ATTR_STRINGT;
		char* temp = const_cast<char*>(fileValue);
		void* value1[1]={ reinterpret_cast<void*>(temp)};
		attributeFileName.attrValuesNum = 1;
		attributeFileName.attrValues = value1;

		string r1("ruleSetFileId=");
		string r2(RDN);
		string r3 = r1 + r2;

		cout<<" Trying to fill LoadVect"<<endl;
                loadvect.push_back(RDN);
		
		for (std::list<string>::iterator it=loadvect.begin(); it!=loadvect.end(); ++it)
                        std::cout << ' ' << (*it);
                        std::cout << '\n';
                cout<<"Filled the loadveect"<<endl;
		
		attributeRDN.attrName = (char*)rulesetFileIdAttr;
		attributeRDN.attrType = ATTR_STRINGT;
		attributeRDN.attrValuesNum = 1;
		
		char* rdnValue = const_cast<char*>(r3.c_str());
		void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
		attributeRDN.attrValues = value2;
			
		ruleSetFileAttrList.push_back(attributeFileName);  
		ruleSetFileAttrList.push_back(attributeRDN);

		returnCode = immHandler.createObject(className, INUSEDN, ruleSetFileAttrList);	
		if(returnCode == ACS_CC_FAILURE)
		{
			if(immHandler.getInternalLastError() == -14)
			{
				DEBUG("%s","immHandler.getInternalLastError() == -14 ");
				cout << "ERROR while loading : Cannot be loaded" << endl;
				index = obj.getProgressReportIndex(parentObject);
				HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
			        	                           acs_hcs_progressReport_ObjectImpl::FAILURE, 
								   "Cannot be loaded", index, true,DUPLICATE_INSTANCE);
				DEBUG("%s","return after immHandler.getInternalLastError() == -14 in load")
				HealthCheckService::isLoadOngoing = false;
				return;
			}
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
					acs_hcs_progressReport_ObjectImpl::FAILURE, 
					"Internal Error", index, true);
			DEBUG("%s"," Creation under InUse failed");
			cout << "Creation under InUse failed" << endl;
			cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;
			DEBUG("%s"," return after getInternalLastErrorText in load");
			HealthCheckService::isLoadOngoing = false;
			return;
		}

		/* Delete the object under ReadyToUse*/
		returnCode = immHandler.deleteObject(loadArg.c_str());
		if(returnCode == ACS_CC_FAILURE)
		{
			DEBUG("%s","Deletion under readytouse failed ");
			cout << "Deletion under readytouse failed" << endl;
			cout<<"getInternalLastErrorText :"<< immHandler.getInternalLastErrorText()<<endl;	
			index = obj.getProgressReportIndex(parentObject);
			HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::CANCELLED, 
					acs_hcs_progressReport_ObjectImpl::FAILURE, 
					"Internal Error", index, true);
			DEBUG("%s","return after Deletion under readytouse failed ")
			HealthCheckService::isLoadOngoing = false;
			return;
		}		
	
                /* Move the rule set file from NBI path to internal storage*/
                string ruleFile(fileValue);
                string internalPath = INTERNAL_PATH;
                cout << "source: " << completePath << endl;
                cout << "destination: " << internalPath << endl;
                string command = "mv " + completePath + " " + internalPath;
                cout << "command: " << command.c_str() << endl;
                system(command.c_str());
	
		/* Get the list of categories from the map*/
		for(pos = theMap.begin(), end = theMap.end(); pos != end; pos = theMap.upper_bound(pos->first))
		{
			CategoryList.push_back(pos->first);
		}
		CategoryCount = CategoryList.size();
		 DEBUG("%s","Creating categories and rules ")
		/* Create categories and rules*/		
		for(it = CategoryList.begin(); it != CategoryList.end(); ++it)
		{
			cout << (*it) << endl;
			std::list<AcsHcs::DocumentXml::rule> ruleinfo;
			getRuleDetailsOfaCategory(*it, ruleinfo, theMap);
			createCategory(*it, ruleinfo, RDN);
		}		

		/* Move the rule set file from NBI path to internal storage*/
/*		string ruleFile(fileValue);
		string internalPath = INTERNAL_PATH;
		cout << "source: " << completePath << endl;
		cout << "destination: " << internalPath << endl;
		string command = "mv " + completePath + " " + internalPath;
		cout << "command: " << command.c_str() << endl;	
		system(command.c_str());
*/
		index = obj.getProgressReportIndex(parentObject);
		HealthCheckService::updateProgress(acs_hcs_progressReport_ObjectImpl::FINISHED, 
				acs_hcs_progressReport_ObjectImpl::SUCCESS, 
				"Successfully loaded", index, true);
			
		immHandler.Finalize();

		HealthCheckService::isLoadOngoing = false;

		cout << "After unload permit flag=======" << HealthCheckService::isLoadOngoing << endl;
		
		DEBUG("%s","Leaving HealthCheckService::load thread!!! ");
	}

	void HealthCheckService::createCategory(std::string category, std::list<DocumentXml::rule>& ruleinfo, std::string ruleSetID)
	{
		DEBUG("%s"," Entering HealthCheckService::createCategory");	
		cout << "createCatRules" << endl;
		OmHandler immHandler;
		ACS_CC_ReturnType returnCode;
		returnCode=immHandler.Init();
		if(returnCode!=ACS_CC_SUCCESS)
		{
			DEBUG("%s","ERROR: init FAILURE!! ");
			cout<<"ERROR: init FAILURE!!!\n";
		}
		
		/* Create category instances*/
		vector<ACS_CC_ValuesDefinitionType> categoryAttrList;
		ACS_CC_ValuesDefinitionType catRDN;
		ACS_CC_ValuesDefinitionType updateTime;
		ACS_CC_ValuesDefinitionType ruleReferences;
		char *className = const_cast<char*>(ACS_HC_CATEGORY_CLASS_NAME);

		string cat1("categoryId=");
		string cat2(category);
		string cat3 = cat1 + cat2;
		
		catRDN.attrName = (char*)categoriesIdAttr;
		catRDN.attrType = ATTR_STRINGT;
		catRDN.attrValuesNum = 1;
		char* rdnValue = const_cast<char*>(cat3.c_str());		
		void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
		catRDN.attrValues = value2;

		/* Calculate the current time*/	
		time_t rawTime = time(0);
		struct tm formattedTime;
		char currentTime[20];
		
		formattedTime = *localtime(&rawTime);
		sprintf(currentTime, "%d-%d-%dT%d:%d:%d", formattedTime.tm_year+1900, formattedTime.tm_mon+1, formattedTime.tm_mday, 
									formattedTime.tm_hour, formattedTime.tm_min, formattedTime.tm_sec); 

		cout << "Formatted Time: " << currentTime << endl;	
		updateTime.attrName = (char*)LastupdateTime_STR;
		updateTime.attrType = ATTR_STRINGT;
		updateTime.attrValuesNum = 1;
		char* timeValue = const_cast<char*>(currentTime);
		void* value3[1]={ reinterpret_cast<void*>(timeValue)};
		updateTime.attrValues = value3;

		ruleReferences.attrName = (char*)ruleReferenceAttr;
		ruleReferences.attrType = ATTR_STRINGT;
		ruleReferences.attrValuesNum = 1;
                char* ruleReferencesValue = const_cast<char*>(ruleSetID.c_str());
                void* value4[1]={ reinterpret_cast<void*>(ruleReferencesValue)};
                ruleReferences.attrValues = value4;
	
		categoryAttrList.push_back(catRDN);
		categoryAttrList.push_back(updateTime);
		categoryAttrList.push_back(ruleReferences);

		returnCode = immHandler.createObject(className, ACS_HC_ID, categoryAttrList);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "Creation failed: " << immHandler.getInternalLastErrorText() << endl;
			if(immHandler.getInternalLastError() == -14)
			{
                                cout << "Already created, appending the new RSID" << endl;

                                /* Get the existing RSID*/
                                string categoryDN = cat3 + "," + ACS_HC_ID;
                                cout << "categoryDN: " << categoryDN << endl;
                                ACS_CC_ImmParameter ruleReferences;
                                ruleReferences.attrName = (char*)ruleReferenceAttr;
				returnCode = immHandler.getAttribute(categoryDN.c_str(), &ruleReferences);
                                if(returnCode != ACS_CC_FAILURE)
                                {
                                        char existingRef[1024];
                                        string newRef = "";
                                        strcpy(existingRef, "");
					string temp_existingRef = (const char*)ruleReferences.attrValues[0];
                                        strcpy(existingRef, temp_existingRef.c_str());
                                        cout << "existingRef " << existingRef << endl;
                                        newRef = existingRef;
                                        newRef = newRef + ",";
                                        newRef = newRef + ruleSetID;
                                        cout << "newRef: " << newRef << endl;

                                        ACS_CC_ImmParameter par;
                                        par.attrName = (char*)ruleReferenceAttr;
                                        par.attrType = ATTR_STRINGT;
                                        par.attrValuesNum = 1;
                                        char* ruleRefValue = const_cast<char*>(newRef.c_str());
                                        void* value7[1]={ reinterpret_cast<void*>(ruleRefValue)};
                                        par.attrValues = value7;

                                        returnCode = immHandler.modifyAttribute(categoryDN.c_str(), &par);
                                        if(returnCode != ACS_CC_FAILURE)
                                        {
                                                cout << "Reference appended" << endl;
                                        }
                                        else
                                        {
                                                cout << "Reference append modify failed" << immHandler.getInternalLastErrorText() << endl;
                                        }
                                }
                                else
                                {
                                        cout << "getAttribute for refereces failed " << immHandler.getInternalLastErrorText() << endl;
                                }
			}
		}
		else
		{
			cout << "Category created" << endl;
		}

		/* Create Rule instances*/
		std::list<DocumentXml::rule>::const_iterator it;
		for( it = ruleinfo.begin(); it != ruleinfo.end(); ++it)
		{
			createRule(cat3, it, ruleSetID);
		}
		 DEBUG("%s"," Leaving HealthCheckService::createCategory");	
	}

	void HealthCheckService::createRule(string parentDN, std::list<DocumentXml::rule>::const_iterator it, std::string ruleSetID)
	{
		DEBUG("%s","Entering HealthCheckService::createRule ");
	        OmHandler immHandler;
	        ACS_CC_ReturnType returnCode;
	        returnCode=immHandler.Init();
	        if(returnCode!=ACS_CC_SUCCESS)
		{
			DEBUG("%s","ERROR: init FAILURE! in create Rule ");
		        cout<<"ERROR: init FAILURE!!!\n";
		}
		
		vector<ACS_CC_ValuesDefinitionType> ruleAttrList;
		ACS_CC_ValuesDefinitionType ruleRDN;
		ACS_CC_ValuesDefinitionType ruleName;
		ACS_CC_ValuesDefinitionType ruleSlogan;
		ACS_CC_ValuesDefinitionType ruleReferences;
		
		char* className = const_cast<char*>(ACS_HC_RULE_CLASS_NAME);
		string rule1("ruleId=");
		char rule2[64];

		string parent = parentDN + "," + ACS_HC_ID;
		strcpy(rule2, (*it).rule_no);
		string rule3(rule2);
		string rule4 = rule1 + rule3;
		
		ruleRDN.attrName = (char*)ruleID_STR;
		ruleRDN.attrType = ATTR_STRINGT;
		ruleRDN.attrValuesNum = 1;
		char* rulerdnValue = const_cast<char*>(rule4.c_str());
		void* value3[1]={ reinterpret_cast<void*>(rulerdnValue)};
		ruleRDN.attrValues = value3;

		ruleName.attrName = (char*)name_STR;
		ruleName.attrType = ATTR_STRINGT;
		ruleName.attrValuesNum = 1;
		char* ruleNameValue = const_cast<char*>((*it).rule_name);
		void* value4[1]={ reinterpret_cast<void*>(ruleNameValue)};
		ruleName.attrValues = value4;

		ruleSlogan.attrName = (char*)description_STR;
		ruleSlogan.attrType = ATTR_STRINGT;
		ruleSlogan.attrValuesNum = 1;
		char* ruleSloganValue = const_cast<char*>((*it).rule_slogan);
		void* value5[1]={ reinterpret_cast<void*>(ruleSloganValue)};
		ruleSlogan.attrValues = value5;

		ruleReferences.attrName = (char*)ruleReferenceAttr;
		ruleReferences.attrType = ATTR_STRINGT;
		ruleReferences.attrValuesNum = 1;
		char* ruleReferencesValue = const_cast<char*>(ruleSetID.c_str());
		void* value6[1]={ reinterpret_cast<void*>(ruleReferencesValue)};
		ruleReferences.attrValues = value6;
		
		ruleAttrList.push_back(ruleRDN);
		ruleAttrList.push_back(ruleName);
		ruleAttrList.push_back(ruleSlogan);
		ruleAttrList.push_back(ruleReferences);
		
		returnCode = immHandler.createObject(className, parent.c_str(), ruleAttrList);
		if(returnCode == ACS_CC_FAILURE)
		{
			DEBUG("%s","Rule Creation failed:  ");
			cout << "Rule Creation failed: " << immHandler.getInternalLastErrorText() << endl;
			if(immHandler.getInternalLastError() == -14)
			{
				cout << "Already created, appending the new RSID" << endl;
				
				/* Get the existing RSID*/
				string ruleModify = rule4 + "," + parent;
				cout << "ruleModify: " << ruleModify << endl;
				ACS_CC_ImmParameter ruleReferences;
				ruleReferences.attrName = (char*)ruleReferenceAttr;
				
				returnCode = immHandler.getAttribute(ruleModify.c_str(), &ruleReferences);
				if(returnCode != ACS_CC_FAILURE)
				{
					char existingRef[1024];
					string newRef = "";
					strcpy(existingRef, "");
					strcpy(existingRef, (const char*)ruleReferences.attrValues[0]);
					cout << "existingRef " << existingRef << endl;
					newRef = existingRef;
					newRef = newRef + ",";
					newRef = newRef + ruleSetID;
					cout << "newRef: " << newRef << endl;
					
					ACS_CC_ImmParameter par;
					par.attrName = (char*)ruleReferenceAttr;
					par.attrType = ATTR_STRINGT;
					par.attrValuesNum = 1;
   			                char* ruleRefValue = const_cast<char*>(newRef.c_str());
                    			void* value7[1]={ reinterpret_cast<void*>(ruleRefValue)};
      					par.attrValues = value7;
				
					returnCode = immHandler.modifyAttribute(ruleModify.c_str(), &par);
					if(returnCode != ACS_CC_FAILURE)
					{
						cout << "Reference appended" << endl;	
					}
					else
					{
						cout << "Reference append modify failed" << immHandler.getInternalLastErrorText() << endl;
					}
				}
				else
				{
					cout << "getAttribute for refereces failed " << immHandler.getInternalLastErrorText() << endl;
				}
			}
		}
		else
		{
			cout << "rule created" << endl;
		}
		immHandler.Finalize();
		 DEBUG("%s","Leaving HealthCheckService::createRule ");
	}

	void HealthCheckService::displayCategoryList(std::vector<string>& CategoryList)
	{
		DEBUG("%s"," Entering HealthCheckService::displayCategoryList");
		std::vector<string>::const_iterator it;
		cout << "CATEGORIES.." << endl << endl;
		for( it = CategoryList.begin(); it != CategoryList.end(); ++it)
		{
			cout<< *it <<endl;
		}
		DEBUG("%s","Leaving  HealthCheckService::displayCategoryList ");
	}

	void HealthCheckService::getRuleDetailsOfaCategory(string category, std::list<DocumentXml::rule>& ruleinfo, AcsHcs::DocumentXml::MultiMap theMap)
	{
		DEBUG("%s","Entering HealthCheckService::getRuleDetailsOfaCategory ");
		pair<multimap<string,DocumentXml::rule>::const_iterator, multimap<string,DocumentXml::rule>::const_iterator>its;
		//make a range
		its = theMap.equal_range(category);
		for( ; its.first != its.second; ++its.first)
		{
			ruleinfo.push_back((its.first)->second);
		}
		DEBUG("%s","Leaving HealthCheckService::getRuleDetailsOfaCategory ");
	}

	void HealthCheckService::displayRuleList(std::list<DocumentXml::rule>& ruleinfo)
	{
		DEBUG("%s","Entering HealthCheckService::displayRuleList ");
		std::list<DocumentXml::rule>::const_iterator it;
		for( it = ruleinfo.begin(); it != ruleinfo.end(); ++it)
		{
			cout << "Rule# " << (*it).rule_no << endl;
			cout << "Name " << (*it).rule_name << endl;
			cout << "Slogan " << (*it).rule_slogan << endl;
		}
		DEBUG("%s","Leaving HealthCheckService::displayRuleList ");
	}

	void HealthCheckService::stopOIThreads()
	{
		DEBUG("%s","Entering HealthCheckService::stopOIThreads ");

		//closing jobscheduler thread
		jobScheduler::stopRequested = true;
		jobScheduler::eventCallbackFlag = true;

		if(ProgressReportImpl)
			ProgressReportImpl->shutdown();

		if(ReadyToUseHandler)
			ReadyToUseHandler->shutdown();

		if(InUseHandler)
			InUseHandler->shutdown();

		if(JobBasedHandler)
			JobBasedHandler->shutdown();

		if(JobSchedulerHandler)
			JobSchedulerHandler->shutdown();

		if(SingleEventHandler)
			SingleEventHandler->shutdown();

		if(PeriodicEventHandler)
			PeriodicEventHandler->shutdown();

		if(CalenderPeriodicEventHandler)
			CalenderPeriodicEventHandler->shutdown();

		if(ReadyToUseHandler)
			ReadyToUseHandler->wait();

		if(InUseHandler)
			InUseHandler->wait();

		if(JobBasedHandler)
			JobBasedHandler->wait();

		if(JobSchedulerHandler)
			JobSchedulerHandler->wait();

		if(SingleEventHandler)
			SingleEventHandler->wait();

		if(PeriodicEventHandler)
			PeriodicEventHandler->wait();

		if(CalenderPeriodicEventHandler)
			CalenderPeriodicEventHandler->wait();

		//if(ProgressReportImpl)
		 	//ProgressReportImpl->shutdown();

		ACE_OS::sleep(1);
		
		DEBUG("%s","Deleting the progress report  info in StopOI thread ");

		for(unsigned int it = 0; it < acs_hcs_progressReport_ObjectImpl::progressReport.size(); it++){

			if(acs_hcs_progressReport_ObjectImpl::progressReport[it].handler){
				DEBUG("%s","In first if-- Deleting the progress report handler");
				delete acs_hcs_progressReport_ObjectImpl::progressReport[it].handler;
				acs_hcs_progressReport_ObjectImpl::progressReport[it].handler = 0;
				DEBUG("%s","After deletion of the progress report handler");
			}

            if(acs_hcs_progressReport_ObjectImpl::progressReport[it].impl){
				DEBUG("%s","In second if-- Deleting the progress report implementer");
				bool removeImplementer = false;

				for (int i=0; i < 3; ++i){
					if(acs_hcs_progressReport_ObjectImpl::progressReport[it].impl->finalize() != ACS_CC_SUCCESS){
						int intErr = getInternalLastError();
						if ( intErr == -6){
							//for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
							DEBUG("%s","Failed in removing progress report implementer");
							sleep(0.1);
							continue;
						}
						else{
							DEBUG("%s","Implementer for progress report removed successfully");
                           	break;
						}
					}
                    else{
						removeImplementer = true;
                        break;
                    }
				}
				
				if(removeImplementer)
				{
					cout<<"In second if- Deleting the progress report implementers"<<endl;
					DEBUG("%s","In second nested if- Deleting the progress report implementers");	
					delete acs_hcs_progressReport_ObjectImpl::progressReport[it].impl;
					acs_hcs_progressReport_ObjectImpl::progressReport[it].impl = 0;
				}
				else
				{
					cout<<"Progressrep implementer is not finalized"<<endl;
					DEBUG("%s","Progressrep implementer is not finalized");
				}

				/*acs_hcs_progressReport_ObjectImpl::progressReport[it].impl->finalize();
				cout<<"In second if- Deleting the progress report implementers"<<endl;
                                DEBUG("%s","In second nested if- Deleting the progress report implementers");
                                delete acs_hcs_progressReport_ObjectImpl::progressReport[it].impl;
                                acs_hcs_progressReport_ObjectImpl::progressReport[it].impl = 0;*/
            }
		}
		DEBUG("%s","Deleting the OI handlers in stopOI threads ");
		if(ReadyToUseHandler)
			delete ReadyToUseHandler;
		if(InUseHandler)
			delete InUseHandler;
		if(JobBasedHandler)
			delete JobBasedHandler;			
		if(JobSchedulerHandler)
			delete JobSchedulerHandler;
		if(SingleEventHandler)
			delete SingleEventHandler;
		if(PeriodicEventHandler)
			delete PeriodicEventHandler;
		if(CalenderPeriodicEventHandler)
			delete CalenderPeriodicEventHandler;

		acs_hcs_progressReport_ObjectImpl::progressReport.erase (acs_hcs_progressReport_ObjectImpl::progressReport.begin(), acs_hcs_progressReport_ObjectImpl::progressReport.end());

		if(ProgressReportImpl)
			delete ProgressReportImpl;		
				
		DEBUG("%s","Leaving HealthCheckService::stopOIThreads ")	
	}

	void HealthCheckService::createProgressReport(std::string parent)
	{
		DEBUG("%s"," Entering HealthCheckService::createProgressReport ");	
		// Introduced progressDeleted variable for TR HS37486...  
		AcsHcs::HealthCheckService::progressDeleted = false;  
		cout << "SAMPLE" << endl;
		ACS_CC_ReturnType returnCode;
		vector<ACS_CC_ValuesDefinitionType> progressAttrList;
		//char *structName = const_cast<char*>(ACS_HC_ASYNCACTION_STRUCT_NAME);
		ACS_CC_ValuesDefinitionType rdnAttr;
		ACS_CC_ValuesDefinitionType actionNameAttr;
		ACS_CC_ValuesDefinitionType additionalInfoAttr;
		ACS_CC_ValuesDefinitionType progressInfoAttr;
		ACS_CC_ValuesDefinitionType progressPercentageAttr;
		ACS_CC_ValuesDefinitionType resultAttr;
		ACS_CC_ValuesDefinitionType resultInfoAttr;
		ACS_CC_ValuesDefinitionType stateAttr;
		ACS_CC_ValuesDefinitionType actionIdAttr;
		ACS_CC_ValuesDefinitionType timeActionStartedAttr;
		ACS_CC_ValuesDefinitionType timeActionCompletedAttr;
		ACS_CC_ValuesDefinitionType timeOfLastStatusUpdateAttr;

		rdnAttr.attrName = (char*)id_STR;
		rdnAttr.attrType = ATTR_STRINGT;
		rdnAttr.attrValuesNum = 1;
		char* rdn = const_cast<char*>("id=1");
		void* value1[1]={ reinterpret_cast<void*>(rdn)};	
		rdnAttr.attrValues = value1;

                actionNameAttr.attrName = (char*)actionName_STR;
                actionNameAttr.attrType = ATTR_STRINGT;
                actionNameAttr.attrValuesNum = 0;
                actionNameAttr.attrValues = 0;

                additionalInfoAttr.attrName = (char*)additionalInfo_STR;
                additionalInfoAttr.attrType = ATTR_STRINGT;
                additionalInfoAttr.attrValuesNum = 0;
                additionalInfoAttr.attrValues = 0;

                progressInfoAttr.attrName = (char*)progressInfo_STR;
                progressInfoAttr.attrType = ATTR_STRINGT;
                progressInfoAttr.attrValuesNum = 0;
                progressInfoAttr.attrValues = 0;

                progressPercentageAttr.attrName = (char*)progressPercentage_STR;
                progressPercentageAttr.attrType = ATTR_UINT32T;
                progressPercentageAttr.attrValuesNum = 0;
                progressPercentageAttr.attrValues = 0;

                resultAttr.attrName = (char*)result_STR;
                resultAttr.attrType = ATTR_INT32T;
                resultAttr.attrValuesNum = 0;
                resultAttr.attrValues = 0;

                resultInfoAttr.attrName = (char*)resultInfo_STR;
                resultInfoAttr.attrType = ATTR_STRINGT;
                resultInfoAttr.attrValuesNum = 0;
                resultInfoAttr.attrValues = 0;

                stateAttr.attrName = (char*)state_STR;
                stateAttr.attrType = ATTR_INT32T;
                stateAttr.attrValuesNum = 0;
                stateAttr.attrValues = 0;

                actionIdAttr.attrName =(char*)actionId_STR;
                actionIdAttr.attrType = ATTR_UINT32T;
                actionIdAttr.attrValuesNum = 0;
                actionIdAttr.attrValues = 0;

                timeActionStartedAttr.attrName = (char*)timeActionStarted_STR;
                timeActionStartedAttr.attrType = ATTR_STRINGT;
                timeActionStartedAttr.attrValuesNum = 0;
                timeActionStartedAttr.attrValues = 0;

                timeActionCompletedAttr.attrName = (char*)timeActionCompleted_STR;
                timeActionCompletedAttr.attrType = ATTR_STRINGT;
                timeActionCompletedAttr.attrValuesNum = 0;
                timeActionCompletedAttr.attrValues = 0;

		timeOfLastStatusUpdateAttr.attrName = (char*)timeOfLastStatusUpdate_STR;
		timeOfLastStatusUpdateAttr.attrType = ATTR_STRINGT;
		timeOfLastStatusUpdateAttr.attrValuesNum = 0;
		timeOfLastStatusUpdateAttr.attrValues = 0;

		progressAttrList.push_back(rdnAttr);
		progressAttrList.push_back(actionNameAttr);
		progressAttrList.push_back(additionalInfoAttr);
		progressAttrList.push_back(progressInfoAttr);
		progressAttrList.push_back(progressPercentageAttr);
		progressAttrList.push_back(resultAttr);
		progressAttrList.push_back(resultInfoAttr);
		progressAttrList.push_back(stateAttr);
		progressAttrList.push_back(actionIdAttr);
		progressAttrList.push_back(timeActionStartedAttr);
		progressAttrList.push_back(timeActionCompletedAttr);
		progressAttrList.push_back(timeOfLastStatusUpdateAttr);

		cout << "initing..." << endl;
		std::string myImplementerName = const_cast<char*>("TestImpl=");
		myImplementerName = myImplementerName + parent;
		acs_hcs_progressReport_ObjectImpl::progress progressToInsert;
		progressToInsert.impl = new acs_hcs_progressReport_ObjectImpl();
		progressToInsert.DN = parent;
		progressToInsert.state = acs_hcs_progressReport_ObjectImpl::FINISHED;
		progressToInsert.result = acs_hcs_progressReport_ObjectImpl::NOT_AVAILABLE;

		bool initSuccess = false;
		for ( int i = 0 ; i<12 ; i++)
                {
			//Various error codes we are handling
			// -6  : ACS_APGCC_ERR_IMM_TRY_AGAIN 
                        // -14 : ACS_APGCC_ERR_IMM_EXIST 
			// -12 : ACS_APGCC_ERR_IMM_NOT_EXIST
			// -9  : ACS_APFCC_ERR_IMM_BAD_HANDLE 
                        if( initSuccess == false)
                        {
				returnCode = progressToInsert.impl->init(myImplementerName);
				int result = 0;
				if(returnCode == ACS_CC_FAILURE)
				{
					result = progressToInsert.impl->getInternalLastError();
					if(result == -6 || result == -12)
					{
						DEBUG("%s","Init failed as error is try again");
						ACE_OS::sleep(1);
						continue;			
					}		
					else if(result == -14)
					{
						DEBUG("%s","Implementer has already been set");
						initSuccess = true;
					}
					else
					{
						DEBUG("%s %d","Init failed in Createprogressreport", result);
						break;
					}	
				}
                                else
                                {
					DEBUG("%s","Init success for progress report");
                                        initSuccess = true;
                                }

                        }
                        else
                        {
                                cout << "Init success" << endl;
                                returnCode = progressToInsert.impl->createRuntimeObj(ACS_HC_ASYNCACTION_STRUCT_NAME, parent.c_str(), progressAttrList);
				int result = 0;
                                if(returnCode == ACS_CC_FAILURE)
                                {
					result = progressToInsert.impl->getInternalLastError();
					if(result == -12 || result == -6)
					{
						DEBUG("%s","Progress report failed as DN does not exists");
						ACE_OS::sleep(1);	
						continue;		
					}
					else if(result == -14)
					{
						DEBUG("%s","Progress report already exists");
	                                        cout << "Progress Report created ==== " << progressToInsert.impl->getInternalLastError() << endl;
        	                                progressToInsert.handler = new acs_hcs_progressReportHandler(progressToInsert.impl);
                                	        progressToInsert.handler->activate();
                                        	acs_hcs_progressReport_ObjectImpl::progressReport.push_back(progressToInsert);
						DEBUG("%s","Progress report thread activated");
	                                        cout << "Pushed: " << progressToInsert.DN << endl;
        	                                break;
					}
					else
					{
                                        	cout << "Creation of progress failed for " << parent.c_str() << " with: " <<progressToInsert.impl->getInternalLastErrorText() << endl;
	                                        DEBUG("%s%d","Creation of progress failed", progressToInsert.impl->getInternalLastErrorText());
						DEBUG("%s%d","error code is: ", result);
                	                        progressToInsert.impl->finalize();
                        	                break;				
					} 
				}
				else
	                        {
					DEBUG("%s","Progress report created successfully");
                                        cout << "Progress Report created ==== " << progressToInsert.impl->getInternalLastError() << endl;
                                        progressToInsert.handler = new acs_hcs_progressReportHandler(progressToInsert.impl);
                                        progressToInsert.handler->activate();
                                        acs_hcs_progressReport_ObjectImpl::progressReport.push_back(progressToInsert);
                                        cout << "Pushed: " << progressToInsert.DN << endl;
					DEBUG("%s","Progress report thread activated");
                                        break;
                                }
                        }
                }//end of for
                	
		DEBUG("%s"," Leaving HealthCheckService::createProgressReport");
	}


	// Starting point for the Health Check Service 
	int HealthCheckService::start()
	{
		DEBUG("%s","Entering HealthCheckService::start() ");
		syslog(LOG_INFO, "HC STARTED"); 
		DEBUG("%s","calling createHCSDirectories()");
		if(!createHCSDirectories())
		{
			cout << "HC directories could not be created" << endl;
			DEBUG("%s","HC directories could not be created");
			return -1;
		}
		DEBUG("%s","Creating backup thread");
		acs_hcs_brfimplementer::stopRequested = false; //Added by Malli for brf tr HS64073 
		AcsHcs::HealthCheckService::isShutdownOngoing = true;
		acs_hcs_brfimplementer brfObj;
		//brfObj.createBrfcThread();
		brfObj.activate();
		DEBUG("%s","calling setupIMMCallBacks()");
	        if(!setupIMMCallBacks())
	        {
                	return -1;
	        }

		// Create a thread to monitor the rules NBI folder
		Inotify::stopRequested = false;
		Inotify* inotify = new Inotify();
		if(inotify)
			//inotify->activate();
			inotify->activate( THR_JOINABLE | THR_NEW_LWP );

		jobScheduler::stopRequested = false;
		jobScheduler *job = new jobScheduler();
		if(job)
			//job->activate();
			job->activate( THR_JOINABLE | THR_NEW_LWP );
	

		AcsHcs::HealthCheckService::stopRequested = false;	
		while(!AcsHcs::HealthCheckService::stopRequested)
		{
			if ( !HealthCheckCmd::stopEventFlag )
			{
			}

			if(acs_hcs_jobbased_ObjectImpl::jobList_create.size() != 0)
			{
				vector<string> job(acs_hcs_jobbased_ObjectImpl::jobList_create);
				acs_hcs_jobbased_ObjectImpl::jobList_create.clear();
				do
				{
					string rdn = job[0];
					acs_hcs_global_ObjectImpl obj;
					obj.createJobScheduler(rdn);
					HealthCheckService hcService;
					ACE_OS::sleep(1);
					hcService.createProgressReport(rdn);
					rdn = JOBSCHEDULER + rdn ;
					hcService.createProgressReport(rdn);
					job.erase(job.begin(),job.begin()+1);
				}while(job.size()!= 0);
			}

						ACE_OS::sleep(1); 
		}
		cout << "Calling HealthCheckService::stopOIThreads " << endl;
		DEBUG("%s","Calling HealthCheckService::stopOIThreads ");
		
		/* Stop backup thread and cleanup brf data*/
		acs_hcs_brfimplementer::stopRequested = true;	
		
		if(acs_hcs_brfimplementer::brfThrCreated)
		{
			brfObj.deleteParticipant();
			if(brfObj.theHcBrfcRuntimeHandlerThreadPtr)
			{
				delete brfObj.theHcBrfcRuntimeHandlerThreadPtr;
				brfObj.theHcBrfcRuntimeHandlerThreadPtr = 0;
			}
			if (brfObj.theHcBrfcImpl)
			{
				delete brfObj.theHcBrfcImpl;
				brfObj.theHcBrfcImpl = 0;
			}
		}
		
		stopOIThreads();
		Inotify::stopRequested = true;
		//jobScheduler::stopRequested = true;
		//jobScheduler::eventCallbackFlag = true;
		//while(!Inotify::threadExited || !jobScheduler::threadExited) 
		//	continue;

		DEBUG("%s","Waiting for Inotify to close");
		inotify->wait();
		DEBUG("%s","Waiting for job to close");
		job->wait();
		DEBUG("%s","Both job and inotify were closed");
		if(inotify)
		{
			delete inotify;	
			inotify = 0;
		}
		if(job)
		{
			delete job;
			job = 0;
		}
		cout << "Retuning from main" << endl;
		acs_hcs_brfimplementer::brfThrCreated = false;
	 	DEBUG("%s","Leaving HealthCheckService::start() ");
		AcsHcs::HealthCheckService::isShutdownOngoing = false;
		return 0;
	}

	bool HealthCheckService::isJobNeedToTrigger(string job, string category)
	{
		acs_hcs_global_ObjectImpl obj;
		if ( job == EMPTY_STR)
			return false;

		else //job to trigger is provided
		{
			obj.removeTrailingSpaces(category);
			if( category == ALL_CATEGORIES )
			{
				cout << "Executing ALL categories" << endl;
				//ACE_OS::sleep(10);	
				int status = obj.getImmIntAttribute(ACS_HC_ID , statusAttr);
				if( status == ACS_HC_NOTOK)
					return true;
			}
			else
			{
				set<string> categoryList;
				category = category + (string)",MANDATORY"; //To add mandatory category to existing list
                        	obj.tokenizeCategories(categoryList, category);
				//categoryList.insert("MANDATORY"); 
                        	for(set<string>::iterator it = categoryList.begin(); it != categoryList.end(); ++it)
                        	{
                                	string categoryItem(*it);
	                                string categoryDN = categoryItem + "," + ACS_HC_ID;
        	                        cout<<"category provided in job are "<<categoryDN<<endl;
                	                int status = obj.getImmIntAttribute(categoryDN,statusAttr);
                        	        cout<<"status of category = "<<status<<endl;

                                	if(status == ACS_HC_NOTOK)
                                        	return true;
                        	}
			}
		}
		return false;
	}

	void HealthCheckService::spawnExecutionThread(string job, bool scheduleExecution)
	{
		string jobDN = "jobId=" + job + "," + ACS_HC_ID;
		char tempRDN[256]={0};
		void* triggerArg;
		strcpy(tempRDN, jobDN.c_str());
		triggerArg = reinterpret_cast<void*>(tempRDN);
		if(scheduleExecution)
	        	AcsHcs::HealthCheckService::scheduledExecute(triggerArg);		

		else
			AcsHcs::HealthCheckService::execute(triggerArg);
	}
}
