#ifndef ACS_USA_EVENTGEN_H
#define ACS_USA_EVENTGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <syslog.h>
#include <saNtf.h>

#define NTFSEND "/usr/bin/ntfsend"
#define SA_NTF_SEVERITY_CLEARED 0
#define SA_NTF_SEVERITY_MINOR 3
#define SA_NTF_SEVERITY_MAJOR 4
#define SA_NTF_SEVERITY_CRITICAL 5
#define CBA_EVTLIST_ARRAY_SIZE 17
#define NON_CBA_EVTLIST_ARRAY_SIZE 6 

class eventGenClass {

	private:

	protected:

	public:
		int eventId;
		int severity;
		float Freq;
		int Num;
		bool IsValidEventId;
		bool IsCBAevent;

		eventGenClass();
		~eventGenClass(){};

		static char *CBAeventList[CBA_EVTLIST_ARRAY_SIZE][2];
		static char *nonCBAeventList[6][2];
		void usage (const char *progname);
		void printEventList(void);
		bool scanCBAEventList(int eventId);
		bool scanNonCBAEventList(int eventId);

		void launchNtfSend(const char* command);
		void applyParamValid(const char *progname);
		void generateEvent(void);
		void genCBAevent(void);
		void genNonCBAevent(void);
		void msec_sleep(int );
};

#endif /* ACS_USA_EVENTGEN_H */


//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

