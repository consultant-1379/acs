





#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetTrafficLeader
#include "ACS_CS_NE_GetTrafficLeader.h"



// Class ACS_CS_NE_GetTrafficLeader 

ACS_CS_NE_GetTrafficLeader::ACS_CS_NE_GetTrafficLeader()
{
}

ACS_CS_NE_GetTrafficLeader::ACS_CS_NE_GetTrafficLeader(const ACS_CS_NE_GetTrafficLeader &right)

  : ACS_CS_NE_BasicGet(right)

{
}


ACS_CS_NE_GetTrafficLeader::~ACS_CS_NE_GetTrafficLeader()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetTrafficLeader::getPrimitiveType () const
{

   // let the caller know what type of primative we are
   return ACS_CS_Protocol::Primitive_GetTrafficLeader;

}

 ACS_CS_Primitive * ACS_CS_NE_GetTrafficLeader::clone () const
{

   // return a new copy of this object.
   return new ACS_CS_NE_GetTrafficLeader(*this);

}

// Additional Declarations

