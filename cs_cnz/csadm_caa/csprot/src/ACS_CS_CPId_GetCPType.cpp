

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPType
#include "ACS_CS_CPId_GetCPType.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPType_TRACE);



// Class ACS_CS_CPId_GetCPType 

ACS_CS_CPId_GetCPType::ACS_CS_CPId_GetCPType()
      : cpId(0)
{
}

ACS_CS_CPId_GetCPType::ACS_CS_CPId_GetCPType(const ACS_CS_CPId_GetCPType &right)
  : ACS_CS_Primitive(right)
{

   this->cpId = right.cpId;

}


ACS_CS_CPId_GetCPType::~ACS_CS_CPId_GetCPType()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPType::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPType;

}

 int ACS_CS_CPId_GetCPType::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPType_TRACE,
			"(%t) ACS_CS_CPId_GetCPType::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 2)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPType_TRACE,
			"(%t) ACS_CS_CPId_GetCPType::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}


	// Get and convert CP id
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   this->cpId = ntohs(tempShort);

   return 0;

}

 int ACS_CS_CPId_GetCPType::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPType_TRACE,
			"(%t) ACS_CS_CPId_GetCPType::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 2)	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPType_TRACE,
			"(%t) ACS_CS_CPId_GetCPType::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy CP id
   tempShort = htons(this->cpId);
	memcpy(buffer, &tempShort, 2);

   return 0;

}

 short unsigned ACS_CS_CPId_GetCPType::getLength () const
{

   return 2;

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPType::clone () const
{

   return new ACS_CS_CPId_GetCPType(*this);

}

 short unsigned ACS_CS_CPId_GetCPType::getCPId () const
{

   return this->cpId;

}

 void ACS_CS_CPId_GetCPType::setCPId (unsigned short identifier)
{

   this->cpId = identifier;

}

// Additional Declarations

