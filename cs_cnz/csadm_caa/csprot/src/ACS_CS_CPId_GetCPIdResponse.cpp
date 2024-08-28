

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPIdResponse
#include "ACS_CS_CPId_GetCPIdResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPIdResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_CPId_GetCPIdResponse 

ACS_CS_CPId_GetCPIdResponse::ACS_CS_CPId_GetCPIdResponse()
      : cpId(0),
        logicalClock(0)
{
}

ACS_CS_CPId_GetCPIdResponse::ACS_CS_CPId_GetCPIdResponse(const ACS_CS_CPId_GetCPIdResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->cpId = right.cpId;
   this->logicalClock = right.logicalClock;

}


ACS_CS_CPId_GetCPIdResponse::~ACS_CS_CPId_GetCPIdResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPIdResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPIdResponse;

}

 int ACS_CS_CPId_GetCPIdResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPIdResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPIdResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
    else if ( (size != 2) && (size != 6) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPIdResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPIdResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

   // Get and convert Result code
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   // Check and cast APZ system
   if (CS_ProtocolChecker::checkResultCode(tempShort))
      this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

   if (size == 6)
   {
      // Get and convert logical clock
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->logicalClock = ntohs(tempShort);

      // Get and convert CP id
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
      this->cpId = ntohs(tempShort);
   }

   return 0;

}

 int ACS_CS_CPId_GetCPIdResponse::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPIdResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPIdResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength() )	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPIdResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPIdResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);

   if (size == 6)
   {
	   // Convert and copy logical clock
      tempShort = htons(this->logicalClock);
	   memcpy(buffer + 2, &tempShort, 2);

      // Convert and copy cp id
      tempShort = htons(this->cpId);
	   memcpy(buffer + 4, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetCPIdResponse::getLength () const
{
   
   if (this->resultCode == Result_Success) // If the result code is success the length must be calculated
		return 6;
	else
		return 2;  // If the result code is not success, the length of this primitive is 2 bytes

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPIdResponse::clone () const
{

   return new ACS_CS_CPId_GetCPIdResponse(*this);

}

 void ACS_CS_CPId_GetCPIdResponse::setCPId (unsigned short identifier)
{

   this->cpId = identifier;

}

 short unsigned ACS_CS_CPId_GetCPIdResponse::getCPId () const
{

   return this->cpId;

}

 short unsigned ACS_CS_CPId_GetCPIdResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_CPId_GetCPIdResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

// Additional Declarations

