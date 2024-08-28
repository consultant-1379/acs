/*=================================================================== */
   /**
   @file acs_logm_logparam_oihandler.cpp

   Class method implementationn for LOGM module.

   This module contains the implementation of class declared in
   the acs_logm_logparam_oihandler.h module

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
#include <acs_logm_logparam_oihandler.h>
#include <acs_logm_logmaint.h>

/*===================================================================
   ROUTINE: acs_logm_oihandler
=================================================================== */
acs_logm_logparam_oihandler::acs_logm_logparam_oihandler( string aLOGMClassName, 
							string aLOGMImplName, 
							ACS_APGCC_ScopeT p_scope )
			: acs_apgcc_objectimplementereventhandler_V2(aLOGMClassName,
								   aLOGMImplName,
								   p_scope)
	,theLOGMClassName(aLOGMClassName)
{
	/**Instance a Reactor to handle the events**/
        DEBUG ( "%s", "Entering acs_logm_oihandler Constructor");

	thetp_reactor_impl = new ACE_TP_Reactor;
	theReactor = new ACE_Reactor(thetp_reactor_impl);
	theoiHandler = new acs_apgcc_oihandler_V2() ;

	DEBUG ("%s", "Leaving acs_logm_logparam_oihandler Constructor");

}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::setObjectImplementer()
{
	DEBUG("Entering setObjectImplementer() , theLOGMClassName : %s",theLOGMClassName.c_str() );

	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;

	errorCode = theoiHandler->addClassImpl(this, theLOGMClassName.c_str());

	DEBUG("Error code returning from setObjectImplementer() : %d",errorCode);

	DEBUG("Leaving setObjectImplementer() , theLOGMClassName : %s",theLOGMClassName.c_str() );

	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::create( 	ACS_APGCC_OiHandle oiHandle, 
						ACS_APGCC_CcbId ccbId, 
						const char *className,
						const char* parentName, 
						ACS_APGCC_AttrValues **attr)
{
	DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::create()");
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;

	DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::create()");
	return ACS_CC_FAILURE;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::deleted(  ACS_APGCC_OiHandle oiHandle, 
						ACS_APGCC_CcbId ccbId, 
						const char *objName)
{
        DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::deleted()");

	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::deleted()");
	return ACS_CC_FAILURE;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::modify(ACS_APGCC_OiHandle oiHandle, 
					ACS_APGCC_CcbId ccbId, 
					const char *objName,
					ACS_APGCC_AttrModification **attrMods)
{
        DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::modify()");

	(void)oiHandle;
	(void)ccbId;

	int i=0;
	while( attrMods[i] )
	{
		if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_LOG_NAME) ==  0 )
		{
			DEBUG("Modify callback triggered for logName for %s", objName);	
			ERROR("%s", "Modification of logName is not allowed.");
			ERROR ( "%s", "Leaving acs_logm_logparam_oihandler::modify()");
			return ACS_CC_FAILURE;
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_LOG_PATH ) == 0 )
		{
			DEBUG("Modify callback triggered for logPath for %s", objName);	
			//Log Path should be a valid path.

			char *myLogPath = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
			ACE_stat statBuff;
			if( ACE_OS::stat(myLogPath, &statBuff)  != 0 )
			{
				ERROR("%s", "Modification of logPath failed");
				ERROR("logPath: %s is not a valid path.", myLogPath );
				ERROR ( "%s", "Leaving acs_logm_logparam_oihandler::modify()");
				return ACS_CC_FAILURE;
			}
			DEBUG("logPath changed to %s", myLogPath );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_LOG_TYPE) == 0 )
		{
			DEBUG("Modify callback triggered for logType for %s", objName);	
		
			char *myLogType = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
			//STUB
			string myLocalLogType = (string)myLogType;
			std::string myRecLogType(myLogType);
			if((myRecLogType.compare("*.ddi") != 0))
			{
				if((myRecLogType.compare("*.log") != 0))
				{

					if((myRecLogType.compare("*.*") != 0))
					{
						cout<<endl<<"My logtype inside: "<<myLogType<<endl;
						ERROR("%s", "Modification of LogType failed");
						ERROR("LogType: %s is not a valid path.", myLogType );
						ERROR ( "%s", "Leaving acs_logm_logparam_oihandler::modify()");
						return ACS_CC_FAILURE;
					}
				}

			}
			/*if ( (strcmp(myLogType,"*.ddi") != 0) && (strcmp(myLogType,"*.log") != 0) )
			{
				cout<<endl<<"My logtype inside: "<<myLogType<<endl;
				ERROR("%s", "Modification of LogType failed");
				ERROR("LogType: %s is not a valid path.", myLogType );
				ERROR ( "%s", "Leaving acs_logm_logparam_oihandler::modify()");
				return ACS_CC_FAILURE;
			}*/
			DEBUG("logType changed to %s", myLogType );
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_LOG_OPERATION ) == 0 )
		{
			DEBUG("Modify callback triggered for logNoOfFiles for %s", objName);	

			int myLogNoOfFiles = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
			if(myLogNoOfFiles  < 0)
			{
				ERROR("%s", "logNoOfFiles cannot take any value other than 0 or 1.");
				ERROR ( "%s", "Leaving acs_logm_logparam_oihandler::modify()");
				return ACS_CC_FAILURE;
			}
			DEBUG("logNoOfFiles changed to %d", myLogNoOfFiles);
		}
		else if( strcmp( attrMods[i]->modAttr.attrName, SERVICE_LOG_OLD_DAYS) == 0 )
		{
			DEBUG("Modify callback triggered for logNoOfDays for %s", objName );
			int myLogNoOfDays =  *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);

			if(myLogNoOfDays < 0)
			{
				ERROR("%s","logNoOfDays cannot be less than zero");
				ERROR ( "%s", "Leaving acs_logm_logparam_oihandler::modify()");
				return ACS_CC_FAILURE;
			}
			DEBUG("logNoOfDays changed to %d", myLogNoOfDays );
		}
		i++;
	}

	DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::modify()");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::complete( ACS_APGCC_OiHandle oiHandle,
						ACS_APGCC_CcbId ccbId)
{
        DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::complete()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::complete()");
        return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void acs_logm_logparam_oihandler::abort(ACS_APGCC_OiHandle oiHandle, 
				ACS_APGCC_CcbId ccbId)
{
        DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::abort()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::abort()");
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::updateRuntime(const char* param1,
						    const char* param2)
{
        DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::updateRuntime()");
	(void)param1;
	(void)param2;
        DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::updateRuntime()");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: apply
=================================================================== */
void acs_logm_logparam_oihandler::apply( ACS_APGCC_OiHandle oiHandle, 
				ACS_APGCC_CcbId ccbId)
{
        DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::apply()");
	(void)oiHandle;
	(void)ccbId;
	//do nothing
        DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::apply()");
}//End of apply

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */

void acs_logm_logparam_oihandler::adminOperationCallback(ACS_APGCC_OiHandle, ACS_APGCC_InvocationType, const char*, ACS_APGCC_AdminOperationIdType, ACS_APGCC_AdminOperationParamType**)
{
	DEBUG ( "%s", "Entering acs_logm_logparam_oihandler::adminOperationCallback()");
	DEBUG ( "%s", "Leaving acs_logm_logparam_oihandler::adminOperationCallback()");
}//End of adminOperationCallback


/*===================================================================
   ROUTINE: startLogParamFunctionality
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::startLogParamFunctionality()
{
        DEBUG("%s", "Entering acs_logm_logparam_oihandler::startLogParamFunctionality.");
	DEBUG("%s", "Calling setObjectImplementer");
	if(setObjectImplementer() != ACS_CC_SUCCESS)
	{
		ERROR("%s", "setObjectImplementer() failed");
		ERROR("%s", "Leaving acs_logm_logparam_oihandler::startLogParamFunctionality.");
                return ACS_CC_FAILURE;
	}
	this->theReactor->open(1);
	dispatch(this->theReactor, ACS_APGCC_DISPATCH_ALL);
	if( this->theReactor->run_reactor_event_loop() == -1 )
	{
		ERROR("%s", "Error occured in run_reactor_loop()");
		ERROR("%s", "Leaving acs_logm_logparam_oihandler::startLogParamFunctionality.");
		return ACS_CC_FAILURE;
	}
	DEBUG("%s", "Leaving acs_logm_logparam_oihandler::startLogParamFunctionality.");
 	return ACS_CC_SUCCESS;
}//End of startLogParamFunctionality

/*===================================================================
   ROUTINE: LogParamFunctionalMethod
=================================================================== */
ACE_THR_FUNC_RETURN acs_logm_logparam_oihandler::LogParamFunctionalMethod(void* aLOGMPtr)
{
	DEBUG("%s", "Entering LogParamFunctionalMethod");
#if 0
	if( ThrExitHandler::init() == false )
	{
		ERROR("%s", "Error occured while registering exit handler");
		ERROR("%s", "Leaving LogParamFunctionalMethod.");
		return 0;
	}
#endif
	acs_logm_logparam_oihandler * myLogParamImplementer = 0;
	myLogParamImplementer = reinterpret_cast<acs_logm_logparam_oihandler*>(aLOGMPtr);

	if(myLogParamImplementer != 0)
	{
		DEBUG( "%s", "Calling startLogParamFunctionality");
                if(myLogParamImplementer->startLogParamFunctionality() == ACS_CC_FAILURE)
                {
                        ERROR("%s", "startLogParamFunctionality() failed");
                        ERROR("%s", "Leaving LogParamFunctionalMethod()");
                        return 0;
                }
	}
	else
	{
                ERROR("%s", "myLogParamImplementer is NULL");
                ERROR("%s", "Leaving LogParamFunctionalMethod");
                return 0;
	}
	DEBUG("%s", "Leaving LogParamFunctionalMethod");
	return 0;
}//End of LogParamFunctionalMethod

/*===================================================================
   ROUTINE: setupLogParamOIThread
=================================================================== */
ACS_CC_ReturnType acs_logm_logparam_oihandler::setupLogParamOIThread(acs_logm_logparam_oihandler * aLOGMImplementer,
								     ACE_INT32 & aThreadGroupId )
{
	DEBUG("%s", "Entering setupLogParamOIThread");
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	DEBUG("%s", "Spawning the Log Param OI Thread");

	#if 0
	aThreadGroupId  = ACE_Thread_Manager::instance()->spawn(&LogParamFunctionalMethod,
							(void*)aLOGMImplementer ,
							THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
							0,
							0,
							ACE_DEFAULT_THREAD_PRIORITY,
							&aThreadGroupId,
							0,
							ACE_DEFAULT_THREAD_STACKSIZE);
	#endif

	aThreadGroupId = ACS_LOGM_Common::StartThread(&LogParamFunctionalMethod,
                                    (void*)aLOGMImplementer,
                                     ACE_TEXT("LogParamFunctionalThread"),
                                     aThreadGroupId );

	if(aThreadGroupId == -1)
	{
		ERROR("%s", "Spawning of Log Param OI Thread failed");
		rc = ACS_CC_FAILURE;
	}
	DEBUG("%s", "Leaving setupLogParamOIThread");

	return rc;

}//End of setupLOGMParamOIThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void acs_logm_logparam_oihandler::shutdown()
{
        DEBUG( "%s", "Entering acs_logm_logparam_oihandler::shutdown()");

	DEBUG("%s", "Calling end_reactor_event_loop()");
	theReactor->end_reactor_event_loop();

	//Remove the implementer.
	theoiHandler->removeClassImpl(this, theLOGMClassName.c_str());
	
	DEBUG("%s", "Leaving acs_logm_logparam_oihandler::shutdown");
}//End of shutdown

/*===================================================================
   ROUTINE: getReactor
=================================================================== */
ACE_Reactor* acs_logm_logparam_oihandler::getReactor()
{
        DEBUG("%s", "Entering acs_logm_logparam_oihandler::getReactor");
        DEBUG("%s", "Leaving acs_logm_logparam_oihandler::getReactor");
	return theReactor;
}//End of getReactor


/*===================================================================
	ROUTINE: ~acs_logm_logparam_oihandler
=================================================================== */
acs_logm_logparam_oihandler::~acs_logm_logparam_oihandler()
{
        DEBUG("%s", "Entering acs_logm_oihandler Destructor");

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

	DEBUG("%s", "Leaving acs_logm_logparam_oihandler Destructor");

}//End of Destructor
#endif
