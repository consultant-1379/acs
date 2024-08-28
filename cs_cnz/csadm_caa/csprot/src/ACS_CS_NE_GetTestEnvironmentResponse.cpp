

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetTestEnvironmentResponse
#include "ACS_CS_NE_GetTestEnvironmentResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetTestEnvironmentResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_NE_GetTestEnvironmentResponse 

ACS_CS_NE_GetTestEnvironmentResponse::ACS_CS_NE_GetTestEnvironmentResponse()
      : testIdentifier(ACS_CS_Protocol::TestEnvironment_NotSpecified)
{
}

ACS_CS_NE_GetTestEnvironmentResponse::ACS_CS_NE_GetTestEnvironmentResponse(const ACS_CS_NE_GetTestEnvironmentResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->testIdentifier = right.testIdentifier;

}


ACS_CS_NE_GetTestEnvironmentResponse::~ACS_CS_NE_GetTestEnvironmentResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetTestEnvironmentResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetTestEnvironmentResponse;

}

 int ACS_CS_NE_GetTestEnvironmentResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetTestEnvironmentResponse_TRACE,
			"(%t) ACS_CS_NE_GetTestEnvironmentResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size != 2) && (size != 4) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_NE_GetTestEnvironmentResponse_TRACE,
			"(%t) ACS_CS_NE_GetTestEnvironmentResponse::setBuffer()\n"
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
      // Get and convert Test environment
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      tempShort = ntohs(tempShort);

      if (CS_ProtocolChecker::checkTestEnvironment(tempShort))
         this->testIdentifier = static_cast<CS_TestEnvironment_Identifier>(tempShort);
	   else
	   {
		   // Some error code
		   ACS_CS_AttributeException ex;
		   ex.setDescription("Error casting test environment identifier");
		   throw ex;
	   }
   }

   return 0;

}

 int ACS_CS_NE_GetTestEnvironmentResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetTestEnvironmentResponse_TRACE,
			"(%t) ACS_CS_NE_GetTestEnvironmentResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_NE_GetTestEnvironmentResponse_TRACE,
			"(%t) ACS_CS_NE_GetTestEnvironmentResponse::getBuffer()\n"
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
      // Convert and copy alarm master
      tempShort = htons(this->testIdentifier);
	   memcpy(buffer + 2, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetTestEnvironmentResponse::getLength () const
{

   if (resultCode == ACS_CS_Protocol::Result_Success)
      return 4;   // Result code + alarm master
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_NE_GetTestEnvironmentResponse::clone () const
{

   return new ACS_CS_NE_GetTestEnvironmentResponse(*this);

}

 ACS_CS_Protocol::CS_TestEnvironment_Identifier ACS_CS_NE_GetTestEnvironmentResponse::getTestEnvironment () const
{

   return this->testIdentifier;

}

 void ACS_CS_NE_GetTestEnvironmentResponse::setTestEnvironment (ACS_CS_Protocol::CS_TestEnvironment_Identifier identifier)
{

   this->testIdentifier = identifier;

}

// Additional Declarations

