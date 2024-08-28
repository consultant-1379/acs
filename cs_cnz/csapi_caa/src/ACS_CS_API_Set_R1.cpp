//	Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_API_Set_Implementation.h"


// ACS_CS_API_Set_R1
#include "ACS_CS_API_Set_R1.h"


// Class ACS_CS_API_Set_R1 

 ACS_CS_API_Set_Implementation* ACS_CS_API_Set_R1::implementation = 0;


 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setAlarmMaster (CPID alarmMaster)
{
   if (implementation == 0)
		implementation = new ACS_CS_API_Set_Implementation();

	if (implementation)
      return implementation->setAlarmMaster(alarmMaster);
	else
      return ACS_CS_API_SET_NS::Result_Failure;

}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setClockMaster (CPID clockMaster)
{
   if (implementation == 0)
		implementation = new ACS_CS_API_Set_Implementation();

	if (implementation)
      return implementation->setClockMaster(clockMaster);
	else
		return ACS_CS_API_SET_NS::Result_Failure;

}


  ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setClusterOpModeChanged (ACS_CS_API_RequestedClusterOpMode::RequestedValue clusterOpMode)
 {
    if (implementation == NULL)
    {
       implementation = new ACS_CS_API_Set_Implementation;
    }
    if (implementation == NULL)
    {
       return ACS_CS_API_SET_NS::Result_Failure;
    }

    return implementation->setClusterOpModeChanged(clusterOpMode);
 }

  ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setQuorumData (ACS_CS_API_QuorumData_R1& quorumData)
 {
    if (implementation == NULL)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation != NULL)
    {
       return implementation->setQuorumData(quorumData);
    }

    return ACS_CS_API_SET_NS::Result_Failure;

 }

  ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setProfiles (ACS_CS_API_OmProfileChange_R1::Profile omProfile, ACS_CS_API_OmProfileChange_R1::Profile aptProfile, ACS_CS_API_OmProfileChange_R1::Profile apzProfile)
 {
    if (implementation == NULL)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation != NULL)
    {
       return implementation->setProfiles(omProfile, aptProfile, apzProfile);
    }

    // no success, notify caller we had a failure
    return ACS_CS_API_SET_NS::Result_Failure;

 }

  ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setOmProfileNotificationStatusSuccess (ACS_CS_API_OmProfilePhase::PhaseValue phase)
 {
    if (implementation == NULL)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation != NULL)
    {
       return implementation->setOmProfileNotificationStatusSuccess(phase);
    }

    // no success, notify caller we had a failure
    return ACS_CS_API_SET_NS::Result_Failure;

 }

  ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setOmProfileNotificationStatusFailure (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_Set_R1::ReasonType reason)
 {
    if (implementation == NULL)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation != NULL)
    {
       return implementation->setOmProfileNotificationStatusFailure(phase, reason);
    }

    // no success, notify caller we had a failure
    return ACS_CS_API_SET_NS::Result_Failure;

 }

  ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setClusterRecovery ()
 {
    if (implementation == NULL)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation != NULL)
    {
       return implementation->setClusterRecovery();
    }

    // no success, notify caller we had a failure
    return ACS_CS_API_SET_NS::Result_Failure;

 }

  ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R1::setOpGroupDissolved ()
 {
    if (implementation == NULL)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation != NULL)
    {
       return implementation->setOpGroupDissolved();
    }

    // no success, notify caller we had a failure
    return ACS_CS_API_SET_NS::Result_Failure;

 }
