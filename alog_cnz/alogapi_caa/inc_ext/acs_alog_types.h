/*
 * acs_alog_types.h
 *
 *  Created on: May 24, 2011
 *      Author: xanttro
 */

#ifndef ACS_ALOG_TYPES_H_
#define ACS_ALOG_TYPES_H_


////////////////////////////////////////////////////////
// 	type declaration for Data Unit Type
///////////////////////////////////////////////////////

typedef enum
{
not_defined 	   		= 0,  // Anything
MMLcmd 		       		= 3,  // MML command
MMLprt 		       		= 4,  // MML printout
MMLcmdpar 	       		= 5,  // MML command parameter
ApplData 	       		= 6,  // Application data
MMLalp 		       		= 7,  // MML alarm printout
Unixcmd 	       		= 8,  // Unix command/process
Unixprt 	       		= 9,  // Unix printout
SecLogData 	       		= 10, // Security log data
MMLCmdInconsistent    	= 11, // MML command causing inconsistency
MMLCmdLog             	= 12, // MML command for logging
MMLCmdLogInconsistent	= 13, // MML command for logging, with inconsistency
MMLPrtInconsistent   	= 14,  // Result printout indicating inconsistency
MMLMediationCommand		= 15
} ACS_ALOG_DataUnitType;


////////////////////////////////////////////////////////
// 	type declaration for return type
///////////////////////////////////////////////////////

typedef enum
		{
			acs_alog_ok	= 0,						//no error
			acs_alog_generic_error = -1,			//generic error
			acs_alog_file_desc_error = -99,			//invalid file descriptor
			acs_alog_max_size_message_error = -100,	//message max size limit error
			acs_alog_write_error = -101,			//write error
			acs_alog_date_format_error=-102			//date/time format error

		} acs_alog_returnType;


/*//========================================================================================
//	Type declarations for parameters
//========================================================================================
typedef char* acs_alog_data;
typedef char* acs_alog_userName;
typedef char* acs_alog_applicationName;
typedef char* acs_alog_remoteHost;
typedef char* acs_alog_localHostname;
typedef char* acs_alog_success;
typedef char* acs_alog_pid;*/



#endif /* ACS_ALOG_TYPES_H_ */
