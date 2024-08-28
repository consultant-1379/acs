//******************************************************************************
//
// NAME
//      MODDSignal
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
//  See MODDSignal.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-26 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .SEE ALSO 
//  N/A.
//
//******************************************************************************
#include	"MODDSignal.H"
#include <cassert>

//******************************************************************************
// Description:
//    Constructor (use when creating new MODDSignals).
// Parameters:
//   thePrimitive  Any value from the MODDSignal::Primitive enumeration.
//******************************************************************************
MODDSignal::MODDSignal(Primitive thePrimitive) :
   m_version(TO_LITTLE_END16(static_cast<u_int16>(CurrentVersion))),
   m_primitive(TO_LITTLE_END16(static_cast<u_int16>(thePrimitive))),
   m_reserved(0),
   m_messageLength(0) // No endian conversion is required for this
{
   m_data[0] = '\0';
}

//******************************************************************************
// Description:
//    Constructor (use when interpreting raw data as a MODDSignal).
// Parameters:
//   data     The data that should be interpreted as a MODDSignal.
//   theSize  The number of bytes in data.
//******************************************************************************
MODDSignal::MODDSignal(const char* data, u_int32 theSize)
{
   assert((HeaderLength + MaximumDataLength) <= sizeof(MODDSignal) &&
          "If (HeaderLength + MaximumDataLegngth) < sizeof(MODDSignal) then heap and/or stack destruction will occur");

   if(theSize > (HeaderLength + MaximumDataLength))
   {
      assert(!"Data is truncated");
      theSize = HeaderLength + MaximumDataLength;
   }
   else if(theSize == 0)
   {
      // Make sure that whatever data is read later on is not considered
      // valid.
      memset(message(), 0, HeaderLength + MaximumDataLength);

      assert(!"No data provided");
      return;
   }

   memcpy(message(), data, theSize);
}

//******************************************************************************
// Description:
//   Assigns a MAC address to the MODDSignal MAC address field.
// Paramters:
//   identifier  A MAC address to assign to the MODDSignal.
//******************************************************************************
void MODDSignal::setMACAddress(MACAddress identifier)
{
   assert((primitive() == Change || primitive() == Revert) &&
          "The MAC address field is not available for this primitive");

   u_char* macAddressStart = &m_data[MACAddressPosition];

   if(isHostLittleEndian())
   {
      // No conversion required.
      memcpy(macAddressStart, identifier.asBytes(), 6);
   }
   else
   {
      int src;
      int dest;

      // BIG to LITTLE endian conversion required.
      for(src = 5, dest = 0; dest < 6; ++dest, --src)
      {
         macAddressStart[dest] = identifier.asBytes()[src];
      }
   }

   if(primitive() == Revert)
   {
      // The Revert primitive contains no other information but the MAC
      // addres.
      m_messageLength = TO_LITTLE_END16(6);
   }
}

//******************************************************************************
// Description:
//   Retrieves the MAC address from the MODDSignal.
// Returns:
//   Returns the information from the MAC address field.
//******************************************************************************
MACAddress MODDSignal::getMACAddress()
{
   assert((primitive() == Change || primitive() == Revert) &&
          "The MAC address field is not available for this primitive");

   u_char* macAddressStart = &m_data[MACAddressPosition];
   u_char identifier[6];

   if(isHostLittleEndian())
   {
      // No conversion required.
      memcpy(identifier, macAddressStart, 6);
   }
   else
   {
      int src;
      int dest;

      // LITTLE to BIG endian conversion required.
      for(src = 5, dest = 0; dest < 6; ++dest, --src)
      {
         identifier[dest] = macAddressStart[src];
      }
   }

   return MACAddress(identifier, 6);
}

//******************************************************************************
// Description:
//   Tells the length of the data beyond the MODDSignal header.
// Returns:
//   The length (in bytes) of the MODDSignal beyond the MODDSignal header.
//******************************************************************************
u_int16 MODDSignal::variableDataLength() const
{
   u_int16 variableLength = 0;

   switch(primitive())
   {
   case Change:
      variableLength = 6 + // MAC address
                       2 + // Timeout value
                       2 + // Boot image name length field
                       bootImageLength();
      break;

   case ChangeResponse:
      variableLength = 2 + // Result
                       2 + // Description length field
                       descriptionLength();
      break;

   case Revert:
      variableLength = 6; // MAC address
      break;

   case RevertResponse:
      variableLength = 2 + // Result
                       2 + // Description length field
                       descriptionLength();
      break;

   default:
      assert(!"Invalid primitive, the isValid method should not allow this to happen");
   }

   return variableLength;
}

//******************************************************************************
// Description:
//   Assigns a boot image file name to the MODDSignal.
// Parameters:
//   file  [in] The boot image file name (NULL terminated).
//******************************************************************************
void MODDSignal::bootImage(const char* file)
{
   assert(primitive() == Change &&
          "The boot image field is not available for this primitive");

   // Determine the length of the file name
   size_t length = (file != NULL ? strlen(file) : 0);

   // Make sure that we will not write the boot image file name outside the
   // allocated space for it in the MODDSignal.
   if((length + 1) > (MaximumDataLength - 10)) // 10 is the sum of the MAC address, timeout value and boot image file name lengths
   {
      assert(!"The supplied boot image file name is too long, it will get truncated");
      length = (MaximumDataLength - 10) - 1;
   }

   if(length != 0)
   {
      memcpy(&m_data[BootImagePosition], file, length);
      m_data[BootImagePosition + length] = '\0';
   }
   else
   {
      assert(!"An invalid boot image file name has been supplied");
   }

   // Set the boot image name length field, include the NULL character too.
   *reinterpret_cast<u_int16*>(&m_data[BootImageLengthPosition]) = TO_LITTLE_END16(static_cast<u_int16>(length + 1));

   // Assign the data length to the message length field as this is now
   // known.
   //m_messageLength = TO_LITTLE_END16(6 + // MAC address
   //                                  2 + // Timeout value
   //                                  2 + // Boot image name length
   //                                  bootImageLength());
   m_messageLength = TO_LITTLE_END16(6 + 2 + 2 + bootImageLength());
}

//******************************************************************************
// Description:
//   Assigns a description to the MODDSignal.
// Parameters:
//   text  [in] The description (NULL terminated).
//******************************************************************************
void MODDSignal::description(const char* text)
{
   assert((primitive() == ChangeResponse || primitive() == RevertResponse) &&
          "The description field is not available for this primitive");

   // Determine the length of the file name
   size_t length = (text != NULL ? strlen(text) : 0);

   // Make sure that we will not write the descirption text outside the
   // allocated space for it in the MODDSignal.
   if((length + 1) > (MaximumDataLength - DescriptionPosition))
   {
      assert(!"The supplied description text is too long, it will get truncated");
      length = (MaximumDataLength - DescriptionPosition) - 1;
   }

   if(length != 0)
   {
      memcpy(&m_data[DescriptionPosition], text, length);
      m_data[DescriptionPosition + length] = '\0';
   }
   else
   {
      assert(!"An invalid description text been supplied");
   }

   // Set the description length field, include the NULL character too.
   *reinterpret_cast<u_int16*>(&m_data[DescriptionLengthPosition]) = TO_LITTLE_END16(static_cast<u_int16>(length + 1));

   // Assign the data length to the message length field as this is now
   // known.
   //m_messageLength = TO_LITTLE_END16(2 + // Result code
   //                                  2 + // Description length
   //                                  descriptionLength());
   m_messageLength = TO_LITTLE_END16(2 + 2 +  descriptionLength());
}

//******************************************************************************
// Description:
//    Tells whether or not the running system endian is LITTLE or BIG.
// Returns:
//   true if the running system is LITTLE endian, false if BIG.
//******************************************************************************
bool MODDSignal::isHostLittleEndian()
{
   int determineEndian = 1;
   char *determineEndianByte0 = reinterpret_cast<char*>(&determineEndian);
   return *determineEndianByte0 == 1 ? true : false;
}
