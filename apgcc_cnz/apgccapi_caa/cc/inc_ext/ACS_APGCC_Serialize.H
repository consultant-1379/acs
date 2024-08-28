//*****************************************************************************
//
// .NAME
//     ACS_APGCC_Serialize - Serializer base class.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_Serialize.H

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
//
//  Base class used to implement serializers; such as readers
//  and writers.
//  Two default serializers exists; SocketStream and FileStream.
//
//  Step-by-step guide:
//
//		1, Build a reader stream by inheriting from MCS_StreamReader
//		   and implement all methods.
//
//		2, Build a writer stream by inheriting from MCS_StreamWriter
//		   and implement all methods.
//
//		3, The class that should be serializable must inherit
//		   from MCS_Serializable and implement the readFrom() and writeTo()
//		   methods. The implementation should call the appropiate methods
//		   in the supplied stream class to "serialize the class" over the stream.
//
//      For example on how to implement streamer objects see MCS_SockStream
//		and MCS_FileStream. For example on how to implement a serializable
//		class see MCS_Command.
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
//      1998-12-02 by UAB/I/LN  UABSJEN

// .REVISION
//      A       98-12-02

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//      A               00-01-25        SJEN            First release

// .LINKAGE
//      MCS_MCC
//*****************************************************************************
#ifndef ACS_APGCC_SERIALIZE_H
#define ACS_APGCC_SERIALIZE_H
#  include "ACS_APGCC_OS.H"


class ACS_APGCC_StreamReader;
class ACS_APGCC_StreamWriter;

class  ACS_APGCC_Serializable
{
public:
	virtual int readFrom(ACS_APGCC_StreamReader* rd) = 0;
	virtual int writeTo(ACS_APGCC_StreamWriter* wr) = 0;
};


class ACS_APGCC_StreamWriter
{
public:
	// write the whole object
	virtual int writeObject(ACS_APGCC_Serializable& ss) = 0;

	// primitive "built-in" value types
	virtual int writeChar(const char ch) = 0;
	virtual int writeChar(const unsigned char ch) = 0;
	virtual int writeInt(const int n) = 0;
	virtual int writeByte(const int byte) = 0;
	virtual int writeWord(const int word) = 0;
	virtual int writeString(const char* s) = 0;
	virtual int writeBuffer(const void* buf, size_t len) = 0;
};


class ACS_APGCC_StreamReader
{
public:
	// read the whole object
	virtual int readObject(ACS_APGCC_Serializable& ss) = 0;

	// primitive "built-in" value types
	virtual int readChar(char& ch) = 0;
	virtual int readChar(unsigned char& ch) = 0;
	virtual int readInt(int& n) = 0;
	virtual int readByte(int& byte) = 0;
	virtual int readWord(int& word) = 0;
	virtual int readString(char* s, const size_t maxLen) = 0;
	virtual int readBuffer(void* buf, size_t len) = 0;

	virtual int readBuffer(void* buf) = 0;
   // Returns size of allocated buffer, or -1 if failed.
   // Caller is responsible for deallocating the returned buffer.
};


#endif
