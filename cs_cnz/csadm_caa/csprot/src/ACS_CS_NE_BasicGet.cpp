

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_BasicGet_TRACE);



// Class ACS_CS_NE_BasicGet 

ACS_CS_NE_BasicGet::ACS_CS_NE_BasicGet()
{
}

ACS_CS_NE_BasicGet::ACS_CS_NE_BasicGet(const ACS_CS_NE_BasicGet &right)
  : ACS_CS_Primitive(right)
{
}


ACS_CS_NE_BasicGet::~ACS_CS_NE_BasicGet()
{
}



 int ACS_CS_NE_BasicGet::setBuffer (const char */*buffer*/, int /*size*/)
{

   // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 int ACS_CS_NE_BasicGet::getBuffer (char */*buffer*/, int /*size*/) const
{

   // This primitive doesn't have any attributes and this function should
   // therefore do nothing.

   return 0;

}

 short unsigned ACS_CS_NE_BasicGet::getLength () const
{

   return 0;

}

// Additional Declarations

