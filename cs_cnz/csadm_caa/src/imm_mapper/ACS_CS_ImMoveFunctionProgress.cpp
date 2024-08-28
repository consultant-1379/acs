/*
 * * @file ACS_CS_ImMoveFunctionProgress.cpp
 *	@brief
 *	Class method implementation for ACS_CS_ImMoveFunctionProgress.
 *
 *  This module contains the implementation of class declared in
 *  the ACS_CS_ImMoveFunctionProgress.h module
 *
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2013-05-27
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2013-05-27 | qvincon      | File created.                       |
 *	+========+============+==============+=====================================+
 *
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "ACS_CS_ImMoveFunctionProgress.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_Trace.h"
#include "ACS_CC_Types.h"

#include "acs_apgcc_omhandler.h"

#include <vector>

ACS_CS_Trace_TDEF(ACS_CS_ImMoveFunctionProgress_TRACE);

namespace asyncActionProgress {
		const char className[] = "AxeFunctionDistributionAsyncActionProgress";

		char idAttribute[] = "id";
		char actionNameAttribute[] = "actionName";
		char additionalInfoAttribute[] = "additionalInfo";
		char progressInfoAttribute[] = "progressInfo";
		char progressPercentageAttribute[] = "progressPercentage";
		char resultAttribute[] = "result";
		char resultInfoAttribute[] = "resultInfo";
		char stateAttribute[] = "state";
		char actionIdAttribute[] = "actionId";
		char timeActionStartedAttribute[] = "timeActionStarted";
		char timeActionCompletedAttribute[] = "timeActionCompleted";
		char timeOfLastStatusUpdateAttribute[] = "timeOfLastStatusUpdate";

		char progressReportAttribute[] = "progressReport";

		const char idValue[] = "moveFunProgress";
		char actionNameValue[] = "moveFunction";
		unsigned int zeroPercentage = 0U;
		unsigned int actionIdValue = 1U;

	};

namespace {
		const char reportResultDN[] = "id=moveFunProgress,AxeFunctionDistributionfunctionDistributionMId=1";
		const int ELEMENT_ALREADY_EXIST = -14;
	};

ACS_CS_ImMoveFunctionProgress::ACS_CS_ImMoveFunctionProgress()
:  m_ObjManager(NULL),
   m_ObjManagerInitialized(false)
{


}

ACS_CS_ImMoveFunctionProgress::~ACS_CS_ImMoveFunctionProgress()
{
	if(NULL != m_ObjManager)
	{
		if(m_ObjManagerInitialized)
			m_ObjManager->Finalize();

		delete m_ObjManager;
	}
}

bool ACS_CS_ImMoveFunctionProgress::createProgressReportObject()
{
	OmHandler objManager;
	bool result = false;
	if( ACS_CC_SUCCESS == objManager.Init())
	{
		std::vector<ACS_CC_ValuesDefinitionType> objAttrList;

		const unsigned int bufferSize = 255;
		char tmpRDN[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(tmpRDN, bufferSize, "%s=%s", asyncActionProgress::idAttribute, asyncActionProgress::idValue);

		// Fill the RDN attribute fields
		ACS_CC_ValuesDefinitionType attributeRDN;
		attributeRDN.attrName = asyncActionProgress::idAttribute;
		attributeRDN.attrType = ATTR_STRINGT;
		attributeRDN.attrValuesNum = 1;
		void* tmpValueRDN[1] = { reinterpret_cast<void*>(tmpRDN) };
		attributeRDN.attrValues = tmpValueRDN;

		objAttrList.push_back(attributeRDN);

		// Fill the actionName attribute fields
		ACS_CC_ValuesDefinitionType attributeActionName;
		attributeActionName.attrName = asyncActionProgress::actionNameAttribute;
		attributeActionName.attrType = ATTR_STRINGT;
		attributeActionName.attrValuesNum = 1;
		void* tmpActionName[1] = { reinterpret_cast<void*>(asyncActionProgress::actionNameValue) };
		attributeActionName.attrValues = tmpActionName;

		objAttrList.push_back(attributeActionName);

		// Fill the result attribute fields
		ACS_CC_ValuesDefinitionType attributeResult;
		attributeResult.attrName = asyncActionProgress::resultAttribute;
		attributeResult.attrType = ATTR_INT32T;
		attributeResult.attrValuesNum = 1;
		int32_t initResult = NOT_AVAILABLE;
		void* tmpResult[1] = { reinterpret_cast<void*>(NOT_AVAILABLE) };
		attributeResult.attrValues = tmpResult;

		//objAttrList.push_back(attributeActionName);

		// Fill the version attribute fields
		ACS_CC_ValuesDefinitionType attributeActionId;
		attributeActionId.attrName = asyncActionProgress::actionIdAttribute;
		attributeActionId.attrType = ATTR_UINT32T;
		attributeActionId.attrValuesNum = 1;
		void* tmpActionId[1] = { reinterpret_cast<void*>(&asyncActionProgress::actionIdValue) };
		attributeActionId.attrValues = tmpActionId;

		objAttrList.push_back(attributeActionId);

		char tmpParantRDN[bufferSize] = {0};
		// Assemble the parent RDN value
		ACE_OS::snprintf(tmpParantRDN, bufferSize, "%s", ACS_CS_ImmMapper::RDN_FUNCTION_DIST.c_str());

		ACS_CC_ReturnType getResult = objManager.createObject(asyncActionProgress::className, tmpParantRDN, objAttrList);

		if( ACS_CC_FAILURE != getResult)
		{
			result = true;

			// Set the attribute progressReport of FunctionDistribution MOC
			ACS_CC_ImmParameter progressReportAttribute;

			progressReportAttribute.attrName = asyncActionProgress::progressReportAttribute;
			progressReportAttribute.attrType = ATTR_NAMET;
			progressReportAttribute.attrValuesNum = 1;

			char tmpValueDN[bufferSize] = {0};
			// Assemble the RDN value
			ACE_OS::snprintf(tmpValueDN, bufferSize, "%s,%s", tmpRDN, tmpParantRDN );

			void* tmpValue[1] = {reinterpret_cast<void*>(tmpValueDN)};
			progressReportAttribute.attrValues = tmpValue;

			getResult = objManager.modifyAttribute(tmpParantRDN, &progressReportAttribute);

		}
		else
		{
			int error = objManager.getInternalLastError();
			if(ELEMENT_ALREADY_EXIST == error)
			{
				result = true;
			}
		}

		objManager.Finalize();
	}

	return result;
}

void ACS_CS_ImMoveFunctionProgress::setProgressPercentage(uint32_t progressPercentage)
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		// Set the attribute
		setReportAttribute(asyncActionProgress::progressPercentageAttribute, progressPercentage);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::setResult(ActionResultType result)
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		int resultValue = static_cast<int>(result);
		setReportAttribute(asyncActionProgress::resultAttribute, resultValue);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::setResultInfo(const std::string& resultInfo)
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		setReportAttribute(asyncActionProgress::resultInfoAttribute, resultInfo);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::setState(ActionStateType state)
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		int stateValue = static_cast<int>(state);
		setReportAttribute(asyncActionProgress::stateAttribute, stateValue);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::setTimeActionStarted()
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		// Get current date and time into format: YYYY-MM-DDThh:mm:ss
		std::string currentDateTime;
		getDateAndTime(currentDateTime);
		// Set the attribute
		setReportAttribute(asyncActionProgress::timeActionStartedAttribute, currentDateTime);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::setTimeActionCompleted()
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		// Get current date and time into format: YYYY-MM-DDThh:mm:ss
		std::string currentDateTime;
		getDateAndTime(currentDateTime);

		// Set the attribute
		setReportAttribute(asyncActionProgress::timeActionCompletedAttribute, currentDateTime);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::resetTimeActionCompleted()
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		// Get current date and time into format: YYYY-MM-DDThh:mm:ss
		std::string currentDateTime;

		// Set the attribute
		setReportAttribute(asyncActionProgress::timeActionCompletedAttribute, currentDateTime);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}


void ACS_CS_ImMoveFunctionProgress::setTimeOfLastStatusUpdate()
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	if( initInternalData() )
	{
		// Get current date and time into format: YYYY-MM-DDThh:mm:ss
		std::string currentDateTime;
		getDateAndTime(currentDateTime);

		// Set the attribute
		setReportAttribute(asyncActionProgress::timeOfLastStatusUpdateAttribute, currentDateTime);
	}
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}


bool ACS_CS_ImMoveFunctionProgress::initInternalData()
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	if(!m_ObjManagerInitialized)
	{
		if(NULL == m_ObjManager)
		{
			m_ObjManager =new (std::nothrow) OmHandler();
		}

		if(NULL != m_ObjManager)
		{
			m_ObjManagerInitialized = ( m_ObjManager->Init() == ACS_CC_SUCCESS );
		}
	}

	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
	return m_ObjManagerInitialized;
}

void ACS_CS_ImMoveFunctionProgress::setReportAttribute(char* attribute, const std::string& value)
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	ACS_CC_ImmParameter progressReportAttribute;

	progressReportAttribute.attrName = attribute;
	progressReportAttribute.attrType = ATTR_STRINGT;
	progressReportAttribute.attrValuesNum = 1;

	const unsigned int bufferSize= 64;
	char tmpValue[bufferSize] = {0};
	// Assemble the RDN value
	ACE_OS::snprintf(tmpValue, bufferSize, "%s", value.c_str() );

	void* tmpValueArray[1] = {reinterpret_cast<void*>(tmpValue)};
	progressReportAttribute.attrValues = tmpValueArray;

	ACS_CC_ReturnType getResult = m_ObjManager->modifyAttribute(reportResultDN, &progressReportAttribute);

	if( ACS_CC_FAILURE != getResult)
	{
		ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "[%s@%d], progressReport updated <%s> to <%s>", __FUNCTION__, __LINE__, attribute, tmpValue ));
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "[%s@%d], progressReport updated <%s> failed, error:<%d>",  __FUNCTION__, __LINE__, attribute, m_ObjManager->getInternalLastError() ));
	}

	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));

}

void ACS_CS_ImMoveFunctionProgress::setReportAttribute(char* attribute, int value)
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	ACS_CC_ImmParameter progressReportAttribute;

	progressReportAttribute.attrName = attribute;
	progressReportAttribute.attrType = ATTR_INT32T;
	progressReportAttribute.attrValuesNum = 1;

	void* tmpValueArray[1] = {reinterpret_cast<void*>(&value)};
	progressReportAttribute.attrValues = tmpValueArray;

	ACS_CC_ReturnType getResult = m_ObjManager->modifyAttribute(reportResultDN, &progressReportAttribute);
	if( ACS_CC_FAILURE != getResult)
	{
		ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "[%s@%d], progressReport updated <%s> to <%d>", __FUNCTION__, __LINE__, attribute, value ));
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "[%s@%d], progressReport updated <%s> failed, error:<%d>",  __FUNCTION__, __LINE__, attribute, m_ObjManager->getInternalLastError() ));
	}

	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::setReportAttribute(char* attribute, uint32_t value)
{
	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	ACS_CC_ImmParameter progressReportAttribute;

	progressReportAttribute.attrName = attribute;
	progressReportAttribute.attrType = ATTR_UINT32T;
	progressReportAttribute.attrValuesNum = 1;

	void* tmpValueArray[1] = {reinterpret_cast<void*>(&value)};
	progressReportAttribute.attrValues = tmpValueArray;

	ACS_CC_ReturnType getResult = m_ObjManager->modifyAttribute(reportResultDN, &progressReportAttribute);

	if( ACS_CC_FAILURE != getResult)
	{
		ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "[%s@%d], progressReport updated <%s> to <%d>", __FUNCTION__, __LINE__, attribute, value ));
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "[%s@%d], progressReport updated <%s> failed, error:<%d>",  __FUNCTION__, __LINE__, attribute, m_ObjManager->getInternalLastError() ));
	}

	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}

void ACS_CS_ImMoveFunctionProgress::getDateAndTime(std::string& curDateTime)
{
    ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	curDateTime.clear();

	char tmpBuffer[30] = {0};
	struct tm *tmpTimeStruct;

	time_t currentTime = time(NULL);
	tmpTimeStruct = localtime(&currentTime);
	if( (NULL != tmpTimeStruct )
			&& ( strftime(tmpBuffer, sizeof(tmpBuffer), "%Y-%m-%dT%X", tmpTimeStruct) != 0 ))
	{
			curDateTime = tmpBuffer;
	}

	ACS_CS_TRACE((ACS_CS_ImMoveFunctionProgress_TRACE, "Leaving [%s@%d]", __FUNCTION__, __LINE__ ));
}
