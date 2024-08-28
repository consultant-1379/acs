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
//  Responsible ............ UAB/KY/SD Martin Wahlstrom
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
#include "TCPServer.hxx"
#include <cassert>

//----------------------------------------------------------------------------
//<
//  The constructor creates a TCP/IP server.
//
//  \pin  theMode        Whether or not operations should be performed
//                       blocking or non-blocking (non-blocking is default).
//  \pin  errorReceiver  The function that should receive error information
//                       (NULL is default).
//>
//----------------------------------------------------------------------------
TCPServer::TCPServer(TCPClientServer::Mode theMode,
                     ErrorReceivingFunction errorReceiver) :
   TCPClientServer(theMode, errorReceiver),
   m_listenSocket(INVALID_SOCKET),
   m_connectedIPAddress(0),
   m_connectedPortNumber(0)
{
}

//----------------------------------------------------------------------------
//<
//  The destructor disconnects if connected.
//>
//----------------------------------------------------------------------------
TCPServer::~TCPServer()
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
//<
//  The method is used to tell the instance for which port number it should
//  set up a connection with the server bound to a local address and accept
//  connections only from a remote address.
//
//  \pin  thePortNumber     The port number used for the server side (host byte
//                          order).
//  \pin  theLocalAddress   The local IP addres used for the connection (host
//                          byte order).
//
//  \return  TCPServer::OK if all went ok, otherwise one of CreateSocketError,
//           BindError or ListenError within the scope of TCPServer:: is
//           returned.
//>
//----------------------------------------------------------------------------
TCPServer::Status TCPServer::listenOn(u_int16 thePortNumber,
                                      u_int32 theLocalAddress)
{
   assert((m_listenSocket == INVALID_SOCKET) &&
          "You are not allowed to reconfigure the TCPServer once its set up");

   if(INVALID_SOCKET == (m_listenSocket = createSocket()))
   {
      static_cast<void>(socketError(__FILE__, __LINE__));
      return CreateSocketError;
   }
  
   // Allow the socket to be bound to an address that is already in use. 
   int32 allowReuse = 1;
   if(setsockopt(m_listenSocket,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 reinterpret_cast<char*>(&allowReuse), sizeof(allowReuse)) != 0)
   {
      // Unable to change the socket options. Log the error and proceed with
      // the execution.
      static_cast<void>(socketError(__FILE__, __LINE__));
   }

   initializeSocketAddressStructure(theLocalAddress, thePortNumber);

   // Bind the socket to an address
   if(0 > bind(m_listenSocket,
               reinterpret_cast<struct sockaddr*>(&m_socketAddress),
               sizeof(m_socketAddress)))
   {
      //static_cast<void>(bindError(__FILE__, __LINE__));
      return BindError;
   }

   assert((thePortNumber == ntohs(m_socketAddress.sin_port)) &&
          "thePortNumber != ntohs(m_socketAddress.sin_port)");

   // Start the acceptance of connections on this socket
   if(0 > listen(m_listenSocket, NumberOfConnectionRequestsToQueue))
   {
      static_cast<void>(listenError(__FILE__, __LINE__));
      closeTheSocket(m_listenSocket);
      return ListenError;
   }

   if(m_Mode == NonBlocking)
   {
      // Set the listening socket in non-blocking mode.
      if(blockingOff(m_listenSocket) != TCPClientServer::OK)
      {
         closeTheSocket(m_listenSocket);
         return TCPClientServer::CreateSocketError;
      }
   }

   return TCPClientServer::OK;
}

//----------------------------------------------------------------------------
//<
//  Tries (non-blocking) to establish a connection to the client.
//
//  \return  Media::Connected when the connection is successfully
//           established and the required mode has been successfully aplied,
//           otherwise Media::Disconnected.
//>
//----------------------------------------------------------------------------
Media::Status TCPServer::accept()
{
   if(m_isConnected == true)
   {
      return Media::Connected;
   }
   
   struct sockaddr_in clientName;
#if defined __linux
   //    The third argument of accept was originally declared as an
   //    `int *' (and is that under libc4 and  libc5  and  on  many
   //    other systems like BSD 4.*, SunOS 4, SGI); a POSIX 1003.1g
   //    draft standard wanted to change it into a `size_t *',  and
   //    that  is  what it is for SunOS 5.  Later POSIX drafts have
   //    `socklen_t *', and so do the Single Unix Specification and
   //    glibc2.  Quoting Linus Torvalds: _Any_ sane library _must_
   //    have "socklen_t" be the same size as int.   Anything  else
   //    breaks  any BSD socket layer stuff.  POSIX initially _did_
   //    make it a size_t, and I (and hopefully others,  but  obvi­
   //    ously not too many) complained to them very loudly indeed.
   //    Making it a size_t is completely broken,  exactly  because
   //    size_t  very  seldom  is  the same size as "int" on 64-bit
   //    architectures, for example.  And it _has_ to be  the  same
   //    size as "int" because that's what the BSD socket interface
   //    is.  Anyway, the POSIX people eventually got a  clue,  and
   //    created  "socklen_t".   They  shouldn't have touched it in
   //    the first place, but once they did they  felt  it  had  to
   //    have  a  named type for some unfathomable reason (probably
   //    somebody didn't like losing  face  over  having  done  the
   //    original stupid thing, so they silently just renamed their
   //    blunder).
   socklen_t size = sizeof(clientName);
#else // !__linux
   int32 size = sizeof(clientName);
#endif // __linux
   
   // Accept a client connection.
   m_socket = ::accept(m_listenSocket,
                       reinterpret_cast<struct sockaddr*>(&clientName),
                       &size);

   if(m_socket != INVALID_SOCKET)
   {
      // Connections from any host is accepted. It is for the user of the
      // TCPServer to decide whether the connected host is accepted or not.
      // Save information about the connected host.
      m_connectedPortNumber = ntohs(clientName.sin_port);
#ifdef _WIN32
      m_connectedIPAddress = ntohl(clientName.sin_addr.S_un.S_addr);
#elif defined __unix // _WIN32
      m_connectedIPAddress = ntohl(clientName.sin_addr.s_addr);
#endif // _WIN32 or __unix

      // Do not close the listening socket in order to be able to accept
      // further connect requests.

      if(m_Mode == NonBlocking)
      {
         // Set the accepted socket in non-blocking mode.
         if(blockingOff(m_socket) != TCPClientServer::OK)
         {
            disconnect();
            return Media::Disconnected;
         }
      }

      m_isConnected = true;
      return Media::Connected;
   }
   
   return Media::Disconnected;
}

//----------------------------------------------------------------------------
//<
//  Closes the connection to the client.
//>
//----------------------------------------------------------------------------
void TCPServer::disconnect()
{
   closeTheSocket(m_listenSocket);
   closeTheSocket(m_socket);
   m_isConnected = false;
}

//----------------------------------------------------------------------------
//<
//  The status method returns the current status of the instance.
//
//  \return  Returns ObjectOK if the instance can be used safely, otherwise
//           ObjectNotOK is returned.
//>
//----------------------------------------------------------------------------
ObjectStatus TCPServer::status() const
{
   return ObjectOK;
}

//----------------------------------------------------------------------------
//<
//  The returnToInitialState method is used to set the instance in an
//  operational state when an error has occurred.
//>
//----------------------------------------------------------------------------
void TCPServer::returnToInitialState()
{
}
