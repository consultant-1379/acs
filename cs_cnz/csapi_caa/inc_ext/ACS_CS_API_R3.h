
//       Copyright Ericsson AB 2017. All rights reserved.


#ifndef ACS_CS_API_R3_h
#define ACS_CS_API_R3_h 1


class ACS_CS_API_CPGroup_R1;
class ACS_CS_API_FunctionDistribution_R1;
class ACS_CS_API_CP_R1;
class ACS_CS_API_HWC_R2;
class ACS_CS_API_NWT_R1;

class ACS_CS_API_Name_Implementation;
class ACS_CS_API_IdList_Implementation;
class ACS_CS_API_NameList_Implementation;
class ACS_CS_API_MacList_Implementation;







class ACS_CS_API_R3
{

  public:

      static ACS_CS_API_CP_R1 * createCPInstance ();

      static ACS_CS_API_CPGroup_R1 * createCPGroupInstance ();

      static ACS_CS_API_FunctionDistribution_R1 * createFunctionDistributionInstance ();

      static ACS_CS_API_HWC_R2 * createHWCInstance ();

      static ACS_CS_API_NWT_R1 * createNWInstance ();

      static void deleteCPInstance (ACS_CS_API_CP_R1 *instance);

      static void deleteCPGroupInstance (ACS_CS_API_CPGroup_R1 *instance);

      static void deleteFunctionDistributionInstance (ACS_CS_API_FunctionDistribution_R1 *instance);

      static void deleteHWCInstance (ACS_CS_API_HWC_R2 *instance);

      static void deleteNWInstance (ACS_CS_API_NWT_R1 *instance);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_API_R3();

    // Additional Private Declarations

    // Additional Implementation Declarations

};


// Class ACS_CS_API_R3



#endif
