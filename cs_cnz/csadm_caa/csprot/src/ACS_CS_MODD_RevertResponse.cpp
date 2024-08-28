/*
 * ACS_CS_MODD_RevertResponse.cpp
 *
 *  Created on: Jan 20, 2011
 *      Author: mann
 */


#include "ACS_CS_MODD_RevertResponse.h"


#include <string.h>

#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_MODD_RevertResponse_TRACE);


ACS_CS_MODD_RevertResponse::ACS_CS_MODD_RevertResponse():
	ACS_CS_Primitive(),
	resultCode(ACS_CS_Protocol::MODD_Result_NotSpecified)
{}


ACS_CS_MODD_RevertResponse::ACS_CS_MODD_RevertResponse(const ACS_CS_MODD_RevertResponse &right):
		ACS_CS_Primitive(),
		resultCode(right.resultCode),
		description(right.description)
{}


ACS_CS_MODD_RevertResponse::~ACS_CS_MODD_RevertResponse()
{}


ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_MODD_RevertResponse::getPrimitiveType () const
{
	return ACS_CS_Protocol::Primitive_MODD_RevertResponse;
}


int ACS_CS_MODD_RevertResponse::setBuffer (const char *buffer, int size)
{
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_RevertResponse_TRACE,
			"(%t) ACS_CS_MODD_RevertResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 5 )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_MODD_RevertResponse_TRACE,
			"(%t) ACS_CS_MODD_RevertResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	// Get resultcode
	uint16_t tempShort = *( reinterpret_cast<const uint16_t *>(buffer) );

	if (ACS_CS_Protocol::CS_ProtocolChecker::checkMODDResultCode(tempShort))
	{
		this->resultCode = static_cast<ACS_CS_Protocol::MODD_Result_Code>(tempShort);
	}
	else
	{
		this->resultCode = ACS_CS_Protocol::MODD_Result_NotSpecified;
	}

	// Get description length
	uint16_t descriptionLength = *( reinterpret_cast<const uint16_t *>(buffer + 2) );

	// Get description
	if (descriptionLength == (size - 4) )
	{
		char * tmpStr = new char[descriptionLength];
		memcpy(tmpStr, buffer + 4, descriptionLength);
		tmpStr[descriptionLength - 1] = 0;
		this->description = tmpStr;
		delete [] tmpStr;
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_MODD_RevertResponse_TRACE,
			"(%t) ACS_CS_MODD_RevertResponse::setBuffer()\n"
			"Error: Invalid description length, length = %d\n",
			descriptionLength ));

		return -1;
	}

	return 0;
}


int ACS_CS_MODD_RevertResponse::getBuffer (char *buffer, int size) const
{
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_RevertResponse_TRACE,
			"(%t) ACS_CS_MODD_RevertResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_MODD_RevertResponse_TRACE,
			"(%t) ACS_CS_MODD_RevertResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}


	// Copy resultCode
	uint16_t tempShort = this->resultCode;
	memcpy(buffer, reinterpret_cast<char *>(&tempShort), sizeof(tempShort));

	// Copy description length
	tempShort = this->description.length() + 1;
	memcpy(buffer + 2, reinterpret_cast<char *>(&tempShort), sizeof(tempShort));

	// Copy description
	memcpy(buffer + 4, this->description.c_str(), this->description.length());
	buffer[4 + this->description.length()] = 0;

	return 0;
}


unsigned short ACS_CS_MODD_RevertResponse::getLength () const
{
	return this->description.length() + 5;
}


ACS_CS_Primitive * ACS_CS_MODD_RevertResponse::clone () const
{
	return new ACS_CS_MODD_RevertResponse(*this);
}


