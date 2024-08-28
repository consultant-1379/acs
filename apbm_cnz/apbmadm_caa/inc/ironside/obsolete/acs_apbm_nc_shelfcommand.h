#ifndef ACS_CS_NETCONFSHELFCOMMAND_H
#define	ACS_CS_NETCONFSHELFCOMMAND_H

#include "acs_apbm_nc_command.h"
#include "acs_apbm_nc_parser.h"
#include "acs_apbm_nc_sender.h"
#include "acs_apbm_nc_shelffilter.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class acs_apbm_nc_shelfcommand :public acs_apbm_nc_command {

public:
    
    acs_apbm_nc_shelfcommand();
    virtual ~acs_apbm_nc_shelfcommand();
    
    virtual void setParser(acs_apbm_nc_parser*);
    virtual void setSender(acs_apbm_nc_sender*);
    virtual int execute();
    virtual void getResult(nresponse &);
    virtual void setXmlFilter(string filter);
    virtual const char *getErrorMessage();
    virtual int getErrorType();
    virtual int getErrorSeverity();
    virtual const char* getErrorMessageInfo();
    
    int createShelfQuery(std::string shelfId,std::string userLabel,std::string rack,std::string shelfType,std::string position,std::string physicalAddress);
    int getListOfShelfs();
    
    virtual void setFilter(acs_apbm_nc_filter* filter);
    
private:
     
    std::string xml_filter;
    
    const char *message_error;
    int  message_error_type;
    int  message_error_severity;
    const char *  message_error_info;

    acs_apbm_nc_parser *_parser;
    acs_apbm_nc_sender *_sender;
    acs_apbm_nc_shelffilter *_shelf_filter;
    
    nresponse _response;
    
};


#endif	/* ACS_CS_NETCONF_MAC_COMMAND_H */

