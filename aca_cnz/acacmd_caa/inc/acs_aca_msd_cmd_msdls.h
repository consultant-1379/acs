/*=================================================================== */
/**
   @file   acs_aca_msd_cmd_msdls.h

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
#ifndef ACS_ACA_MSDCMD_MSDLS_H
#define ACS_ACA_MSDCMD_MSDLS_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aca_command.h"
#include "acs_aca_msd_cmd_stub.h"
#include <acs_aca_command_invoker.h>
#include "acs_aca_msdls_api_struct.h"
#include <ACS_CS_API.h> 
#include <ACS_TRA_trace.h>
using namespace std;
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief        ACS_ACA_MSDCMD_msdls
                     ACS_ACA_MSDCMD_msdls class implments the client part of the msdls command
                     which is used to list Message Store informations.
                     This class is part of a Command Design pattern maked by following classes:
                     the class ACS_ACA_MSDCMD_msdls is a "concrete command" of the pattern;
                     the class ACS_ACA_Command is the "command" interface of th pattern;
                     the class ACS_ACA_MSDCMD_Stub is the "receiver". It implements the command;
                     the class ACS_ACA_CommandInvoker is the "invoker". It makes the callbacks to the commands.

 */
/*=================================================================== */
class ACS_ACA_MSDCMD_msdls : public ACS_ACA_Command
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
   @brief       Constructor for ACS_ACA_MSDCMD_msdls class.

   @param       argc

   @param       argv

   @param       stub

 */
/*=================================================================== */
  ACS_ACA_MSDCMD_msdls (int argc, char* argv [], ACS_ACA_MSDCMD_Stub* stub);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
  virtual ~ACS_ACA_MSDCMD_msdls();
/*=================================================================== */
/**
   @brief       isMultipleCpSystem

   @return      bool

 */
/*=================================================================== */
  bool isMultipleCpSystem();
/*=================================================================== */
/**
   @brief       ap
 */
/*=================================================================== */
  char* ap;
/*=================================================================== */
/**
   @brief       cp
 */
/*=================================================================== */
  char* cp;
/*=================================================================== */
/**
   @brief       ms
 */
/*=================================================================== */
  char* ms;
/*=================================================================== */
/**
   @brief       data
 */
/*=================================================================== */
  char* data;
/*=================================================================== */
/**
   @brief       option
 */
/*=================================================================== */
  unsigned short option;
/*=================================================================== */
/**
   @brief       apid
 */
/*=================================================================== */
  unsigned int apid;
 // void launch ();
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
/**
   @brief       m_stub
 */
/*=================================================================== */
  ACS_ACA_MSDCMD_Stub* m_stub;
/*=================================================================== */
/**
   @brief       parse

   @return      void
 */
/*=================================================================== */
  void parse();	
/*=================================================================== */
/**
   @brief       execute

   @return      void
 */
/*=================================================================== */
  void execute();	
/*=================================================================== */
/**
   @brief       usage

   @return      void 
 */
/*=================================================================== */
  void usage();
/*=================================================================== */
/**
   @brief       CheckApName

   @param       apname

   @param       apid

   @return      void 
 */
/*=================================================================== */
  void CheckApName(char* apname,unsigned int& apid);
/*=================================================================== */
/**
   @brief       CheckData

   @param       data

   @return      bool
 */
/*=================================================================== */
  bool CheckData(const char* data);

  char * to_upper(char * str);

/*=================================================================== */
/**
   @brief       argc_
 */
/*=================================================================== */
  int argc_;
/*=================================================================== */
/**
   @brief       argv_
 */
/*=================================================================== */
  char** argv_;
  
};

#endif
