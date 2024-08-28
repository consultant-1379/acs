/*=================================================================== */
/**
   @file   CUTE_ACS_JTP_Conversation_R2A.h

   @brief  Header file for testing Conversation R2A interface for APJTP.

   @version 1.0.0

   @documentno CAA 109 0870

   @copyright Ericsson AB,Sweden 2010. All rights reserved.


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/18/2010     XTANAGG   APG43 on Linux.
==================================================================== */
#ifndef _CUTE_ACS_JTP_CONVERSATION_R2A_
#define _CUTE_ACS_JTP_CONVERSATION_R2A_

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ACS_JTP_Conversation_R2A.h"


class CUTE_ACS_JTP_Conversation_R2A{

public:

	static void init();

	static void destroy();

	static void testTimeout();

	static void testRetries();

	static void testDelay();

	static void testSide();

	static void testState();

	static void testJexinitreq();

	static void testJexinitconf();

	static void testJexdataind();

	static void testJexdiscreq();

	static void testJTPHandle();

	static void testJexdatareq();

	static cute::suite makeACSJTPConversationR2ASuite();

	static ACS_JTP_Conversation_R2A *C1;

};


#endif
