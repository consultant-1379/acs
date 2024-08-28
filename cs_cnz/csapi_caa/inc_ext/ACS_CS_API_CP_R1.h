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


#ifndef ACS_CS_API_CP_R1_h
#define ACS_CS_API_CP_R1_h 1



#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_CpData_R1.h"





class ACS_CS_API_CP_R1
{

  public:
		virtual ~ACS_CS_API_CP_R1();

		virtual ACS_CS_API_NS::CS_API_Result getCPId (const ACS_CS_API_Name_R1 &name, CPID &cpid) = 0;

		virtual ACS_CS_API_NS::CS_API_Result getCPName (CPID cpid, ACS_CS_API_Name_R1 &name) = 0;

		virtual ACS_CS_API_NS::CS_API_Result getAPZType (CPID cpid, ACS_CS_API_NS::CS_API_APZ_Type &type) = 0;

		virtual ACS_CS_API_NS::CS_API_Result getCPList (ACS_CS_API_IdList_R1 &cpList) = 0;

		virtual ACS_CS_API_NS::CS_API_Result getAPZSystem (CPID cpid, uint16_t &system) = 0;

		virtual ACS_CS_API_NS::CS_API_Result getCPType (CPID cpid, uint16_t &type) = 0;

		virtual ACS_CS_API_NS::CS_API_Result getCPAliasName (CPID cpid, ACS_CS_API_Name_R1 &alias, bool &isAlias) = 0;

		virtual ACS_CS_API_NS::CS_API_Result getState (CPID cpid, ACS_CS_API_NS::CpState &cpState) = 0;

		//	Fetch application ID
		virtual ACS_CS_API_NS::CS_API_Result getApplicationId (CPID cpid, ACS_CS_API_NS::ApplicationId &appId) = 0;

		//	Fetch the APZ substate
		virtual ACS_CS_API_NS::CS_API_Result getApzSubstate (CPID cpid, ACS_CS_API_NS::ApzSubstate &apzSubstate) = 0;

		//	Fetch the state transition
		virtual ACS_CS_API_NS::CS_API_Result getStateTransition (CPID cpid, ACS_CS_API_NS::StateTransition &stateTrans) = 0;

		//	Fetch APT substate
		virtual ACS_CS_API_NS::CS_API_Result getAptSubstate (CPID cpid, ACS_CS_API_NS::AptSubstate &aptSubstate) = 0;

		//	Fetch all the quorum attributes given a CPID
		virtual ACS_CS_API_NS::CS_API_Result getCPQuorumData (CPID cpid, ACS_CS_API_CpData_R1 &quorumData) = 0;

		//	Fetch blocking Info
		virtual ACS_CS_API_NS::CS_API_Result getBlockingInfo (const CPID cpid, ACS_CS_API_NS::BlockingInfo &blockingInfo) = 0;

		//	Fetch CP Capacity
		virtual ACS_CS_API_NS::CS_API_Result getCpCapacity (const CPID cpid, ACS_CS_API_NS::CpCapacity &cpCapacity) = 0;

		//      Fetch CP MauType
		virtual ACS_CS_API_NS::CS_API_Result getMauType (CPID cpid, ACS_CS_API_NS::MauType &type) = 0;
};


// Class ACS_CS_API_CP_R1 



#endif
