/**
   @file acs_rtr_filebased_ObjectImpl.cpp

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
   N/A       05/12/2013   QVINCON   Re-factoring
*/

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_rtr_filebased_objectImpl.h"
#include "acs_rtr_server.h"
#include "acs_rtr_global.h"
#include "acs_rtr_tracer.h"

#include "acs_apgcc_omhandler.h"
#include "aes_ohi_extfilehandler2.h"


ACS_RTR_TRACE_DEFINE(ACS_RTR_FileBased_ObjectImpl);

namespace fileBasedJobClass{
    const char ImmImplementerName[] = "RTR_OI_FileBaseJob";
}


/*===================================================================
   ROUTINE: ACS_RTR_FileBased_ObjectImpl
=================================================================== */
ACS_RTR_FileBased_ObjectImpl::ACS_RTR_FileBased_ObjectImpl(ACS_RTR_Server* serverObject)
: acs_apgcc_objectimplementereventhandler_V3(fileBasedJobClass::ImmImplementerName),
  m_FileJobOperationTable(),
  m_ImmClassName(rtr_imm::FileBaseJobClassName),
  m_poRtrServer(serverObject)
{
	ACS_RTR_TRACE_MESSAGE("In");

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: ~ACS_RTR_FileBased_ObjectImpl
=================================================================== */
ACS_RTR_FileBased_ObjectImpl::~ACS_RTR_FileBased_ObjectImpl()
{
	ACS_RTR_TRACE_MESSAGE("In");
	m_poRtrServer = NULL;
	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_RTR_FileBased_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_RTR_TRACE_MESSAGE("In, create a FileJOB object MS DN:<%s>", parentName);
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(className);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	uint32_t msgStoreRecordSize = 0U;
	FileJobInfo newFileJob;
	newFileJob.cdrCounterFlag = false;
	newFileJob.fileSize = 0U;
	newFileJob.fixedFileRecordsFlag = FIXED;
	newFileJob.hmacMd5 = false;
	newFileJob.fileHoldTime = 0U;
	newFileJob.minFileSize = 0U;
	newFileJob.paddingChar = 0;
	newFileJob.recordlength = 0;
	newFileJob.action = Create;

	// extract the attributes
	for(size_t idx = 0; attr[idx] != NULL ; ++idx)
	{
		// check if RDN attribute
		if( 0 == ACE_OS::strcmp(rtr_imm::FileBaseJobId, attr[idx]->attrName) )
		{
			newFileJob.fileJobDN.assign(reinterpret_cast<char *>(attr[idx]->attrValues[0]));
			newFileJob.fileJobDN += parseSymbol::comma;
			newFileJob.fileJobDN.append(parentName);
			ACS_RTR_TRACE_MESSAGE("File Job DN:<%s>", newFileJob.fileJobDN.c_str());
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::TransferQueueAttribute, attr[idx]->attrName) )
		{
			newFileJob.transferQueue.assign(reinterpret_cast<char *>(attr[idx]->attrValues[0]));

			ACS_RTR_TRACE_MESSAGE("transferQueue:<%s>", newFileJob.transferQueue.c_str());

			if(!validateFileTQ(newFileJob.transferQueue))
			{
				result = ACS_CC_FAILURE;
				break;
			}
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::CdrCounterFlagAttribute, attr[idx]->attrName) )
		{
			int crdCounterValue = *reinterpret_cast<int*>(attr[idx]->attrValues[0]);
			newFileJob.cdrCounterFlag = (crdCounterValue == ENABLE);
			ACS_RTR_TRACE_MESSAGE(" CDR counter is <%s>", (newFileJob.cdrCounterFlag ? "ENABLE" : "DISABLE") );
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::FileRecordlengthAttribute, attr[idx]->attrName) )
		{
			newFileJob.recordlength = *reinterpret_cast<int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" File Record length:<%d>", newFileJob.recordlength);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::FixedFileRecordsFlagAttribute, attr[idx]->attrName) )
		{
			newFileJob.fixedFileRecordsFlag = *reinterpret_cast<int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" File Record length:<%d>", newFileJob.fixedFileRecordsFlag);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::FileSizeAttribute, attr[idx]->attrName) )
		{
			newFileJob.fileSize = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" File size:<%d>", newFileJob.fileSize);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::MinFileSizeAttribute, attr[idx]->attrName) )
		{
			newFileJob.minFileSize = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" Minimum File size:<%d>", newFileJob.minFileSize);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::HoldTimeAttrribute, attr[idx]->attrName) )
		{
			newFileJob.fileHoldTime = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" File Hold Time:<%d>", newFileJob.fileHoldTime);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::PaddingCharAttribute, attr[idx]->attrName) )
		{
			newFileJob.paddingChar = *reinterpret_cast<int*>(attr[idx]->attrValues[0]);
			ACS_RTR_TRACE_MESSAGE(" File Padding char:<%d>", newFileJob.paddingChar);
			continue;
		}

		if( 0 == ACE_OS::strcmp(rtr_imm::HashKeyAttribute, attr[idx]->attrName) && (attr[idx]->attrValuesNum != 0))
		{
			newFileJob.hmacMd5 = true;
			ACS_RTR_TRACE_MESSAGE(" MD5 is enabled");
			continue;
		}
	}

	if((ACS_CC_FAILURE != result) && getMessageStoreParameter(parentName, newFileJob.messageStoreName, msgStoreRecordSize))
	{
		ACS_RTR_TRACE_MESSAGE("Validate parameters value");

		// Check for parameters error
		if( (FIXED == newFileJob.fixedFileRecordsFlag) && (msgStoreRecordSize > newFileJob.recordlength) )
		{
			setExitCode(MS_BUFFER_REC, CmdPrintouts::MsBufFile);
			result = ACS_CC_FAILURE;
		}
		else if( (newFileJob.fileSize*kiloByteToByte) <= newFileJob.minFileSize )
		{
			setExitCode(UNREASVALUE, CmdPrintouts::UnReasonval);
			result = ACS_CC_FAILURE;
		}
		else if( m_poRtrServer->isJobDefined(newFileJob.messageStoreName,newFileJob.transferQueue))
		{
			setExitCode(MSDEFERROR, CmdPrintouts::TqDefError);
			result = ACS_CC_FAILURE;
		}

		if(ACS_CC_SUCCESS == result)
		{
			m_FileJobOperationTable.insert(pair<ACS_APGCC_CcbId, FileJobInfo>(ccbId, newFileJob));
			ACS_RTR_TRACE_MESSAGE("Create a new File Job");
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out");

	return result;
}

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_RTR_FileBased_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_RTR_TRACE_MESSAGE("In, delete the FileJOB object<%s>", objName);
	// to avoid warning msg
	UNUSED(oiHandle);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	std::string fileJobDN(objName);
	size_t tagCommaPos = fileJobDN.find_first_of(parseSymbol::comma);

	if( std::string::npos != tagCommaPos)
	{
		std::string messageStoreDN(fileJobDN.substr(tagCommaPos+1));

		std::string messageStoreName;
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
ACS_CC_ReturnType ACS_RTR_FileBased_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_RTR_TRACE_MESSAGE("In, modify the FileJOB object<%s>, CCBID:<%zu>", objName, ccbId);
	// to avoid warning msg
	UNUSED(oiHandle);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	std::string fileJobDN(objName);
	size_t tagCommaPos = fileJobDN.find_first_of(parseSymbol::comma);

	if( std::string::npos != tagCommaPos)
	{
		// File Job setting
		FileJobInfo modFileJob;
		modFileJob.action = Modify;

		// Message store DN
		std::string messageStoreDN(fileJobDN.substr(tagCommaPos+1));

		bool getParametersResult = true;

		// Check if already present this transaction
		operationTable::iterator element = m_FileJobOperationTable.find(ccbId);
		if(m_FileJobOperationTable.end() != element)
		{
			// Update previous info
			ACS_RTR_TRACE_MESSAGE("Multi attribute change, update previous info");
			modFileJob = element->second;
			m_FileJobOperationTable.erase(element);
		}
		else
		{
			// new transaction change get current parameters
			// get Message store parameters
			getParametersResult = (	getMessageStoreParameter(messageStoreDN.c_str(), modFileJob.messageStoreName, modFileJob.msgStoreRecordSize)
						&&  getCurrentParamters(modFileJob) );
		}

		// check get result
		if( getParametersResult )
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

				if( 0 == ACE_OS::strcmp(rtr_imm::CdrCounterFlagAttribute, modAttribute.attrName) )
				{
					int crdCounterValue = *reinterpret_cast<int*>(modAttribute.attrValues[0]);
					modFileJob.cdrCounterFlag = (crdCounterValue == ENABLE);
					ACS_RTR_TRACE_MESSAGE(" CDR counter is <%s>", (modFileJob.cdrCounterFlag ? "ENABLE" : "DISABLE") );
					continue;
				}

				if( 0 == ACE_OS::strcmp(rtr_imm::FileRecordlengthAttribute, modAttribute.attrName) )
				{
					modFileJob.recordlength = *reinterpret_cast<int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" File Record length:<%d>", modFileJob.recordlength);
					continue;
				}

				if( 0 == ACE_OS::strcmp(rtr_imm::FixedFileRecordsFlagAttribute, modAttribute.attrName) )
				{
					modFileJob.fixedFileRecordsFlag = *reinterpret_cast<int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" Fixed file Record flag:<%d>", modFileJob.fixedFileRecordsFlag);
					continue;
				}

				if( 0 == ACE_OS::strcmp(rtr_imm::FileSizeAttribute, modAttribute.attrName) )
				{
					modFileJob.fileSize = *reinterpret_cast<unsigned int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" File size:<%d>", modFileJob.fileSize);
					continue;
				}

				if( 0 == ACE_OS::strcmp(rtr_imm::MinFileSizeAttribute, modAttribute.attrName) )
				{
					modFileJob.minFileSize = *reinterpret_cast<unsigned int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" Minimum File size:<%d>", modFileJob.minFileSize);
					continue;
				}

				if( 0 == ACE_OS::strcmp(rtr_imm::HoldTimeAttrribute, modAttribute.attrName) )
				{
					modFileJob.fileHoldTime = *reinterpret_cast<unsigned int*>(modAttribute.attrValues[0]);
					ACS_RTR_TRACE_MESSAGE(" File Hold Time:<%d>", modFileJob.fileHoldTime);
					continue;
				}

				if( 0 == ACE_OS::strcmp(rtr_imm::HashKeyAttribute, modAttribute.attrName) )
				{
					modFileJob.hmacMd5 = (0 != modAttribute.attrValuesNum);

					ACS_RTR_TRACE_MESSAGE(" MD5 changed to <%s>", (modFileJob.hmacMd5 ? "ON" :"OFF") );
					continue;
				}
			}

			if( ACS_CC_FAILURE != result )
			{
				// validate new parameters
				if( (FIXED == modFileJob.fixedFileRecordsFlag) && (modFileJob.msgStoreRecordSize > modFileJob.recordlength) )
				{
					setExitCode(MS_BUFFER_REC, CmdPrintouts::MsBufFile);
					result = ACS_CC_FAILURE;
				}
				else if( (modFileJob.fileSize*kiloByteToByte) <= modFileJob.minFileSize )
				{
					setExitCode(UNREASVALUE, CmdPrintouts::UnReasonval);
					result = ACS_CC_FAILURE;
				}

				if(ACS_CC_SUCCESS == result)
				{
					m_FileJobOperationTable.insert(std::make_pair(ccbId, modFileJob));
					ACS_RTR_TRACE_MESSAGE("Modify the File Job");
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
ACS_CC_ReturnType ACS_RTR_FileBased_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
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
void ACS_RTR_FileBased_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	UNUSED(ccbId);
	// find all operations related to the ccbid
	std::pair<operationTable::iterator, operationTable::iterator> operationRange;
	operationRange = m_FileJobOperationTable.equal_range(ccbId);

	// Erase all elements from the table of the operations
	m_FileJobOperationTable.erase(operationRange.first, operationRange.second);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_RTR_FileBased_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_RTR_TRACE_MESSAGE("In");
	// to avoid warning msg
	UNUSED(oiHandle);
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	// find all operations related to the ccbid
	std::pair<operationTable::iterator, operationTable::iterator> operationRange;
	operationRange = m_FileJobOperationTable.equal_range(ccbId);

	//for each operation found
	operationTable::iterator element;
	for(element = operationRange.first; (element != operationRange.second) && (result == ACS_CC_SUCCESS); ++element)
	{
		switch(element->second.action)
		{
			case Create:
			{
				RTRMS_Parameters parameters;
				memset(&parameters, 0, sizeof(RTRMS_Parameters));

				parameters.OutputType = FileOutput;
				parameters.HmacMd5 = element->second.hmacMd5;
				parameters.FiCdrFormat = element->second.cdrCounterFlag;
				parameters.FileSize = element->second.fileSize;
				parameters.FiFileHoldTime = element->second.fileHoldTime;
				parameters.FixedFileRecords = element->second.fixedFileRecordsFlag;
				parameters.FileRecordLength = element->second.recordlength;
				parameters.FiPaddingChar = element->second.paddingChar;
				parameters.MinFileSize = element->second.minFileSize;
				strncpy(parameters.TransferQueue, element->second.transferQueue.c_str(), sizeof(parameters.TransferQueue)/sizeof(parameters.TransferQueue[0]));

				m_poRtrServer->addNewManager(element->second.messageStoreName.c_str(), &parameters, element->second.fileJobDN );
			}
			break;

			case Modify:
			{
				// change file job parameter
				RTRMS_Parameters parameters;
				memset(&parameters, 0, sizeof(RTRMS_Parameters));

				parameters.OutputType = FileOutput;
				parameters.HmacMd5 = element->second.hmacMd5;
				parameters.FiCdrFormat = element->second.cdrCounterFlag;
				parameters.FileSize = element->second.fileSize;
				parameters.FiFileHoldTime = element->second.fileHoldTime;
				parameters.FixedFileRecords = element->second.fixedFileRecordsFlag;
				parameters.FileRecordLength = element->second.recordlength;
				parameters.FiPaddingChar = element->second.paddingChar;
				parameters.MinFileSize = element->second.minFileSize;
				strncpy(parameters.TransferQueue, element->second.transferQueue.c_str(), sizeof(parameters.TransferQueue)/sizeof(parameters.TransferQueue[0]) );

				m_poRtrServer->setManagerParameters(element->second.messageStoreName, &parameters);
			}
			break;

		}
	}

	// Erase all elements from the table of the operations
	m_FileJobOperationTable.erase(operationRange.first, operationRange.second);

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_RTR_FileBased_ObjectImpl::updateRuntime(const char *objName, const char **attrName)
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
void ACS_RTR_FileBased_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType** paramList)
{
	ACS_RTR_TRACE_MESSAGE("In, File Job DN:<%s> action ID:<%d>", p_objName, static_cast<int>(operationId) );

	// to avoid warning msg
	UNUSED(paramList);

	std::string fileJobDN(p_objName);
	size_t tagCommaPos = fileJobDN.find_first_of(parseSymbol::comma);
	bool result = false;
	if( std::string::npos != tagCommaPos)
	{
		// Message store DN
		std::string messageStoreDN(fileJobDN.substr(tagCommaPos+1));
		std::string messageStoreName;

		// Get the message store name
		if(getMessageStoreName(messageStoreDN.c_str(), messageStoreName))
		{
			ACS_RTR_TRACE_MESSAGE("force file end of MS:<%s>", messageStoreName.c_str());
			m_poRtrServer->forceFileEnd(messageStoreName);
			result = true;
		}
	}

	if(result)
	{
		ACS_CC_ReturnType result = adminOperationResult(oiHandle, invocation, actionResult::SUCCESS);
		if(ACS_CC_SUCCESS != result)
		{
			ACS_RTR_TRACE_MESSAGE("error on action result reply");
		}
	}
	else
	{
		char errMsg[512] = {0};
		snprintf(errMsg, sizeof(errMsg),"%s%s", actionResult::NBI_PREFIX, "Unable to get Message Store" );

		ACS_APGCC_AdminOperationParamType errorMessageParameter;
		errorMessageParameter.attrName = actionResult::ErrorMessageAttribute;
		errorMessageParameter.attrType = ATTR_STRINGT;
		errorMessageParameter.attrValues = reinterpret_cast<void*>(errMsg);
		std::vector<ACS_APGCC_AdminOperationParamType> outParameteres;
		outParameteres.push_back(errorMessageParameter);

		ACS_CC_ReturnType result = adminOperationResult(oiHandle, invocation, actionResult::FAILED, outParameteres);
		if(ACS_CC_SUCCESS != result)
		{
			ACS_RTR_TRACE_MESSAGE("error on action result reply");
		}
	}
	
	ACS_RTR_TRACE_MESSAGE("OUT");
}

/*===================================================================
        ROUTINE: validateFileTQ
=================================================================== */
bool ACS_RTR_FileBased_ObjectImpl::validateFileTQ(const std::string& fileTqName)
{
	ACS_RTR_TRACE_MESSAGE("In, TQ<%s>", fileTqName.c_str());
	bool result = false;

	AES_OHI_ExtFileHandler2 mySenderFile(OHI_USERSPACE::SUBSYS, OHI_USERSPACE::APPNAME);

	unsigned int ohiErrorCode = mySenderFile.attach();

	//check attach operation
	if(AES_OHI_NOERRORCODE == ohiErrorCode )
	{
		// Check the TQ
		ohiErrorCode = mySenderFile.fileTransferQueueDefined(fileTqName.c_str());

		result = (AES_OHI_NOERRORCODE == ohiErrorCode);
		//detach
		mySenderFile.detach();

	}

	if(!result)
	{
		ACS_RTR_TRACE_MESSAGE(" OHI File Handler error:<%d>", ohiErrorCode);
		if(AES_OHI_NOSERVERACCESS == ohiErrorCode)
		{
			char errorText[128] = {0};
			strcpy(errorText, CmdPrintouts::IntProgFault);
			strcat(errorText, ": ");
			strcat(errorText, CmdPrintouts::NoDatatransferAccess);
			setExitCode(DATATRANSFERERROR, errorText);
		}
		else
		{
			setExitCode(TQUNDEF, CmdPrintouts::NoTQDef);
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}

/*===================================================================
        ROUTINE: getMessageStoreParameter
=================================================================== */
bool ACS_RTR_FileBased_ObjectImpl::getMessageStoreParameter(const char* messageStoreDN, std::string& messageStoreName, uint32_t& msgStoreRecordSize)
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

			ACS_RTR_TRACE_MESSAGE(" MS name:<%s>, reocrdSize:<%d>", messageStoreName.c_str(), msgStoreRecordSize);
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
bool ACS_RTR_FileBased_ObjectImpl::getMessageStoreName(const char* messageStoreDN, std::string& messageStoreName)
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
bool ACS_RTR_FileBased_ObjectImpl::getCurrentParamters(FileJobInfo& currentSet)
{
	ACS_RTR_TRACE_MESSAGE("In");

	RTRMS_Parameters parameters;

	bool result = m_poRtrServer->getManagerParameters(currentSet.messageStoreName, &parameters);

	// Check result
	if(result)
	{
		currentSet.hmacMd5 = parameters.HmacMd5;
		currentSet.cdrCounterFlag = parameters.FiCdrFormat;
		currentSet.fileSize = parameters.FileSize;
		currentSet.fileHoldTime = parameters.FiFileHoldTime;
		currentSet.fixedFileRecordsFlag = parameters.FixedFileRecords;
		currentSet.recordlength = parameters.FileRecordLength;
		currentSet.paddingChar = parameters.FiPaddingChar;
		currentSet.minFileSize = parameters.MinFileSize;
		currentSet.transferQueue.assign(parameters.TransferQueue);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}
