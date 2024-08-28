#ifndef ACS_USA_EVENTCREATE_H
#define ACS_USA_EVENTCREATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <syslog.h>
#include <fcntl.h>


class eventCreateClass {

	private:

	protected:

	public:
		int numOfZombies;
		int durationMin;
		int percentMemUsage;
		int percentDiskUsage;
		char *deamonName;
		char *userName;

		eventCreateClass();
		~eventCreateClass(){};

		void usage (const char *progname);
		void createCPUHog( int durationInSec); 
		int getCPUCount();
		void createZombies( int numOfZombies); 
		void cleanZombies(char *zombieProcName);
		void stopDeamon( char *deamonName);
		void startDeamon( char *deamonName);
		void hogCPU( int durationInSec, int cpuID);
		void makeIllegalLogonAttempts(char *userName);
		void makeThresholdDiskUsage(char *dirPath, int pecentage);
		void makeThresholdMemoryUsage(int percentage);
		void createProcess(long long memSize);

		//void createNonCBAevent(void);
};

#endif /* ACS_USA_EVENTCREATE_H */


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

