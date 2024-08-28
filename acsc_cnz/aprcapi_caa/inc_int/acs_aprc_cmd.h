//*************************************************************************
//
//    COPYRIGHT Ericsson 2011.
//    All rights reserved.
//
//    The Copyright to the computer program(s) herein
//    is the property of Ericsson 2011.
//    The program(s) may be used and/or copied only with
//    the written permission from Ericsson 2011 or in
//    accordance with the terms and conditions stipulated in the
//    agreement/contract under which the program(s) have been
//    supplied.
//
//    *************************************************************************

#ifndef acs_aprc_cmd_h
#define acs_aprc_cmd_h


#include <ComCliApiCmdComp_7.h>
#include <ComMwSpiServiceIdentities_1.h>
#include <ComMgmtSpiComponent_1.h>
#include <ComMwSpiLog_1.h>
#include <ComMwSpiTrace_1.h>

#include <string>
#include <vector>


namespace apg
{

/**
* @brief Command component containing only one command: aprc,
* that displays a last executed action status.
*/
class aprc_cmd_comp
{
public:
    /**
     * Constructor.
     */
    aprc_cmd_comp( void );

    /**
     * Destructor.
     */
    virtual ~aprc_cmd_comp( void );

    static  aprc_cmd_comp& instance();

    /**
     * Command Component start function.
     *
     * @param reason - ComActivating or similar.
     *
     * @return ComOk, or one of the other common ComReturn codes.
     */
    ComReturnT start(ComStateChangeReasonT reason);

    /**
     * Command Component stop function.
     *
     * @param reason - ComDeactivating or similar.
     *
     * @return ComOk, or one of the other common ComReturn codes.
     */
    ComReturnT stop(ComStateChangeReasonT reason);

    /**
     * Called by CLI Agent (via C wrapper) when some user input is to be processed.
     *
     * @param context - session context structure passed in from
     *               CliAgent
     * @param action - the action to perform
     * @param actionId - action id
     * @param commandMatchCount - increased if a matching command is
     *                          found in this command component
     * @param text - the command string input from user
     *
     * @return ComOk, or one of the other common ComReturn error
     *         codes.
     */
    ComReturnT useModule(
        struct ComCliApiContext_6T *context,
        ComCliActionEnum_2T action,
        unsigned int  actionId,
        unsigned int  *commandMatchCount,
        const char    *text);

    const bool isTransactional(struct ComCliApiContext_6T *context);

    /**
     * Get this component's component interface. Used by LCM to
     * register this component to the portal.
     * @return component interface.
     */
    ComMgmtSpiComponent_1T& getComponent( void );

private:

    /** The command name */
    const std::string aprc_cmd_name;
    /** Prefix prepended to command output */
    const std::string aprc_cmd_output_prefix;


    /**
     * Component interface.
     */
    ComMgmtSpiComponent_1T _component;

    /**
     * CLI Interface of this command component.
     */
    //ComCliApiCmdComp_5T      interface;
      ComCliApiCmdComp_7T      interface;
      
    /**
     * Array of interfaces this component provides. Ref to this
     * command component's CLI interface is put in here.
     */
    ComMgmtSpiInterface_1T *ifArray [2];

    /**
     * Interfaces that this command component is dependent of. Ref
     * to log and trace interfaces are put in here.
     */
    ComMgmtSpiInterface_1T *depsArray [3];

}; // class aprc_cmd_comp
}
#endif
