#include "ACS_CS_API_CP_Implementation.h"
#include "ACS_CS_API_CPGroup_Implementation.h"
#include "ACS_CS_API_FunctionDistribution_Implementation.h"
#include "ACS_CS_API_HWC_Implementation.h"
#include "ACS_CS_API_NWT_Implementation.h"


#include "ACS_CS_API_R3.h"


 ACS_CS_API_CP_R1 * ACS_CS_API_R3::createCPInstance ()
{
   return new ACS_CS_API_CP_Implementation();
}

 ACS_CS_API_CPGroup_R1 * ACS_CS_API_R3::createCPGroupInstance ()
{
   return new ACS_CS_API_CPGroup_Implementation();
}

 ACS_CS_API_FunctionDistribution_R1 * ACS_CS_API_R3::createFunctionDistributionInstance ()
{
   return new ACS_CS_API_FunctionDistribution_Implementation();
}

 ACS_CS_API_HWC_R2 * ACS_CS_API_R3::createHWCInstance ()
{
   return new ACS_CS_API_HWC_Implementation();
}

 ACS_CS_API_NWT_R1 * ACS_CS_API_R3::createNWInstance ()
{
   return new ACS_CS_API_NWT_Implementation();
}

 void ACS_CS_API_R3::deleteCPInstance (ACS_CS_API_CP_R1 *instance)
{
   if (instance)
		delete instance;
}

 void ACS_CS_API_R3::deleteCPGroupInstance (ACS_CS_API_CPGroup_R1 *instance)
{
   if (instance)
		delete instance;
}

 void ACS_CS_API_R3::deleteFunctionDistributionInstance (ACS_CS_API_FunctionDistribution_R1 *instance)
{
   if (instance)
		delete instance;
}

 void ACS_CS_API_R3::deleteHWCInstance (ACS_CS_API_HWC_R2 *instance)
{
   if (instance)
		delete instance;
}


 void ACS_CS_API_R3::deleteNWInstance (ACS_CS_API_NWT_R1 *instance)
{
 	if (instance)
 			delete instance;
}

