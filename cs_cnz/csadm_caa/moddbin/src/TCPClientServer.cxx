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
#include "TCPClientServer.hxx"
#include <cstdio> // sprintf, sscanf
#include <cerrno>
#include <cassert>

#ifdef __unix
#include <signal.h>
#endif // __unix

#if !(defined __unix || defined _WIN32)
#error This file does not support your platform.
#endif // !(unix || _WIN32)

//----------------------------------------------------------------------------
//  Class Variable Initializations
//----------------------------------------------------------------------------
// The error database that contains all logged errors.
TCPClientServer::ErrorDatabase TCPClientServer::s_errorDatabase[TCPClientServer::NumberOfErrorMethods];
TCPClientServer::ErrorReceivingFunction TCPClientServer::s_errorReceivingFunction = NULL;

#ifdef _WIN32
u_int32 TCPClientServer::s_noOfInstances = 0;
#elif defined __unix
bool TCPClientServer::s_sigPIPEHandlerIsInstalled = false;
#endif // _WIN32 || __unix

//----------------------------------------------------------------------------
//<
//  Initialises the member variables according to the arguments supplied by
//  the derived class. In a Windows environment it also initializes the socket
//  functionality for the first instance of any derived class.
//
//  \pin  theMode        Whether or not operations should be performed
//                       blocking or non-blocking.
//  \pin  errorReceiver  The function that should receive error information.
//>
//----------------------------------------------------------------------------
TCPClientServer::TCPClientServer(TCPClientServer::Mode theMode,
                                 ErrorReceivingFunction errorReceiver) :
   m_socket(INVALID_SOCKET),
   m_isConnected(false),
   m_status(ObjectOK),
   m_Mode(theMode)
{
   // Set up the error receiving function.
   errorReceivingFunction(errorReceiver);

   assert(s_errorReceivingFunction != NULL && "Assign an error receiving function before creating an instance of TCPClientServer");

   // It does not matter whether we clear the client or the server member
   // since they share the space.
   memset(&m_socketAddress, 0, sizeof(m_socketAddress));

#ifdef _WIN32
   if(++s_noOfInstances == 1)
   {
      // Ask for Winsock version 2.2.
      WORD versionRequested = MAKEWORD(2, 2);
      WSADATA wsaData;
      if(WSAStartup(versionRequested, &wsaData) != 0)
      {
         m_status = ObjectNotOK;
         s_errorReceivingFunction(SocketLevelError,
                                  __FILE__,
                                  __LINE__,
                                  "Error, no usable Winsock.dll for Winsock version 2.2 could be found.",
                                  0, 0, 0, 0);
      }
   }
#elif defined __unix
   if(s_sigPIPEHandlerIsInstalled == false)
   {
      // Install a signal handler for PIPE (ignore the signal).
      if(signal(SIGPIPE, TCPClientServer::sigPIPEHandler) != SIG_ERR)
      {
         s_sigPIPEHandlerIsInstalled = true;
      }
      else
      {
         // It was not possible to install the signal handler.
         char errorMessage[1024];

         sprintf(errorMessage,
                 "Unable to install a signal handler for SIGPIPE, error is %d",
                 errno);

         s_errorReceivingFunction(InstallSignalHandlerFailure,
                                  __FILE__,
                                  __LINE__,
                                  errorMessage,
                                  0, 0, 0, 0);
      }
   }
#endif // _WIN32 || __unix
}

//----------------------------------------------------------------------------
//<
//  Cleans up the socket initialization if in a Windows environment when the
//  last instance is destroyed. Otherwise does nothing.
//>
//----------------------------------------------------------------------------
TCPClientServer::~TCPClientServer()
{
#ifdef _WIN32
   if(--s_noOfInstances == 0)
   {
      static_cast<void>(WSACleanup());
   }
#endif // _WIN32
}

//----------------------------------------------------------------------------
//<
//  Tells the socket's buffer size.
//
//  \pin  name  Either SendBuffer or ReceiveBuffer.
//
//  \return  The socket's buffer size in bytes for the requested buffer,
//           0 indicates an error.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::socketBufferInformation(TCPClientServer::BufferName name) const
{
   assert((name == SendBuffer || name == ReceiveBuffer) && "Invalid name passed to TCPClientServer::socketBufferInformation");

   if(m_socket != INVALID_SOCKET)
   {
      int32 getsockoptReply;

      int32 bufferSize;
#if defined __linux
      socklen_t length = sizeof(bufferSize);
#else // !__linux
      int32 length = sizeof(bufferSize);
#endif // __linux
      char* buffer = reinterpret_cast<char*>(&bufferSize);

      getsockoptReply = getsockopt(m_socket, SOL_SOCKET, name, buffer, &length);

      return getsockoptReply == 0 ? bufferSize : 0;
   }
   else
   {
      //assert(!"Coding error: Do not try to find out the buffer size of a not valid socket");

      // No valid socket has been created yet.
      return 0;
   }
}

//----------------------------------------------------------------------------
//<
//  Sets the socket's buffer size.
//
//  \pin  name  Either SendBuffer or ReceiveBuffer.
//  \pin  size  Wanted buffer size.
//
//  \return  OK if setting the new buffer size was successful, otherwise NotOK
//           is returned.
//>
//----------------------------------------------------------------------------
TCPClientServer::Status TCPClientServer::socketBufferInformation(TCPClientServer::BufferName name,
                                                                 int32 size) const
{
   assert((name == SendBuffer || name == ReceiveBuffer) && "Invalid name passed to TCPClientServer::socketBufferInformation");

   if(m_socket != INVALID_SOCKET)
   {
      int32 getsockoptReply;

      int32 length = sizeof(size);
      char* buffer = reinterpret_cast<char*>(&size);

      getsockoptReply = setsockopt(m_socket, SOL_SOCKET, name, buffer, length);

      if(getsockoptReply == 0)
      {
         return OK;
      }
      else
      {
         char message[256];
         
         sprintf(message,
                 "Unable to change the socket's %s buffer size to %d bytes",
                 (name == SendBuffer ? "send" : "receive"),
                 size);

         s_errorReceivingFunction(SocketLevelError,
                                  __FILE__,
                                  __LINE__,
                                  message,
                                  name,
                                  IPAddress() >> 16,
                                  IPAddress() & 0xFFFF,
                                  portNumber());
         return NotOK;
      }
   }
   else
   {
      //assert(!"Coding error: Do not try to change the buffer size of a not valid socket");

      // No valid socket has been created yet.
      return NotOK;
   }
}

//----------------------------------------------------------------------------
//<
//  Converts an IP address (numeric format) to a string.
//
//  \pin  theIPAddress  The numerical representation of the IP address to
//                      compute a string representation from.
//
//  \return  A string representation of the IP address, eg and IP address of
//           0xc0a88101 will return a string on the format "192.168.129.1".
//>
//----------------------------------------------------------------------------
char* TCPClientServer::IPAddressToString(u_int32 theIPAddress)
{
   // 16 characters is sufficient to hold xxx.xxx.xxx.xxx including the NULL
   // termination character.
   static char IPAddressString[16];

   sprintf(IPAddressString, "%u.%u.%u.%u",
           (theIPAddress >> 24) & 0xFF,
           (theIPAddress >> 16) & 0xFF,
           (theIPAddress >>  8) & 0xFF,
           theIPAddress & 0xFF);

   return IPAddressString;
}

//----------------------------------------------------------------------------
//<
//  Converts an IP address (string format) to a numerical value.
//
//  \pin  theIPAddress  The string representation of the IP address to compute
//                      a numerical representation from.
//
//  \return  A numerical representation of the IP address, eg and IP address
//           of "192.168.129.1" will return a numerical value of 0xc0a88101.
//>
//----------------------------------------------------------------------------
u_int32 TCPClientServer::IPAddressToDecimal(const char* theIPAddress)
{
   // The address is computed from a.b.c.d
   u_int32 a;
   u_int32 b;
   u_int32 c;
   u_int32 d;

   int returnValue = sscanf(theIPAddress, "%u.%u.%u.%u", &a, &b, &c, &d);

   if (returnValue == 4)
	return (a << 24) | (b << 16) | (c << 8) | d;
   else
    return 0;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Translates a machine name into an IP address.
//
//  \pin  machineName  The name of the machine for which an IP address should
//                     be found.
//
//  \return  Returns the IP address for the given machine name if found. If
//           the IP address cannot be found, then 0 is returned.
//>
//----------------------------------------------------------------------------
u_int32 TCPClientServer::getIPAddressByName(const char* machineName)
{
   hostent* machineInformation;

   if(machineName == NULL)
   {
      return 0;
   }

#ifdef _WIN32
   if(s_noOfInstances == 0)
   {
      // Ask for version 2.2 of the Windows sockets.
      WORD versionRequested = MAKEWORD(2, 2);
      WSADATA wsaData;
      
      if(WSAStartup(versionRequested, &wsaData) != 0)
      {
         return 0;
      }
   }
#endif // _WIN32

   machineInformation = gethostbyname(machineName);
   
#ifdef _WIN32
   if(s_noOfInstances == 0)
   {
      static_cast<void>(WSACleanup());
   }
#endif // _WIN32

   if(machineInformation == NULL)
   {
      APZcout << "getHostByName(" << machineName << ") failed" << endl;
      return 0;
   }
   else
   {
      return ntohl(*(reinterpret_cast<u_int32*>(machineInformation->h_addr)));
   }
}

//----------------------------------------------------------------------------
//<
//  Looks up the host name for the local machine.
//
//  \pin  hostName  Specifies the address of an array of bytes where the host
//                  name is stored.
//  \pin  size      Specifies the length of the array pointed to by the name
//                  parameter.
//
//  \pout hostName  The hostname is valid only if the size is non zero.
//  \pout size      The length of the hostname. If the host name cannot be
//                  found, then size is -1.
//>
//----------------------------------------------------------------------------
void TCPClientServer::getHostname(const char* hostName, u_int32& size)
{
#ifdef _WIN32
   if(s_noOfInstances == 0)
   {
      // Ask for version 2.2 of the Windows sockets.
      WORD versionRequested = MAKEWORD(2, 2);
      WSADATA wsaData;
      
      if(WSAStartup(versionRequested, &wsaData) != 0)
      {
         size = 0xFFFFFFFF;
         return;
      }
   }
#endif // _WIN32

   static_cast<void>(gethostname(const_cast<char*>(hostName), size));

#ifdef _WIN32
   if(s_noOfInstances == 0)
   {
      static_cast<void>(WSACleanup());
   }
#endif // _WIN32
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::socketError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Socket, errorCode) && !isUniqueError(Socket, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The process does not have appropriate privileges.
   case EACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The process does not have appropriate privileges.", 0, 0, 0, 0);
      break;

   // The addresses in the specified address family are not available in the kernel.
   case EAFNOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The addresses in the specified address family are not available in the kernel.", 0, 0, 0, 0);
      break;

   // The per-process descriptor table is full.
   case EMFILE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The per-process descriptor table is full.", 0, 0, 0, 0);
      break;

   // No more file descriptors are available for the system.
   case ENFILE:
      s_errorReceivingFunction(SocketLevelError, file, line, "No more file descriptors are available for the system.", 0, 0, 0, 0);
      break;

   // Insufficient resources were available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources were available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The system was unable to allocate kernel memory to increase the process descriptor table.
   case ENOMEM:
      s_errorReceivingFunction(SocketLevelError, file, line, "The system was unable to allocate kernel memory to increase the process descriptor table.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // The process is attempting to open a raw socket and does not have superuser privilege.
   case EPERM:
      s_errorReceivingFunction(SocketLevelError, file, line, "The process is attempting to open a raw socket and does not have superuser privilege.", 0, 0, 0, 0);
      break;

   // The socket in the specified address family is not supported.
   case EPROTONOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket in the specified address family is not supported.", 0, 0, 0, 0);
      break;

   // The socket type is not supported by the protocol.
   case EPROTOTYPE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket type is not supported by the protocol.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown socket error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Socket, errorCode) && !isUniqueError(Socket, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // The specified address family is not supported. 
   case WSAEAFNOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The specified address family is not supported. ", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The queue is nonempty upon entry to accept and there are no descriptors available.
   case WSAEMFILE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The queue is nonempty upon entry to accept and there are no descriptors available.", 0, 0, 0, 0);
      break;

   // No buffer space is available.
   case WSAENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "No buffer space is available.", 0, 0, 0, 0);
      break;

   // The specified protocol is not supported.
   case WSAEPROTONOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The specified protocol is not supported.", 0, 0, 0, 0);
      break;

   // The specified protocol is the wrong type for this socket.
   case WSAEPROTOTYPE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The specified protocol is the wrong type for this socket.", 0, 0, 0, 0);
      break;

   // The specified socket type is not supported in this address family.
   case WSAESOCKTNOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The specified socket type is not supported in this address family.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown socket error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::connectError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Connect, errorCode) && !isUniqueError(Connect, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // Search permission is denied for a component of the path prefix; or write access to the named socket is denied.
   case EACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "Search permission is denied for a component of the path prefix; or write access to the named socket is denied.", 0, 0, 0, 0);
      break;

   // The specified address is already in use.
   case EADDRINUSE:
      // This error should be handled by the user.
      break;

   // The specified address is not available from the local machine.
   case EADDRNOTAVAIL:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "The specified address is not available from the local machine (%s).", IPAddressToString(IPAddress()));

         s_errorReceivingFunction(NetworkLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
      break;

   // The addresses in the specified address family cannot be used with this socket.
   case EAFNOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The addresses in the specified address family cannot be used with this socket.", 0, 0, 0, 0);
      break;

   // A connection request is already in progress for the specified socket.
   case EALREADY:
      // This is not an error, it is only information stating that the
      // connect request is beeing processed.
      break;

   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // The attempt to connect was rejected.
   case ECONNREFUSED:
      // This is not a fault really, it is the result of a call to connect
      // when there is not yet a server that has called accept.
      break;

   // The remote host reset the connection request.
   case ECONNRESET:
      s_errorReceivingFunction(SocketLevelError, file, line, "The remote host reset the connection request.", 0, 0, 0, 0);
      errorCode = Disconnected;
      break;

   // The address parameter is not in a readable part of the useraddress space.
   case EFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The address parameter is not in a readable part of the useraddress space.", 0, 0, 0, 0);
      break;

   // The specified host is not reachable.
   case EHOSTUNREACH:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage,
                 "The specified host is not reachable.\nAddress to connect to: %s, port number: %d, socket: %d",
                 IPAddressToString(ntohl(m_socketAddress.sin_addr.s_addr)),
                 ntohs(m_socketAddress.sin_port),
                 m_socket);

         s_errorReceivingFunction(NetworkLevelError, file, line, errorMessage, 0, 0, 0, 0);

         errorCode = Disconnected;
      }
      break;

   // O_NONBLOCK is set for the file descriptor for the socket and the connection cannot be immediately established; the connection will be established asynchronously.
   case EINPROGRESS:
      // This is not an error, it is only information stating that the
      // connection may complete later.
      break;

   // The connect() function was interrupted by a signal while waiting for the connection to be established. The connection establishment may continue asynchronously.
   case EINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The connect() function was interrupted by a signal while waiting for the connection to be established. The connection establishment may continue asynchronously.", 0, 0, 0, 0);
      break;

   // The value of the address_len parameter is invalid for the specified address family; or the sa_family field in the socket address structure is invalid for the protocol.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The value of the address_len parameter is invalid for the specified address family; or the sa_family field in the socket address structure is invalid for the protocol.", 0, 0, 0, 0);
      break;

   // An I/O error occurred while reading from or writing to the file system.
   case EIO:
      s_errorReceivingFunction(SocketLevelError, file, line, "An I/O error occurred while reading from or writing to the file system.", 0, 0, 0, 0);
      break;

   // The socket is already connected.
   case EISCONN:
      errorCode = AlreadyConnected;
      break;

   // Too many symbolic links were encountered in translating the pathname in address.
   case ELOOP:
      s_errorReceivingFunction(SocketLevelError, file, line, "Too many symbolic links were encountered in translating the pathname in address.", 0, 0, 0, 0);
      break;

   // A component of the pathname exceeded NAME_MAX characters, or an entire pathname exceeded PATH_MAX characters.
   case ENAMETOOLONG:
      s_errorReceivingFunction(SocketLevelError, file, line, "A component of the pathname exceeded NAME_MAX characters, or an entire pathname exceeded PATH_MAX characters.", 0, 0, 0, 0);
      break;

   // The local network connection is not operational.
   case ENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The local network connection is not operational.", 0, 0, 0, 0);
      break;

   // No route to the network or host is present.
   case ENETUNREACH:
      s_errorReceivingFunction(NetworkLevelError, file, line, "No route to the network or host is present.", 0, 0, 0, 0);
      break;

   // Insufficient resources are available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // A component of the pathname does not name an existing file or the pathname is an empty string.
   case ENOENT:
      s_errorReceivingFunction(SocketLevelError, file, line, "A component of the pathname does not name an existing file or the pathname is an empty string.", 0, 0, 0, 0);
      break;

   // A component of the path prefix of the pathname in address is not a directory.
   case ENOTDIR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A component of the path prefix of the pathname in address is not a directory.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The socket is listening and cannot be connected.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is listening and cannot be connected.", 0, 0, 0, 0);
      break;
            
   // The specified address has a different type than the socket bound to the specified peer address.
   case EPROTOTYPE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The specified address has a different type than the socket bound to the specified peer address.", 0, 0, 0, 0);
      break;

   // The establishment of a connection timed out before a connection was made.
   case ETIMEDOUT:
      // This is not an error, it is only information stating that the
      // connection may complete later.
      break;

   // The socket is marked nonblocking, so the connection cannot be immediately completed. The application program can select the socket for writing during the connection process.
   case EWOULDBLOCK:
      // This is not an error, it is only information stating that the
      // connection may complete later.
      break;
   
   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown connect error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Connect, errorCode) && !isUniqueError(Connect, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function.
   case WSAEADDRINUSE:
      // This error should be handled by the user.
      break;

   // The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.
   case WSAEINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // A nonblocking connect call is in progress on the specified socket. Note In order to preserve backward compatibility, this error is reported as WSAEINVAL to Windows Sockets 1.1 applications that link to either WINSOCK.DLL or WSOCK32.DLL.
   case WSAEALREADY:
      // This is not an error, it is only information stating that the
      // connect request is beeing processed.
      break;

   // The remote address is not a valid address (such as ADDR_ANY).
   case WSAEADDRNOTAVAIL:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The remote address is not a valid address (such as ADDR_ANY).", 0, 0, 0, 0);
      break;

   // Addresses in the specified family cannot be used with this socket.
   case WSAEAFNOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "Addresses in the specified family cannot be used with this socket.", 0, 0, 0, 0);
      break;

   // The attempt to connect was forcefully rejected.
   case WSAECONNREFUSED:
      // This is not a fault really, it is the result of a call to connect
      // when there is not yet a server that has called accept.
      break;

   // The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.", 0, 0, 0, 0);
      break;

   // The parameter s is a listening socket, or the destination address specified is not consistent with that of the constrained group the socket belongs to.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The parameter s is a listening socket, or the destination address specified is not consistent with that of the constrained group the socket belongs to.", 0, 0, 0, 0);
      break;

   // The socket is already connected (connection-oriented sockets only).
   case WSAEISCONN:
      errorCode = AlreadyConnected;
      break;

   // The network cannot be reached from this host at this time.
   case WSAENETUNREACH:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage,
                 "The network cannot be reached from this host at this time.\nAddress to connect to: %s, port number: %d, socket: %d",
                 IPAddressToString(ntohl(m_socketAddress.sin_addr.s_addr)),
                 ntohs(m_socketAddress.sin_port),
                 m_socket);

         s_errorReceivingFunction(NetworkLevelError, file, line, errorMessage, 0, 0, 0, 0);

         errorCode = Disconnected;
      }
      break;

   // No buffer space is available. The socket cannot be connected.
   case WSAENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "No buffer space is available. The socket cannot be connected.", 0, 0, 0, 0);
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // Attempt to connect timed out without establishing a connection.
   case WSAETIMEDOUT:
      // This is not an error, it is only information stating that the
      // connection may complete later.
      break;

   // The socket is marked as nonblocking and the connection cannot be completed immediately.
   case WSAEWOULDBLOCK:
      // This is not an error, it is only information stating that the
      // connection may complete later.
      break;

   // Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.
   case WSAEACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown connect error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::bindError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Bind, errorCode) && !isUniqueError(Bind, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The requested address is protected and the current user does not have permission to access it.
   case EACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The requested address is protected and the current user does not have permission to access it.", 0, 0, 0, 0);
      break;

   // The specified address is already in use.
   case EADDRINUSE:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "The specified address is already in use (%s).", IPAddressToString(IPAddress()));

         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
      break;

   // The specified address is not available from the local machine.
   case EADDRNOTAVAIL:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "The specified address is not available from the local machine (%s)", IPAddressToString(IPAddress()));

         s_errorReceivingFunction(NetworkLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
      break;

   // The specified address is protected and the current user does not support binding to a new address; or the socket has been shut down.
   case EAFNOSUPPORT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The specified address is protected and the current user does not support binding to a new address; or the socket has been shut down.", 0, 0, 0, 0);
      break;

   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // The address argument is a null pointer.
   case EDESTADDRREQ:
      s_errorReceivingFunction(SocketLevelError, file, line, "The address argument is a null pointer.", 0, 0, 0, 0);
      break;

   // The address parameter is not in a readable part of the user address space.
   case EFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The address parameter is not in a readable part of the user address space.", 0, 0, 0, 0);
      break;

   // The socket is already bound to an address.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is already bound to an address.", 0, 0, 0, 0);
      break;

   // An I/O error occurred.
   case EIO:
      s_errorReceivingFunction(SocketLevelError, file, line, "An I/O error occurred.", 0, 0, 0, 0);
      break;

   // The socket is already connected.
   case EISCONN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is already connected.", 0, 0, 0, 0);
      break;

   // The address argument is a null pointer.
   case EISDIR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The address argument is a null pointer.", 0, 0, 0, 0);
      break;

   // Too many symbolic links were encountered in translating the pathname in address.
   case ELOOP:
      s_errorReceivingFunction(SocketLevelError, file, line, "Too many symbolic links were encountered in translating the pathname in address.", 0, 0, 0, 0);
      break;

   // A component of the pathname exceeded NAME_MAX characters, or an entire pathname exceeded PATH_MAX characters.
   case ENAMETOOLONG:
      s_errorReceivingFunction(SocketLevelError, file, line, "A component of the pathname exceeded NAME_MAX characters, or an entire pathname exceeded PATH_MAX characters.", 0, 0, 0, 0);
      break;

   // Insufficient resources are available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // A component of the pathname does not name an existing file or the pathname is an empty string.
   case ENOENT:
      s_errorReceivingFunction(SocketLevelError, file, line, "A component of the pathname does not name an existing file or the pathname is an empty string.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // A component of the path prefix of the pathname in address is not a directory.
   case ENOTDIR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A component of the path prefix of the pathname in address is not a directory.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The socket type of the specified socket does not support binding to an address.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket type of the specified socket does not support binding to an address.", 0, 0, 0, 0);
      break;

   // The name would reside on a read-only filesystem.
   case EROFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "The name would reside on a read-only filesystem.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown bind error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Bind, errorCode) && !isUniqueError(Bind, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The requested address is protected and the current user does not have permission to access it.
   case WSAEACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The requested address is protected and the current user does not have permission to access it.", 0, 0, 0, 0);
      break;

   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // A process on the machine is already bound to the same fully-qualified address and the socket has not been marked to allow address re-use with SO_REUSEADDR. For example, IP address and port are bound in the af_inet case) . (See the SO_REUSEADDR socket option under setsockopt.)
   case WSAEADDRINUSE:
      s_errorReceivingFunction(SocketLevelError, file, line, "A process on the machine is already bound to the same fully-qualified address and the socket has not been marked to allow address re-use with SO_REUSEADDR. For example, IP address and port are bound in the af_inet case) . (See the SO_REUSEADDR socket option under setsockopt.)", 0, 0, 0, 0);
      break;

   // The specified address is not a valid address for this machine
   case WSAEADDRNOTAVAIL:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The specified address is not a valid address for this machine", 0, 0, 0, 0);
      break;

   // The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The socket is already bound to an address.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is already bound to an address.", 0, 0, 0, 0);
      break;

   // Not enough buffers available, too many connections.
   case WSAENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Not enough buffers available, too many connections.", 0, 0, 0, 0);
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown bind error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::listenError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Listen, errorCode) && !isUniqueError(Listen, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The requested address is protected and the current process does not have permission to access it.
   case EACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The requested address is protected and the current process does not have permission to access it.", 0, 0, 0, 0);
      break;

   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // The socket is not bound to a local address, and the protocol does not support listening on an unbound socket.
   case EDESTADDRREQ:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is not bound to a local address, and the protocol does not support listening on an unbound socket.", 0, 0, 0, 0);
      break;

   // The socket is already connected.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is already connected.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The referenced socket is not a type that supports the listen() function.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The referenced socket is not a type that supports the listen() function.", 0, 0, 0, 0);
      break;

   // Insufficient resources were available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources were available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown listen error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Listen, errorCode) && !isUniqueError(Listen, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be "committed" at the time of this function.
   case WSAEADDRINUSE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be \"committed\" at the time of this function.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The socket has not been bound with bind.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket has not been bound with bind.", 0, 0, 0, 0);
      break;

   // The socket is already connected.
   case WSAEISCONN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is already connected.", 0, 0, 0, 0);
      break;

   // No more socket descriptors are available.
   case WSAEMFILE:
      s_errorReceivingFunction(SocketLevelError, file, line, "No more socket descriptors are available.", 0, 0, 0, 0);
      break;

   // No buffer space is available.
   case WSAENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "No buffer space is available.", 0, 0, 0, 0);
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // The referenced socket is not of a type that supports the listen operation.
   case WSAEOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The referenced socket is not of a type that supports the listen operation.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown listen error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::acceptError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined (__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Accept, errorCode) && !isUniqueError(Accept, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // A connection has been aborted.
   case ECONNABORTED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A connection has been aborted.", 0, 0, 0, 0);
      break;

   // The address parameter is not in a writable part of the user address space.
   case EFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The address parameter is not in a writable part of the user address space.", 0, 0, 0, 0);
      break;

   // The accept() function was interrupted by a signal that was caught before a valid connection arrived.
   case EINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The accept() function was interrupted by a signal that was caught before a valid connection arrived.", 0, 0, 0, 0);
      break;

   // The socket is not accepting connections.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is not accepting connections.", 0, 0, 0, 0);
      break;

   // There are too many open file descriptors.
   case EMFILE:
      s_errorReceivingFunction(SocketLevelError, file, line, "There are too many open file descriptors.", 0, 0, 0, 0);
      break;

   // The maximum number of file descriptors in the system are already open.
   case ENFILE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The maximum number of file descriptors in the system are already open.", 0, 0, 0, 0);
      break;

   // Insufficient resources are available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The system was unable to allocate kernel memory to increase the process descriptor table.
   case ENOMEM:
      s_errorReceivingFunction(SocketLevelError, file, line, "The system was unable to allocate kernel memory to increase the process descriptor table.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The referenced socket can not accept connections.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The referenced socket can not accept connections.", 0, 0, 0, 0);
      break;

   // A protocol error occurred.
   case EPROTO:
      s_errorReceivingFunction(SocketLevelError, file, line, "A protocol error occurred.", 0, 0, 0, 0);
      break;

   // The socket is marked nonblocking, and no connections are present to be accepted.
   case EWOULDBLOCK:
      // This is not a fault really, it is the result of a call to accept when
      // there is not yet a client that has called connect.
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown accept error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Accept, errorCode) && !isUniqueError(Accept, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // The addrlen parameter is too small or addr is not a valid part of the user address space.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The addrlen parameter is too small or addr is not a valid part of the user address space.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.
   case WSAEINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The listen function was not invoked prior to accept.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The listen function was not invoked prior to accept.", 0, 0, 0, 0);
      break;

   // The queue is nonempty upon entry to accept and there are no descriptors available.
   case WSAEMFILE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The queue is nonempty upon entry to accept and there are no descriptors available.", 0, 0, 0, 0);
      break;

   // No buffer space is available.
   case WSAENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "No buffer space is available.", 0, 0, 0, 0);
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // The referenced socket is not a type that supports connection-oriented service.
   case WSAEOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The referenced socket is not a type that supports connection-oriented service.", 0, 0, 0, 0);
      break;

   // The socket is marked as nonblocking and no connections are present to be accepted.
   case WSAEWOULDBLOCK:
      // This is not a fault really, it is the result of a call to accept when
      // there is not yet a client that has called connect.
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown accept error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::getsockoptError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Getsockopt, errorCode) && !isUniqueError(Getsockopt, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The calling process does not have appropriate permissions.
   case EACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The calling process does not have appropriate permissions.", 0, 0, 0, 0);
      break;

   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // The send and receive timeout values are too large to fit in the timeout fields of the socket structure.
   case EDOM:
      s_errorReceivingFunction(SocketLevelError, file, line, "The send and receive timeout values are too large to fit in the timeout fields of the socket structure.", 0, 0, 0, 0);
      break;

   // The address pointed to by the option_value parameter is not in a valid (writable) part of the process space, or the option_len parameter is not in a valid part of the process address space.
   case EFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The address pointed to by the option_value parameter is not in a valid (writable) part of the process space, or the option_len parameter is not in a valid part of the process address space.", 0, 0, 0, 0);
      break;

   // The option_value or option_len parameter is invalid.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The option_value or option_len parameter is invalid.", 0, 0, 0, 0);
      break;

   // Insufficient resources are available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The option is unknown.
   case ENOPROTOOPT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The option is unknown.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The operation is not supported by the socket protocol.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The operation is not supported by the socket protocol.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown getsockopt error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Getsockopt, errorCode) && !isUniqueError(Getsockopt, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // One of the optval or the optlen parameters is not a valid part of the user address space, or the optlen parameter is too small.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "One of the optval or the optlen parameters is not a valid part of the user address space, or the optlen parameter is too small.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The level parameter is unknown or invalid.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The level parameter is unknown or invalid.", 0, 0, 0, 0);
      break;

   // No buffer space is available.
   case WSAENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "No buffer space is available.", 0, 0, 0, 0);
      break;

   // The option is unknown or unsupported by the indicated protocol family.
   case WSAENOPROTOOPT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The option is unknown or unsupported by the indicated protocol family.", 0, 0, 0, 0);
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown getsockopt error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::selectError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Select, errorCode) && !isUniqueError(Select, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // One or more of the I/O descriptor sets specified an invalid file descriptor.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "One or more of the I/O descriptor sets specified an invalid file descriptor.", 0, 0, 0, 0);
      break;

   // A signal was delivered before the time limit specified by the timeout parameter expired and before any of the selected events occurred.
   case EINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A signal was delivered before the time limit specified by the timeout parameter expired and before any of the selected events occurred.", 0, 0, 0, 0);
      break;

   // The time limit specified by the timeout parameter is invalid. The nfds parameter is less than 0, or greater than or equal to FD_SETSIZE. One of the specified file descriptors refers to a STREAM or multiplexer that is linked (directly or indirectly) downstream from a multiplexer.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The time limit specified by the timeout parameter is invalid. The nfds parameter is less than 0, or greater than or equal to FD_SETSIZE. One of the specified file descriptors refers to a STREAM or multiplexer that is linked (directly or indirectly) downstream from a multiplexer.", 0, 0, 0, 0);
      break;

   //  Allocation of internal data structures failed. A later call to the select() function may complete successfully.
   case EAGAIN:
      s_errorReceivingFunction(SocketLevelError, file, line, "Allocation of internal data structures failed. A later call to the select() function may complete successfully.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown select error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Select, errorCode) && !isUniqueError(Select, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // The timeout value is not valid, or all three descriptor parameters were NULL.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The timeout value is not valid, or all three descriptor parameters were NULL.", 0, 0, 0, 0);
      break;

   // A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.
   case WSAEINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // One of the descriptor sets contains an entry that is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "One of the descriptor sets contains an entry that is not a socket.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown select error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault. If the connection is already
//           disconnected, then Disconnected is returned.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::shutdownError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Shutdown, errorCode) && !isUniqueError(Shutdown, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // One or more of the I/O descriptor sets specified an invalid file descriptor.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "One or more of the I/O descriptor sets specified an invalid file descriptor.", 0, 0, 0, 0);
      break;

   // The second parameter is not valid, or is not consistent with the socket type.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The second parameter is not valid, or is not consistent with the socket type.", 0, 0, 0, 0);
      break;

   // Insufficient resources are available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // The socket is not connected.
   case ENOTCONN:
      // This is not an error, really. It may be the result of the remote host
      // disconnecting just prior to our call to shutdown.
      errorCode = Disconnected;
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;
      
   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown shutdown error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Shutdown, errorCode) && !isUniqueError(Shutdown, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // The second parameter is not valid, or is not consistent with the socket type. For example, SD_SEND is used with a UNI_RECV socket type.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The second parameter is not valid, or is not consistent with the socket type. For example, SD_SEND is used with a UNI_RECV socket type.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The socket is not connected (connection-oriented sockets only).
   case WSAENOTCONN:
      // This is not an error, really. It may be the result of the remote host
      // disconnecting just prior to our call to shutdown.
      errorCode = Disconnected;
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown shutdown error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::sendError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Send, errorCode) && !isUniqueError(Send, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The calling proces does not have the appropriate privileges.
   case EACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The calling proces does not have the appropriate privileges.", 0, 0, 0, 0);
      break;

   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // A connection was forcibly closed by a peer.
   case ECONNRESET:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The socket is not connection-oriented and no peer address is set.
   case EDESTADDRREQ:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is not connection-oriented and no peer address is set.", 0, 0, 0, 0);
      break;

   // The message parameter is not in a readable or writable part of the user address space.
   case EFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The message parameter is not in a readable or writable part of the user address space.", 0, 0, 0, 0);
      break;

   // A signal interrupted send before any data was transmitted.
   case EINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A signal interrupted send before any data was transmitted.", 0, 0, 0, 0);
      break;

   // An I/O error occurred while reading from or writing to the file system.
   case EIO:
      s_errorReceivingFunction(NetworkLevelError, file, line, "An I/O error occurred while reading from or writing to the file system.", 0, 0, 0, 0);
      break;

   // The message is too large to be sent all at once, as the socket requires.
   case EMSGSIZE:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The message is too large to be sent all at once, as the socket requires.", 0, 0, 0, 0);
      break;

   // The local network connection is not operational.
   case ENETDOWN:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The local network connection is not operational.", 0, 0, 0, 0);
      break;

   // The destination network is unreachable.
   case ENETUNREACH:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The destination network is unreachable.", 0, 0, 0, 0);
      break;

   // Insufficient resources are available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(NetworkLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // The socket is not connected or otherwise has not had the peer prespecified.
   case ENOTCONN:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The socket is not connected or otherwise has not had the peer prespecified.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The socket argument is associated with a socket that does not support one or more of the values set in flags.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket argument is associated with a socket that does not support one or more of the values set in flags.", 0, 0, 0, 0);
      break;

   // The socket is shut down for writing, or the socket is connection-oriented and the peer is closed or shut down for reading. In the latter case, and if the socket is of type SOCK_STREAM, the SIGPIPE signal is generated to the calling process.
   case EPIPE:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The socket is marked nonblocking, and no space is available for the send() function.
   case EWOULDBLOCK:
      // This is not a fault really, it is merely information.
      break;
      
   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown send error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Send, errorCode) && !isUniqueError(Send, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address.
   case WSAEACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.
   case WSAEINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The buf parameter is not completely contained in a valid part of the user address space.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The buf parameter is not completely contained in a valid part of the user address space.", 0, 0, 0, 0);
      break;

   // The connection has been broken due to the "keep-alive" activity detecting a failure while the operation was in progress.
   case WSAENETRESET:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // No buffer space is available.
   case WSAENOBUFS:
      s_errorReceivingFunction(NetworkLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The socket is not connected.
   case WSAENOTCONN:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The socket is not connected.", 0, 0, 0, 0);
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations.
   case WSAEOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations.", 0, 0, 0, 0);
      break;

   // The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.
   case WSAESHUTDOWN:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The socket is marked as nonblocking and the requested operation would block.
   case WSAEWOULDBLOCK:
      // This is not a fault really, it is merely information.
      break;

   // The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.
   case WSAEMSGSIZE:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.", 0, 0, 0, 0);
      break;

   // The remote host cannot be reached from this host at this time.
   case WSAEHOSTUNREACH:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The remote host cannot be reached from this host at this time.", 0, 0, 0, 0);
      break;

   // The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.", 0, 0, 0, 0);
      break;

   // The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.
   case WSAECONNABORTED:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The virtual circuit was reset by the remote side executing a "hard" or "abortive" close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a "Port Unreachable" ICMP packet. The application should close the socket as it is no longer usable.
   case WSAECONNRESET:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The connection has been dropped, because of a network failure or because the system on the other end went down without notice.
   case WSAETIMEDOUT:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The connection has been dropped, because of a network failure or because the system on the other end went down without notice.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown send error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::recvError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Recv, errorCode) && !isUniqueError(Recv, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The read() would cause the information label of the process to float to an information level that is not dominated by the process's sensitivity level.
   case EACCES:
      s_errorReceivingFunction(SocketLevelError, file, line, "The read() would cause the information label of the process to float to an information level that is not dominated by the process's sensitivity level.", 0, 0, 0, 0);
      break;

   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // A connection was forcibly closed by a peer.
   case ECONNRESET:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The data was directed to be received into a nonexistent or protected part of the process address space. The buffer parameter is invalid.
   case EFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The data was directed to be received into a nonexistent or protected part of the process address space. The buffer parameter is invalid.", 0, 0, 0, 0);
      break;

   // A signal interrupted the recv() function before any data was available.
   case EINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "A signal interrupted the recv() function before any data was available.", 0, 0, 0, 0);
      break;

   // The MSG_OOB flag is set and no out-of-band data is available.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The MSG_OOB flag is set and no out-of-band data is available.", 0, 0, 0, 0);
      break;

   // An I/O error occurred while reading from or writing to the file system.
   case EIO:
      s_errorReceivingFunction(SocketLevelError, file, line, "An I/O error occurred while reading from or writing to the file system.", 0, 0, 0, 0);
      break;

   // No buffer space is available.
   case ENOBUFS:
      s_errorReceivingFunction(NetworkLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The system did not have sufficient memory to fulfill the request.
   case ENOMEM:
      s_errorReceivingFunction(SocketLevelError, file, line, "The system did not have sufficient memory to fulfill the request.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // Receive is attempted on a connection-oriented socket that is not connected.
   case ENOTCONN:
      s_errorReceivingFunction(NetworkLevelError, file, line, "Receive is attempted on a connection-oriented socket that is not connected.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The specified flags are not supported for this socket type or protocol.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The specified flags are not supported for this socket type or protocol.", 0, 0, 0, 0);
      break;

   // The connection timed out during connection establishment or due to a transmission timeout on active connection.
   case ETIMEDOUT:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The connection timed out during connection establishment or due to a transmission timeout on active connection.", 0, 0, 0, 0);
      break;

   // The socket is marked nonblocking, and no data is waiting to be received.
   case EWOULDBLOCK:
      // This is not a fault really, it is merely information.
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown recv error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Recv, errorCode) && !isUniqueError(Recv, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The buf parameter is not completely contained in a valid part of the user address space.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The buf parameter is not completely contained in a valid part of the user address space.", 0, 0, 0, 0);
      break;

   // The socket is not connected.
   case WSAENOTCONN:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The socket is not connected.", 0, 0, 0, 0);
      break;

   // The (blocking) call was canceled through WSACancelBlockingCall.
   case WSAEINTR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The (blocking) call was canceled through WSACancelBlockingCall.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.
   case WSAENETRESET:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.
   case WSAEOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.", 0, 0, 0, 0);
      break;

   // The socket has been shut down; it is not possible to recv on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.
   case WSAESHUTDOWN:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The socket is marked as nonblocking and the receive operation would block.
   case WSAEWOULDBLOCK:
      // This is not a fault really, it is merely information.
      break;

   // The message was too large to fit into the specified buffer and was truncated.
   case WSAEMSGSIZE:
      s_errorReceivingFunction(NetworkLevelError, file, line, "The message was too large to fit into the specified buffer and was truncated.", 0, 0, 0, 0);
      break;

   // The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.", 0, 0, 0, 0);
      break;

   // The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.
   case WSAECONNABORTED:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // The connection has been dropped because of a network failure or because the peer system failed to respond.
   case WSAETIMEDOUT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The connection has been dropped because of a network failure or because the peer system failed to respond.", 0, 0, 0, 0);
      break;

   // No buffer space is available.
   case WSAENOBUFS:
      s_errorReceivingFunction(NetworkLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The virtual circuit was reset by the remote side executing a "hard" or "abortive" close. The application should close the socket as it is no longer usable. On a UDP datagram socket this error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
   case WSAECONNRESET:
      // The connection is closed, set the errorCode to Disconnected for the
      // user to detect.
      errorCode = Disconnected;
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown recv error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
//<
//  Reports the error information textually to the log routine.
//
//  \pin  file   The file in which the error occured (the calling file).
//  \pin  line   The line number at which the error occured (from the calling
//               file).
//
//  \return  The error code related to the fault.
//>
//----------------------------------------------------------------------------
int32 TCPClientServer::getsocknameError(char* file, u_int32 line) const
{
   int32 errorCode;
#if defined(__unix) // for now, treat all unix dialects the same way
   errorCode = errno;

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Recv, errorCode) && !isUniqueError(Recv, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // The socket parameter is not valid.
   case EBADF:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter is not valid.", 0, 0, 0, 0);
      break;

   // The address or address_len parameter is not in a readable or writable part of the user address space.
   case EFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The address or address_len parameter is not in a readable or writable part of the user address space.", 0, 0, 0, 0);
      break;

   // The socket is shut down.
   case EINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket is shut down.", 0, 0, 0, 0);
      break;

   // Insufficient resources are available in the system to complete the call.
   case ENOBUFS:
      s_errorReceivingFunction(SocketLevelError, file, line, "Insufficient resources are available in the system to complete the call.", 0, 0, 0, 0);
      break;

   // The available STREAMS resources were insufficient for the operation to complete.
   case ENOSR:
      s_errorReceivingFunction(SocketLevelError, file, line, "The available STREAMS resources were insufficient for the operation to complete.", 0, 0, 0, 0);
      break;

   // The socket parameter refers to a file, not a socket.
   case ENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket parameter refers to a file, not a socket.", 0, 0, 0, 0);
      break;

   // The operation is not supported for this socket's protocol.
   case EOPNOTSUPP:
      s_errorReceivingFunction(SocketLevelError, file, line, "The operation is not supported for this socket's protocol.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown getsockname error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#elif defined(_WIN32)
   errorCode = WSAGetLastError();

   // If this is not a unique error (it has occured before) then it should not
   // be logged again.
   if(!shouldErrorBeProcessed(Recv, errorCode) && !isUniqueError(Recv, errorCode))
   {
      return errorCode;
   }

   switch(errorCode)
   {
   // A successful WSAStartup must occur before using this function.
   case WSANOTINITIALISED:
      s_errorReceivingFunction(SocketLevelError, file, line, "A successful WSAStartup must occur before using this function.", 0, 0, 0, 0);
      break;

   // The network subsystem has failed.
   case WSAENETDOWN:
      s_errorReceivingFunction(SocketLevelError, file, line, "The network subsystem has failed.", 0, 0, 0, 0);
      break;

   // The name or the namelen parameter is not a valid part of the user address space, or the namelen parameter is too small.
   case WSAEFAULT:
      s_errorReceivingFunction(SocketLevelError, file, line, "The name or the namelen parameter is not a valid part of the user address space, or the namelen parameter is too small.", 0, 0, 0, 0);
      break;

   // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
   case WSAEINPROGRESS:
      s_errorReceivingFunction(SocketLevelError, file, line, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", 0, 0, 0, 0);
      break;

   // The descriptor is not a socket.
   case WSAENOTSOCK:
      s_errorReceivingFunction(SocketLevelError, file, line, "The descriptor is not a socket.", 0, 0, 0, 0);
      break;

   // The socket has not been bound to an address with bind, or ADDR_ANY is specified in bind but connection has not yet occurs.
   case WSAEINVAL:
      s_errorReceivingFunction(SocketLevelError, file, line, "The socket has not been bound to an address with bind, or ADDR_ANY is specified in bind but connection has not yet occurs.", 0, 0, 0, 0);
      break;

   // Unknown error
   default:
      {
         char errorMessage[ErrorMessageMaxSize];

         sprintf(errorMessage, "Unknown getsockname error: %d", errorCode);
         s_errorReceivingFunction(SocketLevelError, file, line, errorMessage, 0, 0, 0, 0);
      }
   }
#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   return errorCode;
}

//----------------------------------------------------------------------------
// Tells whether or not the error has been encountered before.
//
//  \pin  errorMethod  The method for which the error may be stored.
//  \pin  errorCode    The reported error
//
//  \return  True if the error is known since before.
//----------------------------------------------------------------------------
bool TCPClientServer::isUniqueError(ErrorMethod errorMethod,
                                    int32 errorCode) const
{
   ErrorDatabase& errorDatabase = s_errorDatabase[errorMethod];

   ErrorData::ErrorStatus errorStatus = ErrorData::DoesNotMatch;

   for(int32 i = 0; (errorStatus == ErrorData::DoesNotMatch) &&
                    (i <= errorDatabase.m_numberOfErrors); i++)
   {
      errorStatus = errorDatabase.m_error[i].isErrorKnown(errorCode, IPAddress());
   }

   // Prevent that more errors are stored than what the database has capacity
   // for.
   if(errorStatus == ErrorData::New &&
      errorDatabase.m_numberOfErrors < ErrorDatabase::MaxRecordedErrors)
   {
      errorDatabase.m_numberOfErrors++;
   }

   return (errorStatus != ErrorData::AlreadyLogged) ? true : false;
}

//----------------------------------------------------------------------------
//  Reports whether or not the error is known. If no error information is
//  added before, false is returned and the error is stored.
//
//  \pin  errorCode  The reported error
//  \pin  IPAddress  The IP address for which the error is reported.
//
//  \return  True if the error is known since before.
//----------------------------------------------------------------------------
TCPClientServer::ErrorData::ErrorStatus TCPClientServer::ErrorData::isErrorKnown(int32 errorCode, u_int32 errorIPAddress)
{
   ErrorData::ErrorStatus errorStatus = ErrorData::DoesNotMatch;

   if(m_error == errorCode &&
      m_IPAddress == errorIPAddress)
   {
      errorStatus = AlreadyLogged;
      m_suppressedOccurencies++;
   }
   else if(m_error == 0 && m_IPAddress == 0)
   {
      m_error = errorCode;
      m_IPAddress = errorIPAddress;

      errorStatus = New;
   }

   return errorStatus;
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  Logs suppressed errors and clears the supressed error statistics.
//>
//----------------------------------------------------------------------------
void TCPClientServer::logSuppressedErrors()
{
   char errorMessage[ErrorMessageMaxSize];
   
   const char *Category[NumberOfErrorMethods] =
   {
      "socket",
      "connect",
      "bind",
      "listen",
      "accept",
      "getsockopt",
      "select",
      "shutdown",
      "send",
      "recv"
   };

   // For each error category, gather suppressed information.
   for(int32 i = 0; i < NumberOfErrorMethods; i++)
   {
      ErrorDatabase& database = s_errorDatabase[i];

      for(int32 err = 0; err < database.m_numberOfErrors; err++)
      {
         ErrorData& errorData = database.m_error[err];

         if(errorData.suppressedOccurencies() > 0)
         {
            // Suppressed information exists.
            sprintf(errorMessage, "Error %d reported as a result of a call to \"%s\" for IP address %s has been suppressed %u times",
                    errorData.error(),
                    Category[i],
                    IPAddressToString(errorData.IPAddress()),
                    errorData.suppressedOccurencies());

            s_errorReceivingFunction(SuppressedError,
                                     __FILE__,
                                     __LINE__,
                                     errorMessage,
                                     0, 0, 0, 0);
         }

         // Make sure that once this error occurs again, it is treated as a new
         // one.
         errorData = ErrorData();
      }

      // All errors have been accounted for (for now).
      database.m_numberOfErrors = 0;
   }
}

//----------------------------------------------------------------------------
//<
//  Assigns an error handling function that receives any error information
//  from the TCPClient/TCPServer instances.
//
//  \pin  theErrorReceiver  The function that should receive error
//                          information.
//>
//----------------------------------------------------------------------------
void TCPClientServer::errorReceivingFunction(TCPClientServer::ErrorReceivingFunction theErrorReceiver)
{
   if(theErrorReceiver)
   {
      assert((s_errorReceivingFunction == NULL ||
              s_errorReceivingFunction == theErrorReceiver) && "Another error handling function is already assigned");

      s_errorReceivingFunction = theErrorReceiver;
   }
}

//----------------------------------------------------------------------------
//  Tells whether or not the error should be processed.
//
//  \pin  errorMethod  The method for which the error may be stored.
//  \pin  errorCode    The reported error
//
//  \return  True if the error should be ignored.
//----------------------------------------------------------------------------
bool TCPClientServer::shouldErrorBeProcessed(ErrorMethod errorMethod,
                                             int32 errorCode) const
{
   bool shouldProcess = false;

   switch(errorMethod)
   {
#ifdef __unix
   case Socket:
      // Errors related to socket-calls should not be processed.
      shouldProcess = false;
      break;

   case Connect:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == EADDRINUSE ||
                       errorCode == EALREADY ||
                       errorCode == ECONNREFUSED ||
                       errorCode == EINPROGRESS ||
                       errorCode == EISCONN ||
                       errorCode == ETIMEDOUT ||
                       errorCode == EWOULDBLOCK) ? true : false;
      break;

   case Bind:
      // Errors related to bind-calls should not be processed.
      shouldProcess = false;
      break;

   case Listen:
     // Errors related to listen-calls should not be processed.
      shouldProcess = false;
      break;

   case Accept:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == EWOULDBLOCK) ? true : false;
      break;

   case Getsockopt:
      // Errors related to getsockopt-calls should not be processed.
      shouldProcess = false;
      break;

   case Select:
      // Errors related to select-calls should not be processed.
      shouldProcess = false;
      break;

   case Shutdown:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == ENOTCONN) ? true : false;
      break;

   case Send:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == ECONNRESET ||
                       errorCode == EPIPE ||
                       errorCode == EWOULDBLOCK) ? true : false;
      break;

   case Recv:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == ECONNRESET ||
                       errorCode == EWOULDBLOCK) ? true : false;
      break;

#elif defined(_WIN32)
   case Socket:
      // Errors related to socket-calls should not be processed.
      shouldProcess = false;
      break;

   case Connect:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == WSAEADDRINUSE ||
                       errorCode == WSAEALREADY ||
                       errorCode == WSAECONNREFUSED ||
                       errorCode == WSAEISCONN ||
                       errorCode == WSAETIMEDOUT ||
                       errorCode == WSAEWOULDBLOCK) ? true : false;
      break;

   case Bind:
      // Errors related to bind-calls should not be processed.
      shouldProcess = false;
      break;

   case Listen:
      // Errors related to listen-calls should not be processed.
      shouldProcess = false;
      break;

   case Accept:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == WSAEWOULDBLOCK) ? true : false;
      break;

   case Getsockopt:
      // Errors related to getsockopt-calls should not be processed.
      shouldProcess = false;
      break;

   case Select:
      // Errors related to select-calls should not be processed.
      shouldProcess = false;
      break;

   case Shutdown:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == WSAENOTCONN) ? true : false;
      break;

   case Send:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == WSAENETDOWN ||
                       errorCode == WSAENETRESET ||
                       errorCode == WSAESHUTDOWN ||
                       errorCode == WSAEWOULDBLOCK ||
                       errorCode == WSAECONNABORTED ||
                       errorCode == WSAECONNRESET) ? true : false;
      break;

   case Recv:
      // The following errors are handled by the TCPServer/TCPClient.
      shouldProcess = (errorCode == WSAENETDOWN ||
                       errorCode == WSAENETRESET ||
                       errorCode == WSAESHUTDOWN ||
                       errorCode == WSAEWOULDBLOCK ||
                       errorCode == WSAECONNABORTED ||
                       errorCode == WSAECONNRESET) ? true : false;
      break;

#else
#error OS not supported, __FILE__:__LINE__
#endif // __unix or _WIN32

   default:
      // If the error method for some reason has not been accounted for, then
      // return information that tells that the error should be logged.
      shouldProcess = false;
   }

   return shouldProcess;
}

#ifdef __unix
//----------------------------------------------------------------------------
//  The sigPIPEHandler method is installed to catch SIGPIPE on Unix systems.
//  This is done since writing to a closed socket will cause the OS to raise
//  a SIGPIPE. When ignored, the write / send call will return EPIPE when
//  writing to a closed connection.
//----------------------------------------------------------------------------
void TCPClientServer::sigPIPEHandler(int32)
{
   // The PIPE handler should just ignore the signal to let the send / write
   // operation catch the fault by the returned EPIPE fault code.
}
#endif // __unix
