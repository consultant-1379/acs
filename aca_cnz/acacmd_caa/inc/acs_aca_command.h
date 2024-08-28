/*=================================================================== */
/**
   @file  acs_aca_command.h 

   @brief Header file for aca module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       28/01/2013   XHARBAV   Initial Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ACA_COMMAND_H
#define ACS_ACA_COMMAND_H

/*=====================================================================
                          DEFINE DECLARATION SECTION
==================================================================== */
#define GENERAL_EXCEPTION          1
#define USAGE_EXCEPTION            2
#define USAGE_EXCEPTION_NEW        12
#define MSNAME_EXCEPTION           3
#define DATA_FILE_EXCEPTION        4
#define COMMIT_EXCEPTION           5
#define STAT_FILE_EXCEPTION        6
#define EOF_FILE_EXCEPTION         7
#define SEND_EXCEPTION             8
#define RECV_EXCEPTION             9
#define CS_EXCEPTION55             55
#define CS_EXCEPTION56             56
#define APNAME_EXCEPTION           113
#define OPTION_EXCEPTION           116
#define CONNECT_EXCEPTION          117
#define CS_EXCEPTION118            118

namespace ACS_ACA_MSDLS_ErrorMessages {
	const char * const AP_NOT_DEFINED_ERROR_MESSAGE             = "AP is not defined";
	const char * const CP_NOT_DEFINED_ERROR_MESSAGE             = "CP is not defined";
	const char * const ILLEGAL_OPTION_ERROR_MESSAGE             = "Illegal option in this system configuration";
	const char * const INCORRECT_USAGE_SCP_SYSTEM_ERROR_MESSAGE = "Incorrect usage\nUsage:\nmsdls [-ap apidentity] [-m messagestore]\nmsdls -a [-ap apidentity] -m messagestore";
	const char * const INCORRECT_USAGE_MCP_SYSTEM_ERROR_MESSAGE = "Incorrect usage\nUsage:\nmsdls [-ap apidentity] [-cp cpname] [-m messagestore]\nmsdls -a [-ap apidentity] -cp cpname -m messagestore";
	const char * const MESSAGE_STORE_NOT_FOUND_ERROR_MESSAGE    = "Message Store not found";
	const char * const INTERNAL_PROGRAM_FAULT_FORMAT            = "Internal program error";
	const char * const ERROR_WHEN_EXECUTING_ERROR_MESSAGE       = "Error when executing";
	const char * const DATA_FILE_ERROR_ERROR_MESSAGE            = "Data file error";
	const char * const COMMIT_FILE_ERROR_ERROR_MESSAGE          = "Commit file error";
	const char * const STAT_FILE_ERROR_ERROR_MESSAGE            = "Statistics file error";
	const char * const EOF_FILE_ERROR_ERROR_MESSAGE             = "Unexpected EOF in data file";
	const char * const SEND_FAILED_ERROR_MESSAGE                = "Error when sending a message to the server";
	const char * const RECV_FAILED_ERROR_MESSAGE                = "Error when receiving a message from the server";
	const char * const CS_CONNECT_ERROR_MESSAGE                 = "Error when connecting to CS";
	const char * const CS_ERROR_ERROR_MESSAGE                   = "Error when retrieving data from CS";
	const char * const CONNECT_FAILED_ERROR_MESSAGE             = "Unable to connect to the server";
}


/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief         ACS_ACA_Command
                    ACS_ACA_Command class is an abstract class which is an interface to msdls command.
                    This class is part of a Command Design pattern maked by following classes:
                    the class ACS_ACA_MSDCMD_msdls is a "concrete command" of the pattern;
                    the class ACS_ACA_Command is the "command" interface of th pattern;
                    the class ACS_ACA_MSDCMD_Stub is the "receiver". It implements the command;
                    the class ACS_ACA_CommandInvoker is the "invoker". It makes the callbacks to the commands.

 */
/*=================================================================== */
class ACS_ACA_Command
{
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief     Constructor for  ACS_ACA_Command class.

   @param     argc

   @param     argv
 */
/*=================================================================== */ 
	ACS_ACA_Command(int argc, char* argv []);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_Command();
/*=================================================================== */
/**
   @brief     launch
 
   @return    void 
 */
/*=================================================================== */
	void launch ();
/*=====================================================================
                          PROTECTED DECLARATION SECTION
==================================================================== */
protected:
/*=================================================================== */
/**
   @brief     launch

   @return    void
 */
/*=================================================================== */
	virtual void	parse() = 0; 
/*=================================================================== */
/**
   @brief     execute

   @return    void
 */
/*=================================================================== */
	virtual void	execute() = 0;
/*=================================================================== */
/**
   @brief     usage

   @return    void
 */
/*=================================================================== */
	virtual void	usage() = 0;
/*=================================================================== */
/**
   @brief     argc_
 */
/*=================================================================== */
	int		argc_;
/*=================================================================== */
/**
   @brief     argv_
 */
/*=================================================================== */
	char**	argv_;
};

#endif
