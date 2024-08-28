/*=================================================================== */
   /**
   @file acs_chb_mtzln_implementer.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_mtzln_implementer.h module

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       14/02/2011   XNADNAR   Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_chb_mtzln_implementer.h>
#include <acs_chb_mtz_common.h>
#include <acs_prc_api.h>
#include <ace/Task.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_CommonLib.h>
#include <acs_chb_tra.h>
#include <acs_chb_common.h>


//static ACS_CHB_filehandler MTZObj;

ACE_Recursive_Thread_Mutex ThrExitHandler::theThrMutex;
map<ACE_thread_t, bool> ThrExitHandler::theThrStatusMap;
//std::string ACS_CHB_Common::dnOfCHBRootObj = "";
/*===================================================================
   ROUTINE: ACS_CHB_MTZLN_Implementer
=================================================================== */
ACS_CHB_MTZLN_Implementer::ACS_CHB_MTZLN_Implementer(string aMTZClassName, string aMTLNImplName, ACS_APGCC_ScopeT p_scope )
	: acs_apgcc_objectimplementereventhandler_V2(aMTZClassName, aMTLNImplName, p_scope)
	,theMTZClassName(aMTZClassName)
{
	/**Instance a Reactor to handle the events**/
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer Constructor");

	mtzFuncThreadId = 0;

	thetp_reactor_impl = new ACE_TP_Reactor;
	theReactor = new ACE_Reactor(thetp_reactor_impl);
	theoiHandler = new acs_apgcc_oihandler_V2() ;
	theDependancyFlag = false;

	if(ACS_CHB_Common::fetchDnOfTzLinkRootObjFromIMM() == -1)
	{
		INFO(1,"%s","Fetching of DN of CHB root object from IMM is failed !!");
	}

	DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer Constructor");

}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::setObjectImplementer()
{
	ACS_CC_ReturnType errorCode;
	DEBUG(1,"%s", "Entering setObjectImplementer()");
	errorCode = theoiHandler->addClassImpl(this, theMTZClassName.c_str());
	DEBUG(1, "Error code returning from setObjectImplementer() : %d",errorCode);
	DEBUG(1,"Leaving setObjectImplementer() , theMTZClassName : %s",theMTZClassName.c_str() );
	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::create()");
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;

	int Tz_value = -1;
	char Tz_name[NTZoneSize];
	int i=0;
	while( attr[i] ){
		switch ( attr[i]->attrType ) {
		case ATTR_INT32T:
			if( ACE_OS::strcmp(attr[i]->attrName, TZ_ID) == 0 )
			{
				Tz_value =  *reinterpret_cast<int *>(attr[i]->attrValues[0]);
			}
			break;
		case ATTR_STRINGT:
			if( ACE_OS::strcmp(attr[i]->attrName, TZ_STRING) == 0)
			{
				ACE_OS::strcpy(Tz_name, reinterpret_cast<char *>(attr[i]->attrValues[0]));
			}
			break;
		default:
			break;

		}
		i++;
	}
	DEBUG(1, "create callback triggered for Tz_value: %d", Tz_value);
	DEBUG(1, "create callback triggered for Tz_name: %s", Tz_name);

	//Check the time zone. It should not greater than 200.
	if( ACE_OS::strlen(Tz_name) > (ACE_INT32)NTZoneSize )
	{
		int errorId = 5;
		string errorText = "Illegal operand TZ_name too long \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::create()");
		return ACS_CC_FAILURE;
	}
	//Retrive the Tzi for the time zone name.
	TIME_ZONE_INFORMATION Tzi;
	ACS_CHB_filehandler MTZObj;
	DEBUG(1, "%s", "Calling CheckTimeZone()");
	if( MTZObj.CheckTimeZone(Tz_name, &Tzi) == false)
	{
		int errorId = 6;
		string errorText = "Illegal operand ";
		errorText = errorText +  string(Tz_name) + " Is not a defined time zone\n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "CheckTimeZone() Failed");
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::create()");
		return ACS_CC_FAILURE;
	}
	//Check the time zone number. Time zone number should be between 0-23.
	if( Tz_value < 0 || Tz_value > 23)
	{
		int errorId = 3;
		string errorText = "Operand TMZ_value should be an integer between 0-23 \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::create()");
		return ACS_CC_FAILURE;
	}
	//Create an iterator for iterating file map.
	FileMapType::iterator p;

	//Look for the time zone in map.
	p = g_timeZoneMap.find(Tz_value);

	//If the value is not present in the map,
	// prevent addition.
	if( p != g_timeZoneMap.end() )
	{
		int errorId = 7;
		string errorText =  "TMZ_value already bound ";
		errorText = errorText + "No action performed \n";
		errorText = errorText + " Use -f option to overwrite existing link \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::create()");
		return ACS_CC_FAILURE;
	}

	//Now insert the new entry into the map.
	g_timeZoneMap.insert(FilePairType(Tz_value,ACS_CHB_NtZone(Tz_name, &Tzi)));
	DEBUG(1, "%s", "Calling WriteZoneFile()");
 	if( MTZObj.WriteZoneFile(&g_timeZoneMap) == false)
	{
		int errorId = 8;
		string errorText =  "The MTZ file can't be accessed \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::create()");
		return ACS_CC_FAILURE;
	}
 	DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::create()");
	return ACS_CC_SUCCESS;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::deleted()");

	(void)oiHandle;
	(void)ccbId;

	int Tz_value = -1;
	char tmpObjName[MAX_DN_SIZE];
	ACE_OS::memset(tmpObjName,0, sizeof(tmpObjName));

	//Extracting the time zone number from object name.
	unsigned int j = 0;
	for( unsigned int i =0 ; i < ACE_OS::strlen(objName); i++)
	{
		if( isdigit(objName[i]))
		{
			tmpObjName[j++] = objName[i];
		}
		if( objName[i] == ',' )
		{
			break;
		}
	}
	tmpObjName[j] = '\0';

	Tz_value = atoi(tmpObjName);

	DEBUG(1, "delete callback triggered for Tz_value: %d", Tz_value);

	//Create an iteraor for file map.
	FileMapType::iterator p;

	//Find the time zone number in the map.
	p = g_timeZoneMap.find(Tz_value);

	if( p == g_timeZoneMap.end() )
	{
		int errorId = 4;
		string errorText =  "TMZ_value not defined \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::deleted()");
		return ACS_CC_FAILURE;
	}
	else
	{
		//Now insert the new entry into the map.
		g_timeZoneMap.erase(Tz_value);
		ACS_CHB_filehandler MTZObj;

		if( MTZObj.WriteZoneFile(&g_timeZoneMap) == false)
		{
			int errorId = 8;
			string errorText =  "The MTZ file can't be accessed \n";
			setErrorText(ccbId, errorId, errorText);
			ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::deleted()");
			return ACS_CC_FAILURE;
		}
	}
	DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::deleted()");
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::modify()");

	(void)oiHandle;
	(void)ccbId;

	int Tz_value = -1;
	char Tz_name[NTZoneSize];
	ACE_OS::memset(Tz_name, 0, sizeof(Tz_name));

	char tmpObjName[MAX_DN_SIZE];
	ACE_OS::memset(tmpObjName,0, sizeof(tmpObjName));

	int i=0;
	while( attrMods[i] ){
		switch ( attrMods[i]->modAttr.attrType ) {
		case ATTR_STRINGT:
			if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TZ_STRING) == 0)
			{
				ACE_OS::strcpy(Tz_name, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
			}
			break;
		default:
			break;

		}
		i++;
	}

	//get the time zone number from object name.
	unsigned int j = 0;
	for( unsigned int i =0 ; i < ACE_OS::strlen(objName); i++)
	{
		if( isdigit(objName[i]))
		{
			tmpObjName[j++] = objName[i];
		}
		if( objName[i] == ',')
		{
			break;
		}
	}
	tmpObjName[j] = '\0';


	Tz_value = atoi(tmpObjName);

	DEBUG(1, "modify callback triggered for Tz_value %d", Tz_value );

	//Check if the name of time zone is greater than 200.
	if( ACE_OS::strlen(Tz_name) > (ACE_INT32)NTZoneSize )
	{
		int errorId = 5;
		string errorText = "Illegal operand TZ_name too long \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::modify()");
		return ACS_CC_FAILURE;
	}

	//Check the time zone.
	TIME_ZONE_INFORMATION Tzi;
	ACS_CHB_filehandler MTZObj;
	if(MTZObj.CheckTimeZone(Tz_name, &Tzi) == false)
	{
		int errorId = 6;
		string errorText = "Illegal operand ";
		errorText = errorText + Tz_name + " Is not a defined time zone\n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::modify()");
		return ACS_CC_FAILURE;
	}

	if( Tz_value < 0 || Tz_value > 23)
	{
		//Time zone number is not a valid time zone number
		int errorId = 3;
		string errorText = "Operand TMZ_value should be an integer between 0-23 \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::modify()");
		return ACS_CC_FAILURE;
	}

	g_timeZoneMap.erase(Tz_value);
	g_timeZoneMap.insert(FilePairType(Tz_value,ACS_CHB_NtZone(Tz_name, &Tzi)));

	if( MTZObj.WriteZoneFile(&g_timeZoneMap) == false)
	{
		int errorId = 8;
		string errorText =  "The MTZ file can't be accessed \n";
		setErrorText(ccbId, errorId, errorText);
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::modify()");
		return ACS_CC_FAILURE;
	}
	DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::modify()");
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::complete()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::complete()");
        return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void ACS_CHB_MTZLN_Implementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::abort()");
	(void)oiHandle;
	(void)ccbId;
        DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::abort()");
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::updateRuntime(const char*, const char*)
{
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::updateRuntime()");
        DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::updateRuntime()");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void ACS_CHB_MTZLN_Implementer :: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::adminOperationCallback()");
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;
        DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::adminOperationCallback()");
}//End of adminOperationCallback

/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_CHB_MTZLN_Implementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
        DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::apply()");
	(void)oiHandle;
	(void)ccbId;
	//do nothing
        DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::apply()");
}//End of

/*===================================================================
   ROUTINE: readTimeZones
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::readTimeZones()
{
	DEBUG ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::readTimeZones()");

	int filepathLength = 200;
	ACS_APGCC_CommonLib oComLib;
	char datadiskPath[200] ;
	while( oComLib.GetDataDiskPath("ACS_DATA", datadiskPath, filepathLength) != ACS_APGCC_DNFPATH_SUCCESS )
	{
		ERROR( 1, "%s","Error occured while getting data disk path");
		ERROR (1, "%s", "ACS_CHB_MTZLN_Implementer::readTimeZones - DATA DISK PATH is not configured !!");
		ERROR (1, "%s", "ACS_CHB_MTZLN_Implementer::readTimeZones - Retries after 10 sec");
		//ERROR (1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::readTimeZones()");
		//return ACS_CC_FAILURE;
		sleep(10);
		if(theDependancyFlag) return ACS_CC_FAILURE;
	}

	ACE_stat statBuff;

	while(1)
	{
		if( ACE_OS::stat(datadiskPath, &statBuff)  != 0 )
		{
			ERROR(1, "datadiskPath = %s", datadiskPath);
			ERROR(1, "Data Disk Read Failure. Last Error No: %d", ACE_OS::last_error());
			ERROR(1, "Last Error Text: %s", ACE_OS::strerror(ACE_OS::last_error()));
			sleep(10);
		}
		else
		{
			break;
		}
		if(theDependancyFlag) return ACS_CC_FAILURE;
	}

	theDependancyFlag = true;

	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	OmHandler omHandler;
	omHandler.Init();
	ACS_CHB_filehandler MTZObj1;
	bool result = MTZObj1.ReadTZsFromIMM( omHandler, &g_timeZoneMap );
	omHandler.Finalize();

	if( result == false )
	{
		ERROR(1, "%s", "Unable to read timezones. MTZObj.ReadTZsFromIMM() is failed ");
		ERROR ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::readTimeZones()");
		return ACS_CC_FAILURE;
	}

	if( MTZObj1.WriteZoneFile(&g_timeZoneMap) == false)
	{
		ERROR(1, "%s", "Node state is ACTIVE!!!But WriteZoneFile() Failed.");
		ERROR ( 1, "%s", "Entering ACS_CHB_MTZLN_Implementer::readTimeZones()");
		rc =  ACS_CC_FAILURE;
	}
	DEBUG ( 1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::readTimeZones()");
	return rc;
}

/*===================================================================
   ROUTINE: startMTZFunctionality
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::startMTZFunctionality()
{
        DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Implementer::startMTZFunctionality().");
        DEBUG(1, "%s", "Calling readTimeZones()");
	if(readTimeZones() != ACS_CC_SUCCESS)
	{
                ERROR(1, "%s", "readTimeZones() failed");
                ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::startMTZFunctionality().");
                return ACS_CC_FAILURE;
	}
	DEBUG(1, "%s", "Calling setObjectImplementer()");
	if(setObjectImplementer() != ACS_CC_SUCCESS)
	{
		ERROR(1, "%s", "setObjectImplementer() failed");
		ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::startMTZFunctionality().");
                return ACS_CC_FAILURE;
	}
	this->theReactor->open(1);
	dispatch(this->theReactor, ACS_APGCC_DISPATCH_ALL);
	this->theReactor->run_reactor_event_loop();
	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Implementer::startMTZFunctionality().");
 	return ACS_CC_SUCCESS;
}//End of startMTZFunctionality

/*===================================================================
   ROUTINE: MTZFunctionalMethod
=================================================================== */
ACE_THR_FUNC_RETURN ACS_CHB_MTZLN_Implementer::MTZFunctionalMethod(void* aMTZPtr)
{
	DEBUG(1, "%s", "Entering MTZFunctionalMethod().");

	if( ThrExitHandler::init() == false )
	{
		ERROR(1, "%s", "Error occured while registering exit handler");
		ERROR(1, "%s", "Leaving MTZFunctionalMethod()");
		return 0;
	}
	ACS_CHB_MTZLN_Implementer * myMTZLNImplementer = 0;
	myMTZLNImplementer = reinterpret_cast<ACS_CHB_MTZLN_Implementer*>(aMTZPtr);

	if(myMTZLNImplementer != 0)
	{
		DEBUG(1, "%s", "Calling startMTZFunctionality()");
                if(myMTZLNImplementer->startMTZFunctionality() == ACS_CC_FAILURE)
                {

                        ERROR(1, "%s", "startMTZFunctionality() failed");
                        ERROR(1, "%s", "Leaving MTZFunctionalMethod()");
                        return 0;
                }
	}
	else
	{
                ERROR(1, "%s", "myMTZLNImplementer is NULL");
                ERROR(1, "%s", "Leaving MTZFunctionalMethod()");
                return 0;
	}
	DEBUG(1, "%s", "Leaving MTZFunctionalMethod()");
	return 0;
}//End of MTZFunctionalMethod

/*===================================================================
   ROUTINE: setupMTZThread
=================================================================== */
ACS_CC_ReturnType ACS_CHB_MTZLN_Implementer::setupMTZThread(ACS_CHB_MTZLN_Implementer * aMTZLNImplementer)
{
	DEBUG(1, "%s", "Entering setupMTZThread()");
	const ACE_TCHAR* lpszThreadName1 = "MTZLNFunctionalThread";
	int mythread;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	DEBUG(1, "%s", "Spawning the MTZ Thread");
	mythread = ACE_Thread_Manager::instance()->spawn(&MTZFunctionalMethod,
							(void*)aMTZLNImplementer ,
							THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
							&mtzFuncThreadId,
							0,
							ACE_DEFAULT_THREAD_PRIORITY,
							-1,
							0,
							ACE_DEFAULT_THREAD_STACKSIZE,
							&lpszThreadName1);
	DEBUG(1, "mtzFuncThreadId = %ld", mtzFuncThreadId );
	if(mythread == -1)
	{
		ERROR(1, "%s", "Spawning of MTZ Functional thread failed");
		rc = ACS_CC_FAILURE;
	}
	DEBUG(1, "%s", "Leaving setupMTZThread()");
	return rc;

}//End of setupMTZThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void ACS_CHB_MTZLN_Implementer::shutdown()
{
	DEBUG(1, "%s", "Entering shutdown()");
	if(theDependancyFlag)
	{
		DEBUG(1, "%s", "Calling end_reactor_event_loop()");
		theReactor->end_reactor_event_loop();

		if( theoiHandler->removeClassImpl(this, theMTZClassName.c_str()) ==
				ACS_CC_FAILURE )
		{
			ERROR(1, "%s", "Error occured while removing class implementer.");
		}

	}
	else
	{
		theDependancyFlag = true;
	}
	DEBUG(1, "%s", "Leaving shutdown()");
}//End of shutdown

/*===================================================================
   ROUTINE: getReactor
=================================================================== */
ACE_Reactor* ACS_CHB_MTZLN_Implementer::getReactor()
{
        DEBUG(1, "%s", "Entering getReactor()");
        DEBUG(1, "%s", "Leaving getReactor()");
	return theReactor;
}//End of getReactor

/*===================================================================
	ROUTINE: ~ACS_CHB_MTZLN_Implementer
=================================================================== */
ACS_CHB_MTZLN_Implementer::~ACS_CHB_MTZLN_Implementer()
{
        DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Implementer Destructor");

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

	ThrExitHandler::cleanup();

	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Implementer Destructor");

}//End of Destructor

/*===================================================================
	ROUTINE: setErrorText
=================================================================== */
bool ACS_CHB_MTZLN_Implementer::setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText)
{
	(void) ccbId;
	ERROR ( 1, "Error Id: %d", aErrorId);
	ERROR ( 1, "Error Text: %s", ErrorText.c_str());
	setExitCode(aErrorId,ErrorText);
	return true;
}
