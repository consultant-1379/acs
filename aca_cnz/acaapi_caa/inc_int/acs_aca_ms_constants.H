/*=================================================================== */
/**
   @file acs_aca_ms_constants.H 

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
   N/A       29/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACAMS_Constants_H
#define ACAMS_Constants_H
#if 0
#include <string>
char site_name[10] = "cp0ex";
char site_name_alias[10] = "axe1";
//****************************************************************
// Configuration data
//
// Files and directories
//****************************************************************
const std::string ACAMS_NameOfCommitFile = "commitFile";
const std::string ACAMS_NameOfDataFileDir = "dataFiles";
const std::string ACAMS_NameOfStatisticsFile = "statisticsFile";

// Message store capacity
const int ACAMS_MaxIP_Addresses = 10;
const int ACAMS_MaxMsgSize = 65536;
const int ACAMS_MaxMTAP_Channels = 32;
const int ACAMS_MaxNumberOfFiles = 2048;
const int ACAMS_MaxSizeOfTransactionData = 125000;
const int ACAMS_MaxWindowSize = 32;

//****************************************************************
// Constants that should never change
//****************************************************************
const int ACAMS_MsgHeaderSize = 12;
const int ACAMS_MaxFileNameLength = 31; //260;
const int ACAMS_MaxMSIP_PrimitiveSize = 64;
const int ACAMS_MaxSizeOfCommitData = ACAMS_MsgHeaderSize + ACAMS_MaxFileNameLength + 1 + ACAMS_MaxSizeOfTransactionData;
const int ACAMS_MTAP_Overhead = 8;
const int ACAMS_FileOverhead = 20;

// This marks the end of a data file
const int ACAMS_Eof_Mark_Length = 12;
const unsigned char ACAMS_Eof_Mark[12] = {255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0};

// This marks the end of a message in the data files
const int ACAMS_Msg_End_Mark_Length = 4;
const unsigned char ACAMS_Msg_End_Mark[4] = {0xFF, 0xFE, 0xFD, 0xFC};
#endif
#endif
