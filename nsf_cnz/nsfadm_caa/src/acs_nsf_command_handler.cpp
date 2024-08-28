/*=================================================================== */
/**
@file acs_nsf_command_handler.cpp

Class method implementation for ACS_NSF_Commandhandler class

@version 1.0.0

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/06/2010   XCHVKUM     Initial Release
N/A       07/05/2014   XQUYDAO     Update for cable-less environment
**/
/*===================================================================
                INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <ace/Signal.h>
#include <acs_apgcc_objectimplementerinterface_V3.h>
#include <acs_nsf_command_handler.h>
#include <saImmOi.h>
using namespace std;

/*===================================================================
   ROUTINE: ACS_NSF_CommandHandler
=================================================================== */
ACS_NSF_CommandHandler::ACS_NSF_CommandHandler( string szimpName )
:acs_apgcc_objectimplementereventhandler_V3( szimpName )
{
	m_poTPReactorImpl = 0;
	m_poReactor = 0;
	theOiHandlerPtr = 0;
	theObjCommonPtr = 0;
	m_szRequestedConfig.clear();
}

/*===================================================================
   ROUTINE: ACS_NSF_CommandHandler
=================================================================== */
ACS_NSF_CommandHandler::ACS_NSF_CommandHandler(string ClassName, ACS_NSF_Common * objCommon,string szImpName, ACS_APGCC_ScopeT enScope)
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope)
{
	theObjCommonPtr = objCommon;

	m_poTPReactorImpl = 0;
	m_poReactor = 0;
	theOiHandlerPtr = 0;

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

	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	if( theOiHandlerPtr == 0 )
	{
		ERROR("%s", "Memory allocation failed for  acs_apgcc_oihandler_V3");
	}
}

/*===================================================================
   ROUTINE: ~ACS_NSF_CommandHandler
=================================================================== */
ACS_NSF_CommandHandler::~ACS_NSF_CommandHandler()
{
	DEBUG("%s", "Entering ~ACS_NSF_CommandHandler");

	if( theOiHandlerPtr != 0 )
	{
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
	theObjCommonPtr = 0;

	DEBUG("%s", "Exiting ~ACS_NSF_CommandHandler");

 }

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 ACS_NSF_CommandHandler::svc( )
{
	//Add Class Implementer
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_CONFIG_IMM_NSF_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		ERROR("%s", "Error occurred while adding Object Implementer.");
		return -1;
	}
	else
	{
		DEBUG("%s", "Object Implementer has been added");

	}
	m_poReactor->open(1);

	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);

	m_poReactor->run_reactor_event_loop();

	DEBUG("%s", "Object Implementer Registered Successfully");
	return 0;
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_NSF_CommandHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;

	return ACS_CC_FAILURE;
}

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_NSF_CommandHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	return ACS_CC_FAILURE;
}

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_NSF_CommandHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	DEBUG("%s", "Entering ACS_NSF_CommandHandler::modify");

	(void)oiHandle;
	(void)objName;
	(void)ccbId;

	if (theObjCommonPtr->GetApgOamAccess() == 1)
	{
		// Reject all the changes related to NSF MOM in case running in cable-less environment
		setExitCode(1, "Not allowed operation for this system configuration.");
		DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify because no attribute is configurable in cable-less environment");
		return ACS_CC_FAILURE;
	}
			
	ACE_UINT32 i = 0;
	unsigned int myErrorCode = 0;
	std::string myErrorMessage;

	while( attrMods[i] )
	{
		if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_SURVELLIENCEACTIVEFLAG) == 0 )  )
		{
			DEBUG("%s", "Callback triggered for NSF_SURVELLIENCEACTIVEFLAG" );

			ACE_INT32* myNetworkSurvellienceValue = reinterpret_cast<ACE_INT32*> ( attrMods[i]->modAttr.attrValues[0]);
			if(theObjCommonPtr->getInitialSetFlag())
			{
				DEBUG("%s", "InitialSetFlag is true");
				return ACS_CC_SUCCESS;
			}
			else
			{
				if ( validateNSValue(*myNetworkSurvellienceValue,myErrorCode,myErrorMessage)	
							== false)
				{

					DEBUG("Error Code = %d",myErrorCode);
					DEBUG("Error Text = %s",myErrorMessage.c_str());
					setExitCode(myErrorCode, myErrorMessage);
					return ACS_CC_FAILURE;
				}

				return ACS_CC_SUCCESS;
			}
		}
#if 0		

		else if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_NOOFFAILOVER) == 0 )  )
		{
			DEBUG("%s", "Callback triggered for NSF_NOOFFAILOVER" );

			ACE_UINT32 *myNoOfFailoverValue = reinterpret_cast<ACE_UINT32*> (attrMods[i]->modAttr.attrValues[0]);
			if(theObjCommonPtr->getFailOverSetFlag())
			{
				return ACS_CC_SUCCESS;
			}
			else
			{
				if ((*myNoOfFailoverValue) == 0)
				{
					DEBUG("Error Code = %d",0);
					DEBUG("Error Text = %s","Failover counter reset");
					myErrorMessage.assign("Failover counter reset");
					setExitCode(0, myErrorMessage);
					return ACS_CC_SUCCESS;
				}
				else
				{
					DEBUG("Error Code = %d",2);
					DEBUG("Error Text = %s", "Invalid Value, Value must be 0");
					myErrorMessage.assign("Invalid Value, Value must be 0");
					setExitCode(2, myErrorMessage);
					return ACS_CC_FAILURE;
				}
			}
		}
		else if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_PING_PERIOD) == 0 )  )
		{
				DEBUG("%s", "Callback triggered for PingPeriod" );

				ACE_UINT32* myPingPeriod = reinterpret_cast<ACE_UINT32*> ( attrMods[i]->modAttr.attrValues[0]);
				if(validateChange(*myPingPeriod,(char *)NSF_PING_PERIOD))
				{
						DEBUG("%s", "validation of PingPeriod is success");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_SUCCESS;
				}
				else
				{
						DEBUG("%s", "validation of PingPeriod failed");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_FAILURE;
				}
		}
		else if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_ROUTER_RESPONSE) == 0 ) )
		{
				DEBUG("%s", "Callback triggered for RouterResponse" );

				ACE_UINT32 *myRouterResponseValue = reinterpret_cast<ACE_UINT32*> (attrMods[i]->modAttr.attrValues[0]);
				if(validateChange(*myRouterResponseValue,(char *)NSF_ROUTER_RESPONSE))
				{
						DEBUG("%s", "validation of RouterResponse is success");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_SUCCESS;
				}
				else
				{
						DEBUG("%s", "validation of RouterResponse failed");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_FAILURE;
				}
		}
		else if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_RESET_TIME) == 0 )  )
		{
				DEBUG("%s", "Callback triggered for ResetTime" );

				ACE_UINT32 *myResetTimeValue = reinterpret_cast<ACE_UINT32*> (attrMods[i]->modAttr.attrValues[0]);
				if(validateChange(*myResetTimeValue,(char *)NSF_RESET_TIME))
				{
						DEBUG("%s", "validation of ResetTime is success");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_SUCCESS;
				}
				else
				{
						DEBUG("%s", "validation of ResetTime failed");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_FAILURE;
				}
		}
		else if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_ACTIVE_FLAG) == 0 )  )
		{
				DEBUG("%s", "Callback triggered for ActiveFlag" );

				ACE_INT32 *myActiveFlagValue = reinterpret_cast<ACE_INT32*> (attrMods[i]->modAttr.attrValues[0]);
				if(validateChange(*myActiveFlagValue,(char *)NSF_ACTIVE_FLAG))
				{
						DEBUG("%s", "validation of ActiveFlag is success");
						//TR HP30402
						//Upadting the isSurveillanceEnabled without waiting for ping period and calculating startStopValue immeditely
						theObjCommonPtr->isSurveillanceEnabled = *myActiveFlagValue;
						if(theObjCommonPtr->calculateStartStopValue(theObjCommonPtr->isSurveillanceEnabled))
						{
							DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
							return ACS_CC_SUCCESS;
						}
				}	
				else
				{
					DEBUG("%s", "validation of ActiveFlag failed");
					DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
					return ACS_CC_FAILURE;
				}
		}
		else if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_FAIL_OVERS) == 0 )  )
		{
				DEBUG("%s", "Callback triggered for Failovers" );

				ACE_UINT32 *myNoOfFailoverValue = reinterpret_cast<ACE_UINT32*> (attrMods[i]->modAttr.attrValues[0]);
				if(validateChange(*myNoOfFailoverValue,(char *)NSF_FAIL_OVERS))
				{
						DEBUG("%s", "validation of Failovers is success");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_SUCCESS;
				}
				else
				{
						DEBUG("%s", "validation of Failovers failed");
						DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
						return ACS_CC_FAILURE;
				}
		}
#endif
		else if(( ACE_OS::strcmp(attrMods[i]->modAttr.attrName,NSF_FAILOVER_PRIORITY) == 0 )  )
		{
			DEBUG("%s", "Callback triggered for Failover Priority" );
			
			ACE_INT32 *myFailoverPriority = reinterpret_cast<ACE_INT32*> (attrMods[i]->modAttr.attrValues[0]);
			if (validateChange(*myFailoverPriority,(char *)NSF_FAILOVER_PRIORITY))
			{
				if (*myFailoverPriority == 1 && theObjCommonPtr->GetPSState() == 0)
				{
					// Reject change in case priority network is set to PS network 
					// while it's not configured yet
					setExitCode(1, "Not allowed operation for this system configuration.");
					DEBUG("%s", "validation of Failover Priority failed");
					DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
					return ACS_CC_FAILURE;
				}				
				
				DEBUG("%s", "validation of Failover Priority is success");
				DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
				return ACS_CC_SUCCESS;
			}
			else
			{
				DEBUG("%s", "validation of Failover Priority failed");
				DEBUG("%s", "Exiting ACS_NSF_CommandHandler::modify");
				return ACS_CC_FAILURE;
			}
		}
	i++;
	i++;
	}
	return ACS_CC_FAILURE;
}

/*===================================================================
   ROUTINE: validateNSValue
=================================================================== */
bool ACS_NSF_CommandHandler::validateNSValue(ACE_INT32 aNetworkSurvellienceValue,unsigned int& aErrorcode,std::string& aErrorMessage)
{
	DEBUG("%s", "Entering validateNSValue");

	if((aNetworkSurvellienceValue == 0)||(aNetworkSurvellienceValue == 1))
	{
		unsigned int myStartStopValue = theObjCommonPtr->getStartStopValue();
		DEBUG("Start Stop Value %d",myStartStopValue);
		switch (myStartStopValue)
		{
			case 0: // NSF Activeflag is not set
			{
				if(aNetworkSurvellienceValue == 0)
				{ //nsfstop is given
					aErrorcode = 0;
					aErrorMessage = "Network surveillance not active and not started";
					DEBUG("%s", "Leaving validateNSValue");
					return false;
				}
				else
				{ //nsfstart is given
					aErrorcode = 1;
					aErrorMessage = "Network surveillance not active. Cannot start";
					theObjCommonPtr->setStartStopValue(0);
					DEBUG("%s", "Leaving validateNSValue");
					return false;
				}
				break;
			}
			case 1:// NSF Activeflag is Set
			{
				if(aNetworkSurvellienceValue == 0)
				{//nsfstop is given
					aErrorcode = 0;
					aErrorMessage = "Network surveillance already stopped";
					DEBUG("%s", "Leaving validateNSValue");
					return false;
				}
				else
				{//nsfstart is given
					aErrorcode = 0;
					aErrorMessage = "Network surveillance started";
					theObjCommonPtr->setStartStopValue(3);
					theObjCommonPtr->isSurveillanceActive = aNetworkSurvellienceValue;
					DEBUG("theObjCommonPtr->isSurveillanceActive = %d",theObjCommonPtr->isSurveillanceActive);
					DEBUG("%s", "Leaving validateNSValue");
					return true;
				}
				break;
			}
			case 2://NSF Activeflag is not set
			{
				if(aNetworkSurvellienceValue == 0)
				{//nsfstop is given
					aErrorcode = 0;
					aErrorMessage = "Network surveillance stopped";
					theObjCommonPtr->setStartStopValue(0);
					theObjCommonPtr->isSurveillanceActive = aNetworkSurvellienceValue;
					DEBUG("theObjCommonPtr->isSurveillanceActive = %d",theObjCommonPtr->isSurveillanceActive);
					DEBUG("%s", "Leaving validateNSValue");
					return true;
				}
				else
				{
					aErrorcode = 1;
					aErrorMessage = "Network surveillance not active. Cannot start";
					DEBUG("%s", "Leaving validateNSValue");
					return false;
				}
				break;
			}
			case 3://NSF ActiveFlag is set
			{
				if(aNetworkSurvellienceValue == 0)
				{
					aErrorcode = 0;
					aErrorMessage = "Network surveillance stopped";
					theObjCommonPtr->setStartStopValue(1);
					theObjCommonPtr->isSurveillanceActive = aNetworkSurvellienceValue;
					DEBUG("theObjCommonPtr->isSurveillanceActive = %d",theObjCommonPtr->isSurveillanceActive);
					DEBUG("%s", "Leaving validateNSValue");
					return true;
				}
				else
				{
					aErrorcode = 0;
					aErrorMessage = "Network surveillance already started";
					DEBUG("%s", "Leaving validateNSValue");
					return false;
				}
				break;
			}
		}

	}
	aErrorcode = 2;
	aErrorMessage = "Illegal value for Network surveillance value";
	DEBUG("%s", "Leaving validateNSValue");
	return false;
}

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_NSF_CommandHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: abort
=================================================================== */
void ACS_NSF_CommandHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
}

/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_NSF_CommandHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	if( theObjCommonPtr != 0 )
	{
		theObjCommonPtr->setLoadParamsFlag(true);
	}
	(void)ccbId;
}

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_NSF_CommandHandler::updateRuntime(const char *objName, const char **attrName) 
{
	DEBUG("Object name = %s", objName);
	DEBUG("Attribute name = %s", *attrName);
	ACE_UINT32 failOverVal = 0;	
	int result = theObjCommonPtr->getRegWord(NSF_NOOFFAILOVER,failOverVal);

	if(result == 1)
	{
		DEBUG("%s", " Value not set to failOverCount");
		return ACS_CC_FAILURE;

	}

	ACS_CC_ImmParameter myParamToChange ;
	myParamToChange.attrName = (char*) NSF_NOOFFAILOVER ;
	myParamToChange.attrType = ATTR_UINT32T;
	myParamToChange.attrValuesNum = 1;
	void * myValues[1] = { reinterpret_cast<void*>(const_cast<unsigned int*>(&failOverVal)) };
	myParamToChange.attrValues = myValues;

	if (this->modifyRuntimeObj(objName, &myParamToChange ) == ACS_CC_FAILURE )
	{
		DEBUG("%s", " Unable to set failOverCount");
		return ACS_CC_FAILURE;
	}
	else
	{
		DEBUG("FailOverCount is successfully set to %d",failOverVal);
	}

	return ACS_CC_SUCCESS;

}

/*===================================================================
ROUTINE: adminOperationCallback
=================================================================== */
void ACS_NSF_CommandHandler::adminOperationCallback (ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation, 
		const char* p_objName, 
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{
	DEBUG("%s","Entering ACS_NSF_CommandHandler::adminOperationCallback");
	(void)paramList;
	string myClassName("");
	string errText = "@ComNbi@";
	string objName = string(p_objName);
	DEBUG("Object Name = %s",objName.c_str());
	OmHandler myOmHandler;
	SaAisErrorT resultOfOperation = SA_AIS_OK;
	if (myOmHandler.Init() == ACS_CC_FAILURE)
	{
		ERROR("%s","OmHandler initalization is failed");
		errText.append("Unable to connect to server");
		resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
	}
	else 
	{ 
		DEBUG("Operation ID received by adminOperationCallback is = %d",operationId);
		if(operationId == RESET_FAILOVER_COUNT)
		{
			DEBUG("%s", "Triggered admin operation for ResetFailOver Count");

			ACS_CC_ImmParameter myParamToChange ;
			myParamToChange.attrName = (char*) NSF_NOOFFAILOVER ;
			myParamToChange.attrType = ATTR_UINT32T;
			myParamToChange.attrValuesNum = 1;
			ACE_UINT32 myVal = 0 ;
			void * myValues[1] = { reinterpret_cast<void*>(const_cast<unsigned int*>(&myVal)) };
			myParamToChange.attrValues = myValues ;
			std::string nsfParamDN = string(ACS_NSF_PARAM_OBJ_DN);
			nsfParamDN.append(",");
			nsfParamDN.append(objName);

			DEBUG("The NSF RDN is %s", nsfParamDN.c_str());
			if (myOmHandler.modifyAttribute(nsfParamDN.c_str() , &myParamToChange ) == ACS_CC_FAILURE )
			{
				ERROR("%s", " Unable to reset failOverCount as omHandler modification is failed");
				errText.append("Unable to connect to server");
				resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
			}
			else
			{
				INFO("%s", " FailOverCount is successfully reset to 0");
			}

		}
		if(myOmHandler.Finalize() == ACS_CC_FAILURE)
		{
			ERROR("%s","OmHandler finalize failed");
			errText.append("Unable to connect to server");
			resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
		}
	}

	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;

	ACS_APGCC_AdminOperationParamType firstElem;

	/*create first Elemenet of param list*/
	char attName1[]= "errorCode";

	firstElem.attrName = attName1;
	firstElem.attrType = ATTR_STRINGT;
	char* errValue = const_cast<char*>(errText.c_str());
	firstElem.attrValues=reinterpret_cast<void*>(errValue);

	/*insert parameter into the vector*/
	if(resultOfOperation != SA_AIS_OK)
	{
		vectorOut.push_back(firstElem);
	}
	ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation,resultOfOperation ,vectorOut);
	if(rc != ACS_CC_SUCCESS)
	{
		ERROR("%s","Failure occurred in sending AdminOperation Result");
		ERROR("%s","Exiting ACS_NSF_CommandHandler::saImmOiAdminOperationCallback");
	}
	DEBUG("%s","Exiting ACS_NSF_CommandHandler::saImmOiAdminOperationCallback");
}

/*===================================================================
ROUTINE: shutdown
=================================================================== */
void ACS_NSF_CommandHandler::shutdown()
{
	DEBUG("%s", "Entering ACS_NSF_CommandHandler::shutdown");

	m_poReactor->end_reactor_event_loop();

	if( theOiHandlerPtr != 0 )
	{
		//remove class implementer
		DEBUG("%s", "Calling removeClassImpl.");
		theOiHandlerPtr->removeClassImpl(this,ACS_CONFIG_IMM_NSF_CLASS_NAME);
	}

	DEBUG("%s", "Leaving ACS_NSF_CommandHandler::shutdown");
}


/*===================================================================
ROUTINE: validateChange
=================================================================== */
bool ACS_NSF_CommandHandler::validateChange(ACE_UINT32 aParamValue, char* attrName)
{
	DEBUG("%s", "Entering validateChange");
	if(ACE_OS::strcmp(attrName, NSF_PING_PERIOD) == 0)
	{
		if (aParamValue >= 10  && aParamValue <= 300)
		{
			DEBUG("Given %s value is between 10 - 300 and validation passed",NSF_PING_PERIOD);
			DEBUG("%s", "Leaving validateChange");
			return true;
		}
		else
		{
			DEBUG("Given %s value is not between 10 - 300 and validation failed",NSF_PING_PERIOD);
			DEBUG("%s", "Leaving validateChange");
			return false;
		}
	}
	else if(ACE_OS::strcmp(attrName, NSF_ROUTER_RESPONSE) == 0)
	{
		if (aParamValue >= 1  && aParamValue <= 10)
		{

			DEBUG("Given %s value is between 1 - 10 and validation passed",NSF_ROUTER_RESPONSE);
			DEBUG("%s", "Leaving validateChange");
			return true;
		}
		else
		{
			DEBUG("Given %s value is not between 1 - 10 and validation failed",NSF_ROUTER_RESPONSE);
			DEBUG("%s", "Leaving validateChange");
			return false;
		}
	}
	else if(ACE_OS::strcmp(attrName, NSF_RESET_TIME) == 0)
	{
		if (aParamValue >= 20  && aParamValue <= 1440)
		{
			DEBUG("Given %s value is between 20 - 1440 and validation passed",NSF_RESET_TIME);
			DEBUG("%s", "Leaving validateChange");
			return true;
		}
		else
		{
			DEBUG("Given %s value is not between 20 - 1440 and validation failed",NSF_RESET_TIME);
			DEBUG("%s", "Leaving validateChange");
			return false;
		}
	}
	else if(ACE_OS::strcmp(attrName, NSF_FAIL_OVERS) == 0)
	{
		if ( aParamValue <= 65535)
		{

			DEBUG("Given %s value is between 0 - 65535 and validation passed",NSF_FAIL_OVERS);
			DEBUG("%s", "Leaving validateChange");
			return true;
		}
		else
		{
			DEBUG("Given %s value is not between 0 - 65535 and validation failed",NSF_FAIL_OVERS);
			DEBUG("%s", "Leaving validateChange");
			return false;
		}
	}
	else if(ACE_OS::strcmp(attrName, NSF_ACTIVE_FLAG) == 0)
	{
		//if (((ACE_INT32)aParamValue) >= 0  && aParamValue <= 1)
		if (((ACE_INT32)aParamValue) == 1)
		{
			DEBUG("Given %s value is 1 and validation passed",NSF_ACTIVE_FLAG);
			DEBUG("%s", "Leaving validateChange");
			return true;
		}
		else
		{
			DEBUG("Given %s value is not 1 and validation failed",NSF_ACTIVE_FLAG);
			DEBUG("%s", "Leaving validateChange");
			return false;
		}
	}
	else if(ACE_OS::strcmp(attrName, NSF_FAILOVER_PRIORITY) == 0)
	{
		if (((ACE_INT32)aParamValue) >= 0  && aParamValue <= 1)
		{
			DEBUG("Given %s value is between 0 - 1 and validation passed",NSF_FAILOVER_PRIORITY);
			DEBUG("%s", "Leaving validateChange");
			return true;
		}
		else
		{
			DEBUG("Given %s value is not between 0 - 1 and validation failed",NSF_ACTIVE_FLAG);
			DEBUG("%s", "Leaving validateChange");
			return false;
		}
	}
	DEBUG("%s", "Leaving validateChange");
	return false;
}
/*
   string ACS_NSF_CommandHandler::getExitCodeString()
   {
   DEBUG(1,"%s", "Entering acs_emf_commandhandler::getExitCodeString");
   DEBUG(1,"%s", "Exiting acs_emf_commandhandler::getExitCodeString");
   return myErrorMessage;
   }
 */
