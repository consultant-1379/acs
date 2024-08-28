/* 
 * File:   acs_apbm_nc_bladecommand.h
 * Author: renato
 *
 * Created on 9 ottobre 2012,  16.57
 */

#ifndef ACS_CS_NETCONF_MAC_COMMAND_H
#define	ACS_CS_NETCONF_MAC_COMMAND_H

#include "acs_apbm_nc_command.h"
#include "acs_apbm_nc_parser.h"
#include "acs_apbm_nc_sender.h"
#include "acs_apbm_nc_bladefilter.h"
#include "acs_apbm_nc_commandbuilder.h"

#include <iostream>
#include <sstream>
#include <string>


        
class acs_apbm_nc_bladecommand :public acs_apbm_nc_command {

public:
    
    acs_apbm_nc_bladecommand(char target_a[16], char target_b[16]);
    virtual ~acs_apbm_nc_bladecommand();
    
    virtual void setParser(acs_apbm_nc_parser*);
    virtual void setSender(acs_apbm_nc_sender*);
    virtual int execute();
    virtual void getResult(nresponse &);
    virtual const char *getErrorMessage();
    virtual int getErrorType();
    virtual int getErrorSeverity();
    virtual const char* getErrorMessageInfo();
    
    virtual int setBladeQuery(std::string tenantId, std::string physicalAddress, std::string shelf_id, std::string slot_id);
    virtual int setFirmwareDataActQuery(std::string tenantId, std::string physicalAddress, std::string shelf_id, std::string slot_id);
    

    std::string getXml();
    int get_blade_led_type(std::string tenantId, std::string physicalAddress, std::string shelf_id, std::string slot_id);
    int setXmlGetLedInfo(std::string tenantId, std::string physicalAddress, std::string shelf_id, std::string slot_id);
    int setXmlBiosRunMode(std::string tenantId, std::string physicalAddress, std::string shelf_id, std::string slot_id);
    int setXmlPowerState(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id, std::string pwr_status);
    int setBladeREDLed(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id,std::string led_status);
    int setBladeBlueLed(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id,std::string led_status);
    int setBladeMIALed(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id, std::string led_status);
    int setBladeLedStatus(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id, std::string led_name, std::string led_status);
    int setXmlFatalEventLog(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id);
    int setAdministrativeStateLocked(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id);
    int setAdministrativeStateUnlocked(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id);
    int setReset(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id);

    int getXmlPowerState(std::string tenantId, std::string physicAdd, std::string shelf_id, std::string slot_id);
    int getBladeUserLabel(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id);

private:

    std::string getShelfidFromPhysicalAddress(std::string physicalAddress);
    std::string getShelfidFromPhysicalAddressLoop(std::string physicalAddress);
    virtual void setXmlFilter(std::string filter);
    void setFilter(acs_apbm_nc_filter* filter);
        
    std::string xml_filter;
    
    const char *message_error;
    int  message_error_type;
    int  message_error_severity;
    const char *  message_error_info;

    acs_apbm_nc_parser *_parser;
    acs_apbm_nc_sender *_sender;
    acs_apbm_nc_bladefilter *_blade_filter;
    
    nresponse _response;
    
    acs_apbm_nc_commandbuilder commandBuilder;
    xmltype::types xtype;
    
};


#endif	/* ACS_CS_NETCONF_MAC_COMMAND_H */

