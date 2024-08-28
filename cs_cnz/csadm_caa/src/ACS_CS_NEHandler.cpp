//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <limits.h>
#include <iostream>

#include "ACS_CS_Event.h"
#include "ACS_CS_API_NeHandling.h"
#include "ACS_CS_EntryCounter.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_Timer.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_BasicResponse.h"
#include "ACS_CS_NE_GetAlarmMaster.h"
#include "ACS_CS_NE_GetAlarmMasterResponse.h"
#include "ACS_CS_NE_GetBSOMIPAddress.h"
#include "ACS_CS_NE_GetBSOMIPAddressResponse.h"
#include "ACS_CS_NE_GetClockMaster.h"
#include "ACS_CS_NE_GetClockMasterResponse.h"
#include "ACS_CS_NE_GetCPSystem.h"
#include "ACS_CS_NE_GetCPSystemResponse.h"
#include "ACS_CS_NE_GetNEId.h"
#include "ACS_CS_NE_GetNEIdResponse.h"
#include "ACS_CS_NE_GetTestEnvironment.h"
#include "ACS_CS_NE_GetTestEnvironmentResponse.h"
#include "ACS_CS_NE_GetClusterOpMode.h"
#include "ACS_CS_NE_GetClusterOpModeResponse.h"
#include "ACS_CS_NE_GetOmProfile.h"
#include "ACS_CS_NE_GetOmProfileResponse.h"
#include "ACS_CS_NE_GetTrafficLeader.h"
#include "ACS_CS_NE_GetTrafficLeaderResponse.h"
#include "ACS_CS_NE_GetTrafficIsolated.h"
#include "ACS_CS_NE_GetTrafficIsolatedResponse.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_Internal_Table.h"
#include "ACS_CS_Timer.h"
#include "ACS_CS_SubscriptionAgent.h"
#include "ACS_CS_TableOperation.h"
#include "ACS_CS_SM_NETableNotify.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImOmProfile.h"
#include "ACS_CS_ImCcFile.h"
#include "ACS_CS_BrfHandler.h"
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

// ACS_CS_NEHandler
#include "ACS_CS_NEHandler.h"
#include "ACS_CS_BrfHandler.h"


#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_NEHandler_TRACE);

using std::ifstream;
using std::string;
using std::vector;
using std::ostringstream;

using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using namespace ACS_CS_INTERNAL_API;

namespace fs = ::boost::filesystem;
#define BOOST_FILESYSTEM_NO_DEPRECATED

static const int TEMP_BUF_SIZE = 32; // temporary scratch buffer size

static void* s_initializationModeSwitchDelayThread(void *arg);

xmlPropertiesMap_t ACS_CS_NEHandler::m_xmlProperties;

const char * const NE_CLUSTEROPMODE_TIMER_ID  = "ClusterOpModeSupervisionTimer";
const char * const NE_PHASE_VALIDATE_TIMER_ID = "PhaseValidateTimer";
const char * const NE_PHASE_APNOTIFY_TIMER_ID = "PhaseApNotifyTimer";
const char * const NE_PHASE_CPNOTIFY_TIMER_ID = "PhaseCpNotifyTimer";
const char * const NE_PHASE_COMMIT_TIMER_ID   = "PhaseCommitTimer";

const  std::string ACS_CS_NEHandler::DTD_FILE_PATH = "/opt/ap/mcs/doc/CommandRules.dtd";

std::string ACS_CS_NEHandler::m_ccfilePath;

bool ACS_CS_NEHandler::IMMWriteFailure= false;

namespace
{
	const string BRF_PATH = "/usr/share/pso/storage-paths/";
	const string BRF_CONFIG_FILENAME = "config";
	const string BRF_CS_DIR = "acs_csbin";
	const string BRF_CCF_DIR = "ccf";

	const string CC_FILE_DIR("/opt/ap/xmlfile/");
	const string CC_FILENAME_BASE("MmlCommandRules_");
	const string CC_FILENAME_EXT(".xml");

	// A XMLString::transcode wrapper to guarantee transcoded memory allocation deallocation
	template<typename T1, typename T2>
	class XmlStringTranscode
	{
	public:
		explicit XmlStringTranscode(T1 const * const toTranscode)
		: m_transcode(XMLString::transcode(toTranscode))
		{
		}
		~XmlStringTranscode()
		{
			XMLString::release(&m_transcode);
		}
		operator T2 const *() const
					 {
			return m_transcode;
					 }
	private:
		XmlStringTranscode(XmlStringTranscode const & ); // Not to be implemented
		XmlStringTranscode & operator =(XmlStringTranscode const & ); // Not to be implemented
		T2 * m_transcode;
	};
	typedef XmlStringTranscode<char, XMLCh> CharToXMLCh;
	typedef XmlStringTranscode<XMLCh, char> XMLChToChar;

}


// Class ACS_CS_NEHandler 

ACS_CS_NEHandler::ACS_CS_NEHandler()
      : immDataSynchronized(true),
        table(0),
        cpCounter(0),
        lock(0),
        clusterOpModeSupervisionTimerHandle(ACS_CS_EVENT_RC_ERROR),
        profileHandling(NULL),
        agentInstance(NULL),
        m_xmlFile(),
        notificationsPending(false),
        subscriberCount(0), startupProfileSupervisionComplete(false), /*HW99445*/m_isRestartAfterRestore(false),m_apaAlarmText(""),m_apaAlarm(false),m_ScopeFlag(false)
{
    //table = new ACS_CS_Table();

    cpCounter = new ACS_CS_EntryCounter(); // CP counter
    lock = new ACS_CS_ReaderWriterLock();

    profileHandling = new ACS_CS_API_ProfileHandling();

    if (CS_ProtocolChecker::checkIfAP1())

    	setStartupProfileSupervision(false);

    // Create cluster timer
    clusterOpModeSupervisionTimerHandle = ACS_CS_Timer::CreateTimer(false, NE_CLUSTEROPMODE_TIMER_ID);

    if (clusterOpModeSupervisionTimerHandle == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
        	"(%t) %s \n Error: Could not create the cluster operation mode supervision timer handle", __FUNCTION__));
    }

    // Agent should always be created before handlers.
    agentInstance = ACS_CS_SubscriptionAgent::getInstance();

    ACS_CS_EventHandle validateHandle = -1;
    ACS_CS_EventHandle apNotifyHandle = -1;
    ACS_CS_EventHandle cpNotifyHandle = -1;
    ACS_CS_EventHandle commitHandle = -1;

    // Reset NE subscribers
    //ACS_CS_API_NeHandling::setNeSubscriberCount(0);

    if( (validateHandle = ACS_CS_Timer::CreateTimer(false, NE_PHASE_VALIDATE_TIMER_ID)) == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) %s \n Error: Could not create the Validate phase supervision timer handle", __FUNCTION__));
        omProfileSupervisionTimerHandle.push_back(ACS_CS_EVENT_RC_ERROR); // placeholder
    }
    else
    {
        omProfileSupervisionTimerHandle.push_back(validateHandle);
    }

    if( (apNotifyHandle = ACS_CS_Timer::CreateTimer(false, NE_PHASE_APNOTIFY_TIMER_ID)) == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) %s \n Error: Could not create the ApNotify phase supervision timer handle", __FUNCTION__));
        omProfileSupervisionTimerHandle.push_back(ACS_CS_EVENT_RC_ERROR); // placeholder
    }
    else
    {
        omProfileSupervisionTimerHandle.push_back(apNotifyHandle);
    }

    if( (cpNotifyHandle = ACS_CS_Timer::CreateTimer(false, NE_PHASE_CPNOTIFY_TIMER_ID)) == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) %s \n Error: Could not create the CpNotify phase supervision timer handle", __FUNCTION__));
        omProfileSupervisionTimerHandle.push_back(ACS_CS_EVENT_RC_ERROR); // placeholder
    }
    else
    {
        omProfileSupervisionTimerHandle.push_back(cpNotifyHandle);
    }

    if( (commitHandle = ACS_CS_Timer::CreateTimer(false, NE_PHASE_COMMIT_TIMER_ID)) == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) %s \n Error: Could not create the Commit phase supervision timer handle", __FUNCTION__));
        omProfileSupervisionTimerHandle.push_back(ACS_CS_EVENT_RC_ERROR); // placeholder
    }
    else
    {
        omProfileSupervisionTimerHandle.push_back(commitHandle);
    }
}


ACS_CS_NEHandler::~ACS_CS_NEHandler()
{
    delete cpCounter;
    delete lock;

    while (!omProfileSupervisionTimerHandle.empty())
    {
        ACS_CS_EventHandle tempHandle = omProfileSupervisionTimerHandle.back();
        omProfileSupervisionTimerHandle.pop_back();

        if (tempHandle != ACS_CS_EVENT_RC_ERROR)
        {
            ACS_CS_Timer::CloseTimer(tempHandle);
        }
    }

    if (clusterOpModeSupervisionTimerHandle != ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_Timer::CloseTimer(clusterOpModeSupervisionTimerHandle);
        clusterOpModeSupervisionTimerHandle = ACS_CS_EVENT_RC_ERROR;
    }

    if(profileHandling) {
    	delete profileHandling;
    }
}

int ACS_CS_NEHandler::handleRequest (ACS_CS_PDU *pdu)
{
   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleRequest()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   int error = 0;

   const ACS_CS_HeaderBase * header= pdu->getHeader(); // Get header from PDU

   if (header)
   {
      // save the primitive ID so that it can be used later
      const ACS_CS_Protocol::CS_Primitive_Identifier primitiveId =
         header->getPrimitiveId();

      // Check which primitive that has been received and call handler function
      if (primitiveId == Primitive_GetCPSystem)
      {
         lock->start_reading();
         error = handleGetCPSystem(pdu);  // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetNEId)
      {
         lock->start_reading();
         error = handleGetNEId(pdu);   // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetClusterOpMode)
      {
         lock->start_reading();
         error = handleGetClusterOpMode(pdu);   // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetTrafficLeader)
      {
         lock->start_reading();
         error = handleGetTrafficLeader(pdu);   // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetTrafficIsolated)
      {
         lock->start_reading();
         error = handleGetTrafficIsolated(pdu); // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetOmProfile)
      {
         lock->start_reading();
         error = handleGetOmProfile(pdu); // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetAlarmMaster)
      {
         lock->start_reading();
         error = handleGetAlarmMaster(pdu); // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetClockMaster)
      {
         lock->start_reading();
         error = handleGetClockMaster(pdu);  // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetBSOMIPAddress)
      {
         lock->start_reading();
         error = handleGetBSOMIPAddress(pdu);   // OK
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetTestEnvironment)
      {
         lock->start_reading();
         error = handleGetTestEnvironment(pdu);    // OK
         lock->stop_reading();
      }
      else
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::handleRequest()\n"
                  "Error: Unknown primitive, id = %d\n",
                  primitiveId));

         error = -1;
      }
   }

   return error;
}

bool ACS_CS_NEHandler::getBladeClusterInfoAndAdvancedConfiguration(const ACS_CS_ImCpCluster **bladeCluster, const ACS_CS_ImAdvancedConfiguration **advConf) const
{
	set<const ACS_CS_ImBase *> advancedSet;
	set<const ACS_CS_ImBase *> bladeClusterSet;

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();

	model->getObjects(advancedSet, ADVANCEDCONFIGURATION_T);
	if(advancedSet.size() == 0 || advancedSet.begin() == advancedSet.end())
		return false;


	const ACS_CS_ImAdvancedConfiguration *advancedConfiguration = dynamic_cast<const ACS_CS_ImAdvancedConfiguration *>(*(advancedSet.begin()));
	if(NULL == advancedConfiguration)
		return false;

	model->getObjects(bladeClusterSet, CPCLUSTER_T);
	if(bladeClusterSet.size() == 0 || bladeClusterSet.begin() == bladeClusterSet.end())
		return false;

	const ACS_CS_ImCpCluster *bladeClusterInfo = dynamic_cast<const ACS_CS_ImCpCluster *>(*(bladeClusterSet.begin()));
	if(NULL == bladeClusterInfo)
		return false;

	*bladeCluster = bladeClusterInfo;
	*advConf = advancedConfiguration;
	return true;
}

int ACS_CS_NEHandler::newTableOperationRequest(ACS_CS_ImModelSubset *subset)
{
	const ACS_CS_ImAdvancedConfiguration *advancedConfiguration;
	const ACS_CS_ImCpCluster *bladeClusterInfo;

	if(subset->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str()) == NULL &&
		subset->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str()) == NULL)
	{
		return false;
	}

	if(!getBladeClusterInfoAndAdvancedConfiguration(&bladeClusterInfo, &advancedConfiguration))
			return 0;

	//TODO: Uncomment when profile change works
//	int result = handleSetProfiles(bladeClusterInfo, advancedConfiguration);
//	if(Result_Other_Failure != result)
//	{
//		return result;
//	}

	handleNeTableSubscription();

	// Handle update phase change request
	if (true == advancedConfiguration->initiateUpdatePhaseChange)
	{
		ACS_CS_API_OmProfileChange newProfile;

		newProfile.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue>(advancedConfiguration->omProfilePhase);
		newProfile.omProfileCurrent = static_cast<uint32_t>(advancedConfiguration->omProfileCurrent);

		newProfile.apzCurrent = static_cast<uint32_t>(advancedConfiguration->apzProfileCurrent);
		newProfile.aptCurrent = static_cast<uint32_t>(advancedConfiguration->aptProfileCurrent);
		newProfile.omProfileRequested = static_cast<uint32_t>(advancedConfiguration->omProfileRequested);
		newProfile.apzRequested = static_cast<uint32_t>(advancedConfiguration->apzProfileRequested);
		newProfile.aptRequested = static_cast<uint32_t>(advancedConfiguration->aptProfileRequested);
		newProfile.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue>(advancedConfiguration->omProfileChangeTrigger);
		newProfile.apzQueued = static_cast<uint32_t>(advancedConfiguration->apzProfileQueued);
		newProfile.aptQueued = static_cast<uint32_t>(advancedConfiguration->aptProfileQueued);

		agentInstance->updatePhaseChange(newProfile);
	}
//        }
//
//        if (entries)
//            delete [] entries;
//    }

    return 0;
}

bool ACS_CS_NEHandler::loadTable ()
{
   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::loadTable()\n"
            "Entering method\n"));

   if ( ! table )
      return false;

   ACS_CS_API_TableLoader *tableLoader = new ACS_CS_API_TableLoader();

   table->clearTable();

   if (tableLoader->loadNETable(*table) == ACS_CS_API_NS::Result_Success)
   {
       ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::loadTable()\n"
               "Loaded %d table entries", table->getSize()));

       delete tableLoader;
       return true;
   }

   if (tableLoader) {
       delete tableLoader;
   }

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
           "(%t) ACS_CS_NEHandler::loadTable()\n"
           "Error: Failed to load the NE table\n"));

   return false;
}

/*int ACS_CS_NEHandler::updateTableAttributes (const uint16_t entryId, const std::vector<ACS_CS_Attribute*> attributes)
{
    ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::updateTableAttributes()\n"
            "Entering method with entry id: %d, attribute size: %d", entryId, attributes.size()));

    if (!table->containsEntry(entryId)) {
        // could not find the board id in the table
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                "(%t) ACS_CS_NEHandler::updateTableAttributes()\n"
                "Could not find entry %d in the NETable", entryId));
        return -1;
    }

    for (size_t i = 0;i < attributes.size(); i++)
    {
        if (table->setValue(entryId, *attributes[i]) != 0) {
            // failed to update attribute
            return -1;
        }
    }

    return 0;
}
*/


int ACS_CS_NEHandler::handleGetCPSystem (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetCPSystem()\n"
            "Entering method\n"));

   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetCPSystem()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
	   header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetCPSystem()\n"
               "Error: Invalid header or body\n"));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
   
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() != Primitive_GetCPSystem)
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetCPSystem()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));

      return -1;
   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header();
   ACS_CS_NE_GetCPSystemResponse * responseBody = new ACS_CS_NE_GetCPSystemResponse();

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return -1;
   }

   unsigned short requestId = header->getRequestId();           // Get request id

   bool isMultipleCPSystem = ACS_CS_Registry::isMultipleCPSystem();
   ACS_CS_Protocol::CS_CP_System_Identifier cpSystem = ACS_CS_Protocol::System_NotSpecified;

   if (isMultipleCPSystem)
      cpSystem = ACS_CS_Protocol::System_Multiple_CP;
   else
      cpSystem = ACS_CS_Protocol::System_Single_CP;

   // Set header values
   responseHeader->setVersion(version);
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetCPSystemResponse);
   responseHeader->setRequestId(requestId);

   // Set body values
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
   responseBody->setCPSystem(cpSystem);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::handleGetNEId (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetNEId()\n"
            "Entering method\n"));

   if (pdu == 0)   // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetNEId()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   
   if (pdu && pdu->getHeader())
	   header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_NE_GetNEId * getNEId = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetNEId()\n"
               "Error: Invalid header or primitive, header = %d, primitive = %d\n",
               header,
               primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetNEId)
   {
      getNEId = reinterpret_cast<const ACS_CS_NE_GetNEId *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetNEId()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();          // Get request id

   int error = -1;

   ACS_CS_API_Name neid;
   ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getNEID(neid);

   if (returnValue == ACS_CS_API_NS::Result_NoValue)
   {
      error = createBasicResponse(pdu,
            ACS_CS_Protocol::Primitive_GetNEIdResponse,
            requestId,
            ACS_CS_Protocol::Result_No_Value);

      return error;

   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header();    // Create header
   ACS_CS_NE_GetNEIdResponse * responseBody = new ACS_CS_NE_GetNEIdResponse();// Create body

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return -1;
   }

   responseHeader->setVersion(version);                           // and set all values
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetNEIdResponse);
   responseHeader->setRequestId(requestId);
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);

   char *name = new char[256];
   size_t len = 0;

   ACS_CS_API_NS::CS_API_Result result = neid.getName(name, len);
   result = neid.getName(name, len);

   responseBody->setNEId(name);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());
   delete [] name;

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::handleGetAlarmMaster (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetAlarmMaster()\n"
            "Entering method\n"));

   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetAlarmMaster()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   
   if (pdu && pdu->getHeader())
   	   header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_NE_GetAlarmMaster * getAlarmMaster = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetAlarmMaster()\n"
               "Error: Invalid header or primitive, header = %d, primitive = %d\n",
               header,
               primitive));

      return -1;
   }
   
   // Get version from incoming package
   version = header->getVersion();

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetAlarmMaster)
   {
      getAlarmMaster = reinterpret_cast<const ACS_CS_NE_GetAlarmMaster *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetAlarmMaster()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();           // Get request id
   int error = -1;

   ACS_CS_Header * responseHeader = new ACS_CS_Header();    // Create header
   ACS_CS_NE_GetAlarmMasterResponse * responseBody = new ACS_CS_NE_GetAlarmMasterResponse();// Create body

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      delete responseHeader;
      delete responseBody;
      return -1;
   }

   responseHeader->setVersion(version);                           // and set all values
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetAlarmMasterResponse);
   responseHeader->setRequestId(requestId);

   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);

   unsigned short master = USHRT_MAX;

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();

   ACS_CS_ImCpCluster *info = reinterpret_cast<ACS_CS_ImCpCluster *>(model.getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str()));

   if(info != NULL)
   {
	   master = info->alarmMaster;
   }
   else
   {
	   ACS_CS_ImAdvancedConfiguration *advInfo = reinterpret_cast<ACS_CS_ImAdvancedConfiguration *>(model.getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str()));

	   if(advInfo != NULL)
	   {
		   master = advInfo->alarmMaster;
	   }
   }

   if (master == USHRT_MAX)
   {
      error = createBasicResponse(pdu,
            ACS_CS_Protocol::Primitive_GetAlarmMasterResponse,
            requestId, ACS_CS_Protocol::Result_No_Such_Entry);

      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return error;
   }

   responseBody->setAlarmMaster(master);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::handleGetClockMaster (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetClockMaster()\n"
            "Entering method\n"));

   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetClockMaster()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
   	   header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_NE_GetClockMaster * getclockMaster = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetClockMaster()\n"
               "Error: Invalid header or primitive, header = %d, primitive = %d\n",
               header,
               primitive));

      return -1;
   }
   
   // Get version from incoming package
   version = header->getVersion();
   
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetClockMaster)
   {
      getclockMaster = reinterpret_cast<const ACS_CS_NE_GetClockMaster *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetClockMaster()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));

      return -1;
   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header();    // Create header
   ACS_CS_NE_GetClockMasterResponse * responseBody = new ACS_CS_NE_GetClockMasterResponse();        // Create body

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return -1;
   }

   unsigned short requestId = header->getRequestId();           // Get request id
   int error = -1;

   responseHeader->setVersion(version);                           // and set all values
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetClockMasterResponse);
   responseHeader->setRequestId(requestId);

   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);

   unsigned short master = USHRT_MAX;

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();

   ACS_CS_ImCpCluster *info = reinterpret_cast<ACS_CS_ImCpCluster *>(model.getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str()));

   if(info != NULL)
   {
	   master = info->clockMaster;
   }
   else
   {
	   ACS_CS_ImAdvancedConfiguration *advInfo = reinterpret_cast<ACS_CS_ImAdvancedConfiguration *>(model.getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str()));

	   if(advInfo != NULL)
	   {
		   master = advInfo->clockMaster;
	   }
   }

   if (master == USHRT_MAX)
   {
      error = createBasicResponse(pdu,
            ACS_CS_Protocol::Primitive_GetClockMasterResponse,
            requestId, ACS_CS_Protocol::Result_No_Such_Entry);

      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return error;
   }

   responseBody->setClockMaster(master);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::handleGetBSOMIPAddress (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetBSOMIPAddress()\n"
            "Entering method\n"));

   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetBSOMIPAddress()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetBSOMIPAddress()\n"
               "Error: Invalid header or body\n"));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
   
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() != Primitive_GetBSOMIPAddress)
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetBSOMIPAddress()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));

      return -1;
   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header(); // Create header
   ACS_CS_NE_GetBSOMIPAddressResponse * responseBody = new ACS_CS_NE_GetBSOMIPAddressResponse();// Create body

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return -1;
   }

   unsigned short requestId = header->getRequestId();           // Get request id

   // Set header values
   responseHeader->setVersion(version);
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetBSOMIPAddressResponse);
   responseHeader->setRequestId(requestId);

   // Set body values
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
   responseBody->setBSOMIPAddress(BSOMIPAddressEthA, BSOMIPAddressEthB);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::handleGetTestEnvironment (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetTestEnvironment()\n"
            "Entering method\n"));

   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTestEnvironment()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTestEnvironment()\n"
               "Error: Invalid header or body\n"));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() != Primitive_GetTestEnvironment)
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTestEnvironment()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));

      return -1;
   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header();
   ACS_CS_NE_GetTestEnvironmentResponse * responseBody = new ACS_CS_NE_GetTestEnvironmentResponse();

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return -1;
   }

   unsigned short requestId = header->getRequestId();           // Get request id

   bool isTestEnvironment = ACS_CS_Registry::isTestEnvironment();
   ACS_CS_Protocol::CS_TestEnvironment_Identifier testEnvironment
      = ACS_CS_Protocol::TestEnvironment_NotSpecified;

   if (isTestEnvironment)
      testEnvironment = ACS_CS_Protocol::TestEnvironment_TRUE;
   else
      testEnvironment = ACS_CS_Protocol::TestEnvironment_FALSE;

   // Set header values
   responseHeader->setVersion(version);
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetTestEnvironmentResponse);
   responseHeader->setRequestId(requestId);

   // Set body values
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
   responseBody->setTestEnvironment(testEnvironment);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, unsigned short requestId, ACS_CS_Protocol::CS_Result_Code result)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   // and freed there
   //lint --e{429}

   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   const ACS_CS_Header * oldHeader = 0;
   if (pdu && pdu->getHeader())
	   oldHeader = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   // Get version from incoming package
   if (oldHeader)
   {
	   version = oldHeader->getVersion();
   }
		
   ACS_CS_Header * header = new ACS_CS_Header();    // Create header

   
   if (header == 0)
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
               "Error: Invalid header\n"));

      return -1;
   }

   ACS_CS_BasicResponse * body = 0;

   // Create body
   if (type == ACS_CS_Protocol::Primitive_GetTestEnvironmentResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetTestEnvironmentResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetCPSystemResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetCPSystemResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetNEIdResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetNEIdResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetClusterOpModeResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetClusterOpModeResponse();
   }


   else if (type == ACS_CS_Protocol::Primitive_GetTrafficLeaderResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetTrafficLeaderResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetTrafficIsolatedResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetTrafficIsolatedResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetOmProfileResponse)
   {
      body = new ACS_CS_NE_GetOmProfileResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetAlarmMasterResponse)
   {
      // This function can only be used fo error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetAlarmMasterResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetClockMasterResponse)
   {
      // This function can only be used fo error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetClockMasterResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetBSOMIPAddressResponse)
   {
      // This function can only be used fo error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                  "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_NE_GetBSOMIPAddressResponse();
   }

   if (body == 0)
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::createBasicResponse()\n"
               "Error: Invalid body\n"));

      delete header;
      return -1;
   }

   // Set all header values
   header->setVersion(version);
   header->setScope(Scope_NE);
   header->setPrimitive(type);
   header->setRequestId(requestId);

   // Set resultcode
   if (body)
	   body->setResultCode(result);

   // Set length
   header->setTotalLength(header->getHeaderLength() + 2);

   // Update PDU
   pdu->setHeader(header);
   pdu->setPrimitive(body);

   return 0;
}

int ACS_CS_NEHandler::handleGetClusterOpMode (ACS_CS_PDU *pdu)
{
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetClusterOpMode()\n"
            "Entering method\n"));

   if (pdu == 0)   // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetClusterOpMode()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_NE_GetClusterOpMode * getClusterOpMode = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetClusterOpMode()\n"
               "Error: Invalid header or primitive, header = %d, primitive = %d\n",
               header,
               primitive));

      return -1;
   }
   
   // Get version from incoming package
   version = header->getVersion();

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetClusterOpMode)
   {
      getClusterOpMode = reinterpret_cast<const ACS_CS_NE_GetClusterOpMode *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetClusterOpMode()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));
      return -1;
   }

   unsigned short requestId = header->getRequestId();          // Get request id
   ACS_CS_API_ClusterOpMode::Value clusterOpMode = ACS_CS_API_ClusterOpMode::Normal;

   if (!readClusterOpMode(clusterOpMode))
   {
      int error = createBasicResponse(pdu,
               ACS_CS_Protocol::Primitive_GetClusterOpModeResponse,
               requestId,
               ACS_CS_Protocol::Result_Busy);

      return error;
   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header();   // Create header
   ACS_CS_NE_GetClusterOpModeResponse *responseBody = new ACS_CS_NE_GetClusterOpModeResponse();// Create body

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      delete responseHeader;
      delete responseBody;
      return -1;
   }

   responseHeader->setVersion(version);                                                       // and set all values
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetClusterOpModeResponse);
   responseHeader->setRequestId(requestId);

   responseBody->setClusterOpMode(clusterOpMode);
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}


int ACS_CS_NEHandler::handleGetOmProfile (ACS_CS_PDU *pdu)
{
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetOmProfile()\n"
            "Entering method\n"));

   if (pdu == 0)   // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetOmProfile()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * const primitive = pdu->getPrimitive();
   const ACS_CS_NE_GetOmProfile * getOmProfile = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	
   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetOmProfile()\n"
               "Error: Invalid header or primitive, header = %d, primitive = %d\n",
               header,
               primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() != Primitive_GetOmProfile)
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetOmProfile()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));
      return -1;

   }

   getOmProfile = reinterpret_cast<const ACS_CS_NE_GetOmProfile *> (primitive);

   const unsigned short requestId = header->getRequestId();          // Get request id

   ACS_CS_API_OmProfileChange currentOmProfile;

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();

   const bool success = ACS_CS_API_Util::readOmProfile(&model, currentOmProfile);

   if( !success )
   {
      const int error = createBasicResponse(pdu,
            ACS_CS_Protocol::Primitive_GetOmProfileResponse,
            requestId,
            ACS_CS_Protocol::Result_No_Value);
      return error;
   }

   ACS_CS_Header * const responseHeader = new ACS_CS_Header();
   ACS_CS_NE_GetOmProfileResponse * const responseBody = new ACS_CS_NE_GetOmProfileResponse();

   if ( (responseHeader == NULL) || (responseBody == NULL) )
   {
      delete responseHeader;
      delete responseBody;

      return -1;
   }

   responseHeader->setVersion(version);                          // and set all values
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetOmProfileResponse);
   responseHeader->setRequestId(requestId);
   responseBody->setOmProfile(currentOmProfile);

   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::handleGetTrafficIsolated (ACS_CS_PDU *pdu)
{
   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetTrafficIsolated()\n"
            "Entering method\n"));

   if (pdu == 0)    // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTrafficIsolated()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header       = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_NE_GetTrafficIsolated * getTrafficIsolated = 0;

   // Check for valid header and body
   if ((header == 0) || (primitive == 0))
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTrafficIsolated()\n"
               "Error: Invalid header or primitive, header = %d, primitive = %d\n",
               header, primitive));

      return -1;
   }
	
   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetTrafficIsolated)
   {
      getTrafficIsolated = reinterpret_cast<const ACS_CS_NE_GetTrafficIsolated *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTrafficIsolated()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();           // Get request id
   int error = -1;


   // Create header
   ACS_CS_Header * responseHeader = new ACS_CS_Header();

   // Create body
   ACS_CS_NE_GetTrafficIsolatedResponse * responseBody = new ACS_CS_NE_GetTrafficIsolatedResponse();

   if ((responseHeader == 0) || (responseBody == 0))
   {
      // check if a value is non NULL
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return -1;
   }

   responseHeader->setVersion(version);                           // and set all values
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetTrafficIsolatedResponse);
   responseHeader->setRequestId(requestId);

   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();

   ACS_CS_ImAdvancedConfiguration *config = reinterpret_cast<ACS_CS_ImAdvancedConfiguration *>(model.getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str()));

   if (config == NULL)
   {
      error = createBasicResponse(pdu,
            ACS_CS_Protocol::Primitive_GetTrafficIsolatedResponse,
            requestId, ACS_CS_Protocol::Result_No_Such_Entry);

      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return error;
   }


   responseBody->setTrafficIsolated(config->trafficIsolatedCp);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

int ACS_CS_NEHandler::handleGetTrafficLeader (ACS_CS_PDU *pdu)
{
   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) ACS_CS_NEHandler::handleGetTrafficLeader()\n"
            "Entering method\n"));

   if (pdu == 0)   // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTrafficLeader()\n"
               "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header       = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_NE_GetTrafficLeader * getTrafficLeader = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTrafficLeader()\n"
               "Error: Invalid header or primitive, header = %d, primitive = %d\n",
               header,
               primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetTrafficLeader)
   {
      getTrafficLeader = reinterpret_cast<const ACS_CS_NE_GetTrafficLeader *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) ACS_CS_NEHandler::handleGetTrafficLeader()\n"
               "Error: Wrong primitive type, type = %d\n",
               primitive->getPrimitiveType()));
      return -1;
   }

   unsigned short requestId = header->getRequestId();          // Get request id

   // Create header
   ACS_CS_Header * responseHeader = new ACS_CS_Header();

   // Create body
   ACS_CS_NE_GetTrafficLeaderResponse * responseBody = new ACS_CS_NE_GetTrafficLeaderResponse();

   if ((responseHeader == 0) || (responseBody == 0))
   {
      // check if a value is non NULL
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return -1;
   }

   responseHeader->setVersion(version);                           // and set all values
   responseHeader->setScope(Scope_NE);
   responseHeader->setPrimitive(Primitive_GetTrafficLeaderResponse);
   responseHeader->setRequestId(requestId);

   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();

   ACS_CS_ImAdvancedConfiguration *config = reinterpret_cast<ACS_CS_ImAdvancedConfiguration *>(model.getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str()));

   int error = -1;
   if (config == NULL)
   {
      error = createBasicResponse(pdu,
            ACS_CS_Protocol::Primitive_GetTrafficLeaderResponse,
            requestId,
            ACS_CS_Protocol::Result_No_Such_Entry);

      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      return error;

   }

   responseBody->setTrafficLeader(config->trafficLeaderCp);
   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

   // Update PDU
   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;
}

ACS_CS_Protocol::CS_Result_Code ACS_CS_NEHandler::handleSetLocalProfile(int newOmProfile, int newApzProfile, int newAptProfile)
{
   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) %s \n Entering method", __FUNCTION__));

   if (!CS_ProtocolChecker::checkIfAP1())
   {
      // handleSetLocalProfile drive phase changes and should only execute
      // on the MASTER.  For replication purposes, handleInternalSetOmProfile
      // will execute on SLAVEs in response to this method executing on MASTER.

      // Just return basic response with success.
      return ACS_CS_Protocol::Result_Success;
   }

   // check to make sure we are out of startup supervision.
   if (!startupProfileSupervisionComplete)
   {
      // This can happen when CS is restarted in Validate Phase after sending
      // notifications to ADH, ADH can respond when CS is in startup supervision.
      // We can safely ignore these requests since CS will send out new
      // notifications anyway.
      ostringstream eventData;
      eventData << "setLocalProfiles received even before supervision is complete."
         << " So ignoring this request.";

      ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
         "OaM Profile Switch",
         eventData.str(),
         "");

      ACS_CS_EventReporter::instance()->resetAllEvents();
      return ACS_CS_Protocol::Result_Success;
   }

   ACS_CS_API_OmProfileChange omProfileChange;

   // get current profile values from table.
   const ACS_CS_ImAdvancedConfiguration *advancedConfiguration;
	const ACS_CS_ImCpCluster *bladeClusterInfo;

	if(!getBladeClusterInfoAndAdvancedConfiguration(&bladeClusterInfo, &advancedConfiguration))
		return ACS_CS_Protocol::Result_Other_Failure;

	ACS_CS_API_OmProfileChange profileChangeData;
	profileChangeData.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue>(advancedConfiguration->omProfilePhase);
	profileChangeData.omProfileCurrent = static_cast<uint32_t>(advancedConfiguration->omProfileCurrent);

	profileChangeData.apzCurrent = static_cast<uint32_t>(advancedConfiguration->apzProfileCurrent);
	profileChangeData.aptCurrent = static_cast<uint32_t>(advancedConfiguration->aptProfileCurrent);
	profileChangeData.omProfileRequested = static_cast<uint32_t>(advancedConfiguration->omProfileRequested);
	profileChangeData.apzRequested = static_cast<uint32_t>(advancedConfiguration->apzProfileRequested);
	profileChangeData.aptRequested = static_cast<uint32_t>(advancedConfiguration->aptProfileRequested);
	profileChangeData.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue>(advancedConfiguration->omProfileChangeTrigger);
	profileChangeData.apzQueued = static_cast<uint32_t>(advancedConfiguration->apzProfileQueued);
	profileChangeData.aptQueued = static_cast<uint32_t>(advancedConfiguration->aptProfileQueued);

   // verify we are currently in Validate Phase
   if (omProfileChange.phase != ACS_CS_API_OmProfilePhase::Validate)
   {
      // We are not in validate phase.  Return an error to the
      // application, and rollback.

      ostringstream eventData;
      eventData << "setLocalProfiles received in unexpected phase "
         << omProfileChange.phase << ". Rolling back OaM Profile phase change."
         << "\n\n" << profileHandling->convertOmProfileToString(omProfileChange);

      ACS_CS_EVENT(Event_ProfileChangeFailure, ACS_CS_EventReporter::Severity_Event,
         "OaM Profile Switch",
         eventData.str(),
         "");

      ACS_CS_EventReporter::instance()->resetAllEvents();

      // Initate a rollback
      profileHandling->rollbackOmProfile();

      return ACS_CS_Protocol::Result_Other_Failure;

   }
   else if (omProfileChange.omProfileRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile &&
            (omProfileChange.aptRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile &&
             omProfileChange.apzRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile))
   {
      // Either omProfileRequested or both aptRequested and apzRequested should be unspecified.
      // If not, that implies setLocalProfiles was already called once in phase 0.
      // Duplicate calls can be ignored safely if old and new values are being the same
      // and if they differ it is considered as an error...rollback!!

	   int omProfileRequested, aptRequested, apzRequested = 0;
	   omProfileRequested = omProfileChange.omProfileRequested;
	   aptRequested = omProfileChange.aptRequested;
	   apzRequested = omProfileChange.apzRequested;

      if (omProfileRequested != newOmProfile ||
         aptRequested != newAptProfile ||
         apzRequested != newApzProfile)
      {
         ostringstream eventData;
         eventData << "Received unexpected call to setLocalProfiles in Notification phase 0."
                  << " Provided profiles differ from the previously recevied."
                  << " Rolling back OaM Profile phase change.\n"
                  << profileHandling->convertOmProfileToString(omProfileChange);

         if( omProfileChange.changeReason == ACS_CS_API_OmProfileChange::AutomaticProfileAlignment )
         {
        	 ACS_CS_EVENT(Event_ProfileChangeFailure,
        			 ACS_CS_EventReporter::Severity_A1,
					 "OaM PROFILE NOT ALIGNED IN AP AND CP",
					 eventData.str(),
					 "");
        	 /*HW99445 start*/
        	 m_apaAlarmText = eventData.str();
        	 if(writeAPAalarmdata())
        	 {
        		 m_apaAlarm = true;
        		 ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_TRACE, "[%s@%d] Successfully written data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
        	 }
        	 else
        	 {
        		 ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_ERROR, "[%s@%d] Failed to write data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
        	 }
        	 /*HW99445 start*/
         }
         else
         {
            ACS_CS_EVENT(Event_ProfileChangeFailure, ACS_CS_EventReporter::Severity_Event,
                        "OaM Profile Switch",
                        eventData.str(),
                        "");
         }

         ACS_CS_EventReporter::instance()->resetAllEvents();

         // Initiate a rollback!!
         profileHandling->rollbackOmProfile();

         return ACS_CS_Protocol::Result_Other_Failure;
      }
   }

   // update profile values with new requested values.
   omProfileChange.omProfileRequested = newOmProfile;
   omProfileChange.aptRequested = newAptProfile;
   omProfileChange.apzRequested = newApzProfile;

   // store updated profile values back to table.
   lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
   if (!ACS_CS_API_Util::writeOmProfile(omProfileChange))
   {
      // Failed to store updated profile values.
      return ACS_CS_Protocol::Result_Other_Failure;
   }
   lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
   ostringstream eventData;
   eventData << "Local Profile values updated." << "\n\n"
      << profileHandling->convertOmProfileToString(omProfileChange);

   ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
      "OaM Profile Switch",
      eventData.str(),
      "");

   ACS_CS_EventReporter::instance()->resetAllEvents();

   return ACS_CS_Protocol::Result_Success;
}

int ACS_CS_NEHandler::handleSetProfiles(const ACS_CS_ImCpCluster * /*bladeCluster*/, const ACS_CS_ImAdvancedConfiguration *advConf)
{
	ACS_CS_Protocol::CS_Result_Code resultCode = ACS_CS_Protocol::Result_Other_Failure;

    if (advConf->omProfileChangeTrigger == AUTOMATIC)
        return 0;

    int unspecifiedProfile = 0;
    unspecifiedProfile = ACS_CS_API_OmProfileChange::UnspecifiedProfile;

    if (advConf->omProfileRequested != unspecifiedProfile &&
    	advConf->aptProfileRequested == unspecifiedProfile &&
    	advConf->apzProfileRequested == unspecifiedProfile)
    {
       // Only oamProfile was provided (by nech)
       resultCode = handleSetOmProfile(advConf->omProfileRequested, advConf->omProfileChangeTrigger);
    }
    else if (advConf->omProfileRequested == unspecifiedProfile &&
    		advConf->aptProfileRequested != unspecifiedProfile &&
    		advConf->apzProfileRequested != unspecifiedProfile)
    {

       // only aptProfile and apzProfile were provided (by APSESH via setQuorumData)
       resultCode = handleSetApaProfile(advConf->apzProfileRequested, advConf->aptProfileRequested);
    }
    else if (advConf->omProfileRequested != unspecifiedProfile &&
    		advConf->aptProfileRequested != unspecifiedProfile &&
    		advConf->apzProfileRequested != unspecifiedProfile)
    {
       // oamProfile, aptProfile and apzProfile were all provided (by ADH via setLocalProfiles)
       resultCode = handleSetLocalProfile(advConf->omProfileRequested, advConf->apzProfileRequested, advConf->aptProfileRequested);
    }
    else
    {
       // oamProfile and only one of apzProfile or aptProfile was provided...not valid!!
       resultCode = ACS_CS_Protocol::Result_Other_Failure;
    }

    return 0;
}

int ACS_CS_NEHandler::handleSetClusterOpModeChanged (ACS_CS_API_ClusterOpMode::Value clusterOpMode, ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType)
{

	ACS_CS_API_ClusterOpMode::Value newClusterOpMode = clusterOpMode;

	if (opType == ACS_CS_API_NE_NS::CLUSTER_OP_MODE_REQUEST)
	{
		if (clusterOpMode == ACS_CS_API_ClusterOpMode::SwitchingToNormal)
		{
			if(agentInstance->getTableSubscriberCount(ACS_CS_Protocol::Scope_NE))
			{
				setClusterOpModeSupervisionTimer(CLUSTEROPMODE_TIMEOUT_IN_SECONDS);
				newClusterOpMode = ACS_CS_API_ClusterOpMode::SwitchingToNormal;
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
						"ACS_CS_NEHandler::handleSetClusterOpModeChanged()\n"
						"No subscribers, switching directly to Normal\n"));

				newClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
			}
		}
		else if (clusterOpMode == ACS_CS_API_ClusterOpMode::SwitchingToExpert)
		{
			if(agentInstance->getTableSubscriberCount(ACS_CS_Protocol::Scope_NE))
			{
				setClusterOpModeSupervisionTimer(CLUSTEROPMODE_TIMEOUT_IN_SECONDS);
				newClusterOpMode = ACS_CS_API_ClusterOpMode::SwitchingToExpert;
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
						"ACS_CS_NEHandler::handleSetClusterOpModeChanged()\n"
						"No subscribers, switching directly to Expert\n"));

				newClusterOpMode = ACS_CS_API_ClusterOpMode::Expert;
			}
		}

		//CpCluster -> ClusterOpMode
		lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
		ACS_CS_API_Util::writeClusterOpMode(newClusterOpMode, opType, RUNNING, CHANGE_OP_MODE);
		lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
	}

	return 0;
}

bool ACS_CS_NEHandler::setClusterOpModeSupervisionTimer (int timeout)
{
	if (clusterOpModeSupervisionTimerHandle == ACS_CS_EVENT_RC_ERROR &&
        (clusterOpModeSupervisionTimerHandle = ACS_CS_Timer::CreateTimer(false, NE_CLUSTEROPMODE_TIMER_ID)) == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
        	"(%t) %s \n Error: Could not create the cluster operation mode supervision timer handle", __FUNCTION__));
        return false;
    }

    return ACS_CS_Timer::SetTimer(clusterOpModeSupervisionTimerHandle, timeout);
}

bool ACS_CS_NEHandler::cancelClusterOpModeSupervisionTimer ()
{
    if(clusterOpModeSupervisionTimerHandle == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
        	"(%t) %s \n Do not have the cluster operation mode supervision timer handle", __FUNCTION__));

        return false;
    }

    return ACS_CS_Timer::CancelTimer(clusterOpModeSupervisionTimerHandle);
}

int ACS_CS_NEHandler::handleClusterOpModeSupervisionTimerExpired ()
{
	//guard to exclusive access
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_timer(_profile_mutex);

	ACS_CS_API_ClusterOpMode::Value oldClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
    ACS_CS_API_ClusterOpMode::Value newClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;

   	getClusterOpMode(oldClusterOpMode);

    switch(oldClusterOpMode)
    {
        case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
            newClusterOpMode = ACS_CS_API_ClusterOpMode::Expert;
            break;

        case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
            newClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
            break;

        default:
        	ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
        			"(%t) %s \n Error: Invalid cluster operation mode %d",
        			__FUNCTION__,
        			static_cast<int>(oldClusterOpMode)));
        	return -1;
    }

    if (oldClusterOpMode != newClusterOpMode)
    {
    	lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
        ACS_CS_API_Util::writeClusterOpMode(newClusterOpMode,ACS_CS_API_NE_NS::CLUSTER_OP_MODE_REQUEST,FINISHED,CHANGE_OP_MODE,FAILURE,"Supervision Timer Expired");
        lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
    }

    return 0;
}

bool ACS_CS_NEHandler::setOmProfileSupervisionTimer (ACS_CS_API_OmProfilePhase::PhaseValue phase, int timeout)
{
	//Safe check
    if (phase < ACS_CS_API_OmProfilePhase::Validate || phase > ACS_CS_API_OmProfilePhase::Commit)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                "(%t) %s \n Error: Phase timer %d out of valid range",
                __FUNCTION__,
                static_cast<int>(phase)));

        // Note that this isn't exactly an error, as doing a
        // cancel on timer 0 ( idle phase) will just return false
        return false;
    }

    { // Log
    	char trace[256] = {0};
    	snprintf(trace, sizeof (trace) - 1, "[%s::%s@%d] Setting %d seconds timeout for phase: %d, ", __FILE__, __FUNCTION__, __LINE__, timeout, static_cast<int>(phase));
    	ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
    	std::cout << "DBG: " << trace << std::endl;
    }

    if (omProfileSupervisionTimerHandle[phase] != ACS_CS_EVENT_RC_ERROR)
    {
        const char *phaseTimerId = NULL;
        switch (phase)
        {
            case ACS_CS_API_OmProfilePhase::Validate:
                phaseTimerId = NE_PHASE_VALIDATE_TIMER_ID;
                break;

            case ACS_CS_API_OmProfilePhase::ApNotify:
                phaseTimerId = NE_PHASE_APNOTIFY_TIMER_ID;
                break;

            case ACS_CS_API_OmProfilePhase::CpNotify:
                phaseTimerId = NE_PHASE_CPNOTIFY_TIMER_ID;
                break;

            case ACS_CS_API_OmProfilePhase::Commit:
                phaseTimerId = NE_PHASE_COMMIT_TIMER_ID;
                break;

            default:
                // We should never reach here, as the phase value is already checked.
                break;
        }
    }

    return ACS_CS_Timer::SetTimer(omProfileSupervisionTimerHandle[phase], timeout);
}

bool ACS_CS_NEHandler::cancelOmProfileSupervisionTimer (ACS_CS_API_OmProfilePhase::PhaseValue phase)
{
    if (phase < ACS_CS_API_OmProfilePhase::Validate || phase > ACS_CS_API_OmProfilePhase::Commit)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                "(%t) %s \n Error: Phase timer %d out of valid range",
                __FUNCTION__,
                static_cast<int>(phase)));
        // Note that this isn't exactly an error, as doing a
        // cancel on timer 0 ( idle phase) will just return false
        return false;
    }

    if(omProfileSupervisionTimerHandle[phase] == ACS_CS_EVENT_RC_ERROR)
    {
        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                "(%t) %s \n Do not have the OmProfilePhase supervision timer handle", __FUNCTION__));
        return false;
    }

    return ACS_CS_Timer::CancelTimer(omProfileSupervisionTimerHandle[phase]);
}

bool ACS_CS_NEHandler::rollbackOmProfile(ACS_CS_API_OmProfileChange &omProfileChange)
{
	ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - In", __FUNCTION__, __LINE__));
	ostringstream eventData;

	switch (omProfileChange.phase)
	{
	case ACS_CS_API_OmProfilePhase::Validate:
	case ACS_CS_API_OmProfilePhase::ApNotify:
	case ACS_CS_API_OmProfilePhase::CpNotify:
		{
			ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] phase: %d", __FUNCTION__, __LINE__, omProfileChange.phase));

			// Immediately stop the supervision timer from the current phase.
			cancelOmProfileSupervisionTimer(omProfileChange.phase);

			// Initiate rollback to current phase.
			omProfileChange.omProfileRequested = omProfileChange.omProfileCurrent;
			omProfileChange.aptRequested = omProfileChange.aptCurrent;
			omProfileChange.apzRequested = omProfileChange.apzCurrent;

			// jump to the commit phase.
			omProfileChange.phase = ACS_CS_API_OmProfilePhase::Commit;

			{// Log
				eventData << "Moving to Phase " << omProfileChange.phase << "." << "\n\n" << profileHandling->convertOmProfileToString(omProfileChange);
				ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");
				ACS_CS_EventReporter::instance()->resetAllEvents();
			}

			// Mark as NoChange to avoid clearing alarm on Commit.
			omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;

			// Write updates profile change info to table.
			lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
			for (int retry = 0; ( (retry < 10) && (writeOmProfile(omProfileChange) == false) ) ; ++retry) ACE_OS::sleep(1);
			lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

			// Notify subscribers and get actual subscriber count for the phase.
			subscriberCount = 0;
			agentInstance->updatePhaseChange(omProfileChange, subscriberCount);

			// Set supervision timer for commit phase.
			setOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Commit, PHASE_COMMIT_TIMEOUT_IN_SECONDS);

			{ // trace
				char trace[256] = {0};
				snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] OmProfile Notification sent to %d services subscribed to the Commit phase", __FILE__, __FUNCTION__, __LINE__, agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Commit));
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s\n", trace));
				std::cout << "DBG: " << trace << std::endl;
			}
		}

		break;

	case ACS_CS_API_OmProfilePhase::Commit:
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] phase: Commit. Subscriber count: %d", __FUNCTION__, __LINE__, subscriberCount));
		if (subscriberCount == 0)
		{
			// Failures in commit phase are simply logged and ignored...
			// However, we do need to handle the case where the last
			// subscriber responds with a failure...go back to Idle.

			// Stop the supervision timer for the Commit phase.
			cancelOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Commit);

			omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;

			// Reset requested profile values to unspecified.
			omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;


			{ // Log
				eventData << "OaM Profile Change completed.\n\n" << profileHandling->convertOmProfileToString(omProfileChange);
				ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");
				ACS_CS_EventReporter::instance()->resetAllEvents();
				std::cout << "DBG:" << eventData.str() << std::endl;
			}

			// Write action progress structure
			ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,UNDEF_ACTIONTYPE,UNDEF_ACTIONRESULTTYPE);

			// Write updates profile change info to table.
			lock->start_writing();std::cout << "LOCK ACQUIRED" <<std::endl;
			writeOmProfile(omProfileChange);
			lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
		}

		break;

	case ACS_CS_API_OmProfilePhase::Idle:
		std::cout << "DBG:" << __FUNCTION__ << "@" << __LINE__ << " - Do nothing" << std::endl;
		// Do nothing
		break;

	default:
		std::cout << "DBG:" << __FUNCTION__ << "@" << __LINE__ << " - Should never be here!!!" << std::endl;
		break;
	}

	ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - Out", __FUNCTION__, __LINE__));
	return true;
}

int ACS_CS_NEHandler::handleStartupSupervision ()
{
   // spawn thread for initialization mode/phase switch delay
   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) %s \n Creating initialization thread", __FUNCTION__));

   ACE_thread_t t;
   ACE_OS::thr_create((ACE_THR_FUNC) s_initializationModeSwitchDelayThread, this, 0, &t);

   return 0;
}

void ACS_CS_NEHandler::handleNeTableSubscription ()
{
   ACS_CS_NetworkElementChange neChange;

   ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
   if(NULL == model)
	   return;

   const ACS_CS_ImBase *base = model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());
   const ACS_CS_ImCpCluster *bladeClusterInfo = dynamic_cast<const ACS_CS_ImCpCluster *>(base);
   if(NULL == bladeClusterInfo)
	   return;

   base = model->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
   const ACS_CS_ImAdvancedConfiguration *advancedConf = dynamic_cast<const ACS_CS_ImAdvancedConfiguration *>(base);
   if(NULL == advancedConf)
	   return;

   neChange.clockMasterCpId = static_cast<uint16_t>(bladeClusterInfo->clockMaster);
   neChange.alarmMasterCpId = static_cast<uint16_t>(bladeClusterInfo->alarmMaster);
   neChange.clusterMode = static_cast<ACS_CS_API_ClusterOpMode::Value>(bladeClusterInfo->clusterOpMode);
   neChange.trafficLeaderCpId = static_cast<CPID>(advancedConf->trafficLeaderCp);
   neChange.trafficIsolatedCpId = static_cast<CPID>(advancedConf->trafficIsolatedCp);

   neChange.omProfile.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue> (advancedConf->omProfilePhase);
   neChange.omProfile.omProfileCurrent = static_cast<uint32_t>(advancedConf->omProfileCurrent);

   neChange.omProfile.apzCurrent = static_cast<uint32_t>(advancedConf->apzProfileCurrent);
   neChange.omProfile.aptCurrent = static_cast<uint32_t>(advancedConf->aptProfileCurrent);
   neChange.omProfile.omProfileRequested = static_cast<uint32_t>(advancedConf->omProfileRequested);
   neChange.omProfile.apzRequested = static_cast<uint32_t>(advancedConf->apzProfileRequested);
   neChange.omProfile.aptRequested = static_cast<uint32_t>(advancedConf->aptProfileRequested);
   neChange.omProfile.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue>(advancedConf->omProfileChangeTrigger);
   neChange.omProfile.apzQueued = static_cast<uint32_t>(advancedConf->apzProfileQueued);
   neChange.omProfile.aptQueued = static_cast<uint32_t>(advancedConf->aptProfileQueued);

   // Notify the subscription agent
   agentInstance->handleNEUpdates(neChange);
}

CPID ACS_CS_NEHandler::getCPID(std::string cpName)
{
	CPID cpId = static_cast<uint16_t>(-1);

	bool cpIdFound = false;

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
	if(NULL == model)
		return static_cast<uint16_t>(-1);

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	set<const ACS_CS_ImBase *> clusterCpObjects;
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	model->getObjects(clusterCpObjects, CLUSTERCP_T);
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	{
		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if (cp)
		{
			string alias = cp->alias;
			string defaultName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);

			transform(alias.begin(), alias.end(), alias.begin(), (int(*)(int))toupper);
			transform(defaultName.begin(), defaultName.end(), defaultName.begin(), (int(*)(int))toupper);

			if((cpName.compare(defaultName) == 0) || (cpName.compare(alias) == 0))
			{
				cpId = cp->systemIdentifier;
				cpIdFound = true;
				break;
			}

		}
	}

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && cpIdFound == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if (cp)
		{
			string defaultName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
			transform(defaultName.begin(), defaultName.end(), defaultName.begin(), (int(*)(int))toupper);

			if((cpName.compare(defaultName) == 0))
			{
				cpId = cp->systemIdentifier;
				cpIdFound = true;
				break;
			}
		}
	}

	return cpId;
}

ACS_CS_Protocol::CS_Result_Code ACS_CS_NEHandler::handleSetApaProfile(int newApzProfile, int newAptProfile)
{
   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
            "(%t) %s \n Entering method with apzProfile:%d and aptProfile:%d", __FUNCTION__,newApzProfile,newAptProfile));

   if (!CS_ProtocolChecker::checkIfAP1())
   {
      // handleSetApaProfile initiates a phase change and should only execute
      // on the MASTER.  For replication purposes, handleInternalSetOmProfile
      // will execute on SLAVEs.

      return ACS_CS_Protocol::Result_Success;
   }

   ACS_CS_API_OmProfileChange profileChangeData;
   if (!readOmProfile(profileChangeData))
   {
	   char trace[256] = {0};
	   snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] readOmProfile failed.", __FILE__, __FUNCTION__, __LINE__);
	   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
	   return ACS_CS_Protocol::Result_Other_Failure;
   }
   // Only initiate change if no change is ongoing and supervision is
   // completed.  Otherwise, change will be initiated when current
   // change returns to idle or startup supervision is complete.

   if (!startupProfileSupervisionComplete)							//TR HX22043
   {
      // If startup supervision is not complete queue requested
      // profiles.  Queued requests will be processed at the end
      // of startup supervision.  If there are previuosly queued
      // values, we just overwrite them.

      profileChangeData.aptQueued = newAptProfile;
      profileChangeData.apzQueued = newApzProfile;

      lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
      if (!writeOmProfile(profileChangeData))
      {
         // Failed to store updated profile values.
    	  lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
         return ACS_CS_Protocol::Result_Other_Failure;
      }
      lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

      ostringstream eventData;
      eventData << "APA Profile change request queued.  "
         << "Awaiting profile subscribers.  " << "\n\n"
         << convertOmProfileToString(profileChangeData);

      ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
         "OaM Profile Switch",
         eventData.str(),
         "");
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] %s", __FUNCTION__, __LINE__, eventData.str().c_str()));
      ACS_CS_EventReporter::instance()->resetAllEvents();

      return ACS_CS_Protocol::Result_Success;
   }


   if (profileChangeData.phase == ACS_CS_API_OmProfilePhase::Idle)
   {
      // We can initiate profile change immediately.
      profileChangeData.changeReason = ACS_CS_API_OmProfileChange::AutomaticProfileAlignment;
      profileChangeData.aptRequested = newAptProfile;
      profileChangeData.apzRequested = newApzProfile;
      // Clear any previously queued values
      profileChangeData.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      profileChangeData.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;

      // Check if we need to initiate APA. If the current and requested profiles
      // are same then ignore APA.
      if (profileChangeData.aptCurrent == profileChangeData.aptRequested &&
         profileChangeData.apzCurrent == profileChangeData.apzRequested)
      {
         if (!ignoreApaProfileRequest(profileChangeData))
         {
            // Failed to ignore the APA profile request.
            return ACS_CS_Protocol::Result_Other_Failure;
         }
         return ACS_CS_Protocol::Result_Success;
      }

      // Move to validate phase
      profileChangeData.phase = ACS_CS_API_OmProfilePhase::Validate;

      int newProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      if (!findMmlFileFolder(newProfileRequested,newApzProfile,newAptProfile))
      {
    	  ostringstream eventData;
    	  eventData << "Command Classification File Missing, Automatic Profile Alignment received in phase " << profileChangeData.phase << ".\n";
    	  ACS_CS_EVENT(Event_ProfileChangeFailure,
    			  ACS_CS_EventReporter::Severity_A1,
    			  "OaM PROFILE NOT ALIGNED IN AP AND CP",
    			  eventData.str(), "");
    	  /*HW99445 start*/
    	  m_apaAlarmText = eventData.str();
    	  if(writeAPAalarmdata())
    	  {
    		  m_apaAlarm = true;
    		  ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_TRACE, "[%s@%d] Successfully written data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
    	  }
    	  else
    	  {
    		  ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_ERROR, "[%s@%d] Failed to write data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
    	  }
    	  /*HW99445 stop*/
    	  ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] %s", __FUNCTION__, __LINE__, eventData.str().c_str()));
    	  // Failed the APA profile request.
    	  return ACS_CS_Protocol::Result_Missing_File;
      }

      profileChangeData.omProfileRequested = newProfileRequested;

      //rename MmlCommandRoles file
      changeMmlFileStatus(profileChangeData.omProfileCurrent, newProfileRequested, false, true);

      // write data to the table
      lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
      writeOmProfile(profileChangeData);
      lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;


      // Notify Subscribers and continue with phase change.
      subscriberCount = 0;
      agentInstance->updatePhaseChange(profileChangeData, subscriberCount);

      // Start Phase 0 Supervision Timer for 60 seconds
      setOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Validate, PHASE_TIMEOUT_IN_SECONDS);

      ostringstream eventData;
      eventData << "APA Profile change initiated.\n\n"
                << convertOmProfileToString(profileChangeData);

      ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
         "OaM Profile Switch",
         eventData.str(),
         "");
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] %s", __FUNCTION__, __LINE__, eventData.str().c_str()));
      ACS_CS_EventReporter::instance()->resetAllEvents();

   }
   else
   {
      // A profile change is already in progress queue requested profiles
      // Queued requests will be processed upon returning to Idle phase.

      profileChangeData.aptQueued = newAptProfile;
      profileChangeData.apzQueued = newApzProfile;

      lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
      writeOmProfile(profileChangeData);
      lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

      ostringstream eventData;
      eventData << "APA Profile change request queued.  "
                << "Switch already in progress.\n\n"
                << convertOmProfileToString(profileChangeData);

      ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
         "OaM Profile Switch",
         eventData.str(),
         "");
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] %s", __FUNCTION__, __LINE__, eventData.str().c_str()));
      ACS_CS_EventReporter::instance()->resetAllEvents();

      // If we are in the Validate or ApNotify phases, we must rollback.
      // Otherwise ongoing profile change is allowed to complete normally.
      if (profileChangeData.phase == ACS_CS_API_OmProfilePhase::Validate ||
          profileChangeData.phase == ACS_CS_API_OmProfilePhase::ApNotify)
      {
         ostringstream eventData;

         eventData << "APA profile change interrupted ongoing profile change in phase "
                   << profileChangeData.phase << ". Rolling back OaM Profile phase change.\n\n"
                   << convertOmProfileToString(profileChangeData);

         ACS_CS_EVENT(Event_ProfileChangeFailure, ACS_CS_EventReporter::Severity_Event,
            "OaM Profile Switch",
            eventData.str(),
            "");
         ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] %s", __FUNCTION__, __LINE__, eventData.str().c_str()));
         ACS_CS_EventReporter::instance()->resetAllEvents();

         // Initate a rollback
         std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
         rollbackOmProfile(profileChangeData);
      }
   }

   return ACS_CS_Protocol::Result_Success;
}

bool ACS_CS_NEHandler::readClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpMode) const
{
   // NOTE: We no longer have timeout exceptions. Good or bad?

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();


   ACS_CS_ImCpCluster *bci =
         reinterpret_cast<ACS_CS_ImCpCluster *>(model.getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str()));

   if (bci != NULL)
   {
      ClusterOpModeEnum com = bci->clusterOpMode;
      clusterOpMode = (ACS_CS_API_ClusterOpMode::Value) com;
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
               "(%t) %s \n Old ClusterOpMode attribute not valid...using default Normal", __FUNCTION__));

      // Use default value of Normal
      clusterOpMode = ACS_CS_API_ClusterOpMode::Normal;

   }

   return true;
}

int ACS_CS_NEHandler::execStartupSupervision ()
{
    // Startup supervision only runs on the master.
    if (CS_ProtocolChecker::checkIfAP1())
    {
        // Read OM Profile
    	ACS_CS_API_OmProfileChange omProfileData;
    	readOmProfile(omProfileData);

        if( omProfileData.phase != ACS_CS_API_OmProfilePhase::Idle)
        {
            ostringstream eventData;
            eventData << "CS Restarted while in Phase " << omProfileData.phase << ".\n";

            // If we restart in validate or ApNotify phases initiated by nech, go ahead and immediately
            // set phase to commit.  Notification will be sent only after the supervision
            // time has expired.
            if ( (omProfileData.changeReason == ACS_CS_API_OmProfileChange::NechCommand ||
                 (omProfileData.changeReason == ACS_CS_API_OmProfileChange::NechCommandForApgOnly)) &&
                 (omProfileData.phase == ACS_CS_API_OmProfilePhase::Validate ||
                 omProfileData.phase == ACS_CS_API_OmProfilePhase::ApNotify))
            {
                // Initiate rollback to current phase.
                omProfileData.omProfileRequested = omProfileData.omProfileCurrent;
                omProfileData.aptRequested = omProfileData.aptCurrent;
                omProfileData.apzRequested = omProfileData.apzCurrent;

                // jump to the commit phase.
                omProfileData.phase = ACS_CS_API_OmProfilePhase::Commit;

                // Write updates profile change info to table.
                lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
                writeOmProfile(omProfileData);
                lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

                eventData << "Moving to Phase " << omProfileData.phase << "."
                          << "\n\n" << profileHandling->convertOmProfileToString(omProfileData);

                ACS_CS_EVENT(Event_ProfileChangeFailure, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");
            }
            else
            {
                eventData << "Continuing with phase change."
                          << "\n\n" << profileHandling->convertOmProfileToString(omProfileData);

                ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");
            }

            ACS_CS_EventReporter::instance()->resetAllEvents();
        }

        ACS_CS_EventHandle shutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);

        int handleCount = 1;
        ACS_CS_EventHandle handles[handleCount];

        handles[0] = shutdownEvent;

        // Sleep 60 sec and prepare to handle the shutdown event
        int result = ACS_CS_Event::WaitForEvents(handleCount, handles, INITIALIZATION_DELAY);

        switch (result)
        {
            case ACS_CS_EVENT_RC_TIMEOUT:   // Time out, means startup supervision completed as expected
                break;                      // No extra action needed here

            case 0:                         // Shutdown event received
                ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                        "(%t) %s \n Shutdown when executing startup supervision", __FUNCTION__));

                return 0;                   // Stop startup supervision thread

            default:
                ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                        "(%t) %s \n Error: Failed to WaitForEvents", __FUNCTION__));
        }

        startupProfileSupervisionComplete = true;
        ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Startup profile supervision is completed!", __FUNCTION__, __LINE__));

        std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - INFO: Startup profile supervision is completed" << std::endl;
        setStartupProfileSupervision(true);

        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                "(%t) %s \n Enabled mode and phase changes after %d seconds",
                __FUNCTION__,
                INITIALIZATION_DELAY / MILLISECONDS_PER_SECOND));

        ACS_CS_API_ClusterOpMode::Value oldClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
        ACS_CS_API_ClusterOpMode::Value newClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;

        ACS_CS_API_Util_Internal::readClusterOpMode(oldClusterOpMode);

        if(oldClusterOpMode == ACS_CS_API_ClusterOpMode::SwitchingToNormal ||
        		oldClusterOpMode == ACS_CS_API_ClusterOpMode::SwitchingToExpert)
        {
        	// CS Service was restarted while a clusterOpModeSwitch was in progress.
        	// Notify Subscribers so that ADH can finalize the switch.

        	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - CS Service was restarted while a clusterOpModeSwitch was in progress." << std::endl;
        	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - Notify Subscribers so that ADH can finalize the switch." << std::endl;

        	if (this->agentInstance->getTableSubscriberCount(ACS_CS_Protocol::Scope_NE) == 0)
        	{
        		// If ADH is still not connected, we'll just forcibly complete the switch.  It
        		// will be up to ADH to read the mode and align prior to subscribing.
        		newClusterOpMode = (oldClusterOpMode == ACS_CS_API_ClusterOpMode::SwitchingToNormal) ?
        				ACS_CS_API_ClusterOpMode::Normal : ACS_CS_API_ClusterOpMode::Expert;

        		std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - ADH is still not connected, we'll just forcibly complete the switch." << std::endl;
        		lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
        		ACS_CS_API_Util::writeClusterOpMode(newClusterOpMode);
        		lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
        	}
        	else
        	{
        		// We have at least 1 subscriber
        		std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - We have at least 1 subscriber..." << std::endl;

        		notificationsPending = true;

        		// Send notifications for NETableChange
        		handleNeTableSubscription();

        		// Restart supervision timer.
        		this->setClusterOpModeSupervisionTimer(CLUSTEROPMODE_TIMEOUT_IN_SECONDS);
        	}
        }
        /*HW99445 start*/
        if (m_isRestartAfterRestore)
        {
        	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] System restarted after restore so APA ALARM is not raised %d", __FUNCTION__, __LINE__,m_isRestartAfterRestore));
        	struct stat fileStat;
        	if(stat(APA_ALARM_FILE.c_str(), &fileStat) < 0)
        	{
        		if(errno == ENOENT)
        		{
        			ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] apaAlarm FILE does not exist nothing to do", __FUNCTION__, __LINE__));
        		}
        	}
        	else if (remove(APA_ALARM_FILE.c_str()) != 0)
        	{
        		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] apaAlarm file exist but not deleted", __FUNCTION__, __LINE__));
        	}
        	else
        	{
        		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] apaAlarm file is deleted successfully", __FUNCTION__, __LINE__));
        	}
        }
        else if(readAPAalarmdata())
        {
        	ostringstream eventData;
        	eventData << m_apaAlarmText;
        	ACS_CS_EVENT(Event_ProfileChangeFailure,
        			ACS_CS_EventReporter::Severity_A1,
					"OaM PROFILE NOT ALIGNED IN AP AND CP",
					eventData.str(), "");
        	m_apaAlarm = true;
        	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] Successfully read data from /data/acs/data/cs/apaAlarm and raised alarm", __FUNCTION__, __LINE__));
        }
        /*HW99445 stop*/
        processProfileChangesDuringStartUp();

    }
    else  // ACS_CS_Protocol::ServiceType_SLAVE
    {
    	//  Immedately end supervision if we are not MASTER.
    	startupProfileSupervisionComplete = true;
    	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - INFO: Startup profile supervision is completed" << std::endl;
    }

    // Kick-start the Agent so that it start sending notifications,
    // including any queued during startup supervision.
    agentInstance->enableProfileChangeNotification();
    std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - INFO: NOTIFICATIONS ENABLED" << std::endl;

    return 0;
}

void ACS_CS_NEHandler::processProfileChangesDuringStartUp ()
{
	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - In" << std::endl;

   ACS_CS_API_OmProfileChange omProfileData;
   readOmProfile(omProfileData);


   bool doneProcessing = false;

   // When phase is not in Idle processing is treated as completed only when
   // either notifications are sent for a phase including rollback or
   // OaM Profile change is committed because of no subscribers. In the later
   // case as well as in Idle phase it checks for any queued APA requests
   // before it is done.
   while (!doneProcessing)
   {
      switch(omProfileData.phase)
      {
         // Note that due to processing that happens immediately before the
         // startup supervision timer is started, The Validate and ApNotify cases
         // can only occur here if there was an APA initiated change in progress.
         // CpNotify and Commit appl for either nech initiated or APA initiated profile changes.

         case ACS_CS_API_OmProfilePhase::Validate:

            // CS restarted in Validate phase. This case is only applicable
            // for APA since nech initiated Profile changes would have been
            // rolled back if it is in Validate phase. Need to reset the
            // OaM Profile value before sending out new notifications for APA.

            if (ACS_CS_API_OmProfileChange::AutomaticProfileAlignment ==
               omProfileData.changeReason)
            {

            	int newProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
            	if (findMmlFileFolder(newProfileRequested,omProfileData.apzRequested,omProfileData.aptRequested))
            	{
            		omProfileData.omProfileRequested = newProfileRequested;

            		// Write updates profile change info to table.
            		lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
            		writeOmProfile(omProfileData);
            		lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
            	}

            }

            // Resend notifications to all subscribers
            subscriberCount = 0;
            agentInstance->updatePhaseChange(omProfileData, subscriberCount);

            if (subscriberCount == 0)
            {
               ostringstream eventData;

               eventData << "No subscribers for phase 0."
                  << " Rolling back OaM Profile phase change."
                  << "\n\n" << profileHandling->convertOmProfileToString(omProfileData);

               ACS_CS_EVENT(Event_ProfileChangeFailure, ACS_CS_EventReporter::Severity_Event,
                     "OaM Profile Switch",
                     eventData.str(),
                     "");

               ACS_CS_EventReporter::instance()->resetAllEvents();

               // No subscriber in these phases requires a rollback
               std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
               rollbackOmProfile(omProfileData);

               // done processing since notifications are went out as part of rollback.
               doneProcessing = true;
            }
            else
            {
               ostringstream eventData;

               eventData << "Sent notifications for phase 0."
                  << "\n\n" << convertOmProfileToString(omProfileData);

               ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
                     "OaM Profile Switch",
                     eventData.str(),
                     "");

               ACS_CS_EventReporter::instance()->resetAllEvents();

               // Start the supervision timer for subscriber responses
               setOmProfileSupervisionTimer(omProfileData.phase, PHASE_TIMEOUT_IN_SECONDS);

               // done processing since notifications are went out.
               doneProcessing = true;
            }
            break;

         case ACS_CS_API_OmProfilePhase::ApNotify:

            // Resend notifications to all subscribers
            agentInstance->updatePhaseChange(omProfileData, subscriberCount);

            if(agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::ApNotify) != 0)
            {
               // We have subscirbers that have been notified...we're now in the
               // ApNotify phase.

               ostringstream eventData;

               eventData << "Sent notifications for phase 1."
                  << "\n\n" << profileHandling->convertOmProfileToString(omProfileData);

               ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
                     "OaM Profile Switch",
                     eventData.str(),
                     "");

               ACS_CS_EventReporter::instance()->resetAllEvents();

               // Start the supervision timer for subscriber responses
               setOmProfileSupervisionTimer(omProfileData.phase, PHASE_TIMEOUT_IN_SECONDS);

               // done processing since notifications are went out.
               doneProcessing = true;
            }
            else
            {
               // No subscribers for ApNotify phase. Moving into CpNotify Phase
               omProfileData.phase = ACS_CS_API_OmProfilePhase::CpNotify;

               // Write updates profile change info to table.
               lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
               writeOmProfile(omProfileData);
               lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
            }

            break;

         case ACS_CS_API_OmProfilePhase::CpNotify:

            // Resend notifications to all subscribers
            agentInstance->updatePhaseChange(omProfileData,subscriberCount);

            if(agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::CpNotify) != 0)
            {
               ostringstream eventData;

               eventData << "Sent notifications for phase 2."
                  << "\n\n" << profileHandling->convertOmProfileToString(omProfileData);

               ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
                     "OaM Profile Switch",
                     eventData.str(),
                     "");

               ACS_CS_EventReporter::instance()->resetAllEvents();

               // Start the supervision timer for subscriber responses
               setOmProfileSupervisionTimer(omProfileData.phase, PHASE_TIMEOUT_IN_SECONDS);

               // done processing since notifications are went out.
               doneProcessing = true;
            }
            else
            {
               if (ACS_CS_API_OmProfileChange::NechCommandForApgOnly == omProfileData.changeReason)
               {
                  // Moving into Commit Phase
                  omProfileData.phase = ACS_CS_API_OmProfilePhase::Commit;

                  // Write updates profile change info to table.
                  lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
                  writeOmProfile(omProfileData);
                  lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
               }
               else
               {
                  // CP_PROXY_REQUIRED is expected to be defined as a MACRO in build
                  // settings when cpProxy is delivered upon which it is mandatory
                  // and must be subscribed.
#ifdef CP_PROXY_REQUIRED
#warning "\nCP_PROXYCOMPILATION \n"
                  ostringstream eventData;

                  eventData << "No subscribers for phase 2."
                     << "Rolling back OaM Profile phase change.\n\n" << profileHandling->convertOmProfileToString(omProfileData);

                  ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
                        "OaM Profile Switch",
                        eventData.str(),
                        "");
                  ACS_CS_EventReporter::instance()->resetAllEvents();

                  // No subscriber in these phases requires a rollback
                  profileHandling->rollbackOmProfile();

                  // done processing since notifications are went out as part of rollback.
                  doneProcessing = true;
#else

                  // Moving into Commit Phase
                  omProfileData.phase = ACS_CS_API_OmProfilePhase::Commit;

                  // Write updates profile change info to table.
                  lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
                  writeOmProfile(omProfileData);
                  lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
#endif
               }
            }

            break;

         case ACS_CS_API_OmProfilePhase::Commit:

            // Resend notifications to all subscribers
            agentInstance->updatePhaseChange(omProfileData,subscriberCount);

            if(agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Commit) != 0)
            {
               ostringstream eventData;

               eventData << "Sent notifications for phase 3."
                  << "\n\n" << profileHandling->convertOmProfileToString(omProfileData);

               ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
                     "OaM Profile Switch",
                     eventData.str(),
                     "");

               ACS_CS_EventReporter::instance()->resetAllEvents();

               // Start the supervision timer for subscriber responses
               setOmProfileSupervisionTimer(omProfileData.phase, PHASE_COMMIT_TIMEOUT_IN_SECONDS);

               // done processing since notifications are went out.
               doneProcessing = true;
            }
            else
            {
               // We're done with the profile switch. Return to Idle phase.
               omProfileData.phase = ACS_CS_API_OmProfilePhase::Idle;

               // Reset requested profile values to unspecified.
               omProfileData.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
               omProfileData.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
               omProfileData.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
               omProfileData.changeReason = ACS_CS_API_OmProfileChange::NoChange;

               // Write updates profile change info to table.
               lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
               writeOmProfile(omProfileData);
               lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

               ostringstream eventData;
               eventData << " No subscribers for phase 3. OaM Profile change completed."
                  << "\n\n" << convertOmProfileToString(omProfileData);

               ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
                     "OaM Profile Switch",
                     eventData.str(),
                     "");

               ACS_CS_EventReporter::instance()->resetAllEvents();

               // After we commit we need to check for any queued profile requests
               checkQueuedApaProfiles();

               // done processing since notifications are went out.
               doneProcessing = true;
            }
            break;

         case ACS_CS_API_OmProfilePhase::Idle:

            // Came up from a restart in the idle phase...check to see if there is a
            // queued APA profile change request to process.
             checkQueuedApaProfiles();

           // done processing since notifications are went out.
           doneProcessing = true;
           break;

         default:
           // Should never be here.
           doneProcessing = true;
           break;
      }
   }
   return;
}


bool ACS_CS_NEHandler::checkForMMLCommandRulesFile (string &mmlFilePath, string &mmlFileName, CcFileStateType type, int profile)
{
	//check file presence
	string cType("");
	ostringstream mmlFileStream;
	ostringstream mmlFileNameStream;
	int profileToCheck = -1;

	if (profile == UNDEF_OMPROFILESTATETYPE)
		profileToCheck = getCurrentOmProfileValue();
	else
		profileToCheck = profile;

	if (type == CURRENT_CCF)
		cType = "";
	else if (type == BACKUP_CCF)
		cType = ".bck";
	else if (type == NEW_CCF)
		cType = ".new";
	else
		return false;

	mmlFileNameStream << CC_FILENAME_BASE << profileToCheck << CC_FILENAME_EXT << cType;
	mmlFileStream << getCcfPath() << CC_FILENAME_BASE << profileToCheck << CC_FILENAME_EXT << cType;
	mmlFilePath = mmlFileStream.str();
	mmlFileName = mmlFileNameStream.str();

	cout << " file to search: "<< mmlFilePath.c_str()<< endl;

	if (!boost::filesystem::exists(mmlFilePath))
	{
		cout << "Can't find file!" << endl;
		return false;
	}

	return true;
}

bool ACS_CS_NEHandler::getClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpMode) const
{
	lock->start_reading();
	bool success = readClusterOpMode (clusterOpMode);
	lock->stop_reading();
	return success;
}

static void* s_initializationModeSwitchDelayThread(void *arg)
{
    ACS_CS_NEHandler *neh = (ACS_CS_NEHandler*)arg;
    neh->execStartupSupervision();
    return 0;
}

void ACS_CS_NEHandler::setStartupProfileSupervision(bool completed){

	ACS_CS_ImModel *model = new ACS_CS_ImModel();
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(!model || !immReader){
		if(model)
			delete model;
		if (immReader)
			delete immReader;

		return;
	}

	ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* advInfo = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

	if(!advInfo){
		delete immReader;
		delete model;
		if (advBase)
			delete advBase;
		return;
	}

	bool oldValue = true;
	int startupProfileSupervision = advInfo->startupProfileSupervision;

	if (startupProfileSupervision == 0)
		oldValue = false;

	if(completed == oldValue){
		delete immReader;
		delete model;
		delete advInfo;
		return;
	}

	if(completed)
		advInfo->startupProfileSupervision = 1;
	else
		advInfo->startupProfileSupervision = 0;

	advInfo->action = ACS_CS_ImBase::MODIFY;
	model->addObject(advInfo);

	ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
	saver->save("startupProfileSupervision");

	delete saver;
	delete immReader;
	delete model;
}


bool ACS_CS_NEHandler::setClusterOpModeChange (ACS_CS_API_ClusterOpMode::Value newClusterOpMode)
{
	bool result = false;

	ACS_CS_API_ClusterOpMode::Value currentClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
	getClusterOpMode(currentClusterOpMode);

	if (currentClusterOpMode != newClusterOpMode)
	{
		if((ACS_CS_BrfHandler::getInstance()->isBackupOngoing()) != true)	
		{
			ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "no backupOngoing\n"));
		ACS_CS_API_ClusterOpMode::Value valueToSet = ACS_CS_API_ClusterOpMode::Normal; //default value
		if(newClusterOpMode == ACS_CS_API_ClusterOpMode::Normal)
		{

			// Request is for a mode switch to Normal, convert to SwitchingToNormal
			// The service will take care of the rest with the propagation to the normal mode
			valueToSet = ACS_CS_API_ClusterOpMode::SwitchingToNormal;

		}
		else if(newClusterOpMode == ACS_CS_API_ClusterOpMode::Expert)
		{
			// Request is for Expert mode, convert to SwitchingToExpert
			// The service will take care of the rest with the propagation to the expert mode
			valueToSet = ACS_CS_API_ClusterOpMode::SwitchingToExpert;
		}

		handleSetClusterOpModeChanged(valueToSet,ACS_CS_API_NE_NS::CLUSTER_OP_MODE_REQUEST);
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "write to async\n"));
			ACS_CS_API_Util::writeAsyncActionStruct(FINISHED, CHANGE_OP_MODE, FAILURE, "Backup Ongoing");

		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "ACS_CS_NEHandler::handleSetClusterOpModeChanged()\n Operative mode already defined\n"));
		ACS_CS_API_Util::writeAsyncActionStruct(FINISHED, CHANGE_OP_MODE, FAILURE, "Operative mode already defined");
	}

	return result;
}

bool ACS_CS_NEHandler::getOmProfilePhase (ACS_CS_API_OmProfilePhase::PhaseValue &omProfilePhase) const
{
	lock->start_reading();
	ACS_CS_API_OmProfileChange omProfileData;
	bool success = ACS_CS_API_Util_Internal::readOmProfile(NULL,omProfileData);
	omProfilePhase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue>(omProfileData.phase);
	lock->stop_reading();
	return success;
}


//*******************************************************
// OM Profile handling
//*******************************************************

//----------------------
// setClusterOmProfile
//----------------------
bool ACS_CS_NEHandler::setClusterOmProfile (int actionId , int profileValue, int apzProfile, int aptProfile)
{
	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Setting new OmProfile: %d", __FUNCTION__, __LINE__, profileValue));

	AsyncActionType id = UNDEF_ACTIONTYPE;
	if (actionId == ACS_CS_ImmMapper::CP_CLUSTER_OP_ACTIVE_OM_PROFILE_ID) id = ACTIVATE_CCF;
	else if (actionId == ACS_CS_ImmMapper::CP_CLUSTER_OP_CHANGE_OM_PROFILE_ID) id = CHANGE_OM_PROFILE;
	else if (actionId == ACS_CS_ImmMapper::CP_CLUSTER_OP_ROLLBACK_FILE_ID) id = ROLLBACK_CCF;
	else return false;
	bool isbackup = ACS_CS_BrfHandler::getInstance()->isBackupOngoing();
	if(isbackup)
	{
		string reason("Backup Ongoing");
		lock->start_writing(); std::cout << __LINE__ << " LOCK ACQUIRED" <<std::endl;
                ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,id,FAILURE,reason);
                lock->stop_writing(); std::cout << __LINE__<< " LOCK RELEASED" <<std::endl;
		
		return false;
	}
	

	ACS_CS_Protocol::CS_Result_Code result = ACS_CS_Protocol::Result_Success;

	if (profileValue == static_cast<int>(ACS_CS_API_OmProfileChange::UnspecifiedProfile) &&
			apzProfile != static_cast<int>(ACS_CS_API_OmProfileChange::UnspecifiedProfile) &&
			aptProfile != static_cast<int>(ACS_CS_API_OmProfileChange::UnspecifiedProfile))
	{
		m_ScopeFlag = false;
		lock->start_writing(); std::cout << __LINE__ << " LOCK ACQUIRED" <<std::endl;
		ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,id,NOT_AVAILABLE,"Automatic Profile Alignment");
		lock->stop_writing(); std::cout << __LINE__<< " LOCK RELEASED" <<std::endl;

		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Handling of APA profile", __FUNCTION__, __LINE__));

		result = handleSetApaProfile(apzProfile, aptProfile);
	}
	else if (isLocalProfileRequested())
	{
		m_ScopeFlag = false;
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_WARN, "[%s@%d] APA Handling of Local Profile Change for APG only.SCOPE FLAG=%d", __FUNCTION__, __LINE__,m_ScopeFlag));
		lock->start_writing(); std::cout << __LINE__ << " LOCK ACQUIRED" <<std::endl;
		ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,id,NOT_AVAILABLE,"Local Update Profile");
		lock->stop_writing(); std::cout << __LINE__<< " LOCK RELEASED" <<std::endl;

		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Handling of Local Profile Change for APG only.", __FUNCTION__, __LINE__));
		result = handleSetOmProfile(profileValue, MANUAL_FOR_APG_ONLY);
	}
	else
	{
		m_ScopeFlag = true;
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_WARN, "[%s@%d] APA scope value is set to %d CP_AP ", __FUNCTION__, __LINE__,m_ScopeFlag));

		lock->start_writing(); std::cout << __LINE__ << " LOCK ACQUIRED" <<std::endl;
		ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,id);
		lock->stop_writing(); std::cout << __LINE__<< " LOCK RELEASED" <<std::endl;

		result = handleSetOmProfile(profileValue, MANUAL);
	}

	if (result != ACS_CS_Protocol::Result_Success)
	{
		//trace error
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s@%d] Error: %d\n", __FUNCTION__, __LINE__, result));
		string ApaProfileReason("");
		if (profileValue == static_cast<int>(ACS_CS_API_OmProfileChange::UnspecifiedProfile)){
			ApaProfileReason = "Automatic Profile Alignment, ";
		}

		string reason("");

		switch(result){
		case ACS_CS_Protocol::Result_Missing_File:
			reason = ApaProfileReason + "Missing command classification file.";
			break;
		case ACS_CS_Protocol::Result_Awaiting_Dependency:
			reason = ApaProfileReason + "Try the option again when the resources necessary for the requested change are available.";
			break;
		case ACS_CS_Protocol::Result_Busy:
			reason = ApaProfileReason + "Necessary resources for the requested change are not available.";
			break;
		default:
			reason = ApaProfileReason + "Internal Error";
			break;
		}

		lock->start_writing(); std::cout << __LINE__ << " LOCK ACQUIRED" <<std::endl;
		ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,id,FAILURE,reason);
		lock->stop_writing(); std::cout << __LINE__<< " LOCK RELEASED" <<std::endl;

		//restore MmlFileFolder
		cleanMmlFileFolder(profileValue);

		return false;
	}

	return true;
}

//---------------------
// handleSetOmProfile
//---------------------
ACS_CS_Protocol::CS_Result_Code ACS_CS_NEHandler::handleSetOmProfile (int newOmProfile, ProfileChangeTriggerEnum reason)
{

	if (!CS_ProtocolChecker::checkIfAP1())
   {
      // handleSetOmProfile initiates a phase change and should only execute
      // on the MASTER.  For replication purposes, handleInternalSetOmProfile
      // will execute on SLAVEs.
      return ACS_CS_Protocol::Result_Success;
   }

   // Verify startup supervision is complete before accepting
   // profile change requests.

   if (!startupProfileSupervisionComplete)
   {
      return ACS_CS_Protocol::Result_Awaiting_Dependency;
   }

   ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] The startup profile supervision is completed!", __FUNCTION__, __LINE__));
	ACS_CS_API_OmProfileChange profileChangeData;
	if (!readOmProfile(profileChangeData))
	{
		char trace[256] = {0};
		snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] readOmProfile failed.", __FILE__, __FUNCTION__, __LINE__);
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
		return ACS_CS_Protocol::Result_Other_Failure;
	}

	std::string path("");
	std::string filename("");
	CcFileStateType type;

	//get type of file to search
	AsyncActionType id;
	readOmProfileActionId(id);
	if (id == ROLLBACK_CCF) type = BACKUP_CCF;
	else type = NEW_CCF;

	if (newOmProfile == static_cast<int>(ACS_CS_API_OmProfileChange::UnspecifiedProfile))
	{
		//activation of a new CcFile with the same OmProfile value
		newOmProfile = profileChangeData.omProfileCurrent;
	}

	//check if the file exists
	if(!checkForMMLCommandRulesFile(path, filename, type, newOmProfile))
	{
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
				"(%t) %s,line: %i\n  Error: Missing command classification file for profile: \n%i", __FUNCTION__, __LINE__,newOmProfile));
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d] Error: Missing command classification file for profile: %d ", __FUNCTION__, __LINE__, newOmProfile));
		return ACS_CS_Protocol::Result_Missing_File;
	}

	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Command Classification file found for profile: %d", __FUNCTION__, __LINE__, newOmProfile));

   	//Check for subscribers
   bool rejectProfileSwitch = false;
   if( (agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Validate) == 0) && newOmProfile )
   {
	   rejectProfileSwitch = true;

	   //trace problem
	   char trace[256] = {0};
	   snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] Error: Reject Profile Switch: No subscriber for validate phase (0) (e.g. ADH down) and requested profile is not 0(blade profile).", __FILE__, __FUNCTION__, __LINE__);
	   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s\n", trace));
   }


   // CP_PROXY_REQUIRED is expected to be defined as a MACRO in build settings
   // when cpProxy is delivered upon which it is mandatory and must be subscribed.

#ifdef CP_PROXY_REQUIRED

   if( reason != MANUAL_FOR_APG_ONLY && newOmProfile &&
      agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::CpNotify) == 0)
   {
      rejectProfileSwitch = true;

      ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
         "(%t) %s, line: %i\n"
         "Error: Reject Profile Switch: No subscription from CP-Proxy.\n",
         __FUNCTION__, __LINE__));
   }

#endif

   if( rejectProfileSwitch )
   {
	   return ACS_CS_Protocol::Result_Busy;
   }


   //rename MmlCommandRoles file
   changeMmlFileStatus(profileChangeData.omProfileCurrent, newOmProfile, false, true);

   // Set updated omProfile and phase data to be written to table
   profileChangeData.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue>(reason);
   profileChangeData.omProfileRequested = newOmProfile;
   profileChangeData.phase = ACS_CS_API_OmProfilePhase::Validate;

   // write data to the table
   lock->start_writing(); std::cout << __LINE__ << " LOCK ACQUIRED" <<std::endl;
   if (!writeOmProfile(profileChangeData))
   {
	   lock->stop_writing(); std::cout << __LINE__ << " LOCK RELEASED" <<std::endl;
	   // Failed to store updated profile values. Abort phase change request.
	   return ACS_CS_Protocol::Result_Other_Failure;
   }

   lock->stop_writing(); std::cout << __LINE__ << " LOCK RELEASED" <<std::endl;

   // Notify subscribers and get actual subscriber count for the phase.
   subscriberCount = 0;
   agentInstance->updatePhaseChange(profileChangeData, subscriberCount);

   { // trace
	   char trace[256] = {0};
	   snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] OmProfile Notification sent to %d services subscribed to the Validate phase", __FILE__, __FUNCTION__, __LINE__, agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Validate));
	   ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s\n", trace));
   }

   // Start Phase Supervision Timer
   if (0 == subscriberCount)
   {
      // no need to wait if there aren't any subscribers
      setOmProfileSupervisionTimer(profileChangeData.phase, PHASE_TIMEOUT_IN_SECONDS/*PHASE_NO_TIMEOUT*/);
   }
   else
   {
	   ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Found %d subscribers in phase: %d. Setting timer!", __FUNCTION__, __LINE__, subscriberCount, profileChangeData.phase));
      if( profileChangeData.phase == ACS_CS_API_OmProfilePhase::Commit)
      {
         setOmProfileSupervisionTimer(profileChangeData.phase, PHASE_COMMIT_TIMEOUT_IN_SECONDS);
      }
      else
      {
         setOmProfileSupervisionTimer(profileChangeData.phase, PHASE_TIMEOUT_IN_SECONDS);
      }
   }

   { // Log
	   ostringstream eventData;
	   eventData << "Profile change initiated." << "\n\n"
			   << convertOmProfileToString(profileChangeData);

	   ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
			   "OaM Profile Switch",
			   eventData.str(),
			   "");

	   ACS_CS_EventReporter::instance()->resetAllEvents();
   }

   std::cout << "return from : " << __FUNCTION__ << std::endl;

   return ACS_CS_Protocol::Result_Success;
}


//*******************************************************
// OM Profile handling: READ / WRITE
//*******************************************************

//---------------
// readOmProfile
//---------------
bool ACS_CS_NEHandler::readOmProfile (ACS_CS_API_OmProfileChange& omProfileChange) const
{
	/* TODO: to be fixed: imm repository not updated
	const char *advancedConfigurationDN = ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str();
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	//Get Object
	ACS_CS_ImBase *base = model->getObject(advancedConfigurationDN);
	if(!base)
	{
		//Log error
		char tracep[512] = {0};
		snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: Cannot read %s", __FILE__, __FUNCTION__, __LINE__, advancedConfigurationDN);
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
		std::cout << "DBG:" << tracep << std::endl;
		return false;
	}

	ACS_CS_ImAdvancedConfiguration *advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration *>(base);
	if(!advancedConfiguration)
	{
		char tracep[512] = {0};

		snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: Cannot cast the base object found", __FILE__, __FUNCTION__, __LINE__);
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
		std::cout << "DBG:" << tracep << std::endl;
		return false;
	}

	omProfileChange.omProfileCurrent = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->omProfileCurrent);
	omProfileChange.omProfileRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->omProfileRequested);
	omProfileChange.aptCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileCurrent);
	omProfileChange.aptRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileRequested);
	omProfileChange.aptQueued = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileQueued);
	omProfileChange.apzCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileCurrent);
	omProfileChange.apzRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileRequested);
	omProfileChange.apzQueued    = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileQueued);
	omProfileChange.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue> (advancedConfiguration->omProfilePhase);
	omProfileChange.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue> (advancedConfiguration->omProfileChangeTrigger);

	*/


	//TODO: This is a workaround! Use the code commented out
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

	if(!advancedConfiguration)
	{
		if (immReader) delete immReader;
		if (advBase) delete advBase;
		return false;
	}


	omProfileChange.omProfileCurrent = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->omProfileCurrent);
	omProfileChange.omProfileRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->omProfileRequested);
	omProfileChange.aptCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileCurrent);
	omProfileChange.aptRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileRequested);
	omProfileChange.aptQueued = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileQueued);
	omProfileChange.apzCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileCurrent);
	omProfileChange.apzRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileRequested);
	omProfileChange.apzQueued    = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileQueued);
	omProfileChange.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue> (advancedConfiguration->omProfilePhase);
	omProfileChange.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue> (advancedConfiguration->omProfileChangeTrigger);

	if (immReader) delete immReader;
	if (advBase) delete advBase;

	return true;
}

//-----------------
// writeOmProfile
//-----------------
bool ACS_CS_NEHandler::writeOmProfile (const ACS_CS_API_OmProfileChange& omProfileChange)
{
	bool start_end_switching = false;
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
	ACS_CS_ImCcFile *ccFile_current = NULL;
	ACS_CS_ImCcFile *ccFile_remove = NULL;

	//Get AdvancedConfig Object
	ACS_CS_ImBase *base = model->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
	if(!base)
	{
		//Log error
		char tracep[512] = {0};
		snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: Cannot read %s", __FILE__, __FUNCTION__, __LINE__, ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
		return false;
	}

	//Get OmProfileManager Object
	ACS_CS_ImBase *baseCp = model->getObject(ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER.c_str());
	if(!baseCp)
	{
		//Log error
		char tracep[512] = {0};
		snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: Cannot read %s", __FILE__, __FUNCTION__, __LINE__, ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER.c_str());
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
		return false;
	}

	ACS_CS_ImAdvancedConfiguration *advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration *>(base);
	ACS_CS_ImOmProfileManager* omProfileMgr = dynamic_cast<ACS_CS_ImOmProfileManager*>(baseCp);

	if((!advancedConfiguration) || (!omProfileMgr))
	{
		char tracep[512] = {0};

		snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: Cannot cast the base object found", __FILE__, __FUNCTION__, __LINE__);
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
		return false;
	}

	ACS_CS_ImAdvancedConfiguration *advancedConfigurationClonedObj = dynamic_cast<ACS_CS_ImAdvancedConfiguration *> (advancedConfiguration->clone());
	ACS_CS_ImOmProfileManager *omProfileMgrClonedObj = dynamic_cast<ACS_CS_ImOmProfileManager *> (omProfileMgr->clone());

	if ((!advancedConfigurationClonedObj) || (!omProfileMgrClonedObj))
		return false;

	/////////////////////////////////
	//advancedConfigurationClonedObj
	/////////////////////////////////
	advancedConfigurationClonedObj->omProfileCurrent = static_cast<int> (omProfileChange.omProfileCurrent);
	advancedConfigurationClonedObj->omProfileRequested = static_cast<int> (omProfileChange.omProfileRequested);
	advancedConfigurationClonedObj->aptProfileCurrent = static_cast<int> (omProfileChange.aptCurrent);
	advancedConfigurationClonedObj->aptProfileRequested = static_cast<int> (omProfileChange.aptRequested);
	advancedConfigurationClonedObj->aptProfileQueued = static_cast<int> (omProfileChange.aptQueued);
	advancedConfigurationClonedObj->apzProfileCurrent = static_cast<int> (omProfileChange.apzCurrent);
	advancedConfigurationClonedObj->apzProfileRequested = static_cast<int> (omProfileChange.apzRequested);
	advancedConfigurationClonedObj->apzProfileQueued = static_cast<int> (omProfileChange.apzQueued);
	advancedConfigurationClonedObj->omProfilePhase = static_cast<OmProfilePhaseEnum> (omProfileChange.phase);
	advancedConfigurationClonedObj->omProfileChangeTrigger = static_cast<ProfileChangeTriggerEnum> (omProfileChange.changeReason);

	advancedConfigurationClonedObj->action = ACS_CS_ImBase::MODIFY;
	advancedConfigurationClonedObj->rdn = ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION;

	/////////////////////////////////
	//cpClusterClonedObj
	/////////////////////////////////
	//Update new value into IMM
	ACS_CS_ImModel tempModel;

	ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s on phase: %i with current profile %i and requested profile: %i with reason: %i",__FUNCTION__,omProfileChange.phase,omProfileChange.omProfileCurrent,omProfileChange.omProfileRequested, omProfileChange.changeReason));

	if ((omProfileChange.phase == ACS_CS_API_OmProfilePhase::Validate) && (omProfileChange.omProfileRequested != omProfileChange.omProfileCurrent))
	{
		start_end_switching = true;
		string omProfileValue = ACS_APGCC::itoa(omProfileChange.omProfileRequested);
		omProfileMgrClonedObj->omProfile = "SWITCHING TO PROFILE " + omProfileValue;
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) SWITCHING TO PROFILE %s in Validate PHASE", omProfileValue.c_str()));

	}
	else
	{
		if ( (omProfileChange.phase == ACS_CS_API_OmProfilePhase::Commit) && (omProfileChange.omProfileRequested == omProfileChange.omProfileCurrent) )
		{
			start_end_switching = true;

			string s_omProfileValue("");
			s_omProfileValue = ACS_APGCC::itoa(omProfileChange.omProfileCurrent);
			omProfileMgrClonedObj->omProfile = "PROFILE " + s_omProfileValue;
			ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) PROFILE %s in Commit PHASE",s_omProfileValue.c_str()));

			//Get OmProfileManagerStruct Object
			ACS_CS_ImBase *baseCpStruct = model->getObject(ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER_STRUCT.c_str());
			if(baseCpStruct)
			{
				ACS_CS_ImOmProfileStruct* omProfileMgrStruct = dynamic_cast<ACS_CS_ImOmProfileStruct*>(baseCpStruct);
				if (omProfileMgrStruct)
				{
					if (omProfileMgrStruct->result == SUCCESS)
					{
						//complete OaM Profile switching
						bool isAlreadyActive = false;
						string activeOmProfileRdn("");
						string activeOmProfileIdValue("");
						string supportedOmProfileRdn("");
						string supportedOmProfileIdValue("");
						string currentCcFileRdn("");string currentRules("");
						string newCcFileRdn("");
						string newCcFileId("");string newRules("");
						string backupCcFileRdn("");
						string backupCcFileId("");
						string newSupportedCcFileRdn("");
						string newSupportedCcFileId("");

						//list of all OmProfile
						set<const ACS_CS_ImBase *> omProfileInstances;
						set<const ACS_CS_ImBase *>::iterator it;
						model->getChildren(ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER, omProfileInstances);

						for (it = omProfileInstances.begin(); it != omProfileInstances.end(); it++)
						{
							const ACS_CS_ImBase* baseOmP = *it;
							const ACS_CS_ImOmProfile* omProfile_instance = dynamic_cast<const ACS_CS_ImOmProfile*>(baseOmP);

							if (omProfile_instance && omProfile_instance->state == ACTIVE_OMP )
							{
								activeOmProfileRdn = omProfile_instance->rdn;
								ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) current ACTIVE OmProfile object: %s ",activeOmProfileRdn.c_str()));

								//already active OmProfile
								activeOmProfileIdValue = ACS_CS_ImUtils::getIdValueFromRdn(activeOmProfileRdn);
								if (activeOmProfileIdValue.compare(s_omProfileValue.c_str()) == 0)
								{
									isAlreadyActive = true;
									ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) change OmProfile of the already ACTIVE OmProfile object: %s ",activeOmProfileRdn.c_str()));

									set<const ACS_CS_ImBase *> ccFileInstances;
									set<const ACS_CS_ImBase *>::iterator it2;
									model->getChildren(activeOmProfileRdn, ccFileInstances);

									for(it2 = ccFileInstances.begin(); it2 != ccFileInstances.end(); it2++)
									{
										const ACS_CS_ImBase* baseCcF = *it2;
										const ACS_CS_ImCcFile *ccFile_new = dynamic_cast<const ACS_CS_ImCcFile *>(baseCcF);
										if (ccFile_new && ccFile_new->state == NEW_CCF)
										{
											newCcFileRdn = ccFile_new->rdn;
											newCcFileId = ACS_CS_ImUtils::getIdValueFromRdn(ccFile_new->ccFileId);
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) new CCF of ACTIVE OmProfile object: %s ",newCcFileRdn.c_str()));
										}
										else if (ccFile_new && ccFile_new->state == CURRENT_CCF)
										{
											currentCcFileRdn = ccFile_new->rdn;
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) current CCF of ACTIVE OmProfile object: %s ",currentCcFileRdn.c_str()));
										}
										else if (ccFile_new && ccFile_new->state == BACKUP_CCF)
										{
											backupCcFileRdn = ccFile_new->rdn;
											backupCcFileId = ACS_CS_ImUtils::getIdValueFromRdn(ccFile_new->ccFileId);
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) backup CCF of ACTIVE OmProfile object: %s ",backupCcFileRdn.c_str()));
										}
									}
								}
								else
								{
									//ACTIVE OmProfile to change in SUPPORTED
									set<const ACS_CS_ImBase *> ccFileInstances;
									set<const ACS_CS_ImBase *>::iterator it2;
									model->getChildren(activeOmProfileRdn, ccFileInstances);

									for(it2 = ccFileInstances.begin(); it2 != ccFileInstances.end(); it2++)
									{
										const ACS_CS_ImBase* baseCcF = *it2;
										const ACS_CS_ImCcFile *ccFile_new = dynamic_cast<const ACS_CS_ImCcFile *>(baseCcF);

										if (ccFile_new && ccFile_new->state == NEW_CCF)
										{
											newCcFileRdn = ccFile_new->rdn;
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) new CCF of ACTIVE OmProfile object: %s ",newCcFileRdn.c_str()));
										}
										else if (ccFile_new && ccFile_new->state == CURRENT_CCF)
										{
											currentCcFileRdn = ccFile_new->rdn;
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) current CCF of ACTIVE OmProfile object: %s ",currentCcFileRdn.c_str()));
										}
										else if (ccFile_new && ccFile_new->state == BACKUP_CCF)
										{
											backupCcFileRdn = ccFile_new->rdn;
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) backup CCF of ACTIVE OmProfile object: %s ",backupCcFileRdn.c_str()));
										}
									}

								}
							}
							else if (omProfile_instance && omProfile_instance->state == SUPPORTED_OMP )
							{
								//SUPPORTED OmProfile to activate
								supportedOmProfileIdValue = ACS_CS_ImUtils::getIdValueFromRdn(omProfile_instance->rdn);
								if (supportedOmProfileIdValue.compare(s_omProfileValue.c_str()) == 0)
								{
									supportedOmProfileRdn = omProfile_instance->rdn;
									ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) SUPPORTED OmProfile object: %s ",supportedOmProfileRdn.c_str()));

									set<const ACS_CS_ImBase *> ccFileInstancesSupp;
									set<const ACS_CS_ImBase *>::iterator it3;
									model->getChildren(supportedOmProfileRdn, ccFileInstancesSupp);

									for(it3 = ccFileInstancesSupp.begin(); it3 != ccFileInstancesSupp.end(); it3++)
									{
										const ACS_CS_ImBase* baseCcF2 = *it3;
										const ACS_CS_ImCcFile *ccFile_new = dynamic_cast<const ACS_CS_ImCcFile *>(baseCcF2);
										if (ccFile_new && ccFile_new->state == NEW_CCF)
										{
											newSupportedCcFileRdn = ccFile_new->rdn;
											newSupportedCcFileId = ACS_CS_ImUtils::getIdValueFromRdn(ccFile_new->ccFileId);
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) new CCF of SUPPORTED OmProfile object: %s ",backupCcFileRdn.c_str()));
										}
									}
								}
							}
						}

						//if the CcFile to activate is already child of active OmProfile
						if (isAlreadyActive)
						{
							//get action type value
							AsyncActionType id;
							readOmProfileActionId(id);

							/////////// ROLLBACK ///////////
							// NEW will be deleted
							// CURRENT --> NEW
							// BACKUP  --> CURRENT

							if (id == ROLLBACK_CCF)
							{
								if (backupCcFileRdn.compare("") !=0 )
								{
									if (newCcFileRdn.compare("") !=0 ){
										ccFile_remove = new ACS_CS_ImCcFile();
										ccFile_remove->rdn = newCcFileRdn;
										ccFile_remove->action = ACS_CS_ImBase::DELETE;
										tempModel.addObject(ccFile_remove);

										ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) DELETE NEW of ACTIVE profile for : %s ",newCcFileRdn.c_str()));
									}
									if (currentCcFileRdn.compare("") !=0 ){
										ACS_CS_ImBase *baseCurrCcf = model->getObject(currentCcFileRdn.c_str());
										ACS_CS_ImCcFile *currCcfClonedObj = dynamic_cast<ACS_CS_ImCcFile *> (baseCurrCcf->clone());
										if (currCcfClonedObj){
											currCcfClonedObj->state = NEW_CCF;
											currCcfClonedObj->action = ACS_CS_ImBase::MODIFY;
											tempModel.addObject(currCcfClonedObj);
										}

										ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) CURRENT->NEW of ACTIVE profile for : %s ",currentCcFileRdn.c_str()));
									}
									if (activeOmProfileRdn.compare("") !=0 ){
										ACS_CS_ImBase *baseActiveOmP = model->getObject(activeOmProfileRdn.c_str());
										ACS_CS_ImOmProfile *activeOmProfileClonedObj = dynamic_cast<ACS_CS_ImOmProfile *> (baseActiveOmP->clone());
										if (activeOmProfileClonedObj){
											omProfileMgrClonedObj->activeCcFile = backupCcFileId;
											activeOmProfileClonedObj->action = ACS_CS_ImBase::MODIFY;
											tempModel.addObject(activeOmProfileClonedObj);
										}

										ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) current reference of ACTIVE profile for : %s with value : %s",activeOmProfileRdn.c_str(),newCcFileId.c_str()));
									}

									ACS_CS_ImBase *baseBckCcf = model->getObject(backupCcFileRdn.c_str());
									ACS_CS_ImCcFile *bckCcfClonedObj = dynamic_cast<ACS_CS_ImCcFile *> (baseBckCcf->clone());
									if (bckCcfClonedObj){
										bckCcfClonedObj->state = CURRENT_CCF;
										bckCcfClonedObj->action = ACS_CS_ImBase::MODIFY;
										tempModel.addObject(bckCcfClonedObj);
									}
									ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) BACKUP->CURRENT of ACTIVE profile for : %s ",backupCcFileRdn.c_str()));

								}
								else
								{
									ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) No change needed, since no BACKUP CCF object found of ACTIVE profile : %s ",currentCcFileRdn.c_str()));
								}
							}

							//////////// CHANGE -ACTIVATE //////////
							// BACKUP will be deleted
							// CURRENT --> BACKUP
							// NEW 	   --> CURRENT
							else
							{

								if (newCcFileRdn.compare("") !=0 )
								{
									if (backupCcFileRdn.compare("") !=0 ){
										ccFile_remove = new ACS_CS_ImCcFile();
										ccFile_remove->rdn = backupCcFileRdn;
										ccFile_remove->action = ACS_CS_ImBase::DELETE;
										tempModel.addObject(ccFile_remove);

										ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) DELETE BACKUP of ACTIVE profile for : %s ",backupCcFileRdn.c_str()));
									}
									if (currentCcFileRdn.compare("") !=0 ){
										ACS_CS_ImBase *baseCurrCcf = model->getObject(currentCcFileRdn.c_str());
										ACS_CS_ImCcFile *currCcfClonedObj = dynamic_cast<ACS_CS_ImCcFile *> (baseCurrCcf->clone());
										if (currCcfClonedObj){
											currCcfClonedObj->state = BACKUP_CCF;
											currCcfClonedObj->action = ACS_CS_ImBase::MODIFY;
											tempModel.addObject(currCcfClonedObj);
										}
										ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) CURRENT->BACKUP of ACTIVE profile for : %s ",currentCcFileRdn.c_str()));
									}
									if (activeOmProfileRdn.compare("") !=0 ){
										ACS_CS_ImBase *baseActiveOmP = model->getObject(activeOmProfileRdn.c_str());
										ACS_CS_ImOmProfile *activeOmProfileClonedObj = dynamic_cast<ACS_CS_ImOmProfile *> (baseActiveOmP->clone());
										if (activeOmProfileClonedObj){
											omProfileMgrClonedObj->activeCcFile = newCcFileId;
											activeOmProfileClonedObj->action = ACS_CS_ImBase::MODIFY;
											tempModel.addObject(activeOmProfileClonedObj);
										}
										ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) current reference of ACTIVE profile for : %s with value : %s",activeOmProfileRdn.c_str(),newCcFileId.c_str()));
									}

									ACS_CS_ImBase *baseNewCcf = model->getObject(newCcFileRdn.c_str());
									ACS_CS_ImCcFile *newCcfClonedObj = dynamic_cast<ACS_CS_ImCcFile *> (baseNewCcf->clone());
									if (newCcfClonedObj){
										newCcfClonedObj->state = CURRENT_CCF;
										newCcfClonedObj->action = ACS_CS_ImBase::MODIFY;
										tempModel.addObject(newCcfClonedObj);
									}
									ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) NEW->CURRENT of ACTIVE profile for : %s ",newCcFileRdn.c_str()));

								}
								else
								{
									ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) No change needed, since no NEW CCF object found of ACTIVE profile : %s ",currentCcFileRdn.c_str()));
								}
							}
						}
						else
						{
							//move OmProfile from ACTIVE to SUPPORTED
							if (newCcFileRdn.compare("") !=0 ){
								ccFile_current = new ACS_CS_ImCcFile();
								ccFile_current->rdn = newCcFileRdn;
								ccFile_current->action = ACS_CS_ImBase::DELETE;
								tempModel.addObject(ccFile_current);
							}
							if (backupCcFileRdn.compare("") !=0 ){
								ccFile_remove = new ACS_CS_ImCcFile();
								ccFile_remove->rdn = backupCcFileRdn;
								ccFile_remove->action = ACS_CS_ImBase::DELETE;
								tempModel.addObject(ccFile_remove);
							}
							if (currentCcFileRdn.compare("") !=0 ){
								ACS_CS_ImBase *baseCurrCcf = model->getObject(currentCcFileRdn.c_str());
								ACS_CS_ImCcFile *currCcfClonedObj = dynamic_cast<ACS_CS_ImCcFile *> (baseCurrCcf->clone());
								if (currCcfClonedObj){
									currCcfClonedObj->state = NEW_CCF;
									currCcfClonedObj->action = ACS_CS_ImBase::MODIFY;
									tempModel.addObject(currCcfClonedObj);
								}
							}
							if (activeOmProfileRdn.compare("") !=0 ){

								ACS_CS_ImBase *baseActiveOmP = model->getObject(activeOmProfileRdn.c_str());
								ACS_CS_ImOmProfile *activeOmProfileClonedObj = dynamic_cast<ACS_CS_ImOmProfile *> (baseActiveOmP->clone());
								if (activeOmProfileClonedObj){
									activeOmProfileClonedObj->state = SUPPORTED_OMP;
									omProfileMgrClonedObj->activeCcFile = "";
									activeOmProfileClonedObj->action = ACS_CS_ImBase::MODIFY;
									tempModel.addObject(activeOmProfileClonedObj);
								}
							}
							if (supportedOmProfileRdn.compare("") !=0 ){
								ACS_CS_ImBase *baseSuppOmP = model->getObject(supportedOmProfileRdn.c_str());
								ACS_CS_ImOmProfile *suppOmProfileClonedObj = dynamic_cast<ACS_CS_ImOmProfile *> (baseSuppOmP->clone());
								if (suppOmProfileClonedObj){
									suppOmProfileClonedObj->state = ACTIVE_OMP;
									omProfileMgrClonedObj->activeCcFile = newSupportedCcFileId;
									suppOmProfileClonedObj->action = ACS_CS_ImBase::MODIFY;
									tempModel.addObject(suppOmProfileClonedObj);
								}
							}
							if (newSupportedCcFileRdn.compare("") !=0 ){
								ACS_CS_ImBase *baseNewCcf = model->getObject(newSupportedCcFileRdn.c_str());
								ACS_CS_ImCcFile *newCcfClonedObj = dynamic_cast<ACS_CS_ImCcFile *> (baseNewCcf->clone());
								if (newCcfClonedObj){
									newCcfClonedObj->state = CURRENT_CCF;
									newCcfClonedObj->action = ACS_CS_ImBase::MODIFY;
									tempModel.addObject(newCcfClonedObj);
								}
							}
						}

						//restore default value of profile scope in OmProfileManager
						omProfileMgrClonedObj->scope = CP_AP;

					}//if success
				}//if (omProfileMgrStruct)
			}//if(baseCpStruct)
		}//if commit
	}

	if (start_end_switching)
	{
		omProfileMgrClonedObj->action = ACS_CS_ImBase::MODIFY;
		omProfileMgrClonedObj->rdn = ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER;

		tempModel.addObject(omProfileMgrClonedObj);
	}

	if (tempModel.addObject(advancedConfigurationClonedObj))
	{
		//Save
		ACS_CS_ImModelSaver saver(&tempModel);
		immDataSynchronized = false;

		if (saver.save(__FUNCTION__) == ACS_CC_SUCCESS)
		{

			while (immDataSynchronized == false) {
				ACE_OS::sleep(1);
				char tracep[512] = {0};
				snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] WAITING FOR DATA SYNCHRONIZATION", __FILE__, __FUNCTION__, __LINE__);
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
			}

			//trace
			char tracep[512] = {0};
			snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] IMM updated with %s", __FILE__, __FUNCTION__, __LINE__, ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
			ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
			return true;
		}
		else
		{
			//trace
			immDataSynchronized = true;
			char tracep[512] = {0};
			snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: cannot save IMM model with %s", __FILE__, __FUNCTION__, __LINE__, ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
			ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));
			return false;
		}
	}
	else
	{
		// Error

		//trace
		char tracep[512] = {0};
		snprintf(tracep, sizeof (tracep) - 1, "[%s::%s@%d] Error: Cannot store object into the model: %s", __FILE__, __FUNCTION__, __LINE__, ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", tracep));

		//delete cloned object since it has not been inserted in the model
		if(ccFile_current) delete ccFile_current;
		if(ccFile_remove) delete ccFile_remove;

		delete omProfileMgrClonedObj;
		delete advancedConfigurationClonedObj;
		return false;
	}

	return true;

}

//*******************************************************
// OM Profile handling: Subscriber asnwers
//*******************************************************

//-----------------------------------------
// handleSetOmProfileNotificationStatus
//-----------------------------------------
int ACS_CS_NEHandler::handleSetOmProfileNotificationStatus (bool success, ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_Set::ReasonType reason)
{
	{
		// Log
		char trace[256] = {0};
		snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d]\n\tsuccess == %d\n\tphase == %d\n\treason == %d\n", __FILE__, __FUNCTION__, __LINE__, success, phase, reason);
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
		std::cout << "DBG: " << trace << std::endl;
	}



	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Handling Set Profile with Phase: %d and Reason : %d ",	__FUNCTION__, __LINE__, phase, reason));
	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
	if (!CS_ProtocolChecker::checkIfAP1())
	{
		// handleSetOmProfile initiates a phase change and should only execute
		// on the MASTER.  For replication purposes, handleInternalSetOmProfile
		// will execute on SLAVEs.
		std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
		return ACS_CS_Protocol::Result_Success;
	}

	// make sure we are out of startup supervision.
	if (!startupProfileSupervisionComplete)
	{
		std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - WARNING: startup ProfileSupervision in NOT Completed" << std::endl;
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_WARN, "[%s@%d] WARNING: startup Profile Supervision in NOT completed!", __FUNCTION__, __LINE__));
		return ACS_CS_Protocol::Result_Success;
	};

	const std::string notifyTypeString = (success ? "setOmProfileNotificationSuccess" : "setOmProfileNotificationFailure");
	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - notifyTypeString:" << notifyTypeString <<std::endl;



	ACS_CS_API_OmProfileChange omProfileChange;
	if (!readOmProfile(omProfileChange))
	{
		char trace[256] = {0};
		snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] readOmProfile failed.", __FILE__, __FUNCTION__, __LINE__);
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
		std::cout << "DBG: " << trace << std::endl;
		return ACS_CS_Protocol::Result_Other_Failure;
	}
	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Profile read: phase(%d) - omProfileCurrent(%d)  ",	__FUNCTION__, __LINE__, omProfileChange.phase, omProfileChange.omProfileCurrent));

	if (phase != omProfileChange.phase)
	{
		// Received a notification for a phase other than the current phase.
		// This could happen if a rollback is initated in a phase before
		// all subscribers have had a chance to respond, so return an error
		// to the application, but continue with current phase change.

		ostringstream eventData;
		eventData << notifyTypeString << " for Phase " << phase << " received in Phase " << omProfileChange.phase << ".";

		ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");

		ACS_CS_EventReporter::instance()->resetAllEvents();
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", eventData.str().c_str()));
		std::cout << "DBG: " << eventData.str() << std::endl;

		if (phase != ACS_CS_API_OmProfilePhase::Commit) return ACS_CS_Protocol::Result_Other_Failure;
	}

	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Internal subsbriberCount: %d ",	__FUNCTION__, __LINE__, subscriberCount));
	if(subscriberCount <= 0)
	{
		std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
		// This is a somewhat unexpected occurence.  We should not receive
		// a notify for current phase when subscriberCount is 0.
		// Return an error, but do not rollback  the current phase change.

		ostringstream eventData;
		eventData << "Unexpected " << notifyTypeString << " for Phase " << phase << " received in phase " << omProfileChange.phase << ".\n";

		if( omProfileChange.changeReason == ACS_CS_API_OmProfileChange::AutomaticProfileAlignment )
		{
			ACS_CS_EVENT(Event_ProfileChangeFailure,
					ACS_CS_EventReporter::Severity_A1,
					"OaM PROFILE NOT ALIGNED IN AP AND CP",
					eventData.str(), "");
			 /*HW99445 start*/
			m_apaAlarmText = eventData.str();
			if(writeAPAalarmdata())
			{
				m_apaAlarm = true;
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_TRACE, "[%s@%d] Successfully written data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
			}
			else
			{
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_ERROR, "[%s@%d] Failed to write data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
			}
			 /*HW99445 stop*/
		}
		else
		{
			ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
			    			"OaM Profile Switch",
			    			eventData.str(), "");
		}

		ACS_CS_EventReporter::instance()->resetAllEvents();

		return ACS_CS_Protocol::Result_Other_Failure;
	}

	subscriberCount--;  // decrement the counter for expected responses in this phase.
	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << " - subscriberCount : " << subscriberCount << std::endl;
	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] There are %d subscribers still to notify", __FUNCTION__, __LINE__,subscriberCount));
	if(success) // Success Notification
	{
		std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
		if (subscriberCount <= 0)
		{
			bool sendUpdate = false;
			// Immediately stop the timer from the current (old) phase.
			cancelOmProfileSupervisionTimer(omProfileChange.phase);

			ostringstream eventData;
			switch (omProfileChange.phase)
			{
			case ACS_CS_API_OmProfilePhase::Validate:
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - VALIDATE phase", __FUNCTION__, __LINE__));

				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] Dispatching VALIDATE PHASE!", __FUNCTION__, __LINE__));
				if (omProfileChange.omProfileRequested == ACS_CS_API_OmProfileChange::UnspecifiedProfile ||
						omProfileChange.aptRequested == ACS_CS_API_OmProfileChange::UnspecifiedProfile ||
						omProfileChange.apzRequested == ACS_CS_API_OmProfileChange::UnspecifiedProfile)
				{
					ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - Error: We've reached the end of the validate phase, apparently without a call to setLocalProfiles from ADH...rollback!!", __FUNCTION__, __LINE__));
					std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;

					// We've reached the end of the validate phase, apparently without a call to
					// setLocalProfiles from ADH...rollback!!
					eventData << "No local profile data received in Phase 0."
							<< " Rolling back OaM Profile phase change.\n"
							<< convertOmProfileToString(omProfileChange);

					if( omProfileChange.changeReason == ACS_CS_API_OmProfileChange::AutomaticProfileAlignment )
					{
						ACS_CS_EVENT(Event_ProfileChangeFailure,
											ACS_CS_EventReporter::Severity_A1,
											"OaM PROFILE NOT ALIGNED IN AP AND CP",
											eventData.str(), "");
						std::cout << "DBG:" << eventData.str() << std::endl;
						 /*HW99445 start*/
						m_apaAlarmText = eventData.str();
						if(writeAPAalarmdata())
						{
							m_apaAlarm = true;
							ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_TRACE, "[%s@%d] Successfully written data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
						}
						else
						{
							ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_ERROR, "[%s@%d] Failed to write data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
						}
						 /*HW99445 stop*/
					}
					else
					{
						ACS_CS_EVENT(Event_ProfileChangeFailure,
								ACS_CS_EventReporter::Severity_Event,
								"OaM PROFILE NOT ALIGNED IN AP AND CP",
								eventData.str(),
								"");
						std::cout << "DBG:" << eventData.str() << std::endl;
					}

					ACS_CS_EventReporter::instance()->resetAllEvents();

					std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
					ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Rolling back to the old OmProfile!", __FUNCTION__, __LINE__));
					rollbackOmProfile(omProfileChange);

					return ACS_CS_Protocol::Result_Other_Failure;
				}

				// Determine the next phase. Transition can happen into any
				// phase ApNotify, CpNotify or Commit
				if(agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::ApNotify) != 0)
				{
					// We have subscribers for apNotify phase...begin the phase.
					ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - We have subscribers for apNotify phase...begin the phase.", __FUNCTION__, __LINE__));
					omProfileChange.phase = ACS_CS_API_OmProfilePhase::ApNotify;
				}
				else
				{
					if (agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::CpNotify) != 0)
					{
						ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - We have subscribers for cpNotify phase...begin the phase.", __FUNCTION__, __LINE__));
						omProfileChange.phase = ACS_CS_API_OmProfilePhase::CpNotify;
					}
					else
					{
						ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - No subscribers ap and cp Notify phases", __FUNCTION__, __LINE__));
						if (ACS_CS_API_OmProfileChange::NechCommandForApgOnly == omProfileChange.changeReason)
						{
							ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - Commit since NechCommandForApgOnly", __FUNCTION__, __LINE__));
							omProfileChange.phase = ACS_CS_API_OmProfilePhase::Commit;
						}
						else
						{
							ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - CP_PROXY IS REQUIRED", __FUNCTION__, __LINE__));
							// CP_PROXY_REQUIRED is expected to be defined as a MACRO in build
							// settings when cpProxy is delivered upon which it is mandatory
							// and must be subscribed.
#ifdef CP_PROXY_REQUIRED
							omProfileChange.phase = ACS_CS_API_OmProfilePhase::CpNotify;
#else
							omProfileChange.phase = ACS_CS_API_OmProfilePhase::Commit;
#endif
						}
					}

					// check if moving to cpNotify phase
					if (ACS_CS_API_OmProfilePhase::CpNotify == omProfileChange.phase)
					{
						// append the skip message to eventData: No subscribers for ApNotify.
						eventData << "No subscribers for Phase 1. ";
					}
					// check if moving to Commit phase and update current values
					else if (ACS_CS_API_OmProfilePhase::Commit == omProfileChange.phase)
					{
						// skip apNotify and begin cpNotify...append action to eventData
						eventData << "No subscribers for Phase 1 or Phase 2. ";

						//rename MmlCommandRoles file
						changeMmlFileStatus(omProfileChange.omProfileCurrent,omProfileChange.omProfileRequested, true, false);

						// Commit new profile values
						omProfileChange.omProfileCurrent = omProfileChange.omProfileRequested;
						omProfileChange.aptCurrent = omProfileChange.aptRequested;
						omProfileChange.apzCurrent = omProfileChange.apzRequested;

						//write in asynch action progress structure
						ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,UNDEF_ACTIONTYPE,SUCCESS);
					}
				}
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Changing from Phase %d to the next Phase to %d", __FUNCTION__, __LINE__, (omProfileChange.phase -1), omProfileChange.phase));
				// Notify subscribers and get actual subscriber count for the new phase.
				sendUpdate = true;

				// Set supervision timer for next phase...30 seconds for commit, 60 seconds otherwise.
				setOmProfileSupervisionTimer(omProfileChange.phase,
						omProfileChange.phase == ACS_CS_API_OmProfilePhase::Commit ? PHASE_COMMIT_TIMEOUT_IN_SECONDS : PHASE_TIMEOUT_IN_SECONDS);

				break;

			case ACS_CS_API_OmProfilePhase::ApNotify:
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) [%s%d] - ApNotify phase", __FUNCTION__, __LINE__));
				// Determine the next phase. Transition can happen into any
				// phase CpNotify or Commit
				if(agentInstance->getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::CpNotify) != 0)
				{
					std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
					// We have subscribers for CpNotify phase...begin the phase.
					omProfileChange.phase = ACS_CS_API_OmProfilePhase::CpNotify;
				}
				else
				{
					std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
					if (ACS_CS_API_OmProfileChange::NechCommandForApgOnly == omProfileChange.changeReason)
					{
						std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
						omProfileChange.phase = ACS_CS_API_OmProfilePhase::Commit;
					}
					else
					{
						std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
						// CP_PROXY_REQUIRED is expected to be defined as a MACRO in build
						// settings when cpProxy is delivered upon which it is mandatory
						// and must be subscribed.
#ifdef CP_PROXY_REQUIRED
						omProfileChange.phase = ACS_CS_API_OmProfilePhase::CpNotify;
#else
						omProfileChange.phase = ACS_CS_API_OmProfilePhase::Commit;
#endif
					}

					std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
					// check if moving to Commit phase and update current values
					if ( omProfileChange.phase == ACS_CS_API_OmProfilePhase::Commit)
					{
						std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
						// skip cpNotify...append action to eventData: No subscribers for Phase CpNotify
						eventData << "No subscribers for Phase 2. ";

						//rename MmlCommandRoles file
						changeMmlFileStatus(omProfileChange.omProfileCurrent,omProfileChange.omProfileRequested, true, false);

						// Commit new profile values
						omProfileChange.omProfileCurrent = omProfileChange.omProfileRequested;
						omProfileChange.aptCurrent = omProfileChange.aptRequested;
						omProfileChange.apzCurrent = omProfileChange.apzRequested;

						//write in asynch action progress structure
						ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,UNDEF_ACTIONTYPE,SUCCESS);
					}
				}
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Changing from the Phase %d to the next Phase to %d", __FUNCTION__, __LINE__, (omProfileChange.phase -1), omProfileChange.phase));
				std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
				// Notify subscribers and get actual subscriber count for the new phase.
				sendUpdate = true;

				// Set supervision timer for next phase...30 seconds for commit, 60 seconds otherwise.
				setOmProfileSupervisionTimer(omProfileChange.phase,	omProfileChange.phase == ACS_CS_API_OmProfilePhase::Commit ? PHASE_COMMIT_TIMEOUT_IN_SECONDS : PHASE_TIMEOUT_IN_SECONDS);

				break;

			case ACS_CS_API_OmProfilePhase::CpNotify:

				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] Dispatching CP NOTIFY PHASE!", __FUNCTION__, __LINE__));
				// Advance to Commit phase
				omProfileChange.phase = ACS_CS_API_OmProfilePhase::Commit;
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Changing from the Phase %d to the next Phase to %d", __FUNCTION__, __LINE__, (omProfileChange.phase -1), omProfileChange.phase));

				//rename MmlCommandRoles file
				changeMmlFileStatus(omProfileChange.omProfileCurrent, omProfileChange.omProfileRequested, true, false);

				// Commit new profile values
				omProfileChange.omProfileCurrent = omProfileChange.omProfileRequested;
				omProfileChange.aptCurrent = omProfileChange.aptRequested;
				omProfileChange.apzCurrent = omProfileChange.apzRequested;

				//write in asynch action progress structure
				ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,UNDEF_ACTIONTYPE,SUCCESS);

				// Notify subscribers and get actual subscriber count for the new phase.
				sendUpdate = true;

				std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
				// Set 30 second supervision timer for final phase.
				setOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Commit, PHASE_COMMIT_TIMEOUT_IN_SECONDS);

				break;

			case ACS_CS_API_OmProfilePhase::Commit:
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_INFO, "[%s@%d] Dispatching COMMIT PHASE!", __FUNCTION__, __LINE__));
				// We're done...reset to Idle phase.
				omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Changing from the Phase %d to the next Phase to %d", __FUNCTION__, __LINE__, (omProfileChange.phase -1), omProfileChange.phase));
				// Clear APA alarm when OaM Profile change is succeeded
				if (omProfileChange.changeReason == ACS_CS_API_OmProfileChange::AutomaticProfileAlignment)
				{
					std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
					ACS_CS_CEASE(Event_ProfileChangeFailure);
				}
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] APA scope value in COMMIT PHASE is %d alarm value is %d", __FUNCTION__, __LINE__,m_ScopeFlag,m_apaAlarm));
				/*HW99445 start*/
				if(m_apaAlarm && m_ScopeFlag){
					m_ScopeFlag =false;
					ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] APA alarm exist,trying to cease", __FUNCTION__, __LINE__));
					struct stat fileStat;
					if(stat(APA_ALARM_FILE.c_str(), &fileStat) < 0)
					{
						if(errno == ENOENT)
						{
							ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] apaAlarm file does not exist nothing to do", __FUNCTION__, __LINE__));
						}
					}
					else if (remove(APA_ALARM_FILE.c_str()) != 0)
					{
						ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] apaAlarm file exist but not deleted", __FUNCTION__, __LINE__));
					}
					else
					{
						ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] apaAlarm file is deleted successfully", __FUNCTION__, __LINE__));
					}
					std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
					m_apaAlarm = false;
					m_apaAlarmText= "";
				}
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] APA scope value after commit phase is %d alarm value is %d", __FUNCTION__, __LINE__,m_ScopeFlag,m_apaAlarm));
				/*HW99445 stop*/
				std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
				// Reset requested profile values to unspecified.
				omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
				omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
				omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
				omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;

				// Clear subscriber count.
				subscriberCount = 0;
				std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;

				break;

			default:
				// Should never be here!
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
						"(%t) ACS_CS_NEHandler::handleSetOmProfileNotificationStatus()\n"
						"Error: Undefined Phase (%d)\n",
						omProfileChange.phase));
				std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << " - ERROR!" << std::endl;
				ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d] Unknown Phase...!!!", __FUNCTION__, __LINE__));

			}  // end of switch


			if (omProfileChange.phase != ACS_CS_API_OmProfilePhase::Idle)
			{
				eventData << "Moving to Phase " << omProfileChange.phase << ".";

				// Commit changes profile values, so log omProfileChange data.
				if (omProfileChange.phase == ACS_CS_API_OmProfilePhase::Commit)
				{
					eventData << "\n\n" << convertOmProfileToString(omProfileChange);
				}
			}
			else
			{
				if(!IMMWriteFailure)
				{
				eventData << "OaM Profile Change completed."
						<< "\n\n" << convertOmProfileToString(omProfileChange);

				//activate->rollback->FAILURE
				//change->roolback(same profile)->FAILURE

				ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,UNDEF_ACTIONTYPE,UNDEF_ACTIONRESULTTYPE);
				}
				else
				{
					IMMWriteFailure =false;
					eventData << "OaM Profile Change Failed"
										<< "\n\n" << convertOmProfileToString(omProfileChange);
					string reasonIMM = "Internal Error";
					ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR,"phase OM PROFILE CHANGE FAILED because of IMM write operation failure"));
					renameMmlFile("", ".new", omProfileChange.omProfileCurrent);
					ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,UNDEF_ACTIONTYPE,FAILURE,reasonIMM);
				}
			}

			ACS_CS_EVENT(Event_ProfileChangeEvent,
					ACS_CS_EventReporter::Severity_Event,
					"OaM Profile Switch",
					eventData.str(),
					"");
			std::cout << "DBG:" << eventData.str() << std::endl;

			ACS_CS_EventReporter::instance()->resetAllEvents();

			//Write the updated phase data to the table.
			int writeRetry;
			lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
			for (writeRetry = 0; ( (writeRetry < 10) && (writeOmProfile(omProfileChange) == false) ) ; ++writeRetry) ACE_OS::sleep(1);
			lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

			if (writeRetry == 10)
				IMMWriteFailure =true;

			if (sendUpdate)
			{
				agentInstance->updatePhaseChange(omProfileChange, subscriberCount);
			}
		}
		else
		{
			// Received a NotificationSuccess from a subscriber, but still
			// expecting more responses.

			ostringstream eventData;
			eventData << "Received " << notifyTypeString << " for phase "
					<< omProfileChange.phase <<". "
					<< "Still expecting " << subscriberCount << " responses.";

			ACS_CS_EVENT(Event_ProfileChangeEvent,
					ACS_CS_EventReporter::Severity_Event,
					"OaM Profile Switch",
					eventData.str(),
					"");
			std::cout << "DBG:" << eventData.str() << std::endl;

			ACS_CS_EventReporter::instance()->resetAllEvents();
		}
	}
	else // Failure notification from subscriber
	{

//		//TODO- VERIFY CORRECTNESS - Immediately stop the timer from the current (old) phase.
//		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d]Deleting OmProfile supervision timer on Phase: %d!", __FUNCTION__, __LINE__, omProfileChange.phase));
//		cancelOmProfileSupervisionTimer(omProfileChange.phase);
		ostringstream eventData;
		string reasonString = convertReasonToString(reason);
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Failure notification from subscriber on Phase: %d due to %s", __FUNCTION__, __LINE__, omProfileChange.phase, reasonString.c_str()));
		eventData << "Received " << notifyTypeString << " for Phase " << omProfileChange.phase << " (reasonCode: " << reason << " = " << reasonString << ").\n";
		// if we are in commit phase, we don't roll back
		if( omProfileChange.phase == ACS_CS_API_OmProfilePhase::Commit )
		{
			ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Notification error. Continuing with Phase 3. Commit!", __FUNCTION__, __LINE__));
			eventData << "Continue with commit.\n\n" << convertOmProfileToString(omProfileChange);

			ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
						    			"OaM Profile Switch",
						    			eventData.str(), "");
			std::cout << "DBG:" << eventData.str() << std::endl;

			ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,UNDEF_ACTIONTYPE,UNDEF_ACTIONRESULTTYPE);
		}
		else
		{
			ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_DEBUG, "[%s@%d]Error: Notification error. Rollback the profile will be started!", __FUNCTION__, __LINE__));
			eventData << "Rolling back OaM Profile phase change.\n\n"
					<< convertOmProfileToString(omProfileChange);

			if (omProfileChange.changeReason == ACS_CS_API_OmProfileChange::AutomaticProfileAlignment)
			{
				ACS_CS_EVENT(Event_ProfileChangeFailure,
						ACS_CS_EventReporter::Severity_A1,
						"OaM PROFILE NOT ALIGNED IN AP AND CP",
						eventData.str(),
						"");
				std::cout << "DBG:" << eventData.str() << std::endl;
				 /*HW99445 start*/
				m_apaAlarmText = eventData.str();
				if(writeAPAalarmdata())
				{
					m_apaAlarm = true;
					ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_TRACE, "[%s@%d] Successfully written data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_ERROR, "[%s@%d] Failed to write data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
				}
				 /*HW99445 stop*/
			}
			else
			{
				ACS_CS_EVENT(Event_ProfileChangeFailure,
						ACS_CS_EventReporter::Severity_Event,
						"OaM Profile Switch",
						eventData.str(),
						"");
				std::cout << "DBG:" << eventData.str() << std::endl;
			}
		}
		ACS_CS_EventReporter::instance()->resetAllEvents();

		// Initiate rollback unless phase is commit
		if( omProfileChange.phase != ACS_CS_API_OmProfilePhase::Commit )
		{
			std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
			ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,UNDEF_ACTIONTYPE,FAILURE,reasonString);

			//rename MmlCommandRoles file
			changeMmlFileStatus(omProfileChange.omProfileCurrent,omProfileChange.omProfileRequested, false, false);

			//rollback to previus profile
			rollbackOmProfile(omProfileChange);
		}
		std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
	}

	// TODO: check APA
	std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
	return ACS_CS_Protocol::Result_Success;
}

//*******************************************************
// OM Profile handling: TIMEOUT
//*******************************************************

//-----------------------------------------
// handleOmProfileSupervisionTimerExpired
//-----------------------------------------
int ACS_CS_NEHandler::handleOmProfileSupervisionTimerExpired (ACS_CS_API_OmProfilePhase::PhaseValue phase)
{

	//guard to exclusive access
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_timer(_profile_mutex);

	//Safe check
    if (phase < ACS_CS_API_OmProfilePhase::Validate || phase > ACS_CS_API_OmProfilePhase::Commit)
    {

    	if (phase == ACS_CS_API_OmProfilePhase::Idle)
    	{
    		// If we find ourselves back in the idle phase after processing a
    		// phase timout, check for and process any queued APA requests.
    		checkQueuedApaProfiles();
    	}

        ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
                "(%t) %s \n Error: Phase timer %d out of valid range",
                __FUNCTION__,
                static_cast<int>(phase)));

        // Note that this isn't exactly an error, as doing a
        // cancel on timer 0 ( idle phase) will just return false
        return -1;
    }

    //Read current omProfile
    ACS_CS_API_OmProfileChange omProfileChange;
    if (!readOmProfile(omProfileChange))
    {
    	char trace[256] = {0};
    	snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] readOmProfile failed. Timeout expired in phase: %d", __FILE__, __FUNCTION__, __LINE__, static_cast<int>(phase));
    	ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
    	std::cout << "DBG: " << trace << std::endl;
    	return ACS_CS_Protocol::Result_Other_Failure;
    }

    //Safe check
    if (phase != omProfileChange.phase)
    {
    	// Ignore timeout

    	// We received a timeout for a phase other than the current phase.
    	// This might happen if we've successfully moved to the next phase
    	// in a separate thread while the thread for the newly expired timer
    	// was waiting to run.  Log and ignore the timeout.

    	ostringstream eventData;
    	eventData << "Supervision Timer expired for Phase " << phase
    			<< " while in Phase " << omProfileChange.phase
    			<< ". Ignoring timer.";

    	ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
    			"OaM Profile Switch",
    			eventData.str(),
    			"");

    	ACS_CS_EventReporter::instance()->resetAllEvents();
    	return ACS_CS_API_NS::Result_Success;
    }


    //Process timeout
    switch (phase)
    {
    case ACS_CS_API_OmProfilePhase::Commit:
		{
			// Supervision timer for Commit phase expired.

			// Clear APA alarm when OaM Profile change is succeeded
			if (omProfileChange.changeReason == ACS_CS_API_OmProfileChange::AutomaticProfileAlignment)
			{
				ACS_CS_CEASE(Event_ProfileChangeFailure);
			}

			// We're done with the switch.  Return to Idle phase.
			omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;

			// Reset requested profile values to unspecified.
			omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
			omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;

			{	// Log
				ostringstream eventData;
				eventData << "Supervision Timer expired for Phase "
						<< phase
						<< ". OaM Profile Change completed."
						<< "\n\n" << convertOmProfileToString(omProfileChange);

				ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event,
						"OaM Profile Switch",
						eventData.str(),
						"");

				ACS_CS_EventReporter::instance()->resetAllEvents();

				// Trace
				char trace[256] = {0};
				snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] Supervision timer for Commit phase expired. Reset to IDLE.", __FILE__, __FUNCTION__, __LINE__);
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
				std::cout << "DBG: " << trace << std::endl;
			}

			// Write action progress structure
			ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,UNDEF_ACTIONTYPE,UNDEF_ACTIONRESULTTYPE);

			// Write updates profile change info to table.
			lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
			writeOmProfile(omProfileChange);
			lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;
		}
    	break;
    default:
		{
			{ // Log
				ostringstream eventData;
				eventData << "Supervision Timer expired for Phase "
						<< omProfileChange.phase
						<< ".  Rolling back OaM Profile phase change.\n"
						<< convertOmProfileToString(omProfileChange);

				if( omProfileChange.changeReason == ACS_CS_API_OmProfileChange::AutomaticProfileAlignment )
				{
					ACS_CS_EVENT(Event_ProfileChangeFailure,
							ACS_CS_EventReporter::Severity_A1,
							"OaM PROFILE NOT ALIGNED IN AP AND CP",
							eventData.str(),
							"");
					 /*HW99445 start*/
					m_apaAlarmText = eventData.str();
					if(writeAPAalarmdata())
					{
						m_apaAlarm = true;
						ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_TRACE, "[%s@%d] Successfully written data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
					}
					else
					{
						ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_ERROR, "[%s@%d] Failed to write data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
					}
					 /*HW99445 stop*/
				}
				else
				{
					ACS_CS_EVENT(Event_ProfileChangeFailure, ACS_CS_EventReporter::Severity_Event,
							"OaM Profile Switch",
							eventData.str(),
							"");
				}

				ACS_CS_EventReporter::instance()->resetAllEvents();

				char trace[256] = {0};
				snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] Supervision timer for current phase expired. Phase: %d. Initiate rollback!!", __FILE__, __FUNCTION__, __LINE__, static_cast<int>(phase));
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s", trace));
				std::cout << "DBG: " << trace << std::endl;
			}

			std::cout << "DBG:" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << std::endl;
			ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,UNDEF_ACTIONTYPE,FAILURE,"Supervision Timer expired");

			//rename MmlCommandRoles file
			changeMmlFileStatus(omProfileChange.omProfileCurrent,omProfileChange.omProfileRequested, false, false);


			//profileHandling->rollbackOmProfile();
			rollbackOmProfile(omProfileChange);
		}
    }

    return 0;
}


bool ACS_CS_NEHandler::importCommandClassificationFile (std::string filename)
{
	bool result = false;
	string pathCcF("");
	string src_pathCcF("");
	string reason("");
	string identity("");
	ActionResultType Actionresult = FAILURE;

	//update report progress
	ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,IMPORT_CCF,NOT_AVAILABLE,"",filename);

	//check presence and extension of command classification file
	if (ACS_CS_ImUtils::getNbiCcFile(pathCcF))
	{
		if(checkExtention(filename.c_str(),".xml"))
		{
			src_pathCcF = pathCcF + "/" + filename;
			if (boost::filesystem::exists(src_pathCcF))
			{
				//////////////////////////////////////////////////////////////////////////////////
				//	extract information from file
				//////////////////////////////////////////////////////////////////////////////////
				if (setXmlFile(filename,pathCcF))
				{
					m_xmlProperties.clear();
					m_xmlProperties.insert(xmlPropertiesMap_t::value_type(FILENAME,filename));

					if (initializeParser())
					{
						//check validity file
						string validFormatFileName = CC_FILENAME_BASE + getXmlProperty(PROFILE) + CC_FILENAME_EXT;
						if (getXmlProperty(FILENAME).compare(validFormatFileName.c_str()) == 0)
						{
							if (getXmlProperty(APZPROFILE).compare("")!=0
									&& getXmlProperty(APTPROFILE).compare("")!=0
									&& getXmlProperty(REVISION).compare("")!=0
									&& getXmlProperty(RULESVERSION).compare("")!=0)
							{

								//////////////////////////////////////////////////////////////////////////////////
								//	create OmProfile object and CcFile object
								//////////////////////////////////////////////////////////////////////////////////

								bool creation = false;
								ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();
								const ACS_CS_ImModel *readModel = ACS_CS_ImRepository::instance()->getModelCopy();

								string omProfileRdn = ACS_CS_ImmMapper::ATTR_OM_PROFILE_ID + "=" + getXmlProperty(PROFILE) + "," + ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER;
								string ccFileRdn = ACS_CS_ImmMapper::ATTR_CCFILE_ID + "=" + getXmlProperty(IDENTITY) + "," + omProfileRdn;
								string omProfileClass = ACS_CS_ImmMapper::CLASS_OM_PROFILE;
								string ccFileClass = ACS_CS_ImmMapper::CLASS_CCFILE;
								//Commented RULESVERSION check for the implementation of TR HV55941 -- XGOUMON
							//	if (isCcFileAlreadyImported(readModel, getXmlProperty(RULESVERSION)))
								//{
									m_xmlProperties.insert(xmlPropertiesMap_t::value_type(IDENTITY,getNextCcFileIdentity(readModel,omProfileRdn)));

									//OmProfile
									ACS_CS_ImOmProfile *omProfile = new ACS_CS_ImOmProfile();
									omProfile->rdn = omProfileRdn;
									omProfile->type = OMPROFILE_T;
									omProfile->omProfileId = ACS_CS_ImmMapper::ATTR_OM_PROFILE_ID + "=" + getXmlProperty(PROFILE);
									omProfile->apzProfile = getXmlProperty(APZPROFILE);
									omProfile->aptProfile = getXmlProperty(APTPROFILE);
									//								omProfile->currentCcFile.clear();

									//check current OmProfile
									if (compareWithCurrentOmProfileValue(atoi(getXmlProperty(PROFILE).c_str())))
										omProfile->state = ACTIVE_OMP;
									else
										omProfile->state = SUPPORTED_OMP;

									omProfile->action = ACS_CS_ImBase::CREATE;

									//CcFile
									ACS_CS_ImCcFile *ccFile = new ACS_CS_ImCcFile();
									ccFile->rdn = ccFileRdn;
									ccFile->type = CCFILE_T;
									ccFile->ccFileId = ACS_CS_ImmMapper::ATTR_CCFILE_ID + "=" + getXmlProperty(IDENTITY);
									ccFile->state = NEW_CCF;
									ccFile->rulesVersion = getXmlProperty(RULESVERSION);
									ccFile->action = ACS_CS_ImBase::CREATE;

									ACS_CS_ImBase *baseOmProfile = 0;
									ACS_CS_ImOmProfile *theOmProfile = 0;

									baseOmProfile = readModel->getObject(omProfileRdn.c_str());
									if (baseOmProfile)
										theOmProfile = dynamic_cast<ACS_CS_ImOmProfile*>(baseOmProfile);

									ACS_CS_ImBase *baseCcFile = 0;
									ACS_CS_ImCcFile *theCcFile = 0;

									baseCcFile = readModel->getObject(ccFileRdn.c_str());
									if (baseCcFile)
										theCcFile = dynamic_cast<ACS_CS_ImCcFile*>(baseCcFile);
									if(NULL != theOmProfile)	//the OmProfile already exists, we do not need to add it
									{
										ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
												"(%t) %s the OmProfile object already exists", __FUNCTION__));

										if (NULL != theCcFile)	//the CcFile already exists, we do not need to add it
										{
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE,
													"(%t) %s the CcFile object already exists", __FUNCTION__));
											delete ccFile;

										}else{
											//check if there is already a CcFile object marked as NEW
											string dn_ccFile_remove("");
											set<const ACS_CS_ImBase *> ccFileInstances;
											set<const ACS_CS_ImBase *>::iterator it;
											readModel->getChildren(omProfileRdn, ccFileInstances);

											for(it = ccFileInstances.begin(); it != ccFileInstances.end(); it++)
											{
												const ACS_CS_ImCcFile *ccFile_new = dynamic_cast<const ACS_CS_ImCcFile *>(*it);
												if (ccFile_new && ccFile_new->state == NEW_CCF)
												{
													dn_ccFile_remove = ccFile_new->rdn;
													break;
												}
											}

											if (!dn_ccFile_remove.empty()){
												ACS_CS_ImCcFile *ccFile_remove = new ACS_CS_ImCcFile();
												ccFile_remove->rdn = dn_ccFile_remove;
												ccFile_remove->action = ACS_CS_ImBase::DELETE;
												tempModel->addObject(ccFile_remove);
											}
											tempModel->addObject(ccFile);
											creation = true;
										}
										delete omProfile;
									}else{
										// add the object
										tempModel->addObject(omProfile);
										creation = true;
										delete ccFile;
									}

									//check if there is an object to create
									if (creation){
										ACS_CS_ImModelSaver saver(tempModel);
										ACS_CC_ReturnType result_action = saver.save("AddImmOmProfile");
										if (result_action == ACS_CC_SUCCESS){
											Actionresult = SUCCESS;
											result = true;
										}
										else{
											reason = "Failed to create CcFile";
											ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: Failed to create CcFile", __FUNCTION__));
										}
									}
								/* Commented below code for the implementation of TR HV55941--XGOUMON
								}else{
									//CcFile is already imported
									reason = "CcFile is already imported";
									ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: CcFile is already imported ", __FUNCTION__));
								}*/

								delete readModel;

							}else{
								//Missing field
								reason = "Invalid CcFile, missing fields";
								ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: Invalid CcFile, missing fields ", __FUNCTION__));
							}
						}else{
							//Invalid format file
							reason = "Invalid CcFile format";
							ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: CcFile Invalid format file ", __FUNCTION__));
						}
					}else{
						//PARSING failure
						reason = "CcFile parsing failed";
						ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: CcFile parsing failed ", __FUNCTION__));
					}
				}
			}else{
				//file not found
				reason = "CcFile not found";
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: CcFile file not found ", __FUNCTION__));
			}
		}else{
			//invalid file extension
			reason = "Invalid CcFile extension";
			ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: invalid file extension ", __FUNCTION__));
		}
	}

	//update report progress
	ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,IMPORT_CCF,Actionresult,reason,filename);

	return result;
}


ACS_CS_NEHandler::SaxHandler::SaxHandler (const std::string &xmlFilePath, propertiesMap_t &xmlProperties)
      : m_xmlProperties(xmlProperties),m_xmlFilePath(xmlFilePath)
{
}

ACS_CS_NEHandler::SaxHandler::~SaxHandler()
{
}

 void ACS_CS_NEHandler::SaxHandler::startElement (const XMLCh* const  name, AttributeList &attributes)
{
	char* elem = XMLString::transcode(name);
	std::string element(elem);

	if (element == "CmdRules")
	{
		for (XMLSize_t i = 0; i < attributes.getLength(); i++)
		{
			char* nm = XMLString::transcode(attributes.getName(i));
			std::string name(nm);
			char* vl = XMLString::transcode(attributes.getValue(i));
			std::string value(vl);

			if (name == "Profile")
			{
				m_xmlProperties.insert(propertiesMap_t::value_type(PROFILE,value));
			}
			else if (name == "APTProfile")
			{
				m_xmlProperties.insert(propertiesMap_t::value_type(APTPROFILE,value));
			}
			else if (name == "APZProfile")
			{
				m_xmlProperties.insert(propertiesMap_t::value_type(APZPROFILE, value));
			}
			else if (name == "RulesVersion")
			{
				m_xmlProperties.insert(propertiesMap_t::value_type(RULESVERSION, value));
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: xml error: problem in CmdRules ", __FUNCTION__));
			}

			XMLString::release(&vl);
			XMLString::release(&nm);
		}
	}
	else 
	{
		for (XMLSize_t i = 0; i < attributes.getLength(); i++)
		{
			char* nm = XMLString::transcode(attributes.getName(i));
			std::string name(nm);
			char* vl = XMLString::transcode(attributes.getValue(i));
			std::string value(vl);

			if (name == "release")
			{
				m_xmlProperties.insert(propertiesMap_t::value_type(REVISION,value));
			}
			else
			{

				ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s Error: xml error: problem in MSC ", __FUNCTION__));

			}
			XMLString::release(&vl);
			XMLString::release(&nm);

		}
	}
	XMLString::release(&elem);
}


 InputSource * 	ACS_CS_NEHandler::SaxHandler::StaticEntityResolver::resolveEntity (const XMLCh *const /*publicId*/, const XMLCh *const /*systemId*/)
{
	 return new LocalFileInputSource(CharToXMLCh(DTD_FILE_PATH.c_str()));
}

 void ACS_CS_NEHandler::SaxHandler::warning (const SAXParseException &/*exception*/)

{

}

 void ACS_CS_NEHandler::SaxHandler::error (const SAXParseException &/*exception*/)
{

}

 void ACS_CS_NEHandler::SaxHandler::fatalError (const SAXParseException &/*exception*/)
{
}

 bool ACS_CS_NEHandler::initializeParser ()
 {
	 bool errorOccurred = false;
	 try
	 {
		 XMLPlatformUtils::Initialize();
	 }
	 catch (const XMLException& e)
	 {
		 ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s XML Exception. Cannot initialize Xerces parser \n", __FUNCTION__));
		 return false;
	 }

	 SAXParser* parser = new SAXParser;
	 parser->setDoNamespaces( true ); /* enable namespace processing */
	 parser->setDoSchema( true );     /* enable schema processing */
	 parser->setValidationScheme( SAXParser::Val_Auto );  /* parser auto validates */
	 parser->setValidationSchemaFullChecking( true ); /* enable full schema checking */

	 //  Create our SAX handler object and install it on the parser
	 SaxHandler handler(this->m_xmlFile, this->m_xmlProperties);
	 parser->setDocumentHandler(&handler);
	 parser->setErrorHandler(&handler);



	 EntityResolver* pResolver = NULL;
	 pResolver = new  SaxHandler::StaticEntityResolver();
	 parser->setEntityResolver(pResolver);


	 try
	 {
		 parser->parse(this->m_xmlFile);
	 }
	 catch (const OutOfMemoryException& e)
	 {
		 ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s  OutOfMemoryException. Cannot parse xml file %s", __FUNCTION__,m_xmlFile));
		 errorOccurred = true;
	 }
	 catch (const XMLException& e)
	 {
		 ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s XMLException. Cannot parse xml file %s", __FUNCTION__,m_xmlFile));
		 errorOccurred = true;
	 }
	 catch (...)
	 {
		 ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t)%s GenericException got parsing xml file %s", __FUNCTION__,m_xmlFile));
		 errorOccurred = true;
	 }

	 delete pResolver;
	 delete parser;

	 // And call the termination method
	 XMLPlatformUtils::Terminate();

	 return (!errorOccurred);
 }



 bool ACS_CS_NEHandler::setXmlFile (std::string filenameXml, std::string folder)
 {
 	bool result = false;
 	string completePath = folder + "/" + filenameXml;

 	std::vector<std::string> xmlfiles;
 	xmlfiles.push_back(completePath);

 	if(xmlfiles.size()>0)
 	{
 		snprintf(m_xmlFile, sizeof(m_xmlFile) - 1, "%s", xmlfiles[0].c_str());
 		result = true;
 	}
 	return result;
 }

 std::string ACS_CS_NEHandler::getXmlProperty (XmlProperties property)
 {
	xmlPropertiesMap_t::iterator it = m_xmlProperties.find(property);
    if (it == m_xmlProperties.end())
    {
       return std::string();
    }

    return (*it).second;
 }

bool ACS_CS_NEHandler::compareWithCurrentOmProfileValue (int reuqestedOmProfile) const
 {
	bool result = false;

	int currentOmProfile = getCurrentOmProfileValue();

 	if (currentOmProfile == reuqestedOmProfile)
 		result = true;
 	return result;
 }

bool ACS_CS_NEHandler::exportCommandClassificationFile()
{
	bool result = false;
	string src_pathCcF("");
	string src_CcFileName("");
	string reason("");
	ActionResultType Actionresult = FAILURE;

	//update report progress
	ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,EXPORT_CCF);

	//check presence of command classification file
	if (checkForMMLCommandRulesFile(src_pathCcF, src_CcFileName, CURRENT_CCF))
	{
		if (copyMmlFileToNbiFolder(src_CcFileName))
		{
			Actionresult = SUCCESS;
			result = true;
		}
		else{
			reason = "Failed to copy file";
		}
	}
	else
	{
		//invalid file extension
		reason = "CcFile not found";
	}

	//update report progress
	ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,EXPORT_CCF,Actionresult,reason);
	return result;
}


bool ACS_CS_NEHandler::moveMmlFileToAdhFolder(bool remove)
{
	bool result = true;
	string pathCcF("");
	string from("");
	string to("");

	if (ACS_CS_ImUtils::getNbiCcFile(pathCcF))
	{
		from = pathCcF + "/" + getXmlProperty(FILENAME);
		to =  getCcfPath() + getXmlProperty(FILENAME) + ".new";

		if (boost::filesystem::exists(from))
		{
			if (boost::filesystem::exists(to))
			{
				boost::filesystem::remove(to);
			}

			try
			{
				//copy file
				boost::filesystem::copy_file(from, to);
			}
			catch (const boost::filesystem::filesystem_error& e)

			{
				std::cerr << "Error: " << e.what() << std::endl;
				result = false;
			}

			if(result && remove)
			{
				try
				{
					boost::filesystem::remove(from);
				}
				catch (const boost::filesystem::filesystem_error& e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
					result = false;
				}

			}

		}
		else
			result = false;
	}
	return result;
}

bool ACS_CS_NEHandler::copyMmlFileToNbiFolder(string filename)
{
	bool result = true;
	string pathCcF("");
	string from("");
	string to("");

	if (ACS_CS_ImUtils::getNbiCcFile(pathCcF))
	{
		from = getCcfPath() + filename;
		to = pathCcF + "/" + filename;
		if (boost::filesystem::exists(from))
		{
			if (boost::filesystem::exists(to))
			{
				boost::filesystem::remove(to);
			}

			try
			{
				//copy file
				boost::filesystem::copy_file(from, to);
			}
			catch (const boost::filesystem::filesystem_error& e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
				result = false;
			}

		}
		else
			result = false;
	}
	return result;
}

bool ACS_CS_NEHandler::checkExtention(const char* file, const char* type_file)
{
	bool result;
	result = false;
	char* fileCheck = strndup(file+strlen(file)-strlen(type_file), strlen(type_file));
	if (strcmp(fileCheck, type_file) == 0)
	{
		result = true;
	}
	free(fileCheck);

	return result;
}



//*******************************************************
// OM Profile handling: HELPER FUNCTIONS
//*******************************************************

std::string ACS_CS_NEHandler::convertReasonToString (ACS_CS_API_Set::ReasonType reason)
{
	string reasonString;
	switch(reason)
	{
	case ACS_CS_API_Set::ClusterSessionLockOngoing:
		reasonString = "Cluster Session Lock Ongoing";
		break;
	case ACS_CS_API_Set::CommandClassificationMissing:
		reasonString = "Command Classification File Missing";
		break;
	case ACS_CS_API_Set::InvalidCommandClassificationFile:
		reasonString = "Invalid Command Classification File";
		break;
	case ACS_CS_API_Set::GeneralFailure:
		reasonString = "General Failure";
		break;
	default:
		reasonString = "Unknown Reason";
		break;
	}

	return reasonString;
}

std::string ACS_CS_NEHandler::convertOmProfileToString (const ACS_CS_API_OmProfileChange& omProfile)
{
   ostringstream omProfileData;
   omProfileData << "Current OaM Profile: " << omProfile.omProfileCurrent << "\n"
      << "Current APT Profile: " << omProfile.aptCurrent << "\n"
      << "Current APZ Profile: " << omProfile.apzCurrent << "\n\n"
      << "Requested OaM Profile: ";

   if (omProfile.omProfileRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.omProfileRequested;
   }

   omProfileData << "\n" << "Requested APT Profile: ";

   if (omProfile.aptRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.aptRequested;
   }

   omProfileData << "\n" << "Requested APZ Profile: ";

   if (omProfile.apzRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.apzRequested;
   }


   omProfileData << "\n" << "Change Reason: ";

   // Note: ACS_CS_API_OmProfileChange_R1 object should get a ".getChangeReasonName()"
   // (1) The way it is now, if changeReason is expanded, this code location is easily missed...
   // (2) the following switch block would just become a one-liner where ever this is used:
   //     omProfileData << omProfile.getChangeReasonName();

   switch (omProfile.changeReason)
   {
      case ACS_CS_API_OmProfileChange::NoChange:
         omProfileData << "NoChange";
         break;
      case ACS_CS_API_OmProfileChange::NechCommand:
         omProfileData << "NechCommand";
         break;
      case ACS_CS_API_OmProfileChange::AutomaticProfileAlignment:
         omProfileData << "AutomaticProfileAlignment";
         break;
      case ACS_CS_API_OmProfileChange::NechCommandForApgOnly:
         omProfileData << "NechCommandOnlyAPG";
         break;
      default:
         omProfileData << "Unknown";
   }

   omProfileData << "(" << omProfile.changeReason << ")\n";


   omProfileData << "Queued APT Profile: ";

   if (omProfile.aptQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.aptQueued;
   }
   omProfileData << "\n"
      << "Queued APZ Profile: ";

   if (omProfile.apzQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.apzQueued;
   }

   return omProfileData.str();

}


bool ACS_CS_NEHandler::checkQueuedApaProfiles ()
{
    ACS_CS_API_OmProfileChange omProfileData;
    readOmProfile(omProfileData);

    if (omProfileData.aptQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile &&
        omProfileData.apzQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
    {
        // We can initiate the queued APA profile change.
        omProfileData.changeReason = ACS_CS_API_OmProfileChange::AutomaticProfileAlignment;

        // Transfer queued values to requested values.
        omProfileData.aptRequested = omProfileData.aptQueued;
        omProfileData.apzRequested = omProfileData.apzQueued;

        // Clear previously queued values now that they have been copied to requested.
        omProfileData.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
        omProfileData.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;

        // Check if we need to initiate APA. If the current and requested profiles
        // are same then ignore APA.
        if (omProfileData.aptCurrent == omProfileData.aptRequested &&
            omProfileData.apzCurrent == omProfileData.apzRequested)
        {
            return ignoreApaProfileRequest(omProfileData);
        }

        // Move to validate phaseACS_CS_API_ProfileHandling
        omProfileData.phase = ACS_CS_API_OmProfilePhase::Validate;

        int newProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
        if (!findMmlFileFolder(newProfileRequested,omProfileData.apzRequested,omProfileData.aptRequested))
        {
        	ostringstream eventData;
        	eventData << "Command Classification File Missing, Queued Automatic Profile Alignment .\n";
        	ACS_CS_EVENT(Event_ProfileChangeFailure,
        			ACS_CS_EventReporter::Severity_A1,
					"OaM PROFILE NOT ALIGNED IN AP AND CP",
					eventData.str(), "");
        	 /*HW99445 start*/
        	m_apaAlarmText = eventData.str();
        	if(writeAPAalarmdata())
        	{
        		m_apaAlarm = true;
        		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_TRACE, "[%s@%d] Successfully written data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
        	}
        	else
        	{
        		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE,LOG_LEVEL_ERROR, "[%s@%d] Failed to write data into /data/acs/data/cs/apaAlarm", __FUNCTION__, __LINE__));
        	}
        	 /*HW99445 stop*/
        	ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] %s", __FUNCTION__, __LINE__, eventData.str().c_str()));
        	// Failed the APA profile request.
        	return ACS_CS_Protocol::Result_Missing_File;
        }

        omProfileData.omProfileRequested = newProfileRequested;

        //rename MmlCommandRoles file
        changeMmlFileStatus(omProfileData.omProfileCurrent, newProfileRequested, false, true);

        // write data to the table
        lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
        writeOmProfile(omProfileData);
        lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

        // Notify Subscribers and continue with phase change.
        subscriberCount = 0;

        // Notify subscribers and get actual subscriber count for the phase.
        agentInstance->updatePhaseChange(omProfileData, subscriberCount);

        // Start Phase 0 Supervision Timer for 60 seconds
        setOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Validate, PHASE_TIMEOUT_IN_SECONDS);

        ostringstream eventData;
        eventData << "APA Profile change initiated." << "\n\n"
                  << convertOmProfileToString(omProfileData);

        ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");

        ACS_CS_EventReporter::instance()->resetAllEvents();
    }

    return true;
}


bool ACS_CS_NEHandler::ignoreApaProfileRequest (ACS_CS_API_OmProfileChange& omProfileChange)
{
	bool result = true;
    ostringstream eventData;

    eventData << "APA Profile change request is ignored since requested profiles are same as current.\n\n"
              << convertOmProfileToString(omProfileChange);

    ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");
    ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] %s", __FUNCTION__, __LINE__, eventData.str().c_str()));
    ACS_CS_EventReporter::instance()->resetAllEvents();

    // We're done...reset to Idle phase.
    omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;

    // Reset requested profile values to unspecified.
    omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
    omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
    omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
    omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;


    lock->start_writing(); std::cout << "LOCK ACQUIRED" <<std::endl;
    result = writeOmProfile(omProfileChange);
    lock->stop_writing(); std::cout << "LOCK RELEASED" <<std::endl;

    lock->start_writing(); std::cout << __LINE__ << " LOCK ACQUIRED" <<std::endl;
    ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,CHANGE_OM_PROFILE,FAILURE,"APA Profile change request is ignored since requested profiles are same as current.");
    lock->stop_writing(); std::cout << __LINE__<< " LOCK RELEASED" <<std::endl;

   return result;
}


bool ACS_CS_NEHandler::removeSupportedOmProfiles()
{
	bool result = true;
	bool saveModel = false;
	string reason("");
	std::vector<string> supportedProfiles;
	ActionResultType Actionresult = FAILURE;

	//update report progress
	ACS_CS_API_Util_Internal::writeAsyncActionStruct(RUNNING,REMOVE_OM_PROFILES);

	ACS_CS_ImModel* tmpModel = new ACS_CS_ImModel();
	if (!tmpModel)
		return false;

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();

	if (!model)
		return false;

	set<const ACS_CS_ImBase *> omProfileInstances;
	set<const ACS_CS_ImBase *>::iterator it;
	model->getChildren(ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER, omProfileInstances);


	for (it = omProfileInstances.begin(); it != omProfileInstances.end(); it++) {
		const ACS_CS_ImBase* baseOmP = *it;
		const ACS_CS_ImOmProfile* omProfile_remove = dynamic_cast<const ACS_CS_ImOmProfile*>(baseOmP);

		if (omProfile_remove && omProfile_remove->state == SUPPORTED_OMP ) {
			cout << "Found OmProfile to delete " << omProfile_remove->rdn << endl;
			ACS_CS_ImOmProfile* remOmp = new ACS_CS_ImOmProfile(*omProfile_remove);
			remOmp->action = ACS_CS_ImBase::DELETE;
			supportedProfiles.push_back(ACS_CS_ImUtils::getIdValueFromRdn(omProfile_remove->rdn));
			tmpModel->addObject(remOmp);
			saveModel = true;
		}
	}

	if (saveModel) {
		ACS_CS_ImModelSaver* saver = new ACS_CS_ImModelSaver(tmpModel);
		if (saver) {
			ACS_CC_ReturnType result_action = saver->save("RemoveSupportedOmProfiles");
			if (result_action == ACS_CC_SUCCESS)
			{
				Actionresult = SUCCESS;
				std::vector<string>::iterator it;
				for (it=supportedProfiles.begin(); it<supportedProfiles.end(); it++)
				{
					ostringstream mmlFileNameStream;
					mmlFileNameStream << CC_FILENAME_BASE << *it << CC_FILENAME_EXT << ".new";
					string mmlFile = getCcfPath() + mmlFileNameStream.str();

					if (boost::filesystem::exists(mmlFile))
					{
						boost::filesystem::remove(mmlFile);
					}

				}
			}
			else
				reason = "Failed to remove supported Profiles";

			delete saver;
		}
	}
	else
	{
		//No supported Profile imported
		reason = "No supported Profile imported";
	}

	delete tmpModel;
	delete model;

	//update report progress
	ACS_CS_API_Util_Internal::writeAsyncActionStruct(FINISHED,REMOVE_OM_PROFILES,Actionresult,reason);

	return result;
}

bool ACS_CS_NEHandler::createCommandClassificationFolder ()
{
	bool result = true;
	string BrfFilePath("");
	string BrfPathValue("");

	BrfFilePath = BRF_PATH + BRF_CONFIG_FILENAME;
	//check if the BRF file exists
	if (boost::filesystem::exists(BrfFilePath))
	{
		//load path value
		ifstream inFile (BrfFilePath.c_str(), ios::in);
		inFile >> BrfPathValue;
		if (boost::filesystem::is_directory(BrfPathValue))
		{
			string CsFolder = BrfPathValue + "/" + BRF_CS_DIR;
			if (!boost::filesystem::is_directory(BrfFilePath))
			{
				if (ACS_APGCC::create_directories(CsFolder.c_str(), ACCESSPERMS) == -1)
				{
					ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s Error: Failed to create the folder: %s", __FUNCTION__, CsFolder.c_str()));
					result = false;
				}
			}
			string CcFileFolder = BrfPathValue + "/" + BRF_CS_DIR + "/" + BRF_CCF_DIR;
			if (!boost::filesystem::is_directory(CcFileFolder))
			{
				if (ACS_APGCC::create_directories(CcFileFolder.c_str(), ACCESSPERMS)==-1)
				{
					ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s Error: Failed to create the folder: %s",__FUNCTION__, CcFileFolder.c_str()));
					result = false;
				}
			}

			m_ccfilePath = CcFileFolder + "/";

		}
	}
	else{
		ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "(%t) %s Error: Could not find BRF config file", __FUNCTION__));
		result = false;
	}

	return result;
}

int ACS_CS_NEHandler::getCurrentOmProfileValue () const
 {
 	lock->start_reading();
 	ACS_CS_API_OmProfileChange omProfileData;
 	ACS_CS_API_Util_Internal::readOmProfile(NULL,omProfileData);
 	int currentOmProfile = static_cast<int>(omProfileData.omProfileCurrent);
 	lock->stop_reading();

 	return currentOmProfile;
 }

bool ACS_CS_NEHandler::renameMmlFile(string stateFrom, string stateTo, int profile)
{
	bool result = true;
	string fileName("");
	string from("");
	string to("");

	ostringstream mmlFileNameStream;
	mmlFileNameStream << CC_FILENAME_BASE << profile << CC_FILENAME_EXT;
	fileName = mmlFileNameStream.str();

	from = getCcfPath() + fileName + stateFrom;
	std::cout << __FUNCTION__ << " from: " << from.c_str() << std::endl;
	to =  getCcfPath() + fileName + stateTo;
	std::cout << __FUNCTION__ << " to: " << to.c_str() << std::endl;

	if (boost::filesystem::exists(from))
	{
		if (boost::filesystem::exists(to))
		{
			boost::filesystem::remove(to);
		}

		try
		{
			//copy file
			boost::filesystem::copy_file(from, to);
		}
		catch (const boost::filesystem::filesystem_error& e)

		{
			std::cerr << "Error: " << e.what() << std::endl;
			result = false;
		}

		if(result)
		{
			try
			{
				boost::filesystem::remove(from);
			}
			catch (const boost::filesystem::filesystem_error& e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
				result = false;
			}

		}

	}
	else
	{
		result = false;
	}

	return result;
}


bool ACS_CS_NEHandler::readOmProfileActionId (AsyncActionType &actionId) const
{
   bool result = false;

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImOmProfileStruct *omProfileStruct = reinterpret_cast<ACS_CS_ImOmProfileStruct *>(model.getObject(ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER_STRUCT.c_str()));

   if (omProfileStruct != NULL)
   {
	  actionId = omProfileStruct->actionId;
      result = true;
   }

   return result;
}

void ACS_CS_NEHandler::changeMmlFileStatus (int profile_current, int profile_requested, bool success, bool starting)
{
	AsyncActionType id;
	if (readOmProfileActionId(id))
	{
		//at first time ADH needs to validate the new MmlCommandRules file.
		//the previous MmlCommandRules is copied with 'tmp' extension
		//the MmlCommandRules file that has to be validate is renamed removing the 'bck' or 'new' extension.
		if (starting)
		{
			renameMmlFile("", ".tmp", profile_current);
			if (id == ROLLBACK_CCF)	renameMmlFile(".bck", "", profile_requested);
			else	renameMmlFile(".new", "", profile_requested);
		}
		else
		{
			//In case of success the 'tmp' MmlCommandRules becomes 'bck', if is given an activation request, 'new' otherwise.
			//The folder is cleared from all unused files and the 'tmp' MmlCommandRules becomes 'new'.
			if (success)
			{
				//in case of ACTIVATE or CHANGE action on an OaMProfile already active.
				if (profile_current == profile_requested && id != ROLLBACK_CCF) renameMmlFile(".tmp", ".bck", profile_requested);

				cleanMmlFileFolder(profile_requested);
			}
			else
			{
				//In case of failure the MmlCommandRules files have been restored.
				if (id == ROLLBACK_CCF)	renameMmlFile("", ".bck", profile_requested);
				else	renameMmlFile("", ".new", profile_requested);

				if (id == CHANGE_OM_PROFILE)	renameMmlFile(".tmp", "", profile_current);
				else	renameMmlFile("tmp", "", profile_requested);
			}
		}
	}

}



bool ACS_CS_NEHandler::cleanMmlFileFolder(int profile)
{
	bool result = true;

	if (profile == static_cast<int>(ACS_CS_API_OmProfileChange::UnspecifiedProfile))
	{
		profile = getCurrentOmProfileValue();
	}

	fs::recursive_directory_iterator it(getCcfPath());
	fs::recursive_directory_iterator endit;
	while(it != endit)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName	= it->path().filename().c_str();
			cout << __FUNCTION__ << " filename: "<< fileName.c_str() << endl;

			if (checkExtention(fileName.c_str(),".xml.tmp"))
			{
				//MmlCommandRules_ sProfile .xml.tmp
				string sProfile = fileName.substr(fileName.rfind("_") +1, fileName.rfind(".xml"));
				int fileProfile = atoi(sProfile.c_str());
				renameMmlFile(".tmp", ".new", fileProfile);

			}
			else if (checkExtention(it->path().filename().c_str(),".xml.bck"))
			{
				ostringstream mmlFileNameStream;
				mmlFileNameStream << CC_FILENAME_BASE << profile << CC_FILENAME_EXT << ".bck";
				string mmlFileName = mmlFileNameStream.str();

				if (fileName.compare(mmlFileName.c_str()) != 0)
				{
					string toRemove = getCcfPath() + fileName;
					boost::filesystem::remove(toRemove.c_str());
				}
			}
		}
		++it;
	}

	return result;
}


bool ACS_CS_NEHandler::getCcFileIdentity(string pathFile, string &identity)
{
	//  not used anymore due to OSU 3.0 to limit the size of the CcFile RDN

	bool result = false;

	bool pFound = false;
	bool rFound = false;
	int iFound = 0;
	const char* product = "<!-- DOCUMENT NO -->";
	const char* revision = "<!-- REVISION -->";
	const char* filename   = pathFile.c_str();
	string sProduct("");
	string sRevision("");
	string t("");

	ifstream f( filename );
	if (!f)
	{
		cerr << "Could not open file " << filename << endl;
		return result;
	}

	while (getline( f, t ))
	{
		if (iFound == 2) break;

		if (pFound){
			removeInvalidChars(t);
			sProduct = t;
			pFound = false;
			iFound++;
		}
		if (rFound){
			removeInvalidChars(t);
			sRevision = t;
			rFound = false;
			iFound++;
		}

		if (t.compare(product) == 0)	pFound = true;
		if (t.compare(revision) == 0)	rFound = true;

		t.clear();
	}

	f.close();


	if (iFound == 2 && ((sProduct.compare("") != 0) && (sRevision.compare("") != 0)))
	{
			identity = sProduct + "_" + sRevision;
			cout << "identity: " << identity << endl;
			result = true;
	}

	return result;
}


bool ACS_CS_NEHandler::removeInvalidChars(std::string &value)
{
	bool res = true;

	const char *invalidChar = "<>!- ";
	std::string::size_type pos;

	while((pos = value.find_last_of(invalidChar)) != std::string::npos)
	{
		value = value.erase(pos,1);
	}

	return res;
}

bool ACS_CS_NEHandler::findMmlFileFolder(int &omProfile, int apzProfile, int aptProfile)
{
	bool result = false;

	fs::recursive_directory_iterator it(getCcfPath());
	fs::recursive_directory_iterator endit;
	while(it != endit)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName	= it->path().filename().c_str();
			cout << __FUNCTION__ << " filename: "<< fileName.c_str() << endl;

			if (setXmlFile(fileName,getCcfPath()))
			{
				m_xmlProperties.clear();
				if (initializeParser())
				{
					if (getXmlProperty(APZPROFILE).compare(ACS_APGCC::itoa(apzProfile)) ==0
							&& getXmlProperty(APTPROFILE).compare(ACS_APGCC::itoa(aptProfile)) ==0 )
					{
						string sProfile = getXmlProperty(PROFILE);
						omProfile = atoi(sProfile.c_str());
						ACS_CS_TRACE((ACS_CS_NEHandler_TRACE, "[%s@%d] with profile:%d ", __FUNCTION__, __LINE__, omProfile));
						result = true;
						break;
					}
				}
			}
		}
		++it;
	}

	return result;
}

bool ACS_CS_NEHandler::isLocalProfileRequested () const
{
   bool result = false;

   ACS_CS_ImModel model = *ACS_CS_ImRepository::instance()->getModel();
   ACS_CS_ImOmProfileManager *omProfileMgr = reinterpret_cast<ACS_CS_ImOmProfileManager *>(model.getObject(ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER.c_str()));

   if (omProfileMgr != NULL)
   {
	  int scope = omProfileMgr->scope;
	  if (scope == ONLY_APG)
		  result = true;
   }
   ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_INFO,"ACS_CS_NEHandler::isLocalProfileRequested APA local profile flag =%d",result));
   return result;
}

bool ACS_CS_NEHandler::isOmPrfSwitching()
{
	bool  omprofSwitch = false;
        //check if change OmProfile ongoing
        ACS_CS_API_OmProfilePhase::PhaseValue omProfilePhase;
        getOmProfilePhase(omProfilePhase);
        if( omProfilePhase != ACS_CS_API_OmProfilePhase::Idle)
        {
                omprofSwitch = true;
	}
	
	 ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_INFO,"ACS_CS_NEHandler::isOmPfSwitching() \n %d m_omprofSwitch =%d\n",omProfilePhase,omprofSwitch));
        return omprofSwitch;
}
bool ACS_CS_NEHandler::isOpModeSwitching()
{
	//check if change ClusterOpMode ongoing
        bool opModeSwitch = false;
        ACS_CS_API_ClusterOpMode::Value currentClusterOpMode;
        getClusterOpMode(currentClusterOpMode);
        if ((currentClusterOpMode==ACS_CS_API_ClusterOpMode::SwitchingToExpert)
                             ||  (currentClusterOpMode==ACS_CS_API_ClusterOpMode::SwitchingToNormal))
        {
                opModeSwitch = true;
        }
	
	 ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_INFO,"ACS_CS_NEHandler::isOpModeSwitching() \n %d m_opModeSwitch =%d\n",currentClusterOpMode,opModeSwitch));
        return opModeSwitch;
}


string ACS_CS_NEHandler::getNextCcFileIdentity(const ACS_CS_ImModel *model, string parentDn)
{
	string identity;
	set<const ACS_CS_ImBase *> ccFileInstances;
	model->getChildren(parentDn, ccFileInstances);

	int i_identity = 0;
	for(set<const ACS_CS_ImBase *>::iterator it = ccFileInstances.begin(); it != ccFileInstances.end(); it++)
	{
		const ACS_CS_ImCcFile *ccFile = dynamic_cast<const ACS_CS_ImCcFile *>(*it);

		string ccFileId = ACS_CS_ImUtils::getIdValueFromRdn(ccFile->rdn);
		int i_ccFileId = atoi(ccFileId.c_str());

		if (i_identity < i_ccFileId)
		{
			i_identity = i_ccFileId;
		}
	}

	i_identity++;
	ACS_APGCC::itoa(identity,i_identity);

	return identity;
}

bool ACS_CS_NEHandler::isCcFileAlreadyImported(const ACS_CS_ImModel *model, string ruleVersion)
{
	set<const ACS_CS_ImBase *> ccFileInstances;
	model->getObjects(ccFileInstances, CCFILE_T);

	for(set<const ACS_CS_ImBase *>::iterator it = ccFileInstances.begin(); it != ccFileInstances.end(); it++)
	{
		const ACS_CS_ImCcFile *ccFile = dynamic_cast<const ACS_CS_ImCcFile *>(*it);

		if (ccFile->rulesVersion.compare(ruleVersion.c_str()) == 0 )
		{
			//Ccf already exists
			return false;
		}
	}

	return true;

}
/*HW99445 start*/
bool ACS_CS_NEHandler::writeAPAalarmdata()
{
	int dwBytesWritten = 0;
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] Opening APA File to write", __FUNCTION__, __LINE__));
	m_hFile = ACE_OS::open(APA_ALARM_FILE.c_str(),
			O_CREAT|O_WRONLY|O_TRUNC);

	if (m_hFile == ACE_INVALID_HANDLE)
	{
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d] INVALID HANDLE received while writing APA data APA DATA into file", __FUNCTION__, __LINE__));
		return false;
	}
	dwBytesWritten = ACE_OS::write(m_hFile,
			(void*)(m_apaAlarmText.c_str()),
			m_apaAlarmText.length());
	if (dwBytesWritten <= 0)
	{   ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d] NO data written into APA file", __FUNCTION__, __LINE__));
		ACE_OS::close(m_hFile);
		return false;
	}
	ACE_OS::close(m_hFile);
	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] DATA written into APA file successfully length = %d,Alarm data = %s", __FUNCTION__, __LINE__,m_apaAlarmText.length(),m_apaAlarmText.c_str()));
	return true;
}

bool ACS_CS_NEHandler::readAPAalarmdata()
{
	int dwBytesRead = 0;
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	struct stat fileStat;
	string tmpStr = "";
	char *tmpCh = NULL;
	tmpCh = new char[MAX_BUFFER_LENGTH];
	if(stat(APA_ALARM_FILE.c_str(), &fileStat) < 0)
	{
		if(errno == ENOENT)
		{
			ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] APA file does not exist to read data", __FUNCTION__, __LINE__));
		}
		else
		{
			ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d] Reading failed for APA file - errno == %d", __FUNCTION__, __LINE__, errno));
		}
		delete tmpCh;
		return false;
	}
	ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] Opening APA File to read data", __FUNCTION__, __LINE__));

	m_hFile = ACE_OS::open(APA_ALARM_FILE.c_str(),O_RDONLY);

	if (m_hFile == ACE_INVALID_HANDLE)
	{
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d] Opening of APA File to read data failed INVALID HANDLE", __FUNCTION__, __LINE__));
		delete tmpCh;
		return false;
	}
	else
	{
		dwBytesRead = ACE_OS::read(m_hFile, (void*) (tmpCh),MAX_BUFFER_LENGTH);
		if(dwBytesRead <= 0)
		{
			ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_ERROR, "[%s@%d] No bytes read from APA file", __FUNCTION__, __LINE__));
			ACE_OS::close(m_hFile);
			delete tmpCh;
			return false;
		}
		tmpStr.assign(tmpCh);
		m_apaAlarmText = tmpStr;
		ACS_CS_FTRACE((ACS_CS_NEHandler_TRACE, LOG_LEVEL_TRACE, "[%s@%d] Successfully read data from APA File length =%d,FILE DATA =%s, file data 2 = %s", __FUNCTION__, __LINE__,m_apaAlarmText.length(),tmpCh,m_apaAlarmText.c_str()));
		ACE_OS::close(m_hFile);
		delete tmpCh;
		return true;

	}

}
/*HW99445 stop*/
