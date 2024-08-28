//******************************************************************************
//
// NAME
//      APZ2123xConfiguration
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
//  See APZ2123xConfiguration.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-06-10 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

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

#include "APZ2123xConfiguration.H"
#include "RegistryInformation.H"
#include "ClientInformation.H"
#include "EventReporter.H"
#include "Log.H"
#include <cassert>

// Substitutes the subnet part in the supplied address with the supplied
// subnet.
#define CREATE_BOOT_ADDRESS(address, subnet) \
   (((address) & 0xFFFF0000) | ((subnet) << 8) | ((address) & 0x000000FF))

//******************************************************************************
// Description:
//    The Constructor.
// Parameters:
//    name        A name associated with the instance.
//    DHCPServer  The IP address to use when accessing the DHCP server.
//******************************************************************************
APZ2123xConfiguration::APZ2123xConfiguration(const char* theName, 
                                             u_int32 DHCPServer) :
   APZConfiguration(theName),
   m_dhcpConfiguration(DHCPServer, Netmask),
   m_isClientLeaseConfigured(false),
   m_isInvalidDiscoveryItemLogged(false),
   m_BootIPAddress_Lan1(CREATE_BOOT_ADDRESS(DHCPServer, 169)),
   m_BootIPAddress_Lan2(CREATE_BOOT_ADDRESS(DHCPServer, 170)),
   m_isDhcpRegistryConfigured(false),
   m_state(InitialSetup),
   m_IPN0_Eth0State(ConfigureDHCP),
   m_IPN1_Eth1State(ConfigureDHCP),
   m_IPN2_Eth0State(ConfigureDHCP),
   m_IPN3_Eth1State(ConfigureDHCP),
   m_currentDiscoveryItem(IPN0_Eth0),
   m_isInvalidStateReportedForSetupPeerConnectionImplementation(false)
{
   DBGLOG("APZ2123xConfiguration(%s, ...)\n",
          name());
}

//******************************************************************************
// Description:
//    The Destructor.
//******************************************************************************
APZ2123xConfiguration::~APZ2123xConfiguration()
{
}

//******************************************************************************
// Description:
//    Checks the APZ 212 3x DHCP basic configuration.
//    The APZ 212 3x scope configuration is static and thereby configurated once
//    is enough. The method checks that unique APZ 212 3x information exist. 
//    If the check is false, it's interpreted as the DHCP is not configured for 
//    APZ 212 3x and this will be done. The scope, iprange and excludes will be
//    created from scratch.
// Returns:
//    Finished when all checks has been successfully done, and that
//    a possible reconfiguration from scratch was successful.
//    Failed if any failure to access DHCP server table whatsoever for the
//    APZ 212 3x setup.
//******************************************************************************
APZConfiguration::Operation APZ2123xConfiguration::checkBasicDHCPSetupImplementation()
{
	// Some parts of this function has been removed by comments. This has been done since
	// some checks are made by code in the new MODDBIN instead of here.

   bool isFound = true;  
   u_int32 netmask = m_dhcpConfiguration.netmask();
/*
   if(m_dhcpConfiguration.existsScope(Eth0_IPAddress_IPN0 & netmask, 
      isFound) != DHCPConfiguration::OK)
   {
      DBGLOG("(%s) Failed to check for primary scope (ipna)\n", name());
      return Failed;
   }
*/
   if(isFound == true)
   {
/*
      // The scope was found, now check for the correct IP range
      if(m_dhcpConfiguration.existsIPRange(Eth0_IPAddress_IPN0 & netmask, 
         Eth0_IPAddress_IPN0, // Lower IP range
         isFound) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to check for lower IP range on primary scope (ipna)\n", name());
         return Failed;
      }
*/
      if(isFound == true)
      {
         // Also the correct IP range was found, check now for specific APZ 212 3x configuration
         // Build up a temporarily client

         // Client information for interface Eth0 for the IPN0.
         ClientInformation anIPN0Eth0Client;
         // Assign hostnames.
         anIPN0Eth0Client.hostname("ipn0");
         // Assign descriptions.
         anIPN0Eth0Client.description("IPN0-AP Lan1");
         // Assign IP address information.
         anIPN0Eth0Client.ipAddress(Eth0_IPAddress_IPN0);
         // Assign MAC adress information. 49:50:4e:41:30:30
         u_char macAddress[6];
         macAddress[0] = 0x49;
         macAddress[1] = 0x50;
         macAddress[2] = 0x4e;
         macAddress[3] = 0x41;
         macAddress[4] = 0x30;
         macAddress[5] = 0x30; // IPN0
         anIPN0Eth0Client.macAddress(MACAddress(macAddress, 6));
         // Assign boot image file names.
         static_cast<void>(anIPN0Eth0Client.bootFilename("boot.ipn0"));
         // Assign boot IP address.
         anIPN0Eth0Client.bootIPAddress(m_BootIPAddress_Lan1);
/*
         if(m_dhcpConfiguration.existsClient(&anIPN0Eth0Client, isFound) != DHCPConfiguration::OK)
         {
            DBGLOG("(%s) Failed to check for client's boot file name: %s/%s\n", name(),
               anIPN0Eth0Client.hostname(),anIPN0Eth0Client.bootFilename());
            return Failed;
         }
 */
      }
   }
/*
   if(isFound == false)
   {
      // Primary scope
      // Add primary scope. Implicit delete of current primary scope.
      if(m_dhcpConfiguration.addPrimaryScope("ipna") != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to add the primary scope (ipna)\n", name());
         return Failed;
      }

      // Set the primary scope in state 'Deactivated'.
      // The scope is activated when fully configured.
      if(m_dhcpConfiguration.setPrimaryScopeState(DHCPConfiguration::Deactivate) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the primary scope state (ipna)\n", name());
         return Failed;
      }

      // Add IP range for primary scope
      if(m_dhcpConfiguration.addIpRange(Eth0_IPAddress_IPN0 & netmask,
         Eth0_IPAddress_IPN0 & 0xFF, // Lower IP range 
         HighIPRange,
         DHCPConfiguration::DHCPAndBOOTP) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the primary scope's (ipna) ip range\n", name());
         return Failed;
      }
      
      // Add exclude ranges for primary scope
      if(m_dhcpConfiguration.addExcludeRange(Eth0_IPAddress_IPN0 & netmask,
         ExcludeRangeOneLow,
         ExcludeRangeOneLow) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the primary scope's (ipna) first exclude range\n", name());
         return Failed;
      }
      if(m_dhcpConfiguration.addExcludeRange(Eth0_IPAddress_IPN0 & netmask, 
         ExcludeRangeTwoLow, ExcludeRangeTwoHigh) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the primary scope's (ipna) second exclude ranges\n", name());
         return Failed;
      }

      // Secondary scope
      // Add secondary scope. Implicit delete of current secondary scope.
      if(m_dhcpConfiguration.addSecondaryScope("ipnb") != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to add the secondary scope (ipnb)\n", name());
         return Failed;
      }

      // Set the secondary scope in state 'Deactivated'.
      // The scope is activated when fully configured.
      if(m_dhcpConfiguration.setSecondaryScopeState(DHCPConfiguration::Deactivate) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the secondary scope state (ipnb)\n", name());
         return Failed;
      }

      // Add IP range for secondary scope
      if(m_dhcpConfiguration.addIpRange(Eth1_IPAddress_IPN1 & netmask,
         Eth1_IPAddress_IPN1 & 0xFF, // Lower IP range
         HighIPRange,
         DHCPConfiguration::DHCPAndBOOTP) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the secondary scope's (ipnb) ip range\n", name());
         return Failed;
      }

      // Add exclude ranges for secondary scope
      if(m_dhcpConfiguration.addExcludeRange(Eth1_IPAddress_IPN1 & netmask, 
         ExcludeRangeOneLow, ExcludeRangeOneLow) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the secondary scope's (ipnb) first exclude range\n", name());
         return Failed;
      }
      if(m_dhcpConfiguration.addExcludeRange(Eth1_IPAddress_IPN1 & netmask,
         ExcludeRangeTwoLow, ExcludeRangeTwoHigh) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the secondary scope's (ipnb) second exclude ranges\n", name());
         return Failed;
      }
   }
   // else
   // ;
*/
   return Finished;
}

//******************************************************************************
// Description:
//    Cleans up the DHCP configuration, by cleaning the scopes.
// Returns:
//    Finished when the clean up has been successfully done
//    Failed if any failure to access the DHCP server.
//******************************************************************************
APZConfiguration::Operation APZ2123xConfiguration::cleanUpDHCPSetupImplementation()
{
   u_int32 netmask = m_dhcpConfiguration.netmask();

   if(m_dhcpConfiguration.deletePrimaryScope(DHCPConfiguration::DhcpFullForce) != DHCPConfiguration::OK)
   {
      DBGLOG("(%s) Failed to delete primary scope (ipna)\n", name());
      return Failed;
   }

   if(m_dhcpConfiguration.deleteSecondaryScope(DHCPConfiguration::DhcpFullForce) != DHCPConfiguration::OK)
   {
      DBGLOG("(%s) Failed to delete secondary scope (ipnb)\n", name());
      return Failed;
   }

   return Finished;
}

//******************************************************************************
// Description:
//    Sets the instance in a well defined state to be able to commence setting
//    up the required information in the DHCP server.
// Returns:
//    Finished when the lease periods have been set successfully.
//    Failed when it was not possible to set the lease period for both
//    subnets.
//******************************************************************************
APZConfiguration::Operation APZ2123xConfiguration::initializeImplementation()
{
   DBGLOG("(%s) initializeImplementation\n", name());

   m_state = InitialSetup;

   // Clear any remaining traces from a previous setup DHCP session.
   clearSetupDHCPInformation();

 
   if(!m_isClientLeaseConfigured)
   {
      u_int32 netmask = m_dhcpConfiguration.netmask();

      // Set the lease period on primary scope (169) level.
      if(m_dhcpConfiguration.setOptionValue(Eth0_IPAddress_IPN0 & netmask, 51, 4294967295) != DHCPConfiguration::OK ||
         m_dhcpConfiguration.setOptionValue(Eth0_IPAddress_IPN0 & netmask, 51, 4294967295, "Default BOOTP Class") != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the lease period on Primary scope level\n", name());
         return Failed;
      }

      // Set the lease period on secondary scope (170) level.
      if(m_dhcpConfiguration.setOptionValue(Eth1_IPAddress_IPN1 & netmask, 51, 4294967295) != DHCPConfiguration::OK ||
         m_dhcpConfiguration.setOptionValue(Eth1_IPAddress_IPN1 & netmask, 51, 4294967295, "Default BOOTP Class") != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the lease period on Secondary scope level\n", name());
         return Failed;
      }
      m_isClientLeaseConfigured = true;
   }
   if(!m_isDhcpRegistryConfigured)
   {
      // Set the registry key value to 1. If the key doesn't exist, create it.
      bool isKeyChanged = false; 
      RegistryInformation regInfo;
      u_int32 keyValue = 1; // The value of the IgnoreBroadcastFlag for APZ 212 3x
      if(regInfo.setIgnoreBroadcastFlag(keyValue, isKeyChanged) != RegistryInformation::OK)
      {
         DBGLOG("Failed to configure the DHCP IgnoreBroadcastFlag in the registry\n");
         return Failed;
      }
      if(isKeyChanged)
      {
         ; // What shall we do. The Event log is already notified by RegistryInformation class
      }
            isKeyChanged = false; // Reset
      keyValue = 100; // The value for DhcpAlertPercentage registry key
      if(regInfo.setDhcpAlertPercentage(keyValue, isKeyChanged) != RegistryInformation::OK)
      {
         DBGLOG("Failed to configure the DhcpAlertPercentage in the registry\n");
         return Failed;
      }
      if(isKeyChanged)
      {
         ; // What shall we do. The Event log is already notified by RegistryInformation class.
      }

      isKeyChanged = false; // Reset
      keyValue = 1; // The value for DhcpAlertCount registry key
      if(regInfo.setDhcpAlertCount(keyValue, isKeyChanged) != RegistryInformation::OK)
      {
         DBGLOG("Failed to configure the DhcpAlertCount in the registry\n");
         return Failed;
      }
      if(isKeyChanged)
      {
         ; // What shall we do. The Event log is already notified by RegistryInformation class.
      }

      m_isDhcpRegistryConfigured = true;
   }

   // Advance to the next state. The next step is almost short-cutted for APZ 212 3x,
   // as there is no PTB comm in place
   m_state = ConnectToPTB;

   return Finished;
}

//******************************************************************************
// Description:
//    * It's almost a dummy for APZ2123x, just sets next state *
//    This method tries to establish an APZ PTB peer connection.
//
//    NOTE: THIS METHOD IS NOT THREAD SAFE AND SHOULD NOT BE THREAD SAFE. NO
//          MEMBER DATA THAT MAY BE EXPOSED TO A CALLER OF ANY OTHER METHOD
//          IS ALLOWED TO BE ACCESSED WHILE EXECUTING IN THE CONTEXT OF THIS
//          METHOD.
// Returns:
//    Finished.
//******************************************************************************
APZConfiguration::Operation APZ2123xConfiguration::setupPeerConnectionImplementation()
{
   APZConfiguration::Operation reply = Failed;

   DBGLOG("(%s) setupPTBConnectionImplementation(state = %d)\n", name(), m_state);

   switch(m_state)
   {
   case ConnectToPTB:
      reply = Finished;
      m_state = SetupDHCPInformation;
      break;

   default:
      if(!m_isInvalidStateReportedForSetupPeerConnectionImplementation)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
            PROBLEM_DATA("(%s) The current state (%d) is not valid, forcing the state to a well-defined start",
            name(),
            m_state),
            PROBLEM_TEXT("")));

         m_isInvalidStateReportedForSetupPeerConnectionImplementation = true;
      }

      m_state = InitialSetup;
   }

   return reply;
}

//******************************************************************************
// Description:
//    This method sets the static and known MAC address information and updates
//    the DHCP server with any information it assembles.
// Returns:
//    InProgress for as long as no error is detected.
//    Failed if any permanent error should occur.
//******************************************************************************
APZConfiguration::Operation APZ2123xConfiguration::setupDHCPImplementation()
{
   static bool isInvalidStateReported = false;
   APZConfiguration::Operation reply = Failed;

   DBGLOG("(%s) setupDHCPImplementation(state = %d)\n", name(), m_state);

   switch(m_state)
   {
   case SetupDHCPInformation:
      {
         // Set up the DHCP server with the APZ specific information.
         Status status = setupDHCPInformtion();

         if(status == OK)
         {
            reply = InProgress;
            m_state = KeepAlive;
         }
         else if(status == Executing)
         {
            reply = InProgress;
         }
      }
      break;

   case KeepAlive:
      // In 212 3x the thread goes to sleep by allowing execution to shift from
      // calls to setupDHCPImplementation to calls to finalizeImplementation.
      reply = Finished;
      break;

   case InitialSetup:
      reply = initialize();
      break;

   default:
      if(!isInvalidStateReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("(%s) The current state (%d) is not valid, forcing the state to a well-defined start",
                            name(),
                            m_state),
               PROBLEM_TEXT("")));

         isInvalidStateReported = true;
      }

      m_state = InitialSetup;
   }

   return reply;
}

//******************************************************************************
// Description:
//    Provides a possibility to continue executing without member data access.
//
//    NOTE: THIS METHOD IS NOT THREAD SAFE AND SHOULD NOT BE THREAD SAFE. NO
//          MEMBER DATA THAT MAY BE EXPOSED TO A CALLER OF ANY OTHER METHOD
//          IS ALLOWED TO BE ACCESSED WHILE EXECUTING IN THE CONTEXT OF THIS
//          METHOD.
// Returns:
//    Finished.
//******************************************************************************
APZ2123xConfiguration::Operation APZ2123xConfiguration::finalizeImplementation()
{
   DBGLOG("(%s) finalizeImplementation, setup is done. Thread goes to Cinderella sleep.\n", name());

   Sleep(INFINITE);

   return Finished;
}

//******************************************************************************
// Description:
//    Sets the known and static MAC address information and adds client specific
//    information to the DHCP server.
// Returns:
//    OK once the all client information has been added to the DHCP server.
//    Executing if not all client information has been processed.
//    NotOK if an error has ocurred.
//******************************************************************************
APZ2123xConfiguration::Status APZ2123xConfiguration::setupDHCPInformtion()
{
   DBGLOG("(%s) setupDHCPInformtion(discoveryItem = %d)\n", name(), m_currentDiscoveryItem);

   DiscoveryState* discoveryState = NULL;
   ClientInformation* clientInformation = NULL;

   m_currentDiscoveryItem = nextDiscoveryItem();

   if(m_currentDiscoveryItem == AllItemsProcessed)
   {
      // We are finished with the process of setting MAC address information
      // and setting up the DHCP server. Activate the scopes.
      // Set the primary scope in state 'Activated'.
      if(m_dhcpConfiguration.setPrimaryScopeState(DHCPConfiguration::Activate) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the primary scope state (ipna)\n", name());
         return NotOK;
      }

      // Set the secondary scope in state 'Activated'.
      if(m_dhcpConfiguration.setSecondaryScopeState(DHCPConfiguration::Activate) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the secondary scope state (ipnb)\n", name());
         return NotOK;
      }

      return OK;
   }

   // Determine the current discovery state for the current item.
   switch(m_currentDiscoveryItem)
   {
   case IPN0_Eth0:
      discoveryState = &m_IPN0_Eth0State;
      clientInformation = &m_IPN0_Eth0;
      break;

   case IPN1_Eth1:
      discoveryState = &m_IPN1_Eth1State;
      clientInformation = &m_IPN1_Eth1;
      break;

   case IPN2_Eth0:
      discoveryState = &m_IPN2_Eth0State;
      clientInformation = &m_IPN2_Eth0;
      break;

   case IPN3_Eth1:
      discoveryState = &m_IPN3_Eth1State;
      clientInformation = &m_IPN3_Eth1;
      break;

   default:
      assert(!"Invalid discovery item");

      if(!m_isInvalidDiscoveryItemLogged)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("(%s) The current discovery item (%d) is not valid",
                            name(),
                            m_currentDiscoveryItem),
               PROBLEM_TEXT("")));

         m_isInvalidDiscoveryItemLogged = true;
      }

      return NotOK;
   }

   // Process the current discovery state.
   switch(*discoveryState)
   {
   case ConfigureDHCP:
      if(configureDHCP(clientInformation) == OK)
      {
         *discoveryState = AllDone;
      }
      else
      {
         return NotOK;
      }
      break;

   case AllDone:
      break;

   default:
      assert(!"Invalid Discovery state");
   }

   return Executing;
}

//******************************************************************************
// Description:
//    Tries to find client information associated with the supplied MAC
//    address.
// Parameters:
//    macAddress  The MAC address for which a client is wanted.
// Returns:
//    A pointer to the found client, NULL if no match can be found.
//******************************************************************************
ClientInformation* APZ2123xConfiguration::getClientInformationImplementation(const MACAddress& macAddress)
{
   DBGLOG("(%s) getClientInformationImplementation\n", name());

   if(m_IPN0_Eth0.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_IPN0_Eth0.macAddress()))
   {
      return &m_IPN0_Eth0;
   }

   if(m_IPN1_Eth1.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_IPN1_Eth1.macAddress()))
   {
      return &m_IPN1_Eth1;
   }

   if(m_IPN2_Eth0.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_IPN2_Eth0.macAddress()))
   {
      return &m_IPN2_Eth0;
   }

   if(m_IPN3_Eth1.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_IPN3_Eth1.macAddress()))
   {
      return &m_IPN3_Eth1;
   }

   // No match was found.
   return NULL;
}

//******************************************************************************
// Description:
//    Should decrease the automatic revert timer for all ClientInformation
//    instances available.
// Parameters:
//    elapsedSeconds  The number of seconds that should be reduced from the
//                    current timer information for all ClientInformation
//                    instances.
// Returns:
//    A list of pointers to ClientInformation instances for which the timer
//    has reached zero.
//******************************************************************************
void APZ2123xConfiguration::decreaseRevertTimerImplementation(u_int16 elapsedSeconds)
{
   decreaseRevertTimer(elapsedSeconds, m_IPN0_Eth0);
   decreaseRevertTimer(elapsedSeconds, m_IPN1_Eth1);
   decreaseRevertTimer(elapsedSeconds, m_IPN2_Eth0);
   decreaseRevertTimer(elapsedSeconds, m_IPN3_Eth1);
}

//******************************************************************************
// Description:
//    The ClientInformation instances for which the timer has expired should
//    perform an automatic MODD revert operation.
//******************************************************************************
void APZ2123xConfiguration::automaticRevertImplementation()
{
   automaticRevert(m_IPN0_Eth0);
   automaticRevert(m_IPN1_Eth1);
   automaticRevert(m_IPN2_Eth0);
   automaticRevert(m_IPN3_Eth1);
}

//******************************************************************************
// Description:
//    Changes the boot filename for the supplied client.
// Parameters:
//    client          The client for which the boot filename should be
//                    changed.
//    filenameLength  The length of the filename (bytes).
//    filename        The new boot filename to use for the supplied client.
// Returns:
//    Finished if the change was successful, otherwise Failed.
//******************************************************************************
APZConfiguration::Operation APZ2123xConfiguration::changeBootFilenameImplementation(const ClientInformation* client,
                                                                                    u_int16 filenameLength,
                                                                                    const char* filename)
{
   DBGLOG("(%s) changeBootFilenameImplementation\n", name());

   return m_dhcpConfiguration.changeBootFilename(client,
                                                filenameLength,
                                                filename) == DHCPConfiguration::OK ? Finished : Failed;
}

//******************************************************************************
// Description:
//    An order that all raised alarms must be ceased because the process is
//    about to terminate has been received from PRC. Cease all alarms and
//    prepare to be terminated.
//******************************************************************************
void APZ2123xConfiguration::ceaseAlarmsImplementation()
{
   ; // No alarms when dealing with APZ 212 30x
}

//******************************************************************************
// Description:
//    Assigns IP addresses and boot filenames to all clients whose MAC address
//    information will be read and set up in the DHCP server.
//******************************************************************************
void APZ2123xConfiguration::assignStaticClientInformation()
{
   DBGLOG("(%s) assignStaticClientInformation\n", name());

   // Clear all old information.
   m_IPN0_Eth0.clear();
   m_IPN2_Eth0.clear();
   m_IPN1_Eth1.clear();
   m_IPN3_Eth1.clear();

   // Assign hostnames.
   m_IPN0_Eth0.hostname("ipn0");
   m_IPN2_Eth0.hostname("ipn2");
   m_IPN1_Eth1.hostname("ipn1");
   m_IPN3_Eth1.hostname("ipn3");

   // Assign descriptions.
   m_IPN0_Eth0.description("IPN0-AP Lan1");
   m_IPN2_Eth0.description("IPN2-AP Lan1");
   m_IPN1_Eth1.description("IPN1-AP Lan2");
   m_IPN3_Eth1.description("IPN3-AP Lan2");

   // Assign IP address information.
   m_IPN0_Eth0.ipAddress(Eth0_IPAddress_IPN0);
   m_IPN2_Eth0.ipAddress(Eth0_IPAddress_IPN2);
   m_IPN1_Eth1.ipAddress(Eth1_IPAddress_IPN1);
   m_IPN3_Eth1.ipAddress(Eth1_IPAddress_IPN3);

   // Assign MAC adress information. 49:50:4e:41:30:30-33
   u_char macAddress[6];
   macAddress[0] = 0x49;
   macAddress[1] = 0x50;
   macAddress[2] = 0x4e;
   macAddress[3] = 0x41;
   macAddress[4] = 0x30;
   macAddress[5] = 0x30; // IPN0
   m_IPN0_Eth0.macAddress(MACAddress(macAddress, 6));
   macAddress[5] = 0x32; // IPN2
   m_IPN2_Eth0.macAddress(MACAddress(macAddress, 6));
   macAddress[5] = 0x31; // IPN1
   m_IPN1_Eth1.macAddress(MACAddress(macAddress, 6));
   macAddress[5] = 0x33; // IPN3
   m_IPN3_Eth1.macAddress(MACAddress(macAddress, 6));

   // Assign boot image file names.
   static_cast<void>(m_IPN0_Eth0.bootFilename("boot.ipn0"));
   static_cast<void>(m_IPN2_Eth0.bootFilename("boot.ipn2"));
   static_cast<void>(m_IPN1_Eth1.bootFilename("boot.ipn1"));
   static_cast<void>(m_IPN3_Eth1.bootFilename("boot.ipn3"));

   // Assign boot IP address.
   m_IPN0_Eth0.bootIPAddress(m_BootIPAddress_Lan1);
   m_IPN2_Eth0.bootIPAddress(m_BootIPAddress_Lan1);
   m_IPN1_Eth1.bootIPAddress(m_BootIPAddress_Lan2);
   m_IPN3_Eth1.bootIPAddress(m_BootIPAddress_Lan2);
}

//******************************************************************************
// Description:
//    Perform an automatic MODD revert operation if the supplied
//    clientInformation instance's automatic revert operation timer has
//    expired.
// Parameters:
//    clientInformation  The ClientInformation instance for which an automatic
//                       MODD revert operation should be executed if its
//                       automatic revert operation timer has expired.
//******************************************************************************
void APZ2123xConfiguration::automaticRevert(ClientInformation& clientInformation)
{
   // Did the timer expire? If so, then perform the automatic revert operation
   // only when a service connection is not also established since an
   // established service connection is processed by the service thread. Hence
   // performing the automatic revert operation now could cause multiple
   // access to the DHCP server tables at the same time from multiple threads.
   if(clientInformation.isTimerExpired())
   {
      DBGLOG("(%s) Time based automatic revert operation targets %d.%d.%d.%d\n",
             name(),
             (clientInformation.ipAddress() >> 24) & 0xFF,
             (clientInformation.ipAddress() >> 16) & 0xFF,
             (clientInformation.ipAddress() >> 8) & 0xFF,
             clientInformation.ipAddress() & 0xFF);

      // Time based automatic revert is required.
      if(changeBootFilename(&clientInformation,
                            clientInformation.originalBootFilename()) == Finished)
      {
         // Make changes to the DHCP configuration visible in the event log
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("(%s) Changed boot filename (MODD automatic revert) for MAC address %02X-%02X-%02X-%02X-%02X-%02X to %s",
                            name(),
                            clientInformation.macAddress().asBytes()[0], clientInformation.macAddress().asBytes()[1],
                            clientInformation.macAddress().asBytes()[2], clientInformation.macAddress().asBytes()[3],
                            clientInformation.macAddress().asBytes()[4], clientInformation.macAddress().asBytes()[5],
                            clientInformation.originalBootFilename()),
               PROBLEM_TEXT("")));

         // Clear the timeout value since we successfully handled the expired
         // timer.
         clientInformation.timeoutValue(0);
      }
      else
      {
         // The automatic revert operation failed.
         static_cast<void>(EVENT(EventReporter::NonCritical,
               PROBLEM_DATA("(%s) Time based automatic revert operation with target %d.%d.%d.%d failed",
                            name(),
                            (clientInformation.ipAddress() >> 24) & 0xFF, (clientInformation.ipAddress() >> 16) & 0xFF,
                            (clientInformation.ipAddress() >> 8) & 0xFF, clientInformation.ipAddress() & 0xFF),
               PROBLEM_TEXT("")));
      }
   }
}

//******************************************************************************
// Description:
//    Should decrease the automatic revert timer for the specific
//    ClientInformation instance.
// Parameters:
//    elapsedSeconds     The number of seconds that should be reduced from the
//                       current timer information the specific
//                       ClientInformation instance.
//    clientInformation  The specific ClientInformation instance for which
//                       the timer should be reduced and an automatic MODD
//                       revert operation should take place if the timer
//                       reaches zero.
// Returns:
//    A list of pointers to ClientInformation instances for which the timer
//    has reached zero.
//******************************************************************************
void APZ2123xConfiguration::decreaseRevertTimer(u_int16 elapsedSeconds,
                                                ClientInformation& clientInformation)
{
   // Decrease the timer with the elapsed time.
   clientInformation.decreaseTimeoutValue(elapsedSeconds);
}

//******************************************************************************
// Description:
//    Clears all information about MAC address to IP address translations
//    along with state information to ensure that all MAC address information
//    is read from scratch again.
//******************************************************************************
void APZ2123xConfiguration::clearSetupDHCPInformation()
{
   // Set up client specific information that will never change.
   assignStaticClientInformation();

   // Initialize the MAC address discovery process.
   m_IPN0_Eth0State = m_IPN1_Eth1State = 
      m_IPN2_Eth0State = m_IPN3_Eth1State = ConfigureDHCP;

   // Begin with acquireing information from IPN0_Eth0.
   m_currentDiscoveryItem = IPN0_Eth0;
}

//******************************************************************************
// Description:
//    Determines what discovery item to process next.
// Returns:
//    Either of IPN0_Eth0, IPN1_Eth1, IPN2_Eth0 or IPN3_Eth1 when further
//    discovery is required. AllItemsProcessed when all information has been
//    acquired and set up in the DHCP server.
//******************************************************************************
APZ2123xConfiguration::DiscoveryItem APZ2123xConfiguration::nextDiscoveryItem()
{
   static bool isInvalidStateReported = false;

   DiscoveryItem nextItem = m_currentDiscoveryItem;

   switch(m_currentDiscoveryItem)
   {
   case IPN0_Eth0:
      if(isInEndState(m_IPN0_Eth0State))
      {
         if(!isInEndState(m_IPN1_Eth1State))
         {
            nextItem = IPN1_Eth1;
         }
         else if(!isInEndState(m_IPN2_Eth0State))
         {
            nextItem = IPN2_Eth0;
         }
         else if(!isInEndState(m_IPN3_Eth1State))
         {
            nextItem = IPN3_Eth1;
         }
         else
         {
            nextItem = AllItemsProcessed;
         }
      }
      break;

   case IPN1_Eth1:
      if(isInEndState(m_IPN1_Eth1State))
      {
         if(!isInEndState(m_IPN2_Eth0State))
         {
            nextItem = IPN2_Eth0;
         }
         else if(!isInEndState(m_IPN3_Eth1State))
         {
            nextItem = IPN3_Eth1;
         }
         else if(!isInEndState(m_IPN0_Eth0State))
         {
            nextItem = IPN0_Eth0;
         }
         else
         {
            nextItem = AllItemsProcessed;
         }
      }
      break;

   case IPN2_Eth0:
      if(isInEndState(m_IPN2_Eth0State))
      {
         if(!isInEndState(m_IPN3_Eth1State))
         {
            nextItem = IPN3_Eth1;
         }
         else if(!isInEndState(m_IPN0_Eth0State))
         {
            nextItem = IPN0_Eth0;
         }
         else if(!isInEndState(m_IPN1_Eth1State))
         {
            nextItem = IPN1_Eth1;
         }
         else
         {
            nextItem = AllItemsProcessed;
         }
      }
      break;

   case IPN3_Eth1:
      if(isInEndState(m_IPN3_Eth1State))
      {
         if(!isInEndState(m_IPN0_Eth0State))
         {
            nextItem = IPN0_Eth0;
         }
         else if(!isInEndState(m_IPN1_Eth1State))
         {
            nextItem = IPN1_Eth1;
         }
         else if(!isInEndState(m_IPN2_Eth0State))
         {
            nextItem = IPN2_Eth0;
         }
         else
         {
            nextItem = AllItemsProcessed;
         }
      }
      break;

   case AllItemsProcessed:
      assert(!"Do not ask for the next state when we are already in the AllItemsProcessed state!");

      // Remain in this state.
      nextItem = m_currentDiscoveryItem;
      break;

   default:
      assert(!"This state does not exist");

      if(!isInvalidStateReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("(%s) The current discovery item value (%d) is not valid",
                            name(),
                            m_currentDiscoveryItem),
               PROBLEM_TEXT("")));

         isInvalidStateReported = true;
      }
   }

   return nextItem;
}

//******************************************************************************
// Description:
//    Adds the client information data to the DHCP server.
// Parameters:
//    clientInformation  The client information data to add to the DHCP
//                       server.
// Returns:
//    OK when all data was added to the DHCP server successfully. NotOK if any
//    failure to set up the DHCP server table have ocurred.
//******************************************************************************
APZ2123xConfiguration::Status APZ2123xConfiguration::configureDHCP(ClientInformation* clientInformation)
{
   // Add the client as a reserved IP address in the DHCP server.
   if(m_dhcpConfiguration.addClient(clientInformation, DHCPConfiguration::DHCPAndBOOTP) != DHCPConfiguration::OK)
   {
      DBGLOG("(%s) failed to add client information for client %s\n",
             name(),
             clientInformation->hostname());
      return NotOK;
   }

   // Set the boot filename for the client.
   if(m_dhcpConfiguration.setReservedOptionValue(clientInformation->ipAddress(),
                                                 67, // Boot filename option ID
                                                 clientInformation->bootFilename()) != DHCPConfiguration::OK)
   {
      DBGLOG("(%s) failed to set the boot filename for client %s\n",
             name(),
             clientInformation->hostname());
      return NotOK;
   }

   char bootAddress[16];

   // Create a boot address string.
   if(sprintf(bootAddress, "%d.%d.%d.%d",
              (clientInformation->bootIPAddress() >> 24) & 0xFF,
              (clientInformation->bootIPAddress() >> 16) & 0xFF,
              (clientInformation->bootIPAddress() >>  8) & 0xFF,
              clientInformation->bootIPAddress() & 0xFF) == -1)
   {
      DBGLOG("(%s) failed to construct the boot IP address from 0x%x\n",
             name(),
             clientInformation->bootIPAddress());
      return NotOK;
   }


   // Set the boot IP address for the client.
   if(m_dhcpConfiguration.setReservedOptionValue(clientInformation->ipAddress(),
                                                 66,
                                                 bootAddress) != DHCPConfiguration::OK)
   {
      DBGLOG("(%s) failed to set the boot IP address for client %s\n",
             name(),
             clientInformation->hostname());
      return NotOK;
   }

   return OK;
}
