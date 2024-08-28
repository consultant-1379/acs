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


#include "acs_aprc_cmd.h"
#include "acs_aprc_lcm.h"
#include <syslog.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <cstring>

using namespace apg;

static aprc_cmd_comp* cmd_comp = 0;

/**
 * The interface id of this command component.
 *
 * It is vital that the componentName, in this case "ComCliaprc_cmd_comp" matches
 * the name in the XML config file for the linked library, in this example the linked library
 * config file "libcli_example1.cfg" contains the XML tag "<name>ComCliaprc_cmd_comp</name>"
 *
 * The same applies for the interfaceVersion "1", it has to match the XML tag "<version>1</version>"
 * in "libcli_example1.cfg"
 *
 * Our component is implementing the "ComMgmtSpiComponent" interface and it is published here
 *
 */
const ComMgmtSpiInterface_1T APG_CMD_COMP_ID = {"ApgAprcCmdComp1","ComMgmtSpiComponent","1"};

// Static methods to use as function pointers for the interface
extern "C" {
    /**
     * C Wrapper for this Command Component's start function.
     *
     * @param reason - ComActivating or similar.
     *
     * @return ComOk, or one of the other common ComReturn codes.
     */
    static ComReturnT _cmdComp1_start(ComStateChangeReasonT reason  );

    /**
     * C Wrapper for this Command Component's stop function.
     *
     * @param reason - ComDeactivating or similar.
     *
     * @return ComOk, or one of the other common ComReturn codes.
     */
    static ComReturnT _cmdComp1_stop( ComStateChangeReasonT reason );

    /**
     * This Command Component's useModule. Called from the CliAgent whenever a command string needs to be processed by this command component.
     *
     * @param context  Contains call-back functions to CliAgent etc.
     * @param action   Execute, Auto-complete, Terminate
     * @param actionId Id for this action
     * @param commandMatchCount
     *                 Increased if a matching command is found.
     * @param text     The command string entered by user.
     *
     * @return ComOk, or one of the other common ComReturn codes.
     */
    static ComReturnT _cmdComp1_useModule(
        struct ComCliApiContext_6T *context,
        ComCliActionEnum_2T action,
        unsigned int  actionId,
        unsigned int  *commandMatchCount,
        const char    *text,
        const char    *trigger);

   
    static const bool _cmdComp1_isTransactional(struct ComCliApiContext_6T *context);

    static const char* _cmdComp1_getGroupName(void);

}


aprc_cmd_comp& aprc_cmd_comp::instance() 
{
    if ( ! cmd_comp ) {
        cmd_comp = new aprc_cmd_comp;
    }

    return *cmd_comp;
}


ComReturnT aprc_cmd_comp::start(ComStateChangeReasonT reason)
{
    (void) reason;
    ComMwSpiLog_1T                    * _logIf;
    ComMwSpiTrace_1T                  * _traceIf;
    ComReturnT retval = ComOk;
    ComMgmtSpiInterfacePortal_1T* _portal = acs_aprc_lcm::instance().getPortal();

    if (!_portal) {
        return ComNotExist;
    }

    /* Obtaining references to the services we need */
    retval = _portal->getInterface( ComMwSpiLog_1Id, (ComMgmtSpiInterface_1T**) &_logIf );
    if (ComOk != retval)
    {
        return retval;
    }

    retval = _portal->getInterface( ComMwSpiTrace_1Id, (ComMgmtSpiInterface_1T**) &_traceIf );
    if (ComOk != retval)
    {
        return retval;
    }

    //Set log and trace interfaces in logging wrapper
    if (_logIf != NULL) {
        // ComCliUtilities::CmdCompLoggingWrapper::instance().set_log_spi( (ComMgmtSpiInterface_1T*) _logIf );
    }

    if (_traceIf != NULL) {
        // ComCliUtilities::CmdCompLoggingWrapper::instance().set_trace_spi( (ComMgmtSpiInterface_1T*) _traceIf );
    }
    return retval;
}



ComReturnT aprc_cmd_comp::stop(ComStateChangeReasonT reason)
{
   (void) reason;
    // CMD_COMP_ENTER_ARGS("Reason: %d", (int)reason );
    // CMD_COMP_INFO("Stopping aprc_cmd_comp.");

    ComReturnT retval = ComOk;

    // CMD_COMP_LEAVE_ARGS( "Retval: %d", (int)retval );
    return retval;
}



const bool aprc_cmd_comp::isTransactional(struct ComCliApiContext_6T *context)
{
	return false;
}

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
ComReturnT aprc_cmd_comp::useModule(
    struct ComCliApiContext_6T* context,
    ComCliActionEnum_2T action,
    unsigned int  actionId,
    unsigned int  *commandMatchCount,
    const char    *text)
{
    // CMD_COMP_ENTER();
    ComReturnT rc = ComOk;
    std::vector<std::string> output;

    switch (action) {
    case ComCliActionEnumTerminate_2:
        rc = ComNotExist;
        break;

    case ComCliActionEnumExecute_2:
        if( aprc_cmd_name.compare( text ) == 0 ) {
            std::stringstream out;
            out << aprc_cmd_output_prefix;
            int status = context->lastStatus;
            if (status == 0)
            out << "OK";
            else
            out << "Not OK";
            output.push_back(out.str());
            (*commandMatchCount)++;
            // INFO_OUTPUT("aprc command executed.");
        } else {
            rc = ComNotExist;
        }
        
        break;

    case ComCliActionEnumHelp_2:
    case ComCliActionEnumAutocomplete_2: {

        std::string txt(text);
        bool match = (txt.size() > 0);
        // TR HQ56327 - BEGIN
        if( aprc_cmd_name.compare( text ) == 0 ) 
        {
        if ( match ) 
        {
            rc = context->setAutocompleteResult(context, 0,
                                                std::string("aprc").size(),
                                                actionId, ComCliCompletionEnumComplete,
                                                aprc_cmd_name.c_str(),
                                                "Display status of previous command.");
        }
        }
        else 
        {
           rc = ComNotExist;
        }
         // TR HQ56327 - END

    }
        
        break;

    case ComCliActionEnumInit_2:
    	 break;

    default:
        rc = ComNotExist;
    }

    for (unsigned i = 0; i < output.size(); i++) {
        context->setOutput(context, 0, output[i].size(), actionId, output[i].c_str());
    }

    // CMD_COMP_LEAVE();
    return rc;
}



aprc_cmd_comp::aprc_cmd_comp( void ) :
    aprc_cmd_name("aprc"), aprc_cmd_output_prefix("")
{
    // CLI Interface
    interface.base.componentName = APG_CMD_COMP_ID.componentName;
    interface.base.interfaceName =  ComCliApiCmdCompIf_7Id.interfaceName;
    interface.base.interfaceVersion = ComCliApiCmdCompIf_7Id.interfaceVersion;
    interface.useModule = &_cmdComp1_useModule;
    interface.getGroupName = &_cmdComp1_getGroupName;
    // To Fix HQ94057
    interface.isTransactional = &_cmdComp1_isTransactional;


    // Interface array
    ifArray[0] = (ComMgmtSpiInterface_1T*) &interface;
    ifArray[1] = 0;

    // Dependency array, this component will use the log and trace interface from the portal
    depsArray[0] = (ComMgmtSpiInterface_1T*)&ComMwSpiLog_1Id;
    depsArray[1] = (ComMgmtSpiInterface_1T*)&ComMwSpiTrace_1Id;
    depsArray[2] = 0;

    // Component structure
    _component.base = APG_CMD_COMP_ID;
    _component.interfaceArray = ifArray;
    _component.dependencyArray = depsArray;
    _component.start = &_cmdComp1_start;
    _component.stop = &_cmdComp1_stop;
}

aprc_cmd_comp::~aprc_cmd_comp( void )
{
}

/**
 *  Get this component's component interface. Used by LCM to
 *  register this component to the portal.
 * @return component interface.
 */
ComMgmtSpiComponent_1T& aprc_cmd_comp::getComponent( void )
{
    return _component;
}


//C Wrappers for start/stop/usemodule functions
extern "C" ComReturnT _cmdComp1_start(ComStateChangeReasonT reason )
{
    return aprc_cmd_comp::instance().start(reason);
}


extern "C" ComReturnT _cmdComp1_stop(ComStateChangeReasonT reason )
{
    return aprc_cmd_comp::instance().stop(reason);
}

extern "C"
ComReturnT _cmdComp1_useModule(
    struct ComCliApiContext_6T *pcontext,
    ComCliActionEnum_2T action,
    unsigned int  actionId,
    unsigned int  *commandMatchCount,
    const char    *text,
    const char    *trigger)
{
    return aprc_cmd_comp::instance().useModule(pcontext, action, actionId, commandMatchCount, text);
}

// To Fix HQ94057
extern "C"
const bool _cmdComp1_isTransactional(struct ComCliApiContext_6T *context)
{
  return aprc_cmd_comp::instance().isTransactional(context);
}

extern "C"
const char* _cmdComp1_getGroupName(void) 
{
    return "ComBasicCommands";
}



