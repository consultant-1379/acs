//  *********************************************************
//   COPYRIGHT Ericsson 2009-2018.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2009-2018.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2009-2018 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#ifndef ACS_CS_HWCHandler_h
#define ACS_CS_HWCHandler_h 1

// ACS_CS_TableHandler
#include "ACS_CS_TableHandler.h"
// ACS_CS_DHCP_Info
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_TableOperation.h"
#include "ACS_CS_SubscriptionAgent.h"
#include "ACS_CS_SM_HWCTableNotify.h"

class ACS_CS_DHCP_Info;
class ACS_CS_Table;
class ACS_CS_TableEntry;
class ACS_CS_ReaderWriterLock;
class ACS_CS_ServiceHandler;

class ACS_CS_HWCHandler : public ACS_CS_TableHandler
{
   struct IPAssignment
      {
          uint32_t iPEthA;
          uint32_t maskEthA;
          uint32_t iPEthB;
          uint32_t maskEthB;
          uint32_t aliasEthA;
          uint32_t aliasEthB;
      };

  public:

      ACS_CS_HWCHandler();

      virtual ~ACS_CS_HWCHandler();

      virtual int handleRequest (ACS_CS_PDU *pdu);

      virtual int newTableOperationRequest(ACS_CS_ImModelSubset *subset);

      virtual bool loadTable ();

      void updateMAC (ACS_CS_DHCP_Info &client);

      void setServiceHandler(ACS_CS_ServiceHandler* serviceHandler);

      static uint32_t getIpAddress(const std::string &address);

      ACS_CS_Table* getTable() {return table;}

  private:

      ACS_CS_HWCHandler(const ACS_CS_HWCHandler &right);

      ACS_CS_HWCHandler & operator=(const ACS_CS_HWCHandler &right);

      int handleGetValue (ACS_CS_PDU *pdu);

      int handleGetBoardList (ACS_CS_PDU *pdu);

      int handleSynchTable (ACS_CS_PDU *pdu);

      void handleHWCTableSubscription (ACS_CS_ImModelSubset *subset);

      int createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result);

      int createGetBoardListResponse (ACS_CS_PDU *pdu, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result, uint16_t logicalClock, uint16_t *boardList, int size);

      int createGetValueResponse (ACS_CS_PDU *pdu, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result, uint16_t logicalClock = 0, const char  *response = 0, int length = 0);

      ACS_CS_DHCP_Info getDhcpInfo (ACS_CS_Protocol::CS_Network_Identifier network, ACS_CS_TableEntry &board);

      ACS_CS_HWCData getHWCTableData(const ACS_CS_ImBlade *blade) const;

      string getStringAttr(const ACS_CS_Attribute *attribute);

      uint16_t getShortAttr(const ACS_CS_Attribute *attribute);

      uint32_t getIpAddress(const ACS_CS_Attribute *attribute);

  private:

       ACS_CS_Table *table;

       ACS_CS_ReaderWriterLock *lock;

       ACS_CS_ServiceHandler* serviceHandler;

       ACS_CS_SubscriptionAgent *agentInstance;

       int shelfArchitecture;
};


#endif
