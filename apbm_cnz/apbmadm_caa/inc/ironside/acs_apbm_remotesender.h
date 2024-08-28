/* 
 * File:   acs_apbm_remotesender.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 16.43
 */

#ifndef ACS_CS_IRONSIDE_REMOTE_SENDER_H
#define	ACS_CS_IRONSIDE_REMOTE_SENDER_H

#include "acs_apbm_sender.h"
#include "../acs_apbm_logger.h"
#include "../acs_apbm_programmacros.h"
class acs_apbm_remotesender : virtual public acs_apbm_sender{
public:
    acs_apbm_remotesender(char target_a[16], char target_b[16], unsigned = 5000);
    virtual ~acs_apbm_remotesender();

    const char* getErrorMessage();
    int         getErrorType();
    int         getErrorSeverity();
    const char* getErrorMessageInfo();

    int sendUdpMessage(std::string query); 
    std::string getUdpData();
private:
    std::string data;
    std::string udp_data;
    const char *message_error;
    int  message_error_type;
    int  message_error_severity;
    const char *  message_error_info;

    char _dmxc_addresses[2][16];
    unsigned _timeout_ms;
};

#endif	

