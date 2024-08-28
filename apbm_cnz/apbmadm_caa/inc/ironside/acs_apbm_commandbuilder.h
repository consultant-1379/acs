
#ifndef ACS_CS_IRONSIDE_BUILDER_COMMAND_H
#define	ACS_CS_IRONSIDE_BUILDER_COMMAND_H

#include "acs_apbm_builder.h"
#include "acs_apbm_command.h"



class acs_apbm_commandbuilder : public acs_apbm_builder {


    public:
    acs_apbm_commandbuilder(char target_a[16], char target_b[16]);

    virtual ~acs_apbm_commandbuilder();
    virtual acs_apbm_command* make(ironsideBuilder::builds x, const unsigned* = 0);
 
private:
    unsigned _sender_timeout;
    char _target_address[2][16];

};


#endif	/* ACS_CS_IRONSIDE_MAC_BUILDER_H */

