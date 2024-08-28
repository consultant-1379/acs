#if 0
//******************************************************************************
//
//  NAME
//     ACS_SSU_Param.h
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

//#ifndef ACS_SSU_PARAM_H
//#define ACS_SSU_PARAM_H

#include <ace/ACE.h>
#include <vector>
#include <ACS_PHA_Tables.h>
//#include <ACS_PHA_Parameter.h>

const char ACS_CS_tableName [] = "ACS/CXC1371218"; // Name of CS parameter table.

// Parameter name. This parameter
// states the if the system is a
// multiple cp system or not
class ACS_SSU_Array
{
public:
   class ACS_SSU_Data
   {
   public:
      ACS_SSU_Data();
      ACS_SSU_Data(const ACS_SSU_Data&);
      ~ACS_SSU_Data();

      ACS_SSU_Data& operator =(const ACE_UINT32);
      ACS_SSU_Data& operator =(const ACS_SSU_Data&);
      ACS_SSU_Data& operator =(const ACE_TCHAR*);

      void Assign(const size_t nLen, const ACE_TCHAR* lpszData);
      const size_t Length() const;

      operator ACE_UINT32();
      operator const ACE_TCHAR*();

   protected:
      ACE_TCHAR m_szData[64];
      LPBYTE m_lpData;
      size_t m_nSize;
   };

public:
   ACS_SSU_Array();
   ~ACS_SSU_Array();

   ACS_SSU_Data& operator [](ACE_INT32 idx);
   const ACS_SSU_Data& operator [](ACE_INT32 idx) const;

   ACS_SSU_Array& operator =(const ACS_SSU_Array&);

   void Resize(ACE_INT32 nNewSize);
   void Clear();
   int Size();

private:
   ACS_SSU_Array(const ACS_SSU_Array&);

private:
   std::vector<ACS_SSU_Data*> m_vData;
};

class ACS_SSU_ParamList
{
public:
   ACS_SSU_ParamList();
   ~ACS_SSU_ParamList();

   void Clear();
   ACE_INT32 NumOfData();
   void Encode();
   void Decode(const LPBYTE lpBuffer);

   BOOL getEncodedBuffer(LPBYTE lpBuffer);
   const LPBYTE getEncodedBuffer() const;
   const size_t getEncodedSize() const;
   bool getIsMultCPSysValue();

public:
   ACS_SSU_Array Data;
   ACE_UINT32         CmdCode;

private:
   ACS_PHA_ParamReturnType getParameters ();
   unsigned long isMultCPSysValue;
   LPBYTE m_lpBuffer;
   size_t m_nBufferSize;
   ACE_UINT32  m_dwResult;
};

#endif
