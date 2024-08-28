/*=================================================================== */
/**
   @file   acs_aca_command_invoker.h

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
#ifndef ACS_ACA_COMMANDINVOKER_H
#define ACS_ACA_COMMANDINVOKER_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aca_command.h"
#include <ACS_TRA_trace.h>
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief       ACS_ACA_CommandInvoker
                  ACS_ACA_CommandInvoker class implments the program interface for the commands.
                  This class is part of a Command Design pattern maked by following classes:
                  the class ACS_ACA_MSDCMD_msdls is a "concrete command" of the pattern;
                  the class ACS_ACA_Command is the "command" interface of th pattern;
                  the class ACS_MSDLS_Stub is the "receiver". It implements the command;
                  the class ACS_ACA_CommandInvoker is the "invoker". It makes the callbacks to the commands.

 */
/*=================================================================== */
class ACS_ACA_Command;
class ACS_ACA_CommandInvoker
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
   @brief        Constructor for class ACS_ACA_CommandInvoker.

   @param        cmd

 */
/*=================================================================== */
	ACS_ACA_CommandInvoker(ACS_ACA_Command *cmd);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_CommandInvoker(void);
/*=================================================================== */
/**
   @brief        msdls

   @return       void

 */
/*=================================================================== */

	void msdls();
private:
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
	ACS_ACA_Command *cmd_;
};

#endif
