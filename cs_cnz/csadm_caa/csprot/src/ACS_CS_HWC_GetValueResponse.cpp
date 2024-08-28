

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_HWC_GetValueResponse
#include "ACS_CS_HWC_GetValueResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_HWC_GetValueResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_HWC_GetValueResponse 

ACS_CS_HWC_GetValueResponse::ACS_CS_HWC_GetValueResponse()
      : response(0),
        responseLength(0),
        logicalClock(0)
{
}

ACS_CS_HWC_GetValueResponse::ACS_CS_HWC_GetValueResponse(const ACS_CS_HWC_GetValueResponse &right)
  : ACS_CS_BasicResponse(right)
{

	// Assign values
	this->logicalClock = right.logicalClock;
	this->responseLength = right.responseLength;
	this->response = 0;

	if (right.response) // Check for valid memory
	{
		if (right.responseLength > 0) // Check for data to copy
		{
			this->response = new char[right.responseLength];	// Allocate memory

			if (this->response)									// Check for successful new
				memcpy(this->response, right.response, right.responseLength);	// and copy
		}
	}

}


ACS_CS_HWC_GetValueResponse::~ACS_CS_HWC_GetValueResponse()
{

	if (this->response)
		delete [] this->response;

}



 short unsigned ACS_CS_HWC_GetValueResponse::getResponseLength () const
{

	return this->responseLength;

}

 int ACS_CS_HWC_GetValueResponse::getResponse (char *buffer, int size) const
{

	// Suppress warning for not freeing buffer,
	// which we shouldn't since it is a value-result argument.
	//lint --e{429}

	if (this->response == 0)				// Check if there is data to copy
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::getResponse()\n"
			"Error: No response to copy\n" ));

		return -1;
	}
	else if (size != this->responseLength)	// Check if data fits
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::getResponse()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}
	else if (buffer == 0)					// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::getResponse()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}

	memcpy(buffer, this->response, this->responseLength);	// Copy data

	return 0;

}

 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_HWC_GetValueResponse::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetValueResponse;

}

 int ACS_CS_HWC_GetValueResponse::getBuffer (char *buffer, int size) const
{


	if (buffer == 0)					// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != this->getLength())	// Check if data fits in buffer
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}
	
	if (this->resultCode == ACS_CS_Protocol::Result_Success) // Result success => complete primitive
	{
		if (response == 0)					// There should be a response when result is success
		{
			ACS_CS_ParsingException ex;
			ex.setDescription("Result success and no response");
			throw ex;
		}
		
		getValueResponseBuffer temp;		// Create struct for temporary values

		// Set values in struct
		temp.resultCode = htons(this->resultCode);
		temp.logicalClock = htons(this->logicalClock);
		temp.responseLength = htons(this->responseLength);

		// Copy the first part of the primitive (everything but the response)
		memcpy(buffer, &temp, 6);  // The first three fields are 6 bytes together

		if (this->responseLength == 2)	// Convert 2-byte value to network byte order
		{
			unsigned short tempShort = htons( *(reinterpret_cast<unsigned short *> (response) ) );
			memcpy(buffer + 6, &tempShort, this->responseLength);
		}
		else if (this->responseLength == 4)	// Convert 4-byte value to network byte order
		{
			unsigned int tempInt = htonl( *(reinterpret_cast<unsigned int *> (response) ) );
			memcpy(buffer + 6, &tempInt, this->responseLength);
		}
		else	// Not 2 or 4 byte value. Don't convert value, just copy it
		{
			memcpy(buffer + 6, response, this->responseLength);
		}
	}
	else	// Result != success => Primitive contains only result code. Convert and copy it
	{
		unsigned short tempShort = htons(this->resultCode);
		memcpy(buffer, &tempShort, 2);
	}

	return 0;

}

 int ACS_CS_HWC_GetValueResponse::setBuffer (const char *buffer, int size)
{

	if (buffer == 0)										// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size < 2) || ( (size > 2) && (size < 6) ) )  // Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}
	
	// Get result code from buffer
	unsigned short tempShort = ntohs( *(reinterpret_cast<const unsigned short *> (buffer) ) );

	// Check for valid value and cast to result code
	if (CS_ProtocolChecker::checkResultCode(tempShort))
		this->resultCode = static_cast<CS_Result_Code> (tempShort);
	else
	{
		this->resultCode = Result_NotSpecified;		// Not valid result code

		ACS_CS_ParsingException ex;
		ex.setDescription("Not a valid result code");
		throw ex;
	}

	if (size > 2)  // Primitive contains more than just the result code, get the rest
	{
		// Get logical clock from buffer and convert to host byte order
		this->logicalClock = ntohs( *(reinterpret_cast<const unsigned short *> (buffer + 2) ) );

		// Get response length from buffer and convert to host byte order
		tempShort = ntohs( *(reinterpret_cast<const unsigned short *> (buffer + 4) ) );

		// Check for valid response length
		if (tempShort == (size - 6) )			// Since the first 3 fields are 6 bytes
			this->responseLength = tempShort;	// Correct value
		else
		{
			this->responseLength = 0;			// Some error, discard the rest

			ACS_CS_ParsingException ex;
			ex.setDescription("Not a valid length");
			throw ex;
		}
		
		// Copy the response
		if (this->responseLength > 0)
		{
			if (this->response)	// Delete old response, if any
				delete [] this->response;

			this->response = new char[this->responseLength];	// New response length

			if (this->response)	// Check for successful new
			{
				
				if (this->responseLength == 2)	// Convert 2-byte value to network byte order
				{
					tempShort = ntohs( *(reinterpret_cast<const unsigned short *> (buffer + 6) ) );
					memcpy(this->response, &tempShort, this->responseLength);
				}
				else if (this->responseLength == 4)	// Convert 4-byte value to network byte order
				{
					unsigned int tempInt = ntohl( *(reinterpret_cast<const unsigned int *> (buffer + 6) ) );
					memcpy(this->response, &tempInt, this->responseLength);
				}
				else	// Not 2 or 4 byte value. Don't convert value, just copy it
				{
					memcpy(this->response, (buffer + 6), this->responseLength);
				}
			}
			else
				return -1;
		}

	}
	else	// Primitive doesn't contain more than the result code, set default values
	{
		this->logicalClock = 0;
		this->responseLength = 0;

		if (this->response)
		{
			delete [] this->response;
			this->response = 0;
		}
	}

	return 0;

}

 int ACS_CS_HWC_GetValueResponse::setResponse (const char *buffer, int size)
{

	// Suppress warning for accessing array out of range
	// lint incorrectly believes that the access can be wrong
	//lint --e{670}

	if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::setResponse()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size < 1) || (size > USHRT_MAX) )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValueResponse_TRACE,
			"(%t) ACS_CS_HWC_GetValueResponse::setResponse()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	if (response)									// Delete old response, if any
		delete [] response;
	
	responseLength = size;
	response = new char[size];						// Allocate memory for the new response

	if (response)									// Check for successful new
		memcpy(response, buffer, size);
	else
		return -1;

	return 0;

}

 void ACS_CS_HWC_GetValueResponse::setLogicalClock (unsigned short clockValue)
{

	this->logicalClock = clockValue;

}

 short unsigned ACS_CS_HWC_GetValueResponse::getLength () const
{

	if (this->resultCode == Result_Success) // If the result code is success the length must be calculated
		return (6 + this->responseLength);
	else
		return 2;  // If the result code is not success, the length of this primitive is 2 bytes

}

 ACS_CS_Primitive * ACS_CS_HWC_GetValueResponse::clone () const
{

   return new ACS_CS_HWC_GetValueResponse(*this);

}

 short unsigned ACS_CS_HWC_GetValueResponse::getLogicalClock () const
{

	return this->logicalClock;

}

// Additional Declarations

