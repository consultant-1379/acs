//******************************************************************************
//
// NAME
// acs_rtr_defaultValues.h
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
// AUTHOR 
// 2012-12-05 by XSAMECH	
//
// CHANGES
//
// RELEASE REVISION HISTORY
//
//	REV NO	DATE 		NAME			DESCRIPTION
//	R1a	051212		XSAMECH			First Release
//
//******************************************************************************


#ifndef defaultValues
#define defaultValues

#include "ace/ACE.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#define MSG_STOR_NAME "messageStoreName"
using namespace std;

// default parameter values for RTRBIN.
const int DEF_BLOCKLENGTH              = 4096;
const int DEF_BLOCKLENGTHTYPE		= 0;
const int DEF_BLOCKHOLDTIME            = 60;
const int DEF_BLPADDINGCHAR            = 0;
const int DEF_FILESIZE                 = 1024;
const int DEF_FIFILEHOLDTIME           = 300;
const char* const DEF_GENERATIONTIME   = "00:00";
const char* const DEF_RESETFLAG        = "NO";
const int DEF_FIXEDFILERECORDS		= 0;
const int DEF_FILERECORDLENGTH         = 2048;
const int DEF_FIPADDINGCHAR            = 0;
const bool DEF_CDRFORMATFLAG           = false;
const bool DEF_HMACMD5FLAG             = false;
const char* const DEF_KEY              = "UNDEFINED";
const int DEF_MINFILESIZE              = 0;

// mapping indexes for parameters
const int IND_BLOCKLENGTH              = 0;
const int IND_BLOCKHOLDTIME            = 4;
const int IND_FILESIZE                 = 8;
const int IND_FIFILEHOLDTIME           = 12;
const int IND_BLPADDINGCHAR            = 16;
const int IND_BLOCKLENGTHTYPE          = 20;
const int IND_GENERATIONTIME           = 40;
const int IND_RESETFLAG                = 60;
const int IND_FIXEDFILERECORDS         = 80;
const int IND_FILERECORDLENGTH         = 120;
const int IND_FIPADDINGCHAR            = 124;
const int IND_CDRFORMAT                = 0;
const int IND_HMACMD5                  = 2;
const int IND_KEY                      = 4;
const int IND_MINFILESIZE              = 40; //Changed for HMAC problem CN-I 907 by XCSVAAN

const int NO_DEFAULTFILES=2;
const char* const DEFAULTFILE[NO_DEFAULTFILES]={"/DefParameters","/DefParameters_1"};
const int DEF_DEFAULTPAR_FILESIZE=1024;
//const char* const messageStoreNameAttr[]={"messageStoreName"};
// max/min constants.
const int MAX_BLOCKLENGTH=32768;
const int MAX_BLOCKHOLDTIME=7200;
const int MAX_BLOCKPADDINGCHAR=255;
const int MAX_FILESIZE=32768;     //TR HN24042
const int MAX_FILEHOLDTIME=86400;
const int MAX_FILERECLENGTH=32768;     
const int MAX_FILEPADDINGCHAR=255;
const int MAX_MINFILESIZE=16777216; // Maximum value for minfilesize got changed to handle in terms of bytes for TR HH71761

const int MIN_BLOCKLENGTH=512;
const int MIN_BLOCKHOLDTIME=1;
const int MIN_BLOCKPADDINGCHAR=0;
const int MIN_FILESIZE=64;
const int MIN_FILEHOLDTIME=1;
const int MIN_FILERECLENGTH=512;
const int MIN_FILEPADDINGCHAR=0;
const int MIN_MINFILESIZE=0;

class defFile
{
public:
	defFile(const std::string RTRpath);
	~defFile();
	bool init(char* errstr);

	void getDefaultValues(struct RTRMS_Parameters& dpar);
	void setDefaultValue(parID p,unsigned int value);
	void setDefaultValue(parID p,const char* strval);

private:
	defFile(const defFile&);
	void map_pointers(int fnum);
	void setHardCodedValues(int fnum);
	std::string defFileName[NO_DEFAULTFILES];
	ACE_HANDLE  FileHandle[NO_DEFAULTFILES];
	ACE_HANDLE FileMap[NO_DEFAULTFILES];
	//BYTE* FileView[NO_DEFAULTFILES];
	char* FileView[NO_DEFAULTFILES];

	// map pointers to file 'DefaultValues'
	int* blockLength;
	int* blockHoldTime;
	int* fileSize;
	int* fileHoldTime;
	int* BlPaddingChar;
	int* BlockLengthType;
	char* GenerationTime;
	char* ResFlag;
	int* FixedFileRecords;
	int* fileRecordLength;
	int* FiPaddingChar;

	// map pointers to file 'DefaultValues_1'
	bool* FiCdrFormat;
	bool* HmacMd5;
	char* FiKey;
	int*  minFileSize;
};

#endif
