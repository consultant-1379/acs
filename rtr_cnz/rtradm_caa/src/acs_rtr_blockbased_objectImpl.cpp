/**
   @file acs_rtr_blockbased_ObjectImpl.cpp

   Class method implementation for RTR module.

   This module contains the implementation of class declared in
   the RTR Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012     XHARBAV       Initial Release
   N/A       13/12/2013   QVINCON   Re-factoring
                        INCLUDE DECLARATION SECTION
**/

#include "acs_rtr_blockbased_objectImpl.h"
#include "acs_rtr_server.h"
#include "acs_rtr_global.h"
#include "acs_rtr_tracer.h"


#include "acs_apgcc_omhandler.h"

#include "aes_ohi_blockhandler2.h"

namespace blockBasedJobClass{
    const char ImmImplementerName[] = "RTR_OI_BlockBaseJob";
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_BlockBased_ObjectImpl);

/*===================================================================
   ROUTINE: ACS_RTR_BlockBased_ObjectImpl
=================================================================== */
ACS_RTR_BlockBased_ObjectImpl::ACS_RTR_BlockBased_ObjectImpl(ACS_RTR_Server* serverObject)
:acs_apgcc_objectimplementereventhandler_V3( blockBasedJobClass::ImmImplementerName ),
 m_ImmClassName(rtr_imm::BlockBaseJobClassName),
 m_poRtrServer(serverObject)
{
	ACS_RTR_TRACE_MESSAGE("In");

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: ~ACS_RTR_BlockBased_ObjectImpl
=================================================================== */
ACS_RTR_BlockBased_ObjectImpl::~ACS_RTR_BlockBased_ObjectImpl()
{
	ACS_RTR_TRACE_MESSAGE("In");
	m_poRtrServer = NULL;
	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_RTR_BlockBased_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_RTR_TRACE_MESSAGE("In, create a FileJOB object MS DN:<%s>", parentName);

	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(className);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	uint32_t msgStoreRecordSize = 0U;
	BlockJobInfo newBlockJob;
	newBlockJob.action = Create;
	newBlockJob.blockHoldTime = 0U;
	newBlockJob.lengthType = 0;
	newBlockJob.length = 0;
	newBlockJob.paddingChar = 0;

	// extract the attributes
	for(size_t idx = 0; attr[idx] != NULL ; ++idx)
	{
		// check if RDN attribute
		if( 0 == ACE_OS::strcmp(rtr_imm::BlockBaseJobId, attr[idx]->attrName) )
		{
			newBlockJob.blockJobDN.assign(reinterpret_cast<char *>(attr[idx]->attrValues[0]));
			newBlockJob.blockJobDN += parseSymbol::comma;
			newBlockJob.blockJobDN.append(parentName);
			ACS_RTR_TRACE_MESSAGE("Block Job DN:<%s>", newBlockJob.blockJobDN.c_str());
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::TransferQueueAttribute, attr[idx]->attrName) )
		{
			newBlockJob.transferQueue.assign(reinterpret_cast<char *>(attr[idx]->attrValues[0]));

			ACS_RTR_TRACE_MESSAGE("transferQueue:<%s>", newBlockJob.transferQueue.c_str());

			if(!validateBlockTQ(newBlockJob.transferQueue))
			{
				result = ACS_CC_FAILURE;
				break;
			}
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::HoldTimeAttrribute, attr[idx]->attrName) )
		{
			newBlockJob.blockHoldTime = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" Block Hold Time:<%d>", newBlockJob.blockHoldTime);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::PaddingCharAttribute, attr[idx]->attrName) )
		{
			newBlockJob.paddingChar = *reinterpret_cast<int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" Block Padding char:<%d>", newBlockJob.paddingChar);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::BlockLengthAttribute, attr[idx]->attrName) )
		{
			newBlockJob.length = *reinterpret_cast<int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" Block length:<%d>", newBlockJob.length);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::BlockLengthTypeAttribute, attr[idx]->attrName) )
		{
			newBlockJob.lengthType = *reinterpret_cast<int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" Block length Type:<%d>", newBlockJob.lengthType);
			continue;
		}
	}

	if((ACS_CC_FAILURE != result) && getMessageStoreParameter(parentName, newBlockJob.messageStoreName, msgStoreRecordSize))
	{
		ACS_RTR_TRACE_MESSAGE("Validate parameters value");

		// Check for parameters error
		if( m_poRtrServer->isJobDefined(newBlockJob.messageStoreName,newBlockJob.transferQueue) )
		{
			setExitCode(MSDEFERROR, CmdPrintouts::TqDefError);
			result = ACS_CC_FAILURE;
		}
		else if( msgStoreRecordSize > newBlockJob.length )
		{
			setExitCode(MS_BUFFER_BLK, CmdPrintouts::MsBufBlock);
			result = ACS_CC_FAILURE;
		}

		// Check validation result
		if(ACS_CC_SUCCESS == result)
		{
			m_BlockJobOperationTable.insert(pair<ACS_APGCC_CcbId, BlockJobInfo>(ccbId, newBlockJob));
			ACS_RTR_TRACE_MESSAGE("Create a new Block Job");
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_RTR_BlockBased_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{ 
	ACS_RTR_TRACE_MESSAGE("In, delete the BlockJOB object<%s>", objName);
	// to avoid warning msg
	UNUSED(oiHandle);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	std::string blockJobDN(objName);
	size_t tagCommaPos = blockJobDN.find_first_of(parseSymbol::comma);

	if( std::string::npos != tagCommaPos)
	{
		std::string messageStoreName;
		std::string messageStoreDN(blockJobDN.substr(tagCommaPos+1));

		// get the related MS name
		if( getMessageStoreName(messageStoreDN.c_str(), messageStoreName) )
		{
			// remove the manager if possible
			if(!m_poRtrServer->removeManager(messageStoreName))
			{
				setExitCode(RMDENIED, CmdPrintouts::RmDenied);
				result = ACS_CC_FAILURE;
			}
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("Failed to get MS name DN:<%s>", messageStoreDN.c_str() );
			setExitCode(INTERNALERR, CmdPrintouts::IntProgFault);
			result = ACS_CC_FAILURE;
		}
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("Failed to get MS DN!");
		setExitCode(INTERNALERR, CmdPrintouts::IntProgFault);
		result = ACS_CC_FAILURE;
	}

	ACS_RTR_TRACE_MESSAGE("Out");

	return result;
}

/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_RTR_BlockBased_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_RTR_TRACE_MESSAGE("In, modify the FileJOB object<%s>", objName);
	// to avoid warning msg
	UNUSED(oiHandle);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	std::string blockJobDN(objName);
	size_t tagCommaPos = blockJobDN.find_first_of(parseSymbol::comma);

	if( std::string::npos != tagCommaPos)
	{
		// Block Job setting
		BlockJobInfo modBlockJob;
		modBlockJob.action = Modify;

		// Message store DN
		std::string messageStoreDN(blockJobDN.substr(tagCommaPos+1));

		bool getParametersResult = true;

		// Check if already present this transaction
		operationTable::iterator element = m_BlockJobOperationTable.find(ccbId);
		if(m_BlockJobOperationTable.end() != element)
		{
			// Update previous info
			ACS_RTR_TRACE_MESSAGE("Multi attribute change, update previous info");
			modBlockJob = element->second;
			m_BlockJobOperationTable.erase(element);
		}
		else
		{
			// new transaction modify get current parameters
			// get Message store parameters
			getParametersResult = (	getMessageStoreParameter(messageStoreDN.c_str(), modBlockJob.messageStoreName, modBlockJob.msgStoreRecordSize)
						&&  getCurrentParamters(modBlockJob) );
		}

		// check get Message store parameters result
		if(getParametersResult)
		{
			// extract the attributes
			for(size_t idx = 0; attrMods[idx] != NULL ; ++idx)
			{
				ACS_APGCC_AttrValues modAttribute = attrMods[idx]->modAttr;

				// Check for TQ parameter change
				if( 0 == ACE_OS::strcmp(rtr_imm::TransferQueueAttribute, modAttribute.attrName) )
				{
					// change not allowed
					setExitCode(INTERNALERR, CmdPrintouts::IntProgFault);
					result = ACS_CC_FAILURE;
					break;
				}

				// Check for HoldTime parameter change
				if( 0 == ACE_OS::strcmp(rtr_imm::HoldTimeAttrribute, modAttribute.attrName) )
				{
					modBlockJob.blockHoldTime = *reinterpret_cast<unsigned int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" Block Hold Time:<%d>", modBlockJob.blockHoldTime);
					continue;
				}

				// Check for Padding Char parameter change
				if( 0 == ACE_OS::strcmp(rtr_imm::PaddingCharAttribute, modAttribute.attrName) )
				{
					modBlockJob.paddingChar = *reinterpret_cast<int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" Block Padding char:<%d>", modBlockJob.paddingChar);
					continue;
				}

				// Check for Block length parameter change
				if( 0 == ACE_OS::strcmp(rtr_imm::BlockLengthAttribute, modAttribute.attrName) )
				{
					modBlockJob.length = *reinterpret_cast<int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" Block length:<%d>", modBlockJob.length);
					continue;
				}

				// Check for Block length Type parameter change
				if( 0 == ACE_OS::strcmp(rtr_imm::BlockLengthTypeAttribute, modAttribute.attrName) )
				{
					modBlockJob.lengthType = *reinterpret_cast<int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" Block length Type:<%d>", modBlockJob.lengthType);
					continue;
				}
			}

			// Check error status
			if( ACS_CC_FAILURE != result )
			{
				ACS_RTR_TRACE_MESSAGE("Validate parameters value");

				// check block length with MS record length
				if( modBlockJob.msgStoreRecordSize > modBlockJob.length )
				{
					setExitCode(MS_BUFFER_BLK, CmdPrintouts::MsBufBlock);
					result = ACS_CC_FAILURE;
				}
				else
				{
					// Parameters change is valid
					m_BlockJobOperationTable.insert(pair<ACS_APGCC_CcbId, BlockJobInfo>(ccbId, modBlockJob));
					ACS_RTR_TRACE_MESSAGE("Modify the Block Job");
				}
			}
		}
		else
		{
			result = ACS_CC_FAILURE;
			setExitCode(INTERNALERR, CmdPrintouts::IntProgFault);
		}
	}
	else
	{
		result = ACS_CC_FAILURE;
		setExitCode(INTERNALERR, CmdPrintouts::IntProgFault);
	}

	ACS_RTR_TRACE_MESSAGE("Out");

	return result;
}

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_RTR_BlockBased_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{ 
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_RTR_TRACE_MESSAGE("Out");
	return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: abort
=================================================================== */
void ACS_RTR_BlockBased_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);
	// find all operations related to the ccbid
	std::pair<operationTable::iterator, operationTable::iterator> operationRange;
	operationRange = m_BlockJobOperationTable.equal_range(ccbId);

	// Erase all elements from the table of the operations
	m_BlockJobOperationTable.erase(operationRange.first, operationRange.second);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_RTR_BlockBased_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	// find all operations related to the ccbid
	std::pair<operationTable::iterator, operationTable::iterator> operationRange;
	operationRange = m_BlockJobOperationTable.equal_range(ccbId);

	//for each operation found
	operationTable::iterator element;
	for(element = operationRange.first; (element != operationRange.second) && (result == ACS_CC_SUCCESS); ++element)
	{
		switch(element->second.action)
		{
			case Create:
			{
				// Create a new Block Job
				RTRMS_Parameters parameters;
				memset(&parameters, 0, sizeof(RTRMS_Parameters));

				parameters.OutputType = BlockOutput;
				parameters.BlPaddingChar = element->second.paddingChar;
				parameters.BlockHoldTime = element->second.blockHoldTime;
				parameters.BlockLength = element->second.length;
				parameters.BlockLengthType = element->second.lengthType;

				strncpy(parameters.TransferQueue, element->second.transferQueue.c_str(), sizeof(parameters.TransferQueue)/sizeof(parameters.TransferQueue[0]));
				m_poRtrServer->addNewManager(element->second.messageStoreName.c_str(), &parameters, element->second.blockJobDN );
			}
			break;

			case Modify:
			{
				// change Block job parameters
				RTRMS_Parameters parameters;
				memset(&parameters, 0, sizeof(RTRMS_Parameters));

				parameters.OutputType = BlockOutput;
				parameters.BlPaddingChar = element->second.paddingChar;
				parameters.BlockHoldTime = element->second.blockHoldTime;
				parameters.BlockLength = element->second.length;
				parameters.BlockLengthType = element->second.lengthType;

				strncpy(parameters.TransferQueue, element->second.transferQueue.c_str(), sizeof(parameters.TransferQueue)/sizeof(parameters.TransferQueue[0]));

				m_poRtrServer->setManagerParameters(element->second.messageStoreName, &parameters);
			}
			break;
		}
	}

	// Erase all elements from the table of the operations
	m_BlockJobOperationTable.erase(operationRange.first, operationRange.second);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_RTR_BlockBased_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	// to avoid warning msg
	UNUSED(objName);
	UNUSED(attrName);

	ACS_RTR_TRACE_MESSAGE("Out");
	return result;
}

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void ACS_RTR_BlockBased_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList)
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
        ROUTINE: validateBlockTQ
=================================================================== */
bool ACS_RTR_BlockBased_ObjectImpl::validateBlockTQ(const std::string& blockTqName)
{
	ACS_RTR_TRACE_MESSAGE("In, TQ<%s>", blockTqName.c_str());
	bool result = true;

	// Validate block TQ
	unsigned int ohiError = AES_OHI_BlockHandler2::blockTransferQueueDefined(blockTqName.c_str());

	if(AES_OHI_NOERRORCODE != ohiError)
	{
		ACS_RTR_TRACE_MESSAGE(" OHI File Handler error:<%d>", ohiError);
		if(AES_OHI_NOSERVERACCESS == ohiError)
		{
			char errorText[128] = {0};
			strcpy(errorText, CmdPrintouts::IntProgFault);
			strcat(errorText, ": ");
			strcat(errorText, CmdPrintouts::NoDatatransferAccess);
			setExitCode(DATATRANSFERERROR, errorText );
		}
		else
		{
			setExitCode(TQUNDEF, CmdPrintouts::NoTQDef);
		}
		result = false;
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}

/*===================================================================
        ROUTINE: getMessageStoreParameter
=================================================================== */
bool ACS_RTR_BlockBased_ObjectImpl::getMessageStoreParameter(const char* messageStoreDN, std::string& messageStoreName, uint32_t& msgStoreRecordSize)
{
	ACS_RTR_TRACE_MESSAGE("In, MS DN<%s>", messageStoreDN);
	bool result = false;

	OmHandler objManager;
	// Init OmHandler
	ACS_CC_ReturnType getResult = objManager.Init();

	if(ACS_CC_SUCCESS == getResult)
	{
		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		// to get the MS name attribute
		ACS_APGCC_ImmAttribute messageStoreNameAttribute;
		messageStoreNameAttribute.attrName =  rtr_imm::MessageStoreNameAttribute;
		attributeList.push_back(&messageStoreNameAttribute);

		// to get record Size
		ACS_APGCC_ImmAttribute recordSizeAttribute;
		recordSizeAttribute.attrName = rtr_imm::RecordSizeAttibute;
		attributeList.push_back(&recordSizeAttribute);

		getResult = objManager.getAttribute(messageStoreDN, attributeList );

		// check for mandatory attributes
		if( (ACS_CC_FAILURE != getResult) &&
				(0 != messageStoreNameAttribute.attrValuesNum) &&
									(0 != recordSizeAttribute.attrValuesNum) )
		{
			result = true;

			messageStoreName.assign(reinterpret_cast<char *>(messageStoreNameAttribute.attrValues[0]));
			msgStoreRecordSize = *reinterpret_cast<uint32_t*>(recordSizeAttribute.attrValues[0]);

			ACS_RTR_TRACE_MESSAGE(" MS name:<%s>, recordSize:<%d>", messageStoreName.c_str(), msgStoreRecordSize);
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("get attribute <%s> of MS:<%s> failed", rtr_imm::MessageStoreNameAttribute, messageStoreDN);
		}

		// Release all resources
		objManager.Finalize();
	}
	else
	{
		// Init error
		ACS_RTR_TRACE_MESSAGE("OmHandler init FAILURE");
	}

	// Check for error
	if(!result)
	{
		setExitCode(INTERNALERR, CmdPrintouts::IntProgFault);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}

/*===================================================================
        ROUTINE: getMessageStoreName
=================================================================== */
bool ACS_RTR_BlockBased_ObjectImpl::getMessageStoreName(const char* messageStoreDN, std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, MS DN<%s>", messageStoreDN);
	bool result = false;

	OmHandler objManager;
	// Init OmHandler
	ACS_CC_ReturnType getResult = objManager.Init();

	if(ACS_CC_SUCCESS == getResult)
	{
		// to get the MS name attribute
		ACS_CC_ImmParameter messageStoreNameAttribute;
		messageStoreNameAttribute.attrName =  rtr_imm::MessageStoreNameAttribute;

		getResult = objManager.getAttribute(messageStoreDN, &messageStoreNameAttribute );

		// check for mandatory attributes
		if( (ACS_CC_FAILURE != getResult) &&
						(0 != messageStoreNameAttribute.attrValuesNum) )
		{
			result = true;
			messageStoreName.assign(reinterpret_cast<char *>(messageStoreNameAttribute.attrValues[0]));
		}

		// Release all resources
		objManager.Finalize();
	}

	// Check for error
	if(!result)
	{
		setExitCode(INTERNALERR, CmdPrintouts::IntProgFault);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}

/*===================================================================
        ROUTINE: getCurrentParamters
=================================================================== */
bool ACS_RTR_BlockBased_ObjectImpl::getCurrentParamters(BlockJobInfo& currentSet)
{
	ACS_RTR_TRACE_MESSAGE("In");

	RTRMS_Parameters parameters;

	bool result = m_poRtrServer->getManagerParameters(currentSet.messageStoreName, &parameters);

	// check result
	if(result)
	{
		currentSet.length = parameters.BlockLength;
		currentSet.lengthType = parameters.BlockLengthType;
		currentSet.blockHoldTime = parameters.BlockHoldTime;
		currentSet.paddingChar = parameters.BlPaddingChar;
		currentSet.transferQueue.assign(parameters.TransferQueue);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}
