/**
   @file acs_rtr_statistics_impl.cpp

   Class method implementation for RTR module.

   This module contains the implementation of class declared in
   the RTR Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012     XHARBAV     Initial Release
   N/A       19/12/2013   	QVINCON   	Re-factoring
*/

#include "acs_rtr_statistics_impl.h"
#include "acs_rtr_server.h"
#include "acs_rtr_systemconfig.h"
#include "acs_rtr_global.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

#include <acs_apgcc_omhandler.h>


namespace statisticsInfoClass{
    const char ImmImplementerName[] = "RTR_OI_StatisticsInfo";
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_Statistics_ObjectImpl);

/*===================================================================
   ROUTINE: ACS_RTR_Statistics_ObjectImpl
=================================================================== */
ACS_RTR_Statistics_ObjectImpl::ACS_RTR_Statistics_ObjectImpl(ACS_RTR_Server* serverObject)
: acs_apgcc_objectimplementereventhandler_V3(statisticsInfoClass::ImmImplementerName ),
  m_ImmClassName(rtr_imm::StatisticsInfoClassName),
  m_IsMultiCP(ACS_RTR_SystemConfig::instance()->isMultipleCPSystem()),
  m_poRtrServer(serverObject)
 {
 	ACS_RTR_TRACE_MESSAGE("In");

 	ACS_RTR_TRACE_MESSAGE("Out");
 }

/*===================================================================
   ROUTINE: ~ACS_RTR_Statistics_ObjectImpl
=================================================================== */
ACS_RTR_Statistics_ObjectImpl::~ACS_RTR_Statistics_ObjectImpl()
{
	ACS_RTR_TRACE_MESSAGE("In");
	m_poRtrServer = NULL;
	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_RTR_Statistics_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_RTR_TRACE_MESSAGE("In, create a Statistics object under DN:<%s>", parentName);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);
	UNUSED(className);
	UNUSED(attr);

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}
/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_RTR_Statistics_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_RTR_TRACE_MESSAGE("In, delete a Statistics object DN:<%s>", objName);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_RTR_Statistics_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_RTR_TRACE_MESSAGE("In, modify a Statistics object DN:<%s>", objName);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);
	UNUSED(attrMods);

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_RTR_Statistics_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
   ROUTINE: abort
=================================================================== */
void ACS_RTR_Statistics_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");

	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_RTR_Statistics_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");

	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
}


/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_RTR_Statistics_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
	ACS_RTR_TRACE_MESSAGE("In, Statistics object:<%s>", objName);
	// to avoid warning msg
	UNUSED(attrName);
	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	std::string messageStoreDN;
	std::string messageStoreName;
	std::string statisticsObjDN(objName);

	if(getMessageDN(statisticsObjDN, messageStoreDN)
			&& getMessageStoreName(messageStoreDN, messageStoreName ) )
	{
		ACS_RTR_TRACE_MESSAGE("Get Cp statistics info internally");
		StatisticsInfo msStatistics;
		bool getResult = m_poRtrServer->getMessageStoreStatistics(messageStoreName, &msStatistics);

		if(!getResult)
		{
			ACS_RTR_TRACE_MESSAGE("Failed to get Message Store Statistics info internally");
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: Failed to get Message Store Statistics info internally, set all to zero");
			// try to read the statistics file directly
			// TODO
			// getResult = getMessageStoreStatisticsFromFile(messageStoreName, &msStatistics);
			msStatistics.volumeRead = 0U;
			msStatistics.volumeReported = 0U;

		}

		ACS_RTR_TRACE_MESSAGE(" Update Run-time attributes");
		updateRunTimeAttributes(objName, msStatistics);
		result = ACS_CC_SUCCESS;

	}

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void ACS_RTR_Statistics_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	ACS_RTR_TRACE_MESSAGE("In");

	// to avoid warning msg
	UNUSED(paramList);
	UNUSED(p_objName);
	// No actions are defined in Block Job class
	adminOperationResult(oiHandle, invocation, actionResult::NOOPERATION);

	ACS_RTR_TRACE_MESSAGE("Out");
}


/*===================================================================
        ROUTINE: updateRunTimeAttributes
=================================================================== */
void ACS_RTR_Statistics_ObjectImpl::updateRunTimeAttributes(const char* objectDN, StatisticsInfo& statisticsInfo)
{
	ACS_RTR_TRACE_MESSAGE("In");

	// Set the volumeRead attribute
	ACS_CC_ImmParameter paramVolumeRead;
	paramVolumeRead.attrName = rtr_imm::VolumeReadAttribute;
	paramVolumeRead.attrType = ATTR_UINT64T;
	paramVolumeRead.attrValuesNum = 1;
	void* volumeRead[1] = { reinterpret_cast<void*>(&(statisticsInfo.volumeRead)) };
	paramVolumeRead.attrValues = volumeRead;

	modifyRuntimeObj(objectDN, &paramVolumeRead);

	// Set the volumeReported attribute
	ACS_CC_ImmParameter paramVolumeReported;
	paramVolumeReported.attrName = rtr_imm::VolumeReportedAttribute;
	paramVolumeReported.attrType = ATTR_UINT64T;
	paramVolumeReported.attrValuesNum = 1;
	void* volumeReported[1] = { reinterpret_cast<void*>(&(statisticsInfo.volumeReported)) };
	paramVolumeReported.attrValues = volumeReported;

	modifyRuntimeObj(objectDN, &paramVolumeReported);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
        ROUTINE: getMessageStoreName
=================================================================== */
bool ACS_RTR_Statistics_ObjectImpl::getMessageDN(const std::string& objectDN, std::string& messageStoreDN)
{
	ACS_RTR_TRACE_MESSAGE("In, Object DN<%s>", objectDN.c_str());
	bool result = false;

	size_t tagStartPos = objectDN.find(rtr_imm::MessageStoreId);

	// Check if the tag is present
	if( std::string::npos != tagStartPos )
	{
		// get the DN
		messageStoreDN = objectDN.substr(tagStartPos);
		result= true;
		ACS_RTR_TRACE_MESSAGE(" message store DN:<%s>", messageStoreDN.c_str());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}

/*===================================================================
        ROUTINE: getMessageStoreName
=================================================================== */
bool ACS_RTR_Statistics_ObjectImpl::getMessageStoreName(const std::string& messageStoreDN, std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, MS DN<%s>", messageStoreDN.c_str());
	bool result = false;

	OmHandler objManager;
	// Init OmHandler
	ACS_CC_ReturnType getResult = objManager.Init();

	if(ACS_CC_SUCCESS == getResult)
	{
		// to get the MS name attribute
		ACS_CC_ImmParameter messageStoreNameAttribute;
		messageStoreNameAttribute.attrName =  rtr_imm::MessageStoreNameAttribute;

		getResult = objManager.getAttribute(messageStoreDN.c_str(), &messageStoreNameAttribute );

		// check for mandatory attributes
		if( (ACS_CC_FAILURE != getResult) &&
						(0 != messageStoreNameAttribute.attrValuesNum) )
		{
			result = true;
			messageStoreName.assign(reinterpret_cast<char *>(messageStoreNameAttribute.attrValues[0]));

			ACS_RTR_TRACE_MESSAGE(" message store Name:<%s>", messageStoreName.c_str());
		}

		// Release all resources
		objManager.Finalize();
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );

	return result;
}
