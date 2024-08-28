

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_CPGroup_GetGroupNamesResponse
#include "ACS_CS_CPGroup_GetGroupNamesResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_CPGroup_GetGroupNamesResponse_TRACE);

using std::string;
using std::vector;
using namespace ACS_CS_Protocol;



// Class ACS_CS_CPGroup_GetGroupNamesResponse 

ACS_CS_CPGroup_GetGroupNamesResponse::ACS_CS_CPGroup_GetGroupNamesResponse()
      : logicalClock(0),
        totalStringSize(0)
{
	this->responseVector.clear();
}

ACS_CS_CPGroup_GetGroupNamesResponse::ACS_CS_CPGroup_GetGroupNamesResponse(const ACS_CS_CPGroup_GetGroupNamesResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->logicalClock = right.logicalClock;
   this->totalStringSize = right.totalStringSize;
   this->responseVector.clear();

   vector<string> tempVector = right.responseVector;
   vector<string>::iterator it;

   for (it = tempVector.begin(); it != tempVector.end(); ++it)
   {
      this->responseVector.push_back(*it);
   }

}


ACS_CS_CPGroup_GetGroupNamesResponse::~ACS_CS_CPGroup_GetGroupNamesResponse()
{
	this->responseVector.clear();
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPGroup_GetGroupNamesResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetGroupNamesResponse;

}

 int ACS_CS_CPGroup_GetGroupNamesResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupNamesResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if ( size < 2 )// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupNamesResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   this->responseVector.clear();

   // Get and convert result code
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   if (CS_ProtocolChecker::checkResultCode(tempShort))
      this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
      ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupNamesResponse::setBuffer()\n"
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
         ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			   "(%t) ACS_CS_CPGroup_GetGroupNamesResponse::setBuffer()\n"
			   "Error: Invalid size, size = %d\n", size));

         return -1;
      }
   }

   unsigned short offset = 6;	               // Current offset into buffer
   unsigned short currentStringLength = 0;   // Length of currect string

   // Loop through buffer
	for (int i = 0; i < responseCount; i++)
   {
      if (offset > (size - 2) )	// We must always be able to get one 16-bit value
		{								         // from the buffer so that we at least can get the
										         // group name length
			ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			   "(%t) ACS_CS_CPGroup_GetGroupNamesResponse::setBuffer()\n"
			   "Error: Invalid offset, size = %d, offset = %d\n", size, offset));

         ACS_CS_ParsingException ex;
		   ex.setDescription("Error parsing data");
		   throw ex;
		}

      // Get and convert response count
	   tempShort = 0;
	   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
      currentStringLength = ntohs(tempShort);

      offset += 2;

      if ( (currentStringLength + offset) > size) // Serious length error
		{
         ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			   "(%t) ACS_CS_CPGroup_GetGroupNamesResponse::setBuffer()\n"
			   "Error: Invalid string length, size = %d, offset = %d, string length = %d\n",
            size, offset, currentStringLength));

			ACS_CS_ParsingException ex;
		   ex.setDescription("Error parsing data");
		   throw ex;
		}

      char * tempBuffer = new char[currentStringLength + 1];
      memcpy(tempBuffer, buffer + offset, currentStringLength);
      offset += currentStringLength;
      tempBuffer[currentStringLength] = 0;

      string tempString = tempBuffer;
      this->responseVector.push_back(tempString);
      delete [] tempBuffer;
   }

   return 0;

}

 int ACS_CS_CPGroup_GetGroupNamesResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)			// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupNamesResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if ( size != this->getLength() )	// Check for valid buffer size
	{
		ACS_CS_TRACE((ACS_CS_CPGroup_GetGroupNamesResponse_TRACE,
			"(%t) ACS_CS_CPGroup_GetGroupNamesResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

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

      // Copy response count to buffer
	   tempShort = htons( static_cast<unsigned short>( responseVector.size() ) );
	   memcpy(buffer + 4, &tempShort, 2);

      vector<string> tempVector = responseVector;
      vector<string>::iterator it;
      unsigned short offset = 6;

      for (it = tempVector.begin(); it != tempVector.end(); ++it)
      {
         string currentString = *it;
         unsigned short currentStringLength = static_cast<unsigned short> ( currentString.size() );
         tempShort = htons(currentStringLength);

         // Copy name length
         memcpy(buffer + offset, &tempShort, 2);
         offset += 2;

         // Copy name
         memcpy(buffer + offset, currentString.c_str(), currentStringLength);
         offset += currentStringLength;
      }
   }

   return 0;

}

 short unsigned ACS_CS_CPGroup_GetGroupNamesResponse::getLength () const
{

    if (resultCode == ACS_CS_Protocol::Result_Success)
        // Result code + logical clock + response count (6 bytes)
        // + (response count * 2) + total string length
       return (6 + totalStringSize + (static_cast<unsigned short>(responseVector.size()) * 2) );
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_CPGroup_GetGroupNamesResponse::clone () const
{

   return new ACS_CS_CPGroup_GetGroupNamesResponse(*this);

}

 short unsigned ACS_CS_CPGroup_GetGroupNamesResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_CPGroup_GetGroupNamesResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

 void ACS_CS_CPGroup_GetGroupNamesResponse::setNameVector (const std::vector<std::string> &nameVector)
{

   vector<string> tempVector = nameVector;
   std::vector<std::string>::iterator it;
   this->responseVector.clear();       // Clear old vector
   this->totalStringSize = 0;          // Reset size

   for (it = tempVector.begin(); it != tempVector.end(); ++it)
   {
      string tmp = *it;
      totalStringSize += static_cast<unsigned short>(tmp.length());
      this->responseVector.push_back(tmp);
   }

}

 void ACS_CS_CPGroup_GetGroupNamesResponse::getNameVector (std::vector<std::string> &nameVector) const
{

   vector<string> tempVector = this->responseVector;
   std::vector<std::string>::iterator it;

   for (it = tempVector.begin(); it != tempVector.end(); ++it)
   {
      string tmp = *it;
      nameVector.push_back(tmp);
   }

}

// Additional Declarations

