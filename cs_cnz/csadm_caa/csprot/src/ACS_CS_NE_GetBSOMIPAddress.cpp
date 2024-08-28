

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetBSOMIPAddress
#include "ACS_CS_NE_GetBSOMIPAddress.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetBSOMIPAddress_TRACE);



// Class ACS_CS_NE_GetBSOMIPAddress 

ACS_CS_NE_GetBSOMIPAddress::ACS_CS_NE_GetBSOMIPAddress()
{
}

ACS_CS_NE_GetBSOMIPAddress::ACS_CS_NE_GetBSOMIPAddress(const ACS_CS_NE_GetBSOMIPAddress &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetBSOMIPAddress::~ACS_CS_NE_GetBSOMIPAddress()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetBSOMIPAddress::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetBSOMIPAddress;

}

 ACS_CS_Primitive * ACS_CS_NE_GetBSOMIPAddress::clone () const
{

   return new ACS_CS_NE_GetBSOMIPAddress(*this);

}

// Additional Declarations

