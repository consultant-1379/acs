#ifndef ACS_CS_API_NWT_BoardSearch_Implementation_h
#define ACS_CS_API_NWT_BoardSearch_Implementation_h 1

#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_NetworkTable_R1.h"

class ACS_CS_API_NWT_BoardSearch_Implementation : public ACS_CS_API_NWT_BoardSearch_R1, public ACS_CS_API_NetworkTable_R1
{

  public:
      ACS_CS_API_NWT_BoardSearch_Implementation();

      virtual ~ACS_CS_API_NWT_BoardSearch_Implementation();

      virtual void setNtwName (std::string networkName);
      virtual void setAdminState (std::string admnStat);
      virtual void setExtNetwID (std::string externalNetwId);
      virtual void setIntNetwID (std::string internalNetwId);
      virtual void setMAC (std::string macAddr);
      virtual void setInterfaceID (std::string intfaceId);
      virtual void setBoardID (BoardID brdId);
      virtual void reset ();

      virtual bool match(const ACS_CS_API_NetworkTable_R1 &tableRow);


};


#endif
