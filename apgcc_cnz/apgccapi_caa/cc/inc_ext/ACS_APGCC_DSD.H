//*****************************************************************************
//
// .NAME
//    ACS_APGCC_DSD - Internal wrapper class, that converts the
//               ACS DSD interface to follow the ACE way.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_DSD.H

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
//  See ACS_APGCC_DSD.
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
//      A       98-03-18

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//      A               98-03-18        SJEN            First release
//      B               99-02-06        SJEN            Second release.
//                                                      Uses new DSD API.

// .LINKAGE
//      -lMCC -lACS_DSD

//*****************************************************************************
#ifndef ACS_APGCC_DSD_H_
#define ACS_APGCC_DSD_H_


#include "ace/Sig_Adapter.h" // for ACE_HANDLER
#include <string>
#include "ACS_DSD_Server.h"

#ifdef ACS_APGCC_DSD_USE_SOCKETS

#  ifndef ACE_INET_ADDR_H
#    include "ace/INET_Addr.h"
#  endif
#  ifndef ACE_SOCK_STREAM_H
#    include <ace/SOCK_Stream.h>
#  endif
#  ifndef ACE_SOCK_ACCEPTOR_H
#    include <ace/SOCK_Acceptor.h>
#  endif
#  ifndef ACE_SOCK_CONNECTOR_H
#    include <ace/SOCK_Connector.h>
#  endif



#else // !ACS_APGCC_DSD_USE_SOCKETS

#  ifndef ACE_OS_H
//ADDs for ACElib porting
//#    include <osf/OS.h>
#include "ace/OS.h"
#  endif
#endif   //ACS_APGCC_DSD_USE_SOCKETS
#  include "ACS_APGCC_OS.H"
#  include "ACS_DSD_Client.h"

//end ADDs



//--------------------------------------------------
// AGPCC DSD Address
//
// Requires the adress on the form "domain:appl"
//
//--------------------------------------------------

class ACS_APGCC_DSD_Addr
{
public:

   friend class ACS_APGCC_DSD_Acceptor;
#ifdef ACS_APGCC_DSD_USE_SOCKETS
   friend class ACS_APGCC_DSD_Acceptor_Impl;
   friend class ACS_APGCC_DSD_Connector_Impl;
#endif

   ACS_APGCC_DSD_Addr();
   ACS_APGCC_DSD_Addr(const char* address);
   ACS_APGCC_DSD_Addr(const char* address, const char* node);
   ACS_APGCC_DSD_Addr(const ACS_APGCC_DSD_Addr& addr);
   virtual ~ACS_APGCC_DSD_Addr();

   ACS_APGCC_DSD_Addr& operator=(const ACS_APGCC_DSD_Addr& addr);

   void set(const char* address);
   void set(const ACS_APGCC_DSD_Addr& addr);
   void setNetwork(const int network);

   const char* getDomain() const;
   const char* getAppl() const;
   const char* getNode() const;
   int getNetwork() const;

protected:

   std::string domain_;
   std::string appl_;
   std::string node_;
   int network_;
   int port_;
   unsigned long ip_;

#ifdef ACS_APGCC_DSD_USE_SOCKETS
   ACE_INET_Addr remSockAddr_;
   ACE_INET_Addr locSockAddr_;
#endif
   char traStr_[100];
};


//--------------------------------------------------
// ACS_APGCC DSD Stream
//--------------------------------------------------

class ACS_APGCC_DSD_Stream
{
public:

   friend class ACS_APGCC_DSD_Acceptor;
   friend class ACS_APGCC_DSD_Connector;
#ifdef APGCC_DSD_USE_SOCKETS
   friend class ACS_APGCC_DSD_Acceptor_Impl;
   friend class ACS_APGCC_DSD_Connector_Impl;
#endif

   ACS_APGCC_DSD_Stream();
   virtual ~ACS_APGCC_DSD_Stream();

   int recv(void* buf, size_t bufSize, bool wrapData=true);
   int recv_n(void* buf, size_t bufSize, bool wrapData=true);
   // Description:
   //    Receives a data message.
   // Parameters:
   //    buf         Buffer to hold data message.
   //                Must be the size of the maximum data
   //                size that can be received plus 4 bytes.
   //                e.g  buf = new char[MAX_DATA_SIZE+4].
   //                4 bytes is reserved for header information.
   //    bufSize     Size of the allocated buffer (e.g MAX_DATA_SIZE).
   //    wrapData    If true, user data begins at buf[0]. The last four
   //                bytes will replace the first four bytes in the buffer,
   //                thus overwritten the header info.
   //                If false, user data begins at buf[4]. The first
   //                four bytes will remain unchanged.
   // Return value:
   //    >0          Size of received data message.
   //    -1          Failed.
   // Additional information:
   //    Both receive methods are blocking.
   //    Both the sender and the receiver must use the same value
   //    of the wrapData field.

   int recvBuffer(char*& buf, bool wrapData=true);
   int recvBuffer(unsigned char*& buf, bool wrapData=true);
   // Description:
   //    Receives a data message.
   // Parameters:
   //    buf         Buffer pointer.
   //                The method returns a new allocated buffer
   //                with the received data message. The last
   //                position in the buffer will be NULL (0), e.g. buf[len]=0.
   //                The buffer MUST be freed by the user
   //                of this method.
   //    wrapData    If true, user data begins at buf[0]. The last four
   //                bytes will replace the first four bytes in the buffer,
   //                thus overwriting the header info.
   //                If false, user data begins at buf[4]. The first
   //                four bytes will remain unchanged.
   // Return value:
   //    >0          Size of received data message excluding the ending NULL sign.
   //    -1          Failed.
   // Additional information:
   //    The method blocks until all data has been received.

   int send(const void* buf, size_t bufSize, bool wrapData=true);
   int send_n(const void* buf, size_t bufSize, bool wrapData=true);
   // Description:
   //    Sends a data message.
   // Parameters:
   //    buf         Buffer that holds data message.
   //                Buffer must be able to hold bufSize plus 4 extra bytes.
   //                e.g  buf = new char[bufSize+4].
   //                4 bytes is reserved for header information.
   //    bufSize     Size of the data to be sent excluding header info.
   //    wrapData    If true, user data begins at buf[0]. The first
   //                four bytes will then be placed at the end of the buffer,
   //                and the header is inserted at pos buf[0].
   //                If false, user data begins at buf[4]. The first
   //                four bytes is overwritten by the header info.
   // Return value:
   //    >=0         Size of sent data message excluding header info.
   //    -1          Failed.
   // Additional information:
   //    Both send methods are blocking.
   //    Both the sender and the receiver must use the same value
   //    of the wrapData field.

   int recv_cp(void* buf, size_t bufSize, bool moveData=false);
   // Description:
   //    Receives a data message from the CP.
   // Parameters:
   //    buf         Buffer to hold data message.
   //                Must be the size of the maximum data
   //                size that can be received plus 4 bytes.
   //                e.g  buf = new char[MAX_DATA_SIZE+4].
   //                4 bytes is reserved for header information.
   //    bufSize     Size of the allocated buffer (e.g MAX_DATA_SIZE).
   //    moveData    If true, user data is shifted down four bytes so that
   //                it starts at buf[0], thus overwriting the header info.
   //                If false, user data remains unchanged and begins at buf[4].
   //                The first four bytes contains the header info.
   // Return value:
   //    >0          Size of received data message.
   //    -1          Failed.
   // Additional information:
   //    The method blocks until all data has been received.

   int send_cp(const void* buf, size_t bufSize);
   // Description:
   //    Sends a data message to the CP.
   // Parameters:
   //    buf         Buffer that holds data message.
   //                Buffer must be able to hold bufSize plus 4 extra bytes.
   //                e.g  buf = new char[bufSize+4].
   //                4 bytes is reserved for header information.
   //    bufSize     Size of the data to be sent excluding header info.
   //                User data begins at buf[0].
   // Return value:
   //    >=0         Size of sent data message excluding header info.
   //    -1          Failed.
   // Additional information:
   //    The method blocks until all data has been sent.
   //    User data begins at buf[0]. The first four bytes will be placed
   //    at the end of the buffer, and the header is inserted at pos buf[0].

   int close();
   // Description:
   //    Close the stream.
   // Parameters:
   //    -
   // Return value:
   //    0           Successful.
   // Additional information:
   //    -

  // ACE_HANDLE get_handles(acs_dsd::HANDLE * /*handles*/, int & handle_count);
   // Description:
   //    Returns a handle to the stream.
   // Parameters:
   //    -
   // Return value:
   //    >0                   Valid handle that can be used in a Reactor/Select.
   //    OSF_INVALID_HANDLE   No handle is available.
   // Additional information:
   //    -

   const char* getErrorText();
   size_t getError();
   ACE_HANDLE get_handles(acs_dsd::HANDLE * handles, int & handle_count);

   //--------------------------------------------------------
   // The below three methods should not be used after SIO I4
   //--------------------------------------------------------

#ifdef ACS_APGCC_DSD_USE_SOCKETS

   //ADDs for ACElib porting => ACE_HANDLER instead of OSF_HANDLER
   void set_handle(ACE_HANDLE newHandle);
   // Sets the handle for the internal socket.
   // Does NOT start the internal thread.
   // BE AWARE: It is only possible to use the send_rpc()
   //           and recv_rpc() methods when this is used.

   int send_rpc(const void* buf, size_t len);
   // Pre-sends an RPC header before the data.
   // The data remains unchanged.
   // BE AWARE: This uses the INTERNAL socket stream!!
   //           It must be set using set_handle().

   int recv_rpc(void* buf, size_t len);
   // Receives RPC data.
   // The data buffer is untouched.
   // BE AWARE: This uses the INTERNAL socket stream!!
   //           It must be set using set_handle().

   size_t id() const;
   // Internal; Do NOT use.

#endif //ACS_APGCC_DSD_USE_SOCKETS

protected:

#ifdef ACS_APGCC_DSD_USE_SOCKETS
   ACS_APGCC_DSD_Stream_Impl* impl_;
   ACE_SOCK_Stream sockStream_;
#else
   void wrap_data(void* buf, size_t& len);
   void unwrap_data(void* buf, int& len);

   ACS_DSD_Session dsdStream_;
#endif

private:

   char traStr_[100];
   ACS_APGCC_DSD_Stream(const ACS_APGCC_DSD_Stream&);
   ACS_APGCC_DSD_Stream& operator=(const ACS_APGCC_DSD_Stream&);

};



//--------------------------------------------------
// ACS_APGCC DSD Connector
//--------------------------------------------------
//#include "ACS_DSD_DSA2.H" // for ACS_DSD_Client stub

class ACS_APGCC_DSD_Connector
{
public:

   ACS_APGCC_DSD_Connector();
   virtual ~ACS_APGCC_DSD_Connector();

   int connect(ACS_APGCC_DSD_Stream& newStream, const ACS_APGCC_DSD_Addr& addr);
   // Description:
   //    Connects to an DSD acceptor.
   // Parameters:
   //    newStream   New stream if connection to the DSD server
   //                succeeded.
   //    addr        Name of DSD server application.
   // Return value:
   //    0           Successful.
   //    <0          Failed.
   // Additional information:
   //    See also MCS_DSD_Stream.

   int close();
   // Description:
   //    Close the connector.
   // Parameters:
   //    -
   // Return value:
   //    0           Successful.
   // Additional information:
   //    -

   const char* getErrorText();
   size_t getError();

protected:

#ifdef ACS_APGCC_DSD_USE_SOCKETS
   ACS_APGCC_DSD_Connector_Impl* impl_;
#else
    //ACS_APGCC_DSD_Service dsdCon_;
	// New class for DSD client
   ACS_DSD_Client dsdCon_;

#endif

private:

   char traStr_[100];
   ACS_APGCC_DSD_Connector(const ACS_APGCC_DSD_Connector&);
   ACS_APGCC_DSD_Connector& operator=(const ACS_APGCC_DSD_Connector&);

};


//--------------------------------------------------
// ACS_APGCC DSD Acceptor
//--------------------------------------------------

class ACS_APGCC_DSD_Acceptor
{
public:

   friend class ACS_APGCC_DSD_Acceptor_Impl;

   ACS_APGCC_DSD_Acceptor();
   virtual ~ACS_APGCC_DSD_Acceptor();

   int open(const ACS_APGCC_DSD_Addr& addr);
   // Description:
   //    Initiates an acceptor.
   // Parameters:
   //    addr        Name of DSD server application.
   // Return value:
   //    0           Successful.
   //    <0          Failed.
   // Additional information:
   //    -

   int open(const std::string& appl, const std::string& domain = "MCS");
   // Description:
   //    Initiates an acceptor.
   // Parameters:
   //    appl        Name of DSD server application.
   //    domain      Name of server domain. Default is "MCS".
   // Return value:
   //    0           Successful.
   //    <0          Failed.
   // Additional information:
   //    -

   int close();
   // Description:
   //    Close the acceptor.
   // Parameters:
   //    -
   // Return value:
   //    0           Successful.
   // Additional information:
   //    -

   int accept(ACS_APGCC_DSD_Stream& newStream);
   int accept(ACS_APGCC_DSD_Stream& newStream, ACS_APGCC_DSD_Addr& remAddr);
   // Description:
   //    Accepts a connection request from a DSD client (connector).
   // Parameters:
   //    newStream   Accepted new stream.
   //    addr        Address of the DSD client application.
   // Return value:
   //    0           Successful.
   //    <0          Failed.
   // Additional information:
   //    See also MCS_DSD_Stream.

   //ADDs for ACElib porting => ACE_HANDLER instead of OSF_HANDLER
   ACE_HANDLE get_handle(int index=0);
   //end ADDs

   // Description:
   //    Returns a handle to the acceptor.
   // Parameters:
   //    index                Handle index.
   //                         It is possible that DSD creates two servers
   //                         to listen on. This parameter can then be used to
   //                         get both handles.
   //                            index=0     Return first handle.
   //                            index=1     Return second handle.
   //
   //                         If no handle was present, OSF_INVALID_HANDLE is returned.
   // Return value:
   //                         Note that a true Win32 HANDLE is returned on Win32, e.g.
   //                         not a socket !!
   //    >0                   Valid handle that can be used in a Reactor/Select.
   //    OSF_INVALID_HANDLE   No handle is available.
   // Additional information:
   //    -

   const char* getErrorText();
   size_t getError();

#ifdef ACS_APGCC_DSD_USE_SOCKETS
   size_t id() const;
#endif

protected:


   ACS_DSD_Server* dsdServer_;

private:

   char traStr_[100];
   ACS_APGCC_DSD_Acceptor(const ACS_APGCC_DSD_Acceptor&);
   ACS_APGCC_DSD_Acceptor& operator=(const ACS_APGCC_DSD_Acceptor&);

};


#endif /* ACS_APGCC_DSD_H_ */
