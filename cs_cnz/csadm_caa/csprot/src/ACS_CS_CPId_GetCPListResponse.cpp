

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPListResponse
#include "ACS_CS_CPId_GetCPListResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPListResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_CPId_GetCPListResponse 

ACS_CS_CPId_GetCPListResponse::ACS_CS_CPId_GetCPListResponse()
      : logicalClock(0),
        responseCount(0),
        cpList(0)
{
}

ACS_CS_CPId_GetCPListResponse::ACS_CS_CPId_GetCPListResponse(const ACS_CS_CPId_GetCPListResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->logicalClock = right.logicalClock;
   this->responseCount = right.responseCount;
   this->cpList = 0;
   
   if (right.responseCount > 0)
   {
      if (right.cpList)
      {
         this->cpList = new unsigned short[right.responseCount];

         if (this->cpList)
            memcpy(this->cpList, right.cpList, right.responseCount * sizeof(unsigned short));
      }
   }

}


ACS_CS_CPId_GetCPListResponse::~ACS_CS_CPId_GetCPListResponse()
{

   if (this->cpList)
      delete [] this->cpList;

}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPListResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPListResponse;

}

 int ACS_CS_CPId_GetCPListResponse::setBuffer (const char *buffer, int size)
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size < 2)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   if (this->cpList)
   {
      delete [] this->cpList;
      this->cpList = 0;
      this->responseCount = 0;
   }

   unsigned short tempShort = 0;

   // Get and convert Result code
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
   tempShort = ntohs(tempShort);

   // Check and cast APZ system
   if (CS_ProtocolChecker::checkResultCode(tempShort))
      this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

   if (this->resultCode == ACS_CS_Protocol::Result_Success)
   {
      if (size >= 6)
      {
         // Get and convert logical clock
	      tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
         this->logicalClock = ntohs(tempShort);

         // Get and convert response count clock
	      tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
         this->responseCount = ntohs(tempShort);

         if ( size != ( (this->responseCount * 2) + 6) )
         {
            ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			         "(%t) ACS_CS_CPId_GetCPListResponse::setBuffer()\n"
			         "Error: Illegal buffer size , size= %d\n",
			         size ));

		         ACS_CS_ParsingException ex;
		         ex.setDescription("Illegal buffer size");
		         throw ex;
         }

         if (this->responseCount > 0)
         {
            this->cpList = new unsigned short[this->responseCount];

            if (this->cpList)
            {
               // Loop through CP list
			      for (int i = 0; i < this->responseCount; i++)
			      {
				      // Convert each id to host byte order and copy it to the list
					   // The offset in bytes for each id is the base offset for the list
					   // plus (2 * i) where 2 is the size of the id (unsigned short is 2 bytes)
					   // and i is the current id.
				      tempShort = ntohs( * (reinterpret_cast<const unsigned short *>(buffer + 6 + 2 * i) ) );
				      this->cpList[i] = tempShort;
			      }
            }
         }
      }
      else
      {
         // Some error code
		   ACS_CS_ParsingException ex;
		   ex.setDescription("Illegal buffer size");
		   throw ex;
      }
   }

   return 0;

}

 int ACS_CS_CPId_GetCPListResponse::getBuffer (char *buffer, int size) const
{
   
   if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength() )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   unsigned short tempShort = 0;

	// Convert and copy result code
   tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);

   if (this->getLength() >= 6)
   {
      // Convert and copy result code
      tempShort = htons(this->logicalClock);
	   memcpy(buffer + 2, &tempShort, 2);

      // Convert and copy result code
      tempShort = htons(this->responseCount);
	   memcpy(buffer + 4, &tempShort, 2);

      if (this->responseCount > 0)
      {
         if (this->cpList)
         {
            // Loop through CP list. Convert each value separately to network byte order and copy it
            for (int i = 0; i < this->responseCount; i++)
			   {
				   tempShort = htons( * (reinterpret_cast<unsigned short *>(this->cpList + i) ) );
				   memcpy(buffer + 6 + 2 * i, &tempShort, 2);
			   }
         }
      }
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetCPListResponse::getLength () const
{
   
   // If the result code is success the length must be calculated
    if (this->resultCode == Result_Success)
       return (6 + 2 * this->responseCount);
   // If the result code is not success, the length of this primitive is 2 bytes
	else
		return 2; 

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPListResponse::clone () const
{

   return new ACS_CS_CPId_GetCPListResponse(*this);

}

 short unsigned ACS_CS_CPId_GetCPListResponse::getLogicalClock () const
{

   return this->logicalClock;

}

 void ACS_CS_CPId_GetCPListResponse::setLogicalClock (unsigned short clock)
{

   this->logicalClock = clock;

}

 short unsigned ACS_CS_CPId_GetCPListResponse::getResponseCount () const
{

   return this->responseCount;

}

 int ACS_CS_CPId_GetCPListResponse::getCPList (unsigned short *buffer, unsigned short size) const
{

   if (buffer == 0)									// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::getCPList()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->responseCount)		// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::getCPList()\n"
			"Error: Illegal buffer size, size = %d\n",
         size));

		return -1;
	}

   if (this->cpList)
   {
      memcpy(buffer, this->cpList, this->responseCount * sizeof(unsigned short));
   }
   
   return 0;


}

 void ACS_CS_CPId_GetCPListResponse::setCPList (const unsigned short *buffer, unsigned short size)
{


   if (buffer == 0)									// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::setCPList()\n"
			"Error: NULL pointer\n" ));

		return;
	}
   else if (size == 0)									// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPListResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPListResponse::setCPList()\n"
			"Error: Zero buffer size\n" ));

		return;
	}

   if (this->cpList)
   {
      delete [] this->cpList;
      this->cpList = 0;
      this->responseCount = 0;
   }

   this->responseCount = size;
   this->cpList = new unsigned short[size];

   if (this->cpList)
   {
      memcpy(this->cpList, buffer, size * sizeof(unsigned short));
   }

}

// Additional Declarations

