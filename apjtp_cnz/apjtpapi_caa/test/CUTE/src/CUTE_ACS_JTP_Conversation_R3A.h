/*=================================================================== */
/**
   @file   CUTE_ACS_JTP_Conversation_R3A.h

   @brief  Header file for testing Conversation R3A interface for APJTP.

   @version 1.0.0

   @documentno CAA 109 0870

   @copyright Ericsson AB,Sweden 2010. All rights reserved.


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/19/2010     XTANAGG        APG43 on Linux.
==================================================================== */
#ifndef _CUTE_ACS_JTP_CONVERSATION_R3A_
#define _CUTE_ACS_JTP_CONVERSATION_R3A_

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ACS_JTP_Conversation_R3A.h"

class CUTE_ACS_JTP_Conversation_R3A{

public:

	static void init();

	static void destroy();

	static void testTimeout();

	static void testRetries();

	static void testDelay();

	static void testSide();

	static void testState();

	static void testJexinitreq();

	static void testJexinitreqNode();

	static void testJexinitconf();

	static void testJexdataind();

	static void testJTPHandle();

	static void testJexdiscreq();

	static void testJexdatareq();

	static void testQuery();

	static void testQueryWithTimeout();

	static cute::suite makeACSJTPConversationR3ASuite();

	//Conversation Object
	static ACS_JTP_Conversation_R3A *C1;

};

#endif
