//  NAME
//     acs_emf_cmd.cpp
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
#include "acs_emf_cmd.h"
#include <acs_emf_tra.h>
#include <cstdlib>
/*=================================================================
	ROUTINE: ACS_EMF_CMD constructor
=================================================================== */
ACS_EMF_Cmd::ACS_EMF_Cmd()
:cmdCode(117),
cmdSize(0)
{
//	DEBUG(1, "s", "ACS_EMF_CMD::ACS_EMF_CMD()  %s","Entering");
//	DEBUG(1, "s", "ACS_EMF_CMD::ACS_EMF_CMD()  %s","Leaving");
	cmdArgs.clear();
}//end of constructor
/*=================================================================
	ROUTINE: ACS_EMF_CMD constructor
=================================================================== */
ACS_EMF_Cmd::ACS_EMF_Cmd(ACE_INT16 cmdCode)
:cmdCode(cmdCode),
cmdSize(0)
{
//	DEBUG(1, "%s", "ACS_EMF_CMD::ACS_EMF_CMD() Parameter Constructor Entering");
	cmdSize = 6;
	cmdArgs.clear();
//	DEBUG(1, "%s", "ACS_EMF_CMD::ACS_EMF_CMD() Parameter Constructor Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_EMF_CMD destructor
=================================================================== */
ACS_EMF_Cmd::~ACS_EMF_Cmd()
{
//	DEBUG(1, "%s", "ACS_EMF_CMD::~ACS_EMF_CMD() Entering");
//	DEBUG(1, "%s", "ACS_EMF_CMD::~ACS_EMF_CMD() Leaving");
	cmdArgs.clear();
}//end of destructor
/*=================================================================
        ROUTINE:addArguments 
=================================================================== */
void ACS_EMF_Cmd::addArguments(std::list<string> alist)
{
	std::list<string>::iterator itr = alist.begin();
	while(itr != alist.end())
	{
		std::string arg = (*itr);
		cmdSize += 2;
		cmdSize += arg.size();
		cmdArgs.push_back(arg);
		++itr;
		//DEBUG(" cmdSize = %d",cmdSize);

	}
}//end of addArguments
/*=================================================================
	ROUTINE: fromBytes
=================================================================== */
bool ACS_EMF_Cmd::fromBytes(ACE_TCHAR * data)
{
//	DEBUG(1, "%s", "ACS_EMF_CMD::fromBytes() Entering");
	bool bytesConverted = true;
	ACE_TCHAR** argv = NULL;
	int argc = 0;
	cmdArgs.clear();
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
		DEBUG(1, "%s", "ACS_EMF_CMD::fromBytes() - Catch Block");
		bytesConverted = false;
	}

	for(int i=0; i<argc; i++)
	{
		delete[] argv[i];
		argv[i] = NULL;
	}
	delete[] argv;
	argv = NULL;
//	DEBUG(1, "%s", "ACS_EMF_CMD::fromBytes() Leaving");
 	return bytesConverted;
}//end of fromBytes
/*=================================================================
	ROUTINE: toBytes
=================================================================== */
ACE_TCHAR* ACS_EMF_Cmd::toBytes() const
{
//	DEBUG(1, "%s", "ACS_EMF_CMD::toBytes() Entering");
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
//	DEBUG(1, "%s", "ACS_EMF_CMD::toBytes()  %s","Leaving");
	return encodedBuf;
}//end of toBytes
/*=================================================================
	ROUTINE: encode16
=================================================================== */

void ACS_EMF_Cmd::encode16(ACE_TCHAR* destBuf, const ACE_UINT16 destPos, const ACE_UINT16 value)
{
//	DEBUG(1, "%s", "ACS_EMF_CMD::encode16()  Entering");
	destBuf[destPos] = (0xFF & value);
	destBuf[destPos+1] = ((0xFF00 & value) >> 8);
//	DEBUG(1, "%s", "ACS_EMF_CMD::encode16()  Leaving");
}//end of encode16
/*=================================================================
	ROUTINE: copyBuffer
=================================================================== */
void ACS_EMF_Cmd::copyBuffer(ACE_TCHAR* destBuf, const ACE_UINT16 destPos, const ACE_TCHAR * srcBuf, const ACE_UINT16 srcPos, const ACE_UINT16 srcLen)
{
//	DEBUG(1, "%s", "ACS_EMF_CMD::copyBuffer()  Entering");
	int destIndex =0;
    for(int i=srcPos; i<srcPos+srcLen; i++)
    {
	     destBuf[destPos+destIndex] = srcBuf[i];
	     destIndex++;
    }
//    DEBUG(1, "%s", "ACS_EMF_CMD::copyBuffer()  Leaving");
}//end of copyBuffer
/*=================================================================
	ROUTINE: decode16
=================================================================== */
ACE_INT32 ACS_EMF_Cmd::decode16(ACE_TCHAR* srcBuf, const ACE_UINT32 srcPos)
{
//	DEBUG(1, "%s", "ACS_EMF_CMD::decode16()  Entering");
	ACE_INT32 decodedValue = (srcBuf[srcPos] & 0x000000FF);
	decodedValue = decodedValue + ((srcBuf[srcPos+1] & 0x000000FF) <<8);
//	DEBUG(1, "%s", "ACS_EMF_CMD::decode16()  Leaving");
	return decodedValue;
}//end of decode16

