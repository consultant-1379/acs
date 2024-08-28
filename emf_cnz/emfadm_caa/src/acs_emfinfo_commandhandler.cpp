/*=================================================================== */
   /**
   @file acs_emfinfo_commandhandler.cpp

   Class method implementation for EMF module.

   This module contains the implementation of class declared in
   the EMF Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
#include<iostream>
#include <string>
#include <vector>
#include <ACS_CC_Types.h>
#include "acs_emf_dvdhandler.h"
#include "acs_emf_param.h"
#include "acs_emfinfo_commandhandler.h"
#include "acs_emf_common.h"
#include "acs_emf_tra.h"
#include "acs_emf_aeh.h"

using namespace std;

ACS_EMF_DVDHandler* acs_emfinfo_commandhandler::theDVDHandlePtr = NULL;
//std::string implName = "Apg_EmfInfo_Admin_Impl";
std::string implName = "APG_EMFINFO_ADMIN_APP";
static const SaImmOiCallbacksT_2 callbacks = {

		 acs_emfinfo_commandhandler::saImmOiAdminOperationCallback,
		 0,
		 0,
		 0,
		 0,
		 0,
		 0,
		 0
	};
SaImmOiHandleT handle;
/*===================================================================
   ROUTINE: acs_emfinfo_commandhandler
=================================================================== */
acs_emfinfo_commandhandler::acs_emfinfo_commandhandler(ACS_EMF_DVDHandler *pDVDHandler, ACE_HANDLE endEvent)
{
	DEBUG(1,"%s","Entering acs_emfinfo_commandhandler::acs_emfinfo_commandhandler");
	theDVDHandlePtr = pDVDHandler;
	stopThreadFds = endEvent;
	DEBUG(1,"%s","Exiting acs_emfinfo_commandhandler::acs_emfinfo_commandhandler");
}
/*===================================================================
   ROUTINE: ~acs_emfinfo_commandhandler
=================================================================== */
acs_emfinfo_commandhandler::~acs_emfinfo_commandhandler()
{
	// please do not delete theDVDHandlePtr; this is not our object. it is done in EMF server.
	DEBUG(1,"%s","Entering acs_emfinfo_commandhandler::~acs_emfinfo_commandhandler");
	DEBUG(1,"%s","Exiting acs_emfinfo_commandhandler::~acs_emfinfo_commandhandler");
}
/*===================================================================
   ROUTINE: remove_Implementor
=================================================================== */
SaAisErrorT acs_emfinfo_commandhandler::remove_Implementor()
{
	SaAisErrorT rc;
	cout << "Handle " << handle << endl;
	rc = saImmOiClassImplementerRelease(handle, (SaImmClassNameT) "EMFInfoAppl");
	if( rc != SA_AIS_OK)
	{
		cout << "saImmOiClassImplementerRelease  ERROR CODE " << rc << endl;
		return rc;
	}
	cout << "saImmOiClassImplementerRelease Done" << endl;
	rc = saImmOiImplementerClear(handle);
	if( rc != SA_AIS_OK)
	{
		cout << "saImmOiClassImplementerRelease  ERROR CODE " << rc << endl;
		return rc;
	}
	cout << "saImmOiImplementerClear Done" << endl;
	rc = saImmOiFinalize(handle);
	if (SA_AIS_OK != rc)
	{
		std::cout<<"saImmOiFinalize  ERROR CODE -  "<<rc<<std::endl;
		return rc;
	}
	cout << "saImmOmFinalize Done" << endl;
	return SA_AIS_OK;
}
/*===================================================================
   ROUTINE: register_object
=================================================================== */
SaAisErrorT acs_emfinfo_commandhandler::register_object(std::string  dn, std::string implName)
{
	DEBUG(1,"%s","Entering acs_emfinfo_commandhandler::register_object");
	SaAisErrorT rc;

	SaImmOiHandleT immOiHandle;
	SaAisErrorT errorCode;
	fd_set readfds;
	int retval = -1;

	static SaVersionT immVersion = { 'A', 2, 1 };

	rc = saImmOiInitialize_2(&immOiHandle, &callbacks, &immVersion);
	if( rc != SA_AIS_OK)
	{
		return rc;
	}

	DEBUG(1,"acs_emfinfo_commandhandler immOiHandle %lu\n",immOiHandle);
	handle = immOiHandle;

	SaImmOiImplementerNameT implementerName;

	implementerName = (char*)implName.c_str();

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object  implementer having the name which is
	 * specified in the implementerName parameter.  **/
	errorCode = saImmOiImplementerSet(immOiHandle, implementerName);

	if( errorCode != SA_AIS_OK)
	{
		return errorCode;
	}

	//rc = saImmOiClassImplementerSet( immOiHandle, (SaImmClassNameT) "EMFInfoAppl");
	SaNameT objectName;

	objectName.length = dn.length();
	memcpy(objectName.value, dn.c_str(), objectName.length);
	rc = saImmOiObjectImplementerSet( immOiHandle, &objectName , SA_IMM_ONE);
	if( rc != SA_AIS_OK)
	{
		return rc;
	}

	SaSelectionObjectT selObj;

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/

	errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj);

	if( errorCode != SA_AIS_OK )
	{
		return errorCode;
	}

	FD_ZERO(&readfds);
	FD_SET(selObj, &readfds);
	FD_SET(stopThreadFds, &readfds);

	ACE_INT32 s32Wait = 1;
	ACE_Time_Value oTimeValue(s32Wait);

	int maxfd = selObj;

	if( (unsigned) stopThreadFds > selObj )
	{
		maxfd = stopThreadFds ;
	}

	while(1)
	{
		retval = select(maxfd +1, &readfds, NULL,NULL,NULL);
		if(retval == -1)
			perror("select()");
		if(retval)
		{
			if( FD_ISSET(stopThreadFds, &readfds))
			{
				ERROR(1,"%s", "Stop event signaled by emfservice while waiting for emfinfo Admin callbacks !!!");
				return SA_AIS_ERR_TRY_AGAIN;
			}

			if(FD_ISSET(selObj, &readfds))
			{
				DEBUG(1,"%s","acs_emfinfo_commandhandler FD selected \n");
				//dispatch all the AMF pending function
				saImmOiDispatch(immOiHandle, SA_DISPATCH_ALL);
				FD_SET(stopThreadFds, &readfds);
			}
		}
	}

	DEBUG(1,"%s","Exiting acs_emfinfo_commandhandler::register_object");
	return SA_AIS_OK;
}
/*===================================================================
   ROUTINE: saImmOiAdminOperationCallback
=================================================================== */
void acs_emfinfo_commandhandler::saImmOiAdminOperationCallback(SaImmOiHandleT immOiHandle,
                                          SaInvocationT invocation,
                                          const SaNameT *objectName,
                                          SaImmAdminOperationIdT opId, const SaImmAdminOperationParamsT_2 **params)
{
	DEBUG(1,"%s","Entering acs_emfinfo_commandhandler::saImmOiAdminOperationCallback");
    (void) objectName;
	std::vector<std::string> myArgumentList;
	ACS_EMF_ParamList p;
	ACS_EMF_DATA srctData;
	acs_emf_operation_status_t state;

	DEBUG(1,"Operation ID received by acs_emfinfo_commandhandler::saImmOiAdminOperationCallback is  = %d",opId);
	SaAisErrorT resultOfOperation = SA_AIS_OK;
	ACE_TCHAR dev_info[10];

	switch (opId)
	{
	case EMF_CMD_TYPE_GETMEDIAINFO:
	{
		// FOR MEDIA INFO
		p.CmdCode = EMF_CMD_TYPE_GETMEDIAINFO;
		myArgumentList.clear();
		int i = 0;
		std::string hname; // for hostname

		while(params[i] !=0)
		{
			std::string myOtName((params[i]->paramName));  // name
			std::string myOtValue((*((SaStringT *)params[i]->paramBuffer)));  // value

			DEBUG(1,"parameter name = %s",myOtName.c_str());
			DEBUG(1,"parameter value = %s",myOtValue.c_str());

			if (ACE_OS::strcmp(myOtName.c_str(),"hostname") == 0)
			{
				hname = myOtValue;
			}
			myArgumentList.push_back(myOtValue);
			i++;
		}

		myArgumentList.clear();
		p.Data[0] = hname.c_str();
		srctData.Code = p.CmdCode;
		p.Encode();
		(void)p.getEncodedBuffer(srctData.Data);

		p.Clear();

		state = ACS_EMF_Common::getMediaOperationState();

		if (state == EMF_MEDIA_OPERATION_IS_IDLE)
		{
			// Marks DVD Operations to IS IN PROGRESS
			ACS_EMF_Common::setMediaOperationStateToProgress(EMF_GETMEDIA_INFO_IS_IN_PROGRESS);
			ACE_INT32 nPlatform = ACS_EMF_Common::GetHWVersion();

			if(nPlatform < 3)
			{
				DEBUG(1,"%s","This option is not valid for this platform!!!");
				DEBUG(1,"%s","ERROR:The option is not valid for this platform");
				DEBUG(1,"%s","EXIT CODE = 31");
				ACS_EMF_Common::setErrorText(0, 31, "The option is not valid for this platform");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else if (hname.empty()) // Check for arguments validity
			{
				DEBUG(1,"%s","Given arguments for EMFINFO are not valid");
				DEBUG(1,"%s","ERROR:Invalid Argument");
				DEBUG(1,"%s","EXIT CODE = 34");
				ACS_EMF_Common::setErrorText(0, EMF_RC_INVALIDARGUMENT, "Invalid Argument");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
     		}
			else if (ACS_EMF_Common::checkForDVDStatus(dev_info) != 0)
			{
				// Report error
				ACE_TCHAR szData[256];
				snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
					ACE_TEXT("EMF service unable to find/access dvd drive \n"));

				//! Send event
				ACS_EMF_AEH::ReportEvent(EMF_EV_EVENTNUM_ERROR,
						ACE_TEXT("EVENT"),
						EMF_EV_EVENTTEXT_FAULT,
						ACE_TEXT(""),
						szData,
						EMF_EV_EVENTTEXT_ERROR);

				if (ACS_EMF_Common::GetHWVariant() == 2)
				{
					DEBUG(1,"%s","ERROR:Assigning DVD drive is not supported on this platform");
					DEBUG(1,"%s","EXIT CODE = 29");
					ACS_EMF_Common::setErrorText(0, 29, "Assigning DVD drive is not supported on this platform");
				}
				else
				{
					DEBUG(1,"%s","ERROR:No external media board available");
					DEBUG(1,"%s","EXIT CODE = 21");
					ACS_EMF_Common::setErrorText(0, 21, "No external media board available");
				}

				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else if (ACE_OS::strcmp(hname.c_str(),ACS_EMF_Common::GetHostName()) != 0)
			{
				// Valid for GEP2 case
				if (ACS_EMF_Common::GetHWVariant() == 2)
				{
					DEBUG(1,"%s","ERROR:Assigning DVD drive is not supported on this platform");
					DEBUG(1,"%s","EXIT CODE = 29");
					ACS_EMF_Common::setErrorText(0, 29, "Assigning DVD drive is not supported on this platform");
				}
				else
				{
					DEBUG(1,"%s","ERROR:No external media board available");
					DEBUG(1,"%s","EXIT CODE = 21");
					ACS_EMF_Common::setErrorText(0, 21, "No external media board available");
				}

				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}

			if (resultOfOperation == SA_AIS_OK)
			{
				if(ACS_EMF_Common::checkForMediumPresence() == 2 ) //ACS_EMF_Common::setDeviceName() == -1) // Check for Media Presence
				{
					DEBUG(1,"%s","ERROR:No Media in DVD Drive");
					DEBUG(1,"%s","EXIT CODE = 22");
					ACS_EMF_Common::setErrorText(0, 22, "No Media in DVD Drive");
					resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				}
			}

			if (resultOfOperation == SA_AIS_OK)
			{
				int result = theDVDHandlePtr->DVDOperation(&srctData);

				if (result != EMF_RC_OK)
				{
					resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				}
			}
			// Marks DVD Operations to IS IN IDLE
			ACS_EMF_Common::setMediaOperationStateToIDLE();
		}
		else
		{
			if (state == EMF_GETMEDIA_INFO_IS_IN_PROGRESS)
			{
				DEBUG(1,"%s","DVD is BUSY and Get Media Info Operation is already in progress");
				DEBUG(1,"%s","ERROR:Operation is already in progress");
				DEBUG(1,"%s","EXIT CODE = 14");
				ACS_EMF_Common::setErrorText(0, 14, "Operation is already in progress");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else
			{
				DEBUG(1,"%s","ERROR:DVD drive is busy\n");
				DEBUG(1,"%s","EXIT CODE = 23");
				ACS_EMF_Common::setErrorText(0, 23, "DVD drive is busy");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
		}
		break;
	}
	case EMF_CMD_TYPE_GETDVDOWNER:
	{
        // FOR MEDIA OWNER INFO
		p.CmdCode = EMF_CMD_TYPE_GETDVDOWNER;

		myArgumentList.clear();
		int i = 0;
		std::string hname; // for hostname

		while(params[i] !=0)
		{
			std::string myOtName((params[i]->paramName));  // name
			std::string myOtValue((*((SaStringT *)params[i]->paramBuffer)));  // value
			DEBUG(1,"parameter name = %s\n",myOtName.c_str());
			DEBUG(1,"parameter value = %s\n",myOtValue.c_str());

			if (ACE_OS::strcmp(myOtName.c_str(),"hostname") == 0)
			{
				hname = myOtValue;
			}
			myArgumentList.push_back(myOtValue);
			i++;
		}

		myArgumentList.clear();
		p.Data[0] = hname.c_str();
		srctData.Code = p.CmdCode;
		p.Encode();
		(void)p.getEncodedBuffer(srctData.Data);

		p.Clear();

		state = ACS_EMF_Common::getMediaOperationState();

		if (state == EMF_MEDIA_OPERATION_IS_IDLE)
		{
			// Marks DVD Operations to IS IN PROGRESS
			ACS_EMF_Common::setMediaOperationStateToProgress(EMF_GETOWNER_INFO_IS_IN_PROGRESS);
			ACE_INT32 nPlatform = ACS_EMF_Common::GetHWVersion();

			if(nPlatform < 3)
			{
				DEBUG(1,"%s","ERROR:The option is not valid for this platform");
				DEBUG(1,"%s","EXIT CODE = 31");
				ACS_EMF_Common::setErrorText(0, 31, "The option is not valid for this platform");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else if (hname.empty()) // Check for arguments validity
			{
				DEBUG(1,"%s","Given arguments for EMFINFO are not valid");
				DEBUG(1,"%s","ERROR:Invalid Argument");
				DEBUG(1,"%s","EXIT CODE = 34");
				ACS_EMF_Common::setErrorText(0, EMF_RC_INVALIDARGUMENT, "Invalid Argument");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;

			}  // COMMENTED BELOW CODE IS USEFUL FOR GEP1
//			else if (ACS_EMF_Common::checkForDVDStatus(dev_info) != 0)
//			{
//				if (ACS_EMF_Common::GetHWVariant() == 2)
//				{
//					DEBUG(1,"%s","Assigning DVD drive is not supported on this platform\n");
//					ACS_EMF_Common::setErrorText(0, 29, "Assigning DVD drive is not supported on this platform");
//				}
//				else
//				{
//					DEBUG(1,"%s","No external media board available\n");
//					ACS_EMF_Common::setErrorText(0, 21, "No external media board available");
//				}
//
//				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
//			}
//			else if (ACE_OS::strcmp(hname.c_str(),ACS_EMF_Common::GetHostName()) != 0)
//			{
//				// Valid for GEP2 case
//				if (ACS_EMF_Common::GetHWVariant() == 2)
//				{
//					DEBUG(1,"%s","Assigning DVD drive is not supported on this platform\n");
//					ACS_EMF_Common::setErrorText(0, 29, "Assigning DVD drive is not supported on this platform");
//				}
//				else
//				{
//					DEBUG(1,"%s","No external media board available\n");
//					ACS_EMF_Common::setErrorText(0, 21, "No external media board available");
//				}
//
//				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
//			}

			if (resultOfOperation == SA_AIS_OK)
			{
				int result = theDVDHandlePtr->DVDOperation(&srctData);

				if (result != EMF_RC_OK)
					resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			// Marks DVD Operations to IS IN IDLE
			ACS_EMF_Common::setMediaOperationStateToIDLE();
		}
		else
		{
			if (state == EMF_GETOWNER_INFO_IS_IN_PROGRESS)
			{
				DEBUG(1,"%s","DVD is BUSY and Get Media Info Operation is already in progress");
				DEBUG(1,"%s","ERROR:Operation is already in progress");
				DEBUG(1,"%s","EXIT CODE = 14");
				ACS_EMF_Common::setErrorText(0, 14, "Operation is already in progress");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else
			{
				DEBUG(1,"%s","ERROR:DVD drive is busy");
				DEBUG(1,"%s","EXIT CODE = 23");
				ACS_EMF_Common::setErrorText(0, 23, "DVD drive is busy");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}

		}
		break;
	}
	default:
		DEBUG(1,"%s","Operation ID is received by acs_emfinfo_commandhandler::saImmOiAdminOperationCallback is not valid");
		resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
		//return;
		break;
	};

	SaAisErrorT rc = saImmOiAdminOperationResult(immOiHandle, invocation, resultOfOperation);

	if(rc != SA_AIS_OK)
	{
		DEBUG(1,"%s","Failure occurred in sending AdminOperation Result !!!");
		return;
	}

	DEBUG(1,"%s","Exiting acs_emfinfo_commandhandler::saImmOiAdminOperationCallback");
}

