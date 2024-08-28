//	Copyright Ericsson AB 2019. All rights reserved.

#ifndef ACS_CS_API_Set_R3_h
#define ACS_CS_API_Set_R3_h 1

#include "ACS_CS_API_Set_R2.h"


class ACS_CS_API_Set_Implementation;


class ACS_CS_API_Set_R3 : public ACS_CS_API_Set_R2
{

public:
	static ACS_CS_API_SET_NS::CS_API_Set_Result ceaseClusterOpModeAlarm ();   // TR HX45316

protected:

private:
	ACS_CS_API_Set_R3();

	ACS_CS_API_Set_R3(const ACS_CS_API_Set_R3 &right);

	ACS_CS_API_Set_R3 & operator=(const ACS_CS_API_Set_R3 &right);

private:
	static ACS_CS_API_Set_Implementation* implementation;
};

#endif
