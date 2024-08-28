/*=================================================================== */
/**
   @file   CUTE_ACS_JTP_Service_R2A.h

   @brief  Header file for testing Service R2A interface for APJTP.

   @version 1.0.0

   @documentno CAA 109 0870

   @copyright Ericsson AB,Sweden 2010. All rights reserved.


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/28/2010		XTANAGG        APG43 on Linux.
==================================================================== */
#ifndef _CUTE_ACS_JTP_SERVICE_R2A_
#define _CUTE_ACS_JTP_SERVICE_R2A_

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ACS_JTP_Conversation_R2A.h"
#include "ACS_JTP_Job_R2A.h"
#include "ACS_JTP_Service_R2A.h"

typedef ACE_HANDLE JTP_HANDLE;

class CUTE_ACS_JTP_Service_R2A
{
public:

	static void testJidrepreq();

	static void testGetHandles();

	static void init();

	static void destroy();

	static void testAccept();

	static void testJinitind();

	static void testJinitrsp();

	static void testJresultreq();

	static void testJexinitind();

	static void testJexinitrsp();

	static void testJexdatareq();

	static void testJexdataind();

	static void testJexdiscind();

	static cute::suite makeACSJTPServiceR2AJobSuite();

	static cute::suite makeACSJTPServiceR2AConvSuite();

	static ACS_JTP_Service_R2A *S1;

	static ACS_JTP_Job_R2A *J1;

	static ACS_JTP_Conversation_R2A *C1;
};


#endif
