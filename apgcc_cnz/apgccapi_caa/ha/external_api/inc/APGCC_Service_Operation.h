/*=================================================================== */
/**
   @file   APGCC_Service_Admin_APIs.h

   @brief Header file for APG Common Class API to stop & start APG services.

          This module contains all the declarations useful to
          APGCC_ServiceAdminOps class.
   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef APG_SERVICE_ADMIN_OPS_H_
#define APG_SERVICE_ADMIN_OPS_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "ACS_APGCC_AmfTypes.h"
#include "ace/ACE.h"
#include "ACS_TRA_Logging.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/* ==================================================================== */

/**
      @brief     The APGCC_ServiceAdminOps class is responsible for providing common APIs for 
	APG aplications to stop and start APG services.
*/


class APGCC_ServiceAdminOps{

/*=====================================================================
	                        PRIVATE DECLARATION SECTION
==================================================================== */
  	private:
/**
      @brief     The cmd_length data member is const and is being used to limit
		length of the command executed in each of the APIs of this class.
*/
		static const ACE_INT32 cmd_length = 100;
		ACS_TRA_Logging log;
		
/*=====================================================================
	                        PUBLIC DECLARATION SECTION
==================================================================== */

  	public:
		APGCC_ServiceAdminOps();
		~APGCC_ServiceAdminOps(){};

/*=================================================================== */
 /**
        @brief  This routine stops the given apg service/application on both APG nodes.

        @par    None

        @pre    None

        @pre    None

        @param  service_name
		Name of the APG application/service to be stopped.
		APG service should be named as per the APG CoreMW naming convention. 

        @return Returns success/failure of the APG application/service stop
		zero return value - Success
		nonzero return value - Failure

        @exception      None
   */
/*=================================================================== */
		ACS_APGCC_ReturnType apgServiceStop(const ACE_TCHAR* service_name);

/*=================================================================== */
 /**
        @brief  This routine starts the given apg service/application on both APG nodes.

        @par    None

        @pre    None

        @pre    None

        @param  service_name
		Name of the APG application/service to be started.
		APG service should be named as per the APG CoreMW naming convention. 

        @return Returns success/failure of the APG application/service start
		zero return value - Success
		nonzero return value - Failure

        @exception      None
   */
/*=================================================================== */
		ACS_APGCC_ReturnType apgServiceStart(const ACE_TCHAR* service_name);

/*=================================================================== */
 /**
        @brief  This routine reboots the passive APG node.

        @par    None

        @pre    None

        @pre    None

        @param  None

        @return Returns success/failure of the passive APG node reboot
		zero return value - Success
		nonzero return value - Failure

        @exception      None
   */
/*=================================================================== */
		ACS_APGCC_ReturnType apgPassiveNodeRestart();
};

//******************************************************************************

#endif /* APG_SERVICE_ADMIN_OPS_H_ */

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

