

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_HWC_SynchTableResponse
#include "ACS_CS_HWC_SynchTableResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_HWC_SynchTableResponse_TRACE);



// Class ACS_CS_HWC_SynchTableResponse 

ACS_CS_HWC_SynchTableResponse::ACS_CS_HWC_SynchTableResponse()
{
}

ACS_CS_HWC_SynchTableResponse::ACS_CS_HWC_SynchTableResponse(const ACS_CS_HWC_SynchTableResponse &right)
  : ACS_CS_BasicResponse(right)
{
}


ACS_CS_HWC_SynchTableResponse::~ACS_CS_HWC_SynchTableResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_HWC_SynchTableResponse::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_SynchTableResponse;

}

 ACS_CS_Primitive * ACS_CS_HWC_SynchTableResponse::clone () const
{

   return new ACS_CS_HWC_SynchTableResponse(*this);

}

// Additional Declarations

