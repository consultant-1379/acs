
//******************************************************************************//
//  NAME
//     acs_ssu_perf_mon_objectimpl.cpp
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
#include "acs_ssu_perf_mon_objectimpl.h"

using namespace std;


//! Constructor
ACS_SSU_PerfMon_ObjectImpl::ACS_SSU_PerfMon_ObjectImpl(string szimpName )
:acs_apgcc_objectimplementereventhandler_V2( szimpName )
{
	DEBUG("%s"," Entering ACS_SSU_PerfMon_ObjectImpl::ACS_SSU_PerfMon_ObjectImpl");
	theOiHandlerPtr = new acs_apgcc_oihandler_V2();
	if( theOiHandlerPtr == 0 )
	{
		ERROR("%s", "Memory allocation failed for  acs_apgcc_oihandler_V2");
	}
	DEBUG("%s"," Exiting ACS_SSU_PerfMon_ObjectImpl::ACS_SSU_PerfMon_ObjectImpl");
}

//! Destructor
ACS_SSU_PerfMon_ObjectImpl::~ACS_SSU_PerfMon_ObjectImpl()
{
	DEBUG("%s"," Entering ACS_SSU_PerfMon_ObjectImpl::~ACS_SSU_PerfMon_ObjectImpl");
	if( theOiHandlerPtr != 0 )
	{
		theOiHandlerPtr->removeClassImpl(this,SSU_PERFMONITOR_INFO_CLASS_NAME);
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}
	DEBUG("%s"," Exiting ACS_SSU_PerfMon_ObjectImpl::~ACS_SSU_Root_ObjectImpl");
}

ACE_INT32 ACS_SSU_PerfMon_ObjectImpl::svc( ACE_Reactor*& poReactor )
{
	DEBUG("%s"," Entering ACS_SSU_PerfMon_ObjectImpl::svc Dispatch");

	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,SSU_PERFMONITOR_INFO_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		NOTICE( "ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG("%s"," Exiting ACS_SSU_PerfMon_ObjectImpl::svc Dispatch");
		return -1;
	}
	else
	{
		NOTICE( "Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}

	dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);

	DEBUG("%s"," Exiting ACS_SSU_PerfMon_ObjectImpl::svc Dispatch");
	return 0;
}


ACS_CC_ReturnType ACS_SSU_PerfMon_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
    (void) oiHandle;
    (void) ccbId;
    (void) className;
    (void) parentName;
    (void) attr;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","          ACS_SSU_PerfMon_ObjectImpl  ObjectCreateCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_SSU_PerfMon_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_PerfMon_ObjectImpl ObjectDeleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_SSU_PerfMon_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	ACE_INT32 A1alarmLimit, A2alarmLimit, A1ceaseLimit, A2ceaseLimit, interval, A1dumpFrequency, A2dumpFrequency, monitoringNode;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_PerfMon_ObjectImpl ObjectModifyCallback invocated           \n");
	DEBUG ("%s","-----------------------------------------------------\n");

	std::string fileHandle_Dn(SSU_PERF_MON_FILEHANDLE_RDN);
	fileHandle_Dn.append(",");
	fileHandle_Dn.append(ACS_SSU_Common::dnOfSSURoot);
	ACE_INT32 i=0;
	while( attrMods[i] )
	{

		switch ( attrMods[i]->modAttr.attrType )
		{
		case ATTR_UINT32T:
			if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_A1_ALARM) == 0)
			{
				A1alarmLimit = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( ACE_OS::strcmp(objName, fileHandle_Dn.c_str()) == 0)
				{
					ERROR("%s", "Cannot modify value for attribute highAlarmLevelLimit in fileHandle object");
					return ACS_CC_FAILURE;
				}
				else
				{
					if( (A1alarmLimit <= 0 ) || ( A1alarmLimit > 100 ))
					{
						ERROR("%s", "Value for attribute highAlarmLevelLimit should be in the range of ( 0 - 100 )");
						return ACS_CC_FAILURE;
					}
				}
			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_A2_ALARM) == 0)
			{
				A2alarmLimit = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( ACE_OS::strcmp(objName, fileHandle_Dn.c_str()) == 0)
				{
					if( ( A2alarmLimit <= 100000 ) || ( A2alarmLimit >= 200000) )
					{
						ERROR("%s", "Value for attribute lowAlarmLevelLimit for fileHandle object should be in the range of ( 100000 - 200000 )");
						return ACS_CC_FAILURE;
					}
				}
				else
				{
					if( (A2alarmLimit <= 0 ) || ( A2alarmLimit > 100 ))
					{
						ERROR("%s", "Value for attribute lowAlarmLevelLimit should be in the range of ( 0 - 100 )");
						return ACS_CC_FAILURE;
					}
				}
			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_A1_CEASE) == 0)
			{
				A1ceaseLimit = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( ACE_OS::strcmp(objName, fileHandle_Dn.c_str()) == 0)
				{
					ERROR("%s", "Cannot modify value for attribute highCeaseLevelLimit in fileHandle object");
					return ACS_CC_FAILURE;
				}
				else
				{
					if( (A1ceaseLimit <= 0 ) || ( A1ceaseLimit > 100 ))
					{
						ERROR("%s", "Value for attribute highCeaseLevelLimit should be in the range of ( 0 - 100 )");
						return ACS_CC_FAILURE;
					}
				}
			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_A2_CEASE) == 0)
			{
				A2ceaseLimit = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( ACE_OS::strcmp(objName, fileHandle_Dn.c_str()) == 0)
				{
					if( ( A2ceaseLimit <= 100000 ) || ( A2ceaseLimit >= 200000) )
					{
						ERROR("%s", "Value for attribute lowCeaseLevelLimit for fileHandle object should be in the range of ( 100000 - 200000 )");
						return ACS_CC_FAILURE;
					}
				}
				else
				{
					if( (A2ceaseLimit <= 0 ) || ( A2ceaseLimit > 100 ))
					{
						ERROR("%s", "Value for attribute lowCeaseLevelLimit should be in the range of ( 0 - 100 )");
						return ACS_CC_FAILURE;
					}
				}
			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_POLL_INTERVAL) == 0)
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
			if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_A1_DUMP_FREQ) == 0)
			{
				A1dumpFrequency = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));

				if( ACE_OS::strcmp(objName, fileHandle_Dn.c_str()) == 0)
				{
					ERROR("%s", "Cannot modify value for attribute processDumpFrequencyForHigh in fileHandle object");
					return ACS_CC_FAILURE;
				}

				if( (A1dumpFrequency != 0 ) && ( A1dumpFrequency != 1 ))
				{
					ERROR("%s", "Value for attribute processDumpFrequencyForHigh should be either 0 or 1");
					return ACS_CC_FAILURE;
				}
			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_A2_DUMP_FREQ) == 0)
			{

				A2dumpFrequency = *((ACE_INT32 *)(attrMods[i]->modAttr.attrValues[0]));


				if( (A2dumpFrequency != 0 ) && ( A2dumpFrequency != 1 ))
				{
					ERROR("%s", "Value for attribute processDumpFrequencyForHigh should be either 0 or 1");
					return ACS_CC_FAILURE;
				}
			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, SSU_PERF_MON_MONITORING_TYPE) == 0)
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

ACS_CC_ReturnType ACS_SSU_PerfMon_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_PerfMon_ObjectImpl CcbCompleteCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_SUCCESS;

}

void ACS_SSU_PerfMon_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           ACS_SSU_PerfMon_ObjectImpl CcbAbortCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
}

void ACS_SSU_PerfMon_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           ACS_SSU_PerfMon_ObjectImpl CcbApplyCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");
}

ACS_CC_ReturnType ACS_SSU_PerfMon_ObjectImpl::updateRuntime(const char *objName, const char *attrName)
{
	(void) objName;
	(void) attrName;
	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","            ACS_SSU_PerfMon_ObjectImpl CcbUpdateRuntimeCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

	return ACS_CC_FAILURE;
}

void ACS_SSU_PerfMon_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) operationId;
	(void) paramList;
	(void) p_objName;

	DEBUG ("%s","\n---------------------------------------------------\n");
	DEBUG ("%s","           ACS_SSU_PerfMon_ObjectImpl adminOperationCallback invocated           \n");
	DEBUG ("%s","---------------------------------------------------\n");

}


