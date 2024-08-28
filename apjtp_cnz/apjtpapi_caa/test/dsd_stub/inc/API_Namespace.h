/*=================================================================== */
/**
   @file   API_namespace.h

   @brief  Header file for DSD stubs for APJTP.

		   This module contains all the declarations useful to
           specify the namespace API_namespace.
           .
   @version N.N.N

   @documentno CAA 109 0870

   @copyright Ericsson AB,Sweden 2010. All rights reserved.

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/28/2010     TA        APG43 on Linux.
==================================================================== */
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _API_Namespace_h
#define _API_Namespace_h

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>

namespace DSD_API
{
	typedef enum
	{
		SERVICE_MODE_UNKNOWN,
		SERVICE_MODE_SOCKET,
		SERVICE_MODE_LSOCKET,
		SERVICE_MODE_PIPE,
		SERVICE_MODE_SOCKET_PRIVATE,
		SERVICE_MODE_LSOCKET_PRIVATE,
		SERVICE_MODE_PIPE_PRIVATE,
	}ServiceMode;

	typedef enum
	{
		NODE_STATE_ACTIVE,
		NODE_STATE_PASSIVE,
		NODE_STATE_UNKNOWN
	}NodeState;

	typedef enum
	{
		NODE_SIDE_LOCAL,
		NODE_SIDE_REMOTE
	}NodeSide;

	typedef enum
	{
		CP_STATE_UNKNOWN,
		CP_STATE_NORMAL,
		CP_STATE_SEPARATED,
	}CpState;

	typedef enum
	{
		SYSTEM_TYPE_AP,
		SYSTEM_TYPE_CP,
		SYSTEM_TYPE_BC
	}SystemType;

	typedef struct
	{
		ACE_UINT32 systemId;
		SystemType sysType;
		NodeState nState;
		ACE_TCHAR nodeName[32];
	} ACS_DSD_Node;

	enum
	{
		THISNODE=50000,   // this node
		PARTNER,      // opposite AP node.
		FRONT_END_AP,
		CLOCK_MASTER,
		ALARM_MASTER
	};

	typedef enum 
	{
		REGISTER=0,
		JINITREQ,
		JINITIND,
		JINITRSP,
		JINITCONF,
		JRESULTREQ,
		JRESULTIND,
		JEXINITREQ,
		JEXINITRSP,
		JEXDATAREQ,
		JEXDISCREQ
	}MsgType;


}

#endif	/* end API_namespace */
