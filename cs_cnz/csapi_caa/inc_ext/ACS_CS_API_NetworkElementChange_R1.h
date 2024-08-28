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

#ifndef ACS_CS_API_NetworkElementChange_R1_h
#define ACS_CS_API_NetworkElementChange_R1_h 1

#include "ACS_CS_API_OmProfileChange_R1.h"
#include "ACS_CS_API_Common_R1.h"


struct ACS_CS_API_NetworkElementChange_R1 
{
       ACS_CS_API_Name_R1 neId;

      ACS_CS_API_OmProfileChange_R1 omProfile;

       ACS_CS_API_ClusterOpMode::Value clusterMode;

       CPID trafficIsolatedCpId;

       CPID trafficLeaderCpId;

       CPID alarmMasterCpId;

       CPID clockMasterCpId;

};


#endif
