/**
   @file acs_rtr_cpstatistics_impl.cpp

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
   N/A       16/12/2013   	QVINCON   	Re-factoring
                        INCLUDE DECLARATION SECTION
**/

#include "acs_rtr_cpstatistics_impl.h"
#include "acs_rtr_server.h"
#include "acs_rtr_systemconfig.h"
#include "acs_rtr_global.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

#include <acs_apgcc_omhandler.h>

#include <sstream>

namespace cpStatisticsClass{
    const char ImmImplementerName[] = "RTR_OI_CpStatisticsInfo";
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_CpStatistics_ObjectImpl);

/*===================================================================
   ROUTINE: ACS_RTR_CpStatistics_ObjectImpl
=================================================================== */
ACS_RTR_CpStatistics_ObjectImpl::ACS_RTR_CpStatistics_ObjectImpl(ACS_RTR_Server* serverObject)
:acs_apgcc_objectimplementereventhandler_V3(cpStatisticsClass::ImmImplementerName),
 m_ImmClassName(rtr_imm::CpStatisticsInfoClassName),
 m_IsMultiCP(ACS_RTR_SystemConfig::instance()->isMultipleCPSystem()),
 m_poRtrServer(serverObject)
{
	ACS_RTR_TRACE_MESSAGE("In");

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: ~ACS_RTR_CpStatistics_ObjectImpl
=================================================================== */
ACS_RTR_CpStatistics_ObjectImpl::~ACS_RTR_CpStatistics_ObjectImpl()
{
	ACS_RTR_TRACE_MESSAGE("In");
	m_poRtrServer = NULL;
	ACS_RTR_TRACE_MESSAGE("Out");
}


/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_RTR_CpStatistics_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_RTR_TRACE_MESSAGE("In, create a Cp Statistics object under DN:<%s>", parentName);

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
ACS_CC_ReturnType ACS_RTR_CpStatistics_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_RTR_TRACE_MESSAGE("In, delete a Cp Statistics object DN:<%s>", objName);

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
ACS_CC_ReturnType ACS_RTR_CpStatistics_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_RTR_TRACE_MESSAGE("In, modify a Cp Statistics object DN:<%s>", objName);

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
ACS_CC_ReturnType ACS_RTR_CpStatistics_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
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
void ACS_RTR_CpStatistics_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
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
void ACS_RTR_CpStatistics_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
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
ACS_CC_ReturnType ACS_RTR_CpStatistics_ObjectImpl::updateRuntime(const char* objName, const char **attrName)
{
	ACS_RTR_TRACE_MESSAGE("In, CpStatistics object:<%s>", objName);
	// to avoid warning msg
	UNUSED(attrName);
	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	std::string messageStoreDN;
	std::string messageStoreName;
	std::string cpStatisticsObjDN(objName);
	short cpId;

	if(getMessageDN(cpStatisticsObjDN, messageStoreDN)
			&& getMessageStoreName(messageStoreDN, messageStoreName )
			&& getCpId(cpStatisticsObjDN, cpId) )
	{
		ACS_RTR_TRACE_MESSAGE("Get Cp statistics info internally");
		CpStatisticsInfo cpStatistics;
		bool getResult = m_poRtrServer->getCpStatistics(messageStoreName, &cpStatistics, cpId);

		if(!getResult)
		{
			ACS_RTR_TRACE_MESSAGE(" failed to get Cp Statistics info internally");
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: Failed to get Cp Statistics info internally, set all to zero");
			// try to read the statistics file directly?
			// TODO
			// getResult = getCpStatisticsFromFile(messageStoreName, &cpStatistics, cpId);
			cpStatistics.recordsLost = 0U;
			cpStatistics.recordsRead = 0U;
			cpStatistics.recordsSkipped = 0U;
		}

		ACS_RTR_TRACE_MESSAGE(" Update Run-time attributes");
		updateRunTimeAttributes(objName, cpStatistics);
		result = ACS_CC_SUCCESS;

	}

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void ACS_RTR_CpStatistics_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType** paramList)
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
void ACS_RTR_CpStatistics_ObjectImpl::updateRunTimeAttributes(const char* objectDN, CpStatisticsInfo& cpStatisticsInfo)
{
	ACS_RTR_TRACE_MESSAGE("In");

	// Set the RecordsRead
	ACS_CC_ImmParameter paramRecordsRead;
	paramRecordsRead.attrName = rtr_imm::RecordsReadAttribute;
	paramRecordsRead.attrType = ATTR_UINT64T;
	paramRecordsRead.attrValuesNum = 1;
	void* recordsRead[1] = { reinterpret_cast<void*>(&(cpStatisticsInfo.recordsRead)) };
	paramRecordsRead.attrValues = recordsRead;

	modifyRuntimeObj(objectDN, &paramRecordsRead);

	// Set the RecordsLost
	ACS_CC_ImmParameter paramRecordsLost;
	paramRecordsLost.attrName = rtr_imm::RecordsLostAttribute;
	paramRecordsLost.attrType = ATTR_UINT64T;
	paramRecordsLost.attrValuesNum = 1;
	void* recordsLost[1] = { reinterpret_cast<void*>(&(cpStatisticsInfo.recordsLost)) };
	paramRecordsLost.attrValues = recordsLost;

	modifyRuntimeObj(objectDN, &paramRecordsLost);

	// Set the RecordsSkipped
	ACS_CC_ImmParameter paramRecordsSkipped;
	paramRecordsSkipped.attrName = rtr_imm::RecordsSkippedAttribute;
	paramRecordsSkipped.attrType = ATTR_UINT64T;
	paramRecordsSkipped.attrValuesNum = 1;
	void* recordsSkipped[1] = { reinterpret_cast<void*>(&(cpStatisticsInfo.recordsSkipped)) };
	paramRecordsSkipped.attrValues = recordsSkipped;

	modifyRuntimeObj(objectDN, &paramRecordsSkipped);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
        ROUTINE: getMessageStoreName
=================================================================== */
bool ACS_RTR_CpStatistics_ObjectImpl::getMessageDN(const std::string& objectDN, std::string& messageStoreDN)
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
bool ACS_RTR_CpStatistics_ObjectImpl::getMessageStoreName(const std::string& messageStoreDN, std::string& messageStoreName)
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

/*===================================================================
        ROUTINE: getCpId
=================================================================== */
bool ACS_RTR_CpStatistics_ObjectImpl::getCpId(const std::string& objectDN, short& cpId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	if(m_IsMultiCP)
	{
		// Get the Cp Id from DN of CpStatistics object
		// Split the field in RDN and Value
		size_t equalPos = objectDN.find_first_of(parseSymbol::equal);
		size_t commaPos = objectDN.find_first_of(parseSymbol::comma);

		// Check if some error happens
		if( (std::string::npos != equalPos) && (std::string::npos != commaPos))
		{
			std::istringstream( objectDN.substr(equalPos + 1, (commaPos - equalPos - 1)) )  >> cpId;
			ACS_RTR_TRACE_MESSAGE(" Cp ID:<%d>", cpId);
			result = true;
		}
	}
	else
	{
		cpId = DEFAULT_CPID;
		result = true;
	}
	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}
