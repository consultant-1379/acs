
#ifndef ACS_CS_NETCONF_BUILDER_COMMAND_H
#define	ACS_CS_NETCONF_BUILDER_COMMAND_H

#include "ACS_CS_NetConfBuilderInt.h"
#include "ACS_CS_NetConfCommandInt.h"



class ACS_CS_NetConfBuilderCommand : public ACS_CS_Netconf_Builder_int {


    public:
    
    ACS_CS_NetConfBuilderCommand()
    {
        
    }
    virtual ~ACS_CS_NetConfBuilderCommand();
    virtual ACS_CS_NetConfCommandInt* make(netconfBuilder::builds x);
 
private:

};


#endif	/* ACS_CS_NETCONF_MAC_BUILDER_H */

