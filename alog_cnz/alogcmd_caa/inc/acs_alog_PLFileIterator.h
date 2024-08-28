//****************************************************************************
//
//  NAME
//     ACS_ALOG_PLFileIterator.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2004. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  PR           DATE      INITIALS    DESCRIPTION
//  -----------------------------------------------------------
//  N/A       05/08/2012     xbhakat       Initial Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************

#ifndef ACS_ALOG_PLFILEITERATOR_H
#define ACS_ALOG_PLFILEITERATOR_H

#include <acs_alog_fileDef.h>


class ACS_ALOG_PLFileIt;

class ACS_ALOG_PLFileIterator
{
	public:
		ACS_ALOG_PLFileIterator(const char* lpszFilename);
		~ACS_ALOG_PLFileIterator();

		operator bool();
		void operator++();
		ACS_ALOG_Exclude* operator()();
		int GetRetCode();

	private:
		ACS_ALOG_PLFileIt* m_pPLFileIt;
};

#endif
