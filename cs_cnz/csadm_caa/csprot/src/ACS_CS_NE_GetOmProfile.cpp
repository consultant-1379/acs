

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************




// ACS_CS_NE_GetOmProfile
#include "ACS_CS_NE_GetOmProfile.h"



// Class ACS_CS_NE_GetOmProfile 

ACS_CS_NE_GetOmProfile::ACS_CS_NE_GetOmProfile()
{
}

ACS_CS_NE_GetOmProfile::ACS_CS_NE_GetOmProfile(const ACS_CS_NE_GetOmProfile &right)
  : ACS_CS_NE_BasicGet(right)
{
}


ACS_CS_NE_GetOmProfile::~ACS_CS_NE_GetOmProfile()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetOmProfile::getPrimitiveType () const
{
	return ACS_CS_Protocol::Primitive_GetOmProfile;
}

 ACS_CS_Primitive * ACS_CS_NE_GetOmProfile::clone () const
{
	return new ACS_CS_NE_GetOmProfile(*this);
}

// Additional Declarations

