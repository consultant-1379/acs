

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_HWC_GetBoardListResponse
#include "ACS_CS_HWC_GetBoardListResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_HWC_GetBoardListResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_HWC_GetBoardListResponse 

ACS_CS_HWC_GetBoardListResponse::ACS_CS_HWC_GetBoardListResponse()
      : logicalClock(0),
        responseCount(0),
        boardList(0),
        length(2)
{
}

ACS_CS_HWC_GetBoardListResponse::ACS_CS_HWC_GetBoardListResponse(const ACS_CS_HWC_GetBoardListResponse &right)
  : ACS_CS_BasicResponse(right)
{

	// Copy each member
	this->logicalClock = right.logicalClock;
	this->responseCount = right.responseCount;
	this->boardList = 0;
	this->length = right.length;
	
	if (right.boardList)	// If there is a list to copy, make a copy of it
	{
		this->boardList = new unsigned short[this->responseCount];

		if (this->boardList)
			memcpy(this->boardList, right.boardList, this->responseCount * 2);
	}
		

}


ACS_CS_HWC_GetBoardListResponse::~ACS_CS_HWC_GetBoardListResponse()
{

	if (boardList)
		delete [] boardList;

}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_HWC_GetBoardListResponse::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetBoardListResponse;

}

 int ACS_CS_HWC_GetBoardListResponse::getBuffer (char *buffer, int size) const
{

	if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != this->length)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	// Copy result code
	unsigned short tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);
	unsigned short bufferOffset = 2;

	// If there are more than the result code, copy it too
	if ( (this->length >= 6) )	// 6 => the size of the first three fields in the primitive
	{
		// Convert logical clock to network byte order and copy it
		tempShort = htons(this->logicalClock);
		memcpy(buffer + bufferOffset, &tempShort, 2);
		bufferOffset += 2;

		// Convert response count to network byte order and copy it
		tempShort = htons(this->responseCount);
		memcpy(buffer + bufferOffset, &tempShort, 2);
		bufferOffset += 2;

		// Copy response
		if (this->boardList)
		{
			// Loop through board list. Convert each value separately to network byte order and copy it
			for (int i = 0; i < this->responseCount; i++)
			{
				tempShort = htons( * (reinterpret_cast<unsigned short *>(this->boardList + i) ) );
				memcpy(buffer + bufferOffset + 2 * i, &tempShort, 2);
			}
		}
	}

	return 0;

}

 int ACS_CS_HWC_GetBoardListResponse::setBuffer (const char *buffer, int size)
{

	if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size < 2) || (size > USHRT_MAX) )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	if (this->boardList)							// Delete old list
	{
		delete [] this->boardList;
		this->boardList = 0;
		this->responseCount = 0;
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
		this->responseCount = tempShort;

		if ( ( size - bufferOffset) != (this->responseCount * 2) )	// Check for valid size
		{
			// Invalid size
			ACS_CS_ParsingException ex;
			ex.setDescription("Response count doesn't match length");
			throw ex;
		}
		else	// Valid size
		{
			// Create space for the board list
			this->boardList = new unsigned short[this->responseCount];

			if (this->boardList)	// Check for successful new
			{
				// Loop through the list
				for (int i = 0; i < this->responseCount; i++)
				{
					// Convert each boardid to host byte order and copy it to the list
					// The offset in bytes for each boardid is the base offset for the list
					// plus (2 * i) where 2 is the size of the boardid (unsigned short is 2 bytes)
					// and i is the current boardid.
					tempShort = ntohs( * (reinterpret_cast<const unsigned short *>(buffer + bufferOffset + 2 * i) ) );
					this->boardList[i] = tempShort;
				}
				
				this->length = size;
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
		this->length = 2;
		this->logicalClock = 0;
	}

	return 0;

}

 short unsigned ACS_CS_HWC_GetBoardListResponse::getLength () const
{

	return this->length;

}

 ACS_CS_Primitive * ACS_CS_HWC_GetBoardListResponse::clone () const
{

   return new ACS_CS_HWC_GetBoardListResponse(*this);

}

 int ACS_CS_HWC_GetBoardListResponse::getResponseCount () const
{

	return this->responseCount;

}

 int ACS_CS_HWC_GetBoardListResponse::getResponseArray (unsigned short *array, int size) const
{

	// Suppress warning for not freeing pointer
	//lint --e{429}
	// Suppress warning for not freeing pointer
	//lint --e{669}

	if (array == 0)									// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::getResponseArray()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size < 1) || (size > USHRT_MAX) )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::getResponseArray()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}
	else if (this->boardList == 0)					// Check if there is a list to copy
		return -1;

	memcpy(array, this->boardList, size * sizeof(unsigned short));	// Copy list

	return 0;

}

 void ACS_CS_HWC_GetBoardListResponse::setResultCode (ACS_CS_Protocol::CS_Result_Code result)
{

	resultCode = result;

	if (resultCode == ACS_CS_Protocol::Result_Success)
		if (this->length < 6)
			this->length = 6;

}

 short unsigned ACS_CS_HWC_GetBoardListResponse::getLogicalClock () const
{

	return this->logicalClock;

}

 void ACS_CS_HWC_GetBoardListResponse::setLogicalClock (unsigned short clockValue)
{

	this->logicalClock = clockValue;

}

 int ACS_CS_HWC_GetBoardListResponse::setResponseArray (const unsigned short *array, int size)
{

	// Suppress warning for accessing array out of range
	// lint incorrectly believes that the access can be wrong
	//lint --e{670}

	if (array == 0)									// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::setResponseArray()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size < 1) || (size > USHRT_MAX) )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetBoardListResponse_TRACE,
			"(%t) ACS_CS_HWC_GetBoardListResponse::setResponseArray()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	if (this->boardList)							// Delete old list
	{
		delete [] this->boardList;
		this->boardList = 0;
		
	}

	// Create new list
	this->responseCount = size;
	this->boardList = new unsigned short[size];

	
	if (this->boardList)				// Check for successful new
	{
		memcpy(this->boardList, array, size * sizeof(short));	// Copy list
		this->length = 6 + size * 2;
		return 0;
	}
	else
		return -1;

}

 short unsigned ACS_CS_HWC_GetBoardListResponse::getResponse (int index) const
{

	unsigned short boardId = 0;

	if (boardList)						// Check if there is a list
		if (index < responseCount)		// Check for valid index
			boardId = boardList[index];	// Get boardid

	return boardId;

}

// Additional Declarations

