/*=================================================================== */
/**
   @file  acs_aca_msd_cmd_stub.h 

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

#ifndef ACS_ACA_MSDCMD_STUB_H
#define ACS_ACA_MSDCMD_STUB_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <inttypes.h>

#include <iostream>
#include <list>
#include <string>
#include <set>

#include "ACS_CS_API.h"
#include "ACS_DSD_Server.h"
#include "ACS_DSD_Client.h"

#include "acs_aca_msdls_api_struct.h"
#include "acs_aca_msg_director.h"
#include "acs_aca_command.h"
#include "acs_aca_structured_request_builder.h"
#include "acs_aca_structured_response_builder.h"

using namespace std;
class ACS_ACA_StructuredRequestBuilder; 
/*=====================================================================
                          DEFINE DECLARATION SECTION
==================================================================== */

#define NO_OF_AP		16
#define FIRST_AP		2001
#define LAST_AP			2016
//#define RESPBUFSIZE		ACS_DSD_MAXBUFFER
#define RESPBUFSIZE		128
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief       ACS_ACA_MSDCMD_Stub
                   ACS_ACA_MSDCMD_Stub class implments the real client side of the msdls command.
                   This class is part of a Command Design pattern maked by following classes:
                   the class ACS_ACA_MSDCMD_msdls is a "concrete command" of the pattern;
                   the class ACS_ACA_Command is the "command" interface of th pattern;
                   the class ACS_ACA_MSDCMD_Stub is the "receiver". It implements the command;
                   the class ACS_ACA_CommandInvoker is the "invoker". It makes the callbacks to the commands.

 */
/*=================================================================== */
/**
 */
class ACS_ACA_MSDCMD_Stub
{
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
	ACS_ACA_MSDCMD_Stub(void);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_MSDCMD_Stub(void);
/*=================================================================== */
/**
   @brief       msdls

   @param       opt

   @param       ap

   @param       APid

   @param       cpsource

   @param       ms

   @param       data

 */
/*=================================================================== */
	void msdls(unsigned short opt,char* ap,unsigned int APid,char* cpsource,char* ms,char* data);
/*=================================================================== */
/**
   @brief       ALLAP
 */
/*=================================================================== */
	bool ALLAP;
/*=================================================================== */
/**
   @brief       CURRENTAP
 */
/*=================================================================== */
	bool CURRENTAP;
/*=================================================================== */
/**
   @brief       SPECIFICAP
 */
/*=================================================================== */
	bool SPECIFICAP;
/*=================================================================== */
/**
   @brief       MCP
 */
/*=================================================================== */
	bool MCP;
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
/**
   @brief       get_All_AP_sysId_in_Cluster

   @param       apSysIds

   @return      void
 */
/*=================================================================== */
	void get_All_AP_sysId_in_Cluster(std::set<uint16_t> & ap_system_ids);
/*=================================================================== */
/**
   @brief       GetCPName

   @param       id

   @param       name

   @return      int 
 */
/*=================================================================== */
	int GetCPName(unsigned short id,string& name);
/*=================================================================== */
/**
   @brief       PrintList

   @param       req

   @return      void
 */
/*=================================================================== */
	void PrintList(aca_msdls_req_t* req);
/*=================================================================== */
/**
   @brief       DeleteList

   @return      void
 */
/*=================================================================== */
	void DeleteList();
/*=================================================================== */
/**
   @brief       GetCPID

   @param       cpname

   @param       id

   @return      void
 */
/*=================================================================== */
	void GetCPID(char* cpname,CPID& id);
/*=================================================================== */
/**
   @brief       ErrorCheck

   @param       rsp

   @return      void
 */
/*=================================================================== */
	void ErrorCheck(aca_msdls_resp_t* rsp);
/*=================================================================== */
/**
   @brief       lprint
 */
/*=================================================================== */
	std::list<char*> lprint;

};

#endif
