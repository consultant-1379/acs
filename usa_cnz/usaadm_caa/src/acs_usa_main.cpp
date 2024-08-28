/*===================================================================
 *  
 *	@file   acs_usa_main.cpp
 *    
 *	@brief
 *      
 *       
 *	@version 1.0.0
 *         
 *          
 *	HISTORY
 *    
 *  
 * 
 *
 *	PR           DATE      INITIALS    DESCRIPTION
 * --------------------------------------------------------------------
 *	N/A       DD/MM/YYYY     NS       Initial Release
 *==================================================================== */

/*====================================================================
 * 		DIRECTIVE DECLARATION SECTION
 *=================================================================== */

#include <syslog.h>
#include <getopt.h>
#include "acs_usa_adm.h"
#include "acs_usa_hamanager.h"
#include "ace/ACE.h"
#include "acs_usa_logtrace.h"
#include <boost/filesystem.hpp>
const char* const SWMVERSION="/cluster/storage/system/config/apos/swm_version";
bool isSWM20();
bool is_swm_2_0 = true;
const char* gAppName="acs_usad";

// forward declarations
int parse_command_line(int argc, char **argv);
ACE_THR_FUNC_RETURN run_usa_daemon(void *);

// enable/disable trace
//static unsigned int __tracemask=CATEGORY_ALL;
static unsigned int __tracemask=0;
int nohaflag = 0;

ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR **argv) {



	// parse the command line
	if(parse_command_line(argc, argv) < 0) {
		fprintf(stderr, "USAGE: acs_usad [-c] [--noha] \n");
		return -1;
	}	
 	is_swm_2_0 = isSWM20();
	if(nohaflag)
		return (long)run_usa_daemon(0);

	// HA Implementation
	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
	int rCode=0;
	usaHAClass* haObj;
	haObj=0;
	ACE_NEW_NORETURN(haObj, usaHAClass("acs_usad", "root"));

	if (!haObj){
		syslog(LOG_ERR, "ha-thread: haObj Creation FAILED");
		rCode=-1;
		return rCode;
	}	

	// Initialize the traceing sub-system first
	char tracefile[256]={'\0'};
	snprintf(tracefile, sizeof(tracefile),TRCE_LOG_DIR "%s.trace", gAppName);
	if (acs_usa_logtrace_init(gAppName, tracefile, __tracemask) != 0) {
		syslog(LOG_ERR, "usa-main: Failed to initialize the trace subsystem");
		return -1;
	}	

	errorCode = haObj->activate();
	
	switch (errorCode) {
		case ACS_APGCC_HA_FAILURE: {

			syslog(LOG_ERR, "ha-thread: acs_usad, HA Activation Failed!");
			rCode=-2;
			break;
		}
		case ACS_APGCC_HA_FAILURE_CLOSE: {

			syslog(LOG_ERR, "ha-thread: acs_usad, HA Application Failed to Gracefullly closed!");
			rCode=-2;
			break;
		}			
		case ACS_APGCC_HA_SUCCESS: {
			
			syslog(LOG_INFO, "ha-thread: acs_usad, HA Application Gracefully closed!");
			rCode=0;
			break;
		}
		default: {
			
			syslog(LOG_ERR, "ha-thread: acs_usad, Unknown HA Application error detected!");
			rCode=-3;
			break;
		}			
	
	}		 	
	delete haObj;
	return rCode;
}

ACE_THR_FUNC_RETURN run_usa_daemon( void *) {

	ACE_THR_FUNC_RETURN rCode=0;
	ACS_USA_Adm *usaAdmObj;
	usaAdmObj=0;

	// Initialize the traceing sub-system first
	char tracefile[256]={'\0'};
	snprintf(tracefile, sizeof(tracefile),TRCE_LOG_DIR "%s.trace", gAppName);
	if (acs_usa_logtrace_init(gAppName, tracefile, __tracemask) != 0) {
		syslog(LOG_ERR, "usa-main: Failed to initialize the trace subsystem");
		return (ACE_THR_FUNC_RETURN)-1;
	}	

	syslog(LOG_INFO, "usa-main-class: Starting USA from the command line");
	ACE_NEW_NORETURN(usaAdmObj, ACS_USA_Adm());
	if (0 == usaAdmObj){
		syslog(LOG_ERR, "usa-main-class: new ACS_USA_Adm() Failed");
		rCode=(void*)-1;
	}else{
		int resAdm = usaAdmObj->start(0,0);
		if (resAdm < 0){
			syslog(LOG_ERR, "usa-main-class: Failed to start ACS_USA_Adm");
		}else{
			usaAdmObj->wait();
		}	

		delete usaAdmObj;
	}	
	return rCode;
}

int parse_command_line(int argc, char **argv) {

	int index;
	int c;
	int noha_opt_cnt = 0;

	static struct option long_options[] =
	{		
		/* These options set a flag. */
		{"noha", no_argument, & nohaflag, 1},
		{0, 0, 0, 0}
	};

	/* getopt_long stores the option index here. */
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "c", long_options, & option_index)) != -1)
		switch (c) {
			
			case 0: {
					if(strcmp(long_options[option_index].name, argv[optind-1] + 2)) {
						fprintf(stderr,"usa-main-class: Unrecognized option '%s'\n",argv[optind-1]);
						return -1;
					}	
					/* found --noha option */
					if(noha_opt_cnt > 0){
						fprintf(stderr,"usa-main-class: duplicated long option 'noha'\n");
						return -1;
					}
					++noha_opt_cnt;
					break;
				}		
						
						
			case '?':
					return -1;
			
			default:
					abort ();
		}				
	
	if((optind > 1) && !strcmp(argv[optind-1],"--")){
		fprintf(stderr,"usa-main-class: Unrecognized option '%s'\n",argv[optind-1]);
		return -1;
	}
	if(optind < argc){
		for (index = optind; index < argc; index++)
			printf ("usa-main-class: Incorrect usage, found non-option argument '%s'\n", argv[index]);
		return -1;
	}	
	
	return 0;
}



bool isSWM20()
{
 if((boost::filesystem::exists(boost::filesystem::path(SWMVERSION)))== true)
 {
        return true;
 }
 else {
        return false;
 }
}


//******************************************************************************
//
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

