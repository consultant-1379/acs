/*=================================================================== */
/**
   @file acs_hcs_inUse_ObjectImpl.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012     XHARBAV       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <ace/Signal.h>
#include "acs_hcs_inUse_objectImpl.h"
#include "acs_hcs_healthcheckservice.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_Util.H"
#include "acs_hcs_transferqueue.h"
#include "acs_hcs_global_ObjectImpl.h"

using namespace std;

/*===================================================================
   ROUTINE: acs_hcs_inUse_ObjectImpl
=================================================================== */
acs_hcs_inUse_ObjectImpl::acs_hcs_inUse_ObjectImpl(string ClassName,
                string szImpName,
                ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3(  ClassName,szImpName,enScope ),
 theClassName(ClassName)
{
	DEBUG("%s"," Entering acs_hcs_inUse_ObjectImpl::acs_hcs_inUse_ObjectImpl");
	memset(errorText,0,100);
	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	m_poTPReactorImpl = 0;
    	m_poReactor = 0;
    	m_poTPReactorImpl = new ACE_TP_Reactor();
	m_isClassImplAdded= false;

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

	DEBUG("%s"," Exiting acs_hcs_inUse_ObjectImpl::acs_hcs_inUse_ObjectImpl");
}//End of acs_hcs_inUse_ObjectImpl

/*===================================================================
   ROUTINE: ~acs_hcs_inUse_ObjectImpl
=================================================================== */
acs_hcs_inUse_ObjectImpl::~acs_hcs_inUse_ObjectImpl()
{
	DEBUG("%s"," Entering acs_hcs_inUse_ObjectImpl::~acs_hcs_inUse_ObjectImpl");
	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_HC_INUSE_CLASS_NAME);
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
			

	DEBUG("%s"," Exiting acs_hcs_inUse_ObjectImpl::~acs_hcs_inUse_ObjectImpl");
}//End of ~acs_hcs_inUse_ObjectImpl

ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::setImpl()
{
	DEBUG("%s","Entering acs_hcs_inUse_ObjectImpl::setImpl() ");
#if 0
        for (int i=0; i < 10; ++i)
        {
                theOiHandlerPtr->removeClassImpl(this, ACS_HC_INUSE_CLASS_NAME);
                ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_HC_INUSE_CLASS_NAME);

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

        ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this, ACS_HC_INUSE_CLASS_NAME);

        if ( errorCode == ACS_CC_FAILURE )
        {
                int intErr = getInternalLastError();
                cout<<"intErr:"<<intErr<<endl;
                cout<<"Error : Set implementer  for the object failure"<<endl;
                ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
                DEBUG("%s"," Exiting acs_hcs_inUse_ObjectImpl::svc Dispatch");
                return ACS_CC_FAILURE;
        }
        else
        {
                cout << "acs_hcs_inUse_ObjectImpl success" << endl;
                DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
        }
	

	DEBUG("%s","Leaving acs_hcs_inUse_ObjectImpl::setImpl() ");
        return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 acs_hcs_inUse_ObjectImpl::svc(  )
{
	DEBUG("%s"," Entering acs_hcs_inUse_ObjectImpl::svc Dispatch");
#if 0
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this, ACS_HC_INUSE_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		int intErr = getInternalLastError();	
		cout<<"intErr:"<<intErr<<endl; 
		cout<<"Error : Set implementer  for the object failure"<<endl;
		ERROR( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s"," Exiting acs_hcs_inUse_ObjectImpl::svc Dispatch");
		return -1;
	}
	else
	{
		cout << "acs_hcs_inUse_ObjectImpl success" << endl;
		DEBUG("Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}
#endif
        
	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();

	DEBUG("%s"," Exiting acs_hcs_inUse_ObjectImpl::svc Dispatch");
	return 0;
}//End of svc

/*===================================================================
   ROUTINE: shutdown
=================================================================== */
void acs_hcs_inUse_ObjectImpl::shutdown()
{
	DEBUG("%s", "Entering acs_hcs_inUse_ObjectImpl::shutdown");

	m_poReactor->end_reactor_event_loop();

	DEBUG("%s", "Calling removeClassImpl.");
	for (int i=0; i < 3; ++i)
	{
		if( theOiHandlerPtr != 0 )
                {
			if(  theOiHandlerPtr->removeClassImpl(this,ACS_HC_INUSE_CLASS_NAME) != ACS_CC_SUCCESS)
			{
				cout<<endl<<endl<<"remove class failed for inuse "<<endl;
				int intErr = getInternalLastError();
				if( intErr == -6 )  //Need to try again as IMM is busy at that moment
				{
					//for(int j=0; j< 100; ++j) ; //Waiting for sometime before retrying
					DEBUG("%s","removing implementer for in use class failed with error code 6");
					sleep(0.2);
					continue;
				}
				else
				{
					DEBUG("%s","Failed in removing in use class implementer");
					break;
				}
			}
			else
			{
				DEBUG("%s","Implementer for in use class removed successfully");
				break;
			}
		}
		else
                {
                        DEBUG("%s","pointer deleted before removing inUse class implementer ");
                        break;
                }
	}

	DEBUG("%s", "Leaving acs_hcs_inUse_ObjectImpl::shutdown");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG("%s","Entering acs_hcs_inUse_ObjectImpl::create ");
    (void) oiHandle;
    (void) ccbId;
    (void) className;
    (void) parentName;
    (void) attr;

	cout<<"Entering acs_hcs_inUse_ObjectImpl::create"<<endl;
	cout<<"After ACS_HC_Server::incOperationInProgressFlag()"<<endl;
	cout<<"className :"<<className<<endl;
	cout<<"parentName :"<<parentName<<endl;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          acs_hcs_inUse_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	DEBUG("%s","Leaving acs_hcs_inUse_ObjectImpl::create"); 
	return ACS_CC_SUCCESS;
}//End of create
/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	DEBUG("%s","Entering acs_hcs_inUse_ObjectImpl::deleted");

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_inUse_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	cout<<"Entering acs_hcs_inUse_ObjectImpl::deleted callback"<<endl;
	
	DEBUG("%s","Leaving acs_hcs_inUse_ObjectImpl::deleted"); 
	return ACS_CC_SUCCESS;
}//End of deleted
/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::modify");
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

   	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_inUse_ObjectImpl ObjectModifyCallback invocated\n");
	DEBUG ("%s","-----------------------------------------------------\n");

	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::modify");
	return ACS_CC_SUCCESS;
}//End of modify
/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    DEBUG("%s","Entering acs_hcs_inUse_ObjectImpl::complete"); 

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_inUse_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
	cout << "Complete returned" << endl;
    DEBUG("%s","Leaving acs_hcs_inUse_ObjectImpl::complete");

	return ACS_CC_SUCCESS;

}//End of complete
/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_hcs_inUse_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    DEBUG("%s","Entering acs_hcs_inUse_ObjectImpl::abort");

	(void)oiHandle;
	(void)ccbId;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_inUse_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
    DEBUG("%s","Leaving acs_hcs_inUse_ObjectImpl::abort"); 
}//End of abort
/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_hcs_inUse_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
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
ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
	DEBUG("%s","Entering ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::updateRuntime");
	(void) objName;
	(void) attrName;
	cout << "objname: " << objName << endl;
	cout << "attrnmae: " << attrName[0] << endl;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            acs_hcs_readyToUse_ObjectImpl CcbUpdateRuntimeCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	string object = idEquOne_Comma;
	object = object + objName;

	cout << "object: " << object << endl;

	ACS_CC_ReturnType returnCode;
	ACS_CC_ImmParameter progressReportAttr;

	progressReportAttr.attrName = (char*)progressReport_Attr;
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

	DEBUG("%s","Leaving ACS_CC_ReturnType acs_hcs_inUse_ObjectImpl::updateRuntime");
	return ACS_CC_SUCCESS;
}//End of updateRuntime
/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_hcs_inUse_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
     DEBUG("%s","Entering acs_hcs_inUse_ObjectImpl::adminOperationCallback");
	cout<<"Entering acs_hcs_inUse_ObjectImpl::adminOperationCallback"<<endl;
	(void) oiHandle;
	(void) invocation;
	(void) operationId;
	(void) paramList;
	(void) p_objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           acs_hcs_inUse_ObjectImpl adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	int myOIValidationError=1;
	string inUseObjname(p_objName);
	size_t posMsName=0;
	size_t posMsNameEnd=0;
   

	posMsName = inUseObjname.find(",",posMsName+1);
	string  parentDN = inUseObjname.substr(posMsName+1,(posMsNameEnd - (posMsName+1)));

	string RDN;
	char tempRDN[256];
	void* unloadArg;

	cout<<"operationId:"<<operationId<<endl;
	switch (operationId)
	{
		case UNLOAD :
			cout<<"admin operation UNLOAD"<<endl;
			RDN = reinterpret_cast<char*>(paramList[0]->attrValues);
			strcpy(tempRDN, RDN.c_str());
			cout << "tempRDN: " << tempRDN << endl;
			unloadArg = reinterpret_cast<void*>(tempRDN);

			if (ACE_Thread_Manager::instance ()->spawn(ACE_THR_FUNC (&AcsHcs::HealthCheckService::unload),
						unloadArg,
						THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
						0,
						0,
						ACE_DEFAULT_THREAD_PRIORITY,
						-1,
						0,
						ACE_DEFAULT_THREAD_STACKSIZE,
						0) == -1)
			{
				cout << "Error creating thread load" << endl;
			}

			break;

		default:
			DEBUG("%s","Operation ID is received by acs_hcs_inUse_ObjectImpl::saImmOiAdminOperationCallback is not valid");
			myOIValidationError = -1;
			break;
	}

	ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError);
        if(rc != ACS_CC_SUCCESS)
        {
			cout<<"Failure occurred in sending AdminOperation Result"<<endl;
            ERROR("%s","Failure occurred in sending AdminOperation Result");
            ERROR("%s","Exiting acs_hcs_inUse_ObjectImpl::saImmOiAdminOperationCallback");
            return;
        }
	DEBUG("%s","Leaving acs_hcs_inUse_ObjectImpl::adminOperationCallback");
	cout << "Leaving acs_hcs_inUse_ObjectImpl::adminOperationCallback" << endl;
}//End of adminOperationCallback

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void acs_hcs_inUse_ObjectImpl::setErrorCode(unsigned int code)
{
	DEBUG("%s","Entering  acs_hcs_inUse_ObjectImpl::setErrorCode ");
	cout<<"Entering acs_hcs_inUse_ObjectImpl::setErrorCode "<<endl;
    setExitCode(code,errorText);
	DEBUG("%s","Leaving  acs_hcs_inUse_ObjectImpl::setErrorCode ");
}
