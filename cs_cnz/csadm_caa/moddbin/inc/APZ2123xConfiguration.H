//******************************************************************************
// 
// .NAME
//    APZ2123xConfiguration - Sets up DHCP information specific to APZ 212 50.
// .LIBRARY 3C++
// .PAGENAME APZ2123xConfiguration
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
//  Defines the required interface for configuration of a specific CP type.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2006-04-19 by EAB/AZA/DG Dan Andersson

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef APZ2123XCONFIGURATION_H
#define APZ2123XCONFIGURATION_H

#include "APZConfiguration.H"
#include "DHCPConfiguration.H"
#include "ClientInformation.H"

class APZ2123xConfiguration : public APZConfiguration
{
public:
   // Constructor.
   APZ2123xConfiguration(const char* name, u_int32 DHCPServer);

   // Destructor.
   virtual ~APZ2123xConfiguration();

protected:
   // Checks the basic DHCP setup. That the scopes, IPRange and excluded ranges
   // are correct, otherwise they all will be created from scratch
   virtual Operation checkBasicDHCPSetupImplementation();
   
   // Cleans up the DHCP setup. 
   virtual Operation cleanUpDHCPSetupImplementation();
   
   // Sets the instance in a well defined state to be able to commence setting
   // up the required information in the DHCP server.
   virtual Operation initializeImplementation();

   // This method should establish an APZ PTB peer connection.
   virtual Operation setupPeerConnectionImplementation();

   // This method should acquire MAC address information and construct MAC
   // address to IP address mappings along with setting up this information in
   // the DHCP server.
   virtual Operation setupDHCPImplementation();

   // Provides a possibility to continue executing without member data access.
   virtual Operation finalizeImplementation();

   // Returns a pointer to a ClientInformation instance if a match for
   // the provided MAC address is found, otherwise NULL should be returned.
   virtual ClientInformation* getClientInformationImplementation(const MACAddress& macAddress);

   // Decreases the automatic revert timer for all ClientInformation
   // instances.
   virtual void decreaseRevertTimerImplementation(u_int16 elapsedSeconds);

   // The ClientInformation instances for which the timer has expired should
   // perform an automatic MODD revert operation.
   virtual void automaticRevertImplementation();

   // Changes the boot filename for the supplied client.
   virtual Operation changeBootFilenameImplementation(const ClientInformation* client, u_int16 filenameLength, const char* filename);

   // An order that all raised alarms must be ceased because the process is
   // about to terminate has been received from PRC. Cease all alarms and
   // prepare to be terminated.
   virtual void ceaseAlarmsImplementation();

private:
   // The netmask for the networks in an APZ 212 3x environment is always
   // 255.255.255.0
   static const u_int32 Netmask = 0xFFFFFF00;

   // 192.168.169.128
   static const u_int32 Eth0_IPAddress_IPN0 = 0xc0a8a980;

   // 192.168.169.130
   static const u_int32 Eth0_IPAddress_IPN2 = 0xc0a8a982;

   // 192.168.170.128
   static const u_int32 Eth1_IPAddress_IPN1 = 0xc0a8aa80;

   // 192.168.170.130
   static const u_int32 Eth1_IPAddress_IPN3 = 0xc0a8aa82;

   // High IP range in the scopes. (192.168.169/170.)135
   // Note, the lower one is the same as (Eth0_IPAddress_IPN0 & 0xFF)
   static const u_int32 HighIPRange = 135;

   // Excluded ranges used in scopes
   // 192.168.169/170.129 - 192.168.169/170.129
   // 192.168.169/170.131 - 192.168.169/170.135
   static const u_int32 ExcludeRangeOneLow = 129;
   static const u_int32 ExcludeRangeTwoLow = 131;
   static const u_int32 ExcludeRangeTwoHigh = 135;

   enum Status
   {
      OK,
      Executing,
      NotOK
   };

   enum State
   {
      InitialSetup,
      ConnectToPTB,
      SetupDHCPInformation,
      KeepAlive
   };

   // These states exist on a per physical node destination basis, that is one
   // exist for the IPN's Eth0 and Eth1.
   enum DiscoveryState
   {
      ConfigureDHCP,
      AllDone,
   };

   // These values define which node is currently being processed.
    enum DiscoveryItem
   {
      IPN0_Eth0 = 0,
      IPN1_Eth1,
      IPN2_Eth0,
      IPN3_Eth1,
      NumberOfDiscoveryItems,
      AllItemsProcessed
   };

   // Assigns IP addresses and boot filenames to all clients whose MAC address
   // information will be read and set up in the DHCP server.
   void assignStaticClientInformation();

   // Should decrease the automatic revert timer for the specific
   // ClientInformation instance and perform an automatic MODD revert
   // operation if the timer reaches zero.
   void decreaseRevertTimer(u_int16 elapsedSeconds, ClientInformation& clientInformation);

   // Request automatic MODD Revert operation for the supplied
   // ClientInformation instance if its automatic revert timer has expired.
   void automaticRevert(ClientInformation& clientInformation);

   // Checks the DHCP configuration
   Status checkConfiguration();

   // Sets the static MAC address information for the IPN's. Adds client specific
   // information to the DHCP server.
   Status setupDHCPInformtion();

   // Clears all information about MAC address to IP address translations
   // along with state information to ensure that all MAC address information
   // is read from scratch again.
   void clearSetupDHCPInformation();

   // Determines what discovery item to process next.
   DiscoveryItem nextDiscoveryItem();

   // Verifies if the supplied state is valid for further execution or not.
   bool isInEndState(DiscoveryState state);

   // Adds the client information data to the DHCP server.
   Status configureDHCP(ClientInformation* clientInformation);

   // Used when reading or modifying DHCP server information.
   DHCPConfiguration m_dhcpConfiguration;

   // Client information for interface Eth0 for the IPN0.
   ClientInformation m_IPN0_Eth0;

   // Client information for interface Eth1 for the IPN1.
   ClientInformation m_IPN1_Eth1;

   // Client information for interface Eth0 for the IPN2.
   ClientInformation m_IPN2_Eth0;

   // Client information for interface Eth1 for the IPN3.
   ClientInformation m_IPN3_Eth1;

   // The current state of the setup DHCP process.
   State m_state;

   // Keeps track of whether the Client's lease periods have been set up or
   // not.
   bool m_isClientLeaseConfigured;

   // Tracks whether an invalid Discovery item has been logged or not.
   bool m_isInvalidDiscoveryItemLogged;

   // Boot IP address for clients on Lan 1.
   const u_int32 m_BootIPAddress_Lan1;

   // Boot IP address for clients on Lan 2.
   const u_int32 m_BootIPAddress_Lan2;

   // States information about the current progress of requesting MAC address
   // information and configuring the DHCP server.
   DiscoveryState m_IPN0_Eth0State;
   DiscoveryState m_IPN1_Eth1State;
   DiscoveryState m_IPN2_Eth0State;
   DiscoveryState m_IPN3_Eth1State;

   // Tells which node's information that is currently being processed.
   DiscoveryItem m_currentDiscoveryItem;

   // Tracks whether an invalid state in MT unsafe 
   // setupPeerConnectionImplementation() method already has been reported or not.
   // Note, because of the MT unsafe method, this member is needed. 
   // (Normally, is a local static bool isInvalidStateReported parameter used).
   bool m_isInvalidStateReportedForSetupPeerConnectionImplementation;

   // Keeps track of whether the DHCP IgnoreBroadcastFlag in registry
   // has been set up or not.
   bool m_isDhcpRegistryConfigured;
};

//******************************************************************************
// Description:
//    Verifies if the supplied state is valid for further execution or not.
// Parameters:
//    state  The state that should be verified.
// Returns:
//    true if execution for this state machine is finished and execution of
//    another state machine should commence, otherwise false is returned.
//******************************************************************************
inline bool APZ2123xConfiguration::isInEndState(DiscoveryState state)
{
   return (state == AllDone); 
}

#endif
