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
#include "ACS_CS_Protocol.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_BasicResponse.h"

#include "ACS_CS_CPId_GetCPId.h"
#include "ACS_CS_CPId_GetCPIdResponse.h"
#include "ACS_CS_CPId_GetCPName.h"
#include "ACS_CS_CPId_GetCPNameResponse.h"
#include "ACS_CS_CPId_GetCPAliasName.h"
#include "ACS_CS_CPId_GetCPAliasNameResponse.h"
#include "ACS_CS_CPId_GetAPZSystem.h"
#include "ACS_CS_CPId_GetAPZSystemResponse.h"
#include "ACS_CS_CPId_GetCPType.h"
#include "ACS_CS_CPId_GetCPTypeResponse.h"
#include "ACS_CS_CPId_GetCPList.h"
#include "ACS_CS_CPId_GetCPListResponse.h"
#include "ACS_CS_CPId_GetCPState.h"
#include "ACS_CS_CPId_GetCPStateResponse.h"
#include "ACS_CS_CPId_GetApplicationId.h"
#include "ACS_CS_CPId_GetApplicationIdResponse.h"
#include "ACS_CS_CPId_GetApzSubstate.h"
#include "ACS_CS_CPId_GetApzSubstateResponse.h"
#include "ACS_CS_CPId_GetStateTransCode.h"
#include "ACS_CS_CPId_GetStateTransCodeResponse.h"
#include "ACS_CS_CPId_GetAptSubstate.h"
#include "ACS_CS_CPId_GetAptSubstateResponse.h"
#include "ACS_CS_CPId_GetBlockingInfo.h"
#include "ACS_CS_CPId_GetBlockingInfoResponse.h"
#include "ACS_CS_CPId_GetCPCapacity.h"
#include "ACS_CS_CPId_GetCPCapacityResponse.h"
#include "ACS_CS_CPId_GetMAUType.h"
#include "ACS_CS_CPId_GetMAUTypeResponse.h"

#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_EntryCounter.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_API_TableLoader.h"

#include "ACS_CS_ImRepository.h"

#include <sstream>
#include <vector>


// ACS_CS_CPIdHandler
#include "ACS_CS_CPIdHandler.h"


#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_CPIdHandler_TRACE);
ACS_CS_Trace_TDEF(ACS_CS_CPIdHandlerSet_TRACE);

using namespace ACS_CS_Protocol;
using namespace ACS_CS_NS;
using std::string;
using std::set;
using std::vector;



// Class ACS_CS_CPIdHandler

ACS_CS_CPIdHandler::ACS_CS_CPIdHandler()
   : table(0),
     lock(0),
     agentInstance(0)
{

   table = new ACS_CS_Table();            // CP table
   lock = new ACS_CS_ReaderWriterLock();

   agentInstance = ACS_CS_SubscriptionAgent::getInstance();
}


ACS_CS_CPIdHandler::~ACS_CS_CPIdHandler()
{
   if (table)
      delete table;

   if (lock)
      delete lock;
}


int ACS_CS_CPIdHandler::handleRequest (ACS_CS_PDU *pdu)
{

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleRequest()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleRequest()\n"
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
      if (primitiveId == Primitive_GetCPId)
      {
         lock->start_reading();
         error = handleGetCPId(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetCPName)
      {
         lock->start_reading();
         error = handleGetCPName(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetCPAliasName)
      {
         lock->start_reading();
         error = handleGetCPAliasName(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetAPZSystem)
      {
         lock->start_reading();
         error = handleGetAPZSystem(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetCPType)
      {
         lock->start_reading();
         error = handleGetCPType(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetCPList)
      {
         lock->start_reading();
         error = handleGetCPList(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetCPState)
      {
         lock->start_reading();
         error = handleGetCPState(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetApplicationId)
      {
         lock->start_reading();
         error = handleGetApplicationId(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetApzSubstate)
      {
         lock->start_reading();
         error = handleGetApzSubstate(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetStateTransCode)
      {
         lock->start_reading();
         error = handleGetStateTransCode(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetAptSubstate)
      {
         lock->start_reading();
         error = handleGetAptSubstate(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetBlockingInfo)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::handleRequest()\n"
                       "Primitive_GetBlockingInfo called\n"));

         lock->start_reading();
         error = handleGetBlockingInfo(pdu);
         lock->stop_reading();

      }
      else if (primitiveId == Primitive_GetCPCapacity)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::handleRequest()\n"
                       "Primitive_GetCPCapacity called\n"));

         lock->start_reading();
         error = handleGetCPCapacity(pdu);
         lock->stop_reading();
      }
      else if (primitiveId == Primitive_GetMAUType)
      {
         lock->start_reading();
         error = handleGetMAUType(pdu);
         lock->stop_reading();
      }
      else
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::handleRequest()\n"
                       "Error: Unknown primitive, id = %d\n",
                       primitiveId));

         error = -1;
      }

   }

   return error;

}

int ACS_CS_CPIdHandler::newTableOperationRequest(ACS_CS_ImModelSubset *subset)
{
     // Handle CPIdTable operations made to the table for the subscriptions
     handleCPIdTableSubscription(subset);

     return 0;
}

bool ACS_CS_CPIdHandler::loadTable ()
{

	ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
			"ACS_CS_CPIdHandler::loadTable()\n"
			"Entering method\n"));

   if ( ! table )
	  return false;

   ACS_CS_API_TableLoader *tableLoader = new ACS_CS_API_TableLoader();

   table->clearTable();

   if (tableLoader->loadCPTable(*table) == ACS_CS_API_NS::Result_Success)
   {
	   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
			   "ACS_CS_CPIdHandler::loadTable()\n"
			   "Loaded %d table entries", table->getSize()));

	   delete tableLoader;
	   return true;
   }

   if (tableLoader) {
	   delete tableLoader;
   }

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
		   "(%t) ACS_CS_CPIdHandler::loadTable()\n"
		   "Error: Failed to load the CP table\n"));

   return false;

}

int ACS_CS_CPIdHandler::handleGetCPId (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetCPId()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPId()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetCPId * getCPId = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPId()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetCPId)
   {
      getCPId = reinterpret_cast<const ACS_CS_CPId_GetCPId *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPId()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id
   const string cpName = getCPId->getName();
   int error = 0;



   //make a copy for multi-threading safety
   ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

//   set<const ACS_CS_ImBase *> objects;
//   set<const ACS_CS_ImBase *>::const_iterator it;
   uint16_t cpId = ILLEGAL_SYSID;
   bool cpIdFound = false;

   /*model->getObjects(objects, CP_T);

   for(it = objects.begin(); it != objects.end(); ++it)
   {
	   const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);

	   if(cp)
	   {
		   if(cp->defaultName == cpName || cp->alias == cpName)
		   {
			   cpId = cp->systemIdentifier;
		   }
	   }
   }*/


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

   delete model;

   if (ILLEGAL_SYSID != cpId)
   {
		ACS_CS_Header * responseHeader = new ACS_CS_Header();
		ACS_CS_CPId_GetCPIdResponse * responseBody = new ACS_CS_CPId_GetCPIdResponse();

		if ((responseHeader == 0) || (responseBody == 0)) {
			if (responseHeader)
				delete responseHeader;

			if (responseBody)
				delete responseBody;

			return -1;
		}

		responseHeader->setVersion(version);
		responseHeader->setScope(Scope_CPIdTable);
		responseHeader->setPrimitive(Primitive_GetCPIdResponse);
		responseHeader->setRequestId(requestId);

		responseBody->setLogicalClock(logicalClock);
		responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		responseBody->setCPId(cpId);

		responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength());

		pdu->setHeader(responseHeader);
		pdu->setPrimitive(responseBody);

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for GetCPId: value = %u, Body Length = %d",
					__FILE__, __FUNCTION__, __LINE__, cpId, responseBody->getLength());
			ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE, "(%t) %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}

		error = 0;
	}else
	{
		error = createBasicResponse(
				pdu, // Create response
				ACS_CS_Protocol::Primitive_GetCPIdResponse, requestId,
				ACS_CS_Protocol::Result_No_Such_Entry);
	}

	return error;
}

int ACS_CS_CPIdHandler::handleGetCPName (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetCPName()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPName()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetCPName * getCPName = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPName()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }
	
   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetCPName)
   {
      getCPName = reinterpret_cast<const ACS_CS_CPId_GetCPName *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPName()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id
   int error = -1;

   const uint16_t cpId = getCPName->getCPId();
   string cpName;


   //make a copy for multi-threading safety
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	set<const ACS_CS_ImBase *> clusterCpObjects;
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	model->getObjects(clusterCpObjects, CLUSTERCP_T);
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	bool foundCp = false;

	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	{
		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			foundCp = true;
			cpName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
			break;
		}
	}

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			foundCp = true;
			cpName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
			break;
		}
	}

	/*model->getObjects(objects, CP_T);
	bool foundCp = false;

	for (it = objects.begin(); it != objects.end(); ++it)
	{
		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);

		if (cp)
		{
			if (cp->systemIdentifier == cpId)
			{
				cpName = cp->defaultName;
				foundCp = true;
			}
		}
	}*/

	delete model;

	if (!cpName.empty())
	{
		ACS_CS_Header *responseHeader = new ACS_CS_Header();
		ACS_CS_CPId_GetCPNameResponse * responseBody = new ACS_CS_CPId_GetCPNameResponse();

		if ( (responseHeader == 0) || (responseBody == 0) )
		{
			if (responseHeader)
				delete responseHeader;

			if (responseBody)
				delete responseBody;

			return -1;
		}

		responseHeader->setVersion(version);
		responseHeader->setScope(Scope_CPIdTable);
		responseHeader->setPrimitive(Primitive_GetCPNameResponse);
		responseHeader->setRequestId(requestId);

		responseBody->setLogicalClock(logicalClock);
		responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		responseBody->setName(cpName);

		responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		pdu->setHeader(responseHeader);
		pdu->setPrimitive(responseBody);

		error = 0;
	} else if(foundCp)
	{
		error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
	} else
	{
		error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_Other_Failure);
	}

	return error;
}

int ACS_CS_CPIdHandler::handleGetAPZSystem (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetAPZSystem()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetAPZSystem()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }


   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetAPZSystem * getAPZSystem = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetAPZSystem()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }
   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetAPZSystem)
   {
      getAPZSystem = reinterpret_cast<const ACS_CS_CPId_GetAPZSystem *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetAPZSystem()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id
   int error = -1;

   const uint16_t cpId = getAPZSystem->getCPId();
   uint16_t apzSystem = 0;


   //make a copy for multi-threading safety
   	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

   	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
   	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

   	set<const ACS_CS_ImBase *> clusterCpObjects;
   	set<const ACS_CS_ImBase *> dualSidedCpObjects;
   	model->getObjects(clusterCpObjects, CLUSTERCP_T);
   	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

   	bool foundCp = false;

   	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
   	{
   		const ACS_CS_ImBase *base = *clusterCpIt;
   		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			string apzSysString = cp->apzSystem;
   			stringstream trim;
   			trim << apzSysString;
   			trim >> apzSysString;
   			apzSysString = trim.str();
   			if(apzSysString.length() == 8)
   				apzSysString = apzSysString.substr(3);

   			int apzSysInt = atoi(apzSysString.c_str());


   			if (CS_ProtocolChecker::checkAPZSystem(apzSysInt))
   			{
   				apzSystem = apzSysInt;
   			}
   			foundCp = true;

   			break;
   		}
   	}

   	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
   	{
   		const ACS_CS_ImBase *base = *dualSidedCpIt;
   		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){

   			string apzSysString = cp->apzSystem;
   			stringstream trim;
   			trim << apzSysString;
   			trim >> apzSysString;
   			apzSysString = trim.str();
   			if(apzSysString.length() == 8)
   				apzSysString = apzSysString.substr(3);

   			int apzSysInt = atoi(apzSysString.c_str());


   			if (CS_ProtocolChecker::checkAPZSystem(apzSysInt))
   			{
   				apzSystem = apzSysInt;
   			}
   			foundCp = true;
   			break;
   		}
   	}

//   	set<const ACS_CS_ImBase *> objects;
//   	set<const ACS_CS_ImBase *>::const_iterator it;
//
//   	model->getObjects(objects, CP_T);
//   	bool foundCp = false;
//
//   	for (it = objects.begin(); it != objects.end(); ++it)
//   	{
//   		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//   		if (cp)
//   		{
//   			if (cp->systemIdentifier == cpId)
//   			{
//   				if (CS_ProtocolChecker::checkAPZSystem(cp->apzSystem))
//				{
//   					apzSystem = cp->apzSystem;
//				}
//   				foundCp = true;
//   			}
//   		}
//   	}

   	delete model;


	if (apzSystem != 0)
	{
		ACS_CS_Header * responseHeader = new ACS_CS_Header();
		ACS_CS_CPId_GetAPZSystemResponse * responseBody = new ACS_CS_CPId_GetAPZSystemResponse();

		if ( (responseHeader == 0) || (responseBody == 0) )
		{
			if (responseHeader)
				delete responseHeader;
			if (responseBody)
				delete responseBody;

			return -1;
		}

		responseHeader->setVersion(version);
		responseHeader->setScope(Scope_CPIdTable);
		responseHeader->setPrimitive(Primitive_GetAPZSystemResponse);
		responseHeader->setRequestId(requestId);

		responseBody->setLogicalClock(logicalClock);
		responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		responseBody->setAPZSystem(apzSystem);

		responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		pdu->setHeader(responseHeader);
		pdu->setPrimitive(responseBody);

		error = 0;
	}
	else if(!foundCp)
	{
		error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		error = createBasicResponse(pdu,						// Create response
				ACS_CS_Protocol::Primitive_GetAPZSystemResponse,
				requestId,
				ACS_CS_Protocol::Result_No_Value);
	}

   return error;

}

int ACS_CS_CPIdHandler::handleGetCPType (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetCPType()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPType()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }


   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	
   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetCPType * getCPType = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPType()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }
   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetCPType)
   {
      getCPType = reinterpret_cast<const ACS_CS_CPId_GetCPType *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPType()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id
   int error = -1;

   const uint16_t cpId = getCPType->getCPId();
   uint16_t cpType = 0;


   //make a copy for multi-threading safety
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	set<const ACS_CS_ImBase *> clusterCpObjects;
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	model->getObjects(clusterCpObjects, CLUSTERCP_T);
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	bool foundCp = false;

	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	{
		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			if (CS_ProtocolChecker::checkCPType(cp->cpType))
			{
				cpType = cp->cpType;
				foundCp = true;
			}
			break;
		}
	}

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if(cp && cp->systemIdentifier == cpId){

			if (CS_ProtocolChecker::checkCPType(cp->cpType))
			{
				cpType = cp->cpType;
				foundCp = true;
			}
			break;
		}
	}

//	set<const ACS_CS_ImBase *> objects;
//	set<const ACS_CS_ImBase *>::const_iterator it;
//
//	model->getObjects(objects, CP_T);
//	bool foundCp = false;
//
//	for (it = objects.begin(); it != objects.end(); ++it)
//	{
//		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//		if (cp)
//		{
//			if (cp->systemIdentifier == cpId)
//			{
//				if (CS_ProtocolChecker::checkCPType(cp->cpType))
//				{
//					cpType = cp->cpType;
//					foundCp = true;
//				}
//			}
//		}
//	}

	delete model;


	if (cpType != 0)
	{
		ACS_CS_Header * responseHeader = new ACS_CS_Header();
		ACS_CS_CPId_GetCPTypeResponse * responseBody = new ACS_CS_CPId_GetCPTypeResponse();

		if ( (responseHeader == 0) || (responseBody == 0) )
		{
			if (responseHeader)
				delete responseHeader;

			if (responseBody)
				delete responseBody;

			return -1;
		}

		responseHeader->setVersion(version);
		responseHeader->setScope(Scope_CPIdTable);
		responseHeader->setPrimitive(Primitive_GetCPTypeResponse);
		responseHeader->setRequestId(requestId);

		responseBody->setLogicalClock(logicalClock);
		responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		responseBody->setCPType(cpType);

		responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		pdu->setHeader(responseHeader);
		pdu->setPrimitive(responseBody);

		error = 0;
	} else if(!foundCp)
	{
		error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
	} else
	{
		error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPTypeResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Value);
	}

   return error;

}

int ACS_CS_CPIdHandler::handleGetCPList (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetCPList()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPList()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	
   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetCPList * getCPList = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPList()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetCPList)
   {
      getCPList = reinterpret_cast<const ACS_CS_CPId_GetCPList *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPList()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id
   int error = -1;

   unsigned short hits;
   unsigned short * cpList;

   switch (getCpIdList(cpList, hits))
   {
      case ACS_CS_Protocol::Result_Success:
         {
            break;
         }
      case ACS_CS_Protocol::Result_Busy:
         {
            error = createBasicResponse(pdu,
                                        ACS_CS_Protocol::Primitive_GetCPListResponse,
                                        requestId,
                                        ACS_CS_Protocol::Result_Busy);

            return error;
         }
      case ACS_CS_Protocol::Result_Other_Failure:
         {
            return -1;
         }
      default:
         {
            break;
         }
   }

   ACS_CS_Header * responseHeader = new ACS_CS_Header();
   ACS_CS_CPId_GetCPListResponse * responseBody = new ACS_CS_CPId_GetCPListResponse();

   if ( (responseHeader == 0) || (responseBody == 0) )
   {
      if (responseHeader)
      {
         delete responseHeader;
      }

      if (responseBody)
      {
         delete responseBody;
      }

      if (cpList)
      {
         delete [] cpList;
      }

      return -1;
   }

   responseHeader->setVersion(version);
   responseHeader->setScope(Scope_CPIdTable);
   responseHeader->setPrimitive(Primitive_GetCPListResponse);
   responseHeader->setRequestId(requestId);

   responseBody->setLogicalClock(logicalClock);
   responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
   if (cpList)
   {
      responseBody->setCPList(cpList, hits);
   }

   responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

   pdu->setHeader(responseHeader);
   pdu->setPrimitive(responseBody);

   if (cpList)
   {
      delete [] cpList;
   }

   return 0;

}


int ACS_CS_CPIdHandler::createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, unsigned short requestId, ACS_CS_Protocol::CS_Result_Code result)
{

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                 "Entering method\n"));

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   // and freed there
   //lint --e{429}

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                    "Error: Invalid PDU\n"));

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

   ACS_CS_Header * header = new ACS_CS_Header();	// Create header

   if (header == 0)
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                    "Error: Invalid header\n"));

      return -1;
   }

   ACS_CS_BasicResponse * body = 0;

   // Create body
   if (type == ACS_CS_Protocol::Primitive_GetCPIdResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                       "Error: Wrong function call, type = %d, result = %d\n",
                       type,
                       result));

         delete header;

         return -1;
      }

      body = new ACS_CS_CPId_GetCPIdResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetCPNameResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                       "Error: Wrong function call, type = %d, result = %d\n",
                       type,
                       result));

         delete header;

         return -1;
      }

      body = new ACS_CS_CPId_GetCPNameResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetCPAliasNameResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                  "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                  "Error: Wrong function call, type = %d, result = %d\n",
                  type,
                  result));

         delete header;

         return -1;
      }

      body = new ACS_CS_CPId_GetCPAliasNameResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetAPZSystemResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                       "Error: Wrong function call, type = %d, result = %d\n",
                       type,
                       result));

         delete header;

         return -1;
      }

      body = new ACS_CS_CPId_GetAPZSystemResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetCPTypeResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                       "Error: Wrong function call, type = %d, result = %d\n",
                       type,
                       result));

         delete header;

         return -1;
      }

      body = new ACS_CS_CPId_GetCPTypeResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetCPListResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                       "Error: Wrong function call, type = %d, result = %d\n",
                       type,
                       result));

         delete header;

         return -1;
      }

      body = new ACS_CS_CPId_GetCPListResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetCPStateResponse)
   {
      body = new ACS_CS_CPId_GetCPStateResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetApplicationIdResponse)
   {
      body = new ACS_CS_CPId_GetApplicationIdResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetApzSubstateResponse)
   {
      body = new ACS_CS_CPId_GetApzSubstateResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetStateTransCodeResponse)
   {
      body = new ACS_CS_CPId_GetStateTransCodeResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetAptSubstateResponse)
   {
      body = new ACS_CS_CPId_GetAptSubstateResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetBlockingInfoResponse)
   {
      body = new ACS_CS_CPId_GetBlockingInfoResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetCPCapacityResponse)
   {
      body = new ACS_CS_CPId_GetCPCapacityResponse();
   }
   else if (type == ACS_CS_Protocol::Primitive_GetMAUTypeResponse)
   {
      // This function can only be used for error responses for this primitive
      if (result == ACS_CS_Protocol::Result_Success)
      {
         ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                       "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                       "Error: Wrong function call, type = %d, result = %d\n",
                       type,
                       result));

         delete header;

         return -1;
      }

      body = new ACS_CS_CPId_GetMAUTypeResponse();
   }
   else
   {
      body = 0;
   }

   if (body == 0)
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::createBasicResponse()\n"
                    "Error: Invalid body\n"));

      delete header;
      return -1;
   }

   // Set all header values
   header->setVersion(version);
   header->setScope(Scope_CPIdTable);
   header->setPrimitive(type);
   header->setRequestId(requestId);

   // Set resultcode
   body->setResultCode(result);

   // Set length
   header->setTotalLength(header->getHeaderLength() + 2);

   // Update PDU
   pdu->setHeader(header);
   pdu->setPrimitive(body);

   return 0;

}

void ACS_CS_CPIdHandler::handleCPIdTableSubscription (ACS_CS_ImModelSubset *subset)
{
	set<const ACS_CS_ImBase *> objects;
	subset->getObjects(objects, CLUSTERCP_T);

	set<const ACS_CS_ImBase *> DSobjects;
	subset->getObjects(DSobjects, DUALSIDEDCP_T);

	if(objects.size() == 0 && DSobjects.size() == 0)
		return;

	logicalClock++;

	set<const ACS_CS_ImBase *>::const_iterator it;

	for(it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImClusterCp *cp = dynamic_cast<const ACS_CS_ImClusterCp *>(*it);
		ACS_CS_API_TableChangeOperation::OpType operationType;

		if(NULL == cp)
			continue;

		switch(cp->action)
		{
		case ACS_CS_ImBase::CREATE:
			operationType = ACS_CS_API_TableChangeOperation::Add;
			break;
		case ACS_CS_ImBase::MODIFY:
			operationType = ACS_CS_API_TableChangeOperation::Change;
			break;
		case ACS_CS_ImBase::DELETE:
			operationType = ACS_CS_API_TableChangeOperation::Delete;
			break;

		default:
			operationType = ACS_CS_API_TableChangeOperation::Unspecified;
			break;
		}

		ACS_CS_CpTableData cpData;

		cpData.operationType = operationType;

		cpData.cpId = cp->systemIdentifier;
		cpData.cpName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
		cpData.cpAliasName = cp->alias;

		//Convert APZ System
		string apzSys = cp->apzSystem;
		stringstream trim;
		trim << apzSys;
		trim >> apzSys;
		apzSys = trim.str();
		if(apzSys.length() == 8)
			apzSys = apzSys.substr(3);

		cpData.apzSystem = atoi(apzSys.c_str());
		cpData.cpType = cp->cpType;
		cpData.cpState = cp->cpState;
		cpData.applicationId = cp->applicationId;
		cpData.apzSubstate = cp->apzSubstate;
		cpData.aptSubstate = cp->aptSubstate;
		cpData.stateTransition = cp->stateTransition;
		cpData.blockingInfo = cp->blockingInfo;
		cpData.cpCapacity = cp->cpCapacity;
		cpData.mauType = ACS_CS_API_NS::MAU_UNDEFINED;

		agentInstance->handleCPIdTableUpdates(cpData);
	}


	for(it = DSobjects.begin(); it != DSobjects.end(); it++)
		{
			const ACS_CS_ImDualSidedCp *cp = dynamic_cast<const ACS_CS_ImDualSidedCp *>(*it);
			ACS_CS_API_TableChangeOperation::OpType operationType;

			if(NULL == cp)
				continue;

			switch(cp->action)
			{
			case ACS_CS_ImBase::CREATE:
				operationType = ACS_CS_API_TableChangeOperation::Add;
				break;
			case ACS_CS_ImBase::MODIFY:
				operationType = ACS_CS_API_TableChangeOperation::Change;
				break;
			case ACS_CS_ImBase::DELETE:
				operationType = ACS_CS_API_TableChangeOperation::Delete;
				break;

			default:
				operationType = ACS_CS_API_TableChangeOperation::Unspecified;
				break;
			}

			ACS_CS_CpTableData cpData;

			cpData.operationType = operationType;

			cpData.cpId = cp->systemIdentifier;
			cpData.cpName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
			cpData.cpAliasName = cp->alias;

			//Convert APZ System
			string apzSys = cp->apzSystem;
			stringstream trim;
			trim << apzSys;
			trim >> apzSys;
			apzSys = trim.str();

			if(apzSys.length() == 8)
				apzSys = apzSys.substr(3);

			cpData.apzSystem = atoi(apzSys.c_str());
			cpData.cpType = cp->cpType;
			cpData.cpState = 0;
			cpData.applicationId = 0;
			cpData.apzSubstate = 0;
			cpData.aptSubstate = 0;
			cpData.stateTransition = 0;
			cpData.blockingInfo = 0;
			cpData.cpCapacity = 0;

			cpData.mauType = (ACS_CS_API_NS::MauType) cp->mauType;

			agentInstance->handleCPIdTableUpdates(cpData);
		}
}

int ACS_CS_CPIdHandler::handleGetCPAliasName (ACS_CS_PDU *pdu)
{
   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetCPAliasName()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPAliasName()\n"
                    "Error: Invalid PDU\n"));
      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetCPAliasName * getCPAliasName = 0;
	
	
   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPAliasName()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));
      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetCPAliasName)
   {
      getCPAliasName = reinterpret_cast<const ACS_CS_CPId_GetCPAliasName *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPAliasName()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));
      return -1;
   }

   unsigned short requestId = header->getRequestId(); // Get request id
   int error = -1;

   const uint16_t cpId = getCPAliasName->getCPId();
   string cpAlias;
   string cpName;

   //make a copy for multi-threading safety
   	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

   	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
   	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

   	set<const ACS_CS_ImBase *> clusterCpObjects;
   	set<const ACS_CS_ImBase *> dualSidedCpObjects;
   	model->getObjects(clusterCpObjects, CLUSTERCP_T);
   	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

   	bool foundCp = false;

   	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
   	{
   		const ACS_CS_ImBase *base = *clusterCpIt;
   		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			cpAlias = cp->alias;
   			cpName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
   			foundCp = true;

   			break;
   		}
   	}

   	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
   	{
   		const ACS_CS_ImBase *base = *dualSidedCpIt;
   		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			cpAlias = cp->alias;
   			cpName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
   			foundCp = true;

   			break;
   		}
   	}


//   	set<const ACS_CS_ImBase *> objects;
//   	set<const ACS_CS_ImBase *>::const_iterator it;
//
//   	model->getObjects(objects, CP_T);
//   	bool foundCp = false;
//
//   	for (it = objects.begin(); it != objects.end(); ++it)
//   	{
//   		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//   		if (cp)
//   		{
//   			if (cp->systemIdentifier == cpId)
//   			{
//   				cpAlias = cp->alias;
//   				cpName = cp->defaultName;
//   				foundCp = true;
//   			}
//   		}
//   	}

   	delete model;


	if (!cpName.empty())
	{
		ACS_CS_Header * responseHeader = new ACS_CS_Header();
		ACS_CS_CPId_GetCPAliasNameResponse * responseBody = new ACS_CS_CPId_GetCPAliasNameResponse();

		if ( (responseHeader == 0) || (responseBody == 0) )
		{
			if (responseHeader)
				delete responseHeader;
			if (responseBody)
				delete responseBody;
			return -1;
		}

		//build the response
		responseHeader->setVersion(version);
		responseHeader->setScope(Scope_CPIdTable);
		responseHeader->setPrimitive(Primitive_GetCPAliasNameResponse);
		responseHeader->setRequestId(requestId);

		responseBody->setLogicalClock(logicalClock);
		responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		if(cpAlias.empty())
		{
			responseBody->setName(cpName);
			responseBody->setAlias(false);
		} else
		{
			responseBody->setName(cpAlias);
			responseBody->setAlias(true);
		}

		responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		pdu->setHeader(responseHeader);
		pdu->setPrimitive(responseBody);

		error = 0;
	} else if(!foundCp)
	{
		error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{ // Create error response
		error = createBasicResponse(pdu, ACS_CS_Protocol::Primitive_GetCPAliasNameResponse,
				requestId, ACS_CS_Protocol::Result_Other_Failure);
	}

   return error;

}

int ACS_CS_CPIdHandler::handleGetCPState (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetCPState()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPState()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetCPState * getCPState = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPState()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetCPState)
   {
      getCPState = reinterpret_cast<const ACS_CS_CPId_GetCPState *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetCPState()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id

   const uint16_t cpId = getCPState->getCpId();
   uint32_t cpState = 0;

	//make a copy for multi-threading safety
   	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

   	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
   	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

   	set<const ACS_CS_ImBase *> clusterCpObjects;
   	set<const ACS_CS_ImBase *> dualSidedCpObjects;
   	model->getObjects(clusterCpObjects, CLUSTERCP_T);
   	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

   	bool foundCp = false;

   	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
   	{
   		const ACS_CS_ImBase *base = *clusterCpIt;
   		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			cpState = static_cast<uint32_t>(cp->cpState);
   			foundCp = true;

   			break;
   		}
   	}

   	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
   	{
   		const ACS_CS_ImBase *base = *dualSidedCpIt;
   		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			cpState = 0;
   			foundCp = true;

   			break;
   		}
   	}


//   	set<const ACS_CS_ImBase *> objects;
//   	set<const ACS_CS_ImBase *>::const_iterator it;
//
//   	model->getObjects(objects, CP_T);
//   	bool foundCp = false;
//
//   	for (it = objects.begin(); it != objects.end(); ++it)
//   	{
//   		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//   		if (cp)
//   		{
//   			if (cp->systemIdentifier == cpId)
//   			{
//   				cpState = static_cast<uint32_t>(cp->cpState);
//   				foundCp = true;
//   			}
//   		}
//   	}

   	delete model;

   	if(!foundCp)
	{
		int error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		ACS_CS_Header * responseHeader = new ACS_CS_Header();
		ACS_CS_CPId_GetCPStateResponse * responseBody = new ACS_CS_CPId_GetCPStateResponse();

		if ( (responseHeader == 0) || (responseBody == 0) )
		{
			if (responseHeader)
				delete responseHeader;

			if (responseBody)
				delete responseBody;

			return -1;
		}

		responseHeader->setVersion(version);
		responseHeader->setScope(Scope_CPIdTable);
		responseHeader->setPrimitive(Primitive_GetCPStateResponse);
		responseHeader->setRequestId(requestId);

		responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		responseBody->setCpState(static_cast<ACS_CS_API_NS::CpState>(cpState));

		responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		pdu->setHeader(responseHeader);
		pdu->setPrimitive(responseBody);
	}

   return 0;

}

int ACS_CS_CPIdHandler::handleGetApplicationId (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetApplicationId()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetApplicationId()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }


   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetApplicationId * getApplicationId = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetApplicationId()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }
   
   // Get version from incoming package
   version = header->getVersion();

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetApplicationId)
   {
      getApplicationId = reinterpret_cast<const ACS_CS_CPId_GetApplicationId *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetApplicationId()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id

   const uint16_t cpId = getApplicationId->getCpId();
   uint32_t applicationId = 0;

   //make a copy for multi-threading safety
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	set<const ACS_CS_ImBase *> clusterCpObjects;
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	model->getObjects(clusterCpObjects, CLUSTERCP_T);
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	bool foundCp = false;

	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	{
		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			applicationId = static_cast<uint32_t>(cp->applicationId);
			foundCp = true;

			break;
		}
	}

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			applicationId = 0;
			foundCp = true;

			break;
		}
	}

//	set<const ACS_CS_ImBase *> objects;
//	set<const ACS_CS_ImBase *>::const_iterator it;
//
//	model->getObjects(objects, CP_T);
//	bool foundCp = false;
//
//	for (it = objects.begin(); it != objects.end(); ++it)
//	{
//		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//		if (cp)
//		{
//			if (cp->systemIdentifier == cpId)
//			{
//				applicationId = static_cast<uint32_t>(cp->applicationId);
//				foundCp = true;
//			}
//		}
//	}

	delete model;

	if(!foundCp)
	{
		int error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		 ACS_CS_Header * responseHeader = new ACS_CS_Header();
		 ACS_CS_CPId_GetApplicationIdResponse * responseBody = new ACS_CS_CPId_GetApplicationIdResponse();

		 if ( (responseHeader == 0) || (responseBody == 0) )
		 {
			if (responseHeader)
			   delete responseHeader;

			if (responseBody)
			   delete responseBody;

			return -1;
		 }

		 responseHeader->setVersion(version);
		 responseHeader->setScope(Scope_CPIdTable);
		 responseHeader->setPrimitive(Primitive_GetApplicationIdResponse);
		 responseHeader->setRequestId(requestId);

		 responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		 responseBody->setApplicationId(static_cast<ACS_CS_API_NS::ApplicationId>(applicationId));

		 responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		 pdu->setHeader(responseHeader);
		 pdu->setPrimitive(responseBody);
	}

	 return 0;
}

int ACS_CS_CPIdHandler::handleGetApzSubstate (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetApzSubstate()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetApzSubstate()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }


   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	
   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetApzSubstate * getApzSubstate = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetApzSubstate()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetApzSubstate)
   {
      getApzSubstate = reinterpret_cast<const ACS_CS_CPId_GetApzSubstate *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetApzSubstate()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id

   const uint16_t cpId = getApzSubstate->getCpId();
   uint32_t apzSubstate = 0;

   //make a copy for multi-threading safety
   	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

   	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
   	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

   	set<const ACS_CS_ImBase *> clusterCpObjects;
   	set<const ACS_CS_ImBase *> dualSidedCpObjects;
   	model->getObjects(clusterCpObjects, CLUSTERCP_T);
   	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

   	bool foundCp = false;

   	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
   	{
   		const ACS_CS_ImBase *base = *clusterCpIt;
   		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			apzSubstate = static_cast<uint32_t>(cp->apzSubstate);
   			foundCp = true;

   			break;
   		}
   	}

   	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
   	{
   		const ACS_CS_ImBase *base = *dualSidedCpIt;
   		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			apzSubstate = 0;
   			foundCp = true;

   			break;
   		}
   	}


//   	set<const ACS_CS_ImBase *> objects;
//   	set<const ACS_CS_ImBase *>::const_iterator it;
//
//   	model->getObjects(objects, CP_T);
//   	bool foundCp = false;
//
//   	for (it = objects.begin(); it != objects.end(); ++it)
//   	{
//   		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//   		if (cp)
//   		{
//   			if (cp->systemIdentifier == cpId)
//   			{
//   				apzSubstate = static_cast<uint32_t>(cp->apzSubstate);
//   				foundCp = true;
//   			}
//   		}
//   	}

   	delete model;

   	if(!foundCp)
	{
		int error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		 ACS_CS_Header * responseHeader = new ACS_CS_Header();
		 ACS_CS_CPId_GetApzSubstateResponse * responseBody = new ACS_CS_CPId_GetApzSubstateResponse();

		 if ( (responseHeader == 0) || (responseBody == 0) )
		 {
			if (responseHeader)
			   delete responseHeader;

			if (responseBody)
			   delete responseBody;

			return -1;
		 }

		 responseHeader->setVersion(version);
		 responseHeader->setScope(Scope_CPIdTable);
		 responseHeader->setPrimitive(Primitive_GetApzSubstateResponse);
		 responseHeader->setRequestId(requestId);

		 responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		 responseBody->setApzSubstate(static_cast<ACS_CS_API_NS::ApzSubstate>(apzSubstate));

		 responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		 pdu->setHeader(responseHeader);
		 pdu->setPrimitive(responseBody);
	}

	 return 0;
}

int ACS_CS_CPIdHandler::handleGetStateTransCode (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetStateTransCode()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetStateTransCode()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetStateTransCode * getStateTransCode = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetStateTransCode()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetStateTransCode)
   {
      getStateTransCode = reinterpret_cast<const ACS_CS_CPId_GetStateTransCode *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetStateTransCode()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id

   const uint16_t cpId = getStateTransCode->getCpId();
   uint32_t stateTransition = 0;

   //make a copy for multi-threading safety
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	set<const ACS_CS_ImBase *> clusterCpObjects;
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	model->getObjects(clusterCpObjects, CLUSTERCP_T);
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	bool foundCp = false;

	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	{
		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			stateTransition = static_cast<uint32_t>(cp->stateTransition);
			foundCp = true;

			break;
		}
	}

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			stateTransition = 0;
			foundCp = true;

			break;
		}
	}

//	set<const ACS_CS_ImBase *> objects;
//	set<const ACS_CS_ImBase *>::const_iterator it;
//
//	model->getObjects(objects, CP_T);
//	bool foundCp = false;
//
//	for (it = objects.begin(); it != objects.end(); ++it)
//	{
//		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//		if (cp)
//		{
//			if (cp->systemIdentifier == cpId)
//			{
//				stateTransition = static_cast<uint32_t>(cp->stateTransition);
//				foundCp = true;
//			}
//		}
//	}

	delete model;

	if(!foundCp)
	{
		int error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		 ACS_CS_Header * responseHeader = new ACS_CS_Header();
		 ACS_CS_CPId_GetStateTransCodeResponse * responseBody = new ACS_CS_CPId_GetStateTransCodeResponse();

		 if ( (responseHeader == 0) || (responseBody == 0) )
		 {
			if (responseHeader)
			   delete responseHeader;

			if (responseBody)
			   delete responseBody;

			return -1;
		 }

		 responseHeader->setVersion(version);
		 responseHeader->setScope(Scope_CPIdTable);
		 responseHeader->setPrimitive(Primitive_GetStateTransCodeResponse);
		 responseHeader->setRequestId(requestId);

		 responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		 responseBody->setStateTransCode(static_cast<ACS_CS_API_NS::StateTransition>(stateTransition));

		 responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		 pdu->setHeader(responseHeader);
		 pdu->setPrimitive(responseBody);
	}

	 return 0;
}

int ACS_CS_CPIdHandler::handleGetAptSubstate (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetAptSubstate()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetAptSubstate()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetAptSubstate * getAptSubstate = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetAptSubstate()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetAptSubstate)
   {
      getAptSubstate = reinterpret_cast<const ACS_CS_CPId_GetAptSubstate *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetAptSubstate()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id

   const uint16_t cpId = getAptSubstate->getCpId();
   uint32_t aptSubstate = 0;

   //make a copy for multi-threading safety
   	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

   	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
   	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

   	set<const ACS_CS_ImBase *> clusterCpObjects;
   	set<const ACS_CS_ImBase *> dualSidedCpObjects;
   	model->getObjects(clusterCpObjects, CLUSTERCP_T);
   	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

   	bool foundCp = false;

   	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
   	{
   		const ACS_CS_ImBase *base = *clusterCpIt;
   		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			aptSubstate = static_cast<uint32_t>(cp->aptSubstate);
   			foundCp = true;

   			break;
   		}
   	}

   	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
   	{
   		const ACS_CS_ImBase *base = *dualSidedCpIt;
   		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

   		if(cp && cp->systemIdentifier == cpId){
   			aptSubstate = 0;
   			foundCp = true;

   			break;
   		}
   	}

//   	set<const ACS_CS_ImBase *> objects;
//   	set<const ACS_CS_ImBase *>::const_iterator it;
//
//   	model->getObjects(objects, CP_T);
//   	bool foundCp = false;
//
//   	for (it = objects.begin(); it != objects.end(); ++it)
//   	{
//   		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//   		if (cp)
//   		{
//   			if (cp->systemIdentifier == cpId)
//   			{
//   				aptSubstate = static_cast<uint32_t>(cp->aptSubstate);
//   				foundCp = true;
//   			}
//   		}
//   	}

   	delete model;

   	if(!foundCp)
	{
		int error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		 ACS_CS_Header * responseHeader = new ACS_CS_Header();
		 ACS_CS_CPId_GetAptSubstateResponse * responseBody = new ACS_CS_CPId_GetAptSubstateResponse();

		 if ( (responseHeader == 0) || (responseBody == 0) )
		 {
			if (responseHeader)
			   delete responseHeader;

			if (responseBody)
			   delete responseBody;

			return -1;
		 }

		 responseHeader->setVersion(version);
		 responseHeader->setScope(Scope_CPIdTable);
		 responseHeader->setPrimitive(Primitive_GetAptSubstateResponse);
		 responseHeader->setRequestId(requestId);

		 responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		 responseBody->setAptSubstate(static_cast<ACS_CS_API_NS::AptSubstate>(aptSubstate));

		 responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		 pdu->setHeader(responseHeader);
		 pdu->setPrimitive(responseBody);
	}

	 return 0;
}

ACS_CS_Protocol::CS_Result_Code ACS_CS_CPIdHandler::getCpIdList (unsigned short* &cpList, unsigned short &cpCount)
{

   cpList = 0;
   cpCount = 0;

	//make a copy for multi-threading safety
   	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

   	set<const ACS_CS_ImBase *> ClusterObjects;
   	set<const ACS_CS_ImBase *> DualSidedObjects;
   	set<const ACS_CS_ImBase *>::const_iterator it;

   	model->getObjects(ClusterObjects, CLUSTERCP_T);
   	model->getObjects(DualSidedObjects, DUALSIDEDCP_T);

   	cpCount = ClusterObjects.size() + DualSidedObjects.size();	// Get no of search hits

   	if(cpCount > 0)
   	{
		cpList = new uint16_t[cpCount];

		int i = 0;
		for (it = ClusterObjects.begin(); it != ClusterObjects.end(); ++it)
		{
			const ACS_CS_ImClusterCp *cp = dynamic_cast<const ACS_CS_ImClusterCp *>(*it);

			if (cp)
			{
				cpList[i] = cp->systemIdentifier;
			}

			++i;
		}

		for (it = DualSidedObjects.begin(); it != DualSidedObjects.end(); ++it)
		{
			const ACS_CS_ImDualSidedCp *cp = dynamic_cast<const ACS_CS_ImDualSidedCp *>(*it);

			if (cp)
			{
				cpList[i] = cp->systemIdentifier;
			}

			++i;
		}

   	}

   	delete model;

   	return ACS_CS_Protocol::Result_Success;
}

int ACS_CS_CPIdHandler::handleGetBlockingInfo (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) %s\n"
                 "Entering method\n",__FUNCTION__));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) %s\n"
                    "Error: Invalid PDU\n",__FUNCTION__));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetBlockingInfo * blockingInfo = NULL;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) %s\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    __FUNCTION__,
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetBlockingInfo)
   {
      blockingInfo = reinterpret_cast<const ACS_CS_CPId_GetBlockingInfo *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) %s\n"
                    "Error: Wrong primitive type, type = %d\n",
                    __FUNCTION__,
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id

   const uint16_t cpId = blockingInfo->getCpId();
   ACS_CS_API_NS::BlockingInfo blockingInfoValue = 0;

	//make a copy for multi-threading safety
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	set<const ACS_CS_ImBase *> clusterCpObjects;
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	model->getObjects(clusterCpObjects, CLUSTERCP_T);
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	bool foundCp = false;

	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	{
		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			blockingInfoValue = static_cast<ACS_CS_API_NS::BlockingInfo>(cp->blockingInfo);
			foundCp = true;

			break;
		}
	}

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			blockingInfoValue = 0;
			foundCp = true;

			break;
		}
	}

//	set<const ACS_CS_ImBase *> objects;
//	set<const ACS_CS_ImBase *>::const_iterator it;
//
//	model->getObjects(objects, CP_T);
//	bool foundCp = false;
//
//	for (it = objects.begin(); it != objects.end(); ++it)
//	{
//		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//		if (cp)
//		{
//			if (cp->systemIdentifier == cpId)
//			{
//				blockingInfoValue = static_cast<ACS_CS_API_NS::BlockingInfo>(cp->blockingInfo);
//				foundCp = true;
//			}
//		}
//	}

	delete model;

	if(!foundCp)
	{
		int error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		 ACS_CS_Header * responseHeader = new ACS_CS_Header();
		 ACS_CS_CPId_GetBlockingInfoResponse * responseBody = new ACS_CS_CPId_GetBlockingInfoResponse();

		 if ( (responseHeader == 0) || (responseBody == 0) )
		 {
			if (responseHeader)
			   delete responseHeader;

			if (responseBody)
			   delete responseBody;

			return -1;
		 }

		 responseHeader->setVersion(version);
		 responseHeader->setScope(Scope_CPIdTable);
		 responseHeader->setPrimitive(Primitive_GetBlockingInfoResponse);
		 responseHeader->setRequestId(requestId);

		 responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		 responseBody->setBlockingInfo(blockingInfoValue);

		 responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		 pdu->setHeader(responseHeader);
		 pdu->setPrimitive(responseBody);
	}

	 return 0;
}

int ACS_CS_CPIdHandler::handleGetCPCapacity (ACS_CS_PDU *pdu)
{
   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) %s\n"
                 "Entering method\n",__FUNCTION__));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) %s\n"
                    "Error: Invalid PDU\n",__FUNCTION__));

      return -1;
   }

   // Get header and body from PDU
   const ACS_CS_Header * header = 0;
   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetCPCapacity * cpCapacity = NULL;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) %s\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    __FUNCTION__,
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();
	
   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetCPCapacity)
   {
      cpCapacity = reinterpret_cast<const ACS_CS_CPId_GetCPCapacity *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) %s\n"
                    "Error: Wrong primitive type, type = %d\n",
                    __FUNCTION__,
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id

   const uint16_t cpId = cpCapacity->getCpId();
   ACS_CS_API_NS::CpCapacity cpCapacityValue = 0;

	//make a copy for multi-threading safety
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	set<const ACS_CS_ImBase *> clusterCpObjects;
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	model->getObjects(clusterCpObjects, CLUSTERCP_T);
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	bool foundCp = false;

	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	{
		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			cpCapacityValue = static_cast<ACS_CS_API_NS::CpCapacity>(cp->cpCapacity);
			foundCp = true;

			break;
		}
	}

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if(cp && cp->systemIdentifier == cpId){
			cpCapacityValue = 0;
			foundCp = true;

			break;
		}
	}

//   	set<const ACS_CS_ImBase *> objects;
//   	set<const ACS_CS_ImBase *>::const_iterator it;
//
//   	model->getObjects(objects, CP_T);
//   	bool foundCp = false;
//
//   	for (it = objects.begin(); it != objects.end(); ++it)
//   	{
//   		const ACS_CS_ImCp *cp = dynamic_cast<const ACS_CS_ImCp *>(*it);
//
//   		if (cp)
//   		{
//   			if (cp->systemIdentifier == cpId)
//   			{
//   				cpCapacityValue = static_cast<ACS_CS_API_NS::CpCapacity>(cp->cpCapacity);
//   				foundCp = true;
//   			}
//   		}
//   	}

   	delete model;

   	if(!foundCp)
	{
		int error = createBasicResponse(pdu,						// Create response
						ACS_CS_Protocol::Primitive_GetCPNameResponse,
						requestId,
						ACS_CS_Protocol::Result_No_Such_Entry);
		return error;
	} else
	{
		 ACS_CS_Header * responseHeader = new ACS_CS_Header();
		 ACS_CS_CPId_GetCPCapacityResponse * responseBody = new ACS_CS_CPId_GetCPCapacityResponse();

		 if ( (responseHeader == 0) || (responseBody == 0) )
		 {
			if (responseHeader)
			   delete responseHeader;

			if (responseBody)
			   delete responseBody;

			return -1;
		 }

		 responseHeader->setVersion(version);
		 responseHeader->setScope(Scope_CPIdTable);
		 responseHeader->setPrimitive(Primitive_GetCPCapacityResponse);
		 responseHeader->setRequestId(requestId);

		 responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		 responseBody->setCPCapacity(cpCapacityValue);

		 responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		 pdu->setHeader(responseHeader);
		 pdu->setPrimitive(responseBody);
	}

	 return 0;
}
int ACS_CS_CPIdHandler::handleGetMAUType (ACS_CS_PDU *pdu)
{

   // Suppress warning for not freeing header and body
   // which we shouldn't since they are passed to the PDU
   //lint --e{429}

   ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                 "(%t) ACS_CS_CPIdHandler::handleGetMAUType()\n"
                 "Entering method\n"));

   if (pdu == 0)	// Check for valid PDU
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetMAUType()\n"
                    "Error: Invalid PDU\n"));

      return -1;
   }


   // Get header and body from PDU
   const ACS_CS_Header * header = 0;

   CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

   if (pdu && pdu->getHeader())
      header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

   const ACS_CS_Primitive * primitive = pdu->getPrimitive();
   const ACS_CS_CPId_GetMAUType * getMAUType = 0;

   // Check for valid header and body
   if ( (header == 0) || (primitive == 0) )
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetMAUType()\n"
                    "Error: Invalid header or primitive, header = %d, primitive = %d\n",
                    header,
                    primitive));

      return -1;
   }

   // Get version from incoming package
   version = header->getVersion();

   // Check primitive type and cast to correct type
   if (primitive->getPrimitiveType() == Primitive_GetMAUType)
   {
      getMAUType = reinterpret_cast<const ACS_CS_CPId_GetMAUType *> (primitive);
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_CPIdHandler_TRACE,
                    "(%t) ACS_CS_CPIdHandler::handleGetMAUType()\n"
                    "Error: Wrong primitive type, type = %d\n",
                    primitive->getPrimitiveType()));

      return -1;
   }

   unsigned short requestId = header->getRequestId();			// Get request id
   int error = -1;

   const uint16_t cpId = getMAUType->getCPId();
   int32_t mauType = 0;//to check


   //make a copy for multi-threading safety
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	
	set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

	
	set<const ACS_CS_ImBase *> dualSidedCpObjects;
	
	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

	bool foundCp = false;

	for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && foundCp == false; dualSidedCpIt++)
	{
		const ACS_CS_ImBase *base = *dualSidedCpIt;
		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		if(cp && cp->systemIdentifier == cpId){

			mauType = cp->mauType;
			foundCp = true;
			break;
		}
	}

	delete model;


	if (mauType != 0)
	{
		ACS_CS_Header * responseHeader = new ACS_CS_Header();
		ACS_CS_CPId_GetMAUTypeResponse * responseBody = new ACS_CS_CPId_GetMAUTypeResponse();

		if ( (responseHeader == 0) || (responseBody == 0) )
		{
			if (responseHeader)
				delete responseHeader;

			if (responseBody)
				delete responseBody;

			return -1;
		}

		responseHeader->setVersion(version);
		responseHeader->setScope(Scope_CPIdTable);
		responseHeader->setPrimitive(Primitive_GetMAUTypeResponse);
		responseHeader->setRequestId(requestId);

		responseBody->setLogicalClock(logicalClock);
		responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
		responseBody->setMAUType(static_cast<ACS_CS_Protocol::CS_MAU_Type_Identifier>(mauType));

		responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

		pdu->setHeader(responseHeader);
		pdu->setPrimitive(responseBody);

		error = 0;
	}
	else if(!foundCp)
	{
		error = createBasicResponse(pdu,						// Create response
				ACS_CS_Protocol::Primitive_GetMAUTypeResponse,
				requestId,
				ACS_CS_Protocol::Result_No_Such_Entry);
	}
	else
	{
		error = createBasicResponse(pdu,						// Create response
				ACS_CS_Protocol::Primitive_GetMAUTypeResponse,
				requestId,
				ACS_CS_Protocol::Result_No_Value);
	}

   return error;

}
