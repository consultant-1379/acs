#ifndef CSAPI_Subscription
#define CSAPI_Subscription

#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

class HWC_Observer : public ACS_CS_API_HWCTableObserver
{
    virtual void update (const ACS_CS_API_HWCTableChange_R1& observee);
};

class NE_Observer : public ACS_CS_API_NetworkElementObserver
{
    virtual void update (const ACS_CS_API_NetworkElementChange& observee);
};


class OgChange_Observer : public ACS_CS_API_OgObserver
{
    virtual void update (const ACS_CS_API_OgChange& observer);
};

class CpChange_Observer : public ACS_CS_API_CpTableObserver
{
    virtual void update (const ACS_CS_API_CpTableChange_R1& observer);
};

class OmProfileChange_Observer : public ACS_CS_API_OmProfilePhaseObserver
{
    virtual void update (const ACS_CS_API_OmProfileChange_R1& observer);
};

extern HWC_Observer *hwc;
extern NE_Observer *ne;
extern OgChange_Observer *og;
extern CpChange_Observer *cp;
extern OmProfileChange_Observer *om;

void cs_subscription_init();
void cs_subscription_cleanup();

std::string getIpAddressValue(uint32_t address);
uint32_t getIpAddressValue(const std::string &address);
std::string getFbnValue(uint16_t fbn);
std::string getDhcpValue(uint16_t dhcpOption);


#endif // CSAPI_Subscription
