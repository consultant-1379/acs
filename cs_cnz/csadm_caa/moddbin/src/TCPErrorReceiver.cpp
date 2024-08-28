//******************************************************************************
//
// NAME
//      TCPErrorReceiver
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
//  See TCPErrorReceiver.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-09-20 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .SEE ALSO 
//  N/A.
//
//******************************************************************************
#include "TCPErrorReceiver.H"
#include "MultiPlatformMutex.hxx"
#include "EventReporter.H"

namespace TCPErrorReceiver
{

MultiPlatformMutex threadSafeAccessMutex;

//******************************************************************************
// Desription:
//    This method receives error information from instances of TCPClientServer
//    and logs the information in the event log using the EventReporter.
// Parameters:
//    theErrorType  The error category.
//    filename      The source file that reports the problem.
//    line          The line number in "filename" that reports the problem.
//    description   Readable error information.
//    data1         Data specific to the reported error.
//    data2         Data specific to the reported error.
//    data3         Data specific to the reported error.
//    data4         Data specific to the reported error.
//******************************************************************************
void TCPErrorReceivingFunction(TCPClientServer::ErrorType theErrorType,
                               const char* filename,
                               int line,
                               const char* description,
                               u_int16 data1,
                               u_int16 data2,
                               u_int16 data3,
                               u_int16 data4)
{
   // Ensure thread safety.
   SafeMutex mutex(threadSafeAccessMutex);

   // XXX Insert ACS trace here, telling about the error.
   EVENT(EventReporter::SocketAPI,
         PROBLEM_DATA("%s@%d reports error type %d \"%s\" with additional data %d (H'%X), %d (H'%X), %d (H'%X), %d (H'%X)",
                      Log::fileWithoutPath(filename),
                      line,
                      theErrorType,
                      description,
                      data1, data1,
                      data2, data2,
                      data3, data3,
                      data4, data4),
         PROBLEM_TEXT(""));
}

};
