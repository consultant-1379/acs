
//      Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_API_HWC_R1_h
#define ACS_CS_API_HWC_R1_h 1

#include <stdint.h>
#include <stddef.h>


class ACS_CS_API_BoardSearch_R1;

namespace ACS_CS_API_HWC_NS
{
        using namespace ACS_CS_API_NS;  // Import global namespace

        // HWC Functional Board Name Identifiers
        enum HWC_FBN_Identifier
        {
                FBN_SCBRP =                     100,
                FBN_RPBIS =                     110,
                FBN_CPUB =                      200,
                FBN_MAUB =                      210,
                FBN_APUB =                      300,
                FBN_Disk =                      310,
                FBN_DVD =                       320,
                FBN_GEA =                       330,
                FBN_SCXB =                      340,
                FBN_IPTB =						350,
                FBN_EPB1 =						360,
                FBN_EvoET =						370,
                FBN_CMXB =						380,
                FBN_IPLB=						390,
                FBN_SMXB = 						400
        };

        // HWC System types
        enum HWC_SystemType_Identifier
        {
                SysType_BC =            0,
                SysType_CP =            1000,
                SysType_AP =            2000    
        };

        // HWC Side Identifiers
        enum HWC_Side_Identifier
        {
                Side_A =                        0,
                Side_B =                        1
        };

        // HWC DHCP Methods
        enum HWC_DHCP_Method_Identifier
        {
                DHCP_None =                     0,
                DHCP_Normal =           1,
                DHCP_Client =           2
        };
}


class ACS_CS_API_HWC_R1
{

  public:
      virtual ~ACS_CS_API_HWC_R1();


      static ACS_CS_API_BoardSearch_R1 * createBoardSearchInstance ();

      static void deleteBoardSearchInstance (ACS_CS_API_BoardSearch_R1 *instance);

      virtual ACS_CS_API_NS::CS_API_Result getMagazine (uint32_t &magazine, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getSlot (uint16_t &slot, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getSysType (uint16_t &sysType, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getSysNo (uint16_t &sysNo, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getFBN (uint16_t &fbn, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getSide (uint16_t &side, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getSeqNo (uint16_t &seqNo, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getIPEthA (uint32_t &ip, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getIPEthB (uint32_t &ip, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getAliasEthA (uint32_t &ip, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getAliasEthB (uint32_t &ip, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getAliasNetmaskEthA (uint32_t &mask, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getAliasNetmaskEthB (uint32_t &mask, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getDhcpMethod (uint16_t &dhcp, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getSysId (uint16_t &sysId, BoardID boardId) = 0;

      virtual ACS_CS_API_NS::CS_API_Result getBoardIds (ACS_CS_API_IdList_R1 &listObj, ACS_CS_API_BoardSearch_R1 *searchObj) = 0;


};

// Class ACS_CS_API_HWC_R1 

#endif
