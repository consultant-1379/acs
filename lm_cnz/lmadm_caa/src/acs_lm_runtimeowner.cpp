//******************************************************************************
//
//  NAME
//     acs_lm_runtimeowner.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
  INCLUDE DECLARATION SECTION
  =================================================================== */
#include "acs_lm_runtimeowner.h"
#include <sys/eventfd.h>

/*=================================================================
ROUTINE: ACS_LM_RuntimeOwner constructor
=================================================================== */
ACS_LM_RuntimeOwner::ACS_LM_RuntimeOwner():theClienthandlerPtr(0),theOmHandlerPtr(0),theElectronicKeyLMRuntimeOwnerPtr(0)
{
	DEBUG("ACS_LM_RuntimeOwner::ACS_LM_RuntimeOwner() %s","Entering");

	DEBUG("ACS_LM_RuntimeOwner::ACS_LM_RuntimeOwner() %s","Leaving");
}//end of constructor
/*=================================================================
ROUTINE: ACS_LM_RuntimeOwner destructor
=================================================================== */
ACS_LM_RuntimeOwner::~ACS_LM_RuntimeOwner()
{
	DEBUG("ACS_LM_RuntimeOwner::~ACS_LM_RuntimeOwner() %s","Entering");
	DEBUG("ACS_LM_RuntimeOwner::~ACS_LM_RuntimeOwner() %s","Leaving");
}//end of destructor
/*=================================================================
ROUTINE: setInternalOMhandler
=================================================================== */

void ACS_LM_RuntimeOwner::setInternalOMhandler(OmHandler*  aOmHandlerPtr)
{
	DEBUG("ACS_LM_RuntimeOwner::setInternalOMhandler() %s","Entering");
	theOmHandlerPtr = aOmHandlerPtr;
	DEBUG("ACS_LM_RuntimeOwner::setInternalOMhandler() %s","Leaving");
}//end of setInternalOMhandler
/*=================================================================
ROUTINE: setClienthandler
=================================================================== */
void ACS_LM_RuntimeOwner::setClienthandler(ACS_LM_ClientHandler* aClienthandler)
{
	DEBUG("ACS_LM_RuntimeOwner::setInternalOMhandler() %s","Entering");
	theClienthandlerPtr = aClienthandler;
	DEBUG("ACS_LM_RuntimeOwner::setClienthandler() %s","Leaving");

}//end of setClienthandler
/*=================================================================
ROUTINE: initLMRunTimeOwner
=================================================================== */
bool ACS_LM_RuntimeOwner::initLMRunTimeOwner()
{

	DEBUG("ACS_LM_RuntimeOwner::initLMRunTimeOwner() %s","Entering");
	for (int i=0; i<3; i++)
	{
		if(init(ACS_LM_DATAIMPLEMENTER) == ACS_CC_SUCCESS)
		{
			DEBUG("ACS_LM_RuntimeOwner::initLMRunTimeOwner()  init(ACS_LM_DATAIMPLEMENTER) Success  ACS_LM_DATAIMPLEMENTER = %s",ACS_LM_DATAIMPLEMENTER);
			return true;
		}
		else
		{
			ERROR("ACS_LM_RuntimeOwner::initLMRunTimeOwner()  init(ACS_LM_DATAIMPLEMENTER) Failed...Retrying for ACS_LM_DATAIMPLEMENTER = %s",ACS_LM_DATAIMPLEMENTER);
			usleep(10000);
		}
	}
	ERROR("ACS_LM_RuntimeOwner::initLMRunTimeOwner() %s","Failed after Retry as well, Hence Leaving");
	return false;

}//end of initLMRunTimeOwner
/*=================================================================
ROUTINE: finalizeLMRunTimeOwner
=================================================================== */
bool ACS_LM_RuntimeOwner::finalizeLMRunTimeOwner()
{
	DEBUG("ACS_LM_RuntimeOwner::finalizeLMRunTimeOwner() %s","Entering");
	if(finalize() != ACS_CC_SUCCESS)
	{
		ERROR("ACS_LM_RuntimeOwner::finalizeLMRunTimeOwner()  %s","finalize() failed");
		return false;
	}
	DEBUG("ACS_LM_RuntimeOwner::finalizeLMRunTimeOwner() %s","Leaving");
	return true;
}//end of finalizeLMRunTimeOwner
/*=================================================================
ROUTINE: setElectronicKeyRunTimeOwnerPointer
=================================================================== */
void ACS_LM_RuntimeOwner::setElectronicKeyRunTimeOwnerPointer(ACS_LM_ElectronicKey_RuntimeOwner* aElectronicKeyLMRuntimeOwnerPtr)
{
	theElectronicKeyLMRuntimeOwnerPtr = aElectronicKeyLMRuntimeOwnerPtr;
}//end of setElectronicKeyRunTimeOwnerPointer
/*=================================================================
ROUTINE: getMonthString
=================================================================== */
std::string ACS_LM_RuntimeOwner::getMonthString(int aMonth)
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
ROUTINE: updateCallback
=================================================================== */
ACS_CC_ReturnType ACS_LM_RuntimeOwner::updateCallback(const char* objName, const char** attrName)
{
	(void)objName;
	(void)attrName;

	DEBUG("ACS_LM_RuntimeOwner::updateCallback() %s","Entering");

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
#if 0
	std::string myFingerPrint("");
	//call the generateFingerPrint to get the finger print value
	bool myFingerPrintFlag= ACS_LM_Common::generateFingerPrint(myFingerPrint,theOmHandlerPtr);
	if(!myFingerPrintFlag)
	{
		//cout<<"Failure in fetching the Finger Print "<<endl;
		return ACS_CC_FAILURE;
	}
	else
	{
		//cout<< "The Finger print Value is "<<myFingerPrint<<endl;
	}
	(void) attrName;
	ACS_CC_ImmParameter parToModify;

	char* myFingerPrintValue = const_cast<char*>(myFingerPrint.c_str());
	// Update fingerPrint
	parToModify.attrName = (ACE_TCHAR *)FINGERPRINT_ATTRIBUTE;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* attrValue[1] = {reinterpret_cast<void*>(myFingerPrintValue)};
	parToModify.attrValues = attrValue;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		INFO("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of fingerPrint is failed");
		result =  ACS_CC_FAILURE;
	}

	bool myGraceMode = false,myEmergencyMode = false,myTestMode = false,myVirginMode = false;
	std::string myVirginModeStatus("NO");
	std::string myGraceModeStatus("NO");
	std::string myEmergencyModeStatus("NO");
	std::string myTestModeStatus("NO");
	std::string myEndDateforEmergencyMode("");
	std::string myLMMode("");
	theClienthandlerPtr->getLMMode(myGraceMode,myEmergencyMode,myTestMode);
	if(myGraceMode)
	{
		myGraceModeStatus = "YES";
		myLMMode = "GRACE MODE";
	}
	if(myTestMode)
	{
		myTestModeStatus = "YES";
		myLMMode = "TEST MODE";
	}
	if(myEmergencyMode)
	{
		myLMMode = "EMERGENCY MODE";
		myEmergencyModeStatus = "YES";
		ACE_INT64  myValue  = theClienthandlerPtr->getEmergencyModeEndDate();
		time_t myEmEndDate = myValue;
		struct tm emEndTimeLocal = *localtime(&myEmEndDate);
		std::ostringstream myOstr;
		myOstr << emEndTimeLocal.tm_mday<<"-"<<getMonthString(emEndTimeLocal.tm_mon+1)<<"-"<<(emEndTimeLocal.tm_year+1900);
		myEndDateforEmergencyMode = myOstr.str();
	}
	if(theClienthandlerPtr->getVirginModeStatus() == true)
	{
		myLMMode = "VIRGIN MODE";
		myVirginModeStatus = "YES";
	}
	if(!myGraceMode && !myVirginMode && !myEmergencyMode && !myTestMode)
	{
		myLMMode = "NORMAL MODE";
	}
	int myEmergencyActivationCount = theClienthandlerPtr->getEmergencyCount();

	char* myEmEndDate = (char*) myEndDateforEmergencyMode.c_str();
	//STUB:END
	parToModify.attrName = (ACE_TCHAR *) EMERGENCYENDDATE_ATTRIBUTE;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* dateValue[1] = {reinterpret_cast<void*>(myEmEndDate)};
	parToModify.attrValues = dateValue;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		INFO("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of endDateforEmergencyMode is failed");
		result =  ACS_CC_FAILURE;
	}


	char* myLMode = (char*) myLMMode.c_str();
	//STUB:END
	parToModify.attrName = (ACE_TCHAR *)LMMODE_ATTRIBUTE ;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* myLMode1[1] = {reinterpret_cast<void*>(myLMode)};
	parToModify.attrValues = myLMode1;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		INFO("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of LMMode is failed");
		result =  ACS_CC_FAILURE;
	}



	// Update emergencyActivatationCount
	int* myEmCount = new int(myEmergencyActivationCount);
	parToModify.attrName = (ACE_TCHAR *) EMERGENCYACTIVATIONCOUNT_ATTRIBUTE;
	parToModify.attrType = ATTR_INT32T;
	parToModify.attrValuesNum = 1;
	void* emValue[1] = {reinterpret_cast<void*>(myEmCount)};
	parToModify.attrValues = emValue;

	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		INFO("ACS_LM_RuntimeOwner::updateCallback() %s","Modification of emergencyActivatationCount is failed");
		result =  ACS_CC_FAILURE;
	}
	return result;
#endif

	DEBUG("ACS_LM_RuntimeOwner::updateCallback() %s","Leaving");

	//Added this statement to remove the compilation warning.
	//Need to remove when business logic implemented in this method
	return result;
}//end of updateCallback

/*=================================================================
ROUTINE: adminOperationCallback
=================================================================== */
void ACS_LM_RuntimeOwner::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;

	DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() %s","Entering");

#if 0
	std::vector<std::string> myArgumentList;
	SaAisErrorT resultOfOperation = SA_AIS_OK;
	int i = 0;

	switch (operationId)
	{
		case LM_INSTALL_LSK_FILE_ACTION:
			{
				DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() %s","LM_INSTALL_LSK_FILE_ACTION Invoked by User");
				// To fetch paramList
				std::string myLKFFilePath;
				while( paramList[i] )
				{
					switch ( paramList[i]->attrType )
					{
						case ATTR_INT32T:
							{
								INFO("ACS_LM_RuntimeOwner::adminOperationCallback() - %s","Integer given as argument - Not expected");

								break;
							}
						case ATTR_STRINGT:
							{
								if( ACE_OS::strcmp(paramList[i]->attrName, LKF_FILEPATH_ATTRIBUTE) == 0)
								{
									DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() - %s","String given as argument");
									std::string myStr( reinterpret_cast<char *>(paramList[i]->attrValues));
									myLKFFilePath = myStr;
									INFO("ACS_LM_RuntimeOwner::adminOperationCallback() myLKFFilePath = %s",myLKFFilePath.c_str());
									i++;
								}

								break;
							}
						default:
							{
								DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() - %s","Argument is unexpected type");
								break;
							}
					}

				}
				if (i == 1)
				{
					DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() - %s","before handleLkInstViaIMM ");
					ACE_INT32 errCode = theClienthandlerPtr->handleLkInstViaIMM(myLKFFilePath);
					if ( errCode != ACS_LM_RC_OK)
					{
						DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() theClienthandlerPtr->handleLkInstViaIMM(myLKFFilePath = %s) Failed",myLKFFilePath.c_str() );
						resultOfOperation = SA_AIS_ERR_INVALID_PARAM;

						std::string myErrStr = ACS_LM_Common::getApplicationErrorText((ACS_LM_AppExitCode)errCode);
						if( setErrorText(errCode, myErrStr) )
						{
							DEBUG("%s ", "Error in setting error text");
						}	
					}
					else
					{
						resultOfOperation = SA_AIS_OK;
						bool myRes = theElectronicKeyLMRuntimeOwnerPtr->createRuntimeObjectElectronicKeyObjects();
						if(myRes == false)
						{
							DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() %s","theElectronicKeyLMRuntimeOwnerPtr->createRuntimeObjectElectronicKeyObjects() Failed");
							resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
						}
					}

				}
				else
				{
					DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() %s","INVALID paramList");
					resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
				}
				break;
			}
		case LM_EMERGENCY_UNLOCK_ACTION:
			{
				DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() %s","LM_EMERGENCY_UNLOCK_ACTION Invoked");
				bool myResult = theClienthandlerPtr->handleLkEmStartViaIMM();
				if ( myResult == false)
				{
					resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
					INFO("ACS_LM_RuntimeOwner::adminOperationCallback() %s","theClienthandlerPtr->handleLkEmStartViaIMM failed");
				}
				else
				{
					resultOfOperation = SA_AIS_OK;
				}
				break;
			}
		default:
			{
				INFO("ACS_LM_RuntimeOwner::adminOperationCallback() %s","Operation ID is received by acs_lm_objectImplementer::saImmOiAdminOperationCallback is not valid");
				resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
				break;
			}
	};
	ACS_CC_ReturnType myRes =  adminOperationResult(oiHandle, invocation, resultOfOperation);
	if(myRes != ACS_CC_SUCCESS)
	{
		INFO("ACS_LM_RuntimeOwner::adminOperationCallback() %s","Failure occurred in sending AdminOperation Result");
		return;
	}
#endif
	DEBUG("ACS_LM_RuntimeOwner::adminOperationCallback() %s","Leaving");
}//end of adminOperationCallback
/*=================================================================
ROUTINE: setErrorText
=================================================================== */
bool ACS_LM_RuntimeOwner::setErrorText(unsigned int aErrorId, std::string ErrorText)
{
	(void)aErrorId;
	(void)ErrorText;
#if 0
	DEBUG("%s","LM Reporting Following Error while serving LM Commands");
	DEBUG("Error ID = %d",aErrorId);
	DEBUG("ERROR TEXT = %s",ErrorText.c_str());
	setExitCode(aErrorId,ErrorText);
#endif
	return true;
}//end of setErrorText
/*=================================================================
ROUTINE: ACS_LM_RuntimeHandlerThread
=================================================================== */
ACS_LM_RuntimeHandlerThread::ACS_LM_RuntimeHandlerThread():implementer(0)
{
	isStop = false;
	unsigned int initval = 0U;
	m_StopEvent = eventfd(initval,0);
}//end of ACS_LM_RuntimeHandlerThread
/*=================================================================
ROUTINE: ACS_LM_RuntimeHandlerThread
=================================================================== */
ACS_LM_RuntimeHandlerThread::ACS_LM_RuntimeHandlerThread(ACS_LM_RuntimeOwner *pImpl)
{
	implementer = pImpl;
	isStop = false;
	unsigned int initval = 0U;
	m_StopEvent = eventfd(initval,0);
}//end of ACS_LM_RuntimeHandlerThread
/*=================================================================
ROUTINE: ACS_LM_RuntimeHandlerThread destructor
=================================================================== */
ACS_LM_RuntimeHandlerThread::~ACS_LM_RuntimeHandlerThread()
{
	// Close the stop event, usually inside the object destructor
	ACE_OS::close(m_StopEvent);
}//end of destructor
/*=================================================================
ROUTINE: setImpl
=================================================================== */
void ACS_LM_RuntimeHandlerThread::setImpl(ACS_LM_RuntimeOwner *pImpl)
{
	implementer = pImpl;

	isStop = false;
}//end of setImpl
/*=================================================================
ROUTINE: stop
=================================================================== */
bool ACS_LM_RuntimeHandlerThread::stop()
{
	DEBUG("%s","ACS_LM_RuntimeHandlerThread::stop() Entered")
		// Signal the Stop event
		ACE_UINT64 stopEvent=1;

	// Signal to server to stop
	ssize_t numByte = ACE_OS::write(m_StopEvent, &stopEvent, sizeof(ACE_UINT64));

	if(sizeof(ACE_UINT64) != numByte)
	{
		ERROR("%s","Error occurred while signaling stop event ");
		ERROR("%s","Leaving ACS_LM_RuntimeHandlerThread::stop...");
		return false;
	}
	// isStop=true;
	DEBUG("%s","Leaving ACS_LM_RuntimeHandlerThread::stop() ");
	return true;
}//end of stop

/*=================================================================
ROUTINE: svc
=================================================================== */
int ACS_LM_RuntimeHandlerThread::svc(void)
{
	DEBUG("%s", "ACS_LM_RuntimeHandlerThread::svc() Entered");
	// Create a fd to wait for request
	const nfds_t nfds = 2;
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	fds[0].fd = m_StopEvent;
	fds[0].events = POLLIN;

	fds[1].fd = implementer->getSelObj();
	fds[1].events = POLLIN;

	ACE_INT32 poolResult;
	ACS_CC_ReturnType result;

	// waiting for IMM requests or stop
	while(true)
	{
		poolResult = ACE_OS::poll(fds, nfds);

		if( 0 == poolResult )
		{
			if(errno == EINTR)
			{
				continue;
			}
			INFO("ACS_LM_RuntimeHandlerThread::svc(), exit after error=%s", strerror(errno) );
			DEBUG("%s", "ACS_LM_RuntimeHandlerThread::svc() Leaving svc(), exit after poll error");
			break;
		}

		if(fds[0].revents & POLLIN)
		{
			// Received a stop request from server
			DEBUG("%s", "ACS_LM_RuntimeHandlerThread::svc() Leaving svc(), received a stop request from server");
			break;
		}

		if(fds[1].revents & POLLIN)
		{
			// Received a IMM request on a CpVolume
			result = implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			if(ACS_CC_SUCCESS != result)
			{
				DEBUG("%s", "ACS_LM_RuntimeHandlerThread::svc() Leaving svc(), error on BrfImplementer dispatch event");
			}
			continue;
		}
	}
	return 0;
}//end of svc
