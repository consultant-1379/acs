/*=================================================================== */
/**
  @file   acs_lm_northbound_objectImplementer.cpp

  @brief Header file for LM module.

  This module contains all the declarations useful to
  specify the class acs_lm_northbound_objectImplementer.

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
#include "acs_lm_northbound_objectImplementer.h"
#include "acs_lm_brfc_interface.h"

using namespace std;
//bool fpChangeClusterIP;
/*=================================================================
ROUTINE: acs_lm_northbound_objectImplementer constructor
=================================================================== */
	acs_lm_northbound_objectImplementer::acs_lm_northbound_objectImplementer(string szimpName )
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
	clusterAddress = "";

}//end of constructor

/*=================================================================
ROUTINE: acs_lm_northbound_objectImplementer destructor
=================================================================== */
acs_lm_northbound_objectImplementer::~acs_lm_northbound_objectImplementer()
{
	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_IMM_LM_northbound_CLASS_NAME);
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
ROUTINE: setLMNorthBoundObjectImplementer
=================================================================== */
ACS_CC_ReturnType acs_lm_northbound_objectImplementer::setLMNorthBoundObjectImplementer()
{
	return theOiHandlerPtr->addClassImpl(this,NORTHBOUND_NETWORK_MAIN_CLASS_NAME);
}//end of setLMNorthBoundObjectImplementer

/*=================================================================
ROUTINE: svc
=================================================================== */
ACE_INT32 acs_lm_northbound_objectImplementer::svc( ACE_Reactor *poReactor )
{
	DEBUG("%s","Entering acs_lm_northbound_objectImplementer::svc");

/*	for (int i=0; i<3; i++)
	{

		ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,NORTHBOUND_NETWORK_MAIN_CLASS_NAME);

		if ( errorCode == ACS_CC_SUCCESS )
		{
			DEBUG( "Set implementer for the Class %s  Success ",NORTHBOUND_NETWORK_MAIN_CLASS_NAME );
			dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);
			DEBUG("%s","Exiting acs_lm_northbound_objectImplementer::svc");
			return 0;

		}
		else
		{
			ERROR( "Set implementer for the Class %s  failure...Retrying for ",NORTHBOUND_NETWORK_MAIN_CLASS_NAME );
			usleep(10000);
		}
	}
	ERROR( "Set implementer for the Class %s  failure, Even after retries. Hence leaving",NORTHBOUND_NETWORK_MAIN_CLASS_NAME );
	return -1;  */

	dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);
	DEBUG("%s","Exiting acs_lm_northbound_objectImplementer::svc");
	return 0;
}//end of svc
/*=================================================================
ROUTINE: create
=================================================================== */

ACS_CC_ReturnType acs_lm_northbound_objectImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){


	DEBUG("acs_lm_northbound_objectImplementer %s"," ObjectCreateCallback invocated           \n");
	INFO ("oiHandle %ld ccbId %ld className %s parentName %s ACS_APGCC_AttrValues %u \n",oiHandle,ccbId,className,parentName,attr);
	DEBUG("acs_lm_northbound_objectImplementer %s"," ObjectCreateCallback Leaving           \n");

	return ACS_CC_SUCCESS;
}//end of create
/*=================================================================
ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_lm_northbound_objectImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	DEBUG("acs_lm_northbound_objectImplementer %s"," ObjectDeleteCallback invocated");
	INFO ("oiHandle %ld ccbId %ld objName %s\n",oiHandle,ccbId,objName);
	DEBUG("acs_lm_northbound_objectImplementer %s"," ObjectDeleteCallback Leaving");
	return ACS_CC_SUCCESS;
}//end of deleted
/*=================================================================
ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_lm_northbound_objectImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)attrMods;
	DEBUG ("acs_lm_northbound_objectImplementer %s","   ObjectmodifyCallback invocated           \n");
	DEBUG ("oiHandle %ld ccbId %ld objName %s\n",oiHandle,ccbId,objName);
	int i=0;
	while( attrMods[i] )
	{
		if( ACE_OS::strcmp( attrMods[i]->modAttr.attrName, NETWORK_CLUSTER_IP_ADDRESS ) == 0 )
		{
			DEBUG("Modify callback triggered for ClusterIpAddress for %s", objName);
			//Log Path should be a valid path.

			char *myClusterIpAddress = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
			INFO("The value of existing clusterAddress is %s", clusterAddress.c_str() );
			INFO("The new value of IMM ClusterIpAddress is %s", myClusterIpAddress);

			std::string clusterIPv4, clusterIPv6, clusterIPfromConfFile;
			ACS_LM_Common::getClusterIPfromConfFile(clusterIPv4, clusterIPv6);
			if(!clusterIPv4.empty() && !clusterIPv6.empty())			// Dual Stack IPs in format: "IPv4,IPv6"
				clusterIPfromConfFile = clusterIPv4 + "," + clusterIPv6;
			else if(!clusterIPv4.empty())					// IPv4 only
				clusterIPfromConfFile = clusterIPv4;
			else if(!clusterIPv6.empty())					// IPv6 only
				clusterIPfromConfFile = clusterIPv6;
			INFO("ClusterIP value from cluster.conf = %s", clusterIPfromConfFile.c_str());

			if(strcmp(myClusterIpAddress, clusterIPfromConfFile.c_str()) != 0)
			{
				strcpy(myClusterIpAddress,clusterIPfromConfFile.c_str());
				INFO("IMM clusterIP value is not aligned with cluster conf file : cluster IP = %s", myClusterIpAddress);
			}

			if(clusterAddress.empty())
			{
				clusterAddress = myClusterIpAddress;
				INFO("OI internal clusterAddress changed to %s", clusterAddress.c_str() );
			}
			if(theClienthandlerPtr->getVirginModeStatus() == false)
			{
				if(clusterAddress != myClusterIpAddress)
				{
					INFO("acs_lm_northbound_objectImplementer::modify() - currentClusterIP[%s] != newClusterIP[%s]",clusterAddress.c_str(),myClusterIpAddress);
					bool updateFpChangeFlag = true;
					string strNewClusterIPAddress(myClusterIpAddress);
					string currStackType="";
					string newStackType="";
					if(clusterAddress.find(".") != string::npos)
					{
						if((clusterAddress.find(",") != string::npos)
								&& (clusterAddress.find(":") != string::npos))	//  current Cluster IP is Dual Stack
							currStackType = "Dual";
						else													//  current Cluster IP is IPv4 only
							currStackType = "IPv4";
					}
					else if(clusterAddress.find(":") != string::npos)			//  current Cluster IP is IPv6 only
						currStackType = "IPv6";

					if(strNewClusterIPAddress.find(".") != string::npos)
					{
						if((strNewClusterIPAddress.find(",") != string::npos)	//  new Cluster IP is Dual Stack
								&& (strNewClusterIPAddress.find(":") != string::npos))
							newStackType = "Dual";
						else													//  new Cluster IP is IPv4 only
							newStackType = "IPv4";
					}
					else if(strNewClusterIPAddress.find(":") != string::npos)	//  new Cluster IP is IPv6 only
						newStackType = "IPv6";
					INFO("acs_lm_northbound_objectImplementer::modify() - currStackType[%s], newStackType[%s]",currStackType.c_str(),newStackType.c_str());
					if((currStackType == "IPv4") && (newStackType == "Dual"))	// IPv4 to IPv4,IPv6 - check that IPv4 address remains same
					{
						string newIPv4InDualStack = strNewClusterIPAddress.substr(0,strNewClusterIPAddress.find(","));
						if(clusterAddress == newIPv4InDualStack)
						{
							updateFpChangeFlag = false;
							INFO("acs_lm_northbound_objectImplementer::modify() - %s"," IPv4 to Dual Stack migration - but IPv4 address is same. DO NOT SET GRACE MODE!");
						}
					}
					else if((currStackType == "Dual") && (newStackType == "IPv4"))
					{
						string currIPv4InDualStack = clusterAddress.substr(0,clusterAddress.find(","));
						if(currIPv4InDualStack == strNewClusterIPAddress)
						{
							updateFpChangeFlag = false;
							INFO("acs_lm_northbound_objectImplementer::modify() - %s"," Dual Stack to IPv4 fallback - but IPv4 address is same. DO NOT SET GRACE MODE!");
						}
					}
					else if((currStackType == "Dual") && (newStackType == "IPv6"))
					{
						size_t posDelim = clusterAddress.find(",");
						string currIPv6InDualStack = clusterAddress.substr(posDelim + 1, clusterAddress.length() - posDelim);
						if(currIPv6InDualStack == strNewClusterIPAddress)
						{
							updateFpChangeFlag = false;
							INFO("acs_lm_northbound_objectImplementer::modify() - %s"," Dual Stack to IPv6 migration - but IPv6 address is same. DO NOT SET GRACE MODE!");
						}
					}
					else if((currStackType == "IPv6") && (newStackType == "Dual"))
					{
						size_t posDelim = strNewClusterIPAddress.find(",");
						string newIPv6InDualStack = strNewClusterIPAddress.substr(posDelim + 1, strNewClusterIPAddress.length() - posDelim);
						if(clusterAddress == newIPv6InDualStack)
						{
							updateFpChangeFlag = false;
							INFO("acs_lm_northbound_objectImplementer::modify() - %s"," IPv6 to Dual Stack fallback - but IPv6 address is same. DO NOT SET GRACE MODE!");
						}
					}

					//TODO: Dual Stack(IPv6 LKF installed) -> IPv6 only migration case to be handled in 3.3.3 US
					if(updateFpChangeFlag)
					{
						fpChangeClusterIP = true;
						INFO("acs_lm_northbound_objectImplementer %s","Grace mode will be set during next triggering activity \n");
					}
					//thePersistent->setLmMode(ACS_LM_MODE_GRACE, true);
					//theEventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
				}
				ACS_LM_Sentinel::cached = 0;
				ACS_LM_Sentinel::customExSize = 0;
				INFO("%s","acs_lm_northbound_objectImplementer::modify ACS_LM_Sentinel::customExSize = 0");
			}
			break;
		}
		i++;
	}
	DEBUG("acs_lm_northbound_objectImplementer %s","   ObjectmodifyCallback Leaving \n");
	return ACS_CC_SUCCESS;
}//end of modify
/*=================================================================
ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_lm_northbound_objectImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("acs_lm_northbound_objectImplementer %s","  CcbCompleteCallback invocated           \n");
	INFO ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);
	DEBUG("acs_lm_northbound_objectImplementer %s","  CcbCompleteCallback Leaving           \n");

	return ACS_CC_SUCCESS;

}//end of complete
/*=================================================================
ROUTINE: abort
=================================================================== */
void acs_lm_northbound_objectImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("acs_lm_northbound_objectImplementer %s"," CcbAbortCallback invocated           \n");
	INFO ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);
	DEBUG("acs_lm_northbound_objectImplementer %s"," CcbAbortCallback Leaving           \n");

}//end of abort
/*=================================================================
ROUTINE: apply
=================================================================== */
void acs_lm_northbound_objectImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	DEBUG("acs_lm_northbound_objectImplementer %s"," CcbApplyCallback invocated           \n");
	INFO ("oiHandle %ld ccbId %ld \n",oiHandle,ccbId);
	DEBUG("acs_lm_northbound_objectImplementer %s"," CcbApplyCallback Leaving           \n");
}//end of apply
/*=================================================================
ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_lm_northbound_objectImplementer::updateRuntime(const char *objName, const char **attrName)
{
	DEBUG("%s"," acs_lm_northbound_objectImplementer CcbUpdateRuntimeCallback Invocated");
	(void) objName;
	(void) attrName;
	DEBUG("%s"," acs_lm_northbound_objectImplementer CcbUpdateRuntimeCallback Leaving");
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

void acs_lm_northbound_objectImplementer::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)p_objName;
	(void)invocation;
	(void)oiHandle;
	(void)operationId;
	(void)paramList;

	DEBUG("%s","acs_lm_northbound_objectImplementer adminOperationCallback invocated");
	DEBUG("acs_lm_northbound_objectImplementer::adminOperationCallback() %s","Leaving");
}//end of adminOperationCallback
/*=================================================================
ROUTINE: setPersistant
=================================================================== */
void acs_lm_northbound_objectImplementer::setEventHandler(ACS_LM_EventHandler* aEventHandler)
{
	DEBUG("acs_lm_northbound_objectImplementer::setEventHandler() %s","Entering");
	theEventHandler = aEventHandler;
	DEBUG("acs_lm_northbound_objectImplementer::setEventHandler() %s","Leaving");
}//end of setPersistant

/*=================================================================
ROUTINE: setPersistant
=================================================================== */
void acs_lm_northbound_objectImplementer::setPersistant(ACS_LM_Persistent* aPersistant)
{
	DEBUG("acs_lm_northbound_objectImplementer::setPersistant() %s","Entering");
	thePersistent = aPersistant;
	DEBUG("acs_lm_northbound_objectImplementer::setPersistant() %s","Leaving");
}//end of setPersistant

void acs_lm_northbound_objectImplementer::setClienthandler(ACS_LM_ClientHandler* aClienthandler)
{
	DEBUG("acs_lm_root_objectImplementer::setClienthandler() %s","Entering");
	theClienthandlerPtr = aClienthandler;
	DEBUG("acs_lm_root_objectImplementer::setClienthandler() %s","Leaving");
}//end of setClienthandler
/*=================================================================
ROUTINE: setOmHandler
=================================================================== */
void acs_lm_northbound_objectImplementer::setOmHandler(OmHandler* omHandler)
{
	DEBUG("acs_lm_northbound_objectImplementer::setOmHandler() %s","Entering");
	theOmHandlerPtr = omHandler;
	DEBUG("acs_lm_northbound_objectImplementer::setOmHandler() %s","Leaving");
}//end of setOmHandler
/*=================================================================
ROUTINE: setErrorText
=================================================================== */
bool acs_lm_northbound_objectImplementer::setErrorText(unsigned int aErrorId, std::string ErrorText)
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
void acs_lm_northbound_objectImplementer::shutdown()
{
	DEBUG("%s", "Entering acs_lm_northbound_objectImplementer::shutdown.");

	INFO("%s", "Calling end_reactor_event_loop.");
	if( theOiHandlerPtr != NULL)
	{
		//Remove the implementer.
		INFO("%s", "Calling removeClassImpl.");
		//theOiHandlerPtr->removeClassImpl(this,NORTHBOUND_NETWORK_MAIN_CLASS_NAME);
		for (int i=0; i < 10; ++i)
		{
			if( theOiHandlerPtr->removeClassImpl(this,NORTHBOUND_NETWORK_MAIN_CLASS_NAME) == ACS_CC_FAILURE )
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
	DEBUG("%s", "Leaving acs_lm_northbound_objectImplementer::shutdown.");
	theReactor->end_reactor_event_loop();
	if( OIFuncThreadId != 0 )
	{
		ACE_Thread_Manager::instance()->join(OIFuncThreadId);
	}
}//End of shutdown


/*===================================================================
ROUTINE: setupLMOIThread 
=================================================================== */
ACS_CC_ReturnType acs_lm_northbound_objectImplementer::setupLMNorthboundOIThread(acs_lm_northbound_objectImplementer *almNorthboundObjectImplementer)
{

	DEBUG( "%s", "Entering setupObjectImplementer()");
	const ACE_TCHAR* lpszThreadName1 = "LMnorthboundObjImpThread";
	int mythread;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;
	OmHandler omHandler;
	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		ERROR("%s","OmHandler Init is failed in setupLMNorthboundOIThread");
	}
	ACS_CC_ImmParameter paramToFind;
	std::string myClusterIPAddressRdn(NETWORK_MAIN_OBJ_DN);
	DEBUG("Fetching %s from IMM", NETWORK_CLUSTER_IP_ADDRESS );
	paramToFind.attrName = (char*)NETWORK_CLUSTER_IP_ADDRESS;
	std::string pszAttrValue;
	if (omHandler.getAttribute(myClusterIPAddressRdn.c_str(), &paramToFind ) == ACS_CC_SUCCESS)
	{
		DEBUG("%s","getAttribute is success and Object is preset in IMM");
		if(paramToFind.attrValuesNum != 0)
		{
			pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
			std::string myclusteripaddress(pszAttrValue);
			DEBUG(" In acs_lm_northbound_objectImplementer::setupLMNorthboundOIThread %s","NetworkManagedElement attribute is defined");
			std::string ClusterIPfromConfFile,clusterIPv4,clusterIPv6;
			ACS_LM_Common::getClusterIPfromConfFile(clusterIPv4,clusterIPv6);
			if(!clusterIPv4.empty() && !clusterIPv6.empty())			// Dual Stack IPs in format: "IPv4,IPv6"
				ClusterIPfromConfFile = clusterIPv4 + "," + clusterIPv6;
			else if(!clusterIPv4.empty())					// IPv4 only
				ClusterIPfromConfFile = clusterIPv4;
			else if(!clusterIPv6.empty())					// IPv6 only
				ClusterIPfromConfFile = clusterIPv6;

			DEBUG("setupLMNorthboundOIThread : Cluster IP from cluster.conf is %s",ClusterIPfromConfFile.c_str());
			if( myclusteripaddress !=  ClusterIPfromConfFile)
			{
				myclusteripaddress = ClusterIPfromConfFile;
				DEBUG("setupLMNorthboundOIThread : IMM clusterIP not aligned with cluster conf file : cluster IP =%s", myclusteripaddress.c_str());
			}
			clusterAddress = myclusteripaddress;
			DEBUG("setupLMNorthboundOIThread : thread internal clusterAddress value is %s",clusterAddress.c_str());
		}
		else
		{
			DEBUG(" In acs_lm_northbound_objectImplementer::setupLMNorthboundOIThread %s","NetworkManagedElement attribute is not Defined");
		}
	}
	else
	{
		ERROR("%s","getAttribute is failed and Object is not preset in IMM");
	}
	INFO( "%s", "Spawning the ObjectImplementer Thread");
	mythread = ACE_Thread_Manager::instance()->spawn(&LMNorthboundOIFunc,
			(void*)almNorthboundObjectImplementer,
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
		ERROR("%s","OmHandler Finalize is failed in setupLMNorthboundOIThread");
	}
	DEBUG( "%s", "Leaving setupObjectImplementer()");
	return rc;
}

/*===================================================================
ROUTINE: LMNorthboundOIFunc
=================================================================== */
ACE_THR_FUNC_RETURN acs_lm_northbound_objectImplementer::LMNorthboundOIFunc(void* aLMOIPtr)
{
	DEBUG( "%s", "Entering LMnorthboundOIThread().");

	acs_lm_northbound_objectImplementer * myLMnorthboundObjImplementer = 0;
	myLMnorthboundObjImplementer= reinterpret_cast<acs_lm_northbound_objectImplementer*>(aLMOIPtr);

	if(myLMnorthboundObjImplementer!= 0)
	{
		DEBUG( "%s", "Calling lmNorthboundOI_svc()");
		if(myLMnorthboundObjImplementer->lmNorthboundOI_svc() == ACS_CC_FAILURE)
		{

			ERROR( "%s", "lmNorthboundOI_svc() failed");
			ERROR( "%s", "Leaving LMNorthboundOIThread()");
			return 0;
		}
	}
	else
	{
		ERROR( "%s", "myLMnorthboundObjImplementer is NULL");
		ERROR( "%s", "Leaving LMnorthboundOIThread()");
		return 0;
	}
	DEBUG( "%s", "Leaving LMnorthboundOIThread()");
	return 0;
}//End of LMnorthboundOIThread

ACE_INT32 acs_lm_northbound_objectImplementer::lmNorthboundOI_svc()
{
	DEBUG("%s","Entering acs_lm_northbound_objectImplementer::lmNorthboundOI_svc");
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
		WARNING("%s","Exception has been caught in acs_lm_northbound_objectImplementer::lmNorthboundOI_svc");
	}
	DEBUG("%s","Exiting acs_lm_northbound_objectImplementer::lmNorthboundOI_svc");
	return 0;

}

void acs_lm_northbound_objectImplementer::setExitCode(int error, std::string text)
{
	DEBUG("%s", "Entering acs_lm_northbound_objectImplementer::setExitCode");

	errorCode = error;

	errorString = text;

	DEBUG("%s", "Exiting acs_lm_northbound_objectImplementer::setExitCode");
}



int acs_lm_northbound_objectImplementer::getExitCode()
{
	DEBUG("%s", "Entering acs_lm_northbound_objectImplementer::getExitCode");

	DEBUG("%s", "Exiting acs_lm_northbound_objectImplementer::getExitCode");

	return errorCode;
}

