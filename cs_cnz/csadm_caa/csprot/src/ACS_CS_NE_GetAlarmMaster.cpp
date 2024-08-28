

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_NE_GetAlarmMaster
#include "ACS_CS_NE_GetAlarmMaster.h"


ACS_CS_Trace_TDEF(ACS_CS_NE_GetAlarmMaster_TRACE);



// Class ACS_CS_NE_GetAlarmMaster 

ACS_CS_NE_GetAlarmMaster::ACS_CS_NE_GetAlarmMaster()
{
}

ACS_CS_NE_GetAlarmMaster::ACS_CS_NE_GetAlarmMaster(const ACS_CS_NE_GetAlarmMaster &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetAlarmMaster::~ACS_CS_NE_GetAlarmMaster()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetAlarmMaster::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetAlarmMaster;

}

 ACS_CS_Primitive * ACS_CS_NE_GetAlarmMaster::clone () const
{

   return new ACS_CS_NE_GetAlarmMaster(*this);

}

// Additional Declarations

