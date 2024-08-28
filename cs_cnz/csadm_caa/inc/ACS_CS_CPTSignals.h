//******************************************************************************
// 
// .NAME
//    CPTSignals - Contain CPT signal descriptions.
// .LIBRARY 3C++
// .PAGENAME <CLASSNAME>
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
//  Contains definitions of the CPT signals CPTREADMAUREG, CPTREADMAUREGR and
//  CPTKEEPALIVE.

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
#ifndef CPTSIGNALS_H 
#define CPTSIGNALS_H

#include "ACS_CS_PlatformTypes.h"

// Member functions, constructors, destructors, operators
class CPTREADMAUREG
{
public:

  enum OrderCode
   {
      UPBB_PROM_REVISION = 22, // APZ21240
      CPBB_ETH0 = 40,          // APZ21240
      CPBB_ETH1 = 41,          // APZ21240
      CPSB_ETH0 = 44,          // APZ21250
      CPSB_ETH1 = 45,          // APZ21250
      PCIH_OWN  = 46,          // APZ21250
      PCIH_TWIN = 47           // APZ21250
   };

   enum CPSide
   {
      CP_A = 0,
      CP_B = 1
   };

   static const u_int32 SignalNumber = 32452;
   static const u_int32 ReturnSignalNumber = 32453;

   // Constructor
   CPTREADMAUREG(OrderCode theOrderCode, CPSide theCPSide)
   {
      m_signalLength = (sizeof(CPTREADMAUREG) - 1 /* m_signalLength */);
      *reinterpret_cast<u_int32*>(m_signalNumber) = TO_BIG_END32(SignalNumber);
      *reinterpret_cast<u_int16*>(m_returnSignalNumber) = TO_BIG_END16(ReturnSignalNumber);
      *reinterpret_cast<u_int16*>(m_cptReference) = TO_BIG_END16(static_cast<u_int16>(theOrderCode));
      *reinterpret_cast<u_int16*>(m_senderID) = TO_BIG_END16(5); // PCCTB
      *reinterpret_cast<u_int16*>(m_CPSide) = TO_BIG_END16(static_cast<u_int16>(theCPSide));
      *reinterpret_cast<u_int16*>(m_orderCode) = TO_BIG_END16(static_cast<u_int16>(theOrderCode));
      *reinterpret_cast<u_int16*>(m_numberOfData) = TO_BIG_END16(1); // One data only should be returned (one MAC address)
      *reinterpret_cast<u_int32*>(m_address) = 0;
   }

   // Returns a pointer to the beginning of the signal.
   const char* message() const
   {
      return reinterpret_cast<const char*>(this);
   }

   // Returns the byte size of the signal.
   u_int32 size() const
   {
      return sizeof(CPTREADMAUREG);
   }

   // Returns the order code in the signal.
   OrderCode orderCode() const
   {
      return static_cast<OrderCode>(FROM_BIG_END16(*reinterpret_cast<const u_int16*>(m_orderCode)));
   }

   // Returns the CP side information in the signal.
   CPSide cpSide() const
   {
      return static_cast<CPSide>(FROM_BIG_END16(*reinterpret_cast<const u_int16*>(m_CPSide)));
   }

private:
   // The length of the signal excluding the m_signalLength field
   u_int8 m_signalLength;

   // Signal data fields
   u_int8 m_signalNumber[4];
   u_int8 m_returnSignalNumber[2];
   u_int8 m_cptReference[2];
   u_int8 m_senderID[2];
   u_int8 m_CPSide[2];
   u_int8 m_orderCode[2];
   u_int8 m_numberOfData[2];
   u_int8 m_address[4];
};

class CPTREADMAUREGR
{
public:
   enum FaultCode
   {
      NoFault = 0
   };

   static const u_int32 SignalNumber = 32453;

   // Default constructor
   CPTREADMAUREGR()
   {
      // Fill the entire data structure with junk.
      memset(this, 0xFF, sizeof(CPTREADMAUREGR));
   }

   // Constructor
   CPTREADMAUREGR(const CPTREADMAUREG& cptreadmaureg, char* macAddress, u_int16 length) :
      m_signalLength(sizeof(CPTREADMAUREGR))
   {
      u_int16 theNumberOfData = (length <= 34 ? length : 34);
      *reinterpret_cast<u_int32*>(m_signalNumber) = TO_BIG_END32(SignalNumber);
      *reinterpret_cast<u_int16*>(m_cptReference) = TO_BIG_END16(static_cast<u_int16>(cptreadmaureg.orderCode()));
      *reinterpret_cast<u_int16*>(m_senderID) = TO_BIG_END16(5); // PCCTB
      *reinterpret_cast<u_int16*>(m_faultCode) = 0; // No fault
      *reinterpret_cast<u_int16*>(m_numberOfData) = TO_BIG_END16(theNumberOfData);
      *reinterpret_cast<u_int16*>(m_dataFormat) = TO_BIG_END16(8);

      memcpy(m_data, macAddress, theNumberOfData);
   }

   // Returns the signal number in the signal.
   u_int32 signalNumber() const
   {
      return FROM_BIG_END32(*reinterpret_cast<const u_int32*>(m_signalNumber));
   }

   // Returns the order code in the signal.
   CPTREADMAUREG::OrderCode orderCode() const
   {
      return static_cast<CPTREADMAUREG::OrderCode>(FROM_BIG_END16(*reinterpret_cast<const u_int16*>(m_cptReference)));
   }

   // Returns the fault code in the signal.
   FaultCode faultCode() const
   {
      return static_cast<FaultCode>(FROM_BIG_END16(*reinterpret_cast<const u_int16*>(m_faultCode)));
   }

   // Returns the MAC address in the signal.
   const char* MACAddress() const
   {
      return reinterpret_cast<const char*>(m_data);
   }

   // Returns a pointer to the beginning of the signal.
   const char* message() const
   {
      return reinterpret_cast<const char*>(this);
   }

   // Returns the byte size of the signal.
   u_int32 size() const
   {
      return sizeof(CPTREADMAUREGR);
   }

   // Returns the number of data in the signal.
   u_int16 numberOfData() const
   {
      return FROM_BIG_END16(*reinterpret_cast<const u_int16*>(m_numberOfData));
   }

private:
   // The length of the signal including the m_signalLength field
   u_int8 m_signalLength;

   // Signal data fields
   u_int8 m_signalNumber[4];
   u_int8 m_cptReference[2];
   u_int8 m_senderID[2];
   u_int8 m_faultCode[2];
   u_int8 m_numberOfData[2];
   u_int8 m_dataFormat[2];
   u_int8 m_data[34];
};

class CPTKEEPALIVE
{
public:
   static const u_int32 SignalNumber = 32710;
   static const u_int32 ReturnSignalNumber = 32711;

   enum CPSide
   {
      CP_A = 0,
      CP_B = 1
   };

   // Constructor
   CPTKEEPALIVE(CPSide theCPSide, u_int16 cptReference)
   {
      m_signalLength = (sizeof(CPTKEEPALIVE) - 1 /* m_signalLength */);
      *reinterpret_cast<u_int32*>(m_signalNumber) = TO_BIG_END32(SignalNumber);
      *reinterpret_cast<u_int16*>(m_returnSignalNumber) = TO_BIG_END16(ReturnSignalNumber);
      *reinterpret_cast<u_int16*>(m_cptReference) = TO_BIG_END16(cptReference);
      *reinterpret_cast<u_int16*>(m_senderID) = TO_BIG_END16(5); // PCCTB
      *reinterpret_cast<u_int16*>(m_CPSide) = TO_BIG_END16(static_cast<u_int16>(theCPSide));
   }

   // Returns a pointer to the beginning of the signal.
   const char* message() const
   {
      return reinterpret_cast<const char*>(this);
   }

   // Returns the byte size of the signal.
   u_int32 size() const
   {
      return sizeof(CPTKEEPALIVE);
   }

   // Returns the CP side information in the signal.
   CPSide cpSide() const
   {
      return static_cast<CPSide>(FROM_BIG_END16(*reinterpret_cast<const u_int16*>(m_CPSide)));
   }

private:
   // The length of the signal excluding the m_signalLength field
   u_int8 m_signalLength;

   // Signal data fields
   u_int8 m_signalNumber[4];
   u_int8 m_returnSignalNumber[2];
   u_int8 m_cptReference[2];
   u_int8 m_senderID[2];
   u_int8 m_CPSide[2];
};

#endif
