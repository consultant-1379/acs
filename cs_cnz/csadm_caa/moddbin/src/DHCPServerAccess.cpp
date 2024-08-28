//******************************************************************************
//
// NAME
//      DHCPServerAccess
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
//  See DHCPServerAccess.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-06-09 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .SEE ALSO 
//  N/A.
//
//******************************************************************************
#ifdef _MSC_VER
#if _MSC_VER > 1200
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif
#endif // _MSC_VER

#include "DHCPServerAccess.H"
#include "PlatformTypes.hxx"
#include "ClientInformation.H"
#include "EventReporter.H"
#include "EventReporter.H"
#include "MultiPlatformMutex.hxx"
#include "Log.H"
#include "TCPClientServer.hxx"
#include <winsock2.h>
#include <windows.h>
#include <dhcpsapi.h>
#include <cassert>

namespace
{
   MultiPlatformMutex DHCPServerAccessMutex;
}

// When in _DEBUG builds, we monitor the execution time for the netsh commands
// that we execute. When in !_DEBUG builds, the MONITOR macro is empty.
#ifdef _DEBUG
#include "PerformanceMonitor.hxx"
class ExecutionMonitor
{
public:
   ExecutionMonitor()
   {
      monitor.start();
   }
   ~ExecutionMonitor()
   {
      try
      {
         monitor.stop();

         DBGLOG("\n\t\tWallClock  : %I64d ms\n\t\tUser time  : %I64d ms\n\t\tKernel time: %I64d ms\n",
                monitor.wallClockTimeInMilliSec(),
                monitor.userTimeInMilliSec(),
                monitor.kernelTimeInMilliSec());
      }
      catch(...)
      {
      }
   }
private:
   PerformanceMonitor monitor;
};
#define MEASURE ExecutionMonitor theExecutionMonitor
#else // _DEBUG
#define MEASURE
#endif // _DEBUG

//******************************************************************************
// Description:
//   Constructor
// Parameters:
//    DHCPAddress  The address to use when accessing the DHCP server.
//    theNetmask   The netmask to use when accessing the DHCP server.
//******************************************************************************
DHCPServerAccess::DHCPServerAccess(u_int32 DHCPAddress, u_int32 theNetmask) :
   DHCPConfiguration(DHCPAddress, theNetmask)
{
   address(DHCPAddress);
}

//******************************************************************************
// Description:
//    Assigns the DHCP server address to use when accessing DHCP server table
//    data.
// Paramters:
//   dhcpIPAddress  The IP address of the DHCP server which should be accessed.
//******************************************************************************
void DHCPServerAccess::address(DWORD dhcpAddress)
{
   // Make sure all access is thread safe.
   SafeMutex mutex(DHCPServerAccessMutex);
   MEASURE;

   char* ip = TCPClientServer::IPAddressToString(dhcpAddress);

   // Calculate the number of bytes required for the conversion to be
   // successful.
   size_t requiredBytesForConversion = mbstowcs(NULL, ip, MaxIPAddressUnicodeLength);

   if(requiredBytesForConversion == static_cast<size_t>(-1))
   {
      EVENT(EventReporter::ProgrammingError,
            PROBLEM_DATA("mbstowcs(NULL, %s, %d) reports that %d bytes are required for the conversion, %d bytes are avilable. The conversion failed",
                         ip,
                         MaxIPAddressUnicodeLength,
                         requiredBytesForConversion,
                         MaxIPAddressUnicodeLength),
            PROBLEM_TEXT(""));
   }

   if(requiredBytesForConversion != mbstowcs(m_IPAddressUnicode, ip, MaxIPAddressUnicodeLength))
   {
      EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("mbstowcs(dest, %s, %d) failed",
                         ip,
                         MaxIPAddressUnicodeLength),
            PROBLEM_TEXT(""));
   }
}

void DHCPServerAccess::interpretAdditionalError(DWORD status)
{
   switch(status)
   {
   case ERROR_DHCP_REGISTRY_INIT_FAILED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_REGISTRY_INIT_FAILED: The DHCP server registry initialization parameters are incorrect."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_DATABASE_INIT_FAILED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_DATABASE_INIT_FAILED: The DHCP server was unable to open the database of DHCP clients."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_RPC_INIT_FAILED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_RPC_INIT_FAILED: The DHCP server was unable to start as a Remote Procedure Call (RPC) server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_NETWORK_INIT_FAILED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_NETWORK_INIT_FAILED: The DHCP server was unable to establish a socket connection."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SUBNET_EXISTS_1:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SUBNET_EXISTS: The specified subnet already exists on the DHCP server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SUBNET_NOT_PRESENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SUBNET_NOT_PRESENT: The specified subnet does not exist on the DHCP server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_PRIMARY_NOT_FOUND:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_PRIMARY_NOT_FOUND: The primary host information for the specified subnet was not found on the DHCP server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ELEMENT_CANT_REMOVE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ELEMENT_CANT_REMOVE: The specified DHCP element has been used by a client and cannot be removed."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_OPTION_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_OPTION_EXISTS: The specified option already exists on the DHCP server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_OPTION_NOT_PRESENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_OPTION_NOT_PRESENT: The specified option does not exist on the DHCP server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ADDRESS_NOT_AVAILABLE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ADDRESS_NOT_AVAILABLE: The specified IP address is not available."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_RANGE_FULL:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_RANGE_FULL: The specified IP address range has all of its member addresses leased."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_JET_ERROR:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_JET_ERROR: An error occurred while accessing the DHCP JET database. For more information about this error, please look at the DHCP server event log."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_CLIENT_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_CLIENT_EXISTS: The specified client already exists in the database."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_INVALID_DHCP_MESSAGE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_INVALID_DHCP_MESSAGE: The DHCP server received an invalid message."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_INVALID_DHCP_CLIENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_INVALID_DHCP_CLIENT: The DHCP server received an invalid message from the client."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SERVICE_PAUSED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SERVICE_PAUSED: The DHCP server is currently paused."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_NOT_RESERVED_CLIENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_NOT_RESERVED_CLIENT: The specified DHCP client is not a reserved client."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_RESERVED_CLIENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_RESERVED_CLIENT: The specified DHCP client is a reserved client."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_RANGE_TOO_SMALL:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_RANGE_TOO_SMALL: The specified IP address range is too small."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_IPRANGE_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_IPRANGE_EXISTS: The specified IP address range is already defined on the DHCP server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_RESERVEDIP_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_RESERVEDIP_EXISTS: The specified IP address is currently taken by another client."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_INVALID_RANGE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_INVALID_RANGE: The specified IP address range either overlaps with an existing range or is invalid."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_RANGE_EXTENDED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_RANGE_EXTENDED: The specified IP address range is an extension of an existing range."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_RANGE_EXTENSION_TOO_SMALL:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_RANGE_EXTENSION_TOO_SMALL: The specified IP address range extension is too small. The number of addresses in the extension must be a multiple of 32."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_WARNING_RANGE_EXTENDED_LESS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_WARNING_RANGE_EXTENDED_LESS: An attempt was made to extend the IP address range to a value less than the specified backward extension. The number of addresses in the extension must be a multiple of 32."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_JET_CONV_REQUIRED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_JET_CONV_REQUIRED: The DHCP database needs to be upgraded to a newer format. For more information, refer to the DHCP server event log."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SERVER_INVALID_BOOT_FILE_TABLE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SERVER_INVALID_BOOT_FILE_TABLE: The format of the bootstrap protocol file table is incorrect. The correct format is:\n<requested boot file name 1>,<boot file server name 1>, <boot file name 1>\n<requested boot file name 2>, <boot file server name 2>, <boot file name 2>"),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SERVER_UNKNOWN_BOOT_FILE_NAME:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SERVER_UNKNOWN_BOOT_FILE_NAME: A boot file name specified in the bootstrap protocol file table is unrecognized or invalid."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SUPER_SCOPE_NAME_TOO_LONG:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SUPER_SCOPE_NAME_TOO_LONG: The specified superscope name is too long."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_IP_ADDRESS_IN_USE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_IP_ADDRESS_IN_USE: The specified IP address is already in use."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_LOG_FILE_PATH_TOO_LONG:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_LOG_FILE_PATH_TOO_LONG: The specified path to the DHCP audit log file is too long."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_UNSUPPORTED_CLIENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_UNSUPPORTED_CLIENT: The DHCP server received a request for a valid IP address not administered by the server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SERVER_INTERFACE_NOTIFICATION_EVENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SERVER_INTERFACE_NOTIFICATION_EVENT: The DHCP server failed to receive a notification when the interface list changed, therefore some of the interfaces will not be enabled on the server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_JET97_CONV_REQUIRED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_JET97_CONV_REQUIRED: The DHCP database needs to be upgraded to a newer format (JET97). For more information, refer to the DHCP server event log."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ROGUE_INIT_FAILED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ROGUE_INIT_FAILED: The DHCP server cannot determine if it has the authority to run, and is not servicing clients on the network. This rogue status may be due to network problems or insufficient server resources."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ROGUE_SAMSHUTDOWN:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ROGUE_SAMSHUTDOWN: The DHCP service is shutting down because another DHCP server is active on the network."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ROGUE_NOT_AUTHORIZED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ROGUE_NOT_AUTHORIZED: The DHCP server does not have the authority to run, and is not servicing clients on the network."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ROGUE_DS_UNREACHABLE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ROGUE_DS_UNREACHABLE: The DHCP server is unable to contact the directory service for this domain. The DHCP server will continue to attempt to contact the directory service. During this time, no clients on the network will be serviced."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ROGUE_DS_CONFLICT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ROGUE_DS_CONFLICT: The DHCP server's authorization information conflicts with that of another DHCP server on the network."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_ROGUE_NOT_OUR_ENTERPRISE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_ROGUE_NOT_OUR_ENTERPRISE: The DHCP server is ignoring a request from another DHCP server because the second server is a member of a different directory service enterprise."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_STANDALONE_IN_DS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_STANDALONE_IN_DS: The DHCP server has detected a directory service environment on the network. If there is a directory service on the network, the DHCP server can only run if it is a part of the directory service. Since the server ostensibly belongs to a workgroup, it is terminating."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_CLASS_NOT_FOUND:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_CLASS_NOT_FOUND: The specified DHCP class name is unknown or invalid."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_CLASS_ALREADY_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_CLASS_ALREADY_EXISTS: The specified DHCP class name (or information) is already in use."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SCOPE_NAME_TOO_LONG:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SCOPE_NAME_TOO_LONG: The specified DHCP scope name is too long; the scope name must not exceed 256 characters."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_DEFAULT_SCOPE_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_DEFAULT_SCOPE_EXISTS: The default scope is already configured on the server."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_CANT_CHANGE_ATTRIBUTE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_CANT_CHANGE_ATTRIBUTE: The Dynamic BOOTP attribute cannot be turned on or off."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_IPRANGE_CONV_ILLEGAL:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_IPRANGE_CONV_ILLEGAL: Conversion of a scope to a \"DHCP Only\" scope or to a \"BOOTP Only\" scope is not allowed when the scope contains other DHCP and BOOTP clients. Either the DHCP or BOOTP clients should be specifically deleted before converting the scope to the other type."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_NETWORK_CHANGED:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_NETWORK_CHANGED: The network has changed. Retry this operation after checking for network changes. Network changes may be caused by interfaces that are new or invalid, or by IP addresses that are new or invalid."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_CANNOT_MODIFY_BINDINGS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_CANNOT_MODIFY_BINDINGS: The bindings to internal IP addresses cannot be modified."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_SUBNET_EXISTS_2:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_SUBNET_EXISTS: The DHCP scope parameters are incorrect. Either the scope already exists, or its properties are inconsistent with the subnet address and mask of an existing scope."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_MSCOPE_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_MSCOPE_EXISTS: The DHCP multicast scope parameters are incorrect. Either the scope already exists, or its properties are inconsistent with the subnet address and mask of an existing scope."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DHCP_MSCOPE_RANGE_TOO_SMALL:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DHCP_MSCOPE_RANGE_TOO_SMALL: The multicast scope range must have at least 256 IP addresses."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_NO_DS_AVAILABLE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_NO_DS_AVAILABLE: The DHCP server could not contact Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_NO_DHCP_ROOT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_NO_DHCP_ROOT: The DHCP service root could not be found in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_UNEXPECTED_ERROR:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_UNEXPECTED_ERROR: An unexpected error occurred while accessing Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_TOO_MANY_ERRORS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_TOO_MANY_ERRORS: There were too many errors to proceed."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_DHCP_SERVER_NOT_FOUND:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_DHCP_SERVER_NOT_FOUND: A DHCP service could not be found."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_OPTION_ALREADY_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_OPTION_ALREADY_EXISTS: The specified DHCP options are already present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_OPTION_DOES_NOT_EXIST:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_OPTION_DOES_NOT_EXIST: The specified DHCP options are not present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_CLASS_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_CLASS_EXISTS: The specified DHCP classes are already present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_CLASS_DOES_NOT_EXIST:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_CLASS_DOES_NOT_EXIST: The specified DHCP classes are not present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_SERVER_ALREADY_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_SERVER_ALREADY_EXISTS: The specified DHCP servers are already present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_SERVER_DOES_NOT_EXIST:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_SERVER_DOES_NOT_EXIST: The specified DHCP servers are not present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_SERVER_ADDRESS_MISMATCH:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_SERVER_ADDRESS_MISMATCH: The specified DHCP server address does not correspond to the identified DHCP server name."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_SUBNET_EXISTS:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_SUBNET_EXISTS: The specified subnets are already present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_SUBNET_HAS_DIFF_SUPER_SCOPE:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_SUBNET_HAS_DIFF_SUPER_SCOPE: The specified subnet belongs to a different superscope."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_SUBNET_NOT_PRESENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_SUBNET_NOT_PRESENT: The specified subnet is not present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_RESERVATION_NOT_PRESENT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_RESERVATION_NOT_PRESENT: The specified reservation is not present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_RESERVATION_CONFLICT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_RESERVATION_CONFLICT: The specified reservation conflicts with another reservation present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_POSSIBLE_RANGE_CONFLICT:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_POSSIBLE_RANGE_CONFLICT: The specified IP address range conflicts with another IP range present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   case ERROR_DDS_RANGE_DOES_NOT_EXIST:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("ERROR_DDS_RANGE_DOES_NOT_EXIST: The specified IP address range is not present in Active Directory."),
            PROBLEM_TEXT(""));
      break;

   default:
      EVENT(EventReporter::DHCPFailure,
            PROBLEM_DATA("Unknown result code received %d, last error is %d\n", status, GetLastError()),
            PROBLEM_TEXT(""));
   }
}

void DHCPServerAccess::interpretOptionValue(LPDHCP_OPTION_VALUE dhcpOptionValue)
{
   if(dhcpOptionValue)
   {
      LPDHCP_OPTION_DATA_ELEMENT optionDataElement;

      optionDataElement = dhcpOptionValue->Value.Elements;

      DBGLOG(", %d option value data exists\n", dhcpOptionValue->Value.NumElements);
      for(u_int32 i = 0; i < dhcpOptionValue->Value.NumElements; ++i, ++optionDataElement)
      {
         u_int32 j;

         DBGLOG("Element #%d: ", i);
         switch(optionDataElement->OptionType)
         {
         case DhcpByteOption:
            // The option data is stored as a BYTE value. 
            DBGLOG("DhcpByteOption %d\n", optionDataElement->Element.ByteOption);
            break;
         case DhcpWordOption:
            // The option data is stored as a WORD value. 
            DBGLOG("DhcpWordOption %d\n", optionDataElement->Element.WordOption);
            break;
         case DhcpDWordOption:
            // The option data is stored as a DWORD value. 
            DBGLOG("DhcpDWordOption %d\n", optionDataElement->Element.DWordOption);
            break;
         case DhcpDWordDWordOption:
            // The option data is stored as a DWORD_DWORD value. 
            DBGLOG("DhcpDWordDWordOption %d, %d\n", optionDataElement->Element.DWordDWordOption.DWord1, optionDataElement->Element.DWordDWordOption.DWord2);
            break;
         case DhcpIpAddressOption:
            // The option data is an IP address, stored as a DHCP_IP_ADDRESS value (DWORD). 
            DBGLOG("DhcpIpAddressOption %s\n", TCPClientServer::IPAddressToString(optionDataElement->Element.IpAddressOption));
            break;
         case DhcpStringDataOption:
            // The option data is stored as a Unicode string.
            {
               const int MAX_LEN = 255;
               char str[MAX_LEN + 1];

               if(optionDataElement->Element.StringDataOption)
               {
                  // Calculate the number of bytes required for the conversion to
                  // be successful.
                  size_t requiredBytesForConversion = wcstombs(NULL, optionDataElement->Element.StringDataOption, MAX_LEN);

                  if(requiredBytesForConversion == static_cast<size_t>(-1))
                  {
                     EVENT(EventReporter::ProgrammingError,
                           PROBLEM_DATA("mbstowcs(NULL, %s, %d) reports that %d bytes are required for the conversion, %d bytes are avilable. The conversion failed",
                                         optionDataElement->Element.StringDataOption,
                                         MAX_LEN,
                                         requiredBytesForConversion,
                                         MAX_LEN),
                           PROBLEM_TEXT(""));
                     return;
                  }

                  if(requiredBytesForConversion != wcstombs(str, optionDataElement->Element.StringDataOption, MAX_LEN))
                  {
                     EVENT(EventReporter::SubSystemError,
                           PROBLEM_DATA("wcstombs(dest, %s, %d) failed",
                                        optionDataElement->Element.StringDataOption,
                                        MAX_LEN),
                           PROBLEM_TEXT(""));
                     return;
                  }
               }
               else
               {
                  str[0] = '\0';
               }

               DBGLOG("DhcpStringDataOption %s\n", str);
            }
            break;
         case DhcpBinaryDataOption:
            // The option data is stored as a DHCP_BINARY_DATA structure. 
            for(j = 0; j < optionDataElement->Element.BinaryDataOption.DataLength; ++j)
            {
               DBGLOG("%x ", optionDataElement->Element.BinaryDataOption.Data[j]);
            }
            DBGLOG("\n");
            break;
         case DhcpEncapsulatedDataOption:
            // The option data is encapsulated and stored as a DHCP_BINARY_DATA structure. 
            for(j = 0; j < optionDataElement->Element.EncapsulatedDataOption.DataLength; ++j)
            {
               DBGLOG("%x ", optionDataElement->Element.EncapsulatedDataOption.Data[j]);
            }
            break;

         default:
            assert(!"Invalid option type");
         }
      }
   }
   else
   {
      DBGLOG(", option value data does not exist\n");
   }
}
