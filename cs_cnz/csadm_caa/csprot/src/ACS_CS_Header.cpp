

//	-
//	Copyright Ericsson AB 2007. All rights reserved.
//	-




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Trace.h"


// ACS_CS_Header
#include "ACS_CS_Header.h"
#include <netinet/in.h>
#include <cstring>
#include <sstream>

ACS_CS_Trace_TDEF(ACS_CS_Header_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_Header 

ACS_CS_Header::ACS_CS_Header()
      : version(ACS_CS_Protocol::Version_NotSpecified),
        totalLength(ACS_CS_Protocol::CS_Header_Size),
        scope(ACS_CS_Protocol::Scope_NotSpecified),
        primtiveId(ACS_CS_Protocol::Primitive_NotSpecified),
        requestId(0)
{
}

ACS_CS_Header::ACS_CS_Header(const ACS_CS_Header &right):
		ACS_CS_HeaderBase()
{

   this->primtiveId = right.primtiveId;
   this->requestId = right.requestId;
   this->scope = right.scope;
   this->totalLength = right.totalLength;
   this->version = right.version;
   
}


ACS_CS_Header::~ACS_CS_Header()
{
}


ACS_CS_Protocol::CS_Protocol_Type ACS_CS_Header::getProtocol() const
{
	return ACS_CS_Protocol::Protocol_CS;
}


 ACS_CS_Protocol::CS_Version_Identifier ACS_CS_Header::getVersion () const
{

	return this->version;

}

 short unsigned ACS_CS_Header::getHeaderLength () const
{

	return ACS_CS_Protocol::CS_Header_Size;

}

 ACS_CS_Protocol::CS_Scope_Identifier ACS_CS_Header::getScope () const
{

	return this->scope;

}

 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_Header::getPrimitiveId () const
{

	return this->primtiveId;

}

 short unsigned ACS_CS_Header::getRequestId () const
{

	return this->requestId;

}

 void ACS_CS_Header::setVersion (ACS_CS_Protocol::CS_Version_Identifier identifier)
{

	this->version = identifier;

}

 void ACS_CS_Header::setScope (ACS_CS_Protocol::CS_Scope_Identifier identifier)
{

	this->scope = identifier;

}

 void ACS_CS_Header::setPrimitive (ACS_CS_Protocol::CS_Primitive_Identifier identifier)
{

	this->primtiveId = identifier;

}

 void ACS_CS_Header::setRequestId (unsigned short identifier)
{

	this->requestId = identifier;

}

 int ACS_CS_Header::getBuffer (char *buffer, int size) const
{

	// Suppress warning for not freeing buffer,
	// which we shouldn't since it is a value-result argument.
	//lint --e{429}

	if (buffer == 0)			// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::getBuffer()\n"
			"Error: Null pointer\n"));

		return -1;
	}
	if (size != CS_Header_Size)	// Check buffer size
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::getBuffer()\n"
			"Error: Wrong header size, size = %d\n",
			size));

		return -1;
	}

	CS_HeaderBuffer tempBuffer;	// Create temporary struct

	// Convert values to network byte order and copy them to the struct
	tempBuffer.version = htons(this->version);
	tempBuffer.length = htons(this->totalLength);
	tempBuffer.scope = htons(this->scope);
	tempBuffer.primitive = htons(this->primtiveId);
	tempBuffer.requestId = htons(this->requestId);
	tempBuffer.reserved = 0;

	memcpy(buffer, &tempBuffer, size);	// Copy the struct to the buffer

	return 0;

}

 int ACS_CS_Header::setBuffer (const char *buffer, int size)
{

	if (buffer == 0)			// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::setBuffer()\n"
			"Error: Null pointer\n"));

		return -1;
	}
	if (size != CS_Header_Size)	// Check buffer size
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::setBuffer()\n"
			"Error: Wrong header size, size = %d\n",
			size));

		return -1;
	}

	CS_HeaderBuffer tempBuffer;	// Temporary buffer to hold the header

	memcpy(&tempBuffer, buffer, size);	// Copy buffer to struct

	// Convert to host byte order
	tempBuffer.version = ntohs(tempBuffer.version);
	tempBuffer.length = ntohs(tempBuffer.length);
	tempBuffer.scope = ntohs(tempBuffer.scope);
	tempBuffer.primitive = ntohs(tempBuffer.primitive);
	tempBuffer.requestId = ntohs(tempBuffer.requestId);

	// Check for valid ranges and cast the values

	if (CS_ProtocolChecker::checkVersion(tempBuffer.version))			// Check for valid version
		this->version = static_cast<CS_Version_Identifier> (tempBuffer.version);
	else
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::setBuffer()\n"
			"Error: Wrong HWC version, version = %d\n",
			tempBuffer.version));

		this->version = Version_NotSpecified;

		std::stringstream tmpEventText;
		tmpEventText<<"Protocol Version: "<< tempBuffer.version <<" unknown.";

		ACS_CS_EVENT(Event_ProgrammingError, ACS_CS_EventReporter::Severity_Event,
						"Error Parsing Data",
						tmpEventText.str(),
						"");

		ACS_CS_VersionException ex;
		ex.setDescription("Not a valid version");
		throw ex;
	}

	if (tempBuffer.length < CS_Header_Size)			// Check for valid length
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::setBuffer()\n"
			"Error: Wrong length, length = %d\n",
			tempBuffer.length));

		this->totalLength = CS_Header_Size;

		ACS_CS_ParsingException ex;
		ex.setDescription("Not a valid length");
		throw ex;
	}
	else
		this->totalLength = tempBuffer.length;

	if (CS_ProtocolChecker::checkScope(tempBuffer.scope))		// Check for valid scope
		this->scope = static_cast<CS_Scope_Identifier> (tempBuffer.scope);
	else
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::setBuffer()\n"
			"Error: Wrong scope, scope = %d\n",
			tempBuffer.scope));

		this->scope = Scope_NotSpecified;

		ACS_CS_ParsingException ex;
		ex.setDescription("Not a valid scope");
		throw ex;
	}

	if (CS_ProtocolChecker::checkPrimitive(tempBuffer.primitive, tempBuffer.scope))	// Check for valid primitive id
		this->primtiveId = static_cast<CS_Primitive_Identifier> (tempBuffer.primitive);
	else
	{
		ACS_CS_TRACE((ACS_CS_Header_TRACE,
			"(%t) ACS_CS_Header::setBuffer()\n"
			"Error: Wrong primitive, primitive = %d\n",
			tempBuffer.primitive));

		this->primtiveId = Primitive_NotSpecified;

		std::stringstream tmpEventText;
		tmpEventText<<"Primitive Id: "<< tempBuffer.primitive <<" unknown.";

		ACS_CS_EVENT(Event_ProgrammingError, ACS_CS_EventReporter::Severity_Event,
						"Error Parsing Data",
						tmpEventText.str(),
						"");

		ACS_CS_ParsingException ex;
		ex.setDescription("Not a valid primitive");
		throw ex;
	}

	this->requestId = tempBuffer.requestId;		// Get request id

	return 0;

}

 short unsigned ACS_CS_Header::getTotalLength () const
{

	return this->totalLength;

}

 void ACS_CS_Header::setTotalLength (int length)
{

   if (length >= ACS_CS_Protocol::CS_Header_Size)
	   this->totalLength = length;

}


ACS_CS_HeaderBase * ACS_CS_Header::clone () const
{
	 return new ACS_CS_Header(*this);
}

// Additional Declarations


