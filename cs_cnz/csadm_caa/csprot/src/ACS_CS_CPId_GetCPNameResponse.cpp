

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPNameResponse
#include "ACS_CS_CPId_GetCPNameResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPNameResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_CPId_GetCPNameResponse 

ACS_CS_CPId_GetCPNameResponse::ACS_CS_CPId_GetCPNameResponse()
      : logicalClock(0)
{
}

ACS_CS_CPId_GetCPNameResponse::ACS_CS_CPId_GetCPNameResponse(const ACS_CS_CPId_GetCPNameResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->logicalClock = right.logicalClock;
   this->nameStr = right.nameStr;

}


ACS_CS_CPId_GetCPNameResponse::~ACS_CS_CPId_GetCPNameResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPNameResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPNameResponse;

}

 int ACS_CS_CPId_GetCPNameResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPNameResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
    else if (size < 2)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPNameResponse::setBuffer()\n"
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

   if (size >= 6)
   {
      // Get and convert logical clock
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->logicalClock = ntohs(tempShort);

      // Get and convert logical clock
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
      unsigned short nameLength = ntohs(tempShort);

      if (nameLength > 0)
      {
         if (size != (nameLength + 6) )
         {
            ACS_CS_TRACE((ACS_CS_CPId_GetCPNameResponse_TRACE,
			      "(%t) ACS_CS_CPId_GetCPNameResponse::setBuffer()\n"
			      "Error: Illegal buffer size , size= %d\n",
			      size ));

		      ACS_CS_ParsingException ex;
		      ex.setDescription("Illegal buffer size");
		      throw ex;
         }

         char * tempStr = new char[nameLength + 1];

         if (tempStr)
         {
            memcpy(tempStr, buffer + 6, nameLength);
            tempStr[nameLength] = 0;
            this->nameStr = tempStr;
            delete [] tempStr;
         }
      }
   }

   return 0;

}

 int ACS_CS_CPId_GetCPNameResponse::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPNameResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength() )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPNameResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);
   unsigned short nameLength = static_cast<unsigned short>(this->nameStr.length());

   if (this->getLength() >= 6)
   {
	   // Convert and copy logical clock
      tempShort = htons(this->logicalClock);
	   memcpy(buffer + 2, &tempShort, 2);

      // Convert and copy name length
      tempShort = htons(nameLength);
	   memcpy(buffer + 4, &tempShort, 2);

      if (this->nameStr.length() > 0)
      {
         memcpy(buffer + 6, this->nameStr.c_str(), nameLength);
      }
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetCPNameResponse::getLength () const
{
   
   if (this->resultCode == Result_Success) // If the result code is success the length must be calculated
      return (6 + static_cast<unsigned short>(this->nameStr.length()));
	else
		return 2;  // If the result code is not success, the length of this primitive is 2 bytes

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPNameResponse::clone () const
{

   return new ACS_CS_CPId_GetCPNameResponse(*this);

}

 short unsigned ACS_CS_CPId_GetCPNameResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_CPId_GetCPNameResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

 std::string ACS_CS_CPId_GetCPNameResponse::getName () const
{

   return this->nameStr;

}

 void ACS_CS_CPId_GetCPNameResponse::setName (std::string name)
{

   this->nameStr = name;

}

// Additional Declarations

