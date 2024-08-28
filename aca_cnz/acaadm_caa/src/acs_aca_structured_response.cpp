//******************************************************************************
//
//  NAME
//     acs_aca_structured_response.cpp
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
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2012-10-15 by XLANSRI
//
//  SEE ALSO
//     -
//
//******************************************************************************

#include "acs_aca_structured_response.h"

//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
StructuredResponse::StructuredResponse(void)
{
	msdls_resp = (aca_msdls_resp_t*)malloc(sizeof(aca_msdls_resp_t));
	memset(msdls_resp, 0, sizeof(aca_msdls_resp_t));
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
StructuredResponse::~StructuredResponse(void)
{
	free(msdls_resp);
	msdls_resp = 0;
}

//------------------------------------------------------------------------------
//      Get response structure
//------------------------------------------------------------------------------
void* StructuredResponse::getData()
{
	return (void*)msdls_resp;
}

//------------------------------------------------------------------------------
//      Get Data Length
//------------------------------------------------------------------------------
unsigned int StructuredResponse::getDataLength()
{
	return (msdls_resp) ? (msdls_resp->data_lenght) : 0;
}

//------------------------------------------------------------------------------
//      setError
//------------------------------------------------------------------------------
void StructuredResponse::setError(unsigned short error)
{
	msdls_resp->error = error;
}

//------------------------------------------------------------------------------
//      setDataLength
//------------------------------------------------------------------------------

void StructuredResponse::setDataLength(unsigned int data_length)
{
	if (data_length > 0)
	{
		unsigned short tmp_err = msdls_resp->error;

		size_t len = sizeof(aca_msdls_resp_t);
		len = len + data_length;

		free (msdls_resp);
		msdls_resp = NULL;

		msdls_resp = (aca_msdls_resp_t*)malloc(len);
		memset(msdls_resp, 0, len);

		msdls_resp->error = tmp_err;
		msdls_resp->data_lenght = data_length;
	}
}

//------------------------------------------------------------------------------
//      setData
//------------------------------------------------------------------------------

void StructuredResponse::setData(const void* data)
{
	if ((data != NULL) && (msdls_resp->data_lenght > 0))
	{
		memcpy((char*)msdls_resp->data, data, msdls_resp->data_lenght);
	}
}

//------------------------------------------------------------------------------
//      setCpId 
//------------------------------------------------------------------------------

void StructuredResponse::setCpId(unsigned short cpID)
{
	msdls_resp->cpID = cpID;
}

//------------------------------------------------------------------------------
//      setSeqNo
//------------------------------------------------------------------------------

void StructuredResponse::setSeqNo(unsigned int seq_no)
{
	msdls_resp->seqNo = seq_no;
}
