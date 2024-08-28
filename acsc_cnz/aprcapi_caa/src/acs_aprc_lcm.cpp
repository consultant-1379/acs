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

#include "acs_aprc_lcm.h"
#include "acs_aprc_cmd.h"
#include <syslog.h>

#include <iostream>


using namespace apg;

static acs_aprc_lcm* com_lib = 0;

acs_aprc_lcm::acs_aprc_lcm()
{
    portal = NULL;
}

acs_aprc_lcm::~acs_aprc_lcm()
{
}

acs_aprc_lcm& acs_aprc_lcm::instance() {

    if ( ! com_lib ) {
        com_lib = new acs_aprc_lcm;
    }
    return *com_lib;
}


/**
 * C Wrapper for Library Component Manager init function.
 * Declared in ComMgmtSpiLibraryComponentManager_1.h.
 *
 * @param accessor COM Interface Portal accessor
 * @param config   Configuration (not used in this LCM)
 *
 * @return ComOk, or one of the other common ComReturn codes.
 */
ComReturnT comLCMinit(struct ComMgmtSpiInterfacePortalAccessor *accessor, const char * config)
{
    return acs_aprc_lcm::instance().comLCMinit(accessor, config);
}

/**
 * C Wrapper for Library Component Manager terminate function.
 * Declared in ComMgmtSpiLibraryComponentManager_1.h.
 */
void comLCMterminate ()
{
    acs_aprc_lcm::instance().comLCMterminate();
}

/**
 * Gets interface portal from this LCM.
 *
 * @return COM Interface portal pointer.
 */
ComMgmtSpiInterfacePortal_1T* acs_aprc_lcm::getPortal()
{
    return portal;
}

/**
 * Library Component Manager init function.
 *
 * @param accessor COM Interface Portal accessor
 * @param config   Configuration (not used in this LCM)
 *
 * @return ComOk, or one of the other common ComReturn codes.
 */
ComReturnT acs_aprc_lcm::comLCMinit(struct ComMgmtSpiInterfacePortalAccessor *accessor, const char *config)
{
    (void) config;
    ComReturnT retval = ComOk;

    portal = (ComMgmtSpiInterfacePortal_1T*)(accessor->getPortal("1"));

    aprc_cmd_comp& component1 = aprc_cmd_comp::instance();
    retval = portal->registerComponent(&(component1.getComponent()));
    if ( retval != ComOk ) {
        syslog(LOG_INFO, "ACS_APRC: Error: aprc cli command module failed to register itself"); 
        //std::cerr << __func__ << ":" << __LINE__
        //          << "  Error: aprc cli command module failed to register itself"
        //          << std::endl;
       
        return retval;
    }
    return retval;
}

/** Library Component Manager terminate function. */
void acs_aprc_lcm::comLCMterminate ()
{
    ComReturnT retval = ComOk;
    if (portal != NULL) {
        aprc_cmd_comp& component1 = aprc_cmd_comp::instance();
        retval = portal->unregisterComponent(&(component1.getComponent()));
    }
}

