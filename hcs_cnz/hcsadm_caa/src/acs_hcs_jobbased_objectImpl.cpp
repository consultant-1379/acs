/*=================================================================== */
/**
   @file acs_hcs_jobbased_ObjectImpl.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012     XMALRAO       Initial Release
=====================================================================*/

/*==================================================================
                        INCLUDE DECLARATION SECTION
================================================================== */
#include <iostream>
#include <string>
#include <ace/Signal.h>
#include "acs_hcs_jobbased_objectImpl.h"
#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_progressReportHandler.h"
#include "acs_hcs_healthcheckservice.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_Util.H"
#include "acs_hcs_transferqueue.h"
#include "acs_hcs_global_ObjectImpl.h"
#include "acs_hcs_jobscheduler.h"
#include <ACS_CS_API.h>
using namespace std;

/*==================================================================
                STATIC VARIABLE DECLARATION SECTION
================================================================== */
bool job_modify_callback = false;
vector<string> acs_hcs_jobbased_ObjectImpl::jobList_create;

/*===================================================================
   ROUTINE: acs_hcs_jobbased_ObjectImpl
=================================================================== */
acs_hcs_jobbased_ObjectImpl::acs_hcs_jobbased_ObjectImpl(string ClassName,string szImpName,ACS_APGCC_ScopeT enScope ) 
:acs_apgcc_objectimplementereventhandler_V3(  ClassName,szImpName,enScope ), theClassName(ClassName)
{
	DEBUG("%s"," Entering acs_hcs_jobbased_ObjectImpl::acs_hcs_jobbased_ObjectImpl");
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

	DEBUG("%s"," Exiting acs_hcs_jobbased_ObjectImpl::acs_hcs_jobbased_ObjectImpl");
}//End of acs_hcs_jobbased_ObjectImpl

/*===================================================================
   ROUTINE: ~acs_hcs_jobbased_ObjectImpl
=================================================================== */
acs_hcs_jobbased_ObjectImpl::~acs_hcs_jobbased_ObjectImpl()
{
	DEBUG("%s"," Entering acs_hcs_jobbased_ObjectImpl::~acs_hcs_jobbased_ObjectImpl");
	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_HC_JOBBASED_CLASS_NAME);
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

	DEBUG("%s"," Exiting acs_hcs_jobbased_ObjectImpl::~acs_hcs_jobbased_ObjectImpl");
}//End of ~acs_hcs_jobbased_ObjectImpl

/*===================================================================
   ROUTINE: setImpl
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::setImpl()
{
	DEBUG("%s"," ");
#if 0
        for (int i=0; i < 10; ++i)
        {
                theOiHandlerPtr->removeClassImpl(this, ACS_HC_JOBBASED_CLASS_NAME);
                ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_JOBBASED_CLASS_NAME);

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
			{
                                continue;
			}
                }
                else
                {
                        DEBUG("Success: Set implementer %s for the object %s",getImpName().c_str(), getObjName().c_str());
                        m_isClassImplAdded = true;
                        return ACS_CC_SUCCESS;
                }
        }
#endif

	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_JOBBASED_CLASS_NAME);

        if ( errorCode == ACS_CC_FAILURE )
        {
                ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
                DEBUG("%s"," Exiting acs_hcs_jobbased_ObjectImpl::svc Dispatch");
                return ACS_CC_FAILURE;
        }
        else
        {
                cout << "acs_hcs_jobbased_ObjectImpl success" << endl;
                DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
        }

	DEBUG("%s"," ");
        return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 acs_hcs_jobbased_ObjectImpl::svc(  )
{
	DEBUG("%s"," Entering acs_hcs_jobbased_ObjectImpl::svc Dispatch");
# if 0
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_JOBBASED_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s"," Exiting acs_hcs_jobbased_ObjectImpl::svc Dispatch");
		return -1;
	}
	else
	{
		cout << "acs_hcs_jobbased_ObjectImpl success" << endl;
		DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}
      
#endif
 
	cout<<"My test 1"<<endl; 
	m_poReactor->open(1);
	cout<<"My test 2"<<endl;
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	cout<<"My test 3"<<endl;	
        m_poReactor->run_reactor_event_loop();
	cout<<"My test 4"<<endl;

	DEBUG("%s"," Exiting acs_hcs_jobbased_ObjectImpl::svc Dispatch");
	return 0;
}//End of svc

/*===================================================================
   ROUTINE: shutdown
=================================================================== */
void acs_hcs_jobbased_ObjectImpl::shutdown()
{
    	DEBUG("%s", "Entering acs_hcs_jobbased_ObjectImpl::shutdown");
    	cout << "In shutdown" << endl;

    	m_poReactor->end_reactor_event_loop();
	DEBUG("%s", "Calling removeClassImpl.");
	for (int i=0; i < 3; ++i)
	{
		if( theOiHandlerPtr != 0 )
		{
			if(  theOiHandlerPtr->removeClassImpl(this,ACS_HC_JOBBASED_CLASS_NAME) != ACS_CC_SUCCESS)
			{
				int intErr = getInternalLastError();
				if( intErr == -6 )  //Need to try again as IMM is busy at that moment
				{
					//for(int j=0; j< 100; ++j) ; //Waiting for sometime before retrying
					DEBUG("%s","Removing implementer for job failed with error code 6");
					sleep(0.2);
					continue;
				}
				else
				{
					DEBUG("%s","job implementer removed successfully");
					break;
				}
			}
			else
			{
				DEBUG("%s","Failure in removing job class implementer");	
				break;	
			}
		}
		else
		{
			DEBUG("%s","pointer deleted before removing job class implementer ");
			break;
		}
	}
    	DEBUG("%s", "Leaving acs_hcs_jobbased_ObjectImpl::shutdown");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::create ");
    	(void) oiHandle;
    	(void) ccbId;
    	(void) className;
    	(void) parentName;
    	(void) attr;

	cout<<"Entering acs_hcs_jobbased_ObjectImpl::create"<<endl;

	char target[32]		= {0};
	char transferQueue[128]	= {0};
	char jobToTrigger[128]	= {0};
	char categories[128]	= {0};
	char jobId[256];		
	memset(jobId,0,256);
	int i = 0;
	bool tq_created		= false; 		

	JOB_Parameters job_parameters ;  //:Malli
	memset(&job_parameters,0,sizeof(JOB_Parameters));

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          acs_hcs_jobbased_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	while( attr[i])
        {
		cout<<"attr[i]->attrName :"<<attr[i]->attrName<<endl;
                switch( attr[i]->attrType)
                {
	                case ATTR_STRINGT:
				if(ACE_OS::strcmp(attr[i]->attrName, jobToTriggerAttr) == 0 && attr[i]->attrValuesNum)
        	                {
                	                ACE_OS::strcpy(jobToTrigger, reinterpret_cast<char *>(attr[i]->attrValues[0]));
				}
				else if(ACE_OS::strcmp(attr[i]->attrName, categoriesAttr) == 0 && attr[i]->attrValuesNum)
        	                {
                	                ACE_OS::strcpy(categories, reinterpret_cast<char *>(attr[i]->attrValues[0]));
				}	
				else if(ACE_OS::strcmp(attr[i]->attrName, jobIdAttr) == 0 && attr[i]->attrValuesNum)
				{
                        	        ACE_OS::strcpy(jobId, reinterpret_cast<char *>(attr[i]->attrValues[0]));
					cout<<jobId<<endl;

					//check whether provided job name is "cache" or not
					int i;
					char temp[256];
					memset(temp,0,256);

					for( i=0; jobId[i]!='\0'; i++)
						temp[i] = (char)tolower(jobId[i]);
					temp[i]='\0';

					string jobname(temp);
					cout<<"job name is "<<jobname<<endl;
					string cache = "jobid=cache";
					if( strcmp(jobname.c_str(),cache.c_str())== 0)
					{
						strcpy(errorText,INVALID_JOBNAME);
						setErrorCode(ACS_HC_INVALID_JOBNAME);
						return ACS_CC_FAILURE;
					}
				}
				if(ACE_OS::strcmp(attr[i]->attrName, transferQueueAttr) == 0 && attr[i]->attrValuesNum)
                        	{
					tq_created = true;
			        	ACE_OS::strcpy(transferQueue, reinterpret_cast<char *>(attr[i]->attrValues[0]));
				}
				else if(ACE_OS::strcmp(attr[i]->attrName, targetAttr) == 0 && attr[i]->attrValuesNum)
                        	{
                                	ACE_OS::strcpy(target, reinterpret_cast<char *>(attr[i]->attrValues[0]));
				}
			break;

			default:	
			break;
		}
		i++;
	}

	strncpy(job_parameters.TransferQueue,transferQueue,sizeof(job_parameters.TransferQueue) - 1);
	strncpy(job_parameters.Target,target,sizeof(job_parameters.Target) - 1);
	strncpy(job_parameters.Categories,categories,sizeof(job_parameters.Categories) - 1);
	strncpy(job_parameters.JobToTrigger,jobToTrigger,sizeof(job_parameters.JobToTrigger) - 1);

	//we pass this value to apply call back
	string Id = jobId;
	string jobrdn = Id + COMMA_STR + ACS_HC_ID ;

        JOB_Parameters *request;
        request =  new JOB_Parameters(job_parameters);
	
	int retValue=createJobValidations(request);
	switch (retValue)
    	{
         	case ACS_HC_INVALID_TRIGGERJOB   :
         		strcpy(errorText,INVALID_JOB);
            		setErrorCode(ACS_HC_INVALID_TRIGGERJOB);
			if(request)
				delete request;
            		return ACS_CC_FAILURE;
										
         	case ACS_HC_INVALID_CATEGORY:
            		strcpy(errorText,INVALID_CATEGORY);
            		setErrorCode(ACS_HC_INVALID_CATEGORY);
			if(request)
				delete request;
            		return ACS_CC_FAILURE;

		case ACS_HC_INVALID_TARGET :
            		strcpy(errorText,INVALID_TARGET);
            		setErrorCode(ACS_HC_INVALID_TARGET);
		 	if(request)
                                delete request;
            		return ACS_CC_FAILURE;

         	case ACS_HC_INVALID_TQ :
            		strcpy(errorText,INVALID_TQ);
            		setErrorCode(ACS_HC_INVALID_TQ);
			if(request)
                                delete request;
            		return ACS_CC_FAILURE;
										
         	
	  	default:
                	break;
    	}

	bool isAugmentFail = false;
	if(tq_created)
        {
                string tq(transferQueue);
                acs_hcs_global_ObjectImpl obj;
                string tq_dn = obj.getTqDN(tq);

                //Implementing augmentation call for destination dn
                ACS_APGCC_CcbHandle ccbHandleVal;
                ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
                ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);
                if(retVal != ACS_CC_SUCCESS)
                {
                        cout<<"Augmentation initialization is failed "<<endl;
                        DEBUG("%s","Augmentation initialization is failed");
			isAugmentFail = true;
                }
                else
                {
                        ACS_CC_ImmParameter attributeRDN;
                        attributeRDN.attrName = (char*)transferQueueDNAttr;
                        attributeRDN.attrType = ATTR_STRINGT;
                        attributeRDN.attrValuesNum = 1;
                        char* rdnValue = const_cast<char*>(tq_dn.c_str());
                        void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
                        attributeRDN.attrValues = value2;

                        cout<<"provided job rdn is "<<jobrdn<<endl;
                        retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal,jobrdn.c_str(),&attributeRDN);
                        if(retVal != ACS_CC_SUCCESS)
                        {
                                cout<<"Augmentation create is failed "<<endl;
                                DEBUG("%s","Augmentation create is failed");
				isAugmentFail = true;
                        }
                        else
                        {
                                retVal = applyAugmentCcb (ccbHandleVal);
                                if(retVal != ACS_CC_SUCCESS)
                                {
                                        cout<<"Augmentation apply is failed "<<endl;
                                        DEBUG("%s","Augmentation apply is failed");
					isAugmentFail = true;
				}
                        }
                }
                //End of augmentation call
        }

	delete request;
	acs_hcs_jobbased_ObjectImpl::jobList_create.push_back(jobrdn);

	if(isAugmentFail)
        {
                strcpy(errorText,"Augmentation failure for the event");
                setErrorCode(ACS_HC_AUGMENT_FAILED);
                return ACS_CC_FAILURE;
        }
	
	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::create");
	return ACS_CC_SUCCESS;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::deleted");

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_jobbased_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	cout<<"Entering acs_hcs_jobbased_ObjectImpl::deleted callback"<<endl;
	string jobDN(objName);

	int deleteFlag = deleteJobValidations(objName);
	switch(deleteFlag)
	{
		case ACS_HC_JOB_ONGOING :
			strcpy(errorText,JOB_ONGOING);
            		setErrorCode(ACS_HC_JOB_ONGOING);
            		return ACS_CC_FAILURE;
			
		case ACS_HC_JOBAS_TRIGGERJOB :
            		strcpy(errorText,JOBAS_TRIGGERJOB);
            		setErrorCode(ACS_HC_JOBAS_TRIGGERJOB);
            		return ACS_CC_FAILURE;
			
		case ACS_HC_SCHEDULER_UNLOCK :
            		strcpy(errorText,SCHEDULER_UNLOCK);
            		setErrorCode(ACS_HC_SCHEDULER_UNLOCK);
            		return ACS_CC_FAILURE;
			
		default :
			break;
	}

	jobScheduler::eventCallbackFlag = true;
	job_modify_callback = true;

	DEBUG ("%s","before deleting Progress report for");

	//deleting the progress report for job
	for(unsigned int it = 0; it < acs_hcs_progressReport_ObjectImpl::progressReport.size(); it++)
	{
		cout<<"In for loop"<<endl;
		if(acs_hcs_progressReport_ObjectImpl::progressReport[it].DN == jobDN)
		{
			cout<<"Entering into for loop"<<endl;

			for (int i=0; i < 10; ++i)
			{
				if(acs_hcs_progressReport_ObjectImpl::progressReport[it].impl->finalize() != ACS_CC_SUCCESS)
				{
					int intErr = getInternalLastError();
					cout<<"Finalize failed for job deletion with error "<<intErr<<endl;
					if ( intErr == -6)
					{
						for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
						continue;
					}
					else
						break;
				}
				else
				{
					cout<<"In job based else loop"<<endl;
					acs_hcs_progressReport_ObjectImpl::progressReport[it].handler->stop();
					ACE_OS::sleep(1);
					delete acs_hcs_progressReport_ObjectImpl::progressReport[it].impl;
					acs_hcs_progressReport_ObjectImpl::progressReport[it].impl = 0;
					delete acs_hcs_progressReport_ObjectImpl::progressReport[it].handler;
					acs_hcs_progressReport_ObjectImpl::progressReport[it].handler = 0;
					acs_hcs_progressReport_ObjectImpl::progressReport.erase(acs_hcs_progressReport_ObjectImpl::progressReport.begin()+it);
					break;
				}
			}
			break;
		}
	}

	 //deleting the progress report for job scheduler
	jobDN = (string)JOBSCHEDULER + jobDN;	
	for(unsigned int it = 0; it < acs_hcs_progressReport_ObjectImpl::progressReport.size(); it++)
        {
                if(acs_hcs_progressReport_ObjectImpl::progressReport[it].DN == jobDN)
		{
			for (int i=0; i < 10; ++i)
                        {
                                if(acs_hcs_progressReport_ObjectImpl::progressReport[it].impl->finalize() != ACS_CC_SUCCESS)
                                {
                                        int intErr = getInternalLastError();
                                        cout<<"Finalize failed for job deletion with error "<<intErr<<endl;
                                        if ( intErr == -6)
                                        {
                                                for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
                                                continue;
                                        }
                                        else
                                                break;
                                }
                                else
                                {
                                        cout<<"In job based else loop"<<endl;
                                        acs_hcs_progressReport_ObjectImpl::progressReport[it].handler->stop();
                                        ACE_OS::sleep(1);
                                        delete acs_hcs_progressReport_ObjectImpl::progressReport[it].impl;
                                        acs_hcs_progressReport_ObjectImpl::progressReport[it].impl = 0;
                                        delete acs_hcs_progressReport_ObjectImpl::progressReport[it].handler;
                                        acs_hcs_progressReport_ObjectImpl::progressReport[it].handler = 0;
                                        acs_hcs_progressReport_ObjectImpl::progressReport.erase(acs_hcs_progressReport_ObjectImpl::progressReport.begin()+it);
                                        break;
                                }
                        }
                        break;
		}
	}
	
	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::deleted"); 
	
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
   	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::modify");
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_jobbased_ObjectImpl ObjectModifyCallback invocated           \n");
	DEBUG ("%s","-----------------------------------------------------\n");

	job_modify_callback = true;	
	char triggerJob[128]		= {0};
	char categories[128]		= {0};
	char target[32]			= {0};
	char transferQueue[128]		= {0};
	bool tq_modify_flag = false;
	bool category_flag = false;
	
	int i = 0;
	JOB_Parameters mod_parameters;  
	memset(&mod_parameters,0,sizeof(JOB_Parameters));

	while( attrMods[i] )
    	{
		cout<<"attrMods[i]->modAttr.attrName :"<<attrMods[i]->modAttr.attrName<<endl;
		switch ( attrMods[i]->modAttr.attrType )
        	{
			case ATTR_STRINGT:
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, categoriesAttr) == 0 )
                		{
					ACE_OS::strcpy(categories, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					category_flag = true;
				}

				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, jobToTriggerAttr) == 0 )
                		{
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						ACE_OS::strcpy(triggerJob,EMPTY_STR);
					}
					else
					{
						ACE_OS::strcpy(triggerJob, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					}
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, targetAttr) == 0)
                		{
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						ACE_OS::strcpy(target,EMPTY_STR);
					}
					else
					{
                    				ACE_OS::strcpy(target, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					}
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, transferQueueAttr) == 0 )
        		        {
					tq_modify_flag = true;
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						ACE_OS::strcpy(transferQueue,"");
					}
					else
					{
						ACE_OS::strcpy(transferQueue, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					}
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,LASTREPORTNAME ) == 0 )
				{
					return	ACS_CC_SUCCESS;
				} 

			break;

			default: 
			break;
		    }
		i++;
	}

	strncpy(mod_parameters.Categories,categories,sizeof(mod_parameters.Categories) - 1);
	strncpy(mod_parameters.JobToTrigger,triggerJob,sizeof(mod_parameters.JobToTrigger) - 1);
	strncpy(mod_parameters.TransferQueue,transferQueue,sizeof(mod_parameters.TransferQueue) - 1);
	strncpy(mod_parameters.Target,target,sizeof(mod_parameters.Target) - 1);

	JOB_Parameters *request;
	request =  new JOB_Parameters(mod_parameters); 

	int retValue=createJobValidations( request,category_flag, objName);
	switch (retValue)
    	{
        	case ACS_HC_INVALID_TRIGGERJOB   :
            		strcpy(errorText,INVALID_JOB);
            		setErrorCode(ACS_HC_INVALID_TRIGGERJOB);
			if(request)
				delete request;
            		return ACS_CC_FAILURE;
										
         	case ACS_HC_INVALID_CATEGORY:
            		strcpy(errorText,INVALID_CATEGORY);
            		setErrorCode(ACS_HC_INVALID_CATEGORY);
			if(request)
                                delete request;
            		return ACS_CC_FAILURE;

		case ACS_HC_INVALID_TARGET :
            		strcpy(errorText,INVALID_TARGET);
            		setErrorCode(ACS_HC_INVALID_TARGET);
			if(request)
				delete request;
            		return ACS_CC_FAILURE;

         	case ACS_HC_INVALID_TQ :
           		strcpy(errorText,INVALID_TQ);
            		setErrorCode(ACS_HC_INVALID_TQ);
			 if(request)
                                delete request;
            		return ACS_CC_FAILURE;

		case ACS_HC_JOB_ONGOING :
                        strcpy(errorText,JOB_ONGOING);
                        setErrorCode(ACS_HC_JOB_ONGOING);
			if(request)
                                delete request;
                        return ACS_CC_FAILURE;

		default :
		break;
	}

	bool isAugmentFail = false;
	if(tq_modify_flag)
        {
                string tq(transferQueue);
                acs_hcs_global_ObjectImpl obj;
                string tq_dn = obj.getTqDN(tq);

                //Implementing augmentation call for destination dn
                ACS_APGCC_CcbHandle ccbHandleVal;
                ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

                ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);
                if(retVal != ACS_CC_SUCCESS)
                {
                        cout<<"Augmentation initialization is failed "<<endl;
                        DEBUG("%s","Augmentation initialization is failed");
			isAugmentFail = true;	
                }
                else
                {
                        ACS_CC_ImmParameter attributeRDN;
                        attributeRDN.attrName = (char*)transferQueueDNAttr;
                        attributeRDN.attrType = ATTR_STRINGT;
                        attributeRDN.attrValuesNum = 1;
                        char* rdnValue = const_cast<char*>(tq_dn.c_str());
                        void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
                        attributeRDN.attrValues = value2;

                        cout<<"provided job rdn is "<<objName<<endl;
                        retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal,objName,&attributeRDN);
                        if(retVal != ACS_CC_SUCCESS)
                        {
                                cout<<"Augmentation create is failed "<<endl;
                                DEBUG("%s","Augmentation create is failed");
				isAugmentFail = true;
                        }
                        else
                        {
                                retVal = applyAugmentCcb (ccbHandleVal);
                                if(retVal != ACS_CC_SUCCESS)
                                {
                                        cout<<"Augmentation apply is failed "<<endl;
					DEBUG("%s","Augmentation apply is failed");
					isAugmentFail = true;
                                }
                        }
                }
                //End of augmentation call
        }

	delete request;

	if(isAugmentFail)
        {
                strcpy(errorText,"Augmentation failure for the event");
                setErrorCode(ACS_HC_AUGMENT_FAILED);
                return ACS_CC_FAILURE;
        }

    	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::modify");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::complete"); 

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_jobbased_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	cout << "Complete returned" << endl;
	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::complete");

	return ACS_CC_SUCCESS;

}//End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_hcs_jobbased_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::abort");

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_jobbased_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::abort"); 
}//End of abort

/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_hcs_jobbased_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering ACS_HC_Job_ObjectImpl::apply"); 

	(void)oiHandle;
	(void)ccbId;

	job_modify_callback = false;

	DEBUG ("%s","\n---------------------------------------------------\n");
    	DEBUG ("%s","           ACS_HC_Job_ObjectImpl CcbApplyCallback invocated           \n");
    	DEBUG ("%s","---------------------------------------------------\n");
	DEBUG("%s","Leaving ACS_HC_Job_ObjectImpl::apply");
}//End of apply

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::updateRuntime");
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
    	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_jobbased_ObjectImpl::updateRuntime");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_hcs_jobbased_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
     	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::adminOperationCallback");
	(void) oiHandle;
	(void) invocation;
	(void) operationId;
	(void) paramList;
	(void) p_objName;
	void* executeArg;
	string DN;
	char tempRDN[256]={0};

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_jobbased_ObjectImpl adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	int myOIValidationError=1;
	switch (operationId)
	{
		case EXECUTE :
			cout<<"admin operation EXECUTE"<<endl;
			DEBUG("%s","Admin operation EXECUTE");
			DN = p_objName;
			strcpy(tempRDN, DN.c_str());
			executeArg = reinterpret_cast<void*>(tempRDN);

                        if (ACE_Thread_Manager::instance ()->spawn(ACE_THR_FUNC (&AcsHcs::HealthCheckService::execute),
                                                                        executeArg,
                                                                        THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
                                                                        0,
                                                                        0,
                                                                        ACE_DEFAULT_THREAD_PRIORITY,
                                                                        -1,
                                                                        0,
                                                                        ACE_DEFAULT_THREAD_STACKSIZE,
                                                                        0) == -1)
                        {
                                cout << "Error creating thread execute" << endl;
				DEBUG("%s","Error creating thread execute");
                        }

	     		break;

		default:
			DEBUG("%s","Operation ID is received by acs_hcs_jobbased_ObjectImpl::saImmOiAdminOperationCallback is not valid");
			myOIValidationError = -1;
		break;
	}

	ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError);
        if(rc != ACS_CC_SUCCESS)
        {
		cout<<"Failure occurred in sending AdminOperation Result"<<endl;
            	ERROR("%s","Failure occurred in sending AdminOperation Result");
            	ERROR("%s","Exiting acs_hcs_jobbased_ObjectImpl::saImmOiAdminOperationCallback");
            	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::adminOperationCallback");
		return;
        }
	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::adminOperationCallback");
}//End of adminOperationCallback

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void acs_hcs_jobbased_ObjectImpl::setErrorCode(unsigned int code)
{
	DEBUG("%s"," Entering acs_hcs_jobbased_ObjectImpl::setErrorCode");
    	setExitCode(code,errorText);
	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::setErrorCode ");
}

/*======================================================================
		ROUTINE : deleteJobValidations 
========================================================================*/
ACE_INT32 acs_hcs_jobbased_ObjectImpl::deleteJobValidations(const char* objName)
{
	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::deleteJobValidations");
	acs_hcs_global_ObjectImpl obj;
	int returnCode;
        std::vector<std::string> pd_dnList;

	//validate whether the job is using as trigger job in other jobs
	string temp = objName;
        size_t commaPos = temp.find_first_of(COMMA_STR);
        size_t equalPos = temp.find_first_of(EQUALTO_STR);
        string jobName = temp.substr(equalPos+1,commaPos-equalPos-1);
	string jobToTriggerValue;
	returnCode = obj.getImmInstances(ACS_HC_JOBBASED_CLASS_NAME, pd_dnList);
        if(returnCode == ACS_CC_SUCCESS)
        {
                 int numDef  = pd_dnList.size();
                 int i;
		 for( i = 0; i < numDef; i++)
                 {
			jobToTriggerValue = obj.getImmAttribute(pd_dnList[i].c_str(),jobToTriggerAttr);
			if(strcmp(jobToTriggerValue.c_str(),EMPTY_STR) != 0)
			{
				if(!strcmp(jobToTriggerValue.c_str(), jobName.c_str()))
				{
			      		break;
				}
			}
                 }
                 if(i != numDef)
                 {
			DEBUG("%s","return from acs_hcs_jobbased_ObjectImpl::deleteJobValidations ");
                         return ACS_HC_JOBAS_TRIGGERJOB;
                 }
         }
         else
         {
		DEBUG("%s","IMM modify attriute fauilure in acs_hcs_jobbased_ObjectImpl::deleteJobValidations ");
                 cout<<"Unable to get trigger job instances"<<endl;
         }
	//End of job to trigger Validation

	//Validate whether the admin state is locked or not
	 string schedulerObj = objName ;
         schedulerObj = JOBSCHEDULER + schedulerObj; 
 
	int admState = obj.getImmIntAttribute(schedulerObj,adminStateAttr);
	if(admState != 0)
       	{
		return ACS_HC_SCHEDULER_UNLOCK;
       	}
		DEBUG("%s"," IMM Modify attribute failure in acs_hcs_jobbased_ObjectImpl::deleteJobValidations ");
			DEBUG("%s","return from acs_hcs_jobbased_ObjectImpl::deleteJobValidations");
	DEBUG("%s","After admin state check in acs_hcs_jobbased_ObjectImpl::deleteJobValidations ");
        //End of admin state validation
		 
	//Validate whether job is ongoing or not
	string jobDN(objName);
	string jobSchedulerDN = JOBSCHEDULER + jobDN ;
	int jobProgressState = obj.getProgressReportState(jobDN);
	int jobSchedulerProgressState = obj.getProgressReportState(jobSchedulerDN);
	if( jobProgressState == STATE_RUNNING || jobSchedulerProgressState == STATE_RUNNING || AcsHcs::HealthCheckService::sameJob == jobName)
	{
		return ACS_HC_JOB_ONGOING;
       	} 
	//End of JOB ONGOING validation

	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::deleteJobValidations");
	return ACS_CC_SUCCESS;
}

/*======================================================================
		ROUTINE : CreateJobValidations 
========================================================================*/
ACE_INT32 acs_hcs_jobbased_ObjectImpl::createJobValidations(JOB_Parameters *request,bool category_flag, const char* dn)
{
	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::createJobValidations");

	acs_hcs_global_ObjectImpl obj;
        int returnCode;
        OmHandler immHandler;
        std::vector<std::string> pd_dnList;

	if(strcmp(dn,"")!=0) //This block of code will get called in case of modify only
	{
		string jobDN(dn);
		string jobSchedulerDN = JOBSCHEDULER + jobDN ;
		int jobProgressState = obj.getProgressReportState(jobDN);
		int jobSchedulerProgressState = obj.getProgressReportState(jobSchedulerDN);
	        string temp = dn;
	        size_t commaPos = temp.find_first_of(COMMA_STR);
        	size_t equalPos = temp.find_first_of(EQUALTO_STR);
	        string jobName = temp.substr(equalPos+1,commaPos-equalPos-1);
	
		if( jobProgressState == STATE_RUNNING || jobSchedulerProgressState == STATE_RUNNING || AcsHcs::HealthCheckService::sameJob == jobName)
		{
			return ACS_HC_JOB_ONGOING;
		}
	}	

	//validations for transfer queue
	if(strcmp(request->TransferQueue,EMPTY_STR )!=0)
	{
        	ACE_INT32 errcode;
	        AES_OHI_ExtFileHandler2* mySenderFile;
        	mySenderFile = new AES_OHI_ExtFileHandler2("ACS","acs_hcd");
	
	        switch (errcode = mySenderFile->attach()) //attach
        	{
        		case AES_OHI_NOPROCORDER:
                		delete mySenderFile;
        	                strcpy(errorText,TQ_NT_DEFINED);
                	        setErrorCode(ACS_HC_INVALID_TQ);
                        	return ACS_HC_INVALID_TQ;
			case AES_OHI_NOSERVERACCESS:
				cout << "No AFP server access" << endl;
				strcpy(errorText,AFP_NO_ACCESS);
				setErrorCode(ACS_HC_RC_NOAFPACESS);
				delete mySenderFile;
				return ACS_HC_INVALID_TQ;
			case AES_OHI_NOERRORCODE:
				cout << "TQ defined" << endl;
				break;
			default:
				strcpy(errorText,INTERNAL_ERR_CODE);
				setErrorCode(ACS_HC_RC_INTPROGERR);
				delete mySenderFile;
				DEBUG("%s","return from acs_hcs_jobbased_ObjectImpl::createJobValidations ");
				return ACS_HC_INVALID_TQ;
		}

		//check that transfer queue is defined
		switch (mySenderFile->fileTransferQueueDefined(request->TransferQueue))
		{
			case AES_OHI_NOERRORCODE:
			    cout << "TQ defined" << endl;
			    break;
			case AES_OHI_NOSERVERACCESS:
			    cout << "No AFP server access" << endl;
			    strcpy(errorText,AFP_NO_ACCESS);
			    setErrorCode(ACS_HC_RC_NOAFPACESS);
			    delete mySenderFile;
			    return ACS_HC_INVALID_TQ;
			default:
			    cout << "TQ not defined" << endl;
			    strcpy(errorText,TQ_NT_DEFINED);
			    setErrorCode(ACS_HC_RC_TQUNDEF);
			    delete mySenderFile;
			    return ACS_HC_INVALID_TQ;
		}
		mySenderFile->detach(); //detach
	        delete mySenderFile;
	}
	//End of tq validations

	// Validate the target
	if(strcmp(request->Target,EMPTY_STR)!=0)
	{
		acs_hcs_global_ObjectImpl obj;
		unsigned int cpType;
		cpType = obj.getImmIntAttribute(AXEFUNCTIONS,systemTypeAttr);
		cout<<" cpType is "<<cpType<<endl;

		if(cpType == 0)
			return ACS_HC_INVALID_TARGET;
		//multi cp type
		else
		{
			std::vector<std::string> cpNames;
			std::vector<std::string> cpGrp;
			string target(request->Target);
			unsigned int cpId;
			bool cpFlag;
			cpFlag=false;
			getCPIDs(cpNames);
			getCPGrps(cpGrp);
			cpNames.insert( cpNames.end(), cpGrp.begin(), cpGrp.end() );
			for(cpId = 0; cpId < cpNames.size(); cpId++)	
			{
				cout << "cpNames: " << cpNames[cpId] << endl;
				if(target == cpNames[cpId])
				{
					cpFlag=true;
					break;
				}
			}


			if( cpId == cpNames.size())
			{
				DEBUG("%s","return from acs_hcs_jobbased_ObjectImpl::createJobValidations ");
				return ACS_HC_INVALID_TARGET;
			}
		}
	}
	//end of target Validation
		
	//validation for categories	
	if(category_flag == true)
	{	
		vector<string> dnList;
		string job_categories(request->Categories);
                job_categories = obj.removeTrailingSpaces(job_categories);
                //To check if only spaces are entered for category
                if(job_categories.size() == 0)
                {
			DEBUG("%s"," return from acs_hcs_jobbased_ObjectImpl::createJobValidations ");
                        //only spaces entered
                        return ACS_HC_INVALID_CATEGORY;
                }

		if(!(job_categories == ALL_CATEGORIES))
		{
			set<string> category;
			char *temp = strtok(const_cast<char *>(job_categories.c_str()),COMMA_STR);
			while(temp != NULL)
			{
				string id = categoriesAttr_with_EQU;
				id = id + temp;
				category.insert(id);
				temp = strtok(NULL,COMMA_STR);
			}

			returnCode = obj.getImmInstances(ACS_HC_CATEGORY_CLASS_NAME, dnList);
			if(returnCode == ACS_CC_SUCCESS)
			{
				int numDef  = dnList.size();

				if(numDef == 0)
				{
					DEBUG("%s","return from createValidations in Job ");
					return ACS_HC_INVALID_CATEGORY;
				}

				int i;
				for(set<string>::iterator it = category.begin(); it != category.end(); ++it)
				{
					for( i = 0; i < numDef; i++)
					{
						string jobCategory = obj.getImmAttribute(dnList[i].c_str(),categoriesIdAttr);
						if(!(strcmp(jobCategory.c_str(), (*it).c_str())))
						{
							break;
						}
					}
					if(i== numDef)
					{
						return ACS_HC_INVALID_CATEGORY;
					}
				}
			}
			else
			{
					return ACS_HC_INVALID_CATEGORY;
			}
		}
		else
		{
			returnCode = obj.getImmInstances(ACS_HC_CATEGORY_CLASS_NAME, dnList);
			if(returnCode == -41)
			{
			 	DEBUG("%s","return from acs_hcs_jobbased_ObjectImpl::createJobValidations ");
			      	return ACS_HC_INVALID_CATEGORY;	
			}
		}
	}
	//validation for categories is over

	//Validation for trigger job
	if(strcmp(request->JobToTrigger, EMPTY_STR) != 0 )
	{		
		vector<string> jobList;
		string rdn_jobToTrigger(request->JobToTrigger);
		rdn_jobToTrigger = (string)jobIdAttr + EQUALTO_STR + rdn_jobToTrigger ;
		
		returnCode = obj.getImmInstances(ACS_HC_JOBBASED_CLASS_NAME, jobList);
	    	if(returnCode == ACS_CC_SUCCESS)
		{
			int numDef  = jobList.size();
			int i;
	        	for( i = 0; i < numDef; i++)
		    	{
				string job = obj.getImmAttribute(jobList[i].c_str(),jobIdAttr);
				if ( job  == rdn_jobToTrigger )
				{
					break;
				}
			}
			if(i== numDef)
			{
				DEBUG("%s"," return from acs_hcs_jobbased_ObjectImpl::createJobValidations");
				return ACS_HC_INVALID_TRIGGERJOB;
			}
		}
		else
		{
			DEBUG("%s","IMM modify attributr failure in acs_hcs_jobbased_ObjectImpl::createJobValidations ");
			return ACS_HC_INVALID_TRIGGERJOB;  //No jobs defined
		}
	}
	//end of trigger job validation

	DEBUG("%s","Leaving acs_hcs_jobbased_ObjectImpl::createJobValidations");	
	return ACS_CC_SUCCESS;
}

/*======================================================================
                ROUTINE : getCPIDs
========================================================================*/
void acs_hcs_jobbased_ObjectImpl::getCPIDs(std::vector<std::string>& cpNames)
{
	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::getCPIDs(std::vector<std::string>& cpNames) ");
	ACS_CS_API_CP * cp = 0;
	try
	{
		cp = ACS_CS_API::createCPInstance();
		if (!cp)
		{
		}
		else
		{
			cout << "createCPInstance failed" << endl;
		}
	}
	catch(...)
	{
		cout << "Exception caught" << endl;
	}

	ACS_CS_API_IdList cpList;
	int result;

	try
	{
		result = (int) cp->getCPList(cpList);
	}
	catch (...)
	{
		cout << "getCPList failed" << endl;
	}

	try
	{
		ACS_CS_API::deleteCPInstance(cp);
	}
	catch (...)
	{
		cout << "deleteCPInstance failed 1" << endl;
	}

	if (result != 0)
	{
		cout << "getCPList failed to get IDs" << endl;
		try
		{
			ACS_CS_API::deleteCPInstance(cp);
		}
		catch(...)
		{
			cout << "deleteCPInstance failed 2" << endl;
		}
	}
	else cout << "Got the CP IDs " << cpList.size() << endl;

	unsigned cpId = 0;
	char cpName[200];
	for (unsigned i = 0; (i < cpList.size()); i++)
	{
		int j=i;
		cpId = cpList[j++];
		
		if (((int)cpId >= 0) && (cpId < 1000)) // Blades
		{
			char strCpId[6];
			sprintf(strCpId,"%u",cpId);
			strcpy(cpName, BC_STR);
			strcat(cpName, strCpId);
			cpNames.push_back(cpName);
		}
		else if ((cpId >= 1000) && (cpId < 2000)) // CP
		{
			cpId = cpId - 1000;
			char strCpId[6];
			sprintf(strCpId,"%u",cpId);
			strcpy(cpName, CP_STR);
			strcat(cpName, strCpId);
			cpNames.push_back(cpName);
		}
	}
	DEBUG("%s"," Leaving acs_hcs_jobbased_ObjectImpl::getCPIDs(std::vector<std::string>& cpNames)");	
}
/*======================================================================
                ROUTINE : getCPGrps
========================================================================*/
void acs_hcs_jobbased_ObjectImpl::getCPGrps(std::vector<std::string>& cpGrp)
{
	DEBUG("%s","Entering acs_hcs_jobbased_ObjectImpl::getCPGrps(std::vector<std::string>& cpNames) ");
        ACS_CS_API_CPGroup * cpG = 0;
	try
        {
                if ((cpG = ACS_CS_API::createCPGroupInstance()) != NULL)
		{
		}
                else
		{
                        cout << "createCPGroupInstance failed" << endl;
                }
        }
        catch(...)
        {
                cout << "Exception caught" << endl;
        }

        ACS_CS_API_NameList nameList;
	int result;
        try
        {
                result =  (int) cpG->getGroupNames(nameList);
        }
        catch (...)
        {
                cout << "getGroupNames failed" << endl;
        }

        try
        {
                ACS_CS_API::deleteCPGroupInstance(cpG);
        }
        catch (...)
        {
                cout << "deleteCPGroupInstance failed 1" << endl;
        }

        if (result != 0)
        {
                cout << "getgroupName failed to get IDs" << endl;
                try
                {
                        ACS_CS_API::deleteCPGroupInstance(cpG);
                }
                catch(...)
                {
                        cout << "deleteCPgroupInstance failed 2" << endl;
                }
        }
        else cout << "Got the CP Groups " << nameList.size() << endl;

        for (unsigned int i = 0; (i < nameList.size()); i++)
        {
		ACS_CS_API_Name cpGroupName = nameList[i];
		size_t nameLength = cpGroupName.length();
                char* groupNameChar = new char[nameLength];
		cpGroupName.getName(groupNameChar, nameLength);
		string groupNameString = groupNameChar;
		cout<<"CPgroups are "<<groupNameString<<endl;
		if(groupNameString!="ALL")
		{
			cpGrp.push_back(groupNameString);
		}
		delete []groupNameChar;

        }
        DEBUG("%s"," Leaving acs_hcs_jobbased_ObjectImpl::getCPGrps(std::vector<std::string>& cpNames)");
}
