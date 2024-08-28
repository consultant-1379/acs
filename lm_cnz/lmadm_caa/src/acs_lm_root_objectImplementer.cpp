/*=================================================================== */
/**
   @file   acs_lm_root_objectImplementer.cpp

   @brief Header file for LM module.

          This module contains all the declarations useful to
          specify the class acs_lm_root_objectImplementer.

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
#include <string>
#include <ace/Signal.h>
#include <saImmOi.h>
#include <acs_apgcc_oihandler_V3.h>
#include "acs_lm_defs.h"
#include "acs_lm_tra.h"
#include "acs_lm_root_objectImplementer.h"
#include "acs_lm_brfc_interface.h"
#include "acs_lm_common_util.h"/* HW-Decoupling: Fetching cluster IP address from APGCC  */
using namespace std;

bool acs_lm_root_objectImplementer::isLmActionInProgress = false;

/*=================================================================
	ROUTINE: acs_lm_root_objectImplementer constructor
=================================================================== */
acs_lm_root_objectImplementer::acs_lm_root_objectImplementer(string szimpName )
:acs_apgcc_objectimplementereventhandler_V3( szimpName ),
 theElectronicKeyLMRuntimeOwnerPtr(0),
 theClienthandlerPtr(0),
 theOmHandlerPtr(0)
{
	thetp_reactor_impl = new ACE_TP_Reactor;
	theReactor = new ACE_Reactor(thetp_reactor_impl);
	theReactor->open(1);
	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	OIFuncThreadId =0;
	errorCode = 0;
	errorString = "";
	strcpy(errorText,"");  /* LM Maintenance Mode */

}//end of constructor

/*=================================================================
	ROUTINE: acs_lm_root_objectImplementer destructor
=================================================================== */
acs_lm_root_objectImplementer::~acs_lm_root_objectImplementer()
{
	if( theOiHandlerPtr != 0 )
	{
		//theOiHandlerPtr->removeClassImpl(this,ACS_IMM_LM_ROOT_CLASS_NAME);
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


bool acs_lm_root_objectImplementer::getLmActionStatus()
{
	return isLmActionInProgress;
}

/*=================================================================
	ROUTINE: getMonthString
=================================================================== */
std::string acs_lm_root_objectImplementer::getMonthString(int aMonth)
{
	switch(aMonth)
	{
	case 1:
		return "JAN";
	case 2:
		return "FEB";
	case 3:
		return "MAR";
	case 4:
		return "APR";
	case 5:
		return "MAY";
	case 6:
		return "JUN";
	case 7:
		return "JUL";
	case 8:
		return "AUG";
	case 9:
		return "SEP";
	case 10:
		return "OCT";
	case 11:
		return "NOV";
	case 12:
		return "DEC";
	default:
		return "NULL";
	}
}//end of getMonthString

/*=================================================================
	ROUTINE: setLMRootObjectImplementer
=================================================================== */
ACS_CC_ReturnType acs_lm_root_objectImplementer::setLMRootObjectImplementer()
{
	return theOiHandlerPtr->addClassImpl(this,ACS_IMM_LM_ROOT_CLASS_NAME);
}//end of setLMRootObjectImplementer

/*=================================================================
	ROUTINE: svc
=================================================================== */
ACE_INT32 acs_lm_root_objectImplementer::svc( ACE_Reactor *poReactor )
{
	DEBUG("%s","Entering acs_lm_root_objectImplementer::svc");

/*	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_IMM_LM_ROOT_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		DEBUG( "Set implementer for the Class %s  failure ",ACS_IMM_LM_ROOT_CLASS_NAME );
		//return -1;
	}*/

	dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);
	DEBUG("%s","Exiting acs_lm_root_objectImplementer::svc");
	return 0;
}//end of svc
/*=================================================================
	ROUTINE: create
=================================================================== */

ACS_CC_ReturnType acs_lm_root_objectImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){


	INFO ("acs_lm_root_objectImplementer %s"," ObjectCreateCallback invocated           \n");
	DEBUG ("oiHandle %ld ccbId %ld className %s parentName %s ACS_APGCC_AttrValues %u \n",oiHandle,ccbId,className,parentName,attr);
	INFO ("acs_lm_root_objectImplementer %s"," ObjectCreateCallback Leaving           \n");

	return ACS_CC_FAILURE;
}//end of create
/*=================================================================
	ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_lm_root_objectImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	INFO ("acs_lm_root_objectImplementer %s"," ObjectDeleteCallback invocated");
	DEBUG ("oiHandle %ld ccbId %ld objName %s\n",oiHandle,ccbId,objName);
	INFO ("acs_lm_root_objectImplementer %s"," ObjectDeleteCallback Leaving");
	return ACS_CC_FAILURE;
}//end of deleted
/*=================================================================
	ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_lm_root_objectImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)attrMods;
	INFO ("acs_lm_root_objectImplementer %s","   ObjectmodifyCallback invocated           \n");
	DEBUG ("oiHandle %ld ccbId %ld objName %s\n",oiHandle,ccbId,objName);
	/* LM Maintenance Mode */
	/*====================================================================
	  maintenanceUnlock = LOCKED --> 0
	  maintenanceUnlock = UNLOCKED --> 1
	=====================================================================*/
        ACE_INT32 i= 0;
	ACE_UINT32 aMaintenanceFlag = 0;

	ACS_LM_BrfImplementer brfImplementer;
        bool brfStatus = brfImplementer.getBrfStatus();
	
	DEBUG("%s","Before if(!brfStatus)  for modfy call back");

        if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, MAINTENANCE_UNLOCK) == 0 )
	{
		DEBUG("%s","In if(!brfStatus)  for modfy call back");
		if(!isFromAutoCease)
		{
			if(!brfStatus)
			{

				aMaintenanceFlag = *((int*)(attrMods[i]->modAttr.attrValues[0]));  
				ACS_LM_AppExitCode myResult = theClienthandlerPtr->handleLkMaintenanceStartViaIMM(aMaintenanceFlag);

				if( myResult != ACS_LM_RC_OK)
				{
					if ( myResult == ACS_LM_RC_MAINTENANCEMODENOTALLOWED )
					{
						INFO("%s\n", "Maintenance mode not allowed error.");
						strcpy(errorText,"Maintenance Mode not allowed"); 
						setErrorCode(10);
					}
					else if ( myResult == ACS_LM_RC_MAINTENANCEMODEALREADYACTIVATED )
					{
						INFO("%s\n", "Maintenance Mode Already Unlocked");
						strcpy(errorText, "Maintenance Mode Already Unlocked");
						setErrorCode(11);
					}
					else if ( myResult == ACS_LM_RC_MAINTENANCEMODEALREADYDEACTIVATED )
					{
						INFO("%s\n", "Maintenance Mode Already Locked");
						strcpy(errorText, "Maintenance Mode Already Locked");
						setErrorCode(12);
					}
					else
					{
						INFO("%s\n", "in else");
					}
					INFO ("acs_lm_root_objectImplementer %s","   ObjectmodifyCallback Leaving \n");
					return ACS_CC_FAILURE;
				}
			}
			else
			{
				INFO("%s\n", "AP Backup on going - Maintenance Mode Lock/Unlock Not Allowed ");
				strcpy(errorText, "AP Backup on going - Maintenance Mode  Lock/Unlock Not Allowed ");
				setErrorCode(13);
				return ACS_CC_FAILURE;
			}
		} //if(!isFromAutoCease)
		else
		{
			return ACS_CC_SUCCESS;
		}

	}
	INFO ("acs_lm_root_objectImplementer %s","   ObjectmodifyCallback Leaving \n");

	return ACS_CC_SUCCESS;
}
//end of modify
/*=================================================================
	ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_lm_root_objectImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	INFO ("acs_lm_root_objectImplementer %s","  CcbCompleteCallback invocated           \n");
	DEBUG ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);
	INFO ("acs_lm_root_objectImplementer %s","  CcbCompleteCallback Leaving           \n");

	return ACS_CC_SUCCESS;

}//end of complete
/*=================================================================
	ROUTINE: abort
=================================================================== */
void acs_lm_root_objectImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	INFO ("acs_lm_root_objectImplementer %s"," CcbAbortCallback invocated           \n");
	DEBUG ("oiHandle %ld ccbId %ld\n",oiHandle,ccbId);
	INFO ("acs_lm_root_objectImplementer %s"," CcbAbortCallback Leaving           \n");

}//end of abort
/*=================================================================
	ROUTINE: apply
=================================================================== */
void acs_lm_root_objectImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	INFO ("acs_lm_root_objectImplementer %s"," CcbApplyCallback invocated           \n");
	DEBUG ("oiHandle %ld ccbId %ld \n",oiHandle,ccbId);
	INFO ("acs_lm_root_objectImplementer %s"," CcbApplyCallback Leaving           \n");
}//end of apply
/*=================================================================
	ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_lm_root_objectImplementer::updateRuntime(const char *objName, const char **attrName)
{
	(void) objName;
	(void) attrName;
	/* To get dispalyed in the end date attribute, as LM triggers at mid night scheduling time(i.e, 00:05:00) */
	std::string strTemp = LM_MIDNIGHT_TRIGGERING_TIME; 
	DEBUG ("%s"," acs_lm_root_objectImplementer CcbUpdateRuntimeCallback invocated");
	std::string myIPv4Fingerprint(""),myIPv6Fingerprint(""),myFingerprint("");
	//call the generateFingerPrint to get the finger print value
	/* HW-Decoupling: Ignoring "generateFingerPrint" method defined on "ACS_LM_Common" class.*/
	bool myFingerPrintFlag= ACS_LM_Common_Util::generateFingerPrintForLicenseM(myIPv4Fingerprint,myIPv6Fingerprint,theOmHandlerPtr);
	if(!myFingerPrintFlag)
	{
		ERROR("ACS_LM_RuntimeOwner::updateCallback() %s","generateFingerPrintForLicenseM() FAILED");
		return ACS_CC_FAILURE;
	}

	bool isDualStack = false;
	if(!myIPv4Fingerprint.empty() && !myIPv6Fingerprint.empty())
	{
                // IPv6 Spillover - Removed IPv6 finger print value in case of dual stack.
		myFingerprint = myIPv4Fingerprint;
		isDualStack = true;
	}
	else if(!myIPv4Fingerprint.empty())
		myFingerprint = myIPv4Fingerprint;
	else if(!myIPv6Fingerprint.empty())
		myFingerprint = myIPv6Fingerprint;
	(void) attrName;
	ACS_CC_ImmParameter parToModify;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	char* myFingerPrintValue = const_cast<char*>(myFingerprint.c_str());
	// Update fingerPrint
	parToModify.attrName = (ACE_TCHAR *)FINGERPRINT_ATTRIBUTE;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* attrValue[1] = {reinterpret_cast<void*>(myFingerPrintValue)};
	parToModify.attrValues = attrValue;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		DEBUG("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of fingerPrint is failed");
		result =  ACS_CC_FAILURE;
	}

	/* Display Fingerprint in hashed format*/
	std::string hashedFingerPrint = theClienthandlerPtr->getHashedFingerprint(isDualStack);

	char* hashedFingerPrintValue = const_cast<char*>(hashedFingerPrint.c_str());
        parToModify.attrName = (ACE_TCHAR *)HASHEDFINGERPRINT_ATTRIBUTE;
        parToModify.attrType = ATTR_STRINGT;
        parToModify.attrValuesNum = 1;
        void* hfpattrValue[1] = {reinterpret_cast<void*>(hashedFingerPrintValue)};
        parToModify.attrValues = hfpattrValue;

        if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
        {
                DEBUG("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of hashedFingerprint is failed");
                result =  ACS_CC_FAILURE;
        }

	/* LM Maintenance Mode myMaintenanceMode bool added*/
	bool myGraceMode = false,myEmergencyMode = false,myTestMode = false,myVirginMode = false,myMaintenanceMode = false; 
	std::string myVirginModeStatus("NO");
	std::string myGraceModeStatus("NO");
	std::string myEmergencyModeStatus("NO");
	std::string myEndDateforEmergencyMode("");
        std::string myEndDateforMaintenanceMode(""); 
	std::string myTestModeStatus("NO");
	//	std::string myLMMode("");
	int myLMMode;
	theClienthandlerPtr->getLMMode(myGraceMode,myEmergencyMode,myTestMode,myMaintenanceMode);
	INFO(" In acs_lm_root_objectImplementer::updateRuntime() Gracemode=%d..EmergencyMode=%d..TestMode=%d..MaintenanceMode=%d\n", myGraceMode,myEmergencyMode,myTestMode,myMaintenanceMode);
	if(myGraceMode)
	{
		myGraceModeStatus = "YES";
		//		myLMMode = "GRACE MODE";
		myLMMode=GRACEMODE;
	}

	if(myTestMode)
	{
		myTestModeStatus = "YES";
		if(myEmergencyMode)
		{
			myLMMode = EMERGENCYMODE;
		}
		else
		{
			myLMMode = LICENSECONTROLLEDMODE;
		}
	}

	/* LM Maintenance Mode */
        if(myMaintenanceMode)
        {
                myLMMode = MAINTENANCEMODE;
                ACE_INT64 myValue = theClienthandlerPtr->getMaintenanceModeEndDate();
                time_t myMaintEndDate = myValue;
                struct tm maintEndTimeLocal = *localtime(&myMaintEndDate);
                std::ostringstream myOstr;
                //myOstr << maintEndTimeLocal.tm_mday<<"-"<<getMonthString(maintEndTimeLocal.tm_mon+1)<<"-"<<(maintEndTimeLocal.tm_year+1900);
		myOstr<<std::setw(4)<<std::setfill('0')<<(maintEndTimeLocal.tm_year+1900)<<"-";
                myOstr<<std::setw(2)<<std::setfill('0')<<(maintEndTimeLocal.tm_mon+1)<<"-";
                myOstr<<std::setw(2)<<std::setfill('0')<<(maintEndTimeLocal.tm_mday);
                myEndDateforMaintenanceMode = myOstr.str();
        }

	if(myEmergencyMode)
	{
		//		myLMMode = "EMERGENCY MODE";
		myLMMode=EMERGENCYMODE;
		myEmergencyModeStatus = "YES";
		ACE_INT64  myValue  = theClienthandlerPtr->getEmergencyModeEndDate();
		time_t myEmEndDate = myValue;
		struct tm emEndTimeLocal = *localtime(&myEmEndDate);
		std::ostringstream myOstr;
		//myOstr << emEndTimeLocal.tm_mday<<"-"<<getMonthString(emEndTimeLocal.tm_mon+1)<<"-"<<(emEndTimeLocal.tm_year+1900);
		//myOstr << (emEndTimeLocal.tm_year+1900)<<"-"<<emEndTimeLocal.tm_mon+1<<"-"<<emEndTimeLocal.tm_mday;
		myOstr<<std::setw(4)<<std::setfill('0')<<(emEndTimeLocal.tm_year+1900)<<"-";
		myOstr<<std::setw(2)<<std::setfill('0')<<(emEndTimeLocal.tm_mon+1)<<"-";
		myOstr<<std::setw(2)<<std::setfill('0')<<(emEndTimeLocal.tm_mday);
		myEndDateforEmergencyMode = myOstr.str();
	}
      
	if(theClienthandlerPtr->getVirginModeStatus() == true)
	{
		//		myLMMode = "VIRGIN MODE";
		myLMMode = VIRGINMODE;
		myVirginModeStatus = "YES";
	}
	if(!myGraceMode && !myVirginMode && !myEmergencyMode && !myTestMode && !myMaintenanceMode)
	{
		//		myLMMode = "NORMAL MODE";
		myLMMode = LICENSECONTROLLEDMODE;

	}
	int myEmergencyActivationCount = theClienthandlerPtr->getEmergencyCount();

	if( myLMMode == EMERGENCYMODE || !myEndDateforEmergencyMode.empty())
			{
	std::replace(myEndDateforEmergencyMode.begin(),myEndDateforEmergencyMode.end(),'/','-');
		myEndDateforEmergencyMode.append(strTemp);
			}
	char* myEmEndDate = (char*) myEndDateforEmergencyMode.c_str();
	//STUB:END
	parToModify.attrName = (ACE_TCHAR *) EMERGENCYENDDATE_ATTRIBUTE;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* dateValue[1] = {reinterpret_cast<void*>(myEmEndDate)};
	parToModify.attrValues = dateValue;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of endDateforEmergencyMode is failed");
		result =  ACS_CC_FAILURE;
	}

	/* LM Maintenance Mode */
        if( myLMMode == MAINTENANCEMODE || !myEndDateforMaintenanceMode.empty())
        {
        	std::replace(myEndDateforMaintenanceMode.begin(),myEndDateforMaintenanceMode.end(),'/','-');
                myEndDateforMaintenanceMode.append(strTemp);
        }
        char* myMaintEndDate = (char*) myEndDateforMaintenanceMode.c_str();
        //STUB:END
        parToModify.attrName = (ACE_TCHAR *) MAINTENANCEENDDATE_ATTRIBUTE;
        parToModify.attrType = ATTR_STRINGT;
        parToModify.attrValuesNum = 1;
        void* dateValue1[1] = {reinterpret_cast<void*>(myMaintEndDate)};
        parToModify.attrValues = dateValue1;

        if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
        {
                DEBUG("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of endDateforMaintenanceMode is failed");
                result =  ACS_CC_FAILURE;
        }

	int * myLMode = new int(myLMMode);
	//STUB:END
	parToModify.attrName = (ACE_TCHAR *)LMMODE_ATTRIBUTE;
	parToModify.attrType = ATTR_INT32T;
	parToModify.attrValuesNum = 1;
	void* myLMode1[1] = {reinterpret_cast<void*>(myLMode)};
	parToModify.attrValues = myLMode1;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of LMMode is failed");
		result =  ACS_CC_FAILURE;
	}
	else
	{
		DEBUG("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of LMMode is successful");
	}

	// Update emergencyActivatationCount
	int * myEmCount = new int(myEmergencyActivationCount);
	parToModify.attrName = (ACE_TCHAR *) EMERGENCYACTIVATIONCOUNT_ATTRIBUTE;
	parToModify.attrType = ATTR_UINT32T;
	parToModify.attrValuesNum = 1;
	void* emValue[1] = {reinterpret_cast<void*>(myEmCount)};
	parToModify.attrValues = emValue;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		ERROR("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of emergencyActivatationCount is failed");
		result =  ACS_CC_FAILURE;
	}
	else
	{
		DEBUG("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of emergencyActivatationCount is Successful");
	}

	DEBUG ("%s"," acs_lm_root_objectImplementer CcbUpdateRuntimeCallback Leaving           \n");
	return result;
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
bool acs_lm_root_objectImplementer::isObjectExiststheLMInfo(std::string aRdnName, std::string dn, bool& isObjectAvailable)
{
	INFO("%s","Entering acs_lm_root_objectImplementer::isObjectExiststheLMInfo");
	OmHandler omHandler;
	bool myResult = true;
	ACS_CC_ImmParameter paramToFind;
	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		ERROR("%s","OmHandler Init is failed in isObjectExiststheLMInfo");
		isObjectAvailable = false;
		return false;
	}
	paramToFind.attrName = new char[30];
	ACE_OS::strcpy(paramToFind.attrName,aRdnName.c_str());
	if (omHandler.getAttribute(dn.c_str(), &paramToFind ) == ACS_CC_FAILURE)
	{
		ERROR("%s","getAttribute is failed and Object is not preset in IMM");
		isObjectAvailable = false;
	}
	else
	{
		DEBUG("%s","getAttribute is successful and Object is already preset in IMM");
		isObjectAvailable = true;
	}

	if (omHandler.Finalize() == ACS_CC_FAILURE)
	{
		myResult = false;
	}

	delete [] paramToFind.attrName;
	INFO("%s","Leaving acs_lm_root_objectImplementer::isObjectExiststheEMFInfoClassName");
	return myResult;
}//end of isObjectExiststheLMInfo
void acs_lm_root_objectImplementer::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)p_objName;
	DEBUG ("%s","acs_lm_root_objectImplementer adminOperationCallback invocated");

	std::vector<std::string> myArgumentList;
	SaAisErrorT resultOfOperation = SA_AIS_OK;
	std::string myLKFObsolutePath;
	std::string myLKFFilePath;

	ACS_LM_BrfImplementer brfImplementer;

       std::string errorMsgToShow = "@ComNbi@"; //change for LM-Erron Handling

	bool brfStatus = brfImplementer.getBrfStatus();

		switch (operationId)
		{
			case LM_INSTALL_LSK_FILE_ACTION:
				{
				if(!brfStatus)
				{	
					//isLmActionInProgress = true;
					int i = 0;
					DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","LM_INSTALL_LSK_FILE_ACTION Invoked by User");
					// To fetch paramList
					//while( paramList[i] )
					//{
						switch ( paramList[i]->attrType )
						{
							/*case ATTR_INT32T:
								{
									DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() - %s","Integer given as argument - Not expected");
									break;
								}*/
							case ATTR_STRINGT:
								{
									if( ACE_OS::strcmp(paramList[i]->attrName, LKF_FILEPATH_ATTRIBUTE) == 0)
									{
										DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() - %s","String given as argument");
										std::string myStr( reinterpret_cast<char *>(paramList[i]->attrValues));
										myLKFFilePath = myStr; //exact file name

										if((ACS_LM_Common::getObsoluteFilePath(myLKFFilePath, myLKFObsolutePath)))
										{
											if(!ACS_LM_Common::isFileExists(myLKFObsolutePath))
											{
												resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
												//Error string is not clear. Modified the string to more specific to the error scenario 
												std::string myErrStr = myLKFFilePath;
												myErrStr += " is Missing";
												ERROR("acs_lm_common::isFileExists : myLKFObsolutePath = %s file not found", myLKFObsolutePath.c_str());

												errorMsgToShow.append(myErrStr); //Change for LM-Error Handling

												if( !setErrorText(ACS_LM_RC_MISSINGLKF, myErrStr) )
												{
													DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
												}
												break;
											}

										}
										else
										{
											resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
											std::string myErrStr = "Unable to connect to server";
											errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
											if( !setErrorText(ACS_LM_RC_SERVERNOTRESPONDING, myErrStr) )
											{
												DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
											}
											ERROR("%s", "getObsoluteFilePath() failed in acs_lm_root_objectImplementer::adminOperationCallback()");
											break;
										}
									}
                                                                        i++;
									break;
								}
							default:
								{
									DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","Argument is unexpected type");
									resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
									//std::string myErrStr = ACS_LM_Common::getApplicationErrorText(ACS_LM_RC_OTHERERROR);
									std::string myErrStr = "Unable to connect to server";
								    errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
									if( !setErrorText(ACS_LM_RC_SERVERNOTRESPONDING, myErrStr) )
									{
										DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
									}
									break;
								}
						}
						

					//}
					if (i == 1)
					{
						ACS_LM_AppExitCode errCode = theClienthandlerPtr->handleLkInstViaIMM(myLKFObsolutePath);
						if ( errCode != ACS_LM_RC_OK)
						{
							DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() theClienthandlerPtr->handleLkInstViaIMM(myLKFFilePath = %s) Failed",myLKFFilePath.c_str() );
							resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
							std::string myErrStr = ACS_LM_Common::getApplicationErrorText(errCode);
                                                        errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
							if( !setErrorText(errCode, myErrStr) )
							{
								ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
							}
						}
						else
						{
							bool myRes = theElectronicKeyLMRuntimeOwnerPtr->createRuntimeObjectElectronicKeyObjects();
							if(myRes == false)
							{
								ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s","theElectronicKeyLMRuntimeOwnerPtr->createRuntimeObjectElectronicKeyObjects() Failed");
                                                                std::string myErrStr = "Unable to connect to server";
                                                                errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
								resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
								std::string myErrStr_new = ACS_LM_Common::getApplicationErrorText(ACS_LM_RC_NOK);
								if( !setErrorText(ACS_LM_RC_SERVERNOTRESPONDING, myErrStr) )
								{
									ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
								}
							}
							else
							{
								resultOfOperation = SA_AIS_OK;
								std::string myErrStr = ACS_LM_Common::getApplicationErrorText(errCode);
								//setExitCode(ACS_LM_RC_OK, "");
								DEBUG("success case myErrStr =  %s",myErrStr.c_str());
								if( !setErrorText(errCode, myErrStr) )
								{
									ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
								}
							}
						}

					}
					/*else
					{
						DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","INVALID paramList");
						resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
						//std::string myErrStr = ACS_LM_Common::getApplicationErrorText(ACS_LM_RC_OTHERERROR);
						std::string myErrStr = "Unable to connect to server";
						if(errorMsgToShow == "@ComNbi@")
						{
						errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
						if( !setErrorText(ACS_LM_RC_SERVERNOTRESPONDING, myErrStr) )
						{
							ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
						}
						}
					}*/
					}
					else
					{
						resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
                                                DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","Backup and Restore Operation in Progress");
                                                std::string myErrStr = ACS_LM_Common::getApplicationErrorText(ACS_LM_RC_BRFINPROGRESS);
                                                errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
                                                if( !setErrorText(ACS_LM_RC_BRFINPROGRESS, myErrStr) )
                                                {
                                                        ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
                                                }
					}
					break;
				}
			case LM_EMERGENCY_UNLOCK_ACTION:
				{
				if(!brfStatus)
				{
					//isLmActionInProgress = true;
					DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","LM_EMERGENCY_UNLOCK_ACTION Invoked");
					ACS_LM_AppExitCode myResult = theClienthandlerPtr->handleLkEmStartViaIMM();
					if ( myResult != ACS_LM_RC_OK)
					{
						resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
						DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","theClienthandlerPtr->handleLkEmStartViaIMM failed");
						std::string myErrStr = ACS_LM_Common::getApplicationErrorText(myResult);
                                                errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
						if( !setErrorText(myResult, myErrStr) )
                                                {
                                                        ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
                                                }
					}
					else
					{
						resultOfOperation = SA_AIS_OK;
					}
				}
				else
				{
					 resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
                                         DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","Backup and Restore Operation in Progress");
                                         std::string myErrStr = ACS_LM_Common::getApplicationErrorText(ACS_LM_RC_BRFINPROGRESS);
                                         errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
                                         if( !setErrorText(ACS_LM_RC_BRFINPROGRESS, myErrStr) )
                                         {
                                         	ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
                                         }

				}
					break;
				}

			/* When 'exportLicenseKeyFile' action invoked, the current LKF from the primary path will be copied to "/license_file" folder. */
			/* It will export the LKF from secondary path, if the "lservrc" file from primary path is tampered. */
			/* Throws error if "lervrc" is tampered in both paths */
			case LM_EXPORT_LKF_ACTION:
			{
				DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","exportLicenseKeyFile action invoked");
				std::string myIPv4Fingerprint(""),myIPv6Fingerprint("");
				std::string myFingerprint("");
				//call the generateFingerPrint to get the finger print value
				/* HW-Decoupling: Ignoring "generateFingerPrint" method defined on "ACS_LM_Common" class.*/
				bool myFingerPrintFlag= ACS_LM_Common_Util::generateFingerPrintForLicenseM(myIPv4Fingerprint,myIPv6Fingerprint,theOmHandlerPtr);
				if(!myFingerPrintFlag)
				{
					resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
					ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s","theClienthandlerPtr->handleLkExportLKFViaIMM failed - Unable to fetch fingerprint!");
					ACS_LM_AppExitCode myResult = ACS_LM_RC_NOK;
					std::string myErrStr = ACS_LM_Common::getApplicationErrorText(myResult);
					errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
					if( !setErrorText(myResult, myErrStr) )
					{
						ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
					}
				}

				if(!myIPv4Fingerprint.empty() && !myIPv6Fingerprint.empty())
					myFingerprint = myIPv4Fingerprint + "," + myIPv6Fingerprint;
				else if(!myIPv4Fingerprint.empty())
					myFingerprint = myIPv4Fingerprint;
				else if(!myIPv6Fingerprint.empty())
					myFingerprint = myIPv6Fingerprint;

				DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() myFingerPrint valuE  =  %s",myFingerprint.c_str());
				ACS_LM_AppExitCode myResult = theClienthandlerPtr->handleLkExportLKFViaIMM(myFingerprint);
				if ( myResult != ACS_LM_RC_OK)
				{
					resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
					DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","theClienthandlerPtr->handleLkExportLKFViaIMM failed");
					std::string myErrStr = getExportErrorText(myResult);
					errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
					if( !setErrorText(myResult, myErrStr) )
					{
						ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
					}
				}
				else
				{
					resultOfOperation = SA_AIS_OK;
				}
				break;
			}
			default:
				{
					DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","Operation ID is received by acs_lm_objectImplementer::saImmOiAdminOperationCallback is not valid");
					resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
					//std::string myErrStr = ACS_LM_Common::getApplicationErrorText(ACS_LM_RC_OTHERERROR);
					std::string myErrStr = "Unable to connect to server";
					errorMsgToShow.append(myErrStr); //Change for LM-Error Handling
					if( !setErrorText(ACS_LM_RC_SERVERNOTRESPONDING, myErrStr) )
					{
						ERROR("acs_lm_root_objectImplementer::adminOperationCallback() %s ", "Error in setting error text");
					}
					break;
				}
		};
	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	ACS_APGCC_AdminOperationParamType elem;
	char attName[]= "errorCode";
	elem.attrName = attName;
	elem.attrType = ATTR_STRINGT;
	//elem.attrType = ATTR_INT32T;
	//int intValue = getExitCode();
	//elem.attrValues=reinterpret_cast<void*>(&intValue);
//	const char * tempErrorMsgToShow = errorMsgToShow.c_str();
        elem.attrValues = reinterpret_cast<void*>(const_cast<char *> (errorMsgToShow.c_str()));//Change for LM-Error Handling
        /*insert parameter into the vector*/
        if(resultOfOperation != SA_AIS_OK)
        {
	vectorOut.push_back(elem);
        }

	ACS_CC_ReturnType myRes =  adminOperationResult(oiHandle, invocation, resultOfOperation,vectorOut);

	DEBUG("myRes = %d", myRes);
	std::string ftpVolPath = "/license_file";
	if(myRes != ACS_CC_SUCCESS)
	{
		DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","Failure occurred in sending AdminOperation Result");
		//ACS_LM_Common::getLicenseKeyFilePath(ftpVolPath);
		//bool status = ACS_LM_Common::doFileDelete(ftpVolPath);
		//DEBUG("ftpVolPath= %s", ftpVolPath.c_str());
		//if(status == true)
		//{
		//DEBUG("%s","All file get deleted successfully")
		//}
		//else
		//{
		//DEBUG("%s","Deletion of all file get failed")
		//}
		return;
	}
	//isLmActionInProgress = false;
	ACS_LM_Common::getLicenseKeyFilePath(ftpVolPath);
	//ACS_LM_Common::doFileDelete(ftpVolPath);
	DEBUG("ftpVolPath= %s", ftpVolPath.c_str());
	DEBUG("acs_lm_root_objectImplementer::adminOperationCallback() %s","Leaving");
}//end of adminOperationCallback
/*=================================================================
ROUTINE: setClienthandler
=================================================================== */
void acs_lm_root_objectImplementer::setClienthandler(ACS_LM_ClientHandler* aClienthandler)
{
	INFO("acs_lm_root_objectImplementer::setClienthandler() %s","Entering");
	theClienthandlerPtr = aClienthandler;
	INFO("acs_lm_root_objectImplementer::setClienthandler() %s","Leaving");
}//end of setClienthandler
/*=================================================================
ROUTINE: setElectronicKeyhandler
=================================================================== */
void acs_lm_root_objectImplementer::setElectronicKeyhandler(ACS_LM_ElectronicKey_RuntimeOwner* electronicKeyLMRuntimeOwnerPtr)
{
	INFO("acs_lm_root_objectImplementer::setElectronicKeyhandler() %s","Entering");
	theElectronicKeyLMRuntimeOwnerPtr = electronicKeyLMRuntimeOwnerPtr;
	INFO("acs_lm_root_objectImplementer::setElectronicKeyhandler() %s","Leaving");
}//end of setElectronicKeyhandler
/*=================================================================
ROUTINE: setOmHandler
=================================================================== */
void acs_lm_root_objectImplementer::setOmHandler(OmHandler* omHandler)
{
	INFO("acs_lm_root_objectImplementer::setOmHandler() %s","Entering");
	theOmHandlerPtr = omHandler;
	INFO("acs_lm_root_objectImplementer::setOmHandler() %s","Leaving");
}//end of setOmHandler
/*=================================================================
ROUTINE: setErrorText
=================================================================== */
bool acs_lm_root_objectImplementer::setErrorText(unsigned int aErrorId, std::string ErrorText)
{
	INFO("%s","LM Reporting Following Error while serving LM Commands");
	DEBUG("Error ID = %d",aErrorId);
	DEBUG("ERROR TEXT = %s",ErrorText.c_str());
	setExitCode(aErrorId,ErrorText);
	//setThisExitCode(aErrorId,ErrorText); 
	return true;
}//end of setErrorText


/*===================================================================
ROUTINE: shutdown
=================================================================== */
void acs_lm_root_objectImplementer::shutdown()
{
	INFO("%s", "Entering acs_lm_root_objectImplementer::shutdown.");

	DEBUG("%s", "Calling end_reactor_event_loop.");

	if( theOiHandlerPtr != NULL)
	{
		//Remove the implementer.
		DEBUG("%s", "Calling removeClassImpl.");
		//theOiHandlerPtr->removeClassImpl(this,ACS_IMM_LM_ROOT_CLASS_NAME);
		for (int i=0; i < 10; ++i)
		{
			if( theOiHandlerPtr->removeClassImpl(this,ACS_IMM_LM_ROOT_CLASS_NAME) == ACS_CC_FAILURE )
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
	INFO("%s", "Leaving acs_lm_root_objectImplementer::shutdown.");
	theReactor->end_reactor_event_loop();
        if( OIFuncThreadId != 0 )
        {
                ACE_Thread_Manager::instance()->join(OIFuncThreadId);
        }
}//End of shutdown


/*===================================================================
ROUTINE: setupLMOIThread 
=================================================================== */
ACS_CC_ReturnType acs_lm_root_objectImplementer::setupLMOIThread(acs_lm_root_objectImplementer *almRootObjectImplementer)
{

	INFO( "%s", "Entering setupObjectImplementer()");
	const ACE_TCHAR* lpszThreadName1 = "LMRootObjImpThread";
	int mythread;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	INFO( "%s", "Spawning the ObjectImplementer Thread");
	mythread = ACE_Thread_Manager::instance()->spawn(&LMRootOIFunc,
			(void*)almRootObjectImplementer,
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
	INFO( "%s", "Leaving setupObjectImplementer()");
	return rc;
}


/*===================================================================
ROUTINE: LMRootOIFunc 
=================================================================== */
ACE_THR_FUNC_RETURN acs_lm_root_objectImplementer::LMRootOIFunc(void* aLMOIPtr)
{
	INFO( "%s", "Entering LMRootOIThread().");

	acs_lm_root_objectImplementer * myLMRootObjImplementer = 0;
	myLMRootObjImplementer= reinterpret_cast<acs_lm_root_objectImplementer*>(aLMOIPtr);

	if(myLMRootObjImplementer!= 0)
	{
		DEBUG( "%s", "Calling lmRootOI_svc()");
		if(myLMRootObjImplementer->lmRootOI_svc() == ACS_CC_FAILURE)
		{

			ERROR( "%s", "lmRootOI_svc() failed");
			ERROR( "%s", "Leaving LMRootOIThread()");
			return 0;
		}
	}
	else
	{
		ERROR( "%s", "myLMRootObjImplementer is NULL");
		ERROR( "%s", "Leaving LMRootOIThread()");
		return 0;
	}
	INFO( "%s", "Leaving LMRootOIThread()");
	return 0;
}//End of LMRootOIThread 


ACE_INT32 acs_lm_root_objectImplementer::lmRootOI_svc()
{
	INFO("%s","Entering acs_lm_root_objectImplementer::lmRootOI_svc");
	ACE_INT32 res = svc(this->theReactor);
	if ( res == -1 )
	{
		DEBUG("%s", "svc() failed" );
		return -1;
	}
	INFO("%s","Exiting acs_lm_root_objectImplementer::lmRootOI_svc");
	try
	{
		this->theReactor->run_reactor_event_loop();
	}
	catch(...)
	{
	}
	return 0;

}


/* LM Maintenance Mode */
//Modifying this method name from "setExitCode" to "setThisExitCode".
void acs_lm_root_objectImplementer::setThisExitCode(int error, std::string text)
{

	INFO("%s", "Entering acs_lm_root_objectImplementer::setExitCode");

	errorCode = error;

	errorString = text;

	INFO("%s", "Exiting acs_lm_root_objectImplementer::setExitCode");

}



int acs_lm_root_objectImplementer::getExitCode()

{

	INFO("%s", "Entering acs_lm_root_objectImplementer::getExitCode");

	INFO("%s", "Exiting acs_lm_root_objectImplementer::getExitCode");

	return errorCode;

}
/* LM Maintenance Mode */
/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void acs_lm_root_objectImplementer::setErrorCode(unsigned int code)
{
        setExitCode(code,errorText);  
}

std::string acs_lm_root_objectImplementer::getExportErrorText(const ACS_LM_AppExitCode appExitCode)
{
	DEBUG("ACS_LM_Common::getexportErrorText()  %s","Entering");
	std::string eText;
	switch(appExitCode)
	{
	case ACS_LM_RC_VIRGIN_EXPORTLKFNOTALLOWED:
		eText = "Not allowed in Virgin Mode";
		break;
	case ACS_LM_RC_GRACE_EXPORTLKFNOTALLOWED:
		eText = "Not allowed in Grace Mode";
		break;
	case ACS_LM_RC_SRC_NOFILE_EXPORTLKFNOTALLOWED:
		eText = "Internal Error: LKF not found";
		break;
	case ACS_LM_RC_DEST_NOTWRITE_EXPORTLKFNOTALLOWED:
		eText = "Quota limit exceeded: Unable to export LKF in the destination folder";
		break;
	case ACS_LM_RC_SERVICE_LOCKED:
		eText = "Unable to connect to server";
		break;
	default:
		char buf[11];
		ACE_OS::itoa(appExitCode, buf, 10 );
		eText = "Unknown fault code ";
		eText += buf;
	}
	DEBUG("ACS_LM_Common::getExportErrorText()  %s","Leaving");
	return eText;
}//end of getExportErrorText
