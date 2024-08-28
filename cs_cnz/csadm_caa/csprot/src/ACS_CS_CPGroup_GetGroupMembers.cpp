

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_CPGroup_GetGroupMembers
#include "ACS_CS_CPGroup_GetGroupMembers.h"


ACS_CS_Trace_TDEF(ACS_CS_CPGroup_GetGroupMembers_TRACE);

using std::string;



// Class ACS_CS_CPGroup_GetGroupMembers 

ACS_CS_CPGroup_GetGroupMembers::ACS_CS_CPGroup_GetGroupMembers()
{
}

ACS_CS_CPGroup_GetGroupMembers::ACS_CS_CPGroup_GetGroupMembers(const ACS_CS_CPGroup_GetGroupMembers &right)
  : ACS_CS_Primitive(right)
{

   this->groupName = right.groupName;

}


ACS_CS_CPGroup_GetGroupMembers::~ACS_CS_CPGroup_GetGroupMembers()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPGroup_GetGroupMembers::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetGroupMembers;

}

 int ACS_CS_CPGroup_GetGroupMembers::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupMembers_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupMembers::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 2 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupMembers_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupMembers::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Get and convert name length
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   if (size != (tempShort + 2) )
   {
      ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupMembers_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupMembers::setBuffer()\n"
			"Error: Invalid size, size = %d, string length = %d\n",
			size, tempShort ));

      ACS_CS_ParsingException ex;
		ex.setDescription("Error when calculating the size of the primitive");
		throw ex;
   }
   else
   {
      char * tempString = new char[tempShort + 1];
      memcpy(tempString, buffer + 2, tempShort);
      tempString[tempShort] = 0;
      this->groupName = tempString;
      delete [] tempString;
   }

   return 0;

}

 int ACS_CS_CPGroup_GetGroupMembers::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupMembers_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupMembers::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupMembers_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupMembers::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Convert name length to network byte order and copy it
   unsigned short tempShort = htons(static_cast<unsigned short>(this->groupName.length()));
	memcpy(buffer, &tempShort, 2);

   if (this->groupName.length() > 0)
   {
      memcpy(buffer + 2, this->groupName.c_str(), this->groupName.length() );
   }

   return 0;

}

 short unsigned ACS_CS_CPGroup_GetGroupMembers::getLength () const
{

   return (2 + static_cast<unsigned short> (groupName.length()) );

}

 ACS_CS_Primitive * ACS_CS_CPGroup_GetGroupMembers::clone () const
{

   return new ACS_CS_CPGroup_GetGroupMembers(*this);

}

 void ACS_CS_CPGroup_GetGroupMembers::setGroupName (std::string name)
{

   this->groupName = name;

}

 std::string ACS_CS_CPGroup_GetGroupMembers::getGroupName () const
{

   return this->groupName;

}

// Additional Declarations

