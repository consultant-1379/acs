/* 
 * File:   ACS_CS_NetConfCommandInt.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 15.38
 */

#ifndef ACS_CS_NETCONF_COMMAND_INT_H
#define	ACS_CS_NETCONF_COMMAND_INT_H


#include "ACS_CS_NetConfParserInt.h"
#include "ACS_CS_NetConfSenderInt.h"

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLChar.hpp>

typedef std::map <std::string,std::string> nresponse; 

using namespace std;

class ACS_CS_NetConfCommandInt {
public:
    
    ACS_CS_NetConfCommandInt()
    {
        
    }
    
    virtual ~ACS_CS_NetConfCommandInt()
    {

    }
    
    virtual void setFilter(std::string filter)=0;
    virtual void setParser(ACS_CS_NetConfParserInt*)=0;
    virtual void setSender(ACS_CS_NetConfSenderInt*)=0;
    virtual int execute()=0;
    virtual int executeUdp(std::string &udp_data)=0;
    virtual void getResult(nresponse &)=0;
    virtual const char *getErrorMessage()=0;
    virtual int getErrorType()=0;
    virtual int getErrorSeverity()=0;
    virtual const char* getErrorMessageInfo()=0;
    
private:
    


};




#endif	/* ACS_CS_NETCONF_COMMAND_INT_H */

