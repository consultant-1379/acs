//******************************************************************************
//
// NAME
//      DHCPConfiguration
//
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
//  See DHCPConfiguration.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-06-09 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .SEE ALSO 
//  N/A.
//
//******************************************************************************
#ifdef _MSC_VER
#if _MSC_VER > 1200
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif
#endif // _MSC_VER

#include "DHCPConfiguration.H"
#include "ClientInformation.H"
#include "MACAddress.H"
#include "EventReporter.H"
#include "MultiPlatformMutex.hxx"
#include "Log.H"
#include <stdio.h>
#include <cassert>

namespace
{
   MultiPlatformMutex DHCPConfigurationMutex;
}

// This mutex class has been added to synchronize this class with the DHCP class
// in the new MODD

class DHCPMutex
{
	public:
			DHCPMutex()
			{
				shutdownEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, ACS_CS_NS::EVENT_NAME_SHUTDOWN);
				dhcpMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, ACS_CS_NS::MUTEX_NAME_DHCP_PROTECTION);

				if ( (dhcpMutex) && (shutdownEvent))
				{
					HANDLE handleArr[] = {shutdownEvent, dhcpMutex};
					DWORD returnValue = WaitForMultipleObjects(2, handleArr, FALSE, INFINITE);

					if (returnValue == WAIT_FAILED)
					{
						 static_cast<void>(EVENT(EventReporter::SubSystemError,
									PROBLEM_DATA("WaitForMultipleObjects failed, GetLastError() = %d",
									GetLastError()),
									PROBLEM_TEXT("")));
					}
				}
			}

			~DHCPMutex()
			{
				if (dhcpMutex)
					ReleaseMutex(dhcpMutex);

				if (shutdownEvent)
					CloseHandle(shutdownEvent);

				if (dhcpMutex)
					CloseHandle(dhcpMutex);
			}

	private:
			HANDLE shutdownEvent;
			HANDLE dhcpMutex;
};

// When in _DEBUG builds, we monitor the execution time for the netsh commands
// that we execute. When in !_DEBUG builds, the MONITOR macro is empty.
#ifdef _DEBUG
#include "PerformanceMonitor.hxx"



class ExecutionMonitor
{
public:
   ExecutionMonitor()
   {
      monitor.start();
   }
   ~ExecutionMonitor()
   {
      try
      {
         monitor.stop();

         DBGLOG("\n\t\tWallClock  : %I64d ms\n\t\tUser time  : %I64d ms\n\t\tKernel time: %I64d ms\n",
                monitor.wallClockTimeInMilliSec(),
                monitor.userTimeInMilliSec(),
                monitor.kernelTimeInMilliSec());
      }
      catch(...)
      {
      }
   }
private:
   PerformanceMonitor monitor;
};
#define MEASURE ExecutionMonitor theExecutionMonitor
#else // _DEBUG
#define MEASURE
#endif // _DEBUG

//******************************************************************************
// Description:
//    Constructor.
// Parameters:
//    DHCPAddress  The address to use when accessing the DHCP server.
//    theNetmask   The netmask to use when accessing the DHCP server.
//******************************************************************************
DHCPConfiguration::DHCPConfiguration(u_int32 DHCPAddress, u_int32 theNetmask) :
   m_pipeHandle(NULL),
   m_DHCPServer(DHCPAddress),
   m_Netmask(theNetmask),
   m_isEventLoggingEnabled(false)
{
   // Set up the Scope identities along with the m_Netmask.
   m_primaryScope.netmask(m_Netmask);
   m_primaryScope.address(IP192_168_169_1 & m_Netmask);
   m_secondaryScope.netmask(m_Netmask);
   m_secondaryScope.address(IP192_168_170_1 & m_Netmask);
   m_commandInformation[0] = '\0';
}

//******************************************************************************
// Description:
//    Checks if a scope exist in the DHCP server.
// Parameters:
//    scope          The scope.
//    existsIt(out)  True if the scope is found.
//                   The parameter has a valid value only if OK is returned.
// Returns: 
//    OK when the scope has been succesfully checked for, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::existsScope(DWORD scope,
                                                         bool &existsIt)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   if(scope == 0)
   {
      // It will not be possible to read any information for this scope.
      return Failure;
   }

   // Ask for the list of possible scopes in DHCP.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d show scope",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:

      ==============================================================================
       Scope Address  - Subnet Mask    - State        - Scope Name          -  Comment

      ==============================================================================

       192.168.169.0  - 255.255.255.0  -Disabled      -ipna                 -

       192.168.170.0  - 255.255.255.0  -Disabled      -ipnb                 -


       Total No. of Scopes = 2
      Command completed successfully.
   */
   bool isScopeFound = false;
   char scopeAsString[16];
   sprintf(scopeAsString, "%d.%d.%d.%d",(scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF);
   while(!isScopeFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);
      if(strstr(m_commandInformation, scopeAsString) != NULL)
      {
         // The scope has been found.
         isScopeFound = true;
      }
   }

   closePipe();
   existsIt = isScopeFound;
   return OK;
}

//******************************************************************************
// Description:
//    Checks if an IP range exist in a scope in the DHCP server.
// Parameters:
//    scope          The scope.
//    existsIt(out)  True if the IP range is found.
//                   The parameter has a valid value only if OK is returned.
// Returns: 
//    OK when the scope has been succesfully checked for, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::existsIPRange(DWORD scope,
                                                           DWORD range,
                                                           bool &existsIt)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   if(scope == 0 || range == 0)
   {
      // It will not be possible to read any information for this scope.
      return Failure;
   }

   // Ask for the list of possible scopes in DHCP.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d show iprange",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            // Scope address
            (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:


      =============================================================
         Start Address   -    End Address    -     Address Type
      =============================================================
         192.168.169.3   -   192.168.169.254 -  DHCP BOOTP

      No of IP Ranges : 1 in the Scope : 192.168.169.0.

      Command completed successfully.
   */
   bool isRangeFound = false;
   char rangeAsString[16];
   sprintf(rangeAsString, "%d.%d.%d.%d",(range >> 24), (range >> 16) & 0xFF, (range >> 8) & 0xFF, range & 0xFF);
   while(!isRangeFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, rangeAsString) != NULL)
      {
         // The range has been found.
         isRangeFound = true;
      }
   }

   closePipe();
   existsIt = isRangeFound;
   return OK;
}

//******************************************************************************
// Description:
//    Checks for an entry in the DHCP server's tables if it matches the supplied
//    client IP address and Identifier.
// Parameters:
//    clientInfo    Carrier of the client's IP address, boot filename and MAC
//                  address.
//    existsIt(out) True if the client was found.
//                  The parameter has a valid value only if OK is returned.
// Returns:
//    OK when the client has been successfully search for.
//    Failure if the requested operation could not be performed.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::existsClient(const ClientInformation* clientInfo, bool &existsIt)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   DWORD scope = clientInfo->ipAddress() & m_Netmask;
   MACAddress theIdentifier;

   // Construct the command that will show the clients for the supplied scope.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d show reservedip",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            // Scope address
            (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      ===============================================================
      Reservation Address -    Unique ID
      ===============================================================

         192.168.169.131   -    00-00-02-00-00-04-
         192.168.169.128   -    00-00-01-00-00-01-
         192.168.169.129   -    00-00-02-00-00-02-
         192.168.169.130   -    00-00-01-00-00-03-
         192.168.169.132   -    32-35-35-2e-32-35-35-2e-31-35-2e-33-1a-65-74-68-30-
   */
   DWORD IPAddress;
   bool isFound = false;

   while(!isFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);
      DWORD data[4];

      int dataIndex = sscanf(m_commandInformation, "%d.%d.%d.%d %*s%*x-",
                             &data[0], &data[1], &data[2], &data[3]);

      if(dataIndex == 4)
      {
         // Excellent, 4 data items were read, construct an IP address from
         // parts of the read information.
         IPAddress = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

         // Does the read entry match the IP address of the client?
         if(IPAddress == clientInfo->ipAddress())
         {
            // An entry with a matching IP address and some sort of identifier
            // (MAC address) has been found. Continue with reading the
            // identifier.
            char separators[] = "-\n";
            char* token = strtok(m_commandInformation, separators);
            token = strtok(NULL, separators);

            char identifierAsBytes[MACAddress::MaxIdentifierLength];
            size_t identifierLength = 0;

            while(token != NULL)
            {
               DWORD id;

               if(sscanf(token, "%x", &id) != 1)
               {
                  DBGLOG("Failed to parse the netsh output\n");
                  return Failure;
               }

               identifierAsBytes[identifierLength++] = static_cast<char>(id);

               token = strtok(NULL, separators);
            }

            theIdentifier = MACAddress(identifierAsBytes, identifierLength);
            if(clientInfo->macAddress() == theIdentifier)
            {
               // This is a match.
               isFound = true;
            }
         } //if(IPAddress == clientInfo->ipAddress())
      } // end if(dataIndex == 4)
   } // end while

   closePipe();
   existsIt = isFound;
   return OK;
}

//******************************************************************************
// Description:
//    Deletes a scope to the DHCP server.
// Parameters:
//    scopeInfo   Scope carrier.
//    deleteFlag  The delete flavour. Brutal or kind.
// Returns: 
//    OK when the scope has been successfully deleted, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::deleteScope(const Scope *scopeInfo, DeleteScopeForceFlag deleteFlag)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   size_t offset = 0;

   // Construct the command that will delete the requested scope.
   offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d delete scope %d.%d.%d.%d %s",
	   // DHCP server IP address
	   (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
       // Scope address
       (scopeInfo->address() >> 24), (scopeInfo->address() >> 16) & 0xFF, (scopeInfo->address() >> 8) & 0xFF, scopeInfo->address() & 0xFF,
	   // delete option
	   ((deleteFlag == DhcpFullForce) ? "DHCPFULLFORCE" : "DHCPNOFORCE"));

   // Sanity check.
   assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
	   static_cast<void>(EVENT(EventReporter::SubSystemError,
		   PROBLEM_DATA("popen failed for the command \"%s\"",
		   m_commandInformation),
		   PROBLEM_TEXT("")));

	   return Failure;
   }
   else
   {
	   // Make all changes to the DHCP server traceable.
	   if(m_isEventLoggingEnabled)
	   {
		   static_cast<void>(EVENT(EventReporter::NonCritical,
			   PROBLEM_DATA("Executing \"%s\"",
			   m_commandInformation),
			   PROBLEM_TEXT("")));
	   }
   }

   // Read the command reply.
   /*
   The read data should be on this form:

   Command completed successfully.

   or:

   The specified subnet does not exist.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
	   DBGLOG("%s", m_commandInformation);

	   if(strstr(m_commandInformation, "Command completed successfully") != NULL ||
         strstr(m_commandInformation, "The specified subnet does not exist") != NULL)
	   {
		   // The scope has been removed or wasn't found.
		   isSuccessful = true;
	   }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}
 
//******************************************************************************
// Description:
//    Adds a scope to the DHCP server.
// Parameters:
//    scopeInfo   Scope carrier.
//    scopeName   The scope name.
// Returns:
//    OK when the scope has been successfully added, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::addScope(const Scope *scopeInfo, const char * scopeName)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   // Delete the scope brutally.
   Status deleteStatus = deleteScope(scopeInfo, DhcpFullForce);

   if(deleteStatus != OK)
   {
      return Failure;
   }

   int offset;

   // Construct the command that will add a client to the DHCP server.
   offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d add scope %d.%d.%d.%d %d.%d.%d.%d \"%s\"",
                    // DHCP server IP address
                    (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
                    // Scope address
                    (scopeInfo->address() >> 24), (scopeInfo->address() >> 16) & 0xFF, (scopeInfo->address() >> 8) & 0xFF, scopeInfo->address() & 0xFF,
                    // Netmask
                    (m_Netmask >> 24), (m_Netmask >> 16) & 0xFF, (m_Netmask >> 8) & 0xFF, m_Netmask & 0xFF,
                    // Scope name
                    scopeName);

   // Sanity check.
   assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
 
      Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL)
      {
         // The scope has been added.
         isSuccessful = true;
      }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}

//******************************************************************************
// Description:
//    Sets a scope state in the DHCP server.
// Parameters:
//    scopeInfo   Scope carrier.
//    state		  The scope state (deactivated or activated).
// Returns: 
//    OK when the scope has been successfully set to wanted state, 
//	  otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::setScopeState(const Scope *scopeInfo, ScopeState state)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   size_t offset = 0;

   // Construct the command that will set a state on the requested scope.
   offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d set state %s",
	   // DHCP server IP address
	   (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
       // Scope address
       (scopeInfo->address() >> 24), (scopeInfo->address() >> 16) & 0xFF, (scopeInfo->address() >> 8) & 0xFF, scopeInfo->address() & 0xFF,
	   // state
	   ((state == Activate) ? "1" : "0"));

   // Sanity check.
   assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
	   static_cast<void>(EVENT(EventReporter::SubSystemError,
		   PROBLEM_DATA("popen failed for the command \"%s\"",
		   m_commandInformation),
		   PROBLEM_TEXT("")));

	   return Failure;
   }
   else
   {
	   // Make all changes to the DHCP server traceable.
	   if(m_isEventLoggingEnabled)
	   {
		   static_cast<void>(EVENT(EventReporter::NonCritical,
			   PROBLEM_DATA("Executing \"%s\"",
			   m_commandInformation),
			   PROBLEM_TEXT("")));
	   }
   }

   // Read the command reply.
   /*
   The read data should be on this form:

   Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
	   DBGLOG("%s", m_commandInformation);

	   if(strstr(m_commandInformation, "Command completed successfully") != NULL)
	   {
		   // The scope state has been set.
		   isSuccessful = true;
	   }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}
 
//******************************************************************************
// Description:
//    Adds an IP range to scope to the DHCP server.
// Parameters:
//    scope       The scope.
//    from        The lower part of the range.
//    to          The higher part of the range
//    dhcpType    Tells whether the client should be granted DHCP requests,
//                BOOTP requests or both.
// Returns:
//    OK when the IP range has been successfully added, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::addIpRange(DWORD scope, DWORD from,
                                                        DWORD to, DHCPType dhcpType)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   int offset;

   // Construct the command that will add a client to the DHCP server.
   offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d add iprange %d.%d.%d.%d %d.%d.%d.%d",
                    // DHCP server IP address
                    (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
                    // Scope address
                    (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF,
                    // Lower range value
                    (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, from,
                    // Higher range value
                    (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, to);
   
   // Sanity check
   assert((dhcpType == DHCPAndBOOTP ||
           dhcpType == DHCPOnly ||
           dhcpType == BOOTPOnly) &&
          "Invalid DHCP type");

   // Add a name and comment about the entry to the command string.
   offset += sprintf(&m_commandInformation[offset], " \"%s\"",
                     dhcpType == DHCPAndBOOTP ? "BOTH" : dhcpType == DHCPOnly ? "DHCP" : "BOOTP");

   // Sanity check.
   assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:

      Changed the current scope context to 192.168.169.0 scope.
 
      Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL)
      {
         // The scope's IP range has been added.
         isSuccessful = true;
      }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}

//******************************************************************************
// Description:
//    Adds an excluded range to a scope to the DHCP server.
// Parameters:
//    scope       The scope.
//    from        The lower part of the range.
//    to          The higher part of the range
// Returns:
//    OK when the excluded range has been successfully added, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::addExcludeRange(DWORD scope,
                                                             DWORD from,
                                                             DWORD to)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   int offset;

   // Construct the command that will add a client to the DHCP server.
   offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d add excluderange %d.%d.%d.%d %d.%d.%d.%d",
                    // DHCP server IP address
                    (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
                    // Scope address
                    (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF,
                    // Lower range value
                    (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, from,
                    // Higher range value
                    (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, to);
   
   // Sanity check.
   assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
 
      Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL)
      {
         // The scope's exclude range has been added.
         isSuccessful = true;
      }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}

//******************************************************************************
// Description:
//    Deletes an entry in the DHCP server's tables if it matches the supplied
//    client IP address.
// Parameters:
//    clientInfo    Carrier of the client's IP address, boot filename and MAC
//                  address.
//    deleteOption  Submit DeleteIfNoMatch if the client should be removed
//                  only if the client's IP address and MAC address does not
//                  match the already existing data in the DHCP table. If the
//                  client should be removed no matter what; supply Force.
// Returns:
//    OK when the client has been successfully removed or it does not exist in
//    the DHCP server's tables.
//    Match when the deleteOption is DeleteIfNoMatch and the supplied client
//    matches the already existing data in the DHCP table.
//    Failure if the requested operation could not be performed.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::deleteClient(const ClientInformation* clientInfo,
                                                          DeleteClientOption deleteOption)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   DWORD scope = clientInfo->ipAddress() & m_Netmask;
   MACAddress theIdentifier;

   // Construct the command that will show the clients for the supplied scope.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d show reservedip",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            // Scope address
            (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      ===============================================================
      Reservation Address -    Unique ID
      ===============================================================

         192.168.169.131   -    00-00-02-00-00-04-
         192.168.169.128   -    00-00-01-00-00-01-
         192.168.169.129   -    00-00-02-00-00-02-
         192.168.169.130   -    00-00-01-00-00-03-
         192.168.169.132   -    32-35-35-2e-32-35-35-2e-31-35-2e-33-1a-65-74-68-30-
   */
   DWORD IPAddress;
   bool isFound = false;

   while(!isFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);
      DWORD data[4];

      int dataIndex = sscanf(m_commandInformation, "%d.%d.%d.%d %*s%*x-",
                             &data[0], &data[1], &data[2], &data[3]);

      if(dataIndex == 4)
      {
         // Excellent, 4 data items were read, construct an IP address from
         // parts of the read information.
         IPAddress = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

         // Does the read entry match the IP address of the client that should
         // be deleted?
         if(IPAddress == clientInfo->ipAddress())
         {
            // An entry with a matching IP address and some sort of identifier
            // (MAC address) has been found. Continue with reading the
            // identifier.
            char separators[] = "-\n";
            char* token = strtok(m_commandInformation, separators);
            token = strtok(NULL, separators);

            char identifierAsBytes[MACAddress::MaxIdentifierLength];
            size_t identifierLength = 0;

            while(token != NULL)
            {
               DWORD id;

               if(sscanf(token, "%x", &id) != 1)
               {
                  DBGLOG("Failed to parse the netsh output\n");
                  return Failure;
               }

               identifierAsBytes[identifierLength++] = static_cast<char>(id);

               token = strtok(NULL, separators);
            }

            if(deleteOption == DeleteIfNoMatch)
            {
               theIdentifier = MACAddress(identifierAsBytes, identifierLength);
               if(clientInfo->macAddress() == theIdentifier)
               {
                  // This is a match.
                  closePipe();
                  return Match;
               }
               else
               {
                  isFound = true;
               }
            }
            else if(deleteOption == Force)
            {
               isFound = true;
            }
         }
      }
   }

   closePipe();

   if(isFound)
   {
      if(deleteOption == Force)
      {
         // This client should not be deleted since it only should be deleted
         // if the MAC address for the client did not match the corresponding
         // information for the client's IP address in the DHCP table.
         return Match;
      }

      // Delete the client that was found among the clients for the current
      // scope.

      DWORD clientIP = clientInfo->ipAddress();
      size_t offset = 0;

      // Construct the command that will delete the requested client.
      offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d delete reservedip %d.%d.%d.%d ",
                       // DHCP server IP address
                       (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
                       // Scope address
                       (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF,
                       // Client IP address
                       (clientIP >> 24), (clientIP >> 16) & 0xFF, (clientIP >> 8) & 0xFF, clientIP & 0xFF);

      // Add the MAC address identifier to the command string.
      for(size_t i = 0; i < theIdentifier.identifierLength(); ++i)
      {
         offset += sprintf(&m_commandInformation[offset], "%02x", theIdentifier.asBytes()[i]);
      }

      // Sanity check.
      assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

      // The command has not yet been executed.
      m_pipeHandle = _popen(m_commandInformation, "rt");

      if(m_pipeHandle == NULL)
      {
         static_cast<void>(EVENT(EventReporter::SubSystemError,
               PROBLEM_DATA("popen failed for the command \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));

         return Failure;
      }
      else
      {
         // Make all changes to the DHCP server traceable.
         if(m_isEventLoggingEnabled)
         {
            static_cast<void>(EVENT(EventReporter::NonCritical,
                  PROBLEM_DATA("Executing \"%s\"",
                               m_commandInformation),
                  PROBLEM_TEXT("")));
         }
      }

      // Read the command reply.
      /*
         The read data should be on this form:
         Changed the current scope context to 192.168.170.0 scope.

         Command completed successfully.
      */
      bool isSuccessful = false;

      while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
      {
         DBGLOG("%s", m_commandInformation);

         if(strstr(m_commandInformation, "Command completed successfully") != NULL)
         {
            // The scope has been changed removed.
            isSuccessful = true;
         }
      }

      closePipe();

      return isSuccessful ? OK : Failure;
   }
   else
   {
      // The client IP was not found, hence deletion is not required.
      return OK;
   }
}

//******************************************************************************
// Description:
//    Adds a client to the DHCP server.
// Parameters:
//    clientInfo  Carrier of the client information.
//    dhcpType    Tells whether the client should be granted DHCP requests,
//                BOOTP requests or both.
// Returns:
//    OK when the client has been successfully added, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::addClient(const ClientInformation* clientInfo,
                                                       DHCPConfiguration::DHCPType dhcpType)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   const char* name = clientInfo->hostname();
   const char* comment = clientInfo->description();

   // Begin with deleting an already existing entry in the DHCP tables for
   // the IP address associated with this client.
   Status deleteStatus = deleteClient(clientInfo, DeleteIfNoMatch);

   DWORD scope = clientInfo->ipAddress() & m_Netmask;

   if(deleteStatus == Match)
   {
      // There is no need to add a client that already exists.
      return OK;
   }
   else if(deleteStatus != OK)
   {
      return Failure;
   }

   int offset;

   // Construct the command that will add a client to the DHCP server.
   offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d add reservedip %d.%d.%d.%d ",
                    // DHCP server IP address
                    (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
                    // Scope address
                    (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF,
                    // Client's IP address
                    (clientInfo->ipAddress() >> 24), (clientInfo->ipAddress() >> 16) & 0xFF, (clientInfo->ipAddress() >> 8) & 0xFF, clientInfo->ipAddress() & 0xFF);

   // Add the MAC address identifier to the command string.
   for(size_t i = 0; i < clientInfo->macAddress().identifierLength(); ++i)
   {
      offset += sprintf(&m_commandInformation[offset], "%02x", clientInfo->macAddress().asBytes()[i]);
   }

   // Sanity check
   assert((dhcpType == DHCPAndBOOTP ||
           dhcpType == DHCPOnly ||
           dhcpType == BOOTPOnly) &&
          "Invalid DHCP type");

   // Add a name and comment about the entry to the command string.
   offset += sprintf(&m_commandInformation[offset], " \"%s\" \"%s\" %s",
                     name,
                     comment,
                     dhcpType == DHCPAndBOOTP ? "BOTH" : dhcpType == DHCPOnly ? "DHCP" : "BOOTP");

   // Sanity check.
   assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      Changed the current scope context to 192.168.169.0 scope.

      Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL)
      {
         // The scope has been changed removed.
         isSuccessful = true;
      }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}

//******************************************************************************
// Description:
//    Sets a reserved option value (string) for the supplied client in the
//    supplied scope.
// Parameters:
//    clientIP  The client for which the option ID should be set.
//    optionID  The option that should be set.
//    value     The string value to be set.
// Returns:
//    OK when the reserved option value has been successfully set, otherwise
//    Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::setReservedOptionValue(DWORD clientIP,
                                                                    DWORD optionID,
                                                                    const char* value)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   DWORD scope = clientIP & m_Netmask;

   // Construct the command that will set the client's boot filename.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d set reservedoptionvalue %d.%d.%d.%d %d STRING \"%s\"",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            // Scope address
            (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF,
            // Client IP address.
            (clientIP >> 24), (clientIP >> 16) & 0xFF, (clientIP >> 8) & 0xFF, clientIP & 0xFF,
            // Option ID
            optionID,
            // Option value
            value);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      Changed the current scope context to 192.168.169.0 scope.

      Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL)
      {
         // The scope has been changed removed.
         isSuccessful = true;
      }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}

//******************************************************************************
// Description:
//    Sets an option value (numerical) on scope level.
// Parameters:
//    scope     The scope for which the option should be set.
//    optionID  The option that should be set.
//    value     The numerical value to be set.
//    user      The userData (optional)
// Returns:
//    OK when the option has been successfully set, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::setOptionValue(DWORD scope,
                                                            DWORD optionID,
                                                            DWORD value,
                                                            const char *user)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   int offset;
   char userData[80];
   memset(userData, 0, 80);
   if (user && user[0])
   {
      offset = sprintf(userData, "user=\"%s\" ", user);
      // Sanity check.
      assert(offset < 80 && "Adding the information to userData wrote outside allocated memory");
   }

   // Construct the command that will set the client's boot filename.
   offset = sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d set optionvalue %d DWORD %s%u",
      // DHCP server IP address
      (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
      // Scope address
      (scope >> 24), (scope >> 16) & 0xFF, (scope >> 8) & 0xFF, scope & 0xFF,
      optionID,
      (user == 0 || user[0] == NULL) ? "" : userData,
      value);

   // Sanity check.
   assert(offset < MaxLength && "Adding the information to m_commandInformation wrote outside allocated memory");

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      Changed the current scope context to 192.168.169.0 scope.

      Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL)
      {
         // The scope has been changed removed.
         isSuccessful = true;
      }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}

//******************************************************************************
// Description:
//    Adds an option def (STRING) to the DHCP server.
// Parameters:
//    optionID  The option that should be set.
//    comment   A description of the new option ID.
// Returns:
//    OK when the option has been successfully created, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::addOptionDef(DWORD optionID,
                                                            char* comment)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   // Construct the command that will set the client's boot filename.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d add optiondef %d \"%s\" STRING",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            optionID,
            comment);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      Changed the current scope context to 192.168.169.0 scope.

      Command completed successfully.
   */
   bool isSuccessful = false;

   while(!isSuccessful && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL ||
         strstr(m_commandInformation, "The specified option already exists") != NULL)
      {
         // The scope has been changed removed.
         isSuccessful = true;
      }
   }

   closePipe();

   return isSuccessful ? OK : Failure;
}

//******************************************************************************
// Description:
//    Updates the provided client information with the associated boot image
//    filename.
// Parameters:
//    client  [in]   Reference to the client to which boot image filename
//                   information should be added.
//    client  [out]  The client's boot image filename has been updated if such
//                   information was found.
// Returns:
//    OK when an entry has been successfully read.
//    Failure when no boot image filename could be read for the provided
//    client.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::getBootfileInformation(ClientInformation& client)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   Scope scope;

   if((client.ipAddress() & m_secondaryScope.netmask()) == m_secondaryScope.address())
   {
      scope = m_secondaryScope;
   }
   else if((client.ipAddress() & m_secondaryScope.netmask()) == m_secondaryScope.address())
   {
      scope = m_secondaryScope;
   }
   else
   {
      DBGLOG("The requested information could not be found in any scope\n");
      assert(!"The requested information could not be found in any scope");
      return Failure;
   }

   if(scope.address() == 0)
   {
      // It will not be possible to read any information for this scope.
      return Failure;
   }

   // Ask for the boot image filename for the provided client.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d show reservedoptionvalue %d.%d.%d.%d",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            // Scope address
            (scope.address() >> 24), (scope.address() >> 16) & 0xFF, (scope.address() >> 8) & 0xFF, scope.address() & 0xFF,
            // Client IP address
            (client.ipAddress() >> 24), (client.ipAddress() >> 16) & 0xFF, (client.ipAddress() >> 8) & 0xFF, client.ipAddress() & 0xFF);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      Changed the current scope context to 192.168.74.0 scope.

      Options for the Reservation Address 192.168.74.15 in the Scope 192.168.74.0 :

            DHCP Standard Option :
            General Option Values:
            OptionId : 150
            Option Value:
                     Number of Option Elements = 1
                     Option Element Type = STRING
                     Option Element Value = (nd)/meny.lst
            OptionId : 67
            Option Value:
                     Number of Option Elements = 1
                     Option Element Type = STRING
                     Option Element Value = FooBootFileName
      Command completed successfully.
   */
   bool isNameFound = false;
   DWORD optionId;
   while(!isNameFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      char bootFilename[ClientInformation::MaxFilenameLength];
      if(sscanf(m_commandInformation, "%*cOptionId : %d%*s", &optionId) == 1 && optionId == 67)
      {
         // The boot filename option has been found, now get the boot image
         // filename.
         while(!isNameFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
         {
            DBGLOG(m_commandInformation);

            if(sscanf(m_commandInformation, "%*c%*cOption Element Value = %s", bootFilename) == 1)
            {
               isNameFound = (client.bootFilename(bootFilename) == ClientInformation::OK ? true : false);
            }
         }
      }
   }

   closePipe();

   return isNameFound ? OK : Failure;
}

//******************************************************************************
// Description:
//    Checks if the provided client information with the associated boot image
//    filename in DHCP.
// Parameters:
//    client  [in]   Reference to the client to which boot image filename
//                   information should be looke for.
//    existsIt [out] True if the client's bootfile name is found
// Returns:
//    OK when a search have been done. With or without any hit.
//    Failure when the search couldn't be performed.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::existsClientsBootfile(ClientInformation& client,
                                                                   bool& existsIt)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   Scope scope;

   if((client.ipAddress() & m_primaryScope.netmask()) == m_primaryScope.address())
   {
      scope = m_primaryScope;
   }
   else if((client.ipAddress() & m_secondaryScope.netmask()) == m_secondaryScope.address())
   {
      scope = m_secondaryScope;
   }
   else
   {
      DBGLOG("The requested information could not be found in any scope\n");
      assert(!"The requested information could not be found in any scope");
      return Failure;
   }

   if(scope.address() == 0)
   {
      // It will not be possible to read any information for this scope.
      return Failure;
   }

   // Ask for the boot image filename for the provided client.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d show reservedoptionvalue %d.%d.%d.%d",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            // Scope address
            (scope.address() >> 24), (scope.address() >> 16) & 0xFF, (scope.address() >> 8) & 0xFF, scope.address() & 0xFF,
            // Client IP address
            (client.ipAddress() >> 24), (client.ipAddress() >> 16) & 0xFF, (client.ipAddress() >> 8) & 0xFF, client.ipAddress() & 0xFF);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:
      Changed the current scope context to 192.168.74.0 scope.

      Options for the Reservation Address 192.168.74.15 in the Scope 192.168.74.0 :

            DHCP Standard Option :
            General Option Values:
            OptionId : 150
            Option Value:
                     Number of Option Elements = 1
                     Option Element Type = STRING
                     Option Element Value = (nd)/meny.lst
            OptionId : 67
            Option Value:
                     Number of Option Elements = 1
                     Option Element Type = STRING
                     Option Element Value = FooBootFileName
      Command completed successfully.
   */
   bool isNameFound = false;
   DWORD optionId;
   while(!isNameFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      char bootFilename[ClientInformation::MaxFilenameLength];
      if(sscanf(m_commandInformation, "%*cOptionId : %d%*s", &optionId) == 1 && optionId == 67)
      {
         // The boot filename option has been found, now get the boot image
         // filename.
         while(!isNameFound && fgets(m_commandInformation, MaxLength, m_pipeHandle))
         {
            DBGLOG(m_commandInformation);

            if(sscanf(m_commandInformation, "%*c%*cOption Element Value = %s", bootFilename) == 1)
            {
               isNameFound = (strcmp(bootFilename, client.bootFilename()) == NULL) ? true : false; 
            }
         }
      }
   }

   closePipe();
   existsIt = isNameFound;
   return OK;
}

//******************************************************************************
// Description:
//    Change the boot filename for a specific client.
// Parameters:
//    client          The client for which the boot filename should be
//                    changed.
//    filenameLength  The length of the filename (bytes).
//    filename        The new boot filename to use for the supplied client.
// Returns:
//    OK when an entry has been successfully changed.
//    Failure when it was not possible to change the boot filename.
//******************************************************************************
DHCPConfiguration::Status DHCPConfiguration::changeBootFilename(const ClientInformation* client,
                                                                u_int16 filenameLength,
                                                                const char* filename)
{
   // Make sure all access is thread safe.
   //SafeMutex mutex(DHCPConfigurationMutex);
   DHCPMutex mutex;
   MEASURE;

   Scope scope;

   if((client->ipAddress() & m_primaryScope.netmask()) == m_primaryScope.address())
   {
      scope = m_primaryScope;
   }
   else if((client->ipAddress() & m_secondaryScope.netmask()) == m_secondaryScope.address())
   {
      scope = m_secondaryScope;
   }
   else
   {
      DBGLOG("The requested information could not be found in any scope");
      assert(!"The requested information could not be found in any scope");
      return Failure;
   }

   if(scope.address() == 0)
   {
      // It will not be possible to set any information for this scope.
      return Failure;
   }
   else if(filenameLength > (DHCPConfiguration::MaxLength - 100))
   {
      // 100 is roughly the length of the netsh command below, this command
      // should be extended with the new filename information which only can
      // be done if it will fit the memory allocated for m_commandInformation.
      return Failure;
   }

   // Set the boot image filename for the provided client.
   sprintf(m_commandInformation, "netsh dhcp server %d.%d.%d.%d scope %d.%d.%d.%d set reservedoptionvalue %d.%d.%d.%d 067 STRING \"%s\"",
            // DHCP server IP address
            (m_DHCPServer >> 24), (m_DHCPServer >> 16) & 0xFF, (m_DHCPServer >> 8) & 0xFF, m_DHCPServer & 0xFF,
            // Scope address
            (scope.address() >> 24), (scope.address() >> 16) & 0xFF, (scope.address() >> 8) & 0xFF, scope.address() & 0xFF,
            // Client IP address
            (client->ipAddress() >> 24), (client->ipAddress() >> 16) & 0xFF, (client->ipAddress() >> 8) & 0xFF, client->ipAddress() & 0xFF,
            // New boot filename.
            filename);

   // The command has not yet been executed.
   m_pipeHandle = _popen(m_commandInformation, "rt");

   if(m_pipeHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("popen failed for the command \"%s\"",
                         m_commandInformation),
            PROBLEM_TEXT("")));

      return Failure;
   }
   else
   {
      // Make all changes to the DHCP server traceable.
      if(m_isEventLoggingEnabled)
      {
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("Executing \"%s\"",
                            m_commandInformation),
               PROBLEM_TEXT("")));
      }
   }

   // Read the command reply.
   /*
      The read data should be on this form:

      Changed the current scope context to 192.168.169.0 scope.

      Command completed successfully.
   */
   bool isNameChanged = false;

   while(!isNameChanged && fgets(m_commandInformation, MaxLength, m_pipeHandle))
   {
      DBGLOG("%s", m_commandInformation);

      if(strstr(m_commandInformation, "Command completed successfully") != NULL)
      {
         // The boot filename has been changed successfully.
         isNameChanged = true;
      }
   }

   closePipe();

   return isNameChanged ? OK : Failure;
}

//******************************************************************************
// Description:
//    Closes the pipe (if open).
//******************************************************************************
void DHCPConfiguration::closePipe()
{
   if(m_pipeHandle)
   {
      int result = _pclose(m_pipeHandle);

      if(result == -1)
      {
         static_cast<void>(EVENT(EventReporter::SubSystemError,
               PROBLEM_DATA("pclose failed, errno = %d",
                            errno),
               PROBLEM_TEXT("")));
      }

      DBGLOG("The last executed command returned %d\n", result);

      m_pipeHandle = NULL;
   }
}

//******************************************************************************
// Description:
//    Allows modification of the event logging feature (this is by default set
//    to on). The event logging feature logs all DHCP server access to the
//    event log.
// Parameters:
//    toggle  true to enable event logging (default), or false to turn event
//            logging off.
//******************************************************************************
void DHCPConfiguration::turnOnEventLogging(bool toggle)
{
   m_isEventLoggingEnabled = toggle;
}
