

//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************




#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "commondll.h"
#include "ACS_CS_Trace.h"

#include <process.h>
#include <sys/stat.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>


// ACS_CS_Logger
#include "ACS_CS_Logger.h"


ACS_CS_Trace_TDEF(ACS_CS_Logger_TRACE);

using namespace std;
using namespace ACS_CS_Protocol;

namespace
{
   const long MAX_LOG_FILE_SIZE = 400000;            // 400K Bytes
   const int  MAX_NUM_OF_BACKUP_FILES = 1;           // Create only one backup file
   const int  MAX_LENGTH = 256;                      // Max length of buffer (for C-style string)
   const string LOG_FILE_NAME("acs_cs_trace.log");
}



// Class ACS_CS_Logger 

ACS_CS_Logger* ACS_CS_Logger::_instance = 0;

 ACS_CS_ReaderWriterLock ACS_CS_Logger::instanceLock;

 ACS_CS_ReaderWriterLock ACS_CS_Logger::logFileLock;

ACS_CS_Logger::ACS_CS_Logger()
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Constructing ACS_CS_Logger",
      __FUNCTION__));

   // Reading paths for the storage of the table file and logs
   (void) getPath();       // Only data disk path can be read
   (void) readRegistry();  // Read system disk paths from registry

   // Create missing directories
   if ( ServiceType_MASTER == CS_ProtocolChecker::getServiceType() )
   {
      createDirectory(dataPath); // Data disk can only be seen on Master node
   }
   createDirectory(systemPath);
   createDirectory(logPath);

   logFileName = logPath + '\\';
   logFileName += LOG_FILE_NAME;

}


ACS_CS_Logger::~ACS_CS_Logger()
{
}



 void ACS_CS_Logger::log (std::string message, ACS_CS_Logger::LogType logType)
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   time_t longTime;
   time(&longTime);  // Get the current time
   struct tm* today = localtime(&longTime); // Convert to local time

   // Retrieve time information
   char timeInfo[MAX_LENGTH] = {0};
   strftime(timeInfo, MAX_LENGTH - 1, "%Y-%m-%d %H:%M:%S ", today);

   // Construct output as (time info + process info + log type + message)
   ostringstream output;
   output << timeInfo;
   output << "acs_csd:" << _getpid() << '\t';

   // Append log type
   switch (logType)
   {
      case Error:
         output << "Error:\t";
         break;

      case Warning:
         output << "Warning:\t";
         break;

      case Info:
      default:
         output << "Info:\t";
         break;
   }

   output << message.c_str();

   printToFile(output.str());

}

 std::string ACS_CS_Logger::getDataDir ()
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   (void) ACS_CS_Logger::createInstance();

   if (_instance)
   {
      return _instance->dataPath;          // return string with AP common path
   }
   else
   {
      return "";
   }

}

 std::string ACS_CS_Logger::getSystemDir ()
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   (void) ACS_CS_Logger::createInstance();

   if (_instance)
   {
      return _instance->systemPath;        // return string with node common path
   }
   else
   {
      return "";
   }

}

ACS_CS_Logger* ACS_CS_Logger::createInstance ()
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));  

   instanceLock.start_writing();
   if ( ! _instance )
   {
      _instance = new ACS_CS_Logger();
   }
   instanceLock.stop_writing();

   return _instance;

}

 void ACS_CS_Logger::createDirectory (std::string pathName)
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   WIN32_FIND_DATA dirInfo;
   HANDLE dirHandle = INVALID_HANDLE_VALUE;

   // Check if directory for data common to both nodes exist
   dirHandle = FindFirstFile(pathName.c_str(), &dirInfo);

   // Create missing directory
   if (dirHandle == INVALID_HANDLE_VALUE)
   {
      if ( ! CreateDirectory(pathName.c_str(), 0) )
      {
         DWORD lastError = GetLastError();
         ACS_CS_TRACE((ACS_CS_Logger_TRACE,
            "(%t) %s():\n  Error: Cannot create directory: %s (Error code: %d)",
            __FUNCTION__, pathName.c_str(), lastError));
      }
   }

   (void) FindClose(dirHandle);

}

 void ACS_CS_Logger::printToFile (std::string message)
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   logFileLock.start_writing();
   
   (void) switchFileOnSize();
   
   if ( ! logFile.is_open() )
   {
      // Open file (or create if necessary) for appending
      logFile.open(logFileName.c_str(), ios_base::app | ios_base::out);
   }

   if ( logFile.is_open() )
   {
      logFile << message << endl;     // Write message to file
   }
   else
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_Logger_TRACE,
         "(%t) %s():\n  Error: Cannot open file: %s (Error code: %d)",
         __FUNCTION__, logFileName.c_str(), lastError));
      
      logFile.clear(); // Clear failbit to reset fstream into a good state
   }

   logFileLock.stop_writing();

}

 bool ACS_CS_Logger::readRegistry ()
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   HKEY hKey;
   const DWORD MAX_VALUE_NAME = 16383;
   const char keyName[] = "SOFTWARE\\Ericsson\\Adjunct Processor\\ACS\\common";	// Registry key

   // Open key where path names for logs and data are stored
   if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
      TEXT(keyName),
      0,
      KEY_READ,
      &hKey) != ERROR_SUCCESS )
   {
      ACS_CS_TRACE((ACS_CS_Logger_TRACE,
         "(%t) %s():\n  Error: Cannot open registry key: %s",
         __FUNCTION__, keyName));

      string message = "Cannot open registry key ";
      message.append(keyName);

      ACS_CS_EVENT(Event_SubSystemError,
         "Open Registry Key",
         message.c_str(),
         "");

      return false;
   }

   DWORD noValues;                      // Number of values for key  
   DWORD retCode;                       // Return value
   char nameBuffer[MAX_VALUE_NAME];     // Buffer to hold value name
   DWORD bufferLength = MAX_VALUE_NAME; // Length of buffer

   // Get the value count. 
   retCode = RegQueryInfoKey(
      hKey,       // key handle 
      0,          // buffer for class name 
      0,          // size of class string 
      0,          // reserved 
      0,          // number of subkeys 
      0,          // longest subkey size 
      0,          // longest class string 
      &noValues,  // number of values for this key 
      0,          // longest value name 
      0,          // longest value data 
      0,          // security descriptor 
      0);         // last write time 
   // Enumerate the key values. 

   if (noValues)  // If there are values below the key
   {
      // Loop through all values
      for (unsigned int i = 0; i < noValues; i++) 
      { 
         bufferLength = MAX_VALUE_NAME; 
         nameBuffer[0] = '\0'; 

         // Get name of value
         retCode = ERROR_SUCCESS;
         retCode = RegEnumValueA(hKey, i, nameBuffer, &bufferLength, 0, 0, 0, 0);

         if (retCode == ERROR_SUCCESS ) // If successful
         { 
            DWORD bufferSize = 8192;
            char perfData[8192];

            // Get the actual value
            retCode = RegQueryValueExA(hKey, nameBuffer, 0, 0, (LPBYTE) perfData, &bufferSize);

            if (retCode == ERROR_SUCCESS ) // If successful
            {
               if (bufferSize > 0)      // If there are a value
               {
                  string valueName = nameBuffer;                  // String for the value name
                  char * valueString = new char[bufferSize + 1];  // Buffer for the value
                  memcpy(valueString, perfData, bufferSize);      // Copy value
                  valueString[bufferSize] = 0;                    // Null terminate string

                  if (valueName == "acsapdata")      // AP common data
                  {
                     if (dataPath.size() == 0)
                     {
                        dataPath = valueString;
                        dataPath.append("\\CS");
                     }
                  }
                  else if (valueName == "acsnsdata") // Node specific data
                  {
                     systemPath = valueString;
                     systemPath.append("\\CS");
                  }
                  else if (valueName == "acsnslogs") // Node specific logs
                  {
                     logPath = valueString;
                     logPath.append("\\CS");
                  }

                  delete [] valueString;
               }
            }
         } 
      }
   }
   else
      return false;

   return true;

}

 bool ACS_CS_Logger::getPath ()
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   char dataName[] = "ACS_DATA$";  // Logical name for data disk
   DWORD bytesRead = MAX_LENGTH;   // Bytes returned
   char buffer[MAX_LENGTH];        // Buffer to hold path

   // Get path to data disk
   int returnValue = GetDataDiskPath(dataName, buffer, &bytesRead);
   if ( 0 == returnValue && bytesRead < MAX_LENGTH )
   {
      buffer[bytesRead] = 0;
      dataPath = buffer;
      dataPath.append("\\CS");
   }
   else
   {
      return false;
   }

   return true;

}

 bool ACS_CS_Logger::switchFileOnSize ()
{

   ACS_CS_TRACE((ACS_CS_Logger_TRACE,
      "(%t) %s():\n  Entering method",
      __FUNCTION__));

   int numOfBackupFiles;

   // Make sure there is at least one backup file
   if ( MAX_NUM_OF_BACKUP_FILES < 1 )
   {
      numOfBackupFiles = 1;
   }
   else
   {
      numOfBackupFiles = MAX_NUM_OF_BACKUP_FILES;
   }

   struct stat info;
   if ( stat(logFileName.c_str(), &info) < 0 )
   {
      ACS_CS_TRACE((ACS_CS_Logger_TRACE,
         "(%t) %s():\n  Warning: %s doesn't exist",
         __FUNCTION__, logFileName.c_str()));

      return false;
   }

   // Check if the log file size exceeds the limit
   if ( info.st_size < MAX_LOG_FILE_SIZE )
   {
      return true;
   }

   // It's time to switch all backup files: (e.g., numOfBackupFiles = 99)
   // 1. delete the oldest file logFileName.99;
   // 2. switch others, i.e., logFileName.98 -> logFileName.99, ..., logFileName.01 -> logFileName.02
   // 3. move the current log file to logFileName.01

   string backupFileName;
   string::size_type pos;

   pos = logFileName.find_last_of(".");
   if ( string::npos != pos )
   {
      backupFileName = logFileName.substr(0, pos+1);
   }
   else
   {
      backupFileName = logFileName + '.';
   }

   // Calculate the length of extension names (i.e., the number of characters)
   int extLen = 0;
   int tmp = numOfBackupFiles;
   while ( tmp > 0 )
   {
      extLen++;
      tmp /= 10;
   }

   // Delete the oldest backup file
   ostringstream extName;
   extName << std::setw(extLen) << std::setfill('0') << numOfBackupFiles;
   string oldestFileName = backupFileName + extName.str();

   BOOL deleted = DeleteFile(oldestFileName.c_str());
   if ( ! deleted )
   {
      DWORD lastError = GetLastError();
      if ( ERROR_FILE_NOT_FOUND != lastError ) // It is OK if the file doesn't exist
      {
         ACS_CS_TRACE((ACS_CS_Logger_TRACE,
            "(%t) %s():\n  Error: Cannot delete %s (Error code: %d)",
            __FUNCTION__, oldestFileName.c_str(), lastError));
      }
   }

   // Switch the remaining backup files
   string currFileName;
   string nextFileName;
   BOOL moved;

   for (int cnt = numOfBackupFiles - 1; cnt > 0; cnt--)
   {
      ostringstream currExtName;
      currExtName << std::setw(extLen) << std::setfill('0') << cnt;
      currFileName = backupFileName + currExtName.str();

      ostringstream nextExtName;
      nextExtName << std::setw(extLen) << std::setfill('0') << (cnt + 1);
      nextFileName = backupFileName + nextExtName.str();

      moved = MoveFileEx(currFileName.c_str(), nextFileName.c_str(), MOVEFILE_REPLACE_EXISTING);
      if ( ! moved )
      {
         DWORD lastError = GetLastError();
         if ( ERROR_FILE_NOT_FOUND != lastError ) // It is OK if the file(s) doesn't exist
         {
            ACS_CS_TRACE((ACS_CS_Logger_TRACE,
               "(%t) %s():\n  Error: Cannot move %s to %s (Error code: %d)",
               __FUNCTION__, currFileName.c_str(), nextFileName.c_str(), lastError));
         }
      }
   }

   // Close the current log file for moving
   if ( logFile.is_open() )
   {
      logFile.close();
   }

   // Move the current log file to the first backup file
   ostringstream nextExtName;
   nextExtName << std::setw(extLen) << std::setfill('0') << (cnt + 1);
   nextFileName = backupFileName + nextExtName.str();

   moved = MoveFileEx(logFileName.c_str(), nextFileName.c_str(), MOVEFILE_REPLACE_EXISTING);
   if ( ! moved )
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_Logger_TRACE,
         "(%t) %s():\n  Error: Cannot move %s to %s (Error code: %d)",
         __FUNCTION__, logFileName.c_str(), nextFileName.c_str(), lastError));
   }

   return true;

}

// Additional Declarations

