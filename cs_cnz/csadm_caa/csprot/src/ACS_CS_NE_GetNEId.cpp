

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetNEId
#include "ACS_CS_NE_GetNEId.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetNEId_TRACE);



// Class ACS_CS_NE_GetNEId 

ACS_CS_NE_GetNEId::ACS_CS_NE_GetNEId()
{
}

ACS_CS_NE_GetNEId::ACS_CS_NE_GetNEId(const ACS_CS_NE_GetNEId &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetNEId::~ACS_CS_NE_GetNEId()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetNEId::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetNEId;

}

 ACS_CS_Primitive * ACS_CS_NE_GetNEId::clone () const
{

   return new ACS_CS_NE_GetNEId(*this);

}

// Additional Declarations

