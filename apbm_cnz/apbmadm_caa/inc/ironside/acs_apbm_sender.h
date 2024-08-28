/* 
 * File:   ACS_CS_Ironside_Sender.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 15.39
 */

#ifndef ACS_CS_IRONSIDE_SENDER_INT_H
#define	ACS_CS_IRONSIDE_SENDER_INT_H

#include <string>

class acs_apbm_sender{
public:
    
    acs_apbm_sender()
    {
        
    }
    
    virtual ~acs_apbm_sender()
    {
        
    }
     
    virtual int sendUdpMessage(std::string xml_filter)=0;
    virtual const char* getErrorMessage()=0;
    virtual int         getErrorType()=0;
    virtual int         getErrorSeverity()=0;
    virtual const char* getErrorMessageInfo()=0;
    virtual std::string getUdpData()=0;

protected:
 
    std::string xml_filter;
 
};

#endif	/* ACS_CS_IRONSIDE_SENDER_H */

