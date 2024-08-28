
#ifndef ACS_CS_API_CpData_R1_h
#define ACS_CS_API_CpData_R1_h 1

#include "ACS_CS_API_Common_R1.h"

struct ACS_CS_API_CpData_R1 
{
      CPID id;
      ACS_CS_API_NS::CpState state;
      ACS_CS_API_NS::ApplicationId applicationId;
      ACS_CS_API_NS::ApzSubstate apzSubstate;
      ACS_CS_API_NS::StateTransition stateTransition;
      ACS_CS_API_NS::AptSubstate aptSubstate;
      ACS_CS_API_NS::BlockingInfo blockingInfo;
      ACS_CS_API_NS::CpCapacity cpCapacity;
};

#endif
