//******************************************************************************
//
// NAME
// ACS_RTR_statistics.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2002.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.

// DOCUMENT NO
//	190 89-CAA 109 0572

// AUTHOR 
// 2004-12-20 by EAB/UZ/DG UABCAJN

// DESCRIPTION
// This class implements the statistical part of RTR, i.e storing/retrieval of
// statistics from a mapped statistics file.
//
//  CHANGES
//    RELEASE REVISION HISTORY
//    DATE          NAME       DESCRIPTION
//    2007-05-02    qpaoele    Blade Cluster adaptations
//    2010-05-18    XCSRPAD    HL89409
//******************************************************************************

#include "acs_rtr_statistics.h"
#include "acs_rtr_global.h"
#include "acs_rtr_server.h"

#include "acs_rtr_macros.h"
#include "acs_rtr_cpstatistics_impl.h"
#include "acs_rtr_statistics_impl.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_CommonLib.h"

namespace {
	const char* const StatisticsFileName = "statistics";
	const int StatisticsFileSize = 1024; // size in byte
	const int SIZEOF_CHARBUFFER  = 21;
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_statistics);

//----------------------------------------
// Constructor
// take the absolute statistics file path
//----------------------------------------
RTR_statistics::RTR_statistics(const std::string& messageStoreName)
: m_mutex(),
  m_initialized(false),
  m_isIMMObjectInit(false),
  m_isFileMapInit(false),
  m_RDNKey(1),
  m_isCpObject(false),
  m_jobDN(),
  m_statisticFilePath(),
  m_mappedMemoryAddr(MAP_FAILED),
  posFileCreationTime(NULL),
  posKbACA(NULL),
  posKbGOH(NULL),
  posFilled(NULL),
  posLost(NULL),
  posSkipped(NULL),
  posConnMS(NULL),
  posAttachGOH(NULL),
  posLastComBno(NULL),
  tempFilled(0U),
  tempLost(0U),
  tempSkipped(0U)
{
	setStatisticFilePath(messageStoreName);
	ACS_RTR_TRACE_MESSAGE("CPid:<%d>, file:<%s>", m_RDNKey, m_statisticFilePath.c_str());
}

RTR_statistics::RTR_statistics(short cpId, const std::string& messageStoreName)
: m_mutex(),
  m_initialized(false),
  m_isIMMObjectInit(false),
  m_isFileMapInit(false),
  m_RDNKey(cpId),
  m_isCpObject(true),
  m_jobDN(),
  m_statisticFilePath(),
  m_mappedMemoryAddr(MAP_FAILED),
  posFileCreationTime(NULL),
  posKbACA(NULL),
  posKbGOH(NULL),
  posFilled(NULL),
  posLost(NULL),
  posSkipped(NULL),
  posConnMS(NULL),
  posAttachGOH(NULL),
  posLastComBno(NULL),
  tempFilled(0U),
  tempLost(0U),
  tempSkipped(0U)
{
	setCpStatisticFilePath(messageStoreName);

	ACS_RTR_TRACE_MESSAGE("CPid:<%d>, file:<%s>", m_RDNKey, m_statisticFilePath.c_str());
}

//------------
// Destructor
//------------
RTR_statistics::~RTR_statistics()
{
	// Close the memory map to the statistic file
	if(MAP_FAILED != m_mappedMemoryAddr)
	{
		ACE_OS::munmap( m_mappedMemoryAddr, StatisticsFileSize);
		m_mappedMemoryAddr = MAP_FAILED;
	}
}

bool RTR_statistics::statisticsInfoInit(const std::string& jobDn)
{
	ACS_RTR_TRACE_MESSAGE("In, job DN:<%s>", jobDn.c_str());

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(!m_initialized)
	{
		//create MO into IMM
		if(!m_isIMMObjectInit)
		{
			m_isIMMObjectInit = statisticIMMObjectCreate(jobDn);
			m_jobDN.assign(jobDn);
		}

		// map the file to the memory
		if(!m_isFileMapInit )
		{
			m_isFileMapInit = mapToMemoryStatisticFile();
		}

		// all initializations are done
		m_initialized = (m_isIMMObjectInit && m_isFileMapInit);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (m_initialized ? "TRUE" : "FALSE" ));
	return m_initialized;
}

bool RTR_statistics::cpStatisticsInfoInit(const std::string& jobDn)
{
	ACS_RTR_TRACE_MESSAGE("In, job DN:<%s>", jobDn.c_str());

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(!m_initialized)
	{
		m_isCpObject = true;
		//create MO into IMM
		if(!m_isIMMObjectInit)
		{
			m_isIMMObjectInit = cpStatisticIMMObjectCreate(jobDn);
			m_jobDN.assign(jobDn);
		}

		// map the file to the memory
		if(!m_isFileMapInit )
		{
			m_isFileMapInit = mapToMemoryStatisticFile();
		}

		// all initializations are done
		m_initialized = (m_isIMMObjectInit && m_isFileMapInit);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (m_initialized ? "TRUE" : "FALSE" ));
	return m_initialized;
}

void RTR_statistics::getCpStatisticsInfo(CpStatisticsInfo* cpStatisticsInfo)
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(m_initialized)
	{
		cpStatisticsInfo->recordsRead = *posFilled;
		cpStatisticsInfo->recordsLost = *posLost;
		cpStatisticsInfo->recordsSkipped = *posSkipped;
	}
	else
	{
		cpStatisticsInfo->recordsRead = 0U;
		cpStatisticsInfo->recordsLost = 0U;
		cpStatisticsInfo->recordsSkipped = 0U;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

void RTR_statistics::getMessageStoreStatistics(StatisticsInfo* msStatisticsInfo)
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(m_initialized)
	{
		msStatisticsInfo->volumeRead = ((*posKbACA)>>10);
		msStatisticsInfo->volumeReported = ((*posKbGOH)>>10);
	}
	else
	{
		msStatisticsInfo->volumeRead = 0U;
		msStatisticsInfo->volumeReported = 0U;
	}
}
//------------------------------------------------------------------
// incFilled
// Increase the 'Filled' counter. Filled means a buffer with a data.
//------------------------------------------------------------------
void RTR_statistics::incFilled()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	tempFilled++;

	if(!m_initialized)	objectInitRecovery();

	if(NULL != posFilled) *posFilled += 1;
}

//-----------------------------
// incLost
// Increase the 'Lost' counter. 
//-----------------------------
void RTR_statistics::incLost()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	tempLost++;
}

//--------------------------------
// incSkipped
// Increase the 'Skipped' counter. 
//--------------------------------
void RTR_statistics::incSkipped()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	tempSkipped++;
}

//-----------------------------------------------
// incConnMSattempts
// Increase the 'Message store connect' counter. 
//-----------------------------------------------
void RTR_statistics::incConnMSattempts()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(!m_initialized)	objectInitRecovery();

	if(NULL != posConnMS) *posConnMS +=1;
}

//--------------------------------------------
// incAttachGOHattempts
// Increase the 'GOH attach attempt' counter. 
//--------------------------------------------
void RTR_statistics::incAttachGOHattempts()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(!m_initialized)	objectInitRecovery();

	if(NULL != posAttachGOH) *posAttachGOH +=1;
}

//---------------------------------------------------------------------
// addReadMSdata
// Increase the number of KB read from Message store.
// This method is called on file boundarys so the 'Filled', 'Lost' and
// 'Skipped' counters are also updated here.
//---------------------------------------------------------------------
void RTR_statistics::addReadMSdata(unsigned int MSbytes)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	if(!m_initialized)	objectInitRecovery();

	if(NULL != posKbACA) *posKbACA += MSbytes;

	if(NULL != posFilled) *posFilled += tempFilled;

	if(NULL!= posLost) *posLost += tempLost;

	if(NULL != posSkipped) *posSkipped += tempSkipped;

	tempFilled = tempLost = tempSkipped = 0U;
}

//--------------------------------------------
// addReportedData
// Increase the number of Kb reported to GOH.
//--------------------------------------------
void RTR_statistics::addReportedData(unsigned int GOHbytes)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	if(!m_initialized)	objectInitRecovery();

	if(NULL != posKbGOH) *posKbGOH = (*posKbGOH) + GOHbytes;
}

//-----------------------------------------------------------
// getFormattedBuffer
// Build a formatted statistics buffer, ready to be printed.
//-----------------------------------------------------------
void RTR_statistics::getFormattedBuffer(char* formBuf, size_t bufSize, int typeOfTransfer)
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	char buf1[80] = {0};
	char buf2[80] = {0};
	char buf3[80] = {0};
	char buf4[80] = {0};
	char buf5[80] = {0};

	if( m_isFileMapInit )
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Statistic file:<%s> is not mapped", m_statisticFilePath.c_str() );
		ACS_RTR_TRACE_MESSAGE("Error: Statistic file:<%s> is not mapped", m_statisticFilePath.c_str() );
	}

	if(0 == typeOfTransfer)
	{
		// file transfer only
		int64_t kbToACA = (NULL != posKbACA) ? ((*posKbACA)>>10) : 0U; //HL89409
		ACE_OS::snprintf(buf1, sizeof(buf1) - 1,"Volume read     (KB)      :%lu\n", kbToACA);//HL89409
	}
	else
	{
		// block transfer only
		uint32_t lastComBno = (NULL != posLastComBno) ? (*posLastComBno) : 0U;
		ACE_OS::snprintf(buf1, sizeof(buf1) - 1, "Last confirmed Block No   :%u\n", lastComBno);
	}

	int64_t kbToGOH = (NULL != posKbGOH) ? ((*posKbGOH)>>10) : 0U; //HL89409
	ACE_OS::snprintf(buf2, sizeof(buf2) - 1, "Volume reported (KB)      :%lu\n", kbToGOH);

	uint32_t tmpValue;
	tmpValue = (NULL != posFilled) ? (*posFilled) : 0U;
	ACE_OS::snprintf(buf3, sizeof(buf3) - 1, "MS buffers read           :%u\n", tmpValue);

	tmpValue = (NULL != posLost) ? (*posLost) : 0U;
	ACE_OS::snprintf(buf4, sizeof(buf4) - 1, "MS buffers lost           :%u\n", tmpValue);

	tmpValue = (NULL != posSkipped) ? (*posSkipped) : 0U;
	ACE_OS::snprintf(buf5, sizeof(buf5) - 1, "MS buffers skipped        :%u\n", tmpValue);

	ACE_OS::snprintf(formBuf, bufSize - 1, "%s%s%s%s%s",  buf1, buf2, buf3, buf4, buf5);
}


//-------------------------------------------------------
// clearBLD
// Reset temporary counters increased during file build.
//-------------------------------------------------------
void RTR_statistics::clearBLD()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	tempFilled=0;
	tempLost=0;
	tempSkipped=0;
}

//---------------------------------------------
// delStatFile
// Deletes the statistics file.
// This method MUST be called on rtrrm command.
//---------------------------------------------
int RTR_statistics::delStatFile()
{
	return(ACE_OS::unlink(m_statisticFilePath.c_str()));
}


void RTR_statistics::statParFormat(char* formBuf, size_t bufSize, int typeOfTransfer)
{
	ACS_RTR_TRACE_FUNCTION;
	(void)typeOfTransfer;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	char buf1[80] = {0};
	char buf2[80] = {0};
	char buf3[80] = {0};
	ACE_OS::snprintf(buf1, sizeof(buf1) - 1, "RUNTIME STATISTICS\n");

	int64_t kbToACA = (NULL != posKbACA) ? ((*posKbACA)>>10) : 0U; //HL89409
	ACE_OS::snprintf(buf2, sizeof(buf2) - 1, "Volume read      (Kb)     %lu\n", kbToACA);//right shift

	int64_t kbToGOH = (NULL != posKbGOH) ? ((*posKbGOH)>>10) : 0U; //HL89409
	ACE_OS::snprintf(buf3, sizeof(buf3) - 1, "Volume reported  (Kb)     %lu\n", kbToGOH);

	ACE_OS::snprintf(formBuf, bufSize - 1, "%s%s%s",  buf1, buf2, buf3);
}


//---------------------------------------------------------------------------
//                              cpStatForrmat
//---------------------------------------------------------------------------
void RTR_statistics::cpStatFormat(char* formBuf, size_t bufSize, const string cp)
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	char buf1[80] = {0};
	char buf2[80] = {0};
	char buf3[80] = {0};
	char buf4[80] = {0};
	char buf5[80] = {0};
	ACE_OS::snprintf(buf1, sizeof(buf1) - 1,  "NODE\n");
	ACE_OS::snprintf(buf2, sizeof(buf2) - 1,  "%s\n", cp.c_str());

	uint32_t tmpValue;
	tmpValue = (NULL != posFilled) ? (*posFilled) : 0U;
	ACE_OS::snprintf(buf3, sizeof(buf3) - 1, "MS buffers read           %u\n", tmpValue);

	tmpValue = (NULL != posLost) ? (*posLost) : 0U;
	ACE_OS::snprintf(buf4, sizeof(buf4) - 1, "MS buffers lost           %u\n", tmpValue);

	tmpValue = (NULL != posSkipped) ? (*posSkipped) : 0U;
	ACE_OS::snprintf(buf5, sizeof(buf5) - 1, "MS buffers skipped        %u\n", tmpValue);

	ACE_OS::snprintf(formBuf, bufSize - 1, "%s%s%s%s%s",  buf1, buf2, buf3, buf4, buf5);
}

void RTR_statistics::setStatisticFilePath(const std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());

	// get RTR internal path
	getRTRInternalPath(m_statisticFilePath);

	char fileName[32] = {0};

	// Assemble the file name
	if(ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
	{
		snprintf(fileName, sizeof(fileName)-1, "%s.%s", StatisticsFileName, messageStoreName.c_str() );
	}
	else
	{
		snprintf(fileName, sizeof(fileName)-1, "%s/%s", messageStoreName.c_str(), StatisticsFileName );
	}

	m_statisticFilePath.append(fileName);
	ACS_RTR_TRACE_MESSAGE("Out, statistic file:<%s>", m_statisticFilePath.c_str());
}

void RTR_statistics::setCpStatisticFilePath(const std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());

	// get RTR internal path
	getRTRInternalPathForCp(m_RDNKey, m_statisticFilePath);

	char fileName[32] = {0};

	snprintf(fileName, sizeof(fileName)-1, "%s/%s", messageStoreName.c_str(), StatisticsFileName );

	m_statisticFilePath.append(fileName);
	ACS_RTR_TRACE_MESSAGE("Out, statistic file:<%s>", m_statisticFilePath.c_str());
}

void RTR_statistics::getRTRInternalPath(std::string& path)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;
	ACS_APGCC_DNFPath_ReturnTypeT getResult;
	ACS_APGCC_CommonLib dataDiskHandler;

	int bufferLength = 512;
	char buffer[bufferLength];

	ACE_OS::memset(buffer, 0, bufferLength);
	// get the physical path
	getResult = dataDiskHandler.GetDataDiskPath(RTR_NBI::DATADISK_ATTRIBUTE, buffer, bufferLength);

	if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
	{
		// path get successful
		path = buffer;
		result = true;
	}
	else if(ACS_APGCC_STRING_BUFFER_SMALL == getResult)
	{
		// Buffer too small, but now we have the right size
		char buffer2[bufferLength+1];
		ACE_OS::memset(buffer2, 0, bufferLength+1);
		// try again to get
		getResult = dataDiskHandler.GetDataDiskPath(RTR_NBI::DATADISK_ATTRIBUTE, buffer2, bufferLength);

		// Check if it now is ok
		if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
		{
			// path get successful now
			path = buffer;
			result = true;
		}
	}

	if(!result)
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: Failed to get RTR data disk path, error:<%d>", getResult);
		path = RTR_NBI::DataDiskPath;
		ACS_RTR_LOG(LOG_LEVEL_WARN, "Set Statistic file path to default path:<%s> ", path.c_str());
	}

	path.push_back(DirDelim);

	ACS_RTR_TRACE_MESSAGE("Out, path:<%s>", path.c_str());
}

void RTR_statistics::getRTRInternalPathForCp(const unsigned int& cpId, std::string& path)
{
	ACS_RTR_TRACE_MESSAGE("In, CPid:<%d>", cpId);
	bool result = false;
	ACS_APGCC_DNFPath_ReturnTypeT getResult;
	ACS_APGCC_CommonLib dataDiskHandler;

	int bufferLength = 512;
	char buffer[bufferLength];

	ACE_OS::memset(buffer, 0, bufferLength);
	// get the physical path
	getResult = dataDiskHandler.GetDataDiskPathForCp(RTR_NBI::DATADISK_ATTRIBUTE, cpId, buffer, bufferLength);

	if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
	{
		// path get successful
		path = buffer;
		result = true;
	}
	else if(ACS_APGCC_STRING_BUFFER_SMALL == getResult)
	{
		// Buffer too small, but now we have the right size
		char buffer2[bufferLength+1];
		ACE_OS::memset(buffer2, 0, bufferLength+1);
		// try again to get
		getResult = dataDiskHandler.GetDataDiskPathForCp(RTR_NBI::DATADISK_ATTRIBUTE, cpId, buffer2, bufferLength);

		// Check if it now is ok
		if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
		{
			// path get successful now
			path = buffer;
			result = true;
		}
	}

	// Check the result
	if(!result)
	{
		// Set the hard coded path
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: Failed to get RTR data disk path, error:<%d>", getResult);
		path = RTR_NBI::DataDiskPath;
		path.push_back(DirDelim);

		if(cpId < SysType_CP)
		{
		   // For example "BC0"
			std::stringstream defaultCpName("BC");
			defaultCpName << cpId;
			path.append(defaultCpName.str());
		}
		else
		{
			// For example "CP1"
			std::stringstream defaultCpName("CP");
			defaultCpName << (cpId - 1000);
			path.append(defaultCpName.str());
		}
		ACS_RTR_LOG(LOG_LEVEL_WARN, "Set Cp:<%d> Statistic file path to default path:<%s>", cpId, path.c_str());
	}

	path.push_back(DirDelim);

	ACS_RTR_TRACE_MESSAGE("Out, path:<%s>", path.c_str());
}

bool RTR_statistics::statisticIMMObjectCreate(const std::string& parentDN )
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	OmHandler objManager;
	if( ACS_CC_SUCCESS == objManager.Init() )
	{
		std::vector<ACS_CC_ValuesDefinitionType> attributesList;
		ACS_CC_ValuesDefinitionType attributeRDN;
		char tmpRDNValue[32] = {0};

		attributeRDN.attrName = rtr_imm::StatisticsInfoId;
		attributeRDN.attrType = ATTR_STRINGT;
		attributeRDN.attrValuesNum = 1;

		// Assemble the statistic object RDN
		snprintf(tmpRDNValue, sizeof(tmpRDNValue)-1, "%s=1", rtr_imm::StatisticsInfoId);
		void* valueRDN[1] = {reinterpret_cast<void*>(tmpRDNValue)};
		attributeRDN.attrValues = valueRDN;

		//Add the attributes to vector
		attributesList.push_back(attributeRDN);

		ACS_CC_ReturnType ImmResult = objManager.createObject(rtr_imm::StatisticsInfoClassName, parentDN.c_str(), attributesList );
		// create the message store statistic object
		if( (ACS_CC_SUCCESS == ImmResult) || (IMM_MO_ALREADYEXIST == objManager.getInternalLastError()) )
		{
			result = true;
			// On SCP create also the cp statistic MO
			if(!ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
			{
				result = cpStatisticIMMObjectCreate(parentDN);
			}
		}
		else
		{
			// Report error
			char errorMsg[512] = {0};
			snprintf(errorMsg, sizeof(errorMsg)-1, "ERROR: Failed to create MOC:<%s> RDN:<%s> under DN:<%s>, IMM error:<%d> ",
						rtr_imm::StatisticsInfoClassName, tmpRDNValue, parentDN.c_str(), objManager.getInternalLastError());

			ACS_RTR_TRACE_MESSAGE("%s", errorMsg);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", errorMsg);
		}

		// free resources
		objManager.Finalize();
	}
	else
	{
		// IMM handler init failed
		ACS_RTR_TRACE_MESSAGE("Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
	}

	ACS_RTR_TRACE_MESSAGE("Out,result:<%s>", (result ? "TRUE" : "FALSE") );
	return result;
}

bool RTR_statistics::cpStatisticIMMObjectCreate(const std::string& jobDN )
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	OmHandler objManager;
	if( ACS_CC_SUCCESS == objManager.Init() )
	{
		std::vector<ACS_CC_ValuesDefinitionType> attributesList;
		ACS_CC_ValuesDefinitionType attributeRDN;
		char tmpRDNValue[32] = {0};

		attributeRDN.attrName = rtr_imm::CpStatisticsInfoId;
		attributeRDN.attrType = ATTR_STRINGT;
		attributeRDN.attrValuesNum = 1;

		// Assemble the statistic object RDN
		snprintf(tmpRDNValue, sizeof(tmpRDNValue)-1, "%s=%d", rtr_imm::CpStatisticsInfoId, m_RDNKey);
		void* valueRDN[1] = {reinterpret_cast<void*>(tmpRDNValue)};
		attributeRDN.attrValues = valueRDN;

		//Add the attributes to vector
		attributesList.push_back(attributeRDN);

		char parentDN[256] = {0};
		// Assemble the statistic object RDN
		snprintf(parentDN, sizeof(parentDN)-1, "%s=1,%s", rtr_imm::StatisticsInfoId, jobDN.c_str());

		ACS_CC_ReturnType ImmResult = objManager.createObject(rtr_imm::CpStatisticsInfoClassName, parentDN, attributesList );
		// create the message store statistic object
		if( (ACS_CC_SUCCESS == ImmResult) || (IMM_MO_ALREADYEXIST == objManager.getInternalLastError()) )
		{
			result = true;
		}
		else
		{
			// Report error
			char errorMsg[512] = {0};
			snprintf(errorMsg, sizeof(errorMsg)-1, "ERROR: Failed to create MOC:<%s> RDN:<%s> under DN:<%s>, IMM error:<%d> ",
						rtr_imm::CpStatisticsInfoClassName, tmpRDNValue, parentDN, objManager.getInternalLastError());

			ACS_RTR_TRACE_MESSAGE("%s", errorMsg);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", errorMsg);
		}

		// free resources
		objManager.Finalize();
	}
	else
	{
		// IMM handler init failed
		ACS_RTR_TRACE_MESSAGE("Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
	}

	ACS_RTR_TRACE_MESSAGE("Out,result:<%s>", (result ? "TRUE" : "FALSE") );
	return result;
}

ACE_HANDLE RTR_statistics::openStatisticFile(bool& fileCreated)
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACE_HANDLE fileStateDescriptor = ACE_INVALID_HANDLE;
	fileCreated = false;

	// try to open the statistic file if it already exists
	fileStateDescriptor = ACE_OS::open(m_statisticFilePath.c_str(), O_RDWR);

	if(ACE_INVALID_HANDLE == fileStateDescriptor)
	{
		ACS_RTR_TRACE_MESSAGE("Create Statistic file:<%s>", m_statisticFilePath.c_str());

		// Check if all path exist
		checkFolders();

		// create the statistic file
		fileStateDescriptor = ACE_OS::open(m_statisticFilePath.c_str(), O_CREAT | O_RDWR);

		if( ACE_INVALID_HANDLE != fileStateDescriptor )
		{
			// file created for the first time
			fileCreated = true;

			// Prepare a file large enough to hold all data
			char emptyBuffer[StatisticsFileSize] = {0};

			if(ACE_OS::write(fileStateDescriptor, emptyBuffer, StatisticsFileSize) < StatisticsFileSize)
			{
				// some error happens on lseek
				char errorMsg[64]={0};
				std::string errorDetail(strerror_r(ACE_OS::last_error(), errorMsg, 63));

				ACS_RTR_LOG(LOG_LEVEL_ERROR, "failed to write to file:<%s>. Error:<%s>", m_statisticFilePath.c_str(), errorDetail.c_str());
				ACS_RTR_TRACE_MESSAGE("Error: failed to write to file:<%s>. Error:<%s>", m_statisticFilePath.c_str(), errorDetail.c_str());

				ACE_OS::close(fileStateDescriptor);
				fileStateDescriptor = ACE_INVALID_HANDLE;
				// remove the created file
				::remove(m_statisticFilePath.c_str());
			}
			else
			{
				// write a terminating cap
				if( ACE_OS::write(fileStateDescriptor, "", 1) != -1)
				{
					// reposition to begin
					ACE_OS::lseek(fileStateDescriptor, 0, SEEK_SET);
					ACS_RTR_TRACE_MESSAGE("statistic file:<%s> created!", m_statisticFilePath.c_str());
				}
				else
				{
					// some error happens on lseek
					char errorMsg[64]={0};
					std::string errorDetail(strerror_r(ACE_OS::last_error(), errorMsg, 63));

					ACS_RTR_LOG(LOG_LEVEL_ERROR, "failed to write to file:<%s>. Error:<%s>", m_statisticFilePath.c_str(), errorDetail.c_str());
					ACS_RTR_TRACE_MESSAGE("Error: failed to write to file:<%s>. Error:<%s>", m_statisticFilePath.c_str(), errorDetail.c_str());

					ACE_OS::close(fileStateDescriptor);
					fileStateDescriptor = ACE_INVALID_HANDLE;
					// remove the created file
					::remove(m_statisticFilePath.c_str());
				}
			}
		}
	}

	return fileStateDescriptor;
}

void RTR_statistics::checkFolders()
{
	ACS_RTR_TRACE_MESSAGE("In");

	size_t tagStartPos = m_statisticFilePath.find_last_of(DirDelim);

	// Check if the tag is present
	if( std::string::npos != tagStartPos )
	{
		std::string fullPath(m_statisticFilePath.substr(0, tagStartPos));
		ACS_RTR_TRACE_MESSAGE("Check path:<%s>", fullPath.c_str());

		int errorNum;
		if(!createDir(fullPath.c_str(), errorNum) )
		{
			// some error happens on map
			char errorMsg[64]={0};
			std::string errorDetail(strerror_r(errorNum, errorMsg, 63));

			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to create folders:<%s>. Error:<%s>", fullPath.c_str(), errorDetail.c_str());
			ACS_RTR_TRACE_MESSAGE("Error: Failed to create folders:<%s>. Error:<%s>", fullPath.c_str(), errorDetail.c_str());
		}
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to found tag:<'/'> into path:<%s>", m_statisticFilePath.c_str());
		ACS_RTR_TRACE_MESSAGE("Error: Failed to found tag:<'/'> into path:<%s>", m_statisticFilePath.c_str());
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}


bool RTR_statistics::mapToMemoryStatisticFile()
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	bool fileCreated;
	ACE_HANDLE fileDescriptor = openStatisticFile(fileCreated);

	if(ACE_INVALID_HANDLE != fileDescriptor )
	{
		ACS_RTR_TRACE_MESSAGE("map a shared memory of size:<%u>, file handle:<%d>", StatisticsFileSize, fileDescriptor);

		// Create the memory mapping of the data file, it will be shared with CDH server
		m_mappedMemoryAddr = ACE_OS::mmap( 0, StatisticsFileSize, PROT_RDWR, MAP_SHARED, fileDescriptor);

		// Close the file handle since it is not more needed
		ACE_OS::close(fileDescriptor);

		if( MAP_FAILED != m_mappedMemoryAddr )
		{
			result = true;
			// set the pointer to the mapped area
			posFileCreationTime = static_cast<char*>(m_mappedMemoryAddr);

			posKbACA = reinterpret_cast<int64_t*> (posFileCreationTime + SIZEOF_CHARBUFFER);
			posKbGOH = posKbACA + 1;

			posFilled = reinterpret_cast<uint32_t*>(posKbGOH + 1);
			posLost = posFilled + 1;
			posSkipped = posLost + 1;

			posConnMS = posSkipped + 1;
			posAttachGOH = posConnMS + 1;
			posLastComBno = posAttachGOH + 1;

			// check if first time
			if(fileCreated)
			{
				// set creation time and reset all values
				time_t rawTime = time(NULL);
				struct tm* currentTime = ::localtime(&rawTime);
				char creationTimeBuffer[21] = {0};

				snprintf(creationTimeBuffer, sizeof(creationTimeBuffer) - 1, "%02d/%02d/%d %02d:%02d:%02d",
							(currentTime->tm_mon+1), currentTime->tm_mday, (currentTime->tm_year + 1900),
							 currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);

				strcpy( posFileCreationTime, creationTimeBuffer);
				*posKbACA = 0;
				*posKbGOH = 0;
				*posFilled = 0U;
				*posLost = 0U;
				*posSkipped = 0U;
				*posConnMS = 0U;
				*posAttachGOH = 0U;
				*posLastComBno = 0U;
			}

			ACS_RTR_TRACE_MESSAGE("posFileCreationTime : %s",posFileCreationTime);
			ACS_RTR_TRACE_MESSAGE("posKbACA : %zu", *posKbACA);
			ACS_RTR_TRACE_MESSAGE("posKbGOH : %zu", *posKbGOH);
			ACS_RTR_TRACE_MESSAGE("posFilled : %d", *posFilled);
			ACS_RTR_TRACE_MESSAGE("posLost : %d",*posLost);
		}
		else
		{
			// some error happens on map
			char errorMsg[64]={0};
			std::string errorDetail(strerror_r(ACE_OS::last_error(), errorMsg, 63));

			ACS_RTR_LOG(LOG_LEVEL_ERROR, "failed to map statistic file:<%s>. Error:<%s>", m_statisticFilePath.c_str(), errorDetail.c_str());
			ACS_RTR_TRACE_MESSAGE("Error: failed to map statistic file:<%s>. Error:<%s>", m_statisticFilePath.c_str(), errorDetail.c_str());
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out,result:<%s>", (result ? "TRUE" : "FALSE") );
	return result;
}

void RTR_statistics::objectInitRecovery()
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	if(!m_initialized)
	{
		if(m_isCpObject)
		{
			cpStatisticsInfoInit(m_jobDN);
		}
		else
		{
			statisticsInfoInit(m_jobDN);
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}
