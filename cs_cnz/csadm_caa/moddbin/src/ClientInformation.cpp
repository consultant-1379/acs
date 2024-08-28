//******************************************************************************
//
// NAME
//      ClientInformation
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
//  See ClientInformation.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-06-09 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .SEE ALSO 
//  N/A.
//
//******************************************************************************
#include "ClientInformation.H"
#include "Log.H"
#include "MultiPlatformMutex.hxx"
#include <iostream>
#include <memory.h>

namespace
{
   MultiPlatformMutex clientInformationMutex;
}

//******************************************************************************
// Description:
//    The Constructor initialises the instance with invalid values for its
//    data members.
//******************************************************************************
ClientInformation::ClientInformation()
{
   clear();
}

//******************************************************************************
// Description:
//    The Constructor initialises the instance with invalid values for its
//    data members and records the client's hostname and a description for the
//    client.
// Paramters:
//    name      The client's hostname.
//    comment   A description of the client entry in the DHCP server.
//    address   The client's IP address.
//    bootFile  The client's boot filename.
//******************************************************************************
ClientInformation::ClientInformation(const char* name,
                                     const char* comment,
                                     DWORD address,
                                     const char* bootFile)
{
   clear();

   hostname(name);
   description(comment);
   ipAddress(address);
   static_cast<void>(bootFilename(bootFile));
}

//******************************************************************************
// Description:
//    Clears all information in the instance.
//******************************************************************************
void ClientInformation::clear()
{
   SafeMutex mutex(clientInformationMutex);

   m_IPAddress = 0;
   m_bootIPAddress = 0;
   m_MACAddress = MACAddress();
   m_bootFilename[0] = '\0';
   m_originalBootFilename[0] = '\0';
   m_timeoutValue = 0;
   m_isTimeoutTimerActive = false;
   m_hostname[0] = '\0';
   m_description[0] = '\0';
   m_isMACAddressInformationKnown = false;
}

//******************************************************************************
// Description:
//    Set the MAC address information for a client.
// Parameters:
//    address  The client's MAC address.
//******************************************************************************
void ClientInformation::macAddress(const MACAddress address)
{
   SafeMutex mutex(clientInformationMutex);
   m_MACAddress = address;
   m_isMACAddressInformationKnown = true;
}

//******************************************************************************
// Description:
//    Set the boot filename for a client.
// Parameters:
//    filename  The client's boot filename.
// Returns:
//    OK if the provided filename was stored successfully, otherwise Failure
//******************************************************************************
ClientInformation::Status ClientInformation::bootFilename(const char* filename)
{
   SafeMutex mutex(clientInformationMutex);
   assert(filename && "The supplied filename must be non NULL");

   Status status = Failure;

   status = safeCopy(m_bootFilename, filename, MaxFilenameLength);

   if(status == OK)
   {
      // Copy the same source to the member that keeps track of the
      // first assigned boot filename if not already done.
      if(m_originalBootFilename[0] == '\0')
      {
         status = safeCopy(m_originalBootFilename, filename, MaxFilenameLength);
      }
   }

   return status;
}

//******************************************************************************
// Description:
//    Tells whether or not the automatic Revert operation timer has expired.
// Returns:
//    True if the timer has expired.
//******************************************************************************
bool ClientInformation::isTimerExpired() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_isTimeoutTimerActive ? (m_timeoutValue == 0 ? true : false) : false;
}

//******************************************************************************
// Description:
// Sets the timeoutValue (time before automatic MODD revert operation) and
// also enables the timer if the value is anything but zero.
// Parameters:
//    value  The value to decrease the timer by.
//******************************************************************************
void ClientInformation::timeoutValue(DWORD value)
{
   SafeMutex mutex(clientInformationMutex);
   m_timeoutValue = value;
   m_isTimeoutTimerActive = (value != 0 ? true : false);
}

//******************************************************************************
// Description:
//    Decrease the timeout counter value.
// Parameters:
//    value  The value to decrease the timer by.
//******************************************************************************
void ClientInformation::decreaseTimeoutValue(DWORD value)
{
   SafeMutex mutex(clientInformationMutex);

   // Never decrease the timeout value below zero.
   if(m_timeoutValue >= value)
   {
      m_timeoutValue -= value;
   }
   else
   {
      m_timeoutValue = 0;
   }
}

//******************************************************************************
// Description:
//    Tells the orignal boot filename for a client (original in the respect
//    that this is the information which was the first one assigned to the
//    ClientInformation instance.
// Returns:
//    The client's original boot filename.
//******************************************************************************
const char* ClientInformation::originalBootFilename() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_originalBootFilename;
}

//******************************************************************************
// Description:
//    Tells the boot filename for a client.
// Returns:
//    The client's boot filename.
//******************************************************************************
const char* ClientInformation::bootFilename() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_bootFilename;
}

//******************************************************************************
// Description:
//    Tells the MAC address informaiton for a client.
// Returns:
//    The client's MAC address.
//******************************************************************************
MACAddress ClientInformation::macAddress() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_MACAddress;
}

//******************************************************************************
// Description:
//    Set the IP address information for a client.
// Parameters:
//    address  The client's IP address.
//******************************************************************************
void ClientInformation::ipAddress(DWORD address)
{
   SafeMutex mutex(clientInformationMutex);
   m_IPAddress = address;
}

//******************************************************************************
// Description:
//    Tells the IP address information for a client.
// Returns:
//    The client's IP address.
//******************************************************************************
DWORD ClientInformation::ipAddress() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_IPAddress;
}

//******************************************************************************
// Description:
//    Assigns a new hostname to the instance.
// Parameters:
//    newName  The client's hostname.
//******************************************************************************
void ClientInformation::hostname(const char* newName)
{
   SafeMutex mutex(clientInformationMutex);
   static_cast<void>(safeCopy(m_hostname, newName, MaxHostnameLength));
}

//******************************************************************************
// Description:
//    Tells the hostname associated with the instance.
// Returns:
//    The client's hostname.
//******************************************************************************
const char* ClientInformation::hostname() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_hostname;
}

//******************************************************************************
// Description:
//    Assigns a new description to the instance.
// Parameters:
//    comment  A description of the client.
//******************************************************************************
void ClientInformation::description(const char* comment)
{
   SafeMutex mutex(clientInformationMutex);
   static_cast<void>(safeCopy(m_description, comment, MaxDescriptionLength));
}

//******************************************************************************
// Description:
//    Tells the description stored for the instance.
// Returns:
//    The client's description.
//******************************************************************************
const char* ClientInformation::description() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_description;
}

//******************************************************************************
// Description:
//    Copies the source data to the destination while guaranteeing that
//    overwriting of the source boundary is not done based on the supplied
//    length information.
// Parameters:
//    dest       This is where the copy should go.
//    source     The data that should be copied.
//    maxLength  The size of the dest container.
// Returns:
//    OK if the copy was successful, otherwise Failure.
//******************************************************************************
ClientInformation::Status ClientInformation::safeCopy(char* dest,
                                                      const char* source,
                                                      size_t maxLength)
{
   // If the source is not NULL, then include the NULL character ending the
   // string in the length of the source.
   size_t length = (source ? strlen(source) + 1 : 0);

   if(length >= maxLength)
   {
      assert(!"The source does not fit in the data member");
      return Failure;
   }

   if(source != NULL)
   {
      // Copy the boot file name including the NULL termination character.
      memcpy(dest, source, length);
   }
   else
   {
      assert(!"The supplied source has zero length");
      return Failure;
   }

   return OK;
}

//******************************************************************************
// Description:
//    Set the boot IP address information for a client.
// Parameters:
//    address  The IP address that this client should boot from.
//******************************************************************************
void ClientInformation::bootIPAddress(DWORD address)
{
   SafeMutex mutex(clientInformationMutex);
   m_bootIPAddress = address;
}

//******************************************************************************
// Description:
//    Tells the boot IP address information for a client.
// Returns:
//    The client's boot IP address.
//******************************************************************************
DWORD ClientInformation::bootIPAddress() const
{
   SafeMutex mutex(clientInformationMutex);
   return m_bootIPAddress;
}
