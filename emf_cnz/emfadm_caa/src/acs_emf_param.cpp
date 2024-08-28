/*=================================================================== */
/**
   @file acs_emf_commandhandler.cpp

   Class method implementation for EMF module.

   This module contains the implementation of class declared in
   the EMF Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     XRAMMAT       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */

//#pragma warning(disable: 4702)
//#include <windows.h>
//#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <ace/ACE.h>
#include "acs_emf_param.h"

using namespace std;

/*===================================================================
   ROUTINE: ACS_EMF_Data
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data::ACS_EMF_Data() : m_lpData(m_szData),
                                              m_nSize(0)
{
   *m_szData = NULL;
}//End of ACS_EMF_Data

/*===================================================================
   ROUTINE: ACS_EMF_Data
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data::ACS_EMF_Data(const ACS_EMF_Data& Data) : m_lpData(m_szData),
                                                                      m_nSize(0)
{
   *m_szData = NULL;
   m_nSize = Data.Length();

   if (m_nSize > 0 && m_nSize < 64)
      ACE_OS::memcpy(m_szData, Data.m_lpData, m_nSize);
   else if (m_nSize > 0)
   {
	   m_lpData =(ACE_TCHAR *) new ACE_TCHAR(m_nSize);
      if (m_lpData)
    	  ACE_OS::memcpy(m_lpData, Data.m_lpData, m_nSize);
      else
         m_nSize = 0;
   }
   else
      m_nSize = 0;
}//End of ACS_EMF_Data

/*===================================================================
   ROUTINE: ~ACS_EMF_Data
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data::~ACS_EMF_Data()
{
   if (m_lpData && m_lpData != m_szData)
      delete [] m_lpData;
}//End of ~ACS_EMF_Data


/*===================================================================
   ROUTINE: ~ACS_EMF_Data
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data& ACS_EMF_Array::ACS_EMF_Data::operator =(const ACE_UINT32 u32Value)
{
   *(ACE_UINT32*)m_szData = u32Value;
   m_nSize = sizeof(ACE_UINT32);

   if (m_lpData != m_szData)
   {
      if (m_lpData)
         delete [] m_lpData;

      m_lpData = m_szData;
   }

   return *this;
}//End of operator =

/*===================================================================
   ROUTINE: operator =
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data& ACS_EMF_Array::ACS_EMF_Data::operator =(const ACE_TCHAR* lpszValue)
{
   m_nSize = ((ACE_OS::strlen(lpszValue) + 1) * sizeof(ACE_TCHAR));

   if (m_nSize > 0 && m_nSize <= 64)
   {
      ACE_OS::strncpy(m_szData, lpszValue, m_nSize);

      if (m_lpData != m_szData)
      {
         if (m_lpData)
            delete [] m_lpData;

         m_lpData = m_szData;
      }
   }
   else if (m_nSize > 1)
   {
      if (m_lpData && m_lpData != m_szData)
         delete [] m_lpData;

      m_lpData = (ACE_TCHAR *) new ACE_TCHAR[m_nSize];
      if (m_lpData)
         ACE_OS::memcpy(m_lpData, lpszValue, m_nSize);
      else
         m_nSize = 0;
   }
   else
   {
      if (m_lpData && m_lpData != m_szData)
         delete [] m_lpData;

      *m_szData = NULL;
      m_lpData = m_szData;
      m_nSize = 0;
   }

   return *this;
}//End of operator =

/*===================================================================
   ROUTINE: operator =
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data& ACS_EMF_Array::ACS_EMF_Data::operator =(const ACS_EMF_Data& Data)
{
	m_nSize = Data.Length();

	if (m_nSize > 0 && m_nSize <= 64)
	{
		ACE_OS::memcpy(m_szData, Data.m_lpData, m_nSize);
		if (m_lpData != m_szData)
		{
			if (m_lpData)
				delete [] m_lpData;
			m_lpData = m_szData;
		}
	}
	else if (m_nSize > 0)
	{
		if (m_lpData && m_lpData != m_szData)
			delete [] m_lpData;
		ACE_TCHAR *tt = (ACE_TCHAR *)new ACE_TCHAR[m_nSize];
		m_lpData = tt;
		if (m_lpData)
			ACE_OS::memcpy(m_lpData, Data.m_lpData, m_nSize);
		else
			m_nSize = 0;
	}
	else
	{
		if (m_lpData && m_lpData != m_szData)
			delete [] m_lpData;

		*m_szData = NULL;
		m_nSize = 0;
		m_lpData = m_szData;
	}
	return *this;
}//End of operator =

/*===================================================================
   ROUTINE: Assign
=================================================================== */
void ACS_EMF_Array::ACS_EMF_Data::Assign(const size_t nSize, const ACE_TCHAR* lpszData)
{
   m_nSize = nSize;

   if (m_nSize > 0 && m_nSize <= 64)
   {
      ACE_OS::memcpy(m_szData, lpszData, m_nSize);

      if (m_lpData != m_szData)
      {
         if (m_lpData)
            delete [] m_lpData;

         m_lpData = m_szData;
      }
   }
   else if (m_nSize > 0)
   {
      if (m_lpData && m_lpData != m_szData)
         delete [] m_lpData;

      m_lpData = (ACE_TCHAR *)new ACE_TCHAR [m_nSize];
      if (m_lpData)
         ACE_OS::memcpy(m_lpData, lpszData, m_nSize);
      else
         m_nSize = 0;
   }
   else
   {
      if (m_lpData && m_lpData != m_szData)
         delete [] m_lpData;

      *m_szData = NULL;
      m_nSize = 0;
      m_lpData = m_szData;
   }
}//End of Assign

/*===================================================================
   ROUTINE: Length
=================================================================== */
size_t ACS_EMF_Array::ACS_EMF_Data::Length() const
{
	return m_nSize;
}//End of Length

/*===================================================================
   ROUTINE: operator ACE_UINT32
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data::operator ACE_UINT32() const
{
   if (m_lpData)
      return *(ACE_UINT32*)m_lpData;
   else
      return 0;
}//End of operator ACE_UINT32

/*===================================================================
   ROUTINE: operator const ACE_TCHAR*
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data::operator const ACE_TCHAR*()
{
   if (!m_lpData)
      return NULL;
   else
      return (const ACE_TCHAR*)m_lpData;
}//End of operator const ACE_TCHAR*

/*===================================================================
   ROUTINE: ACS_EMF_Array
=================================================================== */
ACS_EMF_Array::ACS_EMF_Array():m_vData(0)
{
}//End of ACS_EMF_Array

/*===================================================================
   ROUTINE: ACS_EMF_Array
=================================================================== */
ACS_EMF_Array::ACS_EMF_Array(const ACS_EMF_Array&):m_vData(0)
{
}//End of ACS_EMF_Array

/*===================================================================
   ROUTINE: ~ACS_EMF_Array
=================================================================== */
ACS_EMF_Array::~ACS_EMF_Array()
{
   try
   {
      Clear();
   }
   catch (...) { }
}//End of ~ACS_EMF_Array
 
/*===================================================================
   ROUTINE: operator []
=================================================================== */
const ACS_EMF_Array::ACS_EMF_Data& ACS_EMF_Array::operator [](ACE_INT32 idx) const
{
   if (idx < 0 || idx >= (int)m_vData.size())
      exit(EXIT_FAILURE);

   return *m_vData.at(idx);
}//End of operator []

/*===================================================================
   ROUTINE: operator []
=================================================================== */
ACS_EMF_Array::ACS_EMF_Data& ACS_EMF_Array::operator [](ACE_INT32 idx)
{
   if (idx < 0)
      exit(EXIT_FAILURE);

   if (idx >= (int)m_vData.size())
      Resize(idx+1);

   return *m_vData.at(idx);
}//End of operator []

/*===================================================================
   ROUTINE: Resize
=================================================================== */
void ACS_EMF_Array::Resize(ACE_INT32 nNewSize)
{
   for (ACE_INT32 nIdx = Size(); nIdx < nNewSize; nIdx++)
   {
	   m_vData.push_back(new ACS_EMF_Data());
   }
}//End of Resize

/*===================================================================
   ROUTINE: Clear
=================================================================== */
void ACS_EMF_Array::Clear() 
{
   while (!m_vData.empty())
   {
      ACS_EMF_Data* pData = *m_vData.begin();
      delete pData;
      (void)m_vData.erase(m_vData.begin());
   }
}//End of Clear

/*===================================================================
   ROUTINE: Size
=================================================================== */
ACE_INT32 ACS_EMF_Array::Size()
{
   return (ACE_INT32)m_vData.size();
}//End of Size

/*===================================================================
   ROUTINE: operator =
=================================================================== */
ACS_EMF_Array& ACS_EMF_Array::operator =(const ACS_EMF_Array& array)
{
   m_vData = array.m_vData;
   return *this;
}//End of operator =

/*===================================================================
   ROUTINE: ACS_EMF_ParamList
=================================================================== */
ACS_EMF_ParamList::ACS_EMF_ParamList() : CmdCode(0),
                                         m_dwResult(0),
                                         m_lpBuffer(NULL),
                                         m_nBufferSize(0)
{
}//End of ACS_EMF_ParamList

/*===================================================================
   ROUTINE: ~ACS_EMF_ParamList
=================================================================== */
ACS_EMF_ParamList::~ACS_EMF_ParamList()
{
   try
   {
      Clear();
   }
   catch (...) { }
}//End of ~ACS_EMF_ParamList

/*===================================================================
   ROUTINE: Clear
=================================================================== */
void ACS_EMF_ParamList::Clear()
{
   if (m_lpBuffer)
   {
      delete [] m_lpBuffer;
      m_lpBuffer = NULL;
   }

   Data.Clear();
   m_dwResult = 0;
}//End of Clear

/*===================================================================
   ROUTINE: NumOfData
=================================================================== */
ACE_INT32 ACS_EMF_ParamList::NumOfData()
{
   return Data.Size();
}//End of NumOfData

/*===================================================================
   ROUTINE: Encode
=================================================================== */
void ACS_EMF_ParamList::Encode()
{
   // |----------- Header ------------||--------- Data ----------
   //   4      4        4        4       4     1-?     4    1-?
   // [Len][CmdCode][Result][NumOfData][Len1][Data1][Len2][Data2]...
   //  0-3    4-7     8-11     12-15   16-19  20-?

   const ACE_INT32 nHeaderSize = 16;
   ACE_INT32 nSize = nHeaderSize;
   ACE_INT32 nIdx;

   for (nIdx = 0; nIdx < NumOfData(); nIdx++)
   {
      nSize += sizeof(ACE_UINT32);
      nSize += (ACE_INT32)Data[nIdx].Length();
   }

   m_nBufferSize = nSize;

   if (m_lpBuffer)
      delete [] m_lpBuffer;

   m_lpBuffer = (ACE_TCHAR *) new ACE_TCHAR [m_nBufferSize];

   if (!m_lpBuffer)
      return;

   int nPos = 0;

   nSize -= sizeof(ACE_UINT32);
   *(ACE_UINT32*)(m_lpBuffer+nPos) = (ACE_UINT32)nSize;
   nPos += sizeof(ACE_UINT32);

   *(ACE_UINT32*)(m_lpBuffer+nPos) = (ACE_UINT32)CmdCode;
   nPos += sizeof(ACE_UINT32);

   *(ACE_UINT32*)(m_lpBuffer+nPos) = (ACE_UINT32)m_dwResult;
   nPos += sizeof(ACE_UINT32);

   *(ACE_UINT32*)(m_lpBuffer+nPos) = (ACE_UINT32)NumOfData();
   nPos += sizeof(ACE_UINT32);

   for (nIdx = 0; nIdx < NumOfData(); nIdx++) 
   {
      size_t nLen = Data[nIdx].Length();
      *(ACE_UINT32*)(m_lpBuffer+nPos) = (ACE_UINT32)nLen;
      nPos += sizeof(ACE_UINT32);

      ACE_TCHAR* lpPtr = (ACE_TCHAR *)(const ACE_TCHAR*)Data[nIdx];
      if (lpPtr)
         ACE_OS::memcpy(m_lpBuffer+nPos, lpPtr, nLen);
      nPos += (ACE_INT32)nLen;
   }
}//End of Encode

/*===================================================================
   ROUTINE: Decode
=================================================================== */
void ACS_EMF_ParamList::Decode(const ACE_TCHAR* lpBuffer)
{
   if (!lpBuffer)
      return;

   ACE_INT32 nPos = 0;

   //int nBufferSize = (int)*(DWORD*)lpBuffer;
   nPos += sizeof(ACE_UINT32);

   CmdCode = *(ACE_UINT32*)(lpBuffer+nPos);
   nPos += sizeof(ACE_UINT32);

   m_dwResult = *(ACE_UINT32*)(lpBuffer+nPos);
   nPos += sizeof(ACE_UINT32);

   int nNumOfData = (int)*(ACE_UINT32*)(lpBuffer+nPos);
   nPos += sizeof(ACE_UINT32);

   ACE_TCHAR* lpPtr;
   for (int nIdx = 0; nIdx < nNumOfData; nIdx++)
   {
      size_t nLen = *(ACE_UINT32*)(lpBuffer+nPos);
      nPos += sizeof(ACE_UINT32);

      lpPtr = (ACE_TCHAR*)(lpBuffer+nPos);
      Data[nIdx].Assign(nLen, (const ACE_TCHAR*)lpPtr);
      nPos += (int)nLen;
   }
}

/*===================================================================
   ROUTINE: getEncodedBuffer
=================================================================== */
bool ACS_EMF_ParamList::getEncodedBuffer(ACE_TCHAR* lpBuffer)
{
   if (!lpBuffer || !m_lpBuffer)
      return FALSE;

   ACE_OS::memcpy(lpBuffer, m_lpBuffer, m_nBufferSize);

   return TRUE;
}//End of getEncodedBuffer

/*===================================================================
   ROUTINE: getEncodedBuffer
=================================================================== */
const ACE_TCHAR* ACS_EMF_ParamList::getEncodedBuffer() const
{
   return m_lpBuffer;
}//End of getEncodedBuffer

/*===================================================================
   ROUTINE: getEncodedSize
=================================================================== */
size_t ACS_EMF_ParamList::getEncodedSize() const
{
   return m_nBufferSize;
}//End of getEncodedSize
