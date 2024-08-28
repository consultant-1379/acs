

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_BasicResponse_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_BasicResponse 

ACS_CS_BasicResponse::ACS_CS_BasicResponse()
      : resultCode(ACS_CS_Protocol::Result_NotSpecified)
{
}

ACS_CS_BasicResponse::ACS_CS_BasicResponse(const ACS_CS_BasicResponse &right)
  : ACS_CS_Primitive(right)
{

   this->resultCode = right.resultCode;

}


ACS_CS_BasicResponse::~ACS_CS_BasicResponse()
{
}



 int ACS_CS_BasicResponse::setBuffer (const char *buffer, int size)
{

	if (buffer == 0)		// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_BasicResponse_TRACE,
			"(%t) ACS_CS_BasicResponse::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != sizeof(unsigned short))		// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_BasicResponse_TRACE,
			"(%t) ACS_CS_BasicResponse::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	// Get pointer to result code
	const unsigned short * tempPtr = reinterpret_cast<const unsigned short *> (buffer);
	unsigned short tempShort = ntohs(*tempPtr);	// Get result code
	
	// Check and cast result code
	if (CS_ProtocolChecker::checkResultCode(tempShort))
		resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting attribute identifier");
		throw ex;
	}

	return 0;

}

 int ACS_CS_BasicResponse::getBuffer (char *buffer, int size) const
{

	if (buffer == 0)		// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_BasicResponse_TRACE,
			"(%t) ACS_CS_BasicResponse::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != sizeof(unsigned short))		// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_BasicResponse_TRACE,
			"(%t) ACS_CS_BasicResponse::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	// Convert and copy result code
	unsigned short tus = static_cast<unsigned short>(resultCode);
	unsigned short tempShort = htons(tus);
   *(reinterpret_cast<unsigned short*>(buffer)) = tempShort;

	return 0;

}

 short unsigned ACS_CS_BasicResponse::getLength () const
{

	return 2;

}

 void ACS_CS_BasicResponse::setResultCode (ACS_CS_Protocol::CS_Result_Code result)
{

	resultCode = result;

}

 ACS_CS_Protocol::CS_Result_Code ACS_CS_BasicResponse::getResultCode () const
{

	return resultCode;

}

// Additional Declarations

