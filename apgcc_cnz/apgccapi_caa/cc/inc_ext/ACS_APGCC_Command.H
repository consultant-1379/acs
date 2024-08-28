//*****************************************************************************
//
// .NAME
//     MCS_Command - Internal Class for passing parameters.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_Command.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1998.
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

// DOCUMENT NO
//      190 89-CAA 109 0171

// AUTHOR
//      1998-03-18 by UAB/I/LN  UABSJEN

// .REVISION
//      B      1999-03-03


// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME          DESCRIPTION
//      A               98-03-18        SJEN          First release
//      B               99-03-03        SJEN          Second release
//                                                    Removed Array template.
//      C             2001-02-01        AKLU          Removed protected "char traStr[100];"
//                                                    from the MCS_Command class
//                                                    This string was inserted by misstake
//                                                    and caused a change to the interface.
//

// .LINKAGE
//      -lMCC

//*****************************************************************************
/*
 * ACS_APGCC_Command.H
 *
 *  Created on: Apr 2, 2010
 *      Author: root
 */

#ifndef ACS_APGCC_COMMAND_H_
#define ACS_APGCC_COMMAND_H_
#  include <string>
#ifndef INCLUDED_VECTOR
#  include <vector>
#  define INCLUDED_VECTOR
#endif

#ifndef ACE_OS_H
//#  include <osf/OS.h>
#include "ace/OS.h"
#endif


#  include "ACS_APGCC_OS.H"
#  include "ACS_APGCC_DSD.H"

#  include "ACS_APGCC_Streams.H"

#ifdef _MCCUSESERIALIZE
#    include "ACS_APGCC_Serialize.H"    // For backwards compability only
#else
#    include "ACS_APGCC_Collections.H"
#endif



// Forward declarations

class APGCC_DSD_Stream;

//===========================================================
// Support class for the generic Command handling class below.
//===========================================================

class  ACS_APGCC_Array
{
public:

   //typedef std::string DataType;  // If use std strings
   typedef char* DataType;

   class  Data
   {
   public:
      Data();
      Data(const Data& d);
      ~Data();
      Data& operator=(int n);
      //Data& operator=(long n);
      Data& operator=(const std::string& s);
      Data& operator=(const Data& d);

      const char* c_str();
      operator int();
      /*operator long();*/
      operator const char*();

   protected:
      DataType s_;

      void init(const char* s);     // Comment out this if using std strings
   };


   typedef std::vector<Data*> DataList;
   typedef DataList::iterator iterator;
   typedef DataList::const_iterator const_iterator;

   ACS_APGCC_Array();
   ACS_APGCC_Array(const ACS_APGCC_Array& array);
   ~ACS_APGCC_Array();
   int size();
   void clear();
   const Data& operator[](int i) const;
   Data& operator[](int i);



   ACS_APGCC_Array& operator=(const ACS_APGCC_Array& array);

protected:
   DataList* list_;
};




//==================================================
// Generic class for Command handling
// Preferable used with the template ACS_APGCC_Cmd_Client.
//==================================================

#ifdef _MCCUSESERIALIZE
class ACS_APGCC_Command: public ACS_APGCC_Serializable
#else
class ACS_APGCC_Command: public ACS_APGCC_IPersistent
#endif
{
public:
   ACS_APGCC_Command();
   ~ACS_APGCC_Command();


   int cmdCode;
   // Description:
   //    Command code between 0-254.
   //    The value 255 is RESERVED.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    Public data member.


   int result;
   // Description:
   //    Result code between +- 32767.
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
   //       ACS_APGCC_Command cmd;
   //       cmd.data[0] = "Hello";
   //       cmd.data[1] = 1234;
   //
   //       std::string str = cmd.data[0];
   //       int num = cmd.data[1];
   //
   //    Method numOfData() returns number of stored
   //    data parameters including null values.
   //    E.g.
   //       ACS_APGCC_Command cmd;
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


   int  numOfData();
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
#ifndef _MCCUSESERIALIZE

   virtual bool Load(ACS_APGCC_IStream* rd);
   // Description:
   //    Read (construct) a command object from given Stream.
   // Parameters:
   //    rd       A ACS_APGCC_IStream object.
   // Return value:
   //    bool        Result code
   //                   true  - Success
   //                   false - Failure
   // Additional information:
   //    Example:
   //    ACS_APGCC_Command cmd;
   //    cmd.cmdCode = 17;
   //    cmd.data[0] = "First item";
   //    cmd.data[1] = 1;
   //    cmd.data[4] = "Last item";
   //
   //    ACS_APGCC_FileStream outFile ("test.tmp");
   //    cmd.writeTo(outFile);
   //
   //    ACS_APGCC_Command test;
   //    MCS_FileStream inFile ("test.tmp");
   //    test.readFrom(inFile);


   virtual bool Save(ACS_APGCC_IStream* wr);
   // Description:
   //    Write a command object to the given Stream.
   // Parameters:
   //    rd          A MCS_StreamWriter object.
   // Return value:
   //    bool        Result code
   //                   true  - Success
   //                   false - Failure
   // Additional information:
   //    See above example.

#endif
#ifdef _MCCUSESERIALIZE
   virtual int readFrom(ACS_APGCC_StreamReader* rd);
   // Description:
   //    Read (construct) a command object from given Stream.
   // Parameters:
   //    rd       A ACS_APGCC_StreamReader object.
   // Return value:
   //    integer     Result code
   //                   0 - Success
   //                  !0 - Failure
   // Additional information:
   //    Example:
   //    ACS_APGCC_Command cmd;
   //    cmd.cmdCode = 17;
   //    cmd.data[0] = "First item";
   //    cmd.data[1] = 1;
   //    cmd.data[4] = "Last item";
   //
   //    ACS_APGCC_FileWriter outFile ("test.tmp");
   //    cmd.writeTo(outFile);
   //
   //    ACS_APGCC_Command test;
   //    ACS_APGCC_FileReader inFile ("test.tmp");
   //    test.readFrom(inFile);


   virtual int writeTo(ACS_APGCC_StreamWriter* wr);
   // Description:
   //    Write a command object to the given Stream.
   // Parameters:
   //    rd          A ACS_APGCC_StreamWriter object.
   // Return value:
   //    integer     Result code
   //                   0 - Success
   //                  !0 - Failure
   // Additional infStreamsormation:
   //    See above example.
#endif //_MCCUSESERIALIZE

   // Normal users does not need to use the following routines.
   // routines to simplify sending and receiving of a cmd object
   // using a MCS DSD Stream.

   int  send(ACS_APGCC_DSD_Stream& stream);
   int  recv(ACS_APGCC_DSD_Stream& stream);


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


   void decode(const unsigned char buf[], bool headerIncluded=false);
   // Description:
   //    Decode the given buffer and construct
   //    this command object.
   // Parameters:
   //    buf               Data buffer to decode.
   //    headerIncluded    If true, the data buffer starts with the
   //                      4 bytes header, which indicates the size of
   //                      the data.
   // Return value:
   //    -
   // Additional information:
   //    -


   void getEncodedBuf(unsigned char buf[]);
   // Description:
   //    Returns encode buffer.
   // Parameters:
   //    buf         Out buffer.
   // Return value:
   //    -
   // Additional information:
   //    -


   int  getEncodedSize();
   // Description:
   //    Returns size of encode buffer.
   // Parameters:
   //    -
   // Return value:
   //    Size of encoded buffer.
   // Additional information:
   //    -


   ACS_APGCC_Command(const ACS_APGCC_Command& cmd);
   // Description:
   //    Copy constructor.
   // Parameters:
   //    cmd         Command object to copy from.
   // Return value:
   //    -
   // Additional information:
   //    -


   ACS_APGCC_Command& operator=(const ACS_APGCC_Command& cmd);
   // Description:
   //    Assignment operator.
   // Parameters:
   //    cmd         Command object to copy from.
   // Return value:
   //    This object.
   // Additional information:
   //    -

   void dump(const char* s = "Dump:");

protected:
   int            encodedSize;      // indicates size of encoded data
   unsigned char* encodedBuf;       // contains encoded data
};


#endif /* APGCC_COMMAND_H_ */
