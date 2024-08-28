

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_FD_GetFunctionUsers
#include "ACS_CS_FD_GetFunctionUsers.h"


ACS_CS_Trace_TDEF(ACS_CS_FD_GetFunctionUsers_TRACE);

using std::string;



// Class ACS_CS_FD_GetFunctionUsers 

ACS_CS_FD_GetFunctionUsers::ACS_CS_FD_GetFunctionUsers()
      : apIdentifier(0)
{
}

ACS_CS_FD_GetFunctionUsers::ACS_CS_FD_GetFunctionUsers(const ACS_CS_FD_GetFunctionUsers &right)
  : ACS_CS_Primitive(right)
{

   this->apIdentifier = right.apIdentifier;
   this->domainName = right.domainName;
   this->serviceName = right.serviceName;

}


ACS_CS_FD_GetFunctionUsers::~ACS_CS_FD_GetFunctionUsers()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_FD_GetFunctionUsers::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetFunctionUsers;

}

 int ACS_CS_FD_GetFunctionUsers::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionUsers_TRACE,
			"(%t) ACS_CS_FD_GetFunctionUsers::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 6 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionUsers_TRACE,
			"(%t) ACS_CS_FD_GetFunctionUsers::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Get and convert Ap identifier
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   this->apIdentifier = ntohs(tempShort);
   unsigned short offset = 2;

   // Get and convert name length
	unsigned short stringLength = 0;
	stringLength = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
   stringLength = ntohs(stringLength);
   offset += 2;

   if (size >= (stringLength + 6) )
   {
      char * tempDomain = new char[stringLength + 1];
      memcpy(tempDomain, buffer + offset, stringLength);
      tempDomain[stringLength] = 0;
      this->domainName = tempDomain;
      delete [] tempDomain;
      offset += stringLength;

      stringLength = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
      stringLength = ntohs(stringLength);

      offset += 2;

      if (size == (stringLength + offset) )
      {
         char * tempService = new char[stringLength + 1];
         memcpy(tempService, buffer + offset, stringLength);
         tempService[stringLength] = 0;
         this->serviceName = tempService;
         delete [] tempService;
      }
      else
      {
         ACS_CS_TRACE((ACS_CS_FD_GetFunctionUsers_TRACE,
			   "(%t) ACS_CS_FD_GetFunctionUsers::setBuffer()\n"
			   "Error: Invalid size, size = %d, string length = %d\n",
			   size, stringLength ));

         ACS_CS_ParsingException ex;
		   ex.setDescription("Error when calculating the size of the primitive");
		   throw ex;
      }
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_FD_GetFunctionUsers_TRACE,
			"(%t) ACS_CS_FD_GetFunctionUsers::setBuffer()\n"
			"Error: Invalid size, size = %d, string length = %d\n",
			size, stringLength ));

      ACS_CS_ParsingException ex;
		ex.setDescription("Error when calculating the size of the primitive");
		throw ex;
   }

   return 0;

}

 int ACS_CS_FD_GetFunctionUsers::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionUsers_TRACE,
			"(%t) ACS_CS_FD_GetFunctionUsers::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionUsers_TRACE,
			"(%t) ACS_CS_FD_GetFunctionUsers::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Convert AP identifier to network byte order and copy it
   unsigned short tempShort = htons(this->apIdentifier);
	memcpy(buffer, &tempShort, 2);

   unsigned short offset = 2;

   // Convert domain name length to network byte order and copy it
   tempShort = htons(static_cast<unsigned short>(this->domainName.length()));
	memcpy(buffer + offset, &tempShort, 2);
   offset += 2;

   if (this->domainName.length() > 0)
   {
      memcpy(buffer + offset, this->domainName.c_str(), this->domainName.length() );
      offset += static_cast<unsigned short>(this->domainName.length());
   }

   // Convert service name length to network byte order and copy it
   tempShort = htons(static_cast<unsigned short>(this->serviceName.length()));
	memcpy(buffer + offset, &tempShort, 2);
   offset += 2;

   if (this->serviceName.length() > 0)
   {
      memcpy(buffer + offset, this->serviceName.c_str(), this->serviceName.length() );
   }

   return 0;

}

 short unsigned ACS_CS_FD_GetFunctionUsers::getLength () const
{
   
   return (6 + static_cast<unsigned short> (this->domainName.length())
             + static_cast<unsigned short> (this->serviceName.length()));

}

 ACS_CS_Primitive * ACS_CS_FD_GetFunctionUsers::clone () const
{

   return new ACS_CS_FD_GetFunctionUsers(*this);

}

 void ACS_CS_FD_GetFunctionUsers::setDomainName (std::string name)
{

   this->domainName = name;

}

 std::string ACS_CS_FD_GetFunctionUsers::getDomainName () const
{

   return this->domainName;

}

 void ACS_CS_FD_GetFunctionUsers::setServiceName (std::string name)
{

   this->serviceName = name;

}

 std::string ACS_CS_FD_GetFunctionUsers::getServiceName () const
{

   return this->serviceName;

}

 void ACS_CS_FD_GetFunctionUsers::setAPIdentifier (unsigned short identifier)
{

   this->apIdentifier = identifier;

}

 short unsigned ACS_CS_FD_GetFunctionUsers::getAPIdentifier () const
{

   return this->apIdentifier;

}

// Additional Declarations

