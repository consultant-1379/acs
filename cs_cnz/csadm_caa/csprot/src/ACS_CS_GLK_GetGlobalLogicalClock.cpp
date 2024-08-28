

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_GLK_GetGlobalLogicalClock
#include "ACS_CS_GLK_GetGlobalLogicalClock.h"


ACS_CS_Trace_TDEF(ACS_CS_GLK_GetGlobalLogicalClock_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_GLK_GetGlobalLogicalClock 

ACS_CS_GLK_GetGlobalLogicalClock::ACS_CS_GLK_GetGlobalLogicalClock()
{
}

ACS_CS_GLK_GetGlobalLogicalClock::ACS_CS_GLK_GetGlobalLogicalClock(const ACS_CS_GLK_GetGlobalLogicalClock &right)
  : ACS_CS_Primitive(right)
{
}


ACS_CS_GLK_GetGlobalLogicalClock::~ACS_CS_GLK_GetGlobalLogicalClock()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_GLK_GetGlobalLogicalClock::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetGlobalLogicalClock;

}

 int ACS_CS_GLK_GetGlobalLogicalClock::setBuffer (const char */*buffer*/, int /*size*/)
{

    // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 int ACS_CS_GLK_GetGlobalLogicalClock::getBuffer (char */*buffer*/, int /*size*/) const
{

    // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 short unsigned ACS_CS_GLK_GetGlobalLogicalClock::getLength () const
{

   return 0;

}

 ACS_CS_Primitive * ACS_CS_GLK_GetGlobalLogicalClock::clone () const
{

   return new ACS_CS_GLK_GetGlobalLogicalClock(*this);

}

// Additional Declarations

