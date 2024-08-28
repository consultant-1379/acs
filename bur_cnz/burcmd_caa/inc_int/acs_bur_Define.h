/*
 * acs_bur_Define.h
 *
 *  Modified on: 2 Apr 2014
 *      Author: xquydao
 *  Created on: Dec 2, 2011
 *      Author: egimarr
 */

#ifndef ACS_BUR_ERRORCODE_H_
#define ACS_BUR_ERRORCODE_H_

#define	RTN_OK	 0
#define RTN_FAIL -1

//	backup status

#define BRM_BACKUP_COMPLETE 	1
#define BRM_BACKUP_INCOMPLETE 	2
#define BRM_BACKUP_CORRUPTED	3

// Export command
#define EXPORTCMD_FIRST 	"/usr/sbin/lde-brf export -l "
#define EXPORTCMD_SECOND 	" -t system -f "

// Linux command to validate a tar file
#define TARFILEVALIDATE 	"tar tf "
#define TARFILENOTVALID_STRING "tar: This does not look like a tar archive"

// Import command
#define IMPORTCMD			"/usr/sbin/lde-brf import -t system -f "
#define IMPORTPRINTCMD		"/usr/sbin/lde-brf print import -t system "
//

//
// Diagnostic message error for burbackup command
#define MSG_OPABORT		"Backup creation aborted"
#define MSG_RSTABORT	"Backup restore aborted"
#define MSG_SRVBUSY		"Backup service error"
#define MSG_ERRBKP		"Error during complete backup archive operation"
#define MSG_ERREXP		"Error on backup archive export operations"
#define MSG_GENERAL		"Error when executing ( %s )"
#define MSG_USERNOTALLOWED		"Command not allowed in this session type"
#define MSG_ILLOPT		"Illegal option "
#define MSG_INCUSA		"Incorrect Usage"
#define MSG_BKPARCOK	"Export backup execution completed."
#define MSG_EXEBBACK	"Execute burbackup with these parameters:"
#define MSG_EXEBREST	"Execute burrestore with these parameters:"
#define MSG_EXEARCH		"Execute burbackup with these parameters:"
#define MSG_INTFAULT	"Internal program fault: "
#define MSG_BACKUP_FAIL "Backup creation failed"
#define MSG_ARCHIVECREATED "Backup archive %s created."
#define MSG_ALREADYEXIST "Export backup failed, backup already exists"
#define MSG_NOSPACELEFT "Not enough space on data disk"
#define MSG_NOSPACERST  "Import backup failed, no space left"
#define MSG_INVBKPTYP	"Import backup failed, invalid backup archive"
#define MSG_DONTEXIST	"Export backup failed, backup does not exist"
#define MSG_IMGNAMNOTVLD "Image name %s is a not valid name"
#define MSG_ADVANCE 	"% of system backup creation completed."
#define MSG_ADVANCERES 	"% of system restore completed."
#define MSG_SYSBCKSUCC	"System backup creation operation success."

#define MSG_BKPSTART 	"Starting burbackup operations."
#define MSG_RSTSTARTR	"Starting burrestore operations."
#define MSG_RSTSTART	"Starting restore of backup image."
#define MSG_STARTARC	"Export backup started."
#define MSG_STARTIMP	"Importing backup archive from backup_restore folder:"
#define MSG_REDIRECTNULL " 1>/dev/null"
#define MSG_NOSTDERR	" 2>&1 "
#define MSG_BKPSUCCOP	"Export backup execution completed."
#define MSG_BKPSUCC		"Complete system backup execution completed."
#define MSG_SECBKPSUCC		"Complete secured system backup execution completed."
#define MSG_INFORPT		"Automatic generated system backup info:"
#define MSG_INFOARC		"System backup progress info: "
#define MSG_INFOEXPORT	"Export backup info:"
#define MSG_INFOBKP		"System restore progress info: "
#define MSG_YESNOT		"[y=yes,n=no]?"

#define MSG_CONFLICT	"Import backup conflict"
#define MSG_EXPCONF		"Export backup conflict"
#define MSG_UNKNOWFRT	"Import backup failed, unknown archive type"
#define MSG_RSTSUCC		"Burrestore execution success."
#define MSG_BKPEXPOK	"Import backup execution completed."
#define MSG_IMPCOMPL	"Backup archive %s import completed."
#define MSG_IMPBKPINF	"Imported backup info:"
#define MSG_BKPNOTVALID "Invalid backup archive file name"
#define MSG_RSTALREXIST	"Imported backup already exist"
#define MSG_BKPARCNOTEX "Backup archive %s does not exist"
#define MSG_IMPORTSTART "Import backup started."
#define MSG_IMPORTCOMPLETED "Import backup completed."
#define MSG_SYSTEMRESTORE_FAIL "System restore failed"
#define MSG_SYSTEMRESTORE_ABORT "System restore aborted"
#define MSG_BRF_CONFLICT "Command not executed, backup or restore operation ongoing"
#define MSG_PWDMISMATCH "Passwords not matching"
#define MSG_INVALIDPWD "Invalid Password"
#define MSG_INCORRECTPWD "Incorrect Password"
#define MSG_RESTOREINVALIDPWD "System restore aborted, invalid password"
#define MSG_INVALIDOPT "Invalid option for restoring regular backup"
#define MSG_MISSINGOPT "Missing option for restoring secured backup"
//
// Delay time after next progressStatus read
//
#define SLEEPDELAY		2	// Default value
#define FOREVER			1
//
// OP CODE ERROR

#define NOERROR			0
#define INITERROR		-1	// Init function error
#define	GETATTRERROR	-2	// GetAttrbute() function error
#define	FINALIZEERROR	-3	// Finalize error
#define	GETCLAINSERR	-4	// Get instances founded error
#define NOINSFOUERR		-5	// No instance founded
#define MORONEERR		-6  // More one instances was founded
#define INVOKEERR		-7  // Admin operation invoke error
#define NOTONEBRM		-8  // zero or more one Brm instances founded
#define NOBKPINSFOU		-9  // No BrmBackup class instance founded
#define BBMNOTFND		-10 // BrmBackupManager Instance not founded
#define ERRHOSTNAME		-11 // Error on gethostname() function
#define INVOPTION		-12 // Invalid option in parse() function
#define SYNTAXERR		-13 // Syntax error in parse() function
#define OPNOTACEPT		-14 // Operation not accepted for BACKUP/RESTORE action
#define FORCEDQUIT		-15 // Forced quit press not
#define OPCANCELLED		-16 // Backup operation cancelled
#define EXPFAIL			-17 // Error on export operation on burbackup -a
#define ACTIONFAIL		-18 // action fail on cratebackup
#define BKPEXPFAIL		-19 // Error on export operation on burbackup -o
#define OPFAILED		-20 // Operation status result FAILURE

#define MANYBACKUPNAME  -21 // Many Backup with the same name are founded
#define CHGCUROP		-22 // Operation ID is different to CREATEBACKUP attempted
#define BRMISTABS		-23 // BrmBackupInstance not present
#define BACKUPFAIL		-24 // Backup failed
#define NOSPACELEFT		-25
#define ALREADYEXIST	-26
#define INVALIDIMAGE	-27 // Invalid imagename not founded
#define GENERALFAULT	-28 // General Fault
#define CONFLICT		-29 // lde-brf export conflict
#define PATHNOTCOR		-30 // Path archive file not correct
#define EXTNOTVALID		-31 // Extention archive file not valid
#define DONTEXIST		-32 // File don't exist from lde-brf command
#define PARINVNAME		-33 // Name of archive name contains extra alfanumeric character
#define OPEINPROG		-34 // Operation in progress lde-brf import
#define UNKNOWARCTYPE   -35 // Unknow archive type from Import operation (lde-brf)
#define NOTBRFBKP		-36 // Is not a BRF backup....Is a not valid backup
#define POPENERR		-37 // Error popen
#define EXTNAMERR		-38 // Extention name error no extention must be present in archivename
#define EXTALFNUM		-39 // Extra alfa numeric character in node name
#define LDEBRFEXPERR    -40 // lde-brf export failed
#define IMPORTFAILS		-41 // lde-brf import fails
#define BACKUPINCOMPLETE -42 // Backup was created but notit not  COMPLETED
#define BACKUPNOTEXIST	-43 // After SUCCESS in creation BACKUP not created
#define SETUIDERR		-44 // Internal error for setuid(0) function
#define PATHDOESNOTSEXIST -45 // Path does't exist probably node is PASSIVE
#define BRFCONFLICT -46 // Blocking brf action already ongoing
#define USERNOTALLOWED -47 // Blocking burbackup command exection for TS user group
#define PWDMISMATCH -48 //Entered password & confirm password are not same
#define INVALIDPWD -49 // Invalid password provided during secure backup restore
#define INCORRECTPWD -50 //Incorrect password provided during secure backup
#define INVALIDOPT -51 // Invalid option provided for restoring regular backup
#define MISSINGOPT -52 // Missing option for restoring secure backup

// Define deault extention for import/export file
#define DEF_EXTENTION	".tar"

// Define on ActionResultType
#define	SUCCESS			1
#define	FAILURE			2
#define NOT_AVAILABLE	3

// Define ActionStateType
#define CANCELLING 		1
#define RUNNING 		2
#define FINISHED 		3
#define CANCELLED 		4

// Define Action for BrmBackupManager

#define CREATEBACKUP	0
#define DELETEBACKUP	1
#define CANCELCURRENT 	2
#define RESTOREBACKUP	3

// Define on maximum lenght of node name
#define MAXNODENAMELENGHT 13

//
#define CN_BRMBACKUPMANAGER "BrmBackupManager"
#define CN_BRM				"Brm"
#define CN_BRMBACKUP		"BrmBackup"
//
#define CLASSNAME			"BrmBackupManager"
#define BACKUPTYPE			"SYSTEM_DATA"
//
#define BURCONFIGFILENAME 	"/data/acs/data/bur/bur.config"
#define BACKUPRESTOREPATH	"/backup_restore/"
#define DEBUGCODE

#ifdef DEBUGCODE
 #define DBG(x) std::cout << x << endl;
#else
 #define DBG(x) /* nothing */
#endif
//
#endif /* ACS_BUR_ERRORCODE_H_ */
