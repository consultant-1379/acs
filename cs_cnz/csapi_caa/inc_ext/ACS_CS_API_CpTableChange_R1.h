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

#ifndef ACS_CS_API_CpTableChange_R1_h
#define ACS_CS_API_CpTableChange_R1_h 1

#include "ACS_CS_API_Common_R1.h"


struct ACS_CS_API_CpTableData_R1 
{
       CPID cpId;
       ACS_CS_API_TableChangeOperation::OpType operationType;
       ACS_CS_API_Name_R1 cpName;
       ACS_CS_API_Name_R1 cpAliasName;
       uint16_t apzSystem;
       uint16_t cpType;
       ACS_CS_API_NS::CpState cpState;
       ACS_CS_API_NS::ApplicationId applicationId;
       ACS_CS_API_NS::ApzSubstate apzSubstate;
       ACS_CS_API_NS::AptSubstate aptSubstate;
       ACS_CS_API_NS::StateTransition stateTransition;
       ACS_CS_API_NS::BlockingInfo blockingInfo;
       ACS_CS_API_NS::CpCapacity cpCapacity;
       int dataSize;
};

struct ACS_CS_API_CpTableChange_R1 
{
       size_t dataSize;
      ACS_CS_API_CpTableData_R1* cpData;
};


#endif
