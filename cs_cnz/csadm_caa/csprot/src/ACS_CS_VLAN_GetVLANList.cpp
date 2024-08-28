

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_VLAN_GetVLANList
#include "ACS_CS_VLAN_GetVLANList.h"


ACS_CS_Trace_TDEF(ACS_CS_VLAN_GetVLANList_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_VLAN_GetVLANList 

ACS_CS_VLAN_GetVLANList::ACS_CS_VLAN_GetVLANList()
{
}

ACS_CS_VLAN_GetVLANList::ACS_CS_VLAN_GetVLANList(const ACS_CS_VLAN_GetVLANList &right)
  : ACS_CS_Primitive(right)
{
}


ACS_CS_VLAN_GetVLANList::~ACS_CS_VLAN_GetVLANList()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_VLAN_GetVLANList::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetVLANList;

}

 int ACS_CS_VLAN_GetVLANList::setBuffer (const char */*buffer*/, int /*size*/)
{

    // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 int ACS_CS_VLAN_GetVLANList::getBuffer (char */*buffer*/, int /*size*/) const
{

    // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 short unsigned ACS_CS_VLAN_GetVLANList::getLength () const
{

   return 0;

}

 ACS_CS_Primitive * ACS_CS_VLAN_GetVLANList::clone () const
{

   return new ACS_CS_VLAN_GetVLANList(*this);

}

// Additional Declarations

