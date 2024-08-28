//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%45869B920399.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%45869B920399.cm

//## begin module%45869B920399.cp preserve=no
//	� Copyright Ericsson AB 2007. All rights reserved.
//## end module%45869B920399.cp

//## Module: ACS_CS_API_R2%45869B920399; Package body
//## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
//## Source file: Z:\ntacs\cs_cnz\csapi_caa\src\ACS_CS_API_R2.cpp

//## begin module%45869B920399.additionalIncludes preserve=no
//## end module%45869B920399.additionalIncludes

//## begin module%45869B920399.includes preserve=yes

#include "ACS_CS_API_CP_Implementation.h"
#include "ACS_CS_API_CPGroup_Implementation.h"
#include "ACS_CS_API_FunctionDistribution_Implementation.h"
#include "ACS_CS_API_HWC_Implementation.h"

//## end module%45869B920399.includes

// ACS_CS_API_R2
#include "ACS_CS_API_R2.h"
//## begin module%45869B920399.declarations preserve=no
//## end module%45869B920399.declarations

//## begin module%45869B920399.additionalDeclarations preserve=yes
//## end module%45869B920399.additionalDeclarations


// Class ACS_CS_API_R2


//## Other Operations (implementation)
 ACS_CS_API_CP_R1 * ACS_CS_API_R2::createCPInstance ()
{
  //## begin ACS_CS_API_R2::createCPInstance%4582885F01D8.body preserve=yes

   return new ACS_CS_API_CP_Implementation();

  //## end ACS_CS_API_R2::createCPInstance%4582885F01D8.body
}

 ACS_CS_API_CPGroup_R1 * ACS_CS_API_R2::createCPGroupInstance ()
{
  //## begin ACS_CS_API_R2::createCPGroupInstance%4582886F00DE.body preserve=yes

   return new ACS_CS_API_CPGroup_Implementation();

  //## end ACS_CS_API_R2::createCPGroupInstance%4582886F00DE.body
}

 ACS_CS_API_FunctionDistribution_R1 * ACS_CS_API_R2::createFunctionDistributionInstance ()
{
  //## begin ACS_CS_API_R2::createFunctionDistributionInstance%4582887802A3.body preserve=yes

   return new ACS_CS_API_FunctionDistribution_Implementation();

  //## end ACS_CS_API_R2::createFunctionDistributionInstance%4582887802A3.body
}

 ACS_CS_API_HWC_R2 * ACS_CS_API_R2::createHWCInstance ()
{
  //## begin ACS_CS_API_R2::createHWCInstance%458288B10051.body preserve=yes

   return new ACS_CS_API_HWC_Implementation();

  //## end ACS_CS_API_R2::createHWCInstance%458288B10051.body
}


 void ACS_CS_API_R2::deleteCPInstance (ACS_CS_API_CP_R1 *instance)
{
  //## begin ACS_CS_API_R2::deleteCPInstance%4582A3CB00A0.body preserve=yes

   if (instance)
		delete instance;

  //## end ACS_CS_API_R2::deleteCPInstance%4582A3CB00A0.body
}

 void ACS_CS_API_R2::deleteCPGroupInstance (ACS_CS_API_CPGroup_R1 *instance)
{
  //## begin ACS_CS_API_R2::deleteCPGroupInstance%4582A3F700FD.body preserve=yes

   if (instance)
		delete instance;

  //## end ACS_CS_API_R2::deleteCPGroupInstance%4582A3F700FD.body
}

 void ACS_CS_API_R2::deleteFunctionDistributionInstance (ACS_CS_API_FunctionDistribution_R1 *instance)
{
  //## begin ACS_CS_API_R2::deleteFunctionDistributionInstance%4582A4050311.body preserve=yes

   if (instance)
		delete instance;

  //## end ACS_CS_API_R2::deleteFunctionDistributionInstance%4582A4050311.body
}

 void ACS_CS_API_R2::deleteHWCInstance (ACS_CS_API_HWC_R2 *instance)
{
  //## begin ACS_CS_API_R2::deleteHWCInstance%4582A4050320.body preserve=yes

   if (instance)
		delete instance;

  //## end ACS_CS_API_R2::deleteHWCInstance%4582A4050320.body
}



// Additional Declarations
  //## begin ACS_CS_API_R2%4582883200DE.declarations preserve=yes
  //## end ACS_CS_API_R2%4582883200DE.declarations

//## begin module%45869B920399.epilog preserve=yes
//## end module%45869B920399.epilog
