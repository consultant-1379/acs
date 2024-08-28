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
//
//< \file
//
//  The TCPClient provides TCP access routines for active establishment of TCP
//  connections (connect), sending and receiving data, disabling Nagle's
//  algorithm (TCP_NODELAY) as well as taking ownership of an already
//  connected socket (e.g. transferred from the TCPServer).
//  The class handles both blocking and non-blocking connections.
//>

#ifndef TCPCLIENT_HXX
#define TCPCLIENT_HXX

#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#pragma warning ( disable : 4244 )
#endif // _MSC_VER

#pragma ident "@(#)filerevision "

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "TCPClientServer.hxx"
#include "TCPServer.hxx"
#include "Media.hxx"
#include <assert.h>

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

//< \ingroup comm
//  Class relationships (relations to TCPClientServer).
//
//  \verbatim
//  +-------------------+
//  |                   |
//  |  TCPClientServer  |
//  |                   |
//  +-------------------+
//            ^
//            |
//           Inh
//            |
//     +-------------+
//     |             |
//     |  TCPClient  |
//     |             |
//     +-------------+
//
//  Inh - Inheritance
//  \endverbatim
//>
class TCPClient : public TCPClientServer
{
public:
   // Create a TCPClient.
   TCPClient(TCPClientServer::Mode = NonBlocking, ErrorReceivingFunction = NULL);

   // Destroy a TCPClient. Disconnects the connected socket (if connected).
   ~TCPClient();

   // Send data.
   Status send(const char*, u_int32&);
   Status send(const u_char*, u_int32&);
   Status send(const char*, u_int32, u_int32&);
   Status send(const u_char*, u_int32, u_int32&);

   // Receive data.
   Status poll(char*, u_int32&);
   Status poll(u_char*, u_int32&);
   Status poll(char*, u_int32, u_int32&);
   Status poll(u_char*, u_int32, u_int32&);
   Status pollWithoutSelect(char*, u_int32&);
   Status pollWithoutSelect(u_char*, u_int32&);
   Status pollWithoutSelect(char*, u_int32, u_int32&);
   Status pollWithoutSelect(u_char*, u_int32, u_int32&);

   // Create a socket and store information about which IP address to connect
   // to.
   Status connectOn(u_int32, u_int16);

   // Creates a socket and sets up internal structures regarding remote IP
   // address and port number to connect to according to what was the input to
   // the connectOn method before.
   Status reconnect();

   // Try to establish a connection (TCP).
   Media::Status connect();

   // Disconnect a connected socket.
   void disconnect();

   // Disable Nagle's algorithm for a socket.
   void disableNagle();

   // Enable Nagle's algorithm for a socket (default).
   void enableNagle();

   // Tells local port number associated with a socket. When for instance the
   // client connects to a server's port number pn, then the local port number
   // is assigned by the operating system. This method may be used to retrieve
   // the local port number associated with a connection.
   u_int16 localPortNumber();

   // Tells local IP address associated with a socket.
   u_int32 localIPAddress();

   // Takes an already connected socket and treats it as its own. If the
   // instance already is using a socket (connected or not), the supplied
   // one replaces the old one.
   void useThisSocket(TCPClientServer::SocketInformation);

   // Returns the socket number.
   SOCKET socket() const;

   // Clears the list of sockets.
   void clearSocketList();

   // Adds a new socket to the list of sockets which may be verified for
   // readability.
   void addSocketToList(SOCKET);

   // Adds a new socket to the list of sockets which may be verified for
   // readability.
   void addSocketToList(const TCPClient&);

   // Adds a new socket to the list of sockets which may be verified for
   // readability.
   void addSocketToList(const TCPServer&);

   // Removes a socket from the list of sockets which may be verified for
   // readability.
   void removeSocketFromList(SOCKET);

   // Verifies whether or not any of the sockets in the socket list has any
   // data available for reading and returns the number of readable sockets.
   u_int16 checkReadability(TCPClientServer::Timeout = TCPClientServer::Timeout());

   // Checks if the socket is readable (requires a previous call to
   // checkReadability).
   bool isReadable(SOCKET) const;

   // Checks if the TCPClient's socket is readable (requires a previous call to
   // checkReadability).
   bool isReadable(const TCPClient&) const;

   // Checks if the TCPServer's socket is readable (requires a previous call to
   // checkReadability).
   bool isReadable(const TCPServer&) const;

   // Returns the status of the instance.
   ObjectStatus status() const;

   // Sets the instance in a well-defined operational state.
   void returnToInitialState();

private:
   // Declare but do not implement in order to render the copy ctor unsuable.
   TCPClient(const TCPClient&);

   enum SockOptError
   {
      NoError,
      Error
   };

   // Used in the disconnect method to bring down the socket gracefully.
   void shutDown();

   // Verifies whether the non-blocking connect request is finished.
   bool isConnectionEstablished();

   // Verifies whether the connection is closed depending on what error is set.
   bool isConnectionClosed(int32) const;

   // Verifies whether or not the socket has any data available for reading.
   bool isReadyForReading() const;

   // Verifies whether there is a pending error on a socket or not.
   SockOptError pendingError() const;

   // When the call to connect returns 'connection refused' immediately, then
   // a call to connect is required instead of trying to read pending errors
   // off the socket. This member keeps track of when that is necessary.
   bool m_isConnectionRefused;

   // True once connect has been called once after a call to connectOn. It is
   // used to determine when to call ::connect or isConnectionEstablished.
   bool m_isConnectCalled;

   // Holds information about the local port number assigned by the operating
   // system when connected to a server port (known).
   u_int16 m_localPortNumber;

   // Holds information about the local IP address when connected to a server
   // IP address (known).
   u_int32 m_localIPAddress;

   // A read set used when checking if multiple sockets are available for
   // reading at the same time.
   fd_set m_readSet;

   // The highest registered socket number in the m_readSet.
   SOCKET m_highestSocketNumber;

   // The number of added sockets to the m_readSet.
   u_int16 m_numberOfAddedSockets;
};

//----------------------------------------------------------------------------
//<
//  Takes an already connected socket and treats it as its own. If the
//  instance already is using a socket (connected or not), the supplied
//  one replaces the old one and the old connected socket is disconnected.
//
//  \pin  newSocket  A connected socket (along with mode information) to use
//                   for communication.
//>
//----------------------------------------------------------------------------
inline void TCPClient::useThisSocket(TCPClientServer::SocketInformation newSocket)
{
   if(m_socket != INVALID_SOCKET)
   {
      // Close the already existing socket.
      closeTheSocket(m_socket);
   }

   m_socket = newSocket.m_socket;
   m_isConnectionRefused = false;


   // Assign the requested mode on the connected socket (if the connected
   // socket is in an undesired mode).
   if(newSocket.m_mode != m_Mode)
   {
      TCPClientServer::Status status;

      if(m_Mode == NonBlocking)
      {
         status = blockingOff(m_socket);
      }
      else
      {
         status = blockingOn(m_socket);
      }

      if(status != TCPClientServer::OK)
      {
         // Setting the required mode failed, disconnect and destroy the
         // socket.
         disconnect();
      }
      else
      {
         m_isConnected = true;
      }
   }
   else
   {
      m_isConnected = true;
   }
}

//----------------------------------------------------------------------------
//  Verifies whether or not the connection is closed based upon the error
//  code.
//
//  \pin  error  The error that occurred.
//
//  \return  True if the connection is closed, false otherwise.
//----------------------------------------------------------------------------
inline bool TCPClient::isConnectionClosed(int32 error) const
{
   return error == Disconnected ? true : false;
}

//----------------------------------------------------------------------------
//  Verifies whether or not the socket has any data available for reading.
//
//  \return  True if data is available for reading, false otherwise.
//----------------------------------------------------------------------------
inline bool TCPClient::isReadyForReading() const
{
   if(m_socket != INVALID_SOCKET)
   {
      fd_set readSet;

      FD_ZERO(&readSet);
      FD_SET(static_cast<u_int32>(m_socket), &readSet);

      // Check to see if the file descriptor sets tells that the socket could be
      // connected.
      int32 selectReply;
      struct timeval timeout = {0, 0};

#ifndef _WIN32
      selectReply = select(m_socket + 1, &readSet, NULL, NULL, &timeout);
#else // _WIN32
      selectReply = select(0 /* ignored input */, &readSet, NULL, NULL, &timeout);
#endif // _WIN#2
   
      if(selectReply > 0)
      {
         // The socket has data available for reading.
         return true;
      }
      else if(selectReply == 0)
      {
         // The socket does not have any data available for reading.
         return false;
      }
      else
      {
         // The call to select caused an Error.
         if(pendingError() == NoError)
         {
            // The socket does not have any data available for reading.
            return false;
         }
         else
         {
            // The socket has an error pending. Let it be fetched by the poll
            // routine. Return true to tell the poll method that data is
            // available for reading, although the recv call only will catch
            // the error.
            return true;
         }
      }
   }
   else
   {
      return false;
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The send method does not guarantee that all the data passed to the method
//  is sent. It may be partially sent upon return.
//
//  \pin  data        Pointer to an area of bytes that should be sent.
//  \pin  sizeToSend  The number of bytes that should be sent, which the
//                    pointer data points to.
//
//  \pout  sizeSent  The number of bytes successfully sent when
//                   TCPClient::NotOK is returned. The output of this
//                   parameter should be ignored for all other return values.
//
//  \return  Returns TCPClient::OK if the send was successful. Returns
//           TCPClient::NotOK if the data was delivered partially, then the
//           number of bytes successfully transferred is found in the size
//           parameter. Returns TCPClient::Disconnected if the connection has
//           been terminated.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::send(const char* data,
                                         u_int32 sizeToSend,
                                         u_int32& sizeSent)
{
   sizeSent = sizeToSend;

   return send(data, sizeSent);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The send method does not guarantee that all the data passed to the method
//  is sent. It may be partially sent upon return.
//
//  \pin  data        Pointer to an area of bytes that should be sent.
//  \pin  sizeToSend  The number of bytes that should be sent, which the
//                    pointer data points to.
//
//  \pout  sizeSent  The number of bytes successfully sent when
//                   TCPClient::NotOK is returned. The output of this
//                   parameter should be ignored for all other return values.
//
//  \return  Returns TCPClient::OK if the send was successful. Returns
//           TCPClient::NotOK if the data was delivered partially, then the
//           number of bytes successfully transferred is found in the size
//           parameter. Returns TCPClient::Disconnected if the connection has
//           been terminated.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::send(const u_char* data,
                                         u_int32 sizeToSend,
                                         u_int32& sizeSent)
{
   sizeSent = sizeToSend;

   return send(reinterpret_cast<const char*>(data), sizeSent);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The send method does not guarantee that all the data passed to the method
//  is sent. It may be partially sent upon return.
//
//  \pin  data   Pointer to an area of bytes that should be sent.
//
//  \pinout  size  The number of bytes that should be sent, which the pointer
//                 data points to. Upon return, the number of bytes
//                 successfully sent when TCPClient::NotOK is returned.
//                 The output of this parameter should be ignored for all
//                 other return values.
//
//  \return  Returns TCPClient::OK if the send was successful. Returns
//           TCPClient::NotOK if the data was delivered partially, then the
//           number of bytes successfully transferred is found in the size
//           parameter. Returns TCPClient::Disconnected if the connection has
//           been terminated.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::send(const u_char* data, u_int32& size)
{
   return send(reinterpret_cast<const char*>(data), size);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Polls a connected socket for incoming data.
//
//  \pinout  data        Pointer to a buffer to which polled data can be read.
//                       Upon return, the contents of data is valid only when
//                       the size output parameter is non 0.
//
//  \pin     wantedSize  The wanted size in bytes of a received message.
//
//  \pout    polledSize  The number of bytes that actually has been read and
//                       returned pointed to by data.
//
//  \return  Returns TCPClient::OK if the poll was successful. Returns
//           TCPClient::Disconnected if the connection has been terminated.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::poll(char* data,
                                         u_int32 wantedSize,
                                         u_int32& polledSize)
{
   polledSize = wantedSize;

   return poll(data, polledSize);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Polls a connected socket for incoming data.
//
//  \pinout  data        Pointer to a buffer to which polled data can be read.
//                       Upon return, the contents of data is valid only when
//                       the size output parameter is non 0.
//
//  \pin     wantedSize  The wanted size in bytes of a received message.
//
//  \pout    polledSize  The number of bytes that actually has been read and
//                       returned pointed to by data.
//
//  \return  Returns TCPClient::OK if the poll was successful. Returns
//           TCPClient::Disconnected if the connection has been terminated.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::poll(u_char* data,
                                         u_int32 wantedSize,
                                         u_int32& polledSize)
{
   polledSize = wantedSize;

   return poll(reinterpret_cast<char*>(data), polledSize);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Polls a connected socket for incoming data.
//
//  \pinout  data  Pointer to a buffer to which polled data can be read, the
//                 buffer must already be allocated to cope with the amount of
//                 data requested. Upon return, the contents of data is valid
//                 only when the size output parameter is non 0.
//  \pinout  size  The size of the buffer data in bytes.
//                 Upon return, the number of bytes read into the data buffer.
//
//  \return  Returns TCPClient::OK if the poll was successful. Returns
//           TCPClient::Disconnected if there is no TCP connection.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::poll(u_char* data, u_int32& size)
{
   return poll(reinterpret_cast<char*>(data), size);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Disables Nagle's algorithm.
//>
//----------------------------------------------------------------------------
inline void TCPClient::disableNagle()
{
   TCPClientServer::disableNagle(m_socket, __FILE__, __LINE__);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Enables Nagle's algorithm.
//>
//----------------------------------------------------------------------------
inline void TCPClient::enableNagle()
{
   TCPClientServer::enableNagle(m_socket, __FILE__, __LINE__);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Creates a socket and sets up internal structures regarding remote IP
//  address and port number to connect to according to what was the input to
//  the connectOn method before.
//
//  \return  Returns the same codes a the connectOn method.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::reconnect()
{
   assert(portNumber() != 0 &&
          "The port number has never been initialized, call the connectOn method prior to reconnect!");

   assert(m_socket == INVALID_SOCKET &&
          "An active socket should be closed before using reconnect. Call disconnect first");

   // Try to establish a connection to the same IP address and port number as
   // was used before.
   return connectOn(IPAddress(), portNumber());
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Clears the list of sockets.
//>
//----------------------------------------------------------------------------
inline void TCPClient::clearSocketList()
{
   FD_ZERO(&m_readSet);
   m_highestSocketNumber = 0;
   m_numberOfAddedSockets = 0;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Adds a new socket to the list of sockets.
//
//  \pin  newSocket  The socket that should be added to the socket list.
//>
//----------------------------------------------------------------------------
inline void TCPClient::addSocketToList(SOCKET newSocket)
{
   // Add only valid sockets to the socketlist.
   if(newSocket != INVALID_SOCKET)
   {
      FD_SET(static_cast<u_int32>(newSocket), &m_readSet);
      if(newSocket > m_highestSocketNumber)
      {
         m_highestSocketNumber = newSocket;
      }
      m_numberOfAddedSockets++;
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Adds a new socket to the list of sockets.
//
//  \pin  theClient  A TCPClient whose socket should be added to the socket
//                   list.
//>
//----------------------------------------------------------------------------
inline void TCPClient::addSocketToList(const TCPClient& theClient)
{
   addSocketToList(theClient.m_socket);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Adds a new socket to the list of sockets.
//
//  \pin  theServer  A TCPServer whose listening socket should be added to the
//                   socket list.
//>
//----------------------------------------------------------------------------
inline void TCPClient::addSocketToList(const TCPServer& theServer)
{
   addSocketToList(theServer.listeningSocket());
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Removes a socket from the list of sockets.
//
//  \pin  socket  The socket that should be removed from the socket list.
//>
//----------------------------------------------------------------------------
inline void TCPClient::removeSocketFromList(SOCKET socket)
{
   // Ignore calls with invalid socket or that can not possibly be set.
   if(socket != INVALID_SOCKET &&
      socket <= m_highestSocketNumber &&
      FD_ISSET(socket, &m_readSet))
   {
      FD_CLR(static_cast<u_int32>(socket), &m_readSet);

      --m_numberOfAddedSockets;
      if(socket == m_highestSocketNumber)
      {
         // Find new highest set file descriptor (socket) number
         m_highestSocketNumber = 0;
         for(int s = m_highestSocketNumber - 1; s >= 0; --s)
         {
            if(FD_ISSET(s, &m_readSet))
            {
               m_highestSocketNumber = s;
               break;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Checks if the socket is readable (requires a previous call to
//  checkReadability).
//
//  \pin  theSocket  The socket to check for readability.
//
//  \return  True if the socket has data available for reading.
//>
//----------------------------------------------------------------------------
inline bool TCPClient::isReadable(SOCKET theSocket) const
{
   return (m_numberOfAddedSockets != 0 && theSocket != INVALID_SOCKET) ? (FD_ISSET(theSocket, &m_readSet) == 0 ? false : true) : 0;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Checks if the TCPClient's socket is readable (requires a previous call to
//  checkReadability).
//
//  \pin  theClient  The TCPClient whose socket to verify for readability.
//
//  \return  True if the socket has data available for reading.
//>
//----------------------------------------------------------------------------
inline bool TCPClient::isReadable(const TCPClient& theClient) const
{
   return isReadable(theClient.m_socket);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Checks if the TCPServer's socket is readable (requires a previous call to
//  checkReadability).
//
//  \pin  theServer  The TCPServer whose socket to verify for readability.
//
//  \return  True if the socket has data available for reading.
//>
//----------------------------------------------------------------------------
inline bool TCPClient::isReadable(const TCPServer& theServer) const
{
   return isReadable(theServer.listeningSocket());
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Verifies whether or not any of the sockets in the socket list has any data
//  available for reading and returns the set of readable sockets.
//
//  \pin  blockTime  The longest time that checkReadability may block for
//                   (default length of timeout is 0)
//
//  \return  The number of sockets that are readable.
//>
//----------------------------------------------------------------------------
inline u_int16 TCPClient::checkReadability(TCPClientServer::Timeout blockTime)
{
   // Check to see if the file descriptor sets tells that the socket could be
   // connected.
   int32 selectReply;
   struct timeval timeout = {blockTime.m_seconds, blockTime.m_microseconds};

#ifndef _WIN32
   selectReply = select(m_highestSocketNumber + 1, &m_readSet, NULL, NULL, &timeout);
#else // _WIN32
   selectReply = select(0 /* ignored input */, &m_readSet, NULL, NULL, &timeout);
#endif // _WIN32

   if(selectReply > 0)
   {
      // The socket has data available for reading.
      return selectReply;
   }
   else if(selectReply == 0)
   {
      // The socket does not have any data available for reading.
      return 0;
   }
   else
   {
      // The call to select caused an Error.
      if(pendingError() == NoError)
      {
         // The socket does not have any data available for reading.
         return 0;
      }
      else
      {
         // At least one socket has an error pending. Lie to the caller and
         // state that all sockets have data available for reading.
         return m_numberOfAddedSockets;
      }
   }
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Returns the socket number.
//
//  \return  The socket number in use by the TCPClient.
//>
//----------------------------------------------------------------------------
inline SOCKET TCPClient::socket() const
{
   return m_socket;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Polls a connected socket for incoming data. The socket is read without
//  checking if the socket is readable (useful with blocking sockets and if
//  you already know that the socket is readable). 
//
//  \pinout  data        Pointer to a buffer to which polled data can be read.
//                       Upon return, the contents of data is valid only when
//                       the size output parameter is non 0.
//
//  \pin     wantedSize  The wanted size in bytes of a received message.
//
//  \pout    polledSize  The number of bytes that actually has been read and
//                       returned pointed to by data.
//
//  \return  Returns TCPClient::OK if the poll was successful. Returns
//           TCPClient::Disconnected if the connection has been terminated.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::pollWithoutSelect(char* data,
                                                      u_int32 wantedSize,
                                                      u_int32& polledSize)
{
   polledSize = wantedSize;

   return pollWithoutSelect(data, polledSize);
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Polls a connected socket for incoming data. The socket is read without
//  checking if the socket is readable (useful with blocking sockets and if
//  you already know that the socket is readable). 
//
//  \pinout  data        Pointer to a buffer to which polled data can be read.
//                       Upon return, the contents of data is valid only when
//                       the size output parameter is non 0.
//
//  \pin     wantedSize  The wanted size in bytes of a received message.
//
//  \pout    polledSize  The number of bytes that actually has been read and
//                       returned pointed to by data.
//
//  \return  Returns TCPClient::OK if the poll was successful. Returns
//           TCPClient::Disconnected if the connection has been terminated.
//>
//----------------------------------------------------------------------------
inline TCPClient::Status TCPClient::pollWithoutSelect(u_char* data,
                                                      u_int32 wantedSize,
                                                      u_int32& polledSize)
{
   polledSize = wantedSize;

   return pollWithoutSelect(reinterpret_cast<char*>(data), polledSize);
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
inline TCPClient::Status TCPClient::pollWithoutSelect(u_char* data,
                                                      u_int32& size)
{
   return pollWithoutSelect(reinterpret_cast<char*>(data), size);
}

#endif // TCPCLIENT_HXX
