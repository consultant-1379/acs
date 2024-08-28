//******************************************************************************
//
//  NAME
//     acs_lm_jtp_conversation.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
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
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_lm_jtp_conversation.h"
#include "acs_lm_jtp_conversation_R2.h"
#include "acs_lm_jtp_conversation_R3.h"

#define MAX_LKS_ALLOWED 900
#if 0
ACS_LM_DefineTrace(ACS_LM_JTP_Conversation);
#endif
/*=================================================================
	ROUTINE: create
=================================================================== */
ACS_LM_JTP_Conversation* ACS_LM_JTP_Conversation::create(int jtpVersion)
{	
	ACS_LM_JTP_Conversation* jtp = NULL;

	if(jtpVersion == 3)
    {
#if 0
        ACS_LM_PrintTrace(ACS_LM_JTP_Conversation, "create(): ACS_LM_JTP_Conversation_R3 is created");
#endif
        jtp = new ACS_LM_JTP_Conversation_R3();
    }
    else
    {
#if 0
        ACS_LM_PrintTrace(ACS_LM_JTP_Conversation, "create(): ACS_LM_JTP_Conversation_R2 is created");
#endif
        jtp = new ACS_LM_JTP_Conversation_R2();
    }

    return jtp;
}//end of create
/*=================================================================
	ROUTINE: encode16
=================================================================== */
void ACS_LM_JTP_Conversation::encode16(unsigned char* destBuf, unsigned int& destPos, int value)
{
	destBuf[destPos] = (0xFF & value);
	destBuf[destPos+1] = ((0xFF00 & value) >> 8);
	destPos += 2;
}//end of encode16
/*=================================================================
	ROUTINE: encode8
=================================================================== */
void ACS_LM_JTP_Conversation::encode8(unsigned char* destBuf, unsigned int& destPos, int value)
{
	destBuf[destPos] = value;
	destPos += 1;
}//end of encode8
/*=================================================================
	ROUTINE: decode16
=================================================================== */
int ACS_LM_JTP_Conversation::decode16(unsigned char* srcBuf, unsigned int& srcPos)
{
    int decodedValue = (srcBuf[srcPos] & 0x000000FF);
    decodedValue = decodedValue + ((srcBuf[srcPos+1] & 0x000000FF) <<8);   
	srcPos -= 2;
	return decodedValue;
}//end of decode16
/*=================================================================
	ROUTINE: decode8
=================================================================== */
int ACS_LM_JTP_Conversation::decode8(unsigned char* srcBuf, unsigned int& srcPos)
{
    int decodedValue = srcBuf[srcPos];
    srcPos -= 1;
	return decodedValue;
}//end of decode8
/*=================================================================
	ROUTINE: encodeString120
=================================================================== */
void ACS_LM_JTP_Conversation::encodeString120(unsigned char* buffer, unsigned int& pos, const std::string& str)
{
	const char* strBuf = str.c_str();

	for(unsigned int i=0; i<str.size() && i < 15; i++)
	{
		buffer[pos+i] = strBuf[i];
	}
	pos += 15;
}//end of encodeString120
/*=================================================================
	ROUTINE: copyBuffer
=================================================================== */
void ACS_LM_JTP_Conversation::copyBuffer(unsigned char* destBuf, unsigned int destPos, const unsigned char* srcBuf, unsigned int srcPos, unsigned int srcLen)
{
    int destIndex =0;
    for(unsigned int i=srcPos; i<srcPos+srcLen; i++)
    {
	     destBuf[destPos+destIndex] = srcBuf[i];
	     destIndex++;
    }
}//end of copyBuffer
/*=================================================================
	ROUTINE: encode
=================================================================== */
//std::list<ACS_LM_JTP_Conversation::Buffer32*> ACS_LM_JTP_Conversation::encode(const std::list<LkData*>& lkDataList, int lmMode)
std::list<ACS_LM_JTP_Conversation::Buffer32*> ACS_LM_JTP_Conversation::encode(const std::list<LkData*>& lkDataList, int lmMode, bool isCleanupdata)//LmCleanUp
{
	std::list<ACS_LM_JTP_Conversation::Buffer32*> bufferList;
	
	// Encode the first buffer
   ACS_LM_JTP_Conversation::Buffer32* buffer32  = new ACS_LM_JTP_Conversation::Buffer32();
	if(lkDataList.size() > MAX_LKS_ALLOWED)
	{
		//encodeHeader(buffer32->buffer, buffer32->size, MAX_LKS_ALLOWED, lmMode, false);
		encodeHeader(buffer32->buffer, buffer32->size, MAX_LKS_ALLOWED, lmMode, false, isCleanupdata);//LmCleanUp

	}
	else
	{
		//encodeHeader(buffer32->buffer, buffer32->size, (unsigned int)lkDataList.size(), lmMode, true);
		encodeHeader(buffer32->buffer, buffer32->size, (unsigned int)lkDataList.size(), lmMode, true, isCleanupdata);//LmCleanUp

	}

    int lkCount=0;
    for(std::list<LkData*>::const_iterator it=lkDataList.begin();
		it != lkDataList.end();  ++it)
	{
        if(lkCount > MAX_LKS_ALLOWED)
		{
			// Encode the next buffer
			bufferList.push_back(buffer32);
            buffer32  = new ACS_LM_JTP_Conversation::Buffer32();
            lkCount = 0;
            
			if((lkDataList.size()-lkCount) > MAX_LKS_ALLOWED)
			{
				//encodeHeader(buffer32->buffer, buffer32->size, MAX_LKS_ALLOWED, lmMode, false);
				encodeHeader(buffer32->buffer, buffer32->size, MAX_LKS_ALLOWED, lmMode, false, isCleanupdata);//LmCleanUp
			}
			else
			{
				//encodeHeader(buffer32->buffer, buffer32->size, (unsigned int)lkDataList.size(), lmMode, true);
				 encodeHeader(buffer32->buffer, buffer32->size, (unsigned int)lkDataList.size(), lmMode, true, isCleanupdata);//LmCleanUp
			}
		}

		LkData* lkData = (*it);

		//Encode SetName
		encode8(buffer32->buffer, buffer32->size, (unsigned int)lkData->setName.size());
		encodeString120(buffer32->buffer, buffer32->size, lkData->setName);

		//Encode ParamName
		encode8(buffer32->buffer, buffer32->size, (unsigned int)lkData->paramName.size());
		encodeString120(buffer32->buffer, buffer32->size, lkData->paramName);

		//Encode Value
		encode16(buffer32->buffer, buffer32->size, lkData->value);

		//Encode Status
		encode16(buffer32->buffer, buffer32->size, lkData->status);

        lkCount++;
	}
	bufferList.push_back(buffer32);

    return bufferList;
}//end of encode

/*=================================================================
	ROUTINE: encodeHeader
=================================================================== */
void ACS_LM_JTP_Conversation::encodeHeader(unsigned char* buffer, unsigned int& pos, int lkNumber, int lmMode, bool isLastLk, bool cleanUpBuffer)//LmcleanUp
{
	//Encode LK NUMBER
    encode16(buffer, pos, lkNumber);
    
	//Encode Last LK status
	if(isLastLk)
	{
		encode16(buffer, pos, 0);
	}
	else
	{
		encode16(buffer, pos, 1);
	}

	//Encode LM Mode
	if(!cleanUpBuffer)
	{
    encode16(buffer, pos, lmMode);
	}
	else
	{
		encode16(buffer, pos, 0);
	}

	//SPARE
    encode16(buffer, pos, 0);
    //SPARE
    encode16(buffer, pos, 0);
}//end of encodeHeader

void ACS_LM_JTP_Conversation::setLMINSTFlag(bool myFlag)
{
	theLMInstCommandFlag = myFlag;
}

void ACS_LM_JTP_Conversation::setStopSignal(bool aStopFlag)
{
 aStopFlag = true;
}

bool ACS_LM_JTP_Conversation::getStopSignal()
{
 return false;
}

/*===============================================================================
        ROUTINE: getProtocolFromCP
        Description: This method is dummy. 
=============================================================================== */
unsigned short ACS_LM_JTP_Conversation::getProtocolFromCP(const std::string& cpService)
{
	//dummy method.
}
