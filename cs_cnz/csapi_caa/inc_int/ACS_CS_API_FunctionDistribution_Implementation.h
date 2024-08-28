//	Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_API_FunctionDistribution_Implementation_h
#define ACS_CS_API_FunctionDistribution_Implementation_h 1


#include "ACS_CS_API_FunctionDistribution_R1.h"

class ACS_CS_ImModel;

class ACS_CS_API_FunctionDistribution_Implementation : public ACS_CS_API_FunctionDistribution_R1
{

  public:

      ACS_CS_API_FunctionDistribution_Implementation();

      virtual ~ACS_CS_API_FunctionDistribution_Implementation();

      virtual ACS_CS_API_NS::CS_API_Result getFunctionNames (ACS_CS_API_NameList_R1 &functionList);

      virtual ACS_CS_API_NS::CS_API_Result getFunctionProviders (const ACS_CS_API_Name_R1 &name, ACS_CS_API_IdList_R1 &apList);

      virtual ACS_CS_API_NS::CS_API_Result getFunctionUsers (APID apid, const ACS_CS_API_Name_R1 &name, ACS_CS_API_IdList_R1 &cpList);

  private:

      bool getFunctionNameAsString(const ACS_CS_API_Name_R1& name, std::string& functionName);

      ACS_CS_ImModel* m_FuncDistModel;
};


#endif
