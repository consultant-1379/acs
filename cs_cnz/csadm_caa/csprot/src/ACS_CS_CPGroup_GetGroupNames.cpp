

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_CPGroup_GetGroupNames
#include "ACS_CS_CPGroup_GetGroupNames.h"


ACS_CS_Trace_TDEF(ACS_CS_CPGroup_GetGroupNames_TRACE);



// Class ACS_CS_CPGroup_GetGroupNames 

ACS_CS_CPGroup_GetGroupNames::ACS_CS_CPGroup_GetGroupNames()
{
}

ACS_CS_CPGroup_GetGroupNames::ACS_CS_CPGroup_GetGroupNames(const ACS_CS_CPGroup_GetGroupNames &right)
  : ACS_CS_Primitive(right)
{
}


ACS_CS_CPGroup_GetGroupNames::~ACS_CS_CPGroup_GetGroupNames()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPGroup_GetGroupNames::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetGroupNames;

}

 int ACS_CS_CPGroup_GetGroupNames::setBuffer (const char */*buffer*/, int /*size*/)
{

   return 0;

}

 int ACS_CS_CPGroup_GetGroupNames::getBuffer (char */*buffer*/, int /*size*/) const
{

   return 0;

}

 short unsigned ACS_CS_CPGroup_GetGroupNames::getLength () const
{

   return 0;

}

 ACS_CS_Primitive * ACS_CS_CPGroup_GetGroupNames::clone () const
{

   return new ACS_CS_CPGroup_GetGroupNames(*this);

}

// Additional Declarations

