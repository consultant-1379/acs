/* 
 * File:   ACS_CS_NetConfCommand.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 16.57
 */

#ifndef ACS_CS_NETCONF_MAC_COMMAND_H
#define	ACS_CS_NETCONF_MAC_COMMAND_H

#include "ACS_CS_NetConfCommandInt.h"
#include "ACS_CS_NetConfParserInt.h"
#include "ACS_CS_NetConfSenderInt.h"

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class ACS_CS_NetConfCommand :public ACS_CS_NetConfCommandInt {

public:
    
    ACS_CS_NetConfCommand();
    virtual ~ACS_CS_NetConfCommand();
    
    virtual void setParser(ACS_CS_NetConfParserInt*);
    virtual void setSender(ACS_CS_NetConfSenderInt*);
    virtual int execute();
    virtual int executeUdp(std::string &udp_data);
    virtual void getResult(nresponse &);
    virtual void setFilter(string filter);
    virtual const char *getErrorMessage();
    virtual int getErrorType();
    virtual int getErrorSeverity();
    virtual const char* getErrorMessageInfo();
    

private:
     
    std::string xml_filter;
    
    const char *message_error;
    int  message_error_type;
    int  message_error_severity;
    const char *  message_error_info;

    ACS_CS_NetConfParserInt *par;
    ACS_CS_NetConfSenderInt *netSen;
    nresponse respon;
    
};


#endif	/* ACS_CS_NETCONF_MAC_COMMAND_H */

