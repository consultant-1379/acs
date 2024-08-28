//******************************************************************************
//
// NAME
// acs_rtr_fileBuild.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// DOCUMENT NO
//
//
// DESCRIPTION
// This file implements everything concerning files such as
// creating files, storing data in files, deleting files, renaming
// files and restoring files after a process restart.
//
// CHANGES
// RELEASE REVISION HISTORY
// DATE		NAME			DESCRIPTION
//******************************************************************************
#include <time.h>
#include <string>
#include <stdlib.h>
#include <queue>
#include <list>
#include <map>
#include <aes_ohi_filehandler.h>
#include "acs_rtr_filereport.h"
#include "acs_rtr_msread.h"
#include "acs_rtr_global.h"
#include "acs_rtr_global.h"
#include "acs_rtr_hmacmd5.h"
#include "acs_rtr_filebuild.h"
#include "acs_rtr_systemconfig.h"

#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

ACS_RTR_TRACE_DEFINE(ACS_RTR_FILE)

//----------------
// Constructor
//----------------
RTRfile::RTRfile(
		string msName,
		unsigned short recsize,
		const RTRMS_Parameters* rtrParams,
		const HMACMD5_keys* rtrKeys,
		string destDir,
		string repDir,
		string REFfile,
		unsigned short cpSystemId)
://_fileBasedjobDN(filebasedjobDN),
		m_msName(msName),
		forcedEnd(false),
		_restored(false),
		fh(ACE_INVALID_HANDLE),
		fsize(0),
		hMD5(0),
		cdrCounter(0),
		noOfRecords(NULL)
{
	ACS_RTR_TRACE_FUNCTION;
	(void)recsize;
	_destDir = destDir;
	_repDir = repDir;
	_REFfile = REFfile;


	_fileHoldTime = rtrParams->FiFileHoldTime;
	hmacMD5On = rtrParams->HmacMd5;
	cdrCountingOn = rtrParams->FiCdrFormat;

	RTRpars.OutputType = rtrParams->OutputType;
	strncpy(RTRpars.TransferQueue, rtrParams->TransferQueue, sizeof(RTRpars.TransferQueue) - 1);
	RTRpars.BlockLength = rtrParams->BlockLength;
	RTRpars.BlockLengthType = rtrParams->BlockLengthType;
	RTRpars.BlockHoldTime = rtrParams->BlockHoldTime;
	RTRpars.BlPaddingChar = rtrParams->BlPaddingChar;
	RTRpars.FileSize = rtrParams->FileSize;
	RTRpars.FiFileHoldTime = rtrParams->FiFileHoldTime;
	strncpy(RTRpars.GenerationTime, rtrParams->GenerationTime, sizeof(RTRpars.GenerationTime) - 1);
	strncpy(RTRpars.ResetFlag, rtrParams->ResetFlag, sizeof(RTRpars.ResetFlag) - 1);
	RTRpars.FixedFileRecords = rtrParams->FixedFileRecords;
	RTRpars.FileRecordLength = rtrParams->FileRecordLength;
	RTRpars.FiPaddingChar = rtrParams->FiPaddingChar;
	RTRpars.FiCdrFormat = rtrParams->FiCdrFormat;
	RTRpars.HmacMd5 = rtrParams->HmacMd5;
	strncpy(RTRpars.FiKey, rtrParams->FiKey, sizeof(RTRpars.FiKey) - 1);
	RTRpars.MinFileSize = rtrParams->MinFileSize;
	strncpy(RTRpars.pad, rtrParams->pad, sizeof(RTRpars.pad));

	m_cpSystemId = cpSystemId;
	m_isBC = ACS_RTR_SystemConfig::instance()->isMultipleCPSystem();
	this->CreateFileName(fname);
	string Absfname=_destDir;
	Absfname.append("/");
	Absfname.append(fname);

	//CHANGE: Adding check on handle open
	int retry=0;
	while((ACE_INVALID_HANDLE == fh) && (retry < 15))
	{
		fh = ACE_OS::open(Absfname.c_str(), O_RDWR|O_CREAT ,ACE_DEFAULT_OPEN_PERMS);
		retry++;
	}

	//CHANGE: Adding check on file handle
	if (fh == ACE_INVALID_HANDLE)
	{
		ACS_RTR_TRACE_MESSAGE("Error: Cannot open file \"%s\". errno: %d", Absfname.c_str(), ACE_OS::last_error());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Cannot open file \"%s\". errno: %d", Absfname.c_str(), ACE_OS::last_error());

		// We failed to open the disk-file
		ACS_RTR_FileNumberMgr::instance()->decrease(msName.c_str(), cpSystemId);
	}


	fileSpaceLeft = (RTRpars.FileSize << 10);

	// fixed file records (YES/NO).
	if (rtrParams->FixedFileRecords == FIXED) {
		recordPadding = true;
	}
	else {
		recordPadding = false;
	}
	hmacMD5On=RTRpars.HmacMd5;
	unsigned char* skey;
	if (hmacMD5On) {
		unsigned char * ipad = const_cast<unsigned char*>(&(rtrKeys->ipad[0]));
		unsigned char* opad = const_cast<unsigned char*>(&(rtrKeys->opad[0]));
		if (RTRpars.HmacMd5)
			skey = const_cast<unsigned char*>(&(rtrKeys->skey[0]));
		else
			skey = reinterpret_cast<unsigned char* >(&(RTRpars.FiKey[0]));
		hMD5=new ACS_RTR_HMACMD5(ipad, opad,skey);
		if (hMD5 && !hMD5->getState()) {
			string err;
			hMD5->getError(err);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s",err.c_str());
		}
	}

	time(&_fileCreationTime);
}
//------------------------------------------------------------------
// Constructor, restore object after a process restart
//------------------------------------------------------------------
RTRfile::RTRfile(string f,
		string destDir,
		string repDir,
		string REFfile,
		unsigned short cpSystemId)
:_restored(true),
 fh(ACE_INVALID_HANDLE),
 fsize(0),
 hMD5(0),
 cdrCounter(0),
 cdrCountingOn(false),
 hmacMD5On(false),
 noOfRecords(0ULL)
{
	ACS_RTR_TRACE_FUNCTION;
	_destDir = destDir;
	_repDir = repDir;
	if (REFfile.empty()) _REFfile = _destDir + "/REF-RTR-0000-0000.0000";
	else _REFfile = REFfile;
	//       memset(_fileBasedjobDN,'\0',sizeof(_fileBasedjobDN));
	m_cpSystemId = cpSystemId;

	m_isBC = ACS_RTR_SystemConfig::instance()->isMultipleCPSystem();

	fname = f;
	string Absfname = _destDir;
	Absfname.append("/");
	Absfname.append(fname);

	//CHANGE: Adding check on handle open
	int retry=0;
	while((ACE_INVALID_HANDLE == fh) && (retry < 15))
	{
		fh=ACE_OS::open(Absfname.c_str(), O_RDWR, ACE_DEFAULT_OPEN_PERMS); 
		retry++;
	}	

	//CHANGE: Adding check for file handle
	if(ACE_INVALID_HANDLE != fh)
	{
		ACE_INT32 fsize = ACE_OS::filesize(fh);

		if (fsize == -1)
		{
			ACS_RTR_TRACE_MESSAGE("Error: Failed in get filesize from \"%s\". errno: %d", Absfname.c_str(), ACE_OS::last_error());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed in get filesize from \"%s\". errno: %d", Absfname.c_str(), ACE_OS::last_error());

			fsize=0;
		}
		ACE_OS::close(fh);
		fh=ACE_INVALID_HANDLE;
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("Error: Cannot open \"%s\". errno: %d", Absfname.c_str(), ACE_OS::last_error());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Cannot open \"%s\". errno: %d", Absfname.c_str(), ACE_OS::last_error());
	}

	// safe initializations
	time(&_fileCreationTime);
	_fileHoldTime = DEF_FIFILEHOLDTIME;
	memset(&RTRpars, 0, sizeof(RTRpars));
	fileSpaceLeft = 0;
	forcedEnd = false;
	recordPadding = true;
}


//---------------
// Destructor
//---------------
RTRfile::~RTRfile()										
{
	ACS_RTR_TRACE_FUNCTION;
	// The physical file should not be removed, GOH owns the file
	// and is reponsible for it.
	if (ACE_INVALID_HANDLE != fh)
	{
		ACE_OS::close(fh);
		fh = ACE_INVALID_HANDLE;
	}	
	if (hMD5) delete hMD5;
}

//-----------------------------------------------------------------------------------------------------------
// fileClose
// Flush the file content to disk to make it safely stored.
// The file name is also changed from:
// BLD-mmdd-hhmm.nnnn to RTR-mmdd-hhmm.nnnn(or to RTR-mmdd-hhmm.nnnn.ttttt)
// return values:
//   true : OK
//   false: flush failed
//-----------------------------------------------------------------------------------------------------------
bool RTRfile::fileClose(MSread* msptr,ACE_TCHAR* errstr, size_t errLen, bool forced, bool & diskProblem)
{
	ACS_RTR_TRACE_FUNCTION;
	forcedEnd = forced;
	bool ret = true;
	diskProblem = false;

	if ((fsize==0) && (!forcedEnd)) 
	{
		time_t closeTime;
		time(&closeTime);
		long long int emptyfileOpened=difftime(closeTime,_fileCreationTime);
		long long int tmpFileHoldTime = static_cast<double>(_fileHoldTime);
		if ( abs(emptyfileOpened-tmpFileHoldTime) > 100 ) 
		{
			// report to the log that the empty file was not opened
			// as long as it should.
			ACE_TCHAR repstr[512] = {0};
			ACE_OS::snprintf(repstr, sizeof(repstr) - 1, "Closing empty file not opened yet. fileHold par:%d seconds, file opened:%lli seconds", _fileHoldTime,emptyfileOpened);
			ACS_RTR_TRACE_MESSAGE("[CP id: %u, CHS: %s, file name: %s] %s", m_cpSystemId, m_msName.c_str(), fname.c_str(), repstr);
			ACS_RTR_LOG(LOG_LEVEL_WARN, "[CP id: %u, CHS: %s, file name: %s] %s", m_cpSystemId, m_msName.c_str(), fname.c_str(), repstr);
		}
	}

	// if HMACMD5 is activated, put the hash last in the file.
	// Get the attributes from File/Block based object and set accordingly.
	if (hmacMD5On && fsize>0) 
	{
		string err;
		if (hMD5->endData()) 
		{
			unsigned char hashValue[16];
			if (!hMD5->getHash(hashValue))
			{			
				hMD5->getError(err);
				strncpy(errstr,err.c_str(), errLen - 1);	
				ret=false;
			}
			else
			{
				//CHANGE: Adding check for file handle
				int nobw = 0;
				if(ACE_INVALID_HANDLE != fh)
				{
					nobw = ACE_OS::write(fh, hashValue,  sizeof(hashValue));
				}

				if (nobw != 16 )
				{
					strncpy(errstr,"cannot write hash to file", errLen - 1);
					ret= false;
				}
			}
		}
		else
		{
			hMD5->getError(err);
			strncpy(errstr,err.c_str(), errLen - 1);
			ret= false;
		}
	}

	if (ret) 
	{
		if (flushBLDfile(errstr, errLen)) 
		{
			unsigned char transactionIdData[64] = {0};
			strncpy((char *)transactionIdData, fname.c_str(), sizeof(transactionIdData) - 1);

			if (msptr != 0) 
			{
				//it should be never NULL
				int res = msptr->commit(noOfRecords, transactionIdData, strlen((const char *)transactionIdData)+1);
				if (!res) 
				{ 
					// Understand the Result Code
					switch (msptr->getLastError())
					{
					case ACS_ACA_MessageStoreClient::NoMessageToCommit:
					case ACS_ACA_MessageStoreClient::SendPrimitiveFailed:
						ret = true;
						break;
					default:
						ret = false;
					}

					ACE_OS::snprintf(errstr, errLen - 1, "Commit failed: %d - %s. Returning %s", msptr->getLastError(), msptr->getLastErrorName().c_str(), (ret ? "success. Not an RTR blocking error" : "error"));
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", errstr);
					ACS_RTR_TRACE_MESSAGE("%s", errstr);
				}
			}
		} 
		else 
		{
			diskProblem = true;
			ret = false;
		}
	}

	return ret;
}

//--------------------------------------------------------------------
// store
// Add data to an open file (BLD-mmdd....).
// return values:
//   true : ok, data stored
//   false: data not stored, errstr contains a fault string.
//--------------------------------------------------------------------
bool RTRfile::store(ACE_UINT32 ln, unsigned char* buf, ACE_INT32 noOfCDRs, ACE_TCHAR* errstr, size_t errLen)
{
	ACS_RTR_TRACE_FUNCTION;
	bool ret = false;
	ACE_INT32 NumberOfBytesWritten = 0;
	ACE_INT32 noOfPadds = 0; 

	if (recordPadding) 
	{
		if ((noOfPadds=RTRpars.FileRecordLength-ln)<0) 
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s","MS record size larger than in RTR");
		}
		else 
		{
			unsigned char* startPadd=buf+ln;
			memset(startPadd,RTRpars.FiPaddingChar,noOfPadds);
			ln=RTRpars.FileRecordLength;
		}
	}

	//CHANGE: Adding check for file handle
	if(ACE_INVALID_HANDLE != fh)
	{
		NumberOfBytesWritten = ACE_OS::write(fh, buf,  ln);
	}

	if(NumberOfBytesWritten > 0 && NumberOfBytesWritten < (ACE_INT32)ln)	
	{
		ACE_OS::snprintf(errstr, errLen - 1, "Partial store failure nobw:%d length: %d lasterror: %d\n", NumberOfBytesWritten,ln,ACE_OS::last_error());

		// Move the file pointer backwards to its previous position
		off_t lastBytePos=ACE_OS::lseek(fh,NumberOfBytesWritten*(-1),SEEK_END);

		//truncate the file to avoid dirty bytes.
		ACE_OS::ftruncate(fh,lastBytePos);

	}  
	else if ((unsigned)NumberOfBytesWritten == ln) 
	{
		// if HMACMD5 is activated, add data to the hash object.

		if (hmacMD5On) hMD5->addData(ln,buf);
		fileSpaceLeft = fileSpaceLeft - NumberOfBytesWritten;
		fsize = fsize + NumberOfBytesWritten;
		cdrCounter += noOfCDRs;
		ret = true;
		++noOfRecords;
	}
	return ret;
}

//----------------------------------------------------------------------
// flushBLDfile
// Flush the file content to disk to make it safely stored.
// The file name is also changed from:
// BLD-mmdd-hhmm.nnnn to RTR-mmdd-hhmm.nnnn.
// return values:
//   true : OK
//   false: flush or rename of file failed
//----------------------------------------------------------------------
bool RTRfile::flushBLDfile(ACE_TCHAR* errstr, size_t errLen)
{
	ACS_RTR_TRACE_FUNCTION;
	bool ret=false;

	if((ACE_INVALID_HANDLE != fh) && (ACE_OS::fsync(fh) == -1))
	{
		ACE_OS::snprintf(errstr, errLen-1, "FlushFileBuffers err %d\n",ACE_OS::last_error());
		//CHANGE: Adding check on file handle
		ACE_OS::close(fh);
		fh = ACE_INVALID_HANDLE;
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Cannot flush file %s/%s. Error: %s", _destDir.c_str(), fname.c_str(), errstr);
		ACS_RTR_TRACE_MESSAGE("ERROR: Cannot flush file %s/%s. Error: %s", _destDir.c_str(), fname.c_str(), errstr);
	}
	else
	{
		// rename the file
		std::string bldname = _destDir + "/" + fname;
		fname.replace(0,3,"RTR");
		if (cdrCountingOn)
		{
			char tmpCDRValue[32] = {0};
			snprintf(tmpCDRValue, sizeof(tmpCDRValue) - 1, ".%.5i", cdrCounter);
			fname.append(tmpCDRValue);

			ACS_RTR_TRACE_MESSAGE("cdr counting enabled of file %s. Adding counter: %d", bldname.c_str(), cdrCounter);
		}

		std::string newFileName = _destDir + "/"+fname;

		//CHANGE: Adding check on file handle, earlier no validations
		if (ACE_INVALID_HANDLE != fh)
		{
			ACE_OS::close(fh);
			fh = ACE_INVALID_HANDLE;
		}

		ret=true;

		if (rename(bldname.c_str(),newFileName.c_str()))
		{
			char errorText[512] = {0};
			strerror_r(errno, errorText, sizeof(errorText) - 1);
			ACE_OS::snprintf(errstr, errLen - 1, "Cannot rename <%s> to <%s>. Error code: <%d>. Description: <%s>",	bldname.c_str(), newFileName.c_str(), ACE_OS::last_error(), errorText );

			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", errstr);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", errstr);

			ret = false;
		}
	}
	return ret;
}



//-----------------------------------------------------------------------------
// getFileName
// Returns the file name, only the file name not the whole path.
//-----------------------------------------------------------------------------
const string& RTRfile::getFileName(void) const								
{
	return fname;
}

//----------------------------
// getFileReportedName
//----------------------------
const string& RTRfile::getFileReportedName(void) const
{
	return _reportedFileName;
}

//---------------------------------------------------
// getFileReportedPath
// Returns the complete file reported path
//---------------------------------------------------
std::string RTRfile::getFileReportedPath(void)
{
	string reppath = _repDir + "/" + fname;
	return reppath;
}

//--------------------------------------------------------------------------------
// enoughSpace
// Checks if there are enough room in the file.
// return values:
//   true : ok
//   false: no, 'bufln' number of bytes would exceed the file size.
//--------------------------------------------------------------------------------
bool RTRfile::enoughSpace(const ACE_UINT32 bufln) const
{
	ACE_UINT32 requiredLn = bufln;
	if (recordPadding && (RTRpars.FileRecordLength > bufln)) 
	{
		requiredLn=RTRpars.FileRecordLength;
	}

	if (requiredLn > fileSpaceLeft) 
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------
// fileOpened
// Checks if the file is opened..
// return values:
//   true : ok
//   false: no, there is no handle to the file, i.e not opened.
//-----------------------------------------------------------------------
bool RTRfile::fileOpened(void) const 
{
	if (fh==ACE_INVALID_HANDLE)
	{
		return false;
	}
	else
		return true;
}

//------------------------------------------------------------------
// CreateFilename
// Creates a file with name BLD-MMDD-HHMM.nnnn .
// return values:
// -
//------------------------------------------------------------------
void RTRfile::CreateFileName(string& filename)
{
	ACE_TCHAR temp[10];
	string parts[7];
	time_t now = time(NULL);
	struct tm tempTimeStruct;
	struct tm * lpSystemTime = ::localtime_r(&now, &tempTimeStruct);

	lpSystemTime->tm_year=lpSystemTime->tm_year + 1900;
	lpSystemTime->tm_mon =lpSystemTime->tm_mon+1;

	filename="";
	parts[0]="BLD-";

	parts[1]=ACE_OS::itoa(lpSystemTime->tm_mon,temp,10);
	if (parts[1].length()==1) parts[1].insert(0,"0",1);

	parts[2]=ACE_OS::itoa(lpSystemTime->tm_mday,temp,10);
	if (parts[2].length()==1) parts[2].insert(0,"0",1);

	parts[3]=ACE_OS::itoa(lpSystemTime->tm_hour,temp,10);
	parts[3].insert(0,"-",1);
	if (parts[3].length()==2) parts[3].insert(1,"0",1);		
	parts[4]=ACE_OS::itoa(lpSystemTime->tm_min,temp,10);
	if (parts[4].length()==1) parts[4].insert(0,"0",1);

	ACE_UINT32 fileNum = ACS_RTR_FileNumberMgr::instance()->get(m_msName.c_str(), m_cpSystemId);
	parts[5]=ACE_OS::itoa(fileNum,temp,10);
	if (parts[5].length()==1) parts[5].insert(0,"000",3);
	else if (parts[5].length()==2) parts[5].insert(0,"00",2);
	else if (parts[5].length()==3) parts[5].insert(0,"0",1);
	parts[5].insert(0,".",1);	

	for (ACE_INT32 iii=0;iii<6;iii++)
	{
		filename.append(parts[iii]);
	}
}


//--------------------------------------
// deleteFile
// Deletes the BLD or RTR file.
//--------------------------------------
void RTRfile::deleteFile(void)
{
	ACS_RTR_TRACE_FUNCTION;

	//CHANGE: Adding check for valid handle
	//if (fh!=ACE_INVALID_HANDLE) 
	if (ACE_INVALID_HANDLE != fh)
	{ 
		// BLD file can only be open here.
		ACE_OS::close(fh);
		fh = ACE_INVALID_HANDLE;
		ACS_RTR_FileNumberMgr::instance()->decrease(m_msName.c_str(), m_cpSystemId);
	}
	string tempstr = _destDir + "/" + fname.c_str();
	ACE_OS::unlink(tempstr.c_str());
}

//--------------------------------------------------------------------------------------
// ResetREFfileName
// Sets the file name of the REF file to 'REF-RTR-0000-0000.0000'.
//--------------------------------------------------------------------------------------
void RTRfile::ResetREFfileName(void)
{
	ACS_RTR_TRACE_FUNCTION;
	string initName = _destDir + "\\REF-RTR-0000-0000.0000";
	char cmdline[256];

	sprintf(cmdline, "mv %s %s", _REFfile.c_str(), initName.c_str());
	system(cmdline);

	_REFfile = initName;
}

//--------------------------------------------------------------------------------------
// MoveRTRfile
// Moves an RTR file either to the 'Reported' directory or back to the
// directory where unreported files are located.
// return values:
//   true:  ok
//   false: move failed, err contains fault code
//--------------------------------------------------------------------------------------
bool RTRfile::MoveRTRfile(int direction, unsigned int & err)
{	
	ACS_RTR_TRACE_FUNCTION;
	ACE_INT32 rc = 1; 
	bool ret = true;
	string dataFileName="";
	dataFileName =_destDir + "/" +fname;
	string reportedFileName = "" ;
	err = 0;

	if (m_isBC) { //if it's Multiple CP system
		string cpname;
		bool csOk = false;
		ACE_INT32 retry = 0;
		do {
			if (retry > 5) return false;
			cpname = ACS_RTR_SystemConfig::instance()->cs_getDefaultCPName(m_cpSystemId);
			if (ACS_RTR_SystemConfig::instance()->isValidCPName(cpname.c_str())) csOk = true;
			else {
				retry++;
				ACE_TCHAR tracep[512] = {0};
				const ACE_TCHAR * errText = ACS_RTR_SystemConfig::instance()->lastErrorDescription(); 
				ACE_INT32 errCode = ACS_RTR_SystemConfig::instance()->lastError(); 
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "MoveRTRfile '%s' failed. GET CS DEFAULT NAME FAILED. Error Code:%d, Error Description:%s", fname.c_str(), errCode, errText);
				ACS_RTR_LOG(LOG_LEVEL_ERROR,"%s",tracep);
				ACE_Time_Value safeSleep(0,500000);
				ACE_OS::sleep(safeSleep);
			}
		} while (!csOk);

		_reportedFileName = cpname + '+' + fname;
		reportedFileName = _repDir + "/" + cpname + '+' + fname;
	}
	else // else it's single CP system
	{
		_reportedFileName = fname;
		reportedFileName=_repDir + "/" + fname;
	}

	//Move Files
	if (direction == toReportedDir) 
	{
		if(dataFileName.find("REF") != string::npos)		
		{
			ACS_RTR_TRACE_MESSAGE("Error: Found REF file returning false. dataFileName is \"%s\". reportedFileName is \"%s\"", dataFileName.c_str(), reportedFileName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Found REF file returning false. dataFileName is \"%s\". reportedFileName is \"%s\"", dataFileName.c_str(), reportedFileName.c_str());
			err = REF_FILE_ERROR ;//found REF file
			return false; 
		}
		rc =ACE_OS::rename(dataFileName.c_str(),reportedFileName.c_str());
		ACS_RTR_TRACE_MESSAGE("Rename: old name = \"%s\", new name = \"%s\". Result: %d",dataFileName.c_str(), reportedFileName.c_str(), rc);
	}
	else 
	{
		if (m_isBC) { //remove "CP<name>+" from reportedFileName
			string app;
			ACE_INT32 pos = reportedFileName.find("+");
			if (pos != -1) {
				app = reportedFileName.substr(pos+1);
				dataFileName = _destDir + "/" + app;
			}
		}
		if(dataFileName.find("REF") != string::npos)
		{
			ACS_RTR_TRACE_MESSAGE("Error: Found REF file. Returning false. dataFileName = \"%s\"", dataFileName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Found REF file. Returning false. dataFileName = \"%s\"", dataFileName.c_str());

			err = REF_FILE_ERROR ;//found REF file
			return false;
		}
		rc = ACE_OS::rename(reportedFileName.c_str(),dataFileName.c_str());		
		ACS_RTR_TRACE_MESSAGE("Rename: old name = \"%s\", new name = \"%s\". Result: %d",reportedFileName.c_str(), dataFileName.c_str(), rc);
	}

	if (rc < 0 ) { // Move Failed
		err = ACE_OS::last_error();
		if (err != ERROR_ALREADY_EXISTS) {
			ACE_TCHAR tracep[512] = {0}; 
			ACE_TCHAR str_direction[256] = {0};
			if (direction == toReportedDir) ACE_OS::snprintf(str_direction, sizeof(str_direction), "to reported directory");
			else ACE_OS::snprintf(str_direction, sizeof(str_direction), "from reported directory");
			ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "MoveRTRfile '%s' failed. Direction: %s.\n      Error Code:%d. MoveFile(%s, %s)\n", fname.c_str(), str_direction, err, reportedFileName.c_str(), dataFileName.c_str());

			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			ret=false;
		}
	}
	return ret;
}

//---------------------------------------------------------------------------------
// refreshRefFile
// GohFileRepoter thread is responsible for changinh REF* name.
//---------------------------------------------------------------------------------
bool RTRfile::refreshRefFile(unsigned int & err)
{
	ACS_RTR_TRACE_FUNCTION;

	bool ret = true;

	std::list<string> lstFiles;
	bool res = search_for_file(_destDir, lstFiles, "REF");

	std::string newREFfileName = _destDir + "/REF-" + fname;

	if(res && lstFiles.size() > 0)
	{
		std::string oldpath = _destDir + "/" + lstFiles.front();
		if (rename(oldpath.c_str(), newREFfileName.c_str()))
		{
			//Error
			err = errno;
			ACS_RTR_TRACE_MESSAGE("ERROR: Cannot rename <%s> to <%s> errno: %d", oldpath.c_str(), newREFfileName.c_str(), ACE_OS::last_error());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Cannot rename <%s> to <%s> errno: %d", oldpath.c_str(), newREFfileName.c_str(), ACE_OS::last_error());
			ret = false;
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("REF file renamed. Rename <%s> to <%s>", oldpath.c_str(), newREFfileName.c_str());
		}
	}

	if (lstFiles.size() > 1)
	{
		ACS_RTR_TRACE_MESSAGE("WARNING: More than one REF file found in <%s>", _destDir.c_str());
		ACS_RTR_LOG(LOG_LEVEL_WARN, "More than one REF file found in <%s>", _destDir.c_str());
	}


	return ret;
}
//--------------------------------------------------
// removeRefFile
// Searches and deletes all the REF files.
//--------------------------------------------------
void RTRfile::list_all_files(const string& sStartDir,
		std::list<string>& lstFound,bool bRecursive)
{
	DIR* pDir = opendir ( sStartDir.c_str());

	if ( !pDir)
		return;

	dirent* pEntry;
	pEntry = readdir (pDir);
	while (pEntry != 0)
	{

		if ( DT_DIR & pEntry->d_type && strcmp ( pEntry->d_name, ".") && strcmp ( pEntry->d_name, "..") && bRecursive) {

			string sSubDir = sStartDir + string ( "/") + string ( pEntry->d_name);

			list_all_files( sSubDir, lstFound,true);

		}

		string sFound = sStartDir + string ( "/") + string ( pEntry->d_name);

		lstFound.push_back ( sFound);
		pEntry = readdir (pDir);
	}
	closedir(pDir);
}


void RTRfile::removeRefFile()
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_TCHAR searchDir[FILENAME_MAX] = {0};

	ACE_OS::snprintf(searchDir, sizeof(searchDir) - 1, "%s\\REF*", _destDir.c_str());

	std::list<string> lstFiles;
	bool res = search_for_file(_destDir, lstFiles, "REF");
	std::string removepath = "";
	if(res && lstFiles.size() > 0)
	{
		do
		{
			removepath = _destDir + "/" + lstFiles.front();
			remove(removepath.c_str());
			lstFiles.pop_front();
		}
		while(lstFiles.size() > 0);
	}
}

//--------------------------------
// getFSize
// Gets the size of the file.
//--------------------------------
ACE_UINT32 RTRfile::getFSize(void)
{
	return fsize;
}


bool RTRfile::search_for_file(const string& sStartDir, std::list<string>& filesFound, std::string compareWith)
{
	DIR* pDir = opendir ( sStartDir.c_str());

	if ( !pDir)
		return false;

	std::string search = compareWith;
	dirent* pEntry;
	pEntry = readdir (pDir);
	while (pEntry != 0)
	{
		string sFound = string (pEntry->d_name);
		if(sFound.find(search) != string::npos)
		{
			filesFound.push_back(sFound);
		}
		pEntry = readdir (pDir);
	}
	closedir(pDir);
	return true;
}

