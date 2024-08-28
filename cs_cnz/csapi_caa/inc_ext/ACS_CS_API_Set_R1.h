#ifndef ACS_CS_API_Set_R1_h
#define ACS_CS_API_Set_R1_h 1

#include <string>

// ACS_CS_API_Common_R1
#include "ACS_CS_API_Common_R1.h"
// ACS_CS_API_QuorumData_R1
#include "ACS_CS_API_QuorumData_R1.h"

#include "ACS_CS_API_OmProfileChange_R1.h"

class ACS_CS_API_Set_Implementation;

typedef uint16_t CPID;

namespace ACS_CS_API_SET_NS
{
    // CS API Set Result Codes
    enum CS_API_Set_Result
    {
        Result_Success      = 0,
        Result_NoAccess     = 12,
        Result_Failure      = 15,
        Result_Incorrect_CP_Id = 18
    };
}

class ACS_CS_API_Set_R1
{

  public:

		typedef enum { GeneralFailure = 1, CommandClassificationMissing=2, ClusterSessionLockOngoing = 3, InvalidCommandClassificationFile = 4  } ReasonType;

		static ACS_CS_API_SET_NS::CS_API_Set_Result setAlarmMaster (CPID alarmMaster);

		static ACS_CS_API_SET_NS::CS_API_Set_Result setClockMaster (CPID clockMaster);

		//	Set the cluster operation mode
		static ACS_CS_API_SET_NS::CS_API_Set_Result setClusterOpModeChanged (ACS_CS_API_RequestedClusterOpMode::RequestedValue clusterOpMode);

		//	Set overall and per-CP quorum data
		static ACS_CS_API_SET_NS::CS_API_Set_Result setQuorumData (ACS_CS_API_QuorumData_R1& quorumData);

		static ACS_CS_API_SET_NS::CS_API_Set_Result setProfiles (ACS_CS_API_OmProfileChange_R1::Profile omProfile, ACS_CS_API_OmProfileChange_R1::Profile aptProfile, ACS_CS_API_OmProfileChange_R1::Profile apzProfile);

		static ACS_CS_API_SET_NS::CS_API_Set_Result setOmProfileNotificationStatusSuccess (ACS_CS_API_OmProfilePhase::PhaseValue phase);

		static ACS_CS_API_SET_NS::CS_API_Set_Result setOmProfileNotificationStatusFailure (ACS_CS_API_OmProfilePhase::PhaseValue phase, ReasonType reason);

		static ACS_CS_API_SET_NS::CS_API_Set_Result setClusterRecovery ();

		static ACS_CS_API_SET_NS::CS_API_Set_Result setOpGroupDissolved ();

  protected:
		ACS_CS_API_Set_R1();

  private:
		ACS_CS_API_Set_R1(const ACS_CS_API_Set_R1 &right);

		ACS_CS_API_Set_R1 & operator=(const ACS_CS_API_Set_R1 &right);

		static  ACS_CS_API_Set_Implementation* implementation;
};

#endif
