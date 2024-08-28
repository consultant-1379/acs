/*
 * ACS_CS_MODD_Header.cpp
 *
 *  Created on: Jan 21, 2011
 *      Author: mann
 */


#include "ACS_CS_MODD_Header.h"

#include <netinet/in.h>

#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_MODD_Header_TRACE);


ACS_CS_MODD_Header::ACS_CS_MODD_Header() :
		version(ACS_CS_Protocol::Version_NotSpecified),
        totalLength(ACS_CS_Protocol::MODD_Header_Size),
        primtiveId(ACS_CS_Protocol::Primitive_NotSpecified)
{}


ACS_CS_MODD_Header::ACS_CS_MODD_Header(const ACS_CS_MODD_Header &right):
		ACS_CS_HeaderBase()
{
	this->primtiveId = right.primtiveId;
	this->totalLength = right.totalLength;
	this->version = right.version;
}


ACS_CS_MODD_Header::~ACS_CS_MODD_Header()
{}


ACS_CS_Protocol::CS_Protocol_Type ACS_CS_MODD_Header::getProtocol() const
{
	return ACS_CS_Protocol::Protocol_MODD;
}


ACS_CS_Protocol::CS_Version_Identifier ACS_CS_MODD_Header::getVersion () const
{
	return this->version;
}


unsigned short ACS_CS_MODD_Header::getHeaderLength () const
{
	return ACS_CS_Protocol::MODD_Header_Size;
}


ACS_CS_Protocol::CS_Scope_Identifier ACS_CS_MODD_Header::getScope () const
{
	return ACS_CS_Protocol::Scope_MODD;
}


ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_MODD_Header::getPrimitiveId () const
{
	return this->primtiveId;
}


void ACS_CS_MODD_Header::setVersion (ACS_CS_Protocol::CS_Version_Identifier identifier)
{
	this->version = identifier;
}


void ACS_CS_MODD_Header::setPrimitive (ACS_CS_Protocol::CS_Primitive_Identifier identifier)
{
	this->primtiveId = identifier;
}



int ACS_CS_MODD_Header::getBuffer (char *buffer, int size) const
{
	if (buffer == 0)			// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_Header_TRACE,
			"(%t) ACS_CS_MODD_Header::getBuffer()\n"
			"Error: Null pointer\n"));

		return -1;
	}
	if (size != ACS_CS_Protocol::MODD_Header_Size)	// Check buffer size
	{
		ACS_CS_TRACE((ACS_CS_MODD_Header_TRACE,
			"(%t) ACS_CS_MODD_Header::getBuffer()\n"
			"Error: Wrong header size, size = %d\n",
			size));

		return -1;
	}

	MODD_HeaderBuffer tempBuffer;	// Create temporary struct

	// Convert values to network byte order and copy them to the struct
	tempBuffer.version = this->version;
	tempBuffer.length = this->totalLength - ACS_CS_Protocol::MODD_Header_Size;
	tempBuffer.primitive = this->primtiveId - ACS_CS_Protocol::MODD_Primitive_Offset;
	tempBuffer.reserved = 0;

	memcpy(buffer, &tempBuffer, size);	// Copy the struct to the buffer

	return 0;
}


int ACS_CS_MODD_Header::setBuffer (const char *buffer, int size)
{
	if (buffer == 0)			// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_MODD_Header_TRACE,
			"(%t) ACS_CS_MODD_Header::setBuffer()\n"
			"Error: Null pointer\n"));

		return -1;
	}
	if (size != ACS_CS_Protocol::MODD_Header_Size)	// Check buffer size
	{
		ACS_CS_TRACE((ACS_CS_MODD_Header_TRACE,
			"(%t) ACS_CS_MODD_Header::setBuffer()\n"
			"Error: Wrong header size, size = %d\n",
			size));

		return -1;
	}

	MODD_HeaderBuffer tempBuffer;	// Temporary buffer to hold the header

	memcpy(&tempBuffer, buffer, size);	// Copy buffer to struct

	// Check for valid ranges and cast the values

	if (tempBuffer.version == 1)			// Check for valid version
		this->version = static_cast<ACS_CS_Protocol::CS_Version_Identifier> (tempBuffer.version);
	else
	{
	ACS_CS_TRACE((ACS_CS_MODD_Header_TRACE,
			"(%t) ACS_CS_MODD_Header::setBuffer()\n"
			"Error: Wrong HWC version, version = %d\n",
			tempBuffer.version));

		this->version = ACS_CS_Protocol::Version_NotSpecified;
		ACS_CS_VersionException ex;
		ex.setDescription("Not a valid version");
		throw ex;
	}

	// MODD protocol differs from CS in that the size in the header for MODD
	// doesn't include the header size
	this->totalLength = ACS_CS_Protocol::MODD_Header_Size + tempBuffer.length;

	// An offset is used to calculate the MODD primitives
	if (ACS_CS_Protocol::CS_ProtocolChecker::checkPrimitive(tempBuffer.primitive, ACS_CS_Protocol::Scope_MODD))	// Check for valid primitive id
		this->primtiveId = static_cast<ACS_CS_Protocol::CS_Primitive_Identifier>
					(tempBuffer.primitive + ACS_CS_Protocol::MODD_Primitive_Offset);
	else
	{
		ACS_CS_TRACE((ACS_CS_MODD_Header_TRACE,
			"(%t) ACS_CS_MODD_Header::setBuffer()\n"
			"Error: Wrong primitive, primitive = %d\n",
			tempBuffer.primitive));

		this->primtiveId = ACS_CS_Protocol::Primitive_NotSpecified;

		ACS_CS_ParsingException ex;
		ex.setDescription("Not a valid primitive");
		throw ex;
	}

	return 0;
}


unsigned short ACS_CS_MODD_Header::getTotalLength () const
{
	return this->totalLength;
}


void ACS_CS_MODD_Header::setTotalLength (int length)
{
	if (length >= ACS_CS_Protocol::MODD_Header_Size)
	   this->totalLength = length;
}


ACS_CS_HeaderBase * ACS_CS_MODD_Header::clone () const
{
	return new ACS_CS_MODD_Header(*this);
}
