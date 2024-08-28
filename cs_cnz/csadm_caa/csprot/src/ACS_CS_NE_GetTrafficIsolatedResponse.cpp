





#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetTrafficIsolatedResponse
#include "ACS_CS_NE_GetTrafficIsolatedResponse.h"


// define a trace routine for our use.
ACS_CS_Trace_TDEF(ACS_CS_NE_GetTrafficIsolatedResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_NE_GetTrafficIsolatedResponse 

ACS_CS_NE_GetTrafficIsolatedResponse::ACS_CS_NE_GetTrafficIsolatedResponse()
      : trafficIsolatedCpId(0)
{
}

ACS_CS_NE_GetTrafficIsolatedResponse::ACS_CS_NE_GetTrafficIsolatedResponse(const ACS_CS_NE_GetTrafficIsolatedResponse &right)

  : ACS_CS_BasicResponse(right)

{

   this->trafficIsolatedCpId = right.trafficIsolatedCpId;

}


ACS_CS_NE_GetTrafficIsolatedResponse::~ACS_CS_NE_GetTrafficIsolatedResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetTrafficIsolatedResponse::getPrimitiveType () const
{

   // let the caller know what type of primative we are
   return ACS_CS_Protocol::Primitive_GetTrafficIsolatedResponse;

}

 ACS_CS_Primitive * ACS_CS_NE_GetTrafficIsolatedResponse::clone () const
{

   // return a new copy of this object.
   return new ACS_CS_NE_GetTrafficIsolatedResponse(*this);

}

 int ACS_CS_NE_GetTrafficIsolatedResponse::setBuffer (const char *buffer, int size)
{

   // Check for valid memory
   if (buffer == 0)
   {
      ACS_CS_TRACE((ACS_CS_NE_GetTrafficIsolatedResponse_TRACE,
         "(%t) ACS_CS_NE_GetTrafficIsolatedResponse::setBuffer()\n"
         "Error: NULL pointer\n"));
      return -1;
   }
   else if ((size != 2) && (size != 4))  // Check for valid size
   {
      // Every PDU consists of 12 bytes of Header and X bytes of 
      // body. The body of getTafficIsolagedResponse primitive consists of 
      // the result code (2 bytes) and in case of success, an additional
      // 2 bytes for the CPID of TrafficIsolated CP.
      ACS_CS_TRACE((ACS_CS_NE_GetTrafficIsolatedResponse_TRACE,
         "(%t) ACS_CS_NE_GetTrafficIsolatedResponse::setBuffer()\n"
         "Error: Invalid size, size = %d\n", size ));
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
      // An error happened
      ACS_CS_AttributeException ex;
      ex.setDescription("Error casting result code");
      throw ex;
   }

   // Get and convert TrafficIsolated
   // Check for valid size, result code (2 bytes) + CPID (2 bytes)
   if (size == 4)
   {
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->trafficIsolatedCpId = ntohs(tempShort);
   }

   return 0;

}

 int ACS_CS_NE_GetTrafficIsolatedResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)  // Check for valid memory
   {
      ACS_CS_TRACE((ACS_CS_NE_GetTrafficIsolatedResponse_TRACE,
         "(%t) __FUNCTION__ \n"
         "Error: NULL pointer\n"));
      return -1;
   }

   if (size != this->getLength()) // Check for valid size
   {
      ACS_CS_TRACE((ACS_CS_NE_GetTrafficIsolatedResponse_TRACE,
         "(%t) __FUNCTION__ \n"
         "Error: Invalid size, size = %d\n", size));
      return -1;
   }

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);

   if (size == 4)
   {
      // Convert and copy clock master
      tempShort = htons(this->trafficIsolatedCpId);
	   memcpy(buffer + 2, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetTrafficIsolatedResponse::getLength () const
{

   if (resultCode == ACS_CS_Protocol::Result_Success)
      return 4; // result code (2) + cpid (2)   
   else
      return 2;   // Result code

}

 unsigned ACS_CS_NE_GetTrafficIsolatedResponse::getTrafficIsolated () const
{

   return this->trafficIsolatedCpId;

}

 void ACS_CS_NE_GetTrafficIsolatedResponse::setTrafficIsolated (unsigned cpId)
{
   this->trafficIsolatedCpId = cpId;
}

// Additional Declarations

