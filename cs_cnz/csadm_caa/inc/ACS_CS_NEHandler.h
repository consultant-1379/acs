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

#ifndef ACS_CS_NEHandler_h
#define ACS_CS_NEHandler_h 1

#include "ACS_CS_Protocol.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
#include "ACS_CS_Event.h"
#include <vector>
#include "ACS_CS_ImModel.h"

#include "ACS_CS_TableHandler.h"
#include "ACS_CS_API_ProfileHandling.h"

#include <xercesc/sax/DocumentHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/HandlerBase.hpp>


#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#include <map>
#include <string>

#define MAX_BUFFER_LENGTH (int)2048  /*HW99445*/

class ACS_CS_PDU;
class ACS_CS_Table;
class ACS_CS_EntryCounter;
class ACS_CS_ReaderWriterLock;
class ACS_CS_API_Util_Implementation;
struct ACS_CS_API_OmProfileChange_R1;
class ACS_CS_SubscriptionAgent;

extern const char * const NE_CLUSTEROPMODE_TIMER_ID;
extern const char * const NE_PHASE_VALIDATE_TIMER_ID;
extern const char * const NE_PHASE_APNOTIFY_TIMER_ID;
extern const char * const NE_PHASE_CPNOTIFY_TIMER_ID;
extern const char * const NE_PHASE_COMMIT_TIMER_ID;

XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_BEGIN
class AttributeList;
XERCES_CPP_NAMESPACE_END

using namespace xercesc;

typedef enum {
		FILENAME,
		IDENTITY,
		PROFILE,
		APTPROFILE,
		APZPROFILE,
		RULESVERSION,
		REVISION
	} XmlProperties;

typedef std::map<XmlProperties, std::string> xmlPropertiesMap_t;

class ACS_CS_NEHandler : public ACS_CS_TableHandler
{
	public:

	class SaxHandler : public HandlerBase
	{
		public:

			typedef std::map<XmlProperties, std::string> propertiesMap_t;

			class StaticEntityResolver : public EntityResolver
			{
				public:

					//	Override the DTD finding routine in Xerces
					virtual InputSource* resolveEntity (const XMLCh* const publicId, const XMLCh* const systemId);
			};



			SaxHandler (const std::string &xmlFilePath, propertiesMap_t &xmlProperties);

			virtual ~SaxHandler();

			//	Method to handle the start events that are generated for each element when
			//	an XML file is parsed
			 void startElement (const XMLCh* const  name, AttributeList &attributes);
//			virtual void startElement (const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attributes);

			//	Metod to handle errors that occured during parsing
			 void warning (const SAXParseException &exception);

			//	Metod to handle errors that occured during parsing
			 void error (const SAXParseException &exception);

			//	Metod to handle errors that occured during parsing
			 void fatalError (const SAXParseException &exception);


//			bool m_errorOccurred;

		protected:
			// Additional Protected Declarations

		private:

			SaxHandler(const SaxHandler &right);

			SaxHandler & operator=(const SaxHandler &right);

			propertiesMap_t &m_xmlProperties;

			std::string m_xmlFilePath;

		private:
			// Additional Implementation Declarations
	};


      ACS_CS_NEHandler();

      virtual ~ACS_CS_NEHandler();

      virtual int handleRequest (ACS_CS_PDU *pdu);

      virtual int newTableOperationRequest(ACS_CS_ImModelSubset *subset);

      virtual bool loadTable ();

      bool setClusterOpModeSupervisionTimer (int timeout);

      bool cancelClusterOpModeSupervisionTimer ();

      int handleClusterOpModeSupervisionTimerExpired ();

      bool setOmProfileSupervisionTimer (ACS_CS_API_OmProfilePhase::PhaseValue phase, int timeout);

      bool cancelOmProfileSupervisionTimer (ACS_CS_API_OmProfilePhase::PhaseValue phase);

      int handleOmProfileSupervisionTimerExpired (ACS_CS_API_OmProfilePhase::PhaseValue phase);

      int handleStartupSupervision ();

      int execStartupSupervision ();

      bool getClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpMode) const;

      static CPID getCPID(std::string cpName);

      bool setClusterOpModeChange (ACS_CS_API_ClusterOpMode::Value newClusterOpMode);

      bool setClusterOmProfile ( int actionId , int profileValue = -1, int apzProfile = -1, int aptProfile = -1);//ACS_CS_API_OmProfileChange::UnspecifiedProfile

      bool getOmProfilePhase (ACS_CS_API_OmProfilePhase::PhaseValue &omProfilePhase) const;

      int handleSetOmProfileNotificationStatus (bool success, ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_Set::ReasonType reason);

      inline bool isStartupProfileSupervisionCompleted () { return startupProfileSupervisionComplete; };

      bool importCommandClassificationFile  (std::string filename);

      bool getCurrentOmProfileValue (ACS_CS_API_OmProfileChange::Profile currentOmProfile) const;

      static std::string getXmlProperty (XmlProperties property);

      bool compareWithCurrentOmProfileValue (int reuqestedOmProfile) const;

      bool exportCommandClassificationFile();

      static bool moveMmlFileToAdhFolder(bool remove = true);

      bool removeSupportedOmProfiles();

      static inline std::string getCcfPath (){ return m_ccfilePath;};

      bool createCommandClassificationFolder ();

      int getCurrentOmProfileValue () const;

      bool renameMmlFile(string stateFrom, string stateTo, int profile);

      bool cleanMmlFileFolder(int profile);

      bool getCcFileIdentity(string pathFile, string &identity);

      string getNextCcFileIdentity(const ACS_CS_ImModel *model, string parentDn);

      bool isCcFileAlreadyImported(const ACS_CS_ImModel *model, string ruleVersion);

      bool removeInvalidChars(std::string &value);

      bool findMmlFileFolder(int &omProfile, int apzProfile, int aptProfile);

      bool isOpModeSwitching();

      bool isOmPrfSwitching();
  protected:

      bool checkForMMLCommandRulesFile (string &mmlFilePath, string &mmlFileName, CcFileStateType type, int profile = UNDEF_OMPROFILESTATETYPE);

      void changeMmlFileStatus (int profile_current, int profile_requested, bool success, bool starting);

  private:

      ACS_CS_NEHandler(const ACS_CS_NEHandler &right);

      ACS_CS_NEHandler & operator=(const ACS_CS_NEHandler &right);

      //int updateTableAttributes (const uint16_t entryId, const std::vector<ACS_CS_Attribute*> attributes);

      int handleGetCPSystem (ACS_CS_PDU *pdu);

      int handleGetNEId (ACS_CS_PDU *pdu);

      int handleGetAlarmMaster (ACS_CS_PDU *pdu);

      int handleGetClockMaster (ACS_CS_PDU *pdu);

      int handleGetBSOMIPAddress (ACS_CS_PDU *pdu);

      int handleGetTestEnvironment (ACS_CS_PDU *pdu);

      int createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, unsigned short requestId, ACS_CS_Protocol::CS_Result_Code result);

      int handleGetClusterOpMode (ACS_CS_PDU *pdu);

      ACS_CS_Protocol::CS_Result_Code handleSetOmProfile (int newOmProfile, ProfileChangeTriggerEnum reason);

      int handleGetOmProfile (ACS_CS_PDU *pdu);

      int handleGetTrafficIsolated (ACS_CS_PDU *pdu);

      int handleGetTrafficLeader (ACS_CS_PDU *pdu);

      ACS_CS_Protocol::CS_Result_Code handleSetLocalProfile (int newOmProfile, int newApzProfile, int newAptProfile);

      int handleSetProfiles (const ACS_CS_ImCpCluster *bladeCluster, const ACS_CS_ImAdvancedConfiguration *advConf);

      int handleSetClusterOpModeChanged (ACS_CS_API_ClusterOpMode::Value clusterOpMode, ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType);

      void handleNeTableSubscription ();

      ACS_CS_Protocol::CS_Result_Code handleSetApaProfile (int newApzProfile, int newAptProfile);

      bool readClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpMode) const;

      void processProfileChangesDuringStartUp ();

      bool getBladeClusterInfoAndAdvancedConfiguration(const ACS_CS_ImCpCluster **bladeCluster, const ACS_CS_ImAdvancedConfiguration **advConf) const;

      void setStartupProfileSupervision(bool completed);

      bool initializeParser ();

      bool setXmlFile (std::string filenameXml, std::string folder);

      bool copyMmlFileToNbiFolder(string filename);

      bool checkExtention(const char* file, const char* type_file);

      bool writeAPAalarmdata();/*HW99445*/

      bool readAPAalarmdata(); /*HW99445*/

      //--------------------------
      // Helper functions
      std::string convertOmProfileToString (const ACS_CS_API_OmProfileChange& omProfile);
      static std::string convertReasonToString (ACS_CS_API_Set::ReasonType reason);

      // OM Profile Handling
      //----------------------

      //	This method gets a package of seven attributes back,
      //	that specify a omProfileChange, these are:
      //
      //	omProfileCurrent/Requested
      //	aptProfileCurrent/Requested
      //	apzProfileCurrent/Requested
      //	phase
      //
      //	all of these attributes are unsigned.
      bool readOmProfile (ACS_CS_API_OmProfileChange& omProfileChange) const;

      bool writeOmProfile (const ACS_CS_API_OmProfileChange& omProfileChange);

      bool rollbackOmProfile (ACS_CS_API_OmProfileChange &omProfileChange);

      bool checkQueuedApaProfiles ();

      bool ignoreApaProfileRequest (ACS_CS_API_OmProfileChange& omProfileChange);

      bool readOmProfileActionId (AsyncActionType &actionId) const;

      bool isLocalProfileRequested () const;


      //--------------------------------------
      // Attributes
      //--------------------------------------
  public:
      bool immDataSynchronized;
      bool m_isRestartAfterRestore; /*HW99445*/

  private:

       ACS_CS_Table *table;

       ACS_CS_EntryCounter *cpCounter;

       ACS_CS_ReaderWriterLock *lock;

       ACS_CS_EventHandle clusterOpModeSupervisionTimerHandle;

       ACS_CS_API_ProfileHandling *profileHandling;

       std::vector<ACS_CS_EventHandle> omProfileSupervisionTimerHandle;

       ACS_CS_SubscriptionAgent *agentInstance;

       static xmlPropertiesMap_t m_xmlProperties;

       char m_xmlFile[256];

       static const  std::string DTD_FILE_PATH;

       bool notificationsPending;

       unsigned int subscriberCount;

       bool startupProfileSupervisionComplete;

       mutable ACE_Recursive_Thread_Mutex _profile_mutex;				// used to synchronize access

       static string m_ccfilePath;

       static bool IMMWriteFailure;

       string m_apaAlarmText;/*HW99445*/

       bool m_apaAlarm, m_ScopeFlag;      /*HW99445*/

};

#endif
