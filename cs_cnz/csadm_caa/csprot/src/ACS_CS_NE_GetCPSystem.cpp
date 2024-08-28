

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetCPSystem
#include "ACS_CS_NE_GetCPSystem.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetCPSystem_TRACE);



// Class ACS_CS_NE_GetCPSystem 

ACS_CS_NE_GetCPSystem::ACS_CS_NE_GetCPSystem()
{
}

ACS_CS_NE_GetCPSystem::ACS_CS_NE_GetCPSystem(const ACS_CS_NE_GetCPSystem &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetCPSystem::~ACS_CS_NE_GetCPSystem()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetCPSystem::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPSystem;

}

 ACS_CS_Primitive * ACS_CS_NE_GetCPSystem::clone () const
{

   return new ACS_CS_NE_GetCPSystem(*this);

}

// Additional Declarations

