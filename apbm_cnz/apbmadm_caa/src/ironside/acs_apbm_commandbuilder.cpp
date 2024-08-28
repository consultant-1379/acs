/* 
 * File:   acs_apbm_commandbuilder.cpp
 * Author: renato
 * 
 * Created on 9 ottobre 2012, 17.15
 */

#include "acs_apbm_commandbuilder.h"
#include "acs_apbm_ironsidecommand.h"

#include "acs_apbm_remotesender.h"
#include "acs_apbm_macros.h"

#include<stdarg.h>

acs_apbm_commandbuilder::acs_apbm_commandbuilder(char target_a[16], char target_b[16]):
_sender_timeout(0)
{
	::strncpy(_target_address[0], target_a, ACS_APBM_ARRAY_SIZE(_target_address[0]));
	::strncpy(_target_address[1], target_b, ACS_APBM_ARRAY_SIZE(_target_address[1]));
}

acs_apbm_commandbuilder::~acs_apbm_commandbuilder()
{
    
}

acs_apbm_command * acs_apbm_commandbuilder::make(ironsideBuilder::builds choice, const unsigned *sender_timeout)
{
    
	_sender_timeout = (sender_timeout)? *sender_timeout: 0;

    acs_apbm_command *coMac = 0;
    acs_apbm_sender  *sender = 0;
    
    switch (choice)
    {
    case ironsideBuilder::UDP:
	
    	coMac = new acs_apbm_ironsidecommand(_target_address[0],_target_address[1]);
        sender = new acs_apbm_remotesender(_target_address[0],_target_address[1],_sender_timeout);
        
	coMac->setSender(sender);
	
	break;
    }           
    return coMac;
}

