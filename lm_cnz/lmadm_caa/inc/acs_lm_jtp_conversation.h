//****************************************************************************
//
//  NAME
//     ACS_LM_JTP_Conversation.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
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
//  DOCUMENT NO
//	    190 89-CAA XXX XXXX
//
//  AUTHOR 
//     2008-12-08 by XCSRPAD PA1
//
//  SEE ALSO 
//     -
//
//****************************************************************************
#ifndef _ACS_LM_JTP_CONVERSATION_H_
#define _ACS_LM_JTP_CONVERSATION_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include "acs_lm_common.h"
#include <ACS_JTP_Conversation_R3A.h>

static char hexCode[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_JTP_Conversation
{
public:
	/*===================================================================
			   						  PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
			   							   PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
	 *  @brief 	connect
	 *
	 *  @param cpService						: const string
	 *
	 *  @param node 							: JtpNode
	 *
	 *  @return bool							: true/false
	 **/
	/*=================================================================== */
	virtual bool connect(const std::string& cpService, JtpNode* node=NULL) = 0;
	/*=================================================================== */
        /**
         *  @param cpService   			                : const string
         *  @return short 
         **/
        /*=================================================================== */
        virtual unsigned short getProtocolFromCP (const std::string& cpService);
        /*=================================================================== */
	/**
	 *  @brief 	disconnect
	 *
	 *  @param lkDataList							: const list
	 *
	 *  @param lmMode 								: int
	 *
	 *  @return bool								: true/false
	 **/
	/*=================================================================== */
	//LMcleanup
    //virtual bool send(const std::list<LkData*>& lkDataList, int lmMode) = 0;
    virtual bool send(const std::list<LkData*>& lkDataList, int lmMode,  const std::list<LkData*>& discLklist = std::list<LkData*>(), const std::list<LkData*>& connLklist = std::list<LkData*>() ) = 0;
    //LMcleanup

	/*=================================================================== */
	/**
	 *  @brief 	disconnect
	 *
	 *  @return bool								: true/false
	 **/
	/*=================================================================== */
	virtual bool disconnect() = 0;
	/*=================================================================== */
	/**
	 *  @brief 	encode
	 *
	 *  @param jtpVersion							: int
	 *
	 *  @return ACS_LM_JTP_Conversation
	 **/
	/*=================================================================== */
	static ACS_LM_JTP_Conversation* create(int jtpVersion);
	/*=================================================================== */

	void setLMINSTFlag(bool myFlag);

	virtual void setStopSignal(bool);
	virtual bool getStopSignal();

	struct Buffer32
	{
	public:
		unsigned char buffer[JTP_BUFFER_SIZE];
		unsigned int capacity;
		unsigned int size;

		Buffer32()
			:capacity(JTP_BUFFER_SIZE), size(0)
		{
			ACE_OS::memset(buffer, 0, JTP_BUFFER_SIZE);
		}

		friend std::ostream& operator<<(std::ostream& out, const Buffer32& buffer32)
		{
			out<<std::endl;
			//print hreader
			unsigned int i=0;
			for(; i<10 && i<buffer32.size; i++)
			{
				int ln = (buffer32.buffer[i]&0x0F);
				int hn = (buffer32.buffer[i]>>4);
				out<<hexCode[hn]<<hexCode[ln]<<" ";
			}
			out<<std::endl;

			while(i<buffer32.size)
			{
				for(unsigned int j=0; j<36 && i<buffer32.size; j++, i++)
				{
					//Print each LK					
					int ln = (buffer32.buffer[i]&0x0F);
					int hn = (buffer32.buffer[i]>>4);
					out<<hexCode[hn]<<hexCode[ln]<<" ";
				}
				out<<std::endl;
			}
			out<<std::endl;
			return out;
		}
	};

	
protected:
	/*=================================================================== */
	/**
	 * 	@brief 	encode
	 *
	 *  @param lkDataList						: const list
	 *
	 *  @param lmMode							: int
	 *
	 *  @return list
	 **/
	/*=================================================================== */
	//LMcleanup
	//static std::list<Buffer32*> encode(const std::list<LkData*>& lkDataList, int lmMode);
	static std::list<Buffer32*> encode(const std::list<LkData*>& lkDataList, int lmMode, bool isCleanupdata=false );
	//LMcleanup

	/*=================================================================== */
	/**
	 *  @brief 	decode8
	 *
	 *  @param buf								: unsigned char pointer
	 *
	 *  @param pos								: unsigned int
	 *
	 *  @return int
	 **/
	/*=================================================================== */
	static int decode8(unsigned char* buf, unsigned int& pos);
	/*=================================================================== */
	/**
	 *  @brief 	encode8
	 *
	 *  @param buf								: unsigned char pointer
	 *
	 *  @param pos								: unsigned int
	 *
	 *  @param value							: int
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	static void encode8(unsigned char* buf, unsigned int& pos, int value);
	/*=================================================================== */
	/**
	 *  @brief 	decode16
	 *
	 *  @param buf								: unsigned char pointer
	 *
	 *  @param pos								: unsigned int
	 *
	 *  @return int
	 **/
	/*=================================================================== */
	static int decode16(unsigned char* buf, unsigned int& pos);
	/*=================================================================== */
	/**
	 *  @brief 	encode16
	 *
	 *  @param buf								: unsigned char pointer
	 *
	 *  @param pos								: unsigned int
	 *
	 *  @param value							: int
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	static void encode16(unsigned char* buf, unsigned int& pos, int value);
	/*=================================================================== */
	/**
	 *  @brief 	encodeHeader
	 *
	 *  @param buffer							: unsigned char pointer
	 *
	 *  @param pos								: unsigned int
	 *
	 *  @param lkNumber							: int
	 *
	 *  @param lmMode							: int
	 *
	 *  @param isLastLk							: bool
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	//LMcleanup
	//static void encodeHeader(unsigned char* buffer, unsigned int& pos, int lkNumber, int lmMode, bool isLastLk);
	static void encodeHeader(unsigned char* buffer, unsigned int& pos, int lkNumber, int lmMode, bool isLastLk, bool cleanup);
	//LMcleanup

	/*=================================================================== */
	/**
	 *  @brief 	encodeString120
	 *
	 *  @param buffer					: unsigned char pointer
	 *
	 *  @param pos						: unsigned int
	 *
	 *  @param str						: const string
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	static void encodeString120(unsigned char* buffer, unsigned int& pos, const std::string& str);
	/*=================================================================== */
	/**
	 *  @brief 	copyBuffer
	 *
	 *  @param destBuf					: unsigned char pointer
	 *
	 *  @param destPos					: unsigned int
	 *
	 *  @param srcBuf					: const unsigned char pointer
	 *
	 *  @param srcPos					: unsigned int
	 *
	 *  @param srcLen					: unsigned int
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	static void copyBuffer(unsigned char* destBuf, unsigned int destPos, const unsigned char* srcBuf, unsigned int srcPos, unsigned int srcLen);

	bool theLMInstCommandFlag;
};

#endif
