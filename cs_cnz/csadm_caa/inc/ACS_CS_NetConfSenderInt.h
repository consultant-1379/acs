/* 
 * File:   ACS_CS_Netconf_Sender.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 15.39
 */

#ifndef ACS_CS_NETCONF_SENDER_INT_H
#define	ACS_CS_NETCONF_SENDER_INT_H

#include <string>

class ACS_CS_NetConfSenderInt{
public:
    
    ACS_CS_NetConfSenderInt()
    {
        
    }
    
    virtual ~ACS_CS_NetConfSenderInt()
    {
        
    }
    
    virtual int sendNetconfMessage(std::string xml_filter)=0;
    virtual int sendUdpMessage(std::string query)=0;
    virtual const char* getErrorMessage()=0;
    virtual int         getErrorType()=0;
    virtual int         getErrorSeverity()=0;
    virtual const char* getErrorMessageInfo()=0;
    virtual std::string getData()=0;
    virtual std::string getUdpData()=0;

    
protected:
 
    std::string xml_filter;
 
};

#endif	/* ACS_CS_NETCONF_SENDER_H */

