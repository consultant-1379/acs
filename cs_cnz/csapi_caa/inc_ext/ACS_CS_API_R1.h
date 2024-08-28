
//       Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_API_R1_h
#define ACS_CS_API_R1_h 1


class ACS_CS_API_CPGroup_R1;
class ACS_CS_API_FunctionDistribution_R1;
class ACS_CS_API_CP_R1;
class ACS_CS_API_HWC_R1;

class ACS_CS_API_Name_Implementation;
class ACS_CS_API_IdList_Implementation;
class ACS_CS_API_NameList_Implementation;






class ACS_CS_API_R1
{

  public:

      static ACS_CS_API_CP_R1 * createCPInstance ();

      static ACS_CS_API_CPGroup_R1 * createCPGroupInstance ();

      static ACS_CS_API_FunctionDistribution_R1 * createFunctionDistributionInstance ();

      static ACS_CS_API_HWC_R1 * createHWCInstance ();

      static void deleteCPInstance (ACS_CS_API_CP_R1 *instance);

      static void deleteCPGroupInstance (ACS_CS_API_CPGroup_R1 *instance);

      static void deleteFunctionDistributionInstance (ACS_CS_API_FunctionDistribution_R1 *instance);

      static void deleteHWCInstance (ACS_CS_API_HWC_R1 *instance);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_API_R1();

    // Additional Private Declarations

    // Additional Implementation Declarations

};


// Class ACS_CS_API_R1 



#endif
