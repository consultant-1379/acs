/*=================================================================== */
   /**
   @file acs_chb_error.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_aeh.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011   XTANAGG   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_chb_error.h>
using namespace std;

/*===================================================================
                        DEFINE DECLARATION SECTION
=================================================================== */
/*===================================================================
   ROUTINE: 
=================================================================== */

void ACS_CHB_Error::setErrorText(const char errorMessage[] )
{

	 ACE_OS::snprintf(_last_error_text, ACS_CHB_MAX_LAST_ERROR_TEXT, "%s", errorMessage);

}

void ACS_CHB_Error::setError( const int errorNo )
{
	_last_error = errorNo;
}


int ACS_CHB_Error::getError() const
{

	return _last_error;
}

const char* ACS_CHB_Error::getErrorText() const
{
	return _last_error_text;
}

