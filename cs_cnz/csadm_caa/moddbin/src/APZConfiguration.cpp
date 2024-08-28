//******************************************************************************
//
// NAME
//      APZConfiguration
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
//  See APZConfiguration.H

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

#include "APZConfiguration.H"
#include "MODDHandler.H"
#include "EventReporter.H"
#include <cassert>

using namespace std;

//******************************************************************************
// Description:
//    The Constructor. It is thread safe.
// Parameters:
//    theName  A name associated with the instance.
//******************************************************************************
APZConfiguration::APZConfiguration(const char* theName) :
   m_name(theName),
   m_isInvalidReplyReportedForSetupPeerConnection(false)
{
   SafeMutex theMutex(m_APZConfigurationMutex);
}

//******************************************************************************
// Description:
//    The Destructor. It is thread safe.
//******************************************************************************
APZConfiguration::~APZConfiguration()
{
   SafeMutex theMutex(m_APZConfigurationMutex);
}

//******************************************************************************
// Description:
//    Tells the name of the current APZConfiguration instance.
// Returns:
//    The name of the instance.
//******************************************************************************
const char* APZConfiguration::name() const
{
   return m_name.c_str();
}

//******************************************************************************
// Description:
//   Check of the basic DHCP setup.
//   It is thread safe. However, as
//   the DHCP setup, such as scopes and IP ranges, may be deleted
//   and re-created, updating the DHCP tables may fail. This method
//   should therefore be called in main thread, before any APZ 
//   configuration threads have been started.
// Returns:
//   Finished or Failed depending on the state of operation.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::checkBasicDHCPSetup()
{
   SafeMutex theMutex(m_APZConfigurationMutex);
   static bool isInvalidReplyReported = false;
   // Call the derived class' implementation now that we have ensured thread
   // safety.
   Operation reply = checkBasicDHCPSetupImplementation();

   if(reply != Finished && reply != Failed)
   {
      assert(!"Invalid reply from the derived class' implementation of checkBasicDHCPSetup()");

      if(!isInvalidReplyReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("Invalid reply (%d) from the derived class' implementation of checkBasicDHCPSetup(), name is %s",
                            reply,
                            name()),
               PROBLEM_TEXT("")));

         isInvalidReplyReported = true;
      }

      // Report this as Failed.
      reply = Failed;
   }

   return reply;
}

//******************************************************************************
// Description:
//   Cleans up the DHCP setup.
//   It is thread safe. However, as
//   the DHCP cleanup deletes the scopes, updating the DHCP tables may fail. 
//   This method should therefore be called in main thread, before any APZ 
//   configuration threads have been started.
// Returns:
//   Finished or Failed depending on the state of operation.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::cleanUpDHCPSetup()
{
   SafeMutex theMutex(m_APZConfigurationMutex);
   static bool isInvalidReplyReported = false;
   // Call the derived class' implementation now that we have ensured thread
   // safety.
   Operation reply = cleanUpDHCPSetupImplementation();

   if(reply != Finished && reply != Failed)
   {
      assert(!"Invalid reply from the derived class' implementation of cleanUpDHCPSetupImplementation()");

      if(!isInvalidReplyReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("Invalid reply (%d) from the derived class' implementation of cleanUpDHCPSetupImplementation(), name is %s",
                            reply,
                            name()),
               PROBLEM_TEXT("")));

         isInvalidReplyReported = true;
      }

      // Report this as Failed.
      reply = Failed;
   }

   return reply;
}

//******************************************************************************
// Description:
//    Prepare to commence setting up the required information in the DHCP
//    server. It is thread safe.
// Returns:
//    Failed or Finished.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::initialize()
{
   SafeMutex theMutex(m_APZConfigurationMutex);
   static bool isInvalidReplyReported = false;

   // Call the derived class' implementation now that we have ensured thread
   // safety.
   Operation reply = initializeImplementation();

   if(reply != Finished && reply != Failed)
   {
      assert(!"Invalid reply from the derived class' implementation of initializeImplementation()");

      if(!isInvalidReplyReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("Invalid reply (%d) from the derived class' implementation of initializeImplementation(), name is %s",
                            reply,
                            name()),
               PROBLEM_TEXT("")));

         isInvalidReplyReported = true;
      }

      // Consider this to be a failure.
      reply = Failed;
   }

   return reply;
}

//******************************************************************************
// Description:
// Establishes a connection towards the APZ peer.
// NOTE: THIS METHOD IS NOT THREAD SAFE AND SHOULD NOT BE THREAD SAFE. NO
//       MEMBER DATA MAY BE ACCESSED WHILE EXECUTING IN THE CONTEXT OF THIS
//       METHOD.
//
// Returns:
//   Finished or Failed depending on the state of operation.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::setupPeerConnection()
{
   // Call the derived class although thread safety is not guaranteed.
   Operation reply = setupPeerConnectionImplementation();

   if(reply != Finished && reply != Failed)
   {
      assert(!"Invalid reply from the derived class' implementation of setupPTBConnectionImplementation()");

      if(!m_isInvalidReplyReportedForSetupPeerConnection)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("Invalid reply (%d) from the derived class' implementation of setupPTBConnectionImplementation(), name is %s",
                            reply,
                            name()),
               PROBLEM_TEXT("")));

         m_isInvalidReplyReportedForSetupPeerConnection = true;
      }

      // Report this as Failed.
      reply = Failed;
   }

   return reply;
}

//******************************************************************************
// Description:
//    Acquires MAC address information and maps it with IP address information
//    while updating the DHCP server with the APZConfiguration specific type
//    of information. It is thread safe.
//
// Returns:
//   InProgress, Finished or Failed depending on the state of operation.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::setupDHCP()
{
   SafeMutex theMutex(m_APZConfigurationMutex);
   static bool isInvalidReplyReported = false;

   // Call the derived class' implementation now that we have ensured thread
   // safety.
   Operation reply = setupDHCPImplementation();

   if(reply != InProgress && reply != Finished && reply != Failed)
   {
      assert(!"Invalid reply from the derived class' implementation of setupDHCPImplementation()");

      if(!isInvalidReplyReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("Invalid reply (%d) from the derived class' implementation of setupDHCPImplementation(), name is %s",
                            reply,
                            name()),
               PROBLEM_TEXT("")));

         isInvalidReplyReported = true;
      }

      // Report this as Failed.
      reply = Failed;
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
//    InProgress or Finished.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::finalize()
{
   static bool isInvalidReplyReported = false;

   // Call the derived class although thread safety is not guaranteed.
   Operation reply = finalizeImplementation();

   if(reply != InProgress && reply != Finished)
   {
      assert(!"Invalid reply from the derived class' implementation of finalizeImplementation()");

      if(!isInvalidReplyReported)
      {
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("Invalid reply (%d) from the derived class' implementation of finalizeImplementation(), name is %s",
                            reply,
                            name()),
               PROBLEM_TEXT("")));

         isInvalidReplyReported = true;
      }

      // Report that the execution is Finished.
      reply = Finished;
   }

   return reply;
}

//******************************************************************************
// Description:
//    Will return with a pointer to a ClientInformation instance set if a
//    match for the provided MAC address is found. Otherwise the pointer is
//    set to NULL. The method will always return Busy if another thread is
//    already executing any of the othrer thread safe methods. Otherwise
//    Finished will be returned. 
//    It is thread safe.
// Parameters:
//    macAddress         The MAC address for which a ClientInformation
//                       instance is requested.
//    clientInformation  Will be assigned a valid ClientInformation instance
//                       if a match for the provided MAC address is found,
//                       otherwise the pointer is set to NULL.
// Returns:
//    Finished if a it was possible to look up a ClientInformation instance
//    for the provided MAC address (the result may be NULL).
//    Busy if the lookup request could not be processed since the
//    APZConfiguration instance had already locked the thread specific mutex.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::getClientInformation(const MACAddress& macAddress,
                                                                   ClientInformation*& clientInformation)
{
   SafeMutex theMutex(m_APZConfigurationMutex, SafeMutex::TryLock);

   if(theMutex.isLocked())
   {
      // Call the derived class' implementation now that we have ensured
      // thread safety.
      clientInformation = getClientInformationImplementation(macAddress);

      return Finished;
   }
   else
   {
      return Busy;
   }
}

//******************************************************************************
// Description:
//    Decreases the automatic revert timer for all ClientInformation in the
//    specific APZConfiguration derived class. It is thread safe.
// Parameters:
//    elapsedSeconds  The amount of seconds that should be reduced from the
//                    automatic revert timer.
// Returns:
//    Busy if another thread has already locked the instance for execution
//    (the requested operation was not processed). Otherwise Finished is
//    returned.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::decreaseRevertTimer(u_int16 elapsedSeconds)
{
   SafeMutex theMutex(m_APZConfigurationMutex, SafeMutex::TryLock);

   if(theMutex.isLocked())
   {
      // Call the derived class' implementation now that we have ensured thread
      // safety.
      decreaseRevertTimerImplementation(elapsedSeconds);

      return Finished;
   }
   else
   {
      return Busy;
   }
}

//******************************************************************************
// Description:
//    Performs an automatic MODD revert operation for all ClientInformation
//    instances for which the automatic revert timer has expired. It is thread
//    safe.
// Returns:
//    Busy if another thread has already locked the instance for execution
//    (the requested operation was not processed). Otherwise Finished is
//    returned.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::automaticRevert()
{
   SafeMutex theMutex(m_APZConfigurationMutex, SafeMutex::TryLock);

   if(theMutex.isLocked())
   {
      // Call the derived class' implementation now that we have ensured thread
      // safety.
      automaticRevertImplementation();

      return Finished;
   }
   else
   {
      return Busy;
   }
}

//******************************************************************************
// Description:
//    Changes the boot filename for the supplied client, this method should
//    only return Finished or Failed. No input pointer may be NULL.
//    This method guarantees that derived classes will never get a call to the
//    derived implementation of changeBootFilename(..., ..., ...) with NULL
//    pointer input. It is thread safe.
// Parameters:
//    client    The ClientInformation instance for which the boot filename
//              should change. NULL is invalid input.
//    filename  The new boot filename. NULL is invalid input.
// Returns:
//    Busy if another thread has already locked the instance for execution
//    (the requested operation was not processed).
//    Failed when the change operation cannot be executed or is executed with
//    errors, otherwise Finished is returned.
//******************************************************************************
APZConfiguration::Operation APZConfiguration::changeBootFilename(const ClientInformation* client,
                                                                 const char* filename)
{
   SafeMutex theMutex(m_APZConfigurationMutex, SafeMutex::TryLock);

   static bool isInvalidInputReported = false;
   static bool isInvalidReturnValueReported = false;

   if(theMutex.isLocked())
   {
      Operation reply = Failed;

      if(client != NULL && filename != NULL)
      {
         // Input has been validated, now it is safe to call the derived class'
         // implementation of changeBootFilename(..., ..., ...);
         u_int16 length = static_cast<u_int16>(strlen(filename));
         
         reply = changeBootFilenameImplementation(client, length, filename);

         // Verify that the derived class reports valid reply codes only.
         if(reply != Failed && reply != Finished)
         {
            assert(!"Programming error in derived class' implementation of changeBootFilenameImplementation(), the return value must be either of Finished or Failed");

            if(!isInvalidReturnValueReported)
            {
               static_cast<void>(EVENT(EventReporter::ProgrammingError,
                     PROBLEM_DATA("A programming error in the derived class' implementation of changeBootFilenameImplementation() has been detected, the return value %d is not valid in this context, name is %s",
                                  reply,
                                  name()),
                     PROBLEM_TEXT("")));

               isInvalidReturnValueReported = true;
            }

            // Since an invalid reply was received, we report that the operation
            // failed.
            reply = Failed;
         }
      }
      else
      {
         assert(!"ProgrammingError, invalid input to APZConfiguration::changeBootFilename(...)");

         if(!isInvalidInputReported)
         {
            // Report the violation to the interface.
            static_cast<void>(EVENT(EventReporter::ProgrammingError,
                  PROBLEM_DATA("Invalid input data to the method APZConfiguration::changeBootFilename(...). Input data may not be NULL. client = 0x%x, filename = 0x%x",
                               client,
                               filename),
                  PROBLEM_TEXT("")));

            isInvalidInputReported = true;
         }
      }

      return reply;
   }
   else
   {
      return Busy;
   }
}

//******************************************************************************
// Description:
//    An order that all raised alarms must be ceased because the process is
//    about to terminate has been received from PRC. Cease all alarms and
//    prepare to be terminated.
//
//    NOTE: THIS METHOD IS NOT THREAD SAFE AND MUST NOT BE THREAD SAFE SINCE
//          A THREAD SAFE CALL MAY CAUSE A DELAY IN TERMINATING THE PROCESS IF
//          ANOTHER THREAD IS ALREDY EXECUTING ON ANY OTHER METHOD FOR THE
//          CURRENT INSTANCE OF THE DERIVED CLASS OF APZCONFIGURATION.
//******************************************************************************
void APZConfiguration::ceaseAlarms()
{
   // Call the derived class although thread safety is not guaranteed.
   ceaseAlarmsImplementation();
}
