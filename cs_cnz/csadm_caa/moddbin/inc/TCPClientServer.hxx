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
//  The TCPClientServer groups the functionality common to the TCPClient and
//  the TCPServer.
//>

#ifndef TCPCLIENTSERVER_HXX
#define TCPCLIENTSERVER_HXX

#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif // _MSC_VER

#pragma ident "@(#)filerevision "

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "PlatformTypes.hxx"
#include "ObjectStatus.hxx"

#if !defined _WIN32 && !defined __unix
#error Your platform is not supported by the TCPClientServer.
#endif // _WIN32 or __unix

#if defined __unix
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/resource.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h> // TCP_NODELAY
#include <cerrno>
#elif defined _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define UNDEF_WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#ifdef UNDEF_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#undef UNDEF_WIN32_LEAN_AND_MEAN
#endif // UNDEF_WIN32_LEAN_AND_MEAN
#endif // unix or _WIN32

#include <cstring>  // memset

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

//< \ingroup comm
//
//  This is the base class for the TCPClient and the TCPServer classes.
//  When used in a Unix environment, it installs a signal handler to ignore
//  the occurrence of PIPE since that is used to indicate that a remote host
//  has disconnected a TCP session when writing to the socket. The signal is
//  ignored in order for the write to return an error which is caught and
//  handled.
//
//  Class relationships (relations to TCPClientServer).
//
//  \verbatim
//  +-------------------+
//  |                   |
//  |  TCPClientServer  |
//  |                   |
//  +-------------------+
//  \endverbatim
//>
#ifndef _WIN32
   typedef int32 SOCKET;
#endif // _WIN32

class TCPClientServer
{
public:
   //<
   //  The different values the Status may report.
   //>
   enum Status 
   {
      OK,
      NotOK,
      CreateSocketError,
      BindError,
      ListenError,
      Disconnected
   };

   //<
   //  Whether or not an instance should operate in blocking or non-blocking
   //  mode.
   //>
   enum Mode
   {
      NonBlocking, ///< All operations on the connection are non blocking.
      Blocking     ///< All operations on the connection are blocking. 
   };

   enum BufferName
   {
      SendBuffer    = SO_SNDBUF,
      ReceiveBuffer = SO_RCVBUF
   };

   //<
   //  The error types reported by instances of TCPClientServer.
   //>
   enum ErrorType
   {
      SocketLevelError,           ///< The error ocurred on socket level.
      NetworkLevelError,          ///< The error ocurred on network level.
      SuppressedError,            ///< This specific error has been suppressed.
      IllegalStateForOperation,   ///< An operation has ocurred when it should not.
      InstallSignalHandlerFailure ///< 
   };

   //<
   //  Valid reasons to use as data1 information when the ErrorType is
   //  IllegalStateForOperation.
   //>
   enum IllegalStateForOperationReason
   {
      AlreadyInUse = 0xffff
   };

   //<
   //  Information about the socket.
   //>
   struct SocketInformation
   {
      SOCKET m_socket; ///< The socket number.
      Mode  m_mode;    ///< The socket Mode.
   };

   struct Timeout
   {
      Timeout(int32 seconds = 0, int32 microseconds = 0) :
         m_seconds(seconds),
         m_microseconds(microseconds)
      {
      }

      int32 m_seconds;
      int32 m_microseconds;
   };

   typedef void (*ErrorReceivingFunction)(ErrorType theErrorType,
                                          const char* filename,
                                          int line,
                                          const char* description,
                                          u_int16 data1,
                                          u_int16 data2,
                                          u_int16 data3,
                                          u_int16 data4);

   // Tells wether the TCPClientServer is connected or not.
   bool isConnected() const;

   // Tells the socket's send buffer size.
   int32 socketSendBufferSize() const;

   // Tells the socket's receive buffer size.
   int32 socketReceiveBufferSize() const;

   // Sets the socket's send buffer size.
   Status socketSendBufferSize(int32) const;

   // Sets the socket's receive buffer size.
   Status socketReceiveBufferSize(int32) const;

   // Converts an IP address (numeric format) to a string.
   static char* IPAddressToString(u_int32);

   // Converts an IP address (string format) to a numerical value.
   static u_int32 IPAddressToDecimal(const char*);

   // Returns the IP address for a given machine name.
   static u_int32 getIPAddressByName(const char*);
   static u_int32 getIPAddressByName(const u_char*);

   // Looks up the host name for the local machine.
   static void getHostname(const char*, u_int32&);

   // Logs suppressed errors and clears the suppressed error statistics.
   static void logSuppressedErrors();

   // Assigns an error handling function that receives any error information
   // from the TCPClientServer instances.
   static void errorReceivingFunction(ErrorReceivingFunction theErrorReceiver);

protected:
#ifdef __unix
   //<
   // These values are already defined in the Win32 environment.
   //>
   enum
   {
      INVALID_SOCKET = -1,
      SOCKET_ERROR   = -1
   };
#endif // __unix

   //<
   // These enumerations are set up in order to be able to use common
   // enumerations for various events although their real values may differ
   // between platforms.
   //>
   enum ServiceError
   {
#ifdef _WIN32
      AlreadyConnected       = WSAEISCONN,
      SendError              = SOCKET_ERROR,
      RecvError              = SOCKET_ERROR,
      ShutDownSendAndReceive = SD_BOTH,
      AddressAlreadyInUse    = WSAEADDRINUSE,
      ConnectionTimedOut     = WSAETIMEDOUT,
      ConnectionRefused      = WSAECONNREFUSED
#elif defined __unix // _WIN32
      AlreadyConnected       = EISCONN,
      SendError              = -1,
      RecvError              = -1,
      ShutDownSendAndReceive = SHUT_RDWR,
      AddressAlreadyInUse    = EADDRINUSE,
      ConnectionTimedOut     = ETIMEDOUT,
      ConnectionRefused      = ECONNREFUSED
#endif // _WIN32 or __unix
   };

   //<
   //  The maximum size of an error message string.
   //>
   enum
   {
      ErrorMessageMaxSize = 1024
   };

   // Constructor.
   TCPClientServer(Mode, ErrorReceivingFunction = NULL);
   
   // Destructor.
   ~TCPClientServer();

   // These are all service routines for the derived classes (TCPClient and
   // TCPServer).

   // Turn blocking off for a socket.
   Status blockingOff(SOCKET);

   // Turn blocking on for a socket.
   Status blockingOn(SOCKET);

   // Enable Nagle's algorithm for a socket.
   void enableNagle(SOCKET, char*, u_int32);

   // Disable Nagle's algorithm for a socket.
   void disableNagle(SOCKET, char*, u_int32);

   // Create a socket.
   SOCKET createSocket();

   // Close a socket.
   void closeTheSocket(SOCKET&);

   // Initializes the fields in the sockaddr_in structure.
   void initializeSocketAddressStructure(int32, int32);

   // Error handling routines.
   int32 socketError(char*, u_int32) const;
   int32 connectError(char*, u_int32) const;
   int32 bindError(char*, u_int32) const;
   int32 listenError(char*, u_int32) const;
   int32 acceptError(char*, u_int32) const;
   int32 getsockoptError(char*, u_int32) const;
   int32 selectError(char*, u_int32) const;
   int32 shutdownError(char*, u_int32) const;
   int32 sendError(char*, u_int32) const;
   int32 recvError(char*, u_int32) const;
   int32 getsocknameError(char*, u_int32) const;

   // Tells the status of a created instance.
   ObjectStatus status() const;

   //<
   //  True when connected, false otherwise.
   //>
   bool m_isConnected;

   //<
   //  The client/server side socket.
   //>
   SOCKET m_socket;

   //<
   //  Handle to the socket address.
   //>
   struct sockaddr_in m_socketAddress;

   // Tells the IP address associated with a socket.
   u_int32 IPAddress() const;

   // Tells the port number associated with a socket.
   u_int16 portNumber() const;

   //<
   //  Defines whether or not operations should be performed blocking or non-
   //  blocking.
   //>
   const Mode m_Mode;

   // Pointer to a function that receives error information.
   static ErrorReceivingFunction s_errorReceivingFunction;

private:
   //<
   //  Counts the number of error handling routines.
   //>
   enum ErrorMethod
   {
      Socket = 0,
      Connect,
      Bind,
      Listen,
      Accept,
      Getsockopt,
      Select,
      Shutdown,
      Send,
      Recv,
      NumberOfErrorMethods
   };

   // Declared but not implemented in order to render the copy ctor unusable.
   TCPClientServer(const TCPClientServer&);

   // Declared but not implemented in order to render the assignment operator
   // unusable.
   TCPClientServer& operator=(const TCPClientServer&);

   // Tells whether or not the error has been encountered before.
   bool isUniqueError(ErrorMethod, int32) const;

   // Tells whether or not the error should be processed.
   bool shouldErrorBeProcessed(ErrorMethod, int32) const;

   // Tells the socket's buffer size.
   int32 socketBufferInformation(BufferName) const;

   // Sets the socket's buffer size.
   Status socketBufferInformation(BufferName, int32) const;

#ifdef __unix
   // Signal handler used to ignore PIPE.
   static void sigPIPEHandler(int32);
#endif // __unix

   // Class that records error information in a pair with a specific IP
   // address.
   class ErrorData
   {
   public:
      // Initializes the ErrorData item to "empty".
      ErrorData() :
         m_error(0),
         m_IPAddress(0),
         m_suppressedOccurencies(0)
      {
      }

      enum ErrorStatus
      {
         New,
         AlreadyLogged,
         DoesNotMatch
      };

      enum
      {
         MaxSuppressedOccurencies = 0xFFFFFFFF
      };

      // Reports whether or not the error is known. If no error information is
      // added before, false is returned and the error is stored.
      ErrorStatus isErrorKnown(int32, u_int32);

      // Reports the number of suppressed occurencies for the current error.
      u_int32 suppressedOccurencies()
      {
         return m_suppressedOccurencies;
      }

      // Returns the error code.
      int32 error() const
      {
         return m_error;
      }

      // Returns the IP address that is related to the current error code.
      u_int32 IPAddress() const
      {
         return m_IPAddress;
      }

   private:
      // The error code in question.
      int32 m_error;

      // The IP address related to the error.
      u_int32 m_IPAddress;

      // The number of times this specific error has occured but has been
      // suppressed (error handling is performed but no information about it
      // has been shared).
      u_int32 m_suppressedOccurencies;
   };

   // Class that contains the database for errors that has occured.
   class ErrorDatabase
   {
   public:
      ErrorDatabase() :
         m_numberOfErrors(0)
      {
      }

      // The number of unique errors that should be kept in the database.
      enum
      {
         MaxRecordedErrors = 100
      };

      // The number of recorded errors.
      int32 m_numberOfErrors;

      // The entries that describe the errors.
      ErrorData m_error[MaxRecordedErrors + 1];
   };

   // The error database that contains all logged errors (if storage allows
   // it).
   static ErrorDatabase s_errorDatabase[NumberOfErrorMethods];

   // The status of an instance.
   ObjectStatus m_status;

#ifdef _WIN32
   // The number of client/server instances.
   static u_int32 s_noOfInstances;
#elif defined __unix
   // Holds information on whether a signal handler ignoring PIPE has been
   // installed or not.
   static bool s_sigPIPEHandlerIsInstalled;
#endif // _WIN32 || __unix
};

//----------------------------------------------------------------------------
//<
//  Tells the status of an instance.
//
//  \return  For Tru64 systems, ObjectOK is always returned. For Windows
//           systems the returned status can be either of ObjectOK and
//           ObjectNotOK depending on the result of the WSAStartup routine.
//>
//----------------------------------------------------------------------------
inline ObjectStatus TCPClientServer::status() const
{
#ifdef _WIN32
   return m_status;
#elif defined __unix // _WIN32
   return ObjectOK;
#endif // _WIN32 or __unix
}

//----------------------------------------------------------------------------
//<
//  Sets a created socket into non-blocking IO mode.
//  No action is taken if the supplied socket is invalid.
//
//  \pin  theSocket  A socket.
//
//  \returns  OK if the socket was successfully set in non-blocking mode,
//            otherwise NotOK.
//>
//----------------------------------------------------------------------------
inline TCPClientServer::Status TCPClientServer::blockingOff(SOCKET theSocket)
{
   if(theSocket != INVALID_SOCKET)
   {
      // Set non blocking flag
#ifdef _WIN32
      unsigned long set = 1;

      // A return value other than 0 indicates an error.
      return ioctlsocket(theSocket, FIONBIO , &set) == 0 ? OK : NotOK;
#elif defined __unix // _WIN32
      int32 flags = fcntl(theSocket, F_GETFL, 0);

      // A negative flags reply indicates an error.
      if(flags >= 0)
      {
         // A return value of -1 indicates an error.
         return fcntl(theSocket, F_SETFL, flags | O_NONBLOCK) != -1 ? OK : NotOK;
      }
      else
      {
         return NotOK;
      }
#endif // _WIN32 or __unix
   }

   return NotOK;
}

//----------------------------------------------------------------------------
//<
//  Sets a created socket into blocking IO mode.
//  No action is taken if the supplied socket is invalid.
//
//  \pin  theSocket  A socket.
//
//  \returns  OK if the socket was successfully set in blocking mode,
//            otherwise NotOK.
//>
//----------------------------------------------------------------------------
inline TCPClientServer::Status TCPClientServer::blockingOn(SOCKET theSocket)
{
   if(theSocket != INVALID_SOCKET)
   {
      // Clear the non blocking flag
#ifdef _WIN32
      unsigned long clear = 0;

      // A return value other than 0 indicates an error.
      return ioctlsocket(theSocket, FIONBIO, &clear) == 0 ? OK : NotOK;
#elif defined __unix // _WIN32
      int32 flags = fcntl(theSocket, F_GETFL, 0);

      // A negative flags reply indicates an error.
      if(flags >= 0)
      {
         // A return value of -1 indicates an error.
         return fcntl(theSocket, F_SETFL, flags & ~O_NONBLOCK) != -1 ? OK : NotOK;
      }
      else
      {
         return NotOK;
      }
#endif // _WIN32 or __unix
   }

   return NotOK;
}

//----------------------------------------------------------------------------
//<
//  Enable Nagle's algorithm for a socket (default).
//  No action is taken if the supplied socket is invalid.
//
//  \error  
//  If Nagle's algorithm cannot be enabled, the error receiver is informed.
//
//  \pin  theSocket  A socket.
//  \pin  file       The name of the calling file (__FILE__).
//  \pin  line       The line number from which the disableNagle method was
//                   called (__LINE__).
//>
//----------------------------------------------------------------------------
inline void TCPClientServer::enableNagle(SOCKET theSocket,
                                         char* file,
                                         u_int32 line)
{
   if(theSocket != INVALID_SOCKET)
   {
      // Do not set the TCP_NODELAY option.
      int32 setTCPNoDelay = 0;

      if(setsockopt(theSocket,
                    IPPROTO_TCP,
                    TCP_NODELAY,
                    reinterpret_cast<char*>(&setTCPNoDelay), sizeof(setTCPNoDelay)) != 0)
      {
         static_cast<void>(socketError(file, line));
      }
   }
}

//----------------------------------------------------------------------------
//<
//  Disable Nagle's algorithm for a socket.
//  No action is taken if the supplied socket is invalid.
//
//  \error  
//  If Nagle's algorithm cannot be disabled, the error receiver is informed.
//
//  \pin  theSocket  A socket.
//  \pin  file       The name of the calling file (__FILE__).
//  \pin  line       The line number from which the disableNagle method was
//                   called (__LINE__).
//>
//----------------------------------------------------------------------------
inline void TCPClientServer::disableNagle(SOCKET theSocket,
                                          char* file,
                                          u_int32 line)
{
   if(theSocket != INVALID_SOCKET)
   {
      // Set the TCP_NODELAY option.
      int32 setTCPNoDelay = 1;

      if(setsockopt(theSocket,
                    IPPROTO_TCP,
                    TCP_NODELAY,
                    reinterpret_cast<char*>(&setTCPNoDelay), sizeof(setTCPNoDelay)) != 0)
      {
         static_cast<void>(socketError(file, line));
      }
   }
}

//----------------------------------------------------------------------------
//<
//  Closes the supplied socket.
//  No action is taken if the supplied socket is invalid.
//
//  \pinout  theSocket  A socket, upon return always set to INVALID_SOCKET.
//>
//----------------------------------------------------------------------------
inline void TCPClientServer::closeTheSocket(SOCKET& theSocket)
{
   if(theSocket != INVALID_SOCKET)
   {
#ifdef _WIN32
      static_cast<void>(closesocket(theSocket));
#elif defined __unix // _WIN32
      static_cast<void>(close(theSocket));
#endif // _WIN32 or __unix
      theSocket = INVALID_SOCKET;
   }
}

//----------------------------------------------------------------------------
//<
//  Returns a created socket.
//
//  \return  A valid socket descriptor or INVALID_SOCKET.
//>
//----------------------------------------------------------------------------
inline SOCKET TCPClientServer::createSocket()
{
   // Create a socket
   return socket(AF_INET, SOCK_STREAM, 0);
}

//----------------------------------------------------------------------------
//<
//  Sets up the sockaddr_in structure with the supplied arguments.
//
//  \pin  theAddress     The IP addres to which the connection should be made.
//  \pin  thePortNumber  The port number that should be used for the
//                       connection.
//>
//----------------------------------------------------------------------------
inline void TCPClientServer::initializeSocketAddressStructure(int32 theAddress,
                                                              int32 thePortNumber)
{
   m_socketAddress.sin_family      = AF_INET;
   m_socketAddress.sin_addr.s_addr = htonl(theAddress);
   m_socketAddress.sin_port        = htons(thePortNumber);
}

//----------------------------------------------------------------------------
//<
//  Tells wether the TCPClientServer is connected or not.
//
//  \return  True if the instance has established a connection, false
//           otherwise.
//>
//----------------------------------------------------------------------------
inline bool TCPClientServer::isConnected() const
{
   return m_isConnected;
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
inline u_int32 TCPClientServer::getIPAddressByName(const u_char* machineName)
{
   return getIPAddressByName(reinterpret_cast<const char*>(machineName));
}

//----------------------------------------------------------------------------
//<
//  Tells the IP address associated with a socket.
//
//  \return  The IP address associated with a socket.
//>
//----------------------------------------------------------------------------
inline u_int32 TCPClientServer::IPAddress() const
{
   return ntohl(m_socketAddress.sin_addr.s_addr);
}

//----------------------------------------------------------------------------
//<
//  Tells the port number associated with a socket.
//
//  \return  The port number associated with a socket.
//>
//----------------------------------------------------------------------------
inline u_int16 TCPClientServer::portNumber() const
{
   return ntohs(m_socketAddress.sin_port);
}

//----------------------------------------------------------------------------
//<
//  Tells the socket's send buffer size.
//
//  \return  The socket's send buffer size in bytes, 0 indicates an error.
//>
//----------------------------------------------------------------------------
inline int32 TCPClientServer::socketSendBufferSize() const
{
   return socketBufferInformation(SendBuffer);
}

//----------------------------------------------------------------------------
//<
//  Tells the socket's receive buffer size.
//
//  \return  The socket's receive buffer size in bytes, 0 indicates an error.
//>
//----------------------------------------------------------------------------
inline int32 TCPClientServer::socketReceiveBufferSize() const
{
   return socketBufferInformation(ReceiveBuffer);
}

//----------------------------------------------------------------------------
//<
//  Sets the socket's send buffer size.
//
//  \pin  size  The wanted send buffer size in bytes.
//
//  \return  OK if successful, otherwise NotOK.
//>
//----------------------------------------------------------------------------
inline TCPClientServer::Status TCPClientServer::socketSendBufferSize(int32 size) const
{
   return socketBufferInformation(SendBuffer, size);
}

//----------------------------------------------------------------------------
//<
//  Sets the socket's receive buffer size.
//
//  \pin  size  The wanted receive buffer size in bytes.
//
//  \return  OK if successful, otherwise NotOK.
//>
//----------------------------------------------------------------------------
inline TCPClientServer::Status TCPClientServer::socketReceiveBufferSize(int32 size) const
{
   return socketBufferInformation(ReceiveBuffer, size);
}

#endif // TCPCLIENTSERVER_HXX
