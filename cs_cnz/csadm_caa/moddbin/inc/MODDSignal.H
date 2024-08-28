//******************************************************************************
// 
// .NAME
//    MODDSignal - Defines the MODD protocol signal formats.
// .LIBRARY 3C++
// .PAGENAME MODDSignal
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
//  The MODDSignal may be used to help setting up data when exchanging
//  information using the MODD protocol (which is defined to be LITTLE
//  ENDIAN).

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
#ifndef MODDSIGNAL_H
#define MODDSIGNAL_H

#ifdef _MSC_VER
#if _MSC_VER > 1200
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif
#endif // _MSC_VER

#include "PlatformTypes.hxx"
#include "MACAddress.H"
#include <cassert>

// Member functions, constructors, destructors, operators

class MODDSignal
{
public:
   // The length of the header in bytes.
   static const u_int16 HeaderLength = 8;

   // This is the maximum allowed data length (+ the HeaderLength) that a
   // MODDSignal may consist of.
   static const u_int16 MaximumDataLength = 1024;

   enum Primitive
   {
      // Change the current DHCP setup
      Change         = 0,

      // The result of a requested Change operation
      ChangeResponse = 1,

      // Revert a previous change made to the DHCP setup
      Revert         = 2,

      // The result of a requested Revert operation
      RevertResponse = 3
   };

   enum ResultCode
   {
      // The requested operation executed successfully
      Success             = 0,

      // The supplied protocol version is not supported
      NotSupportedVersion = 1,

      // The supplied MAC address is not part of the DHCP data
      NoSuchEntry         = 2,

      // The requested operation has failed
      Failure             = 3,

      // Another operation is already in progress, try again later
      Busy                = 4, // Try again

      // The supplied boot image file name was not found in the file system
      FileNotFound        = 5
   };

   enum Version
   {
      CurrentVersion = 1
   };

   // Constructor (use when creating new MODDSignals).
   MODDSignal(Primitive);

   // Constructor (use when interpreting raw data as a MODDSignal).
   MODDSignal(const char* data, u_int32 size);

   // Returns the MODDSignal as a raw byte stream.
   u_char* message();

   // Returns the size (in bytes) of the data returned by the message method.
   u_int16 size() const;

   // Returns the information stored in the message length field.
   u_int16 messageLength() const;

   // Tells of the version of the created MODDSignal.
   u_int16 version() const;

   // Tells of the service request carried by the MODDSignal.
   Primitive primitive() const;

   // Assigns a MAC address to the MODDSignal MAC address field.
   void setMACAddress(MACAddress identifier);

   // Retrieves the MAC address from the MODDSignal.
   MACAddress getMACAddress();

   // Assigns a boot image file name to the MODDSignal.
   void bootImage(const char* file);

   // Returns the boot image name.
   const char* bootImage() const;

   // Returns the length of the boot image file name.
   u_int16 bootImageLength() const;

   // Assigns a description to the MODDSignal.
   void description(const char* text);

   // Returns a pointer to the description field.
   const char* description() const;

   // Returns the length of the description field.
   u_int16 descriptionLength() const;

   // Assigns a result code to the MODDSignal.
   void result(ResultCode resultCode);

   // Tells of the result code stored in the MODDSignal.
   ResultCode result() const;

   // Sets the timeout value field.
   void timeout(u_int16 seconds);

   // Tells of the timeout value stored int the MODDSignal.
   u_int16 timeout() const;

private:
   // These values define the data placement of specific fields that reside
   // beyond the MODDSignal header and are relative to HeaderLength.
   static const u_int16 MACAddressPosition        =  0; // 6 bytes, Primitives Change and Revert
   static const u_int16 TimeoutValuePosition      =  6; // 2 bytes, Primitive Change
   static const u_int16 BootImageLengthPosition   =  8; // 2 bytes, Primitive Change
   static const u_int16 BootImagePosition         = 10; // n bytes, Primitive Change
   static const u_int16 ResultPosition            =  0; // 2 bytes, Primitive ChangeResponse and RevertResponse
   static const u_int16 DescriptionLengthPosition =  2; // 2 bytes, Primitive ChangeResponse and RevertResponse
   static const u_int16 DescriptionPosition       =  4; // n bytes, Primitive ChangeResponse and RevertResponse

   // Tells the length of the data beyond the MODDSignal header.
   u_int16 variableDataLength() const;

   // Tells whether or not the running system endian is LITTLE or BIG.
   static bool isHostLittleEndian();

   // The fields version, message length, primitive and reserved form the
   // header for the MODDSignal and is common for all signals.

   // The version field.
   u_int16 m_version;

   // The length of the MODDSignal in bytes excluding the header.
   u_int16 m_messageLength;

   // The primitive field.
   u_int16 m_primitive;

   // This field should always be zero.
   u_int16 m_reserved;

   // This array of bytes contains the variable data content which will vary
   // from one Primitive to another.
   u_char m_data[MaximumDataLength];
};

//******************************************************************************
// Description:
//    Returns the MODDSignal as a raw byte stream.
// Returns:
//    A pointer to the raw byte data of the MODDSignal.
//******************************************************************************
inline u_char* MODDSignal::message()
{
   // The raw data representaion of the MODDSignal begins with the version
   // field.
   return reinterpret_cast<u_char*>(&m_version);
}

//******************************************************************************
// Description:
//    Returns the size (in bytes) of the data returned by the message method.
// Returns:
//    The number of bytes that the data returned by a call to the message
//    method will point to.
//******************************************************************************
inline u_int16 MODDSignal::size() const
{
   return static_cast<u_int16>(HeaderLength) + variableDataLength();
}

//******************************************************************************
// Description:
//   Returns the length of the boot image file name.
// Returns:
//   The boot image file name length.
//******************************************************************************
inline u_int16 MODDSignal::bootImageLength() const
{
   assert(primitive() == Change &&
          "The boot image name length field is not available for this primitive");

   return FROM_LITTLE_END16(*reinterpret_cast<const u_int16*>(&m_data[BootImageLengthPosition]));
}

//******************************************************************************
// Description:
//   Returns the length of the description field.
// Returns:
//   The description field length.
//******************************************************************************
inline u_int16 MODDSignal::descriptionLength() const
{
   assert((primitive() == ChangeResponse || primitive() == RevertResponse) &&
          "The description length field is not available for this primitive");

   return FROM_LITTLE_END16(*reinterpret_cast<const u_int16*>(&m_data[DescriptionLengthPosition]));
}

//******************************************************************************
// Description:
//   Tells of the service request carried by the MODDSignal.
// Returns:
//   The Primitive carried in the MODDSignal.
//******************************************************************************
inline MODDSignal::Primitive MODDSignal::primitive() const
{
   return static_cast<Primitive>(FROM_LITTLE_END16(m_primitive));
}

//******************************************************************************
// Description:
//   Assigns a result code to the MODDSignal.
// Parameters:
//   resultCode  The result code which should be stored in the MODDSignal.
//******************************************************************************
inline void MODDSignal::result(MODDSignal::ResultCode resultCode)
{
   assert((primitive() == ChangeResponse || primitive() == RevertResponse) &&
          "The result field is not available for this primitive");

   *reinterpret_cast<u_int16*>(&m_data[ResultPosition]) = TO_LITTLE_END16(static_cast<u_int16>(resultCode));
}

//******************************************************************************
// Description:
//   Tells of the result code stored in the MODDSignal.
// Returns:
//   The result code carried in the MODDSignal.
//******************************************************************************
inline MODDSignal::ResultCode MODDSignal::result() const
{
   assert((primitive() == ChangeResponse || primitive() == RevertResponse) &&
          "The result field is not available for this primitive");

   return static_cast<ResultCode>(FROM_LITTLE_END16(*reinterpret_cast<const u_int16*>(&m_data[ResultPosition])));
}

//******************************************************************************
// Description:
//   Tells of the version of the created MODDSignal.
// Returns:
//   The version of the MODDSignal instance.
//******************************************************************************
inline u_int16 MODDSignal::version() const
{
   return FROM_LITTLE_END16(m_version);
}

//******************************************************************************
// Description:
//   Returns the information stored in the message length field.
// Returns:
//   The message length (MODDSignal data excluding the MODDSignal header
//   size).
//******************************************************************************
inline u_int16 MODDSignal::messageLength() const
{
   return FROM_LITTLE_END16(m_messageLength);
}

//******************************************************************************
// Description:
//   Sets the timeout value field.
// Parameters:
//   seconds  The amount of seconds before automatic Revert operation will
//            take place. 0 disable s automatic Revert operation.
//******************************************************************************
inline void MODDSignal::timeout(u_int16 seconds)
{
   assert(primitive() == Change &&
          "The timeout field is not available for this primitive");

   *reinterpret_cast<u_int16*>(&m_data[TimeoutValuePosition]) = TO_LITTLE_END16(seconds);
}

//******************************************************************************
// Description:
//   Tells of the timeout value stored int the MODDSignal.
// Returns:
//   The timeout value.
//******************************************************************************
inline u_int16 MODDSignal::timeout() const
{
   assert(primitive() == Change &&
          "The timeout field is not available for this primitive");

   return FROM_LITTLE_END16(*reinterpret_cast<const u_int16*>(&m_data[TimeoutValuePosition]));
}

//******************************************************************************
// Description:
//   Returns the boot image name.
// Returns:
//   The boot image file name.
//******************************************************************************
inline const char* MODDSignal::bootImage() const
{
   assert(primitive() == Change &&
          "The boot image field is not available for this primitive");

   return reinterpret_cast<const char*>(&m_data[BootImagePosition]);
}

//******************************************************************************
// Description:
//   Returns a pointer to the description field.
// Returns:
//   The description information stored in thd MODDSignal.
//******************************************************************************
inline const char* MODDSignal::description() const
{
   assert((primitive() == ChangeResponse || primitive() == RevertResponse) &&
          "The boot image field is not available for this primitive");

   return reinterpret_cast<const char*>(&m_data[DescriptionPosition]);
}

#endif
