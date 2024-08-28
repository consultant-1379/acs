/*=================================================================== */
/**
   @file   acs_lm_managed_objectImplementer.cpp

   @brief Header file for LM module.

          This module contains all the declarations useful to
          specify the class acs_lm_managed_objectImplementer.

   @version 1.0.0
 */

/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       09-Nov-2011   XCSSATA   Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <ace/Signal.h>
#include <saImmOi.h>
#include <acs_apgcc_oihandler_V3.h>
#include "acs_lm_defs.h"
#include "acs_lm_tra.h"
#include "acs_lm_managed_objectImplementer.h"
#include "acs_lm_brfc_interface.h"
using namespace std;
//bool fpChangeNodeName;
/*=================================================================
	ROUTINE: acs_lm_managed_objectImplementer constructor
=================================================================== */
acs_lm_managed_objectImplementer::acs_lm_managed_objectImplementer(string szimpName )
:acs_apgcc_objectimplementereventhandler_V3( szimpName ),
 thePersistent(0),
 theClienthandlerPtr(0),
 theEventHandler(0),
 theOmHandlerPtr(0)
{
	thetp_reactor_impl = new ACE_TP_Reactor;
	theReactor = new ACE_Reactor(thetp_reactor_impl);
	theReactor->open(1);
	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	OIFuncThreadId =0;
	errorCode = 0;
	errorString = "";
	managedElementvalue = "";

}//end of constructor

/*=================================================================
	ROUTINE: acs_lm_managed_objectImplementer destructor
=================================================================== */
acs_lm_managed_objectImplementer::~acs_lm_managed_objectImplementer()
{
	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_IMM_LM_managed_CLASS_NAME);
		shutdown();
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}

	if( theReactor != 0)
	{
		delete theReactor;
		theReactor = 0;
	}

	if( thetp_reactor_impl != 0 )
	{
		delete thetp_reactor_impl;
		thetp_reactor_impl = 0;
	}


}//end of constructor

/*=================================================================
	ROUTINE: setLMManagedObjectImplementer
=================================================================== */
ACS_CC_ReturnType acs_lm_managed_objectImplementer::setLMManagedObjectImplementer()
{
	return theOiHandlerPtr->addClassImpl(this,ACS_APZ_IMM_APZ_CLASS_NAME);
}//end of setLMManagedObjectImplementer

/*=================================================================
	ROUTINE: svc
=================================================================== */
ACE_INT32 acs_lm_managed_objectImplementer::svc( ACE_Reactor *poReactor )
{
	DEBUG("%s","Entering acs_lm_managed_objectImplementer::svc");

/*	for (int i=0; i<3; i++)
	{

		ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_APZ_IMM_APZ_CLASS_NAME);

		if ( errorCode == ACS_CC_SUCCESS )
		{
			DEBUG( "Set implementer for the Class %s  Success ",ACS_APZ_IMM_APZ_CLASS_NAME );
			dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);
			DEBUG("%s","Exiting acs_lm_managed_objectImplementer::svc");
			return 0;
		}
		else
		{
			ERROR( "Set implementer for the Class %s  failure...Retrying ",ACS_APZ_IMM_APZ_CLASS_NAME );
			usleep(10000);
		}
	}

	ERROR( "Set implementer for the Class %s  failure, Even after retries. Hence leaving ",ACS_APZ_IMM_APZ_CLASS_NAME );
	return -1; */

	dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);
	DEBUG("%s","Exiting acs_lm_managed_objectImplementer::svc");
	return 0;

}//end of svc
/*=================================================================
	ROUTINE: create
=================================================================== */

ACS_CC_ReturnType acs_lm_managed_objectImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){


	DEBUG ("acs_lm_managed_objectImplementer %s"," ObjectCreateCallback invoked \n");
	INFO ("oiHandle %ld ccbId %ld className %s parentName %s ACS_APGCC_AttrValues %u \n",oiHandle,ccbId,className,parentName,attr);
	DEBUG ("acs_lm_managed_objectImplementer %s"," ObjectCreateCallback Leaving \n");
	return ACS_CC_SUCCESS;
}//end of create
/*=================================================================
	ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_lm_managed_objectImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	DEBUG ("acs_lm_managed_objectImplementer %s"," ObjectDeleteCallback invocated");
	INFO ("oiHandle %ld ccbId %ld objName %s\n",oiHandle,ccbId,objName);
	DEBUG ("acs_lm_managed_objectImplementer %s"," ObjectDeleteCallback Leaving");
	return ACS_CC_SUCCESS;
}//end of deleted
/*=================================================================
	ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_lm_managed_objectImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)attrMods;
	DEBUG ("acs_lm_managed_objectImplementer %s","   ObjectmodifyCallback invocated           \n");
	DEBUG ("oiHandle %ld ccbId %ld objName %s\n",oiHandle,ccbId,objName);
	int i=0;
	while( attrMods[i] )
	{
		if( ACE_OS::strcmp( attrMods[i]->modAttr.attrName, NETWORK_MANAGEDELEMENT_ATTR ) == 0 )
		{
			INFO("Modify callback triggered for NetworkManagedElement for %s", objName);
			//Log Path should be a valid path.
			char *myNetworkManagedElement = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
			INFO("The value of managedElementvalue is %s", managedElementvalue.c_str() );
			INFO("The value of myNetworkManagedElement is %s", myNetworkManagedElement);
			if(managedElementvalue.empty())
			{
				managedElementvalue = myNetworkManagedElement;
				INFO("managedElementvalue changed to %s", managedElementvalue.c_str() );
			}

			//theClienthandlerPtr->getLMMode(myGraceMode,myEmergencyMode,myTestMode);
			if(theClienthandlerPtr->getVirginModeStatus() == false)
			{
				if(managedElementvalue != myNetworkManagedElement)
				{
					fpChangeNodeName = true;
					INFO ("acs_lm_managed_objectImplementer %s","Grace mode will be set during next triggering activity \n");
					//thePersistent->setLmMode(ACS_LM_MODE_GRACE, true);
					//DEBUG("%s","acs_lm_managed_objectImplementer::modify Grace mode activated");
					//theEventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
				}
				/* Tampered the node name and then again modified it to the correct node name,
				   in this scenario, it should not set to Grace mode in next triggering activity */
				else
				{
					fpChangeNodeName = false;
				}
			}
			/* Moved the below three lines outside of above 'if' block, 
			   to intialize Sentinel attributes to zero during Virin mode also */
			ACS_LM_Sentinel::cached = 0;
			ACS_LM_Sentinel::customExSize = 0;
			INFO("%s","acs_lm_managed_objectImplementer::modify ACS_LM_Sentinel::customExSize = 0");

		}
		i++;
	}
	DEBUG ("acs_lm_managed_objectImplementer %s","   ObjectmodifyCallback Leaving \n");
	return ACS_CC_SUCCESS;
}//end of modify
/*=================================================================
	ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_lm_managed_objectImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG ("acs_lm_managed_objectImplementer %s","  CcbCompleteCallback invocated           \n");
	INFO ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);
	DEBUG ("acs_lm_managed_objectImplementer %s","  CcbCompleteCallback Leaving           \n");

	return ACS_CC_SUCCESS;

}//end of complete
/*=================================================================
	ROUTINE: abort
=================================================================== */
void acs_lm_managed_objectImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG ("acs_lm_managed_objectImplementer %s"," CcbAbortCallback invocated           \n");
	INFO ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);
	DEBUG ("acs_lm_managed_objectImplementer %s"," CcbAbortCallback Leaving           \n");

}//end of abort
/*=================================================================
	ROUTINE: apply
=================================================================== */
void acs_lm_managed_objectImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG ("acs_lm_managed_objectImplementer %s"," CcbApplyCallback invocated           \n");
	INFO ("oiHandle %ld ccbId %ld \n",oiHandle,ccbId);
	DEBUG ("acs_lm_managed_objectImplementer %s"," CcbApplyCallback Leaving           \n");
}//end of apply
/*=================================================================
	ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_lm_managed_objectImplementer::updateRuntime(const char *objName, const char **attrName)
{
	DEBUG ("%s"," acs_lm_managed_objectImplementer CcbUpdateRuntimeCallback Invocated");
	(void) objName;
	(void) attrName;
	DEBUG ("%s"," acs_lm_managed_objectImplementer CcbUpdateRuntimeCallback Leaving");
	return ACS_CC_SUCCESS;
}//end of updateRuntime
/*=================================================================
	ROUTINE: isObjectExiststheLMInfo
=================================================================== */

//============================================================================
// isObjectExiststheEMFInfoClassName
// checks the runtime object exists in imm or not.
// true on success
// false on failure
//============================================================================

void acs_lm_managed_objectImplementer::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)p_objName;
	(void)invocation;
	(void)oiHandle;
	(void)operationId;
	(void)paramList;

	DEBUG ("%s","acs_lm_managed_objectImplementer adminOperationCallback invocated");
	DEBUG("acs_lm_managed_objectImplementer::adminOperationCallback() %s","Leaving");
}//end of adminOperationCallback
/*=================================================================
	ROUTINE: setPersistant
=================================================================== */
void acs_lm_managed_objectImplementer::setEventHandler(ACS_LM_EventHandler* aEventHandler)
{
	DEBUG("acs_lm_managed_objectImplementer::setEventHandler() %s","Entering");
	theEventHandler = aEventHandler;
	DEBUG("acs_lm_managed_objectImplementer::setEventHandler() %s","Leaving");
}//end of setPersistant

/*=================================================================
	ROUTINE: setPersistant
=================================================================== */
void acs_lm_managed_objectImplementer::setPersistant(ACS_LM_Persistent* aPersistant)
{
	DEBUG("acs_lm_managed_objectImplementer::setPersistant() %s","Entering");
	thePersistent = aPersistant;
	DEBUG("acs_lm_managed_objectImplementer::setPersistant() %s","Leaving");
}//end of setPersistant

void acs_lm_managed_objectImplementer::setClienthandler(ACS_LM_ClientHandler* aClienthandler)
{
	DEBUG("acs_lm_root_objectImplementer::setClienthandler() %s","Entering");
	theClienthandlerPtr = aClienthandler;
	DEBUG("acs_lm_root_objectImplementer::setClienthandler() %s","Leaving");
}//end of setClienthandler
/*=================================================================
	ROUTINE: setOmHandler
=================================================================== */
void acs_lm_managed_objectImplementer::setOmHandler(OmHandler* omHandler)
{
	DEBUG("acs_lm_managed_objectImplementer::setOmHandler() %s","Entering");
	theOmHandlerPtr = omHandler;
	DEBUG("acs_lm_managed_objectImplementer::setOmHandler() %s","Leaving");
}//end of setOmHandler
/*=================================================================
	ROUTINE: setErrorText
=================================================================== */
bool acs_lm_managed_objectImplementer::setErrorText(unsigned int aErrorId, std::string ErrorText)
{
	DEBUG("%s","LM Reporting Following Error while serving LM Commands");
	DEBUG("Error ID = %d",aErrorId);
	DEBUG("ERROR TEXT = %s",ErrorText.c_str());
	setExitCode(aErrorId,ErrorText);
	return true;
}//end of setErrorText


/*===================================================================
        ROUTINE: shutdown
=================================================================== */
void acs_lm_managed_objectImplementer::shutdown()
{
	DEBUG("%s", "Entering acs_lm_managed_objectImplementer::shutdown.");

	DEBUG("%s", "Calling end_reactor_event_loop.");

	if( theOiHandlerPtr != NULL)
	{
		//Remove the implementer.
		DEBUG("%s", "Calling removeClassImpl.");
		//theOiHandlerPtr->removeClassImpl(this,ACS_APZ_IMM_APZ_CLASS_NAME);
		for (int i=0; i < 10; ++i)
		{
			if( theOiHandlerPtr->removeClassImpl(this,ACS_APZ_IMM_APZ_CLASS_NAME) == ACS_CC_FAILURE )
			{
				int intErr = getInternalLastError();
				if ( intErr == -6)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if ( i >= 10)
					{
						break;
					}
					else
						continue;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

	}
	DEBUG("%s", "Leaving acs_lm_managed_objectImplementer::shutdown.");
	theReactor->end_reactor_event_loop();
        if( OIFuncThreadId != 0 )
        {
                ACE_Thread_Manager::instance()->join(OIFuncThreadId);
        }
}//End of shutdown


/*===================================================================
   ROUTINE: setupLMOIThread 
=================================================================== */
ACS_CC_ReturnType acs_lm_managed_objectImplementer::setupLMManagedOIThread(acs_lm_managed_objectImplementer *almManagedObjectImplementer)
{

	DEBUG( "%s", "Entering setupObjectImplementer()");
	const ACE_TCHAR* lpszThreadName1 = "LMmanagedObjImpThread";
	int mythread;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;
	OmHandler omHandler;
	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		DEBUG("%s","OmHandler Init is failed in setupLMManagedOIThread");
	}
	ACS_CC_ImmParameter paramToFind;
	std::string myManagedElementRdn(MANAGEDELEMENT_RDN);
	DEBUG("Fetching %s from IMM", NETWORK_MANAGEDELEMENT_ATTR );
	paramToFind.attrName = (char*)NETWORK_MANAGEDELEMENT_ATTR;
	std::string pszAttrValue;
	if (omHandler.getAttribute(myManagedElementRdn.c_str(), &paramToFind ) == ACS_CC_SUCCESS)
	{
		DEBUG("%s","getAttribute is success and Object is preset in IMM");
		if(paramToFind.attrValuesNum != 0)
		{
			pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			std::string mynodename(pszAttrValue);
			DEBUG(" In acs_lm_managed_objectImplementer::setupLMManagedOIThread %s","NetworkManagedElement attribute is defined");
			managedElementvalue = mynodename;
		}
		else
		{
			DEBUG(" In acs_lm_managed_objectImplementer::setupLMManagedOIThread %s","NetworkManagedElement attribute is not Defined");
		}
	}
	else
	{
		ERROR("%s","getAttribute is failed and Object is not preset in IMM");
	}

	INFO( "%s", "Spawning the ObjectImplementer Thread");
	mythread = ACE_Thread_Manager::instance()->spawn(&LMManagedOIFunc,
			(void*)almManagedObjectImplementer,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&OIFuncThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName1);
	DEBUG( "OIFuncThreadId= %ld", OIFuncThreadId);
	if(mythread == -1)
	{
		ERROR( "%s", "Spawning of LM OI Functional thread failed");
		rc = ACS_CC_FAILURE;
	}
	if (omHandler.Finalize() == ACS_CC_FAILURE)
	{
		ERROR("%s","OmHandler Finalize is failed in setupLMManagedOIThread");
	}
	DEBUG( "%s", "Leaving setupObjectImplementer()");
	return rc;
}


/*===================================================================
   ROUTINE: LMmanagedOIFunc
=================================================================== */
ACE_THR_FUNC_RETURN acs_lm_managed_objectImplementer::LMManagedOIFunc(void* aLMOIPtr)
{
	DEBUG( "%s", "Entering LMmanagedOIThread().");

	acs_lm_managed_objectImplementer * myLMmanagedObjImplementer = 0;
	myLMmanagedObjImplementer= reinterpret_cast<acs_lm_managed_objectImplementer*>(aLMOIPtr);

	if(myLMmanagedObjImplementer!= 0)
	{
		INFO( "%s", "Calling lmManagedOI_svc()");
		if(myLMmanagedObjImplementer->lmManagedOI_svc() == ACS_CC_FAILURE)
		{

			ERROR( "%s", "lmManagedOI_svc() failed");
			ERROR( "%s", "Leaving LMmanagedOIThread()");
			return 0;
		}
	}
	else
	{
		ERROR( "%s", "myLMmanagedObjImplementer is NULL");
		ERROR( "%s", "Leaving LMmanagedOIThread()");
		return 0;
	}
	DEBUG( "%s", "Leaving LMmanagedOIThread()");
	return 0;
}//End of LMmanagedOIThread


ACE_INT32 acs_lm_managed_objectImplementer::lmManagedOI_svc()
{
	DEBUG("%s","Entering acs_lm_managed_objectImplementer::lmManagedOI_svc");
	ACE_INT32 res = svc(this->theReactor);
	if ( res == -1 )
	{
		ERROR("%s", "svc() failed" );
		return -1;
	}
        try
        {
             this->theReactor->run_reactor_event_loop();
        }
        catch(...)
        { 
        	WARNING("%s","Exception has been caught in acs_lm_managed_objectImplementer::lmManagedOI_svc");
        }
	DEBUG("%s","Exiting acs_lm_managed_objectImplementer::lmManagedOI_svc");
	return 0;

}


void acs_lm_managed_objectImplementer::setExitCode(int error, std::string text)

{

	DEBUG("%s", "Entering acs_lm_managed_objectImplementer::setExitCode");

	errorCode = error;

	errorString = text;

	DEBUG("%s", "Exiting acs_lm_managed_objectImplementer::setExitCode");

}



int acs_lm_managed_objectImplementer::getExitCode()

{

	DEBUG("%s", "Entering acs_lm_managed_objectImplementer::getExitCode");

	DEBUG("%s", "Exiting acs_lm_managed_objectImplementer::getExitCode");

	return errorCode;

}

