//******************************************************************************
// 
// .NAME
//    MODDHandler - Assembles MAC address information from PTB peers and sets
//                  up the DHCP server with the assembled information.
// .LIBRARY 3C++
// .PAGENAME MODDHandler
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
//  Whenever a new connection is made to any PTB peer (PTB-A or PTB-B), the
//  MODDHandler retrieves MAC address informatin from the newly connected peer
//  and updates the contents of the local DHCP server with the assembled
//  information. Apart from setting up the DHCP server, the MODDHandler
//  provides the ability to ask to change the current boot image file
//  currently being used by a specific MAC address in the DHCP server.
//  The MODDHandler runs in three threads. Handling the PTB peers is divided
//  into handling one PTB per thread (a total of 2 MAC address discovery
//  threads) and one thread is handling new service requests apart from the
//  main thread that dispatches new service connections to the serviceThread.

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
#ifndef MODDHANDLER_H
#define MODDHANDLER_H

#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif // _MSC_VER

#include "ClientInformation.H"
#include "MODDSignal.H"
#include "MACAddress.H"
#include "TCPServer.hxx"
#include "TCPClient.hxx"
#include <list>

class APZConfiguration;

// Member functions, constructors, destructors, operators

class MODDHandler
{
public:
   enum Status
   {
      OK,
      Busy,
      NotOK
   };

   // Constructor
   MODDHandler(int CPType);

   // Destructor.
   ~MODDHandler();

   // Tells whether or not a created instance of MODDHandler will be able to
   // execute or not.
   Status status() const;

   // The start point for the MODDHandler process
   Status run();

   // Suspends the execution of all threads created by the MODDHandler.
   bool suspendThreads();

   // Tries to cease all alarms raised by the processing threads.
   void ceaseAlarms();

   // Resumes the execution of all threads created by the MODDHandler.
   bool resumeThreads();

   // Returns a pointer to the instance of the MODDHandler.
   static MODDHandler* instance();

   // Register an instance of an APZConfiguration.
   void registerAPZConfiguration(APZConfiguration* apzConfiguration);

private:
   enum APZType
   {
      APZUnknown  = 0,
      APZ2123x = 1,
      APZ21240 = 2,
      APZ21250 = 3
   };

   // Resolution is ms.
   static const DWORD OneSecond = 1000;

   // Resolution is ms.
   static const DWORD TransferTimeout = 3000;

   // Resolution is ms.
   static const DWORD ServiceRXTimeout = 5000;

   // 192.168.169.1
   static const u_int32 IPAddress1_A_Node = 0xc0a8a901;

   // 192.168.170.1
   static const u_int32 IPAddress2_A_Node = 0xc0a8aa01;

   // 192.168.169.2
   static const u_int32 IPAddress1_B_Node = 0xc0a8a902;

   // 192.168.170.2
   static const u_int32 IPAddress2_B_Node = 0xc0a8aa02;

   enum InternalStatus
   {
      Success,
      InterfaceSetupFailed,
      EventHandlesCreationFailed,
      UnsupportedCPType,
      NotInitialised
   };

   enum TransferStatus
   {
      TransferOK,
      Disconnected,
      Timeout
   };

   enum ExitCode
   {
      ExitCodeNoPTBA,
      ExitCodeNoPTBB,
      ExitCodeNoAvailableNetwork,
      ExitCodePRC,
      ExitCodeInvalidSetup,
      ExitCodeSubsystemError
   };

   enum ThreadStatus
   {
      NotInitialized,
      ThreadCreationSuccess,
      ThreadCreationFailure
   };

   // Creates the threads that are required for this application.
   Status createThreads();

   // Creates the required event handles for the lifetime of the process.
   Status createEventHandles();

   // Closes all event handles created by createEventHandles().
   void destroyEventHandles();

   // Handles the newly accepted connection and dispatches it to the
   // serviceThread if this is the only live service connection, in any other
   // case this newly accepted connection will be notified of the fact that we
   // are already busy.
   void handleNewConnection(TCPServer& listener);

   // Handles a newly accepted service connection.
   // This method should only be used in the context of the serviceThread.
   void handleServiceConnection();

   // Sends a MODDSignal using an existing TCP connection.
   TransferStatus sendMessage(TCPClient* connection, WSAEVENT* event, MODDSignal& signal, DWORD timeout);

   // Reads data based on the MODD protocol to the supplied data area.
   TransferStatus readMessage(TCPClient* connection, WSAEVENT* event, u_char* data, u_int32& dataSize, DWORD timeout);

   // Looks up the APZConfiguration instance that is handling the
   // ClientInformation instance associated with the supplied MAC address.
   // Should the supplied MAC address find a match, the output parameter
   // APZConfiguration is returned along with setting the output
   // parameter clientInformation to a valid data area.
   Status getClientAccessData(const MACAddress& macAddress, ClientInformation*& clientInformation, APZConfiguration*& apzConfiguration);

   // Decrease timers for all clients and perform automatic revert operations
   // if required.
   void checkTimerExpirations(u_int16 elapsedSeconds);

   // Returns the APZ type that should be set up in the DHCP server.
   APZType getAPZType() const;

   // Notifies the sender of the received signal that the MODD protocol
   // version that was received is not supported and suggests the version to
   // use instead.
   void notifySenderOfNotSupportedVersion(TCPClient* connection, WSAEVENT* event, const MODDSignal& receivedSignal);

   // Determines the port number associated with the supplied service.
   u_int16 getPort(const char* serviceName);

   // Returns the DHCP server IP address.
   u_int32 getDHCPServerAddress() const;

   // Creates the required APZConfiguration based on the APZ type.
   Status createAPZConfiguration();

   // The service thread start point.
   static void serviceThread(LPDWORD parameter);

   // The DHCP server setup thread start point, there will exist one such
   // thread per each APZ that should be configured in the DHCP server tables.
   static void setupDHCPServerThread(LPDWORD parameter);

   // The threads should be friends of us since they operate on our data.
   friend void serviceThread(LPDWORD parameter);
   friend void macAddressDiscoveryThread(LPDWORD parameter);

   // XXX This method exist for debug purposes only.
   void print(char* text, u_char* data, u_int32 size);

   // When the service port has accepted a connection which it will serve
   // the connection is transferred to this TCPClient.
   TCPClient m_serviceConnection;

   // TCP server end that listens on the ServicePort for connections on
   // network 1 from any service user.
   TCPServer m_serviceListenerNet1;

   // True when m_serviceListenerNet1 is bound to a valid address and can be
   // used to accept new service connections.
   bool m_isNetwork1Available;

   // TCP server end that listens on the ServicePort for connections on
   // network 2 from any service user.
   TCPServer m_serviceListenerNet2;

   // True when m_serviceListenerNet2 is bound to a valid address and can be
   // used to accept new service connections.
   bool m_isNetwork2Available;

   // Holds the status of the created MODDHandler instance.
   InternalStatus m_status;

   // Event handle to be used from the service thread when using the
   // sendMessage interface.
   WSAEVENT m_serviceSendEvent;

   // Event handle to be used from the service thread when using the
   // receiveMessage interface.
   WSAEVENT m_serviceReceiveEvent;

   // Event handle to be used when handling the temporary TCP connection
   // which should only respond with a "Busy" result code when calling
   // the sendMessage interface.
   WSAEVENT m_temporarySendEvent;

   // Event handle to be used when handling the temporary TCP connection
   // which should only respond with a "Busy" result code when calling
   // the receiveMessage interface.
   WSAEVENT m_temporaryReceiveEvent;

   // Event handle to be used when informing the serviceThread to begin
   // processing of a connected service connection, handling whatever request
   // that is received.
   WSAEVENT m_triggerServiceThreadEvent;

   // The IP address of the DHCP server.
   u_int32 m_DHCPAddress;

   // The APZ type that the MODDHandler function should interact with.
   const APZType m_APZType;

   // Pointer to the MODDHandler instance.
   static MODDHandler* s_moddHandler;

   // Holds the status of created APZConfiguration instances and their
   // relation with threads.
   ThreadStatus m_APZConfigurationThreadStatus;

   // A list of APZConfiguration instances.
   list<APZConfiguration*> m_apzConfiguration;

   // A list of handles to created threads.
   list<HANDLE> m_threadHandle;

   // Event used as global shutdown signal
   HANDLE shutdownEvent;
};

//******************************************************************************
// Description:
//    Tells whether or not a created instance of MODDHandler will be able to
//    execute or not.
// Returns:
//    OK if the MODDHandler instance is ready to continue with execution in the
//    run method after its creation, otherwise NotOK is returned.
//******************************************************************************
inline MODDHandler::Status MODDHandler::status() const
{
   return m_status == Success ? OK : NotOK;
}

//******************************************************************************
// Description:
//    Tells the APZ type that should be set up in the DHCP server.
// Returns:
//    The APZ type that the MODDHandler should configure in the DHCP server.
//******************************************************************************
inline MODDHandler::APZType MODDHandler::getAPZType() const
{
   return m_APZType;
}

//******************************************************************************
// Description:
//    Tells the DHCP server IP address.
// Returns:
//    The DHCP server IP address.
//******************************************************************************
inline u_int32 MODDHandler::getDHCPServerAddress() const
{
   return m_DHCPAddress;
}

#endif
