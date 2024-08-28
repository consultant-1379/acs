//******************************************************************************
//
// NAME
//      UnsupportedDHCPServerAccess
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
//  This file contains use of undocumented DHCP Server Management API
//  functions.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-25 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .SEE ALSO 
//  N/A.
//
//******************************************************************************
#include "DHCPServerAccess.H"
#include "ClientInformation.H"
#include "EventReporter.H"
#include "MultiPlatformMutex.hxx"
#include "Log.H"
#include <cassert>
#include <winsock2.h>
#include <windows.h>
#include <dhcpsapi.h>
#include <stdio.h>

#if WINVER >= 0x0500 // Pre Windows Server 2000
typedef DWORD (WINAPI* TypeDhcpSetOptionValue)(
   IN      LPWSTR                 ServerIpAddress,
   IN      DWORD                  Flags,
   IN      DHCP_OPTION_ID         OptionId,
   IN      LPWSTR                 ClassName,
   IN      LPWSTR                 VendorName,
   IN      LPDHCP_OPTION_SCOPE_INFO ScopeInfo,
   IN      LPDHCP_OPTION_DATA     OptionValue
   );
#define DHCPSETOPTIONVALUE "DhcpSetOptionValueV5"
#else
typedef DWORD (WINAPI* TypeDhcpSetOptionValue(
               LPWSTR ServerIpAddress,
               DHCP_OPTION_ID OptionID,
               LPDHCP_OPTION_SCOPE_INFO ScopeInfo,
               LPDHCP_OPTION_DATA OptionValue
               );
#define DHCPSETOPTIONVALUE L"DhcpSetOptionValue"
#endif

#define DHCPSAPIDLL "dhcpsapi.dll"

namespace
{
   MultiPlatformMutex DHCPServerAccessMutex;
}

// This method is using functions that are not supported by Microsoft,
// hence is unsupported.
DHCPServerAccess::Status DHCPServerAccess::unsupportedChangeBootFilename(const Scope& scope,
                                                                         const ClientInformation* client,
                                                                         u_int16 filenameLength,
                                                                         const char* filename)
{
   SafeMutex mutex(DHCPServerAccessMutex);
   HMODULE hDhcpsapidll = NULL;
   TypeDhcpSetOptionValue DhcpSetOptionValue;
   DHCP_OPTION_SCOPE_INFO ScopeInfo;
   DHCP_OPTION_DATA OptionData;
   DHCP_OPTION_ID OptionID;
   DHCP_OPTION_DATA_ELEMENT OptionDataElement;
   const int MaxBootFileNameLength = 256;
   WCHAR newBootFilename[MaxBootFileNameLength];

   if(filenameLength >= MaxBootFileNameLength)
   {
      DBGLOG("Cannot change to a boot filename that is this long (%d bytes), %d bytes is max\n", filenameLength, MaxBootFileNameLength);
      return Failure;
   }

#if WINVER >= 0x0500
   LPWSTR pwcClassname = NULL;
   LPWSTR pwcVendor = NULL;
#endif

   // Since you already will have loaded the library implicitly by linking in the lib file you should
   // be able to get away with GetModuleHandle but to fallback you can loadlibrary to get module handle
   if((hDhcpsapidll = GetModuleHandle(DHCPSAPIDLL)) == NULL)
   { 
      printf("Unable to locate %s in memory!\n",DHCPSAPIDLL);
      if((hDhcpsapidll = LoadLibrary(DHCPSAPIDLL)) == NULL)
      {
         static_cast<void>(EVENT(EventReporter::SubSystemError,
               PROBLEM_DATA("Unable to load %s",
                            DHCPSAPIDLL),
               PROBLEM_TEXT("")));

         return Failure;
      }
   }

   if((DhcpSetOptionValue = (TypeDhcpSetOptionValue)GetProcAddress(hDhcpsapidll, DHCPSETOPTIONVALUE)) == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("DhcpSetOptionValue() export not found!"),
            PROBLEM_TEXT("")));

      return Failure; 
   } 

   // HACKHACK - I'm only going to implement this for WINVER >= 0x0500
   memset(&OptionID, 0x00, sizeof(DHCP_OPTION_ID));
   memset(&ScopeInfo, 0x00, sizeof(DHCP_OPTION_SCOPE_INFO));
   memset(&OptionData, 0x00, sizeof(DHCP_OPTION_DATA));
   memset(&OptionDataElement, 0x00, sizeof(DHCP_OPTION_DATA_ELEMENT));

   ScopeInfo.ScopeType = DhcpReservedOptions;
   ScopeInfo.ScopeInfo.ReservedScopeInfo.ReservedIpAddress = client->ipAddress();
   ScopeInfo.ScopeInfo.ReservedScopeInfo.ReservedIpSubnetAddress = scope.address();

   // Calculate the number of bytes required for the conversion to be
   // successful.
   size_t requiredBytesForConversion = mbstowcs(NULL, filename, filenameLength + 1 /* Include the null character in the conversion*/);

   if(requiredBytesForConversion > MaxBootFileNameLength ||
      requiredBytesForConversion == static_cast<size_t>(-1))
   {
      static_cast<void>(EVENT(EventReporter::ProgrammingError,
            PROBLEM_DATA("mbstowcs(NULL, %s, %d) reports that %d bytes are required for the conversion, %d bytes are avilable. The conversion failed",
                         filename,
                         filenameLength + 1,
                         requiredBytesForConversion,
                         MaxBootFileNameLength),
            PROBLEM_TEXT("")));

      return Failure;
   }

   if(requiredBytesForConversion != mbstowcs(newBootFilename, filename, filenameLength + 1 /* Include the null character in the conversion*/))
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("mbstowcs(dest, %s, %d) failed",
                         filename,
                         filenameLength + 1),
            PROBLEM_TEXT("")));

      return Failure;
   }

   OptionDataElement.OptionType = DhcpStringDataOption; // In the Case of Boot Image This value is a string
   OptionDataElement.Element.StringDataOption = newBootFilename;

   OptionData.NumElements = 1;
   OptionData.Elements = &OptionDataElement;

   OptionID = 67; // Boot Image filename

   DWORD result = DhcpSetOptionValue(m_IPAddressUnicode,
                                     0,
                                     OptionID,
                                     pwcClassname,
                                     pwcVendor,
                                     &ScopeInfo,
                                     &OptionData);

   if(result != ERROR_SUCCESS)
   {
      DBGLOG("Unsupported function DhcpSetOptionValue returned error code %d\n", result);
   }
   return result == ERROR_SUCCESS ? OK : Failure;
}

//******************************************************************************
// Description:
//    Checks if a scope exist in the DHCP server.
// Parameters:
//    scope          The scope.
//    existsIt(out)  True if the scope is found.
//                   The parameter has a valid value only if OK is returned.
// Returns: 
//    OK when the scope has been succesfully checked for, otherwise Failure.
//******************************************************************************
DHCPConfiguration::Status DHCPServerAccess::unsupportedExistsScope(DWORD scope,
                                                                   bool &existsIt)
{
   // Make sure all access is thread safe.
   SafeMutex mutex(DHCPServerAccessMutex);

   if(scope == 0)
   {
      // It will not be possible to read any information for this scope.
      return Failure;
   }

   LPDHCP_SUBNET_INFO subnetInfo;
   // Try to get information of the scope
   DWORD result = DhcpGetSubnetInfo(m_IPAddressUnicode, scope, &subnetInfo);
   switch(result)
   {
   case ERROR_SUCCESS:
      // The scope was found
      existsIt = true;
      return OK;
   case 20005: //ERROR_DHCP_SUBNET_NOT_PRESENT
      // The scope was succesfully not found
      existsIt = false;
      return OK;
   default:
      // Failure
      DBGLOG("Unsupported function DhcpGetSubnetInfo() returned error code %d\n", result);
      static_cast<void>(EVENT(EventReporter::DHCPFailure,
         PROBLEM_DATA("Fault code from DHCP:%d", result),
         PROBLEM_TEXT("")));
      return Failure;
   }
}