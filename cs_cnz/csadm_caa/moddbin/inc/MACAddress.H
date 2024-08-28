//******************************************************************************
// 
// .NAME
//    MACAddress - MAC address container.
// .LIBRARY 3C++
// .PAGENAME MACAddress
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
//  Provides an instantiable type that contains a MAC address.

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
#ifndef MACADDRESS_H
#define MACADDRESS_H

class MACAddress
{
public:
   // The maximum number of bytes that is allowed to be used as an identifier.
   static const unsigned int MaxIdentifierLength = 100;

   // Constructor.
   MACAddress(const unsigned char identifier[MaxIdentifierLength], size_t theIdentifierLength)
   {
      memcpy(m_identifier, identifier, theIdentifierLength);

      m_identifierLength = theIdentifierLength;
   }

   // Constructor.
   MACAddress(const char identifier[MaxIdentifierLength], size_t theIdentifierLength)
   {
      memcpy(m_identifier, identifier, theIdentifierLength);

      m_identifierLength = theIdentifierLength;
   }

   // Constructor.
   MACAddress()
   {
      memset(m_identifier, 0, MaxIdentifierLength);
      m_identifierLength = 0;
   }

   // Check for equality.
   bool operator==(const MACAddress& rvalue) const
   {
      return rvalue.m_identifierLength == m_identifierLength &&
             memcmp(rvalue.m_identifier, m_identifier, m_identifierLength) == 0 ? true : false;
   }

   // Assign information from ince instance of MACAddress to another.
   void operator=(const MACAddress& rvalue)
   {
      // Handle self assignment.
      if(this != &rvalue)
      {
         memcpy(m_identifier, rvalue.m_identifier, rvalue.m_identifierLength);
         m_identifierLength = rvalue.m_identifierLength;
      }
   }

   // Returns the MACAddress identifier as a consecutive sequence of bytes.
   const unsigned char* asBytes() const
   {
      return const_cast<const unsigned char*>(m_identifier);
   }

   // Tells the amount of bytes that are stored in the address identifier.
   size_t identifierLength() const
   {
      return m_identifierLength;
   }

private:
   // Declare this operator but do not implement it to ensure that it cannot
   // be used.
   void operator=(const char identifier[MaxIdentifierLength]);

   // The number of bytes that the identifying address consists of.
   size_t m_identifierLength;

   // The MAC address bytes
   unsigned char m_identifier[MaxIdentifierLength];
};


#endif
