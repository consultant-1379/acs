/*=================================================================== */
   /**
   @file acs_chb_param_oihandler.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_param_oihandler.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       09/07/2011   XTANAGG   Initial Release
=================================================================== */
#if 0
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_apgcc_omhandler.h>
#include <ace/ACE.h>
#include <acs_chb_clock_def.h>
#include <acs_chb_param_oihandler.h>
/*===================================================================
   ROUTINE: acs_chb_param_oihandler
=================================================================== */
acs_chb_param_oihandler::acs_chb_param_oihandler( string aCHBObjName, 
					string aCHBImplName, 
					ACS_APGCC_ScopeT p_scope )
			: acs_apgcc_objectimplementereventhandler_V2(aCHBObjName, 
								   aCHBImplName,
								   p_scope)
	,theCHBObjName(aCHBObjName)
{
	/**Instance a Reactor to handle the events**/
        DEBUG ( 1, "%s", "Entering acs_chb_param_oihandler Constructor");

	thetp_reactor_impl = 0;
	theReactor = 0;
	theoiHandler = 0;

	thetp_reactor_impl = new ACE_TP_Reactor;
	if( thetp_reactor_impl == 0 )
	{
		ERROR(1, "%s", "Error occured while allocating memory for ACE_TP_Reactor");
	}
	theReactor = new ACE_Reactor(thetp_reactor_impl);
	if( theReactor == 0 )
	{
		ERROR(1, "%s", "Error occured while allocating memory for theReactor");
	}
	theoiHandler = new acs_apgcc_oihandler_V2() ;
	if( theoiHandler == 0 )
	{
		ERROR(1, "%s", "Error occured while allocating memory for theoiHandler");
	}

	DEBUG (1, "%s", "Leaving acs_chb_param_oihandler Constructor");

}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::setObjectImplementer()
{
	DEBUG(1, "Entering setObjectImplementer() , theCHBObjName : %s",theCHBObjName.c_str() );

	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;

	errorCode = theoiHandler->addObjectImpl(this);

	DEBUG(1, "Error code returning from setObjectImplementer() : %d",errorCode);

	DEBUG(1, "Leaving setObjectImplementer() , theCHBObjName : %s",theCHBObjName.c_str() );

	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::create( 	ACS_APGCC_OiHandle oiHandle, 
						ACS_APGCC_CcbId ccbId, 
						const char *className,
						const char* parentName, 
						ACS_APGCC_AttrValues **attr)
{
	DEBUG ( 1, "%s", "Entering acs_chb_param_oihandler::create()");
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;

	DEBUG ( 1, "%s", "Leaving acs_chb_param_oihandler::create()");
	return ACS_CC_FAILURE;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::deleted(  ACS_APGCC_OiHandle oiHandle, 
						ACS_APGCC_CcbId ccbId, 
						const char *objName)
{
        DEBUG ( 1, "%s", "Entering acs_chb_param_oihandler::deleted()");

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ( 1, "%s", "Leaving acs_chb_param_oihandler::deleted()");
	return ACS_CC_FAILURE;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::modify(ACS_APGCC_OiHandle oiHandle, 
					ACS_APGCC_CcbId ccbId, 
					const char *objName,
					ACS_APGCC_AttrModification **attrMods)
{
        DEBUG ( 1, "%s", "Entering acs_chb_param_oihandler::modify()");

	(void)oiHandle;
	(void)ccbId;

	int i=0;
	while( attrMods[i] )
	{
		if( strcmp( attrMods[i]->modAttr.attrName, ACS_CHB_cycleTime ) ==  0 )
		{
			DEBUG(1, "Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 myCycleTime =  *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( myCycleTime <= 0 )
			{
				ERROR(1, "cycleTime: %u. cycleTime cannot be less than or equal to zero", myCycleTime );
				ERROR(1, "%s", "Modification of cycleTime failed.");
				ERROR(1, "%s", "Leaving acs_chb_param_oihandler::modify.");
                                return ACS_CC_FAILURE;
			}
			DEBUG(1, "cycleTime has been modified to %u", myCycleTime );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, ACS_CHB_maxCallTime ) == 0 )
		{
			DEBUG(1, "Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 myMaxCallTime = *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			DEBUG(1, "maxCallTime has been modified to %u", myMaxCallTime );
				
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, ACS_CHB_minDeviation ) == 0 )
		{
			DEBUG(1, "Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 myMinDeviation = *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( myMinDeviation <= 0 )
			{
				ERROR(1, "minDeviation: %u, minDeviation  cannot be less than or equal to zero", myMinDeviation);
				ERROR(1, "%s", "Modification of minDeviation failed.");
				ERROR(1, "%s", "Leaving acs_chb_param_oihandler::modify.");
				return ACS_CC_FAILURE;
			}
			DEBUG(1, "minDeviation has been modified to %u", myMinDeviation );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, ACS_CHB_maxDeviation) == 0 )
		{
			DEBUG(1, "Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 myMaxDeviation = *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( myMaxDeviation  <= 0 )
			{
				ERROR(1, "maxDeviation: %u, maxDeviation cannot be less than or equal to zero", myMaxDeviation);
				ERROR(1, "%s", "Modification of maxDeviation failed.");
				ERROR(1, "%s", "Leaving acs_chb_param_oihandler::modify.");
				return ACS_CC_FAILURE;
			}
			DEBUG(1, "maxDeviation has been modified to %u", myMaxDeviation );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, ACS_CHB_hbTimeout ) == 0 )
		{
			DEBUG(1, "Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 myHbTimeout = *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( myHbTimeout <= 0 )
			{
				ERROR(1, "hbTimeout: %u, hbTimeout cannot be less than or equal to zero", myHbTimeout );
				ERROR(1, "%s", "Modification of hbTimeout failed");
				ERROR(1, "%s", "Leaving acs_chb_param_oihandler::modify.");
				return ACS_CC_FAILURE;
			} 
			DEBUG(1, "hbTimeout has been modified to %u", myHbTimeout);
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, ACS_CHB_hbInterval ) == 0 )
		{
			DEBUG(1, "Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 myHbInterval = *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( myHbInterval <= 0 )
			{
				ERROR(1, "hbInterval: %u, hbInterval cannot be less than or equal to zero", myHbInterval);
				ERROR(1, "%s", "Modification of hbInterval failed.");
				ERROR(1, "%s", "Leaving acs_chb_param_oihandler::modify.");
				return ACS_CC_FAILURE;
			}
			DEBUG(1, "hbInterval has been modified to %u", myHbInterval);
		}
	i++;
	}

	DEBUG ( 1, "%s", "Leaving acs_chb_param_oihandler::modify()");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::complete( ACS_APGCC_OiHandle oiHandle,
						ACS_APGCC_CcbId ccbId)
{
        DEBUG ( 1,  "%s", "Entering acs_chb_param_oihandler::complete()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( 1, "%s", "Leaving acs_chb_param_oihandler::complete()");
        return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_chb_param_oihandler::abort(ACS_APGCC_OiHandle oiHandle, 
				ACS_APGCC_CcbId ccbId)
{
        DEBUG (1,  "%s", "Entering acs_chb_param_oihandler::abort()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( 1, "%s", "Leaving acs_chb_param_oihandler::abort()");
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::updateRuntime(const char* param1,
						    const char* param2)
{
        DEBUG (1, "%s", "Entering acs_chb_param_oihandler::updateRuntime()");
	(void)param1;
	(void)param2;
        DEBUG (1, "%s", "Leaving acs_chb_param_oihandler::updateRuntime()");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
void acs_chb_param_oihandler :: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
        DEBUG (1, "%s", "Entering acs_chb_param_oihandler::adminOperationCallback()");
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;
        DEBUG (1, "%s", "Leaving acs_chb_param_oihandler::adminOperationCallback()");
}//End of adminOperationCallback

/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_chb_param_oihandler::apply( ACS_APGCC_OiHandle oiHandle, 
				ACS_APGCC_CcbId ccbId)
{
        DEBUG ( 1, "%s", "Entering acs_chb_param_oihandler::apply()");
	(void)oiHandle;
	(void)ccbId;
	//do nothing
        DEBUG ( 1, "%s", "Leaving acs_chb_param_oihandler::apply()");
}//End of apply

/*===================================================================
   ROUTINE: startIMMParamFunctionality
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::startIMMParamFunctionality()
{
        DEBUG(1, "%s", "Entering acs_chb_param_oihandler::startIMMParamFunctionality.");
	DEBUG(1, "%s", "Calling setObjectImplementer");
	if(setObjectImplementer() != ACS_CC_SUCCESS)
	{
		ERROR(1, "%s", "setObjectImplementer() failed.");
		ERROR(1, "%s", "Leaving acs_chb_param_oihandler::startIMMParamFunctionality.");
                return ACS_CC_FAILURE;
	}
	this->theReactor->open(1);
	dispatch(this->theReactor, ACS_APGCC_DISPATCH_ALL);
	if( this->theReactor->run_reactor_event_loop() == -1 )
	{
		ERROR(1, "%s", "Error occured in run_reactor_loop()");
		ERROR(1, "%s", "Leaving acs_chb_param_oihandler::startIMMParamFunctionality.");
		return ACS_CC_FAILURE;
	}
	DEBUG(1, "%s", "Leaving acs_chb_param_oihandler::startIMMParamFunctionality.");
 	return ACS_CC_SUCCESS;
}//End of startIMMParamFunctionality

/*===================================================================
   ROUTINE: IMMParamFunctionalMethod
=================================================================== */
ACE_THR_FUNC_RETURN acs_chb_param_oihandler::IMMParamFunctionalMethod(void* aCHBPtr)
{
	DEBUG(1, "%s", "Entering IMMParamFunctionalMethod.");

	if( ThrExitHandler::init() == false )
	{
		ERROR(1, "%s", "Error occured while registering exit handler.");
		ERROR(1, "%s", "Leaving IMMParamFunctionalMethod.");
		return 0;
	}
	acs_chb_param_oihandler * myIMMParamImplementer = 0;
	myIMMParamImplementer = reinterpret_cast<acs_chb_param_oihandler*>(aCHBPtr);

	if(myIMMParamImplementer != 0)
	{
		DEBUG( 1, "%s", "Calling startIMMParamFunctionality.");
                if(myIMMParamImplementer->startIMMParamFunctionality() == ACS_CC_FAILURE)
                {
                        ERROR(1, "%s", "startIMMParamFunctionality method returned failure.");
                        ERROR(1, "%s", "Leaving IMMParamFunctionalMethod.");
                        return 0;
                }
	}
	else
	{
                ERROR(1, "%s", "myIMMParamImplementer is NULL.");
                ERROR(1, "%s", "Leaving IMMParamFunctionalMethod.");
                return 0;
	}
	DEBUG(1, "%s", "Leaving IMMParamFunctionalMethod");
	return 0;
}//End of IMMParamFunctionalMethod

/*===================================================================
   ROUTINE: setupIMMParamOIThread
=================================================================== */
ACS_CC_ReturnType acs_chb_param_oihandler::setupIMMParamOIThread(acs_chb_param_oihandler * aCHBImplementer)
{
	DEBUG(1, "%s", "Entering setupIMMParamOIThread.");
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	DEBUG(1, "%s", "Spawning the CHB Service Param OI Thread.");
	int myThreadGroupId  = ACE_Thread_Manager::instance()->spawn(&IMMParamFunctionalMethod,
							(void*)aCHBImplementer ,
							THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
							0,
							0,
							ACE_DEFAULT_THREAD_PRIORITY,
							-1,
							0,
							ACE_DEFAULT_THREAD_STACKSIZE);
	if(myThreadGroupId == -1)
	{
		ERROR(1, "%s", "Spawning of CHB IMM Param OI Thread failed.");
		rc = ACS_CC_FAILURE;
	}
	DEBUG(1, "%s", "Leaving setupIMMParamOIThread");
	return rc;

}//End of setupIMMParamOIThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void acs_chb_param_oihandler::shutdown()
{
        DEBUG(1, "%s", "Entering acs_chb_param_oihandler::shutdown.");

	DEBUG(1, "%s", "Calling end_reactor_event_loop.");
	theReactor->end_reactor_event_loop();

	//Remove the implementer.
	theoiHandler->removeObjectImpl(this);
	
	DEBUG(1, "%s", "Leaving acs_chb_param_oihandler::shutdown.");
}//End of shutdown

/*===================================================================
   ROUTINE: getReactor
=================================================================== */
ACE_Reactor* acs_chb_param_oihandler::getReactor()
{
        DEBUG(1, "%s", "Entering acs_chb_param_oihandler::getReactor.");
        DEBUG(1, "%s", "Leaving acs_chb_param_oihandler::getReactor.");
	return theReactor;
}//End of getReactor


/*===================================================================
	ROUTINE: ~acs_chb_param_oihandler
=================================================================== */
acs_chb_param_oihandler::~acs_chb_param_oihandler()
{
        DEBUG(1, "%s", "Entering acs_chb_param_oihandler Destructor");

	if( theoiHandler != 0 )
	{
		delete theoiHandler;
		theoiHandler = 0;
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

	DEBUG(1, "%s", "Leaving acs_chb_param_oihandler Destructor");

}//End of Destructor
#endif
