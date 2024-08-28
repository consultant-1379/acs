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


#ifndef ACS_CS_SubscriptionAgent_h
#define ACS_CS_SubscriptionAgent_h 1


#include <map>
#include <set>
#include <list>

#include <ace/Singleton.h>

#include "ACS_CS_API.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_TCPServer.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_SM_CPTableNotify.h"
#include "ACS_CS_SM_HWCTableNotify.h"

// ACS_CS_SubscriptionConnection
#include "ACS_CS_SubscriptionConnection.h"
// ACS_CS_Thread
#include "ACS_CS_Thread.h"

class ACS_CS_ReaderWriterLock;
class ACS_CS_PDU;
class ACS_CS_CpGroupData;
class ACS_CS_Primitive;
class ACS_CS_NetworkElementChange;



class ACS_CS_SubscriptionAgent : public ACS_CS_Thread
{
   typedef std::map<ACS_CS_Protocol::CS_Scope_Identifier, std::set<ACS_CS_SubscriptionConnection *> > TableSubscriberMapType;
   typedef std::map<ACS_CS_API_OmProfilePhase::PhaseValue, std::set<ACS_CS_SubscriptionConnection *> > PhaseSubscriberMapType;
   typedef std::list<ACS_CS_PDU *> PduContainer;

   public:

      static ACS_CS_SubscriptionAgent * getInstance ();

      static void start ();

      static void stop ();

      virtual int exec ();

      virtual void updatePhaseChange (const ACS_CS_API_OmProfileChange &newProfile);
      virtual void updatePhaseChange (const ACS_CS_API_OmProfileChange &newProfile, unsigned int &notificationCount);

      friend ACS_CS_Subscription_NS::Handle_Event_Results ACS_CS_SubscriptionConnection::parse ();

      unsigned int getTableSubscriberCount (ACS_CS_Protocol::CS_Scope_Identifier scope = ACS_CS_Protocol::Scope_NotSpecified) const;

      unsigned int getPhaseSubscriberCount (ACS_CS_API_OmProfilePhase::PhaseValue phase = ACS_CS_API_OmProfilePhase::Idle) const;

      void enableProfileChangeNotification ();

      void disableProfileChangeNotification ();

      void handleGroupUpdates (ACS_CS_CpGroupData &groupData);

      void handleCPIdTableUpdates (const ACS_CS_CpTableData &cpData);

      void handleNEUpdates (const ACS_CS_NetworkElementChange &neChange);

      void handleHWCUpdates(const ACS_CS_HWCChange &hwcChange);

      void updateOgClear (const std::string& groupName, const ACS_CS_API_OgChange::ACS_CS_API_EmptyOG_ReasonCode& reason);

  private:
      ACS_CS_SubscriptionAgent();

      virtual ~ACS_CS_SubscriptionAgent();

      ACS_CS_SubscriptionAgent(const ACS_CS_SubscriptionAgent &right);

      ACS_CS_SubscriptionAgent & operator=(const ACS_CS_SubscriptionAgent &right);


      void deleteAllConnections ();

      void emptyPduContainer ();

      void clearMaps ();

      void startListener ();

      void removeFinishedConnections ();

      void handleAcceptEvent (int handle);

      void handlePduEvent ();

      bool dispatchSinglePdu (const ACS_CS_PDU * const pdu);

      void addTableSubscriber (ACS_CS_Protocol::CS_Scope_Identifier scope = ACS_CS_Protocol::Scope_NotSpecified, ACS_CS_SubscriptionConnection *subscriber = 0);

      void removeTableSubscriber (ACS_CS_Protocol::CS_Scope_Identifier scope = ACS_CS_Protocol::Scope_NotSpecified, ACS_CS_SubscriptionConnection *subscriber = 0);

      void addPhaseSubscriber (ACS_CS_API_OmProfilePhase::PhaseValue phase = ACS_CS_API_OmProfilePhase::Idle, ACS_CS_SubscriptionConnection *subscriber = 0);

      void removePhaseSubscriber (ACS_CS_API_OmProfilePhase::PhaseValue phase = ACS_CS_API_OmProfilePhase::Idle, ACS_CS_SubscriptionConnection *subscriber = 0);

      void updatePhaseSubcriberCount();

      ACS_CS_PDU * createNotificationPDU (ACS_CS_Protocol::CS_Scope_Identifier scope, ACS_CS_Protocol::CS_Primitive_Identifier primitiveType, ACS_CS_Primitive *primitive);

      void signalPduEvent ();


  private:

      ACS_CS_EventHandle shutdownEvent;

      ACS_CS_EventHandle pduEvent;

      ACS_CS_TCPServer *tcpServerLocalhost;

      ACS_CS_ReaderWriterLock *lock;

      PduContainer pduContainer;

      TableSubscriberMapType tableSubscriberMap;

      PhaseSubscriberMapType phaseSubscriberMap;

      PhaseSubscriberMapType phaseNotificationMap;

      bool profileChangeNotificationDisabled;

      std::set<ACS_CS_SubscriptionConnection *> connectionSet;

      typedef ACE_Singleton<ACS_CS_SubscriptionAgent, ACE_Recursive_Thread_Mutex> instance_;
      friend class ACE_Singleton<ACS_CS_SubscriptionAgent, ACE_Recursive_Thread_Mutex>;
};

#endif
