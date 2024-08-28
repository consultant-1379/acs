//  **************************************************************************
//  **************************************************************************
//  ***                                                                    ***
//  ***  COPYRIGHT (C) Ericsson Utvecklings AB 1999                        ***
//  ***  Box 1505, 125 25 Alvsjo, Sweden, Phone: +46 8 7273000             ***
//  ***                                                                    ***
//  ***  The computer program(s) herein may be used and/or copied only     ***
//  ***  with the written permission of Ericsson Utvecklings AB or in      ***
//  ***  accordance with the terms and conditions stipulated in the        ***
//  ***  agreement/contract under which the program(s) have been supplied. ***
//  ***                                                                    ***
//  **************************************************************************
//  **************************************************************************
//
//  File Name ........@(#)fn 
//  Document Number ..@(#)dn 
//  Revision Date ....@(#)rd 
//  Current Version ..@(#)cv 
//  Responsible ............ UAB/Y/SG Martin Wahlstrom
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory

#ifdef _MSC_VER
#if _MSC_VER > 1200
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif
#endif // _MSC_VER

#pragma ident "@(#)filerevision "

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "TCPClient.hxx"
#include <cstring>
#include <cstddef>
#include <cstdio> // sprintf
#include <cassert>

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The constructor creates a TCP/IP client and initializes its member
//  variables.
//
//  \pin  theMode        Whether or not operations should be performed
//                       blocking or non-blocking (non-blocking is default).
//  \pin  errorReceiver  The function that should receive error information
//                       (NULL is default).
//>
//----------------------------------------------------------------------------
TCPClient::TCPClient(TCPClientServer::Mode theMode,
                     ErrorReceivingFunction errorReceiver) :
   TCPClientServer(theMode, errorReceiver),
   m_isConnectionRefused(false),
   m_localPortNumber(0),
   m_localIPAddress(0),
   m_highestSocketNumber(0),
   m_numberOfAddedSockets(0),
   m_isConnectCalled(false)
{
   clearSocketList();
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The destructor disconnects if connected.
//>
//----------------------------------------------------------------------------
TCPClient::~TCPClient()
{
   // Prevent exceptions to be thrown from the destructor (if one should
   // arise).
   try
   {
      disconnect();
   }
   catch(...)
   {
      // Just ignore it.
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The method is used to tell the instance at which port number and to which
//  server IP address it should establish a connection.
//
//  \pin  theRemoteAddress  The remote IP address to which connections should
//                          be established.
//  \pin  thePortNumber     The port number used for the client side.
//
//  \return  Returns TCPClient::CreateSocketError if the socket could not be
//           created or the correct mode could not be set, otherwise
//           TCPClient::OK is returned.
//>
//----------------------------------------------------------------------------
TCPClient::Status TCPClient::connectOn(u_int32 theRemoteAddress,
                                       u_int16 thePortNumber)
{
   // Verify that a socket has not already been created.
   if(m_socket != INVALID_SOCKET)
   {
      //assert(!"You are not allowed to reconfigure the TCPClient once it is set up");

      char errorMsg[512];

      sprintf(errorMsg,
              "Attempt to reconfigure an existing socket detected. The existing connection was made to %s on port %d.",
              IPAddressToString(IPAddress()),
              portNumber());

      s_errorReceivingFunction(IllegalStateForOperation,
                               __FILE__,
                               __LINE__,
                               errorMsg,
                               AlreadyInUse,
                               portNumber(),
                               (IPAddress() >> 16) & 0xFFFF,
                               IPAddress() & 0xFFFF);

      // Recover and release the already existing socket to the operating
      // system to prevent a loss of resources.
      disconnect();
   }

   // Create a socket
   if(INVALID_SOCKET == (m_socket = createSocket()))
   {
      static_cast<void>(socketError(__FILE__, __LINE__));
      return CreateSocketError;
   }

   if(m_Mode == NonBlocking)
   {
      // Set the socket in non-blocking mode.
      if(blockingOff(m_socket) != TCPClientServer::OK)
      {
         return TCPClient::CreateSocketError;
      }
   }

   initializeSocketAddressStructure(theRemoteAddress, thePortNumber);

   // No attempt has been made yet to establish a connection using this socket
   // hence no connection has been refused.
   m_isConnectionRefused = false;

   return TCPClient::OK;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Tries to establish a connection to a server.
//
//  \return  Media::Connected when the connection is established.
//           Media::AddressAlreadyInUse if the port number is already used,
//           otherwise Media::Disconnected is returned.
//           When AddressAlreadyInUse is returned, the socket is also closed
//           and the connectOn method has to be called again to be able to
//           use the instance to create a connection.
//>
//----------------------------------------------------------------------------
Media::Status TCPClient::connect()
{
   if(m_isConnected == true)
   {
      return Media::Connected;
   }
   else
   {
      if(m_socket == INVALID_SOCKET)
      {
         return Media::Disconnected;
      }
   }

   // Check to see if a previous call to ::connect has rendered into an
   // established connection.
   if(m_isConnectCalled == true &&
      isConnectionEstablished() == true)
   {
      m_isConnected = true;
      return Media::Connected;
   }
   else
   {
      // Try to connect to the server
      int32 connectResult = ::connect(m_socket,
                                      reinterpret_cast<const struct sockaddr*>(&m_socketAddress),
                                      sizeof(m_socketAddress));

      // Check for errors
      if(connectResult == 0)
      {
         m_isConnected = true;
         return Media::Connected;
      }
      else
      {
         // What went wrong?
         int32 theError = connectError(__FILE__, __LINE__);

         if(theError == AlreadyConnected)
         {
            assert(m_isConnectCalled == true &&
                   "Connect must have been called at least once for this to happen");

            m_isConnected = true;
            return Media::Connected;
         }
         else if(theError == AddressAlreadyInUse)
         {
            closeTheSocket(m_socket);
            m_isConnectionRefused = false;
            return Media::AddressAlreadyInUse;
         }
         else if(theError == ConnectionTimedOut)
         {
            return Media::Disconnected;
         }
         else if(theError == ConnectionRefused)
         {
            // No server is yet set up to listen to the connect attempt.
            // Since the error was returned immediately in the call to connect
            // no error informaiton may be read using getssockopt in the
            // pendingError method. Another call to connect must be executed
            // for as long as this faultcode is returned.

            // The socket has to be closed and reopened.
            closeTheSocket(m_socket);

            m_isConnectionRefused = false;

            // Create a socket
            if(INVALID_SOCKET == (m_socket = createSocket()))
            {
               static_cast<void>(socketError(__FILE__, __LINE__));

               return Media::Disconnected;
            }

            if(m_Mode == NonBlocking)
            {
               // Set the socket in non-blocking mode.
               if(blockingOff(m_socket) != TCPClientServer::OK)
               {
                  return Media::Disconnected;
               }
            }
            return Media::Disconnected;
         }
      }

      m_isConnectCalled = true;
      return Media::Disconnected;
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Closes the connection to the server.
//>
//----------------------------------------------------------------------------
void TCPClient::disconnect()
{
   shutDown();
   closeTheSocket(m_socket);
   m_isConnectionRefused = false;
   m_isConnected = false;
   m_localPortNumber = 0;
   m_localIPAddress = 0;
   m_isConnectCalled = false;
}

//----------------------------------------------------------------------------
//  Verifies whether the non-blocking connect request is finished.
//
//  \return  true when connected, false otherwise.
//----------------------------------------------------------------------------
bool TCPClient::isConnectionEstablished()
{
   assert(m_isConnectCalled == true &&
          "The method TCPClient::isConnectionEstablished cannot deliver valid information unless connect has been called (in a portable manner)");

   fd_set readSet;
   fd_set writeSet;
   struct timeval timeout = {0, 0};

   FD_ZERO(&readSet);
   FD_ZERO(&writeSet);
   FD_SET(static_cast<u_int32>(m_socket), &readSet);
   FD_SET(static_cast<u_int32>(m_socket), &writeSet);

   // Check to see if the file descriptor sets tells that the socket could be
   // connected.
   int32 selectReply;

#ifndef _WIN32
   selectReply = select(m_socket + 1, &readSet, &writeSet, NULL, &timeout);
#else // _WIN32
   selectReply = select(0 /* ignored input */, &readSet, &writeSet, NULL, &timeout);
#endif // _WIN32
   
   if(selectReply > 0)
   {
      if(pendingError() == NoError)
      {
         // Connection is established.
         return true;
      }
      else
      {
         // The socket has to be closed and reopened.
         closeTheSocket(m_socket);

         m_isConnectionRefused = false;

         // Create a socket
         if(INVALID_SOCKET == (m_socket = createSocket()))
         {
            static_cast<void>(socketError(__FILE__, __LINE__));

            return false;
         }

         if(m_Mode == NonBlocking)
         {
            // Set the socket in non-blocking mode.
            static_cast<void>(blockingOff(m_socket));
         }

         return false;
      }
   }
   else if(selectReply == 0)
   {
      // The operation timed out or the connection is still in progress,
      // but did not cause an error.
      return false;
   }
   else
   {
      static_cast<void>(selectError(__FILE__, __LINE__));
      return false;
   }
}

//----------------------------------------------------------------------------
//  Verifies whether there is a pending error on a socket or not.
//
//  \return  Returns NoError if there are no pending errors on the socket,
//           otherwise Error is returned and the socket has to be closed and
//           reopened again.
//----------------------------------------------------------------------------
TCPClient::SockOptError TCPClient::pendingError() const
{
   // Use getsockopt to tell whether there is a pending error on the socket or
   // not.
   int32 getsockoptReply;

   int32 errorCode;
#if defined __linux
   socklen_t length = sizeof(errorCode);
#else // !__linux
   int32 length = sizeof(errorCode);
#endif // __linux
   char* error = reinterpret_cast<char*>(&errorCode);

   // If getsockopt causes an error, then for Berkley derived implementations
   // getsockopt returns 0 with the pending error returned in the variable
   // error (getsockoptReply == 0 && *error != 0).
   // getsockopt return -1 when it causes an error and sets errno equal to the
   // pending error (getsockoptReply == -1 and error == NULL).
   // Windows lets getsockopt return -1 upon an error and requires
   // WSAGetLastError to be called to find out about the error.
   // Tru64, Solaris and Windows lets getsockopt return 0 for successful
   // execution. However for Tru64 and Solaris, the error (not errno)
   // parameter reflect the error.
   // The code below covers the possible error conditions for
   // non-Berkley and Berkley derived implementations.
   getsockoptReply = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, error, &length);

   if(!(getsockoptReply < 0 || errorCode != 0))
   {
      return m_isConnectionRefused == false ? NoError : Error;
   }
   else
   {
      // Either getsockoptReply is < 0 or errorCode is non 0.
      // This covers faults by both Berkley-derived and non Berkley-derived
      // implementations of the socket API.
      return Error;
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The status method returns the current status of the instance.
//
//  \return  Always returns ObjectOK.
//>
//----------------------------------------------------------------------------
ObjectStatus TCPClient::status() const
{
   return ObjectOK;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The returnToInitialState method is used to set the instance in an
//  operational state when an error has occurred.
//>
//----------------------------------------------------------------------------
void TCPClient::returnToInitialState()
{
}

//----------------------------------------------------------------------------
//  Used in the disconnect method to bring down the socket gracefully.
//----------------------------------------------------------------------------
void TCPClient::shutDown()
{
   // Make the call to shutdown only when the socket is valid and it is in
   // use by an established connection.
   if(m_socket != INVALID_SOCKET &&
      m_isConnected)
   {
      if(shutdown(m_socket, ShutDownSendAndReceive) != 0)
      {
         // If it is a real error, this will log the cause.
         static_cast<void>(shutdownError(__FILE__, __LINE__));
      }
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The send method does not guarantee that all the data passed to the method
//  is sent. It may be partially sent upon return.
//
//  \pin  data   Pointer to an area of bytes that should be sent.
//
//  \pinout  size   The number of bytes that should be sent, which the pointer
//                  data points to. Upon return, the number of bytes
//                  successfully sent when TCPClient::NotOK is returned.
//                  The output of this parameter should be ignored for all
//                  other return values.
//
//  \return  Returns TCPClient::OK if the send was successful. Returns
//           TCPClient::NotOK if the data was delivered partially, then the
//           number of bytes successfully transferred is found in the size
//           parameter. Returns TCPClient::Disconnected if the connection has
//           been terminated.
//>
//----------------------------------------------------------------------------
TCPClient::Status TCPClient::send(const char* data, u_int32& size)
{
   if(m_isConnected)
   {
      if(size == 0)
      {
         return OK;
      }
#ifdef _DEBUG
      else if(data == NULL)
      {
         //assert(!"The pointer to the data that should be sent is a NULL pointer although the size tells that it should point to something");
         return TCPClient::NotOK;
      }
#endif // _DEBUG

      // The socket is writable
      u_int32 sendReply;

      sendReply = ::send(m_socket, data, size, 0);

      if(sendReply == size)
      {
         // The bytes were all sent successfully.
         return OK;
      }
      else if(static_cast<int32>(sendReply) != SendError)
      {
         // A part of the data was sent successfully. Let the caller know the
         // amount of data that was sent.
         size = sendReply;
         return NotOK;
      }
      else
      {
         // The call to send caused an error.
         if(isConnectionClosed(sendError(__FILE__, __LINE__)) == true)
         {
            // The connection has been closed. Close our end as well.
            disconnect();
            return Disconnected;
         }
         else
         {
            size = 0;
            return NotOK;
         }
      }
   }
   else
   {
      // The client is not connected and cannot handle send requests.
      size = 0;
      return Disconnected;
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Polls a connected socket for incoming data.
//
//  \pinout  data   Pointer to a buffer to which polled data can be read, the
//                  buffer must already be allocated to cope with the amount of
//                  data requested. Upon return, the contents of data is valid
//                  only when the size output parameter is non 0.
//  \pinout  size   The size of the buffer data in bytes.
//                  Upon return, the number of bytes read into the data buffer.
//
//  \return  Returns TCPClient::OK if the poll was successful. Returns
//           TCPClient::Disconnected if there is no TCP connection.
//>
//----------------------------------------------------------------------------
TCPClient::Status TCPClient::poll(char* data, u_int32& size)
{
   if(size != 0 && (m_Mode == Blocking || isReadyForReading()))
   {
      int32 recvReply;

      recvReply = ::recv(m_socket, data, size, 0);

      if(recvReply > 0)
      {
         // recvReply number of bytes were read.
         size = recvReply;
         return OK;
      }
      else if(recvReply == 0)
      {
         // The TCP session has been disconnected.
         size = 0;
         disconnect();
         return Disconnected;
      }
      else
      {
         size = 0;

         // The call to recv caused an error.
         if(isConnectionClosed(recvError(__FILE__, __LINE__)) == false)
         {
            return OK;
         }
         else
         {
            // The connection has been closed. Close our end as well.
            disconnect();
            return Disconnected;
         }
      }
   }

   size = 0;
   return m_isConnected ? OK : Disconnected;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Polls a connected socket for incoming data. The socket is read without
//  checking if the socket is readable (useful with blocking sockets and if
//  you already know that the socket is readable). 
//
//  \pinout  data   Pointer to a buffer to which polled data can be read, the
//                  buffer must already be allocated to cope with the amount of
//                  data requested. Upon return, the contents of data is valid
//                  only when the size output parameter is non 0.
//  \pinout  size   The size of the buffer data in bytes.
//                  Upon return, the number of bytes read into the data buffer.
//
//  \return  Returns TCPClient::OK if the poll was successful. Returns
//           TCPClient::Disconnected if there is no TCP connection.
//>
//----------------------------------------------------------------------------
TCPClient::Status TCPClient::pollWithoutSelect(char* data, u_int32& size)
{
   if(size != 0)
   {
      int32 recvReply;

      recvReply = ::recv(m_socket, data, size, 0);

      if(recvReply > 0)
      {
         // recvReply number of bytes were read.
         size = recvReply;
         return OK;
      }
      else if(recvReply == 0)
      {
         // The TCP session has been disconnected.
         size = 0;
         disconnect();
         return Disconnected;
      }
      else
      {
         size = 0;

         // The call to recv caused an error.
         if(isConnectionClosed(recvError(__FILE__, __LINE__)) == false)
         {
            return OK;
         }
         else
         {
            // The connection has been closed. Close our end as well.
            disconnect();
            return Disconnected;
         }
      }
   }

   size = 0;
   return m_isConnected ? OK : Disconnected;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Tells local port number associated with a socket. When for instance the
//  client connects to a server's port number pn, then the local port number
//  is assigned by the operating system. This method may be used to retrieve
//  the local port number associated with a connection. If the method fails to
//  retrieve the actual local port number or the client is not yet connected,
//  then 0 is returned.
//
//  \return  The local port number associated with a socket.
//>
//----------------------------------------------------------------------------
u_int16 TCPClient::localPortNumber()
{
   if(m_localPortNumber == 0 &&
      m_isConnected == true)
   {
      sockaddr address;

#if defined __linux
      socklen_t len = sizeof(sockaddr);
#else // !__linux
      int32 len = sizeof(sockaddr);
#endif // __linux

      if(getsockname(m_socket, &address, &len) >= 0)
      {
         m_localPortNumber = ntohs((reinterpret_cast<struct sockaddr_in*>(&address)->sin_port));
      }
      else
      {
         // Log the error.
         static_cast<void>(getsocknameError(__FILE__, __LINE__));
      }
   }

   return m_isConnected == true ? m_localPortNumber : 0;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Tells local IP addressassociated with a socket.
//
//  \return  The local IP address associated with a socket.
//>
//----------------------------------------------------------------------------
u_int32 TCPClient::localIPAddress()
{
   if(m_localIPAddress == 0 &&
      m_isConnected == true)
   {
      sockaddr address;
#if defined __linux
      socklen_t len = sizeof(sockaddr);
#else // !__linux
      int32 len = sizeof(sockaddr);
#endif // __linux

      if(getsockname(m_socket, &address, &len) >= 0)
      {
         m_localIPAddress = ntohl((reinterpret_cast<struct sockaddr_in*>(&address)->sin_addr.s_addr));
      }
      else
      {
         // Log the error.
         static_cast<void>(getsocknameError(__FILE__, __LINE__));
      }
   }

   return m_isConnected == true ? m_localIPAddress : 0;
}

