/*acs_alog_types.h
 *
 *  Created on: Set 13, 2011
 *      Author: xgencol
 */

#ifndef ACS_ALOG_TYPES_H_
#define ACS_ALOG_TYPES_H_

//========================================================================================
//      Size declarations for parameters.
//========================================================================================

#define END                              1
#define NO_END                           0

//========================================================================================
//      Type declarations for return type and error type.
//========================================================================================

typedef enum {

        ACS_ALOG_ok      = 0,        //  Ok return code from sendEventMessage method
        ACS_ALOG_error   = 1         //  Error return code from the sendEventMessage method

} ACS_ALOG_ReturnType;

#define ERR55               "Unable to connect to configuration server"
#define ERR56               "Configuration Server Error"
#define MAXLENGTHBUFFER     5000000
#define EVENT_MAXSIZE       3200000
#define MAXLEN_SYSLOG       300000
#define SOURCEMAIN_P        "/var/run/ap/acs_alogmainfifo"
#define SOURCEAPI_P         "/var/run/ap/ALOG_API_pipe"
#define WAITREAD            20
#define MAXLENGTHRECORD     1048576
#define LOGROTATEFILE       "/etc/logrot.d/default"
#define MAXEXCLADD          30
#define MAXEXCLDEL          30
#define ONE_MEGA			1048576         //  1024 * 1024
#define TWO_MEGA            2097152   //  2*1024 * 1024 //TR HX86564
#define ONE_GIGA			1073741824      //  1024 * 1024 * 1024
#define HUNDRED_MEGA        104857600		//  100 * 1024 *1024
#define EVENTDELIMITATION   255

#define LINUX_FORMAT		0		//  the audit log files will be in the format :  audit.yyyymmdd-hhmms
#define WINDOWS_FORMAT		1		//  the audit log files will be in the format :  LOGFILE-yyyy-mm-dd-hhmm

#define FILES_TIMESTAMP_LEN		12		//  used to sort the audit files based on the Time Stamp creation

#endif /* ACS_ALOG_TYPES_H_ */

