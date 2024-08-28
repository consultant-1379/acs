

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetClockMasterResponse
#include "ACS_CS_NE_GetClockMasterResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetClockMasterResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_NE_GetClockMasterResponse 

ACS_CS_NE_GetClockMasterResponse::ACS_CS_NE_GetClockMasterResponse()
      : clockMaster(0)
{
}

ACS_CS_NE_GetClockMasterResponse::ACS_CS_NE_GetClockMasterResponse(const ACS_CS_NE_GetClockMasterResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->clockMaster = right.clockMaster;

}


ACS_CS_NE_GetClockMasterResponse::~ACS_CS_NE_GetClockMasterResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetClockMasterResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetClockMasterResponse;

}

 int ACS_CS_NE_GetClockMasterResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetClockMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetClockMasterResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size != 2) && (size != 4) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_NE_GetClockMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetClockMasterResponse::setBuffer()\n"
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
      // Get and convert Clock master
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->clockMaster = ntohs(tempShort);
   }

   return 0;

}

 int ACS_CS_NE_GetClockMasterResponse::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetClockMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetClockMasterResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_NE_GetClockMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetClockMasterResponse::getBuffer()\n"
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
      // Convert and copy clock master
      tempShort = htons(this->clockMaster);
	   memcpy(buffer + 2, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetClockMasterResponse::getLength () const
{
   
   if (resultCode == ACS_CS_Protocol::Result_Success)
      return 4;   // Result code + clock master
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_NE_GetClockMasterResponse::clone () const
{

   return new ACS_CS_NE_GetClockMasterResponse(*this);

}

 short unsigned ACS_CS_NE_GetClockMasterResponse::getClockMaster () const
{

   return this->clockMaster;

}

 void ACS_CS_NE_GetClockMasterResponse::setClockMaster (unsigned short identifier)
{

   this->clockMaster = identifier;

}

// Additional Declarations

