//****************************************************************************
//
//  NAME
//     ACS_ALOG_PLFileIterator.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2004. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  PR           DATE      INITIALS    DESCRIPTION
//  -----------------------------------------------------------
//  N/A       05/08/2012     xbhakat       Initial Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************


#include <iostream>
#include <string>
#include <ace/ACE.h>
#include <acs_alog_fileDef.h>
#include <acs_alog_PLFileIterator.h>
#include <acs_alog_cmdUtil.h>

//using namespace std;

class ACS_ALOG_PLFileIt
{
	public:
		ACS_ALOG_PLFileIt(const char* lpszFilename);
		~ACS_ALOG_PLFileIt();

		operator bool();
		void operator++();
		ACS_ALOG_Exclude* operator()();
		int GetRetCode();
		char* Encode(char* lpszData);
		char* Decode(char* lpszData);

	private:
		string m_szFilename;
		int m_hFile;
		ACS_ALOG_Exclude m_Exclude;
		bool m_bEOF;
		int m_nRetCode;
};

ACS_ALOG_PLFileIterator::ACS_ALOG_PLFileIterator(const char* lpszFilename)
{
	m_pPLFileIt = new ACS_ALOG_PLFileIt(lpszFilename);
}

ACS_ALOG_PLFileIterator::~ACS_ALOG_PLFileIterator()
{
	delete m_pPLFileIt;
}

ACS_ALOG_PLFileIterator::operator bool()
{
	return m_pPLFileIt->operator bool();
}

void ACS_ALOG_PLFileIterator::operator++()
{
	m_pPLFileIt->operator++();
}

ACS_ALOG_Exclude* ACS_ALOG_PLFileIterator::operator()()
{
	return m_pPLFileIt->operator()();
}

int ACS_ALOG_PLFileIterator::GetRetCode()
{
	return m_pPLFileIt->GetRetCode();
}

///////////////////////////////////////////////////////////////////////////////

ACS_ALOG_PLFileIt::ACS_ALOG_PLFileIt(const char* lpszFilename)
{
	acs_alog_cmdUtil::getAlogDataDiskPath(m_szFilename);
	m_szFilename += "/ACS_ALOG/" + (string)lpszFilename;

	m_bEOF = false;
	m_nRetCode = 0;

	m_hFile = ACE_OS::open(m_szFilename.c_str(),O_RDONLY);

	if (m_hFile == ACE_INVALID_HANDLE)
		m_nRetCode = 1;
	else
	{
		int dwBytesRead = ACE_OS::read(m_hFile,&m_Exclude,sizeof(ACS_ALOG_Exclude));
		if (dwBytesRead > 0 && dwBytesRead != sizeof(ACS_ALOG_Exclude))
			m_nRetCode = 1; // Error
		else if (dwBytesRead == 0)
			m_bEOF = true;  // End-of-file
	}

	/*
	   m_hFile = ::CreateFile(ACS_ALOG_Path(m_szFilename),
	   GENERIC_READ,
	   0,
	   NULL,
	   OPEN_EXISTING,
	   FILE_ATTRIBUTE_NORMAL,
	   NULL);

	   if (m_hFile == INVALID_HANDLE_VALUE)
	   m_nRetCode = 1;
	   else
	   {
	   DWORD dwBytesRead;
	   bool bSuccess = ::ReadFile(m_hFile,
	   &m_Exclude,
	   sizeof(ACS_ALOG_Exclude),
	   &dwBytesRead,
	   NULL);

	   if (!bSuccess || (dwBytesRead > 0 && dwBytesRead != sizeof(ACS_ALOG_Exclude)))
	   m_nRetCode = 1; // Error
	   else if (dwBytesRead == 0)
	   m_bEOF = TRUE;  // End-of-file
	   }
	 */


}

ACS_ALOG_PLFileIt::~ACS_ALOG_PLFileIt()
{
	if (m_hFile && m_hFile != ACE_INVALID_HANDLE)
		ACE_OS::close(m_hFile);
}

ACS_ALOG_PLFileIt::operator bool()
{
	if (m_hFile && m_hFile != ACE_INVALID_HANDLE && m_nRetCode == 0 && !m_bEOF)
		return true;
	else
		return false;
}

void ACS_ALOG_PLFileIt::operator++()
{
	int dwBytesRead = ACE_OS::read(m_hFile,&m_Exclude,sizeof(ACS_ALOG_Exclude));
	if (dwBytesRead > 0 && dwBytesRead != sizeof(ACS_ALOG_Exclude))
		m_nRetCode = 1; // Error
	else if (dwBytesRead == 0)
		m_bEOF = true;  // End-of-file
}

ACS_ALOG_Exclude* ACS_ALOG_PLFileIt::operator()()
{
	ACS_ALOG_Exclude* pExclude = new ACS_ALOG_Exclude();

	strcpy(pExclude->Data1, Decode(m_Exclude.Data1));
	strcpy(pExclude->Data2, Decode(m_Exclude.Data2));

	return pExclude;
}

int ACS_ALOG_PLFileIt::GetRetCode()
{
	return m_nRetCode;
}

char* ACS_ALOG_PLFileIt::Encode(char* lpszData)
{
	int nLen = strlen(lpszData);

	for (int nIdx = 0; nIdx < nLen; nIdx++)
		lpszData[nIdx] ^= 255;

	return lpszData;
}

char* ACS_ALOG_PLFileIt::Decode(char* lpszData)
{
	return Encode(lpszData);
}

