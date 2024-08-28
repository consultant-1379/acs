#include <acs_usa_eventCreate.h>
#include <signal.h>
#include <sched.h>
#include <sys/statvfs.h>
#include <iostream>
#include <sys/sysinfo.h>
volatile sig_atomic_t keep_running = 1;



eventCreateClass::eventCreateClass() {
	percentDiskUsage = 0;
	percentMemUsage = 0;
}

void eventCreateClass::usage( const char *progname) {

	printf("\n\t%s - Create CBA/Non CBA Events \n", progname);

	printf("\nSYNOPSIS\n");
	printf("\t%s [options]...\n", progname);

	printf("\nDESCRIPTION\n");
	printf("\t-h|--help\n");
	printf("\t-z|--createZombies	<<number>>\n");
	printf("\t-l|--cleanZombies\n");
	printf("\t-s|--stopDaemon		<<LOTC DaemonName>>\n");
	printf("\t-t|--startDaemon	<<LOTC DaemonName>>\n");
	printf("\t-c|--cpuHog		<<Duration In Min (1 to 15 min)>>\n");
	//printf("\t-i|--illegalLogin	<<UserName>>\n");
	printf("\t-i|--illegalLogin	\n");
	printf("\t-d|--chewDisk 		<<percentage (1 to 100)>>\n");
	printf("\t-m|--chewMem	 	<<percentage (1 to 100)>>\n");
	
	printf("\nExample:\n");
	printf("\t%s --createZombies 4 \n", progname);
	printf("\t%s --stopDaemon dhcpd \n", progname);
	printf("\t%s --startDaemon dhcpd \n", progname);
	printf("\t%s --chewDisk 85\n", progname);
	printf("\t%s --chewMem 85\n", progname);
	printf("\n");
}

extern "C" void alarmHandler( int sig) {

	(void) sig;
	fprintf(stderr, "\nAlarm generated PID = %d", getpid());
	keep_running = 0;
}

extern "C" void intHandler( int sig) {
	(void) sig;
	keep_running = 0;
}

int eventCreateClass::getCPUCount() {

	cpu_set_t cpuSet;
	CPU_ZERO(&cpuSet);
	sched_getaffinity(0, sizeof(cpuSet), &cpuSet);
	int count = 0;
	for(int i = 0; i < (int)sizeof(cpu_set_t); i++) {
		if(CPU_ISSET(i, &cpuSet))
			count++;
	}
	return count;
}

void eventCreateClass::hogCPU( int durationInSec, int cpuID) {

	signal(SIGALRM, alarmHandler);
        alarm(durationInSec);
	cpu_set_t cpuMask;
	CPU_ZERO(&cpuMask);
	CPU_SET(cpuID, &cpuMask);
	sched_setaffinity(0, sizeof(cpuMask), &cpuMask);
	for( ; keep_running; ) { /* Busy loop */

	}
	exit(EXIT_SUCCESS);

}

void eventCreateClass::createCPUHog( int durationInSec) {

	int numOfProc;
	int i;
	numOfProc = getCPUCount();
	if(numOfProc > 0) {
		for(i = 0; i < numOfProc; i++) {
			int pid = fork();
			if(pid == 0) {
				hogCPU(durationInSec, i);
			}
		}
	}
}

void eventCreateClass::createZombies( int numOfZombies) {
	int pid;
	pid = fork();
	if(pid == 0)
	{
		for(int i = 0; i < numOfZombies; i++) {
		
			pid = fork();
			if(pid == 0) {
				exit(EXIT_SUCCESS);
			}
		}
		pause();
	}
}

void eventCreateClass::cleanZombies(char *zombieProcName)
{
	char command_buffer[1024];
	int retCode;

	signal(SIGTERM, SIG_IGN);
	sprintf(command_buffer, "killall %s", zombieProcName);
	retCode = system(command_buffer);
	if(retCode == -1) {
		fprintf(stderr, "Failed to launch [%s]", command_buffer);
		exit(EXIT_FAILURE);
	}
	retCode=WEXITSTATUS(retCode);
	if(retCode != 0) {
		fprintf(stderr, "Failed to execute [%s]", command_buffer);
		exit(EXIT_FAILURE);
	}
}

void eventCreateClass::stopDeamon( char *deamonName) {
	char command_buffer[256];
	int retCode;
	FILE *fp;

	sprintf(command_buffer, "pidof %s 2>&1", deamonName);
	fp = popen(command_buffer, "r");
	if(fp != NULL)
	{
		char *ret;
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
	 	ret = fgets(buffer, sizeof(buffer), fp);	 
		if(ret)
		{
			int pid = atoi(buffer);
			if(pid == 0){
				fprintf(stderr, "No LOTC daemon exists with name: [%s]\n", deamonName);
				exit(EXIT_FAILURE);
			}
				
		}
		else {
			fprintf(stderr, "No daemon exists %s\n", deamonName);
			exit(EXIT_FAILURE);
		}
	}
	sprintf(command_buffer, "killall %s", deamonName);
	retCode = system(command_buffer);
	if(retCode == -1) {
		fprintf(stderr, "Failed to launch [%s]", command_buffer);
		exit(EXIT_FAILURE);
	}
	retCode=WEXITSTATUS(retCode);
	if(retCode != 0) {
		fprintf(stderr, "Failed to execute [%s]", command_buffer);
		exit(EXIT_FAILURE);
	}
	
}
void eventCreateClass::startDeamon( char *deamonName) {

	char command_buffer[1024];
	int retCode;
	sprintf(command_buffer, "/usr/lib/systemd/system/%s", deamonName);
	if (access(command_buffer, F_OK) == -1) {
		 fprintf(stderr, "NO Daemon Exits With Name: [%s]\n", deamonName);
		 exit(EXIT_FAILURE);
	}
	sprintf(command_buffer, "/opt/ap/apos/bin/servicemgmt/servicemgmt start %s", deamonName);
	retCode = system(command_buffer);
	if(retCode == -1) {
		fprintf(stderr, "Failed to launch [%s]\n", command_buffer);
		exit(EXIT_FAILURE);
	}
	retCode=WEXITSTATUS(retCode);
	if(retCode != 0) {
		fprintf(stderr, "Failed to execute [%s]\n", command_buffer);
		exit(EXIT_FAILURE);
	}

}
void eventCreateClass::makeIllegalLogonAttempts(char *userName)
{
	(void)userName;
	printf("\n\t Open a new ssh terminal use a valid username with wrong password three times\n");
}

void eventCreateClass::makeThresholdDiskUsage(char *dirPath, int percentage)
{
	struct statvfs stData;
	int consumeBlocks;
	int fillBlocks;
	int usedBlocks;
	int retCode = 0;
	char cmdBuf[1024];
	

	if((statvfs(dirPath, &stData)) < 0 ) {
		printf("statvfs failed  %s:\n", dirPath);
	} 
	else {
		//printf("Disk path %s: \n", dirPath);
		//std::cout << "\tBlock size: " << stData.f_bsize <<std::endl;
		//std::cout << "\tTotal blocks: " << stData.f_blocks << std::endl;
		//std::cout << "\tFree blocks: " << stData.f_bfree << std::endl;
		//std::cout << "\tAvail blocks: " << stData.f_bavail << std::endl;

		consumeBlocks = (stData.f_blocks * percentage)/100;
		usedBlocks = stData.f_blocks - stData.f_bavail;
	       	fillBlocks = consumeBlocks - usedBlocks;

		fprintf(stderr, "\nWriting %d blocks each of size %lu bytes to %s/DeleteIt_JunkFile", fillBlocks, stData.f_bsize, dirPath);
		fprintf(stderr, "\nPlease wait................!\n");
		sprintf(cmdBuf, "dd if=/dev/zero of=%s/DeleteIt_JunkFile bs=%lu count=%d", dirPath, stData.f_bsize, fillBlocks);
		retCode = system(cmdBuf);
		if(retCode == -1) {
			fprintf(stderr, "Failed to launch [%s]", cmdBuf);
			exit(EXIT_FAILURE);
		}
		retCode=WEXITSTATUS(retCode);
		if(retCode != 0) {
			fprintf(stderr, "Failed to execute [%s]", cmdBuf);
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "Remove the file %s/DeleteIt_JunkFile After Event is Observed\n", dirPath);
	}
}
void eventCreateClass::createProcess(long long memSize)
{
	
	int pid = fork();
	if(pid == 0)
	{

		void *ptr = malloc(memSize);
		if(ptr == NULL) {
			fprintf(stderr, "malloc failed with size = %llu Bytes\n", memSize);
		}
		else
		{
			memset(ptr, 0, memSize);
			/*while(keep_running)
			{
				memset(ptr, 0, memSize);
				fprintf(stderr, "touched allocated mem\n");
				sleep(1);
			}*/
			pause();
			free(ptr);
		}
		exit(0);
	}
	else if(pid < 0)
	{
		fprintf(stderr, "fork failed to create process\n");
	}
	else
	{
		return;
	}
}

void eventCreateClass::makeThresholdMemoryUsage(int percentage)
{
	//std::cout << "percentage = " << percentage << std::endl;

	struct sysinfo memInfo;
	memset(&memInfo, 0, sizeof(memInfo));

	signal(SIGINT, intHandler);
	int retCode = sysinfo (&memInfo);
	if(retCode == 0)
	{

		//std::cout<< "\tTotal RAM Size : "memInfo.totalram << std::endl
		//std::cout<< "\tTotal Swap Size : " << memInfo.totalswap << std::endl
		//std::cout << "\tFree RAm : " << memInfo.freeram << std::endl
		//std::cout << "\tFree Swap : " << memInfo.freeswap << std::endl
		//std::cout << memInfo.mem_unit << std::endl;
		unsigned long long totalUsedMem = ((memInfo.totalram - memInfo.freeram) + (memInfo.totalswap - memInfo.freeswap));
		unsigned long long sizeToMake = (memInfo.totalram + memInfo.totalswap)* (unsigned long long)percentage/100;

		
		long long allocSize = 0LL ;
		allocSize = sizeToMake - totalUsedMem;
		//std::cout << "Alloc Size " << allocSize << std::endl;
		long long allocSizeForEachProcess = allocSize/32;
		//std::cout << "allocSizeForEachProcess =  " << allocSizeForEachProcess << std::endl;
		if(allocSize > 0)
		{
			for(int i = 0; i < 32; i++)
			{
				createProcess(allocSizeForEachProcess);
			}
		/*	fprintf(stderr, "malloc failed with size = %llu Bytes\n", allocSizeForEachProcess);
					void *ptr = malloc(allocSizeForEachProcess);
					if(ptr == NULL) {
						fprintf(stderr, "malloc failed with size = %llu Bytes\n", allocSizeForEachProcess);
					}
					else
					{
							memset(ptr, 0, allocSizeForEachProcess);
						pause();
						free(ptr);
					}*/
			fprintf(stderr, "Press 'CTRL + C' to exit after oserving the alarm\n");
			pause();

		} else {
			fprintf(stderr, "Already more than the given [ %d ] is consumed\n", percentage);
		}
	}
	else
	{
		fprintf(stderr, "sysinfo Failed\n");
	}
}

int main(int argc, char *argv[]) {

	struct option long_options[] = {
		{"createZombies", required_argument, 0, 'z'},
		{"cleanZombies", no_argument, 0, 'l'},
		{"stopDaemon", required_argument, 0, 's'},
		{"startDaemon", required_argument, 0, 't'},
		{"cpuHog", required_argument, 0, 'c'},
//{"illegalLogin", required_argument, 0, 'i'},
		{"illegalLogin", no_argument, 0, 'i'},
		{"chewDisk", required_argument, 0, 'd'},
		{"chewMem", required_argument, 0, 'm'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	int c;

	if ( argc < 2) {
		fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	eventCreateClass eGen;

	while(true) {
		c = getopt_long(argc, argv, "s:t:z:c:i:d:m:hl", long_options, NULL);

		if (c == -1)		
			break;

		switch(c) {
		
			case 'z':
				eGen.numOfZombies = atoi(optarg);
				eGen.createZombies(eGen.numOfZombies);
				break;

			case 'l':
				eGen.cleanZombies(argv[0]);
				break;
				
			case 's':
				eGen.deamonName = optarg;
				eGen.stopDeamon(eGen.deamonName);
				break;
			case 'd':
				eGen.percentDiskUsage = atoi(optarg);
				if(eGen.percentDiskUsage > 0 && eGen.percentDiskUsage <= 100) {
					eGen.makeThresholdDiskUsage((char *)"/var/ftp/ACS", eGen.percentDiskUsage);
				}
				else {
					fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
					exit(EXIT_FAILURE);	
				}
				break;
				
			case 'm':
				eGen.percentMemUsage = atoi(optarg);
				//std::cout<< "eGen.percentMemUsage = " << eGen.percentMemUsage << std::endl;
				if(eGen.percentMemUsage > 0 && eGen.percentMemUsage <= 100) {
					eGen.makeThresholdMemoryUsage(eGen.percentMemUsage);
				}
				else {
					fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
					exit(EXIT_FAILURE);	
				}
				

				break;
				
			case 't':
				eGen.deamonName = optarg;
				eGen.startDeamon(eGen.deamonName);
				break;

			case 'c':
				eGen.durationMin = atoi(optarg);
				if(eGen.durationMin > 0 && eGen.durationMin <= 15){
					eGen.createCPUHog(eGen.durationMin * 60);
				}
				else {
					fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
					exit(EXIT_FAILURE);	
				}
				break;
			case 'i':
				eGen.userName = optarg;
				eGen.makeIllegalLogonAttempts(NULL);
				//fprintf(stderr, "User Name = %s\n", eGen.userName);
				break;

			case 'h':
				eGen.usage(basename(argv[0]));
				exit(EXIT_SUCCESS);
				break;

			default:
				fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
				exit(EXIT_FAILURE);	
		}
	}
	return 0;
}


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

