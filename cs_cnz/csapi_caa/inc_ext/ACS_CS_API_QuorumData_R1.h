//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************

#ifndef ACS_CS_API_QuorumData_R1_h
#define ACS_CS_API_QuorumData_R1_h 1

#include <vector>

#include "ACS_CS_API_OmProfileChange_R1.h"
#include "ACS_CS_API_CpData_R1.h"


struct ACS_CS_API_QuorumData_R1 
{
       CPID trafficIsolatedCpId;

       CPID trafficLeaderCpId;

       bool automaticQuorumRecovery;

       ACS_CS_API_OmProfileChange_R1::Profile apzProfile;

       ACS_CS_API_OmProfileChange_R1::Profile aptProfile;

      std::vector<ACS_CS_API_CpData_R1> cpData;

};


#endif
