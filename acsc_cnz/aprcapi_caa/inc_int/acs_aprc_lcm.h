/*=================================================================== */
/**
   @file   acs_aprc_lcm.h

   @brief  Header file for APRC type module.

          This module contains all the declarations useful to
          specify the class aprc_lcm.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/08/2012     XRAMMAT        Initial version.
==================================================================== */

#ifndef acs_aprc_lcm_h
#define acs_aprc_lcm_h

#include <ComMgmtSpiLibraryComponentManager_1.h>
#include <ComMgmtSpiInterfacePortal_1.h>

namespace apg
{

/**
* @brief Library Component Manager for this library. Registers
*        aprc command component to the COM Interface portal:
*        aprc.
*/
class acs_aprc_lcm
{
public:

    /** Constructor */
    acs_aprc_lcm();

    /** Destructor */
    virtual ~acs_aprc_lcm();

    static acs_aprc_lcm& instance();

    /**
    * Library Component Manager init function.
    *
    * @param accessor COM Interface Portal accessor
    * @param config   Configuration (not used in this LCM)
    *
    * @return ComOk, or one of the other common ComReturn codes.
    */
    ComReturnT comLCMinit(struct ComMgmtSpiInterfacePortalAccessor* accessor, const char* config);

    /**
    * Library Component Manager terminate function.
    */
    void comLCMterminate();

    /**
    * Gets interface portal from this LCM.
    *
    * @return COM Interface portal pointer.
    */
    ComMgmtSpiInterfacePortal_1T* getPortal();

private:
    /**
     * Pointer to COM Interface portal.
     */
    ComMgmtSpiInterfacePortal_1T* portal;

};
}

#endif

