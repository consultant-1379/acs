//********************************************************************************
//
// NAME
// acs_rtr_gohreporter.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2012
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-17 by XSAMECH
//
// DESCRIPTION 
// This class implements a base class for the communication with GOH.
//
//********************************************************************************
#include "acs_rtr_gohreporter.h"
#include "acs_rtr_tracer.h"
#include <stdexcept>


ACS_RTR_TRACE_DEFINE(ACS_RTR_GOHReporter)

//--------------------
//      Constructor
//--------------------
ACS_RTR_GOHReporter::ACS_RTR_GOHReporter(std::string transferQueue,
		ACS_RTR_OutputQueue * queue,
		RTR_Events * eventHandler,
		RTR_statistics * statptr)
: _transferQueue(transferQueue),
  _outputQueue(queue),
  _eventHandler(eventHandler),
  _statptr(statptr),
  _state(IDLE),
  _gohStopRequested(false),
  m_jobRemoved(ACE_INVALID_HANDLE ),
  m_stopEvent(ACE_INVALID_HANDLE ),
  m_sendBlockError(0),
  m_attachFileTQerror(0),
  m_attachBlockTQerror(0),
  m_sendFilesError(0)
{
	// Parameter check
	if(NULL == _outputQueue )
	{
		ACS_RTR_TRACE_MESSAGE("Error: Invalid output queue reference");
		throw std::invalid_argument("ERROR: Invalid output queue reference");
	} 

	if(NULL == _statptr)
	{
		ACS_RTR_TRACE_MESSAGE("Error: Invalid ststistics file reference");
		throw std::invalid_argument("ERROR: Invalid ststistics file reference");
	}

}
