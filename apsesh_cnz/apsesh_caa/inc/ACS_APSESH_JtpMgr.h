//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_JtpMgr
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************
#ifndef ACS_APSESH_JtpMgr_h
#define ACS_APSESH_JtpMgr_h 1

#include "ace/Reactor.h"

#include "ACS_JTP.h"
#include "ACS_APSESH_Trace.h"
#include "ACS_APSESH_JtpReturnType.h"
#include "ACS_APSESH_JtpMgrInterface.h"

//	Implements a JTP server that serves a single JTP client, which is always
//	SESH.  The connection is a JTP conversation, and it is intended that the
//	connection will always be active as long as CP and AP are in operation.
//	While the connection is active, this class manages the data flow between
//	APSESH and SESH, and provides an API to APSESH for access and control of the
//	data.

class ACS_APSESH_JtpMgr: public ACS_APSESH_JtpMgrInterface
{

  private:

    struct JexInitInd 
    {
		short unsigned data1;
		short unsigned data2;
		short unsigned maxBufferSize;
    };

    struct JexInitRsp 
    {
		short unsigned data1;
		short unsigned data2;
		short resultCode;
    };

    struct JexDataReq 
    {
		short unsigned data1;
		short unsigned data2;
		char* buffer;
		int bufLen;
    };

    struct JexDataInd 
    {
		short unsigned data1;
		short unsigned data2;
		char* buffer;
		int bufLen;
    };

    struct JexDiscReq 
    {
		short unsigned data1;
		short unsigned data2;
		short reasonCode;
    };

    struct JexDiscInd 
    {
		short unsigned data1;
		short unsigned data2;
		short reasonCode;
    };

    union JtpMsgUnion 
    {
		JexInitInd jexInitInd;
		JexInitRsp jexInitRsp;
		JexDataInd jexDataInd;
		JexDiscReq jexDiscReq;
		JexDiscInd jexDiscInd;
		JexDataReq jexDataReq;
    };

    typedef enum { JEXINITIND, JEXINITRSP, JEXDATAREQ, JEXDATAIND, JEXDISCREQ, JEXDISCIND } JtpMsgType;

    struct JtpMsg 
    {
		JtpMsgType type;
		JtpMsgUnion content;
    };

    class JtpMgrState 
    {
      public:
          virtual ~JtpMgrState();

          //	Gets quorum data sent from SESH.
          //
          //	This function tries to retrieve quorum data from SESH via JTP.  This is a
          //	blocking function; it will wait until it receives the data, or until an
          //	error is encountered.
          //
          //	@param buffer
          //	  The buffer into which the quorum data will be placed.
          //	@param context
          //	  Points back to the context object (JtpMgr), which provides access to
          //	functions used in common by the state objects.
          virtual ACS_APSESH_JtpReturnType getData (char*& buffer, int& bufLen, ACS_APSESH_JtpMgr& context);

          //	Notifies SESH of APSESH's success or failure in sending quorum data to CS.
          //	This function is not blocking.
          //
          //	@param status
          //	  Zero if APSESH failed in sending the quorum data to CS, and nonzero if it
          //	succeded.
          //
          //	@param context
          //	  Points back to the context object (JtpMgr), which provides access to
          //	functions used in common by the state objects.
          virtual ACS_APSESH_JtpReturnType sendStatusCs (short status, ACS_APSESH_JtpMgr& context);
    };

    class StateNoConn : public JtpMgrState
    {
      public:

          virtual ~StateNoConn();

          ACS_APSESH_JtpReturnType getData (char*& buffer, int& bufLen, ACS_APSESH_JtpMgr& context);

          ACS_APSESH_JtpReturnType sendStatusCs (short status, ACS_APSESH_JtpMgr& context);

    };

    class StateWaitData : public JtpMgrState
    {
      public:

		virtual ~StateWaitData();

		ACS_APSESH_JtpReturnType getData (char*& buffer, int& bufLen, ACS_APSESH_JtpMgr& context);

		ACS_APSESH_JtpReturnType sendStatusCs (short status, ACS_APSESH_JtpMgr& context);
    };

    bool m_published;

  public:

      ACS_APSESH_JtpMgr ();

      virtual ~ACS_APSESH_JtpMgr();

      virtual ACS_APSESH_JtpReturnType init ();

      virtual ACS_APSESH_JtpReturnType getData (char*& buffer, int& bufLen);

      virtual ACS_APSESH_JtpReturnType sendStatusCs (short status);

      virtual void setJtpWaitTimeout (const unsigned int& timeout);

      virtual void resetJtpWaitTimeout ();

      virtual void unregister();

  private:

		//	Listens for and accepts a new JTP connection.
		ACS_APSESH_JtpReturnType connect ();


		//	Makes a single query for a JTP message.
		//
		//	@param msg
		//	  The message's contents will be placed here.
		ACS_APSESH_JtpReturnType getMsgFromJtp (JtpMsg& msg);


		//	Sends a message to SESH via JTP.
		//
		//	@param msg
		//	  Holds the message's contents.
		ACS_APSESH_JtpReturnType sendMsgToJtp (JtpMsg msg);

		void nextState (JtpMgrState& state);

		ACS_JTP_Conversation m_jtpConv;

		ACS_JTP_Service* m_jtpSvc;

		static const short  DEF_TRACE_MSG_SIZE;

		int unsigned m_jtpWaitTimeout;

		JtpMgrState* m_currentState;

		static StateWaitData m_stateWaitData;

		static StateNoConn m_stateNoConn;

};

#endif
