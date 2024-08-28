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


#ifndef ACS_CS_API_Set_Implementation_h
#define ACS_CS_API_Set_Implementation_h 1

#include "ACS_CS_API_Set_R3.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_QuorumData_R1.h"
#include "ACS_CS_API_SetQuorumData.h"

using namespace ACS_CS_INTERNAL_API;


class ACS_CS_API_Set_Implementation 
{

  public:

      ACS_CS_API_Set_Implementation();

      virtual ~ACS_CS_API_Set_Implementation();

      static ACS_CS_API_SET_NS::CS_API_Set_Result setAlarmMaster (CPID alarmMaster);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setClockMaster (CPID clockMaster);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setClusterOpModeChanged (ACS_CS_API_RequestedClusterOpMode::RequestedValue clusterOpMode);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setQuorumData (ACS_CS_API_QuorumData_R1 &quorumData);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setProfiles (ACS_CS_API_OmProfileChange_R1::Profile omProfile, ACS_CS_API_OmProfileChange_R1::Profile aptProfile, ACS_CS_API_OmProfileChange_R1::Profile apzProfile);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setOmProfileNotificationStatusSuccess (ACS_CS_API_OmProfilePhase::PhaseValue phase);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setOmProfileNotificationStatusFailure (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_Set_R1::ReasonType reason);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setClusterRecovery ();

      static ACS_CS_API_SET_NS::CS_API_Set_Result setOpGroupDissolved ();

	  static ACS_CS_API_SET_NS::CS_API_Set_Result setSoftwareVersion (std::string version,std::string mag,unsigned short boardId);

	  static ACS_CS_API_SET_NS::CS_API_Set_Result ceaseClusterOpModeAlarm();			// TR HX45316

  private:

      ACS_CS_API_Set_Implementation(const ACS_CS_API_Set_Implementation &right);

      ACS_CS_API_Set_Implementation & operator=(const ACS_CS_API_Set_Implementation &right);

      static ACS_CS_API_SET_NS::CS_API_Set_Result setAPAProfiles (ACS_CS_API_OmProfileChange_R1::Profile aptProfile, ACS_CS_API_OmProfileChange_R1::Profile apzProfile);

      static ACS_CS_API_SET_NS::CS_API_Set_Result clearOgGroup (ACS_CS_API_OgChange_R1::ACS_CS_API_EmptyOG_ReasonCode reasonCode);

};


#endif
