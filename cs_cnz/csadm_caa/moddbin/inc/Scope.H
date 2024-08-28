//******************************************************************************
// 
// .NAME
//    Scope - Provides a container for IP address and netmask information.
// .LIBRARY 3C++
// .PAGENAME Scope
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
//  An instance of Scope provides a storage location for data considered to
//  define a scope when accessing the DHCP server. The scope is defined by an
//  IP address and its correlating netmask.

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
#ifndef SCOPE_H
#define SCOPE_H

#include <winsock2.h>
#include <windows.h>

class Scope
{
public:
   // Constructor
   Scope()
   {
      clear();
   }

   // Sets the address and netmask to zero.
   void clear()
   {
      m_address = 0;
      m_netmask = 0;
   }

   // Assign an address to the scope information.
   void address(DWORD theAddress)
   {
      m_address = theAddress;
   }

   // Return the address already defined in the scope information.
   DWORD address() const
   {
      return m_address;
   }

   // Assign a netmask to the scope information.
   void netmask(DWORD theNetmask)
   {
      m_netmask = theNetmask;
   }

   // Return the netmask already defined in the scope information.
   DWORD netmask() const
   {
      return m_netmask;
   }

private:
   // The scope address.
   DWORD m_address;

   // The netmask.
   DWORD m_netmask;
};

#endif
