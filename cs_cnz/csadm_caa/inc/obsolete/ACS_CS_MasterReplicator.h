

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_MasterReplicator_h
#define ACS_CS_MasterReplicator_h 1



#include <vector>
#include <map>
#include <queue>


// ACS_CS_Thread
#include "ACS_CS_Thread.h"


class ACS_CS_PDU;
class TCPClient;
class TCPServer;
class ACS_CS_ReaderWriterLock;
class ACS_CS_Parser;







class ACS_CS_MasterReplicator : public ACS_CS_Thread
{

   typedef std::map<unsigned short, std::vector<ACS_CS_PDU *> * > tableMapType; 
   typedef std::map<unsigned long, TCPClient * > clientMapType; 
   typedef std::map<unsigned long, std::queue<ACS_CS_PDU *> * > queueMapType;


  public:
      virtual ~ACS_CS_MasterReplicator();


      static ACS_CS_MasterReplicator * getInstance ();

      static void start ();

      static void stop ();

      virtual int exec ();

      int sendRequest (ACS_CS_PDU *pdu);

      void updateTable (ACS_CS_Protocol::CS_Scope_Identifier scope, const std::vector<ACS_CS_PDU *> &tableVector);

    // Additional Public Declarations

  protected:
      ACS_CS_MasterReplicator();

    // Additional Protected Declarations

  private:
      ACS_CS_MasterReplicator(const ACS_CS_MasterReplicator &right);

      ACS_CS_MasterReplicator & operator=(const ACS_CS_MasterReplicator &right);


      void startListener ();

      bool send (unsigned long ipAddress, std::queue<ACS_CS_PDU *> &outQueue) const;

      void createQueue (unsigned long ipAddress);

      void copyToQueue (ACS_CS_Protocol::CS_Scope_Identifier scope, std::queue<ACS_CS_PDU *> &targetQueue) const;

      void checkQueues ();

      void deleteClient (unsigned long ipAddress);

      void emptyQueues ();

      int getQueueSize () const;

      void checkACKs ();

      bool isLocalSlave (unsigned long ipAddress) const;

      void checkECHOs () const;

    // Additional Private Declarations

    // Data Members for Class Attributes

       TCPServer *tcpServerEthA;

       TCPServer *tcpServerEthB;

       bool ethAAvailable;

       bool ethBAvailable;

//RoGa       HANDLE acceptEventEthA;

//RoGa        HANDLE acceptEventEthB;

//RoGa        SOCKET socketEthA;

//RoGa        SOCKET socketEthB;

//RoGa        HANDLE shutdownEvent;

      static ACS_CS_MasterReplicator* replicatorInstance;

       clientMapType *clientMap;

       tableMapType *tableMap;

       queueMapType *queueMap;

      std::queue<ACS_CS_PDU * > commonQueue;

       ACS_CS_ReaderWriterLock *lock;

//RoGa       HANDLE pduEvent;

       long unsigned ackClientIP;

       ACS_CS_Parser *ackParser;

    // Additional Implementation Declarations

};


// Class ACS_CS_MasterReplicator 



#endif
