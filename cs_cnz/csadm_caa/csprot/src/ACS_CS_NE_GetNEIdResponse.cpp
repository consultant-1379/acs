

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetNEIdResponse
#include "ACS_CS_NE_GetNEIdResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetNEIdResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_NE_GetNEIdResponse 

ACS_CS_NE_GetNEIdResponse::ACS_CS_NE_GetNEIdResponse()
{
}

ACS_CS_NE_GetNEIdResponse::ACS_CS_NE_GetNEIdResponse(const ACS_CS_NE_GetNEIdResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->neStr = right.neStr;

}


ACS_CS_NE_GetNEIdResponse::~ACS_CS_NE_GetNEIdResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetNEIdResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetNEIdResponse;

}

 int ACS_CS_NE_GetNEIdResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetNEIdResponse_TRACE,
			"(%t) ACS_CS_NE_GetNEIdResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if ( (size < 2) || (size == 3) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_NE_GetNEIdResponse_TRACE,
			"(%t) ACS_CS_NE_GetNEIdResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Get and convert Result code
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

	// Get and convert Network identifier length
	tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
   unsigned short neLength = ntohs(tempShort);

   if (size > 2)
   {
      // Check for legal size
      if (size != (neLength + 4) )
      {
         ACS_CS_TRACE((ACS_CS_NE_GetNEIdResponse_TRACE,
			   "(%t) ACS_CS_NE_GetNEIdResponse::setBuffer()\n"
			   "Error: Illegal buffer size , size= %d\n",
			   size ));

		   ACS_CS_ParsingException ex;
		   ex.setDescription("Illegal buffer size");
		   throw ex;
      }

      if ( neLength > 0 )
      {
         char * tempStr = new char[neLength + 1];

         if (tempStr)
         {
            memcpy(tempStr, buffer + 4, neLength);
            tempStr[neLength] = 0;
            this->neStr = tempStr;
            delete [] tempStr;
         }
      }
   }
  
   return 0;

}

 int ACS_CS_NE_GetNEIdResponse::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_NE_GetNEIdResponse_TRACE,
			"(%t) ACS_CS_NE_GetNEIdResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength() )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_NE_GetNEIdResponse_TRACE,
			"(%t) ACS_CS_NE_GetNEIdResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;
   unsigned short neLength = static_cast<unsigned short>(this->neStr.length());

   // Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);

   if (size > 2)
   {

      // Convert and copy NE id length
      tempShort = htons(neLength);
	   memcpy(buffer + 2, &tempShort, 2);

      if (neLength > 0)
      {
         // Copy NE Id
         memcpy(buffer + 4, this->neStr.c_str(), neLength);
      }
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetNEIdResponse::getLength () const
{
   
   if (resultCode == ACS_CS_Protocol::Result_Success)
      return ( 4 + static_cast<unsigned short>(this->neStr.length()) );   // Result code + 2 bytes + length of NE id
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_NE_GetNEIdResponse::clone () const
{

   return new ACS_CS_NE_GetNEIdResponse(*this);

}

 void ACS_CS_NE_GetNEIdResponse::setNEId (std::string neId)
{

   this->neStr = neId;

}

 std::string ACS_CS_NE_GetNEIdResponse::getNEId () const
{

   return this->neStr;

}

// Additional Declarations

