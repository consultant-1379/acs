
#ifndef acs_apbm_netconfDMXFUNCTIONSFILTER_H_
#define acs_apbm_netconfDMXFUNCTIONSFILTER_H_

#include "acs_apbm_nc_filter.h"

#include <string>
#include <vector>

typedef struct
{   std::string tagname;
	std::string tagval;
} xmtag;

class acs_apbm_nc_dmxfunctionsfilter : public acs_apbm_nc_filter {


public:

    acs_apbm_nc_dmxfunctionsfilter();
    acs_apbm_nc_dmxfunctionsfilter(std::string mag,int slot);
    virtual ~acs_apbm_nc_dmxfunctionsfilter();

    int setXmlSetSnmpOneSnmpTarget(std::string address1,std::string userLabel1,std::string &xml);
    int setXmlSetSnmpTwoSnmpTargets(std::string address1,std::string userLabel1,std::string address2,std::string userLabel2,std::string &xml);
    int setXmlSetAdministrativeStateLockBothSnmpTargets(std::string &xml);
    int setXmlSetAdministrativeStateLockSnmpTarget(std::string snmpTargetId,std::string &xml);
    int setXmlUpgradeJobs(std::string physicalAddress,std::string shelf_id,std::string slot_id,std::string &xml);
    int setXmlActUpgradeIpmiFirmware(std::string shelf_id,std::string slot_id,std::string ipAddress,std::string port,std::string username,std::string password,std::string filename,std::string &xml);
    int setXmlGetSnmpAddressAttribute(std::string &xml);
    //TO DO : New BSP Adapataions
    int getIpV4controlNetworkIds(std::string tenantId, std::string &xml);
    int getIpV4Connection(std::string ipV4NetworkId_str,std::string ipV4ConnectionId,std::string &xml);
    int createIpV4Connection(std::string ipV4NetworkId_str,std::string ipV4ConnectionId,std::string Ipaddr_str,std::string &xml);
    int removeIpV4Connection(std::string ipV4NetworkId_str,std::string ipV4ConnectionId,std::string &xml);
    int trapsubscription_all(std::string tenantId,std::string contrlNetworkId_str,std::string bgciManagerId_str,std::string &xml);
    int getBgciManagerStructure(DOMDocument **docOut,DOMElement **element,std::string tenantId,std::string controlNetworkId_str,std::string bgciManagerId_str);

    int getTenants(std::string &xml);

private:

    int setXmlGetSnmpAttribute(std::string attribute,std::string &xml);

    void createElement(DOMDocument *doc, DOMElement *prec,std::vector<xmtag> c);

    int slot;

    std::string mag;

    int getXml(DOMDocument*d,std::string&);

};

#endif /* acs_apbm_netconfDMXFUNCTIONSFILTER_H_ */

