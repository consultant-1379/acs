/* 
 * File:   acs_apbm_nc_bladefilter.h
 * Author: renato
 *
 * Created on 10 ottobre 2012, 17.08
 */

#ifndef ACS_CS_NETCONFBLADEFILTER_H
#define	ACS_CS_NETCONFBLADEFILTER_H

#include "acs_apbm_nc_filter.h"

#include <string>
    
class acs_apbm_nc_bladefilter : public acs_apbm_nc_filter {
    
    
public:
    acs_apbm_nc_bladefilter();
    acs_apbm_nc_bladefilter(std::string mag,int slot);
    virtual ~acs_apbm_nc_bladefilter();

    int makeXmlgetFirmwareDataAct(std::string tenantId, std::string physicalAddress,std::string shelf_id, std::string slot,std::string &xml);
    int setBladeQuery(std::string tenantId, std::string physicalAddress, std::string slot_id, std::string &xml);
    int setBladeQuery(std::string tenantId, std::string physicalAddress, std::string shelf_id, std::string slot_id, std::string &xml);
    int setXmlGetLedInfo(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int get_blade_led_type(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int setXmlBiosRunMode(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int setXmlPowerState(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string pwr_status,std::string &xml);
    int setXmlFatalEventLog(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int setAdministrativeStateLocked(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int setAdministrativeStateUnlocked(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int setReset(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int setReset(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string resetType,std::string gracefulReset,std::string &xml);
    int getXmlPowerState(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
    int setBladeREDLed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status,std::string &xml);

    int setBladeBlueLed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status,std::string &xml);

    int setBladeMIALed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status,std::string &xml);

    int setBladeLedStatus(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_name,std::string led_status,std::string &xml);
    int getBladeUserLabel(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml);
private:

    int getBladeStructureData(DOMDocument **docOut,DOMElement **element, std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id);
//    int getBladeStructureData(DOMDocument **docOut,std::string physicAdd,std::string shelf_id,std::string slot_id);
//    int getBladeStructureData(std::string physicAdd,std::string shelf_id,std::string slot_id);

    int slot;
    std::string mag;
    
    int getXml(DOMDocument*d,std::string&);

};

#endif	/* ACS_CS_NETCONFBLADEFILTER_H */

