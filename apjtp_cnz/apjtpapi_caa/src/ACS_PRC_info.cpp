/*=================================================================== */
   /**
   @file ACS_PRC_info.cpp

   Class method implementation for getting the last modification time
   of ACS_PRC_Reboot file.

   This module contains the implementation of class declared in
   the ACS_PRC_info.h module

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/04/2010     XTANAGG        APG43 on Linux.
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ACS_PRC_info.h>
#include <acs_prc_api.h>


/*===================================================================
   ROUTINE: getSystemLifeId
=================================================================== */
unsigned char ACS_PRC_info::getSystemLifeId()
{

	//Commented in APG43L as it is causing a purging mechanism 
	
	//Get the last reboot time of the node from PRC method lastRebootTime.
	unsigned char ltime = 0;
	//Create an object of ACS_PRC_API.
	ACS_PRC_API prcObj;

	//Invoke the lastRebootTime method on the PRC object.
	ltime = (prcObj.lastRebootTime());
	return ltime;

}

