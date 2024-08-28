//******************************************************************************
//
// NAME
//      RegistryInformation
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
//  See RegistryInformation.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2006-04-19 by EAB/AZA/DG Dan Andersson

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

#include "RegistryInformation.H"
#include "EventReporter.H"
#include "Log.H"

#include <tchar.h>

#define DHCP_SERVER_REG_PARAMETER_ROOT_KEY "System\\CurrentControlSet\\Services\\DHCPServer\\Parameters"
#define DHCP_SERVER_REG_PARAMETER_IGNOREBROADCASTFLAG_KEY_NAME "IgnoreBroadcastFlag"
#define DHCP_SERVER_REG_PARAMETER_DHCPALERTPERCENTAGE_KEY_NAME "DhcpAlertPercentage"
#define DHCP_SERVER_REG_PARAMETER_DHCPALERTCOUNT_KEY_NAME "DhcpAlertCount"
#define CLUSTER_REG_PARAMETER_ROOT_KEY "Cluster"
#define CLUSTER_REG_PARAMETER_APCFG "APCFG"
#define BUFSIZE 80

//******************************************************************************
// Description:
//    Sets the DHCP IgnoreBroadcastFlag key value to the keyValueIn parameter.
//    The key is created if necessary. If a change in registry, an event is logged.
// Parameters:
//    keyValueIn     The wanted value of the IgnoreBroadcastFlag in the registry.
//    isFlagChanged  The out parameter is only valid if the metod returns successfully.
//                   If true, the key value has been changed, or has been set
//                   from scratch (the key didn't exist). True should mean that the
//                   DHCP service must be restarted.
//                   If false, means that the key value was already correctly set.
// Returns:
//    OK if the key value was already correctly set or if an explicit set was 
//    successful, otherwise NotOK.
//******************************************************************************
RegistryInformation::Status RegistryInformation::setIgnoreBroadcastFlag(DWORD const theValue, 
                                                                        bool& isFlagChanged) const
{
   return setKeyValue(theValue,
      DHCP_SERVER_REG_PARAMETER_ROOT_KEY, 
      DHCP_SERVER_REG_PARAMETER_IGNOREBROADCASTFLAG_KEY_NAME,
      isFlagChanged);
}

//******************************************************************************
// Description:
//    Sets the DHCP DhcpAlertPercentage key value to the keyValueIn parameter.
//    The key is created if necessary. If a change in registry, an event is logged.
// Parameters:
//    keyValueIn     The wanted value of the DhcpAlertPercentage in the registry. (1-100%)
//    isFlagChanged  The out parameter is only valid if the metod returns successfully.
//                   If true, the key value has been changed, or has been set
//                   from scratch (the key didn't exist). True should mean that the
//                   DHCP service must be restarted.
//                   If false, means that the key value was already correctly set.
// Returns:
//    OK if the key value was already correctly set or if an explicit set was 
//    successful, otherwise NotOK.
//******************************************************************************
RegistryInformation::Status RegistryInformation::setDhcpAlertPercentage(DWORD const theValue, 
                                                                        bool& isFlagChanged) const
{
   return setKeyValue(theValue,
      DHCP_SERVER_REG_PARAMETER_ROOT_KEY, 
      DHCP_SERVER_REG_PARAMETER_DHCPALERTPERCENTAGE_KEY_NAME,
      isFlagChanged);
}

//******************************************************************************
// Description:
//    Sets the DHCP DhcpAlertCount key value to the keyValueIn parameter.
//    The key is created if necessary. If a change in registry, an event is logged.
// Parameters:
//    keyValueIn     The wanted value of the DhcpAlertCount in the registry.
//    isFlagChanged  The out parameter is only valid if the metod returns successfully.
//                   If true, the key value has been changed, or has been set
//                   from scratch (the key didn't exist). True should mean that the
//                   DHCP service must be restarted.
//                   If false, means that the key value was already correctly set.
// Returns:
//    OK if the key value was already correctly set or if an explicit set was 
//    successful, otherwise NotOK.
//******************************************************************************
RegistryInformation::Status RegistryInformation::setDhcpAlertCount(DWORD const theValue, 
                                                                   bool& isFlagChanged) const
{
   return setKeyValue(theValue,
      DHCP_SERVER_REG_PARAMETER_ROOT_KEY, 
      DHCP_SERVER_REG_PARAMETER_DHCPALERTCOUNT_KEY_NAME,
      isFlagChanged);
}

//******************************************************************************
// Description:
//    Gets the APCFG key value.
// Returns:
//    One of the APConfig enumerations.
//******************************************************************************
RegistryInformation::APConfig RegistryInformation::getAPCfg() const
{
   HKEY hKey;
   TCHAR szAPCfgValue[BUFSIZE];
   DWORD dwBufLen = BUFSIZE;
   LONG lRet;

   lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
      CLUSTER_REG_PARAMETER_ROOT_KEY, 0, KEY_QUERY_VALUE, &hKey);

   if(lRet != ERROR_SUCCESS)
   {
      return NotFound;
   }

   lRet = RegQueryValueEx(hKey, CLUSTER_REG_PARAMETER_APCFG,
      NULL, NULL, reinterpret_cast<LPBYTE>(szAPCfgValue), &dwBufLen);

   if((lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE))
   {
      return NotFound;
   }

   static_cast<void>(RegCloseKey(hKey));

   APConfig rVal = Unknown;
   if(lstrcmpi("AP1", szAPCfgValue) == 0)
   {
      rVal = AP1;
   }
   else if (lstrcmpi("AP2", szAPCfgValue) == 0)
   {
      rVal = AP2;
   }
   //else
   //  ;
   
   return rVal;
}

//******************************************************************************
// Description:
//    Private Method
//    Sets a registry key value.
//    The key is created if necessary. If a change in registry, an event is logged.
// Parameters:
//    keyValueIn     The wanted value of the key in the registry.
//    isFlagChanged  The out parameter is only valid if the metod returns successfully.
//                   If true, the key value has been changed, or has been set
//                   from scratch (the key didn't exist). True should mean that the
//                   DHCP service must be restarted.
//                   If false, means that the key value was already correctly set.
// Returns:
//    OK if the key value was already correctly set or if an explicit set was 
//    successful, otherwise NotOK.
//******************************************************************************
RegistryInformation::Status RegistryInformation::setKeyValue(DWORD const theValue,
                                                             TCHAR* theRootKey,
                                                             TCHAR* theKey,
                                                             bool& isFlagChanged) const
{
   HKEY hKey;
   TCHAR * rootKeyName = theRootKey;
   TCHAR * keyName = theKey;
   DWORD dwDisp;
   LONG result;
   DWORD keyValueIn = theValue; 
   DWORD keyValueOut;
   DWORD keyValueOutLen = sizeof(DWORD);

   isFlagChanged = false;

   // Try to open the root key. If it doesn't exist, it's created.
   result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, rootKeyName, 
                           0, NULL, REG_OPTION_NON_VOLATILE,
                           KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);
   if(result != ERROR_SUCCESS) 
   {
      // Severe
      DBGLOG("RegCreateKeyEx returned error code %d for the root key HKEY_LOCAL_MACHINE\\%s.\n",
             result, rootKeyName);
      if(hKey)
         static_cast<void>(RegCloseKey(hKey));
      return NotOK;
   }
   
   // Retrive the keyName's value. If it's not found create it.
   result = RegQueryValueEx(hKey, keyName, 0, 0, 
                            reinterpret_cast<LPBYTE>(&keyValueOut), 
                            &keyValueOutLen);
   if(result != ERROR_SUCCESS)
   {
      if(result == ERROR_FILE_NOT_FOUND)
      {
         // The keyName's value wasn't defined. Create it and set it's correct value.
         result = RegSetValueEx(hKey, keyName, 0, REG_DWORD, 
                                reinterpret_cast<LPBYTE>(&keyValueIn),
                                sizeof(DWORD));
         if(result == ERROR_SUCCESS)
         {
            // Success, the keyName has been created and it's value has been set.
            isFlagChanged = true;
            if(hKey)
               static_cast<void>(RegCloseKey(hKey));
            // Make the change to the DHCP registry key visible in the event log
            sendEvent(keyName, keyValueIn);
            return OK;
         }
      }
      // Severe, either problem with querying or setting the keyName
      DBGLOG("RegQuery/SetValueEx returned error code %d for the key name HKEY_LOCAL_MACHINE\\%s\\%s.\n",
             result, rootKeyName, keyName);
      if(hKey)
         static_cast<void>(RegCloseKey(hKey));
      return NotOK;
   } 

   // Check the successfully retrived keyName's value.
   if(keyValueOut != keyValueIn)
   {
      // The keyName's value is incorrect. Set it to the right value. 
      result = RegSetValueEx(hKey, keyName, 0, REG_DWORD,
                             reinterpret_cast<LPBYTE>(&keyValueIn),
                             sizeof(DWORD));
      if(result != ERROR_SUCCESS)
      {
         // Severe
         DBGLOG("RegSetValueEx returned error code %d for the key name HKEY_LOCAL_MACHINE\\%s\\%s when setting it to %d.\n",
                result, rootKeyName, keyName, keyValueIn);
         if(hKey)
            static_cast<void>(RegCloseKey(hKey));
         return NotOK;
      }
      // Success, the keyName's value has been correctly set,
      isFlagChanged = true;
   }

   // Success, the keyName's value has been correctly set to 0, or it 
   // was already correctly set to 0.
   if(hKey)
      static_cast<void>(RegCloseKey(hKey));
   if(isFlagChanged)
   {
      // Make the change to the DHCP registry key visible in the event log
      sendEvent(keyName, keyValueIn);
   }
   return OK;
}

void RegistryInformation::sendEvent(const TCHAR* key, DWORD const val) const
{
   static_cast<void> (EVENT(EventReporter::NonCritical,
      PROBLEM_DATA("The key name (HKEY_LOCAL_MACHINE\\%s\\%s) has been created or its value has been changed. The value is now set to %d.",
      DHCP_SERVER_REG_PARAMETER_ROOT_KEY,
      key, 
      val),
      PROBLEM_TEXT("")));
}