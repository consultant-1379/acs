//	Copyright Ericsson AB 2019. All rights reserved.

#include "ACS_CS_API_Set_Implementation.h"

// ACS_CS_API_Set_R3
#include "ACS_CS_API_Set_R3.h"

// Class ACS_CS_API_Set_R3
class ACS_CS_API_Set_Implementation;

ACS_CS_API_Set_Implementation* ACS_CS_API_Set_R3::implementation = 0;

ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R3::ceaseClusterOpModeAlarm()  // TR HX45316
{
	if (implementation == 0)
	{
		implementation = new ACS_CS_API_Set_Implementation;
	}
	if (implementation)
	{
		return implementation->ceaseClusterOpModeAlarm();
	}
	else
	{
		return ACS_CS_API_SET_NS::Result_Failure;
	}
}

