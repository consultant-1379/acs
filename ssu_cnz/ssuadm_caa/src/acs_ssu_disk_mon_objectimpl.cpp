
//******************************************************************************//
//  NAME
//     acs_ssu_disk_mon_objectimpl.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
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
//
//
//  AUTHOR
//     2011-10-21 by XRAMMAT
//
//  SEE ALSO
//     -
//
//******************************************************************************
#include <iostream>
#include <ace/Signal.h>
#include "acs_ssu_common.h"
#include "acs_ssu_disk_mon_objectimpl.h"

using namespace std;

//! Constructor
ACS_SSU_DiskMon_ObjectImpl::ACS_SSU_DiskMon_ObjectImpl(string szimpName )
:acs_apgcc_objectimplementereventhandler_V2( szimpName )
{
	DEBUG("%s"," Entering ACS_SSU_DiskMon_ObjectImpl::ACS_SSU_DiskMon_ObjectImpl");
	theOiHandlerPtr = new acs_apgcc_oihandler_V2();
	if( theOiHandlerPtr == 0 )
	{
		ERROR("%s", "Memory allocation failed for  acs_apgcc_oihandler_V2");
	}
	DEBUG("%s"," Exiting ACS_SSU_DiskMon_ObjectImpl::ACS_SSU_DiskMon_ObjectImpl");
}

//! Destructor
ACS_SSU_DiskMon_ObjectImpl::~ACS_SSU_DiskMon_ObjectImpl()
{
	DEBUG("%s"," Entering ACS_SSU_DiskMon_ObjectImpl::~ACS_SSU_DiskMon_ObjectImpl");
	if( theOiHandlerPtr != 0 )
	{
		theOiHandlerPtr->removeClassImpl(this,SSU_DISKMONITOR_INFO_CLASS_NAME);
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}
	DEBUG("%s"," Exiting ACS_SSU_DiskMon_ObjectImpl::~ACS_SSU_DiskMon_ObjectImpl");
}

ACE_INT32 ACS_SSU_DiskMon_ObjectImpl::svc( ACE_Reactor *poReactor )
{
	DEBUG("%s"," Entering ACS_SSU_DiskMon_ObjectImpl::svc Dispatch");

	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,SSU_DISKMONITOR_INFO_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		NOTICE( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s"," Exiting ACS_SSU_DiskMon_ObjectImpl::svc Dispatch");
		return -1;
	}
	else
	{
		NOTICE( "Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}

	dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);

	DEBUG("%s"," Exiting ACS_SSU_DiskMon_ObjectImpl::svc Dispatch");
	return 0;
}


ACS_CC_ReturnType ACS_SSU_DiskMon_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
    (void) oiHandle;
    (void) ccbId;
    (void) className;
    (void) parentName;
    (void) attr;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          ACS_SSU_DiskMon_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_SSU_DiskMon_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_DiskMon_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_SSU_DiskMon_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	ACE_INT32 smallA1alarm, smallA2alarm, smallA1cease, smallA2cease, largeA1alarm, largeA2alarm, largeA1cease, largeA2cease, interval, dumpFrequency, monitoringNode;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_DiskMon_ObjectImpl ObjectModifyCallback invocated           \n");
	DEBUG ("%s","-----------------------------------------------------\n");

	ACE_INT32 i=0;
	while( attrMods[i] )
	{

		switch ( attrMods[i]->modAttr.attrType )
		{
		case ATTR_UINT32T:
			if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_SMALLDISK_A1_ALARM) == 0)
			{
				smallA1alarm = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (smallA1alarm <= 0 ) || ( smallA1alarm > 100 ))
				{
					ERROR("%s", "Value for attribute smallDiskHighAlarmLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_SMALLDISK_A2_ALARM) == 0)
			{
				smallA2alarm = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (smallA2alarm <= 0 ) || ( smallA2alarm > 100 ))
				{
					ERROR("%s", "Value for attribute smallDiskLowAlarmLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_SMALLDISK_A1_CEASE) == 0)
			{
				smallA1cease = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (smallA1cease <= 0 ) || ( smallA1cease > 100 ))
				{
					ERROR("%s", "Value for attribute smallDiskHighCeaseLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_SMALLDISK_A2_CEASE) == 0)
			{
				smallA2cease = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (smallA2cease <= 0 ) || ( smallA2cease > 100 ))
				{
					ERROR("%s", "Value for attribute smallDiskLowCeaseLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_LARGEDISK_A1_ALARM) == 0)
			{
				largeA1alarm = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (largeA1alarm <= 0 ) || ( largeA1alarm > 100 ))
				{
					ERROR("%s", "Value for attribute largeDiskHighAlarmLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_LARGEDISK_A2_ALARM) == 0)
			{
				largeA2alarm = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (largeA2alarm <= 0 ) || ( largeA2alarm > 100 ))
				{
					ERROR("%s", "Value for attribute largeDiskLowAlarmLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_LARGEDISK_A1_CEASE) == 0)
			{
				largeA1cease = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (largeA1cease <= 0 ) || ( largeA1cease > 100 ))
				{
					ERROR("%s", "Value for attribute largeDiskHighCeaseLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_LARGEDISK_A2_CEASE) == 0)
			{
				largeA2cease = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (largeA2cease <= 0 ) || ( largeA2cease > 100 ))
				{
					ERROR("%s", "Value for attribute largeDiskLowCeaseLevelLimit should be in the range of ( 0 - 100 )");
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_DISK_POLL_INTERVAL) == 0)
			{
				interval = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (interval <= 45 ) || ( interval > 360 ))
				{
					ERROR("%s", "Value for attribute pollingInterval should be in the range of ( 45 - 360 )");
					return ACS_CC_FAILURE;
				}
			}
			else
			{
				ERROR("%s", "Cannot modify parameter values  ");
				return ACS_CC_FAILURE;
			}
			break;
		case ATTR_INT32T:
			if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_DISK_DUMP_FREQ) == 0)
			{
				dumpFrequency = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (dumpFrequency != 0 ) && ( dumpFrequency != 1 ))
				{
					ERROR("%s", "Value for attribute processDumpFrequency should be either 0 or 1");
					return ACS_CC_FAILURE;
				}
			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_LOGICAL_DISK_MONITORING_TYPE) == 0)
			{
				monitoringNode = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( (monitoringNode  <1 ) || ( monitoringNode >3 ))
				{
					ERROR("%s", "Value for attribute monitoringOnNode should be in the range of ( 1 - 3 )");
					return ACS_CC_FAILURE;
				}
			}
			else
			{
				ERROR("%s", "Cannot modify parameter values  ");
				return ACS_CC_FAILURE;
			}
			break;
		default :
			ERROR("%s", "Cannot modify parameter values  ");
			return ACS_CC_FAILURE;
			break;
		}
		i++;
	}
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_SSU_DiskMon_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_DiskMon_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_SUCCESS;

}

void ACS_SSU_DiskMon_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           ACS_SSU_DiskMon_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
}

void ACS_SSU_DiskMon_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           ACS_SSU_DiskMon_ObjectImpl CcbApplyCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
}

ACS_CC_ReturnType ACS_SSU_DiskMon_ObjectImpl::updateRuntime(const char *objName, const char *attrName)
{
	(void) objName;
	(void) attrName;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_DiskMon_ObjectImpl CcbUpdateRuntimeCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_FAILURE;
}

void ACS_SSU_DiskMon_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) operationId;
	(void) paramList;
	(void) p_objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           ACS_SSU_DiskMon_ObjectImpl adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

}


