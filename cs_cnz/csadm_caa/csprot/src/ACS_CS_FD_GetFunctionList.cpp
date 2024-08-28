

//	Copyright Ericsson AB 2007. All rights reserved.




// ACS_CS_FD_GetFunctionList
#include "ACS_CS_FD_GetFunctionList.h"



// Class ACS_CS_FD_GetFunctionList 

ACS_CS_FD_GetFunctionList::ACS_CS_FD_GetFunctionList()
{
}

ACS_CS_FD_GetFunctionList::ACS_CS_FD_GetFunctionList(const ACS_CS_FD_GetFunctionList &right)
  : ACS_CS_Primitive(right)
{
}


ACS_CS_FD_GetFunctionList::~ACS_CS_FD_GetFunctionList()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_FD_GetFunctionList::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetFunctionList;

}

 int ACS_CS_FD_GetFunctionList::setBuffer (const char */*buffer*/, int /*size*/)
{

   return 0;

}

 int ACS_CS_FD_GetFunctionList::getBuffer (char */*buffer*/, int /*size*/) const
{

   return 0;

}

 short unsigned ACS_CS_FD_GetFunctionList::getLength () const
{

   return 0;

}

 ACS_CS_Primitive * ACS_CS_FD_GetFunctionList::clone () const
{

   return new ACS_CS_FD_GetFunctionList(*this);

}

// Additional Declarations

