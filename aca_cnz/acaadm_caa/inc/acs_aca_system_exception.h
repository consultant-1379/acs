/*=================================================================== */
/**
        @file           acs_aca_system_exception.h

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the SystemException class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       09/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef __SYSTEM_EXCEPTION_H__
#define __SYSTEM_EXCEPTION_H__
/*=====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aca_exception.h"

using Ericsson::System::Exception;

namespace Ericsson { namespace System {
	/*=====================================================================
																	CLASS DECLARATION SECTION
	==================================================================== */
	class SystemException : public Exception {
	/*=====================================================================
																	PUBLIC DECLARATION SECTION
	==================================================================== */
	public:
	/*=====================================================================
																	CLASS CONSTRUCTOR
	==================================================================== */
	/*===================================================================

		 @brief      SystemException

		 @param      message

		 @param      innerException

	=================================================================== */
		inline explicit SystemException(
				const std::string & message = "A system exception occurred!",
				const Exception * innerException = 0)
		: Exception(message, innerException) {}

	public:
	/*=====================================================================
																	CLASS DESTRUCTOR
	==================================================================== */
		inline virtual ~SystemException() {}
	};
} }
// namespace Ericsson::System

#endif //__SYSTEM_EXCEPTION_H__
