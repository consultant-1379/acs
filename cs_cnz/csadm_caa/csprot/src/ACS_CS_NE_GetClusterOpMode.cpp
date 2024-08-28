




#include "ACS_CS_Trace.h"

// ACS_CS_NE_GetClusterOpMode
#include "ACS_CS_NE_GetClusterOpMode.h"

ACS_CS_Trace_TDEF(ACS_CS_NE_GetClusterOpMode_TRACE);


// Class ACS_CS_NE_GetClusterOpMode 

ACS_CS_NE_GetClusterOpMode::ACS_CS_NE_GetClusterOpMode()
{
}

ACS_CS_NE_GetClusterOpMode::ACS_CS_NE_GetClusterOpMode(const ACS_CS_NE_GetClusterOpMode &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetClusterOpMode::~ACS_CS_NE_GetClusterOpMode()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetClusterOpMode::getPrimitiveType () const
{
     return ACS_CS_Protocol::Primitive_GetClusterOpMode;
}

 ACS_CS_Primitive * ACS_CS_NE_GetClusterOpMode::clone () const
{
     return new ACS_CS_NE_GetClusterOpMode(*this);
}

// Additional Declarations

