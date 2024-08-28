
#if 0
//******************************************************************************
//
//  NAME
//     ACS_SSU_Param.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA 109 0260
//
//  AUTHOR 
//     2005-04-01 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#include <ace/ACE.h>
#include <stdio.h>
#include <iostream>
#include "acs_ssu_param.h"

using namespace std;

//============================================================================
// Constructor
// Default constructor
//============================================================================
ACS_SSU_Array::ACS_SSU_Data::ACS_SSU_Data() : m_lpData(m_szData),
                                              m_nSize(0)
{
   *m_szData = NULL;
}

//============================================================================
// Constructor
// Copy constructor
//============================================================================
ACS_SSU_Array::ACS_SSU_Data::ACS_SSU_Data(const ACS_SSU_Data& Data) : m_lpData(m_szData),
                                                                      m_nSize(0)
{
   *m_szData = NULL;
   m_nSize = Data.Length();

   if (m_nSize > 0 && m_nSize < 64)
      ACE_OS::memcpy(m_szData, Data.m_lpData, m_nSize);
   else if (m_nSize > 0)
   {
      m_lpData =  new ACE_TCHAR[m_nSize];

      if (m_lpData)
         ACE_OS::memcpy(m_lpData, Data.m_lpData, m_nSize);
      else
         m_nSize = 0;
   }
   else
      m_nSize = 0;
}

// Destructor
ACS_SSU_Array::ACS_SSU_Data::~ACS_SSU_Data()
{
   if (m_lpData && m_lpData != m_szData)
      delete[] m_lpData;
}

//============================================================================
// Operator
// Assigned a DWORD value
//============================================================================
ACS_SSU_Array::ACS_SSU_Data& ACS_SSU_Array::ACS_SSU_Data::operator =(const ACE_UINT32 u32Value)
{
   *(ACE_UINT32*)m_szData = u32Value;
   m_nSize = sizeof(ACE_UINT32);

   if (m_lpData != m_szData)
   {
      if (m_lpData)
         delete[] m_lpData;

      m_lpData = m_szData;
   }

   return *this;
}

// Assigned a NULL terminated string value
ACS_SSU_Array::ACS_SSU_Data& ACS_SSU_Array::ACS_SSU_Data::operator =(const ACE_TCHAR* lpszValue)
{
   m_nSize = ((ACE_OS::strlen(lpszValue) + 1) * sizeof(ACE_TCHAR));

   if (m_nSize > 0 && m_nSize <= 64)
   {
      ACE_OS::strncpy(m_szData, lpszValue, m_nSize);

      if (m_lpData != m_szData)
      {
         if (m_lpData)
            delete[] m_lpData;

         m_lpData = m_szData;
      }
   }
   else if (m_nSize > 1)
   {
      if (m_lpData && m_lpData != m_szData)
         delete[] m_lpData;

      m_lpData = (LPBYTE)new ACE_TCHAR[m_nSize];
      if (m_lpData)
         ACE_OS::memcpy(m_lpData, lpszValue, m_nSize);
      else
         m_nSize = 0;
   }
   else
   {
      if (m_lpData && m_lpData != m_szData)
         delete[] m_lpData;

      *m_szData = NULL;
      m_lpData = m_szData;
      m_nSize = 0;
   }

   return *this;
}

//! Assigned a ACS_SSU_Data object
ACS_SSU_Array::ACS_SSU_Data& ACS_SSU_Array::ACS_SSU_Data::operator =(const ACS_SSU_Data& Data)
{
   m_nSize = Data.Length();

   if (m_nSize > 0 && m_nSize <= 64)
   {
      ACE_OS::memcpy(m_szData, Data.m_lpData, m_nSize);

      if (m_lpData != m_szData)
      {
         if (m_lpData)
            delete[] m_lpData;

         m_lpData = m_szData;
      }
   }
   else if (m_nSize > 0)
   {
      if (m_lpData && m_lpData != m_szData)
         delete[] m_lpData;

      m_lpData = (LPBYTE)new ACE_TCHAR[m_nSize];
      if (m_lpData)
         ACE_OS::memcpy(m_lpData, Data.m_lpData, m_nSize);
      else
         m_nSize = 0;
   }
   else
   {
      if (m_lpData && m_lpData != m_szData)
         delete[] m_lpData;

      *m_szData = NULL;
      m_nSize = 0;
      m_lpData = m_szData;
   }

   return *this;
}

//============================================================================
// Assign
// Copy nSize of characters to a new allocated string buffer
//============================================================================
void ACS_SSU_Array::ACS_SSU_Data::Assign(const size_t nSize, const ACE_TCHAR* lpszData)
{
   m_nSize = nSize;

   if (m_nSize > 0 && m_nSize <= 64)
   {
      ACE_OS::memcpy(m_szData, lpszData, m_nSize);

      if (m_lpData != m_szData)
      {
         if (m_lpData)
            delete[] m_lpData;

         m_lpData = m_szData;
      }
   }
   else if (m_nSize > 0)
   {
      if (m_lpData && m_lpData != m_szData)
         delete[] m_lpData;

      m_lpData = (LPBYTE)new ACE_TCHAR[m_nSize];
      if (m_lpData)
         ACE_OS::memcpy(m_lpData, lpszData, m_nSize);
      else
         m_nSize = 0;
   }
   else
   {
      if (m_lpData && m_lpData != m_szData)
         delete[] m_lpData;

      *m_szData = NULL;
      m_nSize = 0;
      m_lpData = m_szData;
   }
}

//============================================================================
// Length
// Returns the size of bytes of a ACS_SSU_Data object
//============================================================================
const size_t ACS_SSU_Array::ACS_SSU_Data::Length() const
{
   return m_nSize;
}

//============================================================================
// Operator
// Returns a DWORD value
//============================================================================
ACS_SSU_Array::ACS_SSU_Data::operator ACE_UINT32()
{
   if (m_lpData)
      return *(ACE_UINT32*)m_lpData;
   else
      return 0;
}

//============================================================================
// Operator
// Returns a null terminated string value
//============================================================================
ACS_SSU_Array::ACS_SSU_Data::operator const ACE_TCHAR*()
{
   if (!m_lpData)
      return NULL;
   else
      return (const ACE_TCHAR*)m_lpData;
}

//============================================================================
// Constructor
// Default Constructor
//============================================================================
ACS_SSU_Array::ACS_SSU_Array()
{
}

//============================================================================
// Constructor
// Copy constructor
//============================================================================
ACS_SSU_Array::ACS_SSU_Array(const ACS_SSU_Array&)
{
}

//============================================================================
// Destructor
//============================================================================
ACS_SSU_Array::~ACS_SSU_Array()
{
   try
   {
      Clear();
   }
   catch (...) { }
}
 
//============================================================================
// Operator
// Returns the ACS_SSU_Data object stores at a specific index of the vector
//============================================================================
const ACS_SSU_Array::ACS_SSU_Data& ACS_SSU_Array::operator [](int idx) const
{
   if (idx < 0 || idx >= (int)m_vData.size())
      exit(EXIT_FAILURE);

   return *m_vData.at(idx);
}

//============================================================================
// Operator
// Adds a new ACS_SSU_Data object and stores it at a specific index of the
// vector
//============================================================================
ACS_SSU_Array::ACS_SSU_Data& ACS_SSU_Array::operator [](ACE_INT32 idx)
{
   if (idx < 0)
      exit(EXIT_FAILURE);

   if (idx >= (int)m_vData.size())
      Resize(idx+1);

   return *m_vData.at(idx);
}

//============================================================================
// Resize
// Allocates a new ACS_SSU_Data object and stores it at a specific index of
// the vector
//============================================================================
void ACS_SSU_Array::Resize(ACE_INT32 nNewSize)
{
   for (ACE_INT32 nIdx = Size(); nIdx < nNewSize; nIdx++)
   {
	   m_vData.push_back(new ACS_SSU_Data());
   }
}

//============================================================================
// Clear
// Deletes all ACS_SSU_Data objects from the vector and deallocate the memory
//============================================================================
void ACS_SSU_Array::Clear() 
{
   while (!m_vData.empty())
   {
      ACS_SSU_Data* pData = *m_vData.begin();
      delete pData;
      (void)m_vData.erase(m_vData.begin());
   }
}

//============================================================================
// Size
// Returns the number of ACS_SSU_Data objects stored in the vector
//============================================================================
ACE_INT32 ACS_SSU_Array::Size()
{
   return (ACE_INT32)m_vData.size();
}

//============================================================================
// Operator
// Returns a ACS_SSU_Array object
//============================================================================
ACS_SSU_Array& ACS_SSU_Array::operator =(const ACS_SSU_Array&)
{
   return *this;
}

//============================================================================
// Constructor
// Default constructor
//============================================================================
ACS_SSU_ParamList::ACS_SSU_ParamList() : CmdCode(0),
                                         m_lpBuffer(NULL),
                                         m_nBufferSize(0),
                                         m_dwResult(0)
{
}

//============================================================================
// Destructor
//============================================================================
ACS_SSU_ParamList::~ACS_SSU_ParamList()
{
   try
   {
      Clear();
   }
   catch (...) { }
}

//============================================================================
// Clear
// Deallocate the memory of an encoded buffer
//============================================================================
void ACS_SSU_ParamList::Clear()
{
   if (m_lpBuffer)
   {
      delete[] m_lpBuffer;
      m_lpBuffer = NULL;
   }

   Data.Clear();
   m_dwResult = 0;
}

//============================================================================
// NumOfData
// Returns the number of objects stored in the vector
//============================================================================
int ACS_SSU_ParamList::NumOfData()
{
   return Data.Size();
}

//============================================================================
// Encode
// Encode all ACS_SSU_Data objects to a byte buffer
//============================================================================
void ACS_SSU_ParamList::Encode()
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
      nSize += (int)Data[nIdx].Length();
   }

   m_nBufferSize = nSize;

   if (m_lpBuffer)
      delete[] m_lpBuffer;

   m_lpBuffer = (LPBYTE)new ACE_TCHAR[m_nBufferSize];

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

      LPBYTE lpPtr = (LPBYTE)(const ACE_TCHAR*)Data[nIdx];
      ACE_OS::memcpy(m_lpBuffer+nPos, lpPtr, nLen);
      nPos += (ACE_INT32)nLen;
   }
}

//============================================================================
// Decode
// Decode a byte buffer and constructs a vector of ACS_SSU_Data objects
//============================================================================
void ACS_SSU_ParamList::Decode(const LPBYTE lpBuffer)
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

   LPBYTE lpPtr;
   for (int nIdx = 0; nIdx < nNumOfData; nIdx++)
   {
      size_t nLen = *(ACE_UINT32*)(lpBuffer+nPos);
      nPos += sizeof(ACE_UINT32);

      lpPtr = (LPBYTE)(lpBuffer+nPos);
      Data[nIdx].Assign(nLen, (const ACE_TCHAR*)lpPtr);
      nPos += (ACE_INT32)nLen;
   }
}

//============================================================================
// getEncodedBuffer
// Copies the encoded byte buffer
//============================================================================
BOOL ACS_SSU_ParamList::getEncodedBuffer(LPBYTE lpBuffer)
{
   if (!lpBuffer || !m_lpBuffer)
      return FALSE;

   ACE_OS::memcpy(lpBuffer, m_lpBuffer, m_nBufferSize);

   return TRUE;
}

//============================================================================
// getEncodedBuffer
// Returns an pointer to the encoded byte buffer
//============================================================================
const LPBYTE ACS_SSU_ParamList::getEncodedBuffer() const
{
   return m_lpBuffer;
}

//============================================================================
// getEncodedSize
// Returns the size in bytes of the encoded byte buffer
//============================================================================
const size_t ACS_SSU_ParamList::getEncodedSize() const
{
   return m_nBufferSize;
}

ACS_PHA_ParamReturnType ACS_SSU_ParamList::getParameters ()
{
	ACS_PHA_ParamReturnType rcode = ACS_PHA_PARAM_RC_OK;
	ACS_PHA_Tables ACS_CS_parameters(ACS_CS_tableName);

	// Get data from the parameter handler
	// Parameter `ACS_CSBIN_isMultipleCPSystem'

	ACS_PHA_Parameter<unsigned long> isMultCPSys(ACS_CSBIN_isMultipleCPSystem);

	if (isMultCPSys.get(ACS_CS_parameters) == ACS_PHA_PARAM_RC_OK)
	{
		isMultCPSysValue = isMultCPSys.data();
	}
	else
	{
		rcode = ACS_PHA_PARAM_RC_ERROR;
	}
	return rcode;
} // End of getParameters

//******************************************************************************
//	unsigned short getIsMultCPSysValue()
//  Returns parameter from PHA
//******************************************************************************

bool ACS_SSU_ParamList::getIsMultCPSysValue()
{
	ACE_UINT32 u32Value = getParameters();
	if ( u32Value ==ACS_PHA_PARAM_RC_OK )
	{
		if (isMultCPSysValue==0)
			return FALSE;
		else
			return TRUE;
	}
	else
		return FALSE;    //considered not multiple cp
} // End of getIsMultCPSysValue

#endif
