

//      Copyright Ericsson AB 2007. All rights reserved.


#include <algorithm>


#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_HWC_GetValue.h"
#include "ACS_CS_HWC_GetValueResponse.h"
#include "ACS_CS_HWC_GetBoardList.h"
#include "ACS_CS_HWC_GetBoardListResponse.h"
#include "ACS_CS_HWC_SynchTable.h"
#include "ACS_CS_HWC_SynchTableResponse.h"

#include "ACS_CS_CPId_GetAPZSystem.h"
#include "ACS_CS_CPId_GetAPZSystemResponse.h"
#include "ACS_CS_CPId_GetCPType.h"
#include "ACS_CS_CPId_GetCPTypeResponse.h"
#include "ACS_CS_CPId_GetCPId.h"
#include "ACS_CS_CPId_GetCPIdResponse.h"
#include "ACS_CS_CPId_GetCPList.h"
#include "ACS_CS_CPId_GetCPListResponse.h"
#include "ACS_CS_CPId_GetCPName.h"
#include "ACS_CS_CPId_GetCPNameResponse.h"
#include "ACS_CS_CPId_GetCPAliasName.h"
#include "ACS_CS_CPId_GetCPAliasNameResponse.h"
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

#include "ACS_CS_CPGroup_GetGroupMembers.h"
#include "ACS_CS_CPGroup_GetGroupMembersResponse.h"
#include "ACS_CS_CPGroup_GetGroupNames.h"
#include "ACS_CS_CPGroup_GetGroupNamesResponse.h"

#include "ACS_CS_FD_GetFunctionList.h"
#include "ACS_CS_FD_GetFunctionListResponse.h"
#include "ACS_CS_FD_GetFunctionProviders.h"
#include "ACS_CS_FD_GetFunctionProvidersResponse.h"
#include "ACS_CS_FD_GetFunctionUsers.h"
#include "ACS_CS_FD_GetFunctionUsersResponse.h"

#include "ACS_CS_NE_GetAlarmMaster.h"
#include "ACS_CS_NE_GetAlarmMasterResponse.h"
#include "ACS_CS_NE_GetBSOMIPAddress.h"
#include "ACS_CS_NE_GetBSOMIPAddressResponse.h"
#include "ACS_CS_NE_GetClockMaster.h"
#include "ACS_CS_NE_GetClockMasterResponse.h"
#include "ACS_CS_NE_GetCPSystem.h"
#include "ACS_CS_NE_GetCPSystemResponse.h"
#include "ACS_CS_NE_GetNEId.h"
#include "ACS_CS_NE_GetNEIdResponse.h"
#include "ACS_CS_NE_GetTestEnvironment.h"
#include "ACS_CS_NE_GetTestEnvironmentResponse.h"
#include "ACS_CS_NE_GetClusterOpMode.h"
#include "ACS_CS_NE_GetClusterOpModeResponse.h"
#include "ACS_CS_NE_GetTrafficLeader.h"
#include "ACS_CS_NE_GetTrafficLeaderResponse.h"
#include "ACS_CS_NE_GetTrafficIsolated.h"
#include "ACS_CS_NE_GetTrafficIsolatedResponse.h"

#include "ACS_CS_NE_GetOmProfile.h"
#include "ACS_CS_NE_GetOmProfileResponse.h"
#include "ACS_CS_VLAN_GetVLANTable.h"
#include "ACS_CS_VLAN_GetVLANTableResponse.h"
#include "ACS_CS_VLAN_GetVLANList.h"
#include "ACS_CS_VLAN_GetVLANListResponse.h"
#include "ACS_CS_VLAN_GetVLANAddress.h"
#include "ACS_CS_VLAN_GetVLANAddressResponse.h"
#include "ACS_CS_VLAN_GetNetworkTable.h"
#include "ACS_CS_VLAN_GetNetworkTableResponse.h"

#include "ACS_CS_GLK_GetGlobalLogicalClock.h"
#include "ACS_CS_GLK_GetGlobalLogicalClockResponse.h"

#include "ACS_CS_SM_SubscribeTable.h"
#include "ACS_CS_SM_SubscribeProfilePhaseChange.h"
#include "ACS_CS_SM_ProfilePhaseNotify.h"
#include "ACS_CS_SM_CPTableNotify.h"
#include "ACS_CS_SM_NETableNotify.h"
#include "ACS_CS_SM_CPGroupTableNotify.h"
#include "ACS_CS_SM_HWCTableNotify.h"

// MODD protocol
#include "ACS_CS_MODD_Header.h"
#include "ACS_CS_MODD_Change.h"
#include "ACS_CS_MODD_ChangeResponse.h"
#include "ACS_CS_MODD_Revert.h"
#include "ACS_CS_MODD_RevertResponse.h"

#include "ACS_CS_Trace.h"


// ACS_CS_Parser
#include "ACS_CS_Parser.h"


ACS_CS_Trace_TDEF(ACS_CS_Parser_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_Parser 

ACS_CS_Parser::ACS_CS_Parser(ACS_CS_Protocol::CS_Protocol_Type type)
   : bufferSize(0),
     currentHeader(0),
     protocolType(type)
{
}


ACS_CS_Parser::~ACS_CS_Parser()
{

   this->flush();       // Clear the buffer

}



int ACS_CS_Parser::newData (const char *buffer, int size)
{

   // Suppress warning for not freeing pointer temp,
   // which we shouldn't since it is stored in an STL queue.
   // Queue and content freed in destructor
   //lint --e{429}

   if (buffer == 0)     // Check for valid memory
   {
      ACS_CS_TRACE((ACS_CS_Parser_TRACE,
                    "(%t) ACS_CS_Parser::newData()\n"
                    "Error: Null pointer\n"));

      return -1;
   }
   else if (size < 1)   // Check for valid size;
   {
      ACS_CS_TRACE((ACS_CS_Parser_TRACE,
                    "(%t) ACS_CS_Parser::newData()\n"
                    "Error: Invalid size, size = %d\n",
                    size));

      return -1;
   }

   char * tempBuffer = new char[size];                  // Create temporary buffer

   if (tempBuffer)      // Check for successful new
   {
      memcpy(tempBuffer, buffer, size);         // Copy data to buffer

      DataBuffer * temp = new DataBuffer;               // Create struct to hold buffer

      if (temp)                                                         // If successful new
      {
         temp->buffer = tempBuffer;                     // Set struct with buffer pointer
         temp->size = size;                                     // Set struct with buffer size

         this->bufferQueue.push(temp);          // Store struct in queue

         this->bufferSize += size;                      // Increase buffer size

         return 0;
      }
      else
      {
         delete [] tempBuffer;
      }

   }

   return -1; // An error if this point is reached

}

void ACS_CS_Parser::flush ()
{

   // Loop through buffer queue
   while ( ! this->bufferQueue.empty() )
   {
      DataBuffer * buffer = this->bufferQueue.front();  // Get buffer struct
      this->bufferQueue.pop();                                                  // Remove struct from queue
      delete [] buffer->buffer;                                                 // Delete buffer
      delete buffer;                                                                            // Delete struct
   }

   this->bufferSize = 0;

   // Loop through PDU queue
   while ( ! this->pduQueue.empty() )
   {
      ACS_CS_PDU * pdu = this->pduQueue.front();                // Get PDU
      this->pduQueue.pop();                                                             // Remove PDU from queue
      delete pdu;
   }

   if (this->currentHeader)                                                             // If there is a header, delete it
   {
      delete this->currentHeader;
      this->currentHeader = 0;
   }

}

bool ACS_CS_Parser::getPDU (ACS_CS_PDU &pdu)
{

   bool pduReturned = false;

   if ( ! this->pduQueue.empty() )      // If the queue isn't empty
   {
      ACS_CS_PDU * frontPdu = this->pduQueue.front();   // get first PDU
      this->pduQueue.pop();                                                                     // remove it from the queue

      if (frontPdu)
      {
         pdu = *frontPdu;
         delete frontPdu;
         pduReturned = true;
      }
   }

   return pduReturned;

}

int ACS_CS_Parser::parse ()
{

   // Suppress warning for not freeing new PDU created
   // (which we shouldn't since it is stored in an STL queue)
   // Queue and content freed in destructor
   //lint --e{429}

   /*
     This function parses the data queue and creates PDUs. If the data cannot be parsed because
     of an error other than faulty data, it returns -1. Otherwise it returns 0 for successful
     parsing or throws an Exception for faulty data.
   */

   ACS_CS_TRACE((ACS_CS_Parser_TRACE,
                 "(%t) ACS_CS_Parser::parse()\n"
                 "Entering function, pduQueue = %d pdus, bufferQueue = %d buffers, bufferSize = %d bytes\n",
                 this->pduQueue.size(),
                 this->bufferQueue.size(),
                 this->bufferSize ));

   int error = 0;

   const int currentHeaderSize = ACS_CS_Protocol::CS_ProtocolChecker::getHeaderSize(protocolType);

   while (this->bufferSize > 0) // Go on parsing as long as there is data to parse
   {
      if (this->currentHeader == 0)                     // No header parsed, parse one
      {
         if (this->bufferSize < currentHeaderSize) // Check if there is enough data for a header
            break;

         char * headBuff = new char[currentHeaderSize];       // Get data for header

         if (headBuff == 0)                                                     // Check for successful new
         {
            error = -1;
            break;
         }

         if (this->getData(headBuff, currentHeaderSize) < 0)  // Get data for header
         {
            ACS_CS_TRACE((ACS_CS_Parser_TRACE,
                          "(%t) ACS_CS_Parser::parse()\n"
                          "Error: Cannot get data for header\n"));

            delete [] headBuff;
            error = -1;
            break;
         }

         if (this->protocolType == ACS_CS_Protocol::Protocol_CS)
         {
        	 this->currentHeader = new ACS_CS_Header();     // Create new header object
         }
         else if (this->protocolType == ACS_CS_Protocol::Protocol_MODD)
         {
        	 this->currentHeader = new ACS_CS_MODD_Header();     // Create new header object
         }

         if (this->currentHeader == 0)                                  // Check for successful new
         {
            delete [] headBuff;
            error = -1;
            break;
         }

         try
         {
            if (this->currentHeader->setBuffer(headBuff, currentHeaderSize) < 0)      // Set buffer to header
            {
               error = -1;
               delete this->currentHeader;
               this->currentHeader = 0;
            }
         }
         catch (ACS_CS_Exception ex)
         {
            delete this->currentHeader;
            this->currentHeader = 0;
            delete [] headBuff;
            throw;
         }

         delete [] headBuff;
      }

      if (this->currentHeader)  // Header already parsed, parse a primitive
      {
         // Calculate primitive size
         int primSize = this->currentHeader->getTotalLength() - this->currentHeader->getHeaderLength();
         if (this->bufferSize < primSize)       // Not enough data for whole primitive.
         {
            break;                                                      // We must wait for more.
         }

         char *primBuffer = new char[primSize];                         // Allocate memory for primitive

         if (primBuffer)                                                                                // Check for successful new
         {
            if (this->getData(primBuffer, primSize) < 0)        // Get data for primitive
            {
               ACS_CS_TRACE((ACS_CS_Parser_TRACE,
                             "(%t) ACS_CS_Parser::parse()\n"
                             "Error: Cannot get data for primitive\n"));

               delete [] primBuffer;
               error = -1;
               break;
            }
         }
         else                                                                                           // Unsuccessful new
         {
            error = -1;
            break;
         }

         if( Primitive_NotSpecified == currentHeader->getPrimitiveId() )
         {
			ACS_CS_TRACE((ACS_CS_Parser_TRACE,"(%t) %s \n, Error: primitive unknown, skipped it\n",
						  __FUNCTION__ ));

			delete [] primBuffer;
			delete currentHeader;
			currentHeader = 0;
			continue;
         }

         ACS_CS_Primitive * primitive = 0;                              // Create primitive object

			switch (this->currentHeader->getPrimitiveId())
			// Check which primitive and create it
			{
			case Primitive_GetValue:
				primitive = new ACS_CS_HWC_GetValue();
				break;

			case Primitive_GetValueResponse:
				primitive = new ACS_CS_HWC_GetValueResponse();
				break;

			case Primitive_GetBoardList:
				primitive = new ACS_CS_HWC_GetBoardList();
				break;

			case Primitive_GetBoardListResponse:
				primitive = new ACS_CS_HWC_GetBoardListResponse();
				break;

			case Primitive_SynchTable:
				primitive = new ACS_CS_HWC_SynchTable();
				break;

			case Primitive_SynchTableResponse:
				primitive = new ACS_CS_HWC_SynchTableResponse();
				break;

			case Primitive_GetAPZSystem:
				primitive = new ACS_CS_CPId_GetAPZSystem();
				break;

			case Primitive_GetAPZSystemResponse:
				primitive = new ACS_CS_CPId_GetAPZSystemResponse();
				break;

			case Primitive_GetCPType:
				primitive = new ACS_CS_CPId_GetCPType();
				break;

			case Primitive_GetCPTypeResponse:
				primitive = new ACS_CS_CPId_GetCPTypeResponse();
				break;

			case Primitive_GetCPId:
				primitive = new ACS_CS_CPId_GetCPId();
				break;

			case Primitive_GetCPIdResponse:
				primitive = new ACS_CS_CPId_GetCPIdResponse();
				break;

			case Primitive_GetCPList:
				primitive = new ACS_CS_CPId_GetCPList();
				break;

			case Primitive_GetCPListResponse:
				primitive = new ACS_CS_CPId_GetCPListResponse();
				break;

			case Primitive_GetCPName:
				primitive = new ACS_CS_CPId_GetCPName();
				break;

			case Primitive_GetCPNameResponse:
				primitive = new ACS_CS_CPId_GetCPNameResponse();
				break;

			case Primitive_GetCPAliasName:
				primitive = new ACS_CS_CPId_GetCPAliasName();
				break;

			case Primitive_GetCPAliasNameResponse:
				primitive = new ACS_CS_CPId_GetCPAliasNameResponse();
				break;

			case Primitive_GetCPState:
				primitive = new ACS_CS_CPId_GetCPState();
				break;

			case Primitive_GetCPStateResponse:
				primitive = new ACS_CS_CPId_GetCPStateResponse();
				break;

			case Primitive_GetApplicationId:
				primitive = new ACS_CS_CPId_GetApplicationId();
				break;

			case Primitive_GetApplicationIdResponse:
				primitive = new ACS_CS_CPId_GetApplicationIdResponse();
				break;

			case Primitive_GetApzSubstate:
				primitive = new ACS_CS_CPId_GetApzSubstate();
				break;

			case Primitive_GetApzSubstateResponse:
				primitive = new ACS_CS_CPId_GetApzSubstateResponse();
				break;

			case Primitive_GetStateTransCode:
				primitive = new ACS_CS_CPId_GetStateTransCode();
				break;

			case Primitive_GetStateTransCodeResponse:
				primitive = new ACS_CS_CPId_GetStateTransCodeResponse();
				break;

			case Primitive_GetAptSubstate:
				primitive = new ACS_CS_CPId_GetAptSubstate();
				break;

			case Primitive_GetAptSubstateResponse:
				primitive = new ACS_CS_CPId_GetAptSubstateResponse();
				break;

			case Primitive_GetMAUType:
				primitive = new ACS_CS_CPId_GetMAUType();
				break;

			case Primitive_GetMAUTypeResponse:
				primitive = new ACS_CS_CPId_GetMAUTypeResponse();
				break;

			case Primitive_GetGroupMembers:
				primitive = new ACS_CS_CPGroup_GetGroupMembers();
				break;

			case Primitive_GetGroupMembersResponse:
				primitive = new ACS_CS_CPGroup_GetGroupMembersResponse();
				break;

			case Primitive_GetGroupNames:
				primitive = new ACS_CS_CPGroup_GetGroupNames();
				break;

			case Primitive_GetGroupNamesResponse:
				primitive = new ACS_CS_CPGroup_GetGroupNamesResponse();
				break;

			case Primitive_GetFunctionList:
				primitive = new ACS_CS_FD_GetFunctionList();
				break;

			case Primitive_GetFunctionListResponse:
				primitive = new ACS_CS_FD_GetFunctionListResponse();
				break;

			case Primitive_GetFunctionProviders:
				primitive = new ACS_CS_FD_GetFunctionProviders();
				break;

			case Primitive_GetFunctionProvidersResponse:
				primitive = new ACS_CS_FD_GetFunctionProvidersResponse();
				break;

			case Primitive_GetFunctionUsers:
				primitive = new ACS_CS_FD_GetFunctionUsers();
				break;

			case Primitive_GetFunctionUsersResponse:
				primitive = new ACS_CS_FD_GetFunctionUsersResponse();
				break;

			case Primitive_GetAlarmMaster:
				primitive = new ACS_CS_NE_GetAlarmMaster();
				break;

			case Primitive_GetAlarmMasterResponse:
				primitive = new ACS_CS_NE_GetAlarmMasterResponse();
				break;

			case Primitive_GetBSOMIPAddress:
				primitive = new ACS_CS_NE_GetBSOMIPAddress();
				break;

			case Primitive_GetBSOMIPAddressResponse:
				primitive = new ACS_CS_NE_GetBSOMIPAddressResponse();
				break;

			case Primitive_GetClockMaster:
				primitive = new ACS_CS_NE_GetClockMaster();
				break;

			case Primitive_GetClockMasterResponse:
				primitive = new ACS_CS_NE_GetClockMasterResponse();
				break;

			case Primitive_GetCPSystem:
				primitive = new ACS_CS_NE_GetCPSystem();
				break;

			case Primitive_GetCPSystemResponse:
				primitive = new ACS_CS_NE_GetCPSystemResponse();
				break;

			case Primitive_GetNEId:
				primitive = new ACS_CS_NE_GetNEId();
				break;

			case Primitive_GetNEIdResponse:
				primitive = new ACS_CS_NE_GetNEIdResponse();
				break;

			case Primitive_GetTestEnvironment:
				primitive = new ACS_CS_NE_GetTestEnvironment();
				break;

			case Primitive_GetTestEnvironmentResponse:
				primitive = new ACS_CS_NE_GetTestEnvironmentResponse();
				break;

			case Primitive_GetClusterOpMode:
				primitive = new ACS_CS_NE_GetClusterOpMode();
				break;

			case Primitive_GetClusterOpModeResponse:
				primitive = new ACS_CS_NE_GetClusterOpModeResponse();
				break;

			case Primitive_GetOmProfile:
				primitive = new ACS_CS_NE_GetOmProfile();
				break;

			case Primitive_GetOmProfileResponse:
				primitive = new ACS_CS_NE_GetOmProfileResponse();
				break;

			case Primitive_GetTrafficLeader:
				primitive = new ACS_CS_NE_GetTrafficLeader();
				break;

			case Primitive_GetTrafficLeaderResponse:
				primitive = new ACS_CS_NE_GetTrafficLeaderResponse();
				break;

			case Primitive_GetTrafficIsolated:
				primitive = new ACS_CS_NE_GetTrafficIsolated();
				break;

			case Primitive_GetTrafficIsolatedResponse:
				primitive = new ACS_CS_NE_GetTrafficIsolatedResponse();
				break;

			case Primitive_GetVLANList:
				primitive = new ACS_CS_VLAN_GetVLANList();
				break;

			case Primitive_GetVLANListResponse:
				primitive = new ACS_CS_VLAN_GetVLANListResponse();
				break;

			case Primitive_GetVLANTable:
				primitive = new ACS_CS_VLAN_GetVLANTable();
				break;

			case Primitive_GetVLANTableResponse:
				primitive = new ACS_CS_VLAN_GetVLANTableResponse();
				break;

			case Primitive_GetVLANAddress:
				primitive = new ACS_CS_VLAN_GetVLANAddress();
				break;

			case Primitive_GetVLANAddressResponse:
				primitive = new ACS_CS_VLAN_GetVLANAddressResponse();
				break;

			case Primitive_GetNetworkTable:
				primitive = new ACS_CS_VLAN_GetNetworkTable();
				break;

			case Primitive_GetNetworkTableResponse:
				primitive = new ACS_CS_VLAN_GetNetworkTableResponse();
				break;

			case Primitive_GetGlobalLogicalClock:
				primitive = new ACS_CS_GLK_GetGlobalLogicalClock();
				break;

			case Primitive_GetGlobalLogicalClockResponse:
				primitive = new ACS_CS_GLK_GetGlobalLogicalClockResponse();
				break;

			case Primitive_NotifyHWCTableChange:
				primitive = new ACS_CS_SM_HWCTableNotify();
				break;

			case Primitive_NotifyCpidTableChange:
				primitive = new ACS_CS_SM_CPTableNotify();
				break;

			case Primitive_NotifyNeTableChange:
				primitive = new ACS_CS_SM_NETableNotify();
				break;

			case Primitive_NotifyProfilePhaseChange:
				primitive = new ACS_CS_SM_ProfilePhaseNotify();
				break;

			case Primitive_NotifyCpGroupTableChange:
				primitive = new ACS_CS_SM_CPGroupTableNotify();
				break;

			case Primitive_SubscribeTable:
				primitive = new ACS_CS_SM_SubscribeTable();
				break;

			case Primitive_SubscribeProfilePhaseChange:
				primitive = new ACS_CS_SM_SubscribeProfilePhaseChange();
				break;

			case Primitive_GetBlockingInfo:
				primitive = new ACS_CS_CPId_GetBlockingInfo();
				break;

			case Primitive_GetBlockingInfoResponse:
				primitive = new ACS_CS_CPId_GetBlockingInfoResponse();
				break;

			case Primitive_GetCPCapacity:
				primitive = new ACS_CS_CPId_GetCPCapacity();
				break;

			case Primitive_GetCPCapacityResponse:
				primitive = new ACS_CS_CPId_GetCPCapacityResponse();
				break;

			case Primitive_MODD_Change:
				primitive = new ACS_CS_MODD_Change();
				break;

			case Primitive_MODD_ChangeResponse:
				primitive = new ACS_CS_MODD_ChangeResponse();
				break;

			case Primitive_MODD_Revert:
				primitive = new ACS_CS_MODD_Revert();
				break;

			case Primitive_MODD_RevertResponse:
				primitive = new ACS_CS_MODD_RevertResponse();
				break;

            default                                   :   ACS_CS_ParsingException ex;
               ex.setDescription("Not a valid length");
               throw ex; // Protocol error
               break;
         }

         if (primitive) // If primitive created
         {
            try
            {
               if (primitive->setBuffer(primBuffer, primSize) < 0)                      // Set data in primitive
               {
            	  ACS_CS_TRACE((ACS_CS_Parser_TRACE,
            	  		"(%t) [%s@%d] ERROR: cannot set buffer of primitive", __FUNCTION__, __LINE__));

                  error = -1;
                  delete primitive;
                  primitive = 0;
               }
            }
            catch (ACS_CS_Exception ex)
            {
            	ACS_CS_TRACE((ACS_CS_Parser_TRACE,
            			"(%t) [%s@%d] ERROR: exception while setting buffer of primitive", __FUNCTION__, __LINE__));
               delete [] primBuffer;
               delete primitive;
               primitive = 0;
               throw;
            }

            // Create PDU
            ACS_CS_PDU * newPDU = new ACS_CS_PDU(currentHeader, primitive);

            if (newPDU)                                                         // Check for successful new
            {
               this->pduQueue.push(newPDU);             // Store PDU in queue
               currentHeader = 0;
            }
            else                                                                        // Unsuccessful new
            {
               delete [] primBuffer;
               error = -1;
               break;
            }
         }
         else                                                                                   // Unsuccessful new
         {
            delete [] primBuffer;
            error = -1;
            break;
         }

         delete [] primBuffer;                                                  // Delete temporary buffer
      }
   }

	ACS_CS_TRACE((ACS_CS_Parser_TRACE,
		"(%t) ACS_CS_Parser::parse()\n"
		"Leaving function, pduQueue = %d pdus, bufferQueue = %d buffers, bufferSize = %d bytes\n",
		this->pduQueue.size(),
		this->bufferQueue.size(),
		this->bufferSize ));

   return error;

}

int ACS_CS_Parser::getData (char *buffer, int size)
{

   // Suppress warning for not freeing buffer
   // (which we shouldn't since it is a value-result argument)
   //lint --e{429}

   if (this->bufferSize < size) // Check for enough data in the queue
   {
      ACS_CS_TRACE((ACS_CS_Parser_TRACE,
                    "(%t) ACS_CS_Parser::getData()\n"
                    "Error: Invalid request, this->bufferSize = %d, size = %d\n",
                    this->bufferSize,
                    size));

      return -1;
   }

   int dataCopied = 0;                                  // Data copied so far
   DataBuffer * currentBuffer = 0;              // The current buffer from the queue

   while (dataCopied < size)                    // While there still is data to copy
   {
      currentBuffer = this->bufferQueue.front();        // Get first buffer

      // Copy data. Copy what is in the buffer but no more than (size - dataCopied)
      // i.e. copy only what is left to copy to satisfy the request
      int currentCopy = std::min( (size - dataCopied), currentBuffer->size); // What can be copied from this buffer
      memcpy(buffer + dataCopied, currentBuffer->buffer, currentCopy);  // Copy from the buffer
      dataCopied += currentCopy;                                                        // Increase data copied so far

      // If we only copied part of current buffer, change the buffer
      if (currentCopy < currentBuffer->size)
      {
         int dataLeft = currentBuffer->size - currentCopy;      // Data that should be left in buffer
         char * newBuffer = 0;

         if (dataLeft > 0)
            newBuffer = new char[dataLeft];                             // Create new buffer

			if (newBuffer)										// Check for successful new
				memcpy(newBuffer, currentBuffer->buffer + currentCopy, dataLeft);	// Copy data
			else
				return -1;

         // Adjust the old buffer
         delete [] currentBuffer->buffer;
         currentBuffer->buffer = newBuffer;
         currentBuffer->size = dataLeft;
      }
      else      // Whole buffer copied, delete it from the queue
      {
         this->bufferQueue.pop();
         delete [] currentBuffer->buffer;
         delete currentBuffer;
      }

      // Adjust buffer size
      this->bufferSize -= currentCopy;
   }

   return 0;

}

int ACS_CS_Parser::getBufferSize () const
{

   return this->bufferSize;

}

int ACS_CS_Parser::getPDUQueue () const
{

   return static_cast<int>(this->pduQueue.size());

}

// Additional Declarations

