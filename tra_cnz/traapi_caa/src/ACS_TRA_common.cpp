/*
 *
 * NAME: ACS_TRA_common.cpp
 *
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 * All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Telecom AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 *  This file contains the definitions of all methods
 *  of class ACS_TRA_common
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0024
 *
 * AUTHOR
 * 	2010-05-05 XYV XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	       	20100505	XGIOPAP		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *	  									"APG50 Tracing and MKTR"
 *
 */

// Module Include Files
#include "ACS_TRA_common.h"
#include <acs_aeh_evreport.h>
#include <ace/ACE.h>


// Extern declaration
char ACS_TRA_common::sarea[MAX_CHARS_COM];
char* ACS_TRA_common::s = ACS_TRA_common::sarea;

struct trace_mem *shmp;
int 			 pid;

/*
 * Class Name: ACS_TRA_common
 * Method Name: ACS_TRA_common
 * Description: Class constructor
 * Used: This method is not used.
 */
ACS_TRA_common::ACS_TRA_common() {
	// Auto-generated constructor stub
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: get_token
 * Description: Fetch parameter from dialog command of trautil
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_basic_set, ACS_TRA_lib::create_task
 *       in ACS_TRA_lib.cpp file and in Main in ACS_TRA_util.cpp file.
 */
char* ACS_TRA_common::get_token()
{
	char* token;

	token = ACE_OS::strtok(s, " 	\n");

	if (token != NULL)
		s =  (char *) (token + ACE_OS::strlen(token) + 1);

	return (token);
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: unget_token
 * Description: -
 * Used: This method is not used.
 */
void ACS_TRA_common::unget_token(char * token)
{
	s--;
	*s = ' ';
	s = token;
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: get_char
 * Description: Find a specified char in the string of the dialog command
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_basic_set, ACS_TRA_lib::create_trace_set
 *       in ACS_TRA_lib.cpp file.
 */
char ACS_TRA_common::get_char()
{
	char c;
	while ((c = *s) == ' ') s++;
	s++;
	return (c);
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: unget_char
 * Description: Reset the pointer to dialog command
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_basic_set, ACS_TRA_lib::create_trace_set
 *       in ACS_TRA_lib.cpp file.
 */
void ACS_TRA_common::unget_char()
{
	s--;
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: readTraceStatus
 * Description: Read the trace ON or OFF parameter
 * 				from Trace_Status file
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::ACS_TRA_lib in ACS_TRA_lib.cpp file.
 */
char* ACS_TRA_common::readTraceStatus(char* b)
{
	FILE * 	file_read;
	FILE * 	file_write;
	char  	buffer[10];

	// Reset the On_Off element
	ACE_OS::memset(buffer, 0, sizeof(buffer));

	/*
	 * Open the file parameter and check if
	 * the value allows the trace (trace is ON)
	 */
	file_read = ACE_OS::fopen (TRACESTATUSPATH, "r");

	if (file_read == NULL)
	{
		// The file doesn't exists, then try to create it
		file_write = ACE_OS::fopen (TRACESTATUSPATH, "w");

		if (file_write == NULL)
		{
			// close the file
			//ACE_OS::fclose (file_read);
			b = const_cast<char*>("ERR");
			//ACE_OS::fprintf(stdout, "Unable to read/write the Trace Status from: %s\n", TRACESTATUSPATH);
			acs_aeh_evreport EvReport;
			EvReport.sendEventMessage(
									"libacs_tra",
									8000,
									"EVENT",
									"Unable to read/write the Trace Status from: /cluster/etc/ap/acs/tra/conf/Trace_Status",
									"EVENT",
									"-",
									"An error occurred reading/writing the file Trace_Status",
									"-"
									);

			return (b);
		}

		// write the value ON in the file
		ACE_OS::fputs(TRACESTATUSON, file_write);

		// close the file
		ACE_OS::fclose (file_write);

		// re-opens the file to read the right parameter
		file_read = ACE_OS::fopen (TRACESTATUSPATH, "r");
	}

	// Read the value
	if (ACE_OS::fgets (buffer , sizeof(buffer) - 1, file_read) != NULL)
	{
		//close the file
		ACE_OS::fclose (file_read);

		// copy the value which return
		ACE_OS::strcpy(b, buffer);
	}

	return (b);
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: writeTraceStatus
 * Description: Switch the trace to ON or OFF and set
 * 				the data in the Trace_Status file.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
int ACS_TRA_common::writeTraceStatus(int on_off)
{
	FILE * 	file_param;

	/*
	 * Fetch the value of parameter ON_OFF
	 * from the file Trace_Status and check it
	 * if is switched
	 */
	file_param = ACE_OS::fopen(TRACESTATUSPATH, "w");

	if (file_param == NULL)
	{
		//ACE_OS::fprintf(stdout, "Unable to open the file: %s\n", TRACESTATUSPATH);
		acs_aeh_evreport EvReport;
		EvReport.sendEventMessage(
								"libacs_tra",
								8000,
								"EVENT",
								"Unable to open the file: /cluster/etc/ap/acs/tra/conf/Trace_Status",
								"EVENT",
								"-",
								"An error occurred reading the file Trace_Status",
								"-"
								);

		return (1);
	}
	else
	{
		// Check the value typed in command line
		if (on_off == 1)
		{
			ACE_OS::fputs(TRACESTATUSON, file_param);
			ACE_OS::fprintf(stdout, "Trace is switched %s\n", TRACESTATUSON);
			ACE_OS::fprintf(stdout, "To use the new setting first stop and then restart all applications\n");
			ACE_OS::fprintf(stdout, "that uses trace.\n");
		}
		else // on_off = 0
		{
			ACE_OS::fputs(TRACESTATUSOFF, file_param);
			ACE_OS::fprintf(stdout, "Trace is switched %s\n", TRACESTATUSOFF);
			ACE_OS::fprintf(stdout, "To use the new setting first stop and then restart all applications\n");
			ACE_OS::fprintf(stdout, "that uses trace.\n");
		}
	}

	// close the file
	ACE_OS::fclose (file_param);

	return (0);
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: readMemorySize
 * Description: Read the Memory Size parameter
 *   			from Memory_Size file
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::ACS_TRA_lib in ACS_TRA_lib.cpp file.
 */
char* ACS_TRA_common::readMemorySize(char* newsize)
{
	FILE * 	file_read;
	FILE * 	file_write;
	char  	buffer[20];

	// Reset the On_Off element
	ACE_OS::memset(buffer, 0, sizeof(buffer));

	/*
	 * Open the file Memory_Size and read the
	 * new value of the Memory Size
	 */
	file_read = ACE_OS::fopen (MEMORYSIZEPATH, "r");

	if (file_read == NULL)
	{
		// The file doesn't exists, then try to create it
		file_write = ACE_OS::fopen (MEMORYSIZEPATH, "w");

		if (file_write == NULL)
		{
			// close the file
			//ACE_OS::fclose (file_read);
			newsize = NULL;
			//ACE_OS::fprintf(stdout, "Unable to read/write the Memory Size from: %s\n", MEMORYSIZEPATH);
			acs_aeh_evreport EvReport;
			EvReport.sendEventMessage(
									"libacs_tra",
									8000,
									"EVENT",
									"Unable to read/write the Memory Size from: /cluster/etc/ap/acs/tra/conf/Memory_Size",
									"EVENT",
									"-",
									"An error occurred reading/writing the file Memory_Size",
									"-"
									);

			return (newsize);
		}

		// write the value ON in the file
		ACE_OS::fputs("1000", file_write);

		// close the file
		ACE_OS::fclose (file_write);

		// re-opens the file to read the right parameter
		file_read = ACE_OS::fopen (MEMORYSIZEPATH, "r");
	}

	// Read the value
	if (ACE_OS::fgets (buffer , sizeof(buffer) - 1, file_read) != NULL)
	{
		//close the file
		ACE_OS::fclose (file_read);

		// copy the value which return
		ACE_OS::strcpy(newsize, buffer);
	}

	return (newsize);
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: writeMemorySize
 * Description: Change the memory size for the shared memory.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
int ACS_TRA_common::writeMemorySize(char* newsize)
{
	FILE * 	file_param;

	/*
	 * Fetch the value of parameter Memory_Size
	 * from the file Memory_Size
	 */
	file_param = ACE_OS::fopen(MEMORYSIZEPATH, "w");

	if (file_param == NULL)
	{
		//ACE_OS::fprintf(stdout, "Unable to open the file: %s\n", MEMORYSIZEPATH);
		acs_aeh_evreport EvReport;
		EvReport.sendEventMessage(
								"libacs_tra",
								8000,
								"EVENT",
								"Unable to open the file: /cluster/etc/ap/acs/tra/conf/Memory_Size",
								"EVENT",
								"-",
								"An error occurred reading the file Memory_Size",
								"-"
								);
		return (1);
	}
	else
	{
		ACE_OS::fputs(newsize, file_param);
		ACE_OS::fprintf(stdout, "Trace memory size changed to %s kbytes\n", newsize);
		ACE_OS::fprintf(stdout, "To use the new setting first stop and then restart all applications\n");
	}

	// close the file
	ACE_OS::fclose (file_param);

	return (0);
}

/*
 * Class Name: ACS_TRA_common
 * Method Name: isDigit
 * Description: Check if exists a character in the string
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_lib.cpp file
 *       and in the main program in ACS_TRA_util.cpp file
 */
int ACS_TRA_common::isDigit(char *str)
{
	int i = 0;

	while(str[i])
	{
		if(!isdigit(str[i]))
			return 1;	// Is not a digit

		i++;
	}

	return 0;	// Is a digit
}
