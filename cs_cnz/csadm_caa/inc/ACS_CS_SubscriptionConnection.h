

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


#ifndef ACS_CS_SubscriptionConnection_h
#define ACS_CS_SubscriptionConnection_h 1


#include <queue>

// ACS_CS_Thread
#include "ACS_CS_Thread.h"

#include "ACS_CS_Event.h"
#include "ACS_CS_TCPClient.h"


namespace ACS_CS_Subscription_NS
{
   // Translate ACS_CS_Protocol::CS_Scope_Identifier
   static const char * const TABLE_NAMES[] =
   {
      "Unspecified Table",
      "HWC Table",
      "CpId Table",
      "CpGroup Table",
      "FD Table",
      "NE Table",
      "VLAN Table",
      "Subscription Scope"
   };

   // Translate ACS_CS_Protocol::CS_Primitive_Identifier( Primitive_NotifyCpidTableChange ~ Primitive_NotifyCpGroupTableChange)
   static const char * const NOTIFICATION_PDU_NAMES[] =
   {
      "CpIdTableChange",
      "NeTableChange",
      "ProfilePhaseChange",
      "CpGroupTableChange",
      "HWCTableChange"
   };

   // The returned values of handleConnectionEvent() and handlePduEvent()
   enum Handle_Event_Results 
   {
      Result_OK = 0,
      Result_Client_Disconnected = 1,
      Result_Parser_Error = 2,
      Result_Invalid_PDU = 3,
      Result_Empty_Queue =4
   };

   // The indexes of events used by SubscritpionAgent and SubscritpionConnection
   enum Event_Indexes 
   {
      Shutdown_Event_Index = 0,
      Socket_Event_Index = 1,   // Accept, Read or Close event of socket
      Pdu_Event_Index = 2,
      Number_Of_Events = 3
   };
}

class ACS_CS_Parser;
class ACS_CS_SubscriptionAgent;
class ACS_CS_ReaderWriterLock;
class ACS_CS_PDU;


class ACS_CS_SubscriptionConnection : public ACS_CS_Thread
{

  public:

      ACS_CS_SubscriptionConnection (ACS_CS_TCPClient *tcpClient, ACS_CS_SubscriptionAgent *agent);

      virtual ~ACS_CS_SubscriptionConnection();

      virtual int exec ();

      void disconnect ();

      bool hasFinished () const;

      virtual void finish ();

      void copyToQueue (const ACS_CS_PDU &pdu);

      ACS_CS_Subscription_NS::Handle_Event_Results parse ();

  private:

      ACS_CS_SubscriptionConnection(const ACS_CS_SubscriptionConnection &right);

      ACS_CS_SubscriptionConnection & operator=(const ACS_CS_SubscriptionConnection &right);

      void emptyQueue ();

      ACS_CS_Subscription_NS::Handle_Event_Results handleConnectionEvent ();

      ACS_CS_Subscription_NS::Handle_Event_Results handlePduEvent ();

      ACS_CS_Subscription_NS::Handle_Event_Results send (const ACS_CS_PDU &pdu) const;


      ACS_CS_SubscriptionAgent *subscriptionAgent;

      ACS_CS_EventHandle shutdownEvent;

      ACS_CS_TCPClient *tcpClient;

      ACS_CS_EventHandle pduEvent;

      ACS_CS_Parser *csParser;

      bool finished;

      std::queue<ACS_CS_PDU * > pduQueue;

      ACS_CS_ReaderWriterLock *lock;
};

#endif
