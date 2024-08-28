

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_CPId_GetCPList
#include "ACS_CS_CPId_GetCPList.h"


ACS_CS_Trace_TDEF(ACS_CS_CPId_GetCPList_TRACE);



// Class ACS_CS_CPId_GetCPList 

ACS_CS_CPId_GetCPList::ACS_CS_CPId_GetCPList()
{
}

ACS_CS_CPId_GetCPList::ACS_CS_CPId_GetCPList(const ACS_CS_CPId_GetCPList &right)
  : ACS_CS_Primitive(right)
{

   // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

}


ACS_CS_CPId_GetCPList::~ACS_CS_CPId_GetCPList()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPList::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetCPList;

}

 int ACS_CS_CPId_GetCPList::setBuffer (const char */*buffer*/, int /*size*/)
{
   
   // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 int ACS_CS_CPId_GetCPList::getBuffer (char */*buffer*/, int /*size*/) const
{
   
   // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 short unsigned ACS_CS_CPId_GetCPList::getLength () const
{
   
   return 0;

}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPList::clone () const
{

   return new ACS_CS_CPId_GetCPList(*this);

}

// Additional Declarations

