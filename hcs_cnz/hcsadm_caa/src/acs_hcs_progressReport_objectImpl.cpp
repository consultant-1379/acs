/*=================================================================== */
/**
   @file acs_hcs_inUse_ObjectImpl.cpp

   Class method implementation for HC module.

   This module contains the implementation of class declared in
   the HC Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012     XHARBAV       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string>
#include <ace/Signal.h>
#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_healthcheckservice.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_Util.H"

using namespace std;

std::vector<acs_hcs_progressReport_ObjectImpl::progress> acs_hcs_progressReport_ObjectImpl::progressReport;
acs_hcs_progressReport_ObjectImpl::state acs_hcs_progressReport_ObjectImpl::stateEnum;
acs_hcs_progressReport_ObjectImpl::result acs_hcs_progressReport_ObjectImpl::resultEnum;

/*===================================================================
   ROUTINE: acs_hcs_inUse_ObjectImpl
=================================================================== */
acs_hcs_progressReport_ObjectImpl::acs_hcs_progressReport_ObjectImpl()
{
	DEBUG("%s","In constructor of progressReport_ObjectImpl ");
}

/*===================================================================
   ROUTINE: ~acs_hcs_inUse_ObjectImpl
=================================================================== */
acs_hcs_progressReport_ObjectImpl::~acs_hcs_progressReport_ObjectImpl()
{
	DEBUG("%s","In destructor of progressReport_ObjectImpl ");
}

ACS_CC_ReturnType acs_hcs_progressReport_ObjectImpl::updateCallback(const char *objName, const char **attrName)
{
	//DEBUG("%s","Entering acs_hcs_progressReport_ObjectImpl::updateCallback ");
	(void) objName;
	(void) attrName;

	//int index, i;
	unsigned int index, i;
	string object = objName;
	string parentObject = object.substr(5, object.size()-1);

	for(i = 0; i < progressReport.size(); i++)
	{
		if(parentObject == progressReport[i].DN)
		{
			break;	
		}
	}

	if(i == progressReport.size())
		return ACS_CC_FAILURE;
	else
		index = i;
	
	ACS_CC_ReturnType returnCode;
	ACS_CC_ImmParameter rdnAttr;
	ACS_CC_ImmParameter actionNameAttr;
	ACS_CC_ImmParameter additionalInfoAttr;
	ACS_CC_ImmParameter progressInfoAttr;
	ACS_CC_ImmParameter progressPercentageAttr;
	ACS_CC_ImmParameter resultAttr;
	ACS_CC_ImmParameter resultInfoAttr;
	ACS_CC_ImmParameter stateAttr;
	ACS_CC_ImmParameter actionIdAttr;
	ACS_CC_ImmParameter timeActionStartedAttr;
	ACS_CC_ImmParameter timeActionCompletedAttr;
	ACS_CC_ImmParameter timeOfLastStatusUpdateAttr;
	
	
	string atName = &(**attrName);
	cout<<"atName is "<<atName<<endl;

	if(atName == actionName_STR)
	{	

		actionNameAttr.attrName = (char*)actionName_STR;
		actionNameAttr.attrType = ATTR_STRINGT;
		actionNameAttr.attrValuesNum = 0;
		actionNameAttr.attrValues = 0;

		returnCode = this->modifyRuntimeObj(objName, &actionNameAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << this->getInternalLastErrorText() << endl;
		}
	}

	else if(atName == additionalInfo_STR)
	{
		additionalInfoAttr.attrName = (char*)additionalInfo_STR;
		additionalInfoAttr.attrType = ATTR_STRINGT;
		additionalInfoAttr.attrValuesNum = 1;
		char* addInfo = const_cast<char*>(progressReport[index].additionalInfo.c_str());
		void* val[1]={ reinterpret_cast<void*>(addInfo)};
		additionalInfoAttr.attrValues = val;

		returnCode = this->modifyRuntimeObj(objName, &additionalInfoAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}

	}

	else if(atName == progressInfo_STR)
	{
		progressInfoAttr.attrName = (char*)progressInfo_STR;
		progressInfoAttr.attrType = ATTR_STRINGT;
		progressInfoAttr.attrValuesNum = 0;
		progressInfoAttr.attrValues = 0;

		returnCode = this->modifyRuntimeObj(objName, &progressInfoAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}

	else if(atName == progressPercentage_STR)
	{
		progressPercentageAttr.attrName = (char*)progressPercentage_STR;
		progressPercentageAttr.attrType = ATTR_UINT32T;
		progressPercentageAttr.attrValuesNum = 0;
		progressPercentageAttr.attrValues = 0;

		returnCode = this->modifyRuntimeObj(objName, &progressPercentageAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}

	if(atName == result_STR)
	{
		resultAttr.attrName = (char*)result_STR;
		resultAttr.attrType = ATTR_INT32T;
		resultAttr.attrValuesNum = 1;
		int result = progressReport[index].result;
		void* value5[1] = {reinterpret_cast<void*>(&result)};
		resultAttr.attrValues = value5;

		returnCode = this->modifyRuntimeObj(objName, &resultAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}

	else if(atName == resultInfo_STR)
	{
		resultInfoAttr.attrName = (char*)resultInfo_STR;
		resultInfoAttr.attrType = ATTR_STRINGT;
		resultInfoAttr.attrValuesNum = 1;
		char* resultInfo = const_cast<char*>(progressReport[index].resultInfo.c_str());
		void* value6[1]={ reinterpret_cast<void*>(resultInfo)};
		resultInfoAttr.attrValues = value6;

		returnCode = this->modifyRuntimeObj(objName, &resultInfoAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}
	
	else if(atName == state_STR)
	{	
		stateAttr.attrName = (char*)state_STR;
		stateAttr.attrType = ATTR_INT32T;
		stateAttr.attrValuesNum = 1;
		int state = progressReport[index].state;
		void* value7[1] = {reinterpret_cast<void*>(&state)};
		stateAttr.attrValues = value7;

		returnCode = this->modifyRuntimeObj(objName, &stateAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}

	else if(atName == actionId_STR)
	{
		actionIdAttr.attrName = (char*)actionId_STR;
		actionIdAttr.attrType = ATTR_UINT32T;
		actionIdAttr.attrValuesNum = 0;
		actionIdAttr.attrValues = 0;

		returnCode = this->modifyRuntimeObj(objName, &actionIdAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}

	else if(atName == timeActionStarted_STR)
	{
		timeActionStartedAttr.attrName = (char*)timeActionStarted_STR;
		timeActionStartedAttr.attrType = ATTR_STRINGT;
		timeActionStartedAttr.attrValuesNum = 1;
		char* timeActionStarted = const_cast<char*>(progressReport[index].timeActionStarted.c_str());
		void* value9[1]={ reinterpret_cast<void*>(timeActionStarted)};
		timeActionStartedAttr.attrValues = value9;

		returnCode = this->modifyRuntimeObj(objName, &timeActionStartedAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}

	else if(atName == timeActionCompleted_STR)
	{
		timeActionCompletedAttr.attrName = (char*)timeActionCompleted_STR;
		timeActionCompletedAttr.attrType = ATTR_STRINGT;
		timeActionCompletedAttr.attrValuesNum = 1;
		char* timeActionCompleted = const_cast<char*>(progressReport[index].timeActionCompleted.c_str());
		void* value10[1]={ reinterpret_cast<void*>(timeActionCompleted)};
		timeActionCompletedAttr.attrValues = value10;

		returnCode = this->modifyRuntimeObj(objName, &timeActionCompletedAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}

	else if(atName == timeOfLastStatusUpdate_STR)
	{
		timeOfLastStatusUpdateAttr.attrName = (char*)timeOfLastStatusUpdate_STR; 
		timeOfLastStatusUpdateAttr.attrType = ATTR_STRINGT;
		timeOfLastStatusUpdateAttr.attrValuesNum = 1;
		char* timeOfLastStatusUpdate = const_cast<char*>(progressReport[index].timeOfLastStatusUpdate.c_str());
		void* value11[1]={ reinterpret_cast<void*>(timeOfLastStatusUpdate)};
		timeOfLastStatusUpdateAttr.attrValues = value11;

		returnCode = this->modifyRuntimeObj(objName, &timeOfLastStatusUpdateAttr);
		if(returnCode == ACS_CC_FAILURE)
		{
			cout << "modify failed " << getInternalLastErrorText() << endl;
		}
	}
	//DEBUG("%s","Leaving acs_hcs_progressReport_ObjectImpl::updateCallback ");
	return ACS_CC_SUCCESS;
} 
/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void acs_hcs_progressReport_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;
	DEBUG("%s","in acs_hcs_progressReport_ObjectImpl::adminOperationCallback ");
}

void acs_hcs_progressReport_ObjectImpl::shutdown()
{
	DEBUG("%s","Entering  acs_hcs_progressReport_ObjectImpl::shutdown() ");
	// Introduced progressDeleted variable for TR HS37486.. value of this variable is set to true when progress report receives shutdown signal
	AcsHcs::HealthCheckService::progressDeleted = true;
	for(unsigned int i = 0; i < acs_hcs_progressReport_ObjectImpl::progressReport.size(); i++){
		acs_hcs_progressReport_ObjectImpl::progressReport[i].handler->stop();
	}

	for(unsigned int i = 0; i < acs_hcs_progressReport_ObjectImpl::progressReport.size(); i++){
		acs_hcs_progressReport_ObjectImpl::progressReport[i].handler->wait();
	}

	DEBUG("%s","Leaving  acs_hcs_progressReport_ObjectImpl::shutdown() ");
}
