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
//  The TCPServer provides non-blocking TCP access routines for passive
//  establishment of TCP connections (accept) as well as transferring
//  ownership of an already connected socket (e.g. transferred to the
//  TCPClient).
//>

#ifndef TCPSERVER_HXX
#define TCPSERVER_HXX

#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif // _MSC_VER

#pragma ident "@(#)filerevision "

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "TCPClientServer.hxx"
#include "Media.hxx"

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

//< \ingroup comm
//
//  This class provides a TCP/IP server. The class supports both blocking and
//  non-blocking TCP/IP connections. The server is not capable of sending or
//  receiving any data.
//  Transfer a connected socket to a TCPClient instance in order to be able
//  to transfer data.
//
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
//     |  TCPServer  |
//     |             |
//     +-------------+
//
//  Inh - Inheritance
//  \endverbatim
//>
class TCPServer : public TCPClientServer
{
public:
   // Create a TCPServer.
   TCPServer(TCPClientServer::Mode = NonBlocking, ErrorReceivingFunction = NULL);

   // Destroy a TCPServer. Stops listening for client connections.
   ~TCPServer();

   // Create a socket, bind it to an IP address and listen for client
   // connections.
   Status listenOn(u_int16, u_int32);

   // Try to perform a non-blocking accept.
   Media::Status accept();

   // Disconnect a connected socket.
   void disconnect();

   // Returns a connected socket.
   SocketInformation connectedSocket();

   // Returns the IP address that is in use by the server when listening for
   // new connections.
   u_int32 listenIPAddress() const;

   // Returns the IP address that the server is connected to.
   u_int32 connectedIPAddress() const;

   // Returns the port number used for the connection.
   u_int16 connectedPortNumber() const;

   // Returns the status of the instance.
   ObjectStatus status() const;

   // Returns the socket number for the listening socket.
   SOCKET listeningSocket() const;

   // Sets the instance in a well-defined operational state.
   void returnToInitialState();

private:
   // Declare but do not implement in order to render the copy ctor unsuable.
   TCPServer(const TCPServer&);

   enum
   {
      // The maximum number of outstanding connection requests.
      NumberOfConnectionRequestsToQueue = SOMAXCONN
   };

   // The socket number on which the server accepts connections.
   SOCKET m_listenSocket;

   // The IP addres of the remote machine that has performed a connect
   // attempt.
   u_int32 m_connectedIPAddress;

   // The port number that the remote machine has used performing a connect
   // attempt.
   u_int16 m_connectedPortNumber;
};

//----------------------------------------------------------------------------
//<
//  Returns the connected socket to be used by a client and sets the state of
//  the server to disconnected in order to be able to wait for further client
//  connections.
//
//  \return  The connected socket along with additional socket information.
//           The returned socket is valid only if a prior call to connect
//           returned true. In any other case an INVALID_SOCKET is returned.
//>
//----------------------------------------------------------------------------
inline TCPClientServer::SocketInformation TCPServer::connectedSocket()
{
   SocketInformation theSocketInformation;

   // Turn the connected flag off in order to be able to accept more
   // connections.
   m_isConnected = false;

   theSocketInformation.m_socket = m_socket;
   theSocketInformation.m_mode = m_Mode;

   m_socket = INVALID_SOCKET;

   // Return the connected socket.
   return theSocketInformation;
}

//----------------------------------------------------------------------------
//<
// Returns the IP address that is in use by the server when listening for
// new connections.
//
//  \return  The IP address that the server is using as the listening IP
//           address.
//>
//----------------------------------------------------------------------------
inline u_int32 TCPServer::listenIPAddress() const
{
   return IPAddress();
}

//----------------------------------------------------------------------------
//<
//  Returns the IP address that the server is connected to.
//
//  \return  The IP address that the server is connected to. If no connection
//           has been established, then 0 is returned. The returned IP address
//           is in host byte order.
//>
//----------------------------------------------------------------------------
inline u_int32 TCPServer::connectedIPAddress() const
{
   return m_isConnected ? m_connectedIPAddress : 0;
}

//----------------------------------------------------------------------------
//<
//  Returns the port number used for the connection.
//
//  \return  The port number which the server used for the connection. If no
//           connection has been established, then 0 is returned. The returned
//           port number is in host byte order.
//>
//----------------------------------------------------------------------------
inline u_int16 TCPServer::connectedPortNumber() const
{
   return m_isConnected ? m_connectedPortNumber : 0;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Returns the socket number for the listening socket.
//
//  \return  The socket number in use by the TCPServer.
//>
//----------------------------------------------------------------------------
inline SOCKET TCPServer::listeningSocket() const
{
   return m_listenSocket;
}

#endif // TCPSERVER_HXX
