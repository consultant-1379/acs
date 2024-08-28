

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_VLAN_GetVLANAddress
#include "ACS_CS_VLAN_GetVLANAddress.h"


ACS_CS_Trace_TDEF(ACS_CS_VLAN_GetVLANAddress_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_VLAN_GetVLANAddress 

ACS_CS_VLAN_GetVLANAddress::ACS_CS_VLAN_GetVLANAddress()
      : boardId(0)
{
}

ACS_CS_VLAN_GetVLANAddress::ACS_CS_VLAN_GetVLANAddress(const ACS_CS_VLAN_GetVLANAddress &right)
  : ACS_CS_Primitive(right)
{

   this->boardId = right.boardId;
   this->vlanName = right.vlanName;

}


ACS_CS_VLAN_GetVLANAddress::~ACS_CS_VLAN_GetVLANAddress()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_VLAN_GetVLANAddress::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetVLANAddress;

}

 int ACS_CS_VLAN_GetVLANAddress::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddress_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddress::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 4 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddress_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddress::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Get and convert Ap identifier
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   this->boardId = ntohs(tempShort);

   // Get and convert name length
	tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
   tempShort = ntohs(tempShort);

   if (size != (tempShort + 4) )
   {
      ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddress_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddress::setBuffer()\n"
			"Error: Invalid size, size = %d, string length = %d\n",
			size, tempShort ));

      ACS_CS_ParsingException ex;
		ex.setDescription("Error when calculating the size of the primitive");
		throw ex;
   }
   else
   {
      char * tempString = new char[tempShort + 1];
      memcpy(tempString, buffer + 4, tempShort);
      tempString[tempShort] = 0;
      this->vlanName = tempString;
      delete [] tempString;
   }

   return 0;

}

 int ACS_CS_VLAN_GetVLANAddress::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddress_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddress::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANAddress_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANAddress::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Convert AP identifier to network byte order and copy it
   unsigned short tempShort = htons(this->boardId);
	memcpy(buffer, &tempShort, 2);

   // Convert name length to network byte order and copy it
   tempShort = htons(static_cast<unsigned short>(this->vlanName.length()));
	memcpy(buffer + 2, &tempShort, 2);

   if (this->vlanName.length() > 0)
   {
      memcpy(buffer + 4, this->vlanName.c_str(), this->vlanName.length() );
   }

   return 0;

}

 short unsigned ACS_CS_VLAN_GetVLANAddress::getLength () const
{

   return (static_cast<unsigned short>(this->vlanName.length()) + 4);

}

 ACS_CS_Primitive * ACS_CS_VLAN_GetVLANAddress::clone () const
{

   return new ACS_CS_VLAN_GetVLANAddress(*this);

}

 void ACS_CS_VLAN_GetVLANAddress::setBoardId (unsigned short identifier)
{

   this->boardId = identifier;

}

 void ACS_CS_VLAN_GetVLANAddress::setVLANName (std::string name)
{

   this->vlanName = name;

}

 short unsigned ACS_CS_VLAN_GetVLANAddress::getBoardId () const
{

   return this->boardId;

}

 std::string ACS_CS_VLAN_GetVLANAddress::getVLANName () const
{

   return this->vlanName;

}

// Additional Declarations

