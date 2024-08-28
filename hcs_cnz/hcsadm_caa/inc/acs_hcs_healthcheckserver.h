//======================================================================
//
// NAME
//      HealthCheckServer.h
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      Server process for Health Check Service in APZ.
//      Based on 190 89-CAA xxxxx.
//
// DOCUMENT NO
//      190 89-CAA xxxxx
//
// AUTHOR
//      2011-04-30 by EGINSAN
// CHANGES
//     
//======================================================================

#ifndef HealthCheckServer_h
#define HealthCheckServer_h 1

#define HCS_MAX_BUFSIZE 512

#include "acs_hcs_healthcheckservice.h"


namespace AcsHcs {

/** Class: HealthCheckServer 
 *	This class is responsible for handling Windows HCS service toward 
 *  Windows service control manager (SCM).
 */
class HealthCheckServer 
{

  public:
      // Constructor
      HealthCheckServer();

      // Destructor
      virtual ~HealthCheckServer();

	  // Install HCS service
      void Install();

	  // Remove HCS service
      void Remove();

	  // Start HCS service
      static void /*WINAPI*/ StartService();

	  // Start HCS service 
      void DebugService();

	  /**
	  * This function gets the service name.
	  * @return The name of the service.
	  */
	  inline char* GetServiceName();

	  /**
	  * This function sets the service name.
	  * @param[in] serviceName_ The name of the service.
	  */
	  inline void SetServiceName(char* serviceName_);

	  /**
	  * This function gets the display name of the service.
	  * @return The display name of the service.
	  */
	  inline char* GetServiceDisplayName();

	  /**
	  * This function sets the service display name.
	  * @param[in] serviceDisplayName_ The display name of the service.
	  */
	  inline void SetServiceDisplayName(char* serviceDisplayName_);

	  /**
	  * This function gets the execution mode of the service.
	  * @return true if the service is executing in debug mode, false otherwise.
	  */
      inline bool GetDebugMode();

	  /**
	  * This function sets the execution mode of the service.
	  * @param[in] debugMode_ true if debug mode, false otherwise.
	  */
      inline void SetDebugMode(bool debugMode_ = false);


  private: 

	  char* serviceName_;

	  char* serviceDisplayName_;

	  bool debugMode_;

	  // The Service provided by this Server
	  static HealthCheckService theService_;

	  // Send service status to SCM
      static void SendStatus(ACE_UINT32 currstate);

	  // Server Control Handler routine
	  //static void WINAPI ServerCtrlHandler(ACE_UINT32 opcode);
	  static void ServerCtrlHandler(ACE_UINT32 opcode);

	  // Console Handler routine
      //static bool WINAPI ConsoleHandler (ACE_UINT32 controlCode);
	  static bool ConsoleHandler (ACE_UINT32 controlCode);

};


}

#endif