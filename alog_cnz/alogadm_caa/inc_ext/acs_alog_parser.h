/*=================================================================== */
/**
   @file   acs_alog_parser.h

   @brief Header file for acs_alog_parser type module.

          This module contains all the declarations useful to
          specify the class.

   @version 2.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       02/02/2011     xgencol/xgaeerr       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef acs_alog_parser_H_
#define acs_alog_parser_H_


/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
//Include file for external library.
#include <iostream>
#include <fstream>
#include <string.h>

#include "acs_alog_types.h"
#include "acs_alog_activeWorks.h"

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Indicates whether the reading of the string must get to the
          end of the audit records.

  */
/*=================================================================== */

using namespace std;

struct TypeCom {
	string typeCommand;
	string typeApi;
};

/*=================================================================== */
/**
    @struct  mess

    @brief   This structure is used to store the strings extracted
             from parses records and audit records to build the store
             in the target file.

			@par            size_record;
			@par 		data;
			@par 		time;
			@par 	        TypeRec;
			@par		prca;
			@par		user_name;
			@par		device;
			@par		node_number;
			@par		pid;
			@par		local_host_name;
			@par		remote_host;
			@par		local_host_info;
			@par		size_msg;
			@par		msg;
			@par		cmdidlength;
			@par		cmdid;
			@par		cpnamegroup;
			@par		cpidlist;
			@par		cmdseqnum;
			@par		type;
			@par		daemon_state;
			@par		user_pid;
			@par		uid;
			@par		auid;
			@par		terminal;
			@par		exe;

*/
/*=================================================================== */
	struct mess {
		string size_record;
		string data;
		string time;
        string TypeRec;
		string prca;
		string user_name;
		string device;
		string node_number;
		string pid;
		string local_host_name;
		string remote_host;
		string local_host_info;
		string size_msg;
		string msg;
		string cmdidlength;
		string cmdid;
		string cpnamegroup;
		string cpidlist;
		string cmdseqnum;
		string daemon_state;
		string user_pid;
		string uid;
		string terminal;
		string res;
		string source;
		string Type;
		string category;
	};
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     The class acs_alog_parser is used to build records to store
                 in the target file.

                 Class description
*/
/*=================================================================== */
class acs_alog_parser {

/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:

/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   String that contains the audit records from parses.
   */
/*===================================================================*/
	string bufStrOne;

/*=================================================================== */
   /**
      @brief   String that contains the audit records from parses.
   */
/*===================================================================*/
	string bufStrTwo;

/*=================================================================== */
   /**
      @brief   String that contains the formatted audit record to be
               retained in the target file.
   */
/*=================================================================== */
	string targetRec;

/*=================================================================== */
   /**
      @brief   String that contains the formatted audit record to be
               retained in the target file by adding the length of the
               record itself.
   */
/*=================================================================== */
//	string targetRecFin;

/*=================================================================== */
   /**
      @brief   This struct contains all string extract from the
               audit record.
   */
/*=================================================================== */
	mess messField;

/*=================================================================== */
	  /**

	   @brief          This method resets the fields in the struct mess.

	   */
/*=================================================================== */
	void _CLEAR();

/*=================================================================== */
          /**

           @brief          This method  delete + and added , in field
		       	   cpidlist in the struct mess.

           */
/*=================================================================== */
        string _REFORMLIST(string);

/*=================================================================== */
   /**

	  @brief           This method formats the date and time in the format
					   requested.

	 */
/*=================================================================== */
	void _DATETIME();

/*=================================================================== */
   /**

	  @brief           This method search pattern in the audit event record

					   This method, after determining if the search string
					   exists in the record, it returns a substring derived
					   based on the parameters offset and end.

	  @param           word
					   This parameter contains the string to search

	  @param           delim
					   This parameter contains the character to be
					   considered as a delimiter for the string to be returned.

	  @param           offset
					   It is used to determine the starting point for
					   reading the string to be returned.

	  @param           end
					   This parameter is used to indicate the function
					   to return the string ends at the end of record

	  @return          ret

   */
/*=================================================================== */
	string _FIND(string Word,  //IN
				 string delim,  //IN
				 int offset,    //IN
				 int end,       //IN
				 int ExSys      //IN
				 );

/*=================================================================== */
   /**

	  @brief           This method extracts the audit records the strings
					   needed to construct the record target.

					   This method checks if the string to be extracted
					   exists in the audit records and then extracts it
					   and stores it in the struct type mess.

   */
/*=================================================================== */
	 void _READREC();

/*=================================================================== */
   /**

	  @brief           This method extracts the message from the audit records.

					   Audit records do not have all the same format,
					   extraction of the text message is different.
					   The method _READMSG identifies the format and
					   extracts the text of the message correct.

	  @param           type
					   This parameter is used to identify whether the record
					   describes an command event or an security event.

	  @param           user_pid
					   This parameter allows the method to figure out what
					   kind of format has the security event to be examined.

	   @return         msg

   */
/*=================================================================== */
	string _READMSG(string type,      //IN
					string user_pid   //IN
					);

/*=================================================================== */
   /**

	  @brief           This method comprises the record target.

					   This method comprises the target record
					   from the fields of struct mess.

	  @return          tRec
   */
/*=================================================================== */
	string _COMPTARGET( int commandSession   //IN
					   );

    void   hidePwdInTheZipCommands (void);

	string trim_string(string,char);


/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:

/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           This is the class constructor

   */
/*=================================================================== */
  acs_alog_parser();

/*=================================================================== */
   /**

      @brief           This method is called to start the parses of
                       all audit records.

                       The parses the input you start taking the audit
                       records from all sources

      @return          targetRecFin
   */
/*=================================================================== */
	string _PARSE_LINE (int commandSession,
						string sid
						);

        void _PARSE_APCOMMAND(int commandSession, string sid);

        void _PARSE_ALOGAPI(int commandSession);

        void _PARSE_CLICOMMAND(int commandSession);

	void _PARSE_NETCONFCOMMAND(int commandSession);

        void _PARSE_CMWEA(int commandSession, string sid);

        void _PARSE_SECURITY(int commandSession, string sid);

        void _PARSE_SECURITY_PAM(int commandSession, string sid);
	void _PARSE_SECURITY_PAM_TLS(int commandSession, string sid);
	void _PARSE_SECURITY_PAM_FTP(int commandSession, string sid);

        void _PARSE_TELNET_CONNECTION(int commandSession, string sid);
	void _PARSE_COMTLS(int commandSession);
	void _PARSE_CLISSH(int commandSession);
	
	string tlsUserName;
	string sshRemoteHost;
	string tlsConnectionStatus;
	bool isFailure;

        void    set_BufStrOne ( const std::string& event);
        void    set_BufStrTwo ( const std::string& event);
        string  get_PartOfMsg ();
        string  get_TypeRec ();
        void    clean_internal_string ();

/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           This is the class destructor

   */
/*=================================================================== */
  ~acs_alog_parser(){};
};

#endif /* acs_alog_parser_H_ */
