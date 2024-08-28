

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_FD_GetFunctionProviders
#include "ACS_CS_FD_GetFunctionProviders.h"


ACS_CS_Trace_TDEF(ACS_CS_FD_GetFunctionProviders_TRACE);

using std::string;



// Class ACS_CS_FD_GetFunctionProviders 

ACS_CS_FD_GetFunctionProviders::ACS_CS_FD_GetFunctionProviders()
{
}

ACS_CS_FD_GetFunctionProviders::ACS_CS_FD_GetFunctionProviders(const ACS_CS_FD_GetFunctionProviders &right)
  : ACS_CS_Primitive(right)
{

   this->domainName = right.domainName;
   this->serviceName = right.serviceName;

}


ACS_CS_FD_GetFunctionProviders::~ACS_CS_FD_GetFunctionProviders()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_FD_GetFunctionProviders::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetFunctionProviders;

}

 int ACS_CS_FD_GetFunctionProviders::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProviders_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProviders::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if ( size < 4 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProviders_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProviders::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Get and convert name length
	unsigned short stringLength = 0;
	stringLength = *( reinterpret_cast<const unsigned short *>(buffer) );
   stringLength = ntohs(stringLength);

   unsigned short offset = 2;

   if (size >= (stringLength + 4) )
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
         ACS_CS_TRACE((ACS_CS_FD_GetFunctionProviders_TRACE,
			   "(%t) ACS_CS_FD_GetFunctionProviders::setBuffer()\n"
			   "Error: Invalid size, size = %d, string length = %d\n",
			   size, stringLength ));

         ACS_CS_ParsingException ex;
		   ex.setDescription("Error when calculating the size of the primitive");
		   throw ex;
      }
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_FD_GetFunctionProviders_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProviders::setBuffer()\n"
			"Error: Invalid size, size = %d, string length = %d\n",
			size, stringLength ));

      ACS_CS_ParsingException ex;
		ex.setDescription("Error when calculating the size of the primitive");
		throw ex;
   }

   return 0;

}

 int ACS_CS_FD_GetFunctionProviders::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProviders_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProviders::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProviders_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProviders::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short offset = 0;

   // Convert domain name length to network byte order and copy it
   unsigned short tempShort = htons(static_cast<unsigned short>(this->domainName.length()));
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

 short unsigned ACS_CS_FD_GetFunctionProviders::getLength () const
{

   return (4 + static_cast<unsigned short> (domainName.length() )
             + static_cast<unsigned short> (serviceName.length() ) );

}

 ACS_CS_Primitive * ACS_CS_FD_GetFunctionProviders::clone () const
{

   return new ACS_CS_FD_GetFunctionProviders(*this);

}

 void ACS_CS_FD_GetFunctionProviders::setDomainName (std::string name)
{
   
   this->domainName = name;

}

 std::string ACS_CS_FD_GetFunctionProviders::getDomainName () const
{

   return this->domainName;

}

 void ACS_CS_FD_GetFunctionProviders::setServiceName (std::string name)
{

   this->serviceName = name;

}

 std::string ACS_CS_FD_GetFunctionProviders::getServiceName () const
{

   return this->serviceName;

}

// Additional Declarations

