/*
 * alogfind.cpp
 *
 *  Created on: Mar01, 2013
 *      Author: xfraerr
 */

#include <ACS_CS_API.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_Util.H>
#include <acs_alog_cmdUtil.h>
#include <acs_alog_Lister.h>

//ENM
#include <ACS_CC_Types.h>
#include <acs_apgcc_omhandler.h>
#include <vector>
//ENM
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <cstring>
#include <cctype>
#include <ctime>
#include <list>
#include <acs_prc_api.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <grp.h> /* defines 'struct group', and getgrnam(). */
#include <cstddef>
#include <mntent.h>
#include <crypto_api.h>//START HX86564
#include <crypto_status.h>
extern "C"{
extern SecCryptoStatus sec_crypto_encrypt_ecimpassword_legacy(/*OUT*/ char** ciphertext, const char* plaintext);
extern SecCryptoStatus sec_crypto_decrypt_ecimpassword_legacy(/*OUT*/ char** plaintext, const char* ciphertext);
}//END //HX86564
using namespace std;


//====================================================================================================================================================
//Macro definition
//====================================================================================================================================================
#define FPRINT_INCOR_USE fprintf(stderr, str_error_incorrect_usage.c_str())
#define FPRINT_UNRES_VALUE(option) fprintf(stderr,"%s: %s\n", str_error_unreas_value.c_str(),option) //xanttro - 14/10/2013
#define ALOG_DATE_TIME_MAX_LEN 12
#define ALOG_DATE_MAX_LEN 8
//====================================================================================================================================================
//global variable declaration
//====================================================================================================================================================
string opt_device;
string opt_user;
string opt_start_time;
string opt_start_date;
string opt_stop_date;
string opt_stop_time;
string opt_local_host;
string opt_remote_host;
string opt_session_id;
string opt_command_id;
string opt_type;
string opt_string;
string opt_cpId;
string opt_cpnameGroup;
string opt_csn;
string transferQueueAttached  = "";
string nameTag1 = "";
string nameTag2 = "";
string renameTemplate = "";
string decryptString(const std::string &strPwdCrypt);
string str_error_incorrect_usage="Incorrect usage\n";
string str_error_unreas_value = "Unreasonable value";
string str_error_illegal_option="Illegal option ";
const string AUDITLOGGINGM_MO = "AxeAuditLoggingauditLoggingMId=1";
const string AUDITLOGGINGM_TRANSFERQUEUE_ATTR = "transferQueue";
const string FILETRANSFERQUEUEID = "fileTransferQueueId=";
const string FILETRANSFERMANAGERRDN	= "fileTransferManagerId=1,AxeDataTransferdataTransferMId=1";
const string NAMETAG1_ATTR = "nameTag1";
const string NAMETAG2_ATTR = "nameTag2";
const string TEMPLATE_ATTR = "template";

bool  IsMultiCP  = false;
bool  CmdIDused  = false;
bool  isTQDefined = false;

bool  CpRoleUser = false;			//  Only the SystemSecAdmin and CpRole users can use the alogfind with different permission
bool  SysSecAdminUser = false;		//  as reported in the table par. 3.1.6 of the User Guide "User Management"

char  FileAudit[300];
char  FilePAudit[300];
char  SupportFile[200];
char  SupportFile_Decrypt[200];//Support file for temporarily storing the decrypted content
string CurrentPlogFile;
std::string event="";

string  LastEvent;
std::map<std::string, time_t> logfileTimestamps;
std::vector< std::pair<string, time_t> > fileTimestamps;

int num_u = 0;
int num_d = 0;
int num_a = 0;
int num_b = 0;
int num_e = 0;
int num_g = 0;
int num_t = 0;
int num_s = 0;
int num_i = 0;
int num_r = 0;
int num_l = 0;
int num_z = 0;		//cpID
int num_w = 0;		//cpname
int num_q = 0;		//csn
int w_flag = 0;		//ProtectedLog

struct fieldRec
{
	string size_record;
	string data;
	string time;
	string type;
	string prca;
	string user_name;
	string device;
	string node_number;
	string session_id;
	string local_host_name;
	string remote_host;
	string size_msg;
	string msg;
	string cmdidlist;
	string cmdid;
	string cpnameGroup;
	string cpidList;
	string cmdSeqNum;
};


//====================================================================================================================================================
//Internal Function declaration
//====================================================================================================================================================

void Usage();

//====================================================================================================================================================
//	return int    0 if all OK, 1 if error
//====================================================================================================================================================
int ElaborateRecord(string lineInput, fieldRec &outputFields);

void OutputPrint(fieldRec &outputFields);

int  VerifyOutput (fieldRec &outputFields);

bool rMatch(const string& s, unsigned int i, const string& p, unsigned int j);

bool isNumeric(const char *s);

bool match(const string& source, const string& pattern);

int matchReg(const char *string, char *pattern);   //match with regular expression

int getNodeState();

void clearStruct(fieldRec &outputFields);

int get_datetime_from_fname (const char * fName, char fdate[]);

string eventTypeConversion(string eventType);

bool isLogfileRenamed(const string fName);

int get_datetime_from_renamedFile(const string fName, char fDate[]);

bool isRenamedLogfileValid(string fName);

int getTQandNametagInfo();

bool findNametagInRenamedFile(const string fName);

string getDateOfFileClosestToStartDate (const string start_date, string &nameOfFileToStartWith);

bool sortFileTimestamps (const pair<string, time_t> & it1, const pair<string, time_t> &it2);

time_t convertStringTimeToEpoch(const char* theTime, const char* format = "%Y%m%d");

string convertEpochTimeToString(const time_t* epochTime, const char* format = "%Y%m%d");

void  signal_handler (int sigNum)
{
		unlink (SupportFile);
		unlink (SupportFile_Decrypt);//removing temporary file  created incase of interruptions by user
		switch (sigNum)
		{
		case  SIGSEGV : // cout << endl << "SIGSEGV signal received !" << endl << endl;
						 break;
		case  SIGINT  : // cout << endl << "SIGINT signal received !" << endl << endl;
						 break;
		case  SIGTERM : // cout << endl << "SIGTERM signal received !" << endl << endl;
						 break;
		case  SIGQUIT : // cout << endl << "SIGQUIT signal received !" << endl << endl;
						 break;
		case  SIGHUP  : // cout << endl << "SIGHUP signal received !" << endl << endl;
						 break;
		}
		exit (1);
}


void Usage()
{
        cerr << "Usage:\r\n";
        cerr << "On Classic CP System\r\n";
        cerr << "Format 1: Searching in logged data\r\n";
        cerr << "alogfind [-d device] [-u user] [-a start_time -e start_date]\r\n";
        cerr << "         [-b stop_time -g stop_date] [-e start_date] [-g stop_date]\r\n";
        cerr << "         [-l local_host] [-r remote_host] [-i session_id]\r\n";
        cerr << "         [-c command_id] [-t type] [-s string]\r\n\r\n" << flush;
        cerr << "Format2: Searching in protected logged data\r\n";
        cerr << "alogfind -w [-d device] [-u user]\r\n";
        cerr << "         [-a start_time -e start_date]\r\n";
        cerr << "         [-b stop_time -g stop_date] [-e start_date] [-g stop_date]\r\n";
        cerr << "         [-l local_host] [-r remote_host] [-i session_id]\r\n";
        cerr << "         [-c command_id] [-t type] [-s string]\r\n\r\n" << flush;
        cerr << "On Multi-CP System\r\n";
        cerr << "Format 1: Searching in logged data\r\n";
        cerr << "alogfind [-d device] [-u user] [-a start_time -e start_date]\r\n";
        cerr << "         [-b stop_time -g stop_date] [-e start_date] [-g stop_date]\r\n";
        cerr << "         [-l local_host] [-r remote_host] [-i session_id]\r\n";
        cerr << "         [-c command_id] [-t type] [-n csn] [-s string]\r\n";
        cerr << "         [-p cpid | -cp name]\r\n\r\n" << flush;
        cerr << "Format2: Searching in protected logged data\r\n";
        cerr << "alogfind -w [-d device] [-u user]\r\n";
        cerr << "         [-a start_time -e start_date] [-b stop_time -g stop_date]\r\n";
        cerr << "         [-e start_date] [-g stop_date] [-l local_host]\r\n";
        cerr << "         [-r remote_host] [-i session_id] [-c command_id] [-t type]\r\n";
        cerr << "         [-n csn] [-s string] [-p cpid | -cp name]\r\n\r\n" << flush;
}


string  toUpper (string  buffer)
{
	int  len = buffer.length();

	for (int j=0; j<len; j++)  buffer[j] = std::toupper(buffer[j]);

	return buffer;
}


bool  matchCPid (string cpidList, string cpId)
{

		int  len = cpidList.length();

		for  (int j=0; j<len; j++)
		{
				string  xx = "";
				while (j < len  &&  cpidList.at(j) != ',')
				{
						xx += cpidList.at(j);  j ++;
				}
				if (cpId.compare(xx) == 0)  return true;
		}

		return false;
}


/*============================================================================
// ROUTINE: get_datetime_from_fname(const char * fName, char fdate[])
//  parameters:
//  fName -  is ALOG/PLOG filename having one of following format:
//  	Windows format: LOGFILE-YYYY-mm-dd-hhmm
//					 	PLOGFILE_yyyy_mm_dd_hhmm.zip
//  	Linux format:   audit.yyyymmdd-hhmmss
//                   	paudit.yyyymmdd-hhmmss.zip
//  fdate  -  datetime extract from file name having the following format
//            yyyymmddhhmm
 ============================================================================ */
int get_datetime_from_fname (const char * fName, char fdate[])
{
	std::string fName_str(fName);
	std::size_t found = fName_str.find_first_of("._-");
	if(found == std::string::npos || found == strlen(fName)-1){
		return -1;
	}

	std::string fdate_str, tempstr;
	tempstr=fName_str.substr(found+1);

	found = tempstr.find_first_of("_-.");
	while (found!=std::string::npos && found < tempstr.length()-1)
	{
		fdate_str.append(tempstr.c_str(),found);
		tempstr=tempstr.substr(found+1);
		found = tempstr.find_first_of("_-.");
	}
	if((tempstr.compare("zip")!=0) || (tempstr.compare("7z")!=0)){
		fdate_str.append(tempstr.c_str());
	}

	strncpy(fdate,fdate_str.c_str(),ALOG_DATE_TIME_MAX_LEN);

	return 0;
}


/*============================================================================ */
// ROUTINE: compare_filename (std::string& fName1,std::string& fName2)
//  parameters:
//  fName1, fname2 - ALOG/PLOG file name
//  return value:
//  true  - if fName1 < fname2 according to date in fName[1,2]
//  false - otherwise
/*============================================================================ */
bool compare_filename (std::string& fName1,std::string& fName2)
{
	char fdate1[ALOG_DATE_TIME_MAX_LEN+1]= {0}, fdate2[ALOG_DATE_TIME_MAX_LEN+1]= {0};
	bool renamedLogFile1 = false, renamedLogFile2 = false;								// use stat() comparison if one of the 2 file names is not in default audit log format

	renamedLogFile1 = isLogfileRenamed(fName1);
	renamedLogFile2 = isLogfileRenamed(fName2);


	struct stat  status1, status2;					//  When time stamps in filename are same or if any 1 file is renamed, the last modification times are compared
	string       pathAbs = "";

	if (w_flag == 0)  pathAbs.assign(FileAudit);
	else			  pathAbs.assign(FilePAudit);
	pathAbs = pathAbs + "/" + fName1;

	std::map<std::string, time_t>::iterator it1, it2;

	if((it1 = logfileTimestamps.find(fName1)) == logfileTimestamps.end())
	{
		if(stat (pathAbs.c_str(), &status1) == 0)
			logfileTimestamps[fName1] = status1.st_mtim.tv_sec;
		else
			logfileTimestamps[fName1] = 0;
		it1 = logfileTimestamps.find(fName1);
	}

	if (w_flag == 0)  pathAbs.assign(FileAudit);
	else			  pathAbs.assign(FilePAudit);
	pathAbs = pathAbs + "/" + fName2;


	if((it2 = logfileTimestamps.find(fName2)) == logfileTimestamps.end())
	{
		if(stat (pathAbs.c_str(), &status2) == 0)
			logfileTimestamps[fName2] = status2.st_mtim.tv_sec;
		else
			logfileTimestamps[fName2] = 0;
		it2 = logfileTimestamps.find(fName2);
	}

	if((renamedLogFile1 == false) && (renamedLogFile2 == false))					// If both files are not renamed, compare timestamps from file name
	{
		if(get_datetime_from_fname(fName1.c_str(), fdate1)<0 ||
				get_datetime_from_fname(fName2.c_str(), fdate2) <0 ){
			return true;
		}

		int ret;
		if((ret = strcmp(fdate1,fdate2))!= 0){
			return (ret < 0);
		}
	}

	bool statCheck = false;

	if((it1 != logfileTimestamps.end()) && (it2 != logfileTimestamps.end()))
		statCheck = (it1->second < it2->second);

	return statCheck;
}

time_t convertStringTimeToEpoch(const char* theTime, const char* format)
{
	std::tm tmTime;
	memset(&tmTime, 0, sizeof(tmTime));
	strptime(theTime, format, &tmTime);
	return mktime(&tmTime);
}

string convertEpochTimeToString(const time_t* epochTime, const char* format)
{
	char dateTmp[80] = {0};
	string dateStr = "";
	std::tm* tmTime = localtime(epochTime);
	strftime(dateTmp, sizeof(dateTmp), format, tmTime);
	dateStr.assign(dateTmp);
	return dateStr;
}

string getDateOfFileClosestToStartDate(const string start_date, string &nameOfFileToStartWith)
{

	string fileToStart = "";
	time_t startDateEpoch = convertStringTimeToEpoch(start_date.c_str());
	std::map<std::string, time_t>::iterator it;

	for(it = logfileTimestamps.begin(); it != logfileTimestamps.end(); it++)
		fileTimestamps.push_back(make_pair(it->first,it->second));

	//logfileTimestamps.empty();
	std::sort(fileTimestamps.begin(),fileTimestamps.end(),sortFileTimestamps);

	int indexofStartFile = 0;
	for(indexofStartFile = 0; indexofStartFile < fileTimestamps.size(); indexofStartFile++)
	{
		if(fileTimestamps[indexofStartFile].second < startDateEpoch)
			fileToStart = fileTimestamps[indexofStartFile].first;
		else
		{
			if(fileToStart.length() == 0)
				fileToStart = fileTimestamps[indexofStartFile].first;
			break;
		}
	}
	nameOfFileToStartWith = fileToStart;
	//string fileDate = "", dateToStart = "00000000";

	char fdate[ALOG_DATE_TIME_MAX_LEN+1] = {0};
	string timeOfFileToStart = "";

	if(isLogfileRenamed(nameOfFileToStartWith) == false)
		get_datetime_from_fname(nameOfFileToStartWith.c_str(), fdate);
	else
		get_datetime_from_renamedFile(nameOfFileToStartWith, fdate);

	timeOfFileToStart.assign(fdate,ALOG_DATE_MAX_LEN);

	return timeOfFileToStart;
}

bool sortFileTimestamps (const pair<string, time_t> & it1, const pair<string, time_t> &it2)
{
	return it1.second < it2.second;
}

int  ElaborateRecord (string lineInput, fieldRec &outputFields)
{

	size_t  posFin = 0, posStop = 0;
	int     j = 0,  counter = 0;
	bool 	singleLine = false;

	while (j < (int)lineInput.length())						// ------  Checks due to record corruption (Hard Limit)
	{														// ------         and to different format (Win vs Linux)
			if (lineInput.at(j) == ';')  counter ++;		// ------
			j ++;
	}
	if (counter < 13)  return 1;

	int  pos = lineInput.length() - 1;
	while (lineInput.at(pos) == '\n' || lineInput.at(pos) == '\r' || lineInput.at(pos) == ' ')  pos --;

	lineInput = lineInput.substr(0,pos+1); // TR-HT78529

	if (lineInput.at(pos) != ';')  return 1;

	j = 0,  counter = 0;

	while (j < pos)
	{
			if (lineInput.at(j) == '\n')  counter ++;
			j ++;
	}														// ------
															// ------
	if (counter == 0)  singleLine = true;					// -----------------------------------------

	posFin = lineInput.find(";");
	if (posFin == string::npos)  return 1;

	outputFields.size_record = lineInput.substr(0,posFin);
	posStop = posFin;
	if (posFin >= lineInput.length())  return 1;

	posFin = lineInput.find(";",posStop+1);
	if (posFin == string::npos)  return 1;

	outputFields.data = lineInput.substr(posStop+1,posFin-posStop-1);

	if (outputFields.data.length() > 0)
	{
			counter = 0;
			for (j=0; j<(int)outputFields.data.length(); j++)
			{
					if (outputFields.data.at(j) == '-')  counter ++;
			}
			if (counter != 2)  return 1;

			outputFields.data.erase (outputFields.data.find('-'),1);
			outputFields.data.erase (outputFields.data.find('-'),1);
	}
	posStop = posFin;
	posFin = lineInput.find(";",posStop+1);
	outputFields.time = lineInput.substr(posStop+1, posFin-posStop-1);
	posStop = posFin;

	if (acs_alog_cmdUtil::isNumeric(outputFields.time.c_str()) == false)  return 1;

	posFin = lineInput.find(";",posStop+1);
	outputFields.type = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;
	posFin = lineInput.find(";",posStop+1);
	outputFields.prca = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;
	posFin = lineInput.find(";",posStop+1);
	outputFields.user_name = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.device = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.node_number = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.session_id = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.local_host_name = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.remote_host = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.size_msg = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	if (acs_alog_cmdUtil::isNumeric(outputFields.size_msg.c_str()) == false)  return 1;

	int  lenData = atoi (outputFields.size_msg.c_str());

	if (singleLine == true)
	{
			outputFields.msg = lineInput.substr(posStop+1, lenData);
			posFin ++;
	}
	else {  //TR-HT78529
			int posFin1 = lineInput.length() - 1;
			while (lineInput.at(posFin1) != ';')   posFin1 --;
			while (lineInput.at(posFin1) != '\n')  posFin1 --;

			outputFields.msg = lineInput.substr(posStop+1, posFin1-posStop);
	}

	CmdIDused = false;

	size_t  lenEve = lineInput.length();
	counter = 0;

	for (j=(posFin+outputFields.msg.length()); j<(int)lenEve; j++)   //TR-HT78529
    {
		if (lineInput.at(j) == ';')  counter ++;
	}

	if (counter == 1)  return 0;		// the event is on a Single CP, without the Cmd ID
//TR HV27706: this section commented
//	posFin ++;
//	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  posFin++;
//	if (posFin == lenEve)  return 1;
//	std::cout <<"TODEL 11 posFin="<< posFin << std::endl;
//	posFin ++;
//	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  posFin++;
//	if (posFin == lenEve)  return 1;
//	std::cout <<"TODEL 12 posFin="<< posFin << std::endl;
//	posFin ++;

/*TR HV27706: Start*/
	posFin= posFin+outputFields.msg.length()+2;//to reach the next character following the ; after msg field
/*TR HV27706: End  */
	//while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cmdid += lineInput.at(posFin++);//TR HV19568: modified
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cmdidlist += lineInput.at(posFin++);//TR HV19568: cmidlength
	if (posFin == lenEve)  return 1;


	CmdIDused = true;
	counter   = 0;

	for (j=posFin; j<(int)lenEve; j++)
	{
			if (lineInput.at(j) == ';')  counter ++;
	}
	//TR HV27706 - Start
	if (counter == 3)  return 0;
	//TR HV27706 - End
	//TR HV27706 moved
	//if (counter < 3)  return 0;        //  the event is on a Single CP

	//TR HV19568:START here management of cmid has to be performed todo here posFin is on ; after cmidlength
	//convert to integer

	if (acs_alog_cmdUtil::isNumeric(outputFields.cmdidlist.c_str()) == false)  return 1;

	int  cmidLenRead = atoi (outputFields.cmdidlist.c_str());

	posFin++;
	outputFields.cmdid = lineInput.substr(posFin,cmidLenRead);//get the substring from posFin for a length cmidLenRead
	posFin+=cmidLenRead;//now posfin is on the ; after the cmdid

	if (counter < 3)  return 0;//moved here
	//TR HV19568:END

	if (outputFields.cmdid.length() == 0)  CmdIDused = false;

	if (outputFields.type.compare("sec") == 0)  return 0;

	posFin ++;
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cpnameGroup += lineInput.at(posFin++);
	if (posFin == lenEve)  return 1;
	posFin ++;
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cpidList += lineInput.at(posFin++);
	if (posFin == lenEve)  return 1;
	posFin ++;
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cmdSeqNum += lineInput.at(posFin++);
	if (posFin == lenEve)  return 1;
	return 0;
}


bool  valueTheOptionType (string recType, string optType)
{

		if (recType.compare(optType) == 0)  return true;

		if ((recType.compare("com") == 0  ||  recType.compare("cmw") == 0)  &&  optType.compare("nc") == 0)  return true;

		if (optType.compare("mc") == 0)
		{
				if (recType.compare("mci") == 0)  return true;
				if (recType.compare("mcl") == 0)  return true;
				if (recType.compare("mli") == 0)  return true;
		}

		if (optType.compare("mp") == 0)
		{
				if (recType.compare("mpi") == 0)  return true;
		}
		/*if (optType.compare("mmc") == 0)
		{
			 return true;
		}*/

		if (optType.compare("mcl") == 0)
		{
				if (recType.compare("mli") == 0)  return true;
		}

		if (optType.compare("mi") == 0)
		{
				if (recType.compare("mci") == 0)  return true;
				if (recType.compare("mpi") == 0)  return true;
				if (recType.compare("mli") == 0)  return true;
		}

		return false;
}


int  VerifyOutput (fieldRec &outputFields)
{
	bool print = false;
	bool exit  = false;


	if ( opt_remote_host.length() == 0  &&  opt_local_host.length() == 0  &&
		 opt_device.length() == 0       &&  opt_user.length() == 0		  &&
		 opt_type.length() == 0         &&  opt_string.length() == 0      &&
		 opt_start_date.length() == 0   &&  opt_stop_date.length() == 0   &&
		 opt_start_time.length() == 0   &&  opt_stop_time.length() == 0   &&
		 opt_session_id.length() == 0   &&  opt_command_id.length() == 0  &&
		 opt_cpnameGroup.length() == 0  &&  opt_csn.length() == 0         &&
		 opt_cpId.length() == 0 )												print = true;

	if (outputFields.data.length() == 0  &&
		(opt_start_date.length() == 8   ||  opt_stop_date.length() == 8))  return 1;

	if (opt_user.length() > 0)
	{
		if (strcasecmp (outputFields.user_name.c_str(), opt_user.c_str()) == 0)
		{
			print = true;
		}
		else {
				print = false;
				exit = true;
		}
	}

	if (opt_device.length() > 0)
	{
		if (match(outputFields.device,opt_device))
		{
			if (!exit)  print = true;
		}
		else {
				print = false;
				exit = true;
		}
	}

	if (opt_local_host.length() > 0)
	{
		if(match(outputFields.local_host_name,opt_local_host))
		{
			if (!exit)  print = true;
		}
		else {
				print = false;
				exit = true;
		}
	}

	if (opt_remote_host.length() > 0)
	{
		if(match(outputFields.remote_host,opt_remote_host))
		{
			if (!exit)  print = true;
		}
		else {
				print = false;
				exit = true;
		}
	}

	if (opt_type.length() > 0)
	{
		if ( w_flag )
		{
			if ( ! opt_type.compare("pnc") )     //  pnc is not a valid value for alogpchg
			{
				if ( (outputFields.type.compare("com") == 0) && (outputFields.msg.find("mml") != string::npos) )
				{
					if (!exit)  print = true;
				}
				else {
						print = false;
						exit  = true;
				}
			}
			else {
				if(valueTheOptionType (outputFields.type, opt_type))
				{
					if (!exit)  print = true;
				}
				else {
						print = false;
						exit  = true;
				}
			}
		}
		else {
			bool  resCheck = valueTheOptionType (outputFields.type, opt_type);

			if (resCheck == true)
			{
				if (! exit)  print = true;
			}
			else {
					print = false;
					exit  = true;
			}
		}
	}

	if (opt_string.length() > 0)
	{
		string  input = toUpper (opt_string);
		string  data  = toUpper (outputFields.msg);

		if (match (data, input))
		{
			if (! exit)  print = true;
		}
		else {
				print = false;
				exit = true;
		}
	}

	if (outputFields.data.length() > 0)
	{
		if (opt_start_date.length() > 1)
		{
			if ( opt_start_date <= outputFields.data)
			{
				if (!exit)  print = true;
			}
			else {
					print = false;
					exit = true;
			}
		}

		if (opt_stop_date.length() > 1)
		{
			if (opt_stop_date >= outputFields.data)
			{
				if (!exit)  print = true;
			}
			else {
					print = false;
					exit = true;
			}
		}
	}
	else {
			if (! exit)  print = true;
	}

	if (opt_stop_time.length() > 0)
	{
		if (opt_stop_date == outputFields.data)
		{
			if (opt_stop_time >= outputFields.time)
			{
				if (! exit)  print = true;
			}
			else {
					print = false;
					exit = true;
			}
		}
	}


	if (opt_start_time.length() > 0)
	{
		if (opt_start_date == outputFields.data)
		{
			if (opt_start_time <= outputFields.time)
			{
				if (! exit)  print = true;
			}
			else {
					print = false;
					exit = true;
			}
		}
	}


	if (opt_session_id.length() > 0)
	{
		if (outputFields.session_id.compare(opt_session_id) == 0)
		{
			if (!exit)  print = true;
		}
		else {
				print = false;
				exit = true;
		}
	}


	if (opt_command_id.length() > 0)
	{
		if (outputFields.cmdid.compare(opt_command_id) == 0)
		{
			if (!exit)  print = true;
		}
		else {
				print = false;
				exit = true;
		}
	}


	if (IsMultiCP)
	{
		if (opt_cpnameGroup.length() > 0)
		{
			if (match(outputFields.cpnameGroup,opt_cpnameGroup))
			{
				if (! exit)  print = true;
			}
			else {
					print = false;
					exit = true;
			}
		}

		if (opt_csn.length() > 0)
		{
			if (match(outputFields.cmdSeqNum,opt_csn))
			{
				if (! exit)  print = true;
			}
			else {
					print = false;
					exit = true;
			}
		}

		if (opt_cpId.length() > 0)
		{
			if (matchCPid(outputFields.cpidList,opt_cpId))
			{
					if (!exit)  print = true;
			}
			else {
					print = false;
					exit = true;
			}
		}
	}


	if (outputFields.size_msg.compare("0") == 0  &&
		(outputFields.type.compare("mp") == 0  ||  outputFields.type.compare("np") == 0))  print = false;   // Printouts :  HP27422 trouble

	if (print)  return 0;
	return 1;
}


void  OutputPrint (fieldRec &outputFields)
{

	cout << "Event type:          " << eventTypeConversion(outputFields.type) << endl; 		//  NT/AP command
	cout << "Date/time:           "; 														//  2003-08-15 152912
	if (outputFields.data.length() == 8)
	{
		cout << outputFields.data.substr(0,4) << "-" << outputFields.data.substr(4,2) << "-" << outputFields.data.substr(6,2);
	}
	cout << " " << outputFields.time << endl;
	cout << "User:                " << outputFields.user_name <<endl;				//  EEMEA\eabuser
	cout << "Session ID:          " << outputFields.session_id <<endl;				//  23
	if (CmdIDused == true)
			cout << "Command ID:          " << outputFields.cmdid << endl;
	if (IsMultiCP)
			cout << "Command Seq Num:     " << outputFields.cmdSeqNum << endl;
	cout << "AP node number:      " << outputFields.node_number << endl;			//  1
	cout << "Local host:          " << outputFields.local_host_name << endl;		//  UPC8095/osd179
	cout << "Remote host:         " << outputFields.remote_host << endl;			//  osd.ericsson.com
	if (IsMultiCP)
	{
			cout << "CP name/group:       " << outputFields.cpnameGroup << endl;
			cout << "CP ID:               " << outputFields.cpidList << endl;
	}
	cout << "Device:              " << outputFields.device << endl;
	cout << "PRCA:                " << outputFields.prca << endl;
	cout << "Data:                ";

	cout << outputFields.msg << endl << endl;

	LastEvent.assign(outputFields.type);
	if (LastEvent.compare("alp") == 0)  cout << endl;
	if (LastEvent.compare("mc")  == 0)  cout << endl << endl;
	if (LastEvent.compare("sec") == 0)  cout << endl;
	if (LastEvent.compare("mmc") == 0)  cout << endl;
	if (LastEvent.compare("mp")  == 0)
	{
			cout << endl;
			if (outputFields.msg.at(outputFields.msg.length()-1) != '\n')  cout << endl;
	}
}


string  eventTypeConversion (string eventType)
{

	if (eventType.compare("mc")  == 0)  return "MML Command";
	if (eventType.compare("mci") == 0)  return "MML Command inconsistency";
	if (eventType.compare("mcl") == 0)  return "MML Command log";
	if (eventType.compare("mli") == 0)	return "MML Command log inconsistency";
	if (eventType.compare("mp") == 0)   return "MML Printout";
	if (eventType.compare("mpi") == 0)  return "MML Printout inconsistency";
	if (eventType.compare("nc") == 0)  	return "AP/TS Session Command";
	if (eventType.compare("np") == 0)  	return "AP/TS Session Printout";
	if (eventType.compare("sec") == 0)	return "Security Audit Event";
	if (eventType.compare("com") == 0)  return "AP/TS Session Command";
	if (eventType.compare("cmw") == 0)  return "Core MW";
	if (eventType.compare("alp") == 0)  return "Alarm Printout";
	if (eventType.compare("apd") == 0)  return "Application Data";
	if (eventType.compare("pnc") == 0)  return "SHELL MML Command";
	if (eventType.compare("mmc") == 0)  return "MML Mediation Command";

	return eventType;
}


bool match(const string& source, const string& pattern)
{
	int ret;

	ret = matchReg(source.c_str(),(char *)pattern.c_str());

	if (ret)  return true;
	else      return false;
}


int matchReg(const char *string, char *pattern)
{
	int status;
	regex_t re;

	if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0)  return 0;

	status = regexec(&re, string, (size_t)0, NULL, 0);
	regfree(&re);

	if(status != 0)  return 0;
	return 1;
}


int getNodeState()
{
	int nodeState = 0;
	ACS_PRC_API prc;

	try {
		nodeState = prc.askForNodeState();
	}
	catch (int e) {
		return -1;
	}
	return nodeState;
}


void clearStruct(fieldRec &outputFields)
{
	outputFields.data.clear();
	outputFields.device.clear();
	outputFields.local_host_name.clear();
	outputFields.msg.clear();
	outputFields.node_number.clear();
	outputFields.cmdid.clear();
	outputFields.remote_host.clear();
	outputFields.session_id.clear();
	outputFields.size_msg.clear();
	outputFields.size_record.clear();
	outputFields.time.clear();
	outputFields.type.clear();
	outputFields.user_name.clear();
	outputFields.cmdidlist.clear();
	outputFields.cpidList.clear();
	outputFields.cpnameGroup.clear();
	outputFields.cmdSeqNum.clear();
	outputFields.prca.clear();
}


bool  checkNewEvent (string line)
{

		int  counter = 0;
		int  countChar = 0;
		int  j = 0;

		while (counter < 3  &&  j < (int)line.length())
		{
				if (line.at(j) == ';')
				{
						counter ++;  j ++;
				}
				else {
						if (line.at(j) >= '0'  &&  line.at(j) <= '9')
						{
								countChar ++;  j ++;
						}
						else  if (line.at(j) == '-')
							  {
									  countChar ++;  j ++;
							  }
							  else  return false;
				}
		}

		if (counter == 3  &&  countChar >= 12)  return true;      //  New Event found
		return false;
}


void  printRecords (string fileName)
{
	string    line, event;
	string    recordOutput;
	fieldRec  fields;
	int       ret;
	size_t 	  pos;

	if (w_flag == 0)
	{
		ifstream   file (fileName.c_str());

		if (! file.is_open())  return;

		while (file.good())
		{
			getline (file, line);
			if (line.length() > 0)
			{
					pos = line.find(";");
					if (pos == string::npos)  continue;

					fields.size_record = line.substr(0, pos);
					if (acs_alog_cmdUtil::isNumeric (fields.size_record.c_str()) == false)  continue;

					event = line;  fields.size_record = "";

					while (file.eof() == false)
					{
							long  pos = file.tellg();
							getline (file, line);

							if (checkNewEvent (line) == true)
							{
									file.seekg (pos);
									break;
							}
							event += "\n" + line;
					}

					ret = ElaborateRecord (event, fields);

					if (ret == 0)
					{
							if (VerifyOutput (fields) == 0)  OutputPrint (fields);
					}
					clearStruct (fields);
			}
		}
		file.close();
	}
	else {
			char  buffer[30];
			int   a = getpid();
			//string SupportFile;

			sprintf (buffer, "%d", a);
			strcpy  (SupportFile, "/tmp/Temp-");
			strcat  (SupportFile, buffer);

			string pwdFor7zExtractionInteractive = acs_alog_cmdUtil::getPasswordInFile() + "\n";
			int childProcStatus = 0;
			string cmdString = "/usr/bin/7z e -o\"/tmp\" -y " + fileName + " -so" + " 1> " + SupportFile + " 2>/dev/null";
			acs_alog_cmdUtil::execute7zCmd(cmdString, pwdFor7zExtractionInteractive, childProcStatus);		// TR HX49170
			pwdFor7zExtractionInteractive.clear();
			chmod (SupportFile, S_IRUSR | S_IWUSR | S_IRGRP);
			ifstream   file (SupportFile);
			if (!file.is_open())
				return;

			if((file.peek() == std::ifstream::traits_type::eof()))
			{
				file.close();
				unlink (SupportFile);
				return;
			}

			while (file.good())
			{
				getline (file, line);
				if (line.length() > 0)
				{
						pos = line.find(";");
						if (pos == string::npos)  continue;

						fields.size_record = line.substr(0, pos);
						if (acs_alog_cmdUtil::isNumeric (fields.size_record.c_str()) == false)  continue;

	 	 	 	 	 	event = line;  fields.size_record = "";

						while (file.eof() == false)
						{
								long  pos = file.tellg();
								getline (file, line);

								if (checkNewEvent (line) == true)
								{
										file.seekg (pos);
										break;
								}
								event += "\n" + line;
						}

						ret = ElaborateRecord (event, fields);

						if (ret == 0)
						{
								if (VerifyOutput (fields) == 0)  OutputPrint (fields);
						}
						clearStruct (fields);
				}
			}

			file.close();
			unlink (SupportFile);
	}
}

void  printRecords (string fileName,string d_content)//HX86564
{
    string    line, event;
    string    recordOutput;
    fieldRec  fields;
    int       ret;
    size_t    pos;
    char  buffer[30];
    int   a = getpid();

    sprintf (buffer, "%d", a);
    strcpy  (SupportFile_Decrypt, "/tmp/Decrypt_Temp-");
    strcat  (SupportFile_Decrypt, buffer);

    fstream targetFile;
    targetFile.open (SupportFile_Decrypt, fstream::in | fstream::app | fstream::out);


    if (! targetFile.is_open())
    {
        cout<<"file cant get opened!"<<endl;
        return;
    }

    targetFile << d_content;

    targetFile.seekg(0,ios::beg);

    while (targetFile.good())
    {

        getline (targetFile, line);
        if (line.length() > 0)
        {

            pos = line.find(";");
            if (pos == string::npos)
            {

                continue;
            }

            fields.size_record = line.substr(0, pos);
            if (acs_alog_cmdUtil::isNumeric (fields.size_record.c_str()) == false)
            {
                cout<<"record-size not numeric!"<<endl;
                continue;
            }

            event = line;  fields.size_record = "";

            while (targetFile.eof() == false)
            {
                long  pos = targetFile.tellg();
                getline (targetFile, line);

                if (checkNewEvent (line) == true)
                {

                    targetFile.seekg (pos);
                    break;
                }
                event += "\n" + line;
            }

            ret = ElaborateRecord (event, fields);

            if (ret == 0)
            {

                if (VerifyOutput (fields) == 0)
                {

                    OutputPrint (fields);

                }
            }
            clearStruct (fields);
        }
    }

    targetFile.close();
    unlink(SupportFile_Decrypt);

}

bool  getUserRoles (const string command, string &output)
{
	FILE  *fp;
	char   readLine[1000];

	fp = popen (command.c_str(), "r");

	if (fp == 0)  return false;

	while (fgets (readLine, sizeof(readLine) - 1, fp) != 0)
	{
		size_t  bufLen = strlen (readLine);

		if (readLine[bufLen-1] == '\r'  ||  readLine[bufLen-1] == '\n')  readLine[bufLen-1] = '\0';
		if (readLine[bufLen-2] == '\r'  ||  readLine[bufLen-2] == '\n')  readLine[bufLen-2] = '\0';

		output += readLine;
	}
	pclose (fp);

	return true;
}

/*============================================================================
	ROUTINE: isMemeberOfComEmergency
 ============================================================================ */

bool isMemberOfComEmergency (const std::string user)
{
   const std::string COM_EM_GRP = "com-emergency";
   bool isMember = false;
   /* get the information about the "strange" group. */
   struct group* group_info = getgrnam(COM_EM_GRP.c_str());
   /* make sure this group actually exists. */
   if (!group_info) {
   }
   else {
	   char** p_member;
	   string str_member;
	   for (p_member = group_info->gr_mem; *p_member; p_member++)
	   {
		   str_member = *p_member;
		   if (strcmp(user.c_str(), str_member.c_str()) == 0)
		   {
			   isMember = true;
			   break;
		   }
	   }
   }

   return isMember;
}

/*============================================================================
	ROUTINE: isMemeberOfTSGroup
 ============================================================================ */

bool isMemeberOfTSGroup (const std::string user)
{
   const std::string TSGROUP_GRP = "tsgroup";
   bool isMember = false;
   /* get the information about the "strange" group. */
   struct group* group_info = getgrnam(TSGROUP_GRP.c_str());
   /* make sure this group actually exists. */
   if (!group_info) {
   }
   else {
	   char** p_member;
	   string str_member;
	   for (p_member = group_info->gr_mem; *p_member; p_member++)
	   {
		   str_member = *p_member;
		   if (strcmp(user.c_str(), str_member.c_str()) == 0)
		   {
			   isMember = true;
			   break;
		   }
	   }
   }

   return isMember;
}

/*============================================================================
	ROUTINE: isMemeberOfRootGroup
 ============================================================================ */

bool isMemeberOfRootGroup (const std::string user)
{
	if (user == "root")  return true;

	const std::string TSGROUP_GRP = "root";
	bool isMember = false;
   /* get the information about the "strange" group. */
	struct group* group_info = getgrnam(TSGROUP_GRP.c_str());
   /* make sure this group actually exists. */
	if (!group_info) {
	}
	else {
	   char** p_member;
	   string str_member;
	   for (p_member = group_info->gr_mem; *p_member; p_member++)
	   {
		   str_member = *p_member;
		   if (strcmp(user.c_str(), str_member.c_str()) == 0){
			   isMember = true;
			   break;
		   }
	   }
	}
	return isMember;
}

bool isCompareRole(std::string output, std::string roleclass) {
	bool result=false;
	OmHandler immHandler;
	ACS_CC_ReturnType immInitRetCode = immHandler.Init();
	if ( immInitRetCode != ACS_CC_SUCCESS) {
		cout<<"ERROR: immHandler.Init() FAILURE!!! - error = " << immInitRetCode << endl; 
    return false;
  }
	std::vector<std::string> dnListRole,listRole;
	immHandler.getClassInstances(roleclass.c_str(), dnListRole);
	//keeping string output(containg roles) in to vector res
	char delim =',';
	std::size_t current, previous = 0;
	std::vector<std::string> res;
	current = output.find(delim);
	while (current != std::string::npos) {
		res.push_back(output.substr(previous, current - previous));
		previous = current + 1;
		current = output.find(delim, previous);
	}
	res.push_back(output.substr(previous, current - previous));
	// getting custom  and sytem roles using apgcc methods based on roleclass
	size_t equPos,commaPos;
	string roleId;
	for (size_t i=0; i< dnListRole.size(); i++)
	{
		equPos = dnListRole[i].find_first_of("=");
		commaPos = dnListRole[i].find_first_of(",");
		roleId = dnListRole[i].substr((equPos+1),(commaPos-equPos-1));
		listRole.push_back(roleId);
	}
	//Comparing vector res(containg roles of user) with custom and system roles
	for(size_t i=0; i<res.size(); i++){
		string temp1 = res[i];
		for(size_t j=0;j<listRole.size();j++)
		{
			std::string cRole =listRole[j];
			if(temp1.compare(cRole.c_str())== 0)
			{
				
				result=true;
				break;
			}
		}
	}
	ACS_CC_ReturnType immfinalizeRetCode = immHandler.Finalize(); 
 	if (immfinalizeRetCode != ACS_CC_SUCCESS) {
				cout<<"ERROR: immHandler.Finalize() FAILURE!!! - error = " << immfinalizeRetCode << endl; 
  }	
	return result;
}

int main (int argc,char *argv[])
{
	opterr = 0;
	int optIndex;
	int nodeStateVal = 0;

	signal (SIGSEGV, signal_handler);
	signal (SIGINT, signal_handler);
	signal (SIGTERM, signal_handler);
	signal (SIGQUIT, signal_handler);
	signal (SIGHUP, signal_handler);

	//get node state
	nodeStateVal = getNodeState();
	if (nodeStateVal == 2)
	{
		fprintf(stderr,"This is the passive node, you must execute the command on the active node\n");
		return 3;
	}
	else if (nodeStateVal != 1  &&  nodeStateVal != 2)
	{
		fprintf(stderr,"Undefined node state\n");
		return -1;
	}

	if (argc < 2)
	{
		fprintf(stderr,"User parameter not present\n");
		return -1;
	}

	char  rootPath[15];
	int   lenPath = 300;

	strcpy (rootPath, "auditLog");
	ACS_APGCC_CommonLib  apgccLib;

	if (apgccLib.GetFileMPath (rootPath, FileAudit, lenPath) != ACS_APGCC_DNFPATH_SUCCESS)
	{
		fprintf(stderr, "ALOGFIND, finding the audit logs path failed !\n");
		return -1;
	}

	strcpy(FilePAudit,FileAudit);

	strcat (FileAudit, "/");
	strcat (FilePAudit , "/protected_logs/");

	string  output = "";

	if (! isMemberOfComEmergency((string)argv[1])  &&  ! isMemeberOfTSGroup((string)argv[1])  &&
			! isMemeberOfRootGroup((string)argv[1]))
	{

		if(apgccLib.GetRoles ((string)argv[1], output) == ACS_APGCC_ROLES_SUCCESS)
		{
			if (output.length() > 0)
			{
				string tmpRoles = output;
				for (int j=0; j<(int) tmpRoles.length(); j++)  tmpRoles[j] = toupper(tmpRoles[j]);
				if (tmpRoles.find("CPROLE") != string::npos)    //  the user has a CpRole<i> authorization
				{
					CpRoleUser = true;
				}
				if (tmpRoles.find("SYSTEMSECURITYADMINISTRATOR") != string::npos)   //  the user has the SystemSecurityAdministrator authorization
				{
					SysSecAdminUser = true;
				}
				if (!CpRoleUser)
				{
					if (isCompareRole(output,"AxeMmlAuthorizationMmlRole")) CpRoleUser = true;
					if(!CpRoleUser && isCompareRole(output,"AxeMmlAuthorizationMmlSystemRole")) CpRoleUser = true;
				}
			}
		}
		else {
			cout << endl << "Unable to connect to server" << endl;
			return 117;
		}
	}

	// invoke the ACS_CS_API to know if we're executing on a SingleCP System or on a MultipleCP System
	ACS_CS_API_NetworkElement::isMultipleCPSystem(IsMultiCP);

	if (argc != 2)      //  without parameter
	{
		for (optIndex=2;optIndex<argc;optIndex++)
		{
			//check for correct input parameter. Two char.
			if (((strlen(argv[optIndex]) > 2) || (strlen(argv[optIndex]) <2 )) && (strcmp(argv[optIndex],"-cp")!=0 ))
			{
				cout << str_error_illegal_option << argv[optIndex] << endl << flush;
				Usage();
				return 2;
			}

			if (argv[optIndex][0] != '-')
			{
				FPRINT_INCOR_USE;
				Usage();
				return 2;
			}

			if (strcmp(argv[optIndex],"-d") == 0)   		// device
			{
				if (argv[optIndex+1])
				{
					opt_device = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					optIndex++;
					num_d++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-w") == 0)   		// device
			{
				w_flag++;
			}
			else if (strcmp(argv[optIndex],"-cp") == 0)   			// CP name
			{
				if(IsMultiCP)
				{
					if (argv[optIndex+1])
					{
						opt_cpnameGroup = argv[optIndex+1];
						if (argv[optIndex+1][0]=='-')
						{
							FPRINT_INCOR_USE;
							Usage();
							return 2;
						}
						optIndex++;
						num_w++;
					}
					else {
						FPRINT_UNRES_VALUE("");
						Usage();
						return 2;
					}

					if (opt_cpnameGroup.length() > 7)
					{
						cout << str_error_unreas_value
								<< opt_cpnameGroup.c_str()
								<< "\r\n\r\n" << flush;
						return 28;
					}
				}
				else {
					cerr << "Illegal option in this system configuration -cp " << "\r\n\r\n" << flush;
					return 116;
				}
			}
			else if (strcmp(argv[optIndex],"-p") == 0)   		// CPID
			{
				if(IsMultiCP)
				{
					if (argv[optIndex+1])
					{
						opt_cpId = argv[optIndex+1];
						if (argv[optIndex+1][0]=='-')
						{
							FPRINT_INCOR_USE;
							Usage();
							return 2;
						}
						optIndex++;
						num_z++;
					}
					else {
						FPRINT_UNRES_VALUE("");
						Usage();
						return 2;
					}
				}
				else {
					cerr << "Illegal option in this system configuration -p " << "\r\n\r\n" << flush;
					return 116;
				}
			}
			else if (strcmp(argv[optIndex],"-n") == 0)   		// csn
			{
				if(IsMultiCP)
				{
					if (argv[optIndex+1])
					{
						opt_csn = argv[optIndex+1];
						if (argv[optIndex+1][0]=='-')
						{
							FPRINT_INCOR_USE;
							Usage();
							return 2;
						}
						optIndex++;
						num_q++;
					}
					else {
						FPRINT_UNRES_VALUE("");
						Usage();
						return 2;
					}
				}
				else {
					cerr << "Illegal option in this system configuration -n " << "\r\n\r\n" << flush;
					return 116;
				}
			}
			else if (strcmp(argv[optIndex],"-u") == 0)		// user
			{
				if (argv[optIndex+1])
				{
					opt_user = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					optIndex++;
					num_u++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-a") == 0) 		// start_time
			{
				if (argv[optIndex+1])
				{
					opt_start_time = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					if ((opt_start_time.length() != 4) || (!acs_alog_cmdUtil::isNumeric(opt_start_time.c_str())) )
					{
						FPRINT_UNRES_VALUE(opt_start_time.c_str());
						Usage();
						return 2;
					}
					else
					{
						if ((strcmp(opt_start_time.substr(0,2).c_str(),"00") < 0 || strcmp(opt_start_time.substr(0,2).c_str(),"23") > 0) ||
								(strcmp(opt_start_time.substr(2,2).c_str(),"00") < 0 || strcmp(opt_start_time.substr(2,2).c_str(),"59") > 0))
						{
							FPRINT_UNRES_VALUE(opt_start_time.c_str());
							Usage();
							return 2;
						}
					}
					optIndex++;
					num_a++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-e") == 0)		// start_date
			{
				if (argv[optIndex+1])
				{
					opt_start_date = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					if ((opt_start_date.length() != 8) || (!acs_alog_cmdUtil::isNumeric(opt_start_date.c_str())) )
					{
						FPRINT_UNRES_VALUE(opt_start_date.c_str());
						Usage();
						return 2;
					}
					else
					{
						if ((strcmp(opt_start_date.substr(0,4).c_str(),"0000") < 0 || strcmp(opt_start_date.substr(0,4).c_str(),"9999") > 0) ||
								(strcmp(opt_start_date.substr(4,2).c_str(),"01") < 0 || strcmp(opt_start_date.substr(4,2).c_str(),"12") > 0) ||
								(strcmp(opt_start_date.substr(6,2).c_str(),"01") < 0 || strcmp(opt_start_date.substr(6,2).c_str(),"31") > 0) )
						{
							FPRINT_UNRES_VALUE(opt_start_date.c_str());
							Usage();
							return 2;
						}
					}
					optIndex++;
					num_e++;
				}
				else {
					FPRINT_UNRES_VALUE(opt_start_date.c_str());
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-b") == 0)		// stop_time
			{
				if (argv[optIndex+1])
				{
					opt_stop_time = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					if ((opt_stop_time.length() != 4) || (!acs_alog_cmdUtil::isNumeric(opt_stop_time.c_str())) )
					{
						FPRINT_UNRES_VALUE(opt_stop_time.c_str());
						Usage();
						return 2;
					}
					else
					{
						if ((strcmp(opt_stop_time.substr(0,2).c_str(),"00") < 0 || strcmp(opt_stop_time.substr(0,2).c_str(),"23") > 0) ||
								(strcmp(opt_stop_time.substr(2,2).c_str(),"00") < 0 || strcmp(opt_stop_time.substr(2,2).c_str(),"59") > 0))
						{
							FPRINT_UNRES_VALUE(opt_stop_time.c_str());
							Usage();
							return 2;
						}
					}
					optIndex++;
					num_b++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-g") == 0)		// stop_date
			{
				if (argv[optIndex+1])
				{
					opt_stop_date = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					if ((opt_stop_date.length() != 8) || (!acs_alog_cmdUtil::isNumeric(opt_stop_date.c_str())) )
					{
						FPRINT_UNRES_VALUE(opt_stop_date.c_str());
						Usage();
						return 2;
					}
					else
					{
						if ((strcmp(opt_stop_date.substr(0,4).c_str(),"0000") < 0 || strcmp(opt_stop_date.substr(0,4).c_str(),"9999") > 0) ||
								(strcmp(opt_stop_date.substr(4,2).c_str(),"01") < 0 || strcmp(opt_stop_date.substr(4,2).c_str(),"12") > 0) ||
								(strcmp(opt_stop_date.substr(6,2).c_str(),"01") < 0 || strcmp(opt_stop_date.substr(6,2).c_str(),"31") > 0) )
						{
							FPRINT_UNRES_VALUE(opt_stop_date.c_str());
							Usage();
							return 2;
						}
					}
					optIndex++;
					num_g++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-l") == 0)		// local_host
			{
				if (argv[optIndex+1])
				{
					opt_local_host = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					optIndex++;
					num_l++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-r") == 0)		// remote_host
			{
				if (argv[optIndex+1])
				{
					opt_remote_host = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					optIndex++;
					num_r++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-i") == 0)		// session_id
			{
				if (argv[optIndex+1])
				{
					opt_session_id = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					optIndex++;
					num_i++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-c") == 0)		// command_id
			{
				if (argv[optIndex+1])
				{
					opt_command_id = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					optIndex++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-t") == 0)		// type
			{
				if (argv[optIndex+1])
				{
					opt_type = argv[optIndex+1];

					if (opt_type.compare("mc")  != 0  &&  opt_type.compare("mci") != 0  &&  opt_type.compare("mcl") != 0  &&
							opt_type.compare("mli") != 0  &&  opt_type.compare("mp")  != 0  &&  opt_type.compare("mpi") != 0  &&
							opt_type.compare("nc")  != 0  &&  opt_type.compare("sec") != 0  &&  opt_type.compare("mmc") != 0  &&
							opt_type.compare("alp") != 0  &&  opt_type.compare("apd") != 0  &&  opt_type.compare("pnc") != 0  &&
							opt_type.compare("mi")  != 0 )
					{
						fprintf(stderr, "%s %s\n\n",str_error_unreas_value.c_str(),opt_type.c_str());
						if ( w_flag )
						{
							if (IsMultiCP)
							{
								fprintf(stderr, "On Multi-CP System, only the following values are accepted with option w and option t\n");
								fprintf(stderr, "mc | mci | mcl | mi | mli | mp | mpi | pnc\n\n");  //xanttro - 07-11-2013 mpa removed
							}
							else
							{
								fprintf(stderr, "On Classic CP System, only the following values are accepted with option w and option t\n");
								fprintf(stderr, "mc | mp | pnc\n\n");
							}
						}
						else
						{
							if (IsMultiCP)
							{
								fprintf(stderr, "On Multi-CP System, only the following values are accepted with option t\n");
								fprintf(stderr, "alp | apd | mc | mci | mcl | mi | mli | mp | mpi | nc | sec\n\n");  //xanttro - 07-11-2013 mpa removed
							}
							else
							{
								fprintf(stderr, "On Classic CP System, only the following values are accepted with option t\n");
								fprintf(stderr, "alp | apd | mc | mmc | mp | nc | sec\n\n");
							}
						}
						return 28;
					}
					if (IsMultiCP == false  &&  (opt_type.compare("mci") == 0  ||
							opt_type.compare("mcl") == 0  ||  opt_type.compare("mi")  == 0  ||
							opt_type.compare("mli") == 0  ||  opt_type.compare("mpi") == 0))
					{
						cerr << "Illegal parameter " << opt_type.c_str() << " in this system configuration " << "\n\n" << flush;
						return 116;
					}
					if (IsMultiCP == true && (opt_type.compare("mmc") == 0) )
					{
						cerr << "Illegal parameter " << opt_type.c_str() << " in this system configuration " << "\n\n" << flush;
						return 116;
					}


					optIndex++;
					num_t++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else if (strcmp(argv[optIndex],"-s") == 0)		// string
			{
				if (argv[optIndex+1])
				{
					opt_string = argv[optIndex+1];
					if (argv[optIndex+1][0]=='-')
					{
						FPRINT_INCOR_USE;
						Usage();
						return 2;
					}
					optIndex++;
					num_s++;
				}
				else {
					FPRINT_UNRES_VALUE("");
					Usage();
					return 2;
				}
			}
			else {
				cout << str_error_illegal_option << argv[optIndex] << endl << flush;
				Usage();
				return 2;
			}
		}
	}

	//Check usage of options
	//Same option may not be used more than once

	if ( (w_flag > 1) ||(num_u > 1) ||(num_d > 1) ||(num_a > 1) ||(num_b > 1) ||(num_e > 1) ||(num_g > 1) ||(num_t > 1) ||(num_s > 1) ||(num_i > 1) ||(num_r > 1))
	{
		FPRINT_INCOR_USE;
		Usage();
		return 2;
	}

	if ((opt_start_date.length() > 1) && (opt_stop_date.length() > 1) )
	{
		if (opt_start_date > opt_stop_date)
		{
			fprintf(stderr, "%s. Start date is later than the stop date\n\n",str_error_unreas_value.c_str());
			return 28;
		}
	}

	if(num_t++)
	{
		if(w_flag && num_t++)
		{
			//TEST
			if (opt_type.compare("mc")  != 0  &&  opt_type.compare("mci") != 0  &&  opt_type.compare("mcl") != 0  &&
					opt_type.compare("mli") != 0  &&  opt_type.compare("mp")  != 0  &&  opt_type.compare("mpi") != 0  &&
					opt_type.compare("pnc") != 0  &&  opt_type.compare("mi")  != 0  /*&& opt_type.compare("mmc") != 0 */)
			{
				fprintf(stderr, "%s %s\n\n",str_error_unreas_value.c_str(),opt_type.c_str());

				if (IsMultiCP)
				{
					fprintf(stderr, "On Multi-CP System, only the following values are accepted with option w and option t\n");
					fprintf(stderr, "mc | mci | mcl | mi | mli | mp | mpi | pnc\n\n");  //xanttro - 07-11-2013 mpa removed
				}
				else
				{
					fprintf(stderr, "On Classic CP System, only the following values are accepted with option w and option t\n");
					fprintf(stderr, "mc | mp | pnc\n\n");
				}
				return 28;
			}
		}
		else
		{

			if (opt_type.compare("mc")  != 0  &&  opt_type.compare("mci") != 0  &&  opt_type.compare("mcl") != 0  &&
					opt_type.compare("mli") != 0  &&  opt_type.compare("mp")  != 0  &&  opt_type.compare("mpi") != 0  &&
					opt_type.compare("nc")  != 0  &&  opt_type.compare("sec") != 0  &&	opt_type.compare("mmc") !=0 &&
					opt_type.compare("alp") != 0  &&  opt_type.compare("apd") != 0  &&  opt_type.compare("mi") != 0)
			{
				fprintf(stderr, "%s %s\n\n",str_error_unreas_value.c_str(),opt_type.c_str());

				if (IsMultiCP)
				{
					fprintf(stderr, "On Multi-CP System, only the following values are accepted with option t\n");
					fprintf(stderr, "alp | apd | mc | mci | mcl | mi | mli | mp | mpi | nc | sec\n\n");  //xanttro - 07-11-2013 mpa removed
				}
				else
				{
					fprintf(stderr, "On Classic CP System, only the following values are accepted with option t\n");
					fprintf(stderr, "alp | apd | mc | mmc | mp | nc | sec \n\n");
				}
				return 28;
			}
		}
	}

	if ((opt_start_time.length() > 1) && (opt_stop_time.length() > 1) )
	{
		if (opt_start_date.length() > 1  &&  opt_stop_date.length() > 1)
		{
			if (opt_start_date == opt_stop_date)
			{
				if (opt_start_time > opt_stop_time)
				{
					fprintf(stderr, "%s! Start time is later than the stop time\n\n",str_error_unreas_value.c_str());
					return 28;
				}
			}
		}
	}

	if (num_a > 0  &&  num_e == 0)
	{
		FPRINT_INCOR_USE;  Usage();
		return 2;
	}

	if (num_b > 0  &&  num_g == 0)
	{
		FPRINT_INCOR_USE;  Usage();
		return 2;
	}

	if (num_z == 1  &&  num_w == 1)		// Options -p and -cp may not be given together
	{
		cerr << "Illegal combination of options -p -cp " << "\r\n\r\n" << flush;
		return 21;
	}

	if (w_flag)
	{
		ACS_CS_API_CommonBasedArchitecture::ApgNumber  APtype;

		ACS_CS_API_NetworkElement::getApgNumber (APtype);

		if (APtype == 2)		//  AP2
		{
			fprintf(stderr,"This is the AP2 node, the Protected Logging function doesn' t exist\n");
			return 3;
		}

		int retcode = acs_alog_cmdUtil::PasswordHandling();

		if (retcode == 29)  cerr << endl << "Internal Audit Log Problems "   << endl;
		if (retcode !=  0)  return(retcode);
	}

	if (CpRoleUser == true  &&		   // If the user is a CPRole<i> only, he must use the -t [mci mcl mi mli mpi] option
			SysSecAdminUser == false)
	{
		if (num_t == 0)
		{
			fprintf (stderr, "User not authorized\n\n");
			return 24;
		}
		if (opt_type.compare("mci") != 0  &&  opt_type.compare("mcl") != 0  &&
				opt_type.compare("mi")  != 0  &&  opt_type.compare("mli") != 0  &&  opt_type.compare("mpi"))
		{
			fprintf (stderr, "User not authorized\n\n");
			return 24;
		}
	}

	std::list<std::string>  auditFiles;
	DIR  *handle = NULL;
	struct dirent  *entry;
	string  pathAbs, fName, fname;

	auditFiles.clear();

	int retNameTagInfo = getTQandNametagInfo();				// TR HV81395 - fetch TQ name if attached and fetch rename template, nametag1 and nametag2 attributes of TQ

	if (! w_flag)  handle = opendir (FileAudit);
	else		   handle = opendir (FilePAudit);

	if (handle == NULL)			//  Error in the File System
	{
		cout << "File access error" << endl;
		return 22;
	}


	while ((entry = readdir(handle)) != NULL)
	{
		fName = entry->d_name;
		if(fName == "." || fName == ".." || fName == "TQdefined.txt" || fName == "ClogTQdefined.txt")
		{
			continue;
		}

		if((w_flag == 1))									// TR HV81395 - renamed files should also be added into list, sorted and printed
		{													// So do not check for file name template here. Instead, check the filename template while parsing date/time
			auditFiles.push_back(fName);
		}
		else if ((w_flag == 0) && (entry->d_type == DT_REG))
		{
			auditFiles.push_back(fName);
		}
	}

	closedir (handle);
	auditFiles.sort(compare_filename);


	std::list<std::string>::iterator  outputs;

	string  fileDate = "", nameOfFileToStart = "", fileToStart = "00000000";
	outputs = auditFiles.begin();

	char fdate[ALOG_DATE_TIME_MAX_LEN +1]={0};
	if (opt_start_date.length() == 8)           //  Performances improvement when
	{											//  the start date is used
		if((auditFiles.size() == 1) && (outputs != auditFiles.end()))
		{
			nameOfFileToStart = *outputs;
			if(isLogfileRenamed(nameOfFileToStart) == false)
				get_datetime_from_fname(nameOfFileToStart.c_str(), fdate);
			else
				get_datetime_from_renamedFile(nameOfFileToStart, fdate);					// TR HV81395 - get date and time for renamed files

			fileToStart.assign(fdate,ALOG_DATE_MAX_LEN);
		}
		else
		{
			fileToStart = getDateOfFileClosestToStartDate(opt_start_date, nameOfFileToStart);	// TR HV81395 - determine file to start using last modified timestamps
		}

		while(outputs != auditFiles.end())
		{
			if(*outputs == nameOfFileToStart)
				break;
			else
				outputs++;
		}
	}

	bool stopDateReached = false;

	while(outputs != auditFiles.end())
	{
		fName.assign ((*outputs));
		//strcpy(fdate, "");

		if((isLogfileRenamed(fName) == true) && (findNametagInRenamedFile(fName) == false))
		{
			if(isRenamedLogfileValid(fName) == false)							// TR HV81395 - if file is renamed and nametags of TQ are not found in the name, validate whether the file is audit/paudit file
			{
				outputs = auditFiles.erase(outputs);
				continue;
			}
		}

		if((opt_start_date.length() == 8) || (opt_stop_date.length() == 8))
		{
			if(auditFiles.size() == 1)  // TR HY16322 - Fetching of filedate when auditFiles.size() = 1
			{
				fileDate.assign(fdate,ALOG_DATE_MAX_LEN);
			}
			else
			{
				std::map<std::string, time_t>::iterator it;
				it = logfileTimestamps.find(fName);
				fileDate = convertEpochTimeToString(&it->second);		// TR HV81395 - fetch date of file
			}
		}

		if (opt_start_date.length() == 8)
		{
			if (fileDate.compare(fileToStart) < 0)
			{
				outputs++;
				continue;
			}
		}

		if (opt_stop_date.length() == 8)        	//  Performances improvement when
		{											//  the stop date is used
			if (fileDate.compare(opt_stop_date) > 0) {
				stopDateReached = true;							// TR HV81395 - print current file and then break from loop in case stop date is reached
			}
		}

		if (w_flag == 0)  pathAbs.assign(FileAudit);
		else			  pathAbs.assign(FilePAudit);

		pathAbs += fName;
		printRecords(pathAbs);
		outputs++;

		if(stopDateReached)
			break;
	}

	while(w_flag){//Checking in the current plog file in /data path.
		handle = opendir ("/data/acs/data/alog");
		if (handle == NULL)         //  Error in the File System
		{
			cout << "File access error" << endl;
			return 22;
		}

		if(handle){
			while ((entry = readdir(handle)) != NULL)
			{
				fName = entry->d_name;

				if (fName.substr(0,7).compare("paudit.") == 0  ||  fName.substr(0,9).compare("PLOGFILE_") == 0) {
					fname=fName;
				}
				else
					continue;
				//now need to decrypt the encrypted file!

				fname = "/data/acs/data/alog/" + fname;
				string file_content = "";
				ifstream sourceFile;
				sourceFile.open(fname.c_str(), ifstream::in);
				stringstream strStream;
				strStream << sourceFile.rdbuf();
				string str = strStream.str(); //content of the file

				sourceFile.close();
				string decrypt_content="";
				string temp = "";
				long int len = strlen(str.c_str());

				long int ref = 0;
				strStream.seekg(0,ios::beg);
				while(ref < len)
				{
					strStream >> temp;

					decrypt_content += decryptString(temp);

					temp += '\n';
					ref += strlen(temp.c_str());

				}
				temp.clear();
				printRecords(fname,decrypt_content);
				decrypt_content.clear();
				//unlink (fName.c_str());


			}
			closedir (handle);

		}
		break;
	}

	cout << endl;

	exit (0);
}


/*
isLogfileRenamed() - This method verifies whether the file 'fName' is a complying with the default filename templates or not
Returns	:	false, if fName matches one of default filename templates
			true, if fName does not match any of the default filename templates
*/

bool isLogfileRenamed(const string fName)
{
	bool ret = false;

	if(w_flag == 0)
	{
		if ((fName.substr(0,6).compare("audit.") == 0) ||
				(fName.substr(0,8).compare("LOGFILE-") == 0) ||
				(fName.substr(0,8).compare("Logfile-") == 0))
			ret = false;			// not renamed

		else
			ret = true;				// renamed
	}
	else
	{
		if ((fName.substr(0,7).compare("paudit.") == 0) ||
				(fName.substr(0,9).compare("PLOGFILE_") == 0))
			ret = false;
		else
			ret = true;
	}

	return ret;
}

/*
findNametagInRenamedFile()	-	fName is already verified to be renamed by isLogfileRenamed() method
This method checks whether the nametag attributes defined in the attached TQ are found in fName.
Returns	:	false, if the nametag attributes are not found in the filename
			true, if the nametag attributes are found in the filename
*/

bool findNametagInRenamedFile(const string fName)
{
	bool foundNametag = false;

	if((isTQDefined == true) && (transferQueueAttached != "") && (renameTemplate != ""))
	{
		if( ((nameTag1 != "") && (fName.find(nameTag1) != string::npos))
				|| ((nameTag2 != "") && (fName.find(nameTag2) != string::npos)) )
			foundNametag = true;
	}

	return foundNametag;
}


/*
	AFP renames files if NameTag attribute is configured in TQ, fetching date and time from the file names is not possible for such files.
	In such cases, read the first event, parse event-size, date and time. If the event-size, date or time fields are found to be improper, do NOT consider
	this file 'fName' as a audit log file.
 */
int get_datetime_from_renamedFile(const string fName, char fDate[])
{
	string line, datetimeStr = "";
	string absoluteFileName = "", tempPlogExtractedFile = "";
	ifstream file;

	int retCode = -1;

	if(w_flag == 0)
	{
		absoluteFileName.assign(FileAudit);
		absoluteFileName.append(fName);

		file.open(absoluteFileName.c_str(),std::ifstream::in);

		if(!file.is_open())
			return retCode;

		if(file.good())
		{
			getline (file, line);
			if (line.length() > 0)
			{
				string tmpField = "";
				char tmpLine[30];
				strncpy(tmpLine, line.c_str(), 30);

				char * field;
				field = strtok(tmpLine,";");

				if((field == NULL) || (acs_alog_cmdUtil::isNumeric (field) == false))							// first field is event length
				{
					file.close();
					return retCode;
				}

				field = strtok(NULL,";");				// Second field is date in YYYY-MM-DD format
				if(field != NULL)
				{
					tmpField.assign(field);
					if (tmpField.length() != 10)
					{
						file.close();
						return retCode;
					}
					size_t hiphenPos = tmpField.find("-");		// Date string is now in YYYY-MM-DD format. Change it to YYYYMMDD.

					if(hiphenPos == string::npos)
					{
						file.close();
						return retCode;
					}

					tmpField.erase(hiphenPos,1);
					hiphenPos = tmpField.find("-");

					if(hiphenPos == string::npos)
					{
						file.close();
						return retCode;
					}

					tmpField.erase(hiphenPos,1);

					if (acs_alog_cmdUtil::isNumeric (tmpField.c_str()) == false)
					{
						file.close();
						return retCode;
					}
					datetimeStr += tmpField;			// build date-time string in YYYYMMDDhhmmss format
				}
				else
				{
					file.close();
					return retCode;
				}

				field = strtok(NULL,";");				// Third field is time in hhmmss format
				tmpField.assign(field);

				if ((tmpField.length() != 6) || (acs_alog_cmdUtil::isNumeric (field) == false))
				{
					file.close();
					return retCode;
				}

				datetimeStr += tmpField;

				strncpy(fDate, datetimeStr.c_str(),ALOG_DATE_TIME_MAX_LEN);
				retCode = 0;
			}
			else
			{
				file.close();
				return retCode;
			}
		}

		file.close();
	}
	else			// For renamed protected logs, get the filename within the renamed 7z archive and pass the name to exiting date/time parsing method
	{
		absoluteFileName.assign(FilePAudit);
		absoluteFileName.append(fName);
		string extractedPlogFilename;
		string cmdListArchiveContents = "7z l " + absoluteFileName + " | egrep 'paudit.|PLOGFILE_' | awk '{print $NF}'";		//extract filename by listing contents of compressed file

		if(acs_alog_cmdUtil::runCommand(cmdListArchiveContents,extractedPlogFilename))
		{
			if(extractedPlogFilename.length() != 0){
				get_datetime_from_fname (extractedPlogFilename.c_str(),fDate);
				retCode = 0;
			}
		}
	}

	return retCode;
}



bool isRenamedLogfileValid(string fName)
{
	bool ret = false;
	char fdate[ALOG_DATE_TIME_MAX_LEN +1] = {0};
	strcpy(fdate,"");

	if (get_datetime_from_renamedFile (fName,fdate) == 0)
		ret = true;

	return ret;
}


/* getTQandNametagInfo() - Fetches nameTag information from TQ object from IMM in case TQ is attached to ALOG/PLOG
Return values: 	-1 is No TQ attached
				 0 is TQ attached and information retrieved
				 1 is IMM failure or unable to read attributes file for PLOG TQ
 */
int getTQandNametagInfo()
{
	int retcode = ACS_CC_FAILURE, immRetCode = 0;
	OmHandler immOmHandler;

	immRetCode = immOmHandler.Init();

	if(immRetCode != ACS_CC_SUCCESS)
	{
		cout<<"ERROR: immOmHandler.init() FAILURE!!! - error = " << immRetCode << endl;
		return retcode;
	}

	if(w_flag == 0)
	{
		std::vector<ACS_APGCC_ImmAttribute *> attributes;
		ACS_APGCC_ImmAttribute transferQueueAttr;
		transferQueueAttr.attrName = AUDITLOGGINGM_TRANSFERQUEUE_ATTR;
		attributes.push_back(&transferQueueAttr);
		immRetCode = immOmHandler.getAttribute(AUDITLOGGINGM_MO.c_str(), attributes);
		if(immRetCode != ACS_CC_SUCCESS)
		{
			cout<<"ERROR: immOmHandler.getAttribute() FAILURE - MO AxeAuditLoggingauditLoggingMId=1 !!!- error = " << immRetCode << endl;
			immRetCode = immOmHandler.Finalize();
			if(immRetCode != ACS_CC_SUCCESS)
				cout<<"ERROR: immOmHandler.Finalize() FAILURE - MO AxeAuditLoggingauditLoggingMId=1 !!! - error = " << immRetCode << endl;
			return retcode;
		}

		if(transferQueueAttr.attrValuesNum != 0)
		{
			retcode = ACS_CC_SUCCESS;
			isTQDefined = true;
			transferQueueAttached = reinterpret_cast<const char*> (*(transferQueueAttr.attrValues));
		}
		else
		{
			retcode = -1;			// TQ not attached
			isTQDefined = false;
			transferQueueAttached = "";
			immRetCode = immOmHandler.Finalize();

			if(immRetCode != ACS_CC_SUCCESS)
				cout<<"ERROR: immOmHandler.Finalize() FAILURE!!! - error = " << immRetCode << endl;

			return retcode;
		}
	}
	else
	{
		string tmpTransferQueueName = "";
		int tempRes = 0;
		ACS_ALOG_Lister * pLister = new ACS_ALOG_Lister();
		if(pLister == NULL)
		{
			cout<<"ERROR: Unable to fetch PLOG TQ information" << endl;
			retcode = ACS_CC_FAILURE;
			delete pLister;
			pLister = NULL;
			immRetCode = immOmHandler.Finalize();
			if(immRetCode != ACS_CC_SUCCESS)
				cout<<"ERROR: immOmHandler.Finalize() FAILURE!!! - error = " << immRetCode << endl;

			return retcode;
		}

		tempRes = pLister->GetRetCode();
		if (tempRes != 0) {
			cerr << "ERROR: File access error while fetching PLOG TQ information" << endl;
			retcode = ACS_CC_FAILURE;
			delete pLister;
			pLister = NULL;
			immRetCode = immOmHandler.Finalize();
			if(immRetCode != ACS_CC_SUCCESS)
				cout<<"ERROR: immOmHandler.Finalize() FAILURE!!! - error = " << immRetCode << endl;

			return retcode;
		}

		tmpTransferQueueName = pLister->GetFileDest();
		delete pLister;
		pLister = NULL;
		if(tmpTransferQueueName != "")
		{
			retcode = ACS_CC_SUCCESS;
			transferQueueAttached = tmpTransferQueueName;
			isTQDefined = true;
		}
		else
		{
			retcode = -1;				// TQ not attached
			transferQueueAttached = "";
			isTQDefined = false;
			immRetCode = immOmHandler.Finalize();

			if(immRetCode != ACS_CC_SUCCESS)
				cout<<"ERROR: immOmHandler.Finalize() FAILURE!!! - error = " << immRetCode << endl;

			return retcode;
		}
	}

	string transferQueueDN = FILETRANSFERQUEUEID + transferQueueAttached + "," + FILETRANSFERMANAGERRDN;
	std::vector<std::string> rdnChildren;
	ACS_APGCC_ImmObject transferQueueObj;
	transferQueueObj.objName = transferQueueDN;
	retcode = ACS_CC_FAILURE;

	immRetCode = immOmHandler.getChildren(transferQueueDN.c_str(), ACS_APGCC_SUBLEVEL, &rdnChildren);
	if((immRetCode != ACS_CC_SUCCESS) || (rdnChildren.size() == 0))
	{
		cout<<"ERROR: immOmHandler.getChildren() FAILURE or unable to fetch advancedConfiguration object of TQ!!!- error = " << immRetCode << endl;
		immRetCode = immOmHandler.Finalize();
		if(immRetCode != ACS_CC_SUCCESS)
			cout<<"ERROR: immOmHandler.Finalize() FAILURE!!! - error = " << immRetCode << endl;

		return retcode;
	}

	string advancedInfoTransferQueueDN = rdnChildren[0];
	ACS_APGCC_ImmObject advancedInfoTQ;
	advancedInfoTQ.objName = advancedInfoTransferQueueDN;

	immRetCode = immOmHandler.getObject(&advancedInfoTQ);
	if(immRetCode != ACS_CC_SUCCESS)
	{
		cout<<"ERROR: immOmHandler.getObject() FAILURE - MO AdvancedConfiguration !!!- error = " << immRetCode << endl;

		immRetCode = immOmHandler.Finalize();

		if(immRetCode != ACS_CC_SUCCESS)
			cout<<"ERROR: immOmHandler.Finalize() FAILURE - MO AdvancedConfiguration !!! - error = " << immRetCode << endl;

		return retcode;
	}

	std::vector<ACS_APGCC_ImmAttribute>::iterator attributeIterator;

	for(attributeIterator = advancedInfoTQ.attributes.begin(); attributeIterator != advancedInfoTQ.attributes.end(); ++attributeIterator)
	{
		if((*attributeIterator).attrType == ATTR_STRINGT)
		{
			if((*attributeIterator).attrName == NAMETAG1_ATTR)
			{
				if((*attributeIterator).attrValuesNum != 0)
				{
					string tmpStr =  reinterpret_cast<char *>((*attributeIterator).attrValues[0]);
					nameTag1.assign(tmpStr);
				}
			}
			else if((*attributeIterator).attrName== NAMETAG2_ATTR)
			{
				if((*attributeIterator).attrValuesNum != 0)
				{
					string tmpStr =  reinterpret_cast<char *>((*attributeIterator).attrValues[0]);
					nameTag2.assign(tmpStr);
				}
			}
			else if((*attributeIterator).attrName== TEMPLATE_ATTR )
			{
				if((*attributeIterator).attrValuesNum !=0)
				{
					string tmpStr =  reinterpret_cast<char *>((*attributeIterator).attrValues[0]);
					renameTemplate.assign(tmpStr);
				}
			}
		}

	}
	retcode = ACS_CC_SUCCESS;
	immRetCode = immOmHandler.Finalize();
	if(immRetCode != ACS_CC_SUCCESS)
		cout<<"ERROR: immOmHandler.Finalize() FAILURE!!! - error = " << immRetCode << endl;

	return retcode;

}

string decryptString(const std::string &strPwdCrypt)//HX86564
{
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	char * plaintext = NULL;
	std::string ciphertext = strPwdCrypt;
	std::string strPwdDecrypt("");
	SecCryptoStatus decryptStatus;

	if(!strPwdCrypt.empty())
	{

		decryptStatus = sec_crypto_decrypt_ecimpassword_legacy(&plaintext, ciphertext.c_str());
		if(decryptStatus != SEC_CRYPTO_OK )
		{
			::snprintf(log_buffer, log_buffer_size, "Decryption failed");
			//log.Write(log_buffer, LOG_LEVEL_ERROR);
		}else{
			strPwdDecrypt = plaintext;

		}
		if(plaintext !=0 )
			free(plaintext);
	}
	return strPwdDecrypt;
}
