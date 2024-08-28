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


#ifndef ACS_CS_CPIdHandler_h
#define ACS_CS_CPIdHandler_h 1

#include "ACS_CS_Protocol.h"

// ACS_CS_TableHandler
#include "ACS_CS_TableHandler.h"
// ACS_CS_TableOperation
#include "ACS_CS_TableOperation.h"
// ACS_CS_SubscriptionAgent
#include "ACS_CS_SubscriptionAgent.h"


class ACS_CS_PDU;
class ACS_CS_Table;
class ACS_CS_API_Util_Implementation;
class ACS_CS_ReaderWriterLock;


//	This is a class that handles the CP table. All requests
//	about this table are sent to an instance of this class.
//	The class manages the table when entries are added and
//	removed and saves the table to disk when it has changed.


class ACS_CS_CPIdHandler : public ACS_CS_TableHandler
{
  public:

      ACS_CS_CPIdHandler();

      virtual ~ACS_CS_CPIdHandler();

      virtual int handleRequest (ACS_CS_PDU *pdu);

      virtual int newTableOperationRequest(ACS_CS_ImModelSubset *subset);

      virtual bool loadTable ();

  private:

      ACS_CS_CPIdHandler(const ACS_CS_CPIdHandler &right);

      ACS_CS_CPIdHandler & operator=(const ACS_CS_CPIdHandler &right);

      int handleGetCPId (ACS_CS_PDU *pdu);

      int handleGetCPName (ACS_CS_PDU *pdu);

      int handleGetAPZSystem (ACS_CS_PDU *pdu);

      int handleGetCPType (ACS_CS_PDU *pdu);

      int handleGetCPList (ACS_CS_PDU *pdu);

      int createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, unsigned short requestId, ACS_CS_Protocol::CS_Result_Code result);

      void handleCPIdTableSubscription (ACS_CS_ImModelSubset *subset);

      int handleGetCPAliasName (ACS_CS_PDU *pdu);

      int handleGetCPState (ACS_CS_PDU *pdu);

      int handleGetApplicationId (ACS_CS_PDU *pdu);

      int handleGetApzSubstate (ACS_CS_PDU *pdu);

      int handleGetStateTransCode (ACS_CS_PDU *pdu);

      int handleGetAptSubstate (ACS_CS_PDU *pdu);

      ACS_CS_Protocol::CS_Result_Code getCpIdList (unsigned short* &cpList, unsigned short &cpCount);

      int handleGetBlockingInfo (ACS_CS_PDU *pdu);

      int handleGetCPCapacity (ACS_CS_PDU *pdu);

      int handleGetMAUType (ACS_CS_PDU *pdu);

  private:

       ACS_CS_Table *table;

       std::set<std::string> currentAliases;

       ACS_CS_ReaderWriterLock *lock;

       ACS_CS_SubscriptionAgent *agentInstance;
};

#endif
