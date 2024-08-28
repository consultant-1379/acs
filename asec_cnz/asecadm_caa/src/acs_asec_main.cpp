/*===================================================================
 *
 *      @file   acs_asec_main.cpp
 *
 *      @brief
 *
 *
 *      @version 1.0.0
 *
 *
 *      HISTORY
 *
 *
 *
 *
 *      PR           DATE      INITIALS    DESCRIPTION
 * --------------------------------------------------------------------
 *      N/A       DD/MM/YYYY     NS       Initial Release
 *==================================================================== */

/*====================================================================
 *              DIRECTIVE DECLARATION SECTION
 *=================================================================== */
#include "ace/ACE.h"
#include <getopt.h>
#include "acs_asec_adm.h"
#include "acs_asec_hamanager.h"


// forward declarations
int parse_command_line(int argc, char **argv);
ACE_THR_FUNC_RETURN run_asec_daemon(void *);

int nohaflag = 0;
static long srv_state=0;

ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR **argv) {

	// parse the command line
	if(parse_command_line(argc, argv) < 0) {
		fprintf(stderr, "USAGE: acs_asec_csadmd [ -d|--noha -s|--srv-state <state> ]\n");
		return -1;
	}

	if(nohaflag)
		return (long)run_asec_daemon(0);


	/* HA mode of running the service */
	syslog(LOG_INFO, "Starting 'csadm service' in ha-mode");
	int rCode;		
	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
	asecHAClass *haObj;
	haObj=0;
      	ACE_NEW_NORETURN(haObj, asecHAClass("acs_asec_csadmd","root"));
       	if (!haObj) {
       		syslog(LOG_ERR, "CSADM:  haObj Creation FAILED");
               	rCode=-2;
               	return rCode;
	}

	haObj->log.Write("Starting 'csadm service' in ha-mode", LOG_LEVEL_INFO);
 	errorCode = haObj->activate();

       	switch (errorCode) {

		case ACS_APGCC_HA_FAILURE: {
               		syslog(LOG_ERR, "acs_asec_csadmd, HA Activation Failed!");
                       	rCode=-1;
                       	break;
                }
                case ACS_APGCC_HA_FAILURE_CLOSE: {
               		syslog(LOG_ERR, "acs_asec_csadmd, HA Application Failed to Gracefullly closed!");
                       	rCode=-2;
                       	break;
		}
		case ACS_APGCC_HA_SUCCESS: {
			syslog(LOG_ERR, "acs_asec_csadmd, HA Application Gracefully closed!");
			rCode=0;
			break;
		}
		default: {
			syslog(LOG_ERR, "acs_asec_csadmd, Unknown Application error detected!");
			rCode=-2;
			break;
		}

	}
	delete haObj;
	return rCode;
}

ACE_THR_FUNC_RETURN run_asec_daemon( void *) {

        ACE_THR_FUNC_RETURN rCode=0;
        ACS_ASEC_Adm *asecAdmObj;
        asecAdmObj=0;

        syslog(LOG_INFO, "Starting 'csadm service' from command-line");
        ACE_NEW_NORETURN(asecAdmObj, ACS_ASEC_Adm());
        if (0 == asecAdmObj){
                syslog(LOG_ERR, "asec-main-class: new ACS_ASEC_Adm() Failed");
               // rCode=-1;
		return (ACE_THR_FUNC_RETURN)-1;
        }else{
		asecAdmObj->log.Write("Starting 'csadm service' from command-line", LOG_LEVEL_INFO);
                int resAdm = asecAdmObj->start(srv_state,0);
                if (resAdm < 0){
                        syslog(LOG_ERR, "asec-main-class: Failed to start CSADM");
			asecAdmObj->log.Write("Starting 'csadm service' from command-line", LOG_LEVEL_INFO);
                }else{
                        asecAdmObj->wait();
                }

                delete asecAdmObj;
        }
        return rCode;
}

int parse_command_line(int argc, char **argv) {

	int index;
        int c;
        int noha_opt_cnt = 0;
	int srv_opt_cnt =0;

        static struct option long_options[] =
        {
                /* These options set a flag. */
                {"noha", no_argument,NULL, 'd'},
		{"srv-state",required_argument, NULL, 's' },
                {0, 0, 0, 0}
        };

	// if number of arguments is one - assume ha-mode
	if (argc == 1)
		return 0;
		

        /* getopt_long stores the option index here. */
        int option_index = 0;
        while ((c = getopt_long(argc, argv, "ds:", long_options, & option_index)) != -1)
                switch (c) {

                        case 'd': {
                                        /* found --noha option */
                                        if(noha_opt_cnt > 0){
                                                fprintf(stderr,"asec-main-class: duplicated long option 'noha'\n");
                                                return -1;
                                        }
                                        ++noha_opt_cnt;
					nohaflag=1;
                                        break;
                                }
			case 's': {
					
					if (srv_opt_cnt > 0){
						fprintf(stderr, "asec-main-class: duplicated long option 'srv-state'\n");
						return -1;
					}	
					srv_state=strtol(optarg, NULL, 10);
					if ((srv_state != 1) && (srv_state != 2)){
						fprintf(stderr, "asec-main-class: parse_command_line() - Invalid service state.(valid: 1-active, 2-passive)\n");
						return -1;
					}	
					srv_opt_cnt++;
					break;
			}	

                        case '?':
                                        return -1;

                        default:
                                        abort ();
                }

	if (noha_opt_cnt == 0 || srv_opt_cnt == 0){
		fprintf(stderr, "asec-main-class: Incorrect usage\n");
		return -1;
	}	

        if((optind > 1) && !strcmp(argv[optind-1],"--")){
                fprintf(stderr,"asec-main-class: Unrecognized option '%s'\n",argv[optind-1]);
                return -1;
        }
        if(optind < argc){
                for (index = optind; index < argc; index++)
                        printf ("asec-main-class: Incorrect usage, found non-option argument '%s'\n", argv[index]);
                return -1;
        }

        return 0;
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

