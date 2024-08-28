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

#include "ACS_CS_Table.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_BasicResponse.h"

class ACS_CS_HostNetworkConverter;

#include "ACS_CS_SM_CPGroupTableNotify.h"
#include "ACS_CS_CPGroup_GetGroupMembers.h"
#include "ACS_CS_CPGroup_GetGroupMembersResponse.h"
#include "ACS_CS_CPGroup_GetGroupNames.h"
#include "ACS_CS_CPGroup_GetGroupNamesResponse.h"
#include "ACS_CS_SubscriptionAgent.h"

#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_ImRepository.h"


#include <vector>
#include <sstream>
#include <string>


// ACS_CS_CPGroupHandler
#include "ACS_CS_CPGroupHandler.h"


#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_CPGroupHandler_TRACE);

using namespace ACS_CS_Protocol;
using namespace ACS_CS_NS;
using std::set;
using std::string;
using std::vector;
using std::ostringstream;



// Class ACS_CS_CPGroupHandler 

ACS_CS_CPGroupHandler::ACS_CS_CPGroupHandler()
      : lock(0),
        CPGroupMap(0),
        agentInstance(0)
{

   CPGroupMap = new stringCPIDSetMap();
   lock = new ACS_CS_ReaderWriterLock();

   // Agent should always be created before handlers.
   agentInstance = ACS_CS_SubscriptionAgent::getInstance();
}


ACS_CS_CPGroupHandler::~ACS_CS_CPGroupHandler()
{
	if (CPGroupMap) {
		clearMap();
		delete CPGroupMap;
	}

	if (lock)
		delete lock;
}


 int ACS_CS_CPGroupHandler::handleRequest (ACS_CS_PDU *pdu)
{

   ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleRequest()\n"
         "Entering method\n"));

   if (pdu == 0)  // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleRequest()\n"
         "Error: Invalid PDU\n"));

      return -1;
   }

   int error = 0;

   const ACS_CS_HeaderBase * header = pdu->getHeader(); // Get header from PDU

   if (header)
   {
      // save the primitive ID so that it can be used later
      const ACS_CS_Protocol::CS_Primitive_Identifier primitiveId =
         header->getPrimitiveId();

      // Check which primitive that has been received and call handler function
      if (primitiveId == Primitive_GetGroupNames)
      {
         lock->start_reading();
         error = handleGetGroupNames(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetGroupMembers)
      {
         lock->start_reading();
         error = handleGetGroupMembers(pdu);
         lock->stop_reading();
      }
      else
      {
         ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
            "(%t) ACS_CS_CPGroupHandler::handleRequest()\n"
            "Error: Unknown primitive, id = %d\n",
            primitiveId));

         error = -1;
      }
   }

   return error;

}

 int ACS_CS_CPGroupHandler::newTableOperationRequest (ACS_CS_ImModelSubset *subset)
{
    // Handle CPGroup operations made to the table for the subscriptions
    handleGroupSubscription(subset);

    // Handle clear og
//    handleClearOG(subset);

    return 0;
}

 bool ACS_CS_CPGroupHandler::loadTable ()
{
     ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
             "ACS_CS_CPGroupHandler::loadTable()\n"
             "Entering method\n"));

	 if (!CPGroupMap)
	     return false;

	 ACS_CS_API_TableLoader *tableLoader = new ACS_CS_API_TableLoader();

	 clearMap();

	 if (tableLoader->loadCPGroupTable(*CPGroupMap) != ACS_CS_API_NS::Result_Success)
	 {
	     ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
	             "(%t) ACS_CS_CPGroupHandler::loadTable()\n"
	             "Error: Failed to load the CP Group table\n"));

	     delete tableLoader;
	     return false;
	 }

     if (tableLoader) {
	     delete tableLoader;
	 }

     ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
             "ACS_CS_CPGroupHandler::loadTable()\n"
             "Loaded %d table entries", CPGroupMap->size()));

	 return true;
}

 int ACS_CS_CPGroupHandler::handleGetGroupNames (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupNames()\n"
         "Entering method\n"));

   if (pdu == 0)  // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupNames()\n"
         "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   // Get version from incoming package
   if (header)
   {
   	version = header->getVersion();
   }

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPGroup_GetGroupNames * getGroupNames = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupNames()\n"
         "Error: Invalid header or primitive, header = %d, primitive = %d\n",
         header,
         primitive));

      return -1;
   }

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetGroupNames)
   {
      getGroupNames = reinterpret_cast<const ACS_CS_CPGroup_GetGroupNames *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupNames()\n"
         "Error: Wrong primitive type, type = %d\n",
         primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();       // Get request id
   int error = -1;

   ACS_CS_Header * responseHeader = new ACS_CS_Header();
   ACS_CS_CPGroup_GetGroupNamesResponse * responseBody = new ACS_CS_CPGroup_GetGroupNamesResponse();

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      error = createBasicResponse(pdu,                // Create response
                                 ACS_CS_Protocol::Primitive_GetGroupNamesResponse,
                                 requestId,
                                 ACS_CS_Protocol::Result_Other_Failure);

      return error;
   }

   vector<string> nameVector;

   nameVector.push_back("ALLBC");
   nameVector.push_back("OPGROUP");

   responseHeader->setVersion(version);
   responseHeader->setScope(Scope_CPGroupTable);
   responseHeader->setPrimitive(Primitive_GetGroupNamesResponse);
   responseHeader->setRequestId(requestId);

   //responseBody->setLogicalClock(groupNameTable->getLogicalClock());
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
   responseBody->setNameVector(nameVector);

   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   return 0;

}

 int ACS_CS_CPGroupHandler::handleGetGroupMembers (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "Entering method\n"));

   if (pdu == 0)  // Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "Error: Invalid PDU\n"));

      return -1;
   }
   else if ( CPGroupMap == 0 )   // Check for valid table
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "Error: No table pointer\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   // Get version from incoming package
   if (header)
   {
	   version = header->getVersion();
   }

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPGroup_GetGroupMembers * groupMembersPrimitive = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "Error: Invalid header or primitive, header = %d, primitive = %d\n",
         header,
         primitive));

      return -1;
   }

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetGroupMembers)
   {
      groupMembersPrimitive = reinterpret_cast<const ACS_CS_CPGroup_GetGroupMembers *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "Error: Wrong primitive type, type = %d\n",
         primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();       // Get request id
   int error = -1;

   string groupName = groupMembersPrimitive->getGroupName();   // Store group name received in PDU
   string storedGroupName = groupName;                   // Create a string to hold group name
                                                         // as stored in CS (considering case)

   ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "groupName: %s\n",groupName.c_str()));

   //Check if any group named as the group received in the PDU exists in CS, if so,
   //the group name stored in CS is returned (might differ considering case)
   if (this->getMemberCase(groupName, storedGroupName) == false)
   {
            ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "getMemberCase returned false\n"));

      error = createBasicResponse(pdu,                // Create response
                                 ACS_CS_Protocol::Primitive_GetGroupMembersResponse,
                                 requestId,
                                 ACS_CS_Protocol::Result_No_Such_Entry);

      return error;
   }

   groupName = storedGroupName;  //Store CS group name as the received group name

   unsigned short nameHits;
   unsigned short * memberList;
   unsigned short memberCount;

   ACS_CS_Protocol::CS_Result_Code rcode =
      getGroupMembers(groupName, nameHits, memberList, memberCount);

   switch (rcode)
   {
      case ACS_CS_Protocol::Result_Success:
         break;
      case ACS_CS_Protocol::Result_No_Such_Entry:
         {
            // Groups is not ALL or ALLBC or OPGROUP
            if ((groupName != CP_GROUP_ALL) &&
               (groupName != CP_GROUP_ALLBC) &&
               (groupName != CP_GROUP_OPGROUP))
            {
               ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
                  "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
                  "Groupname not found: %s\n",groupName.c_str()));

               error = createBasicResponse(pdu,
                  ACS_CS_Protocol::Primitive_GetGroupMembersResponse,
                  requestId,
                  ACS_CS_Protocol::Result_No_Such_Entry);

               return error;
            }
         }
         break;
      case ACS_CS_Protocol::Result_Busy: // If timeout from table
         {
            error = createBasicResponse(pdu,
               ACS_CS_Protocol::Primitive_GetGroupMembersResponse,
               requestId,
               ACS_CS_Protocol::Result_Busy);

            return error;
         }
         break;
      case ACS_CS_Protocol::Result_Other_Failure:
         {
            if (nameHits > 1)
            {
               ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
                  "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
                  "nameHits not 0 or 1\n"));
            }
            error = createBasicResponse(pdu,
               ACS_CS_Protocol::Primitive_GetGroupMembersResponse,
               requestId,
               ACS_CS_Protocol::Result_Other_Failure);

            return error;
         }
         break;
      default:
         {
            ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
               "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
               "getGroupMembers() returned %u \n", rcode));

            error = createBasicResponse(pdu,
               ACS_CS_Protocol::Primitive_GetGroupMembersResponse,
               requestId,
               ACS_CS_Protocol::Result_Other_Failure);

            return error;
         }
         break;
   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header();
   ACS_CS_CPGroup_GetGroupMembersResponse * responseBody = new ACS_CS_CPGroup_GetGroupMembersResponse();

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
         delete responseHeader;

      if (responseBody)
         delete responseBody;

      if (memberList)
         delete [] memberList;

        ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
         "(%t) ACS_CS_CPGroupHandler::handleGetGroupMembers()\n"
         "ResponseHeader or responseBody == 0\n"));

      error = createBasicResponse(pdu,                // Create response
                                 ACS_CS_Protocol::Primitive_GetGroupMembersResponse,
                                 requestId,
                                 ACS_CS_Protocol::Result_Other_Failure);

      return error;
   }

   responseHeader->setVersion(version);
   responseHeader->setScope(Scope_CPGroupTable);
   responseHeader->setPrimitive(Primitive_GetGroupMembersResponse);
   responseHeader->setRequestId(requestId);

   //responseBody->setLogicalClock(groupMemberTable->getLogicalClock());
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);

   if (memberList)
      (void) responseBody->setResponseArray(memberList, memberCount);

   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   error = 0;

   if (memberList)
      delete [] memberList;

   return error;

   return -1;
}

void ACS_CS_CPGroupHandler::handleGroupSubscription(ACS_CS_ImModelSubset *subset)
{
	const ACS_CS_ImBase * object = 0;
	object = subset->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());

	if(object == 0)
		return;

	const ACS_CS_ImCpCluster *bladeCluster = dynamic_cast<const ACS_CS_ImCpCluster *>(object);


		ACS_CS_CpGroupData AllBcgroupChange;

		std::set<string> allBcGroup;

		if (bladeCluster)
			allBcGroup = bladeCluster->allBcGroup;

		if(allBcGroup.size() == 0)
		{

			int ogClearCode, undefOgClearCode = 0;
			if(bladeCluster)
			{
				ogClearCode = bladeCluster->ogClearCode;
				undefOgClearCode = ACS_CS_ImCpCluster::UNDEFINED;

				if(undefOgClearCode == ogClearCode)
				{
					AllBcgroupChange.emptyOgReason = ACS_CS_API_OgChange::OtherReason;
				} else {
					AllBcgroupChange.emptyOgReason = static_cast<ACS_CS_API_OgChange::ACS_CS_API_EmptyOG_ReasonCode>(bladeCluster->ogClearCode);
				}
			} else {
				AllBcgroupChange.emptyOgReason = ACS_CS_API_OgChange::OtherReason;
			}
		}
		else
			AllBcgroupChange.emptyOgReason = ACS_CS_API_OgChange::NotEmpty;

		//TODO: Find a way to fetch the info about the action performed on the group if needed
		AllBcgroupChange.operationType = ACS_CS_API_TableChangeOperation::Unspecified;

//		switch(cpGroup->action)
//		{
//		case ACS_CS_ImBase::CREATE:
//			groupChange.operationType = ACS_CS_API_TableChangeOperation::Add;
//			break;
//		case ACS_CS_ImBase::MODIFY:
//			groupChange.operationType = ACS_CS_API_TableChangeOperation::Change;
//			break;
//		case ACS_CS_ImBase::DELETE:
//			groupChange.operationType = ACS_CS_API_TableChangeOperation::Delete;
//			break;
//
//		default:
//			groupChange.operationType = ACS_CS_API_TableChangeOperation::Unspecified;
//			break;
//		}

		AllBcgroupChange.cpGroupName = ACS_CS_NS::CP_GROUP_ALLBC;


		ACS_CS_CpGroupData OpgroupChange;
		std::set<string> opGroup;

		if (bladeCluster)
			opGroup = bladeCluster->operativeGroup;

		if(opGroup.size() == 0)
		{
			int ogClearCode, undefOgClearCode = 0;
			if(bladeCluster)
			{
				ogClearCode = bladeCluster->ogClearCode;
				undefOgClearCode = ACS_CS_ImCpCluster::UNDEFINED;

				if(undefOgClearCode == ogClearCode)
				{
					OpgroupChange.emptyOgReason = ACS_CS_API_OgChange::OtherReason;
				} else {
					OpgroupChange.emptyOgReason = static_cast<ACS_CS_API_OgChange::ACS_CS_API_EmptyOG_ReasonCode>(bladeCluster->ogClearCode);
				}
			} else {
				OpgroupChange.emptyOgReason = ACS_CS_API_OgChange::OtherReason;
			}
		}
		else
			OpgroupChange.emptyOgReason = ACS_CS_API_OgChange::NotEmpty;

		//TODO: Find a way to fetch the info about the action performed on the group if needed
		OpgroupChange.operationType = ACS_CS_API_TableChangeOperation::Unspecified;

		OpgroupChange.cpGroupName = ACS_CS_NS::CP_GROUP_OPGROUP;

		// Group should be used with subscription ALLBC, OPGROUP.
		// Now add all the CPID:s that are a part of this group
		getGroupMembers(AllBcgroupChange.cpIdList, ACS_CS_NS::CP_GROUP_ALLBC);
		agentInstance->handleGroupUpdates(AllBcgroupChange);

		getGroupMembers(OpgroupChange.cpIdList, ACS_CS_NS::CP_GROUP_OPGROUP);
		agentInstance->handleGroupUpdates(OpgroupChange);

}

void ACS_CS_CPGroupHandler::getGroupMembers(std::vector<CPID> &cpIds, string cpName) const
{
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
	if(NULL == model)
		return;

	const ACS_CS_ImBase * object = 0;
	object = model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());

	if(object == 0)
		return;

	const ACS_CS_ImCpCluster *cpCluster = dynamic_cast<const ACS_CS_ImCpCluster *>(object);

	if (!cpCluster)
		return;

	std::set<string> cpGroup;

	if (cpName.compare("ALLBC") == 0) {
		cpGroup = cpCluster->allBcGroup;
	} else if (cpName.compare("OPGROUP") == 0) {
		cpGroup = cpCluster->operativeGroup;
	}

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *> clusterCpObjects;

	model->getObjects(clusterCpObjects, CLUSTERCP_T);

	set<string>::const_iterator it;
	for(it = cpGroup.begin(); it != cpGroup.end(); it++)
	{
		string cpName =  *it;

		for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
		{
			const ACS_CS_ImBase *base = *clusterCpIt;
			const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

			if(cp && cpName.compare(ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn)) == 0){
				cpIds.push_back(static_cast<uint16_t>(cp->systemIdentifier));
				break;
			}
		}
	}
}

//void ACS_CS_CPGroupHandler::handleGroupSubscription (ACS_CS_ImModelSubset *subset)
//{
//     TableOperationData *members = tableOperation->tableEntries[ACS_CS_INTERNAL_API::Table_CPGroupMembers];
//
//     if (!members)
//         return;
//
//     ACS_CS_Table *table = members->entryTable[ACS_CS_TableOperation_NS::Create];
//     ACS_CS_API_TableChangeOperation::OpType operationType = ACS_CS_API_TableChangeOperation::Change;
//
//     if (!table)
//     {
//         // Entry has not been removed, try checking if the group is being removed
//         table = members->entryTable[ACS_CS_TableOperation_NS::Delete];
//         operationType = ACS_CS_API_TableChangeOperation::Delete;
//     }
//
//     if (table)
//     {
//         ACS_CS_TableSearch tableSearch;
//
//         table->search(tableSearch);
//
//         if (tableSearch.getEntryCount() > 0)
//         {
//             // Loop thru all entries
//             size_t sizeOfEntries = tableSearch.getEntryCount();
//             unsigned short *entries = new unsigned short[sizeOfEntries];
//             tableSearch.getEntryList(entries, sizeOfEntries);
//
//             for (int i = 0;i < table->getSize(); i++)
//             {
//                 string groupName;
//                 ACS_CS_CpGroupData groupChange;
//                 ACS_CS_TableEntry tableEntry = table->getEntry(entries[i]);
//
//                 const ACS_CS_Attribute groupIdentityAttr = tableEntry.getValue(Attribute_CPGroup_GroupIdentifier);
//
//                 // Set default values
//                 groupChange.emptyOgReason = ACS_CS_API_OgChange::NotEmpty;
//                 groupChange.operationType = operationType;
//
//                 TableOperationData *names = tableOperation->tableEntries[ACS_CS_INTERNAL_API::Table_CPGroupNames];
//
//                 if (names)
//                 {
//                     // A new group has been created
//                     ACS_CS_Table *table = names->entryTable[ACS_CS_TableOperation_NS::Create];
//
//                     if (table)
//                     {
//                         // Check if the group name is created now
//                         groupName = groupNameByGroupId(table, groupIdentityAttr);
//
//                         if (groupName.length() > 0)
//                             groupChange.operationType = ACS_CS_API_TableChangeOperation::Add;    // New group name entry is being added
//                     }
//                 }
//
//                 if (groupName.length() <= 0)
//                     groupName = groupNameByGroupId(groupNameTable, groupIdentityAttr);   // Group exists since before
//
//                 if (groupName.length() > 0)
//                 {
//                     groupChange.cpGroupName = groupName;
//
//                     // Group name has been found
//                     if (ACS_CS_API_Util::isReservedCPGroupName(groupName))
//                     {
//                         // Group should be used with subscription ALL, ALLBC, OPGROUP.
//                         // Now add all the CPID:s that are a part of this group
//                         if (groupMembersByGroupId(groupChange.cpIdList, groupIdentityAttr))
//                         {
//                             agentInstance->handleGroupUpdates(groupChange);
//                         }
//                     }
//                 }
//             }
//
//             if (entries)
//                 delete [] entries;
//         }
//     }
//}


int ACS_CS_CPGroupHandler::createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, unsigned short requestId, ACS_CS_Protocol::CS_Result_Code result)
{

	ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
			"(%t) ACS_CS_CPGroupHandler::createBasicResponse()\n"
			"Entering method\n"));

	// Suppress warning for not freeing header and body
	// which we shouldn't since they are passed to the PDU
	// and freed there
	//lint --e{429}

	if (pdu == 0)  // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
				"(%t) ACS_CS_CPGroupHandler::createBasicResponse()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}

	ACS_CS_Header * header = new ACS_CS_Header();   // Create header

	if (header == 0)
	{
		ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
				"(%t) ACS_CS_CPGroupHandler::createBasicResponse()\n"
				"Error: Invalid header\n"));

		return -1;
	}

	ACS_CS_BasicResponse * body = 0;

	// Create body
	if (type == ACS_CS_Protocol::Primitive_GetGroupNamesResponse)
	{
		// This function can only be used for error responses for this primitive
		if (result == ACS_CS_Protocol::Result_Success)
		{
			ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
					"(%t) ACS_CS_CPGroupHandler::createBasicResponse()\n"
					"Error: Wrong function call, type = %d, result = %d\n",
					type,
					result));

			delete header;

			return -1;
		}

		body = new ACS_CS_CPGroup_GetGroupNamesResponse();
	}
	else if (type == ACS_CS_Protocol::Primitive_GetGroupMembersResponse)
	{
		// This function can only be used fo error responses for this primitive
		if (result == ACS_CS_Protocol::Result_Success)
		{
			ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
					"(%t) ACS_CS_CPGroupHandler::createBasicResponse()\n"
					"Error: Wrong function call, type = %d, result = %d\n",
					type,
					result));

			delete header;

			return -1;
		}

		body = new ACS_CS_CPGroup_GetGroupMembersResponse();
	}

	if (body == 0)
	{
		ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
				"(%t) ACS_CS_CPGroupHandler::createBasicResponse()\n"
				"Error: Invalid body\n"));

		delete header;
		return -1;
	}

	const ACS_CS_Header * oldHeader = 0;
	CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

	if (pdu && pdu->getHeader())
		oldHeader = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

	// Get version from incoming package
	if (oldHeader)
	{
		version = oldHeader->getVersion();
	}
	// Set all header values
	header->setVersion(version);
	header->setScope(Scope_CPGroupTable);
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

bool ACS_CS_CPGroupHandler::getMemberCase (std::string setMember, std::string &storedMember)
{

   bool contains = false;

   stringCPIDSetMap::iterator it;

   for (it = CPGroupMap->begin(); it != CPGroupMap->end(); ++it)
   {
      if (ACS_CS_API_Util::compareWithoutCase(it->first, setMember) )
      {
         contains = true;
         storedMember = it->first;
         break;
      }
   }

   return contains;

	 return false;
}

ACS_CS_Protocol::CS_Result_Code ACS_CS_CPGroupHandler::getGroupMembers (const std::string &groupName, unsigned short &nameHits, unsigned short* &memberList, unsigned short &memberCount) const
{

   ACS_CS_TRACE((ACS_CS_CPGroupHandler_TRACE,
      "(%t) Entered %s, group: %s \n", __FUNCTION__, groupName.c_str()));

   nameHits = 0;
   memberList = 0;
   memberCount = 0;

   stringCPIDSetMap::iterator mapIt = CPGroupMap->find(groupName); // Search for entry in table

   // check for valid group name
   if (mapIt == CPGroupMap->end())
   {
      return ACS_CS_Protocol::Result_No_Such_Entry;
   }

   cpidSet * memberSet = mapIt->second;

   nameHits = 1;
   memberCount = memberSet->size();
   memberList = new unsigned short[memberCount];

   cpidSet::iterator setIt;
   int i = 0;

   for (setIt = memberSet->begin(); setIt != memberSet->end(); ++setIt)
   {
	   memberList[i] = *setIt;
	   i++;
   }

   return ACS_CS_Protocol::Result_Success;

}

void ACS_CS_CPGroupHandler::clearMap()
{
	stringCPIDSetMap::iterator it;

	for(it = CPGroupMap->begin(); it != CPGroupMap->end(); ++it)
	{
		delete it->second;
	}

	CPGroupMap->clear();
}

void ACS_CS_CPGroupHandler::handleClearOG(ACS_CS_ImModelSubset */*subset*/)
{
//	TableOperationData *namesTable =
//			tableOperation->tableEntries[ACS_CS_INTERNAL_API::Table_CPGroupNames];
//
//	if (!namesTable)
//		return;
//
//	ACS_CS_Table *table =
//			namesTable->entryTable[ACS_CS_TableOperation_NS::Create];
//
//	if (!table)
//		table = namesTable->attributeTable[ACS_CS_TableOperation_NS::Modify]; // Try the attributes table instead
//
//	if (!table)
//		return;
//
//	ACS_CS_TableSearch tableSearch;
//
//	// Search for all entries for this scope
//	(void) table->search(tableSearch);
//	int entryCount = tableSearch.getEntryCount();
//
//	if (entryCount == 0)
//		return; // No entries found
//
//	unsigned short * entryList = new unsigned short[entryCount];
//
//	// Get list with all entry ids
//	tableSearch.getEntryList(entryList, entryCount);
//
//	// Loop through list of entry ids
//	for (uint16_t entryNo = 0; entryNo < entryCount; entryNo++) {
//		// Get table entry with this id
//		unsigned short entryId = entryList[entryNo];
//		ACS_CS_TableEntry entry = table->getEntry(entryId);
//
//		vector<ACS_CS_Attribute> attr;
//		entry.getValueVector(attr);
//
//		// See if attribute is present
//		ACS_CS_Attribute tmpAttribute = entry.getValue(
//				ACS_CS_Protocol::Attribute_CPGroup_ClearOgCode);
//
//		if (tmpAttribute.getIdentifier()
//				== ACS_CS_Protocol::Attribute_CPGroup_ClearOgCode) {
//			// Found the attribute, do something useful with it!
//			ACS_CS_API_OgChange::ACS_CS_API_EmptyOG_ReasonCode reasonCode;
//
//			tmpAttribute.getValue(reinterpret_cast<char*>(&reasonCode),
//					sizeof(reasonCode));
//
//			// Notify Agent of these updates
//			agentInstance->updateOgClear(CP_GROUP_OPGROUP, reasonCode);
//
//			break;
//		}
//	}
//
//	if (entryList)
//		delete[] entryList;
}
