/*=================================================================== */
/**
   @file acs_alog_drainer.cpp

   Class method implementationn for eventParser type module.

   This module contains the implementation of class declared in
   the ACS_ALOG_Drainer.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/02/2011     xgencol/xgaeerr       Initial Release
  =================================================================== */

/*===================================================================

////////////////////////////////////////////////////////
// 	type declaration for return type
///////////////////////////////////////////////////////

                        INCLUDE DECLARATION SECTION
=================================================================== */
//Include file for external library.
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

//Mandatory.

#include "acs_alog_drainer.h"

#define  EVENT_MAX_SIZE      3145728

using namespace std;

static const std::string dateTimeFormat = "%Y-%m-%d %H%M%S";

const char delimiter_[] = ",, ";		// Used to compose output string

const char end_[] = { 255 };


/*===================================================================
   ROUTINE: logData
=================================================================== */

acs_alog_returnType acs_alog_Drainer::logData ( const ACS_ALOG_DataUnitType type,
												const char * data,
												const char * remoteHost,
												const char * remoteUser,
												const int dataLength,
												const char * logDate,
												const char * logTime,
												const char * applicationName,
												const char * success,
												const char * pid,
												const char * device,
												const char * sessId,
												const char * cmdId,
												const char* CPnameGroup,
												const int* CPIDlist,
												const int CPIDlistLength,
												const unsigned cmdSeqNum )
{

	return logData(type,data,remoteHost,remoteUser,dataLength,logDate,logTime,applicationName,success,pid,device,sessId,cmdId,CPnameGroup,CPIDlist,CPIDlistLength,cmdSeqNum,-1,"");
}

/*===================================================================
   ROUTINE: logData
=================================================================== */

acs_alog_returnType acs_alog_Drainer::logData ( const ACS_ALOG_DataUnitType type,
												const char * data,
												const char * remoteHost,
												const char * remoteUser,
												const int dataLength,
												const char * logDate,
												const char * logTime,
												const char * applicationName,
												const char * success,
												const char * pid,
												const char * device,
												const char * sessId,
												const char * cmdId,
												const char* CPnameGroup,
												const int* CPIDlist,
												const int CPIDlistLength,
												const unsigned cmdSeqNum,
												const int prca,
												const char * localHostInfo )
{

	/* ====== Internal Function declaration =======*/
	string convertTypeToString (const ACS_ALOG_DataUnitType type);
	void coverPasswords (char data[], long lenData);
	string returnNowTime ();
	string returnNowDate ();

	/* ====== Internal declaration =======*/
	acs_alog_communicationhandler logevent;
	string messagePreparation;
	acs_alog_returnType returnValue = acs_alog_ok;
	string logDateNow="";
	string logTimeNow="";
	string voidCode_;
	string _cpidList;

	if ((data == NULL) || (remoteHost == NULL) || (remoteUser == NULL) || (logDate == NULL) || (logTime == NULL) || (applicationName == NULL)
			|| (success == NULL) || (pid == NULL) || (device == NULL) || (sessId == NULL) || (cmdId == NULL) || (CPnameGroup == NULL))
	{
		returnValue	= acs_alog_generic_error;
		return returnValue;
	}

	if((strcmp(logDate,voidChar_)!=0) && (strcmp(logTime,voidChar_) != 0))
	{
			if (checkDateTime(logDate,logTime) == -1)  returnValue = acs_alog_date_format_error;
	}
	else {
			logDateNow = returnNowDate();
			logTimeNow = returnNowTime();
	}

	voidCode_.append(success);

	stringstream  sscpid;

	for (int i=0;i<CPIDlistLength;i++)
	{
			sscpid.str("");
			sscpid << CPIDlist[i];

			if (i == 0)  _cpidList += sscpid.str();
			else	   	 _cpidList += cpidlistSeparator_  + sscpid.str() ;
	}

	if (! returnValue)
	{
		messagePreparation.append(tag_);
		messagePreparation.append(colon_);
		messagePreparation.append(logdata_);

		if (strcmp(logDate,voidChar_) != 0)  messagePreparation.append(logDate);
		else								 messagePreparation.append(logDateNow);
		messagePreparation.append(delimiter_);

		messagePreparation.append(logtime_);

		if (strcmp(logTime,voidChar_) != 0)  messagePreparation.append(logTime);
		else								 messagePreparation.append(logTimeNow);
		messagePreparation.append(delimiter_);

		messagePreparation.append(type_);
		messagePreparation.append(convertTypeToString(type));
		messagePreparation.append(delimiter_);

		messagePreparation.append(user_);

		if (strcmp(remoteUser,voidChar_) != 0)  messagePreparation.append(remoteUser);
		else {
				if(getenv("USER") != NULL)  messagePreparation.append(getenv("USER"));
				else						messagePreparation.append("");
		}
		messagePreparation.append(delimiter_);

		messagePreparation.append(device_);
		messagePreparation.append(device);
		messagePreparation.append(delimiter_);

		messagePreparation.append(block_);
		messagePreparation.append(applicationName);
		messagePreparation.append(delimiter_);

		messagePreparation.append(pid_);
		messagePreparation.append(pid);
		messagePreparation.append(delimiter_);

		messagePreparation.append(remotehost_);
		messagePreparation.append(remoteHost);
		messagePreparation.append(delimiter_);

		messagePreparation.append(sessionid_);
		messagePreparation.append(sessId);
		messagePreparation.append(delimiter_);

		messagePreparation.append(cmdid_);
		messagePreparation.append(cmdId);
		messagePreparation.append(delimiter_);

		messagePreparation.append(cpnamegroup_);
		messagePreparation.append(CPnameGroup);
		messagePreparation.append(delimiter_);

		messagePreparation.append(cpidlist_);
		messagePreparation.append(_cpidList);
		messagePreparation.append(delimiter_);

		messagePreparation.append(cmdseqnum_);
		std::stringstream sscmdSeqNum;
		sscmdSeqNum << cmdSeqNum;
		messagePreparation.append(sscmdSeqNum.str());
		messagePreparation.append(delimiter_);

		messagePreparation.append(prca_);
		if (prca != -1)
		{
				std::stringstream ssprca;
				ssprca << prca;
				messagePreparation.append(ssprca.str());
		}
		messagePreparation.append(delimiter_);

		messagePreparation.append(localhostInfo_);
		messagePreparation.append(localHostInfo);
		messagePreparation.append(delimiter_);

		messagePreparation.append(cmd_);

//		char  myData[EVENT_MAX_SIZE + 1];        // ---------   E V E N T   P A R S I N G   -----------
//		bool  eventTooLarge = false;
//		register long  dimData, pos, idx;
//		register char  car;
//
//		if (dataLength > EVENT_MAX_SIZE)
//		{
//				dimData = EVENT_MAX_SIZE - 2;
//				eventTooLarge = true;
//		}
//		else  	dimData = dataLength;
//
//		pos = idx = 0;
//
//		while (pos < dimData)
//		{
//				if ((car = data[pos]) != '\r')
//				{
//						myData[idx++] = car;
//				}
//				pos ++;
//		}
//		if (eventTooLarge == true)  myData[idx++] = '\n';
//		myData[idx] = '\0';
//
//		long  lenData = (long)strlen(myData);
//
//		coverPasswords (myData, lenData);		 //  --------------------------------------------------

//		messagePreparation.append(myData, lenData);

		messagePreparation.append(data, dataLength);

		messagePreparation.append(delimiter_);

		messagePreparation.append(size_);
		stringstream  ss;
		ss << dataLength;
		messagePreparation.append(ss.str());
		messagePreparation.append(delimiter_);

		messagePreparation.append(end_);
		messagePreparation.append(newline_);

//		if (logevent.open() == -1)
//			return logevent.getErrorCode();

		if (logevent.sendMsg(messagePreparation.c_str()) == 0)
			returnValue = acs_alog_ok;
		else
			returnValue = logevent.getErrorCode();

//		logevent.close();
	}

	return returnValue;
}

/*===================================================================
   ROUTINE: newLoggingSessionint *intVoid;
=================================================================== */
void acs_alog_Drainer::newLoggingSession()
{
		acs_alog_queue = std::queue<const char *>();
}

/*===================================================================
   ROUTINE: appendData
=================================================================== */
void acs_alog_Drainer::appendData (const char *data, const int dataLength)
{
		char  *info = new char[dataLength+1];

		memcpy (info, data, dataLength);
		info[dataLength] = '\0';

		acs_alog_queue.push(info);
}

/*===================================================================
   ROUTINE: commit
=================================================================== */
acs_alog_returnType acs_alog_Drainer::commit ( const ACS_ALOG_DataUnitType type,
											   const char * remoteHost,
											   const char * remoteUser,
											   const char * logDate,
											   const char * logTime,
											   const char * applicationName,
											   const char * success,
											   const char * pid,
											   const char * device,
											   const char * sessId,
											   const char * cmdId,
											   const char* CPnameGroup,
											   const int* CPIDlist,
											   const int CPIDlistLength,
											   const unsigned cmdSeqNum )
{
	return commit(type,remoteHost,remoteUser,logDate,logTime,applicationName,success,pid,device,sessId,cmdId,CPnameGroup,CPIDlist,CPIDlistLength,cmdSeqNum,-1,"");
}

/*===================================================================
   ROUTINE: commit
=================================================================== */
acs_alog_returnType acs_alog_Drainer::commit ( const ACS_ALOG_DataUnitType type,
											   const char * remoteHost,
											   const char * remoteUser,
											   const char * logDate,
											   const char * logTime,
											   const char * applicationName,
											   const char * success,
											   const char * pid,
											   const char * device,
											   const char * sessId,
											   const char * cmdId,
											   const char* CPnameGroup,
											   const int* CPIDlist,
											   const int CPIDlistLength,
											   const unsigned cmdSeqNum,
											   const int prca,
											   const char * localHostInfo )
{

	acs_alog_returnType  returnValue = acs_alog_ok;

	if ((remoteHost == NULL) || (remoteUser == NULL) || (logDate == NULL) || (logTime == NULL) || (applicationName == NULL)
			|| (success == NULL) || (pid == NULL) || (device == NULL) || (sessId == NULL) || (cmdId == NULL) || (CPnameGroup == NULL))
	{
			returnValue	= acs_alog_generic_error;
			return returnValue;
	}

	std::string  dataInfo = "";

	while (! acs_alog_queue.empty())
	{
			dataInfo.append (acs_alog_queue.front());
			delete [] acs_alog_queue.front();

			acs_alog_queue.pop();
	}

	returnValue = logData (type, dataInfo.c_str(), remoteHost, remoteUser, dataInfo.size(),
			               logDate, logTime, applicationName, success, pid, device, sessId,
			               cmdId, CPnameGroup, CPIDlist, CPIDlistLength, cmdSeqNum, prca, localHostInfo);
	return returnValue;
}

/*===================================================================
   ROUTINE: rollback
=================================================================== */
acs_alog_returnType acs_alog_Drainer::rollback()
{

		while (! acs_alog_queue.empty())
		{
				delete [] acs_alog_queue.front();
				acs_alog_queue.pop();
		}

		return acs_alog_ok;
}

/*===================================================================
   ROUTINE: checkDateTime
=================================================================== */
int checkDateTime(std::string data, std::string time)
{
	std::string tempDateTime = data + time;
	tm nowTime;

	if (strptime(tempDateTime.c_str(),dateTimeFormat.c_str(),&nowTime) != NULL)  return 0;

	return -1;
}

/*===================================================================
   ROUTINE: returnNowDate
=================================================================== */
std::string returnNowDate()
{
	//start date default value to 10 years previus now
	time_t t;
	time(&t);
	struct tm* lt = localtime(&t);
	char temp[10];
	sprintf(temp,"%04d-%02d-%02d",lt->tm_year + 1900,lt->tm_mon +1,lt->tm_mday);

	return (std::string)temp;
}

/*===================================================================
   ROUTINE: returnNowTime
=================================================================== */
std::string returnNowTime()
{
	//start date default value to 10 years previus now
	time_t t;
	time(&t);
	struct tm* lt = localtime(&t);
	char temp[10];
	sprintf(temp,"%02d%02d%02d",lt->tm_hour,lt->tm_min,lt->tm_sec);
	return (std::string) temp;
}

/*===================================================================
   ROUTINE: coverPasswords
=================================================================== */
void  coverPasswords (char data[], long lenData)
{

		int  crCounter = 0;

		for (long j=0; j<lenData-6; j++)
		{
				if ((unsigned char)data[j] == '\n')          //  the check is executed only on the first lines
				{
						crCounter ++;
						if (crCounter >= 5)  return;
				}
				if (strncasecmp(&data[j], "PSW", 3) == 0  ||  strncasecmp(&data[j], "PWD", 3) == 0)
				{
						j = j + 3;

						while (data[j] == ' ')  j ++;
						if (data[j] == '=')  j ++;

						while (data[j] != ','  &&  data[j] != ';'  &&  (unsigned char)data[j] != '\n')
						{
								data[j] = '*';
								j ++;
						}
				}
		}
}

/*===================================================================
   ROUTINE: convertTypeToString
=================================================================== */
std::string convertTypeToString(const ACS_ALOG_DataUnitType type)
{
	std::string returnValue;
	/*
		not_defined 	   		= 0,  // Anything
MC 	=>	MMLcmd 		       		= 3,  // MML command
MP	=>	MMLprt 		       		= 4,  // MML printout
MPA	=>	MMLcmdpar 	       		= 5,  // MML command parameter
APD	=>	ApplData 	       		= 6,  // Application data
ALP	=>	MMLalp 		       		= 7,  // MML alarm printout
NC	=>	Unixcmd 	       		= 8,  // Unix command/process
NP	=>	Unixprt 	       		= 9,  // Unix printout
SEC	=>	SecLogData 	       		= 10, // Security log data
MCI	=>	MMLCmdInconsistent    	= 11, // MML command causing inconsistency
MCL	=>	MMLCmdLog             	= 12, // MML command for logging
MLI	=>	MMLCmdLogInconsistent	= 13, // MML command for logging, with inconsistency
MPI	=>	MMLPrtInconsistent   	= 14,  // Result printout indicating inconsistency
MIC	=>	MMLInternalCommand   	= 15  // FCCML and FCCMP
	 */

	//syslog(LOG_INFO,"convertTypetoSTring");
	std::stringstream ss;

	switch(type)
	{
	case 0  :  returnValue = "undefined";
			   break;
	case 3  :  returnValue = "mc";
			   break;
	case 4  :  returnValue = "mp";
			   break;
	case 5  :  returnValue = "mpa";
			   break;
	case 6  :  returnValue = "apd";
			   break;
	case 7  :  returnValue = "alp";
			   break;
	case 8  :  returnValue = "nc";
			   break;
	case 9  :  returnValue = "np";
			   break;
	case 10 :  returnValue = "sec";
			   break;
	case 11 :  returnValue = "mci";
			   break;
	case 12 :  returnValue = "mcl";
			   break;
	case 13 :  returnValue = "mli";
			   break;
	case 14 :  returnValue = "mpi";
			   break;
	case 15 :
		returnValue = "mmc";
			   break;
	default :
		syslog(LOG_INFO,"default is the type");
		ss << type;
			   returnValue = ss.str();
	};

	//syslog(LOG_INFO,returnValue.c_str());
	return returnValue;
}
