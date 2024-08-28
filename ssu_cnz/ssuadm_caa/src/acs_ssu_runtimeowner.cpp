/*=================================================================== */
/**
   @file acs_ssu_runtimeowner.cpp

   This module contains the implementation for ObjectImplementer for SSU FolderQuota Objects.

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
#include "acs_ssu_runtimeowner.h"


ACS_SSU_RuntimeOwner::ACS_SSU_RuntimeOwner()
{
	theFolderQuotaMonObject = NULL;
}

ACS_SSU_RuntimeOwner::~ACS_SSU_RuntimeOwner()
{

}

ACS_CC_ReturnType ACS_SSU_RuntimeOwner::updateCallback(const char* objName, const char** ImplName)
{
	(void) objName;
	(void) ImplName;
	SSU_FOLDER_OBJ folderObj;

	theFolderQuotaMonObject->getFolderSettings(objName,folderObj);
	void* new_value[1] = {0};
	ACS_CC_ImmParameter parToModify;
	ACS_CC_ReturnType enResult;
	DEBUG("Runtime Object that is going to Modified is :%s", objName);

	// Current Size
	unsigned long currentSize = folderObj.CurrentConsumption;
	parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_CURR_FOLDERSIZE;
	parToModify.attrType = ATTR_UINT64T;
	parToModify.attrValuesNum = 1;
	new_value[0] = reinterpret_cast<void*>(&currentSize);
	parToModify.attrValues = new_value;
	enResult = this->modifyRuntimeObj(objName, &parToModify);
	DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_CURR_FOLDERSIZE),enResult);

	// Quota Limit
	long maxFolderSize = folderObj.QuotaLimit;
	parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_UPPER_LIMIT;
	parToModify.attrType = ATTR_INT64T;
	parToModify.attrValuesNum = 1;
	new_value[0] = reinterpret_cast<void*>(&maxFolderSize);
	parToModify.attrValues = new_value;
	enResult = this->modifyRuntimeObj(objName, &parToModify);
	DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_UPPER_LIMIT),enResult);

	unsigned int value = folderObj.A1AlarmLevel;


	// A1 Alarm Level
	parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A1_ALARM_LIMIT;
	parToModify.attrType = ATTR_UINT32T;
	parToModify.attrValuesNum = 1;
	new_value[0] = reinterpret_cast<void*>(&value);
	parToModify.attrValues = new_value;
	enResult = this->modifyRuntimeObj(objName, &parToModify);
	DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A1_ALARM_LIMIT),enResult);

	// A1 Cease Level
	value = folderObj.A1CeaseLevel;
	parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A1_CEASE_LIMIT;
	parToModify.attrType = ATTR_UINT32T;
	parToModify.attrValuesNum = 1;
	new_value[0] = reinterpret_cast<void*>(&value);
	parToModify.attrValues = new_value;
	enResult = this->modifyRuntimeObj(objName, &parToModify);
	DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A1_CEASE_LIMIT),enResult);

	// A2 Alarm Level
	value = folderObj.A2AlarmLevel;
	parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A2_ALARM_LIMIT;
	parToModify.attrType = ATTR_UINT32T;
	parToModify.attrValuesNum = 1;
	new_value[0] = reinterpret_cast<void*>(&value);
	parToModify.attrValues = new_value;
	enResult = this->modifyRuntimeObj(objName, &parToModify);
	DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A2_ALARM_LIMIT),enResult);

	// A2 Cease Level
	value = folderObj.A2CeaseLevel;
	parToModify.attrName = (char *) SSU_FUNCTIONQUOTA_INFO_A2_CEASE_LIMIT;
	parToModify.attrType = ATTR_UINT32T;
	parToModify.attrValuesNum = 1;
	new_value[0] = reinterpret_cast<void*>(&value);
	parToModify.attrValues = new_value;
	enResult = this->modifyRuntimeObj(objName, &parToModify);
	DEBUG("Status of modification of %s in runtime object is %d",ACE_TEXT(SSU_FUNCTIONQUOTA_INFO_A2_CEASE_LIMIT),enResult);


	return ACS_CC_SUCCESS;
}

void ACS_SSU_RuntimeOwner::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) p_objName;
	(void) operationId;
	(void) paramList;
}

void ACS_SSU_RuntimeOwner::setFolderQuotaMonPointer(ACS_SSU_FolderQuotaMon* aPtr)
{
	theFolderQuotaMonObject = aPtr;
}

ACS_SSU_RtHanThread::ACS_SSU_RtHanThread()
{
	implementer = 0;
	isStop = false;
}

ACS_SSU_RtHanThread::ACS_SSU_RtHanThread(ACS_SSU_RuntimeOwner *impl)
{
	implementer = impl;
	isStop = false;
}

ACS_SSU_RtHanThread::~ACS_SSU_RtHanThread()
{

}

void ACS_SSU_RtHanThread::setImpl(ACS_SSU_RuntimeOwner *impl)
{
	implementer = impl;
	isStop = false;
}

void ACS_SSU_RtHanThread::stop()
{
	isStop=true;
}

int ACS_SSU_RtHanThread::svc(void)
{
	int ret;
	struct pollfd fds[1];

	fds[0].fd = implementer->getSelObj();
	fds[0].events = POLLIN;

	while(!isStop)
	{
		ret = poll(fds, 1, 1);
		if (ret == 0)
		{
			//cout<<"timeout off"<<endl;

		}else if (ret == -1)
		{
			printf("poll error: \n");
		}
		else
		{	// ACS_APGCC_DISPATCH_ONE
			implementer->dispatch(ACS_APGCC_DISPATCH_ALL);
		}
	}
	return 0;
}
// RT END
