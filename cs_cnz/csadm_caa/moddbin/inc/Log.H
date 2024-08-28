//******************************************************************************
// 
// .NAME
//    Log - Provides an interface to log debug information to screen or a TCP
//          connection.
// .LIBRARY 3C++
// .PAGENAME LOG
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
//  Log may be used to generate debug printouts in runtime directed either to
//  screen (console) or to a TCP connection. When a TCP connection is
//  established, all log information is sent using the connection. In any
//  other case, the log data is printed directly to the screen. Log will not
//  perform any action (printing to screen or accepting connections) unless
//  _DEBUG or RUNTIME_TRACE is defined when compiling.
//  Use of Log is thread safe.
//
//  Usage:
//
//  int main()
//  {
//     Log::listenPort(8001);
//
//     DBGLOG("The same input format as accepeted by printf may be used\n");
//
//     return 0;
//  }
//
//  This would print the debug string along with time, file and line
//  information to the screen if no connection with port 8001 is established.
//  The output could for instance look like this:
//  14:39:17 CPTASP.cpp@387: Sent CPTWRITEBUFFR(SRC ID=0x100002)

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-18 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef LOG_H
#define LOG_H

#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif // _MSC_VER

#define DBGLOG (Log(__FILE__, __LINE__))

class Log
{
public:
   // The constructor records the file and line location where the instance
   // was created.
   Log(const char* file, int line) :
      m_Line(line),
      m_File(fileWithoutPath(file))
   {
   }

   // Sets the port number to use when listening for TCP connections that are
   // willing to handle console output rather than printing them to the screen
   // where the current process is running.
   static void listenPort(unsigned short int port);

   // This operator parses input of the same format as would be accepted by
   // printf and passes it along either to the console or sends it using
   // a TCP connection accepted on the port set by a call to the method
   // listenPort(...).
   void operator()(const char* format, ...) const;

   // Strips the input file from its path (if any) and returns a pointer to a
   // character array that consist only of the filename. Note that the
   // returned pointer points to the memory location allocated by file.
   static const char* fileWithoutPath(const char* file);

private:

   // The line at which the Log instance was created.
   const int m_Line;

   // The file in which the Log instance was created.
   const char* m_File;

   // True when the listening port is set up correctly.
   static bool m_isServerSetup;

   // The port number to use when listening for connections that would want to
   // receive console printout.
   static unsigned int short s_listenPort;
};

#endif
