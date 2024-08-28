//******************************************************************************
// 
// .NAME
//    DHCPConfiguration - Provides access to the DHCP server using the netsh
//                        command.
// .LIBRARY 3C++
// .PAGENAME <DHCPConfiguration>
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2005.
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
//  The DHCPConfiguration class is supposed to be used when accessing the DHCP
//  server. Accesses to the DHCPServerAccess's methods are thread safe hence
//  guarantee single thread operation towards the DHCP server independent of
//  the number of instances created from the DHCPConfiguration class.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-26 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef DHCPCONFIGURATION_H
#define DHCPCONFIGURATION_H

#include "PlatformTypes.hxx"
#include "Scope.H"
#include "ClientInformation.H"
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

// Member functions, constructors, destructors, operators
class DHCPConfiguration
{
public:
   enum Status
   {
      OK,
      EndOfData,
      Failure,
      Match
   };

   enum DeleteScopeForceFlag
   {
      DhcpFullForce,
      DhcpNoForce
   };

   enum ScopeState
   {
      Deactivate,
      Activate
   };

   enum DeleteClientOption
   {
      Force,
      DeleteIfNoMatch
   };

   enum Operation
   {
      Start,
      Continue
   };

   enum DHCPType
   {
      DHCPOnly,
      BOOTPOnly,
      DHCPAndBOOTP
   };

   // Constructor.
   DHCPConfiguration(u_int32 DHCPAddress, u_int32 netmask);

   // Updates the provided client information with the associated boot image
   // filename.
   virtual Status getBootfileInformation(ClientInformation& client);

   // Checks whether a client's bootfile name exists in the DHCP tables
   virtual Status existsClientsBootfile(ClientInformation& client, bool& existsIt);

   // Change the boot filename for a specific client.
   virtual Status changeBootFilename(const ClientInformation* client, u_int16 filenameLength, const char* filename);

   // Checks if a scope exist in the DHCP server.
   virtual Status existsScope(DWORD scope, bool &existsIt);

   // Checks if an IP range exist in a scope in the DHCP server.
   virtual Status existsIPRange(DWORD scope, DWORD range, bool &existsIt);

   // Checks for an entry in the DHCP server's tables if it matches the supplied
   // client IP address, scope and Identifier.
   virtual Status existsClient(const ClientInformation* clientInfo, bool &existsIt);

   // Deletes the primary and scondary scopes in the DHCP server's tables
   virtual Status deletePrimaryScope(DeleteScopeForceFlag deleteFlag) 
   {
      return deleteScope(&m_primaryScope, deleteFlag);
   }
   virtual Status deleteSecondaryScope(DeleteScopeForceFlag deleteFlag)
   {
      return deleteScope(&m_secondaryScope, deleteFlag);
   }

   // Adds the primary and secondary scopes in the DHCP server. 
   // Prior to add, any current scope is deleted.
   virtual Status addPrimaryScope(const char * primaryScopeName)
   {
      return addScope(&m_primaryScope, primaryScopeName);
   }
   virtual Status addSecondaryScope(const char * secondaryScopeName)
   {
     return addScope(&m_secondaryScope, secondaryScopeName);
   }

   // Sets the primary and secondary scope states in the DHCP server
   virtual Status setPrimaryScopeState(ScopeState state)
   {
	   return setScopeState(&m_primaryScope, state);
   }
   virtual Status setSecondaryScopeState(ScopeState state)
   {
	   return setScopeState(&m_secondaryScope, state);
   }

   // Adds an IP range to a scope in the DHCP server
   virtual Status addIpRange(DWORD scope, DWORD from, DWORD to, DHCPType dhcpType);

   // Adds an exclude range to a scope in the DHCP server
   virtual Status addExcludeRange(DWORD scope, DWORD from, DWORD to);

   // Deletes an entry in the DHCP server's tables if it matches the supplied
   // client IP address.
   virtual Status deleteClient(const ClientInformation* clientInfo, DeleteClientOption deleteOption);

   // Adds a client to the DHCP server.
   virtual Status addClient(const ClientInformation* clientInfo, DHCPType dhcpType);

   // Sets a reserved option value (string) for the supplied client in the
   // supplied scope.
   virtual Status setReservedOptionValue(DWORD clientIP, DWORD optionID, const char* value);

   // Sets an option value (numerical) on scope level.
   virtual Status setOptionValue(DWORD scope, DWORD optionID, DWORD value, const char *user = "");

   // Adds an option def (STRING) to the DHCP server.
   virtual Status addOptionDef(DWORD optionID, char* comment);

   // Returns the netmask value.
   u_int32 netmask() const;

   // Allows modification of the event logging feature (this is by default set
   // to on). The event logging feature logs all DHCP server access to the
   // event log.
   void turnOnEventLogging(bool setOn);

protected:
   Status deleteScope(const Scope *scopeInfo, DeleteScopeForceFlag deleteflag);
   Status addScope(const Scope *scopeInfo, const char *scopeName);
   Status setScopeState(const Scope *scopeInfo, ScopeState state);

   // The maximum length of any command or its reply (line) that is supported.
   static const int MaxLength = 1024;

   // IP address constants.
   static const u_int32 IP192_168_169_1 = 0xc0a8a901;
   static const u_int32 IP192_168_169_2 = 0xc0a8a902;
   static const u_int32 IP192_168_170_1 = 0xc0a8aa01;
   static const u_int32 IP192_168_170_2 = 0xc0a8aa02;

   // Closes the pipe (if open).
   void closePipe();

   // Command or command reply information is stored here.
   char m_commandInformation[MaxLength];

   // Handle to a pipe which is used when executing commands and reading the
   // replies.
   FILE* m_pipeHandle;

   // DHCP server scope information for the primiary network is stored here.
   Scope m_primaryScope;

   // DHCP server scope information for the secondary network is stored here.
   Scope m_secondaryScope;

   // The IP address to use when accessing the DHCP server.
   const u_int32 m_DHCPServer;

   // The netmask to use when accessing the DHCP server.
   const u_int32 m_Netmask;

   // Contains information about whether all DHCP server access should be
   // logged in the event log or not.
   bool m_isEventLoggingEnabled;
};

//******************************************************************************
// Description:
//    Tells the netmask value in use when accessing the DHCP server.
// Returns:
//    The netmask value.
//******************************************************************************
inline u_int32 DHCPConfiguration::netmask() const
{
   return m_Netmask;
}

#endif
