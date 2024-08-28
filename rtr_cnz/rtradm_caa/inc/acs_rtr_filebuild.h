//******************************************************************************
//
// NAME
// acs_rtr_filebuild.h
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
// AUTHOR 
// 2012-12-06 by XLANSRI
//******************************************************************************

#ifndef ACS_RTR_fileBuild_h
#define ACS_RTR_fileBuild_h

//#include <winsock2.h>

#include "ace/ACE.h"
#include <string>
#include "ACS_CS_API.h"
#include "acs_rtr_lib.h"
#include "acs_rtr_defaultvalues.h"

#include "acs_rtr_events.h"

#include "acs_rtr_systemconfig.h"
#include "acs_rtr_filenumbermgr.h"
#include "ace/ACE.h"

using namespace std;

enum {
	toReportedDir,
    fromReportedDir
};

class MSread;  // forward declaration
class ACS_RTR_HMACMD5;

class RTRfile
{
public:

	// constructor 
	RTRfile(
			string msName,
			unsigned short recsize,
			const RTRMS_Parameters* rtrParams,
			const HMACMD5_keys* rtrKeys,
			string destDir,
			string repDir,
			string REFfile,
			unsigned short cpSystemId=~0 );

	// constructor, restore after process restart
	RTRfile(std::string fname,
				std::string destDir,
				std::string repDir,
				std::string REFfile,
				unsigned short cpSystemId=~0);
	
	// destructor
	virtual ~RTRfile();


	bool store(ACE_UINT32 ln, unsigned char* buf, ACE_INT32 noOfCDRs, ACE_TCHAR* err, size_t errLen);

	bool fileClose(MSread* msptr,ACE_TCHAR* errstr, size_t errLen, bool forced, bool & diskProblem);

	bool flushBLDfile(ACE_TCHAR* err, size_t errLen);
	const std::string& getFileName(void) const;
	const std::string& getFileReportedName(void) const;

	std::string getFileReportedPath(void);

	bool enoughSpace(const ACE_UINT32 bufln) const;
	bool fileOpened(void) const;
	bool refreshRefFile(unsigned int & err);
	void CreateFileName(std::string& filename);
	void deleteFile(void);
	void ResetREFfileName(void);
	bool MoveRTRfile(int direction, unsigned int& err);
	void list_all_files(const string& sStartDir,std::list<string>&lstFound, bool bRecursive);
	bool search_for_file(const string& sStartDir, std::list<string>& filesFound, std::string compareWith = "REF");
	ACE_UINT32 getFSize(void);

	inline ACE_INT32 getFileHoldTime() { return RTRpars.FiFileHoldTime; }

	inline bool isRestored() { return _restored; }

	std::string _destDir;  // the 'dataFiles' directory
	std::string _repDir;  // the 'Reported' directory
	std::string _REFfile;  // the RTR REF file

private:
	void removeRefFile();

	RTRMS_Parameters RTRpars;

	unsigned short m_cpSystemId;
	bool m_isBC;
	string m_msName;

	bool forcedEnd;

	bool _restored;

	ACE_HANDLE fh;
	std::string fname;
	std::string _reportedFileName;
	//unsigned int fileSpaceLeft;
        ACE_UINT32 fileSpaceLeft;
	bool recordPadding;
	ACE_INT32 fsize;

	// HF76580 empty files close too early.
	//int _fileHoldTime;
        ACE_INT32 _fileHoldTime;
	time_t _fileCreationTime; 

	ACS_RTR_HMACMD5* hMD5;
	//int cdrCounter;
        ACE_INT32 cdrCounter;
	bool cdrCountingOn;
	bool hmacMD5On;

	unsigned long long noOfRecords;
};

#endif
