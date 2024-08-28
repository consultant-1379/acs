/*=================================================================== */
/**
   @file acs_alog_parser.cpp

   Class method implementationn for acs_alog_parser type module.

   This module contains the implementation of class declared in
   the acs_alog_parser.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       02/02/2011     xgencol/xgaeerr       Initial Release
   N/A		 12/04/2015		xsansud					TR HU40809
  =================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
//Include file for external library.

//Mandatory.
#include "acs_alog_parser.h"
#include "string"
#include "cstdlib"
//#include <alog_activeWorks.h>

extern int     RecForCLog;
extern string  CLOGevent;

extern bool  MCpSystem;
extern int   LargeEventDataSkipped;
extern string strHostname; //TR:HY25168 

int  Sec = 0;


acs_alog_parser::acs_alog_parser()
{

	messField.size_record.reserve(10);
	messField.data.reserve(15);
	messField.time.reserve(10);
	messField.TypeRec.reserve(10);
	messField.prca.reserve(50);
	messField.user_name.reserve(50);
	messField.device.reserve(20);
	messField.node_number.reserve(10);
	messField.pid.reserve(10);
	messField.local_host_name.reserve(90);
	messField.remote_host.reserve(50);
	messField.local_host_info.reserve(50);
	messField.size_msg.reserve(10);
	messField.msg.reserve(3000000);
	messField.cmdidlength.reserve(5);
	messField.cmdid.reserve(30);
	messField.cpnamegroup.reserve(50);
	messField.cpidlist.reserve(1000);
	messField.cmdseqnum.reserve(10);
	messField.daemon_state.reserve(20);
	messField.user_pid.reserve(20);
	messField.uid.reserve(30);
	messField.terminal.reserve(20);
	messField.res.reserve(50);
	messField.source.reserve(30);
	messField.Type.reserve(20);
	messField.category.reserve(20);

	targetRec.reserve(3000000);
	bufStrOne.reserve(3000000);
	bufStrTwo.reserve(3000000);
}



string acs_alog_parser::trim_string(string strToTrim, char trimType) {

	string strTemp = "";
	int index = 0;

	//remove tabulation from string
	strToTrim.erase(remove(strToTrim.begin(), strToTrim.end(), '\t'), strToTrim.end());

	switch(trimType) {

	case 'B': //trim both side

		//if the first character is a space
		index = 1;

		while(1) {
			strTemp = strToTrim.substr(0,index);

			if(strTemp == " ") {
				strToTrim = strToTrim.substr(index);
			}else {
				break;
			}
		}


		index = strToTrim.length();

		while(1) {
			strTemp = strToTrim.substr(index,index);

			if(strTemp == " ") {
				strToTrim = strToTrim.substr(0,index-1);
			}else {
				break;
			}
		}

		break;

	case 'L': //trim left space

		//if the first character is a space
		index = 1;

		while(1) {
			strTemp = strToTrim.substr(0,index);

			if(strTemp == " ") {
				strToTrim = strToTrim.substr(index);
			}else {
				break;
			}
		}
		break;

	case 'R': //trim right space

		index = strToTrim.length();

		while(1) {
			strTemp = strToTrim.substr(index,index);

			if(strTemp == " ") {
				strToTrim = strToTrim.substr(0,index-1);
			}else {
				break;
			}
		}
		break;

	case 'D': //remove all spaces

		break;
	}

	return strToTrim;
}

/*===================================================================
   ROUTINE: _CLEAR
=================================================================== */
void acs_alog_parser::_CLEAR()
{
	messField.size_record.clear();
	messField.data.clear();
	messField.time.clear();
	messField.TypeRec.clear();
	messField.prca.clear();
	messField.user_name.clear();
	messField.device.clear();
	messField.node_number.clear();
	messField.pid.clear();
	messField.local_host_name.clear();
	messField.remote_host.clear();
	messField.local_host_info.clear();
	messField.size_msg.clear();
	messField.msg.clear();
	messField.cmdidlength.clear();
	messField.cmdid.clear();
	messField.cpnamegroup.clear();
	messField.cpidlist.clear();
	messField.cmdseqnum.clear();
	messField.daemon_state.clear();
	messField.user_pid.clear();
	messField.uid.clear();
	messField.terminal.clear();
	messField.res.clear();
	messField.source.clear();
	messField.Type.clear();
	messField.category.clear();
}

/*===================================================================
   ROUTINE: _DATETIME
=================================================================== */
void acs_alog_parser::_DATETIME()
{
	char time[80];
	char data[80];

	time_t ctimevar;

	ctimevar = atoi(messField.data.c_str());

	struct tm * timeinfo;
	timeinfo = localtime(&ctimevar);

	strftime (data,80,"%Y-%m-%d",timeinfo);
	messField.data = data;
	messField.data = messField.data + ";";

	strftime (time,80,"%H%M%S",timeinfo);
	messField.time = time;
	messField.time = messField.time + ";";
}

/*===================================================================
   ROUTINE: _FIND
=================================================================== */
string acs_alog_parser::_FIND(string Word, string delim, int offset, int end, int ExSys)
{
	size_t found = 0;
	size_t foundOne = 0;
	size_t foundTwo = 0;
	size_t posIni = 0;
	size_t posFin = 0;
	size_t Set = 0;

	string subString;
	string ret;
	string bufFind;

	if (ExSys == 0)
	{
		Set = 1;
		foundOne = bufStrOne.find(Word);

		if (foundOne == string::npos)
		{
			ret = "";
			return ret;
		}

		bufFind = bufStrOne;
		found = foundOne + offset;
	}

	if (ExSys == 1)
	{
		Set = 1;
		foundTwo = bufStrTwo.find(Word);

		if (foundTwo == string::npos)
		{
			ret = "";
			return ret;
		}

		bufFind=bufStrTwo;
		found = foundTwo + offset;
	}

	if (ExSys == 2)
	{
		Set = 1;
		foundOne = bufStrOne.find(Word);
		foundTwo = bufStrTwo.find(Word);

		if (foundTwo == string::npos  &&  foundTwo == string::npos)
		{
			ret = "";
			return ret;
		}

		if (foundOne != string::npos)
		{
			bufFind = bufStrOne;
			found = foundOne + offset;
		}
		else {
			bufFind = bufStrTwo;
			found = foundTwo + offset;
		}
	}

	if (Set)
	{
		if (end == END)
		{
			subString = bufFind.substr(found);
			ret = subString;
		}
		else {
			subString = bufFind.substr(found);
			posFin = subString.find(delim,0);

			if (posFin == string::npos)  ret = "";
			else                         ret = subString.substr(posIni,posFin);
		}
	}
	return ret;
}

/*===================================================================
   ROUTINE: _READREC
=================================================================== */
void acs_alog_parser::_READREC()
{
	size_t found = 0;

	messField.Type = _FIND("type", " ", 5, NO_END, 0);
	messField.data = _FIND(" msg=audit(", ":", 11, NO_END, 0);

	found = bufStrOne.find(" auditd start");

	if (found == string::npos)  messField.daemon_state = "";
	else 						messField.daemon_state = "start auditd";

	messField.user_pid = _FIND(" user pid", " ", 10, NO_END, 0);

	if (messField.user_pid.empty())
	{
		messField.pid = _FIND(" pid", " ", 5, NO_END, 0);
	}
	else {
		messField.pid = "";
	}

	messField.uid = _FIND(" uid", " ", 5, NO_END, 0);

	messField.terminal = _FIND(" terminal", " ", 10, NO_END, 0);

	messField.res = _FIND(" res", ",", 5, NO_END, 0);
	if(messField.res.size())
	{
		messField.res = _FIND(" res", ")'", 5, NO_END, 0);
	}
}

/*===================================================================
   ROUTINE: _READMSG
=================================================================== */
string acs_alog_parser::_READMSG(string type, string user_pid)
{
	string cycleMsg;
	string msg;
	string parOne;
	string Sinc;

	char Cinc[2];

	int length = 0;
	int inc = 0;

	if (!type.compare("apcommand.notice:"))
	{
		if(bufStrOne.size() == 0)
		{
			msg = _FIND(" comm=", "\\", 7, NO_END, 1);
			if (msg != "")
			{
				msg = trim_string(msg,'L');
			}
		}
		else {
			msg = _FIND(" a0=", "\" ", 5, NO_END, 0);
			if (msg.size() != 0)
			{
				msg = trim_string(msg,'L');
				cycleMsg = msg;
				while (cycleMsg != "")
				{
					cycleMsg.clear();
					inc = inc + 1;
					ACE_OS::sprintf(Cinc,"%d",inc);
					Sinc = Cinc;
					parOne = " a" + Sinc + "=";
					cycleMsg = _FIND(parOne, "\" ", 5, NO_END, 0);
					if (cycleMsg.size() != 0)
					{
						msg = msg + " " + cycleMsg;
					}
				}
			}
		}
	}
	else if (!type.compare("alogapi.notice:"))
	{
		msg = _FIND(" cmd=", ",, ", 5, NO_END, 0);
		msg = trim_string(msg,'L');
	}
	 else if (!type.compare("comtls.notice:"))
        {
                string tmp_str = " ";
		string reason_str = " ";
		int posSt = bufStrOne.find("from");
		tmp_str = bufStrOne.substr(posSt);
		int posFn = tmp_str.find("hostname");
		int posReason = tmp_str.find("reason");
		posSt = tmp_str.find("from");
		if(posReason != (int)string::npos){
			reason_str = tmp_str.substr(posReason);
		}
		if(posFn != (int)string::npos){
			tmp_str=tmp_str.substr(posSt,(posFn));
		}
		else {
			tmp_str = "";
		}
		msg =tmp_str+reason_str+"\n";
        }
        else if (!type.compare("clissh.notice:"))
        {
                 msg = msg+"\n";
        }
	else if (!type.compare("clicommand.notice:"))
	{
		//msg = _FIND(" session-id=", " ", 15, END, 0);
		/*
		 * Start Modify: To manage properly session id composed of more
		 * than only one figure.
		 */
		string tmp_str = _FIND(" command:", " ", 10, END, 0);

		int i_offset=0;

		int lenStrWord=tmp_str.length();
		// counts figures and spaces
		while ( (i_offset< lenStrWord ) && ((tmp_str.at(i_offset) >= '0'  &&  tmp_str.at(i_offset) <= '9' ) || tmp_str.at(i_offset)== ' ') ){

			i_offset++;
		}

		//once retrieved how many figures and spaces are after session-id select the substring
		msg = _FIND(" command:", " ", 10+i_offset, END, 0);

		/* End Modify*/
	}
	else if (!type.compare("netconfcommand.notice:"))
	{
		int posSt = bufStrOne.find("ses=");
                int posFn = bufStrOne.find(" ",posSt);
                msg = bufStrOne.substr(posFn+2);
	}
	else if (!type.compare("cmwea.notice:"))
	{
		msg = _FIND(" cmwea:", " ", 8, END, 0);
	}
	else {
		if (!user_pid.empty())
		{
			msg = _FIND(" msg=\\'", " ", 7, END, 0);
			length = msg.length();

			msg = msg.substr(0,length - 3);
			msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: " + "\n" + msg;
		}
		else {
			msg = _FIND("):", " ", 3, END, 0);
			msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: " + "\n" + msg;
		}
	}
	return msg;
}

/*===================================================================
   ROUTINE: _COMPTARGET
=================================================================== */
string  acs_alog_parser::_COMPTARGET(int commandSession)
{

	string tRec;

	tRec = messField.data + messField.time + messField.TypeRec + messField.prca + messField.user_name + messField.device +
			messField.node_number + messField.pid + messField.local_host_name + messField.remote_host + messField.size_msg +
			messField.msg;

	if (MCpSystem == true)
	{
		tRec = tRec + messField.cmdidlength + messField.cmdid + messField.cpnamegroup + messField.cpidlist + messField.cmdseqnum;
	}
	else  if (commandSession)
	{
		tRec = tRec + messField.cmdidlength + messField.cmdid;
	}

	return tRec;
}

/*===================================================================
   ROUTINE: _PARSE_LINE
=================================================================== */
string acs_alog_parser::_PARSE_LINE (int commandSession, string sid)
{
	int compEvent = 0;

	_CLEAR();
//		syslog(LOG_INFO, "bufStrOne : %s",bufStrOne.c_str());
	if (bufStrOne.compare("NO_STRING") || bufStrTwo.compare("NO_STRING"))
	{
		if (((bufStrOne.find("apcommand.notice:") != string::npos) || (bufStrTwo.find("apcommand.notice:") != string::npos)))
		{
			_PARSE_APCOMMAND(commandSession, sid);
			compEvent = 1;
		}
		else if (bufStrOne.find("alogapi.notice:") != string::npos)
		{
			_PARSE_ALOGAPI(commandSession);
		        compEvent = 1;
		}
		else if (bufStrOne.find("clicommand.notice:") != string::npos)
		{
			_PARSE_CLICOMMAND(commandSession);
			compEvent = 1;
		}
		else if (bufStrOne.find("netconfcommand.notice:") != string::npos)
		{
			_PARSE_NETCONFCOMMAND(commandSession);
			compEvent = 1;
		}
		 else if (bufStrOne.find("comtls.notice:") != string::npos)
                {
                        _PARSE_COMTLS(commandSession);
                        compEvent = 1;
                }
                else if (bufStrOne.find("clissh.notice:") != string::npos)
                {
                        _PARSE_CLISSH(commandSession);
                        compEvent = 1;
                }
		else if (bufStrOne.find("cmwea.notice:") != string::npos)
		{
			_PARSE_CMWEA(commandSession, sid);
			compEvent = 1;
		}
		else if (bufStrOne.find("audispd:") != string::npos)
		{
			_PARSE_SECURITY(commandSession, sid);
			compEvent = 1;
		}
		else if ((bufStrOne.find("sshd") != string::npos ) &&
			((bufStrOne.find("error: PAM") != string::npos ) || (bufStrOne.find("Accepted keyboard-interactive/pam") != string::npos)
			|| (bufStrOne.find("Failed password for") != string::npos) || (bufStrOne.find("Accepted password for") != string::npos) ))// TR HU40809  & SSH failed login events
		{
			_PARSE_SECURITY_PAM(commandSession, sid);
			compEvent = 1;
		}
		else if ((bufStrOne.find("sshd") != string::npos ) && ((bufStrOne.find("Disconnected from") != string::npos) || (bufStrOne.find("session closed for user")!= string::npos)))
			 // SSH Logout and failed login events
                {
                        _PARSE_SECURITY_PAM(commandSession, sid);
			compEvent = 1;
                }
		else if ((bufStrOne.find("sshd") != string::npos) && ((bufStrOne.find("sshd:account")!= string::npos) && 
				((bufStrOne.find("Access denied")!= string::npos) || (bufStrOne.find("expired")!= string::npos) )))
                {
                        _PARSE_SECURITY_PAM(commandSession, sid);
                        compEvent = 1;
                }
		else if ( (bufStrOne.find("login") != string::npos) && 
				((bufStrOne.find("LOGIN ON") != string::npos)||(bufStrOne.find("FAILED LOGIN") != string::npos)|| ((bufStrOne.find("remote:session") != string::npos) && (bufStrOne.find("session closed") != string::npos) )) ) 
                {
			_PARSE_TELNET_CONNECTION(commandSession, sid);
			compEvent = 1;
		}
//              Removal of PAM events for TLS          
/*		else if (bufStrOne.find("com-tlsd") != string::npos)
		{

			if(((bufStrOne.find("err [") != string::npos) && (bufStrOne.find("pam_acct_mgmt") != string::npos)) || 
					((bufStrOne.find("pam_sss(com-tlsd:account)") != string::npos)||
							(bufStrOne.find("Authentication failure") != string::npos)||
							(bufStrOne.find("Shared object load failure") != string::npos)||
							(bufStrOne.find("Permission denied") != string::npos)||(bufStrOne.find("No account present for user") != string::npos)||
							(bufStrOne.find("Maximum number of tries exceeded") != string::npos)||
							(bufStrOne.find("Error in underlying service module") != string::npos)||
							(bufStrOne.find("Failed TLS connection") != string::npos)||
							(bufStrOne.find("System error") != string::npos)) ) // To handle Failure scenarios
			{
				_PARSE_SECURITY_PAM_TLS(commandSession, sid);
				compEvent = 1;
			}
                  
			else if ((bufStrOne.find("com-tlsd:") != string::npos) && (bufStrOne.find("pam_acct_mgmt = 0(Success)") != string::npos) )//For cases like local user, ts user and Ldap user during CLI & Netconf sessions
			{
				_PARSE_SECURITY_PAM_TLS(commandSession, sid);
				tlsConnectionStatus = "pam_acct_mgmt = 0(Success)";
				compEvent = 0;
			}
	

		}*/
/*		else if ((bufStrOne.find("TTY=") != string::npos) && ((bufStrOne.find("tls-netconf") != string::npos) || (bufStrOne.find("tls-cliss") != string::npos)))
		{ 	
								//to extract the line containing User name for Cli and Netconf sessions 
			_PARSE_SECURITY_PAM_TLS(commandSession, sid);
			 compEvent = 1;
		}*/

		else if (bufStrOne.find("vsftpd") != string::npos) 
		{
			//Strings are checked with "::ffff:" so as to avoid logging for FTP & SFTP Connections also.We bothered only FTP-TLS Connection//
			if((bufStrOne.find("OK LOGIN: addr=\"::ffff:") != string::npos)&&(bufStrOne.find("anon password") == string::npos) )//To extract the line containing User name for FtpOverTls session
			{
				_PARSE_SECURITY_PAM_TLS(commandSession, sid);
				compEvent = 1;
			}
			else if((bufStrOne.find("FAIL LOGIN: addr=\"::ffff:") != string::npos) )//To extract the line containing User name for FtpOverTls session
			{
				_PARSE_SECURITY_PAM_TLS(commandSession, sid);
				compEvent = 1;
			}
			else if((bufStrOne.find("OK DOWNLOAD: addr=\"::ffff:") != string::npos) ||(bufStrOne.find("OK UPLOAD: addr=\"::ffff:") != string::npos)||(bufStrOne.find("OK DELETE: addr=\"::ffff:") != string::npos)||(bufStrOne.find("OK RENAME: addr=\"::ffff:") != string::npos)||(bufStrOne.find("OK RMDIR: addr=\"::ffff:") != string::npos)||(bufStrOne.find("OK MKDIR: addr=\"::ffff:") != string::npos)||(bufStrOne.find("FAIL DOWNLOAD: addr=\"::ffff:") != string::npos)||(bufStrOne.find("FAIL UPLOAD: addr=\"::ffff:") != string::npos)||(bufStrOne.find("FAIL DELETE: addr=\"::ffff:") != string::npos)||(bufStrOne.find("FAIL RENAME: addr=\"::ffff:") != string::npos)||(bufStrOne.find("FAIL RMDIR: addr=\"::ffff:") != string::npos)||(bufStrOne.find("FAIL MKDIR: addr=\"::ffff:") != string::npos))//To extract the line for GET/PUT operations using FtpOverTls session
			{
				_PARSE_SECURITY_PAM_TLS(commandSession, sid);
				compEvent = 1;
			}
			else if((bufStrOne.find("authentication success") != string::npos) || (bufStrOne.find("authentication failure") != string::npos) ||
			(bufStrOne.find("session closed for user")!= string::npos)|| (bufStrOne.find("session opened for user")!= string::npos) )
                        {
                                _PARSE_SECURITY_PAM_FTP(commandSession, sid);
                                compEvent = 1;
                        }
			 else if((bufStrOne.find("OK LOGOUT:") != string::npos) )
                        {
                                _PARSE_SECURITY_PAM_TLS(commandSession, sid);
                                compEvent = 1;
                        }




		}


		if (compEvent)
		{
			int   lenEvent  = 0;
			int   lenHeader = 0;
			char  lenBuffer[15];

			lenEvent = messField.msg.length();

			if (LargeEventDataSkipped)
			{
				if (lenEvent > MAXLENGTHRECORD)
				{
					lenEvent = MAXLENGTHRECORD;
					messField.msg = messField.msg.substr(0,MAXLENGTHRECORD);
				}
			}
			sprintf (lenBuffer, "%010d", lenEvent);		//  compliant to Windows Record Format

			messField.size_msg = lenBuffer;
			messField.size_msg = messField.size_msg + ";";

			if (messField.TypeRec.compare("sec;") == 0  &&  messField.data.compare(";") == 0)
			{
				targetRec = "NOT_PARSER";        //  SEC msg without information (size_msg = 63)
				return targetRec;				//  They are discarded
			}

			if (messField.TypeRec.compare("mp;") == 0  &&  lenEvent == 0)
			{
				targetRec = "NOT_PARSER";        			//  CPT printouts :  HP27422 trouble
				return targetRec;
			}

			if (MCpSystem == true)  							//  in a MCP configuration the CmdId len zero
			{													//  indicates that the CommandAndSessionState is not set
				if (commandSession == 0)  lenEvent = 0;
				//else					  lenEvent = messField.cmdid.length();  //TR HV19568: modified
				else					  lenEvent = messField.cmdid.length()-1;//TR HV19568: cmdid previously set has char ; added so length must be decreased by one
			}
			else {
				//if (commandSession == 1)  lenEvent = messField.cmdid.length();//TR HV19568: modified
				if (commandSession == 1)  lenEvent = messField.cmdid.length()-1;    //TR HV19568: cmdid previously set has char ; added so length must be decreased by one
			}
			sprintf (lenBuffer, "%d", lenEvent);
			messField.cmdidlength = lenBuffer;
			messField.cmdidlength = messField.cmdidlength + ";";

			messField.msg = messField.msg + ";";
			messField.local_host_name = messField.local_host_name + ";";
			messField.remote_host = messField.remote_host + ";";
			targetRec = _COMPTARGET (commandSession);

			lenHeader = messField.data.length() + messField.time.length() + messField.TypeRec.length() +
					messField.prca.length() + messField.user_name.length() + messField.device.length() +
					messField.node_number.length() + messField.pid.length() + messField.local_host_name.length() +
					messField.remote_host.length() + messField.size_msg.length();

			sprintf (lenBuffer, "%03d", lenHeader);			//  compliant to Windows Record Format
			messField.size_record = lenBuffer;
			messField.size_record = messField.size_record + ";";
			targetRec = messField.size_record + targetRec;

			return targetRec;
		}
	}
	targetRec = "NOT_PARSER";
	return targetRec;
}

/*===================================================================
   ROUTINE: _PARSE_APCOMMAND
=================================================================== */
void acs_alog_parser::_PARSE_APCOMMAND(int commandSession, string sid) {

	char hostname[1024];
	size_t found = 0;

	messField.data = _FIND(" msg=audit(", ":", 11, NO_END, 2);
	//Extract field from record received
	if(messField.data.compare(""))
	{
		_DATETIME();
	}
	else {
		messField.data = messField.data + ";";
		messField.time = messField.time + ";";
	}

	messField.terminal = _FIND(" tty=(", ")", 6, NO_END, 1);
	if ((!messField.terminal.compare("none")) || (!bufStrTwo.compare("NO_STRING")))
	{
		messField.TypeRec = "np";
		messField.TypeRec = messField.TypeRec + ";";
	}
	else {
		messField.TypeRec = "nc";
		messField.TypeRec = messField.TypeRec + ";";
	}

	messField.prca = messField.prca + ";";

	messField.uid = _FIND(" uid=", " ", 5, NO_END, 1);
	struct passwd *p;
	uid_t  uid=atoi(messField.uid.c_str());
	if (messField.uid.size() != 0)
	{
		if ((p = getpwuid(uid)) == NULL)
		{
			ACSALOGLOG(LOG_LEVEL_ERROR,"Get user: getpwuid() error");
			messField.user_name = "unknown";
		}
		else  messField.user_name = p->pw_name;
	}
	else  messField.user_name = "unknown";

	messField.user_name = messField.user_name + ";";

	messField.device = messField.device + ";";

	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	messField.local_host_name = hostname;

	messField.remote_host = _FIND(" node=", " ", 6, NO_END, 2);

	if (messField.remote_host.size() != 0)
	{
		//Get node number
		found = messField.remote_host.length();
		messField.node_number = messField.remote_host.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	messField.pid = sid;
	messField.pid = messField.pid + ";";

	if (MCpSystem == true)
	{
		messField.cmdid = messField.cmdid + ";";
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist    = messField.cpidlist + ";";
		messField.cmdseqnum   = messField.cmdseqnum + ";";
	}
	else {
		if (commandSession)
		{
			messField.cmdid = messField.cmdid + ";";
		}
	}

	messField.msg = _READMSG("apcommand.notice:", "");

	hidePwdInTheZipCommands ();
}

/*===================================================================
   ROUTINE: _PARSE_ALOGAPI
=================================================================== */
void acs_alog_parser::_PARSE_ALOGAPI(int commandSession)
{
	string  buffer;
	size_t  found = 0;
        char error_message[256] = {0};
        char    hostname[1024];
        hostname[1023] = '\0';

	buffer = _FIND(" logdata=", ",", 9, NO_END, 0);

	if (buffer.length() > 10)  messField.data.clear();
	else        			   messField.data.assign(buffer);

	buffer = _FIND(" logtime=", ",", 9, NO_END, 0);
	if (buffer.length() > 6)  messField.time.clear();
	else        			  messField.time.assign(buffer);

	messField.data.append(";");
	messField.time.append(";");

	messField.prca = _FIND(" prca=", ",", 6, NO_END, 0);
	messField.prca = messField.prca + ";";

	messField.user_name = _FIND(" user=", ",", 6, NO_END, 0);
	messField.user_name = messField.user_name + ";";

	messField.device = _FIND(" device=", ",", 8, NO_END, 0);
	messField.device = messField.device + ";";

	messField.remote_host = _FIND(" remotehost=", ",", 12, NO_END, 0);

	messField.local_host_name = "";
	if(strHostname.find(",") != string::npos)		// Dual Stack IPs - Log the same IP version as that of remote_host
	{
		if(messField.remote_host.find(".") != string::npos)
		{
			size_t posDelim = strHostname.find(",");
			messField.local_host_name = strHostname.substr(0, posDelim);		// log only IPv4 address
		}
		else if(messField.remote_host.find(":") != string::npos)
		{
			size_t posDelim = strHostname.find(",");
			size_t len = strHostname.length();
			messField.local_host_name = strHostname.substr(posDelim + 1, len - posDelim + 1);	// log only IPv6 address
		}
		else
			messField.local_host_name = strHostname;
	}
	else
		messField.local_host_name = strHostname; //TR:HY25168
	
	messField.local_host_info = _FIND(" localhostinfo=", ",", 15, NO_END, 0);

	if(messField.local_host_info.compare(""))
	{
		messField.local_host_name = messField.local_host_name + "/" + messField.local_host_info;
	}

	gethostname(hostname, 1023);
	string tempHostName = hostname;
	if (tempHostName.size() != 0)
	{
		found = tempHostName.length();
		messField.node_number = tempHostName.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	messField.pid = _FIND(" ses=", ",", 5, NO_END, 0);
	messField.pid = messField.pid + ";";

	if (MCpSystem == true)
	{
		if (commandSession)
		{
			messField.cmdid = _FIND(" cmdid=", ",", 7, NO_END, 0);
			messField.cmdid = messField.cmdid + ";";
		}
		else    messField.cmdid = messField.cmdid + ";";

		messField.cpnamegroup = _FIND(" cpnamegroup=", ",", 13, NO_END, 0);
		messField.cpnamegroup = messField.cpnamegroup + ";";

		messField.cpidlist = _FIND(" cpidlist=", ",", 10, NO_END, 0);
		messField.cpidlist = _REFORMLIST(messField.cpidlist);
		messField.cpidlist = messField.cpidlist + ";";

		messField.cmdseqnum = _FIND(" cmdseqnum=", ",", 11, NO_END, 0);
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
	else {
		if (commandSession)
		{
			messField.cmdid = _FIND(" cmdid=", ",", 7, NO_END, 0);
			messField.cmdid = messField.cmdid + ";";
		}
	}

	//Read field cmd
	messField.msg = _READMSG("alogapi.notice:", "");

	//Fields for final record//////////////
	messField.TypeRec = _FIND(" type=", ",", 6, NO_END, 0);
	messField.TypeRec = messField.TypeRec + ";";

	if (messField.TypeRec.compare("mcl;") == 0  ||  messField.TypeRec.compare("mli;") == 0)
	{
		if (messField.data.length() == 0  ||  messField.time.length() == 0)  return;

		RecForCLog = 1;	 CLOGevent = "";

		for (int j=2; j<(int)messField.data.length(); j++)        //  j = 2 :  TR HR46115 (format YYMMDD)
		{
			if (messField.data.at(j) >= '0'  &&  messField.data.at(j) <= '9')
			{
				CLOGevent = CLOGevent + messField.data.at(j);
			}
		}
		CLOGevent = CLOGevent + '-';

		CLOGevent = CLOGevent + messField.time.at(0);  CLOGevent = CLOGevent + messField.time.at(1);
		CLOGevent = CLOGevent + messField.time.at(2);  CLOGevent = CLOGevent + messField.time.at(3);

		CLOGevent = CLOGevent + ' ';

		for (int j=0; j<(int)messField.msg.length(); j++)
		{
			if ((unsigned char)messField.msg.at(j) == '\n')  break;

			CLOGevent = CLOGevent + messField.msg.at(j);
		}
		CLOGevent = CLOGevent + '\n';
	}
}
/*===================================================================
   ROUTINE: _PARSE_CLICOMMAND
=================================================================== */
void acs_alog_parser::_PARSE_CLICOMMAND(int commandSession) {

        char hostname[1024];
        size_t found = 0;

        messField.data = _FIND(" sysdate=", " ", 9, NO_END, 0);
        messField.time = _FIND(" systime=", " ", 9, NO_END, 0);

        messField.data = messField.data + ";";
        messField.time = messField.time + ";";

        //Fields for final record//////////////
        messField.TypeRec = "com";
        messField.TypeRec = messField.TypeRec + ";";

        messField.prca = messField.prca + ";";

        messField.user_name = _FIND(" acct=", " ", 6, NO_END, 0);

        if (!messField.user_name.compare(""))
        {
                if (getenv("USER") != NULL)  messField.user_name = getenv("USER");
        }
        string localUserName = messField.user_name;
        messField.user_name = messField.user_name + ";";


        messField.device = messField.device + ";";

        memset (hostname, 0, 1024);
        if (gethostname (hostname, 1024) == 0)  messField.local_host_name = hostname;
        string tempHostName = hostname;
        messField.msg = "";
        //@ messField.remote_host = _FIND(" node=", " ", 6, NO_END, 0);
        //#################################################################################
        //To transform and log the event as Security event especially in case of Cli-Tls connection
        //Example: 2019-04-17 09:21:25 SystemEvent com SC-2-1 info interface=cli  user-name=ts_com  session-id=19 client-ip=10.210.133.153 server-incoming-port=9830.
        int pos1 = bufStrOne.find("addr");
        string remoteIpBuffer="";
        string portNumber="";
        if(pos1 != (int)string::npos)
        {
                ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(bufStrOne));
                int pos2 = bufStrOne.find("rport");

               if(pos2 != (int)string::npos){
                    remoteIpBuffer = bufStrOne.substr(pos1+10,(pos2-(pos1+11))); //remote-ip
                    messField.remote_host = remoteIpBuffer;
                   //portNumber = bufStrOne.substr(pos2+21); //port number is always 9830 for Cli-Tls connection
                    //Fields for final record//////////////
                    messField.TypeRec = "sec";//Transforming into Security event based on Fransesco's suggestion
                    messField.TypeRec = messField.TypeRec + ";";

                      messField.Type = "Successful login";
                      messField.source = "Security";
                      messField.category = "USER_AUTH";
                      messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
                      string message_desc = "COM-CLI session opened for user " + localUserName + " with " ;
                      messField.msg += message_desc ;
                  }
                   if (remoteIpBuffer == "") {
                     remoteIpBuffer = "";
                     messField.remote_host = _FIND(" node=", " ", 6, NO_END, 0);//remoteIpBuffer;
                }
           }
          else{
                    messField.remote_host = _FIND(" node=", " ", 6, NO_END, 0);//remoteIpBuffer;
                                                                                                 
   }
//#############################END#########################################
        if (tempHostName.size() != 0)
        {
                found = tempHostName.length();
                messField.node_number = tempHostName.substr(found - 1,1);
        }

        messField.node_number = messField.node_number + ";";

        messField.pid = _FIND(" ses=", " ", 5, NO_END, 0);
        messField.pid = messField.pid + ";";

        if (MCpSystem == true)
        {
                messField.cmdid = messField.cmdid + ";";
                messField.cpnamegroup = messField.cpnamegroup + ";";
                messField.cpidlist = messField.cpidlist + ";";
                messField.cmdseqnum = messField.cmdseqnum + ";";
        }
        else {
                if (commandSession)
                {
                        messField.cmdid = messField.cmdid + ";";
                }
        }
        //Read field cmd
        messField.msg += _READMSG("clicommand.notice:", "");
}
 


/*===================================================================
   ROUTINE: _PARSE_COMTLS
=================================================================== */
void acs_alog_parser::_PARSE_COMTLS(int commandSession) {

        char hostname[1024];
        size_t found = 0;

        messField.data = _FIND(" sysdate=", " ", 9, NO_END, 0);
        messField.time = _FIND(" systime=", " ", 9, NO_END, 0);
        messField.data = messField.data + ";";
        messField.time = messField.time + ";";

        //Fields for final record//////////////

        messField.prca = messField.prca + ";";

        messField.user_name = _FIND(" acct=", " ", 6, NO_END, 0);

        if (!messField.user_name.compare(""))
        {
                if (getenv("USER") != NULL)  messField.user_name = getenv("USER");
        }
        string localUserName = messField.user_name;
        messField.user_name = messField.user_name + ";";

        messField.device = messField.device + ";";

        memset (hostname, 0, 1024);
        if (gethostname (hostname, 1024) == 0)  messField.local_host_name = hostname;
        string tempHostName = hostname;

        messField.msg = "";
        string remoteIpBuffer="";
        string portNumber="";
        int posSt = bufStrOne.find("Accepted request");
        int posFn = bufStrOne.find("ses=");
        if(posSt != (int)string::npos)
        {
                ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(bufStrOne));

                posSt = bufStrOne.find("addr=");
                posFn = bufStrOne.find("rport=");
                posFn = posFn-1;
                remoteIpBuffer = bufStrOne.substr(posSt+5,(posFn-(posSt+5)));
                messField.remote_host = remoteIpBuffer;

                messField.TypeRec = "sec";
                messField.TypeRec = messField.TypeRec + ";";

                posSt = bufStrOne.find("laddr=");
                posFn = bufStrOne.find("lport=");
                posFn = posFn-1;
                messField.local_host_name = bufStrOne.substr(posSt+6,(posFn-(posSt+6)));

                messField.Type = "Successful login";
                messField.source = "Security";
                messField.category = "USER_AUTH";
                messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
                string message_desc = "TLS session opened for user "+localUserName+" " ;

                posSt = bufStrOne.find("lport=");
                posFn = bufStrOne.find("hostname=");
                posFn = posFn-3;
                string lport= bufStrOne.substr(posSt+6,(posFn-(posSt+6)));
                if(lport == "9830"){
                        message_desc= "CLI over "+message_desc;
                }
                else {
                        message_desc= "NETCONF over "+message_desc;
                }
                messField.msg += message_desc ;
        }
        posSt = bufStrOne.find("Disconnected");
        if(posSt != (int)string::npos){
                messField.TypeRec = "sec";
                messField.TypeRec = messField.TypeRec + ";";
                messField.Type = "Successful logout";
                messField.source = "Security";
                messField.category = "USER_AUTH";
                messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
                string message_desc = "Disconnected from TLS session for user "+localUserName;
                messField.msg += message_desc;
        }
        posSt = bufStrOne.find("Authentication failure");
        if(posSt != (int)string::npos){
                posSt = bufStrOne.find("addr=");
                posFn = bufStrOne.find("rport=");
                posFn = posFn-1;
                remoteIpBuffer = bufStrOne.substr(posSt+5,(posFn-(posSt+5)));
                messField.remote_host = remoteIpBuffer;

                messField.TypeRec = "sec";
                messField.TypeRec = messField.TypeRec + ";";
                messField.Type = "TLS Login Failed";
                messField.source = "Security";
                messField.category = "USER_AUTH";
                messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
                string message_desc = "TLS Login Failed ";
                messField.msg += message_desc ;
                }

//#############################END#########################################  
	if (tempHostName.size() != 0)
        {
                found = tempHostName.length();
                messField.node_number = tempHostName.substr(found - 1,1);
        }

	messField.node_number = messField.node_number + ";";

	messField.pid = _FIND(" ses=", " ",5, NO_END, 0);
	int pos_1 = messField.pid.find(",");
	if(pos_1 != (int)string::npos){
		messField.pid=messField.pid.erase(pos_1,1);
        }
	messField.pid = messField.pid + ";";

	if (MCpSystem == true)
	{
		messField.cmdid = messField.cmdid + ";";
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
	else {
		if (commandSession)
		{
			messField.cmdid = messField.cmdid + ";";
		}
	}
		
	//Read field cmd
	messField.msg += _READMSG("comtls.notice:", "");
}
/*===================================================================
   ROUTINE: _PARSE_CLISSH
=================================================================== */
void acs_alog_parser::_PARSE_CLISSH(int commandSession) {

	char hostname[1024];
	size_t found = 0;

	messField.data = _FIND(" sysdate=", " ", 9, NO_END, 0);
	messField.time = _FIND(" systime=", " ", 9, NO_END, 0);

	messField.data = messField.data + ";";
	messField.time = messField.time + ";";

	//Fields for final record//////////////
	messField.prca = messField.prca + ";";
	int posSt = bufStrOne.find("acct=");
	int posFn = bufStrOne.find("exe=");
	posFn = posFn -2;
	messField.user_name = bufStrOne.substr(posSt+6,(posFn-(posSt+6)));
	
	if (!messField.user_name.compare("")) {
		if (getenv("USER") != NULL)  messField.user_name = getenv("USER");
	}
	string localUserName = messField.user_name;
	messField.user_name = messField.user_name + ";";

	messField.device = messField.device + ";";
	memset (hostname, 0, 1024);
	if (gethostname (hostname, 1024) == 0)  messField.local_host_name = hostname;
	string tempHostName = hostname;
	messField.msg = "";
	string remoteIpBuffer="";
	string portNumber="";
	string sessionType="";
	string result="";
	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(bufStrOne));

	posSt = bufStrOne.find("hostname=");
	posFn = bufStrOne.find("addr");
	posFn = posFn-1;
	messField.local_host_name = bufStrOne.substr(posSt+9,(posFn-(posSt+9)));

	posSt = bufStrOne.find("addr");
	posFn = bufStrOne.find("terminal");
	posFn = posFn-1;
	messField.remote_host = bufStrOne.substr(posSt+5,(posFn-(posSt+5)));
	string remoteHost = messField.remote_host;
	messField.TypeRec = "sec";
	messField.TypeRec = messField.TypeRec + ";";
	
	posSt = bufStrOne.find("ses=");
	posFn = bufStrOne.find("msg='op=");
	posFn = posFn-1;
	messField.pid = bufStrOne.substr(posSt+4,(posFn-(posSt+4)));
	messField.pid = messField.pid + ";";
	
	posSt = bufStrOne.find("msg='op=");
	posFn = bufStrOne.find("acct");
	posFn = posFn-1;
	sessionType = bufStrOne.substr(posSt+5,(posFn-(posSt+5)));
		
	posSt = bufStrOne.find("res=");
	result = bufStrOne.substr(posSt+4,6);

	if (!sessionType.compare("op=PAM:session_open")) {
		messField.Type = "Successful login";
                messField.category = "USER_AUTH";
                messField.source = "Security";
                messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
                string message_desc = "SSH session opened for user "+ localUserName  + " from " + remoteHost ;
                messField.msg += message_desc ;
	}
	else if (!sessionType.compare("op=PAM:session_close")) {
        	messField.Type = "Successful logout";
		messField.source = "Security";
		messField.category = "USER_AUTH";
		messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
		string message_desc = "SSH session closed for user "+localUserName + " from " + remoteHost ;
		messField.msg += message_desc ;
	}
	else {
		if(!(sessionType.compare("op=PAM:authentication")) && !(result.compare("failed"))) {
			messField.Type = "Failed login";
			messField.source = "Security";
			messField.category = "USER_AUTH";
			messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
			string message_desc = "SSH session login failed for user "+localUserName+ " from " + remoteHost ;
			messField.msg += message_desc ;
		}
	}
	
	if (tempHostName.size() != 0) {
		found = tempHostName.length();
		messField.node_number = tempHostName.substr(found - 1,1);
	}

	messField.node_number = messField.node_number + ";";
	if (MCpSystem == true) {
		messField.cmdid = messField.cmdid + ";";
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
	else {
		if (commandSession) {
			messField.cmdid = messField.cmdid + ";";
		}
	}
	//Read field cmd
	messField.msg += _READMSG("clissh.notice:", "");
}


/*===================================================================
   ROUTINE: _PARSE_NETCONFCOMMAND
=================================================================== */
void acs_alog_parser::_PARSE_NETCONFCOMMAND(int commandSession) {

	char hostname[1024];
	size_t found = 0;

	messField.data = _FIND(" sysdate=", " ", 9, NO_END, 0);
	messField.time = _FIND(" systime=", " ", 9, NO_END, 0);

	messField.data = messField.data + ";";
	messField.time = messField.time + ";";

	//Fields for final record//////////////
	messField.TypeRec = "com";
	messField.TypeRec = messField.TypeRec + ";";

	messField.prca = messField.prca + ";";

	messField.user_name = _FIND(" acct=", " ", 6, NO_END, 0);

	if (!messField.user_name.compare(""))
	{
		if (getenv("USER") != NULL)  messField.user_name = getenv("USER");
	}
	messField.user_name = messField.user_name + ";";

	messField.device = messField.device + ";";

	memset (hostname, 0, 1024);
	if (gethostname (hostname, 1024) == 0)  messField.local_host_name = hostname;

	messField.remote_host = _FIND(" node=", " ", 6, NO_END, 0);

	if (messField.remote_host.size() != 0)
	{
		//Get node number
		found = messField.remote_host.length();
		messField.node_number = messField.remote_host.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	messField.pid = _FIND(" ses=", " ", 5, NO_END, 0);
	messField.pid = messField.pid + ";";

	if (MCpSystem == true)
	{
		messField.cmdid = messField.cmdid + ";";
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
	else {
		if (commandSession)
		{
			messField.cmdid = messField.cmdid + ";";
		}
	}
	//Read field cmd
	messField.msg = _READMSG("netconfcommand.notice:", "");
}



/*===================================================================
   ROUTINE: _PARSE_CMWEA
=================================================================== */
void acs_alog_parser::_PARSE_CMWEA(int commandSession, string sid) {

	char    hostname[1024], username[1024];
	size_t  found = 0;

	messField.data = _FIND(" sysdate=", " ", 9, NO_END, 0);
	messField.time = _FIND(" systime=", " ", 9, NO_END, 0);

	messField.data = messField.data + ";";
	messField.time = messField.time + ";";

	messField.TypeRec = "cmw";
	messField.TypeRec = messField.TypeRec + ";";

	messField.prca = messField.prca + ";";

	memset (username, 0, 1024);
	if (getenv("USER") != NULL)        //  HP19448
	{
		strcpy (username, getenv("USER"));
	}
	else {
		strcpy (username, "unknown");
	}

	messField.user_name = username;
	messField.user_name = messField.user_name + ";";

	messField.device = messField.device + ";";

	memset (hostname, 0, 1024);
	if (gethostname (hostname, 1024) == 0)  messField.local_host_name = hostname;                 //  HP19448
	else									messField.local_host_name = "HostName undefined";

	//		messField.local_host_name = messField.local_host_name + ";";       PATCH :  to be investigated

	messField.remote_host = _FIND(" node=", " ", 6, NO_END, 0);

	if (messField.remote_host.size() != 0)
	{
		found = messField.remote_host.length();
		messField.node_number = messField.remote_host.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	messField.pid = sid;
	messField.pid = messField.pid + ";";

	if (MCpSystem == true)
	{
		messField.cmdid       = messField.cmdid + ";";
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist    = messField.cpidlist + ";";
		messField.cmdseqnum   = messField.cmdseqnum + ";";
	}
	else {
		if (commandSession)  messField.cmdid = messField.cmdid + ";";
	}

	messField.msg = _READMSG("cmwea.notice:", "");
}

/*===================================================================
   ROUTINE: _PARSE_SECURITY
=================================================================== */
void acs_alog_parser::_PARSE_SECURITY(int commandSession, string sid)
{
	char hostname[1024];
	size_t found = 0;

	Sec = 1;

	_READREC();    			// Extract field from record received

	if(messField.data.compare(""))
	{
		_DATETIME();
	}
	else {
		messField.data = messField.data + ";";
		messField.time = messField.time + ";";
	}

	messField.source = "Security";

	messField.category = messField.Type;

	messField.Type = "Success Audit";

	messField.TypeRec = "sec";
	messField.TypeRec = messField.TypeRec + ";";

	messField.prca = messField.prca + ";";

	struct passwd *p;
	uid_t  uid = atoi(messField.uid.c_str());

	if ((p = getpwuid(uid)) == NULL)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("get user: getpwuid() error"));
		messField.user_name = "unknown";
	}
	else  messField.user_name = p->pw_name;

	messField.user_name = messField.user_name + ";";

	messField.device = messField.device + ";";

	messField.pid = sid;
	messField.pid = messField.pid + ";";

	messField.msg = _READMSG("", messField.user_pid);

	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	messField.local_host_name = hostname;

	if (messField.local_host_name.size() != 0)
	{
		found = messField.local_host_name.length();
		messField.node_number = messField.local_host_name.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	if (MCpSystem == true)
	{
		messField.cmdid = messField.cmdid + ";";
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
	else {
		if (commandSession)  messField.cmdid = messField.cmdid + ";";
	}
}

void acs_alog_parser::_PARSE_TELNET_CONNECTION(int commandSession, string sid)
{
	char hostname[16] = {0};
	size_t found = 0;

	char msg_time[80] = {0};
	char msg_data[80] = {0};
	std::string message_desc = "";

	Sec = 1;

	//_READREC();    			// Extract field from record received

	messField.daemon_state = "";

	messField.pid = "";

	messField.uid = "";

	messField.terminal = "";

	messField.res = "";

	//_DATETIME();

	time_t currecttime = time(NULL);

	struct tm * timeinfo;
	timeinfo = localtime(&currecttime);

	strftime (msg_data,80,"%Y-%m-%d",timeinfo);
	messField.data = msg_data;
	messField.data = messField.data + ";";

	strftime (msg_time,80,"%H%M%S",timeinfo);
	messField.time = msg_time;
	messField.time = messField.time + ";";

	messField.source = "Security";

	messField.category = "USER_AUTH";

	if((bufStrOne.find("FAILED LOGIN") != string::npos))		//Telnet Failed login case
	{
		messField.Type = "Failed login";
	}
	else if((bufStrOne.find("LOGIN ON") != string::npos))
	{
		messField.Type = "Successful login";
	}else if((bufStrOne.find("session closed for user") != string::npos))
        {
                messField.Type = "Logout successful";
        }  																	//end of TR HU40809

	messField.TypeRec = "sec;";

	messField.prca = messField.prca + ";";

	struct passwd *p;
	uid_t  uid = atoi(messField.uid.c_str());

	if ((p = getpwuid(uid)) == NULL)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("get user: getpwuid() error"));
		messField.user_name = "unknown";
	}
	else    messField.user_name = p->pw_name;

	messField.device = messField.device + ";";

	messField.pid = sid;
	messField.pid = messField.pid + ";";
        
	messField.remote_host="";
                
		size_t pos1 = bufStrOne.find("LOGIN ON");  // Then search for other substrings which relate to different login events
                if(pos1 != string::npos)
                {
			size_t pos2 = bufStrOne.find("FROM");
			if(pos2 != string::npos)
			{
        	        	messField.user_name = bufStrOne.substr(pos1+18,((pos2-1)-(pos1+18)));
				string RHWithNewLineChar = bufStrOne.substr(pos2+5);
				int avoidNewLine = (RHWithNewLineChar.length()-1);
				string remoteHost = bufStrOne.substr((pos2+5),avoidNewLine);
				messField.remote_host = remoteHost;
			}
			else{
				messField.remote_host = "";
			}
			message_desc ="User " + messField.user_name + " logged in successfully with " + messField.remote_host;
		}
		else if (bufStrOne.find("FAILED LOGIN") != string::npos)
		{
			ACSALOGLOG(LOG_LEVEL_INFO,bufStrOne);
			size_t pos2 = bufStrOne.find("FROM");
			if(pos2 != string::npos)
			{
				size_t pos3 = bufStrOne.find("FOR");
				if(pos3 != string::npos){
					messField.remote_host = bufStrOne.substr((pos2+5),((pos3-1)-(pos2+5)));
                        
					size_t pos4 = bufStrOne.find("Authentication failure");//error: PAM: Authentication failure for ts_com from 10.210.133.149
		                        if(pos4 != string::npos){
						messField.user_name = bufStrOne.substr(pos3+4,((pos4-2)-(pos3+4)));
					}
				}else{
					messField.remote_host="";
				}
			}
			else{
	                        messField.remote_host = "";
        		}	
			message_desc ="User "+ messField.user_name + " failed to login with " + messField.remote_host;
		}
		else 
                {
                	size_t pos2 = bufStrOne.find("session closed for user");

                        if(pos2 != string::npos){
				string uNameWithNewLineChar = bufStrOne.substr(pos2+24); 
				int avoidNewLine = (uNameWithNewLineChar.length() - 1);
				string uName = bufStrOne.substr((pos2+24),avoidNewLine);
				messField.user_name = uName; ///gives the user name of Logged out TELNET connection
                        }

			message_desc ="Telnet session closed for user " + messField.user_name ;
		}

	message_desc=message_desc+"\n";
	messField.user_name = messField.user_name + ";";	

	messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: " + message_desc;

	gethostname(hostname, sizeof(hostname));
	messField.local_host_name = hostname;

	if (messField.local_host_name.size() != 0)
	{
		found = messField.local_host_name.length();
		messField.node_number = messField.local_host_name.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	if (commandSession)  messField.cmdid = messField.cmdid + ";";

	if (MCpSystem)
	{
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
	messField.remote_host;
}

void acs_alog_parser::_PARSE_SECURITY_PAM(int commandSession, string sid)
{
	char hostname[16] = {0};
	size_t found = 0;

	char msg_time[80] = {0};
	char msg_data[80] = {0};
	std::string message_desc = "";
	int position;
	bool isSshUserName = false;
	Sec = 1;

	//_READREC();    			// Extract field from record received

	messField.daemon_state = "";

	messField.pid = "";

	messField.uid = "";

	messField.terminal = "";

	messField.res = "";

	//_DATETIME();

	time_t currecttime = time(NULL);

	struct tm * timeinfo;
	timeinfo = localtime(&currecttime);

	strftime (msg_data,80,"%Y-%m-%d",timeinfo);
	messField.data = msg_data;
	messField.data = messField.data + ";";

	strftime (msg_time,80,"%H%M%S",timeinfo);
	messField.time = msg_time;
	messField.time = messField.time + ";";

	messField.source = "Security";

	messField.category = "USER_AUTH";

	if((bufStrOne.find("error: PAM") != string::npos) || (bufStrOne.find("Failed password for") != string::npos) || 
		((bufStrOne.find("sshd:account")!= string::npos) && ((bufStrOne.find("Access denied")!= string::npos) ||(bufStrOne.find("expired")!= string::npos))))
	{
		messField.Type = "Failed login";
	}
	else if((bufStrOne.find("Accepted keyboard-interactive/pam") != string::npos) || (bufStrOne.find("Accepted password for") != string::npos))
	{
		messField.Type = "Successful login";
	}																	//end of TR HU40809

	messField.TypeRec = "sec;";

	messField.prca = messField.prca + ";";

	struct passwd *p;
	uid_t  uid = atoi(messField.uid.c_str());

	if ((p = getpwuid(uid)) == NULL)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("get user: getpwuid() error"));
		messField.user_name = "unknown";
	}
	else    messField.user_name = p->pw_name;

	messField.device = messField.device + ";";

	messField.pid = sid;
	messField.pid = messField.pid + ";";
        
	messField.remote_host="";
       int pos1 = bufStrOne.find("error: PAM");         //start of TR HU40809
        if(pos1 == (int)string::npos)                    // If "error: PAM" not found
        {
                pos1 = bufStrOne.find("Failed password for");  // Then search for other substrings which relate to different login events
                if(pos1 == (int)string::npos)
                {
                        pos1 = bufStrOne.find("Accepted keyboard-interactive/pam for");
                        if(pos1 == string::npos)
                        {
                                pos1 = (bufStrOne.find("Accepted password for"));
				if(pos1 != string::npos)
				{
					int pos2 = bufStrOne.find("from");
					if(pos2 != string::npos)
                        	                messField.user_name = bufStrOne.substr(pos1+22,(pos2-(pos1+22)));
					
					int pos3 = bufStrOne.find("port");
					if(pos3 != string::npos)
						messField.remote_host = bufStrOne.substr(pos2+5,(pos3-(pos2+5)));
				}
			}
			else
			{
				//"Accepted keyboard-interactive/pam for"
				size_t pos2 = bufStrOne.find("from");
				if(pos2 != string::npos)
                        	        messField.user_name = bufStrOne.substr(pos1+38,(pos2-(pos1+39)));
				
				size_t pos3 = bufStrOne.find("port");
				if(pos3 != string::npos)
					messField.remote_host = bufStrOne.substr(pos2+5,(pos3-(pos2+6)));
			}
                }
		else{
			    //"Failed password for"
				string user_name="";
				size_t pos2 = bufStrOne.find("from");
				if(pos2 != string::npos)
				{
					size_t pos3 = bufStrOne.find("port");
					if(pos3 != string::npos)
						messField.remote_host = bufStrOne.substr(pos2+5,(pos3-(pos2+6)));
					else
                        		        messField.remote_host ="";
					
					size_t pos4 = bufStrOne.find("for");
					if(pos4 != string::npos)
					{
						string uName = bufStrOne.substr(pos4+4,(pos2-(pos4+5)));
		                                if (uName.find("invalid") != std::string::npos)
                		                {
                                		        //"Failed password for invalid user ecutuser from 10.210.139.158 port 42185 ssh2"
                                        		size_t pos2 = uName.find("user");
                                        		if(pos2 != string::npos)
                                                		user_name = uName.substr(pos2+5);
                                		}
                                		else{
							//"Failed password for ts_test from 10.210.139.158 port 61675 ssh2"
                                        		user_name = uName;
                                		}
                                		messField.user_name = user_name;
					}
				}						

		   }
        }
	else 
	{
		string user_name="";
		//Code for SSH failed login case "error: PAM: Authentication failure for ts_com from 10.210.139.158"
		size_t pos2 = bufStrOne.find("for");
                if(pos2 != string::npos)
                {
			size_t pos3 = bufStrOne.find("from");
                        if(pos3 != string::npos)
			{
                        	//@ messField.user_name = bufStrOne.substr(pos2+4,(pos3-(pos2+4)));
				string uName = bufStrOne.substr(pos2+4,(pos3-(pos2+5)));
				if (uName.find("illegal") != std::string::npos)
				{
					//"error: PAM: Authentication failure for illegal user xpamsur from 10.210.139.158"
					size_t pos2 = uName.find("user");
					if(pos2 != string::npos)
						user_name = uName.substr(pos2+5);						
				}
				else{
					user_name = uName;
				}
                        	messField.user_name = user_name;
			}
			
			size_t pos4 = bufStrOne.find("Authentication failure");
			if(pos4 == (int)string::npos)
				pos4 = bufStrOne.find("User account has expired");
                        if(pos4 != string::npos)
			{
                        	string remoteHostWithNewLineChar = bufStrOne.substr(pos3+5);
				int avoidNewLine = (remoteHostWithNewLineChar.length() - 1);
				string remoteHost = bufStrOne.substr((pos3+5),avoidNewLine);
				messField.remote_host = remoteHost;
			}
			else 
                                messField.remote_host = "";

		}
        }
	
	if ((bufStrOne.find("sshd:account")!= string::npos) && ((bufStrOne.find("expired")!= string::npos)))
        {
        //SecurityEvent sshd SC-2-2 notice pam_unix(sshd:account): account ts_comem1 has expired (failed to change password)
                size_t pos2 = bufStrOne.find(": account");
                if (pos2 != string::npos)
                {
                        size_t pos3 = bufStrOne.find("has expired");
                        if (pos3 != string::npos){
                                messField.user_name = bufStrOne.substr(pos2+10,((pos3)-(pos2+10)));
                        }
                }
                messField.remote_host = "";
        }
        else if ((bufStrOne.find("pam_sss(sshd:account)")!= string::npos) && ((bufStrOne.find("Access denied")!= string::npos)))
        {
        	//2019-07-02 12:33:04 SecurityEvent sshd SC-2-2 notice pam_sss(sshd:account): Access denied for user xgeompa: 6 (Permission denied)
                size_t pos2 = bufStrOne.find("user");
                if (pos2 != string::npos)
                {
                        size_t pos3 = bufStrOne.find("Permission denied");
                        if (pos3 != string::npos){
                                messField.user_name = bufStrOne.substr(pos2+5,((pos3-5)-(pos2+5)));
                        }
                }
                messField.remote_host = "";
        }
	else if(bufStrOne.find("Disconnected from") != string::npos)
	{
		///Code SSH logout- user name can be displayed "Disconnected from 10.210.131.135 port 59476"
		size_t pos2 = bufStrOne.find("Disconnected from");
        	if(pos2 != string::npos){
			size_t pos3 = bufStrOne.find("port");
                	if(pos3 != string::npos){
				isSshUserName=true;
                      		messField.remote_host = bufStrOne.substr(pos2+18,(pos3-(pos2+19)));
				messField.Type = "Logout Successful";
                        	message_desc = "User logged out from SSH Session\n";
			}
                	else{
                       		messField.remote_host = "";		
	    	       		messField.Type = "";
                       		message_desc = "";
			}
		
		}
	}
	else if (bufStrOne.find("session closed for user")!= string::npos)
        {
		isSshUserName = true;
                size_t pos2 = bufStrOne.find("session closed for user");

                if(pos2 != string::npos){
                               string userNameWithNewLineChar = bufStrOne.substr(pos2+24);
				int avoidNewLine = (userNameWithNewLineChar.length() -1); 
                               messField.user_name = bufStrOne.substr((pos2+24),avoidNewLine);
                               messField.Type = "Logout Successful";
                               message_desc = "User logged out from SSH Session\n";
                }
        }

	messField.user_name = messField.user_name + ";";	

	position = bufStrOne.find("error: PAM");							//start of TR HU40809

	if(position == (int)string::npos)			// If "error: PAM" not found
	{
		position = bufStrOne.find("Failed password for");	// Then search for other substrings which relate to different login events
		if(position == (int)string::npos)
		{
			position = bufStrOne.find("Accepted keyboard-interactive/pam");
			if(position == (int)string::npos)
			{
				position = bufStrOne.find("Accepted password for");
				if(position == (int)string::npos)
				{
					position = bufStrOne.find("pam_sss(sshd:account)");
					if(position != (int)string::npos) 
					{
						position = bufStrOne.find("sshd:account");
                                                position +=15; 
					}   

				}
			}
		}
	}
	else											// If 'error: PAM' is found
		position += 11;							// Then point to the index after the substring "error: PAM"

	if(position == string::npos)
 		position = bufStrOne.find("sshd");
	
	if(isSshUserName){
		messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: " + message_desc ;
	}
	else
		messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: " + bufStrOne.substr ( position );
        

	//end of TR HU40809
	gethostname(hostname, sizeof(hostname));
	messField.local_host_name = hostname;

	if (messField.local_host_name.size() != 0)
	{
		found = messField.local_host_name.length();
		messField.node_number = messField.local_host_name.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	if (commandSession)  messField.cmdid = messField.cmdid + ";";

	if (MCpSystem)
	{
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
	messField.remote_host;
}


void acs_alog_parser::_PARSE_SECURITY_PAM_FTP(int commandSession, string sid)
{
	char hostname[16] = {0};
	size_t found = 0;

	char msg_time[80] = {0};
	char msg_data[80] = {0};
	std::string message_desc = "";
	std::string remoteIpBuffer = "";
	
	Sec = 1;

	//_READREC();                           // Extract field from record received

	messField.daemon_state = "";

	messField.pid = "";

	messField.uid = "";

	messField.terminal = "";

	messField.res = "";

	//_DATETIME();

	time_t currecttime = time(NULL);

	struct tm * timeinfo;
	timeinfo = localtime(&currecttime);

	strftime (msg_data,80,"%Y-%m-%d",timeinfo);
	messField.data = msg_data;
	messField.data = messField.data + ";";

	strftime (msg_time,80,"%H%M%S",timeinfo);
	messField.time = msg_time;
	messField.time = messField.time + ";";

	messField.source = "Security";

	messField.category = "USER_AUTH";

	messField.TypeRec = "sec;";

	messField.prca = messField.prca + ";";
	
        struct passwd *p;
        uid_t  uid = atoi(messField.uid.c_str());

        if ((p = getpwuid(uid)) == NULL)
        {
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("get user: getpwuid() error"));
                messField.user_name = "unknown";
        }
        else {
	   messField.user_name = p->pw_name;
	}

	ACSALOGLOG(LOG_LEVEL_INFO,bufStrOne.c_str());
	if ((bufStrOne.find("vsftpd") != string::npos) && 
	( (bufStrOne.find("pam_sss(vsftpd:auth): authentication success")!= string::npos) || (bufStrOne.find("(vsftpd:auth): authentication failure")!= string::npos)))
        {
		///For FTP connection(Unsecure) with Port 21 ----- NOT related to TLS feature
                string tempBuffer = bufStrOne.substr(bufStrOne.find("rhost="));
                size_t pos2 = tempBuffer.find("rhost=");
                if (pos2 != string::npos)
                {
                        size_t pos3 = tempBuffer.find("user=");
                        if (pos3 != string::npos){
                                messField.remote_host = tempBuffer.substr(pos2+6,((pos3)-(pos2+6)));
                                string userNameWithNewLineChar = tempBuffer.substr(pos3+5);
		
                               int avoidNewLine = (userNameWithNewLineChar.length() -1);
                               messField.user_name = tempBuffer.substr((pos3+5),avoidNewLine);
                        }
                }
		
		if (bufStrOne.find("pam_sss(vsftpd:auth): authentication success") != string::npos)
		{
                 ///If not for "session closed" then enter into this condition for "session opened"
			messField.Type = "Login Successful";                
                }
                else if (bufStrOne.find("(vsftpd:auth): authentication failure") != string::npos)
                {
			messField.Type = "Login Failed";
                }

	} 
	else if(((bufStrOne.find("vsftpd") != string::npos) && ( (bufStrOne.find("session closed for user")!= string::npos)||(bufStrOne.find("session opened for user")!= string::npos))))
	{
                        size_t pos2 = bufStrOne.find("session closed for user");

			if(pos2 == string::npos) ///If not for "session closed" then enter into this condition for "session opened"
			{
				pos2 = bufStrOne.find("session opened for user");
				if (pos2!=string::npos)
				{	
                        		size_t pos3 = bufStrOne.find("by (uid=0)");
		                        if(pos3 != string::npos){
                                		messField.user_name = bufStrOne.substr(pos2+24,((pos3-1)-(pos2+24)));
		                                messField.Type = "FTP Login Successful";
                        		}
				}

	
			   }
	                   else if(pos2 != string::npos){
				string uNameWithNewLineChar = bufStrOne.substr(pos2+24);
                                int avoidNewLine = (uNameWithNewLineChar.length() - 1);
                                string uName = bufStrOne.substr((pos2+24),avoidNewLine);
                                messField.user_name = uName; ///gives the user name of Logged out FTP connection
                                messField.Type = "FTP Logout Successful";
                        }
       } 


	messField.user_name = messField.user_name + ";";

	messField.device = messField.device + ";";

	messField.pid = sid;
	messField.pid = messField.pid + ";";

	///Position number to display the description part
       size_t position = bufStrOne.find("authentication success"); //For Ldap & Local users   

        if(position == (int)string::npos)                      
        {
		position = bufStrOne.find("authentication failure");
		if (position == (int)string::npos)
		{
	                position = bufStrOne.find("session opened");       //For TS user
        	        if(position == (int)string::npos)
                	{
                        	position = bufStrOne.find("session closed"); // for TS user
                        	if(position == (int)string::npos)
	                        {
					position= bufStrOne.find("vsftpd["); //default cases
			}	}
		}
	}
                            	

	messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: " + bufStrOne.substr ( position );

	gethostname(hostname, sizeof(hostname));
	messField.local_host_name = hostname;

	if (messField.local_host_name.size() != 0)
	{
		found = messField.local_host_name.length();
		messField.node_number = messField.local_host_name.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	if (commandSession)  messField.cmdid = messField.cmdid + ";";

	if (MCpSystem)
	{
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
}


void acs_alog_parser::_PARSE_SECURITY_PAM_TLS(int commandSession, string sid)
{
	char hostname[16] = {0};
	size_t found = 0;

	char msg_time[80] = {0};
	char msg_data[80] = {0};
	std::string message_desc = "";
	std::string remoteIpBuffer = "";
	bool isFailureCase=false;
	bool isFtpTlsConnection=false,isCertificateAuthenticationFailed=false,isAuthenticationTokenExpired=false;
	
	Sec = 1;

	//_READREC();                           // Extract field from record received

	messField.daemon_state = "";

	messField.pid = "";

	messField.uid = "";

	messField.terminal = "";

	messField.res = "";

	//_DATETIME();

	time_t currecttime = time(NULL);

	struct tm * timeinfo;
	timeinfo = localtime(&currecttime);

	strftime (msg_data,80,"%Y-%m-%d",timeinfo);
	messField.data = msg_data;
	messField.data = messField.data + ";";

	strftime (msg_time,80,"%H%M%S",timeinfo);
	messField.time = msg_time;
	messField.time = messField.time + ";";

	messField.source = "Security";

	messField.category = "USER_AUTH";

	messField.TypeRec = "sec;";

	messField.prca = messField.prca + ";";
	
        struct passwd *p;
        uid_t  uid = atoi(messField.uid.c_str());

        if ((p = getpwuid(uid)) == NULL)
        {
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("get user: getpwuid() error"));
                messField.user_name = "unknown";
        }
        else {
	   messField.user_name = p->pw_name;
		tlsUserName = p->pw_name;
	}

        ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(bufStrOne));
        if (((bufStrOne.find("com-tlsd") != string::npos) && (bufStrOne.find("pam_sss(com-tlsd:account)") != string::npos) ))
        {
                int pos1 = bufStrOne.find("Access denied for user");

                if(pos1 != (int)string::npos)
                {
                        if(bufStrOne.find("(User not ") != string::npos)
                        {
                                int pos2 = bufStrOne.find("User not known to the underlying authentication module");
                                if(pos2 != (int)string::npos)
                                {
                                        tlsUserName = bufStrOne.substr(pos1+23, ((pos2-6)-(pos1+23)));
                                        messField.user_name = tlsUserName;
                                }
                        }else{
                                size_t pos2 = bufStrOne.find("Permission denied");
                                if(pos2 != (int)string::npos)
                                {
                                        tlsUserName = bufStrOne.substr(pos1+23, ((pos2-5)-(pos1+23)));
                                        messField.user_name = tlsUserName;
                                }

                        }
                }
        }

	if ((bufStrOne.find("TTY=") != string::npos) && ((bufStrOne.find("tls-netconf") != string::npos) || (bufStrOne.find("tls-cliss") != string::npos)))
	{
		int pos1 = bufStrOne.find("USER=");

		if(pos1 != (int)string::npos)
		{
			int pos2 = bufStrOne.find(";", pos1+5);
			if(pos2 != (int)string::npos){
				tlsUserName = bufStrOne.substr(pos1+5, (pos2-(pos1+6)));  
				messField.user_name = tlsUserName;
			}
		}	
	}
	else if (((bufStrOne.find("vsftpd") != string::npos) && ((bufStrOne.find("OK LOGIN: addr") != string::npos)||(bufStrOne.find("FAIL LOGIN") != string::npos)) && (bufStrOne.find("anon password") == string::npos) ) ||((bufStrOne.find("OK DOWNLOAD: addr") != string::npos)||(bufStrOne.find("OK UPLOAD: addr") != string::npos)||(bufStrOne.find("OK DELETE: addr") != string::npos)||(bufStrOne.find("OK RENAME: addr") != string::npos)||(bufStrOne.find("OK RMDIR") != string::npos)||(bufStrOne.find("OK MKDIR") != string::npos)||(bufStrOne.find("FAIL MKDIR") != string::npos)||(bufStrOne.find("FAIL RENAME") != string::npos)|| (bufStrOne.find("FAIL RMDIR") != string::npos)||(bufStrOne.find("FAIL DELETE") != string::npos)||(bufStrOne.find("FAIL UPLOAD") != string::npos)||(bufStrOne.find("FAIL DOWNLOAD") != string::npos)|| (bufStrOne.find("OK LOGOUT") != string::npos)))        {
		int pos1 = bufStrOne.find("vsftpd");
		remoteIpBuffer="";
		if(pos1 != (int)string::npos)
		{
			if (bufStrOne.find("OK LOGIN: addr")!= string::npos)
			{
				int pos2 = bufStrOne.find("OK LOGIN: addr");

				if(pos2 != (int)string::npos){
					//tlsUserName = bufStrOne.substr(pos1+20, (pos2-(pos1+22))); ///gives the user name of FTPOverTls connection
					tlsUserName=_FIND(" acct=", " ", 6, NO_END, 0);
					messField.user_name = tlsUserName;
				}
				tlsConnectionStatus="OK LOGIN";
			}
			else if (bufStrOne.find("FAIL LOGIN")!= string::npos)
			{
				int pos2 = bufStrOne.find("FAIL LOGIN");

				if(pos2 != (int)string::npos){
//					tlsUserName = bufStrOne.substr(pos1+23, (pos2-(pos1+25))); ///gives the user name of FTPOverTls connection
					 tlsUserName=_FIND(" acct=", " ", 6, NO_END, 0);
					messField.user_name = tlsUserName;
				}
				tlsConnectionStatus="FAIL LOGIN";
			}
			 else if (bufStrOne.find("OK LOGOUT")!= string::npos)
                        {
                                int pos2 = bufStrOne.find("OK LOGOUT");

                                if(pos2 != (int)string::npos){
//                                      tlsUserName = bufStrOne.substr(pos1+23, (pos2-(pos1+25))); ///gives the user name of FTPOverTls connection
                                         tlsUserName=_FIND(" acct=", " ", 6, NO_END, 0);
                                        messField.user_name = tlsUserName;
                                }
                                tlsConnectionStatus="OK LOGOUT";
                        }

			else if ((bufStrOne.find("OK DOWNLOAD: addr") != string::npos)|| (bufStrOne.find("OK UPLOAD: addr")!= string::npos)||(bufStrOne.find("OK DELETE: addr")!= string::npos)||(bufStrOne.find("OK RENAME: addr") != string::npos)||(bufStrOne.find("OK RMDIR") != string::npos)||(bufStrOne.find("OK MKDIR") != string::npos)||(bufStrOne.find("FAIL DOWNLOAD: addr") != string::npos)||(bufStrOne.find("FAIL UPLOAD: addr") != string::npos)||(bufStrOne.find("FAIL DELETE: addr") != string::npos)||(bufStrOne.find("FAIL RENAME: addr") != string::npos)||(bufStrOne.find("FAIL RMDIR") != string::npos)||(bufStrOne.find("FAIL MKDIR") != string::npos))
			{
				tlsConnectionStatus="COMMAND";
				isFtpTlsConnection=true;
				size_t pos2=0;

				if (bufStrOne.find("OK")!= string::npos){
					pos2 = bufStrOne.find("OK");
					message_desc = bufStrOne.substr(pos2+3);
				}
				else {
					pos2 = bufStrOne.find("FAIL");
					message_desc = bufStrOne.substr(pos2+5);
				}
				if ((bufStrOne.find("OK MKDIR: addr")!= string::npos)||(bufStrOne.find("OK RMDIR: addr")!= string::npos)||(bufStrOne.find("OK UPLOAD: addr")!= string::npos)||(bufStrOne.find("OK DOWNLOAD: addr")!= string::npos)||(bufStrOne.find("OK RENAME: addr")!= string::npos)||(bufStrOne.find("OK DELETE: addr")!= string::npos)){
					messField.Type = "FTP-TLS OPERATION SUCCESS";
				}
				else if ((bufStrOne.find("FAIL MKDIR: addr")!= string::npos)||(bufStrOne.find("FAIL RMDIR: addr")!= string::npos)||(bufStrOne.find("FAIL UPLOAD: addr")!= string::npos)||(bufStrOne.find("FAIL DOWNLOAD: addr")!= string::npos)||(bufStrOne.find("FAIL RENAME: addr")!= string::npos)||(bufStrOne.find("FAIL DELETE: addr")!= string::npos)){
					messField.Type = "FTP-TLS OPERATION FAILED";
				}

			//	tlsUserName = bufStrOne.substr(pos1+20,(pos2-(pos1+22)));      ///gives the user name for FTPOverTLS connection
				 tlsUserName=_FIND(" acct=", " ", 6, NO_END, 0);
				messField.user_name = tlsUserName;
			}

                       if ( (bufStrOne.find("OK LOGIN: addr") != string::npos)||(bufStrOne.find("FAIL LOGIN") != string::npos) || (bufStrOne.find("OK DOWNLOAD: addr") != string::npos)|| (bufStrOne.find("OK UPLOAD: addr")!= string::npos)||(bufStrOne.find("OK DELETE: addr")!= string::npos)||(bufStrOne.find("OK RENAME: addr") != string::npos)||(bufStrOne.find("OK RMDIR: addr") != string::npos)||(bufStrOne.find("OK MKDIR: addr") != string::npos)||(bufStrOne.find("FAIL DOWNLOAD: addr") != string::npos)||(bufStrOne.find("FAIL UPLOAD: addr") != string::npos)||(bufStrOne.find("FAIL DELETE: addr") != string::npos)||(bufStrOne.find("FAIL RENAME: addr") != string::npos)||(bufStrOne.find("FAIL RMDIR: addr") != string::npos)||(bufStrOne.find("OK DELETE: addr")!= string::npos)||(bufStrOne.find("FAIL MKDIR: addr") != string::npos) || (bufStrOne.find("anon password") == string::npos)||(bufStrOne.find("OK LOGOUT") != string::npos) )
                        {
                        //To get the remote IP address for all the events based on FTP oVer TLS connection
                                int pos2 = bufStrOne.find("::ffff:");
                                if(pos2 != string::npos)
                                {
                                //Case-1:2019-04-17 15:21:36 SecurityEvent vsftpd SC-2-2 info [ts_com] OK RMDIR: Client "::ffff:10.210.133.153", "/sw_package/jh"
                                //Case-2:2019-04-17 15:25:24 SecurityEvent vsftpd SC-2-2 info [ts_com] FAIL MKDIR: Client "::ffff:10.210.133.153", "/etc/g"
                                //Case-3:2019-04-17 15:26:18 SecurityEvent vsftpd SC-2-2 info [ts_com] FAIL DOWNLOAD: Client "::ffff:10.210.133.153", "/etc/d", 0.00Kbyte/sec
                                //Case-4:"2019-03-02 11:59:52 SecurityEvent vsftpd SC-2-2 info [xyz] OK LOGIN: Client \"::ffff:137.58.117.145\"";
                                //Case-5:"2019-03-19 06:37:12 SecurityEvent vsftpd SC-2-2 warning [ts_tcs] FAIL LOGIN: Client \"::ffff:137.58.117.143\"";
					int pos3 = bufStrOne.find("\"",(pos2+7)); //searching for "
                                        if (pos3 != string::npos)
                                        {
                                                remoteIpBuffer = bufStrOne.substr(pos2+7,(pos3-(pos2+7)));
                                                messField.remote_host = remoteIpBuffer;
                                        }
                                }

                        }

		}
	}
	else if ((bufStrOne.find("Authentication failure")!= string::npos)){
		messField.user_name = tlsUserName;
	}

	messField.user_name = messField.user_name + ";";

	messField.device = messField.device + ";";

	messField.pid = sid;
	messField.pid = messField.pid + ";";


	size_t position;
	bool tls_cliss_netconf = false;
	if ( ((bufStrOne.find("TTY=") != string::npos) && ((bufStrOne.find("tls-netconf") != string::npos) || (bufStrOne.find("tls-cliss") != string::npos)))
			|| ((bufStrOne.find("vsftpd") != string::npos) && ((bufStrOne.find("OK LOGIN: addr") != string::npos)|| (bufStrOne.find("FAIL LOGIN") != string::npos)) &&
					(bufStrOne.find("anon password") == string::npos) ))
	{
		position = bufStrOne.find("TTY=");
		tls_cliss_netconf = true;
	}

	if(tls_cliss_netconf == false) 
	{
		if ((bufStrOne.find("com-tlsd") != string::npos))
		{
			position=0;
			if (bufStrOne.find("PAM: pam_acct_mgmt") != string::npos)
			{
				position = bufStrOne.find("PAM: pam_acct_mgmt"); //positioned as 63//correct
				if(position == string::npos)
				{
					position =0;
					position = bufStrOne.find("pam_sss(com-tlsd:");

					if(position != string::npos)
					{
						position = bufStrOne.find("): ", position) + 3;
					}
				}
				else
				{
					tlsConnectionStatus = bufStrOne.substr(position); //Failure case pam_acct_mgmt = 0,7 &12
				}
			}
			else if (bufStrOne.find("Failed TLS connection accept") != string::npos) 
		//|| (bufStrOne.find("Error starting server com-tlsd") != string::npos) || (bufStrOne.find("Failed to Start TLS session") != string::npos) )) 
	        	{	
        	        //Use Case: When correct certificates are installed on the node. The wrong certificate is the one that is used 
			//          from the user to become authenticated in APG and should be loged in alog output
				tlsConnectionStatus = "Failed TLS connection accept";
                                isCertificateAuthenticationFailed = true;
        		}
			else if(bufStrOne.find("Access denied for user") != string::npos)
			{
			//019-07-05T13:30:18.442651+00:00 SC-2-2 com-tlsd: pam_sss(com-tlsd:account): Access denied for user testuser2: 6 (Permission denied)
				position = bufStrOne.find("com-tlsd:account"); //positioned as 63//correct
	                        if(position != string::npos)
                                {
					tlsConnectionStatus = bufStrOne.substr(position+19);
				}
			}
		}
	}


	if ((tlsConnectionStatus.find ("pam_acct_mgmt = 0(Success)") != string::npos) || (tlsConnectionStatus.find ("OK LOGIN") != string::npos))
	{
		messField.Type = "TLS Login Successful";
		message_desc="Accepted Login using TLS for user ";
	}
	else if (tlsConnectionStatus.find ("Failed TLS connection accept") != string::npos)
	{
		messField.Type = "TLS Login Failed";
                message_desc = "TLS Authentication failed";

	}
	else if (tlsConnectionStatus.find ("pam_acct_mgmt = 12") != string::npos)
	{
		//pam_acct_mgmt = 12(Authentication token is no longer valid;new one required)
		isAuthenticationTokenExpired = true;
		messField.Type = "TLS Login Failed";
		message_desc = "Authentication token is no longer valid; new one is required";
	}
	 else if (tlsConnectionStatus.find ("OK LOGOUT")!= string::npos)
        {
                 messField.Type = "TLS Logout Successful";
                message_desc="TLS Logout Successful for user ";

        }
	else if (tlsConnectionStatus.find ("COMMAND") == string::npos)
        {
                messField.Type = "TLS Login Failed";
                message_desc="TLS Login failed for user ";
	
	}



	messField.msg = "\nSource: " + messField.source + "\nCategory: " + messField.category + "\nType: " + messField.Type + "\nDescription: ";
	if(tls_cliss_netconf) 
		messField.msg += message_desc + tlsUserName + " - " + tlsConnectionStatus + "\n";
	else if(isFtpTlsConnection){
		isFtpTlsConnection=false;
		messField.msg += message_desc+"\n";
	}else if (isCertificateAuthenticationFailed){
		isCertificateAuthenticationFailed = false;
		messField.msg += message_desc+"\n";
	}	
	else if ( isAuthenticationTokenExpired) {
		isAuthenticationTokenExpired = false;
		messField.msg += message_desc+"\n";
	}	
	else
		messField.msg += message_desc + tlsUserName + " - " + tlsConnectionStatus + "\n";


	gethostname(hostname, sizeof(hostname));
	messField.local_host_name = hostname;

	if (messField.local_host_name.size() != 0)
	{
		found = messField.local_host_name.length();
		messField.node_number = messField.local_host_name.substr(found - 1,1);
	}
	messField.node_number = messField.node_number + ";";

	if (commandSession)  messField.cmdid = messField.cmdid + ";";

	if (MCpSystem)
	{
		messField.cpnamegroup = messField.cpnamegroup + ";";
		messField.cpidlist = messField.cpidlist + ";";
		messField.cmdseqnum = messField.cmdseqnum + ";";
	}
}




/*===================================================================
   ROUTINE: _REFORMLIST
=================================================================== */
string acs_alog_parser::_REFORMLIST(string eventEntry)
{

	size_t  pos = 0;

	while ((pos = eventEntry.find("+")) != string::npos)
	{
		eventEntry.at(pos) = ',';
	}

	return eventEntry;
}


void  acs_alog_parser::hidePwdInTheZipCommands ()
{

	size_t  pos1, pos2;

	if ((pos1 = messField.msg.find("zip ")) != string::npos)
	{
		if ((pos2 = messField.msg.find(" -P ", pos1)) == string::npos)
			return;

		int  j = pos2 + 3;

		while (messField.msg.at(j) == ' '  ||  messField.msg.at(j) == '\t')  j ++;

		while (messField.msg.at(j) != ' '  &&  messField.msg.at(j) != '\t')
		{
			messField.msg.at(j) = '*';  j++;
		}
	}
	else if((pos1 = messField.msg.find("7z ")) != string::npos)
	{
		if ((pos2 = messField.msg.find(" -p\"", pos1)) == string::npos)
			return;

		int  j = pos2 + 3;
		size_t pos3 = messField.msg.find('\"', j+1);

		while (j < pos3)
		{
			messField.msg.at(j) = '*';
			j++;
		}
	}
	else
		return;
}


void  acs_alog_parser::set_BufStrOne ( const std::string& event)
{
	bufStrOne.assign (event);
}


void  acs_alog_parser::set_BufStrTwo ( const std::string& event)
{
	bufStrTwo.assign (event);
}


string acs_alog_parser::get_PartOfMsg()
{

	if (messField.msg.length() < 500)  return messField.msg;

	return messField.msg.substr(0,499);
}


string acs_alog_parser::get_TypeRec()
{

	if (messField.TypeRec.length() > 0)  return messField.TypeRec.substr(0, messField.TypeRec.length()-1);

	return "";
}

void acs_alog_parser::clean_internal_string (){

	bufStrOne.clear();
	bufStrTwo.clear();
	targetRec.clear();
	_CLEAR();
}
