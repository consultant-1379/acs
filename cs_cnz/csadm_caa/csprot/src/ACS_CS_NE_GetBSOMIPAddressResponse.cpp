

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetBSOMIPAddressResponse
#include "ACS_CS_NE_GetBSOMIPAddressResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetBSOMIPAddressResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_NE_GetBSOMIPAddressResponse 

ACS_CS_NE_GetBSOMIPAddressResponse::ACS_CS_NE_GetBSOMIPAddressResponse()
      : ipEthA(0),
        ipEthB(0)
{
}

ACS_CS_NE_GetBSOMIPAddressResponse::ACS_CS_NE_GetBSOMIPAddressResponse(const ACS_CS_NE_GetBSOMIPAddressResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->ipEthA = right.ipEthA;
   this->ipEthB = right.ipEthB;

}


ACS_CS_NE_GetBSOMIPAddressResponse::~ACS_CS_NE_GetBSOMIPAddressResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetBSOMIPAddressResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetBSOMIPAddressResponse;

}

 int ACS_CS_NE_GetBSOMIPAddressResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetBSOMIPAddressResponse_TRACE,
			"(%t) ACS_CS_NE_GetBSOMIPAddressResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size != 2) && (size != 10) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_NE_GetBSOMIPAddressResponse_TRACE,
			"(%t) ACS_CS_NE_GetBSOMIPAddressResponse::setBuffer()\n"
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

   if (size == 10)
   {
      // Get and convert IP EthA
	   uint32_t tempLong = 0;
	   tempLong = *( reinterpret_cast<const uint32_t *>(buffer + 2) );
      this->ipEthA = ntohl(tempLong);

      // Get and convert IP EthB
	   tempLong = 0;
	   tempLong = *( reinterpret_cast<const uint32_t *>(buffer + 6) );
      this->ipEthB = ntohl(tempLong);
   }

   return 0;

}

 int ACS_CS_NE_GetBSOMIPAddressResponse::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetBSOMIPAddressResponse_TRACE,
			"(%t) ACS_CS_NE_GetBSOMIPAddressResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_NE_GetBSOMIPAddressResponse_TRACE,
			"(%t) ACS_CS_NE_GetBSOMIPAddressResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);

   if (size == 10)
   {
      // Convert and copy IP EthA
	   uint32_t tempLong = 0;
      tempLong = htonl(this->ipEthA);
	   memcpy(buffer + 2, &tempLong, 4);

      // Convert and copy IP EthB
      tempLong = 0;
      tempLong = htonl(this->ipEthB);
	   memcpy(buffer + 6, &tempLong, 4);
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetBSOMIPAddressResponse::getLength () const
{
   
   if (resultCode == ACS_CS_Protocol::Result_Success)
      return 10;   // Result code + 2 IP addresses
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_NE_GetBSOMIPAddressResponse::clone () const
{

   return new ACS_CS_NE_GetBSOMIPAddressResponse(*this);

}

 int ACS_CS_NE_GetBSOMIPAddressResponse::getBSOMIPAddress (uint32_t &addressEthA, uint32_t &addressEthB) const
{

   if ( (this->ipEthA == 0) || (this->ipEthB == 0) )
      return -1;

   addressEthA = this->ipEthA;
   addressEthB = this->ipEthB;

   return 0;

}

 void ACS_CS_NE_GetBSOMIPAddressResponse::setBSOMIPAddress (uint32_t addressEthA, uint32_t addressEthB)
{

   this->ipEthA = addressEthA;
   this->ipEthB = addressEthB;

}

// Additional Declarations

