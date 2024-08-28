

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPId
#include "ACS_CS_CPId_GetCPId.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPId_TRACE);

using namespace ACS_CS_Protocol;
using std::string;



// Class ACS_CS_CPId_GetCPId 

ACS_CS_CPId_GetCPId::ACS_CS_CPId_GetCPId()
{
}

ACS_CS_CPId_GetCPId::ACS_CS_CPId_GetCPId(const ACS_CS_CPId_GetCPId &right)
  : ACS_CS_Primitive(right)
{

   this->nameStr = right.nameStr;
   
}


ACS_CS_CPId_GetCPId::~ACS_CS_CPId_GetCPId()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPId::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPId;

}

 int ACS_CS_CPId_GetCPId::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPId_TRACE,
			"(%t) ACS_CS_CPId_GetCPId::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size < 2)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPId_TRACE,
			"(%t) ACS_CS_CPId_GetCPId::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short nameLength = 0;

   // Get and convert name length
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   nameLength = ntohs(tempShort);

   if (size != (nameLength + 2) )
   {
      ACS_CS_TRACE((ACS_CS_CPId_GetCPId_TRACE,
			"(%t) ACS_CS_CPId_GetCPId::setBuffer()\n"
			"Error: Illegal buffer size , size= %d\n",
			size ));

		ACS_CS_ParsingException ex;
		ex.setDescription("Illegal buffer size");
		throw ex;
   }

   if (nameLength > 0)
   {
      char * tempStr = new char[nameLength + 1];

      if (tempStr)
      {
         memcpy(tempStr, buffer + 2, nameLength);
         tempStr[nameLength] = 0;
         this->nameStr = tempStr;
         delete [] tempStr;
      }
   }

   return 0;

}

 int ACS_CS_CPId_GetCPId::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPId_TRACE,
			"(%t) ACS_CS_CPId_GetCPId::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength() )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPId_TRACE,
			"(%t) ACS_CS_CPId_GetCPId::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short nameLength = static_cast<unsigned short>(this->nameStr.length());
   unsigned short tempShort = 0;

	// Convert and copy name length
   tempShort = htons(nameLength);
	memcpy(buffer, &tempShort, 2);

   if (nameLength > 0)
   {
      memcpy(buffer + 2, this->nameStr.c_str(), nameLength);
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetCPId::getLength () const
{
   
   return (static_cast<unsigned short>(this->nameStr.length()) + 2);

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPId::clone () const
{

   return new ACS_CS_CPId_GetCPId(*this);

}

 void ACS_CS_CPId_GetCPId::setName (std::string name)
{

   this->nameStr = name;

}

 std::string ACS_CS_CPId_GetCPId::getName () const
{

   return this->nameStr;

}

// Additional Declarations

