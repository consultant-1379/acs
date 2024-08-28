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

#ifndef ACS_CS_API_OgChange_R1_h
#define ACS_CS_API_OgChange_R1_h 1

#include "ACS_CS_API_Common_R1.h"

struct ACS_CS_API_OgChange_R1 
{
    typedef enum { NotEmpty = 0, ClusterRecovery = 1, OtherReason = 2 } ACS_CS_API_EmptyOG_ReasonCode;

    ACS_CS_API_IdList_R1 ogCpIdList;

    ACS_CS_API_EmptyOG_ReasonCode emptyOgReason;
};

#endif
