//******************************************************************************
//
// NAME
//      Log
//
//  COPYRIGHT Ericsson AB, Sweden 2005.
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
//  See Log.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-18 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .SEE ALSO 
//  N/A.
//
//******************************************************************************
#include "Log.H"
#include "MultiPlatformMutex.hxx"
#include "TCPServer.hxx"
#include "TCPClient.hxx"
#include "TCPErrorReceiver.H"
#include <iostream>
#include <time.h>

using namespace std;

// Remove the comment from the follwing line to enable internal debug
// printouts on release builds.
// #define RUNTIME_TRACE 1

namespace
{
   MultiPlatformMutex MAS_Common_logMutex;
   TCPServer MAS_Common_logServer(TCPClientServer::NonBlocking, TCPErrorReceiver::TCPErrorReceivingFunction);
   TCPClient MAS_Common_logConnection(TCPClientServer::NonBlocking, TCPErrorReceiver::TCPErrorReceivingFunction);
};

bool Log::m_isServerSetup = false;
unsigned int short Log::s_listenPort = 0;

//******************************************************************************
// Desription:
//    This method interprets the input and print the output to screen unless a
//    TCP connection is accepted on the port supplied in a previous call to the
//    method listenPort. In this case, all debug output is sent to an external
//    process instead.
// Parameters:
//    All input data (format and ...) is of the same syntax as would be
//    accepted by for instance printf.
//******************************************************************************
void Log::operator()(const char* format, ...) const
{
#if defined _DEBUG || defined RUNTIME_TRACE
   // Make sure all access is thread safe.
   SafeMutex mutex(MAS_Common_logMutex);

   const int MaxLength = 2000;
   char temp[MaxLength];
   va_list arguments;

   if(!m_isServerSetup)
   {
      m_isServerSetup = MAS_Common_logServer.listenOn(s_listenPort, 0) == TCPServer::OK ? true : false;
   }

   if(!MAS_Common_logConnection.isConnected())
   {
      if(MAS_Common_logServer.accept() == Media::Connected)
      {
         MAS_Common_logConnection.useThisSocket(MAS_Common_logServer.connectedSocket());
      }
   }

   va_start(arguments, format);
   unsigned int logLength = vsnprintf(temp, MaxLength, format, arguments);
   va_end(arguments);

   char timeFileAndLine[256];
   static_cast<void>(_strtime(timeFileAndLine)); // 8 byte long
   unsigned int length = sprintf(timeFileAndLine + 8, " %d:%s@%d: ", GetCurrentThreadId(), m_File, m_Line) + 8;

   if(!MAS_Common_logConnection.isConnected())
   {
      cout << timeFileAndLine << temp << flush;
   }
   else
   {
      if(MAS_Common_logConnection.send(timeFileAndLine, length) == TCPClient::Disconnected ||
         MAS_Common_logConnection.send(temp, logLength) == TCPClient::Disconnected)
      {
         MAS_Common_logConnection.disconnect();
      }
   }
#endif // _DEBUG || RUNTIME_TRACE
}

//******************************************************************************
// <memberFunction()>
//******************************************************************************
void Log::listenPort(unsigned short int port)
{
   assert((s_listenPort == 0 || port == s_listenPort) && "Do not change the listening port once set");

   s_listenPort = port;
}


//******************************************************************************
// Description:
//    Strips the input file from its path (if any) and returns a pointer to a
//    character array that consist only of the filename. Note that the
//    returned pointer points to the memory location allocated by file.
// Paramters:
//    file  The filename including that path that should be removed.
// Returns:
//    The input filename without the path.
//******************************************************************************
const char* Log::fileWithoutPath(const char* file)
{
   if(file == 0)
   {
      return "Unknown";
   }

   const char* fileNameStart = strrchr(file, '\\');

   if(fileNameStart != NULL && (fileNameStart[1] != '\0'))
   {
      return fileNameStart + 1;
   }
   else
   {
      return file;
   }
}
