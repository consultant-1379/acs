

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetClockMaster
#include "ACS_CS_NE_GetClockMaster.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetClockMaster_TRACE);



// Class ACS_CS_NE_GetClockMaster 

ACS_CS_NE_GetClockMaster::ACS_CS_NE_GetClockMaster()
{
}

ACS_CS_NE_GetClockMaster::ACS_CS_NE_GetClockMaster(const ACS_CS_NE_GetClockMaster &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetClockMaster::~ACS_CS_NE_GetClockMaster()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetClockMaster::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetClockMaster;

}

 ACS_CS_Primitive * ACS_CS_NE_GetClockMaster::clone () const
{

   return new ACS_CS_NE_GetClockMaster(*this);

}

// Additional Declarations

