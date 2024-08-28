//******************************************************************************
// 
// .NAME
//    RegistryInformation - Sets Registry information.
// .LIBRARY 3C++
// .PAGENAME RegistryInformation
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2006.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  Defines the required interface for dealing with information in the registry.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2006-04-11 EAB/AZA/DG Dan Andersson

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef REGISTRYINFORMATION_H
#define REGISTRYINFORMATION_H

#include <winsock2.h>
#include <windows.h>

class RegistryInformation
{
public:
   enum Status
   {
      OK,
      NotOK
   };

   enum APConfig
   {
      NotFound,
      AP1,
      AP2,
      Unknown
   };
 
   // Configures the DHCP registry IgnoreBroadcastFlag key value.
   // If the key does not exist, it is created.
   // If a change is done to registry, it's logged.
   Status setIgnoreBroadcastFlag(DWORD const flagvalue, bool& isFlagChanged) const;

   // Configures the DHCP registry DhcpAlertPercentage key value.
   // If the key does not exist, it is created.
   // If a change is done to registry, it's logged.
   Status setDhcpAlertPercentage(DWORD const flagvalue, bool& isFlagChanged) const;

   // Configures the DHCP registry DhcpAlertCount key value.
   // If the key does not exist, it is created.
   // If a change is done to registry, it's logged.
   Status setDhcpAlertCount(DWORD const flagvalue, bool& isFlagChanged) const;

   // Reads the APCFG key value from registry
   APConfig getAPCfg() const;

private:
   // Configures a DWORD registry key
   Status setKeyValue(DWORD const theValue, TCHAR* theRootKey,
                      TCHAR* theKey, bool& isFlagChanged) const;

   // Generates an event in the event log.
   void sendEvent(const char* keyName, DWORD const flagValue) const;   

};

#endif