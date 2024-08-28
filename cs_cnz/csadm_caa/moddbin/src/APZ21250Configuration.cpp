//******************************************************************************
//
// NAME
//      APZ21250Configuration
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
//  See APZ21250Configuration.H

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

#include "APZ21250Configuration.H"
#include "RegistryInformation.H"
#include "ClientInformation.H"
#include "EventReporter.H"
#include "CPTSignals.H"
#include "Log.H"
#include "TCPErrorReceiver.H"
#include <cassert>

#define DHCP_CONFIG_DISCLAIMER            "The DHCP server's tables might no longer contain valid information, which may cause boot problems for the CP."
#define ALARM_TEXT_PTB_CONNECTION         "MODD INFORMATION PROVIDER CONNECTION FAULT"
#define ALARM_TEXT_INVALID_CPTREADMAUREGR "MODD CPT SIGNAL FAULT"
#define ALARM_TEXT_PARSE_CPTREADMAUREGR   "MODD CPT SIGNAL DATA FAULT"

// Substitutes the subnet part in the supplied address with the supplied
// subnet.
#define CREATE_BOOT_ADDRESS(address, subnet) \
   (((address) & 0xFFFF0000) | ((subnet) << 8) | ((address) & 0x000000FF))

//******************************************************************************
// Description:
//    The Constructor.
// Parameters:
//    name        A name associated with the instance.
//    ptbPeer     The PTB peer that tells which physical side's CP and PCIH
//                that should be set up in the DHCP server.
//    DHCPServer  The IP address to use when accessing the DHCP server.
//    PTBAddress  The IP address to use when contacting the PTB peer.
//    PTBPort     The PTB port number to use when contacting the PTB peer.
//******************************************************************************
APZ21250Configuration::APZ21250Configuration(const char* theName, PTBPeer ptbPeer,
                                             u_int32 DHCPServer, u_int32 PTBAddress,
                                             u_int16 PTBPort) :
   APZConfiguration(theName),
   m_dhcpConfiguration(DHCPServer, Netmask),
   m_PTBPeer(ptbPeer),
   m_PTBIPAddress(PTBAddress),
   m_PTBPort(PTBPort),
   m_ptbConnection(TCPClientServer::Blocking, TCPErrorReceiver::TCPErrorReceivingFunction),
   m_isClientLeaseConfigured(false),
   m_isKeepAliveProgrammingErrorLogged(false),
   m_isInvalidDiscoveryItemLogged(false),
   m_isSendProblemReported(false),
   m_isReadProblemReported(false),
   m_isSendProgrammingErrorReported(false),
   m_isReadProgrammingErrorReported(false),
   m_isInvalidCPTREADMAUREGRReported(false),
   m_BootIPAddress_Lan1(CREATE_BOOT_ADDRESS(DHCPServer, 169)),
   m_BootIPAddress_Lan2(CREATE_BOOT_ADDRESS(DHCPServer, 170)),
   m_isConnectionAlarmSet(false),
   m_isInvalidStateReportedForSetupPeerConnectionImplementation(false),
   m_isDhcpRegistryConfigured(false),
   m_state(InitialSetup),
   m_CPSB_Eth0State(SendMACAddressRequest),
   m_CPSB_Eth1State(SendMACAddressRequest),
   m_PCIH_Eth0State(SendMACAddressRequest),
   m_PCIH_Eth1State(SendMACAddressRequest),
   m_currentDiscoveryItem(CPSB_Eth0)
{
   DBGLOG("APZ21250Configuration(%s, ...)\n",
          name());
   DBGLOG("Using %d.%d.%d.%d:%d when communicating with PTB-A\n", (PTBAIPAddress >> 24) & 0xFF, (PTBAIPAddress >> 16) & 0xFF, (PTBAIPAddress >> 8) & 0xFF, PTBAIPAddress & 0xFF, PTBAPort);
   DBGLOG("Using %d.%d.%d.%d:%d when communicating with PTB-B\n", (PTBBIPAddress >> 24) & 0xFF, (PTBBIPAddress >> 16) & 0xFF, (PTBBIPAddress >> 8) & 0xFF, PTBBIPAddress & 0xFF, PTBBPort);

   for(int item = CPSB_Eth0; item < NumberOfDiscoveryItems; ++item)
   {
      m_isInvalidCPTREADMAUREGRAlarmSet[item] = false;
      m_isParseCPTREADMAUREGRAlarmSet[item] = false;
   }
}

//******************************************************************************
// Description:
//    The Destructor.
//******************************************************************************
APZ21250Configuration::~APZ21250Configuration()
{
}

//******************************************************************************
// Description:
//    Checks the APZ 212 50 DHCP basic configuration.
//    The APZ 212 50 scope configuration is static and thereby configurated once
//    is enough. The method checks that unique APZ 212 50 information exist. 
//    If the check is false, it's interpreted as the DHCP is not configured for 
//    APZ 212 50 and this will be done. The scope, iprange and excludes will be
//    created from scratch.
// Returns:
//    Finished when all checks has been successfully done, and that
//    a possible reconfiguration from scratch was successful.
//    Failed if any failure to access DHCP server table whatsoever for the
//    APZ 212 50 setup.
//******************************************************************************
APZConfiguration::Operation APZ21250Configuration::checkBasicDHCPSetupImplementation()
{
	// Some parts of this function has been removed by comments. This has been done since
	// some checks are made by code in the new MODDBIN instead of here.

   bool isFound = true;  
   u_int32 netmask = m_dhcpConfiguration.netmask();
/*
   if(m_dhcpConfiguration.existsScope(Eth0_IPAddress_CPSB_A & netmask, 
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
      if(m_dhcpConfiguration.existsIPRange(Eth0_IPAddress_CPSB_A & netmask, 
         Eth0_IPAddress_CPSB_A, // Lower IP range
         isFound) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to check for lower IP range on primary scope (ipna)\n", name());
         return Failed;
      }
*/
      if(isFound == true)
      {
         // Also the correct IP range was found, check now for 
         // specific APZ 212 50 configuration
         // Build up a temporarily client
         // Client information for interface Eth0 for the CPSB.
         ClientInformation aCPSBClient;
         // Assign hostnames.
         aCPSBClient.hostname("eib_a");
         // Assign descriptions.
         aCPSBClient.description("");
         // Assign IP address information.
         aCPSBClient.ipAddress(Eth0_IPAddress_CPSB_A);

         // Assign boot image file names.
         static_cast<void>(aCPSBClient.bootFilename("/image/a/elilo.efi"));
         // Assign boot IP address.
         aCPSBClient.bootIPAddress(m_BootIPAddress_Lan1);
/*
         if(m_dhcpConfiguration.existsClientsBootfile(aCPSBClient, isFound) != DHCPConfiguration::OK)
         {
            DBGLOG("(%s) Failed to check for client's boot file name: %s/%s\n", name(),
               aCPSBClient.hostname(),aCPSBClient.bootFilename());
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
      if(m_dhcpConfiguration.addIpRange(Eth0_IPAddress_CPSB_A & netmask, 
         Eth0_IPAddress_CPSB_A & 0xFF, // Lower IP range 
         HighIPRange, 
         DHCPConfiguration::DHCPAndBOOTP) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the primary scope's (ipna) ip range\n", name());
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
      if(m_dhcpConfiguration.addIpRange(Eth1_IPAddress_CPSB_A & netmask,
         Eth1_IPAddress_CPSB_A & 0xFF, // Lower IP range
         HighIPRange, DHCPConfiguration::DHCPAndBOOTP) != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the secondary scope's (ipnb) ip range\n", name());
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
APZConfiguration::Operation APZ21250Configuration::cleanUpDHCPSetupImplementation()
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
//    subnets or when it was not possible to create an operational TCP
//    client.
//******************************************************************************
APZConfiguration::Operation APZ21250Configuration::initializeImplementation()
{
   DBGLOG("(%s) initializeImplementation\n", name());

   m_state = InitialSetup;

   // Disconnect whatever TCP session we have with the PTB peer.
   m_ptbConnection.disconnect();
   
   // We do not have any information on MAC address to IP address translation
   // since this information has not yet been retrieved. Clear any remaining
   // traces from a previous setup DHCP session.
   clearSetupDHCPInformation();

   // Set up the PTB TCP connection to know where to connect to.
   if(m_ptbConnection.connectOn(m_PTBIPAddress, m_PTBPort) != TCPClient::OK)
   {
      DBGLOG("(%s) Initialization of the PTB TCP client failed\n", name());
      return Failed;
   }

   if(!m_isClientLeaseConfigured)
   {
      u_int32 netmask = m_dhcpConfiguration.netmask();

      // Set the lease period on primary scope (169) level.
      if(m_dhcpConfiguration.setOptionValue(Eth0_IPAddress_CPSB_A & netmask, 51, 4294967295) != DHCPConfiguration::OK ||
         m_dhcpConfiguration.setOptionValue(Eth0_IPAddress_CPSB_A & netmask, 51, 4294967295, "Default BOOTP Class") != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the lease period on Primary scope level\n", name());
         return Failed;
      }

      // Set the lease period on secondary scope (170) level.
      if(m_dhcpConfiguration.setOptionValue(Eth1_IPAddress_CPSB_A & netmask, 51, 4294967295) != DHCPConfiguration::OK ||
         m_dhcpConfiguration.setOptionValue(Eth1_IPAddress_CPSB_A & netmask, 51, 4294967295, "Default BOOTP Class") != DHCPConfiguration::OK)
      {
         DBGLOG("(%s) Failed to set the lease period on Secondary scope level\n", name());
         return Failed;
      }

      // Set up the static entries required to provide IP addresses to the
      // GESB blades (switches).
      if(addStaticGESBEntries() != OK)
      {
         DBGLOG("(%s) Failed to set up the GESB DHCP entries\n", name());
         return Failed;
      }

      m_isClientLeaseConfigured = true;
   }

   if(!m_isDhcpRegistryConfigured && (m_PTBPeer == PTBB))
   {
      // Set the registry key value to 0. If the key doesn't exist, create it.
      bool isKeyChanged = false; 
      RegistryInformation regInfo;
      u_int32 keyValue = 0; // The value of the IgnoreBroadcastFlag for APZ 212 50
      if(regInfo.setIgnoreBroadcastFlag(keyValue, isKeyChanged) != RegistryInformation::OK)
      {
         DBGLOG("Failed to configure the DHCP IgnoreBroadcastFlag in the registry\n");
         return Failed;
      }
      if(isKeyChanged)
      {
         ; // What shall we do. The Event log is already notified by RegistryInformation class.
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

   // Advance to the next state.
   m_state = ConnectToPTB;

   return Finished;
}

//******************************************************************************
// Description:
//    This method tries to establish an APZ PTB peer connection.
//
//    NOTE: THIS METHOD IS NOT THREAD SAFE AND SHOULD NOT BE THREAD SAFE. NO
//          MEMBER DATA THAT MAY BE EXPOSED TO A CALLER OF ANY OTHER METHOD
//          IS ALLOWED TO BE ACCESSED WHILE EXECUTING IN THE CONTEXT OF THIS
//          METHOD.
// Returns:
//    Finished.
//******************************************************************************
APZConfiguration::Operation APZ21250Configuration::setupPeerConnectionImplementation()
{
   APZConfiguration::Operation reply = Failed;

   DBGLOG("(%s) setupPTBConnectionImplementation(state = %d)\n", name(), m_state);

   switch(m_state)
   {
   case ConnectToPTB:
      // Establish a connection with the PTB peer.
      if(connectToPTB() == OK)
      {
         reply = Finished;
         m_state = SetupDHCPInformation;
      }
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
//    This method reads MAC address information from the PTB peer and updates
//    the DHCP server with any information it assembles.
// Returns:
//    InProgress for as long as no error is detected.
//    Failed if any permanent error should occur.
//******************************************************************************
APZConfiguration::Operation APZ21250Configuration::setupDHCPImplementation()
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
      // Keep the PTB connection alive by allowing execution to shift from
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
APZ21250Configuration::Operation APZ21250Configuration::finalizeImplementation()
{
   keepAlive();

   return Finished;
}

//******************************************************************************
// Description:
//    Establishes a TCP connection with the PTB peer.
// Returns:
//    OK once the connection has been established, otherwise NotOK.
//******************************************************************************
APZ21250Configuration::Status APZ21250Configuration::connectToPTB()
{
   DBGLOG("(%s) connectToPTB\n", name());

   APZ21250Configuration::Status reply = NotOK;
   Media::Status status = m_ptbConnection.connect();

   if(status != Media::Connected)
   {
      m_ptbConnection.disconnect();

      // Make sure to raise the PTB connection alarm.
      raisePTBConnectionAlarm();
   }
   else
   {
      // Make sure any PTB connection alarm is no longer active.
      ceasePTBConnectionAlarm();

      m_ptbConnection.disableNagle();
      reply = OK;
   }

   return reply;
}

//******************************************************************************
// Description:
//    Reads MAC address information from the PTB peer and adds client specific
//    information to the DHCP server.
// Returns:
//    OK once the all client information has been added to the DHCP server.
//    Executing if not all client information has been processed.
//    NotOK if an error has ocurred.
//******************************************************************************
APZ21250Configuration::Status APZ21250Configuration::setupDHCPInformtion()
{
   DBGLOG("(%s) setupDHCPInformtion(discoveryItem = %d)\n", name(), m_currentDiscoveryItem);

   DiscoveryState* discoveryState = NULL;
   ClientInformation* clientInformation = NULL;
   CPTREADMAUREG::OrderCode orderCode;

   m_currentDiscoveryItem = nextDiscoveryItem();

   if(m_currentDiscoveryItem == AllItemsProcessed)
   {
      // We are finished with the process of acquireing MAC address information
      // and setting up the DHCP server.
      // Activate the scopes. Let only one APZ21250Configuration thread do the job.
      if(m_PTBPeer == PTBB)
      {
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
      }

      return OK;
   }

   // Determine the current discovery state for the current item.
   switch(m_currentDiscoveryItem)
   {
   case CPSB_Eth0:
      discoveryState = &m_CPSB_Eth0State;
      orderCode = CPTREADMAUREG::CPSB_ETH0;
      clientInformation = &m_CPSB_Eth0;
      break;

   case CPSB_Eth1:
      discoveryState = &m_CPSB_Eth1State;
      orderCode = CPTREADMAUREG::CPSB_ETH1;
      clientInformation = &m_CPSB_Eth1;
      break;

   case PCIH_Eth0:
      discoveryState = &m_PCIH_Eth0State;
      orderCode = CPTREADMAUREG::PCIH_OWN;
      clientInformation = &m_PCIH_Eth0;
      break;

   case PCIH_Eth1:
      discoveryState = &m_PCIH_Eth1State;
      orderCode = CPTREADMAUREG::PCIH_TWIN;
      clientInformation = &m_PCIH_Eth1;
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
   case SendMACAddressRequest:
      if(sendCPTREADMAUREG(orderCode) == OK)
      {
         *discoveryState = ReadMACAddressReply;
      }
      else
      {
         return NotOK;
      }
      break;

   case ReadMACAddressReply:
      // Read the reply to the requested MAC address (this will change the
      // state to ConfigureDHCP if successful).
      if(readCPTREADMAUREG(orderCode, clientInformation, discoveryState) != OK)
      {
         return NotOK;
      }
      break;

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
//    Keeps the PTB connection alive by sending CPTKEEPALIVE signals at a
//    frequency of approximately 5 seconds. The method does not return until
//    the PTB connection has been severed.
//******************************************************************************
void APZ21250Configuration::keepAlive()
{
   DBGLOG("(%s) keepAlive\n", name());

   u_int16 counter = 0;

   while(m_ptbConnection.isConnected())
   {
      CPTKEEPALIVE cptkeepalive(m_PTBPeer == PTBA ? CPTKEEPALIVE::CP_A : CPTKEEPALIVE::CP_B,
                                ++counter);
      u_int32 size;
      char* data = const_cast<char*>(cptkeepalive.message());
      u_int32 sentSize = 0;

      do
      {
         TCPClient::Status theStatus;

         theStatus = m_ptbConnection.send(&data[sentSize],
                                          cptkeepalive.size() - sentSize,
                                          size);

         switch(theStatus)
         {
         case TCPClient::NotOK:
            if(size > 0)
            {
               // Add to the already sent amount of data.
               sentSize += size;
            }
            else
            {
               // It is not possible to send the data we want. Terminate the
               // connection if some data was sent (we need to get back on
               // track again and cannot reuse this TCP connection unless all
               // data has been successfully transferred). If however no data
               // was sent, then just keep on trying.
               if(sentSize != 0)
               {
                  // TCP connection termination required.
                  DBGLOG("(%s) discovery thread lost its TCP connection when sending to the PTB peer\n", name());
                  m_ptbConnection.disconnect();
               }
            }
            break;

         case TCPClient::OK:
            // All data should have been delivered by now.
            assert((sentSize + size) == cptkeepalive.size() && "TCPClient error, all data should have been sent when it returns TCPClient::OK");
            sentSize += size;
            break;

         default:
            if(!m_isKeepAliveProgrammingErrorLogged)
            {
               m_isKeepAliveProgrammingErrorLogged = true;
               static_cast<void>(EVENT(EventReporter::ProgrammingError,
                     PROBLEM_DATA("(%s) Discovery thread programming error, not all return values (%d) taken into account",
                                  name(),
                                  theStatus),
                     PROBLEM_TEXT("")));
            }

            // Note that the absence of a break statement here is
            // intential to use the same action as in the
            // TCPClient::Disconnected case.

         case TCPClient::Disconnected:
            static_cast<void>(EVENT(EventReporter::PTBConnection,
                  PROBLEM_DATA("(%s) Discovery thread lost its TCP connection when reading from the PTB peer",
                               name()),
                  PROBLEM_TEXT("")));

            m_ptbConnection.disconnect();
            break;
         }
      }while(sentSize != cptkeepalive.size() && m_ptbConnection.isConnected());

      if(m_ptbConnection.isConnected())
      {
         // Await a CPTKEEPALIVER reply which should be 8 bytes long. But,
         // since we do not want to have to use a poll loop similar to the
         // send loop above, we try to read more than one CPTKEEPALIVER
         // message each time we call poll. This will ensure that if the PTB
         // peer is sending the data in fragments, the first fragment will be
         // read by the first call to poll, and additional fragments will be
         // read the next time poll is called (approx 5 seconds later). We are
         // never interested in the polled data as we are just sending
         // heartbeats from now on.
         const u_int32 MaxDataToRead = 512;
         u_char reply[MaxDataToRead];
         u_int32 polledSize;

         if(m_ptbConnection.poll(reply, MaxDataToRead, polledSize) == TCPClient::Disconnected)
         {
            DBGLOG("(%s) discovery thread lost its TCP connection when reading from the PTB peer\n", name());
            m_ptbConnection.disconnect();
         }
         else
         {
            // Wait approximately another 5 seconds before sending another CPTKEEPALIVE signal.
            Sleep(5000);
         }
      }
   }
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
ClientInformation* APZ21250Configuration::getClientInformationImplementation(const MACAddress& macAddress)
{
   DBGLOG("(%s) getClientInformationImplementation\n", name());

   if(m_CPSB_Eth0.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_CPSB_Eth0.macAddress()))
   {
      return &m_CPSB_Eth0;
   }

   if(m_CPSB_Eth1.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_CPSB_Eth1.macAddress()))
   {
      return &m_CPSB_Eth1;
   }

   if(m_PCIH_Eth0.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_PCIH_Eth0.macAddress()))
   {
      return &m_PCIH_Eth0;
   }

   if(m_PCIH_Eth1.isMACAddressInformationKnown() &&
      macAddress == MACAddress(m_PCIH_Eth1.macAddress()))
   {
      return &m_PCIH_Eth1;
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
void APZ21250Configuration::decreaseRevertTimerImplementation(u_int16 elapsedSeconds)
{
   decreaseRevertTimer(elapsedSeconds, m_CPSB_Eth0);
   decreaseRevertTimer(elapsedSeconds, m_CPSB_Eth1);
   decreaseRevertTimer(elapsedSeconds, m_PCIH_Eth0);
   decreaseRevertTimer(elapsedSeconds, m_PCIH_Eth1);
}

//******************************************************************************
// Description:
//    The ClientInformation instances for which the timer has expired should
//    perform an automatic MODD revert operation.
//******************************************************************************
void APZ21250Configuration::automaticRevertImplementation()
{
   automaticRevert(m_CPSB_Eth0);
   automaticRevert(m_CPSB_Eth1);
   automaticRevert(m_PCIH_Eth0);
   automaticRevert(m_PCIH_Eth1);
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
APZConfiguration::Operation APZ21250Configuration::changeBootFilenameImplementation(const ClientInformation* client,
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
void APZ21250Configuration::ceaseAlarmsImplementation()
{
   ceasePTBConnectionAlarm();
   ceaseInvalidCPTREADMAUREGRAlarm(CPSB_Eth0);
   ceaseInvalidCPTREADMAUREGRAlarm(CPSB_Eth1);
   ceaseInvalidCPTREADMAUREGRAlarm(PCIH_Eth0);
   ceaseInvalidCPTREADMAUREGRAlarm(PCIH_Eth1);
   ceaseCPTREADMAUREGRParseErrorAlarm(CPSB_Eth0);
   ceaseCPTREADMAUREGRParseErrorAlarm(CPSB_Eth1);
   ceaseCPTREADMAUREGRParseErrorAlarm(PCIH_Eth0);
   ceaseCPTREADMAUREGRParseErrorAlarm(PCIH_Eth1);
}

//******************************************************************************
// Description:
//    Assigns IP addresses and boot filenames to all clients whose MAC address
//    information will be read and set up in the DHCP server.
//******************************************************************************
void APZ21250Configuration::assignStaticClientInformation()
{
   DBGLOG("(%s) assignStaticClientInformation\n", name());

   if(m_PTBPeer == PTBA)
   {
      // Clear all old information.
      m_CPSB_Eth0.clear();
      m_PCIH_Eth0.clear();
      m_CPSB_Eth1.clear();
      m_PCIH_Eth1.clear();
      
      // Assign hostnames.
      m_CPSB_Eth0.hostname("CPSB_A_Eth0");
      m_PCIH_Eth0.hostname("PCIH_A_Own");
      m_CPSB_Eth1.hostname("CPSB_A_Eth1");
      m_PCIH_Eth1.hostname("PCIH_A_Twin");

      // Assign descriptions.
      m_CPSB_Eth0.description("CPSB_A-AP Lan1");
      m_PCIH_Eth0.description("PCIH_A-AP Lan1");
      m_CPSB_Eth1.description("CPSB_A-AP Lan2");
      m_PCIH_Eth1.description("PCIH_A-AP Lan2");

      // Assign IP address information.
      m_CPSB_Eth0.ipAddress(Eth0_IPAddress_CPSB_A);
      m_PCIH_Eth0.ipAddress(Eth0_IPAddress_PCIH_A);
      m_CPSB_Eth1.ipAddress(Eth1_IPAddress_CPSB_A);
      m_PCIH_Eth1.ipAddress(Eth1_IPAddress_PCIH_A);

      // Assign boot image file names.
      static_cast<void>(m_CPSB_Eth0.bootFilename("/image/a/elilo.efi"));
      static_cast<void>(m_PCIH_Eth0.bootFilename("/image/a/pxelinux.0"));
      static_cast<void>(m_CPSB_Eth1.bootFilename("/image/a/elilo.efi"));
      static_cast<void>(m_PCIH_Eth1.bootFilename("/image/a/pxelinux.0"));

      // Assign boot IP address.
      m_CPSB_Eth0.bootIPAddress(m_BootIPAddress_Lan1);
      m_PCIH_Eth0.bootIPAddress(m_BootIPAddress_Lan1);
      m_CPSB_Eth1.bootIPAddress(m_BootIPAddress_Lan2);
      m_PCIH_Eth1.bootIPAddress(m_BootIPAddress_Lan2);
   }
   else if(m_PTBPeer == PTBB)
   {
      // Clear all old information.
      m_CPSB_Eth0.clear();
      m_PCIH_Eth0.clear();
      m_CPSB_Eth1.clear();
      m_PCIH_Eth1.clear();
      
      // Assign hostnames.
      m_CPSB_Eth0.hostname("CPSB_B_Eth0");
      m_PCIH_Eth0.hostname("PCIH_B_Own");
      m_CPSB_Eth1.hostname("CPSB_B_Eth1");
      m_PCIH_Eth1.hostname("PCIH_B_Twin");

      // Assign descriptions.
      m_CPSB_Eth0.description("CPSB_B-AP Lan2");
      m_PCIH_Eth0.description("PCIH_B-AP Lan2");
      m_CPSB_Eth1.description("CPSB_B-AP Lan1");
      m_PCIH_Eth1.description("PCIH_B-AP Lan1");

      // Assign IP address information.
      m_CPSB_Eth0.ipAddress(Eth0_IPAddress_CPSB_B);
      m_PCIH_Eth0.ipAddress(Eth0_IPAddress_PCIH_B);
      m_CPSB_Eth1.ipAddress(Eth1_IPAddress_CPSB_B);
      m_PCIH_Eth1.ipAddress(Eth1_IPAddress_PCIH_B);

      // Assign boot image file names.
      static_cast<void>(m_CPSB_Eth0.bootFilename("/image/b/elilo.efi"));
      static_cast<void>(m_PCIH_Eth0.bootFilename("/image/b/pxelinux.0"));
      static_cast<void>(m_CPSB_Eth1.bootFilename("/image/b/elilo.efi"));
      static_cast<void>(m_PCIH_Eth1.bootFilename("/image/b/pxelinux.0"));

      // Assign boot IP address.
      m_CPSB_Eth0.bootIPAddress(m_BootIPAddress_Lan2);
      m_PCIH_Eth0.bootIPAddress(m_BootIPAddress_Lan2);
      m_CPSB_Eth1.bootIPAddress(m_BootIPAddress_Lan1);
      m_PCIH_Eth1.bootIPAddress(m_BootIPAddress_Lan1);
   }
   else
   {
      assert(!"Invalid PTB peer value");

      // Invalid thread information.
      static_cast<void>(EVENT(EventReporter::ProgrammingError,
            PROBLEM_DATA("(%s) Invalid PTB peer value (%d)",
                         name(),
                         m_PTBPeer),
            PROBLEM_TEXT("")));
   }
}

//******************************************************************************
// Description:
//    Assigns static DHCP entries for the GESB blades.
// Returns:
//    OK if adding the DHCP entries was successful, otherwise NotOK.
//******************************************************************************
APZ21250Configuration::Status APZ21250Configuration::addStaticGESBEntries()
{
   // Let only one APZ21250Configuration thread set up the GESB blade's
   // DHCP entries.
   if(m_PTBPeer == PTBB)
   {
      return OK;
   }

   // "MAC address" identifier for GESB-I-A eth0 for net 169 in slot 1.
   const char Eth0_GESB_I_A_Identifier[] = "255.255.15.1:eth0";

   // "MAC address" identifier for GESB-E-A eth0 for net 169 in slot 3.
   const char Eth0_GESB_E_A_Identifier[] = "255.255.15.3:eth0";

   // "MAC address" identifier for GESB-E-B eth0 for net 170 in slot 21.
   const char Eth0_GESB_E_B_Identifier[] = "255.255.15.21:eth0";

   // "MAC address" identifier for GESB-I-B eth0 for net 170 in slot 23.
   const char Eth0_GESB_I_B_Identifier[] = "255.255.15.23:eth0";

   // "MAC address" identifier for GESB-I-A eth1 for net 169 in slot 1.
   const char Eth1_GESB_I_A_Identifier[] = "255.255.15.1:eth1";

   // "MAC address" identifier for GESB-E-A eth1 for net 169 in slot 3.
   const char Eth1_GESB_E_A_Identifier[] = "255.255.15.3:eth1";

   // "MAC address" identifier for GESB-E-B eth1 for net 170 in slot 21.
   const char Eth1_GESB_E_B_Identifier[] = "255.255.15.21:eth1";

   // "MAC address" identifier for GESB-I-B eth1 for net 170 in slot 23.
   const char Eth1_GESB_I_B_Identifier[] = "255.255.15.23:eth1";

   // Add an entry to the DHCP server to supply an IP address for the Eth0 port
   // on the GESB that switches traffic on the internal 169 net.
   {
      ClientInformation gesbEntry("GESB-I-A", "Internal network plane A (eth0)", Eth0_GESB_I_A_Address, "");
      gesbEntry.macAddress(MACAddress(Eth0_GESB_I_A_Identifier, strlen(Eth0_GESB_I_A_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-I-A (eth0)\"\n");
         return NotOK;
      }
   }

   // Add an entry to the DHCP server to supply an IP address for the Eth0 port
   // on the GESB that switches traffic on the external 169 net.
   {
      ClientInformation gesbEntry("GESB-E-A", "External network plane A (eth0)", Eth0_GESB_E_A_Address, "");
      gesbEntry.macAddress(MACAddress(Eth0_GESB_E_A_Identifier, strlen(Eth0_GESB_E_A_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-E-A (eth0)\"\n");
         return NotOK;
      }
   }

   // Add an entry to the DHCP server to supply an IP address for the Eth0 port
   // on the GESB that switches traffic on the external 170 net.
   {
      ClientInformation gesbEntry("GESB-E-B", "External network plane B (eth0)", Eth0_GESB_E_B_Address, "");
      gesbEntry.macAddress(MACAddress(Eth0_GESB_E_B_Identifier, strlen(Eth0_GESB_E_B_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-E-B (eth0)\"\n");
         return NotOK;
      }
   }

   // Add an entry to the DHCP server to supply an IP address for the Eth0 port
   // on the GESB that switches traffic on the internal 170 net.
   {
      ClientInformation gesbEntry("GESB-I-B", "Internal network plane B (eth0)", Eth0_GESB_I_B_Address, "");
      gesbEntry.macAddress(MACAddress(Eth0_GESB_I_B_Identifier, strlen(Eth0_GESB_I_B_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-I-B (eth0)\"\n");
         return NotOK;
      }
   }

   // Add an entry to the DHCP server to supply an IP address for the Eth1 port
   // on the GESB that switches traffic on the internal 169 net.
   {
      ClientInformation gesbEntry("GESB-I-A", "Internal network plane A (eth1)", Eth1_GESB_I_A_Address, "");
      gesbEntry.macAddress(MACAddress(Eth1_GESB_I_A_Identifier, strlen(Eth1_GESB_I_A_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-I-A (eth1)\"\n");
         return NotOK;
      }
   }

   // Add an entry to the DHCP server to supply an IP address for the Eth1 port
   // on the GESB that switches traffic on the external 169 net.
   {
      ClientInformation gesbEntry("GESB-E-A", "External network plane A (eth1)", Eth1_GESB_E_A_Address, "");
      gesbEntry.macAddress(MACAddress(Eth1_GESB_E_A_Identifier, strlen(Eth1_GESB_E_A_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-E-A (eth1)\"\n");
         return NotOK;
      }
   }

   // Add an entry to the DHCP server to supply an IP address for the Eth1 port
   // on the GESB that switches traffic on the external 170 net.
   {
      ClientInformation gesbEntry("GESB-E-B", "External network plane B (eth1)", Eth1_GESB_E_B_Address, "");
      gesbEntry.macAddress(MACAddress(Eth1_GESB_E_B_Identifier, strlen(Eth1_GESB_E_B_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-E-B (eth1)\"\n");
         return NotOK;
      }
   }

   // Add an entry to the DHCP server to supply an IP address for the Eth1 port
   // on the GESB that switches traffic on the internal 170 net.
   {
      ClientInformation gesbEntry("GESB-I-B", "Internal network plane B (eth1)", Eth1_GESB_I_B_Address, "");
      gesbEntry.macAddress(MACAddress(Eth1_GESB_I_B_Identifier, strlen(Eth1_GESB_I_B_Identifier)));

      if(DHCPConfiguration::OK != m_dhcpConfiguration.addClient(&gesbEntry, DHCPConfiguration::DHCPOnly))
      {
         DBGLOG("Failed to add a reserved entry to the DHCP server for \"GESB-I-B (eth1)\"\n");
         return NotOK;
      }
   }

   return OK;
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
void APZ21250Configuration::automaticRevert(ClientInformation& clientInformation)
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
void APZ21250Configuration::decreaseRevertTimer(u_int16 elapsedSeconds,
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
void APZ21250Configuration::clearSetupDHCPInformation()
{
   // Set up client specific information that will never change.
   assignStaticClientInformation();

   // Initialize the MAC address discovery process by making sure to request
   // MAC address information.
   m_CPSB_Eth0State = SendMACAddressRequest;
   m_CPSB_Eth1State = SendMACAddressRequest;
   m_PCIH_Eth0State = SendMACAddressRequest;
   m_PCIH_Eth1State = SendMACAddressRequest;

   // Begin with acquireing information from CPSB_Eth0.
   m_currentDiscoveryItem = CPSB_Eth0;
}

//******************************************************************************
// Description:
//    Determines what discovery item to process next.
// Returns:
//    Either of CPSB_Eth0, CPSB_Eth1, PCIH_Eth0 or PCIH_Eth1 when further
//    discovery is required. AllItemsProcessed when all information has been
//    acquired and set up in the DHCP server.
//******************************************************************************
APZ21250Configuration::DiscoveryItem APZ21250Configuration::nextDiscoveryItem()
{
   static bool isInvalidStateReported = false;

   DiscoveryItem nextItem = m_currentDiscoveryItem;

   switch(m_currentDiscoveryItem)
   {
   case CPSB_Eth0:
      if(isInEndState(m_CPSB_Eth0State))
      {
         if(!isInEndState(m_CPSB_Eth1State))
         {
            nextItem = CPSB_Eth1;
         }
         else if(!isInEndState(m_PCIH_Eth0State))
         {
            nextItem = PCIH_Eth0;
         }
         else if(!isInEndState(m_PCIH_Eth1State))
         {
            nextItem = PCIH_Eth1;
         }
         else
         {
            nextItem = AllItemsProcessed;
         }
      }
      break;

   case CPSB_Eth1:
      if(isInEndState(m_CPSB_Eth1State))
      {
         if(!isInEndState(m_PCIH_Eth0State))
         {
            nextItem = PCIH_Eth0;
         }
         else if(!isInEndState(m_PCIH_Eth1State))
         {
            nextItem = PCIH_Eth1;
         }
         else if(!isInEndState(m_CPSB_Eth0State))
         {
            nextItem = CPSB_Eth0;
         }
         else
         {
            nextItem = AllItemsProcessed;
         }
      }
      break;

   case PCIH_Eth0:
      if(isInEndState(m_PCIH_Eth0State))
      {
         if(!isInEndState(m_PCIH_Eth1State))
         {
            nextItem = PCIH_Eth1;
         }
         else if(!isInEndState(m_CPSB_Eth0State))
         {
            nextItem = CPSB_Eth0;
         }
         else if(!isInEndState(m_CPSB_Eth1State))
         {
            nextItem = CPSB_Eth1;
         }
         else
         {
            nextItem = AllItemsProcessed;
         }
      }
      break;

   case PCIH_Eth1:
      if(isInEndState(m_PCIH_Eth1State))
      {
         if(!isInEndState(m_CPSB_Eth0State))
         {
            nextItem = CPSB_Eth0;
         }
         else if(!isInEndState(m_CPSB_Eth1State))
         {
            nextItem = CPSB_Eth1;
         }
         else if(!isInEndState(m_PCIH_Eth0State))
         {
            nextItem = PCIH_Eth0;
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
//    Raises the PTB connection alarm if not already raised.
//******************************************************************************
void APZ21250Configuration::raisePTBConnectionAlarm()
{
   if(!m_isConnectionAlarmSet)
   {
      // Raise this problem as an alarm that has to be attended.
      if(ALARM_A2(EventReporter::PTBConnectionAlarm,
                  PROBLEM_DATA("It is not possible to establish a TCP connection with IP %d.%d.%d.%d, port %d using %s\n%s",
                               (m_PTBIPAddress >> 24) & 0xFF,
                               (m_PTBIPAddress >> 16) & 0xFF,
                               (m_PTBIPAddress >> 8) & 0xFF,
                               m_PTBIPAddress & 0xFF,
                               m_PTBPort,
                               name(),
                               DHCP_CONFIG_DISCLAIMER),
                  PROBLEM_TEXT("CONNECTION\n%s",
                               name()),
                  PROBABLE_CAUSE(ALARM_TEXT_PTB_CONNECTION)) == EventReporter::OK)
      {
         m_isConnectionAlarmSet = true;
      }
   }
}

//******************************************************************************
// Description:
//    Ceases the PTB connection alarm if raised.
//******************************************************************************
void APZ21250Configuration::ceasePTBConnectionAlarm()
{
   if(m_isConnectionAlarmSet)
   {
      // Cease the issued alarm.
      if(CEASING(EventReporter::PTBConnectionAlarm,
                 PROBLEM_DATA("Not an issue any longer"),
                 PROBLEM_TEXT("CONNECTION\n%s",
                              name()),
                 PROBABLE_CAUSE(ALARM_TEXT_PTB_CONNECTION)) == EventReporter::OK)
      {
         m_isConnectionAlarmSet = false;
      }
   }
}

//******************************************************************************
// Description:
//    Raises the invalid CPTREADMAUREG alarm if not already raised.
// Parameters:
//    orderCode       The order code that was requested.
//    cptreadmauregr  The received CPTREADMAUREGR signal that is considered
//                    to be faulty.
//    discoveryState  Will be changed to Stuck.
//    discoveryItem   The DiscoveryItem for which the alarm should be raised.
//******************************************************************************
void APZ21250Configuration::raiseInvalidCPTREADMAUREGRAlarm(CPTREADMAUREG::OrderCode orderCode,
                                                            const CPTREADMAUREGR* cptreadmauregr,
                                                            DiscoveryState* discoveryState,
                                                            DiscoveryItem discoveryItem)
{
   static bool isInvalidDiscoveryItemReported = false;

   // We cannot get out of this state until the TCP PTB connection is
   // re-established.
   *discoveryState = Stuck;

   if(discoveryItem == CPSB_Eth0 ||
      discoveryItem == CPSB_Eth1 ||
      discoveryItem == PCIH_Eth0 ||
      discoveryItem == PCIH_Eth1)
   {
      if(!m_isInvalidCPTREADMAUREGRAlarmSet[discoveryItem])
      {
         // Raise this problem as an alarm that has to be attended.
         if(ALARM_A2(EventReporter::InvalidCPTREADMAUREGRAlarm,
                     PROBLEM_DATA("The reply to the sent CPTREADMAUREG is not valid, %s\n"
                                  "Signal number: %d, should be %d\n"
                                  "Fault code: %d, should be 0\n"
                                  "Order code: %d, expected %d\n"
                                  "Number of data: %d, should be 14",
                                  DHCP_CONFIG_DISCLAIMER,
                                  cptreadmauregr->signalNumber(),
                                  CPTREADMAUREGR::SignalNumber,
                                  cptreadmauregr->faultCode(),
                                  cptreadmauregr->orderCode(),
                                  orderCode,
                                  cptreadmauregr->numberOfData()),
                     PROBLEM_TEXT("CONNECTION\n%s\n\n"
                                  "ORDERCODE\n%d",
                                  name(),
                                  orderCode),
                     PROBABLE_CAUSE(ALARM_TEXT_INVALID_CPTREADMAUREGR)) == EventReporter::OK)
         {
            m_isInvalidCPTREADMAUREGRAlarmSet[discoveryItem] = true;
            m_invalidCPTREADMAUREGRAlarmOrderCode[discoveryItem] = orderCode;
         }
      }
   }
   else
   {
      if(!isInvalidDiscoveryItemReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("(%s) The provided discovery item for which an alarm should be raised is not valid (%d)",
                            name(),
                            discoveryItem),
               PROBLEM_TEXT("")));

         isInvalidDiscoveryItemReported = true;
      }
   }
}

//******************************************************************************
// Description:
//    Ceases the invalid CPTREADMAUREG alarm if raised.
// Parameters:
//    discoveryItem  The DiscoveryItem for which the alarm should be ceased.
//******************************************************************************
void APZ21250Configuration::ceaseInvalidCPTREADMAUREGRAlarm(DiscoveryItem discoveryItem)
{
   static bool isInvalidDiscoveryItemReported = false;

   if(discoveryItem == CPSB_Eth0 ||
      discoveryItem == CPSB_Eth1 ||
      discoveryItem == PCIH_Eth0 ||
      discoveryItem == PCIH_Eth1)
   {
      if(m_isInvalidCPTREADMAUREGRAlarmSet[discoveryItem])
      {
         // Cease the issued alarm.
         if(CEASING(EventReporter::InvalidCPTREADMAUREGRAlarm,
                    PROBLEM_DATA("Not an issue any longer"),
                    PROBLEM_TEXT("CONNECTION\n%s\n\n"
                                 "ORDERCODE\n%d",
                                 name(),
                                 m_invalidCPTREADMAUREGRAlarmOrderCode[discoveryItem]),
                    PROBABLE_CAUSE(ALARM_TEXT_INVALID_CPTREADMAUREGR)) == EventReporter::OK)
         {
            m_isInvalidCPTREADMAUREGRAlarmSet[discoveryItem] = false;
         }
      }
   }
   else
   {
      if(!isInvalidDiscoveryItemReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("(%s) The provided discovery item for which an alarm should be ceased is not valid (%d)",
                            name(),
                            discoveryItem),
               PROBLEM_TEXT("")));

         isInvalidDiscoveryItemReported = true;
      }
   }
}

//******************************************************************************
// Description:
//    Rasies the parse CPTREADMAUREGR alarm.
// Parameters:
//    orderCode       The order code that was requested.
//    cptreadmauregr  The received CPTREADMAUREGR signal that is considered
//                    to be faulty.
//    discoveryState  Will be changed to Stuck.
//    discoveryItem   The DiscoveryItem for which the alarm should be raised.
//******************************************************************************
void APZ21250Configuration::raiseParseCPTREADMAUREGRErrorAlarm(CPTREADMAUREG::OrderCode orderCode,
                                                               const CPTREADMAUREGR* cptreadmauregr,
                                                               DiscoveryState* discoveryState,
                                                               DiscoveryItem discoveryItem)
{
   static bool isInvalidDiscoveryItemReported = false;

   // We cannot get out of this state until the TCP PTB connection is
   // re-established.
   *discoveryState = Stuck;

   if(discoveryItem == CPSB_Eth0 ||
      discoveryItem == CPSB_Eth1 ||
      discoveryItem == PCIH_Eth0 ||
      discoveryItem == PCIH_Eth1)
   {
      const char* macAddress = cptreadmauregr->MACAddress();

      if(!m_isParseCPTREADMAUREGRAlarmSet[discoveryItem])
      {
         // Raise the issued alarm.
         if(ALARM_A2(EventReporter::ParseCPTREADMAUREGRAlarm,
                     PROBLEM_DATA("The received MAC address data, requested with order code %d, cannot be parsed (probably because it does not follow the outlined specification)\n%s\n"
                                  "MAC address byte 00: H'%X\n"
                                  "MAC address byte 01: H'%X\n"
                                  "MAC address byte 02: H'%X\n"
                                  "MAC address byte 03: H'%X\n"
                                  "MAC address byte 04: H'%X\n"
                                  "MAC address byte 05: H'%X\n"
                                  "MAC address byte 06: H'%X\n"
                                  "MAC address byte 07: H'%X\n"
                                  "MAC address byte 08: H'%X\n"
                                  "MAC address byte 09: H'%X\n"
                                  "MAC address byte 10: H'%X\n"
                                  "MAC address byte 11: H'%X\n"
                                  "MAC address byte 12: H'%X\n"
                                  "MAC address byte 13: H'%X",
                                  orderCode,
                                  DHCP_CONFIG_DISCLAIMER,
                                  macAddress[0], macAddress[1],
                                  macAddress[2], macAddress[3],
                                  macAddress[4], macAddress[5],
                                  macAddress[6], macAddress[7],
                                  macAddress[8], macAddress[9],
                                  macAddress[10], macAddress[11],
                                  macAddress[12], macAddress[13]),
                     PROBLEM_TEXT("CONNECTION\n%s\n\n"
                                  "ORDERCODE\n%d",
                                  name(),
                                  orderCode),
                     PROBABLE_CAUSE(ALARM_TEXT_PARSE_CPTREADMAUREGR)) == EventReporter::OK)
         {
            m_isParseCPTREADMAUREGRAlarmSet[discoveryItem] = true;
            m_parseCPTREADMAUREGRAlarmOrderCode[discoveryItem] = orderCode;
         }
      }
   }
   else
   {
      if(!isInvalidDiscoveryItemReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("(%s) The provided discovery item for which an alarm should be raised is not valid (%d)",
                            name(),
                            discoveryItem),
               PROBLEM_TEXT("")));

         isInvalidDiscoveryItemReported = true;
      }
   }
}

//******************************************************************************
// Description:
//    Ceases the parse CPTREADMAUREGR alarm.
// Parameters:
//    discoveryItem  The DiscoveryItem for which the alarm should be ceased.
//******************************************************************************
void APZ21250Configuration::ceaseCPTREADMAUREGRParseErrorAlarm(DiscoveryItem discoveryItem)
{
   static bool isInvalidDiscoveryItemReported = false;

   if(discoveryItem == CPSB_Eth0 ||
      discoveryItem == CPSB_Eth1 ||
      discoveryItem == PCIH_Eth0 ||
      discoveryItem == PCIH_Eth1)
   {
      if(m_isParseCPTREADMAUREGRAlarmSet[discoveryItem])
      {
         // Cease the issued alarm.
         if(CEASING(EventReporter::ParseCPTREADMAUREGRAlarm,
                    PROBLEM_DATA("Not an issue any longer"),
                    PROBLEM_TEXT("CONNECTION\n%s\n\n"
                                 "ORDERCODE\n%d",
                                 name(),
                                 m_parseCPTREADMAUREGRAlarmOrderCode[discoveryItem]),
                    PROBABLE_CAUSE(ALARM_TEXT_PARSE_CPTREADMAUREGR)) == EventReporter::OK)
         {
            m_isParseCPTREADMAUREGRAlarmSet[discoveryItem] = false;
         }
      }
   }
   else
   {
      if(!isInvalidDiscoveryItemReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("(%s) The provided discovery item for which an alarm should be ceased is not valid (%d)",
                            name(),
                            discoveryItem),
               PROBLEM_TEXT("")));

         isInvalidDiscoveryItemReported = true;
      }
   }
}

//******************************************************************************
// Description:
//    Sends data to the destination PTB peer.
// Parameters:
//    data      The data that should be sent.
//    dataSize  The number of bytes that data points to.
// Returns:
//    OK if the data was sent successfully, NotOK if we got disconnected in
//    the middle of the sending process.
//******************************************************************************
APZ21250Configuration::Status APZ21250Configuration::send(const char* data,
                                                          u_int32 dataSize)
{
   u_int32 sentSize = 0;
   u_int32 size;
   TCPClient::Status theStatus;
   
   do
   {
      // Send the CPTREADMAUREG signal to the destination PTB peer.
      theStatus = m_ptbConnection.send(&data[sentSize],
                                       dataSize - sentSize,
                                       size);

      switch(theStatus)
      {
      case TCPClient::NotOK:
         if(size > 0)
         {
            // Add to the already sent amount of data.
            sentSize += size;
         }
         else
         {
            // It is not possible to send the data we want. Terminate the
            // connection if some data was sent (we need to get back on
            // track again and cannot reuse this TCP connection unless all
            // data has been successfully transferred). If however no data
            // was sent, then just keep on trying.
            if(sentSize != 0)
            {
               // TCP connection termination required.
               m_ptbConnection.disconnect();
               return NotOK;
            }
         }
         break;

      case TCPClient::OK:
         // All data should have been delivered by now.
         sentSize += size;
         break;

      default:
         if(!m_isSendProgrammingErrorReported)
         {
            m_isSendProgrammingErrorReported = true;

            static_cast<void>(EVENT(EventReporter::ProgrammingError,
                  PROBLEM_DATA("Programming error, not all return values taken into account (%d)",
                               theStatus),
                  PROBLEM_TEXT("")));
         }

         // Note that the absence of a break statement here is
         // intential to use the same action as in the
         // TCPClient::Disconnected case.

      case TCPClient::Disconnected:
         m_ptbConnection.disconnect();
         return NotOK;
      }
   }while(sentSize != dataSize);

   return OK;
}

//******************************************************************************
// Description:
//    Receives data from the source PTB peer.
// Parameters:
//    data      An area of MaxSize bytes where read data should be stored.
//    maxSize   The amount of bytes allocated by data.
//    readSize  Is set upon return to the number of read bytes.
// Returns:
//    OK if the CPTREADMAUREG signal was sent successfully, NotOK if we got
//    disconnected in the middle of the sending process.
//******************************************************************************
APZ21250Configuration::Status APZ21250Configuration::read(char* data,
                                                          u_int32 maxSize,
                                                          u_int32& readSize)
{
   // Destroy the data area before reading to catch problems with reading
   // data that was not received from the source PTB peer.
   memset(data, 0xFF, maxSize);

   readSize = 0;
   u_int32 size;
   TCPClient::Status theStatus;

   // Begin with reading the CPT signal length identifier.
   do
   {
      // Read the length indicator data from the PTB peer.
      theStatus = m_ptbConnection.poll(data, 1, size);

      switch(theStatus)
      {
      case TCPClient::NotOK:
         if(size > 0)
         {
            // Add to the already sent amount of data.
            readSize += size;
         }
         else
         {
            // The connection has been severed.
            m_ptbConnection.disconnect();
            return NotOK;
         }
         break;

      case TCPClient::OK:
         // Data has been read
         readSize += size;
         break;

      default:
         if(!m_isReadProgrammingErrorReported)
         {
            m_isReadProgrammingErrorReported = true;

            static_cast<void>(EVENT(EventReporter::ProgrammingError,
                  PROBLEM_DATA("Programming error, not all return values taken into account (%d)",
                               theStatus),
                  PROBLEM_TEXT("")));
         }

         // Note that the absence of a break statement here is
         // intential to use the same action as in the
         // TCPClient::Disconnected case.

      case TCPClient::Disconnected:
         m_ptbConnection.disconnect();
         return NotOK;
      }
   }while(readSize != 1);

   // Note: When reading data from the source PTB peer, the length indicator
   //       does not include itself in the byte count.
   u_int8 signalSize = *data - 1;
   readSize = 0;

   do
   {
      // Read data from the PTB peer.
      theStatus = m_ptbConnection.poll(&data[readSize + 1],
                                       signalSize - readSize,
                                       size);

      switch(theStatus)
      {
      case TCPClient::NotOK:
         if(size > 0)
         {
            // Add to the already sent amount of data.
            readSize += size;
         }
         else
         {
            // The connection has been severed.
            m_ptbConnection.disconnect();
         }
         break;

      case TCPClient::OK:
         // Data has been read
         readSize += size;
         break;

      default:
         if(!m_isReadProgrammingErrorReported)
         {
            m_isReadProgrammingErrorReported = true;

            static_cast<void>(EVENT(EventReporter::ProgrammingError,
                  PROBLEM_DATA("Programming error, not all return values taken into account (%d)",
                               theStatus),
                  PROBLEM_TEXT("")));
         }

         // Note that the absence of a break statement here is
         // intential to use the same action as in the
         // TCPClient::Disconnected case.

      case TCPClient::Disconnected:
         m_ptbConnection.disconnect();
         return NotOK;
      }
   }while(readSize != signalSize);

   return OK;
}

//******************************************************************************
// Description:
//    Sends a CPTREADMAUREG signal with the supplied order code to the
//    destination PTB peer.
// Parameters:
//    orderCode  Defines which MAC address that is of interest.
// Returns:
//    OK if the CPTREADMAUREG signal was sent successfully, NotOK if we got
//    disconnected in the middle of the sending process.
//******************************************************************************
APZ21250Configuration::Status APZ21250Configuration::sendCPTREADMAUREG(CPTREADMAUREG::OrderCode orderCode)
{
   CPTREADMAUREG::CPSide cpSide = (m_PTBPeer == PTBA ? CPTREADMAUREG::CP_A : CPTREADMAUREG::CP_B);
   CPTREADMAUREG cptreadmaureg(orderCode, cpSide);

   return send(cptreadmaureg.message(), cptreadmaureg.size());
}

//******************************************************************************
// Description:
//    Attempts to read a CPTREADMAUREG signal that should match the supplied
//    orderCodewith the supplied order code to the
//    destination PTB peer.
// Parameters:
//    orderCode          Defines which MAC address that is expected.
//    clientInformation  The ClientInformation instance that should be updated
//                       with the received data if it is a match.
//    discoveryState     Will change to ConfigureDHCP if a match is found.
// Returns:
//    NotOK if we got disconnected in the middle of the receiving process.
//******************************************************************************
APZ21250Configuration::Status APZ21250Configuration::readCPTREADMAUREG(CPTREADMAUREG::OrderCode orderCode,
                                                                       ClientInformation* clientInformation,
                                                                       DiscoveryState* discoveryState)
{
   const int MaxSize = 1000;
   char readData[MaxSize];
   u_int32 readSize;
   bool isExpectedSignalFound = false;
   CPTREADMAUREGR* cptreadmauregr;

   // Read until we have received a CPTREADMAUREG signal or we get
   // disconnected.
   do
   {
      // Read one CPT signal from the source PTB peer.
      if(read(readData, MaxSize, readSize) != OK)
      {
         return NotOK;
      }

      // Data was read successfully from the source PTB peer. Did we receive a
      // CPTREADMAUREGR signal?
      cptreadmauregr = reinterpret_cast<CPTREADMAUREGR*>(readData);

      isExpectedSignalFound = (cptreadmauregr->signalNumber() == CPTREADMAUREGR::SignalNumber ? true : false);

      if(!isExpectedSignalFound)
      {
         DBGLOG("(s) Waited for a CPTREADMAUREGR signal but received %d (ignored)\n", name(), cptreadmauregr->signalNumber());
      }
   }while(!isExpectedSignalFound);

   // Proceed to validation of the reply.

   // Transfer the received MAC address to the clientInformation instance
   // currently in use if the reply is considered valid.
   if(cptreadmauregr->signalNumber() == CPTREADMAUREGR::SignalNumber &&
      cptreadmauregr->faultCode() == CPTREADMAUREGR::NoFault &&
      cptreadmauregr->orderCode() == orderCode &&
      cptreadmauregr->numberOfData() == 14) // A valid reply always has 14 byte worth of data.
   {
      // Cease a previously alarmed state (if required).
      ceaseInvalidCPTREADMAUREGRAlarm(m_currentDiscoveryItem);

      u_char address[6];
      int a, b, c, d, e, f;

      // The received MAC address is on the form 0xaabbccddeeff (string).
      int converted = sscanf(&cptreadmauregr->MACAddress()[2],
                              "%2x%2x%2x%2x%2x%2x",
                              &a, &b, &c, &d, &e, &f);

      // Verify that we were able to interpret the data correctly.
      if(converted == 6)
      {
         // Cease the parse CPTREADMAUREGR alarm (if required).
         ceaseCPTREADMAUREGRParseErrorAlarm(m_currentDiscoveryItem);

         address[0] = a;
         address[1] = b;
         address[2] = c;
         address[3] = d;
         address[4] = e;
         address[5] = f;

         // Copy the MAC address to the current client information container.
         DBGLOG("(%s) Read MAC address %02X:%02X:%02X:%02X:%02X:%02X for %s is assigned to client IP %d.%d.%d.%d\n",
                name(),
                a, b, c, d, e, f,
                (orderCode == CPTREADMAUREG::CPSB_ETH0 ? "CPSB_Eth0" : orderCode == CPTREADMAUREG::CPSB_ETH1 ? "CPSB_Eth1" : orderCode == CPTREADMAUREG::PCIH_OWN ? "PCIH_Own" : orderCode == CPTREADMAUREG::PCIH_TWIN ? "PCIH_Twin" : "Unknown"),
                (clientInformation->ipAddress() >> 24) & 0xFF, (clientInformation->ipAddress() >> 16) & 0xFF, (clientInformation->ipAddress() >> 8) & 0xFF, clientInformation->ipAddress() & 0xFF);

         // Add the read MAC address to the correct ClientInformation container.
         clientInformation->macAddress(MACAddress(address, 6));

         // Advance to the next state of operation.
         *discoveryState = ConfigureDHCP;
      }
      else
      {
         // Raise the parse CPTREADMAUREGR alarm.
         raiseParseCPTREADMAUREGRErrorAlarm(orderCode, cptreadmauregr, discoveryState, m_currentDiscoveryItem);
      }
   }
   else
   {
      // Raise the invalid CPTREADMAUREGR alarm.
      raiseInvalidCPTREADMAUREGRAlarm(orderCode, cptreadmauregr, discoveryState, m_currentDiscoveryItem);
   }

   return OK;
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
APZ21250Configuration::Status APZ21250Configuration::configureDHCP(ClientInformation* clientInformation)
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
