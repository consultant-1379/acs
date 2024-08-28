/*=================================================================== */
   /**
   @file acs_logm_srvparam_oihandler.cpp

   Class method implementationn for LOGM module.

   This module contains the implementation of class declared in
   the acs_logm_srvparam_oihandler.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       09/07/2011   XTANAGG   Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#if 0
#include <ace/ACE.h>
#include <acs_apgcc_omhandler.h>
#include <acs_logm_srvparam_oihandler.h>
#include <acs_logm_logmaint.h>
#include <acs_logm_common.h>

/*===================================================================
   ROUTINE: acs_logm_srvparam_oihandler
=================================================================== */
acs_logm_srvparam_oihandler::acs_logm_srvparam_oihandler( string aLOGMObjName, 
					string aLOGMImplName, 
					ACS_APGCC_ScopeT p_scope )
			: acs_apgcc_objectimplementereventhandler_V2(aLOGMObjName,
								   aLOGMImplName,
								   p_scope)
	,theLOGMObjName(aLOGMObjName)
{
	/**Instance a Reactor to handle the events**/
        DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler Constructor");

	thetp_reactor_impl = new ACE_TP_Reactor;
	theReactor = new ACE_Reactor(thetp_reactor_impl);
	theoiHandler = new acs_apgcc_oihandler_V2() ;

	DEBUG ("%s", "Leaving acs_logm_srvparam_oihandler Constructor");

}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::setObjectImplementer()
{
	DEBUG("Entering setObjectImplementer() , theLOGMObjName : %s",theLOGMObjName.c_str() );

	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;

	errorCode = theoiHandler->addClassImpl(this,LOGM_ROOT_CLASS_NAME);

	DEBUG("Error code returning from setObjectImplementer() : %d",errorCode);

	DEBUG("Leaving setObjectImplementer() , theLOGMObjName : %s",theLOGMObjName.c_str() );

	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::create( 	ACS_APGCC_OiHandle oiHandle, 
						ACS_APGCC_CcbId ccbId, 
						const char *className,
						const char* parentName, 
						ACS_APGCC_AttrValues **attr)
{
	DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::create()");
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;

	DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::create()");
	return ACS_CC_FAILURE;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::deleted(  ACS_APGCC_OiHandle oiHandle, 
						ACS_APGCC_CcbId ccbId, 
						const char *objName)
{
        DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::deleted()");

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::deleted()");
	return ACS_CC_FAILURE;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::modify(ACS_APGCC_OiHandle oiHandle, 
					ACS_APGCC_CcbId ccbId, 
					const char *objName,
					ACS_APGCC_AttrModification **attrMods)
{
        DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::modify()");

	(void)oiHandle;
	(void)ccbId;

	int i=0;
	while( attrMods[i] )
	{
		if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_TRIGGER_HOUR) ==  0 )
		{
			//Value of srvTrgHour should be in the range 0-23.

			DEBUG("Modify callback triggered for srvTrgHour for %s", objName);	
			ACE_UINT32 mySrvTrgHour = *reinterpret_cast< ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( mySrvTrgHour > 23 )
			{
				ERROR("srvTrgHour value is greater than 23 : %d", mySrvTrgHour );
				ERROR("%s", "Modification of srvTrgHour failed." );
				ERROR ("%s", "Leaving acs_logm_srvparam_oihandler::modify.");
				return ACS_CC_FAILURE;
			}
			DEBUG("srvTrgHour has been modified to %d", mySrvTrgHour );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_TRIGGER_MINUTES ) == 0 )
		{
			//Value of srvTrgMinutes should be in the range 0-59.

			DEBUG("Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 mySrvTrgMin = *reinterpret_cast< ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);

                        if( mySrvTrgMin > 59 )
                        {
                                ERROR("srvTrgMinutes value is greater than 59 : %d", mySrvTrgMin );
                                ERROR("%s", "Modification of srvTrgMinutes failed." );
                                ERROR("%s", "Leaving acs_logm_srvparam_oihandler::modify.");
                                return ACS_CC_FAILURE;
                        }
                        DEBUG("srvTrgMinutes has been modified to %d", mySrvTrgMin );

		}
		else if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_TRIGGER_FREQUENCY) == 0 )
		{
			DEBUG("Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 mySrvTrgFreq = *reinterpret_cast< ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);

                        if( mySrvTrgFreq !=  0 &&  mySrvTrgFreq != 1 )
                        {
                                ERROR("srvTrgFreq: %d. srvTrgFreq value is neither 0 nor 1.", mySrvTrgFreq );
                                ERROR("%s", "Modification of srvTrgFreq failed." );
                                ERROR("%s", "Leaving acs_logm_srvparam_oihandler::modify.");
                                return ACS_CC_FAILURE;
                        }
                        DEBUG("srvTrgFreq has been modified to %d", mySrvTrgFreq );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, LOG_FILE_COUNT) == 0 )
		{
			DEBUG("Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 myLogTypeCnt = *reinterpret_cast< ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( myLogTypeCnt != 7  )
			{
				ERROR("%s", "LogTypeCnt is not equal to 6." );
				ERROR("%s", "Modification of logTypeCnt failed." );
				ERROR("%s", "Leaving acs_logm_srvparam_oihandler::modify.");
				return ACS_CC_FAILURE;
			}
			DEBUG("LogTypeCnt has been modified to %d", myLogTypeCnt );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, STS_STATUS_LOG_DEL) == 0 )
		{
			DEBUG("Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);	
			ACE_UINT32 mySrvTrgStsDelete =  *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
			if( mySrvTrgStsDelete != 0   && mySrvTrgStsDelete != 1)
			{
				ERROR("srvTrgStsDelete: %d. srvTrgStsDelete value is neither 0 nor 1.", mySrvTrgStsDelete );
                                ERROR("%s", "Modification of srvTrgStsDelete failed." );
                                ERROR("%s", "Leaving acs_logm_srvparam_oihandler::modify.");
                                return ACS_CC_FAILURE;
                        }
                        DEBUG("srvTrgStsDelete has been modified to %d", mySrvTrgStsDelete );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, FILE_COUNT_FREQUNCY) == 0 )
                {
                        DEBUG("Modify callback triggered for %s for %s", attrMods[i]->modAttr.attrName, objName);
                        ACE_UINT32 myFileCntFreq =  *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
                        if( myFileCntFreq > 59)
                        {
				ERROR("fileCntFreq value is greater than 59 : %d", myFileCntFreq );
                                ERROR("%s", "Modification of fileCntFreq failed." );
                                ERROR("%s", "Leaving acs_logm_srvparam_oihandler::modify.");
                                return ACS_CC_FAILURE;
                        }
                        DEBUG("fileCntFreq has been modified to %d", myFileCntFreq );
                }
	i++;
	}

	DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::modify()");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::complete( ACS_APGCC_OiHandle oiHandle,
						ACS_APGCC_CcbId ccbId)
{
        DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::complete()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::complete()");
        return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_logm_srvparam_oihandler::abort(ACS_APGCC_OiHandle oiHandle, 
				ACS_APGCC_CcbId ccbId)
{
        DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::abort()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::abort()");
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::updateRuntime(const char* param1,
						    const char* param2)
{
        DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::updateRuntime()");
	(void)param1;
	(void)param2;
        DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::updateRuntime()");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_logm_srvparam_oihandler::apply( ACS_APGCC_OiHandle oiHandle, 
				ACS_APGCC_CcbId ccbId)
{
        DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::apply()");
	(void)oiHandle;
	(void)ccbId;
	//do nothing
        DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::apply()");
}//End of apply

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_logm_srvparam_oihandler::adminOperationCallback(ACS_APGCC_OiHandle, ACS_APGCC_InvocationType, const char*, ACS_APGCC_AdminOperationIdType, ACS_APGCC_AdminOperationParamType**)
{
	DEBUG ( "%s", "Entering acs_logm_srvparam_oihandler::adminOperationCallback()");
	DEBUG ( "%s", "Leaving acs_logm_srvparam_oihandler::adminOperationCallback()");
}//End of adminOperationCallback


/*===================================================================
   ROUTINE: startSrvParamFunctionality
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::startSrvParamFunctionality()
{
	DEBUG("%s", "Entering acs_logm_srvparam_oihandler::startSrvParamFunctionality.");
	DEBUG("%s", "Calling setObjectImplementer");
	if(setObjectImplementer() != ACS_CC_SUCCESS)
	{
		ERROR("%s", "setObjectImplementer() failed.");
		ERROR("%s", "Leaving acs_logm_srvparam_oihandler::startSrvParamFunctionality.");
		return ACS_CC_FAILURE;
	}
	//TR HP20427
	try
	{
		if (theReactor != NULL)
		{
			this->theReactor->open(1);
			dispatch(this->theReactor, ACS_APGCC_DISPATCH_ALL);
			//ACE_OS::sleep(5);
			if( this->theReactor->run_reactor_event_loop() == -1 )
			{
				ERROR("%s", "Error occured in run_reactor_loop()");
				ERROR("%s", "Leaving acs_logm_srvparam_oihandler::startSrvParamFunctionality.");
				return ACS_CC_FAILURE;
			}
		}
		else
		{
			ERROR("%s", "theReactor is NULL");
			ERROR("%s", "Leaving acs_logm_srvparam_oihandler::startSrvParamFunctionality.");
			return ACS_CC_FAILURE;
		}
	}
	catch(...)
	{
		ERROR("%s", "Generic exception caught in acs_logm_srvparam_oihandler::startSrvParamFunctionality()");
	}

	DEBUG("%s", "Leaving acs_logm_srvparam_oihandler::startSrvParamFunctionality.");
 	return ACS_CC_SUCCESS;
}//End of startLOGMFunctionality

/*===================================================================
   ROUTINE: SrvParamFunctionalMethod
=================================================================== */
ACE_THR_FUNC_RETURN acs_logm_srvparam_oihandler::SrvParamFunctionalMethod(void* aLOGMPtr)
{
	DEBUG("%s", "Entering SrvParamFunctionalMethod.");
#if 0
	if( ThrExitHandler::init() == false )
	{
		ERROR("%s", "Error occured while registering exit handler.");
		ERROR("%s", "Leaving SrvParamFunctionalMethod.");
		return 0;
	}
#endif	
	acs_logm_srvparam_oihandler * mySrvParamImplementer = 0;
	mySrvParamImplementer = reinterpret_cast<acs_logm_srvparam_oihandler*>(aLOGMPtr);

	if(mySrvParamImplementer != 0)
	{
		DEBUG( "%s", "Calling startSrvParamFunctionality.");
                if(mySrvParamImplementer->startSrvParamFunctionality() == ACS_CC_FAILURE)
                {
                        ERROR("%s", "startSrvParamFunctionality method returned failure.");
                        ERROR("%s", "Leaving SrvParamFunctionalMethod.");
                        return 0;
                }
	}
	else
	{
                ERROR("%s", "mySrvParamImplementer is NULL.");
                ERROR("%s", "Leaving SrvParamFunctionalMethod.");
                return 0;
	}
	DEBUG("%s", "Leaving SrvParamFunctionalMethod");
	return 0;
}//End of SrvParamFunctionalMethod

/*===================================================================
   ROUTINE: setupSrvParamOIThread
=================================================================== */
ACS_CC_ReturnType acs_logm_srvparam_oihandler::setupSrvParamOIThread(acs_logm_srvparam_oihandler * aLOGMImplementer,
								     ACE_INT32 & aThreadGroupId )
{
	DEBUG("%s", "Entering setupSrvParamOIThread.");
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	#if 0
	DEBUG("%s", "Spawning the LOGM Service Param OI Thread.");
	aThreadGroupId  = ACE_Thread_Manager::instance()->spawn(&SrvParamFunctionalMethod,
							(void*)aLOGMImplementer ,
							THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
							0,
							0,
							ACE_DEFAULT_THREAD_PRIORITY,
							&aThreadGroupId,
							0,
							ACE_DEFAULT_THREAD_STACKSIZE);
	if(aThreadGroupId == -1)
	{
		ERROR("%s", "Spawning of LOGM Service Param OI Thread failed.");
		rc = ACS_CC_FAILURE;
	}
	#endif

	aThreadGroupId = ACS_LOGM_Common::StartThread(&SrvParamFunctionalMethod, 
				    (void*)aLOGMImplementer,
				     ACE_TEXT("SrvParamFunctionalThread"),
				     aThreadGroupId );

	if(aThreadGroupId == -1)
        {
                ERROR("%s", "Spawning of LOGM Service Param OI Thread failed.");
                rc = ACS_CC_FAILURE;
        }

	DEBUG("%s", "Leaving setupSrvParamOIThread");
	return rc;

}//End of setupSrvParamOIThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void acs_logm_srvparam_oihandler::shutdown()
{
	DEBUG("%s", "Entering acs_logm_srvparam_oihandler::shutdown.");

	DEBUG("%s", "Calling end_reactor_event_loop.");
	theReactor->end_reactor_event_loop();

	//Remove the implementer.
	DEBUG("%s", "Calling removeClassImpl.");
	theoiHandler->removeClassImpl(this,LOGM_ROOT_CLASS_NAME);

	DEBUG("%s", "Leaving acs_logm_srvparam_oihandler::shutdown.");
}//End of shutdown

/*===================================================================
   ROUTINE: getReactor
=================================================================== */
ACE_Reactor* acs_logm_srvparam_oihandler::getReactor()
{
        DEBUG("%s", "Entering acs_logm_srvparam_oihandler::getReactor.");
        DEBUG("%s", "Leaving acs_logm_srvparam_oihandler::getReactor.");
	return theReactor;
}//End of getReactor


/*===================================================================
	ROUTINE: ~acs_logm_srvparam_oihandler
=================================================================== */
acs_logm_srvparam_oihandler::~acs_logm_srvparam_oihandler()
{
        DEBUG("%s", "Entering acs_logm_srvparam_oihandler Destructor");

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

	//ThrExitHandler::cleanup();

	DEBUG("%s", "Leaving acs_logm_srvparam_oihandler Destructor");

}//End of Destructor
#endif
