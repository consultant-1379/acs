//******************************************************************************
// 
// .NAME
//    DHCPServerAccess - Provides access to the DHCP server using the DHCP
//                       Server Management API.
// .LIBRARY 3C++
// .PAGENAME <DHCPServerAccess>
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
//  The DHCPServerAccess class is supposed to be used when accessing the DHCP
//  server. Accesses to the DHCPServerAccess's methods are thread safe hence
//  guarantee single thread operation towards the DHCP server independent of
//  the number of instances created from the DHCPServerAccess class.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-25 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef DHCPSERVERACCESS_H
#define DHCPSERVERACCESS_H

#ifdef _MSC_VER
#if _MSC_VER > 1200
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif
#endif // _MSC_VER

#include "DHCPConfiguration.H"
#include "Scope.H"
#include "PlatformTypes.hxx"
#include <winsock2.h>
#include <windows.h>
#include <Dhcpsapi.h>

class ClientInformation;

// Member functions, constructors, destructors, operators
class DHCPServerAccess : public DHCPConfiguration
{
public:
   enum DHCP_Server_Management_API_Error_Codes
   {
      ERROR_DHCP_REGISTRY_INIT_FAILED = 20000,
      ERROR_DHCP_DATABASE_INIT_FAILED = 20001,
      ERROR_DHCP_RPC_INIT_FAILED = 20002,
      ERROR_DHCP_NETWORK_INIT_FAILED = 20003,
      ERROR_DHCP_SUBNET_EXISTS_1 = 20004,
      ERROR_DHCP_SUBNET_NOT_PRESENT = 20005,
      ERROR_DHCP_PRIMARY_NOT_FOUND = 20006,
      ERROR_DHCP_ELEMENT_CANT_REMOVE = 20007,
      ERROR_DHCP_OPTION_EXISTS = 20009,
      ERROR_DHCP_OPTION_NOT_PRESENT = 20010,
      ERROR_DHCP_ADDRESS_NOT_AVAILABLE = 20011,
      ERROR_DHCP_RANGE_FULL = 20012,
      ERROR_DHCP_JET_ERROR = 20013,
      ERROR_DHCP_CLIENT_EXISTS = 20014,
      ERROR_DHCP_INVALID_DHCP_MESSAGE = 20015,
      ERROR_DHCP_INVALID_DHCP_CLIENT = 20016,
      ERROR_DHCP_SERVICE_PAUSED = 20017,
      ERROR_DHCP_NOT_RESERVED_CLIENT = 20018,
      ERROR_DHCP_RESERVED_CLIENT = 20019,
      ERROR_DHCP_RANGE_TOO_SMALL = 20020,
      ERROR_DHCP_IPRANGE_EXISTS = 20021,
      ERROR_DHCP_RESERVEDIP_EXISTS = 20022,
      ERROR_DHCP_INVALID_RANGE = 20023,
      ERROR_DHCP_RANGE_EXTENDED = 20024,
      ERROR_DHCP_RANGE_EXTENSION_TOO_SMALL = 20025,
      ERROR_DHCP_WARNING_RANGE_EXTENDED_LESS = 20026,
      ERROR_DHCP_JET_CONV_REQUIRED = 20027,
      ERROR_DHCP_SERVER_INVALID_BOOT_FILE_TABLE = 20028,
      ERROR_DHCP_SERVER_UNKNOWN_BOOT_FILE_NAME = 20029,
      ERROR_DHCP_SUPER_SCOPE_NAME_TOO_LONG = 20030,
      ERROR_DHCP_IP_ADDRESS_IN_USE = 20032,
      ERROR_DHCP_LOG_FILE_PATH_TOO_LONG = 20033,
      ERROR_DHCP_UNSUPPORTED_CLIENT = 20034,
      ERROR_DHCP_SERVER_INTERFACE_NOTIFICATION_EVENT = 20035,
      ERROR_DHCP_JET97_CONV_REQUIRED = 20036,
      ERROR_DHCP_ROGUE_INIT_FAILED = 20037,
      ERROR_DHCP_ROGUE_SAMSHUTDOWN = 20038,
      ERROR_DHCP_ROGUE_NOT_AUTHORIZED = 20039,
      ERROR_DHCP_ROGUE_DS_UNREACHABLE = 20040,
      ERROR_DHCP_ROGUE_DS_CONFLICT = 20041,
      ERROR_DHCP_ROGUE_NOT_OUR_ENTERPRISE = 20042,
      ERROR_DHCP_STANDALONE_IN_DS = 20043,
      ERROR_DHCP_CLASS_NOT_FOUND = 20044,
      ERROR_DHCP_CLASS_ALREADY_EXISTS = 20045,
      ERROR_DHCP_SCOPE_NAME_TOO_LONG = 20046,
      ERROR_DHCP_DEFAULT_SCOPE_EXISTS = 20047,
      ERROR_DHCP_CANT_CHANGE_ATTRIBUTE = 20048,
      ERROR_DHCP_IPRANGE_CONV_ILLEGAL = 20049,
      ERROR_DHCP_NETWORK_CHANGED = 20050,
      ERROR_DHCP_CANNOT_MODIFY_BINDINGS = 20051,
      ERROR_DHCP_SUBNET_EXISTS_2 = 20052,
      ERROR_DHCP_MSCOPE_EXISTS = 20053,
      ERROR_DHCP_MSCOPE_RANGE_TOO_SMALL = 20054,
      ERROR_DDS_NO_DS_AVAILABLE = 20070,
      ERROR_DDS_NO_DHCP_ROOT = 20071,
      ERROR_DDS_UNEXPECTED_ERROR = 20072,
      ERROR_DDS_TOO_MANY_ERRORS = 20073,
      ERROR_DDS_DHCP_SERVER_NOT_FOUND = 20074,
      ERROR_DDS_OPTION_ALREADY_EXISTS = 20075,
      ERROR_DDS_OPTION_DOES_NOT_EXIST = 20076,
      ERROR_DDS_CLASS_EXISTS = 20077,
      ERROR_DDS_CLASS_DOES_NOT_EXIST = 20078,
      ERROR_DDS_SERVER_ALREADY_EXISTS = 20079,
      ERROR_DDS_SERVER_DOES_NOT_EXIST = 20080,
      ERROR_DDS_SERVER_ADDRESS_MISMATCH = 20081,
      ERROR_DDS_SUBNET_EXISTS = 20082,
      ERROR_DDS_SUBNET_HAS_DIFF_SUPER_SCOPE = 20083,
      ERROR_DDS_SUBNET_NOT_PRESENT = 20084,
      ERROR_DDS_RESERVATION_NOT_PRESENT = 20085,
      ERROR_DDS_RESERVATION_CONFLICT = 20086,
      ERROR_DDS_POSSIBLE_RANGE_CONFLICT = 20087,
      ERROR_DDS_RANGE_DOES_NOT_EXIST = 20088
   };

   // Constructor.
   DHCPServerAccess(u_int32 DHCPAddress, u_int32 theNetmask);

private:
   static const size_t MaxIPAddressUnicodeLength = 62;

   // Assign the DHCP server address.
   void address(DWORD dhcpAddress);

   // These methods are using functions that are not supported by Microsoft,
   // hence is unsupported.
   Status unsupportedChangeBootFilename(const Scope& scope, const ClientInformation* client,
                                        u_int16 filenameLength, const char* filename);
   Status unsupportedExistsScope(DWORD scope, bool &existsIt);

   // Performs error code to string conversion.
   void interpretAdditionalError(DWORD status);

   // Performs option value interpretations.
   void interpretOptionValue(LPDHCP_OPTION_VALUE dhcpOptionValue);

   // The IP address uniencoded as a string.
   WCHAR m_IPAddressUnicode[MaxIPAddressUnicodeLength];
};

#endif
