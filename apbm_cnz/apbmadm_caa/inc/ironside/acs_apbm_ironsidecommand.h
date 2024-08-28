
#ifndef acs_apbm_ironsideCOMMAND_H_
#define acs_apbm_ironsideCOMMAND_H_

#include "acs_apbm_command.h"
#include "acs_apbm_sender.h"
#include "acs_apbm_commandbuilder.h"

#include <iostream>
#include <sstream>
#include <string>
#include <acs_apbm_programmacros.h>
using namespace std;

class acs_apbm_ironsidecommand :public acs_apbm_command {

public:

    acs_apbm_ironsidecommand(char target_a[16], char target_b[16]);
    virtual ~acs_apbm_ironsidecommand();

    virtual void setSender(acs_apbm_sender*);
    virtual const char *getErrorMessage();
    virtual int getErrorType();
    virtual int getErrorSeverity();
    virtual const char* getErrorMessageInfo();

    int executeUdp(std::string udp_filter,std::string &udp_data);

private:


    std::string xml_filter;

    const char *message_error;
    int  message_error_type;
    int  message_error_severity;
    const char *  message_error_info;

    acs_apbm_sender *_sender;

    acs_apbm_commandbuilder commandBuilder;

};


#endif /* acs_apbm_ironsideCOMMAND_H_ */

