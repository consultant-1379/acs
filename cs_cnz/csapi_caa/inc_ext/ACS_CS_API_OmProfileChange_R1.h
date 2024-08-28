//	*********************************************************
//	********************
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
//	********************

#ifndef ACS_CS_API_OmProfileChange_R1_h
#define ACS_CS_API_OmProfileChange_R1_h 1


// ACS_CS_API_Common_R1
#include "ACS_CS_API_Common_R1.h"



struct ACS_CS_API_OmProfileChange_R1 
{
      typedef uint32_t Profile;

    static const Profile UnspecifiedProfile = -1;

    static const Profile BladeProfile = 0;

    typedef enum { UnspecifiedReason = -1, NoChange = 0, NechCommand = 1, AutomaticProfileAlignment = 2, NechCommandForApgOnly = 3 }
     ChangeReasonValue;

       ACS_CS_API_OmProfilePhase::PhaseValue phase;

       Profile omProfileCurrent;

       Profile apzCurrent;

       Profile aptCurrent;

       Profile omProfileRequested;

       Profile apzRequested;

       Profile aptRequested;

       Profile apzQueued;

       Profile aptQueued;

      ChangeReasonValue changeReason;

};


#endif
