

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_FD_GetFunctionProvidersResponse
#include "ACS_CS_FD_GetFunctionProvidersResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_FD_GetFunctionProvidersResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_FD_GetFunctionProvidersResponse 

ACS_CS_FD_GetFunctionProvidersResponse::ACS_CS_FD_GetFunctionProvidersResponse()
      : logicalClock(0),
        apArray(0),
        apCount(0)
{
}

ACS_CS_FD_GetFunctionProvidersResponse::ACS_CS_FD_GetFunctionProvidersResponse(const ACS_CS_FD_GetFunctionProvidersResponse &right)
  : ACS_CS_BasicResponse(right)
{

   this->logicalClock = right.logicalClock;
   this->apCount = right.apCount;
   this->apArray = 0;

   if (right.apCount > 0)
   {
      if (right.apArray)
      {
         this->apArray = new unsigned short[right.apCount];

         if (this->apArray)
            memcpy(this->apArray, right.apArray, right.apCount * sizeof(unsigned short));
      }
   }

}


ACS_CS_FD_GetFunctionProvidersResponse::~ACS_CS_FD_GetFunctionProvidersResponse()
{

   if (this->apArray)
      delete [] this->apArray;

}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_FD_GetFunctionProvidersResponse::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetFunctionProvidersResponse;

}

 int ACS_CS_FD_GetFunctionProvidersResponse::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 2 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   if (this->apArray)							// Delete old list
	{
		delete [] this->apArray;
		this->apArray = 0;
      this->apCount = 0;
	}

	// Get result code from buffer and convert it to host byte order
	unsigned short tempShort = ntohs( * (reinterpret_cast<const unsigned short *>(buffer) ) );
	unsigned short bufferOffset = 2;

	// Check the result code to see if it is valid
	if (CS_ProtocolChecker::checkResultCode(tempShort))
	{
		this->resultCode = static_cast<CS_Result_Code>(tempShort);	// Cast and assign result code
	}
	else	// Invalid result code
	{
		// Do something
		ACS_CS_ParsingException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

	if (this->resultCode == Result_Success)	// When success we should have a complete primitve
	{										// of at least 6 bytes (the first three fields)

		if (size < 6)						// Incorrect size for a success
		{
			ACS_CS_ParsingException ex;
			ex.setDescription("Error when calculating the size of the primitive");
			throw ex;
		}

		// Get logical clock from buffer and convert it to host byte order
		tempShort = ntohs( * (reinterpret_cast<const unsigned short *>(buffer + bufferOffset) ) );
		bufferOffset += 2;
		this->logicalClock = tempShort;

		// Get responseCount from buffer and convert it to host byte order
		tempShort = ntohs( * (reinterpret_cast<const unsigned short *>(buffer + bufferOffset) ) );
		bufferOffset += 2;
      this->apCount = tempShort;

		if ( ( size - bufferOffset) != (this->apCount * 2) )	// Check for valid size
		{
			// Invalid size
			ACS_CS_ParsingException ex;
			ex.setDescription("Response count doesn't match length");
			throw ex;
		}
		else	// Valid size
		{
			// Create space for the list
         this->apArray = new unsigned short[this->apCount];

			if (this->apArray)	// Check for successful new
			{
				// Loop through the list
				for (int i = 0; i < this->apCount; i++)
				{
					// Convert each id to host byte order and copy it to the list
					// The offset in bytes for each id is the base offset for the list
					// plus (2 * i) where 2 is the size of the id (unsigned short is 2 bytes)
					// and i is the current id.
					tempShort = ntohs( * (reinterpret_cast<const unsigned short *>(buffer + bufferOffset + 2 * i) ) );
					this->apArray[i] = tempShort;
				}
			}
			else	// Unsuccessful new
			{
				return -1;
			}
		}
	}
	else	// Result is not success. The primitive should only contain the result code
	{
		if (size != 2)		// Illegal size, the result code is only 2 bytes
		{
			// Do something
			ACS_CS_ParsingException ex;
			ex.setDescription("Illegal size");
			throw ex;
		}

		// Assign default values
		this->logicalClock = 0;
	}

	return 0;

}

 int ACS_CS_FD_GetFunctionProvidersResponse::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	// Copy result code
	unsigned short tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);
	unsigned short bufferOffset = 2;

	// If there are more than the result code, copy it too
   if ( (this->getLength() >= 6) )	// 6 => the size of the first three fields in the primitive
	{
		// Convert logical clock to network byte order and copy it
		tempShort = htons(this->logicalClock);
		memcpy(buffer + bufferOffset, &tempShort, 2);
		bufferOffset += 2;

		// Convert response count to network byte order and copy it
      tempShort = htons(this->apCount);
		memcpy(buffer + bufferOffset, &tempShort, 2);
		bufferOffset += 2;

		// Copy response
      if (this->apArray)
		{
			// Loop through list. Convert each value separately to network byte order and copy it
         for (int i = 0; i < this->apCount; i++)
			{
				tempShort = htons( * (reinterpret_cast<unsigned short *>(this->apArray + i) ) );
				memcpy(buffer + bufferOffset + 2 * i, &tempShort, 2);
			}
		}
	}

	return 0;

}

 short unsigned ACS_CS_FD_GetFunctionProvidersResponse::getLength () const
{

   if (resultCode == ACS_CS_Protocol::Result_Success)
        // Result code + logical clock + response count (6 bytes)
        // + (response count * 2)
        return (6 + (apCount* 2) );
   else
      return 2;   // Only result code

}

 ACS_CS_Primitive * ACS_CS_FD_GetFunctionProvidersResponse::clone () const
{

   return new ACS_CS_FD_GetFunctionProvidersResponse(*this);

}

 short unsigned ACS_CS_FD_GetFunctionProvidersResponse::getLogicalClock () const
{
   
   return this->logicalClock;

}

 void ACS_CS_FD_GetFunctionProvidersResponse::setLogicalClock (unsigned short clock)
{
   
   this->logicalClock = clock;

}

 int ACS_CS_FD_GetFunctionProvidersResponse::getResponseArray (unsigned short *array, unsigned short size) const
{

   if (array == 0)									// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::getResponseArray()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if ( size != this->apCount )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::getResponseArray()\n"
			"Error: Invalid size, size = %d\n", size ));

		return -1;
	}
   else if (this->apArray == 0)					// Check if there is a list to copy
		return -1;

	memcpy(array, this->apArray, size * sizeof(unsigned short));	// Copy list

	return 0;

}

 short unsigned ACS_CS_FD_GetFunctionProvidersResponse::getResponseCount () const
{

   return this->apCount;

}

 int ACS_CS_FD_GetFunctionProvidersResponse::setResponseArray (const unsigned short *array, unsigned short size)
{

   if (array == 0)									// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::setResponseArray()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size == 0 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_FD_GetFunctionProvidersResponse_TRACE,
			"(%t) ACS_CS_FD_GetFunctionProvidersResponse::setResponseArray()\n"
			"Error: Invalid size, size = 0\n" ));

		return -1;
	}

   if (this->apArray)							// Delete old list
	{
		delete [] this->apArray;
		this->apArray = 0;
      this->apCount = 0;
		
	}

	// Create new list
	this->apCount = size;
	this->apArray = new unsigned short[size];

	
	if (this->apArray)				// Check for successful new
	{
		memcpy(this->apArray, array, size * sizeof(short));	// Copy list
		return 0;
	}
	else
		return -1;

}

// Additional Declarations

