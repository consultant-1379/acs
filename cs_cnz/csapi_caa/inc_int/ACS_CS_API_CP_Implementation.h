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

#ifndef ACS_CS_API_CP_Implementation_h
#define ACS_CS_API_CP_Implementation_h 1


// ACS_CS_API_CP_R1
#include "ACS_CS_API_CP_R1.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImIMMReader.h"

class ACS_CS_Table;

class ACS_CS_API_CP_Implementation : public ACS_CS_API_CP_R1
{

  public:
      ACS_CS_API_CP_Implementation();

      virtual ~ACS_CS_API_CP_Implementation();

      virtual ACS_CS_API_NS::CS_API_Result getCPId (const ACS_CS_API_Name_R1 &name, CPID &cpid);

      virtual ACS_CS_API_NS::CS_API_Result getCPName (CPID cpid, ACS_CS_API_Name_R1 &name);

      virtual ACS_CS_API_NS::CS_API_Result getAPZType (CPID cpid, ACS_CS_API_NS::CS_API_APZ_Type &type);

      virtual ACS_CS_API_NS::CS_API_Result getCPList (ACS_CS_API_IdList_R1 &cpList);

      virtual ACS_CS_API_NS::CS_API_Result getAPZSystem (CPID cpid, uint16_t &system);

      virtual ACS_CS_API_NS::CS_API_Result getCPType (CPID cpid, uint16_t &type);

      virtual ACS_CS_API_NS::CS_API_Result getCPAliasName (CPID cpid, ACS_CS_API_Name_R1 &alias, bool &isAlias);

      //	Fetch CP state
      virtual ACS_CS_API_NS::CS_API_Result getState (CPID cpid, ACS_CS_API_NS::CpState &cpState);

      //	Fetch application ID
      virtual ACS_CS_API_NS::CS_API_Result getApplicationId (CPID cpid, ACS_CS_API_NS::ApplicationId &type);

      //	Fetch the APZ substate
      virtual ACS_CS_API_NS::CS_API_Result getApzSubstate (CPID cpid, ACS_CS_API_NS::ApzSubstate &type);

      //	Fetch the state transition
      virtual ACS_CS_API_NS::CS_API_Result getStateTransition (CPID cpid, ACS_CS_API_NS::StateTransition &type);

      //	Fetch APT substate
      virtual ACS_CS_API_NS::CS_API_Result getAptSubstate (CPID cpid, ACS_CS_API_NS::AptSubstate &type);

      //	Fetch all the quorum attributes given a CPID
      virtual ACS_CS_API_NS::CS_API_Result getCPQuorumData (const CPID cpid, ACS_CS_API_CpData_R1 &quorumData);

      //	Fetch blocking Info
      virtual ACS_CS_API_NS::CS_API_Result getBlockingInfo (const CPID cpid, ACS_CS_API_NS::BlockingInfo &blockingInfo);

      //	Fetch Cp capacity
      virtual ACS_CS_API_NS::CS_API_Result getCpCapacity (const CPID cpid, ACS_CS_API_NS::CpCapacity &cpCapacity);

      //        Fetch CP MauType
      virtual ACS_CS_API_NS::CS_API_Result getMauType (CPID cpid, ACS_CS_API_NS::MauType &type);
      
  private:
      ACS_CS_ImModel *model;
      ACS_CS_ImIMMReader * immReader;
      const ACS_CS_ImBase* getCpObject(const CPID cpid);
};

#endif
