

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetTestEnvironment
#include "ACS_CS_NE_GetTestEnvironment.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetTestEnvironment_TRACE);



// Class ACS_CS_NE_GetTestEnvironment 

ACS_CS_NE_GetTestEnvironment::ACS_CS_NE_GetTestEnvironment()
{
}

ACS_CS_NE_GetTestEnvironment::ACS_CS_NE_GetTestEnvironment(const ACS_CS_NE_GetTestEnvironment &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetTestEnvironment::~ACS_CS_NE_GetTestEnvironment()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetTestEnvironment::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetTestEnvironment;

}

 ACS_CS_Primitive * ACS_CS_NE_GetTestEnvironment::clone () const
{

   return new ACS_CS_NE_GetTestEnvironment(*this);

}

// Additional Declarations

