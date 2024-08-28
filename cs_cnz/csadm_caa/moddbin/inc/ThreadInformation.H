//******************************************************************************
// 
// .NAME
//    ThreadInformation - Provides a container thread specific data belonging
//                        to threads that set up the DHCP server tables with
//                        APZ specific client data information.
// .LIBRARY 3C++
// .PAGENAME ThreadInformation
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
//  An instance of ThreadInformation provides a storage location for data
//  that should be passed to newly created threads that should configure the
//  DHCP server with APZ client information data.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-05-26 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef THREADINFORMATION_H
#define THREADINFORMATION_H

#include <memory.h>

class APZConfiguration;

class ThreadInformation
{
public:
   // Constructor
   ThreadInformation(APZConfiguration* configuration)
   {
      // Clear all member data.
      memset(this, 0, sizeof(ThreadInformation));

      // Add the provided data.
      m_apzConfiguration = configuration;
   }

   // Returns the APZConfiguration instance
   APZConfiguration* apzConfiguration()
   {
      return m_apzConfiguration;
   }

private:
   APZConfiguration* m_apzConfiguration;
};

#endif
