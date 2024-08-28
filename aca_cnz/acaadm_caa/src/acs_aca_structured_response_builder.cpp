//******************************************************************************
//
//  NAME
//     acs_aca_structured_response_builder.cpp
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

#include "acs_aca_structured_response_builder.h"

//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
ACS_ACA_StructuredResponseBuilder::ACS_ACA_StructuredResponseBuilder(void)
{
	msg = new StructuredResponse();
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_StructuredResponseBuilder::~ACS_ACA_StructuredResponseBuilder(void)
{
	delete(msg);
}

//------------------------------------------------------------------------------
// build msdls response:
//		Set the Error code in the response
//------------------------------------------------------------------------------
void ACS_ACA_StructuredResponseBuilder::setError(unsigned short error)
{
	msg->setError(error);
}

//------------------------------------------------------------------------------
// build msdls response:
//		Set the data length in the response
//------------------------------------------------------------------------------
void ACS_ACA_StructuredResponseBuilder::setDataLength(unsigned int data_length)
{
	msg->setDataLength(data_length);
}

//------------------------------------------------------------------------------
// build msdls response:
//		Set the data content in the response
//------------------------------------------------------------------------------
void ACS_ACA_StructuredResponseBuilder::setData(const void* data)
{
	msg->setData(data);
}

//------------------------------------------------------------------------------
//		Set response message size
//------------------------------------------------------------------------------
void ACS_ACA_StructuredResponseBuilder::setSize(unsigned int &size)
{
	size_t hdr_len = sizeof(aca_msdls_resp_t);
	size_t data_len = msg->getDataLength();
	if (data_len > 0)
	{
		size = hdr_len + data_len - 1;
	}
	else
	{
		size = hdr_len;
	}
}

//------------------------------------------------------------------------------
//		Get request built
//------------------------------------------------------------------------------
void* ACS_ACA_StructuredResponseBuilder::getMessage()
{
	void* req = NULL;
	if (msg != NULL)
	{
		req = msg->getData();
	}
	return req;
}

void ACS_ACA_StructuredResponseBuilder::setCpId(unsigned short cpID)
{
	msg->setCpId(cpID);
}

void ACS_ACA_StructuredResponseBuilder::setSeqNo(unsigned int seq_no)
{
	msg->setSeqNo(seq_no);
}
