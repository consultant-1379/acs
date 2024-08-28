//******************************************************************************
//
// NAME
// acs_rtr_defaultValues.cpp
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
// 	2012-12-05 by xsamech
//
// DESCRIPTION
// tbd
// CHANGES
//
// RELEASE REVISION HISTORY
//
//	REV NO	DATE 		NAME			DESCRIPTION
//	R1A	051212		XSAMECH			FIRST RELEASE
//******************************************************************************

#include "ace/ACE.h"
#include <string>
#include <acs_rtr_lib.h>
#include <acs_rtr_defaultvalues.h>
#include <cstdio>
#include <iostream>
using namespace std;


//*************************************************************************
// Constructor
//*************************************************************************
defFile::defFile(const string RTRpath) : blockLength(0), blockHoldTime(0),fileSize(0),fileHoldTime(0), BlPaddingChar(0),BlockLengthType(0),GenerationTime(0), ResFlag(0), FixedFileRecords(0),fileRecordLength(0),FiPaddingChar(0),FiCdrFormat(0), HmacMd5(0),FiKey(0), minFileSize(0)
{
	for (int i=0;i<NO_DEFAULTFILES;i++)
	{
		defFileName[i]=RTRpath;
		defFileName[i].append(DEFAULTFILE[i]);
		FileHandle[i]=ACE_INVALID_HANDLE;
		FileMap[i]=ACE_INVALID_HANDLE;
                FileView[i]=NULL;
	}
}

//*************************************************************************
// Destructor
//*************************************************************************
defFile::~defFile()
{
	for (int i=0;i<NO_DEFAULTFILES;i++)
	{
		if (FileMap[i] != ACE_INVALID_HANDLE)
		  {
		    int sizet = (size_t)FileMap[i];
                    munmap((void *)FileMap[i],sizet);
		  }
		if (FileHandle[i] != ACE_INVALID_HANDLE)
		  {
		    ACE_OS::close(FileHandle[i]);	
		    FileHandle[i] = ACE_INVALID_HANDLE;
		  }	
	}
}

//*************************************************************************
// init
//
// Initialize the mapped default file(s).
//
//  return value:
//    true : ok.
//    false: something went wrong, errstr contains a fault string.
//*************************************************************************
bool defFile::init(char* errstr)										 
{ 
	bool ret=true;
	struct stat buf;	
	for (int i=0;i<NO_DEFAULTFILES;i++)
	{
		bool fileCreated=false;
		FileHandle[i]=ACE_OS::open(defFileName[i].c_str(),O_RDWR,ACE_DEFAULT_OPEN_PERMS);

		if (FileHandle[i] == ACE_INVALID_HANDLE)
		{
			// We failed to create/open the default file
			sprintf(errstr,"create default file failed: %d \n file :%s\n",
							ACE_OS::last_error(),
		                    defFileName[i].c_str());
			ret=false;
		}
		else
		{
			//Geting the file size
			ACE_INT32 result = stat(defFileName[i].c_str(), &buf);
			if( result != 0 )
			{
				return false; 
			}
			unsigned long fSize=buf.st_size;;

			if (fSize==0 || fSize!=(unsigned)DEF_DEFAULTPAR_FILESIZE)
			{
				fileCreated=true;
				ACE_INT32 nobw=0;
				unsigned char dummyBuf[DEF_DEFAULTPAR_FILESIZE];
				memset(dummyBuf,0,DEF_DEFAULTPAR_FILESIZE);
				
				nobw=ACE_OS::write(FileHandle[i],dummyBuf,DEF_DEFAULTPAR_FILESIZE);
				if (nobw<DEF_DEFAULTPAR_FILESIZE)
				{	
					// We failed to write to file.
					sprintf(errstr,"zeroize new default file failed: %d\n",ACE_OS::last_error());
					ret=false;
					break;
				}
				cout<<"zeroize new default file failed:"<<endl;
			}

			FileView[i] = reinterpret_cast<char*>(mmap(0,DEF_DEFAULTPAR_FILESIZE,PROT_READ,MAP_SHARED ,(int)FileHandle[i],0));
 
			if (FileView[i] == NULL)
			{
				// Failed to map the file.
				sprintf(errstr,"map to default file failed: %d\n",ACE_OS::last_error());
				ret=false;
			}
			else
			{
			   	map_pointers(i);
				if (fileCreated) 
				{
					setHardCodedValues(i);
				}
				// Added for TR HH21154 by XCSVAAN
				// Minimum file Size default value is set to zero for the first time when 
				// it is installed.
				else if ( i == 1)
				{
					// Changed the default minimum file size value to handle in term of bytes for TR HH71761
					if( *minFileSize < 0 || *minFileSize > 16777216)
						*minFileSize=DEF_MINFILESIZE;
				}
				
			}
	    } 
		if (!ret) break;
   } // for
   return ret;
}

//*************************************************************************
// setHardCodedValues
//
// Sets parameters to original startup values.
//
//  return value:
//    -
//*************************************************************************
void defFile::setHardCodedValues(int fnum)
{
	if (fnum==0)
	{
		*blockLength=DEF_BLOCKLENGTH;
		*blockHoldTime=DEF_BLOCKHOLDTIME;
		*fileSize=DEF_FILESIZE;
		*fileHoldTime=DEF_FIFILEHOLDTIME;
		*BlPaddingChar=DEF_BLPADDINGCHAR;
		*fileRecordLength=DEF_FILERECORDLENGTH;
		*FiPaddingChar=DEF_FIPADDINGCHAR;
		BlockLengthType = DEF_BLOCKLENGTHTYPE;
		strcpy(GenerationTime,DEF_GENERATIONTIME);
		strcpy(ResFlag,DEF_RESETFLAG);
		FixedFileRecords = DEF_FIXEDFILERECORDS;
	}
	else if (fnum==1)
	{
		*FiCdrFormat=DEF_CDRFORMATFLAG;
		*HmacMd5=DEF_HMACMD5FLAG;
		strcpy(FiKey,DEF_KEY);
		*minFileSize=DEF_MINFILESIZE;
	}
}

//*************************************************************************
// map_pointers
//
// Sets pointers for every parameter to the mapped file view.
//
//  return value:
//    -
//*************************************************************************
void defFile::map_pointers(int fnum)
{
	if (fnum==0)
	{
		blockLength=(int*)&FileView[0][IND_BLOCKLENGTH];
		blockHoldTime=(int*)&FileView[0][IND_BLOCKHOLDTIME];
		fileSize=(int*)&FileView[0][IND_FILESIZE];
		fileHoldTime=(int*)&FileView[0][IND_FIFILEHOLDTIME];
		BlPaddingChar=(int*)&FileView[0][IND_BLPADDINGCHAR];
		fileRecordLength=(int*)&FileView[0][IND_FILERECORDLENGTH];
		FiPaddingChar=(int*)&FileView[0][IND_FIPADDINGCHAR];
		BlockLengthType=(int*)&FileView[0][IND_BLOCKLENGTHTYPE];
		GenerationTime=(char*)&FileView[0][IND_GENERATIONTIME];
		ResFlag=(char*)&FileView[0][IND_RESETFLAG];
		FixedFileRecords=(int*)&FileView[0][IND_FIXEDFILERECORDS];
	}
	else if (fnum==1)
	{
		FiCdrFormat=(bool*)&FileView[1][IND_CDRFORMAT];
		HmacMd5=(bool*)&FileView[1][IND_HMACMD5];
		FiKey=(char*)&FileView[1][IND_KEY];
		minFileSize=(int*)&FileView[1][IND_MINFILESIZE];
	}
}

//*************************************************************************
// setDefaultValue
//
// Redefines the default value of a parameter.
//
//  return value:
//    -
//*************************************************************************
void defFile::setDefaultValue(parID p,unsigned int value)
{
	switch (p)
	{
	case a_blockLength:
		*blockLength=value;
		break;
	case b_cdrFileFormat:
		*FiCdrFormat = ( value ? true : false );
		break;
	case e_blockHoldTime:
		*blockHoldTime=value;
		break;
	case h_fileSize:
		*fileSize=value;
		break;
	case j_fileHoldTime:
		*fileHoldTime=value;
		break;
	case g_blockPaddingChar:
		*BlPaddingChar=value;
		break;
	case n_fileRecordLength:
		*fileRecordLength=value;
		break;
	case o_filePaddingChar:
		*FiPaddingChar=value;
		break;
	case p_minFileSize:
		*minFileSize=value;
		break;
	case d_blockLengthType:
		*BlockLengthType=value;
		break;
	case m_fixedFileRecords:
		*FixedFileRecords=value;
	default:
		break;
	};
	msync(FileView,DEF_DEFAULTPAR_FILESIZE,MS_SYNC);
}

//*************************************************************************
// setDefaultValue
//
// Redefines the default value of a parameter.
//
//  return value:
//    -
//*************************************************************************
void defFile::setDefaultValue(parID p,const char* strval)
{
	switch (p)
	{
	case c_key:
		if (!strcmp(strval,DEF_KEY))
		{
			strcpy(FiKey,strval);				
			*HmacMd5=false;
		}
		else
		{
			*HmacMd5=true;
			strcpy(FiKey,strval);
		}
		break;
	case k_generationTime:
		strcpy(GenerationTime,strval);
		break;
	case l_resetFlag:
		strcpy(ResFlag,strval);
		break;
	default:
		break;
	};
	msync(FileView,DEF_DEFAULTPAR_FILESIZE,MS_SYNC);
}

//*************************************************************************
// getDefaultValue
//
// Redefines the default value of a parameter.
//
//  return value:
//    -
//*************************************************************************
void defFile::getDefaultValues(struct RTRMS_Parameters& dpar)
{
	
	dpar.BlockLength=*blockLength;
	dpar.BlockHoldTime=*blockHoldTime;
	dpar.FileSize=*fileSize;
	dpar.MinFileSize=*minFileSize;
	dpar.FiFileHoldTime=*fileHoldTime;
	dpar.BlPaddingChar=*BlPaddingChar;
	dpar.FileRecordLength=*fileRecordLength;
	dpar.FiPaddingChar=*FiPaddingChar;
	dpar.FixedFileRecords = *FixedFileRecords;
	dpar.BlockLengthType = *BlockLengthType;	
	ACE_OS::snprintf(dpar.GenerationTime, 11, "%s", GenerationTime); //strcpy(dpar.GenerationTime,GenerationTime);
	ACE_OS::snprintf(dpar.ResetFlag, 3, "%s", ResFlag); //strcpy(dpar.ResetFlag,ResFlag);
	dpar.FiCdrFormat=*FiCdrFormat;
	dpar.HmacMd5=*HmacMd5;
	ACE_OS::snprintf(dpar.FiKey, 63, "%s", FiKey); //strcpy(dpar.FiKey,FiKey);
}


