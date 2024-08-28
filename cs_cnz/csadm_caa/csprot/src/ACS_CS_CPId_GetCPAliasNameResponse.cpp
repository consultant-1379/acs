





#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPAliasNameResponse
#include "ACS_CS_CPId_GetCPAliasNameResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPAliasNameResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_CPId_GetCPAliasNameResponse 

ACS_CS_CPId_GetCPAliasNameResponse::ACS_CS_CPId_GetCPAliasNameResponse()
      : logicalClock(0),
        aliasFlag(true)
{
}

ACS_CS_CPId_GetCPAliasNameResponse::ACS_CS_CPId_GetCPAliasNameResponse(const ACS_CS_CPId_GetCPAliasNameResponse &right)
  : ACS_CS_BasicResponse(right)
{

	this->logicalClock = right.logicalClock;
	this->nameStr = right.nameStr;
	this->aliasFlag = right.aliasFlag;

}


ACS_CS_CPId_GetCPAliasNameResponse::~ACS_CS_CPId_GetCPAliasNameResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPAliasNameResponse::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetCPAliasNameResponse;

}

 int ACS_CS_CPId_GetCPAliasNameResponse::setBuffer (const char *buffer, int size)
{
	
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasNameResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));
		return -1;
	}
	else if (size < 2)	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasNameResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));
		return -1;
	}
	
	unsigned short tempShort = 0;
	
	// Get and convert Result code
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
	tempShort = ntohs(tempShort);
	
	// Check and cast result code
	if (CS_ProtocolChecker::checkResultCode(tempShort))
		this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}
	
	if (size >= 8)
	{
		// Get and convert logical clock
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
		this->logicalClock = ntohs(tempShort);

		// Get and convert alias flag
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 4) );
		unsigned short aliasFlagShort = ntohs(tempShort);
		this->aliasFlag = (aliasFlagShort == 0) ? false : true;
		
		// Get and convert name length
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 6) );
		unsigned short nameLength = ntohs(tempShort);
		
		if (nameLength > 0)
		{
			if (size != (nameLength + 8) )
			{
				ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasNameResponse_TRACE,
					"(%t) ACS_CS_CPId_GetCPAliasNameResponse::setBuffer()\n"
					"Error: Illegal buffer size , size= %d\n",
					size ));
				
				ACS_CS_ParsingException ex;
				ex.setDescription("Illegal buffer size");
				throw ex;
			}
			
			char * tempStr = new char[nameLength + 1];
			if (tempStr)
			{
				memcpy(tempStr, buffer + 8, nameLength);
				tempStr[nameLength] = 0;
				this->nameStr = tempStr;
				delete [] tempStr;
			}
		}
	}

   return 0;

}

 int ACS_CS_CPId_GetCPAliasNameResponse::getBuffer (char *buffer, int size) const
{
	
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasNameResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));
		return -1;
	}
	else if (size != this->getLength() )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_CPId_GetCPAliasNameResponse_TRACE,
			"(%t) ACS_CS_CPId_GetCPAliasNameResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));
		return -1;
	}
	
	unsigned short tempShort = 0;
	
	// Convert and copy result code
	tempShort = htons(this->resultCode);
	memcpy(buffer, &tempShort, 2);
	
	if (this->getLength() >= 8)
	{
		// Convert and copy logical clock
		tempShort = htons(this->logicalClock);
		memcpy(buffer + 2, &tempShort, 2);

		// Convert and copy alias flag
		unsigned short aliasFlagShort = (this->aliasFlag) ? 1 : 0;
		tempShort = htons(aliasFlagShort);
		memcpy(buffer + 4, &tempShort, 2);
		
		// Convert and copy name length
		unsigned short nameLength = static_cast<unsigned short>(this->nameStr.length());
		tempShort = htons(nameLength);
		memcpy(buffer + 6, &tempShort, 2);
		
		if (this->nameStr.length() > 0)
		{
			memcpy(buffer + 8, this->nameStr.c_str(), nameLength);
		}
	}
	
	return 0;

}

 short unsigned ACS_CS_CPId_GetCPAliasNameResponse::getLength () const
{
	
	if (this->resultCode == Result_Success) // If the result code is success the length must be calculated
		return (8 + static_cast<unsigned short>(this->nameStr.length()));
	else
		return 2;  // If the result code is not success, the length of this primitive is 2 bytes

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPAliasNameResponse::clone () const
{

	return new ACS_CS_CPId_GetCPAliasNameResponse(*this);

}

 short unsigned ACS_CS_CPId_GetCPAliasNameResponse::getLogicalClock () const
{

	return this->logicalClock;

}

 void ACS_CS_CPId_GetCPAliasNameResponse::setLogicalClock (unsigned short clock)
{

	this->logicalClock = clock;

}

 std::string ACS_CS_CPId_GetCPAliasNameResponse::getName () const
{

	return this->nameStr;

}

 void ACS_CS_CPId_GetCPAliasNameResponse::setName (std::string name)
{

	this->nameStr = name;

}

 bool ACS_CS_CPId_GetCPAliasNameResponse::isAlias () const
{

	return this->aliasFlag;

}

 void ACS_CS_CPId_GetCPAliasNameResponse::setAlias (bool flag)
{

	this->aliasFlag = flag;

}

// Additional Declarations

