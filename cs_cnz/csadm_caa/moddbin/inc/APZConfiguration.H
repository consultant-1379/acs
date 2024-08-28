//******************************************************************************
// 
// .NAME
//    APZConfiguration - Interface to classes that should set up DHCP
//                       information specific to a certain CP generation.
// .LIBRARY 3C++
// .PAGENAME APZConfiguration
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
//  All access to public interfaces are thread safe independent of the
//  implementation in the derived class with the exception of the
//  methods setupPeerConnection(), ceaseAlarms() and finalize() that are 
//  not thread safe. 
//  The reason for this is that:
//  (1) The setupPeerConnection() cannot be thread safe holding a mutex, as
//  only one of the subnet may be up and one peer is connected (managed by 
//  one DHCPServer thread). If the other subnet is down, another DHCPServer
//  thread in MODD keeps trying to establish its peer connection
//  (and in that case it would take the mutex). 
//  Now, if the service thread is called to carry out an order, e.g. RUF,
//  which is ok to do with just one subnet up, the service thread would return
//  busy to the application for every attempt, due to the fact that the mutex
//´ is taken almost all the time (it's just released for a very short period, 
//  which is impossible for the service thread to hit). Hence, in the case of
//  one subnet is down setupPeerConnection() must be MT unsafe, otherwise the
//  service thread cannot carry out its orders on the other subnet.
//  (2) The ceaseAlarms() method should be called only when PRC has ordered a
//  shutdown of the process (this may be received by a thread that is not
//  currently executing). When the thread that received the PRC event orders
//  alarm ceasing an APZConfiguration instance may already be executing an
//  will then have locked the thread specific mutex. When PRC has ordered a
//  shutdown we are not allowed to delay that shutdown for more than 4
//  seconds. To guarantee this we must ensure that the same APZConfiguration
//  instance may execute concurrently in two threads where one is executing
//  ceaseAlarms and the other is executing any other method.
//  (3) The finalize() method is called when the DHCP server's table has been 
//  setup, and keeps sending HBs at a regular interval. It's no need to 
//  serialize the execution, as the finalize() method won't interfere with
//  member data.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-05-25 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef APZCONFIGURATION_H
#define APZCONFIGURATION_H

#include "PlatformTypes.hxx"
#include "MACAddress.H"
#include "MultiPlatformMutex.hxx"

class ClientInformation;

class APZConfiguration
{
public:
   // Defines the status of the execution of a specific interface method.
   enum Operation
   {
      Finished,
      InProgress,
      Busy,
      Failed
   };

   // Contructor.
   APZConfiguration(const char* name);

   // Destructor.
   virtual ~APZConfiguration();

   // Tells the name of the current APZConfiguration instance.
   const char* name() const;

   // Checks the basic DHCP Setup. It is thread safe. However, as
   // the DHCP setup, such as scopes and IP ranges, may be deleted
   // and re-created, updating the DHCP tables may fail. This method
   // should therefore be called in main thread, before any APZ 
   // configuration threads have been started.
   Operation checkBasicDHCPSetup();

   // Cleans up the DHCP setup by deleting the scopes. This method
   // should be called in main thread, before any APZ 
   // configuration threads have been started.
   Operation cleanUpDHCPSetup();

   // Prepare to commence setting up the required information in the DHCP
   // server.
   Operation initialize();

   // Establishes a connection towards the APZ peer.
   // NOTE: THIS METHOD IS NOT THREAD SAFE AND SHOULD NOT BE THREAD SAFE. NO
   //       MEMBER DATA MAY BE ACCESSED WHILE EXECUTING IN THE CONTEXT OF THIS
   //       METHOD.
   Operation setupPeerConnection();

   // Acquires MAC address information and maps it with IP address information
   // while updating the DHCP server with the APZConfiguration specific type
   // of information. It is thread safe.
   Operation setupDHCP();

   // Provides a possibility to continue executing without member data access.
   // NOTE: THIS METHOD IS NOT THREAD SAFE AND SHOULD NOT BE THREAD SAFE. NO
   //       MEMBER DATA MAY BE ACCESSED WHILE EXECUTING IN THE CONTEXT OF THIS
   //       METHOD.
   Operation finalize();

   // Will return with a pointer to a ClientInformation instance set if a
   // match for the provided MAC address is found. Otherwise the pointer is
   // set to NULL. The method will always return Busy if another thread is
   // already executing any of the other thread safe methods. Otherwise
   // Finished will be returned. 
   Operation getClientInformation(const MACAddress& macAddress, ClientInformation*& clientInformation);

   // Should decrease the automatic revert timer for all ClientInformation
   // instances. The method will always return Busy if another thread is
   // already executing any of the other thread safe methods.
   Operation decreaseRevertTimer(u_int16 elapsedSeconds);

   // The ClientInformation instances for which the timer has expired should
   // perform an automatic MODD revert operation. The method will always
   // return Busy if another thread is already executing any of the other
   // thread safe methods.
   Operation automaticRevert();

   // Changes the boot filename for the supplied client, this method should
   // only return Finished or Failed. No input pointer may be NULL. The method
   // will always return Busy if another thread is already executing any of
   // the other thread safe methods.
   Operation changeBootFilename(const ClientInformation* client, const char* filename);

   // An order that all raised alarms must be ceased because the process is
   // about to terminate has been received from PRC. Cease all alarms and
   // prepare to be terminated.
   // NOTE: THIS METHOD IS NOT THREAD SAFE AND MUST NOT BE THREAD SAFE SINCE
   //       A THREAD SAFE CALL MAY CAUSE A DELAY IN TERMINATING THE PROCESS IF
   //       ANOTHER THREAD IS ALREDY EXECUTING ON ANY OTHER METHOD FOR THE
   //       CURRENT INSTANCE OF THE DERIVED CLASS OF APZCONFIGURATION.
   void ceaseAlarms();

protected:
   // This method checks that the basic DHCP setup is correct, if not it is setup.
   virtual Operation checkBasicDHCPSetupImplementation() = 0;

   // This method brutally cleans up the DHCP setup.
   virtual Operation cleanUpDHCPSetupImplementation() = 0;

   // This method provide the deriving class of means to detect when setting
   // up the DHCP server is about to commence.
   virtual Operation initializeImplementation() = 0;

   // This method establishes a connection towards a PTB peer
   virtual Operation setupPeerConnectionImplementation() = 0;

   // This method should acquire MAC address information and construct MAC
   // address to IP address mappings along with setting up this information in
   // the DHCP server.
   virtual Operation setupDHCPImplementation() = 0;

   // Provides a possibility to continue executing without member data access.
   virtual Operation finalizeImplementation() = 0;

   // Should return a pointer to a ClientInformation instance if a match for
   // the provided MAC address is found, otherwise NULL should be returned.
   virtual ClientInformation* getClientInformationImplementation(const MACAddress& macAddress) = 0;

   // Should decrease the automatic revert timer for all ClientInformation
   // instances.
   virtual void decreaseRevertTimerImplementation(u_int16 elapsedSeconds) = 0;

   // The ClientInformation instances for which the timer has expired should
   // perform an automatic MODD revert operation.
   virtual void automaticRevertImplementation() = 0;

   // Changes the boot filename for the supplied client, this method should
   // only return Finished or Failed. No input pointer may be NULL.
   virtual Operation changeBootFilenameImplementation(const ClientInformation* client, u_int16 filenameLength, const char* filename) = 0;

   // An order that all raised alarms must be ceased because the process is
   // about to terminate has been received from PRC. Cease all alarms and
   // prepare to be terminated.
   virtual void ceaseAlarmsImplementation() = 0;

private:
   // The name of the APZConfiguration instance.
   std::string m_name;

   // The mutex that should make an instance of APZConfiguration thread safe.
   mutable MultiPlatformMutex m_APZConfigurationMutex;

   // Tracks whether an invalid reply from the MT unsafe 
   // setupPeerConnectionImplementation() method has been reported or not.
   // Note, because of the MT unsafe setUpPeerconnection() method,
   // this member is needed. (Normally, is a local 
   // static bool isInvalidReplyReported parameter used).
   bool m_isInvalidReplyReportedForSetupPeerConnection;
};

#endif
