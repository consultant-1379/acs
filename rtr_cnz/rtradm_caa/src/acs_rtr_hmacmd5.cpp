//******************************************************************************
//
// NAME
// acs_rtr_hmacmd5.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2005.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.

// AUTHOR 
// 2012-12-17 by XLANSRI

// DESCRIPTION
// This file implements the HMAC-MD5 algorithm, i.e
//   MD5(secretKey XOR opad),MD5(secretKey XOR ipad,data))
// 
// This means:
// (1) append 0x00 to the secret key (16 bytes) to create a 
//     64 byte secret key.
// (2) XOR the secret key (1) with ipad.
// (3) append the data to the result in (2).
// (4) get the hash of the result in (3).
// (5) XOR the secret key (1) with opad.
// (6) append the result in (4) to the result in (5).
// (7) get the hash of the result in (6).
//******************************************************************************

#include "acs_rtr_hmacmd5.h"
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "acs_rtr_tracer.h"
using namespace std;

ACS_RTR_TRACE_DEFINE(ACS_RTR_HMACMD5)

//*************************************************************************
// Constructor
//*************************************************************************

ACS_RTR_HMACMD5::ACS_RTR_HMACMD5(unsigned char* ipad, unsigned char* opad, unsigned char* key)
{
	ACS_RTR_TRACE_FUNCTION;
	unsigned char paddedKey[64];
	memset(paddedKey, 0x00, 64);
	memcpy(paddedKey, key, 16);
	memset(hashInner, '\0', sizeof(hashInner));

	for (int i=0;i<64;i++)
	{
		k_ipad[i] = paddedKey[i] ^ipad[i];
		k_opad[i] = paddedKey[i] ^opad[i];
	}

	int res = MD5_Init(&md5);

	if(res == 1)
	{
		state = true;
		state = addData(64,k_ipad);
	}
	else
	{
		state = false;
		err = "MD5_Init failed";
	}
}

//*************************************************************************
// Destructor
//*************************************************************************
ACS_RTR_HMACMD5::~ACS_RTR_HMACMD5()
{
	ACS_RTR_TRACE_FUNCTION;
}

//*************************************************************************
// addData
//
// Adds data to the hash object.
//
// Return values:
// true : ok
// false: something failed
//*************************************************************************
bool ACS_RTR_HMACMD5::addData(size_t ln, unsigned char* data)
{
	ACS_RTR_TRACE_FUNCTION;
	int res = MD5_Update(&md5, data, ln);
	if(res == 1)
	{
		state = true;	
	}
	else
	{
		state = false;
		err = "MD5_Update failed";
	}
	return state;
}

//*************************************************************************
// endData
//
// Implements steps (4)-(6).
//
// Return values:
// true : ok
// false: something failed
//*************************************************************************
bool ACS_RTR_HMACMD5::endData()
{
	ACS_RTR_TRACE_FUNCTION;

	int res = MD5_Final(hashInner, &md5);
	if (res)
	{
		res = MD5_Init(&md5);
		if(res == 1)
		{
			unsigned char tot[80];
			memcpy(tot,k_opad,64);
			memcpy(&tot[64],hashInner,16);
			state = addData(80,tot);
		}
		else
		{
			state = false;
			err = "MD5_Init failed";
		}
	}
	else
	{
		state = false;
		err = "MD5_Final failed";
	}

	return state;
}


//*************************************************************************
// getHash
//
// Get the hash value.
//
// Return values:
// true : ok
// false: something failed
//*************************************************************************
bool ACS_RTR_HMACMD5::getHash(unsigned char* hash)
{
	ACS_RTR_TRACE_FUNCTION;
	int res = MD5_Final(hashInner, &md5);
	if (res == 1)
	{
		memcpy(hash, hashInner, 16); 
		state = true;
	}
	else
	{
		state = false;
		err = " MD5_Final Failed in getHash";
	}			
	return state;
}

//*************************************************************************
// getState
//
// Get the state of the object.
//
// Return values:
// -
//*************************************************************************
bool ACS_RTR_HMACMD5::getState(void)
{
	ACS_RTR_TRACE_FUNCTION;
	return state;
}
//*************************************************************************
// getError
//
// Get the error string.
//
// Return values:
// -
//*************************************************************************
void ACS_RTR_HMACMD5::getError(string& errString)
{ 
	ACS_RTR_TRACE_FUNCTION;
	errString = err;
}
