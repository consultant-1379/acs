/*=================================================================== */
/**
        @file          acs_aca_exception.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the MCS_ALD_Display class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       07/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__
/*====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
//#include "System\System.h"

namespace Ericsson { namespace System {
//#pragma pack(push)
//#pragma pack(2)
/*=====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
	class Exception {
/*====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
	public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for Exception class.

        @param          message

        @param          innerException

=================================================================== */
		inline explicit Exception(const std::string & message = "An exception occurred!", const Exception * innerException = 0)
			: _source(), _message(message), _pInnerException(innerException) {}

	public:
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
		inline virtual ~Exception() {}
	public:
/*===================================================================

        @brief          message

        @return         _message
=================================================================== */
		inline virtual const std::string & message() const { return _message; }
/*===================================================================

        @brief          innerException

        @return         _pInnerException
=================================================================== */
		inline const Exception * innerException() const { return _pInnerException; }
/*===================================================================

        @brief          source

        @return         _source
=================================================================== */
		inline virtual const std::string & source() const { return _source; }
/*====================================================================
                                PROTECTED DECLARATION SECTION
==================================================================== */
	protected:
/*====================================================================
                                PROTECTED DATA MEMBER
==================================================================== */
/*===================================================================

        @brief          _source

=================================================================== */
		std::string _source;
/*====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */

	private:
/*====================================================================
                                PRIVATE DATA MEMBER
==================================================================== */
/*===================================================================

        @brief          _message

=================================================================== */
		std::string _message;
/*===================================================================

        @brief          _pInnerException

=================================================================== */

		const Exception * _pInnerException;
	};

//#pragma pack(pop)
               }
 			}
//namespace Ericsson::System

#endif //__EXCEPTION_H__
