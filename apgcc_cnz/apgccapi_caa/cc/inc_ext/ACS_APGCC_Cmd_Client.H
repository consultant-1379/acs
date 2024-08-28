/*
 * ACS_APGCC_Cmd_Client.H
 *
 *  Created on: Apr 2, 2010
 *      Author: root
 */

#ifndef ACS_APGCC_CMD_CLIENT_H_
#define ACS_APGCC_CMD_CLIENT_H_


//*****************************************************************************
//
// .NAME
//     ACS_APGCC_Cmd_Client - Internal template for a command client
// .LIBRARY 3C++
// .PAGENAME ACS_APGCC
// .HEADER  ACS_APGCC
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_Cmd_Client.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1996.
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
//  This template can be used to implement a command client
//  that needs to access a command server. This template
//  is build upon the concept of Connector, Stream and Addr
//  classes.
//
//  To be able to use this class the following must be provided:
//    PEER_Connector
//    PEER_Stream
//    PEER_Addr
//
//  Define a command client by defining the template:
//
//  ACS_APGCC_Cmd_Client<SOCK_Connector,SOCK_Stream,INET_Addr> client("127.0.0.1:123")
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
//      A               11-06-07        xantior         porting

// .LINKAGE
//      -

//*****************************************************************************


#  include "ACS_APGCC_Command.H"



#ifdef ACE_NLOGGING
#  undef DEBUG_CLIENT
#else
#  undef DEBUG_CLIENT
#endif


template <class PEER_Connector, class PEER_Stream, class PEER_Addr>
class ACS_APGCC_Cmd_Client
{
public:

   ACS_APGCC_Cmd_Client()
   {
   }


   ACS_APGCC_Cmd_Client(const PEER_Addr& addr)
   :addr_(addr)
   {}


   void set(const PEER_Addr& addr)
   {
      addr_ = addr;
   }


   int execute(ACS_APGCC_Command& cmd)
   {
	  int res = 0;
      PEER_Stream    stream_;
      PEER_Connector server_;


      cmd.result = -1;

      if ( server_.connect(stream_, addr_) < 0 ) {
    	res = -1;
		stream_.close();
		server_.close();
      }

      cmd.encode();

      if ( cmd.send(stream_) < 0 ) {
    	  res = -1;
    	  stream_.close();
    	  server_.close();
      }


      if ( cmd.recv(stream_) < 0 ) {
         res = -1;
      }

      return res;
   }


protected:

   PEER_Addr      addr_;
};

#endif /* ACS_APGCC_CMD_CLIENT_H_ */
