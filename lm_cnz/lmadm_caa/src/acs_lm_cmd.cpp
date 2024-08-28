//******************************************************************************
//
//  NAME
//     acs_lm_cmd.cpp
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
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_lm_cmd.h"
#include "acs_lm_tra.h"
#include <cstdlib>
/*=================================================================
	ROUTINE: ACS_LM_Cmd constructor
=================================================================== */
ACS_LM_Cmd::ACS_LM_Cmd()
:cmdCode(ACS_LM_RC_SERVERNOTRESPONDING),
cmdSize(0)
{
	DEBUG("ACS_LM_Cmd::ACS_LM_Cmd()  %s","Entering");
	DEBUG("ACS_LM_Cmd::ACS_LM_Cmd()  %s","Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_Cmd constructor
=================================================================== */
ACS_LM_Cmd::ACS_LM_Cmd(ACE_INT16 cmdCode, ACE_INT16 argc, ACE_TCHAR ** argv)
:cmdCode(cmdCode),
cmdSize(0)
{
	DEBUG("ACS_LM_Cmd::ACS_LM_Cmd()  %s","Parameter Constructor Entering");

	cmdSize = 6;
	for(int i=0; i<argc; i++)
	{
		std::string arg = argv[i];
		//DEBUG(" arg = %s",arg );
		cmdSize += 2;
		cmdSize += (ACE_INT16)arg.size();
		cmdArgs.push_back(arg);
		//DEBUG(" cmdSize = %d",cmdSize);

	}
	DEBUG("ACS_LM_Cmd::ACS_LM_Cmd()  %s","Parameter Constructor Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_Cmd destructor
=================================================================== */
ACS_LM_Cmd::~ACS_LM_Cmd()
{
	DEBUG("ACS_LM_Cmd::~ACS_LM_Cmd()  %s","Entering");
	DEBUG("ACS_LM_Cmd::~ACS_LM_Cmd()  %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: fromBytes
=================================================================== */
bool ACS_LM_Cmd::fromBytes(ACE_TCHAR * data)
{
	DEBUG("ACS_LM_Cmd::fromBytes()  %s","Entering");
	bool bytesConverted = true;
	ACE_TCHAR** argv = NULL;
	int argc = 0;
	try
	{
		int pos = 0;
		cmdSize = decode16(data, pos);
		pos += 2;
		cmdCode = decode16(data, pos);
		pos += 2;
		argc = decode16(data, pos);
		if(argc > 0)
		{
			pos += 2;
			argv = new ACE_TCHAR*[argc];
			for(int i=0; i<argc; i++)
			{
				int argLen = decode16(data, pos);
				argv[i] = new ACE_TCHAR[argLen + 1];
				pos += 2;
				copyBuffer(argv[i], 0, data, pos, argLen);
				argv[i][argLen] = '\0';
				pos += argLen;
			}
			
			cmdSize = 6;
			for(int i=0; i<argc; i++)
			{
				std::string arg = (char*)argv[i];
				cmdSize += 2;
				cmdSize += (int)arg.size();
				cmdArgs.push_back(arg);
			}
		}
	}
	catch(...)
	{
		DEBUG("ACS_LM_Cmd::fromBytes()  %s","Catch Block");
		bytesConverted = false;
	}

	for(int i=0; i<argc; i++)
	{
		delete[] argv[i];
		argv[i] = NULL;
	}
	delete[] argv;
	argv = NULL;
	DEBUG("ACS_LM_Cmd::fromBytes()  %s","Leaving");
 	return bytesConverted;
}//end of fromBytes
/*=================================================================
	ROUTINE: toBytes
=================================================================== */
ACE_TCHAR* ACS_LM_Cmd::toBytes() const
{
	DEBUG("ACS_LM_Cmd::toBytes()  %s","Entering");
	ACE_TCHAR * encodedBuf = new ACE_TCHAR[cmdSize];
	ACE_INT16 pos = 0;
	//Encode data length
	encode16(encodedBuf, pos, cmdSize);
	//Encode cmd type
	pos += 2;
	encode16(encodedBuf, pos, cmdCode);
	//Encode cmd args
	pos += 2;
	encode16(encodedBuf, pos, (ACE_INT16)cmdArgs.size());
	pos += 2;
	for(std::list<std::string>::const_iterator it = cmdArgs.begin();
		it != cmdArgs.end(); ++it)
	{
		std::string arg = (*it);
		encode16(encodedBuf, pos, (ACE_INT16)arg.size());
    	pos += 2;
		copyBuffer(encodedBuf, pos, (const ACE_TCHAR*)arg.c_str(), 0, (ACE_INT16)arg.size());
		//Encode cmd args
		pos += (ACE_INT16)arg.size();
	}
	DEBUG("ACS_LM_Cmd::toBytes()  %s","Leaving");
	return encodedBuf;
}//end of toBytes
/*=================================================================
	ROUTINE: encode16
=================================================================== */

void ACS_LM_Cmd::encode16(ACE_TCHAR* destBuf, const ACE_UINT16 destPos, const ACE_UINT16 value)
{
	DEBUG("ACS_LM_Cmd::encode16()  %s","Entering");
	destBuf[destPos] = (0xFF & value);
	destBuf[destPos+1] = ((0xFF00 & value) >> 8);
	DEBUG("ACS_LM_Cmd::encode16()  %s","Leaving");
}//end of encode16
/*=================================================================
	ROUTINE: copyBuffer
=================================================================== */
void ACS_LM_Cmd::copyBuffer(ACE_TCHAR* destBuf, const ACE_UINT16 destPos, const ACE_TCHAR * srcBuf, const ACE_UINT16 srcPos, const ACE_UINT16 srcLen)
{
	DEBUG("ACS_LM_Cmd::copyBuffer()  %s","Entering");
	int destIndex =0;
    for(int i=srcPos; i<srcPos+srcLen; i++)
    {
	     destBuf[destPos+destIndex] = srcBuf[i];
	     destIndex++;
    }
    DEBUG("ACS_LM_Cmd::copyBuffer()  %s","Leaving");
}//end of copyBuffer
/*=================================================================
	ROUTINE: decode16
=================================================================== */
ACE_INT32 ACS_LM_Cmd::decode16(ACE_TCHAR* srcBuf, const ACE_UINT32 srcPos)
{
	DEBUG("ACS_LM_Cmd::decode16()  %s","Entering");
	ACE_INT32 decodedValue = (srcBuf[srcPos] & 0x000000FF);
	decodedValue = decodedValue + ((srcBuf[srcPos+1] & 0x000000FF) <<8);
	DEBUG("ACS_LM_Cmd::decode16()  %s","Leaving");
	return decodedValue;
}//end of decode16
