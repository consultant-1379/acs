#include <acs_usa_eventGen.h>

char *eventGenClass::CBAeventList[CBA_EVTLIST_ARRAY_SIZE][2]= {
		{ (char*)"EVENT NUMBER", (char*)"EVENT DESCRIPTION					SUPPORTED SEVERITY"},
		{ (char*)"------------", (char*)"-----------------					------------------"},
		{ (char*)"8701", (char*)"Faulty Ethernet Bonding				CLEAR(0)	  MAJOR(4) CRITICAL(5)"},
		{ (char*)"8702", (char*)"System Disk Replication Communication Failure		CLEAR(0)	  MAJOR(4)"},
		{ (char*)"8703", (char*)"System Disk Replication In-Consistant			CLEAR(0) MINOR(3)"},
		{ (char*)"8704", (char*)"System Disk Usage Threshold reached.			CLEAR(0) 	  MAJOR(4)"},
		{ (char*)"8705", (char*)"Memory Usage Threshold reached				CLEAR(0) 	  MAJOR(4)"},
		{ (char*)"8706", (char*)"AP Process Cleanup Failed				CLEAR(0) MINOR(3) MAJOR(4)"},
		{ (char*)"8707", (char*)"AP Process Failed To Start				CLEAR(0) MINOR(3) MAJOR(4)"},
		{ (char*)"8708", (char*)"HA Node Unavailable					CLEAR(0) MINOR(3) MAJOR(4)"},
		{ (char*)"8709", (char*)"BRF Cancel Restore Backup failed			CLEAR(0)          MAJOR(4)"},
		{ (char*)"8710", (char*)"MDF Detected Model Error                		CLEAR(0)          MAJOR(4)"},
		{ (char*)"8711", (char*)"Number of Files in FileGroup Exceeded         		CLEAR(0)          MAJOR(4)"},
		{ (char*)"6022", (char*)"AP Process Stopped					CLEAR(0) MINOR(3) MAJOR(4)"},
		{ (char*)"8771", (char*)"Valid Certificate Not Available			CLEAR(0)                   CRITICAL(5)"},
		{ (char*)"8772", (char*)"Certificate about to expire				CLEAR(0) MINOR(3) MAJOR(4)"},
		{ (char*)"8773", (char*)"Auto Enrollment of certificate Failed			CLEAR(0) MINOR(3)"}
	};	

char *eventGenClass::nonCBAeventList[6][2]= {
		{ (char*)"EVENT NUMBER", (char*)"EVENT DESCRIPTION"},
		{ (char*)"------------", (char*)"-----------------"},
		{ (char*)"8796", (char*)"Zombie process threshold limit reached"},
		{ (char*)"8797", (char*)"CPU load threshold limit reached."},
		{ (char*)"8798", (char*)"AP process stopped."},
		{ (char*)"8799", (char*)"Illegal logon attempts."},
	};	

eventGenClass::eventGenClass() {

	this->eventId=0;
	this->Freq=0;
	this->Num=0;
	this->IsValidEventId=false;
	this->IsCBAevent=false;
}

void eventGenClass::usage( const char *progname) {

	printf("\n\t%s - Generate Events (CBA/non CBA)\n", progname);

	printf("\nSYNOPSIS\n");
	printf("\t%s [options]...\n", progname);

	printf("\nDESCRIPTION\n");
	printf("\t-h|--help\n");
	printf("\t-e|--event	<<event-number>>\n");
	printf("\t-s|--set	<<severity>>\n");
	printf("\t-n|--num	<<num-of-events-to-generate>>\n");
	printf("\t-t|--frequency	<<frequency-in-secs>>\n");
	printf("\t-l|--list\n");
	
	printf("\nExample:\n");
	printf("\t%s --event 8701 --set 5 \n", progname);
	printf("\t%s --event 8702 --set 4 --num 5 --frequency 0.5\n", progname);
	printf("\t%s --list\n", progname);
	printf("\n");
}

void eventGenClass::printEventList(void){

	printf("\nSupported CBA Event List:\n");
	printf("------------------------\n");
	printf("\n");

	for (int i=0; i<CBA_EVTLIST_ARRAY_SIZE; i++) {
		if (i<2)
			printf("%s \t\t %s \n", this->CBAeventList[i][0], this->CBAeventList[i][1]);
		else
			printf("%s \t\t\t %s \n", this->CBAeventList[i][0], this->CBAeventList[i][1]);
	}
	printf("\n");
	printf("\nSupported NonCBA Event List:\n");
	printf("------------------------\n");
	printf("\n");
	for (int i=0; i<6; i++) {
		if (i<2)
			printf("%s \t\t %s \n", this->nonCBAeventList[i][0], this->nonCBAeventList[i][1]);
		else
			printf("%s \t\t\t %s \n", this->nonCBAeventList[i][0], this->nonCBAeventList[i][1]);
	}
	printf("\n");
}

bool eventGenClass::scanCBAEventList(int eventId){

	for (int i=2; i<CBA_EVTLIST_ARRAY_SIZE; i++) {
		if (eventId == (atoi(this->CBAeventList[i][0]))) {
			this->IsCBAevent=true;
			return true;
		}	
	}	
	return false;
}


bool eventGenClass::scanNonCBAEventList(int eventId){
	
	for (int i=2; i<6; i++) {
		if (eventId == (atoi(this->nonCBAeventList[i][0]))) {
			return true;
		}	
	}	
	return false;
}

void eventGenClass::applyParamValid(const char* progname){

	/* Event number is mandtory. Check if we have valid event-no.
	 */
	if (!this->IsValidEventId) {
		fprintf(stderr, "Event-Id not exist. Try '%s --help' for more information.\n", progname);
		exit(EXIT_FAILURE);
	}

	/* Check supported alarm severity to the entered severity */
	if(this->eventId == 8701){
		if (this->severity != SA_NTF_SEVERITY_CLEARED
		 && this->severity != SA_NTF_SEVERITY_MAJOR
		 && this->severity != SA_NTF_SEVERITY_CRITICAL) {	
			fprintf(stderr, "Event severity not supported. Try 'eventGen --list' for supported severites.\n");
			exit(EXIT_FAILURE);
		}	
		return;
	}
	if(this->eventId == 8702 || this->eventId == 8704 || this->eventId == 8705 || this->eventId == 8709 || this->eventId == 8710|| this->eventId == 8711){
		if (this->severity != SA_NTF_SEVERITY_CLEARED
		 && this->severity != SA_NTF_SEVERITY_MAJOR) {
			fprintf(stderr, "Event severity not supported. Try 'eventGen --list' for supported severites.\n");
			exit(EXIT_FAILURE);
		}	
		return;
	}
	if(this->eventId == 8703 || this->eventId == 8773){
		if (this->severity != SA_NTF_SEVERITY_CLEARED
		 && this->severity != SA_NTF_SEVERITY_MINOR) {
			fprintf(stderr, "Event severity not supported. Try 'eventGen --list' for supported severites.\n");
			exit(EXIT_FAILURE);
		}	
		return;
	}
        if(this->eventId == 8771){
                if (this->severity != SA_NTF_SEVERITY_CLEARED
                 && this->severity != SA_NTF_SEVERITY_CRITICAL) {
                        fprintf(stderr, "Event severity not supported. Try 'eventGen --list' for supported severites.\n");
                        exit(EXIT_FAILURE);
                }
                return;
        }

	if((this->eventId == 8796) || (this->eventId == 8797) || (this->eventId == 8798) || (this->eventId == 8799)) {
		return;
	}

	if (this->severity != SA_NTF_SEVERITY_CLEARED
	 && this->severity != SA_NTF_SEVERITY_MINOR 	
	 && this->severity != SA_NTF_SEVERITY_MAJOR) {
		fprintf(stderr, "Event severity not supported. Try 'eventGen --list' for supported severites.\n");
		exit(EXIT_FAILURE);
	}	
}

void eventGenClass::msec_sleep(int time_in_msec) {

	struct timeval tv;

	tv.tv_sec = time_in_msec/1000;
	tv.tv_usec = ((time_in_msec) % 1000) * 1000;

	while(select (0 ,0 ,0 ,0 ,&tv) != 0)
		if (errno == EINTR)
			continue;
}

void eventGenClass::generateEvent(void) {

	/* generate CBA events using ntfsend
	 */

	/* Check if we have more than 1 events to fire
	 */

	int iterations=0;

	fprintf(stdout, "Generating Event-%d %d :",this->eventId, iterations+1);
	fflush(stdout);
	do{
		if (this->IsCBAevent)		
			this->genCBAevent();
		else
			this->genNonCBAevent();
	
		fprintf(stdout, " ...done\n");
		fflush(stdout);
		iterations++;	
		if ( iterations != this->Num && this->Num != 0){
			fprintf(stdout, "Generating Event-%d %d :",this->eventId, iterations+1);
			fflush(stdout);
			msec_sleep(this->Freq);
		}	
	}while (iterations<this->Num);
}

void eventGenClass::genCBAevent(void) {

	/* declare the locals */
	const char *ClassId;
	const char *notfnObj;
	const char *notfngObj;
	const char *addText;
	SaNtfProbableCauseT probableCause;
	SaNtfEventTypeT eventType;

	/* Fill notifying object and notifcation object */
	//notfnObj="safApp=ERIC-APG,safSi=eventGen_MO";
	notfnObj="safSi=eventGen_MO,safApp=ERIC-APG";
	//notfnObj="eventGen_MO";
	notfngObj="safApp=safAmfService";

	/* Fill evenType */
	eventType=SA_NTF_ALARM_PROCESSING;

	/* Fill probableCause */
	probableCause=SA_NTF_SOFTWARE_ERROR;

	switch(this->eventId) {

			case 8701:

				addText= (char*)"Faulty Ethernet Bonding.";
				ClassId="193,50994,2";
				break;	
			case 8702:
					
				addText= (char*)"System Disk Replication Communication Failure.";
				ClassId="193,50994,4";
				break;
			case 8703:

				addText= (char*)"System Disk Replication In-Consistant.";
				ClassId="193,50994,6";
				break;
			case 8704:

				//addText= (char*)"System Disk Usage Threshold reached.";
				addText= (char*)"Disk usage above threshold major 90% (/ (91%), / (91%), /var/ftp/ACS (91%)).";
				ClassId="193,50994,3";
				break;
			case 8705:

				//addText= (char*)"Memory Usage Threshold reached.";
				addText= (char*)"Memory usage above 20%";
				ClassId="193,50994,5";
				break;
			case 8706:
		
				addText= (char*)"AP Process Cleanup Failed.";
				ClassId="18568,2,3";
				break;
			case 8707:
				
				addText= (char*)"AP Process Failed To Start.";
				ClassId="18568,2,2";
				break;
			case 8708:
				
				addText= (char*)"HA Node Unavailable.";
				ClassId="193,12960,1";
				break;
			case 8709:
				
				addText= (char*)"BRF Cancel Restore Backup failed.";
				ClassId="193,32278,1";
				break;
			case 8710:
				
				addText= (char*)"COM SA, MDF Detected Model Error.";
				ClassId="193,12960,2";
				break;
			case 8711:
				
				addText= (char*)"File Management, Number of Files in FileGroup Exceeded.";
				ClassId="193,2,1";
				break;
			case 6022:
				addText= (char*)"AP Process Stopped.";
				ClassId="18568,2,5";
				break;
				//CERTM changes
			case 8771:
				addText= (char*)"Certificate Management, a Valid Certificate is Not Available";
				ClassId="193,106,1";
				break;
			case 8772:
				addText= (char*)"Certificate Management, the Certificate is to Expire";
				ClassId="193,106,2";
				break;
			case 8773:
				addText = (char*)"Certificate Management, Automatic Enrollment Failed";
				ClassId="193,106,3";
				break;
			default:
					/* supposed to get caught in the validation part.
					 * anyway, exit with error
					 */
					fprintf(stderr, "Event-Id not exist. Try 'eventGen --list' For Supported Event Numbers.\n");
					exit (EXIT_FAILURE);
	}	

	/* Form the command to launch */

	char command_string[512];
	sprintf(command_string, "%s -c \"%s\" -n \"%s\" -N \"%s\" -a \"%s\" -s %d -p %d -e %d 1>/dev/null", NTFSEND, ClassId, notfnObj, notfngObj, addText, this->severity, probableCause, eventType);
		
	/* launch ntfsend */
	int rCode;
	rCode=system(command_string);
	if (rCode == -1) {
		fprintf(stderr, "Failed to launch:[%s] \n", command_string);
		exit(EXIT_FAILURE);
	}
	
	rCode=WEXITSTATUS(rCode);
	if (rCode != 0) {
		fprintf(stderr, "[%s] execution failed. rCode[%d] \n", command_string, rCode);
		exit(EXIT_FAILURE);
	}
}


void eventGenClass::genNonCBAevent(void) {

	const char *addText;
	int priority;
	switch(this->eventId) {

			case 8796:

				addText= (char*)"Zombie process threshold limit reached";
				priority = LOG_ALERT;
				syslog(priority, "%s", addText);
				break;	
			case 8797:
					
				addText= (char*)"CPU load threshold limit reached.";
				priority = LOG_ALERT;
				syslog(priority, "%s", addText);
				break;
			case 8798:

				//addText= (char*)"AP process stopped.";
				priority = LOG_ERR;
				addText= (char*)"monitord: dhcpd: Task terminated. Received signal 15";
				syslog(priority, "%s", addText);
				priority = LOG_INFO;
				addText= (char*)"monitord: dhcpd: Task started";
				syslog(priority, "%s", addText);
				break;
			case 8799:

				//addText= (char*)"Illegal Logon Attempts";
				addText= (char*)"error: PAM: Authentication failure for user1 from 172.16.45.21";
				priority = LOG_ERR | LOG_AUTHPRIV;
				syslog(priority, "%s", addText);
				addText= (char*)"Failed password for user1 from 172.16.45.21 port 1212 ssh2";
				priority = LOG_INFO | LOG_AUTHPRIV;
				syslog(priority, "%s", addText);
				addText= (char*)"error: PAM: Authentication failure for user1 from 172.16.45.21";
				priority = LOG_ERR | LOG_AUTHPRIV;
				syslog(priority, "%s", addText);
				addText= (char*)"Failed keyboard-interactive/pam for user1 from 172.16.45.21 port 1212 ssh2";
				priority = LOG_INFO | LOG_AUTHPRIV;
				syslog(priority, "%s", addText);
					
				break;
			default:
					/* supposed to get caught in the validation part.
					 * anyway, exit with error
					 */
					fprintf(stderr, "Event-Id not exist. Try 'eventGen --list' For Supported Event Numbers.\n");
					exit (EXIT_FAILURE);
	}	
	
}

int main(int argc, char *argv[]) {

	struct option long_options[] = {
		{"event", required_argument, 0, 'e'},
		{"set", required_argument, 0, 's'},
		{"num", required_argument, 0, 'n'},
		{"frequency", required_argument, 0, 't'},
		{"help", no_argument, 0, 'h'},
		{"list", no_argument, 0, 'l'},
		{0, 0, 0, 0}
	};

	int c;

	if ( argc < 2) {
		fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	eventGenClass eGen;

	while(true) {
		c = getopt_long(argc, argv, "s:n:t:he:l", long_options, NULL);

		if (c == -1)		
			break;

		switch(c) {
		
			case 'e':
				eGen.eventId=atoi(optarg);
				if (!eGen.scanCBAEventList(eGen.eventId)) {
					if ( !eGen.scanNonCBAEventList(eGen.eventId)) {
						fprintf(stderr, "Event-Id not exist. Try '%s --list' For Supported Event Numbers.\n", argv[0]);
						exit (EXIT_FAILURE);
					}
				}
				eGen.IsValidEventId=true;
				break;

			case 's':
				eGen.severity=atoi(optarg);
				break;

			case 'n':
				eGen.Num=atoi(optarg);	
				if (eGen.Num < 1) {
					fprintf(stderr, "Provide possitive Integer with -n option.Try '%s --help' for more information\n", argv[0]);
					exit(EXIT_FAILURE);
				}
				break;
			case 't':
				eGen.Freq=atof(optarg);
				if (eGen.Freq <= 0) {
					fprintf(stderr, "Provide Possitive Integer with -t option.Try '%s --help' for more information.\n", argv[0]);
					exit(EXIT_FAILURE);
				}
				/* convert seconds to milli seconds
				 */
				eGen.Freq *= 1000;
				break;

			case 'h':
				eGen.usage(basename(argv[0]));
				exit(EXIT_SUCCESS);
				break;

			case 'l':
				eGen.printEventList();
				exit(EXIT_SUCCESS);
				break;	

			default:
				fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
				exit(EXIT_FAILURE);	
		}
	}
	eGen.applyParamValid(basename(argv[0]));

	/* All good so far, proceed with the event fire
	 */

	eGen.generateEvent();
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

