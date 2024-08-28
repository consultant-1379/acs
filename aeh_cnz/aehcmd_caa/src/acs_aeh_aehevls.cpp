/*
 * acs_aeh_aehevls.cpp
 *
 *  Created on: Sep 05, 2011
 *      Author: xgiopap
 */


#include <iostream>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <map>
#include <sstream>

#include <fstream>

using namespace std;

//====================================================================================================================================================
//Macro definition
//====================================================================================================================================================
#ifndef DEFAULT_MAX_RECORDS_TO_DISPLAY
#define DEFAULT_MAX_RECORDS_TO_DISPLAY 100
#endif

#ifndef MAX_NUMBER_CHAR_TO_READ
#define MAX_NUMBER_CHAR_TO_READ 4096
#endif

//====================================================================================================================================================
//global variable declaration
//====================================================================================================================================================
int opt_numberOfRecords = DEFAULT_MAX_RECORDS_TO_DISPLAY;

bool opt_oldestFirst = true;
bool opt_waitForNew = false;

string opt_startTime;
string opt_stopTime;
string opt_perceivedSeverity;
string opt_processName;
string opt_objectOfReference;
string opt_specificProblem;
string opt_probableCause;
string opt_objectClassOfReference;
string opt_user;
string opt_logtype;

int numOfRecord = 0;

std::list<string> recordOutputs;

int num_a = 0;
int num_c = 0;
int num_l = 0;
int num_m = 0;
int num_p = 0;
int num_q = 0;
int num_t = 0;
int num_u = 0;
int num_x = 0;
int num_y = 0;

//====================================================================================================================================================
//Internal Function declaration
//====================================================================================================================================================

void usage();
bool isNumeric(const char *s);
bool match(const string& source, const string& pattern);
bool rMatch(const string& s, unsigned int i, const string& p, unsigned int j);
bool parseAPEventMessage(std::string line, std::string &recordOutput);
bool parseSSEventMessage(std::string line, std::string &recordOutput);
std::string getFormattedProblemDataText(std::string p_text);
std::string trim(std::string parameter);

//====================================================================================================================================================
//Internal Function implementation
//====================================================================================================================================================


//----------------------------------------------------------------------------------------------------------------------------------------------------
//usage method
//----------------------------------------------------------------------------------------------------------------------------------------------------
void usage()
{
	//Display aehevls usage
	cerr << "Usage: aehevls [-a user][-c source][-t startdate][-u starttime][-x stopdate][-y stoptime][-l logtype][-m number][-p][-q]" << endl
		<< "  -a Search on user" << endl
		<< "  -c Search on source" << endl
		<< "  -l Search on EventLog (app, sys, sec)" << endl
		<< "  -m Print max x number of records" << endl
		<< "  -p Print in reverse order" << endl
		<< "  -q Display events from current time and wait for new events to arrive" << endl
		<< "  -t Search on start date (YYYYMMDD, YYMMDD)." << endl
		<< "     Can also be used to set start time by adding -HHMM to the date." << endl
		<< "  -u Search on start time (HHMM)" << endl
		<< "  -x Search on stop date (YYYYMMDD, YYMMDD)." << endl
		<< "     Can also be used to set stop time by adding -HHMM to the date." << endl
		<< "  -y Search on stop time (HHMM)" << endl
		<< endl
		<< "All searches except start/stop date/time allow '*' as wild card." << endl
		<< endl;

}  //Usage()


//----------------------------------------------------------------------------------------------------------------------------------------------------
//isNumeric method
//----------------------------------------------------------------------------------------------------------------------------------------------------
bool isNumeric(const char *s)
{

	int l = strlen(s);

	for(int i = 0; i<l; i++)
	{

		if((s[i] < '0') || (s[i] > '9'))
			return false;
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
//match method
//----------------------------------------------------------------------------------------------------------------------------------------------------
bool match(const string& source, const string& pattern)
{
	return rMatch(source, 0, pattern, 0);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
//match method
//----------------------------------------------------------------------------------------------------------------------------------------------------
bool rMatch(const string& s, unsigned int i, const string& p, unsigned int j)
{
	// s = to be tested,     i = position in s
	// p = pattern to match, j = position in p
	bool matched;
	unsigned int k;

	if (p.length() == 0)
		return true;

	for (;;)
	{
		if (i >= s.length() && j >= p.length())
			return true;

		else if (j >= p.length())
			return false;

		else if (p[j] == '*')
		{
			k = i;
			if (j >= p.length()-1)
				return true;

			for (;;)
			{
				matched = rMatch(s,k,p,j+1);
				if (matched || (k >= s.length()))
					return matched;
				k++;
			}
		}
		else if ((p[j]=='?' && i<s.length()) || (toupper(p[j]) == toupper(s[i])))
		{
			i++;
			j++;
		}
		else
			return false;
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
//parseAPEventMessage method
//----------------------------------------------------------------------------------------------------------------------------------------------------
bool parseAPEventMessage(std::string line, std::string &recordOutput)
{
	std::string  parLine = line;

	try
	{
		std::string firstPart;
		std::string secondPart;
		size_t pos;

		pos = line.find(" ");
		std::string data = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string time = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string typeEvent = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string nodeName = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string stateNode = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string priority = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string user = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string processName = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string specificNumber = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string severity = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string cease = line.substr(0, pos);

		pos = line.find("P_CAUSE:");
		firstPart = line.substr(0, pos);
		secondPart = line.substr(pos);

		line = line.substr(pos+10);
		pos = line.find("CLASS_REF");
		std::string p_cause = line.substr(0, pos-1);

		line = line.substr(pos+11);
		pos = line.find("OBJ_REF");
		std::string class_ref = line.substr(0, pos-1);

		line = line.substr(pos+9);
		pos = line.find("P_DATA");
		std::string obj_ref = line.substr(0, pos-1);

		line = line.substr(pos+8);
		pos = line.find("P_TEXT");
		std::string p_data = line.substr(0, pos-1);

		std::string p_text = line.substr(pos+8);

		p_data = getFormattedProblemDataText(p_data);
		p_text = getFormattedProblemDataText(p_text);

		string eventTime ="";
		eventTime.append( data.substr(0,4) + data.substr(5,2) + data.substr(8,2));
		eventTime.append("-");
		eventTime.append( time.substr(0,2) + time.substr(3,2));

		//Check start and stop time
		if((opt_startTime.length() > 1) && (strcmp(eventTime.c_str(), opt_startTime.c_str())) < 0)
			return false;

		if((opt_stopTime.length() > 1) && (strcmp(eventTime.c_str(), opt_stopTime.c_str())) > 0)
			return false;

		if((processName.length() > 0) && (opt_processName.length() > 0) && (!match(trim(processName), opt_processName)))
		{
			return false;
		}

		if((user.length() > 0) && (opt_user.length() > 0) && (!match(trim(user), opt_user)))
		{
			return false;
		}

		recordOutput.append("Date                :      " + data + " " + time + "\n");
		recordOutput.append("Source              :      " + ((processName.length() > 0 ) ? processName : "?") + "\n");
		recordOutput.append("Nodename            :      " + ((nodeName.length() > 0) ? nodeName : "?") + "\n");
		recordOutput.append("Priority            :      " + ((priority.length() > 0) ? priority : "?") + "\n");
		recordOutput.append("User                :      " + ((user.length() > 0) ? user : "?") + "\n");
		recordOutput.append("Problem Data        :      " + ((p_data.length() > 0) ? p_data : "-") + "\n");
		recordOutput.append("Description         :      " + ((p_text.length() > 0) ? p_text : "-") + "\n");
		recordOutput.append("\n");

		numOfRecord++;

		return true;
	}
	catch (...)
	{
		//		cout << "\nCatched exception in : " << parLine << endl << endl;
		return false;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//parseSSEventMessage method
//----------------------------------------------------------------------------------------------------------------------------------------------------
bool parseSSEventMessage(std::string line, std::string &recordOutput)
{
	std::string  parLine = line;

	try
	{
		std::string description;
		size_t pos;

		pos = line.find(" ");
		std::string data = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string time = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string typeEvent = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string processName = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string nodeName = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string priority = line.substr(0, pos);

		line = line.substr(pos+1);
		pos = line.find(" ");
		std::string tmpPrcName = line.substr(0, pos);

		description = line.substr(pos+1);

		string eventTime ="";
		eventTime.append( data.substr(0,4) + data.substr(5,2) + data.substr(8,2));
		eventTime.append("-");
		eventTime.append( time.substr(0,2) + time.substr(3,2));

		//Check start and stop time
		if((opt_startTime.length() > 1) && (strcmp(eventTime.c_str(), opt_startTime.c_str())) < 0)
			return false;

		if((opt_stopTime.length() > 1) && (strcmp(eventTime.c_str(), opt_stopTime.c_str())) > 0)
			return false;

		if((processName.length() > 0) && (opt_processName.length() > 0) && (!match(trim(processName), opt_processName)))
		{
			return false;
		}

		recordOutput.append("Date                :      " + data + " " + time + "\n");
		recordOutput.append("Source              :      " + ((processName.length() > 0) ? processName : "?") + "\n");
		recordOutput.append("Nodename            :      " + ((nodeName.length() > 0) ? nodeName : "?") + "\n");
		recordOutput.append("Priority            :      " + ((priority.length() > 0) ? priority : "?") + "\n");
		recordOutput.append("User                :      -\n");
		recordOutput.append("Problem Data        :      -\n");
		recordOutput.append("Description         :      " + ((description.length() > 0) ? description : "-") + "\n");
		recordOutput.append("\n");

		numOfRecord++;

		return true;
	}
	catch (...)
	{
		//		cout << "\nCatched exception in : " << parLine << endl << endl;
		return false;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//getFormattedProblemDataText method
//----------------------------------------------------------------------------------------------------------------------------------------------------
std::string getFormattedProblemDataText(std::string p_text)
{
	std::string newProblem;

	char str[p_text.length()];
	::sprintf(str, "%s",p_text.c_str());

	char *pch = 0;
	pch = strtok(str, "##");

	while(pch != 0)
	{
		newProblem += pch;
		pch = strtok(0, "##");
		if(pch !=0)
			newProblem += "\n";
	}

	return newProblem;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
//trim method
//----------------------------------------------------------------------------------------------------------------------------------------------------
std::string trim(std::string field)
{
	int lenght = field.length();
	char c = field.c_str()[lenght-1];

	if((lenght > 0) && ( isspace(c)))
		return field.erase((lenght-1), 1);

	return field;
}


//========================================================================================
//	Main program
//========================================================================================

int main(int argc, char *argv[])
{
	int optIndex = 0;
	int moreEventsCheck = 0;
	string temp = "";
	string tempCopy_start = "";
	string tempCopy_stop = "";
	string strMaxNumOfEvents = "";

	std::stringstream tmp;
	moreEventsCheck = opt_numberOfRecords + 1;
	tmp << moreEventsCheck;
	tmp >> strMaxNumOfEvents;				//TR HW68675 - grep for 101 events to check if 'more events to show' message has to be printed

	opterr = 0;

	if ( argc == 1 ){ // aehevls has been executed without any options

		// Compose the path of event log file
		char defNodeName[32] = {0};
		string defPathEventLog("/var/log/");

		gethostname( defNodeName, 32);
		if( defNodeName == 0){
			fprintf(stderr, "ERROR: Failed to open EventLog\n\n");
			return 1;
		}

		defPathEventLog.append(defNodeName);
		defPathEventLog.append("/messages");

		string line = "";
		string defRecordOutput = "";
		size_t found;

		std::list<string> defRecordOutputs;

		FILE* cmd = NULL;
		ssize_t read;
		string cmdString = "grep -E 'APEvent | SystemEvent | SecurityEvent' " + defPathEventLog + " | tail -" + strMaxNumOfEvents;  //TR HW68675

		char *buf = NULL;
		size_t len;

		cmd = popen(cmdString.c_str(),"r");

		cout << endl;

		if (cmd != NULL) {
			while((read = getline(&buf, &len, cmd)) != -1){
				line = ::string(buf, (strlen(buf) - 1));
				found = line.find("APEvent");

				if((found != string::npos) && ((int)found == 20)){
					defRecordOutput = "";
					if(parseAPEventMessage(line, defRecordOutput))
					{
						defRecordOutputs.push_back(defRecordOutput);
					}
				}
				else{
					found = line.find("SystemEvent");
					if((found != string::npos) && ((int)found == 20)){
						defRecordOutput = "";
						if(parseSSEventMessage(line, defRecordOutput))
						{
							defRecordOutputs.push_back(defRecordOutput);
						}
					}
					else {
						found = line.find("SecurityEvent");
						if((found != string::npos) && ((int)found == 20)){
							defRecordOutput = "";
							if(parseSSEventMessage(line, defRecordOutput))
							{
								defRecordOutputs.push_back(defRecordOutput);
							}
						}
					}
				}
			}
			pclose(cmd);
		}
		else {
			fprintf(stderr, "ERROR: Failed to open EventLog\n\n");
			return 1;
		}

		//sort the list

		defRecordOutputs.sort();

		std::list<std::string>::iterator defOutputs;
		defRecordOutputs.reverse();

		int i = 1;
		for(defOutputs = defRecordOutputs.begin(); defOutputs != defRecordOutputs.end(); defOutputs++){
			cout << *(defOutputs);
			i++;

			if ( i > opt_numberOfRecords ){
				if ((int)defRecordOutputs.size() > opt_numberOfRecords){ // HO92774
					cout << "\nMore events to show\n\n";
				}
				break;
			}
		}
		return 0;
	}

	for (optIndex = 1; optIndex < argc; optIndex++)
	{
		if(strlen(argv[optIndex]) > 2 || strlen(argv[optIndex]) < 2)
		{
			fprintf(stderr, "ERROR: Incorrect usage\n\n");
			usage();
			return 2;
		}

		if(strcmp (argv[optIndex], "-a") != 0 && strcmp (argv[optIndex], "-c") != 0 && strcmp (argv[optIndex], "-t") != 0 &&
				strcmp (argv[optIndex], "-u") != 0 && strcmp (argv[optIndex], "-x") != 0 && strcmp (argv[optIndex], "-y") != 0 &&
				strcmp (argv[optIndex], "-l") != 0 && strcmp (argv[optIndex], "-m") != 0 && strcmp (argv[optIndex], "-p") != 0 &&
				strcmp (argv[optIndex], "-q") != 0)
		{
			fprintf(stderr,"ERROR: Invalid parameter %s\n\n", argv[optIndex]);
			usage();
			return 2;
		}

		if (strcmp (argv[optIndex], "-a") == 0)	// Fetch parameter -a
		{
			// Fetch the value of user
			if (argv[optIndex + 1])
			{
				opt_user = argv[optIndex + 1];
				optIndex++;
				num_a++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-c") == 0)	// Fetch parameter -c
		{
			// Fetch the value of source
			if (argv[optIndex + 1])
			{
				opt_processName = argv[optIndex + 1];
				optIndex++;
				num_c++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-t") == 0)	// Fetch parameter -t
		{
			// Fetch the value of start data
			if (argv[optIndex + 1])
			{
				temp = argv[optIndex + 1];
				tempCopy_start = temp;

				if((temp.length() == 6) || (temp.length() == 11))
				{
					if( temp[0] < '7' )
					{
						temp = "20" + temp;
					}
					else
					{
						temp = "19" + temp;
					}
				}

				if((temp.length() == 8) && (isNumeric(temp.c_str())))
				{
					temp += "-0000";
				}
				else if((temp.length() == 13) &&  (match(temp, "*-*")))
				{
					int ret = 0;
					int pos_date = 0;
					int pos_time = 9;
					int date_len = 8;
					int time_len = 4;
					num_t = 0;
					num_u = 0;

					ret = temp.find("-");
					if(ret == 8)
					{
						string date_l13 = temp.substr(pos_date, date_len);
						string time_l13 = temp.substr(pos_time, time_len);
						if((isNumeric(date_l13.c_str())) && (isNumeric(time_l13.c_str())))
						{
							num_u++;
						}
						else
						{
							fprintf(stderr, "ERROR: startdate %s\n\n", tempCopy_start.c_str());
							return 5;
						}
					}
					else
					{
						fprintf(stderr, "ERROR: startdate %s\n\n", tempCopy_start.c_str());
						return 5;
					}
				}
				else
				{
					fprintf(stderr, "ERROR: startdate %s\n\n", tempCopy_start.c_str());
					return 5;
				}

				opt_startTime = temp;
				optIndex++;
				num_t++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-u") == 0)	// Fetch parameter -u (HHMM)
		{
			// Fetch the value of start time
			if (argv[optIndex + 1])
			{
				temp = argv[optIndex + 1];
				if(!isNumeric(temp.c_str()))
				{
					fprintf(stderr, "ERROR: starttime %s\n\n", temp.c_str());
					return 7;
				}
				if((temp.length() != 4) || temp.substr(0,2) > "23" || temp.substr(2,2) > "59")
				{
					fprintf(stderr, "ERROR: starttime %s\n\n", temp.c_str());
					return 7;
				}
				else
				{
					int pos = opt_startTime.find("-");
					if (pos >= 0)
					{
						opt_startTime = opt_startTime.substr(0, pos + 1) + temp;
					}
					else
					{
						opt_startTime = "-" + temp;
					}
				}

				optIndex++;
				num_u++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-x") == 0)	// Fetch parameter -x (YYYYMMDD or YYMMDD and optional -HHMM)
		{
			// Fetch the value of stop date
			if (argv[optIndex + 1])
			{
				temp = argv[optIndex + 1];
				tempCopy_stop = temp;

				if((temp.length() == 6) || (temp.length() == 11))
				{
					if( temp[0] < '7'  )
						temp = "20" + temp;
					else
						temp = "19" + temp;
				}

				if((temp.length() == 8) && isNumeric(temp.c_str()))
				{
					temp += "-2359";
				}
				else if ((temp.length() == 13) && match(temp, "*-*"))
				{
					int ret = 0;
					int pos_date = 0;
					int pos_time = 9;
					int date_len = 8;
					int time_len = 4;

					num_x = 0;
					num_y = 0;

					ret = temp.find("-");

					if(ret == 8)
					{
						string date_l13 = temp.substr(pos_date, date_len);
						string time_l13 = temp.substr(pos_time, time_len);
						if((isNumeric(date_l13.c_str())) && isNumeric(time_l13.c_str()))
						{
							num_y++;
						}
						else
						{
							fprintf(stderr, "ERROR: stopdate %s\n\n", tempCopy_stop.c_str());
							return 9;
						}
					}
					else
					{
						fprintf(stderr, "ERROR: stopdate %s\n\n", tempCopy_stop.c_str());
						return 9;
					}
				}
				else
				{
					fprintf(stderr, "ERROR: stopdate %s\n\n", tempCopy_stop.c_str());
					return 9;
				}

				opt_stopTime = temp;
				optIndex++;
				num_x++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-y") == 0)	// Fetch parameter -y (HHMM)
		{
			// Fetch the value of stop time
			if (argv[optIndex + 1])
			{
				temp = argv[optIndex + 1];
				if(!isNumeric(temp.c_str()))
				{
					fprintf(stderr, "ERROR: stoptime %s\n\n", temp.c_str());
					return 10;
				}

				if ((temp.length() != 4) || (temp.substr(0,2) > "23") || (temp.substr(2,2) > "59"))
				{
					fprintf(stderr, "ERROR: stoptime %s\n\n", temp.c_str());
					return 10;
				}
				else
				{
					int pos = opt_stopTime.find("-");
					if(pos >= 0)
					{
						opt_stopTime = opt_stopTime.substr(0, pos + 1) + temp;
					}
					else
					{
						opt_stopTime = "-" + temp;
					}
				}

				optIndex++;
				num_y++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-l") == 0)	// Fetch parameter -l
		{
			// Fetch the value of log typ
			if (argv[optIndex + 1])
			{
				opt_logtype = argv[optIndex + 1];

				if(opt_logtype.compare("app") != 0 && opt_logtype.compare("sys") != 0 && opt_logtype.compare("sec") != 0)
				{
					fprintf(stderr, "ERROR: -l parameter value must be app/sec/sys\n\n");
					return 4;
				}

				optIndex++;
				num_l++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-m") == 0)	// Fetch parameter -m
		{
			// Fetch the value of number
			if (argv[optIndex + 1])
			{
				temp = argv[optIndex + 1];

				if(temp[0] == '0')
				{
					fprintf(stderr,"ERROR: -m parameter value must be greater than 0 or the first digit must be different by 0\n\n");
					return 11;
				}

				if (!isNumeric(temp.c_str()))
				{
					fprintf(stderr,"ERROR: -m parameter value not numeric. Number must be numeric and in the interval 1-999.999.999\n\n");
					return 12;
				}

				opt_numberOfRecords = ::atoi(temp.c_str());

				if (opt_numberOfRecords == 0)
				{
					fprintf(stderr,"ERROR: -m parameter value not numeric. Number must be numeric and in the interval 1-999.999.999\n\n");
					return 12;
				}

				if (temp.length() > 9)
				{
					fprintf(stderr,"ERROR: -m parameter value must be less than 1.000.000.000\n\n");
					return 13;
				}

				if (opt_numberOfRecords >= 1000000000)
				{
					fprintf(stderr,"ERROR: -m parameter value must be less than 1.000.000.000\n\n");
					return 13;
				}

				optIndex++;
				num_m++;
			}
			else
			{
				fprintf(stderr,"ERROR: A value must be specified for parameter %s\n\n", argv[optIndex]);
				return 3;
			}
		}
		else if (strcmp (argv[optIndex], "-p") == 0)	// Fetch parameter -p
		{
			opt_oldestFirst = false;
			num_p++;
		}
		else if (strcmp (argv[optIndex], "-q") == 0)	// Fetch parameter -q
		{
			opt_waitForNew = true;
			opt_oldestFirst = false;
			num_q++;
		}
	}//end for

	//Set startdate/stopdate to current date if only time is set
	if((opt_startTime[0] == '-') || (opt_stopTime[0] == '-'))
	{
		char tempData[13] = "0";
		time_t t = time(0);
		struct tm tmp = *localtime(&t);
		char month[2] = "0";
		char day[2] = "0";

		if(tmp.tm_mon+1 > 9)
			sprintf(month, "%d", tmp.tm_mon+1);
		else
		{
			month[0] = '0';
			sprintf(&month[1], "%d", tmp.tm_mon+1);
		}

		if(tmp.tm_mday <= 9)
		{
			day[0] = '0';
			sprintf(&day[1], "%d", tmp.tm_mday);
		}
		else
			sprintf(day, "%d", tmp.tm_mday);

		if(opt_startTime[0] == '-')
		{
			sprintf(tempData, "%d%s%s%s", (tmp.tm_year+1900), month, day, opt_startTime.c_str());
			opt_startTime = tempData;
		}

		if(opt_stopTime[0] == '-')
		{
			sprintf(tempData, "%d%s%s%s", (tmp.tm_year+1900), month, day, opt_stopTime.c_str());
			opt_stopTime = tempData;
		}
	}

	//Check usage of options
	//Same option may not be used more than once
	if((num_a > 1) || (num_c > 1) || (num_t > 1) || (num_u > 1) || (num_x > 1) || (num_y > 1) || (num_l > 1) || (num_m > 1) || (num_p > 1) || (num_q > 1))
	{
		fprintf(stderr, "ERROR: Incorrect usage\n\n");
		usage();
		return 2;
	}

	//Check DateTime Options
	//Start 2.1
	if(num_t == 1 || num_u == 1) //YYYYMMDD-HHNN
	{
		// Check format
		if(tempCopy_start.length() == 8 || tempCopy_start.length() == 13)
		{
			if ((strcmp(opt_startTime.substr(0,4).c_str(),"0000") < 0 || strcmp(opt_startTime.substr(0,4).c_str(),"9999") > 0) ||
					(strcmp(opt_startTime.substr(4,2).c_str(),"01") < 0 || strcmp(opt_startTime.substr(4,2).c_str(),"12") > 0) ||
					(strcmp(opt_startTime.substr(6,2).c_str(),"01") < 0 || strcmp(opt_startTime.substr(6,2).c_str(),"31") > 0))
			{
				fprintf(stderr, "ERROR: startdate %s\n\n",opt_startTime.substr(0,8).c_str());
				return 5;
			}
			else if((strcmp(opt_startTime.substr(9,2).c_str(),"00") < 0 || strcmp(opt_startTime.substr(9,2).c_str(),"23") > 0) ||
					(strcmp(opt_startTime.substr(11,2).c_str(),"00") < 0 || strcmp(opt_startTime.substr(11,2).c_str(),"59") > 0))
			{
				fprintf(stderr, "ERROR: starttime %s\n\n",opt_startTime.substr(9,12).c_str());
				return 7;
			}
		}
		else if(tempCopy_start.length() == 6 || tempCopy_start.length() == 11)
		{
			if((strcmp(opt_startTime.substr(0,4).c_str(),"0000") < 0 || strcmp(opt_startTime.substr(0,4).c_str(),"9999") > 0) ||
					(strcmp(opt_startTime.substr(4,2).c_str(),"01") < 0 || strcmp(opt_startTime.substr(4,2).c_str(),"12") > 0) ||
					(strcmp(opt_startTime.substr(6,2).c_str(),"01") < 0 || strcmp(opt_startTime.substr(6,2).c_str(),"31") > 0))
			{
				fprintf(stderr, "ERROR: startdate %s\n\n",opt_startTime.substr(2,6).c_str());
				return 5;
			}
			else if((strcmp(opt_startTime.substr(9,2).c_str(),"00") < 0 || strcmp(opt_startTime.substr(9,2).c_str(),"23") > 0) ||
					(strcmp(opt_startTime.substr(11,2).c_str(),"00") < 0 || strcmp(opt_startTime.substr(11,2).c_str(),"59") > 0))
			{
				fprintf(stderr, "ERROR: starttime %s\n\n", opt_startTime.substr(9,12).c_str());
				return 7;
			}
		}//else if
	}//end if

	// 2.2 Stop
	if (num_x == 1 || num_y == 1) //YYYYMMDD-HHNN
	{
		// Check format
		if(tempCopy_stop.length() == 8 || tempCopy_stop.length() == 13)
		{
			if((strcmp(opt_stopTime.substr(0,4).c_str(),"0000") < 0 || strcmp(opt_stopTime.substr(0,4).c_str(),"9999") > 0) ||
					(strcmp(opt_stopTime.substr(4,2).c_str(),"01") < 0 || strcmp(opt_stopTime.substr(4,2).c_str(),"12") > 0) ||
					(strcmp(opt_stopTime.substr(6,2).c_str(),"01") < 0 || strcmp(opt_stopTime.substr(6,2).c_str(),"31") > 0))
			{
				fprintf(stderr, "ERROR: stopdate %s\n\n",opt_stopTime.substr(0,8).c_str());
				return 9;
			}
			else if((strcmp(opt_stopTime.substr(9,2).c_str(),"00") < 0 || strcmp(opt_stopTime.substr(9,2).c_str(),"23") > 0) ||
					(strcmp(opt_stopTime.substr(11,2).c_str(),"00") < 0 || strcmp(opt_stopTime.substr(11,2).c_str(),"59") > 0))
			{
				fprintf(stderr, "ERROR: stoptime %s\n\n",opt_stopTime.substr(9,12).c_str());
				return 10;
			}
		}
		else if(tempCopy_stop.length() == 6 || tempCopy_stop.length() == 11)
		{
			if((strcmp(opt_stopTime.substr(0,4).c_str(),"0000") < 0 || strcmp(opt_stopTime.substr(0,4).c_str(),"9999") > 0) ||
					(strcmp(opt_stopTime.substr(4,2).c_str(),"01") < 0 || strcmp(opt_stopTime.substr(4,2).c_str(),"12") > 0) ||
					(strcmp(opt_stopTime.substr(6,2).c_str(),"01") < 0 || strcmp(opt_stopTime.substr(6,2).c_str(),"31") > 0))
			{
				fprintf(stderr, "ERROR: stopdate %s\n\n",opt_stopTime.substr(2,6).c_str());
				return 9;
			}
			else if((strcmp(opt_stopTime.substr(9,2).c_str(),"00") < 0 || strcmp(opt_stopTime.substr(9,2).c_str(),"23") > 0) ||
					(strcmp(opt_stopTime.substr(11,2).c_str(),"00") < 0 || strcmp(opt_stopTime.substr(11,2).c_str(),"59") > 0))
			{
				fprintf(stderr, "ERROR: stoptime %s\n\n", opt_stopTime.substr(9,12).c_str());
				return 10;
			}
		}

		// Sanity check on options
		if((opt_startTime.length() > 1) && (opt_stopTime.length() > 1) &&
				(strcmp(opt_startTime.substr(0,8).c_str(), opt_stopTime.substr(0,8).c_str()) == 0) &&
				(strcmp(opt_startTime.c_str(), opt_stopTime.c_str()) > 0))
		{
			fprintf(stderr, "ERROR: starttime greater than stoptime \n\n");
			return 8;
		}

		if((opt_startTime.length() > 1) && (opt_stopTime.length() > 1) && ( strcmp(opt_startTime.c_str(),opt_stopTime.c_str()) > 0))
		{
			fprintf(stderr, "ERROR: startdate greater than stopdate\n\n");
			return 6;
		}
	}

	// Compose the path of event log file
	char nodeName[32] = {0};
	string pathEventLog("/var/log/");

	//nodeName = getenv("HOST");
	gethostname( nodeName, 32);
	if( nodeName == 0)
	{
		fprintf(stderr, "ERROR: Failed to open EventLog\n\n");
		return 1;
	}

	pathEventLog.append(nodeName);
	pathEventLog.append("/messages");

	string line;
	string recordOutput;
	size_t found;

	if(opt_waitForNew)		//  Option -q specified
	{
		char buff[1000] = {0};
		FILE *file;
		file = fopen(pathEventLog.c_str(), "r");

		if( file == 0){
			fprintf(stderr, "ERROR: Failed to open EventLog\n\n");
			return 1;
		}

		fseek(file, 0, SEEK_END);

		while(numOfRecord < opt_numberOfRecords)
		{
			if((fgets(buff, MAX_NUMBER_CHAR_TO_READ, file)) == 0)
				sleep(1);
			else
			{
				line = buff;

				if(opt_logtype.compare("app") == 0)
				{
					found = line.find("APEvent");
					if((found != string::npos) && ((int)found == 20))
					{
						recordOutput = "";
						if(parseAPEventMessage(line, recordOutput))
						{
							cout << recordOutput;
						}
					}
				}
				else if(opt_logtype.compare("sys") == 0)
				{
					found = line.find("SystemEvent");
					if((found != string::npos) && ((int)found == 20))
					{
						recordOutput = "";
						if(parseSSEventMessage(line, recordOutput))
						{
							cout << recordOutput;
						}
					}
				}
				else if(opt_logtype.compare("sec") == 0)
				{
					found = line.find("SecurityEvent");
					if((found != string::npos) && ((int)found == 20))
					{
						recordOutput = "";
						if(parseSSEventMessage(line, recordOutput))
						{
							cout << recordOutput;
						}
					}
				}
				else {

					found = line.find("APEvent");
					if((found != string::npos) && ((int)found == 20)){
						recordOutput = "";
						if(parseAPEventMessage(line, recordOutput))
						{
							//Tr HR34276 - START
							//recordOutputs.push_back(recordOutput);
							cout << recordOutput;
							//Tr HR34276 - END
						}
					}
					else{
						found = line.find("SystemEvent");
						if((found != string::npos) && ((int)found == 20)){
							recordOutput = "";
							if(parseSSEventMessage(line, recordOutput))
							{
								//Tr HR34276 - START
								//recordOutputs.push_back(recordOutput);
								cout << recordOutput;
								//Tr HR34276 - END
							}
						}
						else {
							found = line.find("SecurityEvent");
							if((found != string::npos) && ((int)found == 20)){
								recordOutput = "";
								if(parseSSEventMessage(line, recordOutput))
								{
									//Tr HR34276 - START
									//recordOutputs.push_back(recordOutput);
									cout << recordOutput;
									//Tr HR34276 - END
								}
							}
						}

					}
				}
			}
		}
		fclose(file);
	}
	else			//  option -q not specified
	{
		char   version;

		for (int j=0; j<=9; j++)
		{
			switch (j)
			{
				case 0  :  	break;
				case 1  :  	pathEventLog.append(".1");
						break;
				default :  	version = '0' + j;
						pathEventLog[pathEventLog.length()-1] = version;
						break;
			}

			fstream  file(pathEventLog.c_str(), ios::in);
			if (! file.is_open())
			{
				if (j == 0)
				{
					fprintf(stderr, "ERROR: Failed to open EventLog\n\n");
					return 1;
				}
				break;
			}

			if (j == 0)  cout << endl;

			while(file.good())
			{
				getline(file, line);

				if(opt_logtype.compare("app") == 0)
				{
					found = line.find("APEvent");
					if((found != string::npos) && ((int)found == 20))
					{
						recordOutput = "";
						if(parseAPEventMessage(line, recordOutput))
						{
							recordOutputs.push_back(recordOutput);
						}
					}
				}
				else if(opt_logtype.compare("sys") == 0)
				{
					found = line.find("SystemEvent");
					if((found != string::npos) && ((int)found == 20))
					{
						recordOutput = "";
						if(parseSSEventMessage(line, recordOutput))
						{
							recordOutputs.push_back(recordOutput);
						}
					}
				}
				else if(opt_logtype.compare("sec") == 0)
				{
					found = line.find("SecurityEvent");
					if((found != string::npos) && ((int)found == 20))
					{
						recordOutput = "";
						if(parseSSEventMessage(line, recordOutput))
						{
							recordOutputs.push_back(recordOutput);
						}
					}
				}
				else {
					found = line.find("APEvent");
					if((found != string::npos) && ((int)found == 20)){
						recordOutput = "";
						if(parseAPEventMessage(line, recordOutput))
						{
							recordOutputs.push_back(recordOutput);
						}
					}
					else{
						found = line.find("SystemEvent");
						if((found != string::npos) && ((int)found == 20)){
							recordOutput = "";
							if(parseSSEventMessage(line, recordOutput))
							{
								recordOutputs.push_back(recordOutput);
							}
						}
						else {
							found = line.find("SecurityEvent");
							if((found != string::npos) && ((int)found == 20)){
								recordOutput = "";
								if(parseSSEventMessage(line, recordOutput))
								{
									recordOutputs.push_back(recordOutput);
								}
							}
						}

					}
				}
			}
			file.close();
		}

		//sort the list
		recordOutputs.sort();

		if(opt_oldestFirst)
		{
			int startPoint = 0;

			if(numOfRecord > opt_numberOfRecords)
			{
				startPoint = recordOutputs.size() - opt_numberOfRecords;
			}

			std::list<std::string>::iterator outputs;
			int i = 0;

			for(outputs = recordOutputs.begin(); outputs != recordOutputs.end(); outputs++)
			{
				if(i >= startPoint)  cout << *(outputs);

				i++;
			}

			if((int)recordOutputs.size() > opt_numberOfRecords)
			{
				cout << "\nMore events to show\n\n";
			}
		}
		else
		{
			std::list<std::string>::iterator outputs;

			recordOutputs.reverse();

			int i = 1;
			for(outputs = recordOutputs.begin(); outputs != recordOutputs.end(); outputs++)
			{
				cout << *(outputs);
				i++;

				if (i > opt_numberOfRecords)
				{
					if ((int)recordOutputs.size() > opt_numberOfRecords)   //  HO92774
					{
						cout << "\nMore events to show\n\n";
					}
					break;
				}
			}
		}
	}

	return 0;
}
