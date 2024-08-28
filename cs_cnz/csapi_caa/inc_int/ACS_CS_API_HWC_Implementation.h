#ifndef ACS_CS_API_HWC_Implementation_h
#define ACS_CS_API_HWC_Implementation_h 1


#include "ACS_CS_Protocol.h"
#include "ACS_CS_API_HWC_R1.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImIMMReader.h"

class ACS_CS_Table;



class ACS_CS_API_HWC_Implementation : public ACS_CS_API_HWC_R2  //## Inherits: <unnamed>%458A5487017B
{

  public:
      ACS_CS_API_HWC_Implementation();

      virtual ~ACS_CS_API_HWC_Implementation();

      virtual ACS_CS_API_NS::CS_API_Result getMagazine (uint32_t &magazine, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSlot (uint16_t &slot, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSysType (uint16_t &sysType, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSysNo (uint16_t &sysNo, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getFBN (uint16_t &fbn, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSide (uint16_t &side, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSeqNo (uint16_t &seqNo, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getIPEthA (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getIPEthB (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getAliasEthA (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getAliasEthB (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getAliasNetmaskEthA (uint32_t &mask, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getAliasNetmaskEthB (uint32_t &mask, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getDhcpMethod (uint16_t &dhcp, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSysId (uint16_t &sysId, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getBoardIds (ACS_CS_API_IdList_R1 &listObj, ACS_CS_API_BoardSearch_R1 *searchObj);

      virtual ACS_CS_API_NS::CS_API_Result getContainerPackage (std::string &container, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getBgciIPEthA (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getBgciIPEthB (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSolIPEthA (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSolIPEthB (uint32_t &ip, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getSoftwareVersionType (uint16_t &version, BoardID boardId);

      virtual ACS_CS_API_NS::CS_API_Result getUuid (std::string &uuid, BoardID boardId);

  private:

      ACS_CS_ImModel *model;
      ACS_CS_ImIMMReader * immReader;
      ACS_CS_API_NS::CS_API_Result hwcTableStatus;
};


#endif
