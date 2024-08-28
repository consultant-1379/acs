//********************************************************************************
//
// NAME
// ACS_RTR_ErrorSupportBase.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2007.
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
// 2007-05-10 by GP/AP/A QNICMUT
//
// DESCRIPTION 
// This class implements a base class used to propagate the error information.
//
//********************************************************************************
#include "acs_rtr_errorsupportbase.h"

#include <new>

namespace {
	//----------
	// Constants
	//----------
	const char * const _DESB_NO_ERROR = "<NO ERRORS>";
	const char * const _DESB_NO_DESCRIPTION = "<NO DESCRIPTION>";
	const char * const _DESB_ERROR_BAD_ALLOC = "The system rejected a new allocation request for the internal error description buffer.";
}

//-------------
// setLastError
//-------------
bool ACS_RTR_ErrorSupportBase::setLastError(int error, const char * description, bool allocate) {
	if (description) {
		if (allocate) {
			size_t requestedSize = strlen(description) + 1;
			
			if (requestedSize > _bufferSize) {
				char * p = new char[requestedSize];
				if (!p) return false;
				delete[] _buffer;
				_bufferSize = requestedSize;
				_buffer = p;
			}

			_lastErrorDescription = strncpy(_buffer, description, _bufferSize);
		} else _lastErrorDescription = description;
	} else _lastErrorDescription = (error == ESB_NO_ERROR ? _DESB_NO_ERROR : _DESB_NO_DESCRIPTION);

	_lastError = error;
	return true;
}

//-----------
// operator =
//-----------
ACS_RTR_ErrorSupportBase & ACS_RTR_ErrorSupportBase::operator=(const ACS_RTR_ErrorSupportBase & rhs) throw() {
	if ((this != &rhs) && !setLastError(rhs._lastError, rhs._lastErrorDescription, rhs._lastErrorDescription == rhs._buffer))
		throw std::bad_alloc();

	return *this;
}
