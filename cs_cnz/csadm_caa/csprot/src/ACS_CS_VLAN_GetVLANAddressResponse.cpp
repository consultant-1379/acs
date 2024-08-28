

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_VLAN_GetVLANAddressResponse
#include "ACS_CS_VLAN_GetVLANAddressResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_VLAN_GetVLANAddressResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_VLAN_GetVLANAddressResponse 

ACS_CS_VLAN_GetVLANAddressResponse::ACS_CS_VLAN_GetVLANAddressResponse()
      : ipAddress(0),
        logicalClock(0)
{
}

ACS_CS_VLAN_GetVLANAddressResponse::ACS_CS_VLAN_GetVLANAddressResponse(const ACS_CS_VLAN_GetVLANAddressResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->ipAddress = right.ipAddress;
   this->logicalClock = right.logicalClock;

}


ACS_CS_VLAN_GetVLANAddressResponse::~ACS_CS_VLAN_GetVLANAddressResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_VLAN_GetVLANAddressResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetVLANAddressResponse;

}

 int ACS_CS_VLAN_GetVLANAddressResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddressResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddressResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size != 2) && (size != 8) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddressResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddressResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	// Get and convert result code
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   if (CS_ProtocolChecker::checkResultCode(tempShort))
      this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

   if ( (size == 8) && (this->resultCode == ACS_CS_Protocol::Result_Success) )
   {
      // Get and convert logical clock
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->logicalClock = ntohs(tempShort);

      // Get and convert IP address
      uint32_t tempLong = 0;
	   tempLong = *( reinterpret_cast<const uint32_t *>(buffer + 4) );
      this->ipAddress = ntohl(tempLong);
   }

   return 0;

}

 int ACS_CS_VLAN_GetVLANAddressResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddressResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddressResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size (at least four 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddressResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddressResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);

   if (size == 8)
   {
      // Convert and copy logical clock
      tempShort = htons(this->logicalClock);
	   memcpy(buffer + 2, &tempShort, 2);

      // Convert and copy IP address
	   uint32_t tempLong = htonl(this->ipAddress);
	   memcpy(buffer + 4, &tempLong, 4);
   }

   return 0;

}

 short unsigned ACS_CS_VLAN_GetVLANAddressResponse::getLength () const
{

   if (resultCode == ACS_CS_Protocol::Result_Success)
      return 8;   // Result code + logical clock + IP address
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_VLAN_GetVLANAddressResponse::clone () const
{

   return new ACS_CS_VLAN_GetVLANAddressResponse(*this);

}

uint32_t ACS_CS_VLAN_GetVLANAddressResponse::getAddress () const
{

   return this->ipAddress;

}

 void ACS_CS_VLAN_GetVLANAddressResponse::setAddress (uint32_t address)
{

   this->ipAddress = address;

}

 short unsigned ACS_CS_VLAN_GetVLANAddressResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_VLAN_GetVLANAddressResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

// Additional Declarations

