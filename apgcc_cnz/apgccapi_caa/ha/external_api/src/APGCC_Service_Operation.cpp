
/*=================================================================== */
   /**
   @file APGCC_Service_Operation.cpp

   Class method implementation for APG common class API to stop & start APG applications.

   This module contains the implementation of class declared in
   the APGCC_Service_Operation.h 

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "APGCC_Service_Operation.h"
#include "syslog.h"

/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */

#define HADIR "/opt/ap/apos/bin"

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: Constructor
=================================================================== */
APGCC_ServiceAdminOps::APGCC_ServiceAdminOps(){

	log.Open("APGCC");
}


/*===================================================================
   ROUTINE: apgServiceStop
=================================================================== */

ACS_APGCC_ReturnType APGCC_ServiceAdminOps::apgServiceStop(const ACE_TCHAR* service_name){

	ACE_INT32 rc = 0;
	ACE_TCHAR command[cmd_length];
	ACE_TCHAR buffer[100];

	// stoping the service
	ACE_OS::memset(command, 0, cmd_length);
	ACE_OS::snprintf(command,cmd_length, HADIR"/apg_service_admin_operations.sh stop %s\n",service_name);
	rc = ACE_OS::system(command);
	if(!rc) {
		sprintf(buffer, "Service %s stopped successfully",service_name);	
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	else{
		syslog(LOG_ERR, "Service %s stop failed",service_name);
		sprintf(buffer, "Service %s stop failed",service_name);
		log.Write(buffer, LOG_LEVEL_ERROR);
		buffer[0]='\0';
		syslog(LOG_ERR,"Error, %d from system('%s')",rc,command);
		sprintf(buffer,"Error, %d from system('%s')",rc,command);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}

	return ACS_APGCC_SUCCESS;
}

/*===================================================================
   ROUTINE: apgServiceStart
=================================================================== */

ACS_APGCC_ReturnType APGCC_ServiceAdminOps::apgServiceStart(const ACE_TCHAR* service_name){

	ACE_INT32 rc = 0;
	ACE_TCHAR command[cmd_length];
	ACE_TCHAR buffer[100];

	// starting the service
	ACE_OS::memset(command,0,cmd_length);
	ACE_OS::snprintf(command,cmd_length,HADIR"/apg_service_admin_operations.sh start %s\n",service_name);
	rc = ACE_OS::system(command);
	if(!rc) {
		syslog(LOG_INFO,"Service %s started successfully",service_name);
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	else{
		syslog(LOG_ERR,"Service %s start failed",service_name);
		sprintf(buffer,"Service %s start failed",service_name);
		log.Write(buffer, LOG_LEVEL_ERROR);
		buffer[0]='\0';
		syslog(LOG_ERR,"Error, %d from system('%s')",rc,command);
		sprintf(buffer,"Error, %d from system('%s')",rc,command);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}
	return ACS_APGCC_SUCCESS;
}

/*===================================================================
   ROUTINE: apgPassiveNodeRestart
=================================================================== */

ACS_APGCC_ReturnType APGCC_ServiceAdminOps::apgPassiveNodeRestart(){
	ACE_INT32 rc = 0;
	ACE_TCHAR command[cmd_length];
	ACE_TCHAR buffer[100];


	// checking the status of the service before stopping
	ACE_OS::memset(command,0,cmd_length);
	ACE_OS::snprintf(command,cmd_length, HADIR"/apg_passive_node_reboot.sh\n");
	rc = ACE_OS::system(command);
	if(!rc) {
		syslog(LOG_INFO,"Passive node rebooted");
		log.Write(buffer, LOG_LEVEL_INFO);
	}
	else{
		syslog(LOG_ERR, "Passive node reboot failed");
		sprintf(buffer, "Passive node reboot failed");
		log.Write(buffer, LOG_LEVEL_ERROR);
		buffer[0]='\0';
		syslog(LOG_ERR, "Error, %d from system('%s')",rc,command);
		sprintf(buffer, "Error, %d from system('%s')",rc,command);
		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}
	return ACS_APGCC_SUCCESS;
}

//******************************************************************************

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

