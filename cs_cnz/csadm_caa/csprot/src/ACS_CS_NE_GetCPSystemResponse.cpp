

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetCPSystemResponse
#include "ACS_CS_NE_GetCPSystemResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetCPSystemResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_NE_GetCPSystemResponse 

ACS_CS_NE_GetCPSystemResponse::ACS_CS_NE_GetCPSystemResponse()
      : cpSystem(ACS_CS_Protocol::System_NotSpecified)
{
}

ACS_CS_NE_GetCPSystemResponse::ACS_CS_NE_GetCPSystemResponse(const ACS_CS_NE_GetCPSystemResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->cpSystem = right.cpSystem;

}


ACS_CS_NE_GetCPSystemResponse::~ACS_CS_NE_GetCPSystemResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetCPSystemResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPSystemResponse;

}

 int ACS_CS_NE_GetCPSystemResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetCPSystemResponse_TRACE,
			"(%t) ACS_CS_NE_GetCPSystemResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size != 2) && (size != 4) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_NE_GetCPSystemResponse_TRACE,
			"(%t) ACS_CS_NE_GetCPSystemResponse::setBuffer()\n"
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

   if (size == 4)
   {
      // Get and convert CP system
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      tempShort = ntohs(tempShort);

      // Check and cast CP system
      if (CS_ProtocolChecker::checkCPSystem(tempShort))
         this->cpSystem = static_cast<CS_CP_System_Identifier>(tempShort);
	   else
	   {
		   // Some error code
		   ACS_CS_AttributeException ex;
		   ex.setDescription("Error casting CP system identifier");
		   throw ex;
	   }
   }

   return 0;

}

 int ACS_CS_NE_GetCPSystemResponse::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetCPSystemResponse_TRACE,
			"(%t) ACS_CS_NE_GetCPSystemResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_NE_GetCPSystemResponse_TRACE,
			"(%t) ACS_CS_NE_GetCPSystemResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);


   if (size == 4)
   {
      // Convert and copy CP system
      tempShort = htons(this->cpSystem);
	   memcpy(buffer + 2, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetCPSystemResponse::getLength () const
{
   
   if (resultCode == ACS_CS_Protocol::Result_Success)
      return 4;   // Result code + CP system
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_NE_GetCPSystemResponse::clone () const
{

   return new ACS_CS_NE_GetCPSystemResponse(*this);

}

 ACS_CS_Protocol::CS_CP_System_Identifier ACS_CS_NE_GetCPSystemResponse::getCPSystem () const
{

   return this->cpSystem;

}

 void ACS_CS_NE_GetCPSystemResponse::setCPSystem (ACS_CS_Protocol::CS_CP_System_Identifier system)
{

   this->cpSystem = system;

}

// Additional Declarations

