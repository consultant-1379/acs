/*
 * File:   ACS_CS_NetConfBladeFilter.h
 * Author: renato
 *
 * Created on 10 ottobre 2012, 17.08
 */

#ifndef ACS_CS_NETCONFBLADEFILTER_H
#define ACS_CS_NETCONFBLADEFILTER_H

#include "ACS_CS_NetConfFilterInt.h"



class ACS_CS_NetConfBladeFilter : ACS_CS_NetConfFilterInt {


public:
    ACS_CS_NetConfBladeFilter();
    virtual ~ACS_CS_NetConfBladeFilter();
    int createBladeQuery(std::string shelf_id,std::string blade_id,std::string slot_id,std::string productname,std::string productnumber,std::string productrevisionstate,std::string firstmacaddress,std::string &xml);
    int createMultipleQueryBlade(std::string bladeid1,std::string bladeid2,std::string productname,std::string productnumber,std::string productrevisionstate,std::string firstmacaddress,std::string &xml);
    int createShelfQuery(std::string shelfId,std::string userLabel,std::string rack,std::string shelfType,std::string position,std::string physicalAddress,std::string &xml);
    int createMacBladeQuery(std::string vEqm_id, std::string blade_id,std::string productname,std::string productnumber,std::string productrevisionstate,std::string firstmacaddress,std::string &xml);

    int createBladeLabelQuery(std::string vEqm_id, std::string blade_id,std::string &xml);
    int createTenantQuery(std::string &xml);

private:

    int getXml(DOMDocument*d,std::string&);



};

#endif  /* ACS_CS_NETCONFBLADEFILTER_H */


