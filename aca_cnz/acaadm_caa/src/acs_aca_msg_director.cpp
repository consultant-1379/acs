//******************************************************************************
//
//  NAME
//     acs_aca_msg_director.cpp
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

#include "acs_aca_msg_director.h"

//------------------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------------------
ACS_ACA_MSGDirector::ACS_ACA_MSGDirector () {
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_MSGDirector::~ACS_ACA_MSGDirector () {
}

//------------------------------------------------------------------------------
//      Build a structured Request
//------------------------------------------------------------------------------
void * ACS_ACA_MSGDirector::buildStructuredRequest (
		ACS_ACA_MSGBuilder * builder,
		unsigned int & reqSize,
		unsigned short optmask,
		const char * cpsource,
		const char * msname,
		const char * data,
		unsigned short cpID) {
	/* todo: parameters check */

	/* build the message */
	builder->setOptions(optmask);
	builder->setCPSource(cpsource);
	builder->setMSGStoreName(msname);
	builder->setData(data);
	builder->setSize(reqSize);
	builder->setCpId(cpID);

	return builder->getMessage();
}

//------------------------------------------------------------------------------
//      Build a structured Response
//------------------------------------------------------------------------------
void * ACS_ACA_MSGDirector::buildStructuredResponse (
		ACS_ACA_MSGBuilder * builder,
		unsigned int & respSize,
		unsigned int seq_no,
		unsigned short error,
		unsigned int data_length,
		const char * data,
		unsigned short cpID) {
	/* todo: parameters check */

	/* build the message */
	builder->setError(error);
	builder->setSeqNo(seq_no);
	builder->setDataLength(data_length);
	builder->setData(data);
	builder->setSize(respSize);
	builder->setCpId(cpID);

	return builder->getMessage();
}
