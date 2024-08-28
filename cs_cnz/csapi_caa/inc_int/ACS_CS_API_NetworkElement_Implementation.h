//	*********************************************************
//	********************
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
//	********************

#ifndef ACS_CS_API_NetworkElement_Implementation_h
#define ACS_CS_API_NetworkElement_Implementation_h 1


#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_API_OmProfileChange_R1.h"
#include "ACS_CS_ImIMMReader.h"

class ACS_CS_Attribute;

class ACS_CS_API_NetworkElement_Implementation 
{

  public:
	  ACS_CS_API_NetworkElement_Implementation();

      ACS_CS_API_NetworkElement_Implementation(bool logicalSubTree);

      virtual ~ACS_CS_API_NetworkElement_Implementation();

      ACS_CS_API_NS::CS_API_Result isMultipleCPSystem (bool &multipleCPSystem);

      ACS_CS_API_NS::CS_API_Result isTestEnvironment (bool &testEnvironment);

      ACS_CS_API_NS::CS_API_Result getFrontAPG (APID &frontAPG);

      ACS_CS_API_NS::CS_API_Result getNEID (ACS_CS_API_Name_R1 &neid);

      ACS_CS_API_NS::CS_API_Result getSingleSidedCPCount (uint32_t &cpCount);

      ACS_CS_API_NS::CS_API_Result getDoubleSidedCPCount (uint32_t &cpCount);

      ACS_CS_API_NS::CS_API_Result getAPGCount (uint32_t &apgCount);

      ACS_CS_API_NS::CS_API_Result getAlarmMaster (CPID &alarmMaster);

      ACS_CS_API_NS::CS_API_Result getClockMaster (CPID &clockMaster);

      ACS_CS_API_NS::CS_API_Result getBSOMIPAddress (uint32_t &addressEthA, uint32_t &addressEthB);

      ACS_CS_API_NS::CS_API_Result getDefaultCPName (CPID cpid, ACS_CS_API_Name_R1 &name);

      ACS_CS_API_NS::CS_API_Result getOmProfile (ACS_CS_API_OmProfileChange_R1 &omProfileChange);

      ACS_CS_API_NS::CS_API_Result getClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpModeOut);

      ACS_CS_API_NS::CS_API_Result getTrafficIsolated (CPID &trafficIsolatedCpId);

      ACS_CS_API_NS::CS_API_Result getTrafficLeader (CPID &trafficLeaderCpId);

      ACS_CS_API_NS::CS_API_Result getAPTType (std::string &aptType);

      ACS_CS_API_NS::CS_API_Result getNodeArchitecture(ACS_CS_API_CommonBasedArchitecture::ArchitectureValue &nodeArchitectureOut);

      ACS_CS_API_NS::CS_API_Result getApgNumber(ACS_CS_API_CommonBasedArchitecture::ApgNumber &apgNumberOut);

      ACS_CS_API_NS::CS_API_Result getBGCIVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2]);

      ACS_CS_API_NS::CS_API_Result getSOLVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2]);

      ACS_CS_API_NS::CS_API_Result getDmxcAddress(uint32_t &addressBgciA, uint32_t &addressBgciB);

      ACS_CS_API_NS::CS_API_Result getApgBgciAddress(uint16_t apgNr, uint16_t side, uint32_t &ipA, uint32_t &ipB);

  private:

      ACS_CS_ImModel *model;
      ACS_CS_ImIMMReader * immReader;
      ACS_CS_ImCpCluster* cpCluster;
      ACS_CS_ImAdvancedConfiguration* advInfo;

      ACS_CS_API_NetworkElement_Implementation(const ACS_CS_API_NetworkElement_Implementation &right);

      ACS_CS_API_NetworkElement_Implementation & operator=(const ACS_CS_API_NetworkElement_Implementation &right);

  private: //## implementation
};


#endif
