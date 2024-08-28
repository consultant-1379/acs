/*
 * nc_common.h
 *
 *  Created on: Jan 28, 2014
 *      Author: estevol
 */

#ifndef NC_COMMON_H_
#define NC_COMMON_H_

#define CMD_OPTIONS  "c:s:a:p:u:w:k:x:o:t:n:i:"
#define IRONSIDE_CMD_OPTIONS "a:p:s:o:h:"
#define CMD_GET "ncget"
#define CMD_EDIT_CONFIG "nceditconfig"
#define CMD_ACTION "ncaction"


namespace nc_cmd_ns {
	enum commandExitCodes {
		NC_NO_ERROR = 0,
		NC_INCORRECT_USAGE = 1,
		NC_UNKNOWN_OPERATION = 2,
		NC_SESSION_OPEN_FAILED = 3,
		NC_SESSION_CLOSE_FAILED = 4,
		NC_SEND_RCV_FAILED = 5,
		NC_INVALID_IP = 6,
		NC_INVALID_AUTHENTICATION = 7,
		NC_INVALID_CONNECTION = 8,
		NC_MISSING_INPUT_FILE = 9,
		NC_INVALID_QUERY = 10

	};


	enum OutputMode {
		OUT_SCREEN = 1,
		OUT_FILE = 2
	};

}

#endif /* NC_COMMON_H_ */
