//******************************************************************************
// 
// .NAME
//    TCPErrorReceiver - Provides an interface that receives error information
//    from instances of TCPClientServer and logs the error information in the event
//    log.
//
// .LIBRARY 3C++
// .PAGENAME TCPErrorReceiver
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
//  TCPErrorReceiver receives error information from the error interpretation
//  functions in TCPClientServer and passes it along to the EventReporter.
//  Use of the TCPErrorReceiver is thread safe.
//
//  Usage:
//  TCPClientServer::errorReceivingFunction(TCPErrorReceivingFunction);

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-09-20 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif // _MSC_VER

#include "TCPClientServer.hxx"

namespace TCPErrorReceiver
{

// This method receives error information from instances of TCPClientServer
// and logs the information in the event log using the EventReporter.
void TCPErrorReceivingFunction(TCPClientServer::ErrorType theErrorType,
                               const char* filename,
                               int line,
                               const char* description,
                               u_int16 data1,
                               u_int16 data2,
                               u_int16 data3,
                               u_int16 data4);
};
