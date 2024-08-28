/*=================================================================== */
/**
   @file   CUTE_ACS_JTP_Job_R3A.h

   @brief  Header file for testing Job R3A interface for APJTP.

   @version 1.0.0

   @documentno CAA 109 0870

   @copyright Ericsson AB, Sweden 2010. All rights reserved.


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       09/21/2010   XTANAGG        APG43 on Linux.
==================================================================== */
#ifndef _CUTE_ACS_JTP_JOB_R3A_
#define _CUTE_ACS_JTP_JOB_R3A_

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ACS_JTP_Job_R3A.h"

class CUTE_ACS_JTP_Job_R3A{

public:

	static void init();

	static void destroy();

	static void testTimeout();

	static void testRetries();

	static void testDelay();

	static void testSide();

	static void testState();

	static void testJinitreq();

	static void testJinitreqNode();

	static void testJinitconf();

	static void testJresultInd();

	static void testJTPHandle();

	static void testQuery();

	static void testQueryWithTimeout();

	static void testJfault();

	static cute::suite makeACSJTPJobR3ASuite();

	//Job Object
	static ACS_JTP_Job_R3A *J1;

};

#endif
