
//      Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_API_FunctionDistribution_R1_h
#define ACS_CS_API_FunctionDistribution_R1_h 1



#include "ACS_CS_API_Common_R1.h"


class ACS_CS_API_FunctionDistribution_R1
{

  public:
      virtual ~ACS_CS_API_FunctionDistribution_R1();


      virtual ACS_CS_API_NS::CS_API_Result getFunctionNames (ACS_CS_API_NameList_R1 &functionList) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getFunctionProviders (const ACS_CS_API_Name_R1 &name, ACS_CS_API_IdList_R1 &apList) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getFunctionUsers (APID apid, const ACS_CS_API_Name_R1 &name, ACS_CS_API_IdList_R1 &cpList) = 0;

};


// Class ACS_CS_API_FunctionDistribution_R1 

#endif
