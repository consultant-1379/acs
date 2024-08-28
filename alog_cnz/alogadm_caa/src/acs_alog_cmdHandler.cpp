
/*=================================================================== */
/**
  @file acs_alog_cmdHandler.cpp

  Class method implementation for acs_alog_cmdHandler type module.

  This module contains the implementation of class declared in
  the acs_alog_cmdHandler.h module

  @version 2.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       02/02/2011     xgencol/xgaeerr       Initial Release
  N/A       07/08/2012     xbhakat               Final Release
  N/A       08/09/2015     xsansud               HT92230
  =================================================================== */

#include <acs_alog_cmdHandler.h>
#include <ACS_APGCC_CommonLib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <acs_alog_utility.h>

extern multiset<string>typecmdnc;
extern multiset<string>typecmdmc;

extern multiset<string>typecmdmpa;
extern multiset<string>typecmdmp;
extern multiset<string>typecmdnp;
extern string  protectedPassword;
extern string  FilePAudit;
extern string  FilePName;
extern char    PathPAudit[];
extern ExListType			   m_PMMLcmds;
extern ExListType			   m_PMMLprts;
extern ExListType              m_PUSRs;

extern ACE_Recursive_Thread_Mutex  _acs_alog_PLog_Mutex;
extern ACE_Recursive_Thread_Mutex  _acs_alog_ZipPlog_Mutex;

extern int  AlogIsAlive;
extern int  PlogIsAlive;
extern int  CommandAndSessionLogState;
extern int  CommandAndSessionPLogState;
extern int  LargeEventDataSkipped;
extern int  FileNamingFormat;
extern int  alogFileisActive;
extern int  mmlSyslogisActive;
extern int  mmlSyslogAvailabilityState;	

extern string  TransferQueue;
extern string  commandLogTransferQueue;
extern string  PLOGTransferQueue;

extern char   ALOGdeactivateError;
extern char   OpenPMMLPRTFileError;
extern char   ReadPMMLPRTFileError;
extern char   OpenPMMCMDFileError;
extern char   ReadPMMLCMDFileError;
extern char   OpenPUSRFileError;
extern char   ReadPUSRFileError;
extern char   AttachPLTQerror;

extern void  stringToUpper (string &str);

using namespace std;

ACS_ALOG_ReturnType acs_alog_cmdHandler::getExclCmd()
{

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	ACS_ALOG_ReturnType res;

	std::vector<std::string> p_dnList;

	ACS_APGCC_ImmAttribute attribute_1;
	ACS_APGCC_ImmAttribute attribute_2;

	attribute_1.attrName = "type";
	attribute_2.attrName = "item";

	std::vector<ACS_APGCC_ImmAttribute *> attributes;
	attributes.push_back(&attribute_1);
	attributes.push_back(&attribute_2);

	char mess[150];

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS )
	{
		ACE_OS::sprintf(mess,"Error: %d %s",immHandle.getInternalLastError(),immHandle.getInternalLastErrorText());
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
		return ACS_ALOG_error;
	}

	result = immHandle.getClassInstances("AxeAuditLoggingExclusionItem", p_dnList);
	if (result != ACS_CC_SUCCESS)
	{
		if (immHandle.getInternalLastError() == -41)
		{
			ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Exclusion list is empty"));
			return ACS_ALOG_ok;
		}

		ACE_OS::sprintf(mess,"Error: %d %s",immHandle.getInternalLastError(),immHandle.getInternalLastErrorText());
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
		return ACS_ALOG_ok;
	}

	for (unsigned i=0; i<p_dnList.size(); i++)
	{
		result = immHandle.getAttribute(p_dnList[i].c_str(), attributes);
		if (result != ACS_CC_SUCCESS )
		{
			ACE_OS::sprintf(mess,"Error: %d %s",immHandle.getInternalLastError(),immHandle.getInternalLastErrorText());
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
			return ACS_ALOG_error;
		}

		res = setExclCmdList((*reinterpret_cast<int *>(attributes[0]->attrValues[0])),(reinterpret_cast<char *>(attributes[1]->attrValues[0])));
		if (res != ACS_ALOG_ok )
		{
			ACE_OS::sprintf(mess,"Error:setExclCmdList failure for command: %s",(reinterpret_cast<char *>(attributes[1]->attrValues[0])));
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
			return ACS_ALOG_error;
		}
	}

	result = immHandle.Finalize();
	if (result != ACS_CC_SUCCESS)
	{
		ACE_OS::sprintf(mess,"Error: %d %s",immHandle.getInternalLastError(),immHandle.getInternalLastErrorText());
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
		return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType  acs_alog_cmdHandler::setExclCmdList (int exclType, string exclCmd)
{

	int count = 0;
	char mess[150];

	ACE_OS::sprintf(mess,"Exclusion item %s type %d is present!",exclCmd.c_str(),exclType);

	stringToUpper (exclCmd);

	switch (exclType)
	{
		case  0  :  count = (int) typecmdmc.count(exclCmd);
			    if (count > 0)
			    {
				    ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
				    return ACS_ALOG_error;
			    }
			    typecmdmc.insert(exclCmd);
			    break;

		case  1  :  count = (int) typecmdmp.count(exclCmd);
			    if (count > 0)
			    {
				    ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
				    return ACS_ALOG_error;
			    }
			    typecmdmp.insert(exclCmd);
			    break;

		case  2  :  count = (int) typecmdmpa.count(exclCmd);
			    if (count > 0)
			    {
				    ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
				    return ACS_ALOG_error;
			    }
			    typecmdmpa.insert(exclCmd);
			    break;

		case  3  :  count = (int) typecmdnp.count(exclCmd);
			    if (count > 0)
			    {
				    ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
				    return ACS_ALOG_error;
			    }
			    typecmdnp.insert(exclCmd);
			    break;
		case  5  :  count = (int) typecmdnc.count(exclCmd);
			    if (count > 0)
			    {
				    ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(mess));
				    return ACS_ALOG_error;
			    }
			    typecmdnc.insert(exclCmd);
			    break;

		default :   ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("Type not recognized"));
			    return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType  acs_alog_cmdHandler::delExclCmdList (int exclType, string exclCmd)
{

	int   count = 0;
	char  mess[150];

	ACE_OS::sprintf (mess,"Exclusion list type %d is empty",exclType);

	switch (exclType)
	{
		case  0  :  count = (int) typecmdmc.count(exclCmd);
			    if (count == 0)  return ACS_ALOG_ok;
			    typecmdmc.erase(exclCmd);
			    break;

		case  1  :  count = (int) typecmdmp.count(exclCmd);
			    if (count == 0)  return ACS_ALOG_ok;
			    typecmdmp.erase(exclCmd);
			    break;

		case  2  :  count = (int) typecmdmpa.count(exclCmd);
			    if (count == 0)  return ACS_ALOG_ok;
			    typecmdmpa.erase(exclCmd);
			    break;

		case  3  :  count = (int) typecmdnp.count(exclCmd);
			    if (count == 0)  return ACS_ALOG_ok;
			    typecmdnp.erase(exclCmd);
			    break;

		case  5  :  count = (int) typecmdnc.count(exclCmd);
			    if (count == 0)  return ACS_ALOG_ok;
			    typecmdnc.erase(exclCmd);
			    break;

		default :   ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Type not recognized"));
			    return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType acs_alog_cmdHandler::setAlogStatus(int Status)
{

	AlogIsAlive = Status;

	if (AlogIsAlive)
	{
		AEHEVENTMESSAGE ("acs_alogmaind", 9900, "CEASING",
				"AUDIT LOG DEACTIVATED", "APZ", "acs_alogmaind/Alarm", "", "");
		ALOGdeactivateError = 0;
	}
	else {
		AEHEVENTMESSAGE ("acs_alogmaind", 9900, "A2", "AUDIT LOG DEACTIVATED", "APZ",
				"acs_alogmaind/Alarm", "alogAdmState = LOCKED", "");
		ALOGdeactivateError = 1;
	}

	return ACS_ALOG_ok;
}

ACS_ALOG_ReturnType acs_alog_cmdHandler::setAlogFileStatus(int Status)
{
	alogFileisActive = Status;
	return ACS_ALOG_ok;
}

ACS_ALOG_ReturnType acs_alog_cmdHandler::setMmlSyslogStatus(int Status)
{
	mmlSyslogisActive = Status;
	return ACS_ALOG_ok;
}

ACS_ALOG_ReturnType acs_alog_cmdHandler::setMmlSyslogAvailabilityStatus(int Status)
{
	mmlSyslogAvailabilityState = Status;
	return ACS_ALOG_ok;
}

ACS_ALOG_ReturnType acs_alog_cmdHandler::File_Exist (char *sPath)
{
	ifstream  in(sPath,ios::in);

	if (! in)
	{
		in.close();
		return ACS_ALOG_ok;
	}

	in.close();
	return ACS_ALOG_error;
}


void acs_alog_cmdHandler::setAlogIDS(int stateIDS)
{
	CommandAndSessionLogState = stateIDS;
}

void acs_alog_cmdHandler::setPlogIDS(int stateIDS)
{
	CommandAndSessionPLogState = stateIDS;
}

void acs_alog_cmdHandler::setAlogTQ(string TQ)
{
	TransferQueue = TQ;
}

void acs_alog_cmdHandler::setAlogCmdLogTQ(string commandLogTQ){
	commandLogTransferQueue = commandLogTQ;
}

void acs_alog_cmdHandler::setFileNaming (int format)
{
		FileNamingFormat = format;
}



enumCmdExecuteResult acs_alog_cmdHandler::changePlogIdTag(const char* idTag)
{
	ACS_ALOG_AttrFileRecord m_AttrFileRecord;

	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/ATTRFILEP";

	ACE_stat statBuff;
	//ACE_INT16 result = -1;
	if( ACE_OS::stat(szFileName.c_str(), &statBuff)  != 0 )
	{
		cerr << endl << "File access error" << endl ;
		return cerFileWriteError;

	}

	if(statBuff.st_size != 0)
	{
		m_hFile = ACE_OS::open(szFileName.c_str(), O_RDONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			//ACE_OS::close(m_hFile);
			return cerFileOpenError;
		}
		int dwBytesRead;
		dwBytesRead = ACE_OS::read(m_hFile, (void*) (& m_AttrFileRecord),
				sizeof(ACS_ALOG_AttrFileRecord));

		if (dwBytesRead != sizeof(ACS_ALOG_AttrFileRecord))
		{
			ACE_OS::close(m_hFile);
			return cerFileReadError;
		}
		if(m_AttrFileRecord.IdTag == 1 && strcasecmp(idTag, "yes") == 0)
		{
			ACE_OS::close(m_hFile);
			return cerItemAlreadyDefined;

		}
		if(m_AttrFileRecord.IdTag == 0 && strcasecmp(idTag, "no") == 0)
		{
			ACE_OS::close(m_hFile);
			return cerItemAlreadyDefined;

		}
		if(!strcasecmp(idTag,"yes"))
		{
			CommandAndSessionPLogState = 1;
			m_AttrFileRecord.IdTag = true;
		}
		else if(!strcasecmp(idTag,"no"))
		{
			CommandAndSessionPLogState = 0;
			m_AttrFileRecord.IdTag = false;
		}
		ACE_OS::close(m_hFile);
	}

	m_hFile = ACE_OS::open(szFileName.c_str(),O_WRONLY | O_TRUNC);

	int dwBytesWritten;
	dwBytesWritten = ACE_OS::write(m_hFile,
			(void*)&m_AttrFileRecord,
			sizeof(ACS_ALOG_AttrFileRecord));

	if (dwBytesWritten == 0)
	{
		ACE_OS::close(m_hFile);
		//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
		//Unlock();
		return cerFileWriteError;
	}
	ACE_OS::close(m_hFile);

	return cerSuccess;
}


enumCmdExecuteResult acs_alog_cmdHandler::changePlogFileDest(const char* TQ)
{
	ACS_ALOG_AttrFileRecord m_AttrFileRecord;

	string  msg = "Change PLOG TQ .... Actual = ";
	msg += PLOGTransferQueue + "   New = ";
	msg += TQ;
	ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(msg.c_str()));

	if (! strcmp(PLOGTransferQueue.c_str(), TQ))
	{
			return cerItemAlreadyDefined;
	}

	if (strlen(TQ) == 0)
	{
		AEHEVENTMESSAGE ("acs_alogmaind", 9926, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmain/Alarm", "Error. No transfer queue defined for the protected log-files", "FCODE 9\nPL Transfer Queue not defined");

		if (AttachPLTQerror == 1)       //  TR :  HS11854
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9927, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
				AttachPLTQerror = 0;
		}
	}
	else {
			AEHEVENTMESSAGE ("acs_alogmaind", 9926, "CEASING",
							 "AUDIT LOG FAULT", "APZ", "acs_alogmain/Alarm", "", "");
	}

	if (TQ[0] != 0)
	{
		if (! strcmp(PLOGTransferQueue.c_str(),TQ))
		{
				return cerItemAlreadyDefined;
		}
	}

	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/ATTRFILEP";

	ACE_stat statBuff;

	if( ACE_OS::stat(szFileName.c_str(), &statBuff)  != 0 )
	{
		cerr << endl << "File access error" << endl ;
		return cerFileWriteError;

	}

	if(statBuff.st_size != 0)
	{
		m_hFile = ACE_OS::open(szFileName.c_str(), O_RDONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			//ACE_OS::close(m_hFile);
			return cerFileOpenError;
		}

		int dwBytesRead;
		dwBytesRead = ACE_OS::read(m_hFile, (void*) (& m_AttrFileRecord), sizeof(ACS_ALOG_AttrFileRecord));

		if (dwBytesRead != sizeof(ACS_ALOG_AttrFileRecord))
		{
			ACE_OS::close(m_hFile);
			return cerFileReadError;
		}

		ACE_OS::close(m_hFile);
	}

	m_hFile = ACE_OS::open(szFileName.c_str(),O_WRONLY | O_TRUNC);

	strcpy(m_AttrFileRecord.FileDest, TQ);

	int dwBytesWritten;
	dwBytesWritten = ACE_OS::write(m_hFile,	(void*)&m_AttrFileRecord, sizeof(ACS_ALOG_AttrFileRecord));

	if (dwBytesWritten == 0)
	{
		ACE_OS::close(m_hFile);
		//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
		//Unlock();
		return cerFileWriteError;
	}
	ACE_OS::close(m_hFile);

	PLOGTransferQueue = TQ;
	return cerSuccess;
}


void acs_alog_cmdHandler::setAlogDataSkipped(int stateDataSkipped)
{
	LargeEventDataSkipped = stateDataSkipped;
}

string  acs_alog_cmdHandler::getAlogTQ()
{
	return TransferQueue;
}

string  acs_alog_cmdHandler::getAlogCmdLogTQ()
{
	return commandLogTransferQueue;
}

int  acs_alog_cmdHandler::getAlogStatus()
{
	return AlogIsAlive;
}

int  acs_alog_cmdHandler::getAlogFileStatus()
{
	return alogFileisActive;
}

int  acs_alog_cmdHandler::getMmlSyslogStatus()
{
	return mmlSyslogisActive;
}
int acs_alog_cmdHandler::getMmlSyslogAvailabilityStatus()
{
	return mmlSyslogAvailabilityState;
}
int  acs_alog_cmdHandler::getAlogDataSkipped()
{
	return LargeEventDataSkipped;
}

int  acs_alog_cmdHandler::getAlogIDS()
{
	return CommandAndSessionLogState;
}

int  acs_alog_cmdHandler::getFileNaming()
{
	return FileNamingFormat;
}

enumCmdExecuteResult acs_alog_cmdHandler::SetPw(const char* cPtrSetPw)
{

	// Need exclusive access
	std::string szFileName;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/SYS";

	char* cPtrEncriptedData;

	ifstream is;
	int length;
	char *buffer;
	try {

		is.open (szFileName.c_str(), ios::binary );

		// get length of file:
		is.seekg (0, ios::end);
		length = is.tellg();
		// if SYS is not empty return error
		if (length > 0)
		{
			//			Unlock();
			return cerPwAlreadyDefined; // PL Password already defined
		}
		is.seekg (0, ios::beg);

		length = (int) strlen(cPtrSetPw);

		// allocate memory:
		buffer = new char [length + 1];

		is.close();

		// EncryptDecriptData()
		char key[6];
		strcpy(key, "alogp");

		cPtrEncriptedData = new char [length+1];

		ACS_APGCC_CommonLib temp;
		temp.EncryptDecryptData(cPtrSetPw, " " ,cPtrEncriptedData, key, ACS_APGCC_DIR_ENCRYPT, length, 5 );

		//write
		ofstream outfile (szFileName.c_str(),ofstream::binary);
		outfile.write (cPtrEncriptedData,length);
		outfile.close();

	}
	catch(int e)
	{
		//Unlock();
		delete[] buffer;
		delete[] cPtrEncriptedData;
		return cerFileWriteError;
	}
	delete[] buffer;
	delete[] cPtrEncriptedData;
	//Unlock();
	PlogIsAlive =1;
	//int m_nRetCode = UpdateProtectedPassword();
	UpdateProtectedPassword();
	return cerSuccess;
}

// PL
enumCmdExecuteResult acs_alog_cmdHandler::SetNewPw(const char* cPtrOldPw,const char* cPtrNewPw)
{
	//   Lock();

	//ACE_TCHAR szFileName[128+1]= "/data/acs/data/alog/system/SYS";
	string szFileName;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/SYS";
	unsigned int  length;
	char         *buffer;
	ifstream      is;

	is.open (szFileName.c_str(), ios::binary);

	if (is.is_open())
	{
		is.seekg (0, ios::end);
		length = is.tellg();

		if (length == 0){
			//Unlock();
			return cerPwNotDefined; // PL Password not defined
		}
		is.seekg (0, ios::beg);

		// allocate memory:
		buffer = new char [length];

		// read data as a block:
		is.read (buffer,length);

		is.close();
	}
	else  return cerFileWriteError;

	if(length != strlen(cPtrOldPw)){
		cout<<"Length is not equal"<<endl;
		// Password don't match
		delete[] buffer;
		//Unlock();
		return cerPwNotMatch;
	}
	char key[6];
	strcpy(key, "alogp");
	char* m_passwordInFile;

	m_passwordInFile = new char [length];
	ACS_APGCC_CommonLib temp1;

	temp1.EncryptDecryptData(buffer , " " ,m_passwordInFile, key, ACS_APGCC_DIR_DECRYPT, length, 5 );

	// Compare old password

	if ( memcmp(cPtrOldPw,m_passwordInFile,length) != 0)
	{
		// Password don't match
		delete[] m_passwordInFile;
		delete[] buffer;
		//		 Unlock();
		return cerPwNotMatch;
	}

	// Old Password Ok
	// Encrypt New Password
	delete[] buffer;
	delete[] m_passwordInFile;

	length = strlen(cPtrNewPw);
	m_passwordInFile = new char [length+1];

	temp1.EncryptDecryptData(cPtrNewPw," ", m_passwordInFile, key, ACS_APGCC_DIR_ENCRYPT, length, 5 );

	try {
		//write
		ofstream outfile (szFileName.c_str(),ofstream::binary);
		outfile.write (m_passwordInFile,length);
		outfile.flush();
		outfile.close();

	}catch(...){

		//Unlock();
		delete[] m_passwordInFile;
		return cerFileWriteError;
	}

	changePasswordToTheZipFile(cPtrNewPw);
	int m_nRetCode = UpdateProtectedPassword();

	if(m_nRetCode == 2)
	{
		delete[] m_passwordInFile;
		return cerFileOpenError;
	}

	delete[] m_passwordInFile;

	//	    Unlock();
	return cerSuccess;
}  // PL


void  acs_alog_cmdHandler::changePasswordToTheZipFile (const char* cPtrNewPw)
{

	FILE  *fp;

	if ((fp = fopen (FilePAudit.c_str(), "r")) == NULL)  return;
	fclose (fp);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogZip (_acs_alog_ZipPlog_Mutex);

	if (FilePAudit.length() == 0)  return;		//  A new file must be created when logging a new event
	string pwdFor7zextraction = protectedPassword + "\n";
	string newPwdFor7zarchive(cPtrNewPw);
	newPwdFor7zarchive += "\n";
	newPwdFor7zarchive += newPwdFor7zarchive;
	string  cmdUnzip = "/usr/bin/7z e -o\"/tmp\" -y " + FilePAudit + " 1>/dev/null 2>/dev/null";
	int  exitCode = -1;
	bool commandExecuted = execute7zCmd(cmdUnzip, pwdFor7zextraction, exitCode);				// TR HX49170
	if ((commandExecuted == false) || ((commandExecuted == true) && (!WIFEXITED(exitCode))))
	{
		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("ALOGSET -P -C :  Failed to execute 7z extaction command failed"));
		return;
	}
	chmod (FilePName.c_str(), S_IRUSR | S_IWUSR | S_IRGRP);

	switch (WEXITSTATUS(exitCode))
	{
		case  0 :   						//  normal; no errors or warnings detected
		case  9 :   						//  the specified zipfiles were not found
			break;

		default :
			string  msg = "ALOGSET -P -C  -  7z extraction failed :  code = ";
			char  code[10];
			sprintf (code, "   %03d", WEXITSTATUS(exitCode));
			msg = msg + code + "\n" + cmdUnzip;;
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(msg));
			return ;
	}

	size_t ext;
	if ((ext = FilePAudit.find(".zip")) != string::npos)
	{
		FilePAudit= FilePAudit.substr(0,ext);
		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("FilePAudit is"));
		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(FilePAudit));
		FilePAudit = FilePAudit +".7z";
		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(FilePAudit));
	}

	commandExecuted = false;
	string  cmdZip = "/usr/bin/7z a -tzip -mx=0 -mem=AES256 " + FilePAudit + " " + FilePName + " -p";
	exitCode = -1;
	commandExecuted = execute7zCmd(cmdZip, newPwdFor7zarchive, exitCode);						// TR HX49170

	if ((commandExecuted == false)  || (!WIFEXITED(exitCode)) || (WEXITSTATUS(exitCode) != 0))
	{
			string  msg = "ALOGSET -P -C  -  7z archive failed :  code = ";
			char  code[10];
			sprintf (code, "   %03d", WEXITSTATUS(exitCode));
			msg = msg + code + "\n" + cmdZip;
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(msg));
	}

	unlink (FilePName.c_str());
}


enumCmdExecuteResult acs_alog_cmdHandler::ChangePassword()
{
	//m_bPLPasswordPresent = TRUE;
	//return m_pPLLogger->ChangePassword();
	return cerSuccess;
}

void acs_alog_cmdHandler::UpdatePassword()
{
	return;
	//m_pPLLogger->UpdatePassword();
}

int acs_alog_cmdHandler::UpdateProtectedPassword()
{
	std::string szFileName;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/SYS";
	char * buffer;

	unsigned int length;
	ifstream is;
	try {

		is.open (szFileName.c_str(), ios::binary );

		// get length of file:
		is.seekg (0, ios::end);
		length = is.tellg();

		if (length <= 0){
			//Unlock();
			return 0; // PL Password not defined
		}

		is.seekg (0, ios::beg);

		// allocate memory:
		buffer = new char [length];
		// read data as a block:
		is.read (buffer,length);
		is.close();

	}catch( ... ){
		//Unlock();
		delete[] buffer;
		return 2;
	}

	char key[6];
	strcpy(key, "alogp");
	char* passwordInFile;

	passwordInFile = new char [length+1];
	ACS_APGCC_CommonLib temp;

	temp.EncryptDecryptData(buffer , " " ,passwordInFile, key, ACS_APGCC_DIR_DECRYPT, length, 5 );

	// Compare old password
	passwordInFile[length] = 0;

	std::string mystr(passwordInFile);
	protectedPassword = mystr;
	delete[] passwordInFile;
	delete [] buffer;
	return 0;
}


enumCmdExecuteResult acs_alog_cmdHandler::CheckPw(const char* cPw)	//PL
{
	// Need exclusive access
	//    Lock();

	std::string szFileName;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/SYS";
	//ACE_TCHAR szFileName[128+1];
	//strcpy(szFileName, "/data/acs/data/alog/system/SYS");

	/*if (!ACS_ALOG_Common::CheckFile(szFileName, FALSE))
	  {
	  Unlock();
	  return cerPwNotDefined; // PL Password not defined
	  }
	 */
	char * buffer;

	unsigned int length;
	ifstream is;
	try {

		is.open (szFileName.c_str(), ios::binary );

		// get length of file:
		is.seekg (0, ios::end);
		length = is.tellg();

		if (length <= 0){
			//Unlock();
			return cerPwNotDefined; // PL Password not defined
		}

		is.seekg (0, ios::beg);

		// allocate memory:
		buffer = new char [length];

		// read data as a block:
		is.read (buffer,length);
		is.close();

	}catch( ... ){
		//Unlock();
		delete[] buffer;
		return cerFileWriteError;
	}
	if(length != strlen(cPw)){
		// Password don't match
		// delete[] buffer;
		// Unlock();
		delete [] buffer;
		return cerPwNotMatch;
	}

	char key[6];
	strcpy(key, "alogp");
	char* passwordInFile;

	passwordInFile = new char [length+1];
	ACS_APGCC_CommonLib temp;

	temp.EncryptDecryptData(buffer , " " ,passwordInFile, key, ACS_APGCC_DIR_DECRYPT, length, 5 );

	// Compare old password
	passwordInFile[length] = 0;

	if ( memcmp(cPw,passwordInFile,length) != 0)
	{
		// Password don't match
		delete[] passwordInFile;
		delete[] buffer;
		//Unlock();
		return cerPwNotMatch;
	}

	delete[] buffer;
	delete[] passwordInFile;

	//Unlock();
	return cerSuccess;
}


//Protected logging
enumCmdExecuteResult acs_alog_cmdHandler::ProtectMMLcmd(const char* lpszCmd)
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	// Need exclusive access
	//Lock();
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	if (checkMMLcmdProtection(lpszCmd, ("*")))
	{
		// Unlock();
		return cerItemAlreadyDefined;
	}

	string szFileName;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/PMMLCMD";

	// Update EXMMLCMD
	// Truncate the existing file but preserve file attributes
	m_hFile = ACE_OS::open(szFileName.c_str(),
			O_WRONLY|O_TRUNC);

	if (m_hFile == ACE_INVALID_HANDLE)
	{
		//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileOpen);
		//Unlock();
		AEHEVENTMESSAGE ("acs_alogmaind", 9930, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access PMMLCMD file");
		OpenPMMCMDFileError =1;
		//ACE_OS::close(m_hFile);
		return cerFileOpenError;
	}

	// Not found in list, then insert it!
	m_PMMLcmds.push_back(ACS_ALOG_Exclude(lpszCmd));

	// Sort the list
	m_PMMLcmds.sort();

	ExListIterType exListIter = m_PMMLcmds.begin();
	while (exListIter != m_PMMLcmds.end())
	{
		strcpy(m_Exclude.Data1, Encode((*exListIter).Data1));
		strcpy(m_Exclude.Data2, (""));

		int dwBytesWritten;
		dwBytesWritten = ACE_OS::write(m_hFile,
				(void*)&m_Exclude,
				sizeof(ACS_ALOG_Exclude));

		if (dwBytesWritten == -1)
		{
			ACE_OS::close(m_hFile);
			//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
			//Unlock();
			return cerFileWriteError;
		}
		exListIter++;
	}

	ACE_OS::close(m_hFile);

	// All is well, cease any outstanding alarms
	//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLCMD, etFileOpen);
	//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLCMD, etFileRead);
	//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLCMD, etFileWrite);

	//Unlock();
	AEHEVENTMESSAGE ("acs_alogmaind", 9930, "CEASING", "AUDIT LOG FAULT",
			"APZ", "acs_alogmaind/Alarm", "", "");
	AEHEVENTMESSAGE ("acs_alogmaind", 9936, "CEASING", "AUDIT LOG FAULT",
			"APZ", "acs_alogmaind/Alarm", "", "");
	OpenPMMCMDFileError = 0;
	ReadPMMLCMDFileError = 0;
	return cerSuccess;
}

//----------------------------------------------------------------------------

enumCmdExecuteResult acs_alog_cmdHandler::ProtectMMLprt(const char* lpszPrt)
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	// Need exclusive access
	//Lock();

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	// Check if the printout is already excluded
	ExListIterType exListIter = m_PMMLprts.begin();
	while (exListIter != m_PMMLprts.end())
	{
		if (strcmp((*exListIter).Data1, lpszPrt) == 0)
		{
			//    Unlock();
			return cerItemAlreadyDefined; // Item Already Excluded
		}
		else
			exListIter++;
	}

	string szFileName;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/PMMLPRT";

	// Update EXMMLPRT
	m_hFile = ACE_OS::open(szFileName.c_str(),
			O_WRONLY|O_TRUNC);

	if (m_hFile == ACE_INVALID_HANDLE)
	{
		//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileOpen);
		//Unlock();
		AEHEVENTMESSAGE ("acs_alogmaind", 9931, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access PMMLPRT file");
		OpenPMMLPRTFileError = 1;
		//ACE_OS::close(m_hFile);
		return cerFileOpenError;
	}
	// Not found in list, then insert it!
	m_PMMLprts.push_back(ACS_ALOG_Exclude(lpszPrt));

	// Sort the list
	m_PMMLprts.sort();

	exListIter = m_PMMLprts.begin();
	while (exListIter != m_PMMLprts.end())
	{

		strcpy(m_Exclude.Data1, Encode((*exListIter).Data1));
		strcpy(m_Exclude.Data2, (""));

		int dwBytesWritten;
		dwBytesWritten = ACE_OS::write(m_hFile,
				(void*)&m_Exclude,
				sizeof(ACS_ALOG_Exclude));

		if (dwBytesWritten == 0)
		{
			ACE_OS::close(m_hFile);
			//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
			//Unlock();
			return cerFileWriteError;
		}
		exListIter++;
	}


	ACE_OS::close(m_hFile);

	// All is well, cease any outstanding alarms
	//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLPRT, etFileOpen);
	//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLPRT, etFileRead);
	//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLPRT, etFileWrite);

	//Unlock();
	AEHEVENTMESSAGE ("acs_alogmaind", 9931, "CEASING", "AUDIT LOG FAULT",
			"APZ", "acs_alogmaind/Alarm", "", "");
	AEHEVENTMESSAGE ("acs_alogmaind", 9937, "CEASING", "AUDIT LOG FAULT",
			"APZ", "acs_alogmaind/Alarm", "", "");
	OpenPMMLPRTFileError = 0;
	ReadPMMLPRTFileError = 0;
	return cerSuccess;
}
//----------------------------------------------------------------------------

enumCmdExecuteResult acs_alog_cmdHandler::ProtectUserData(const char* lpszUsr)
{
	ACE_HANDLE m_hFile = NULL;
	// Need exclusive access
	//Lock();

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	// Check if user is already excluded
	ExListIterType exListIter = m_PUSRs.begin();
	while (exListIter != m_PUSRs.end())
	{
		if (strcmp((*exListIter).Data1, lpszUsr) == 0)
		{
			return cerUserAlreadyDefined; // User Already Excluded
		}
		else
			exListIter++;
	}

	string szFileName;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/PUSR";

	// Update EXUSR
	m_hFile = ACE_OS::open(szFileName.c_str(),
			O_WRONLY|O_TRUNC);

	if (m_hFile == ACE_INVALID_HANDLE)
	{
	    AEHEVENTMESSAGE ("acs_alogmaind", 9939, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access PUSR file");
		OpenPUSRFileError = 1;
		return cerFileOpenError;
	}
	// Not found in list, then insert it!
	m_PUSRs.push_back(ACS_ALOG_Exclude(lpszUsr));

	// Sort the list
	m_PUSRs.sort();

	exListIter = m_PUSRs.begin();
	while (exListIter != m_PUSRs.end())
	{

		strcpy(m_Exclude.Data1, Encode((*exListIter).Data1));
		strcpy(m_Exclude.Data2, (""));

		int dwBytesWritten;
		dwBytesWritten = ACE_OS::write(m_hFile,
				(void*)&m_Exclude,
				sizeof(ACS_ALOG_Exclude));

		if (dwBytesWritten == 0)
		{
			ACE_OS::close(m_hFile);
			return cerFileWriteError;
		}
		exListIter++;
	}

	ACE_OS::close(m_hFile);

	AEHEVENTMESSAGE ("acs_alogmaind", 9939, "CEASING", "AUDIT LOG FAULT",
			"APZ", "acs_alogmaind/Alarm", "", "");
	AEHEVENTMESSAGE ("acs_alogmaind", 9940, "CEASING", "AUDIT LOG FAULT",
			"APZ", "acs_alogmaind/Alarm", "", "");
	OpenPUSRFileError  = 0;
	ReadPUSRFileError = 0;
	return cerSuccess;
}
//----------------------------------------------------------------------------
enumCmdExecuteResult acs_alog_cmdHandler::UnprotectMMLcmd(const char* lpszCmd)
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	// Need exclusive access
	//Lock();

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	bool m_bFound = false;

	ExListIterType exListIter = m_PMMLcmds.begin();
	while (!m_bFound && exListIter != m_PMMLcmds.end())
	{
		if (strcmp((*exListIter).Data1, lpszCmd) == 0)
		{
			m_bFound = true;
		}
		else
			exListIter++;
	}

	if (m_bFound)
	{
		string szFileName;
		getAlogDataDiskPath(szFileName);
		szFileName += "/ACS_ALOG/PMMLCMD";
		// Update EXMMLCMD
		m_hFile = ACE_OS::open(szFileName.c_str(),
				O_WRONLY|O_TRUNC);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileOpen);
			//Unlock();

			AEHEVENTMESSAGE ("acs_alogmaind", 9930, "A2", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
					"FCODE: 5\nCAUSE: Could not access PMMLCMD file");
			OpenPMMCMDFileError =1;
			//ACE_OS::close(m_hFile);
			return cerFileOpenError;
		}

		// Found in list, then remove it!
		m_PMMLcmds.erase(exListIter);

		exListIter = m_PMMLcmds.begin();
		while (exListIter != m_PMMLcmds.end())

		{
			strcpy(m_Exclude.Data1, Encode((*exListIter).Data1));
			strcpy(m_Exclude.Data2, (""));

			int dwBytesWritten;

			dwBytesWritten = ACE_OS::write(m_hFile,
					(void*)&m_Exclude,
					sizeof(ACS_ALOG_Exclude));
			if (dwBytesWritten == -1)
			{
				ACE_OS::close(m_hFile);
				//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
				//Unlock();
				return cerFileWriteError;
			}
			exListIter++;
		}
		ACE_OS::close(m_hFile);

		AEHEVENTMESSAGE ("acs_alogmaind", 9930, "CEASING", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "", "");

		AEHEVENTMESSAGE ("acs_alogmaind", 9936, "CEASING", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "", "");

		OpenPMMCMDFileError = 0;
		ReadPMMLCMDFileError = 0;
		return cerSuccess;
	}

	//Unlock();

	return cerItemNotDefined; // Item Not excluded
}

//----------------------------------------------------------------------------
enumCmdExecuteResult acs_alog_cmdHandler::UnprotectMMLprt(const char* lpszPrt)
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	// Need exclusive access
	//Lock();

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	bool m_bFound = false;

	ExListIterType exListIter = m_PMMLprts.begin();
	while (!m_bFound && exListIter != m_PMMLprts.end())
	{
		if (strcmp((*exListIter).Data1, lpszPrt) == 0)
			m_bFound = true;
		else
			exListIter++;
	}

	if (m_bFound)
	{
		string szFileName;
		getAlogDataDiskPath(szFileName);
		szFileName += "/ACS_ALOG/PMMLPRT";

		// Update EXMMLCMD
		m_hFile = ACE_OS::open(szFileName.c_str(),O_WRONLY|O_TRUNC);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileOpen);
			//Unlock();
			OpenPMMLPRTFileError = 1;
			AEHEVENTMESSAGE ("acs_alogmaind", 9931, "A2", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
					"FCODE: 5\nCAUSE: Could not access PMMLPRT file");
			//ACE_OS::close(m_hFile);
			return cerFileOpenError;
		}

		// Found in list, then remove it!
		m_PMMLprts.erase(exListIter);

		exListIter = m_PMMLprts.begin();

		while (exListIter != m_PMMLprts.end())
		{
			strcpy(m_Exclude.Data1, Encode((*exListIter).Data1));
			strcpy(m_Exclude.Data2, (""));

			int dwBytesWritten;

			dwBytesWritten = ACE_OS::write(m_hFile,
					(void*)&m_Exclude,
					sizeof(ACS_ALOG_Exclude));

			if (dwBytesWritten == 0)
			{
				ACE_OS::close(m_hFile);
				//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
				//Unlock();
				return cerFileWriteError;
			}
			exListIter++;
		}


		ACE_OS::close(m_hFile);


		// All is well, cease any outstanding alarms
		//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLCMD, etFileOpen);
		//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLCMD, etFileRead);
		//m_nRetCode = m_pAlarmHandler->CeaseError(esPMMLCMD, etFileWrite);

		//Unlock();
		AEHEVENTMESSAGE ("acs_alogmaind", 9931, "CEASING", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "", "");
		AEHEVENTMESSAGE ("acs_alogmaind", 9937, "CEASING", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "", "");
		OpenPMMLPRTFileError = 0;
		ReadPMMLPRTFileError = 0;

		return cerSuccess;
	}

	//Unlock();

	return cerItemNotDefined; // Item Not excluded
}

//----------------------------------------------------------------------------
enumCmdExecuteResult acs_alog_cmdHandler::UnprotectUserData(const char* lpszUsr)
{
	ACE_HANDLE m_hFile = NULL;
	// Need exclusive access
	//Lock();

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	bool m_bFound = false;

	ExListIterType exListIter = m_PUSRs.begin();
	while (!m_bFound && exListIter != m_PUSRs.end())
	{
		if (strcmp((*exListIter).Data1, lpszUsr) == 0)
			m_bFound = true;
		else
			exListIter++;
	}

	if (m_bFound)
	{
		string szFileName;
		getAlogDataDiskPath(szFileName);
		szFileName += "/ACS_ALOG/PUSR";

		// Update EXMMLCMD
		m_hFile = ACE_OS::open(szFileName.c_str(),O_WRONLY|O_TRUNC);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			OpenPUSRFileError = 1;
			AEHEVENTMESSAGE ("acs_alogmaind", 9939, "A2", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
					"FCODE: 5\nCAUSE: Could not access PUSR file");
			//ACE_OS::close(m_hFile);
			return cerFileOpenError;
		}

		// Found in list, then remove it!
		m_PUSRs.erase(exListIter);

		exListIter = m_PUSRs.begin();

		while (exListIter != m_PUSRs.end())
		{
			strcpy(m_Exclude.Data1, Encode((*exListIter).Data1));
			strcpy(m_Exclude.Data2, (""));

			int dwBytesWritten;

			dwBytesWritten = ACE_OS::write(m_hFile,
					(void*)&m_Exclude,
					sizeof(ACS_ALOG_Exclude));

			if (dwBytesWritten == 0)
			{
				ACE_OS::close(m_hFile);
				return cerFileWriteError;
			}
			exListIter++;
		}

		ACE_OS::close(m_hFile);

		//Unlock();
		AEHEVENTMESSAGE ("acs_alogmaind", 9939, "CEASING", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "", "");
		AEHEVENTMESSAGE ("acs_alogmaind", 9940, "CEASING", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "", "");
		OpenPUSRFileError = 0;
		ReadPUSRFileError = 0;

		return cerSuccess;
	}

	//Unlock();

	return  cerUserNotDefined; // Item Not excluded
}

//----------------------------------------------------------------------------

bool acs_alog_cmdHandler::checkMMLcmdProtection(const char* lpszCmd, const char* lpszPar)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	// Check if the command is already excluded
	ExListIterType exListIter = m_PMMLcmds.begin();

	while (exListIter != m_PMMLcmds.end()){
		if (strcmp((*exListIter).Data1, lpszCmd) == 0)
			return true;
		else
			exListIter++;
	}

	(void)(lpszPar);
	return false;
}


enumCmdExecuteResult acs_alog_cmdHandler::ResetFile(const char* fileName)
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName = szFileName +"/ACS_ALOG/" + fileName;
	if (checkFile(szFileName.c_str(), false))
	{
		m_hFile = ACE_OS::open(szFileName.c_str(), O_WRONLY | O_TRUNC);
		if (m_hFile == ACE_INVALID_HANDLE)
		{
			//ACE_OS::close(m_hFile);
			return cerFileOpenError;
		}
		ACE_OS::close(m_hFile);

	}
	else
	{
		return cerFileOpenError;
	}
	return cerSuccess;

}

enumCmdExecuteResult acs_alog_cmdHandler::TQ_CeaseHandler()
{
	AEHEVENTMESSAGE ("acs_alogmaind", 9926, "CEASING",
			"AUDIT LOG FAULT", "APZ", "acs_alogmain/Alarm", "", "");
	AEHEVENTMESSAGE ("acs_alogmaind", 9928, "CEASING",
			"DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
	AEHEVENTMESSAGE ("acs_alogmaind", 9927, "CEASING",
			"DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
	return cerSuccess;
}


void acs_alog_cmdHandler::ClearExcludedList()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);
	m_PMMLcmds.clear();
	m_PMMLprts.clear();
	m_PUSRs.clear();
}


enumCmdExecuteResult acs_alog_cmdHandler::UpdatePasswordPresent(bool value)
{
	PlogIsAlive = value;
	ClearExcludedList();
	protectedPassword = "";
	return cerSuccess;
}


enumCmdExecuteResult acs_alog_cmdHandler::deleteAllPlogFiles()
{

	enumCmdExecuteResult ExecResult = cerSuccess;

	std::string  fName;
	std::string audit_dir(PathPAudit);
	DIR  *handle;
	struct dirent  *entry;
	if ((handle = opendir (audit_dir.c_str())) == NULL)  ExecResult = cerOtherError;
	else
	{
		while ((entry = readdir(handle)) != NULL)
		{
			fName = entry->d_name;

			if (fName.substr(0,7).compare("paudit.") == 0  ||  fName.substr(0,9).compare("PLOGFILE_") == 0  ||  fName.substr(0,9).compare("PLogfile_") == 0)
			{
					std::string fNameToDelete(PathPAudit);
					fNameToDelete += "/";
					fNameToDelete += fName;
					unlink(fNameToDelete.c_str());
			}
		}
		closedir (handle);
	}

	return ExecResult;
}


enumCmdExecuteResult acs_alog_cmdHandler::InitATTRFILEP()
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	ACS_ALOG_AttrFileRecord m_AttrFileRecord;
	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/ATTRFILEP";

	if (!checkFile(szFileName.c_str(), false))
	{
		//m_nRetCode = -1;
		m_hFile = ACE_OS::open(szFileName.c_str(),  O_WRONLY | O_TRUNC);

		m_AttrFileRecord.status = activee;
		m_AttrFileRecord.FileSize = 100;     // MB
		(*m_AttrFileRecord.FileDest) = '\0';

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			//ACE_OS::close(m_hFile);
			return cerFileOpenError;
		}
		int dwBytesWritten;
		dwBytesWritten = ACE_OS::write(m_hFile,
				(void*)&m_AttrFileRecord,
				sizeof(ACS_ALOG_AttrFileRecord));

		if (dwBytesWritten == 0)
		{
			ACE_OS::close(m_hFile);
			//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
			//Unlock();
			return cerFileWriteError;
		}

		ACE_OS::close(m_hFile);
		return cerSuccess	;
	}
	return cerSuccess	;
}
