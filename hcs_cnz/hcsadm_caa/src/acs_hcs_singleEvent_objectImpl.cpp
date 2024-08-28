/*=================================================================== */
/**
   @file acs_hcs_singleEvent_ObjectImpl.cpp

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
=====================================================================*/

/*==================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <ace/Signal.h>
#include "acs_hcs_singleEvent_objectImpl.h"
#include "acs_hcs_periodicEvent_objectImpl.h"
#include "acs_hcs_jobScheduler_objectImpl.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_apgcc_omhandler.h"

using namespace std;

/*==================================================================
                 STATIC VARIABLE DECLARATION SECTION
=================================================================== */
bool acs_hcs_singleEvent_ObjectImpl::delete_Sevent_flag = false;
string acs_hcs_singleEvent_ObjectImpl::completeSeventRdn = EMPTY_STR;
string acs_hcs_singleEvent_ObjectImpl::newDn = "";
vector<eventDetails> create_single_event = vector<eventDetails>();
vector<eventDetails> modify_single_event = vector<eventDetails>();

/*===================================================================
   ROUTINE: acs_hcs_singleEvent_ObjectImpl
=================================================================== */
acs_hcs_singleEvent_ObjectImpl::acs_hcs_singleEvent_ObjectImpl(string ClassName,string szImpName,ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3(  ClassName,szImpName,enScope ),theClassName(ClassName)
{
	DEBUG("%s"," Entering acs_hcs_singleEvent_ObjectImpl::acs_hcs_singleEvent_ObjectImpl");
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
		ERROR("%s", "Memory allocation failed for acs_hcs_global_ObjectImpl");
	}

	DEBUG("%s"," Exiting acs_hcs_singleEvent_ObjectImpl::acs_hcs_singleEvent_ObjectImpl");
}//End of acs_hcs_singleEvent_ObjectImpl

/*===================================================================
   ROUTINE: ~acs_hcs_singleEvent_ObjectImpl
=================================================================== */
acs_hcs_singleEvent_ObjectImpl::~acs_hcs_singleEvent_ObjectImpl()
{
	DEBUG("%s"," Entering acs_hcs_singleEvent_ObjectImpl::~acs_hcs_singleEvent_ObjectImpl");

	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_HC_SINGLEEVENT_CLASS_NAME);
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
	if(g_ptr != 0)
	{
		delete g_ptr;
		g_ptr = 0;
	}

	DEBUG("%s"," Exiting acs_hcs_singleEvent_ObjectImpl::~acs_hcs_singleEvent_ObjectImpl");
}//End of ~acs_hcs_singleEvent_ObjectImpl

/*===================================================================
   ROUTINE: setImpl
=================================================================== */
ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::setImpl()
{
	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::setImpl()");
#if 0

        for (int i=0; i < 10; ++i)
        {
                theOiHandlerPtr->removeClassImpl(this, ACS_HC_SINGLEEVENT_CLASS_NAME);
                ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_SINGLEEVENT_CLASS_NAME);

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
			DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::setImpl()");
                        return ACS_CC_SUCCESS;
                }
        }
#endif

	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_SINGLEEVENT_CLASS_NAME);

        if ( errorCode == ACS_CC_FAILURE )
        {
                int intErr = getInternalLastError();
                cout<<"intErr:"<<intErr<<endl;
                cout<<"Error : Set implementer  for the object failure"<<endl;
                ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
                DEBUG("%s"," Exiting acs_hcs_singleEvent_ObjectImpl::svc Dispatch");
                return ACS_CC_FAILURE;
        }
        else
        {
                cout << "acs_hcs_singleEvent_ObjectImpl success" << endl;
                DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
        }

	DEBUG("%s"," Leaving acs_hcs_singleEvent_ObjectImpl::setImpl()");
        return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 acs_hcs_singleEvent_ObjectImpl::svc(  )
{
	DEBUG("%s"," Entering acs_hcs_singleEvent_ObjectImpl::svc Dispatch");
#if 0
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_SINGLEEVENT_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		int intErr = getInternalLastError();	
		cout<<"intErr:"<<intErr<<endl; 
		cout<<"Error : Set implementer  for the object failure"<<endl;
		ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s"," Exiting acs_hcs_singleEvent_ObjectImpl::svc Dispatch");
		return -1;
	}
	else
	{
		cout << "acs_hcs_singleEvent_ObjectImpl success" << endl;
		DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}
#endif
        
	m_poReactor->open(1);
    	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
    	m_poReactor->run_reactor_event_loop();

	DEBUG("%s"," Exiting acs_hcs_singleEvent_ObjectImpl::svc Dispatch");
	return 0;
}//End of svc

/*===================================================================
   ROUTINE: shutdown
=================================================================== */
void acs_hcs_singleEvent_ObjectImpl::shutdown()
{
    	DEBUG("%s", "Entering acs_hcs_singleEvent_ObjectImpl::shutdown");
	cout << "In shutdown" << endl;

    	m_poReactor->end_reactor_event_loop();

	for (int i=0; i < 3; ++i)
	{
		if( theOiHandlerPtr != 0 )
                {
			if(  theOiHandlerPtr->removeClassImpl(this,ACS_HC_SINGLEEVENT_CLASS_NAME) != ACS_CC_SUCCESS)
			{
				int intErr = getInternalLastError();
				if( intErr == -6 )  //Need to try again as IMM is busy at that moment
				{
					//for(int j=0; j< 100; ++j) ; //Waiting for sometime before retrying
					DEBUG("%s","removing single event implementer failed with return code 6");
					sleep(0.2);
					continue;
				}
				else
				{
					DEBUG("%s","Failed in removing single event class implementer");
					break;
				}
			}
			else
			{
				DEBUG("%s","Single event implementer removed successfully");
				break;
			}
		}
		else
                {
                        DEBUG("%s","pointer deleted before removing job class implementer ");
                        break;
                }
	}
                //theOiHandlerPtr->removeClassImpl(this,ACS_HC_SINGLEEVENT_CLASS_NAME);

    	DEBUG("%s", "Leaving acs_hcs_singleEvent_ObjectImpl::shutdown");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
        DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::create ");
    	(void) oiHandle;
    	(void) ccbId;
    	(void) className;
    	(void) parentName;
    	(void) attr;

	cout<<"Entering acs_hcs_singleEvent_ObjectImpl::create"<<endl;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          acs_hcs_singleEvent_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	char scheduledTime[32]	= {0};
	char singleEventId[128] = {0};
	int i = 0;
	
	cout<<"Before while"<<endl;
	while( attr[i])
        {
		cout<<"attr[i]->attrName :"<<attr[i]->attrName<<endl;
                switch( attr[i]->attrType)
                {
	                case ATTR_STRINGT:
				if(ACE_OS::strcmp(attr[i]->attrName, scheduledTimeAttr  ) == 0)
                        	{
                                	ACE_OS::strcpy(scheduledTime, reinterpret_cast<char *>(attr[i]->attrValues[0]));
	                 	}
				else if(ACE_OS::strcmp(attr[i]->attrName, singleEventIdAttr) == 0 && attr[i]->attrValuesNum)
                                {
                                        ACE_OS::strcpy(singleEventId, reinterpret_cast<char *>(attr[i]->attrValues[0]));
                                }
			 break;

			default:	
			break;
		}	
		i++;
	}

	string time(scheduledTime);
	int retValue=createValidations(time);
	switch (retValue)
    	{
           	case ACS_HC_STARTTIME_EXPIRED :
                	strcpy(errorText,SCHEDULEDTIME_EXPIRED);
                        setErrorCode(ACS_HC_STARTTIME_EXPIRED);
	                return ACS_CC_FAILURE;

        	case ACS_HC_INVALID_TIME :
                        strcpy(errorText,INVALID_SCHDTIME);
                        setErrorCode(ACS_HC_INVALID_TIME);
	                return ACS_CC_FAILURE;
	
		case ACS_HC_INVALID_TIMEZONE :
			strcpy(errorText,INVALID_TIMEZONE);
                        setErrorCode(ACS_HC_INVALID_TIMEZONE);
			return ACS_CC_FAILURE;
        
		default:
                break;
    	}

	string Id = singleEventId;
	string rdn = Id + COMMA_STR + parentName;
	acs_hcs_singleEvent_ObjectImpl::newDn = rdn;		

	if(augmentcall(oiHandle,ccbId,scheduledTime,rdn,CREATE))
	{
		strcpy(errorText,"Augmentation failure for the event");
                setErrorCode(ACS_HC_AUGMENT_FAILED);
                return ACS_CC_FAILURE;
	}

	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::create"); 
	return ACS_CC_SUCCESS;
}//End of create
/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::deleted");
	(void)oiHandle;
        (void)ccbId;
        (void)objName;

	if(!(g_ptr->isAdminStateLOCKED((string)objName)))
	{
		strcpy(errorText,"Admin state for job Scheduler is not locked");
                setErrorCode(ACS_HC_SCHEDULER_UNLOCK);
                return ACS_CC_FAILURE;
	}
	DEBUG ("%s","            acs_hcs_singleEvent_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	cout<<"Entering acs_hcs_singleEvent_ObjectImpl::deleted callback"<<endl;
	
	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::deleted");

	acs_hcs_singleEvent_ObjectImpl::completeSeventRdn = objName;
	acs_hcs_singleEvent_ObjectImpl::delete_Sevent_flag = true;

	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::modify");
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_singleEvent_ObjectImpl ObjectModifyCallback invocated\n");
	DEBUG ("%s","-----------------------------------------------------\n");

	string rdn = objName;
	acs_hcs_singleEvent_ObjectImpl::newDn = rdn;
	char scheduledTime[32]			= {0};
	int i = 0;

	cout<<"Before while"<<endl;
	while( attrMods[i])
        {
		cout<<"attrMods[i]->modAttr.attrName :"<<attrMods[i]->modAttr.attrName<<endl;
                switch( attrMods[i]->modAttr.attrType)
                {
	                case ATTR_STRINGT:
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, scheduledTimeAttr  ) == 0)
                        	{
                                	ACE_OS::strcpy(scheduledTime, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
	                    	} 
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,LASTSCHEDULEDATE) == 0)
                                {
                                        return ACS_CC_SUCCESS;
                                        //no need to validation in case of lastscheduled date updation
                                }
			break;
                       
			case ATTR_INT32T:
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,executionStatusAttr) == 0)
				{
					return ACS_CC_SUCCESS;
                                        //no need to validation in case of execution status updation date updation
                                }
			break;

			default:	
			break;
		}
		i++;
	}
	
	string time(scheduledTime);
        int retValue=createValidations(time);
	switch (retValue)
        {
                case ACS_HC_STARTTIME_EXPIRED :
                        strcpy(errorText,SCHEDULEDTIME_EXPIRED);
                        setErrorCode(ACS_HC_STARTTIME_EXPIRED);
                        return ACS_CC_FAILURE;

                case ACS_HC_INVALID_TIME :
                        strcpy(errorText,INVALID_SCHDTIME);
                        setErrorCode(ACS_HC_INVALID_TIME);
                        return ACS_CC_FAILURE;

                case ACS_HC_INVALID_TIMEZONE :
                        strcpy(errorText,INVALID_TIMEZONE);
                        setErrorCode(ACS_HC_INVALID_TIMEZONE);
                        return ACS_CC_FAILURE;

                default:
                break;
        }

	if(augmentcall(oiHandle,ccbId,scheduledTime,rdn,MODIFY))
        {
                strcpy(errorText,"Augmentation failure for the event");
                setErrorCode(ACS_HC_AUGMENT_FAILED);
                return ACS_CC_FAILURE;
        }

	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::modify");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::complete"); 

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_singleEvent_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	cout << "Complete returned" << endl;
	
	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::complete");

	return ACS_CC_SUCCESS;

}//End of complete

/*===================================================================
   ROUTINE: abort
================================================================== */
void acs_hcs_singleEvent_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::abort");

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_singleEvent_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::abort"); 
}//End of abort

/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_hcs_singleEvent_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    	DEBUG("%s","Entering ACS_HC_Job_ObjectImpl::apply"); 
	cout<<"I am in apply call back"<<endl;
	(void)oiHandle;
	(void)ccbId;

	if(acs_hcs_singleEvent_ObjectImpl::delete_Sevent_flag == true)//This block is to caliculate the next schedule time as an event is deleted it may vary
	{
		g_ptr->updateNextScheduledTime(acs_hcs_singleEvent_ObjectImpl::completeSeventRdn);
		acs_hcs_singleEvent_ObjectImpl::completeSeventRdn = "";
		acs_hcs_singleEvent_ObjectImpl::delete_Sevent_flag = false;
	}
	
        int create_size = create_single_event.size();
        int modify_size = modify_single_event.size();

        if( create_size != 0 )
        {
                g_ptr->updateNextScheduledTime(acs_hcs_singleEvent_ObjectImpl::newDn,create_single_event,SINGLE,CREATE);
                create_single_event.clear();
        }

        if(modify_size != 0 )
        {
                g_ptr->updateNextScheduledTime(acs_hcs_singleEvent_ObjectImpl::newDn,modify_single_event,SINGLE,MODIFY);
                modify_single_event.clear();
        }
		
	DEBUG ("%s","\n---------------------------------------------------\n");
    	DEBUG ("%s","           ACS_HC_Job_ObjectImpl CcbApplyCallback invocated           \n");
    	DEBUG ("%s","---------------------------------------------------\n");

	DEBUG("%s","Leaving ACS_HC_Job_ObjectImpl::apply");
}//End of apply

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
	cout<<"I am in update run time call back"<<endl;
    	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::updateRuntime");
	(void) objName;
	(void) attrName;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_singleEvent_ObjectImpl CcbUpdateRuntimeCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_singleEvent_ObjectImpl::updateRuntime");

	return ACS_CC_FAILURE;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_hcs_singleEvent_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)oiHandle;	
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;

     	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::adminOperationCallback");
	cout<<"Entering acs_hcs_singleEvent_ObjectImpl::adminOperationCallback"<<endl;
	
	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::adminOperationCallback");
}//End of adminOperationCallback

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void acs_hcs_singleEvent_ObjectImpl::setErrorCode(unsigned int code)
{
	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::setErrorCode ");
	cout<<"Entering acs_hcs_singleEvent_ObjectImpl::setErrorCode "<<endl;
    	setExitCode(code,errorText);
	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::setErrorCode ");
}

/*===================================================================
        ROUTINE: createValidations
=================================================================== */
ACE_INT32 acs_hcs_singleEvent_ObjectImpl::createValidations(string startTime)
{
	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::createValidations ");
	int returnCode=-1;

	returnCode = g_ptr->timeValidation(startTime,scheduledTimeAttr);
	if(returnCode != 0)
	{
		DEBUG("%s"," return after validation acs_hcs_singleEvent_ObjectImpl::createValidations");
		return returnCode;
	}
	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::createValidations ");
	return 	ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE: augmentcall 
=================================================================== */
bool acs_hcs_singleEvent_ObjectImpl::augmentcall(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, string scheduledTime, string rdn, int callback)
{
	DEBUG("%s","Entering acs_hcs_singleEvent_ObjectImpl::augmentcall ");
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
                acs_hcs_global_ObjectImpl obj;
                string time = obj.localZoneTime(scheduledTime);

                eventDetails e(time,rdn);
		if(callback == CREATE)
	                create_single_event.push_back(e);
		else
			modify_single_event.push_back(e);

                ACS_CC_ImmParameter attributeRDN;
                attributeRDN.attrName = (char*)LASTSCHEDULEDATE;
                attributeRDN.attrType = ATTR_STRINGT;
                attributeRDN.attrValuesNum = 1;
                char* rdnValue = const_cast<char*>(time.c_str());
                void* value2[1]={ reinterpret_cast<void*>(rdnValue)};
                attributeRDN.attrValues = value2;

                ACS_CC_ImmParameter attributeRDN1;
                attributeRDN1.attrName = (char*)executionStatusAttr;
                attributeRDN1.attrType = ATTR_INT32T;
                attributeRDN1.attrValuesNum = 1;
                int executionStatus = (int)NOTEXECUTED;
                void* value3[1] = { reinterpret_cast<int*>(&executionStatus)};
                attributeRDN1.attrValues = value3;

                cout<<"statement 6"<<endl;

                retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal,rdn.c_str(),&attributeRDN);
                if(retVal != ACS_CC_SUCCESS )
                {
                        isAugmentFail = true;
		}
                else
                {
                        retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal,rdn.c_str(),&attributeRDN1);
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
        }
	DEBUG("%s","Leaving acs_hcs_singleEvent_ObjectImpl::augmentcall ");
        return isAugmentFail;
}
