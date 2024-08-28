

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_SlaveReplicator_h
#define ACS_CS_SlaveReplicator_h 1



#include <queue>
#include <map>


// ACS_CS_Thread
#include "ACS_CS_Thread.h"


class ACS_CS_PDU;
class ACS_CS_Parser;
class TCPClient;
class ACS_CS_ReaderWriterLock;







class ACS_CS_SlaveReplicator : public ACS_CS_Thread
{

   typedef std::map<unsigned short, std::queue<ACS_CS_PDU *> * > tableMapType; 


  public:
      virtual ~ACS_CS_SlaveReplicator();


      static ACS_CS_SlaveReplicator * getInstance ();

      virtual int exec ();

      int sendRequest (ACS_CS_PDU *pdu);

      int getRequest (ACS_CS_Protocol::CS_Scope_Identifier scope, ACS_CS_PDU &pdu);

      void sendACK (ACS_CS_PDU *pdu);

      bool startSync ();

      void stopSync ();

      bool isSyncing ();

      static void start ();

      static void stop ();

    // Additional Public Declarations

  protected:
      ACS_CS_SlaveReplicator();

    // Additional Protected Declarations

  private:
      ACS_CS_SlaveReplicator(const ACS_CS_SlaveReplicator &right);

      ACS_CS_SlaveReplicator & operator=(const ACS_CS_SlaveReplicator &right);


      bool startSession (TCPClient **client, unsigned short port);

      void addToqueue (ACS_CS_PDU &pdu);

      ACS_CS_PDU * getFromOutQueue ();

      bool isConnectionAlive (TCPClient *tcpClient = 0);

    // Additional Private Declarations

    // Data Members for Class Attributes

      static ACS_CS_SlaveReplicator* replicatorInstance;

       volatile unsigned long lastWorkingIP;

//RoGa       CRITICAL_SECTION ipCriticalSection;

       tableMapType *tableMap;

       ACS_CS_Parser *csParser;

       bool syncInProgress;

//RoGa       CRITICAL_SECTION syncCriticalSection;

//RoGa       HANDLE syncEventHandle;

//RoGa       HANDLE hwcReplicationEvent;

//RoGa       HANDLE cpIdReplicationEvent;

//RoGa       HANDLE cpGroupReplicationEvent;

//RoGa       HANDLE fdReplicationEvent;

//RoGa       HANDLE neReplicationEvent;

//RoGa       HANDLE vlanReplicationEvent;

       ACS_CS_ReaderWriterLock *lock;

       bool isAP1;

       ACS_CS_Protocol::CS_Side_Identifier side;

      std::queue<ACS_CS_PDU * > outQueue;

    // Additional Implementation Declarations

};


// Class ACS_CS_SlaveReplicator 



#endif
