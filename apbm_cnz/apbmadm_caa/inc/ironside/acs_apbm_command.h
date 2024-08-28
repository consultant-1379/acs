/* 
 * File:   acs_apbm_command.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 15.38
 */

#ifndef ACS_CS_IRONSIDE_COMMAND_INT_H
#define	ACS_CS_IRONSIDE_COMMAND_INT_H

#include <string>
#include <map>
#include "acs_apbm_sender.h" 

using namespace std;


class acs_apbm_command {
public:
    
    acs_apbm_command()
    {
        
    }
    
    virtual ~acs_apbm_command()
    {

    }

    virtual void setSender(acs_apbm_sender*) = 0;
    virtual const char *getErrorMessage() = 0;
    virtual int getErrorType() = 0;
    virtual int getErrorSeverity() = 0;
    virtual const char* getErrorMessageInfo() = 0;

};




#endif	/* ACS_CS_IRONSIDE_COMMAND_INT_H */

