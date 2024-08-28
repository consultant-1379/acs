//======================================================================
//
// NAME
//      Tracer.h
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      Implementation of Tracing of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Tracer_h
#define AcsHcs_Tracer_h

#include <iostream>
#include <sstream>
#include <string>

#include "ACS_TRA_trace.h"

/**
* Initialises tracing.
* Instantiates a global (on file-level) ACS_TRA_trace-object and
* initialises it with the class name derived from the file name.
* This macro must be called before macro ACS_HCS_TRACE can be used.
* This macro must be called once per .cpp-file.
* Preferably call this macro within namespace AcsHcs.
*/
#define ACS_HCS_TRACE_INIT \
	static std::string TracerFileName  = std::string(__FILE__).substr(std::string(__FILE__).find_last_of("\\/") + 1);\
	static std::string TracerClassName = TracerFileName.substr(0, TracerFileName.find_last_of("."));\
	static ACS_TRA_trace TracerTracer  = ACS_TRA_DEF(TracerClassName.c_str(), "C400");

/**
* Traces message msg.
* Macro ACS_TRACE_INIT must have been called once before.
* @param[in] msg The message to be logged in the trace.
*/
#define ACS_HCS_TRACE(msg) { if (ACS_TRA_ON(TracerTracer)) { std::stringstream s; s << TracerFileName << "(" << __LINE__ << "): " << msg << endl; ACS_TRA_event(&TracerTracer, s.str().c_str()); } }

#endif // AcsHcs_Tracer_h