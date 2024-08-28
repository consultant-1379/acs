

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_FD_GetFunctionListResponse
#include "ACS_CS_FD_GetFunctionListResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_FD_GetFunctionListResponse_TRACE);

using std::string;
using std::vector;
using namespace ACS_CS_Protocol;



// Class ACS_CS_FD_GetFunctionListResponse 

ACS_CS_FD_GetFunctionListResponse::ACS_CS_FD_GetFunctionListResponse()
      : logicalClock(0),
        totalDomainLength(0),
        totalServiceLength(0)
{
	this->serviceVector.clear();
	this->domainVector.clear();
}

ACS_CS_FD_GetFunctionListResponse::ACS_CS_FD_GetFunctionListResponse(const ACS_CS_FD_GetFunctionListResponse &right)
  : ACS_CS_BasicResponse(right)
{

	this->serviceVector.clear();
	this->domainVector.clear();
	this->logicalClock = right.logicalClock;
	this->totalDomainLength = right.totalDomainLength;
	this->totalServiceLength = right.totalServiceLength;

	vector<string> tempServiceVector = right.serviceVector;
	vector<string>::iterator it;

	for (it = tempServiceVector.begin(); it != tempServiceVector.end(); ++it)
	{
	  this->serviceVector.push_back(*it);
	}

	vector<string> tempDomainVector = right.domainVector;

	for (it = tempDomainVector.begin(); it != tempDomainVector.end(); ++it)
	{
	  this->domainVector.push_back(*it);
	}

}


ACS_CS_FD_GetFunctionListResponse::~ACS_CS_FD_GetFunctionListResponse()
{
	this->serviceVector.clear();
	this->domainVector.clear();
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_FD_GetFunctionListResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetFunctionListResponse;

}

 int ACS_CS_FD_GetFunctionListResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionListResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if ( size < 2 )// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionListResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   this->serviceVector.clear();
   this->domainVector.clear();

   // Get and convert result code
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   if (CS_ProtocolChecker::checkResultCode(tempShort))
      this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
      ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionListResponse::setBuffer()\n"
			"Error: Invalid result code\n"));

		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

   unsigned short responseCount = 0;

   // Get and convert
   if (size >= 6)
   {
      // Get and convert logical clock
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
      this->logicalClock = ntohs(tempShort);

      // Get and convert response count
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
      responseCount = ntohs(tempShort);

   }

   if (responseCount == 0)    // If no response, then we are finished parsing
   {
      if (size == 6)          // Correct size for 0 responses
         return 0;
      else                    // Incorrect size for 0 responses
      {
         ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			   "(%t) ACS_CS_FD_GetFunctionListResponse::setBuffer()\n"
			   "Error: Invalid size, size = %d\n", size));

         return -1;
      }
   }

   unsigned short offset = 6;	               // Current offset into buffer
   unsigned short currentStringLength = 0;   // Length of currect string

   // Loop through buffer
	for (int i = 0; i < responseCount; i++)
   {
      if (offset > (size - 4) )	// We must always be able to get two 16-bit values
		{								         // from the buffer so that we at least can get the
										         // domain length and service length
			ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			   "(%t) ACS_CS_FD_GetFunctionListResponse::setBuffer()\n"
			   "Error: Invalid offset, size = %d, offset = %d\n", size, offset));

         ACS_CS_ParsingException ex;
		   ex.setDescription("Error parsing data");
		   throw ex;
		}

      // Get and convert domain length
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
      currentStringLength = ntohs(tempShort);

      offset += 2;

      if ( (currentStringLength + offset + 2) > size) // Serious length error
		{
         ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			   "(%t) ACS_CS_FD_GetFunctionListResponse::setBuffer()\n"
			   "Error: Invalid string length, size = %d, offset = %d, string length = %d\n",
            size, offset, currentStringLength));

			ACS_CS_ParsingException ex;
		   ex.setDescription("Error parsing data");
		   throw ex;
		}

      char * tempDomain = new char[currentStringLength + 1];
      memcpy(tempDomain, buffer + offset, currentStringLength);
      offset += currentStringLength;
      tempDomain[currentStringLength] = 0;

      string tempString = tempDomain;
      this->domainVector.push_back(tempString);
      delete [] tempDomain;


      // Get and convert service length
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
      currentStringLength = ntohs(tempShort);

      offset += 2;

      if ( (currentStringLength + offset) > size) // Serious length error
		{
         ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			   "(%t) ACS_CS_FD_GetFunctionListResponse::setBuffer()\n"
			   "Error: Invalid string length, size = %d, offset = %d, string length = %d\n",
            size, offset, currentStringLength));

			ACS_CS_ParsingException ex;
		   ex.setDescription("Error parsing data");
		   throw ex;
		}

      char * tempService = new char[currentStringLength + 1];
      memcpy(tempService, buffer + offset, currentStringLength);
      offset += currentStringLength;
      tempService[currentStringLength] = 0;

      tempString = tempService;
      this->serviceVector.push_back(tempString);
      delete [] tempService;
   }

   return 0;

}

 int ACS_CS_FD_GetFunctionListResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)			// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionListResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if ( size != this->getLength() )	// Check for valid buffer size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionListResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}
   else if (this->domainVector.size() != this->serviceVector.size())
   {
      ACS_CS_TRACE((ACS_CS_FD_GetFunctionListResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionListResponse::getBuffer()\n"
			"Error: Invalid size, domains = %d, services = %d\n",
			this->domainVector.size(), this->serviceVector.size() ));

      return -1;
   }

   // Copy result code to buffer
	unsigned short tempShort = htons(resultCode);
	memcpy(buffer, &tempShort, 2);

   if (size >= 6)
   {
      // Copy logical clock to buffer
	   tempShort = htons(logicalClock);
	   memcpy(buffer + 2, &tempShort, 2);

      unsigned short responseCount = static_cast<unsigned short> (domainVector.size());

      // Copy response count to buffer
	   tempShort = htons(responseCount);
	   memcpy(buffer + 4, &tempShort, 2);

      vector<string>::iterator it;
      unsigned short offset = 6;

      for (unsigned short i = 0; i < responseCount; i++)
      {
         string currentDomain = this->domainVector[i];
         unsigned short currentDomainLength = static_cast<unsigned short> ( currentDomain.size() );
         tempShort = htons(currentDomainLength);

         // Copy name length
         memcpy(buffer + offset, &tempShort, 2);
         offset += 2;

         // Copy name
         memcpy(buffer + offset, currentDomain.c_str(), currentDomainLength);
         offset += currentDomainLength;

         string currentService = this->serviceVector[i];
         unsigned short currentServiceLength = static_cast<unsigned short> ( currentService.size() );
         tempShort = htons(currentServiceLength);

         // Copy name length
         memcpy(buffer + offset, &tempShort, 2);
         offset += 2;

         // Copy name
         memcpy(buffer + offset, currentService.c_str(), currentServiceLength);
         offset += currentServiceLength;
      }
   }

   return 0;

}

 short unsigned ACS_CS_FD_GetFunctionListResponse::getLength () const
{

   if (resultCode == ACS_CS_Protocol::Result_Success)
        // Result code + logical clock + response count (6 bytes)
        // + (response count * 2) + total string length
       return (6 + totalDomainLength + totalServiceLength
                 + (static_cast<unsigned short>(domainVector.size()) * 2)
                 + (static_cast<unsigned short>(serviceVector.size()) * 2) );
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_FD_GetFunctionListResponse::clone () const
{

   return new ACS_CS_FD_GetFunctionListResponse(*this);

}

 short unsigned ACS_CS_FD_GetFunctionListResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_FD_GetFunctionListResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

 void ACS_CS_FD_GetFunctionListResponse::setServiceVector (const std::vector<std::string> &nameVector)
{

   vector<string> tempVector = nameVector;
   std::vector<std::string>::iterator it;
   this->serviceVector.clear();       // Clear old vector
   this->totalServiceLength = 0;          // Reset size

   for (it = tempVector.begin(); it != tempVector.end(); ++it)
   {
      string tmp = *it;
      totalServiceLength += static_cast<unsigned short>(tmp.length());
      this->serviceVector.push_back(tmp);
   }

}

 void ACS_CS_FD_GetFunctionListResponse::getServiceVector (std::vector<std::string> &nameVector) const
{

   vector<string> tempVector = this->serviceVector;
   std::vector<std::string>::iterator it;

   for (it = tempVector.begin(); it != tempVector.end(); ++it)
   {
      string tmp = *it;
      nameVector.push_back(tmp);
   }

}

 void ACS_CS_FD_GetFunctionListResponse::setDomainVector (const std::vector<std::string> &nameVector)
{

   vector<string> tempVector = nameVector;
   std::vector<std::string>::iterator it;
   this->domainVector.clear();       // Clear old vector
   this->totalDomainLength = 0;          // Reset size

   for (it = tempVector.begin(); it != tempVector.end(); ++it)
   {
      string tmp = *it;
      totalDomainLength += static_cast<unsigned short>(tmp.length());
      this->domainVector.push_back(tmp);
   }

}

 void ACS_CS_FD_GetFunctionListResponse::getDomainVector (std::vector<std::string> &nameVector) const
{

   vector<string> tempVector = this->domainVector;
   std::vector<std::string>::iterator it;

   for (it = tempVector.begin(); it != tempVector.end(); ++it)
   {
      string tmp = *it;
      nameVector.push_back(tmp);
   }

}

// Additional Declarations

