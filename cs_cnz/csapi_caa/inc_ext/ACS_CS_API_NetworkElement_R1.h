#ifndef ACS_CS_API_NetworkElement_R1_h
#define ACS_CS_API_NetworkElement_R1_h 1

#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_OmProfileChange_R1.h"

class ACS_CS_API_NetworkElement_Implementation;

class ACS_CS_API_NetworkElement_R1
{

  public:

		static ACS_CS_API_NS::CS_API_Result isMultipleCPSystem (bool &multipleCPSystem);

		static ACS_CS_API_NS::CS_API_Result isTestEnvironment (bool &testEnvironment);

		static ACS_CS_API_NS::CS_API_Result getFrontAPG (APID &frontAPG);

		static ACS_CS_API_NS::CS_API_Result getNEID (ACS_CS_API_Name_R1 &neid);

		static ACS_CS_API_NS::CS_API_Result getSingleSidedCPCount (uint32_t &cpCount);

		static ACS_CS_API_NS::CS_API_Result getDoubleSidedCPCount (uint32_t &cpCount);

		static ACS_CS_API_NS::CS_API_Result getAPGCount (uint32_t &apgCount);

		static ACS_CS_API_NS::CS_API_Result getAlarmMaster (CPID &alarmMaster);

		static ACS_CS_API_NS::CS_API_Result getClockMaster (CPID &clockMaster);

		static ACS_CS_API_NS::CS_API_Result getBSOMIPAddress (uint32_t &addressEthA, uint32_t &addressEthB);

		static ACS_CS_API_NS::CS_API_Result getDefaultCPName (CPID cpid, ACS_CS_API_Name_R1 &name);

		//	Fetches the current O&M Profile
		static ACS_CS_API_NS::CS_API_Result getOmProfile (ACS_CS_API_OmProfileChange_R1 &omProfile);

		//	Fetches the current cluster op mode
		static ACS_CS_API_NS::CS_API_Result getClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpModeOut);

		//	Retreives a CPID for a CP that is Traffic Isolated.
		static ACS_CS_API_NS::CS_API_Result getTrafficIsolated (CPID &trafficIsolatedCpId);

		//	Retreives a CPID for a CP that is theTraffic Leader.
		static ACS_CS_API_NS::CS_API_Result getTrafficLeader (CPID &trafficLeaderCpId);

		static ACS_CS_API_NS::CS_API_Result getAPTType (std::string &aptType);

		static ACS_CS_API_NS::CS_API_Result getNodeArchitecture(ACS_CS_API_CommonBasedArchitecture::ArchitectureValue &nodeArchitectureOut);

		static ACS_CS_API_NS::CS_API_Result getApgNumber(ACS_CS_API_CommonBasedArchitecture::ApgNumber &apgNumberOut);

		static ACS_CS_API_NS::CS_API_Result getBGCIVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2]);

		static ACS_CS_API_NS::CS_API_Result getDmxcAddress(uint32_t &addressBgciA, uint32_t &addressBgciB);

		static ACS_CS_API_NS::CS_API_Result getApgBgciAddress(uint16_t apgNr, uint16_t side, uint32_t &ipA, uint32_t &ipB);

		static ACS_CS_API_NS::CS_API_Result getSOLVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2]);
    // Additional Public Declarations

  protected:
      ACS_CS_API_NetworkElement_R1();

  private:

		ACS_CS_API_NetworkElement_R1(const ACS_CS_API_NetworkElement_R1 &right);

		ACS_CS_API_NetworkElement_R1 & operator=(const ACS_CS_API_NetworkElement_R1 &right);

//		static ACS_CS_API_NetworkElement_Implementation* implementation;

};


// Class ACS_CS_API_NetworkElement_R1 



#endif
