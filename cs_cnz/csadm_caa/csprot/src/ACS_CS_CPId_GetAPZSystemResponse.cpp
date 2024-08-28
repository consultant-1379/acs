

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetAPZSystemResponse
#include "ACS_CS_CPId_GetAPZSystemResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetAPZSystemResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_CPId_GetAPZSystemResponse 

ACS_CS_CPId_GetAPZSystemResponse::ACS_CS_CPId_GetAPZSystemResponse()
      : logicalClock(0),
        apzSystem(0)
{
}

ACS_CS_CPId_GetAPZSystemResponse::ACS_CS_CPId_GetAPZSystemResponse(const ACS_CS_CPId_GetAPZSystemResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->logicalClock = right.logicalClock;
   this->apzSystem = right.apzSystem;

}


ACS_CS_CPId_GetAPZSystemResponse::~ACS_CS_CPId_GetAPZSystemResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetAPZSystemResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetAPZSystemResponse;

}

 int ACS_CS_CPId_GetAPZSystemResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetAPZSystemResponse_TRACE,
			"(%t) ACS_CS_CPId_GetAPZSystemResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
    else if ( (size != 2) && (size != 6) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetAPZSystemResponse_TRACE,
			"(%t) ACS_CS_CPId_GetAPZSystemResponse::setBuffer()\n"
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

      // Get and convert APZ system
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
      tempShort = ntohs(tempShort);

      // Check and cast APZ system
      if (CS_ProtocolChecker::checkAPZSystem(tempShort))
         this->apzSystem = tempShort;
	   else
	   {
		   // Some error code
		   ACS_CS_AttributeException ex;
		   ex.setDescription("Error casting APZ system");
		   throw ex;
	   }
   }

   return 0;

}

 int ACS_CS_CPId_GetAPZSystemResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetAPZSystemResponse_TRACE,
			"(%t) ACS_CS_CPId_GetAPZSystemResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength() )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetAPZSystemResponse_TRACE,
			"(%t) ACS_CS_CPId_GetAPZSystemResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);

   if (this->getLength() == 6)
   {
	   // Convert and copy logical clock
      tempShort = htons(this->logicalClock);
	   memcpy(buffer + 2, &tempShort, 2);

      // Convert and copy APZ system
      tempShort = this->apzSystem;
      tempShort = htons(tempShort);
	   memcpy(buffer + 4, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetAPZSystemResponse::getLength () const
{

   // If the result code is success the length of this primitive is 6 bytes
   if (this->resultCode == Result_Success)
		return 6;
   // If the result code is not success, the length of this primitive is 2 bytes
	else
		return 2;

}

 ACS_CS_Primitive * ACS_CS_CPId_GetAPZSystemResponse::clone () const
{

   return new ACS_CS_CPId_GetAPZSystemResponse(*this);

}

 short unsigned ACS_CS_CPId_GetAPZSystemResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_CPId_GetAPZSystemResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

 short unsigned ACS_CS_CPId_GetAPZSystemResponse::getAPZSystem () const
{

   return this->apzSystem;

}

 void ACS_CS_CPId_GetAPZSystemResponse::setAPZSystem (unsigned short system)
{

   this->apzSystem = system;

}

// Additional Declarations

