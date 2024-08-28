

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"


// ACS_CS_VLAN_GetVLANListResponse
#include "ACS_CS_VLAN_GetVLANListResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_VLAN_GetVLANListResponse_TRACE);

using namespace ACS_CS_Protocol;
using std::vector;



// Class ACS_CS_VLAN_GetVLANListResponse 

ACS_CS_VLAN_GetVLANListResponse::ACS_CS_VLAN_GetVLANListResponse()
      : length(0),
        logicalClock(0)
{
}

ACS_CS_VLAN_GetVLANListResponse::ACS_CS_VLAN_GetVLANListResponse(const ACS_CS_VLAN_GetVLANListResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->length = right.length;
   this->logicalClock = right.logicalClock;
   this->vlanData = right.vlanData;

}


ACS_CS_VLAN_GetVLANListResponse::~ACS_CS_VLAN_GetVLANListResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_VLAN_GetVLANListResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetVLANListResponse;

}

 int ACS_CS_VLAN_GetVLANListResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANListResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size != 2) && (size != 6) && (size < 18) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANListResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Clear current vector
   this->vlanData.clear();
   this->length = 2;

   // Get and convert result code
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   if (CS_ProtocolChecker::checkResultCode(tempShort))
      this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
      ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANListResponse::setBuffer()\n"
			"Error: Illegal result code , result= %d\n",
			tempShort ));

		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

   if ( ((size == 6) && (this->resultCode == ACS_CS_Protocol::Result_Success)) ||
        ((size >= 18) && (this->resultCode == ACS_CS_Protocol::Result_Success)) )
   {
      unsigned short responseCount = 0;
      unsigned short offset = 6;
      unsigned short currentNameLength = 0;

      // Get and convert logical clock
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->logicalClock = ntohs(tempShort);
      this->length += 2;

      // Get and convert response count
      tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
      responseCount = ntohs(tempShort);
      this->length += 2;

      for (int i = 0; i < responseCount; i++)
      {
         // Check that the remaining space is enough
         if ( size < ( offset + 12) )
         {
            // Clear current vector
            this->vlanData.clear();
            this->length = 2;

            ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			      "(%t) ACS_CS_VLAN_GetVLANListResponse::setBuffer()\n"
			      "Error: Illegal buffer size , size= %d\n",
			      size ));

		      ACS_CS_ParsingException ex;
		      ex.setDescription("Illegal buffer size");
		      throw ex;
         }

         tempShort = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
         currentNameLength = ntohs(tempShort);
         offset += 2;

         // Check that there is space for the name
         if ( (size < (offset + 10 + currentNameLength) ) ||
            (currentNameLength > ACS_CS_NS::MAX_VLAN_NAME_LENGTH) )
         {
            // Clear current vector
            this->vlanData.clear();
            this->length = 2;

            ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			      "(%t) ACS_CS_VLAN_GetVLANListResponse::setBuffer()\n"
			      "Error: Illegal string size , size = %d, string length = %d\n",
			      size, currentNameLength ));

		      ACS_CS_ParsingException ex;
		      ex.setDescription("Illegal string size");
		      throw ex;
         }

         ACS_CS_VLAN_DATA tempData;
         memset(&tempData, 0, sizeof(tempData));

         // Copy VLAN name # i
         memcpy(tempData.vlanName, buffer + offset, currentNameLength);
         tempData.vlanName[ACS_CS_NS::MAX_VLAN_NAME_LENGTH] = 0;
         offset += currentNameLength;

         // Copy Network address # i
         uint32_t tempLong = 0;
         memcpy(&tempLong, buffer + offset, sizeof(tempLong));
         tempData.vlanAddress = ntohl(tempLong);
         offset += sizeof(tempLong);

         // Copy Netmask # i
         memcpy(&tempLong, buffer + offset, sizeof(tempLong));
         tempData.vlanNetmask = ntohl(tempLong);
         offset += sizeof(tempLong);

         // Copy VLAN protocol stack # i
         memcpy(&tempShort, buffer + offset, sizeof(tempShort));
         tempData.vlanStack = ntohs(tempShort);
         offset += sizeof(tempShort);

         if ( (tempData.vlanStack != ACS_CS_Protocol::Stack_KIP) &&
            (tempData.vlanStack != ACS_CS_Protocol::Stack_TIP) )
         {
            // Clear current vector
            this->vlanData.clear();
            this->length = 2;

            ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			      "(%t) ACS_CS_VLAN_GetVLANListResponse::setBuffer()\n"
			      "Error: Illegal VLAN protocol stack , type = %d\n",
			      tempData.vlanStack ));

		      // Some error code
		      ACS_CS_AttributeException ex;
		      ex.setDescription("Error casting VLAN protocol stack");
		      throw ex;
         }

         // Store VLAN data
         this->vlanData.push_back(tempData);
         this->length += (currentNameLength + 12);
      }
   }

   return 0;

}

 int ACS_CS_VLAN_GetVLANListResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANListResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANListResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANListResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;
   unsigned short offset = 0;
   unsigned short currentNameLength = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer + offset, &tempShort, sizeof(tempShort));
   offset += sizeof(tempShort);

   if (this->getLength() > 2)
   {
      // Convert and copy logical clock
      tempShort = htons(this->logicalClock);
	   memcpy(buffer + offset, &tempShort, sizeof(tempShort));
      offset += sizeof(tempShort);

      // Convert and copy response count
      tempShort = htons(static_cast<unsigned short>(this->vlanData.size()));
	   memcpy(buffer + offset, &tempShort, sizeof(tempShort));
      offset += sizeof(tempShort);

      vector<ACS_CS_VLAN_DATA> tempVector = this->vlanData;
      vector<ACS_CS_VLAN_DATA>::iterator it;

      for (it = tempVector.begin(); it != tempVector.end(); ++it)
      {
         ACS_CS_VLAN_DATA tempData = *it;
         tempData.vlanName[ACS_CS_NS::MAX_VLAN_NAME_LENGTH] = 0;
         currentNameLength = static_cast<unsigned short>(strlen(tempData.vlanName));
         tempShort = htons(currentNameLength);;

         //Copy name length
         memcpy(buffer + offset, &tempShort, sizeof(tempShort));
         offset += sizeof(tempShort);

         //Copy VLAN name
         memcpy(buffer + offset, tempData.vlanName, currentNameLength);
         offset += currentNameLength;

         // Convert and copy network address
         uint32_t tempLong = htonl(tempData.vlanAddress);
         memcpy(buffer + offset, &tempLong, sizeof(tempLong));
         offset += sizeof(tempLong);

         // Convert and copy netmask
         tempLong = htonl(tempData.vlanNetmask);
         memcpy(buffer + offset, &tempLong, sizeof(tempLong));
         offset += sizeof(tempLong);

         // Convert and copy VLAN protocol stack
         tempShort = htons(tempData.vlanStack);
         memcpy(buffer + offset, &tempShort, sizeof(tempShort));
         offset += sizeof(tempShort);
      }
   }

   return 0;

}

 short unsigned ACS_CS_VLAN_GetVLANListResponse::getLength () const
{

   if (resultCode == ACS_CS_Protocol::Result_Success)
      return this->length;    // Result code + content
   else
      return 2;               // Only result code

}

 ACS_CS_Primitive * ACS_CS_VLAN_GetVLANListResponse::clone () const
{

   return new ACS_CS_VLAN_GetVLANListResponse(*this);

}

 void ACS_CS_VLAN_GetVLANListResponse::getVLANVector (std::vector<ACS_CS_VLAN_DATA> &vlanVector) const
{

   vlanVector = this->vlanData;

}

 void ACS_CS_VLAN_GetVLANListResponse::setVLANVector (std::vector<ACS_CS_VLAN_DATA> vlanVector)
{

   this->vlanData.clear();
   this->length = 6;

   vector<ACS_CS_VLAN_DATA>::iterator it;

   for (it = vlanVector.begin(); it != vlanVector.end(); ++it)
   {
      ACS_CS_VLAN_DATA tempData = *it;
      tempData.vlanName[ACS_CS_NS::MAX_VLAN_NAME_LENGTH] = 0;
      length += static_cast<unsigned short>(strlen(tempData.vlanName));
      length += 12;

      this->vlanData.push_back(*it);
   }

}

 short unsigned ACS_CS_VLAN_GetVLANListResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_VLAN_GetVLANListResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

// Additional Declarations

