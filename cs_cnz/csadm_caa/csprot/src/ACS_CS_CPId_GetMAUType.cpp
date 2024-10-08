

//	� Copyright Ericsson AB 2014. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GETMAUType
#include "ACS_CS_CPId_GetMAUType.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetMAUType_TRACE);



// Class ACS_CS_CPId_GetMAUType 

ACS_CS_CPId_GetMAUType::ACS_CS_CPId_GetMAUType()
      : cpId(0)
{
}

ACS_CS_CPId_GetMAUType::ACS_CS_CPId_GetMAUType(const ACS_CS_CPId_GetMAUType &right)
  : ACS_CS_Primitive(right)
{

   this->cpId = right.cpId;

}

ACS_CS_CPId_GetMAUType::~ACS_CS_CPId_GetMAUType()
{
}

 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetMAUType::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetMAUType;

}

 int ACS_CS_CPId_GetMAUType::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetMAUType_TRACE,
			"(%t) ACS_CS_CPId_GetMAUType::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 2)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetMAUType_TRACE,
			"(%t) ACS_CS_CPId_GetMAUType::setBuffer()\n"
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

 int ACS_CS_CPId_GetMAUType::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetMAUType_TRACE,
			"(%t) ACS_CS_CPId_GetMAUType::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 2)	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetMAUType_TRACE,
			"(%t) ACS_CS_CPId_GetMAUType::getBuffer()\n"
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

 short unsigned ACS_CS_CPId_GetMAUType::getLength () const
{
   return 2;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetMAUType::clone () const
{
   return new ACS_CS_CPId_GetMAUType(*this);
}

 short unsigned ACS_CS_CPId_GetMAUType::getCPId () const
{
   return this->cpId;
}

 void ACS_CS_CPId_GetMAUType::setCPId (unsigned short identifier)
{
   this->cpId = identifier;
}

// Additional Declarations

