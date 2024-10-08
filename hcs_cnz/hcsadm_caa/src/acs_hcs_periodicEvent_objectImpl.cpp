/*=================================================================== */
/**
   @file acs_hcs_periodicEvent_ObjectImpl.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A			     XMALRAO       Initial Release
====================================================================*/

/*==================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <ace/Signal.h>
#include "acs_hcs_periodicEvent_objectImpl.h"
#include "acs_hcs_jobScheduler_objectImpl.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_apgcc_omhandler.h"
#include <ctime>

using namespace std;

/*==================================================================
               STATIC VARIALE DECLARATION SECTION
=================================================================== */
string acs_hcs_periodicEvent_ObjectImpl::completePeventRdn = EMPTY_STR;
bool acs_hcs_periodicEvent_ObjectImpl::delete_Pevent_flag = false;
string acs_hcs_periodicEvent_ObjectImpl::newDn = "";
vector<eventDetails> create_periodic_event = vector<eventDetails>();
vector<eventDetails> modify_periodic_event = vector<eventDetails>();

/*===================================================================
   ROUTINE: acs_hcs_periodicEvent_ObjectImpl
=================================================================== */
acs_hcs_periodicEvent_ObjectImpl::acs_hcs_periodicEvent_ObjectImpl(string ClassName,string szImpName,ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3(  ClassName,szImpName,enScope ),theClassName(ClassName)
{
	memset(errorText,0,100);
	DEBUG("%s"," Entering acs_hcs_periodicEvent_ObjectImpl::acs_hcs_periodicEvent_ObjectImpl");
	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	g_ptr = new acs_hcs_global_ObjectImpl();
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
	if(g_ptr == 0)
	{
		ERROR("%s", "Memory allocation failed for  acs_hcs_global_ObjectImpl");
	}

	DEBUG("%s"," Exiting acs_hcs_periodicEvent_ObjectImpl::acs_hcs_periodicEvent_ObjectImpl");
}//End of acs_hcs_periodicEvent_ObjectImpl

/*===================================================================
   ROUTINE: ~acs_hcs_periodicEvent_ObjectImpl
=================================================================== */
acs_hcs_periodicEvent_ObjectImpl::~acs_hcs_periodicEvent_ObjectImpl()
{
	DEBUG("%s"," Entering acs_hcs_periodicEvent_ObjectImpl::~acs_hcs_periodicEvent_ObjectImpl");
	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_HC_PERIODICEVENT_CLASS_NAME);
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
		if( g_ptr != 0 )
        {
                delete g_ptr;
                g_ptr = 0;
        }

	DEBUG("%s"," Exiting acs_hcs_periodicEvent_ObjectImpl::~acs_hcs_periodicEvent_ObjectImpl");
}//End of ~acs_hcs_periodicEvent_ObjectImpl

/*===================================================================
   ROUTINE: setImpl
=================================================================== */
ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::setImpl()
{
	DEBUG("%s","entering ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::setImpl() ");
        ACS_CC_ReturnType errorCode = ACS_CC_SUCCESS;

	errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_PERIODICEVENT_CLASS_NAME);

        if ( errorCode == ACS_CC_FAILURE )
        {
                cout << "Failed " << getInternalLastError() << endl;
                int intErr = getInternalLastError();
                cout<<"intErr:"<<intErr<<endl;
                cout<<"Error : Set implementer  for the object failure"<<endl;
                ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
                DEBUG("%s"," Exiting acs_hcs_periodicEvent_ObjectImpl::svc Dispatch");
                return ACS_CC_FAILURE;
        }
        else
        {
                cout << "acs_hcs_periodicEvent_ObjectImpl success" << endl;
                DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
        }

	DEBUG("%s","leaving ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::setImpl() ");
        return errorCode;
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 acs_hcs_periodicEvent_ObjectImpl::svc(  )
{
	DEBUG("%s"," Entering acs_hcs_periodicEvent_ObjectImpl::svc Dispatch");
      
	m_poReactor->open(1);
    	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
    	m_poReactor->run_reactor_event_loop();

	DEBUG("%s"," Exiting acs_hcs_periodicEvent_ObjectImpl::svc Dispatch");
	return 0;
}//End of svc

/*===================================================================
   ROUTINE: shutdown
=================================================================== */
void acs_hcs_periodicEvent_ObjectImpl::shutdown()
{
    	DEBUG("%s", "Entering acs_hcs_periodicEvent_ObjectImpl::shutdown");

    	m_poReactor->end_reactor_event_loop();
	//remove class implementer
	DEBUG("%s", "Calling removeClassImpl.");
	for (int i=0; i < 3; ++i)
	{
		if( theOiHandlerPtr != 0 )
                {
			if(  theOiHandlerPtr->removeClassImpl(this,ACS_HC_PERIODICEVENT_CLASS_NAME) != ACS_CC_SUCCESS)
			{
				int intErr = getInternalLastError();
				if( intErr == -6 )  //Need to try again as IMM is busy at that moment
				{
					//for(int j=0; j< 100; ++j) ; //Waiting for sometime before retrying
					DEBUG("%s","removing periodic event implementer failed with error code 6");
					sleep(0.2);
					continue;
				}
				else
				{
					DEBUG("%s","Failed in removing periodic event class implementer");
					break;
				}	
			}
			else
			{
				DEBUG("%s","Periodic event implementer removed successfully");
				break;
			}
		}
		else
                {
                        DEBUG("%s","pointer deleted before removing periodic event class implementer ");
                        break;
                }
	}

    	DEBUG("%s", "Leaving acs_hcs_periodicEvent_ObjectImpl::shutdown");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::create ");
    	(void) oiHandle;
    	(void) ccbId;
    	(void) className;
    	(void) parentName;
    	(void) attr;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          acs_hcs_periodicEvent_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	char startTime[32]		= {0};
	char stopTime[32]		= {0};
	char periodicEventId[128]       = {0};
	int i = 0;
	bool isStartTimeProvided = false;

	cout<<"Before while"<<endl;
	while( attr[i])
        {
		cout<<"attr[i]->attrName :"<<attr[i]->attrName<<endl;
                switch( attr[i]->attrType)
                {
	                case ATTR_STRINGT:
				if(ACE_OS::strcmp(attr[i]->attrName, stopTimeAttr  ) == 0 && attr[i]->attrValuesNum)
                        	{
                                	ACE_OS::strcpy(stopTime, reinterpret_cast<char *>(attr[i]->attrValues[0]));
	                	}                        
				else if(ACE_OS::strcmp(attr[i]->attrName, startTimeAttr )== 0 && attr[i]->attrValuesNum)
                        	{
                        		ACE_OS::strcpy(startTime, reinterpret_cast<char *>(attr[i]->attrValues[0]));
					isStartTimeProvided = true;
				}
				else if(ACE_OS::strcmp(attr[i]->attrName,periodicEventIdAttr) == 0 && attr[i]->attrValuesNum)
                                {
                                        ACE_OS::strcpy(periodicEventId, reinterpret_cast<char *>(attr[i]->attrValues[0]));
                                }
			break;

			default:	
			break;
		}
		i++;
	}

	string StartTime(startTime);
	string StopTime(stopTime);	

	int retValue=createValidations( StartTime, StopTime);
	switch (retValue)
    	{
        	case ACS_HC_STARTTIME_EXPIRED :
                	strcpy(errorText,STARTTIME_EXPIRED);
                        setErrorCode(ACS_HC_STOPTIME_EXPIRED);
			return ACS_CC_FAILURE;

        	case ACS_HC_INVALID_TIME:
                	strcpy(errorText,INVALID_TIME);
                        setErrorCode(ACS_HC_INVALID_TIME);
			return ACS_CC_FAILURE;
		
		case ACS_HC_INVALID_TIMEZONE	:	
			strcpy(errorText,INVALID_TIMEZONE);
                        setErrorCode(ACS_HC_INVALID_TIMEZONE);
		 	return ACS_CC_FAILURE;

		case ACS_HC_STOPTIME_EXPIRED    :
                        strcpy(errorText,STOPTIME_EXPIRED);
                        setErrorCode(ACS_HC_STOPTIME_EXPIRED);
			return ACS_CC_FAILURE;

		case ACS_HC_INVALID_STOPTIME	:
			strcpy(errorText,INVALID_STOPTIME);
                        setErrorCode(ACS_HC_INVALID_STOPTIME);
			return ACS_CC_FAILURE;

		default:
                break;
        }

	string Id = periodicEventId;
        string rdn = Id + COMMA_STR + parentName;
	acs_hcs_periodicEvent_ObjectImpl::newDn = rdn;

        string time(startTime);
	acs_hcs_global_ObjectImpl obj;	
        if((time.compare(EMPTY_STR) == 0))
		time = obj.getCurrentTime();

        if(augmentcall(oiHandle,ccbId,time,rdn,CREATE,isStartTimeProvided))
        {
                strcpy(errorText,"Augmentation failure for the event");
                setErrorCode(ACS_HC_AUGMENT_FAILED);
                return ACS_CC_FAILURE;
        }

	DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::create"); 
	return ACS_CC_SUCCESS;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::deleted");

	if(!(g_ptr->isAdminStateLOCKED((string)objName)))
        {
                strcpy(errorText,"Admin state for job Scheduler is not locked");
                setErrorCode(ACS_HC_SCHEDULER_UNLOCK);
                return ACS_CC_FAILURE;
        }

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_periodicEvent_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

        acs_hcs_periodicEvent_ObjectImpl::completePeventRdn = objName;
        acs_hcs_periodicEvent_ObjectImpl::delete_Pevent_flag = true;

	DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::deleted"); 
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::modify");
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	string rdn(objName);
	acs_hcs_periodicEvent_ObjectImpl::newDn = rdn;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_periodicEvent_ObjectImpl ObjectModifyCallback invocated\n");
	DEBUG ("%s","-----------------------------------------------------\n");

	char startTime[32]	= {0};
	char stopTime[32]	= {0};
	bool stoptime_flag = false;
	bool starttime_flag = false;
	
	int i = 0;

	cout<<"Before while"<<endl;
	while( attrMods[i])
        {
		cout<<"attrMods[i]->modAttr.attrName :"<<attrMods[i]->modAttr.attrName<<endl;
                switch( attrMods[i]->modAttr.attrType)
                {
	                case ATTR_STRINGT:
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, stopTimeAttr  ) == 0 )
				{
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						ACE_OS::strcpy(stopTime,EMPTY_STR);
						stoptime_flag = true;
					}
					else
					{
						ACE_OS::strcpy(stopTime, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					}
				}                        
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, startTimeAttr) == 0 )
				{
					if((int)attrMods[i]->modAttr.attrValuesNum == 0)
					{
						ACE_OS::strcpy(startTime,EMPTY_STR);
						starttime_flag = true;	
					}
					else
					{
						ACE_OS::strcpy(startTime, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));		
						starttime_flag = true;
					}				
				}
				else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,LASTSCHEDULEDATE) == 0)
				{
					return ACS_CC_SUCCESS;
					//no need to validation in case of lastscheduled date updation
				}
			break;

			default:	
			break;
		}
		i++;
	}
		
        string StartTime(startTime);
        string StopTime(stopTime);
	if(stoptime_flag == false && StopTime.size()== 0) //To get the previously provided stopTime
	{
		StopTime = g_ptr->getImmAttribute(objName,stopTimeAttr);	
	}
	if(starttime_flag == false)
	{
		StartTime = g_ptr->getImmAttribute(objName,startTimeAttr);
		if(g_ptr->compareDates(StartTime,g_ptr->getCurrentTime()))
			StartTime=EMPTY_STR;
	}

        int retValue=createValidations( StartTime, StopTime);
	switch (retValue)
        {
                case ACS_HC_STARTTIME_EXPIRED :
                        strcpy(errorText,STARTTIME_EXPIRED);
                        setErrorCode(ACS_HC_STOPTIME_EXPIRED);
			return ACS_CC_FAILURE;

                case ACS_HC_INVALID_TIME:
                        strcpy(errorText,INVALID_TIME);
                        setErrorCode(ACS_HC_INVALID_TIME);
			return ACS_CC_FAILURE;

                case ACS_HC_INVALID_TIMEZONE    :
                        strcpy(errorText,INVALID_TIMEZONE);
                        setErrorCode(ACS_HC_INVALID_TIMEZONE);
			return ACS_CC_FAILURE;

		case ACS_HC_STOPTIME_EXPIRED    :
                        strcpy(errorText,STOPTIME_EXPIRED);
                        setErrorCode(ACS_HC_STOPTIME_EXPIRED);
			return ACS_CC_FAILURE;

                case ACS_HC_INVALID_STOPTIME    :
                        strcpy(errorText,INVALID_STOPTIME);
                        setErrorCode(ACS_HC_INVALID_STOPTIME);
			return ACS_CC_FAILURE;

                default:
                break;

        }
	
	string time(startTime);
	acs_hcs_global_ObjectImpl obj;
	bool isStartTimeProvided = true;
        if((time.compare(EMPTY_STR) == 0))
	{
		time = obj.getCurrentTime();
		isStartTimeProvided = false;
	}
		
        if(augmentcall(oiHandle,ccbId,time,rdn,MODIFY,isStartTimeProvided))
        {
                strcpy(errorText,"Augmentation failure for the event");
                setErrorCode(ACS_HC_AUGMENT_FAILED);
                return ACS_CC_FAILURE;
        }

	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::modify");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::complete"); 

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_periodicEvent_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	cout << "Complete returned" << endl;
    	DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::complete");

	return ACS_CC_SUCCESS;
}//End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_hcs_periodicEvent_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::abort");

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_periodicEvent_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    	DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::abort"); 
}//End of abort

/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_hcs_periodicEvent_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("%s","Entering ACS_HC_PERIODICEVENT_ObjectImpl::apply"); 

	(void)oiHandle;
	(void)ccbId;
	
	DEBUG ("%s","\n---------------------------------------------------\n");
    	DEBUG ("%s","      ACS_HC_PERIODICEVENT_ObjectImpl CcbApplyCallback invocated           \n");
    	DEBUG ("%s","---------------------------------------------------\n");

	cout<<"In apply call back"<<endl;

	if(acs_hcs_periodicEvent_ObjectImpl::delete_Pevent_flag == true)
        {
                g_ptr->updateNextScheduledTime(acs_hcs_periodicEvent_ObjectImpl::completePeventRdn);
                acs_hcs_periodicEvent_ObjectImpl::completePeventRdn = EMPTY_STR;
                acs_hcs_periodicEvent_ObjectImpl::delete_Pevent_flag = false;
        }

	cout<<"statement 4"<<endl;
        int create_size = create_periodic_event.size();
        int modify_size = modify_periodic_event.size();
        cout<<"statement 5"<<endl;

        if( create_size != 0 )
        {
                g_ptr->updateNextScheduledTime(acs_hcs_periodicEvent_ObjectImpl::newDn,create_periodic_event,PERIODIC,CREATE);
                create_periodic_event.clear();
        }

        if(modify_size != 0 )
        {
                g_ptr->updateNextScheduledTime(acs_hcs_periodicEvent_ObjectImpl::newDn,modify_periodic_event,PERIODIC,MODIFY);
                modify_periodic_event.clear();
        }
	
	DEBUG("%s","Leaving ACS_HC_Job_ObjectImpl::apply");
}//End of apply

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::updateRuntime");
	(void) objName;
	(void) attrName;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_periodicEvent_ObjectImpl CcbUpdateRuntimeCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_periodicEvent_ObjectImpl::updateRuntime");

	return ACS_CC_FAILURE;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_hcs_periodicEvent_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)oiHandle;
        (void)invocation;
        (void)p_objName;
        (void)operationId;
        (void)paramList;

     	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::adminOperationCallback");
	cout<<"Entering acs_hcs_periodicEvent_ObjectImpl::adminOperationCallback"<<endl;
	
	DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::adminOperationCallback");
}//End of adminOperationCallback

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void acs_hcs_periodicEvent_ObjectImpl::setErrorCode(unsigned int code)
{
	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::setErrorCode  ");
	cout<<"Entering acs_hcs_periodicEvent_ObjectImpl::setErrorCode "<<endl;
    	setExitCode(code,errorText);
        DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::setErrorCode  ");
}

/*===================================================================
        ROUTINE: createValidations
=================================================================== */
ACE_INT32 acs_hcs_periodicEvent_ObjectImpl::createValidations(string &startTime, string stopTime)
{
	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::createValidations ");
	int returnCode = -1;
	
	//Validations for start time
	if(startTime.compare(EMPTY_STR) == 0 )
	{
		DEBUG("%s","Taking default starttime acs_hcs_periodicEvent_ObjectImpl::createValidations ");
		cout<<"providing default starttime"<<endl;
		startTime = g_ptr->getCurrentTime();
		cout<<startTime<<endl;
	}
	else
	{
		returnCode = g_ptr->timeValidation(startTime, startTimeAttr);
		if( returnCode != 0)
		{
			 DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::createValidations in starttime");
			return returnCode;
		}
	}

	//validations for stopTime
	if(stopTime.compare(EMPTY_STR) != 0)
	{
		returnCode = g_ptr->timeValidation(stopTime, stopTimeAttr);
		if( returnCode != 0)
		{
			DEBUG("%s","leaving acs_hcs_periodicEvent_ObjectImpl::createValidationsin stoptime");
			return returnCode;
		}
		if( g_ptr->compareDates(startTime,stopTime) == false ) //check whether stop time is greater than start time
		{
			DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::createValidations as stoptime invalid");
                        return ACS_HC_INVALID_STOPTIME;
		}
	}
	DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::createValidations ");
	return 0;
}
//End of create validations

/*===================================================================
        ROUTINE: augmentcall
=================================================================== */
bool acs_hcs_periodicEvent_ObjectImpl::augmentcall(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, string time, string rdn, int callback, bool isStartTimeProvided)
{
	DEBUG("%s","Entering acs_hcs_periodicEvent_ObjectImpl::augmentcall ");
	//Augmentation call to update last schedule date in single event
        ACS_APGCC_CcbHandle ccbHandleVal;
        ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
        bool isAugmentFail = false;

        ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);
        if(retVal != ACS_CC_SUCCESS)
        {
                cout<<"Augmentation initialization is failed "<<endl;
                DEBUG("%s","Augmentation initialization is failed");
                isAugmentFail = true;
        }
        else
        {
                time = g_ptr->localZoneTime(time);
                eventDetails e(time,rdn);
		if(callback == CREATE)
	                create_periodic_event.push_back(e);
		else
			modify_periodic_event.push_back(e);

                ACS_CC_ImmParameter attributeRDN;
                attributeRDN.attrName = (char*)LASTSCHEDULEDATE;
                attributeRDN.attrType = ATTR_STRINGT;
                attributeRDN.attrValuesNum = 1;
                char* rdnValue = const_cast<char*>(time.c_str());
                void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
                attributeRDN.attrValues = value2;

                if(!isStartTimeProvided)
                {
                        ACS_CC_ImmParameter attributeRDN1;
                        attributeRDN1.attrName = (char*)startTimeAttr;
                        attributeRDN1.attrType = ATTR_STRINGT;
                        attributeRDN1.attrValuesNum = 1;
                        char* rdnValue1 = const_cast<char*>(time.c_str());
                        void* value3[1] = { reinterpret_cast<int*>(rdnValue1)};
                        attributeRDN1.attrValues = value3;

                        retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal,rdn.c_str(),&attributeRDN1);
                        if(retVal != ACS_CC_SUCCESS )
                        {
                                isAugmentFail = true;
                        }
                }

		 retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal,rdn.c_str(),&attributeRDN);
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
	DEBUG("%s","Leaving acs_hcs_periodicEvent_ObjectImpl::augmentcall ");
	return isAugmentFail;
}
