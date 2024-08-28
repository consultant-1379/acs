//******************************************************************************
//
//  NAME
//     ACS_ALOG_Lister.cpp
//
//  COPYRIGHT
//     Ericsson Utvecklings AB, Sweden 2001. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson Utvecklings AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA 109 0xxx
//
//  AUTHOR
//     2001-05-09 by UAB/S/GM UABCHSN
//
//  SEE ALSO
//     -
//
//******************************************************************************

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <sys/types.h>
#include <acs_alog_Lister.h>
#include <acs_alog_cmdUtil.h>
#include <ace/ACE.h>

using namespace std;

#define EMPTYMINUS 	string("-")
#define EMPTY 		string("")

#define FILES_TIMESTAMP_LEN		12


char  AbsPAuditPath[300];				//  contain the /data path for protected_logs


void  getDateTimeFromFname (const char *fName, char fdate[])			//  return in 'fdate' the Time Stamp in the format  YYYYMMDDhhmm
{

		string  fName_str(fName);
		size_t  found = fName_str.find_first_of("_.-");
		string  fdate_str, tempstr;

		tempstr = fName_str.substr(found+1);

		while ((found = tempstr.find_first_of("_-.")) != string::npos  &&  found < tempstr.length()-1)
		{
				fdate_str.append (tempstr.c_str(),found);
				tempstr = tempstr.substr (found+1);
		}

		strncpy (fdate, fdate_str.c_str(), FILES_TIMESTAMP_LEN);
}


bool  compareFilename (string fName1, string fName2)   			//  return TRUE if fName1 Time Stamp is less than fName2 one
{

		char  fdate1[FILES_TIMESTAMP_LEN+1]= {0}, fdate2[FILES_TIMESTAMP_LEN+1]= {0};

		getDateTimeFromFname (fName1.c_str(), fdate1);
		getDateTimeFromFname (fName2.c_str(), fdate2);

		int  ret;
		if ((ret = strcmp (fdate1, fdate2)) != 0)  return (ret < 0);

		struct stat  status1, status2;						//  When the Time Stamps are equal, the last modification times are evaluated
		string       pathAbs;								//
															//  This situation is verified when the audit.... and LOGFILE-... have the same
		pathAbs.assign (AbsPAuditPath);						//  year, the same month, the same day, the same hour and the same minute
		pathAbs = pathAbs + "/" + fName1;
		stat (pathAbs.c_str(), &status1);

		pathAbs.assign (AbsPAuditPath);
		pathAbs = pathAbs + "/" + fName2;
		stat (pathAbs.c_str(), &status2);

		return (status1.st_mtim.tv_sec < status2.st_mtim.tv_sec) ? true : false;
}


class ACS_ALOG_Ls
{
	public:
		ACS_ALOG_Ls();
		~ACS_ALOG_Ls();

	public:
		Status  GetStatus();
		const ACE_TCHAR*  GetFileDest();
		long  GetMaximumFileSize();
		long  GetCurrentFileSize();
		ACE_TCHAR*  GetOldestLogfile();
		ACE_TCHAR*  GetNewestLogfile();
		int  GetRetCode();
		bool  GetIdTag(); // new 0512
		int  GetLogRecordTimes(std::string &firstRecordTime,std::string &lastRecordTime,std::string &OldFile,std::string &NewFile);
		void  makeFileListZIP();
//		void  getDateTimeFromFname (const char *fName, char fdate[]);
//		bool  compareFilename (string fName1, string fName2);

	private:
		int  m_nRetCode;       // Return code from save attempt
		ACS_ALOG_AttrFileRecord  m_AttrFileRecord;
		long  m_nCurrentFileSize;

		std::list<string> loggingFiles;
		std::list<std::string>::iterator  files;

		ACE_TCHAR m_szNewestLogfile[32+1];
		ACE_TCHAR m_szOldestLogfile[32+1];
		ACE_TCHAR m_szLogfilePath[260 + 1];
};


ACS_ALOG_Ls::ACS_ALOG_Ls()
{
	m_nRetCode = 0;
	m_nCurrentFileSize = 0;
	long nFileSize = 0;
	*m_szNewestLogfile = *m_szOldestLogfile = NULL;
	*m_szLogfilePath = 0;

	ACE_HANDLE hFile;

	string lpszFile;

	acs_alog_cmdUtil::getAlogDataDiskPath(lpszFile);
	lpszFile += "/ACS_ALOG/ATTRFILEP";

	ACE_stat statBuff;
	if( ACE_OS::stat(lpszFile.c_str(), &statBuff)  != 0 )  m_nRetCode = 1;

	if(statBuff.st_size != 0)
	{
		hFile= ACE_OS::open(lpszFile.c_str(),O_RDONLY);

		if (hFile == ACE_INVALID_HANDLE)  m_nRetCode = 1;
		else {
				int dwBytesRead;
				dwBytesRead = ACE_OS::read(hFile,(void*)(&m_AttrFileRecord),sizeof(ACS_ALOG_AttrFileRecord));

				ACE_OS::close(hFile);

				if (!dwBytesRead)  m_nRetCode = 1;
		}
	}
	else {
			(*m_AttrFileRecord.FileDest) = NULL;
			m_AttrFileRecord.IdTag = false;
	}

	char  pathAudit[300];
	char  rootPath[15];
	int   lenPath = 300;
	strcpy (rootPath, "auditLog");
	ACS_APGCC_CommonLib  apgccLib;

	if (apgccLib.GetFileMPath (rootPath, pathAudit, lenPath) != ACS_APGCC_DNFPATH_SUCCESS)
	{
			fprintf(stderr, "ALOGLIST, finding the audit logs path failed !\n");
			return;
	}

	strcpy (AbsPAuditPath, pathAudit);
	strcat (AbsPAuditPath , "/protected_logs/");

	DIR  *handle;
	struct dirent  *entry;
	string  fName, pathAbs;
	struct stat  fileStatus;

	if ((handle = opendir (AbsPAuditPath)) == NULL)
	{
			m_nRetCode = 1;
			return;
	}
	loggingFiles.clear();

	while ((entry = readdir(handle)) != NULL)
	{
		fName = entry->d_name;
		if (fName.substr(0,7).compare("paudit.") == 0  ||  fName.substr(0,9).compare("PLOGFILE_") == 0)
		{
				pathAbs.assign(AbsPAuditPath);
				pathAbs = pathAbs + fName;
				stat (pathAbs.c_str(), &fileStatus);
				nFileSize = nFileSize + fileStatus.st_size;

				loggingFiles.push_back (fName);
		}
	}
	closedir (handle);

	loggingFiles.sort (compareFilename);

	if (nFileSize > 0)
	{
		m_nCurrentFileSize = (long)(nFileSize / 1048576);
		long nRes = (nFileSize % 1048576);
		if (nRes > 0)
		{
				char szRes[32];
				sprintf(szRes, ("%ld"), nRes);
				if ( (strlen(szRes)>6) || ((strlen(szRes)>5) && (*szRes >= ('5'))))  m_nCurrentFileSize++;
		}
	}
	else  m_nCurrentFileSize = 0;
}


void ACS_ALOG_Ls::makeFileListZIP(void)
{
}


ACS_ALOG_Ls::~ACS_ALOG_Ls()
{
		loggingFiles.clear();
}


Status ACS_ALOG_Ls::GetStatus()
{
	return m_AttrFileRecord.status;
}


const ACE_TCHAR* ACS_ALOG_Ls::GetFileDest()
{
	return m_AttrFileRecord.FileDest;
}


long ACS_ALOG_Ls::GetMaximumFileSize()
{
	return m_AttrFileRecord.FileSize;
}


long ACS_ALOG_Ls::GetCurrentFileSize()
{
	return m_nCurrentFileSize;
}


ACE_TCHAR* ACS_ALOG_Ls::GetOldestLogfile()
{

	if (! loggingFiles.empty())
	{
			string  oldFile = loggingFiles.front();
			ACE_OS::strcpy (m_szOldestLogfile, oldFile.c_str());
	}
	else {
			ACE_OS::strcpy(m_szOldestLogfile, EMPTYMINUS.c_str());
			m_nRetCode = 2;
	}
	return m_szOldestLogfile;
}


ACE_TCHAR* ACS_ALOG_Ls::GetNewestLogfile()
{
	m_nRetCode = 0;

	if (! loggingFiles.empty())
	{
			string  newFile = loggingFiles.back();
			ACE_OS::strcpy(m_szNewestLogfile,newFile.c_str());
	}
	else {
			ACE_OS::strcpy(m_szNewestLogfile,EMPTY.c_str());
			m_nRetCode = 2;
	}
	return m_szNewestLogfile;
}


int ACS_ALOG_Ls::GetRetCode()
{
		return m_nRetCode;
}


bool ACS_ALOG_Ls::GetIdTag()
{
	return m_AttrFileRecord.IdTag;  // new 0512
}


int ACS_ALOG_Ls::GetLogRecordTimes(std::string& firstRecordtime,std::string& lastRecordTime,std::string& OldFile,std::string& NewFile)
{
	char*   line = NULL;
	string  strLine("");
	string  strSizeOfRecord("");
	string  strLogEvent("");
	size_t  posFin = 0;
	size_t  posStop = 0;
	size_t  len =0;
	ssize_t read;
	FILE*   fp;

	string cmdString;
	string  oldeProtectedFile, nweProtectedFile, tmpOldProtectedFile, tmpNewProtectedFile;
	string  pathAbs(AbsPAuditPath);

	oldeProtectedFile = pathAbs + GetOldestLogfile();
	nweProtectedFile  = pathAbs + GetNewestLogfile();

	tmpOldProtectedFile = "/tmp/oldTemp-aloglist";
	tmpNewProtectedFile = "/tmp/newTemp-aloglist";

	string pwdFor7zExtractionInteractive = acs_alog_cmdUtil::getPasswordInFile() + "\n";

	cmdString = "/usr/bin/7z e -o\"/tmp\" -y " + nweProtectedFile + " -so" + " 1> " + tmpNewProtectedFile + " 2>/dev/null";
	int childProcStatus = 0;
	acs_alog_cmdUtil::execute7zCmd(cmdString,pwdFor7zExtractionInteractive,childProcStatus);

	fp = fopen(tmpNewProtectedFile.c_str(), "r");
	if (! fp)  return 1;

	while ((read = getline(&line, &len, fp)) != -1)
	{
		strLine.assign(line);
		if (strLine.length() > 0)
		{
				posFin = strLine.find(";");
				if (posFin == string::npos  ||  posFin == 0)  continue;
				strSizeOfRecord = strLine.substr(0,posFin);
				if (acs_alog_cmdUtil::isNumeric (strSizeOfRecord.c_str()) == false)  continue;

				posStop = posFin;							// get the date and time from EVENT ROW
				posFin = strLine.find(";",posStop+1);
				lastRecordTime = strLine.substr(posStop+1,posFin-posStop-1);
				lastRecordTime += " ";
				posStop = posFin;
				posFin = strLine.find(";",posStop+1);
				lastRecordTime += strLine.substr(posStop+1, posFin-posStop-1);
		}
	}
	fclose(fp);
	unlink(tmpNewProtectedFile.c_str());

	cmdString = "/usr/bin/7z e -o\"/tmp\" -y " + nweProtectedFile + " -so" + " 1> " + tmpNewProtectedFile + " 2>/dev/null";
	childProcStatus = 0;
	acs_alog_cmdUtil::execute7zCmd(cmdString,pwdFor7zExtractionInteractive,childProcStatus);

	fp = fopen(tmpNewProtectedFile.c_str(), "r");
	if (! fp)  return 1;

	while (1)
	{
		if ((read = getline(&line, &len, fp)) != -1)
		{
			if (strcmp(line,"\n") != 0)
			{
					std::string p_lineString(line);
					size_t posFin = 0;
					posFin = p_lineString.find(";");
					if ( posFin != string::npos )
					{
							size_t posStop = 0;
							posFin = p_lineString.find(";");
							posStop = posFin;
							posFin = p_lineString.find(";",posStop+1);
							NewFile = p_lineString.substr(posStop+1,posFin-posStop-1);
							NewFile += " ";
							posStop = posFin;
							posFin = p_lineString.find(";",posStop+1);
							NewFile += p_lineString.substr(posStop+1, posFin-posStop-1);
							break;
					}
			}
		}
		else  break;
	}
	fclose(fp);
	unlink(tmpNewProtectedFile.c_str());

	cmdString = "/usr/bin/7z e -o\"/tmp\" -y " + oldeProtectedFile + " -so" + " 1> " + tmpOldProtectedFile + " 2>/dev/null";
	childProcStatus = 0;
	acs_alog_cmdUtil::execute7zCmd(cmdString,pwdFor7zExtractionInteractive,childProcStatus);

	fp = fopen(tmpOldProtectedFile.c_str(), "r");

	if (! fp)  return 1;

	while (1)
	{
		if((read = getline(&line, &len, fp)) != -1)
		{
			if (strcmp(line,"\n")!=0)
			{
				std::string p_lineString(line);
				size_t posFin = 0;
				posFin = p_lineString.find(";");
				if ( posFin != string::npos )
				{
						size_t posStop = 0;
						posFin = p_lineString.find(";");
						posStop = posFin;
						posFin = p_lineString.find(";",posStop+1);
						firstRecordtime = p_lineString.substr(posStop+1,posFin-posStop-1);
						firstRecordtime += " ";
						posStop = posFin;
						posFin = p_lineString.find(";",posStop+1);
						firstRecordtime += p_lineString.substr(posStop+1, posFin-posStop-1);
						OldFile = firstRecordtime;
						break;
				}
			}
		}
		else  break;
	}
	fclose(fp);
	unlink(tmpOldProtectedFile.c_str());

	return 0;
}


///////////////////////////////////////////////////////////////////////////////


void ACS_ALOG_Lister::makeFileListZIP()
{
	m_pLs->makeFileListZIP();
}

ACS_ALOG_Lister::ACS_ALOG_Lister()
{
	m_pLs = new ACS_ALOG_Ls();
}

ACS_ALOG_Lister::~ACS_ALOG_Lister()
{
	delete m_pLs;
}

Status ACS_ALOG_Lister::GetStatus()
{
	return m_pLs->GetStatus();
}

const ACE_TCHAR* ACS_ALOG_Lister::GetFileDest()
{
	return m_pLs->GetFileDest();
}

long ACS_ALOG_Lister::GetMaximumFileSize()
{
	return m_pLs->GetMaximumFileSize();
}

long ACS_ALOG_Lister::GetCurrentFileSize()
{
	return m_pLs->GetCurrentFileSize();
}

ACE_TCHAR* ACS_ALOG_Lister::GetOldestLogfile()
{
	return m_pLs->GetOldestLogfile();
}

ACE_TCHAR* ACS_ALOG_Lister::GetNewestLogfile()
{
	return m_pLs->GetNewestLogfile();
}

int ACS_ALOG_Lister::GetRetCode()
{
	return m_pLs->GetRetCode();
}

bool ACS_ALOG_Lister::GetIdTag()
{
	return m_pLs->GetIdTag();  // new 0512
}


int ACS_ALOG_Lister::GetLogRecordTimes(std::string& firstRecordTime, std::string& lastRecordTime,std::string& OldFile,std::string& NewFile)
{
		return m_pLs->GetLogRecordTimes(firstRecordTime,lastRecordTime,OldFile,NewFile);
}

