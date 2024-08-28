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


#ifndef ACS_CS_CPGroupHandler_h
#define ACS_CS_CPGroupHandler_h 1

#include <string>

#include "ACS_CS_Protocol.h"

#include "ACS_CS_TableHandler.h"
#include "ACS_CS_ImModelSubset.h"
#include "ACS_CS_API_Internal.h"


class ACS_CS_PDU;
class ACS_CS_Table;
class ACS_CS_API_Util_Implementation;
class ACS_CS_ReaderWriterLock;
class ACS_CS_SubscriptionAgent;


//	This is a class that handles the CP group table. All
//	requests about this table are sent to an instance of
//	this class. The class manages the table when entries are
//	added and removed and saves the table to disk when it
//	has changed.


class ACS_CS_CPGroupHandler : public ACS_CS_TableHandler
{
    public:

      ACS_CS_CPGroupHandler();

      virtual ~ACS_CS_CPGroupHandler();

      virtual int handleRequest (ACS_CS_PDU *pdu);

      virtual int newTableOperationRequest (ACS_CS_ImModelSubset *subset);

      virtual bool loadTable ();

  private:

      ACS_CS_CPGroupHandler(const ACS_CS_CPGroupHandler &right);

      ACS_CS_CPGroupHandler & operator=(const ACS_CS_CPGroupHandler &right);

      int handleGetGroupNames (ACS_CS_PDU *pdu);

      int handleGetGroupMembers (ACS_CS_PDU *pdu);

      //string groupNameByGroupId(ACS_CS_Table *table, const ACS_CS_Attribute &groupIdentityAttr);

      //bool groupMembersByGroupId(std::vector<CPID> &cpIds, const ACS_CS_Attribute &groupIdentityAttr);

      void handleGroupSubscription (ACS_CS_ImModelSubset *subset);

      int createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, unsigned short requestId, ACS_CS_Protocol::CS_Result_Code result);

      bool getMemberCase (std::string setMember, std::string &storedMember);

      ACS_CS_Protocol::CS_Result_Code getGroupMembers (const std::string &groupName, unsigned short &nameHits, unsigned short* &memberList, unsigned short &memberCount) const;

      void getGroupMembers(std::vector<CPID> &cpIds, string cpName) const;

      void handleClearOG (ACS_CS_ImModelSubset *subset);

      void clearMap();

  private:

       ACS_CS_ReaderWriterLock *lock;

       stringCPIDSetMap *CPGroupMap;

       ACS_CS_SubscriptionAgent *agentInstance;
};

#endif
