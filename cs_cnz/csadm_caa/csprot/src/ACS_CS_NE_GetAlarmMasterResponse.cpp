

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetAlarmMasterResponse
#include "ACS_CS_NE_GetAlarmMasterResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetAlarmMasterResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_NE_GetAlarmMasterResponse 

ACS_CS_NE_GetAlarmMasterResponse::ACS_CS_NE_GetAlarmMasterResponse()
      : alarmMaster(0)
{
}

ACS_CS_NE_GetAlarmMasterResponse::ACS_CS_NE_GetAlarmMasterResponse(const ACS_CS_NE_GetAlarmMasterResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->alarmMaster = right.alarmMaster;

}


ACS_CS_NE_GetAlarmMasterResponse::~ACS_CS_NE_GetAlarmMasterResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetAlarmMasterResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetAlarmMasterResponse;

}

 int ACS_CS_NE_GetAlarmMasterResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetAlarmMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetAlarmMasterResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size != 2) && (size != 4) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_NE_GetAlarmMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetAlarmMasterResponse::setBuffer()\n"
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
      // Get and convert Alarm master
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->alarmMaster = ntohs(tempShort);
   }

   return 0;

}

 int ACS_CS_NE_GetAlarmMasterResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetAlarmMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetAlarmMasterResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_NE_GetAlarmMasterResponse_TRACE,
			"(%t) ACS_CS_NE_GetAlarmMasterResponse::getBuffer()\n"
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
      tempShort = htons(this->alarmMaster);
	   memcpy(buffer + 2, &tempShort, 2);
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetAlarmMasterResponse::getLength () const
{
   
   if (resultCode == ACS_CS_Protocol::Result_Success)
      return 4;   // Result code + alarm master
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_NE_GetAlarmMasterResponse::clone () const
{

   return new ACS_CS_NE_GetAlarmMasterResponse(*this);

}

 short unsigned ACS_CS_NE_GetAlarmMasterResponse::getAlarmMaster () const
{

   return this->alarmMaster;

}

 void ACS_CS_NE_GetAlarmMasterResponse::setAlarmMaster (unsigned short identifier)
{

   this->alarmMaster = identifier;

}

// Additional Declarations

