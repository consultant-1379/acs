

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPAliasName
#include "ACS_CS_CPId_GetCPAliasName.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPAliasName_TRACE);



// Class ACS_CS_CPId_GetCPAliasName 

ACS_CS_CPId_GetCPAliasName::ACS_CS_CPId_GetCPAliasName()
      : cpId(0)
{
}

ACS_CS_CPId_GetCPAliasName::ACS_CS_CPId_GetCPAliasName(const ACS_CS_CPId_GetCPAliasName &right)
  : ACS_CS_Primitive(right)
{
  
  this->cpId = right.cpId;
  
}


ACS_CS_CPId_GetCPAliasName::~ACS_CS_CPId_GetCPAliasName()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPAliasName::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetCPAliasName;

}

 int ACS_CS_CPId_GetCPAliasName::setBuffer (const char *buffer, int size)
{
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasName_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasName::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 2)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasName_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasName::setBuffer()\n"
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

 int ACS_CS_CPId_GetCPAliasName::getBuffer (char *buffer, int size) const
{
	//## begin ACS_CS_CPId_GetCPAliasName::getBuffer%473577C40313.body preserve=yes
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasName_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasName::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 2)	// Check for valid size (at least three 2-byte fields)
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasName_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasName::getBuffer()\n"
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

 short unsigned ACS_CS_CPId_GetCPAliasName::getLength () const
{
  
  return 2;
  
}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPAliasName::clone () const
{
  
  return new ACS_CS_CPId_GetCPAliasName(*this);
  
}

 void ACS_CS_CPId_GetCPAliasName::setCPId (unsigned short identifier)
{
  
  this->cpId = identifier;
  
}

 short unsigned ACS_CS_CPId_GetCPAliasName::getCPId () const
{
  
  return this->cpId;
  
}

// Additional Declarations

