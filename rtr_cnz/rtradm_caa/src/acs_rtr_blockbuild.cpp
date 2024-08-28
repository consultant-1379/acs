//******************************************************************************
//
// NAME
// acs_rtr_blockBuild.cpp
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
// 2003-01-20 by EAB/UKY/GD UABCAJN
//
// DESCRIPTION
// This file implements everything concerning blocks such as
// storing data in blocks and handling its block number.
//
// CHANGES
// RELEASE REVISION HISTORY
// DATE		NAME			DESCRIPTION
// 2012-12-12	XSAMECH			First Release
//******************************************************************************

#include <acs_rtr_blockbuild.h>
#include <acs_rtr_global.h>
#include <acs_rtr_events.h>
#include "acs_rtr_defaultvalues.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"
#include <iostream>
#include <exception>
#include <stdexcept>
using namespace std;


RTRblock::RTRblock(unsigned int recsize, const RTRMS_Parameters* rtrParams)
: parBlockLen(0),
  holdTime(0),
  blockNumber(0),
  blockLn(0),
  noOfMessages(0),
  accSizeInd(0),
  lastBlockInTransaction(false)
{

	if(NULL != rtrParams )
	{
		if(rtrParams->BlockLengthType == FIXED)
		{
			blockLengthType = LENGTH_FIXED;
		}
		else if (rtrParams->BlockLengthType == EVEN)
		{
			blockLengthType = LENGTH_EVEN;
		}
		else
		{
			blockLengthType = LENGTH_VARIABLE;
		}

		holdTime = rtrParams->BlockHoldTime;
		parBlockLen = rtrParams->BlockLength;

		if (parBlockLen < MIN_BLOCKLENGTH)
		{
			parBlockLen = DEF_BLOCKLENGTH;
		}

		blockBuf = new (std::nothrow) char[parBlockLen];

		if(NULL == blockBuf)
		{
			throw std::runtime_error( "runtime error" );
		}

		// Prepare buffer by filling it with the padding character.
		// It doesn't matter if padding should be used or not.
		memset(blockBuf,rtrParams->BlPaddingChar,rtrParams->BlockLength);
	}
	else
	{
		blockLengthType = LENGTH_FIXED;

		holdTime = DEF_BLOCKHOLDTIME;
		parBlockLen = DEF_BLOCKLENGTH;

		blockBuf = new (std::nothrow) char[parBlockLen];
		if(NULL == blockBuf)
		{
			throw runtime_error( "runtime error" );
		}

		// Prepare buffer by filling it with the padding character.
		// It doesn't matter if padding should be used or not.
		memset(blockBuf, DEF_BLPADDINGCHAR, DEF_BLOCKLENGTH);
	}

	if (recsize > (ACE_UINT32)parBlockLen) blockSpaceLeft = 0;
	blockSpaceLeft = parBlockLen - recsize;
}


//*************************************************************************
// destructor
//*************************************************************************
RTRblock::~RTRblock()
{
	delete [] blockBuf;
}



//*************************************************************************
// store

// Add data to the block buffer.

// return values:
//   -
//*************************************************************************
void RTRblock::store(const ACE_UINT32 ln, const unsigned char* buf)
{
	if (ln > (parBlockLen - blockLn))
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "CANNOT STORE BLOCK!!! ln: %u, parBlockLen: %d, blockLn: %u", ln, parBlockLen, blockLn);
	}
	else
	{
		memcpy(&blockBuf[blockLn], buf, ln);
		blockLn +=ln;

		if (ln>blockSpaceLeft)
		{
			blockSpaceLeft=0;
		}
		else
		{
			blockSpaceLeft= blockSpaceLeft-ln;
		}

		noOfMessages++;
	}
}

//*************************************************************************
// enoughSpace

// Checks if there is enough space left in the block.

// return values:
//   true : ok
//   false: no, 'bufln' number of bytes would exceed the block size.
//*************************************************************************
bool RTRblock::enoughSpace(const ACE_UINT32 bufln) const
{
	if (bufln>blockSpaceLeft)
	{
		return false;
	}
	return true;
}

//*************************************************************************
// getSendData

// Gets the block data (buffer,length and number).
//
// return values:
//   -
//*************************************************************************
bool RTRblock::getSendData(ACE_TCHAR* buf,ACE_INT32& bufln)
{
	bool result = true;
	// Consider block padding.
	ACE_UINT32 adjustLn=0;

	if (blockLengthType == LENGTH_FIXED)
	{
		adjustLn = parBlockLen - blockLn;	
	}
	else if (blockLengthType == LENGTH_EVEN)
	{
		if ((blockLn%2) !=0)
		{
			// add one padding character to make it even.
			adjustLn =1;
		}
	}

	// else "VARIABLE", i.e as the buffer already looks.
	if ((blockLn + adjustLn) <= (ACE_UINT32)parBlockLen)
	{
		memcpy(buf, blockBuf, (blockLn + adjustLn));
		bufln = blockLn + adjustLn;
	}
	else
	{
		result = false;
	}
	return result;
}

//*************************************************************************
// getBSize

// Gets the block size.
//
// return values:
//   block size in bytes
//*************************************************************************
ACE_UINT32 RTRblock::getBSize()
{
	ACE_UINT32 adjustLn=0;
	if (blockLengthType == LENGTH_FIXED)
	{
		adjustLn = parBlockLen - blockLn;	
	}
	else if (blockLengthType == LENGTH_EVEN)
	{
		if ((blockLn%2) !=0)
		{
			// add one padding character to make it even.
			adjustLn =1;
		}
	}
	return (blockLn + adjustLn);
}

//*************************************************************************
// getBlockNumber

// Gets the block number.
//
// return values:
//   block number
//*************************************************************************
ACE_UINT32 RTRblock::getBlockNumber(void)
{
	return blockNumber;
}

//*************************************************************************
// getNumberOfMessages

// Gets the number of Message store messages in a block.
//
// return values:
//   number of messages
//*************************************************************************
ACE_UINT32 RTRblock::getNumberOfMessages(void)
{
	return noOfMessages;
}


//*************************************************************************
// setBlockNumber

// Defines the current block number.
//
// return values:
//   -
//*************************************************************************
void RTRblock::setBlockNumber(ACE_UINT32 blockNo)
{
	blockNumber = blockNo;
}
