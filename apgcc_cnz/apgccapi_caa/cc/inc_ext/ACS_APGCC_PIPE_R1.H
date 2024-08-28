//*****************************************************************************
//
// .NAME
//     ACS_APGCC_PIPE_R1 - Classes for named pipes.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  ACS_APGCC
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_PIPE_R1.H

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
//  Class that simplifies the usage of named pipes.
//
//  It does not make use of any signals; nor will it throw any
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
//      B               01-08-13        SJEN          Added trace points.
//
//***                         Porting                      ***
//       REV NO          DATE            NAME    DESCRIPTION
//       PR_01           10-04-15        AI      First version
//*****************************************************************************

#ifndef ACS_APGCC_PIPE_R1_H
#define ACS_APGCC_PIPE_R1_H
#ifndef INCLUDED_STRING
#  include <string>

#  define INCLUDED_STRING
#endif
#  include "ACS_APGCC_OS.H"            // For MCC_Export
#ifndef ACE_SYNCH_H
#  include <ace/Synch.h>
#endif

#  include "ACS_APGCC_Collections.H"   // For ACS_APGCC_IStream


#include "ace/SPIPE_Addr.h"
#include "ace/SPIPE_Acceptor.h"
#include "ace/SPIPE_Stream.h"
#include "ace/SPIPE_Connector.h"
#include "ACS_APGCC_Common_Define.h"
#include "ace/ACE.h"

// Forward declarations
class ACS_APGCC_PIPE_Acceptor_R1;
class ACS_APGCC_PIPE_Connector_R1;
typedef  int HANDLE;
typedef  bool BOOL;
typedef  int OVERLAPPED;
/*PR_01 BEGIN
 stub Forward declarations */
class ACS_TRA_trace;
ACS_TRA_trace ACS_TRA_DEF(const char *,const char *);
int ACS_TRA_ON(ACS_TRA_trace);
int ACS_TRA_event(ACS_TRA_trace *,const char *);

/*PR_01 END*/


//========================================================
// Classes that implements support for named pipe streams.
//========================================================


//--------------------------------------------------
// MCS DSD Address
//
// Requires the address on the form "domain:appl"
//
//--------------------------------------------------
class  ACS_APGCC_PIPE_Addr_R1: public ACE_SPIPE_Addr
{
public:
	ACS_APGCC_PIPE_Addr_R1();
   // Default constructor

	ACS_APGCC_PIPE_Addr_R1(char  pipe_name);
   // Constructor with constant string

	ACS_APGCC_PIPE_Addr_R1(const std::string& pipe_name);
   // Construtor with std string

	ACS_APGCC_PIPE_Addr_R1(const ACS_APGCC_PIPE_Addr_R1& addr);
   // Copy construtor

   virtual ~ACS_APGCC_PIPE_Addr_R1();
   // Destructor

   ACS_APGCC_PIPE_Addr_R1& operator=(const ACS_APGCC_PIPE_Addr_R1& addr);
   // Assignment operator

   void set(const std::string& pipe_name);
   // Set path name

   void set(const ACS_APGCC_PIPE_Addr_R1& addr);
   // Set pipe address

   const char* get_pipe_path() const;
   // Returns complete pipe path.

   const char* get_domain() const;
   // Returns first part of pipe name.
   // Returns "ap99" if pipe name is "\\ap99\pipename.pipe".

protected:
   std::string pipe_name_;
   // Complete path name

   std::string domain_;
   // First part of path name (host)
};



//========================================================
// MCS_PIPE_Stream
// Implements a named pipe stream object.
//========================================================

class ACS_APGCC_PIPE_Stream_R1: public ACS_APGCC_IStream,ACE_SPIPE_Stream
{
public:

   friend class ACS_APGCC_PIPE_Acceptor_R1;
   friend class ACS_APGCC_PIPE_Connector_R1;


   ACS_APGCC_PIPE_Stream_R1();
   // Constructor

   virtual ~ ACS_APGCC_PIPE_Stream_R1();
   // Destructor

   // Methods to support IStream interface
   //=======================================

   virtual bool Read(void* buf, size_t len);
   // Description:
   //    Read data from pipe
   // Parameters:
   //    buf            Data buffer specified by the user.
   //    len            Number of bytes to retrieve.
   // Return value:
   //    true           Success
   //    false          Failure. Check GetLastError().
   // Additional information:
   //    -

   virtual bool Write(const void* buf, size_t len);
   // Description:
   //    Write data to pipe
   // Parameters:
   //    buf            Data buffer specified by the user.
   //    len            Number of bytes to send.
   // Return value:
   //    true           Success
   //    false          Failure. Check GetLastError().
   // Additional information:
   //    -

   // Methods to send and receive message std::strings
   //=============================================

   bool RecvMsg(std::string& out);
   // Receive message from pipe

   bool SendMsg(const std::string& in);
   // Send message to pipe

   int close();
   // Close pipe

   void set_handle(ACE_HANDLE h);
   // Sets the underlaying pipe handle

   HANDLE get_handle() const;
   // Return the underlaying pipe handle


protected:

   // Not implemented methods
   //-------------------------

   virtual long GetPos();
   // Get the current position in the stream

   virtual bool SetPos(long distance, size_t moveMethod);
   // Set a position within the stream

   virtual bool Empty() const;
   // Returns true if stream is empty

   virtual bool Lock();
   // Lock the access to the stream

   virtual bool Unlock();
   // Unlock the access to the stream


private:
   ACE_HANDLE hPipe_;
   // Pipe handle

   bool server_side_;
   // True if accepted by MCS_PIPE_Acceptor_R1

   char traStr_[200];
   // Trace string

private:
   // forbidden methods
   ACS_APGCC_PIPE_Stream_R1(const ACS_APGCC_PIPE_Stream_R1&);
   ACS_APGCC_PIPE_Stream_R1& operator=(const ACS_APGCC_PIPE_Stream_R1&);
};


//========================================================
// MCS_PIPE_Acceptor
// Implements a named pipe acceptor object.
// used to implement a named pipe server and wait for clients.
//========================================================

class ACS_APGCC_PIPE_Acceptor_R1
{
public:
	ACS_APGCC_PIPE_Acceptor_R1();
   // Constructor

   virtual ~ACS_APGCC_PIPE_Acceptor_R1();
   // Deconstructor

   //PR_01 BEGIN
 /*  int open(const ACS_APGCC_PIPE_Addr_R1& pipe_addr,int reuse_addr = 1,
           int perms = ACE_DEFAULT_FILE_PERMS,
           LPSECURITY_ATTRIBUTES sa = 0,
           int pipe_mode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE);*/
   // Description:
      //    Create listening point for named pipe clients
      // Parameters:
      //    pipe_addr      Name of pipe to listen on.
      //    secattr        Security attributes that are used
      //                   when creating the pipe.
      //                   Without a security attribute, the pipe gets a default
      //                   security setting which makes it unaccessable from
      //                   remote hosts. See also Additional Information below.
      // Return value:
      //    0              Success
      //    <0             Failure. Check GetLastError().
      // Additional information:
      //
      //
    // int accept(ACS_APGCC_PIPE_Stream_R1& client);
       // Description:
       //    Accept a new client that connects to this pipe
       // Parameters:
       //    client         Pipe stream object which is used to
       //                   communicate with the connected client.
       // Return value:
       //    0              Success
       //    <0             Failure. Check GetLastError().
       // Additional information:
       //    -
     //PR_01 END

   int Open(const ACS_APGCC_PIPE_Addr_R1& pipe_addr, LPSECURITY_ATTRIBUTES secattr=NULL);
   // Description:
   //    Create listening point for named pipe clients
   // Parameters:
   //    pipe_addr      Name of pipe to listen on.
   //    secattr        Security attributes that are used
   //                   when creating the pipe.
   //                   Without a security attribute, the pipe gets a default
   //                   security setting which makes it unaccessable from
   //                   remote hosts. See also Additional Information below.
   // Return value:
   //    0              Success
   //    <0             Failure. Check GetLastError().
   // Additional information:
   //    Example of creating a no-authority pipe that is accessable
   //    from remote hosts. This example makes the pipe open to
   //    everyone.
   //
   //    SECURITY_DESCRIPTOR sd;
   //    InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
   //    SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
   //    SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, FALSE };
   //
   //    MCS_PIPE_Acceptor server();
   //    if ( server.open("\\\\.\\pipe\\mypipe.pipe",&sa) < 0 ) {
   //       cerr << "Failed to create pipe!" << endl;
   //       // handle error;
   //    }



   int accept(ACS_APGCC_PIPE_Stream_R1& client);
   // Description:
   //    Accept a new client that connects to this pipe
   // Parameters:
   //    client         Pipe stream object which is used to
   //                   communicate with the connected client.
   // Return value:
   //    0              Success
   //    <0             Failure. Check GetLastError().
   // Additional information:
   //    -

   int close();
   // Close pipe

   void set_handle(ACE_HANDLE h);
   // Sets the underlaying pipe handle
   // Does not close the replaced handle.

   ACE_HANDLE get_handle() const;
   // Return the underlaying pipe handle

   ACE_event_t get_event_handle() const;
   // Return the underlaying event handle
   // It is better to use this handle in a wait call.


private:

   ACE_HANDLE hPipe_;
   // Internal pipe handle

   BOOL already_connected_;
   // True if a client is already connected

   ACE_Manual_Event event_;
   // Manual cleared event

   OVERLAPPED overlapped_;
   // Structured used for overlapped transfer

   ACS_APGCC_PIPE_Addr_R1 local_addr_;
   // Saved pipe address
   ACE_SPIPE_Acceptor hPipeAcceptor_;

   LPSECURITY_ATTRIBUTES secattr_;
   // Saved reference to security attributes supplied by user

private:

   int create_new_instance();
   // Creates a new pipe instance

   char traStr_[200];
   // Trace string

   // forbidden methods
   ACS_APGCC_PIPE_Acceptor_R1(const ACS_APGCC_PIPE_Acceptor_R1&);
   ACS_APGCC_PIPE_Acceptor_R1& operator=(const ACS_APGCC_PIPE_Acceptor_R1&);
};


//========================================================
// ACS_APGCC_PIPE_Connector
// Implements a named pipe connector object.
// Used to connect to a named pipe.
//========================================================

class ACS_APGCC_PIPE_Connector_R1: public ACE_SPIPE_Connector
{
public:
	ACS_APGCC_PIPE_Connector_R1();
   // Constructor

   virtual ~ACS_APGCC_PIPE_Connector_R1();
   // Destructor

  // int connect(ACS_APGCC_PIPE_Stream_R1& newPipe, const ACS_APGCC_PIPE_Addr_R1& pipe_addr);
   //int connect(ACS_APGCC_PIPE_Stream_R1& newPipe, const ACS_APGCC_PIPE_Addr_R1& pipe_addr, DWORD timeout=1000);
   // Description:
   //    Connect to the given named pipe and returns a PIPE stream object
   //    if successful.
   // Parameters:
   //    newPipe        Pipe stream object if connection succeeded.
   //    pipe_addr      Address of pipe to connect to.
   //    timeout        Timeout value.
   // Return value:
   //    0              Success
   //    <0             Failure. Check GetLastError().
   // Additional information:
   //    -

private:
   char traStr_[200];
   // Trace string

   // forbidden methods
   ACS_APGCC_PIPE_Connector_R1(const ACS_APGCC_PIPE_Connector_R1&);
   ACS_APGCC_PIPE_Connector_R1& operator=(const ACS_APGCC_PIPE_Connector_R1&);
};



//========================================================
// MCS_PIPE_Msg
// Quick (and dirty) send and receive object.
// Example:
//    MCS_PIPE_Msg msg;
//    string str = "Hello over there!";
//
//    if ( msg.Talk("\\\\.\\talk.pipe",str) )
//       cout << "Server replies with: " << str.c_str() << endl;
//    else
//       cout << "Failed to contact server!" << endl;
//
//========================================================

class ACS_APGCC_PIPE
{
public:

   static bool Talk(const std::string& pipe_name, std::string& msg);
   // Description:
   //    Connects to the given named pipe and sends the given
   //    message. A response is then awaited for and returned
   //    in the msg string.
   // Parameters:
   //    pipe_name      Name of pipe to connect to.
   //    msg            String that contains the message to send and
   //                   that receives the reply message.
   // Return value:
   //    true           Success
   //    false          Failure. Check GetLastError().
   // Additional information:
   //    Usage:
   //       string str = "Client says Hello!";
   //       if ( MCS_PIPE.Talk("\\\\.pipe\\mypipe.pipe",str) ) {
   //          cout << "Server replies: " str.c_str() << endl;
   //       }

};


#endif //ACS_APGCC_PIPE_R1_H
