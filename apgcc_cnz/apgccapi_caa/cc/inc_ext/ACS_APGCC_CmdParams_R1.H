//*****************************************************************************
//
// .NAME
//     ACS_APGCC_CmdParams_R1 - Internal Class for passing command parameters.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_CmdParams_R1.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2001.
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
//  Class that simplifies the sending and receival of command,
//  or other, parameters between a sender and a receiver.
//
//  It does not make use of any UNIX signals; nor will it throw any
//  exceptions.
//
//  Over- and underflow will not be notified.

// .ERROR HANDLING
//
//  .fill_off
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.
//  .fill_on

// AUTHOR
//      2001-04-18 by UAB/S/AU  QABSJEN

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME          DESCRIPTION
//      A               01-04-18        SJEN          First release
//
//*****************************************************************************
/*
 * ACS_APGCC_CmdParams_R1.H
 *
 *  Created on: Apr 12, 2010
 *      Author: root
 */

#ifndef ACS_APGCC_CMDPARAMS_R1_H_
#define ACS_APGCC_CMDPARAMS_R1_H_

#pragma once

#ifndef INCLUDED_STRING
#  include <string>
#  define INCLUDED_STRING
#endif

#ifndef INCLUDED_VECTOR
#  include <vector>
#  define INCLUDED_VECTOR
#endif

#ifndef APGCC_OS_H
#  include "ACS_APGCC_OS.H"            // For MCC_Export
#endif

#ifndef APGCC_COMMAND_H
#  include "ACS_APGCC_Command.H"       // For APGCC_Array
#endif

#ifndef APGCC_COLLECTIONS_H
#  include "ACS_APGCC_Collections.H"   // For APGCC_IPersistent & APGCC_IStream
#endif
#include "ACS_APGCC_Common_Define.h"


// Forward declarations



//==================================================
// Generic class for handling of command parameters.
// Preferable used with the template APGCC_Cmd_Client.
//==================================================

class ACS_APGCC_CmdParams_R1: public ACS_APGCC_IPersistent
{
public:
   ACS_APGCC_CmdParams_R1();
   // Constructor

   virtual ~ACS_APGCC_CmdParams_R1();
   // Destructor

   int cmdCode;
   // Description:
   //    Command code.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    Public data member.


   int result;
   // Description:
   //    Result code.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    Public data member.


   ACS_APGCC_Array data;
   // Description:
   //    Data parameters.
   //    The maximum number of data parameters is unlimited
   //    as long as the total size of parameters is less than 60K characters.
   //
   //    The data parameter values are set/retrieved
   //    in vector style, e.g
   //
   //       ACS_APGCC_CmdParams_R1 cmd;
   //       cmd.data[0] = "Hello";
   //       cmd.data[1] = 1234;
   //
   //       std::string str = cmd.data[0];
   //       int num = cmd.data[1];
   //
   //    Method numOfData() returns number of stored
   //    data parameters including null values.
   //    E.g.
   //       ACS_APGCC_CmdParams_R1 cmd;
   //       cmd.data[99] = "Item 100";
   //       cout << "Items " << cmd.numOfData() << endl;
   //
   //    Returns:
   //       Items 100
   //
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    Public data member.


   void clear();
   // Description:
   //    Clear all data parameters including result code
   //    but not cmdCode.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    -


   int numOfData();
   // Description:
   //    Returns number of data parameters
   // Parameters:
   //    -
   // Return value:
   //    integer     Number of data parameters
   // Additional information:
   //    -


   //============================================================
   // The following methos are used when sending and
   // receiving a command object. Before sending an object
   // it must be encoded(), and after an object has been
   // received it must be decoded to form a valid command object.
   //============================================================

   virtual bool Load(ACS_APGCC_IStream* rd);
   // Description:
   //    Read (construct) a command object from the given Stream.
   // Parameters:
   //    rd          A APGCC_IStream object.
   // Return value:
   //    bool        Result code
   //                   true  - Success
   //                   false - Failure
   // Additional information:
   //    Example:
   //    ACS_APGCC_CmdParams_R1 cmd;
   //    cmd.cmdCode = 17;
   //    cmd.data[0] = "First item";
   //    cmd.data[1] = 1;
   //    cmd.data[4] = "Last item";
   //
   //    APGCC_FileStream outFile ("test.tmp");
   //    cmd.writeTo(outFile);
   //
   //    ACS_APGCC_CmdParams_R1 test;
   //    APGCC_FileStream inFile ("test.tmp");
   //    test.readFrom(inFile);


   virtual bool Save(ACS_APGCC_IStream* wr);
   // Description:
   //    Write a command object to the given Stream.
   // Parameters:
   //    rd          A APGCC_StreamWriter object.
   // Return value:
   //    bool        Result code
   //                   true  - Success
   //                   false - Failure
   // Additional information:
   //    See above example.


   // Methods mostly used by internal functions
   //==========================================

   void encode();
   // Description:
   //    Encode command object.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    -


   void decode();
   // Description:
   //    Decode the given buffer and construct
   //    this command object.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    -


   const char* getEncodedBuf() const;
   // Description:
   //    Returns encode buffer.
   // Parameters:
   //    buf         Out buffer.
   // Return value:
   //    -
   // Additional information:
   //    -


   int getEncodedSize() const;
   // Description:
   //    Returns size of encode buffer.
   // Parameters:
   //    -
   // Return value:
   //    Size of encoded buffer.
   // Additional information:
   //    -


   ACS_APGCC_CmdParams_R1(const ACS_APGCC_CmdParams_R1& cmd);
   // Description:
   //    Copy constructor.
   // Parameters:
   //    cmd         Command object to copy from.
   // Return value:
   //    -
   // Additional information:
   //    -


   ACS_APGCC_CmdParams_R1& operator=(const ACS_APGCC_CmdParams_R1& cmd);
   // Description:
   //    Assignment operator.
   // Parameters:
   //    cmd         Command object to copy from.
   // Return value:
   //    This object.
   // Additional information:
   //    -

   void dump(const char* s = "Dump:", bool dump_encodedbuf=false);
   // Description:
   //    Dumps data on screen.


protected:

   // Protected (internal) members

   int encodedSize_;
   // Indicates size of encoded data

   unsigned char* encodedBuf_;
   // Holds encoded data
};


#endif /* ACS_APGCC_CMDPARAMS_R1_H_ */
