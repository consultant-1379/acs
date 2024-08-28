/*=================================================================== */
/**
        @file           acs_aca_msgfile.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the MsgFile  class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       19/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _MSG_FILE_H_
#define _MSG_FILE_H_
/*=====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>

//#include <ACS_PRC_Process.h>

#include "acs_aca_ms_parameters.h"
//#include <acs_PHA_Parameter.H>
#include "acs_aca_message_number.h"
#include "acs_aca_ms_commit_file.h"
#include "acs_aca_ms_statistics_file.h"
//#include <ACS_ExceptionHandler.H>
//#include <commondll.h>
#include "acs_aca_common.h"
#include "acs_aca_ms_const_values.h"

//using namespace std;

/*=====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class MsgFile {
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for MsgFile class.

        @param          s

        @param          name(s)

        @param          dataDirectoryName

        @param          firstMsgNum

        @param          lastMsgNum

        @param          fileNum

        @param          noOfMsgs

        @param          eofFound

        @param          eomFound        

=================================================================== */
	inline MsgFile (std::string s, std::string directory)
		: name(s), dataDirectoryName(directory), firstMsgNum(0), lastMsgNum(0),
		  fileNum(0), noOfMsgs(0), eofFound(0), eomFound(0) {
		std::string tmp(s);
//		tmp=s;

		size_t pos = tmp.find_last_of( "/\\" );
		const char * ptr = tmp.c_str() + pos + 1;

		// First time. Must fake a previous number.
		firstMsgNum = ::atoi(ptr);
		const char * ri = ::strchr(s.c_str(), '_');
		if (ri) {
			fileNum = ::atoi(ri + 1);
		}
	}
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	inline ~MsgFile () {}
/*=====================================================================
                                OPERATOR DECLARATION SECTION
==================================================================== */
	int operator< (const MsgFile & f) const { return firstMsgNum < f.firstMsgNum; }
	int operator== ( const MsgFile & f) const { return firstMsgNum == f.firstMsgNum; }
	bool operator() (const MsgFile & x, const MsgFile & y) { return x.firstMsgNum < y.firstMsgNum; }
/*===================================================================

        @brief          readLastMsg

        @return         int

=================================================================== */
	int readLastMsg ();
/*===================================================================

        @brief          getFullPathName

        @return         string

=================================================================== */
	inline std::string getFullPathName (){ return name;};
                
/*=====================================================================
                         DATA MEMBER       
==================================================================== */
	std::string name;
	std::string dataDirectoryName;

	ULONGLONG	firstMsgNum;
	ULONGLONG	lastMsgNum;
	int fileNum;
	int noOfMsgs;
	bool eofFound;
	bool eomFound;
};

#endif
