/*
 * ACS_APGCC_Texts.H
 *
 *  Created on: Apr 6, 2010
 *      Author: root
 */

#ifndef ACS_APGCC_TEXTS_H_
#define ACS_APGCC_TEXTS_H_

// All defined texts must be given an unique value, and
// the corresponding text string must be inserted in the
// file ACS_APGCC_Util.cpp. Make sure to update the No_Of_Texts
// counter also.


typedef enum {

   // The first 0 to 79 values are reserved for MML standard
   // text strings. These are mapped directly towards the value
   // of MCS_MML_CMDRESULT. See MCS_MML_Defs.H.
   // IMPORTANT: DO NOT CHANGE THE GIVEN NUMBER FOR THESE TEXTS.

   /**** begin of CP/MML constants; do not change given numbers! ***/

   MTXT_EXECUTED = 0,           // The command was executed
   MTXT_ORDERED = 1,            // The command was ordered, a result printout is expected
   MTXT_CONFIRMEXPECTED = 2,    // The command needs to be confirmed with a semicolon before execution
   MTXT_NOTACCEPTED = 3,        // Probably never received; See subcodes instead
   MTXT_PARTLYEXECUTED = 4,     // The command was partly executed
   MTXT_INHIBITED = 5,          // The command was inhibited due to a faulty response to MTXT_CONFIRMEXPECTED
   MTXT_CMDUNKNOWN = 6,         // Nu such command available
   MTXT_CMDRESTRICTED = 7,      // Command is resticted
   MTXT_FUNCTIONBUSY = 8,       // The function is at the moment busy, try again later
   MTXT_FORMATERROR = 9,        // The command was not correctly specified (Subcode to MTXT_NOTACCEPTED)
   MTXT_UNRESVALUE = 10,        // A command parameter was given a faulty value (Subcode to MTXT_NOTACCEPTED)
   MTXT_FAULTCODE = 11,         // Command returned a fault code (Subcode to MTXT_NOTACCEPTED)
   MTXT_END = 12,               // A printout ended with END. MTXT_EXECUTED is received instead of MTXT_END.
   MTXT_BUFEXCEEDED = 13,       // The command was not correctly specified (Subcode to MTXT_NOTACCEPTED)
   MTXT_TIMEOUT = 14,
   MTXT_SYNTAXFAULT = 15,       // The command was not correctly specified (Subcode to MTXT_NOTACCEPTED)
   MTXT_PMRBLOCKTOLONG = 16,    // The command was not correctly specified (Subcode to MTXT_NOTACCEPTED)
   MTXT_EOT = 17,               // An answer printed was ended with EOT (EOT DURING PRINTOUT)
   MTXT_NOTEXECUTED = 18,
   MTXT_FAULTINTERRUPT = 19,
   MTXT_DEVICEERROR = 20,
   MTXT_FILENOTFOUND = 21,
   MTXT_EOTREQUESTED = 22,
   MTXT_FULLDEVICEQUEUE = 23,
   MTXT_VOLUMEERROR = 24,
   MTXT_FILENAMEERROR = 25,
   MTXT_CODEERROR = 26,
   MTXT_ILLOGICAL = 27,
   MTXT_CANCELLED = 28,
   MTXT_BLOCKSIZEERROR = 29,
   MTXT_DATAERROR = 30,
   MTXT_TAPEEND = 31,
   MTXT_NEXTTAPE = 32,
   MTXT_BUFCONGESTION = 33,
   MTXT_WAIT = 34,
   MTXT_DUMPRESTRICTED = 35,
   MTXT_NOTLOGGED = 36,
   MTXT_CMDEXECUTED = 37,
   MTXT_CMDNOTEXECUTED = 38,
   MTXT_FILEERROR = 39,
   MTXT_MEDIAEND = 40,
   MTXT_CLOGNOTAVAILABLE = 41,
   MTXT_CMDRESTRICTEDTOGRP = 50,
   MTXT_INTERRUPTEDBYSR = 54,          // Command execution was interrupted by a system restart
   MTXT_SYSTEMRESTARTED = 55,
   MTXT_INTERRUPTEDBYFR = 57,          // Command execution was ended by a forlopp release
   MTXT_REENTRYTOCMD = 58,
   MTXT_INTERRUPTEDCMD = 61,
   MTXT_AUTHFAILURE = 62,              // User is not authorized to execute command
   MTXT_TOOMANYATTEMPTS = 63,
   MTXT_CPNOTOBTAINABLE = 64,          // "CP NOT OBTAINABLE"
   MTXT_CPSTANDBYNOTOBTAINABLE = 65,   // "CP STANDBY NOT OBTAINABLE"
   MTXT_INCOMPLETECPGROUP = 66,
   //MTXT_CONNECTINTERRUPT; Defined below

   /**** end of CP/MML constants ***/

   // More texts
   MTXT_ENTERAPLOCEXIT		=73,  // "YOU MAY NOW ENTER: APLOC, OR EXIT COMMAND"
   MTXT_PASSWORDEXPIRED    = 74, // "PASSWORD HAS EXPIRED"
   MTXT_NEWPASSWORD        = 75, // "NEW PASSWORD:"
   MTXT_CONFIRMPASSWORD    = 76, // "CONFIRM NEW PASSWORD:"
   MTXT_INVALIDPASSWORD    = 77, // "INVALID PASSWORD"
   MTXT_ENTERIODEVICENO    = 78, // "ENTER IO DEVICE NUMBER:"
   MTXT_IODEVICEOCCUPIED   = 79, // "IO DEVICE IS OCCUPIED"
   MTXT_USERCODE           = 80, // "USERCODE:"
   MTXT_PASSWORD           = 81, // "PASSWORD:"
   MTXT_ASKCONTINUE        = 82, // "Continue ? <"
   MTXT_CONNECTIONTIMEOUT  = 83, // "CONNECTION TIMED OUT"
   MTXT_CONNECTINTERRUPT   = 84, // "CONNECTION INTERRUPTED"
   MTXT_MMLSERVERFAULTY    = 85, // "MML SERVER UNREACHABLE"
   MTXT_PDSEVENTHEADER     = 86, // "IO MML, PRINTOUT DESTINATION FAULT"
   MTXT_LOGGEDOFF          = 87, // "LOGGED OFF"
   MTXT_ASKCPSIDE          = 88, // "Connect to CP-EX?  yes/[no]: "
   MTXT_YOUMAYENTER        = 89, // "YOU MAY NOW ENTER: APLOC, PTCOI, OR EXIT COMMAND"
   MTXT_ADDINFO            = 90, // "Additional Information:"
   //free 91,
   MTXT_MMLSRV_NOTDEFINED  = 92, // "MML SERVER NOT PROPERLY DEFINED"
   MTXT_RESTARTFILEREADING = 93, // "Use option -l to restart file reading"
   MTXT_LOWONMEMORY        = 94, // "REJECTED DUE TO LACK OF AVAILABLE AP SYSTEM MEMORY"
   MTXT_NESTEDAPLOC			= 95, // "NESTED APLOC"
   MTXT_REDUCEDCPGROUPWARNING = 96, // "WARNING: ACTIONS TOWARDS INCOMPLETE CP GROUP COULD AFFECT DATA CONSISTENCY"
   MTXT_DOMAIN             = 97, // "DOMAIN: "

   MTXT_IODEVICERESERVED   = 98, // "IO DEVICE IS RESERVED"


   // Dummy empty text

   MTXT_DUMMY              = 99, // ""


   // More general MCS texts should start at 100 !!

   MTXT_AIAP_EVENTHEADER   = 100,   // "AP IO LOG SERVER FAULT"
   MTXT_MMLSRV_EVENTHEADER = 101,   // "IO MML SERVER FAULT"
   MTXT_V24_EVENTHEADER    = 102,   // "AP ALI FAULT"
   MTXT_ALEC_EVENTHEADER   = 103,   // "EXTERNAL ALARM SERVER FAULT"
   MTXT_ALDA_EVENTHEADER   = 104,   // "ALARM DISPLAY SERVER FAULT"
   MTXT_ALIS_EVENTHEADER   = 105,   // "ALARM INTERFACE SERVICES SERVER FAULT"

   MTXT_MMLSRV_ALARMSTORAGEWARNING  = 106, // "IO STORAGE SPACE WARNING"
   MTXT_MMLSRV_ALARMPDSFAULTY       = 108, // "IO PRINTOUT DESTINATION FAULTY"

   MTXT_CPD_PDSLOGDIR      = 110,   // "PDS LOG DIRECTORY"
   MTXT_CPD_PDSPGMDIR      = 111,   // "PDS PROGRAM DIRECTORY"
   MTXT_CPD_INSTALLEDPDS   = 112,   // "INSTALLED PDS PROGRAMS"

   MTXT_PDS_FAILEDTOEXECMD = 120,   // "Failed to execute command"

   MTXT_PDS_CMDMISSMATCH   = 121,   // "The command cannot be used with this function"
   MTXT_PDS_EXECUTIONFAILED= 122,   // "Execution failed due to program exception"
   MTXT_PDS_TOMANYSTARTATTEMPTS = 123, //"Too many start attempts"

   // External alarm strings

   MTXT_EXTALARM           = 130,   // "EXTERNAL ALARM"
   MTXT_EXTALARMRCVFAULT   = 131,   // "EXTERNAL ALARM RECEIVER FAULT"
   MTXT_EXTALARMRCVBLOCK   = 132,   // "EXTERNAL ALARM RECEIVER BLOCKED"

   // Other String values

   MTXT_NONE               = 150    // "None"

} ACS_APGCC_Text;



#endif /* ACS_APGCC_TEXTS_H_ */
