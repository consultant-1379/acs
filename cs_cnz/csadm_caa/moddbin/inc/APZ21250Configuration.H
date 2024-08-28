//******************************************************************************
// 
// .NAME
//    APZ21250Configuration - Sets up DHCP information specific to APZ 212 50.
// .LIBRARY 3C++
// .PAGENAME APZ21250Configuration
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
//  2005-06-10 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef APZ21250CONFIGURATION_H
#define APZ21250CONFIGURATION_H

#include "APZConfiguration.H"
#include "DHCPConfiguration.H"
#include "ClientInformation.H"
#include "TCPClient.hxx"
#include "CPTSignals.H"

class APZ21250Configuration : public APZConfiguration
{
public:
   enum PTBPeer
   {
      PTBA,
      PTBB
   };

   // This is the correct information to use in the product.
   static const u_int32 PTBAIPAddress = 0xc0a8a97f; // 192.168.169.127
   static const u_int32 PTBBIPAddress = 0xc0a8aa7f; // 192.168.170.127
   static const u_int16 PTBAPort      = 3000;
   static const u_int16 PTBBPort      = 3000;

   // Constructor.
   APZ21250Configuration(const char* name, PTBPeer ptbPeer, u_int32 DHCPServer, u_int32 IPAddress, u_int16 port);

   // Destructor.
   virtual ~APZ21250Configuration();

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
   // The netmask for the networks in an APZ 212 50 environment is always
   // 255.255.255.0
   static const u_int32 Netmask = 0xFFFFFF00;

   // 192.168.169.128
   static const u_int32 Eth0_IPAddress_CPSB_A = 0xc0a8a980;

   // 192.168.170.128
   static const u_int32 Eth1_IPAddress_CPSB_A = 0xc0a8aa80;

   // 192.168.170.129
   static const u_int32 Eth0_IPAddress_CPSB_B = 0xc0a8aa81;

   // 192.168.169.129
   static const u_int32 Eth1_IPAddress_CPSB_B = 0xc0a8a981;

   // 192.168.169.130
   static const u_int32 Eth0_IPAddress_PCIH_A = 0xc0a8a982;

   // 192.168.170.130
   static const u_int32 Eth1_IPAddress_PCIH_A = 0xc0a8aa82;

   // 192.168.170.131
   static const u_int32 Eth0_IPAddress_PCIH_B = 0xc0a8aa83;

   // 192.168.169.131
   static const u_int32 Eth1_IPAddress_PCIH_B = 0xc0a8a983;

   // 192.168.169.132
   static const u_int32 Eth0_GESB_I_A_Address = 0xc0a8a984;

   // 192.168.169.133
   static const u_int32 Eth0_GESB_I_B_Address = 0xc0a8a985;

   // 192.168.169.134
   static const u_int32 Eth0_GESB_E_A_Address = 0xc0a8a986;

   // 192.168.169.135
   static const u_int32 Eth0_GESB_E_B_Address = 0xc0a8a987;

   // 192.168.170.132
   static const u_int32 Eth1_GESB_I_A_Address = 0xc0a8aa84;

   // 192.168.170.133
   static const u_int32 Eth1_GESB_I_B_Address = 0xc0a8aa85;

   // 192.168.170.134
   static const u_int32 Eth1_GESB_E_A_Address = 0xc0a8aa86;

   // 192.168.170.135
   static const u_int32 Eth1_GESB_E_B_Address = 0xc0a8aa87;

   // High IP range in the scopes. (192.168.169/170.)135
   // Note, the lower one is the same as (Eth0_IPAddress_CPSB_A & 0xFF)
   static const u_int32 HighIPRange = 135;

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
   // exist for the CPSB's Eth0 and Eth1 as well as the PCIH's Eth0 and Eth1.
   enum DiscoveryState
   {
      SendMACAddressRequest,
      ReadMACAddressReply,
      ConfigureDHCP,
      AllDone,

      // If the received CPTREADMAUREGR signal is not valid then we end up in
      // the Stuck state for the current DiscoveryState instance. This state
      // is preserved during the lifetime of the current TCP connection with
      // the PTB peer that produced the error.
      Stuck
   };

   // These values define which node is currently being processed.
   enum DiscoveryItem
   {
      CPSB_Eth0 = 0,
      CPSB_Eth1,
      PCIH_Eth0,
      PCIH_Eth1,
      NumberOfDiscoveryItems,
      AllItemsProcessed
   };

   // Assigns IP addresses and boot filenames to all clients whose MAC address
   // information will be read and set up in the DHCP server.
   void assignStaticClientInformation();

   // Assigns static DHCP entries for the GESB blades.
   Status addStaticGESBEntries();

   // Should decrease the automatic revert timer for the specific
   // ClientInformation instance and perform an automatic MODD revert
   // operation if the timer reaches zero.
   void decreaseRevertTimer(u_int16 elapsedSeconds, ClientInformation& clientInformation);

   // Request automatic MODD Revert operation for the supplied
   // ClientInformation instance if its automatic revert timer has expired.
   void automaticRevert(ClientInformation& clientInformation);

   // Establishes a TCP connection with the PTB peer.
   Status connectToPTB();

   // Reads MAC address information from the PTB peer and adds client specific
   // information to the DHCP server.
   Status setupDHCPInformtion();

   // Keeps the PTB connection alive by sending CPTKEEPALIVE signals at a
   // frequency of approximately 5 seconds. The method does not return until
   // the PTB connection has been severed.
   void keepAlive();

   // Clears all information about MAC address to IP address translations
   // along with state information to ensure that all MAC address information
   // is read from scratch again.
   void clearSetupDHCPInformation();

   // Determines what discovery item to process next.
   DiscoveryItem nextDiscoveryItem();

   // Raises the PTB connection alarm if not already raised.
   void raisePTBConnectionAlarm();

   // Ceases the PTB connection alarm if raised.
   void ceasePTBConnectionAlarm();

   // Raises the invalid CPTREADMAUREG alarm if not already raised.
   void raiseInvalidCPTREADMAUREGRAlarm(CPTREADMAUREG::OrderCode orderCode,
                                        const CPTREADMAUREGR* cptreadmauregr,
                                        DiscoveryState* discoveryState,
                                        DiscoveryItem discoveryItem);

   // Ceases the invalid CPTREADMAUREG alarm if raised.
   void ceaseInvalidCPTREADMAUREGRAlarm(DiscoveryItem discoveryItem);

   // Rasies the parse CPTREADMAUREGR alarm.
   void raiseParseCPTREADMAUREGRErrorAlarm(CPTREADMAUREG::OrderCode orderCode,
                                           const CPTREADMAUREGR* cptreadmauregr,
                                           DiscoveryState* discoveryState,
                                           DiscoveryItem discoveryItem);

   // Ceases the parse CPTREADMAUREGR alarm.
   void ceaseCPTREADMAUREGRParseErrorAlarm(DiscoveryItem discoveryItem);

   // Sends data to the destination PTB peer.
   Status send(const char* data, u_int32 dataSize);

   // Receives data from the source PTB peer.
   Status read(char* data, u_int32 maxSize, u_int32& readSize);

   // Sends a CPTREADMAUREG signal with the supplied order code to the
   // destination PTB peer.
   Status sendCPTREADMAUREG(CPTREADMAUREG::OrderCode orderCode);

   // Attempts to read a CPTREADMAUREG signal that should match the supplied
   // orderCodewith the supplied order code to the
   // destination PTB peer.
   Status readCPTREADMAUREG(CPTREADMAUREG::OrderCode orderCode,
                            ClientInformation* clientInformation,
                            DiscoveryState* discoveryState);

   // Verifies if the supplied state is valid for further execution or not.
   bool isInEndState(DiscoveryState state);

   // Adds the client information data to the DHCP server.
   Status configureDHCP(ClientInformation* clientInformation);

   // Used when reading or modifying DHCP server information.
   DHCPConfiguration m_dhcpConfiguration;

   // Client information read from PTB for interface Eth0 for the CPSB.
   ClientInformation m_CPSB_Eth0;

   // Client information read from PTB for interface Eth1 for the CPSB.
   ClientInformation m_CPSB_Eth1;

   // Client information read from PTB for interface Eth0 for the PCIG.
   ClientInformation m_PCIH_Eth0;

   // Client information read from PTB for interface Eth1 for the PCIH.
   ClientInformation m_PCIH_Eth1;

   // The PTB peer that this specific instance should set up the DHCP server
   // for.
   const PTBPeer m_PTBPeer;

   // The IP address to use when accessing the PTB.
   const u_int32 m_PTBIPAddress;

   // The port number to use when accessing the PTB.
   const u_int16 m_PTBPort;

   // The current state of the setup DHCP process.
   State m_state;

   // Handle to the PTB TCP connection.
   TCPClient m_ptbConnection;

   // Keeps track of whether the Client's lease periods have been set up or
   // not.
   bool m_isClientLeaseConfigured;

   // Tracks whether or not a detecter programming error in the keepAlive
   // method has been logged.
   bool m_isKeepAliveProgrammingErrorLogged;

   // Tracks whether an invalid Discovery item has been logged or not.
   bool m_isInvalidDiscoveryItemLogged;

   // Tracks whether sending a CPTREADMAUREAG has failed or not.
   bool m_isSendProblemReported;

   // Tracks whether receiving a CPTREADMAUREG has failed or not.
   bool m_isReadProblemReported;

   // Tracks whether the sending of CPT signals has reported a programming
   // error or not.
   bool m_isSendProgrammingErrorReported;

   // Tracks whether the reception of CPT signals has reported a programming
   // error or not.
   bool m_isReadProgrammingErrorReported;

   // Tracks whether a received CPTREADMAUREGR signal has been reported as
   // invalid or not.
   bool m_isInvalidCPTREADMAUREGRReported;

   // Boot IP address for clients on Lan 1.
   const u_int32 m_BootIPAddress_Lan1;

   // Boot IP address for clients on Lan 2.
   const u_int32 m_BootIPAddress_Lan2;

   // States information about the current progress of requesting MAC address
   // information and configuring the DHCP server.
   DiscoveryState m_CPSB_Eth0State;
   DiscoveryState m_CPSB_Eth1State;
   DiscoveryState m_PCIH_Eth0State;
   DiscoveryState m_PCIH_Eth1State;

   // Tells which node's information that is currently being processed.
   DiscoveryItem m_currentDiscoveryItem;

   // Tracks whether an alarm has been raised because we cannot connect to the
   // destination PTB peer.
   bool m_isConnectionAlarmSet;

   // Tracks whether an alarm has been raised because we received an invalid
   // reply to a CPTREADMAUREG signal.
   bool m_isInvalidCPTREADMAUREGRAlarmSet[NumberOfDiscoveryItems];

   // Tracks whether an alarm has been raised because the received MAC address
   // data in a CPTREADMAUREGR signal does not conform with specifications.
   bool m_isParseCPTREADMAUREGRAlarmSet[NumberOfDiscoveryItems];

   // These two members are used to keep track of the order code for which a
   // an alarm has been raised for a specific order code. This information is
   // then later on used when ceasing the alarm to be able to use the same
   // order code information in the ceasing process.
   CPTREADMAUREG::OrderCode m_invalidCPTREADMAUREGRAlarmOrderCode[NumberOfDiscoveryItems];
   CPTREADMAUREG::OrderCode m_parseCPTREADMAUREGRAlarmOrderCode[NumberOfDiscoveryItems];

   // Tracks whether an invalid state in MT unsafe 
   // setupPeerConnectionImplementation() method already has been reported or not.
   // Note, because of the MT unsafe method, this member is needed. 
   // (Normally, is a local static bool isInvalidStateReported parameter used).
   bool m_isInvalidStateReportedForSetupPeerConnectionImplementation;

   // Keeps track of whether the DHCP IgnoreBroadcastFlag in registry
   // has been checked or not.
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
inline bool APZ21250Configuration::isInEndState(DiscoveryState state)
{
   return (state == AllDone) || (state == Stuck);
}

#endif
