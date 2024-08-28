/* 
 * File:   ACS_CS_NetConfRemoteSender.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 16.43
 */

#ifndef ACS_CS_NETCONF_REMOTE_SENDER_H
#define	ACS_CS_NETCONF_REMOTE_SENDER_H

#include "ACS_CS_NetConfSenderInt.h"


class ACS_CS_NetConfRemoteSender : virtual public ACS_CS_NetConfSenderInt{
public:
    ACS_CS_NetConfRemoteSender();
    virtual ~ACS_CS_NetConfRemoteSender();
    virtual int sendNetconfMessage(std::string xml_filter);
    virtual int sendUdpMessage(std::string query);
    std::string getUdpData();
    std::string getData();
    
    const char* getErrorMessage();
    int         getErrorType();
    int         getErrorSeverity();
    const char* getErrorMessageInfo();
    
private:
    std::string data;
    std::string udp_data;
    const char *message_error;
    int  message_error_type;
    int  message_error_severity;
    const char *  message_error_info;
    
    const char *ipaddress;
    int  port;
};

#endif	

