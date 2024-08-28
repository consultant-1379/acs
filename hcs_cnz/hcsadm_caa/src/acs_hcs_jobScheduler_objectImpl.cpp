/*=================================================================== */
/**
   @file acs_hcs_jobScheduler_ObjectImpl.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A				     XMALRAO       Initial Release
=================================================================== */

/*=================================================================== 
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <ace/Signal.h>
#include "acs_hcs_jobScheduler_objectImpl.h"
#include <ace/Signal.h>
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_Util.H"
#include "acs_hcs_jobscheduler.h"
#include "acs_hcs_progressReport_objectImpl.h"
#include <ACS_CS_API.h>

using namespace std;

/*===================================================================
                STATIC VARIABLE DECLARATION SECTION
=================================================================== */
bool acs_hcs_jobScheduler_ObjectImpl::updation_after_execution = false;

/*===================================================================
   ROUTINE: acs_hcs_jobScheduler_ObjectImpl
=================================================================== */
acs_hcs_jobScheduler_ObjectImpl::acs_hcs_jobScheduler_ObjectImpl(string ClassName, string szImpName, ACS_APGCC_ScopeT enScope)
		:acs_apgcc_objectimplementereventhandler_V3(  ClassName,szImpName,enScope), theClassName(ClassName)
{
	DEBUG("%s"," Entering acs_hcs_jobScheduler_ObjectImpl::acs_hcs_jobScheduler_ObjectImpl");
	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	m_poTPReactorImpl = 0;
	m_poReactor = 0;
	m_poTPReactorImpl = new ACE_TP_Reactor();
        if( m_poTPReactorImpl == 0 )
        {
                ERROR("%s", "Memory allocation failed for m_poTPReactorImpl");
        }

        m_poReactor = new ACE_Reactor(m_poTPReactorImpl);
        if( m_poReactor == 0 )
        {
                ERROR("%s", "Memory allocation failed for m_poReactor");
        }

	if( theOiHandlerPtr == 0 )
	{
		ERROR("%s", "Memory allocation failed for  acs_apgcc_oihandler_V3");
	}

	DEBUG("%s"," Exiting acs_hcs_jobScheduler_ObjectImpl::acs_hcs_jobScheduler_ObjectImpl");
}//End of acs_hcs_jobScheduler_ObjectImpl

/*===================================================================
   ROUTINE: ~acs_hcs_jobScheduler_ObjectImpl
=================================================================== */
acs_hcs_jobScheduler_ObjectImpl::~acs_hcs_jobScheduler_ObjectImpl()
{
	DEBUG("%s"," Entering acs_hcs_jobScheduler_ObjectImpl::~acs_hcs_jobScheduler_ObjectImpl");
	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_HC_JOBSCHEDULER_CLASS_NAME);
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}
        if( m_poReactor != 0  )
        {
                delete m_poReactor;
                m_poReactor = 0;
        }

        if( m_poTPReactorImpl != 0 )
        {
                delete m_poTPReactorImpl;
                m_poTPReactorImpl = 0;
        }

	DEBUG("%s"," Exiting acs_hcs_jobScheduler_ObjectImpl::~acs_hcs_jobScheduler_ObjectImpl");
}//End of ~acs_hcs_jobScheduler_ObjectImpl

/*===================================================================
   ROUTINE: setImpl()
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::setImpl()
{
	DEBUG("%s","Entering  acs_hcs_jobScheduler_ObjectImpl::setImpl() ");	
#if 0
        for (int i=0; i < 10; ++i)
        {
                theOiHandlerPtr->removeClassImpl(this, ACS_HC_JOBSCHEDULER_CLASS_NAME);
                ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_JOBSCHEDULER_CLASS_NAME);

                if (errorCode == ACS_CC_FAILURE)
                {
                        ERROR("Set implementer %s for the object %s failed",getImpName().c_str(), getObjName().c_str());
                        int intErr = getInternalLastError();
                        ERROR("Failed in addClassImpl with error : %s", getInternalLastErrorText());
                        for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
                        if ( i >= 9 )
                        {
                                DEBUG("addClassImpl failed with ErrCode = %d",intErr);

                                return ACS_CC_SUCCESS;
                        }
                        else
                                continue;
                }
                else
                {
                        DEBUG("Success: Set implementer %s for the object %s",getImpName().c_str(), getObjName().c_str());
                        m_isClassImplAdded = true;
                        return ACS_CC_SUCCESS;
                }
        }
#endif

        ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this, ACS_HC_JOBSCHEDULER_CLASS_NAME);

        if ( errorCode == ACS_CC_FAILURE )
        {
                int intErr = getInternalLastError();
                cout<<"intErr:"<<intErr<<endl;
                cout<<"Error : Set implementer  for the object failure"<<endl;
                ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
                DEBUG("%s"," Exiting acs_hcs_jobScheduler_ObjectImpl::svc Dispatch");
                return ACS_CC_FAILURE;
        }
        else
        {
                cout << "acs_hcs_jobScheduler_ObjectImpl success" << endl;
                DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
        }


	DEBUG("%s"," Leaving acs_hcs_jobScheduler_ObjectImpl::setImpl() ");
        return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 acs_hcs_jobScheduler_ObjectImpl::svc(  )
{
	DEBUG("%s"," Entering acs_hcs_jobScheduler_ObjectImpl::svc Dispatch");
#if 0
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this, ACS_HC_JOBSCHEDULER_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		int intErr = getInternalLastError();	
		cout<<"intErr:"<<intErr<<endl; 
		cout<<"Error : Set implementer  for the object failure"<<endl;
		ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s"," Exiting acs_hcs_jobScheduler_ObjectImpl::svc Dispatch");
		return -1;
	}
	else
	{   
		cout << "acs_hcs_jobScheduler_ObjectImpl success" << endl;
		DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}
#endif
        
	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();

	DEBUG("%s"," Exiting acs_hcs_jobScheduler_ObjectImpl::svc Dispatch");
	return 0;
}//End of svc

/*===================================================================
   ROUTINE: shutdown
=================================================================== */
void acs_hcs_jobScheduler_ObjectImpl::shutdown()
{
	DEBUG("%s", "Entering acs_hcs_jobScheduler_ObjectImpl::shutdown");

        m_poReactor->end_reactor_event_loop();

	DEBUG("%s", "Calling removeClassImpl.");
	for (int i=0; i < 3; ++i)
	{
		if( theOiHandlerPtr != 0 )
                {
			if(  theOiHandlerPtr->removeClassImpl(this,ACS_HC_JOBSCHEDULER_CLASS_NAME) != ACS_CC_SUCCESS)
			{
				int intErr = getInternalLastError();
				if( intErr == -6 )  //Need to try again as IMM is busy at that moment
				{
					//for(int j=0; j< 100; ++j) ; //Waiting for sometime before retrying
					DEBUG("%s","Removing implementer for job scheduler failed with error code 6");
					sleep(0.2);
					continue;
				}
				else
				{
					DEBUG("%s","job scheduler implementer removed successfully");
					break;
				}
			}
			else
			{
				DEBUG("%s","Failed in removing job scheduler class implementer");
				break;
			}
		}
		else
                {
                        DEBUG("%s","pointer deleted before removing job scheduler class implementer ");
                        break;
                }
	}

        DEBUG("%s", "Leaving acs_hcs_jobScheduler_ObjectImpl::shutdown");
}
/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG("%s","Entering  acs_hcs_jobScheduler_ObjectImpl::create ");
	(void) oiHandle;
	(void) ccbId;
	(void) className;
	(void) parentName;
	(void) attr;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          acs_hcs_jobScheduler_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	DEBUG("%s","Leaving acs_hcs_jobScheduler_ObjectImpl::create");
	return ACS_CC_SUCCESS;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s","Entering acs_hcs_jobScheduler_ObjectImpl::deleted");

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_jobScheduler_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	string DN(objName);

	cout<<"Entering acs_hcs_jobScheduler_ObjectImpl::deleted callback"<<endl;
	
	DEBUG("%s","Leaving acs_hcs_jobScheduler_ObjectImpl::deleted"); 
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::modify");
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_jobScheduler_ObjectImpl ObjectModifyCallback invocated\n");
	DEBUG ("%s","-----------------------------------------------------\n");

	int i = 0;
	cout<<"Before while"<<endl;
        while( attrMods[i])
        {
                cout<<"attrMods[i]->modAttr.attrName :"<<attrMods[i]->modAttr.attrName<<endl;
                switch( attrMods[i]->modAttr.attrType)
                {

                        case ATTR_INT32T :
				if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, adminStateAttr) == 0)
				{
					cout<<"Updating admin state value"<<endl;
				       jobScheduler::eventCallbackFlag = true;	
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,schedulerStateAttr) == 0)
				{
					jobScheduler::eventCallbackFlag = true;
				}
			break;

			case ATTR_STRINGT :
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,nextScheduledTimeAttr)== 0 && (int)attrMods[i]->modAttr.attrValuesNum == 1)
				{
					//when job is ongoing nextschedule time should not be updated
					string jobSchedulerDN(objName); //jobSchedulerId=1,jobId=j1,HealthCheckhealthCheckMId=1
					acs_hcs_global_ObjectImpl obj;
					int jobSchedulerProgressState = obj.getProgressReportState(jobSchedulerDN);
					if(jobSchedulerProgressState == STATE_RUNNING ) 
							return ACS_CC_FAILURE;

					cout<<"updating the next scheduled time attribute"<<endl;
					jobScheduler::eventCallbackFlag = true;
				}
                        break;

                        default:
                        break;
                }
                i++;
       }

	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::modify");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_jobScheduler_ObjectImpl::complete"); 

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_jobScheduler_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	cout << "Complete returned" << endl;

    	DEBUG("%s","Leaving acs_hcs_jobScheduler_ObjectImpl::complete");

	return ACS_CC_SUCCESS;

}//End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_hcs_jobScheduler_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_jobScheduler_ObjectImpl::abort");

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_jobScheduler_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

    	DEBUG("%s","Leaving acs_hcs_jobScheduler_ObjectImpl::abort"); 
}//End of abort

/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_hcs_jobScheduler_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
   	 DEBUG("%s","Entering ACS_HC_Job_ObjectImpl::apply"); 

	(void)oiHandle;
	(void)ccbId;
	
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           ACS_HC_Job_ObjectImpl CcbApplyCallback invocated           \n");
    	DEBUG ("%s","---------------------------------------------------\n");
	//---Ecim Password starts
	DEBUG("%s","Leaving ACS_HC_Job_ObjectImpl::apply");
}//End of apply

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::updateRuntime");
    	(void) objName;
	(void) attrName;
        string object = idEquOne_Comma;
        object = object + objName;

        ACS_CC_ReturnType returnCode;
        ACS_CC_ImmParameter progressReportAttr;

        progressReportAttr.attrName = (char *)progressReport_Attr;
        progressReportAttr.attrType = ATTR_NAMET;
        progressReportAttr.attrValuesNum = 1;
        char* progressReport = const_cast<char*>(object.c_str());
        void* value1[1]={ reinterpret_cast<void*>(progressReport)};
        progressReportAttr.attrValues =  value1;

        returnCode = this->modifyRuntimeObj(objName, &progressReportAttr);
        if(returnCode == ACS_CC_FAILURE)
        {
                cout << "modify of result failed " << getInternalLastErrorText() << endl;
        }
        else
        {
                cout << "modify success" << endl;
        }

    	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_jobScheduler_ObjectImpl::updateRuntime");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_hcs_jobScheduler_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;
	
     	DEBUG("%s","Entering acs_hcs_jobScheduler_ObjectImpl::adminOperationCallback");
	cout<<"Entering acs_hcs_jobScheduler_ObjectImpl::adminOperationCallback"<<endl;
	
	DEBUG("%s","Leaving acs_hcs_jobScheduler_ObjectImpl::adminOperationCallback");
}//End of adminOperationCallback

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void acs_hcs_jobScheduler_ObjectImpl::setErrorCode(unsigned int code)
{
	DEBUG("%s","Entering acs_hcs_jobScheduler_ObjectImpl::setErrorCode ");
	cout<<"Entering acs_hcs_jobScheduler_ObjectImpl::setErrorCode "<<endl;
    setExitCode(code,errorText);
	DEBUG("%s","Leaving  acs_hcs_jobScheduler_ObjectImpl::setErrorCode ");
}

/*===================================================================
        ROUTINE: updationAfterJobExecution
=================================================================== */
void acs_hcs_jobScheduler_ObjectImpl::updationAfterJobExecution(string jobRDN,string executionTime)
{
	DEBUG("%s","Entering acs_hcs_jobScheduler_ObjectImpl::updationAfterJobExecution() ");

	//string temp_jobRDN = jobRDN;
	acs_hcs_jobScheduler_ObjectImpl::updation_after_execution = true ;
	
	acs_hcs_global_ObjectImpl obj;
	//string jobSchedulerId = JOBSCHEDULER ; //jobSchedulerId=1,
	//string jobSchedulerRDN = jobSchedulerId.append(jobRDN); //append jobscheduler and job rdn
	//cout<<"Job scheduler rdn for updationAfterJobExecution:: function is"<<jobSchedulerRDN<<endl;
	//string executionTime;
	string scheduledTime = "";

	size_t pos = jobRDN.find_first_of(COMMA_STR);
	string eventRDN = jobRDN.substr(0,pos+1); //parse jobRDN in the form job=jobName
	int returnCode;
	//executionTime = obj.getImmAttribute(jobSchedulerRDN,nextScheduledTimeAttr);

	int numDef;

	//check in all single events of the class which has lastScheduledDate=timeInSeconds
        std::vector<std::string> pd_dnList_single;
	std::vector<std::string>  status_pd_dnList ;
        returnCode = obj.getImmInstances(ACS_HC_SINGLEEVENT_CLASS_NAME, pd_dnList_single);
        if(returnCode == ACS_CC_SUCCESS)
        {
                numDef  = pd_dnList_single.size();
                for(int i = 0; i < numDef; i++)
                {
			if( jobScheduler::stopRequested == false )
			{
				int executionStatus = obj.getImmIntAttribute(pd_dnList_single[i],executionStatusAttr);
				if(executionStatus == NOTEXECUTED)
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
					scheduledTime = obj.getImmAttribute(status_pd_dnList[i],LASTSCHEDULEDATE);
					if( scheduledTime.substr(0,19) == executionTime.substr(0,19) )
					{
						string rdn = status_pd_dnList[i];
	                                        int executionStatus = EXECUTED;
        	                                void* value[1] = { reinterpret_cast<int*>(&executionStatus)};
                	                        obj.updateImmAttribute(rdn,executionStatusAttr,ATTR_INT32T,value);
					}			
                        	}
            		 }
			 else
				return; 
	 	}

	}
	else
	{
		cout<<"Error in getting single event instances"<<endl;
	}


	//check all periodic events
	std::vector<std::string> pd_dnList_periodic;
	returnCode = obj.getImmInstances(ACS_HC_PERIODICEVENT_CLASS_NAME, pd_dnList_periodic);
	if(returnCode == ACS_CC_SUCCESS)
	{
		numDef  = pd_dnList_periodic.size();
		for(int i = 0; i < numDef; i++)
		{
			if(jobScheduler::stopRequested == false )
			{
				size_t dnPos = pd_dnList_periodic[i].find(eventRDN);
				if(dnPos != std::string::npos)
				{
					scheduledTime = obj.getImmAttribute(pd_dnList_periodic[i],LASTSCHEDULEDATE);
					if( scheduledTime.substr(0,19) == executionTime.substr(0,19))
						obj.periodic_updationAfterJobExecution(pd_dnList_periodic[i]);
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

	//To check in calendar events	
        std::vector<std::string> pd_dnList_cal;
	returnCode = obj.getImmInstances(ACS_HC_CALENDERPERIODICEVENT_CLASS_NAME,pd_dnList_cal);
        if(returnCode == ACS_CC_SUCCESS)
        {
                numDef  = pd_dnList_cal.size();
                cout<<"claendar "<<numDef<<endl;
                for( int i = 0; i < numDef; i++)
                {
			if (jobScheduler::stopRequested == false )
			{
                        	size_t dnPos = pd_dnList_cal[i].find(eventRDN);
	                        if(dnPos != std::string::npos)
        	                {
          				scheduledTime = obj.getImmAttribute(pd_dnList_cal[i],LASTSCHEDULEDATE);
					if( scheduledTime.substr(0,19) == executionTime.substr(0,19) )
						obj.cal_updationAfterJobExecution(pd_dnList_cal[i]);
                        	}
			}
			else
				return;

                }
        }
        else
        {
                cout<<"Error in getting calendar event instances"<<endl;
        }

	jobRDN = "periodicEventId=1,jobSchedulerId=1," + jobRDN;//Dummy value need to provide in this format	
	obj.updateNextScheduledTime(jobRDN);

	acs_hcs_jobScheduler_ObjectImpl::updation_after_execution = false;

	DEBUG("%s","Leaving acs_hcs_jobScheduler_ObjectImpl::updationAfterJobExecution(s ");
}

/*===================================================================
        ROUTINE: getJobRdn_executionget
=================================================================== */
string acs_hcs_jobScheduler_ObjectImpl::getJobRdn_execution()
{
	//DEBUG("%s"," Entering  acs_hcs_jobScheduler_ObjectImpl::getJobRdn_execution()");
	acs_hcs_global_ObjectImpl obj;
	std::vector<std::string> pd_dnList;
	std::vector<jobToExecute> JobsList;
	int numDef = 0;
	string scheduled_job;

	int returnCode = -1;
	returnCode = obj.getImmInstances(ACS_HC_JOBSCHEDULER_CLASS_NAME,pd_dnList);
	if(returnCode == ACS_CC_SUCCESS)
	{
		numDef  = pd_dnList.size();
		for(int i = 0; i < numDef; i++)
		{
			int schedulerState_value = obj.getImmIntAttribute(pd_dnList[i],schedulerStateAttr);

			if(schedulerState_value == ENABLE )
			{
				string nextScheduleDate = obj.getImmAttribute(pd_dnList[i],nextScheduledTimeAttr);
				string complete_rdn = pd_dnList[i];
				size_t pos = complete_rdn.find_first_of(COMMA_STR);
				complete_rdn = complete_rdn.substr(pos+1,complete_rdn.size()); 
				jobToExecute job(nextScheduleDate,complete_rdn);
				JobsList.push_back(job);
			}
		}
		if(JobsList.size() == 0)
			return NOJOB;
		else
		{
			//sorting the vector to get least possible time
               		sort(JobsList.begin(),JobsList.end(),timeSort);
			scheduled_job = JobsList[0].job_rdn;
			//DEBUG("%s","return jobExecute -- Leaving  acs_hcs_jobScheduler_ObjectImpl::getJobRdn_execution()  ");
			return scheduled_job ;
		}
	}
	else
	{
		return NOJOB;
	}
}

/*===================================================================
        ROUTINE:timeSort 
=================================================================== */
bool acs_hcs_jobScheduler_ObjectImpl::timeSort(const  jobToExecute &a, const jobToExecute  &b)
{
	//DEBUG("%s","Entering acs_hcs_jobScheduler_ObjectImpl::timeSort(");
	acs_hcs_global_ObjectImpl obj;
	if(obj.compareDates(a.scheduleDate,b.scheduleDate) == true)
		return true;
	else
		return false;
}
