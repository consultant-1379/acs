





#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetTrafficIsolated
#include "ACS_CS_NE_GetTrafficIsolated.h"



// Class ACS_CS_NE_GetTrafficIsolated 

ACS_CS_NE_GetTrafficIsolated::ACS_CS_NE_GetTrafficIsolated()
{
}

ACS_CS_NE_GetTrafficIsolated::ACS_CS_NE_GetTrafficIsolated(const ACS_CS_NE_GetTrafficIsolated &right)

  : ACS_CS_NE_BasicGet(right)

{
}


ACS_CS_NE_GetTrafficIsolated::~ACS_CS_NE_GetTrafficIsolated()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetTrafficIsolated::getPrimitiveType () const
{

   // let the caller know what type of primative we are
   return ACS_CS_Protocol::Primitive_GetTrafficIsolated;

}

 ACS_CS_Primitive * ACS_CS_NE_GetTrafficIsolated::clone () const
{

   // return a new copy of this object.
   return new ACS_CS_NE_GetTrafficIsolated(*this);

}

// Additional Declarations

