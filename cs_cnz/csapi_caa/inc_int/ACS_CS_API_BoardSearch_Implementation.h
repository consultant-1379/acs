
#ifndef ACS_CS_API_BoardSearch_Implementation_h
#define ACS_CS_API_BoardSearch_Implementation_h 1

#include <vector>
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_ImSearchableBlade.h"

class ACS_CS_Attribute;

class ACS_CS_API_BoardSearch_Implementation : public ACS_CS_API_BoardSearch_R2
{

  public:
      ACS_CS_API_BoardSearch_Implementation();

      virtual ~ACS_CS_API_BoardSearch_Implementation();

      virtual void setMagazine (uint32_t magazine);
      virtual void setSlot (uint16_t slot);
      virtual void setSysType (uint16_t sysType);
      virtual void setSysNo (uint16_t sysNo);
      virtual void setFBN (uint16_t fbn);
      virtual void setSide (uint16_t side);
      virtual void setSeqNo (uint16_t seqNo);
      virtual void setIPEthA (uint32_t address);
      virtual void setIPEthB (uint32_t address);
      virtual void setAliasEthA (uint32_t address);
      virtual void setAliasEthB (uint32_t address);
      virtual void setAliasNetmaskEthA (uint32_t mask);
      virtual void setAliasNetmaskEthB (uint32_t mask);
      virtual void setDhcpMethod (uint16_t method);
      virtual void setSysId (uint16_t sysId);
      virtual void setSwVerType (uint16_t swVerType);
      virtual void reset ();
      virtual void setUuid (const std::string & uuid);
      ACS_CS_Attribute* getAttribute (size_t number) const;
      int getAttributeVectorSize () const;

      virtual bool match(const ACS_CS_ImBase *object);
      //virtual bool isDefault();

  private:
      ACS_CS_API_BoardSearch_Implementation(const ACS_CS_API_BoardSearch_Implementation &right);
      ACS_CS_API_BoardSearch_Implementation & operator=(const ACS_CS_API_BoardSearch_Implementation &right);

  public:

      ACS_CS_ImSearchableBlade *searchableBlade;
};


#endif
