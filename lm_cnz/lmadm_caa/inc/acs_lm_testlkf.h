//******************************************************************************
//
//  NAME
//     ACS_LM_TestLkf.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
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
//	    190 89-CAA nnn nnnn
//
//  AUTHOR 
//     2008-12-08 by XCSVEMU PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************
#ifndef _ACS_LM_TESTLKF_H_ 
#define _ACS_LM_TESTLKF_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include "acs_lm_persistent.h"
#include <iostream>
#include <ace/ACE.h>

#define TESTLKF_RECORD_LEN 48
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_Persistent;
class ACS_LM_TestLkf
{
	/*===================================================================
		   						  PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
		   							   PUBLIC METHOD
	=================================================================== */
public:
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_TestLkf

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_TestLkf();
	/*=================================================================== */
	/**
	 *  @brief addTestLk
	 *
	 *  @param srctlkData                	 : const LkData
	 *
	 *  @return ACS_LM_AppExitCode
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode addTestLk(const LkData& srctlkData);
	/*=================================================================== */
	/**
	 *  @brief removeTestLk
	 *
	 *  @param lkid							 : const string
	 *
	 *  @return ACS_LM_AppExitCode
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode removeTestLk(const std::string& lkid);
	/*=================================================================== */
	/**
	 *  @brief getTestLks
	 *
	 *  @param attr 						: list
	 *
	 *  @return ACS_LM_AppExitCode
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode getTestLks(std::list<LkData*>& attr);
	/*=================================================================== */
	/**
	 *  @brief removeAllTestLks
	 *
	 *  @return ACS_LM_AppExitCode
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode removeAllTestLks();
	/*=================================================================== */
	/**
	 *  @brief removeAllTestLksTemp
	 *
	 *  @return ACS_LM_AppExitCode
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode removeAllTestLksTemp();
	/*=================================================================== */
	/**
	 *  @brief removeAllTestLksPerm
	 *
	 *  @return ACS_LM_AppExitCode
	 */
	/*=================================================================== */
	ACS_LM_AppExitCode removeAllTestLksPerm();
	/*=================================================================== */
	/**
	 *  @brief restoreAllTestLks
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void restoreAllTestLks();
	/*=================================================================== */
	/**
	 *  @brief sizeInBytes
	 *
	 *  @return ACE_UINT64
	 */
	/*=================================================================== */
	ACE_UINT64 sizeInBytes();
	/*=================================================================== */
	/**
	 *  @brief store
	 *
	 *  @param destBuff 						: ACE_TCHAR
	 *
	 *  @param destPos 							: ACE_UINT64
	 *
	 *  @return void
	 *
	 */
	/*=================================================================== */
	void store(ACE_TCHAR * destBuff, ACE_UINT64& destPos);
	/*=================================================================== */
	/**
	 *  @brief load
	 *
	 *  @param persistFile 						: ACS_LM_Persistent
	 *
	 *  @return ACS_LM_TestLkf
	 */
	/*=================================================================== */
	static ACS_LM_TestLkf* load(ACS_LM_Persistent* persistFile);
	/*=================================================================== */
	/**
	 *  @brief load
	 *
	 *  @param persistFile					 	: ACS_LM_Persistent
	 *
	 *  @param srcBuff 							: ACE_TCHAR
	 *
	 *  @param srcPos 							: ACE_UINT64
	 *
	 *  @return ACS_LM_TestLkf
	 */
	/*=================================================================== */
	static ACS_LM_TestLkf* load(ACS_LM_Persistent* persistFile, ACE_TCHAR * srcBuff, ACE_UINT64& srcPos);
	/*=================================================================== */

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	ACS_LM_TestLkf(ACS_LM_Persistent* persistFile);	
	struct Lk
	{
		std::string id;
		std::string paramName;
		std::string setName;
		int status;
		int value;

		Lk():id(""), paramName(""), setName(""), status(0), value(0)
		{
		}
		static Lk* fromBytes(ACE_TCHAR * buffer, ACE_UINT64& pos)
				{
			Lk* lk = new Lk();
			lk->id = ACS_LM_Common::decodeString120(buffer, pos);
			lk->paramName = ACS_LM_Common::decodeString120(buffer, pos);
			lk->setName = ACS_LM_Common::decodeString120(buffer, pos);
			lk->status = ACS_LM_Common::decode8(buffer, pos);
			lk->value = ACS_LM_Common::decode16(buffer, pos);

			return lk;
				}

		void toBytes(ACE_TCHAR * buffer, ACE_UINT64& pos)
		{
			ACS_LM_Common::encodeString120(buffer, pos, id);
			ACS_LM_Common::encodeString120(buffer, pos, paramName);
			ACS_LM_Common::encodeString120(buffer, pos, setName);
			ACS_LM_Common::encode8(buffer, pos, status);
			ACS_LM_Common::encode16(buffer, pos, value);
		}
	};

	std::list<Lk*> lkList;
	std::list<Lk*> tempLKList;
	ACS_LM_Persistent* persistFile;
	/*=================================================================== */
	/**
	 *  @brief copyList
	 *
	 *  @param destList 					: list
	 *
	 *  @param srcList 						: const list
	 *
	 *  @return void
	 */
	/*=================================================================== */
	static void copyList(std::list<Lk*>& destList, const std::list<Lk*>& srcList);
public:
	/*=================================================================== */
	/**
	 * 	@brief reload
	 *
	 *  @param srcBuff 						: ACE_TCHAR
	 *
	 *  @param srcPos 						: ACE_UINT64
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void reload(ACE_TCHAR * srcBuff, ACE_UINT64& srcPos);

private:

	//Disable the copy.
	//*=================================================================== */
	/**
				@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_LM_TestLkf(const ACS_LM_TestLkf&);
	//*=================================================================== */
	/**
						@brief      operator
	 */
	/*=================================================================== */
	ACS_LM_TestLkf operator =(const ACS_LM_TestLkf&);
};

#endif
