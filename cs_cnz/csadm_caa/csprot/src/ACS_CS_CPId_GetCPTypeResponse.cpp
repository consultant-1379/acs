

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPTypeResponse
#include "ACS_CS_CPId_GetCPTypeResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPTypeResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_CPId_GetCPTypeResponse 

ACS_CS_CPId_GetCPTypeResponse::ACS_CS_CPId_GetCPTypeResponse()
      : logicalClock(0),
        cpType(0)
{
}

ACS_CS_CPId_GetCPTypeResponse::ACS_CS_CPId_GetCPTypeResponse(const ACS_CS_CPId_GetCPTypeResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->logicalClock = right.logicalClock;
   this->cpType = right.cpType;

}


ACS_CS_CPId_GetCPTypeResponse::~ACS_CS_CPId_GetCPTypeResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPTypeResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPTypeResponse;

}

 int ACS_CS_CPId_GetCPTypeResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPTypeResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPTypeResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
    else if ( (size != 2) && (size != 6) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPTypeResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPTypeResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

   // Get and convert Result code
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   // Check and cast result code
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

      // Get and convert CP type
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
      tempShort = ntohs(tempShort);

      // Check and cast CP type
      if (CS_ProtocolChecker::checkCPType(tempShort))
         this->cpType = tempShort;
	   else
	   {
		   // Some error code
		   ACS_CS_AttributeException ex;
		   ex.setDescription("Error casting CP type");
		   throw ex;
	   }
   }

   return 0;

}

 int ACS_CS_CPId_GetCPTypeResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPTypeResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPTypeResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength() )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPTypeResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPTypeResponse::getBuffer()\n"
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

      // Convert and copy CP type
      tempShort = this->cpType;
      tempShort = htons(tempShort);
	   memcpy(buffer + 4, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetCPTypeResponse::getLength () const
{

   // If the result code is success the length of this primitive is 6 bytes
   if (this->resultCode == Result_Success)
		return 6;
   // If the result code is not success, the length of this primitive is 2 bytes
	else
		return 2;

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPTypeResponse::clone () const
{

   return new ACS_CS_CPId_GetCPTypeResponse(*this);

}

 short unsigned ACS_CS_CPId_GetCPTypeResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_CPId_GetCPTypeResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

 short unsigned ACS_CS_CPId_GetCPTypeResponse::getCPType () const
{

   return this->cpType;

}

 void ACS_CS_CPId_GetCPTypeResponse::setCPType (unsigned short type)
{

   this->cpType = type;

}

// Additional Declarations

