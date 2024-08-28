//******************************************************************************
// 
// .NAME
//    ClientInformation - Contains DHCP information bound to a specific
//                        client.
// .LIBRARY 3C++
// .PAGENAME ClientInformation
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
//  An instance of ClientInformation contain information about a specific
//  client's IP address and its correlation with a specific boot image and
//  boot option values. It also contains information about whether an
//  automatic MODD revert operation is in progress and if so when it should
//  occur.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-06-09 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef CLIENTINFORMATION_H
#define CLIENTINFORMATION_H

#include "MODDSignal.H"
#include "MACAddress.H"
#include <winsock2.h>
#include <windows.h>

// Member functions, constructors, destructors, operators

class ClientInformation
{
public:
   // Global class constants.
   static const int MaxFilenameLength = MODDSignal::MaximumDataLength;
   static const int MaxHostnameLength = 30;
   static const int MaxDescriptionLength  = 100;

   enum Status
   {
      OK,
      Failure
   };

   // Constructor.
   ClientInformation();

   // Constructor.
   ClientInformation(const char* hostname, const char* comment, DWORD address, const char* bootFilename);

   // Clears all information in the instance.
   void clear();

   // Set the IP address information for a client.
   void ipAddress(DWORD address);

   // Tells the IP address information for a client.
   DWORD ipAddress() const;

   // Set the MAC address information for a client.
   void macAddress(const MACAddress address);

   // Tells the MAC address informaiton for a client.
   MACAddress macAddress() const;

   // Set the boot filename for a client.
   Status bootFilename(const char* filename);

   // Tells the boot filename for a client.
   const char* bootFilename() const;

   // Tells the orignal boot filename for a client (original in the respect
   // that this is the information which was the first one assigned to the
   // ClientInformation instance.
   const char* originalBootFilename() const;

   // Tells whether or not the automatic Revert operation timer has expired.
   bool isTimerExpired() const;

   // Sets the timeoutValue (time before automatic MODD revert operation).
   void timeoutValue(DWORD value);

   // Decrease the timeout counter value.
   void decreaseTimeoutValue(DWORD value);

   // Assigns a new hostname to the instance.
   void hostname(const char* newName);

   // Tells the hostname associated with the instance.
   const char* hostname() const;

   // Assigns a new comment to the instance.
   void description(const char* comment);

   // Tells the description stored for the instance.
   const char* description() const;

   // Set the boot IP address information for a client.
   void bootIPAddress(DWORD address);

   // Tells the boot IP address information for a client.
   DWORD bootIPAddress() const;

   // Tells whether the ClientInformation instance have been assigned MAC
   // address information or not.
   bool isMACAddressInformationKnown() const;

   // Make sure that the assignment operator is unavailable by defining but
   // not implementing it.
   ClientInformation& operator=(const ClientInformation& source);

private:
   // Copies the source data to the destination while guaranteeing that
   // overwriting of the source boundary is not done based on the supplied
   // length information.
   Status safeCopy(char* dest, const char* source, size_t maxLength);

   // The client IP address.
   DWORD m_IPAddress;

   // The IP address that the client should boot from.
   DWORD m_bootIPAddress;

   // The number of seconds before an automatic Revert operation should take
   // place.
   DWORD m_timeoutValue;

   // True when the timoue value has been set to anything but 0.
   bool m_isTimeoutTimerActive;

   // The client MAC address.
   MACAddress m_MACAddress;

   // The boot file name that the client should boot from.
   char m_bootFilename[MaxFilenameLength];

   // The original boot file read from the DHCP server.
   char m_originalBootFilename[MaxFilenameLength];

   // The client's hostname.
   char m_hostname[MaxHostnameLength];

   // Description of the client.
   char m_description[MaxDescriptionLength];

   // True if MAC address information has been added to the ClientInformation
   // instance.
   bool m_isMACAddressInformationKnown;
};

//******************************************************************************
// Description:
//    Tells whether the ClientInformation instance have been assigned MAC
//    address information or not.
// Returns:
//    true if MAC address information has been assigned to the instance.
//******************************************************************************
inline bool ClientInformation::isMACAddressInformationKnown() const
{
   return m_isMACAddressInformationKnown;
}

#endif
